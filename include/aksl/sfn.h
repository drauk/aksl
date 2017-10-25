// src/aksl/sfn.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/sfn.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

// Classes of trivial functions (tfns) and simple functions (sfns).
#ifndef AKSL_SFN_H
#define AKSL_SFN_H
/*------------------------------------------------------------------------------
Classes in this file:

tfn::
tfnlist::
sfn::
sfnlist::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif

/*------------------------------------------------------------------------------
This class represents a function whose value is zero except in the integer
interval [start, length).
"start" is in the range [-2^31, 2^31).
"length" is in the range [0, 2^32).
"value" is in the range [-2^31, 2^31).
The list of intervals should be maintained so that it represents
a non-decreasing pairwise disjoint sequence of non-empty intervals.
Wherever the value is not specified
by a tfn, the value is assumed to be 0.
The "value" members of tfns should always be non-zero.
The "length" members must always be non-zero.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
It might have been simpler to store members "min" and "max" indicating the
endpoints of each interval, which would have helped avoid overflow problems.
But the (start, length) representation can potentially be easily modified
to minimise memory by using, say, uint16 for the length.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
To avoid overflow problems, the inequality  length <= max_int32 - start + 1
must always be satisfied. Actually, it would have been a good idea to use the
length value 0 to represent the length 0x100000000, which would be the
length of the interval [min_int32, max_int32], but then the last element
of this interval would not have been representable. Hence a better compromise
might be to simple remove the number min_int32 from the number system.
This would make life even simpler. As it is now, the use of start == min_int32
could possibly create problems. And maybe there are problems at the max_int32
end of the line too. Please avoid using these extremes for now....

On this topic, even though the interval from min_int32 to max_int32 is no
permitted on entry to each function, it could arise as a result of the merging
of a new tfn with an old tfn via the sfn::set() function. Hence an error in the
list could arise. This could be avoided by checking for this on exit from the
sfn::set() and other functions -- or else length 0 could be taken to always mean
maximum length -- i.e. the semi-"infinite" interval from start up to max_int32.
------------------------------------------------------------------------------*/
//----------------------//
//         tfn::        //
//----------------------//
struct tfn: public slink {      // Represents [start, start + length).
    int32       start;          // Offset of first byte in the interval.
    uint32      length;         // Number of bytes in the interval.
    int32       value;          // Value within the interval.

    tfn* next() const { return (tfn*)slink::next(); }

//    tfn& operator=(const tfn& x) {}
//    tfn(const tfn& x) {}
    tfn(int32 s, uint32 l, int32 v) {
        // This function truncates the interval if it is too long.
        // If l == 0, then the interval is stretched to max_int32.
        start = s;
        if (l == 0 || l - 1 > uint32(max_int32 - s)) {
            length = max_int32 - s + 1; // Truncate to int32 range.
            if (length == 0) {          // Case of the entire line.
                start = s + 1;          // Remove min_int32 from the interval
                length = l - 1;         // to keep the peace.
                }
            }
        else
            length = l;
        value = v;
        }
    tfn() { start = 0; length = 0; value = 0; }
    ~tfn() {}
    }; // End of struct tfn.

