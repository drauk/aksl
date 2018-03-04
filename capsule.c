// src/aksl/capsule.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

frame8::
    print
    encode
f8stream::
    decode
frame32::
    print
    encode
f32stream::
    decode
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/capsule.h"

#ifndef AKSL_FORM_H
#include "aksl/form.h"
#endif
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#if defined(sun)
#ifndef AKSL_X_MALLOC_H
#define AKSL_X_MALLOC_H
#include <malloc.h>
#endif
#endif

#ifndef AKSL_X_MEMORY_H
#define AKSL_X_MEMORY_H
#include <memory.h>
#endif
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

//----------------------//
//    frame8::print     //
//----------------------//
void frame8::print(ostream& os) const {
    os << "    opcode  = " << opcode << NL;
    os << "    arg     = " << arg << NL;
    os << "    LI      = " << LI << NL;
    os << "    payload:\n";
    if (payload)
        hex_print(payload, LI, os);
    os << NL;
    os << "    n_coded = " << n_coded << NL;
    os << "    coded:\n";
    if (coded)
        hex_print(coded, n_coded, os);
    os << NL;
    } // End of function frame8::print.

/*------------------------------------------------------------------------------
This function encodes a single data frame.
If successful, the encoded array is returned as "coded", the number of
encoded bytes is returned as "n_coded", and the return value is 0.
Otherwise "coded" is set to 0, "n_coded" is set to 0, and the return value is
negative. Any previous array found in "coded" is always deleted.
(This function does encode the sync byte!)
------------------------------------------------------------------------------*/
//----------------------//
//    frame8::encode    //
//----------------------//
int frame8::encode(uint8 sync_byte) {
    n_coded = 0;
    delete[] coded;
    coded = 0;
    if (LI > frame8_max_LI || (LI > 0 && !payload))
        return -1;

    // frame8 length: sync(1) + opcode(1) + arg/LI(2) + HCS(2)
    //                        + payload + DCS(4):
    n_coded = frame8_hdr_size + LI + frame8_dcs_size;
    char* pc2 = coded = new char[n_coded];

    // Encode the PDU:
    *pc2++ = sync_byte;
    *pc2++ = opcode;
    *pc2++ = ((arg << 4) & 0xf0) | ((LI >> 8) & 0x0f);
    *pc2++ = LI & 0xff;
    *pc2++ = crc8calc(coded + 1, 3); // CRC-8 over first 3 octets after sync.
    if (LI > 0)
        memcpy(pc2, payload, LI);
    pc2 += LI;
    *pc2++ = crc8calc(payload, LI);
    return 0;
    } // End of function frame8::encode.

