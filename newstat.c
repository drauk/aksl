// src/aksl/newstat.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

newstat_new
newstat_delete
newstat_activate
newstat_clear
newstat_count
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This module should _not_ use the voidptr class, because it can only store the
address of each memory allocation. It would be much better to keep a record of
(1) the memory address, (2) the memory size, (3) the time at which the
allocation occurs.
------------------------------------------------------------------------------*/

// AKSL header files.
#include "aksl/newstat.h"

#ifndef AKSL_VPLIST_H
#include "aksl/vplist.h"
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

// List of addresses of all allocated memory chunks.
// This hopefully does _not_ call the standard global new-operator
// to allocate memory. If it does, then there will be a nasty loop!!!!
static voidptr_blist addresses;

// Parameter to control whether "newstat" is active.
// This is to prevent nasty recursion, hopefully.
static bool_enum active = false;

//----------------------//
//     newstat_new      //
//----------------------//
void* newstat_new(size_t n) {
    void* x = malloc(n);

    // Make a record of the address. Causes infinite recursion!!!!
    if (active)
        addresses.prepend(x);

    return x;
    } // End of function newstat_new.

//----------------------//
//    newstat_delete    //
//----------------------//
void newstat_delete(void* x) {
    // Remove the pointer from the list.
    if (active)
        addresses.remove(x);

    free((char*)x);
    } // End of function newstat_delete.

/*------------------------------------------------------------------------------
Activate the statistics functions, and return the previous value of "active".
------------------------------------------------------------------------------*/
//----------------------//
//   newstat_activate   //
//----------------------//
bool_enum newstat_activate() {
    bool_enum x = active;
    active = true;
    return x;
    } // End of function newstat_activate.

/*------------------------------------------------------------------------------
This clears the entire record of all memory chunk addresses. The effect of this
is that whenever previously allocated memory is freed, it is simply not found in
the address list. Obviously from this point on only new allocations are recorded
in the address list.
------------------------------------------------------------------------------*/
//----------------------//
//     newstat_clear    //
//----------------------//
void newstat_clear() {
    addresses.clear();
    active = false;
    } // End of function newstat_clear.

/*------------------------------------------------------------------------------
Return the number of memory chunks currently allocated.
------------------------------------------------------------------------------*/
//----------------------//
//    newstat_count     //
//----------------------//
long newstat_count() {
    return addresses.length();
    } // End of function newstat_count.
