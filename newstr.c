// src/aksl/newstr.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

new_str_subst
new_str
new_str
------------------------------------------------------------------------------*/

// AKSL header files.
#include "aksl/newstr.h"
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files.
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
// #include <ctype.h>
#endif
// For snprintf().
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
Returns a new heap-allocated string which has all instances of the string
pc1 in pc0 replaced by pc2.
If the string pc0 does not need to be changed, or if any error occurs, a
null pointer is returned.
If pc2 is the null pointer or *pc2 is the null string, the null string is
substituted for *pc1.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This routine depends heavily on the absence of errors in the string.h library.
If any errors are found, then the whole function should be written without
strstr() in particular, and probably without strncpy() also.
------------------------------------------------------------------------------*/
//----------------------//
//     new_str_subst    //
//----------------------//
char* new_str_subst(const char* pc0, const char* pc1, const char* pc2) {
    if (nullstr(pc0) || nullstr(pc1))
        return 0;
    static const char n_string[] = "";
    if (!pc2)
        pc2 = n_string;
    int i0 = strlen(pc0);
    int i1 = strlen(pc1);
    int i2 = strlen(pc2);
    int i_diff = i2 - i1;

    // First pass to find out the number of substitutions.
    int n_subst = 0;
    for (const char* pos = pc0; (pos = strstr(pos, pc1)) != 0; pos += i1)
        n_subst += 1;

    // If no substitution necessary, return null pointer.
    if (n_subst <= 0)
        return 0;

    // Create a new string.
    char* pc3 = new char[i0 + n_subst * i_diff + 1];

    // Copy the string pc0 with substitutions of pc2 for pc1.
    const char* pc0_last = pc0;
    char* pc3_last = pc3;
    for (FOR_DECL(const char*) pos = pc0; (pos = strstr(pos, pc1)) != 0;
                                          pos += i1) {
        strncpy(pc3_last, pc0_last, (pos - pc0_last));
        pc3_last += pos - pc0_last;
        strcpy(pc3_last, pc2);
        pc3_last += i2;
        pc0_last = pos + i1;
        }
    strcpy(pc3_last, pc0_last);

    return pc3;
    } // End of function new_str_subst.

/*------------------------------------------------------------------------------
Return a heap-allocated string which if the decimal representation of
the given integer.
An unsigned integer with 31 bits can be at most 2,000,000,000.
So 11 digits should be sufficient for the "long" string.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note that these new_str() functions require snprintf().
If snprintf() is not available, this function should be written differently.
That's on my todo list....
------------------------------------------------------------------------------*/
//----------------------//
//        new_str       //
//----------------------//
#if HAVE_SNPRINTF
char* new_str(long x) {
    const int n = 24;
    static char buf[n];

    buf[0] = 0;
    int nchars = snprintf(buf, n, "%ld", x);
    if (nchars < 0)
        return 0;
    return new_strcpy(buf);
    } // End of function new_str.
#endif

/*------------------------------------------------------------------------------
Return a heap-allocated string which if the decimal representation of
the given integer.
An unsigned integer with 32 bits can be at most 4,000,000,000.
So 11 digits should be sufficient for the "unsigned long" string.
------------------------------------------------------------------------------*/
//----------------------//
//        new_str       //
//----------------------//
#if HAVE_SNPRINTF
char* new_str(unsigned long x) {
    const int n = 24;
    static char buf[n];

    buf[0] = 0;
    int nchars = snprintf(buf, n, "%lu", x);
    if (nchars < 0)
        return 0;
    return new_strcpy(buf);
    } // End of function new_str.
#endif
