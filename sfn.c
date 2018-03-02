// src/aksl/sfn.c   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/sfn.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

tfnlist::
    clear
    value_first
    container
    next_container
    next_container
    next_container_not
sfn::
    append
    insertafter
    carry_over
    carry_over_zero
    set
    get
    incorrect
    correct
    print
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/sfn.h"

enum sfn_error_t {
    sfnNONE,
    sfnLENGTH,
    sfnVALUE,
    sfnMONO,
    sfnOVERLAP,
    sfnTOUCH
    };

static const char* sfn_error_msg[] = {
    "none",
    "length == 0",
    "value == 0",
    "starts not strictly increasing",
    "overlapping intervals",
    "touching intervals with same value",
    (char*)0
    };

/*------------------------------------------------------------------------------
This function is not in the class definition because the Sun C++ 3.0.1
compiler generates incorrect C code output when it is there.
The compiler tries to generate a callable "clear" function, but it
uses the wrong pointer __2p instead of __0this for the list location.
------------------------------------------------------------------------------*/
//----------------------//
//    tfnlist::clear    //
//----------------------//
#ifdef SUNOS4_COMP_VERSION3_0_1
void tfnlist::clear() {
    for (tfn* p = first(); p; ) {
        tfn* q = p->next();
        delete p;
        p = q;
        }
    clearptrs();
    } // End of function tfnlist::clear.
#endif

/*------------------------------------------------------------------------------
This function is slow when v == 0. But then it always returns the null pointer
if the list is in a correct sfn condition.
------------------------------------------------------------------------------*/
//----------------------//
// tfnlist::value_first //
//----------------------//
tfn* tfnlist::value_first(int32 v) {
    tfn* p = 0;
    forall(p, *this)
        if (p->value == v)
            break;
    return p;
    } // End of function tfnlist::value_first.

/*------------------------------------------------------------------------------
This returns the (unique) first tfn in the list such that
p->start <= i < p->start + p->length.
------------------------------------------------------------------------------*/
//----------------------//
//  tfnlist::container  //
//----------------------//
tfn* tfnlist::container(int32 i) {
    tfn* p = 0;
    forall(p, *this) {
        if (p->start > i)
            return 0;
        // This gets around overflow problems.
        if (p->length > uint32(i - p->start))
            break;
        }
    return p;
    } // End of function tfnlist::container.

/*------------------------------------------------------------------------------
This returns the first tfn containing the index if there is one, otherwise
the tfn following i if there is one, or 0 if the list is empty or i is
to the right of all intervals.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Summary:
If p->start <= i < p->start + p->length for some p, then return p.
Otherwise, if i falls in a gap, where
p = q->next(), and
    q->start + q->length <= i < p->start,
then return p.
If i < first()->start, return first().
If i >= last()->start + last()->length, return 0.
------------------------------------------------------------------------------*/
//--------------------------//
//  tfnlist::next_container //
//--------------------------//
tfn* tfnlist::next_container(int32 i) {
    tfn* p = 0;
    forall(p, *this) {
        if (p->start > i)               // Return next tfn after the gap.
            break;
        // [The following line gets around overflow problems.]
        if (p->length > uint32(i - p->start))
            break;
        }
    return p;
    } // End of function tfnlist::next_container.

/*------------------------------------------------------------------------------
This returns the first tfn with value v containing the index if there is one,
otherwise the tfn with value v following i if there is one,
or 0 if the list is empty or i is to the right of all intervals with value v.
This is equivalent to the 1-parameter version, except that intervals with
values other than v are ignored as if they weren't in the list.
------------------------------------------------------------------------------*/
//--------------------------//
//  tfnlist::next_container //
//--------------------------//
tfn* tfnlist::next_container(int32 i, int32 v) {
    tfn* p = 0;
    forall(p, *this) {
        if (p->value != v)              // Ignore intervals with other values.
            continue;
        if (p->start > i)               // Return next tfn after the gap.
            break;
        // [The following line gets around overflow problems.]
        if (p->length > uint32(i - p->start))
            break;
        }
    return p;
    } // End of function tfnlist::next_container.

