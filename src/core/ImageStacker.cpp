#include "ImageStacker.h"
#include "FitsImage.h"

#include <fftw3.h>
#include <QPointF>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <optional>
#include <vector>

namespace core {

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

namespace {

/// Allocate an fftw_complex array of size n (caller must call fftw_free)
fftw_complex* allocComplex(int n) {
    return reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * n));
}

/// Forward real-to-complex FFT of a float image row stored in row-major order.
/// Returns newly allocated complex array (size w*(h/2+1)); caller owns it.
fftw_complex* forwardFFT(const float* src, int w, int h) {
    const int nc = w * (h / 2 + 1);
    double* in   = reinterpret_cast<double*>(fftw_malloc(sizeof(double) * w * h));
    fftw_complex* out = allocComplex(nc);

    for (int i = 0; i < w * h; ++i)
        in[i] = static_cast<double>(src[i]);

    fftw_plan p = fftw_plan_dft_r2c_2d(h, w, in, out, FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    fftw_free(in);
    return out;
}

/// Compute cross-power spectrum: R[k] = A[k] * conj(B[k]) / |A[k] * conj(B[k])|
/// Result stored in 'out' (size nc).
void crossPowerSpectrum(const fftw_complex* a, const fftw_complex* b,
                        fftw_complex* out, int nc)
{
    for (int k = 0; k < nc; ++k) {
        const double re = a[k][0] * b[k][0] + a[k][1] * b[k][1];
        const double im = a[k][1] * b[k][0] - a[k][0] * b[k][1];
        const double mag = std::sqrt(re * re + im * im);
        if (mag > 1e-12) {
            out[k][0] = re / mag;
            out[k][1] = im / mag;
        } else {
            out[k][0] = out[k][1] = 0.0;
        }
    }
}

/// Inverse complex-to-real FFT. Returns vector of size w*h.
std::vector<double> inverseFFT(fftw_complex* in, int w, int h) {
    std::vector<double> out(static_cast<size_t>(w * h));
    fftw_plan p = fftw_plan_dft_c2r_2d(h, w, in, out.data(), FFTW_ESTIMATE);
    fftw_execute(p);
    fftw_destroy_plan(p);
    const double norm = static_cast<double>(w * h);
    for (auto& v : out) v /= norm;
    return out;
}

struct Shift { double dx, dy; };

/// Compute sub-pixel shift between ref and img via FFT phase correlation.
Shift phaseCorrelation(const FitsImage& ref, const FitsImage& img) {
    const int w = ref.width;
    const int h = ref.height;
    const int nc = w * (h / 2 + 1);

    fftw_complex* fref  = forwardFFT(ref.data.data(), w, h);
    fftw_complex* fimg  = forwardFFT(img.data.data(), w, h);
    fftw_complex* cps   = allocComplex(nc);

    crossPowerSpectrum(fref, fimg, cps, nc);

    fftw_free(fref);
    fftw_free(fimg);

    std::vector<double> corr = inverseFFT(cps, w, h);
    fftw_free(cps);

    // Find peak
    int peakIdx = static_cast<int>(
        std::max_element(corr.begin(), corr.end()) - corr.begin());
    int py = peakIdx / w;
    int px = peakIdx % w;

    // Sub-pixel refinement via parabolic interpolation
    auto val = [&](int x, int y) -> double {
        x = (x + w) % w;
        y = (y + h) % h;
        return corr[static_cast<size_t>(y) * w + x];
    };

    double subX = 0.0, subY = 0.0;
    {
        const double l = val(px - 1, py);
        const double c = val(px,     py);
        const double r = val(px + 1, py);
        const double denom = l - 2.0 * c + r;
        if (std::abs(denom) > 1e-12) subX = 0.5 * (l - r) / denom;
    }
    {
        const double u = val(px, py - 1);
        const double c = val(px, py);
        const double d = val(px, py + 1);
        const double denom = u - 2.0 * c + d;
        if (std::abs(denom) > 1e-12) subY = 0.5 * (u - d) / denom;
    }

    double dx = static_cast<double>(px) + subX;
    double dy = static_cast<double>(py) + subY;

    // Wrap to [-w/2, w/2) and [-h/2, h/2)
    if (dx > w / 2.0) dx -= w;
    if (dy > h / 2.0) dy -= h;

    return { dx, dy };
}

/// Bilinear-sample src at sub-pixel position (x,y); returns 0 outside bounds.
float sampleBilinear(const FitsImage& src, double x, double y) {
    const int x0 = static_cast<int>(std::floor(x));
    const int y0 = static_cast<int>(std::floor(y));
    const int x1 = x0 + 1;
    const int y1 = y0 + 1;
    const double fx = x - x0;
    const double fy = y - y0;

    auto pix = [&](int xi, int yi) -> float {
        if (xi < 0 || xi >= src.width || yi < 0 || yi >= src.height) return 0.0f;
        return src.data[static_cast<size_t>(yi) * src.width + xi];
    };

    return static_cast<float>(
        pix(x0, y0) * (1 - fx) * (1 - fy) +
        pix(x1, y0) * fx       * (1 - fy) +
        pix(x0, y1) * (1 - fx) * fy       +
        pix(x1, y1) * fx       * fy);
}

/// Shift src by (dx, dy) into dst (same dimensions as ref).
/// Pixels that fall outside src are set to 0.
void shiftImage(const FitsImage& src, FitsImage& dst, double dx, double dy) {
    const int w = dst.width;
    const int h = dst.height;
    dst.data.resize(static_cast<size_t>(w) * h, 0.0f);
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            dst.data[static_cast<size_t>(y) * w + x] =
                sampleBilinear(src, x - dx, y - dy);
}

/// Combine a set of aligned float buffers (all same length) with mode.
std::vector<float> combineBuffers(const std::vector<const float*>& bufs,
                                  int n, StackMode mode)
{
    std::vector<float> result(static_cast<size_t>(n));
    const int k = static_cast<int>(bufs.size());

    if (mode == StackMode::Add) {
        for (int i = 0; i < n; ++i) {
            float s = 0.0f;
            for (int j = 0; j < k; ++j) s += bufs[j][i];
            result[i] = s;
        }
    } else if (mode == StackMode::Average) {
        for (int i = 0; i < n; ++i) {
            float s = 0.0f;
            for (int j = 0; j < k; ++j) s += bufs[j][i];
            result[i] = s / static_cast<float>(k);
        }
    } else { // Median
        std::vector<float> tmp(static_cast<size_t>(k));
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < k; ++j) tmp[j] = bufs[j][i];
            std::nth_element(tmp.begin(), tmp.begin() + k / 2, tmp.end());
            if (k % 2 == 1) {
                result[i] = tmp[k / 2];
            } else {
                float upper = *std::min_element(tmp.begin() + k / 2, tmp.end());
                result[i] = (tmp[k / 2 - 1] + upper) * 0.5f;
            }
        }
    }
    return result;
}

