// src/aksl/cpbuf.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

cp_pkt::
    copy_in
    copy_out
fifo_compare_neg
cp_fifo_array::
    ~cp_fifo_array
    resize
    get_free_fifo
    n_packets
    n_packets
    empty
    fifo_sort
cp_buffer::
    check_bitrates
    set_lb_rate
    set_lb_rates
    get_lb_rate
    get_lb_rate_fixed
    set_lb_cap
    set_lb_caps
    get_lb_cap
    set_rr_weight
    get_rr_weight
    set_rr_active
    get_rr_active
    set_pause
    clr_pause
    set_free
    set_finished
    reset_credit
    init
    store
    store
    fetch_abs_prio
    fetch
    print
------------------------------------------------------------------------------*/

// AKSL header files.
#include "aksl/cpbuf.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files.
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif
#ifndef AKSL_X_FLOAT_H
#define AKSL_X_FLOAT_H
#include <float.h>
#endif

// Arbitrary constants.
const double deft_lb_cap = 9000 * 8;        // Default 9000 byte LB capacity.

// Block memory class for packets, because of (alleged) solaris bug in malloc.
#if USE_CP_PKT_BMEM
bmem_define(cp_pkt, bmem0);
bmem_define(udp_cp_pkt, bmem0);
#endif

//----------------------//
//    cp_pkt::copy_in   //
//----------------------//
void cp_pkt::copy_in(const char* p0, int len0) {
    // Check for errors in arguments.
    if ((!p0 && len0 > 0) || len0 < 0)
        return;

    // Delete old memory, if any, and make a copy of the byte-array.
    delete[] buf0;
    if (len0 == 0)
        buf0 = 0;
    else {
        buf0 = new char[len0];
        memcpy(buf0, p0, len0);     // Copy from p0 to buf0.
        }
    len = len0;
    } // End of function cp_pkt::copy_in.

/*------------------------------------------------------------------------------
If an error occurs, -1 is returned.
A particular error condition is "len < 0", which indicates "no packet", not a
packet of zero length. This is the initial state of a cp_pkt if it has not been
set to anything.

Otherwise, the packet contents are copied to the given char array, which is
assumed to be of length "buflen". The return value is then the number of bytes
copied. If the buffer "buf" is too short, the copied packet is truncated.
------------------------------------------------------------------------------*/
//----------------------//
//   cp_pkt::copy_out   //
//----------------------//
int cp_pkt::copy_out(char* buf, int buflen) {
    // Error conditions.
    if (!buf || buflen < 0 || (!buf0 && len > 0) || len < 0)
        return -1;

    // Zero-length packet.
    if (len == 0)
        return 0;

    // Positive-length packet.
    if (buflen > len)
        buflen = len;
    memcpy(buf, buf0, buflen);     // Copy from buf0 to buf.
    return buflen;
    } // End of function cp_pkt::copy_out.

/*------------------------------------------------------------------------------
This function is intended to be used in a qsort() sort.
It compares the "vcred" values of two fifos, given the addresses of pointers to
"cp_fifo" structures.
The value returned is the negative of the usual comparison, because the sorting
desired is _decreasing_ order. That's why "_neg" is appended to the function
name.
------------------------------------------------------------------------------*/
//----------------------//
//   fifo_compare_neg   //
//----------------------//
int fifo_compare_neg(const void* p1, const void* p2) {
    // Deal with special case that one or more of the cp_fifo pointers
    // is a null pointer.
    // This should never happen, but if it does, then the null pointer is
    // assumed to be "less than" any non-null pointer.
    if (!*(cp_fifo**)p1) {
        if (!*(cp_fifo**)p2)
            return 0;           // p1 == p2.
        else
            return 1;           // p1 < p2.
        }
    if (!*(cp_fifo**)p2)
        return -1;              // p1 > p2.

    // Deal with special case that one or more of the cp_fifo objects
    // is not offering packets.
    // If this happens, then any non-offering cp_fifo object is
    // assumed to be "less than" any offering cp_fifo object.
    if (!(*(cp_fifo**)p1)->offering) {
        if (!(*(cp_fifo**)p2)->offering)
            return 0;           // p1 == p2.
        else
            return 1;           // p1 < p2.
        }
    if (!(*(cp_fifo**)p2)->offering)
        return -1;              // p1 > p2.

    // If both cp_fifo objects are offereing, just compare their vcred values.
    double vcred1 = (*(cp_fifo**)p1)->vcred;
    double vcred2 = (*(cp_fifo**)p2)->vcred;

    if (vcred1 > vcred2)
        return -1;              // p1 > p2.
    if (vcred1 < vcred2)
        return 1;               // p1 < p2.
    return 0;                   // p1 == p2.
    } // End of function fifo_compare_neg.

//----------------------------------//
//   cp_fifo_array::~cp_fifo_array  //
//----------------------------------//
cp_fifo_array::~cp_fifo_array() {
    // First delete all of the FIFOs.
    for (int i = 0; i < n_fifos; ++i) {
        delete fifos[i];
        fifos[i] = 0;
        fifo_sorter[i] = 0;
        }
    // The following lines do not call any destructors, hopefully.
#if defined(SOLARIS) || defined(linux)
    delete[] fifos;
    delete[] fifo_sorter;
#else
    delete[n_fifos] fifos;
    delete[n_fifos] fifo_sorter;
#endif
    } // End of function cp_fifo_array::~cp_fifo_array.

