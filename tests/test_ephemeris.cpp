// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-CORR-12: solveKepler()/heliocentricPosition()/computeEphemeris() had
// zero direct tests before this remediation. Mutation review (2026-09-24)
// confirmed the gap was real: removing the Sun-longitude-to-J2000 precession
// term in Ephemeris.cpp's earthHeliocentricEcliptic() — the actual
// AUD-CORR-12 fix — passed the ENTIRE suite, because nothing here called
// computeEphemeris() at all (only its two Earth-independent helpers,
// solveKepler and heliocentricPosition, were covered). The two
// computeEphemeris() tests below close that gap with real orbital elements
// and an independent JPL Horizons oracle (fetched via curl, L-78 — never
// WebFetch for an API call).

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/Astronomy.h"
#include "core/Ephemeris.h"
#include "core/MpcOrb.h"

#include <cmath>
#include <numbers>

using Catch::Matchers::WithinAbs;

namespace {
constexpr double kD2R = std::numbers::pi / 180.0;
constexpr double kR2D = 180.0 / std::numbers::pi;

core::AsteroidRecord makeOrbit(double epoch, double M, double omega, double Omega,
                                double incl, double e, double n, double a)
{
    core::AsteroidRecord orb;
    orb.epoch = epoch;
    orb.M     = M;
    orb.omega = omega;
    orb.Omega = Omega;
    orb.incl  = incl;
    orb.e     = e;
    orb.n     = n;
    orb.a     = a;
    return orb;
}
} // namespace

// ─── solveKepler ──────────────────────────────────────────────────────────────

TEST_CASE("solveKepler matches Meeus example 30.a (e=0.1 M=5deg -> E=5.554589deg)",
          "[ephemeris][kepler]")
{
    // Astronomical Algorithms, 2nd ed., example 30.a: M=5 deg, e=0.1.
    // Meeus quotes E=5.554589°. Independently reproduced with astropy-free
    // Python (plain Newton-Raphson, same recurrence as solveKepler) on
    // 2026-09-24: E=5.554589253872315 deg.
    const double E = core::solveKepler(5.0 * kD2R, 0.1);
    CHECK_THAT(E * kR2D, WithinAbs(5.554589253872315, 1e-8));
}

TEST_CASE("solveKepler satisfies M = E - e*sin(E) to 1e-12 for e between 0 and 0.999",
          "[ephemeris][kepler]")
{
    // Property-based sweep (not a single fixed value): for ANY valid
    // (M, e), the returned E must satisfy Kepler's equation itself. This
    // catches a wrong formula/sign that happens to pass a single fixed-value
    // test but fails the actual equation it claims to solve — e.g. a
    // maxIter that is too low to converge for near-parabolic e.
    for (double e : { 0.0, 0.1, 0.3, 0.5, 0.7, 0.9, 0.99, 0.999 }) {
        for (double Mdeg : { 0.0, 1.0, 5.0, 45.0, 90.0, 135.0, 179.0, 181.0, 270.0, 359.0 }) {
            const double M = Mdeg * kD2R;
            const double E = core::solveKepler(M, e);
            const double residual = std::abs(M - (E - e * std::sin(E)));
            INFO("e=" << e << " M(deg)=" << Mdeg << " E(deg)=" << (E * kR2D)
                       << " residual=" << residual);
            CHECK(residual < 1e-12);
        }
    }
}

// ─── heliocentricPosition ─────────────────────────────────────────────────────

TEST_CASE("heliocentricPosition: circular orbit (e=0 a=1) returns to the same point after one period",
          "[ephemeris][heliocentric]")
{
    // A circular orbit's period in days is exactly 360/n for a mean motion n
    // (deg/day) — regardless of the physical value chosen for n, propagating
    // by dt = 360/n days must land on the SAME mean anomaly (mod 360) and
    // therefore the identical (x,y,z), because r = a(1 - e*cosE) = a is
    // constant for e=0. A regression that propagated M by the wrong sign or
    // scaled dt incorrectly would move the position away from the start.
    const double n = 0.9856076686;              // deg/day (Earth-like, arbitrary)
    const double period = 360.0 / n;             // days for exactly one revolution
    const double epoch = 2451545.0;
    auto orb = makeOrbit(epoch, /*M=*/45.0, /*omega=*/30.0, /*Omega=*/60.0,
                          /*incl=*/10.0, /*e=*/0.0, n, /*a=*/1.0);

    const auto h0 = core::heliocentricPosition(orb, epoch);
    const auto h1 = core::heliocentricPosition(orb, epoch + period);

    CHECK_THAT(h0.r, WithinAbs(1.0, 1e-12));   // circular orbit: r == a always
    CHECK_THAT(h1.r, WithinAbs(1.0, 1e-12));
    CHECK_THAT(h1.x, WithinAbs(h0.x, 1e-9));
    CHECK_THAT(h1.y, WithinAbs(h0.y, 1e-9));
    CHECK_THAT(h1.z, WithinAbs(h0.z, 1e-9));
}