/*------------------------------------------------------------------------------
This returns the first tfn with value != v containing the index if there is one,
otherwise the tfn with value != v following i if there is one, or 0 if the list
is empty or i is to the right of all intervals with value != v.
This is equivalent to the 1-parameter version, except that intervals with
values equal to v are ignored as if they weren't in the list.
------------------------------------------------------------------------------*/
//------------------------------//
//  tfnlist::next_container_not //
//------------------------------//
tfn* tfnlist::next_container_not(int32 i, int32 v) {
    tfn* p = 0;
    forall(p, *this) {
        if (p->value == v)              // Ignore intervals with value v.
            continue;
        if (p->start > i)               // Return next tfn after the gap.
            break;
        // This gets around overflow problems.
        if (p->length > uint32(i - p->start))
            break;
        }
    return p;
    } // End of function tfnlist::next_container_not.

/*------------------------------------------------------------------------------
Function for internal use. Intervals should only be appended after checking
that the correctness of the list will be maintained.
For public use, a version of this function could be provided which checks
that the correctness of the list will not be lost if it is currently correct.
------------------------------------------------------------------------------*/
//----------------------//
//      sfn::append     //
//----------------------//
tfn* sfn::append(int32 start, uint32 len, int32 value) {
    if (len == 0 || value == 0)
        return 0;
    tfn* p = new tfn;
    p->start = start;
    p->length = len;
    p->value = value;
    tfns.append(p);
    return p;
    } // End of function sfn::append.

/*------------------------------------------------------------------------------
Function for internal use. Intervals should only be inserted after checking
that the correctness of the list will be maintained.
If "p1" is the null pointer, the new list element is prepended to the list.
For public use, a version of this function could be provided which checks
that the correctness of the list will not be lost if it is currently correct.
------------------------------------------------------------------------------*/
//----------------------//
//   sfn::insertafter   //
//----------------------//
tfn* sfn::insertafter(tfn* p1, int32 start, uint32 len, int32 value) {
    if (len == 0 || value == 0)
        return 0;
    tfn* p = new tfn;
    p->start = start;
    p->length = len;
    p->value = value;
    tfns.insertafter(p1, p);        // Always succeeds if p != 0.
    return p;
    } // End of function sfn::insertafter.

/*------------------------------------------------------------------------------
This routine is intended for internal use.
It is a specialized version of set(start, len, value) for the case that
p1 is already in the list with members (start, len, value),
but p1 may be overlapping the rest of the list from p1->next() onwards.
p1 is the tfn to be carried over the rest of the list starting at p1->next().
Return value:
0   success
-1  error
It is assumed that p1->start <= p1->next()->start if p1->next() != 0.
This means that p1->start == p2->start is possible!
More precisely, it is assumed that the tfn list without p1 would be a correct
list.
------------------------------------------------------------------------------*/
//----------------------//
//    sfn::carry_over   //
//----------------------//
int sfn::carry_over(tfn* p1) {
    // Check for some simple kinds of error:
    if (!p1)
        return -1;
    register tfn* p2 = p1->next();
    if (!p2)                // If p1 is last element of list, nothing to do.
        return 0;

    // Load p1 attributes into registers and check for errors:
    register uint32 length = p1->length;
    if (length == 0)
        return -1;
    register int32 value = p1->value;
    if (value == 0)
        return -1;
    register int32 start = p1->start;
    if (start > p2->start)  // Check for weak monotonicity.
        return -1;

    // Truncate forbidden values of length (shouldn't be necessary):
    if (length - 1 > uint32(max_int32 - start))
        length = max_int32 - start + 1;

    // Then get on with it:
    while (p2) {
        // Case 1: gap between intervals.
        if (length < uint32(p2->start - start))
            break;
        // Case 2: touching intervals.
        if (length == uint32(p2->start - start)) {
            if (value == p2->value) {
                // Absorb the touching interval:
                length += p2->length;
                tfns.delremove(p2);
                }
            break;
            }
        // Case 3: overlap.
        if (length < p2->start - start + p2->length) {
            if (value == p2->value) {
                // Merge overlapping intervals:
                length = p2->start - start + p2->length;
                tfns.delremove(p2);
                break;
                }
            p2->length = p2->start - start + p2->length - length;
            p2->start = start + length;
            break;
            }
        // Case 4: precise cover: treat the same as general cover:
        register tfn* p3 = p2->next();
        tfns.delremove(p2);
        p2 = p3;
        }

    // Save the possibly altered value of "length":
    p1->length = length;

    return 0;
    } // End of function sfn::carry_over.

