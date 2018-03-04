// src/aksl/numb.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

lfloor
u16encode
u32encode
i16encode
i32encode
dbl_encode
u16decode
u32decode
i16decode
i32decode
dbl_decode
u8checksum_xor
u8checksum_add
u16checksum_xor
u16checksum_add
u32checksum_xor
u32checksum_add
parse_lll
parse_i16i16i16
parse_u16u16u16
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/numb.h"

// System header files:
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif

/*------------------------------------------------------------------------------
Return floor of n/d.
This is a very cautious version which assumes only that n/d gives the correct
answer for n >= 0 and d > 0.
------------------------------------------------------------------------------*/
//----------------------//
//        lfloor        //
//----------------------//
long lfloor(long n, long d) {
    return (d > 0)
        ? ((n >= 0)
            ? (n/d)                 // n >= 0, d > 0.
            : (-1-((-1-n)/d))       // n < 0, d > 0.
            )
        : ((d < 0)
            ? ((n > 0)
                ? (-1-((n-1)/(-d))) // n > 0, d < 0.
                : ((-n)/(-d))       // n <= 0, d < 0.
                )
            : 0                     // Error case that d == 0.
            );
    } // End of function lfloor.

//----------------------//
//       u16encode      //
//----------------------//
void u16encode(char buf[2], uint16 x) {
    buf[1] = x & 0xff;
    buf[0] = (x >> 8) & 0xff;
    } // End of function u16encode.

//----------------------//
//       u32encode      //
//----------------------//
void u32encode(char buf[4], uint32 x) {
    buf[3] = char(x & 0xff);
    buf[2] = char((x >>= 8) & 0xff);
    buf[1] = char((x >>= 8) & 0xff);
    buf[0] = char((x >> 8) & 0xff);
    } // End of function u32encode.

//----------------------//
//       i16encode      //
//----------------------//
void i16encode(char buf[2], int16 x) {
    buf[1] = x & 0xff;
    buf[0] = (x >> 8) & 0xff;
    } // End of function i16encode.

//----------------------//
//       i32encode      //
//----------------------//
void i32encode(char buf[4], int32 x) {
    buf[3] = char(x & 0xff);
    buf[2] = char((x >>= 8) & 0xff);
    buf[1] = char((x >>= 8) & 0xff);
    buf[0] = char((x >> 8) & 0xff);
    } // End of function i32encode.

/*------------------------------------------------------------------------------
Warning: This is almost certainly not network-portable.
Fix this up for each kind of compiler/machine used!!!!
------------------------------------------------------------------------------*/
//----------------------//
//      dbl_encode      //
//----------------------//
void dbl_encode(char buf[sizeof(double)], double x) {
    // Union of double with properly aligned array (hopefully!).
    union {
        double d;
        char buf[sizeof(double)];
        } xx;

    xx.d = x;

    // Copy the bytes from the given array to a properly aligned array.
    for (int i = 0; i < sizeof(double);  ++i)
        buf[i] = xx.buf[i];
    } // End of function dbl_encode.

//----------------------//
//       u16decode      //
//----------------------//
uint16 u16decode(const char buf[2]) {
    return (((uint16)buf[0] << 8) & 0xff00) | (buf[1] & 0xff);
    } // End of function u16decode.

//----------------------//
//       u32decode      //
//----------------------//
uint32 u32decode(const char buf[4]) {
    register uint32 x = (uint32)buf[0] & 0xff;
    x <<= 8;
    x |= (uint32)buf[1] & 0xff;
    x <<= 8;
    x |= (uint32)buf[2] & 0xff;
    x <<= 8;
    x |= (uint32)buf[3] & 0xff;
    return x;
    } // End of function u32decode.

//----------------------//
//       i16decode      //
//----------------------//
int16 i16decode(const char buf[2]) {
    return (((int16)buf[0] << 8) & 0xff00) | (buf[1] & 0xff);
    } // End of function i16decode.

//----------------------//
//       i32decode      //
//----------------------//
int32 i32decode(const char buf[4]) {
    register uint32 x = (uint32)buf[0] & 0xff;
    x <<= 8;
    x |= (uint32)buf[1] & 0xff;
    x <<= 8;
    x |= (uint32)buf[2] & 0xff;
    x <<= 8;
    x |= (uint32)buf[3] & 0xff;
    return (int32)x;
    } // End of function i32decode.

/*------------------------------------------------------------------------------
Warning: This might not be quite portable as is.
Check this for non-MSByte machines!!!!
For different kinds of machines, may have to do different codings here
to get portability of double float across networks.
------------------------------------------------------------------------------*/
//----------------------//
//      dbl_decode      //
//----------------------//
double dbl_decode(const char buf[sizeof(double)]) {
    // Union of double with properly aligned array (hopefully!).
    union {
        double d;
        char buf[sizeof(double)];
        } x;

    // Copy the bytes from the given array to a properly aligned array.
    for (int i = 0; i < sizeof(double);  ++i)
        x.buf[i] = buf[i];

    return x.d;
    } // End of function dbl_decode.

