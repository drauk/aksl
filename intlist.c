// src/aksl/intlist.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

intlist::
    find
    add
intkeylist::
    insert_inc
    insert_dec
    find_data
    find_key
------------------------------------------------------------------------------*/

#include "aksl/intlist.h"

//----------------------//
//     intlist::find    //
//----------------------//
intlink* intlist::find(long i) const {
    intlink* p = 0;
    forall(p, *this)
        if (p->i == i)
            break;
    return p;
    } // End of function intlist::find.

/*------------------------------------------------------------------------------
intlist::add() is a sorting list insert.
------------------------------------------------------------------------------*/
//----------------------//
//     intlist::add     //
//----------------------//
void intlist::add(long i) {
    intlink* x = new intlink(i);
    intlink* q = 0;
    Forall(intlink, p, *this) {
        if (p->i > i)
            break;
        q = p;
        }
    insertafter(q, x);
    } // End of function intlist::add.

/*------------------------------------------------------------------------------
This function will over-write any previous value for the given key.
This function maintains increasing order by key value.
------------------------------------------------------------------------------*/
//--------------------------//
//  intkeylist::insert_inc  //
//--------------------------//
intkey* intkeylist::insert_inc(long data, long key) {
    intkey *p = 0, *q = 0;
    forall(p, *this) {
        if (p->key >= key)
            break;
        q = p;
        }
    if (p && p->key == key) {
        p->data = data;
        return p;
        }
    p = new intkey(data, key);
    insertafter(q, p);
    return p;
    } // End of function intkeylist::insert_inc.

/*------------------------------------------------------------------------------
This function will over-write any previous value for the given key.
This function maintains decreasing order by key value.
------------------------------------------------------------------------------*/
//--------------------------//
//  intkeylist::insert_dec  //
//--------------------------//
intkey* intkeylist::insert_dec(long data, long key) {
    intkey *p = 0, *q = 0;
    forall(p, *this) {
        if (p->key <= key)
            break;
        q = p;
        }
    if (p && p->key == key) {
        p->data = data;
        return p;
        }
    p = new intkey(data, key);
    insertafter(q, p);
    return p;
    } // End of function intkeylist::insert_dec.

//--------------------------//
//  intkeylist::find_data   //
//--------------------------//
intkey* intkeylist::find_data(long& data, long key) {
    intkey* p = 0;
    forall(p, *this)
        if (p->key == key)
            break;
    if (p)
        data = p->data;
    return p;
    } // End of function intkeylist::find_data.

//--------------------------//
//   intkeylist::find_key   //
//--------------------------//
intkey* intkeylist::find_key(long data, long& key) {
    intkey* p = 0;
    forall(p, *this)
        if (p->data == data)
            break;
    if (p)
        key = p->key;
    return p;
    } // End of function intkeylist::find_key.
