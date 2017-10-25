// src/aksl/charbuf.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

charbuf::
    ~charbuf
    resize_block
    new_block
    get
    set
    add
    max_index_nz
    max_element
    get_bytes
    get_bytes
    set_bytes
    copy
    copy0
    append_bytes
    read
    write
c_array::
    clear
    resize
    set
    copy
    copy0
    operator const char*
    append
    append_bytes
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/charbuf.h"
#ifndef AKSL_NEWSTR_H
#include "aksl/newstr.h"
#endif

// System header files:
#ifndef AKSL_X_MEMORY_H
#define AKSL_X_MEMORY_H
#include <memory.h>
#endif

//--------------------------//
//     charbuf::~charbuf    //
//--------------------------//
charbuf::~charbuf() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    } // End of function charbuf::~charbuf.

//--------------------------//
//      charbuf::clear      //
//--------------------------//
void charbuf::clear() {
    for (long i = 0; i < n_blocks; ++i)
        delete blocks[i];
    delete[] blocks;
    n_blocks = 0;
    blocks = 0;
    next_char = 0;
    } // End of function charbuf::clear.

/*------------------------------------------------------------------------------
For internal use only.
charbuf::resize_block takes the index "k" of the block pointer which should be
made available, NOT the index "i" of the long integer itself.
------------------------------------------------------------------------------*/
//--------------------------//
//  charbuf::resize_block   //
//--------------------------//
void charbuf::resize_block(long k) {
    if (k < n_blocks)
        return;
    long new_n_blocks = charblock_quantum * (1 + k/charblock_quantum);
    char** new_blocks = new char*[new_n_blocks];
    char** pp = blocks;
    char** npp = new_blocks;
    long j = 0;
    for (j = 0; j < n_blocks; ++j)
        *npp++ = *pp++;
    for ( ; j < new_n_blocks; ++j)
        *npp++ = 0;
    delete[] blocks;
    blocks = new_blocks;
    n_blocks = new_n_blocks;
    } // End of function charbuf::resize_block.

/*------------------------------------------------------------------------------
For internal use only.
------------------------------------------------------------------------------*/
//--------------------------//
//    charbuf::new_block    //
//--------------------------//
void charbuf::new_block(long k) {
    if (k < 0)
        return;
    if (k >= n_blocks)
        resize_block(k);
    if (!blocks[k]) {
        register char* pl = blocks[k] = new char[charblock_size];
        for (long j = 0; j < charblock_size; ++j)
            *pl++ = 0;
        }
    } // End of function charbuf::new_block.

//----------------------//
//     charbuf::get     //
//----------------------//
char charbuf::get(long i) {
    if (i < 0)
        return 0;
    long k = i/charblock_size;
    if (!block_exists(k))
        return 0;
    return blocks[k][i % charblock_size];
    } // End of function charbuf::get.

//----------------------//
//     charbuf::set     //
//----------------------//
void charbuf::set(long i, char x) {
    if (i < 0)
        return;
    long k = i/charblock_size;
    if (!block_exists(k)) {
        if (x == 0)
            return;
        new_block(k);
        }
    blocks[k][i % charblock_size] = x;
    } // End of function charbuf::set.

/*------------------------------------------------------------------------------
Arithmetically add a character to a given array element.
------------------------------------------------------------------------------*/
//----------------------//
//     charbuf::add     //
//----------------------//
void charbuf::add(long i, char x) {
    if (i < 0 || x == 0)
        return;
    long k = i/charblock_size;
    if (!block_exists(k))
        new_block(k);
    blocks[k][i % charblock_size] += x;
    } // End of function charbuf::add.

/*------------------------------------------------------------------------------
charbuf::max_index_nz() returns the index of the non-zero array element
with the highest index. If there is no non-zero element, then -1 is returned.
------------------------------------------------------------------------------*/
//------------------------------//
//     charbuf::max_index_nz    //
//------------------------------//
long charbuf::max_index_nz() {
    register char** pb = blocks + n_blocks;
    for (long k = n_blocks - 1; k >= 0; --k) {
        register char* pl = *--pb;
        if (!pl)
            continue;
        pl += charblock_size;
        for (long j = charblock_size - 1; j >= 0; --j)
            if (*--pl != 0)
                return k * charblock_size + j;
        }
    return -1;
    } // End of function charbuf::max_index_nz.

