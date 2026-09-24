// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#include "ImageLoader.h"
#include "FitsImage.h"
#include "PdsLoader.h"

#ifdef ASTROFIND_HAS_LIBRAW
#include <libraw/libraw.h>
#endif

#include <QImage>
#include <QImageReader>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QTimeZone>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstring>
#include <cmath>
#include <exception>
#include <memory>

namespace core {
namespace {

// ─── Qt image loader (TIFF / PNG / BMP / JPEG) ───────────────────────────────

std::expected<FitsImage, QString> loadQImage(const QString& filePath)
{
    // AUD-INPUT-gaps: same axis/pixel ceiling as every other loader. The size
    // the header DECLARES is checked first (QImageReader::size() parses only
    // the header), so a PNG/TIFF claiming e.g. 100000x100000 is refused before
    // Qt decodes or allocates anything. The decoded size is checked again
    // below, for formats whose handler cannot report a size up front. No
    // file-size cross-check here: PNG/TIFF/JPEG are compressed.
    QImageReader reader(filePath);
    const QSize declared = reader.size();
    if (declared.isValid()) {
        QString dimErr;
        if (!validateDecodedDims(declared.width(), declared.height(), filePath, dimErr))
            return std::unexpected(dimErr);
    }
    const QImage qi = reader.read();
    if (qi.isNull())
        return std::unexpected(QObject::tr("Cannot load image (unsupported or corrupt): %1")
            .arg(filePath));
    {
        QString dimErr;
        if (!validateDecodedDims(qi.width(), qi.height(), filePath, dimErr))
            return std::unexpected(dimErr);
    }

    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();
    img.width    = qi.width();
    img.height   = qi.height();
    const size_t n = static_cast<size_t>(img.width) * img.height;

    const bool isGray16 = (qi.format() == QImage::Format_Grayscale16);
    const bool isGray   = isGray16 || qi.isGrayscale()
                          || qi.format() == QImage::Format_Grayscale8;
    img.isColor = !isGray;

    // No exception may escape into the Qt slot that calls us (AUD-INPUT-2):
    // even under the ceiling a colour frame needs 4 float planes.
    try {
        if (isGray16) {
            // 16-bit grayscale — direct scanline access (avoids per-pixel calls)
            img.data.resize(n);
            for (int y = 0; y < img.height; ++y) {
                const quint16* row = reinterpret_cast<const quint16*>(qi.constScanLine(y));
                for (int x = 0; x < img.width; ++x)
                    img.data[static_cast<size_t>(y) * img.width + x] =
                        static_cast<float>(row[x]);
            }
        } else if (isGray) {
            const QImage g8 = qi.convertToFormat(QImage::Format_Grayscale8);
            img.data.resize(n);
            for (int y = 0; y < img.height; ++y) {
                const uchar* row = g8.constScanLine(y);
                for (int x = 0; x < img.width; ++x)
                    img.data[static_cast<size_t>(y) * img.width + x] =
                        static_cast<float>(row[x]);
            }
        } else {
            // Convert to packed RGB32 for predictable layout
            const QImage rgb = qi.convertToFormat(QImage::Format_RGB32);
            img.dataR.resize(n); img.dataG.resize(n); img.dataB.resize(n);
            img.data.resize(n);
            for (int y = 0; y < img.height; ++y) {
                const QRgb* row = reinterpret_cast<const QRgb*>(rgb.constScanLine(y));
                for (int x = 0; x < img.width; ++x) {
                    const size_t i = static_cast<size_t>(y) * img.width + x;
                    img.dataR[i] = static_cast<float>(qRed(row[x]));
                    img.dataG[i] = static_cast<float>(qGreen(row[x]));
                    img.dataB[i] = static_cast<float>(qBlue(row[x]));
                    img.data[i]  = 0.2126f * img.dataR[i]
                                  + 0.7152f * img.dataG[i]
                                  + 0.0722f * img.dataB[i];
                }
            }
        }
    } catch (const std::exception& e) {
        return std::unexpected(QObject::tr("Out of memory loading image '%1': %2")
                                   .arg(filePath, QString::fromLocal8Bit(e.what())));
    }

    computeAutoStretch(img);
    spdlog::info("Loaded {} image: {}  {}x{}  {}",
        QFileInfo(filePath).suffix().toStdString(),
        img.fileName.toStdString(), img.width, img.height,
        img.isColor ? "RGB" : "grayscale");
    return img;
}

// ─── SER loader ──────────────────────────────────────────────────────────────

#pragma pack(push, 1)
struct SerHeader {
    char     fileID[14];      ///< Typically "LUCAM-RECORDER"
    uint32_t luID;
    uint32_t colorID;
    uint32_t littleEndian;    ///< 0 = big-endian pixel data, non-zero = little-endian
    uint32_t imageWidth;
    uint32_t imageHeight;
    uint32_t pixelDepth;      ///< Bits per channel (8 or 16)
    uint32_t frameCount;
    char     observer[40];
    char     instrument[40];
    char     telescope[40];
    int64_t  dateTime;
    int64_t  dateTimeUTC;
};
#pragma pack(pop)

static_assert(sizeof(SerHeader) == 178, "SerHeader must be 178 bytes");

// AUD-INPUT-3: imageWidth/imageHeight are attacker-controlled uint32_t read
// directly from the binary header. A value with the high bit set (e.g.
// 0xFFFFFFFF or 0x80000010) previously passed the `== 0` guard below and was
// then narrowed via static_cast<int>, producing a negative width/height that
// silently became a huge size_t once multiplied into a pixel count — feeding
// std::vector::resize() a request beyond max_size() and crashing the process
// via an uncaught std::length_error. Mirror FitsImage.cpp's kMaxImageDim
// ceiling (20000 px/axis) here and validate strictly BEFORE narrowing.
constexpr uint32_t kMaxSerDim = 20000;

enum SerColorID : uint32_t {
    SER_MONO       = 0,
    SER_BAYER_RGGB = 8,
    SER_BAYER_GRBG = 9,
    SER_BAYER_GBRG = 10,
    SER_BAYER_BGGR = 11,
    SER_RGB        = 100,
    SER_BGR        = 101,
};

/// Simple 2×2 average Bayer debayer. pattern: 0=RGGB, 1=GRBG, 2=GBRG, 3=BGGR.
static void demosaicBayer(const std::vector<float>& raw, int W, int H, int pattern,
                           std::vector<float>& R, std::vector<float>& G, std::vector<float>& B)
{
    // Row/col offsets for [R, G1, G2, B] within each 2×2 block
    int rr, rc, g1r, g1c, g2r, g2c, br, bc;
    switch (pattern) {
        default:
        case 0: rr=0; rc=0; g1r=0; g1c=1; g2r=1; g2c=0; br=1; bc=1; break; // RGGB
        case 1: rr=0; rc=1; g1r=0; g1c=0; g2r=1; g2c=1; br=1; bc=0; break; // GRBG
        case 2: rr=1; rc=0; g1r=0; g1c=0; g2r=1; g2c=1; br=0; bc=1; break; // GBRG
        case 3: rr=1; rc=1; g1r=0; g1c=1; g2r=1; g2c=0; br=0; bc=0; break; // BGGR
    }
    const size_t n = static_cast<size_t>(W) * H;
    R.assign(n, 0.f); G.assign(n, 0.f); B.assign(n, 0.f);
    for (int y = 0; y < H - 1; y += 2) {
        for (int x = 0; x < W - 1; x += 2) {
            const float rv  = raw[(y+rr)*W + (x+rc)];
            const float g1v = raw[(y+g1r)*W + (x+g1c)];
            const float g2v = raw[(y+g2r)*W + (x+g2c)];
            const float bv  = raw[(y+br)*W + (x+bc)];
            const float gv  = (g1v + g2v) * 0.5f;
            for (int dy = 0; dy < 2; ++dy)
                for (int dx = 0; dx < 2; ++dx) {
                    const size_t i = static_cast<size_t>(y+dy)*W + (x+dx);
                    R[i] = rv; G[i] = gv; B[i] = bv;
                }
        }
    }
}

std::expected<FitsImage, QString> loadSer(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return std::unexpected(QObject::tr("Cannot open SER file: %1").arg(filePath));
    if (f.size() < static_cast<qint64>(sizeof(SerHeader)))
        return std::unexpected(QObject::tr("File too small to be a valid SER file: %1").arg(filePath));

    SerHeader hdr{};
    f.read(reinterpret_cast<char*>(&hdr), sizeof(SerHeader));

    // AUD-INPUT-6: SerHeader is #pragma pack(1) — its uint32_t members are not
    // naturally aligned in memory. Binding a reference to them (which is what
    // fmt/spdlog's variadic templates and, on some Qt builds, QString::arg()
    // do internally) is UB per the C++ object model (the reference type
    // implies alignof(uint32_t)==4) and aborts under UBSan halt-on-error.
    // Copy each field to a properly-aligned local BEFORE it is ever passed to
    // logging/formatting; all later uses of these four fields in this
    // function go through the locals below, never through `hdr.` directly.
    const uint32_t serImageWidth  = hdr.imageWidth;
    const uint32_t serImageHeight = hdr.imageHeight;
    const uint32_t serPixelDepth  = hdr.pixelDepth;
    const uint32_t serFrameCount  = hdr.frameCount;

    if (serImageWidth == 0 || serImageHeight == 0 || serFrameCount == 0)
        return std::unexpected(QObject::tr("Invalid SER header (zero dimensions): %1").arg(filePath));
    // Validate the raw uint32_t values BEFORE narrowing to int: a header with
    // the sign bit set (e.g. 0xFFFFFFFF/0x80000010) must be rejected here,
    // not after static_cast<int> turns it into a negative width/height.
    if (serImageWidth > kMaxSerDim || serImageHeight > kMaxSerDim)
        return std::unexpected(
            QObject::tr("SER dimensions out of range (%1x%2, max %3): %4")
                .arg(serImageWidth).arg(serImageHeight).arg(kMaxSerDim).arg(filePath));
    if (serPixelDepth != 8 && serPixelDepth != 16)
        return std::unexpected(QObject::tr("Unsupported SER pixel depth %1 in: %2")
            .arg(serPixelDepth).arg(filePath));

    FitsImage img;
    img.filePath  = filePath;
    img.fileName  = QFileInfo(filePath).fileName();
    img.width     = static_cast<int>(serImageWidth);
    img.height    = static_cast<int>(serImageHeight);
    // The 40-byte text fields are NUL- or space-padded. trimmed() does not
    // strip '\0', so stop at the first NUL (found by AUD-TEST-6: the padding
    // otherwise ended up inside img.observer/telescope).
    auto serText = [](const char* p, size_t n) {
        const char* end = std::find(p, p + n, '\0');
        return QString::fromLatin1(p, static_cast<qsizetype>(end - p)).trimmed();
    };
    img.observer  = serText(hdr.observer,  sizeof(hdr.observer));
    img.telescope = serText(hdr.telescope, sizeof(hdr.telescope));

    const bool isRgb   = (hdr.colorID == SER_RGB || hdr.colorID == SER_BGR);
    const bool isBayer = (hdr.colorID >= SER_BAYER_RGGB && hdr.colorID <= SER_BAYER_BGGR);
    const int  channels = isRgb ? 3 : 1;
    const int  bps      = static_cast<int>(serPixelDepth) / 8;   // bytes per sample
    const qint64 frameBytes = static_cast<qint64>(img.width) * img.height * channels * bps;

    QByteArray raw = f.read(frameBytes);
    if (raw.size() < frameBytes)
        return std::unexpected(
            QObject::tr("SER file truncated (expected %1 bytes for frame 1): %2")
                .arg(frameBytes).arg(filePath));

    // Pixel data endianness: big-endian when littleEndian==0 on a LE machine
    const bool needSwap = (serPixelDepth == 16) && (hdr.littleEndian == 0);

    const size_t n = static_cast<size_t>(img.width) * img.height;

    auto readSamples = [&](const char* src, size_t count) -> std::vector<float> {
        std::vector<float> out(count);
        if (bps == 1) {
            const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
            for (size_t i = 0; i < count; ++i) out[i] = static_cast<float>(p[i]);
        } else {
            const uint16_t* p = reinterpret_cast<const uint16_t*>(src);
            for (size_t i = 0; i < count; ++i) {
                uint16_t v = p[i];
                if (needSwap) v = static_cast<uint16_t>((v >> 8) | (v << 8));
                out[i] = static_cast<float>(v);
            }
        }
        return out;
    };

    if (isRgb) {
        img.isColor = true;
        // SER RGB/BGR: interleaved samples (R0G0B0 R1G1B1 ...)
        img.dataR.resize(n); img.dataG.resize(n); img.dataB.resize(n); img.data.resize(n);
        const bool isBGR = (hdr.colorID == SER_BGR);
        if (bps == 1) {
            const uint8_t* p = reinterpret_cast<const uint8_t*>(raw.constData());
            for (size_t i = 0; i < n; ++i) {
                img.dataR[i] = static_cast<float>(isBGR ? p[i*3+2] : p[i*3+0]);
                img.dataG[i] = static_cast<float>(p[i*3+1]);
                img.dataB[i] = static_cast<float>(isBGR ? p[i*3+0] : p[i*3+2]);
            }
        } else {
            const uint16_t* p = reinterpret_cast<const uint16_t*>(raw.constData());
            auto sw = [needSwap](uint16_t v) -> float {
                if (needSwap) v = static_cast<uint16_t>((v >> 8) | (v << 8));
                return static_cast<float>(v);
            };
            for (size_t i = 0; i < n; ++i) {
                img.dataR[i] = sw(isBGR ? p[i*3+2] : p[i*3+0]);
                img.dataG[i] = sw(p[i*3+1]);
                img.dataB[i] = sw(isBGR ? p[i*3+0] : p[i*3+2]);
            }
        }
        for (size_t i = 0; i < n; ++i)
            img.data[i] = 0.2126f*img.dataR[i] + 0.7152f*img.dataG[i] + 0.0722f*img.dataB[i];

    } else if (isBayer) {
        const std::vector<float> rawMono = readSamples(raw.constData(), n);
        const int bayerPat = static_cast<int>(hdr.colorID) - static_cast<int>(SER_BAYER_RGGB);
        demosaicBayer(rawMono, img.width, img.height, bayerPat,
                      img.dataR, img.dataG, img.dataB);
        img.data    = rawMono;   // keep raw mono for astrometry
        img.isColor = true;

    } else {
        img.data = readSamples(raw.constData(), n);
    }

    if (serFrameCount > 1)
        spdlog::warn("SER file has {} frames — only frame 1 loaded from: {}",
            serFrameCount, img.fileName.toStdString());

    computeAutoStretch(img);
    spdlog::info("Loaded SER: {}  {}x{}  {}  frames={}",
        img.fileName.toStdString(), img.width, img.height,
        img.isColor ? "color" : "mono", serFrameCount);
    return img;
}

// ─── XISF loader ─────────────────────────────────────────────────────────────

std::expected<FitsImage, QString> loadXisf(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly))
        return std::unexpected(QObject::tr("Cannot open XISF file: %1").arg(filePath));
    if (f.size() < 16)
        return std::unexpected(QObject::tr("File too small to be XISF: %1").arg(filePath));

