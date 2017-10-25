// src/aksl/boolvec.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

boole_vec_l::
    boole_vec_l
    boole_vec_l
    instantiate
    free_mem
    clear
    resize
    set_from
    set_from
    copy_from
    copy_to
    print
boole_vec_r::
    boole_vec_r
    free_mem
    clear
    resize
//    copy_from
//    copy_to
------------------------------------------------------------------------------*/

#include "aksl/boolvec.h"

//--------------------------//
// boole_vec_l::boole_vec_l //
//--------------------------//
boole_vec_l::boole_vec_l(long s) {
    if (s > 0) {
        length = s;
        nblocks = ceil_div32(s);
        vec = new uint32[nblocks];
        clear();
        }
    else {
        length = 0;
        nblocks = 0;
        vec = 0;
        }
    } // End of function boole_vec_l::boole_vec_l.

/*------------------------------------------------------------------------------
Just copy the whole given array into "vec" and set the other members.
The given array "buf" is assumed to consist of "buflen" 8-bit bytes.
Hence "length" becomes buflen * 8, and "nblocks" becomes ceiling(buflen/4).
------------------------------------------------------------------------------*/
//--------------------------//
// boole_vec_l::boole_vec_l //
//--------------------------//
boole_vec_l::boole_vec_l(const char* buf, long buflen) {
    // Check the sanity of the arguments.
    if (!buf || buflen <= 0) {
        length = 0;
        nblocks = 0;
        vec = 0;
        return;
        }

    // Length in bits = number of bytes * 8.
    length = buflen * 8;                // Hope that this doesn't overflow!

    // Number of 32-bit blocks = ceiling(nbytes/4).
    nblocks = ceil_div4(buflen);

    // Create the 32-bit blocks, and clear out the last block.
    vec = new uint32[nblocks];
//    vec[nblocks - 1] = 0;               // Clear the possible pad bytes.

    // Copy the given bytes into the 32-bit blocks, except the last block.
    register const char* q = buf;
    register int i;
    for (i = 0; i < nblocks - 1; ++i) {
        vec[i] = u32decode(q);
        q += 4;
        }

    // Take care of the last 32-bit block in case of a multiple of 4 bytes.
    i = int(mod4(buflen));
    if (i == 0) {
        vec[nblocks - 1] = u32decode(q);
        return;
        }

    // Take care of a partial last 32-bit block.
    // Pack the remaining bytes into the top of the last 32-bit block.
    register uint32 x = 0;
    switch (i) {
    case 1:
        x = uint8(*q) << 24;
        break;
    case 2:
        x = uint8(*q++) << 24;
        x |= uint8(*q) << 16;
        break;
    case 3:
        x = uint8(*q++) << 24;
        x |= uint8(*q++) << 16;
        x |= uint8(*q) << 8;
        break;
        } // End of switch (i).
    vec[nblocks - 1] = x;
    } // End of function boole_vec_l::boole_vec_l.

/*------------------------------------------------------------------------------
If the i-th bit is not already within range, make it in range.
If the bit is already physically there, just adjust "length".
If the bit is not there, then get more memory to represent it.
------------------------------------------------------------------------------*/
//--------------------------//
// boole_vec_l::instantiate //
//--------------------------//
void boole_vec_l::instantiate(long i) {
    // If the i-th bit is already instantiated, do nothing.
    if (i < length)
        return;

    // If the 32-bit block is already there, just adjust "length".
    if (i < nblocks * 32) {
        length = i + 1;
        return;
        }

    // Must resize if the array is too small.
    resize(i + 1);
    } // End of function boole_vec_l::instantiate.

//--------------------------//
//   boole_vec_l::free_mem  //
//--------------------------//
void boole_vec_l::free_mem() {
    length = 0;
    nblocks = 0;
    delete[] vec;
    vec = 0;
    } // End of function boole_vec_l::free_mem.

/*------------------------------------------------------------------------------
Set all bit values to 0, without changing the size of the boolean vector.
------------------------------------------------------------------------------*/
//----------------------//
//  boole_vec_l::clear  //
//----------------------//
void boole_vec_l::clear() {
    if (!vec)
        return;

    register uint32* p = vec;
    for (register long i = 0; i < nblocks; ++i)
        *p++ = 0;
    } // End of function boole_vec_l::clear.

