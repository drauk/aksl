// src/aksl/aksltime.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/aksltime.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

gettime
ftime_diff
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/aksltime.h"
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
#ifndef AKSL_X_ERRNO_H
#define AKSL_X_ERRNO_H
#include <errno.h>
#endif

// Linux needs this for definition of perror().
#if defined(WIN32) || defined(linux)
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif
#endif

#ifdef WIN32
#ifndef AKSL_X_SYS_TIMEB_H
#define AKSL_X_SYS_TIMEB_H
#include <sys/timeb.h>
#endif
#endif

//----------------------//
//        gettime       //
//----------------------//
int gettime(timeval& tv) {
// #if !defined(WIN32)
#ifdef HAVE_GETTIMEOFDAY
#ifdef SOLARIS
    if (gettimeofday(&tv, (void*)0) < 0) {
#else  /* SOLARIS */
    if (gettimeofday(&tv, (struct timezone*)0) < 0) {
#endif /* SOLARIS */
//        cout << "Error returned by gettimeofday()." << endl;
        cout << flush;
        perror("gettimeofday");
        return -1;
        }
#else /* HAVE_GETTIMEOFDAY */
    // The win32 case.
    // If anyone knows how to get usec out of win32, please let me know!
    struct _timeb tb;
    tb.time = 0;
    _ftime(&tb);
    if (tb.time == 0)
        return -1;
    tv.tv_sec = tb.time;
    tv.tv_usec = long(tb.millitm) * 1000;
#endif /* HAVE_GETTIMEOFDAY */
    return 0;
    } // End of function gettime.

#if defined(sun) || defined(WIN32)
/*------------------------------------------------------------------------------
Returns the time t1 - t0, in milliseconds.
------------------------------------------------------------------------------*/
//----------------------//
//     ftime_diff       //
//----------------------//
int ftime_diff(const timeb& t0, const timeb& t1) {
    time_t ds = t1.time - t0.time;
    return ds * 1000 + t1.millitm - t0.millitm;
    } // End of function ftime_diff.
#endif /* sun */