    // ── Signature: "XISF0100" + XML length (4 bytes LE) + reserved (4 bytes) ─
    char sig[8];
    f.read(sig, 8);
    if (std::memcmp(sig, "XISF0100", 8) != 0)
        return std::unexpected(QObject::tr("Not an XISF 1.0 file: %1").arg(filePath));

    uint32_t xmlLen = 0;
    f.read(reinterpret_cast<char*>(&xmlLen), 4);
    f.seek(16);   // skip reserved 4 bytes (already read sig+xmlLen = 12, so seek to 16)
    if (xmlLen == 0 || xmlLen > 64 * 1024 * 1024)
        return std::unexpected(QObject::tr("Invalid XISF XML header length in: %1").arg(filePath));

    const QByteArray xmlData = f.read(static_cast<qint64>(xmlLen));
    if (xmlData.size() < static_cast<qint64>(xmlLen))
        return std::unexpected(QObject::tr("XISF XML header truncated: %1").arg(filePath));

    // ── Parse XML ─────────────────────────────────────────────────────────────
    QDomDocument doc;
    {
        QString parseErr; int errLine = 0;
        if (!doc.setContent(xmlData, &parseErr, &errLine))
            return std::unexpected(QObject::tr("XISF XML parse error in %1 (line %2): %3")
                .arg(filePath).arg(errLine).arg(parseErr));
    }

