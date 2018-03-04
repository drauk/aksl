// src/aksl/phys.h   2017-10-25   Alan U. Kennington.
/*-----------------------------------------------------------------------------
Copyright (C) 1989-2018, Alan U. Kennington.
You may distribute this software under the terms of Alan U. Kennington's
modified Artistic Licence, as specified in the accompanying LICENCE file.
-----------------------------------------------------------------------------*/
#ifndef AKSL_PHYS_H
#define AKSL_PHYS_H
/*------------------------------------------------------------------------------
This file contains physical constants and similar things.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Classes in this file:

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
The recommended units for AKSL software are MKS.
The recommended units of angles are radians.
The recommended order of geographical coordinates is (East, North). !!!
------------------------------------------------------------------------------*/

// Constants taken from 1986 CODATA rec. values of fund phys consts.
// NIST at http://physics.nist.gov/PhysRefData/codata86/codata86.html.

// Speed of light in vacuum.
const double c_light        = 299792458;                    // m s^-1.

// Newtonian constant G for gravitation.
const double G_newton       = 6.6725985e-11;                // m^3 kg^-1 s^-2

// Planck's constants h and h_bar.
const double h_planck       = 6.62607554e-34;               // J s.
const double h_bar_planck   = 1.0545726663e-34;             // J s.

//------------------------------------------------------------------------------
// Geography:

// Average radius of Earth. (Equatorial 6378 km. Polar 6357 km.)
const double R_earth_mean   = 6368000;                      // Metres.
const double R_earth_mean_2 = R_earth_mean * R_earth_mean;  // Metres^2.

// SI/imperial conversions:
const double kg_per_pound       = 0.45359237;
const double pound_per_kg       = 1/kg_per_pound;
const double calorie_per_joule  = 4.1868;
const double joule_per_calorie  = 1/calorie_per_joule;
const double mile_per_km        = 1.609344;
const double km_per_mile        = 1/mile_per_km;
const double nmile_per_km       = 1.852;                    // Nautical mile.
const double km_per_nmile       = 1/nmile_per_km;

#endif /* AKSL_PHYS_H */
