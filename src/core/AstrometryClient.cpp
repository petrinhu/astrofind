#include "AstrometryClient.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHttpMultiPart>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <QUrlQuery>

#include <fitsio.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace core {

AstrometryClient::AstrometryClient(QNetworkAccessManager* nam, QObject* parent)
    : QObject(parent)
    , nam_(nam)
    , pollTimer_(new QTimer(this))
{
    pollTimer_->setInterval(5000);
    connect(pollTimer_, &QTimer::timeout, this, [this]() {
        if (state_ == State::WaitingForJob) {
            // Poll submission → get job list
            const QUrl url(baseUrl_ + "/api/submissions/" + QString::number(submissionId_));
            auto* reply = nam_->get(QNetworkRequest(url));
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                reply->deleteLater();
                if (reply->error() != QNetworkReply::NoError) {
                    if (++pollCount_ > maxPollCount_) fail(tr("Timeout waiting for astrometry job"));
                    return;
                }
                const auto body = reply->readAll().toStdString();
                const auto json = nlohmann::json::parse(body, nullptr, false);
                if (!json.is_object()) { ++pollCount_; return; }

                auto jobs = json.value("jobs", nlohmann::json::array());
                for (auto& j : jobs) {
                    if (!j.is_null() && j.is_number_integer()) {
                        jobId_ = j.get<int>();
                        state_ = State::PollingJob;
                        emit progress(tr("Solving... (job %1)").arg(jobId_), 60);
                        pollCount_ = 0;
                        return;
                    }
                }
                if (++pollCount_ > maxPollCount_) fail(tr("Timeout: no job started after 5 min"));
            });
        } else if (state_ == State::PollingJob) {
            // Poll job info → check status
            const QUrl url(baseUrl_ + "/api/jobs/" + QString::number(jobId_) + "/info");
            auto* reply = nam_->get(QNetworkRequest(url));
            connect(reply, &QNetworkReply::finished, this, [this, reply]() {
                reply->deleteLater();
                if (reply->error() != QNetworkReply::NoError) {
                    if (++pollCount_ > maxPollCount_) fail(tr("Timeout polling job status"));
                    return;
                }
                const auto json = nlohmann::json::parse(
                    reply->readAll().toStdString(), nullptr, false);
                if (!json.is_object()) { ++pollCount_; return; }

                const auto status = json.value("status", std::string("processing"));
                if (status == "success") {
                    pollTimer_->stop();
                    doDownloadWcs();
                } else if (status == "failure") {
                    pollTimer_->stop();
                    fail(tr("Astrometry.net: no plate solution found"));
                } else {
                    emit progress(tr("Solving... (processing)"), 60 + std::min(pollCount_, 20));
                    if (++pollCount_ > maxPollCount_) fail(tr("Timeout: plate solving took too long"));
                }
            });
        }
    });
}

void AstrometryClient::solveFits(const QString& fitsPath,
                                  double raHint, double decHint, double radiusDeg)
{
    if (isBusy()) {
        spdlog::warn("AstrometryClient: already busy, ignoring request");
        return;
    }
    pendingFitsPath_ = fitsPath;
    raHint_          = raHint;
    decHint_         = decHint;
    radiusDeg_       = radiusDeg;
    submissionId_    = 0;
    jobId_           = 0;
    session_.clear();
    pollCount_       = 0;

    if (apiKey_.isEmpty()) {
        fail(tr("Astrometry.net API key is not set"));
        return;
    }
    doLogin();
}

void AstrometryClient::cancel()
{
    pollTimer_->stop();
    if (!currentReply_.isNull())
        currentReply_->abort();
    state_ = State::Idle;
}

// ─── Private ─────────────────────────────────────────────────────────────────

void AstrometryClient::fail(const QString& reason)
{
    spdlog::error("AstrometryClient: {}", reason.toStdString());
    state_ = State::Idle;
    emit failed(reason);
}

void AstrometryClient::doLogin()
{
    state_ = State::LoggingIn;
    emit progress(tr("Logging in to astrometry.net..."), 5);

    QNetworkRequest req(QUrl(baseUrl_ + "/api/login"));
    req.setHeader(QNetworkRequest::ContentTypeHeader,
                  "application/x-www-form-urlencoded");

    const nlohmann::json j = {{"apikey", apiKey_.toStdString()}};
    const QByteArray body  = "request-json=" +
        QUrl::toPercentEncoding(QString::fromStdString(j.dump()));

    auto* reply     = nam_->post(req, body);
    currentReply_   = reply;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        currentReply_ = nullptr;
        if (reply->error() != QNetworkReply::NoError) {
            fail(reply->errorString());
            return;
        }
        const auto json = nlohmann::json::parse(
            reply->readAll().toStdString(), nullptr, false);
        if (!json.is_object() || json.value("status", "") != "success") {
            fail(tr("Login failed: %1")
                 .arg(QString::fromStdString(json.value("errormessage", "unknown"))));
            return;
        }
        session_ = QString::fromStdString(json["session"].get<std::string>());
        spdlog::debug("AstrometryClient: logged in, session={}", session_.toStdString());
        doUpload();
    });
}

