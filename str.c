// src/aksl/str.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

c_string_rep::
    upper
    lower
    cat
    cat
    cat
    cat
    subst
    subst
c_string::
    subst
    subst
    para_fill
    print_with_quotes
    stat_ext
c_stringlist::
    index
    merge
    find
    print
    copy
operator<<(ostream&, const c_stringlist&)
time_interp
------------------------------------------------------------------------------*/

// AKSL header files:
#include "aksl/str.h"
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif

// System header files:
#ifndef AKSL_X_CTYPE_H
#define AKSL_X_CTYPE_H
#include <ctype.h>
#endif
#ifndef AKSL_X_SYS_STAT_H
#define AKSL_X_SYS_STAT_H
#include <sys/stat.h>
#endif

// For strftime():
#ifndef AKSL_X_TIME_H
#define AKSL_X_TIME_H
#include <time.h>
#endif

#ifdef HAVE_SNPRINTF
// For snprintf().
#ifndef AKSL_X_STDIO_H
#define AKSL_X_STDIO_H
#include <stdio.h>
#endif
#endif

typedef unsigned long ulong;

//--------------------------//
//    c_string_rep::upper   //
//--------------------------//
void c_string_rep::upper() {
    if (nullstr())
        return;
    for (char* pc = s; *pc; ++pc)
        *pc = toupper(*pc);
    } // End of function c_string_rep::upper.

//--------------------------//
//    c_string_rep::lower   //
//--------------------------//
void c_string_rep::lower() {
    if (nullstr())
        return;
    for (char* pc = s; *pc; ++pc)
        *pc = tolower(*pc);
    } // End of function c_string_rep::lower.

//----------------------//
//   c_string_rep::cat  //
//----------------------//
void c_string_rep::cat(const char* pc) {
    if (pc && *pc) { // Should always be true.
        if (s) {
            char* x = new char[strlen(s) + strlen(pc) + 1];
            strcpy(x, s);
            strcat(x, pc);
            delete[] s;
            s = x;
            }
        else {
            s = new char[strlen(pc) + 1];
            strcpy(s, pc);
            }
        }
    } // End of function c_string_rep::cat.

/*------------------------------------------------------------------------------
Concatenate a string which is the decimal representation of
the given integer.
An unsigned integer with 31 bits can be at most 2,000,000,000.
So 11 digits should be sufficient for the "long" string.
------------------------------------------------------------------------------*/
//----------------------//
//  c_string_rep::cat   //
//----------------------//
#if HAVE_SNPRINTF
void c_string_rep::cat(long x) {
    const int n = 24;
    static char buf[n];

    buf[0] = 0;
    int nchars = snprintf(buf, n, "%ld", x);
    if (nchars <= 0)
        return;

    if (s) {
        char* s2 = new char[strlen(s) + strlen(buf) + 1];
        strcpy(s2, s);
        strcat(s2, buf);
        delete[] s;
        s = s2;
        }
    else {
        s = new char[strlen(buf) + 1];
        strcpy(s, buf);
        }
    } // End of function c_string_rep::cat.
#endif

/*------------------------------------------------------------------------------
Concatenate a string which is the decimal representation of
the given integer.
An unsigned integer with 32 bits can be at most 4,000,000,000.
So 11 digits should be sufficient for the "unsigned long" string.
------------------------------------------------------------------------------*/
//----------------------//
//  c_string_rep::cat   //
//----------------------//
#if HAVE_SNPRINTF
void c_string_rep::cat(unsigned long x) {
    const int n = 24;
    static char buf[n];

    buf[0] = 0;
    int nchars = snprintf(buf, n, "%lu", x);
    if (nchars <= 0)
        return;

    if (s) {
        char* s2 = new char[strlen(s) + strlen(buf) + 1];
        strcpy(s2, s);
        strcat(s2, buf);
        delete[] s;
        s = s2;
        }
    else {
        s = new char[strlen(buf) + 1];
        strcpy(s, buf);
        }
    } // End of function c_string_rep::cat.
#endif

