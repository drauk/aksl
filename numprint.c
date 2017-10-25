// src/aksl/numprint.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

bool_string
byte2hex_test
intstring
uintstring
hexstring
hex2u8
octstring
f_string
e_string
g_string
hex_print
bin_print
ipstring
bin_dump_header
bin_dump
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/numprint.h"
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_STDARG_H
#define AKSL_X_STDARG_H
#include <stdarg.h>
#endif
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

#ifdef sun
#ifndef AKSL_X_SYSENT_H
#define AKSL_X_SYSENT_H
#include <sysent.h>
#endif
#endif

#ifndef WIN32
#ifndef AKSL_X_NETINET_IN_H
#define AKSL_X_NETINET_IN_H
#include <netinet/in.h>
#endif
#endif

#if defined(WIN32) && defined(__GNUC__)
#ifndef AKSL_X_ASM_BYTEORDER_H
#define AKSL_X_ASM_BYTEORDER_H
#include <asm/byteorder.h>
#endif
#endif

// Look-up table for printing hexadecimal strings:
char byte2hex[256][3] = {
    "00", "01", "02", "03", "04", "05", "06", "07",
    "08", "09", "0a", "0b", "0c", "0d", "0e", "0f",
    "10", "11", "12", "13", "14", "15", "16", "17",
    "18", "19", "1a", "1b", "1c", "1d", "1e", "1f",
    "20", "21", "22", "23", "24", "25", "26", "27",
    "28", "29", "2a", "2b", "2c", "2d", "2e", "2f",
    "30", "31", "32", "33", "34", "35", "36", "37",
    "38", "39", "3a", "3b", "3c", "3d", "3e", "3f",
    "40", "41", "42", "43", "44", "45", "46", "47",
    "48", "49", "4a", "4b", "4c", "4d", "4e", "4f",
    "50", "51", "52", "53", "54", "55", "56", "57",
    "58", "59", "5a", "5b", "5c", "5d", "5e", "5f",
    "60", "61", "62", "63", "64", "65", "66", "67",
    "68", "69", "6a", "6b", "6c", "6d", "6e", "6f",
    "70", "71", "72", "73", "74", "75", "76", "77",
    "78", "79", "7a", "7b", "7c", "7d", "7e", "7f",
    "80", "81", "82", "83", "84", "85", "86", "87",
    "88", "89", "8a", "8b", "8c", "8d", "8e", "8f",
    "90", "91", "92", "93", "94", "95", "96", "97",
    "98", "99", "9a", "9b", "9c", "9d", "9e", "9f",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "a8", "a9", "aa", "ab", "ac", "ad", "ae", "af",
    "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7",
    "b8", "b9", "ba", "bb", "bc", "bd", "be", "bf",
    "c0", "c1", "c2", "c3", "c4", "c5", "c6", "c7",
    "c8", "c9", "ca", "cb", "cc", "cd", "ce", "cf",
    "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7",
    "d8", "d9", "da", "db", "dc", "dd", "de", "df",
    "e0", "e1", "e2", "e3", "e4", "e5", "e6", "e7",
    "e8", "e9", "ea", "eb", "ec", "ed", "ee", "ef",
    "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
    "f8", "f9", "fa", "fb", "fc", "fd", "fe", "ff"
    };

