#include <catch2/catch_test_macros.hpp>

#include "core/ImageSession.h"

static core::FitsImage makeImg(double ra = 0.0, double dec = 0.0)
{
    core::FitsImage img;
    img.width  = 10;
    img.height = 10;
    img.data.assign(100, 1.0f);
    img.ra  = ra;
    img.dec = dec;
    return img;
}

// ─── stepChanged signal ───────────────────────────────────────────────────────

TEST_CASE("ImageSession: stepChanged emitted on new step value", "[ui][session]")
{
    core::ImageSession session;
    core::SessionStep captured = core::SessionStep::Idle;
    QObject::connect(&session, &core::ImageSession::stepChanged,
                     [&](core::SessionStep s){ captured = s; });

    session.setStep(core::SessionStep::ImagesLoaded);
    CHECK(captured == core::SessionStep::ImagesLoaded);

    session.setStep(core::SessionStep::DataReduced);
    CHECK(captured == core::SessionStep::DataReduced);
}

TEST_CASE("ImageSession: stepChanged NOT emitted when step unchanged", "[ui][session]")
{
    core::ImageSession session;
    int count = 0;
    QObject::connect(&session, &core::ImageSession::stepChanged,
                     [&](core::SessionStep){ ++count; });

    session.setStep(core::SessionStep::Idle);   // same as initial
    CHECK(count == 0);

    session.setStep(core::SessionStep::ImagesLoaded);
    CHECK(count == 1);
    session.setStep(core::SessionStep::ImagesLoaded);  // same again
    CHECK(count == 1);
}

// ─── imagesChanged signal ─────────────────────────────────────────────────────

TEST_CASE("ImageSession: imagesChanged emitted on addImage", "[ui][session]")
{
    core::ImageSession session;
    int count = 0;
    QObject::connect(&session, &core::ImageSession::imagesChanged, [&]{ ++count; });

    session.addImage(makeImg());
    CHECK(count == 1);

    session.addImage(makeImg());
    CHECK(count == 2);
}

TEST_CASE("ImageSession: imagesChanged emitted on clear", "[ui][session]")
{
    core::ImageSession session;
    session.addImage(makeImg());

    bool fired = false;
    QObject::connect(&session, &core::ImageSession::imagesChanged, [&]{ fired = true; });

    session.clear();
    CHECK(fired);
}

TEST_CASE("ImageSession: clear resets step to Idle and emits stepChanged", "[ui][session]")
{
    core::ImageSession session;
    session.addImage(makeImg());
    session.setStep(core::SessionStep::DataReduced);

    core::SessionStep captured = core::SessionStep::DataReduced;
    QObject::connect(&session, &core::ImageSession::stepChanged,
                     [&](core::SessionStep s){ captured = s; });

    session.clear();

    CHECK(session.step() == core::SessionStep::Idle);
    CHECK(captured == core::SessionStep::Idle);
}

// ─── setMaxImages dynamic enforcement ────────────────────────────────────────

TEST_CASE("ImageSession: setMaxImages(3) limits session to 3 images", "[ui][session]")
{
    core::ImageSession session;
    session.setMaxImages(3);
    CHECK(session.maxImages() == 3);

    REQUIRE(session.addImage(makeImg()));   // 1
    REQUIRE(session.addImage(makeImg()));   // 2
    REQUIRE(session.addImage(makeImg()));   // 3
    CHECK_FALSE(session.addImage(makeImg())); // 4 → rejected
    CHECK(session.imageCount() == 3);
}

TEST_CASE("ImageSession: raising maxImages allows more images without reload", "[ui][session]")
{
    core::ImageSession session;
    session.setMaxImages(2);
    REQUIRE(session.addImage(makeImg()));
    REQUIRE(session.addImage(makeImg()));
    CHECK_FALSE(session.addImage(makeImg())); // limit=2, rejected

    session.setMaxImages(4);
    REQUIRE(session.addImage(makeImg())); // now ok
    REQUIRE(session.addImage(makeImg())); // 4th ok
    CHECK_FALSE(session.addImage(makeImg())); // 5th rejected
    CHECK(session.imageCount() == 4);
}

TEST_CASE("ImageSession: setMaxImages clamps to minimum 1", "[ui][session]")
{
    core::ImageSession session;
    session.setMaxImages(-5);
    CHECK(session.maxImages() == 1);
    session.setMaxImages(0);
    CHECK(session.maxImages() == 1);

    REQUIRE(session.addImage(makeImg()));
    CHECK_FALSE(session.addImage(makeImg())); // 2nd rejected at maxImages=1
}

// ─── isCohesive ───────────────────────────────────────────────────────────────

TEST_CASE("ImageSession: isCohesive is true for empty session", "[ui][session]")
{
    core::ImageSession session;
    CHECK(session.isCohesive());
}

TEST_CASE("ImageSession: isCohesive is true for single image", "[ui][session]")
{
    core::ImageSession session;
    session.addImage(makeImg(100.0, -10.0));
    CHECK(session.isCohesive());
}

TEST_CASE("ImageSession: isCohesive is true when images are within 1° tolerance", "[ui][session]")
{
    core::ImageSession session;
    session.addImage(makeImg(100.000, -10.000));
    session.addImage(makeImg(100.500, -10.500));  // 0.5° apart — within 1° tolerance
    CHECK(session.isCohesive());
}

TEST_CASE("ImageSession: isCohesive is false when images differ by more than 1°", "[ui][session]")
{
    core::ImageSession session;
    session.addImage(makeImg(  0.0,  0.0));
    session.addImage(makeImg( 90.0, 45.0));   // completely different field
    CHECK_FALSE(session.isCohesive());
}
