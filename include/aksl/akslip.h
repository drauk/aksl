// src/aksl/akslip.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_AKSLIP_H
#define AKSL_AKSLIP_H

/*------------------------------------------------------------------------------
Classes in this file:

tcp_packet::
tcp_packetlist::
ip_option::
ip_optionlist::
ip_packet::
ip_packetlist::
nbytes_from::
udp_port::
udp_portlist::
udp_port_set::
icmp_sock::
ip_if::
ip_iflist::
ip_map_info::
ip_map_table::
ip_switch::
ip_switchlist::

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inline (external) functions in this file:

sendto
set_in
reset_in
clear_in
equal_in
bind_in
connect_in
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_ERROR_H
#include "aksl/error.h"
#endif
#ifndef AKSL_NBYTES_H
#include "aksl/nbytes.h"
#endif
#ifndef AKSL_STR_H
#include "aksl/str.h"
#endif
#ifndef AKSL_HASHFN_H
#include "aksl/hashfn.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// System header files:
// For netinet/in.h:
#ifndef AKSL_X_SYS_TYPES_H
#define AKSL_X_SYS_TYPES_H
#include <sys/types.h>
#endif

// For ntohl() and struct sockaddr_in.
#ifdef WIN32
#ifndef AKSL_X_WINSOCK_H
#define AKSL_X_WINSOCK_H
#include <winsock.h>
#endif
#endif

#ifndef WIN32
#ifndef AKSL_X_NETINET_IN_H
#define AKSL_X_NETINET_IN_H
#include <netinet/in.h>
#endif

// For socket(), AF_INET:
#ifndef AKSL_X_SYS_SOCKET_H
#define AKSL_X_SYS_SOCKET_H
#include <sys/socket.h>
#endif

// For struct hostent:
#ifndef AKSL_X_NETDB_H
#define AKSL_X_NETDB_H
#include <netdb.h>
#endif
#endif /* not WIN32 */

// For ioctl(), close():
#if !defined(linux) && !defined(WIN32)
#ifndef AKSL_X_SYSENT_H
#define AKSL_X_SYSENT_H
#include <sysent.h>
#endif
#endif

const int IP_BUFSIZE = 65536;   // Maximum size of UDP packets. [Or bigger?]

/*------------------------------------------------------------------------------
RFC 793: "Transmission control protocol", Jon Postel, September 1981.
------------------------------------------------------------------------------*/
//----------------------//
//      tcp_packet::    //
//----------------------//
struct tcp_packet: public slink {
    int n_bytes;            // Number of bytes in the whole packet.
    int status;             // Negative if there's an error in the packet.
                            // Equal to 0 if no packet parsed.
                            // Equal to 1 if packet successfully parsed.

    // Directly parsed fields:
    uint16 src_port;        // Source TCP port. 16 bits.
    uint16 dst_port;        // Destination TCP port. 16 bits.
    uint32 seq_no;          // Sequence number. 32 bits.
    uint32 ack_no;          // Acknowledgement number. 32 bits.
    uint8 data_offset;      // Data offset. 4 bits.
    uint8 res0;             // Reserved bits. 6 bits.
    uint8 flags;            // Urg, ack, psh, rst, syn, fin. 6 bits.
    uint16 window;          // 16 bits.
    uint16 checksum;        // 16 bits.
    uint16 urgent_pointer;  // 16 bits.
//    tcp_optionlist tcp_options;

    nbytes payload;

    // Indirectly parsed fields:
    bool_int urg;
    bool_int ack;
    bool_int psh;
    bool_int rst;
    bool_int syn;
    bool_int fin;

    tcp_packet* next() const { return (tcp_packet*)slink::next(); }