/*------------------------------------------------------------------------------
If the resizing does not increase the size of the array, then the old contents
are copied to the new array. If the size is decreased, then those contents
are saved which can be saved. If it is possible to reduce memory usage, then the
old memory is released and new memory is allocated.
------------------------------------------------------------------------------*/
//----------------------//
//  boole_vec_l::resize //
//----------------------//
void boole_vec_l::resize(long s) {
    if (s <= 0) {
        if (vec) {
            length = 0;
            nblocks = 0;
            delete[] vec;
            vec = 0;
            }
        return;
        }
    if (s == length)
        return;
    long new_nblocks = ceil_div32(s);
    if (new_nblocks == nblocks) {
        // Just need to clear out the unused bits:
        length = s;
        if (mod32(length))
            vec[nblocks - 1] &= boole_left_mask_32[mod32(length)];
        return;
        }
    uint32* new_vec = new uint32[new_nblocks];
    register uint32* p = vec;
    register uint32* q = new_vec;
    long min_nblocks = (new_nblocks < nblocks) ? new_nblocks : nblocks;
    for (register long i = min_nblocks; --i >= 0; )
        *q++ = *p++;
    for (FOR_DECL(register long) i = new_nblocks - min_nblocks; --i >= 0; )
        *q++ = 0;
    delete[] vec;
    vec = new_vec;
    nblocks = new_nblocks;
    length = s;
    if (mod32(length))
        vec[nblocks - 1] &= boole_left_mask_32[mod32(length)];
    } // End of function boole_vec_l::resize.

/*------------------------------------------------------------------------------
Set bits from a given char-array.
The bits [offset, offset + n) in *this are set to the bits [0, n) in the given
char-array.
------------------------------------------------------------------------------*/
//--------------------------//
//   boole_vec_l::set_from  //
//--------------------------//
void boole_vec_l::set_from(const char* buf, long offset, long n) {
    if (!buf || n <= 0)
        return;

    // Find out how many bits to copy and where to start copying from:
    long nbits = n;     // Number of bits to take from buf.
    long start = 0;     // Starting bit in "buf".
    if (offset < 0) {
        n += offset;
        if (n <= 0)
            return;
        start = -offset;
        offset = 0;
        }
    long finish = offset + n;
    if (finish < 0)
        finish = max_int32;
    if (length < finish)
        resize(finish);

    // Extremely slow implementation because I have to go to ict'97 soon:
    // Write this efficiently later!!!!
    // ....
    for (long i = start; i < n; ++i) {
        uint32 bit = buf[div8(i)] & boole_left_bit_8[mod8(i)];
        set_in_range(i, bool_enum(bit != 0));
        }
    } // End of function boole_vec_l::set_from.

/*------------------------------------------------------------------------------
Set bits [offset, offset + n) of *this to bits [0, n) of "x", where n is the
length of the "x" array.
------------------------------------------------------------------------------*/
//--------------------------//
//   boole_vec_l::set_from  //
//--------------------------//
void boole_vec_l::set_from(const boole_vec_l& x, long offset) {
    long n = x.length;
    if (n <= 0)
        return;

    // Rushed method (write this properly when I have the leisure...):
    for (long i = 0; i < n; ++i)
        set(offset + i, bool_enum(x.get(i) != 0));
    } // End of function boole_vec_l::set_from.

/*------------------------------------------------------------------------------
This function replaces this boole_vec_l with the n bits of "x" which start at
offset "offset". Any bits that are not defined in "x" are assumed to be zero.
If "offset" is negative, the interval [offset, offset + n) is used, just as for
positive "offset". But what if offset + n overflows 31 bits?
------------------------------------------------------------------------------*/
//--------------------------//
//  boole_vec_l::copy_from  //
//--------------------------//
void boole_vec_l::copy_from(const boole_vec_l& x, long offset, long n) {
    // For an empty segment, just clear this boole_vec_l.
    if (n <= 0) {
        free_mem();
        return;
        }

    // Set this object to n zero-bits:
    resize(n);
    clear();

    // If the chosen segment (offset, n) does not intersect x, then return:
    if (!x.vec || offset >= x.length || offset + n <= 0)
        return;

    if (offset >= 0) {
        // Set "n" to number of bits to copy:
        if (n > x.length - offset)
            n = x.length - offset;

        long nright = mod32(offset);
        long nleft = 32 - nright;
        uint32* px = &x.vec[div32(offset)]; // Word to start copying from.
        uint32* py = vec;                   // Word to start copying to.
        while (n >= 32) {
            *py = *px++ << nright;
            if (nright > 0)
                *py |= (*px >> nleft) & boole_right_mask_32[nright];
            py += 1;
            n -= 32;
            }
        if (n <= 0)
            return;
        *py = *px++ << nright;
        if (n < nleft) {
            *py &= boole_left_mask_32[n];
            return;
            }
        n -= nleft;
        if (n > 0)  // (This would imply that nright > 0, since n <= nright.)
            *py |= (*px >> nleft) & (boole_right_mask_32[n] << (nright - n));
        return;
        }

    // Finally, treat the case of negative "offset":
    // Set "n" to number of bits to copy:
    n += offset;
    if (n > x.length)
        n = x.length;

    long nleft = mod32(-offset);
    long nright = 32 - nleft;
    uint32* px = x.vec;                 // Word to start copying from.
    uint32* py = &vec[div32(-offset)];  // Word to start copying to.
    while (n >= 32) {
        *py++ |= (*px >> nleft) & boole_right_mask_32[nright];
        if (nleft > 0)
            *py = *px << nright;
        px += 1;
        n -= 32;
        }
    if (n <= 0)
        return;
    *py++ |= ((*px & boole_left_mask_32[n]) >> nleft)
           & boole_right_mask_32[nright];
    n -= nright;
    if (n <= 0)
        return;
    *py = (*px << nright) & boole_left_mask_32[n];
    } // End of function boole_vec_l::copy_from.

