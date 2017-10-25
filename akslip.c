// src/aksl/akslip.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/akslip.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
// This module contains simple atomic IP actions without event-handling.
// For event-handling software, see module selector.[ch].
/*------------------------------------------------------------------------------
Functions in this file:

print
hostname2ip
sendto
sendto
udp_open
udp_open
tcp_open
tcp_open
tcp_reopen
reset_ip_src_dst
reset_tcp_src_dst
accept_in
tcp_packet::
    print
ip_packet::
    ip_packet
    decode_options
    decode
    print
nbytes_from::
    recvfrom
udp_port_set::
    open
ip_map_table::
    insert_map
    clear_map
    print_maps
ip_switch::
    smap
    dmap
    tcp_mirror
    send_to_switch
    print
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/akslip.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files:
#if defined(SOLARIS) || defined(linux)
// For FIONBIO:
#ifndef AKSL_X_FCNTL_H
#define AKSL_X_FCNTL_H
#include <fcntl.h>
#endif

// For perror():
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

#elif !defined(WIN32)
// For FIONBIO:
#ifndef AKSL_X_SYS_IOCTL_H
#define AKSL_X_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif

#ifdef WIN32
#ifndef AKSL_X_WINIOCTL_H
#define AKSL_X_WINIOCTL_H
#include <winioctl.h>
#endif

#else
#ifndef AKSL_X_ARPA_INET_H
#define AKSL_X_ARPA_INET_H
#include <arpa/inet.h>
#endif
#endif

#ifndef AKSL_X_ERRNO_H
#define AKSL_X_ERRNO_H
#include <errno.h>
#endif

static const char* ip_protocol_string[256] = {
    "ip", "icmp", "igmp", "ggp", "?", "?", "tcp", "?",      // 0
    "egp", "?", "?", "?", "pup", "?", "?", "?",             // 8
    "?", "udp", "?", "?", "?", "?", "idp", "?",             // 16
    "?", "?", "?", "?", "?", "?", "?", "?",                 // 24
    "?", "?", "?", "?", "?", "?", "?", "?",                 // 32
    "?", "?", "?", "?", "?", "?", "?", "?",                 // 40
    "?", "?", "?", "?", "?", "?", "?", "?",                 // 48
    "?", "?", "?", "?", "?", "?", "?", "hello",             // 56
    "?", "?", "?", "?", "?", "?", "?", "?",                 // 64
    "?", "?", "?", "?", "?", "nd", "?", "?",                // 72
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "?",
    "?", "?", "?", "?", "?", "?", "?", "raw"
    };

/*------------------------------------------------------------------------------
Print a "hostent" structure, which is the structure returned by a
gethostbyname() call. It describes the contents of an entry in the /etc/hosts
file.
------------------------------------------------------------------------------*/
//----------------------//
//         print        //
//----------------------//
void print(hostent* pe, ostream& os) {
    if (!pe) {
        os << "Null hostent pointer." << endl;
        return;
        }
    if (!pe->h_name)
        os << "official host name = null pointer\n";
    else
        os << "official host name = \"" << pe->h_name << "\"\n";

    for (char** ppc = pe->h_aliases; *ppc; ++ppc)
        os << "host name alias = \"" << *ppc << "\"\n";

    os << "host address type = " << pe->h_addrtype << NL;

    os << "host address length = " << pe->h_length << NL;

    for (FOR_DECL(char**) ppc = pe->h_addr_list; *ppc; ++ppc) {
        os << "host IP address = ";
        for (int i = 0; i < pe->h_length; ++i) {
            if (i > 0)
                os << ".";
            os << (int)(0xff & (*ppc)[i]);
            }
        os << NL;
        }
    os << flush;
    } // End of function print.

/*------------------------------------------------------------------------------
This returns a 32-bit address (in host byte order) for the given string.
If any error occurs, then 0 is returned.
Warning: unfortunately, INADDR_ANY == 0 !!!!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note: this function should cache IP addresses, because the DNS look-ups are very
costly.
------------------------------------------------------------------------------*/
//----------------------//
//     hostname2ip      //
//----------------------//
uint32 hostname2ip(const char* hostname) {
    if (nullstr(hostname))
        return INADDR_ANY;      // The default local host IP address.

    // Try to decode address as a dotted address:
    uint32 addr_net = inet_addr(hostname);
    uint32 addr = ntohl(addr_net);    // Convert to host order.

    // If not a dotted address, try the /etc/hosts file etc.:
    if (addr_net == u_long(-1)) {
        // The gethostbyname() call seems to require two TCP connections!
        hostent* pe = gethostbyname(hostname);
        if (pe && *pe->h_addr_list)
            addr = ntohl(*(uint32*)*pe->h_addr_list);
        else
            addr = 0;
        }
    return addr;
    } // End of function hostname2ip.

/*------------------------------------------------------------------------------
Function to interface to the Unix sendto function.
------------------------------------------------------------------------------*/
//----------------------//
//        sendto        //
//----------------------//
int sendto(int fd_udp, const char* bytes, int n_bytes,
                  uint32 host, uint16 port) {
    if (fd_udp < 0 || !bytes || n_bytes < 0)
        return -1;

    sockaddr_in to;
    set_in(to, host, port);

    // Transmit the UDP packet:
    int flags = 0;
    return sendto(fd_udp, bytes, n_bytes, flags, (sockaddr*)&to, sizeof(to));
    } // End of function sendto.

