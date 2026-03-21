#include "FitsImage.h"

#include <fitsio.h>
#include <spdlog/spdlog.h>

#include <QFileInfo>
#include <QRegularExpression>
#include <QTimeZone>

#include <algorithm>
#include <cmath>
#include <limits>

namespace core {

// ─── PlateSolution ───────────────────────────────────────────────────────────

void PlateSolution::pixToSky(double px, double py, double& ra, double& dec) const noexcept
{
    const double dx = px - crpix1;
    const double dy = py - crpix2;
    ra  = crval1 + cd1_1 * dx + cd1_2 * dy;
    dec = crval2 + cd2_1 * dx + cd2_2 * dy;
}

void PlateSolution::skyToPix(double ra, double dec, double& px, double& py) const noexcept
{
    const double det = cd1_1 * cd2_2 - cd1_2 * cd2_1;
    if (std::abs(det) < 1e-15) { px = py = 0.0; return; }
    const double dra  = ra  - crval1;
    const double ddec = dec - crval2;
    px = crpix1 + ( cd2_2 * dra - cd1_2 * ddec) / det;
    py = crpix2 + (-cd2_1 * dra + cd1_1 * ddec) / det;
}

// ─── FITS Loading ─────────────────────────────────────────────────────────────

namespace {

QString cfitsioError(int status)
{
    char errmsg[FLEN_ERRMSG];
    fits_get_errstatus(status, errmsg);
    return QString::fromLatin1(errmsg);
}

double parseRaHMS(const QString& s)
{
    // "HH MM SS.sss" or "HHhMMmSS.ssss"
    const QStringList parts = s.trimmed().split(QRegularExpression(R"([\s:hHmMdD]+)"), Qt::SkipEmptyParts);
    if (parts.size() < 3) return 0.0;
    const double h = parts[0].toDouble();
    const double m = parts[1].toDouble();
    const double sc= parts[2].toDouble();
    return (h + m/60.0 + sc/3600.0) * 15.0;
}

double parseDecDMS(const QString& s)
{
    const QString ts = s.trimmed();
    const bool neg = ts.startsWith('-');
    const QStringList parts = ts.mid(neg ? 1 : 0).split(QRegularExpression(R"([\s:dDmM'\"]+)"), Qt::SkipEmptyParts);
    if (parts.size() < 3) return 0.0;
    const double d = parts[0].toDouble();
    const double m = parts[1].toDouble();
    const double sc= parts[2].toDouble();
    const double v = d + m/60.0 + sc/3600.0;
    return neg ? -v : v;
}

void readHeader(fitsfile* fptr, FitsImage& img)
{
    char valstr[FLEN_VALUE] = {};
    char comment[FLEN_COMMENT] = {};
    int  status = 0;

    auto getStr = [&](const char* key) -> QString {
        status = 0;
        if (fits_read_keyword(fptr, key, valstr, comment, &status) == 0) {
            QString s = QString::fromLatin1(valstr).trimmed();
            // Remove FITS string quotes
            if (s.startsWith('\'')) s = s.mid(1);
            if (s.endsWith('\''))   s.chop(1);
            return s.trimmed();
        }
        return {};
    };
    auto getDbl = [&](const char* key, double def = 0.0) -> double {
        const QString s = getStr(key);
        bool ok = false;
        const double v = s.toDouble(&ok);
        return ok ? v : def;
    };

    img.objectName = getStr("OBJECT");
    img.filter     = getStr("FILTER");
    img.telescope  = getStr("TELESCOP");
    img.observer   = getStr("OBSERVER");
    img.expTime    = getDbl("EXPTIME");
    img.gain       = getDbl("GAIN", 1.0);
    // SATURATE (common), SATLEVEL (PS1/MaxIm), DATAMAX (CFITSIO default)
    {
        double sat = getDbl("SATURATE", 0.0);
        if (sat == 0.0) sat = getDbl("SATLEVEL", 0.0);
        if (sat == 0.0) sat = getDbl("DATAMAX",  0.0);
        img.saturation = sat > 0.0 ? sat : 65535.0;
    }
    img.jd         = getDbl("JD");

    // Binning factor — affects pixel scale if present in header
    const int binX = std::max(1, static_cast<int>(getDbl("XBINNING", 1.0)));
    const int binY = std::max(1, static_cast<int>(getDbl("YBINNING", 1.0)));
    img.binningX = binX;
    img.binningY = binY;

    // RA/Dec: try decimal first, then HMS/DMS strings
    const QString raStr  = getStr("OBJCTRA");
    const QString decStr = getStr("OBJCTDEC");
    if (!raStr.isEmpty())  img.ra  = parseRaHMS(raStr);
    if (!decStr.isEmpty()) img.dec = parseDecDMS(decStr);
    if (img.ra == 0.0)  img.ra  = getDbl("RA");
    if (img.dec == 0.0) img.dec = getDbl("DEC");

    // DATE-OBS + TIMESYS
    // FITS standard mandates UTC, but amateur cameras sometimes omit the 'Z'
    // suffix or even write local time.  Strategy:
    //   1. Parse the string — Qt::ISODate sets timeSpec to UTC if 'Z' or
    //      OffsetFromUTC if '+HH:MM' is present, LocalTime otherwise.
    //   2. If TIMESYS='UTC'/'UT'  → treat LocalTime result as UTC.
    //   3. If TIMESYS is absent and result is LocalTime → assume UTC but flag.
    //   4. Any OffsetFromUTC → convert to UTC.
    // In all cases dateObs is stored as UTC so JD is correct.
    const QString dateStr = getStr("DATE-OBS");
    const QString timesys = getStr("TIMESYS").trimmed().toUpper();
    if (!dateStr.isEmpty()) {
        img.dateObs = QDateTime::fromString(dateStr, Qt::ISODate);
        if (!img.dateObs.isValid())
            img.dateObs = QDateTime::fromString(dateStr, "yyyy-MM-dd'T'HH:mm:ss.zzz");
        if (!img.dateObs.isValid())
            img.dateObs = QDateTime::fromString(dateStr, "yyyy-MM-dd");

        if (img.dateObs.isValid()) {
            switch (img.dateObs.timeSpec()) {
            case Qt::UTC:
                // String had explicit 'Z' — already UTC, nothing to do
                break;
            case Qt::OffsetFromUTC:
                // String had explicit +HH:MM / -HH:MM — convert to UTC
                img.dateObs = img.dateObs.toUTC();
                break;
            case Qt::LocalTime:
            default:
                // No timezone designator in the string
                if (timesys == "UTC" || timesys == "UT") {
                    // TIMESYS confirms UTC — safe to interpret as such
                    img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(),
                                            QTimeZone::utc());
                } else if (!timesys.isEmpty()) {
                    // TIMESYS present but non-UTC (TT, TAI, etc.)
                    // Treat as UTC for JD but warn
                    img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(),
                                            QTimeZone::utc());
                    img.dateObsAmbiguous = true;
                    spdlog::warn("DATE-OBS TIMESYS='{}' is not UTC in {} — assumed UTC",
                                 timesys.toStdString(), img.fileName.toStdString());
                } else {
                    // No timezone marker AND no TIMESYS — could be local time
                    img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(),
                                            QTimeZone::utc());
                    img.dateObsAmbiguous = true;
                    spdlog::warn("DATE-OBS has no timezone marker and TIMESYS is absent in {}"
                                 " — assumed UTC", img.fileName.toStdString());
                }
                break;
            }
        }
    }

    // Pixel scale — try CD matrix first, then CDELT1/CDELT2 (older FITS convention)
    const double cd1_1 = getDbl("CD1_1");
    const double cd1_2 = getDbl("CD1_2");
    const double cd2_1 = getDbl("CD2_1");
    const double cd2_2 = getDbl("CD2_2");
    if (cd1_1 != 0.0 || cd2_2 != 0.0) {
        // WCS already encodes the binned scale; no correction needed here
        img.pixScaleX = std::abs(std::hypot(cd1_1, cd2_1)) * 3600.0;
        img.pixScaleY = std::abs(std::hypot(cd1_2, cd2_2)) * 3600.0;

        auto& wcs = img.wcs;
        wcs.cd1_1  = cd1_1;
        wcs.cd1_2  = cd1_2;
        wcs.cd2_1  = cd2_1;
        wcs.cd2_2  = cd2_2;
        wcs.crval1 = getDbl("CRVAL1");
        wcs.crval2 = getDbl("CRVAL2");
        wcs.crpix1 = getDbl("CRPIX1");
        wcs.crpix2 = getDbl("CRPIX2");
        wcs.solved = true;
    } else {
        // CDELT convention (deg/px); PIXSCALE key (arcsec/px) used by some software
        const double cdelt1 = getDbl("CDELT1", 0.0);
        const double cdelt2 = getDbl("CDELT2", 0.0);
        const double pixscale = getDbl("PIXSCALE", 0.0);
        if (pixscale > 0.0) {
            img.pixScaleX = img.pixScaleY = pixscale;
        } else if (cdelt1 != 0.0 || cdelt2 != 0.0) {
            img.pixScaleX = std::abs(cdelt1) * 3600.0;
            img.pixScaleY = std::abs(cdelt2) * 3600.0;
        }
        // WCS from CDELT/CROTA — populate if present so overlay works
        const double crval1 = getDbl("CRVAL1", 0.0);
        const double crval2 = getDbl("CRVAL2", 0.0);
        if (crval1 != 0.0 || crval2 != 0.0) {
            const double crota = getDbl("CROTA2", getDbl("CROTA1", 0.0));
            const double cosR  = std::cos(crota * M_PI / 180.0);
            const double sinR  = std::sin(crota * M_PI / 180.0);
            auto& wcs  = img.wcs;
            wcs.crval1 = crval1;
            wcs.crval2 = crval2;
            wcs.crpix1 = getDbl("CRPIX1");
            wcs.crpix2 = getDbl("CRPIX2");
            if (cdelt1 != 0.0 || cdelt2 != 0.0) {
                wcs.cd1_1  =  cdelt1 * cosR;
                wcs.cd1_2  = -cdelt2 * sinR;
                wcs.cd2_1  =  cdelt1 * sinR;
                wcs.cd2_2  =  cdelt2 * cosR;
            }
            wcs.solved = (wcs.cd1_1 != 0.0 || wcs.cd2_2 != 0.0);
        }
    }

    // Observer site location — try several common keyword conventions
    // Priority: SITELAT/SITELONG → LAT-OBS/LONG-OBS → OBSGEO-B/OBSGEO-L
    auto tryDbl = [&](std::initializer_list<const char*> keys) -> double {
        for (const char* k : keys) {
            const double v = getDbl(k, std::numeric_limits<double>::quiet_NaN());
            if (!std::isnan(v)) return v;
        }
        return std::numeric_limits<double>::quiet_NaN();
    };
    img.siteLat = tryDbl({"SITELAT",  "LAT-OBS",  "OBSGEO-B", "LATITUDE"});
    img.siteLon = tryDbl({"SITELONG", "LONG-OBS",  "OBSGEO-L", "LONGITUD"});
    img.siteAlt = tryDbl({"SITEELEV", "ALT-OBS",  "OBSGEO-H", "ALTITUDE"});

    if (img.jd == 0.0 && img.dateObs.isValid()) {
        // Approximate JD from datetime
        const QDateTime j2000(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone::utc());
        img.jd = 2451545.0 + static_cast<double>(j2000.secsTo(img.dateObs)) / 86400.0 + img.expTime / 172800.0;
    }
}

} // anonymous namespace

