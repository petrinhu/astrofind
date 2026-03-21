#include "Ephemeris.h"
#include "MpcOrb.h"

#include <QFile>
#include <QTextStream>

#include <cmath>
#include <numbers>

namespace core {

// ─── Constants ───────────────────────────────────────────────────────────────

static constexpr double kPI    = std::numbers::pi;
static constexpr double kTwoPI = 2.0 * kPI;
static constexpr double kD2R   = kPI / 180.0;  // degrees → radians
static constexpr double kR2D   = 180.0 / kPI;  // radians → degrees
static constexpr double kAU    = 1.495978707e11; // metres per AU (unused here, kept for reference)
static constexpr double kObliq = 23.43928 * kD2R; // mean obliquity of ecliptic at J2000 (radians)

// ─── solveKepler ─────────────────────────────────────────────────────────────

double solveKepler(double M_rad, double e, int maxIter) noexcept
{
    // Wrap M into [0, 2π)
    M_rad = std::fmod(M_rad, kTwoPI);
    if (M_rad < 0.0) M_rad += kTwoPI;

    // Initial guess
    double E = (e < 0.8) ? M_rad : kPI;

    for (int i = 0; i < maxIter; ++i) {
        const double dE = (M_rad - E + e * std::sin(E)) / (1.0 - e * std::cos(E));
        E += dE;
        if (std::abs(dE) < 1.0e-12)
            break;
    }
    return E;
}

// ─── Low-precision Sun position (Meeus, Astronomical Algorithms Ch.25) ───────
// Returns heliocentric position of Earth (= geocentric Sun, negated) in
// ecliptic rectangular coordinates (AU), referred to mean ecliptic J2000.

struct Vec3 { double x, y, z; };

static Vec3 earthHeliocentricEcliptic(double jd) noexcept
{
    const double T  = (jd - 2451545.0) / 36525.0;   // Julian centuries from J2000.0

    // Geometric mean longitude of the Sun (degrees)
    double L0 = 280.46646 + 36000.76983 * T + 0.0003032 * T * T;
    L0 = std::fmod(L0, 360.0);
    if (L0 < 0.0) L0 += 360.0;

    // Mean anomaly of the Sun (degrees)
    double M_sun = 357.52911 + 35999.05029 * T - 0.0001537 * T * T;
    M_sun = std::fmod(M_sun, 360.0);
    if (M_sun < 0.0) M_sun += 360.0;
    const double Mr = M_sun * kD2R;

    // Equation of centre (degrees)
    const double C = (1.914602 - 0.004817 * T - 0.000014 * T * T) * std::sin(Mr)
                   + (0.019993 - 0.000101 * T)                     * std::sin(2.0 * Mr)
                   +  0.000289                                      * std::sin(3.0 * Mr);

    // Sun's true longitude (degrees)
    const double sunLon = L0 + C;

    // Sun-Earth distance (AU)
    const double sunV = (M_sun + C) * kD2R;
    const double R    = 1.000001018 * (1.0 - 0.01671123 * 0.01671123)
                      / (1.0 + 0.01671123 * std::cos(sunV));

    // Earth's heliocentric longitude = Sun's geocentric longitude + 180°
    const double lon = (sunLon + 180.0) * kD2R;
    const double lat = 0.0;  // Earth stays ~on ecliptic plane for this approximation

    // Ecliptic → rectangular
    return { R * std::cos(lat) * std::cos(lon),
             R * std::cos(lat) * std::sin(lon),
             R * std::sin(lat) };
}

// ─── Ecliptic rectangular → equatorial rectangular ───────────────────────────

static Vec3 eclipticToEquatorial(const Vec3& ec) noexcept
{
    const double cosOb = std::cos(kObliq);
    const double sinOb = std::sin(kObliq);
    return { ec.x,
             cosOb * ec.y - sinOb * ec.z,
             sinOb * ec.y + cosOb * ec.z };
}

// ─── Topocentric correction ───────────────────────────────────────────────────
// Shifts a geocentric direction vector by the observer's geocentric position.
// obsLat degrees geodetic, obsLon degrees east, obsAlt metres above geoid.
// Returns the offset (AU) to subtract from the geocentric vector.

static Vec3 topocentricOffset(double jd, double obsLat, double obsLon, double obsAlt) noexcept
{
    // WGS84 reference ellipsoid
    static constexpr double a_earth = 6378137.0;        // metres
    static constexpr double f       = 1.0 / 298.257223563;
    static constexpr double AUm     = 1.495978707e11;   // metres per AU

    const double phi    = obsLat * kD2R;
    const double N      = a_earth / std::sqrt(1.0 - (2*f - f*f) * std::sin(phi)*std::sin(phi));
    const double rcos   = (N + obsAlt) * std::cos(phi) / AUm;   // AU
    const double rsin   = (N * (1 - (2*f - f*f)) + obsAlt) * std::sin(phi) / AUm;

    // Greenwich Apparent Sidereal Time (very low-precision, but good enough for
    // the tiny topocentric shift on a small-telescope field)
    const double T      = (jd - 2451545.0) / 36525.0;
    double GAST = 280.46061837
                + 360.98564736629 * (jd - 2451545.0)
                + 0.000387933 * T * T;
    GAST = std::fmod(GAST, 360.0);
    if (GAST < 0.0) GAST += 360.0;

    const double LST = (GAST + obsLon) * kD2R;   // Local sidereal time (radians)

    // Observer's geocentric position in equatorial rectangular (AU)
    return { rcos * std::cos(LST),
             rcos * std::sin(LST),
             rsin };
}

// ─── Angular separation (degrees) ────────────────────────────────────────────

static double angSepDeg(double ra1, double dec1, double ra2, double dec2) noexcept
{
    const double d1 = dec1 * kD2R, d2 = dec2 * kD2R;
    const double dRA = (ra1 - ra2) * kD2R;
    const double x   = std::cos(d2) * std::sin(dRA);
    const double y   = std::cos(d1) * std::sin(d2) - std::sin(d1) * std::cos(d2) * std::cos(dRA);
    const double z   = std::sin(d1) * std::sin(d2) + std::cos(d1) * std::cos(d2) * std::cos(dRA);
    return std::atan2(std::sqrt(x*x + y*y), z) * kR2D;
}

// ─── computeEphemeris ─────────────────────────────────────────────────────────

EphemerisMatch computeEphemeris(const AsteroidRecord& orb,
                                double jd,
                                double obsLat,
                                double obsLon,
                                double obsAlt) noexcept
{
    EphemerisMatch res;
    res.number = orb.number;
    res.name   = orb.name;
    res.H      = orb.H;

    // 1. Propagate mean anomaly from epoch to jd
    const double dt = jd - orb.epoch;               // days since epoch
    double M = orb.M + orb.n * dt;                  // degrees
    M = std::fmod(M, 360.0);
    if (M < 0.0) M += 360.0;

    // 2. Solve Kepler → eccentric anomaly E
    const double E = solveKepler(M * kD2R, orb.e);

    // 3. True anomaly ν
    const double sinE  = std::sin(E);
    const double cosE  = std::cos(E);
    const double sqrte = std::sqrt(1.0 - orb.e * orb.e);
    const double nu    = std::atan2(sqrte * sinE, cosE - orb.e);

    // 4. Radius vector (heliocentric distance, AU)
    const double r = orb.a * (1.0 - orb.e * cosE);
    res.rHelio = r;

    // 5. Heliocentric ecliptic position
    //    Convert angles to radians
    const double omegaR = orb.omega * kD2R;   // argument of perihelion
    const double OmegaR = orb.Omega * kD2R;   // longitude of ascending node
    const double inclR  = orb.incl  * kD2R;   // inclination

    const double u = nu + omegaR;  // argument of latitude
    const double cosu = std::cos(u), sinu = std::sin(u);
    const double cosO = std::cos(OmegaR), sinO = std::sin(OmegaR);
    const double cosi = std::cos(inclR),  sini = std::sin(inclR);

    // Ecliptic rectangular heliocentric (AU)
    const Vec3 hec {
        r * (cosO * cosu - sinO * sinu * cosi),
        r * (sinO * cosu + cosO * sinu * cosi),
        r * (sinu * sini)
    };

    // 6. Earth's heliocentric position (ecliptic rectangular, AU)
    const Vec3 earth = earthHeliocentricEcliptic(jd);

    // 7. Geocentric ecliptic vector (asteroid - Earth)
    const Vec3 gec { hec.x - earth.x, hec.y - earth.y, hec.z - earth.z };

    // 8. Convert to equatorial rectangular
    Vec3 geq = eclipticToEquatorial(gec);

    // 9. Topocentric correction (if observer location provided)
    if (obsLat != 0.0 || obsLon != 0.0 || obsAlt != 0.0) {
        const Vec3 topo = topocentricOffset(jd, obsLat, obsLon, obsAlt);
        geq.x -= topo.x;
        geq.y -= topo.y;
        geq.z -= topo.z;
    }

    // 10. Geocentric distance and RA/Dec
    const double rGeo = std::sqrt(geq.x*geq.x + geq.y*geq.y + geq.z*geq.z);
    res.rGeo = rGeo;

    double ra  = std::atan2(geq.y, geq.x) * kR2D;
    if (ra < 0.0) ra += 360.0;
    const double dec = std::asin(geq.z / rGeo) * kR2D;
    res.ra  = ra;
    res.dec = dec;

    // 11. Phase angle (Sun-asteroid-Earth angle)
    // cos(α) = (r² + Δ² - R²) / (2·r·Δ)  where R = Earth-Sun distance
    const double R_earth = std::sqrt(earth.x*earth.x + earth.y*earth.y + earth.z*earth.z);
    const double cosPhase = (r*r + rGeo*rGeo - R_earth*R_earth) / (2.0 * r * rGeo);
    const double phaseAngle = std::acos(std::clamp(cosPhase, -1.0, 1.0)) * kR2D;
    res.phaseAngle = phaseAngle;

    // 12. Apparent magnitude — H-G photometric system
    //   Φ1 = exp(-3.33 * tan(α/2)^0.63)
    //   Φ2 = exp(-1.87 * tan(α/2)^1.22)
    //   V  = H + 5·log10(r·Δ) - 2.5·log10((1-G)·Φ1 + G·Φ2)
    const double tanHalfPhase = std::tan(phaseAngle * kD2R * 0.5);
    const double Phi1 = std::exp(-3.33 * std::pow(tanHalfPhase, 0.63));
    const double Phi2 = std::exp(-1.87 * std::pow(tanHalfPhase, 1.22));
    const double G    = static_cast<double>(orb.G);
    res.mag = orb.H + 5.0 * std::log10(r * rGeo)
            - 2.5 * std::log10((1.0 - G) * Phi1 + G * Phi2);

    // 13. Sky-plane motion: compute position at jd+1hr and jd-1hr, difference / 2
    //     (arcsec/hr — first-order central difference)
    {
        const double h = 1.0 / 24.0;  // 1 hour in days

        auto posAt = [&](double jd2) -> std::pair<double,double> {
            const double dt2  = jd2 - orb.epoch;
            double M2 = orb.M + orb.n * dt2;
            M2 = std::fmod(M2, 360.0);
            if (M2 < 0.0) M2 += 360.0;
            const double E2    = solveKepler(M2 * kD2R, orb.e);
            const double sinE2 = std::sin(E2), cosE2 = std::cos(E2);
            const double nu2   = std::atan2(sqrte * sinE2, cosE2 - orb.e);
            const double r2    = orb.a * (1.0 - orb.e * cosE2);
            const double u2    = nu2 + omegaR;
            const Vec3 hec2 {
                r2 * (cosO * std::cos(u2) - sinO * std::sin(u2) * cosi),
                r2 * (sinO * std::cos(u2) + cosO * std::sin(u2) * cosi),
                r2 * (std::sin(u2) * sini)
            };
            const Vec3 e2 = earthHeliocentricEcliptic(jd2);
            const Vec3 g2 { hec2.x-e2.x, hec2.y-e2.y, hec2.z-e2.z };
            Vec3 q2 = eclipticToEquatorial(g2);
            const double d2 = std::sqrt(q2.x*q2.x + q2.y*q2.y + q2.z*q2.z);
            double ra2 = std::atan2(q2.y, q2.x) * kR2D;
            if (ra2 < 0.0) ra2 += 360.0;
            const double dec2 = std::asin(q2.z / d2) * kR2D;
            return {ra2, dec2};
        };

        auto [raP, decP] = posAt(jd + h);
        auto [raM, decM] = posAt(jd - h);

        // RA motion: scale by cos(dec) to get on-sky arcsec, then /2hr
        double dra = (raP - raM);
        // handle wrap-around near 0/360
        if (dra >  180.0) dra -= 360.0;
        if (dra < -180.0) dra += 360.0;
        res.dRA  = dra * 3600.0 * std::cos(dec * kD2R) / 2.0;  // arcsec/hr
        res.dDec = (decP - decM) * 3600.0 / 2.0;               // arcsec/hr
    }

    return res;
}

// ─── queryFieldMpcOrb ─────────────────────────────────────────────────────────

QVector<EphemerisMatch> queryFieldMpcOrb(
    const QString& mpcOrbPath,
    double ra, double dec, double radiusDeg,
    double jd,
    double obsLat, double obsLon, double obsAlt,
    const std::function<void(int,int)>& progress,
    const bool* cancelled)
{
    QVector<EphemerisMatch> results;

    QFile file(mpcOrbPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return results;

    // Count total lines for progress reporting (re-uses countMpcOrbRecords logic inline)
    const int totalLines = countMpcOrbRecords(mpcOrbPath);

    QTextStream in(&file);
    int lineIdx = 0;

    while (!in.atEnd()) {
        if (cancelled && *cancelled)
            break;

        const QString line = in.readLine();
        ++lineIdx;

        if (progress && (lineIdx % 5000 == 0))
            progress(lineIdx, totalLines);

        auto optRec = parseMpcOrbLine(line);
        if (!optRec)
            continue;

        const AsteroidRecord& rec = *optRec;

        // Quick pre-filter on magnitude if H is very faint (optional speedup)
        // (skip objects fainter than H=20 at 3 AU to save compute)
        if (rec.H > 20.0f)
            continue;

        EphemerisMatch em = computeEphemeris(rec, jd, obsLat, obsLon, obsAlt);

        if (angSepDeg(em.ra, em.dec, ra, dec) <= radiusDeg)
            results.push_back(std::move(em));
    }

    if (progress)
        progress(totalLines, totalLines);

    return results;
}

// ─── ephemerisMatchToKoo ──────────────────────────────────────────────────────

KooObject ephemerisMatchToKoo(const EphemerisMatch& m)
{
    KooObject koo;
    koo.number = m.number;
    koo.name   = m.number > 0
                 ? QStringLiteral("(%1) %2").arg(m.number).arg(m.name)
                 : m.name;
    koo.type   = QStringLiteral("asteroid");
    koo.ra     = m.ra;
    koo.dec    = m.dec;
    koo.mag    = m.mag;
    koo.dRA    = m.dRA;
    koo.dDec   = m.dDec;
    return koo;
}

} // namespace core
