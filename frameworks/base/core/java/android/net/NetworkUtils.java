/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package android.net;

import android.os.Parcel;
import android.util.Log;
import android.util.Pair;

import java.io.FileDescriptor;
import java.math.BigInteger;
import java.net.Inet4Address;
import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.Collection;
import java.util.Locale;
import java.util.TreeSet;

/**
 * Native methods for managing network interfaces.
 *
 * {@hide}
 */
public class NetworkUtils {

    private static final String TAG = "NetworkUtils";

    /**
     * Start the DHCP client daemon, in order to have it request addresses
     * for the named interface.  This returns {@code true} if the DHCPv4 daemon
     * starts, {@code false} otherwise.  This call blocks until such time as a
     * result is available or the default discovery timeout has been reached.
     * Callers should check {@link #getDhcpResults} to determine whether DHCP
     * succeeded or failed, and if it succeeded, to fetch the {@link DhcpResults}.
     * @param interfaceName the name of the interface to configure
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean startDhcp(String interfaceName);

    /**
     * Initiate renewal on the DHCP client daemon for the named interface.  This
     * returns {@code true} if the DHCPv4 daemon has been notified, {@code false}
     * otherwise.  This call blocks until such time as a result is available or
     * the default renew timeout has been reached.  Callers should check
     * {@link #getDhcpResults} to determine whether DHCP succeeded or failed,
     * and if it succeeded, to fetch the {@link DhcpResults}.
     * @param interfaceName the name of the interface to configure
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean startDhcpRenew(String interfaceName);

    /**
     * Start the DHCP client daemon, in order to have it request addresses
     * for the named interface, and then configure the interface with those
     * addresses. This call blocks until it obtains a result (either success
     * or failure) from the daemon.
     * @param interfaceName the name of the interface to configure
     * @param dhcpResults if the request succeeds, this object is filled in with
     * the IP address information.
     * @return {@code true} for success, {@code false} for failure
     */
    public static boolean runDhcp(String interfaceName, DhcpResults dhcpResults) {
        return startDhcp(interfaceName) && getDhcpResults(interfaceName, dhcpResults);
    }

    /**
     * Initiate renewal on the DHCP client daemon. This call blocks until it obtains
     * a result (either success or failure) from the daemon.
     * @param interfaceName the name of the interface to configure
     * @param dhcpResults if the request succeeds, this object is filled in with
     * the IP address information.
     * @return {@code true} for success, {@code false} for failure
     */
    public static boolean runDhcpRenew(String interfaceName, DhcpResults dhcpResults) {
        return startDhcpRenew(interfaceName) && getDhcpResults(interfaceName, dhcpResults);
    }

    /**
     * Fetch results from the DHCP client daemon. This call returns {@code true} if
     * if there are results available to be read, {@code false} otherwise.
     * @param interfaceName the name of the interface to configure
     * @param dhcpResults if the request succeeds, this object is filled in with
     * the IP address information.
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean getDhcpResults(String interfaceName, DhcpResults dhcpResults);

    /**
     * Shut down the DHCP client daemon.
     * @param interfaceName the name of the interface for which the daemon
     * should be stopped
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean stopDhcp(String interfaceName);

    /**
     * Release the current DHCP lease.
     * @param interfaceName the name of the interface for which the lease should
     * be released
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean releaseDhcpLease(String interfaceName);

    /**
     * Return the last DHCP-related error message that was recorded.
     * <p/>NOTE: This string is not localized, but currently it is only
     * used in logging.
     * @return the most recent error message, if any
     */
    public native static String getDhcpError();

    /**
     * Start the DHCPv6 client daemon, in order to have it request addresses
     * for the named interface, and then configure the interface with those
     * addresses. This call blocks until it obtains a result (either success
     * or failure) from the daemon.
     * @param interfaceName the name of the interface to configure
     * @return ipv6 address for success, null for failure
     */

    //@type 0-dhcpv6, 1-stateless
    public native static boolean runDhcpv6(String interfaceName, int type);
    public static boolean runDhcpv6(String interfaceName){
        return runDhcpv6(interfaceName, 0);
    }

    /**
     * Shut down the DHCPv6 client daemon.
     * @param interfaceName the name of the interface for which the daemon
     * should be stopped
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean stopDhcpv6(String interfaceName, int type);
    public static boolean stopDhcpv6(String interfaceName){
        return stopDhcpv6(interfaceName, 0);
    }

    /**
     * Release the current DHCPv6 lease.
     * @param interfaceName the name of the interface for which the lease should
     * be released
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean releaseDhcpv6Lease(String interfaceName, int type);
    public static boolean releaseDhcpv6Lease(String interfaceName){
        return releaseDhcpv6Lease(interfaceName, 0);
    }

    /* check DHCPv6 status
     * @param interfaceName the name of the interface
     * be released
     * @return {@code true} for success, {@code false} for failure
     */
    public native static boolean checkDhcpv6Status(String interfaceName);

