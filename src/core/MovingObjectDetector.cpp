#include "MovingObjectDetector.h"
#include "Overlay.h"

#include <cmath>
#include <algorithm>

namespace core {

namespace {

struct MotionSeed {
    double dx;  ///< pixels per frame
    double dy;
};

struct Track {
    QVector<int>     frames;
    QVector<QPointF> positions;
    double           dx = 0.0;
    double           dy = 0.0;
    double           minSnr = 1e9;
};

double dist(QPointF a, QPointF b) {
    const double dx = a.x() - b.x();
    const double dy = a.y() - b.y();
    return std::sqrt(dx * dx + dy * dy);
}

} // anonymous namespace

std::expected<QVector<MovingCandidate>, QString>
detectMovingObjects(const QVector<FitsImage>& images,
                    double tolerancePx,
                    int    minFrames,
                    double minSnr)
{
    if (images.size() < 2)
        return std::unexpected(QStringLiteral("Need at least 2 images"));

    const int nFrames = static_cast<int>(images.size());

    // Collect source positions per frame
    QVector<QVector<QPointF>> pos(nFrames);
    QVector<QVector<double>>  snrs(nFrames);
    for (int f = 0; f < nFrames; ++f) {
        for (const auto& s : images[f].detectedStars) {
            if (s.snr < minSnr) continue;
            pos[f].append(QPointF(s.x, s.y));
            snrs[f].append(s.snr);
        }
    }

    // Build motion seeds from frame 0 → frame f pairs
    const int f0 = 0;
    QVector<Track> tracks;

    for (int sA = 0; sA < pos[f0].size(); ++sA) {
        for (int fB = 1; fB < nFrames; ++fB) {
            for (int sB = 0; sB < pos[fB].size(); ++sB) {
                const double steps = static_cast<double>(fB - f0);
                const double dx = (pos[fB][sB].x() - pos[f0][sA].x()) / steps;
                const double dy = (pos[fB][sB].y() - pos[f0][sA].y()) / steps;
                const double speed = std::sqrt(dx * dx + dy * dy);

                // Ignore stationary sources (likely stars; speed < 0.3 px/frame)
                if (speed < 0.3) continue;

                // Check if this seed is already claimed by an existing track
                bool merged = false;
                for (const auto& t : tracks) {
                    if (std::abs(t.dx - dx) < tolerancePx / steps &&
                        std::abs(t.dy - dy) < tolerancePx / steps)
                    {
                        // Check positional consistency for frame 0
                        const QPointF pred0 = QPointF(pos[fB][sB].x() - dx * fB,
                                                      pos[fB][sB].y() - dy * fB);
                        if (dist(pred0, pos[f0][sA]) < tolerancePx) {
                            merged = true;
                            break;
                        }
                    }
                }
                if (merged) continue;

                // Start a new track from this seed
                Track t;
                t.dx = dx;
                t.dy = dy;
                t.frames.append(f0);
                t.positions.append(pos[f0][sA]);
                t.minSnr = snrs[f0][sA];
                t.frames.append(fB);
                t.positions.append(pos[fB][sB]);
                t.minSnr = std::min(t.minSnr, snrs[fB][sB]);
                tracks.append(t);
            }
        }
    }

    // For each track, try to extend to remaining frames
    for (auto& t : tracks) {
        for (int f = 1; f < nFrames; ++f) {
            if (t.frames.contains(f)) continue;
            // Predict position in frame f
            const QPointF pred(t.positions[0].x() + t.dx * f,
                               t.positions[0].y() + t.dy * f);
            double bestDist = tolerancePx;
            int    bestIdx  = -1;
            for (int s = 0; s < pos[f].size(); ++s) {
                const double d = dist(pos[f][s], pred);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx  = s;
                }
            }
            if (bestIdx >= 0) {
                t.frames.append(f);
                t.positions.append(pos[f][bestIdx]);
                t.minSnr = std::min(t.minSnr, snrs[f][bestIdx]);
            }
        }
    }

    // Filter: must span minFrames, and sort frames
    QVector<MovingCandidate> result;
    for (const auto& t : tracks) {
        if (t.frames.size() < minFrames) continue;

        // Refit dx/dy as least-squares slope over available frames
        double sumF = 0, sumX = 0, sumY = 0, sumFF = 0, sumFX = 0, sumFY = 0;
        const int n = static_cast<int>(t.frames.size());
        for (int i = 0; i < n; ++i) {
            const double f = t.frames[i];
            sumF  += f;
            sumX  += t.positions[i].x();
            sumY  += t.positions[i].y();
            sumFF += f * f;
            sumFX += f * t.positions[i].x();
            sumFY += f * t.positions[i].y();
        }
        const double denom = n * sumFF - sumF * sumF;
        double dxFit = t.dx, dyFit = t.dy;
        if (std::abs(denom) > 1e-9) {
            dxFit = (n * sumFX - sumF * sumX) / denom;
            dyFit = (n * sumFY - sumF * sumY) / denom;
        }

        // Build sorted positions array by frame index
        QVector<std::pair<int,QPointF>> sorted;
        for (int i = 0; i < n; ++i)
            sorted.append({ t.frames[i], t.positions[i] });
        std::sort(sorted.begin(), sorted.end(),
                  [](const auto& a, const auto& b){ return a.first < b.first; });

        MovingCandidate mc;
        mc.dxPerFrame = dxFit;
        mc.dyPerFrame = dyFit;
        mc.speed      = std::sqrt(dxFit * dxFit + dyFit * dyFit);
        mc.snr        = t.minSnr;
        for (const auto& [f, p] : sorted)
            mc.positions.append(p);

        result.append(mc);
    }

    // Remove duplicates: if two candidates share the same starting position
    // within tolerancePx, keep the one with more frames
    std::sort(result.begin(), result.end(),
              [](const MovingCandidate& a, const MovingCandidate& b){
                  return a.positions.size() > b.positions.size();
              });

    QVector<MovingCandidate> deduped;
    for (const auto& mc : result) {
        bool dup = false;
        for (const auto& prev : deduped) {
            if (!mc.positions.isEmpty() && !prev.positions.isEmpty() &&
                dist(mc.positions[0], prev.positions[0]) < tolerancePx * 2.0)
            {
                dup = true;
                break;
            }
        }
        if (!dup) deduped.append(mc);
    }

    return deduped;
}

} // namespace core
