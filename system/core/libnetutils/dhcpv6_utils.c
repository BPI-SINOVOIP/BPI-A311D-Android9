#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netdb.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/netlink.h>
#include <linux/if.h>
#include <linux/route.h>
#include <linux/ipv6_route.h>
#include <cutils/properties.h>
#define LOG_TAG "dhcpv6_utils"
#include <cutils/log.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/icmp6.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <poll.h>
#include <ctype.h>
#include <linux/filter.h>
#include <stddef.h>
#include <unistd.h>

#define NS_MULTI_ADDR "FF02::1:FF00:0"
#define IPV6_DHCPV6     0
#define IPV6_STATELESS  1
const char ipv6_proc_path[] = "/proc/sys/net/ipv6/conf";

static const char DAEMON_NAME[]                 = "dhclient";
static const char DAEMON_PROP_NAME[]            = "init.svc.dhclient";
static const char DAEMON_NAME_RELEASE[]         = "release6";
static const int NAP_TIME = 200;   /* wait for 200ms at a time */
static char errmsg[100];

/*
 * Wait for a system property to be assigned a specified value.
 * If desired_value is NULL, then just wait for the property to
 * be created with any value. maxwait is the maximum amount of
 * time in seconds to wait before giving up.
 */
static int wait_for_property(const char *name, const char *desired_value, int maxwait)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    int maxnaps = (maxwait * 1000) / NAP_TIME;

    if (maxnaps < 1) {
        maxnaps = 1;
    }

    while (maxnaps-- > 0) {
        usleep(NAP_TIME * 1000);
        if (property_get(name, value, NULL)) {
            if (desired_value == NULL ||
                    strcmp(value, desired_value) == 0) {
                return 0;
            }
        }
    }
    return -1; /* failure */
}

static int wait_for_property_with_emergency(const char *name, const char *desired_value, int maxwait,
        const char* daemon, const char* daemon_emerg)
{
    char value[PROPERTY_VALUE_MAX] = {'\0'};
    char value2[PROPERTY_VALUE_MAX] = {'\0'};
    int maxnaps = (maxwait * 1000) / NAP_TIME;

    if (maxnaps < 1) {
        maxnaps = 1;
    }

    while (maxnaps-- > 0) {
        usleep(NAP_TIME * 1000);
        if (property_get(name, value, NULL)) {
            if (desired_value == NULL ||
                    strcmp(value, desired_value) == 0) {
                return 0;
            }
        }else if(property_get(daemon, value2, NULL)){
            if (daemon_emerg != NULL &&
                    strcmp(value2, daemon_emerg) == 0) {
                return -2;//we think that daemon is stopped
            }  
            
        }
    }
    return -1; /* failure */
}

/**
 * Start the DHCPv6 client daemon.
 */
 /*
 *@type: IPV6_DHCPV6 means dhcpv6; IPV6_STATELESS means stateless
 */
