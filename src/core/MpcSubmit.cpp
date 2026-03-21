#include "MpcSubmit.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>

#include <spdlog/spdlog.h>

namespace core {

MpcSubmit::MpcSubmit(QObject* parent)
    : QObject(parent)
    , nam_(new QNetworkAccessManager(this))
{}

void MpcSubmit::submitHttp(const QString& content, bool isXml)
{
    if (busy_) return;

    QNetworkRequest req{QUrl(endpoint_)};
    req.setHeader(QNetworkRequest::UserAgentHeader,
                  QStringLiteral("AstroFind/1.0"));
    if (timeoutMs_ > 0)
        req.setTransferTimeout(timeoutMs_);

    // MPC expects multipart/form-data with field name "ades"
    auto* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart reportPart;
    reportPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                         QVariant(isXml
                             ? QStringLiteral("form-data; name=\"ades\"; filename=\"report.xml\"")
                             : QStringLiteral("form-data; name=\"ades\"; filename=\"report.psv\"")));
    reportPart.setHeader(QNetworkRequest::ContentTypeHeader,
                         isXml
                             ? QVariant(QStringLiteral("application/xml; charset=utf-8"))
                             : QVariant(QStringLiteral("text/plain; charset=utf-8")));
    reportPart.setBody(content.toUtf8());
    multiPart->append(reportPart);

    const QByteArray epBa = endpoint_.toUtf8();
    spdlog::info("MpcSubmit: posting {} bytes to {}",
                 content.toUtf8().size(), epBa.constData());

    busy_         = true;
    pendingReply_ = nam_->post(req, multiPart);
    multiPart->setParent(pendingReply_);   // auto-cleanup with reply
    connect(pendingReply_, &QNetworkReply::finished, this, &MpcSubmit::onReply);
}

void MpcSubmit::submitByEmail(const QString& psv)
{
    QUrlQuery q;
    q.addQueryItem(QStringLiteral("subject"),
                   QStringLiteral("ADES Observation Report"));
    q.addQueryItem(QStringLiteral("body"), psv);

    QUrl url(QStringLiteral("mailto:obs@minorplanetcenter.net"));
    url.setQuery(q);
    QDesktopServices::openUrl(url);
}

void MpcSubmit::onReply()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    pendingReply_ = nullptr;
    busy_         = false;

    // Timeout: Qt sets OperationCanceledError when transferTimeout fires
    if (reply->error() == QNetworkReply::OperationCanceledError) {
        spdlog::warn("MpcSubmit: request timed out after {} ms", timeoutMs_);
        emit timedOut();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        const QString reason = reply->errorString();
        spdlog::warn("MpcSubmit: network error: {}",
                     reason.toUtf8().constData());
        emit failed(reason);
        return;
    }

    const int     httpStatus = reply->attribute(
        QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QString body       = QString::fromUtf8(reply->readAll()).trimmed();

    spdlog::info("MpcSubmit: HTTP {} — response: {}",
                 httpStatus, body.left(200).toUtf8().constData());

    if (httpStatus >= 400) {
        emit failed(tr("Server returned HTTP %1:\n%2")
                        .arg(httpStatus).arg(body.left(500)));
        return;
    }

    emit submitted(body.isEmpty()
        ? tr("Report submitted (HTTP %1).").arg(httpStatus)
        : body);
}

} // namespace core
