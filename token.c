// src/aksl/token.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/token.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

token::
    token
    print
token_stream::
    token_stream
    open
    next_token
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/token.h"

// System header files:
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif

//----------------------//
//     token::token     //
//----------------------//
token::token(token_t t) {
    switch(t) {
    case tokNULL:
        break;
    case tokID:
        id_type = idNULL;
        break;
    case tokLIT:
        lit_type = litNULL;
        break;
    case tokSYM:
        sym_type = symNULL;
        break;
    case tokERROR:
        break;
    default:    // An error!
        t = tokNULL;
        break;
        } // End of switch(t).
    type = t;
    line_number = 0;
    } // End of function token::token.

//----------------------//
//     token::print     //
//----------------------//
void token::print(ostream& os) {
    if (s != (const char*)"\n")
        os << "`" << (const char*)s << "' ";
    else
        os << "`\\n' ";
    switch(type) {
    case tokNULL:
        os << "[null]";
        break;
    case tokID:
        os << "[id:]";
        break;
    case tokLIT:
        os << "[lit:";
        switch(lit_type) {
        case litNULL:
            os << "null]";
            break;
        case litINTEGER:
            os << "integer] = " << i;
            break;
        case litREAL:
            os << "real] = " << r;
            break;
        case litSTRING:
            os << "string] = \"" << pc << "\"";
            break;
        case litERROR:
            os << "error]";
            break;
            } // End of switch(lit_type).
        break;
    case tokSYM:
        os << "[sym]";
        break;
    case tokERROR:
        os << "[error]";
        break;
    default:    // An error!
        os << "[undefined]";
        break;
        } // End of switch(type).
    } // End of function token::print.

/*------------------------------------------------------------------------------
This is here because of a bug in one of the compilers -- the GNU compiler, I
think.
------------------------------------------------------------------------------*/
//------------------------------//
//  token_stream::token_stream  //
//------------------------------//
token_stream::token_stream(istream& i, long l) {
    line_number = l;
    is = &i;
    already_open = (bool_enum)(is && *is);
    eof_found = false;
    linefeed_is_token = false;
    } // End of function token_stream::token_stream.

