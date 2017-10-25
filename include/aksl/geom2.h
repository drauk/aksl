// src/aksl/geom2.h   2017-10-25   Alan U. Kennington.
// $Id$
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2017, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/

// A comprehensive 2-d geometry library should be included in AKSL.
// But in the meantime, this is a quarter-baked set of 2-d geometry things.
#ifndef AKSL_GEOM2_H
#define AKSL_GEOM2_H
/*------------------------------------------------------------------------------
Inline functions in this file:

dist2
dist
dist
dist
E_dist
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes declared in this file:

position2::
position2list::
motion2_state::
motion2_statelist::
int_rect::
real_rect::
pt_int16::
rect_int16::
------------------------------------------------------------------------------*/

using namespace std;

// AKSL header files:
#ifndef AKSL_LIST_H
#include "aksl/list.h"
#endif
#ifndef AKSL_AKSLDEFS_H
#include "aksl/aksldefs.h"
#endif
#ifndef AKSL_PHYS_H
#include "aksl/phys.h"
#endif
#ifndef AKSL_NUMB_H
#include "aksl/numb.h"
#endif

// System header files:
#ifndef AKSL_X_IOSTREAM_H
#define AKSL_X_IOSTREAM_H
#include <iostream>
#endif
#ifndef AKSL_X_MATH_H
#define AKSL_X_MATH_H
#include <math.h>
#endif

// PI = 3.14159 26535 89793 23846 26433 83279 50288 41971 69399 37511;
#ifndef M_PI
const double M_PI = 3.14159265358979323846264338327950288419716939937511;
#endif

// Some useful quotients:
const double Q_PI_180 = M_PI / 180;
const double Q_PI_360 = M_PI / 360;
const double Q_180_PI = 180 / M_PI;
const double Q_360_PI = 360 / M_PI;

// Exported functions:
extern double dms2deg(long i);
extern double E_dist2(double phi0, double theta0, double phi1, double theta1);
extern double E_dist2_lin(double phi0, double theta0,
                          double phi1, double theta1);
extern double E_dist_lin(double phi0, double theta0,
                         double phi1, double theta1);

//==============================================================================
// Inline functions:
//==============================================================================
/*------------------------------------------------------------------------------
The square of the Euclidean distance between (x1, y1) and (x2, y2):
This is an integer argument version which returns a double.
------------------------------------------------------------------------------*/
//----------------------//
//        dist2         //
//----------------------//
inline double dist2(int x1, int y1, int x2, int y2) {
    double dx = double(x1 - x2);
    double dy = double(y1 - y2);
    return dx*dx + dy*dy;
    }

//----------------------//
//        dist          //
//----------------------//
inline double dist(int x1, int y1, int x2, int y2)
    { return hypot(x1 - x2, y1 - y2); }

//----------------------//
//        dist          //
//----------------------//
inline double dist(double x1, double y1, double x2, double y2)
    { return hypot(x1 - x2, y1 - y2); }

//----------------------//
//         dist         //
//----------------------//
inline double dist(double x, double y) { return hypot(x, y); }

/*------------------------------------------------------------------------------
E_dist() takes the square root of E_dist2().
This is distance on the earth in metres, with angles in radians.
------------------------------------------------------------------------------*/
//----------------------//
//        E_dist        //
//----------------------//
inline double E_dist(double phi0, double theta0, double phi1, double theta1) {
    return sqrt(E_dist2(phi0, theta0, phi1, theta1));
    } // End of function E_dist.

//==============================================================================
// Classes.
//==============================================================================
/*------------------------------------------------------------------------------
This represents a point in 2-d modelled space, not on the screen.
------------------------------------------------------------------------------*/
//----------------------//
//      position2::     //
//----------------------//
struct position2: public slink {
    double x, y;                // Coords of a point.

    position2* next() { return (position2*)slink::next(); }

    void set(double xx, double yy) { x = xx; y = yy; }
    void set_x(double xx) { x = xx; }
    void set_y(double yy) { y = yy; }

//    position2& operator=(const position2& x) {}
//    position2(const position2& x) {};
    position2() { x = y = 0; }
    position2(double xx, double yy) { x = xx; y = yy; }
    ~position2() {}
    }; // End of struct position2.

