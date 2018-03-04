// src/aksl/cod.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
// General coding things.
/*------------------------------------------------------------------------------
Functions in this file:

crc8init
crc8calc
crc8calc
crc8calc_rl
crc16init
crc16calc
crc16calc
crc16calc_rl
crc32init
crc32calc
crc32calc
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/cod.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files:
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

// Lookup table for CRC-8 (poly = x^8 + x^2 + x^1 + x^0 = 0x107):
// (crc8table[x] = remainder of x * 2^8 divided by 0x107.)
static uint8 crc8table[256]={
    0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31,
    0x24, 0x23, 0x2a, 0x2d, 0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65,
    0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d, 0xe0, 0xe7, 0xee, 0xe9,
    0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
    0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1,
    0xb4, 0xb3, 0xba, 0xbd, 0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2,
    0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea, 0xb7, 0xb0, 0xb9, 0xbe,
    0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
    0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16,
    0x03, 0x04, 0x0d, 0x0a, 0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42,
    0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a, 0x89, 0x8e, 0x87, 0x80,
    0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
    0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8,
    0xdd, 0xda, 0xd3, 0xd4, 0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c,
    0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44, 0x19, 0x1e, 0x17, 0x10,
    0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
    0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f,
    0x6a, 0x6d, 0x64, 0x63, 0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b,
    0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13, 0xae, 0xa9, 0xa0, 0xa7,
    0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
    0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef,
    0xfa, 0xfd, 0xf4, 0xf3
    }; // End of array crc8table.

/*------------------------------------------------------------------------------
Lookup table for CRC-16 (poly = 0x1 1021):
(crc16table[x] = remainder of x * 2^16 divided by  0x11021, with
the log division register initialised to 0xffff.)
------------------------------------------------------------------------------*/
static uint16 crc16table[256]={
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
    0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
    0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
    0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
    0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
    0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
    0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
    0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
    0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
    0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
    0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
    0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
    0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
    0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
    0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
    0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
    0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
    0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
    0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
    0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
    0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
    0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
    }; // End of array crc16table.

/*------------------------------------------------------------------------------
Lookup table for CRC-32 (poly = 0x1 04c1 1db7):
(crc32table[x] = remainder of x * 2^32 divided by  0x104c11db7, with
the log division register initialised to 0xffffffff.)
------------------------------------------------------------------------------*/
static uint32 crc32table[256]={
    0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9, 0x130476dc, 0x17c56b6b,
    0x1a864db2, 0x1e475005, 0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
    0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd, 0x4c11db70, 0x48d0c6c7,
    0x4593e01e, 0x4152fda9, 0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
    0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011, 0x791d4014, 0x7ddc5da3,
    0x709f7b7a, 0x745e66cd, 0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
    0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5, 0xbe2b5b58, 0xbaea46ef,
    0xb7a96036, 0xb3687d81, 0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
    0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49, 0xc7361b4c, 0xc3f706fb,
    0xceb42022, 0xca753d95, 0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
    0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d, 0x34867077, 0x30476dc0,
    0x3d044b19, 0x39c556ae, 0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
    0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16, 0x018aeb13, 0x054bf6a4,
    0x0808d07d, 0x0cc9cdca, 0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
    0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02, 0x5e9f46bf, 0x5a5e5b08,
    0x571d7dd1, 0x53dc6066, 0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
    0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e, 0xbfa1b04b, 0xbb60adfc,
    0xb6238b25, 0xb2e29692, 0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
    0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a, 0xe0b41de7, 0xe4750050,
    0xe9362689, 0xedf73b3e, 0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
    0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686, 0xd5b88683, 0xd1799b34,
    0xdc3abded, 0xd8fba05a, 0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
    0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb, 0x4f040d56, 0x4bc510e1,
    0x46863638, 0x42472b8f, 0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
    0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47, 0x36194d42, 0x32d850f5,
    0x3f9b762c, 0x3b5a6b9b, 0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
    0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623, 0xf12f560e, 0xf5ee4bb9,
    0xf8ad6d60, 0xfc6c70d7, 0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
    0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f, 0xc423cd6a, 0xc0e2d0dd,
    0xcda1f604, 0xc960ebb3, 0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
    0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b, 0x9b3660c6, 0x9ff77d71,
    0x92b45ba8, 0x9675461f, 0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
    0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640, 0x4e8ee645, 0x4a4ffbf2,
    0x470cdd2b, 0x43cdc09c, 0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
    0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24, 0x119b4be9, 0x155a565e,
    0x18197087, 0x1cd86d30, 0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
    0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088, 0x2497d08d, 0x2056cd3a,
    0x2d15ebe3, 0x29d4f654, 0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
    0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c, 0xe3a1cbc1, 0xe760d676,
    0xea23f0af, 0xeee2ed18, 0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
    0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0, 0x9abc8bd5, 0x9e7d9662,
    0x933eb0bb, 0x97ffad0c, 0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
    0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
    }; // End of array crc32table.

