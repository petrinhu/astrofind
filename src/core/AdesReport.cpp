#include "AdesReport.h"

#include <QXmlStreamWriter>
#include <QDateTime>
#include <QTimeZone>
#include <QMap>

namespace core {

// ─── jdToIso8601 ─────────────────────────────────────────────────────────────

QString jdToIso8601(double jd, int decimalPlaces)
{
    // Unix epoch = JD 2440587.5
    const double unixSec  = (jd - 2440587.5) * 86400.0;
    const qint64 unixMs   = static_cast<qint64>(std::round(unixSec * 1000.0));
    const QDateTime dt    = QDateTime::fromMSecsSinceEpoch(unixMs, QTimeZone::utc());

    const int dp = std::clamp(decimalPlaces, 0, 3);
    QString fmt = QStringLiteral("yyyy-MM-ddTHH:mm:ss");
    if (dp == 1) fmt += QStringLiteral(".z");   // tenths
    else if (dp == 2) fmt += QStringLiteral(".zzz");  // will trim below // NOLINT(bugprone-branch-clone)
    else if (dp == 3) fmt += QStringLiteral(".zzz");  // milliseconds

    QString result = dt.toString(fmt);

    // Qt's "zzz" always gives 3 digits; trim to 2 if dp==2
    if (dp == 2 && result.contains(QLatin1Char('.')))
        result.chop(1);

    return result + QLatin1Char('Z');
}

// ─── Object name → ADES id fields ────────────────────────────────────────────
// ADES 2022 distinguishes:
//   permID  — numbered object: "433" (asteroid), "1P" (periodic comet)
//   provID  — provisional:     "2023 AB1" (asteroid), "C/2023 A1" (comet)
//   trkSub  — tracklet/local:  "NEOCP2024A"

struct AdesId {
    QString permID;
    QString provID;
    QString trkSub;
    bool    isComet = false;  ///< True when designation indicates a comet
};

// Returns "1P" for "1P/Halley", "2P" for "2P/Encke", etc.
// Returns empty string if not a numbered periodic comet.
static QString parseCometPermId(const QString& name)
{
    // Numbered periodic: one or more digits followed by 'P', then '/' or end
    int i = 0;
    while (i < name.size() && name[i].isDigit()) ++i;
    if (i > 0 && i < name.size() && name[i] == QLatin1Char('P')) {
        const QChar next = (i + 1 < name.size()) ? name[i + 1] : QLatin1Char('/');
        if (next == QLatin1Char('/') || next == QLatin1Char(' ') || i + 1 == name.size())
            return name.left(i + 1);  // e.g. "1P"
    }
    return {};
}

static AdesId parseObjectId(const core::Observation& obs)
{
    AdesId id;

    // ── Numbered asteroid ─────────────────────────────────────────────────────
    if (obs.mpcNumber > 0 && obs.objectType != QLatin1String("comet")) {
        id.permID = QString::number(obs.mpcNumber);
        return id;
    }

    // ── Strip leading "(nnn) " or "nnn — " prefix ────────────────────────────
    QString name = obs.objectName.trimmed();
    if (name.startsWith(QLatin1Char('('))) {
        const int paren = static_cast<int>(name.indexOf(QLatin1Char(')')));
        if (paren != -1) name = name.mid(paren + 2).trimmed();
    }

    if (name.isEmpty()) { id.trkSub = QStringLiteral("UNKN"); return id; }

    // ── Comet designations ────────────────────────────────────────────────────
    // Prefixes: C/ (long-period), P/ (unnumbered periodic), D/ (defunct),
    //           A/ (asteroid-like), X/ (insufficient data)
    // Numbered periodic: "NP/..." e.g. "1P/Halley"
    static const QStringList cometPrefixes = {
        QStringLiteral("C/"), QStringLiteral("P/"), QStringLiteral("D/"),
        QStringLiteral("A/"), QStringLiteral("X/")
    };

    const QString permId = parseCometPermId(name);
    if (!permId.isEmpty()) {
        // Numbered periodic comet: permID = "1P", "2P", …
        id.permID  = permId;
        id.isComet = true;
        return id;
    }

    for (const QString& pfx : cometPrefixes) {
        if (name.startsWith(pfx)) {
            // Provisional comet: use up to first space-separated word pair (e.g. "C/2023 A1")
            // ADES expects the canonical unpacked form: "C/2023 A1"
            const QStringList parts = name.split(QLatin1Char(' '));
            // parts[0] = "C/2023", parts[1] = "A1", parts[2…] = name (drop)
            if (parts.size() >= 2)
                id.provID = parts[0] + QLatin1Char(' ') + parts[1];
            else
                id.provID = name;
            id.isComet = true;
            return id;
        }
    }

    // ── Asteroid provisional (heuristic): "YYYY XX..." ────────────────────────
    if (name.length() >= 5 && name[4] == QLatin1Char(' ') && name.left(4).toInt() > 1800)
        id.provID = name;
    else if (!name.isEmpty())
        id.trkSub = name;
    else
        id.trkSub = QStringLiteral("UNKN");

    return id;
}

// Returns the ADES notes string for comet photometry.
static QString cometNoteStr(CometMagNote note)
{
    switch (note) {
    case CometMagNote::Nuclear: return QStringLiteral("N");
    case CometMagNote::Total:   return QStringLiteral("T");
    case CometMagNote::Coma:    return QStringLiteral("C");
    default:                    return {};
    }
}

// ─── XML helpers ──────────────────────────────────────────────────────────────

static void writeElement(QXmlStreamWriter& xml, const QString& name, const QString& value)
{
    xml.writeStartElement(name);
    xml.writeCharacters(value);
    xml.writeEndElement();
}

static void writeContext(QXmlStreamWriter& xml, const AdesContext& ctx)
{
    xml.writeStartElement(QStringLiteral("obsContext"));

    // Observatory
    xml.writeStartElement(QStringLiteral("observatory"));
    writeElement(xml, QStringLiteral("mpcCode"), ctx.mpcCode.isEmpty()
                                                   ? QStringLiteral("XXX") : ctx.mpcCode);
    xml.writeEndElement();

    // Submitter — written once, merging contact info when opted in
    {
        const bool useContact = ctx.includeContact && !ctx.contact1.isEmpty();
        const QString subName = useContact ? ctx.contact1 : ctx.observerName;
        if (!subName.isEmpty()) {
            xml.writeStartElement(QStringLiteral("submitter"));
            writeElement(xml, QStringLiteral("name"), subName);
            if (useContact) {
                if (!ctx.contact2.isEmpty())
                    writeElement(xml, QStringLiteral("institution"), ctx.contact2);
                if (!ctx.email.isEmpty())
                    writeElement(xml, QStringLiteral("email"), ctx.email);
            }
            xml.writeEndElement(); // submitter
        }
    }

    // Observers
    if (!ctx.observerName.isEmpty()) {
        xml.writeStartElement(QStringLiteral("observers"));
        writeElement(xml, QStringLiteral("name"), ctx.observerName);
        xml.writeEndElement();
    }

    // Measurers
    const QString mea = ctx.measurer.isEmpty() ? ctx.observerName : ctx.measurer;
    if (!mea.isEmpty()) {
        xml.writeStartElement(QStringLiteral("measurers"));
        writeElement(xml, QStringLiteral("name"), mea);
        xml.writeEndElement();
    }

    // Telescope
    if (!ctx.telescope.isEmpty()) {
        xml.writeStartElement(QStringLiteral("telescope"));
        writeElement(xml, QStringLiteral("name"), ctx.telescope);
        xml.writeEndElement();
    }

    // Software
    const QString sw = ctx.software.isEmpty() ? QStringLiteral("AstroFind") : ctx.software;
    xml.writeStartElement(QStringLiteral("software"));
    writeElement(xml, QStringLiteral("product"), sw);
    xml.writeEndElement();

    xml.writeEndElement(); // obsContext
}

static void writeOptical(QXmlStreamWriter& xml,
                          const core::Observation& obs,
                          const AdesContext& ctx)
{
    const AdesId id = parseObjectId(obs);
    const QString catalog = ctx.catalog.isEmpty() ? QStringLiteral("UCAC4") : ctx.catalog;
    const QString stn     = ctx.mpcCode.isEmpty() ? QStringLiteral("XXX") : ctx.mpcCode;

    xml.writeStartElement(QStringLiteral("optical"));

    if (!id.permID.isEmpty()) writeElement(xml, QStringLiteral("permID"), id.permID);
    if (!id.provID.isEmpty()) writeElement(xml, QStringLiteral("provID"), id.provID);
    if (!id.trkSub.isEmpty()) writeElement(xml, QStringLiteral("trkSub"), id.trkSub);

    writeElement(xml, QStringLiteral("mode"),    QStringLiteral("CCD"));
    writeElement(xml, QStringLiteral("stn"),     stn);
    writeElement(xml, QStringLiteral("obsTime"), jdToIso8601(obs.jd, ctx.timePrecision));
    writeElement(xml, QStringLiteral("ra"),      QString::number(obs.ra,  'f', 9));
    writeElement(xml, QStringLiteral("dec"),     QString::number(obs.dec, 'f', 9));
    // ADES 2022 §4.2: declare reference frame explicitly.
    // All coordinates are topocentric apparent place in the ICRF (= ICRS).
    writeElement(xml, QStringLiteral("sys"),     QStringLiteral("ICRF"));

    // Positional uncertainty — use measured value or conservative default
    const double rmsRA  = obs.raErr  > 0.0 ? obs.raErr  : 0.5;
    const double rmsDec = obs.decErr > 0.0 ? obs.decErr : 0.5;
    writeElement(xml, QStringLiteral("rmsRA"),   QString::number(rmsRA,  'f', 3));
    writeElement(xml, QStringLiteral("rmsDec"),  QString::number(rmsDec, 'f', 3));
    writeElement(xml, QStringLiteral("rmsCorr"), QStringLiteral("0.0000"));

    writeElement(xml, QStringLiteral("astCat"),  catalog);

    if (obs.mag < 90.0) {
        writeElement(xml, QStringLiteral("mag"),  QString::number(obs.mag, 'f', 2));
        if (obs.magErr > 0.0)
            writeElement(xml, QStringLiteral("rmsMag"), QString::number(obs.magErr, 'f', 2));
        const QString band = obs.magBand.isEmpty() ? QStringLiteral("C") : obs.magBand;
        writeElement(xml, QStringLiteral("band"), band);
        writeElement(xml, QStringLiteral("photCat"), catalog);
    }

    // Comet photometry note (N=nuclear, T=total, C=coma)
    const QString noteStr = cometNoteStr(obs.cometNote);
    if (!noteStr.isEmpty())
        writeElement(xml, QStringLiteral("notes"), noteStr);

    xml.writeEndElement(); // optical
}

// ─── generateAdesXml ─────────────────────────────────────────────────────────

QString generateAdesXml(const QVector<Observation>& obs, const AdesContext& ctx)
{
    QString output;
    QXmlStreamWriter xml(&output);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);

