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

/// Apply atmospheric refraction correction to sky coordinates.
///
/// Converts an **apparent** position (as seen through the atmosphere) to the
/// true/catalog (ICRS) position by removing the refraction shift.
/// Uses the Bennett (1982) formula: R = 1.02 / tan(h + 10.3/(h+5.11)) arcmin.
/// The correction is applied through the alt-az frame and is exact for standard
/// atmosphere (T=10°C, P=1010 hPa); residual error < 0.1" above 20° altitude.
///
/// No-op if the object is below 1° altitude, site coordinates are non-finite,
/// or the caller is a space telescope (no atmosphere).
///
/// @param ra    Apparent RA (degrees), corrected in-place.
/// @param dec   Apparent Dec (degrees), corrected in-place.
/// @param jd    Julian Date (mid-exposure, UT1≈TT).
/// @param lat   Observer geodetic latitude (degrees, North positive).
/// @param lon   Observer geodetic longitude (degrees, East positive).
/// @returns Refraction correction R (arcmin, ≥ 0). Returns 0.0 when no-op.
double applyRefractionCorrection(double& ra, double& dec,
                                 double jd, double lat, double lon) noexcept;

/// Propagate a catalog star position to a target epoch using linear proper motion.
///
/// @param star      CatalogStar with ra, dec (degrees), pmRA (mas/yr · cos δ),
///                  pmDec (mas/yr), and refEpochJd.
/// @param targetJd  Target Julian Date (e.g. mid-exposure JD of the image).
/// @returns {ra_corrected, dec_corrected} in degrees.
///          Returns original coordinates unchanged if pmRA == pmDec == 0.
std::pair<double,double> applyProperMotion(const CatalogStar& star,
                                            double targetJd) noexcept;

// ─── Coordinate frame conversions ────────────────────────────────────────────

/// Convert ecliptic to equatorial coordinates (J2000.0).
/// Uses the mean obliquity ε = 23.4392911° (IAU J2000.0 value).
/// @param lambda_deg  Ecliptic longitude (degrees).
/// @param beta_deg    Ecliptic latitude  (degrees).
/// @param ra_deg      Output RA (degrees, 0–360).
/// @param dec_deg     Output Dec (degrees, −90 to +90).
void eclipticToEquatorial(double lambda_deg, double beta_deg,
                          double& ra_deg, double& dec_deg) noexcept;

/// Convert equatorial J2000.0 to galactic coordinates (IAU 1958 system, J2000 poles).
/// Galactic north pole: αNGP = 192.859508°, δNGP = 27.128336°; l_NCP = 122.932°.
/// @param ra_deg   Input RA  (degrees, J2000).
/// @param dec_deg  Input Dec (degrees, J2000).
/// @param l_deg    Output galactic longitude l (degrees, 0–360).
/// @param b_deg    Output galactic latitude  b (degrees, −90 to +90).
void equatorialToGalactic(double ra_deg, double dec_deg,
                          double& l_deg, double& b_deg) noexcept;

// ─── ICRS → CIRS → Topocentric chain ─────────────────────────────────────────

/// Low-precision geocentric ecliptic longitude and mean obliquity of the Sun.
/// Accuracy: ~0.01° (36"); adequate for aberration and nutation calculations.
/// Uses the truncated VSOP87 series (Meeus, Ch. 25).
/// @returns {ecliptic_longitude_deg, mean_obliquity_deg}
std::pair<double,double> sunEclipticPosition(double jd) noexcept;

/// Compute annual stellar aberration components (do not apply).
///
/// Calculates the shift (Δα, Δδ) that transforms astrometric (catalog) ICRS
/// coordinates to geocentric apparent coordinates due to Earth's heliocentric
/// velocity.  Uses the classical first-order Meeus (1998) formula, eq. 23.3.
///
/// **When to call:**
///   - For logging/diagnostics: magnitude of annual aberration at the epoch.
///   - To apply aberration: add dRa_as/3600 and dDec_as/3600 to astrometric coords.
///   - To remove aberration: subtract (apparent → astrometric).
///
/// **Note:** A WCS plate solution calibrated against an ICRS catalog (Gaia,
/// UCAC4, 2MASS) already absorbs annual aberration as a field-wide systematic.
/// Applying this correction a second time would double-correct.
///
/// @param ra       Object RA (degrees, ICRS astrometric).
/// @param dec      Object Dec (degrees, ICRS astrometric).
/// @param jd       Julian Date of observation.
/// @param dRa_as   Output: Δα correction in arcseconds (positive = East).
/// @param dDec_as  Output: Δδ correction in arcseconds (positive = North).
/// @returns Total aberration magnitude in arcseconds.
double annualAberrationComponents(double ra, double dec, double jd,
                                  double& dRa_as, double& dDec_as) noexcept;

/// Apply simplified IAU 2006 precession (Lieske 1977 rotation angles) from
/// J2000.0 mean equinox to the mean equinox of @p jd.
///
/// Use this when input coordinates are in the FK5/J2000.0 mean equinox frame
/// (e.g. catalog epochs before Hipparcos/Gaia) and must be converted to
/// approximate ICRS / coordinates of date.
///
/// Accuracy: < 0.1" per century from J2000.0.  No nutation included.
///
/// @param ra   J2000.0 RA (degrees), updated in-place.
/// @param dec  J2000.0 Dec (degrees), updated in-place.
/// @param jd   Target Julian Date (defines the epoch of date).
void applyPrecessionJ2000ToDate(double& ra, double& dec, double jd) noexcept;

/// Apply dominant nutation terms (9-term IAU 1980 series) to convert
/// mean-equinox-of-date coordinates to the true (apparent) equinox of date.
///
/// Magnitude can reach 17.2" in Δψ (longitude) and 9.2" in Δε (obliquity).
/// Accuracy: ~0.5" (adequate for sub-arcsecond ADES residuals).
/// Call AFTER applyPrecessionJ2000ToDate() when building apparent place from
/// historical J2000 mean-equinox coordinates.
///
/// @param ra   Mean-equinox-of-date RA (degrees), updated in-place.
/// @param dec  Mean-equinox-of-date Dec (degrees), updated in-place.
/// @param jd   Julian Date.
void applyNutation(double& ra, double& dec, double jd) noexcept;

} // namespace core
