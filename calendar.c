// src/aksl/calendar.c   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

y2j
leap
ymd2jd
jd2ymd
cal_time
utc_time_string
loc_time_string
julian_day
print_date
y2kwcal
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/calendar.h"
#ifndef AKSL_NUMPRINT_H
#include "aksl/numprint.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_CONFIG_H
#include "aksl/config.h"
#endif

// System header files:
#ifndef AKSL_X_MATH_H
#define AKSL_X_MATH_H
#include <math.h>
#endif

#ifndef AKSL_X_STRING_H
#define AKSL_X_STRING_H
#include <string.h>
#endif

#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream.h>
#endif

// Linux needs stdio.h for sprintf().
#if defined(WIN32) || defined(linux)
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif
#endif

const int month_offsets[2][13] = { // Non-leap years, then leap years:
    { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 }, // ....365.
    { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }  // ....366.
    };

const char* month_names[12] = { // Full month names.
    "January", "February", "March",     "April",   "May",      "June",
    "July",    "August",   "September", "October", "November", "December"
    };

const char* month_names3[12] = { // 3-letter month names.
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };

/*------------------------------------------------------------------------------
Convert a year number to Julian day, using 1753 as the changeover year from the
Julian to Gregorian calendar.
------------------------------------------------------------------------------*/
//----------------------//
//          y2j         //
//----------------------//
long y2j(long y) {
    long j = 0;

    if (y > 1752) {
        j = 1721061 + 365 * y
//         + lfloor((y-1)/4) - lfloor((y-1)/100) + lfloor((y-1)/400);
         + ilfloor(y - 1, 4) - ilfloor(y - 1, 100) + ilfloor(y - 1, 400);
        }
    else if (y > 0) {
//        j = 1 + 365*(y+4713) + lfloor((y+4715)/4) - 366;
        j = 1 + 365 * (y + 4713) + ilfloor(y + 4715, 4) - 366;
        }
    else if (y < 0) {
//        j = 1 + 365*(y+4713) + lfloor((y+4715)/4);
        j = 1 + 365 * (y + 4713) + ilfloor(y + 4715, 4);
        }
    else
        j = 0;
    return j;
    } // End of function y2j.

//----------------------//
//         leap         //
//----------------------//
bool_enum leap(long y) {
    bool_enum l = false;

    if (y > 1752)
        l = (bool_enum)(((y % 4) == 0) - ((y % 100) == 0) + ((y % 400) == 0));
    else if (y > 0)
        l = (bool_enum)((y % 4) == 0);
    else if (y < 0)
        l = (bool_enum)(((-y) % 4) == 0);
    else
        l = false;
    return l;
    } // End of function leap.

/*------------------------------------------------------------------------------
Convert year-month-day to Julian day. Assumes the Gregorian calendar from about
1753 onwards, as in the British empire and in the US. Note that the Julian day
is only correct at midday UTC.

E.g. (1900, 1, 1) => 2415021.
     (1970, 1, 1) => 2440588.
     (1993, 1, 1) => 2448989.
------------------------------------------------------------------------------*/
//----------------------//
//         ymd2jd       //
//----------------------//
long ymd2jd(long y, long m, long d) {
    // Get the Julian day for 1 January:
    long j = y2j(y);

    // Make a vain attempt to get the month in range if it isn't:
    if (m < 1)
        m = 1;
    if (m > 12)
        m = 12;

    // Add in the month offset and day:
    j += month_offsets[0][m - 1] + d - 1;

    // Correction for the special year 1752:
    if (y == 1752 && (m >= 10 || m == 9 && d >= 14))
        j -= 11;

    // Correction for leap years:
    bool_enum l = leap(y);
    if (l && m >= 3)
        j += 1;

    return j;
    } // End of function ymd2jd.