/*------------------------------------------------------------------------------
This routine carries the value zero over the top of tfn list elements starting
at p2. The zero-value interval starts at "start" and has length "length".
It is assumed that start <= p2->start.
------------------------------------------------------------------------------*/
//----------------------//
// sfn::carry_over_zero //
//----------------------//
int sfn::carry_over_zero(tfn* p2, int32 start, uint32 length) {
    // These cases are taken care of by the general case:
//    if (!p2 || length == 0)
//        return 0;
    while (p2) {
        if (length <= uint32(p2->start - start)) // Cases 1, 2: gap or touching.
            break;
        if (length < p2->start - start + p2->length) { // Case 3: overlap.
            p2->length = p2->start - start + p2->length - length;
            p2->start = start + length;
            break;
            }
        // If p2 is covered, delete it:
        register tfn* p3 = p2->next();
        tfns.delremove(p2);
        p2 = p3;
        }
    return 0;
    } // End of function sfn::carry_over_zero.

/*------------------------------------------------------------------------------
Set the function value in an interval of the set of integers.
Always maintain the interval list as a non-decreasing sequence of pairwise
disjoint non-empty intervals. The function value in each interval must be
non-zero, and the function values in touching intervals must be different.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Warning: issues relating to integer overflow have not been totally solved
in this function. In particular, the set of all int32 numbers is not
currently premitted to be represented.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note: probably the interval algebra is best done as a simple iterative loop
through all of the tfns which have any sort of intersection with the
new interval.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note: It is tempting to regard p->start + p->length as the endpoint of
an interval, but this is often not true. Hence this value should generally
never be calculated, because it cannot generally be correctly used.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
A useful extension of this function might be to return a pointer to the
tfn which includes the new interval, if "value" is non-zero. For further study!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This function currently returns an integer referencing the 46 exit points,
numbered from 0 to 45.
------------------------------------------------------------------------------*/
//----------------------//
//        sfn::set      //
//----------------------//
int sfn::set(int32 start, uint32 len, int32 value) {
    // Ignore zero-length intervals:
    if (len == 0)
        return 0;

    // Try to avoid overflow problems by truncating any excessive length:
    if (len - 1 > uint32(max_int32 - start))
        len = max_int32 - start + 1;    // Truncate interval to max_int32.

    //----------------------------------------------------------------------
    // The following algorithm finds the unique p1 and p2 such that:
    // p1->start < start <= p2->start,              general case
    //    if tfns is not empty, and
    //        tfns.first()->start < start <= tfns.last()->start;
    //
    // p1 = 0  and  start <= p2->start,             beginning of list
    //    if tfns is not empty, and
    //        start <= tfns.first()->start;
    //
    // p1->start < start  and  p2 = 0,              end of list
    //    if tfns is not empty, and
    //        tfns.last()->start < start;
    //
    // p1 = p2 = 0,                                 empty list
    //    if tfns is empty.
    // Roughly, p1 is the largest tfn for which p1->start < start.
    //          p2 is the smallest tfn for which start <= p2->start.
    //----------------------------------------------------------------------
    tfn* p1 = 0;
    tfn* p2 = 0;
    forall(p2, tfns) {
        if (start <= p2->start)
            break;
        p1 = p2;
        }

    // In each case, determine intersections of the given interval
    // with existing intervals, and make the appropriate adjustments.
    // (Note that when value == 0, the processing is much simpler, and
    // therefore this could be done separately.)
    if (p2) {
        if (p1) {   // p1 && p2. The most general case.
                    // tfns.first()->start < start <= tfns.last()->start.
            // p1 is the first list element which could possibly affect or
            // be affected by the addition of the new interval.
            // Note: after dealing with the first relevant element of the list,
            // p1 and p2 may be altered, and p3 must be taken into account.
            // The purpose of the treatment of the first relevant element is
            // to either make the change and return, or else determine the
            // "carry-over" tfn (possibly 0-valued) which must be applied
            // to later list elements.
            // Note: to make iteration possible, should find a carry-over "tfn"
            // which is in the same range relative to p2 and p2->next() that
            // the given "tfn" was relative to p1 and p2.
            // Should really have some functions for doing the
            // carry-over operations. Then call these after taking care
            // of the initial part of the algorithm.

            uint32 n1 = start - p1->start;  // start relative to p1. n1 > 0.
            uint32 n2 = n1 + len;           // start + len relative to p1.
            // There are 4 cases of n1:
            // 1.   0 <  n1  < p1->length
            // 2.        n1 == p1->length
            // 3.              p1->length <  n1  < p2->start - p1->start
            // 4.                            n1 == p2->start - p1->start.

            // p1 && p2. p1->start < start < p1->end:  [case 1 of n1]
            if (n1 < p1->length) {          // If new interval intersects p1...
                if (value == p1->value) {   // If the same value:
                    if (p1->length < n2) {  // If not a sub-interval...
                        p1->length = n2;    // Extend the existing interval.
                        carry_over(p1);
                        }
                    return 1;
                    }
               // p1 && p2. p1->start < start < p1->end, value == 0:
               else if (value == 0) {       // If new value == 0:
                    if (n2 < p1->length) {  // Add the bit left over:
                        insertafter(p1, p1->start + n2,
                                        p1->length - n2, p1->value);
                        p1->length = n1;    // Shrink the existing interval.
                        return 2;           // Finished.
                        }
                    p1->length = n1;        // Shrink the existing interval.
                    carry_over_zero(p2, start, len);
                    return 3;
                    }
                // p1 && p2. p1->start < start < p1->end, 0!=value!=p1->value:
                else {  // The new value is non-zero and different to p1...
                    // The insertafter() could be unnecessary if
                    // p2->value == value and len >= p2->start - start.
                    if (value == p2->value
                            && len >= uint32(p2->start - start)) {
                        // Merge new tfn with p2:
                        p2->length += p2->start - start;
                        p2->start = start;
                        if (p2->length < len) {
                            p2->length = len;
                            carry_over(p2);
                            }
                        p1->length = n1;
                        return 44;
                        }
                    tfn* p3 = insertafter(p1, start, len, value);
                    if (!p3)                    // Should never happen.
                        return 4;
                    if (n2 < p1->length) {      // Add the bit left over:
                        insertafter(p3, p1->start + n2,
                                        p1->length - n2, p1->value);
                        p1->length = n1;        // Shrink existing interval.
                        return 5;               // Finished.
                        }
                    p1->length = n1;            // Truncate old tfn.
                    carry_over(p3);
                    return 6;
                    }
                }
            // p1 && p2. p1->end < start <= p2->start.
            else if (n1 > p1->length) {
                // p1 && p2. p1->end < start == p2->start:  [case 4 of n1]
                // (Note: this is just the carry over of new tfn over p2.)
                if (n1 == uint32(p2->start - p1->start)) {
                    if (value == p2->value) {
                        if (len <= p2->length)      // Value already set.
                            return 7;
                        p2->length = len;           // Extend p2.
                        carry_over(p2);
                        return 8;
                        }
                    else if (value == 0) {
                        carry_over_zero(p2, start, len);
                        return 45;
                        }
                    else {  // 0 != value != p2->value:
                        if (len < p2->length) {
                            p2->start += len;
                            p2->length -= len;
                            insertafter(p1, start, len, value);
                            return 9;
                            }
                        if (len == p2->length) {    // Just change the value!
                            p2->value = value;
                            carry_over(p2); // Maybe value == p2->next()->value!
                            return 10;
                            }
                        // Extend and re-value p2:
                        p2->length = len;
                        p2->value = value;
                        carry_over(p2);
                        return 11;
                        }
                    }
                // p1 && p2. p1->end < start < p2->start:  [case 3 of n1]
                else {
                    // This should be much the same as start == p2->start,
                    // except that (start, len, value) is truncated...
                    if (n2 < uint32(p2->start - p1->start)) {
                        // Isolated new interval:
                        if (value != 0)
                            insertafter(p1, start, len, value);
                        return 12;
                        }
                    if (n2 == uint32(p2->start - p1->start)) {
                        // Touching intervals.
                        if (value == 0)
                            return 13;
                        if (value == p2->value) {   // Merge with p2.
                            p2->length += len;
                            p2->start -= len;
                            return 14;
                            }
                        // New interval touches p2, but different value:
                        insertafter(p1, start, len, value);
                        return 15;
                        }
                    // New interval overlaps p2:
                    if (value == 0) {
                        carry_over_zero(p2, start, len);
                        return 16;
                        }
                    if (value == p2->value) {
                        if (len <= p2->start - start + p2->length) {
                            // Extend p2 to the left:
                            p2->length += p2->start - start;
                            p2->start = start;
                            return 17;
                            }
                        // Replace p2 with new tfn (which covers p2):
                        p2->start = start;
                        p2->length = len;
                        carry_over(p2);
                        return 18;
                        }
                    // p1->end < start < p2->start. 0 != value != p2->value,
                    // and n2 > p2->start - p1->start:
                    if (len < p2->start - start + p2->length) {
                        insertafter(p1, start, len, value);
                        p2->length -= start + len - p2->start;
                        p2->start = start + len;
                        return 19;
                        }
                    // New tfn covers p2, so hijack it:
                    p2->start = start;
                    p2->length = len;
                    p2->value = value;
                    carry_over(p2); // In case value == p2->next()->value.
                    return 20;
                    }
                }
            // p1 && p2. p1->end == start:  [case 2 of n1]
            else {  // ...if n1 == p1->length...
                if (value == p1->value) {
                    p1->length += len;
                    carry_over(p1);
                    return 21;
                    }
                // p1 && p2. p1->end == start. value == 0:
                if (value == 0) {
                    carry_over_zero(p2, start, len);
                    return 22;
                    }
                // p1 && p2. p1->end == start. 0 != value != p1->value:
                // (This could be done by carry_over, but possibly by
                // doing an unnecessary memory allocation/freeing.)
                if (n2 < uint32(p2->start - p1->start)) { // tfn lands in a gap.
                    insertafter(p1, start, len, value);
                    return 23;
                    }
                // The tfn precisely fills a gap:
                if (n2 == uint32(p2->start - p1->start)) {
                    if (value != p2->value) {
                        insertafter(p1, start, len, value);
                        return 24;
                        }
                    // Extend p2 back to meet p1:
                    p2->length += p2->start - start;
                    p2->start = start;
                    return 25;
                    }
                // The new tfn overlaps p2 (p2->start < end <= p2->end):
                if (len <= p2->start - start + p2->length) {
                    if (value == p2->value) {
                        // Extend p2 back to start:
                        p2->length += p2->start - start;
                        p2->start = start;
                        return 26;
                        }
                    // end < p2->end. 0 != value != p2->value:
                    if (len < p2->start - start + p2->length) {
                        p2->length = p2->start - start + p2->length - len;
                        p2->start = start + len;
                        insertafter(p1, start, len, value);
                        return 27;
                        }
                    // end == p2->end. 0 != value != p2->value:
                    // Re-use p2 to represent the new tfn:
                    p2->length = len;
                    p2->start = start;
                    p2->value = value;
                    carry_over(p2);
                    return 28;
                    }
                // New tfn covers p2 and beyond. Re-use p2:
                p2->length = len;
                p2->start = start;
                p2->value = value;
                carry_over(p2);
                return 29;
                }
            // This completes the processing of the p1 && p2 case by
            // breaking it into cases according to the sign of n1 - p1->length.
            }
        else {      // !p1 && p2. start <= tfns.first()->start.
                    // p2 == tfns.first().
            // This is the same as the carry-over for
            // the p1 && p2 case, where the new tfn itself is the carry-over.
            // But do some processing first to minimize memory alloc/free.
            if (value == 0) {
                carry_over_zero(p2, start, len);
                return 30;
                }
            if (len < uint32(p2->start - start)) {      // Case 1. gap.
                insertafter(p1, start, len, value);
                return 31;
                }
            if (len == uint32(p2->start - start)) {     // Case 2: touching.
                if (value != p2->value) {
                    insertafter(p1, start, len, value);
                    return 32;
                    }
                // Re-use p2:
                p2->length += len;
                p2->start = start;
                return 33;
                }
            // p2->start < end < p2->end:
            if (len < p2->start - start + p2->length) { // Case 3: overlap.
                if (value != p2->value) {
                    insertafter(p1, start, len, value);
                    p2->length = p2->start - start + p2->length - len;
                    p2->start = start + len;
                    return 34;
                    }
                // Extend p2 back to "start":
                p2->length += p2->start - start;
                p2->start = start;
                return 35;
                }
            // Case 4: covering. Gobble up p2, and carry over:
            p2->start = start;
            p2->length = len;
            p2->value = value;
            carry_over(p2);
            return 36;
            }
        }
    else {
        if (p1) {   // p1 && !p2. tfns.last()->start < start.
                    // p1 == tfns.last().
            uint32 n1 = start - p1->start;  // start relative to tfn. n1 > 0.
            uint32 n2 = n1 + len;           // start + len relative to tfn.

            // Deal with the very simple value == 0 case first:
            if (value == 0) {
                if (n1 < p1->length) {
                    if (n2 < p1->length)    // Hole in the middle of p1.
                        append(start + len, p1->length - n2, p1->value);
                    p1->length = n1;        // Truncate p1.
                    }
                return 37;
                }
            // p1 && !p2. p1->start < start < p1->end. value != 0:
            if (n1 < p1->length) {          // If tfn intersects...
                if (value == p1->value) {   // If the same value:
                    if (p1->length < n2)    // Extend the existing interval.
                        p1->length = n2;
                    return 38;
                    }
                append(start, len, value);
                if (n2 < p1->length)        // Right portion of p1.
                    append(start + len, p1->length - n2, p1->value);
                p1->length = n1;            // Truncate old tfn.
                return 39;
                }
            // p1 && !p2. p1->end < start. value != 0:
            if (n1 > p1->length) {          // No intersection, positive gap:
                append(start, len, value);
                return 40;
                }
            // p1 && !p2. p1->end == start. value != 0:
            // No intersection, but touching.
            if (value == p1->value) {
                p1->length += len;          // Extend p1.
                return 41;
                }
            append(start, len, value);      // Touching, but different value.
            return 42;
            }
        else {      // !p1 && !p2. tfns was empty.
            if (value != 0)
                append(start, len, value);
            return 43;
            }
        }
    } // End of function sfn::set.

