// src/aksl/str.h   2018-3-3   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_STR_H
#define AKSL_STR_H
/*------------------------------------------------------------------------------
Classes in this file:

c_string_rep::  A string class for private use by c_string.
c_string::      A storage-minimising string class.
c_stringlink::
c_stringlist::  A list of c_strings.
c_stringlistlink::
c_stringlistlist::  A list of lists of string.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Inline functions in this file:

operator=(char*&, c_string&)
operator<<(ostream&, c_string&).
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The guiding principles of the class c_string:
    - The difference between a null-string s = 0, and a zero-length string
      s = "" is maintained throughout.
    - The user is never to be trusted (except only sometimes, if they
      promise to put it back when they're finished with it).
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_NEWSTR_H
#include "aksl/newstr.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

// Forward references:
// struct skilist;
struct c_stringlist;

/*------------------------------------------------------------------------------
c_string_rep:: is a char*-style string, with a link-count field to keep track of
the number of links currently to the string. ("rep" means "representation".)
------------------------------------------------------------------------------*/
//----------------------//
//     c_string_rep::   //
//----------------------//
struct c_string_rep {
friend struct c_string;
private:
    char* s;                        // The actual string.
    long nlink;                     // The number of links to this string.

    // Read-only functions:
    c_string_rep* copy() { return new c_string_rep(s); }
    void cpy(const char* s2) {      // Analogous to ::strcpy().
        delete[] s;
        s = ::new_strcpy(s2);
        }
    void eat(char* s2) { delete[] s; s = s2; }
    char* new_strcpy() const { return ::new_strcpy(s); }
    char* new_strcpy(const char* s2) const { return ::new_strcpy(s, s2); }
    char* new_strcpy_nz() const { return ::new_strcpy_nz(s); }
    long length() const { return s ? strlen(s) : 0; }
    char element(long i) const { return s ? s[i] : 0; }
    bool_enum null() const { return (bool_enum)(!s); }
    bool_enum nullstr() const { return (bool_enum)(!s || !*s); }
    bool_enum notnullstr() const { return (bool_enum)(s && *s); }
    const char* nz(const char* s2) const { return (s && *s) ? s : s2; }
    void upper();
    void lower();
    int cmp(const c_string_rep* p) const {
        if (s)
            if (p->s)
                return strcmp(s, p->s);
            else
                return 1;
        else
            if (p->s)
                return -1;
            else
                return 0;
        }
    int cmp(const char* p) const {
        if (s)
            if (p)
                return strcmp(s, p);
            else
                return 1;
        else
            if (p)
                return -1;
            else
                return 0;
        }
    // Write functions:
    void clear() { delete[] s; s = 0; }
    void cat(const char* pc);
#if HAVE_SNPRINTF
    void cat(long x);
    void cat(unsigned long x);
#endif
    void cat(const c_string_rep* p);

    // Substitute p2 for p1 in string (or c2 for c1):
    void subst(const c_string_rep* p1, const c_string_rep* p2);
    void subst(char c1, char c2);

    // Non-standard constructors:
    c_string_rep(const char* s2) { s = ::new_strcpy(s2); nlink = 1; }

    c_string_rep() { s = 0; nlink = 1; }    // Assume immediate linkage!
    ~c_string_rep() { delete[] s; }
    }; // End of struct c_string_rep.

/*------------------------------------------------------------------------------
c_string:: is a storage-minimising character-string class. The pointer "rep"
is always non-null, and points to an actual c_string_rep. However, the
member "s" of "rep" is not necessarily non-null.
------------------------------------------------------------------------------*/
//----------------------//
//       c_string::     //
//----------------------//
struct c_string {
friend struct c_stringlink;
private:
    c_string_rep* rep;

