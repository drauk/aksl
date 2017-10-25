// src/aksl/form.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

vform
#if EXPT_FORM_STRING
form_string
// vlform
#endif
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/form.h"
#ifndef AKSL_CHARBUF_H
#include "aksl/charbuf.h"
#endif
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_STDARG_H
#define AKSL_X_STDARG_H
#include <stdarg.h>
#endif
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

/*------------------------------------------------------------------------------
This function uses vsprintf to format the string and then returns a copy which
is heap-allocated. Very large strings could fail by overflowing the buffer.
------------------------------------------------------------------------------*/
//----------------------//
//         vform        //
//----------------------//
#ifdef HAVE_VPRINTF
char* vform(const char* fmt, ...) {
    const int bufsize = 1024;
    static char buf[bufsize];

    if (nullstr(fmt))
        *buf = 0;
    else {
        va_list pvar;
        va_start(pvar, fmt);
        vsprintf(buf, fmt, pvar);
        va_end(pvar);
        }
    register char* pc1 = 0;
    for (pc1 = buf; *pc1; ++pc1)
        ;
    int n = pc1 - buf;          // n = strlen(buf).
    char* new_string = new char[n + 1];
    pc1 = buf;
    register char* pc2 = new_string;
    while (*pc2++ = *pc1++);    // strcpy(new_string, buf).

    return new_string;
    } // End of function vform.
#endif /* HAVE_VPRINTF */