/*------------------------------------------------------------------------------
charbuf::max_element() returns the maximum value found in the array. The
maximum is always non-negative, since it is considered that the array always has
an infinite number of zero elements.
------------------------------------------------------------------------------*/
//----------------------//
// charbuf::max_element //
//----------------------//
long charbuf::max_element() {
    register char x = 0;
    register char** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register char* pl = *pb++;
        if (!pl)
            continue;
        pl += charblock_size;
        for (long j = charblock_size - 1; j >= 0; --j)
            if (*--pl > x)  // Optimized for 68000-type architectures!
                x = *pl;
        }
    return x;
    } // End of function charbuf::max_element.

/*------------------------------------------------------------------------------
charbuf::sum_elements() returns the sum of elements found in the array.
------------------------------------------------------------------------------*/
//------------------------------//
//    charbuf::sum_elements     //
//------------------------------//
long charbuf::sum_elements() {
    register char x = 0;
    register char** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register char* pl = *pb++;
        if (!pl)
            continue;
        pl += charblock_size;
        for (long j = charblock_size - 1; j >= 0; --j)
            x += *--pl; // Optimized for 68000-type architectures?
        }
    return x;
    } // End of function charbuf::sum_elements.

/*------------------------------------------------------------------------------
charbuf::dsum_elements() returns the (double) sum of elements in array.
------------------------------------------------------------------------------*/
//------------------------------//
//    charbuf::dsum_elements    //
//------------------------------//
double charbuf::dsum_elements() {
    register double x = 0;
    register char** pb = blocks;
    for (long k = 0; k < n_blocks; ++k) {
        register char* pl = *pb++;
        if (!pl)
            continue;
        pl += charblock_size;
        for (long j = charblock_size - 1; j >= 0; --j)
            x += *pl;
        }
    return x;
    } // End of function charbuf::dsum_elements.

/*------------------------------------------------------------------------------
Get n bytes from the buffer, starting at offset i. If any part of the
desired interval of the buffer is uninstantiated, the value is taken to
be zero, as usual.
------------------------------------------------------------------------------*/
//----------------------//
//  charbuf::get_bytes  //
//----------------------//
int charbuf::get_bytes(long i, char* buf, long n) {
    if (i < 0 || !buf || n < 0)
        return -1;
    while (n > 0) {
        long k = i/charblock_size;      // Block index.
        long n_copy = charblock_size - i % charblock_size;
        if (n_copy > n)
            n_copy = n;
        if (n_copy <= 0)    // Impossible internal error.
            return -1;
        if (k < n_blocks && blocks[k])
            memcpy(buf, &blocks[k][i % charblock_size], (int)n_copy);
        else
            memset(buf, 0, (int)n_copy);
        i += n_copy;
        buf += n_copy;
        n -= n_copy;
        }
    return 0;
    } // End of function charbuf::get_bytes.

/*------------------------------------------------------------------------------
Get n bytes from the buffer, starting at offset i. If any part of the
desired interval of the buffer is uninstantiated, the value is taken to
be zero, as usual.
------------------------------------------------------------------------------*/
//----------------------//
//  charbuf::get_bytes  //
//----------------------//
int charbuf::get_bytes(nbytes& nbuf, long i, long n) {
    nbuf.clear();
    if (i < 0 || n < 0)
        return -1;
    if (n <= 0)
        return 0;
#if LONG_MAX > INT_MAX
    if (n > INT_MAX)
        return -1;
#endif

    // Allocate an array, and copy bytes to it.
    char* buf = new char[n];
    if (get_bytes(i, buf, n) < 0) {
        delete[] buf;
        return -2;
        }

    // Move the copied bytes into the "nbytes" structure.
    nbuf.swallow(buf, int(n));
    return 0;
    } // End of function charbuf::get_bytes.

