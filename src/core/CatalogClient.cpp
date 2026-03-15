#include "CatalogClient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QDir>
#include <QUrl>
#include <QUrlQuery>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace core {

// VizieR TAP endpoint
static const char* kVizierTapUrl =
    "https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync";

// SQLite connection name
static const char* kDbConnection = "astrofind_catalog";

CatalogClient::CatalogClient(QNetworkAccessManager* nam, QObject* parent)
    : QObject(parent)
    , nam_(nam)
{
    const QString dataDir = QStandardPaths::writableLocation(
        QStandardPaths::AppLocalDataLocation);
    QDir().mkpath(dataDir);
    dbPath_ = dataDir + QStringLiteral("/catalog_cache.db");
    ensureDb();
}

void CatalogClient::queryRegion(double ra, double dec,
                                 double radiusDeg, double limitMag)
{
    if (busy_) return;
    queryRa_       = ra;
    queryDec_      = dec;
    queryRadius_   = radiusDeg;
    queryLimitMag_ = limitMag;

    // Try local cache first
    QVector<CatalogStar> cached;
    if (tryCache(ra, dec, radiusDeg, limitMag, cached)) {
        spdlog::debug("CatalogClient: cache hit ({} stars)", cached.size());
        emit starsReady(cached);
        return;
    }

    // Build ADQL query for UCAC4
    const QString adql = QString(
        "SELECT UCAC4, RAJ2000, DEJ2000, Vmag, pmRA, pmDE "
        "FROM \"I/322A/out\" "
        "WHERE CONTAINS(POINT('ICRS', RAJ2000, DEJ2000), "
        "  CIRCLE('ICRS', %1, %2, %3)) = 1 "
        "AND Vmag < %4 "
        "ORDER BY Vmag")
        .arg(ra, 0, 'f', 6)
        .arg(dec, 0, 'f', 6)
        .arg(radiusDeg, 0, 'f', 4)
        .arg(limitMag, 0, 'f', 1);

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("REQUEST"), QStringLiteral("doQuery"));
    query.addQueryItem(QStringLiteral("LANG"),    QStringLiteral("ADQL"));
    query.addQueryItem(QStringLiteral("FORMAT"),  QStringLiteral("json"));
    query.addQueryItem(QStringLiteral("QUERY"),   adql);

    QUrl url(kVizierTapUrl);
    url.setQuery(query);

    spdlog::debug("CatalogClient: querying VizieR ra={:.4f} dec={:.4f} r={:.2f}°",
                  ra, dec, radiusDeg);

    busy_      = true;
    auto* reply = nam_->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &CatalogClient::onReply);
    reply->setProperty("reply_ptr", QVariant::fromValue(reply));
}

void CatalogClient::onReply()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    busy_ = false;

    if (reply->error() != QNetworkReply::NoError) {
        spdlog::warn("CatalogClient: network error: {}", reply->errorString().toStdString());
        emit failed(reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    const auto stars = parseVizierJson(data);

    if (stars.isEmpty()) {
        emit failed(tr("No catalog stars found in field"));
        return;
    }

    saveCache(stars, queryRa_, queryDec_, queryRadius_, queryLimitMag_);
    spdlog::info("CatalogClient: {} UCAC4 stars downloaded", stars.size());
    emit starsReady(stars);
}

QVector<CatalogStar> CatalogClient::parseVizierJson(const QByteArray& data) const
{
    QVector<CatalogStar> result;
    const auto json = nlohmann::json::parse(data.toStdString(), nullptr, false);
    if (!json.is_object()) return result;

    // VizieR JSON TAP format: {"metadata": [...], "data": [[row0], [row1], ...]}
    const auto& cols = json.value("metadata", nlohmann::json::array());
    const auto& rows = json.value("data", nlohmann::json::array());

    // Map column names to indices
    int idxId  = -1, idxRa  = -1, idxDec = -1;
    int idxMag = -1, idxPmRa= -1, idxPmDe= -1;
    for (int i = 0; i < (int)cols.size(); ++i) {
        const auto name = cols[i].value("name", "");
        if (name == "UCAC4")   idxId   = i;
        else if (name == "RAJ2000") idxRa  = i;
        else if (name == "DEJ2000") idxDec = i;
        else if (name == "Vmag")    idxMag = i;
        else if (name == "pmRA")    idxPmRa= i;
        else if (name == "pmDE")    idxPmDe= i;
    }
    if (idxRa < 0 || idxDec < 0) return result;

    result.reserve(static_cast<int>(rows.size()));
    for (const auto& row : rows) {
        if (!row.is_array()) continue;
        CatalogStar s;
        if (idxId  >= 0 && !row[idxId].is_null())
            s.id   = QString::fromStdString(row[idxId].get<std::string>());
        if (idxRa  >= 0 && row[idxRa].is_number())
            s.ra   = row[idxRa].get<double>();
        if (idxDec >= 0 && row[idxDec].is_number())
            s.dec  = row[idxDec].get<double>();
        if (idxMag >= 0 && row[idxMag].is_number())
            s.mag  = row[idxMag].get<double>();
        if (idxPmRa>= 0 && row[idxPmRa].is_number())
            s.pmRA = row[idxPmRa].get<double>();
        if (idxPmDe>= 0 && row[idxPmDe].is_number())
            s.pmDec= row[idxPmDe].get<double>();
        result.append(s);
    }
    return result;
}

// ─── SQLite cache ──────────────────────────────────────────────────────────

void CatalogClient::ensureDb()
{
    auto db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), kDbConnection);
    db.setDatabaseName(dbPath_);
    if (!db.open()) {
        spdlog::warn("CatalogClient: cannot open catalog DB: {}",
                     db.lastError().text().toStdString());
        return;
    }
    QSqlQuery q(db);
    q.exec(QStringLiteral(
        "CREATE TABLE IF NOT EXISTS catalog_stars ("
        "  id      TEXT,"
        "  ra      REAL NOT NULL,"
        "  dec     REAL NOT NULL,"
        "  mag     REAL,"
        "  pm_ra   REAL DEFAULT 0,"
        "  pm_dec  REAL DEFAULT 0,"
        "  field_ra   REAL,"
        "  field_dec  REAL,"
        "  field_radius REAL,"
        "  field_limmag REAL"
        ")"));
    q.exec(QStringLiteral(
        "CREATE INDEX IF NOT EXISTS idx_catalog_radec "
        "ON catalog_stars(field_ra, field_dec)"));
}

