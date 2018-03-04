// src/aksl/aksldate.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

------------------------------------------------------------------------------*/

#include "aksl/aksldate.h"

#ifdef DATESTRING
const char *const aksldatestring = DATESTRING;
#elif defined(__DATE__)
const char *const aksldatestring = __DATE__;
#else
const char *const aksldatestring = "unknown date";
#endif

#ifdef MACHINESTRING
const char *const akslmachinestring = MACHINESTRING;
#else
const char *const akslmachinestring = "unknown host";
#endif
