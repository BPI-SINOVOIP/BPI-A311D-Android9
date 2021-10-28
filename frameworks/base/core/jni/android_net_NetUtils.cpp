/*
 * Copyright 2008, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "NetUtils"

#include "jni.h"
#include <nativehelper/JNIHelp.h>
#include "NetdClient.h"
#include <utils/misc.h>
#include <android_runtime/AndroidRuntime.h>
#include <utils/Log.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/filter.h>
#include <linux/if_arp.h>
#include <netinet/ether.h>
#include <netinet/icmp6.h>
#include <netinet/ip.h>
#include <netinet/ip6.h>
#include <netinet/udp.h>
#include <cutils/properties.h>

#include "core_jni_helpers.h"

extern "C" {
int ifc_enable(const char *ifname);
int ifc_disable(const char *ifname);
int ifc_reset_connections(const char *ifname, int reset_mask);

int dhcp_start(const char * const ifname);
int dhcp_start_renew(const char * const ifname);
int dhcp_get_results(const char * const ifname,
                     const char *ipaddr,
                     const char *gateway,
                     uint32_t *prefixLength,
                     const char *dns[],
                     const char *server,
                     uint32_t *lease,
                     const char *vendorInfo,
                     const char *domains,
                     const char *mtu);

int dhcp_stop(const char *ifname);
int dhcp_release_lease(const char *ifname);
char *dhcp_get_errmsg();
int dhcpv6_do_request(const char *interface, int type);
int dhcpv6_stop(const char *interface, int type);
int dhcpv6_release_lease(const char *interface, int type);
int dhcpv6_check_status(const char *interface);
const char *dhcpv6_get_ipaddress(const char *interface, char *ipv6address);
const char *dhcpv6_get_gateway(char *ipv6_gateway);
const char *dhcpv6_get_prefixlen(const char *interface, char *prefixlen);
const char *dhcpv6_get_dns(const char *interface, char *dns, int dns_cnt);
int dhcpv6_get_dns_cnt(const char *interface);
char *dhcpv6_get_errmsg();
int ifc_configure6(const char *ifname, const char *address, uint32_t prefixLength, const char *gateway, const char *dns1, const char
 *dns2);
int ifc_clear_ipv6_addresses(const char *name);
void ifc_clear_ipv6_static_dns(const char *name);
int ipv6_addr_arp(const char* iface, const char* addr);

}

#define NETUTILS_PKG_NAME "android/net/NetworkUtils"

namespace android {

static const uint32_t kEtherTypeOffset = offsetof(ether_header, ether_type);
static const uint32_t kEtherHeaderLen = sizeof(ether_header);
static const uint32_t kIPv4Protocol = kEtherHeaderLen + offsetof(iphdr, protocol);
static const uint32_t kIPv4FlagsOffset = kEtherHeaderLen + offsetof(iphdr, frag_off);
static const uint32_t kIPv6NextHeader = kEtherHeaderLen + offsetof(ip6_hdr, ip6_nxt);
static const uint32_t kIPv6PayloadStart = kEtherHeaderLen + sizeof(ip6_hdr);
static const uint32_t kICMPv6TypeOffset = kIPv6PayloadStart + offsetof(icmp6_hdr, icmp6_type);
static const uint32_t kUDPSrcPortIndirectOffset = kEtherHeaderLen + offsetof(udphdr, source);
static const uint32_t kUDPDstPortIndirectOffset = kEtherHeaderLen + offsetof(udphdr, dest);
static const uint16_t kDhcpClientPort = 68;

/*
 * The following remembers the jfieldID's of the fields
 * of the DhcpInfo Java object, so that we don't have
 * to look them up every time.
 */
static struct fieldIds {
    jmethodID clear;
    jmethodID setIpAddress;
    jmethodID setGateway;
    jmethodID addDns;
    jmethodID setDomains;
    jmethodID setServerAddress;
    jmethodID setLeaseDuration;
    jmethodID setVendorInfo;
} dhcpResultsFieldIds;

/*static jint android_net_utils_resetConnections(JNIEnv* env, jobject clazz,
      jstring ifname, jint mask)
{
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);

    ALOGD("android_net_utils_resetConnections in env=%p clazz=%p iface=%s mask=0x%x\n",
          env, clazz, nameStr, mask);

    result = ::ifc_reset_connections(nameStr, mask);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jint)result;
}*/

