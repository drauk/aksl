// src/aksl/geom2.c   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------
Functions in this file:

dms2deg
E_dist2
// E_dist
E_dist2_lin
E_dist_lin
// translate

int_rect::
    print
real_rect::
    positivize
    view
rect_int16::

------------------------------------------------------------------------------*/

#include "aksl/geom2.h"

/*------------------------------------------------------------------------------
dms2deg() converts an integer in the form d*10000 + m*100 + s to degrees.
------------------------------------------------------------------------------*/
//----------------------//
//        dms2deg       //
//----------------------//
double dms2deg(long i) {
    double r = (i%100);
    i /= 100;
    r /= 60;
    r += (i%100);
    i /= 100;
    r /= 60;
    return r + i;
    } // End of function dms2deg.

/*------------------------------------------------------------------------------
Function E_dist2() calculates distance between two points on the earth,
given the coordinates of the two points, and the radius of the earth
(a "global" constant). Distances in metres, angles in radians!
The formula is (probably):

d = radius_E * sqrt(2*(1 - cos(theta1 - theta0)
    + (1 - cos(phi1 - phi0))*cos(theta0)*cos(theta1)));
------------------------------------------------------------------------------*/
//----------------------//
//       E_dist2        //
//----------------------//
double E_dist2(double phi0, double theta0, double phi1, double theta1) {
    register double t0 = (1 - cos(phi1 - phi0))*cos(theta1)*cos(theta0);
    register double t1 = 1 - cos(theta1 - theta0);
    t0 += t1;
    t0 += t0;
    return R_earth_mean_2 * t0;
    } // End of function E_dist2.

/*------------------------------------------------------------------------------
Function E_dist() calculates distance between two points on the earth,
given the coordinates of the two points, and the radius of the earth
(a "global" constant). Distances in metres, angles in radians!
The formula is (probably):

d = radius_E * sqrt(2*(1 - cos(theta1 - theta0)
    + (1 - cos(phi1 - phi0))*cos(theta0)*cos(theta1)));
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
This is commented out because there is an inline function E_dist() which simply
invokes E_dist2, and which therefore avoids duplication of code.
------------------------------------------------------------------------------*/
//----------------------//
//        E_dist        //
//----------------------//
/*------------------------------------------------------------------------------
double E_dist(double phi0, double theta0, double phi1, double theta1) {
    register double t0 = (1 - cos(phi1 - phi0))*cos(theta1)*cos(theta0);
    register double t1 = 1 - cos(theta1 - theta0);
    t0 += t1;
    t0 += t0;
    return R_earth_mean * sqrt(t0);
    } // End of function E_dist.
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
Function E_dist2_lin() calculates the approximate distance between two points
on the earth, given the coordinates of the two points, and the radius of the
earth (a "global" constant). Distances in metres, angles in radians!
The formula for the linearised distance is (probably):

d = radius_E *
    sqrt((cos((theta0 + theta1)/2)*(phi1 - phi0))^2 + (theta1 - theta0)^2);
------------------------------------------------------------------------------*/
//----------------------//
//      E_dist2_lin     //
//----------------------//
double E_dist2_lin(double phi0, double theta0, double phi1, double theta1) {
    register double t0 = (phi1 - phi0)*cos((theta0 + theta1)/2);
    register double t1 = theta1 - theta0;
    return R_earth_mean_2 * (t0*t0 + t1*t1);
    } // End of function E_dist2_lin.

//----------------------//
//      E_dist_lin      //
//----------------------//
double E_dist_lin(double phi0, double theta0, double phi1, double theta1) {
    register double t0 = (phi1 - phi0)*cos((theta0 + theta1)/2);
    register double t1 = theta1 - theta0;
    return R_earth_mean * hypot(t0, t1);
    } // End of function E_dist_lin.

/*------------------------------------------------------------------------------
Function translate() calculates the new coordinates of a point, given that
it has moved a given distance in a given direction on the earth.
Distances in metres, angles in radians! The bearing alpha is measured with east
as 0 radians, and north as PI/2 radians.
------------------------------------------------------------------------------*/
/*
//----------------------//
//      translate       //
//----------------------//
void translate(double phi0, double theta0, double delta, double alpha,
        double& phi1, double& theta1) {

    } // End of function translate.
*/

//----------------------//
//    int_rect::print   //
//----------------------//
void int_rect::print(ostream& os) {
    os << "(x, y, w, h) = (" << x << ", " << y
       << ", " << w << ", " << h << ")\n";
    } // End of function int_rect::print.

//--------------------------//
//   real_rect::positivize  //
//--------------------------//
void real_rect::positivize() {
    if (w < 0) {
        x += w;
        w = -w;
        }
    if (h < 0) {
        y += h;
        h = -h;
        }
    } // End of function real_rect::positivize.

/*------------------------------------------------------------------------------
This function returns a "position2" structure if the given point lies within the
real rectangle. If the rectangle has zero width or height and the point lies in
the zero-area region, then it is still considered to be in the region.
If w or h is negative, then the region is considered to be empty.
If the rectangle parameter is non-null, the returned point is scaled to lie in
that rectangle. Otherwise it lies in [0,1] x [0,1].
------------------------------------------------------------------------------*/
//----------------------//
//    real_rect::view   //
//----------------------//
position2* real_rect::view(position2* pp, real_rect* rr) {
    if (!pp)
        return 0;
    if (pp->x < x || pp->x > x + w || pp->y < y || pp->y > y + h)
        return 0;
    double x_view = pp->x - x;
    double y_view = pp->y - y;
    if (w == 0)
        x_view = 0;
    else
        x_view /= w;
    if (h == 0)
        y_view = 0;
    else
        y_view /= h;
    if (rr) {
        x_view *= rr->w;
        x_view += rr->x;
        y_view *= rr->h;
        y_view += rr->y;
        }
    position2* pp2 = new position2(x_view, y_view);
    return pp2;
    } // End of function real_rect::view.

