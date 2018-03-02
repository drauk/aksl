// src/aksl/vplist.h   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/include/aksl/vplist.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_VPLIST_H
#define AKSL_VPLIST_H
/*------------------------------------------------------------------------------
Classes in this file:

voidptr::
voidptrlist::
voidptr_b::
voidptr_blist::
voidptrkey::
voidptrkeylist::
voidptrstr::
voidptrstrlist::
------------------------------------------------------------------------------*/

#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_NEWSTR_H
#include "aksl/newstr.h"
#endif
#ifndef AKSL_BMEM_H
#include "aksl/bmem.h"
#endif

/*------------------------------------------------------------------------------
This class is almost identical to "ref" in list.h. Maybe it isn't needed!?
------------------------------------------------------------------------------*/
//----------------------//
//       voidptr::      //
//----------------------//
struct voidptr: public slink {
    void* i;

    voidptr* next() const { return (voidptr*)slink::next(); }

    voidptr(void* ii) { i = ii; }
    voidptr() { i = 0; }
    ~voidptr() {}
    }; // End of struct voidptr.

//----------------------//
//     voidptrlist::    //
//----------------------//
struct voidptrlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    voidptr* first() const { return (voidptr*)s2list::first(); }
    voidptr* last() const { return (voidptr*)s2list::last(); }
    voidptr* element(long i) const
        { return (voidptr*)s2list::element(i); }
    void append(voidptr* p) { s2list::append(p); }
    void prepend(voidptr* p) { s2list::prepend(p); }
    voidptr* popfirst() { return (voidptr*)s2list::popfirst(); }
    voidptr* poplast() { return (voidptr*)s2list::poplast(); }
    voidptr* remove(voidptr* p)
        { return (voidptr*)s2list::remove(p); }
    voidptr* removeafter(voidptr* q, voidptr* p)
        { return (voidptr*)s2list::removeafter(q, p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(voidptr* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(voidptrlist& l) { s2list::swallow(&l); }
    void gulp(voidptrlist& l) { s2list::gulp(&l); }
    void clear() { for (voidptr* p = first(); p; )
        { voidptr* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(void* i) { voidptr* p = new voidptr(i); append(p); }
    voidptr* find(void*) const;
    void add(void*);        // Append _without_ duplicates.
    void add_sort(void*);   // Sorting add _without_ duplicates.

//    voidptrlist& operator=(const voidptrlist& x) {}
//    voidptrlist(const voidptrlist& x) {};
    voidptrlist() {}
    ~voidptrlist() { clear(); }
    }; // End of struct voidptrlist.

/*------------------------------------------------------------------------------
This class differs from "voidptr" in being bmem-allocated.
This is, it uses block memory allocation via class "bmem" rather than being
allocated item-by-item with the new-operator.
Advantages:
-   Block-memory allocation is quicker.
-   Memory is never freed to the global heap, so it avoids possible
    heap management bugs. I.e. it is safer.
-   You can keep track of memory allocation using the statistical
    functions in class "bmem".
Disadvantages:
-   You must NOT derive any class from class "voidptr_b", unless you also do
    explicit memory allocation for any derived classes. I.e. you must
    define "new" and "delete" operators for every derived class, e.g.
    using the "bmem" class as here.
-   Memory is never give back. So it could be wasteful if a large
    amount of memory is allocated in voidprt_b objects and then deallocated.
------------------------------------------------------------------------------*/
//----------------------//
//      voidptr_b::     //
//----------------------//
struct voidptr_b: public voidptr {
    voidptr_b* next() const { return (voidptr_b*)voidptr::next(); }

    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }

    voidptr_b(void* ii) : voidptr(ii) {}
    voidptr_b() {}
    ~voidptr_b() {}
    }; // End of struct voidptr_b.

//----------------------//
//    voidptr_blist::   //
//----------------------//
struct voidptr_blist: private voidptrlist {
protected:
    using voidptrlist::clearptrs;
public:
    // The routine members:
    using voidptrlist::empty;
    using voidptrlist::length;
    using voidptrlist::member;
    using voidptrlist::position;
    voidptr_b* first() const { return (voidptr_b*)voidptrlist::first(); }
    voidptr_b* last() const { return (voidptr_b*)voidptrlist::last(); }
    voidptr_b* element(long i) const
        { return (voidptr_b*)voidptrlist::element(i); }
    void append(voidptr_b* p) { voidptrlist::append(p); }
    void prepend(voidptr_b* p) { voidptrlist::prepend(p); }
    voidptr_b* popfirst() { return (voidptr_b*)voidptrlist::popfirst(); }
    voidptr_b* poplast() { return (voidptr_b*)voidptrlist::poplast(); }
    voidptr_b* remove(voidptr_b* p)
        { return (voidptr_b*)voidptrlist::remove(p); }
    voidptr_b* removeafter(voidptr_b* q, voidptr_b* p)
        { return (voidptr_b*)voidptrlist::removeafter(q, p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(voidptr_b* p) { delete remove(p); }
    using voidptrlist::insertafter;
    void swallow(voidptr_blist& l) { voidptrlist::swallow(l); }
    void gulp(voidptr_blist& l) { voidptrlist::gulp(l); }
    void clear() { for (voidptr_b* p = first(); p; )
        { voidptr_b* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(void* i) { voidptr_b* p = new voidptr_b(i); append(p); }
    void prepend(void* i) { voidptr_b* p = new voidptr_b(i); prepend(p); }
    voidptr_b* find(void*) const;
    voidptr_b* remove(void* i);
    void add(void*);        // Append _without_ duplicates.
    void add_sort(void*);   // Sorting add _without_ duplicates.

//    voidptr_blist& operator=(const voidptr_blist& x) {}
//    voidptr_blist(const voidptr_blist& x) {};
    voidptr_blist() {}
    ~voidptr_blist() { clear(); }
    }; // End of struct voidptr_blist.

//----------------------//
//     voidptrkey::     //
//----------------------//
struct voidptrkey: public slink {
    void*   data;           // Data being stored.
    long    key;            // Key to data.

    voidptrkey* next() const { return (voidptrkey*)slink::next(); }

    voidptrkey& operator=(const voidptrkey& x)
        { data = x.data; key = x.key; return *this; }
    voidptrkey(const voidptrkey& x) { data = x.data; key = x.key; };
    voidptrkey(void* d, long k) { data = d; key = k; };
    voidptrkey() { data = 0; key = 0; }
    ~voidptrkey() {}
    }; // End of struct voidptrkey.

//----------------------//
//    voidptrkeylist::  //
//----------------------//
struct voidptrkeylist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    voidptrkey* first() const { return (voidptrkey*)s2list::first(); }
    voidptrkey* last() const { return (voidptrkey*)s2list::last(); }
    voidptrkey* element(long i) const
        { return (voidptrkey*)s2list::element(i); }
    void append(voidptrkey* p) { s2list::append(p); }
    void prepend(voidptrkey* p) { s2list::prepend(p); }
    voidptrkey* popfirst() { return (voidptrkey*)s2list::popfirst(); }
    voidptrkey* poplast() { return (voidptrkey*)s2list::poplast(); }
    voidptrkey* remove(voidptrkey* p)
        { return (voidptrkey*)s2list::remove(p); }
    voidptrkey* removeafter(voidptrkey* q, voidptrkey* p)
        { return (voidptrkey*)s2list::removeafter(q, p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(voidptrkey* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(voidptrkeylist& l) { s2list::swallow(&l); }
    void gulp(voidptrkeylist& l) { s2list::gulp(&l); }
    void clear() { for (voidptrkey* p = first(); p; )
        { voidptrkey* q = p->next(); delete p; p = q; } clearptrs(); }

    voidptrkey* append(void* data, long key)    // Append a record.
        { voidptrkey* p = new voidptrkey(data, key); append(p); return p; }
    voidptrkey* prepend(void* data, long key)    // Prepend a record.
        { voidptrkey* p = new voidptrkey(data, key); prepend(p); return p; }
    voidptrkey* insert(void* data, long key);   // Insert in incr order of key.
    voidptrkey* find_data(long key); // Find entry, given key.
    voidptrkey* find_data(void*& data, long key); // Find data, given key.
    voidptrkey* find_key(void* data, long& key);  // Find key, given data.
    voidptrkey* remove_key(long key); // Remove entry for given key.

//    voidptrkeylist& operator=(const voidptrkeylist& x) {}
//    voidptrkeylist(const voidptrkeylist& x) {};
    voidptrkeylist() {}
    ~voidptrkeylist() { clear(); }
    }; // End of struct voidptrkeylist.

//----------------------//
//     voidptrstr::     //
//----------------------//
struct voidptrstr: public slink {
    void*   data;           // Data being stored.
    char*   key;            // Key to data.

    voidptrstr* next() const { return (voidptrstr*)slink::next(); }

    voidptrstr& operator=(const voidptrstr& x)
        { data = x.data; delete[] key; key = new_strcpy(x.key); return *this; }
    voidptrstr(const voidptrstr& x) { data = x.data; key = new_strcpy(x.key); };
    voidptrstr(void* d, const char* k) { data = d; key = new_strcpy(k); };
    voidptrstr() { data = 0; key = 0; }
    ~voidptrstr() {} // Why is there no delete here?
    }; // End of struct voidptrstr.

//----------------------//
//    voidptrstrlist::  //
//----------------------//
struct voidptrstrlist: private s2list {
protected:
    using s2list::clearptrs;
public:
    // The routine members:
    using s2list::empty;
    using s2list::length;
    using s2list::member;
    using s2list::position;
    voidptrstr* first() const { return (voidptrstr*)s2list::first(); }
    voidptrstr* last() const { return (voidptrstr*)s2list::last(); }
    voidptrstr* element(long i) const
        { return (voidptrstr*)s2list::element(i); }
    void append(voidptrstr* p) { s2list::append(p); }
    void prepend(voidptrstr* p) { s2list::prepend(p); }
    voidptrstr* popfirst() { return (voidptrstr*)s2list::popfirst(); }
    voidptrstr* poplast() { return (voidptrstr*)s2list::poplast(); }
    voidptrstr* remove(voidptrstr* p)
        { return (voidptrstr*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(voidptrstr* p) { delete remove(p); }
    using s2list::insertafter;
    void swallow(voidptrstrlist& l) { s2list::swallow(&l); }
    void gulp(voidptrstrlist& l) { s2list::gulp(&l); }
    void clear() { for (voidptrstr* p = first(); p; )
        { voidptrstr* q = p->next(); delete p; p = q; } clearptrs(); }

    // Non-routine members:
    voidptrstr* append(void* data, const char* key)     // Append a record.
        { voidptrstr* p = new voidptrstr(data, key); append(p); return p; }
    voidptrstr* prepend(void* data, const char* key)    // Prepend a record.
        { voidptrstr* p = new voidptrstr(data, key); prepend(p); return p; }
    // Insert in increasing order of key.
    voidptrstr* insert(void* data, const char* key);
    voidptrstr* find_data(void*& data, const char* key); // Find data from key.
    voidptrstr* find_key(void* data, const char*& key);  // Find key from data.

//    voidptrstrlist& operator=(const voidptrstrlist& x) {}
//    voidptrstrlist(const voidptrstrlist& x) {};
    voidptrstrlist() {}
    ~voidptrstrlist() { clear(); }
    }; // End of struct voidptrstrlist.

#endif /* AKSL_VPLIST_H */
