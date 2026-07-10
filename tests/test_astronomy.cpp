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
        REQUIRE_THAT(px1, WithinAbs(px0, 1e-5));  // trig round-trip; 1e-5 px ≈ 10 mas
        REQUIRE_THAT(py1, WithinAbs(py0, 1e-5));
    }
}

// ─── WCS: 8-projection value-absolute oracle (AUD-CORR-2) ────────────────────
//
// The round-trip test above (pre-existing) only ever exercised TAN — the
// default projection when PlateSolution::projection is never set. Round-trip
// is a NECESSARY-but-NOT-SUFFICIENT invariant: AUD-CORR-1 showed CAR closing
// its own round-trip to 8.6e-11 px while reporting a value 90° WRONG in
// absolute RA (axes swapped). The tests below check absolute (α,δ) against
// an INDEPENDENT oracle (astropy 8.0.1 `astropy.wcs.WCS`) for all 8
// projections AstroFind supports, so an axis-swap/90°-offset regression like
// AUD-CORR-1 fails immediately instead of hiding behind a closed round-trip.
//
// Oracle generation scripts (not part of the build):
//   $SCRATCH/oracle_wcs_onda2.py, oracle_wrap3.py, oracle_pole_mer.py
// CI has no astropy — all reference values below are hard-coded doubles.

namespace {

struct ProjCase { core::WcsProjection proj; const char* name; };

constexpr ProjCase kAllProjections[] = {
    { core::WcsProjection::TAN, "TAN" },
    { core::WcsProjection::SIN, "SIN" },
    { core::WcsProjection::ARC, "ARC" },
    { core::WcsProjection::STG, "STG" },
    { core::WcsProjection::CAR, "CAR" },
    { core::WcsProjection::MER, "MER" },
    { core::WcsProjection::GLS, "GLS" },
    { core::WcsProjection::AIT, "AIT" },
};

core::PlateSolution makeWcs(core::WcsProjection proj,
                             double crval1, double crval2,
                             double crpix1 = 512.0, double crpix2 = 512.0)
{
    core::PlateSolution wcs;
    wcs.solved     = true;
    wcs.projection = proj;
    wcs.crval1 = crval1; wcs.crval2 = crval2;
    wcs.crpix1 = crpix1; wcs.crpix2 = crpix2;
    wcs.cd1_1  = -1.0 / 3600.0;  // 1"/px, RA flips with +X (standard CCD convention)
    wcs.cd1_2  = 0.0;
    wcs.cd2_1  = 0.0;
    wcs.cd2_2  =  1.0 / 3600.0;
    return wcs;
}

} // namespace

TEST_CASE("WCS: reference pixel maps to CRVAL sub-arcsec, all 8 projections", "[astronomy][wcs]")
{
    for (const auto& pc : kAllProjections) {
        INFO("projection = " << pc.name);
        auto wcs = makeWcs(pc.proj, 180.0, 0.0);
        double ra = 0.0, dec = 0.0;
        wcs.pixToSky(wcs.crpix1, wcs.crpix2, ra, dec);
        CHECK_THAT(ra,  WithinAbs(180.0, 1e-6));   // 1e-6° ≈ 3.6 mas — sub-arcsec
        CHECK_THAT(dec, WithinAbs(0.0,   1e-6));
    }
}

TEST_CASE("WCS: pixToSky/skyToPix round-trip identity, all 8 projections", "[astronomy][wcs]")
{
    for (const auto& pc : kAllProjections) {
        INFO("projection = " << pc.name);
        auto wcs = makeWcs(pc.proj, 180.0, 0.0);
        for (auto [px0, py0] : std::initializer_list<std::pair<double,double>>{
                {512.0, 512.0}, {612.0, 512.0}, {512.0, 612.0}, {462.0, 542.0}}) {
            double ra, dec, px1, py1;
            wcs.pixToSky(px0, py0, ra, dec);
            wcs.skyToPix(ra, dec, px1, py1);
            CHECK_THAT(px1, WithinAbs(px0, 1e-4));
            CHECK_THAT(py1, WithinAbs(py0, 1e-4));
        }
    }
}