// Look-up table for printing binary strings:
char byte2bin[256][9] = {
    "00000000", "00000001", "00000010", "00000011",
    "00000100", "00000101", "00000110", "00000111",
    "00001000", "00001001", "00001010", "00001011",
    "00001100", "00001101", "00001110", "00001111",
    "00010000", "00010001", "00010010", "00010011",
    "00010100", "00010101", "00010110", "00010111",
    "00011000", "00011001", "00011010", "00011011",
    "00011100", "00011101", "00011110", "00011111",
    "00100000", "00100001", "00100010", "00100011",
    "00100100", "00100101", "00100110", "00100111",
    "00101000", "00101001", "00101010", "00101011",
    "00101100", "00101101", "00101110", "00101111",
    "00110000", "00110001", "00110010", "00110011",
    "00110100", "00110101", "00110110", "00110111",
    "00111000", "00111001", "00111010", "00111011",
    "00111100", "00111101", "00111110", "00111111",
    "01000000", "01000001", "01000010", "01000011",
    "01000100", "01000101", "01000110", "01000111",
    "01001000", "01001001", "01001010", "01001011",
    "01001100", "01001101", "01001110", "01001111",
    "01010000", "01010001", "01010010", "01010011",
    "01010100", "01010101", "01010110", "01010111",
    "01011000", "01011001", "01011010", "01011011",
    "01011100", "01011101", "01011110", "01011111",
    "01100000", "01100001", "01100010", "01100011",
    "01100100", "01100101", "01100110", "01100111",
    "01101000", "01101001", "01101010", "01101011",
    "01101100", "01101101", "01101110", "01101111",
    "01110000", "01110001", "01110010", "01110011",
    "01110100", "01110101", "01110110", "01110111",
    "01111000", "01111001", "01111010", "01111011",
    "01111100", "01111101", "01111110", "01111111",
    "10000000", "10000001", "10000010", "10000011",
    "10000100", "10000101", "10000110", "10000111",
    "10001000", "10001001", "10001010", "10001011",
    "10001100", "10001101", "10001110", "10001111",
    "10010000", "10010001", "10010010", "10010011",
    "10010100", "10010101", "10010110", "10010111",
    "10011000", "10011001", "10011010", "10011011",
    "10011100", "10011101", "10011110", "10011111",
    "10100000", "10100001", "10100010", "10100011",
    "10100100", "10100101", "10100110", "10100111",
    "10101000", "10101001", "10101010", "10101011",
    "10101100", "10101101", "10101110", "10101111",
    "10110000", "10110001", "10110010", "10110011",
    "10110100", "10110101", "10110110", "10110111",
    "10111000", "10111001", "10111010", "10111011",
    "10111100", "10111101", "10111110", "10111111",
    "11000000", "11000001", "11000010", "11000011",
    "11000100", "11000101", "11000110", "11000111",
    "11001000", "11001001", "11001010", "11001011",
    "11001100", "11001101", "11001110", "11001111",
    "11010000", "11010001", "11010010", "11010011",
    "11010100", "11010101", "11010110", "11010111",
    "11011000", "11011001", "11011010", "11011011",
    "11011100", "11011101", "11011110", "11011111",
    "11100000", "11100001", "11100010", "11100011",
    "11100100", "11100101", "11100110", "11100111",
    "11101000", "11101001", "11101010", "11101011",
    "11101100", "11101101", "11101110", "11101111",
    "11110000", "11110001", "11110010", "11110011",
    "11110100", "11110101", "11110110", "11110111",
    "11111000", "11111001", "11111010", "11111011",
    "11111100", "11111101", "11111110", "11111111"
    };

//----------------------//
//      bool_string     //
//----------------------//
const char* bool_string(long i) {
    static char t[] = "true";
    static char f[] = "false";
    return (i == 0) ? f : t;
    } // End of function bool_string.

//----------------------//
//     byte2hex_test    //
//----------------------//
void byte2hex_test() {
    // Test the hex string table:
    cout << "Testing the hex string table byte2hex[256]:\n";
    for (int z = 0; z < 256; ++z) {
        cout << byte2hex[z] << " ";
        if (z % 16 == 15)
            cout << endl;
        else if (z % 4 == 3)
            cout << " ";
        }
    } // End of function byte2hex_test.

/*------------------------------------------------------------------------------
Note: the string conversion routines in this file should be rewritten to avoid
the use of the higher-level function sprintf().
------------------------------------------------------------------------------*/
//----------------------//
//       intstring      //
//----------------------//
const char* intstring(long l, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*ld", fw, n_digits, l);
    return buf;
    } // End of function intstring.

//----------------------//
//       uintstring     //
//----------------------//
const char* uintstring(long l, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*lu", fw, n_digits, l);
    return buf;
    } // End of function intstring.

/*------------------------------------------------------------------------------
Return a static _over-writeable_ string of hex chars.
------------------------------------------------------------------------------*/
//----------------------//
//       hexstring      //
//----------------------//
const char* hexstring(unsigned long l, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*lx", fw, n_digits, l);
    return buf;
    } // End of function hexstring.

