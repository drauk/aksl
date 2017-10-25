// src/aksl/array.c   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/array.c be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

short_big_array::
    ~short_big_array
    resize
    new_block
    get
    set
    add
    max_index_nz
    max_element
uint16_big_array::
    ~uint16_big_array
    resize
    new_block
    get
    set
    add
    max_index_nz
    max_element
long_big_array::
    ~long_big_array
    resize
    new_block
    get
    set
    add
    max_index_nz
    max_element
double_big_array::
    ~double_big_array
    resize
    new_block
    get
    set
    add
    max_index_nz
    max_element
------------------------------------------------------------------------------*/

#include "aksl/array.h"

//--------------------------------------//
//   short_big_array::~short_big_array  //
//--------------------------------------//
short_big_array::~short_big_array() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    } // End of function short_big_array::~short_big_array.

/*------------------------------------------------------------------------------
short_big_array::resize takes the index "k" of the block pointer which should
be made available, NOT the index "i" of the short itself.
------------------------------------------------------------------------------*/
//--------------------------//
//  short_big_array::resize //
//--------------------------//
void short_big_array::resize(long k) {
    if (k < n_blocks)
        return;
    long new_n_blocks = block_quantum * (1 + k/block_quantum);
    short** new_blocks = new short*[new_n_blocks];
    short** pp = blocks;
    short** npp = new_blocks;
    long j = 0;
    for (j = 0; j < n_blocks; ++j)
        *npp++ = *pp++;
    for ( ; j < new_n_blocks; ++j)
        *npp++ = 0;
    delete[] blocks;
    blocks = new_blocks;
    n_blocks = new_n_blocks;
    } // End of function short_big_array::resize.

//------------------------------//
//  short_big_array::new_block  //
//------------------------------//
void short_big_array::new_block(long k) {
    if (k < 0)
        return;
    if (k >= n_blocks)
        resize(k);
    if (!blocks[k]) {
        register short* pl = blocks[k] = new short[block_size];
        for (long j = 0; j < block_size; ++j)
            *pl++ = 0;
        }
    } // End of function short_big_array::new_block.

//--------------------------//
//   short_big_array::get   //
//--------------------------//
short short_big_array::get(long i) {
    if (i < 0)
        return 0;
    long k = i/block_size;
    if (!block_exists(k))
        return 0;
    return blocks[k][i % block_size];
    } // End of function short_big_array::get.

//--------------------------//
//   short_big_array::set   //
//--------------------------//
void short_big_array::set(long i, short x) {
    if (i < 0)
        return;
    long k = i/block_size;
    if (!block_exists(k)) {
        if (x == 0)
            return;
        new_block(k);
        }
    blocks[k][i % block_size] = x;
    } // End of function short_big_array::set.

//--------------------------//
//   short_big_array::add   //
//--------------------------//
void short_big_array::add(long i, short x) {
    if (i < 0 || x == 0)
        return;
    long k = i/block_size;
    if (!block_exists(k))
        new_block(k);
    blocks[k][i % block_size] += x;
    } // End of function short_big_array::add.

/*------------------------------------------------------------------------------
short_big_array::max_index_nz() returns the index of the non-zero array element
with the highest index. If there is no non-zero element, then -1 is returned.
------------------------------------------------------------------------------*/
//----------------------------------//
//   short_big_array::max_index_nz  //
//----------------------------------//
long short_big_array::max_index_nz() {
    register short** pb = blocks + n_blocks;
    for (long k = n_blocks - 1; k >= 0; --k) {
        register short* pl = *--pb;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl != 0)
                return k * block_size + j;
        }
    return -1;
    } // End of function short_big_array::max_index_nz.

/*------------------------------------------------------------------------------
short_big_array::max_element() returns the maximum value found in the array.
The maximum is always non-negative, since it is considered that the array always
has an infinite number of zero elements.
------------------------------------------------------------------------------*/
//----------------------------------//
//   short_big_array::max_element   //
//----------------------------------//
short short_big_array::max_element() {
    register short x = 0;
    register short** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register short* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl > x)  // Optimized for 68000-type architectures!
                x = *pl;
        }
    return x;
    } // End of function short_big_array::max_element.

/*------------------------------------------------------------------------------
short_big_array::sum_elements() returns the sum of elements found in the array.
------------------------------------------------------------------------------*/
//----------------------------------//
//   short_big_array::sum_elements  //
//----------------------------------//
short short_big_array::sum_elements() {
    register short x = 0;
    register short** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register short* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *--pl; // Optimized for 68000-type architectures?
        }
    return x;
    } // End of function short_big_array::sum_elements.