TEST_CASE("WCS: off-axis pixels match astropy oracle, all 8 projections", "[astronomy][wcs]")
{
    // crval=(180,0), crpix=(512,512), 1"/px. Offsets (dx,dy) in pixels from
    // CRPIX; expected (RA,Dec) in degrees from astropy 8.0.1 (full precision).
    struct Off { double dx, dy, ra, dec; };
    struct Case { core::WcsProjection proj; const char* name; Off offs[3]; };

    static const Case kCases[] = {
        { core::WcsProjection::TAN, "TAN", {
            {100.0, 0.0, 179.97222222439854, 3.5032515425613497e-15},
            {0.0, 100.0, 180.0, 0.027777775601464327},
            {-50.0, 30.0, 180.01388888861686, 0.008333333029741109} } },
        { core::WcsProjection::SIN, "SIN", {
            {100.0, 0.0, 179.97222222113405, 3.5032515419615752e-15},
            {0.0, 100.0, 180.0, 0.02777777886595345},
            {-50.0, 30.0, 180.0138888891718, 0.008333333362712083} } },
        { core::WcsProjection::ARC, "ARC", {
            {100.0, 0.0, 179.97222222222223, 3.503251542161503e-15},
            {0.0, 100.0, 180.0, 0.027777777777771462},
            {-50.0, 30.0, 180.01388888898683, 0.008333333251732666} } },
        { core::WcsProjection::STG, "STG", {
            {100.0, 0.0, 179.9722222227663, 3.5032515422614646e-15},
            {0.0, 100.0, 180.0, 0.027777777233694678},
            {-50.0, 30.0, 180.01388888889431, 0.008333333196226598} } },
        { core::WcsProjection::CAR, "CAR", {
            {100.0, 0.0, 179.97222222222223, 0.0},
            {0.0, 100.0, 180.0, 0.027777777777777776},
            {-50.0, 30.0, 180.01388888888889, 0.008333333333333333} } },
        { core::WcsProjection::MER, "MER", {
            {100.0, 0.0, 179.97222222222223, 0.0},
            {0.0, 100.0, 180.0, 0.027777776689617895},
            {-50.0, 30.0, 180.01388888888889, 0.008333333303951918} } },
        { core::WcsProjection::GLS, "GLS", {
            {100.0, 0.0, 179.97222222222223, 0.0},
            {0.0, 100.0, 180.0, 0.027777777777777776},
            {-50.0, 30.0, 180.0138888890358, 0.008333333333333333} } },
        { core::WcsProjection::AIT, "AIT", {
            {100.0, 0.0, 179.97222222215422, 0.0},
            {0.0, 100.0, 180.0, 0.027777778049819807},
            {-50.0, 30.0, 180.01388888900757, 0.008333333325376105} } },
    };

    for (const auto& c : kCases) {
        INFO("projection = " << c.name);
        auto wcs = makeWcs(c.proj, 180.0, 0.0);
        for (const auto& o : c.offs) {
            double ra = 0.0, dec = 0.0;
            wcs.pixToSky(wcs.crpix1 + o.dx, wcs.crpix2 + o.dy, ra, dec);
            CHECK_THAT(ra,  WithinAbs(o.ra,  1e-6));
            CHECK_THAT(dec, WithinAbs(o.dec, 1e-6));
        }
    }
}

TEST_CASE("WCS: negative Dec field matches astropy oracle", "[astronomy][wcs]")
{
    // crval=(45,-30), crpix=(500,500) — this is the case AUD-CORR-1's original
    // bug would get most visibly wrong (southern-hemisphere target field).
    struct Off { double dx, dy, ra, dec; };
    struct Case { core::WcsProjection proj; const char* name; Off offs[2]; };
    static const Case kCases[] = {
        { core::WcsProjection::TAN, "TAN", {
            {100.0, 0.0, 44.967924988395694, -29.999996112399224},
            {0.0, -100.0, 45.0, -30.02777777560145} } },
        { core::WcsProjection::CAR, "CAR", {
            {100.0, 0.0, 44.96792498588271, -29.999996112398623},
            {0.0, -100.0, 45.0, -30.027777777777764} } },
        { core::WcsProjection::AIT, "AIT", {
            {100.0, 0.0, 44.967924985804174, -29.999996112398605},
            {0.0, -100.0, 45.0, -30.027777778049806} } },
    };
    for (const auto& c : kCases) {
        INFO("projection = " << c.name);
        auto wcs = makeWcs(c.proj, 45.0, -30.0, 500.0, 500.0);
        double ra = 0.0, dec = 0.0;
        wcs.pixToSky(wcs.crpix1, wcs.crpix2, ra, dec);
        CHECK_THAT(ra,  WithinAbs(45.0,  1e-6));
        CHECK_THAT(dec, WithinAbs(-30.0, 1e-6));
        for (const auto& o : c.offs) {
            wcs.pixToSky(wcs.crpix1 + o.dx, wcs.crpix2 + o.dy, ra, dec);
            CHECK_THAT(ra,  WithinAbs(o.ra,  1e-6));
            CHECK_THAT(dec, WithinAbs(o.dec, 1e-6));
        }
    }
}

