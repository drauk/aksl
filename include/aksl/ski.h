// src/aksl/ski.h   2018-3-4   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_SKI_H
#define AKSL_SKI_H
/*------------------------------------------------------------------------------
Classes defined in this file:

stringkey::
strng::
strnglist::
ski2::
ski2list::
ski::
skilist::
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_STR_H
#include "aksl/str.h"
#endif
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// System header files:
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif

//----------------------//
//      stringkey::     //
//----------------------//
struct stringkey {
//    char* s;
    const char* s;
    long i;
    }; // End of struct stringkey.

//----------------------//
//       strng::        //
//----------------------//
struct strng: slink {
    char* s;

    strng* next() { return (strng*)slink::next(); }

    strng() { s = 0; }
    ~strng() { delete[] s; }
    }; // End of struct strng.

//----------------------//
//      strnglist::     //
//----------------------//
struct strnglist: private s2list {
public:
    // The routine members:
    strng* first() const { return (strng*)s2list::first(); }
    strng* last() const { return (strng*)s2list::last(); }
    using s2list::empty;
    using s2list::length;
    void append(strng* p) { s2list::append(p); }
    void prepend(strng* p) { s2list::prepend(p); }
    strng* remove(strng* p) { return (strng*)s2list::remove(p); }
    strng* popfirst() { return (strng*)s2list::popfirst(); }
    strng* poplast() { return (strng*)s2list::poplast(); }
    void delremove(strng* p) { delete remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    using s2list::insertafter;
    void clear() { for (strng* p = first(); p; )
        { strng* q = p->next(); delete p; p = q; } clearptrs(); }

    void append(const char*);
    void prepend(const char*);
    void remove(const char*);
    strnglist* copy();      // Returns a new copy of a list.
    strng* find(const char*);   // Find a given string.
    void merge(strnglist*); // Merge in a list of strings.
    void print(ostream& = cout, char* = 0);
    char** argv();          // Returns an array that can be used as an argv.
    void execute();         // Executes the list as a command line.

    strnglist() {}
    ~strnglist() { clear(); }
    }; // End of struct strnglist.

/*------------------------------------------------------------------------------
A "ski2" is a string-key pair, linked into a list. The string representation
is the traditional char*-style.
------------------------------------------------------------------------------*/
//----------------------//
//        ski2::        //
//----------------------//
struct ski2: public slink {
    char* s;
    long i;

    ski2* next() { return (ski2*)slink::next(); }

    ski2() { s = 0; i = 0; }
    ~ski2() { delete[] s; }
    }; // End of struct ski2.

//----------------------//
//       ski2list::     //
//----------------------//
struct ski2list: private s2list {
private:
    // The routine members:
    void append(ski2* p) { s2list::append(p); }
    void prepend(ski2* p) { s2list::prepend(p); }
//    s2list::insertafter;
public:
    ski2* first() const { return (ski2*)s2list::first(); }
    ski2* last() const { return (ski2*)s2list::last(); }
    using s2list::empty;
    using s2list::length;
    ski2* remove(ski2* p) { return (ski2*)s2list::remove(p); }
    ski2* popfirst() { return (ski2*)s2list::popfirst(); }
    ski2* poplast() { return (ski2*)s2list::poplast(); }
    void delremove(ski2* p) { delete remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void clear() { for (ski2* p = first(); p; )
        { ski2* q = p->next(); delete p; p = q; } clearptrs(); }

    void insert(ski2*);             // Insert ski2 in non-decr order of i.
    void insert(const char*, long); // Insert ski2 in non-decr order of i.
    void insert(long i, const char* s) { insert(s, i); }

    long index(const char*); // Returns the (lowest) index of a string.
    void remove(char*);
    ski2list* copy();      // Returns a new copy of a list.
    ski2* find(char*);     // Find a given string.
    void merge(ski2list*); // Merge in a list of strings.
    void print(ostream&, char* = 0);

    ski2list() {}
    ~ski2list() { clear(); }
    }; // End of struct ski2list.

/*------------------------------------------------------------------------------
A "ski" is a string-key pair, linked into a list. The string representation
is not the traditional char*-style. Class "c_string" is used instead.
It is expected that all key values will be non-negative. But all algorithms
work for signed integer keys. The member "i" could have been made unsigned
to enforce non-negativity. This could be done in another class definition!
Null strings and zero-length strings are tolerated.
------------------------------------------------------------------------------*/
//----------------------//
//         ski::        //
//----------------------//
struct ski: public slink {
    c_string s;
    long i;

    ski* next() { return (ski*)slink::next(); }

    ski() { i = 0; }
    ski(ski& sk) { s = sk.s; i = sk.i; }
    ski& operator=(ski& sk) { s = sk.s; i = sk.i; return *this; }
    ~ski() {}
    }; // End of struct ski.

/*------------------------------------------------------------------------------
skilist:: If possible, there should be no repeat strings or repeat keys in the
list. But this is not enforced in any way. Non-decreasing order with respect
to the key "i" is always maintained.
------------------------------------------------------------------------------*/
//----------------------//
//       skilist::      //
//----------------------//
struct skilist: private s2list {
private:
    // The routine members:
    void append(ski* p) { s2list::append(p); }
    void prepend(ski* p) { s2list::prepend(p); }
//    s2list::insertafter;

    void insert(ski*);             // Insert ski in non-decr order of i.
public:
    // More routine members:
    ski* first() const { return (ski*)s2list::first(); }
    ski* last() const { return (ski*)s2list::last(); }
    using s2list::empty;
    using s2list::length;
    ski* remove(ski* p) { return (ski*)s2list::remove(p); }
    ski* popfirst() { return (ski*)s2list::popfirst(); }
    ski* poplast() { return (ski*)s2list::poplast(); }
    void delremove(ski* p) { delete remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void clear() { for (ski* p = first(); p; )
        { ski* q = p->next(); delete p; p = q; } clearptrs(); }

    void insert(const char*, long);  // Insert string in non-decr order of i.
    void insert(long i, const char* s) { insert(s, i); }
    bool_enum key(const c_string&, long&); // Finds (lowest) key of a string.
    long key(const c_string&);          // Finds the (lowest) key of a string.
    bool_enum key(const char*, long&);  // Finds the (lowest) key of a string.
    bool_enum string(long, c_string&);  // Finds (first) string of a key.
    ski* find(const char*);             // Find a given string.
    ski* find(const c_string&);
    ski* find(long);                    // Find a string with a given key.
    ski* find(ski*);
    ski* find(skilist&);
    skilist& merge(ski*);
    skilist& merge(skilist&);
    bool_enum repeats();                // True if a string or key is repeated.

    void remove(char*);
    skilist* copy();                // Returns a new copy of a list.
    long count() { return length(); }
    long max_i();
    long min_i();
    void print(ostream&, c_string&);

    skilist& operator=(const stringkey*); // Assign from a stringkey table.
    skilist(const stringkey*);    // Initialize from a table of strings/keys.

    skilist() {}
    ~skilist() { clear(); }
    }; // End of struct skilist.

// Exported functions:
// extern long string_to_key(const stringkey*, const char*);
extern long string2key(const stringkey*, const char*);
extern long string_to_index(const stringkey*, const char*);
extern const char* key_to_string(const stringkey*, long i);
extern void opcode_print(const stringkey* psk, int opcode, ostream& = cout);
extern void merge(c_stringlist&, skilist&);

#endif /* AKSL_SKI_H */