/// Compute sub-pixel shift from matched detectedStars (ref → img).
///
/// Algorithm:
///   1. Nearest-neighbour matching within kMatchTol pixels.
///   2. Collect candidate shifts (dx, dy) = ref.pos − img.pos per pair.
///   3. Sigma-clip: compute median shift, reject > 2.5·MAD outliers.
///   4. Return mean of survivors, or nullopt if fewer than 3 pairs.
///
/// Returns nullopt when insufficient stars are available, so the caller can
/// fall back to FFT phase correlation.
std::optional<Shift> starShift(const FitsImage& ref, const FitsImage& img) {
    constexpr double kMatchTol   = 10.0;   // pixels — max match distance
    constexpr double kMatchTol2  = kMatchTol * kMatchTol;
    constexpr int    kMinPairs   = 3;

    if (ref.detectedStars.isEmpty() || img.detectedStars.isEmpty())
        return std::nullopt;

    struct Pair { double dx, dy; };
    std::vector<Pair> pairs;
    pairs.reserve(static_cast<size_t>(ref.detectedStars.size()));

    for (const auto& rs : ref.detectedStars) {
        // Find nearest star in img within kMatchTol
        const DetectedStar* best = nullptr;
        double bestD2 = kMatchTol2;
        for (const auto& is : img.detectedStars) {
            // Pre-apply a rough guess: if we already have pairs, shift the
            // search centre — but for the first pass use raw coordinates.
            const double ddx = rs.x - is.x;
            const double ddy = rs.y - is.y;
            const double d2  = ddx * ddx + ddy * ddy;
            if (d2 < bestD2) { bestD2 = d2; best = &is; }
        }
        if (best)
            pairs.push_back({ rs.x - best->x, rs.y - best->y });
    }

    if (static_cast<int>(pairs.size()) < kMinPairs)
        return std::nullopt;

    // Median shift
    auto median1D = [](std::vector<double>& v) -> double {
        std::nth_element(v.begin(), v.begin() + static_cast<std::ptrdiff_t>(v.size() / 2), v.end());
        return v[v.size() / 2];
    };

    std::vector<double> dxv, dyv;
    dxv.reserve(pairs.size()); dyv.reserve(pairs.size());
    for (const auto& p : pairs) { dxv.push_back(p.dx); dyv.push_back(p.dy); }

    const double medDx = median1D(dxv);
    const double medDy = median1D(dyv);

    // MAD → sigma for each axis
    std::vector<double> adx(pairs.size()), ady(pairs.size());
    for (size_t i = 0; i < pairs.size(); ++i) {
        adx[i] = std::abs(pairs[i].dx - medDx);
        ady[i] = std::abs(pairs[i].dy - medDy);
    }
    const double sigDx = 1.4826 * median1D(adx);
    const double sigDy = 1.4826 * median1D(ady);

    // Sigma-clip and compute mean of survivors
    double sumDx = 0.0, sumDy = 0.0;
    int    n = 0;
    for (const auto& p : pairs) {
        const bool okX = (sigDx < 0.01) || (std::abs(p.dx - medDx) <= 2.5 * sigDx);
        const bool okY = (sigDy < 0.01) || (std::abs(p.dy - medDy) <= 2.5 * sigDy);
        if (okX && okY) {
            sumDx += p.dx;
            sumDy += p.dy;
            ++n;
        }
    }

    if (n < kMinPairs)
        return std::nullopt;

    return Shift{ sumDx / n, sumDy / n };
}

} // anonymous namespace