TEST_CASE("heliocentricPosition: eccentric orbit (e=0.5 a=2) gives r_peri=1 and r_apo=3",
          "[ephemeris][heliocentric]")
{
    // Textbook identity, independent of this codebase: r_perihelion = a(1-e),
    // r_aphelion = a(1+e). At M=0 the body is exactly at perihelion (E=0);
    // at M=180 it is exactly at aphelion (E=180deg) for any e.
    auto orb = makeOrbit(/*epoch=*/2451545.0, /*M=*/0.0, 0.0, 0.0, 0.0,
                          /*e=*/0.5, /*n=*/0.5, /*a=*/2.0);

    const auto peri = core::heliocentricPosition(orb, orb.epoch);
    CHECK_THAT(peri.r, WithinAbs(1.0, 1e-9));
    CHECK_THAT(peri.E, WithinAbs(0.0, 1e-9));

    // Advance to M=180 deg: dt such that orb.M + orb.n*dt == 180 (mod 360).
    const double dtToApo = (180.0 - orb.M) / orb.n;
    const auto apo = core::heliocentricPosition(orb, orb.epoch + dtToApo);
    CHECK_THAT(apo.r, WithinAbs(3.0, 1e-9));
    CHECK_THAT(apo.E * kR2D, WithinAbs(180.0, 1e-6));
}

// ─── computeEphemeris vs JPL Horizons (AUD-CORR-12) ──────────────────────────
//
// Real osculating heliocentric elements and the independent astrometric
// (ICRF J2000, geocentric) RA/Dec, both fetched live from the JPL Horizons
// API on 2026-09-24 via curl (L-78 — WebFetch is never used for an
// authenticated/data API call; this endpoint needs no key but the same rule
// of thumb applies to any such lookup: local HTTP request, not a routed
// fetch). jd == orb.epoch exactly in both cases, so dt=0 and solveKepler()/
// the two-body radius-vector math contribute zero propagation error — any
// residual is earthHeliocentricEcliptic() (Earth's position) plus the
// inherent low-precision-Sun-position/no-perturbation approximation this
// module always had. Tolerance is 60″: comfortably above the current
// implementation's residual (~13-49″ measured in the AUD-CORR-12 fix,
// f6b0712) and comfortably below what removing the J2000 precession
// correction produces (hundreds to low thousands of arcsec — see the
// REDCHECK evidence for this file's commit).
//
// Query used for Ceres (curl -G https://ssd.jpl.nasa.gov/api/horizons.api):
//   Elements: COMMAND=1; CENTER=500@10 EPHEM_TYPE=ELEMENTS REF_PLANE=ECLIPTIC
//             REF_SYSTEM=J2000 OUT_UNITS=AU-D TLIST=2460450.5
//   Observer: COMMAND=1; CENTER=500@399 EPHEM_TYPE=OBSERVER QUANTITIES=1
//             (astrometric RA&DEC) ANG_FORMAT=DEG TLIST=2460450.5
// Eros: identical queries with COMMAND=433; TLIST=2455957.5 (2012-Jan-31, near
// its 2012 close approach to Earth, ~0.18 AU).

TEST_CASE("computeEphemeris: Ceres 2024-May-20 astrometric RA/Dec matches JPL Horizons",
          "[ephemeris][horizons]")
{
    // Real (1) Ceres osculating elements at JD 2460450.5 TDB, heliocentric
    // ecliptic J2000 (JPL Horizons, fetched 2026-09-24; EC/OM/W/IN/N/MA/A
    // map directly onto AsteroidRecord's e/Omega/omega/incl/n/M/a).
    auto orb = makeOrbit(/*epoch=*/2460450.5,
                          /*M=*/113.6800190017589,
                          /*omega=*/73.33598956358216,
                          /*Omega=*/80.25365960043708,
                          /*incl=*/10.58773174310077,
                          /*e=*/0.07906268261892506,
                          /*n=*/0.2141410062839562,
                          /*a=*/2.766959632117654);
    orb.number = 1;
    orb.H = 3.34f;
    orb.G = 0.12f;

    // Horizons astrometric (ICRF J2000) geocentric RA/Dec at the same instant.
    const double horizonsRa  = 293.46991;
    const double horizonsDec = -25.39378;

    const core::EphemerisMatch m = core::computeEphemeris(orb, orb.epoch);
    const double sepArcsec = core::angularDistance(m.ra, m.dec, horizonsRa, horizonsDec) * 3600.0;
    INFO("computeEphemeris ra=" << m.ra << " dec=" << m.dec
                                << " Horizons ra=" << horizonsRa << " dec=" << horizonsDec
                                << " separation(arcsec)=" << sepArcsec);
    CHECK(sepArcsec < 60.0);
}

TEST_CASE("computeEphemeris: Eros 2012-Jan-31 astrometric RA/Dec matches JPL Horizons",
          "[ephemeris][horizons]")
{
    // Real (433) Eros osculating elements at JD 2455957.5 TDB, near its 2012
    // close approach to Earth (large angular rate -> any Earth-position
    // error shows up strongly). JPL Horizons, fetched 2026-09-24.
    auto orb = makeOrbit(/*epoch=*/2455957.5,
                          /*M=*/7.461306477359140,
                          /*omega=*/178.7556219376383,
                          /*Omega=*/304.34943864178,
                          /*incl=*/10.82900379833996,
                          /*e=*/0.2225690094319617,
                          /*n=*/0.5598647921259432,
                          /*a=*/1.457965602360455);
    orb.number = 433;
    orb.H = 10.4f;
    orb.G = 0.46f;

    const double horizonsRa  = 158.32917;
    const double horizonsDec = -4.80639;

    const core::EphemerisMatch m = core::computeEphemeris(orb, orb.epoch);
    const double sepArcsec = core::angularDistance(m.ra, m.dec, horizonsRa, horizonsDec) * 3600.0;
    INFO("computeEphemeris ra=" << m.ra << " dec=" << m.dec
                                << " Horizons ra=" << horizonsRa << " dec=" << horizonsDec
                                << " separation(arcsec)=" << sepArcsec);
    CHECK(sepArcsec < 60.0);
}