bool CatalogClient::tryCache(double ra, double dec, double radius, double limitMag,
                              QVector<CatalogStar>& out) const
{
    auto db = QSqlDatabase::database(kDbConnection);
    if (!db.isOpen()) return false;

    // Look for a cached field that covers our request (within 0.05° and same limit)
    QSqlQuery q(db);
    q.prepare(QStringLiteral(
        "SELECT id, ra, dec, mag, pm_ra, pm_dec "
        "FROM catalog_stars "
        "WHERE ABS(field_ra  - :ra)  < 0.05 "
        "  AND ABS(field_dec - :dec) < 0.05 "
        "  AND field_radius >= :r "
        "  AND field_limmag >= :lm "
        "LIMIT 10000"));
    q.bindValue(":ra",  ra);
    q.bindValue(":dec", dec);
    q.bindValue(":r",   radius);
    q.bindValue(":lm",  limitMag);
    if (!q.exec()) return false;

    while (q.next()) {
        CatalogStar s;
        s.id    = q.value(0).toString();
        s.ra    = q.value(1).toDouble();
        s.dec   = q.value(2).toDouble();
        s.mag   = q.value(3).toDouble();
        s.pmRA  = q.value(4).toDouble();
        s.pmDec = q.value(5).toDouble();
        out.append(s);
    }
    return !out.isEmpty();
}

void CatalogClient::saveCache(const QVector<CatalogStar>& stars,
                               double ra, double dec,
                               double radius, double limitMag)
{
    auto db = QSqlDatabase::database(kDbConnection);
    if (!db.isOpen()) return;

    // Remove old cache for this field
    QSqlQuery del(db);
    del.prepare(QStringLiteral(
        "DELETE FROM catalog_stars "
        "WHERE ABS(field_ra - :ra) < 0.05 AND ABS(field_dec - :dec) < 0.05"));
    del.bindValue(":ra",  ra);
    del.bindValue(":dec", dec);
    del.exec();

    db.transaction();
    QSqlQuery ins(db);
    ins.prepare(QStringLiteral(
        "INSERT INTO catalog_stars "
        "(id, ra, dec, mag, pm_ra, pm_dec, field_ra, field_dec, field_radius, field_limmag) "
        "VALUES (:id, :ra, :dec, :mag, :pmra, :pmdec, :fra, :fdec, :fr, :flm)"));
    for (const auto& s : stars) {
        ins.bindValue(":id",   s.id);
        ins.bindValue(":ra",   s.ra);
        ins.bindValue(":dec",  s.dec);
        ins.bindValue(":mag",  s.mag);
        ins.bindValue(":pmra", s.pmRA);
        ins.bindValue(":pmdec",s.pmDec);
        ins.bindValue(":fra",  ra);
        ins.bindValue(":fdec", dec);
        ins.bindValue(":fr",   radius);
        ins.bindValue(":flm",  limitMag);
        ins.exec();
    }
    db.commit();
}

} // namespace core
