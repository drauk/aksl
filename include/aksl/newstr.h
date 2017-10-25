// src/aksl/newstr.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_NEWSTR_H
#define AKSL_NEWSTR_H
/*------------------------------------------------------------------------------
Inline functions in this file:

new_strcpy(const char*)
new_strcpy(const char*, const char*)
new_strcpy_nz
strcmpz
------------------------------------------------------------------------------*/

// AKSL header files.
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files.
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

/*------------------------------------------------------------------------------
new_strcpy(): If the argument is the null pointer, then the null pointer is
returned. Otherwise, a brand new copy of the string is returned.
------------------------------------------------------------------------------*/
//----------------------//
//      new_strcpy      //
//----------------------//
inline char* new_strcpy(const char* s2) {
    if (s2) {
        char* s1 = new char[strlen(s2) + 1];
        strcpy(s1, s2);
        return s1;
        }
    else
        return 0;
    } // End of function new_strcpy.

/*------------------------------------------------------------------------------
new_strcpy(): If the argument is the null pointer, then a copy of the second
argument is returned. Otherwise, a copy of the first string is returned.
If the second string is the zero pointer, then the zero pointer is returned.
------------------------------------------------------------------------------*/
//----------------------//
//      new_strcpy      //
//----------------------//
inline char* new_strcpy(const char* s2, const char* s3) {
    char* s1 = 0;
    if (s2) {
        s1 = new char[strlen(s2) + 1];
        strcpy(s1, s2);
        }
    else if (s3) {
        s1 = new char[strlen(s3) + 1];
        strcpy(s1, s3);
        }
    return s1;
    } // End of function new_strcpy.

/*------------------------------------------------------------------------------
new_strcpy_nz(): If the argument is the null pointer, then a copy of the string
"" is returned. Otherwise, a copy of the first string is returned.
This has the same effect as new_strcpy(s2, "").
------------------------------------------------------------------------------*/
//----------------------//
//    new_strcpy_nz     //
//----------------------//
inline char* new_strcpy_nz(const char* s2) {
    char* s1 = 0;
    if (s2) {
        s1 = new char[strlen(s2) + 1];
        strcpy(s1, s2);
        }
    else {
        s1 = new char[1];
        *s1 = 0;
        }
    return s1;
    } // End of function new_strcpy_nz.

/*------------------------------------------------------------------------------
Compare two strings with strcmp(), but allow null pointers.
A null-pointer is considered to be lower than any other string.
------------------------------------------------------------------------------*/
//----------------------//
//        strcmpz       //
//----------------------//
inline int strcmpz(const char* s1, const char* s2) {
    return s1
        ? (s2 ? strcmp(s1, s2) : 1)
        : (s2 ? -1 : 0);
    } // End of function strcmpz.

extern char* new_str_subst(const char* pc0, const char* pc1, const char* pc2);
#if HAVE_SNPRINTF
extern char* new_str(long x);
extern char* new_str(unsigned long x);
#endif

#endif /* AKSL_NEWSTR_H */
