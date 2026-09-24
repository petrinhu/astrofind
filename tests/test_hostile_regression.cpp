// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-4 / AUD-INPUT-9: regression net for the July CRITICAL fixes
// (AUD-INPUT-1/2, AUD-MEM-2/3) and the XISF ceiling. Every fixture is a tiny
// file whose HEADER lies (huge or unsupported axes); the loaders must refuse
// it from the header alone, with the specific guard message, before any large
// allocation. Checking the message (not just "failed") is what makes these
// tests catch a weakened guard: with the NAXIS guard removed cfitsio still
// fails, but later and with a different error.

#include <catch2/catch_test_macros.hpp>

#include "core/Centroid.h"
#include "core/FitsImage.h"
#include "core/ImageLoader.h"

#include <QByteArray>
#include <QFile>
#include <QTemporaryDir>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <vector>

namespace {

/// One 80-column FITS header card.
QByteArray card(const QByteArray& key, const QByteArray& value)
{
    QByteArray c = key.leftJustified(8, ' ') + "= " + value.rightJustified(20, ' ');
    return c.leftJustified(80, ' ');
}

/// Write a FITS file by hand: a header declaring `axes` with BITPIX=-32 and
/// `dataBytes` zero bytes of pixel data (padded to 2880). The header may lie
/// about the data size — that is the point.
bool writeRawFits(const QString& path, const std::vector<long long>& axes, qint64 dataBytes)
{
    QByteArray hdr;
    hdr += card("SIMPLE", "T");
    hdr += card("BITPIX", "-32");
    hdr += card("NAXIS", QByteArray::number(static_cast<qlonglong>(axes.size())));
    for (size_t i = 0; i < axes.size(); ++i)
        hdr += card("NAXIS" + QByteArray::number(static_cast<qlonglong>(i + 1)),
                    QByteArray::number(axes[i]));
    hdr += QByteArray("END").leftJustified(80, ' ');
    hdr = hdr.leftJustified(((hdr.size() + 2879) / 2880) * 2880, ' ');

    QByteArray data(static_cast<qsizetype>(dataBytes), '\0');
    if (!data.isEmpty())
        data = data.leftJustified(((data.size() + 2879) / 2880) * 2880, '\0');

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return f.write(hdr) == hdr.size() && f.write(data) == data.size();
}

/// Minimal monolithic XISF 1.0: signature, XML header, attached UInt8 pixels.
bool writeXisf(const QString& path, const QByteArray& geometry, const QByteArray& pixels)
{
    // Data block starts right after the (fixed-size) XML, at a known offset.
    const QByteArray xmlTemplate =
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<xisf version=\"1.0\"><Image geometry=\"" + geometry + "\" sampleFormat=\"UInt8\" "
        "colorSpace=\"Gray\" location=\"attachment:%1:" +
        QByteArray::number(static_cast<qlonglong>(pixels.size())) + "\"/></xisf>";
    // Offset has a fixed width (10 digits) so the XML length does not depend on it.
    QByteArray xml = xmlTemplate;
    const qint64 offset = 16 + xml.size() - 2 + 10;   // "%1" (2 chars) -> 10 digits
    xml.replace("%1", QByteArray::number(static_cast<qlonglong>(offset)).rightJustified(10, '0'));

    QByteArray out("XISF0100");
    const uint32_t len = static_cast<uint32_t>(xml.size());
    out.append(reinterpret_cast<const char*>(&len), 4);   // little-endian on every CI arch
    out.append(QByteArray(4, '\0'));
    out.append(xml);
    out.append(pixels);

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) return false;
    return f.write(out) == out.size();
}

core::FitsImage flatImage(int w, int h, float v)
{
    core::FitsImage img;
    img.width  = w;
    img.height = h;
    img.data.assign(static_cast<size_t>(w) * h, v);
    return img;
}

} // namespace

// ─── NAXIS ceiling (AUD-INPUT-1/2) ────────────────────────────────────────────

