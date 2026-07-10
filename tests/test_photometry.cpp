#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Photometry.h"
#include "core/FitsImage.h"
#include "core/Astronomy.h"

#include <array>
#include <cmath>
#include <limits>
#include <utility>

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

/// Flat-top disk source: every pixel with (dx²+dy²) ≤ diskR² is EXACTLY
/// `sky + delta` ADU, all other pixels are exactly `sky`. Used for
/// AUD-CORR-5 (non-circular flux/mag oracle): because the elevated region is
/// a perfect step function (not a Gaussian profile), the EXPECTED net flux
/// inside any aperture rAp ≥ diskR can be computed analytically — as an
/// independent ground truth — by counting star pixels ourselves, WITHOUT
/// calling aperturePhotometry() first and reading back its own flux.
static core::FitsImage makeFlatDisk(int w, int h, double cx, double cy,
                                    double diskR, float delta, float sky,
                                    int& outNStarPixels)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.assign(static_cast<size_t>(w * h), sky);
    const double diskR2 = diskR * diskR;
    int n = 0;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x) {
            const double dx = x - cx, dy = y - cy;
            if (dx * dx + dy * dy <= diskR2) {
                img.data[static_cast<size_t>(y * w + x)] = sky + delta;
                ++n;
            }
        }
    outNStarPixels = n;
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
    // AUD-CORR-5: `magInst == -2.5*log10(res->flux)` was removed here — it is
    // circular/vaidade (it re-derives the expected value FROM the function's
    // own output, so it can never fail even if flux itself is wrong). The
    // non-circular replacement, with an independently-computed ground-truth
    // flux, is "aperturePhotometry: flux and magInst match an independent
    // ground truth (flat-top disk)" below.
    // Formal error should be small for a bright star
    CHECK(res->magErr > 0.0);
    CHECK(res->magErr < 0.1);
}

