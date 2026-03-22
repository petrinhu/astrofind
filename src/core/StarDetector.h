#pragma once

#include "FitsImage.h"
#include "Overlay.h"

#include <QVector>
#include <QString>
#include <expected>

namespace core {

struct StarDetectorConfig {
    double threshold           = 3.0;   ///< Detection threshold (sigma above background)
    double minArea             = 5.0;   ///< Minimum connected pixels per object
    double deblendContrast     = 0.005; ///< SEP deblend contrast ratio
    int    deblendNthresh      = 32;    ///< SEP deblend N-threshold levels
    bool   filterKernel        = true;  ///< Apply 3×3 matched filter before extraction
    int    maxStars            = 500;   ///< Maximum stars returned (brightest by flux)
    /// Elongation a/b threshold above which a source is flagged as a streak/trail.
    /// 0.0 disables streak flagging. Default 3.0 catches fast asteroids and satellites.
    double streakMinElongation = 3.0;

    /// Enable multi-peak blended-source detection (ClumpFind pass).
    /// For each source, scans local maxima within its isophote; marks blended=true
    /// when ≥2 peaks are found more than blendPeakSepFraction*a apart.
    bool   detectBlended          = true;
    /// Minimum separation between two peaks (as a fraction of the semi-major axis)
    /// below which they are treated as the same peak (noise suppression).
    double blendPeakSepFraction   = 0.5;
};

/// Detect stars in a FitsImage using SEP (Source Extractor as C library).
/// Returns detected stars sorted by flux descending, or an error string.
std::expected<QVector<DetectedStar>, QString>
detectStars(const FitsImage& img, const StarDetectorConfig& cfg = {});

} // namespace core
