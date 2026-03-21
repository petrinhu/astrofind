#pragma once

#include "FitsImage.h"

#include <QString>

namespace core {

/// Result of a session comparison (image subtraction).
struct CompareResult {
    FitsImage residual;      ///< new − aligned_ref; detectedStars holds residual sources
    bool      usedWcs = false; ///< true = WCS warp; false = direct subtraction
    int       detectionCount = 0;
    QString   error;         ///< non-empty on failure
};

/// Subtract @p refImg from @p newImg and detect residual sources.
///
/// Alignment strategy (in order of preference):
///   1. Both images have solved WCS  → warp refImg onto newImg's pixel grid
///      using bilinear interpolation via SkyToPix / PixToSky.
///   2. No WCS available             → direct subtraction (user must ensure
///      images are already registered).
///
/// After subtraction the residual is sigma-clipped and sources brighter than
/// @p sigmaThreshold × σ are detected via 4-connected flood fill.
/// Detections are stored in @p result.residual.detectedStars so they render
/// automatically in FitsImageView.
///
/// Designed to run in a background thread (no Qt UI calls).
CompareResult compareImages(const FitsImage& newImg,
                             const FitsImage& refImg,
                             float sigmaThreshold = 4.0f);

} // namespace core
