// src/aksl/nbytes.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_NBYTES_H
#define AKSL_NBYTES_H
/*------------------------------------------------------------------------------
Classes defined in this file:

nbytes::
------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
#ifdef WIN32
#ifndef AKSL_X_IO_H
#define AKSL_X_IO_H
#include <io.h>
#endif

#ifndef AKSL_X_WINDOWS_H
#define AKSL_X_WINDOWS_H
// #include <windows.h>
#endif
#endif /* WIN32 */

#ifdef HAVE_UNISTD_H
#ifndef AKSL_X_UNISTD_H
#define AKSL_X_UNISTD_H
#include <unistd.h>
#endif
#endif /* HAVE_UNISTD_H */

/*------------------------------------------------------------------------------
For simplicity, it will be assumed that pc is non-null if and only if n > 0,
and pc is null if and only if n == 0.
That is, there will be no such thing as a zero-length array of chars here.
The difference is meaningful for strings, where the empty string is different to
a null char pointer. But here the zero-length string will be assumed to be
synonymous with a null char pointer.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The per-process buffer nbytes_buf is exported to derived classes, so that it can
be re-used. Its size is nbytes_bufsize.
------------------------------------------------------------------------------*/
//----------------------//
//       nbytes::       //
//----------------------//
struct nbytes {
protected:
    char* pc;
    int n;

    static char* nbytes_buf;    // A per-process buffer for derived classes.
    static int nbytes_bufsize;  // Size of nbytes_buf.
public:
    const char* bytes() const { return pc; }
    int n_bytes() const { return n; }
    int length() { return pc ? n : 0; }
    bool_enum empty() const { return bool_enum(!pc); }
    void clear() { delete[] pc; pc = 0; n = 0; }
    void copy_from(const char* pc1, int n1);    // Reads n1 bytes from buffer.
    void swallow(char*& pc1, int n1);           // Swallows n1-byte heap mem.
    int copy_to(char* buf, int bufsize);        // Returns n bytes copied.

    // Read from a Unix file descriptor. (If bufsize > 0, use different buffer.)
    int read(int fd, int bufsize = 0, bool_enum append = false);
    int read_append(int fd, int bufsize = 0) { return read(fd, bufsize, true); }

    int write(int fd) const { return ::write(fd, pc, n); }

    // Should have a non-blocking version of read(), which calls select():
//    int read_nonblock(int fd, int bufsize = 0);

    // Function to modify bytes:
    void set16(int offset, uint16 value);
    void set32(int offset, uint32 value);

    // Dodgy function to swap the IP addresses, regarding bytes as IP payload.
    void swap_ip();

    char operator[](int i) { return (pc && i >= 0 && i < n) ? pc[i] : 0; }

    nbytes& operator=(const nbytes& x);
    nbytes(const nbytes& x);
    nbytes() { pc = 0; n = 0; }
    ~nbytes() { delete[] pc; }
    }; // End of struct nbytes.

// Exported buffer for use by classes derived from "nbytes".
// extern char nbytes_buf[nbytes_bufsize];

#endif /* AKSL_NBYTES_H */
