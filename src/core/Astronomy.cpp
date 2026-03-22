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

// ─── applyRefractionCorrection ───────────────────────────────────────────────

double applyRefractionCorrection(double& ra, double& dec,
                                  double jd, double lat, double lon) noexcept
{
    if (!std::isfinite(lat) || !std::isfinite(lon)) return 0.0;

    constexpr double kD2R = M_PI / 180.0;
    constexpr double kR2D = 180.0 / M_PI;

    const double lst     = localSiderealTime(jd, lon);
    const double ha_rad  = (lst - ra)  * kD2R;
    const double dec_rad = dec * kD2R;
    const double lat_rad = lat * kD2R;

    // ── Apparent altitude and azimuth ─────────────────────────────────────────
    const double sinAlt  = std::sin(lat_rad) * std::sin(dec_rad)
                         + std::cos(lat_rad) * std::cos(dec_rad) * std::cos(ha_rad);
    const double alt_app = std::asin(std::clamp(sinAlt, -1.0, 1.0)) * kR2D;

    if (alt_app < 1.0) return 0.0;   // too close to horizon — formula unstable

    const double cosAlt  = std::cos(alt_app * kD2R);
    const double sinAz   = -std::cos(dec_rad) * std::sin(ha_rad) / cosAlt;
    const double cosAz   = (std::sin(dec_rad) - std::sin(lat_rad) * sinAlt)
                         / (std::cos(lat_rad) * cosAlt);
    const double az_rad  = std::atan2(sinAz, cosAz);

    // ── Bennett (1982) refraction ─────────────────────────────────────────────
    // R = 1.02 / tan(h + 10.3/(h+5.11))  [arcmin]
    const double hArg   = alt_app + 10.3 / (alt_app + 5.11);
    const double R_arcmin = 1.02 / std::tan(hArg * kD2R);
    const double R_deg    = R_arcmin / 60.0;

    // ── True altitude (remove refraction: atmosphere pushes objects up) ───────
    const double alt_true_rad = (alt_app - R_deg) * kD2R;
    const double cosAltTrue   = std::cos(alt_true_rad);

    // ── Convert (az, alt_true) back to equatorial ─────────────────────────────
    const double sinDecTrue = std::sin(lat_rad) * std::sin(alt_true_rad)
                            + std::cos(lat_rad) * cosAltTrue * std::cos(az_rad);
    const double dec_true   = std::asin(std::clamp(sinDecTrue, -1.0, 1.0)) * kR2D;
    const double cosDecTrue = std::cos(dec_true * kD2R);
    if (cosDecTrue < 1e-10) return 0.0;   // at the pole — skip

    const double sinHaTrue = -sinAz * cosAltTrue / cosDecTrue;
    const double cosHaTrue = (std::sin(alt_true_rad) - std::sin(lat_rad) * sinDecTrue)
                           / (std::cos(lat_rad) * cosDecTrue);
    const double ha_true_deg = std::atan2(sinHaTrue, cosHaTrue) * kR2D;

    ra  = std::fmod(lst - ha_true_deg + 720.0, 360.0);
    dec = dec_true;

    return R_arcmin;
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

// ─── eclipticToEquatorial ─────────────────────────────────────────────────────

void eclipticToEquatorial(double lambda_deg, double beta_deg,
                          double& ra_deg,  double& dec_deg) noexcept
{
    constexpr double eps = 23.4392911 * M_PI / 180.0;   // obliquity J2000
    const double lam = lambda_deg * M_PI / 180.0;
    const double bet = beta_deg   * M_PI / 180.0;

    const double sinDec = std::sin(bet) * std::cos(eps)
                        + std::cos(bet) * std::sin(eps) * std::sin(lam);
    dec_deg = std::asin(std::clamp(sinDec, -1.0, 1.0)) * 180.0 / M_PI;

    const double y = std::sin(lam) * std::cos(eps) - std::tan(bet) * std::sin(eps);
    const double x = std::cos(lam);
    ra_deg = std::fmod(std::atan2(y, x) * 180.0 / M_PI + 360.0, 360.0);
}

// ─── equatorialToGalactic ─────────────────────────────────────────────────────

void equatorialToGalactic(double ra_deg, double dec_deg,
                          double& l_deg, double& b_deg) noexcept
{
    // IAU 1958 galactic pole in J2000 equatorial
    constexpr double kAlphaGP = 192.859508 * M_PI / 180.0;  // RA of NGP
    constexpr double kDeltaGP =  27.128336 * M_PI / 180.0;  // Dec of NGP
    constexpr double kLcp     = 122.932;                     // l of NCP (degrees)

    const double a   = ra_deg  * M_PI / 180.0;
    const double d   = dec_deg * M_PI / 180.0;
    const double da  = a - kAlphaGP;

    const double sinB = std::sin(d) * std::sin(kDeltaGP)
                      + std::cos(d) * std::cos(kDeltaGP) * std::cos(da);
    b_deg = std::asin(std::clamp(sinB, -1.0, 1.0)) * 180.0 / M_PI;

    const double y = std::cos(d) * std::sin(da);
    const double x = std::sin(d) * std::cos(kDeltaGP)
                   - std::cos(d) * std::sin(kDeltaGP) * std::cos(da);
    l_deg = std::fmod(kLcp - std::atan2(y, x) * 180.0 / M_PI + 720.0, 360.0);
}

// ─── sunEclipticPosition ─────────────────────────────────────────────────────

std::pair<double,double> sunEclipticPosition(double jd) noexcept
{
    // Meeus, Ch. 25 — truncated VSOP87, accuracy ~0.01°
    const double T  = (jd - 2451545.0) / 36525.0;
    const double L0 = 280.46646 + 36000.76983 * T + 0.0003032 * T * T;
    const double M  = (357.52911 + 35999.05029 * T - 0.0001537 * T * T) * M_PI / 180.0;
    const double C  = (1.914602 - 0.004817 * T - 0.000014  * T * T) * std::sin(M)
                    + (0.019993 - 0.000101 * T)                       * std::sin(2.0 * M)
                    + 0.000289                                          * std::sin(3.0 * M);
    const double lambda = std::fmod(L0 + C + 3600.0, 360.0);  // geometric ecliptic long.

    // Mean obliquity (Laskar 1986, truncated to T³, degrees)
    const double eps = 23.439291111
                     - 0.013004167 * T
                     - 0.000000164 * T * T
                     + 0.000000504 * T * T * T;

    return { lambda, eps };
}

// ─── annualAberrationComponents ──────────────────────────────────────────────

double annualAberrationComponents(double ra, double dec, double jd,
                                  double& dRa_as, double& dDec_as) noexcept
{
    const auto [lambda, eps] = sunEclipticPosition(jd);

    constexpr double kappa = 20.49552;   // IAU constant of aberration, arcseconds
    constexpr double kD2R  = M_PI / 180.0;

    const double a   = ra  * kD2R;
    const double d   = dec * kD2R;
    const double lam = lambda * kD2R;
    const double ep  = eps   * kD2R;

    const double cosD = std::cos(d);
    if (std::abs(cosD) < 1e-10) { dRa_as = dDec_as = 0.0; return 0.0; }

    // Meeus (1998) eq. 23.3 — Δα, Δδ to ADD to astrometric to get apparent.
    // Signs: apparent = astrometric + Δ  →  astrometric = apparent − Δ
    dRa_as  = -kappa * (std::cos(a) * std::cos(lam) * std::cos(ep)
                      + std::sin(a) * std::sin(lam)) / cosD;

    dDec_as = -kappa * (std::cos(lam) * std::cos(ep)
                          * (std::tan(ep) * cosD - std::sin(a) * std::sin(d))
                      + std::cos(a) * std::sin(d) * std::sin(lam));

    return std::hypot(dRa_as * cosD, dDec_as);   // total angular shift, arcsec
}

// ─── applyPrecessionJ2000ToDate ───────────────────────────────────────────────

void applyPrecessionJ2000ToDate(double& ra, double& dec, double jd) noexcept
{
    // Lieske et al. (1977) precession angles (degrees).
    // Accuracy < 0.1" per century; consistent with IAU 2000 at J2000 epoch.
    const double T  = (jd - 2451545.0) / 36525.0;
    const double T2 = T * T;
    const double T3 = T * T2;

    const double zetaA  = (0.6406161 * T + 0.0000839 * T2 + 0.0000050 * T3) * M_PI / 180.0;
    const double zA     = (0.6406161 * T + 0.0003041 * T2 + 0.0000051 * T3) * M_PI / 180.0;
    const double thetaA = (0.5567530 * T - 0.0001185 * T2 - 0.0000116 * T3) * M_PI / 180.0;

    // 3-rotation matrix application: R_z(-zA) · R_y(θ_A) · R_z(-ζ_A)
    // Applied directly to spherical coordinates (Meeus, Ch. 21):
    const double a  = ra * M_PI / 180.0;
    const double d  = dec * M_PI / 180.0;

    const double A = std::cos(d) * std::sin(a + zetaA);
    const double B = std::cos(thetaA) * std::cos(d) * std::cos(a + zetaA)
                   - std::sin(thetaA) * std::sin(d);
    const double C = std::sin(thetaA) * std::cos(d) * std::cos(a + zetaA)
                   + std::cos(thetaA) * std::sin(d);

    ra  = std::fmod(std::atan2(A, B) * 180.0 / M_PI + zA * 180.0 / M_PI + 3600.0, 360.0);
    dec = std::asin(std::clamp(C, -1.0, 1.0)) * 180.0 / M_PI;
}

// ─── applyNutation ────────────────────────────────────────────────────────────

void applyNutation(double& ra, double& dec, double jd) noexcept
{
    // 9-term IAU 1980 nutation series (Seidelmann 1982, dominant terms).
    // Converts mean-equinox-of-date → true (apparent) equinox of date.
    const double T  = (jd - 2451545.0) / 36525.0;
    constexpr double kD2R = M_PI / 180.0;

    // Fundamental arguments (degrees)
    const double Omega = std::fmod(125.04452 - 1934.136261 * T + 3600.0 * 10.0, 360.0);
    const double Lsun  = std::fmod(280.4664567 + 36000.7698278 * T + 3600.0 * 100.0, 360.0);
    const double Lmoon = std::fmod(218.3165778 + 481267.8813028 * T + 3600.0 * 100.0, 360.0);
    const double Msun  = std::fmod(357.52772   + 35999.050340 * T + 3600.0 * 100.0, 360.0);

    const double W = Omega * kD2R;
    const double L = Lsun  * kD2R;
    const double M = Lmoon * kD2R;
    const double G = Msun  * kD2R;

    // Δψ (nutation in longitude, arcseconds) — 9 leading terms
    const double dpsi =  -17.2062 * std::sin(W)
                         - 1.3187 * std::sin(2.0 * L)
                         - 0.2274 * std::sin(2.0 * M)
                         + 0.2062 * std::sin(2.0 * W)
                         + 0.1426 * std::sin(G)
                         - 0.0517 * std::sin(G - W)   // M_sun - Omega
                         + 0.0217 * std::sin(2.0 * L + G)
                         - 0.0153 * std::sin(2.0 * L - W)
                         + 0.0129 * std::sin(2.0 * W - G);

    // Δε (nutation in obliquity, arcseconds)
    const double deps = + 9.2025 * std::cos(W)
                        + 0.5736 * std::cos(2.0 * L)
                        + 0.0977 * std::cos(2.0 * M)
                        - 0.0895 * std::cos(2.0 * W)
                        + 0.0224 * std::cos(G)
                        + 0.0200 * std::cos(G - W)
                        - 0.0128 * std::cos(2.0 * L + G)
                        + 0.0115 * std::cos(2.0 * L - W)
                        - 0.0093 * std::cos(2.0 * W - G);

    // Mean obliquity (degrees)
    const double eps0 = 23.439291111 - 0.013004167 * T
                      - 0.000000164 * T * T + 0.000000504 * T * T * T;
    const double eps  = (eps0 + deps / 3600.0) * kD2R;   // true obliquity in radians

    // Equatorial nutation corrections (Meeus, Ch. 23, eq. 23.1):
    //   Δα = Δψ(cos ε + sin ε · sin α · tan δ) − Δε · cos α · tan δ
    //   Δδ = Δψ · sin ε · cos α + Δε · sin α
    const double a  = ra  * kD2R;
    const double d  = dec * kD2R;

    const double dra_as  = dpsi * (std::cos(eps) + std::sin(eps) * std::sin(a) * std::tan(d))
                         - deps * std::cos(a) * std::tan(d);
    const double ddec_as = dpsi * std::sin(eps) * std::cos(a)
                         + deps * std::sin(a);

    // dpsi, deps are in arcseconds → convert to degrees
    ra  += dra_as  / 3600.0;
    dec += ddec_as / 3600.0;
}

} // namespace core
