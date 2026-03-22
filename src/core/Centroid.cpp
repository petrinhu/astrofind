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

    return CentroidResult{ cx, cy, fwhmX, fwhmY, /*theta=*/0.0, peak, snr };
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

    return CentroidResult{ cx, cy, fwhm, fwhm, /*theta=*/0.0, amp, snr };
}

// ─── findCentroidElliptical (Levenberg-Marquardt, 6-parameter elliptic Gaussian) ──

/// In-place 6×6 Gaussian elimination with partial pivoting. Returns false if singular.
static bool solve6(std::array<double,36>& A, std::array<double,6>& b) noexcept
{
    for (int col = 0; col < 6; ++col) {
        // Find pivot row
        int    pivRow = col;
        double pivMax = std::abs(A[static_cast<size_t>(col*6+col)]);
        for (int row = col+1; row < 6; ++row) {
            const double v = std::abs(A[static_cast<size_t>(row*6+col)]);
            if (v > pivMax) { pivMax = v; pivRow = row; }
        }
        if (pivMax < 1e-14) return false;
        if (pivRow != col) {
            for (int k = 0; k < 6; ++k)
                std::swap(A[static_cast<size_t>(col*6+k)], A[static_cast<size_t>(pivRow*6+k)]);
            std::swap(b[static_cast<size_t>(col)], b[static_cast<size_t>(pivRow)]);
        }
        // Eliminate below pivot
        for (int row = col+1; row < 6; ++row) {
            const double f = A[static_cast<size_t>(row*6+col)] / A[static_cast<size_t>(col*6+col)];
            for (int k = col; k < 6; ++k)
                A[static_cast<size_t>(row*6+k)] -= f * A[static_cast<size_t>(col*6+k)];
            b[static_cast<size_t>(row)] -= f * b[static_cast<size_t>(col)];
        }
    }
    // Back-substitution
    for (int row = 5; row >= 0; --row) {
        double s = b[static_cast<size_t>(row)];
        for (int k = row+1; k < 6; ++k)
            s -= A[static_cast<size_t>(row*6+k)] * b[static_cast<size_t>(k)];
        b[static_cast<size_t>(row)] = s / A[static_cast<size_t>(row*6+row)];
    }
    return true;
}

