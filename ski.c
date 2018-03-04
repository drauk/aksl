// src/aksl/ski.c   2017-10-26   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

string2key
string_to_index
key_to_string
opcode_print
strnglist::
    append
    prepend
    remove
    copy
    find
    merge
    print
    argv
    execute
ski2list::
    insert(ski2*)
    insert(const char*, long)
skilist::
    insert(ski*)
    insert(const char*, long)
    key(c_string, long&)
    key(const char*, long&)
    find(c_string&)
    find(const char*)
    find(ski*)
    find(skilist&)
    merge
    merge
    repeats
    max_i
    min_i
    print
    merge(c_stringlist&, skilist&);
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/ski.h"
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
// #if defined(SOLARIS) || defined(WIN32) || defined(linux)
#ifdef HAVE_LIMITS_H
#ifndef AKSL_X_LIMITS_H
#define AKSL_X_LIMITS_H
#include <limits.h>
#endif

#elif defined(HAVE_SYS_LIMITS_H)

#ifndef AKSL_X_SYS_LIMITS_H
#define AKSL_X_SYS_LIMITS_H
#include <sys/limits.h>
#endif
#endif /* HAVE_LIMITS_H */

// Kludge from cygnus limits.h file
#if defined(WIN32) && defined(__GNUC__)
#define __LONG_MAX__ 2147483647L
#define LONG_MIN (-LONG_MAX-1)
#define LONG_MAX __LONG_MAX__
#endif

#ifdef HAVE_UNISTD_H
#ifndef AKSL_X_UNISTD_H
#define AKSL_X_UNISTD_H
#include <unistd.h>
#endif
#endif

// #include <sys/wait.h>
extern "C" {
//  This declaration was required on emu, but is erroneous on goose.
//  Clashes with /usr/include/unistd.h line 508.
//    int execvp(const char*, const char*[]);
#if !defined(WIN32)
    int fork();
    int execlp(const char*, const char*, ...);
    int wait(int*);
#endif
    }

/*------------------------------------------------------------------------------
Returns a negative number if the string is not found. Hence only non-negative
numbers should be used in the table. The table must be terminated by a
null char pointer. The returned value is the key in the table.
------------------------------------------------------------------------------*/
//----------------------//
//      string2key      //
//----------------------//
long string2key(const stringkey* p, const char* pc) {
    if (!p || !pc)
        return -1;
    for (register const stringkey* q = p; q->s; ++q)
        if (strcmp(q->s, pc) == 0)
            return q->i;
    return -1;
    } // End of function string2key.

/*------------------------------------------------------------------------------
Returns a negative number if the string is not found. Hence only non-negative
numbers should be used in the table. The table must be terminated by a
null char pointer. The returned value is the array index.
------------------------------------------------------------------------------*/
//----------------------//
//    string_to_index   //
//----------------------//
long string_to_index(const stringkey* p, const char* pc) {
    if (!p || !pc)
        return -1;
    register long idx = 0;
    for (register const stringkey* q = p; q->s; ++q, ++idx)
        if (strcmp(q->s, pc) == 0)
            return idx;
    return -1;
    } // End of function string_to_index.

/*------------------------------------------------------------------------------
Returns the first string with the same key as that given. The null string is
returned if the integer is not found.
------------------------------------------------------------------------------*/
//----------------------//
//     key_to_string    //
//----------------------//
const char* key_to_string(const stringkey* p, long i) {
    if (!p)
        return 0;
    for (register const stringkey* q = p; q->s; ++q)
        if (q->i == i)
            return q->s;
    return 0;
    } // End of function key_to_string.

//----------------------//
//     opcode_print     //
//----------------------//
void opcode_print(const stringkey* psk, int opcode, ostream& os) {
    const char* pc = key_to_string(psk, opcode);
    if (pc) {
        os << "\"" << pc << "\"";
        return;
        }
    os << "[undefined opcode " << opcode << "]";
    } // End of function opcode_print.

/*------------------------------------------------------------------------------
strnglist::append appends a string to a strnglist.
------------------------------------------------------------------------------*/
//--------------------------//
//    strnglist::append     //
//--------------------------//
void strnglist::append(const char* s) {
    if (!s)
        return;
    strng* pl = new strng;
    pl->s = new_strcpy(s);
    append(pl);
    } // End of function strnglist::append.

/*------------------------------------------------------------------------------
strnglist::prepend inserts a string at the beginning of a strnglist.
------------------------------------------------------------------------------*/
//--------------------------//
//    strnglist::prepend    //
//--------------------------//
void strnglist::prepend(const char* s) {
    if (!s)
        return;
    strng* pl = new strng;
    pl->s = new_strcpy(s);
    prepend(pl);
    } // End of function strnglist::prepend.

