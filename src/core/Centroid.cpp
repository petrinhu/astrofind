#include "Centroid.h"

#include <cmath>
#include <algorithm>

namespace core {

std::optional<CentroidResult> findCentroid(const FitsImage& img,
                                           double clickX,
                                           double clickY,
                                           int    boxRadius)
{
    if (!img.isValid()) return std::nullopt;

    // ── 1. Bounding box ───────────────────────────────────────────────────────
    const int x0 = std::max(0,            static_cast<int>(std::floor(clickX)) - boxRadius);
    const int y0 = std::max(0,            static_cast<int>(std::floor(clickY)) - boxRadius);
    const int x1 = std::min(img.width-1,  static_cast<int>(std::floor(clickX)) + boxRadius);
    const int y1 = std::min(img.height-1, static_cast<int>(std::floor(clickY)) + boxRadius);

    if (x1 <= x0 || y1 <= y0) return std::nullopt;

    // ── 2. Background estimation: median of border pixels ─────────────────────
    std::vector<float> border;
    border.reserve(static_cast<size_t>(4) * static_cast<size_t>(2*boxRadius + 1));
    for (int x = x0; x <= x1; ++x) {
        border.push_back(img.pixelAt(x, y0));
        border.push_back(img.pixelAt(x, y1));
    }
    for (int y = y0+1; y < y1; ++y) {
        border.push_back(img.pixelAt(x0, y));
        border.push_back(img.pixelAt(x1, y));
    }
    const auto mid = static_cast<std::ptrdiff_t>(border.size() / 2);
    std::nth_element(border.begin(), border.begin() + mid, border.end());
    const double bkg = static_cast<double>(border[static_cast<std::size_t>(mid)]);

    // ── 3. Peak above background ──────────────────────────────────────────────
    double peak = 0.0;
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x)
            peak = std::max(peak, static_cast<double>(img.pixelAt(x,y)) - bkg);

    if (peak <= 0.0) return std::nullopt;

    // ── 4. Intensity-weighted moments (first pass) ────────────────────────────
    double sumW = 0.0, sumWX = 0.0, sumWY = 0.0;
    for (int y = y0; y <= y1; ++y) {
        for (int x = x0; x <= x1; ++x) {
            const double w = std::max(0.0, static_cast<double>(img.pixelAt(x,y)) - bkg);
            sumW  += w;
            sumWX += w * x;
            sumWY += w * y;
        }
    }
    if (sumW <= 0.0) return std::nullopt;

    double cx = sumWX / sumW;
    double cy = sumWY / sumW;

    // ── 5. Second pass centred on first-pass centroid (reduces bias) ──────────
    const int r2 = std::max(4, boxRadius / 2);
    const int ax0 = std::max(0,           static_cast<int>(std::round(cx)) - r2);
    const int ay0 = std::max(0,           static_cast<int>(std::round(cy)) - r2);
    const int ax1 = std::min(img.width-1, static_cast<int>(std::round(cx)) + r2);
    const int ay1 = std::min(img.height-1,static_cast<int>(std::round(cy)) + r2);

    sumW = sumWX = sumWY = 0.0;
    double sumWX2 = 0.0, sumWY2 = 0.0;  // second moments for FWHM
    for (int y = ay0; y <= ay1; ++y) {
        for (int x = ax0; x <= ax1; ++x) {
            const double w = std::max(0.0, static_cast<double>(img.pixelAt(x,y)) - bkg);
            sumW   += w;
            sumWX  += w * x;
            sumWY  += w * y;
            sumWX2 += w * x * x;
            sumWY2 += w * y * y;
        }
    }
    if (sumW <= 0.0) return std::nullopt;

    cx = sumWX / sumW;
    cy = sumWY / sumW;

    // Second-moment FWHM: σ² = <x²> - <x>²;  FWHM = 2.355·σ
    const double varX = sumWX2/sumW - cx*cx;
    const double varY = sumWY2/sumW - cy*cy;
    const double fwhmX = (varX > 0.0) ? 2.355 * std::sqrt(varX) : 1.0;
    const double fwhmY = (varY > 0.0) ? 2.355 * std::sqrt(varY) : 1.0;

    // ── 6. SNR: peak / sqrt(peak + bkg) (Poisson + background noise) ─────────
    const double snr = (bkg > 0.0)
        ? peak / std::sqrt(peak + bkg)
        : std::sqrt(peak);

