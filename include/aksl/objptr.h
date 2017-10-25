// src/aksl/objptr.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/objptr.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_OBJPTR_H
#define AKSL_OBJPTR_H
/*------------------------------------------------------------------------------
Classes in this file:

objptr::
objptrlist::
objcat::
objcatlist::
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_AKSL_H
#include "aksl/aksl.h"
#endif
#ifndef AKSL_STR_H
#include "aksl/str.h"
#endif
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

struct object;   // Externally defined.

/*------------------------------------------------------------------------------
Probably should derive "objptr" from "ref".
------------------------------------------------------------------------------*/
//----------------------//
//       objptr::       //
//----------------------//
struct objptr: public slink {
    object* dest;

    objptr* next() const { return (objptr*)slink::next(); }

    operator object*() const { return dest; }

    objptr() { dest = 0; }
    ~objptr() {}
    }; // End of struct objptr.

//----------------------//
//      objptrlist::    //
//----------------------//
struct objptrlist: private s2list {
protected:
    s2list::clearptrs;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    objptr* first() const { return (objptr*)s2list::first(); }
    objptr* last() const { return (objptr*)s2list::last(); }
    void append(objptr* p) { s2list::append(p); }
    void prepend(objptr* p) { s2list::prepend(p); }
    objptr* popfirst() { return (objptr*)s2list::popfirst(); }
    objptr* poplast() { return (objptr*)s2list::poplast(); }
    objptr* remove(objptr* p) { return (objptr*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(objptr* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(objptrlist& l) { s2list::swallow(&l); }
    void clear() { for (objptr* p = first(); p; )
        { objptr* q = p->next(); delete p; p = q; } clearptrs(); }

    objptr* find(object*);      // Search for an object by address.
    objptr* add(object*);       // Append an objptr if it is not already there.
    objptr* append(object*);    // Append an objptr without checking if there.
    objptr* prepend(object*);   // Prepend an objptr without checking if there.
    void remove1(object*);      // Remove one copy of the given object.
    void removeall(object*);    // Remove all copies of the given object.
    void add(const objectlist&, const c_string&);
                                // Add objptrs of the given category.
    void copy(const objptrlist&);     // Clear list and copy the other list.

    objptrlist() {}
    ~objptrlist() { clear(); }
    }; // End of struct objptrlist.

/*------------------------------------------------------------------------------
objcat:: represents a category of objects. These objects all have the same
class.
------------------------------------------------------------------------------*/
//----------------------//
//       objcat::       //
//----------------------//
struct objcat: public slink {
friend struct objcatlist;
private:
    c_string type;
    objptrlist objs;
public:
    objcat* next() { return (objcat*)slink::next(); }

    void settype(c_string& s) {
        if (!s.null() && s != type) { clear(); type = s; } }
    void settype(const char* s) {
        if (notnullstr(s)) { clear(); type = s; } }
    void clear() { type.clear(); objs.clear(); }
    void add(objptrlist&);  // Make copies of objects with the correct class.
    void add(objectlist&);  // Make copies of objects with the correct class.
    void add(object* p) {   // Make a copy of an object if of the correct class.
        if (type == p->type()) objs.add(p); }

    objcat() {}
    ~objcat() {}
    }; // End of struct objcat.

/*------------------------------------------------------------------------------
Class objcatlist:: manages a set of object categories.
------------------------------------------------------------------------------*/
//----------------------//
//      objcatlist::    //
//----------------------//
struct objcatlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    objcat* first() const { return (objcat*)s2list::first(); }
    objcat* last() const { return (objcat*)s2list::last(); }
    void append(objcat* p) { s2list::append(p); }
    void prepend(objcat* p) { s2list::prepend(p); }
    objcat* popfirst() { return (objcat*)s2list::popfirst(); }
    objcat* poplast() { return (objcat*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    s2list::insertafter;
    void clear() { for (objcat* p = first(); p; )
        { objcat* q = p->next(); delete p; p = q; } clearptrs(); }

    void add(objectlist&);      // Add all categories in the given list.
    void print(ostream& = cout);
//    objptrlist* findlist(const char* s); // Find a named list.
    objptrlist* findlist(c_string& s); // Find a named list.
//    objcat* find(object*);      // Search for an object.
//    void add(object*);          // Add an object if it is not already there.

    objcatlist() {}
    ~objcatlist() { clear(); }
    }; // End of struct objcatlist.

#endif /* AKSL_OBJPTR_H */