/*------------------------------------------------------------------------------
Create a larger array of fifo pointers, and copy the old fifo pointers into it.
The return value is the new array size.
------------------------------------------------------------------------------*/
//--------------------------//
//   cp_fifo_array::resize  //
//--------------------------//
int cp_fifo_array::resize(int new_size) {
    // Never make the array smaller.
    if (new_size <= n_fifos)
        return n_fifos;

    // Quantize the jump in the array size.
    if (resize_quantum > 1 && new_size < n_fifos + resize_quantum)
        new_size = n_fifos + resize_quantum;

    // Create two new arrays of pointers-to-FIFOs.
    cp_fifo** p1 = new cp_fifo*[new_size];
    cp_fifo** p2 = new cp_fifo*[new_size];

    // Copy the old pointers to FIFOs into the new pointer array.
    for (int i = 0; i < n_fifos; ++i) {
        p1[i] = fifos[i];
        fifos[i] = 0;   // Not necessary, but does no harm to be safe!
        }

    // Create new FIFOs to fill the empty gap.
    for (FOR_DECL(int) i = n_fifos; i < new_size; ++i)
        p1[i] = new cp_fifo(i);

    // Initialise the new fifo-pointer sorting-array.
    for (FOR_DECL(int) i = 0; i < new_size; ++i)
        p2[i] = p1[i];

    // Delete old stuff and point to the new stuff.
    // Hopefully the delete/free() algorithms will cope.
#if defined(SOLARIS) || defined(linux)
    delete[] fifos;
    delete[] fifo_sorter;
#else
    delete[n_fifos] fifos;
    delete[n_fifos] fifo_sorter;
#endif
    fifos = p1;
    n_fifos = new_size;
    fifo_sorter = p2;
    return n_fifos;
    } // End of function cp_fifo_array::resize.

/*------------------------------------------------------------------------------
This function tries to efficiently locate a free FIFO.
A non-homing seek algorithm is used to find a free FIFO.
That is, a rotation pointer (last_fifo_alloc) keeps track of the last FIFO
allocated, and starts from there next time an allocation occurs.
If all FIFOs are in use, then a new FIFO is created.
This function always succeeds and returns the index of a free FIFO.
This function marks the allocated FIFO as `not free'.
To free up the FIFO again, call function cp_fifo::free_fifo() or
function cp_buffer::set_free().
------------------------------------------------------------------------------*/
//------------------------------//
// cp_fifo_array::get_free_fifo //
//------------------------------//
int cp_fifo_array::get_free_fifo() {
    // The initial case of an empty fifo array.
    if (!fifos || n_fifos <= 0) {
        resize(1);
        fifos[0]->state = cfsACTIVE;
        last_fifo_alloc = 0;
        return last_fifo_alloc;
        }

    // Do a search of all current FIFOs for a free one, starting at the last
    // allocated FIFO. I.e. Check the last allocated FIFO last.
    // This algorithm always terminates iff 0 <= last_fifo_array < n_fifos.
    int i = last_fifo_alloc;
    for (;;) {
        i += 1;                 // Start seeking just after the last choice.
        if (i >= n_fifos)       // Wraparound at end of array.
            i = 0;

        // The case that a free pre-existing FIFO is found.
        if (fifos[i]->state == cfsFREE) {
            fifos[i]->state = cfsACTIVE;
            last_fifo_alloc = i;
            return last_fifo_alloc;
            }

        // If all FIFOs have been checked, break and create a new FIFO.
        if (i == last_fifo_alloc)
            break;
        }

    // The case that no free FIFOs were found. Create a new FIFO.
    last_fifo_alloc = n_fifos;
    resize(last_fifo_alloc + 1);
    fifos[last_fifo_alloc]->state = cfsACTIVE;
    return last_fifo_alloc;
    } // End of function cp_fifo_array::get_free_fifo.

//--------------------------//
// cp_fifo_array::n_packets //
//--------------------------//
ulong cp_fifo_array::n_packets() {
    ulong x = 0;
    for (int i = 0; i < n_fifos; ++i)
        x += fifos[i]->pkts.length();
    return x;
    } // End of function cp_fifo_array::n_packets.

//--------------------------//
// cp_fifo_array::n_packets //
//--------------------------//
ulong cp_fifo_array::n_packets(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return 0;
    return fifos[chan]->pkts.length();
    } // End of function cp_fifo_array::n_packets.

/*------------------------------------------------------------------------------
Returns true if and only if all current fifos are empty.
------------------------------------------------------------------------------*/
//----------------------//
// cp_fifo_array::empty //
//----------------------//
bool_enum cp_fifo_array::empty() {
    for (int i = 0; i < n_fifos; ++i)
        if (!fifos[i]->pkts.empty())
            return false;
    return true;
    } // End of function cp_fifo_array::empty.

