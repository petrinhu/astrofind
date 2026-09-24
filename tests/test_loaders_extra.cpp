// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-6 / AUD-INPUT-gaps: T1 coverage for loaders and extractors that had
// no TEST_CASE: loadSer, loadXisf (the cases not already in
// test_hostile_regression.cpp), loadQImage (PNG/TIFF), loadSpectrum1D (cases
// not in test_fits_loaders.cpp), importReductionTable and the libarchive
// extractor (core::extractArchiveImages). Each has a normal case and at least
// one hostile case. Every fixture is written into a QTemporaryDir by hand
// (byte-exact headers) or with cfitsio/QImage, so nothing depends on external
// files. The ZIP extractor shells out to unzip(1) through QProcess and lives in
// test_archive_zip_ui.cpp (astrofind_ui_tests, which has a QApplication).

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/ArchiveExtractor.h"
#include "core/FitsImage.h"
#include "core/FitsTableReader.h"
#include "core/ImageLoader.h"
#include "core/Spectrum1D.h"
#include "synthetic_fits.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QImageWriter>
#include <QTemporaryDir>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

using namespace testutil;

namespace {

bool writeBytes(const QString& path, const QByteArray& bytes)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return f.write(bytes) == bytes.size();
}

QByteArray readBytes(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) return {};
    return f.readAll();
}

void putLE32(QByteArray& b, int off, uint32_t v)
{
    for (int i = 0; i < 4; ++i)
        b[off + i] = static_cast<char>((v >> (8 * i)) & 0xFF);
}

void appendLE16(QByteArray& b, uint16_t v)
{
    b.append(static_cast<char>(v & 0xFF));
    b.append(static_cast<char>((v >> 8) & 0xFF));
}

void appendLE32(QByteArray& b, uint32_t v)
{
    for (int i = 0; i < 4; ++i) b.append(static_cast<char>((v >> (8 * i)) & 0xFF));
}

void appendBE32(QByteArray& b, uint32_t v)
{
    for (int i = 3; i >= 0; --i) b.append(static_cast<char>((v >> (8 * i)) & 0xFF));
}

// ─── SER ──────────────────────────────────────────────────────────────────────
// 178-byte header, all integers little-endian (SER v3): fileID[14], luID,
// colorID, littleEndian, width, height, pixelDepth, frameCount (uint32 at
// offsets 14..38), observer/instrument/telescope[40], two int64 timestamps.

QByteArray serHeader(uint32_t w, uint32_t h, uint32_t depth, uint32_t colorID,
                     uint32_t frames = 1, uint32_t littleEndian = 1)
{
    QByteArray hdr(178, '\0');
    std::memcpy(hdr.data(), "LUCAM-RECORDER", 14);
    putLE32(hdr, 18, colorID);
    putLE32(hdr, 22, littleEndian);
    putLE32(hdr, 26, w);
    putLE32(hdr, 30, h);
    putLE32(hdr, 34, depth);
    putLE32(hdr, 38, frames);
    std::memcpy(hdr.data() + 42, "Observer Name", 13);
    std::memcpy(hdr.data() + 122, "Test Scope", 10);
    return hdr;
}

// ─── XISF ─────────────────────────────────────────────────────────────────────

/// Monolithic XISF 1.0: "XISF0100" + XML length (LE) + 4 reserved bytes + XML.
QByteArray xisfContainer(const QByteArray& xml)
{
    QByteArray out("XISF0100");
    appendLE32(out, static_cast<uint32_t>(xml.size()));
    out.append(QByteArray(4, '\0'));
    out.append(xml);
    return out;
}

