#include "StarDetector.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>

extern "C" {
#include <sep.h>
}

namespace core {

// Default 3×3 matched filter (Gaussian-like, same as SourceExtractor default)
static const float kConv3x3[9] = {
    1.0f, 2.0f, 1.0f,
    2.0f, 4.0f, 2.0f,
    1.0f, 2.0f, 1.0f,
};

std::expected<QVector<DetectedStar>, QString>
detectStars(const FitsImage& img, const StarDetectorConfig& cfg)
{
    if (!img.isValid())
        return std::unexpected(QStringLiteral("Invalid image"));

    const int w = img.width;
    const int h = img.height;

    // Work on a copy for background subtraction
    std::vector<float> data(img.data);

    // ── 1. Build sep_image for background estimation ───────────────────────
    sep_image bgIm = {};
    bgIm.data  = data.data();
    bgIm.noise = nullptr;
    bgIm.mask  = nullptr;
    bgIm.dtype = SEP_TFLOAT;
    bgIm.w     = w;
    bgIm.h     = h;
    bgIm.gain  = img.gain;

    // ── 2. Estimate background ─────────────────────────────────────────────
    sep_bkg* bkg = nullptr;
    int err = sep_background(&bgIm,
                             64, 64,   // bw, bh: tile size
                             3,  3,    // fw, fh: filter size in tiles
                             0.0,      // fthresh
                             &bkg);
    if (err) {
        char errbuf[256] = {};
        sep_get_errmsg(err, errbuf);
        return std::unexpected(QString::fromLatin1("SEP background: ") + errbuf);
    }

    const float globalRms = sep_bkg_globalrms(bkg);
    const float globalBg  = sep_bkg_global(bkg);
    spdlog::debug("StarDetector: background={:.1f} rms={:.1f}", globalBg, globalRms);

    // ── 3. Subtract background from copy ──────────────────────────────────
    err = sep_bkg_subarray(bkg, data.data(), SEP_TFLOAT);
    sep_bkg_free(bkg);
    bkg = nullptr;
    if (err) {
        char errbuf[256] = {};
        sep_get_errmsg(err, errbuf);
        return std::unexpected(QString::fromLatin1("SEP bkg_subarray: ") + errbuf);
    }

    // ── 4. Build sep_image for extraction (background-subtracted) ─────────
    sep_image exIm = {};
    exIm.data        = data.data();
    exIm.noise       = nullptr;
    exIm.mask        = nullptr;
    exIm.dtype       = SEP_TFLOAT;
    exIm.ndtype      = SEP_TFLOAT;
    exIm.w           = w;
    exIm.h           = h;
    exIm.noiseval    = static_cast<double>(globalRms);
    exIm.noise_type  = SEP_NOISE_STDDEV;
    exIm.gain        = img.gain;

    // ── 5. Extract sources ────────────────────────────────────────────────
    const float  thresh    = cfg.threshold;    // SEP_THRESH_REL = in sigma
    const float* conv      = cfg.filterKernel ? kConv3x3 : nullptr;
    const int    convw     = cfg.filterKernel ? 3 : 0;
    const int    convh     = cfg.filterKernel ? 3 : 0;

    sep_catalog* cat = nullptr;
    err = sep_extract(&exIm,
                      thresh, SEP_THRESH_REL,
                      static_cast<int>(cfg.minArea),
                      conv, convw, convh,
                      SEP_FILTER_CONV,
                      cfg.deblendNthresh,
                      cfg.deblendContrast,
                      1,     // clean_flag
                      1.0,   // clean_param
                      &cat);
    if (err) {
        char errbuf[256] = {};
        sep_get_errmsg(err, errbuf);
        return std::unexpected(QString::fromLatin1("SEP extract: ") + errbuf);
    }

    // ── 6. Convert to DetectedStar ────────────────────────────────────────
    QVector<DetectedStar> stars;
    stars.reserve(cat->nobj);

    const double rmsArea = (globalRms > 0.0f) ? static_cast<double>(globalRms) : 1.0;

    for (int i = 0; i < cat->nobj; ++i) {
        if (cat->flux[i] <= 0.0f) continue;

        DetectedStar s;
        s.x     = cat->x[i];
        s.y     = cat->y[i];
        s.flux  = static_cast<double>(cat->flux[i]);
        s.a     = static_cast<double>(cat->a[i]);
        s.b     = static_cast<double>(cat->b[i]);
        s.theta = static_cast<double>(cat->theta[i]);
        s.flag  = static_cast<int>(cat->flag[i]);

        // Approximate SNR: flux / (rms * sqrt(π·a·b))
        const double area = M_PI * s.a * s.b;
        s.snr = (area > 0.0) ? s.flux / (rmsArea * std::sqrt(area)) : 0.0;

        stars.append(s);
    }
    sep_catalog_free(cat);

    // ── 7. Sort by flux, limit count ──────────────────────────────────────
    std::sort(stars.begin(), stars.end(),
              [](const DetectedStar& a, const DetectedStar& b) {
                  return a.flux > b.flux;
              });
    if (stars.size() > cfg.maxStars)
        stars.resize(cfg.maxStars);

    spdlog::info("StarDetector: {} stars detected in {}x{} image (sigma={:.1f})",
                 stars.size(), w, h, static_cast<double>(globalRms));

    return stars;
}

} // namespace core
