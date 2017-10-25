// src/aksl/calendar.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

#ifndef AKSL_CALENDAR_H
#define AKSL_CALENDAR_H
/*------------------------------------------------------------------------------
Classes in this file:

------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_BOOLE_H
#include "aksl/boole.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_TIME_H
#define AKSL_X_TIME_H
#include <time.h>
#endif

// Weekdays. Monday = 0, ... Sunday = 6.
enum cal_wd_t {
    calMONDAY,
    calTUESDAY,
    calWEDNESDAY,
    calTHURSDAY,
    calFRIDAY,
    calSATURDAY,
    calSUNDAY
    };

// Julian day at noon UTC on 1 January 1970, 1752 (in the Gregorian calendar).
// (Note that 1 Jan 4713 B.C. = Julian day 1.
// That is, 31 Dec 4714 B.C. = Julian day 0.)
const long      jd_1970             = 2440588;
const long      jd_1752             = 2360976;

// Date of changeover from Julian to Gregorian calendar in Britain:
const long      y_jg_uk             = 1752;
const long      m_jg_uk             = 9;    // September.
const long      d_jg_uk0            = 3;    // Old day number of first new day.
const long      d_jg_uk1            = 14;   // New day number of first new day.
const long      jd_jg_uk            = 2361222;

// Mean number of days per Gregorian year:
const double    days_per_greg_year  = 365.2425;
const double    days_per_julian_year = 365.25;

const long      secs_per_min        = 60;
const long      mins_per_hr         = 60;
const long      hrs_per_day         = 24;

const long      secs_per_hr         = secs_per_min * mins_per_hr;
const long      mins_per_day        = mins_per_hr * hrs_per_day;

const long      secs_per_day        = secs_per_hr * hrs_per_day;

// Exported functions:
extern long         y2j(long y);
extern bool_enum    leap(long y);
extern long         ymd2jd(long y, long m, long d);
extern int          jd2ymd(long jd, long& y, long& m, long& d);
extern time_t       cal_time();
extern char*        utc_time_string(time_t* = 0);
extern char*        loc_time_string(time_t* = 0);
extern double       julian_day(time_t* = 0);
extern void         print_date(ostream& = cout);
extern const char*  y2kwcal(long y);

// This returns the week-day for a given year.
inline cal_wd_t y2wd(long y) { return cal_wd_t(y2j(y) % 7); }

// Return time in Unix seconds since 1970 (no leap seconds added!).
inline long ymd2unix(long y, long m, long d)
    { return (ymd2jd(y, m, d) - jd_1970) * 86400; }

// Exported arrays:
extern const int month_offsets[2][13];
extern const char* month_names[12];
extern const char* month_names3[12];

#endif /* AKSL_CALENDAR_H */
