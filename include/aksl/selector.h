// src/aksl/selector.h   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/include/aksl/selector.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_SELECTOR_H
#define AKSL_SELECTOR_H
/*------------------------------------------------------------------------------
Classes defined in this file:

select_handler::
select_handlerlist::
stdin_handler::
cdev_handler::
udp_delay_handler::
udp_handler::
udp_port_hand::
udp_port_handlist::
udp_hand_set::
tcp_handler::
tcp_context::
tcp_contextlist::
m_tcp_handler::
timer::
timer_heap::
timer_heap_traversal::
fdtype::
selector::
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The timers in this module use time expressed in "Unix seconds", which means UTC
seconds since 0000 UTC 1970 January 1, ignoring leap seconds. Actually, UTC only
started at 0000 UTC 1972 January 1, and from then until about 0000 UTC 1996
January 1, there were 20 leap seconds. So leap seconds are assumed to be zero in
the UTC years 1970 and 1971, and are ignored after 1972. This means that every
day has precisely 24*60*60 Unix seconds. Since system clocks are usually wrong
by many seconds, this does not make much difference to elapsed time. However...
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Warning: if a clock adjustment is underway (e.g. using "date -a"), then the
timers in this module will give a false time interval, since they are based on
the Unix system clock (e.g. gettimeofday), not directly on a hardware clock.
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_AKSLIP_H
#include "aksl/akslip.h"
#endif
#ifndef AKSL_HEAP_H
#include "aksl/heap.h"
#endif
#ifndef AKSL_CHARBUF_H
#include "aksl/charbuf.h"
#endif
#ifndef AKSL_BMEM_H
#include "aksl/bmem.h"
#endif
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_AKSLTIME_H
#include "aksl/aksltime.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
#ifndef WIN32
#if defined(HAVE_UNISTD_H) && !defined(AKSL_X_UNISTD_H)
#define AKSL_X_UNISTD_H
#include <unistd.h>
#endif

#ifndef AKSL_X_SYS_SOCKET_H
#define AKSL_X_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef AKSL_X_NETINET_IN_H
#define AKSL_X_NETINET_IN_H
#include <netinet/in.h>
#endif
#endif /* ! WIN32 */

// Maximum size of IP packets:
// const int IP_BUFSIZE = 65535;

// Masks for I/O event types:
enum event_cat_t {
    sNULL       = 0x00,         // Either no event, or a timer event.
    sREAD       = 0x01,
    sWRITE      = 0x02,
    sERROR      = 0x04
    };

// TCP handler events:
enum tcp_event_t {
    tcpNULL,
    tcpCALLING,
    tcpOPEN,
    tcpDATA,
    tcpCLOSE
    };

/*------------------------------------------------------------------------------
The user of the selector is supposed to derive another class from this
handler class, and redefine the virtual function "handler".
Then a pointer to the derived object should be handed to the selector
object when registering a handler.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Whenever a select_handler is invoked (through handler()), the members of the
object are set first.
"psel" is set to point to the selector object which is invoking the handler.
"fd" and "type" are set to the file descriptor and event category
for which this handler was called.
The full sets of events masks may be obtained by calling the "?fds" functions.
These just read the masks via the "psel" pointer.
Similarly, the time of return from the select() function can be read via the
t() and tv() functions. (These are both Unix seconds since 1 Jan 1970.)
------------------------------------------------------------------------------*/
//----------------------//
//    select_handler::  //
//----------------------//
struct select_handler: public slink {
friend struct selector;
private:
    struct selector* psel;          // The selector which called this handler.
public:
    select_handler* next() const { return (select_handler*)slink::next(); }

    bool_enum       delete_me;      // Flag to delete handler after called.
                                    // Only relevant when used for timer event.

    // Parameters made available for the handler() to read:
    int                     fd;     // File descriptor to be handled.
    event_cat_t             type;   // Event category to be handled.
    inline const fd_set*    rfds(); // The full read-event mask.
    inline const fd_set*    wfds(); // The full write-event mask.
    inline const fd_set*    efds(); // The full error-event mask.
    inline const timeval*   tv();   // Time of return from select().
    inline double           t();    // Time of return from select().

    // Register/deregister an I/O fd + type + handler with *psel.
    inline int set_fd_mask(int fd0, int types, select_handler* psh = 0);
    inline int clear_fd_mask(int fd0, int types);