/// XISF with the pixel block attached right after the XML header.
QByteArray xisfAttached(const QByteArray& geometry, const QByteArray& sampleFormat,
                        const QByteArray& pixels, qsizetype declaredBlockSize = -1)
{
    const qsizetype blockSize = declaredBlockSize >= 0 ? declaredBlockSize : pixels.size();
    // 10-character placeholder replaced by a 10-digit offset: XML size fixed.
    QByteArray xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<xisf version=\"1.0\"><Image geometry=\"" + geometry + "\" sampleFormat=\""
        + sampleFormat + "\" location=\"attachment:@@OFFSET@@:"
        + QByteArray::number(static_cast<qlonglong>(blockSize)) + "\"/></xisf>";
    const qlonglong offset = 16 + xml.size();
    xml.replace("@@OFFSET@@", QByteArray::number(offset).rightJustified(10, '0'));
    return xisfContainer(xml) + pixels;
}

// ─── PNG ──────────────────────────────────────────────────────────────────────

uint32_t crc32(const QByteArray& data)
{
    uint32_t c = 0xFFFFFFFFu;
    for (const char ch : data) {
        c ^= static_cast<uint8_t>(ch);
        for (int k = 0; k < 8; ++k)
            c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
    }
    return c ^ 0xFFFFFFFFu;
}

QByteArray pngChunk(const QByteArray& type, const QByteArray& payload)
{
    QByteArray out;
    appendBE32(out, static_cast<uint32_t>(payload.size()));
    const QByteArray body = type + payload;
    out.append(body);
    appendBE32(out, crc32(body));
    return out;
}

/// Valid PNG signature + IHDR (correct CRC) declaring w x h 8-bit grayscale,
/// then IEND with NO image data: a header that lies about a huge image.
QByteArray pngHeaderOnly(uint32_t w, uint32_t h)
{
    QByteArray ihdr;
    appendBE32(ihdr, w);
    appendBE32(ihdr, h);
    ihdr.append('\x08');   // bit depth
    ihdr.append('\x00');   // colour type: grayscale
    ihdr.append('\x00');   // compression
    ihdr.append('\x00');   // filter
    ihdr.append('\x00');   // interlace
    return QByteArray("\x89PNG\r\n\x1a\n", 8) + pngChunk("IHDR", ihdr) + pngChunk("IEND", {});
}

/// Deterministic 16-bit test value for pixel (x, y).
uint16_t px16(int x, int y) { return static_cast<uint16_t>(1000 * y + 10 * x + 7); }

QImage gray16Image(int w, int h)
{
    QImage img(w, h, QImage::Format_Grayscale16);
    for (int y = 0; y < h; ++y) {
        auto* row = reinterpret_cast<quint16*>(img.scanLine(y));
        for (int x = 0; x < w; ++x) row[x] = px16(x, y);
    }
    return img;
}

// ─── FITS tables / spectra ───────────────────────────────────────────────────

/// BINTABLE (extension "REDUCED") with one "1D" (double) column per name.
bool writeDoubleTable(const QString& path, const std::vector<std::string>& names,
                      const std::vector<std::vector<double>>& cols)
{
    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;
    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long emptyAxes[1] = { 0 };
    fits_create_img(fptr, BYTE_IMG, 0, emptyAxes, &status);

    std::vector<std::string> ttypeS = names;
    std::vector<std::string> tformS(names.size(), "1D");
    std::vector<std::string> tunitS(names.size(), "");
    std::vector<char*> ttype, tform, tunit;
    for (size_t i = 0; i < names.size(); ++i) {
        ttype.push_back(ttypeS[i].data());
        tform.push_back(tformS[i].data());
        tunit.push_back(tunitS[i].data());
    }
    const LONGLONG nRows = cols.empty() ? 0 : static_cast<LONGLONG>(cols[0].size());
    fits_create_tbl(fptr, BINARY_TBL, nRows, static_cast<int>(names.size()),
                    ttype.data(), tform.data(), tunit.data(), "REDUCED", &status);
    for (size_t c = 0; c < cols.size(); ++c) {
        std::vector<double> v = cols[c];
        fits_write_col(fptr, TDOUBLE, static_cast<int>(c + 1), 1, 1, nRows, v.data(), &status);
    }
    const int saveStatus = status;
    fits_close_file(fptr, &status);
    return saveStatus == 0;
}

