#pragma once

#include "FitsImage.h"

#include <QString>
#include <QVector>
#include <expected>
#include <functional>

namespace core {

/// Subtract a dark frame from an image (in-place).
/// The dark must have the same dimensions as the image.
/// Returns true on success, false if dimensions don't match.
bool applyDark(FitsImage& img, const FitsImage& dark);

/// Divide an image by a normalised flat field (in-place).
/// The flat is normalised to its median before division.
/// Returns true on success, false if dimensions don't match or flat is invalid.
bool applyFlat(FitsImage& img, const FitsImage& flat);

enum class MasterMode { Average, Median };

/// Combine N calibration frames (dark or flat) into a single master frame.
/// Unlike stackImages(), this does NOT align frames — calibration frames are
/// taken without moving the telescope so alignment is unnecessary.
///
/// @param frames   Input frames — must all have the same dimensions (≥ 2).
/// @param mode     Combination method.
/// @param progress Optional progress callback (0–100).
/// @returns Master FitsImage on success, or error string.
std::expected<FitsImage, QString>
buildMasterFrame(const QVector<FitsImage>& frames,
                 MasterMode mode = MasterMode::Average,
                 const std::function<void(int)>& progress = {});

/// Auto-detect and correct hot pixels / bad columns using local statistics.
///
/// Algorithm:
///  1. For each pixel, compute the median of its 3×3 neighbourhood.
///  2. Estimate image noise via the MAD of all (pixel − local_median) differences.
///  3. Flag pixels where |pixel − local_median| > sigmaThreshold × noise_sigma.
///  4. Replace each flagged pixel with the median of its valid (non-flagged)
///     3×3 neighbours; if no valid neighbours exist, the pixel is left unchanged.
///
/// Typical value: sigmaThreshold = 5.0 — flags only clear outliers without
/// disturbing faint stars. Lower values (e.g. 3.0) are more aggressive.
///
/// @returns Number of pixels corrected (0 if image is empty or all clean).
int applyBadPixelCorrection(FitsImage& img, double sigmaThreshold = 5.0);

/// Estimate and subtract a sky background model using a sliding median filter.
///
/// Algorithm:
///  1. Divide the image into tileSize×tileSize tiles.
///  2. Per tile: collect finite, non-NaN pixels; sigma-clip (3σ, 3 iterations)
///     to remove stars; compute the clipped median as the tile background.
///  3. Bilinearly interpolate the sparse tile grid to a full per-pixel background map.
///  4. Subtract the map from img.data in-place; store the map in img.background.
///  5. Recompute auto-stretch.
///
/// @param tileSize  Tile side length in pixels (clamped to [8, min(W,H)]).
/// @returns         Number of tiles computed (0 if image invalid or too small).
int subtractBackground(FitsImage& img, int tileSize = 64);

} // namespace core
