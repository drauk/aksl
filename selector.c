// src/aksl/selector.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Functions in this file:

dispatch
select_handler::
    select_handler
stdin_handler::
    handler
cdev_handler::
    handler
udp_delay_handler::
    copy_pkt
    handler
udp_handler::
    handler
udp_port_hand::
    action
udp_hand_set::
    open
tcp_handler::
    event_type_string
    open
    open
    handler
    write
    print
tcp_context::
    find_fd
    find_from
m_tcp_handler::
    event_type_string
    open
    handler
    print
fdtype::
    print
selector::
    selector
    print
    set_timer
    set_timer_rel
    cancel_timer
    set_wait_time
    set_fd_mask
    clear_fd_mask
    get_event
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/selector.h"
#ifndef AKSL_CHARBUF_H
#include "aksl/charbuf.h"
#endif
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif

// System header files:
#if defined(SOLARIS) && defined(HAVE_FCNTL_H)
#ifndef AKSL_X_FCNTL_H
#define AKSL_X_FCNTL_H
#include <fcntl.h>
#endif
#endif

#if !defined(SOLARIS) && !defined(WIN32) && defined(HAVE_SYS_IOCTL_H)
#ifndef AKSL_X_SYS_IOCTL_H
#define AKSL_X_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif
#endif

#ifndef AKSL_X_ERRNO_H
#define AKSL_X_ERRNO_H
#include <errno.h>
#endif

// Get perror() declaration for linux.
#ifdef linux
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif
#endif

// Upper limit on wait time for select() function:
const double t_selmax = 1e8;

// Special kludge-oriented handler for cancelling timers efficiently:
// [This could save hundreds of nanoseconds.]
static select_handler _cancel_timer_handler;
select_handler* cancel_timer_handler = &_cancel_timer_handler;

// Fast block memory allocation.
bmem_define(timer, bmem0);

/*------------------------------------------------------------------------------
If the given "delay" parameter is positive, the given packet is copied to a
udp_delay_handler object, which is registered with "sel0" for sending later.
Otherwise, the UDP packet is sent immediately.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Important note: it is assumed that this function is called just after an event
has been handled by "sel0", in which case the time parameter t() is correct.
Otherwise, the wrong time will be used!!!!
------------------------------------------------------------------------------*/
//----------------------//
//       dispatch       //
//----------------------//
int dispatch(selector& sel0, int fd, nbytes& buf, sockaddr_in& to,
             double delay) {
    int ret1 = 0;
    if (delay > 0) {    // Send the packet later:
        udp_delay_handler* pdh = new udp_delay_handler;

        // Set attributes of pdh:
        pdh->copy_pkt(fd, buf, to);

        // Register the timer event:
        pdh->delete_me = true;
        sel0.set_timer(sel0.t() + delay, pdh);
        }
    else {  // Send the packet straight away:
        // Forward transfer (hope it doesn't block!):
        ret1 = sendto(fd, buf, to);
        if (ret1 < 0) {
            cout << "dispatch(): "
                    "error while sending UDP packet." << endl;
            perror("sendto");
            }
        }
    return ret1;
    } // End of function dispatch.

//----------------------------------//
//  select_handler::select_handler  //
//----------------------------------//
select_handler::select_handler() {
    psel = 0;
    delete_me = false;
    fd = -1;
    type = sNULL;
    } // End of function select_handler::select_handler.

/*------------------------------------------------------------------------------
This handler should be called whenever the select() call on standard input
finds that there are bytes to be read.
No assumptions are made about whether stdin is non-blocking. A select() call
is made before each read() call.
------------------------------------------------------------------------------*/
//--------------------------//
//  stdin_handler::handler  //
//--------------------------//
int stdin_handler::handler() {
    // Clear out the old info:
    delete[] bytes;
    bytes = 0;
    n_bytes = 0;

    // Check sanity of fd:
    if (fd != 0)
        return -1;

    // Handle only read-events:
    if (type != sREAD)
        return -1;

    // Receive the (hopefully non-blocking) input:
    const int smallbuf_size = 512;
    char smallbuf[smallbuf_size];
    charbuf cbuf;   // Infinite buffer for collecting large input.

    for (;;) {
        // Re-poll the standard input before risking getting blocked:
        fd_set rfds0;
        FD_ZERO(&rfds0);
        FD_SET(fd, &rfds0);
        timeval timeout;
        timeval_set_zero(timeout);
        int x = select(fd + 1, &rfds0, (fd_set*)0, (fd_set*)0, &timeout);
        if (x < 0) {
            cout << "stdin_handler::handler() error calling select()" << endl;
            perror("select");
            return -1;
            }
        if (x <= 0)
            break;

        // Risk getting blocked by reading stdin:
        int len = read(fd, smallbuf, smallbuf_size);
        if (len < 0) {
            cout << "stdin_handler::handler() error calling select()" << endl;
            perror("select");
            return -1;
            }
        if (len == 0)
            break;

        // Save the bytes in the temporary buffer:
        cbuf.append_bytes(smallbuf, len);
        }

    // Copy from large buffer to new string:
    bytes = cbuf.copy();
    n_bytes = (int)cbuf.n_bytes();

    // Take the appropriate action for the new input:
    int err = action(bytes, n_bytes);

    // Return negative value to force return to selector caller:
    return (err < 0) ? -1 : 0;
    } // End of function stdin_handler::handler.

/*------------------------------------------------------------------------------
This handler should be called whenever the select() call on standard input
finds that there are bytes to be read.
No assumptions are made about whether cdev is non-blocking. A select() call
is made before each read() call.
------------------------------------------------------------------------------*/
//--------------------------//
//   cdev_handler::handler  //
//--------------------------//
int cdev_handler::handler() {
    // Clear out the old info:
    buf.clear();

    // Check sanity of fd:
//    if (fd != 0)
//        return -1;

    // Handle only read-events:
    if (type != sREAD)
        return -1;

    // Receive the (hopefully non-blocking) input:

    int len = buf.read(fd);
    if (len < 0) {
        cout << "cdev_handler::handler() error calling read()" << endl;
        perror("read");
        return -1;
        }

    // Take the appropriate action for the new input:
    int err = action();

    // Return negative value to force return to selector caller:
    return (err < 0) ? -1 : 0;
    } // End of function cdev_handler::handler.

//------------------------------//
//  udp_delay_handler::copy_pkt //
//------------------------------//
void udp_delay_handler::copy_pkt(int fd1, nbytes& buf, sockaddr_in& to) {
    fd0 = fd1;
    buf0 = buf;
    to0 = to;       // Struct copy.
    } // End of function udp_delay_handler::copy_pkt.

