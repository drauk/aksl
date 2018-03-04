// src/aksl/num.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

num::
    encoding_length
    encode
    decode
    print
    operator uint8
    operator uint16
    operator uint32
    operator int8
    operator int16
    operator int32
    operator double
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/num.h"

//----------------------//
// num::encoding_length //
//----------------------//
int num::encoding_length() {
    // 1 bytes for the number-type.
    int n_bytes = 1;

    // Add the bytes for the number itself.
    switch (typ) {
    case ntNULL:
        // Nothing to do.
        break;
    case ntUI8:
        n_bytes += 1;
        break;
    case ntUI16:
        n_bytes += 2;
        break;
    case ntUI32:
        n_bytes += 4;
        break;
    case ntI8:
        n_bytes += 1;
        break;
    case ntI16:
        n_bytes += 2;
        break;
    case ntI32:
        n_bytes += 4;
        break;
    case ntD:
        n_bytes += sizeof(double);
        break;
    default:
        return 0;
        } // End of switch (typ).

    return n_bytes;
    } // End of function num::encoding_length.

/*------------------------------------------------------------------------------
The return value is the number of bytes written.
If the return value is 0, then an error has occurred.
------------------------------------------------------------------------------*/
//----------------------//
//      num::encode     //
//----------------------//
int num::encode(char* buf, int buflen) {
    if (!buf || buflen < 1)
        return 0;

    int n_bytes = 1;
    switch (typ) {
    case ntNULL:
        // No value to encode.
        break;
    case ntUI8:
        if (buflen < 2)
            return 0;
        buf[1] = (char)ui8;
        n_bytes += 1;
        break;
    case ntUI16:
        if (buflen < 3)
            return 0;
        u16encode(buf + 1, ui16);
        n_bytes += 2;
        break;
    case ntUI32:
        if (buflen < 5)
            return 0;
        u32encode(buf + 1, ui32);
        n_bytes += 4;
        break;
    case ntI8:
        if (buflen < 2)
            return 0;
        buf[1] = (char)i8;
        n_bytes += 1;
        break;
    case ntI16:
        if (buflen < 3)
            return 0;
        u16encode(buf + 1, (uint16)i16);
        n_bytes += 2;
        break;
    case ntI32:
        if (buflen < 5)
            return 0;
        u32encode(buf + 1, (uint32)i32);
        n_bytes += 4;
        break;
    case ntD:
        if (buflen < 1 + sizeof(double))
            return 0;
        dbl_encode(buf + 1, d);
        n_bytes += sizeof(double);
        break;
    default:
        return 0;
        } // End of switch (typ).

    // Write the number type to the first byte.
    *buf = (char)typ;
    return n_bytes;
    } // End of function num::encode.

/*------------------------------------------------------------------------------
The return value is the number of bytes successfully decoded.
------------------------------------------------------------------------------*/
//----------------------//
//      num::decode     //
//----------------------//
int num::decode(const char* buf, int buflen) {
    if (!buf || buflen < 1) {
        typ = ntNULL;
        return 0;
        }

    int n_bytes = 1;
    switch (num_t(buf[0])) {
    case ntNULL:
        // Nothing to do.
        break;
    case ntUI8:
        if (buflen < 2) {
            typ = ntNULL;
            return 0;
            }
        ui8 = (uint8)buf[1];
        n_bytes += 1;
        break;
    case ntUI16:
        if (buflen < 3) {
            typ = ntNULL;
            return 0;
            }
        ui16 = u16decode(buf + 1);
        n_bytes += 2;
        break;
    case ntUI32:
        if (buflen < 5) {
            typ = ntNULL;
            return 0;
            }
        ui32 = u32decode(buf + 1);
        n_bytes += 4;
        break;
    case ntI8:
        if (buflen < 2) {
            typ = ntNULL;
            return 0;
            }
        i8 = (int8)buf[1];
        n_bytes += 1;
        break;
    case ntI16:
        if (buflen < 3) {
            typ = ntNULL;
            return 0;
            }
        i16 = i16decode(buf + 1);
        n_bytes += 2;
        break;
    case ntI32:
        if (buflen < 5) {
            typ = ntNULL;
            return 0;
            }
        i32 = i32decode(buf + 1);
        n_bytes += 4;
        break;
    case ntD:
        if (buflen < 1 + sizeof(double)) {
            typ = ntNULL;
            return 0;
            }
        d = dbl_decode(buf + 1);
        n_bytes += sizeof(double);
        break;
    default:
        return 0;
        } // End of switch (typ).

    // If (and only if) successful, copy the number-type.
    typ = num_t(buf[0]);
    return n_bytes;
    } // End of function num::decode.

