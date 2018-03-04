// src/aksl/charbuf.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_CHARBUF_H
#define AKSL_CHARBUF_H
/*------------------------------------------------------------------------------
Classes in this file:

charbuf::
charbuff::
c_array::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_NBYTES_H
#include "aksl/nbytes.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_FSTREAM_H
#define AKSL_X_FSTREAM_H
#include <fstream>
#endif
#ifndef AKSL_X_OSTREAM_H
#define AKSL_X_OSTREAM_H
#include <ostream>
#endif

enum cbmode_t {             // Modes for reading/writing files into charbuff:
    cbRAM,                  // Default: file is accessed in "core" memory.
    cbDISK                  // File is maintained entirely on disk.
                            // There may be demand paged versions later...
    };

// Default parameters for the charbuf class:
// (The fine-tuning of these could possibly affect efficiency.)
static const long deft_charblock_size       = 64;
static const long deft_charblock_quantum    = 64;

/*------------------------------------------------------------------------------
charbuf:: is a class of arrays of chars. It is implemented as a variable-length
array of pointers to blocks of chars. The pointers are only assigned to new
blocks if a reference requires it. Similarly, the array of pointers is only
extended (by a quantum) if a reference makes this necessary.
Thus a charbuf is like a demand-paged char array in memory, with the added
feature that the page table is extensible.
------------------------------------------------------------------------------*/
//----------------------//
//       charbuf::      //
//----------------------//
struct charbuf {
private:
    long charblock_size;
    long charblock_quantum;
    long n_blocks;              // Number of blocks.
    char** blocks;              // Array of pointers to blocks.
    long next_char;             // Next reading/writing position.

    bool_enum block_exists(long k)    // For internal use.
        { return (bool_enum)(k >= 0 && k < n_blocks && blocks[k]); }
    void resize_block(long);    // For internal use.
    void new_block(long);       // For internal use.
public:
    // Random access functions.
    char get(long i);           // Get element i.
    void set(long i, char x);   // Set element i to x.
    void add(long i, char x);   // Add x to element i.
    long max_index_nz();        // Returns max index of non-zero element.
    long max_element();         // Returns max of all elements in the array.
    long sum_elements();        // Returns sum of all elements in the array.
    double dsum_elements();     // Returns (double) sum of elements in array.
    int get_bytes(long i, char* buf, long n);    // Read n bytes starting at i.
    int get_bytes(nbytes& nbuf, long i, long n); // Read n bytes starting at i.
    void set_bytes(long i, const char*, long n); // Set n bytes starting at i.

    // Functions for input buffer use (only these functions apply next_char).
    void append(char x) { set(next_char, x); next_char += 1; }
    char* copy();               // Get a copy of the current buffer contents.
    char* copy0();              // Get a null-terminated copy.
    void append_bytes(const char*, long n);      // Append n bytes.
    void append_bytes(const nbytes& buf)         // Append n bytes.
        { append_bytes(buf.bytes(), buf.n_bytes()); }
    long n_bytes() { return next_char; }
    void set_n_bytes(long n) { if (n >= 0) next_char = n; } // Risky!??
    void reset() { next_char = 0; }         // Clear without freeing memory.
    void clear();                           // Demolish everything.
    long read(const char* file_name);       // Read a file.
//    long write(const char* file_name);      // Write buffer to a file.
    long write(ostream& os);                // Write buffer to a stream.

//    charbuf& operator=(const charbuf& x) {}
//    charbuf(const charbuf& x) {};
    charbuf(long s = deft_charblock_size, long q = deft_charblock_quantum) {
        charblock_size      = (s > 0) ? s : deft_charblock_size;
        charblock_quantum   = (q > 0) ? q : deft_charblock_quantum;
        n_blocks            = 0;
        blocks              = 0;
        next_char           = 0;
        }
    ~charbuf();
    }; // End of struct charbuf.

/*------------------------------------------------------------------------------
Do not use this class. It is not ready to use yet.
This is a modification of "charbuf" to permit the buffer to be resident in a
disk file.
All of the behaviour should be the same as for "charbuf" if the "mode" is not
changed from the default. But this class uses more memory for storing info about
the disk file etc. There is also extra time overhead for determining which
access mode is being used. Hence if you are sure you don't want the disk file
resident option(s), you should use "charbuf" to save time and space.
------------------------------------------------------------------------------*/
//----------------------//
//       charbuff::     //
//----------------------//
struct charbuff : public charbuf {
private:
    short       mode;           // Currently this is for both read and write.
    bool_int    mode_locked;    // True if mode is locked.
    bool_int    open;           // True if file is open.
    filebuf     fbuf;           // Buffer for the disk file (if any).
    char*       filename;       // Name of file.
public:
    cbmode_t set_mode(cbmode_t m)
        { if (!mode_locked) mode = m; return (cbmode_t)mode; }
    cbmode_t get_mode() { return (cbmode_t)mode; }

    // Re-defined members of "charbuf":
    long read(const char* file_name);       // Read a file (possibly delayed).

//    charbuff& operator=(const charbuff& x) {}
//    charbuff(const charbuff& x) {};
    charbuff(long s = deft_charblock_size, long q = deft_charblock_quantum)
        : charbuf(s, q) {
        mode                = cbRAM;
        mode_locked         = false;
        open                = false;
        filename            = 0;
        }
    ~charbuff() {}
    }; // End of struct charbuff.

/*------------------------------------------------------------------------------
This class just manages a char array. If the array needs to be extended, a new
array is allocated and the old array is copied to it. The user may get a copy of
it at any time. If required, a \0 is appended when needed.
------------------------------------------------------------------------------*/
//----------------------//
//       c_array::      //
//----------------------//
struct c_array {
private:
    long len;                   // Number of chars in char array.
    char* chars;                // Char array.
    long min_len_step;          // Minimum increment for resizing.
    long next_char;             // Next reading/writing position.

    void resize(long);          // For internal use.
public:
    // Random access:
    char get(long i) { return (i < 0 || i >= len) ? 0 : chars[i]; }
    void set(long i, char x);   // Set element i to x.

    // Sequential access:
    void append(char x) { set(next_char, x); next_char += 1; }
    void append(const char*, long precision = 0);   // Append a string.
    void append_bytes(const char*, long n);         // Append n bytes.
    long length() { return next_char; }
    char* copy();               // Get a copy of the current buffer contents.
    char* copy0();              // Get a null-terminated copy.
    operator const char*();     // Return null-terminated constant char array.
    void reset() { next_char = 0; } // Clear without freeing memeory.
    void clear();               // Demolish everything.

//    c_array& operator=(const c_array& x) {}
//    c_array(const c_array& x) {};
    c_array() { len = 0; chars = 0; min_len_step = 10; next_char = 0; }
    ~c_array() { delete[] chars; }
    }; // End of struct c_array.

#endif /* AKSL_CHARBUF_H */
