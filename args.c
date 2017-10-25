// src/aksl/args.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Command line interpretation software.
Note: should also provide a generic function which takes a valuelist as a
parameter, and fills in the values from the command line.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Functions in this file:

get_arg0
get_arg0
get_arg
get_arg3
get_arg
get_arg
get_arg
get_arg
get_arg
get_arg
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/args.h"

using namespace std;

// System header files:
#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_STDLIB_H
#define AKSL_X_STDLIB_H
#include <stdlib.h>
#endif

/*------------------------------------------------------------------------------
This version just reads a single argument. No parameter indicator is assumed.
------------------------------------------------------------------------------*/
//----------------------//
//       get_arg0       //
//----------------------//
bool_enum get_arg0(int argc, char** argv, long& result) {
    if (argc <= 0 || !argv) { // Ran out of arguments.
        cout << flush;
        cerr << "\nNo arguments.\n";
        return false;
        }
    char* arg = *argv;
    char* ptr = arg;
    long ltmp = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cout << flush;
        cerr << "\nError in parameter.\n";
        return false;
        }
    result = ltmp;
    return true;
    } // End of function get_arg0.

/*------------------------------------------------------------------------------
This version just reads a single argument. No parameter indicator is assumed.
------------------------------------------------------------------------------*/
//----------------------//
//       get_arg0       //
//----------------------//
bool_enum get_arg0(int argc, char** argv, char*& result) {
    if (argc <= 0 || !argv) { // Ran out of arguments.
        cout << flush;
        cerr << "\nNo arguments.\n";
        return false;
        }
    char* arg = *argv;
    int len = strlen(arg);
    char* ptr = new char[len + 1];
    strcpy(ptr, arg);
    result = ptr;
    return true;
    } // End of function get_arg0.