/*------------------------------------------------------------------------------
The hard work here is done in the fifo_compare_neg() function.
------------------------------------------------------------------------------*/
//--------------------------//
// cp_fifo_array::fifo_sort //
//--------------------------//
void cp_fifo_array::fifo_sort() {
    if (n_fifos <= 1)
        return;
    qsort(fifo_sorter, n_fifos, sizeof(cp_fifo*), fifo_compare_neg);
    } // End of function cp_fifo_array::fifo_sort.

/*------------------------------------------------------------------------------
This function returns 0 if the sum of the fifo bitrates is less than
or equal to the overall maximum bitrate. Otherwise -1 is returned.
As a side-effect, some information is sent to the specified output stream.
------------------------------------------------------------------------------*/
//------------------------------//
//  cp_buffer::check_bitrates   //
//------------------------------//
int cp_buffer::check_bitrates(ostream& os) {
    // Find out the amount of available bandwidth.
    double total_free_rate = output_bitrate;

    // Subtract each per-fifo bitrate guarantee.
    for (int i = 0; i < n_fifos; ++i) {
        if (fifos[i]->state == cfsFREE)
            continue;
        total_free_rate -= fifos[i]->lb_rate;
        os << "lb_rate[" << i << "] = " << fifos[i]->lb_rate << NL;
        }
    os << "total rate = " << output_bitrate << endl;

    int ret = 0;
    if (total_free_rate < 0) {
        os << "Warning: insufficient TX bandwidth."
              " Deficit = " << (-total_free_rate) << endl;
        ret = -1;
        }
    else
        os << "Free TX bandwidth = " << total_free_rate << endl;
    return ret;
    } // End of function cp_buffer::check_bitrates.

/*------------------------------------------------------------------------------
This is actually only the setup of the requested value, and the final values are
determined later.
------------------------------------------------------------------------------*/
//--------------------------//
//  cp_buffer::set_lb_rate  //
//--------------------------//
void cp_buffer::set_lb_rate(int chan, double x) {
    if (chan < 0 || chan >= n_fifos || x < 0)
        return;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return;
    p->lb_rate = x;
    p->lb_rate_fixed = true;
    } // End of function cp_buffer::set_lb_rate.

/*------------------------------------------------------------------------------
Set the LB rates of all FIFOs which have not yet been fixed by a call to the
set_lb_rate() function.
------------------------------------------------------------------------------*/
//--------------------------//
// cp_buffer::set_lb_rates  //
//--------------------------//
void cp_buffer::set_lb_rates(double x) {
    if (x < 0)
        return;
    for (int chan = 0; chan < n_fifos; ++chan) {
        register cp_fifo* p = fifos[chan];
        if (!p->lb_rate_fixed && p->state != cfsFREE
                              && p->state != cfsFINISHED)
            p->lb_rate = x;
        }
    } // End of function cp_buffer::set_lb_rates.

//--------------------------//
//  cp_buffer::get_lb_rate  //
//--------------------------//
double cp_buffer::get_lb_rate(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return 0;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return 0;
    return p->lb_rate;
    } // End of function cp_buffer::get_lb_rate.

/*------------------------------------------------------------------------------
This function tells the caller whether a FIFO has a fixed LB rate already.
Then the unallocated bandwidth can be shared between the non-fixed FIFOs.
------------------------------------------------------------------------------*/
//------------------------------//
// cp_buffer::get_lb_rate_fixed //
//------------------------------//
bool_enum cp_buffer::get_lb_rate_fixed(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return true;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return true;
    return p->lb_rate_fixed;
    } // End of function cp_buffer::get_lb_rate_fixed.

//--------------------------//
//   cp_buffer::set_lb_cap  //
//--------------------------//
void cp_buffer::set_lb_cap(int chan, double x) {
    if (chan < 0 || chan >= n_fifos || x < 0)
        return;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return;
    p->lb_cap = x;
    p->lb_cap_fixed = true;
    } // End of function cp_buffer::set_lb_cap.

//--------------------------//
//  cp_buffer::set_lb_caps  //
//--------------------------//
void cp_buffer::set_lb_caps(double x) {
    if (x < 0)
        return;
    for (int chan = 0; chan < n_fifos; ++chan) {
        register cp_fifo* p = fifos[chan];
        if (!p->lb_cap_fixed && p->state != cfsFREE
                             && p->state != cfsFINISHED)
            p->lb_cap = x;
        }
    } // End of function cp_buffer::set_lb_caps.

//--------------------------//
//  cp_buffer::get_lb_cap   //
//--------------------------//
double cp_buffer::get_lb_cap(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return 0;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return 0;
    return p->lb_cap;
    } // End of function cp_buffer::get_lb_cap.

/*------------------------------------------------------------------------------
If you want to set an RR weight of zero, call set_rr_active(chan, false).
------------------------------------------------------------------------------*/
//--------------------------//
// cp_buffer::set_rr_weight //
//--------------------------//
void cp_buffer::set_rr_weight(int chan, double x) {
    if (chan < 0 || chan >= n_fifos || x <= 0)
        return;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return;
    p->rr_rate = 1/x;
    } // End of function cp_buffer::set_rr_weight.

//--------------------------//
// cp_buffer::get_rr_weight //
//--------------------------//
double cp_buffer::get_rr_weight(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return 1;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return 1;
    double r = p->rr_rate;
    return (r > 0) ? (1/r) : 0;
    } // End of function cp_buffer::get_rr_weight.

