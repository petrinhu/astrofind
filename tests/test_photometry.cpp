#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Photometry.h"
#include "core/FitsImage.h"

#include <cmath>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

// ─── Helpers ─────────────────────────────────────────────────────────────────

/// Uniform-background image with a Gaussian star at (cx, cy).
static core::FitsImage makeStar(int w, int h,
                                double cx, double cy,
                                float peak, float sigma, float sky)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.resize(static_cast<size_t>(w * h));
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const float dx = x - static_cast<float>(cx);
            const float dy = y - static_cast<float>(cy);
            img.data[static_cast<size_t>(y * w + x)] =
                sky + peak * std::exp(-(dx*dx + dy*dy) / (2.0f * sigma * sigma));
        }
    return img;
}

// ─── aperturePhotometry ───────────────────────────────────────────────────────

TEST_CASE("aperturePhotometry: rejects invalid image", "[photometry]")
{
    core::FitsImage empty;
    CHECK_FALSE(core::aperturePhotometry(empty, 10.0, 10.0, 5.0).has_value());
}

TEST_CASE("aperturePhotometry: rejects non-positive radius", "[photometry]")
{
    auto img = makeStar(60, 60, 30.0, 30.0, 3000.0f, 2.0f, 500.0f);
    CHECK_FALSE(core::aperturePhotometry(img, 30.0, 30.0,  0.0).has_value());
    CHECK_FALSE(core::aperturePhotometry(img, 30.0, 30.0, -2.0).has_value());
}

TEST_CASE("aperturePhotometry: flat background returns nullopt (no net flux)", "[photometry]")
{
    // Perfectly flat → netFlux = sumAp - bkg * nPix ≈ 0 → nullopt
    core::FitsImage img;
    img.width  = 60;
    img.height = 60;
    img.data.assign(60 * 60, 1000.0f);
    CHECK_FALSE(core::aperturePhotometry(img, 30.0, 30.0, 5.0).has_value());
}

TEST_CASE("aperturePhotometry: measures positive flux on Gaussian star", "[photometry]")
{
    // 80×80, sky=500, star peak=3000, sigma=2 at centre
    auto img = makeStar(80, 80, 40.0, 40.0, 3000.0f, 2.0f, 500.0f);
    auto res  = core::aperturePhotometry(img, 40.0, 40.0, /*rAp=*/5.0);

    REQUIRE(res.has_value());
    CHECK(res->flux  > 0.0);
    CHECK(res->nPix  > 0);
    // Sky annulus pixels should all be close to 500 ADU
    CHECK_THAT(res->bkg, WithinAbs(500.0, 5.0));
    // magInst must equal -2.5 * log10(flux) by definition
    CHECK_THAT(res->magInst, WithinAbs(-2.5 * std::log10(res->flux), 0.001));
    // Formal error should be small for a bright star
    CHECK(res->magErr > 0.0);
    CHECK(res->magErr < 0.1);
}

TEST_CASE("aperturePhotometry: centroid outside image returns nullopt", "[photometry]")
{
    auto img = makeStar(60, 60, 30.0, 30.0, 3000.0f, 2.0f, 500.0f);
    CHECK_FALSE(core::aperturePhotometry(img, -100.0, -100.0, 5.0).has_value());
    CHECK_FALSE(core::aperturePhotometry(img, 1000.0, 1000.0, 5.0).has_value());
}

TEST_CASE("aperturePhotometry: explicit annulus radii match defaults", "[photometry]")
{
    // Default inner/outer = 1.5×rAp / 2.5×rAp — explicit call should match.
    auto img         = makeStar(80, 80, 40.0, 40.0, 3000.0f, 2.0f, 500.0f);
    auto resDefault  = core::aperturePhotometry(img, 40.0, 40.0, 5.0);
    auto resExplicit = core::aperturePhotometry(img, 40.0, 40.0, 5.0, 7.5, 12.5);
    REQUIRE(resDefault.has_value());
    REQUIRE(resExplicit.has_value());
    CHECK_THAT(resDefault->flux, WithinRel(resExplicit->flux, 0.001));
}

// ─── multiAperturePhotometry ──────────────────────────────────────────────────

TEST_CASE("multiAperturePhotometry: empty radii returns empty", "[photometry]")
{
    auto img = makeStar(60, 60, 30.0, 30.0, 3000.0f, 2.0f, 500.0f);
    CHECK(core::multiAperturePhotometry(img, 30.0, 30.0, {}).isEmpty());
}

TEST_CASE("multiAperturePhotometry: all non-positive radii returns empty", "[photometry]")
{
    auto img = makeStar(60, 60, 30.0, 30.0, 3000.0f, 2.0f, 500.0f);
    CHECK(core::multiAperturePhotometry(img, 30.0, 30.0, {0.0, -1.0, -5.0}).isEmpty());
}