std::expected<FitsImage, QString> loadFits(const QString& filePath)
{
    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();

    fitsfile* fptr = nullptr;
    int status = 0;

    if (fits_open_image(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status)) {
        return std::unexpected(QObject::tr("Cannot open FITS file: %1 (%2)")
            .arg(filePath, cfitsioError(status)));
    }

    struct Guard { fitsfile* f; int& s; ~Guard() { fits_close_file(f, &s); } } guard{fptr, status};

    // Image dimensions
    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    if (naxis < 2) {
        return std::unexpected(QObject::tr("FITS file has no 2D image: %1").arg(filePath));
    }

    long naxes[3] = {1, 1, 1};
    fits_get_img_size(fptr, 3, naxes, &status);
    img.width  = static_cast<int>(naxes[0]);
    img.height = static_cast<int>(naxes[1]);

    if (img.width <= 0 || img.height <= 0) {
        return std::unexpected(QObject::tr("Invalid image dimensions in: %1").arg(filePath));
    }

    // Read pixel data as float (cfitsio handles BZERO/BSCALE)
    const long npix = static_cast<long>(img.width) * img.height;
    img.data.resize(static_cast<size_t>(npix));

    long fpixel[3] = {1, 1, 1};
    float nullval = 0.0f;
    int   anynull = 0;
    fits_read_pix(fptr, TFLOAT, fpixel, npix, &nullval, img.data.data(), &anynull, &status);
    if (status) {
        return std::unexpected(QObject::tr("Error reading pixel data from: %1 (%2)")
            .arg(filePath, cfitsioError(status)));
    }

    // Read header metadata
    status = 0;
    readHeader(fptr, img);

    // Auto-stretch for display
    computeAutoStretch(img);

    spdlog::info("Loaded FITS: {}  {}x{}  RA={:.4f} Dec={:.4f}  JD={:.6f}",
        img.fileName.toStdString(), img.width, img.height, img.ra, img.dec, img.jd);

    return img;
}

