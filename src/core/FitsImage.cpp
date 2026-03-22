#include "FitsImage.h"

#include <fitsio.h>
#include <spdlog/spdlog.h>

#include <QFileInfo>
#include <QRegularExpression>
#include <QTimeZone>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>

namespace core {

// ─── PlateSolution — full WCS pipeline ───────────────────────────────────────
//
// Standard 3-step FITS WCS pipeline (Calabretta & Greisen 2002, A&A 395, 1077):
//   Step 1: pixel → intermediate world coords (IWC, degrees) via CD matrix
//   Step 2: IWC (x,y) → native spherical (φ,θ, degrees) — projection-dependent
//   Step 3: native spherical → celestial (α,δ, degrees) — spherical rotation
//
// Projection formulas adapted from WCSLIB 8.2 (prj.c, Calabretta, CSIRO ATNF).
// r₀ = 180/π ≈ 57.2958° is the standard WCS sphere radius in degrees.

namespace {

constexpr double D2R = M_PI / 180.0;
constexpr double R2D = 180.0 / M_PI;
constexpr double R0  = R2D;  // sphere radius r₀ in degrees

// φ_p: native longitude of the celestial pole.
// Zenithal projections (θ₀=90°): φ_p = 180° (WCS Paper II §2.4, eq. 82).
// Cylindrical/conventional (θ₀=0°): φ_p = 90° if δ_p ≥ 0, else 270°.
inline double nativePoleAngle(WcsProjection proj, double crval2)
{
    switch (proj) {
        case WcsProjection::TAN:
        case WcsProjection::SIN:
        case WcsProjection::ARC:
        case WcsProjection::STG:
            return 180.0;
        default:
            return crval2 >= 0.0 ? 90.0 : 270.0;
    }
}

/// Step 2 forward: IWC (x,y) in degrees → native spherical (φ,θ) in degrees.
/// Returns false when the coordinates are outside the valid domain.
bool iwcToNative(double x, double y, WcsProjection proj,
                 double& phi, double& theta) noexcept
{
    const double R = std::hypot(x, y);  // distance from IWC origin (degrees)
    switch (proj) {

    case WcsProjection::TAN:
        // TAN (gnomonic): R = r0/tan(θ), φ=atan2(x,-y)
        phi   = std::atan2(x, -y) * R2D;
        theta = std::atan2(R0, R) * R2D;
        return true;

    case WcsProjection::SIN:
        // SIN (orthographic): R = r0*cos(θ), θ=asin(sqrt(1-(R/r0)²))
        if (R > R0) return false;  // off-disk
        phi   = (R > 0.0) ? std::atan2(x, -y) * R2D : 0.0;
        theta = std::asin(std::sqrt(std::max(0.0, 1.0 - (R / R0) * (R / R0)))) * R2D;
        return true;

    case WcsProjection::ARC:
        // ARC (zenithal equidistant): R = r0*(90°-θ) → θ = 90° - R
        phi   = (R > 0.0) ? std::atan2(x, -y) * R2D : 0.0;
        theta = 90.0 - R;
        return true;

    case WcsProjection::STG:
        // STG (stereographic): R = 2*r0*cos(θ)/(1+sin(θ)) → θ = 90°-2*atan(R/(2r0))
        phi   = (R > 0.0) ? std::atan2(x, -y) * R2D : 0.0;
        theta = 90.0 - 2.0 * std::atan(R / (2.0 * R0)) * R2D;
        return true;

    case WcsProjection::CAR:
        // CAR (plate carrée): IWC maps directly to native (φ=x, θ=y)
        phi   = x;
        theta = y;
        return true;

    case WcsProjection::MER:
        // MER (Mercator): x=r0*φ, y=r0*ln(tan((θ+90)/2))
        phi   = x;
        theta = 2.0 * std::atan(std::exp(y / R0)) * R2D - 90.0;
        return true;

    case WcsProjection::GLS:
        // GLS/SFL (global sinusoidal): x=r0*φ*cos(θ), y=r0*θ
        theta = y;
        {
            const double cosTheta = std::cos(theta * D2R);
            phi = (std::abs(cosTheta) > 1e-12) ? x / (R0 * cosTheta) * R2D : 0.0;
        }
        // Clamp to [-180,180]
        phi = std::clamp(phi, -180.0, 180.0);
        return true;

    case WcsProjection::AIT:
        // AIT (Hammer-Aitoff): see Calabretta & Greisen (2002) A.6
        {
            const double z2 = 1.0 - (x * x) / (4.0 * R0 * R0 * 4.0)
                                   - (y * y) / (4.0 * R0 * R0);
            if (z2 < 0.5) return false;  // outside ellipse
            const double z = std::sqrt(z2);
            // φ = 2 * atan2(z*x, 2*r0*(2z²-1))  (Calabretta & Greisen 2002, eq. A6)
            phi   = 2.0 * std::atan2(z * x, 2.0 * (2.0 * z * z - 1.0) * R0) * R2D;
            theta = std::asin(z * y / R0) * R2D;
        }
        return true;
    }
    return false;  // unreachable
}

/// Step 2 inverse: native spherical (φ,θ) in degrees → IWC (x,y) in degrees.
bool nativeToIwc(double phi, double theta, WcsProjection proj,
                 double& x, double& y) noexcept
{
    const double phi_r   = phi   * D2R;
    const double theta_r = theta * D2R;
    switch (proj) {

    case WcsProjection::TAN:
        if (std::abs(theta) < 1e-10) return false;  // horizon
        {
            const double R = R0 * std::cos(theta_r) / std::sin(theta_r);
            x =  R * std::sin(phi_r);
            y = -R * std::cos(phi_r);
        }
        return true;

    case WcsProjection::SIN:
        {
            const double R = R0 * std::cos(theta_r);
            x =  R * std::sin(phi_r);
            y = -R * std::cos(phi_r);
        }
        return true;

    case WcsProjection::ARC:
        {
            const double R = 90.0 - theta;  // degrees
            x =  R * std::sin(phi_r);
            y = -R * std::cos(phi_r);
        }
        return true;

    case WcsProjection::STG:
        if (theta <= -90.0) return false;
        {
            const double R = 2.0 * R0 * std::cos(theta_r) / (1.0 + std::sin(theta_r));
            x =  R * std::sin(phi_r);
            y = -R * std::cos(phi_r);
        }
        return true;

    case WcsProjection::CAR:
        x = phi;
        y = theta;
        return true;

    case WcsProjection::MER:
        {
            const double arg = (90.0 + theta) / 2.0 * D2R;
            if (std::abs(theta) >= 90.0) return false;
            x = R0 * phi_r;
            y = R0 * std::log(std::tan(arg));
        }
        return true;

    case WcsProjection::GLS:
        x = R0 * phi_r * std::cos(theta_r);
        y = theta;  // IWC y is in degrees = r0 * (theta * D2R) ... here IWC = native directly
        return true;

    case WcsProjection::AIT:
        {
            // z = sqrt(2/(1+cos(θ)cos(φ/2)))
            const double halfPhi = phi_r / 2.0;
            const double denom = 1.0 + std::cos(theta_r) * std::cos(halfPhi);
            if (denom < 1e-12) return false;
            const double w = R0 * std::sqrt(2.0 / denom);
            x = 2.0 * w * std::cos(theta_r) * std::sin(halfPhi);
            y = w * std::sin(theta_r);
        }
        return true;
    }
    return false;
}

/// Step 3: native spherical (φ,θ) → celestial (α,δ), all in degrees.
/// phi_p = native longitude of the celestial pole.
void nativeToCelestial(double phi, double theta,
                       double alpha_p, double delta_p, double phi_p,
                       double& alpha, double& delta) noexcept
{
    const double phi_r     = (phi - phi_p) * D2R;
    const double theta_r   = theta          * D2R;
    const double delta_p_r = delta_p        * D2R;
    const double cTheta = std::cos(theta_r);
    const double sTheta = std::sin(theta_r);
    const double cDelta_p = std::cos(delta_p_r);
    const double sDelta_p = std::sin(delta_p_r);
    const double cosDphi  = std::cos(phi_r);
    const double sinDphi  = std::sin(phi_r);

    delta = std::asin(std::clamp(sTheta * sDelta_p + cTheta * cDelta_p * cosDphi, -1.0, 1.0)) * R2D;
    alpha = alpha_p + std::atan2(-cTheta * sinDphi,
                                  sTheta * cDelta_p - cTheta * sDelta_p * cosDphi) * R2D;
    // Normalise RA to [0, 360)
    alpha = std::fmod(alpha, 360.0);
    if (alpha < 0.0) alpha += 360.0;
}

/// Step 3 inverse: celestial (α,δ) → native spherical (φ,θ), all in degrees.
void celestialToNative(double alpha, double delta,
                       double alpha_p, double delta_p, double phi_p,
                       double& phi, double& theta) noexcept
{
    const double da       = (alpha - alpha_p) * D2R;
    const double delta_r  = delta   * D2R;
    const double delta_p_r = delta_p * D2R;
    const double cDelta  = std::cos(delta_r);
    const double sDelta  = std::sin(delta_r);
    const double cDelta_p = std::cos(delta_p_r);
    const double sDelta_p = std::sin(delta_p_r);
    const double cosDa   = std::cos(da);
    const double sinDa   = std::sin(da);

    theta = std::asin(std::clamp(sDelta * sDelta_p + cDelta * cDelta_p * cosDa, -1.0, 1.0)) * R2D;
    phi   = phi_p + std::atan2(-cDelta * sinDa,
                                sDelta * cDelta_p - cDelta * sDelta_p * cosDa) * R2D;
}

}  // anonymous namespace

void PlateSolution::pixToSky(double px, double py, double& ra, double& dec) const noexcept
{
    // Step 1: pixel → IWC (degrees)
    const double dx = px - crpix1;
    const double dy = py - crpix2;
    const double x  = cd1_1 * dx + cd1_2 * dy;
    const double y  = cd2_1 * dx + cd2_2 * dy;

    // Step 2: IWC → native spherical
    double phi = 0.0, theta = 0.0;
    if (!iwcToNative(x, y, projection, phi, theta)) {
        ra = crval1; dec = crval2; return;
    }

    // Step 3: native spherical → celestial
    const double phi_p = nativePoleAngle(projection, crval2);
    nativeToCelestial(phi, theta, crval1, crval2, phi_p, ra, dec);
}

void PlateSolution::skyToPix(double ra, double dec, double& px, double& py) const noexcept
{
    // Step 3 inverse: celestial → native spherical
    const double phi_p = nativePoleAngle(projection, crval2);
    double phi = 0.0, theta = 0.0;
    celestialToNative(ra, dec, crval1, crval2, phi_p, phi, theta);

    // Step 2 inverse: native spherical → IWC
    double x = 0.0, y = 0.0;
    if (!nativeToIwc(phi, theta, projection, x, y)) {
        px = crpix1; py = crpix2; return;
    }

    // Step 1 inverse: IWC → pixel via CD⁻¹
    const double det = cd1_1 * cd2_2 - cd1_2 * cd2_1;
    if (std::abs(det) < 1e-15) { px = py = 0.0; return; }
    px = crpix1 + ( cd2_2 * x - cd1_2 * y) / det;
    py = crpix2 + (-cd2_1 * x + cd1_1 * y) / det;
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
    img.origin     = getStr("ORIGIN");
    img.observer   = getStr("OBSERVER");

    // MPC observatory code: explicit header first, then derive from TELESCOP/ORIGIN
    img.mpcCode = getStr("MPCCODE");
    if (img.mpcCode.isEmpty()) img.mpcCode = getStr("OBSCODE");
    if (img.mpcCode.isEmpty()) img.mpcCode = getStr("OBSERVAT");
    if (img.mpcCode.isEmpty()) {
        // Known telescope→MPC mappings for professional surveys
        const QString tel = img.telescope.toUpper();
        const QString ori = img.origin.toUpper();
        if (tel == "PS1" || tel == "GPC1" || ori == "PS1")      img.mpcCode = "F51";
        else if (tel == "PS2" || tel == "GPC2")                  img.mpcCode = "F52";
        else if (ori.contains("CFHT"))                           img.mpcCode = "568";
        else if (ori.contains("ESO") && tel.contains("VLT"))     img.mpcCode = "309";
    }

    // ── Space telescope detection ─────────────────────────────────────────────
    // Check TELESCOP (and ORIGIN as fallback) for known space missions.
    // mpcCode is only set here when we have a confirmed MPC code for that mission.
    {
        struct SpacecraftEntry { const char* keyword; const char* mpcCode; };
        static const SpacecraftEntry kSpacecraft[] = {
            { "HST",        "250" },   // Hubble Space Telescope
            { "HUBBLE",     "250" },
            { "JWST",       "C57" },   // James Webb Space Telescope
            { "JAMES WEBB", "C57" },
            { "SOHO",       "249" },   // Solar and Heliospheric Observatory
            { "SPITZER",    ""    },   // Spitzer Space Telescope
            { "SIRTF",      ""    },   // Spitzer pre-launch name
            { "CHANDRA",    ""    },   // Chandra X-ray Observatory
            { "XMM",        ""    },   // XMM-Newton
            { "KEPLER",     ""    },   // Kepler Space Telescope
            { "TESS",       ""    },   // Transiting Exoplanet Survey Satellite
            { "WISE",       ""    },   // Wide-field Infrared Survey Explorer
            { "NEOWISE",    ""    },
            { "GALEX",      ""    },   // Galaxy Evolution Explorer
            { "EUCLID",     ""    },   // Euclid (ESA)
            { "HERSCHEL",   ""    },   // Herschel Space Observatory
            { "PLANCK",     ""    },   // Planck
            { "FERMI",      ""    },   // Fermi Gamma-ray Space Telescope
        };

        const QString tel = img.telescope.toUpper();
        const QString ori = img.origin.toUpper();

        for (const auto& sc : kSpacecraft) {
            if (tel.contains(QLatin1String(sc.keyword))) {
                img.isSpaceTelescope = true;
                if (img.mpcCode.isEmpty() && sc.mpcCode[0] != '\0')
                    img.mpcCode = QLatin1String(sc.mpcCode);
                break;
            }
        }

        // Fallback: recognise STScI (operates HST and JWST) and ESA/ESAC
        if (!img.isSpaceTelescope) {
            if (ori.contains("STSCI") || ori.contains("SPACE TELESCOPE SCIENCE"))
                img.isSpaceTelescope = true;
            else if (ori.contains("ESAC") || ori == "ESA")
                img.isSpaceTelescope = true;
        }
    }
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
                                            Qt::UTC);
                } else if (!timesys.isEmpty()) {
                    // TIMESYS present but non-UTC (TT, TAI, etc.)
                    // Treat as UTC for JD but warn
                    img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(),
                                            Qt::UTC);
                    img.dateObsAmbiguous = true;
                    spdlog::warn("DATE-OBS TIMESYS='{}' is not UTC in {} — assumed UTC",
                                 timesys.toStdString(), img.fileName.toStdString());
                } else {
                    // No timezone marker AND no TIMESYS — could be local time
                    img.dateObs = QDateTime(img.dateObs.date(), img.dateObs.time(),
                                            Qt::UTC);
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
    // Parse projection type from CTYPE1 (e.g. "RA---TAN", "RA---SIN", "RA---AIT")
    auto parseCtype = [](const QString& ctype) -> WcsProjection {
        const QString ct = ctype.toUpper();
        if (ct.contains(QLatin1String("SIN"))) return WcsProjection::SIN;
        if (ct.contains(QLatin1String("ARC"))) return WcsProjection::ARC;
        if (ct.contains(QLatin1String("STG"))) return WcsProjection::STG;
        if (ct.contains(QLatin1String("CAR"))) return WcsProjection::CAR;
        if (ct.contains(QLatin1String("MER"))) return WcsProjection::MER;
        if (ct.contains(QLatin1String("GLS")) || ct.contains(QLatin1String("SFL")))
            return WcsProjection::GLS;
        if (ct.contains(QLatin1String("AIT"))) return WcsProjection::AIT;
        return WcsProjection::TAN;  // default — covers "RA---TAN" and empty
    };

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
        wcs.projection = parseCtype(getStr("CTYPE1"));
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
            wcs.projection = parseCtype(getStr("CTYPE1"));
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
        const QDateTime j2000(QDate(2000, 1, 1), QTime(12, 0, 0), Qt::UTC);
        img.jd = 2451545.0 + static_cast<double>(j2000.secsTo(img.dateObs)) / 86400.0 + img.expTime / 172800.0;
    }
}