/*------------------------------------------------------------------------------
This function decodes a single message out of given data bytes. The new input
data bytes are passed as buf and nbuf. If one or more messages are successfully
decoded, then a single "frame8" structure is returned, containing the opcode,
payload length (LI) and payload fields. If a message is returned, then this
function should be called with zero data bytes until no more messages are
returned, so as to empty the message FIFO.
Errored data is silently ignored (at present).
------------------------------------------------------------------------------*/
//----------------------//
//   f8stream::decode   //
//----------------------//
frame8* f8stream::decode(const char* bytes, int n_bytes) {
    // Return a message if the FIFO is non-empty:
    if (!bytes || n_bytes <= 0)
        return fifo.popfirst();

    // Attempt to decode the incoming bytes:
    int maxlen = nbuf;              // Number of valid bytes in buffer.
    for (int i = 0; i < n_bytes; ) {
        char c;                     // Next character to interpret.
        if (nbuf < maxlen)
            c = buf[nbuf++];        // Recycle an old byte.
        else {
            c = bytes[i++];         // Fetch a new byte.
            buf[nbuf++] = c;
            maxlen = nbuf;
            }

        // Switch depending on current parse state, which depends on nbuf:
        switch(nbuf) {
        case 1: // Expecting sync byte.
            if (c != sync_byte) {
//                cout << flush;
//                fprintf(stderr, "false sync byte: 0x%02x\n", (int)c);

                // Shift the valid bytes to the left:
                for (int k = 0; k < maxlen - 1; ++k)
                    buf[k] = buf[k + 1];
                maxlen -= 1;
                nbuf = 0;
                }
            break;
        case 2: // Expecting opcode.
            // Do nothing.
            break;
        case 3: // Expecting MSB of LI.
            // Do nothing.
            break;
        case 4: // Expecting LSB of LI.
            // Interpret the LI field to help locate the DCS later:
//            arg = buf[2] & 0xf0;
            LI = (((uint16)buf[2] << 8) & 0x0f00) | (buf[3] & 0xff);
            break;
        case 5: { // Expecting HCS.
            char crc = crc8calc(buf + 1, 3);
            if (crc != c) { // The HCS failed!!! What shall we do?
//                fprintf(stderr, "false HCS: 0x%02x\n", (int)c);

                // Try to find another sync byte in the buffer:
                for (int j = 1; j < maxlen; ++j) {
                    if (buf[j] == sync_byte) {
                        // Shift the reassembly buffer to the left:
                        for (int k = 0; k < maxlen - j; ++k)
                            buf[k] = buf[k + j];
                        maxlen -= j;
                        nbuf = 0;
                        break;
                        }
                    }

                // If no sync byte found, start from nothing again:
                if (nbuf > 0) { // Dodgy test.
                    nbuf = 0;
                    maxlen = 0;
                    }
//                fprintf(stderr, "skipping %d non-sync characters\n", j - 1);
                }
            }
            break;
        default:    // Either a payload byte or the DCS.
            if (nbuf < int(LI) + 6) { // Payload byte.
                // Do nothing.
                }
            else if (nbuf == LI + 6) { // The DCS.
                // Check the CRC, omitting the first 5 chars of buffer:
                // (The DCS only covers the payload.)
                char crc = crc8calc(buf + 5, LI);
                if (crc == c) { // We've got a match!
                    // Assemble it into a message:
                    frame8* pm1 = new frame8;
                    pm1->opcode = buf[1];
                    pm1->arg = (buf[2] >> 4) & 0x0f;
                    pm1->LI = LI;
                    pm1->payload = new char[LI];
                    memcpy(pm1->payload, buf + 5, LI);
                    // Note: The coded version could be copied into the frame8
                    // structure at this point, if this would be useful.

                    // Find the end of the fifo (slowly!!):
                    fifo.append(pm1);

                    // Remove the parsed bytes from the reassembly buffer:
                    for (int k = 0; k < maxlen - nbuf; ++k)
                        buf[k] = buf[k + nbuf];
                    maxlen -= nbuf;
                    nbuf = 0;
                    }
                else { // DCS fail: try to re-sync from reassembly buffer.
//                    fprintf(stderr, "false DCS: 0x%02x\n", (int)c);

                    // Try to find another sync byte in reassembly buffer:
                    for (int j = 1; j < maxlen; ++j) {
                        if (buf[j] == sync_byte) {
                            // Shift the reassembly buffer to the left:
                            for (int k = 0; k < maxlen - j; ++k)
                                buf[k] = buf[k + j];
                            maxlen -= j;
                            nbuf = 0;
                            break;
                            }
                        }

                    // If no sync byte found, start from nothing again:
                    if (nbuf > 0) { // Dodgy test.
                        nbuf = 0;
                        maxlen = 0;
                        }
//                    fprintf(stderr, "skipping %d non-sync chars\n", j - 1);
                    }
                }
            else { // nbuf > LI + 6: should never happen!!!
//                fprintf(stderr, "Warning: impossible parse state.\n");
                // Clear the buffer, but keep reading the input buffer:
                nbuf = 0;
                maxlen = 0;
                }
            break;
            } // End of switch(p->len).
        }

    return fifo.popfirst();
    } // End of function f8stream::decode.