    void detach() {             // Detach this c_string from its c_string_rep.
        if (rep->nlink > 1) {
            --rep->nlink;
            rep = rep->copy();
            }
        }
    void attach(const c_string* p) {
        if (p) {
            ++p->rep->nlink;
            if (--rep->nlink == 0)
                delete rep;
            rep = p->rep;
            }
        }
public:
    // Read-only functions:
    char* new_strcpy() const { return rep->new_strcpy(); }
    char* new_strcpy(const char* s) const { return rep->new_strcpy(s); }
    char* new_strcpy_nz() const { return rep->new_strcpy_nz(); }
    long length() const { return rep->length(); }
    char element(long i) const { return rep->element(i); }
    bool_enum null() const { return rep->null(); }
    bool_enum operator!() const { return null(); }
    bool_enum nullstr() const { return rep->nullstr(); }
    bool_enum notnullstr() const { return rep->notnullstr(); }
    const char* nz(const char* s2) const { return rep->nz(s2); }
    const char* read() const { return rep->s; }
    operator const char*() const { return read(); }
    // The following function may result in unintended memory allocation if
    // the compiler is not clever enough. E.g. ostream<<.
    operator char*() const { return ::new_strcpy(read()); }
    void lower() { rep->lower(); }
    void upper() { rep->upper(); }
    int cmp(const c_string& cs) const { return rep->cmp(cs.rep); }
    bool_enum operator==(const c_string& cs) const {
        return (bool_enum)(cmp(cs) == 0); }
    bool_enum operator!=(const c_string& cs) const {
        return (bool_enum)(cmp(cs) != 0); }
    int cmp(const char* s) const { return rep->cmp(s); }
    bool_enum operator==(const char* s) const
        { return (bool_enum)(cmp(s) == 0); }
    bool_enum operator!=(const char* s) const
        { return (bool_enum)(cmp(s) != 0); }

    // Write functions:
    void clear() { detach(); rep->clear(); }
    c_string& operator=(const char* s) { detach(); rep->cpy(s); return *this; }
    c_string& eat(char* s) { detach(); rep->eat(s); return *this; }
    c_string& cat(const c_string& cs)
        { if (!cs.nullstr()) { detach(); rep->cat(cs.rep); } return *this; }
    c_string& cat(const char* pc)
        { if (pc && *pc) { detach(); rep->cat(pc); } return *this; }
    c_string& operator+=(const c_string& cs) { return cat(cs); }
    c_string& operator<<(const c_string& cs) { return cat(cs); }
    c_string& operator+=(const char* pc) { return cat(pc); }
    c_string& operator<<(const char* pc) { return cat(pc); }
#if HAVE_SNPRINTF
    c_string& cat(long x) { detach(); rep->cat(x); return *this; }
    c_string& cat(unsigned long x) { detach(); rep->cat(x); return *this; }
    c_string& operator+=(long x) { return cat(x); }
    c_string& operator<<(long x) { return cat(x); }
    c_string& operator+=(unsigned long x) { return cat(x); }
    c_string& operator<<(unsigned long x) { return cat(x); }
#endif

    // Substitute string s2 for string s1:
    void subst(const c_string& s1, const c_string& s2) {
        char* pc = new_str_subst(rep->s, s1.rep->s, s2.rep->s);
        if (pc) { detach(); eat(pc); }
        }
    void subst(char c1, char c2);   // Substitute c1 with c2 in string.

    // Substitute strings in list sl1 with corresponding strings in list sl2:
    void subst(const c_stringlist& sl1, const c_stringlist& sl2);

    // Paragraph formatting functions:
//    void para_centre();     // Centre each line with initial spaces.
    int para_fill(int width = 80);  // Fill to margin. Return max line length.
//    void para_just();               // Justify both left and right.
//    void para_left();               // Left justify -- remove initial spaces.
//    void para_right();              // Right justify with initial spaces.
//    void para_squeeze();            // Squeeze out space.

    // I/O:
    void print_with_quotes(ostream& = cout) const;
    int stat_ext(const char*);

    // Non-standard constructors:
    c_string(const char* s) { rep = new c_string_rep(s); }

    // Standard create/destroy functions:
    c_string() { rep = new c_string_rep; }
    c_string(const c_string& cs2) { ++cs2.rep->nlink; rep = cs2.rep; }
    c_string& operator=(const c_string& cs2) { attach(&cs2); return *this; }
    ~c_string() { if (--rep->nlink == 0) delete rep; }
    }; // End of struct c_string.

//----------------------//
//    c_stringlink::    //
//----------------------//
struct c_stringlink: public c_string, public slink {

public:
    c_stringlink* next() const { return (c_stringlink*)slink::next(); }

    c_stringlink& operator=(const c_string& cs) { attach(&cs); return *this; }
    c_stringlink& operator=(const c_stringlink& cs)
        { attach(&cs); return *this; }
    c_stringlink() {}
    ~c_stringlink() {}
    }; // End of struct c_stringlink.