//------------------------------//
//  udp_delay_handler::handler  //
//------------------------------//
int udp_delay_handler::handler() {
    if (fd0 < 0 || buf0.empty())
        return 0;

    // UDP transmission (hope it doesn't block!):
    int ret1 = sendto(fd0, buf0, to0);
    if (ret1 < 0) {
        cout << "udp_delay_handler::handler: Error sending delayed UDP packet."
             << endl;
        perror("sendto");
        }

    return 0;
    } // End of function udp_delay_handler::handler.

/*------------------------------------------------------------------------------
This handler should be called whenever the select() call on a UDP socket
finds that there are bytes to be read.
------------------------------------------------------------------------------*/
//----------------------//
// udp_handler::handler //
//----------------------//
int udp_handler::handler() {
    // Check sanity of fd:
    if (fd < 0)
        return -1;

    // Handle only read-events:
    if (type != sREAD)
        return -1;

    // Receive the UDP packet on a non-blocking socket:
    int ret = buf.recvfrom(fd);

    // Check if recvfrom() had an error:
    if (ret < 0) {
        cout << "udp_handler::handler() error calling recvfrom()" << endl;
        perror("recvfrom");
        return -1;
        }

    // Check that the packet is at least an IP packet:
    if (buf.from.sa_family != AF_INET || buf.fromlen != sizeof(sockaddr_in)) {
        cout << "udp_handler::handler: received non-UDP packet." << endl;
        return -1;
        }

    // Parse out the IP address and UDP port number of the sender:
    fromhost = ntohl(((sockaddr_in*)&buf.from)->sin_addr.s_addr);
    fromport = ntohs(((sockaddr_in*)&buf.from)->sin_port);

    // Note: could at this point provide an optional cached address look-up of
    // fromhost via inet_ntoa. But maybe this would be better in akslip.[ch].

    // Take the appropriate action for the UDP packet:
    int err = action();

    // Return negative value to force return to selector caller:
    return (err < 0) ? -1 : 0;
    } // End of function udp_handler::handler.

//----------------------//
// udp_port_hand::action//
//----------------------//
int udp_port_hand::action() {
    if (trace >= 10) {
        cout << "Entered udp_port_hand::action()." << NL;
        cout << "Number of handlers = " << handlers.length() << endl;
        }

    if (!get_selector()) {
        if (trace >= 5)
            cout << "udp_port_hand::action() failed to get selector." << endl;
        return -1;
        }

    int err = -1;
    Forall(voidptr, p, handlers) {
        if (trace >= 10)
            cout << "Checking UDP handler." << endl;

        // Read a handler out of the chain of handlers:
        udp_handler* ph0 = (udp_handler*)p->i;
        if (!ph0)
            continue;   // Should never happen.

        // Copy event info from *this (very slow -- should use reference):
        ph0->set_selector(*get_selector());
        ph0->fd = fd;
        ph0->type = type;
        ph0->buf = buf;
        ph0->fromhost = fromhost;
        ph0->fromport = fromport;

        // Call the handler:
        err = ph0->action();
        if (trace >= 10)
            cout << "Received " << err << " from UDP handler." << endl;

        // Here should delete the handler if "delete_me" is set:
        // ....

        // If the event was handled okay, exit from function:
        if (err >= 0)
            break;

        // Otherwise loop around to the next handler in the chain.
        }

    return err;
    } // End of function udp_port_hand::action.

/*------------------------------------------------------------------------------
This just registers a new handler for a given local port. It is upt to the
handler to service the event if it can. Otherwise, it must indicate that the
event is not serviced, so that the next handler in the chain can handle it.
The returned udp_port_hand pointer is not supposed to be used for anything.
But a null return pointer means that the function failed.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Warning: the positivity of loc_port is not required here. If this results in the
kernel allocating the port number, then things will not be implemented as
intended. For instance, the second time you open port 0, you might not get a new
port number allocation. You would then re-use an existing port, which might not
be what you really wanted.
------------------------------------------------------------------------------*/
//----------------------//
//  udp_hand_set::open  //
//----------------------//
udp_port_hand* udp_hand_set::open(uint16 loc_port, uint32 loc_ip,
                                  udp_handler* ph0, selector& sel0) {
    if (trace >= 10)
        cout << "Entering udp_hand_set::open()." << endl;

    // Test arguments for sanity:
    if (!ph0)
        return 0;

    // Open the local port (in a cached manner):
    udp_port* p0 = port_set.open(loc_port, loc_ip);
    if (!p0 || p0->fd() < 0)
        return 0;

    if (trace >= 10) {
        cout << "udp_hand_set::open(): opened port_set:\n";
        cout << "loc_port = " << loc_port << NL;
        cout << "loc_ip   = " << loc_ip << NL;
        cout << flush;
        }

    // The file descriptor for the port:
    int fd0 = p0->fd();

    // Try to find the port in the list of handlers:
    udp_port_hand* pph0 = 0;
    forall(pph0, port_hands)
        if (pph0->p0 == p0)
            break;

    // If this local port is new, create a new object for it:
    if (!pph0) {
        if (trace >= 10)
            cout << "udp_hand_set::open() creating new udp_port_hand." << endl;
        pph0 = new udp_port_hand(p0);
        port_hands.append(pph0);
        }

    // Add the new handler to the list (sorted by "ph0", no duplicates!):
    pph0->handlers.add((void*)ph0);
    if (trace >= 10) {
        cout << "udp_hand_set::open() added udp_handler.\n";
        cout << "ph0 = 0x" << hex8(uint32(ph0)) << endl;
        }

    // Register the port with the selector:
    sel0.set_fd_mask(fd0, sREAD, pph0);

    return pph0;
    } // End of function udp_hand_set::open.

//----------------------------------//
//  tcp_handler::event_type_string  //
//----------------------------------//
const char* tcp_handler::event_type_string(tcp_event_t t) {
    switch(t) {
    case tcpNULL:
        return "null";
    case tcpCALLING:
        return "calling";
    case tcpOPEN:
        return "open";
    case tcpDATA:
        return "data";
    case tcpCLOSE:
        return "close";
        } // End of switch(t).
    return "[unknown TCP event]";
    } // End of function tcp_handler::event_type_string.

