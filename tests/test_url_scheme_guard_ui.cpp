// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-SEC-4 (CatalogClient half): see test_url_scheme_guard.cpp for the
// AstrometryClient/MpcSubmit coverage and the full rationale. CatalogClient's
// constructor opens a QSqlDatabase (local cache), which needs a running
// QApplication/QCoreApplication event loop to resolve its SQL driver plugin —
// registered here (astrofind_ui_tests, which provides a real QApplication in
// test_ui_main.cpp) rather than in the plain astrofind_tests binary, where
// constructing a CatalogClient SEGVs inside QSqlDatabase::open() before
// setVizierUrl() is even reached.

#include <catch2/catch_test_macros.hpp>

#include "core/CatalogClient.h"
#include "ui/SettingsDialog.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QSettings>
#include <QTimer>

TEST_CASE("CatalogClient::setVizierUrl accepts https://", "[security][url-scheme]")
{
    core::CatalogClient client(nullptr);
    client.setVizierUrl("https://vizier-mirror.example.org/tap/sync");
    CHECK(client.vizierUrl() == "https://vizier-mirror.example.org/tap/sync");
}

TEST_CASE("CatalogClient::setVizierUrl accepts http:// for localhost", "[security][url-scheme]")
{
    core::CatalogClient client(nullptr);
    client.setVizierUrl("http://localhost:9999/tap/sync");
    CHECK(client.vizierUrl() == "http://localhost:9999/tap/sync");
}

TEST_CASE("CatalogClient::setVizierUrl rejects http:// to a remote host", "[security][url-scheme]")
{
    core::CatalogClient client(nullptr);
    const QString safeDefault = client.vizierUrl();
    client.setVizierUrl("http://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync");
    CHECK(client.vizierUrl() == safeDefault);
    CHECK(client.vizierUrl().startsWith("https://"));
}

TEST_CASE("CatalogClient::setVizierUrl rejects an unknown scheme", "[security][url-scheme]")
{
    core::CatalogClient client(nullptr);
    const QString safeDefault = client.vizierUrl();
    client.setVizierUrl("ftp://tapvizier.cds.unistra.fr/tap/sync");
    CHECK(client.vizierUrl() == safeDefault);
}

// ─── AUD-SEC-13: SettingsDialog save-time guard ─────────────────────────────

namespace {
// onAccept() reports a rejected URL through QMessageBox::warning(), which runs
// a nested event loop. This timer dismisses any visible QMessageBox from inside
// that loop (so the test cannot block) and records that one was shown.
struct MessageBoxDismisser {
    QTimer timer;
    bool   shown = false;

    MessageBoxDismisser()
    {
        timer.setInterval(20);
        QObject::connect(&timer, &QTimer::timeout, [this]() {
            for (QWidget* w : QApplication::topLevelWidgets()) {
                auto* mb = qobject_cast<QMessageBox*>(w);
                if (mb && mb->isVisible()) {
                    shown = true;
                    mb->done(QMessageBox::Ok);
                    return;
                }
            }
        });
        timer.start();
    }
};

struct TempSettingsIni {
    QString   path;
    QSettings settings;

    explicit TempSettingsIni(const char* suffix)
        : path(QDir::temp().filePath(
              QStringLiteral("astrofind_ui_test_%1.ini").arg(QLatin1String(suffix))))
        , settings(path, QSettings::IniFormat)
    {
        settings.clear();
        settings.sync();
    }
    ~TempSettingsIni()
    {
        settings.clear();
        settings.sync();
        QFile::remove(path);
    }
};
} // namespace

TEST_CASE("SettingsDialog keeps the dialog open for an insecure astrometry baseUrl",
          "[security][url-scheme][settings]")
{
    TempSettingsIni ini("sec13_reject");
    ini.settings.setValue(QStringLiteral("astrometry/baseUrl"),
                          QStringLiteral("http://nova.astrometry.net"));
    ini.settings.sync();

    bool accepted = false;
    MessageBoxDismisser dismisser;
    {
        SettingsDialog dlg(ini.settings);
        QObject::connect(&dlg, &QDialog::accepted, [&]() { accepted = true; });
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onAccept", Qt::DirectConnection));
    }
    dismisser.timer.stop();

    CHECK(dismisser.shown);
    CHECK_FALSE(accepted);
    // Nothing was saved: saveToSettings() would have written timeoutSec.
    CHECK_FALSE(ini.settings.contains(QStringLiteral("astrometry/timeoutSec")));
    CHECK(ini.settings.value(QStringLiteral("astrometry/baseUrl")).toString()
          == QStringLiteral("http://nova.astrometry.net"));
}

TEST_CASE("SettingsDialog rejects an insecure MPC submission URL on save",
          "[security][url-scheme][settings]")
{
    TempSettingsIni ini("sec13_mpc");
    ini.settings.setValue(QStringLiteral("mpc/submitUrl"),
                          QStringLiteral("http://mpc-relay.example.org/report_ades"));
    ini.settings.sync();

    bool accepted = false;
    MessageBoxDismisser dismisser;
    {
        SettingsDialog dlg(ini.settings);
        QObject::connect(&dlg, &QDialog::accepted, [&]() { accepted = true; });
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onAccept", Qt::DirectConnection));
    }
    dismisser.timer.stop();

    CHECK(dismisser.shown);
    CHECK_FALSE(accepted);
    CHECK_FALSE(ini.settings.contains(QStringLiteral("astrometry/timeoutSec")));
}

TEST_CASE("SettingsDialog saves loopback http service URLs",
          "[security][url-scheme][settings]")
{
    TempSettingsIni ini("sec13_accept");
    ini.settings.setValue(QStringLiteral("astrometry/baseUrl"),
                          QStringLiteral("http://localhost:8080"));
    ini.settings.setValue(QStringLiteral("catalog/vizierServer"),
                          QStringLiteral("http://127.0.0.1:9999/tap/sync"));
    ini.settings.sync();

    bool accepted = false;
    MessageBoxDismisser dismisser;
    {
        SettingsDialog dlg(ini.settings);
        QObject::connect(&dlg, &QDialog::accepted, [&]() { accepted = true; });
        REQUIRE(QMetaObject::invokeMethod(&dlg, "onAccept", Qt::DirectConnection));
    }
    dismisser.timer.stop();

    CHECK_FALSE(dismisser.shown);
    CHECK(accepted);
    CHECK(ini.settings.value(QStringLiteral("astrometry/baseUrl")).toString()
          == QStringLiteral("http://localhost:8080"));
    CHECK(ini.settings.value(QStringLiteral("catalog/vizierServer")).toString()
          == QStringLiteral("http://127.0.0.1:9999/tap/sync"));
    CHECK(ini.settings.contains(QStringLiteral("astrometry/timeoutSec")));
}
