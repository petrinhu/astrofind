#pragma once

#include "FitsImage.h"

#include <QObject>
#include <QPointer>
#include <QString>
#include <QTimer>

class QNetworkAccessManager;
class QNetworkReply;

namespace core {

/// Async client for the nova.astrometry.net REST API.
///
/// Workflow:  setApiKey() → solveFits() → listen to progress/solved/failed.
/// Only one solve can run at a time; check isBusy() before calling solveFits().
class AstrometryClient : public QObject {
    Q_OBJECT
public:
    explicit AstrometryClient(QNetworkAccessManager* nam, QObject* parent = nullptr);

    void setApiKey(const QString& key) { apiKey_ = key; }
    QString apiKey() const             { return apiKey_; }

    /// Override base URL for self-hosted astrometry.net instances.
    void setBaseUrl(const QString& url) { baseUrl_ = url; }

    /// Submit a FITS file for blind plate solving.
    /// raHint/decHint are initial position guesses (degrees, 0 = no hint).
    /// radiusDeg is the search radius around the hint position.
    void solveFits(const QString& fitsPath,
                   double raHint    = 0.0,
                   double decHint   = 0.0,
                   double radiusDeg = 5.0);

    bool isBusy() const noexcept { return state_ != State::Idle; }
    void cancel();

signals:
    void progress(const QString& message, int percent);
    void solved(const core::PlateSolution& wcs, int jobId);
    void failed(const QString& reason);

private:
    enum class State { Idle, LoggingIn, Uploading, WaitingForJob, PollingJob, DownloadingWcs };

    void doLogin();
    void doUpload();
    void doDownloadWcs();
    void startPolling();
    void fail(const QString& reason);

    QNetworkAccessManager*    nam_;
    QPointer<QNetworkReply>   currentReply_;
    QString  apiKey_;
    QString  baseUrl_       = QStringLiteral("https://nova.astrometry.net");
    QString  session_;
    int      submissionId_  = 0;
    int      jobId_         = 0;
    QString  pendingFitsPath_;
    double   raHint_        = 0.0;
    double   decHint_       = 0.0;
    double   radiusDeg_     = 5.0;
    State    state_         = State::Idle;
    QTimer*  pollTimer_     = nullptr;
    int      pollCount_     = 0;
};

} // namespace core