static jboolean android_net_utils_getDhcpResults(JNIEnv* env, jobject clazz, jstring ifname,
        jobject dhcpResults)
{
    int result;
    char  ipaddr[PROPERTY_VALUE_MAX];
    uint32_t prefixLength;
    char gateway[PROPERTY_VALUE_MAX];
    char    dns1[PROPERTY_VALUE_MAX];
    char    dns2[PROPERTY_VALUE_MAX];
    char    dns3[PROPERTY_VALUE_MAX];
    char    dns4[PROPERTY_VALUE_MAX];
    const char *dns[5] = {dns1, dns2, dns3, dns4, NULL};
    char  server[PROPERTY_VALUE_MAX];
    uint32_t lease;
    char vendorInfo[PROPERTY_VALUE_MAX];
    char domains[PROPERTY_VALUE_MAX];
    char mtu[PROPERTY_VALUE_MAX];

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    if (nameStr == NULL) return (jboolean)false;

    result = ::dhcp_get_results(nameStr, ipaddr, gateway, &prefixLength,
            dns, server, &lease, vendorInfo, domains, mtu);
    if (result != 0) {
        ALOGD("dhcp_get_results failed : %s (%s)", nameStr, ::dhcp_get_errmsg());
    }

    env->ReleaseStringUTFChars(ifname, nameStr);
    if (result == 0) {
        env->CallVoidMethod(dhcpResults, dhcpResultsFieldIds.clear);

        // set the linkAddress
        // dhcpResults->addLinkAddress(inetAddress, prefixLength)
        result = env->CallBooleanMethod(dhcpResults, dhcpResultsFieldIds.setIpAddress,
                env->NewStringUTF(ipaddr), prefixLength);
    }

    if (result == 0) {
        // set the gateway
        result = env->CallBooleanMethod(dhcpResults,
                dhcpResultsFieldIds.setGateway, env->NewStringUTF(gateway));
    }

    if (result == 0) {
        // dhcpResults->addDns(new InetAddress(dns1))
        result = env->CallBooleanMethod(dhcpResults,
                dhcpResultsFieldIds.addDns, env->NewStringUTF(dns1));
    }

    if (result == 0) {
        env->CallVoidMethod(dhcpResults, dhcpResultsFieldIds.setDomains,
                env->NewStringUTF(domains));

        result = env->CallBooleanMethod(dhcpResults,
                dhcpResultsFieldIds.addDns, env->NewStringUTF(dns2));

        if (result == 0) {
            result = env->CallBooleanMethod(dhcpResults,
                    dhcpResultsFieldIds.addDns, env->NewStringUTF(dns3));
            if (result == 0) {
                result = env->CallBooleanMethod(dhcpResults,
                        dhcpResultsFieldIds.addDns, env->NewStringUTF(dns4));
            }
        }
    }

    if (result == 0) {
        // dhcpResults->setServerAddress(new InetAddress(server))
        result = env->CallBooleanMethod(dhcpResults, dhcpResultsFieldIds.setServerAddress,
                env->NewStringUTF(server));
    }

    if (result == 0) {
        // dhcpResults->setLeaseDuration(lease)
        env->CallVoidMethod(dhcpResults,
                dhcpResultsFieldIds.setLeaseDuration, lease);

        // dhcpResults->setVendorInfo(vendorInfo)
        env->CallVoidMethod(dhcpResults, dhcpResultsFieldIds.setVendorInfo,
                env->NewStringUTF(vendorInfo));
    }
    return (jboolean)(result == 0);
}

static jboolean android_net_utils_startDhcp(JNIEnv* env, jobject clazz, jstring ifname)
{
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    if (nameStr == NULL) return (jboolean)false;
    if (::dhcp_start(nameStr) != 0) {
        ALOGD("dhcp_start failed : %s", nameStr);
        return (jboolean)false;
    }
    return (jboolean)true;
}

static jboolean android_net_utils_startDhcpRenew(JNIEnv* env, jobject clazz, jstring ifname)
{
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    if (nameStr == NULL) return (jboolean)false;
    if (::dhcp_start_renew(nameStr) != 0) {
        ALOGD("dhcp_start_renew failed : %s", nameStr);
        return (jboolean)false;
    }
    return (jboolean)true;
}

