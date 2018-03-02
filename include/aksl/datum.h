// src/aksl/datum.h   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/include/aksl/datum.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_DATUM_H
#define AKSL_DATUM_H
/*------------------------------------------------------------------------------
Classes declared in this file:

datum::
datumlist::
datumrefbuf::
------------------------------------------------------------------------------*/

#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_ERROR_H
#include "aksl/error.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// Forward reference to class "value":
struct value;

/*------------------------------------------------------------------------------
A "datum" is intended to be used as a base class for PDU classes.
An experimental recv_message() member has been added, to carry out various
actions in packages where there is no knowledge of how they are to be carried
out, nor what data members the derived class may have.
The "short" parameter is meant to be the _global_ message type, as translated by
the sending object via a loc2glob table, and the derived datum class is supposed
to look this up in its own glob2loc table (somehow....).
------------------------------------------------------------------------------*/
//----------------------//
//        datum::       //
//----------------------//
struct datum: public slink {
public:
    datum* next() const { return (datum*)slink::next(); }
    virtual const char* type() { return "undefined_datum_type"; }
    virtual int itype() { return -1; } // Faster integer type function.
    virtual value* recv_message(mtype, value*) { return 0; }
    virtual void print(ostream& = cout) {}

    virtual ~datum() {}
    }; // End of struct datum.

//----------------------//
//      datumlist::     //
//----------------------//
struct datumlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    datum* first() const { return (datum*)s2list::first(); }
    datum* last() const { return (datum*)s2list::last(); }
    datum* element(long i) const { return (datum*)s2list::element(i); }
    void append(datum* p) { s2list::append(p); }
    void prepend(datum* p) { s2list::prepend(p); }
    datum* popfirst() { return (datum*)s2list::popfirst(); }
    datum* poplast() { return (datum*)s2list::poplast(); }
    datum* remove(datum* p) { return (datum*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(datum* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(datumlist& l) { s2list::swallow(&l); }
    void clear() { for (datum* p = first(); p; )
        { datum* q = p->next(); delete p; p = q; } clearptrs(); }

    datumlist() {}
    ~datumlist() { clear(); }
    }; // End of struct datumlist.

/*------------------------------------------------------------------------------
This is a class to quickly handle queuing of pointers to datums.
The pointers should always be left in the ranges:
base <= pptr <  eptr
base <  gptr <= eptr
This simplifies testing of wrap-around simultaneously with fullness or
emptiness.
NOTE: NOTE: This class should be derived from a more general refbuf class!!!!!!!
------------------------------------------------------------------------------*/
//----------------------//
//     datumrefbuf::    //
//----------------------//
struct datumrefbuf {
private:
    long siz;        // The number of datum pointers that can be accepted.
    datum** base;
    datum** eptr;
    datum** pptr;
    datum** gptr;
public:
    void size(long n) {
        if (n >= 0) {
            delete[] base;
            siz = n;
            base = new datum*[n + 1];
            eptr = base + n + 1;
            pptr = base;
            gptr = eptr;
            }
        }
    bool_enum empty() const {
        return (bool_enum)((pptr == gptr) || (pptr == base) && (gptr == eptr));
        }
    bool_enum full() const { return (bool_enum)(pptr + 1 == gptr); }

    void put(datum* p) { // If buffer is not full, accept the pointer.
        *pptr++ = p;
        if (pptr == gptr) --pptr;
        if (pptr >= eptr) pptr = base;
        }
    void pushback(datum* p) { // If buffer not full, push an item back.
        *--gptr = p;
        if (pptr == gptr) ++gptr;
        if (gptr == base) gptr = eptr;
        }
    datum* get() {                                   // Dequeue an item.
        if (gptr >= eptr) gptr = base;
        return (gptr == pptr) ? (gptr = eptr, (datum*)0) : *gptr++;
        }
    datum* read() { // Read current item.
        return (gptr == eptr) ?
            ((pptr == base) ? 0 : *base) : ((pptr == gptr) ? 0 : *gptr) ;
        }
    void print() {}     // Ad hoc printing routine.

    datumrefbuf() { siz = 0; base = pptr = gptr = eptr = 0; }
    ~datumrefbuf() { delete[] base; }
    }; // End of struct datumrefbuf.

#endif /* AKSL_DATUM_H */