/// Returns {R_hdu, G_hdu, B_hdu} (1-indexed cfitsio HDU numbers) when the file
/// has exactly 3 NAXIS=2 image extensions with matching dimensions.
/// Returns nullopt when the file should be loaded as a single image.
std::optional<std::array<int,3>> findMultiExtRgbHdus(fitsfile* fptr)
{
    int totalHdus = 0;
    int st = 0;
    fits_get_num_hdus(fptr, &totalHdus, &st);
    if (st || totalHdus < 3) return std::nullopt;

    struct ExtInfo { int hduNum; int w, h; QString extname; QString filter; };
    std::vector<ExtInfo> imgs;

    for (int h = 1; h <= totalHdus; ++h) {
        st = 0;
        fits_movabs_hdu(fptr, h, nullptr, &st);
        if (st) { continue; }

        int hdutype = ANY_HDU;
        fits_get_hdu_type(fptr, &hdutype, &st);
        if (st || hdutype != IMAGE_HDU) { continue; }

        int naxis = 0;
        fits_get_img_dim(fptr, &naxis, &st);
        if (st || naxis != 2) { continue; }   // must be exactly 2D (single plane)

        long naxes[2] = {1, 1};
        fits_get_img_size(fptr, 2, naxes, &st);
        if (st || naxes[0] <= 0 || naxes[1] <= 0) { continue; }

        char valstr[FLEN_VALUE] = {};
        char comment[FLEN_COMMENT] = {};
        int kst = 0;
        QString extname, filter;
        kst = 0; fits_read_keyword(fptr, "EXTNAME", valstr, comment, &kst);
        if (!kst) extname = QString::fromLatin1(valstr).remove('\'').trimmed();
        kst = 0; fits_read_keyword(fptr, "FILTER", valstr, comment, &kst);
        if (!kst) filter  = QString::fromLatin1(valstr).remove('\'').trimmed();

        imgs.push_back({h, static_cast<int>(naxes[0]), static_cast<int>(naxes[1]),
                        extname, filter});
    }

    if (imgs.size() != 3) return std::nullopt;

    // All three must share the same dimensions
    if (imgs[0].w != imgs[1].w || imgs[0].w != imgs[2].w ||
        imgs[0].h != imgs[1].h || imgs[0].h != imgs[2].h)
        return std::nullopt;

    // Default ordering: positional (first=R, second=G, third=B)
    std::array<int,3> result = {imgs[0].hduNum, imgs[1].hduNum, imgs[2].hduNum};

    // Try to refine ordering from EXTNAME / FILTER keywords
    auto channelOf = [](const QString& extname, const QString& filter) -> int {
        const QString s = (extname.isEmpty() ? filter : extname).toUpper().trimmed();
        if (s == "R" || s.startsWith("RED")  || s.startsWith("H_A")) return 0;
        if (s == "G" || s.startsWith("GRE")  || s == "V")            return 1;
        if (s == "B" || s.startsWith("BLU"))                          return 2;
        return -1;
    };

    std::array<int,3> labeled = {-1, -1, -1};
    int identified = 0;
    for (const auto& e : imgs) {
        const int ch = channelOf(e.extname, e.filter);
        if (ch >= 0 && labeled[ch] == -1) { labeled[ch] = e.hduNum; ++identified; }
    }
    if (identified == 3) result = labeled;

    return result;
}

} // anonymous namespace