/*------------------------------------------------------------------------------
short_big_array::dsum_elements() returns the (double) sum of elements in array.
------------------------------------------------------------------------------*/
//----------------------------------//
//  short_big_array::dsum_elements  //
//----------------------------------//
double short_big_array::dsum_elements() {
    register double x = 0;
    register short** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register short* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *pl;
        }
    return x;
    } // End of function short_big_array::dsum_elements.

//--------------------------------------//
//  uint16_big_array::~uint16_big_array //
//--------------------------------------//
uint16_big_array::~uint16_big_array() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    } // End of function uint16_big_array::~uint16_big_array.

/*------------------------------------------------------------------------------
uint16_big_array::resize takes the index "k" of the block pointer which should
be made available, NOT the index "i" of the uint16 itself.
------------------------------------------------------------------------------*/
//--------------------------//
// uint16_big_array::resize //
//--------------------------//
void uint16_big_array::resize(long k) {
    if (k < n_blocks)
        return;
    long new_n_blocks = block_quantum * (1 + k/block_quantum);
    uint16** new_blocks = new uint16*[new_n_blocks];
    uint16** pp = blocks;
    uint16** npp = new_blocks;
    long j = 0;
    for (j = 0; j < n_blocks; ++j)
        *npp++ = *pp++;
    for ( ; j < new_n_blocks; ++j)
        *npp++ = 0;
    delete[] blocks;
    blocks = new_blocks;
    n_blocks = new_n_blocks;
    } // End of function uint16_big_array::resize.

//------------------------------//
//  uint16_big_array::new_block //
//------------------------------//
void uint16_big_array::new_block(long k) {
    if (k < 0)
        return;
    if (k >= n_blocks)
        resize(k);
    if (!blocks[k]) {
        register uint16* pl = blocks[k] = new uint16[block_size];
        for (long j = 0; j < block_size; ++j)
            *pl++ = 0;
        }
    } // End of function uint16_big_array::new_block.

//--------------------------//
//   uint16_big_array::get  //
//--------------------------//
uint16 uint16_big_array::get(long i) {
    if (i < 0)
        return 0;
    long k = i/block_size;
    if (!block_exists(k))
        return 0;
    return blocks[k][i % block_size];
    } // End of function uint16_big_array::get.

//--------------------------//
//   uint16_big_array::set  //
//--------------------------//
void uint16_big_array::set(long i, uint16 x) {
    if (i < 0)
        return;
    long k = i/block_size;
    if (!block_exists(k)) {
        if (x == 0)
            return;
        new_block(k);
        }
    blocks[k][i % block_size] = x;
    } // End of function uint16_big_array::set.

//--------------------------//
//   uint16_big_array::add  //
//--------------------------//
void uint16_big_array::add(long i, uint16 x) {
    if (i < 0 || x == 0)
        return;
    long k = i/block_size;
    if (!block_exists(k))
        new_block(k);
    blocks[k][i % block_size] += x;
    } // End of function uint16_big_array::add.

/*------------------------------------------------------------------------------
uint16_big_array::max_index_nz() returns the index of the non-zero array element
with the highest index. If there is no non-zero element, then -1 is returned.
------------------------------------------------------------------------------*/
//----------------------------------//
//  uint16_big_array::max_index_nz  //
//----------------------------------//
long uint16_big_array::max_index_nz() {
    register uint16** pb = blocks + n_blocks;
    for (long k = n_blocks - 1; k >= 0; --k) {
        register uint16* pl = *--pb;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl != 0)
                return k * block_size + j;
        }
    return -1;
    } // End of function uint16_big_array::max_index_nz.

/*------------------------------------------------------------------------------
uint16_big_array::max_element() returns the maximum value found in the array.
The maximum is always non-negative, since it is considered that the array always
has an infinite number of zero elements.
------------------------------------------------------------------------------*/
//----------------------------------//
//  uint16_big_array::max_element   //
//----------------------------------//
uint16 uint16_big_array::max_element() {
    register uint16 x = 0;
    register uint16** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register uint16* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl > x)  // Optimized for 68000-type architectures!
                x = *pl;
        }
    return x;
    } // End of function uint16_big_array::max_element.

/*------------------------------------------------------------------------------
uint16_big_array::sum_elements() returns the sum of elements found in the array.
------------------------------------------------------------------------------*/
//----------------------------------//
//  uint16_big_array::sum_elements  //
//----------------------------------//
uint16 uint16_big_array::sum_elements() {
    register uint16 x = 0;
    register uint16** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register uint16* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *--pl; // Optimized for 68000-type architectures?
        }
    return x;
    } // End of function uint16_big_array::sum_elements.