/*------------------------------------------------------------------------------
This copies the whole boole_vec_l to a given buffer area.
------------------------------------------------------------------------------*/
//----------------------//
// boole_vec_l::copy_to //
//----------------------//
void boole_vec_l::copy_to(char* buf, long bufsize) {
    if (!buf || bufsize <= 0 || !vec)
        return;

    long n_bytes = size_bytes();
    if (bufsize > n_bytes)
        bufsize = n_bytes;
    long n_bits = bufsize * 8;
    if (n_bits > length)
        n_bits = length;
    register uint32* px = vec;
    register uint32 x;
    while (n_bits >= 32) {
        x = *px++;
        *buf++ = char(x >> 24);
        *buf++ = char(x >> 16);
        *buf++ = char(x >> 8);
        *buf++ = char(x);
        n_bits -= 32;
        }
    if (n_bits <= 0)
        return;
    x = *px;
    if (n_bits < 8) {
        *buf++ = char((x >> 24) & boole_left_mask_8[n_bits]);
        return;
        }
    *buf++ = char(x >> 24);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    if (n_bits < 8) {
        *buf++ = char((x >> 16) & boole_left_mask_8[n_bits]);
        return;
        }
    *buf++ = char(x >> 16);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    if (n_bits < 8) {
        *buf++ = char((x >> 8) & boole_left_mask_8[n_bits]);
        return;
        }
    *buf++ = char(x >> 8);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    *buf++ = char(x & boole_left_mask_8[n_bits]);
    } // End of function boole_vec_l::copy_to.

//----------------------//
//  boole_vec_l::print  //
//----------------------//
void boole_vec_l::print(ostream& os) {
    os << "nblocks = " << nblocks
       << ", length = " << length;
    if (length <= 0) {
        os << NL;
        return;
        }
    os << ", bit pattern:\n";
    for (long i = 0; i < length; ++i)
        os << (get_bit(i) ? "1" : "0");
    os << NL;
    } // End of function boole_vec_l::print.

//--------------------------//
// boole_vec_r::boole_vec_r //
//--------------------------//
boole_vec_r::boole_vec_r(long s) {
    if (s > 0) {
        length = s;
        nblocks = ceil_div32(s);
        vec = new uint32[nblocks];
        clear();
        }
    else {
        length = 0;
        nblocks = 0;
        vec = 0;
        }
    } // End of function boole_vec_r::boole_vec_r.

//--------------------------//
//   boole_vec_r::free_mem  //
//--------------------------//
void boole_vec_r::free_mem() {
    length = 0;
    nblocks = 0;
    delete[] vec;
    vec = 0;
    } // End of function boole_vec_r::free_mem.

//----------------------//
//  boole_vec_r::clear  //
//----------------------//
void boole_vec_r::clear() {
    register uint32* p = vec;
    for (register long i = 0; i < nblocks; ++i)
        *p++ = 0;
    } // End of function boole_vec_r::clear.