std::expected<FitsImage, QString> loadFits(const QString& filePath)
{
    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();

    fitsfile* fptr = nullptr;
    int status = 0;

    // Use fits_open_file (not fits_open_image) so we can inspect all HDUs first
    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status)) {
        return std::unexpected(QObject::tr("Cannot open FITS file: %1 (%2)")
            .arg(filePath, cfitsioError(status)));
    }

    struct Guard { fitsfile* f; int& s; ~Guard() { fits_close_file(f, &s); } } guard{fptr, status};

    // ── Multi-extension RGB probe ─────────────────────────────────────────────
    const auto multiRgb = findMultiExtRgbHdus(fptr);
    status = 0;

    if (multiRgb.has_value()) {
        const auto& hdus = *multiRgb;   // [R_hdu, G_hdu, B_hdu]

        // Move to R extension to read dimensions and header metadata
        fits_movabs_hdu(fptr, hdus[0], nullptr, &status);
        long axes[2] = {1, 1};
        fits_get_img_size(fptr, 2, axes, &status);
        if (status || axes[0] <= 0 || axes[1] <= 0)
            return std::unexpected(QObject::tr("Invalid image dimensions in: %1").arg(filePath));

        img.width   = static_cast<int>(axes[0]);
        img.height  = static_cast<int>(axes[1]);
        img.isColor = true;
        readHeader(fptr, img);
        status = 0;

        const long planeSize = static_cast<long>(img.width) * img.height;
        img.dataR.resize(static_cast<size_t>(planeSize));
        img.dataG.resize(static_cast<size_t>(planeSize));
        img.dataB.resize(static_cast<size_t>(planeSize));

        float nullval = 0.0f;
        int   anynull = 0;
        auto readPlane = [&](int hduNum, std::vector<float>& plane) {
            status = 0;
            fits_movabs_hdu(fptr, hduNum, nullptr, &status);
            long fpixel[2] = {1, 1};
            fits_read_pix(fptr, TFLOAT, fpixel, planeSize,
                          &nullval, plane.data(), &anynull, &status);
        };
        readPlane(hdus[0], img.dataR);
        readPlane(hdus[1], img.dataG);
        readPlane(hdus[2], img.dataB);
        if (status)
            return std::unexpected(QObject::tr("Error reading multi-extension color FITS: %1 (%2)")
                .arg(filePath, cfitsioError(status)));

        // Compute Rec.709 luminance for astrometry / star detection
        const size_t ps = static_cast<size_t>(planeSize);
        img.data.resize(ps);
        for (size_t i = 0; i < ps; ++i)
            img.data[i] = 0.2126f * img.dataR[i] + 0.7152f * img.dataG[i] + 0.0722f * img.dataB[i];

        computeAutoStretch(img);

        spdlog::info("Loaded FITS (multi-ext RGB HDUs {}/{}/{}): {}  {}x{}  "
                     "RA={:.4f} Dec={:.4f}  JD={:.6f}",
            hdus[0], hdus[1], hdus[2],
            img.fileName.toStdString(), img.width, img.height,
            img.ra, img.dec, img.jd);

        return img;
    }

    // ── Single-image path ─────────────────────────────────────────────────────
    // Navigate to the first IMAGE_HDU with NAXIS ≥ 2 (replicates fits_open_image behaviour)
    {
        int totalHdus = 0;
        fits_get_num_hdus(fptr, &totalHdus, &status);
        status = 0;
        bool found = false;
        for (int h = 1; h <= totalHdus; ++h) {
            fits_movabs_hdu(fptr, h, nullptr, &status);
            status = 0;
            int hdutype = ANY_HDU;
            fits_get_hdu_type(fptr, &hdutype, &status);
            if (status || hdutype != IMAGE_HDU) { status = 0; continue; }
            int nax = 0;
            fits_get_img_dim(fptr, &nax, &status);
            if (!status && nax >= 2) { found = true; break; }
            status = 0;
        }
        if (!found)
            return std::unexpected(QObject::tr("FITS file has no 2D image: %1").arg(filePath));
        status = 0;
    }

    // Image dimensions
    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    if (naxis < 2)
        return std::unexpected(QObject::tr("FITS file has no 2D image: %1").arg(filePath));

    long naxes[3] = {1, 1, 1};
    fits_get_img_size(fptr, 3, naxes, &status);
    img.width   = static_cast<int>(naxes[0]);
    img.height  = static_cast<int>(naxes[1]);
    img.isColor    = (naxes[2] == 3);  ///< NAXIS3=3 → RGB color image
    img.cubeDepth  = static_cast<int>(naxes[2]);

    if (img.width <= 0 || img.height <= 0)
        return std::unexpected(QObject::tr("Invalid image dimensions in: %1").arg(filePath));

    // Read pixel data as float (cfitsio handles BZERO/BSCALE)
    const long planeSize = static_cast<long>(img.width) * img.height;
    const long npix      = img.isColor ? planeSize * 3 : planeSize;
    long fpixel[3] = {1, 1, 1};
    float nullval = 0.0f;
    int   anynull = 0;

    if (img.isColor) {
        std::vector<float> allPlanes(static_cast<size_t>(npix));
        fits_read_pix(fptr, TFLOAT, fpixel, npix, &nullval, allPlanes.data(), &anynull, &status);
        if (status)
            return std::unexpected(QObject::tr("Error reading color pixel data from: %1 (%2)")
                .arg(filePath, cfitsioError(status)));
        const size_t ps = static_cast<size_t>(planeSize);
        img.dataR.assign(allPlanes.begin(),        allPlanes.begin() + ps);
        img.dataG.assign(allPlanes.begin() + ps,   allPlanes.begin() + ps * 2);
        img.dataB.assign(allPlanes.begin() + ps*2, allPlanes.end());
        img.data.resize(ps);
        for (size_t i = 0; i < ps; ++i)
            img.data[i] = 0.2126f * img.dataR[i] + 0.7152f * img.dataG[i] + 0.0722f * img.dataB[i];
    } else {
        img.data.resize(static_cast<size_t>(planeSize));
        fits_read_pix(fptr, TFLOAT, fpixel, planeSize, &nullval, img.data.data(), &anynull, &status);
        if (status)
            return std::unexpected(QObject::tr("Error reading pixel data from: %1 (%2)")
                .arg(filePath, cfitsioError(status)));
    }

    // Read header metadata
    status = 0;
    readHeader(fptr, img);

    // Auto-stretch for display
    computeAutoStretch(img);

    spdlog::info("Loaded FITS: {}  {}x{}  {}  RA={:.4f} Dec={:.4f}  JD={:.6f}",
        img.fileName.toStdString(), img.width, img.height,
        img.isColor ? "RGB" : "grayscale", img.ra, img.dec, img.jd);

    return img;
}

