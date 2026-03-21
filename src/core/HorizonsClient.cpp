#include "HorizonsClient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QStringList>

#include <cmath>

namespace core {

HorizonsClient::HorizonsClient(QNetworkAccessManager* nam, QObject* parent)
    : QObject(parent), nam_(nam)
{}

void HorizonsClient::setObserverLocation(double lat, double lon, double alt)
{
    obsLat_ = lat;
    obsLon_ = lon;
    obsAlt_ = alt;
}

void HorizonsClient::query(const QString& target, double jd)
{
    if (busy_) return;
    busy_   = true;
    target_ = target.trimmed();

    // ── Build Horizons REST URL ────────────────────────────────────────────
    // Request one hour centred on the observation JD (ensures at least one row).
    QUrl url(QStringLiteral("https://ssd.jpl.nasa.gov/api/horizons.api"));
    QUrlQuery q;

    q.addQueryItem(QStringLiteral("format"),     QStringLiteral("json"));
    q.addQueryItem(QStringLiteral("COMMAND"),    QStringLiteral("'%1'").arg(target_));
    q.addQueryItem(QStringLiteral("OBJ_DATA"),   QStringLiteral("NO"));
    q.addQueryItem(QStringLiteral("MAKE_EPHEM"), QStringLiteral("YES"));
    q.addQueryItem(QStringLiteral("EPHEM_TYPE"), QStringLiteral("OBSERVER"));

    // Observer: use site coordinates if provided, else geocentre
    const bool hasLoc = (obsLat_ != 0.0 || obsLon_ != 0.0);
    if (hasLoc) {
        q.addQueryItem(QStringLiteral("CENTER"),     QStringLiteral("coord@399"));
        q.addQueryItem(QStringLiteral("COORD_TYPE"), QStringLiteral("GEODETIC"));
        // Horizons SITE_COORD: longitude (E), latitude (N), altitude (km)
        q.addQueryItem(QStringLiteral("SITE_COORD"),
                       QString("%1,%2,%3")
                           .arg(obsLon_, 0, 'f', 4)
                           .arg(obsLat_, 0, 'f', 4)
                           .arg(obsAlt_ / 1000.0, 0, 'f', 3));
    } else {
        q.addQueryItem(QStringLiteral("CENTER"), QStringLiteral("500@399"));  // geocentre
    }

    const double jdStart = jd - 1.0 / 24.0;   // 1 hr before
    const double jdStop  = jd + 1.0 / 24.0;   // 1 hr after
    q.addQueryItem(QStringLiteral("START_TIME"), QString("JD%1").arg(jdStart, 0, 'f', 6));
    q.addQueryItem(QStringLiteral("STOP_TIME"),  QString("JD%1").arg(jdStop,  0, 'f', 6));
    q.addQueryItem(QStringLiteral("STEP_SIZE"),  QStringLiteral("1h"));

    // Quantities: 1=RA/Dec, 3=sky motion, 9=Vmag
    q.addQueryItem(QStringLiteral("QUANTITIES"), QStringLiteral("1,3,9"));
    q.addQueryItem(QStringLiteral("ANG_FORMAT"), QStringLiteral("DEG"));
    q.addQueryItem(QStringLiteral("CAL_FORMAT"), QStringLiteral("JD"));

    url.setQuery(q);

    auto* reply = nam_->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &HorizonsClient::onReply);
}

void HorizonsClient::onReply()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    reply->deleteLater();
    busy_ = false;

    if (reply->error() != QNetworkReply::NoError) {
        emit failed(reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    QJsonParseError jsonErr;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &jsonErr);
    if (doc.isNull()) {
        emit failed(tr("JSON parse error: %1").arg(jsonErr.errorString()));
        return;
    }

    const QJsonObject root = doc.object();

    // Horizons returns errors under the "error" key
    if (root.contains(QStringLiteral("error"))) {
        emit failed(root.value(QStringLiteral("error")).toString());
        return;
    }

    const QString resultText = root.value(QStringLiteral("result")).toString();
    if (resultText.isEmpty()) {
        emit failed(tr("Empty result from Horizons API"));
        return;
    }

    if (resultText.contains(QLatin1String("No matches found")) ||
        resultText.contains(QLatin1String("No such target")) ||
        resultText.contains(QLatin1String("Cannot resolve")))
    {
        emit failed(tr("Object not found in Horizons: \"%1\"").arg(target_));
        return;
    }

    const EphemerisMatch match = parseHorizonsResult(resultText);
    if (!std::isfinite(match.ra) || !std::isfinite(match.dec)) {
        emit failed(tr("Could not parse Horizons ephemeris for \"%1\"").arg(target_));
        return;
    }

    emit resultReady(match);
}