static jboolean android_net_utils_stopDhcp(JNIEnv* env, jobject clazz, jstring ifname)
{
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcp_stop(nameStr);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
}

static jboolean android_net_utils_releaseDhcpLease(JNIEnv* env, jobject clazz, jstring ifname)
{
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcp_release_lease(nameStr);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
}

static jstring android_net_utils_getDhcpError(JNIEnv* env, jobject clazz)
{
    return env->NewStringUTF(::dhcp_get_errmsg());
}

static void android_net_utils_attachDhcpFilter(JNIEnv *env, jobject clazz, jobject javaFd)
{
    struct sock_filter filter_code[] = {
        // Check the protocol is UDP.
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kIPv4Protocol),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,    IPPROTO_UDP, 0, 6),

        // Check this is not a fragment.
        BPF_STMT(BPF_LD  | BPF_H    | BPF_ABS, kIPv4FlagsOffset),
        BPF_JUMP(BPF_JMP | BPF_JSET | BPF_K,   IP_OFFMASK, 4, 0),

        // Get the IP header length.
        BPF_STMT(BPF_LDX | BPF_B    | BPF_MSH, kEtherHeaderLen),

        // Check the destination port.
        BPF_STMT(BPF_LD  | BPF_H    | BPF_IND, kUDPDstPortIndirectOffset),
        BPF_JUMP(BPF_JMP | BPF_JEQ  | BPF_K,   kDhcpClientPort, 0, 1),

        // Accept or reject.
        BPF_STMT(BPF_RET | BPF_K,              0xffff),
        BPF_STMT(BPF_RET | BPF_K,              0)
    };
    struct sock_fprog filter = {
        sizeof(filter_code) / sizeof(filter_code[0]),
        filter_code,
    };

    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(SO_ATTACH_FILTER): %s", strerror(errno));
    }
}

#define IPV6
static jboolean android_net_utils_runDhcpv6(JNIEnv* env, jobject clazz, jstring ifname, jint type)
{
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcpv6_do_request(nameStr, type);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
#else
    return false;
#endif
}

static jboolean android_net_utils_stopDhcpv6(JNIEnv* env, jobject clazz, jstring ifname, jint type)
{
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcpv6_stop(nameStr, type);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
#else
    return 0;
#endif
}
static jboolean android_net_utils_releaseDhcpv6Lease(JNIEnv* env, jobject clazz, jstring ifname, jint type)
{
#ifdef DHCPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcpv6_release_lease(nameStr, type);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
#else
        return 0;
#endif
}

static jboolean android_net_utils_checkDhcpv6Status(JNIEnv* env, jobject clazz, jstring ifname)
{
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::dhcpv6_check_status(nameStr);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
#else
    return 0;
#endif
}
static jboolean android_net_utils_clearIpv6Addresses(JNIEnv* env,
        jobject clazz,
        jstring ifname)
{
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    result = ::ifc_clear_ipv6_addresses(nameStr);
    ifc_clear_ipv6_static_dns(nameStr);
    env->ReleaseStringUTFChars(ifname, nameStr);
    return (jboolean)(result == 0);
#else
    return false;
#endif
}

static jboolean android_net_utils_isIpv6AddrConflict(JNIEnv* env, jobject clazz, jstring ifname, jstring ipaddr){
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
        const char *ipaddrStr = env->GetStringUTFChars(ipaddr, NULL);
    result = ::ipv6_addr_arp(nameStr, ipaddrStr);// @result equals 1 means conflict.
    env->ReleaseStringUTFChars(ifname, nameStr);
    env->ReleaseStringUTFChars(ipaddr, ipaddrStr);
    return (jboolean)(result == 0);
#else
    return false;
#endif
}