TEST_CASE("WCS: near-pole field (dec=89.9) matches astropy oracle, all 8 projections", "[astronomy][wcs]")
{
    // crval=(0, 89.9), crpix=(500,500) — near the celestial pole, a classic
    // singularity/branch-cut stress case (φ ill-defined at θ=±90°, and RA
    // sweeps through the full [0,360) range for a small pixel offset).
    struct Off { double dx, dy, ra, dec; };
    struct Case { core::WcsProjection proj; const char* name; Off offs[2]; };
    static const Case kCases[] = {
        { core::WcsProjection::TAN, "TAN", {
            {50.0, 0.0, 352.0928333333852, 89.89904010186643},
            {0.0, -50.0, 8.556967963118149e-16, 89.88611111138316} } },
        { core::WcsProjection::SIN, "SIN", {
            {50.0, 0.0, 352.0928331040057, 89.89904010181031},
            {0.0, -50.0, 8.556968183868118e-16, 89.8861111109751} } },
        { core::WcsProjection::ARC, "ARC", {
            {50.0, 0.0, 352.09283318046795, 89.899040101829},
            {0.0, -50.0, 8.556968110282499e-16, 89.88611111111112} } },
        { core::WcsProjection::STG, "STG", {
            {50.0, 0.0, 352.09283321869907, 89.89904010183838},
            {0.0, -50.0, 8.556968073489695e-16, 89.88611111117913} } },
        { core::WcsProjection::CAR, "CAR", {
            {50.0, 0.0, 352.0928331804665, 89.899040101829},
            {0.0, -50.0, 0.0, 89.88611111111112} } },
        { core::WcsProjection::MER, "MER", {
            {50.0, 0.0, 352.0928331804665, 89.899040101829},
            {0.0, -50.0, 0.0, 89.88611111111112} } },
        { core::WcsProjection::GLS, "GLS", {
            {50.0, 0.0, 352.0928331804665, 89.899040101829},
            {0.0, -50.0, 0.0, 89.88611111111112} } },
        { core::WcsProjection::AIT, "AIT", {
            {50.0, 0.0, 352.09283317568776, 89.89904010182785},
            {0.0, -50.0, 0.0, 89.88611111107711} } },
    };
    for (const auto& c : kCases) {
        INFO("projection = " << c.name);
        auto wcs = makeWcs(c.proj, 0.0, 89.9, 500.0, 500.0);
        double ra = 0.0, dec = 0.0;
        wcs.pixToSky(wcs.crpix1, wcs.crpix2, ra, dec);
        CHECK_THAT(dec, WithinAbs(89.9, 1e-6));
        for (const auto& o : c.offs) {
            wcs.pixToSky(wcs.crpix1 + o.dx, wcs.crpix2 + o.dy, ra, dec);
            CHECK_THAT(ra,  WithinAbs(o.ra,  1e-6));
            CHECK_THAT(dec, WithinAbs(o.dec, 1e-6));
        }
    }
}