/// NAXIS=1 float spectrum with a full set of axis keywords.
bool writeLabelledSpectrum(const QString& path, const std::vector<float>& flux)
{
    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;
    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long naxes[1] = { static_cast<long>(flux.size()) };
    fits_create_img(fptr, FLOAT_IMG, 1, naxes, &status);
    double crval = 6500.0, cdelt = 0.5, crpix = 3.0;
    fits_write_key(fptr, TDOUBLE, "CRVAL1", &crval, nullptr, &status);
    fits_write_key(fptr, TDOUBLE, "CDELT1", &cdelt, nullptr, &status);
    fits_write_key(fptr, TDOUBLE, "CRPIX1", &crpix, nullptr, &status);
    char ctype[] = "WAVE", cunit[] = "Angstrom", bunit[] = "ADU";
    fits_write_key(fptr, TSTRING, "CTYPE1", ctype, nullptr, &status);
    fits_write_key(fptr, TSTRING, "CUNIT1", cunit, nullptr, &status);
    fits_write_key(fptr, TSTRING, "BUNIT", bunit, nullptr, &status);
    std::vector<float> v = flux;
    fits_write_img(fptr, TFLOAT, 1, static_cast<LONGLONG>(v.size()), v.data(), &status);
    const int saveStatus = status;
    fits_close_file(fptr, &status);
    return saveStatus == 0;
}

// ─── TAR (ustar, uncompressed; libarchive detects the format by content) ────

QByteArray octal(qint64 value, int width)   // width includes the trailing NUL
{
    return QByteArray::number(value, 8).rightJustified(width - 1, '0') + '\0';
}

/// One ustar entry: typeflag '0' regular, '2' symlink (linkname), '6' FIFO.
QByteArray tarEntry(const QByteArray& name, char typeflag, const QByteArray& data = {},
                    const QByteArray& linkname = {})
{
    QByteArray h(512, '\0');
    auto put = [&](int off, const QByteArray& v) { std::memcpy(h.data() + off, v.constData(), v.size()); };
    put(0, name.left(99));
    put(100, octal(typeflag == '2' ? 0777 : 0644, 8));
    put(108, octal(0, 8));
    put(116, octal(0, 8));
    put(124, octal(typeflag == '0' ? data.size() : 0, 12));
    put(136, octal(1700000000, 12));
    put(148, QByteArray(8, ' '));            // checksum field counts as spaces
    h[156] = typeflag;
    if (!linkname.isEmpty()) put(157, linkname.left(99));
    put(257, QByteArray("ustar\0", 6));
    put(263, QByteArray("00", 2));
    unsigned sum = 0;
    for (const char c : h) sum += static_cast<uint8_t>(c);
    put(148, QByteArray::number(sum, 8).rightJustified(6, '0') + QByteArray("\0 ", 2));

    QByteArray out = h;
    if (typeflag == '0' && !data.isEmpty()) {
        out.append(data);
        out.append(QByteArray((512 - data.size() % 512) % 512, '\0'));
    }
    return out;
}

QByteArray tarEnd() { return QByteArray(1024, '\0'); }

bool hasSymlinkIn(const QString& dir)
{
    const QFileInfoList entries = QDir(dir).entryInfoList(QDir::AllEntries | QDir::System
                                                          | QDir::Hidden | QDir::NoDotAndDotDot);
    for (const QFileInfo& fi : entries)
        if (fi.isSymLink()) return true;
    return false;
}

} // namespace

// ═══ loadSer ═════════════════════════════════════════════════════════════════