/*------------------------------------------------------------------------------
Get the value of the simple function at a given index i.
The space between defined intervals is assumed to have the value 0.
------------------------------------------------------------------------------*/
//----------------------//
//       sfn::get       //
//----------------------//
int32 sfn::get(int32 i) {
    tfn* p1 = 0;
    Forall(tfn, p2, tfns) {
        if (i < p2->start)
            break;
        p1 = p2;
        }
    // The empty set.
    if (!p1)
        return 0;

    // i is in a gap or past the last point.
    if (p1->length <= uint32(i - p1->start))
        return 0;

    // i is in a defined interval.
    return p1->value;
    } // End of function sfn::get.

/*------------------------------------------------------------------------------
Returns first incorrect interval in the list, if any.
Errors could be:
1   length == 0
2   value == 0
3   starts not strictly increasing
4   overlapping intervals
5   touching intervals with same value
In the last 3 cases (2-tfn error), the first of two intervals is returned.
If "reason" is a non-null pointer, then *reason is set to the reason for
incorrectness, or to 0 if there was no error.
------------------------------------------------------------------------------*/
//----------------------//
//    sfn::incorrect    //
//----------------------//
tfn* sfn::incorrect(int* reason) {
    for (register tfn* p1 = tfns.first(); p1; ) {
        if (p1->length == 0) {
            if (reason)
                *reason = sfnLENGTH;
            return p1;
            }
        if (p1->value == 0) {
            if (reason)
                *reason = sfnVALUE;
            return p1;
            }
        register tfn* p2 = p1->next();
        if (!p2)
            break;
        if (p1->start >= p2->start) {
            if (reason)
                *reason = sfnMONO;
            return p1;
            }
        register uint32 n = p2->start - p1->start;
        if (p1->length > n) {
            if (reason)
                *reason = sfnOVERLAP;
            return p1;
            }
        if (p1->length == n && p1->value == p2->value) {
            if (reason)
                *reason = sfnTOUCH;
            return p1;
            }
        p1 = p2;
        }
    if (reason)
        *reason = sfnNONE;
    return 0;
    } // End of function sfn::incorrect.

/*------------------------------------------------------------------------------
Work in progress...
This routine will traverse the tfns list, correcting all errors by removing all
tfns with value == 0 and length == 0, removing tfns which are not in increasing
order, shortening or removing tfns which are overlapped, and combining touching
tfns which have the same value.
The return value is the number (in some sense) of errors corrected.
------------------------------------------------------------------------------*/
//----------------------//
//     sfn::correct     //
//----------------------//
int sfn::correct() {
    return 0;
    } // End of function sfn::correct.

//----------------------//
//      sfn::print      //
//----------------------//
void sfn::print(ostream& os) {
    int reason = sfnNONE;
    tfn* q = incorrect(&reason);
    const char* msg = sfn_error_msg[reason];
    Forall(tfn, p, tfns) {
        os << "(" << p->start << ", " << p->length << ", "
                  << p->value << ")";
        if (p == q)
            os << " <== error [" << reason << ": " << msg << "]";
        os << NL;
        }
    } // End of function sfn::print.
