// src/aksl/hashfn.h   2018-3-3   Alan U. Kennington.
// $Id: src/aksl/include/aksl/hashfn.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_HASHFN_H
#define AKSL_HASHFN_H
/*------------------------------------------------------------------------------
Classes in this file:

hashtab_32_8::
hashtab_32_16::
hashtab_str_8::
------------------------------------------------------------------------------*/

#ifndef AKSL_COD_H
#include "aksl/cod.h"
#endif
#ifndef AKSL_VPLIST_H
#include "aksl/vplist.h"
#endif

const int tab8size  = 256;
const int tab16size = 65536;

// Inline functions for hashing small objects down to smaller objects:
inline uint8  hashfn_32_8(uint32 key) { return crc8calc_rl(key); }
inline uint16 hashfn_32_16(uint32 key) { return crc16calc_rl(key); }
inline uint8  hashfn_str_8(const char* key) { return crc8calc(key); }
inline uint16 hashfn_str_16(const char* key) { return crc16calc(key); }
inline uint32 hashfn_str_32(const char* key) { return crc32calc(key); }

inline uint8  hashfn_d_8(double key)
    { return crc8calc((const char*)&key, sizeof(double)); }
inline uint16 hashfn_d_16(double key)
    { return crc16calc((const char*)&key, sizeof(double)); }
inline uint32 hashfn_d_32(double key)
    { return crc32calc((const char*)&key, sizeof(double)); }

/*------------------------------------------------------------------------------
This table stores and retrieves void* pointers according to 32-bit keys, using
an 8-bit hash table containing lists of void* pointers.
Warning: sizeof(hashtab_32_8) = 2056.
Warning: if void* pointers are not 32-bit, then this class will obviously fail.
------------------------------------------------------------------------------*/
//----------------------//
//     hashtab_32_8::   //
//----------------------//
struct hashtab_32_8 {
private:
    voidptrkeylist table[tab8size]; // 256 lists of (void*, key) pairs.
    voidptrkey* trav_ptr;           // For traversals.
    int         trav_i;             // For traversals.
public:
//    void insert(const void* data, uint32 key); // Over-writes old entry.
    void insert(void* data, uint32 key); // Over-writes old entry.
    void append(void* data, uint32 key);    // Does not over-write old entry.
    void prepend(void* data, uint32 key);   // Does not over-write old entry.
    bool_enum find(void*& data, uint32 key);
    bool_enum del(uint32 key);              // Delete entry for given key.

    // These versions only work if a void* fits into a uint32.
    void insert(void* data, void* key) { insert(data, uint32(key)); }
    void append(void* data, void* key) { append(data, uint32(key)); }
    void prepend(void* data, void* key) { prepend(data, uint32(key)); }
    bool_enum find(void*& data, void* key) { return find(data, uint32(key)); }
    bool_enum del(void* key) { return del(uint32(key)); }

    // Kludge versions which take a "double" key (using double hashing):
    // Should replace these with single hashing.
    // Danger, danger! Don't use these functions. Non-unique storage!!!!
    void insert(void* data, double key) { insert(data, hashfn_d_32(key)); }
    void append(void* data, double key) { append(data, hashfn_d_32(key)); }
    void prepend(void* data, double key) { prepend(data, hashfn_d_32(key)); }
    bool_enum find(void*& data, double key)
        { return find(data, hashfn_d_32(key)); }

    long n_entries();                   // Number of entries in table.
    voidptrkey* first(int* pi = 0);     // First element of traversal.
    voidptrkey* next(int* pi = 0);      // Next element of traversal.

//    hashtab_32_8& operator=(const hashtab_32_8& x) {}
//    hashtab_32_8(const hashtab_32_8& x) {};
    hashtab_32_8() { trav_ptr = 0; trav_i = 0; }
    ~hashtab_32_8() {}
    }; // End of struct hashtab_32_8.