static jboolean android_net_utils_configure6Interface(JNIEnv* env,
        jobject clazz,
        jstring ifname,
        jstring ipaddr,
        jint prefixLength,
        jstring gateway,
        jstring dns1,
        jstring dns2)
{
#ifdef IPV6
    int result;

    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    const char *ipaddrStr = env->GetStringUTFChars(ipaddr, NULL);
    const char *gatewayStr = env->GetStringUTFChars(gateway, NULL);
    const char *dns1Str = env->GetStringUTFChars(dns1, NULL);
    const char *dns2Str = env->GetStringUTFChars(dns2, NULL);
    result = ::ifc_configure6(nameStr, ipaddrStr, prefixLength, gatewayStr, dns1Str, dns2Str);
    env->ReleaseStringUTFChars(ifname, nameStr);
    env->ReleaseStringUTFChars(ipaddr, ipaddrStr);
    env->ReleaseStringUTFChars(gateway, gatewayStr);
    env->ReleaseStringUTFChars(dns1, dns1Str);
    env->ReleaseStringUTFChars(dns2, dns2Str);
    return (jboolean)(result == 0);
#else
    return false;
#endif
}
static jstring android_net_utils_getDhcpv6Ipaddress(JNIEnv* env, jobject clazz, jstring ifname)
{
#ifdef IPV6
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    char ipv6address[50] = {0x00};
    return env->NewStringUTF(::dhcpv6_get_ipaddress(nameStr,(char *)ipv6address));
#else
    return NULL;
#endif
}

static jstring android_net_utils_getDhcpv6Gateway(JNIEnv* env, jobject clazz)
{
#ifdef IPV6
    char gateway[50] = {0x00};
    return env->NewStringUTF(::dhcpv6_get_gateway((char *)gateway));
#else
    return NULL;
#endif
}
static jstring android_net_utils_getDhcpv6Prefixlen(JNIEnv* env, jobject clazz, jstring ifname)
{
#ifdef IPV6
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    char prefixlen[10] = {0x00};
    return env->NewStringUTF(::dhcpv6_get_prefixlen(nameStr,(char *)prefixlen));
#else
    return NULL;
#endif
}

static jstring android_net_utils_getDhcpv6Dns(JNIEnv* env, jobject clazz, jstring ifname, jint cnt)
{
#ifdef IPV6
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    char dns[50] = {0x00};
    return env->NewStringUTF(::dhcpv6_get_dns(nameStr,(char *)dns, cnt));
#else
    return NULL;
#endif
}

static jint android_net_utils_getDhcpv6DnsCnt(JNIEnv* env, jobject clazz, jstring ifname)
{
#ifdef IPV6
    const char *nameStr = env->GetStringUTFChars(ifname, NULL);
    return (jint)(::dhcpv6_get_dns_cnt(nameStr));
#else
    return 0;
#endif
}
static jstring android_net_utils_getDhcpv6Error(JNIEnv* env, jobject clazz)
{
#ifdef IPV6
    return env->NewStringUTF(::dhcpv6_get_errmsg());
#else
    return NULL;
#endif
}

static void android_net_utils_attachRaFilter(JNIEnv *env, jobject clazz, jobject javaFd,
        jint hardwareAddressType)
{
    if (hardwareAddressType != ARPHRD_ETHER) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "attachRaFilter only supports ARPHRD_ETHER");
        return;
    }

    struct sock_filter filter_code[] = {
        // Check IPv6 Next Header is ICMPv6.
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kIPv6NextHeader),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,    IPPROTO_ICMPV6, 0, 3),

        // Check ICMPv6 type is Router Advertisement.
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kICMPv6TypeOffset),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,    ND_ROUTER_ADVERT, 0, 1),

        // Accept or reject.
        BPF_STMT(BPF_RET | BPF_K,              0xffff),
        BPF_STMT(BPF_RET | BPF_K,              0)
    };
    struct sock_fprog filter = {
        sizeof(filter_code) / sizeof(filter_code[0]),
        filter_code,
    };

    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(SO_ATTACH_FILTER): %s", strerror(errno));
    }
}