TEST_CASE("multiAperturePhotometry: invalid image returns empty", "[photometry]")
{
    core::FitsImage empty;
    CHECK(core::multiAperturePhotometry(empty, 0.0, 0.0, {3.0, 5.0}).isEmpty());
}

TEST_CASE("multiAperturePhotometry: flux monotonically increases with radius", "[photometry]")
{
    auto img = makeStar(100, 100, 50.0, 50.0, 4000.0f, 2.0f, 300.0f);
    auto pts = core::multiAperturePhotometry(img, 50.0, 50.0, {2.0, 4.0, 6.0, 8.0});

    REQUIRE(pts.size() >= 3);
    for (int i = 1; i < pts.size(); ++i) {
        INFO("radius[" << i << "] = " << pts[i].radius
             << "  flux = " << pts[i].flux << " vs " << pts[i-1].flux);
        CHECK(pts[i].flux >= pts[i-1].flux);
    }
}

TEST_CASE("multiAperturePhotometry: fluxFrac at largest radius equals 1.0", "[photometry]")
{
    auto img = makeStar(100, 100, 50.0, 50.0, 4000.0f, 2.0f, 300.0f);
    auto pts = core::multiAperturePhotometry(img, 50.0, 50.0, {2.0, 4.0, 6.0, 8.0, 10.0});
    REQUIRE_FALSE(pts.isEmpty());
    CHECK_THAT(pts.last().fluxFrac, WithinAbs(1.0, 0.001));
}

TEST_CASE("multiAperturePhotometry: magInst = -2.5 log10(flux) for each point", "[photometry]")
{
    auto img = makeStar(100, 100, 50.0, 50.0, 4000.0f, 2.0f, 300.0f);
    auto pts = core::multiAperturePhotometry(img, 50.0, 50.0, {3.0, 6.0, 9.0});
    REQUIRE_FALSE(pts.isEmpty());
    for (const auto& pt : pts)
        CHECK_THAT(pt.magInst, WithinAbs(-2.5 * std::log10(pt.flux), 0.001));
}

TEST_CASE("multiAperturePhotometry: duplicate radii are deduplicated", "[photometry]")
{
    auto img = makeStar(60, 60, 30.0, 30.0, 3000.0f, 2.0f, 500.0f);
    // All three radii are identical → after dedup only one survives
    auto pts = core::multiAperturePhotometry(img, 30.0, 30.0, {5.0, 5.0, 5.0});
    CHECK(pts.size() == 1);
}

TEST_CASE("multiAperturePhotometry: radii sorted internally", "[photometry]")
{
    auto img = makeStar(100, 100, 50.0, 50.0, 4000.0f, 2.0f, 300.0f);
    // Pass radii in reverse order — result must be sorted ascending
    auto pts = core::multiAperturePhotometry(img, 50.0, 50.0, {8.0, 4.0, 2.0, 6.0});
    REQUIRE(pts.size() == 4);
    for (int i = 1; i < pts.size(); ++i)
        CHECK(pts[i].radius > pts[i-1].radius);
}

// ─── computeZeroPoint (guard cases) ──────────────────────────────────────────

TEST_CASE("computeZeroPoint: nullopt with empty catalog", "[photometry]")
{
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.data.assign(100 * 100, 1000.0f);
    // catalogStars and detectedStars empty by default
    CHECK_FALSE(core::computeZeroPoint(img, 5.0).has_value());
}

TEST_CASE("computeZeroPoint: nullopt with empty detectedStars", "[photometry]")
{
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.data.assign(100 * 100, 1000.0f);

    core::CatalogStar cs;
    cs.ra = 10.0; cs.dec = 20.0; cs.mag = 12.0;
    img.catalogStars.push_back(cs);
    // detectedStars empty → no matches → <3 → nullopt
    CHECK_FALSE(core::computeZeroPoint(img, 5.0).has_value());
}

// ─── computeDifferentialZeroPoint (guard cases) ───────────────────────────────

TEST_CASE("computeDifferentialZeroPoint: nullopt when WCS not solved", "[photometry]")
{
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.data.assign(100 * 100, 1000.0f);
    img.wcs.solved = false;
    CHECK_FALSE(core::computeDifferentialZeroPoint(img, 50.0, 50.0, 5.0).has_value());
}

TEST_CASE("computeDifferentialZeroPoint: nullopt with empty catalog", "[photometry]")
{
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.data.assign(100 * 100, 1000.0f);
    img.wcs.solved = true;
    // No catalog stars → nullopt
    CHECK_FALSE(core::computeDifferentialZeroPoint(img, 50.0, 50.0, 5.0).has_value());
}
