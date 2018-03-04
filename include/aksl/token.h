// src/aksl/token.h   2018-3-3   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_TOKEN_H
#define AKSL_TOKEN_H
/*------------------------------------------------------------------------------
Classes in this file:

token::
tokenlist::
token_error::
token_errorlist::
token_stream::
------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Modifications required:

-   Should replace the line number values by a line number +
    file name structure. This would take more space, and some sort of
    clever trick is necessary to not use more space than at present.
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_STR_H
#include "aksl/str.h"
#endif
#ifndef AKSL_CHARBUF_H
#include "aksl/charbuf.h"
#endif

// System header files:
#ifndef AKSL_X_FSTREAM_H
#define AKSL_X_FSTREAM_H
#include <fstream.h>
#endif

enum token_t { // Token type.
    tokNULL,
    tokID,
    tokSYM,
    tokLIT,
    tokERROR
    };

enum ident_t { // Identifier type. Not determined by the token_stream.
    idNULL,
    idKEYWORD,
    idCLASSNAME,
    idOBJECTNAME,
    idGLOBVARNAME,
    idATTRNAME,
    idERROR
    };

enum sym_t { // Symbol [operator/separator] type.
    symNULL,
    symLEFTBRACE,                   // {
    symRIGHTBRACE,                  // }
    symLEFTPAREN,                   // (
    symRIGHTPAREN,                  // )
    symEQUALS,                      // =
    symPLUS,                        // +
    symMINUS,                       // -
    symSEMICOLON,                   // ;
    symCOLON,                       // :
    symHAT,                         // ^
    symDOT,                         // .
    symLINEFEED,                    // '\n' in case of eoln recognition.
    symERROR
    };

enum lit_t { // Literal type.
    litNULL,
    litINTEGER,                     // Never negative.
    litREAL,                        // Never negative.
    litSTRING,
    litERROR
    };

//----------------------//
//        token::       //
//----------------------//
struct token: public slink {
    long line_number;
    token_t type;
    c_string s;
    union { // The sub-type, if type != tokNULL:
        ident_t id_type;
        lit_t   lit_type;
        sym_t   sym_type;
        };
    union { // The value of the token, if it is a literal:
        unsigned long i;
        double r;
        char* pc;
        };

    long line() { return line_number; }

    bool_enum Identifier() { return (bool_enum)(type == tokID); }
    bool_enum Literal() { return (bool_enum)(type == tokLIT); }
    bool_enum Symbol() { return (bool_enum)(type == tokSYM); }

    bool_enum Integer()
        { return (bool_enum)(type == tokLIT && lit_type == litINTEGER); }
    bool_enum Real()
        { return (bool_enum)(type == tokLIT && lit_type == litREAL); }
    bool_enum Number()
        { return (bool_enum)(type == tokLIT
            && (lit_type == litINTEGER || lit_type == litREAL)); }
    bool_enum String()
        { return (bool_enum)(type == tokLIT && lit_type == litSTRING); }

    bool_enum Leftbrace()
        { return (bool_enum)(type == tokSYM && sym_type == symLEFTBRACE); }
    bool_enum Rightbrace()
        { return (bool_enum)(type == tokSYM && sym_type == symRIGHTBRACE); }
    bool_enum Leftparen()
        { return (bool_enum)(type == tokSYM && sym_type == symLEFTPAREN); }
    bool_enum Rightparen()
        { return (bool_enum)(type == tokSYM && sym_type == symRIGHTPAREN); }
    bool_enum Equals()
        { return (bool_enum)(type == tokSYM && sym_type == symEQUALS); }
    bool_enum Plus()
        { return (bool_enum)(type == tokSYM && sym_type == symPLUS); }
    bool_enum Minus()
        { return (bool_enum)(type == tokSYM && sym_type == symMINUS); }
    bool_enum Semicolon()
        { return (bool_enum)(type == tokSYM && sym_type == symSEMICOLON); }
    bool_enum Colon()
        { return (bool_enum)(type == tokSYM && sym_type == symCOLON); }
    bool_enum Hat()
        { return (bool_enum)(type == tokSYM && sym_type == symHAT); }
    bool_enum Dot()
        { return (bool_enum)(type == tokSYM && sym_type == symDOT); }
    bool_enum Linefeed()
        { return (bool_enum)(type == tokSYM && sym_type == symLINEFEED); }

    void print(ostream& = cout);

    token* next() { return (token*)slink::next(); }

//    token& operator=(const token& x) {}
//    token(const token& x) {};
    token() { type = tokNULL; line_number = 0; }
    token(token_t);
    ~token() { if (String()) delete[] pc; }
    }; // End of struct token.

//----------------------//
//       tokenlist::    //
//----------------------//
struct tokenlist: private s2list {
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    token* first() const { return (token*)s2list::first(); }
    token* last() const { return (token*)s2list::last(); }
    void append(token* p) { s2list::append(p); }
    void prepend(token* p) { s2list::prepend(p); }
    token* popfirst() { return (token*)s2list::popfirst(); }
    token* poplast() { return (token*)s2list::poplast(); }
    token* remove(token* p)
        { return (token*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(token* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(tokenlist& l) { s2list::swallow(&l); }
    void clear() { for (token* p = first(); p; )
        { token* q = p->next(); delete p; p = q; } clearptrs(); }

//    tokenlist& operator=(const tokenlist& x) {}
//    tokenlist(const tokenlist& x) {};
    tokenlist() {}
    ~tokenlist() { clear(); }
    }; // End of struct tokenlist.

/*------------------------------------------------------------------------------
This class is used to return lists of errors found while tokenising a file.
------------------------------------------------------------------------------*/
//----------------------//
//     token_error::    //
//----------------------//
struct token_error: public slink {
    long line_number;
    c_string message;

