// src/aksl/vplist.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

voidptrlist::
    find
    add
    add_sort
voidptrlist_b::
    find
    add
    add_sort
voidptrkeylist::
    insert
    find_data
    find_data
    find_key
    remove_key
voidptrstrlist::
    insert
    find_data
    find_key
------------------------------------------------------------------------------*/

#include "aksl/vplist.h"

// Block memory allocation for class "voidptr_b".
// Configure it to use malloc() to avoid infinite recursion in newstat.[ch].
bmem_define_malloc(voidptr_b, bmem0);
// bmem voidptr_b::bmem0(sizeof(voidptr_b), msMALLOC);

//----------------------//
//   voidptrlist::find  //
//----------------------//
voidptr* voidptrlist::find(void* i) const {
    voidptr* p = 0;
    forall(p, *this)
        if (p->i == i)
            break;
    return p;
    } // End of function voidptrlist::find.

/*------------------------------------------------------------------------------
voidptrlist::add() is a non-sorting list append.
If the pointer is already there, don't add a duplicate!
------------------------------------------------------------------------------*/
//----------------------//
//   voidptrlist::add   //
//----------------------//
void voidptrlist::add(void* i) {
    Forall(voidptr, p, *this)
        if (p->i == i)
            return;
    voidptr* x = new voidptr(i);
    append(x);
    } // End of function voidptrlist::add.

/*------------------------------------------------------------------------------
voidptrlist::add_sort() is a sorting list insert.
If the pointer is already there, don't add a duplicate!
------------------------------------------------------------------------------*/
//----------------------//
// voidptrlist::add_sort//
//----------------------//
void voidptrlist::add_sort(void* i) {
    voidptr* q = 0;
    Forall(voidptr, p, *this) {
        if ((unsigned long)(p->i) > (unsigned long)i)
            break;
        if (p->i == i)
            return;
        q = p;
        }
    voidptr* x = new voidptr(i);
    insertafter(q, x);
    } // End of function voidptrlist::add_sort.

//----------------------//
//  voidptr_blist::find //
//----------------------//
voidptr_b* voidptr_blist::find(void* i) const {
    voidptr_b* p = 0;
    forall(p, *this)
        if (p->i == i)
            break;
    return p;
    } // End of function voidptr_blist::find.

/*------------------------------------------------------------------------------
Remove one copy of "i" only.
If no reference to "i" is found, then null is returned.
If there _is_ a pointer to "i", then the referring voidptr_b object is returned.
------------------------------------------------------------------------------*/
//----------------------//
// voidptr_blist::remove//
//----------------------//
voidptr_b* voidptr_blist::remove(void* i) {
    register voidptr_b* p = 0;
    register voidptr_b* q = 0;
    forall(p, *this) {
        if (p->i == i)
            break;
        q = p;
        }
    if (p)
        return removeafter(q, p);
    return 0;
    } // End of function voidptr_blist::remove.

/*------------------------------------------------------------------------------
voidptr_blist::add() is a non-sorting list append.
If the pointer is already there, don't add a duplicate!
------------------------------------------------------------------------------*/
//----------------------//
//  voidptr_blist::add  //
//----------------------//
void voidptr_blist::add(void* i) {
    Forall(voidptr_b, p, *this)
        if (p->i == i)
            return;
    voidptr_b* x = new voidptr_b(i);
    append(x);
    } // End of function voidptr_blist::add.

/*------------------------------------------------------------------------------
voidptr_blist::add_sort() is a sorting list insert.
If the pointer is already there, don't add a duplicate!
------------------------------------------------------------------------------*/
//--------------------------//
//  voidptr_blist::add_sort //
//--------------------------//
void voidptr_blist::add_sort(void* i) {
    voidptr_b* q = 0;
    Forall(voidptr_b, p, *this) {
        if ((unsigned long)(p->i) > (unsigned long)i)
            break;
        if (p->i == i)
            return;
        q = p;
        }
    voidptr_b* x = new voidptr_b(i);
    insertafter(q, x);
    } // End of function voidptr_blist::add_sort.