    const QDomNodeList imageNodes =
        doc.documentElement().elementsByTagName(QStringLiteral("Image"));
    if (imageNodes.isEmpty())
        return std::unexpected(QObject::tr("No <Image> element in XISF file: %1").arg(filePath));

    const QDomElement img_el = imageNodes.at(0).toElement();

    // ── geometry / sampleFormat / location ───────────────────────────────────
    const QString geom = img_el.attribute(QStringLiteral("geometry"));
    const QString fmt  = img_el.attribute(QStringLiteral("sampleFormat"), QStringLiteral("UInt16"));
    const QString loc  = img_el.attribute(QStringLiteral("location"));

    const QStringList gp = geom.split(':');
    if (gp.size() < 2)
        return std::unexpected(QObject::tr("XISF: invalid geometry '%1' in: %2")
            .arg(geom, filePath));
    // Parse as long so an oversized axis is not silently turned into 0 by toInt().
    const long wL  = gp[0].toLong();
    const long hL  = gp[1].toLong();
    const long chL = gp.size() >= 3 ? gp[2].toLong() : 1;
    if (wL <= 0 || hL <= 0 || chL <= 0 || chL > 3)
        return std::unexpected(QObject::tr("XISF: invalid image dimensions %1 in: %2")
            .arg(geom, filePath));
    // AUD-INPUT-9: same ceiling and file-size cross-check as every other loader,
    // BEFORE any allocation.
    {
        QString dimErr;
        if (!validateLoaderDims(wL, hL, chL, filePath, dimErr))
            return std::unexpected(dimErr);
    }
    const int W  = static_cast<int>(wL);
    const int H  = static_cast<int>(hL);
    const int Ch = static_cast<int>(chL);