//--------------------------//
// cp_buffer::set_rr_active //
//--------------------------//
void cp_buffer::set_rr_active(int chan, bool_enum x) {
    if (chan < 0 || chan >= n_fifos)
        return;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return;
    p->rr_active = x;
    } // End of function cp_buffer::set_rr_active.

//--------------------------//
// cp_buffer::get_rr_active //
//--------------------------//
bool_enum cp_buffer::get_rr_active(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return true;
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return true;
    return p->rr_active;
    } // End of function cp_buffer::get_rr_active.

/*------------------------------------------------------------------------------
Make the indicated FIFO pause if it is active.
------------------------------------------------------------------------------*/
//--------------------------//
//   cp_buffer::set_pause   //
//--------------------------//
void cp_buffer::set_pause(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return;
    if (fifos[chan]->state == cfsACTIVE)
        fifos[chan]->state = cfsPAUSE;
    } // End of function cp_buffer::set_pause.

/*------------------------------------------------------------------------------
Make the indicated FIFO active if it is paused.
------------------------------------------------------------------------------*/
//--------------------------//
//   cp_buffer::clr_pause   //
//--------------------------//
void cp_buffer::clr_pause(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return;
    if (fifos[chan]->state == cfsPAUSE)
        fifos[chan]->state = cfsACTIVE;
    } // End of function cp_buffer::clr_pause.

/*------------------------------------------------------------------------------
This function has serious consequences. It deletes all packets (cp_pkt objects)
in the FIFO's buffer, along with any virtual packets that may be attached to
these packets. Therefore do not call this function from any member function of
any packet or virtual packet that might potentially be deleted.
------------------------------------------------------------------------------*/
//--------------------------//
//   cp_buffer::set_free    //
//--------------------------//
void cp_buffer::set_free(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return;
    fifos[chan]->free_fifo();
    } // End of function cp_buffer::set_free.

/*------------------------------------------------------------------------------
Mark the indicated FIFO as finished if it is not free, i.e. if it is paused or
active.
------------------------------------------------------------------------------*/
//--------------------------//
// cp_buffer::set_finished  //
//--------------------------//
void cp_buffer::set_finished(int chan) {
    if (chan < 0 || chan >= n_fifos)
        return;
    if (fifos[chan]->state != cfsFREE)
        fifos[chan]->state = cfsFINISHED;
    } // End of function cp_buffer::set_finished.

/*------------------------------------------------------------------------------
Reset the credit of all non-free, non-finished FIFOs.
------------------------------------------------------------------------------*/
//--------------------------//
//  cp_buffer::reset_credit //
//--------------------------//
void cp_buffer::reset_credit(double t) {
    for (int i = 0; i < n_fifos; ++i) {
        register cp_fifo* p = fifos[i];
        if (!p->lb_cap_fixed && p->state != cfsFREE
                             && p->state != cfsFINISHED)
            p->reset_credit();
        }
    set_last_credit_time(t);
    } // End of function cp_buffer::reset_credit.

/*------------------------------------------------------------------------------
This function initialises the CP buffer.
-   It sets "tx_debt" to 0.
-   It allocates not-yet-allocated bandwidth (lb_rates).
Return value:
0       success
< 0     failure
------------------------------------------------------------------------------*/
//----------------------//
//    cp_buffer::init   //
//----------------------//
int cp_buffer::init(int verbosity) {
    tx_debt = 0;        // Current debt for buffer output (bits).

    // Find out the amounts of allocated and free bandwidth.
    double total_free_rate = output_bitrate;
    double total_free_fifos = n_fifos;
    for (int i = 0; i < n_fifos; ++i)
        if (get_lb_rate_fixed(i)) {
            total_free_rate -= get_lb_rate(i);
            total_free_fifos -= 1;
            }
    if (total_free_rate < 0) {
        cout << "Insufficient bandwidth for allocations:\n";
        cout << "Total = " << output_bitrate << NL;
        cout << "Deficit = " << total_free_rate << endl;
        return -1;
        }
    if (total_free_fifos > 0)
        set_lb_rates(total_free_rate/total_free_fifos);
    set_lb_caps(deft_lb_cap);
    if (verbosity >= 1) {
        cout << "Bitrate allocations:\n";
        for (int i = 0; i < n_fifos; ++i) {
            register cp_fifo* p = fifos[i];
            if (p->state == cfsFREE || p->state == cfsFINISHED)
                continue;
            cout << "    Fifo " << i << ": "
                 << p->lb_rate << " bits/sec\n";
            }
        cout << endl;
        }
    return 0;
    } // End of function cp_buffer::init.

