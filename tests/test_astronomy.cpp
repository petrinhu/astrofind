#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Astronomy.h"
#include "core/FitsImage.h"

#include <cmath>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

// ─── GMST ─────────────────────────────────────────────────────────────────────

TEST_CASE("GMST at J2000.0 matches IAU 1982 reference value", "[astronomy][gmst]")
{
    // J2000.0 = JD 2451545.0 (noon TT, 1 Jan 2000)
    // IAU 1982: GMST(J2000.0) = 280.46061837° (T=0 term)
    const double gmst = core::greenwichMeanSiderealTime(2451545.0);
    REQUIRE_THAT(gmst, WithinAbs(280.46061837, 0.0001));
}

TEST_CASE("GMST advances ~0.985 degrees per solar day (mod 360)", "[astronomy][gmst]")
{
    // One Julian day = 360.98564736629° of sidereal rotation.
    // After fmod to [0,360): the net advance is 0.98564736629°.
    const double jd0 = 2451545.0;
    const double jd1 = jd0 + 1.0;
    const double g0  = core::greenwichMeanSiderealTime(jd0);
    const double g1  = core::greenwichMeanSiderealTime(jd1);
    // Both values are in [0,360); g1 > g0 by ~0.985°
    const double diff = std::fmod(g1 - g0 + 360.0, 360.0);
    REQUIRE_THAT(diff, WithinAbs(0.98564736629, 0.0001));
}

TEST_CASE("GMST stays in [0, 360)", "[astronomy][gmst]")
{
    // Test several JDs including far-future and far-past
    for (double jd : {2400000.0, 2451545.0, 2460000.0, 2500000.0}) {
        const double g = core::greenwichMeanSiderealTime(jd);
        REQUIRE(g >= 0.0);
        REQUIRE(g < 360.0);
    }
}

// ─── LST ──────────────────────────────────────────────────────────────────────

TEST_CASE("LST at Greenwich lon=0 equals GMST", "[astronomy][lst]")
{
    const double jd   = 2451545.0;
    const double gmst = core::greenwichMeanSiderealTime(jd);
    const double lst  = core::localSiderealTime(jd, 0.0);
    REQUIRE_THAT(lst, WithinAbs(gmst, 1e-9));
}

TEST_CASE("LST offset by longitude", "[astronomy][lst]")
{
    const double jd  = 2451545.0;
    const double lst = core::localSiderealTime(jd, 45.0);
    const double expected = std::fmod(core::greenwichMeanSiderealTime(jd) + 45.0, 360.0);
    REQUIRE_THAT(lst, WithinAbs(expected, 1e-9));
}

// ─── Airmass ──────────────────────────────────────────────────────────────────

TEST_CASE("Airmass = 1.0 at zenith (object on meridian at same dec as lat)", "[astronomy][airmass]")
{
    // Place observer at lat=30°N, lon=0°.
    // Place object at dec=30°, RA = LST (transit → hour angle = 0 → alt = 90°).
    const double lat = 30.0, lon = 0.0;
    const double jd  = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);

    // Object at transit: RA = LST, dec = lat → altitude = 90°
    const double x = core::computeAirmass(lst, lat, jd, lat, lon);
    REQUIRE_THAT(x, WithinAbs(1.0, 0.01));
}

TEST_CASE("Airmass ≈ 2.0 at altitude 30 degrees", "[astronomy][airmass]")
{
    // Simple geometric check: 1/sin(30°) = 2.0 exactly.
    // The Pickering correction at 30° is < 0.001, so result ≈ 2.000.
    // Build a geometry: observer at equator (lat=0), object at dec=0.
    // Altitude = 90° − hour_angle when dec=lat=0 doesn't work simply.
    // Instead use: alt = arcsin(sin(lat)sin(dec) + cos(lat)cos(dec)cos(HA))
    // With lat=0, dec=0: alt = arcsin(cos(HA)) = 90° − |HA|
    // For alt=30°: HA=60° → RA = LST − 60°
    const double lat = 0.0, lon = 0.0;
    const double jd  = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);
    const double ra  = std::fmod(lst - 60.0 + 360.0, 360.0);  // HA = 60° → alt ≈ 30°

    const double x = core::computeAirmass(ra, 0.0, jd, lat, lon);
    REQUIRE_THAT(x, WithinAbs(2.0, 0.01));
}

TEST_CASE("Airmass = 0 for object below horizon", "[astronomy][airmass]")
{
    // Object at dec = −89° observed from lat = +45°N → always below horizon at transit alt ≈ −44°
    // Use transit (HA=0): alt = arcsin(sin(45)sin(−89) + cos(45)cos(−89)) ≈ −44°
    const double lat = 45.0, lon = 0.0;
    const double jd  = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);
    const double x   = core::computeAirmass(lst, -89.0, jd, lat, lon);
    REQUIRE(x == 0.0);
}

// ─── Angular distance ─────────────────────────────────────────────────────────

TEST_CASE("Angular distance of identical points is zero", "[astronomy][angdist]")
{
    REQUIRE_THAT(core::angularDistance(180.0, 45.0, 180.0, 45.0), WithinAbs(0.0, 1e-10));
}

TEST_CASE("Angular distance on equator 90 degrees apart", "[astronomy][angdist]")
{
    REQUIRE_THAT(core::angularDistance(0.0, 0.0, 90.0, 0.0), WithinAbs(90.0, 1e-9));
}

