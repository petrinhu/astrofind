#pragma once

#include "Overlay.h"

#include <QObject>
#include <QVector>
#include <QString>

class QNetworkAccessManager;

namespace core {

/// Downloads astrometric reference stars from VizieR (UCAC4) and caches them locally.
///
/// Usage: queryRegion() → listen to starsReady / failed.
class CatalogClient : public QObject {
    Q_OBJECT
public:
    explicit CatalogClient(QNetworkAccessManager* nam, QObject* parent = nullptr);

    void setVizierUrl(const QString& url)    { vizierUrl_    = url; }
    /// Set the astrometric catalog to query. Supported: "UCAC4" (default), "GaiaDR3".
    void setCatalogType(const QString& type) { catalogType_   = type; }

    /// Query stars in a circle around (ra, dec) with given radius (degrees).
    /// Uses SQLite cache if available; downloads from VizieR otherwise.
    void queryRegion(double ra, double dec, double radiusDeg, double limitMag = 16.0);

    bool isBusy() const noexcept { return busy_; }

signals:
    void starsReady(const QVector<core::CatalogStar>& stars);
    void failed(const QString& reason);

private slots:
    void onReply();

private:
    bool tryCache(double ra, double dec, double radius, double limitMag,
                  QVector<CatalogStar>& out) const;
    void saveCache(const QVector<CatalogStar>& stars,
                   double ra, double dec, double radius, double limitMag);
    void ensureDb();
    QVector<CatalogStar> parseVizierJson(const QByteArray& data) const;

    QNetworkAccessManager* nam_;
    bool    busy_        = false;
    QString dbPath_;
    QString vizierUrl_   = QStringLiteral("https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync");
    QString catalogType_ = QStringLiteral("UCAC4");
    double  queryRa_     = 0.0;
    double  queryDec_    = 0.0;
    double  queryRadius_ = 0.0;
    double  queryLimitMag_ = 16.0;
};

} // namespace core