// ─── parseHorizonsResult ──────────────────────────────────────────────────────
//
// Parses the plain-text section embedded in the Horizons JSON "result" field.
// Data lines appear between $$SOE and $$EOE markers.
//
// With QUANTITIES='1,3,9', ANG_FORMAT='DEG', CAL_FORMAT='JD' the first data
// line looks like (after splitting on whitespace):
//
//   token[0]  = JD (e.g. "2460000.500000000")
//   token[1]  = optional flag ('*' = visible, 'C' = in shadow, etc.)
//               OR already RA if no flag present
//   ...       = RA (decimal degrees, 0–360)
//   ...       = Dec (decimal degrees, ±90)
//   ...       = dRA*cosD (arcsec/hr)
//   ...       = dDec (arcsec/hr)
//   ...       = APmag (or "n.a.")
//
// We skip any flag tokens and locate columns by position.

EphemerisMatch HorizonsClient::parseHorizonsResult(const QString& text) const
{
    EphemerisMatch match;
    match.name = target_;

    // Extract object name from the "Target body name:" header line
    static const QRegularExpression nameRe(
        R"(Target body name:\s+(.+?)\s+\{)",
        QRegularExpression::CaseInsensitiveOption);
    const auto nameM = nameRe.match(text);
    if (nameM.hasMatch()) {
        match.name = nameM.captured(1).trimmed();
        static const QRegularExpression numRe(R"(^\(?(\d+)\)?\s+)");
        const auto numM = numRe.match(match.name);
        if (numM.hasMatch())
            match.number = numM.captured(1).toInt();
    }

    // Locate ephemeris data block
    const int soePos = text.indexOf(QLatin1String("$$SOE"));
    const int eoePos = text.indexOf(QLatin1String("$$EOE"));
    if (soePos < 0 || eoePos < 0 || eoePos <= soePos)
        return match;

    // First non-empty data line after $$SOE
    int lineStart = text.indexOf(QLatin1Char('\n'), soePos);
    if (lineStart < 0) return match;
    ++lineStart;

    int lineEnd = text.indexOf(QLatin1Char('\n'), lineStart);
    if (lineEnd < 0 || lineEnd > eoePos) lineEnd = eoePos;

    const QString line = text.mid(lineStart, lineEnd - lineStart).trimmed();
    if (line.isEmpty() || line.startsWith(QLatin1String("$$EOE")))
        return match;

    // Split and parse — skip JD (token 0) then any single-char flag tokens
    const QStringList tokens = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
    int idx = 1;
    while (idx < tokens.size() && tokens[idx].size() == 1 && !tokens[idx][0].isDigit())
        ++idx;

    // idx now points at RA
    if (idx + 1 >= tokens.size()) return match;

    bool ok = false;
    const double ra = tokens[idx].toDouble(&ok);
    if (!ok || ra < 0.0 || ra > 360.0) return match;

    const double dec = tokens[idx + 1].toDouble(&ok);
    if (!ok || dec < -90.0 || dec > 90.0) return match;

    match.ra  = ra;
    match.dec = dec;

    if (idx + 3 < tokens.size()) {
        match.dRA  = tokens[idx + 2].toDouble();
        match.dDec = tokens[idx + 3].toDouble();
    }

    if (idx + 4 < tokens.size() && tokens[idx + 4] != QLatin1String("n.a.")) {
        match.mag = tokens[idx + 4].toDouble();
    }

    return match;
}

} // namespace core