TEST_CASE("WCS: RA wrap across 0/360 matches astropy oracle, all 8 projections", "[astronomy][wcs]")
{
    // crval=(0.01,10), crpix=(500,500); pixel +60 in X crosses the RA=0
    // meridian and must normalise to just under 360°, not go negative.
    struct Ref { core::WcsProjection proj; const char* name; double ra, dec; };
    static const Ref kRefs[] = {
        { core::WcsProjection::TAN, "TAN", 359.99307622362744, 9.999999572571369 },
        { core::WcsProjection::SIN, "SIN", 359.99307622291144, 9.999999572571333 },
        { core::WcsProjection::ARC, "ARC", 359.99307622315007, 9.999999572571346 },
        { core::WcsProjection::STG, "STG", 359.99307622326944, 9.999999572571351 },
        { core::WcsProjection::CAR, "CAR", 359.99307622315007, 9.99999957257134  },
        { core::WcsProjection::MER, "MER", 359.99307622315007, 9.99999957257134  },
        { core::WcsProjection::GLS, "GLS", 359.99307622315007, 9.99999957257134  },
        { core::WcsProjection::AIT, "AIT", 359.9930762231352,  9.999999572571339 },
    };
    for (const auto& r : kRefs) {
        INFO("projection = " << r.name);
        auto wcs = makeWcs(r.proj, 0.01, 10.0, 500.0, 500.0);
        double ra = 0.0, dec = 0.0;
        wcs.pixToSky(wcs.crpix1 + 60.0, wcs.crpix2, ra, dec);
        REQUIRE(ra >= 0.0);
        REQUIRE(ra < 360.0);
        CHECK_THAT(ra,  WithinAbs(r.ra,  1e-6));
        CHECK_THAT(dec, WithinAbs(r.dec, 1e-6));
    }
}


// ─── Atmospheric refraction (AUD-CORR-3) ─────────────────────────────────────
//
// applyRefractionCorrection() had ZERO tests before this remediation despite
// being the only frame correction actually applied to the coordinate that
// gets reported to the MPC (see MainWindow_measurement.cpp). A sign flip on
// the Bennett term (`alt_app - R_deg` → `alt_app + R_deg`) compiles cleanly
// and would have passed the whole suite; the direction check below fails
// immediately under that mutation.

namespace {
/// Independent (not-under-test) alt computation — same spherical trig as the
/// SUT, but written separately here purely as a checking oracle, so that a
/// bug in applyRefractionCorrection's OWN alt/az computation cannot silently
/// cancel out against an equally-buggy check.
double altitudeDeg(double ra_deg, double dec_deg, double jd, double lat_deg, double lon_deg)
{
    const double lst_deg = core::localSiderealTime(jd, lon_deg);
    const double ha_rad  = (lst_deg - ra_deg) * M_PI / 180.0;
    const double dec_rad = dec_deg * M_PI / 180.0;
    const double lat_rad = lat_deg * M_PI / 180.0;
    const double sinAlt  = std::sin(lat_rad) * std::sin(dec_rad)
                         + std::cos(lat_rad) * std::cos(dec_rad) * std::cos(ha_rad);
    return std::asin(std::clamp(sinAlt, -1.0, 1.0)) * 180.0 / M_PI;
}

/// Bennett (1982) formula, computed independently of Astronomy.cpp, as the
/// numeric oracle for R at a given TRUE altitude in degrees.
double bennettRArcmin(double trueAltDeg)
{
    const double hArg = trueAltDeg + 10.3 / (trueAltDeg + 5.11);
    return 1.02 / std::tan(hArg * M_PI / 180.0);
}
} // namespace

TEST_CASE("Refraction: R matches Bennett formula and pushes the true position toward the horizon",
          "[astronomy][refraction]")
{
    // Geometry: lat=0, lon=0, dec=0 → alt = 90 - |HA|. HA=60° → apparent alt=30°.
    const double lat = 0.0, lon = 0.0, jd = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);
    const double raApp = std::fmod(lst - 60.0 + 360.0, 360.0);
    const double decApp = 0.0;

    double ra = raApp, dec = decApp;
    const double R = core::applyRefractionCorrection(ra, dec, jd, lat, lon);

    // The doc/audit reference value at 30° altitude: R ≈ 1.746' (≈104.76").
    CHECK_THAT(R, WithinAbs(1.745985, 0.001));
    CHECK_THAT(R * 60.0, WithinAbs(104.759, 0.1));   // arcseconds

    // The apparent altitude is exactly the geometry we built (30°, up to the
    // asin/atan2 round-trip precision of the alt/az conversion inside the
    // function) — corroborate independently.
    const double altApp = altitudeDeg(raApp, decApp, jd, lat, lon);
    CHECK_THAT(altApp, WithinAbs(30.0, 0.01));

    // The corrected (true, catalog) position must be LOWER in altitude than
    // the apparent one by exactly R (atmosphere lifts the apparent image up).
    // A sign flip (alt_app + R instead of alt_app - R) would make this fail:
    // it would place the true altitude 2R too high instead of R too low.
    const double altTrue = altitudeDeg(ra, dec, jd, lat, lon);
    CHECK_THAT(altApp - altTrue, WithinAbs(R / 60.0, 0.001));
    CHECK(altTrue < altApp);
}