/*------------------------------------------------------------------------------
This function appends the given packet (passed as a simple char array) to the
FIFO for the given channel.
It is better to use the cp_pkt version of store().
This function is here for the convenience of users who want to enqueue a
simple byte array rather than creating the cp_pkt structure themselves.
------------------------------------------------------------------------------*/
//----------------------//
//   cp_buffer::store   //
//----------------------//
int cp_buffer::store(int chan, const char* pc, int len) {
    // Check that the FIFO index is in range.
    if (chan < 0 || chan >= n_fifos)
        return -1;

    // Check for errors in arguments (permit zero-length packets).
    if ((!pc && len > 0) || len < 0)
        return -1;

    // Don't permit storing packets in free or finished FIFOs.
    register cp_fifo* p = fifos[chan];
    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return -1;

    // Copy the byte-array into a (plain, base-class) cp_pkt, and queue it.
    cp_pkt* p1 = new cp_pkt;
    p1->copy_in(pc, len);
    p->pkts.append(p1);
    return 0;
    } // End of function cp_buffer::store.

/*------------------------------------------------------------------------------
This function appends the given packet to the FIFO for the given channel.
------------------------------------------------------------------------------*/
//----------------------//
//   cp_buffer::store   //
//----------------------//
int cp_buffer::store(int chan, cp_pkt* p0) {
//    cout << "cp_buffer::store: chan = " << chan << NL;

    // Check the range of "chan" and non-zero "p0".
    if (chan < 0 || chan >= n_fifos || !p0)
        return -1;

    // Don't permit storing packets in free or finished FIFOs.
    register cp_fifo* p = fifos[chan];

//    cout << "cp_buffer::store: p->state = " << p->state << NL;

    if (p->state == cfsFREE || p->state == cfsFINISHED)
        return -1;

//    cout << "cp_buffer::store: appending cp_pkt\n";

    // Queue the packet:
    p->pkts.append(p0);
    return 0;
    } // End of function cp_buffer::store.

/*------------------------------------------------------------------------------
This function fetches a packet from the FIFO of lowest index which has a packet.
This gives absolute priority to FIFO 0 over all other FIFOs, and so forth.
The parameter "t0" is expected to be the time in Unix seconds since 1 Jan 1970.
If an error occurs, then the return value is -1.
If there is no packet to fetch (i.e. the buffer is empty), then the return value
is 0, and the channel number "chan" is set to -1.
If a packet is found, it is appended to "pkts", the channel index is copied to
"chan", and the return value is the number of bytes copied.
At most one packet is copied to "pkts" in a single call to this function.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Return value        chan            Description
-1                  <not set>       Bad function parameter.
-1                  <not set>       Failed to copy a packet (internal error).
0                   -1              The buffer is empty.
bytes_copied >= 0   channel >= 0    A packet has been successfully fetched.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note: zero-length packets seem to be permitted by this function.
------------------------------------------------------------------------------*/
//------------------------------//
//   cp_buffer::fetch_abs_prio  //
//------------------------------//
int cp_buffer::fetch_abs_prio(int& chan, cp_pktlist& pkts, double t0) {
    // Fetch a packet from one of the FIFOs.
    // This is absolute priority buffering.
    // Non-empty FIFO with lowest index is served first.
    cp_pkt* pcp = 0;
    int i_best = -1;
    for (int i = 0; i < n_fifos; ++i) {
        // Ignore inactive FIFOs.
        register cp_fifo* p0 = fifos[i];
        if (p0->state != cfsACTIVE) {
            // Convert any finished FIFO to a free FIFO.
            if (p0->state == cfsFINISHED)
                p0->free_fifo();
            continue;
            }

        // Ignore empty FIFOs.
        pcp = p0->pkts.first();
        if (!pcp)
            continue;

        // If the packet is not virtual, dequeue it and exit the loop.
        virtual_pkt* vp1 = pcp->get_virtual_pkt();
        if (!vp1) {
            pcp = p0->pkts.popfirst();
            i_best = i;
            break;
            }

        // If the packet is virtual, ask it to construct a packet.
        // This could have the side effect of setting vp1->finished to true.
        cp_pkt* pkt1 = vp1->packet_create(t0);

        // If the stream for this FIFO is finished, free the packet.
        // After this operation, the attached virtual packet is also deleted!
        // The FIFO itself continues to be active.
        if (vp1->finished) {
            // Delete the packet at the head of the list.
            p0->pkts.delfirst();

            // Do not make use of packet pkt1, even if it is non-zero.
            delete pkt1;
            continue;
            }

        // If no packet was created, progress to the next FIFO.
        if (!pkt1) {
            pcp = 0;
            continue;
            }

        // Successful return after getting a virtual packet.
        pkts.append(pkt1);
        chan = i;
        return pkt1->n_bytes();
        }

    // The case that no packet was found.
    if (!pcp) {
        chan = -1;
        return 0;
        }

    // Successful return after getting a non-virtual packet.
    pkts.append(pcp);
    chan = i_best;
    return pcp->n_bytes();
    } // End of function cp_buffer::fetch_abs_prio.

