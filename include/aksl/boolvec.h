// src/aksl/boolvec.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/boolvec.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_BOOLVEC_H
#define AKSL_BOOLVEC_H
/*------------------------------------------------------------------------------
Classes in this file:

boole32::
boole_vec_l::
boole_vec_r::
boole_vec::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif
#ifndef AKSL_BINDEF_H
#include "aksl/bindef.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif

inline long mod4(long i) { return i & bin_right_mask_2; }
inline long div4(long i) { return i >> 2; }
inline long ceil_div4(long i) { return (i + 3) >> 2; }

inline long mod8(int i) { return i & bin_right_mask_3; }
inline long div8(int i) { return i >> 3; }
inline long ceil_div8(int i) { return (i + 7) >> 3; }

inline long mod8(long i) { return i & bin_right_mask_3; }
inline long div8(long i) { return i >> 3; }
inline long ceil_div8(long i) { return (i + 7) >> 3; }

inline long mod32(long i) { return i & bin_right_mask_5; }
inline long mod32c(long i) { return 31 - (i & bin_right_mask_5); }
inline long div32(long i) { return i >> 5; }
inline long ceil_div32(long i) { return (i + 31) >> 5; }

// Some arrays to make programs go faster.
extern uint8 boole_left_bit_8[8];
extern uint8 boole_right_bit_8[8];
extern uint8 boole_compl_left_bit_8[8];
extern uint8 boole_compl_right_bit_8[8];
extern uint8 boole_left_mask_8[9];
extern uint8 boole_right_mask_8[9];

extern uint32 boole_left_bit_32[32];
extern uint32 boole_right_bit_32[32];
extern uint32 boole_compl_left_bit_32[32];
extern uint32 boole_compl_right_bit_32[32];
extern uint32 boole_left_mask_32[33];
extern uint32 boole_right_mask_32[33];

/*------------------------------------------------------------------------------
This ad hoc version of boole32 does not check for bad arguments.
------------------------------------------------------------------------------*/
//----------------------//
//       boole32::      //
//----------------------//
struct boole32 {
private:
    uint32 mask;
public:
    // Would table lookups be faster????
    void set(int i) { mask |= (1 << i); }
    void clr(int i) { mask &= ~(1 << i); }
    bool_enum get(int i) { return (bool_enum)((mask & (1 << i)) != 0); }
    void clear() { mask = 0; }

    boole32() { mask = 0; }
    ~boole32() {}
    }; // End of struct boole32.

/*------------------------------------------------------------------------------
It is assumed that "vec" is zero if and only if "nblocks" equals zero.
"length" must always be <= nblocks * 32.
------------------------------------------------------------------------------*/
//----------------------//
//     boole_vec_l::    //
//----------------------//
struct boole_vec_l {
private:
    uint32* vec;                // The array of bits.
    long nblocks;               // Number of 32-bit words in "vec".
    long length;                // Number of elements in the array.

    int in_range(long i) const { return i >= 0 && i < length; }
    void instantiate(long i);

    // The following 3 functions must be called only if "i" is in range.
    void set_bit(long i) { vec[div32(i)] |= boole_left_bit_32[mod32(i)]; }
    void clear_bit(long i)
        { vec[div32(i)] &= boole_compl_left_bit_32[mod32(i)]; }
    bool_enum get_bit(long i) const
        { return (bool_enum)(vec[div32(i)] & boole_left_bit_32[mod32(i)] ?
                    true : false); }
    void free_mem();            // Release all memory.
public:
    void clear();               // Set all element values to 0.
    void resize(long);
    long size_bits() { return length; }
    long size_bytes() { return ceil_div8(length); }
    long size_word32() { return ceil_div32(length); }

    // Versions which do not extend the array when needed:
    void set_in_range(long i) { if (in_range(i)) set_bit(i); }
    void clear_in_range(long i) { if (in_range(i)) clear_bit(i); }
    void set_in_range(long i, bool_enum x)
        { if (in_range(i)) { if (x) set_bit(i); else clear_bit(i); } }

    // Versions which _do_ extend the array when needed:
    void set(long i) { if (i >= 0) { instantiate(i); set_bit(i); } }
    void clear(long i) { if (i >= 0) { instantiate(i); clear_bit(i); } }
    void set(long i, bool_enum x)
        { if (i >= 0) { instantiate(i); if (x) set_bit(i); else clear_bit(i);}}
    void set_from(const char* buf, long offset, long n);
    void set_from(const boole_vec_l& x, long offset);
    bool_enum get(long i) const { return in_range(i) ? get_bit(i) : false; }

    void copy_from(const boole_vec_l& x, long offset, long n);
    void copy_to(boole_vec_l& x, long offset, long n) const
        { x.copy_from(*this, offset, n); }
    void copy_to(char* buf, long bufsize);
    void print(ostream& = cout);

    boole_vec_l(const char* buf, long buflen);
    boole_vec_l(long s);
    boole_vec_l() { length = 0; nblocks = 0; vec = 0; }
    ~boole_vec_l() { delete[] vec; }
    }; // End of struct boole_vec_l.

/*------------------------------------------------------------------------------
It is assumed that "vec" is zero if and only if "nblocks" equals zero.
This differs from boole_vec_l in having bits stored right-justified in
32-bit words.
------------------------------------------------------------------------------*/
//----------------------//
//     boole_vec_r::    //
//----------------------//
struct boole_vec_r {
private:
    long length;                // Number of elements in the array.
    long nblocks;               // Number of 32-bit words.
    uint32* vec;                // The array itself.

    int in_range(long i) { return i >= 0 && i < length; }
    void set_bit(long i) { vec[div32(i)] |= boole_right_bit_32[mod32(i)]; }
    void clear_bit(long i)
        { vec[div32(i)] &= boole_compl_right_bit_32[mod32(i)]; }
    bool_enum get_bit(long i)
//        { return (bool_enum)(vec[i/32] & boole_right_bit_32[i%32] != 0); }
        { return (bool_enum)(vec[div32(i)] & boole_right_bit_32[mod32(i)] ?
                    true : false); }
    void free_mem();            // Release all memory.
public:
    void clear();               // Set all element values to 0.
    void resize(long);
    long size_bits() { return length; }
    long size_bytes() { return ceil_div8(length); }
    long size_word32() { return ceil_div32(length); }
    void set(long i) { if (in_range(i)) set_bit(i); }
    void clear(long i) { if (in_range(i)) clear_bit(i); }
    void set(long i, bool_enum x)
        { if (in_range(i)) { if (x) set_bit(i); else clear_bit(i); } }
    bool_enum get(long i) { return in_range(i) ? get_bit(i) : false; }
//    void copy_from(const boole_vec_r& x, long offset, long n);
//    void copy_to(boole_vec_r& x, long offset, long n) const
//        { x.copy_from(*this, offset, n); }
//    void copy_to(char* buf, long bufsize);

    boole_vec_r(long s = 32);
    ~boole_vec_r() { delete[] vec; }
    }; // End of struct boole_vec_r.

typedef boole_vec_r boole_vec;

#endif /* AKSL_BOOLVEC_H */