/*------------------------------------------------------------------------------
If the resizing does not increase the size of the array, then the old contents
are copied to the new array. If the size is decreased, then those contents
are saved which can be saved. If it is possible to reduce memory usage, then the
old memory is released and new memory is allocated.
------------------------------------------------------------------------------*/
//----------------------//
//  boole_vec_r::resize //
//----------------------//
void boole_vec_r::resize(long s) {
    if (s <= 0) {
        if (vec) {
            length = 0;
            nblocks = 0;
            delete[] vec;
            vec = 0;
            }
        return;
        }
    if (s == length)
        return;
    long new_nblocks = ceil_div32(s);
    if (new_nblocks == nblocks) {
        // Just need to clear out the unused bits:
        length = s;
        if (mod32(length))
            vec[nblocks - 1] &= boole_right_mask_32[mod32(length)];
        return;
        }
    uint32* new_vec = new uint32[new_nblocks];
    register uint32* p = vec;
    register uint32* q = new_vec;
    long min_nblocks = (new_nblocks < nblocks) ? new_nblocks : nblocks;
    for (register long i = min_nblocks; --i >= 0; )
        *q++ = *p++;
    for (FOR_DECL(register long) i = new_nblocks - min_nblocks; --i >= 0; )
        *q++ = 0;
    delete[] vec;
    vec = new_vec;
    nblocks = new_nblocks;
    length = s;
    if (mod32(length))
        vec[nblocks - 1] &= boole_right_mask_32[mod32(length)];
    } // End of function boole_vec_r::resize.

/*------------------------------------------------------------------------------
This function replaces this boole_vec_r with the n bits of "x" which start at
offset "offset". Any bits that are not defined in "x" are assumed to be zero.
If "offset" is negative, the interval [offset, offset + n) is used, just as for
positive "offset". But what if offset + n overflows 31 bits?
------------------------------------------------------------------------------*/
//--------------------------//
//  boole_vec_r::copy_from  //
//--------------------------//
/*------------------------------------------------------------------------------
void boole_vec_r::copy_from(const boole_vec_r& x, long offset, long n) {
    // For an empty segment, just clear this boole_vec_r.
    if (n <= 0) {
        free_mem();
        return;
        }

    // Set this object to n zero-bits:
    resize(n);
    clear();

    // If the chosen segment (offset, n) does not intersect x, then return:
    if (!x.vec || offset >= x.length || offset + n <= 0)
        return;

    if (offset >= 0) {
        // Set "n" to number of bits to copy:
        if (n > x.length - offset)
            n = x.length - offset;

        long nright = mod32(offset);
        long nleft = 32 - nright;
        uint32* px = &x.vec[div32(offset)]; // Word to start copying from.
        uint32* py = vec;                   // Word to start copying to.
        while (n >= 32) {
            *py = *px++ << nright;
            if (nright > 0)
                *py |= (*px >> nleft) & boole_right_mask_32[nright];
            py += 1;
            n -= 32;
            }
        if (n <= 0)
            return;
        *py = *px++ << nright;
        if (n < nleft) {
            *py &= boole_left_mask_32[n];
            return;
            }
        n -= nleft;
        if (n > 0)  // (This would imply that nright > 0, since n <= nright.)
            *py |= (*px >> nleft) & (boole_right_mask_32[n] << (nright - n));
        return;
        }

    // Finally, treat the case of negative "offset":
    // Set "n" to number of bits to copy:
    n += offset;
    if (n > x.length)
        n = x.length;

    long nleft = mod32(-offset);
    long nright = 32 - nleft;
    uint32* px = x.vec;                 // Word to start copying from.
    uint32* py = &vec[div32(-offset)];  // Word to start copying to.
    while (n >= 32) {
        *py++ |= (*px >> nleft) & boole_right_mask_32[nright];
        if (nleft > 0)
            *py = *px << nright;
        px += 1;
        n -= 32;
        }
    if (n <= 0)
        return;
    *py++ |= ((*px & boole_left_mask_32[n]) >> nleft)
           & boole_right_mask_32[nright];
    n -= nright;
    if (n <= 0)
        return;
    *py = (*px << nright) & boole_left_mask_32[n];
    } // End of function boole_vec_r::copy_from.
------------------------------------------------------------------------------*/

//----------------------//
// boole_vec_r::copy_to //
//----------------------//
/*------------------------------------------------------------------------------
void boole_vec_r::copy_to(char* buf, long bufsize) {
    if (!buf || bufsize <= 0 || !vec)
        return;

    long n_bytes = size_bytes();
    if (bufsize < n_bytes)
        bufsize = n_bytes;
    register uint32* px = vec;
    register uint32 x;
    while (n_bytes >= 4) {
        x = *px++;
        *buf++ = char(x >> 24);
        *buf++ = char(x >> 16);
        *buf++ = char(x >> 8);
        *buf++ = char(x);
        n_bytes -= 4;
        }
    if (n_bytes <= 0)
        return;
    x = *px;
    long n_bits = mod32(length);
    if (n_bits < 8) {
        *buf++ = char((x >> 24) & boole_left_mask_32[n_bits]);
        return;
        }
    *buf++ = char(x >> 24);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    if (n_bits < 8) {
        *buf++ = char((x >> 16) & boole_left_mask_32[n_bits]);
        return;
        }
    *buf++ = char(x >> 16);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    if (n_bits < 8) {
        *buf++ = char((x >> 8) & boole_left_mask_32[n_bits]);
        return;
        }
    *buf++ = char(x >> 8);
    n_bits -= 8;
    if (n_bits <= 0)
        return;
    *buf++ = char(x & boole_left_mask_32[n_bits]);
    } // End of function boole_vec_r::copy_to.
------------------------------------------------------------------------------*/