/*------------------------------------------------------------------------------
This list should always be maintained as a non-decreasing list of
pairwise disjoint intervals of the integers.
------------------------------------------------------------------------------*/
//----------------------//
//       tfnlist::      //
//----------------------//
struct tfnlist: private s2list {
friend struct sfn;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    tfn* first() const { return (tfn*)s2list::first(); }
    tfn* last() const { return (tfn*)s2list::last(); }
protected:
    void append(tfn* p) { s2list::append(p); }
    void prepend(tfn* p) { s2list::prepend(p); }
    tfn* popfirst() { return (tfn*)s2list::popfirst(); }
    tfn* poplast() { return (tfn*)s2list::poplast(); }
    tfn* remove(tfn* p)
        { return (tfn*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(tfn* p) { delete remove(p); }
//    s2list::insertafter;
    void insertafter(tfn* p1, tfn* p2) { s2list::insertafter(p1, p2); }
    void swallow(tfnlist& l) { s2list::swallow(&l); }
    void gulp(tfnlist& l) { s2list::gulp(&l); }
    // Moved to .c file because of Sun C++ 3.0.1 compiler bug.
#ifdef SUNOS4_COMP_VERSION3_0_1
    void clear();
#else
    void clear() { for (tfn* p = first(); p; )
        { tfn* q = p->next(); delete p; p = q; } clearptrs(); }
#endif
    // Non-routine members:
    tfn* value_first(int32 v);  // Return first tfn with given value.
                                // If v == 0, return null pointer.
    tfn* container(int32 i);    // Return the unique tfn covering given index.
    tfn* next_container(int32 i); // Return tfn at or after given index.

    // Return tfn with value v at or after given index. (If v == 0, return 0.)
    tfn* next_container(int32 i, int32 v);

    // Return tfn with value != v at or after given index.
    // (If v == 0, this is the same as the 1-param unconditional version.)
    tfn* next_container_not(int32 i, int32 v);
public:
//    tfnlist& operator=(const tfnlist& x) {}
//    tfnlist(const tfnlist& x) {}
    tfnlist() {}
    ~tfnlist() { clear(); }
    }; // End of struct tfnlist.

/*------------------------------------------------------------------------------
This class represents a "simple function", which is a function that takes
on constant values in each of a specified list of finite intervals of the set of
integers, and is 0 elsewhere.
See member incorrect() for a precise statement of the tfn list constraints.
Another way of thinking of this is that an "sfn" represents a general
array of integers with index valued in int32, but it is optimised for
the situation that most array values are the same as their neighbours'.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The list "tfns" should not be altered by the user.
It is public only to facilitate traversal for purposes of reading it.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Probably a vastly simpler and better implementation of this class would be
in terms of a list of (start, value) pairs, where the value is understood to
be valid from p->start to p->next()->start - 1. The value 0 would be understood
from min_int32 to first()->start - 1. This would get rid of the overflow
problems, and the set() function would be vastly easier to write. The tfn
intervals would then be replaced effectively by semi-infinite intervals with a
given starting point, which are overridden by the following starting point in
the list. The tfn and tfnlist classes could still be used for representing
various subsets of the integer line, but maybe a specific interval class would
be better for this, with only (start, length) attributes, or (start, end)
attributes, where length = end - start, or length == 0 to represent the
semi-infinite interval.
------------------------------------------------------------------------------*/
//----------------------//
//          sfn::       //
//----------------------//
struct sfn: public slink {
    tfnlist     tfns;           // Please regard this list as constant.
private:
    // Functions for internal use:
    int carry_over(tfn* p1);
//    int carry_over(tfn* p2, int32 start, uint32 length, int32 value);
    int carry_over_zero(tfn* p2, int32 start, uint32 length);
    tfn* append(int32 start, uint32 len, int32 value);
    tfn* insertafter(tfn* p, int32 start, uint32 len, int32 value);
public:
    sfn* next() const { return (sfn*)slink::next(); }

    // Public functions:
    int set(int32 start, uint32 len, int32 value);
    int32 get(int32 i);                 // Return value at position i.
    tfn* first_tfn() { return tfns.first(); }
    tfn* value_first(int32 v)           // Return first tfn with given value.
        { return tfns.value_first(v); }
    tfn* container(int32 i)             // Return the unique tfn covering i.
        { return tfns.container(i); }
    tfn* next_container(int32 i)        // Return the tfn covering or after i.
        { return tfns.next_container(i); }
    tfn* next_container(int32 i, int32 v)       // Same, but with value v.
        { return tfns.next_container(i, v); }
    tfn* next_container_not(int32 i, int32 v)   // Same, but with value != v.
        { return tfns.next_container_not(i, v); }
    tfn* incorrect(int* reason = 0);    // First incorrect tfn in list, if any.
    int correct();      // Corrects errors. Returns number of errors found.
    void clear() { tfns.clear(); }      // Revert to the empty set indicator.
    void print(ostream& = cout);

//    sfn& operator=(const sfn& x) {}
//    sfn(const sfn& x) {}
    sfn() {}
    ~sfn() {}
    }; // End of struct sfn.

//----------------------//
//        sfnlist::     //
//----------------------//
struct sfnlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
//    sfn* first() const { return (sfn*)s2list::first(); }
    sfn* first() { return (sfn*)s2list::first(); }
    sfn* last() const { return (sfn*)s2list::last(); }
    void append(sfn* p) { s2list::append(p); }
    void prepend(sfn* p) { s2list::prepend(p); }
    sfn* popfirst() { return (sfn*)s2list::popfirst(); }
    sfn* poplast() { return (sfn*)s2list::poplast(); }
    sfn* remove(sfn* p)
        { return (sfn*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(sfn* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(sfnlist& l) { s2list::swallow(&l); }
    void gulp(sfnlist& l) { s2list::gulp(&l); }
    void clear() { for (sfn* p = first(); p; )
        { sfn* q = p->next(); delete p; p = q; } clearptrs(); }

//    sfnlist& operator=(const sfnlist& x) {}
//    sfnlist(const sfnlist& x) {}
    sfnlist() {}
    ~sfnlist() { clear(); }
    }; // End of struct sfnlist.

#endif /* AKSL_SFN_H */