/*------------------------------------------------------------------------------
1970 January 1 (Julian day 2440588) is returned as 1970, 1, 1.
This is probably not a very fast implementation. The code should be improved!
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
To improve this algorithm, could eliminate the iterative part by finding
an exact formula for converting from jd to year.
....
------------------------------------------------------------------------------*/
//----------------------//
//        jd2ymd        //
//----------------------//
int jd2ymd(long jd, long& y, long& m, long& d) {
    // Estimate year by approximating around 1970 [or 1752]:
    if (jd < jd_jg_uk)
        y = 1752 + long(floor((jd - jd_1752) / days_per_julian_year));
    else
        y = 1970 + long(floor((jd - jd_1970) / days_per_greg_year));

    // Then make fine adjustments:
    long yj = y2j(y);
    while (yj < jd) {
        y += 1;
        yj = y2j(y);
        }
    while (yj > jd) {
        y -= 1;
        yj = y2j(y);
        }

    // Check if it's a leap year:
    int ly = leap(y) ? 1 : 0;

    // Calculate the month and day of month:
    m = 0;
    d = jd - yj;
    while (m < 12 && month_offsets[ly][m] <= d)
        m += 1;
    d -= month_offsets[ly][m - 1];
    d += 1;
    return 0;
    } // End of function jd2ymd.

//----------------------//
//       cal_time       //
//----------------------//
time_t cal_time() {
    return time((time_t*)0);
    } // End of function cal_time.

/*------------------------------------------------------------------------------
Example: "UTC 1993 June 07 Monday 03:18:45".
------------------------------------------------------------------------------*/
//----------------------//
//   utc_time_string    //
//----------------------//
char* utc_time_string(time_t* pt) {
#ifdef HAVE_STRFTIME
    const int max_time_string_length = 100;
    static char time_string[max_time_string_length + 1];

    // Get the time:
    time_t cal_time = pt ? *pt : time((time_t*)0);

    // Find out UTC time:
    // WARNING: The functions strftime() function uses the previous call to
    // either gmtime() or localtime() to work out which values to use to use.
    // I.e. it does not just use the fourth function argument!!!!
    tm* t = gmtime(&cal_time);
    size_t time_string_length = strftime(time_string,
        max_time_string_length + 1, "UTC %Y %B %d %A %H:%M:%S", t);

    if (time_string_length <= 0)
        time_string[0] = 0;
#else  /* HAVE_STRFTIME */
    static char time_string[] = "[Undefined strftime()]";
#endif /* HAVE_STRFTIME */
    char* pc = new char[strlen(time_string) + 1];
    strcpy(pc, time_string);
    return pc;
    } // End of function utc_time_string.

/*------------------------------------------------------------------------------
Example: "CST 1993 June 07 Monday 03:18:45".
------------------------------------------------------------------------------*/
//----------------------//
//   loc_time_string    //
//----------------------//
char* loc_time_string(time_t* pt) {
    const int max_time_string_length = 100;
    static char time_string[max_time_string_length + 1];

    // Get the time:
    time_t cal_time = pt ? *pt : time((time_t*)0);

    // Find out the local time:
    tm* t = localtime(&cal_time);
    size_t time_string_length = strftime(time_string,
        max_time_string_length + 1, "%Z %Y %B %d %A %H:%M:%S", t);

    if (time_string_length <= 0)
        time_string[0] = 0;
    char* pc = new char[strlen(time_string) + 1];
    strcpy(pc, time_string);
    return pc;
    } // End of function loc_time_string.

//----------------------//
//      julian_day      //
//----------------------//
double julian_day(time_t* pt) {
    // Get the time:
    time_t cal_time = pt ? *pt : time((time_t*)0);

    // Find out the utc time:
    tm* utc_time = gmtime(&cal_time);

    // Work out the Julian day:
    long y_utc = 1900 + utc_time->tm_year;
    long y_utc1 = y_utc - 1;

    // UTC 12 noon 1 January 1900 was Julian Day 2415021.0:
    double j_date = 1721060.5 + y_utc*365 + floor((double)y_utc1/4.0);

    // Convert from Julian date to Gregorian date:
    j_date += -floor((double)y_utc1/100.0) + floor((double)y_utc1/400.0);

    // Add in the day of the year (0 to 365):
    j_date += utc_time->tm_yday;

    // Calculate seconds in day relative to midnight:
    long day_seconds = utc_time->tm_sec
                       + 60*(utc_time->tm_min + 60*utc_time->tm_hour);

    // Add in the time of day:
    j_date += day_seconds/double(24*60*60);

    return j_date;
    } // End of function julian_day.

