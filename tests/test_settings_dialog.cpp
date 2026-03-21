#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "ui/SettingsDialog.h"

#include <QDir>
#include <QFile>
#include <QSettings>

using Catch::Matchers::WithinAbs;

// Each test uses a private INI file in /tmp so it never pollutes the real
// application settings.  The file is removed in the fixture destructor.
struct TempIni {
    QString path;
    QSettings settings;

    TempIni(const char* suffix)
        : path(QDir::temp().filePath(
              QStringLiteral("astrofind_ui_test_%1.ini").arg(suffix)))
        , settings(path, QSettings::IniFormat)
    {
        settings.clear();
        settings.sync();
    }

    ~TempIni()
    {
        settings.clear();
        settings.sync();
        QFile::remove(path);
    }
};

// ─── Round-trip ───────────────────────────────────────────────────────────────

TEST_CASE("SettingsDialog: round-trip preserves loaded values", "[ui][settings]")
{
    TempIni ini("roundtrip");
    ini.settings.setValue(QStringLiteral("observer/name"),          QStringLiteral("Alice Astronomer"));
    ini.settings.setValue(QStringLiteral("camera/pixelScaleX"),     0.000412);  // typical degrees/px
    ini.settings.setValue(QStringLiteral("detection/minSnr"),       7.5);
    ini.settings.setValue(QStringLiteral("astrometry/timeoutSec"),  120);
    ini.settings.sync();

    {
        // Constructor calls loadFromSettings() automatically.
        // onAccept() is a private slot accessible via invokeMethod (Qt MOC).
        SettingsDialog dlg(ini.settings);
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onAccept", Qt::DirectConnection));
    }

    CHECK(ini.settings.value("observer/name").toString() == "Alice Astronomer");
    CHECK_THAT(ini.settings.value("camera/pixelScaleX").toDouble(),    WithinAbs(0.000412, 1e-6));
    CHECK_THAT(ini.settings.value("detection/minSnr").toDouble(),      WithinAbs(7.5,  0.001));
    CHECK(ini.settings.value("astrometry/timeoutSec").toInt() == 120);
}

// ─── Reset to defaults ────────────────────────────────────────────────────────

TEST_CASE("SettingsDialog: reset to defaults writes canonical default values", "[ui][settings]")
{
    TempIni ini("defaults");
    // Pre-set values that differ from the documented defaults
    ini.settings.setValue(QStringLiteral("detection/minSnr"),          99.0);
    ini.settings.setValue(QStringLiteral("camera/pixelScaleX"),         5.5);
    ini.settings.setValue(QStringLiteral("display/blinkIntervalMs"),  9999);
    ini.settings.setValue(QStringLiteral("astrometry/timeoutSec"),     999);
    ini.settings.sync();

    {
        SettingsDialog dlg(ini.settings);
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onResetDefaults", Qt::DirectConnection));
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onAccept",        Qt::DirectConnection));
    }

    // After reset + accept, widgets should hold the documented defaults and
    // saveToSettings() should have persisted them.
    CHECK_THAT(ini.settings.value("detection/minSnr").toDouble(),      WithinAbs(5.0,  0.001));
    CHECK_THAT(ini.settings.value("camera/pixelScaleX").toDouble(),    WithinAbs(0.0,  0.001));
    CHECK(ini.settings.value("display/blinkIntervalMs").toInt() == 500);
    CHECK(ini.settings.value("astrometry/timeoutSec").toInt()   == 300);
}
