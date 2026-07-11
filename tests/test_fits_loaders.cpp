// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-3: loadFitsCube, loadFitsHdu, scanImageHdus (FitsImage.cpp) and
// FitsTableReader (readFitsTable/importDaophotTable/readLocalCatalogTable)
// had ZERO test coverage — exactly the input paths hardened against hostile
// FITS in the AUD-INPUT-* remediation. All fixtures here are synthesized on
// disk via cfitsio (tests/synthetic_fits.h); nothing depends on external
// files.

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/FitsImage.h"
#include "core/FitsTableReader.h"
#include "synthetic_fits.h"

#include <QTemporaryDir>
#include <QDir>

using namespace testutil;

// ─── loadFitsCube ─────────────────────────────────────────────────────────────

TEST_CASE("loadFitsCube: reads all N planes of a temporal cube (NAXIS3=5)", "[loaders][cube]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("cube5.fits");

    const int W = 24, H = 16, D = 5;
    std::vector<std::vector<float>> planes;
    for (int k = 0; k < D; ++k)
        planes.push_back(renderGaussianImage(W, H, static_cast<float>(100 * (k + 1)), {}));

    QString err;
    REQUIRE(writeSynthCube(path, W, H, planes, &err));

    auto result = core::loadFitsCube(path);
    REQUIRE(result.has_value());
    REQUIRE(result->size() == D);

    for (int k = 0; k < D; ++k) {
        const auto& frame = (*result)[k];
        CHECK(frame.width  == W);
        CHECK(frame.height == H);
        CHECK(frame.data.size() == static_cast<size_t>(W * H));
        // Plane k was filled with a flat background of 100*(k+1); spot-check
        // a pixel far from any edge effects.
        CHECK(frame.data[static_cast<size_t>(H / 2) * W + W / 2]
              == Catch::Approx(100.0 * (k + 1)).margin(0.01));
    }
}

TEST_CASE("loadFitsCube: rejects a plain 2-D image (not a cube)", "[loaders][cube]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("plain2d.fits");

    auto data = renderGaussianImage(16, 16, 100.0f, {});
    SynthHeader hdr;
    QString err;
    REQUIRE(writeSynthImage(path, 16, 16, data, hdr, &err));

    auto result = core::loadFitsCube(path);
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("loadFitsCube: rejects NAXIS3=3 (that's the RGB shape, not a cube)", "[loaders][cube]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("rgb3.fits");

    std::vector<std::vector<float>> planes;
    for (int k = 0; k < 3; ++k)
        planes.push_back(renderGaussianImage(16, 16, 50.0f, {}));
    QString err;
    REQUIRE(writeSynthCube(path, 16, 16, planes, &err));

    auto result = core::loadFitsCube(path);
    REQUIRE_FALSE(result.has_value());
}

// ─── loadFitsHdu / scanImageHdus ──────────────────────────────────────────────

TEST_CASE("scanImageHdus: finds all image extensions in a multi-HDU file", "[loaders][hdu]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("multi_hdu.fits");

    const int W = 20, H = 12;
    std::vector<std::vector<float>> planes = {
        renderGaussianImage(W, H, 10.0f, {}),
        renderGaussianImage(W, H, 20.0f, {}),
        renderGaussianImage(W, H, 30.0f, {}),
    };
    QString err;
    REQUIRE(writeSynthMultiHdu(path, W, H, planes, &err));

    auto hdus = core::scanImageHdus(path);
    // Primary HDU is empty (NAXIS=0) and must be skipped; 3 image extensions remain.
    REQUIRE(hdus.size() == 3);
    for (const auto& info : hdus) {
        CHECK(info.width  == W);
        CHECK(info.height == H);
        CHECK(info.naxis3 == 1);
        CHECK_FALSE(info.name.isEmpty());
    }
}

TEST_CASE("loadFitsHdu: reads a specific extension's pixel data", "[loaders][hdu]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("multi_hdu2.fits");

    const int W = 20, H = 12;
    std::vector<std::vector<float>> planes = {
        renderGaussianImage(W, H, 111.0f, {}),
        renderGaussianImage(W, H, 222.0f, {}),
    };
    QString err;
    REQUIRE(writeSynthMultiHdu(path, W, H, planes, &err));

    auto hdus = core::scanImageHdus(path);
    REQUIRE(hdus.size() == 2);

    // Load the second extension (HDU number from the scan) and confirm it's
    // the plane filled with 222, not the first one (111).
    auto result = core::loadFitsHdu(path, hdus[1].hduNumber);
    REQUIRE(result.has_value());
    CHECK(result->width  == W);
    CHECK(result->height == H);
    CHECK(result->data[0] == Catch::Approx(222.0).margin(0.01));
}

TEST_CASE("loadFitsHdu: error on out-of-range HDU number", "[loaders][hdu]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("multi_hdu3.fits");

    std::vector<std::vector<float>> planes = { renderGaussianImage(8, 8, 5.0f, {}) };
    QString err;
    REQUIRE(writeSynthMultiHdu(path, 8, 8, planes, &err));

    auto result = core::loadFitsHdu(path, 99);
    REQUIRE_FALSE(result.has_value());
}

// ─── FitsTableReader::readFitsTable ───────────────────────────────────────────

TEST_CASE("readFitsTable: reads a well-formed BINTABLE (header matches data on disk)",
          "[loaders][bintable]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("legit.fits");

    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/10, /*nRowsPhysical=*/10, &err));

    auto result = core::readFitsTable(path);
    REQUIRE(result.has_value());
    CHECK(result->nRows == 10);
    CHECK(result->hduName == QStringLiteral("TESTTBL"));

    const auto xs = result->column("X");
    REQUIRE(xs.size() == 10);
    CHECK(xs[0].toDouble() == Catch::Approx(10.0));
    CHECK(xs[9].toDouble() == Catch::Approx(19.0));
}