// ─── scanImageHdus ────────────────────────────────────────────────────────────

QVector<HduInfo> scanImageHdus(const QString& filePath)
{
    fitsfile* fptr = nullptr;
    int status = 0;
    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status))
        return {};

    int totalHdus = 0;
    fits_get_num_hdus(fptr, &totalHdus, &status);

    QVector<HduInfo> result;
    char valstr[FLEN_VALUE] = {};
    char comment[FLEN_COMMENT] = {};

    for (int h = 1; h <= totalHdus; ++h) {
        status = 0;
        fits_movabs_hdu(fptr, h, nullptr, &status);
        if (status) continue;

        int hdutype = ANY_HDU;
        fits_get_hdu_type(fptr, &hdutype, &status);
        if (status || hdutype != IMAGE_HDU) continue;

        int naxis = 0;
        fits_get_img_dim(fptr, &naxis, &status);
        if (status || naxis < 2) continue;

        long naxes[3] = {1, 1, 1};
        fits_get_img_size(fptr, 3, naxes, &status);
        if (status || naxes[0] <= 0 || naxes[1] <= 0) continue;

        int kst = 0;
        fits_read_keyword(fptr, "EXTNAME", valstr, comment, &kst);
        QString name = kst ? QStringLiteral("HDU %1").arg(h)
                           : QString::fromLatin1(valstr).remove('\'').trimmed();
        if (name.isEmpty()) name = QStringLiteral("HDU %1").arg(h);

        result.push_back({h, static_cast<int>(naxes[0]), static_cast<int>(naxes[1]),
                          static_cast<int>(naxes[2]), name});
    }

    fits_close_file(fptr, &status);
    return result;
}

