// src/aksl/oral.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_ORAL_H
#define AKSL_ORAL_H
/*------------------------------------------------------------------------------
ORAL = Object Relationship Attribute Language.

Classes in this file:

avaluelist::
acolonlist::
avalue::
attr::
attrlist::
item::
itemlist::
itemblock::
itemptr::
itemptrlist::
oralsystem::
oralsystemlist::
oraldata::
------------------------------------------------------------------------------*/

#ifndef AKSL_TOKEN_H
#include "aksl/token.h"
#endif
#ifndef AKSL_ERROR_H
#include "aksl/error.h"
#endif

// Categories of values.
enum avaluetype {
    avNULL,                 // Undefined category.
    avSTRING,               // Character string.
    avINTEGER,              // Integer.
    avREAL,                 // Real number.
    avREF,                  // Pointer/reference to an item/object.
    avLIST,                 // List of avalues.
    avBLOCK,                // Block/list of tagged/named attribute values.
    avCOLONLIST             // Colon-separated list of avalues.
    };

// Forward declarations of classes.
struct avalue;
struct attrlist;
struct item;
struct itemlist;
struct itemblock;
struct object;

//----------------------//
//      avaluelist::    //
//----------------------//
struct avaluelist: public s2list {
public:
    s2list::empty;
    s2list::length;
    avalue* first() const { return (avalue*)s2list::first(); }
    avalue* last() const { return (avalue*)s2list::last(); }
    void append(avalue* p) { s2list::append((slink*)p); }
    void prepend(avalue* p) { s2list::prepend((slink*)p); }
    avalue* popfirst() { return (avalue*)s2list::popfirst(); }
    avalue* poplast() { return (avalue*)s2list::poplast(); }
//    void delfirst() { delete popfirst(); }
//    void dellast() { delete poplast(); }
    inline void delfirst();
    inline void dellast();
    s2list::insertafter;
    inline void clear();

    int read(token_stream&);
    int read(istream& is) { token_stream ts(is); return read(ts); }
    void print(ostream&, int = 0, const char* = 0);

    avaluelist() {}
    ~avaluelist() { clear(); }
    }; // End of struct avaluelist.

/*------------------------------------------------------------------------------
This is the same as an avaluelist, except that the syntax is colon-separated
with no parentheses, and the semantics is slightly extended.
------------------------------------------------------------------------------*/
//----------------------//
//      acolonlist::    //
//----------------------//
struct acolonlist: public s2list {
public:
    s2list::empty;
    s2list::length;
    avalue* first() const { return (avalue*)s2list::first(); }
    avalue* last() const { return (avalue*)s2list::last(); }
    void append(avalue* p) { s2list::append((slink*)p); }
    void prepend(avalue* p) { s2list::prepend((slink*)p); }
    avalue* popfirst() { return (avalue*)s2list::popfirst(); }
    avalue* poplast() { return (avalue*)s2list::poplast(); }
//    void delfirst() { delete popfirst(); }
//    void dellast() { delete poplast(); }
    inline void delfirst();
    inline void dellast();
    s2list::insertafter;
    inline void clear();

    int read(token_stream&);
    int read(istream& is) { token_stream ts(is); return read(ts); }
    void print(ostream&, int = 0, const char* = 0);

    acolonlist() {}
    ~acolonlist() { clear(); }
    }; // End of struct acolonlist.

//----------------------//
//        avalue::      //
//----------------------//
struct avalue: public slink {
    enum avaluetype type;
    unsigned long line_number;  // Line number in input stream.
    c_string s_refname;         // Actually the union of "s" and "refname".
    union {
        long        i;
        double      r;
        item*       itm;
        avaluelist* l;          // List of attribute values.
        attrlist*   blk;        // Block of tagged attribute values.
        acolonlist* cl;         // Colon-separated list of attribute values.
        };

    avalue* next() { return (avalue*)slink::next(); }
    int read(token_stream&, bool_enum ignore_colon = false);
    int read(istream& is) { token_stream ts(is); return read(ts); }
    void print(ostream&, int = 0, const char* = 0);
    bool_enum resolvenames(itemlist&);
    avalue* clearout();   // Move this avalue into a new avalue.

    // Should have a wider range of read-access functions here.
    operator double();
    operator long();

