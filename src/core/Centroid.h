#pragma once

#include "FitsImage.h"

#include <optional>

namespace core {

struct CentroidResult {
    double x     = 0.0;   ///< Sub-pixel centroid X (0-based image coords)
    double y     = 0.0;   ///< Sub-pixel centroid Y
    double fwhmX = 0.0;   ///< FWHM along X axis / major axis (pixels)
    double fwhmY = 0.0;   ///< FWHM along Y axis / minor axis (pixels)
    double theta = 0.0;   ///< Angle of major axis (degrees, 0=+X, CCW positive).
                          ///  Always 0 for symmetric fits; set by findCentroidElliptical.
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

/// Fits a fully elliptical 2-D Gaussian (6 parameters: cx, cy, σ_a, σ_b,
/// rotation angle θ, amplitude) using Levenberg-Marquardt.
///
/// Returns fwhmX = 2.355·σ_a (major axis), fwhmY = 2.355·σ_b (minor axis),
/// and theta = angle of the major axis in degrees (CCW from +X).
/// Useful for measuring true FWHM, elongation (coma, tracking error, defocus),
/// and position angle of the PSF.
///
/// Falls back to findCentroidPsf if LM does not converge.
std::optional<CentroidResult> findCentroidElliptical(const FitsImage& img,
                                                     double clickX,
                                                     double clickY,
                                                     int    boxRadius = 12);

} // namespace core