/*------------------------------------------------------------------------------
This opens the TCP port in the passive mode.
------------------------------------------------------------------------------*/
//----------------------//
//   tcp_handler::open  //
//----------------------//
int tcp_handler::open(selector& sel0,
                      uint16 loc_port, uint32 loc_ip, int backlog) {
    // If it's already open, ignore the request.
    if (fd_cntl >= 0)
        return eBAD_ARGUMENT;

    // Clear everything:
    buf.clear();
    clear_in(from);
    fromhost = 0;
    fromport = 0;
    fd_cntl = -1;
    fd_data = -1;
    tcp_open_return = 0;

    if (trace >= 10)
        cout << "tcp_handler::open: calling tcp_open()...\n";
    int fd0 = tcp_open(loc_port, loc_ip, backlog);
    if (trace >= 10)
        cout << "tcp_handler::open: tcp_open() returned " << fd0 << DOTNL;
    if (fd0 < 0) {
        cout << "tcp_handler::open: failed to open TCP port "
             << ipstring(loc_ip) << "/" << loc_port << NL;
        tcp_open_return = fd0;
        return eOPEN_FAILED;
        }

    // Set the control port fd:
    fd_cntl = fd0;

    // Register the port and this handler with the selector:
    if (trace >= 10)
        cout << "tcp_handler::open: calling sel0.set_fd_mask()...\n";
    sel0.set_fd_mask(fd0, sREAD, this);
    active = false;

    return 0;
    } // End of function tcp_handler::open.

/*------------------------------------------------------------------------------
This opens the TCP port in the active mode.
Note: this function may block for a while.
Should really do this as some sort of a separate thread [joke].
------------------------------------------------------------------------------*/
//----------------------//
//   tcp_handler::open  //
//----------------------//
int tcp_handler::open(selector& sel0, uint16 rem_port, uint32 rem_ip,
                      uint16 loc_port, uint32 loc_ip) {
    // If it's already open, ignore the request.
    if (fd_cntl >= 0)
        return -1;

    // Clear everything:
    buf.clear();
    fromhost = rem_ip;
    fromport = rem_port;
    set_in(from, fromhost, fromport);
    fd_cntl = -1;       // Not used.
    fd_data = -1;
    tcp_open_return = 0;

    if (trace >= 10)
        cout << "tcp_handler::open: calling tcp_open()...\n";
    int fd0 = -1;
    int err = tcp_open(rem_port, rem_ip, loc_port, loc_ip, fd0);
    if (trace >= 10) {
        cout << "tcp_handler::open: tcp_open() return value = " << err;
        if (err < 0)
            cout << " (" << error_str(err) << ")";
        cout << ", fd = " << fd0 << DOTNL;
        }
    if (err < 0) {
        tcp_open_return = err;
        if (err == eIN_PROGRESS) {
            connect_in_progress = true;
            event_type = tcpCALLING;

            // Must queue data until the connection is ready.
            }
        else if (err < 0) {
            cout << "tcp_handler::open: failed to open TCP conn: "
                 << ipstring(loc_ip) << "/" << loc_port << " to ";
            cout << ipstring(rem_ip) << "/" << rem_port << NL;
            return -1;
            }
        }
    else
        event_type = tcpOPEN;

    // Set the control port fd: [this is actually the data port!!!]
    fd_data = fd0;

    // Register the port and this handler with the selector:
    if (trace >= 10)
        cout << "tcp_handler::open: calling sel0.set_fd_mask()...\n";
    sel0.set_fd_mask(fd0, (sREAD | sWRITE), this);
    active = true;

    err = action();
    if (trace >= 5) {
        cout << "tcp_handler::open(): action() returned "
             << err << "." << endl;
        }

    // Return negative value to force return to selector caller:
    return (err < 0) ? -1 : 0;

//    return 0;
    } // End of function tcp_handler::open.