// ─── loadFitsHdu ─────────────────────────────────────────────────────────────

std::expected<FitsImage, QString> loadFitsHdu(const QString& filePath, int hduNumber)
{
    FitsImage img;
    img.filePath = filePath;
    img.fileName = QFileInfo(filePath).fileName();

    fitsfile* fptr = nullptr;
    int status = 0;

    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status))
        return std::unexpected(QObject::tr("Cannot open FITS file: %1 (%2)")
            .arg(filePath, cfitsioError(status)));

    struct Guard { fitsfile* f; int& s; ~Guard() { fits_close_file(f, &s); } } guard{fptr, status};

    fits_movabs_hdu(fptr, hduNumber, nullptr, &status);
    if (status)
        return std::unexpected(QObject::tr("HDU %1 not found in: %2").arg(hduNumber).arg(filePath));

    int hdutype = ANY_HDU;
    fits_get_hdu_type(fptr, &hdutype, &status);
    if (hdutype != IMAGE_HDU)
        return std::unexpected(QObject::tr("HDU %1 is not an image extension in: %2")
            .arg(hduNumber).arg(filePath));

    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    if (naxis < 2)
        return std::unexpected(QObject::tr("HDU %1 has no 2D image in: %2")
            .arg(hduNumber).arg(filePath));

    long naxes[3] = {1, 1, 1};
    fits_get_img_size(fptr, 3, naxes, &status);
    img.width      = static_cast<int>(naxes[0]);
    img.height     = static_cast<int>(naxes[1]);
    img.isColor    = (naxes[2] == 3);
    img.cubeDepth  = static_cast<int>(naxes[2]);

    if (img.width <= 0 || img.height <= 0)
        return std::unexpected(QObject::tr("Invalid image dimensions in HDU %1: %2")
            .arg(hduNumber).arg(filePath));

    const long planeSize = static_cast<long>(img.width) * img.height;
    const long npix      = img.isColor ? planeSize * 3 : planeSize;
    long fpixel[3] = {1, 1, 1};
    float nullval = 0.0f;
    int   anynull = 0;

    if (img.isColor) {
        std::vector<float> allPlanes(static_cast<size_t>(npix));
        fits_read_pix(fptr, TFLOAT, fpixel, npix, &nullval, allPlanes.data(), &anynull, &status);
        if (status)
            return std::unexpected(QObject::tr("Error reading pixel data from HDU %1: %2 (%3)")
                .arg(hduNumber).arg(filePath, cfitsioError(status)));
        const size_t ps = static_cast<size_t>(planeSize);
        img.dataR.assign(allPlanes.begin(),        allPlanes.begin() + ps);
        img.dataG.assign(allPlanes.begin() + ps,   allPlanes.begin() + ps * 2);
        img.dataB.assign(allPlanes.begin() + ps*2, allPlanes.end());
        img.data.resize(ps);
        for (size_t i = 0; i < ps; ++i)
            img.data[i] = 0.2126f * img.dataR[i] + 0.7152f * img.dataG[i] + 0.0722f * img.dataB[i];
    } else {
        img.data.resize(static_cast<size_t>(planeSize));
        fits_read_pix(fptr, TFLOAT, fpixel, planeSize, &nullval, img.data.data(), &anynull, &status);
        if (status)
            return std::unexpected(QObject::tr("Error reading pixel data from HDU %1: %2 (%3)")
                .arg(hduNumber).arg(filePath, cfitsioError(status)));
    }

    status = 0;
    readHeader(fptr, img);
    computeAutoStretch(img);

    spdlog::info("Loaded FITS HDU {}: {}  {}x{}  {}",
        hduNumber, img.fileName.toStdString(), img.width, img.height,
        img.isColor ? "RGB" : "grayscale");

    return img;
}

