/// @file test_robustness.cpp
/// Input validation / edge-case / fuzzing tests.
///
/// These tests exercise every public API at invalid or extreme inputs to
/// confirm that:
///   - No undefined behaviour (crash, assertion, memory corruption) occurs.
///   - Error paths return nullopt / std::unexpected rather than garbage.
///   - Image boundary clamps are applied consistently.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/FitsImage.h"
#include "core/StarDetector.h"
#include "core/ImageStacker.h"
#include "core/MovingObjectDetector.h"
#include "core/Photometry.h"
#include "core/Calibration.h"

// ─── FitsImage invariants ─────────────────────────────────────────────────────

TEST_CASE("FitsImage: default-constructed is invalid", "[robustness][fitsimage]")
{
    core::FitsImage img;
    CHECK_FALSE(img.isValid());
}

TEST_CASE("FitsImage: zero dimensions are invalid", "[robustness][fitsimage]")
{
    core::FitsImage img;
    img.width  = 0;
    img.height = 0;
    img.data.clear();
    CHECK_FALSE(img.isValid());
}

TEST_CASE("FitsImage: pixelAt clamps all out-of-bounds reads to 0", "[robustness][fitsimage]")
{
    core::FitsImage img;
    img.width  = 4;
    img.height = 4;
    img.data.assign(16, 42.0f);

    CHECK(img.pixelAt(-1,  0) == 0.0f);
    CHECK(img.pixelAt( 0, -1) == 0.0f);
    CHECK(img.pixelAt( 4,  0) == 0.0f);
    CHECK(img.pixelAt( 0,  4) == 0.0f);
    CHECK(img.pixelAt(99, 99) == 0.0f);
    // In-bounds access still works
    CHECK(img.pixelAt( 0,  0) == 42.0f);
    CHECK(img.pixelAt( 3,  3) == 42.0f);
}

// ─── computeAutoStretch edge cases ────────────────────────────────────────────

TEST_CASE("computeAutoStretch: does not crash on empty image", "[robustness][fitsimage]")
{
    core::FitsImage img;   // invalid, zero-size
    REQUIRE_NOTHROW(core::computeAutoStretch(img));
}

TEST_CASE("computeAutoStretch: single-pixel image", "[robustness][fitsimage]")
{
    core::FitsImage img;
    img.width  = 1;
    img.height = 1;
    img.data   = { 5000.0f };
    REQUIRE_NOTHROW(core::computeAutoStretch(img));
}

TEST_CASE("computeAutoStretch: all-identical pixels", "[robustness][fitsimage]")
{
    core::FitsImage img;
    img.width  = 20;
    img.height = 20;
    img.data.assign(400, 1234.0f);
    REQUIRE_NOTHROW(core::computeAutoStretch(img));
    // displayMin == displayMax is allowed when all pixels equal
}

// ─── loadFits with bad paths ──────────────────────────────────────────────────

TEST_CASE("loadFits: nonexistent path returns error", "[robustness][fitsimage]")
{
    auto r = core::loadFits("/nonexistent/path/does_not_exist.fits");
    CHECK_FALSE(r.has_value());
}

TEST_CASE("loadFits: empty path returns error", "[robustness][fitsimage]")
{
    auto r = core::loadFits("");
    CHECK_FALSE(r.has_value());
}

// ─── stackImages edge cases ───────────────────────────────────────────────────

TEST_CASE("stackImages: empty input returns error", "[robustness][stacker]")
{
    QVector<core::FitsImage> empty;
    CHECK_FALSE(core::stackImages(empty, core::StackMode::Average).has_value());
}

TEST_CASE("stackImages: mismatched dimensions returns error", "[robustness][stacker]")
{
    core::FitsImage a, b;
    a.width = 10; a.height = 10; a.data.assign(100, 1.0f);
    b.width = 20; b.height = 20; b.data.assign(400, 1.0f);

    QVector<core::FitsImage> imgs = { a, b };
    CHECK_FALSE(core::stackImages(imgs, core::StackMode::Average).has_value());
}

TEST_CASE("stackImages: single image returns that image unchanged", "[robustness][stacker]")
{
    core::FitsImage img;
    img.width  = 4;
    img.height = 4;
    img.data.assign(16, 7.0f);

    QVector<core::FitsImage> imgs = { img };
    auto res = core::stackImages(imgs, core::StackMode::Average);
    REQUIRE(res.has_value());
    CHECK(res->image.width  == 4);
    CHECK(res->image.height == 4);
    CHECK(res->shifts.size() == 1);
    CHECK(res->shifts[0].x() == 0.0);
    CHECK(res->shifts[0].y() == 0.0);
}

// ─── trackAndStack edge cases ─────────────────────────────────────────────────

TEST_CASE("trackAndStack: empty input returns error", "[robustness][stacker]")
{
    QVector<core::FitsImage> empty;
    CHECK_FALSE(core::trackAndStack(empty, 0.5, 0.5).has_value());
}

