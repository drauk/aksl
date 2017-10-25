// src/aksl/bbcod.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/bbcod.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
// Byte-wise coding things, like a spurious 4B-8B code.
/*------------------------------------------------------------------------------
Functions in this file:

symcode_init
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/bbcod.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files:
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
4B-8B look-up table:
    symcode[i][0] = symbol for upper nibble i. Value = i | ((i ^ 0x0f) << 4).
    symcode[i][1] = symbol for lower nibble i. Value = i | ((i ^ 0x0a) << 4).
Transmit the upper nibble symbol first.
------------------------------------------------------------------------------*/
unsigned char symcode[16][2] = {
    { 0xf0, 0xa0 }, { 0xe1, 0xb1 }, { 0xd2, 0x82 }, { 0xc3, 0x93 },
    { 0xb4, 0xe4 }, { 0xa5, 0xf5 }, { 0x96, 0xc6 }, { 0x87, 0xd7 },
    { 0x78, 0x28 }, { 0x69, 0x39 }, { 0x5a, 0x0a }, { 0x4b, 0x1b },
    { 0x3c, 0x6c }, { 0x2d, 0x7d }, { 0x1e, 0x4e }, { 0x0f, 0x5f }
    }; // End of array symcode.

/*------------------------------------------------------------------------------
symcode_init() (re-)initialises the symcode table.
------------------------------------------------------------------------------*/
//----------------------//
//     symcode_init     //
//----------------------//
int symcode_init(void) {
    int i, i_error_first = -1;

    for (i = 0; i < 16; ++i) {
        if (symcode[i][0] != (i | ((i ^ 0x0f) << 4)) && i_error_first < 0)
            i_error_first = i;
        symcode[i][0] = i | ((i ^ 0x0f) << 4);
        if (symcode[i][1] != (i | ((i ^ 0x0a) << 4)) && i_error_first < 0)
            i_error_first = i;
        symcode[i][1] = i | ((i ^ 0x0a) << 4);
        }
    if (i_error_first >= 0) {
        fprintf(stderr, "Found error in symcode[%d].\n", i_error_first);
        return -1;
        }

    return 0;
    } // End of function symcode_init.
