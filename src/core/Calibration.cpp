#include "Calibration.h"

#include <algorithm>
#include <vector>
#include <cmath>

namespace core {

bool applyDark(FitsImage& img, const FitsImage& dark)
{
    if (dark.width != img.width || dark.height != img.height)
        return false;

    const size_t n = img.data.size();
    for (size_t i = 0; i < n; ++i)
        img.data[i] -= dark.data[i];

    computeAutoStretch(img);
    return true;
}

bool applyFlat(FitsImage& img, const FitsImage& flat)
{
    if (flat.width != img.width || flat.height != img.height)
        return false;
    if (flat.data.empty()) return false;

    // Compute flat median for normalisation
    std::vector<float> sample;
    sample.reserve(flat.data.size());
    for (float v : flat.data)
        if (std::isfinite(v) && v > 0.0f) sample.push_back(v);

    if (sample.empty()) return false;

    const size_t mid = sample.size() / 2;
    std::nth_element(sample.begin(), sample.begin() + static_cast<std::ptrdiff_t>(mid), sample.end());
    const float flatMedian = sample[mid];
    if (flatMedian <= 0.0f) return false;

    const size_t n = img.data.size();
    for (size_t i = 0; i < n; ++i) {
        const float norm = flat.data[i] / flatMedian;
        img.data[i] = (norm > 0.01f) ? img.data[i] / norm : img.data[i];
    }

    computeAutoStretch(img);
    return true;
}

std::expected<FitsImage, QString>
buildMasterFrame(const QVector<FitsImage>& frames,
                 MasterMode mode,
                 const std::function<void(int)>& progress)
{
    if (frames.size() < 2)
        return std::unexpected(QStringLiteral("At least 2 frames are required."));

    const int W = frames[0].width;
    const int H = frames[0].height;
    for (int i = 1; i < frames.size(); ++i) {
        if (frames[i].width != W || frames[i].height != H)
            return std::unexpected(
                QStringLiteral("Frame %1 has different dimensions (%2×%3 vs %4×%5).")
                    .arg(i + 1).arg(frames[i].width).arg(frames[i].height).arg(W).arg(H));
    }

    const size_t N       = static_cast<size_t>(frames.size());
    const size_t nPixels = static_cast<size_t>(W) * static_cast<size_t>(H);

    FitsImage out = frames[0];   // copy metadata (header, dateObs, etc.)
    out.data.assign(nPixels, 0.0f);
    out.fileName = QStringLiteral("master");

    const size_t reportEvery = std::max<size_t>(1, nPixels / 100);

    if (mode == MasterMode::Average) {
        // Accumulate sum, then divide
        for (size_t f = 0; f < N; ++f) {
            const auto& fd = frames[static_cast<int>(f)].data;
            for (size_t p = 0; p < nPixels; ++p)
                out.data[p] += fd[p];
        }
        const float inv = 1.0f / static_cast<float>(N);
        for (size_t p = 0; p < nPixels; ++p)
            out.data[p] *= inv;

        if (progress) progress(100);
    } else {
        // Median: per-pixel nth_element
        std::vector<float> vals(N);
        const size_t mid = N / 2;
        for (size_t p = 0; p < nPixels; ++p) {
            for (size_t f = 0; f < N; ++f)
                vals[f] = frames[static_cast<int>(f)].data[p];
            std::nth_element(vals.begin(), vals.begin() + static_cast<std::ptrdiff_t>(mid), vals.end());
            out.data[p] = vals[mid];

            if (progress && (p % reportEvery == 0))
                progress(static_cast<int>(p * 100 / nPixels));
        }
        if (progress) progress(100);
    }

    computeAutoStretch(out);
    return out;
}

} // namespace core