uint8 boole_left_bit_8[8] = {
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
    };
uint8 boole_right_bit_8[8] = {
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80
    };
uint8 boole_compl_left_bit_8[8] = {
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe
    };
uint8 boole_compl_right_bit_8[8] = {
    0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f
    };
uint8 boole_left_mask_8[9] = {
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
    };
uint8 boole_right_mask_8[9] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
    };

uint32 boole_left_bit_32[32] = {
    0x80000000, 0x40000000, 0x20000000, 0x10000000,
    0x08000000, 0x04000000, 0x02000000, 0x01000000,
    0x00800000, 0x00400000, 0x00200000, 0x00100000,
    0x00080000, 0x00040000, 0x00020000, 0x00010000,
    0x00008000, 0x00004000, 0x00002000, 0x00001000,
    0x00000800, 0x00000400, 0x00000200, 0x00000100,
    0x00000080, 0x00000040, 0x00000020, 0x00000010,
    0x00000008, 0x00000004, 0x00000002, 0x00000001
    };
uint32 boole_right_bit_32[32] = {
    0x00000001, 0x00000002, 0x00000004, 0x00000008,
    0x00000010, 0x00000020, 0x00000040, 0x00000080,
    0x00000100, 0x00000200, 0x00000400, 0x00000800,
    0x00001000, 0x00002000, 0x00004000, 0x00008000,
    0x00010000, 0x00020000, 0x00040000, 0x00080000,
    0x00100000, 0x00200000, 0x00400000, 0x00800000,
    0x01000000, 0x02000000, 0x04000000, 0x08000000,
    0x10000000, 0x20000000, 0x40000000, 0x80000000
    };
uint32 boole_compl_left_bit_32[32] = {
    0x7fffffff, 0xbfffffff, 0xdfffffff, 0xefffffff,
    0xf7ffffff, 0xfbffffff, 0xfdffffff, 0xfeffffff,
    0xff7fffff, 0xffbfffff, 0xffdfffff, 0xffefffff,
    0xfff7ffff, 0xfffbffff, 0xfffdffff, 0xfffeffff,
    0xffff7fff, 0xffffbfff, 0xffffdfff, 0xffffefff,
    0xfffff7ff, 0xfffffbff, 0xfffffdff, 0xfffffeff,
    0xffffff7f, 0xffffffbf, 0xffffffdf, 0xffffffef,
    0xfffffff7, 0xfffffffb, 0xfffffffd, 0xfffffffe
    };
uint32 boole_compl_right_bit_32[32] = {
    0xfffffffe, 0xfffffffd, 0xfffffffb, 0xfffffff7,
    0xffffffef, 0xffffffdf, 0xffffffbf, 0xffffff7f,
    0xfffffeff, 0xfffffdff, 0xfffffbff, 0xfffff7ff,
    0xffffefff, 0xffffdfff, 0xffffbfff, 0xffff7fff,
    0xfffeffff, 0xfffdffff, 0xfffbffff, 0xfff7ffff,
    0xffefffff, 0xffdfffff, 0xffbfffff, 0xff7fffff,
    0xfeffffff, 0xfdffffff, 0xfbffffff, 0xf7ffffff,
    0xefffffff, 0xdfffffff, 0xbfffffff, 0x7fffffff
    };
uint32 boole_left_mask_32[33] = {
    0x00000000,
    0x80000000, 0xc0000000, 0xe0000000, 0xf0000000,
    0xf8000000, 0xfc000000, 0xfe000000, 0xff000000,
    0xff800000, 0xffc00000, 0xffe00000, 0xfff00000,
    0xfff80000, 0xfffc0000, 0xfffe0000, 0xffff0000,
    0xffff8000, 0xffffc000, 0xffffe000, 0xfffff000,
    0xfffff800, 0xfffffc00, 0xfffffe00, 0xffffff00,
    0xffffff80, 0xffffffc0, 0xffffffe0, 0xfffffff0,
    0xfffffff8, 0xfffffffc, 0xfffffffe, 0xffffffff
    };
uint32 boole_right_mask_32[33] = {
    0x00000000,
    0x00000001, 0x00000003, 0x00000007, 0x0000000f,
    0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
    0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
    0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
    0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
    0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
    0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
    0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
    };
