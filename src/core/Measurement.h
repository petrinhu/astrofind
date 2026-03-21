#pragma once

#include <QString>
#include <QVector>

namespace core {

/// Comet photometry note code (ADES 2022 §4.4).
/// None = not a comet / not applicable.
/// Nuclear = nucleus only; Total = coma+nucleus; Coma = coma only.
enum class CometMagNote { None, Nuclear, Total, Coma };

/// One astrometric + photometric observation of a moving object.
/// Maps directly to one row in the ADES 2022 optical observation record.
struct Observation {
    // ── Identification ──────────────────────────────────────────────────────
    QString objectName;          ///< Name/designation (e.g. "(433) Eros")
    QString objectType;          ///< "asteroid", "comet", "unknown"
    int     mpcNumber   = 0;     ///< MPC packed number (0 = unnumbered)

    // ── Astrometry ──────────────────────────────────────────────────────────
    double  ra          = 0.0;   ///< Right ascension (degrees J2000)
    double  dec         = 0.0;   ///< Declination (degrees J2000)
    double  raErr       = 0.0;   ///< Formal RA uncertainty (arcsec)
    double  decErr      = 0.0;   ///< Formal Dec uncertainty (arcsec)
    double  jd          = 0.0;   ///< Julian Date (mid-exposure)

    // ── Pixel position ──────────────────────────────────────────────────────
    double  pixX        = 0.0;   ///< Sub-pixel centroid X (0-based)
    double  pixY        = 0.0;   ///< Sub-pixel centroid Y (0-based)

    // ── Photometry ──────────────────────────────────────────────────────────
    double  mag         = 99.0;  ///< Calibrated magnitude
    double  magErr      = 0.0;   ///< Formal magnitude uncertainty
    QString magBand;             ///< Filter band: "V","R","B","C","w","g","r","i"

    // ── PSF quality ─────────────────────────────────────────────────────────
    double  fwhm        = 0.0;   ///< FWHM (arcsec)
    double  snr         = 0.0;   ///< Signal-to-noise ratio

    // ── Atmospheric correction ───────────────────────────────────────────────
    double  airmass     = 0.0;   ///< Airmass X (0 = not computed / unknown)

    // ── Comet-specific ───────────────────────────────────────────────────────
    CometMagNote cometNote = CometMagNote::None;  ///< Photometry note for comet obs

    // ── Source image ────────────────────────────────────────────────────────
    QString imagePath;
    int     imageIndex  = -1;    ///< Index in the session image list
};

} // namespace core