// TODO: Move all this filter code into libnetutils.
static void android_net_utils_attachControlPacketFilter(
        JNIEnv *env, jobject clazz, jobject javaFd, jint hardwareAddressType) {
    if (hardwareAddressType != ARPHRD_ETHER) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "attachControlPacketFilter only supports ARPHRD_ETHER");
        return;
    }

    // Capture all:
    //     - ARPs
    //     - DHCPv4 packets
    //     - Router Advertisements & Solicitations
    //     - Neighbor Advertisements & Solicitations
    //
    // tcpdump:
    //     arp or
    //     '(ip and udp port 68)' or
    //     '(icmp6 and ip6[40] >= 133 and ip6[40] <= 136)'
    struct sock_filter filter_code[] = {
        // Load the link layer next payload field.
        BPF_STMT(BPF_LD  | BPF_H   | BPF_ABS,  kEtherTypeOffset),

        // Accept all ARP.
        // TODO: Figure out how to better filter ARPs on noisy networks.
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ETHERTYPE_ARP, 16, 0),

        // If IPv4:
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ETHERTYPE_IP, 0, 9),

        // Check the protocol is UDP.
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kIPv4Protocol),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,    IPPROTO_UDP, 0, 14),

        // Check this is not a fragment.
        BPF_STMT(BPF_LD  | BPF_H    | BPF_ABS, kIPv4FlagsOffset),
        BPF_JUMP(BPF_JMP | BPF_JSET | BPF_K,   IP_OFFMASK, 12, 0),

        // Get the IP header length.
        BPF_STMT(BPF_LDX | BPF_B    | BPF_MSH, kEtherHeaderLen),

        // Check the source port.
        BPF_STMT(BPF_LD  | BPF_H    | BPF_IND, kUDPSrcPortIndirectOffset),
        BPF_JUMP(BPF_JMP | BPF_JEQ  | BPF_K,   kDhcpClientPort, 8, 0),

        // Check the destination port.
        BPF_STMT(BPF_LD  | BPF_H    | BPF_IND, kUDPDstPortIndirectOffset),
        BPF_JUMP(BPF_JMP | BPF_JEQ  | BPF_K,   kDhcpClientPort, 6, 7),

        // IPv6 ...
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ETHERTYPE_IPV6, 0, 6),
        // ... check IPv6 Next Header is ICMPv6 (ignore fragments), ...
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kIPv6NextHeader),
        BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K,    IPPROTO_ICMPV6, 0, 4),
        // ... and check the ICMPv6 type is one of RS/RA/NS/NA.
        BPF_STMT(BPF_LD  | BPF_B   | BPF_ABS,  kICMPv6TypeOffset),
        BPF_JUMP(BPF_JMP | BPF_JGE | BPF_K,    ND_ROUTER_SOLICIT, 0, 2),
        BPF_JUMP(BPF_JMP | BPF_JGT | BPF_K,    ND_NEIGHBOR_ADVERT, 1, 0),

        // Accept or reject.
        BPF_STMT(BPF_RET | BPF_K,              0xffff),
        BPF_STMT(BPF_RET | BPF_K,              0)
    };
    struct sock_fprog filter = {
        sizeof(filter_code) / sizeof(filter_code[0]),
        filter_code,
    };

    int fd = jniGetFDFromFileDescriptor(env, javaFd);
    if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(SO_ATTACH_FILTER): %s", strerror(errno));
    }
}