/*------------------------------------------------------------------------------
crc8init() (re-)initialises the crc8table. This can be called to verify that
there are no typing errors in the table.
------------------------------------------------------------------------------*/
//----------------------//
//       crc8init       //
//----------------------//
int crc8init() {
    int i_error_first = -1;

    for (int i = 0; i < 256; ++i) {
        unsigned int r = i << 8;        // r is the remainder.

        // The following loop is just long division:
        for (int j = 7; j >= 0; --j)
            if (r & (1 << (j + 8)))
                r ^= crc8poly << j;

        // Check for error in the previous value:
        if ((crc8table[i] != (r & 0xff)) && i_error_first < 0)
            i_error_first = i;
        crc8table[i] = r & 0xff;
        }
    if (i_error_first >= 0) {
        fprintf(stderr, "Found error in crc8table[%d].\n", i_error_first);
        return -1;
        }
    return 0;
    } // End of function crc8init.

/*------------------------------------------------------------------------------
crc8calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" 0x107, and XORed with 0x55.
The given array has address "buf" and length "len".
This CRC is the same as used in (the 5th byte of) ATM cell headers.
------------------------------------------------------------------------------*/
//----------------------//
//       crc8calc       //
//----------------------//
char crc8calc(const char* buf, int len) {
    char r = 0;             // r is the remainder.
    if (buf && len > 0)
        while (--len >= 0)  // Do long division, 8 bits at a time.
            r = crc8table[(r ^ *buf++) & 0xff];
    return r ^ 0x55;
    } // End of function crc8calc.

/*------------------------------------------------------------------------------
crc8calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" 0x107, and XORed with 0x55.
The given array has address "buf" and is null-terminated.
This CRC is the same as used in (the 5th byte of) ATM cell headers.
------------------------------------------------------------------------------*/
//----------------------//
//       crc8calc       //
//----------------------//
char crc8calc(const char* buf) {
    char r = 0;             // r is the remainder.
    if (buf)
        while (*buf)  // Do long division, 8 bits at a time.
            r = crc8table[(r ^ *buf++) & 0xff];
    return r ^ 0x55;
    } // End of function crc8calc.

/*------------------------------------------------------------------------------
This does a right-to-left CRC-8 on the given 32-bit integer.
------------------------------------------------------------------------------*/
//----------------------//
//     crc8calc_rl      //
//----------------------//
uint8 crc8calc_rl(uint32 data) {
    char r = crc8table[data & 0xff];
    data >>= 8;
    r = crc8table[(r ^ data) & 0xff];
    data >>= 8;
    r = crc8table[(r ^ data) & 0xff];
    data >>= 8;
    r = crc8table[(r ^ data) & 0xff];
    return uint8(r ^ 0x55);
    } // End of function crc8calc_rl.

/*------------------------------------------------------------------------------
crc16init() (re-)initialises the crc16table. This can be called to verify that
there are no typing errors in the table.
------------------------------------------------------------------------------*/
//----------------------//
//       crc16init      //
//----------------------//
int crc16init(uint16 poly, int trace) {
    int i_error_first = -1;
    for (int i = 0; i < 256; ++i) {
        // Initialize the remainder:
        uint16 r = i << 8;

        // The following loop is just long division:
        for (int j = 7; j >= 0; --j)
            if (r & 0x8000) {
                r <<= 1;
                r ^= poly;
                }
            else
                r <<= 1;

        // Check for error in the previous value:
        if ((crc16table[i] != r) && i_error_first < 0)
            i_error_first = i;
        crc16table[i] = r;

        // Do a trace to create the initial table:
        if (trace >= 3) {
            cout << "crc16table[" << i << "] = 0x"
                 << hex4(crc16table[i]) << "\n";
            }
        }
    if (trace >= 1 && i_error_first >= 0) {
        cout << "Found error in crc16table[" << i_error_first << endl;
        return -1;
        }
    return 0;
    } // End of function crc16init.

