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
    double theta    = 0.0;   ///< Position angle (radians, E of N)
    double snr      = 0.0;   ///< Signal-to-noise ratio
    int    flag     = 0;     ///< SEP extraction flags

    // Filled after plate solve:
    double ra       = 0.0;   ///< RA (degrees J2000)
    double dec      = 0.0;   ///< Dec (degrees J2000)
    bool   matched  = false; ///< Matched to a catalog star?

    double fwhm() const noexcept { return 2.355 * (a + b) * 0.5; }
};

/// A reference star from an astrometric catalog (UCAC4)
struct CatalogStar {
    QString id;
    double  ra    = 0.0;   ///< Right Ascension (degrees J2000)
    double  dec   = 0.0;   ///< Declination (degrees J2000)
    double  mag   = 99.0;  ///< Visual magnitude
    double  pmRA  = 0.0;   ///< Proper motion RA (mas/yr)
    double  pmDec = 0.0;   ///< Proper motion Dec (mas/yr)
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
