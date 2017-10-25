// src/aksl/newstat.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/newstat.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_NEWSTAT_H
#define AKSL_NEWSTAT_H
/*------------------------------------------------------------------------------
Classes in this file:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
To invoke this module, write the two lines
    `#include "newstat.h"'
and `NEWSTAT_NEW_AND_DELETE'.
This module is experimental. Be cautious.
------------------------------------------------------------------------------*/

// AKSL header files.
#ifndef AKSL_BOOLE_H
#include "boole.h"
#endif

// System header files.
#ifndef AKSL_X_SYS_TYPES_H
#define AKSL_X_SYS_TYPES_H
#include <sys/types.h>
#endif

// This macro should be invoked only once.
// This will redefine the "new" and "delete" operators.
//--------------------------//
//  NEWSTAT_NEW_AND_DELETE  //
//--------------------------//
#define NEWSTAT_NEW_AND_DELETE \
void* operator new(size_t n) { return newstat_new(n); } \
void operator delete(void* x) { newstat_delete(x); }

extern void* newstat_new(size_t n);
extern void newstat_delete(void* x);
extern bool_enum newstat_activate();    // Start up the statistics functions.
extern void newstat_clear();            // Clear the address list.
extern long newstat_count();            // Return number of allocated chunks.

#endif /* AKSL_NEWSTAT_H */