/*------------------------------------------------------------------------------
The returned string is allocated on the heap, and should be deleted when
finished. The returned pointer is never
null, although the null-terminated string it points to may be an empty string.
The full formatted string can be of arbitrary length.
This sort of function should have been in stdio.h. It would have been so easy to
provide it, since it is almost identical to sprintf().
------------------------------------------------------------------------------*/
//----------------------//
//      form_string     //
//----------------------//
#if EXPT_FORM_STRING
char* form_string(const char* format, ...) {
    static c_array cbuf;        // The infinite buffer for the whole string.
//    const int bufsize = 1024;
//    static char buf[bufsize];   // Working buffer for string components.

    cbuf.reset();
    if (nullstr(format))
        return cbuf.copy0();

    va_list pvar;
    va_start(pvar, format);
    const char* pc = format;
    char c;
    while ((c = *pc++) != 0) {
        if (c == '%') { // Substitution symbol.
            int fw_sign = 1;
            bool_enum zero_padding = false;
            int field_width = 0;
            int precision = -1;     // -1 means undefined here.
            char size = 0;          // 'h' for short, 'l' for long.

            enum { // Parse state [the last field seen]:
                pBEGIN, pFW_SIGN, pFW, pPREC, pSIZE, pTYPE, pEND
                } pstate = pBEGIN;

            while (pstate < pEND && (c = *pc++) != 0) {
                switch(c) { // Interpret the printf escape codes:
                case '-':
                    if (pstate < pFW_SIGN) {
                        fw_sign = -1;
                        pstate = pFW_SIGN;
                        }
                    break;
                case '0':
                    if (pstate < pFW)
                        zero_padding = true;
                    // The non-break here is intentional!
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                    if (pstate < pFW) {
                        field_width = 0;
                        do {
                            if (!isdigit(c)) {
                                --pc;
                                break;
                                }
                            field_width *= 10;
                            field_width += c - '0'; // Assume ascii etc.
                            } while ((c = *pc++) != 0);
                        if (c == 0)
                            pstate = pEND;
                        else
                            pstate = pFW;
                        }
                    else if (pstate < pPREC) {
                        precision = 0;
                        do {
                            if (!isdigit(c)) {
                                --pc;
                                break;
                                }
                            precision *= 10;
                            precision += c - '0'; // Assume ascii etc.
                            } while ((c = *pc++) != 0);
                        if (c == 0)
                            pstate = pEND;
                        else
                            pstate = pPREC;
                        }
                    break;
                case '.':   // Separator for field width and precision.
                    if (pstate < pFW)
                        pstate = pFW;
                    break;
                case '*':   // Field width or precision is an int argument.
                    if (pstate < pFW) {
                        field_width = va_arg(pvar, int);
                        pstate = pFW;
                        }
                    else if (pstate < pPREC) {
                        precision = va_arg(pvar, int);
                        pstate = pPREC;
                        }
                    break;
                case 'h': // Size for d, o, x, u is short.
                    if (pstate < pSIZE) {
                        size = 'h';
                        pstate = pSIZE;
                        }
                    break;
                case 'l': // Size for d, o, x, u is long.
                    if (pstate < pSIZE) {
                        size = 'l';
                        pstate = pSIZE;
                        }
                    break;
                case 'c': { // %c
                    char x = va_arg(pvar, char);
                    if (x)  // Ignore null characters.
                        cbuf.append(x);
                    pstate = pEND;
                    }
                    break;
                case 's': { // %s
                    const char* s = va_arg(pvar, const char*);
                    if (precision < 0)
                        precision = 0;
                    cbuf.append(s, precision);
                    pstate = pEND;
                    }
                    break;
                case 'd':
                case 'i':
                    { // %d, %i: decimal.
                    long i = 0;
                    if (size == 'h')
                        i = va_arg(pvar, short int);
                    else if (size == 'l')
                        i = va_arg(pvar, long int);
                    else
                        i = va_arg(pvar, int);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 1;
                    cbuf.append(intstring(i, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'u': { // %u: unsigned decimal.
                    unsigned long i = 0;
                    if (size == 'h')
                        i = va_arg(pvar, unsigned short int);
                    else if (size == 'l')
                        i = va_arg(pvar, unsigned long int);
                    else
                        i = va_arg(pvar, unsigned int);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 1;
                    cbuf.append(uintstring(i, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'x': { // %x: hexadecimal.
                    long i = 0;
                    if (size == 'h')
                        i = va_arg(pvar, short int);
                    else if (size == 'l')
                        i = va_arg(pvar, long int);
                    else
                        i = va_arg(pvar, int);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 1;
                    cbuf.append(hexstring(i, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'o': { // %o: octal.
                    long i = 0;
                    if (size == 'h')
                        i = va_arg(pvar, short int);
                    else if (size == 'l')
                        i = va_arg(pvar, long int);
                    else
                        i = va_arg(pvar, int);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 1;
                    cbuf.append(octstring(i, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'p': { // %p, a pointer.
                    void* pv = va_arg(pvar, void*);
                    cbuf.append("0x");
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 1;
                    cbuf.append(hexstring(long(pv), field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'f': { // %f: float.
                    double r = va_arg(pvar, double);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 6;
                    cbuf.append(f_string(r, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'e': { // %e.
                    double r = va_arg(pvar, double);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 6;
                    cbuf.append(e_string(r, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case 'g': { // %g.
                    double r = va_arg(pvar, double);
                    field_width *= fw_sign;
                    if (precision < 0)
                        precision = 6;
                    cbuf.append(g_string(r, field_width, precision));
                    pstate = pEND;
                    }
                    break;
                case '%':   // %%
                    cbuf.append(c);
                    pstate = pEND;
                    break;
                case 0:     // Sudden end of string.
                    return cbuf.copy0();
                    // break;
                default:    // Actually this should be an error!?
                    cbuf.append(c);
                    pstate = pEND;
                    break;
                    } // End of switch(c).
                }
            }
        else {  // Plain character.
            cbuf.append(c);
            }
        }
    va_end(pvar);
    return cbuf.copy0();
    } // End of function form_string.

/*------------------------------------------------------------------------------
This is a secure form of the old stream.h formatting function "form".
It uses a "valuelist" instead of variable numbers/types of function arguments.
This should be placed in value.c or thereabouts.
Also have functions with 2, 3, or more value arguments. Type conversion would be
used to turn int, double, etc. into arguments of dynamic type.
These sorts of functions could be implemented as a sequence of functions popping
a single from the argument list and passing on the remainder of the string and
arguments to the next function?
Note that it would be best _not_ to use class "value", but rather to create a
new class of dynamic type which has only the required argument types.
------------------------------------------------------------------------------*/
//----------------------//
//      vformstring     //
//----------------------//
/*------------------------------------------------------------------------------
const char* vformstring(const char* format, value* pv1) {

    return 0;
    } // End of function vformstring.
------------------------------------------------------------------------------*/
#endif
