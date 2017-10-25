// src/aksl/options.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_OPTIONS_H
#define AKSL_OPTIONS_H
/*------------------------------------------------------------------------------
This file is supposed to contain compile-time options.
Modify this file to choose these options.
------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
This option will control a new feature to force FEL dequeuing of simultaneous
events to be in the same order that they have been enqueued.
This will be implemented by adding an extra field to each event object to
indicate the relative order of events which have the same execution time.
Since this involves some space and time overheads, it is an option.
Turn this feature off if you don't need it.
---------------------------------------------------------------------------*/
#ifndef AKSL_SYSTM_FEL_STRICT_ORDER
#define AKSL_SYSTM_FEL_STRICT_ORDER     1
#endif

#endif /* AKSL_OPTIONS_H */
