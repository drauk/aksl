// src/aksl/termdefs.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

// Some definitions for terminal interfaces.
#ifdef unix
/*------------------------------------------------------------------------------
Functions in this file:

baud_rate_print_diff
termios_print_diff
term_config
terminal::
    config
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/termdefs.h"
#ifndef AKSL_STR_H
#include "aksl/str.h"
#endif
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
// stdio.h should not be used for I/O -- use only sprintf().
// Use C++ streams instead of printf, fprintf etc.
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif

#ifdef HAVE_FCNTL_H
#ifndef AKSL_X_FCNTL_H
#define AKSL_X_FCNTL_H
#include <fcntl.h>
#endif
#endif

#ifdef HAVE_UNISTD_H
#ifndef AKSL_X_UNISTD_H
#define AKSL_X_UNISTD_H
#include <unistd.h>
#endif
#endif

#ifndef AKSL_X_ERRNO_H
#define AKSL_X_ERRNO_H
#include <errno.h>
#endif

// This is needed on goose, but not on emu.
#ifndef AKSL_X_IOMANIP_H
#define AKSL_X_IOMANIP_H
#include <iomanip>
#endif

#ifdef linux
#ifndef AKSL_X_SYS_IOCTL_H
#define AKSL_X_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif

// Serial port defaults:
static const char deft_dev_name[]   = "/dev/ttya";
const ulong       deft_speed_mask   = B9600;

//----------------------//
// baud_rate_print_diff //
//----------------------//
void baud_rate_print_diff(ulong cbaud, ulong cbaud0) {
    long b = 0;
    switch (cbaud0) {
    case B0: b = 0; break;
    case B50: b = 50; break;
    case B75: b = 75; break;
    case B110: b = 110; break;
    case B134: b = 134; break;
    case B150: b = 150; break;
    case B200: b = 200; break;
    case B300: b = 300; break;
    case B600: b = 600; break;
    case B1200: b = 1200; break;
    case B1800: b = 1800; break;
    case B2400: b = 2400; break;
    case B4800: b = 4800; break;
    case B9600: b = 9600; break;
    case B19200: b = 19200; break;
    case B38400: b = 38400; break;
    default:
        b = -1;
        cout << "Unrecognized baud rate.\n";
        break;
        } // End of switch(cbaud0).
    long b0 = b;

    switch(cbaud) {
    case B0: b = 0; break;
    case B50: b = 50; break;
    case B75: b = 75; break;
    case B110: b = 110; break;
    case B134: b = 134; break;
    case B150: b = 150; break;
    case B200: b = 200; break;
    case B300: b = 300; break;
    case B600: b = 600; break;
    case B1200: b = 1200; break;
    case B1800: b = 1800; break;
    case B2400: b = 2400; break;
    case B4800: b = 4800; break;
    case B9600: b = 9600; break;
    case B19200: b = 19200; break;
    case B38400: b = 38400; break;
    default:
        b = -1;
        cout << "Unrecognized baud rate.\n";
        break;
        } // End of switch(cbaud).

    cout << "    Baud rate = " << b << " [" << b0 << "].\n";
    } // End of function baud_rate_print_diff.

/*------------------------------------------------------------------------------
The second parameter is taken to be the previous value of the structure.
------------------------------------------------------------------------------*/
//----------------------//
//  termios_print_diff  //
//----------------------//
void termios_print_diff(termios* tio, termios* tio0) {
    if (!tio || !tio0)
        return;

#define pr0(s, m) \
    cout << s << " = 0x" \
         << hex << tio->m << " [0x" << tio0->m << "].\n" << dec
#define pr1(s, m) \
    cout << "    " << s << " = " << bool_string(m & tio->c_iflag) \
         << " [" << bool_string(m & tio0->c_iflag) << "].\n"
#define pr2(s, m) \
    cout << "    " << s << " = " << bool_string(m & tio->c_oflag) \
         << " [" << bool_string(m & tio0->c_oflag) << "].\n"
#define pr3(s, m) \
    cout << "    " << s << " = " << bool_string(m & tio->c_cflag) \
         << " [" << bool_string(m & tio0->c_cflag) << "].\n"
#define pr4(s, m) \
    cout << "    " << s << " = " << bool_string(m & tio->c_lflag) \
         << " [" << bool_string(m & tio0->c_lflag) << "].\n"
#define pr5(s, ii) \
    cout << "    " << s << " = 0x" << (long)tio->c_cc[ii] \
         << " [0x" << (long)tio0->c_cc[ii] << "].\n";

    pr0("c_iflag", c_iflag);
    pr1("IGNBRK  ", IGNBRK);
    pr1("BRKINT  ", BRKINT);
    pr1("IGNPAR  ", IGNPAR);
    pr1("PARMRK  ", PARMRK);
    pr1("INPCK   ", INPCK);
    pr1("ISTRIP  ", ISTRIP);
    pr1("INLCR   ", INLCR);
    pr1("IGNCR   ", IGNCR);
    pr1("ICRNL   ", ICRNL);
    pr1("IUCLC   ", IUCLC);
    pr1("IXON    ", IXON);
    pr1("IXANY   ", IXANY);
    pr1("IXOFF   ", IXOFF);
    pr1("IMAXBEL ", IMAXBEL);

    pr0("c_oflag", c_oflag);
    pr2("OPOST   ",  OPOST);
    pr2("OLCUC   ",  OLCUC);
    pr2("ONLCR   ",  ONLCR);
    pr2("OCRNL   ",  OCRNL);
    pr2("ONOCR   ",  ONOCR);
    pr2("ONLRET  ",  ONLRET);
    pr2("OFILL   ",  OFILL);
    pr2("OFDEL   ",  OFDEL);
    pr2("NLDLY   ",  NLDLY);
    cout << "    CRDLY    = 0x"
         << hex << ((CRDLY & tio->c_oflag) >> 9)
         << " [0x" << ((CRDLY & tio0->c_oflag) >> 9) << "].\n" << dec;
    cout << "    TABDLY   = 0x"
         << hex << ((TABDLY & tio->c_oflag) >> 11)
         << " [0x" << ((TABDLY & tio0->c_oflag) >> 11) << "].\n" << dec;
    pr2("BSDLY   ",  BSDLY);
    pr2("VTDLY   ",  VTDLY);
    pr2("FFDLY   ",  FFDLY);

    pr0("c_cflag", c_cflag);
    ulong cbaud = CBAUD & tio->c_cflag;
    ulong cbaud0 = CBAUD & tio0->c_cflag;
    cout << "    CBAUD    = 0x"
         << hex << cbaud << " [0x" << cbaud0 << "].\n" << dec;
    baud_rate_print_diff(cbaud, cbaud0);
    long csize = CSIZE & tio->c_cflag;
    long csize0 = CSIZE & tio0->c_cflag;
    cout << "    CSIZE    = 0x"
         << hex << (csize >> 4) << " [0x" << (csize0 >> 4) << "].\n" << dec;
    int cs = 0;
    int cs0 = 0;
    switch(csize) {
    case CS5: cs = 5; break;
    case CS6: cs = 6; break;
    case CS7: cs = 7; break;
    case CS8: cs = 8; break;
        } // End of switch(csize).
    switch(csize0) {
    case CS5: cs0 = 5; break;
    case CS6: cs0 = 6; break;
    case CS7: cs0 = 7; break;
    case CS8: cs0 = 8; break;
        } // End of switch(csize0).
    cout << "Character size = " << hex << cs
         << " [" << cs0 << "] bits.\n" << dec;
    pr3("CSTOPB  ", CSTOPB);
    pr3("CREAD   ", CREAD);
    pr3("PARENB  ", PARENB);
    pr3("PARODD  ", PARODD);
    pr3("HUPCL   ", HUPCL);
    pr3("CLOCAL  ", CLOCAL);
    ulong cibaud = CIBAUD & tio->c_cflag;
    ulong cibaud0 = CIBAUD & tio0->c_cflag;
#ifndef linux
    cout << "    CIBAUD   = 0x"
         << hex << (cibaud >> IBSHIFT)
         << " [0x" << (cibaud0 >> IBSHIFT) << "].\n" << dec;
    cout << "Input baud rate:\n";
    baud_rate_print_diff(cibaud >> IBSHIFT, cibaud0 >> IBSHIFT);
#endif
    pr3("CRTSCTS ", CRTSCTS);

    pr0("c_lflag", c_lflag);
    pr4("ISIG    ", ISIG);
    pr4("ICANON  ", ICANON);
    pr4("XCASE   ", XCASE);
    pr4("ECHO    ", ECHO);
    pr4("ECHOE   ", ECHOE);
    pr4("ECHOK   ", ECHOK);
    pr4("ECHONL  ", ECHONL);
    pr4("NOFLSH  ", NOFLSH);
    pr4("TOSTOP  ", TOSTOP);
    pr4("ECHOCTL ", ECHOCTL);
    pr4("ECHOPRT ", ECHOPRT);
    pr4("ECHOKE  ", ECHOKE);
    pr4("FLUSHO  ", FLUSHO);
    pr4("PENDIN  ", PENDIN);
    pr4("IEXTEN  ", IEXTEN);
    pr5("VINTR   ", 0);
    pr5("VQUIT   ", 1);
    pr5("VERASE  ", 2);
    pr5("VKILL   ", 3);
    pr5("VEOF    ", 4);
    pr5("VEOL    ", 5);
    pr5("VEOL2   ", 6);
    pr5("VSWTCH  ", 7);
    pr5("VSTART  ", 8);
    pr5("VSTOP   ", 9);
    pr5("VSUSP   ", 10);
    pr5("        ", 11);
    pr5("VREPRINT", 12);
    pr5("VDISCARD", 13);
    pr5("VWERASE ", 14);
    pr5("VLNEXT  ", 15);
    pr5("        ", 16);
    } // End of function termios_print_diff.