/*------------------------------------------------------------------------------
This handler should be called whenever the select() call on a TCP socket
finds that there are bytes to be read or some other event occurs.
------------------------------------------------------------------------------*/
//----------------------//
// tcp_handler::handler //
//----------------------//
int tcp_handler::handler() {
    if (fd < 0) {
        if (trace >= 5)
            cout << "tcp_handler::handler called with fd < 0." << endl;
        return -1;
        }
    if (!active && fd == fd_cntl) {   // The control socket.
        if (trace >= 5)
            cout << "tcp_handler::handler called with fd == fd_cntl." << endl;
        if (type != sREAD) {
            cout << "tcp_handler::handler: non-read event for fd_cntl." << endl;
            return -1;
            }

        // If this happens, then a second remote port is calling while a
        // connection is already in progress.
        if (fd_data >= 0) {
            cout << "tcp_handler::handler:"
                    " read event for fd_cntl when fd_data open.\n";
            cout << "(This is a serious error!)" << endl;
            }

        int fd0 = accept_in(fd_cntl, from);
        if (fd0 < 0) {
            cout << "tcp_handler::handler(): could not accept TCP socket.\n";
            return -1;
            }

        // Register the new data socket and this handler with the selector:
        // (...and also turn off arrival of new connections...)
        if (get_selector()) {
            get_selector()->set_fd_mask(fd0, sREAD, this);
            get_selector()->clear_fd_mask(fd_cntl, sREAD);
            }

        // Make a copy of the new data fd:
        fd_data = fd0;

        // Parse out the IP address and TCP port number of the sender:
        fromhost = ntohl(from.sin_addr.s_addr);
        fromport = ntohs(from.sin_port);

        // Note: could provide an optional cached address look-up here of
        // fromhost via inet_ntoa. But this might be better in akslip.[ch].
        // ....

        // Give any derived class a chance to take an action on event arrival:
        event_type = tcpOPEN;
        int err = action();
        if (trace >= 5) {
            cout << "tcp_handler::handler: action() returned "
                 << err << "." << endl;
            }

        // Return negative value to force return to selector caller:
        return (err < 0) ? -1 : 0;

//        return 0;
        }
    else if (fd == fd_data) {   // The data socket (active or passive).
        if (trace >= 5)
            cout << "tcp_handler::handler called with fd == fd_data." << endl;
        if (!active && fd_cntl < 0) {
            cout << "tcp_handler::handler:"
                    " read event for fd_data when fd_cntl closed." << endl;
            return -1;
            }

        // It might be a delayed TCP connect():
        if (type == sWRITE && active && connect_in_progress) {
            if (trace >= 10)
                cout << "tcp_handler::handler: calling tcp_reopen()...\n";
            int fd0 = tcp_reopen(fd_data, fromport, fromhost);
            if (trace >= 10) {
                cout << "tcp_handler::handler: tcp_reopen() returned " << fd0;
                if (fd0 < 0)
                    cout << " (" << error_str(fd0) << ")";
                cout << DOTNL;
                }
            if (fd0 == eIN_PROGRESS) {
                connect_in_progress = true;

                // Give derived class a chance to take action on event arrival:
                event_type = tcpCALLING;
                int err = action();
                if (trace >= 5) {
                    cout << "tcp_handler::handler: action() returned "
                         << err << "." << endl;
                    }

                // Must queue data until the connection is ready.
                return (err < 0) ? -1 : 0;
                }
            else if (fd0 < 0) {
                cout << "tcp_handler::handler: failed to reopen TCP conn. to "
                     << ipstring(fromhost) << "/" << fromport << endl;
                return -1;
                }
            else
                event_type = tcpOPEN;

            // Clear write-events on the control/data port fd:
            fd_data = fd0;  // Is this sensible?
            connect_in_progress = false;
            get_selector()->clear_fd_mask(fd_data, sWRITE);

            if (tx_data_wait.n_bytes() > 0) {
                // Write the saved-up bytes:
                char* pc = tx_data_wait.copy();
                int err = ::write(fd_data, pc, int(tx_data_wait.n_bytes()));
                if (trace >= 5) {
                    cout << "tcp_handler::handler(): wrote "
                         << int(tx_data_wait.n_bytes())
                         << " bytes to TCP socket.\n";
                    cout << "result from ::write() was " << err << NL;
                    }
                delete[] pc;
                if (err < 0) {
                    cout << "tcp_handler::handler(): error writing to fd_data."
                         << endl;
                    perror("write");
                    return 0;
                    }
                tx_data_wait.clear();
                }

            // Give derived class a chance to take action on event arrival:
            event_type = tcpOPEN;
            int err = action();
            if (trace >= 5) {
                cout << "tcp_handler::handler: action() returned "
                     << err << "." << endl;
                }

            return (err < 0) ? -1 : 0;
//            return 0;
            }

        // At this point, we have fd == fd_data and
        // either (type != sWRITE) or (!active) or (!connect_in_progress).
        // We should not get a write-event except when active and connecting.
        if (type != sREAD) {
            cout << "tcp_handler::handler: non-read event for fd_data." << endl;
            return -1;
            }

        // At this point, fd == fd_data and the event is a read-event.
        // Read the data:
        int n_bytes = buf.read_append(fd_data);
        if (trace >= 5) {
            cout << "tcp_handler::handler: buf.read_append() returned "
                 << n_bytes << " byte" << plural_s(n_bytes) << DOTNL;
            }
        if (n_bytes < 0) {
            cout << "tcp_handler::handler: TCP socket read error. fd_data = "
                 << fd_data << endl;
            perror("read");
            return -1;
            }

        // If the number of bytes is 0, then the socket has been closed:
        if (n_bytes == 0) {
            // Un-register the new port and this handler with the selector:
            // (...and re-register the control port...)
            if (get_selector()) {
                get_selector()->clear_fd_mask(fd_data, sREAD);
                get_selector()->set_fd_mask(fd_cntl, sREAD, this);
                }
            ::close(fd_data);
            fd_data = -1;
            clear_in(from);
            fromhost = 0;
            fromport = 0;
            if (trace >= 5)
                cout << "tcp_handler::handler: TCP connection closed." << endl;

            // Give derived class a chance to take an action on closing:
            event_type = tcpCLOSE;
            int err = action();
            if (trace >= 5) {
                cout << "tcp_handler::handler: action() returned "
                     << err << "." << endl;
                }

            // Return negative value to force return to selector caller:
            return (err < 0) ? -1 : 0;
//            return 0;
            }

        // Take the appropriate action for the TCP packet:
        event_type = tcpDATA;
        int err = action();
        if (trace >= 5) {
            cout << "tcp_handler::handler: action() returned "
                 << err << "." << endl;
            }

        // Return negative value to force return to selector caller:
        return (err < 0) ? -1 : 0;
        }

    // Case that the fd etc. are not recognised:
    if (trace >= 5) {
        cout << "tcp_handler::handler() called with fd == "
             << fd << "." << endl;
        }
    return -1;
    } // End of function tcp_handler::handler.

/*------------------------------------------------------------------------------
Return the number of bytes written successfully to the TCP port.
------------------------------------------------------------------------------*/
//----------------------//
//  tcp_handler::write  //
//----------------------//
int tcp_handler::write(const char* nbuf, int n) {
    if (trace >= 10) {
        cout << "tcp_handler::write(): number of bytes = " << n << NL;
        print(cout);
        }
    if (!nbuf || n <= 0)
        return 0;
    if (!open() || (active && connect_in_progress)) {
        tx_data_wait.append_bytes(nbuf, n);
        return 0;
        }

    int err = ::write(fd_data, nbuf, n);
    if (err < 0) {
        cout << "tcp_handler::write(): error writing to TCP port." << endl;
        perror("write");
        return 0;
        }

    // Return number of bytes written:
    return err;
    } // End of function tcp_handler::write.

/*------------------------------------------------------------------------------
Return the number of bytes written successfully to the TCP port.
------------------------------------------------------------------------------*/
//----------------------//
//  tcp_handler::write  //
//----------------------//
/*------------------------------------------------------------------------------
int tcp_handler::write(const nbytes& nbuf) {
    if (trace >= 10) {
        cout << "tcp_handler::write(): number of bytes = " << nbuf.n_bytes()
             << NL;
        print(cout);
        }
    if (nbuf.empty())
        return 0;
    if (!open() || (active && connect_in_progress)) {
        tx_data_wait.append_bytes(nbuf);
        return 0;
        }

    int err = ::write(fd_data, nbuf.bytes(), nbuf.n_bytes());
    if (err < 0) {
        cout << "tcp_handler::write(): error writing to TCP port." << endl;
        perror("write");
        return 0;
        }

    // Return number of bytes written:
    return err;
    } // End of function tcp_handler::write.
------------------------------------------------------------------------------*/

//----------------------//
//  tcp_handler::print  //
//----------------------//
void tcp_handler::print(ostream& os) {
    os << "tcp_handler state:\n";
    os << "active = " << be_string(active) << NL;
    os << "remote host/port = " << ipstring(fromhost)
         << "/" << fromport << NL;
    os << "fd_cntl = " << fd_cntl << ", fd_data = " << fd_data << NL;
    os << "connect_in_progress = " << be_string(connect_in_progress) << NL;
    os << "buf contains " << buf.n_bytes() << " bytes\n";
    os << "tx_data_wait contains " << tx_data_wait.n_bytes()
         << " bytes\n";
    } // End of function tcp_handler::print.

//--------------------------//
// tcp_contextlist::find_fd //
//--------------------------//
tcp_context* tcp_contextlist::find_fd(int fd) {
    tcp_context* tcp0 = 0;
    forall(tcp0, *this)
        if (tcp0->fd_data == fd)
            break;
    return tcp0;
    } // End of function tcp_contextlist::find_fd.

//------------------------------//
//  tcp_contextlist::find_from  //
//------------------------------//
tcp_context* tcp_contextlist::find_from(const sockaddr_in& from) {
    tcp_context* tcp0 = 0;
    forall(tcp0, *this)
        if (equal_in(tcp0->from, from))
            break;
    return tcp0;
    } // End of function tcp_contextlist::find_from.

