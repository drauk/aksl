// src/aksl/cpbuf.h   2018-3-3   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_CPBUF_H
#define AKSL_CPBUF_H
/*------------------------------------------------------------------------------
Classes in this file:

virtual_pkt::
cp_pkt::
cp_pktlist::
udp_cp_pkt::
udp_cp_pktlist::
cp_fifo::
cp_fifo_array::
cp_buffer::
cp_bufferlist::
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This module implements credit priority buffering algorithms.
See ``Credit priority queueing for ATM switches'', Alan Kennington,
ATNAC 1994 conference, Melbourne, December 1994, 399-403.
http://www.topology.org/tex/atnac/README.html
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_BMEM_H
#include "aksl/bmem.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// Styles of buffering.
enum buffer_style_t {
    bsABSOLUTE_PRIORITY,
    bsCREDIT_PRIORITY
    };

// CP FIFO states.
enum cp_fifo_state_t {
    cfsACTIVE,                  // Normal active state.
    cfsPAUSE,                   // Paused, i.e. not to be served.
    cfsFINISHED,                // To be made free as soon as possible.
    cfsFREE                     // Re-initialised, and free to be re-allocated.
    };

// Block memory class for packets, because of (alleged) solaris bug in malloc.
// [Note that this bug does seem to be there for PC/solaris Sun C++ !!!!]
// This flag will probably be permanently set to 1, but if there are any
// negative consequences, it could be set to 0.
#define USE_CP_PKT_BMEM 1

// Forward references:
struct cp_pkt;

/*------------------------------------------------------------------------------
This is a base class for "virtual packets". These are packets which are not
actually finally created until it is time to be transmitted.
This should be useful for acknowledgement packets, for instance, whose contents
are not finally determined until the last moment. That is, this class should be
useful for traffic streams whose packets can easily get out of date while
waiting in the queue.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The member function "packet_size" should return the number of bytes which the
stream would put into the next transmitted packet if transmission could occur
right now. A negative number should be returned if no packet is ready to send.

Immediately after a "packet_size" call (if the return value is non-negative and
the packet is chosen for transmission), function "packet_create" may be called.
This member function should create a packet of the same size as
returned by "packet_size", or smaller. (It is assumed that no hardware
concurrency comes in between. If this does happen, then the packet creation
routine is still obliged to stay within the bound last returned by
"packet_size". Otherwise, the packet may be rejected.)

If the "finished" member is true after "packet_size" or "packet_create" are
called, then the "cp_pkt" structure through which this "virtual_pkt" structure
was invoked is dequeued and deleted, and the "virtual_pkt" structure "v_pkt" is
also deleted. Any cp_pkt structure returned by packet_create just before the
"finished" member is found to be set is simply deleted. I.e. it is not sent.

In summary, the chronological sequence may be thought of as follows:
-   It's time to send a packet.
-   The CP buffer reads the packet lengths at the head of all per-stream FIFOs.
    (This is done by calling "packet_size" if the packet is virtual.)
-   The CP buffer chooses the most worthy traffic stream.
-   If the packet at the head of this stream is virtual, then "packet_create"
    is called to create the packet.
-   The "cp_pkt" structure is dequeued and deleted if "finished" is true in the
    "v_pkt" structure.
-   If a packet is created, it is accepted for transmission.

Note that the previously returned value of "packet_size" may be used for
calculating the next wake-up time at which it will be "time to send a packet".

Note also that the "packet_size" need not be called if the CP buffer is not
placing bounds on the packet size.
------------------------------------------------------------------------------*/
//----------------------//
//     virtual_pkt::    //
//----------------------//
struct virtual_pkt {
friend struct cp_buffer;
protected:
    bool_enum   finished;                   // De-queueing indication.
public:
    // Functions to be re-defined in derived classes.

    // Return the anticipated size of the next packet:
    virtual int packet_size(double /*t*/) { return -1; }

    // Create the next packet, and indicate its size.
    // [Parameter "t" is time in Unix seconds since 1 Jan 1970.]
    virtual cp_pkt* packet_create(double /*t*/) { return 0; }

//    virtual_pkt& operator=(const virtual_pkt& x) {}
//    virtual_pkt(const virtual_pkt& x) {};
    virtual_pkt() { finished = false; }
    virtual ~virtual_pkt() {}
    }; // End of struct virtual_pkt.

