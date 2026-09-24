#include <catch2/catch_test_macros.hpp>
#include "core/FitsImage.h"

#include <cmath>

TEST_CASE("stretchPixel clamps correctly", "[fits]")
{
    REQUIRE(core::stretchPixel(0.0f,  0.0f, 1.0f) == 0);
    REQUIRE(core::stretchPixel(1.0f,  0.0f, 1.0f) == 255);
    REQUIRE(core::stretchPixel(-1.0f, 0.0f, 1.0f) == 0);
    REQUIRE(core::stretchPixel(2.0f,  0.0f, 1.0f) == 255);
    REQUIRE(core::stretchPixel(0.5f,  0.0f, 1.0f) == 127);
}

TEST_CASE("PlateSolution identity transform", "[wcs]")
{
    core::PlateSolution ps;
    ps.crval1 = 180.0;
    ps.crval2 = 45.0;
    ps.crpix1 = 512.0;
    ps.crpix2 = 512.0;
    ps.cd1_1  = -1.0 / 3600.0;
    ps.cd2_2  =  1.0 / 3600.0;
    ps.solved = true;

    double ra, dec, px, py;
    ps.pixToSky(512.0, 512.0, ra, dec);
    REQUIRE(std::abs(ra  - 180.0) < 1e-9);
    REQUIRE(std::abs(dec -  45.0) < 1e-9);

    ps.skyToPix(180.0, 45.0, px, py);
    REQUIRE(std::abs(px - 512.0) < 1e-6);
    REQUIRE(std::abs(py - 512.0) < 1e-6);
}

TEST_CASE("applyClockCorrection is idempotent and never adds DeltaT", "[fits][time]")
{
    // AUD-CORR-15: every Data Reduction used to add DeltaT (68 s) and the time
    // offset to img.jd again, and ADES reported that JD as UTC.
    core::FitsImage img;
    const double jd0 = 2460000.5;           // header UTC
    img.jd = jd0;

    core::applyClockCorrection(img, 0.0);   // default setting: no change
    REQUIRE(img.jd == jd0);

    core::applyClockCorrection(img, 2.5);   // camera clock 2.5 s slow
    const double jd1 = img.jd;
    REQUIRE(std::abs((jd1 - jd0) * 86400.0 - 2.5) < 1e-4);

    core::applyClockCorrection(img, 2.5);   // second reduction: unchanged
    REQUIRE(img.jd == jd1);

    core::applyClockCorrection(img, -1.0);  // user edits the setting
    REQUIRE(std::abs((img.jd - jd0) * 86400.0 + 1.0) < 1e-4);

    core::applyClockCorrection(img, 0.0);   // back to zero restores header time
    REQUIRE(std::abs((img.jd - jd0) * 86400.0) < 1e-4);

    core::FitsImage noTime;                 // images without DATE-OBS stay at 0
    core::applyClockCorrection(noTime, 5.0);
    REQUIRE(noTime.jd == 0.0);
}