    /**
     * get DHCPv6 ipaddress
     * @param interfaceName the name of the interface
     * @return ipv6 address for success, null for failure
     */
    public native static String getDhcpv6Ipaddress(String interfaceName);

    /**
     * get DHCPv6 gateway
     * @param null
     * @return ipv6 gateway for success, null for failure
     */
    public native static String getDhcpv6Gateway();

    /**
     * get DHCPv6 prefixlen
     * @param interfaceName the name of the interface
     * @return ipv6 prefixlen for success, null for failure
     */
    public native static String getDhcpv6Prefixlen(String interfaceName);

    /**
     * get DHCPv6 dns
     * @param interfaceName the name of the interface
     * @param cnt the number of dns, 1 for primary dns, 2 for secondary dns
     * @return ipv6 dns for success, null for failure
     */
    public native static String getDhcpv6Dns(String interfaceName, int cnt);

    /**
     * get DHCPv6 dns count
     * @param interfaceName the name of the interface
     * @return ipv6 dns count
     */
    public native static int getDhcpv6DnsCnt(String interfaceName);
    /**
     * Return the last DHCPv6-related error message that was recorded.
     * <p/>NOTE: This string is not localized, but currently it is only
     * used in logging.
     * @return the most recent error message, if any
     */
    public native static String getDhcpv6Error();

    /**
     * Configure interface by Static IPV6
     * @param interfaceName
     * @param ipAddress
     * @param prefixLength
     * @param gateway
     * @param dns1
     * @param dns2
     * @return true for success, false for fail
     */
    public native static boolean configure6Interface(
        String interfaceName, String ipAddress, int prefixLength, String gateway, String dns1, String dns2);
    /**
     * Clear interface IPV6 address
     * @param interfaceName
     * @return true for success, false for fail
     */
    public native static boolean clearIpv6Addresses(String interfaceName);

    /**
    *ipconflict check
    *@iface: interface used
    *@ipAddr: target addr to solicitation
    */
    public native static boolean isIpv6AddrConflict(String iface, String ipAddr);

    /**
     * Attaches a socket filter that accepts DHCP packets to the given socket.
     */
    public native static void attachDhcpFilter(FileDescriptor fd) throws SocketException;

    /**
     * Attaches a socket filter that accepts ICMPv6 router advertisements to the given socket.
     * @param fd the socket's {@link FileDescriptor}.
     * @param packetType the hardware address type, one of ARPHRD_*.
     */
    public native static void attachRaFilter(FileDescriptor fd, int packetType) throws SocketException;

    /**
     * Attaches a socket filter that accepts L2-L4 signaling traffic required for IP connectivity.
     *
     * This includes: all ARP, ICMPv6 RS/RA/NS/NA messages, and DHCPv4 exchanges.
     *
     * @param fd the socket's {@link FileDescriptor}.
     * @param packetType the hardware address type, one of ARPHRD_*.
     */
    public native static void attachControlPacketFilter(FileDescriptor fd, int packetType)
            throws SocketException;

    /**
     * Configures a socket for receiving ICMPv6 router solicitations and sending advertisements.
     * @param fd the socket's {@link FileDescriptor}.
     * @param ifIndex the interface index.
     */
    public native static void setupRaSocket(FileDescriptor fd, int ifIndex) throws SocketException;

    /**
     * Binds the current process to the network designated by {@code netId}.  All sockets created
     * in the future (and not explicitly bound via a bound {@link SocketFactory} (see
     * {@link Network#getSocketFactory}) will be bound to this network.  Note that if this
     * {@code Network} ever disconnects all sockets created in this way will cease to work.  This
     * is by design so an application doesn't accidentally use sockets it thinks are still bound to
     * a particular {@code Network}.  Passing NETID_UNSET clears the binding.
     */
    public native static boolean bindProcessToNetwork(int netId);

    /**
     * Return the netId last passed to {@link #bindProcessToNetwork}, or NETID_UNSET if
     * {@link #unbindProcessToNetwork} has been called since {@link #bindProcessToNetwork}.
     */
    public native static int getBoundNetworkForProcess();