int dhcpv6_do_request(const char *interface, int type)
{
    char result_prop_name[PROPERTY_VALUE_MAX];
    char daemon_prop[PROPERTY_VALUE_MAX] = {'\0'};
    char daemon_cmd[PROPERTY_VALUE_MAX];
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};
    const char *ctrl_prop = "ctl.start";
    const char *desired_status = "running";
    char ipaddress6[256] = {'\0'};
    int ret = -1;

    memset(daemon_cmd, '\0', PROPERTY_VALUE_MAX);
    if(type == IPV6_DHCPV6){
        snprintf(daemon_cmd, sizeof(daemon_cmd), "%s_%s", DAEMON_NAME, interface);
    }else if (type == IPV6_STATELESS){
        snprintf(daemon_cmd, sizeof(daemon_cmd), "%s_%s:-S", DAEMON_NAME, interface);
    }else{
        ALOGE("wrong type of request");
        return -1;
    }

    ALOGD("dhcpv6_do_request: %s %s", ctrl_prop, daemon_cmd);
    property_set(ctrl_prop, daemon_cmd);

    memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
    snprintf(result_prop_name, sizeof(result_prop_name), "%s_%s", DAEMON_PROP_NAME, interface);
    snprintf(daemon_prop, sizeof(daemon_prop), "%s", result_prop_name);

    if (wait_for_property(result_prop_name, desired_status, 10) < 0) {
        snprintf(errmsg, sizeof(errmsg), "%s", "Timed out waiting for DHCPv6 to start");
	ALOGE("Timed out waiting for DHCPv6 to start");
        return -1;
    }

    memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
    snprintf(result_prop_name, sizeof(result_prop_name), "%s.%s.result", DAEMON_NAME, interface);
    /* Wait for the daemon to return a result */
    ret = wait_for_property_with_emergency(result_prop_name, NULL, 30, daemon_prop, "stopped");
    if (ret < 0) {
        if(ret == -2){
            snprintf(errmsg, sizeof(errmsg), "%s", "dhcpv6 is killed while running");
            ALOGE("dhcpv6 is killed while running");
        }else{
            snprintf(errmsg, sizeof(errmsg), "%s", "Timed out waiting for DHCPv6 to finished");
            ALOGE("Timed out waiting for DHCPv6 to finished");
        }
        return -1;
    }

    if (!property_get(result_prop_name, prop_value, NULL)) {
        /* shouldn't ever happen, given the success of wait_for_property() */
        snprintf(errmsg, sizeof(errmsg), "%s", "DHCPv6 result property was not set");
	ALOGE("DHCPv6 result property was not set");
        return -1;
    }
    if (strcmp(prop_value, "ok") == 0) {
        snprintf(errmsg, sizeof(errmsg), "%s", "DHCPv6 Success");
    } else {
        snprintf(errmsg, sizeof(errmsg), "%s", "DHCPv6 Fail");
	ALOGE("DHCPv6 Fail");
        return -1;
    }
    ALOGD("dhcpv6_do_request errmsg %s", errmsg);
    return 0;
}

/**
 * Stop the DHCPv6 client daemon.
 */
 /*
 * @type, in fact, it is of no use.
 */
int dhcpv6_stop(const char *interface, int type)
{
    char result_prop_name[PROPERTY_VALUE_MAX];
    char daemon_cmd[PROPERTY_VALUE_MAX];
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};
    const char *ctrl_prop = "ctl.stop";
    const char *desired_status = "stopped";

    memset(daemon_cmd, '\0', PROPERTY_VALUE_MAX);
    snprintf(daemon_cmd, sizeof(daemon_cmd), "%s_%s", DAEMON_NAME, interface);
    property_set(ctrl_prop, daemon_cmd);
    ALOGD("dhcpv6_stop %s, type = %d", daemon_cmd, type);
    /* Stop the daemon and wait until it's reported to be stopped */
    memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
    snprintf(result_prop_name, sizeof(result_prop_name), "%s_%s", DAEMON_PROP_NAME, interface);
    ALOGD("stop dhclient");
    if (wait_for_property(result_prop_name, desired_status, 10) < 0) {
        ALOGE("here is return,error");
        return -1;
    }
    snprintf(prop_value, sizeof(prop_value), "%s.ipaddress.%s", DAEMON_NAME, interface);
    property_set(prop_value, "");
    snprintf(prop_value, sizeof(prop_value), "%s.prefixlen.%s", DAEMON_NAME, interface);
    property_set(prop_value, "");
    snprintf(prop_value, sizeof(prop_value), "%s.%s.result", DAEMON_NAME, interface);
    property_set(prop_value, "");
    return 0;
}

/**
 * Rlease the DHCPv6 ip address.
 */
 /*
 *@type: for IPV6_STATELESS, do nothing. Because ipv6 server do not manage stateless ip. 
 */