    int  bps     = 2;
    bool isfloat = false;
    if      (fmt == "UInt8")   { bps = 1; isfloat = false; }
    else if (fmt == "UInt16")  { bps = 2; isfloat = false; }
    else if (fmt == "UInt32")  { bps = 4; isfloat = false; }
    else if (fmt == "Float32") { bps = 4; isfloat = true;  }
    else if (fmt == "Float64") { bps = 8; isfloat = true;  }
    else return std::unexpected(QObject::tr("XISF: unsupported sampleFormat '%1' in: %2")
            .arg(fmt, filePath));
    (void)isfloat;

    const qint64 planeBytes = static_cast<qint64>(W) * H * bps;
    const qint64 totalBytes = planeBytes * Ch;

    QByteArray pixelRaw;
    if (loc.startsWith(QStringLiteral("attachment:"))) {
        const QStringList lp = loc.split(':');
        if (lp.size() < 3)
            return std::unexpected(QObject::tr("XISF: malformed location '%1' in: %2")
                .arg(loc, filePath));
        const qint64 offset = lp[1].toLongLong();
        if (!f.seek(offset))
            return std::unexpected(QObject::tr("XISF: cannot seek to offset %1 in: %2")
                .arg(offset).arg(filePath));
        pixelRaw = f.read(totalBytes);
    } else if (loc.startsWith(QStringLiteral("embedded"))) {
        const QDomElement dataEl = img_el.firstChildElement(QStringLiteral("Data"));
        if (dataEl.isNull())
            return std::unexpected(QObject::tr("XISF: missing <Data> element in: %1").arg(filePath));
        pixelRaw = QByteArray::fromBase64(dataEl.text().toLatin1());
    } else {
        return std::unexpected(QObject::tr("XISF: unsupported location type '%1' in: %2")
            .arg(loc, filePath));
    }