TEST_CASE("loadSer reads a 16-bit mono frame and the header strings", "[loaders][ser]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("mono16.ser");
    QByteArray bytes = serHeader(4, 3, 16, /*MONO*/0, /*frames=*/2);
    for (int y = 0; y < 3; ++y)
        for (int x = 0; x < 4; ++x) appendLE16(bytes, px16(x, y));
    bytes.append(QByteArray(4 * 3 * 2, '\x11'));   // frame 2 (ignored)
    REQUIRE(writeBytes(path, bytes));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->width == 4);
    CHECK(r->height == 3);
    CHECK_FALSE(r->isColor);
    REQUIRE(r->data.size() == 12u);
    CHECK(r->data[0] == static_cast<float>(px16(0, 0)));
    CHECK(r->data[11] == static_cast<float>(px16(3, 2)));
    CHECK(r->observer == QStringLiteral("Observer Name"));
    CHECK(r->telescope == QStringLiteral("Test Scope"));
}

TEST_CASE("loadSer reads interleaved 8-bit RGB into three planes", "[loaders][ser]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("rgb8.ser");
    QByteArray bytes = serHeader(2, 1, 8, /*SER_RGB*/100);
    bytes.append(QByteArray("\x0a\x14\x1e\x28\x32\x3c", 6));   // R0G0B0 R1G1B1
    REQUIRE(writeBytes(path, bytes));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->isColor);
    REQUIRE(r->dataR.size() == 2u);
    CHECK(r->dataR[0] == 10.0f);
    CHECK(r->dataG[0] == 20.0f);
    CHECK(r->dataB[0] == 30.0f);
    CHECK(r->dataR[1] == 40.0f);
    CHECK(r->dataB[1] == 60.0f);
}

TEST_CASE("loadSer rejects a sign-bit width before narrowing it", "[loaders][ser][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("huge.ser");
    QByteArray bytes = serHeader(0xFFFFFFFFu, 16, 16, 0);
    bytes.append(QByteArray(64, '\0'));
    REQUIRE(writeBytes(path, bytes));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("out of range")));
}

TEST_CASE("loadSer rejects a frame shorter than the header declares", "[loaders][ser][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("truncated.ser");
    QByteArray bytes = serHeader(64, 64, 16, 0);
    bytes.append(QByteArray(10, '\x01'));          // needs 8192 bytes
    REQUIRE(writeBytes(path, bytes));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("truncated")));
}

TEST_CASE("loadSer rejects an unsupported pixel depth and a short file", "[loaders][ser][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());

    const QString depthPath = dir.filePath("depth12.ser");
    QByteArray bytes = serHeader(2, 2, 12, 0);
    bytes.append(QByteArray(16, '\0'));
    REQUIRE(writeBytes(depthPath, bytes));
    auto r1 = core::loadImage(depthPath);
    REQUIRE_FALSE(r1.has_value());
    CHECK(r1.error().contains(QStringLiteral("pixel depth")));

    const QString shortPath = dir.filePath("short.ser");
    REQUIRE(writeBytes(shortPath, QByteArray("LUCAM-RECORDER", 14)));
    auto r2 = core::loadImage(shortPath);
    REQUIRE_FALSE(r2.has_value());
    CHECK(r2.error().contains(QStringLiteral("too small")));
}

// ═══ loadXisf (valid 2x2 UInt8 and the geometry ceilings are in
//     test_hostile_regression.cpp) ═════════════════════════════════════════════

TEST_CASE("loadXisf reads an embedded base64 Float32 image and FITS keywords", "[loaders][xisf]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("embedded.xisf");

    const float values[4] = { 1.5f, -2.0f, 3.25f, 1.0e6f };
    const QByteArray raw(reinterpret_cast<const char*>(values), sizeof(values));  // LE on CI
    const QByteArray xml =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<xisf version=\"1.0\"><Image geometry=\"2:2:1\" sampleFormat=\"Float32\" "
        "location=\"embedded\"><Data encoding=\"base64\">" + raw.toBase64() + "</Data>"
        "<FITSKeyword name=\"OBJECT\" value=\"'M42'\" comment=\"\"/>"
        "<FITSKeyword name=\"EXPTIME\" value=\"12.5\" comment=\"\"/>"
        "</Image></xisf>";
    REQUIRE(writeBytes(path, xisfContainer(xml)));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    REQUIRE(r->data.size() == 4u);
    CHECK(r->data[0] == 1.5f);
    CHECK(r->data[1] == -2.0f);
    CHECK(r->data[3] == 1.0e6f);
    CHECK(r->objectName == QStringLiteral("M42"));
    CHECK(r->expTime == Catch::Approx(12.5));
}

