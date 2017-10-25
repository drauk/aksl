// src/aksl/args.h   2017-10-25   Alan U. Kennington.
// $Id: src/aksl/include/aksl/args.h be54be8f6d 2017-10-25 10:29:45Z Alan U. Kennington $
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_ARGS_H
#define AKSL_ARGS_H
/*------------------------------------------------------------------------------
Classes in this file:

------------------------------------------------------------------------------*/

#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// Command line reading functions, for function main():
extern bool_enum get_arg0(int argc, char** argv, long&);
extern bool_enum get_arg0(int argc, char** argv, char*&);
extern bool_enum get_arg(int& argc, char**& argv, long&);
extern bool_enum get_arg3(int& argc, char**& argv, long&);
extern bool_enum get_arg(int& argc, char**& argv, long&, long&);
extern bool_enum get_arg(int& argc, char**& argv, long&, double&);
extern bool_enum get_arg(int& argc, char**& argv, double&);
extern bool_enum get_arg(int& argc, char**& argv, char*&);
extern bool_enum get_arg(int& argc, char**& argv, char*&, long&);
extern bool_enum get_arg(int& argc, char**& argv, char*&, char*&);

#endif /* AKSL_ARGS_H */
