#pragma once

#include "MpcOrb.h"
#include "Overlay.h"

#include <QVector>
#include <QString>
#include <functional>

namespace core {

/// Result of an ephemeris computation for a single asteroid at a given JD.
struct EphemerisMatch {
    // Source
    int     number  = 0;
    QString name;
    float   H       = 99.0f;

    // Predicted sky position (J2000, degrees)
    double  ra      = 0.0;
    double  dec     = 0.0;

    // Distances
    double  rHelio  = 0.0;   ///< Heliocentric distance (AU)
    double  rGeo    = 0.0;   ///< Geocentric distance (AU)

    // Predicted apparent magnitude (H-G system)
    double  mag     = 99.0;

    // Sky-plane motion (arcsec/hr)
    double  dRA     = 0.0;
    double  dDec    = 0.0;

    // Phase angle (degrees)
    double  phaseAngle = 0.0;
};

/// Solve Kepler's equation  M = E - e·sin(E)  for the eccentric anomaly E.
/// @param M_rad  Mean anomaly in radians.
/// @param e      Orbital eccentricity (0 ≤ e < 1).
double solveKepler(double M_rad, double e, int maxIter = 50) noexcept;

/// Compute geocentric (optionally topocentric) ephemeris for one asteroid.
/// @param orb     Orbital elements from MPCORB.
/// @param jd      Julian Date of interest.
/// @param obsLat  Observer geodetic latitude  (degrees, +N).
/// @param obsLon  Observer geodetic longitude (degrees, +E).
/// @param obsAlt  Observer altitude above sea level (meters).
EphemerisMatch computeEphemeris(const AsteroidRecord& orb,
                                double jd,
                                double obsLat = 0.0,
                                double obsLon = 0.0,
                                double obsAlt = 0.0) noexcept;

/// Scan a MPCORB.DAT file and return all asteroids whose predicted position
/// falls within @p radiusDeg of the given field centre at @p jd.
///
/// @param mpcOrbPath  Path to MPCORB.DAT.
/// @param ra          Field centre RA  (degrees J2000).
/// @param dec         Field centre Dec (degrees J2000).
/// @param radiusDeg   Search radius (degrees).
/// @param jd          Julian Date.
/// @param obsLat/Lon/Alt  Observer location (degrees / meters).
/// @param progress    Optional callback(linesRead, totalLines) for UI feedback.
/// @param cancelled   Pointer to a bool that, if set to true, aborts the scan.
QVector<EphemerisMatch> queryFieldMpcOrb(
    const QString& mpcOrbPath,
    double ra, double dec, double radiusDeg,
    double jd,
    double obsLat = 0.0, double obsLon = 0.0, double obsAlt = 0.0,
    const std::function<void(int,int)>& progress = nullptr,
    const bool* cancelled = nullptr);

/// Convert an EphemerisMatch to a KooObject so it can be shown in the existing overlay.
KooObject ephemerisMatchToKoo(const EphemerisMatch& m);

} // namespace core
