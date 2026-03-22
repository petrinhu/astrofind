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

// ─── applyBadPixelCorrection ─────────────────────────────────────────────────

int applyBadPixelCorrection(FitsImage& img, double sigmaThreshold)
{
    if (!img.isValid()) return 0;

    const int W = img.width;
    const int H = img.height;
    const size_t N = static_cast<size_t>(W) * H;

    // ── Step 1: compute per-pixel (value − 3×3 median) difference ─────────────
    std::vector<float> diff(N, 0.0f);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            // Collect 3×3 neighbourhood (including self)
            float nbuf[9]; int cnt = 0;
            for (int dy = -1; dy <= 1; ++dy)
                for (int dx = -1; dx <= 1; ++dx)
                    nbuf[cnt++] = img.pixelAt(x+dx, y+dy);  // clamps at edges

            // 3×3 median via partial sort
            std::nth_element(nbuf, nbuf+4, nbuf+9);
            const float localMed = nbuf[4];

            diff[static_cast<size_t>(y)*W + x] = img.pixelAt(x, y) - localMed;
        }
    }

    // ── Step 2: noise estimate via MAD → sigma ────────────────────────────────
    // MAD = median |diff|; for Gaussian noise: sigma ≈ 1.4826 × MAD
    std::vector<float> absDiff(N);
    for (size_t i = 0; i < N; ++i) absDiff[i] = std::abs(diff[i]);
    std::nth_element(absDiff.begin(), absDiff.begin() + static_cast<std::ptrdiff_t>(N/2), absDiff.end());
    const float mad = absDiff[N/2];
    if (mad <= 0.0f) return 0;   // uniform image — nothing to do
    const float noiseSigma = 1.4826f * mad;

    // ── Step 3: flag bad pixels ───────────────────────────────────────────────
    const float thresh = static_cast<float>(sigmaThreshold) * noiseSigma;
    std::vector<bool> bad(N, false);
    for (size_t i = 0; i < N; ++i)
        bad[i] = (std::abs(diff[i]) > thresh);

    // ── Step 4: interpolate bad pixels from valid neighbours ──────────────────
    int nFixed = 0;
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const size_t idx = static_cast<size_t>(y)*W + x;
            if (!bad[idx]) continue;

            // Median of valid (non-bad) 3×3 neighbours
            float vbuf[8]; int vcnt = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0) continue;
                    const int nx = x+dx, ny = y+dy;
                    if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
                    const size_t nidx = static_cast<size_t>(ny)*W + nx;
                    if (!bad[nidx]) vbuf[vcnt++] = img.data[nidx];
                }
            }
            if (vcnt == 0) continue;  // all neighbours also bad — leave as-is

            std::nth_element(vbuf, vbuf + vcnt/2, vbuf + vcnt);
            img.data[idx] = vbuf[vcnt/2];
            ++nFixed;
        }
    }

    if (nFixed > 0) computeAutoStretch(img);
    return nFixed;
}

// ─── subtractBackground ───────────────────────────────────────────────────────

