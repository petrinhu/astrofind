#pragma once

#include <QObject>
#include <QString>

class QNetworkAccessManager;
class QNetworkReply;

namespace core {

/// Submits an ADES 2022 observation report to the Minor Planet Center.
///
/// Two delivery methods:
///   1. HTTP POST  — async multipart/form-data, to the configured endpoint.
///   2. Email      — opens the system email client via mailto:.
///
/// Timeout: if the server does not respond within timeoutMs() milliseconds,
/// the timedOut() signal is emitted (and the request is aborted).
class MpcSubmit : public QObject {
    Q_OBJECT
public:
    explicit MpcSubmit(QObject* parent = nullptr);

    void    setEndpoint(const QString& url) { endpoint_ = url; }
    QString endpoint() const               { return endpoint_; }
    bool    isBusy()   const               { return busy_; }

    /// Network timeout in milliseconds (default 30 s). 0 = no timeout.
    void setTimeoutMs(int ms) { timeoutMs_ = ms; }
    int  timeoutMs()   const  { return timeoutMs_; }

    /// POST PSV (isXml=false) or XML (isXml=true) as multipart/form-data.
    void submitHttp(const QString& content, bool isXml = false);

    /// Open the system email client addressed to the MPC with the PSV in body.
    static void submitByEmail(const QString& psv);

signals:
    /// Emitted on a successful HTTP 2xx response (body may contain ack ID).
    void submitted(const QString& response);
    /// Emitted when the request fails at the network or server level (non-2xx).
    void failed(const QString& reason);
    /// Emitted when the server does not respond within timeoutMs().
    void timedOut();

private slots:
    void onReply();

private:
    QNetworkAccessManager* nam_        = nullptr;
    QNetworkReply*         pendingReply_ = nullptr;
    QString  endpoint_   = QStringLiteral("https://www.minorplanetcenter.net/report_ades");
    bool     busy_       = false;
    int      timeoutMs_  = 30000;
};

} // namespace core