/*------------------------------------------------------------------------------
uint16_big_array::dsum_elements() returns the (double) sum of elements in array.
------------------------------------------------------------------------------*/
//----------------------------------//
//  uint16_big_array::dsum_elements //
//----------------------------------//
double uint16_big_array::dsum_elements() {
    register double x = 0;
    register uint16** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register uint16* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *pl;
        }
    return x;
    } // End of function uint16_big_array::dsum_elements.

//----------------------------------//
//  long_big_array::~long_big_array //
//----------------------------------//
long_big_array::~long_big_array() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    } // End of function long_big_array::~long_big_array.

/*------------------------------------------------------------------------------
long_big_array::resize takes the index "k" of the block pointer which should be
made available, NOT the index "i" of the long integer itself.
------------------------------------------------------------------------------*/
//--------------------------//
//  long_big_array::resize  //
//--------------------------//
void long_big_array::resize(long k) {
    if (k < n_blocks)
        return;
    long new_n_blocks = block_quantum * (1 + k/block_quantum);
    long** new_blocks = new long*[new_n_blocks];
    long** pp = blocks;
    long** npp = new_blocks;
    long j = 0;
    for (j = 0; j < n_blocks; ++j)
        *npp++ = *pp++;
    for ( ; j < new_n_blocks; ++j)
        *npp++ = 0;
    delete[] blocks;
    blocks = new_blocks;
    n_blocks = new_n_blocks;
    } // End of function long_big_array::resize.

//------------------------------//
//   long_big_array::new_block  //
//------------------------------//
void long_big_array::new_block(long k) {
    if (k < 0)
        return;
    if (k >= n_blocks)
        resize(k);
    if (!blocks[k]) {
        register long* pl = blocks[k] = new long[block_size];
        for (long j = 0; j < block_size; ++j)
            *pl++ = 0;
        }
    } // End of function long_big_array::new_block.

//----------------------//
//  long_big_array::get //
//----------------------//
long long_big_array::get(long i) {
    if (i < 0)
        return 0;
    long k = i/block_size;
    if (!block_exists(k))
        return 0;
    return blocks[k][i % block_size];
    } // End of function long_big_array::get.

//----------------------//
//  long_big_array::set //
//----------------------//
void long_big_array::set(long i, long x) {
    if (i < 0)
        return;
    long k = i/block_size;
    if (!block_exists(k)) {
        if (x == 0)
            return;
        new_block(k);
        }
    blocks[k][i % block_size] = x;
    } // End of function long_big_array::set.

//----------------------//
//  long_big_array::add //
//----------------------//
void long_big_array::add(long i, long x) {
    if (i < 0 || x == 0)
        return;
    long k = i/block_size;
    if (!block_exists(k))
        new_block(k);
    blocks[k][i % block_size] += x;
    } // End of function long_big_array::add.

/*------------------------------------------------------------------------------
long_big_array::max_index_nz() returns the index of the non-zero array element
with the highest index. If there is no non-zero element, then -1 is returned.
------------------------------------------------------------------------------*/
//----------------------------------//
//   long_big_array::max_index_nz   //
//----------------------------------//
long long_big_array::max_index_nz() {
    register long** pb = blocks + n_blocks;
    for (long k = n_blocks - 1; k >= 0; --k) {
        register long* pl = *--pb;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl != 0)
                return k * block_size + j;
        }
    return -1;
    } // End of function long_big_array::max_index_nz.

/*------------------------------------------------------------------------------
long_big_array::max_element() returns the maximum value found in the array. The
maximum is always non-negative, since it is considered that the array always has
an infinite number of zero elements.
------------------------------------------------------------------------------*/
//----------------------------------//
//   long_big_array::max_element    //
//----------------------------------//
long long_big_array::max_element() {
    register long x = 0;
    register long** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register long* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl > x)  // Optimized for 68000-type architectures!
                x = *pl;
        }
    return x;
    } // End of function long_big_array::max_element.

/*------------------------------------------------------------------------------
long_big_array::sum_elements() returns the sum of elements found in the array.
------------------------------------------------------------------------------*/
//----------------------------------//
//   long_big_array::sum_elements   //
//----------------------------------//
long long_big_array::sum_elements() {
    register long x = 0;
    register long** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register long* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *--pl; // Optimized for 68000-type architectures?
        }
    return x;
    } // End of function long_big_array::sum_elements.

/*------------------------------------------------------------------------------
long_big_array::dsum_elements() returns the (double) sum of elements in array.
------------------------------------------------------------------------------*/
//----------------------------------//
//  long_big_array::dsum_elements   //
//----------------------------------//
double long_big_array::dsum_elements() {
    register double x = 0;
    register long** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register long* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *pl;
        }
    return x;
    } // End of function long_big_array::dsum_elements.

