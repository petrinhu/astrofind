// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-SEC-4: astrometry/baseUrl, catalog/vizierServer and mpc/submitUrl are
// user-configurable endpoints that carry secrets in the clear if downgraded
// to plain HTTP — the astrometry.net API key (AstrometryClient), and the
// full ADES/PSV observation report (MpcSubmit). This validates the setter
// enforcement added to AstrometryClient::setBaseUrl, CatalogClient::
// setVizierUrl and MpcSubmit::setEndpoint: https:// always accepted;
// http:// accepted ONLY for localhost/127.0.0.1/::1; anything else rejected
// (previous value kept, not silently downgraded).
//
// No actual network I/O happens in these tests — nam=nullptr is safe because
// the setters under test never touch the QNetworkAccessManager pointer.

#include <catch2/catch_test_macros.hpp>

#include "core/AstrometryClient.h"
#include "core/CatalogClient.h"
#include "core/MpcSubmit.h"

// ─── AstrometryClient::setBaseUrl ─────────────────────────────────────────────

TEST_CASE("AstrometryClient::setBaseUrl accepts https://", "[security][url-scheme]")
{
    core::AstrometryClient client(nullptr);
    const QString original = client.baseUrl();
    client.setBaseUrl("https://my-astrometry-mirror.example.org");
    CHECK(client.baseUrl() == "https://my-astrometry-mirror.example.org");
    CHECK(client.baseUrl() != original);
}

TEST_CASE("AstrometryClient::setBaseUrl accepts http:// for localhost (self-hosted instance)",
          "[security][url-scheme]")
{
    core::AstrometryClient client(nullptr);
    client.setBaseUrl("http://localhost:8080");
    CHECK(client.baseUrl() == "http://localhost:8080");

    client.setBaseUrl("http://127.0.0.1:8080");
    CHECK(client.baseUrl() == "http://127.0.0.1:8080");
}

TEST_CASE("AstrometryClient::setBaseUrl rejects http:// to a remote host (API key leak risk)",
          "[security][url-scheme]")
{
    core::AstrometryClient client(nullptr);
    const QString safeDefault = client.baseUrl();

    client.setBaseUrl("http://nova.astrometry.net");
    // Rejected: the (safe) previous value must be unchanged, not downgraded.
    CHECK(client.baseUrl() == safeDefault);
    CHECK(client.baseUrl().startsWith("https://"));
}

TEST_CASE("AstrometryClient::setBaseUrl rejects an unknown scheme", "[security][url-scheme]")
{
    core::AstrometryClient client(nullptr);
    const QString safeDefault = client.baseUrl();
    client.setBaseUrl("ftp://nova.astrometry.net");
    CHECK(client.baseUrl() == safeDefault);
}

// CatalogClient::setVizierUrl is exercised in test_url_scheme_guard_ui.cpp
// (astrofind_ui_tests) instead of here: CatalogClient's constructor opens a
// QSqlDatabase (cache), which needs a running QCoreApplication/QApplication
// event loop to resolve its SQL driver plugin — the plain Catch2WithMain
// binary (astrofind_tests) has none, and instantiating CatalogClient here
// SEGVs inside QSqlDatabase::open() before setVizierUrl() is even reached.
// This is a pre-existing gap in the test infra (nothing previously
// constructed a CatalogClient in this binary), unrelated to the URL-scheme
// guard itself.

// ─── MpcSubmit::setEndpoint ────────────────────────────────────────────────────

TEST_CASE("MpcSubmit::setEndpoint accepts https://", "[security][url-scheme]")
{
    core::MpcSubmit submit;
    submit.setEndpoint("https://mpc-mirror.example.org/report_ades");
    CHECK(submit.endpoint() == "https://mpc-mirror.example.org/report_ades");
}

TEST_CASE("MpcSubmit::setEndpoint accepts http:// for localhost (local relay)",
          "[security][url-scheme]")
{
    core::MpcSubmit submit;
    submit.setEndpoint("http://127.0.0.1:5000/report_ades");
    CHECK(submit.endpoint() == "http://127.0.0.1:5000/report_ades");
}

TEST_CASE("MpcSubmit::setEndpoint rejects http:// to a remote host (ADES report leak risk)",
          "[security][url-scheme]")
{
    core::MpcSubmit submit;
    const QString safeDefault = submit.endpoint();
    submit.setEndpoint("http://www.minorplanetcenter.net/report_ades");
    CHECK(submit.endpoint() == safeDefault);
    CHECK(submit.endpoint().startsWith("https://"));
}

TEST_CASE("MpcSubmit::setEndpoint rejects an unknown scheme", "[security][url-scheme]")
{
    core::MpcSubmit submit;
    const QString safeDefault = submit.endpoint();
    submit.setEndpoint("ws://www.minorplanetcenter.net/report_ades");
    CHECK(submit.endpoint() == safeDefault);
}