TEST_CASE("Refraction: zero below 1 degree altitude (formula unstable near horizon)",
          "[astronomy][refraction]")
{
    // lat=0, dec=0, HA=89.5° → alt ≈ 0.5°, well below the 1° cutoff.
    const double lat = 0.0, lon = 0.0, jd = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);
    const double raApp = std::fmod(lst - 89.5 + 360.0, 360.0);
    double ra = raApp, dec = 0.0;

    const double R = core::applyRefractionCorrection(ra, dec, jd, lat, lon);
    CHECK(R == 0.0);
    CHECK_THAT(ra,  WithinAbs(raApp, 1e-12));   // no-op: unchanged in place
    CHECK_THAT(dec, WithinAbs(0.0,   1e-12));
}

TEST_CASE("Refraction: zero and no-op with non-finite site coordinates", "[astronomy][refraction]")
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    double ra = 180.0, dec = 45.0;
    const double R1 = core::applyRefractionCorrection(ra, dec, 2451545.0, nan, 0.0);
    CHECK(R1 == 0.0);
    CHECK_THAT(ra,  WithinAbs(180.0, 1e-12));
    CHECK_THAT(dec, WithinAbs(45.0,  1e-12));

    double ra2 = 90.0, dec2 = -20.0;
    const double R2 = core::applyRefractionCorrection(ra2, dec2, 2451545.0, 0.0, nan);
    CHECK(R2 == 0.0);
    CHECK_THAT(ra2,  WithinAbs(90.0,  1e-12));
    CHECK_THAT(dec2, WithinAbs(-20.0, 1e-12));
}

TEST_CASE("Refraction: R monotonically decreases as altitude increases", "[astronomy][refraction]")
{
    const double lat = 0.0, lon = 0.0, jd = 2451545.0;
    const double lst = core::localSiderealTime(jd, lon);

    double prevR = std::numeric_limits<double>::infinity();
    for (double ha : {85.0, 75.0, 60.0, 30.0, 10.0}) {   // alt = 5,15,30,60,80 deg
        double ra  = std::fmod(lst - ha + 360.0, 360.0);
        double dec = 0.0;
        const double R = core::applyRefractionCorrection(ra, dec, jd, lat, lon);
        INFO("HA=" << ha << "  alt=" << (90.0 - ha) << "  R=" << R);
        CHECK(R < prevR);
        CHECK(R > 0.0);
        // Cross-check each point against the independent Bennett oracle too.
        CHECK_THAT(R, WithinAbs(bennettRArcmin(90.0 - ha), 0.01));
        prevR = R;
    }
}


// ─── Ecliptic / galactic frame conversions (AUD-CORR-4) ──────────────────────
//
// eclipticToEquatorial() and equatorialToGalactic() feed the ecliptic/
// galactic-plane overlay (feature #16) — real callers in FitsImageView.cpp —
// but had zero tests. Oracle values below are well-known astronomical facts
// (vernal equinox, solstice, galactic centre direction), independent of this
// codebase's own formulas.

TEST_CASE("eclipticToEquatorial: vernal equinox (lambda=0) maps to RA=0, Dec=0",
          "[astronomy][ecliptic]")
{
    double ra = 0.0, dec = 0.0;
    core::eclipticToEquatorial(0.0, 0.0, ra, dec);
    CHECK_THAT(ra,  WithinAbs(0.0, 1e-9));
    CHECK_THAT(dec, WithinAbs(0.0, 1e-9));
}

TEST_CASE("eclipticToEquatorial: summer solstice (lambda=90) maps to RA=90, Dec=+obliquity",
          "[astronomy][ecliptic]")
{
    // At the solstice point (beta=0, lambda=90): Dec = obliquity of the
    // ecliptic exactly, RA = 90 exactly — a textbook spherical-trig identity,
    // independent of any AstroFind formula.
    double ra = 0.0, dec = 0.0;
    core::eclipticToEquatorial(90.0, 0.0, ra, dec);
    CHECK_THAT(ra,  WithinAbs(90.0, 1e-9));
    CHECK_THAT(dec, WithinAbs(23.4392911, 1e-6));   // IAU J2000.0 mean obliquity
}

