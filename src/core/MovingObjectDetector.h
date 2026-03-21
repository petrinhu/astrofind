#pragma once

#include "FitsImage.h"

#include <QVector>
#include <QString>
#include <QPointF>
#include <expected>

namespace core {

/// A candidate moving object found across multiple frames.
struct MovingCandidate {
    QVector<QPointF> positions;  ///< Centroid in each frame (image coordinates)
    double           dxPerFrame; ///< Average pixel shift per frame in X
    double           dyPerFrame; ///< Average pixel shift per frame in Y
    double           speed;      ///< Pixel shift magnitude per frame
    double           snr;        ///< Minimum SNR across detections
};

/// Detect moving objects across a stack of aligned images.
///
/// The algorithm:
///   1. For each frame, collect detected stars (from FitsImage::detectedStars).
///   2. Build a "line pool": for every pair (frame i, frame j) and every pair of
///      detected sources, compute the implied motion vector.
///   3. Group motion vectors that agree within tolerancePx into tracks.
///   4. Return tracks seen in at least minFrames frames with SNR >= minSnr.
///
/// @param images        Sequence of images (must have detectedStars populated).
/// @param tolerancePx   Max positional residual (pixels) to accept a detection
///                      onto an existing track.
/// @param minFrames     Minimum number of frames a track must span.
/// @param minSnr        Minimum SNR for candidate detections.
std::expected<QVector<MovingCandidate>, QString>
detectMovingObjects(const QVector<FitsImage>& images,
                    double tolerancePx = 2.0,
                    int    minFrames   = 3,
                    double minSnr      = 5.0);

} // namespace core