    /**
     * Binds host resolutions performed by this process to the network designated by {@code netId}.
     * {@link #bindProcessToNetwork} takes precedence over this setting.  Passing NETID_UNSET clears
     * the binding.
     *
     * @deprecated This is strictly for legacy usage to support startUsingNetworkFeature().
     */
    @Deprecated
    public native static boolean bindProcessToNetworkForHostResolution(int netId);

    /**
     * Explicitly binds {@code socketfd} to the network designated by {@code netId}.  This
     * overrides any binding via {@link #bindProcessToNetwork}.
     * @return 0 on success or negative errno on failure.
     */
    public native static int bindSocketToNetwork(int socketfd, int netId);

    /**
     * Protect {@code fd} from VPN connections.  After protecting, data sent through
     * this socket will go directly to the underlying network, so its traffic will not be
     * forwarded through the VPN.
     */
    public static boolean protectFromVpn(FileDescriptor fd) {
        return protectFromVpn(fd.getInt$());
    }

    /**
     * Protect {@code socketfd} from VPN connections.  After protecting, data sent through
     * this socket will go directly to the underlying network, so its traffic will not be
     * forwarded through the VPN.
     */
    public native static boolean protectFromVpn(int socketfd);

    /**
     * Determine if {@code uid} can access network designated by {@code netId}.
     * @return {@code true} if {@code uid} can access network, {@code false} otherwise.
     */
    public native static boolean queryUserAccess(int uid, int netId);

    /**
     * Convert a IPv4 address from an integer to an InetAddress.
     * @param hostAddress an int corresponding to the IPv4 address in network byte order
     */
    public static InetAddress intToInetAddress(int hostAddress) {
        byte[] addressBytes = { (byte)(0xff & hostAddress),
                                (byte)(0xff & (hostAddress >> 8)),
                                (byte)(0xff & (hostAddress >> 16)),
                                (byte)(0xff & (hostAddress >> 24)) };

        try {
           return InetAddress.getByAddress(addressBytes);
        } catch (UnknownHostException e) {
           throw new AssertionError();
        }
    }

    /**
     * Convert a IPv4 address from an InetAddress to an integer
     * @param inetAddr is an InetAddress corresponding to the IPv4 address
     * @return the IP address as an integer in network byte order
     */
    public static int inetAddressToInt(Inet4Address inetAddr)
            throws IllegalArgumentException {
        byte [] addr = inetAddr.getAddress();
        return ((addr[3] & 0xff) << 24) | ((addr[2] & 0xff) << 16) |
                ((addr[1] & 0xff) << 8) | (addr[0] & 0xff);
    }

    /**
     * Convert a network prefix length to an IPv4 netmask integer
     * @param prefixLength
     * @return the IPv4 netmask as an integer in network byte order
     */
    public static int prefixLengthToNetmaskInt(int prefixLength)
            throws IllegalArgumentException {
        if (prefixLength < 0 || prefixLength > 32) {
            throw new IllegalArgumentException("Invalid prefix length (0 <= prefix <= 32)");
        }
        int value = 0xffffffff << (32 - prefixLength);
        return Integer.reverseBytes(value);
    }

    /**
     * Convert a IPv4 netmask integer to a prefix length
     * @param netmask as an integer in network byte order
     * @return the network prefix length
     */
    public static int netmaskIntToPrefixLength(int netmask) {
        return Integer.bitCount(netmask);
    }

    /**
     * Convert an IPv4 netmask to a prefix length, checking that the netmask is contiguous.
     * @param netmask as a {@code Inet4Address}.
     * @return the network prefix length
     * @throws IllegalArgumentException the specified netmask was not contiguous.
     * @hide
     */
    public static int netmaskToPrefixLength(Inet4Address netmask) {
        // inetAddressToInt returns an int in *network* byte order.
        int i = Integer.reverseBytes(inetAddressToInt(netmask));
        int prefixLength = Integer.bitCount(i);
        int trailingZeros = Integer.numberOfTrailingZeros(i);
        if (trailingZeros != 32 - prefixLength) {
            throw new IllegalArgumentException("Non-contiguous netmask: " + Integer.toHexString(i));
        }
        return prefixLength;
    }


    /**
     * Create an InetAddress from a string where the string must be a standard
     * representation of a V4 or V6 address.  Avoids doing a DNS lookup on failure
     * but it will throw an IllegalArgumentException in that case.
     * @param addrString
     * @return the InetAddress
     * @hide
     */
    public static InetAddress numericToInetAddress(String addrString)
            throws IllegalArgumentException {
        return InetAddress.parseNumericAddress(addrString);
    }