TEST_CASE("trackAndStack: zero motion is identity shift", "[robustness][stacker]")
{
    core::FitsImage img;
    img.width  = 8;
    img.height = 8;
    img.data.assign(64, 100.0f);

    QVector<core::FitsImage> imgs = { img, img, img };
    auto res = core::trackAndStack(imgs, 0.0, 0.0);
    REQUIRE(res.has_value());
    CHECK(res->image.width  == 8);
    CHECK(res->image.height == 8);
}

// ─── detectStars edge cases ───────────────────────────────────────────────────

TEST_CASE("detectStars: rejects invalid image", "[robustness][stardetector]")
{
    core::FitsImage empty;
    CHECK_FALSE(core::detectStars(empty).has_value());
}

TEST_CASE("detectStars: very high threshold finds no stars", "[robustness][stardetector]")
{
    core::FitsImage img;
    img.width  = 50;
    img.height = 50;
    img.gain   = 1.0;
    img.saturation = 65535.0;
    img.data.assign(50 * 50, 1000.0f);
    img.displayMin = 990.0f;
    img.displayMax = 1010.0f;

    core::StarDetectorConfig cfg;
    cfg.threshold = 1000.0;   // impossibly high threshold

    auto res = core::detectStars(img, cfg);
    REQUIRE(res.has_value());
    CHECK(res->isEmpty());
}

// ─── detectMovingObjects edge cases ───────────────────────────────────────────

TEST_CASE("detectMovingObjects: empty input returns error or empty", "[robustness][detector]")
{
    QVector<core::FitsImage> empty;
    auto res = core::detectMovingObjects(empty, 2.0, 3, 5.0);
    // std::expected: either error (no value) or empty candidates — both fine
    if (res.has_value())
        CHECK(res->isEmpty());
}

TEST_CASE("detectMovingObjects: single frame, minFrames=3, yields no tracks", "[robustness][detector]")
{
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.gain   = 1.0;
    img.saturation = 65535.0;
    img.data.assign(100 * 100, 500.0f);
    img.displayMin = 490.0f;
    img.displayMax = 510.0f;

    QVector<core::FitsImage> frames = { img };
    auto res = core::detectMovingObjects(frames, 2.0, /*minFrames=*/3, 5.0);
    if (res.has_value())
        CHECK(res->isEmpty());   // can't span 3 frames with only 1
}

TEST_CASE("detectMovingObjects: images without detectedStars yield no tracks", "[robustness][detector]")
{
    // 3 frames, but all detectedStars vectors empty → no pairs → no tracks
    core::FitsImage img;
    img.width  = 50;
    img.height = 50;
    img.data.assign(50 * 50, 200.0f);

    QVector<core::FitsImage> frames = { img, img, img };
    auto res = core::detectMovingObjects(frames, 2.0, 3, 5.0);
    REQUIRE(res.has_value());
    CHECK(res->isEmpty());
}

// ─── Calibration edge cases ───────────────────────────────────────────────────

TEST_CASE("applyDark: empty dark rejected when image is non-empty", "[robustness][calibration]")
{
    core::FitsImage img, dark;
    img.width  = 4;
    img.height = 4;
    img.data.assign(16, 500.0f);
    // dark is default-constructed (0×0) → dimension mismatch → false
    CHECK_FALSE(core::applyDark(img, dark));
}

TEST_CASE("applyFlat: single-pixel image round-trips correctly", "[robustness][calibration]")
{
    core::FitsImage img, flat;
    img.width  = flat.width  = 1;
    img.height = flat.height = 1;
    img.data  = { 1000.0f };
    flat.data = {  500.0f };

    REQUIRE(core::applyFlat(img, flat));
    // flat normalised by its own median (=500) → norm=1.0 → img / 1.0 = 1000
    CHECK_THAT(img.data[0], Catch::Matchers::WithinAbs(1000.0f, 1.0f));
}

// ─── aperturePhotometry extreme positions ─────────────────────────────────────

TEST_CASE("aperturePhotometry: centroid far outside image returns nullopt", "[robustness][photometry]")
{
    core::FitsImage img;
    img.width  = 50;
    img.height = 50;
    img.data.assign(50 * 50, 1000.0f);

    CHECK_FALSE(core::aperturePhotometry(img,  1000.0,  1000.0, 5.0).has_value());
    CHECK_FALSE(core::aperturePhotometry(img, -1000.0, -1000.0, 5.0).has_value());
}

TEST_CASE("aperturePhotometry: aperture larger than image clips gracefully", "[robustness][photometry]")
{
    // Flat image + huge aperture → still no net flux, but must not crash
    core::FitsImage img;
    img.width  = 10;
    img.height = 10;
    img.data.assign(100, 1000.0f);

    REQUIRE_NOTHROW(core::aperturePhotometry(img, 5.0, 5.0, 100.0));
}
