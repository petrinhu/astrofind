#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/StarDetector.h"
#include "core/FitsImage.h"

#include <cmath>
#include <limits>
#include <vector>

// Build a synthetic FitsImage with Gaussian blobs at known positions
static core::FitsImage makeSyntheticImage(int w, int h,
    const std::vector<std::pair<double,double>>& centers,
    float amplitude = 5000.0f, float sigma = 2.5f, float sky = 1000.0f)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.gain   = 1.0;
    img.saturation = 65535.0;
    img.data.resize(static_cast<size_t>(w * h), sky);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            for (auto [cx, cy] : centers) {
                const float dx = x - (float)cx;
                const float dy = y - (float)cy;
                img.data[y * w + x] += amplitude *
                    std::exp(-(dx*dx + dy*dy) / (2.0f * sigma * sigma));
            }
        }
    }

    // Set display stretch
    img.displayMin = 900.0f;
    img.displayMax = 6500.0f;

    return img;
}

TEST_CASE("StarDetector: detects stars in synthetic image", "[stardetector]")
{
    // Create 200×200 image with 4 Gaussian stars at known positions
    const std::vector<std::pair<double,double>> positions = {
        {50.0,  50.0},
        {100.0, 80.0},
        {150.0, 120.0},
        {70.0,  160.0},
    };

    auto img = makeSyntheticImage(200, 200, positions, 5000.0f, 2.5f, 100.0f);
    core::computeAutoStretch(img);

    core::StarDetectorConfig cfg;
    cfg.threshold = 3.0;
    cfg.minArea   = 3.0;
    cfg.maxStars  = 50;

    auto result = core::detectStars(img, cfg);

    REQUIRE(result.has_value());
    REQUIRE(result->size() >= 4);   // at least all 4 stars found

    // Stars should be sorted by flux (descending)
    for (int i = 1; i < result->size(); ++i)
        CHECK((*result)[i-1].flux >= (*result)[i].flux);

    // Each star should be within 2 pixels of a known position
    int matched = 0;
    for (const auto& star : *result) {
        for (auto [cx, cy] : positions) {
            const double dist = std::hypot(star.x - cx, star.y - cy);
            if (dist < 2.0) { ++matched; break; }
        }
    }
    CHECK(matched >= 4);
}

TEST_CASE("StarDetector: flat image at high threshold returns no stars", "[stardetector]")
{
    // An image with only background noise should return no stars at high threshold
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.gain   = 1.0;
    img.saturation = 65535.0;
    img.data.assign(100 * 100, 500.0f);   // flat background
    img.displayMin = 490.0f;
    img.displayMax = 510.0f;

    core::StarDetectorConfig cfg;
    cfg.threshold = 50.0;   // very high threshold → no detections

    auto result = core::detectStars(img, cfg);
    REQUIRE(result.has_value());
    CHECK(result->size() == 0);
}

TEST_CASE("StarDetector: rejects invalid image", "[stardetector]")
{
    core::FitsImage empty;
    auto result = core::detectStars(empty);
    REQUIRE_FALSE(result.has_value());
}

// AUD-MEM-7: the ClumpFind pass must never static_cast<int> a non-finite or
// off-image centroid, nor let a huge semi-major axis overflow the search box.
TEST_CASE("StarDetector: markBlendedSources skips non-finite and off-image stars",
          "[stardetector][AUD-MEM-7]")
{
    const int w = 64, h = 64;
    std::vector<float> data(static_cast<size_t>(w * h), 0.0f);
    // Two separated peaks around (32,32) so a sane star there is blended.
    data[32 * w + 28] = 100.0f;
    data[32 * w + 36] = 100.0f;

    auto star = [](double x, double y, double a) {
        core::DetectedStar s;
        s.x = x; s.y = y; s.a = a; s.b = 1.0;
        return s;
    };
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double inf = std::numeric_limits<double>::infinity();

    QVector<core::DetectedStar> stars = {
        star(32.0, 32.0, 3.0),      // control: two peaks → blended
        star(nan,  32.0, 3.0),
        star(32.0, inf,  3.0),
        star(32.0, 32.0, nan),
        star(-1e12, 32.0, 3.0),     // finite but far off-image
        star(32.0, 32.0, 1e12),     // huge axis: box must clamp, not overflow int
    };
    core::markBlendedSources(data.data(), w, h, 10.0f, 0.5, stars);

    CHECK(stars[0].blended);
    CHECK_FALSE(stars[1].blended);
    CHECK_FALSE(stars[2].blended);
    CHECK_FALSE(stars[3].blended);
    CHECK_FALSE(stars[4].blended);
    // Huge axis → min peak separation spans the image, so a single peak
    // survives; the point is that the box cast did not overflow (UBSan).
    CHECK_FALSE(stars[5].blended);
}

TEST_CASE("DetectedStar::fwhm uses the exact Gaussian constant", "[stardetector][AUD-CORR-9]")
{
    core::DetectedStar s;
    s.a = 2.0; s.b = 2.0;
    CHECK_THAT(s.fwhm(), Catch::Matchers::WithinAbs(2.0 * std::sqrt(2.0 * std::log(2.0)) * 2.0, 1e-8));
}
