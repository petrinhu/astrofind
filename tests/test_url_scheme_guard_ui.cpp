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