/*------------------------------------------------------------------------------
This is (currently) the same as the tcp_handler:: function of the same name.
------------------------------------------------------------------------------*/
//----------------------------------//
// m_tcp_handler::event_type_string //
//----------------------------------//
const char* m_tcp_handler::event_type_string(tcp_event_t t) {
    switch(t) {
    case tcpNULL:
        return "null";
    case tcpCALLING:
        return "calling";
    case tcpOPEN:
        return "open";
    case tcpDATA:
        return "data";
    case tcpCLOSE:
        return "close";
        } // End of switch(t).
    return "[unknown TCP event]";
    } // End of function m_tcp_handler::event_type_string.

/*------------------------------------------------------------------------------
This opens the TCP control socket in the passive mode.
There is only 1 control socket, but there may be many data sockets.
------------------------------------------------------------------------------*/
//----------------------//
//  m_tcp_handler::open //
//----------------------//
int m_tcp_handler::open(selector& sel0,
                        uint16 loc_port, uint32 loc_ip, int backlog) {
    // If it's already open, ignore the request.
    if (fd_cntl >= 0)
        return eBAD_ARGUMENT;

    // Clear some variables in case of error-return.
    fd_cntl = -1;

    if (trace >= 10)
        cout << "m_tcp_handler::open: calling tcp_open()...\n";
    int fd0 = tcp_open(loc_port, loc_ip, backlog);
    if (trace >= 10)
        cout << "m_tcp_handler::open: tcp_open() returned " << fd0 << DOTNL;
    if (fd0 < 0) {
        cout << "m_tcp_handler::open: failed to open TCP port "
             << ipstring(loc_ip) << ":" << loc_port << NL;
        return eOPEN_FAILED;
        }

    // Set the control port fd:
    fd_cntl = fd0;

    // Register the port and this handler with the selector:
    if (trace >= 10)
        cout << "m_tcp_handler::open: calling sel0.set_fd_mask()...\n";
    sel0.set_fd_mask(fd0, sREAD, this);

    return 0;
    } // End of function m_tcp_handler::open.

/*------------------------------------------------------------------------------
This handler is called whenever the select() call on a TCP socket finds that
there are bytes to be read or some other event occurs.
Unlike the single data-socket version, this multiple data-socket handler must
use the "from" address for a data-socket to retrieve the context descriptor for
the connection.
------------------------------------------------------------------------------*/
//--------------------------//
//  m_tcp_handler::handler  //
//--------------------------//
int m_tcp_handler::handler() {
    if (fd < 0) {
        if (trace >= 5)
            cout << "m_tcp_handler::handler called with fd < 0." << endl;
        return -1;
        }

    if (fd == fd_cntl) { // The event is for the control socket.
        if (trace >= 5)
            cout << "m_tcp_handler::handler called with fd == fd_cntl." << endl;
        if (type != sREAD) {
            cout << "m_tcp_handler::handler:"
                    " non-read event for control socket." << endl;
            return -1;
            }

        // A read-event on the control socket means "new connection".
        int fd0 = accept_in(fd_cntl, from);
        if (fd0 < 0) {
            cout << "m_tcp_handler::handler(): could not accept TCP socket.\n";
            return -1;
            }

        // Register the new data socket and this handler with the selector.
        // (...and don't turn off arrival of new connections...)
        if (get_selector()) { // Should always be non-zero.
            get_selector()->set_fd_mask(fd0, sREAD, this);
            }

        // Parse the IP address and TCP port number from the sender.
        fromhost = ntohl(from.sin_addr.s_addr);
        fromport = ntohs(from.sin_port);

        // Find out if the connection is already in the active list.
        tcp_context* tcp0 = tcp_contexts.find_fd(fd0);
        if (tcp0) { // Should never happen.
            cout << "m_tcp_handler::handler warning: re-used fd.\n";
            }

        tcp0 = tcp_contexts.find_from(from);
        if (tcp0) { // Should never happen.
            cout << "m_tcp_handler::handler warning: re-used from-address.\n";
            }

        // Create a new connection context descriptor.
        tcp_context* tcp1 = new tcp_context;

        // Record the fd, IP address and TCP port number of the connection.
        tcp1->fd_data = fd0;
        tcp1->fromhost = fromhost;
        tcp1->fromport = fromport;

        // Add the context to the active-list.
        tcp_contexts.append(tcp1);

        // Make the current context available to action().
        context = tcp1;

        // Note: could provide an optional cached address look-up here of
        // fromhost via inet_ntoa. But this might be better in akslip.[ch].
        // ....

        // Give any derived class a chance to take an action on event arrival.
        event_type = tcpOPEN;
        int err = action();
        if (trace >= 5) {
            cout << "m_tcp_handler::handler: action() returned "
                 << err << "." << endl;
            }

        // Return negative value to force return to selector caller.
        return (err < 0) ? -1 : 0;

//        return 0;
        }

    // Now dealing with a data socket, hopefully.
    if (trace >= 5)
        cout << "m_tcp_handler::handler called with fd != fd_cntl." << endl;

    // Find out if the TCP connection is already in the active list.
    tcp_context* tcp0 = tcp_contexts.find_fd(fd);
    if (!tcp0) { // Should never happen.
        cout << "m_tcp_handler::handler warning:"
                " no fd found for read-event.\n";
        return -1;
        }

    if (fd_cntl < 0) { // Should never happen.
        cout << "m_tcp_handler::handler:"
                " read event for fd_data when fd_cntl closed." << endl;
        return -1;
        }

    // We should not get a write-event.
    if (type != sREAD) {
        cout << "m_tcp_handler::handler: non-read-event for fd_data."
             << endl;
        return -1;
        }

    // At this point, fd == fd_data and the event is a read-event.
    // Read the data.
    int n_bytes = tcp0->buf.read_append(tcp0->fd_data);
    if (trace >= 5) {
        cout << "m_tcp_handler::handler: buf.read_append() returned "
             << n_bytes << " byte" << plural_s(n_bytes) << DOTNL;
        }
    if (n_bytes < 0) {
        cout << "m_tcp_handler::handler: TCP socket read error." << endl;
        perror("read");
        return -1;
        }

    // Make the current context available to action().
    context = tcp0;

    // If the number of bytes is 0, then the socket has been closed:
    if (n_bytes == 0) {
        // Un-register the new port and this handler with the selector:
        // (...and re-register the control port...)
        if (get_selector()) {
            get_selector()->clear_fd_mask(tcp0->fd_data, sREAD);
            }

        // Give derived class a chance to take an action on closing:
        event_type = tcpCLOSE;
        int err = action();
        if (trace >= 5) {
            cout << "m_tcp_handler::handler: action() returned "
                 << err << "." << endl;
            }

        ::close(tcp0->fd_data);
        tcp_contexts.remove(tcp0);
        if (trace >= 5)
            cout << "m_tcp_handler::handler:"
                    " TCP connection closed." << endl;

        // Return negative value to force return to selector caller:
        return (err < 0) ? -1 : 0;
//        return 0;
        }

    // Take the appropriate action for the TCP packet:
    event_type = tcpDATA;
    int err = action();
    if (trace >= 5) {
        cout << "m_tcp_handler::handler: action() returned "
             << err << "." << endl;
        }

    // Return negative value to force return to selector caller:
    return (err < 0) ? -1 : 0;
    } // End of function m_tcp_handler::handler.

