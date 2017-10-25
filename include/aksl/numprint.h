// src/aksl/numprint.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_NUMPRINT_H
#define AKSL_NUMPRINT_H
/*------------------------------------------------------------------------------
This module contains numerical output formatting things.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes in this file:

------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_NBYTES_H
#include "aksl/nbytes.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
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

// Look-up table for hex strings:
extern char byte2hex[256][3];

// Return an int/hex/real string for a long, with a given number of digits.
// Probably all of this stuff should go into form.[ch].
extern const char* intstring(long, int = 0, int = 0);
extern const char* uintstring(long, int = 0, int = 0);

extern const char* hexstring(unsigned long, int = 0, int = 0);
extern const char* hex2u8(uint8 x);
inline const char* hex2(unsigned long l) { return hexstring(l, 2, 2); }
inline const char* hex4(unsigned long l) { return hexstring(l, 4, 4); }
inline const char* hex8(unsigned long l) { return hexstring(l, 8, 8); }

extern const char* octstring(unsigned long, int = 0, int = 0);

extern const char* f_string(double, int = 0, int = 0);
extern const char* e_string(double, int = 0, int = 0);
extern const char* g_string(double, int = 0, int = 0);
inline const char* realstring(double r, int fw = 0, int prec = 0)
    { return f_string(r, fw, prec); }

// Return string "false" or "true":
extern const char* bool_string(long);

// Test the byte2hex array:
extern void byte2hex_test();

//==============================================================================
//  IP things:
//==============================================================================

// Print a given number of bytes in hexadecimal:
extern void hex_print(const char*, int, ostream& = cout);
inline void hex_print(const nbytes& buf, ostream& os = cout)
    { hex_print(buf.bytes(), buf.n_bytes(), os); }

// Print a given number of bytes in binary:
extern void bin_print(const char*, int, ostream& = cout);
inline void bin_print(const nbytes& buf, ostream& os = cout)
    { bin_print(buf.bytes(), buf.n_bytes(), os); }

// Return ip address (host byte order) as "dot" string.
extern const char* ipstring(unsigned long);

// Print a given number of bytes as an IP packet:
// extern void IP_print(const char*, int, ostream& = cout);

// Dump an IP packet to a file in "snoop" format:
extern int bin_dump_header(int fd);
extern int bin_dump(int fd, const char* buf, int n_bytes,
                    const timeval& ret_time);
inline int bin_dump(int fd, const nbytes& buf, const timeval& ret_time)
    { return bin_dump(fd, buf.bytes(), buf.n_bytes(), ret_time); }

#endif /* AKSL_NUMPRINT_H */
