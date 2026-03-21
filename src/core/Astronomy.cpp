#include "Astronomy.h"

#include <cmath>

namespace core {

// ─── greenwichMeanSiderealTime ────────────────────────────────────────────────

double greenwichMeanSiderealTime(double jd) noexcept
{
    // IAU 1982 formula (Astronomical Almanac, B6):
    //   GMST = 280.46061837 + 360.98564736629·(JD − 2451545.0)
    //          + 0.000387933·T² − T³/38710000
    // where T = (JD − 2451545.0) / 36525.
    const double T    = (jd - 2451545.0) / 36525.0;
    const double gmst = 280.46061837
                      + 360.98564736629 * (jd - 2451545.0)
                      + 0.000387933 * T * T
                      - T * T * T / 38710000.0;
    return std::fmod(std::fmod(gmst, 360.0) + 360.0, 360.0);  // normalise to [0, 360)
}

// ─── localSiderealTime ────────────────────────────────────────────────────────

double localSiderealTime(double jd, double lonDeg) noexcept
{
    return std::fmod(greenwichMeanSiderealTime(jd) + lonDeg + 3600.0 * 360.0, 360.0);
}

// ─── computeAirmass ───────────────────────────────────────────────────────────

double computeAirmass(double ra_deg, double dec_deg, double jd,
                      double lat_deg, double lon_deg) noexcept
{
    const double lst_deg = localSiderealTime(jd, lon_deg);
    const double ha_rad  = (lst_deg - ra_deg) * M_PI / 180.0;
    const double dec_rad = dec_deg * M_PI / 180.0;
    const double lat_rad = lat_deg * M_PI / 180.0;

    const double sinAlt = std::sin(lat_rad) * std::sin(dec_rad)
                        + std::cos(lat_rad) * std::cos(dec_rad) * std::cos(ha_rad);
    const double alt_deg = std::asin(std::clamp(sinAlt, -1.0, 1.0)) * 180.0 / M_PI;

    if (alt_deg <= 0.0) return 0.0;

    // Pickering (2002): X = 1 / sin(alt + 244 / (165 + 47·alt^2.575))
    const double x = alt_deg + 244.0 / (165.0 + 47.0 * std::pow(alt_deg, 2.575));
    return 1.0 / std::sin(x * M_PI / 180.0);
}

// ─── angularDistance ─────────────────────────────────────────────────────────

double angularDistance(double ra1_deg, double dec1_deg,
                       double ra2_deg, double dec2_deg) noexcept
{
    // Haversine formula — stable at all separations including near-zero and 180°
    const double dra  = (ra2_deg  - ra1_deg)  * M_PI / 180.0;
    const double ddec = (dec2_deg - dec1_deg) * M_PI / 180.0;
    const double dec1 = dec1_deg * M_PI / 180.0;
    const double dec2 = dec2_deg * M_PI / 180.0;

    const double a = std::sin(ddec * 0.5) * std::sin(ddec * 0.5)
                   + std::cos(dec1) * std::cos(dec2)
                   * std::sin(dra * 0.5) * std::sin(dra * 0.5);
    const double c = 2.0 * std::asin(std::sqrt(std::clamp(a, 0.0, 1.0)));
    return c * 180.0 / M_PI;
}

// ─── applyProperMotion ────────────────────────────────────────────────────────

std::pair<double,double> applyProperMotion(const CatalogStar& star,
                                            double targetJd) noexcept
{
    if (star.pmRA == 0.0 && star.pmDec == 0.0)
        return { star.ra, star.dec };

    const double dtYears = (targetJd - star.refEpochJd) / 365.25;

    // pmRA is in mas/yr in the RA·cos(δ) direction → convert to degrees
    const double cosDec = std::cos(star.dec * M_PI / 180.0);
    const double dRa    = (cosDec > 1e-10)
        ? star.pmRA  * dtYears / (3600000.0 * cosDec)
        : 0.0;
    const double dDec   = star.pmDec * dtYears / 3600000.0;

    return { star.ra + dRa, star.dec + dDec };
}

} // namespace core
