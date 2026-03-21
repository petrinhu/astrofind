#pragma once

#include "Overlay.h"

#include <QObject>
#include <QVector>
#include <QString>
#include <QFutureWatcher>

class QNetworkAccessManager;

namespace core {

/// Queries the IMCCE SkyBoT web service for known solar system objects in a field.
/// If SkyBoT is unreachable and a MPCORB.DAT path is configured, falls back to
/// a local Keplerian ephemeris scan run in a background thread.
///
/// Usage: queryField() → listen to objectsReady / failed.
class KooEngine : public QObject {
    Q_OBJECT
public:
    explicit KooEngine(QNetworkAccessManager* nam, QObject* parent = nullptr);

    /// Query all known SSOs in a circle around (ra, dec) at the given Julian Date.
    /// radiusDeg is the search radius (degrees).
    void queryField(double ra, double dec, double radiusDeg, double jd);

    bool isBusy() const noexcept { return busy_; }

    // ── Offline fallback configuration ──────────────────────────────────────
    void setMpcOrbPath(const QString& path)  { mpcOrbPath_ = path; }
    void setObserverLocation(double lat, double lon, double alt)
        { obsLat_ = lat; obsLon_ = lon; obsAlt_ = alt; }

    /// If true, run a local MPCORB scan when SkyBoT is not reachable.
    void setOfflineFallbackEnabled(bool enabled) { offlineFallback_ = enabled; }

signals:
    void objectsReady(const QVector<core::KooObject>& objects);
    void failed(const QString& reason);
    /// Emitted during offline MPCORB scan to update a progress bar (linesRead, totalLines).
    void scanProgress(int linesRead, int totalLines);

private slots:
    void onReply();
    void onOfflineScanDone();

private:
    QVector<KooObject> parseSkyBot(const QByteArray& data) const;
    void startOfflineScan();

    QNetworkAccessManager* nam_;
    bool busy_ = false;

    // Offline fallback
    QString mpcOrbPath_;
    double  obsLat_ = 0.0;
    double  obsLon_ = 0.0;
    double  obsAlt_ = 0.0;
    bool    offlineFallback_ = false;

    // State saved for fallback retry
    double savedRa_  = 0.0;
    double savedDec_ = 0.0;
    double savedRad_ = 0.0;
    double savedJd_  = 0.0;

    QFutureWatcher<QVector<KooObject>>* watcher_ = nullptr;
    bool cancelled_ = false;
};

} // namespace core
