// src/aksl/aksldefs.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

plural_s
plural_es
plural_y_ies
spaces
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/aksldefs.h"

const char NL[] = "\n";
const char DOTNL[] = ".\n";

// Create a compiler/OS/architecture string (using very ugly macros!):
#ifdef WIN32

#ifdef __GNUC__
// Environment 5:
#define COMP_OS_ARCH_STRING "gnu g++/win32/pc"
#else /* __GNUC__ */
// Environment ?:
#define COMP_OS_ARCH_STRING "visual c++/win32/wind95/pc"
#endif /* __GNUC__ */

#else /* WIN32 */

#ifdef sparc

#ifdef SOLARIS

#ifdef __GNUC__
// Environment 6:
#define COMP_OS_ARCH_STRING "gnu g++/solaris/sparc"
#else /* __GNUC__ */
// Environment 2:
#define COMP_OS_ARCH_STRING "sun c++/solaris/sparc"
#endif /* __GNUC__ */

#else /* SOLARIS */

#ifdef SUNOS4_COMP_VERSION3_0_1
// Environment 1b:
#define COMP_OS_ARCH_STRING "sun c++ 3.0.1/sunos/sparc"
#else /* SUNOS4_COMP_VERSION3_0_1 */
// Environment 1a:
#define COMP_OS_ARCH_STRING "sun c++ 2.x.y/sunos/sparc"
#endif /* SUNOS4_COMP_VERSION3_0_1 */

#endif /* SOLARIS */

#else /* sparc */

#ifdef __GNUC__
#ifdef linux
// Environment 9:
#define COMP_OS_ARCH_STRING "gnu g++/linux/pc"
#else
// Environment 3:
#define COMP_OS_ARCH_STRING "gnu g++/solaris/pc"
#endif

#else /* __GNUC__ */
// Environment 4:
#define COMP_OS_ARCH_STRING "sun c++/solaris/pc"
#endif /* __GNUC__ */

#endif /* sparc */
#endif /* WIN32 */

#ifndef COMP_OS_ARCH_STRING
#define COMP_OS_ARCH_STRING "?/?/?"
#endif

// Workaround because of silly gnu g++ restrictions on array declarations:
static const char s[] = COMP_OS_ARCH_STRING;
const char *const COMP_OS_ARCH = s;

// Strings for plurals:
//----------------------//
//       plural_s       //
//----------------------//
const char* plural_s(int i) {
    return (i == 1) ? "" : "s";
    } // End of function plural_s.

//----------------------//
//       plural_es      //
//----------------------//
const char* plural_es(int i) {
    return (i == 1) ? "" : "es";
    } // End of function plural_es.

//----------------------//
//     plural_y_ies     //
//----------------------//
const char* plural_y_ies(int i) {
    return (i == 1) ? "y" : "ies";
    } // End of function plural_y_ies.

/*------------------------------------------------------------------------------
Return a string of spaces.
The returned pointer is always valid, but it is truncated to within a
reasonable range.
------------------------------------------------------------------------------*/
//----------------------//
//        spaces        //
//----------------------//
const char* spaces(int n) {
    const int ns = 1000;
    static char* space_buf = 0;
    static char* space_buf_end = 0;

    // Create the space buffer if this is the first call to this function:
    if (!space_buf) {
        space_buf = new char[ns + 1];
        for (int i = 0; i < ns; ++i)
            space_buf[i] = ' ';
        space_buf[ns + 1] = 0;
        space_buf_end = space_buf + ns;
        }

    // Clip the value of n:
    if (n < 0)
        n = 0;
    else if (n > ns)
        n = ns;

    return space_buf_end - n;
    } // End of function spaces.