static void android_net_utils_setupRaSocket(JNIEnv *env, jobject clazz, jobject javaFd,
        jint ifIndex)
{
    static const int kLinkLocalHopLimit = 255;

    int fd = jniGetFDFromFileDescriptor(env, javaFd);

    // Set an ICMPv6 filter that only passes Router Solicitations.
    struct icmp6_filter rs_only;
    ICMP6_FILTER_SETBLOCKALL(&rs_only);
    ICMP6_FILTER_SETPASS(ND_ROUTER_SOLICIT, &rs_only);
    socklen_t len = sizeof(rs_only);
    if (setsockopt(fd, IPPROTO_ICMPV6, ICMP6_FILTER, &rs_only, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(ICMP6_FILTER): %s", strerror(errno));
        return;
    }

    // Most/all of the rest of these options can be set via Java code, but
    // because we're here on account of setting an icmp6_filter go ahead
    // and do it all natively for now.
    //
    // TODO: Consider moving these out to Java.

    // Set the multicast hoplimit to 255 (link-local only).
    int hops = kLinkLocalHopLimit;
    len = sizeof(hops);
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_HOPS, &hops, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(IPV6_MULTICAST_HOPS): %s", strerror(errno));
        return;
    }

    // Set the unicast hoplimit to 255 (link-local only).
    hops = kLinkLocalHopLimit;
    len = sizeof(hops);
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_UNICAST_HOPS, &hops, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(IPV6_UNICAST_HOPS): %s", strerror(errno));
        return;
    }

    // Explicitly disable multicast loopback.
    int off = 0;
    len = sizeof(off);
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &off, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(IPV6_MULTICAST_LOOP): %s", strerror(errno));
        return;
    }

    // Specify the IPv6 interface to use for outbound multicast.
    len = sizeof(ifIndex);
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_MULTICAST_IF, &ifIndex, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(IPV6_MULTICAST_IF): %s", strerror(errno));
        return;
    }

    // Additional options to be considered:
    //     - IPV6_TCLASS
    //     - IPV6_RECVPKTINFO
    //     - IPV6_RECVHOPLIMIT

    // Bind to [::].
    const struct sockaddr_in6 sin6 = {
            .sin6_family = AF_INET6,
            .sin6_port = 0,
            .sin6_flowinfo = 0,
            .sin6_addr = IN6ADDR_ANY_INIT,
            .sin6_scope_id = 0,
    };
    auto sa = reinterpret_cast<const struct sockaddr *>(&sin6);
    len = sizeof(sin6);
    if (bind(fd, sa, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "bind(IN6ADDR_ANY): %s", strerror(errno));
        return;
    }

    // Join the all-routers multicast group, ff02::2%index.
    struct ipv6_mreq all_rtrs = {
        .ipv6mr_multiaddr = {{{0xff,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}}},
        .ipv6mr_interface = ifIndex,
    };
    len = sizeof(all_rtrs);
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_JOIN_GROUP, &all_rtrs, len) != 0) {
        jniThrowExceptionFmt(env, "java/net/SocketException",
                "setsockopt(IPV6_JOIN_GROUP): %s", strerror(errno));
        return;
    }
}

static jboolean android_net_utils_bindProcessToNetwork(JNIEnv *env, jobject thiz, jint netId)
{
    return (jboolean) !setNetworkForProcess(netId);
}

static jint android_net_utils_getBoundNetworkForProcess(JNIEnv *env, jobject thiz)
{
    return getNetworkForProcess();
}

static jboolean android_net_utils_bindProcessToNetworkForHostResolution(JNIEnv *env, jobject thiz,
        jint netId)
{
    return (jboolean) !setNetworkForResolv(netId);
}

static jint android_net_utils_bindSocketToNetwork(JNIEnv *env, jobject thiz, jint socket,
        jint netId)
{
    return setNetworkForSocket(netId, socket);
}

static jboolean android_net_utils_protectFromVpn(JNIEnv *env, jobject thiz, jint socket)
{
    return (jboolean) !protectFromVpn(socket);
}

static jboolean android_net_utils_queryUserAccess(JNIEnv *env, jobject thiz, jint uid, jint netId)
{
    return (jboolean) !queryUserAccess(uid, netId);
}


// ----------------------------------------------------------------------------

/*
 * JNI registration.
 */
