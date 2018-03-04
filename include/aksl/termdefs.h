// src/aksl/termdefs.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_TERMDEFS_H
#define AKSL_TERMDEFS_H
#ifdef unix
/*------------------------------------------------------------------------------
Classes in this file:

terminal::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_TERMIOS_H
#define AKSL_X_TERMIOS_H
#include <termios.h>
#endif

/*------------------------------------------------------------------------------
This represents a termio-style of terminal -- basically a serial line in this
case, but this could be extended to general terminals.
------------------------------------------------------------------------------*/
//----------------------//
//       terminal::     //
//----------------------//
struct terminal {
private:
    termios* tio;           // The last set of terminal settings.
    termios* tio_old;       // The previous set of terminal settings.
    char* dev_path;         // Path of current device.
    int fd;                 // File descriptor for open device. -1 if not open.
    ulong speed_mask;       // Speed mask.
    bool_enum software_carrier_mode;
public:
    bool_enum crtscts;      // Set CRTSCTS flag.
    void baud_rate_print_diff(ostream& = cout);
    void termios_print_diff(ostream& = cout);
    int config(const char* dev_name = 0, ulong speed_mask = 0, int trace = 0);

//    terminal& operator=(const terminal& x) {}
//    terminal(const terminal& x) {};
    terminal() {
        tio = 0;
        tio_old = 0;
        dev_path = 0;
        fd = -1;
        speed_mask = 0;
        software_carrier_mode = false;
        crtscts = true;
        }
    ~terminal() {
        delete tio;
        delete tio_old;
        delete[] dev_path;
        }
    }; // End of struct terminal.

extern void baud_rate_print_diff(ulong cbaud, ulong cbaud0);
extern void termios_print_diff(termios* tio, termios* tio0);
extern int term_config(bool_enum software_carrier_mode = false,
                       bool_enum crtscts = true,
                       const char* dev_name = 0,
                       ulong speed_mask = 0, int trace = 0);

#endif /* unix */
#endif /* AKSL_TERMDEFS_H */