TEST_CASE("loadXisf reads planar UInt16 RGB", "[loaders][xisf]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("rgb16.xisf");
    QByteArray planes;
    for (uint16_t v : {10, 20, 30, 40, 50, 60}) appendLE16(planes, v);   // R0 R1 G0 G1 B0 B1
    REQUIRE(writeBytes(path, xisfAttached("2:1:3", "UInt16", planes)));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->isColor);
    REQUIRE(r->dataR.size() == 2u);
    CHECK(r->dataR[1] == 20.0f);
    CHECK(r->dataG[0] == 30.0f);
    CHECK(r->dataB[1] == 60.0f);
}

TEST_CASE("loadXisf rejects pixel data shorter than the geometry", "[loaders][xisf][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("short_block.xisf");
    // 8x8 UInt8 needs 64 bytes (the file itself is larger, so the file-size
    // cross-check passes); only 16 are attached.
    REQUIRE(writeBytes(path, xisfAttached("8:8:1", "UInt8", QByteArray(16, '\x07'), 64)));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("truncated")));
}

TEST_CASE("loadXisf rejects a bad signature and an XML length past the end of file",
          "[loaders][xisf][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());

    const QString sigPath = dir.filePath("badsig.xisf");
    REQUIRE(writeBytes(sigPath, QByteArray("XISF0200") + QByteArray(64, '\0')));
    auto r1 = core::loadImage(sigPath);
    REQUIRE_FALSE(r1.has_value());
    CHECK(r1.error().contains(QStringLiteral("Not an XISF")));

    const QString lenPath = dir.filePath("xmllie.xisf");
    QByteArray bytes = xisfContainer("<xisf/>");
    putLE32(bytes, 8, 4096);                     // claims 4 KB of XML, file has 7 bytes
    REQUIRE(writeBytes(lenPath, bytes));
    auto r2 = core::loadImage(lenPath);
    REQUIRE_FALSE(r2.has_value());
    CHECK(r2.error().contains(QStringLiteral("truncated")));
}

// ═══ loadQImage (PNG / TIFF through Qt) ═══════════════════════════════════════

TEST_CASE("loadQImage keeps the full 16-bit range of a grayscale PNG", "[loaders][qimage]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("gray16.png");
    REQUIRE(gray16Image(4, 3).save(path, "PNG"));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->width == 4);
    CHECK(r->height == 3);
    CHECK_FALSE(r->isColor);
    REQUIRE(r->data.size() == 12u);
    CHECK(r->data[0] == static_cast<float>(px16(0, 0)));
    CHECK(r->data[11] == static_cast<float>(px16(3, 2)));   // 2037 > 255: not 8-bit
}

TEST_CASE("loadQImage splits an 8-bit RGB PNG into three planes", "[loaders][qimage]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("rgb.png");
    QImage img(3, 2, QImage::Format_RGB32);
    img.fill(qRgb(0, 0, 0));
    img.setPixel(2, 1, qRgb(200, 100, 50));
    REQUIRE(img.save(path, "PNG"));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->isColor);
    REQUIRE(r->dataR.size() == 6u);
    CHECK(r->dataR[5] == 200.0f);
    CHECK(r->dataG[5] == 100.0f);
    CHECK(r->dataB[5] == 50.0f);
    CHECK(r->dataR[0] == 0.0f);
}