/*------------------------------------------------------------------------------
Return a static _over-writeable_ string of 2 hex chars.
------------------------------------------------------------------------------*/
//----------------------//
//        hex2u8        //
//----------------------//
const char* hex2u8(uint8 x) {
    static char buf[3] = "00";

    uint8 d0 = x >> 4;
    uint8 d1 = x - (d0 << 4);

    if (d0 <= 9)
        buf[0] = '0' + d0;
    else
        buf[0] = 'a' - 10 + d0;
    if (d1 <= 9)
        buf[1] = '0' + d1;
    else
        buf[1] = 'a' - 10 + d1;
    return buf;
    } // End of function hex2u8.

//----------------------//
//       octstring      //
//----------------------//
const char* octstring(unsigned long l, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*lo", fw, n_digits, l);
    return buf;
    } // End of function octstring.

//----------------------//
//       f_string       //
//----------------------//
const char* f_string(double r, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*lf", fw, n_digits, r);
    return buf;
    } // End of function f_string.

//----------------------//
//       e_string       //
//----------------------//
const char* e_string(double r, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*le", fw, n_digits, r);
    return buf;
    } // End of function e_string.

//----------------------//
//       g_string       //
//----------------------//
const char* g_string(double r, int fw, int n_digits) {
    const int buf_length = 80;
    static char buf[buf_length];

    if (fw > buf_length - 1)
        fw = buf_length - 1;
    if (fw < -buf_length + 1)
        fw = -buf_length + 1;
    if (n_digits > buf_length - 1)
        n_digits = buf_length - 1;
    sprintf(buf, "%*.*lg", fw, n_digits, r);
    return buf;
    } // End of function g_string.

/*-------------------------------------------------------------------------
Here's an example of what this function produces for a typical ping packet.

45 00 00 54  fd dc 00 00  40 01 be 0c  cb 26 94 3e     E..T....@....&.>
cb 26 94 34  08 00 56 84  42 20 00 00  07 e8 09 3b     .&.4..V.B .....;
5b 35 08 00  08 09 0a 0b  0c 0d 0e 0f  10 11 12 13     [5..............
14 15 16 17  18 19 1a 1b  1c 1d 1e 1f  20 21 22 23     ............ !"#
24 25 26 27  28 29 2a 2b  2c 2d 2e 2f  30 31 32 33     $%&'()*+,-./0123
34 35 36 37                                            4567
-------------------------------------------------------------------------*/
//----------------------//
//      hex_print       //
//----------------------//
void hex_print(const char* buf, int n_bytes, ostream& os) {
    if (!buf || n_bytes <= 0)
        return;

    int i = 0;
    for (i = 0; i < n_bytes; ++i) {
        os << byte2hex[0xff & buf[i]] << " ";
        if (i % 16 == 15) {
            os << "    ";
            for (int j = (i / 16) * 16; j <= i; ++j) {
                char c = buf[j];
                if (isprint(c))
                    os << c;
                else
                    os << ".";
                }
            os << NL;
            }
        else if (i % 4 == 3)
            os << " ";
        }
    if (i % 16 != 0) {
        int n = (16 - i % 16) * 3 + (15 - i % 16)/4;
        for (int x = 0; x < n; ++x)
            os << " ";
        os << "    ";
        for (int j = (i / 16) * 16; j < i; ++j) {
            char c = buf[j];
            if (isprint(c))
                os << c;
            else
                os << ".";
            }
        os << NL;
        }
//    os << flush;
    } // End of function hex_print.