TEST_CASE("aperturePhotometry: flux and magInst match an independent ground truth (flat-top disk)",
          "[photometry][AUD-CORR-5]")
{
    // A flat-top disk (step function, not Gaussian) lets us compute the
    // EXPECTED net flux analytically — by counting elevated pixels ourselves
    // in makeFlatDisk() — entirely independently of aperturePhotometry(). We
    // then check the function's OWN measured flux/magInst against that
    // independent number, instead of checking magInst against a formula
    // applied to the function's own flux (the AUD-CORR-5 circularity bug).
    int nStarPixels = 0;
    const double diskR = 4.0;
    const float  delta = 2000.0f;
    const float  sky   = 500.0f;
    auto img = makeFlatDisk(80, 80, 40.0, 40.0, diskR, delta, sky, nStarPixels);
    REQUIRE(nStarPixels > 0);

    const double expectedFlux   = static_cast<double>(delta) * nStarPixels;
    const double expectedMagInst = -2.5 * std::log10(expectedFlux);

    auto res = core::aperturePhotometry(img, 40.0, 40.0, /*rAp=*/diskR + 2.0);
    REQUIRE(res.has_value());

    CHECK_THAT(res->flux,    WithinRel(expectedFlux, 0.005));
    CHECK_THAT(res->magInst, WithinAbs(expectedMagInst, 0.005));
    // A sign-flipped or scale-flipped magInst formula (e.g. +2.5*log10, or
    // log10 base error) would miss this independently-derived value by much
    // more than 0.005 mag.
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

// AUD-CORR-5: the previous version of this test asserted
// `pt.magInst == -2.5*log10(pt.flux)` — circular/vaidade, since pt.flux is
// the SUT's own output being fed back into the very formula being checked.
// Replaced below with a flat-top-disk ground truth computed independently of
// multiAperturePhotometry(), at radii both inside and outside the disk.
TEST_CASE("multiAperturePhotometry: flux/magInst at each radius match an independent ground truth",
          "[photometry][AUD-CORR-5]")
{
    const double diskR = 5.0;
    const float  delta = 1500.0f;
    const float  sky   = 300.0f;
    int nStarPixels = 0;
    auto img = makeFlatDisk(120, 120, 60.0, 60.0, diskR, delta, sky, nStarPixels);
    REQUIRE(nStarPixels > 0);

    // Independent pixel count for a radius INSIDE the disk (r=3 < diskR=5):
    // every pixel in this smaller aperture is necessarily inside the disk too.
    auto countPixelsWithin = [](double r) {
        int n = 0;
        for (int y = 0; y < 120; ++y)
            for (int x = 0; x < 120; ++x) {
                const double dx = x - 60.0, dy = y - 60.0;
                if (dx * dx + dy * dy <= r * r) ++n;
            }
        return n;
    };
    const int nInner = countPixelsWithin(3.0);
    REQUIRE(nInner > 0);

    auto pts = core::multiAperturePhotometry(img, 60.0, 60.0, {3.0, 5.0, 8.0});
    REQUIRE(pts.size() == 3);

    // r=3.0 (< diskR): expected flux = delta * (all pixels within r=3, which
    // all lie inside the disk since 3 < diskR).
    const double expFlux3   = static_cast<double>(delta) * nInner;
    CHECK_THAT(pts[0].flux,    WithinRel(expFlux3, 0.01));
    CHECK_THAT(pts[0].magInst, WithinAbs(-2.5 * std::log10(expFlux3), 0.01));

    // r=8.0 (> diskR): expected flux plateaus at delta * nStarPixels (no more
    // elevated pixels exist beyond the disk edge).
    const double expFlux8 = static_cast<double>(delta) * nStarPixels;
    CHECK_THAT(pts[2].flux,    WithinRel(expFlux8, 0.01));
    CHECK_THAT(pts[2].magInst, WithinAbs(-2.5 * std::log10(expFlux8), 0.01));
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

// ─── Zero-point SIGN test (AUD-CORR-5) ───────────────────────────────────────
//
// Prior tests of computeZeroPoint()/computeDifferentialZeroPoint() only ever
// checked guard cases (nullopt). Neither tested the definition
// `zp = mag_cat - mag_inst` with values traceable to an independent ground
// truth. Here 3 identical flat-top-disk stars give an EXACT, independently
// computed instrumental magnitude; a known catalog magnitude then pins down
// the expected zero point (and, crucially, its SIGN) unambiguously. A
// `zp = mag_inst - mag_cat` flip would produce a value of the same magnitude
// but opposite sign — clearly distinguishable from the expected one.

TEST_CASE("computeZeroPoint: ZP = mag_cat - mag_inst, with correct sign, for known flux+catalog mag",
          "[photometry][AUD-CORR-5]")
{
    constexpr double diskR = 4.0;
    constexpr float  delta = 3000.0f;
    constexpr float  sky   = 400.0f;
    constexpr double kCatMag = 15.0;   // known catalog magnitude, identical for all 3 stars

    core::FitsImage img;
    img.width  = 300;
    img.height = 300;
    img.data.assign(static_cast<size_t>(img.width * img.height), sky);

    // Simple WCS: 1"/px, reference pixel at image centre — used only as
    // plumbing to give catalog stars a sky position; not part of what this
    // test is checking (that is the ZP sign/value).
    img.wcs.solved = true;
    img.wcs.crval1 = 180.0; img.wcs.crval2 = 0.0;
    img.wcs.crpix1 = 150.0; img.wcs.crpix2 = 150.0;
    img.wcs.cd1_1  = -1.0 / 3600.0; img.wcs.cd1_2 = 0.0;
    img.wcs.cd2_1  =  0.0;          img.wcs.cd2_2 = 1.0 / 3600.0;

    const std::array<std::pair<double,double>, 3> positions{{
        {80.0, 80.0}, {220.0, 80.0}, {150.0, 220.0}
    }};

    int nStarPixels = 0;
    for (const auto& [px, py] : positions) {
        const double diskR2 = diskR * diskR;
        for (int y = 0; y < img.height; ++y)
            for (int x = 0; x < img.width; ++x) {
                const double dx = x - px, dy = y - py;
                if (dx * dx + dy * dy <= diskR2)
                    img.data[static_cast<size_t>(y * img.width + x)] = sky + delta;
            }
    }
    // Count once (identical disks) for the independent expected-flux ground truth.
    for (int y = 0; y < img.height; ++y)
        for (int x = 0; x < img.width; ++x) {
            const double dx = x - positions[0].first, dy = y - positions[0].second;
            if (dx * dx + dy * dy <= diskR * diskR) ++nStarPixels;
        }

    for (const auto& [px, py] : positions) {
        double ra = 0.0, dec = 0.0;
        img.wcs.pixToSky(px, py, ra, dec);
        core::CatalogStar cs;
        cs.ra = ra; cs.dec = dec; cs.mag = kCatMag;
        img.catalogStars.push_back(cs);

        core::DetectedStar det;
        det.x = px; det.y = py;
        img.detectedStars.push_back(det);
    }

    const double rAp = diskR + 2.0;
    const double expectedFlux    = static_cast<double>(delta) * nStarPixels;
    const double expectedMagInst = -2.5 * std::log10(expectedFlux);
    const double expectedZp      = kCatMag - expectedMagInst;   // definition: zp = mag_cat - mag_inst

    double zpSigma = -1.0;
    auto zp = core::computeZeroPoint(img, rAp, &zpSigma);
    REQUIRE(zp.has_value());

    CHECK_THAT(*zp, WithinAbs(expectedZp, 0.02));
    // Explicit SIGN assertion: mag_cat(15) is much fainter (numerically
    // larger) than the bright synthetic star's mag_inst (strongly negative),
    // so the correct zp must be strongly POSITIVE. A `mag_inst - mag_cat`
    // flip would yield a strongly NEGATIVE value of similar magnitude.
    CHECK(*zp > 10.0);
    CHECK(zpSigma < 0.05);   // identical stars → near-zero scatter
}

// ─── Extinction correction SIGN test (AUD-CORR-5) ────────────────────────────
//
// The atmospheric extinction correction (`photometry/extinctionCoeff` in
// Settings, applied in MainWindow::runMeasurePipeline as
// `extCorr = -kExt * airmass`) had no unit anywhere: it lives inline in UI
// code. core::applyExtinctionCorrection() (added in this remediation,
// Astronomy.h/.cpp) is the tested reference implementation of that same
// convention: mag_corrected = mag_inst - k*X. This locks the SIGN so a flip
// (+k*X instead of -k*X) fails here first.

TEST_CASE("applyExtinctionCorrection: X=2, k=0.2 gives exactly -0.4 mag correction",
          "[photometry][AUD-CORR-5]")
{
    const double corrected = core::applyExtinctionCorrection(/*instMag=*/10.0, /*kExt=*/0.2, /*airmass=*/2.0);
    CHECK_THAT(corrected, WithinAbs(10.0 - 0.4, 1e-12));
    CHECK_THAT(corrected - 10.0, WithinAbs(-0.4, 1e-12));
    // A sign-flipped implementation (`+k*X`) would give 10.4, not 9.6 — the
    // WithinAbs(1e-12) tolerance leaves zero room for that mutation to pass.
}

TEST_CASE("applyExtinctionCorrection: disabled (k<=0) or below horizon (X<=0) is a no-op",
          "[photometry]")
{
    CHECK(core::applyExtinctionCorrection(12.5, 0.0, 1.5) == 12.5);
    CHECK(core::applyExtinctionCorrection(12.5, -0.1, 1.5) == 12.5);
    CHECK(core::applyExtinctionCorrection(12.5, 0.2, 0.0) == 12.5);
    CHECK(core::applyExtinctionCorrection(12.5, 0.2, -1.0) == 12.5);
}

TEST_CASE("applyExtinctionCorrection: non-finite inputs are a no-op (fail-safe)", "[photometry]")
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    // instMag itself NaN → function returns instMag unchanged, i.e. still NaN.
    CHECK(std::isnan(core::applyExtinctionCorrection(nan, 0.2, 1.5)));
    CHECK(core::applyExtinctionCorrection(12.5, nan, 1.5) == 12.5);
    CHECK(core::applyExtinctionCorrection(12.5, 0.2, nan) == 12.5);
}