/*------------------------------------------------------------------------------
Copy the given n bytes into the charbuf array starting at position "start".
This uses fast copying in blocks.
No check is made to see if the bytes to be inserted are zero or not.
Hence some blocks might be unnecessarily instantiated. But this is better
than wasting time checking all of the given bytes which land in each block.
If space efficiency is very important, the set(i, char) member function can be
called repeatedly to minimise unnecessary instantiation of blocks.
Note that next_char is neither read nor written, so that bytes may appear
beyond the position indicated by next_char.
------------------------------------------------------------------------------*/
//----------------------//
//  charbuf::set_bytes  //
//----------------------//
void charbuf::set_bytes(long start, const char* buf, long n) {
    if (start < 0 || !buf || n <= 0)
        return;

    // Copy the given byte array:
    const char* pc1 = buf;                  // Position in array being read.
    long k = start / charblock_size;        // Block to write to.
    long i = start % charblock_size;        // Index within the block.
    long bytes_copied = 0;                  // Bytes copied in each loop.
    for (long bytes_left = n; bytes_left > 0; bytes_left -= bytes_copied) {
        // Instantiate the block if necessary:
        if (!block_exists(k))
            new_block(k);

        // Calculate the number of bytes to copy:
        // (It is important that bytes_copied is always positive.)
        bytes_copied = charblock_size - i;
        if (bytes_copied > bytes_left)
            bytes_copied = bytes_left;

        // Emergency exit in case the arithmetic breaks down somehow.
        if (bytes_copied < 0)
            break;

        // Copy bytes from pc1 to the current block:
        memcpy(&blocks[k][i], pc1, (int)bytes_copied);

        // Move forward in the charbuf array being written to:
        k += 1;
        i = 0;

        // Move forward in the char array being read from:
        pc1 += bytes_copied;
        }
    } // End of function charbuf::set_bytes.

/*------------------------------------------------------------------------------
Bytewise heap-allocated copy of bytes up to next_char, using the slow
bytewise get() function. No null-termination.
------------------------------------------------------------------------------*/
//----------------------//
//     charbuf::copy    //
//----------------------//
char* charbuf::copy() {
    char* pc = new char[next_char];
    char* pc2 = pc;
    for (long i = 0; i < next_char; ++i)
        *pc2++ = get(i);
    return pc;
    } // End of function charbuf::copy.

/*------------------------------------------------------------------------------
Bytewise heap-allocated copy of bytes up to next_char, using the slow
bytewise get() function. A null byte _is_ appended.
------------------------------------------------------------------------------*/
//----------------------//
//    charbuf::copy0    //
//----------------------//
char* charbuf::copy0() {
    char* pc = new char[next_char + 1];
    char* pc2 = pc;
    for (long i = 0; i < next_char; ++i)
        *pc2++ = get(i);
    *pc2 = 0;
    return pc;
    } // End of function charbuf::copy0.

/*------------------------------------------------------------------------------
Append the given n bytes to the array. This uses fast copying in blocks.
No check is made to see if the bytes to be appended are zero or not.
Hence some blocks might be unnecessarily instantiated. But this is better
than wasting time checking all of the given bytes which land in each block.
If space efficiency is very important, the set() function can be called
repeatedly to minimise unnecessary instantiation of blocks.
The next_char index is incremented by this operation if bytes are appended.
------------------------------------------------------------------------------*/
//--------------------------//
//   charbuf::append_bytes  //
//--------------------------//
void charbuf::append_bytes(const char* buf, long n) {
    if (!buf || n <= 0)
        return;

    // Copy the given byte array:
    const char* pc1 = buf;                  // Position in array being read.
    long k = next_char / charblock_size;    // Block to write to.
    long i = next_char % charblock_size;    // Index within the block.
    long bytes_copied = 0;                  // Bytes copied in each loop.
    for (long bytes_left = n; bytes_left > 0; bytes_left -= bytes_copied) {
        // Instantiate the block if necessary:
        if (!block_exists(k))
            new_block(k);

        // Calculate the number of bytes to copy:
        // (It is important that bytes_copied is always positive.)
        bytes_copied = charblock_size - i;
        if (bytes_copied > bytes_left)
            bytes_copied = bytes_left;

        // Emergency exit in case the arithmetic breaks down somehow.
        if (bytes_copied < 0)
            break;

        // Copy bytes from pc1 to the current block:
        memcpy(&blocks[k][i], pc1, (int)bytes_copied);

        // Move forward in the charbuf array being written to:
        k += 1;
        i = 0;

        // Move forward in the char array being read from:
        pc1 += bytes_copied;
        }

    // Update the current read/write position:
    next_char += n;
    } // End of function charbuf::append_bytes.

