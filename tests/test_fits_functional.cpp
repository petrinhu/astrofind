// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-2 remediation: these 10 cases used to SKIP unconditionally unless
// real Pan-STARRS practice FITS existed at /tmp/fits_test/ (a path that only
// ever existed on the original author's machine). Catch2 counts SKIP as
// "pass", so the table showed "10 ✅" while 0/10 ever actually ran in CI.
//
// Fixtures are now synthesized on disk via cfitsio (tests/synthetic_fits.h),
// so the full loadFits() -> detectStars() -> findCentroid() -> stackImages()
// -> detectMovingObjects() pipeline runs unconditionally, everywhere.
//
// F-10 also fixes a latent "test that cannot fail" bug in the original:
// core::detectMovingObjects() only ever reads FitsImage::detectedStars — it
// does not run detection itself. The original test called it on freshly
// loadFits()-ed images (detectedStars always empty) and asserted
// `tracks->size() >= 0`, which holds unconditionally regardless of the
// implementation. This version runs core::detectStars() per frame first, so
// the moving-object assertion can actually fail if the pipeline breaks.

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/FitsImage.h"
#include "core/StarDetector.h"
#include "core/Centroid.h"
#include "core/ImageStacker.h"
#include "core/MovingObjectDetector.h"
#include "synthetic_fits.h"

#include <QDir>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include <QTimeZone>
#include <QTemporaryDir>

using namespace testutil;

namespace {

constexpr int kW = 320, kH = 320;

// 9x9 grid of field stars (81 total), amplitude varying so flux-sort ordering
// is non-trivial to get right (F-07). Fixed across all cadence frames.
std::vector<StarSpec> makeFieldStars()
{
    std::vector<StarSpec> stars;
    for (int gx = 0; gx < 9; ++gx) {
        for (int gy = 0; gy < 9; ++gy) {
            const double x = 30.0 + gx * 32.5;
            const double y = 30.0 + gy * 32.5;
            const float amp = 3000.0f + static_cast<float>((gx * 9 + gy) * 50);
            stars.push_back(StarSpec{x, y, amp, 2.3f});
        }
    }
    return stars;
}

// A single "asteroid" moving +5 px/frame in X, placed in a grid gap so it
// never blends with a field star.
StarSpec movingObjectAt(int frame)
{
    return StarSpec{110.0 + 5.0 * frame, 110.0, 7000.0f, 2.3f};
}

constexpr double kRaDeg  = 323.689;
constexpr double kDecDeg = -12.134;
constexpr double kExpTimeSec = 45.0;
const QString kBaseDateObs = QStringLiteral("2019-08-28T10:03:44Z");
constexpr int  kCadenceMinutes = 19;
constexpr int  kFrameCount = 4;

/// Write the 4-frame synthetic cadence set (static field + 1 mover) to `dir`
/// and load every frame back through core::loadFits(). REQUIREs success at
/// every step, so a fixture-generation failure surfaces as a normal test
/// failure rather than a silent skip.
QVector<core::FitsImage> loadCadenceSet(const QString& dir)
{
    const auto fieldStars = makeFieldStars();
    const QDateTime base = QDateTime::fromString(kBaseDateObs, Qt::ISODate);
    REQUIRE(base.isValid());

    QVector<core::FitsImage> images;
    for (int f = 0; f < kFrameCount; ++f) {
        auto stars = fieldStars;
        stars.push_back(movingObjectAt(f));
        auto data = renderGaussianImage(kW, kH, 600.0f, stars);
        addDeterministicNoise(data, 18.0f, /*seed=*/1000u + static_cast<unsigned>(f));

        SynthHeader hdr;
        hdr.raDeg      = kRaDeg;
        hdr.decDeg     = kDecDeg;
        hdr.expTimeSec = kExpTimeSec;
        hdr.dateObsIso = base.addSecs(f * kCadenceMinutes * 60).toString(Qt::ISODate);

        const QString path = dir + QStringLiteral("/frame%1.fits").arg(f);
        QString err;
        REQUIRE(writeSynthImage(path, kW, kH, data, hdr, &err));

        auto result = core::loadFits(path);
        REQUIRE(result.has_value());
        images.push_back(std::move(*result));
    }
    return images;
}

} // namespace