//----------------------//
//    frame32::print    //
//----------------------//
void frame32::print(ostream& os) const {
    os << "    opcode  = " << opcode << NL;
    os << "    arg     = " << arg << NL;
    os << "    LI      = " << LI << NL;
    os << "    payload:\n";
    if (payload)
        hex_print(payload, LI, os);
    os << NL;
    os << "    n_coded = " << n_coded << NL;
    os << "    coded:\n";
    if (coded)
        hex_print(coded, n_coded, os);
    os << NL;
    } // End of function frame32::print.

/*------------------------------------------------------------------------------
This function encodes a single data frame.
If successful, the encoded array is returned as "coded", the number of
encoded bytes is returned as "n_coded", and the return value is 0.
Otherwise "coded" is set to 0, "n_coded" is set to 0, and the return value is
negative. Any previous array found in "coded" is always deleted.
(This function does encode the sync byte!)
------------------------------------------------------------------------------*/
//----------------------//
//    frame32::encode   //
//----------------------//
int frame32::encode(uint8 sync_byte) {
    n_coded = 0;
    delete[] coded;
    coded = 0;
    if (LI > frame32_max_LI || (LI > 0 && !payload))
        return -1;

    // frame32 length: sync(1), opcode(1), arg/LI(2), HCS(2), payload, DCS(4):
    n_coded = frame32_hdr_size + LI + frame32_dcs_size;
    char* pc2 = coded = new char[n_coded];

    // Encode the PDU:
    *pc2++ = sync_byte;
    *pc2++ = opcode;
    *pc2++ = ((arg << 6) & 0xc0) | ((LI >> 8) & 0x3f);
    *pc2++ = LI & 0xff;
    u16encode(pc2, crc16calc(coded + 1, 3));
    pc2 += 2;
    if (LI > 0)
        memcpy(pc2, payload, LI);
    pc2 += LI;

    // Calculate CRC-32 over all bytes except sync byte and DCS:
    u32encode(pc2, crc32calc(coded + 1, frame32_hdr_size - 1 + LI));
    return 0;
    } // End of function frame32::encode.

