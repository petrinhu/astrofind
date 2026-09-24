// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

#include "Ephemeris.h"

#include <QObject>
#include <QString>
#include <QPointer>

class QNetworkAccessManager;
class QNetworkReply;

namespace core {

/// AUD-SEC-12: true when @p target (already trimmed) is safe to interpolate
/// into the Horizons `COMMAND='...'` parameter. Allowlist: 1–64 characters,
/// ASCII letters/digits, space and `/ - ( ) . _`, with at least one letter or
/// digit. This covers numbers ("433"), names ("Ceres"), provisional
/// designations ("2024 AB1") and comets ("C/2023 A3", "1P/Halley",
/// "P/2010 A2 (LINEAR)"). Quotes (which would close the COMMAND string),
/// `;`, `=`, `+` (decoded as a space in a query string), control characters
/// and non-ASCII input are rejected.
bool isValidHorizonsTarget(const QString& target);

/// Queries the JPL Horizons REST API for a single solar system object's
/// ephemeris at a given Julian Date.
///
/// Usage:
///   auto* h = new HorizonsClient(nam, this);
///   h->setObserverLocation(lat, lon, alt);
///   connect(h, &HorizonsClient::resultReady, ...);
///   h->query("433", jd);
class HorizonsClient : public QObject {
    Q_OBJECT
public:
    explicit HorizonsClient(QNetworkAccessManager* nam, QObject* parent = nullptr);

    void setObserverLocation(double lat, double lon, double alt);

    /// Query JPL Horizons for @p target at the given JD.
    /// @p target can be a number ("433"), name ("Eros"), or designation ("2023 DW").
    /// A target rejected by isValidHorizonsTarget() emits failed() without any
    /// network request (AUD-SEC-12).
    void query(const QString& target, double jd);

    bool isBusy() const noexcept { return busy_; }

    /// Abort the in-flight Horizons request (if any) and reset busy state.
    void cancel();

signals:
    void resultReady(const core::EphemerisMatch& match);
    void failed(const QString& reason);

private slots:
    void onReply();

private:
    EphemerisMatch parseHorizonsResult(const QString& text) const;

    QNetworkAccessManager* nam_;
    QPointer<QNetworkReply> currentReply_;
    bool    busy_   = false;
    double  obsLat_ = 0.0;
    double  obsLon_ = 0.0;
    double  obsAlt_ = 0.0;  ///< metres
    QString target_;
};

} // namespace core
