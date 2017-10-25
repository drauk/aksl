// src/aksl/intlist.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/intlist.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_INTLIST_H
#define AKSL_INTLIST_H
/*------------------------------------------------------------------------------
Classes in this file:

intlink::
intlist::
intkey::
intkeylist::
------------------------------------------------------------------------------*/

#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif

//----------------------//
//      intlink::       //
//----------------------//
struct intlink: public slink {
    long i;

    intlink* next() const { return (intlink*)slink::next(); }

    intlink(long ii) { i = ii; }
    intlink() { i = 0; }
    ~intlink() {}
    }; // End of struct intlink.

//----------------------//
//       intlist::      //
//----------------------//
struct intlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    intlink* first() const { return (intlink*)s2list::first(); }
    intlink* last() const { return (intlink*)s2list::last(); }
    void append(intlink* p) { s2list::append(p); }
    void prepend(intlink* p) { s2list::prepend(p); }
    intlink* popfirst() { return (intlink*)s2list::popfirst(); }
    intlink* poplast() { return (intlink*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    s2list::insertafter;
    void clear() { for (intlink* p = first(); p; )
        { intlink* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(long i) { intlink* p = new intlink(i); append(p); }
    intlink* find(long) const;
    void add(long);     // Addition _with_ duplicates.

    intlist() {}
    ~intlist() { clear(); }
    }; // End of struct intlist.

//----------------------//
//       intkey::       //
//----------------------//
struct intkey: public slink {
    long    data;           // Data being stored.
    long    key;            // Key to data.

    intkey* next() const { return (intkey*)slink::next(); }

    intkey& operator=(const intkey& x)
        { data = x.data; key = x.key; return *this; }
    intkey(const intkey& x) { data = x.data; key = x.key; };
    intkey(long d, long k) { data = d; key = k; };
    intkey() { data = 0; key = 0; }
    ~intkey() {}
    }; // End of struct intkey.

//----------------------//
//      intkeylist::    //
//----------------------//
struct intkeylist: private s2list {
protected:
    s2list::clearptrs;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    s2list::position;
    intkey* first() const { return (intkey*)s2list::first(); }
    intkey* last() const { return (intkey*)s2list::last(); }
    intkey* element(long i) const
        { return (intkey*)s2list::element(i); }
    void append(intkey* p) { s2list::append(p); }
    void prepend(intkey* p) { s2list::prepend(p); }
    intkey* popfirst() { return (intkey*)s2list::popfirst(); }
    intkey* poplast() { return (intkey*)s2list::poplast(); }
    intkey* remove(intkey* p)
        { return (intkey*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(intkey* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(intkeylist& l) { s2list::swallow(&l); }
    void gulp(intkeylist& l) { s2list::gulp(&l); }
    void clear() { for (intkey* p = first(); p; )
        { intkey* q = p->next(); delete p; p = q; } clearptrs(); }

    intkey* append(long data, long key)     // Append a record.
        { intkey* p = new intkey(data, key); append(p); return p; }
    intkey* prepend(long data, long key)     // Prepend a record.
        { intkey* p = new intkey(data, key); prepend(p); return p; }
    intkey* insert_inc(long data, long key); // Insert in incr order of key.
    intkey* insert_dec(long data, long key);  // Insert in incr order of key.
    intkey* insert(long data, long key)
        { return insert_dec(data, key); } // For backwards compatibility.
    intkey* find_data(long& data, long key); // Find data, given key.
    intkey* find_key(long data, long& key); // Find (lowest) key, given data.

//    intkeylist& operator=(const intkeylist& x) {}
//    intkeylist(const intkeylist& x) {};
    intkeylist() {}
    ~intkeylist() { clear(); }
    }; // End of struct intkeylist.

#endif /* AKSL_INTLIST_H */