void computeAutoStretch(FitsImage& img, float sigmaLow, float sigmaHigh)
{
    if (img.data.empty()) return;

    // Sample for statistics (use at most 100k pixels for speed)
    std::vector<float> sample;
    const size_t npix = img.data.size();
    const size_t step = std::max<size_t>(1, npix / 100000);
    sample.reserve(npix / step + 1);
    for (size_t i = 0; i < npix; i += step)
        sample.push_back(img.data[i]);

    // Median (background estimate)
    const size_t mid = sample.size() / 2;
    std::nth_element(sample.begin(), sample.begin() + static_cast<std::ptrdiff_t>(mid), sample.end());
    const float median = sample[mid];

    // Sigma (MAD * 1.4826)
    std::vector<float> absDev(sample.size());
    std::transform(sample.begin(), sample.end(), absDev.begin(),
        [median](float v) { return std::abs(v - median); });
    const size_t mid2 = absDev.size() / 2;
    std::nth_element(absDev.begin(), absDev.begin() + static_cast<std::ptrdiff_t>(mid2), absDev.end());
    const float sigma = absDev[mid2] * 1.4826f;

    img.displayMin = median - sigmaLow  * sigma;
    img.displayMax = median + sigmaHigh * sigma;

    spdlog::debug("AutoStretch: median={:.1f} sigma={:.1f} min={:.1f} max={:.1f}",
        median, sigma, img.displayMin, img.displayMax);
}

