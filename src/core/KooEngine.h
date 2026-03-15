#pragma once

#include "Overlay.h"

#include <QObject>
#include <QVector>
#include <QString>

class QNetworkAccessManager;

namespace core {

/// Queries the IMCCE SkyBoT web service for known solar system objects in a field.
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

signals:
    void objectsReady(const QVector<core::KooObject>& objects);
    void failed(const QString& reason);

private slots:
    void onReply();

private:
    QVector<KooObject> parseSkyBot(const QByteArray& data) const;

    QNetworkAccessManager* nam_;
    bool busy_ = false;
};

} // namespace core