/*------------------------------------------------------------------------------
strnglist::remove(char*) removes the first copy of a string from a list.
------------------------------------------------------------------------------*/
//----------------------//
//   strnglist::remove  //
//----------------------//
void strnglist::remove(const char* s) {
    if (!s)
        return;
    Forall(strng, p, *this)
        if (strcmp(s, p->s) == 0) {
            delremove(p);
            break;
            }
    } // End of function strnglist::remove.

/*------------------------------------------------------------------------------
strnglist::copy() returns a new copy of the current list.
------------------------------------------------------------------------------*/
//----------------------//
//   strnglist::copy    //
//----------------------//
strnglist* strnglist::copy() {
    strnglist* sl = new strnglist;
    Forall(strng, p, *this)
        sl->append(p->s);
    return sl;
    } // End of function strnglist::copy.

/*------------------------------------------------------------------------------
strnglist::find() returns a pointer to a list element matching the given string,
or null, if there is no copy of it.
------------------------------------------------------------------------------*/
//----------------------//
//   strnglist::find    //
//----------------------//
strng* strnglist::find(const char* s) {
    if (!s)
        return 0;
    strng* p = 0;
    forall(p, *this)
        if (p->s && strcmp(s, p->s) == 0)
            break;
    return p;
    } // End of function strnglist::find.

/*------------------------------------------------------------------------------
strnglist::merge() merges in the contents of a given list of strings.
------------------------------------------------------------------------------*/
//----------------------//
//   strnglist::merge   //
//----------------------//
void strnglist::merge(strnglist* sl) {
    if (!sl)
        return;
    Forall(strng, p, *sl)
        if (p->s && !find(p->s))
            append(p->s);
    } // End of function strnglist::merge.

/*------------------------------------------------------------------------------
strnglist::print() prints the current list of strings to a stream.
------------------------------------------------------------------------------*/
//----------------------//
//   strnglist::print   //
//----------------------//
void strnglist::print(ostream& os, char* s) {
    Forall(strng, p, *this)
        if (p->s) {
            os << p->s;
            if (p->next())
                os << (s ? s : " ");
            }
    } // End of function strnglist::print.

/*------------------------------------------------------------------------------
strnglist::argv() returns an array that can be used as an argv. In other words,
an array is returned which contains pointers to strings, each one of which is
copied from the current contents of the strnglist. The array is null-terminated.
------------------------------------------------------------------------------*/
//----------------------//
//    strnglist::argv   //
//----------------------//
char** strnglist::argv() {
    int argc = (int)length();
    char** v = new char*[argc + 1];
    char** ppc = v;
    Forall(strng, ps, *this) {
        char* s = new_strcpy_nz(ps->s);
        *ppc++ = s;
        }
    *ppc = 0; // Null-terminate the array of pointers to characters.
    return v;
    } // End of function strnglist::argv.

/*------------------------------------------------------------------------------
strnglist::execute() executes a command line as a child process, which it
waits for.
------------------------------------------------------------------------------*/
//----------------------//
//  strnglist::execute  //
//----------------------//
void strnglist::execute() {
    // Print the command string:
    print(cout);
    cout << NL;
    cout.flush();
    char** v = argv();
#ifdef WIN32
    execvp(*v, (const char**)v);
#else
    if (fork())                 // Parent.
        wait((int*)0);
    else {                      // Child.
//        execvp(*v, (const char**)v);
        execvp(*v, v);
        }
#endif
    } // End of function strnglist::execute.

/*------------------------------------------------------------------------------
ski2list::insert(ski2*) inserts a "ski2" into a ski2list in non-decreasing
order of i.
------------------------------------------------------------------------------*/
//--------------------------//
//     ski2list::insert     //
//--------------------------//
void ski2list::insert(ski2* pski2) {
    if (!pski2)
        return;
    register long i = pski2->i;
    register ski2* prev = 0;
    Forall(ski2, p, *this) {
        if (i < p->i)
            break;
        prev = p;
        }
    insertafter(prev, pski2);   // Insert pski2 after prev.
    } // End of function ski2list::insert.

/*------------------------------------------------------------------------------
ski2list::insert(const char*, int) inserts a string-key pair into a ski2list, in
non-decreasing order of i.
------------------------------------------------------------------------------*/
//--------------------------//
//     ski2list::insert     //
//--------------------------//
void ski2list::insert(const char* s, long i) {
    if (!s)
        return;
    ski2* pl = new ski2;
    pl->s = new_strcpy(s);
    pl->i = i;
    insert(pl);
    } // End of function ski2list::insert.