//----------------------//
//       get_arg        //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, long& result) {
    // Repeat things which have probably already been done:
    char* arg = *argv;
    int len = strlen(arg);
    char c0 = arg[0];
    char c1 = arg[1];

    if (len <= 2) {
        if (--argc > 0)
            arg = *++argv;
        else { // Ran out of arguments.
//            help(cerr);
            cerr << "\nNo parameter given for option " << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = arg;
    long ltmp = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
//        help(cerr);
        cerr << "\nError in parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    result = ltmp;
    return true;
    } // End of function get_arg.

//----------------------//
//       get_arg3       //
//----------------------//
bool_enum get_arg3(int& argc, char**& argv, long& result) {
    // Repeat things which have probably already been done:
    char* arg = *argv;
    char c0 = arg[0];
    char c1 = arg[1];
    char c2 = arg[2];

    int len = strlen(arg);
    if (len <= 3) {
        if (--argc > 0)
            arg = *++argv;
        else { // Ran out of arguments.
            cerr << "\nNo parameter given for option "
                 << c0 << c1 << c2 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 3;
        }
    char* ptr = arg;
    long ltmp = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cerr << "\nError in parameter to option " << c0 << c1 << c2 << ".\n";
        return false;
        }
    result = ltmp;
    return true;
    } // End of function get_arg3.

/*------------------------------------------------------------------------------
Interpret a command line portion of the form
    -x <arg1> <arg2>
or
    -x<arg1> <arg2>
where arg1 is a long integer and arg2 is a long integer.
This functions expects to be called with argv pointing to the command line
argument in which the "-x" was found, and argc equal to the number of command
line arguments left in the array argv. Both argc and argv are updated as the
arguments are read, so that they point to the second argument if the reading
is successful, otherwise they are unaltered or else point to the argument
containing the first character after the "-x".
Any two-character string may be used in place of "-x".
The return value is "true" if two arguments are successfully read. Otherwise the
return value is "false", and the values of "res1" and "res2" are left unaltered.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, long& res1, long& res2) {
    if (argc < 2 || !argv || !argv[0] || !argv[1])
        return false;

    // Repeat things which have probably already been done:
    char* arg = argv[0];
    if (!arg[0] || !arg[1])
        return false;
    char c0 = arg[0];
    char c1 = arg[1];

    if (!arg[2]) {
        if (argc >= 3 && argv[2]) {
            --argc;
            arg = *++argv;
            }
        else { // Ran out of arguments.
            cerr << "\nInsufficient parameters for option "
                 << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = arg;
    long ltmp1 = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cerr << "\nError in first parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    arg = argv[1];
    ptr = arg;
    long ltmp2 = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cerr << "\nError in second parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    res1 = ltmp1;
    res2 = ltmp2;
    --argc;
    ++argv;
    return true;
    } // End of function get_arg.

/*------------------------------------------------------------------------------
Interpret a command line portion of the form
    -x <arg1> <arg2>
or
    -x<arg1> <arg2>
where arg1 is a long integer and arg2 is a double float.
This functions expects to be called with argv pointing to the command line
argument in which the "-x" was found, and argc equal to the number of command
line arguments left in the array argv. Both argc and argv are updated as the
arguments are read, so that they point to the second argument if the reading
is successful, otherwise they are unaltered or else point to the argument
containing the first character after the "-x".
Any two-character string may be used in place of "-x".
The return value is "true" if two arguments are successfully read. Otherwise the
return value is "false", and the values of "res1" and "res2" are left unaltered.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, long& res1, double& res2) {
    if (argc < 2 || !argv || !argv[0] || !argv[1])
        return false;

    // Repeat things which have probably already been done:
    char* arg = argv[0];
    if (!arg[0] || !arg[1])
        return false;
    char c0 = arg[0];
    char c1 = arg[1];

    if (!arg[2]) {
        if (argc >= 3 && argv[2]) {
            --argc;
            arg = *++argv;
            }
        else { // Ran out of arguments.
            cerr << "\nInsufficient parameters for option "
                 << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = arg;
    long tmp1 = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cerr << "\nError in first parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    arg = argv[1];
    ptr = arg;
    double tmp2 = strtod(arg, &ptr);
    if (ptr == arg || *ptr) {
        cerr << "\nError in second parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    res1 = tmp1;
    res2 = tmp2;
    --argc;
    ++argv;
    return true;
    } // End of function get_arg.

/*------------------------------------------------------------------------------
This function gets a double parameter to the option *argv[0] *argv[1]. That is,
it is assumed that the first two characters of *argv are of the form "-c", or
some such option specifier. And then a double parameter for this option is to be
found. If all is okay, d is set to the parameter, and "true" is returned.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, double& result) {
    // Repeat things which have probably already been done:
    char* arg = *argv;
    int len = strlen(arg);
    char c0 = arg[0];
    char c1 = arg[1];

    if (len <= 2) {
        if (--argc > 0)
            arg = *++argv;
        else { // Ran out of arguments.
//            help(cerr);
            cerr << "\nNo parameter given for option " << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = arg;
    double dtmp = strtod(arg, &ptr);
    if (ptr == arg || *ptr) {
//        help(cerr);
        cerr << "\nError in parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    result = dtmp;
    return true;
    } // End of function get_arg.

/*------------------------------------------------------------------------------
This function gets a string parameter to the option *argv[0] *argv[1]. That is,
it is assumed that the first two characters of *argv are of the form "-c", or
some such option specifier. And then a string parameter for this option is to be
found. If all is okay, "result" is set to the parameter, and "true" is returned.
Otherwise "result" is not altered.
The result string is heap-allocated.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, char*& result) {
    // Repeat things which have probably already been done:
    char* arg = *argv;
    int len = strlen(arg);
    char c0 = arg[0];
    char c1 = arg[1];

    if (len <= 2) {
        if (--argc <= 0) {
            // Ran out of arguments.
            cerr << "\nNo parameter given for option " << c0 << c1 << ".\n";
            return false;
            }
        arg = *++argv;
        len = strlen(arg);
        }
    else { // No space between option and parameter.
        arg += 2;
        len -= 2;
        }
    char* ptr = new char[len + 1];
    strcpy(ptr, arg);
    result = ptr;
    return true;
    } // End of function get_arg.

/*------------------------------------------------------------------------------
This function gets a string argument and an integer argument to the option
*argv[0] *argv[1]. That is, it is assumed that the first two characters of *argv
are of the form "-c", or some such option specifier. etc. etc.
The result1 string is heap-allocated.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, char*& result1, long& result2) {
    if (argc < 2 || !argv || !argv[0] || !argv[1])
        return false;

    // Repeat things which have probably already been done:
    char* arg = argv[0];
    if (!arg[0] || !arg[1])
        return false;
    char c0 = arg[0];
    char c1 = arg[1];

    if (!arg[2]) {
        if (argc >= 3 && argv[2]) {
            --argc;
            arg = *++argv;
            }
        else { // Ran out of arguments.
            cerr << "\nInsufficient parameters for option "
                 << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = new char[strlen(arg) + 1];
    strcpy(ptr, arg);
    result1 = ptr;

    arg = argv[1];
    ptr = arg;
    long ltmp = strtol(arg, &ptr, (int)0);
    if (ptr == arg || *ptr) {
        cerr << "\nError in parameter to option " << c0 << c1 << ".\n";
        return false;
        }
    result2 = ltmp;

    --argc;
    ++argv;
    return true;
    } // End of function get_arg.

/*------------------------------------------------------------------------------
This function gets two string arguments to the option *argv[0] *argv[1]. That
is, it is assumed that the first two characters of *argv are of the form "-c",
or some such option specifier. And then two string arguments for this option is
to be found. If all is okay, "result1" and "result2" are set to the arguments,
and "true" is returned. Otherwise "result1" and "result2" are not altered.
The result strings are heap-allocated.
------------------------------------------------------------------------------*/
//----------------------//
//        get_arg       //
//----------------------//
bool_enum get_arg(int& argc, char**& argv, char*& result1, char*& result2) {
    if (argc < 2 || !argv || !argv[0] || !argv[1])
        return false;

    // Repeat things which have probably already been done:
    char* arg = argv[0];
    if (!arg[0] || !arg[1])
        return false;
    char c0 = arg[0];
    char c1 = arg[1];

    if (!arg[2]) {
        if (argc >= 3 && argv[2]) {
            --argc;
            arg = *++argv;
            }
        else { // Ran out of arguments.
            cerr << "\nInsufficient parameters for option "
                 << c0 << c1 << ".\n";
            return false;
            }
        }
    else { // No space between option and parameter.
        arg += 2;
        }
    char* ptr = new char[strlen(arg) + 1];
    strcpy(ptr, arg);
    result1 = ptr;

    arg = argv[1];
    ptr = new char[strlen(arg) + 1];
    strcpy(ptr, arg);
    result2 = ptr;

    --argc;
    ++argv;
    return true;
    } // End of function get_arg.