    if (pixelRaw.size() < totalBytes)
        return std::unexpected(QObject::tr("XISF: pixel data truncated in: %1").arg(filePath));

    // ── Convert samples to float ──────────────────────────────────────────────
    const size_t n = static_cast<size_t>(W) * H;
    auto toFloat = [&](const char* src, size_t count) -> std::vector<float> {
        std::vector<float> out(count);
        if      (fmt == "UInt8")  { const uint8_t*  p = reinterpret_cast<const uint8_t* >(src); for (size_t i=0;i<count;++i) out[i]=static_cast<float>(p[i]); }
        else if (fmt == "UInt16") { const uint16_t* p = reinterpret_cast<const uint16_t*>(src); for (size_t i=0;i<count;++i) out[i]=static_cast<float>(p[i]); }
        else if (fmt == "UInt32") { const uint32_t* p = reinterpret_cast<const uint32_t*>(src); for (size_t i=0;i<count;++i) out[i]=static_cast<float>(p[i]); }
        else if (fmt == "Float32") { std::memcpy(out.data(), src, count * 4); }
        else if (fmt == "Float64") { double tmp; for (size_t i=0;i<count;++i) { std::memcpy(&tmp, src + i*8, 8); out[i]=static_cast<float>(tmp); } }
        return out;
    };

    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();
    img.width    = W;
    img.height   = H;
    img.isColor  = (Ch == 3);