/*------------------------------------------------------------------------------
If successful, return the fd value of the new channel. On failure, a negative
value is returned.
------------------------------------------------------------------------------*/
//----------------------//
//      term_config     //
//----------------------//
int term_config(bool_enum software_carrier_mode, bool_enum crtscts,
                const char* dev_name,
                ulong speed_mask, int trace) {
    if (!dev_name)
        dev_name = deft_dev_name;

    if (speed_mask == 0)
        speed_mask = deft_speed_mask;

    // Open hf device driver:
    if (trace >= 4)
        cout << dev_name << ": opening device..." << endl;

    int open_mode = O_RDWR;
    if (software_carrier_mode)
        open_mode |= O_NDELAY;
    int fd = open(dev_name, open_mode);
    if (fd < 0) {
        cerr << "Failed to open device " << dev_name << DOTNL;
        perror("term_config");
        return -1;
        }
    if (trace >= 4)
        cout << dev_name << ": opened device." << endl;

    // Do the ioctls here:
    if (software_carrier_mode) {
        // Set the software DCD mode to "on":
        int i = 1;
        int ret = ioctl(fd, (int)TIOCSSOFTCAR, (caddr_t)&i);
        if (ret < 0) {
            cerr << "ioctl TIOCSSOFTCAR failed for device "
                 << dev_name << DOTNL;
            perror("term_config: ioctl TIOCSSOFTCAR");
            return -1;
            }
        }
    termios tio;
    int ret = ioctl(fd, TCGETS, &tio);
    if (ret < 0) {
        cerr << "Ioctl failed for device " << dev_name << DOTNL;
        perror("term_config");
        return -1;
        }
    if (trace >= 4)
        cout << dev_name << ": ioctl TCGETS succeeded.\n" << flush;

    // Save the initial values:
    termios tio0 = tio;

    // Set up the TTY interface for clear 8-bit communication:
    // Note; should get a better understanding of this, and do it properly!
// #define FORCE_SIMPLE_CONFIG
#if defined(SOLARIS) || defined(FORCE_SIMPLE_CONFIG)
    tio.c_iflag = 0;
    tio.c_oflag = 0;
    tio.c_cflag = B9600|CS8|CREAD|CLOCAL;
    if (crtscts)
        tio.c_cflag |= CRTSCTS;
    tio.c_lflag = 0;
#else
    tio.c_iflag |= (IGNBRK|BRKINT|IGNPAR);
    tio.c_iflag &= ~(INLCR|IGNCR|ICRNL|IUCLC|IXON|IXOFF|ISTRIP);

//    tio.c_oflag &= ~(ONLCR|OCRNL|ONLRET|OPOST);

    // Modify output baud rate:
    tio.c_cflag &= ~CBAUD;
    tio.c_cflag |= speed_mask;

    // Modify input baud rate:
#ifndef linux
    tio.c_cflag &= ~CIBAUD;
    tio.c_cflag |= (speed_mask << IBSHIFT) & CIBAUD;
#endif
    // Change from 7-bit to 8-bit:
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;

    // Turn on/off RTS-CTS:
    tio.c_cflag |= (CLOCAL|HUPCL|CREAD);
    if (crtscts)
        tio.c_cflag |= CRTSCTS;
    else
        tio.c_cflag &= ~CRTSCTS;

//    tio.c_cflag |= (CLOCAL|HUPCL);

    // Turn off parity enable:
    tio.c_cflag &= ~(PARENB);

    // Turn off echo and canonical mode:
//    tio.c_lflag &= ~(ISIG|ICANON|IEXTEN|ECHO);

    // Get immediate return if >= VMIN bytes are available:
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
#endif

    // Send the new setup to the tty:
    ret = ioctl(fd, TCSETS, &tio);
    if (ret < 0 ) {
        perror("term_config");
        return -1;
        }
    if (trace >= 5) {
        cout << dev_name << ": ioctl TCSETS succeeded.\n";
        cout << "State of " << dev_name << " after [before] configuration:\n";
        termios_print_diff(&tio, &tio0);
        cout << flush;
        }
    return fd;
    } // End of function term_config.

//----------------------//
//   terminal::config   //
//----------------------//
int terminal::config(const char* dev_name, ulong speed, int trace) {
    int ret = term_config(software_carrier_mode,
                          crtscts, dev_name, speed, trace);
    if (ret < 0)
        return ret;
    delete[] dev_path;
    dev_path = new_strcpy(dev_name);
    speed_mask = speed;
    return ret;
    } // End of function terminal::config.

#endif /* unix */