    /**
     * Writes an InetAddress to a parcel. The address may be null. This is likely faster than
     * calling writeSerializable.
     */
    protected static void parcelInetAddress(Parcel parcel, InetAddress address, int flags) {
        byte[] addressArray = (address != null) ? address.getAddress() : null;
        parcel.writeByteArray(addressArray);
    }

    /**
     * Reads an InetAddress from a parcel. Returns null if the address that was written was null
     * or if the data is invalid.
     */
    protected static InetAddress unparcelInetAddress(Parcel in) {
        byte[] addressArray = in.createByteArray();
        if (addressArray == null) {
            return null;
        }
        try {
            return InetAddress.getByAddress(addressArray);
        } catch (UnknownHostException e) {
            return null;
        }
    }


    /**
     *  Masks a raw IP address byte array with the specified prefix length.
     */
    public static void maskRawAddress(byte[] array, int prefixLength) {
        if (prefixLength < 0 || prefixLength > array.length * 8) {
            throw new RuntimeException("IP address with " + array.length +
                    " bytes has invalid prefix length " + prefixLength);
        }

        int offset = prefixLength / 8;
        int remainder = prefixLength % 8;
        byte mask = (byte)(0xFF << (8 - remainder));

        if (offset < array.length) array[offset] = (byte)(array[offset] & mask);

        offset++;

        for (; offset < array.length; offset++) {
            array[offset] = 0;
        }
    }

    /**
     * Get InetAddress masked with prefixLength.  Will never return null.
     * @param address the IP address to mask with
     * @param prefixLength the prefixLength used to mask the IP
     */
    public static InetAddress getNetworkPart(InetAddress address, int prefixLength) {
        byte[] array = address.getAddress();
        maskRawAddress(array, prefixLength);

        InetAddress netPart = null;
        try {
            netPart = InetAddress.getByAddress(array);
        } catch (UnknownHostException e) {
            throw new RuntimeException("getNetworkPart error - " + e.toString());
        }
        return netPart;
    }

    /**
     * Returns the implicit netmask of an IPv4 address, as was the custom before 1993.
     */
    public static int getImplicitNetmask(Inet4Address address) {
        int firstByte = address.getAddress()[0] & 0xff;  // Convert to an unsigned value.
        if (firstByte < 128) {
            return 8;
        } else if (firstByte < 192) {
            return 16;
        } else if (firstByte < 224) {
            return 24;
        } else {
            return 32;  // Will likely not end well for other reasons.
        }
    }

    /**
     * Utility method to parse strings such as "192.0.2.5/24" or "2001:db8::cafe:d00d/64".
     * @hide
     */
    public static Pair<InetAddress, Integer> parseIpAndMask(String ipAndMaskString) {
        InetAddress address = null;
        int prefixLength = -1;
        try {
            String[] pieces = ipAndMaskString.split("/", 2);
            prefixLength = Integer.parseInt(pieces[1]);
            address = InetAddress.parseNumericAddress(pieces[0]);
        } catch (NullPointerException e) {            // Null string.
        } catch (ArrayIndexOutOfBoundsException e) {  // No prefix length.
        } catch (NumberFormatException e) {           // Non-numeric prefix.
        } catch (IllegalArgumentException e) {        // Invalid IP address.
        }

        if (address == null || prefixLength == -1) {
            throw new IllegalArgumentException("Invalid IP address and mask " + ipAndMaskString);
        }

        return new Pair<InetAddress, Integer>(address, prefixLength);
    }

    /**
     * Check if IP address type is consistent between two InetAddress.
     * @return true if both are the same type.  False otherwise.
     */
    public static boolean addressTypeMatches(InetAddress left, InetAddress right) {
        return (((left instanceof Inet4Address) && (right instanceof Inet4Address)) ||
                ((left instanceof Inet6Address) && (right instanceof Inet6Address)));
    }

    /**
     * Convert a 32 char hex string into a Inet6Address.
     * throws a runtime exception if the string isn't 32 chars, isn't hex or can't be
     * made into an Inet6Address
     * @param addrHexString a 32 character hex string representing an IPv6 addr
     * @return addr an InetAddress representation for the string
     */
    public static InetAddress hexToInet6Address(String addrHexString)
            throws IllegalArgumentException {
        try {
            return numericToInetAddress(String.format(Locale.US, "%s:%s:%s:%s:%s:%s:%s:%s",
                    addrHexString.substring(0,4),   addrHexString.substring(4,8),
                    addrHexString.substring(8,12),  addrHexString.substring(12,16),
                    addrHexString.substring(16,20), addrHexString.substring(20,24),
                    addrHexString.substring(24,28), addrHexString.substring(28,32)));
        } catch (Exception e) {
            Log.e("NetworkUtils", "error in hexToInet6Address(" + addrHexString + "): " + e);
            throw new IllegalArgumentException(e);
        }
    }

