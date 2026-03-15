#include "KooEngine.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>

#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

namespace core {

// IMCCE SkyBoT cone-search endpoint
static const char* kSkyBotUrl =
    "https://vo.imcce.fr/webservices/skybot/skybotconesearch_query.php";

KooEngine::KooEngine(QNetworkAccessManager* nam, QObject* parent)
    : QObject(parent)
    , nam_(nam)
{}

void KooEngine::queryField(double ra, double dec, double radiusDeg, double jd)
{
    if (busy_) return;

    // SkyBoT expects radius in arcminutes
    const double radiusArcmin = radiusDeg * 60.0;

    QUrlQuery query;
    query.addQueryItem(QStringLiteral("-ep"),     QString::number(jd, 'f', 6));
    query.addQueryItem(QStringLiteral("-ra"),     QString::number(ra, 'f', 6));
    query.addQueryItem(QStringLiteral("-dec"),    QString::number(dec, 'f', 6));
    query.addQueryItem(QStringLiteral("-rm"),     QString::number(radiusArcmin, 'f', 2));
    query.addQueryItem(QStringLiteral("-output"), QStringLiteral("basic"));
    query.addQueryItem(QStringLiteral("-mime"),   QStringLiteral("json"));
    query.addQueryItem(QStringLiteral("-from"),   QStringLiteral("AstroFind/1.0"));

    QUrl url(kSkyBotUrl);
    url.setQuery(query);

    spdlog::debug("KooEngine: querying SkyBoT ra={:.4f} dec={:.4f} r={:.2f}' JD={:.4f}",
                  ra, dec, radiusArcmin, jd);

    busy_       = true;
    auto* reply = nam_->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &KooEngine::onReply);
}

void KooEngine::onReply()
{
    auto* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    reply->deleteLater();
    busy_ = false;

    if (reply->error() != QNetworkReply::NoError) {
        spdlog::warn("KooEngine: network error: {}", reply->errorString().toStdString());
        emit failed(reply->errorString());
        return;
    }

    const QByteArray data = reply->readAll();
    const auto objects = parseSkyBot(data);

    spdlog::info("KooEngine: {} known objects found", objects.size());
    emit objectsReady(objects);
}

QVector<KooObject> KooEngine::parseSkyBot(const QByteArray& data) const
{
    QVector<KooObject> result;
    const auto json = nlohmann::json::parse(data.toStdString(), nullptr, false);
    if (!json.is_object()) return result;

    // SkyBoT JSON response: {"data": {"result": [{...}, ...]}}
    const auto* resultPtr = [&]() -> const nlohmann::json* {
        if (json.contains("data") && json["data"].is_object() &&
            json["data"].contains("result"))
            return &json["data"]["result"];
        return nullptr;
    }();
    if (!resultPtr || !resultPtr->is_array()) return result;

    for (const auto& item : *resultPtr) {
        if (!item.is_object()) continue;
        KooObject obj;

        // Number / name
        const auto num  = item.value("Number",  std::string(""));
        const auto name = item.value("Name",    std::string(""));
        obj.number = num.empty() ? 0 : QString::fromStdString(num).toInt();
        obj.name   = QString::fromStdString(name.empty() ? num : name);

        // Type
        const auto cls = item.value("Class", std::string(""));
        if (cls == "Planet" || cls == "Dwarf Planet")
            obj.type = QStringLiteral("planet");
        else if (cls.find("Comet") != std::string::npos)
            obj.type = QStringLiteral("comet");
        else
            obj.type = QStringLiteral("asteroid");

        // Position
        if (item.contains("RA")  && item["RA"].is_number())
            obj.ra  = item["RA"].get<double>();
        if (item.contains("DEC") && item["DEC"].is_number())
            obj.dec = item["DEC"].get<double>();

        // Magnitude
        if (item.contains("V")   && item["V"].is_number())
            obj.mag = item["V"].get<double>();

        // Proper motion (SkyBoT gives dRA and dDEC in arcsec/hr)
        if (item.contains("dracosdec") && item["dracosdec"].is_number())
            obj.dRA  = item["dracosdec"].get<double>();
        if (item.contains("ddec") && item["ddec"].is_number())
            obj.dDec = item["ddec"].get<double>();

        if (!obj.name.isEmpty())
            result.append(obj);
    }
    return result;
}

} // namespace core