    xml.writeStartDocument(QStringLiteral("1.0"));
    xml.writeStartElement(QStringLiteral("ades"));
    xml.writeAttribute(QStringLiteral("version"), QStringLiteral("2022"));

    // Group observations by MPC station code (all ours → one block)
    xml.writeStartElement(QStringLiteral("obsBlock"));
    writeContext(xml, ctx);
    xml.writeStartElement(QStringLiteral("obsData"));

    for (const auto& o : obs)
        writeOptical(xml, o, ctx);

    xml.writeEndElement(); // obsData
    xml.writeEndElement(); // obsBlock
    xml.writeEndElement(); // ades
    xml.writeEndDocument();

    return output;
}

// ─── generateAdesPsv ─────────────────────────────────────────────────────────

QString generateAdesPsv(const QVector<Observation>& obs, const AdesContext& ctx)
{
    const QString catalog = ctx.catalog.isEmpty() ? QStringLiteral("UCAC4") : ctx.catalog;
    const QString stn     = ctx.mpcCode.isEmpty() ? QStringLiteral("XXX") : ctx.mpcCode;

    QString out;
    out.reserve(obs.size() * 120);

    // Header — ADES 2022 PSV; sys=ICRF is included per §4.2
    out += QStringLiteral("# version=2022\n");
    out += QStringLiteral("permID|provID|trkSub|mode|stn|obsTime|ra|dec|sys|"
                          "rmsRA|rmsDec|astCat|mag|rmsMag|band|photCat|"
                          "notes|observers|measurers|telescope\n");

    const QString mea = ctx.measurer.isEmpty() ? ctx.observerName : ctx.measurer;

    for (const auto& o : obs) {
        const AdesId id = parseObjectId(o);

        const QString rmsRA  = QString::number(o.raErr  > 0.0 ? o.raErr  : 0.5, 'f', 3);
        const QString rmsDec = QString::number(o.decErr > 0.0 ? o.decErr : 0.5, 'f', 3);

        QString mag, rmsMag, band, photCat;
        if (o.mag < 90.0) {
            mag     = QString::number(o.mag, 'f', 2);
            rmsMag  = o.magErr > 0.0 ? QString::number(o.magErr, 'f', 2) : QString();
            band    = o.magBand.isEmpty() ? QStringLiteral("C") : o.magBand;
            photCat = catalog;
        }

        // PSV row: pipe-separated, empty fields left blank
        out += id.permID + QLatin1Char('|')
             + id.provID + QLatin1Char('|')
             + id.trkSub + QLatin1Char('|')
             + QStringLiteral("CCD") + QLatin1Char('|')
             + stn + QLatin1Char('|')
             + jdToIso8601(o.jd, ctx.timePrecision) + QLatin1Char('|')
             + QString::number(o.ra,  'f', 9) + QLatin1Char('|')
             + QString::number(o.dec, 'f', 9) + QLatin1Char('|')
             + QStringLiteral("ICRF") + QLatin1Char('|')  // ADES 2022 §4.2 frame declaration
             + rmsRA  + QLatin1Char('|')
             + rmsDec + QLatin1Char('|')
             + catalog + QLatin1Char('|')
             + mag + QLatin1Char('|')
             + rmsMag + QLatin1Char('|')
             + band + QLatin1Char('|')
             + photCat + QLatin1Char('|')
             + cometNoteStr(o.cometNote) + QLatin1Char('|')
             + ctx.observerName + QLatin1Char('|')
             + mea + QLatin1Char('|')
             + ctx.telescope
             + QLatin1Char('\n');
    }

    return out;
}

} // namespace core
