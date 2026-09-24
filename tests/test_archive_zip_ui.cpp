// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-6: core::extractZipImages (the body of MainWindow::expandZip). It
// shells out to unzip(1) through QProcess, so it is registered in
// astrofind_ui_tests, which provides a real QApplication (test_ui_main.cpp).
// The ZIP fixtures are written byte by byte (stored entries, no compression),
// including a Unix symlink entry — the AUD-INPUT-4 attack. unzip(1) is an
// optional runtime tool: without it the cases SKIP with that reason (the
// audit workflow installs it, so they run there on every distro).

#include <catch2/catch_test_macros.hpp>

#include "core/ArchiveExtractor.h"

#include <QByteArray>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTemporaryDir>

#include <cstdint>
#include <vector>

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

void le16(QByteArray& b, uint32_t v)
{
    b.append(static_cast<char>(v & 0xFF));
    b.append(static_cast<char>((v >> 8) & 0xFF));
}

void le32(QByteArray& b, uint32_t v)
{
    for (int i = 0; i < 4; ++i) b.append(static_cast<char>((v >> (8 * i)) & 0xFF));
}

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

struct ZipEntry {
    QByteArray name;
    QByteArray data;          ///< File content, or the link target for a symlink
    bool       symlink = false;
};

/// Minimal ZIP (method 0 = stored), "made by" Unix so unzip honours the mode
/// bits in the external attributes (0100644 regular, 0120777 symlink).
QByteArray makeZip(const std::vector<ZipEntry>& entries)
{
    QByteArray out, central;
    for (const ZipEntry& e : entries) {
        const uint32_t crc = crc32(e.data);
        const uint32_t size = static_cast<uint32_t>(e.data.size());
        const uint32_t offset = static_cast<uint32_t>(out.size());

        le32(out, 0x04034b50u);                // local file header
        le16(out, 10);                         // version needed
        le16(out, 0);                          // flags
        le16(out, 0);                          // method: stored
        le16(out, 0);                          // mod time
        le16(out, 0x21);                       // mod date 1980-01-01
        le32(out, crc);
        le32(out, size);
        le32(out, size);
        le16(out, static_cast<uint32_t>(e.name.size()));
        le16(out, 0);                          // extra length
        out.append(e.name);
        out.append(e.data);

        le32(central, 0x02014b50u);            // central directory header
        le16(central, (3u << 8) | 20u);        // made by: Unix, spec 2.0
        le16(central, 10);
        le16(central, 0);
        le16(central, 0);
        le16(central, 0);
        le16(central, 0x21);
        le32(central, crc);
        le32(central, size);
        le32(central, size);
        le16(central, static_cast<uint32_t>(e.name.size()));
        le16(central, 0);                      // extra
        le16(central, 0);                      // comment
        le16(central, 0);                      // disk
        le16(central, 0);                      // internal attributes
        le32(central, (e.symlink ? 0120777u : 0100644u) << 16);
        le32(central, offset);
        central.append(e.name);
    }
    const uint32_t cdOffset = static_cast<uint32_t>(out.size());
    out.append(central);
    le32(out, 0x06054b50u);                    // end of central directory
    le16(out, 0);
    le16(out, 0);
    le16(out, static_cast<uint32_t>(entries.size()));
    le16(out, static_cast<uint32_t>(entries.size()));
    le32(out, static_cast<uint32_t>(central.size()));
    le32(out, cdOffset);
    le16(out, 0);
    return out;
}

bool unzipAvailable() { return !QStandardPaths::findExecutable(QStringLiteral("unzip")).isEmpty(); }

} // namespace

TEST_CASE("extractZipImages extracts image entries and ignores the rest", "[archive][zip]")
{
    if (!unzipAvailable())
        SKIP("unzip(1) not installed");
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString zip = dir.filePath("set.zip");
    REQUIRE(writeBytes(zip, makeZip({{"night1/a.fits", "FITS-A"},
                                     {"readme.txt", "not an image"},
                                     {"B.PNG", "PNG-B"}})));
    const QString dest = dir.filePath("out");
    REQUIRE(QDir().mkpath(dest));

    const auto ex = core::extractZipImages(zip, dest);
    REQUIRE(ex.status == core::ArchiveExtraction::Status::Ok);
    CHECK(ex.skippedNonRegular.isEmpty());
    REQUIRE(ex.files.size() == 2);
    QStringList names;
    for (const QString& f : ex.files) names << QFileInfo(f).fileName();
    CHECK(names.contains(QStringLiteral("a.fits")));
    CHECK(names.contains(QStringLiteral("B.PNG")));
    CHECK(readBytes(QDir(dest).filePath("night1/a.fits")) == QByteArray("FITS-A"));
    CHECK_FALSE(QFileInfo::exists(QDir(dest).filePath("readme.txt")));
}

TEST_CASE("extractZipImages never returns a symlink entry or a file outside the target",
          "[archive][zip][hostile]")
{
    if (!unzipAvailable())
        SKIP("unzip(1) not installed");
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString secret = dir.filePath("secret.txt");
    REQUIRE(writeBytes(secret, "top secret"));
    const QString zip = dir.filePath("evil.zip");
    REQUIRE(writeBytes(zip, makeZip({{"evil.fits", secret.toLocal8Bit(), /*symlink=*/true},
                                     {"../escape.fits", "ESCAPE"},
                                     {"ok.fits", "OK"}})));
    const QString dest = dir.filePath("out/sub");
    REQUIRE(QDir().mkpath(dest));

    const auto ex = core::extractZipImages(zip, dest);
    REQUIRE(ex.status == core::ArchiveExtraction::Status::Ok);
    for (const QString& f : ex.files) {
        CHECK(core::isSafeRegularFile(f));
        CHECK(readBytes(f) != QByteArray("top secret"));
    }
    CHECK(ex.files.contains(QDir(dest).filePath("ok.fits")));
    // unzip recreates the entry as a symlink: it must be refused, not returned.
    const QString evil = QDir(dest).filePath("evil.fits");
    if (QFileInfo(evil).isSymLink())
        CHECK(ex.skippedNonRegular.contains(evil));
    CHECK_FALSE(ex.files.contains(evil));
    // unzip strips "../" by default; nothing may land above the target.
    CHECK_FALSE(QFileInfo::exists(dir.filePath("out/escape.fits")));
    CHECK_FALSE(QFileInfo::exists(dir.filePath("escape.fits")));
}

TEST_CASE("extractZipImages returns nothing for a corrupt ZIP", "[archive][zip][hostile]")
{
    if (!unzipAvailable())
        SKIP("unzip(1) not installed");
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString zip = dir.filePath("junk.zip");
    REQUIRE(writeBytes(zip, QByteArray("PK\x03\x04", 4) + QByteArray(512, '\x5A')));
    const QString dest = dir.filePath("out");
    REQUIRE(QDir().mkpath(dest));

    const auto ex = core::extractZipImages(zip, dest, 10000);
    CHECK(ex.status == core::ArchiveExtraction::Status::Ok);
    CHECK(ex.files.isEmpty());
}