//--------------------------//
//     skilist::skilist     //
//--------------------------//
skilist::skilist(const stringkey* p) {
    if (!p)
        return;
    for (const stringkey* q = p; q->s; ++q)
        insert(q->s, q->i);
    } // End of function skilist::skilist.

//--------------------------//
//    skilist::operator=    //
//--------------------------//
skilist& skilist::operator=(const stringkey* p) {
    clear();
    if (p)
        for (const stringkey* q = p; q->s; ++q)
            insert(q->s, q->i);
    return *this;
    } // End of function skilist::operator=(stringkey*).

/*------------------------------------------------------------------------------
skilist::insert(ski*) inserts a "ski" into a skilist in
non-decreasing order of i.
------------------------------------------------------------------------------*/
//--------------------------//
//      skilist::insert     //
//--------------------------//
void skilist::insert(ski* pski) {
    if (!pski)
        return;
    register long i = pski->i;
    register ski* prev = 0;
    Forall(ski, p, *this) {
        if (i < p->i)
            break;
        prev = p;
        }
    insertafter(prev, pski);   // Insert pski after prev.
    } // End of function skilist::insert(ski*).

/*------------------------------------------------------------------------------
skilist::insert(const char*, long) inserts a string-key pair into a skilist, in
non-decreasing order of i.
------------------------------------------------------------------------------*/
//--------------------------------------//
//   skilist::insert(const char*, long) //
//--------------------------------------//
void skilist::insert(const char* s, long i) {
    if (!s)
        return;
    ski* pl = new ski;
    pl->s = s;              // Actually makes a copy.
    pl->i = i;
    insert(pl);
    } // End of function skilist::insert(const char*, long).

/*------------------------------------------------------------------------------
skilist::key() finds the key of the first occurence of the string in the list.
If the string is not found, then "false" is returned. Otherwise "true"
is returned.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::key     //
//----------------------//
bool_enum skilist::key(const c_string& cs, long& key) {
    ski* p = 0;
    forall (p, *this)
        if (p->s == cs)
            break;
    if (!p)
        return false;
    key = p->i;
    return true;
    } // End of function skilist::key.

/*------------------------------------------------------------------------------
skilist::key() finds the key of the first occurence of the string in the list.
If the string is found, then the corresponding key is returned. Otherwise,
-1 is returned. This function should therefore only be used when it is known
that the string is actually in the list.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::key     //
//----------------------//
long skilist::key(const c_string& cs) {
    ski* p = 0;
    forall (p, *this)
        if (p->s == cs)
            break;
    return p ? p->i : -1;
    } // End of function skilist::key.

/*------------------------------------------------------------------------------
skilist::key() finds the key of the first occurence of the string in the list.
If the string is not found, then "false" is returned. Otherwise "true"
is returned.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::key     //
//----------------------//
bool_enum skilist::key(const char* s, long& key) {
    ski* p = 0;
    forall (p, *this)
        if (p->s == s)
            break;
    if (!p)
        return false;
    key = p->i;
    return true;
    } // End of function skilist::key.

/*------------------------------------------------------------------------------
skilist::string(): If the given key appears in this list, then a pointer to
the first element of this list with the key is returned.
------------------------------------------------------------------------------*/
//----------------------//
//    skilist::string   //
//----------------------//
bool_enum skilist::string(long key, c_string& string) {
    ski* p = 0;
    forall(p, *this)
        if (p->i == key)
            break;
    if (!p)
        return false;
    string = p->s;
    return true;
    } // End of function skilist::string.