/*------------------------------------------------------------------------------
This routine reads a file into a character buffer.
Return value:
-1          failure
>= 0        number of bytes read from file.
Note that for win32 interface, CR-LF to LF translation is not done.
------------------------------------------------------------------------------*/
//----------------------//
//     charbuf::read    //
//----------------------//
long charbuf::read(const char* file_name) {
    if (!file_name || !*file_name)
        return -1;

    // Open the input file:
    ifstream ifs;
#ifndef WIN32
    ifs.open(file_name, ios::in);
#else
    ifs.open(file_name, ios::in | ios:: binary);
#endif
    if (!ifs) {
        cout << "Error: could not open file " << file_name << endl;
        return -1;
        }

    // Read the file:
    const int bufsize = 8192;
    char buf[bufsize];
    long total_bytes = 0;
    for (;;) {
        ifs.read(buf, bufsize);
        long n_bytes = ifs.gcount();
        if (n_bytes <= 0)
            break;
        total_bytes += n_bytes;
        append_bytes(buf, n_bytes);
        if (!ifs)
            break;
        }
    ifs.close();
    return total_bytes;
    } // End of function charbuf::read.

/*------------------------------------------------------------------------------
This very simple method of writing to a file only writes the bytes up to
the next_char position. I.e. if the byte-appending methods are not used,
then this function will write out 0 bytes.
This function should check the ostream error return codes,
but I'm in a hurry. Some other day...
------------------------------------------------------------------------------*/
//----------------------//
//    charbuf::write    //
//----------------------//
long charbuf::write(ostream& os) {
    if (!blocks)
        return 0;
    long n_bytes = next_char;   // Number of bytes left to write.
    long n_block = 0;           // Current block to write.
    while (n_bytes > 0) {
        // Calculate number of bytes to write from this block.
        long n = charblock_size;
        if (n > n_bytes)
            n = n_bytes;

        // Get the next block.
        char* blk = (n_block < n_blocks) ? blocks[n_block] : 0;
        if (!blk) {
            // This doesn't need to be efficient, because in byte-append
            // mode, a block of all zeros will be extremely rare, I hope.
            for (int i = 0; i < n; ++i)
                os.put((char)0);
            }
        else {
            // Copy the block or part-block to the stream.
            os.write(blk, (int)n);
            }
        // Update the for-loop variables.
        n_bytes -= n;
        n_block += 1;
        }
    return next_char;
    } // End of function charbuf::write.