//----------------------//
// m_tcp_handler::print //
//----------------------//
void m_tcp_handler::print(ostream& os) {
    os << "m_tcp_handler state:\n";
    os << "current remote host/port = " << ipstring(fromhost)
         << "/" << fromport << NL;
    os << "fd_cntl = " << fd_cntl << NL;
//    os << "connect_in_progress = " << be_string(connect_in_progress) << NL;
//    os << "buf contains " << buf.n_bytes() << " bytes\n";
//    os << "tx_data_wait contains " << tx_data_wait.n_bytes()
//         << " bytes\n";
    } // End of function m_tcp_handler::print.

//----------------------//
//     fdtype::print    //
//----------------------//
void fdtype::print(ostream& os) {
    os << "fd = " << fd << ". handlers = ";
    bool_enum found = false;
    if (types & sREAD) {
        os << "READ (" << (ulong)r_handler << ")";
        found = true;
        }
    if (types & sWRITE) {
        if (found)
            os << ", ";
        os << "WRITE (" << (ulong)w_handler << ")";
        found = true;
        }
    if (types & sERROR) {
        if (found)
            os << ", ";
        os << "ERROR (" << (ulong)e_handler << ")";
        found = true;
        }
    if (!found)
        os << "[no handler-flags set]";
    os << NL;
    } // End of function fdtype::print.

//----------------------//
//  selector::selector  //
//----------------------//
selector::selector() {
    // By default, no mask bits are set:
    max_fd = -1;
    FD_ZERO(&read_mask);
    FD_ZERO(&write_mask);
    FD_ZERO(&error_mask);

    // The default wait is forever. I.e. wait until there are events.
    wait_time = 0;
    wait_forever = true;
    ie_handler = 0;

    // Temporaries, which are set to the above masks and timeout value
    // each time before select() is called:
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    FD_ZERO(&efds);
    timeval_set_zero(tv_return);
    t_return = 0;
    timeval_set_zero(timeout);

    // The round robin fd list is initially empty:
    n_fds = 0;
    last_fd = 0;
    last_cat = 0;

    // Return values from select() call:
    select_return = 0;
    select_errno = 0;
    trace = 0;
    } // End of function selector::selector.

//----------------------//
//    selector::print   //
//----------------------//
void selector::print(ostream& os) {
    os << "max_fd = " << max_fd << NL;
    os << "waiting time = ";
    if (wait_forever)
        os << "forever";
    else
        os << wait_time;
    os << NL;

    os << "number of fds monitored = " << n_fds << NL;
    os << "fd monitoring settings are:\n";
    for (int i = 0; i < n_fds; ++i)
        fdlist[i].print(os);
    os << "last fd monitored (in round robin order) = " << last_fd << NL;
    os << "last category monitored (in round robin order) = "
       << last_cat << " (";
    switch (last_cat) {
    case 0:
        os << "read";
        break;
    case 1:
        os << "write";
        break;
    case 2:
        os << "error";
        break;
        } // End of switch.
    os << ")\n";
    } // End of function selector::print.

/*------------------------------------------------------------------------------
Add a timer handler to the sorted heap of timer handlers.
The parameter x is the number of Unix seconds since 1 Jan 1970.
There is intentionally no check that the event is not "in the past".
------------------------------------------------------------------------------*/
//----------------------//
//  selector::set_timer //
//----------------------//
const void* selector::set_timer(double x, select_handler* psh) {
    timer* pt = new timer(x);
    pt->t_handler = psh;
    timers.insert(pt);
    return pt;
    } // End of function selector::set_timer.

/*------------------------------------------------------------------------------
There is intentionally no check that the event is not "in the past".
------------------------------------------------------------------------------*/
//--------------------------//
// selector::set_timer_rel  //
//--------------------------//
const void* selector::set_timer_rel(double dx, select_handler* psh) {
    // Find out the current time:
    timeval tv0;
    gettime(tv0);

    // Register a event for dx seconds in the future.
    double t1 = timeval_get(tv0) + dx;
    return set_timer(t1, psh);
    } // End of function selector::set_timer_rel.

//--------------------------//
//  selector::cancel_timer  //
//--------------------------//
void selector::cancel_timer(const void* pt0) {
    if (!pt0)
        return;
    timer_heap_traversal tt(timers);

    for (;;) {
        timer* pt = tt.next();
        if (!pt)
            break;
        if ((const void*)pt == pt0) {
            pt->cancel();
            break;
            }
        }
    } // End of function selector::cancel_timer.

/*------------------------------------------------------------------------------
This function registers a handler (possibly null) for the "inter-event timeout"
event. This is the event that no events (timer or I/O events) occur for a given
length of time.
------------------------------------------------------------------------------*/
//--------------------------//
//  selector::set_wait_time //
//--------------------------//
void selector::set_wait_time(double dx, select_handler* psh) {
    wait_time = (dx >= 0) ? dx : 0;

    // Clear the over-riding wait-forever field:
    wait_forever = false;

    // Register the handler for this event:
    ie_handler = psh;
    } // End of function selector::set_wait_time.

