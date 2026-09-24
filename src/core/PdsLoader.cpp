// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// Item 21.2 — NASA Planetary Data System image loader (PDS3 ODL labels and
// PDS4 XML labels). Hand-written on purpose: GDAL has PDS drivers but is far
// too heavy a dependency for "read one 2-D array described by a label".
// Every size read from a label is validated against the shared loader ceiling
// (validateLoaderDims) AND against the exact byte count the data needs before
// anything is allocated (lesson of AUD-INPUT-8: never trust a header).

#include "PdsLoader.h"

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QRegularExpression>
#include <QTimeZone>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <exception>
#include <limits>
#include <vector>

namespace core {
namespace {

// ─── Sample decoding (shared by PDS3 and PDS4) ───────────────────────────────

enum class SampleKind { SignedInt, UnsignedInt, Real };

struct SampleFormat {
    SampleKind kind  = SampleKind::UnsignedInt;
    int        bytes = 1;       ///< 1, 2, 4 or 8
    bool       msb   = true;    ///< big-endian on disk
};

double decodeSample(const unsigned char* p, const SampleFormat& f)
{
    std::uint64_t u = 0;
    if (f.msb) {
        for (int i = 0; i < f.bytes; ++i) u = (u << 8) | p[i];
    } else {
        for (int i = f.bytes - 1; i >= 0; --i) u = (u << 8) | p[i];
    }
    switch (f.kind) {
    case SampleKind::UnsignedInt:
        return static_cast<double>(u);
    case SampleKind::SignedInt: {
        const int bits = f.bytes * 8;
        if (bits < 64 && (u >> (bits - 1)) & 1u)
            u |= ~std::uint64_t{0} << bits;          // sign-extend
        return static_cast<double>(static_cast<std::int64_t>(u));
    }
    case SampleKind::Real:
        if (f.bytes == 4) {
            const std::uint32_t u32 = static_cast<std::uint32_t>(u);
            float v; std::memcpy(&v, &u32, sizeof v);
            return static_cast<double>(v);
        } else {
            double v; std::memcpy(&v, &u, sizeof v);
            return v;
        }
    }
    return 0.0;
}

struct ArrayLayout {
    QString      dataPath;
    qint64       offset       = 0;   ///< byte offset of the first line
    long         width        = 0;   ///< samples per line
    long         height       = 0;   ///< lines
    SampleFormat fmt;
    qint64       linePrefix   = 0;
    qint64       lineSuffix   = 0;
    double       scale        = 1.0;
    double       valueOffset  = 0.0;
    bool         hasMissing   = false;
    double       missing      = 0.0;  ///< raw (stored) value that means "no data"
};

/// Validate the layout against the loader ceiling and the real file size,
/// then read and decode it into img.data. Nothing is allocated before both
/// checks pass.
std::expected<void, QString> readArray(const ArrayLayout& a, FitsImage& img)
{
    QString err;
    if (!validateLoaderDims(a.width, a.height, 1, a.dataPath, err))
        return std::unexpected(err);

    QFile f(a.dataPath);
    if (!f.open(QIODevice::ReadOnly))
        return std::unexpected(QObject::tr("Cannot open PDS data file: %1").arg(a.dataPath));

    const qint64 lineBytes = a.linePrefix + static_cast<qint64>(a.width) * a.fmt.bytes + a.lineSuffix;
    const qint64 needed    = a.offset + lineBytes * static_cast<qint64>(a.height);
    if (a.offset < 0 || a.linePrefix < 0 || a.lineSuffix < 0 || needed > f.size())
        return std::unexpected(
            QObject::tr("PDS label declares %1 bytes of image data but '%2' has only %3 "
                        "(lying/corrupt label)")
                .arg(needed).arg(a.dataPath).arg(f.size()));
    if (!f.seek(a.offset))
        return std::unexpected(QObject::tr("Cannot seek in PDS data file: %1").arg(a.dataPath));

    const size_t n = static_cast<size_t>(a.width) * static_cast<size_t>(a.height);
    try {
        img.data.resize(n);
    } catch (const std::exception& e) {
        return std::unexpected(QObject::tr("Out of memory allocating PDS image '%1': %2")
                                   .arg(a.dataPath, QString::fromLocal8Bit(e.what())));
    }

    QByteArray line;
    const float nan = std::numeric_limits<float>::quiet_NaN();
    for (long y = 0; y < a.height; ++y) {
        line = f.read(lineBytes);
        if (line.size() != lineBytes)
            return std::unexpected(QObject::tr("PDS data file truncated: %1").arg(a.dataPath));
        const auto* p = reinterpret_cast<const unsigned char*>(line.constData()) + a.linePrefix;
        float* out = img.data.data() + static_cast<size_t>(y) * static_cast<size_t>(a.width);
        for (long x = 0; x < a.width; ++x, p += a.fmt.bytes) {
            const double raw = decodeSample(p, a.fmt);
            if ((a.hasMissing && raw == a.missing) || !std::isfinite(raw))
                out[x] = nan;
            else
                out[x] = static_cast<float>(raw * a.scale + a.valueOffset);
        }
    }
    img.width  = static_cast<int>(a.width);
    img.height = static_cast<int>(a.height);
    return {};
}

/// PDS dates are UTC ("2002-03-15T10:23:45.123", optional trailing Z, or
/// day-of-year "2002-074T10:23:45").
QDateTime parsePdsTime(QString s)
{
    s = s.trimmed();
    if (s.endsWith('Z') || s.endsWith('z')) s.chop(1);
    static const QRegularExpression doy(R"(^(\d{4})-(\d{3})(T.*)?$)");
    const auto m = doy.match(s);
    if (m.hasMatch()) {
        const QDate d = QDate(m.captured(1).toInt(), 1, 1).addDays(m.captured(2).toInt() - 1);
        s = d.toString(Qt::ISODate) + m.captured(3);
    }
    QDateTime t = QDateTime::fromString(s, Qt::ISODateWithMs);
    if (!t.isValid()) t = QDateTime::fromString(s, Qt::ISODate);
    if (t.isValid()) t.setTimeZone(QTimeZone(0));
    return t;
}

void finishTiming(FitsImage& img, const QDateTime& start, const QDateTime& stop, double exposure)
{
    if (start.isValid()) img.dateObs = start;
    if (exposure > 0.0)
        img.expTime = exposure;
    else if (start.isValid() && stop.isValid() && stop > start)
        img.expTime = static_cast<double>(start.msecsTo(stop)) / 1000.0;
    if (img.dateObs.isValid()) {
        // Same convention as loadFits: JD at mid-exposure.
        const QDateTime j2000(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
        img.jd = 2451545.0 + static_cast<double>(j2000.msecsTo(img.dateObs)) / 86400000.0
                 + img.expTime / 172800.0;
    }
}

/// Resolve a file name from a label next to the label itself. PDS names are
/// usually upper case while the copy on disk is often lower case.
QString resolveSibling(const QString& labelPath, const QString& name)
{
    const QDir dir = QFileInfo(labelPath).absoluteDir();
    for (const QString& cand : {name, name.toLower(), name.toUpper()}) {
        const QString p = dir.filePath(cand);
        if (QFileInfo::exists(p)) return p;
    }
    return dir.filePath(name);
}

// ─── PDS3 (ODL) ──────────────────────────────────────────────────────────────

struct OdlLabel {
    QHash<QString, QString> top;    ///< keywords outside any OBJECT
    QHash<QString, QString> image;  ///< keywords of the first top-level IMAGE object
    bool   hasImage   = false;
    qint64 labelBytes = 0;          ///< size of an attached label (bytes before END + EOL)
};

QString stripValue(QString v)
{
    v = v.trimmed();
    if (v.size() >= 2 && ((v.startsWith('"') && v.endsWith('"')) ||
                          (v.startsWith('\'') && v.endsWith('\''))))
        v = v.mid(1, v.size() - 2);
    return v.trimmed();
}

/// Numeric value of an ODL keyword, ignoring a trailing "<UNIT>".
double odlNumber(const QString& v, double def, QString* unit = nullptr)
{
    static const QRegularExpression re(R"(^\s*([-+]?[0-9.]+(?:[eE][-+]?\d+)?)\s*(?:<\s*([^>]*)>)?)");
    const auto m = re.match(stripValue(v));
    if (!m.hasMatch()) return def;
    if (unit) *unit = m.captured(2).trimmed().toUpper();
    bool ok = false;
    const double d = m.captured(1).toDouble(&ok);
    return ok ? d : def;
}

/// Parse an ODL label from `text` (attached or detached). Values may span
/// several lines when quoted or parenthesised; /* comments */ are dropped.
OdlLabel parseOdl(const QByteArray& text)
{
    OdlLabel L;
    int depth = 0;
    bool inImage = false;
    QString pendingKey, pendingVal;
    int open = 0; bool inQuote = false;

    auto commit = [&](const QString& key, const QString& val) {
        const QString k = key.trimmed().toUpper();
        const QString v = val.trimmed();
        if (k == "OBJECT") {
            ++depth;
            if (depth == 1 && stripValue(v).toUpper() == "IMAGE" && !L.hasImage) {
                inImage = true; L.hasImage = true;
            }
            return;
        }
        if (k == "END_OBJECT") {
            if (depth == 1) inImage = false;
            depth = std::max(0, depth - 1);
            return;
        }
        if (inImage && depth == 1) L.image.insert(k, v);
        else if (depth == 0 && !L.top.contains(k)) L.top.insert(k, v);
    };

    qint64 pos = 0;
    while (pos < text.size()) {
        qint64 nl = text.indexOf('\n', pos);
        if (nl < 0) nl = text.size();
        QString line = QString::fromLatin1(text.mid(pos, nl - pos));
        pos = nl + 1;

        // Drop /* ... */ comments (single-line; ODL comments do not nest).
        for (int c = line.indexOf("/*"); c >= 0; c = line.indexOf("/*")) {
            const int e = line.indexOf("*/", c + 2);
            line = (e < 0) ? line.left(c) : line.left(c) + line.mid(e + 2);
        }

        if (!pendingKey.isEmpty()) {             // continuation of a multi-line value
            pendingVal += ' ' + line.trimmed();
        } else {
            const QString t = line.trimmed();
            if (t.isEmpty()) continue;
            if (t.toUpper() == "END") { L.labelBytes = pos; break; }
            const int eq = t.indexOf('=');
            if (eq < 0) continue;
            pendingKey = t.left(eq);
            pendingVal = t.mid(eq + 1);
        }
        open = 0; inQuote = false;
        for (QChar ch : pendingVal) {
            if (ch == '"') inQuote = !inQuote;
            else if (!inQuote && (ch == '(' || ch == '{')) ++open;
            else if (!inQuote && (ch == ')' || ch == '}')) --open;
        }
        if (open <= 0 && !inQuote) {
            commit(pendingKey, pendingVal);
            pendingKey.clear(); pendingVal.clear();
        }
    }
    return L;
}

std::expected<SampleFormat, QString> pds3Format(const QString& sampleType, int bits)
{
    const QString t = stripValue(sampleType).toUpper();
    SampleFormat f;
    f.bytes = bits / 8;
    if (bits % 8 != 0 || (f.bytes != 1 && f.bytes != 2 && f.bytes != 4 && f.bytes != 8))
        return std::unexpected(QObject::tr("Unsupported PDS3 SAMPLE_BITS %1").arg(bits));

    static const QStringList lsbPrefixes = {"LSB_", "PC_", "VAX_"};
    f.msb = true;
    for (const QString& p : lsbPrefixes)
        if (t.startsWith(p)) f.msb = false;

    if (t.startsWith("VAX_") && t.contains("REAL"))
        return std::unexpected(QObject::tr("VAX floating point PDS3 images are not supported"));
    if (t.contains("REAL") || t == "FLOAT") {
        if (f.bytes != 4 && f.bytes != 8)
            return std::unexpected(QObject::tr("Unsupported PDS3 real sample size %1 bits").arg(bits));
        f.kind = SampleKind::Real;
    } else if (t.contains("UNSIGNED")) {
        f.kind = SampleKind::UnsignedInt;
    } else if (t.contains("INTEGER")) {
        // 8-bit "MSB_INTEGER" images are unsigned in practice (PDS3 SR note).
        f.kind = (f.bytes == 1) ? SampleKind::UnsignedInt : SampleKind::SignedInt;
    } else if (t.isEmpty() && f.bytes == 1) {
        f.kind = SampleKind::UnsignedInt;
    } else {
        return std::unexpected(QObject::tr("Unsupported PDS3 SAMPLE_TYPE '%1'").arg(t));
    }
    return f;
}

bool looksLikeOdl(const QByteArray& head)
{
    const QByteArray h = head.left(64).trimmed();
    return h.startsWith("PDS_VERSION_ID") || h.startsWith("ODL_VERSION_ID")
        || h.startsWith("CCSD3ZF");
}

} // anonymous namespace

// ─── PDS3 public entry point ────────────────────────────────────────────────

std::expected<FitsImage, QString> loadPds3(const QString& filePath)
{
    QFile lf(filePath);
    if (!lf.open(QIODevice::ReadOnly))
        return std::unexpected(QObject::tr("Cannot open PDS file: %1").arg(filePath));
    constexpr qint64 kMaxLabelBytes = 1 << 20;     // labels are a few KB; 1 MiB cap
    QByteArray head = lf.read(kMaxLabelBytes);
    lf.close();

    QString labelPath = filePath;
    bool attached = looksLikeOdl(head);
    if (!attached) {
        // An .img without attached label: look for a detached .lbl beside it.
        const QFileInfo fi(filePath);
        const QString base = fi.absoluteDir().filePath(fi.completeBaseName());
        for (const QString& ext : {QStringLiteral(".lbl"), QStringLiteral(".LBL")}) {
            if (QFileInfo::exists(base + ext)) { labelPath = base + ext; break; }
        }
        if (labelPath == filePath)
            return std::unexpected(QObject::tr("Not a PDS3 file (no attached or detached label): %1")
                                       .arg(filePath));
        QFile df(labelPath);
        if (!df.open(QIODevice::ReadOnly))
            return std::unexpected(QObject::tr("Cannot open PDS label: %1").arg(labelPath));
        head = df.read(kMaxLabelBytes);
        if (!looksLikeOdl(head))
            return std::unexpected(QObject::tr("Not a PDS3 label: %1").arg(labelPath));
    }

    const OdlLabel L = parseOdl(head);
    if (!L.hasImage)
        return std::unexpected(QObject::tr("PDS3 label has no IMAGE object: %1").arg(labelPath));

    auto img0 = [&](const char* k) { return L.image.value(QString::fromLatin1(k)); };
    auto top0 = [&](const char* k) { return L.top.value(QString::fromLatin1(k)); };
    auto any  = [&](const char* k) {
        const QString v = img0(k);
        return v.isEmpty() ? top0(k) : v;
    };

    const long recordBytes = static_cast<long>(odlNumber(top0("RECORD_BYTES"), 0));
    ArrayLayout a;
    a.width  = static_cast<long>(odlNumber(img0("LINE_SAMPLES"), 0));
    a.height = static_cast<long>(odlNumber(img0("LINES"), 0));
    const int bits = static_cast<int>(odlNumber(img0("SAMPLE_BITS"), 8));
    auto fmt = pds3Format(img0("SAMPLE_TYPE"), bits);
    if (!fmt) return std::unexpected(fmt.error() + QStringLiteral(": ") + labelPath);
    a.fmt        = *fmt;
    a.linePrefix = static_cast<qint64>(odlNumber(img0("LINE_PREFIX_BYTES"), 0));
    a.lineSuffix = static_cast<qint64>(odlNumber(img0("LINE_SUFFIX_BYTES"), 0));
    a.scale      = odlNumber(img0("SCALING_FACTOR"), 1.0);
    a.valueOffset= odlNumber(img0("OFFSET"), 0.0);
    if (!img0("MISSING_CONSTANT").isEmpty()) {
        a.hasMissing = true;
        a.missing    = odlNumber(img0("MISSING_CONSTANT"), 0.0);
    }

    const int bands = static_cast<int>(odlNumber(img0("BANDS"), 1));
    if (bands > 1)
        spdlog::warn("PDS3: {} bands in '{}' — loading band 1 only",
                     bands, QFileInfo(labelPath).fileName().toStdString());

    // ^IMAGE pointer: n | n <BYTES> | "FILE" | ("FILE", n) | ("FILE", n <BYTES>)
    const QString ptr = stripValue(top0("^IMAGE"));
    QString dataName;
    QString offsetPart = ptr;
    if (ptr.startsWith('(')) {
        const QString inner = ptr.mid(1, ptr.lastIndexOf(')') - 1);
        const int comma = inner.indexOf(',');
        dataName   = stripValue(comma < 0 ? inner : inner.left(comma));
        offsetPart = comma < 0 ? QString() : inner.mid(comma + 1);
    } else if (!ptr.isEmpty() && !ptr.at(0).isDigit()) {
        dataName   = ptr;
        offsetPart.clear();
    }
    a.dataPath = dataName.isEmpty() ? (attached ? filePath : QString())
                                    : resolveSibling(labelPath, dataName);
    if (a.dataPath.isEmpty()) {
        // Detached label without a file name in ^IMAGE: the .img we were given.
        a.dataPath = (labelPath == filePath)
            ? QFileInfo(filePath).absoluteDir().filePath(QFileInfo(filePath).completeBaseName() + ".img")
            : filePath;
    }
    if (!offsetPart.trimmed().isEmpty()) {
        QString unit;
        const double n = odlNumber(offsetPart, 1, &unit);
        if (n < 1)
            return std::unexpected(QObject::tr("Invalid ^IMAGE pointer in PDS3 label: %1").arg(labelPath));
        if (unit == "BYTES")          a.offset = static_cast<qint64>(n) - 1;
        else if (recordBytes > 0)     a.offset = (static_cast<qint64>(n) - 1) * recordBytes;
        else return std::unexpected(QObject::tr("PDS3 ^IMAGE record pointer without RECORD_BYTES: %1")
                                        .arg(labelPath));
    } else if (attached && dataName.isEmpty()) {
        const long labelRecords = static_cast<long>(odlNumber(top0("LABEL_RECORDS"), 0));
        a.offset = (labelRecords > 0 && recordBytes > 0)
                       ? static_cast<qint64>(labelRecords) * recordBytes : L.labelBytes;
    }

    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();
    auto rd = readArray(a, img);
    if (!rd) return std::unexpected(rd.error());

    img.objectName = stripValue(any("TARGET_NAME"));
    img.telescope  = stripValue(any("INSTRUMENT_HOST_NAME"));
    img.origin     = stripValue(any("INSTRUMENT_NAME"));
    img.filter     = stripValue(any("FILTER_NAME"));
    if (!any("RIGHT_ASCENSION").isEmpty()) img.ra  = odlNumber(any("RIGHT_ASCENSION"), 0.0);
    if (!any("DECLINATION").isEmpty())     img.dec = odlNumber(any("DECLINATION"), 0.0);

    QString expUnit;
    double exposure = odlNumber(any("EXPOSURE_DURATION"), 0.0, &expUnit);
    if (expUnit == "MS" || expUnit == "MSEC" || expUnit == "MILLISECONDS") exposure /= 1000.0;
    QString startStr = stripValue(any("START_TIME"));
    if (startStr.isEmpty() || startStr.toUpper() == "N/A") startStr = stripValue(any("IMAGE_TIME"));
    finishTiming(img, parsePdsTime(startStr), parsePdsTime(stripValue(any("STOP_TIME"))), exposure);

    computeAutoStretch(img);
    spdlog::info("Loaded PDS3: {}  {}x{}  {}-bit  data='{}' offset={}",
                 img.fileName.toStdString(), img.width, img.height, bits,
                 QFileInfo(a.dataPath).fileName().toStdString(), a.offset);
    return img;
}

// ─── PDS4 (XML) ──────────────────────────────────────────────────────────────

namespace {

QDomElement firstChild(const QDomElement& parent, const QString& localName)
{
    for (QDomElement e = parent.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        const QString n = e.localName().isEmpty() ? e.tagName().section(':', -1) : e.localName();
        if (n == localName) return e;
    }
    return {};
}

/// Depth-first (document order) search without recursion: a hostile label
/// nested thousands of levels deep must not be able to overflow the stack.
QDomElement firstDescendant(const QDomElement& root, const QString& localName)
{
    QDomElement e = root.firstChildElement();
    while (!e.isNull()) {
        const QString n = e.localName().isEmpty() ? e.tagName().section(':', -1) : e.localName();
        if (n == localName) return e;
        QDomElement next = e.firstChildElement();
        if (next.isNull()) {
            // Climb until a sibling exists, never above `root`.
            QDomElement up = e;
            while (!up.isNull() && up != root) {
                next = up.nextSiblingElement();
                if (!next.isNull()) break;
                up = up.parentNode().toElement();
            }
            if (up.isNull() || up == root) return {};
        }
        e = next;
    }
    return {};
}

QString childText(const QDomElement& parent, const QString& localName)
{
    return firstChild(parent, localName).text().trimmed();
}

std::expected<SampleFormat, QString> pds4Format(const QString& dataType)
{
    static const QHash<QString, SampleFormat> table = {
        {"SignedByte",         {SampleKind::SignedInt,   1, true }},
        {"UnsignedByte",       {SampleKind::UnsignedInt, 1, true }},
        {"SignedMSB2",         {SampleKind::SignedInt,   2, true }},
        {"SignedMSB4",         {SampleKind::SignedInt,   4, true }},
        {"SignedMSB8",         {SampleKind::SignedInt,   8, true }},
        {"UnsignedMSB2",       {SampleKind::UnsignedInt, 2, true }},
        {"UnsignedMSB4",       {SampleKind::UnsignedInt, 4, true }},
        {"UnsignedMSB8",       {SampleKind::UnsignedInt, 8, true }},
        {"SignedLSB2",         {SampleKind::SignedInt,   2, false}},
        {"SignedLSB4",         {SampleKind::SignedInt,   4, false}},
        {"SignedLSB8",         {SampleKind::SignedInt,   8, false}},
        {"UnsignedLSB2",       {SampleKind::UnsignedInt, 2, false}},
        {"UnsignedLSB4",       {SampleKind::UnsignedInt, 4, false}},
        {"UnsignedLSB8",       {SampleKind::UnsignedInt, 8, false}},
        {"IEEE754MSBSingle",   {SampleKind::Real,        4, true }},
        {"IEEE754MSBDouble",   {SampleKind::Real,        8, true }},
        {"IEEE754LSBSingle",   {SampleKind::Real,        4, false}},
        {"IEEE754LSBDouble",   {SampleKind::Real,        8, false}},
    };
    const auto it = table.constFind(dataType.trimmed());
    if (it == table.constEnd())
        return std::unexpected(QObject::tr("Unsupported PDS4 data_type '%1'").arg(dataType));
    return *it;
}

} // anonymous namespace

bool isPds4Label(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    const QByteArray head = f.read(8192);
    return head.contains("<Product_Observational") && head.contains("pds.nasa.gov/pds4");
}

std::expected<FitsImage, QString> loadPds4(const QString& labelPath)
{
    QFile lf(labelPath);
    if (!lf.open(QIODevice::ReadOnly))
        return std::unexpected(QObject::tr("Cannot open PDS4 label: %1").arg(labelPath));
    constexpr qint64 kMaxLabelBytes = 16 << 20;   // PDS4 labels can be verbose; 16 MiB cap
    if (lf.size() > kMaxLabelBytes)
        return std::unexpected(QObject::tr("PDS4 label too large: %1").arg(labelPath));

    // Qt 6.4 API (Debian 12 baseline): the ParseResult overload is Qt >= 6.5.
    QDomDocument doc;
    QString parseErr;
    int errLine = 0;
    if (!doc.setContent(lf.readAll(), /*namespaceProcessing=*/true, &parseErr, &errLine))
        return std::unexpected(QObject::tr("Invalid PDS4 XML label '%1': %2 (line %3)")
                                   .arg(labelPath, parseErr).arg(errLine));
    const QDomElement root = doc.documentElement();
    if (root.localName() != "Product_Observational")
        return std::unexpected(QObject::tr("Not a PDS4 observational product: %1").arg(labelPath));

    // First File_Area_Observational that holds a 2-D image array.
    QDomElement fileArea, array;
    for (QDomElement e = root.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        if (e.localName() != "File_Area_Observational") continue;
        for (const QString& n : {QStringLiteral("Array_2D_Image"), QStringLiteral("Array_2D")}) {
            array = firstChild(e, n);
            if (!array.isNull()) break;
        }
        if (!array.isNull()) { fileArea = e; break; }
    }
    if (array.isNull())
        return std::unexpected(QObject::tr("PDS4 label has no Array_2D_Image: %1").arg(labelPath));

    const QString fileName = childText(firstChild(fileArea, "File"), "file_name");
    if (fileName.isEmpty())
        return std::unexpected(QObject::tr("PDS4 label has no file_name: %1").arg(labelPath));
    const QString dataPath = resolveSibling(labelPath, fileName);

    // A PDS4 label may simply describe a FITS file: let the FITS loader do it.
    const QString dext = QFileInfo(dataPath).suffix().toLower();
    if (dext == "fits" || dext == "fit" || dext == "fts")
        return loadFits(dataPath);

    if (childText(array, "axes").toInt() != 2)
        return std::unexpected(QObject::tr("PDS4 array is not 2-D: %1").arg(labelPath));
    const QString order = childText(array, "axis_index_order");
    if (!order.isEmpty() && order != "Last Index Fastest")
        return std::unexpected(QObject::tr("Unsupported PDS4 axis_index_order '%1'").arg(order));

    ArrayLayout a;
    a.dataPath = dataPath;
    a.offset   = childText(array, "offset").toLongLong();
    const QDomElement elem = firstChild(array, "Element_Array");
    auto fmt = pds4Format(childText(elem, "data_type"));
    if (!fmt) return std::unexpected(fmt.error() + QStringLiteral(": ") + labelPath);
    a.fmt = *fmt;
    bool ok = false;
    const double sf = childText(elem, "scaling_factor").toDouble(&ok);
    if (ok) a.scale = sf;
    const double vo = childText(elem, "value_offset").toDouble(&ok);
    if (ok) a.valueOffset = vo;

    for (QDomElement ax = array.firstChildElement(); !ax.isNull(); ax = ax.nextSiblingElement()) {
        if (ax.localName() != "Axis_Array") continue;
        const int  seq      = childText(ax, "sequence_number").toInt();
        const long elements = childText(ax, "elements").toLong();
        if (seq == 1) a.height = elements;          // slowest axis = lines
        else if (seq == 2) a.width = elements;      // fastest axis = samples
    }

    const QDomElement special = firstChild(array, "Special_Constants");
    if (!special.isNull()) {
        const double m = childText(special, "missing_constant").toDouble(&ok);
        if (ok) { a.hasMissing = true; a.missing = m; }
    }

    FitsImage img;
    img.filePath = labelPath;
    img.fileName = QFileInfo(labelPath).fileName();
    auto rd = readArray(a, img);
    if (!rd) return std::unexpected(rd.error());

    const QDomElement area = firstChild(root, "Observation_Area");
    const QDomElement tc   = firstChild(area, "Time_Coordinates");
    const QDomElement tgt  = firstChild(area, "Target_Identification");
    img.objectName = childText(tgt, "name");
    const QDomElement osc = firstDescendant(area, "Observing_System_Component");
    img.telescope = childText(osc, "name");

    double exposure = 0.0;
    const QDomElement expEl = firstDescendant(root, "exposure_duration");
    if (!expEl.isNull()) {
        exposure = expEl.text().trimmed().toDouble();
        const QString unit = expEl.attribute("unit").toLower();
        if (unit == "ms") exposure /= 1000.0;
        else if (unit == "microseconds" || unit == "us") exposure /= 1e6;
    }
    finishTiming(img, parsePdsTime(childText(tc, "start_date_time")),
                 parsePdsTime(childText(tc, "stop_date_time")), exposure);

    computeAutoStretch(img);
    spdlog::info("Loaded PDS4: {}  {}x{}  data='{}' offset={}",
                 img.fileName.toStdString(), img.width, img.height,
                 QFileInfo(dataPath).fileName().toStdString(), a.offset);
    return img;
}

} // namespace core