/*------------------------------------------------------------------------------
If block memory allocation (cp_pkt_bmem) is used for this class, then derived
classes (with a larger structure size) cannot use the new and delete functions
of this class. Then separate functions must be used for the derived class.
This is a really good reason why the virtual_pkt concept is implemented
as a pointer member rather than a derived class, because fixed-length
block-memory allocation does not work for derived classes.
Therefore please do _not_ derive any class from class cp_pkt, unless you
supply your own memory management operators "delete" and "new", e.g. using the
"bmem" class as for "c_pkt".
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
If the packet is virtual, then "v_pkt" should be set to the address of the
virtual packet descriptor. In this case, the "cp_pkt" structure is not deleted
from the front of the packet queue, unless the "finished" member of "*v_pkt" is
true. If a virtual_pkt is registered at cp_pkt delete-time, it is deleted.
------------------------------------------------------------------------------*/
//----------------------//
//        cp_pkt::      //
//----------------------//
struct cp_pkt: public slink {
private:
    char* buf0;                         // The packet contents.
    int len;                            // The length of the packet.
    virtual_pkt* v_pkt;                 // On-demand packet creator.
public:
    cp_pkt* next() const { return (cp_pkt*)slink::next(); }

    int length(double t) { return v_pkt ? v_pkt->packet_size(t) : len; }
    void copy_in(const char*, int len); // Copy from the given char array.
    int copy_out(char*, int len);       // Copy to the given char array.

    // Functions for reading the packet data address and length.
    const char* bytes() { return buf0; }
    int n_bytes() { return len; }

    // Virtual packet management functions.
    void set_virtual_pkt(virtual_pkt* p) { delete v_pkt; v_pkt = p; }
    virtual_pkt* get_virtual_pkt() { return v_pkt; }
    void del_virtual_pkt() { delete v_pkt; v_pkt = 0; }
    virtual_pkt* pop_virtual_pkt()
        { virtual_pkt* p = v_pkt; v_pkt = 0; return p; }

    // Block memory allocation to get around (non-existent) solaris malloc bug:
#if USE_CP_PKT_BMEM
    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }
#endif
//    cp_pkt& operator=(const cp_pkt& x) {}
//    cp_pkt(const cp_pkt& x) {};
    cp_pkt() { buf0 = 0; len = -1; v_pkt = 0; }
    ~cp_pkt() { delete[] buf0; delete v_pkt; }
    }; // End of struct cp_pkt.

