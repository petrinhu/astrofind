#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/StarDetector.h"
#include "core/FitsImage.h"

#include <cmath>

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

TEST_CASE("StarDetector: empty image returns stars or succeeds", "[stardetector]")
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
