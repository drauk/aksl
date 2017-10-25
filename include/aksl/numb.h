// src/aksl/numb.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
// Mostly integer things.
#ifndef AKSL_NUMB_H
#define AKSL_NUMB_H
/*------------------------------------------------------------------------------
Classes in this file:

------------------------------------------------------------------------------*/

//==============================================================================
//  Integers
//==============================================================================

typedef char            int8;
typedef short           int16;
typedef long            int32;

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long   uint32;

const int8              max_int8    = 0x7f;
const int16             max_int16   = 0x7fff;
const int32             max_int32   = 0x7fffffff;
const int8              min_int8    = int8(0x80);
const int16             min_int16   = int16(0x8000);
const int32             min_int32   = 0x80000000;

const uint8             max_uint8   = 0xff;
const uint16            max_uint16  = 0xffff;
const uint32            max_uint32  = 0xffffffff;
const uint8             min_uint8   = 0x00;
const uint16            min_uint16  = 0x0000;
const uint32            min_uint32  = 0x00000000;

typedef unsigned char   ubyte;
typedef unsigned int    uint;
typedef unsigned long   ulong;

inline int    max2(int a, int b)       { return (a > b) ? a : b; }
inline int    min2(int a, int b)       { return (a < b) ? a : b; }
inline long   max2(long a, long b)     { return (a > b) ? a : b; }
inline long   min2(long a, long b)     { return (a < b) ? a : b; }
inline uint   max2(uint a, uint b)     { return (a > b) ? a : b; }
inline uint   min2(uint a, uint b)     { return (a < b) ? a : b; }
inline ulong  max2(ulong a, ulong b)   { return (a > b) ? a : b; }
inline ulong  min2(ulong a, ulong b)   { return (a < b) ? a : b; }
inline double max2(double a, double b) { return (a > b) ? a : b; }
inline double min2(double a, double b) { return (a < b) ? a : b; }

// Inline floor function for integer division.
//----------------------//
//        ilfloor       //
//----------------------//
inline long ilfloor(long n, long d) {
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
    } // End of function ilfloor.

// Callable function version of "ilfloor".
extern long lfloor(long n, long d);

// Functions to do host <-> network coding of 16 and 32 bit integers:
extern void u16encode(char buf[2], uint16 x);
extern void u32encode(char buf[4], uint32 x);
extern void i16encode(char buf[2], int16 x);
extern void i32encode(char buf[4], int32 x);
extern void dbl_encode(char buf[sizeof(double)], double x);

extern uint16 u16decode(const char buf[2]);
extern uint32 u32decode(const char buf[4]);
extern int16 i16decode(const char buf[2]);
extern int32 i32decode(const char buf[4]);
extern double dbl_decode(const char buf[sizeof(double)]);

/*------------------------------------------------------------------------------
Inline functions to do machine-independent (hopefully!) reading/writing
of integers in "network byte order", i.e. in MSByte order.
The pointer is always advanced in these functions.
Note that this code will not work unchanged if uint8* becomes char*!!!
Maybe should write a char* version too...
------------------------------------------------------------------------------*/
inline void u16get(uint8*& pc, uint16& x) {
    x = (*pc++) << 8;
    x |= *pc++;
    }
inline void u32get(uint8*& pc, uint32& x) {
    x = (*pc++) << 8;
    x |= *pc++; x <<= 8;
    x |= *pc++; x <<= 8;
    x |= *pc++;
    }

inline uint16 u16get(uint8*& pc) {
    uint16 x;
    u16get(pc, x);
    return x;
    }
inline uint32 u32get(uint8*& pc) {
    uint32 x;
    u32get(pc, x);
    return x;
    }

inline void u16put(uint8*& pc, uint16 x) {
    *pc++ = uint8(x >> 8);
    *pc++ = uint8(x);
    }
inline void u32put(uint8*& pc, uint32 x) {
    // This is clumsy because there is no bit-rotate operator in C/C++.
    // This code tries to optimise for slow bit-shift.
    pc[3] = uint8(x);
    x >>= 8; pc[2] = uint8(x);
    x >>= 8; pc[1] = uint8(x);
    *pc = uint8(x >> 8);
    pc += sizeof(uint32);
    }

// XOR and arithmetic-add checksums:
extern uint8  u8checksum_xor(const char* buf, int n);
extern uint8  u8checksum_add(const char* buf, int n);
extern uint16 u16checksum_xor(const char* buf, int n);
extern uint16 u16checksum_add(const char* buf, int n);
extern uint16 u16checksum_addx(const char* buf, int n);
extern uint16 u16checksum_fold(uint16 x, uint16 y);
extern uint32 u32checksum_xor(const char* buf, int n);
extern uint32 u32checksum_add(const char* buf, int n);

// Some simple special-purpose parsing functions.
extern int parse_lll(char*& p, long& i, long& j, long& k);
extern int parse_i16i16i16(char*& p, int16& i, int16& j, int16& k);
extern int parse_u16u16u16(char*& p, uint16& i, uint16& j, uint16& k);

#endif /* AKSL_NUMB_H */
