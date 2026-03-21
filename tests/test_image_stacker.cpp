#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/ImageStacker.h"
#include "core/FitsImage.h"

#include <QPointF>

using Catch::Matchers::WithinAbs;

static core::FitsImage makeFlat(int w, int h, float fill)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.assign(static_cast<size_t>(w * h), fill);
    return img;
}

/// Build an image with a bright spot at (cx, cy) on a flat background.
static core::FitsImage makeSpot(int w, int h, int cx, int cy,
                                 float spotVal, float bkg = 100.0f)
{
    auto img = makeFlat(w, h, bkg);
    // 3×3 bright patch
    for (int dy = -1; dy <= 1; ++dy)
        for (int dx = -1; dx <= 1; ++dx) {
            int x = cx + dx, y = cy + dy;
            if (x >= 0 && x < w && y >= 0 && y < h)
                img.data[static_cast<size_t>(y) * w + x] = spotVal;
        }
    return img;
}

// ─── stackImages — Add ────────────────────────────────────────────────────────

TEST_CASE("stackImages Add: two identical images doubles pixel values", "[stacker]")
{
    const float V = 500.0f;
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, V));
    imgs.push_back(makeFlat(32, 32, V));

    auto res = core::stackImages(imgs, core::StackMode::Add);
    REQUIRE(res.has_value());

    for (float v : res->image.data)
        REQUIRE_THAT(v, WithinAbs(2.0f * V, 1.0f));
}

// ─── stackImages — Average ────────────────────────────────────────────────────

TEST_CASE("stackImages Average: two identical images returns same values", "[stacker]")
{
    const float V = 700.0f;
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, V));
    imgs.push_back(makeFlat(32, 32, V));

    auto res = core::stackImages(imgs, core::StackMode::Average);
    REQUIRE(res.has_value());

    for (float v : res->image.data)
        REQUIRE_THAT(v, WithinAbs(V, 1.0f));
}

TEST_CASE("stackImages Average: averages correctly", "[stacker]")
{
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, 200.0f));
    imgs.push_back(makeFlat(32, 32, 400.0f));

    auto res = core::stackImages(imgs, core::StackMode::Average);
    REQUIRE(res.has_value());

    for (float v : res->image.data)
        REQUIRE_THAT(v, WithinAbs(300.0f, 1.0f));
}

// ─── stackImages — Median ────────────────────────────────────────────────────

TEST_CASE("stackImages Median: suppresses single outlier", "[stacker]")
{
    // 3 images: two have value 300, one has a spike at pixel 0
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, 300.0f));
    imgs.push_back(makeFlat(32, 32, 300.0f));
    auto spike = makeFlat(32, 32, 300.0f);
    spike.data[0] = 9999.0f;
    imgs.push_back(spike);

    auto res = core::stackImages(imgs, core::StackMode::Median);
    REQUIRE(res.has_value());

    // Median of (300, 300, 9999) = 300
    REQUIRE_THAT(res->image.data[0], WithinAbs(300.0f, 1.0f));
}

// ─── stackImages — shifts ─────────────────────────────────────────────────────

TEST_CASE("stackImages returns one shift per input image", "[stacker]")
{
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeSpot(64, 64, 32, 32, 8000.0f));
    imgs.push_back(makeSpot(64, 64, 32, 32, 8000.0f));
    imgs.push_back(makeSpot(64, 64, 32, 32, 8000.0f));

    auto res = core::stackImages(imgs, core::StackMode::Average);
    REQUIRE(res.has_value());
    REQUIRE(res->shifts.size() == imgs.size());
}

TEST_CASE("stackImages single image returns itself with zero shift", "[stacker]")
{
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, 500.0f));

    auto res = core::stackImages(imgs, core::StackMode::Average);
    REQUIRE(res.has_value());
    REQUIRE(res->shifts.size() == 1);
    REQUIRE_THAT(res->shifts[0].x(), WithinAbs(0.0, 0.5));
    REQUIRE_THAT(res->shifts[0].y(), WithinAbs(0.0, 0.5));
}

// ─── trackAndStack ────────────────────────────────────────────────────────────

TEST_CASE("trackAndStack with zero motion is equivalent to regular stack", "[stacker]")
{
    const float V = 400.0f;
    QVector<core::FitsImage> imgs;
    imgs.push_back(makeFlat(32, 32, V));
    imgs.push_back(makeFlat(32, 32, V));

    auto res = core::trackAndStack(imgs, 0.0, 0.0, core::StackMode::Average);
    REQUIRE(res.has_value());

    for (float v : res->image.data)
        REQUIRE_THAT(v, WithinAbs(V, 1.0f));
}

TEST_CASE("trackAndStack rejects empty input", "[stacker]")
{
    QVector<core::FitsImage> imgs;
    auto res = core::trackAndStack(imgs, 1.0, 0.0);
    REQUIRE_FALSE(res.has_value());
}
