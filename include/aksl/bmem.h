// src/aksl/bmem.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_BMEM_H
#define AKSL_BMEM_H
/*------------------------------------------------------------------------------
Classes defined in this file:

bmem::
bmem_ptr::
bmem_safe::
------------------------------------------------------------------------------*/

using namespace std;

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif

// BMEM_TRACE should normally be set to 0.
// Set this to 1 when compiling AKSL to activate bmem allocation tracing.
// This is intended for diagnosis of unusual memory allocation errors.
#define BMEM_TRACE 0

// GNU g++/solaris/pc seems particularly prone to the alignment problem.
// But should check on which environments have the problem exactly.
// Warning: It is assumed here that sizeof(char*) == 4.
#ifdef __GNUC__
// Temporarily use the old value for alignment.
// Later use the value of 8 to fix up double-float alignments.
// #define BMEM_ALIGN  8
#define BMEM_ALIGN  8
#else
#define BMEM_ALIGN  4
#endif

// The default number of chunks in a block:
const long deftnchunks = 64;

enum memsrc_t {
    msNEW,
    msMALLOC
    };

/*------------------------------------------------------------------------------
This class manages a very simple singly-linked memory allocation list.
The class never gives memory back to "new" (or ultimately "malloc"),
and all allocation is of fixed length blocks.
The advantages of this way of doing things are:
1.  Less bugs.  [Surprisingly many malloc libraries have bugs.]
2.  More speed. [This is because fixed-length allocation is very fast.]
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The pointer "free" points to the first free memory chunk at the head
of the list of free chunks. No record is kept of allocated chunks as such.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The memsrc member determines whether malloc or operator new should be used for
allocating memory.
------------------------------------------------------------------------------*/
//----------------------//
//        bmem::        //
//----------------------//
struct bmem {
friend struct bmem_safe;
private:
    char* free;         // The start of the free list.
    long usersize;      // Size of the user part of each chunk (in bytes).
    long chunksize;     // Size of the whole chunk, including "next" pointer.
    long nchunks;       // Number of chunks.
    long blocksize;     // Hope that "long" has sufficient bits!
    long nblocks;       // The number of blocks.

    memsrc_t memsrc;    // The memory source: "operator new" or malloc().
    void getnewblock(); // Link in a new block.
public:
#if BMEM_TRACE
    char* owner;        // Name of class which owns this bmem object.
#endif
    void* newchunk() {
        if (!free)
            getnewblock();
        register char* p = free;
        free = *(char**)p;
#if BMEM_TRACE
        if (owner) {
            cerr << "\n[bmem::newchunk(" << owner
                 << "): " << (void*)p << "]\n";
            }
#endif
        return p + BMEM_ALIGN;
        }
    void freechunk(void* p) {
        p = (char*)p - BMEM_ALIGN;
#if BMEM_TRACE
        if (owner) {
            cerr << "\n[bmem::freechunk(" << owner
                 << "): " << (void*)p << "]\n";
            }
#endif
        *(char**)p = free;
        free = (char*)p;
        }
    void print();               // Print number and list of free chunks.
    unsigned long size() { return nblocks * blocksize; }
    unsigned long length();     // Number of chunks in use.

    // Constructor called with size of memory lump required, and nchunks.
    // If you want the malloc() version, just tack msMALLOC on the end.
    // The four options for the constructor are as follows.
    // bmem(s);
    // bmem(s, n);
    // bmem(s, ms);
    // bmem(s, n, ms);
private:
    void ctor(long s, long n, memsrc_t ms = msNEW);
public:
    bmem(long s, long n, memsrc_t ms = msNEW) { ctor(s, n, ms); }
    bmem(long s, memsrc_t ms) { ctor(s, deftnchunks, ms); }
    bmem(long s) { ctor(s, deftnchunks); }
    ~bmem() {}  // Assume that the user may try to clean up AFTER bmem.
    }; // End of struct bmem.

/*------------------------------------------------------------------------------
A linked pointer to a bmem block.
------------------------------------------------------------------------------*/
//----------------------//
//       bmem_ptr::     //
//----------------------//
struct bmem_ptr {
    bmem_ptr*   next;           // Pointer to next bmem_ptr structure.
    char*       block;          // Pointer to bmem block of chunks.

//    bmem_ptr& operator=(const bmem_ptr& x) {}
//    bmem_ptr(const bmem_ptr& x) {}
    bmem_ptr(char* pc = 0) { next = 0; block = pc; }
    ~bmem_ptr() {}
    }; // End of struct bmem_ptr.

/*------------------------------------------------------------------------------
This is supposed to be a safe form of the "bmem" class.
I don't think this class is ready to use yet, and I don't know what it's
supposed to do either. So probably you should not use it.
------------------------------------------------------------------------------*/
//----------------------//
//      bmem_safe::     //
//----------------------//
struct bmem_safe : public bmem {
private:
    bmem_ptr*   blocks;

    void getnewblock(); // Link in a new block.
public:
    void* newchunk() {
        if (!free)
            getnewblock();  // This is the bmem_safe version of getnewblock.
        register char* p = free;
        free = *(char**)p;

        // Set the chunk's link to a strange value to indicate allocation.
        *(char**)p = (char*)1;
        return p + BMEM_ALIGN;
        }
    void freechunk(void* p);

//    bmem_safe& operator=(const bmem_safe& x) {}
//    bmem_safe(const bmem_safe& x) {}
    bmem_safe(long s, long n = deftnchunks) : bmem(s, n)
        { blocks = new bmem_ptr(free); }
    ~bmem_safe();
    }; // End of struct bmem_safe.

// Trivial macro to help safely define bmem member B of class X.
#define bmem_define(X, B)           bmem        X::B(sizeof(X))
#define bmem_define_malloc(X, B)    bmem        X::B(sizeof(X), msMALLOC)

// Trivial macro to help safely define bmem_safe member B of class X.
#define bmem_safe_define(X, B)      bmem_safe   X::B(sizeof(X))

#endif /* AKSL_BMEM_H */