static const JNINativeMethod gNetworkUtilMethods[] = {
    /* name, signature, funcPtr */
	{ "startDhcp", "(Ljava/lang/String;)Z",  (void *)android_net_utils_startDhcp },
	{ "startDhcpRenew", "(Ljava/lang/String;)Z",  (void *)android_net_utils_startDhcpRenew },
	{ "getDhcpResults", "(Ljava/lang/String;Landroid/net/DhcpResults;)Z",  (void *)android_net_utils_getDhcpResults },
	{ "stopDhcp", "(Ljava/lang/String;)Z",	(void *)android_net_utils_stopDhcp },
	{ "releaseDhcpLease", "(Ljava/lang/String;)Z",	(void *)android_net_utils_releaseDhcpLease },
	{ "getDhcpError", "()Ljava/lang/String;", (void*) android_net_utils_getDhcpError },
	{ "bindProcessToNetwork", "(I)Z", (void*) android_net_utils_bindProcessToNetwork },
    { "getBoundNetworkForProcess", "()I", (void*) android_net_utils_getBoundNetworkForProcess },
    { "bindProcessToNetworkForHostResolution", "(I)Z", (void*) android_net_utils_bindProcessToNetworkForHostResolution },
    { "bindSocketToNetwork", "(II)I", (void*) android_net_utils_bindSocketToNetwork },
    { "protectFromVpn", "(I)Z", (void*)android_net_utils_protectFromVpn },
    { "queryUserAccess", "(II)Z", (void*)android_net_utils_queryUserAccess },
    { "attachDhcpFilter", "(Ljava/io/FileDescriptor;)V", (void*) android_net_utils_attachDhcpFilter },
    { "attachRaFilter", "(Ljava/io/FileDescriptor;I)V", (void*) android_net_utils_attachRaFilter },
    { "attachControlPacketFilter", "(Ljava/io/FileDescriptor;I)V", (void*) android_net_utils_attachControlPacketFilter },
    { "setupRaSocket", "(Ljava/io/FileDescriptor;I)V", (void*) android_net_utils_setupRaSocket },
	{ "runDhcpv6", "(Ljava/lang/String;I)Z",  (void *)android_net_utils_runDhcpv6 },
	{ "stopDhcpv6", "(Ljava/lang/String;I)Z",	(void *)android_net_utils_stopDhcpv6 },
	{ "releaseDhcpv6Lease", "(Ljava/lang/String;I)Z",	(void *)android_net_utils_releaseDhcpv6Lease },
	{ "checkDhcpv6Status", "(Ljava/lang/String;)Z",  (void *)android_net_utils_checkDhcpv6Status },
	{ "getDhcpv6Ipaddress", "(Ljava/lang/String;)Ljava/lang/String;",	(void *)android_net_utils_getDhcpv6Ipaddress },
	{ "getDhcpv6Gateway", "()Ljava/lang/String;",	(void *)android_net_utils_getDhcpv6Gateway},
	{ "getDhcpv6Prefixlen", "(Ljava/lang/String;)Ljava/lang/String;",	(void *)android_net_utils_getDhcpv6Prefixlen},
	{ "getDhcpv6Dns", "(Ljava/lang/String;I)Ljava/lang/String;",  (void *)android_net_utils_getDhcpv6Dns},
	{ "getDhcpv6DnsCnt", "(Ljava/lang/String;)I",	(void *)android_net_utils_getDhcpv6DnsCnt},
	{ "getDhcpv6Error", "()Ljava/lang/String;", (void*) android_net_utils_getDhcpv6Error },
	{ "configure6Interface", "(Ljava/lang/String;Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;)Z", (void *)android_net_utils_configure6Interface },
	{ "clearIpv6Addresses", "(Ljava/lang/String;)Z",  (void *)android_net_utils_clearIpv6Addresses},
	{ "isIpv6AddrConflict", "(Ljava/lang/String;Ljava/lang/String;)Z", (void*)android_net_utils_isIpv6AddrConflict},	 
};

int register_android_net_NetworkUtils(JNIEnv* env)
{
	jclass dhcpResultsClass = FindClassOrDie(env, "android/net/DhcpResults");
	
	dhcpResultsFieldIds.clear = GetMethodIDOrDie(env, dhcpResultsClass, "clear", "()V");
	dhcpResultsFieldIds.setIpAddress =GetMethodIDOrDie(env, dhcpResultsClass, "setIpAddress",
			 "(Ljava/lang/String;I)Z");
	dhcpResultsFieldIds.setGateway = GetMethodIDOrDie(env, dhcpResultsClass, "setGateway",
			 "(Ljava/lang/String;)Z");
	dhcpResultsFieldIds.addDns = GetMethodIDOrDie(env, dhcpResultsClass, "addDns",
			 "(Ljava/lang/String;)Z");
	dhcpResultsFieldIds.setDomains = GetMethodIDOrDie(env, dhcpResultsClass, "setDomains",
			 "(Ljava/lang/String;)V");
	dhcpResultsFieldIds.setServerAddress = GetMethodIDOrDie(env, dhcpResultsClass,
			 "setServerAddress", "(Ljava/lang/String;)Z");
	dhcpResultsFieldIds.setLeaseDuration = GetMethodIDOrDie(env, dhcpResultsClass,
			 "setLeaseDuration", "(I)V");
	dhcpResultsFieldIds.setVendorInfo = GetMethodIDOrDie(env, dhcpResultsClass, "setVendorInfo",
			 "(Ljava/lang/String;)V");
	
    return RegisterMethodsOrDie(env, NETUTILS_PKG_NAME, gNetworkUtilMethods,
                                NELEM(gNetworkUtilMethods));
}

}; // namespace android
