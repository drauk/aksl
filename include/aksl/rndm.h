// src/aksl/rndm.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_RNDM_H
#define AKSL_RNDM_H

// AKSL header files:
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif
#ifndef AKSL_X_FLOAT_H
#define AKSL_X_FLOAT_H
#include <float.h>
#endif

#if defined(SOLARIS)
#define srandom srand48
#define random lrand48

#elif defined(WIN32)
#define srandom srand
#define random rand
#endif

const double randivisorR = 2147483648.0;    // 2**31.
const long randivisorN = 2147483647;        // 2**31 - 1.

/*------------------------------------------------------------------------------
random0n(n) returns a random integer in the range 0 <= x < n.
------------------------------------------------------------------------------*/
inline long random0n(long n) { return random()/(1 + randivisorN/n); }

/*------------------------------------------------------------------------------
randomnn(n) returns a random integer in the range -n <= x < n.
------------------------------------------------------------------------------*/
#ifdef SOLARIS
inline long randomnn(long n) { return mrand48()/(1 + randivisorN/n); }
#else
inline long randomnn(long n) { return random0n(2*n) - n; }
#endif

/*------------------------------------------------------------------------------
random01() returns a random real number in the range 0 < x <= 1.
------------------------------------------------------------------------------*/
inline double random01() { return (random() + 1)/randivisorR; }

// Exports:
extern void srandom01();

#endif /* AKSL_RNDM_H */
