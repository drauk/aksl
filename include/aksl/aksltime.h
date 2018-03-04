// src/aksl/aksltime.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_AKSLTIME_H
#define AKSL_AKSLTIME_H
/*------------------------------------------------------------------------------
Inline functions in this file:

print
timeval_set_zero
timeval_set
timeval_get
timeval_diff
timeval_diff_uS
timeval_sum
------------------------------------------------------------------------------*/

using namespace std;

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_MATH_H
#define AKSL_X_MATH_H
#include <math.h>
#endif
//#include <float.h>
#ifndef AKSL_X_SYS_TYPES_H
#define AKSL_X_SYS_TYPES_H
#include <sys/types.h>
#endif

#if defined(sun) || defined(WIN32)
#ifndef AKSL_X_SYS_TIMEB_H
#define AKSL_X_SYS_TIMEB_H
#include <sys/timeb.h>
#endif
#endif

#ifdef WIN32
#ifndef AKSL_X_WINSOCK_H
#define AKSL_X_WINSOCK_H
#include <winsock.h>
#endif

#else
#ifndef AKSL_X_SYS_TIME_H
#define AKSL_X_SYS_TIME_H
#include <sys/time.h>
#endif
#endif

//----------------------//
//        print         //
//----------------------//
inline void print(const timeval& t, ostream& os = cout) {
    os << "(" << t.tv_sec << ", " << t.tv_usec << ") -> "
       << (t.tv_sec + t.tv_usec / 1e6);
    } // End of function print.

//----------------------//
//   timeval_set_zero   //
//----------------------//
inline void timeval_set_zero(timeval& t) {
    t.tv_sec = 0;
    t.tv_usec = 0;
    } // End of function timeval_set_zero.

//----------------------//
//      timeval_set     //
//----------------------//
inline void timeval_set(timeval& t, double x) {
    t.tv_sec = (long)floor(x);
#if defined(WIN32)
    t.tv_usec = (long)floor((x - t.tv_sec) * 1e6);
#else
    t.tv_usec = (long)rint((x - t.tv_sec) * 1e6);
#endif
    } // End of function timeval_set.

//----------------------//
//      timeval_get     //
//----------------------//
inline double timeval_get(timeval& t) {
    return t.tv_sec + t.tv_usec * 1e-6;
    } // End of function timeval_get.

/*------------------------------------------------------------------------------
Returns t1 - t2, not the other way around!
------------------------------------------------------------------------------*/
//----------------------//
//     timeval_diff     //
//----------------------//
inline double timeval_diff(timeval& t1, timeval& t2) {
    return (t1.tv_sec - t2.tv_sec) + (t1.tv_usec - t2.tv_usec) * 1e-6;
    } // End of function timeval_diff.

/*------------------------------------------------------------------------------
Returns t1 - t2 in uS, not the other way around!
------------------------------------------------------------------------------*/
//----------------------//
//   timeval_diff_uS    //
//----------------------//
inline double timeval_diff_uS(timeval& t1, timeval& t2) {
    return (t1.tv_sec - t2.tv_sec) * 1e6 + (t1.tv_usec - t2.tv_usec);
    } // End of function timeval_diff_uS.

/*------------------------------------------------------------------------------
A not very useful function.
------------------------------------------------------------------------------*/
//----------------------//
//      timeval_sum     //
//----------------------//
inline double timeval_sum(timeval& t1, timeval& t2) {
    return (t1.tv_sec + t2.tv_sec) + (t1.tv_usec + t2.tv_usec) * 1e-6;
    } // End of function timeval_sum.

// Exported functions:
// Gettimeofday function:
extern int gettime(timeval& tv);

#if defined(sun) || defined(WIN32)
// Returns the time tb1 - tb0, in milliseconds:
extern int ftime_diff(const timeb& tb0, const timeb& tb1);
#endif /* sun */

#endif /* AKSL_AKSLTIME_H */