//----------------------//
//      print_date      //
//----------------------//
void print_date(ostream& os) {
    time_t t = cal_time();
    char* utc = utc_time_string(&t);
    char* loc = loc_time_string(&t);
    double jd = julian_day(&t);

    if (loc)
        os << "Local time:     " << loc << "\n";
    if (utc)
        os << "World time:     " << utc << "\n";
    os << "Julian day:     " << realstring(jd, 10, 6) << endl;
    os << "Ephemeris time: " << realstring(jd - 2415020.5, 10, 6) << endl;
    os << "Unix-based JD:      "
       << realstring(2440587.5 + (double)t/(24*3600), 10, 6) << endl;
    os << "Unix 1970 seconds:  " << (unsigned long)t << endl;
    } // End of function print_date.

/*------------------------------------------------------------------------------
This function will return a calendar-week calendar for the given year.
The returned string is static, and should not be deleted or altered.
It is volatile in the sense that it is overwritten by the following
call to this function.
------------------------------------------------------------------------------*/
//----------------------//
//        y2kwcal       //
//----------------------//
const char* y2kwcal(long y) {
    // Week-number, per-day entries, month name, end-of-line.
    const int line_length = 3 + (3 * 7) + 4 + 1;
    const int max_weeks = 54;

    // One line for title. The rest for individual weeks.
    const int str_length = (1 + max_weeks) * line_length + 1;
    static char buf[str_length];

    // Find out the week-day of the first day of the year.
    cal_wd_t wd = y2wd(y);

    // If first day is Monday to Thursday, then the first day is in the
    // first week, and therefore the first week is week 1.
    // If the first day is in Friday to Sunday, then the first day is in
    // the 0th week, because week 1 is then the following week.
    int first_week = (wd <= 3) ? 1 : 0;

    // Find out if this is a leap year.
    int lp = leap(y);

   // Write the calendar.
    int month = 0;
    int last_month = -1;
    int year_day = 0;
    int month_day = 1;
    char* pc = buf;
    char* pc0 = pc;

    // Top header line.
    static const char header[] = "wk  M  Tu W  Th F  Sa Su    \n";
    sprintf(pc, "%s", header);
    pc += strlen(header);

    int finished = 0;
    for (int w = first_week; w <= max_weeks; ++w) {
        // Print the week number.
        sprintf(pc, "%2d:", w);
        pc += 3;
        for (int i = 0; i < 7; ++i) {
            // Print the day of the month.
            if (!finished && (w > first_week || i >= wd)) {
                sprintf(pc, "%3d", month_day);
                month_day += 1;
                year_day += 1;
                }
            else {
                // First or last week.
                sprintf(pc, "   ");
                }
            pc += 3;

            // If not at the last week, check if the month has ended.
            if (!finished && year_day >= month_offsets[lp][month + 1]) {
                month += 1;
                if (month >= 12) {
                    finished = 1;
                    }
                month_day = 1;
                }
            }
        if (!finished && month > last_month && month_day > 1) {
            // 4-character string for month-name abbreviation.
            sprintf(pc, " %s\n", month_names3[month]);
            last_month = month;
            }
        else {
            // Blank 4 characters.
            sprintf(pc, "    ");
            }
        pc += 4;

        // Append end-of-line.
        *pc++ = '\n';

        // Trace.
//        cout << pc0;

        if (finished)
            break;
        pc0 = pc;
        }

    // Terminal null-character.
    *pc++ = 0;

    return buf;
    } // End of function y2kwcal.
