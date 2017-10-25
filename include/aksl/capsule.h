// src/aksl/capsule.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/capsule.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_CAPSULE_H
#define AKSL_CAPSULE_H
/*------------------------------------------------------------------------------
This module contains classes for encapsulation and reassembly of data
into and out of simple data link frames.
Just two sample encapsulations are provided: frame8 and frame32.
Each case makes particular assumptions and trade-offs which are not all that
general.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes defined in this file:

frame8::
frame8list::
f8stream::
f8streamlist::
frame32::
frame32list::
f32stream::
f32streamlist::
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_COD_H
#include "aksl/cod.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_SYS_TYPES_H
#define AKSL_X_SYS_TYPES_H
#include <sys/types.h>
#endif

// frame8 header parameters:
const uint8 deft_f8_sync_byte = 0xea;
const int frame8_sync_size = 1;     // sync byte.
const int frame8_op_size  = 1;      // opcode field.
const int frame8_LI_size  = 2;      // arg (4 bits) + LI (12 bits) fields.
const int frame8_hcs_size = 1;      // CRC-8 field.
const int frame8_hdr_size = frame8_sync_size
                          + frame8_op_size
                          + frame8_LI_size
                          + frame8_hcs_size;

// frame8 payload parameters:
const int frame8_max_LI = 0x0fff;   // 12-bit LI => 4095 bytes max payload.

// frame8 trailer parameters:
const int frame8_dcs_size = 1;      // CRC-8 field.

// Buffer size required for frame8 frames:
const int frame8_buf_size = frame8_hdr_size
                          + frame8_max_LI
                          + frame8_dcs_size;

// frame32 header parameters:
const uint8 deft_f32_sync_byte = 0xcd;
const int frame32_sync_size = 1;    // sync byte.
const int frame32_op_size  = 1;     // opcode field.
const int frame32_LI_size  = 2;     // arg (2 bits) + LI field (14 bits).
const int frame32_hcs_size = 2;     // CRC-16 field.
const int frame32_hdr_size = frame32_sync_size
                           + frame32_op_size
                           + frame32_LI_size
                           + frame32_hcs_size;

// frame32 payload parameters:
const int frame32_max_LI   = 0x3fff;    // 14-bit LI (16383 bytes max payload).

// frame32 trailer parameters:
const int frame32_dcs_size = 4;         // CRC-32 field.

// Buffer size required for frame32 frames: (1 extra for sync byte)
const int frame32_buf_size = frame32_hdr_size   // 6
                           + frame32_max_LI     // 16383
                           + frame32_dcs_size;  // 4

/*------------------------------------------------------------------------------
This class represents a data link frame with compact 8-bit CRCs for both
the header and payload.
------------------------------------------------------------------------------*/
//----------------------//
//        frame8::      //
//----------------------//
struct frame8: public slink {
    // The decoded version:
    uint8           opcode;         // 8-bit opcode. Type opcode_t.
    uint8           arg;            // 4 bit-argument.
    uint16          LI;             // 12-bit number of bytes in "payload".
    char*           payload;        // Heap-allocated.

    // The encoded version, complete with HCS and DCS, but no "sync":
    char*           coded;          // Heap-allocated.
    uint16          n_coded;        // Number of bytes in "coded".

    frame8* next() const { return (frame8*)slink::next(); }
private:
friend struct f8stream;
    int encode(uint8 sync_byte = 0);
public:
    void print(ostream& = cout) const;

//    frame8& operator=(const frame8& x) {}
//    frame8(const frame8& x) {};
    frame8() {
        opcode = 0;
        arg = 0;
        LI = 0;
        payload = 0;
        coded = 0;
        n_coded = 0;
        }
    ~frame8() { delete[] payload; delete[] coded; }
    }; // End of struct frame8.

