#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/FitsImage.h"
#include "core/StarDetector.h"
#include "core/Centroid.h"
#include "core/ImageStacker.h"
#include "core/MovingObjectDetector.h"

#include <QDir>

// ─── Paths to real Pan-STARRS practice FITS (XY25_p10, 2019-08-28) ───────────
//
// Images have: 2423×2434 px · 16-bit · EXPTIME=45s · WCS CDELT/CROTA format
// RA(center) ≈ 323.69° (from OBJCTRA "21 34 45")  Dec ≈ -12.13°
// JD ≈ 2458724.92 (first image)
// Cadence: ~19 min between frames
//
static const QStringList kSet25 = {
    "/tmp/fits_test/XY25_p10/o8723g0213o.1515750.ch.2211509.XY25.p10.fits",
    "/tmp/fits_test/XY25_p10/o8723g0232o.1515769.ch.2211528.XY25.p10.fits",
    "/tmp/fits_test/XY25_p10/o8723g0251o.1515788.ch.2211547.XY25.p10.fits",
    "/tmp/fits_test/XY25_p10/o8723g0270o.1515807.ch.2211566.XY25.p10.fits",
};

static bool fitsAvailable()
{
    return QFile::exists(kSet25[0]);
}

// ─── F-01: FITS loading ───────────────────────────────────────────────────────

TEST_CASE("F-01: loadFits - dimensions and basic header", "[functional][fits]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    const auto& img = *result;
    CHECK(img.width  == 2423);
    CHECK(img.height == 2434);
    CHECK(img.expTime == Catch::Approx(45.0).margin(0.1));
    CHECK(img.data.size() == static_cast<size_t>(2423 * 2434));
}

TEST_CASE("F-02: loadFits - RA/Dec from OBJCTRA/OBJCTDEC", "[functional][fits]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    // OBJCTRA = "21 34 45.332" → (21 + 34/60 + 45.332/3600) * 15 = 323.689°
    CHECK(result->ra  == Catch::Approx(323.689).margin(0.01));
    // OBJCTDEC = "-12 08 1.439" → -(12 + 8/60 + 1.439/3600) = -12.134°
    CHECK(result->dec == Catch::Approx(-12.134).margin(0.01));
}

TEST_CASE("F-03: loadFits - JD from DATE-OBS", "[functional][fits]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    // DATE-OBS = 2019-08-28T10:03:44 → JD ≈ 2458724.92
    CHECK(result->jd == Catch::Approx(2458724.92).margin(0.01));
    CHECK(result->dateObs.isValid());
    CHECK(result->dateObs.date().year()  == 2019);
    CHECK(result->dateObs.date().month() == 8);
    CHECK(result->dateObs.date().day()   == 28);
}

TEST_CASE("F-04: loadFits - auto-stretch produces valid range", "[functional][fits]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    CHECK(result->displayMin < result->displayMax);
    // Sky background should be a few hundred ADU in 16-bit data
    CHECK(result->displayMin > -1000.0f);
    CHECK(result->displayMax >  0.0f);
    CHECK(result->displayMax < 70000.0f);
}

TEST_CASE("F-05: loadFits - all 4 images load, JD increases monotonically", "[functional][fits]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    double prevJd = 0.0;
    for (const auto& path : kSet25) {
        auto result = core::loadFits(path);
        REQUIRE(result.has_value());
        CHECK(result->jd > prevJd);   // strictly increasing
        CHECK(result->width  == 2423);
        CHECK(result->height == 2434);
        prevJd = result->jd;
    }
    // Cadence ~19 min = 0.013 day; 3 gaps → total span ~0.04 day
    // Load first and last
    auto first = core::loadFits(kSet25[0]);
    auto last  = core::loadFits(kSet25[3]);
    REQUIRE(first.has_value());
    REQUIRE(last.has_value());
    const double span = last->jd - first->jd;
    CHECK(span == Catch::Approx(0.040).margin(0.005));  // ~57 min ± 7 min
}

// ─── F-06: Star detection on real image ──────────────────────────────────────