int dhcpv6_release_lease(const char *interface, int type)
{
    char result_prop_name[PROPERTY_VALUE_MAX];
    char daemon_cmd[PROPERTY_VALUE_MAX];
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};
    const char *ctrl_prop = "ctl.start";
    const char *desired_status = "stopped";
    if(type == IPV6_STATELESS)
        return 0;
    memset(daemon_cmd, '\0', PROPERTY_VALUE_MAX);
    snprintf(daemon_cmd, sizeof(daemon_cmd), "%s_%s", DAEMON_NAME_RELEASE,interface);
    property_set(ctrl_prop, daemon_cmd);

    /* Stop the daemon and wait until it's reported to be stopped */
    memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
    snprintf(result_prop_name, sizeof(result_prop_name), "%s_%s", DAEMON_PROP_NAME, interface);
    if (wait_for_property(result_prop_name, desired_status, 10) < 0) {
        return -1;
    }

    snprintf(prop_value, sizeof(prop_value), "%s.ipaddress.%s", DAEMON_NAME, interface);
    property_set(prop_value, "");
    snprintf(prop_value, sizeof(prop_value), "%s.prefixlen.%s", DAEMON_NAME, interface);
    property_set(prop_value, "");
    snprintf(prop_value, sizeof(prop_value), "%s.%s.result", DAEMON_NAME, interface);
    property_set(prop_value, "");
    return 0;
}

int dhcpv6_check_status(const char *interface)
{
    char result_prop_name[PROPERTY_VALUE_MAX];
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};

    memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
    snprintf(result_prop_name, sizeof(result_prop_name), "%s.%s.result", DAEMON_NAME, interface);

    if (!property_get(result_prop_name, prop_value, NULL)) {
        /* shouldn't ever happen, given the success of wait_for_property() */
        return -1;
    }
    if (strcmp(prop_value, "ok") == 0) {
        return 0;
    } else {
        return -1;
    }
}

const char *dhcpv6_get_ipaddress(const char *interface, char *ipv6address)
{
    char result_prop_name[PROPERTY_VALUE_MAX];

    if(!dhcpv6_check_status(interface)) {
        memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
        snprintf(result_prop_name, sizeof(result_prop_name), "%s.ipaddress.%s", DAEMON_NAME, interface);
        if(!property_get(result_prop_name, ipv6address, NULL)) {
            return NULL;
        } else {
            return ipv6address;
        }
    } else {
        return NULL;
    }
}

const char *dhcpv6_get_gateway(char *ipv6_gateway)
{
    char addr6x[80];
    char iface[16];
    int iflags, metric, refcnt, use, prefix_len, slen;
    FILE *fp = fopen("/proc/net/ipv6_route", "r");

    while (1) {
        int r;
        r = fscanf(fp, "%32s%x%*s%x%32s%x%x%x%x%s\n",
                addr6x+14, &prefix_len, &slen, addr6x+40+7,
                &metric, &use, &refcnt, &iflags, iface);
        if(iflags & RTF_GATEWAY) {
            int i = 0;
            char *p = addr6x+14;

            do {
                if (!*p) {
                    if (i == 40) {
                        addr6x[39] = 0;
                        ++p;
                        continue;
                    }
                    return NULL;
                }
                addr6x[i++] = *p++;
                if (!((i+1) % 5)) {
                    addr6x[i++] = ':';
                }
            } while (i < 40+28+7);
            strcpy(ipv6_gateway, addr6x+40);
            *(ipv6_gateway + strlen(addr6x+40) + 1) = 0x00;
        }
        if (r != 9) {
            if ((r < 0) && feof(fp)) { /* EOF with no (nonspace) chars read. */
                break;
            }
        }
    }
    fclose(fp);
    if(strlen(ipv6_gateway) != 0) {
        return ipv6_gateway;
    } else {
        return NULL;
    }
}

const char *dhcpv6_get_prefixlen(const char *interface, char *prefixlen)
{
    char result_prop_name[PROPERTY_VALUE_MAX];

    if(!dhcpv6_check_status(interface)) {
        memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
        snprintf(result_prop_name, sizeof(result_prop_name), "%s.prefixlen.%s", DAEMON_NAME, interface);
        if(!property_get(result_prop_name, prefixlen, NULL)) {
            return NULL;
        } else {
            return prefixlen;
        }
    } else {
        return NULL;
    }
}