    void print(ostream& = cout);

//    tcp_packet& operator=(const tcp_packet& x) {}
//    tcp_packet(const tcp_packet& x) {}
    tcp_packet() {
        n_bytes = 0;
        status = 0;
        src_port = 0;
        dst_port = 0;
        seq_no = 0;
        ack_no = 0;
        data_offset = 0;
        res0 = 0;
        flags = 0;
        window = 0;
        checksum = 0;
        urgent_pointer = 0;
        urg = false;
        ack = false;
        psh = false;
        rst = false;
        syn = false;
        fin = false;
        }
    ~tcp_packet() {}
    }; // End of struct tcp_packet.

//----------------------//
//    tcp_packetlist::  //
//----------------------//
struct tcp_packetlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    tcp_packet* first() const { return (tcp_packet*)s2list::first(); }
    tcp_packet* last() const { return (tcp_packet*)s2list::last(); }
    tcp_packet* element(long i) const
        { return (tcp_packet*)s2list::element(i); }
    void append(tcp_packet* p) { s2list::append(p); }
    void prepend(tcp_packet* p) { s2list::prepend(p); }
    tcp_packet* popfirst() { return (tcp_packet*)s2list::popfirst(); }
    tcp_packet* poplast() { return (tcp_packet*)s2list::poplast(); }
    tcp_packet* remove(tcp_packet* p)
        { return (tcp_packet*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(tcp_packet* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(tcp_packetlist& l) { s2list::swallow(&l); }
    void gulp(tcp_packetlist& l) { s2list::gulp(&l); }
    void clear() { for (tcp_packet* p = first(); p; )
        { tcp_packet* q = p->next(); delete p; p = q; } clearptrs(); }

//    tcp_packetlist& operator=(const tcp_packetlist& x) {}
//    tcp_packetlist(const tcp_packetlist& x) {}
    tcp_packetlist() {}
    ~tcp_packetlist() { clear(); }
    }; // End of struct tcp_packetlist.

//----------------------//
//      ip_option::     //
//----------------------//
struct ip_option: public slink {
    // Directly parsed fields:
    uint8       type;
    uint8       length;
    nbytes      data;

    // Indirectly parsed fields:
    bool_int    copied;                 // True if to be copied to IP fragments.
    uint8       optclass;               // 2 bits. Option class.
    uint8       number;                 // 5 bits. Option number.

    ip_option* next() const { return (ip_option*)slink::next(); }

//    ip_option& operator=(const ip_option& x) {}
//    ip_option(const ip_option& x) {}
    ip_option() {
        type = 0;
        length = 0;
        copied = false;
        optclass = 0;                   // 0 = control. 2 = degugging, measmt.
        number = 0;
        }
    ~ip_option() {}
    }; // End of struct ip_option.

//----------------------//
//     ip_optionlist::  //
//----------------------//
struct ip_optionlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    ip_option* first() const { return (ip_option*)s2list::first(); }
    ip_option* last() const { return (ip_option*)s2list::last(); }
    ip_option* element(long i) const
        { return (ip_option*)s2list::element(i); }
    void append(ip_option* p) { s2list::append(p); }
    void prepend(ip_option* p) { s2list::prepend(p); }
    ip_option* popfirst() { return (ip_option*)s2list::popfirst(); }
    ip_option* poplast() { return (ip_option*)s2list::poplast(); }
    ip_option* remove(ip_option* p)
        { return (ip_option*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(ip_option* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(ip_optionlist& l) { s2list::swallow(&l); }
    void gulp(ip_optionlist& l) { s2list::gulp(&l); }
    void clear() { for (ip_option* p = first(); p; )
        { ip_option* q = p->next(); delete p; p = q; } clearptrs(); }

//    ip_optionlist& operator=(const ip_optionlist& x) {}
//    ip_optionlist(const ip_optionlist& x) {}
    ip_optionlist() {}
    ~ip_optionlist() { clear(); }
    }; // End of struct ip_optionlist.

/*------------------------------------------------------------------------------
This is a class for parsing IP version 4 packets.
RFC 791: "Internet protocol", Jon Postel, September 1981.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The "version" is set to a negative number to indicate that there is no packet.
"status" is set to 1 only if all fields in the structure are valid.
The "n_bytes" field is the actual number of bytes offered for parsing.
The "total_length" field is the value found inside the packet. These two should
be the same in a valid packet.
The "status" field if set to 1 if and only if the following conditions are met:
-   Length of offered packet >= 20 bytes.
-   IP protocol version field == 4.
-   Length of offered packet >= IHL * 4.
-   "total_length" field == length of offered packet.
-
-   ....
------------------------------------------------------------------------------*/
//----------------------//
//      ip_packet::     //
//----------------------//
struct ip_packet: public slink {
    int n_bytes;            // Number of bytes in the whole packet.
    int status;             // Negative if there's an error in the packet.
                            // Equal to 0 if no packet parsed.
                            // Equal to 1 if packet successfully parsed.
    // Directly parsed fields:
    uint8 version;          // IP protocol version. 4 bits.
    uint8 IHL;              // Internet Header Length: (quad-bytes). 4 bits.
    uint8 type_of_service;  // Precedence + flags. 8 bits.
    uint16 total_length;    // Length of packet incl. header (bytes). 16 bits.
    uint16 id;              // Identification. 16 bits.
    uint8 flags;            // 3 control flags.
    uint16 fragment_offset; // Offset of this fragment (oct-bytes).
    uint8 time_to_live;     // 8 bits.
    uint8 protocol;         // 8 bits.
    uint16 header_checksum; // 16 bits. [HCS.]
    uint32 source;          // Source IP address.
    uint32 destination;     // Destination IP address.
    ip_optionlist options;  // List of options.
    nbytes payload;         // The payload. ((n_bytes - IHL * 4) bytes.)

    // Indirectly parsed fields:
    uint8 precedence;           // 3 bits.
    bool_int low_delay;         // 1 bit.
    bool_int high_throughput;   // 1 bit.
    bool_int high_reliability;  // 1 bit.
    bool_int dont_fragment;     // 1 bit.
    bool_int more_fragments;    // 1 bit.

    uint16 hcs_calc;            // Calculated HCS from received header.
    uint16 hcs_syndrome;        // HCS syndrome from received header.

    tcp_packet* tcp;            // TCP structure, it this _is_ a TCP packet.

    ip_packet* next() const { return (ip_packet*)slink::next(); }

    int decode(const nbytes& buf);
    int decode_options(const char* pc, int n);
    void print(ostream& = cout);

//    ip_packet& operator=(const ip_packet& x) {}
//    ip_packet(const ip_packet& x) {}
    ip_packet();
    ~ip_packet() { delete tcp; }
    }; // End of struct ip_packet.

//----------------------//
//    ip_packetlist::   //
//----------------------//
struct ip_packetlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    ip_packet* first() const { return (ip_packet*)s2list::first(); }
    ip_packet* last() const { return (ip_packet*)s2list::last(); }
    ip_packet* element(long i) const
        { return (ip_packet*)s2list::element(i); }
    void append(ip_packet* p) { s2list::append(p); }
    void prepend(ip_packet* p) { s2list::prepend(p); }
    ip_packet* popfirst() { return (ip_packet*)s2list::popfirst(); }
    ip_packet* poplast() { return (ip_packet*)s2list::poplast(); }
    ip_packet* remove(ip_packet* p)
        { return (ip_packet*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(ip_packet* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(ip_packetlist& l) { s2list::swallow(&l); }
    void gulp(ip_packetlist& l) { s2list::gulp(&l); }
    void clear() { for (ip_packet* p = first(); p; )
        { ip_packet* q = p->next(); delete p; p = q; } clearptrs(); }

//    ip_packetlist& operator=(const ip_packetlist& x) {}
//    ip_packetlist(const ip_packetlist& x) {}
    ip_packetlist() {}
    ~ip_packetlist() { clear(); }
    }; // End of struct ip_packetlist.

/*------------------------------------------------------------------------------
This is not necessarily for use on UDP sockets, but is aimed at UDP sockets.
------------------------------------------------------------------------------*/
//----------------------//
//     nbytes_from::    //
//----------------------//
struct nbytes_from : public nbytes {
public:
    sockaddr    from;                   // Address of packet sender.
#ifdef linux
    socklen_t   fromlen;                // Length of real contents of "from".
#else
    int         fromlen;                // Length of real contents of "from".
#endif
public:
    int recvfrom(int fd, int bufsize = 0);  // Read from UDP (or other) socket.

//    nbytes_from& operator=(const nbytes_from& x) {}
//    nbytes_from(const nbytes_from& x) {}
    nbytes_from() { fromlen = 0; }
    ~nbytes_from() {}
    }; // End of struct nbytes_from.

/*------------------------------------------------------------------------------
Whether or not this port is open is indicated by fd0, which is non-negative
if and only if the port is open.
------------------------------------------------------------------------------*/
//----------------------//
//      udp_port::      //
//----------------------//
struct udp_port : public slink {
friend struct udp_port_set;
private:
    uint16      port0;          // Port number, host byte order.
    uint32      ip0;            // Local IP address.
    int         fd0;            // File descriptor for this port.
public:
    udp_port* next() { return (udp_port*)slink::next(); }

    int fd() { return fd0; }
    int port() { return port0; }
    void close() { if (fd0 > 0) { ::close(fd0); fd0 = -1; } }

//    udp_port& operator=(const udp_port& x) {}
//    udp_port(const udp_port& x) {}
    udp_port() {
        port0 = 0;
        ip0 = ntohl(long(INADDR_ANY));
        fd0 = -1;
        }
    ~udp_port() { close(); }
    }; // End of struct udp_port.

//----------------------//
//     udp_portlist::   //
//----------------------//
struct udp_portlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    udp_port* first() const { return (udp_port*)s2list::first(); }
    udp_port* last() const { return (udp_port*)s2list::last(); }
    udp_port* element(long i) const
        { return (udp_port*)s2list::element(i); }
    void append(udp_port* p) { s2list::append(p); }
    void prepend(udp_port* p) { s2list::prepend(p); }
    udp_port* popfirst() { return (udp_port*)s2list::popfirst(); }
    udp_port* poplast() { return (udp_port*)s2list::poplast(); }
    udp_port* remove(udp_port* p)
        { return (udp_port*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(udp_port* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(udp_portlist& l) { s2list::swallow(&l); }
    void gulp(udp_portlist& l) { s2list::gulp(&l); }
    void clear() { for (udp_port* p = first(); p; )
        { udp_port* q = p->next(); delete p; p = q; } clearptrs(); }

//    udp_portlist& operator=(const udp_portlist& x) {}
//    udp_portlist(const udp_portlist& x) {}
    udp_portlist() {}
    ~udp_portlist() { clear(); }
    }; // End of struct udp_portlist.

/*------------------------------------------------------------------------------
This is a class for allocating and caching udp ports. This enables multiple
classes/functions to allocate the same port somehow.
------------------------------------------------------------------------------*/
//----------------------//
//    udp_port_set::    //
//----------------------//
struct udp_port_set {
private:
    udp_portlist    udp_ports;
public:
    // Warning: should really have ntohl(long(INADDR_ANY)) here:
    udp_port* open(uint16 loc_port, uint32 loc_ip = INADDR_ANY);

//    udp_port_set& operator=(const udp_port_set& x) {}
//    udp_port_set(const udp_port_set& x) {}
    udp_port_set() {}
    ~udp_port_set() {}
    }; // End of struct udp_port_set.

/*------------------------------------------------------------------------------
Whether or not this socket is open is indicated by fd0, which is non-negative
if and only if the socket is open.
The original spec of ICMP was RFC 792.
On linux systems, type "man 7 raw" for info on ICMP sockets.
I don't think that this class is used. It's just a place-holder for
a future development.
------------------------------------------------------------------------------*/
//----------------------//
//      icmp_sock::     //
//----------------------//
struct icmp_sock : public slink {
// friend struct icmp_sock_set;
private:
    // Note: This sock0 variable doesn't really mean anything.
    uint16      sock0;          // Socket number, host byte order.
    uint32      ip0;            // Local IP address.
    int         fd0;            // File descriptor for this socket.
public:
    icmp_sock* next() { return (icmp_sock*)slink::next(); }

    int fd() { return fd0; }
    int sock() { return sock0; }
    void close() { if (fd0 > 0) { ::close(fd0); fd0 = -1; } }

//    icmp_sock& operator=(const icmp_sock& x) {}
//    icmp_sock(const icmp_sock& x) {}
    icmp_sock() {
        sock0 = 0;
        ip0 = ntohl(long(INADDR_ANY));
        fd0 = -1;
        }
    ~icmp_sock() { close(); }
    }; // End of struct icmp_sock.

/*------------------------------------------------------------------------------
This represents an interface that can send packets to an ip_switch or receive
packet from it.
------------------------------------------------------------------------------*/
//----------------------//
//        ip_if::       //
//----------------------//
struct ip_if: public slink {
private:
    struct ip_switch* sw;
protected:
    // For use by derived class:
    inline int send_to_switch(const nbytes& buf);
public:
    ip_if* next() const { return (ip_if*)slink::next(); }

    c_string name;          // For documentary purposes.

    // To be redefined in derived class:
    virtual int send_from_switch(ip_if* /*sender*/, const nbytes& /*buf*/)
        { return -1; }

    // Link the ip_if to an ip_switch:
    void set_ip_switch(ip_switch* p) { sw = p; }

//    ip_if& operator=(const ip_if& x) {}
//    ip_if(const ip_if& x) {}
    ip_if(ip_switch& x) { sw = &x; }
    ip_if() { sw = 0; }
    virtual ~ip_if() {}
    }; // End of struct ip_if.

//----------------------//
//      ip_iflist::     //
//----------------------//
struct ip_iflist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    ip_if* first() const { return (ip_if*)s2list::first(); }
    ip_if* last() const { return (ip_if*)s2list::last(); }
    ip_if* element(long i) const
        { return (ip_if*)s2list::element(i); }
    void append(ip_if* p) { s2list::append(p); }
    void prepend(ip_if* p) { s2list::prepend(p); }
    ip_if* popfirst() { return (ip_if*)s2list::popfirst(); }
    ip_if* poplast() { return (ip_if*)s2list::poplast(); }
    ip_if* remove(ip_if* p)
        { return (ip_if*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(ip_if* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(ip_iflist& l) { s2list::swallow(&l); }
    void gulp(ip_iflist& l) { s2list::gulp(&l); }
    void clear() { for (ip_if* p = first(); p; )
        { ip_if* q = p->next(); delete p; p = q; } clearptrs(); }

//    ip_iflist& operator=(const ip_iflist& x) {}
//    ip_iflist(const ip_iflist& x) {}
    ip_iflist() {}
    ~ip_iflist() { clear(); }
    }; // End of struct ip_iflist.

/*------------------------------------------------------------------------------
This is intended to be used in an address mapping table to store information
required for routing packets.
------------------------------------------------------------------------------*/
//----------------------//
//     ip_map_info::    //
//----------------------//
struct ip_map_info {
    ip_if*      recipient;          // Interface to recieve the IP packet.
    bool_enum   swap_ip;            // Swap src and dst IP addresses.
    bool_enum   swap_tcp;           // Swap src and dst TCP addresses.

//    ip_map_info& operator=(const ip_map_info& x) {}
//    ip_map_info(const ip_map_info& x) {}
    ip_map_info() { recipient = 0; swap_ip = false; swap_tcp = false; }
    ~ip_map_info() {}
    }; // End of struct ip_map_info.

/*------------------------------------------------------------------------------
This class manages a hash table which has ip_if pointers for keys, and
ip_map_info structures for data.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Warning:  When data is cleared, the ip_map_info structures are deleted.
Therefore the only access to these structures should be through this container
class. Otherwise accesses may be made to already deleted objects.
------------------------------------------------------------------------------*/
//----------------------//
//     ip_map_table::   //
//----------------------//
struct ip_map_table {
private:
    hashtab_32_8    maps;   // Hash table for mapping incoming interfaces.
public:
    ip_map_info* find_map(ip_if* sender) {
        void* pv = 0;
        return maps.find(pv, sender) ? (ip_map_info*)pv : 0;
        }
    void insert_map(ip_if* sender, ip_map_info* p);
    void clear_map(ip_if* sender);
    int n_maps() { return maps.n_entries(); }
    void print_maps(ostream& = cout);

//    ip_map_table& operator=(const ip_map_table& x) {}
//    ip_map_table(const ip_map_table& x) {}
    ip_map_table() {}
    ~ip_map_table() {}  // Should delete ip_map_info structures here.
    }; // End of struct ip_map_table.

/*------------------------------------------------------------------------------
This class is meant for gluing together the I/O event handlers from various
interfaces. It should accept IP packet payloads from any interface, determine
the correct outgoing interface, and send the packet on to that interface.
------------------------------------------------------------------------------*/
//----------------------//
//      ip_switch::     //
//----------------------//
struct ip_switch: public ip_map_table, public slink {
    c_string name;          // For documentary purposes.

    ip_switch* next() const { return (ip_switch*)slink::next(); }

    // Set up or read an address mapping:
    int smap(ip_if* sender, ip_if* recipient);      // Simplex.
    int dmap(ip_if* sender, ip_if* recipient);      // Duplex.
    int tcp_mirror(ip_if* sender, ip_if* recipient = 0);

    // This accepts bytes to be sent to another interface.
    int send_to_switch(ip_if* sender, const nbytes& buf);
    void print(ostream& = cout);

//    ip_switch& operator=(const ip_switch& x) {}
//    ip_switch(const ip_switch& x) {}
    ip_switch() {}
    ~ip_switch() {}
    }; // End of struct ip_switch.

//----------------------//
//    ip_switchlist::   //
//----------------------//
struct ip_switchlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    ip_switch* first() const { return (ip_switch*)s2list::first(); }
    ip_switch* last() const { return (ip_switch*)s2list::last(); }
    ip_switch* element(long i) const
        { return (ip_switch*)s2list::element(i); }
    void append(ip_switch* p) { s2list::append(p); }
    void prepend(ip_switch* p) { s2list::prepend(p); }
    ip_switch* popfirst() { return (ip_switch*)s2list::popfirst(); }
    ip_switch* poplast() { return (ip_switch*)s2list::poplast(); }
    ip_switch* remove(ip_switch* p)
        { return (ip_switch*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(ip_switch* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(ip_switchlist& l) { s2list::swallow(&l); }
    void gulp(ip_switchlist& l) { s2list::gulp(&l); }
    void clear() { for (ip_switch* p = first(); p; )
        { ip_switch* q = p->next(); delete p; p = q; } clearptrs(); }

//    ip_switchlist& operator=(const ip_switchlist& x) {}
//    ip_switchlist(const ip_switchlist& x) {}
    ip_switchlist() {}
    ~ip_switchlist() { clear(); }
    }; // End of struct ip_switchlist.

// Functions exported from this module:
extern void     print(hostent* pe, ostream& = cout);
extern uint32   hostname2ip(const char* hostname);
extern int      sendto(int fd_udp, const char* bytes, int n_bytes,
                       uint32 host, uint16 port);
extern int      sendto(int fd_udp, const nbytes& buf, const sockaddr_in& to);

// Warning: should really have ntohl(long(INADDR_ANY)) here:
extern int      udp_open(uint16& port1, int n_tries = 1,
                         uint32 loc_ip = INADDR_ANY);
extern int      udp_open(sockaddr_in& bname);

// Open passive TCP connection:
extern int tcp_open(uint16 loc_port,
                    uint32 loc_ip = INADDR_ANY, int backlog = 5);
// Open active TCP connection:
extern int tcp_open(uint16 rem_port, uint32 rem_ip,
                    uint16 loc_port, uint32 loc_ip, int& fd0);
// Re-open an active TCP connection:
extern int tcp_reopen(int fd, uint16 rem_port, uint32 rem_ip);

// These functions are supposed to modify IP packets and recalculate checksums:
extern int reset_ip_src_dst(nbytes& buf, uint32 src_ip, uint32 dst_ip);
extern int reset_tcp_src_dst(nbytes& buf, uint16 src_port, uint16 dst_port);

// Writes from-address to "from" and returns new TCP data socket.
extern int accept_in(int fd1, sockaddr_in& from);

// Inline member function with forward references:
//- - - - - - - - - - - //
//        ip_if::       //
//- - - - - - - - - - - //
inline int ip_if::send_to_switch(const nbytes& buf)
    { return sw ? sw->send_to_switch(this, buf) : -1; }

// Some inline functions:
//----------------------//
//        sendto        //
//----------------------//
inline int sendto(int fd_udp, const nbytes& buf, uint32 host, uint16 port) {
    return sendto(fd_udp, buf.bytes(), buf.n_bytes(), host, port);
    } // End of function sendto.

//----------------------//
//        set_in        //
//----------------------//
inline void set_in(sockaddr_in& x, uint32 host, uint16 port) {
    x.sin_family = AF_INET;
    x.sin_port = htons((short)port);
    x.sin_addr.s_addr = htonl((long)host);
    } // End of function set_in.

//----------------------//
//       reset_in       //
//----------------------//
inline void reset_in(sockaddr_in& x, uint32 host, uint16 port) {
    x.sin_port = htons((short)port);
    x.sin_addr.s_addr = htonl((long)host);
    } // End of function reset_in.

//----------------------//
//       clear_in       //
//----------------------//
inline void clear_in(sockaddr_in& x) {
    x.sin_family = AF_INET;
    x.sin_port = 0;
    x.sin_addr.s_addr = INADDR_ANY;
    } // End of function clear_in.

/*------------------------------------------------------------------------------
Test to see if two Internet ports have the same port number and IP address.
------------------------------------------------------------------------------*/
//----------------------//
//       equal_in       //
//----------------------//
inline int equal_in(const sockaddr_in& x1, const sockaddr_in& x2) {
    return (x1.sin_port == x2.sin_port
         && x1.sin_addr.s_addr == x2.sin_addr.s_addr);
    } // End of function equal_in.

/*------------------------------------------------------------------------------
Note that strangely the sunos4 C++ 2.x.y header file sys/socket.h defines
bind() to take a non-const sockaddr*, whereas connect() is defined to take a
const sockaddr*.
The other compiler environments seems to also require non-const sockaddr*
also for connect().
------------------------------------------------------------------------------*/
//----------------------//
//       bind_in        //
//----------------------//
inline int bind_in(int fd0, sockaddr_in& bname) {
    return ::bind(fd0, (sockaddr*)&bname, int(sizeof(bname)));
    } // End of function bind_in.

//----------------------//
//      connect_in      //
//----------------------//
inline int connect_in(int fd0, sockaddr_in& cname) {
    return ::connect(fd0, (sockaddr*)&cname, int(sizeof(cname)));
    } // End of function connect_in.

#endif /* AKSL_AKSLIP_H */
