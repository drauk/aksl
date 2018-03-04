// src/aksl/nbytes.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

nbytes::
    nbytes
    operator=
    copy_from
    swallow
    copy_to
    read
    set16
    set32
    swap_ip
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/nbytes.h"

// System header files:
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

// Size of static char buffer used by nbytes::read.
const int nbytes_bufsize0 = 65536;

// A generous buffer -- but only one per process. (This is a _good_ thing.)
static char nbytes_buf0[nbytes_bufsize0];

// Info about the per-process buffer exported to classes derived from nbytes:
char* nbytes::nbytes_buf    = nbytes_buf0;
int nbytes::nbytes_bufsize  = nbytes_bufsize0;

//----------------------//
//    nbytes::nbytes    //
//----------------------//
nbytes::nbytes(const nbytes& x) {
    if (x.pc) {
        n = x.n;
        pc = new char[n];
        memcpy(pc, x.pc, n);
        }
    else {
        pc = 0;
        n = 0;
        }
    } // End of function nbytes::nbytes.

//----------------------//
//   nbytes::operator=  //
//----------------------//
nbytes& nbytes::operator=(const nbytes& x) {
    delete[] pc;
    if (x.pc) {
        n = x.n;
        pc = new char[n];
        memcpy(pc, x.pc, n);
        }
    else {
        n = 0;
        pc = 0;
        }
    return *this;
    } // End of function nbytes::operator=.

//----------------------//
//   nbytes::copy_from  //
//----------------------//
void nbytes::copy_from(const char* pc1, int n1) {
    clear();
    if (!pc1 || n1 <= 0)
        return;
    n = n1;
    pc = new char[n1];
    memcpy(pc, pc1, n);
    } // End of function nbytes::copy_from.

/*------------------------------------------------------------------------------
This function assumes that "pc1" points to a heap-allocated char array of length
n1 bytes. The "pc1" pointer is deleted to try to prevent the user from deleting
the memory, because that responsibility is taken over by the "nbytes" class.
------------------------------------------------------------------------------*/
//----------------------//
//    nbytes::swallow   //
//----------------------//
void nbytes::swallow(char*& pc1, int n1) {
    clear();
    if (!pc1 || n1 <= 0)
        return;
    n = n1;
    pc = pc1;

    // Zero the char-pointer so that the user won't be able to delete it!
    pc1 = 0;
    } // End of function nbytes::swallow.

/*------------------------------------------------------------------------------
Copies a maximum of bufsize bytes.
------------------------------------------------------------------------------*/
//----------------------//
//    nbytes::copy_to   //
//----------------------//
int nbytes::copy_to(char* buf, int bufsize) {
    if (!buf || bufsize <= 0 || empty())
        return 0;
    if (bufsize > n)
        bufsize = n;
    memcpy(buf, pc, bufsize);
    return bufsize;
    } // End of function nbytes::copy_to.

/*------------------------------------------------------------------------------
This will block if there's no data to read!
If a "bufsize" is specified which is positive and not equal to nbytes_bufsize0,
then a different size buffer is used. If it is smaller, then ::read is called
with a smaller bufsize parameter. If bufsize is larger than nbytes_bufsize0,
then a temporary buffer is created of that size, and this buffer is deleted
afterwards.
In the append-mode, any new data is appended to the existing data.
The return value is the number of bytes read rather than the new value of n.
------------------------------------------------------------------------------*/
//----------------------//
//     nbytes::read     //
//----------------------//
int nbytes::read(int fd, int bufsize, bool_enum append) {
    // Ignore silly arguments:
    if (fd < 0 || bufsize < 0) {
        if (!append)
            clear();
        return -1;
        }

    // Try to read some bytes:
    int rval = 0;
    char* buf = nbytes_buf0;
    if (bufsize > 0) {
        if (bufsize <= nbytes_bufsize0)
            rval = ::read(fd, nbytes_buf0, bufsize);
        else {
            // Create a temporary larger buffer:
            buf = new char[bufsize];
            rval = ::read(fd, buf, bufsize);
            }
        }
    else
        rval = ::read(fd, nbytes_buf0, nbytes_bufsize0);

    if (!append) {
        // Copy the new bytes to the (pc, n) pair:
        if (rval > 0) {
            delete[] pc;
            pc = new char[rval];
            memcpy(pc, buf, rval);
            n = rval;
            }
        else
            clear();
        }
    else {
        // Append the new bytes to the (pc, n) pair:
        if (rval > 0) {
            char* pc2 = new char[n + rval];
            memcpy(pc2, pc, n);
            memcpy(pc2 + n, buf, rval);
            delete[] pc;
            pc = pc2;
            n += rval;
            }
        }

    // Delete the temporary buffer, if non-standard size was used:
    if (buf != nbytes_buf0)
        delete[] buf;

    return rval;
    } // End of function nbytes::read.

//----------------------//
//     nbytes::set16    //
//----------------------//
void nbytes::set16(int offset, uint16 value) {
    if (!pc || offset >= n)
        return;

    // MSByte:
    pc[offset++] = char(value >> 8);

    // LSByte:
    if (offset < n)
        pc[offset] = char(value);
    } // End of function nbytes::set16.

//----------------------//
//     nbytes::set32    //
//----------------------//
void nbytes::set32(int offset, uint32 value) {
    if (!pc || offset >= n)
        return;

    // MSByte:
    pc[offset] = char(value >> 24);
    if (++offset >= n)
        return;
    pc[offset] = char(value >> 16);
    if (++offset >= n)
        return;
    pc[offset] = char(value >> 8);
    if (++offset >= n)
        return;

    // LSByte:
    pc[offset] = char(value);
    } // End of function nbytes::set32.

/*------------------------------------------------------------------------------
This dodgy function attempts to swap the bytes in pc[12] to pc[15] with the
corresponding bytes in pc[16] to pc[19]. If the bytes were the payload of an IP
packet, then these positions would hold the source and destination IP addresses
respectively.
------------------------------------------------------------------------------*/
//----------------------//
//   nbytes::swap_ip    //
//----------------------//
void nbytes::swap_ip() {
    // No swap if not sufficient bytes.
    if (!pc || n < 20)
        return;

    // Swap pc[12] to pc[15] with pc[16] to pc[19]:
    // Note: this would be faster with swapping them as 32-bit integers.
    for (register int ii = 12; ii < 16; ++ii) {
        register char t = pc[ii + 4];
        pc[ii + 4] = pc[ii];
        pc[ii] = t;
        }
    } // End of function nbytes::swap_ip.
