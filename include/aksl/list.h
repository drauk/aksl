// src/aksl/list.h   2018-3-3   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_LIST_H
#define AKSL_LIST_H
/*------------------------------------------------------------------------------
Classes in this file:

slink::
s1list::
s2list::
s2nlist::
ref::
reflist::
<T>Tlist::
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

//----------------------//
//       slink::        //
//----------------------//
struct slink { // A singly linked item.
friend struct s1list;
friend struct s2list;
friend struct s2nlist;
private:
    slink* nxt;
public:
    slink* next() const { return nxt; }

    slink(slink* n = 0) { nxt = n; }
    }; // End of struct slink.

/*------------------------------------------------------------------------------
Note that the length and position function members are vulnerable to error
when there are over 1 billion elements in the list. But then the RAM is likely
to run out too.
------------------------------------------------------------------------------*/
//----------------------//
//        s1list::      //
//----------------------//
struct s1list { // A list with only 1 pointer: to the first element.
private:
    slink* fst;
protected:
    void clearptrs() { fst = 0; } // To be used by clear().
public:
    bool_enum empty() const { return (bool_enum)(fst == 0); }
    long length() const;
    slink* first() const { return fst; }
    slink* element(register long i) const;
    bool_enum member(register slink*) const; // Returns true if in the list.
    long position(register slink*) const;    // Position of object in the list.
    void prepend(slink* x);
    slink* popfirst();
    slink* remove(register slink*);
    void delfirst() { delete popfirst(); }
    void delremove(slink* c) { delete remove(c); }
    void insertafter(slink* x1, slink* x2); // Insert x2 after x1.
    void clear() {
        for (register slink* x = fst; x; )
            { register slink* y = x->nxt; delete x; x = y; }
        clearptrs();
        }
    s1list() { fst = 0; }
    ~s1list();
    }; // End of struct s1list.

/*------------------------------------------------------------------------------
Note that the length and position function members are vulnerable to error
when there are over 1 billion elements in the list. But then the RAM is likely
to run out too.
------------------------------------------------------------------------------*/
//----------------------//
//        s2list::      //
//----------------------//
struct s2list { // A list with two pointers: to the first and last elements.
    friend class s2nlist;
private:
    slink *fst, *lst;
protected:
    void clearptrs() { fst = lst = 0; } // Used by clear() in derived classes.
public:
    bool_enum empty() const { return (bool_enum)(fst == 0); }
    long length() const;
    slink* first() const { return fst; }
    slink* last() const { return lst; }
    slink* element(register long i) const;
    bool_enum member(register slink*) const; // True if it is in the list.
    long position(register slink*) const;    // Position of object in the list.
    void append(slink* x);
    void prepend(slink* x);
    slink* popfirst();
    slink* poplast();
    slink* remove(register slink*);
    slink* removeafter(slink* x1, slink* x2); // Remove x2 after x1. Not secure.
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(slink* c) { delete remove(c); }

    void insertafter(slink* x1, slink* x2); // Insert x2 after x1.
    void swallow(s2list*); // Swallow contents of another list.
    void gulp(s2list*);    // Gulp contents of another list.
    void clear() {
        for (register slink* x = fst; x; )
            { register slink* y = x->nxt; delete x; x = y; }
        clearptrs();
        }
    s2list() { fst = lst = 0; }
    ~s2list();
    }; // End of struct s2list.