/*------------------------------------------------------------------------------
crc16calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" crc16poly, modulo some fine points.
The given array has address "buf" and length "len".
This CRC is the same as used in PPP frames. But in PPP it may or may not be sent
in reverse bit order, depending on one's perspective.
------------------------------------------------------------------------------*/
//----------------------//
//       crc16calc      //
//----------------------//
uint16 crc16calc(const char* buf, int len) {
    register uint16 crc = crc16begin;
    if (buf)
        while (len > 0) { // Do long division, 8 bits at a time.
            crc = (crc << 8) ^ crc16table[((crc >> 8) ^ *buf++) & 0xff];
            len -= 1;
            }
    return ~crc & 0xffff;
    } // End of function crc16calc.

/*------------------------------------------------------------------------------
crc16calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" crc16poly, modulo some fine points.
The given array has address "buf" and is (8-bit) null-terminated.
This CRC is the same as used in PPP frames. But in PPP it may or may not be sent
in reverse bit order, depending on one's perspective.
------------------------------------------------------------------------------*/
//----------------------//
//       crc16calc      //
//----------------------//
uint16 crc16calc(const char* buf) {
    register uint16 crc = crc16begin;
    if (buf)
        while (*buf)    // Do long division, 8 bits at a time.
            crc = (crc << 8) ^ crc16table[((crc >> 8) ^ *buf++) & 0xff];
    return ~crc & 0xffff;
    } // End of function crc16calc.

/*------------------------------------------------------------------------------
This does a right-to-left CRC-16 on the given 32-bit integer.
------------------------------------------------------------------------------*/
//----------------------//
//     crc16calc_rl     //
//----------------------//
uint16 crc16calc_rl(uint32 data) {
    register uint16 crc = crc16begin;
    crc = (crc << 8) ^ crc16table[((crc >> 8) ^ (data & 0xff)) & 0xff];
    data >>= 8;
    crc = (crc << 8) ^ crc16table[((crc >> 8) ^ (data & 0xff)) & 0xff];
    data >>= 8;
    crc = (crc << 8) ^ crc16table[((crc >> 8) ^ (data & 0xff)) & 0xff];
    data >>= 8;
    crc = (crc << 8) ^ crc16table[((crc >> 8) ^ (data & 0xff)) & 0xff];

    return ~crc & 0xffff;
    } // End of function crc16calc_rl.

/*------------------------------------------------------------------------------
crc32init() (re-)initialises the crc32table. This can be called to verify that
there are no typing errors in the table.
------------------------------------------------------------------------------*/
//----------------------//
//       crc32init      //
//----------------------//
int crc32init(uint32 poly, int trace) {
    int i_error_first = -1;
    for (int i = 0; i < 256; ++i) {
        // Initialize the remainder:
        uint32 r = i << 24;

        // The following loop is just long division:
        for (int j = 7; j >= 0; --j)
            if (r & 0x80000000) {
                r <<= 1;
                r ^= poly;
                }
            else
                r <<= 1;

        // Check for error in the previous value:
        if ((crc32table[i] != r) && i_error_first < 0)
            i_error_first = i;
        crc32table[i] = r;

        // Do a trace to create the initial table:
        if (trace >= 3) {
            cout << "crc32table[" << i << "] = 0x"
                 << hex8(crc32table[i]) << "\n";
            }
        }
    if (trace >= 1 && i_error_first >= 0) {
        cout << "Found error in crc32table[" << i_error_first << endl;
        return -1;
        }
    return 0;
    } // End of function crc32init.

/*------------------------------------------------------------------------------
crc32calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" crc32poly, modulo some fine points.
The given array has address "buf" and length "len".
This CRC is the same as used in AAL5 packets.
------------------------------------------------------------------------------*/
//----------------------//
//       crc32calc      //
//----------------------//
uint32 crc32calc(const char* buf, int len) {
    register uint32 crc = crc32begin;
    if (buf)
        while (len > 0) { // Do long division, 8 bits at a time.
            crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *buf++) & 0xff];
            len -= 1;
            }
    return ~crc;
    } // End of function crc32calc.

/*------------------------------------------------------------------------------
crc32calc() returns the" remainder" of the given byte array "divided" by the
"polynomial" crc32poly, modulo some fine points.
The given array has address "buf" and is null-terminated.
This CRC is the same as used in AAL5 packets.
------------------------------------------------------------------------------*/
//----------------------//
//       crc32calc      //
//----------------------//
uint32 crc32calc(const char* buf) {
    register uint32 crc = crc32begin;
    if (buf)
        while (*buf)    // Do long division, 8 bits at a time.
            crc = (crc << 8) ^ crc32table[((crc >> 24) ^ *buf++) & 0xff];
    return ~crc;
    } // End of function crc32calc.
