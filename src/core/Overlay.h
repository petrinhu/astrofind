#pragma once

#include <QString>
#include <QVector>
#include <cmath>

namespace core {

/// A star detected by SEP (Source Extractor)
struct DetectedStar {
    double x        = 0.0;   ///< Centroid X (0-based image pixels)
    double y        = 0.0;   ///< Centroid Y
    double flux     = 0.0;   ///< Isophotal flux (ADU)
    double a        = 0.0;   ///< Semi-major axis (pixels)
    double b        = 0.0;   ///< Semi-minor axis (pixels)
    double theta    = 0.0;   ///< Position angle (radians, CCW from +x axis in image coords)
    double snr      = 0.0;   ///< Signal-to-noise ratio
    int    flag     = 0;     ///< SEP extraction flags

    /// True when elongation (a/b) exceeds the configured streak threshold.
    /// Indicates a trailed source: fast asteroid, satellite, or tracking smear.
    bool   streak   = false;

    /// True when the source contains ≥2 distinct flux peaks within its isophote,
    /// indicating two or more overlapping objects that were not fully deblended.
    bool   blended  = false;

    // Filled after plate solve:
    double ra       = 0.0;   ///< RA (degrees J2000)
    double dec      = 0.0;   ///< Dec (degrees J2000)
    bool   matched  = false; ///< Matched to a catalog star?

    double fwhm()       const noexcept { return 2.355 * (a + b) * 0.5; }
    double elongation() const noexcept { return (b > 0.1) ? a / b : 1.0; }
};

/// A reference star from an astrometric catalog (UCAC4 / Gaia DR3)
struct CatalogStar {
    QString id;
    double  ra           = 0.0;        ///< Right Ascension (degrees) at refEpochJd
    double  dec          = 0.0;        ///< Declination (degrees) at refEpochJd
    double  mag          = 99.0;       ///< Visual magnitude
    double  pmRA         = 0.0;        ///< Proper motion in RA·cos(Dec) (mas/yr)
    double  pmDec        = 0.0;        ///< Proper motion in Dec (mas/yr)
    double  refEpochJd   = 2451545.0;  ///< Catalog reference epoch (J2000.0 default)
};

/// A known solar system object (from IMCCE SkyBoT)
struct KooObject {
    QString name;
    QString type;           ///< "asteroid", "comet", "planet", etc.
    double  ra    = 0.0;   ///< Predicted RA (degrees J2000)
    double  dec   = 0.0;   ///< Predicted Dec (degrees J2000)
    double  mag   = 99.0;  ///< Predicted magnitude
    double  dRA   = 0.0;   ///< Proper motion RA (arcsec/hr)
    double  dDec  = 0.0;   ///< Proper motion Dec (arcsec/hr)
    int     number= 0;     ///< MPC number (0 if unnumbered/comet)
};

} // namespace core