/*------------------------------------------------------------------------------
Singly linked list with first/last pointers and n_elements count.
------------------------------------------------------------------------------*/
//----------------------//
//       s2nlist::      //
//----------------------//
struct s2nlist: private s2list {
    long n_elements;    // Number of elements, if there aren't too many!
protected:
    // Used by clear() in derived classes.
    void clearptrs() { s2list::clearptrs(); n_elements = 0; }
public:
    // The routine members:
    using s2list::empty;
    long length() const { return n_elements; }
    using s2list::first;
    using s2list::last;
    using s2list::element;
    using s2list::member;
    using s2list::position;
    void append(slink* p) { s2list::append(p); if (p) n_elements += 1; }
    void prepend(slink* p) { s2list::prepend(p); if (p) n_elements += 1; }
    slink* popfirst()
        { slink* p = s2list::popfirst(); if (p) n_elements -= 1; return p; }
    slink* poplast()
        { slink* p = s2list::poplast(); if (p) n_elements -= 1; return p; }
    slink* remove(slink* x)
        { slink* p = s2list::remove(x); if (p) n_elements -= 1; return p; }
    slink* removeafter(slink* x1, slink* x2) {
        slink* p = s2list::removeafter(x1, x2);
        if (p)
            n_elements -= 1;
        return p;
        }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(slink* c) { delete remove(c); }

    void insertafter(slink* x1, slink* x2) // Insert x2 after x1.
        { s2list::insertafter(x1, x2); if (x2) n_elements += 1; }
    void swallow(s2nlist*); // Swallow contents of another list.
    void gulp(s2nlist*);    // Gulp contents of another list.
    void clear() { s2list::clear(); n_elements = 0; }

    s2nlist() { n_elements = 0; }
    ~s2nlist() { clear(); }
    }; // End of struct s2nlist.

//----------------------//
//         ref::        //
//----------------------//
struct ref: public slink {
private:
    void* rf;
public:
    ref* next() const { return (ref*)slink::next(); }
    void* value() const { return rf; }
    void* setvalue(void* v) { return rf = v; }

    ref(void* v = 0) { rf = v; }
    }; // End of struct ref.

//----------------------//
//       reflist::      //
//----------------------//
struct reflist: public s2list {
public:
    using s2list::empty;
    using s2list::length;
    ref* first() const { return (ref*)s2list::first(); }
    ref* last() const { return (ref*)s2list::last(); }
    void append(ref* c) { s2list::append(c); }
    void append(void* v) { append(new ref(v)); }
    void prepend(ref* c) { s2list::prepend(c); }
    void prepend(void* v) { prepend(new ref(v)); }
    ref* popfirst() { return (ref*)s2list::popfirst(); }
    ref* poplast() { return (ref*)s2list::poplast(); }
    ref* remove(ref* c) { return (ref*)s2list::remove(c); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(ref* c) { delete remove(c); }
    void insertafter(ref* p1, ref* p2) { s2list::insertafter(p1, p2); }
    void clear() {
        for (register ref* x = first(); x; )
            { register ref* y = x->next(); delete x; x = y; }
        clearptrs();
        }
    bool_enum disjoint(reflist&) const;
    }; // End of struct reflist.

// Experimental use of templates. Generates directory ptrepository!!!!!
// #ifdef TEMPLATES_OK
#ifdef SUNOS4_COMP_VERSION3_0_1
//----------------------//
//        Tlist::       //
//----------------------//
template<struct T>
struct Tlist: private s2list {
protected:
    s2list::clearptrs;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    s2list::position;
    T* first() const { return (T*)s2list::first(); }
    T* last() const { return (T*)s2list::last(); }
    T* element(long i) const
        { return (T*)s2list::element(i); }
    void append(T* p) { s2list::append(p); }
    void prepend(T* p) { s2list::prepend(p); }
    T* popfirst() { return (T*)s2list::popfirst(); }
    T* poplast() { return (T*)s2list::poplast(); }
    T* remove(T* p)
        { return (T*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(T* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(Tlist& l) { s2list::swallow(&l); }
    void gulp(Tlist& l) { s2list::gulp(&l); }
    void clear() { for (T* p = first(); p; )
        { T* q = p->next(); delete p; p = q; } clearptrs(); }

//    Tlist& operator=(const Tlist& x) {}
//    Tlist(const Tlist& x) {};
    Tlist() {}
    ~Tlist() { clear(); }
    }; // End of struct Tlist.
#endif /* TEMPLATES_OK */

#endif /* AKSL_LIST_H */