/*------------------------------------------------------------------------------
This function will over-write any previous value for the given key.
------------------------------------------------------------------------------*/
//--------------------------//
//  voidptrkeylist::insert  //
//--------------------------//
voidptrkey* voidptrkeylist::insert(void* data, long key) {
    voidptrkey *p = 0, *q = 0;
    forall(p, *this) {
        if (p->key <= key)
            break;
        q = p;
        }
    if (p && p->key == key) {
        p->data = data;
        return p;
        }
    p = new voidptrkey(data, key);
    insertafter(q, p);
    return p;
    } // End of function voidptrkeylist::insert.

/*------------------------------------------------------------------------------
Find an entry in the table, given the key.
------------------------------------------------------------------------------*/
//------------------------------//
//   voidptrkeylist::find_data  //
//------------------------------//
voidptrkey* voidptrkeylist::find_data(long key) {
    voidptrkey* p = 0;
    forall(p, *this)
        if (p->key == key)
            break;
    return p;
    } // End of function voidptrkeylist::find_data.

/*------------------------------------------------------------------------------
Find an entry in the table, given the key, and return the data too.
------------------------------------------------------------------------------*/
//------------------------------//
//   voidptrkeylist::find_data  //
//------------------------------//
voidptrkey* voidptrkeylist::find_data(void*& data, long key) {
    voidptrkey* p = 0;
    forall(p, *this)
        if (p->key == key)
            break;
    if (p)
        data = p->data;
    return p;
    } // End of function voidptrkeylist::find_data.

//--------------------------//
// voidptrkeylist::find_key //
//--------------------------//
voidptrkey* voidptrkeylist::find_key(void* data, long& key) {
    voidptrkey* p = 0;
    forall(p, *this)
        if (p->data == data)
            break;
    if (p)
        key = p->key;
    return p;
    } // End of function voidptrkeylist::find_key.

/*------------------------------------------------------------------------------
Remove an entry in the table, given the key.
Return the key to the caller for deletion if desired.
------------------------------------------------------------------------------*/
//------------------------------//
//  voidptrkeylist::remove_key  //
//------------------------------//
voidptrkey* voidptrkeylist::remove_key(long key) {
    voidptrkey* p = 0;
    voidptrkey* q = 0;
    forall(p, *this) {
        if (p->key == key)
            break;
        q = p;
        }
    return removeafter(q, p);
    } // End of function voidptrkeylist::remove_key.

/*------------------------------------------------------------------------------
This function will over-write any previous value for the given key.
------------------------------------------------------------------------------*/
//--------------------------//
//  voidptrstrlist::insert  //
//--------------------------//
voidptrstr* voidptrstrlist::insert(void* data, const char* key) {
    voidptrstr *p = 0, *q = 0;
    forall(p, *this) {
        if (strcmpz(p->key, key) <= 0)
            break;
        q = p;
        }
    if (p && strcmpz(p->key, key) == 0) {
        p->data = data;
        return p;
        }
    p = new voidptrstr(data, key);
    insertafter(q, p);
    return p;
    } // End of function voidptrstrlist::insert.

//------------------------------//
//   voidptrstrlist::find_data  //
//------------------------------//
voidptrstr* voidptrstrlist::find_data(void*& data, const char* key) {
    voidptrstr* p = 0;
    forall(p, *this)
        if (strcmpz(p->key, key) == 0)
            break;
    if (p)
        data = p->data;
    return p;
    } // End of function voidptrstrlist::find_data.

/*------------------------------------------------------------------------------
This returns a string pointer in "key" which should be used immediately.
It should not be deleted!
------------------------------------------------------------------------------*/
//--------------------------//
// voidptrstrlist::find_key //
//--------------------------//
voidptrstr* voidptrstrlist::find_key(void* data, const char*& key) {
    voidptrstr* p = 0;
    forall(p, *this)
        if (p->data == data)
            break;
    if (p)
        key = p->key;
    return p;
    } // End of function voidptrstrlist::find_key.
