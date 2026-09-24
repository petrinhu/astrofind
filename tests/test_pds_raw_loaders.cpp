// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// Items 21.1 (DSLR RAW via LibRaw) and 21.2 (NASA PDS3/PDS4). Every fixture
// is synthesized on disk here: PDS labels are plain text/XML, and the RAW
// case writes a minimal 32x32 RGGB DNG (TIFF + CFA tags) so the real LibRaw
// path runs in CI. Hostile cases are labels whose declared array is larger
// than the file (AUD-INPUT-8 lesson: never allocate from a header).

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include "core/ImageLoader.h"
#include "core/PdsLoader.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <vector>

namespace {

bool writeBytes(const QString& path, const QByteArray& bytes)
{
    QFile f(path);
    return f.open(QIODevice::WriteOnly) && f.write(bytes) == bytes.size();
}

void putBE16(QByteArray& b, std::int16_t v)
{
    const auto u = static_cast<std::uint16_t>(v);
    b.append(static_cast<char>(u >> 8)); b.append(static_cast<char>(u & 0xFF));
}
void putLE16(QByteArray& b, std::uint16_t u)
{
    b.append(static_cast<char>(u & 0xFF)); b.append(static_cast<char>(u >> 8));
}
void putLE32(QByteArray& b, std::uint32_t u)
{
    for (int i = 0; i < 4; ++i) b.append(static_cast<char>((u >> (8 * i)) & 0xFF));
}
void putLEFloat(QByteArray& b, float f)
{
    std::uint32_t u; std::memcpy(&u, &f, 4); putLE32(b, u);
}

/// PDS3 attached label padded to `recordBytes * labelRecords`.
QByteArray attachedLabel(const QString& body, int recordBytes, int labelRecords)
{
    QByteArray lbl = body.toLatin1();
    lbl.append("END\r\n");
    REQUIRE(lbl.size() <= recordBytes * labelRecords);
    lbl.append(QByteArray(recordBytes * labelRecords - lbl.size(), ' '));
    return lbl;
}

} // namespace

// ─── PDS3 ────────────────────────────────────────────────────────────────────

TEST_CASE("loadPds3 reads an attached-label MSB 16-bit image with scaling", "[loaders][pds]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("attached.img");

    const int W = 4, H = 3;
    const QString body =
        "PDS_VERSION_ID = PDS3\r\n"
        "RECORD_TYPE = FIXED_LENGTH\r\n"
        "RECORD_BYTES = 512\r\n"
        "LABEL_RECORDS = 2\r\n"
        "^IMAGE = 3\r\n"
        "TARGET_NAME = \"(433) EROS\"\r\n"
        "INSTRUMENT_NAME = \"TEST CAMERA\"\r\n"
        "START_TIME = 2002-03-15T10:00:00.000Z\r\n"
        "EXPOSURE_DURATION = 2000 <MS>\r\n"
        "/* a comment that must be ignored */\r\n"
        "OBJECT = IMAGE\r\n"
        "  LINES = 3\r\n"
        "  LINE_SAMPLES = 4\r\n"
        "  SAMPLE_TYPE = MSB_INTEGER\r\n"
        "  SAMPLE_BITS = 16\r\n"
        "  SCALING_FACTOR = 2.0\r\n"
        "  OFFSET = 10.0\r\n"
        "END_OBJECT = IMAGE\r\n";
    QByteArray file = attachedLabel(body, 512, 2);
    for (int i = 0; i < W * H; ++i) putBE16(file, static_cast<std::int16_t>(i - 5));
    REQUIRE(writeBytes(path, file));

    auto img = core::loadImage(path);
    REQUIRE(img.has_value());
    CHECK(img->width == W);
    CHECK(img->height == H);
    for (int i = 0; i < W * H; ++i)
        CHECK(img->data[static_cast<size_t>(i)] == Catch::Approx((i - 5) * 2.0 + 10.0));
    CHECK(img->objectName == "(433) EROS");
    CHECK(img->expTime == Catch::Approx(2.0));
    REQUIRE(img->dateObs.isValid());
    CHECK(img->dateObs.toUTC().toString(Qt::ISODate) == "2002-03-15T10:00:00Z");
    // JD at mid-exposure: 2002-03-15 10:00:01 UTC
    CHECK(img->jd == Catch::Approx(2452348.9166782).margin(1e-6));
}

