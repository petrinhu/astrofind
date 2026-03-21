#pragma once

#include "FitsImage.h"

#include <optional>

namespace core {

struct CentroidResult {
    double x     = 0.0;   ///< Sub-pixel centroid X (0-based image coords)
    double y     = 0.0;   ///< Sub-pixel centroid Y
    double fwhmX = 0.0;   ///< FWHM along X (pixels)
    double fwhmY = 0.0;   ///< FWHM along Y (pixels)
    double peak  = 0.0;   ///< Peak pixel value above background
    double snr   = 0.0;   ///< Signal-to-noise estimate
};

/// Refine a click position to a sub-pixel centroid using intensity-weighted
/// moments in a box centred on (clickX, clickY).
///
/// @param img       Source FITS image.
/// @param clickX    Initial X guess (image pixel, 0-based).
/// @param clickY    Initial Y guess.
/// @param boxRadius Half-size of the search box (pixels). Default 12.
/// @returns CentroidResult on success, nullopt if the box is empty or
///          the source is saturated / off-edge.
std::optional<CentroidResult> findCentroid(const FitsImage& img,
                                           double clickX,
                                           double clickY,
                                           int    boxRadius = 12);

/// Same as findCentroid but fits a symmetric 2-D Gaussian iteratively
/// (gradient descent, up to 40 iterations) for more accurate sub-pixel
/// positions and FWHM on under-sampled PSFs.
/// Falls back to findCentroid if the fit does not converge.
std::optional<CentroidResult> findCentroidPsf(const FitsImage& img,
                                              double clickX,
                                              double clickY,
                                              int    boxRadius = 12);

} // namespace core