/*------------------------------------------------------------------------------
skilist::find(): If the given string appears in this list, then a pointer to
the first element of this list with the string is returned. Do not delete it
before removing it.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::find    //
//----------------------//
ski* skilist::find(const c_string& cs) {
    ski* p = 0;
    forall(p, *this)
        if (p->s == cs)
            break;
    return p;
    } // End of function skilist::find.

/*------------------------------------------------------------------------------
skilist::find(): If the given string appears in this list, then a pointer to
the first element of this list with the string is returned. Do not delete it
before removing it.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::find    //
//----------------------//
ski* skilist::find(const char* s) {
    ski* p = 0;
    forall(p, *this)
        if (p->s == s)
            break;
    return p;
    } // End of function skilist::find.

/*------------------------------------------------------------------------------
skilist::find(): If the given key appears in this list, then a pointer to
the first element of this list with the key is returned. Do not delete it
before removing it.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::find    //
//----------------------//
ski* skilist::find(long key) {
    ski* p = 0;
    forall(p, *this)
        if (p->i == key)
            break;
    return p;
    } // End of function skilist::find.

/*------------------------------------------------------------------------------
skilist::find(): If the string in the given "ski" appears in this list, then a
pointer to the first element of this list with the string is returned. If the
given "ski" is null, then null is returned.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::find    //
//----------------------//
ski* skilist::find(ski* pski) {
    if (!pski)
        return 0;
    ski* p = 0;
    forall(p, *this)
        if (p->s == pski->s)
            break;
    return p;
    } // End of function skilist::find.

/*------------------------------------------------------------------------------
skilist::find(): If there is a string which appears in both this list and the
given list, then a pointer to the first element of this list with this
string is returned.
------------------------------------------------------------------------------*/
//----------------------//
//     skilist::find    //
//----------------------//
ski* skilist::find(skilist& sl) {
    ski* p1 = 0;
    forall(p1, *this) {
        Forall(ski, p2, sl)
            if (p1->s == p2->s)
                break;
        }
    return p1;
    } // End of function skilist::find.

/*------------------------------------------------------------------------------
skilist::merge() adds a copy of the given "ski" to this list
if an only if there is no copy of the corresponding string in this list.
The resulting list maintains non-decreasing order, as if the string had been
inserted by a direct call to "insert".
The return value is the new value of this list.
------------------------------------------------------------------------------*/
//----------------------//
//    skilist::merge    //
//----------------------//
skilist& skilist::merge(ski* p2) {
    if (!p2)
        return *this;

    ski* prev = 0;
    Forall(ski, p1, *this) {
        if (p1->s == p2->s)
            break;
        prev = p1;
        }

    ski* p = new ski(*p2);
    insert(p);
    return *this;
    } // End of function skilist::merge.

/*------------------------------------------------------------------------------
skilist::merge() inserts a copy of the given "ski" into this list
if an only if there is no copy of the corresponding string in this list.
The resulting list maintains non-decreasing order, as if the string had been
inserted by a direct call to "insert".
The return value is the new value of this list.
------------------------------------------------------------------------------*/
//----------------------//
//    skilist::merge    //
//----------------------//
skilist& skilist::merge(skilist& sl) {
    // Alternative version:
//    Forall(ski, p, sl)
//        merge(p);

    // Explicitly inline version:
    Forall(ski, p2, sl) {
        ski* prev = 0;
        Forall(ski, p1, *this) {
            if (p1->s == p2->s)
                break;
            prev = p1;
            }
        ski* p = new ski(*p2);
        insert(p);
        }
    return *this;
    } // End of function skilist::merge.

//----------------------//
//   skilist::repeats   //
//----------------------//
bool_enum skilist::repeats() {
    Forall(ski, p, *this)
        for (ski* q = p->next(); q; q = q->next())
            if (p->s == q->s || p->i == q->i)
                return true;
    return false;
    } // End of function skilist::repeats.

//----------------------//
//    skilist::max_i    //
//----------------------//
long skilist::max_i() {
    long m = LONG_MIN;
    Forall(ski, p, *this)
        if (p->i > m)
            m = p->i;
    return m;
    } // End of function skilist::max_i.

//----------------------//
//    skilist::min_i    //
//----------------------//
long skilist::min_i() {
    long m = LONG_MAX;
    Forall(ski, p, *this)
        if (p->i < m)
            m = p->i;
    return m;
    } // End of function skilist::min_i.

//----------------------//
//    skilist::print    //
//----------------------//
void skilist::print(ostream& os, c_string& comment) {
    const int indexindent = 20;
    if (!comment.null())
        os << comment;
    os << NL;
    Forall(ski, p, *this) {
        int indent = indexindent - (int)(p->s.length());
        if (indent < 0)
            indent = 0;
//        os << p->s << str(" ", indent) << p->i << NL;
        os << p->s;
        for (int z = 0; z < indent; ++z)
            os.put(' ');
        os << p->i << NL;
        }
    } // End of function skilist::print.

/*------------------------------------------------------------------------------
This ad hoc function (yes, another one) reads the strings from the skilist
and makes copies of anything new to the c_stringlist. The integer keys of the
skilist are ignored.
------------------------------------------------------------------------------*/
//----------------------------------//
//  merge(c_stringlist&, skilist&)  //
//----------------------------------//
void merge(c_stringlist& csl, skilist& skl) {
    Forall(ski, p2, skl) {
        c_stringlink* p1 = 0;
        forall(p1, csl)
            if (*p1 == p2->s)
                break;
        if (!p1)
            csl.append(p2->s);
        }
    } // End of function merge(c_stringlist&, skilist&).