//----------------------//
//       num::print     //
//----------------------//
void num::print(ostream& os) {
    switch (typ) {
    case ntUI8:
        os << "uint8: " << (unsigned long)ui8;
        break;
    case ntUI16:
        os << "uint16: " << (unsigned long)ui16;
        break;
    case ntUI32:
        os << "uint32: " << (unsigned long)ui32;
        break;
    case ntI8:
        os << "int8: " << (long)i8;
        break;
    case ntI16:
        os << "int16: " << (long)i16;
        break;
    case ntI32:
        os << "int32: " << (long)i32;
        break;
    case ntD:
        os << "double: " << d;
        break;
    case ntNULL:
        os << "[null]";
        break;
    default:
        os << "[undefined]";
        break;
        }
    } // End of function num::print.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
//  num::operator uint8 //
//----------------------//
num::operator uint8() const {
    switch(typ) {
    case ntUI8:
        return (uint8)ui8;
    case ntUI16:
        return (uint8)ui16;
    case ntUI32:
        return (uint8)ui32;
    case ntI8:
        return (uint8)i8;
    case ntI16:
        return (uint8)i16;
    case ntI32:
        return (uint8)i32;
    case ntD:
        return (uint8)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator uint8.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
// num::operator uint16 //
//----------------------//
num::operator uint16() const {
    switch(typ) {
    case ntUI8:
        return (uint16)ui8;
    case ntUI16:
        return (uint16)ui16;
    case ntUI32:
        return (uint16)ui32;
    case ntI8:
        return (uint16)i8;
    case ntI16:
        return (uint16)i16;
    case ntI32:
        return (uint16)i32;
    case ntD:
        return (uint16)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator uint16.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
// num::operator uint32 //
//----------------------//
num::operator uint32() const {
    switch(typ) {
    case ntUI8:
        return (uint32)ui8;
    case ntUI16:
        return (uint32)ui16;
    case ntUI32:
        return (uint32)ui32;
    case ntI8:
        return (uint32)i8;
    case ntI16:
        return (uint32)i16;
    case ntI32:
        return (uint32)i32;
    case ntD:
        return (uint32)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator uint32.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
//  num::operator int8  //
//----------------------//
num::operator int8() const {
    switch(typ) {
    case ntUI8:
        return (int8)ui8;
    case ntUI16:
        return (int8)ui16;
    case ntUI32:
        return (int8)ui32;
    case ntI8:
        return (int8)i8;
    case ntI16:
        return (int8)i16;
    case ntI32:
        return (int8)i32;
    case ntD:
        return (int8)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator int8.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
//  num::operator int16 //
//----------------------//
num::operator int16() const {
    switch(typ) {
    case ntUI8:
        return (int16)ui8;
    case ntUI16:
        return (int16)ui16;
    case ntUI32:
        return (int16)ui32;
    case ntI8:
        return (int16)i8;
    case ntI16:
        return (int16)i16;
    case ntI32:
        return (int16)i32;
    case ntD:
        return (int16)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator int16.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
//  num::operator int32 //
//----------------------//
num::operator int32() const {
    switch(typ) {
    case ntUI8:
        return (int32)ui8;
    case ntUI16:
        return (int32)ui16;
    case ntUI32:
        return (int32)ui32;
    case ntI8:
        return (int32)i8;
    case ntI16:
        return (int32)i16;
    case ntI32:
        return (int32)i32;
    case ntD:
        return (int32)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator int32.

/*------------------------------------------------------------------------------
This function makes no serious attempt to convert out-of-range numbers into
range.
------------------------------------------------------------------------------*/
//----------------------//
// num::operator double //
//----------------------//
num::operator double() const {
    switch(typ) {
    case ntUI8:
        return (double)ui8;
    case ntUI16:
        return (double)ui16;
    case ntUI32:
        return (double)ui32;
    case ntI8:
        return (double)i8;
    case ntI16:
        return (double)i16;
    case ntI32:
        return (double)i32;
    case ntD:
        return (double)d;
    default:
    case ntNULL:
        return 0;
        }
    } // End of function num::operator double.
