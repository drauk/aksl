// src/aksl/form.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_FORM_H
#define AKSL_FORM_H
/*------------------------------------------------------------------------------
This module contains general output formatting things.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes in this file:

------------------------------------------------------------------------------*/

// AKSL header files:
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

#ifdef HAVE_VPRINTF
extern char* vform(const char*, ...);
#endif /* HAVE_VPRINTF */

#define EXPT_FORM_STRING 0
#if EXPT_FORM_STRING
extern char* form_string(const char*, ...);
// extern const char* vformstring(const char*, value*);
#endif

#endif /* AKSL_FORM_H */
