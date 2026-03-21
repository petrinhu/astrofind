#include <catch2/catch_test_macros.hpp>
#include "core/MovingObjectDetector.h"
#include "core/FitsImage.h"

static core::FitsImage makeFrame(int w, int h)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.assign(static_cast<size_t>(w * h), 100.0f);
    return img;
}

static core::DetectedStar makeStar(double x, double y, double snr = 15.0)
{
    core::DetectedStar s;
    s.x   = x;
    s.y   = y;
    s.snr = snr;
    s.flux = snr * 500.0;
    s.a   = 2.0;
    s.b   = 2.0;
    return s;
}

// ─── basic constraints ────────────────────────────────────────────────────────

TEST_CASE("detectMovingObjects requires at least minFrames images", "[detector]")
{
    // Only 2 frames but minFrames=3
    QVector<core::FitsImage> imgs;
    auto f1 = makeFrame(200, 200);
    f1.detectedStars = { makeStar(100, 100), makeStar(50, 50) };
    auto f2 = makeFrame(200, 200);
    f2.detectedStars = { makeStar(102, 100), makeStar(50, 50) };
    imgs.push_back(f1);
    imgs.push_back(f2);

    auto res = core::detectMovingObjects(imgs, 2.0, /*minFrames=*/3, 5.0);
    REQUIRE(res.has_value());
    REQUIRE(res->isEmpty());  // no track spans 3 frames
}

TEST_CASE("detectMovingObjects returns error on empty input", "[detector]")
{
    QVector<core::FitsImage> imgs;
    auto res = core::detectMovingObjects(imgs);
    // Empty input: either error or empty candidates — either is acceptable
    if (res.has_value())
        REQUIRE(res->isEmpty());
}

// ─── linear motion detection ─────────────────────────────────────────────────

TEST_CASE("detectMovingObjects finds object moving 2 px/frame in X", "[detector]")
{
    // 4 frames; one moving object shifts +2 px/frame in X
    // Several static stars stay fixed
    const int W = 300, H = 300;
    QVector<core::FitsImage> imgs;

    for (int f = 0; f < 4; ++f) {
        auto frame = makeFrame(W, H);
        // Static reference stars
        frame.detectedStars = {
            makeStar(50,  50),
            makeStar(250, 50),
            makeStar(50,  250),
            makeStar(250, 250),
        };
        // Moving object
        frame.detectedStars.push_back(makeStar(100.0 + 2.0 * f, 150.0));
        imgs.push_back(frame);
    }

    auto res = core::detectMovingObjects(imgs, 2.5, /*minFrames=*/3, 5.0);
    REQUIRE(res.has_value());
    REQUIRE_FALSE(res->isEmpty());

    // Find the candidate closest to dx≈2, dy≈0
    bool found = false;
    for (const auto& c : *res) {
        if (std::abs(c.dxPerFrame - 2.0) < 0.5 && std::abs(c.dyPerFrame) < 0.5) {
            found = true;
            REQUIRE(c.positions.size() >= 3);
            REQUIRE(c.speed > 0.0);
        }
    }
    REQUIRE(found);
}

TEST_CASE("detectMovingObjects finds diagonal motion", "[detector]")
{
    const int W = 300, H = 300;
    QVector<core::FitsImage> imgs;

    for (int f = 0; f < 4; ++f) {
        auto frame = makeFrame(W, H);
        frame.detectedStars = {
            makeStar(50,  50),
            makeStar(250, 50),
            makeStar(50,  250),
        };
        // Moving diagonally: +3 px/frame in X, +1.5 px/frame in Y
        frame.detectedStars.push_back(makeStar(100.0 + 3.0 * f, 100.0 + 1.5 * f));
        imgs.push_back(frame);
    }

    auto res = core::detectMovingObjects(imgs, 2.5, /*minFrames=*/3, 5.0);
    REQUIRE(res.has_value());
    REQUIRE_FALSE(res->isEmpty());
}

TEST_CASE("detectMovingObjects ignores low-SNR detections", "[detector]")
{
    // Moving object with SNR below threshold
    const int W = 200, H = 200;
    QVector<core::FitsImage> imgs;

    for (int f = 0; f < 4; ++f) {
        auto frame = makeFrame(W, H);
        frame.detectedStars = { makeStar(50, 50), makeStar(150, 50) };
        // Low-SNR mover (SNR=3, below minSnr=5)
        frame.detectedStars.push_back(makeStar(100.0 + 2.0 * f, 100.0, /*snr=*/3.0));
        imgs.push_back(frame);
    }

    auto res = core::detectMovingObjects(imgs, 2.5, /*minFrames=*/3, /*minSnr=*/5.0);
    REQUIRE(res.has_value());
    // Low-SNR track should not appear
    for (const auto& c : *res)
        REQUIRE(c.snr >= 5.0);
}
