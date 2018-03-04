// src/aksl/rndm.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

srandom01
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/rndm.h"

// System header files:
#ifndef AKSL_X_TIME_H
#define AKSL_X_TIME_H
#include <time.h>
#endif

static int seeded = 0;

/*------------------------------------------------------------------------------
srandom01() seeds the random number function random01(). If the random number
has already been seeded, then it is not seeded again.
------------------------------------------------------------------------------*/
//----------------------//
//      srandom01       //
//----------------------//
void srandom01() {
    if (!seeded) {
        srandom((int)time(0));
        seeded = 1;
        }
    } // End of function srandom01.