/*------------------------------------------------------------------------------
This registers a new fd with the selector.
This causes the mask to be set for the corresponding fd.
The "type" parameter is a 3-bit mask indicating the set of events to
be monitored.
The "psh" argument is used as the handler for all specified events.
To specify separate handlers for each event, this function must be called
once for each handler. If the null select_handler is specified, then
this is used, and this will cause the get_event call to always return
when it gets this event.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Return values:
0   success
-1  fd0 argument is out of range
-2  error in "types" bit-mask argument
-3  total number of fds set is already at the maximum
------------------------------------------------------------------------------*/
//--------------------------//
//  selector::set_fd_mask   //
//--------------------------//
int selector::set_fd_mask(int fd0, int types, select_handler* psh) {
    // Check parameter sanity:
#ifndef WIN32
    if (fd0 < 0 || fd0 >= FD_SETSIZE)
#else
    if (fd0 < 0 || fd0 >= INVALID_SOCKET)
#endif
        return -1;

    // Reject bit-mask bits outside the 3 permitted positions:
    if (types & ~(sREAD | sWRITE | sERROR))
        return -2;

    // Add the new event(s) to the round robin list:
    // First try to find the fd value in the list:
    int i = 0;
    for (i = 0; i < n_fds; ++i)
        if (fdlist[i].fd == (uint16)fd0)
            break;

    // If the fd is already there, OR in the new types:
    if (i < n_fds) {
        fdlist[i].types |= types;
        }
    // If it's not there, start a new record. (This should never overflow.)
    else {
        // Check to see if the number of fds is greater than the
        // fdlist array size. This could happen in WIN32 environment
        // because fds can be greater than FD_SETSIZE !!
        if (n_fds >= FD_SETSIZE)
            return -3;
        fdlist[i].fd = (uint16)fd0;
        fdlist[i].types = types;
        n_fds += 1;
        }

    // Set the new event types:
    if (types & sREAD) {
        fdlist[i].r_handler = psh;
        FD_SET(fd0, &read_mask);
        }
    if (types & sWRITE) {
        fdlist[i].w_handler = psh;
        FD_SET(fd0, &write_mask);
        }
    if (types & sERROR) {
        fdlist[i].e_handler = psh;
        FD_SET(fd0, &error_mask);
        }

    // Update the upper limit of the set bits in the masks:
    if (fd0 > max_fd)
        max_fd = fd0;

    return 0;
    } // End of function selector::set_fd_mask.

/*------------------------------------------------------------------------------
This clears events from the selector.
The "types" parameter is a 3-bit mask indicating the set of event types to
be cleared.
------------------------------------------------------------------------------*/
//--------------------------//
// selector::clear_fd_mask  //
//--------------------------//
int selector::clear_fd_mask(int fd0, int types) {
    // Check parameter sanity:
#ifndef WIN32
    if (fd0 < 0 || fd0 >= FD_SETSIZE)
#else
    if (fd0 < 0 || fd0 >= INVALID_SOCKET)
#endif
        return -1;

    // Reject bit-mask bits outside the 3 permitted positions:
    if (types & ~(sREAD | sWRITE | sERROR))
        return -2;

    // Find the fd value in the round robin list:
    int i = 0;
    for (i = 0; i < n_fds; ++i)
        if (fdlist[i].fd == (uint16)fd0)
            break;

    // If it's not there, ignore it (it probably isn't in the masks...):
    if (i >= n_fds)
        return -3;

    // Clear the event types mask:
    fdlist[i].types &= ~types;

    // Clear the event types:
    if (types & sREAD) {
        fdlist[i].r_handler = 0;
#ifndef WIN32
        FD_CLR(fd0, &read_mask);
#else
        FD_CLR((unsigned int)fd0, &read_mask);
#endif
        }
    if (types & sWRITE) {
        fdlist[i].w_handler = 0;
#ifndef WIN32
        FD_CLR(fd0, &write_mask);
#else
        FD_CLR((unsigned int)fd0, &write_mask);
#endif
        }
    if (types & sERROR) {
        fdlist[i].e_handler = 0;
#ifndef WIN32
        FD_CLR(fd0, &error_mask);
#else
        FD_CLR((unsigned int)fd0, &error_mask);
#endif
        }

    return 0;
    } // End of function selector::clear_fd_mask.