/*------------------------------------------------------------------------------
Function to interface to the Unix sendto function.
This does not permit sending of empty UDP packets.
------------------------------------------------------------------------------*/
//----------------------//
//        sendto        //
//----------------------//
int sendto(int fd_udp, const nbytes& buf, const sockaddr_in& to) {
    if (fd_udp < 0 || buf.empty())
        return -1;

    // Transmit the UDP packet:
    int flags = 0;
    return sendto(fd_udp, buf.bytes(), buf.n_bytes(), flags,
                  (sockaddr*)&to, sizeof(to));
    } // End of function sendto.

/*------------------------------------------------------------------------------
This function opens a UDP port for listening.
The UDP port numbers from port1 to (port1 + n_tries - 1) are tried
until one of them can successfully be opened and put into non-blocking mode.
port1 is assumed to be in host byte-order.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Return value:
>= 0                    fd of successfully opened UDP port.
eBAD_ARGUMENT           erroneous argument
eSOCKET_FAILED          could not open UDP socket;
eBIND_FAILED            could not bind any of the n_tries sockets;
eNONBLOCKING_FAILED     could not put port into non-blocking mode.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Returned value of port1:
unchanged   any error condition
changed     successful opening of port: the opened port number.
------------------------------------------------------------------------------*/
//----------------------//
//       udp_open       //
//----------------------//
int udp_open(uint16& port1, int n_tries, uint32 loc_ip) {
    if (n_tries <= 0)
        return eBAD_ARGUMENT;

    // Create a UDP socket:
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        cout << "udp_open(): Could not open UDP socket." << endl;
        perror("socket");
        return eSOCKET_FAILED;
        }

    // Choose the input address:
    struct sockaddr_in bname;
    set_in(bname, loc_ip, port1);

    bool_enum found_port = false;
    uint16 i = 0;
    for (i = 0; i < n_tries; ++i) {
        // Set the address for input:
        bname.sin_port = htons((short)(port1 + i));
        int err = bind_in(fd, bname);
        if (err >= 0) {
            found_port = true;
            break;
            }
        if (err < 0) {
            cout << "udp_open(): could not bind UDP port "
                 << (port1 + i) << endl;
            perror("bind");
            }
        }
    if (!found_port) {
        ::close(fd);
        return eBIND_FAILED;
        }

    // Put socket into non-blocking mode:
#if defined(SOLARIS) || defined(linux)
    int ret = fcntl(fd, F_SETFL, (fcntl(fd, F_GETFL, 0) | O_NONBLOCK));
    if (ret < 0) {
        cout << "udp_open(): Error while calling fcntl to set non-blocking."
             << endl;
        perror("fcntl");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#elif defined(WIN32)
    unsigned long x = true;
    if (ioctlsocket(fd, FIONBIO, &x) < 0) {
        cout << "udp_open(): Error while calling ioctl to set non-blocking."
             << endl;
        perror("ioctlsocket");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#else
    int x = true;
    if (ioctl(fd, FIONBIO, &x) < 0) {
        cout << "udp_open(): Error while calling ioctl to set non-blocking."
             << endl;
        perror("ioctl");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#endif
    port1 += i;
    return fd;
    } // End of function udp_open.

/*------------------------------------------------------------------------------
This function opens a UDP port for listening.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Return value:
>= 0                    fd of successfully opened UDP port.
eSOCKET_FAILED          could not open UDP socket;
eBIND_FAILED            could not bind the socket;
eNONBLOCKING_FAILED     could not put port into non-blocking mode.
------------------------------------------------------------------------------*/
//----------------------//
//       udp_open       //
//----------------------//
int udp_open(sockaddr_in& bname) {
    // Create a UDP socket.
    int fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        cout << "udp_open(): Could not open UDP socket." << endl;
        perror("socket");
        return eSOCKET_FAILED;
        }

    // Try to bind the port.
    int err = bind_in(fd, bname);
    if (err < 0) {
        cout << "udp_open(): Could not bind UDP port." << endl;
        perror("bind");
        ::close(fd);
        return eBIND_FAILED;
        }

    // Put socket into non-blocking mode:
#if defined(SOLARIS) || defined(linux)
    int ret = fcntl(fd, F_SETFL, (fcntl(fd, F_GETFL, 0) | O_NONBLOCK));
    if (ret < 0) {
        cout << "udp_open(): Error while calling fcntl to set non-blocking."
             << endl;
        perror("fcntl");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#elif defined(WIN32)
    unsigned long x = true;
    if (ioctlsocket(fd, FIONBIO, &x) < 0) {
        cout << "udp_open(): Error calling ioctlsocket to set non-blocking."
             << endl;
        perror("ioctlsocket");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#else
    int x = true;
    if (ioctl(fd, FIONBIO, &x) < 0) {
        cout << "udp_open(): Error while calling ioctl to set non-blocking."
             << endl;
        perror("ioctl");
        ::close(fd);
        return eNONBLOCKING_FAILED;
        }
#endif
    return fd;
    } // End of function udp_open.

/*------------------------------------------------------------------------------
This opens the TCP port in the passive mode.
Function return value:
>= 0                    fd of successfully opened TCP port.
eSOCKET_FAILED          could not open TCP socket.
eBIND_FAILED            could not bind the TCP socket.
eLISTEN_FAILED          failed to put the TCP socket into listen-mode.
eNONBLOCKING_FAILED     could not put TCP port into non-blocking mode.
------------------------------------------------------------------------------*/
//----------------------//
//       tcp_open       //
//----------------------//
int tcp_open(uint16 loc_port, uint32 loc_ip, int backlog) {
    // Open the new TCP control port for proxying:
    int fd0 = socket(PF_INET, SOCK_STREAM, 0);
    if (fd0 < 0) {
        cout << "tcp_open: could not open TCP socket." << endl;
        perror("socket");
        return eSOCKET_FAILED;
        }

    // Bind the local port:
    struct sockaddr_in bname;
    set_in(bname, loc_ip, loc_port);
    int err = bind_in(fd0, bname);
    if (err < 0) {
        cout << "tcp_open: could not bind TCP port " << loc_port << endl;
        perror("bind");
        ::close(fd0);
        return eBIND_FAILED;
        }

    // Put the socket into passive mode, using listen():
    err = listen(fd0, backlog);
    if (err < 0) {
        cout << "tcp_open: error putting TCP port into passive mode."
             << endl;
        perror("listen");
        ::close(fd0);
        return eLISTEN_FAILED;
        }

    // Put the socket into non-blocking mode:
#if defined(SOLARIS) || defined(linux)
    err = fcntl(fd0, F_SETFL, (fcntl(fd0, F_GETFL, 0) | O_NONBLOCK));
    if (err < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("fcntl");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#elif defined(WIN32)
    unsigned long x = true;
    if (ioctlsocket(fd0, FIONBIO, &x) < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("ioctlsocket");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#else
    int x = true;
    if (ioctl(fd0, FIONBIO, &x) < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("ioctl");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#endif
    // Return the fd for the control port fd:
    return fd0;
    } // End of function tcp_open.

/*------------------------------------------------------------------------------
This opens the TCP port in the active mode.
Function return value:
>= 0                    fd of successfully opened (and connected) TCP port.
eIN_PROGRESS            must wait for connection to be completed.
eSOCKET_FAILED          could not open TCP socket.
eBIND_FAILED            could not bind the TCP socket.
eNONBLOCKING_FAILED     could not put TCP port into non-blocking mode.
eCONNECT_FAILED         failed to initiate TCP port connection.
------------------------------------------------------------------------------*/
//----------------------//
//       tcp_open       //
//----------------------//
int tcp_open(uint16 rem_port, uint32 rem_ip, uint16 loc_port, uint32 loc_ip,
             int& fd0) {
    // Open the new TCP control port for proxying:
    fd0 = socket(PF_INET, SOCK_STREAM, 0);
    if (fd0 < 0) {
        cout << "tcp_open: could not open TCP socket." << endl;
        perror("socket");
        return eSOCKET_FAILED;
        }

    // Bind the local port:
    struct sockaddr_in bname;
    set_in(bname, loc_ip, loc_port);
    int err = bind_in(fd0, bname);
    if (err < 0) {
        cout << "tcp_open: could not bind TCP port " << loc_port << endl;
        perror("bind");
        ::close(fd0);
        return eBIND_FAILED;
        }

    // Put the socket into non-blocking mode (before connecting!!!):
#if defined(SOLARIS) || defined(linux)
    err = fcntl(fd0, F_SETFL, (fcntl(fd0, F_GETFL, 0) | O_NONBLOCK));
    if (err < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("fcntl");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#elif defined(WIN32)
    unsigned long x = true;
    if (ioctlsocket(fd0, FIONBIO, &x) < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("ioctlsocket");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#else
    int x = true;
    if (ioctl(fd0, FIONBIO, &x) < 0) {
        cout << "tcp_open: error setting socket non-blocking mode." << endl;
        perror("ioctl");
        ::close(fd0);
        return eNONBLOCKING_FAILED;
        }
#endif

    // Put the socket into passive mode, using listen():
    struct sockaddr_in cname;
    set_in(cname, rem_ip, rem_port);
    err = connect_in(fd0, cname);
    if (err < 0) {
#ifndef WIN32
        if (errno == EINPROGRESS) {
#else
        if (errno == WSAEINPROGRESS) {
#endif
            // In this case, must register a handler to deal with this!!!
//            cout << flush;
//            perror("connect");
            return eIN_PROGRESS;
            }
        cout << "tcp_open: error while connecting putting TCP port."
             << endl;
        perror("connect");
        ::close(fd0);
        return eCONNECT_FAILED;
        }

    // Return the fd for the control port fd:
    return fd0;
    } // End of function tcp_open.

/*------------------------------------------------------------------------------
This reattempts opening a TCP port in the active mode.
"rem_port" and "rem_ip" should be the original values.
Returns:
fd >= 0             file descriptor (success)
eIN_PROGRESS        must wait for completion
eBAD_ARGUMENT       bad argument
eCONNECT_FAILED     serious error in connect() call.
------------------------------------------------------------------------------*/
//----------------------//
//      tcp_reopen      //
//----------------------//
int tcp_reopen(int fd0, uint16 rem_port, uint32 rem_ip) {
    if (fd0 < 0) {
        cout << "tcp_reopen: negative file descriptor.\n";
        return eBAD_ARGUMENT;
        }

    // Put the socket into passive mode, using listen():
    struct sockaddr_in cname;
    set_in(cname, rem_ip, rem_port);
    int err = connect_in(fd0, cname);
    if (err < 0) {
#ifndef WIN32
        if (errno == EINPROGRESS || errno == EALREADY) {
#else
        if (errno == WSAEINPROGRESS || errno == WSAEALREADY) {
#endif
            // In this case, must register a handler to deal with this!!!
//            cout << flush;
//            perror("connect");
            return eIN_PROGRESS;
            }
#ifndef WIN32
        if (errno == EISCONN) {
#else
        if (errno == WSAEISCONN) {
#endif
            // In this case, must register a handler to deal with this!!!
//            cout << flush;
//            perror("connect");
            return fd0;
            }
        cout << "tcp_reopen: error putting TCP port into active mode."
             << endl;
        perror("connect");
        ::close(fd0);
        return eCONNECT_FAILED;
        }

    // Return the fd for the control port fd:
    return err;
    } // End of function tcp_reopen.

/*------------------------------------------------------------------------------
If a full IP header is not present, then nothing is altered.
If a reset is performed, then the header checksum is recalculated.
------------------------------------------------------------------------------*/
//----------------------//
//   reset_ip_src_dst   //
//----------------------//
int reset_ip_src_dst(nbytes& buf, uint32 src_ip, uint32 dst_ip) {
    if (buf.n_bytes() < 20)
        return -1;

    // Look up the IP header length:
    uint8 x8 = uint8(buf[0]);
    int IHL_bytes = (x8 & 0x0f) * 4;
    if (buf.n_bytes() < IHL_bytes)
        return -2;

    // Modify the addresses:
    buf.set32(12, src_ip);
    buf.set32(16, dst_ip);

    // Clear the old header checksum:
    buf.set16(10, 0);

    // Calculate the header checksum:
    uint16 hcs_calc = ~u16checksum_addx(buf.bytes(), IHL_bytes);

    // Insert the new header checksum:
    buf.set16(10, hcs_calc);
    return 0;
    } // End of function reset_ip_src_dst.

/*------------------------------------------------------------------------------
If a full IP and TCP header are not present, then nothing is altered.
If a reset is performed, then the TCP header checksum is recalculated.
------------------------------------------------------------------------------*/
//----------------------//
//   reset_tcp_src_dst  //
//----------------------//
int reset_tcp_src_dst(nbytes& buf, uint16 src_port, uint16 dst_port) {
    // Return if buffer is shorter than minimal IP + TCP headers:
    if (buf.n_bytes() < 40)
        return -1;
    const char* pc = buf.bytes();

    // Look up the IP header length:
    uint8 x8 = uint8(pc[0]);
//    cout << "buf[0] = 0x" << hex2(buf[0]) << "\n";
    int IHL_bytes = (x8 & 0x0f) * 4;

    // Return if buffer is shorter than IP header + minimal TCP header:
    if (buf.n_bytes() < IHL_bytes + 20 || IHL_bytes < 20)
        return -2;

    // Modify the TCP ports:
    buf.set16(IHL_bytes, src_port);
    buf.set16(IHL_bytes + 2, dst_port);

    // Clear the old header checksum:
    buf.set16(IHL_bytes + 16, 0);

    // Calculate the TCP header checksum:
    uint16 tcp_hcs_calc = u16checksum_addx(pc + 12, 8);

    // The protocol:
    uint8 protocol = pc[9];
    tcp_hcs_calc = u16checksum_fold(tcp_hcs_calc, uint16(protocol));

    // TCP packet length:
    uint16 tcp_length = buf.n_bytes() - IHL_bytes;
    tcp_hcs_calc = u16checksum_fold(tcp_hcs_calc, tcp_length);

    // The first 16 bytes of TCP header:
    tcp_hcs_calc = ~u16checksum_fold(tcp_hcs_calc,
                    u16checksum_addx(pc + IHL_bytes, tcp_length));

    // Insert the new header checksum:
    buf.set16(IHL_bytes + 16, tcp_hcs_calc);
    return 0;
    } // End of function reset_tcp_src_dst.

/*------------------------------------------------------------------------------
Calls the Unix accept() function for passive TCP socket fd1.
If the call succeeds, the calling TCP port is written to "from", and
the function return value is the non-negative fd for the new data socket.
Otherwise "from" is unaltered, and the function return value is negative.
------------------------------------------------------------------------------*/
//----------------------//
//       accept_in      //
//----------------------//
int accept_in(int fd1, sockaddr_in& from) {
    if (fd1 < 0)
        return eBAD_ARGUMENT;

    // Accept the data socket from the control socket:
    sockaddr cname;
#ifdef linux
    socklen_t cnamelen = sizeof(cname);
#else
    int cnamelen = sizeof(cname);
#endif
    int fd0 = ::accept(fd1, &cname, &cnamelen);
    if (fd0 < 0) {
        cout << "tcp_handler::handler: could not accept TCP socket."
             << endl;
        perror("accept");
        return eACCEPT_FAILED;
        }

    // Check that the peer is an IP socket:
    if (cnamelen != sizeof(from) || cname.sa_family != AF_INET) {
        cout << "accept_in(): received non-IP TCP connection.\n";
        ::close(fd0);
        return eACCEPT_FAILED;
        }

    // Make a copy of the peer address and fd:
    memcpy(&from, &cname, cnamelen);
    return fd0;
    } // End of function accept_in.

//----------------------//
//   tcp_packet::print  //
//----------------------//
void tcp_packet::print(ostream& os) {
    os << "TCP packet contents.\n";
    os << "n_bytes         = " << n_bytes << NL;
    os << "status          = " << status << NL;
    os << "src_port        = " << int(src_port) << NL;
    os << "dst_port        = " << int(dst_port) << NL;
    os << "seq_no          = " << seq_no << NL;
    os << "ack_no          = " << ack_no << NL;
    os << "data_offset     = " << int(data_offset) << NL;
    os << "res0            = 0x" << hex2(res0) << NL;
    os << "flags           = 0x" << hex2(flags) << NL;
    os << "window          = " << int(window) << NL;
    os << "checksum        = 0x" << hex4(checksum) << NL;
    os << "urgent_pointer  = " << int(urgent_pointer) << NL;
    if (payload.empty())
        os << "no data payload\n";
    else {
        os << "payload (" << payload.n_bytes() << " bytes):\n";
        hex_print(payload, os);
        }
    os << "urg             = " << bool_string(urg) << NL;
    os << "ack             = " << bool_string(ack) << NL;
    os << "psh             = " << bool_string(psh) << NL;
    os << "rst             = " << bool_string(rst) << NL;
    os << "syn             = " << bool_string(syn) << NL;
    os << "fin             = " << bool_string(fin) << NL;
//    os << "dcs_calc        = 0x" << hex4(dcs_calc) << NL;
//    os << "dcs_syndrome    = 0x" << hex4(dcs_syndrome) << NL;
    os << flush;
    } // End of function tcp_packet::print.

//----------------------//
// ip_packet::ip_packet //
//----------------------//
ip_packet::ip_packet() {
    // Basic status variables:
    n_bytes = 0;
    status = 0;

    // Directly parsed fields:
    version = 0;
    IHL = 0;
    type_of_service = 0;
    total_length = 0;
    id = 0;
    flags = 0;
    fragment_offset = 0;
    time_to_live = 0;
    protocol = 0;
    header_checksum = 0;
    source = 0;
    destination = 0;

    // Indirectly parsed fields.
    precedence = 0;
    low_delay = false;
    high_throughput = false;
    high_reliability = false;
    dont_fragment = false;
    more_fragments = false;

    // Header checksum things:
    hcs_calc = 0;
    hcs_syndrome = 0;

    tcp = 0;
    } // End of function ip_packet::ip_packet.

//------------------------------//
//  ip_packet::decode_options   //
//------------------------------//
int ip_packet::decode_options(const char* pc, int n) {
    if (!pc || n <= 0)
        return -1;

    bool_enum found_error = false;
    while (!found_error && n > 0) {
        ip_option* po1 = new ip_option;
        po1->type = uint8(*pc++);
        n -= 1;
        po1->optclass = (po1->type >> 5) & 0x03;
        po1->number = po1->type & 0x1f;

        switch(po1->optclass) {
        case 0:             // Class: Control.
            switch(po1->number) {
            case 0:         // 1 byte. End of option list.
                n = 0;      // Terminate parsing of options.
                break;
            case 1:         // 1 byte. No operation.
                            // Just ignore it.
                break;
            case 2:         // 11 bytes. Security.
                if (n < 10) {
                    cout << "Security option too short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n >= 1)
                        po1->length = uint8(*pc++);
                    n -= 1;
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy all fields:
                po1->length = uint8(*pc++);
                if (po1->length != 11) {
                    cout << "IP security option length field error.\n";
                    found_error = true;
                    }
                po1->data.copy_from(pc, 9);
                pc += 9;
                n -= 10;
                break;
            case 3:         // Variable bytes. Loose source routing.
                if (n < 1) {
                    cout << "Loose source routing option short in IP packet.\n";
                    found_error = true;
                    break;
                    }
                po1->length = uint8(*pc++);
                n -= 1;
                if (n < po1->length - 2) {
                    cout << "Loose source routing option short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy the data.
                po1->data.copy_from(pc, po1->length - 2);
                pc += po1->length - 2;
                n -= po1->length - 2;
                break;
            case 7:         // Variable bytes. Record route.
                if (n < 1) {
                    cout << "Record route option short in IP packet.\n";
                    found_error = true;
                    break;
                    }
                po1->length = uint8(*pc++);
                n -= 1;
                if (n < po1->length - 2) {
                    cout << "Record route option short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy the data.
                po1->data.copy_from(pc, po1->length - 2);
                pc += po1->length - 2;
                n -= po1->length - 2;
                break;
            case 8:         // 4 bytes. Stream identifier.
                if (n < 3) {
                    cout <<"Stream identifier option too short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n >= 1)
                        po1->length = uint8(*pc++);
                    n -= 1;
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy all fields:
                po1->length = uint8(*pc++);
                if (po1->length != 4) {
                    cout << "Stream identifier option length field error.\n";
                    found_error = true;
                    }
                po1->data.copy_from(pc, 2);
                pc += 2;
                n -= 3;
                break;
            case 9:         // Variable bytes. Strict source routing.
                if (n < 1) {
                    cout <<"Strict source routing option short in IP packet.\n";
                    found_error = true;
                    break;
                    }
                po1->length = uint8(*pc++);
                n -= 1;
                if (n < po1->length - 2) {
                    cout <<"Strict source routing option short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy the data.
                po1->data.copy_from(pc, po1->length - 2);
                pc += po1->length - 2;
                n -= po1->length - 2;
                break;
            default:
                cout << "Unrecognised control option in IP packet.\n";
                found_error = true;
                break;
                } // End of switch(number).
            break;
        case 2:             // Class: Debugging and measurement.
            switch(po1->number) {
            case 4:         // Variable bytes. Internet timestamp.
                if (n < 1) {
                    cout << "Internet timestamp option short in IP packet.\n";
                    found_error = true;
                    break;
                    }
                po1->length = uint8(*pc++);
                n -= 1;
                if (n < po1->length - 2) {
                    cout << "Internet timestamp option short in IP packet.\n";
                    found_error = true;

                    // Copy as much of the option as possible:
                    if (n > 0) {
                        po1->data.copy_from(pc, n);
                        pc += n;
                        n = 0;
                        }
                    break;
                    }
                // Copy the data.
                po1->data.copy_from(pc, po1->length - 2);
                pc += po1->length - 2;
                n -= po1->length - 2;
                break;
            default:
                cout << "Unrecognised debug/measmt option in IP packet.\n";
                found_error = true;
                break;
                } // End of switch(number).
            break;
        case 1:             // Class: Reserved classes.
        case 3:
            cout << "Found reserved option class in IP packet.\n";
            found_error = true;
            break;
            } // End of switch(optclass).

        options.append(po1);
        } // End of while (!found_error && n > 0).

    return 0;
    } // End of function ip_packet::decode_options.

/*------------------------------------------------------------------------------
This function decodes n bytes as an IP packet.
If the interpretation is successful, 0 is returned.
Otherwise -1 is returned. The "status" field indicates the type of error that
occurred, if any. If parsing is successful, "status" is set to 1.
Therefore "status" is never equal to 0 after this function has been called.
Status codes:
1       Successfully parsed. All parsed fields are valid.
0       Nothing has been parsed.
-1      Null char* offered in packet buffer. No fields parsed.
-2      Zero-length packet buffer offered. No fields parsed.
-3      Packet is not IP version 4.
-4      Offered packet is shorter than the indicated header length.
-5      IHL < 5.
-6      Header checksum failed.
------------------------------------------------------------------------------*/
//----------------------//
//  ip_packet::decode   //
//----------------------//
int ip_packet::decode(const nbytes& buf) {
    // Clear out some things:
    delete tcp;
    tcp = 0;
    options.clear();

    const char* pc = buf.bytes();
    if (!pc) {
        n_bytes = 0;
        status = -1;
        return -1;
        }
    n_bytes = buf.n_bytes();
    if (n_bytes <= 0) {
        status = -2;
        return -1;
        }

    // Since there is at least 1 byte in the packet, parse the first byte:
    uint8 x8 = uint8(pc[0]);
    version = (x8 >> 4) & 0x0f;

    // Only decode version 4 packets:
    if (version != 4) {
        status = -3;
        return -1;
        }

    // Parse the Internet Header Length (in units of quad-bytes):
    IHL = x8 & 0x0f;

    // Check that the offered bytes include at least the header:
    if (n_bytes < IHL * 4) {
        status = -4;
        return -1;
        }

    // Check that IHL >= 5:
    if (IHL < 5) {
        status = -5;
        return -1;
        }

    // Now we know that we have enough bytes for the whole header.
    type_of_service = (uint8)pc[1];
    total_length = u16decode(pc + 2);
    id = u16decode(pc + 4);
    uint16 x16 = u16decode(pc + 6);
    flags = (x16 >> 13) & 0x07;
    fragment_offset = x16 & 0x1fff;
    time_to_live = (uint8)pc[8];
    protocol = (uint8)pc[9];
    header_checksum = u16decode(pc + 10);
    source = u32decode(pc + 12);            // Stored in host order.
    destination = u32decode(pc + 16);       // Stored in host order.

    // Decode the options:
    if (IHL > 5)
        decode_options(pc + 20, IHL * 4 - 20);

    // Copy the payload:
    payload.copy_from(pc + IHL * 4, n_bytes - IHL * 4);

    // Do the indirect parsing:
    precedence = (type_of_service >> 5) & 0x07;
    low_delay = (type_of_service >> 4) & 0x01;
    high_throughput = (type_of_service >> 3) & 0x01;
    high_reliability = (type_of_service >> 3) & 0x01;
    dont_fragment = (flags >> 1) & 0x01;
    more_fragments = flags & 0x01;

    // Run a 16-bit checksum over the whole header:
    // [Warning: probably should really convert 0xffff to 0 upon return
    // from u16checksum functions!!!! What is the correct thing to do?]
    hcs_calc = ~u16checksum_fold(u16checksum_addx(pc, 10),
                                 u16checksum_addx(pc + 12, IHL * 4 - 12));
    hcs_syndrome = u16checksum_addx(pc, IHL * 4);
    if (header_checksum != 0 && hcs_syndrome != 0) {
        status = -6;
        return -1;
        }

    // Next try to decode the payload of the packet:
    // [Warning:  this should not work if the IP packet is fragmented!!!]
    switch(protocol) {
    case 1: {   // ICMP.

        }
        break;
    case 6: {   // TCP.
        // Status codes:
        // 1    successful reading of packet.
        // 0    nothing read.
        // -1   packet shorter than a minimum TCP header.
        // -2
        // Note: tcp is cleared at the beginning of decode().
        const char* pc2 = payload.bytes();
        tcp = new tcp_packet;
        tcp->n_bytes = payload.n_bytes();
        if (!pc2 || tcp->n_bytes < 20) {
            tcp->status = -1;
            break;
            }
        tcp->src_port = u16decode(pc2);         // Stored in host order.
        tcp->dst_port = u16decode(pc2 + 2);     // Stored in host order.
        tcp->seq_no = u32decode(pc2 + 4);
        tcp->ack_no = u32decode(pc2 + 8);
        uint16 z16 = u16decode(pc2 + 12);
        tcp->data_offset = (z16 >> 12) & 0x0f;
        tcp->res0 = (z16 >> 6) & 0x3f;
        tcp->flags = z16 & 0x3f;
        tcp->urg = (tcp->flags >> 5) & 0x01;
        tcp->ack = (tcp->flags >> 4) & 0x01;
        tcp->psh = (tcp->flags >> 3) & 0x01;
        tcp->rst = (tcp->flags >> 2) & 0x01;
        tcp->syn = (tcp->flags >> 1) & 0x01;
        tcp->fin = tcp->flags & 0x01;
        tcp->window = u16decode(pc2 + 14);
        tcp->checksum = u16decode(pc2 + 16);
        tcp->urgent_pointer = u16decode(pc2 + 18);

        // Check is there is enough space for the options:
        if (tcp->n_bytes < tcp->data_offset * 4) {
            tcp->status = -2;
            break;
            }
        tcp->payload.copy_from(pc2 + tcp->data_offset * 4,
                          tcp->n_bytes - tcp->data_offset * 4);

        // Check the TCP header checksum:
        // IP src and dst addresses:
        uint16 tcp_hcs_calc = u16checksum_addx(pc + 12, 8);

        // The protocol:
        tcp_hcs_calc = u16checksum_fold(tcp_hcs_calc, uint16(protocol));

        // TCP packet length:
        uint16 tcp_length = payload.n_bytes();
        tcp_hcs_calc = u16checksum_fold(tcp_hcs_calc, tcp_length);

        // The first 16 bytes of TCP header:
        tcp_hcs_calc = u16checksum_fold(tcp_hcs_calc,
                                        u16checksum_addx(pc2, 16));

        // Rest of TCP header + the rest of the TCP packet.
        tcp_hcs_calc = ~u16checksum_fold(tcp_hcs_calc,
                        u16checksum_addx(pc2 + 18, tcp_length - 18));

        if (tcp->checksum != tcp_hcs_calc) {
            tcp->status = -3;
            break;
            }
        tcp->status = 1;
        }
        break;
    case 17: {  // UDP.

        }
        break;
        } // End of switch(protocol).

    // Packet is successfully parsed:
    status = 1;
    return 0;
    } // End of function ip_packet::decode.

//----------------------//
//   ip_packet::print   //
//----------------------//
void ip_packet::print(ostream& os) {
    os << "IP packet contents.\n";
    os << "n_bytes         = " << n_bytes << NL;
    os << "status          = " << status << NL;
    os << "version         = " << int(version) << NL;
    os << "IHL             = " << int(IHL) << NL;
    os << "type_of_service = 0x" << hex2(type_of_service) << NL;
    os << "total_length    = " << int(total_length) << NL;
    os << "id              = 0x" << hex4(id) << NL;
    os << "flags           = " << int(flags) << NL;
    os << "fragment_offset = " << int(fragment_offset) << NL;
    os << "time_to_live    = " << int(time_to_live) << NL;
    os << "protocol        = " << int(protocol)
       << " (" << ip_protocol_string[protocol & 0xff] << ")\n";
    os << "header_checksum = 0x" << hex4(header_checksum) << NL;
    os << "source          = " << ipstring(source) << NL;
    os << "destination     = " << ipstring(destination) << NL;
    os << "payload:\n";
    hex_print(payload, os);

    os << "precedence      = " << int(precedence) << NL;
    os << "low_delay       = " << bool_string(low_delay) << NL;
    os << "high_throughput = " << bool_string(high_throughput) << NL;
    os << "high_reliability= " << bool_string(high_reliability) << NL;
    os << "dont_fragment   = " << bool_string(dont_fragment) << NL;
    os << "more_fragments  = " << bool_string(more_fragments) << NL;
    os << "hcs_calc        = 0x" << hex4(hcs_calc) << NL;
    os << "hcs_syndrome    = 0x" << hex4(hcs_syndrome) << NL;
    if (tcp)
        tcp->print(os);
    os << flush;
    } // End of function ip_packet::print.

/*------------------------------------------------------------------------------
The difference between this function and nbytes::read is the assumption that the
"fd" refers to a UDP (or other) socket.
This will block if there's no data to read!
If a "bufsize" is specified which is positive and not equal to nbytes_bufsize,
then a different size buffer is used. If it is smaller, then ::read is called
with a smaller bufsize parameter. If bufsize is larger than nbytes_bufsize, then
a temporary buffer is created of that size, and this buffer is deleted
afterwards.
------------------------------------------------------------------------------*/
//----------------------//
// nbytes_from::recvfrom//
//----------------------//
int nbytes_from::recvfrom(int fd, int bufsize) {
    // Ignore silly arguments:
    if (fd < 0 || bufsize < 0) {
        clear();
        return -1;
        }

    // Try to read some bytes:
    int rval = 0;
    char* buf = nbytes_buf;
    fromlen = sizeof(from);     // Must initialise fromlen every time.
    const int flags = 0;
    if (bufsize > 0) {
        if (bufsize <= nbytes_bufsize)
            rval = ::recvfrom(fd, nbytes_buf, bufsize, flags, &from, &fromlen);
        else {
            // Create a temporary larger buffer:
            buf = new char[bufsize];
            rval = ::recvfrom(fd, buf, bufsize, flags, &from, &fromlen);
            }
        }
    else {
        rval = ::recvfrom(fd, nbytes_buf, nbytes_bufsize,
                          flags, &from, &fromlen);
        }

    // Copy the bytes to the (pc, n) pair:
    if (rval > 0) {
        delete pc;
        pc = new char[rval];
        memcpy(pc, buf, rval);
        n = rval;
        }
    else
        clear();

    // Delete the temporary buffer, if non-standard size was used:
    if (buf != nbytes_buf)
        delete buf;

    return rval;
    } // End of function nbytes_from::recvfrom.

/*------------------------------------------------------------------------------
Warning: The requirement for the loc_port to be positive has been relaxed here.
However, if this results in the kernel deciding on the real port number, this
will completely upset the method of checking to see that the port has not been
previously opened. Hence if you use this function (and this class, in fact) with
the port number set to 0, you should expect unintended consequences!
------------------------------------------------------------------------------*/
//----------------------//
//  udp_port_set::open  //
//----------------------//
udp_port* udp_port_set::open(uint16 loc_port, uint32 loc_ip) {
    // See if the port number has been used before:
    udp_port* p0 = 0;
    forall(p0, udp_ports)
        if (p0->port() == loc_port)
            break;

    // If the UDP port number has been used previously...
    if (p0) {
        // If it's already open, return the descriptor for it:
        if (p0->fd() >= 0)
            return p0;

        // If it is not open, try to open it:
        p0->fd0 = udp_open(loc_port, 1, loc_ip);
        return p0;
        }

    // Start a new record for the UDP port, and open the port:
    p0 = new udp_port;
    p0->port0 = loc_port;
    p0->ip0 = loc_ip;
    p0->fd0 = udp_open(loc_port, 1, loc_ip);
    udp_ports.append(p0);

    return p0;
    } // End of function udp_port_set::open.

/*------------------------------------------------------------------------------
This deletes any ip_map_info structure which is stored for the given key, and
then substitutes the given ip_map_info pointer.
------------------------------------------------------------------------------*/
//--------------------------//
// ip_map_table::insert_map //
//--------------------------//
void ip_map_table::insert_map(ip_if* sender, ip_map_info* p) {
    void* pv = 0;
    if (maps.find(pv, sender) && pv)
        delete (ip_map_info*)pv;
    maps.insert(p, sender);
    } // End of function ip_map_table::insert_map.

/*------------------------------------------------------------------------------
This clears the hash table entry to 0, and deletes the ip_map_info
structure which is stored for the given key.
------------------------------------------------------------------------------*/
//--------------------------//
//  ip_map_table::clear_map //
//--------------------------//
void ip_map_table::clear_map(ip_if* sender) {
    void* pv = 0;
    if (!maps.find(pv, sender) || !pv)
        return;
    delete (ip_map_info*)pv;
    maps.insert(0, sender);
    } // End of function ip_map_table::clear_map.

//--------------------------//
// ip_map_table::print_maps //
//--------------------------//
void ip_map_table::print_maps(ostream& os) {
    if (n_maps() <= 0)
        return;

    os << "Contents of map table:\n";
    os << "Format: sender (address) -> recipient (address)"
          " {swap_ip swap_tcp} [hash keys]:\n";
    int i;
    for (voidptrkey* p = maps.first(&i); p; p = maps.next(&i)) {
        ip_if* A = (ip_if*)p->key;
        ip_map_info* pi = (ip_map_info*)p->data;
        ip_if* B = pi ? pi->recipient : 0;
        os << "    " << (A ? (const char*)A->name : "[no type]");
        os << " (0x" << hex8(long(A)) << ") -> ";
        os << (B ? (const char*)B->name : "[no type]");
        os << " (0x" << hex8(long(B)) << ") ";
        if (pi) {
            os << "{" << be_string(pi->swap_ip)
               << " " << be_string(pi->swap_tcp) << "}";
            }
        os << " [";
        os << i << "]\n";
        }
    } // End of function ip_map_table::print_maps.

//----------------------//
//    ip_switch::smap   //
//----------------------//
int ip_switch::smap(ip_if* sender, ip_if* recipient) {
    if (!sender)
        return -1;

    // Insert data for key "sender":
    ip_map_info* p = new ip_map_info;
    p->recipient = recipient;
    insert_map(sender, p);

    // Then set the switch_pointer for the sender.
    sender->set_ip_switch(this);

    return 0;
    } // End of function ip_switch::smap.

//----------------------//
//    ip_switch::dmap   //
//----------------------//
int ip_switch::dmap(ip_if* sender, ip_if* recipient) {
    if (!sender || !recipient)
        return -1;

    // Insert data for key "sender":
    ip_map_info* p = new ip_map_info;
    p->recipient = recipient;
    insert_map(sender, p);              // sender -> recipient.
    sender->set_ip_switch(this);

    // Insert data for key "recipient":
    if (recipient != sender) {
        p = new ip_map_info;
        p->recipient = sender;
        insert_map(recipient, p);       // recipient -> sender.
        recipient->set_ip_switch(this);
        }

    return 0;
    } // End of function ip_switch::dmap.

/*------------------------------------------------------------------------------
A TCP mirror can either land back at the same IP interface, or a different IP
interface. If it is the same, then there is no need to specify the recipient.
An unspecified recipient is taken to mean that it is the same as the sender.
------------------------------------------------------------------------------*/
//----------------------//
// ip_switch::tcp_mirror//
//----------------------//
int ip_switch::tcp_mirror(ip_if* sender, ip_if* recipient) {
    if (!sender)
        return -1;

    // Default recipient is the sender:
    if (!recipient)
        recipient = sender;

    // Insert data for key "sender":
    ip_map_info* p = new ip_map_info;
    p->recipient = recipient;
    p->swap_ip = true;
//    p->swap_tcp = true;
    insert_map(sender, p);              // sender -> recipient.
    sender->set_ip_switch(this);

    // Insert (with over-write) data for key "recipient":
    if (recipient != sender) {
        p = new ip_map_info;
        p->recipient = sender;
        p->swap_ip = true;
//        p->swap_tcp = true;
        insert_map(recipient, p);       // recipient -> sender.
        recipient->set_ip_switch(this);
        }

    return 0;
    } // End of function ip_switch::tcp_mirror.

//------------------------------//
//  ip_switch::send_to_switch   //
//------------------------------//
int ip_switch::send_to_switch(ip_if* sender, const nbytes& buf) {
    if (!sender)
        return -1;

    if (buf.empty())
        return -2;

    // Find out who is supposed to get the bytes:
    ip_map_info* p = find_map(sender);

    // If the sender was not in the map table, return error code.
    if (!p)
        return -3;

    // Send the bytes to the mapped recipient:
    if (!p->recipient)
        return -4;

    // Then possibly swap the IP addresses and/or TCP port numbers.
    if (p->swap_ip) {
        nbytes buf2 = buf;
        buf2.swap_ip();
        p->recipient->send_from_switch(sender, buf2);
        }
    else
        p->recipient->send_from_switch(sender, buf);

    return 0;
    } // End of function ip_switch::send_to_switch.

//----------------------//
//   ip_switch::print   //
//----------------------//
void ip_switch::print(ostream& os) {
    os << "IP switch \"" << name << "\": " << n_maps()
       <<  " maps in map table.\n";
    print_maps(os);
    } // End of function ip_switch::print.
