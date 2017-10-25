// src/aksl/value.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/value.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_VALUE_H
#define AKSL_VALUE_H
/*------------------------------------------------------------------------------
Classes defined in this file:

value::
valuelist::
tagvalue::
tagvaluelist::
colonlist::
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_DATUM_H
#include "aksl/datum.h"
#endif
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_BMEM_H
#include "aksl/bmem.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

// Forward references:
struct valuelist;
struct tagvaluelist;
struct colonlist;

// Defined somewhere else:
struct object;

// Types of "values":
enum Vtype {
    vNONE,                          // Undefined.
    vINTEGER,                       // Integer.
    vREAL,                          // Real number.
    vSTRING,                        // Char string.
    vOBJECT,                        // Pointer to object.
    vDATUM,                         // Datum. (Packet/message between objects.)
    vLIST,                          // Value list.
    vTVLIST,                        // Tagged value list.
    vCOLONLIST                      // Colon-separated list.
    };

// In future, have a value of type "reference to value", which will be
// a pointer to a value, and which will be used as an enum substitute in
// system files etc.

//extern bmem value_bmem;         // Fast block memory for "value"s.
//extern bmem tagvalue_bmem;      // Fast block memory for "tagvalue"s.

/*------------------------------------------------------------------------------
A "value" can take on various value types, for passing arguments in messages,
and also for setting initial values of object attributes.
Note that the "value" class does not make its own copies of strings, lists,
datums or objects. It is used only to _refer_ to pieces of data in these
cases.
------------------------------------------------------------------------------*/
//----------------------//
//       value::        //
//----------------------//
struct value: public slink {
friend struct tagvalue;
private:
    Vtype ty;
    union {
        long            i;
        double          r;
        char*           s;  // May be static or heap allocated.
        object*         p;
        datum*          d;
        valuelist*      l;
        tagvaluelist*   tvl;
        colonlist*      cl;
        };
public:
    value* next() { return (value*)slink::next(); }

    Vtype type() { return ty; }
//    const char* type_name();

    // Tests for various types of value:
    bool_enum Integer() const { return (bool_enum)(ty == vINTEGER); }
    bool_enum PosInteger() const
        { return (bool_enum)(ty == vINTEGER && i > 0); }
    bool_enum NonNegInteger() const
        { return (bool_enum)(ty == vINTEGER && i >= 0); }
    bool_enum Real() const { return (bool_enum)(ty == vREAL); }
    bool_enum PosReal() const
        { return (bool_enum)(ty == vREAL && r > 0); }
    bool_enum NonNegReal() const
        { return (bool_enum)(ty == vREAL && r >= 0); }
    bool_enum Number() const
        { return (bool_enum)(ty == vINTEGER || ty == vREAL); }
    bool_enum PosNumber() const {
        return (bool_enum)((ty == vINTEGER && i > 0) || (ty == vREAL && r > 0));
        }
    bool_enum NonNegNumber() const {
        return (bool_enum)((ty == vINTEGER && i >= 0) || (ty == vREAL && r>=0));
        }
    bool_enum String() const { return (bool_enum)(ty == vSTRING); }
    bool_enum Object() const { return (bool_enum)(ty == vOBJECT); }
    bool_enum Datum() const { return (bool_enum)(ty == vDATUM); }
    bool_enum List() const { return (bool_enum)(ty == vLIST); }
    bool_enum Tvlist() const { return (bool_enum)(ty == vTVLIST); }
    bool_enum Colonlist() const { return (bool_enum)(ty == vCOLONLIST); }

    void clear() { ty = vNONE; }
    void copyto(value& x) const;    // Copy this value to another value.
    value* copy() const {           // Make a carbon copy of this "value".
        value* pv = new value;
        copyto(*pv);
        return pv;
        }
    void print(ostream&) const;     // Ad hoc printing routine.

    operator long() const;
    operator double() const;
    operator const char*() const;
    operator object*() const;
    operator datum*() const;
    operator valuelist*() const;
    operator tagvaluelist*() const;
    operator colonlist*() const;

