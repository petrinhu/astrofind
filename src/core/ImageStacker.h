#pragma once

#include "FitsImage.h"

#include <QVector>
#include <functional>

namespace core {

enum class StackMode { Add, Average, Median };

/// How to compute the inter-image shift during alignment.
///
/// FFT         — Phase correlation on the full pixel array (existing behaviour).
///               Fast, but sensitive to background gradients and large shifts.
///
/// StarCatalog — Match detectedStars between the reference and each frame,
///               then compute a sigma-clipped mean translation.
///               More robust when stars are available; sub-pixel precision.
///               Falls back to FFT automatically if fewer than 3 star pairs
///               can be matched.
enum class AlignMode { FFT, StarCatalog };

struct StackResult {
    FitsImage image;                ///< Combined image
    QVector<QPointF> shifts;        ///< Pixel shift applied to each input image (dx, dy)
};

/// Align and stack a set of images.
///
/// Images are aligned to images[0] (the reference).
/// The combined image has the same dimensions as the reference.
///
/// @param images     Input images (must all have the same dimensions).
/// @param mode       Combination method.
/// @param align      Alignment algorithm (default: FFT phase correlation).
/// @param progress   Optional callback (0..100).
/// @returns StackResult on success, or an error string.
std::expected<StackResult, QString>
stackImages(const QVector<FitsImage>& images,
            StackMode mode = StackMode::Average,
            AlignMode align = AlignMode::FFT,
            const std::function<void(int)>& progress = {});

/// Track and stack: shift each image by a linear motion vector before stacking.
///
/// @param images     Input images (ordered chronologically).
/// @param dxPerFrame Pixel shift in X between consecutive frames.
/// @param dyPerFrame Pixel shift in Y between consecutive frames.
/// @param mode       Combination method.
std::expected<StackResult, QString>
trackAndStack(const QVector<FitsImage>& images,
              double dxPerFrame,
              double dyPerFrame,
              StackMode mode = StackMode::Average);

} // namespace core
