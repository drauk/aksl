// src/aksl/cod.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/cod.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_COD_H
#define AKSL_COD_H
/*------------------------------------------------------------------------------
This module contains things to do with coding, like CRCs and FEC coding.
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

// Miscellaneous constants:
const int    crc8poly   = 0x107;        // As in ATM HCS field.
const uint16 crc16poly  = 0x1021;       // Reverse to PPP? Actually 0x11021.
const uint16 crc16begin = 0xffff;       // Initial value of register.
const uint32 crc32poly  = 0x04c11db7;   // As in AAL5. Actually 0x104c11db7.
const uint32 crc32begin = 0xffffffff;   // Initial value of register.

// General exported functions:
extern int          crc8init();
extern char         crc8calc(const char* buf, int len);
extern char         crc8calc(const char* buf);  // Null-terminated string.
extern uint8        crc8calc_rl(uint32 data);

extern int          crc16init(uint16 poly = crc16poly, int trace = 0);
extern uint16       crc16calc(const char* buf, int len);
extern uint16       crc16calc(const char* buf); // Null-terminated string.
extern uint16       crc16calc_rl(uint32 data);

extern int          crc32init(uint32 poly = crc32poly, int trace = 0);
extern uint32       crc32calc(const char* buf, int len);
extern uint32       crc32calc(const char* buf); // Null-terminated string.

#endif /* AKSL_COD_H */