/*------------------------------------------------------------------------------
This function decodes a single message out of given data bytes. The new input
data bytes are passed as buf and nbuf. If one or more messages are successfully
decoded, then a single "frame32" structure is returned, containing the opcode,
payload length (LI) and payload fields. If a message is returned, then this
function should be called with zero data bytes until no more messages are
returned, so as to empty the message FIFO.
Errored data is silently ignored (at present).
------------------------------------------------------------------------------*/
//----------------------//
//   f32stream::decode  //
//----------------------//
frame32* f32stream::decode(const char* bytes, int n_bytes) {
    // Return a message if the FIFO is non-empty:
    if (!bytes || n_bytes <= 0)
        return fifo.popfirst();

    // Attempt to decode the incoming bytes:
    int maxlen = nbuf;              // Number of valid bytes in buffer.
    for (int i = 0; i < n_bytes; ) {
        char c;                     // Next character to interpret.
        if (nbuf < maxlen)
            c = buf[nbuf++];        // Recycle an old byte.
        else {
            c = bytes[i++];         // Fetch a new byte.
            buf[nbuf++] = c;
            maxlen = nbuf;
            }
//        cout << "nbuf = " << nbuf << NL;

        // Switch depending on current parse state, which depends on nbuf:
        switch(nbuf) {
        case 1: // Expecting sync byte.
            if ((c & 0xff) != (sync_byte & 0xff)) {
//                cout << flush;
//                fprintf(stderr, "false sync byte: 0x%02x\n", (int)c);

                // Shift the valid bytes to the left:
                for (int k = 0; k < maxlen - 1; ++k)
                    buf[k] = buf[k + 1];
                maxlen -= 1;
                nbuf = 0;
//                cout << "Failed sync byte. Expected 0x" << hex2(sync_byte)
//                     << ", received 0x" << hex2(c & 0xff) << NL;
                }
            break;
        case 2: // Expecting opcode.
            // Do nothing.
            break;
        case 3: // Expecting MSB of LI.
            // Do nothing.
            break;
        case 4: // Expecting LSB of LI.
            // Interpret the LI field to help locate the DCS later:
//            arg = buf[2] & 0xc0;
            LI = (((uint16)buf[2] << 8) & 0x3f00) | (buf[3] & 0xff);
            break;
        case 5: // Expect first byte of HCS.
            // Do nothing.
            break;
        case 6: { // Expecting HCS.
            uint16 crc_calc = crc16calc(buf + 1, 3);
            uint16 crc_rcvd = u16decode(buf + 4);
            if (crc_calc != crc_rcvd) { // The HCS failed!!! What shall we do?
//                fprintf(stderr, "false HCS: 0x%02x\n", (int)c);
                cout << "CRC-16 HCS failed.\n";

                // Try to find another sync byte in the buffer:
                for (int j = 1; j < maxlen; ++j) {
                    if (buf[j] == sync_byte) {
                        // Shift the reassembly buffer to the left:
                        for (int k = 0; k < maxlen - j; ++k)
                            buf[k] = buf[k + j];
                        maxlen -= j;
                        nbuf = 0;
                        break;
                        }
                    }

                // If no sync byte found, start from nothing again:
                if (nbuf > 0) { // Dodgy test.
                    nbuf = 0;
                    maxlen = 0;
                    }
//                fprintf(stderr, "skipping %d non-sync characters\n", j - 1);
                }
            }
            break;
        default:    // Either a payload byte or the DCS.
            if (nbuf < frame32_hdr_size + int(LI) + frame32_dcs_size) {
                // Payload byte: do nothing.
                }
            else if (nbuf == frame32_hdr_size + LI + frame32_dcs_size) {
                // The DCS has arrived (maybe).
                // Check the CRC, including the first bytes 1 to 5 of buffer:
                // (The DCS covers the header (minus sync), HCS and payload.)
                uint32 crc_calc = crc32calc(buf + 1, frame32_hdr_size - 1 + LI);
                uint32 crc_rcvd = u32decode(buf + frame32_hdr_size + LI);
                if (crc_calc == crc_rcvd) { // We've got a match!
                    // Assemble it into a message:
                    frame32* pm1 = new frame32;
                    pm1->opcode = buf[1];
                    pm1->arg = (buf[2] >> 6) & 0x03;
                    pm1->LI = LI;
                    pm1->payload = new char[LI];
                    memcpy(pm1->payload, buf + frame32_hdr_size, LI);
                    // Note: The coded version could be copied into the frame32
                    // structure at this point, if this would be useful.

                    // Find the end of the fifo (slowly!!):
                    fifo.append(pm1);

                    // Remove the parsed bytes from the reassembly buffer:
                    for (int k = 0; k < maxlen - nbuf; ++k)
                        buf[k] = buf[k + nbuf];
                    maxlen -= nbuf;
                    nbuf = 0;
                    }
                else { // DCS fail: try to re-sync from reassembly buffer.
//                    fprintf(stderr, "false DCS: 0x%02x\n", (int)c);
                    cout << "CRC-32 DCS failed.\n";

                    // Try to find another sync byte in reassembly buffer:
                    for (int j = 1; j < maxlen; ++j) {
                        if (buf[j] == sync_byte) {
                            // Shift the reassembly buffer to the left:
                            for (int k = 0; k < maxlen - j; ++k)
                                buf[k] = buf[k + j];
                            maxlen -= j;
                            nbuf = 0;
                            break;
                            }
                        }

                    // If no sync byte found, start from nothing again:
                    if (nbuf > 0) { // Dodgy test.
                        nbuf = 0;
                        maxlen = 0;
                        }
//                    fprintf(stderr, "skipping %d non-sync chars\n", j - 1);
                    }
                }
            else { // nbuf > LI + 6: should never happen!!!
//                fprintf(stderr, "Warning: impossible parse state.\n");
                // Clear the buffer, but keep reading the input buffer:
                nbuf = 0;
                maxlen = 0;
                }
            break;
            } // End of switch(p->len).
        }

    return fifo.popfirst();
    } // End of function f32stream::decode.