    // Register/cancel a timer handler via *psel.
    inline const void* set_timer(double x, select_handler* psh = 0);
    inline void cancel_timer(const void*);
    void set_selector(selector& s) { psel = &s; }
    selector* get_selector() { return psel; }

    // Set inter-event timeout time/handler for *psel:
    inline void set_wait_time(double dx, select_handler* psh = 0);
    inline void set_wait_forever();

    // Re-define this in derived classes. Neg return value means error.
    virtual int handler() { return -1; }

//    select_handler& operator=(const select_handler& x) {}
//    select_handler(const select_handler& x) {};
    select_handler();
    virtual ~select_handler() {}
    }; // End of struct select_handler.

//--------------------------//
//    select_handlerlist::  //
//--------------------------//
struct select_handlerlist: private s2list {
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    select_handler* first() const { return (select_handler*)s2list::first(); }
    select_handler* last() const { return (select_handler*)s2list::last(); }
    select_handler* element(long i) const
        { return (select_handler*)s2list::element(i); }
    void append(select_handler* p) { s2list::append(p); }
    void prepend(select_handler* p) { s2list::prepend(p); }
    select_handler* popfirst() { return (select_handler*)s2list::popfirst(); }
    select_handler* poplast() { return (select_handler*)s2list::poplast(); }
    select_handler* remove(select_handler* p)
        { return (select_handler*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(select_handler* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(select_handlerlist& l) { s2list::swallow(&l); }
    void gulp(select_handlerlist& l) { s2list::gulp(&l); }
    void clear() { for (select_handler* p = first(); p; )
        { select_handler* q = p->next(); delete p; p = q; } clearptrs(); }

//    select_handlerlist& operator=(const select_handlerlist& x) {}
//    select_handlerlist(const select_handlerlist& x) {};
    select_handlerlist() {}
    ~select_handlerlist() { clear(); }
    }; // End of struct select_handlerlist.

/*------------------------------------------------------------------------------
This class handles the arrival of bytes on standard input.
The user of this class should define the "action" member to a function which
acts appropriately when input arrives.
The user of this class is supposed to derive another class from it, which will
have extra members to enable the input to go to the right object.
------------------------------------------------------------------------------*/
//----------------------//
//    stdin_handler::   //
//----------------------//
struct stdin_handler: public select_handler {
private:
    // Temporary variables for handler():
    char*       bytes;      // Heap-allocated. Set to 0 if taken over.
    int         n_bytes;
    virtual int handler();
protected:
    // To be defined in a derived class:
    virtual int action(const char* /*buf*/, int /*buflen*/) { return -1; }
public:

//    stdin_handler& operator=(const stdin_handler& x) {}
//    stdin_handler(const stdin_handler& x) {};
    stdin_handler() { bytes = 0; n_bytes = 0; }
    virtual ~stdin_handler() {}
    }; // End of struct stdin_handler.

/*------------------------------------------------------------------------------
This class handles the arrival of bytes on a Unix character (pseudo-)device.
The user of this class should define the "action" member to a function which
acts appropriately when input arrives.
The user of this class is supposed to derive another class from it, which will
have extra members to enable the input to go to the right object.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This class is similar to stdin_handler, but it assumes that the device is really
sending bytes in demarcated packets. Only a single read is done for each
read-event, and the result is assumed to be a single packet, not just some bytes
in a stream of bytes.
------------------------------------------------------------------------------*/
//----------------------//
//     cdev_handler::   //
//----------------------//
struct cdev_handler: public select_handler {
private:
    virtual int handler();
protected:
    nbytes  buf;

    // To be re-defined in a derived class:
    virtual int action() { return -1; }
public:

//    cdev_handler& operator=(const cdev_handler& x) {}
//    cdev_handler(const cdev_handler& x) {};
    cdev_handler() {}
    virtual ~cdev_handler() {}
    }; // End of struct cdev_handler.

/*------------------------------------------------------------------------------
This handler is woken up when it is time to send a delayed UDP packet.
It is really a timer handler -- not a UDP event handler.
The UDP packet payload is stored in "buf0", and the packet is then sent off
whenever "handler" is called. "buf0" can be set with copy_pkt().
Objects of this class should be registered with a selector with a set_timer
call of some sort.
The "fd0" member, set from "fd1" in copy_pkt(), is used for transmitting the UDP
packet, and has nothing to do with waiting for the reception of UDP packets.
The user should have already opened the UDP socket with this handle.
The "fd0" and "to0" parameters are used in a sendto() call in handler().
------------------------------------------------------------------------------*/
//----------------------//
//  udp_delay_handler:: //
//----------------------//
struct udp_delay_handler: public select_handler {
private:
    int         fd0;            // File descriptor of UDP socket.
    nbytes      buf0;           // The payload for the UDP packet.
    sockaddr_in to0;            // UDP socket to send packet to.
public:
    // Copy parameters for delayed transmission:
    void copy_pkt(int fd1, nbytes& buf, sockaddr_in& to);

    // Re-definition of select_handler::handler():
    int handler();

//    udp_delay_handler& operator=(const udp_delay_handler& x) {}
//    udp_delay_handler(const udp_delay_handler& x) {};
    udp_delay_handler() { fd0 = -1; clear_in(to0); }
    virtual ~udp_delay_handler() {}
    }; // End of struct udp_delay_handler.

/*------------------------------------------------------------------------------
This class handles the arrival of packets on a UDP port.
The user of this class should set the "action" member to a function which acts
appropriately when a packet arrives.
Note that this is starting to look like a stack of functions as in streams
modules. In fact, "select_handler" could have been written with a
function-pointer too, and then there could have been a linked list of handlers,
each passing on their results to the next function. The difference here is that
everything is happening in user space, not in the kernel.
------------------------------------------------------------------------------*/
//----------------------//
//     udp_handler::    //
//----------------------//
struct udp_handler: public select_handler {
friend struct udp_hand_set;
friend struct udp_port_hand;
protected:
    // Temporary variables passed by handler() to action():
    nbytes_from buf;                // Variable-length array of char.
    uint32      fromhost;           // IP host address of sender.
    uint16      fromport;           // UDP port of sender.
private:
    virtual int handler();          // Redefines select_handler::handler.
public:
    udp_handler* next() const { return (udp_handler*)select_handler::next(); }

    int udp_open(int& udp_port, int n_tries = 1);   // Work in progress.

    // To be defined in a derived class (called by handler()):
    virtual int action() { return -1; }

//    udp_handler& operator=(const udp_handler& x) {}
//    udp_handler(const udp_handler& x) {};
    udp_handler() { fromhost = 0; fromport = 0; }
    virtual ~udp_handler() {}
    }; // End of struct udp_handler.

/*------------------------------------------------------------------------------
This represents a single UDP port that is to be handled.
Many event handlers may be defined for a single UDP port.
------------------------------------------------------------------------------*/
//----------------------//
//    udp_port_hand::   //
//----------------------//
struct udp_port_hand: public udp_handler {
friend struct udp_hand_set;
private:
    udp_port*       p0;         // The UDP port to be handled.
    voidptrlist     handlers;   // The "udp_handler" chain for this port.
public:
    udp_port_hand* next() const { return (udp_port_hand*)udp_handler::next(); }

    int             trace;

    // The UDP socket fd which will be monitored:
    int fd_udp_port() { return p0 ? p0->fd() : -1; }

    // This will pass packets onto the chain of UDP handlers.
    virtual int action();

//    udp_port_hand& operator=(const udp_port_hand& x) {}
//    udp_port_hand(const udp_port_hand& x) {};
    udp_port_hand(udp_port* p1 = 0) { p0 = p1; trace = 0; }
    virtual ~udp_port_hand() {}
    }; // End of struct udp_port_hand.

//----------------------//
//  udp_port_handlist:: //
//----------------------//
struct udp_port_handlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    udp_port_hand* first() const { return (udp_port_hand*)s2list::first(); }
    udp_port_hand* last() const { return (udp_port_hand*)s2list::last(); }
    udp_port_hand* element(long i) const
        { return (udp_port_hand*)s2list::element(i); }
    void append(udp_port_hand* p) { s2list::append(p); }
    void prepend(udp_port_hand* p) { s2list::prepend(p); }
    udp_port_hand* popfirst() { return (udp_port_hand*)s2list::popfirst(); }
    udp_port_hand* poplast() { return (udp_port_hand*)s2list::poplast(); }
    udp_port_hand* remove(udp_port_hand* p)
        { return (udp_port_hand*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(udp_port_hand* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(udp_port_handlist& l) { s2list::swallow(&l); }
    void gulp(udp_port_handlist& l) { s2list::gulp(&l); }
    void clear() { for (udp_port_hand* p = first(); p; )
        { udp_port_hand* q = p->next(); delete p; p = q; } clearptrs(); }

//    udp_port_handlist& operator=(const udp_port_handlist& x) {}
//    udp_port_handlist(const udp_port_handlist& x) {};
    udp_port_handlist() {}
    ~udp_port_handlist() { clear(); }
    }; // End of struct udp_port_handlist.

/*------------------------------------------------------------------------------
This represents a set of UDP event handlers. The idea is that since the Unix
select() function only indicates which "fd" has an event, there is a need for
handlers which have a more differentiated event to act on. In particular, a
handler may wish to wait only for UDP packets from a given source IP host and/or
UDP socket number, or for some subset of combinations of these parameters.
Hence this class is provided to divide up the various sub-events of given UDP
socket read-events among lower-level event handlers.
------------------------------------------------------------------------------*/
//----------------------//
//     udp_hand_set::   //
//----------------------//
struct udp_hand_set {
private:
    udp_port_set        port_set;       // A (well-managed) set of UDP ports.
    udp_port_handlist   port_hands;     // Handlers for ports in port_set.
public:
    int                 trace;

    // Set up event handling for a given UDP port-to-port link:
    udp_port_hand* open(uint16 loc_port, uint32 loc_ip,
                        udp_handler* ph0, selector& sel0);

//    udp_hand_set& operator=(const udp_hand_set& x) {}
//    udp_hand_set(const udp_hand_set& x) {};
    udp_hand_set() { trace = 0; }
    ~udp_hand_set() {}
    }; // End of struct udp_hand_set.

/*------------------------------------------------------------------------------
This class handles the arrival of events on a tcp control and data socket.
The user of this class should set the "action" member to a function which acts
appropriately when a packet arrives.
This class is suitable for only one data socket at a time.
------------------------------------------------------------------------------*/
//----------------------//
//     tcp_handler::    //
//----------------------//
struct tcp_handler: public select_handler {
protected:
    // Temporary variables passed by handler() to action():
    tcp_event_t event_type;         // Event for use of action().
    static const char* event_type_string(tcp_event_t t);

    nbytes      buf;                // Variable-length array of char.
    sockaddr_in from;               // Full address of sender.
    uint32      fromhost;           // IP host address of sender.
    uint16      fromport;           // TCP port of sender.
private:
    bool_enum   active;             // True if TCP connection is active.
    charbuf     tx_data_wait;       // Buffer while waiting for connect.
    int         fd_cntl;            // The control socket.
    int         fd_data;            // The data socket.

    virtual int handler();          // Redefines select_handler::handler.
public:
    bool_enum   connect_in_progress;
    int         tcp_open_return;
    int         trace;

    tcp_handler* next() const { return (tcp_handler*)select_handler::next(); }

    // Warning: should really have ntohl(long(INADDR_ANY)) here:
    // Open passive TCP socket (i.e. server).
    int open(selector& sel0,
             uint16 loc_port, uint32 loc_ip = INADDR_ANY, int backlog = 5);
    // Open active TCP socket (i.e. client).
    int open(selector& sel0, uint16 rem_port, uint32 rem_ip,
             uint16 loc_port, uint32 loc_ip = INADDR_ANY);
    // Open for writing:
    bool_enum open()
        { return bool_enum(fd_data >= 0 && (active || fd_cntl >= 0)); }
    int write(const char* nbuf, int n);
    int write(const nbytes& nbuf)
        { return write(nbuf.bytes(), nbuf.n_bytes()); }
    void print(ostream& = cout);

    // To be re-defined in a derived class (called by handler()):
    virtual int action() { return -1; }

//    tcp_handler& operator=(const tcp_handler& x) {}
//    tcp_handler(const tcp_handler& x) {};
    tcp_handler() {
        event_type = tcpNULL;
        clear_in(from);
        fromhost = 0;
        fromport = 0;
        active = false;
        fd_cntl = -1;
        fd_data = -1;
        connect_in_progress = false;
        tcp_open_return = 0;
        trace = 0;
        }
    virtual ~tcp_handler() {}
    }; // End of struct tcp_handler.

/*------------------------------------------------------------------------------
NOTE: This class is work in progress.
This structure is a context descriptor for each TCP data socket opened within
the m_tcp_handler class.
The tcp_handler::action() function (i.e. the function action() in any class
which is derived from tcp_handler), is presented with a tcp_context structure
whenever it is called.
------------------------------------------------------------------------------*/
//----------------------//
//     tcp_context::    //
//----------------------//
struct tcp_context: public slink {
friend struct tcp_contextlist;
friend struct m_tcp_handler;
protected:
    // Info for the use of the m_tcp_handler::action() function.
    nbytes      buf;                // Variable-length array of char.
    sockaddr_in from;               // Full address of sender.
    uint32      fromhost;           // IP host address of sender.
    uint16      fromport;           // TCP port of sender.
private:
    charbuf     tx_data_wait;       // Buffer while waiting for connect.
    int         fd_data;            // The data socket.
protected:
    int         tcp_open_return;    // This is for ....
public:
    tcp_context* next() const { return (tcp_context*)slink::next(); }

//    tcp_context& operator=(const tcp_context& x) {}
//    tcp_context(const tcp_context& x) {}
    tcp_context() {
        clear_in(from);
        fromhost = 0;
        fromport = 0;
        fd_data = -1;
        tcp_open_return = 0;
        }
    ~tcp_context() {}
    }; // End of struct tcp_context.

//----------------------//
//   tcp_contextlist::  //
//----------------------//
struct tcp_contextlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    tcp_context* first() const { return (tcp_context*)s2list::first(); }
    tcp_context* last() const { return (tcp_context*)s2list::last(); }
    tcp_context* element(long i) const
        { return (tcp_context*)s2list::element(i); }
    void append(tcp_context* p) { s2list::append(p); }
    void prepend(tcp_context* p) { s2list::prepend(p); }
    tcp_context* popfirst() { return (tcp_context*)s2list::popfirst(); }
    tcp_context* poplast() { return (tcp_context*)s2list::poplast(); }
    tcp_context* remove(tcp_context* p)
        { return (tcp_context*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(tcp_context* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(tcp_contextlist& l) { s2list::swallow(&l); }
    void gulp(tcp_contextlist& l) { s2list::gulp(&l); }
    void clear() { for (tcp_context* p = first(); p; )
        { tcp_context* q = p->next(); delete p; p = q; } clearptrs(); }

    // Non-routine members.
    tcp_context* find_fd(int fd);
    tcp_context* find_from(const sockaddr_in& from);

//    tcp_contextlist& operator=(const tcp_contextlist& x) {}
//    tcp_contextlist(const tcp_contextlist& x) {}
    tcp_contextlist() {}
    ~tcp_contextlist() { clear(); }
    }; // End of struct tcp_contextlist.

/*------------------------------------------------------------------------------
NOTE: This class is work in progress.
This is a variation of tcp_handler to take care of multiple
simultaneously open TCP data sockets.
Active TCP port facilities are _not_ provided in this class because for an
active TCP port there is no control port and only one data port.
------------------------------------------------------------------------------*/
//----------------------//
//    m_tcp_handler::   //
//----------------------//
struct m_tcp_handler: public select_handler {
protected:
    // Temporary variables/functions passed by handler() to action():
    tcp_event_t event_type;         // Event for use of action().
    static const char* event_type_string(tcp_event_t t);

    tcp_contextlist tcp_contexts;   // One context for each open socket.
    tcp_context* context;           // Context for action() to read.

    sockaddr_in from;               // Full address of sender.
    uint32      fromhost;           // IP host address of sender.
    uint16      fromport;           // TCP port of sender.
    int         fd_cntl;            // The control socket.
private:
    int         handler();          // Redefines select_handler::handler.
public:
    int         trace;              // User-settable trace value.

    m_tcp_handler* next() const
        { return (m_tcp_handler*)select_handler::next(); }

    // Open passive TCP socket (i.e. server).
    // (Comment: In theory, should have ntohl(long(INADDR_ANY)) here.)
    int open(selector& sel0,
             uint16 loc_port, uint32 loc_ip = INADDR_ANY, int backlog = 5);

    // Write to one of the open connections.
//    int write(const char* nbuf, int n);
//    int write(const nbytes& nb) { return write(nb.bytes(), nb.n_bytes()); }
    void print(ostream& = cout);

    // To be re-defined in a derived class. (It is called by handler().)
    virtual int action() { return -1; }

//    m_tcp_handler& operator=(const m_tcp_handler& x) {}
//    m_tcp_handler(const m_tcp_handler& x) {};
    m_tcp_handler() {
        event_type = tcpNULL;
        context = 0;
        clear_in(from);
        fromhost = 0;
        fromport = 0;
        fd_cntl = -1;
        trace = 0;
        }
    virtual ~m_tcp_handler() {}
    }; // End of struct m_tcp_handler.

// Special kludge-oriented handler for cancelling timers efficiently.
// (This handler is never called. Its address is used as a "cancel-event"
// indication.)
extern select_handler* cancel_timer_handler;

//----------------------//
//        timer::       //
//----------------------//
struct timer: private tim {
friend struct timer_heap;
friend struct timer_heap_traversal;
friend struct selector;
private:
    select_handler*    t_handler;
public:
    double time() const { return t; }
    void cancel() { t_handler = cancel_timer_handler; }

    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }

//    timer& operator=(const timer& x) {}
//    timer(const timer& x) {};
    timer(double tt) {
        t = tt;
        t_handler = 0;
#if BMEM_TRACE
        bmem0.owner = "timer";
#endif
        }
    ~timer() {}
    }; // End of struct timer.

//----------------------//
//      timer_heap::    //
//----------------------//
struct timer_heap: private min_tim_heap {
friend struct timer_heap_traversal;
public:
    // The routine members:
    const timer* first() const { return (const timer*)min_tim_heap::first(); }
    const timer* last() const { return (const timer*)min_tim_heap::last(); }
    using min_tim_heap::empty;
    using min_tim_heap::length;

    // The cast in the next line should not be necessary!
    void insert(timer* p) { min_tim_heap::insert((tim*)p); }
    timer* popfirst() { return (timer*)min_tim_heap::popfirst(); }
    void delfirst() { delete popfirst(); }
    void del_timers() {
        timer* pt;
        while ((pt = popfirst()) != 0)
            delete(pt);
        }
    void clear() { del_timers(); }

    timer_heap() {}
    ~timer_heap() {}
    }; // End of struct timer_heap.

//--------------------------//
//  timer_heap_traversal::  //
//--------------------------//
struct timer_heap_traversal: private heap_traversal {

public:
    timer* next() { return (timer*)heap_traversal::next(); }
    using heap_traversal::init;

//    timer_heap_traversal& operator=(const timer_heap_traversal& x) {}
//    timer_heap_traversal(const timer_heap_traversal& x) {};
    timer_heap_traversal(timer_heap& h): heap_traversal(h) {}
    ~timer_heap_traversal() {}
    }; // End of struct timer_heap_traversal.

/*------------------------------------------------------------------------------
This class records a file descriptor and the set of events to be monitored.
------------------------------------------------------------------------------*/
//----------------------//
//       fdtype::       //
//----------------------//
struct fdtype {
    uint16 fd;
    uint16 types;                   // Mask of event types.
                                    // Duplicates info in event masks (?).
    select_handler*     r_handler;
    select_handler*     w_handler;
    select_handler*     e_handler;

    void print(ostream& = cout);    // For tracing the selector state.

//    fdtype& operator=(const fdtype& x) {}
//    fdtype(const fdtype& x) {};
    fdtype() {
        fd = 0;
        types = 0;
        r_handler = 0;
        w_handler = 0;
        e_handler = 0;
        }
    ~fdtype() {}
    }; // End of struct fdtype.

/*------------------------------------------------------------------------------
This is an I/O event handling class.
An object of this class monitors select() calls for multiple client
routines within the same program.
To try to avoid some events from being locked out by others, a round robin
cycle through all requested events is implemented.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The call to fetch the next event is selector::get_event.
The idea is that first you register all select-handlers with the selector, and
then you go into an event handling loop by calling selector::get_event.
The timeout event is controlled by "wait_time" and "wait_forever".
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note that the fdlist structure can be quite big. If it gets too big,
then linked lists will have to be used instead of an array.
SunOS4 uses 4096 bytes for the "fdlist", but the limit on open files or file
descriptors is 16 (POSIX) or 5 (sysconf). So the 256 "fdtype" structures are a
bit excessive!
------------------------------------------------------------------------------*/
//----------------------//
//       selector::     //
//----------------------//
struct selector {
private:
    int         max_fd;             // Maximum fd to monitor.
    fd_set      read_mask;          // Read-mask.
    fd_set      write_mask;         // Write-mask.
    fd_set      error_mask;         // Error-mask.
    double      wait_time;          // Time to wait in seconds.
    bool_enum   wait_forever;       // True if no timeout to be given.
    select_handler* ie_handler;     // Handler for inter-event timeout events.
                                    // Temporaries for return of values:
    fd_set      rfds;               //  for read events,
    fd_set      wfds;               //  for write events,
    fd_set      efds;               //  for error events,
    timeval     tv_return;          //  time of return.
    double      t_return;           //  time of return.
public:                             // (Readable by caller of get_event().)
    const fd_set* prfds() { return &rfds; }
    const fd_set* pwfds() { return &wfds; }
    const fd_set* pefds() { return &efds; }
    const timeval* ptv()  { return &tv_return; }
    double      t()       { return t_return; }
private:
    timeval     timeout;            //  for time to wait.

                                    // fd list for optimising select() calls:
    fdtype      fdlist[FD_SETSIZE]; // Array of event fds/types to monitor.
    int         n_fds;              // Number of fds in the array.
    int         last_fd;            // Last fd checked (for round robin).
    int         last_cat;           // Last category checked (for round robin).
    timer_heap  timers;             // A sorted heap of timeout handlers.
public:
    int         select_return;      // Return value from select() call.
    int         select_errno;       // Errno value if select returns error.
    int         trace;

    void print(ostream& = cout);    // For tracing the selector state.

    // Register/deregister an I/O fd + type + handler.
    int set_fd_mask(int fd, int types, select_handler* psh = 0);
    int clear_fd_mask(int fd, int types);

    // Register a timer handler. (Time x is absolute, dx is relative.)
    const void* set_timer(double x, select_handler* psh = 0);
    const void* set_timer_rel(double dx, select_handler* psh = 0);
    void cancel_timer(const void*); // Cancel a registered timer.
    void set_selector(select_handler& s) { s.psel = this; }

    // Set timeout times (for event-free intervals):
    void set_wait_time(double dx, select_handler* psh = 0);
    void set_wait_forever() { wait_forever = true; ie_handler = 0; }

    // Wait for I/O and/or timer/timeout events.
    int get_event();

//    selector& operator=(const selector& x) {}
//    selector(const selector& x) {};
    selector();
    ~selector() {}
    }; // End of struct selector.

// A posteriori inline member functions:
//- - - - - - - - - - - //
//    select_handler::  //
//- - - - - - - - - - - //
inline const fd_set* select_handler::rfds() { return psel ? psel->prfds() : 0; }
inline const fd_set* select_handler::wfds() { return psel ? psel->pwfds() : 0; }
inline const fd_set* select_handler::efds() { return psel ? psel->pefds() : 0; }
inline const timeval* select_handler::tv()  { return psel ? psel->ptv() : 0; }
inline double   select_handler::t()    { return psel ? psel->t() : 0; }
inline int select_handler::set_fd_mask(int fd0, int types, select_handler* psh)
    { return psel ? psel->set_fd_mask(fd0, types, psh) : 0; }
inline int select_handler::clear_fd_mask(int fd0, int types)
    { return psel ? psel->clear_fd_mask(fd0, types) : 0; }
inline const void* select_handler::set_timer(double x, select_handler* psh)
    { return psel ? psel->set_timer(x, psh) : 0; }
inline void select_handler::cancel_timer(const void* p)
    { if (psel) psel->cancel_timer(p); }
inline void select_handler::set_wait_time(double dx, select_handler* psh)
    { if (psel) psel->set_wait_time(dx, psh); }
inline void select_handler::set_wait_forever()
    { if (psel) psel->set_wait_forever(); }

// Exported functions:
extern int dispatch(selector& sel0, int fd, nbytes& buf, sockaddr_in& to,
             double delay);

#endif /* AKSL_SELECTOR_H */
