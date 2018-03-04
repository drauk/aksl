// src/aksl/num.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
// Mostly integer things.

#ifndef AKSL_NUM_H
#define AKSL_NUM_H
/*------------------------------------------------------------------------------
Classes in this file:

num::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files.
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files.
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif

// Type of value of object of class "num".
enum num_t {
    ntNULL,
    ntUI8,
    ntUI16,
    ntUI32,
    ntI8,
    ntI16,
    ntI32,
    ntD
    };

/*------------------------------------------------------------------------------
There is a "standard encoding" so that typed integers can be
sent reliably across networks etc.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Probably should not use type "double" until standard encoding questions
are resolved for floating types. [Do some research on this...]
------------------------------------------------------------------------------*/
//----------------------//
//         num::        //
//----------------------//
struct num {
private:
    num_t           typ;            // Type of number.
    union {                         // Value of the number.
        uint8       ui8;
        uint16      ui16;
        uint32      ui32;
        int8        i8;
        int16       i16;
        int32       i32;
        double      d;
        };
public:
    num_t type() { return typ; }
    int encoding_length();              // Number of bytes in encoding.

    // Encode this object to a byte stream.
    int encode(char* buf, int buflen);

    // Decode this object from a byte stream.
    int decode(const char* buf, int buflen);

    void print(ostream& = cout);

    // Cast operators to plain numbers.
    operator uint8() const;
    operator uint16() const;
    operator uint32() const;
    operator int8() const;
    operator int16() const;
    operator int32() const;
    operator double() const;

    // Assignment operators for plain numbers.
    num& operator=(uint8 x) { ui8 = x; typ = ntUI8; return *this; }
    num& operator=(uint16 x) { ui16 = x; typ = ntUI16; return *this; }
    num& operator=(uint32 x) { ui32 = x; typ = ntUI32; return *this; }
    num& operator=(int8 x) { i8 = x; typ = ntI8; return *this; }
    num& operator=(int16 x) { i16 = x; typ = ntI16; return *this; }
    num& operator=(int32 x) { i32 = x; typ = ntI32; return *this; }
    num& operator=(double x) { d = x; typ = ntD; return *this; }

    // Constructors (initialisers) for plain numbers.
    num(uint8 x) { ui8 = x; typ = ntUI8; }
    num(uint16 x) { ui16 = x; typ = ntUI16; }
    num(uint32 x) { ui32 = x; typ = ntUI32; }
    num(int8 x) { i8 = x; typ = ntI8; }
    num(int16 x) { i16 = x; typ = ntI16; }
    num(int32 x) { i32 = x; typ = ntI32; }
    num(double x) { d = x; typ = ntD; }

    // Hopefully the default assignment and initialiser will be bitwise copies.
//    num& operator=(const num& x) {}
//    num(const num& x) {}
    num() { typ = ntNULL; }
    ~num() {}
    }; // End of struct num.

#endif /* AKSL_NUM_H */
