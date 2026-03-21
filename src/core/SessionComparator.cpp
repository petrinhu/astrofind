#include "SessionComparator.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <vector>

namespace core {

// ── Sigma-clipping stats ────────────────────────────────────────────────────

struct Stats { float median = 0.f, sigma = 0.f; };

static Stats sigmaClip(const std::vector<float>& vals, int iters = 3)
{
    if (vals.empty()) return {};
    std::vector<float> v = vals;

    float lo = -1e30f, hi = 1e30f;
    for (int it = 0; it < iters; ++it) {
        // median
        std::vector<float> tmp;
        tmp.reserve(v.size());
        for (float x : v) if (x > lo && x < hi) tmp.push_back(x);
        if (tmp.empty()) break;
        std::nth_element(tmp.begin(), tmp.begin() + tmp.size()/2, tmp.end());
        float med = tmp[tmp.size()/2];

        // MAD → sigma estimate
        std::vector<float> dev;
        dev.reserve(tmp.size());
        for (float x : tmp) dev.push_back(std::abs(x - med));
        std::nth_element(dev.begin(), dev.begin() + dev.size()/2, dev.end());
        float mad = dev[dev.size()/2];
        float sig = 1.4826f * mad;
        if (sig < 1e-8f) sig = 1e-8f;

        lo = med - 3.f * sig;
        hi = med + 3.f * sig;
        v = tmp;
    }

    // Final pass
    std::vector<float> clipped;
    clipped.reserve(v.size());
    for (float x : v) if (x > lo && x < hi) clipped.push_back(x);
    if (clipped.empty()) clipped = v;

    std::nth_element(clipped.begin(), clipped.begin() + clipped.size()/2, clipped.end());
    float med = clipped[clipped.size()/2];

    float sum2 = 0.f;
    for (float x : clipped) sum2 += (x - med) * (x - med);
    float sig = std::sqrt(sum2 / static_cast<float>(clipped.size()));

    return { med, sig };
}

// ── Bilinear interpolation ──────────────────────────────────────────────────

static float bilinear(const FitsImage& img, double fx, double fy)
{
    const int x0 = static_cast<int>(std::floor(fx));
    const int y0 = static_cast<int>(std::floor(fy));
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;
    const float dx = static_cast<float>(fx - x0);
    const float dy = static_cast<float>(fy - y0);

    const float v00 = img.pixelAt(x0, y0);
    const float v10 = img.pixelAt(x1, y0);
    const float v01 = img.pixelAt(x0, y1);
    const float v11 = img.pixelAt(x1, y1);

    return v00*(1-dx)*(1-dy) + v10*dx*(1-dy) +
           v01*(1-dx)*dy     + v11*dx*dy;
}

// ── WCS warp: remap refImg pixels onto newImg pixel grid ───────────────────

static FitsImage warpRef(const FitsImage& newImg, const FitsImage& refImg)
{
    FitsImage warped;
    warped.width  = newImg.width;
    warped.height = newImg.height;
    warped.data.resize(static_cast<size_t>(newImg.width) * newImg.height, 0.f);

    for (int y = 0; y < newImg.height; ++y) {
        for (int x = 0; x < newImg.width; ++x) {
            // new pixel → sky
            double ra, dec;
            newImg.wcs.pixToSky(x, y, ra, dec);

            // sky → ref pixel
            double rx, ry;
            refImg.wcs.skyToPix(ra, dec, rx, ry);

            if (rx >= 0 && rx < refImg.width - 1 &&
                ry >= 0 && ry < refImg.height - 1)
            {
                warped.data[static_cast<size_t>(y) * newImg.width + x] =
                    bilinear(refImg, rx, ry);
            }
        }
    }
    return warped;
}

// ── BFS flood-fill source detection ────────────────────────────────────────

static QVector<DetectedStar> floodFillDetect(
    const FitsImage& residual,
    float threshold,
    float bkg)
{
    const int W = residual.width;
    const int H = residual.height;
    std::vector<bool> visited(static_cast<size_t>(W) * H, false);
    QVector<DetectedStar> sources;

    auto idx = [&](int x, int y){ return static_cast<size_t>(y) * W + x; };

    const int dx4[4] = {1,-1,0,0};
    const int dy4[4] = {0,0,1,-1};

    for (int sy = 0; sy < H; ++sy) {
        for (int sx = 0; sx < W; ++sx) {
            if (visited[idx(sx,sy)]) continue;
            float v = residual.data[idx(sx,sy)] - bkg;
            if (v < threshold) continue;

            // BFS
            std::queue<std::pair<int,int>> q;
            q.push({sx,sy});
            visited[idx(sx,sy)] = true;

            double sumX = 0, sumY = 0, sumV = 0;
            int    count = 0;
            float  peak  = 0.f;

            while (!q.empty()) {
                auto [cx, cy] = q.front(); q.pop();
                float pv = residual.data[idx(cx,cy)] - bkg;
                sumX += cx * pv;
                sumY += cy * pv;
                sumV += pv;
                ++count;
                if (pv > peak) peak = pv;

                for (int d = 0; d < 4; ++d) {
                    int nx = cx + dx4[d];
                    int ny = cy + dy4[d];
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    if (visited[idx(nx,ny)]) continue;
                    float nv = residual.data[idx(nx,ny)] - bkg;
                    if (nv < threshold) continue;
                    visited[idx(nx,ny)] = true;
                    q.push({nx,ny});
                }
            }

            if (sumV <= 0.0 || count < 2) continue;

            DetectedStar src;
            src.x    = sumX / sumV;
            src.y    = sumY / sumV;
            src.flux = peak;
            src.snr  = peak / threshold * 4.0f;  // rough SNR estimate
            src.a    = std::sqrt(static_cast<double>(count) / M_PI);
            src.b    = src.a;

            sources.append(src);
        }
    }
    return sources;
}

// ── Public API ──────────────────────────────────────────────────────────────

CompareResult compareImages(const FitsImage& newImg,
                             const FitsImage& refImg,
                             float sigmaThreshold)
{
    CompareResult result;

    if (!newImg.isValid() || !refImg.isValid()) {
        result.error = QStringLiteral("Invalid input images");
        return result;
    }

    // ── Step 1: align ref onto new grid ────────────────────────────────────
    FitsImage aligned;
    const bool hasWcs = newImg.wcs.solved && refImg.wcs.solved;

    if (hasWcs) {
        aligned = warpRef(newImg, refImg);
        result.usedWcs = true;
    } else {
        // Direct subtraction — images must already be registered
        if (newImg.width != refImg.width || newImg.height != refImg.height) {
            result.error = QStringLiteral(
                "Images have different sizes and no WCS — cannot subtract");
            return result;
        }
        aligned       = refImg;   // shallow copy is fine; we don't mutate
        result.usedWcs = false;
    }

    // ── Step 2: subtract ────────────────────────────────────────────────────
    FitsImage& res = result.residual;
    res.width  = newImg.width;
    res.height = newImg.height;
    res.data.resize(static_cast<size_t>(res.width) * res.height);
    res.fileName = QStringLiteral("residual");
    res.filePath = QStringLiteral("residual");

    for (size_t i = 0; i < res.data.size(); ++i)
        res.data[i] = newImg.data[i] - aligned.data[i];

    // ── Step 3: sigma-clip background of residual ───────────────────────────
    // Sample 10 % of pixels for speed
    const size_t total = res.data.size();
    const size_t step  = std::max<size_t>(1, total / (total / 10 + 1));
    std::vector<float> sample;
    sample.reserve(total / step + 1);
    for (size_t i = 0; i < total; i += step)
        sample.push_back(res.data[i]);

    const Stats s = sigmaClip(sample);
    const float threshold = s.sigma * sigmaThreshold;

    // Set display stretch on residual
    res.displayMin = s.median - 2.f * s.sigma;
    res.displayMax = s.median + 6.f * s.sigma;

    // ── Step 4: flood-fill detection ────────────────────────────────────────
    res.detectedStars = floodFillDetect(res, threshold, s.median);
    result.detectionCount = static_cast<int>(res.detectedStars.size());

    // Fill RA/Dec for detections if WCS available
    if (hasWcs) {
        for (auto& star : res.detectedStars)
            newImg.wcs.pixToSky(star.x, star.y, star.ra, star.dec);
    }

    return result;
}

} // namespace core