std::optional<CentroidResult> findCentroidElliptical(const FitsImage& img,
                                                     double clickX,
                                                     double clickY,
                                                     int    boxRadius)
{
    // Seed from symmetric fit
    auto seed = findCentroidPsf(img, clickX, clickY, boxRadius);
    if (!seed) return std::nullopt;

    // Build pixel sample in tight box around seed
    const int r  = std::max(4, boxRadius / 2);
    const int x0 = std::max(0,            static_cast<int>(std::round(seed->x)) - r);
    const int y0 = std::max(0,            static_cast<int>(std::round(seed->y)) - r);
    const int x1 = std::min(img.width-1,  static_cast<int>(std::round(seed->x)) + r);
    const int y1 = std::min(img.height-1, static_cast<int>(std::round(seed->y)) + r);
    if (x1 <= x0 || y1 <= y0) return seed;

    // Background: median of border pixels
    std::vector<float> border;
    for (int x = x0; x <= x1; ++x) { border.push_back(img.pixelAt(x,y0)); border.push_back(img.pixelAt(x,y1)); }
    for (int y = y0+1; y < y1; ++y) { border.push_back(img.pixelAt(x0,y)); border.push_back(img.pixelAt(x1,y)); }
    const auto bkgMid = static_cast<std::ptrdiff_t>(border.size() / 2);
    std::nth_element(border.begin(), border.begin() + bkgMid, border.end());
    const double bkg = static_cast<double>(border[static_cast<size_t>(bkgMid)]);

    struct Pix { double x, y, val; };
    std::vector<Pix> pixels;
    pixels.reserve(static_cast<size_t>((x1-x0+1) * (y1-y0+1)));
    for (int y = y0; y <= y1; ++y)
        for (int x = x0; x <= x1; ++x)
            pixels.push_back({static_cast<double>(x), static_cast<double>(y),
                              std::max(0.0, static_cast<double>(img.pixelAt(x,y)) - bkg)});
    if (pixels.size() < 7) return seed;   // under-determined

    // Parameters p[0..5] = {cx, cy, sigma_a, sigma_b, theta_rad, amplitude}
    const double sig0 = std::max(0.5, seed->fwhmX / 2.355);
    std::array<double,6> p = { seed->x, seed->y, sig0, sig0 * 0.9, 0.0, seed->peak };

    // ── Levenberg-Marquardt ────────────────────────────────────────────────────
    // Model: g = p5 * exp(-0.5*(u²/p2² + v²/p3²))
    //        u =  (x-p0)*cos(p4) + (y-p1)*sin(p4)
    //        v = -(x-p0)*sin(p4) + (y-p1)*cos(p4)

    auto chi2fn = [&](const std::array<double,6>& q) -> double {
        double s = 0.0;
        const double cosT = std::cos(q[4]), sinT = std::sin(q[4]);
        const double sa2 = q[2]*q[2], sb2 = q[3]*q[3];
        for (const auto& px : pixels) {
            const double dx = px.x - q[0], dy = px.y - q[1];
            const double u  =  dx*cosT + dy*sinT;
            const double v  = -dx*sinT + dy*cosT;
            const double g  = q[5] * std::exp(-0.5*(u*u/sa2 + v*v/sb2));
            const double res = px.val - g;
            s += res * res;
        }
        return s;
    };

    double lambda  = 1e-3;
    double curChi2 = chi2fn(p);
    constexpr int kMaxIter = 60;

    for (int iter = 0; iter < kMaxIter; ++iter) {
        const double cosT = std::cos(p[4]), sinT = std::sin(p[4]);
        const double sa2  = p[2]*p[2], sb2 = p[3]*p[3];
        const double sa3  = p[2]*sa2,  sb3 = p[3]*sb2;

        // Accumulate J^T J (6×6) and J^T r (6)
        std::array<double,36> JtJ = {};
        std::array<double,6>  Jtr = {};

        for (const auto& px : pixels) {
            const double dx = px.x - p[0], dy = px.y - p[1];
            const double u  =  dx*cosT + dy*sinT;
            const double v  = -dx*sinT + dy*cosT;
            const double e  = std::exp(-0.5*(u*u/sa2 + v*v/sb2));
            const double g  = p[5] * e;
            const double res = px.val - g;

            // ∂g/∂p[k] — see derivation in Astronomy.cpp header comment
            std::array<double,6> J;
            J[0] = g * ( u*cosT/sa2 - v*sinT/sb2);   // ∂g/∂cx
            J[1] = g * ( u*sinT/sa2 + v*cosT/sb2);   // ∂g/∂cy
            J[2] = g * u*u / sa3;                     // ∂g/∂σ_a
            J[3] = g * v*v / sb3;                     // ∂g/∂σ_b
            J[4] = g * u*v * (1.0/sb2 - 1.0/sa2);    // ∂g/∂θ
            J[5] = e;                                  // ∂g/∂A

            for (int j = 0; j < 6; ++j) {
                for (int k = 0; k < 6; ++k)
                    JtJ[static_cast<size_t>(j*6+k)] += J[static_cast<size_t>(j)] * J[static_cast<size_t>(k)];
                Jtr[static_cast<size_t>(j)] += J[static_cast<size_t>(j)] * res;
            }
        }

        // LM damping: (J^T J + λ · diag(J^T J)) · Δp = J^T r
        std::array<double,36> JtJdamp = JtJ;
        for (int j = 0; j < 6; ++j)
            JtJdamp[static_cast<size_t>(j*6+j)] *= (1.0 + lambda);

        std::array<double,6> dp = Jtr;
        if (!solve6(JtJdamp, dp)) break;   // singular — stop

        // Trial step
        std::array<double,6> pTrial = p;
        for (int j = 0; j < 6; ++j)
            pTrial[static_cast<size_t>(j)] += dp[static_cast<size_t>(j)];

        // Enforce constraints
        pTrial[2] = std::max(0.3, pTrial[2]);  // sigma_a >= 0.3
        pTrial[3] = std::max(0.3, pTrial[3]);  // sigma_b >= 0.3
        pTrial[5] = std::max(0.0, pTrial[5]);  // amplitude >= 0

        const double trialChi2 = chi2fn(pTrial);
        if (trialChi2 < curChi2) {
            p        = pTrial;
            curChi2  = trialChi2;
            lambda  *= 0.2;
            if (lambda < 1e-10) lambda = 1e-10;
        } else {
            lambda *= 5.0;
            if (lambda > 1e8) break;   // diverged
        }
    }

    // Ensure sigma_a is the major axis
    if (p[3] > p[2]) {
        std::swap(p[2], p[3]);
        p[4] += M_PI * 0.5;  // rotate major axis by 90°
    }

    // Normalise theta to (-90°, +90°]
    double thetaDeg = p[4] * (180.0 / M_PI);
    thetaDeg = std::fmod(thetaDeg + 360.0, 180.0);
    if (thetaDeg > 90.0) thetaDeg -= 180.0;

    // Sanity check: centroid within search box, sigma plausible
    if (p[0] < x0 || p[0] > x1 || p[1] < y0 || p[1] > y1) return seed;
    if (p[2] > boxRadius || p[3] > boxRadius)               return seed;

    const double fwhmA = 2.355 * p[2];
    const double fwhmB = 2.355 * p[3];
    const double snr   = (bkg > 0.0)
        ? p[5] / std::sqrt(p[5] + bkg)
        : std::sqrt(p[5]);

    return CentroidResult{ p[0], p[1], fwhmA, fwhmB, thetaDeg, p[5], snr };
}

} // namespace core