    // Tests for various types of avalue.
    bool_enum Integer() const { return (bool_enum)(type == avINTEGER); }
    bool_enum IntegerColon() const; // Either integer or colon-list integers.
    bool_enum Real() const { return (bool_enum)(type == avREAL); }
    bool_enum Number() const
        { return (bool_enum)(type == avINTEGER || type == avREAL); }
    bool_enum String() const { return (bool_enum)(type == avSTRING); }
    bool_enum Ref() const { return (bool_enum)(type == avREF); }
    bool_enum List() const { return (bool_enum)(type == avLIST); }
    bool_enum Block() const { return (bool_enum)(type == avBLOCK); }
    bool_enum Colonlist() const { return (bool_enum)(type == avCOLONLIST); }

    avalue(unsigned long ln = 0) { type = avNULL; line_number = ln; }
    ~avalue();
    }; // End of struct avalue.

//----------------------//
//        attr::        //
//----------------------//
struct attr: public slink { // An attribute of an item.
    c_string name;
    avalue* a;

    attr* next() { return (attr*)slink::next(); }

    bool_enum resolvenames(itemlist& p)
        { return a ? a->resolvenames(p) : true; }

    attr() { a = 0; }
    ~attr() { delete a; }
    }; // End of struct attr.

//----------------------//
//       attrlist::     //
//----------------------//
struct attrlist: private s2list {
public:
    attr* first() const { return (attr*)s2list::first(); }
    attr* last() const { return (attr*)s2list::last(); }
    s2list::empty;
    s2list::length;
    void append(attr* p) { s2list::append(p); }
    void prepend(attr* p) { s2list::prepend(p); }
    attr* popfirst() { return (attr*)s2list::popfirst(); }
    attr* poplast() { return (attr*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    s2list::insertafter;
    void clear() { for (attr* p = first(); p; )
        { attr* q = p->next(); delete p; p = q; } clearptrs(); }

    int read(token_stream&);
    int read(istream& is) { token_stream ts(is); return read(ts); }
    void print(ostream&, int = 0, const char* = 0);

    attrlist() {}
    ~attrlist() { clear(); }
    }; // End of struct attrlist.

/*------------------------------------------------------------------------------
An item represents an object of a particular class.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note:
To implement sequential assignment of globvars and object attributes, should
generalize "item" to include globvars. Then the globvar assignments and object
attribute assignments can be effected in the same order as in the system file.
------------------------------------------------------------------------------*/
//----------------------//
//        item::        //
//----------------------//
struct item: public slink {
    c_string classname; // The class of the item.
    c_string name;      // The name given to the item.
    attrlist attrs;     // Information read from a data file.
    void*  obj;         // The object created for this item.

    item* next() { return (item*)slink::next(); }
    void print(ostream&, int = 0, const char* = 0);

    item() { obj = 0; }
    ~item() {}
    }; // End of struct item.

//----------------------//
//       itemlist::     //
//----------------------//
struct itemlist: private s2list {
public:
    item* first() const { return (item*)s2list::first(); }
    item* last() const { return (item*)s2list::last(); }
    s2list::empty;
    s2list::length;
    void append(item* p) { s2list::append(p); }
    void prepend(item* p) { s2list::prepend(p); }
    item* popfirst() { return (item*)s2list::popfirst(); }
    item* poplast() { return (item*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    s2list::insertafter;
    void clear() { for (item* p = first(); p; )
        { item* q = p->next(); delete p; p = q; } clearptrs(); }

    item* findname(c_string&);

    itemlist() {}
    ~itemlist() { clear(); }
    }; // End of struct itemlist.

/*------------------------------------------------------------------------------
This class represents the set of all items and global variables in a system.
------------------------------------------------------------------------------*/
//----------------------//
//      itemblock::     //
//----------------------//
struct itemblock {
    attrlist        globvars;           // The global variables in the block.
    itemlist        items;              // The objects/items in a block.
    unsigned long   anon_item_count;    // Number of anonymous items found.

    // For backwards compatibility.
    item* first() const { return items.first(); }

    int         read(token_stream&);
    void        print(ostream&, int = 0, const char* = 0);
    item*       findname(c_string&);
    int         resolvenames();

//    itemblock& operator=(const itemblock& x) {}
//    itemblock(const itemblock& x) {};
    itemblock() { anon_item_count = 0; }
    ~itemblock() {}
    }; // End of struct itemblock.

//----------------------//
//        itemptr::     //
//----------------------//
struct itemptr: public slink {
    item* p;

    itemptr* next() const { return (itemptr*)slink::next(); }

//    itemptr& operator=(const itemptr& x) {}
//    itemptr(const itemptr& x) {};
    itemptr() { p = 0; }
    ~itemptr() {}
    }; // End of struct itemptr.

//----------------------//
//     itemptrlist::    //
//----------------------//
struct itemptrlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    itemptr* first() const { return (itemptr*)s2list::first(); }
    itemptr* last() const { return (itemptr*)s2list::last(); }
    void append(itemptr* p) { s2list::append(p); }
    void prepend(itemptr* p) { s2list::prepend(p); }
    itemptr* popfirst() { return (itemptr*)s2list::popfirst(); }
    itemptr* poplast() { return (itemptr*)s2list::poplast(); }
    itemptr* remove(itemptr* p)
        { return (itemptr*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(itemptr* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(itemptrlist& l) { s2list::swallow(&l); }
    void gulp(itemptrlist& l) { s2list::gulp(&l); }
    void clear() { for (itemptr* p = first(); p; )
        { itemptr* q = p->next(); delete p; p = q; } clearptrs(); }

    // Non-routine members.
    itemptr* find(item*);       // Return null if not in list.
    itemptr* append(item*);     // Append pointer to given item.
    void print(item* = 0, ostream& = cout);

//    itemptrlist& operator=(const itemptrlist& x) {}
//    itemptrlist(const itemptrlist& x) {};
    itemptrlist(item* pi) { append(pi); }
    itemptrlist() {}
    ~itemptrlist() { clear(); }
    }; // End of struct itemptrlist.

//----------------------//
//     oralsystem::     //
//----------------------//
struct oralsystem: public slink {
    c_string    name;       // The name given to the system in the file.
    itemblock   items;      // The set of all items and globvars in the system.

    oralsystem* next() { return (oralsystem*)slink::next(); }
    void print(ostream&, int = 0, const char* = 0);

    oralsystem() {}
    ~oralsystem() {}
    }; // End of struct oralsystem.

//----------------------//
//    oralsystemlist::  //
//----------------------//
struct oralsystemlist: private s2list {
public:
    oralsystem* first() const { return (oralsystem*)s2list::first(); }
    oralsystem* last() const { return (oralsystem*)s2list::last(); }
    s2list::empty;
    s2list::length;
    void append(oralsystem* p) { s2list::append(p); }
    void prepend(oralsystem* p) { s2list::prepend(p); }
    oralsystem* popfirst() { return (oralsystem*)s2list::popfirst(); }
    oralsystem* poplast() { return (oralsystem*)s2list::poplast(); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    s2list::insertafter;
    void clear() { for (oralsystem* p = first(); p; )
        { oralsystem* q = p->next(); delete p; p = q; } clearptrs(); }

    int read(istream&);
    void print(ostream&, int = 0, const char* = 0);
    oralsystem* getsystem(c_string&);     // Find or create a system.

    oralsystemlist() {}
    ~oralsystemlist() { clear(); }
    }; // End of struct oralsystemlist.

//----------------------//
//       oraldata::     //
//----------------------//
struct oraldata: public slink {
public:
    oralsystemlist oslist;      // List of systems in the data.
    strnglist packagenames;     // List of packages required for the systems.
    int readfile(c_string&, int = 0, c_stringlist* = 0);
                                // Read a file into the structure.
    oraldata() {}
    ~oraldata() {}
    }; // End of struct oraldata.

/*------------------------------------------------------------------------------
Inline functions for classes "avalue" and "avaluelist".
[These are here because of compiler bugs. I forget which compiler had the bugs.]
------------------------------------------------------------------------------*/
//- - - - - - - - - - - //
//     avaluelist::     //
//- - - - - - - - - - - //
inline void avaluelist::delfirst() { delete popfirst(); }
inline void avaluelist::dellast() { delete poplast(); }
inline void avaluelist::clear() {
    for (avalue* p = first(); p; )
        { avalue* q = p->next(); delete p; p = q; }
    clearptrs();
    } // End of function avaluelist::clear.

//- - - - - - - - - - - //
//     acolonlist::     //
//- - - - - - - - - - - //
inline void acolonlist::delfirst() { delete popfirst(); }
inline void acolonlist::dellast() { delete poplast(); }
inline void acolonlist::clear() {
    for (avalue* p = first(); p; )
        { avalue* q = p->next(); delete p; p = q; }
    clearptrs();
    } // End of function acolonlist::clear.

#endif /* AKSL_ORAL_H */