TEST_CASE("readFitsTable: NAXIS2 lies about row count (truncated data) -> returns error, "
          "not fabricated rows",
          "[loaders][bintable][regression][AUD-INPUT-5]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("naxis2_lie.fits");

    // Header declares 1000 rows; only 3 rows of real data remain on disk
    // after truncation — this is the "BINTABLE com NAXIS2 mentiroso" fixture
    // described in AUDIT_FIND.md (AUD-INPUT-5 / AUD-INPUT-7).
    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/1000, /*nRowsPhysical=*/3, &err));

    auto result = core::readFitsTable(path);
    // Must fail loudly. The historical bug (AUD-INPUT-5, fixed 2801ea8) was
    // readColumn() swallowing the short-read exception and returning 1000
    // NaN-filled rows with a SUCCESS status — asserting failure here is a
    // permanent regression guard for that fix.
    REQUIRE_FALSE(result.has_value());
    CHECK_FALSE(result.error().isEmpty());
}

TEST_CASE("readFitsTable: error on file with no BINTABLE extension", "[loaders][bintable]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("no_table.fits");

    auto data = renderGaussianImage(8, 8, 100.0f, {});
    SynthHeader hdr;
    QString err;
    REQUIRE(writeSynthImage(path, 8, 8, data, hdr, &err));

    auto result = core::readFitsTable(path);
    REQUIRE_FALSE(result.has_value());
}

// ─── FitsTableReader::importDaophotTable ──────────────────────────────────────

TEST_CASE("importDaophotTable: X/Y/MAG columns become DetectedStar entries", "[loaders][bintable]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("daophot.fits");

    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/5, /*nRowsPhysical=*/5, &err));

    auto result = core::importDaophotTable(path);
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 5);
    // X/Y in the fixture are 1-based FITS pixel coords (10..14 / 20..24);
    // importDaophotTable converts to 0-based.
    CHECK((*result)[0].x == Catch::Approx(9.0));
    CHECK((*result)[0].y == Catch::Approx(19.0));
}

TEST_CASE("importDaophotTable: NAXIS2-lie fixture propagates the read error "
          "(no NaN stars)", "[loaders][bintable][regression][AUD-INPUT-5]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("daophot_lie.fits");

    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/500, /*nRowsPhysical=*/2, &err));

    auto result = core::importDaophotTable(path);
    REQUIRE_FALSE(result.has_value());
}

// ─── FitsTableReader::readLocalCatalogTable ───────────────────────────────────

TEST_CASE("readLocalCatalogTable: needs RA/Dec columns, fails on X/Y-only table",
          "[loaders][bintable]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("catalog_no_radec.fits");

    // writeSynthBinTable's fixture columns are X/Y/MAG, no RA/Dec -- exactly
    // the "missing required column" case readLocalCatalogTable must reject.
    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/5, /*nRowsPhysical=*/5, &err));

    auto result = core::readLocalCatalogTable(path, 180.0, 20.0, 5.0);
    REQUIRE_FALSE(result.has_value());
}
