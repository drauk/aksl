// src/aksl/hashfn.c   2018-3-4   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

hashtab_32_8::
    insert
    append
    prepend
    find
    del
    n_entries
    first
    next
hashtab_32_16::
    insert
    append
    prepend
    find
    first
    next
hashtab_str_8::
    insert
    append
    prepend
    find
    first
    next
------------------------------------------------------------------------------*/

#include "aksl/hashfn.h"

static const int mask8     = 0xff;
static const int mask16    = 0xffff;

/*------------------------------------------------------------------------------
This insert/find algorithm only works if a void* fits into a long.
This function will always over-write an existing entry for the given key.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_32_8::insert //
//----------------------//
// void hashtab_32_8::insert(void* data, uint32 key) {
void hashtab_32_8::insert(const void* data, uint32 key) {
    int h = int(hashfn_32_8(key)) & mask8;
    table[h].insert(data, long(key));
    } // End of function hashtab_32_8::insert.

/*------------------------------------------------------------------------------
This append/find algorithm only works if a void* fits into a long.
Note that this function does _not_ call voidptrkeylist::insert. For speed, the
append function is used instead.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_32_8::append //
//----------------------//
void hashtab_32_8::append(void* data, uint32 key) {
    int h = int(hashfn_32_8(key)) & mask8;
    table[h].append(data, long(key));
    } // End of function hashtab_32_8::append.

/*------------------------------------------------------------------------------
This prepend/find algorithm only works if a void* fits into a long.
Note that this function does _not_ call voidptrkeylist::insert. For speed, the
prepend function is used instead.
------------------------------------------------------------------------------*/
//--------------------------//
//   hashtab_32_8::prepend  //
//--------------------------//
void hashtab_32_8::prepend(void* data, uint32 key) {
    int h = int(hashfn_32_8(key)) & mask8;
    table[h].prepend(data, long(key));
    } // End of function hashtab_32_8::prepend.

//----------------------//
//  hashtab_32_8::find  //
//----------------------//
// bool_enum hashtab_32_8::find(void*& data, uint32 key) {
bool_enum hashtab_32_8::find(const void*& data, uint32 key) {
    int h = int(hashfn_32_8(key)) & mask8;
    return table[h].find_data(data, long(key)) ? true : false;
    } // End of function hashtab_32_8::find.

/*------------------------------------------------------------------------------
This first finds the specified entry, and then deletes it from the hash table.
Returns "true" iff there was an entry for that key to delete.
------------------------------------------------------------------------------*/
//----------------------//
//   hashtab_32_8::del  //
//----------------------//
bool_enum hashtab_32_8::del(uint32 key) {
    // Hash the key.
    int h = int(hashfn_32_8(key)) & mask8;

    // Remove the entry for the given key, if any.
    voidptrkey* p = table[h].remove_key(long(key));
    if (!p)
        return false;

    // Delete the entry if it is found.
    delete p;
    return true;
    } // End of function hashtab_32_8::del.

//--------------------------//
//  hashtab_32_8::n_entries //
//--------------------------//
long hashtab_32_8::n_entries() {
    long x = 0;
    for (int i = 0; i < tab8size; ++i)
        x += table[i].length();

    return x;
    } // End of function hashtab_32_8::n_entries.

