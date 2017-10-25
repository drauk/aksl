// src/aksl/bbcod.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/bbcod.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_BBCOD_H
#define AKSL_BBCOD_H
/*------------------------------------------------------------------------------
This module contains things to do with byte-wise coding, like 4B-8B coding.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes defined in this file:

------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif

// Out-of-band control symbols (idle and interrupt):
#define IDLE_SYMBOL     0x00
#define INTR_SYMBOL     0xff

// Tests for valid data symbols:
#define is_usym(x)          (symcode[x & 0x0f][0] == x)
#define is_lsym(x)          (symcode[x & 0x0f][1] == x)

/*  For hardware version, note that x is a data symbol if and only if
    ((x >> 4) ^ x) == 0x0f or 0x0a. */

// Conversion of symbol to nibble:
#define sym2nibble(x)       (x & 0x0f)

// Exported data:
extern unsigned char symcode[16][2];

// Exported functions:
extern int      symcode_init(void);

#endif /* AKSL_BBCOD_H */
