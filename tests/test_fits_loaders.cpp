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
#include "core/Spectrum1D.h"
#include "synthetic_fits.h"

#include <QTemporaryDir>
#include <QDir>
#include <QFile>

#include <vector>

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

TEST_CASE("loadFitsCube: rejects NAXIS3=3 (that's the RGB shape - not a cube)", "[loaders][cube]")
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

TEST_CASE("readFitsTable: NAXIS2 lies about row count (truncated data) -> returns error - "
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

// AUD-INPUT-12: NAXIS2 patched to 2e9 on a 3-row table. Before the fix the
// value went unchecked into reserve()/static_cast<int>; now it must be refused
// by the row ceiling before anything is allocated from it.
TEST_CASE("readFitsTable: huge NAXIS2 is refused before any allocation",
          "[loaders][bintable][hostile][AUD-INPUT-12]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("naxis2_huge.fits");
    QString err;
    REQUIRE(writeSynthBinTable(path, /*nRowsDeclared=*/3, /*nRowsPhysical=*/3, &err));

    auto patchNaxis2 = [&](qint64 rows) {
        QFile f(path);
        REQUIRE(f.open(QIODevice::ReadWrite));
        QByteArray bytes = f.readAll();
        // Second HDU (the BINTABLE) header starts after the 2880-byte primary.
        const qsizetype at = bytes.indexOf("NAXIS2  =", 2880);
        REQUIRE(at > 0);
        const QByteArray card = QByteArray("NAXIS2  = ")
            + QByteArray::number(rows).rightJustified(20, ' ');
        bytes.replace(at, card.size(), card);
        REQUIRE(f.seek(0));
        REQUIRE(f.write(bytes) == bytes.size());
    };

    patchNaxis2(2'000'000'000LL);
    auto huge = core::readFitsTable(path);
    REQUIRE_FALSE(huge.has_value());
    CHECK(huge.error().contains(QStringLiteral("ceiling")));

    // Under the ceiling but far beyond the file: refused by the size check.
    patchNaxis2(1'000'000LL);
    auto lie = core::readFitsTable(path);
    REQUIRE_FALSE(lie.has_value());
    CHECK(lie.error().contains(QStringLiteral("lying/corrupt header")));
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

TEST_CASE("importDaophotTable: optional variable-length (TFORM=1PE) FLUX column "
          "fails cleanly - never an OOB/short read (AUD-INPUT-7)",
          "[loaders][bintable][regression][AUD-INPUT-7]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("daophot_varlen_flux.fits");

    QString err;
    REQUIRE(writeSynthBinTableWithVarLenOptionalColumn(path, /*nRows=*/5, &err));

    // readColumn() cannot extract a std::vector<double> out of a P/Q
    // (ColumnVectorData) column at all -- CCfits throws WrongColumnType,
    // which readFitsTable() turns into a clean error. importDaophotTable
    // must propagate that error rather than silently indexing a
    // shorter-than-nRows FLUX column out of bounds.
    auto result = core::importDaophotTable(path);
    REQUIRE_FALSE(result.has_value());
}

// ─── FitsTableReader::readLocalCatalogTable ───────────────────────────────────

TEST_CASE("readLocalCatalogTable: needs RA/Dec columns - fails on X/Y-only table",
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

// ─── loadSpectrum1D (AUD-INPUT-8) ────────────────────────────────────────────

namespace {

/// Write a valid NAXIS=1 float spectrum of `n` channels.
bool writeSpectrum1D(const QString& path, const std::vector<float>& flux)
{
    fitsfile* fptr = nullptr;
    int status = 0;
    QFile::remove(path);
    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long naxes[1] = {static_cast<long>(flux.size())};
    fits_create_img(fptr, FLOAT_IMG, 1, naxes, &status);
    double crval = 4000.0, cdelt = 2.0;
    fits_write_key(fptr, TDOUBLE, "CRVAL1", &crval, nullptr, &status);
    fits_write_key(fptr, TDOUBLE, "CDELT1", &cdelt, nullptr, &status);
    fits_write_img(fptr, TFLOAT, 1, static_cast<LONGLONG>(flux.size()),
                   const_cast<float*>(flux.data()), &status);
    fits_close_file(fptr, &status);
    return status == 0;
}

/// Overwrite the NAXIS1 card value in place (header lies, file stays small).
bool patchNaxis1(const QString& path, long declared)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadWrite)) return false;
    QByteArray hdr = f.read(2880);
    const int pos = hdr.indexOf("NAXIS1  =");
    if (pos < 0 || pos % 80 != 0) return false;
    // Fixed-format integer: value right-justified in columns 11-30.
    const QByteArray value = QByteArray::number(static_cast<qlonglong>(declared)).rightJustified(20, ' ');
    f.seek(pos + 10);
    return f.write(value) == 20;
}

} // namespace

TEST_CASE("loadSpectrum1D reads a small valid spectrum", "[loaders][spectrum]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("spec8.fits");
    REQUIRE(writeSpectrum1D(path, {1, 2, 3, 4, 5, 6, 7, 8}));

    auto spec = core::loadSpectrum1D(path);
    REQUIRE(spec.has_value());
    REQUIRE(spec->flux.size() == 8);
    CHECK(spec->flux[7] == Catch::Approx(8.0));
    CHECK(spec->wavelength[0] == Catch::Approx(4000.0));
    CHECK(spec->wavelength[1] == Catch::Approx(4002.0));
}

TEST_CASE("loadSpectrum1D rejects a lying NAXIS1 larger than the file", "[loaders][spectrum][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("spec_lying.fits");
    REQUIRE(writeSpectrum1D(path, std::vector<float>(8, 1.0f)));
    // 1e6 channels = 4 MB of float data declared in a 5760-byte file: under
    // the channel ceiling, so only the file-size cross-check can reject it.
    REQUIRE(patchNaxis1(path, 1'000'000));

    auto spec = core::loadSpectrum1D(path);
    REQUIRE_FALSE(spec.has_value());
    CHECK(spec.error().contains("exceeds"));
}

TEST_CASE("loadSpectrum1D rejects NAXIS1 above the channel ceiling", "[loaders][spectrum][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("spec_1e8.fits");
    REQUIRE(writeSpectrum1D(path, std::vector<float>(8, 1.0f)));
    REQUIRE(patchNaxis1(path, 100'000'000)); // audit payload (AUD-INPUT-8)

    auto spec = core::loadSpectrum1D(path);
    REQUIRE_FALSE(spec.has_value());
    CHECK(spec.error().contains("too large"));
}