/*------------------------------------------------------------------------------
This function leaves the members trav_i and trav_ptr positioned for the next
traversal access.
Clearly it is not possible to have two concurrent traversals of a single object
by this method.
------------------------------------------------------------------------------*/
//----------------------//
//  hashtab_32_8::first //
//----------------------//
voidptrkey* hashtab_32_8::first(int* pi) {
    for (trav_i = 0; trav_i < tab8size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_32_8::first.

/*------------------------------------------------------------------------------
The intended calling sequence is to call first() first, which initialises the
traversal pointers. Then next() is called until it returns null. The order of
the pointers thus returned is not particularly monotonic with respect to "key".
------------------------------------------------------------------------------*/
//----------------------//
//  hashtab_32_8::next  //
//----------------------//
voidptrkey* hashtab_32_8::next(int* pi) {
    // If at the end of the traversal, return null pointer:
    if (!trav_ptr || trav_i >= tab8size) {
        if (pi)
            *pi = trav_i;
        return 0;
        }

    // Increment the traversal pointer. If non-null for same i, return this:
    trav_ptr = trav_ptr->next();
    if (trav_ptr) {
        if (pi)
            *pi = trav_i;
        return trav_ptr;
        }

    // If current i is exhausted, try the rest of the array:
    for (++trav_i ; trav_i < tab8size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_32_8::next.

/*------------------------------------------------------------------------------
This function will always over-write an existing entry for the given key.
------------------------------------------------------------------------------*/
//--------------------------//
//   hashtab_32_16::insert  //
//--------------------------//
void hashtab_32_16::insert(void* data, uint32 key) {
    int h = int(hashfn_32_16(key)) & mask16;
    table[h].insert(data, long(key));
    } // End of function hashtab_32_16::insert.

/*------------------------------------------------------------------------------
Note that this function does _not_ call voidptrkeylist::insert. For speed, the
append function is used instead.
------------------------------------------------------------------------------*/
//--------------------------//
//   hashtab_32_16::append  //
//--------------------------//
void hashtab_32_16::append(void* data, uint32 key) {
    int h = int(hashfn_32_16(key)) & mask16;
    table[h].append(data, long(key));
    } // End of function hashtab_32_16::append.

/*------------------------------------------------------------------------------
Note that this function does _not_ call voidptrkeylist::insert. For speed, the
prepend function is used instead.
------------------------------------------------------------------------------*/
//--------------------------//
//  hashtab_32_16::prepend  //
//--------------------------//
void hashtab_32_16::prepend(void* data, uint32 key) {
    int h = int(hashfn_32_16(key)) & mask16;
    table[h].prepend(data, long(key));
    } // End of function hashtab_32_16::prepend.

//----------------------//
//  hashtab_32_16::find //
//----------------------//
// bool_enum hashtab_32_16::find(void*& data, uint32 key) {
bool_enum hashtab_32_16::find(const void*& data, uint32 key) {
    int h = int(hashfn_32_16(key)) & mask16;
    return table[h].find_data(data, long(key)) ? true : false;
    } // End of function hashtab_32_16::find.

//--------------------------//
// hashtab_32_16::n_entries //
//--------------------------//
long hashtab_32_16::n_entries() {
    long x = 0;
    for (int i = 0; i < tab16size; ++i)
        x += table[i].length();

    return x;
    } // End of function hashtab_32_16::n_entries.

/*------------------------------------------------------------------------------
This function leaves the members trav_i and trav_ptr positioned for the next
traversal access.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_32_16::first //
//----------------------//
voidptrkey* hashtab_32_16::first(int* pi) {
    for (trav_i = 0; trav_i < tab16size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_32_16::first.

/*------------------------------------------------------------------------------
The intended calling sequence is to call first() first, which initialises the
traversal pointers. Then next() is called until it returns null. The order of
the pointers thus returned is not particularly monotonic with respect to "key".
------------------------------------------------------------------------------*/
//----------------------//
//  hashtab_32_16::next //
//----------------------//
voidptrkey* hashtab_32_16::next(int* pi) {
    // If at the end of the traversal, return null pointer:
    if (!trav_ptr || trav_i >= tab16size) {
        if (pi)
            *pi = trav_i;
        return 0;
        }

    // Increment the traversal pointer. If non-null for same i, return this:
    trav_ptr = trav_ptr->next();
    if (trav_ptr) {
        if (pi)
            *pi = trav_i;
        return trav_ptr;
        }

    // If current i is exhausted, try the rest of the array:
    for (++trav_i ; trav_i < tab16size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_32_16::next.

/*------------------------------------------------------------------------------
This function will always over-write an existing entry for the given key.
This could be quite slow, since string sort/insert is used.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_str_8::insert//
//----------------------//
void hashtab_str_8::insert(void* data, const char* key) {
    int h = int(hashfn_str_8(key)) & mask8;
    table[h].insert(data, key);
    } // End of function hashtab_str_8::insert.

/*------------------------------------------------------------------------------
Note that this function does _not_ call voidptrstrlist::insert. For speed, the
append function is used instead.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_str_8::append//
//----------------------//
void hashtab_str_8::append(void* data, const char* key) {
    int h = int(hashfn_str_8(key)) & mask8;
    table[h].append(data, key);
    } // End of function hashtab_str_8::append.

/*------------------------------------------------------------------------------
Note that this function does _not_ call voidptrstrlist::insert. For speed, the
prepend function is used instead.
------------------------------------------------------------------------------*/
//--------------------------//
//  hashtab_str_8::prepend  //
//--------------------------//
void hashtab_str_8::prepend(void* data, const char* key) {
    int h = int(hashfn_str_8(key)) & mask8;
    table[h].prepend(data, key);
    } // End of function hashtab_str_8::prepend.

//----------------------//
//  hashtab_str_8::find //
//----------------------//
bool_enum hashtab_str_8::find(void*& data, const char* key) {
    int h = int(hashfn_str_8(key)) & mask8;
    return table[h].find_data(data, key) ? true : false;
    } // End of function hashtab_str_8::find.

/*------------------------------------------------------------------------------
This is slow!
------------------------------------------------------------------------------*/
//--------------------------//
// hashtab_str_8::n_entries //
//--------------------------//
long hashtab_str_8::n_entries() {
    long x = 0;
    for (int i = 0; i < tab8size; ++i)
        x += table[i].length();

    return x;
    } // End of function hashtab_str_8::n_entries.

/*------------------------------------------------------------------------------
This function leaves the members trav_i and trav_ptr positioned for the next
traversal access.
------------------------------------------------------------------------------*/
//----------------------//
// hashtab_str_8::first //
//----------------------//
voidptrstr* hashtab_str_8::first(int* pi) {
    for (trav_i = 0; trav_i < tab8size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_str_8::first.

/*------------------------------------------------------------------------------
The intended calling sequence is to call first() first, which initialises the
traversal pointers. Then next() is called until it returns null. The order of
the pointers thus returned is not particularly monotonic with respect to "key".
------------------------------------------------------------------------------*/
//----------------------//
//  hashtab_str_8::next //
//----------------------//
voidptrstr* hashtab_str_8::next(int* pi) {
    // If at the end of the traversal, return null pointer:
    if (!trav_ptr || trav_i >= tab8size) {
        if (pi)
            *pi = trav_i;
        return 0;
        }

    // Increment the traversal pointer. If non-null for same i, return this:
    trav_ptr = trav_ptr->next();
    if (trav_ptr) {
        if (pi)
            *pi = trav_i;
        return trav_ptr;
        }

    // If current i is exhausted, try the rest of the array:
    for (++trav_i ; trav_i < tab8size; ++trav_i) {
        trav_ptr = table[trav_i].first();
        if (trav_ptr)
            break;
        }
    if (pi)
        *pi = trav_i;
    return trav_ptr;
    } // End of function hashtab_str_8::next.