const char *dhcpv6_get_dns(const char *interface, char *dns, int dns_cnt)
{
    char result_prop_name[PROPERTY_VALUE_MAX];

    if(!dhcpv6_check_status(interface)) {
        memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
        snprintf(result_prop_name, sizeof(result_prop_name), "%s.%s.dns%d", DAEMON_NAME, interface, dns_cnt);
        if(!property_get(result_prop_name, dns, NULL)) {
            return NULL;
        } else {
            return dns;
        }
    } else {
        return NULL;
    }
}

#define DHCPV6_DNS_CNT_MAX 6
int dhcpv6_get_dns_cnt(const char *interface)
{
    char result_prop_name[PROPERTY_VALUE_MAX];
    char prop_value[PROPERTY_VALUE_MAX] = {'\0'};
    int i = 1;

    if(!dhcpv6_check_status(interface)) {
        for(i = 1; i < DHCPV6_DNS_CNT_MAX; i++) {
            memset(result_prop_name, '\0', PROPERTY_VALUE_MAX);
            snprintf(result_prop_name, sizeof(result_prop_name), "%s.%s.dns%d", DAEMON_NAME, interface, i);
            if(!property_get(result_prop_name, prop_value, NULL)) {
                ALOGD("dhcpv6_get_dns_cnt dns %d = NULL", i);
                break;
            }
        }
    }
    return i - 1;
}

char *dhcpv6_get_errmsg() {
    return errmsg;
}

