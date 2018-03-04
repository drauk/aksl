// src/aksl/heap.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

min_tim_heap::
    resize
    insert
    popfirst
min_tim2_heap::
    resize
    insert
    popfirst
------------------------------------------------------------------------------*/

#include "aksl/heap.h"

/*------------------------------------------------------------------------------
min_tim_heap::resize() dynamically resizes the heap.
Argument checks not necessary because only used internally.
The size is increased by the constant value "heap_block_size".
------------------------------------------------------------------------------*/
//----------------------//
// min_tim_heap::resize //
//----------------------//
void min_tim_heap::resize() {
    tim** p = new tim*[size + heap_block_size];
    tim** q = p;
    tim** r = heap;
    for (unsigned long i = 0; i < size; ++i)
        *q++ = *r++;
    delete[] heap;
    heap = p;
    size += heap_block_size;
    } // End of function min_tim_heap::resize.

/*------------------------------------------------------------------------------
min_tim_heap::insert() inserts a pointer to a "tim" or derived structure into
the heap.
------------------------------------------------------------------------------*/
//----------------------//
// min_tim_heap::insert //
//----------------------//
void min_tim_heap::insert(tim* p) {
    if (!p)
        return;
    if (++n >= size)
        resize();                   // Must maintain size >= n + 1.
    unsigned long j = n;
    double t = p->t;
    while (j > 1) {
        unsigned long i = j >> 1;    // i is the parent of j.
        if (heap[i]->t <= t)        // <= for speed and FIFO justice.
            break;
        heap[j] = heap[i];
        j = i;
        }
    heap[j] = p;
    } // End of function min_tim_heap::insert.

/*------------------------------------------------------------------------------
min_tim_heap::popfirst() removes a pointer in the heap with the least value
of the member "t". The pointer is returned to the caller.
------------------------------------------------------------------------------*/
//--------------------------//
//  min_tim_heap::popfirst  //
//--------------------------//
tim* min_tim_heap::popfirst() {
    if (n < 1)
        return 0;
    tim* p = heap[1];           // The return value.
    tim* q = heap[n];           // The pointer to be reinserted.
    n -= 1;
    double t = q->t;
    unsigned long i = 1;
    unsigned long j;
    while ((j = i << 1) <= n) { // j and j+1 are children of i.
        if (j < n && heap[j+1]->t < heap[j]->t) // < for speed and FIFO justice.
            j += 1;
        if (heap[j]->t >= t)    // >= for speed, but not FIFO justice.
            break;
        heap[i] = heap[j];
        i = j;
        }
    heap[i] = q;
    return p;
    } // End of function min_tim_heap::popfirst.

/*------------------------------------------------------------------------------
min_tim2_heap::resize() dynamically resizes the heap.
Argument checks not necessary because only used internally.
The size is increased by the constant value "heap_block_size".
------------------------------------------------------------------------------*/
//--------------------------//
//   min_tim2_heap::resize  //
//--------------------------//
void min_tim2_heap::resize() {
    // Create a new array of pointers.
    tim2** p = new tim2*[size + heap_block_size];

    // Copy the old pointers to the new array.
    tim2** q = p;
    tim2** r = (tim2**)heap;
    // (Only need to copy up to i <= n here. But play safe.)
    for (unsigned long i = 0; i < size; ++i)
        *q++ = *r++;

    // Delete the old array, and update pointer and size.
    delete[] heap;
    heap = (tim**)p;
    size += heap_block_size;
    } // End of function min_tim2_heap::resize.

#include <iostream>

/*------------------------------------------------------------------------------
min_tim2_heap::insert() inserts a pointer to a "tim2" or derived structure into
the heap.
This differs from the min_tim_heap class in the sorting criterion.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Note that the comparison "heap[i]->t <= t" really signifies that the parent is
_less_ than the child. Suppose heap[i]->t == p->t, then it is necessarily true
that heap[i]->index < p->index, because the index is the latest index to be
allocated. So the comparison rule is the same as for min_tim_heap::insert().
------------------------------------------------------------------------------*/
//--------------------------//
//   min_tim2_heap::insert  //
//--------------------------//
void min_tim2_heap::insert(tim2* p) {
    if (!p)
        return;
    // If it is not true that n <= size - 1, must resize.
    if (++n >= size)
        resize();                   // Must maintain size >= n + 1.

    // Attach a secondary sorting-index to the new event.
    p->index = next_index;
    next_index += 1;

    // Insert new event at end-of-heap, and percolate upwards.
    unsigned long j = n;
    double t = p->t;
    while (j > 1) {
        unsigned long i = j >> 1;   // i is the parent of j.
        // If the parent is less than the child, finished. (See above note.)
        if (heap[i]->t <= t)
            break;

        // Move the parent to the child position.
        heap[j] = heap[i];
        j = i;
        }
    // Leave the new event in the child position.
    heap[j] = p;
    } // End of function min_tim2_heap::insert.

/*------------------------------------------------------------------------------
min_tim2_heap::popfirst() removes a pointer in the heap with the least value
of the member "t". The pointer is returned to the caller.
This differs from the min_tim_heap class in the sorting criterion.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
It is very important to compare indices by comparing the difference with 0
rather than comparing the indices with each other.
This copes optimally with the integer wrap-around problem.
Thus (i1 < i2) will give the wrong answer if i2 has wrapped around to -infinity.
But ((i1 - i2) < 0) will give the right answer if there are less than
2 billion events enqueued between the two events.
This should be satisfactory for all except the most extreme situations.
------------------------------------------------------------------------------*/
//--------------------------//
//  min_tim2_heap::popfirst //
//--------------------------//
tim2* min_tim2_heap::popfirst() {
    if (n < 1)
        return 0;

    // Pop the least-time event.
    tim2* p = (tim2*)heap[1];

    // Remove the end-of-heap event for re-insertion.
    tim2* q = (tim2*)heap[n];
    n -= 1;
    double t = q->t;
    long index = q->index;

    // Percolate the end-of-heap event into the heap from the top.
    unsigned long i = 1;
    unsigned long j;
    while ((j = i << 1) <= n) { // j and j+1 are children of i.
        // c0 = left child.
        register tim2* c0 = (tim2*)heap[j];

        // If there is a right child, consider it.
        if (j < n) {
            // c1 = right child.
            register tim2* c1 = (tim2*)heap[j+1];

            // If right child < left child, focus on the right child.
            if (c1->t < c0->t
                    || ((c1->t == c0->t) && (c1->index - c0->index) < 0)) {
                c0 = c1;
                j += 1;
                }
            }

        // If parent < least child, exit the loop.
        if (t < c0->t || (t == c0->t && (index - c0->index) < 0))
            break;

        // If parent >= least child, move the child into the parent position.
        heap[i] = c0;
        i = j;
        }
    // Put the parent event in the hole created in the heap.
    heap[i] = q;

    return p;
    } // End of function min_tim2_heap::popfirst.