    token_error* next() { return (token_error*)slink::next(); }

//    token_error& operator=(const token_error& x) {}
//    token_error(const token_error& x) {};
    token_error() { line_number = 0; }
    ~token_error() {}
    }; // End of struct token_error.

//----------------------//
//   token_errorlist::  //
//----------------------//
struct token_errorlist: private s2list {
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    token_error* first() const { return (token_error*)s2list::first(); }
    token_error* last() const { return (token_error*)s2list::last(); }
    void append(token_error* p) { s2list::append(p); }
    void prepend(token_error* p) { s2list::prepend(p); }
    token_error* popfirst() { return (token_error*)s2list::popfirst(); }
    token_error* poplast() { return (token_error*)s2list::poplast(); }
    token_error* remove(token_error* p)
        { return (token_error*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(token_error* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(token_errorlist& l) { s2list::swallow(&l); }
    void clear() { for (token_error* p = first(); p; )
        { token_error* q = p->next(); delete p; p = q; } clearptrs(); }

//    token_errorlist& operator=(const token_errorlist& x) {}
//    token_errorlist(const token_errorlist& x) {};
    token_errorlist() {}
    ~token_errorlist() { clear(); }
    }; // End of struct token_errorlist.

/*------------------------------------------------------------------------------
This class is supposed to be attached to an input stream. Then a stream of
tokens can be extracted from the file, according to the lexical conventions of
the ORAL data langauge. ORAL = Object Relationship Attribute Language.

Tokens are returned one at a time rather than as a complete list representing
the whole file because it may be necessary to change the lexical rules on the
basis of previously interpreted commands. If the lexical rules do change during
interpretation, then this should be recorded in members of this class. In the
absence of lexical rule changes, it would be possible to offer a member function
"read_tokens" which reads all of the tokens in the file into the member
"tokens".
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NOTE:
It seems that if the istream "is" is created in the open() call, then it is
not closed and deleted when the token_stream is closed or goes out of scope.
This is very bad, because it wastes memory and I/O handles etc.
This must be fixed!!!!!
------------------------------------------------------------------------------*/
//----------------------//
//     token_stream::   //
//----------------------//
struct token_stream {
public:             // Temporarily public.
    istream* is;
private:
    filebuf fb;
    long line_number;
    bool_enum already_open;
    tokenlist putback_tokens;
    bool_enum eof_found;  // Set to true if next_token() returns 0.
public:
    tokenlist tokens;
    token_errorlist token_errors;
    bool_enum linefeed_is_token;        // For the user to control. [false]

    bool_enum open(const char* = 0);    // Default stream is stdin.
    token* next_token();
    token_stream& operator>>(token*& t) { t = next_token(); return *this; }
//    token_stream& operator>>(token*& t)
//        { t = next_token(); ttprint(t); return *this; }
    void ttprint(token* t) {
        cout << "Token trace: ";
        if (t) {
            t->print(cout);
            cout << endl;
            }
        else
            cout << "<>\n";
        }
    void putback(token* pt)
        { if (already_open) { putback_tokens.prepend(pt); eof_found = false; }}
    operator void*() { return eof_found ? 0 : this; }
    int operator!() { return eof_found; }
    long line() { return line_number; }

//    token_stream& operator=(const token_stream& x) {}
//    token_stream(const token_stream& x) {};
    token_stream(istream& i, long l = 0);
//    token_stream(istream& i, long l = 0) {
//        line_number = l;
//        is = &i;
//        already_open = (bool_enum)(is && *is);
//        eof_found = false;
//        };
    token_stream() { line_number = 0; already_open = false; eof_found = false;
                     linefeed_is_token = false; }
    ~token_stream() {}
    }; // End of struct token_stream.

#endif /* AKSL_TOKEN_H */