//----------------------//
//      cp_pktlist::    //
//----------------------//
struct cp_pktlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    cp_pkt* first() const { return (cp_pkt*)s2list::first(); }
    cp_pkt* last() const { return (cp_pkt*)s2list::last(); }
    cp_pkt* element(long i) const
        { return (cp_pkt*)s2list::element(i); }
    void append(cp_pkt* p) { s2list::append(p); }
    void prepend(cp_pkt* p) { s2list::prepend(p); }
    cp_pkt* popfirst() { return (cp_pkt*)s2list::popfirst(); }
    cp_pkt* poplast() { return (cp_pkt*)s2list::poplast(); }
    cp_pkt* remove(cp_pkt* p)
        { return (cp_pkt*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(cp_pkt* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(cp_pktlist& l) { s2list::swallow(&l); }
    void gulp(cp_pktlist& l) { s2list::gulp(&l); }
    void clear() { for (cp_pkt* p = first(); p; )
        { cp_pkt* q = p->next(); delete p; p = q; } clearptrs(); }

//    cp_pktlist& operator=(const cp_pktlist& x) {}
//    cp_pktlist(const cp_pktlist& x) {};
    cp_pktlist() {}
    ~cp_pktlist() { clear(); }
    }; // End of struct cp_pktlist.

/*------------------------------------------------------------------------------
The _source_ host and port should not matter, because a single cp_buffer
should generally correspond to at most one local UDP port. Hence only the
destination addressing is given here.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The memory management members new/delete demonstrate how this should be done by
users who want to derive other classes from cp_pkt. If cp_pkt is doing
block memory managment, then so must every derived class.
------------------------------------------------------------------------------*/
//----------------------//
//      udp_cp_pkt::    //
//----------------------//
struct udp_cp_pkt: public cp_pkt {
public:
    uint32  dest_host;
    uint16  dest_port;
public:
    udp_cp_pkt* next() const { return (udp_cp_pkt*)cp_pkt::next(); }

    // Block memory allocation to get around (non-existent) solaris malloc bug.
#if USE_CP_PKT_BMEM
    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p0) { bmem0.freechunk(p0); }
    static unsigned long n_objects() { return bmem0.length(); }
#endif
//    udp_cp_pkt& operator=(const udp_cp_pkt& x) {}
//    udp_cp_pkt(const udp_cp_pkt& x) {};
    udp_cp_pkt() { dest_host = 0; dest_port = 0; }
    ~udp_cp_pkt() {}
    }; // End of struct udp_cp_pkt.

//----------------------//
//    udp_cp_pktlist::  //
//----------------------//
struct udp_cp_pktlist: private cp_pktlist {
protected:
    using cp_pktlist::clearptrs;
public:
    // The routine members:
    using cp_pktlist::empty;
    using cp_pktlist::length;
    using cp_pktlist::member;
    using cp_pktlist::position;
    udp_cp_pkt* first() const { return (udp_cp_pkt*)cp_pktlist::first(); }
    udp_cp_pkt* last() const { return (udp_cp_pkt*)cp_pktlist::last(); }
    udp_cp_pkt* element(long i) const
        { return (udp_cp_pkt*)cp_pktlist::element(i); }
    void append(udp_cp_pkt* p) { cp_pktlist::append(p); }
    void prepend(udp_cp_pkt* p) { cp_pktlist::prepend(p); }
    udp_cp_pkt* popfirst() { return (udp_cp_pkt*)cp_pktlist::popfirst(); }
    udp_cp_pkt* poplast() { return (udp_cp_pkt*)cp_pktlist::poplast(); }
    udp_cp_pkt* remove(udp_cp_pkt* p)
        { return (udp_cp_pkt*)cp_pktlist::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(udp_cp_pkt* p) { delete remove(p); }
    using cp_pktlist::insertafter;
//    void swallow(udp_cp_pktlist& l) { cp_pktlist::swallow(&l); }
//    void gulp(udp_cp_pktlist& l) { cp_pktlist::gulp(&l); }
    void clear() { for (udp_cp_pkt* p = first(); p; )
        { udp_cp_pkt* q = p->next(); delete p; p = q; } clearptrs(); }

//    udp_cp_pktlist& operator=(const udp_cp_pktlist& x) {}
//    udp_cp_pktlist(const udp_cp_pktlist& x) {};
    udp_cp_pktlist() {}
    ~udp_cp_pktlist() { clear(); }
    }; // End of struct udp_cp_pktlist.

/*------------------------------------------------------------------------------
This class has members suitable for implementing CP service. This is
"credit priority" service, which is a combination of LB and RR service.
First, packets are sent by spending credit from a leaky bucket, if possible.
If there is insufficient credit in the leaky bucket, then credit is borrowed at
a certain rate (rr_rate) from other FIFOs to get the packet through. This
"lending rate" is the inverse of the weight the FIFO receives in the round robin
bandwidth allocation.
------------------------------------------------------------------------------*/
//----------------------//
//       cp_fifo::      //
//----------------------//
struct cp_fifo {
friend struct cp_fifo_array;
friend struct cp_buffer;
friend int fifo_compare_neg(const void* p1, const void* p2);

// Public private members (temporary kludge to save time).
public:
    cp_pktlist pkts;        // The packets waiting for service.
    double lb_cred;         // Leaky bucket credit (bits).
    double lb_cap;          // Leaky bucket max credit (bits).
    double lb_rate;         // Leaky bucket credit rate (bits/sec).
    bool_enum lb_cap_fixed;
    bool_enum lb_rate_fixed;

    double rr_cred;         // Round robin credit/debt (relative bits).
    double rr_rate;         // Round robin __inverse__ relative weight.
    bool_enum rr_rate_fixed;
    bool_enum rr_active;    // Turns RR credit mechanism on/off.
private:
    cp_fifo_state_t state;  // Either of: active, pause, finished, free.
    double vcred;           // Temp var: virtual credit for prioritising fifos.
    bool_enum offering;     // Temp var: true if offering a packet.
    int index;              // Position of cp_fifo in cp_fifo_array.
public:
    // Reset LB credit to max, and RR credit to 0.
    void reset_credit() { lb_cred = lb_cap; rr_cred = 0; }

    // Make this FIFO free for future re-use.
    void free_fifo() {
        pkts.clear();       // Remove all packets. (But not virutal packets!)
        lb_cred = 0;        // Do everything else as for FIFO contruction.
        lb_cap = 0;
        lb_rate = 0;
        lb_cap_fixed = false;
        lb_rate_fixed = false;

        rr_cred = 0;
        rr_rate = 1;
        rr_rate_fixed = false;
        rr_active = true;
        vcred = 0;

        state = cfsFREE;
        offering = false;
        }
    cp_fifo& operator=(cp_fifo& x) {
        pkts.gulp(x.pkts);
        lb_cred = x.lb_cred;
        lb_cap = x.lb_cap;
        lb_rate = x.lb_rate;
        lb_cap_fixed = x.lb_cap_fixed;
        lb_rate_fixed = x.lb_rate_fixed;

        rr_cred = x.rr_cred;
        rr_rate = x.rr_rate;
        rr_rate_fixed = x.rr_rate_fixed;
        rr_active = x.rr_active;
        vcred = x.vcred;

        state = x.state;
        offering = x.offering;
        index = x.index;
        return *this;
        }
//    cp_fifo(const cp_fifo& x) {};
    cp_fifo(int i) {    // Specify the position within the containing array.
        lb_cred = 0;
        lb_cap = 0;
        lb_rate = 0;
        lb_cap_fixed = false;
        lb_rate_fixed = false;

        rr_cred = 0;
        rr_rate = 1;
        rr_rate_fixed = false;
        rr_active = true;
        vcred = 0;

        state = cfsFREE;
        offering = false;
        index = i;
        }
    ~cp_fifo() {}
    }; // End of struct cp_fifo.

/*------------------------------------------------------------------------------
This class is implemented as a dynamically resized array of _pointers_ to
cp_fifo objects. The pointers in this array will _always_ point to instantiated
cp_fifo objects. This has the efficiency advantage that it is not necessary to
check the pointers before using them. It has the minor disadvantage that there
could be some memory waste if not all cp_fifo objects are used.
The array is extended (i.e. copied to a larger allocated array) whenever
the array must be extended because the user uses a FIFO index which is
out of the current range or calls get_free_fifo() when there are no free FIFOs.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The FIFO sorter "fifo_sorter" array is just an array of pointers to the elements
of the real array "fifos". Then the qsort() function is used for sorting the
elements of the pointer-array "fifo_sorter" rather than the FIFOs themselves.
This is actually not a very efficient way of doing the prioritisation. It is
just easy to code. It would be more efficient to use some sort of percolation
sorting algorithm.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
If you are going to gradually increase the array size from a small value to a
large value, it would be best to set "resize_quantum" to a large value.
------------------------------------------------------------------------------*/
//----------------------//
//    cp_fifo_array::   //
//----------------------//
struct cp_fifo_array {
protected:                          // Should really make these private.
    cp_fifo**   fifos;              // Array of pointers to packet FIFOs.
    int         n_fifos;            // Number of packet FIFOs.
    int         last_fifo_alloc;    // Last FIFO allocated, for non-homing.

    cp_fifo**   fifo_sorter;        // Temp. array for sorting fifos by credit.
                                    // (Used in cp_buffer::fetch().)
    int         resize_quantum;     // Array resizing quantum.
public:
    int resize(int new_size);       // Resize FIFO array. (Never gets smaller.)
    int num_fifos() { return n_fifos; }
    int get_free_fifo();            // Get a free FIFO. (Always succeeds.)
    ulong n_packets();              // Get total number of packets. (Slow.)
    ulong n_packets(int chan);      // Get number of packets in channel. (Slow.)
    bool_enum empty();              // True if all fifos are empty.
    void fifo_sort();               // Sort fifos by decreasing "vcred" field.
    void set_resize_quantum(int i) { if (i >= 1) resize_quantum = i; }

//    cp_fifo_array& operator=(const cp_fifo_array& x) {}
//    cp_fifo_array(const cp_fifo_array& x) {};
    cp_fifo_array(int n = 0) {
        fifos = 0;
        n_fifos = 0;
        last_fifo_alloc = 0;
        fifo_sorter = 0;
        resize_quantum = 1;
        resize(n);
        }
    ~cp_fifo_array();
    }; // End of struct cp_fifo_array.

/*------------------------------------------------------------------------------
This class represents a packet buffer, which will implement round robin,
credit priority and other kinds of queueing algorithms.
The buffer is organised into FIFOs, one per channel.
The credit update algorithms used should take into account the value of
output_bitrate, because this is the maximum rate at which the buffer will be
served.
Note that the round robin weight cannot be set to 0. But the RR access can be
turned off by calling set_rr_active(chan, false).
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This class has been used as a passive buffer. In future, it should become a
class with attached objects which have virtual functions to handle the sending
of packets.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The min_pkt_wait() function tells you how long to wait if there are packets to
send but none of them currently have enough credit to transmit.
------------------------------------------------------------------------------*/
//----------------------//
//      cp_buffer::     //
//----------------------//
struct cp_buffer: public slink, public cp_fifo_array {
private:
    double  output_bitrate;         // Maximum output bit rate (bits/sec).
    double  last_credit_time;       // Last time a credit update occurred.
    double  last_delay_min;         // Previous min delay, if packets present.
    double  tx_debt;                // Current TX debt.
public:
    cp_buffer* next() const { return (cp_buffer*)slink::next(); }

    // User-settable control parameters.
//    buffer_style_t  buffer_style;   // Absolute or credit priority.
    int             trace;

    double min_pkt_wait() { return last_delay_min; }

    void set_output_bitrate(double r) { if (r >= 0) output_bitrate = r; }
    double get_output_bitrate() { return output_bitrate; }
    int check_bitrates(ostream& = cout);
    void set_last_credit_time(double r) { last_credit_time = r; }
    double get_tx_debt() { return tx_debt; }

    // Access to CP FIFO attributes for channel "chan".
    void set_lb_rate(int chan, double x);       // Set leaky bucket rate.
    void set_lb_rates(double x);                // Set all leaky bucket rates.
    double get_lb_rate(int chan);               // Get leaky bucket rate.
    bool_enum get_lb_rate_fixed(int chan);      // Get leaky bucket rate flag.

    void set_lb_cap(int chan, double x);        // Set leaky bucket capacity.
    double get_lb_cap(int chan);                // Get leaky bucket capacity.
    void set_lb_caps(double x);                 // Set all LB capacities.

    void set_rr_weight(int chan, double x);     // Set round robin weight.
    double get_rr_weight(int chan);             // Get round robin weight.

    void set_rr_active(int chan, bool_enum x);  // Set round robin active.
    bool_enum get_rr_active(int chan);          // Get round robin active.

    // Access to CP FIFO states.
    void set_pause(int chan);                   // Set pause flag, if active.
    void clr_pause(int chan);                   // Clear pause flag, if paused.
    void set_free(int chan);                    // Set a FIFO free.
    void set_finished(int chan);                // Mark a FIFO as finished.
    cp_fifo_state_t get_state(int chan)         // Get CP FIFO state.
        { return (chan < 0 || chan >= n_fifos) ? cfsFREE : fifos[chan]->state; }

    void reset_credit(double t);                // Reset credits and time base.
    int init(int verbosity = 0);                // Initialise cp_buffer for use.

    // Byte-array version of the cp_pkt store() function.
    int store(int chan, const char*, int len);  // Store a packet.

    // Packet versions of store and fetch functions.
    int store(int chan, cp_pkt* p0);            // Store a packet.
    int fetch_abs_prio(int& chan, cp_pktlist& pkts, double t);
    int fetch(int& chan, cp_pktlist& pkts, double t);

    // To be redefined in derived class to return the numbers of
    // bits in the physical layer for the encapsulated packet.
    virtual int pkt_bit_count(int len) { return len * 8; }

    void print(ostream& os = cout);

//    cp_buffer& operator=(const cp_buffer& x) {};
//    cp_buffer(const cp_buffer& x) {};
    cp_buffer(int n = 0) : cp_fifo_array(n) {
        output_bitrate = 0;
        last_credit_time = 0;
        last_delay_min = 0;
//        buffer_style = bsCREDIT_PRIORITY;
        trace = 0;
        tx_debt = 0;
        }
    virtual ~cp_buffer() {}
    }; // End of struct cp_buffer.

//----------------------//
//    cp_bufferlist::   //
//----------------------//
struct cp_bufferlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    cp_buffer* first() const { return (cp_buffer*)s2list::first(); }
    cp_buffer* last() const { return (cp_buffer*)s2list::last(); }
    void append(cp_buffer* p) { s2list::append(p); }
    void prepend(cp_buffer* p) { s2list::prepend(p); }
    cp_buffer* popfirst() { return (cp_buffer*)s2list::popfirst(); }
    cp_buffer* poplast() { return (cp_buffer*)s2list::poplast(); }
    cp_buffer* remove(cp_buffer* p)
        { return (cp_buffer*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(cp_buffer* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(cp_bufferlist& l) { s2list::swallow(&l); }
    void gulp(cp_bufferlist& l) { s2list::gulp(&l); }
    void clear() { for (cp_buffer* p = first(); p; )
        { cp_buffer* q = p->next(); delete p; p = q; } clearptrs(); }

//    cp_bufferlist& operator=(const cp_bufferlist& x) {}
//    cp_bufferlist(const cp_bufferlist& x) {};
    cp_bufferlist() {}
    ~cp_bufferlist() { clear(); }
    }; // End of struct cp_bufferlist.

#endif /* AKSL_CPBUF_H */