    const char* raw = pixelRaw.constData();
    if (Ch == 1) {
        img.data = toFloat(raw, n);
    } else {
        // XISF planar storage: all-R, then all-G, then all-B
        img.dataR = toFloat(raw,                    n);
        img.dataG = toFloat(raw + planeBytes,       n);
        img.dataB = toFloat(raw + planeBytes * 2,   n);
        img.data.resize(n);
        for (size_t i = 0; i < n; ++i)
            img.data[i] = 0.2126f*img.dataR[i] + 0.7152f*img.dataG[i] + 0.0722f*img.dataB[i];
    }

    // ── Extract FITS keywords from <FITSKeyword> children ────────────────────
    const QDomNodeList kwds = img_el.elementsByTagName(QStringLiteral("FITSKeyword"));
    double mjdObs = std::numeric_limits<double>::quiet_NaN();
    for (int i = 0; i < kwds.size(); ++i) {
        const QDomElement kw = kwds.at(i).toElement();
        const QString kn = kw.attribute(QStringLiteral("name")).trimmed().toUpper();
        const QString kv = kw.attribute(QStringLiteral("value")).trimmed().remove('\'').trimmed();
        if      (kn == "OBJECT")   img.objectName = kv;
        else if (kn == "EXPTIME")  img.expTime    = kv.toDouble();
        else if (kn == "GAIN")     img.gain       = kv.toDouble();
        else if (kn == "FILTER")   img.filter     = kv;
        else if (kn == "TELESCOP") img.telescope  = kv;
        else if (kn == "OBSERVER") img.observer   = kv;
        else if (kn == "JD")       img.jd         = kv.toDouble();
        else if (kn == "RA")       img.ra         = kv.toDouble();
        else if (kn == "DEC")      img.dec        = kv.toDouble();
        else if (kn == "DATE-OBS") {
            img.dateObs = QDateTime::fromString(kv, Qt::ISODate);
            if (!img.dateObs.isValid())
                img.dateObs = QDateTime::fromString(kv, "yyyy-MM-dd'T'HH:mm:ss.zzz");
            if (img.dateObs.isValid() && img.dateObs.timeSpec() == Qt::LocalTime)
                img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(), QTimeZone(0));
        }
        else if (kn == "MJD-OBS")  mjdObs         = kv.toDouble();
    }
    // AUD-CORR-13: same mid-exposure JD rule as FITS (MJD-OBS, else DATE-OBS
    // with milliseconds). XISF used to leave jd = 0 unless a JD keyword existed.
    if (img.jd == 0.0)
        img.jd = midExposureJd(mjdObs, img.dateObs, img.expTime, img.fileName);

    computeAutoStretch(img);
    spdlog::info("Loaded XISF: {}  {}x{}×{}  fmt={}",
        img.fileName.toStdString(), W, H, Ch, fmt.toStdString());
    return img;
}


// ─── DSLR RAW loader (item 21.1, LibRaw) ─────────────────────────────────────
//
// Astrometry wants linear, unprocessed data: the CFA mosaic straight from the
// sensor (no gamma, no white balance, no black subtraction — same convention
// as a raw FITS). The luminance plane used for detection/centroiding is the
// 2x2 superpixel mean (R + 2G + B)/4 of each Bayer block, which removes the
// colour checkerboard a plain CFA frame would feed into star detection (the
// approach recommended by Siril and the BAA DSLR astrometry guide).
// Non-Bayer sensors (Fuji X-Trans, Foveon, linear DNG) go through LibRaw's own
// linear 16-bit processing instead.

#ifdef ASTROFIND_HAS_LIBRAW

/// Bayer pattern index for demosaicBayer (0=RGGB 1=GRBG 2=GBRG 3=BGGR) from the
/// colours of the top-left 2x2 block of the visible area, or -1 if not a plain
/// RGB Bayer mosaic.
int bayerPatternOf(LibRaw& rp)
{
    const char* cdesc = rp.imgdata.idata.cdesc;
    auto name = [&](int r, int c) -> char {
        const int idx = rp.COLOR(r, c);
        return (idx >= 0 && idx < 4) ? cdesc[idx] : '?';
    };
    char pat[5] = {name(0, 0), name(0, 1), name(1, 0), name(1, 1), 0};
    const QByteArray p(pat);
    if (p == "RGGB") return 0;
    if (p == "GRBG") return 1;
    if (p == "GBRG") return 2;
    if (p == "BGGR") return 3;
    return -1;
}