//----------------------//
//    position2list::   //
//----------------------//
struct position2list: private s2list {
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    position2* first() { return (position2*)s2list::first(); }
    position2* last() { return (position2*)s2list::last(); }
    void append(position2* p) { s2list::append(p); }
    void prepend(position2* p) { s2list::prepend(p); }
    position2* popfirst() { return (position2*)s2list::popfirst(); }
    position2* poplast() { return (position2*)s2list::poplast(); }
    position2* remove(position2* p)
        { return (position2*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(position2* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(position2list& l) { s2list::swallow(&l); }
#ifdef SUNOS4_COMP_VERSION3_0_1
    inline void clear();
#else
    void clear() { for (position2* p = first(); p; )
        { position2* q = p->next(); delete p; p = q; } clearptrs(); }
#endif

//    position2list& operator=(const position2list& x) {}
//    position2list(const position2list& x) {};
    position2list() {}
    ~position2list() { clear(); }
    }; // End of struct position2list.

// Bug workaround:
#ifdef SUNOS4_COMP_VERSION3_0_1
inline void position2list::clear() { for (position2* p = first(); p; )
    { position2* q = p->next(); delete p; p = q; } clearptrs(); }
#endif

/*------------------------------------------------------------------------------
This represents combined space, time and velocity.
------------------------------------------------------------------------------*/
//----------------------//
//    motion2_state::   //
//----------------------//
struct motion2_state: public position2 {
    position2   v;          // The velocity. (A point in velocity space.)
    double      t;          // The time. (Any old time scale will do.)

    motion2_state* next() const { return (motion2_state*)slink::next(); }

    void set(double xx, double yy, double vx, double vy, double tt)
        { position2::set(xx, yy); v.set(vx, vy); t = tt; }
    void set_pos_t(double xx, double yy, double tt)
        { position2::set(xx, yy); t = tt; }
    void set_vx(double vx) { v.set_x(vx); }
    void set_vy(double vy) { v.set_y(vy); }
    void set_t(double tt) { t = tt; }

    // Extrapolated position at time tt.
    void current_pos(position2& pos, double tt)
        { pos.x = x + v.x * (tt - t); pos.y = y + v.y * (tt - t); }
    double current_x(double tt)     // Extrapolated x value at time tt.
        { return x + v.x * (tt - t); }
    double current_y(double tt)     // Extrapolated y value at time tt.
        { return y + v.y * (tt - t); }

//    motion2_state& operator=(const motion2_state& x) {}
//    motion2_state(const motion2_state& x) {}
    motion2_state() { t = 0; }
    ~motion2_state() {}
    }; // End of struct motion2_state.

//----------------------//
//  motion2_statelist:: //
//----------------------//
struct motion2_statelist: private s2list {
protected:
    s2list::clearptrs;
public:
    // The routine members:
    s2list::empty;
    s2list::length;
    s2list::member;
    s2list::position;
    motion2_state* first() const { return (motion2_state*)s2list::first(); }
    motion2_state* last() const { return (motion2_state*)s2list::last(); }
    motion2_state* element(long i) const
        { return (motion2_state*)s2list::element(i); }
    void append(motion2_state* p) { s2list::append(p); }
    void prepend(motion2_state* p) { s2list::prepend(p); }
    motion2_state* popfirst() { return (motion2_state*)s2list::popfirst(); }
    motion2_state* poplast() { return (motion2_state*)s2list::poplast(); }
    motion2_state* remove(motion2_state* p)
        { return (motion2_state*)s2list::remove(p); }
    void delfirst() { delete popfirst(); }
    void dellast() { delete poplast(); }
    void delremove(motion2_state* p) { delete remove(p); }
    s2list::insertafter;
    void swallow(motion2_statelist& l) { s2list::swallow(&l); }
    void gulp(motion2_statelist& l) { s2list::gulp(&l); }
    void clear() { for (motion2_state* p = first(); p; )
        { motion2_state* q = p->next(); delete p; p = q; } clearptrs(); }

//    motion2_statelist& operator=(const motion2_statelist& x) {}
//    motion2_statelist(const motion2_statelist& x) {}
    motion2_statelist() {}
    ~motion2_statelist() { clear(); }
    }; // End of struct motion2_statelist.

/*------------------------------------------------------------------------------
An integer rectangle presumably is used to represent a portion of the screen.
------------------------------------------------------------------------------*/
//----------------------//
//      int_rect::      //
//----------------------//
struct int_rect { // A rectangle with int coordinates.
    int x, y;
    uint w, h;

    void set(int xx, int yy, uint ww, uint hh)
        { x = xx; y = yy; w = ww; h = hh; }
    void print(ostream& = cout);

    int_rect() { x = y = 0; w = h = 0; }
    ~int_rect() {}
    }; // End of struct int_rect.

/*------------------------------------------------------------------------------
A real rectangle is used to represent a portion of the earth. I.e. it
represents a rectangle in modelled space, not the screen.
------------------------------------------------------------------------------*/
//----------------------//
//      real_rect::     //
//----------------------//
struct real_rect { // A rectangle with real coordinates.
    double x, y, w, h;

    void set(int xx, int yy, uint ww, uint hh)
        { x = xx; y = yy; w = ww; h = hh; }

    // Set the rectangle acording to given vertices:
    void set_minmax(double min_x, double max_x, double min_y, double max_y) {
        x = min_x; y = min_y; w = max_x - min_x; h = max_y - min_y; }

    void positivize();  // Convert so that w >= 0 and h >= 0.

    // Scale the rectangle by ratio s:
    void wide(double s) { x -= w*(s-1)/2; y -= h*(s-1)/2; w *= s; h *= s; }

    // Scale the rectangle by ratio 1/s:
    void zoom(double s) {
        if (s != 0) { w /= s; h /= s; x += w*(s-1)/2; y += h*(s-1)/2; }}

    // Widen/heighten a window by ratios s or 1/s:
    void w_wide(double s) { x -= w*(s-1)/2; w *= s; }
    void h_wide(double s) { y -= h*(s-1)/2; h *= s; }
    void w_zoom(double s) { if (s != 0) { w /= s; x += w*(s-1)/2; }}
    void h_zoom(double s) { if (s != 0) { h /= s; y += h*(s-1)/2; }}

    void scale(double s) { wide(s); }
    void w_scale(double s) { w_wide(s); }
    void h_scale(double s) { h_wide(s); }

    // Relative coords of a point within the rectangle:
    position2* view(position2*, real_rect* = 0);

    real_rect& operator=(real_rect& r)
        { x = r.x; y = r.y; w = r.w; h = r.h; return *this; }
    real_rect() { x = y = w = h = 0; }
    ~real_rect() {}
    }; // End of struct real_rect.

//----------------------//
//       pt_int16::     //
//----------------------//
struct pt_int16 {
    int16 x;
    int16 y;

//    pt_int16& operator=(const pt_int16& x) {}
//    pt_int16(const pt_int16& x) {}
    pt_int16(int16 xx, int16 yy) { x = xx; y = yy; }
    pt_int16() { x = y = 0; }
    ~pt_int16() {}
    }; // End of struct pt_int16.

/*------------------------------------------------------------------------------
This represents a rectangle with int16 left/right/top/bottom coords.
------------------------------------------------------------------------------*/
//----------------------//
//      rect_int16::    //
//----------------------//
struct rect_int16 {
    int16 xmin;
    int16 ymin;
    int16 xmax;
    int16 ymax;

    int cuts(const pt_int16& pt0, int16 kx, int16 ky,
                   pt_int16& pt1, pt_int16& pt2) const;

//    rect_int16& operator=(const rect_int16& x) {}
//    rect_int16(const rect_int16& x) {}
    rect_int16(int16 x1, int16 y1, int16 x2, int16 y2)
        { xmin = x1; ymin = y1; xmax = x2; ymax = y2; }
    rect_int16() { xmin = ymin = xmax = ymax = 0; }
    ~rect_int16() {}
    }; // End of struct rect_int16.

#endif /* AKSL_GEOM2_H */
