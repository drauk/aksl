// src/aksl/aksldefs.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/aksldefs.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_AKSLDEFS_H
#define AKSL_AKSLDEFS_H
/*------------------------------------------------------------------------------
Inline functions in this file:

copy_bytes
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
In principle, this file should be included (effectively) in all or most other
AKSL source files. Therefore it should not be overloaded with things which do
not have any global significance.
------------------------------------------------------------------------------*/

using namespace std;

// System header files:
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

/*------------------------------------------------------------------------------
Environments:
1a      Sun C++ 2.1/sunos4/sparc
1b      Sun C++ 3.0.1/sunos4/sparc
2       Sun C++/solaris/sparc
3       GNU g++/solaris/PC
4       Sun C++/solaris/PC
5       GNU g++/win95/PC
6       GNU g++/solaris/sparc
9       GNU g++/linux/PC

Values of pre-defined macros:
            1a  1b  2   3   4   5   6   9
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
unix        1   1   1   1   1       1   1
sun         1   1   1   1   1       1
sparc       1   1   1               1
__GNUC__                2       2   2   2
__svr4__                1           1
__SVR4              1   1   1       1
__WIN32__                       1
__STDC__        0   0           1   1   1
linux                                   1
------------------------------------------------------------------------------*/

// Tests for OS and/or compiler and/or hardware.
#if defined(__SVR4) || defined(__svr4__)
#define SOLARIS
#endif

// Test for Windows 95 sort of environment.
#if defined(__WIN32__) && !defined(WIN32)
#define WIN32
#endif

// Test for version 3.0.1 Sun C++ compiler on sparc station with SunOS 4.x.y.
#if defined(__STDC__) && (__STDC__ == 0) && !defined(__SVR4) && defined(sparc)
#define SUNOS4_COMP_VERSION3_0_1
#endif

#if defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x400)
#define SUNPRO400
#endif
#if defined(__SVR4) && defined(sparc) && defined(SUNPRO400)
#define SOLARIS_COMP_VERSION4
#endif

//==============================================================================
// Determine compiler/OS/architecture integer (using very ugly macros!):
#ifdef WIN32
#ifdef __GNUC__
// Environment 5:
#define COMP_OS_ARCH_ID     5
#else /* __GNUC__ */
// Environment ?:
#define COMP_OS_ARCH_ID     0
#endif /* __GNUC__ */
#endif /* WIN32 */

#if !defined(COMP_OS_ARCH_ID) && defined(sparc)
#ifdef SOLARIS
#ifdef __GNUC__
// Environment 6:
#define COMP_OS_ARCH_ID     6
#else /* __GNUC__ */
// Environment 2:
#define COMP_OS_ARCH_ID     2
#endif /* __GNUC__ */

#else /* SOLARIS */

#ifdef SUNOS4_COMP_VERSION3_0_1
// Environment 1b:
#define COMP_OS_ARCH_ID     1
#else /* SUNOS4_COMP_VERSION3_0_1 */
// Environment 1a:
#define COMP_OS_ARCH_ID     1
#endif /* SUNOS4_COMP_VERSION3_0_1 */

#endif /* SOLARIS */
#endif /* ! COMP_OS_ARCH_ID && sparc */

#if !defined(COMP_OS_ARCH_ID) && !defined(sparc)
#ifdef __GNUC__

#ifdef linux
// Environment 9:
#define COMP_OS_ARCH_ID     9
#else
// Environment 3:
#define COMP_OS_ARCH_ID     3
#endif /* linux */

#else /* __GNUC__ */
// Environment 4:
#define COMP_OS_ARCH_ID     4
#endif /* __GNUC__ */

#endif /* ! COMP_OS_ARCH_ID && ! sparc */

//==============================================================================
// Determine if templates are supported.
#if !defined(sparc) || defined(SUNOS4_COMP_VERSION3_0_1)
#define TEMPLATES_OK
#endif

// Determine if for-loop declaration scope is restricted to the for-loop block:
// #if !defined(__GNUC__) && !defined(SOLARIS_COMP_VERSION4)
#if !defined(__GNUC__)
#define OLD_FOR_LOOP_SCOPE
#endif

#ifdef OLD_FOR_LOOP_SCOPE
#define FOR_DECL(x)
#else
#define FOR_DECL(x) x
#endif

// C++ type for AKSL messages and attributes.
typedef short mtype;

//==============================================================================
//  Strings
//==============================================================================

// String describing this compiler/OS/architecture (always non-null):
extern const char *const COMP_OS_ARCH;

// Non-flushing versions of "endl":
#ifdef __GNUC__
extern const char NL[2];
extern const char DOTNL[3];
#else
extern const char NL[];
extern const char DOTNL[];
#endif

// Quick tests for null strings and non-null strings: (for char* s)
inline int nullstr(const char* s) { return !s || !*s; }
inline int notnullstr(const char* s) { return s && *s; }

// Strings for plurals etc.:
extern const char* plural_s(int i);
extern const char* plural_es(int i);
extern const char* plural_y_ies(int i);
extern const char* spaces(int n);

/*------------------------------------------------------------------------------
This is a naive form of memcpy, which does not work correctly if the
two strings overlap and the origin array (s2) has a lower address than the
destination array (s1).
It is not clear what memcpy does for ovelaps. So if there is no overlap, or
if s1 < s2, then everything should be fine.
The function "memmove" for solaris does not seem to be available for Sun OS 4.
------------------------------------------------------------------------------*/
//----------------------//
//      copy_bytes      //
//----------------------//
inline void copy_bytes(void* s1, const void* s2, int n) {
    if (s1 && s2 && n > 0) {
        char* pc1 = (char*)s1;
        const char* pc2 = (char*)s2;
        for (int i = 0; i < n; ++i)
            *pc1++ = *pc2++;
        }
    } // End of function copy_bytes.

#endif /* AKSL_AKSLDEFS_H */