/*------------------------------------------------------------------------------
Returns "true" if the stream is successfully opened.
Note that for win32 API, the file is opened in text mode.
If this causes problems, use the ios::binary attribute when opening the file.
------------------------------------------------------------------------------*/
//----------------------//
//  token_stream::open  //
//----------------------//
bool_enum token_stream::open(const char* filename) {
    if (already_open)
        return false;

    if (!filename) {
        is = &cin;
        already_open = (bool_enum)(is && *is);
        }
#ifndef WIN32
    else if (fb.open(filename, ios::in)) {
#else
//    else if (fb.open(filename, ios::in | ios::binary)) {
    else if (fb.open(filename, ios::in)) {
#endif
        is = new istream(&fb);
        already_open = (bool_enum)(is && *is);
        }
    if (already_open) {
        line_number = 1;
        putback_tokens.clear();
        }
    return already_open;
    } // End of function token_stream::open.

/*------------------------------------------------------------------------------
Interprets a file to produce a token. If no token can be read, then the zero
pointer is returned.
------------------------------------------------------------------------------*/
//------------------------------//
//   token_stream::next_token   //
//------------------------------//
token* token_stream::next_token() {
    if (!already_open) {
        eof_found = true;
        return 0;
        }

    // First check if any tokens have been put back by the user:
    if (putback_tokens.first())
        return putback_tokens.popfirst();

    static charbuf cbuf, cbuf2;

    // Skip past white space:
    char c;
    while (is->get(c)) {
        if (c == '\n') {            // End of line.
            line_number += 1;
            if (linefeed_is_token)  // Consider line feed to be on next line.
                break;
            continue;
            }
        if (isspace(c))             // Space.
            continue;
        if (c == '#') {             // Comment to end of line.
            while (is->get(c))
                if (c == '\n') {
                    line_number += 1;
                    break;
                    }
            if (!is->good()) {
                eof_found = true;
                return 0;
                }
            if (linefeed_is_token)
                break;
            continue;
            }
        break; // Must be something other than space or comments.
        }
    if (!is->good()) {
        eof_found = true;
        return 0;
        }

    // Identifier:
    if (isalpha(c) || c == '_') { // Identifier.
        cbuf.reset();
        do {
            cbuf.append(c);
            } while (is->get(c) && (isalnum(c) || c == '_'));
        if (is->good())
            is->putback(c);
        token* pt = new token(tokID);
        pt->line_number = line_number;
        pt->s.eat(cbuf.copy0());
        return pt;
        }

    // Numerical literal:
    // [Note: negative constants are regarded as a non-negative constant
    // preceded by the operator '-'. Hence negative numbers need 2 tokens.]
    if (isdigit(c) || c == '.') {
        // Store the string:
        cbuf.reset();
        cbuf.append(c);

        // Check for the special case of "." without a following digit:
        if (c == '.') {
            // Peek at the next character:
            char c2;
            bool_enum single_dot = true;
            if (is->get(c2)) {
                if (isdigit(c2))
                    single_dot = false;
                is->putback(c2);
                }
            if (single_dot) {
                token* pt = new token(tokSYM);
                pt->line_number = line_number;
                pt->s.eat(cbuf.copy0());
                pt->sym_type = symDOT;
                return pt;
                }
            }

        // State variable for the lexical demangler:
        enum { nDEC, nOCT, nHEX, nFRAC, nEXP, nEXPS } ntype = nDEC;

        // Keep track of which parts have appeared:
        bool_enum int_part = true; // There is an integer part.
        bool_enum frac_part = false;
        bool_enum exp_part = false;

        if (c == '.') {
            int_part = false;
            ntype = nFRAC;
            }
        else if (c == '0' && is->get(c)) {
            // Interpret and remove possible prefixes: 0, 0x, 0X.
            if (isdigit(c)) {
                ntype = nOCT;
                is->putback(c);
                }
            else if (c == 'x' || c == 'X') {
                ntype = nHEX;
                cbuf.append(c); // Gobble the 'x' or 'X'.
                }
            else
                is->putback(c);
            }

        // A simple state machine to read the rest of the number:
        // [At the end of the loop, c is put back into the input stream.]
        bool_enum finished = false;
        bool_enum errored = false;
        while (is->get(c)) {
            switch(ntype) {
            case nDEC:
            case nOCT:
                // Note: In the case of an octal constant, the token is the
                // sequence of _decimal_ digits following the '0', although
                // the token is in error if any of the digits is '8' or '9'.
                if (isdigit(c))
                    ;
                else if (c == '.')
                    ntype = nFRAC;
                else if (c == 'e' || c == 'E')
                    ntype = nEXP;
                else {
                    finished = true;
                    break;
                    }
                cbuf.append(c);
                break;
            case nHEX:
                if (isxdigit(c))
                    ;
                else { // Float can't have hex integer part.
                    finished = true;
                    break;
                    }
                cbuf.append(c);
                break;
            case nFRAC:     // Fraction part.
                if (isdigit(c))
                    frac_part = true;
                else if ((c == 'e' || c == 'E') && (int_part || frac_part))
                    ntype = nEXP;
                else {
                    finished = true;
                    // Note that the following should never happen, because
                    // a single "." has been treated as a special case.
                    if (!int_part && !frac_part)
                        errored = true;
                    break;
                    }
                cbuf.append(c);
                break;
            case nEXP:      // Exponent.
                if (isdigit(c))
                    exp_part = true;
                else if (!exp_part && (c == '+' || c == '-'))
                    ntype = nEXPS;
                else {
                    finished = true;
                    if (!exp_part)
                        errored = true;
                    break;
                    }
                cbuf.append(c);
                break;
            case nEXPS:     // Signed exponent.
                if (isdigit(c))
                    exp_part = true;
                else {
                    finished = true;
                    if (!exp_part)
                        errored = true;
                    break;
                    }
                cbuf.append(c);
                break;
                } // End of switch(ntype).
            if (finished)
                break;
            } // End of "state machine" loop.
        if (is->good())
            is->putback(c);

        token* pt = new token(tokLIT);
        pt->line_number = line_number;
        char* pc = cbuf.copy0();
        if (ntype == nFRAC || ntype == nEXP || ntype == nEXPS) {
            if (errored)
                pt->lit_type = litERROR;
            else {
                char* pc2 = 0;
                double r = strtod(pc, &pc2);
                // Success  <=>  pc2 - pc == length(pc)  <=>  *pc2 == 0.
                if (pc2 && *pc2)
                    pt->lit_type = litERROR;
                else {
                    pt->r = r;
                    pt->lit_type = litREAL;
                    }
                }
            }
        else if (ntype == nDEC || ntype == nOCT || ntype == nHEX) {
            char* pc2 = 0;
            unsigned long i = strtol(pc, &pc2, 0);
            // Success  <=>  pc2 - pc == length(pc)  <=>  *pc2 == 0.
            if (pc2 && *pc2)
                pt->lit_type = litERROR;
            else {
                pt->i = i;
                pt->lit_type = litINTEGER;
                }
            }
        pt->s.eat(pc);
        return pt;
        }

    if (c == '"') { // String literal.
        cbuf.reset();
        cbuf.append(c);
        cbuf2.reset();
        long crossed_lines = 0;  // To delay the update of line_number.
        bool_enum midline = false;
        while (is->get(c)) {
            if (c == '\n') { // Line ends before end of string.
                crossed_lines += 1; // Don't bother to put back the '\n'.
                midline = true;
                break;
                }
            cbuf.append(c);
            if (c == '"')
                break;
            if (c == '\\') {
                if (!is->get(c))
                    break;
                cbuf.append(c);
                // (Note that a string crossing lines is never regarded as
                // have the linefeed token in the middle of it.)
                if (c == '\n') { // Continuation of string on next line.
                    crossed_lines += 1;
                    continue;
                    }
                switch(c) { // Imbedded bit patterns are not implemented.
                case '"':
                    c = '"';
                    break;
                case 'n':
                    c = '\n';
                    break;
                case 't':
                    c = '\t';
                    break;
                case 'v':
                    c = '\v';
                    break;
                case 'b':
                    c = '\b';
                    break;
                case 'r':
                    c = '\r';
                    break;
                case 'f':
                    c = '\f';
                    break;
                    } // End of switch(c).
                }
            cbuf2.append(c);
            }
        token* pt = new token(tokLIT);
        pt->line_number = line_number;
        line_number += crossed_lines;
        pt->s.eat(cbuf.copy0());
        if (midline || !is->good()) { // Line or file ended in middle of string!
            pt->lit_type = litERROR;
            return pt;
            }
        pt->pc = cbuf2.copy0();
        pt->lit_type = litSTRING;
        return pt;
        }

    // Probably a symbol:
    cbuf.reset();
    cbuf.append(c);
    sym_t st = symNULL;
    switch(c) {
    case '{':
        st = symLEFTBRACE;
        break;
    case '}':
        st = symRIGHTBRACE;
        break;
    case '(':
        st = symLEFTPAREN;
        break;
    case ')':
        st = symRIGHTPAREN;
        break;
    case '=':
        st = symEQUALS;
        break;
    case '+':
        st = symPLUS;
        break;
    case '-':
        st = symMINUS;
        break;
    case ';':
        st = symSEMICOLON;
        break;
    case ':':
        st = symCOLON;
        break;
    case '^':
        st = symHAT;
        break;
//    case '.':
//        st = symDOT;
//        break;
    case '\n':
        st = symLINEFEED;
        break;
    default:    // Syntax error.
        st = symERROR;
        break;
        } // End of switch(c).

    token* pt = new token(tokSYM);
    pt->line_number = line_number;
    pt->s.eat(cbuf.copy0());
    pt->sym_type = st;
    return pt;
    } // End of function token_stream::next_token.