/*------------------------------------------------------------------------------
This table stores and retrieves void* pointers according to 32-bit keys, using
an 16-bit hash table containing lists of void* pointers.
Warning: sizeof(hashtab_32_16) = 524296.
------------------------------------------------------------------------------*/
//----------------------//
//    hashtab_32_16::   //
//----------------------//
struct hashtab_32_16 {
private:
    voidptrkeylist table[tab16size];// 65536 lists of (void*, key) pairs.
    voidptrkey* trav_ptr;           // For traversals.
    int         trav_i;             // For traversals.
public:
    void insert(void* data, uint32 key);    // Over-writes old entry.
    void append(void* data, uint32 key);    // Does not over-write old entry.
    void prepend(void* data, uint32 key);   // Does not over-write old entry.
    bool_enum find(void*& data, uint32 key);

    // These versions only work if a void* fits into a uint32.
    void insert(void* data, void* key) { insert(data, uint32(key)); }
    void append(void* data, void* key) { append(data, uint32(key)); }
    void prepend(void* data, void* key) { prepend(data, uint32(key)); }
    bool_enum find(void*& data, void* key) { return find(data, uint32(key)); }

    // Kludge versions which take a "double" key (using double hashing):
    // Should replace these with single hashing.
    void insert(void* data, double key) { insert(data, hashfn_d_32(key)); }
    void append(void* data, double key) { append(data, hashfn_d_32(key)); }
    void prepend(void* data, double key) { prepend(data, hashfn_d_32(key)); }
    bool_enum find(void*& data, double key)
        { return find(data, hashfn_d_32(key)); }

    long n_entries();                   // Number of entries in table.
    voidptrkey* first(int* pi = 0);     // First element of traversal.
    voidptrkey* next(int* pi = 0);      // Next element of traversal.

//    hashtab_32_16& operator=(const hashtab_32_16& x) {}
//    hashtab_32_16(const hashtab_32_16& x) {};
    hashtab_32_16() { trav_ptr = 0; trav_i = 0; }
    ~hashtab_32_16() {}
    }; // End of struct hashtab_32_16.

/*------------------------------------------------------------------------------
This table stores and retrieves void* pointers according to char-string keys,
using an 8-bit hash table containing lists of void* pointers.
Warning: sizeof(hashtab_str_8) = ....
------------------------------------------------------------------------------*/
//----------------------//
//    hashtab_str_8::   //
//----------------------//
struct hashtab_str_8 {
private:
    voidptrstrlist table[tab8size]; // 256 lists of (void*, key) pairs.
    voidptrstr* trav_ptr;           // For traversals.
    int         trav_i;             // For traversals.
public:
    void insert(void* data, const char* key);  // Over-writes old entry.
    void append(void* data, const char* key);  // Does not over-write old entry.
    void prepend(void* data, const char* key); // Does not over-write old entry.
    bool_enum find(void*& data, const char* key);

    // Warning: The following only work if sizeof(uint32) == sizeof(void*) !!!
    void insert(uint32 data, const char* key)  // Over-writes old entry.
        { insert((void*)data, key); }
    void append(uint32 data, const char* key)  // Does not over-write old entry.
        { append((void*)data, key); }
    void prepend(uint32 data, const char* key) // Does not over-write old entry.
        { prepend((void*)data, key); }
    bool_enum find(uint32& data, const char* key)
        { void* p = 0; bool_enum x = find(p, key); data = uint32(p); return x; }

    long n_entries();                   // Number of entries in table.
    voidptrstr* first(int* pi = 0);     // First element of traversal.
    voidptrstr* next(int* pi = 0);      // Next element of traversal.

//    hashtab_str_8& operator=(const hashtab_str_8& x) {}
//    hashtab_str_8(const hashtab_str_8& x) {};
    hashtab_str_8() { trav_ptr = 0; trav_i = 0; }
    ~hashtab_str_8() {}
    }; // End of struct hashtab_str_8.

#endif /* AKSL_HASHFN_H */
