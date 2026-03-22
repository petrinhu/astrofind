#include "ImageLoader.h"
#include "FitsImage.h"

#include <QImage>
#include <QFile>
#include <QFileInfo>
#include <QDomDocument>
#include <QTimeZone>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cstring>
#include <cmath>

namespace core {
namespace {

// ─── Qt image loader (TIFF / PNG / BMP / JPEG) ───────────────────────────────

std::expected<FitsImage, QString> loadQImage(const QString& filePath)
{
    QImage qi(filePath);
    if (qi.isNull())
        return std::unexpected(QObject::tr("Cannot load image (unsupported or corrupt): %1")
            .arg(filePath));

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

    if (hdr.imageWidth == 0 || hdr.imageHeight == 0 || hdr.frameCount == 0)
        return std::unexpected(QObject::tr("Invalid SER header (zero dimensions): %1").arg(filePath));
    if (hdr.pixelDepth != 8 && hdr.pixelDepth != 16)
        return std::unexpected(QObject::tr("Unsupported SER pixel depth %1 in: %2")
            .arg(hdr.pixelDepth).arg(filePath));

    FitsImage img;
    img.filePath  = filePath;
    img.fileName  = QFileInfo(filePath).fileName();
    img.width     = static_cast<int>(hdr.imageWidth);
    img.height    = static_cast<int>(hdr.imageHeight);
    img.observer  = QString::fromLatin1(hdr.observer,   sizeof(hdr.observer)).trimmed();
    img.telescope = QString::fromLatin1(hdr.telescope, sizeof(hdr.telescope)).trimmed();

    const bool isRgb   = (hdr.colorID == SER_RGB || hdr.colorID == SER_BGR);
    const bool isBayer = (hdr.colorID >= SER_BAYER_RGGB && hdr.colorID <= SER_BAYER_BGGR);
    const int  channels = isRgb ? 3 : 1;
    const int  bps      = static_cast<int>(hdr.pixelDepth) / 8;   // bytes per sample
    const qint64 frameBytes = static_cast<qint64>(img.width) * img.height * channels * bps;

    QByteArray raw = f.read(frameBytes);
    if (raw.size() < frameBytes)
        return std::unexpected(
            QObject::tr("SER file truncated (expected %1 bytes for frame 1): %2")
                .arg(frameBytes).arg(filePath));

    // Pixel data endianness: big-endian when littleEndian==0 on a LE machine
    const bool needSwap = (hdr.pixelDepth == 16) && (hdr.littleEndian == 0);

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

    if (hdr.frameCount > 1)
        spdlog::warn("SER file has {} frames — only frame 1 loaded from: {}",
            hdr.frameCount, img.fileName.toStdString());

    computeAutoStretch(img);
    spdlog::info("Loaded SER: {}  {}x{}  {}  frames={}",
        img.fileName.toStdString(), img.width, img.height,
        img.isColor ? "color" : "mono", hdr.frameCount);
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
    const int W  = gp[0].toInt();
    const int H  = gp[1].toInt();
    const int Ch = gp.size() >= 3 ? gp[2].toInt() : 1;
    if (W <= 0 || H <= 0 || Ch <= 0 || Ch > 3)
        return std::unexpected(QObject::tr("XISF: invalid image dimensions %1 in: %2")
            .arg(geom, filePath));

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
        else if (fmt == "Float64") { const double* p = reinterpret_cast<const double*>(src); for (size_t i=0;i<count;++i) out[i]=static_cast<float>(p[i]); }
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
                img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(), QTimeZone::utc());
        }
    }

    computeAutoStretch(img);
    spdlog::info("Loaded XISF: {}  {}x{}×{}  fmt={}",
        img.fileName.toStdString(), W, H, Ch, fmt.toStdString());
    return img;
}

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

    return std::unexpected(
        QObject::tr("Unsupported image format '.%1' — "
                    "supported: fits, ser, xisf, tiff, tif, png, bmp, jpg").arg(ext));
}

} // namespace core
