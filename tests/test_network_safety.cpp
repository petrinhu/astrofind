// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-SEC-13: core::isSafeServiceUrl() is the single URL guard shared by the
// client setters (AUD-SEC-4) and the SettingsDialog save-time check.
// AUD-SEC-12: core::isValidHorizonsTarget() allowlists the text interpolated
// into the Horizons COMMAND='...' parameter; HorizonsClient::query() refuses
// anything else before touching the network.
// AUD-SEC-10: KooEngine::cancel() is safe with no request in flight.
//
// No network I/O: every client is built with nam=nullptr, and the paths under
// test return before the QNetworkAccessManager pointer is used.

#include <catch2/catch_test_macros.hpp>

#include "core/NetworkSafety.h"
#include "core/HorizonsClient.h"
#include "core/KooEngine.h"

#include <QString>
#include <QUrl>

// ─── core::isSafeServiceUrl ───────────────────────────────────────────────────

TEST_CASE("isSafeServiceUrl accepts https to any host", "[security][url-scheme]")
{
    CHECK(core::isSafeServiceUrl(QStringLiteral("https://nova.astrometry.net")));
    CHECK(core::isSafeServiceUrl(QStringLiteral("https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync")));
    CHECK(core::isSafeServiceUrl(QStringLiteral("HTTPS://www.minorplanetcenter.net/report_ades")));
    CHECK(core::isSafeServiceUrl(QUrl(QStringLiteral("https://example.org:8443/api"))));
}

TEST_CASE("isSafeServiceUrl accepts http only on loopback", "[security][url-scheme]")
{
    CHECK(core::isSafeServiceUrl(QStringLiteral("http://localhost:8080")));
    CHECK(core::isSafeServiceUrl(QStringLiteral("http://LOCALHOST/api")));
    CHECK(core::isSafeServiceUrl(QStringLiteral("http://127.0.0.1:8080/api")));
    CHECK(core::isSafeServiceUrl(QStringLiteral("http://[::1]:8080/api")));
}

TEST_CASE("isSafeServiceUrl rejects http to a remote host", "[security][url-scheme]")
{
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("http://nova.astrometry.net")));
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("http://192.168.1.10:8080")));
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("http://localhost.evil.example.org")));
}

TEST_CASE("isSafeServiceUrl rejects other schemes and bare hosts", "[security][url-scheme]")
{
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("ftp://nova.astrometry.net")));
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("file:///etc/passwd")));
    // The old SettingsDialog VizieR default: no scheme at all.
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("vizier.cfa.harvard.edu")));
    CHECK_FALSE(core::isSafeServiceUrl(QString()));
    CHECK_FALSE(core::isSafeServiceUrl(QStringLiteral("   ")));
}

TEST_CASE("isSafeServiceUrl QString overload trims surrounding whitespace", "[security][url-scheme]")
{
    CHECK(core::isSafeServiceUrl(QStringLiteral("  https://nova.astrometry.net  ")));
}

// ─── core::isValidHorizonsTarget ──────────────────────────────────────────────

TEST_CASE("isValidHorizonsTarget accepts real designations", "[security][horizons]")
{
    CHECK(core::isValidHorizonsTarget(QStringLiteral("433")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("Ceres")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("Eros")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("2024 AB1")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("2023 DW")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("C/2023 A3")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("C/2020 F3")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("1P/Halley")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("P/2010 A2 (LINEAR)")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("67P/Churyumov-Gerasimenko")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("2004 MN4")));
    CHECK(core::isValidHorizonsTarget(QStringLiteral("K24A01B")));
}

TEST_CASE("isValidHorizonsTarget rejects quotes and command separators", "[security][horizons]")
{
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433'")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433' OBJ_DATA='YES")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("\"433\"")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433;")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("DES=2000433;")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433&MAKE_EPHEM=NO")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433+1")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433#frag")));
}

TEST_CASE("isValidHorizonsTarget rejects control chars and non-ASCII", "[security][horizons]")
{
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433\n")));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("433\tEros")));
    CHECK_FALSE(core::isValidHorizonsTarget(QString(QChar(0)) + QStringLiteral("433")));
    CHECK_FALSE(core::isValidHorizonsTarget(QString::fromUtf8("\xC3\x89ros")));   // "Éros"
}

TEST_CASE("isValidHorizonsTarget rejects empty and oversized input", "[security][horizons]")
{
    CHECK_FALSE(core::isValidHorizonsTarget(QString()));
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("   ")));      // no letter/digit
    CHECK_FALSE(core::isValidHorizonsTarget(QStringLiteral("()/-._")));
    CHECK(core::isValidHorizonsTarget(QString(64, QLatin1Char('A'))));
    CHECK_FALSE(core::isValidHorizonsTarget(QString(65, QLatin1Char('A'))));
}

TEST_CASE("HorizonsClient::query refuses an invalid target without a request", "[security][horizons]")
{
    core::HorizonsClient client(nullptr);   // nam=nullptr: must never be used here
    QString reason;
    int failures = 0;
    QObject::connect(&client, &core::HorizonsClient::failed,
                     [&](const QString& r) { reason = r; ++failures; });

    client.query(QStringLiteral("433' OBJ_DATA='YES"), 2460000.5);

    CHECK(failures == 1);
    CHECK_FALSE(reason.isEmpty());
    CHECK_FALSE(client.isBusy());
}

// ─── KooEngine::cancel ────────────────────────────────────────────────────────

TEST_CASE("KooEngine::cancel with no request in flight is a no-op", "[security][koo]")
{
    core::KooEngine engine(nullptr);
    int failures = 0;
    QObject::connect(&engine, &core::KooEngine::failed,
                     [&](const QString&) { ++failures; });
    engine.cancel();
    CHECK_FALSE(engine.isBusy());
    CHECK(failures == 0);
}