TEST_CASE("loadQImage reads a 16-bit TIFF or refuses it cleanly without the plugin",
          "[loaders][qimage][tiff]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("gray16.tif");

    // TIFF support is the optional qtimageformats plugin: both branches assert.
    if (QImageWriter::supportedImageFormats().contains(QByteArray("tiff"))) {
        REQUIRE(gray16Image(5, 2).save(path, "TIFF"));
        auto r = core::loadImage(path);
        REQUIRE(r.has_value());
        CHECK(r->width == 5);
        CHECK(r->height == 2);
        CHECK_FALSE(r->isColor);
        REQUIRE(r->data.size() == 10u);
        CHECK(r->data[9] == static_cast<float>(px16(4, 1)));
    } else {
        REQUIRE(writeBytes(path, QByteArray("II*\0", 4) + QByteArray(64, '\x33')));
        auto r = core::loadImage(path);
        REQUIRE_FALSE(r.has_value());
        CHECK(r.error().contains(QStringLiteral("Cannot load image")));
    }
}

TEST_CASE("loadQImage fails cleanly on a truncated PNG", "[loaders][qimage][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("truncated.png");
    // Noise does not compress, so the IDAT is most of the file.
    QImage img(64, 64, QImage::Format_Grayscale8);
    uint32_t lcg = 12345u;
    for (int y = 0; y < 64; ++y)
        for (int x = 0; x < 64; ++x) {
            lcg = lcg * 1664525u + 1013904223u;
            img.scanLine(y)[x] = static_cast<uchar>(lcg >> 24);
        }
    REQUIRE(img.save(path, "PNG"));
    const QByteArray full = readBytes(path);
    REQUIRE(full.size() > 1000);
    REQUIRE(writeBytes(path, full.left(full.size() * 6 / 10)));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
}

TEST_CASE("loadQImage refuses a PNG header declaring 100000 x 100000 px", "[loaders][qimage][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("huge_header.png");
    REQUIRE(writeBytes(path, pngHeaderOnly(100000, 100000)));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("too large")));
}

TEST_CASE("loadQImage applies the loader axis ceiling to a real 20001 px wide PNG",
          "[loaders][qimage][hostile]")
{
    // Small enough for Qt's own allocation limit, so only AstroFind's ceiling
    // (the one every other loader has, AUD-INPUT-gaps) can refuse it.
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("wide.png");
    QImage img(20001, 1, QImage::Format_Grayscale8);
    img.fill(0);
    REQUIRE(img.save(path, "PNG"));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("too large")));
}

TEST_CASE("validateDecodedDims skips only the file-size cross-check", "[loaders][qimage]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("tiny.bin");
    REQUIRE(writeBytes(path, QByteArray(16, '\0')));

    QString err;
    // 1000x1000 px from a 16-byte file: compressed formats do this legitimately.
    CHECK(core::validateDecodedDims(1000, 1000, path, err));
    CHECK_FALSE(core::validateLoaderDims(1000, 1000, 1, path, err));   // raw loaders still refuse
    CHECK_FALSE(core::validateDecodedDims(20001, 10, path, err));
    CHECK_FALSE(core::validateDecodedDims(0, 10, path, err));
}

// ═══ loadSpectrum1D (valid 8-channel, lying NAXIS1 and 1e8 channels are in
//     test_fits_loaders.cpp) ═══════════════════════════════════════════════════

TEST_CASE("loadSpectrum1D honours CRPIX1 and reads the axis keywords", "[loaders][spectrum]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("labelled.fits");
    REQUIRE(writeLabelledSpectrum(path, {10, 20, 30, 40, 50}));

    auto spec = core::loadSpectrum1D(path);
    REQUIRE(spec.has_value());
    REQUIRE(spec->flux.size() == 5);
    // lambda(i) = CRVAL1 + CDELT1 * (i + 1 - CRPIX1): pixel 3 (i = 2) is CRVAL1.
    CHECK(spec->wavelength[2] == Catch::Approx(6500.0));
    CHECK(spec->wavelength[0] == Catch::Approx(6499.0));
    CHECK(spec->flux[4] == Catch::Approx(50.0));
    CHECK(spec->ctype1 == QStringLiteral("WAVE"));
    CHECK(spec->cunit1 == QStringLiteral("Angstrom"));
    CHECK(spec->bunit == QStringLiteral("ADU"));
}