    value& operator=(long);
    value& operator=(double);
    value& operator=(char*);
    value& operator=(object*);
    value& operator=(datum*);
    value& operator=(valuelist*);
    value& operator=(tagvaluelist*);
    value& operator=(colonlist*);

    value(long ii) { ty = vINTEGER; i = ii; }
    value(double rr) { ty = vREAL; r = rr; }
    value(char* ss) { ty = vSTRING; s = ss; }
    value(object* pp) { ty = vOBJECT; p = pp; }
    value(datum* dd) { ty = vDATUM; d = dd; }
    value(valuelist* ll) { ty = vLIST; l = ll; }
    value(tagvaluelist* tt) { ty = vTVLIST; tvl = tt; }
    value(colonlist* cll) { ty = vCOLONLIST; cl = cll; }

    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }

    value& operator=(value&);
    value(value&);
    value() { ty = vNONE; }
    ~value() {}
    }; // End of struct value.

//----------------------//
//      valuelist::     //
//----------------------//
struct valuelist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    value* first() { return (value*)s2list::first(); }
    value* last() { return (value*)s2list::last(); }
    void append(value* p) { s2list::append(p); }
    void prepend(value* p) { s2list::prepend(p); }
    value* popfirst() { return (value*)s2list::popfirst(); }
    value* poplast() { return (value*)s2list::poplast(); }
    value* remove(value* p) { return (value*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(value* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(valuelist& l) { s2list::swallow(&l); }
    void clear() { for (value* p = first(); p; )
        { value* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(double r) { value* p = new value(r); append(p); }
    void append(long i) { value* p = new value(i); append(p); }
    void append(char* s) { value* p = new value(s); append(p); }
    void append(object* pe) { value* p = new value(pe); append(p); }
    void append(datum* d) { value* p = new value(d); append(p); }
    void append(valuelist* vl) { value* p = new value(vl); append(p); }
    void append(colonlist* cl) { value* p = new value(cl); append(p); }
    valuelist* copy();

//    valuelist& operator=(const valuelist& x) {}
//    valuelist(const valuelist& x) {};
    valuelist() {}
    ~valuelist() { clear(); }
    }; // End of struct valuelist.

/*------------------------------------------------------------------------------
A "tagvalue" represents a "value" with a tag.
------------------------------------------------------------------------------*/
//----------------------//
//       tagvalue::     //
//----------------------//
struct tagvalue: public value {
    mtype tag;

    tagvalue* next() { return (tagvalue*)value::next(); }

    tagvalue* copy() const {    // A carbon copy of this "tagvalue".
        tagvalue* ptv = new tagvalue;
        ptv->tag = tag;
        value::copyto(*(value*)ptv);
        return ptv;
        }
    void print(ostream&) const; // Ad hoc printing routine.

//    operator long() const { return value::operator long(); }
//    operator double() const { return value::operator double(); }
//    operator const char*() const { return value::operator const char*(); }
//    operator object*() const { return value::operator object*(); }
//    operator datum*() const { return value::operator datum*(); }
//    operator valuelist*() const { return value::operator valuelist*(); }
//    operator tagvaluelist*() const { return value::operator tagvaluelist*(); }
//    operator colonlist*() const { return value::operator colonlist*(); }

    tagvalue& operator=(long);
    tagvalue& operator=(double);
    tagvalue& operator=(char*);
    tagvalue& operator=(object*);
    tagvalue& operator=(datum*);
    tagvalue& operator=(valuelist*);
    tagvalue& operator=(tagvaluelist*);
    tagvalue& operator=(colonlist*);

    tagvalue(long x): value(x) { tag = 0; }
    tagvalue(double x): value(x) { tag = 0; }
    tagvalue(char* x): value(x) { tag = 0; }
    tagvalue(object* x): value(x) { tag = 0; }
    tagvalue(datum* x): value(x) { tag = 0; }
    tagvalue(valuelist* x): value(x) { tag = 0; }
    tagvalue(tagvaluelist* x): value(x) { tag = 0; }
    tagvalue(colonlist* x): value(x) { tag = 0; }

    tagvalue(mtype t, long x): value(x) { tag = t; }
    tagvalue(mtype t, double x): value(x) { tag = t; }
    tagvalue(mtype t, char* x): value(x) { tag = t; }
    tagvalue(mtype t, object* x): value(x) { tag = t; }
    tagvalue(mtype t, datum* x): value(x) { tag = t; }
    tagvalue(mtype t, valuelist* x): value(x) { tag = t; }
    tagvalue(mtype t, tagvaluelist* x): value(x) { tag = t; }
    tagvalue(mtype t, colonlist* x): value(x) { tag = t; }

    // Memory management things.
    static bmem bmem0;
    void* operator new(size_t) { return bmem0.newchunk(); }
    void operator delete(void* p) { bmem0.freechunk(p); }
    static unsigned long n_objects() { return bmem0.length(); }

    tagvalue& operator=(tagvalue&);
    tagvalue(tagvalue&);
    tagvalue() { tag = 0; }
    ~tagvalue() {}
    }; // End of struct tagvalue.

//----------------------//
//     tagvaluelist::   //
//----------------------//
struct tagvaluelist: private s2list {
protected:
    s2list::clearptrs;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    s2list::position;
    tagvalue* first() const { return (tagvalue*)s2list::first(); }
    tagvalue* last() const { return (tagvalue*)s2list::last(); }
    tagvalue* element(long i) const
        { return (tagvalue*)s2list::element(i); }
    void append(tagvalue* p) { s2list::append(p); }
    void prepend(tagvalue* p) { s2list::prepend(p); }
    tagvalue* popfirst() { return (tagvalue*)s2list::popfirst(); }
    tagvalue* poplast() { return (tagvalue*)s2list::poplast(); }
    tagvalue* remove(tagvalue* p)
        { return (tagvalue*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(tagvalue* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(tagvaluelist& l) { s2list::swallow(&l); }
    void gulp(tagvaluelist& l) { s2list::gulp(&l); }
    void clear() { for (tagvalue* p = first(); p; )
        { tagvalue* q = p->next(); delete p; p = q; } clearptrs(); }

    tagvaluelist* copy();

//    tagvaluelist& operator=(const tagvaluelist& x) {}
//    tagvaluelist(const tagvaluelist& x) {};
    tagvaluelist() {}
    ~tagvaluelist() { clear(); }
    }; // End of struct tagvaluelist.

/*------------------------------------------------------------------------------
Apart from the name, this is the same as a valuelist.
------------------------------------------------------------------------------*/
//----------------------//
//      colonlist::     //
//----------------------//
struct colonlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    value* first() { return (value*)s2list::first(); }
    value* last() { return (value*)s2list::last(); }
    void append(value* p) { s2list::append(p); }
    void prepend(value* p) { s2list::prepend(p); }
    value* popfirst() { return (value*)s2list::popfirst(); }
    value* poplast() { return (value*)s2list::poplast(); }
    value* remove(value* p) { return (value*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(value* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(colonlist& l) { s2list::swallow(&l); }
    void clear() { for (value* p = first(); p; )
        { value* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(double r) { value* p = new value(r); append(p); }
    void append(long i) { value* p = new value(i); append(p); }
    void append(char* s) { value* p = new value(s); append(p); }
    void append(object* pe) { value* p = new value(pe); append(p); }
    void append(datum* d) { value* p = new value(d); append(p); }
    void append(valuelist* vl) { value* p = new value(vl); append(p); }
    void append(colonlist* cl) { value* p = new value(cl); append(p); }
    colonlist* copy();

//    colonlist& operator=(const colonlist& x) {}
//    colonlist(const colonlist& x) {};
    colonlist() {}
    ~colonlist() { clear(); }
    }; // End of struct colonlist.

#endif /* AKSL_VALUE_H */