/*------------------------------------------------------------------------------
This is the simple XOR of all bytes in the given array.
Neither this nor the other checksum functions are optimized to exploit the case
that there is 32-bit word alignment. Some day that should be done.
------------------------------------------------------------------------------*/
//----------------------//
//    u8checksum_xor    //
//----------------------//
uint8 u8checksum_xor(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint8 x = 0;
    while (--n >= 0)
        x ^= *buf++;
    return x;
    } // End of function u8checksum_xor.

/*------------------------------------------------------------------------------
This is the arithmetic sum of all bytes in the given array.
Neither this nor the other checksum functions are optimized to exploit the case
that there is 32-bit word alignment. Some day that should be done.
------------------------------------------------------------------------------*/
//----------------------//
//    u8checksum_add    //
//----------------------//
uint8 u8checksum_add(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint8 x = 0;
    while (--n >= 0)
        x += *buf++;
    return x;
    } // End of function u8checksum_add.

/*------------------------------------------------------------------------------
This is the simple XOR of all byte-pairs in the given array.
The returned value is in MSByte order.
------------------------------------------------------------------------------*/
//----------------------//
//    u16checksum_xor   //
//----------------------//
uint16 u16checksum_xor(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint8 x0 = 0;
    register uint8 x1 = 0;
    while ((n -= 2) >= 0) {
        x0 ^= *buf++;
        x1 ^= *buf++;
        }

    // If there's an odd byte left over, read that one too:
    if (n >= -1)
        x0 ^= *buf;

    return (x0 << 8) | (x1 & 0xff);
    } // End of function u16checksum_xor.

/*------------------------------------------------------------------------------
This is the arithmetic sum of all byte-pairs in the given array.
The returned value is in MSByte order.
------------------------------------------------------------------------------*/
//----------------------//
//    u16checksum_add   //
//----------------------//
uint16 u16checksum_add(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint16 x = 0;
    while ((n -= 2) >= 0) {
        register uint8 x0 = *buf++;
        register uint8 x1 = *buf++;
        x += uint16((x0 << 8) | x1);
        }

    // If there's an odd byte left over, read that one too:
    if (n >= -1)
        x += *buf << 8;

    return x;
    } // End of function u16checksum_add.

/*------------------------------------------------------------------------------
This is the 1's complement sum of all byte-pairs in the given array.
The returned value is in MSByte order.
------------------------------------------------------------------------------*/
//----------------------//
//   u16checksum_addx   //
//----------------------//
uint16 u16checksum_addx(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint32 x = 0;
    while ((n -= 2) >= 0) {
        register uint8 x0 = *buf++;
        register uint8 x1 = *buf++;
        x += uint32((x0 << 8) | x1);
        }

    // If there's an odd byte left over, read that one too:
    if (n >= -1)
        x += uint32(*buf << 8);

    // Add in the carry bits:
    register uint32 y = x & 0xffff;
    y += (x >> 16) & 0xffff;
    y += (y >> 16);     // Add in the carry bits from adding in the carry bits.

    return (y == 0xffff) ? 0 : (uint16)y;
    } // End of function u16checksum_addx.

//----------------------//
//   u16checksum_fold   //
//----------------------//
uint16 u16checksum_fold(uint16 x, uint16 y) {
    register uint32 z = uint32(x) + uint32(y);
    z += (z >> 16);
    return (z == 0xffff) ? 0 : (uint16)z;
    } // End of function u16checksum_fold.

/*------------------------------------------------------------------------------
This is the simple XOR of all quad-bytes in the given array.
The returned value is in MSByte order.
------------------------------------------------------------------------------*/
//----------------------//
//    u32checksum_xor   //
//----------------------//
uint32 u32checksum_xor(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint8 x0 = 0;      // MSByte.
    register uint8 x1 = 0;
    register uint8 x2 = 0;
    register uint8 x3 = 0;      // LSByte.
    while ((n -= 4) >= 0) {
        x0 ^= *buf++;
        x1 ^= *buf++;
        x2 ^= *buf++;
        x3 ^= *buf++;
        }

    // If there's an odd byte left over, read that one too:
    if (n >= -1)
        x0 ^= *buf++;
    if (n >= -2)
        x1 ^= *buf++;
    if (n >= -3)
        x2 ^= *buf;

    uint32 x = (x3 << 8) | x2;
    x <<= 8;
    x |= x1;
    x <<= 8;
    x |= x0;

    return x;
    } // End of function u32checksum_xor.