    /**
     * Create a string array of host addresses from a collection of InetAddresses
     * @param addrs a Collection of InetAddresses
     * @return an array of Strings containing their host addresses
     */
    public static String[] makeStrings(Collection<InetAddress> addrs) {
        String[] result = new String[addrs.size()];
        int i = 0;
        for (InetAddress addr : addrs) {
            result[i++] = addr.getHostAddress();
        }
        return result;
    }

    /**
     * Trim leading zeros from IPv4 address strings
     * Our base libraries will interpret that as octel..
     * Must leave non v4 addresses and host names alone.
     * For example, 192.168.000.010 -> 192.168.0.10
     * TODO - fix base libraries and remove this function
     * @param addr a string representing an ip addr
     * @return a string propertly trimmed
     */
    public static String trimV4AddrZeros(String addr) {
        if (addr == null) return null;
        String[] octets = addr.split("\\.");
        if (octets.length != 4) return addr;
        StringBuilder builder = new StringBuilder(16);
        String result = null;
        for (int i = 0; i < 4; i++) {
            try {
                if (octets[i].length() > 3) return addr;
                builder.append(Integer.parseInt(octets[i]));
            } catch (NumberFormatException e) {
                return addr;
            }
            if (i < 3) builder.append('.');
        }
        result = builder.toString();
        return result;
    }

    /**
     * Returns a prefix set without overlaps.
     *
     * This expects the src set to be sorted from shorter to longer. Results are undefined
     * failing this condition. The returned prefix set is sorted in the same order as the
     * passed set, with the same comparator.
     */
    private static TreeSet<IpPrefix> deduplicatePrefixSet(final TreeSet<IpPrefix> src) {
        final TreeSet<IpPrefix> dst = new TreeSet<>(src.comparator());
        // Prefixes match addresses that share their upper part up to their length, therefore
        // the only kind of possible overlap in two prefixes is strict inclusion of the longer
        // (more restrictive) in the shorter (including equivalence if they have the same
        // length).
        // Because prefixes in the src set are sorted from shorter to longer, deduplicating
        // is done by simply iterating in order, and not adding any longer prefix that is
        // already covered by a shorter one.
        newPrefixes:
        for (IpPrefix newPrefix : src) {
            for (IpPrefix existingPrefix : dst) {
                if (existingPrefix.containsPrefix(newPrefix)) {
                    continue newPrefixes;
                }
            }
            dst.add(newPrefix);
        }
        return dst;
    }

    /**
     * Returns how many IPv4 addresses match any of the prefixes in the passed ordered set.
     *
     * Obviously this returns an integral value between 0 and 2**32.
     * The behavior is undefined if any of the prefixes is not an IPv4 prefix or if the
     * set is not ordered smallest prefix to longer prefix.
     *
     * @param prefixes the set of prefixes, ordered by length
     */
    public static long routedIPv4AddressCount(final TreeSet<IpPrefix> prefixes) {
        long routedIPCount = 0;
        for (final IpPrefix prefix : deduplicatePrefixSet(prefixes)) {
            if (!prefix.isIPv4()) {
                Log.wtf(TAG, "Non-IPv4 prefix in routedIPv4AddressCount");
            }
            int rank = 32 - prefix.getPrefixLength();
            routedIPCount += 1L << rank;
        }
        return routedIPCount;
    }

    /**
     * Returns how many IPv6 addresses match any of the prefixes in the passed ordered set.
     *
     * This returns a BigInteger between 0 and 2**128.
     * The behavior is undefined if any of the prefixes is not an IPv6 prefix or if the
     * set is not ordered smallest prefix to longer prefix.
     */
    public static BigInteger routedIPv6AddressCount(final TreeSet<IpPrefix> prefixes) {
        BigInteger routedIPCount = BigInteger.ZERO;
        for (final IpPrefix prefix : deduplicatePrefixSet(prefixes)) {
            if (!prefix.isIPv6()) {
                Log.wtf(TAG, "Non-IPv6 prefix in routedIPv6AddressCount");
            }
            int rank = 128 - prefix.getPrefixLength();
            routedIPCount = routedIPCount.add(BigInteger.ONE.shiftLeft(rank));
        }
        return routedIPCount;
    }
}