void fillRawMetadata(LibRaw& rp, FitsImage& img)
{
    const auto& other = rp.imgdata.other;
    img.expTime    = other.shutter > 0.0f ? static_cast<double>(other.shutter) : 0.0;
    img.origin     = QStringLiteral("%1 %2").arg(QString::fromLatin1(rp.imgdata.idata.make),
                                                 QString::fromLatin1(rp.imgdata.idata.model)).trimmed();
    img.saturation = rp.imgdata.color.maximum > 0 ? static_cast<double>(rp.imgdata.color.maximum)
                                                  : img.saturation;
    if (other.timestamp > 0) {
        // EXIF DateTimeOriginal is the camera's wall clock (usually local time,
        // time zone unknown). LibRaw turns it into time_t with mktime(), so the
        // local-time view recovers the wall clock; it is flagged ambiguous so the
        // UI warns before the time reaches an MPC report.
        const QDateTime wall = QDateTime::fromSecsSinceEpoch(static_cast<qint64>(other.timestamp));
        img.dateObs = QDateTime(wall.date(), wall.time(), QTimeZone(0));
        img.dateObsAmbiguous = true;
        const QDateTime j2000(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
        img.jd = 2451545.0 + static_cast<double>(j2000.secsTo(img.dateObs)) / 86400.0
                 + img.expTime / 172800.0;
    }
}

std::expected<FitsImage, QString> loadDslrRawImpl(const QString& filePath)
{
    // LibRaw is large (hundreds of KB): never on the stack.
    auto rp = std::make_unique<LibRaw>();
    int ret = rp->open_file(QFile::encodeName(filePath).constData());
    if (ret != LIBRAW_SUCCESS)
        return std::unexpected(QObject::tr("Cannot open RAW file '%1': %2")
                                   .arg(filePath, QString::fromLatin1(libraw_strerror(ret))));

    const auto& S = rp->imgdata.sizes;
    QString err;
    if (!validateLoaderDims(S.width, S.height, 1, filePath, err))
        return std::unexpected(err);

    ret = rp->unpack();
    if (ret != LIBRAW_SUCCESS)
        return std::unexpected(QObject::tr("Cannot decode RAW file '%1': %2")
                                   .arg(filePath, QString::fromLatin1(libraw_strerror(ret))));

    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();
    img.width    = S.width;
    img.height   = S.height;
    const size_t n = static_cast<size_t>(S.width) * static_cast<size_t>(S.height);

    const int pattern = rp->imgdata.rawdata.raw_image ? bayerPatternOf(*rp) : -1;
    try {
        if (pattern >= 0) {
            // Plain Bayer: copy the visible CFA area, demosaic for display only.
            const unsigned short* raw = rp->imgdata.rawdata.raw_image;
            const size_t pitch = S.raw_pitch / sizeof(unsigned short);
            std::vector<float> cfa(n);
            for (int y = 0; y < S.height; ++y) {
                const unsigned short* row = raw + static_cast<size_t>(y + S.top_margin) * pitch
                                                + S.left_margin;
                float* out = cfa.data() + static_cast<size_t>(y) * S.width;
                for (int x = 0; x < S.width; ++x) out[x] = static_cast<float>(row[x]);
            }
            demosaicBayer(cfa, S.width, S.height, pattern, img.dataR, img.dataG, img.dataB);
        } else {
            // X-Trans / Foveon / linear DNG: LibRaw linear 16-bit, raw colour space.
            auto& P = rp->imgdata.params;
            P.gamm[0] = P.gamm[1] = 1.0;
            P.no_auto_bright = 1;
            P.output_bps     = 16;
            P.output_color   = 0;      // raw camera colour space, no matrix
            P.use_camera_wb  = 0;
            P.use_auto_wb    = 0;
            ret = rp->dcraw_process();
            if (ret != LIBRAW_SUCCESS)
                return std::unexpected(QObject::tr("Cannot process RAW file '%1': %2")
                                           .arg(filePath, QString::fromLatin1(libraw_strerror(ret))));
            std::unique_ptr<libraw_processed_image_t, void (*)(libraw_processed_image_t*)>
                mem(rp->dcraw_make_mem_image(&ret), LibRaw::dcraw_clear_mem);
            if (!mem || mem->type != LIBRAW_IMAGE_BITMAP || mem->bits != 16 || mem->colors < 1)
                return std::unexpected(QObject::tr("Unsupported RAW layout in '%1'").arg(filePath));
            img.width  = mem->width;
            img.height = mem->height;
            if (!validateLoaderDims(img.width, img.height, 1, filePath, err))
                return std::unexpected(err);
            const size_t m = static_cast<size_t>(img.width) * static_cast<size_t>(img.height);
            const auto* px = reinterpret_cast<const unsigned short*>(mem->data);
            const int ch = mem->colors;
            img.dataR.resize(m); img.dataG.resize(m); img.dataB.resize(m);
            for (size_t i = 0; i < m; ++i) {
                img.dataR[i] = px[i * ch];
                img.dataG[i] = px[i * ch + (ch > 1 ? 1 : 0)];
                img.dataB[i] = px[i * ch + (ch > 2 ? 2 : 0)];
            }
        }
        const size_t m = img.dataR.size();
        img.data.resize(m);
        for (size_t i = 0; i < m; ++i)
            img.data[i] = 0.25f * (img.dataR[i] + 2.0f * img.dataG[i] + img.dataB[i]);
    } catch (const std::exception& e) {
        return std::unexpected(QObject::tr("Out of memory loading RAW file '%1': %2")
                                   .arg(filePath, QString::fromLocal8Bit(e.what())));
    }
    img.isColor = true;
    fillRawMetadata(*rp, img);

    computeAutoStretch(img);
    spdlog::info("Loaded RAW: {}  {}x{}  camera='{}'  {}  exp={}s",
                 img.fileName.toStdString(), img.width, img.height, img.origin.toStdString(),
                 pattern >= 0 ? "Bayer CFA (superpixel luminance)" : "LibRaw linear 16-bit",
                 img.expTime);
    return img;
}

#endif // ASTROFIND_HAS_LIBRAW

} // anonymous namespace

