// src/aksl/list.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

s1list::
    ~s1list
    element
    length
    member
    position
    prepend
    popfirst
    remove
    insertafter
s2list::
    ~s2list
    element
    length
    member
    position
    append
    prepend
    popfirst
    poplast
    remove
    removeafter
    insertafter
    swallow
    gulp
s2nlist::
    swallow
    gulp
reflist::
    disjoint
------------------------------------------------------------------------------*/

#include "aksl/list.h"

//----------------------//
//   s1list::~s1list    //
//----------------------//
s1list::~s1list() {
    for (register slink* x = fst; x; ) {
        register slink* y = x->nxt;
        delete x;
        x = y;
        }
    } // End of function s1list::~s1list.

//----------------------//
//    s1list::element   //
//----------------------//
slink* s1list::element(register long i) const {
    if (i < 0)
        return 0;
    register long j = 0;
    register slink* x = 0;
    for (x = fst; x; x = x->nxt) {
        if (j == i)
            break;
        j += 1;
        }
    return x;
    } // End of function s1list::element.

//----------------------//
//    s1list::length    //
//----------------------//
long s1list::length() const {
    register unsigned long i = 0;
    for (register slink* x = fst; x; x = x->nxt)
        i += 1;
    return i;
    } // End of function s1list::length.

//----------------------//
//    s1list::member    //
//----------------------//
bool_enum s1list::member(register slink* c) const {
    if (!c)
        return false;
    register slink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p == c)
            break;
    return (bool_enum)(p != 0);
    } // End of function s1list::member.

//----------------------//
//   s1list::position   //
//----------------------//
long s1list::position(register slink* c) const {
    if (!c)
        return -1;
    register long x = 0;
    for (register slink* p = fst; p; p = p->nxt) {
        if (p == c)
            return x;
        x += 1;
        }
    return -1;
    } // End of function s1list::position.

//----------------------//
//    s1list::prepend   //
//----------------------//
void s1list::prepend(slink* x) {
    if (!x)
        return;
    x->nxt = fst;
    fst = x;
    } // End of function s1list::prepend.

//----------------------//
//   s1list::popfirst   //
//----------------------//
slink* s1list::popfirst() {
    if (!fst)
        return 0;
    slink* ret = fst;
    fst = fst->nxt;
    return ret;
    } // End of function s1list::popfirst.

//----------------------//
//    s1list::remove    //
//----------------------//
slink* s1list::remove(register slink* c) {
    if (!fst || !c)
        return 0;
    if (c == fst) {
        fst = c->nxt;
        return c;
        }
    register slink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p->nxt == c)
            break;
    if (!p)
        return 0;
    p->nxt = c->nxt;
    return c;
    } // End of function s1list::remove.

/*------------------------------------------------------------------------------
s1list::insertafter(x1, x2) inserts x2 after x1 in this list.
If x1 is zero, then x2 is inserted at the beginning of the list.
No check is made of whether x1 really is in the list.
------------------------------------------------------------------------------*/
//----------------------//
//  s1list::insertafter //
//----------------------//
void s1list::insertafter(slink* x1, slink* x2) {
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
    } // End of function s1list::insertafter.

//----------------------//
//    s2list::~s2list   //
//----------------------//
s2list::~s2list() {
    for (register slink* x = fst; x; ) {
        register slink* y = x->nxt;
        delete x;
        x = y;
        }
    } // End of function s2list::~s2list.

//----------------------//
//    s2list::element   //
//----------------------//
slink* s2list::element(register long i) const {
    if (i < 0)
        return 0;
    register long j = 0;
    register slink* x = 0;
    for (x = fst; x; x = x->nxt) {
        if (j == i)
            break;
        j += 1;
        }
    return x;
    } // End of function s2list::element.

//----------------------//
//    s2list::length    //
//----------------------//
long s2list::length() const {
    register unsigned long i = 0;
    for (register slink* x = fst; x; x = x->nxt)
        ++i;
    return i;
    } // End of function s2list::length.

//----------------------//
//    s2list::member    //
//----------------------//
bool_enum s2list::member(register slink* c) const {
    if (!c)
        return false;
    register slink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p == c)
            break;
    return (bool_enum)(p != 0);
    } // End of function s2list::member.

//----------------------//
//   s2list::position   //
//----------------------//
long s2list::position(register slink* c) const {
    if (!c)
        return -1;
    register long x = 0;
    for (register slink* p = fst; p; p = p->nxt) {
        if (p == c)
            return x;
        x += 1;
        }
    return -1;
    } // End of function s2list::position.

//----------------------//
//    s2list::append    //
//----------------------//
void s2list::append(slink* x) {
    if (!x)
        return;
    if (lst)
        lst->nxt = x;
    else
        fst = x;
    lst = x;
    x->nxt = 0;
    } // End of function s2list::append.

//----------------------//
//    s2list::prepend   //
//----------------------//
void s2list::prepend(slink* x) {
    if (!x)
        return;
    x->nxt = fst;
    fst = x;
    if (!lst)
        lst = fst;
    } // End of function s2list::prepend.

