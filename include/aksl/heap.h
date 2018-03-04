// src/aksl/heap.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_HEAP_H
#define AKSL_HEAP_H
/*------------------------------------------------------------------------------
Classes in this file:

tim::
min_tim_heap::
heap_traversal::

tim2::
min_tim2_heap::
tim2_heap_traversal::
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The classes in this file are intended for implementing future event lists. But
they may be of use in general sorting, or to represent lists where an object
can be a multiple member of the same list. But random removal from such lists
is linear in the size of the list.
------------------------------------------------------------------------------*/

#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// The heap block size wil be made variable if there is a demand for it:
static const unsigned int heap_block_size = 512;

/*------------------------------------------------------------------------------
The struct "tim" is intended to be used for the derivation of event classes
and such, so that pointers to events can be sent to the functions in the
heap class(es). Note that derived classes are responsible for initialising "t".
------------------------------------------------------------------------------*/
//----------------------//
//         tim::        //
//----------------------//
struct tim {
    double t;     // Uninitilised time, for speed.

    tim() {} // Derived classes are responsible for initialising "t" !
    ~tim() {}
    }; // End of struct tim.

/*------------------------------------------------------------------------------
min_tim_heap:: basically implements the heap algorithm in
    G.H. Gonnet, "Handbook of algorithms and data structures",
    Addison-Wesley, London 1984, Pp. 169-172.
The algorithm here sifts pointers rather than the objects themselves. The
algorithm has also been optimised a little bit.

The function popfirst() returns a "tim" with the lowest value of "t" in the
heap. The insertion and extraction times are both strictly bounded by a
logarithm (to base 2) of the size of the heap. Average behaviour for random
keys is probably also logarithmic. For a non-decreasing sequence of inserted
keys, insertion time is constant, and extraction is logarithmic.

The algorithm does not maintain FIFO order among events with the same value
of "t". (FIFO order of equal-time events could probably be implemented by
tagging each incoming event with a sequence number, which is then used in
addition to the member "t" as a secondary sorting key. But this would only work
up to about 2^32 - 1 = 4294967295 events, for an unsigned long tag.)

The size is "unsigned int" rather than "unsigned long" because any machine that
has 16-bit "int" probably doesn't want to do massive simulations anyway. So
speed is probably more important than the ability to store more than 65535
events.

Note the interesting fact that more than one element in the heap may point
to the same event. That is, an event may be a multiple member of the heap.
It is important, though, that the element not be deleted while it is referred
to in the heap. The multiple-membership possibility may or may not have a
useful application.
------------------------------------------------------------------------------*/
//----------------------//
//    min_tim_heap::    //
//----------------------//
struct min_tim_heap {
friend struct heap_traversal;
protected:
    tim** heap;         // Array of pointers to "tim"s. heap[0] is never used.
    unsigned int size;  // The size of the array "heap".
    unsigned int n;     // Number of occupants of the heap, starting at heap[1].

    void resize();
public:
    unsigned int length() const { return n; }
    int empty() const { return n == 0; }
    const tim* first() const { return (n > 0) ? heap[1] : 0; }
    const tim* last() const { return (n > 0) ? heap[n] : 0; } // Not a maximum!

    void insert(tim*);
    tim* popfirst();
    tim* remove(tim*); // Define this some day to remove an arbitrary element.

    min_tim_heap() {
        heap = new tim*[heap_block_size];
        size = heap_block_size;
        n = 0;
        }
    ~min_tim_heap() { delete[] heap; }
    }; // End of struct min_tim_heap.

//----------------------//
//    heap_traversal::  //
//----------------------//
struct heap_traversal {
private:
    min_tim_heap* hp;           // Pointer to a heap.
    unsigned int i;             // Position in heap.
public:
    tim* next() { i += 1; return (i <= hp->n) ? hp->heap[i] : 0; }
    void init() { i = 0; }      // Useful for restarting.

//    heap_traversal& operator=(const heap_traversal& x) {}
//    heap_traversal(const heap_traversal& x) {};
    heap_traversal(min_tim_heap& h) { hp = &h; i = 0; }
    ~heap_traversal() {}
    }; // End of struct heap_traversal.

/*------------------------------------------------------------------------------
The struct "tim2" is intended to be used for the derivation of event classes
and such, so that pointers to events can be sent to the functions in the
heap class(es).
Note that derived classes are _not_ responsible for initialising "index".
The min_tim2_heap::insert() function should set "index".
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This class is intended for situations where the user wishes to control the order
of dequeuing of events which have the same execution time.
------------------------------------------------------------------------------*/
//----------------------//
//        tim2::        //
//----------------------//
struct tim2 : public tim {
friend class min_tim2_heap;
private:
    long index;     // This is intentionally signed. Should be >= 32 bits.
public:
    tim2() {}       // min_tim2_heap is responsible for initialising "index".
    ~tim2() {}
    }; // End of struct tim2.

/*------------------------------------------------------------------------------
The successful derivation of this class from min_tim_heap relies on the fact
that an array of tim2* is implemented the same as an array of tim*.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The insert and pop functions for this class must be separately defined to the
parent class because they involve sorting of elements of the heap, and this
sorting depends on the "index" field.
------------------------------------------------------------------------------*/
//----------------------//
//    min_tim2_heap::   //
//----------------------//
struct min_tim2_heap : public min_tim_heap {
friend struct tim2_heap_traversal;
private:
    long next_index;    // May be arbitrarily initialized at beginning of use.

    void resize();
public:
    const tim2* first() const { return (tim2*)min_tim_heap::first(); }
    const tim2* last() const { return (tim2*)min_tim_heap::last(); }

    void insert(tim2*);
    tim2* popfirst();
    tim2* remove(tim2*); // Define this some day to remove an arbitrary element.
    void reset_index() { next_index = 0; } // Not as useful as you might think.

    min_tim2_heap() { next_index = 0; }
    ~min_tim2_heap() {}
    }; // End of struct min_tim2_heap.

//--------------------------//
//   tim2_heap_traversal::  //
//--------------------------//
struct tim2_heap_traversal {
private:
    min_tim2_heap* hp;          // Pointer to a heap.
    unsigned int i;             // Position in heap.
public:
    tim2* next() { i += 1; return (i <= hp->n) ? (tim2*)hp->heap[i] : 0; }
    void init() { i = 0; }      // Useful for restarting.

//    tim2_heap_traversal& operator=(const tim2_heap_traversal& x) {}
//    tim2_heap_traversal(const tim2_heap_traversal& x) {};
    tim2_heap_traversal(min_tim2_heap& h) { hp = &h; i = 0; }
    ~tim2_heap_traversal() {}
    }; // End of struct tim2_heap_traversal.

#endif /* AKSL_HEAP_H */
