#include <catch2/catch_test_macros.hpp>
#include "core/ImageSession.h"

TEST_CASE("ImageSession starts empty", "[session]")
{
    core::ImageSession session;
    REQUIRE(session.isEmpty());
    REQUIRE(session.imageCount() == 0);
    REQUIRE(session.step() == core::SessionStep::Idle);
}

TEST_CASE("ImageSession limits to 20 images", "[session]")
{
    core::ImageSession session;
    core::FitsImage img;
    img.width  = 100;
    img.height = 100;
    img.data.resize(10000, 1.0f);

    for (int i = 0; i < 20; ++i)
        REQUIRE(session.addImage(img));

    REQUIRE_FALSE(session.addImage(img)); // 21st should fail
    REQUIRE(session.imageCount() == 20);
}

TEST_CASE("ImageSession clear resets step", "[session]")
{
    core::ImageSession session;
    session.setStep(core::SessionStep::DataReduced);
    REQUIRE(session.step() == core::SessionStep::DataReduced);
    session.clear();
    REQUIRE(session.step() == core::SessionStep::Idle);
    REQUIRE(session.isEmpty());
}