//----------------------//
//      frame8list::    //
//----------------------//
struct frame8list: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    frame8* first() const { return (frame8*)s2list::first(); }
    frame8* last() const { return (frame8*)s2list::last(); }
    void append(frame8* p) { s2list::append(p); }
    void prepend(frame8* p) { s2list::prepend(p); }
    frame8* popfirst() { return (frame8*)s2list::popfirst(); }
    frame8* poplast() { return (frame8*)s2list::poplast(); }
    frame8* remove(frame8* p)
        { return (frame8*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(frame8* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(frame8list& l) { s2list::swallow(&l); }
    void gulp(frame8list& l) { s2list::gulp(&l); }
    void clear() { for (frame8* p = first(); p; )
        { frame8* q = p->next(); delete p; p = q; } clearptrs(); }

//    frame8list& operator=(const frame8list& x) {}
//    frame8list(const frame8list& x) {};
    frame8list() {}
    ~frame8list() { clear(); }
    }; // End of struct frame8list.

/*------------------------------------------------------------------------------
This class decodes an incoming byte stream into messages of class "frame8".
Encoding is easily done on a per-frame basis. But decoding is more appropriately
done by stream object whose output is a stream of frames.
------------------------------------------------------------------------------*/
//----------------------//
//       f8stream::     //
//----------------------//
struct f8stream: public slink {
    char buf[frame8_buf_size];  // Byte stream reassembly buffer.
    int nbuf;                   // Valid bytes in buf.
    uint16 LI;                  // LI found in incmoing bytes.
    frame8list fifo;            // Holding list for reassembled frames.
    uint8 sync_byte;            // Sync byte, set by user.

    f8stream* next() const { return (f8stream*)slink::next(); }

    int encode(frame8& fr) { return fr.encode(sync_byte); }

    // Call with zero args to get buffered frames:
    frame8* decode(const char* bytes = 0, int n_bytes = 0);

//    f8stream& operator=(const f8stream& x) {}
//    f8stream(const f8stream& x) {};
    f8stream() { nbuf = 0; LI = 0; sync_byte = deft_f8_sync_byte; }
    ~f8stream() {}
    }; // End of struct f8stream.

//----------------------//
//     f8streamlist::   //
//----------------------//
struct f8streamlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    f8stream* first() const { return (f8stream*)s2list::first(); }
    f8stream* last() const { return (f8stream*)s2list::last(); }
    void append(f8stream* p) { s2list::append(p); }
    void prepend(f8stream* p) { s2list::prepend(p); }
    f8stream* popfirst() { return (f8stream*)s2list::popfirst(); }
    f8stream* poplast() { return (f8stream*)s2list::poplast(); }
    f8stream* remove(f8stream* p)
        { return (f8stream*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(f8stream* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(f8streamlist& l) { s2list::swallow(&l); }
    void gulp(f8streamlist& l) { s2list::gulp(&l); }
    void clear() { for (f8stream* p = first(); p; )
        { f8stream* q = p->next(); delete p; p = q; } clearptrs(); }

//    f8streamlist& operator=(const f8streamlist& x) {}
//    f8streamlist(const f8streamlist& x) {};
    f8streamlist() {}
    ~f8streamlist() { clear(); }
    }; // End of struct f8streamlist.

/*------------------------------------------------------------------------------
This class represents a data link frame with 16-bit CRCs headers and 32-bit
CRCs for payloads. Max payload = 16383 bytes.
------------------------------------------------------------------------------*/
//----------------------//
//       frame32::      //
//----------------------//
struct frame32: public slink {
    // The decoded version:
    uint8           opcode;         // 8-bit opcode. Type opcode_t.
    uint8           arg;            // 2-bit argument.
    uint16          LI;             // 14-bit number of bytes in "payload".
    char*           payload;        // Heap-allocated.

    // The encoded version, complete with HCS and DCS, but no "sync":
    char*           coded;          // Heap-allocated.
    uint16          n_coded;        // Number of bytes in "coded".

    frame32* next() const { return (frame32*)slink::next(); }
private:
friend struct f32stream;
    int encode(uint8 sync_byte = 0);
public:
    void print(ostream& = cout) const;

//    frame32& operator=(const frame32& x) {}
//    frame32(const frame32& x) {};
    frame32() {
        opcode = 0;
        arg = 0;
        LI = 0;
        payload = 0;
        coded = 0;
        n_coded = 0;
        }
    ~frame32() { delete[] payload; delete[] coded; }
    }; // End of struct frame32.

//----------------------//
//      frame32list::   //
//----------------------//
struct frame32list: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    frame32* first() const { return (frame32*)s2list::first(); }
    frame32* last() const { return (frame32*)s2list::last(); }
    void append(frame32* p) { s2list::append(p); }
    void prepend(frame32* p) { s2list::prepend(p); }
    frame32* popfirst() { return (frame32*)s2list::popfirst(); }
    frame32* poplast() { return (frame32*)s2list::poplast(); }
    frame32* remove(frame32* p)
        { return (frame32*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(frame32* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(frame32list& l) { s2list::swallow(&l); }
    void gulp(frame32list& l) { s2list::gulp(&l); }
    void clear() { for (frame32* p = first(); p; )
        { frame32* q = p->next(); delete p; p = q; } clearptrs(); }

//    frame32list& operator=(const frame32list& x) {}
//    frame32list(const frame32list& x) {};
    frame32list() {}
    ~frame32list() { clear(); }
    }; // End of struct frame32list.

/*------------------------------------------------------------------------------
This class decodes an incoming byte stream into messages of class "frame32".
Encoding is easily done on a per-frame basis. But decoding is more appropriately
done by stream object whose output is a stream of frames.
------------------------------------------------------------------------------*/
//----------------------//
//       f32stream::    //
//----------------------//
struct f32stream: public slink {
    char buf[frame32_buf_size]; // Byte stream reassembly buffer.
    int nbuf;                   // Valid bytes in buf.
    uint16 LI;                  // LI found in incmoing bytes.
    frame32list fifo;           // Holding list for reassembled frames.
    uint8 sync_byte;            // Sync byte, set by user and expected in input.

    f32stream* next() const { return (f32stream*)slink::next(); }

    int encode(frame32& fr) { return fr.encode(sync_byte); }

    // Call with zero args to get buffered frames:
    frame32* decode(const char* bytes = 0, int n_bytes = 0);

//    f32stream& operator=(const f32stream& x) {}
//    f32stream(const f32stream& x) {};
    f32stream() { nbuf = 0; LI = 0; sync_byte = deft_f32_sync_byte; }
    ~f32stream() {}
    }; // End of struct f32stream.

//----------------------//
//     f32streamlist::  //
//----------------------//
struct f32streamlist: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    f32stream* first() const { return (f32stream*)s2list::first(); }
    f32stream* last() const { return (f32stream*)s2list::last(); }
    void append(f32stream* p) { s2list::append(p); }
    void prepend(f32stream* p) { s2list::prepend(p); }
    f32stream* popfirst() { return (f32stream*)s2list::popfirst(); }
    f32stream* poplast() { return (f32stream*)s2list::poplast(); }
    f32stream* remove(f32stream* p)
        { return (f32stream*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(f32stream* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(f32streamlist& l) { s2list::swallow(&l); }
    void gulp(f32streamlist& l) { s2list::gulp(&l); }
    void clear() { for (f32stream* p = first(); p; )
        { f32stream* q = p->next(); delete p; p = q; } clearptrs(); }

//    f32streamlist& operator=(const f32streamlist& x) {}
//    f32streamlist(const f32streamlist& x) {};
    f32streamlist() {}
    ~f32streamlist() { clear(); }
    }; // End of struct f32streamlist.

#endif /* AKSL_CAPSULE_H */