//----------------------//
//    c_stringlist::    //
//----------------------//
struct c_stringlist: private s2list {
public:
    using s2list::empty;
    using s2list::length;
    using s2list::position;
    c_stringlink* first() const { return (c_stringlink*)s2list::first(); }
    c_stringlink* last() const { return (c_stringlink*)s2list::last(); }
    c_stringlink* element(long i) const
        { return (c_stringlink*)s2list::element(i); }
    void append(c_stringlink* p) { s2list::append(p); }
    void prepend(c_stringlink* p) { s2list::prepend(p); }
    using s2list::insertafter;
    c_stringlink* popfirst() { return (c_stringlink*)s2list::popfirst(); }
    c_stringlink* poplast() { return (c_stringlink*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void clear() { for (c_stringlink* p = first(); p; )
        { c_stringlink* q = p->next(); delete p; p = q; } clearptrs(); }

    long count() const { return length(); }
    long index(const c_string&) const;
    void merge(const c_stringlist&);
    c_stringlink* find(const c_string&) const;
    void append(const c_string& s) {
        c_stringlink* q = new c_stringlink;
        *q = s;           // Why doesn't this work?
//        q->operator=(s);
        append(q);
        }
    void print(ostream&) const;
    void copy(const c_stringlist& x);

    c_stringlist() {}
    ~c_stringlist() { clear(); }
    }; // End of struct c_stringlist.

/*------------------------------------------------------------------------------
This represents a linked element of a list whose elements a string lists.
It is intended for use in struct c_stringlistlist.
------------------------------------------------------------------------------*/
//----------------------//
//  c_stringlistlink::  //
//----------------------//
struct c_stringlistlink: public c_stringlist, public slink {

public:
    c_stringlistlink* next() const { return (c_stringlistlink*)slink::next(); }

    c_stringlistlink& operator=(const c_stringlist& csl)
        { copy(csl); return *this; }
    c_stringlistlink& operator=(const c_stringlistlink& csl)
        { copy(csl); return *this; }
    c_stringlistlink() {}
    ~c_stringlistlink() {}
    }; // End of struct c_stringlistlink.

//----------------------//
//  c_stringlistlist::  //
//----------------------//
struct c_stringlistlist: private s2list {
public:
    using s2list::empty;
    using s2list::length;
    using s2list::position;
    c_stringlistlink* first() const
        { return (c_stringlistlink*)s2list::first(); }
    c_stringlistlink* last() const { return (c_stringlistlink*)s2list::last(); }
    c_stringlistlink* element(long i) const
        { return (c_stringlistlink*)s2list::element(i); }
    void append(c_stringlistlink* p) { s2list::append(p); }
    void prepend(c_stringlistlink* p) { s2list::prepend(p); }
    using s2list::insertafter;
    c_stringlistlink* popfirst()
        { return (c_stringlistlink*)s2list::popfirst(); }
    c_stringlistlink* poplast() { return (c_stringlistlink*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void clear() { for (c_stringlistlink* p = first(); p; )
        { c_stringlistlink* q = p->next(); delete p; p = q; } clearptrs(); }

    c_stringlistlist() {}
    ~c_stringlistlist() { clear(); }
    }; // End of struct c_stringlistlist.

/*------------------------------------------------------------------------------
operator=(char*&, c_string&) defines the following equality:
    c_string cs;
    char* pc = cs;
The string addressed by "pc" is not deleted.
------------------------------------------------------------------------------*/
//------------------------------//
// operator=(char*&, c_string&) //
//------------------------------//
/*------------------------------------------------------------------------------
inline char*& operator=(char*& pc, const c_string& cs) {
    return pc = ::new_strcpy(cs.read());
    } // End of function operator=(char*&, c_string&).
------------------------------------------------------------------------------*/

//----------------------------------//
//  operator<<(ostream&, c_string&) //
//----------------------------------//
inline ostream& operator<<(ostream& os, const c_string& cs) {
    return os << (cs.null() ? "" : cs.read());
    } // End of function operator<<(ostream&, c_string&).

extern ostream& operator<<(ostream& os, const c_stringlist& csl);
extern void time_interp(c_string& cs, struct tm* time);

#endif /* AKSL_STR_H */