void AstrometryClient::doUpload()
{
    state_ = State::Uploading;
    emit progress(tr("Uploading image..."), 15);

    auto* file = new QFile(pendingFitsPath_);
    if (!file->open(QIODevice::ReadOnly)) {
        delete file;
        fail(tr("Cannot open FITS file: %1").arg(pendingFitsPath_));
        return;
    }

    nlohmann::json reqJson = {
        {"session",              session_.toStdString()},
        {"allow_commercial_use", "d"},
        {"allow_modifications",  "d"},
        {"publicly_visible",     "n"},
    };
    // Add position hint if valid
    if (std::abs(raHint_) > 0.001 || std::abs(decHint_) > 0.001) {
        reqJson["ra"]     = raHint_;
        reqJson["dec"]    = decHint_;
        reqJson["radius"] = radiusDeg_;
    }

    auto* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart jsonPart;
    jsonPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QStringLiteral("form-data; name=\"request-json\""));
    jsonPart.setBody(QByteArray::fromStdString(reqJson.dump()));
    multiPart->append(jsonPart);

    QHttpPart filePart;
    filePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QStringLiteral("form-data; name=\"file\"; filename=\"image.fits\""));
    filePart.setHeader(QNetworkRequest::ContentTypeHeader,
                       QStringLiteral("application/octet-stream"));
    filePart.setBodyDevice(file);
    file->setParent(multiPart);
    multiPart->append(filePart);

    QNetworkRequest req(QUrl(baseUrl_ + "/api/upload"));
    auto* reply  = nam_->post(req, multiPart);
    multiPart->setParent(reply);
    currentReply_ = reply;

    connect(reply, &QNetworkReply::uploadProgress, this,
            [this](qint64 sent, qint64 total) {
        if (total > 0)
            emit progress(tr("Uploading... %1%").arg(sent * 100 / total),
                          15 + static_cast<int>(sent * 35 / total));
    });

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        currentReply_ = nullptr;
        if (reply->error() != QNetworkReply::NoError) {
            fail(reply->errorString());
            return;
        }
        const auto json = nlohmann::json::parse(
            reply->readAll().toStdString(), nullptr, false);
        if (!json.is_object() || json.value("status", "") != "success") {
            fail(tr("Upload rejected by astrometry.net"));
            return;
        }
        submissionId_ = json["subid"].get<int>();
        state_        = State::WaitingForJob;
        pollCount_    = 0;
        emit progress(tr("Submitted (ID=%1), waiting for solver...").arg(submissionId_), 50);
        pollTimer_->start();
    });
}

void AstrometryClient::doDownloadWcs()
{
    state_ = State::DownloadingWcs;
    emit progress(tr("Downloading WCS solution..."), 90);

    const QUrl url(baseUrl_ + "/wcs_file/" + QString::number(jobId_));
    auto* reply  = nam_->get(QNetworkRequest(url));
    currentReply_ = reply;

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        reply->deleteLater();
        currentReply_ = nullptr;
        if (reply->error() != QNetworkReply::NoError) {
            fail(reply->errorString());
            return;
        }
        const QByteArray wcsData = reply->readAll();
        if (wcsData.isEmpty()) {
            fail(tr("Empty WCS response from astrometry.net"));
            return;
        }

        // Write to temp FITS file, parse WCS headers with cfitsio
        const QString tmpPath = QDir::temp().filePath(
            QStringLiteral("astrofind_wcs_%1.fits").arg(jobId_));
        {
            QFile f(tmpPath);
            if (f.open(QIODevice::WriteOnly))
                f.write(wcsData);
        }

        PlateSolution wcs;
        fitsfile* fptr = nullptr;
        int status     = 0;
        if (fits_open_file(&fptr, tmpPath.toLocal8Bit().constData(), READONLY, &status) == 0) {
            char val[FLEN_VALUE] = {};
            char cmt[FLEN_COMMENT] = {};
            auto getDbl = [&](const char* key) -> double {
                status = 0;
                if (fits_read_keyword(fptr, key, val, cmt, &status) == 0) {
                    bool ok = false;
                    const double v = QString::fromLatin1(val).trimmed().toDouble(&ok);
                    return ok ? v : 0.0;
                }
                return 0.0;
            };
            wcs.crval1 = getDbl("CRVAL1");
            wcs.crval2 = getDbl("CRVAL2");
            wcs.crpix1 = getDbl("CRPIX1");
            wcs.crpix2 = getDbl("CRPIX2");
            wcs.cd1_1  = getDbl("CD1_1");
            wcs.cd1_2  = getDbl("CD1_2");
            wcs.cd2_1  = getDbl("CD2_1");
            wcs.cd2_2  = getDbl("CD2_2");
            wcs.solved = (wcs.cd1_1 != 0.0 || wcs.cd2_2 != 0.0);
            fits_close_file(fptr, &status);
        }
        QFile::remove(tmpPath);

        if (!wcs.solved) {
            fail(tr("Could not parse WCS from solution file"));
            return;
        }

        spdlog::info("AstrometryClient: solved! CRVAL=({:.4f}, {:.4f}) job={}",
                     wcs.crval1, wcs.crval2, jobId_);
        state_ = State::Idle;
        emit progress(tr("Plate solving complete!"), 100);
        emit solved(wcs, jobId_);
    });
}

} // namespace core