TEST_CASE("loadSpectrum1D refuses a 2-D image and a missing file", "[loaders][spectrum][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("image2d.fits");
    REQUIRE(writeSynthImage(path, 4, 4, renderGaussianImage(4, 4, 10.0f, {}), SynthHeader{}));

    auto r1 = core::loadSpectrum1D(path);
    REQUIRE_FALSE(r1.has_value());
    CHECK(r1.error().contains(QStringLiteral("Not a 1-D spectrum")));

    auto r2 = core::loadSpectrum1D(dir.filePath("does_not_exist.fits"));
    REQUIRE_FALSE(r2.has_value());
    CHECK_FALSE(r2.error().isEmpty());
}

// ═══ importReductionTable ═════════════════════════════════════════════════════

TEST_CASE("importReductionTable maps 1-based pixel columns and MAG", "[loaders][bintable][reduction]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("pixels.fits");
    QString err;
    REQUIRE(writeSynthBinTable(path, 5, 5, &err));   // X = 10.., Y = 20.., MAG = 15 + 0.01 i

    auto r = core::importReductionTable(path);
    REQUIRE(r.has_value());
    REQUIRE(r->size() == 5);
    CHECK((*r)[0].x == Catch::Approx(9.0));
    CHECK((*r)[0].y == Catch::Approx(19.0));
    CHECK((*r)[4].x == Catch::Approx(13.0));
    CHECK_FALSE((*r)[0].matched);
    CHECK((*r)[0].snr == Catch::Approx(1.0));        // 10^((15 - 15) / 2.5)
}

TEST_CASE("importReductionTable builds sky-only entries and drops non-finite rows",
          "[loaders][bintable][reduction][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("sky.fits");
    const double nan = std::numeric_limits<double>::quiet_NaN();
    REQUIRE(writeDoubleTable(path, {"RA", "DEC"},
                             {{150.25, nan, 150.75}, {2.5, 2.6, -2.7}}));

    auto r = core::importReductionTable(path);
    REQUIRE(r.has_value());
    REQUIRE(r->size() == 2);                          // the NaN row is skipped
    CHECK((*r)[0].x == Catch::Approx(-1.0));          // no pixel columns
    CHECK((*r)[0].matched);
    CHECK((*r)[0].ra == Catch::Approx(150.25));
    CHECK((*r)[1].dec == Catch::Approx(-2.7));
}

TEST_CASE("importReductionTable refuses a table without position columns",
          "[loaders][bintable][reduction][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("nopos.fits");
    REQUIRE(writeDoubleTable(path, {"FOO", "BAR"}, {{1.0, 2.0}, {3.0, 4.0}}));

    auto r = core::importReductionTable(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("No position columns")));
}

TEST_CASE("importReductionTable propagates a lying NAXIS2 instead of inventing rows",
          "[loaders][bintable][reduction][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("lie.fits");
    QString err;
    REQUIRE(writeSynthBinTable(path, 400, 2, &err));

    auto r = core::importReductionTable(path);
    REQUIRE_FALSE(r.has_value());
}

// ═══ Archive extraction (core::extractArchiveImages, libarchive) ══════════════

TEST_CASE("isSafeRegularFile accepts only plain regular files", "[archive][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString regular = dir.filePath("a.fits");
    REQUIRE(writeBytes(regular, "data"));
    const QString link = dir.filePath("link.fits");
    REQUIRE(QFile::link(regular, link));

    CHECK(core::isSafeRegularFile(regular));
    CHECK_FALSE(core::isSafeRegularFile(link));       // lstat: the link itself
    CHECK_FALSE(core::isSafeRegularFile(dir.path())); // a directory
    CHECK_FALSE(core::isSafeRegularFile(dir.filePath("missing.fits")));
}

#ifdef ASTROFIND_HAS_LIBARCHIVE

