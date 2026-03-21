#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Calibration.h"
#include "core/FitsImage.h"

using Catch::Matchers::WithinAbs;

static core::FitsImage makeImage(int w, int h, float fill)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.assign(static_cast<size_t>(w * h), fill);
    return img;
}

// ─── applyDark ────────────────────────────────────────────────────────────────

TEST_CASE("applyDark subtracts pixel-by-pixel", "[calibration]")
{
    auto img  = makeImage(4, 4, 1000.0f);
    auto dark = makeImage(4, 4,  200.0f);

    REQUIRE(core::applyDark(img, dark));

    for (float v : img.data)
        REQUIRE_THAT(v, WithinAbs(800.0f, 0.5f));
}

TEST_CASE("applyDark with dark larger than image gives negative values", "[calibration]")
{
    // applyDark does a straight subtraction with no clamping
    auto img  = makeImage(4, 4,  100.0f);
    auto dark = makeImage(4, 4, 5000.0f);

    REQUIRE(core::applyDark(img, dark));

    for (float v : img.data)
        REQUIRE_THAT(v, WithinAbs(-4900.0f, 1.0f));
}

TEST_CASE("applyDark rejects dimension mismatch", "[calibration]")
{
    auto img  = makeImage(4, 4, 500.0f);
    auto dark = makeImage(8, 4, 100.0f);

    REQUIRE_FALSE(core::applyDark(img, dark));
}

TEST_CASE("applyDark rejects empty dark", "[calibration]")
{
    auto img = makeImage(4, 4, 500.0f);
    core::FitsImage dark;  // empty

    REQUIRE_FALSE(core::applyDark(img, dark));
}

// ─── applyFlat ────────────────────────────────────────────────────────────────

TEST_CASE("applyFlat with uniform flat leaves image unchanged", "[calibration]")
{
    // Flat = constant c → normalised to 1.0 everywhere → img / 1 = img
    auto img  = makeImage(4, 4, 800.0f);
    auto flat = makeImage(4, 4, 500.0f);  // any non-zero constant

    REQUIRE(core::applyFlat(img, flat));

    for (float v : img.data)
        REQUIRE_THAT(v, WithinAbs(800.0f, 1.0f));
}

TEST_CASE("applyFlat corrects gain variation", "[calibration]")
{
    // flat = {1,1,1,1,4,4,4,4} (4×2 image)
    // nth_element median at index 4 → 4  (upper-half median for even N)
    // norm[0] = 1/4 = 0.25  → img[0] = 1000/0.25 = 4000
    // norm[4] = 4/4 = 1.0   → img[4] = 1000/1.0  = 1000
    core::FitsImage img, flat;
    img.width  = flat.width  = 4;
    img.height = flat.height = 2;
    img.data.resize(8, 1000.0f);
    flat.data = {1.0f, 1.0f, 1.0f, 1.0f, 4.0f, 4.0f, 4.0f, 4.0f};

    REQUIRE(core::applyFlat(img, flat));

    REQUIRE_THAT(img.data[0], WithinAbs(4000.0f, 5.0f));
    REQUIRE_THAT(img.data[4], WithinAbs(1000.0f, 5.0f));
}

TEST_CASE("applyFlat rejects dimension mismatch", "[calibration]")
{
    auto img  = makeImage(4, 4, 800.0f);
    auto flat = makeImage(4, 8, 500.0f);

    REQUIRE_FALSE(core::applyFlat(img, flat));
}

TEST_CASE("applyFlat rejects all-zero flat", "[calibration]")
{
    auto img  = makeImage(4, 4, 800.0f);
    auto flat = makeImage(4, 4,   0.0f);

    REQUIRE_FALSE(core::applyFlat(img, flat));
}
