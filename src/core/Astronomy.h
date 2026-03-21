#pragma once

#include "Overlay.h"   // CatalogStar

#include <utility>

namespace core {

/// Greenwich Mean Sidereal Time (degrees, 0–360) for a given Julian Date.
/// Uses the IAU 1982 formula: accurate to ~0.1 s over ±1 century from J2000.
/// @param jd  Julian Date (UT1 ≈ TT for typical amateur use).
double greenwichMeanSiderealTime(double jd) noexcept;

/// Local Sidereal Time (degrees, 0–360) for a given Julian Date and longitude.
/// @param jd      Julian Date (UT1).
/// @param lonDeg  Observer geodetic longitude (degrees, East positive).
double localSiderealTime(double jd, double lonDeg) noexcept;

/// Airmass using the Pickering (2002) formula.
/// Accurate to 0.1 % down to altitude = 1°.
/// @param ra_deg   Object Right Ascension (degrees J2000).
/// @param dec_deg  Object Declination (degrees J2000).
/// @param jd       Julian Date of the observation (mid-exposure, UT1≈TT).
/// @param lat_deg  Observer geodetic latitude (degrees, North positive).
/// @param lon_deg  Observer geodetic longitude (degrees, East positive).
/// @returns Airmass X (≥ 1.0), or 0.0 if the object is below the horizon.
double computeAirmass(double ra_deg, double dec_deg, double jd,
                      double lat_deg, double lon_deg) noexcept;

/// Great-circle angular distance between two sky positions (degrees).
/// Uses the haversine formula — numerically stable at any separation.
/// @returns Distance in degrees, always in [0, 180].
double angularDistance(double ra1_deg, double dec1_deg,
                       double ra2_deg, double dec2_deg) noexcept;

/// Propagate a catalog star position to a target epoch using linear proper motion.
///
/// @param star      CatalogStar with ra, dec (degrees), pmRA (mas/yr · cos δ),
///                  pmDec (mas/yr), and refEpochJd.
/// @param targetJd  Target Julian Date (e.g. mid-exposure JD of the image).
/// @returns {ra_corrected, dec_corrected} in degrees.
///          Returns original coordinates unchanged if pmRA == pmDec == 0.
std::pair<double,double> applyProperMotion(const CatalogStar& star,
                                            double targetJd) noexcept;

} // namespace core
