#include "GusProject.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <cmath>
#include <limits>

namespace core {

// ── NaN helpers ───────────────────────────────────────────────────────────────

static QJsonValue nanToNull(double v)
{
    return std::isnan(v) ? QJsonValue(QJsonValue::Null) : QJsonValue(v);
}

static double nullToNan(const QJsonValue& v)
{
    if (v.isNull() || v.isUndefined())
        return std::numeric_limits<double>::quiet_NaN();
    return v.toDouble(0.0);
}

// ── PlateSolution ─────────────────────────────────────────────────────────────

static QJsonObject wcsToJson(const PlateSolution& w)
{
    QJsonObject o;
    o["crval1"] = w.crval1;  o["crval2"] = w.crval2;
    o["crpix1"] = w.crpix1;  o["crpix2"] = w.crpix2;
    o["cd1_1"]  = w.cd1_1;   o["cd1_2"]  = w.cd1_2;
    o["cd2_1"]  = w.cd2_1;   o["cd2_2"]  = w.cd2_2;
    o["rms"]    = w.rms;
    o["solved"] = w.solved;
    return o;
}

static PlateSolution wcsFromJson(const QJsonObject& o)
{
    PlateSolution w;
    w.crval1 = o["crval1"].toDouble();  w.crval2 = o["crval2"].toDouble();
    w.crpix1 = o["crpix1"].toDouble();  w.crpix2 = o["crpix2"].toDouble();
    w.cd1_1  = o["cd1_1"].toDouble();   w.cd1_2  = o["cd1_2"].toDouble();
    w.cd2_1  = o["cd2_1"].toDouble();   w.cd2_2  = o["cd2_2"].toDouble();
    w.rms    = o["rms"].toDouble();
    w.solved = o["solved"].toBool();
    return w;
}

// ── DetectedStar ──────────────────────────────────────────────────────────────

static QJsonObject starToJson(const DetectedStar& s)
{
    QJsonObject o;
    o["x"]       = s.x;        o["y"]       = s.y;
    o["flux"]    = s.flux;
    o["a"]       = s.a;        o["b"]       = s.b;
    o["theta"]   = s.theta;    o["snr"]     = s.snr;
    o["flag"]    = s.flag;
    o["ra"]      = s.ra;       o["dec"]     = s.dec;
    o["matched"] = s.matched;
    return o;
}

static DetectedStar starFromJson(const QJsonObject& o)
{
    DetectedStar s;
    s.x       = o["x"].toDouble();       s.y       = o["y"].toDouble();
    s.flux    = o["flux"].toDouble();
    s.a       = o["a"].toDouble();       s.b       = o["b"].toDouble();
    s.theta   = o["theta"].toDouble();   s.snr     = o["snr"].toDouble();
    s.flag    = o["flag"].toInt();
    s.ra      = o["ra"].toDouble();      s.dec     = o["dec"].toDouble();
    s.matched = o["matched"].toBool();
    return s;
}

// ── CatalogStar ───────────────────────────────────────────────────────────────

static QJsonObject catStarToJson(const CatalogStar& s)
{
    QJsonObject o;
    o["id"]    = s.id;
    o["ra"]    = s.ra;    o["dec"]   = s.dec;
    o["mag"]   = s.mag;
    o["pmRA"]  = s.pmRA;  o["pmDec"] = s.pmDec;
    return o;
}

static CatalogStar catStarFromJson(const QJsonObject& o)
{
    CatalogStar s;
    s.id    = o["id"].toString();
    s.ra    = o["ra"].toDouble();    s.dec   = o["dec"].toDouble();
    s.mag   = o["mag"].toDouble(99.0);
    s.pmRA  = o["pmRA"].toDouble();  s.pmDec = o["pmDec"].toDouble();
    return s;
}

// ── KooObject ─────────────────────────────────────────────────────────────────

static QJsonObject kooToJson(const KooObject& k)
{
    QJsonObject o;
    o["name"]   = k.name;    o["type"]   = k.type;
    o["ra"]     = k.ra;      o["dec"]    = k.dec;
    o["mag"]    = k.mag;
    o["dRA"]    = k.dRA;     o["dDec"]   = k.dDec;
    o["number"] = k.number;
    return o;
}

static KooObject kooFromJson(const QJsonObject& o)
{
    KooObject k;
    k.name   = o["name"].toString();  k.type   = o["type"].toString();
    k.ra     = o["ra"].toDouble();    k.dec    = o["dec"].toDouble();
    k.mag    = o["mag"].toDouble(99.0);
    k.dRA    = o["dRA"].toDouble();   k.dDec   = o["dDec"].toDouble();
    k.number = o["number"].toInt();
    return k;
}

// ── FitsImage (metadata only, no pixel data) ──────────────────────────────────

static QJsonObject imageToJson(const FitsImage& img)
{
    QJsonObject o;
    o["filePath"]      = img.filePath;
    o["fileName"]      = img.fileName;
    o["sourceZipPath"] = img.sourceZipPath;
    o["width"]      = img.width;
    o["height"]     = img.height;
    o["displayMin"] = static_cast<double>(img.displayMin);
    o["displayMax"] = static_cast<double>(img.displayMax);
    o["objectName"] = img.objectName;
    o["ra"]         = img.ra;
    o["dec"]        = img.dec;
    o["expTime"]    = img.expTime;
    o["dateObs"]    = img.dateObs.toString(Qt::ISODateWithMs);
    o["jd"]         = img.jd;
    o["filter"]     = img.filter;
    o["telescope"]  = img.telescope;
    o["observer"]   = img.observer;
    o["gain"]       = img.gain;
    o["pixScaleX"]  = img.pixScaleX;
    o["pixScaleY"]  = img.pixScaleY;
    o["saturation"] = img.saturation;
    o["binningX"]   = img.binningX;
    o["binningY"]   = img.binningY;
    o["siteLat"]            = nanToNull(img.siteLat);
    o["siteLon"]            = nanToNull(img.siteLon);
    o["siteAlt"]            = nanToNull(img.siteAlt);
    o["dateObsAmbiguous"]   = img.dateObsAmbiguous;
    o["wcs"]        = wcsToJson(img.wcs);

    QJsonArray stars;
    for (const auto& s : img.detectedStars) stars.append(starToJson(s));
    o["detectedStars"] = stars;

    QJsonArray cats;
    for (const auto& s : img.catalogStars) cats.append(catStarToJson(s));
    o["catalogStars"] = cats;

    QJsonArray koos;
    for (const auto& k : img.kooObjects) koos.append(kooToJson(k));
    o["kooObjects"] = koos;

    return o;
}

static FitsImage imageFromJson(const QJsonObject& o)
{
    FitsImage img;
    img.filePath      = o["filePath"].toString();
    img.fileName      = o["fileName"].toString();
    img.sourceZipPath = o["sourceZipPath"].toString();
    img.width       = o["width"].toInt();
    img.height      = o["height"].toInt();
    img.displayMin  = static_cast<float>(o["displayMin"].toDouble());
    img.displayMax  = static_cast<float>(o["displayMax"].toDouble(1.0));
    img.objectName  = o["objectName"].toString();
    img.ra          = o["ra"].toDouble();
    img.dec         = o["dec"].toDouble();
    img.expTime     = o["expTime"].toDouble();
    img.dateObs     = QDateTime::fromString(o["dateObs"].toString(), Qt::ISODateWithMs);
    img.jd          = o["jd"].toDouble();
    img.filter      = o["filter"].toString();
    img.telescope   = o["telescope"].toString();
    img.observer    = o["observer"].toString();
    img.gain        = o["gain"].toDouble(1.0);
    img.pixScaleX   = o["pixScaleX"].toDouble();
    img.pixScaleY   = o["pixScaleY"].toDouble();
    img.saturation  = o["saturation"].toDouble(65535.0);
    img.binningX    = o["binningX"].toInt(1);
    img.binningY    = o["binningY"].toInt(1);
    img.siteLat           = nullToNan(o["siteLat"]);
    img.siteLon           = nullToNan(o["siteLon"]);
    img.siteAlt           = nullToNan(o["siteAlt"]);
    img.dateObsAmbiguous  = o["dateObsAmbiguous"].toBool();
    img.wcs               = wcsFromJson(o["wcs"].toObject());

    for (const auto& v : o["detectedStars"].toArray())
        img.detectedStars.append(starFromJson(v.toObject()));
    for (const auto& v : o["catalogStars"].toArray())
        img.catalogStars.append(catStarFromJson(v.toObject()));
    for (const auto& v : o["kooObjects"].toArray())
        img.kooObjects.append(kooFromJson(v.toObject()));

    return img;
}

// ── Observation ───────────────────────────────────────────────────────────────

static QJsonObject obsToJson(const Observation& obs)
{
    QJsonObject o;
    o["objectName"]  = obs.objectName;
    o["objectType"]  = obs.objectType;
    o["mpcNumber"]   = obs.mpcNumber;
    o["ra"]          = obs.ra;
    o["dec"]         = obs.dec;
    o["raErr"]       = obs.raErr;
    o["decErr"]      = obs.decErr;
    o["jd"]          = obs.jd;
    o["pixX"]        = obs.pixX;
    o["pixY"]        = obs.pixY;
    o["mag"]         = obs.mag;
    o["magErr"]      = obs.magErr;
    o["magBand"]     = obs.magBand;
    o["fwhm"]        = obs.fwhm;
    o["snr"]         = obs.snr;
    o["cometNote"]   = static_cast<int>(obs.cometNote);
    o["imagePath"]   = obs.imagePath;
    o["imageIndex"]  = obs.imageIndex;
    return o;
}

static Observation obsFromJson(const QJsonObject& o)
{
    Observation obs;
    obs.objectName  = o["objectName"].toString();
    obs.objectType  = o["objectType"].toString();
    obs.mpcNumber   = o["mpcNumber"].toInt();
    obs.ra          = o["ra"].toDouble();
    obs.dec         = o["dec"].toDouble();
    obs.raErr       = o["raErr"].toDouble();
    obs.decErr      = o["decErr"].toDouble();
    obs.jd          = o["jd"].toDouble();
    obs.pixX        = o["pixX"].toDouble();
    obs.pixY        = o["pixY"].toDouble();
    obs.mag         = o["mag"].toDouble(99.0);
    obs.magErr      = o["magErr"].toDouble();
    obs.magBand     = o["magBand"].toString();
    obs.fwhm        = o["fwhm"].toDouble();
    obs.snr         = o["snr"].toDouble();
    obs.cometNote   = static_cast<CometMagNote>(o["cometNote"].toInt());
    obs.imagePath   = o["imagePath"].toString();
    obs.imageIndex  = o["imageIndex"].toInt(-1);
    return obs;
}

// ── GusProject ────────────────────────────────────────────────────────────────

QString GusProject::save(const QString& path) const
{
    QJsonObject root;
    root["version"]   = VERSION;
    root["sessionId"] = sessionId;
    root["step"]      = step;

    QJsonArray imgArr;
    for (const auto& img : images) imgArr.append(imageToJson(img));
    root["images"] = imgArr;

    QJsonArray obsArr;
    for (const auto& obs : observations) obsArr.append(obsToJson(obs));
    root["observations"] = obsArr;

    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return f.errorString();
    f.write(QJsonDocument(root).toJson());
    return {};
}

QString GusProject::load(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return f.errorString();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (doc.isNull())
        return err.errorString();

    const QJsonObject root = doc.object();
    const int ver = root["version"].toInt();
    if (ver != VERSION)
        return QStringLiteral("Versão de projeto desconhecida: %1").arg(ver);

    sessionId = root["sessionId"].toString();
    step      = root["step"].toInt();

    images.clear();
    for (const auto& v : root["images"].toArray())
        images.append(imageFromJson(v.toObject()));

    observations.clear();
    for (const auto& v : root["observations"].toArray())
        observations.append(obsFromJson(v.toObject()));

    return {};
}

} // namespace core
