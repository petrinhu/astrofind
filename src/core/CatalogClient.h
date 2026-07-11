// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

#include "Overlay.h"

#include <QObject>
#include <QVector>
#include <QString>
#include <QPointer>

class QNetworkAccessManager;
class QNetworkReply;

namespace core {

/// Downloads astrometric reference stars from VizieR (UCAC4) and caches them locally.
///
/// Usage: queryRegion() → listen to starsReady / failed.
class CatalogClient : public QObject {
    Q_OBJECT
public:
    explicit CatalogClient(QNetworkAccessManager* nam, QObject* parent = nullptr);

    /// Override the VizieR TAP endpoint.
    ///
    /// AUD-SEC-4: enforces transport security. `https://` is always accepted.
    /// `http://` is accepted only for localhost/127.0.0.1/::1 (a local
    /// mirror/proxy on the same machine). Any other `http://` (or unknown
    /// scheme) is rejected — the previous (safe) vizierUrl_ is kept and a
    /// warning is logged.
    void setVizierUrl(const QString& url);
    const QString& vizierUrl() const { return vizierUrl_; }
    /// Set the astrometric catalog to query. Supported: "UCAC4" (default), "GaiaDR3".
    void setCatalogType(const QString& type) { catalogType_   = type; }

    /// Query stars in a circle around (ra, dec) with given radius (degrees).
    /// Uses SQLite cache if available; downloads from VizieR otherwise.
    void queryRegion(double ra, double dec, double radiusDeg, double limitMag = 16.0);

    bool isBusy() const noexcept { return busy_; }

    /// Abort the in-flight VizieR request (if any) and reset busy state.
    void cancel();

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
    QPointer<QNetworkReply> currentReply_;
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
