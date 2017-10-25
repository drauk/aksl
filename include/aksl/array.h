// src/aksl/array.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_ARRAY_H
#define AKSL_ARRAY_H
/*------------------------------------------------------------------------------
Classes in this file:

short_big_array::
uint16_big_array::
long_big_array::
double_big_array::
------------------------------------------------------------------------------*/

#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// Constants for the big_array classes:
// (The fine-tuning of these could possibly affect efficiency.)
static const long block_size = 1024;
static const long block_quantum = 100;

/*------------------------------------------------------------------------------
short_big_array is a space-efficient class for large arrays of short integers.
It is implemented as a variable-length array of pointers to blocks of short
integers. The pointers are only assigned to new blocks if a reference requires
it. Similarly, the array of pointers is only extended (by a quantum) if a
reference makes this necessary.

Clearly this class should be made into a template!
------------------------------------------------------------------------------*/
//----------------------//
//   short_big_array::  //
//----------------------//
struct short_big_array {
private:
    long n_blocks;          // Number of blocks.
    short** blocks;        // Array of pointers to blocks.

    bool_enum block_exists(long k)    // For internal use.
        { return (bool_enum)(k >= 0 && k < n_blocks && blocks[k]);}
    void resize(long);      // For internal use.
    void new_block(long);   // For internal use.
public:
    short get(long);
    short i_get(long i) { // Inline version of "get":
        long k;
        return (i < 0) ? 0 :
           (k = i/block_size, (block_exists(k)) ? (blocks[k][i % block_size])
                                                : 0); }
    void set(long i, short x); // Set element i to x.
    void add(long i, short x); // Add x to element i.
    long max_index_nz();        // Returns max index of non-zero element.
    short max_element();       // Returns max of all elements in the array.
    short sum_elements();      // Returns sum of all elements in the array.
    double dsum_elements();     // Returns (double) sum of elements in array.

//    short_big_array& operator=(const short_big_array& x) {}
//    short_big_array(const short_big_array& x) {};
    short_big_array() { n_blocks = 0; blocks = 0; }
    ~short_big_array();
    }; // End of struct short_big_array.

/*------------------------------------------------------------------------------
uint16_big_array is a space-efficient class for large arrays of uint16 integers.
It is implemented as a variable-length array of pointers to blocks of uint16
integers. The pointers are only assigned to new blocks if a reference requires
it. Similarly, the array of pointers is only extended (by a quantum) if a
reference makes this necessary.

Clearly this class should be made into a template!
------------------------------------------------------------------------------*/
//----------------------//
//  uint16_big_array::  //
//----------------------//
struct uint16_big_array {
private:
    long n_blocks;          // Number of blocks.
    uint16** blocks;        // Array of pointers to blocks.

    bool_enum block_exists(long k)    // For internal use.
        { return (bool_enum)(k >= 0 && k < n_blocks && blocks[k]);}
    void resize(long);      // For internal use.
    void new_block(long);   // For internal use.
public:
    uint16 get(long);
    uint16 i_get(long i) { // Inline version of "get":
        long k;
        return (i < 0) ? 0 :
           (k = i/block_size, (block_exists(k)) ? (blocks[k][i % block_size])
                                                : 0); }
    void set(long i, uint16 x); // Set element i to x.
    void add(long i, uint16 x); // Add x to element i.
    long max_index_nz();        // Returns max index of non-zero element.
    uint16 max_element();       // Returns max of all elements in the array.
    uint16 sum_elements();      // Returns sum of all elements in the array.
    double dsum_elements();     // Returns (double) sum of elements in array.

//    uint16_big_array& operator=(const uint16_big_array& x) {}
//    uint16_big_array(const uint16_big_array& x) {};
    uint16_big_array() { n_blocks = 0; blocks = 0; }
    ~uint16_big_array();
    }; // End of struct uint16_big_array.

/*------------------------------------------------------------------------------
long_big_array is a space-efficient class for large arrays of long integers.
It is implemented as a variable-length array of pointers to blocks of long
integers. The pointers are only assigned to new blocks if a reference requires
it. Similarly, the array of pointers is only extended (by a quantum) if a
reference makes this necessary.
------------------------------------------------------------------------------*/
//----------------------//
//   long_big_array::   //
//----------------------//
struct long_big_array {
private:
    long n_blocks;          // Number of blocks.
    long** blocks;          // Array of pointers to blocks.

    bool_enum block_exists(long k)    // For internal use.
        { return (bool_enum)(k >= 0 && k < n_blocks && blocks[k]);}
    void resize(long);      // For internal use.
    void new_block(long);   // For internal use.
public:
    long get(long);
    long i_get(long i) { // Inline version of "get":
        long k;
        return (i < 0) ? 0 :
           (k = i/block_size, (block_exists(k)) ? (blocks[k][i % block_size])
                                                : 0); }
    void set(long i, long x);   // Set element i to x.
    void add(long i, long x);   // Add x to element i.
    long max_index_nz();        // Returns max index of non-zero element.
    long max_element();         // Returns max of all elements in the array.
    long sum_elements();        // Returns sum of all elements in the array.
    double dsum_elements();     // Returns (double) sum of elements in array.

//    long_big_array& operator=(const long_big_array& x) {}
//    long_big_array(const long_big_array& x) {};
    long_big_array() { n_blocks = 0; blocks = 0; }
    ~long_big_array();
    }; // End of struct long_big_array.

/*------------------------------------------------------------------------------
double_big_array is a space-efficient class for large arrays of doubles.
It is implemented as a variable-length array of pointers to blocks of doubles.
The pointers are only assigned to new blocks if a reference requires
it. Similarly, the array of pointers is only extended (by a quantum) if a
reference makes this necessary.
------------------------------------------------------------------------------*/
//----------------------//
//  double_big_array::  //
//----------------------//
struct double_big_array {
private:
    long n_blocks;          // Number of blocks.
    double** blocks;        // Array of pointers to blocks.

    bool_enum block_exists(long k)    // For internal use.
        { return (bool_enum)(k >= 0 && k < n_blocks && blocks[k]);}
    void resize(long);      // For internal use.
    void new_block(long);   // For internal use.
public:
    double get(long);
    double i_get(long i) { // Inline version of "get":
        long k;
        return (i < 0) ? 0 :
           (k = i/block_size, (block_exists(k)) ? (blocks[k][i % block_size])
                                                : 0); }
    void set(long i, double x); // Set element i to x.
    void add(long i, double x); // Add x to element i.
    long max_index_nz();        // Returns max index of non-zero element.
    double max_element();       // Returns max of all elements in the array.
    double sum_elements();      // Returns sum of all elements in the array.
    double dsum_elements();     // Returns (double) sum of elements in array.

//    double_big_array& operator=(const double_big_array& x) {}
//    double_big_array(const double_big_array& x) {};
    double_big_array() { n_blocks = 0; blocks = 0; }
    ~double_big_array();
    }; // End of struct double_big_array.

#endif /* AKSL_ARRAY_H */