TEST_CASE("Angular distance pole to equator is 90 degrees", "[astronomy][angdist]")
{
    REQUIRE_THAT(core::angularDistance(0.0, 90.0, 0.0, 0.0), WithinAbs(90.0, 1e-9));
}

TEST_CASE("Angular distance is symmetric", "[astronomy][angdist]")
{
    const double d1 = core::angularDistance(30.0, 20.0, 200.0, -15.0);
    const double d2 = core::angularDistance(200.0, -15.0, 30.0, 20.0);
    REQUIRE_THAT(d1, WithinAbs(d2, 1e-12));
}

// ─── Proper motion ────────────────────────────────────────────────────────────

TEST_CASE("Zero PM returns original position", "[astronomy][pm]")
{
    core::CatalogStar s;
    s.ra = 100.0; s.dec = 20.0; s.pmRA = 0.0; s.pmDec = 0.0;
    s.refEpochJd = 2451545.0;

    auto [ra, dec] = core::applyProperMotion(s, 2460000.0);
    REQUIRE_THAT(ra,  WithinAbs(100.0, 1e-12));
    REQUIRE_THAT(dec, WithinAbs(20.0,  1e-12));
}

TEST_CASE("Pure RA proper motion shifts RA correctly", "[astronomy][pm]")
{
    // pmRA = 3600000 mas/yr = 1°/yr in the RA·cos(dec) direction.
    // At dec=0: dRA = 1°/yr. After exactly 1 year (365.25 days):
    core::CatalogStar s;
    s.ra = 0.0; s.dec = 0.0;
    s.pmRA  = 3600000.0;   // mas/yr
    s.pmDec = 0.0;
    s.refEpochJd = 2451545.0;

    auto [ra, dec] = core::applyProperMotion(s, 2451545.0 + 365.25);
    REQUIRE_THAT(ra,  WithinAbs(1.0, 1e-6));
    REQUIRE_THAT(dec, WithinAbs(0.0, 1e-12));
}

TEST_CASE("Pure Dec proper motion shifts Dec correctly", "[astronomy][pm]")
{
    // pmDec = 3600000 mas/yr = 1°/yr. After 1 year: dec increases by 1°.
    core::CatalogStar s;
    s.ra = 0.0; s.dec = 0.0;
    s.pmRA  = 0.0;
    s.pmDec = 3600000.0;   // mas/yr
    s.refEpochJd = 2451545.0;

    auto [ra, dec] = core::applyProperMotion(s, 2451545.0 + 365.25);
    REQUIRE_THAT(ra,  WithinAbs(0.0, 1e-12));
    REQUIRE_THAT(dec, WithinAbs(1.0, 1e-6));
}

TEST_CASE("Barnard's Star PM: 25-year propagation matches known shift", "[astronomy][pm]")
{
    // Barnard's Star (SIMBAD J2000):
    //   RA = 269.45402°, Dec = +4.693453°
    //   pmRA = -798.58 mas/yr, pmDec = +10328.12 mas/yr (in RA·cos(dec) convention)
    // Expected shift over 25 years to J2025 (JD 2451545 + 25*365.25 = 2460696.25):
    //   dDec ≈ +0.07172° (+4.3')
    //   dRA  ≈ −0.005564° (significant given large pmRA)

    core::CatalogStar s;
    s.ra         = 269.45402;
    s.dec        = 4.693453;
    s.pmRA       = -798.58;
    s.pmDec      = 10328.12;
    s.refEpochJd = 2451545.0;  // J2000.0

    const double jd2025 = 2451545.0 + 25.0 * 365.25;
    auto [ra, dec] = core::applyProperMotion(s, jd2025);

    REQUIRE_THAT(dec - s.dec, WithinAbs(+0.07172, 0.00005));
    REQUIRE_THAT(ra  - s.ra,  WithinAbs(-0.005564, 0.000005));
}

// ─── WCS round-trip ───────────────────────────────────────────────────────────

TEST_CASE("WCS pixToSky / skyToPix round-trip is identity", "[astronomy][wcs]")
{
    // 1 arcsec/pixel plate scale, reference pixel at centre of a 1024×1024 image
    core::PlateSolution wcs;
    wcs.solved = true;
    wcs.crval1 = 180.0; wcs.crval2 = 0.0;
    wcs.crpix1 = 512.0; wcs.crpix2 = 512.0;
    wcs.cd1_1  = -1.0 / 3600.0;  // −1 arcsec/px in RA direction
    wcs.cd1_2  = 0.0;
    wcs.cd2_1  = 0.0;
    wcs.cd2_2  = 1.0 / 3600.0;   // +1 arcsec/px in Dec direction

    // Test several pixels across the image
    for (auto [px0, py0] : std::initializer_list<std::pair<double,double>>{
            {0.0, 0.0}, {511.5, 511.5}, {1023.0, 1023.0}, {200.0, 700.0}}) {
        double ra, dec, px1, py1;
        wcs.pixToSky(px0, py0, ra, dec);
        wcs.skyToPix(ra, dec, px1, py1);
        REQUIRE_THAT(px1, WithinAbs(px0, 1e-9));
        REQUIRE_THAT(py1, WithinAbs(py0, 1e-9));
    }
}