    return CentroidResult{ cx, cy, fwhmX, fwhmY, peak, snr };
}

// ─── PSF-fit centroid (symmetric 2-D Gaussian, gradient descent) ─────────────

std::optional<CentroidResult> findCentroidPsf(const FitsImage& img,
                                              double clickX,
                                              double clickY,
                                              int    boxRadius)
{
    // Seed with moment centroid
    auto seed = findCentroid(img, clickX, clickY, boxRadius);
    if (!seed) return std::nullopt;

    // Build pixel sample in a tight box around seed
    const int r = std::max(3, boxRadius / 2);
    const int x0 = std::max(0,            static_cast<int>(std::round(seed->x)) - r);
    const int y0 = std::max(0,            static_cast<int>(std::round(seed->y)) - r);
    const int x1 = std::min(img.width-1,  static_cast<int>(std::round(seed->x)) + r);
    const int y1 = std::min(img.height-1, static_cast<int>(std::round(seed->y)) + r);
    if (x1 <= x0 || y1 <= y0) return seed;

    struct Pix { double x, y, val; };
    std::vector<Pix> pix;
    pix.reserve(static_cast<size_t>(x1-x0+1) * static_cast<size_t>(y1-y0+1));

    // Background: median of border
    std::vector<float> border;
    for (int x = x0; x <= x1; ++x) { border.push_back(img.pixelAt(x,y0)); border.push_back(img.pixelAt(x,y1)); }
    for (int y = y0+1; y < y1; ++y) { border.push_back(img.pixelAt(x0,y)); border.push_back(img.pixelAt(x1,y)); }
    const auto bkgMid = static_cast<std::ptrdiff_t>(border.size() / 2);
    std::nth_element(border.begin(), border.begin() + bkgMid, border.end());
    const double bkg = border[static_cast<std::size_t>(bkgMid)];

    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x)
            pix.push_back({static_cast<double>(x), static_cast<double>(y),
                           std::max(0.0, static_cast<double>(img.pixelAt(x,y)) - bkg)});

    // Parameters: p[0]=cx, p[1]=cy, p[2]=sigma, p[3]=amplitude
    double cx    = seed->x;
    double cy    = seed->y;
    double sigma = std::max(0.5, (seed->fwhmX + seed->fwhmY) * 0.5 / 2.355);
    double amp   = seed->peak;

    constexpr int    kMaxIter = 40;
    constexpr double kLr      = 0.3;   // learning rate
    constexpr double kTol     = 1e-4;

    for (int iter = 0; iter < kMaxIter; ++iter) {
        double dCx = 0, dCy = 0, dSig = 0, dAmp = 0;
        const double sig2 = sigma * sigma;

        for (const auto& p : pix) {
            const double dx = p.x - cx, dy = p.y - cy;
            const double r2 = dx*dx + dy*dy;
            const double g  = std::exp(-0.5 * r2 / sig2);
            const double model = amp * g;
            const double resid = p.val - model;
            // Gradients of model w.r.t. each parameter
            const double dM_dCx  = amp * g * dx / sig2;
            const double dM_dCy  = amp * g * dy / sig2;
            const double dM_dSig = amp * g * r2 / (sig2 * sigma);
            const double dM_dAmp = g;

            dCx  += resid * dM_dCx;
            dCy  += resid * dM_dCy;
            dSig += resid * dM_dSig;
            dAmp += resid * dM_dAmp;
        }

        const double prevCx = cx, prevCy = cy;
        cx    += kLr * dCx    / static_cast<double>(pix.size());
        cy    += kLr * dCy    / static_cast<double>(pix.size());
        sigma += kLr * dSig   / static_cast<double>(pix.size());
        amp   += kLr * dAmp   / static_cast<double>(pix.size());
        sigma = std::max(0.3, sigma);
        amp   = std::max(0.0, amp);

        if (std::hypot(cx - prevCx, cy - prevCy) < kTol) break;
    }

    // Sanity check: centroid must stay within the search box
    if (cx < x0 || cx > x1 || cy < y0 || cy > y1) return seed;

    const double fwhm = 2.355 * sigma;
    const double snr  = (bkg > 0.0) ? amp / std::sqrt(amp + bkg) : std::sqrt(amp);

    return CentroidResult{ cx, cy, fwhm, fwhm, amp, snr };
}

} // namespace core
