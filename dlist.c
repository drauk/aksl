// src/aksl/dlist.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/dlist.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
WARNING:  Do not use this module. Work in progress!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Functions in this file:

dz1list::
    ~dz1list
    element
    length
    member
    position
    prepend
    popfirst
    remove
    insertafter
dl1list::
    ~dl1list
    element
    length
    member
    position
    prepend
    popfirst
    remove
    insertafter
------------------------------------------------------------------------------*/

#include "aksl/dlist.h"

//----------------------//
//  dz1list::~dz1list   //
//----------------------//
dz1list::~dz1list() {
    for (register dlink* x = fst; x; ) {
        register dlink* y = x->nxt;
        delete x;
        x = y;
        }
    } // End of function dz1list::~dz1list.

//----------------------//
//   dz1list::element   //
//----------------------//
dlink* dz1list::element(register long i) const {
    if (i < 0)
        return 0;
    register long j = 0;
    register dlink* x = 0;
    for (x = fst; x; x = x->nxt) {
        if (j == i)
            break;
        j += 1;
        }
    return x;
    } // End of function dz1list::element.

//----------------------//
//    dz1list::length   //
//----------------------//
long dz1list::length() const {
    register long i = 0;
    for (register dlink* x = fst; x; x = x->nxt)
        i += 1;
    return i;
    } // End of function dz1list::length.

//----------------------//
//    dz1list::member   //
//----------------------//
bool_enum dz1list::member(register dlink* c) const {
    if (!c)
        return false;
    register dlink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p == c)
            break;
    return (bool_enum)(p != 0);
    } // End of function dz1list::member.

//----------------------//
//   dz1list::position  //
//----------------------//
long dz1list::position(register dlink* c) const {
    if (!c)
        return -1;
    register long x = 0;
    for (register dlink* p = fst; p; p = p->nxt) {
        if (p == c)
            return x;
        x += 1;
        }
    return -1;
    } // End of function dz1list::position.

//----------------------//
//   dz1list::prepend   //
//----------------------//
void dz1list::prepend(dlink* x) {
    // Ignore null item.
    if (!x)
        return;

    // Modify the incoming item.
    x->prv = 0;
    x->nxt = fst;

    // Modify first item of current list.
    if (fst)
        fst->prv = x;

    // Move the "first" pointer to the new item.
    fst = x;
    } // End of function dz1list::prepend.

/*------------------------------------------------------------------------------
Warning: Currently upgrading this function from s1list to dz1list.
------------------------------------------------------------------------------*/
//----------------------//
//   dz1list::popfirst  //
//----------------------//
dlink* dz1list::popfirst() {
    if (!fst)
        return 0;
    dlink* ret = fst;
    fst = fst->nxt;
    return ret;
    } // End of function dz1list::popfirst.

//----------------------//
//    dz1list::remove   //
//----------------------//
dlink* dz1list::remove(register dlink* c) {
    if (!fst || !c)
        return 0;
    if (c == fst) {
        fst = c->nxt;
        return c;
        }
    register dlink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p->nxt == c)
            break;
    if (!p)
        return 0;
    p->nxt = c->nxt;
    return c;
    } // End of function dz1list::remove.

/*------------------------------------------------------------------------------
dz1list::insertafter(x1, x2) inserts x2 after x1 in this list.
If x1 is zero, then x2 is inserted at the beginning of the list.
No check is made of whether x1 really is in the list.
------------------------------------------------------------------------------*/
//----------------------//
// dz1list::insertafter //
//----------------------//
void dz1list::insertafter(dlink* x1, dlink* x2) {
    if (!x2)
        return;
    if (!x1) { // Insert x2 at beginning of list:
        x2->nxt = fst;
        fst = x2;
        }
    else {
        x2->nxt = x1->nxt;
        x1->nxt = x2;
        }
    } // End of function dz1list::insertafter.

//----------------------//
//  dl1list::~dl1list   //
//----------------------//
dl1list::~dl1list() {
    for (register dlink* x = fst; x; ) {
        register dlink* y = x->nxt;
        delete x;
        x = y;
        }
    } // End of function dl1list::~dl1list.

//----------------------//
//   dl1list::element   //
//----------------------//
dlink* dl1list::element(register long i) const {
    if (i < 0)
        return 0;
    register long j = 0;
    register dlink* x = 0;
    for (x = fst; x; x = x->nxt) {
        if (j == i)
            break;
        j += 1;
        }
    return x;
    } // End of function dl1list::element.

//----------------------//
//    dl1list::length   //
//----------------------//
long dl1list::length() const {
    register long i = 0;
    for (register dlink* x = fst; x; x = x->nxt)
        i += 1;
    return i;
    } // End of function dl1list::length.

//----------------------//
//    dl1list::member   //
//----------------------//
bool_enum dl1list::member(register dlink* c) const {
    if (!c)
        return false;
    register dlink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p == c)
            break;
    return (bool_enum)(p != 0);
    } // End of function dl1list::member.

//----------------------//
//   dl1list::position  //
//----------------------//
long dl1list::position(register dlink* c) const {
    if (!c)
        return -1;
    register long x = 0;
    for (register dlink* p = fst; p; p = p->nxt) {
        if (p == c)
            return x;
        x += 1;
        }
    return -1;
    } // End of function dl1list::position.

/*------------------------------------------------------------------------------
Warning: Currently upgrading this function from s1list to dl1list.
------------------------------------------------------------------------------*/
//----------------------//
//   dl1list::prepend   //
//----------------------//
void dl1list::prepend(dlink* x) {
    // Ignore null item.
    if (!x)
        return;

    // Modify the incoming item.
    x->prv = 0;
    x->nxt = fst;

    // Modify first item of current list.
    if (fst)
        fst->prv = x;

    // Move the "first" pointer to the new item.
    fst = x;
    } // End of function dl1list::prepend.

//----------------------//
//   dl1list::popfirst  //
//----------------------//
dlink* dl1list::popfirst() {
    if (!fst)
        return 0;
    dlink* ret = fst;
    fst = fst->nxt;
    return ret;
    } // End of function dl1list::popfirst.

//----------------------//
//    dl1list::remove   //
//----------------------//
dlink* dl1list::remove(register dlink* c) {
    if (!fst || !c)
        return 0;
    if (c == fst) {
        fst = c->nxt;
        return c;
        }
    register dlink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p->nxt == c)
            break;
    if (!p)
        return 0;
    p->nxt = c->nxt;
    return c;
    } // End of function dl1list::remove.

/*------------------------------------------------------------------------------
dl1list::insertafter(x1, x2) inserts x2 after x1 in this list.
If x1 is zero, then x2 is inserted at the beginning of the list.
No check is made of whether x1 really is in the list.
------------------------------------------------------------------------------*/
//----------------------//
// dl1list::insertafter //
//----------------------//
void dl1list::insertafter(dlink* x1, dlink* x2) {
    if (!x2)
        return;
    if (!x1) { // Insert x2 at beginning of list:
        x2->nxt = fst;
        fst = x2;
        }
    else {
        x2->nxt = x1->nxt;
        x1->nxt = x2;
        }
    } // End of function dl1list::insertafter.