/*------------------------------------------------------------------------------
This returns the coords of the intersections of a given line with
the rectangle. This is intended for use in drawing the line segment which is
created by the intersection of the line with the rectangle.
If there is no intersection, then return a negative number.
If there is an intersection, then 0 is returned, and the intersection
points are returned in pt1 and pt2.
This function tries to avoid many kinds of integer overflow conditions.
The line pt1 to pt2 always goes from left to right, except that a vertical line
goes from bottom to top.
------------------------------------------------------------------------------*/
//----------------------//
//   rect_int16::cuts   //
//----------------------//
int rect_int16::cuts(const pt_int16& pt0, int16 kx, int16 ky,
                     pt_int16& pt1, pt_int16& pt2) const {
    // The case of a horizontal line.
    if (kx == 0) {
        if (pt0.y < ymin)
            return -1;
        if (pt0.y > ymax)
            return -2;
        pt1.x = xmin;   pt1.y = pt0.y;
        pt2.x = xmax;   pt2.y = pt0.y;
        return 0;
        }

    // The case of a vertical line.
    if (ky == 0) {
        if (pt0.x < xmin)
            return -3;
        if (pt0.x > xmax)
            return -4;
        pt1.x = pt0.x;  pt1.y = ymin;
        pt2.x = pt0.x;  pt2.y = ymax;
        return 0;
        }

    // Normalize the slope vector.
    if (kx < 0) {
        kx = -kx;
        ky = -ky;
        }

    // The case of positive slope.
    if (ky > 0) {
        // Find intersection with bottom edge.
        // (Note excessively zealous casting.)
        int32 x1 = (int32)pt0.x
                 + (((int32)ymin - pt0.y) * (int32)ky)/(int32)kx;
        if (x1 > (int32)xmax)
            return -5;

        // Find intersection with top edge.
        int32 x2 = (int32)pt0.x
                 + (((int32)ymax - pt0.y) * (int32)ky)/(int32)kx;
        if (x2 < (int32)xmin)
            return -6;

        // At this point, the line definitely intersects the rectangle.
        if (x1 >= (int32)xmin) {
            // Intersects the bottom edge.
            if (x2 <= (int32)xmax) {
                // Intersects the top edge too.
                pt1.x = (int16)x1;  pt1.y = ymin;
                pt2.x = (int16)x2;  pt2.y = ymax;
                return 0;
                }
            // Intersects the right edge.
            int32 y2 = (int32)pt0.y
                     + (((int32)xmax - pt0.x) * (int32)kx)/(int32)ky;
            pt1.x = (int16)x1;  pt1.y = ymin;
            pt2.x = xmax;       pt2.y = (int16)y2;
            return 0;
            }
        // Intersects the left edge. (x1 < (int32)xmin.)
        int32 y1 = (int32)pt0.y
                 + (((int32)xmin - pt0.x) * (int32)kx)/(int32)ky;
        if (x2 <= (int32)xmax) {
            // Intersects the top edge.
            pt1.x = xmin;       pt1.y = (int16)y1;
            pt2.x = (int16)x2;  pt2.y = ymax;
            return 0;
            }
        // Intersects the right edge.
        int32 y2 = (int32)pt0.y
                 + (((int32)xmax - pt0.x) * (int32)kx)/(int32)ky;
        pt1.x = xmin;       pt1.y = (int16)y1;
        pt2.x = xmax;       pt2.y = (int16)y2;
        return 0;
        }

    // The case of negative slope.
    // Find intersection with bottom edge.
    int32 x1 = (int32)pt0.x
             + (((int32)ymin - pt0.y) * (int32)ky)/(int32)kx;
    if (x1 < (int32)xmin)
        return -7;

    // Find intersection with top edge.
    int32 x2 = (int32)pt0.x
             + (((int32)ymax - pt0.y) * (int32)ky)/(int32)kx;
    if (x2 > (int32)xmax)
        return -8;

    // At this point, the line definitely intersects the rectangle.
    if (x1 <= (int32)xmax) {
        // Intersects the bottom edge.
        if (x2 >= (int32)xmin) {
            // Intersects the top edge too.
            pt1.x = (int16)x2;  pt1.y = ymax;
            pt2.x = (int16)x1;  pt2.y = ymin;
            return 0;
            }
        // Intersects the left edge.
        int32 y2 = (int32)pt0.y
                 + (((int32)xmin - pt0.x) * (int32)kx)/(int32)ky;
        pt1.x = xmin;       pt1.y = (int16)y2;
        pt2.x = (int16)x1;  pt2.y = ymin;
        return 0;
        }
    // Intersects the right edge. (x1 > (int32)xmax.)
    int32 y1 = (int32)pt0.y
             + (((int32)xmax - pt0.x) * (int32)kx)/(int32)ky;
    if (x2 >= (int32)xmin) {
        // Intersects the top edge.
        pt1.x = (int16)x2;  pt1.y = ymax;
        pt2.x = xmax;       pt2.y = (int16)y1;
        return 0;
        }
    // Intersects the left edge.
    int32 y2 = (int32)pt0.y
             + (((int32)xmin - pt0.x) * (int32)kx)/(int32)ky;
    pt1.x = xmin;       pt1.y = (int16)y2;
    pt2.x = xmax;       pt2.y = (int16)y1;
    return 0;
    } // End of function rect_int16::cuts.