/*------------------------------------------------------------------------------
This function fetches a packet from the most deserving FIFO.
The algorithm used to choose the FIFO is a combination of leaky bucket
and weighted round robin.
The parameter "t" should be the time in Unix seconds since 1 Jan 1970.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
If an error occurs, then the return value is -1.
If there is no packet to fetch (i.e. the buffer is empty), then the return value
is 0, and the channel number "chan" is set to -1.
If a packet is found, it is appended to "pkts" the channel index is copied to
"chan", and the return value is the number of bytes copied.
Note that either 0 or 1 packets are appended to "pkts".
Under no circumstances will more than 1 packet be appended.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Return value        chan            Description
-1                  <not set>       Bad function parameter.
-1                  <not set>       Failed to copy a packet (internal error).
0                   -1              The buffer is empty, or not enough credit.
bytes_copied >= 0   channel >= 0    A packet has been successfully fetched.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
If no packet is returned, then last_delay_min is set to 0 if there were no
packets in the CP buffer at all. If packets are present, but none can be sent,
then the minimum time required to wait until one of them will be ready to send
is assigned to last_delay_min.
Thus if return value == 0 and chan == -1, the value of last_delay_min
distinguishes between:

last_delay_min      meaning
0                   the buffer really has no packets to send
> 0                 in last_delay_min seconds, a packet is expected to be ready
------------------------------------------------------------------------------*/
//----------------------//
//   cp_buffer::fetch   //
//----------------------//
int cp_buffer::fetch(int& chan, cp_pktlist& pkts, double t0) {
    if (trace >= 10) {
        cout << flush;
        cerr << "Starting cp_buffer::fetch() at time " << t0 << " seconds.\n";
        cout << "cp_buffer::fetch: last credit time = " << last_credit_time
             << " seconds\n";
        }
    last_delay_min = 0;
    if (t0 < 0)
        return -1;
    if (n_fifos <= 0) {
        chan = -1;
        if (trace >= 10)
            cout << "cp_buffer::fetch: n_fifos <= 0\n";
        return 0;
        }

    // The case of uninitialised "last_credit_time".
    // [This is unnecessary if set_last_credit_time() has been called.]
//    if (last_credit_time == 0)
//        last_credit_time = t0;

    // Find out how much time has elapsed since the last call.
    double dt = t0 - last_credit_time;
    if (dt < 0)
        dt = 0;
    last_credit_time = t0;

    if (trace >= 10) {
        cout << "cp_buffer::fetch: time interval since last call = " << dt
             << " seconds\n";
        }

    // Update the LB credit values.
    // Also use this opportunity to free all FIFOs which are finished.
    for (int i = 0; i < n_fifos; ++i) {
        register cp_fifo* p0 = fifos[i];
        if (p0->state == cfsFREE)
            continue;
        // If the CP FIFO is marked as "finished", free it.
        if (p0->state == cfsFINISHED) {
            p0->free_fifo();
            continue;
            }
        p0->lb_cred += p0->lb_rate * dt;
        if (p0->lb_cred > p0->lb_cap)
            p0->lb_cred = p0->lb_cap;
        }

    // Phase I, part 1.
    // Scan all FIFOs for the best virtual (i.e. after-TX) credit.
    // (This ensures that no FIFO shall get into debt.)
    bool_enum pkt_found = false;
    for (FOR_DECL(int) i = 0; i < n_fifos; ++i) {
        // If the CP FIFO is not active, ignore it.
        // (Note that the credit update converts finished FIFOs to free.
        // So at this point, there can be no FIFOs in the finished state.)
        register cp_fifo* p0 = fifos[i];
        if (p0->state != cfsACTIVE) {
            p0->vcred = -DBL_MAX;       // Used by fifo_sort().
            p0->offering = false;       // Used in Phase I, part 2.
            continue;
            }

        // See if the FIFO has any packets to send.
        cp_pkt* p = p0->pkts.first();

        // Ignore the FIFO if it has no packets.
        if (!p) {
            p0->vcred = -DBL_MAX;
            p0->offering = false;
            continue;
            }

        // The case of a virtual packet. Check if it is finished already.
        for (;;) {
            // If the packet is non-virtual, continue with the main loop.
            virtual_pkt* pvp0 = p->get_virtual_pkt();
            if (!pvp0)
                break;

            // If the virtual packet is not finished, continue to main loop.
            if (!pvp0->finished)
                break;

            // If the virtual packet _is_ finished, dequeue and delete it.
            p0->pkts.delfirst();

            // Fetch the next packet, if there is one.
            p = p0->pkts.first();

            // If there is no next packet, mark the FIFO as not offering.
            // (Otherwise the next packet is considered for service.)
            if (!p) {
                p0->vcred = -DBL_MAX;
                p0->offering = false;
                break;
                }
            }
        // If no packet came out of this loop, ignore this FIFO.
        if (!p)
            continue;

        // The case of an active FIFO with an initially non-finished packet.
        // Evaluate FIFO credit until a real packet offer is obtained.
        for (;;) {
            // The length() function calls the virtual packet if there is one.
            p0->vcred = p0->lb_cred - pkt_bit_count(p->length(t0));

            // If it just went inactive, then skip.
            if (p0->state != cfsACTIVE) {
                p0->vcred = -DBL_MAX;
                p0->offering = false;
                p = 0;
                break;
                }

            // Check if packet_size() has set "finished" to true.
            // If the packet is non-virtual, continue with the main loop.
            virtual_pkt* pvp0 = p->get_virtual_pkt();
            if (!pvp0)
                break;

            // If the virtual packet is not finished, continue to main loop.
            if (!pvp0->finished)
                break;

            // If the virtual packet _is_ finished, dequeue and delete it.
            p0->pkts.delfirst();

            // Fetch the next packet, if there is one.
            p = p0->pkts.first();

            // If there is no next packet, mark the FIFO as not offering.
            // (Otherwise the next packet is considered for service.)
            if (!p) {
                p0->vcred = -DBL_MAX;
                p0->offering = false;
                break;
                }
            }
        // If no packet came out of this loop, ignore this FIFO.
        if (!p)
            continue;

        // If we get to here, then the FIFO may be offering a packet.
        p0->offering = true;
        pkt_found = true;

        if (trace >= 10) {
            cout << "cp_buffer::fetch: number of packets for FIFO[" << i
                 << "] = " << p0->pkts.length() << NL;
            }
        }

    // If there are no packets being offered, then return.
    if (!pkt_found) {
        chan = -1;
        return 0;
        }

    // Sort the "fifo_sorter" pointer table (in order of decreasing vcred).
    fifo_sort();

    // Phase I, part 2.
    // Try to get a packet from one of the sorted LB candidates.
    for (FOR_DECL(int) i = 0; i < n_fifos; ++i) {
        // Find the next fifo in decreasing order of virtual credit.
        cp_fifo* p0 = fifo_sorter[i];
        if (!p0->offering)
            continue;           // This could be replaced with "break".
        if (p0->vcred < 0) {
            // Calculate the expected time before the packet will be ready:
            if (p0->lb_rate > 0) {
                // Assume here for efficiency that virtual pkts don't renege.
                double delay = -p0->vcred/p0->lb_rate;
                if (last_delay_min == 0 || last_delay_min > delay)
                    last_delay_min = delay;
                }
            continue;           // All others must be negative too.
            }

        // See if a packet is still being offered.
        cp_pkt* p_best = p0->pkts.first();

        // Loop until a packet is obtained or the FIFO is empty.
        // Exit this loop with either p_best == 0 or equal to best candidate.
        for (;;) {
            // The FIFO is empty. So ignore this FIFO.
            if (!p_best) {
                p0->offering = false;
                break;
                }

            // Check to see if this is a virtual packet.
            virtual_pkt* vp1 = p_best->get_virtual_pkt();
            if (!vp1) {
                // It's a non-virtual packet. So pop it and break to send it.
                p_best = p0->pkts.popfirst();
                break;
                }

            // It's a virtual packet. So get it to create a real packet.
            cp_pkt* pkt1 = vp1->packet_create(t0);

            // If the virtual packet is not finished, break to send pkt1.
            if (!vp1->finished) {
                // If pkt1 is non-null, it will be sent.
                // If pkt1 is null, it will make the main loop continue.
                p_best = pkt1;
                if (!p_best)
                    p0->offering = false;
                break;
                }

            // The virtual packet _has_ finished. So delete any packet
            // that was returned by packet_create(). (A formality.)
            delete pkt1;

            // Dequeue and delete the underlying packet in the FIFO.
            p0->pkts.delfirst();

            // Fetch the next packet, if there is one. Then loop.
            p_best = p0->pkts.first();
            }

        // Couldn't get the promised packet. So try another FIFO.
        if (!p_best)
            continue;

        // Update [reduce] the credit of the FIFO.
        p0->lb_cred = p0->vcred;

        // Return the packet.
        pkts.append(p_best);
        chan = p0->index;           // Not usually equal to i.
        return p_best->n_bytes();
        }

    // Phase II, part 1.
    // If the program gets to here, then no offering FIFO had sufficient credit.
    // So find the FIFO with the best RR credit:
    int n_rr_fifos = 0; // Number of FIFOs in the RR game.
    for (FOR_DECL(int) i = 0; i < n_fifos; ++i) {
        // Ignore previously ruled-out FIFOs.
        register cp_fifo* p0 = fifos[i];
        if (!p0->offering)
            continue;

        // Look at first packet. If no packet, or RR not active, ignore FIFO.
        cp_pkt* p = p0->pkts.first();
        if (!p || !p0->rr_active) {
            p0->offering = false;
            continue;
            }

        // Evaluate FIFO credit until a real packet offer is obtained.
        for (;;) {
            // Calculate credit including accumulated RR credit.
            // [The rule is: vcred = credit_deficit * rr_rate + rr_credit.]
            // The length() function calls the virtual packet if there is one.
            p0->vcred = (p0->lb_cred - pkt_bit_count(p->length(t0)))
                       * p0->rr_rate + p0->rr_cred;

            // If it just went inactive, then skip.
            if (p0->state != cfsACTIVE) {
                p0->vcred = -DBL_MAX;
                p0->offering = false;
                p = 0;
                break;
                }

            // Check if packet_size() has set "finished" to true.
            // If the packet is non-virtual, continue with the main loop.
            virtual_pkt* pvp0 = p->get_virtual_pkt();
            if (!pvp0)
                break;

            // If the virtual packet is not finished, continue to main loop.
            if (!pvp0->finished)
                break;

            // If the virtual packet _is_ finished, dequeue and delete it.
            p0->pkts.delfirst();

            // Fetch the next packet, if there is one.
            p = p0->pkts.first();

            // If there is no next packet, mark the FIFO as not offering.
            // (Otherwise loop to consider the next packet for service.)
            if (!p) {
                p0->vcred = -DBL_MAX;
                p0->offering = false;
                break;
                }
            }
        // If no packet came out of this loop, ignore this FIFO.
        if (!p)
            continue;

        // Keep count of the FIFOs still in the race.
        n_rr_fifos += 1;
        }

    if (n_rr_fifos <= 0) { // May happen if some nodes are not RR-active.
        chan = -1;
        return 0;
        }

    // Sort the "fifo_sorter" pointer table again.
    fifo_sort();

    // Phase II, part 2.
    // Try to get a packet from one of the successful RR candidates.
    for (FOR_DECL(int) i = 0; i < n_fifos; ++i) {
        // Find the next fifo in decreasing order of virtual credit.
        cp_fifo* p0 = fifo_sorter[i];
        if (!p0->offering)
            continue;           // This should be replaced with "break".

        // See if a packet is still being offered.
        cp_pkt* p_best = p0->pkts.first();

        // Loop until a packet is obtained or the FIFO is empty.
        // Exit this loop with either p_best == 0 or equal to best candidate.
        for (;;) {
            // The FIFO is empty. So ignore this FIFO.
            if (!p_best) {
                p0->offering = false;
                break;
                }

            // Check to see if this is a virtual packet.
            virtual_pkt* vp1 = p_best->get_virtual_pkt();
            if (!vp1) {
                // It's a non-virtual packet. So pop it and break to send it.
                p_best = p0->pkts.popfirst();
                break;
                }

            // It's a virtual packet. So get it to create a real packet.
            cp_pkt* pkt1 = vp1->packet_create(t0);

            // If the virtual packet is not finished, break to send pkt1.
            if (!vp1->finished) {
                // If pkt1 is non-null, it will be sent.
                // If pkt1 is null, it will make the main loop continue.
                p_best = pkt1;
                if (!p_best)
                    p0->offering = false;
                break;
                }

            // The virtual packet _has_ finished. So delete any packet
            // that was returned by packet_create(). (A formality.)
            delete pkt1;

            // Dequeue and delete the underlying packet in the FIFO.
            p0->pkts.delfirst();

            // Fetch the next packet, if there is one. Then loop.
            p_best = p0->pkts.first();
            }

        // Couldn't get the promised packet. So try another FIFO.
        if (!p_best)
            continue;

        // Take all remaining LB credit from the FIFO:
        p0->lb_cred = 0;

        // Take the required RR credit from RR accounts of other FIFOs.
        int i_best = p0->index;
        if ((n_rr_fifos -= 1) > 0) {
            // (credit_taken should be positive.)
            double credit_taken = p0->rr_cred - p0->vcred;
            p0->rr_cred = p0->vcred;

            // Distribute the RR credit to other active FIFOs as compensation
            // for waiting so patiently.
            // Empty and RR-inactive FIFOs get nothing!
            // (Note that the total RR credit of all FIFOs should always be 0.)
            for (int j = 0; j < n_fifos; ++j)
                if (fifos[j]->offering && j != i_best)
                    fifos[j]->rr_cred += credit_taken/n_rr_fifos;
            }

        // Return the packet:
        pkts.append(p_best);
        chan = i_best;
        return p_best->n_bytes();
        }

    // If the program gets to here, then all FIFOs were empty or RR-inactive.
    chan = -1;
    return 0;
    } // End of function cp_buffer::fetch.