static int icmpv6_probe(char *pIfname, struct in6_addr *dst)
{
    int sockfd      = -1;
    int sockopt		= 0;
    char buff[512] = {0};
    struct sockaddr_in6 multi_addr;
    struct ipv6_mreq mreq6;
	fd_set fdset;
    struct nd_neighbor_solicit *ns_pkt;	
	int ret_value = 0;
    if((NULL == pIfname) || (NULL == dst))
    {
        return ret_value;
    }
	//Debug test the dst 
	char ipDst[512] = {0};
	inet_ntop(AF_INET6,(void*)dst,ipDst,128);
	ALOGD("Debug: the pIfname is %s\nDebug: the dst addr is %s\n",pIfname,ipDst);
	
	//create socket and set the opt
    sockfd = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6);
    if(sockfd <= 0)
    {
        ALOGD("NS icmpv6 socket create error: %s \r\n",strerror(errno));
        return ret_value;
    }
	
    sockopt = offsetof(struct icmp6_hdr, icmp6_cksum);
    if(2 != sockopt)
    {
        ALOGD("icmp_cksum offset error: %s\r\n",strerror(errno));
        close(sockfd);
        return ret_value;
    }

    if(setsockopt(sockfd, SOL_RAW, IPV6_CHECKSUM, &sockopt, sizeof(sockopt)) < 0)
    {
        ALOGD("icmp_cksum set error: %s\r\n",strerror(errno));
        close(sockfd);
        return ret_value;
    }
	
    /*ns packet*/
    ns_pkt = (struct nd_neighbor_solicit*)buff;
    ns_pkt->nd_ns_type      = ND_NEIGHBOR_SOLICIT;
    ns_pkt->nd_ns_code      = 0;
    ns_pkt->nd_ns_reserved  = 0;
    memcpy(&ns_pkt->nd_ns_target, dst, sizeof(struct in6_addr));
    multi_addr.sin6_family = AF_INET6;

   if(inet_pton(AF_INET6, NS_MULTI_ADDR, &multi_addr.sin6_addr) < 0)  
   {  
		ALOGD("inet_pton error");  
		return ret_value; 
	}  

    multi_addr.sin6_addr.s6_addr[13] = dst->s6_addr[13];
    multi_addr.sin6_addr.s6_addr[14] = dst->s6_addr[14];  
    multi_addr.sin6_addr.s6_addr[15] = dst->s6_addr[15];

	//debug test the multiaddr 
	char ipNSMulti[512] = {0};
	inet_ntop(AF_INET6,(void*)&multi_addr.sin6_addr,ipNSMulti,512);
	ALOGD("Debug: the multi_addr is %s\n",ipNSMulti);
	
    //join group
    memcpy(&mreq6.ipv6mr_multiaddr, &multi_addr.sin6_addr,sizeof(struct in6_addr));
    mreq6.ipv6mr_interface = if_nametoindex(pIfname);
	ALOGD("Debug: the interface index is %d\n",mreq6.ipv6mr_interface);
    if(0 > setsockopt(sockfd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &mreq6, sizeof(mreq6)))
	{
		ALOGD("IPV6_JOIN_GROUP : %s\n",strerror(errno));
		close(sockfd);
		return ret_value;
	}
	else
	{
		ALOGD("Debug: join the grout success !\n");	
	}
    //set multicast hops to 255
    sockopt = 255;
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &sockopt, sizeof(int));
	ALOGD("Debug: multicast's icmpv6 type is  %d\n",((struct nd_neighbor_solicit*)buff)->nd_ns_type);	
   	/* wait neighbor advert reply, and check it */
	struct nd_neighbor_advert na_pkt ;	
	int nsProbeTimes = 3;
	struct timeval tm_probe = {1,0};	
    time_t	 prevTime;	
	
	while(nsProbeTimes > 0)
	{
		//send data
		if(0 > sendto(sockfd, buff, sizeof(struct nd_neighbor_solicit), 0, 
		         (struct sockaddr*)&multi_addr, sizeof(multi_addr)))
		{
			ALOGD("sendto: %s\n", strerror(errno));
			nsProbeTimes = -1;
			ret_value =  -1;		
		}
		else
		{
			ALOGD("Debug: sendto ns multicast success\n");
		}
	
		time(&prevTime);
		while (1) {
			FD_ZERO(&fdset);
			FD_SET(sockfd, &fdset);
			if (select(sockfd + 1, &fdset, (fd_set *) NULL, (fd_set *) NULL, &tm_probe) < 0) {
				if (errno != EINTR) 
				{
					ALOGD("select error: %s\r\n",strerror(errno));
					close(sockfd);
					nsProbeTimes = -1;
					ret_value =  -1;
					break;
				}
			}
			else if (FD_ISSET(sockfd, &fdset)) {
				if (recv(sockfd, &na_pkt, sizeof(na_pkt), 0) < 0 ) 
				{
					ALOGD("recv error: %s\r\n",strerror(errno));					
					nsProbeTimes = -1;
					ret_value =  -1;
					break;
				}
			
				if(na_pkt.nd_na_type == ND_NEIGHBOR_ADVERT){
					//Debug test the dst 
					char naIpTarget[128] = {0};
					inet_ntop(AF_INET6,(void*)&na_pkt.nd_na_target,naIpTarget,128);
					ALOGD("Debug: the dst addr is %s\n",naIpTarget);
					if( 0 == memcmp((void*)dst,(void*)&na_pkt.nd_na_target,sizeof(struct in6_addr)))
					{
						ALOGD("Debug: Ipv6 conflict!\n");
						nsProbeTimes = -1;
						ret_value =  1;
						break;
					}
				}
			}
			if((time(NULL) - prevTime) > 1)	
			{
				ALOGD("Debug: have not receive NA in 1s\n");
				--nsProbeTimes;
				break;
			}
		}
	}
    //leave the group
    setsockopt(sockfd, IPPROTO_IPV6, IPV6_LEAVE_GROUP, &mreq6, sizeof(mreq6));   
    close(sockfd);
	if(ret_value == 0)
	{
		ALOGD("Debug: Ipv6 address is legal!!");
	}
    return ret_value;
}

int ipv6_addr_arp(const char* iface, const char* addr)
{
    struct in6_addr  m_ip;
	char ifname[32];
	if(iface == NULL || addr == NULL)
		return -1;
	inet_pton(AF_INET6, addr, &m_ip);
	strcpy(ifname, iface);
	return icmpv6_probe(ifname, &m_ip);
}

int setEnable_ipv6_stateless(const char* iface, int conf)
{
    ALOGD("setEnable_ipv6_stateless: iface = %s, conf = %d", iface, conf);
    return 0;
} 
