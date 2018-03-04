// src/aksl/boole.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_BOOLE_H
#define AKSL_BOOLE_H
/*------------------------------------------------------------------------------
Classes in this file:

bool_obj::
------------------------------------------------------------------------------*/

// Kludge for the sake of GNU:
#if defined(__GNUC__) || defined(__WIN32__)
#define true    TrUe
#define false   FaLsE
#endif

enum bool_enum { false, true };     // Type-safe boolean variable.
typedef unsigned char bool_int;     // Compact form of boolean variable.

/*------------------------------------------------------------------------------
This bool_obj class is so safe that it is not really needed.
Enumerator "bool_enum" is usually safe enough.
------------------------------------------------------------------------------*/
//----------------------//
//      bool_obj::      //
//----------------------//
struct bool_obj {
private:
    short value;
public:
    // Outgoing assignments:
    operator int() const { return value; }

    // Incoming assignments:
    bool_obj& operator=(const int x)
        { value = x ? true : false; return *this; }
    bool_obj& operator=(const bool_enum x) { value = x; return *this; }
//    bool_obj& operator=(const bool_obj& x) {}

    // Con/destructors:
//    bool_obj(const bool_obj& x) {};
    bool_obj(const bool_enum& x) { value = (short)x; };
    bool_obj() { value = false; }
    ~bool_obj() {}
    }; // End of struct bool_obj.

inline const char* be_string(bool_enum b)
    { return (b == true) ? "true" : "false"; }

#endif /* AKSL_BOOLE_H */
