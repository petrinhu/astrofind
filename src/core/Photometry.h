#pragma once

#include "FitsImage.h"
#include "Astronomy.h"   // applyProperMotion, computeAirmass — re-exported for callers

#include <optional>

namespace core {

struct ApertureResult {
    double flux     = 0.0;   ///< Net flux (ADU) after background subtraction
    double bkg      = 0.0;   ///< Background level per pixel
    double bkgSigma = 0.0;   ///< Background RMS (used for uncertainty)
    double magInst  = 99.0;  ///< Instrumental magnitude = -2.5·log10(flux)
    double magErr   = 0.0;   ///< Formal photometric uncertainty
    int    nPix     = 0;     ///< Pixels in aperture
};

/// Circular aperture photometry at (cx, cy).
///
/// Background is estimated in an annulus [rIn, rOut] using the median.
/// @param img       Source FITS image.
/// @param cx / cy   Centroid position (sub-pixel, 0-based).
/// @param rAp       Aperture radius (pixels).
/// @param rIn       Inner sky annulus radius (pixels). Default 1.5·rAp.
/// @param rOut      Outer sky annulus radius (pixels). Default 2.5·rAp.
std::optional<ApertureResult> aperturePhotometry(const FitsImage& img,
                                                  double cx, double cy,
                                                  double rAp,
                                                  double rIn  = 0.0,
                                                  double rOut = 0.0);

/// Compute photometric zero-point from matched catalog stars.
///
/// For each catalog star that also appears in detectedStars (by proximity),
/// compute: ZP = mag_cat − mag_inst
/// Returns the sigma-clipped median ZP and its uncertainty.
///
/// @param img           FITS image (for pixel data and WCS).
/// @param rAp           Aperture radius (pixels).
/// @param[out] zpSigma  Standard deviation of the ZP fit.
/// @returns Zero-point value, or nullopt if fewer than 3 matched stars.
std::optional<double> computeZeroPoint(const FitsImage& img,
                                        double rAp,
                                        double* zpSigma = nullptr);

/// Result from differential zero-point calibration.
struct ZeroPointResult {
    double zp     = 0.0;  ///< Zero-point: mag_calib = mag_inst + zp
    double sigma  = 0.0;  ///< Weighted RMS scatter of comp star residuals (mag)
    int    nStars = 0;    ///< Number of comparison stars used after sigma-clipping
};

/// Differential photometric zero-point using nearby isolated catalog stars.
///
/// Improvements over computeZeroPoint:
///   - Isolation check: rejects stars with a neighbour within 2.5×rAp
///   - Edge exclusion: rejects stars within 2.5×rAp of the image border
///   - Weighted mean: weight = 1 / (σ²_inst + σ²_cat_floor)
///   - 2.5σ iterative clipping on the weighted distribution
///
/// @param img      FITS image (wcs must be solved, catalogStars populated).
/// @param cx, cy   Target centroid (pixels) — currently unused but reserved for
///                 future proximity-weighted mode.
/// @param rAp      Aperture radius (pixels).
/// @returns ZeroPointResult, or nullopt if fewer than 3 comparison stars survive.
std::optional<ZeroPointResult> computeDifferentialZeroPoint(
    const FitsImage& img,
    double cx, double cy,
    double rAp);

/// Single data point of a growth curve (aperture photometry at one radius).
struct GrowthPoint {
    double radius   = 0.0;   ///< Aperture radius (pixels)
    double flux     = 0.0;   ///< Net flux (ADU) after background subtraction
    double fluxFrac = 0.0;   ///< flux / flux_at_max_radius (0–1 growth fraction)
    double magInst  = 99.0;  ///< Instrumental magnitude = -2.5·log10(flux)
    double magErr   = 0.0;   ///< Formal photometric uncertainty
    int    nPix     = 0;     ///< Pixels inside aperture
};

/// Compute aperture photometry at multiple radii for a single source (growth curve).
///
/// Sky background is estimated once from an annulus at [1.5, 2.5]×maxRadius.
/// Radii must be positive; they are sorted internally.
///
/// @param img    Source FITS image.
/// @param cx/cy  Centroid position (sub-pixel, 0-based).
/// @param radii  Aperture radii (pixels). Duplicates are skipped.
/// @returns      Sorted vector of GrowthPoint, one per valid radius.
QVector<GrowthPoint> multiAperturePhotometry(const FitsImage& img,
                                              double cx, double cy,
                                              const QVector<double>& radii);

} // namespace core