TEST_CASE("loadPds3 reads a detached label from either file", "[loaders][pds]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString imgPath = dir.filePath("frame.img");
    const QString lblPath = dir.filePath("frame.lbl");

    QByteArray data;
    for (int i = 0; i < 6; ++i) putLE16(data, static_cast<std::uint16_t>(60000 + i));
    REQUIRE(writeBytes(imgPath, data));
    // Upper-case name in the label, lower-case file on disk (common in archives).
    REQUIRE(writeBytes(lblPath,
        "PDS_VERSION_ID = PDS3\n"
        "^IMAGE = \"FRAME.IMG\"\n"
        "OBJECT = IMAGE\n"
        "  LINES = 2\n"
        "  LINE_SAMPLES = 3\n"
        "  SAMPLE_TYPE = LSB_UNSIGNED_INTEGER\n"
        "  SAMPLE_BITS = 16\n"
        "END_OBJECT = IMAGE\n"
        "END\n"));

    for (const QString& p : {imgPath, lblPath}) {
        auto img = core::loadImage(p);
        REQUIRE(img.has_value());
        CHECK(img->width == 3);
        CHECK(img->height == 2);
        CHECK(img->data[0] == Catch::Approx(60000.0));
        CHECK(img->data[5] == Catch::Approx(60005.0));
    }
}

TEST_CASE("loadPds3 maps MISSING_CONSTANT to NaN and skips line prefixes", "[loaders][pds]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("real.img");
    const QString body =
        "PDS_VERSION_ID = PDS3\n"
        "RECORD_BYTES = 256\n"
        "LABEL_RECORDS = 2\n"
        "^IMAGE = 513 <BYTES>\n"
        "OBJECT = IMAGE\n"
        "  LINES = 2\n"
        "  LINE_SAMPLES = 2\n"
        "  LINE_PREFIX_BYTES = 4\n"
        "  SAMPLE_TYPE = PC_REAL\n"
        "  SAMPLE_BITS = 32\n"
        "  MISSING_CONSTANT = -9999.0\n"
        "END_OBJECT = IMAGE\n";
    QByteArray file = attachedLabel(body, 256, 2);
    const float vals[4] = {1.5f, -9999.0f, 3.25f, 4.0f};
    for (int y = 0; y < 2; ++y) {
        file.append("PFX!");                       // 4-byte line prefix
        for (int x = 0; x < 2; ++x) putLEFloat(file, vals[y * 2 + x]);
    }
    REQUIRE(writeBytes(path, file));

    auto img = core::loadPds3(path);
    REQUIRE(img.has_value());
    CHECK(img->data[0] == Catch::Approx(1.5));
    CHECK(std::isnan(img->data[1]));
    CHECK(img->data[2] == Catch::Approx(3.25));
    CHECK(img->data[3] == Catch::Approx(4.0));
}

TEST_CASE("loadPds3 rejects a label declaring more data than the file holds", "[loaders][pds][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("lying.img");
    const QString body =
        "PDS_VERSION_ID = PDS3\n"
        "RECORD_BYTES = 512\n"
        "LABEL_RECORDS = 1\n"
        "^IMAGE = 2\n"
        "OBJECT = IMAGE\n"
        "  LINES = 10000\n"
        "  LINE_SAMPLES = 10000\n"
        "  SAMPLE_TYPE = MSB_INTEGER\n"
        "  SAMPLE_BITS = 16\n"
        "END_OBJECT = IMAGE\n";
    QByteArray file = attachedLabel(body, 512, 1);
    file.append(QByteArray(64, '\0'));
    REQUIRE(writeBytes(path, file));

    CHECK_FALSE(core::loadPds3(path).has_value());
}

TEST_CASE("loadPds3 rejects dimensions above the shared loader ceiling", "[loaders][pds][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("huge.img");
    const QString body =
        "PDS_VERSION_ID = PDS3\n"
        "RECORD_BYTES = 512\n"
        "LABEL_RECORDS = 1\n"
        "OBJECT = IMAGE\n"
        "  LINES = 100000\n"
        "  LINE_SAMPLES = 100000\n"
        "  SAMPLE_BITS = 8\n"
        "END_OBJECT = IMAGE\n";
    REQUIRE(writeBytes(path, attachedLabel(body, 512, 1)));
    CHECK_FALSE(core::loadPds3(path).has_value());
}

TEST_CASE("loadPds3 refuses an img without any label", "[loaders][pds]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("nolabel.img");
    REQUIRE(writeBytes(path, QByteArray(1024, '\x42')));
    CHECK_FALSE(core::loadImage(path).has_value());
}

// ─── PDS4 ────────────────────────────────────────────────────────────────────