//----------------------//
//   c_string_rep::cat  //
//----------------------//
void c_string_rep::cat(const c_string_rep* p) {
    // Forget the check for null p. Only c_string can call the function.
    if (p->s && *p->s) { // Should always be true.
        if (s) {
            char* x = new char[strlen(s) + strlen(p->s) + 1];
            strcpy(x, s);
            strcat(x, p->s);
            delete[] s;
            s = x;
            }
        else {
            s = new char[strlen(p->s) + 1];
            strcpy(s, p->s);
            }
        }
    } // End of function c_string_rep::cat.

/*------------------------------------------------------------------------------
All instances of string p1 in the object string are substituted with p2.
If p2 is the null pointer or if p2->s is the null pointer, then this is regarded
as equivalent to the null string.
------------------------------------------------------------------------------*/
//----------------------//
//  c_string_rep::subst //
//----------------------//
void c_string_rep::subst(const c_string_rep* p1, const c_string_rep* p2) {
    // String for converting null pointer to null string:
    static const char n_string[] = "";

    if (nullstr() || !p1 || p1->nullstr())
        return;

    // Regard the null pointer or null string pointer as a null string:
    const char* s2 = p2 ? ((p2->s) ? p2->s : n_string) : n_string;
    char* pc3 = new_str_subst(s, p1->s, s2);

    // If the string has been modified, then eat it:
    if (pc3)
        eat(pc3);
    } // End of function c_string_rep::subst.

/*------------------------------------------------------------------------------
Substitute c1 with c2.
Return the number of substitutions made.
------------------------------------------------------------------------------*/
//----------------------//
//  c_string_rep::subst //
//----------------------//
void c_string_rep::subst(char c1, char c2) {
    if (!s)
        return;
    for (register char* pc = s; *pc; ++pc)
        if (*pc == c1)
            *pc = c2;
    } // End of function c_string_rep::subst.

/*------------------------------------------------------------------------------
Replace c1 with c2 in the string.
------------------------------------------------------------------------------*/
//----------------------//
//    c_string::subst   //
//----------------------//
void c_string::subst(char c1, char c2) {
    if (nullstr())
        return;
    if (!strchr(rep->s, c1))
        return;
    detach();
    rep->subst(c1, c2);
    } // End of function c_string::subst.

/*------------------------------------------------------------------------------
This function substitutes corresponding elements of sl1 with elements of sl2.
If both lists contain 1 element, then this reduces to the same as
c_string::subst(c_string&, c_string). If sl1 is empty, no substitution
takes place. If sl2 has less elements than sl1, then only the first sl2.length()
elements of sl1 are substituted. If sl2 has more elements than sl1, then the
excess elements are ignored.
------------------------------------------------------------------------------*/
//----------------------//
//    c_string::subst   //
//----------------------//
void c_string::subst(const c_stringlist& sl1, const c_stringlist& sl2) {
    c_stringlink* pl2 = sl2.first();
    Forall(c_stringlink, pl1, sl1) {
        if (!pl2)
            break;
        subst(*pl1, *pl2);
        pl2 = pl2->next();
        }
    } // End of function c_string::subst.