// ─── F-01: FITS loading ───────────────────────────────────────────────────────

TEST_CASE("F-01: loadFits - dimensions and basic header", "[functional][fits]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    const auto& img = images[0];
    CHECK(img.width  == kW);
    CHECK(img.height == kH);
    CHECK(img.expTime == Catch::Approx(kExpTimeSec).margin(0.1));
    CHECK(img.data.size() == static_cast<size_t>(kW * kH));
}

// ─── F-02: RA/Dec from OBJCTRA/OBJCTDEC ───────────────────────────────────────

TEST_CASE("F-02: loadFits - RA/Dec from OBJCTRA/OBJCTDEC", "[functional][fits]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    CHECK(images[0].ra  == Catch::Approx(kRaDeg).margin(0.01));
    CHECK(images[0].dec == Catch::Approx(kDecDeg).margin(0.01));
}

// ─── F-03: JD from DATE-OBS ────────────────────────────────────────────────────

TEST_CASE("F-03: loadFits - JD from DATE-OBS", "[functional][fits]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    const auto& img = images[0];
    CHECK(img.dateObs.isValid());
    CHECK(img.dateObs.date().year()  == 2019);
    CHECK(img.dateObs.date().month() == 8);
    CHECK(img.dateObs.date().day()   == 28);

    // Mid-exposure JD = 2451545.0 (J2000.0) + secsTo(dateObs)/86400 + expTime/172800
    // (same formula as core::readHeader(); this test validates the round-trip
    // through the FITS header string, not the formula itself — that is unit
    // tested in test_astronomy.cpp).
    const QDateTime j2000(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
    const double expectedJd = 2451545.0
        + static_cast<double>(j2000.secsTo(img.dateObs)) / 86400.0
        + kExpTimeSec / 172800.0;
    CHECK(img.jd == Catch::Approx(expectedJd).margin(1e-6));
}

// ─── F-04: auto-stretch ────────────────────────────────────────────────────────

TEST_CASE("F-04: loadFits - auto-stretch produces valid range", "[functional][fits]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    const auto& img = images[0];
    CHECK(img.displayMin < img.displayMax);
    CHECK(img.displayMin > -1000.0f);
    CHECK(img.displayMax >  0.0f);
    CHECK(img.displayMax < 70000.0f);
}

// ─── F-05: cadence — all frames load, JD increases monotonically ─────────────

TEST_CASE("F-05: loadFits - all 4 images load, JD increases monotonically", "[functional][fits]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());
    REQUIRE(images.size() == kFrameCount);

    double prevJd = 0.0;
    for (const auto& img : images) {
        CHECK(img.jd > prevJd);
        CHECK(img.width  == kW);
        CHECK(img.height == kH);
        prevJd = img.jd;
    }

    // 3 gaps of 19 minutes = 57 min = 0.039583... day
    const double span = images.last().jd - images.first().jd;
    CHECK(span == Catch::Approx(0.03958).margin(0.001));
}

// ─── F-06: Star detection on the synthetic field ──────────────────────────────

TEST_CASE("F-06: StarDetector - detects stars in the synthetic field", "[functional][stardetector]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());
    const auto& img = images[0];

    core::StarDetectorConfig cfg;
    cfg.threshold    = 5.0;
    cfg.minArea      = 5;
    cfg.maxStars     = 2000;
    cfg.filterKernel = true;

    auto stars = core::detectStars(img, cfg);
    REQUIRE(stars.has_value());

    // 81 field stars + 1 mover = 82 sources.
    INFO("Stars detected: " << stars->size());
    CHECK(stars->size() >= 50);
    CHECK(stars->size() <= 2000);

    for (const auto& s : *stars) {
        CHECK(s.x >= 0.0);
        CHECK(s.x < static_cast<double>(kW));
        CHECK(s.y >= 0.0);
        CHECK(s.y < static_cast<double>(kH));
        CHECK(s.flux > 0.0);
    }
}

TEST_CASE("F-07: StarDetector - brightest stars have highest SNR", "[functional][stardetector]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());
    const auto& img = images[0];

    core::StarDetectorConfig cfg;
    cfg.threshold = 5.0;
    cfg.minArea   = 5;
    cfg.maxStars  = 500;

    auto stars = core::detectStars(img, cfg);
    REQUIRE(stars.has_value());
    REQUIRE(stars->size() >= 2);

    // detectStars returns sorted by flux descending
    for (size_t i = 1; i < stars->size(); ++i)
        CHECK((*stars)[i-1].flux >= (*stars)[i].flux);
}

// ─── F-08: Centroid on a detected star ────────────────────────────────────────

TEST_CASE("F-08: findCentroid - refines position on a synthetic star", "[functional][centroid]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());
    const auto& img = images[0];

    core::StarDetectorConfig cfg;
    cfg.threshold = 5.0;
    cfg.minArea   = 5;
    cfg.maxStars  = 50;

    auto stars = core::detectStars(img, cfg);
    REQUIRE(stars.has_value());
    REQUIRE_FALSE(stars->empty());

    const auto& brightest = (*stars)[0];
    auto c = core::findCentroid(img, brightest.x, brightest.y, /*boxRadius=*/12);
    REQUIRE(c.has_value());

    CHECK(std::abs(c->x - brightest.x) < 3.0);
    CHECK(std::abs(c->y - brightest.y) < 3.0);
    CHECK(c->fwhmX > 0.5);
    CHECK(c->fwhmY > 0.5);
    CHECK(c->snr   > 3.0);
}

// ─── F-09: Image stacking ──────────────────────────────────────────────────────

TEST_CASE("F-09: ImageStacker - mean-stack the 4 synthetic frames", "[functional][stacker]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    auto stacked = core::stackImages(images, core::StackMode::Average);
    REQUIRE(stacked.has_value());

    CHECK(stacked->image.width  == kW);
    CHECK(stacked->image.height == kH);
    CHECK(stacked->image.data.size() == static_cast<size_t>(kW * kH));
    CHECK(stacked->image.displayMin < stacked->image.displayMax);
    CHECK(stacked->image.displayMax > 0.0f);
    CHECK(stacked->image.displayMax < 70000.0f);
}

// ─── F-10: Moving object detector ──────────────────────────────────────────────

TEST_CASE("F-10: MovingObjectDetector - finds the synthetic mover in the cadence set",
          "[functional][detector]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    auto images = loadCadenceSet(dir.path());

    // detectMovingObjects() only reads FitsImage::detectedStars — run real
    // detection on every frame first (a freshly loadFits()-ed image never has
    // detectedStars populated, which is what made the original assertion
    // `tracks->size() >= 0` unconditionally true).
    core::StarDetectorConfig cfg;
    cfg.threshold = 5.0;
    cfg.minArea   = 5;
    cfg.maxStars  = 500;
    for (auto& img : images) {
        auto stars = core::detectStars(img, cfg);
        REQUIRE(stars.has_value());
        img.detectedStars = std::move(*stars);
    }

    auto tracks = core::detectMovingObjects(images, /*tolerancePx=*/3.0,
                                            /*minFrames=*/3, /*minSnr=*/5.0);
    REQUIRE(tracks.has_value());
    INFO("Tracks found: " << tracks->size());
    REQUIRE_FALSE(tracks->isEmpty());

    // The synthetic mover moves +5 px/frame in X, 0 in Y.
    bool found = false;
    for (const auto& t : *tracks) {
        if (std::abs(t.dxPerFrame - 5.0) < 1.0 && std::abs(t.dyPerFrame) < 1.0) {
            found = true;
            CHECK(t.positions.size() >= 3);
            CHECK(t.speed > 0.0);
        }
    }
    CHECK(found);
}