// ─── loadFitsCube ─────────────────────────────────────────────────────────────

std::expected<QVector<FitsImage>, QString>
loadFitsCube(const QString& filePath, int hduNumber)
{
    fitsfile* fptr = nullptr;
    int status = 0;
    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status))
        return std::unexpected(QObject::tr("Cannot open '%1': %2")
            .arg(filePath, cfitsioError(status)));
    struct Guard { fitsfile* f; int& s; ~Guard() { fits_close_file(f, &s); } } guard{fptr, status};

    if (hduNumber > 1) {
        fits_movabs_hdu(fptr, hduNumber, nullptr, &status);
        if (status)
            return std::unexpected(QObject::tr("Cannot move to HDU %1 in '%2'")
                .arg(hduNumber).arg(filePath));
    }

    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    long naxes[3] = {1, 1, 1};
    fits_get_img_size(fptr, 3, naxes, &status);
    const int W = static_cast<int>(naxes[0]);
    const int H = static_cast<int>(naxes[1]);
    const int D = static_cast<int>(naxes[2]);

    if (W <= 0 || H <= 0)
        return std::unexpected(QObject::tr("Invalid image dimensions in '%1'").arg(filePath));
    if (D <= 1 || D == 3)
        return std::unexpected(QObject::tr("'%1' is not a temporal cube (NAXIS3=%2)")
            .arg(filePath).arg(D));

    // Read shared header metadata (once for all planes)
    FitsImage meta;
    meta.filePath  = filePath;
    meta.width     = W;
    meta.height    = H;
    meta.cubeDepth = D;
    readHeader(fptr, meta);

    const QString baseName = QFileInfo(filePath).fileName();
    const long planeSize   = static_cast<long>(W) * H;
    float nullval = 0.0f;
    int   anynull = 0;

    QVector<FitsImage> frames;
    frames.reserve(D);
    for (int k = 0; k < D; ++k) {
        FitsImage frame = meta;
        frame.data.resize(static_cast<size_t>(planeSize));
        long fpixel[3] = {1, 1, static_cast<long>(k + 1)};
        int st = 0;
        fits_read_pix(fptr, TFLOAT, fpixel, planeSize, &nullval,
                      frame.data.data(), &anynull, &st);
        if (st) {
            // On read error keep the frame with zeros so the animation still plays
            frame.data.assign(static_cast<size_t>(planeSize), 0.0f);
        }
        frame.fileName = QStringLiteral("%1 [%2/%3]").arg(baseName).arg(k + 1).arg(D);
        computeAutoStretch(frame);
        frames.append(std::move(frame));
    }
    spdlog::info("loadFitsCube: '{}' — {} frames {}×{}",
                 baseName.toStdString(), D, W, H);
    return frames;
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