// ---------------------------------------------------------------------------
// stackImages
// ---------------------------------------------------------------------------

std::expected<StackResult, QString>
stackImages(const QVector<FitsImage>& images,
            StackMode mode,
            AlignMode align,
            const std::function<void(int)>& progress)
{
    if (images.isEmpty())
        return std::unexpected(QStringLiteral("No images provided"));
    if (images.size() == 1)
        return StackResult{ images[0], { QPointF(0, 0) } };

    const FitsImage& ref = images[0];
    const int w = ref.width;
    const int h = ref.height;
    const int npix = w * h;

    for (int i = 1; i < images.size(); ++i) {
        if (images[i].width != w || images[i].height != h)
            return std::unexpected(
                QStringLiteral("Image %1 has different dimensions").arg(i));
    }

    StackResult result;
    result.shifts.resize(images.size());
    result.shifts[0] = QPointF(0, 0);

    // Shifted images: index 0 is reference (no copy needed)
    std::vector<FitsImage> shifted(static_cast<size_t>(images.size()));
    shifted[0] = ref;   // shallow copy OK for data access

    if (progress) progress(0);

    for (int i = 1; i < images.size(); ++i) {
        Shift s{ 0.0, 0.0 };

        if (align == AlignMode::StarCatalog) {
            // Try star-based registration; fall back to FFT if insufficient pairs.
            if (auto ss = starShift(ref, images[i]))
                s = *ss;
            else
                s = phaseCorrelation(ref, images[i]);
        } else {
            s = phaseCorrelation(ref, images[i]);
        }

        result.shifts[i] = QPointF(s.dx, s.dy);

        shifted[i].width  = w;
        shifted[i].height = h;
        shiftImage(images[i], shifted[i], s.dx, s.dy);

        if (progress) progress(static_cast<int>(50.0 * i / static_cast<double>(images.size() - 1)));
    }

    // Gather pointers for combineBuffers
    std::vector<const float*> ptrs(static_cast<size_t>(images.size()));
    for (int i = 0; i < images.size(); ++i)
        ptrs[i] = shifted[i].data.data();

    if (progress) progress(60);

    auto combined = combineBuffers(ptrs, npix, mode);

    if (progress) progress(90);

    // Build output FitsImage from reference metadata
    result.image = ref;
    result.image.data = std::move(combined);
    result.image.filePath.clear();
    result.image.fileName = QStringLiteral("stacked.fits");
    computeAutoStretch(result.image);

    if (progress) progress(100);

    return result;
}

// ---------------------------------------------------------------------------
// trackAndStack
// ---------------------------------------------------------------------------

std::expected<StackResult, QString>
trackAndStack(const QVector<FitsImage>& images,
              double dxPerFrame,
              double dyPerFrame,
              StackMode mode)
{
    if (images.isEmpty())
        return std::unexpected(QStringLiteral("No images provided"));

    const int w = images[0].width;
    const int h = images[0].height;
    const int npix = w * h;

    for (int i = 1; i < images.size(); ++i) {
        if (images[i].width != w || images[i].height != h)
            return std::unexpected(
                QStringLiteral("Image %1 has different dimensions").arg(i));
    }

    StackResult result;
    result.shifts.resize(images.size());

    std::vector<FitsImage> shifted(static_cast<size_t>(images.size()));
    shifted[0] = images[0];
    result.shifts[0] = QPointF(0, 0);

    for (int i = 1; i < images.size(); ++i) {
        const double dx = dxPerFrame * i;
        const double dy = dyPerFrame * i;
        result.shifts[i] = QPointF(dx, dy);
        shifted[i].width  = w;
        shifted[i].height = h;
        shiftImage(images[i], shifted[i], dx, dy);
    }

    std::vector<const float*> ptrs(static_cast<size_t>(images.size()));
    for (int i = 0; i < images.size(); ++i)
        ptrs[i] = shifted[i].data.data();

    auto combined = combineBuffers(ptrs, npix, mode);

    result.image = images[0];
    result.image.data = std::move(combined);
    result.image.filePath.clear();
    result.image.fileName = QStringLiteral("track_stacked.fits");
    computeAutoStretch(result.image);

    return result;
}

} // namespace core
