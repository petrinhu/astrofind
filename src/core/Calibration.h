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

} // namespace core