/*------------------------------------------------------------------------------
This is not yet written!!
This routine reads a file into a character buffer.
Return value:
-1          failure
>= 0        number of bytes read from (or found in) file.
If the mode is cbDISK, don't actually read anything. Just open the file.
------------------------------------------------------------------------------*/
//----------------------//
//    charbuff::read    //
//----------------------//
long charbuff::read(const char* file_name) {
    // Commit to using the current mode.
    mode_locked = true;

    if (mode == cbRAM)
        return charbuf::read(file_name);
    if (mode != cbDISK)
        return -1;

    if (!file_name || !*file_name)
        return -1;

    // Open the input file:
#ifndef WIN32
    if (!fbuf.open(file_name, ios::in)) {
#else
    if (!fbuf.open(file_name, ios::in | ios::binary)) {
#endif
        cout << "Error: could not open file " << file_name << endl;
        return -1;
        }
    filename = new_strcpy(file_name);
    open = true;

    // Find out how long the file is:
    long total_bytes = 0;
    // ....

    return total_bytes;
    } // End of function charbuff::read.

//----------------------//
//    c_array::clear    //
//----------------------//
void c_array::clear() {
    delete[] chars;
    len = 0;
    chars = 0;
    next_char = 0;
    } // End of function c_array::clear.

/*------------------------------------------------------------------------------
The argument is the desired new length of the array. If the desired length is
not much larger than the current length, then a larger length is chosen.
------------------------------------------------------------------------------*/
//--------------------------//
//      c_array::resize     //
//--------------------------//
void c_array::resize(long new_len) {
    if (new_len <= len)
        return;
    if (new_len < len + min_len_step)
        new_len = len + min_len_step;
    char* new_chars = new char[new_len];
    register char* pp = chars;
    register char* npp = new_chars;
    register long j = 0;
    for (j = 0; j < len; ++j)
        *npp++ = *pp++;
    for ( ; j < new_len; ++j)
        *npp++ = 0;
    delete[] chars;
    chars = new_chars;
    len = new_len;
    } // End of function c_array::resize.

//----------------------//
//     c_array::set     //
//----------------------//
void c_array::set(long i, char x) {
    if (i < 0)
        return;
    if (i < len) {
        chars[i] = x;
        return;
        }
    if (x == 0) // Chars beyond the physical array are assumed to be zero.
        return;

    // If absolutely necessary, extend the array:
    resize(i + 1);
    chars[i] = x;
    } // End of function c_array::set.

//----------------------//
//     c_array::copy    //
//----------------------//
char* c_array::copy() {
    char* pc = new char[next_char];
    char* pc1 = chars;
    char* pc2 = pc;
    long phys_length = next_char;
    if (phys_length >= len)
        phys_length = len;
    long i = 0;
    for (i = 0; i < phys_length; ++i)
        *pc2++ = *pc1++;
    for ( ; i < next_char; ++i)
        *pc2++ = 0;
    return pc;
    } // End of function c_array::copy.

/*------------------------------------------------------------------------------
A null-terminated copy of the char array.
------------------------------------------------------------------------------*/
//----------------------//
//    c_array::copy0    //
//----------------------//
char* c_array::copy0() {
    char* pc = new char[next_char + 1];
    char* pc1 = chars;
    char* pc2 = pc;
    long phys_length = next_char;
    if (phys_length >= len)
        phys_length = len;
    long i = 0;
    for (i = 0; i < phys_length; ++i)
        *pc2++ = *pc1++;
    for ( ; i < next_char; ++i)
        *pc2++ = 0;
    *pc2 = 0;
    return pc;
    } // End of function c_array::copy0.

/*------------------------------------------------------------------------------
Returns pointer to null-terminated string. This function is inteded for
inspecting the string only. It should not be incorporated into any other data
structure.
------------------------------------------------------------------------------*/
//------------------------------//
// c_array::operator const char*//
//------------------------------//
c_array::operator const char*() {
    if (next_char >= len)
        resize(next_char + 1);
    set(next_char, 0);  // Don't increment next_char.
    return chars;
    } // End of function c_array::operator const char*.

/*------------------------------------------------------------------------------
Append the given string to the array. If "precision" > 0, append at most
"precision" characters.
------------------------------------------------------------------------------*/
//----------------------//
//    c_array::append   //
//----------------------//
void c_array::append(const char* s, long precision) {
    if (!s || !*s)          // if (nullstr(s)) ...
        return;
    const char* pc1 = s;
    while (*pc1)
        ++pc1;
    long n = pc1 - s;       // n = strlen(s).
    if (precision > 0 && n > precision)
        n = precision;
    resize(next_char + n);
    pc1 = s;
    char* pc2 = chars + next_char;
    for (long i = 0; i < n; ++i)
        *pc2++ = *pc1++;
    next_char += n;
    } // End of function c_array::append.

/*------------------------------------------------------------------------------
Append the given n bytes to the array.
------------------------------------------------------------------------------*/
//--------------------------//
//   c_array::append_bytes  //
//--------------------------//
void c_array::append_bytes(const char* buf, long n) {
    if (!buf || n <= 0)
        return;
    resize(next_char + n);
    const char* pc1 = buf;
    char* pc2 = chars + next_char;
    for (long i = 0; i < n; ++i)
        *pc2++ = *pc1++;
    next_char += n;
    } // End of function c_array::append_bytes.