int subtractBackground(FitsImage& img, int tileSize)
{
    if (!img.isValid()) return 0;

    const int W = img.width;
    const int H = img.height;

    // Clamp tile size to [8, min(W,H)]
    tileSize = std::max(8, std::min(tileSize, std::min(W, H)));

    // Tile grid dimensions (last tile may be partial)
    const int nTX = (W + tileSize - 1) / tileSize;
    const int nTY = (H + tileSize - 1) / tileSize;
    const int nTiles = nTX * nTY;

    // ── Step 1: compute sigma-clipped median per tile ─────────────────────────
    // Store the background estimate at each tile centre
    std::vector<float> tileVal(static_cast<size_t>(nTiles), 0.0f);
    std::vector<float> tileCx (static_cast<size_t>(nTiles), 0.0f);
    std::vector<float> tileCy (static_cast<size_t>(nTiles), 0.0f);

    for (int ty = 0; ty < nTY; ++ty) {
        for (int tx = 0; tx < nTX; ++tx) {
            const int x0 = tx * tileSize;
            const int y0 = ty * tileSize;
            const int x1 = std::min(x0 + tileSize, W);
            const int y1 = std::min(y0 + tileSize, H);

            tileCx[ty * nTX + tx] = (x0 + x1) * 0.5f;
            tileCy[ty * nTX + tx] = (y0 + y1) * 0.5f;

            // Collect finite pixels
            std::vector<float> vals;
            vals.reserve(static_cast<size_t>((x1-x0)*(y1-y0)));
            for (int y = y0; y < y1; ++y)
                for (int x = x0; x < x1; ++x) {
                    float v = img.data[static_cast<size_t>(y)*W + x];
                    if (std::isfinite(v)) vals.push_back(v);
                }

            if (vals.empty()) {
                tileVal[ty * nTX + tx] = 0.0f;
                continue;
            }

            // 3-iteration sigma clipping at 3σ
            for (int iter = 0; iter < 3; ++iter) {
                if (vals.size() < 4) break;
                // Compute median
                const size_t mid = vals.size() / 2;
                std::nth_element(vals.begin(), vals.begin() + static_cast<std::ptrdiff_t>(mid), vals.end());
                const float med = vals[mid];

                // Compute σ via MAD → 1.4826 * MAD
                std::vector<float> devs;
                devs.reserve(vals.size());
                for (float v : vals) devs.push_back(std::abs(v - med));
                const size_t dmid = devs.size() / 2;
                std::nth_element(devs.begin(), devs.begin() + static_cast<std::ptrdiff_t>(dmid), devs.end());
                const float sigma = 1.4826f * devs[dmid];
                if (sigma <= 0.0f) break;

                const float lo = med - 3.0f * sigma;
                const float hi = med + 3.0f * sigma;
                vals.erase(std::remove_if(vals.begin(), vals.end(),
                    [lo, hi](float v){ return v < lo || v > hi; }), vals.end());
            }

            if (vals.empty()) {
                tileVal[ty * nTX + tx] = 0.0f;
                continue;
            }

            const size_t mid = vals.size() / 2;
            std::nth_element(vals.begin(), vals.begin() + static_cast<std::ptrdiff_t>(mid), vals.end());
            tileVal[ty * nTX + tx] = vals[mid];
        }
    }

    // ── Step 2: bilinear interpolation to per-pixel background map ────────────
    const size_t nPixels = static_cast<size_t>(W) * H;
    img.background.assign(nPixels, 0.0f);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            // Find surrounding tile indices (fractional)
            const float fx = (static_cast<float>(x) + 0.5f - tileCx[0]) /
                             (nTX > 1 ? tileCx[1] - tileCx[0] : static_cast<float>(W));
            const float fy = (static_cast<float>(y) + 0.5f - tileCy[0]) /
                             (nTY > 1 ? tileCy[nTX] - tileCy[0] : static_cast<float>(H));

            const int tx0 = std::max(0, std::min(nTX - 2, static_cast<int>(std::floor(fx))));
            const int ty0 = std::max(0, std::min(nTY - 2, static_cast<int>(std::floor(fy))));
            const int tx1 = tx0 + 1;
            const int ty1 = ty0 + 1;

            float u = fx - static_cast<float>(tx0);
            float v = fy - static_cast<float>(ty0);
            u = std::clamp(u, 0.0f, 1.0f);
            v = std::clamp(v, 0.0f, 1.0f);

            const float v00 = tileVal[ty0 * nTX + tx0];
            const float v10 = tileVal[ty0 * nTX + tx1];
            const float v01 = tileVal[ty1 * nTX + tx0];
            const float v11 = tileVal[ty1 * nTX + tx1];

            const float bg = v00*(1-u)*(1-v) + v10*u*(1-v) + v01*(1-u)*v + v11*u*v;
            img.background[static_cast<size_t>(y)*W + x] = bg;
        }
    }

    // ── Step 3: subtract background from data ────────────────────────────────
    for (size_t i = 0; i < nPixels; ++i)
        img.data[i] -= img.background[i];

    computeAutoStretch(img);
    return nTiles;
}

} // namespace core