//--------------------------------------//
//  double_big_array::~double_big_array //
//--------------------------------------//
double_big_array::~double_big_array() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    } // End of function double_big_array::~double_big_array.

/*------------------------------------------------------------------------------
double_big_array::resize takes the index "k" of the block pointer which should
be made available, NOT the index "i" of the double itself.
------------------------------------------------------------------------------*/
//--------------------------//
// double_big_array::resize //
//--------------------------//
void double_big_array::resize(long k) {
    if (k < n_blocks)
        return;
    long new_n_blocks = block_quantum * (1 + k/block_quantum);
    double** new_blocks = new double*[new_n_blocks];
    double** pp = blocks;
    double** npp = new_blocks;
    long j = 0;
    for (j = 0; j < n_blocks; ++j)
        *npp++ = *pp++;
    for ( ; j < new_n_blocks; ++j)
        *npp++ = 0;
    delete[] blocks;
    blocks = new_blocks;
    n_blocks = new_n_blocks;
    } // End of function double_big_array::resize.

//------------------------------//
//  double_big_array::new_block //
//------------------------------//
void double_big_array::new_block(long k) {
    if (k < 0)
        return;
    if (k >= n_blocks)
        resize(k);
    if (!blocks[k]) {
        register double* pl = blocks[k] = new double[block_size];
        for (long j = 0; j < block_size; ++j)
            *pl++ = 0;
        }
    } // End of function double_big_array::new_block.

//--------------------------//
//   double_big_array::get  //
//--------------------------//
double double_big_array::get(long i) {
    if (i < 0)
        return 0;
    long k = i/block_size;
    if (!block_exists(k))
        return 0;
    return blocks[k][i % block_size];
    } // End of function double_big_array::get.

//--------------------------//
//   double_big_array::set  //
//--------------------------//
void double_big_array::set(long i, double x) {
    if (i < 0)
        return;
    long k = i/block_size;
    if (!block_exists(k)) {
        if (x == 0)
            return;
        new_block(k);
        }
    blocks[k][i % block_size] = x;
    } // End of function double_big_array::set.

//--------------------------//
//   double_big_array::add  //
//--------------------------//
void double_big_array::add(long i, double x) {
    if (i < 0 || x == 0)
        return;
    long k = i/block_size;
    if (!block_exists(k))
        new_block(k);
    blocks[k][i % block_size] += x;
    } // End of function double_big_array::add.

/*------------------------------------------------------------------------------
double_big_array::max_index_nz() returns the index of the non-zero array element
with the highest index. If there is no non-zero element, then -1 is returned.
------------------------------------------------------------------------------*/
//----------------------------------//
//  double_big_array::max_index_nz  //
//----------------------------------//
long double_big_array::max_index_nz() {
    register double** pb = blocks + n_blocks;
    for (long k = n_blocks - 1; k >= 0; --k) {
        register double* pl = *--pb;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl != 0)
                return k * block_size + j;
        }
    return -1;
    } // End of function double_big_array::max_index_nz.

/*------------------------------------------------------------------------------
double_big_array::max_element() returns the maximum value found in the array.
The maximum is always non-negative, since it is considered that the array always
has an infinite number of zero elements.
------------------------------------------------------------------------------*/
//----------------------------------//
//  double_big_array::max_element   //
//----------------------------------//
double double_big_array::max_element() {
    register double x = 0;
    register double** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register double* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            if (*--pl > x)  // Optimized for 68000-type architectures!
                x = *pl;
        }
    return x;
    } // End of function double_big_array::max_element.

/*------------------------------------------------------------------------------
double_big_array::sum_elements() returns the sum of elements found in the array.
------------------------------------------------------------------------------*/
//----------------------------------//
//  double_big_array::sum_elements  //
//----------------------------------//
double double_big_array::sum_elements() {
    register double x = 0;
    register double** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register double* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *--pl; // Optimized for 68000-type architectures?
        }
    return x;
    } // End of function double_big_array::sum_elements.

/*------------------------------------------------------------------------------
double_big_array::dsum_elements() returns the (double) sum of elements in array.
------------------------------------------------------------------------------*/
//----------------------------------//
// double_big_array::dsum_elements  //
//----------------------------------//
double double_big_array::dsum_elements() {
    register double x = 0;
    register double** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register double* pl = *pb++;
        if (!pl)
            continue;
        pl += block_size;
        for (long j = block_size - 1; j >= 0; --j)
            x += *pl;
        }
    return x;
    } // End of function double_big_array::dsum_elements.