/*-------------------------------------------------------------------------
Here's an example output from this function for a ping packet, the
same packet as shown in the example for hex_print().

01000101 00000000 00000000 01010100  11111101 11011100 00000000 00000000
01000000 00000001 10111110 00001100  11001011 00100110 10010100 00111110
11001011 00100110 10010100 00110100  00001000 00000000 01010110 10000100
01000010 00100000 00000000 00000000  00000111 11101000 00001001 00111011
01011011 00110101 00001000 00000000  00001000 00001001 00001010 00001011
00001100 00001101 00001110 00001111  00010000 00010001 00010010 00010011
00010100 00010101 00010110 00010111  00011000 00011001 00011010 00011011
00011100 00011101 00011110 00011111  00100000 00100001 00100010 00100011
00100100 00100101 00100110 00100111  00101000 00101001 00101010 00101011
00101100 00101101 00101110 00101111  00110000 00110001 00110010 00110011
00110100 00110101 00110110 00110111
-------------------------------------------------------------------------*/
//----------------------//
//      bin_print       //
//----------------------//
void bin_print(const char* buf, int n_bytes, ostream& os) {
    if (!buf || n_bytes <= 0)
        return;

    int i = 0;
    for (i = 0; i < n_bytes; ++i) {
        os << byte2bin[0xff & buf[i]] << " ";
        if (i % 8 == 7)
            os << NL;
        else if (i % 4 == 3)
            os << " ";
        }
    if (i % 8 != 0)
        os << NL;
//    os << flush;
    } // End of function bin_print.

//----------------------//
//       ipstring       //
//----------------------//
const char* ipstring(unsigned long x) {
    static char buf[16] = "...............";
    sprintf(buf, "%u.%u.%u.%u", (x >> 24) & 0xff,
                                (x >> 16) & 0xff,
                                (x >>  8) & 0xff,
                                 x        & 0xff);
    return buf;
    } // End of function ipstring.

//----------------------//
//    bin_dump_header   //
//----------------------//
int bin_dump_header(int fd) {
    if (fd <= 0)
        return -1;
    static char prog_name[8] = { 's', 'n', 'o', 'o', 'p', 0, 0, 0 };

    // Use MSB-first long integers:
    static char version[8] = { 0, 0, 0, 2, 0, 0, 0, 4 };

    if (write(fd, prog_name, 8) < 0) {
        perror("bin dump write");
        return -1;
        }
    if (write(fd, (char*)version, 8) < 0) {
        perror("bin dump write");
        return -1;
        }
    return 0;
    } // End of function bin_dump_header.

/*------------------------------------------------------------------------------
Dump an IP packet to a file in the format that "snoop" expects (hopefully).
------------------------------------------------------------------------------*/
//----------------------//
//       bin_dump       //
//----------------------//
int bin_dump(int fd, const char* buf, int n_bytes, const timeval& ret_time) {
    if (fd <= 0 || !buf || n_bytes <= 0)
        return -1;

    // The record header:
    static ulong n[6] = { 0, 0, 0, 0, 0, 0 };   // 24 bytes.
    static char pad[3] = { 0, 0, 0 };           // Padding to 4-byte alignment.

    // Ethernet addresses (2 * 6 bytes) + ethernet packet type:
    static uint16 ether_header[7] = { 0, 0, 0, 0, 0, 0, 0x0800 };

    // Must round n_bytes + 14 up to a multiple of 4:
//    int n_pad = ((n_bytes + 14) & ~3) - n_bytes - 14;
    int n_pad = (4 - ((n_bytes + 14) & 3)) & 3;

    n[0] = n[1] = htonl(14 + n_bytes);    // Ethernet header is 14 bytes.
    n[2] = htonl(24 + 14 +n_bytes+n_pad); // Record header: 24; padding: n_pad.
    n[3] = 0;                             // Dropped packets.
    n[4] = htonl(ret_time.tv_sec);
    n[5] = htonl(ret_time.tv_usec);

    // Do an atomic write of the whole record:
    int total_length = 24 + 14 + n_bytes + n_pad;
    char* wbuf = new char[total_length];
    memcpy(wbuf, (char*)n, 24);

    // Correct the ether_header last long integer for PCs:
    ether_header[6] = htons(0x0800);

    memcpy(wbuf + 24,         (char*)ether_header, 14);
    memcpy(wbuf + 24 + 14,           buf,          n_bytes);
    memcpy(wbuf + 24 + 14 + n_bytes, pad,          n_pad);

    if (write(fd, wbuf, total_length) < 0) {
        perror("bin dump write");
        delete[] wbuf;
        return -1;
        }
    delete wbuf;
    return 0;
    } // End of function bin_dump.
