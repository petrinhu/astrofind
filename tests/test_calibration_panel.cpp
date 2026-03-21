#include <catch2/catch_test_macros.hpp>

#include "ui/CalibrationPanel.h"

#include <QPushButton>

// Find the Nth QPushButton (0-based) whose text equals `text` among all
// descendants of `panel`.
static QPushButton* findButtonN(CalibrationPanel& panel, const QString& text, int n)
{
    int idx = 0;
    for (auto* b : panel.findChildren<QPushButton*>())
        if (b->text() == text && idx++ == n)
            return b;
    return nullptr;
}

// Find the first button whose text starts with `prefix`.
static QPushButton* findButtonPrefix(CalibrationPanel& panel, const QString& prefix)
{
    for (auto* b : panel.findChildren<QPushButton*>())
        if (b->text().startsWith(prefix))
            return b;
    return nullptr;
}

// ─── Initial state ────────────────────────────────────────────────────────────

TEST_CASE("CalibrationPanel: both Clear buttons start disabled", "[ui][calibration]")
{
    CalibrationPanel panel;

    auto* clearDark = findButtonN(panel, "Clear", 0);
    auto* clearFlat = findButtonN(panel, "Clear", 1);

    REQUIRE(clearDark != nullptr);
    REQUIRE(clearFlat != nullptr);
    CHECK_FALSE(clearDark->isEnabled());
    CHECK_FALSE(clearFlat->isEnabled());
}

// ─── Dark frame ───────────────────────────────────────────────────────────────

TEST_CASE("CalibrationPanel: setDarkStatus enables clearDark, leaves clearFlat off", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setDarkStatus("master_dark.fits", 2048, 2048);

    auto* clearDark = findButtonN(panel, "Clear", 0);
    auto* clearFlat = findButtonN(panel, "Clear", 1);
    REQUIRE(clearDark != nullptr);
    REQUIRE(clearFlat != nullptr);

    CHECK(clearDark->isEnabled());
    CHECK_FALSE(clearFlat->isEnabled());
}

TEST_CASE("CalibrationPanel: clearDark disables clearDark button", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setDarkStatus("dark.fits", 512, 512);
    panel.clearDark();

    auto* clearDark = findButtonN(panel, "Clear", 0);
    REQUIRE(clearDark != nullptr);
    CHECK_FALSE(clearDark->isEnabled());
}

// ─── Flat field ───────────────────────────────────────────────────────────────

TEST_CASE("CalibrationPanel: setFlatStatus enables clearFlat, leaves clearDark off", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setFlatStatus("flat.fits", 1024, 1024);

    auto* clearDark = findButtonN(panel, "Clear", 0);
    auto* clearFlat = findButtonN(panel, "Clear", 1);
    REQUIRE(clearDark != nullptr);
    REQUIRE(clearFlat != nullptr);

    CHECK_FALSE(clearDark->isEnabled());
    CHECK(clearFlat->isEnabled());
}

TEST_CASE("CalibrationPanel: clearFlat disables clearFlat button", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setFlatStatus("flat.fits", 1024, 1024);
    panel.clearFlat();

    auto* clearFlat = findButtonN(panel, "Clear", 1);
    REQUIRE(clearFlat != nullptr);
    CHECK_FALSE(clearFlat->isEnabled());
}

// ─── Independent dark and flat ────────────────────────────────────────────────

TEST_CASE("CalibrationPanel: dark and flat can be loaded independently", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setDarkStatus("dark.fits",  200, 200);
    panel.setFlatStatus("flat.fits",  200, 200);

    auto* clearDark = findButtonN(panel, "Clear", 0);
    auto* clearFlat = findButtonN(panel, "Clear", 1);
    REQUIRE(clearDark != nullptr);
    REQUIRE(clearFlat != nullptr);

    CHECK(clearDark->isEnabled());
    CHECK(clearFlat->isEnabled());

    // Clear dark — flat should remain enabled
    panel.clearDark();
    CHECK_FALSE(clearDark->isEnabled());
    CHECK(clearFlat->isEnabled());
}

// ─── Signal emission ──────────────────────────────────────────────────────────

TEST_CASE("CalibrationPanel: Load button emits loadDarkRequested", "[ui][calibration]")
{
    CalibrationPanel panel;
    bool fired = false;
    QObject::connect(&panel, &CalibrationPanel::loadDarkRequested, [&]{ fired = true; });

    // "Load…" buttons: first one is for dark
    auto* loadBtn = findButtonPrefix(panel, "Load");
    REQUIRE(loadBtn != nullptr);
    loadBtn->click();

    CHECK(fired);
}

TEST_CASE("CalibrationPanel: Clear button emits clearDarkRequested", "[ui][calibration]")
{
    CalibrationPanel panel;
    panel.setDarkStatus("dark.fits", 200, 200);   // enables clearDark

    bool fired = false;
    QObject::connect(&panel, &CalibrationPanel::clearDarkRequested, [&]{ fired = true; });

    auto* clearDark = findButtonN(panel, "Clear", 0);
    REQUIRE(clearDark != nullptr);
    REQUIRE(clearDark->isEnabled());
    clearDark->click();

    CHECK(fired);
}

TEST_CASE("CalibrationPanel: Apply All button emits applyAllRequested", "[ui][calibration]")
{
    CalibrationPanel panel;
    bool fired = false;
    QObject::connect(&panel, &CalibrationPanel::applyAllRequested, [&]{ fired = true; });

    auto* applyBtn = findButtonPrefix(panel, "Apply");
    REQUIRE(applyBtn != nullptr);
    applyBtn->click();

    CHECK(fired);
}