namespace {

QByteArray pds4Label(const QString& dataFile, const QString& dataType, int lines, int samples,
                     const QString& extraElement = {}, const QString& special = {})
{
    return QStringLiteral(
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<Product_Observational xmlns=\"http://pds.nasa.gov/pds4/pds/v1\">\n"
        "  <Identification_Area><logical_identifier>urn:nasa:pds:test:data:x</logical_identifier>"
        "<version_id>1.0</version_id></Identification_Area>\n"
        "  <Observation_Area>\n"
        "    <Time_Coordinates><start_date_time>2020-06-01T00:00:00Z</start_date_time>"
        "<stop_date_time>2020-06-01T00:00:30Z</stop_date_time></Time_Coordinates>\n"
        "    <Target_Identification><name>(101955) Bennu</name></Target_Identification>\n"
        "  </Observation_Area>\n"
        "  <File_Area_Observational>\n"
        "    <File><file_name>%1</file_name></File>\n"
        "    <Array_2D_Image>\n"
        "      <offset unit=\"byte\">16</offset>\n"
        "      <axes>2</axes>\n"
        "      <axis_index_order>Last Index Fastest</axis_index_order>\n"
        "      <Element_Array><data_type>%2</data_type>%5</Element_Array>\n"
        "      <Axis_Array><axis_name>Line</axis_name><elements>%3</elements>"
        "<sequence_number>1</sequence_number></Axis_Array>\n"
        "      <Axis_Array><axis_name>Sample</axis_name><elements>%4</elements>"
        "<sequence_number>2</sequence_number></Axis_Array>\n"
        "      %6\n"
        "    </Array_2D_Image>\n"
        "  </File_Area_Observational>\n"
        "</Product_Observational>\n")
        .arg(dataFile, dataType).arg(lines).arg(samples).arg(extraElement, special)
        .toUtf8();
}

} // namespace

TEST_CASE("loadPds4 reads a SignedLSB2 array with scaling and missing constant", "[loaders][pds4]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString lbl = dir.filePath("bennu.xml");
    QByteArray data(16, '\0');                         // 16-byte header before the array
    const std::int16_t vals[6] = {-3, 0, 7, -32768, 100, 5};
    for (std::int16_t v : vals) putLE16(data, static_cast<std::uint16_t>(v));
    REQUIRE(writeBytes(dir.filePath("bennu.img"), data));
    REQUIRE(writeBytes(lbl, pds4Label("bennu.img", "SignedLSB2", 2, 3,
        "<scaling_factor>0.5</scaling_factor><value_offset>1</value_offset>",
        "<Special_Constants><missing_constant>-32768</missing_constant></Special_Constants>")));

    REQUIRE(core::isPds4Label(lbl));
    auto img = core::loadImage(lbl);
    REQUIRE(img.has_value());
    CHECK(img->width == 3);
    CHECK(img->height == 2);
    CHECK(img->data[0] == Catch::Approx(-0.5));
    CHECK(img->data[2] == Catch::Approx(4.5));
    CHECK(std::isnan(img->data[3]));
    CHECK(img->data[4] == Catch::Approx(51.0));
    CHECK(img->objectName == "(101955) Bennu");
    CHECK(img->expTime == Catch::Approx(30.0));
}

TEST_CASE("loadPds4 rejects an array larger than its data file", "[loaders][pds4][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString lbl = dir.filePath("lying.xml");
    REQUIRE(writeBytes(dir.filePath("lying.img"), QByteArray(64, '\0')));
    REQUIRE(writeBytes(lbl, pds4Label("lying.img", "IEEE754MSBSingle", 5000, 5000)));
    CHECK_FALSE(core::loadPds4(lbl).has_value());
}

TEST_CASE("loadImage refuses an XML file that is not a PDS4 label", "[loaders][pds4]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("other.xml");
    REQUIRE(writeBytes(path, "<?xml version=\"1.0\"?><root><a>1</a></root>"));
    CHECK_FALSE(core::isPds4Label(path));
    CHECK_FALSE(core::loadImage(path).has_value());
}

// ─── DSLR RAW ────────────────────────────────────────────────────────────────