//----------------------//
//   cp_buffer::print   //
//----------------------//
void cp_buffer::print(ostream& os) {
    os << "output_bitrate = " << output_bitrate << NL;
    os << "last_credit_time = " << last_credit_time << NL;
    os << "last_delay_min = " << last_delay_min << NL;
    os << "tx_debt = " << tx_debt << NL;
    os << "trace = " << trace << NL;
    os << NL;

    os << "n_fifos = " << n_fifos << NL;
    os << "last_fifo_alloc = " << last_fifo_alloc << NL;
    os << "resize_quantum = " << resize_quantum << NL;
    os << endl;

    os << "FIFO states....\n";
    for (int i = 0; i < n_fifos; ++i) {
        os << "FIFO[" << i << "]:\n";
        cp_fifo* p = fifos[i];
        if (!p) {
            os << "fifos[" << i << "] = 0\n";
            continue;
            }
        os << "    pkts.length() = " << p->pkts.length() << NL;
        os << "    lb_cred = " << p->lb_cred << NL;
        os << "    lb_cap = " << p->lb_cap << NL;
        os << "    lb_rate = " << p->lb_rate << NL;
        os << "    lb_cap_fixed = " << bool_string(p->lb_cap_fixed) << NL;
        os << "    lb_rate_fixed = " << bool_string(p->lb_rate_fixed) << NL;
        os << "    rr_cred = " << p->rr_cred << NL;
        os << "    rr_rate = " << p->rr_rate << NL;
        os << "    rr_rate_fixed = " << bool_string(p->rr_rate_fixed) << NL;
        os << "    rr_active = " << bool_string(p->rr_active) << NL;
        os << "    state = " << p->state << NL;
        os << "    vcred = " << p->vcred << NL;
        os << "    offering = " << bool_string(p->offering) << NL;
        os << "    index = " << p->index << NL;

        // ....

        }

    // ....


    } // End of function cp_buffer::print.