TEST_CASE("extractArchiveImages flattens image entries and skips other files", "[archive]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString tar = dir.filePath("set.tar");
    REQUIRE(writeBytes(tar, tarEntry("night1/a.fits", '0', "FITS-A")
                            + tarEntry("notes.txt", '0', "not an image")
                            + tarEntry("b.XISF", '0', "XISF-B")
                            + tarEnd()));
    const QString dest = dir.filePath("out");
    REQUIRE(QDir().mkpath(dest));

    const auto ex = core::extractArchiveImages(tar, dest);
    CHECK(ex.status == core::ArchiveExtraction::Status::Ok);
    CHECK(ex.skippedNonRegular.isEmpty());
    REQUIRE(ex.files.size() == 2);
    CHECK(QFileInfo(ex.files[0]).fileName() == QStringLiteral("a.fits"));
    CHECK(QFileInfo(ex.files[0]).absolutePath() == QFileInfo(dest).absoluteFilePath());
    CHECK(QFileInfo(ex.files[1]).fileName() == QStringLiteral("b.XISF"));
    CHECK(readBytes(ex.files[0]) == QByteArray("FITS-A"));
    CHECK_FALSE(QFileInfo::exists(QDir(dest).filePath("notes.txt")));
}

TEST_CASE("extractArchiveImages refuses symlink and FIFO entries and path traversal",
          "[archive][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString secret = dir.filePath("secret.txt");
    REQUIRE(writeBytes(secret, "top secret"));
    const QString tar = dir.filePath("evil.tar");
    REQUIRE(writeBytes(tar, tarEntry("link.fits", '2', {}, secret.toLocal8Bit())
                            + tarEntry("pipe.fits", '6')
                            + tarEntry("../../escape.fits", '0', "ESCAPE")
                            + tarEntry("ok.fits", '0', "OK")
                            + tarEnd()));
    const QString dest = dir.filePath("out/sub");
    REQUIRE(QDir().mkpath(dest));

    const auto ex = core::extractArchiveImages(tar, dest);
    CHECK(ex.status == core::ArchiveExtraction::Status::Ok);
    CHECK(ex.skippedNonRegular.contains(QStringLiteral("link.fits")));
    CHECK(ex.skippedNonRegular.contains(QStringLiteral("pipe.fits")));
    CHECK_FALSE(hasSymlinkIn(dest));
    for (const QString& f : ex.files) {
        CHECK(core::isSafeRegularFile(f));
        CHECK(readBytes(f) != QByteArray("top secret"));
    }
    // "../../escape.fits" is flattened INTO dest, never written above it.
    CHECK_FALSE(QFileInfo::exists(dir.filePath("escape.fits")));
    CHECK_FALSE(QFileInfo::exists(dir.filePath("out/escape.fits")));
    CHECK(ex.files.contains(QDir(dest).filePath("ok.fits")));
}

TEST_CASE("extractArchiveImages reports a missing archive and survives garbage",
          "[archive][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString dest = dir.filePath("out");
    REQUIRE(QDir().mkpath(dest));

    const auto missing = core::extractArchiveImages(dir.filePath("nope.tar.gz"), dest);
    CHECK(missing.status == core::ArchiveExtraction::Status::OpenFailed);
    CHECK_FALSE(missing.error.isEmpty());
    CHECK(missing.files.isEmpty());

    const QString junk = dir.filePath("junk.tar.xz");
    REQUIRE(writeBytes(junk, QByteArray(4096, '\x5A')));
    const auto garbage = core::extractArchiveImages(junk, dest);
    CHECK(garbage.files.isEmpty());
    CHECK(QDir(dest).entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty());
}

#else

TEST_CASE("extractArchiveImages reports NotSupported without libarchive", "[archive]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const auto ex = core::extractArchiveImages(dir.filePath("set.tar.gz"), dir.path());
    CHECK(ex.status == core::ArchiveExtraction::Status::NotSupported);
    CHECK(ex.files.isEmpty());
    CHECK_FALSE(core::hasLibArchiveSupport());
}

#endif // ASTROFIND_HAS_LIBARCHIVE