namespace {

/// Minimal little-endian DNG: one 32x32 16-bit RGGB CFA strip (R=1000,
/// G=2000, B=3000), Make/Model, DateTime and a 1/2 s ExposureTime.
/// 32 px because dcraw/LibRaw refuse anything under 22 px per side.
QByteArray makeTinyDng()
{
    constexpr int W = 32, H = 32;
    struct Entry { std::uint16_t tag, type; std::uint32_t count; QByteArray value; };
    auto shortVal = [](std::uint16_t v) { QByteArray b; putLE16(b, v); return b; };
    auto longVal  = [](std::uint32_t v) { QByteArray b; putLE32(b, v); return b; };
    auto ascii    = [](const char* s)   { return QByteArray(s, static_cast<int>(std::strlen(s)) + 1); };

    QByteArray cfa;
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            putLE16(cfa, (y % 2 == 0) ? (x % 2 == 0 ? 1000 : 2000)
                                      : (x % 2 == 0 ? 2000 : 3000));
    QByteArray rational; putLE32(rational, 1); putLE32(rational, 2);
    QByteArray repeat; putLE16(repeat, 2); putLE16(repeat, 2);

    std::vector<Entry> e = {
        {254,   4, 1, longVal(0)},
        {256,   4, 1, longVal(W)},
        {257,   4, 1, longVal(H)},
        {258,   3, 1, shortVal(16)},
        {259,   3, 1, shortVal(1)},
        {262,   3, 1, shortVal(32803)},           // CFA
        {271,   2, 0, ascii("AstroFind")},
        {272,   2, 0, ascii("Synthetic")},
        {273,   4, 1, QByteArray()},              // StripOffsets: patched below
        {277,   3, 1, shortVal(1)},
        {278,   4, 1, longVal(H)},
        {279,   4, 1, longVal(static_cast<std::uint32_t>(cfa.size()))},
        {284,   3, 1, shortVal(1)},
        {306,   2, 0, ascii("2024:01:02 03:04:05")},
        {33421, 3, 2, repeat},
        {33422, 1, 4, QByteArray("\x00\x01\x01\x02", 4)},
        {33434, 5, 1, rational},                  // ExposureTime 1/2 s
        {50706, 1, 4, QByteArray("\x01\x04\x00\x00", 4)},
        {50708, 2, 0, ascii("AstroFind Synthetic")},
        {50717, 4, 1, longVal(65535)},
    };
    for (auto& x : e) if (x.count == 0) x.count = static_cast<std::uint32_t>(x.value.size());

    const std::uint32_t ifdOff  = 8;
    const std::uint32_t ifdSize = 2 + 12 * static_cast<std::uint32_t>(e.size()) + 4;
    std::uint32_t extraOff = ifdOff + ifdSize;
    QByteArray extra;
    // Strip data goes after all out-of-line values; compute their size first.
    std::uint32_t extraLen = 0;
    for (const auto& x : e)
        if (x.tag != 273 && x.value.size() > 4)
            extraLen += static_cast<std::uint32_t>((x.value.size() + 1) / 2 * 2);
    const std::uint32_t stripOff = extraOff + extraLen;
    for (auto& x : e) if (x.tag == 273) x.value = longVal(stripOff);

    QByteArray out("II*\0", 4);
    putLE32(out, ifdOff);
    putLE16(out, static_cast<std::uint16_t>(e.size()));
    for (const auto& x : e) {
        putLE16(out, x.tag); putLE16(out, x.type); putLE32(out, x.count);
        if (x.value.size() <= 4) {
            QByteArray v = x.value; v.append(QByteArray(4 - v.size(), '\0'));
            out.append(v);
        } else {
            putLE32(out, extraOff + static_cast<std::uint32_t>(extra.size()));
            extra.append(x.value);
            if (extra.size() % 2) extra.append('\0');
        }
    }
    putLE32(out, 0);                              // no next IFD
    out.append(extra);
    out.append(cfa);
    return out;
}

} // namespace

TEST_CASE("DSLR RAW extensions are routed to the RAW loader", "[loaders][raw]")
{
    for (const char* ext : {"cr2", "CR3", "nef", "arw", "dng", "raf", "orf", "rw2", "pef"})
        CHECK(core::isDslrRawExtension(QString::fromLatin1(ext)));
    CHECK_FALSE(core::isDslrRawExtension("fits"));
    CHECK_FALSE(core::isDslrRawExtension("img"));
}

TEST_CASE("loadDslrRaw rejects a corrupt RAW file", "[loaders][raw][hostile]")
{
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("garbage.cr2");
    REQUIRE(writeBytes(path, QByteArray(4096, '\x5A')));
    auto img = core::loadImage(path);
    REQUIRE_FALSE(img.has_value());
    if (!core::hasDslrRawSupport())
        CHECK(img.error().contains("LibRaw"));
}

TEST_CASE("loadDslrRaw decodes a minimal RGGB DNG", "[loaders][raw]")
{
    if (!core::hasDslrRawSupport())
        SKIP("built without LibRaw");
    QTemporaryDir dir;
    REQUIRE(dir.isValid());
    const QString path = dir.filePath("tiny.dng");
    REQUIRE(writeBytes(path, makeTinyDng()));

    auto img = core::loadDslrRaw(path);
    REQUIRE(img.has_value());
    CHECK(img->width == 32);
    CHECK(img->height == 32);
    CHECK(img->isColor);
    REQUIRE(img->data.size() == 32u * 32u);
    // Superpixel luminance (R + 2G + B) / 4 = (1000 + 4000 + 3000) / 4.
    CHECK(img->data[0] == Catch::Approx(2000.0));
    CHECK(img->data[33] == Catch::Approx(2000.0));
    CHECK(img->dataR[0] == Catch::Approx(1000.0));
    CHECK(img->dataB[0] == Catch::Approx(3000.0));
    CHECK(img->expTime == Catch::Approx(0.5));
    CHECK(img->origin.contains("AstroFind"));
    CHECK(img->dateObsAmbiguous);
    CHECK(img->dateObs.toString("yyyy-MM-dd HH:mm:ss") == "2024-01-02 03:04:05");
}
