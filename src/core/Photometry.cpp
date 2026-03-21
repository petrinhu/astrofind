#include "Photometry.h"

#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>

namespace core {

// ─── aperturePhotometry ───────────────────────────────────────────────────────

std::optional<ApertureResult> aperturePhotometry(const FitsImage& img,
                                                  double cx, double cy,
                                                  double rAp,
                                                  double rIn,
                                                  double rOut)
{
    if (!img.isValid() || rAp <= 0.0) return std::nullopt;

    if (rIn  <= 0.0) rIn  = 1.5 * rAp;
    if (rOut <= 0.0) rOut = 2.5 * rAp;

    const double rAp2  = rAp * rAp;
    const double rIn2  = rIn * rIn;
    const double rOut2 = rOut * rOut;

    // Integer bounding box
    const int boxR = static_cast<int>(std::ceil(rOut)) + 1;
    const int ix0  = std::max(0,            static_cast<int>(std::floor(cx)) - boxR);
    const int iy0  = std::max(0,            static_cast<int>(std::floor(cy)) - boxR);
    const int ix1  = std::min(img.width-1,  static_cast<int>(std::ceil(cx))  + boxR);
    const int iy1  = std::min(img.height-1, static_cast<int>(std::ceil(cy))  + boxR);

    std::vector<float> skyPixels;
    skyPixels.reserve(static_cast<size_t>(ix1-ix0+1) * static_cast<size_t>(iy1-iy0+1));

    double sumAp = 0.0;
    int    nAp   = 0;

    for (int y = iy0; y <= iy1; ++y) {
        for (int x = ix0; x <= ix1; ++x) {
            const double dx = x - cx, dy = y - cy;
            const double r2 = dx*dx + dy*dy;
            const float  v  = img.pixelAt(x, y);

            if (r2 <= rAp2) {
                sumAp += v;
                ++nAp;
            } else if (r2 > rIn2 && r2 <= rOut2) {
                skyPixels.push_back(v);
            }
        }
    }

    if (nAp == 0 || skyPixels.empty()) return std::nullopt;

    // Background: median of sky annulus
    const size_t mid = skyPixels.size() / 2;
    std::nth_element(skyPixels.begin(), skyPixels.begin() + static_cast<std::ptrdiff_t>(mid), skyPixels.end());
    const double bkg = skyPixels[mid];

    // Background RMS (using MAD → sigma ≈ 1.4826·MAD)
    std::vector<float> absdev(skyPixels.size());
    for (size_t i = 0; i < skyPixels.size(); ++i)
        absdev[i] = std::abs(skyPixels[i] - static_cast<float>(bkg));
    const size_t mid2 = absdev.size() / 2;
    std::nth_element(absdev.begin(), absdev.begin() + static_cast<std::ptrdiff_t>(mid2), absdev.end());
    const double bkgSigma = 1.4826 * absdev[mid2];

    const double netFlux = sumAp - bkg * nAp;
    if (netFlux <= 0.0) return std::nullopt;

    // Instrumental magnitude
    const double magInst = -2.5 * std::log10(netFlux);

    // Formal uncertainty: σ_flux² = netFlux + nAp·bkgSigma² (Poisson + sky)
    const double sigFlux = std::sqrt(netFlux + static_cast<double>(nAp) * bkgSigma * bkgSigma);
    const double magErr  = 2.5 / std::log(10.0) * (sigFlux / netFlux);

    return ApertureResult{ netFlux, bkg, bkgSigma, magInst, magErr, nAp };
}

// ─── computeZeroPoint ─────────────────────────────────────────────────────────

std::optional<double> computeZeroPoint(const FitsImage& img,
                                        double rAp,
                                        double* zpSigma)
{
    if (img.catalogStars.isEmpty() || img.detectedStars.isEmpty())
        return std::nullopt;

    // Match catalog stars to detected stars by proximity (within rAp pixels)
    // using WCS to project catalog positions (epoch-corrected) to pixels
    std::vector<double> zpValues;
    for (const auto& cat : img.catalogStars) {
        if (cat.mag > 90.0) continue;  // no mag

        const auto [raCorr, decCorr] = applyProperMotion(cat, img.jd);
        double px = 0.0, py = 0.0;
        img.wcs.skyToPix(raCorr, decCorr, px, py);

        // Find nearest detected star within rAp
        const DetectedStar* best = nullptr;
        double bestDist2 = rAp * rAp;
        for (const auto& det : img.detectedStars) {
            const double dx = det.x - px, dy = det.y - py;
            const double d2 = dx*dx + dy*dy;
            if (d2 < bestDist2) { bestDist2 = d2; best = &det; }
        }
        if (!best) continue;

        // Measure instrumental mag at the detected star position
        auto res = aperturePhotometry(img, best->x, best->y, rAp);
        if (!res) continue;

        zpValues.push_back(cat.mag - res->magInst);
    }

    if (zpValues.size() < 3) return std::nullopt;

    // Sigma-clipped median
    std::sort(zpValues.begin(), zpValues.end());
    const double median = zpValues[zpValues.size() / 2];

    // Compute MAD
    std::vector<double> devs(zpValues.size());
    for (size_t i = 0; i < zpValues.size(); ++i)
        devs[i] = std::abs(zpValues[i] - median);
    std::sort(devs.begin(), devs.end());
    const double mad   = devs[devs.size()/2];
    const double sigma = 1.4826 * mad;

    // Keep only values within 2.5·sigma
    std::vector<double> clipped;
    for (double v : zpValues)
        if (std::abs(v - median) <= 2.5 * sigma)
            clipped.push_back(v);

    if (clipped.empty()) return std::nullopt;

    const double zp = std::accumulate(clipped.begin(), clipped.end(), 0.0) / static_cast<double>(clipped.size());

    if (zpSigma) {
        double var = 0.0;
        for (double v : clipped) var += (v - zp)*(v - zp);
        *zpSigma = std::sqrt(var / static_cast<double>(clipped.size()));
    }

    return zp;
}

// ─── computeDifferentialZeroPoint ─────────────────────────────────────────────

std::optional<ZeroPointResult> computeDifferentialZeroPoint(
    const FitsImage& img,
    double /*cx*/, double /*cy*/,
    double rAp)
{
    if (!img.wcs.solved) return std::nullopt;
    if (img.catalogStars.isEmpty() || img.detectedStars.isEmpty()) return std::nullopt;

    const double isoRadius2  = (2.5 * rAp) * (2.5 * rAp);
    const double edgeMargin  = 2.5 * rAp;
    constexpr double kCatErr = 0.02;  // floor catalog uncertainty (mag)
    constexpr double kMaxInstErr = 0.10;  // reject noisier measurements

    struct CompStar { double zpValue; double weight; };
    std::vector<CompStar> comps;
    comps.reserve(static_cast<size_t>(img.catalogStars.size()));

    for (const auto& cat : img.catalogStars) {
        if (cat.mag > 90.0) continue;

        // Project catalog star to pixel (with proper motion epoch correction)
        const auto [raCorr, decCorr] = applyProperMotion(cat, img.jd);
        double px = 0.0, py = 0.0;
        img.wcs.skyToPix(raCorr, decCorr, px, py);

        // Edge exclusion
        if (px < edgeMargin || py < edgeMargin ||
            px > img.width  - edgeMargin ||
            py > img.height - edgeMargin)
            continue;

        // Match to nearest detected star within rAp
        const DetectedStar* best = nullptr;
        double bestDist2 = rAp * rAp;
        for (const auto& det : img.detectedStars) {
            const double dx = det.x - px, dy = det.y - py;
            const double d2 = dx*dx + dy*dy;
            if (d2 < bestDist2) { bestDist2 = d2; best = &det; }
        }
        if (!best) continue;

        // Isolation: no other detected star within 2.5×rAp of the match
        bool isolated = true;
        for (const auto& det : img.detectedStars) {
            if (&det == best) continue;
            const double dx = det.x - best->x, dy = det.y - best->y;
            if (dx*dx + dy*dy < isoRadius2) { isolated = false; break; }
        }
        if (!isolated) continue;

        // Measure instrumental mag with the same aperture
        auto res = aperturePhotometry(img, best->x, best->y, rAp);
        if (!res || res->magErr > kMaxInstErr) continue;

        const double zpVal  = cat.mag - res->magInst;
        const double sig2   = res->magErr * res->magErr + kCatErr * kCatErr;
        comps.push_back({ zpVal, 1.0 / sig2 });
    }

    if (comps.size() < 3) return std::nullopt;

    // Weighted mean
    auto weightedMean = [](const std::vector<CompStar>& v) -> std::pair<double,double> {
        double sumW = 0.0, sumWZ = 0.0;
        for (const auto& c : v) { sumW += c.weight; sumWZ += c.weight * c.zpValue; }
        if (sumW <= 0.0) return {0.0, 0.0};
        const double mean = sumWZ / sumW;
        double sumWR2 = 0.0;
        for (const auto& c : v)
            sumWR2 += c.weight * (c.zpValue - mean) * (c.zpValue - mean);
        return { mean, std::sqrt(sumWR2 / sumW) };
    };

    auto [zp1, sig1] = weightedMean(comps);

    // 2.5σ clipping
    if (sig1 > 0.001) {
        std::vector<CompStar> clipped;
        clipped.reserve(comps.size());
        for (const auto& c : comps)
            if (std::abs(c.zpValue - zp1) <= 2.5 * sig1)
                clipped.push_back(c);

        if (clipped.size() >= 3) {
            auto [zpF, sigF] = weightedMean(clipped);
            return ZeroPointResult{ zpF, sigF, static_cast<int>(clipped.size()) };
        }
    }

    return ZeroPointResult{ zp1, sig1, static_cast<int>(comps.size()) };
}

// ─── multiAperturePhotometry ──────────────────────────────────────────────────

QVector<GrowthPoint> multiAperturePhotometry(const FitsImage& img,
                                              double cx, double cy,
                                              const QVector<double>& radii)
{
    if (!img.isValid() || radii.isEmpty()) return {};

    // Sort and deduplicate radii
    QVector<double> sorted = radii;
    std::sort(sorted.begin(), sorted.end());
    sorted.erase(std::unique(sorted.begin(), sorted.end()), sorted.end());
    // Remove non-positive radii
    while (!sorted.isEmpty() && sorted.first() <= 0.0)
        sorted.removeFirst();
    if (sorted.isEmpty()) return {};

    const double maxR  = sorted.last();
    const double rIn   = 1.5 * maxR;
    const double rOut  = 2.5 * maxR;
    const double rIn2  = rIn  * rIn;
    const double rOut2 = rOut * rOut;
    const double rMax2 = maxR * maxR;

    // Bounding box for the largest aperture + annulus
    const int boxR = static_cast<int>(std::ceil(rOut)) + 1;
    const int ix0  = std::max(0,           static_cast<int>(std::floor(cx)) - boxR);
    const int iy0  = std::max(0,           static_cast<int>(std::floor(cy)) - boxR);
    const int ix1  = std::min(img.width-1, static_cast<int>(std::ceil(cx))  + boxR);
    const int iy1  = std::min(img.height-1,static_cast<int>(std::ceil(cy))  + boxR);

    // ── Estimate sky background once from the outermost annulus ──────────────
    std::vector<float> skyPx;
    skyPx.reserve(static_cast<size_t>((ix1-ix0+1) * (iy1-iy0+1)));
    for (int y = iy0; y <= iy1; ++y)
        for (int x = ix0; x <= ix1; ++x) {
            const double r2 = (x-cx)*(x-cx) + (y-cy)*(y-cy);
            if (r2 > rIn2 && r2 <= rOut2)
                skyPx.push_back(img.pixelAt(x, y));
        }

    double bkg = 0.0, bkgSigma = 1.0;
    if (!skyPx.empty()) {
        const size_t mid = skyPx.size() / 2;
        std::nth_element(skyPx.begin(), skyPx.begin() + static_cast<std::ptrdiff_t>(mid), skyPx.end());
        bkg = skyPx[mid];
        std::vector<float> absdev(skyPx.size());
        for (size_t i = 0; i < skyPx.size(); ++i)
            absdev[i] = std::abs(skyPx[i] - static_cast<float>(bkg));
        const size_t mid2 = absdev.size() / 2;
        std::nth_element(absdev.begin(), absdev.begin() + static_cast<std::ptrdiff_t>(mid2), absdev.end());
        bkgSigma = std::max(1.0, 1.4826 * static_cast<double>(absdev[mid2]));
    }

    // ── Accumulate sums for each radius band using a single pixel pass ───────
    // Build squared radii thresholds for each aperture
    const int N = sorted.size();
    std::vector<double> r2thresh(static_cast<size_t>(N));
    for (int i = 0; i < N; ++i) r2thresh[static_cast<size_t>(i)] = sorted[i] * sorted[i];

    std::vector<double> sums(static_cast<size_t>(N), 0.0);
    std::vector<int>    npix(static_cast<size_t>(N), 0);

    for (int y = iy0; y <= iy1; ++y)
        for (int x = ix0; x <= ix1; ++x) {
            const double r2 = (x-cx)*(x-cx) + (y-cy)*(y-cy);
            if (r2 > rMax2) continue;
            const float v = img.pixelAt(x, y);
            // Add to all apertures whose radius² ≥ r2
            for (int i = N-1; i >= 0; --i) {
                if (r2 <= r2thresh[static_cast<size_t>(i)]) {
                    sums[static_cast<size_t>(i)] += v;
                    npix[static_cast<size_t>(i)] += 1;
                } else break;
            }
        }

    // ── Build result vector ───────────────────────────────────────────────────
    QVector<GrowthPoint> result;
    result.reserve(N);

    double maxFlux = 0.0;
    for (int i = 0; i < N; ++i) {
        const int    n       = npix[static_cast<size_t>(i)];
        const double netFlux = sums[static_cast<size_t>(i)] - bkg * n;
        if (n == 0 || netFlux <= 0.0) continue;
        maxFlux = std::max(maxFlux, netFlux);
        const double magInst = -2.5 * std::log10(netFlux);
        const double sigFlux = std::sqrt(netFlux + static_cast<double>(n) * bkgSigma * bkgSigma);
        const double magErr  = 2.5 / std::log(10.0) * (sigFlux / netFlux);
        result.append(GrowthPoint{ sorted[i], netFlux, 0.0, magInst, magErr, n });
    }

    // Normalise fluxFrac
    if (maxFlux > 0.0)
        for (auto& pt : result) pt.fluxFrac = pt.flux / maxFlux;

    return result;
}

} // namespace core
