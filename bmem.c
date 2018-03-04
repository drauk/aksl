// src/aksl/bmem.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

bmem::
    ctor
    getnewblock
    print
    length
bmem_safe::
    ~bmem_safe
    getnewblock
    freechunk
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/bmem.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files.
#if defined(sun) || defined(WIN32)
#ifndef AKSL_X_MALLOC_H
#define AKSL_X_MALLOC_H
#include <malloc.h>
#endif
#endif
#if defined(linux) || defined(WIN32)
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif
#endif

/*------------------------------------------------------------------------------
bmem::ctor(long s, long n, memsrc_t ms)
s   = number of bytes for the user in each memory chunk.
n   = number of chunks in each block of chunks.
ms  = memory source (new or malloc).

Each bmem block contains ((BMEM_ALIGN + s) * n) bytes.
Each bmem chunk contains (BMEM_ALIGN + s) bytes.

Structure of chunk:

         nxt pad          user
        |---|---|----------------------|
        |<-------- chunksize ----------|
                |<----- usersize ------|

nxt:    char* pointer to next chunk
pad:    pad to make user-bytes start at correct alignment
user:   user space (s bytes), aligned to BMEM_ALIGN-byte boundary (hopefully)

length of "pad" = BMEM_ALIGN - sizeof(char*).
BMEM_ALIGN = 4 or 8 bytes.
So length of pad is 0 bytes in the case of 4-byte alignment, or
4 bytes in the case of 8-byte alignment.
Therefore length of "nxt" + length of "pad" = BMEM_ALIGN.

The chunks and blocks may be thought of as being defined as follows.

struct chunk {
    char*   nxt;
    char    pad[BMEM_ALIGN - sizeof(char*)];
    char    user[usersize];
    };
struct block {
    chunk   chunks[nchunks];
    };
------------------------------------------------------------------------------*/
//----------------------//
//      bmem::ctor      //
//----------------------//
void bmem::ctor(long s, long n, memsrc_t ms) {
    // Take note of the memory source.
    if (ms == msNEW)
        memsrc = msNEW;
    else if (ms == msMALLOC)
        memsrc = msMALLOC;
    else
        memsrc = msNEW;             // Default for errored arg is "new".

    // Round the value of "s" up to the nearest multiple of BMEM_ALIGN.
#if BMEM_ALIGN == 4
    usersize = (s + BMEM_ALIGN - 1) & ~0x03;
#endif
#if BMEM_ALIGN == 8
    usersize = (s + BMEM_ALIGN - 1) & ~0x07;
#endif
    chunksize = BMEM_ALIGN + usersize;
    nchunks = n;
    blocksize = nchunks * chunksize;
    if (memsrc == msNEW)
        free = new char[blocksize];
    else
        free = (char*)malloc((int)blocksize);
    nblocks = 1;

    // Initialise the first (nchunks - 1) chunks in the initial block.
    register char* p = free;            // Trailing pointer.
    register char* q = p;               // Advancing pointer.
    for (register long i = nchunks; --i > 0; ) {
        q += chunksize;
        *(char**)p = q;
        p = q;
        }
    // Make the last chunk point to nowhere.
    *(char**)p = 0;
#if BMEM_TRACE
    // The owner of this bmem object.
    owner = 0;
#endif
    } // End of function bmem::ctor.

/*------------------------------------------------------------------------------
This function is to be called by class bmem:: only when all allocated memory is
in use.
A new block of "nchunks" chunks is allocated and initialised.
All knoweldge of the previous chunk is forgotten, so that it is not possible to
ge back and free old chunks. This is intentional, so as to prevent bugs which
arise from calling the free() function, and block allocation "thrashing".
------------------------------------------------------------------------------*/
//----------------------//
//   bmem::getnewblock  //
//----------------------//
void bmem::getnewblock() {
    // Assume that the call is made only when free == 0.
    if (memsrc == msNEW)
        free = new char[blocksize];
    else
        free = (char*)malloc((int)blocksize);
    nblocks += 1;
    register char* p = free;            // Trailing pointer.
    register char* q = p;               // Advancing pointer.
    for (register long i = nchunks; --i > 0; ) {
        q += chunksize;
        *(char**)p = q;
        p = q;
        }
    *(char**)p = 0;
    } // End of function bmem::getnewblock.

//----------------------//
//      bmem::print     //
//----------------------//
void bmem::print() {
    cout << "Number of bmem blocks = " << nblocks << endl;
    for (char** p = (char**)free; p; p = (char**)*p)
        cout << "0x" << hex8((long)p) << endl;
    } // End of function bmem::print.

//----------------------//
//     bmem::length     //
//----------------------//
unsigned long bmem::length() {
    // Calculate the number of free chunks:
    unsigned long n = 0;
    for (char** p = (char**)free; p; p = (char**)*p)
        n += 1;

    // Subtract the free chunks from the total chunks:
    return nblocks * nchunks - n;
    } // End of function bmem::length.

/*------------------------------------------------------------------------------
Same algorithm as s1list::~s1list().
------------------------------------------------------------------------------*/
//----------------------//
// bmem_safe::~bmem_safe//
//----------------------//
bmem_safe::~bmem_safe() {
    // Delete the linked list of block-pointers.
    for (register bmem_ptr* q = blocks; q; ) {
        register bmem_ptr* p = q->next;
        delete q;
        q = p;
        }
    } // End of function bmem_safe::~bmem_safe.

/*------------------------------------------------------------------------------
The parent class bmem::getnewblock() function is invoked when free == 0 to
allocate and initialise a new block of free chunks, pointed to by "free".
The bmem_safe::getnewblock() just records the new free block in the "blocks"
list. The new item is prepended to the "blocks" list, not appended.
------------------------------------------------------------------------------*/
//--------------------------//
//  bmem_safe::getnewblock  //
//--------------------------//
void bmem_safe::getnewblock() {
    // First do the standard getnewblock() function.
    bmem::getnewblock();

    // Then update the list of blocks.
    bmem_ptr* p = new bmem_ptr(free);
    p->next = blocks;
    blocks = p;
    } // End of function bmem_safe::getnewblock.

//----------------------//
// bmem_safe::freechunk //
//----------------------//
void bmem_safe::freechunk(void* p) {
    p = (char*)p - BMEM_ALIGN;

    // Check that the chunk is really in the allocated memory list.
    if (*(char**)p != (char*)1) {
        cout << flush;
        cerr << "<<error: bmem_safe::freechunk non-allocated chunk>>\n";
        return;
        }

    // Here should also check that the chunk is within one of the
    // allocated blocks of chunks. Use "blocks" to do this.
    // ....

    *(char**)p = free;
    free = (char*)p;
    } // End of function bmem_safe::freechunk.