//----------------------//
//   s2list::popfirst   //
//----------------------//
slink* s2list::popfirst() {
    if (!fst)
        return 0;
    if (lst == fst)
        lst = 0;
    slink* ret = fst;
    fst = fst->nxt;
    return ret;
    } // End of function s2list::popfirst.

//----------------------//
//    s2list::poplast   //
//----------------------//
slink* s2list::poplast() {
    if (!fst)
        return 0;
    if (fst == lst) {
        slink* ret = fst;
        fst = lst = 0;
        return ret;
        }
    register slink* p = 0;
    for (p = fst; p; p = p->nxt)
        if (p->nxt == lst)
            break;
    if (!p)
        return 0; // This should never happen.
    p->nxt = 0;
    slink* ret = lst;
    lst = p;
    return ret;
    } // End of function s2list::poplast.

//----------------------//
//    s2list::remove    //
//----------------------//
slink* s2list::remove(register slink* p) {
    if (!fst || !p)
        return 0;
    if (fst == p) {
        if (lst == p)
            lst = 0;
        fst = p->nxt;
        return p;
        }
    register slink* q = 0;
    for (q = fst; q; q = q->nxt)
        if (q->nxt == p)
            break;
    if (!q)
        return 0; // This should never happen!!!
    q->nxt = p->nxt;
    if (lst == p)
        lst = q;
    return p;
    } // End of function s2list::remove.

/*------------------------------------------------------------------------------
This is the same as s2list::remove, except that the caller gives the
address of the previous item in the list as a hint to where the item is.
This is supposed to make the call more efficient by avoiding a linear search
of the list.
Warning: This function should only be called if you are certain that
x1 is the element of the list which is before x2.
------------------------------------------------------------------------------*/
//----------------------//
//  s2list::removeafter //
//----------------------//
slink* s2list::removeafter(slink* x1, slink* x2) {
    if (!fst || !x2)
        return 0;

    // Ignore the "x1" hint in the case of the first element of the list.
    if (fst == x2) {
        if (lst == x2)
            lst = 0;
        fst = x2->nxt;
        return x2;
        }

    // At this point, don't do a linear search. Just trust the hint!
    if (!x1)
        return 0; // This should not happen! This is a caller error.
    x1->nxt = x2->nxt;
    if (lst == x2)
        lst = x1;
    return x2;
    } // End of function s2list::removeafter.

/*------------------------------------------------------------------------------
s2list::insertafter(x1, x2) inserts x2 after x1 in this list.
If x1 is zero, then x2 is inserted at the beginning of the list.
No check is made of whether x1 really is in the list.
------------------------------------------------------------------------------*/
//----------------------//
//  s2list::insertafter //
//----------------------//
void s2list::insertafter(slink* x1, slink* x2) {
    if (!x2)
        return;
    if (!x1) { // Insert x2 at beginning of list:
        x2->nxt = fst;
        fst = x2;
        if (!lst)
            lst = x2;
        return;
        }
    x2->nxt = x1->nxt;
    x1->nxt = x2;
    if (lst == x1)
        lst = x2;
    } // End of function s2list::insertafter.

//----------------------//
//    s2list::swallow   //
//----------------------//
void s2list::swallow(s2list* pl) {
    if (!pl || pl == this)
        return;
    slink* p;
    while ((p = pl->popfirst()) != 0)
        append(p);
    } // End of function s2list::swallow.

//----------------------//
//     s2list::gulp     //
//----------------------//
void s2list::gulp(s2list* pl) {
    if (!pl || pl == this || pl->empty())
        return;
    if (empty()) {
        fst = pl->fst;
        lst = pl->lst;
        }
    else {
        lst->nxt = pl->fst;
        lst = pl->lst;
        }
    pl->fst = pl->lst = 0;
    } // End of function s2list::gulp.

/*------------------------------------------------------------------------------
The popfirst and append functions update the n_elements value correctly.
------------------------------------------------------------------------------*/
//----------------------//
//   s2nlist::swallow   //
//----------------------//
void s2nlist::swallow(s2nlist* pl) {
    if (!pl || pl == this)
        return;
    slink* p;
    while ((p = pl->popfirst()) != 0)
        append(p);
    } // End of function s2nlist::swallow.

//----------------------//
//    s2nlist::gulp     //
//----------------------//
void s2nlist::gulp(s2nlist* pl) {
    if (!pl || pl == this || pl->empty())
        return;
    if (empty()) {
        fst = pl->fst;
        lst = pl->lst;
        n_elements = pl->n_elements;
        }
    else {
        lst->nxt = pl->fst;
        lst = pl->lst;
        n_elements += pl->n_elements;
        }
    pl->clearptrs();
    } // End of function s2nlist::gulp.

//----------------------//
//   reflist::disjoint  //
//----------------------//
bool_enum reflist::disjoint(reflist& z) const {
    Forall(register ref, c1, *this) {
        Forall(register ref, c2, z)
            if (c1->value() == c2->value())
                return false;
        }
    return true;
    } // End of function reflist::disjoint.