TEST_CASE("eclipticToEquatorial: winter solstice (lambda=270) maps to Dec=-obliquity",
          "[astronomy][ecliptic]")
{
    double ra = 0.0, dec = 0.0;
    core::eclipticToEquatorial(270.0, 0.0, ra, dec);
    CHECK_THAT(ra,  WithinAbs(270.0, 1e-9));
    CHECK_THAT(dec, WithinAbs(-23.4392911, 1e-6));
}

TEST_CASE("equatorialToGalactic: known galactic-centre direction maps close to l=0,b=0",
          "[astronomy][galactic]")
{
    // Widely-cited J2000 equatorial coordinates of the galactic centre
    // (Sgr A*, IAU 1958 system reference direction): RA=266.405, Dec=-28.936.
    // By the definition of the IAU galactic coordinate system this direction
    // is (l,b) ≈ (0,0); residual ~0.001° is the historical rounding in the
    // commonly-quoted RA/Dec figure, not a code error.
    double l = 0.0, b = 0.0;
    core::equatorialToGalactic(266.405, -28.936, l, b);
    const double lWrapped = (l > 180.0) ? l - 360.0 : l;   // compare near 0, not 360
    CHECK_THAT(lWrapped, WithinAbs(0.0, 0.01));
    CHECK_THAT(b,        WithinAbs(0.0, 0.01));
}

TEST_CASE("equatorialToGalactic: north galactic pole maps to b=+90", "[astronomy][galactic]")
{
    // The NGP is at RA=192.859508, Dec=27.128336 by definition — must map to
    // b=+90 exactly (l is undefined/degenerate there, not checked).
    double l = 0.0, b = 0.0;
    core::equatorialToGalactic(192.859508, 27.128336, l, b);
    CHECK_THAT(b, WithinAbs(90.0, 1e-6));
}

// ─── Annual aberration (AUD-CORR-4) ──────────────────────────────────────────
//
// annualAberrationComponents() DOES have a real caller (MainWindow_measurement
// .cpp, for diagnostic logging only — the plate solution already absorbs
// aberration, see Astronomy.h doc comment), so it is kept (not dead code),
// but had zero tests. Property test: total shift is bounded by the IAU
// constant of aberration kappa=20.49552" for any date/position (Meeus 23.3
// is a first-order truncation of an ellipse of semi-major axis kappa).

TEST_CASE("annualAberrationComponents: total magnitude never exceeds kappa=20.4954\"",
          "[astronomy][aberration]")
{
    for (double jd : {2451545.0, 2451545.0 + 91.0, 2451545.0 + 182.0, 2451545.0 + 273.0}) {
        for (auto [ra, dec] : std::initializer_list<std::pair<double,double>>{
                {0.0, 0.0}, {90.0, 45.0}, {180.0, -60.0}, {270.0, 80.0}}) {
            double dRa = 0.0, dDec = 0.0;
            const double mag = core::annualAberrationComponents(ra, dec, jd, dRa, dDec);
            INFO("jd=" << jd << " ra=" << ra << " dec=" << dec << " mag=" << mag);
            CHECK(mag >= 0.0);
            CHECK(mag <= 20.4954 + 1e-4);   // kappa, with small numeric margin
        }
    }
}

TEST_CASE("annualAberrationComponents: at the ecliptic pole the shift is (near-)pure kappa",
          "[astronomy][aberration]")
{
    // At the ecliptic pole (beta=90) the star-Sun angle is always 90°, so the
    // aberration ellipse degenerates to a circle of radius kappa — the
    // classic textbook maximum-shift case.  Approximate the ecliptic pole in
    // equatorial coordinates: RA=270, Dec=90-obliquity=66.5607089.
    double dRa = 0.0, dDec = 0.0;
    const double mag = core::annualAberrationComponents(270.0, 66.5607089, 2451545.0, dRa, dDec);
    CHECK_THAT(mag, WithinAbs(20.49552, 0.01));
}
