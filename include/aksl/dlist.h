// src/aksl/dlist.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/dlist.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_DLIST_H
#define AKSL_DLIST_H
/*------------------------------------------------------------------------------
WARNING:  Do not use this module. Work in progress!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes in this file:

dlink::
dz1list::
dl1list::
------------------------------------------------------------------------------*/

#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// List traversal macros:
#ifndef forall
#define forall(p, list) \
    for ((p) = (list).first(); (p); (p) = (p)->next())
#endif
#ifndef Forall
#define Forall(type, p, list) \
    for (type* (p) = (list).first(); (p); (p) = (p)->next())
#endif

/*------------------------------------------------------------------------------
The doubly linked list classes are work in progress.
Please do not use these classes yet.
------------------------------------------------------------------------------*/
//----------------------//
//       dlink::        //
//----------------------//
struct dlink { // A doubly linked item.
friend struct dz1list;
friend struct dl1list;
friend struct dz2list;
friend struct dl2list;
private:
    dlink* nxt;
    dlink* prv;
public:
    dlink* next() const { return nxt; }
    dlink* prev() const { return prv; }

    dlink(dlink* n = 0, dlink* p = 0) { nxt = n; prv = p; }
    }; // End of struct dlink.

/*------------------------------------------------------------------------------
Note that unlike the typical Unix kernel lists which are doubly looped so that
the first and last items point to each other, _this_ doubly linked list
implementation makes the first and last elements point to zero.
------------------------------------------------------------------------------*/
//----------------------//
//       dz1list::      //
//----------------------//
struct dz1list { // A list with only 1 pointer: to the first element.
private:
    dlink* fst;
protected:
    void clearptrs() { fst = 0; } // To be used by clear().
public:
    bool_enum empty() const { return (bool_enum)(fst == 0); }
    dlink* first() const { return fst; }
    dlink* element(register long i) const;
    long length() const;
    bool_enum member(register dlink*) const; // Returns true if in the list.
    long position(register dlink*) const;    // Position of object in the list.
    void prepend(dlink* x);
    dlink* popfirst();
    dlink* remove(register dlink*);
    void delfirst() { delete popfirst(); }
    void delremove(dlink* c) { delete remove(c); }
    void insertafter(dlink* x1, dlink* x2); // Insert x2 after x1.
    void clear() {
        for (register dlink* x = fst; x; )
            { register dlink* y = x->nxt; delete x; x = y; }
        clearptrs();
        }
    dz1list() { fst = 0; }
    ~dz1list();
    }; // End of struct dz1list.

/*------------------------------------------------------------------------------
Like the typical Unix kernel list implementations, this list class is doubly
looped so that the first and last items point to each other.
------------------------------------------------------------------------------*/
//----------------------//
//       dl1list::      //
//----------------------//
struct dl1list { // A list with only 1 pointer: to the first element.
private:
    dlink* fst;
protected:
    void clearptrs() { fst = 0; } // To be used by clear().
public:
    bool_enum empty() const { return (bool_enum)(fst == 0); }
    dlink* first() const { return fst; }
    dlink* element(register long i) const;
    long length() const;
    bool_enum member(register dlink*) const; // Returns true if in the list.
    long position(register dlink*) const;    // Position of object in the list.
    void prepend(dlink* x);
    dlink* popfirst();
    dlink* remove(register dlink*);
    void delfirst() { delete popfirst(); }
    void delremove(dlink* c) { delete remove(c); }
    void insertafter(dlink* x1, dlink* x2); // Insert x2 after x1.
    void clear() {
        for (register dlink* x = fst; x; )
            { register dlink* y = x->nxt; delete x; x = y; }
        clearptrs();
        }
    dl1list() { fst = 0; }
    ~dl1list();
    }; // End of struct dl1list.

#endif /* AKSL_DLIST_H */