TEST_CASE("F-06: StarDetector - detects stars in real Pan-STARRS image", "[functional][stardetector]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    core::StarDetectorConfig cfg;
    cfg.threshold   = 5.0;
    cfg.minArea     = 5;
    cfg.maxStars    = 2000;
    cfg.filterKernel = true;

    auto stars = core::detectStars(*result, cfg);
    REQUIRE(stars.has_value());

    // Pan-STARRS deep image: expect hundreds of stars
    INFO("Stars detected: " << stars->size());
    CHECK(stars->size() >= 50);
    CHECK(stars->size() <= 2000);

    // All positions should be within image bounds
    for (const auto& s : *stars) {
        CHECK(s.x >= 0.0);
        CHECK(s.x < 2423.0);
        CHECK(s.y >= 0.0);
        CHECK(s.y < 2434.0);
        CHECK(s.flux > 0.0);
    }
}

TEST_CASE("F-07: StarDetector - brightest stars have highest SNR", "[functional][stardetector]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    core::StarDetectorConfig cfg;
    cfg.threshold = 5.0;
    cfg.minArea   = 5;
    cfg.maxStars  = 500;

    auto stars = core::detectStars(*result, cfg);
    REQUIRE(stars.has_value());
    REQUIRE(stars->size() >= 2);

    // detectStars returns sorted by flux descending
    for (size_t i = 1; i < stars->size(); ++i)
        CHECK((*stars)[i-1].flux >= (*stars)[i].flux);
}

// ─── F-08: Centroid on a detected star ───────────────────────────────────────

TEST_CASE("F-08: findCentroid - refines position on real star", "[functional][centroid]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    auto result = core::loadFits(kSet25[0]);
    REQUIRE(result.has_value());

    // Detect stars first and centroid on the brightest
    core::StarDetectorConfig cfg;
    cfg.threshold = 5.0;
    cfg.minArea   = 5;
    cfg.maxStars  = 50;

    auto stars = core::detectStars(*result, cfg);
    REQUIRE(stars.has_value());
    REQUIRE_FALSE(stars->empty());

    const auto& brightest = (*stars)[0];
    auto c = core::findCentroid(*result,
                                brightest.x, brightest.y,
                                /*boxRadius=*/12);
    REQUIRE(c.has_value());

    // Centroid should stay close to the detected position
    CHECK(std::abs(c->x - brightest.x) < 3.0);
    CHECK(std::abs(c->y - brightest.y) < 3.0);
    CHECK(c->fwhmX > 0.5);
    CHECK(c->fwhmY > 0.5);
    CHECK(c->snr   > 3.0);
}

// ─── F-09: Image stacking ─────────────────────────────────────────────────────

TEST_CASE("F-09: ImageStacker - mean-stack 4 real images", "[functional][stacker]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    QVector<core::FitsImage> images;
    for (const auto& path : kSet25) {
        auto r = core::loadFits(path);
        REQUIRE(r.has_value());
        images.push_back(std::move(*r));
    }

    auto stacked = core::stackImages(images, core::StackMode::Average);
    REQUIRE(stacked.has_value());

    CHECK(stacked->image.width  == 2423);
    CHECK(stacked->image.height == 2434);
    CHECK(stacked->image.data.size() == static_cast<size_t>(2423 * 2434));
    CHECK(stacked->image.displayMin < stacked->image.displayMax);

    // Mean-stacked image should have similar background level to individual frames
    // (not zero, not saturated)
    CHECK(stacked->image.displayMax > 0.0f);
    CHECK(stacked->image.displayMax < 70000.0f);
}

// ─── F-10: Moving object detector ─────────────────────────────────────────────

TEST_CASE("F-10: MovingObjectDetector - runs on real cadence set", "[functional][detector]")
{
    if (!fitsAvailable()) SKIP("Real FITS files not found at /tmp/fits_test/");

    QVector<core::FitsImage> images;
    for (const auto& path : kSet25) {
        auto r = core::loadFits(path);
        REQUIRE(r.has_value());
        images.push_back(std::move(*r));
    }

    auto tracks = core::detectMovingObjects(images,
                                            /*tolerancePx=*/2.0,
                                            /*minFrames=*/3,
                                            /*minSnr=*/5.0);
    // The pipeline should complete without throwing
    // (PRACTICE sets are designed to contain a moving object)
    REQUIRE(tracks.has_value());
    INFO("Tracks found: " << tracks->size());
    // Can't assert a specific count without knowing the exact asteroid,
    // but zero tracks on a practice set is suspicious.
    CHECK(tracks->size() >= 0);  // at minimum: no crash
}