/*------------------------------------------------------------------------------
This function goes into a wait state until an event occurs.
The handler object appropriate to each event is called, and if a negative
value is returned by a handler, the get_event function returns a negative
value. A negative value is also returned if any error occurs.
A zero value is returned if a timeout occurs.
Otherwise, any select-event results in the appropriate handler being called.
The two values "select_return" and "select_errno" are also returned when a
return occurs.
If there is a timeout, select_return is set to 0. If select() returns an error
code, it is stored in select_errno. Otherwise, select_errno is set to zero.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Actions for each event type:
select() function call error:
    - set select_errno to errno
    - return -1
timeout occurs with no I/O events:
    - set select_errno to 0
    - return 0
one or more I/O events occur:
    - choose one of the I/O events, using a round robin choice algorithm.
    - for the chosen I/O event:
        - if the event is being monitored, but there is no handler:
            - return 0.
        - if the event is being monitored and does have a handler:
            - set the following members of the handler object:
                - file descriptor
                - I/O event type
                - pointers to the complete event masks
            - call the handler
            - if the return value is negative, return the negative value.
            - if the return value is non-negative, wait for next event

    - if no events are found, this is an error in the select() function;
      so return -1.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The possible conditions upon return are:
Return value    select_return   condition(s)
-1              < 0             error in select() call [then select_errno < 0]
-1              0               some other sort of error
0               0               timeout
0               > 0             I/O event occurs, but has no event handler
< 0             > 0             event handler returned negative value

The function caller may also read the member variables rfds, wfds and efds
to see how the event masks were set by select().
------------------------------------------------------------------------------*/
//----------------------//
//  selector::get_event //
//----------------------//
int selector::get_event() {
    for (;;) {
        // Can't assume that "timeout" won't be modified:
        // (The manual says that it might be volatile in the future.)
        timeval* ptv = 0;
        if (!wait_forever) {
            // Impose maximum bound on timeout.
            timeval_set(timeout, min2(wait_time, t_selmax));
            ptv = &timeout;
            }

        // See if there are any timers to go off before the timeout:
        while (timers.first()) {
            // Find the time for the earliest timer:
            double t1 = timers.first()->t;

            // Find the current time:
            gettime(tv_return);
            t_return = timeval_get(tv_return);

            // Find the time to wait for the first timer:
            double dt = t1 - t_return;

            // If the timer has not gone off, book the event for the future:
            if (dt > 0) {
                if (wait_forever || dt < wait_time) {
                    timeval_set(timeout, min2(dt, t_selmax));
                    ptv = &timeout;
                    }
                break;
                }

            // If the timer _has_ already gone off, dequeue and invoke it:
            timer* pt = timers.popfirst();
            if (!pt)        // Safety check. Should never happen.
                break;

            // Clear the bit masks for the handler:
            FD_ZERO(&rfds);
            FD_ZERO(&wfds);
            FD_ZERO(&efds);

            select_handler* psh = pt->t_handler;
            delete pt;

            // If the timer has been cancelled, ignore it:
            if (psh == cancel_timer_handler)
                continue;

            // If no handler is registered, return to caller:
            if (!psh) {
                if (trace >= 10) {
                    cout << flush;
                    cerr << "selector: No handler for timer event.\n";
                    }
                return 0;
                }

            // Set members of handler so that it knows what happened:
            psh->psel = this;
            psh->fd = -1;
            psh->type = sNULL;

            // Call the handler:
            int err = psh->handler();
            if (psh->delete_me)     // User must ensure only one copy in heap.
                delete psh;
            if (err < 0) {
                if (trace >= 10) {
                    cout << flush;
                    cerr << "selector: Handler returned negative value.\n";
                    }
                return err;
                }
            }

        // Set the volatile select() arguments to the specified values:
        rfds = read_mask;
        wfds = write_mask;
        efds = error_mask;

        // Fetch the next event:
        select_return = select(max_fd + 1, &rfds, &wfds, &efds, ptv);

        if (trace >= 10) {
            cout << flush;
            cerr << "selector: select() returned " << select_return << NL;
            }

        // Case that an error occurs (very rare, not expected):
        if (select_return < 0) {
            select_errno = errno;
            cout << "Error while calling select() in selector::get_event()."
                 << endl;
            perror("select");

            // Note: Could possibly define a handler for this (rare) event!
            // Then it would not be necessary to return.
            return -1;
            }

        // No error has occured:
        select_errno = 0;

        // Find out what time it is:
        gettime(tv_return);
        t_return = timeval_get(tv_return);

        // Find out if any timers went off:
        bool_enum called_timer = false;
        while (timers.first()) {
            // If the earliest timer isn't ready to go, proceed to I/O events.
            if (timers.first()->t > t_return)
                break;

            // If it's ready, dequeue it.
            timer* pt = timers.popfirst();
            if (!pt)        // Should never happen.
                break;

            // If no handler is registered, return to caller.
            select_handler* psh = pt->t_handler;
            delete pt;

            // If the timer has been cancelled, ignore it.
            if (psh == cancel_timer_handler)
                continue;

            // If no handler is registered, return to caller.
            if (!psh) {
                if (trace >= 10) {
                    cout << flush;
                    cerr << "selector: No handler for timer event.\n";
                    }
                return 0;
                }

            // Set members of handler so that it knows what happened.
            psh->psel = this;
            psh->fd = -1;
            psh->type = sNULL;

            // Call the handler.
            int err = psh->handler();
            if (psh->delete_me)     // User must ensure only one copy in heap.
                delete psh;
            if (err < 0) {
                if (trace >= 10) {
                    cout << flush;
                    cerr << "selector: Handler returned negative value.\n";
                    }
                return err;
                }
            // Set condition for continuation of outer for-loop.
            called_timer = true;

            // If a timer handler has been called, update the clock reading.
            // (This avoids a live-lock situation where a timer handler
            // keeps requeueing itself with an "in the past" invocation time,
            // which never changes.)
            gettime(tv_return);
            t_return = timeval_get(tv_return);
            }

        // If a timer has been called, one or more I/O events may be cleared.
        // So don't risk getting blocked in one of the I/O handlers:
        if (called_timer)
            continue;

        // If there are zero I/O events, an inter-event timeout has occured.
        if (select_return == 0) {
            // [Check here whether an inter-event timeout occurred.]
            // ....

            continue;
            }

        // At this point, it is known that there are one or more I/O events.
        // Only one event is handled before calling select() again. This is
        // because an event handler for one event may clear one or more
        // other events anyway. This is especially likely if more than one
        // event category occurs for a single fd.

        // Check for impossible case that there are no registered events:
        if (n_fds <= 0) {   // This should never happen.
            cout << flush;
            cerr << "selector::get_event() error: Impossible n_fds <= 0.\n";
            return -1;
            }

        // Find out which event occured (in round robin fd/cat order):
        int old_last_fd = last_fd;
        int old_last_cat = last_cat;
        bool_enum found = false;
        for (;;) {
            // Increment last_cat and maybe last_fd:
            last_cat += 1;
            if (last_cat >= 3) {
                last_cat = 0;
                last_fd += 1;
                if (last_fd >= n_fds)
                    last_fd = 0;
                }
            int fd0 = fdlist[last_fd].fd;

            // Check the returned mask for this fd and event category:
            switch (last_cat) {
            case 0:
                // Is there a read condition?
                if ((fdlist[last_fd].types & sREAD) && FD_ISSET(fd0, &rfds)) {
                    // If there's no handler, return:
                    select_handler* psh = fdlist[last_fd].r_handler;
                    if (!psh) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: No handler for read-event.\n";
                            }
                        return 0;
                        }
                    // Call the handler:
                    psh->psel = this;
                    psh->fd = fd0;
                    psh->type = sREAD;
                    int err = psh->handler();

                    // If the handler returns an error, return:
                    if (err < 0) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: Read-handler negative value.\n";
                            }
                        return err;
                        }
                    // Otherwise, go back for another select() call:
                    found = true;
                    }
                break;
            case 1:
                // Is there a write condition?
                if ((fdlist[last_fd].types & sWRITE) && FD_ISSET(fd0, &wfds)) {
                    // If there's no handler, return:
                    select_handler* psh = fdlist[last_fd].w_handler;
                    if (!psh) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: No handler for write-event.\n";
                            }
                        return 0;
                        }
                    // Call the handler:
                    psh->psel = this;
                    psh->fd = fd0;
                    psh->type = sWRITE;
                    int err = psh->handler();

                    // If the handler returns an error, return:
                    if (err < 0) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: Read-handler negative value.\n";
                            }
                        return err;
                        }
                    // Otherwise, go back for another select() call:
                    found = true;
                    }
                break;
            case 2:
                // Is there an error condition?
                if ((fdlist[last_fd].types & sERROR) && FD_ISSET(fd0, &efds)) {
                    // If there's no handler, return:
                    select_handler* psh = fdlist[last_fd].e_handler;
                    if (!psh) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: No handler for error-event.\n";
                            }
                        return 0;
                        }
                    // Call the handler:
                    psh->psel = this;
                    psh->fd = fd0;
                    psh->type = sERROR;
                    int err = psh->handler();

                    // If the handler returns an error, return:
                    if (err < 0) {
                        if (trace >= 10) {
                            cout << flush;
                            cerr << "selector: Read-handler negative value.\n";
                            }
                        return -1;
                        }
                    // Otherwise, go back for another select() call:
                    found = true;
                    }
                break;
                } // End of switch(last_cat).

            if (found)
                break;

            // Give up if all fds and categories have been checked:
            if (last_fd == old_last_fd && last_cat == old_last_cat)
                break;
            }

        // If there is no matching fd, return to the caller:
        // [This should never happen.]
        if (!found) {
            if (trace >= 10) {
                cout << flush;
                cerr << "selector: No matching fd!! [Should never happen.]\n";
                }
            return -1;
            }
        // Otherwise loop around for the next select() call.
        } // End of for(;;) loop.

    // Statement never reached:
//    return 0;
    } // End of function selector::get_event.