/*------------------------------------------------------------------------------
This is the arithmetic sum of all quad-bytes in the given array.
The returned value is in MSByte order.
------------------------------------------------------------------------------*/
//----------------------//
//    u32checksum_add   //
//----------------------//
uint32 u32checksum_add(const char* buf, int n) {
    if (!buf)
        return 0;
    register uint32 x = 0;
    while ((n -= 4) >= 0) {
        register uint32 y = *buf++ << 8;
        y |= *buf++;
        y <<= 8;
        y |= *buf++;
        y <<= 8;
        y |= *buf++;
        x += y;
        }

    // If there are odd bytes left over, read them also:
    if (n >= -1)
        x += (*buf++ << 24);
    if (n >= -2)
        x += (*buf++ << 16);
    if (n >= -3)
        x += (*buf << 8);

    return x;
    } // End of function u32checksum_add.

/*------------------------------------------------------------------------------
This parses a string of the form "i:j:k", where i, j and k are long integers.
Return value:
-1      error
1       parsed string of form "i"
2       parsed string of form "i:j"
3       parsed string of form "i:j:k"
If there is an error, none of the arguments are altered.
If the parsing is successful, all of the arguments are altered.
The unparsed integer parameters are filled with zeros.
If the parsing is successful, "p" is replaced with a pointer to the character
which terminates the last parsed integer.
To check if the parsing exhausted the given string, just check *p.
------------------------------------------------------------------------------*/
//----------------------//
//       parse_lll      //
//----------------------//
int parse_lll(char*& p, long& i, long& j, long& k) {
    if (!p || !*p)
        return -1;

    // Read the first integer.
    char* q1 = p;
    char* q2 = 0;
    long a = strtol(q1, &q2, 0);
    if (!q2 || q2 == q1)
        return -1;

    // If this is the end of the string, fill in the rest of the fields.
    if (!*q2 || *q2 != ':') {   // This has a redundancy.
        p = q2;
        i = a;
        j = 0;
        k = 0;
        return 1;
        }

    // Check for the ':' character.
//    if (*q2 != ':')
//        return -1;

    // Skip the ':' character and read the second integer.
    q1 = q2 + 1;
    q2 = 0;
    long b = strtol(q1, &q2, 0);
    if (!q2 || q2 == q1)
        return -1;

    // If this is the end of the string, fill in the rest of the fields.
    if (!*q2 || *q2 != ':') {   // This has a redundancy.
        p = q2;
        i = a;
        j = b;
        k = 0;
        return 2;
        }

    // Check for the ':' character.
//    if (*q2 != ':')
//        return -1;

    // Skip the ':' character and read the third integer.
    q1 = q2 + 1;
    q2 = 0;
    long c = strtol(q1, &q2, 0);
    if (!q2 || q2 == q1)
        return -1;

    // Return the values.
    p = q2;
    i = a;
    j = b;
    k = c;
    return 3;
    } // End of function parse_lll.

/*------------------------------------------------------------------------------
This is the int16 version of parse_lll.
The arguments are checked additionally to see if they fit into the range
of an int16. If any does not fit, then none are returned.
------------------------------------------------------------------------------*/
//----------------------//
//   parse_i16i16i16    //
//----------------------//
int parse_i16i16i16(char*& p, int16& i, int16& j, int16& k) {
    char* q = p;
    long a = 0;
    long b = 0;
    long c = 0;
    int n_valid = parse_lll(q, a, b, c);
    if (n_valid < 0)
        return n_valid;

    // Check the range of the parsed integers.
    if (a < min_int16 || a > max_int16)
        return -1;
    if (b < min_int16 || b > max_int16)
        return -1;
    if (c < min_int16 || c > max_int16)
        return -1;

    // If everything is fine, set the arguments.
    p = q;
    i = int16(a);
    j = int16(b);
    k = int16(c);
    return n_valid;
    } // End of function parse_i16i16i16.

/*------------------------------------------------------------------------------
This is the uint16 version of parse_lll.
The arguments are checked additionally to see if they fit into the range
of a uint16. If any does not fit, then none are returned.
------------------------------------------------------------------------------*/
//----------------------//
//   parse_u16u16u16    //
//----------------------//
int parse_u16u16u16(char*& p, uint16& i, uint16& j, uint16& k) {
    char* q = p;
    long a = 0;
    long b = 0;
    long c = 0;
    int n_valid = parse_lll(q, a, b, c);
    if (n_valid < 0)
        return n_valid;

    // Check the range of the parsed integers.
    if (a < min_uint16 || a > max_uint16)
        return -1;
    if (b < min_uint16 || b > max_uint16)
        return -1;
    if (c < min_uint16 || c > max_uint16)
        return -1;

    // If everything is fine, set the arguments.
    p = q;
    i = uint16(a);
    j = uint16(b);
    k = uint16(c);
    return n_valid;
    } // End of function parse_u16u16u16.