QString saveWcsToFits(const QString& filePath, const PlateSolution& wcs)
{
    if (!wcs.solved) return QStringLiteral("WCS not solved");

    fitsfile* fptr = nullptr;
    int status = 0;

    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READWRITE, &status)) {
        return cfitsioError(status);
    }

    auto writeD = [&](const char* key, double val, const char* comment) {
        fits_update_key(fptr, TDOUBLE, key, &val, comment, &status);
    };

    writeD("CRVAL1", wcs.crval1, "RA at reference pixel [deg]");
    writeD("CRVAL2", wcs.crval2, "Dec at reference pixel [deg]");
    writeD("CRPIX1", wcs.crpix1, "Reference pixel X");
    writeD("CRPIX2", wcs.crpix2, "Reference pixel Y");
    writeD("CD1_1",  wcs.cd1_1,  "CD matrix element");
    writeD("CD1_2",  wcs.cd1_2,  "CD matrix element");
    writeD("CD2_1",  wcs.cd2_1,  "CD matrix element");
    writeD("CD2_2",  wcs.cd2_2,  "CD matrix element");
    fits_update_key(fptr, TSTRING, "CTYPE1", const_cast<char*>("RA---TAN"),  "WCS type", &status);
    fits_update_key(fptr, TSTRING, "CTYPE2", const_cast<char*>("DEC--TAN"),  "WCS type", &status);

    const int saveStatus = status;
    fits_close_file(fptr, &status);
    if (saveStatus) return cfitsioError(saveStatus);
    return {};
}

} // namespace core
