#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Centroid.h"
#include "core/FitsImage.h"

#include <cmath>

using Catch::Matchers::WithinAbs;

/// Build a synthetic image with a 2-D Gaussian star at (cx, cy).
static core::FitsImage makeGaussianStar(int w, int h,
                                         double cx, double cy,
                                         double sigma, float peak,
                                         float bkg = 100.0f)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.resize(static_cast<size_t>(w * h), bkg);
    img.saturation = 65535.0;

    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            double dx = x - cx, dy = y - cy;
            float v = bkg + peak * std::exp(-(dx*dx + dy*dy) / (2.0 * sigma * sigma));
            img.data[static_cast<size_t>(y) * w + x] = v;
        }
    return img;
}

// ─── findCentroid (moment-based) ──────────────────────────────────────────────

TEST_CASE("findCentroid locates star at image centre", "[centroid]")
{
    const int W = 64, H = 64;
    auto img = makeGaussianStar(W, H, 32.0, 32.0, 3.0, 5000.0f);

    auto result = core::findCentroid(img, 32.0, 32.0);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->x, WithinAbs(32.0, 0.5));
    REQUIRE_THAT(result->y, WithinAbs(32.0, 0.5));
}

TEST_CASE("findCentroid locates off-centre star", "[centroid]")
{
    const int W = 64, H = 64;
    auto img = makeGaussianStar(W, H, 20.3, 35.7, 3.0, 5000.0f);

    auto result = core::findCentroid(img, 21.0, 36.0);  // click near star
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->x, WithinAbs(20.3, 0.5));
    REQUIRE_THAT(result->y, WithinAbs(35.7, 0.5));
}

TEST_CASE("findCentroid clips box to image bounds (does not fail on edge click)", "[centroid]")
{
    // findCentroid clips x0/y0/x1/y1 to image bounds — it always returns a result
    // as long as there's a bright source inside the clipped box.
    const int W = 64, H = 64;
    auto img = makeGaussianStar(W, H, 5.0, 32.0, 3.0, 5000.0f);  // star near left edge

    // Click at x=-2: box is clipped to [0..10], star at x=5 is inside
    auto result = core::findCentroid(img, -2.0, 32.0);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->x, WithinAbs(5.0, 1.0));
}

TEST_CASE("findCentroid SNR is positive for a real star", "[centroid]")
{
    auto img    = makeGaussianStar(64, 64, 32.0, 32.0, 3.0, 8000.0f);
    auto result = core::findCentroid(img, 32.0, 32.0);

    REQUIRE(result.has_value());
    REQUIRE(result->snr > 0.0);
    REQUIRE(result->peak > 0.0);
}

// ─── findCentroidPsf (Gaussian fit) ──────────────────────────────────────────

TEST_CASE("findCentroidPsf locates star accurately", "[centroid]")
{
    // PSF fit may converge to a different sigma than the true value for synthetic
    // data, but the centroid position should still be accurate.
    auto img = makeGaussianStar(64, 64, 30.0, 34.0, 2.5, 6000.0f);

    auto result = core::findCentroidPsf(img, 30.0, 34.0);
    REQUIRE(result.has_value());
    REQUIRE_THAT(result->x, WithinAbs(30.0, 0.5));
    REQUIRE_THAT(result->y, WithinAbs(34.0, 0.5));
    REQUIRE(result->fwhmX > 0.0);
    REQUIRE(result->fwhmY > 0.0);
}

TEST_CASE("findCentroidPsf result agrees with findCentroid to sub-pixel", "[centroid]")
{
    auto img     = makeGaussianStar(64, 64, 27.5, 31.5, 3.0, 7000.0f);
    auto moment  = core::findCentroid(img, 28.0, 32.0);
    auto psf     = core::findCentroidPsf(img, 28.0, 32.0);

    REQUIRE(moment.has_value());
    REQUIRE(psf.has_value());

    // Both should agree within 1 pixel
    REQUIRE_THAT(psf->x, WithinAbs(moment->x, 1.0));
    REQUIRE_THAT(psf->y, WithinAbs(moment->y, 1.0));
}