/*------------------------------------------------------------------------------
Substitute '\n' for space in such a way as to make the line widths
no greater than "width".
Also substitute ' ' for any '\n' which is not necessary.
In other words, the final result should be such that each substring should have
the minimal number of '\n' characters required to keep the line length
limited to "width".
If a "word" is longer than "width", then the word is allowed to go outside the
specified line width. This means that paragraphing is fully reversible.
Other possible future options are:
-   words are split when longer than "width"
-   words are split, with a '-' appearing at the beginning of carry-over lines.
    - suboption: no recombination of hyphened words
    - suboption: hyphened words at the line boundary are recombined if
      they move to the interior of the line
    - suboption: all hyphened words are recombined if possible.
Space means ' ' (SP), '\t' (HT), '\r' (CR), '\n' (LF), '\f' (FF) or VT.
Paragraph formatting may replace such characters with LF, and reformatting
to a different line width may replace LF with SP. Hence reformatting should
be reversible as long as the only spaces in the string are SP and LF.
No consideration is taken for the CR-LF representation for end of line.
CR, FF and VT characters are regarded as equivalent to SP, so that the
result will look wrong if they are included in a string.
HT characters are dealt with as indicating a move to the nearest multiple of
8 columns, but this effect is lost if they are removed by substituting with LF.
The return value is the length of the longest line.
If "width" is zero, then all LFs in the string are removed and 0 is returned.
------------------------------------------------------------------------------*/
//----------------------//
//  c_string::para_fill //
//----------------------//
int c_string::para_fill(int width) {
    // "rep" is always non-zero.
    if (rep->nullstr())
        return 0;

    // Regard width == 0 as meaning infinite width:
    if (width <= 0) {
        const char* pc = ::strchr(rep->s, '\n');
        if (pc) {
            detach();
            rep->subst('\n', ' ');
            }
        return 0;
        }

    // Two passes may be necessary if the string must be detached.
    // If any change is required, detach and loop back.
    // This coding method is slower than two explicit loops, but it
    // is easier to maintain.
    register bool_enum mult_link = bool_enum(rep->nlink > 1);
    int max_length = 0;                 // Maximum line length.
    for (;;) {
        register int n = 0;             // Column number of current character.
        register char* last_space = 0;  // Position of last space on line.

        // Traverse the string, inserting and clearing LFs:
        // (If any change must be made, break the loop, detach, and re-run.)
        register char* pc = 0;
        for (pc = rep->s; *pc; ++pc) {
            // Record candidates for a line break:
            if (isspace(*pc)) {
                // If there's a later space than an LF, then clear the LF:
                if (last_space && *last_space == '\n') {
                    if (mult_link)
                        break;
                    *last_space = ' ';
                    }
                last_space = pc;

                // Even if the previous character is space, regard it as
                // being printed, and therefore contributing to line length:
                // If the last character of a line is space, this may result in
                // the max_length being greater than the maximum visible length.
                if (max_length < n)
                    max_length = n;
                }

            // If the line exceeds "width", go to the next line:
            if (n >= width) {
                if (last_space) {
                    // Insert an LF if it's not there already:
                    if (*last_space != '\n') {
                        if (mult_link)
                            break;
                        *last_space = '\n';
                        }

                    // Adjust "pc" to the LF at the end of the previous line:
                    pc = last_space;
                    last_space = 0;
                    n = 0;
                    }
                else    // If a word is longer than "width", just soldier on:
                    n += 1;
                continue;
                }

            // Skip the right amount for a tab or other character:
            if (*pc == '\t') {
                n &= ~0x07;     // n = 8 * floor(n/8).
                n += 8;
                }
            else
                n += 1;
            } // End of string traversal loop.

        // If all characters in the string have been examined, it's finished:
        if (!*pc) {
            // Before exiting, tidy up the last line:
            if (last_space && *last_space == '\n') {
                if (!mult_link) {
                    *last_space = ' ';
                    break;
                    }
                // Otherwise go round again....
                }
            else
                break;
            }

        // Otherwise, the loop exited because of multiple linking. So detach:
        detach();
        mult_link = bool_enum(rep->nlink > 1);
        if (mult_link)          // This should never happen.
            break;
        }
    return max_length;
    } // End of function c_string::para_fill.

//------------------------------//
//  c_string::print_with_quotes //
//------------------------------//
void c_string::print_with_quotes(ostream& os) const {
    if (nullstr())
        return;
    os << " \"" << *this << "\"";
    } // End of function c_string::print_with_quotes.