// ─── loadImage (public dispatcher) ───────────────────────────────────────────

std::expected<FitsImage, QString> loadImage(const QString& filePath)
{
    const QString ext = QFileInfo(filePath).suffix().toLower();

    if (ext == "fits" || ext == "fit" || ext == "fts")
        return loadFits(filePath);

    if (ext == "ser")
        return loadSer(filePath);

    if (ext == "xisf")
        return loadXisf(filePath);

    if (ext == "tiff" || ext == "tif" || ext == "png"
     || ext == "bmp"  || ext == "jpg" || ext == "jpeg")
        return loadQImage(filePath);

    if (ext == "img" || ext == "lbl")
        return loadPds3(filePath);

    if (ext == "xml" && isPds4Label(filePath))
        return loadPds4(filePath);

    if (isDslrRawExtension(ext))
        return loadDslrRaw(filePath);

    return std::unexpected(
        QObject::tr("Unsupported image format '.%1' — "
                    "supported: fits, ser, xisf, tiff, tif, png, bmp, jpg, "
                    "PDS3 (img/lbl), PDS4 (xml), DSLR RAW").arg(ext));
}

bool isDslrRawExtension(const QString& ext)
{
    static const QStringList kRaw = {
        "cr2", "cr3", "crw", "nef", "nrw", "arw", "srf", "sr2", "orf", "rw2",
        "raf", "pef", "dng", "srw", "3fr", "erf", "kdc", "mrw", "x3f", "iiq",
        "mef", "mos", "rwl",
    };
    return kRaw.contains(ext.toLower());
}

bool hasDslrRawSupport() noexcept
{
#ifdef ASTROFIND_HAS_LIBRAW
    return true;
#else
    return false;
#endif
}

std::expected<FitsImage, QString> loadDslrRaw(const QString& filePath)
{
#ifdef ASTROFIND_HAS_LIBRAW
    return loadDslrRawImpl(filePath);
#else
    return std::unexpected(
        QObject::tr("DSLR RAW support is not available in this build of AstroFind "
                    "(compiled without LibRaw): %1").arg(filePath));
#endif
}

} // namespace core
