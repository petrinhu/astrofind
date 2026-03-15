#include <catch2/catch_test_macros.hpp>
#include "core/FitsImage.h"

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