TEST_CASE("loadFits rejects NAXIS=4 at the header guard", "[regression][hostile][loaders]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("naxis4.fits");
    REQUIRE(writeRawFits(path, {4, 4, 2, 2}, 4LL * 4 * 2 * 2 * 4));

    auto r = core::loadFits(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("NAXIS=4")));

    auto h = core::loadFitsHdu(path, 1);
    REQUIRE_FALSE(h.has_value());
    CHECK(h.error().contains(QStringLiteral("NAXIS=4")));

    CHECK(core::scanImageHdus(path).isEmpty());
}

TEST_CASE("loadFits rejects a 100000 x 100000 header on a tiny file", "[regression][hostile][loaders]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("huge.fits");
    REQUIRE(writeRawFits(path, {100000, 100000}, 2880));   // header claims ~40 GB

    auto r = core::loadFits(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("too large")));
}

TEST_CASE("loadFits rejects a header larger than the file on disk", "[regression][hostile][loaders]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("lying.fits");
    REQUIRE(writeRawFits(path, {10000, 10000}, 2880));     // inside the ceiling, not in the file

    auto r = core::loadFits(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("exceeds")));
}

// ─── XISF ceiling (AUD-INPUT-9) ───────────────────────────────────────────────

TEST_CASE("loadImage reads a tiny valid XISF", "[regression][loaders][xisf]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("ok.xisf");
    REQUIRE(writeXisf(path, "2:2:1", QByteArray("\x01\x02\x03\x04", 4)));

    auto r = core::loadImage(path);
    REQUIRE(r.has_value());
    CHECK(r->width == 2);
    CHECK(r->height == 2);
    CHECK(r->data.size() == 4);
    CHECK(r->data[3] == 4.0f);
}

TEST_CASE("loadImage rejects XISF geometry above the FITS axis ceiling", "[regression][hostile][loaders][xisf]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("geom20001.xisf");
    REQUIRE(writeXisf(path, "20001:20001:1", QByteArray(16, '\0')));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("too large")));
}

TEST_CASE("loadImage rejects XISF geometry that does not fit in the file", "[regression][hostile][loaders][xisf]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("geom_lie.xisf");
    REQUIRE(writeXisf(path, "5000:5000:1", QByteArray(16, '\0')));

    auto r = core::loadImage(path);
    REQUIRE_FALSE(r.has_value());
    CHECK(r.error().contains(QStringLiteral("exceeds")));
}

// ─── Non-finite pixels in the centroid (AUD-MEM-2/3) ──────────────────────────

TEST_CASE("centroids refuse +Inf and NaN pixels instead of returning garbage", "[regression][hostile][centroid]")
{
    const float inf = std::numeric_limits<float>::infinity();
    const float nan = std::numeric_limits<float>::quiet_NaN();

    for (const float bad : {inf, nan}) {
        // Bad value at the peak (inside the box) ...
        core::FitsImage img = flatImage(40, 40, 100.0f);
        img.data[20 * 40 + 20] = bad;
        // ... and at the box border (poisons the background median).
        core::FitsImage img2 = flatImage(40, 40, 100.0f);
        img2.data[20 * 40 + 20] = 5000.0f;
        for (int x = 8; x <= 32; ++x) img2.data[8 * 40 + x] = bad;

        for (const core::FitsImage* im : {&img, &img2}) {
            const auto c = core::findCentroid(*im, 20.0, 20.0, 12);
            if (c) {
                CHECK(std::isfinite(c->x));
                CHECK(std::isfinite(c->y));
            }
            const auto p = core::findCentroidPsf(*im, 20.0, 20.0, 12);
            if (p) {
                CHECK(std::isfinite(p->x));
                CHECK(std::isfinite(p->y));
            }
            const auto e = core::findCentroidElliptical(*im, 20.0, 20.0, 12);
            if (e) {
                CHECK(std::isfinite(e->x));
                CHECK(std::isfinite(e->y));
            }
        }
        // +Inf must be refused outright by the moments centroid (explicit
        // isfinite guards). NaN pixels get zero weight (max(0, NaN) == 0), so
        // a result is allowed there as long as it is finite (checked above).
        if (std::isinf(bad)) {
            CHECK_FALSE(core::findCentroid(img, 20.0, 20.0, 12).has_value());
            CHECK_FALSE(core::findCentroid(img2, 20.0, 20.0, 12).has_value());
        }
    }
}