/*------------------------------------------------------------------------------
This function tries to make a decision on whether or not to add a given
extension to a file name. If the decision is made to add the extension, then
the extension is appended.
The current algorithm is:  if the extension does not appear in the string
and the extended name corresponds to a file which is not a directory or
block device, then the extension is added to the string.
Of course, this would be silly if the desired file is actually a directory
or block device!
------------------------------------------------------------------------------*/
//----------------------//
//  c_string::stat_ext  //
//----------------------//
int c_string::stat_ext(const char* ext) {
    if (nullstr() || ::nullstr(ext))
        return 0;

    // If the extension is found at the end of the string, don't add extension:
    const char* fn = *this;
    int fn_length = strlen(fn);
    int ext_length = strlen(ext);
    if (ext_length <= fn_length
            && strcmp(fn + fn_length - ext_length, ext) == 0)
        return 0;

    // Try to add the given extension:
    c_string fn_ext = *this;
    fn_ext += ext;

    // See if a file with the extended name exists:
    struct stat statbuf;
    int ret = stat((const char*)fn_ext, &statbuf);

    // If the extended file exists and is not a directory or block device,
    // use it as the file name:
#ifndef WIN32
    if (ret >= 0 && !S_ISDIR(statbuf.st_mode) && !S_ISBLK(statbuf.st_mode)) {
#else
    // There don't seem to be block special devices in win32.
    if (ret >= 0 && (statbuf.st_mode & _S_IFMT) != _S_IFDIR) {
#endif
        *this = fn_ext;
        }

    return 0;
    } // End of function c_string::stat_ext.

//----------------------//
//  c_stringlist::index //
//----------------------//
long c_stringlist::index(const c_string& cs) const {
    long i = 0;
    c_stringlink* p = 0;
    forall(p, *this)
        if (*p == cs)
            break;
        else
            ++i;
    return p ? i : -1;
    } // End of function c_stringlist::index.

//----------------------//
//  c_stringlist::merge //
//----------------------//
void c_stringlist::merge(const c_stringlist& csl) {
    Forall(c_stringlink, p, csl)
        if (!find(*p)) {
            c_stringlink* q = new c_stringlink;
            q->operator=(*p);
            append(*q);
            }
    } // End of function c_stringlist::merge.

/*------------------------------------------------------------------------------
c_stringlist::find() find a c_string in a list.
------------------------------------------------------------------------------*/
//----------------------//
//  c_stringlist::find  //
//----------------------//
c_stringlink* c_stringlist::find(const c_string& cs) const {
    c_stringlink* p = 0;
    forall(p, *this)
        if (*p == cs)
            break;
    return p;
    } // End of function c_stringlist::find.

//----------------------//
//  c_stringlist::print //
//----------------------//
void c_stringlist::print(ostream& os) const {
    Forall(c_stringlink, p, *this)
        os << *p << "\n";
    } // End of function c_stringlist::print.

//----------------------//
//  c_stringlist::copy  //
//----------------------//
void c_stringlist::copy(const c_stringlist& x) {
    Forall(c_stringlink, p0, x) {
        c_stringlink* p1 = new c_stringlink;
        *p1 = *p0;
        append(p1);
        }
    } // End of function c_stringlist::copy.

/*------------------------------------------------------------------------------
This returns:
()                  empty list
"abc"               single element
("abc" ... "xyz")   2 or more elements
This is not really consistent with operator<<(ostream&, c_string&), because
of the use of quotes.
------------------------------------------------------------------------------*/
//----------------------//
//      operator<<      //
//----------------------//
ostream& operator<<(ostream& os, const c_stringlist& csl) {
    c_stringlink* psl = csl.first();
    if (!psl) {                         // Empty list.
        os << "()";
        return os;
        }
    if (!psl->next()) {                 // Single string.
        os << "\"" << *psl << "\"";
        return os;
        }
    os << "(";
    forall(psl, csl) {                  // Two or more elements.
        os << "\"" << *psl << "\"";
        if (psl->next())
            os << " ";
        }
    os << ")";
    return os;
    } // End of function operator<<.

/*------------------------------------------------------------------------------
This function uses strftime() to convert a string to a file name which is
hopefully different to other file names.
"cs" is supposed to contain a strftime-style format string.
"time" is supposed to be a time structure.
The new formatted string is returned in "cs".
------------------------------------------------------------------------------*/
//----------------------//
//      time_interp     //
//----------------------//
void time_interp(c_string& cs, struct tm* time) {
    if (cs.nullstr() || !time)
        return;

    const int buflen = 1024;
    char buf[buflen];

    // strftime() incorrectly requires non-const format string:
    char* pc = cs.new_strcpy();
    size_t x = strftime(buf, buflen, pc, time);
    delete[] pc;
    if (x > 0) {
        cs = buf;
        cs.lower();
        }
    } // End of function time_interp.
