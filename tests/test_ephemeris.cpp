// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-CORR-12: solveKepler()/heliocentricPosition() had zero direct tests
// before this remediation — only the end-to-end computeEphemeris() output was
// exercised indirectly through the field-query tests. The two bugs fixed
// under AUD-CORR-12 (MPCORB epoch off by 0.5 day; Sun longitude computed in
// the equinox of date instead of J2000) both lived inside these two
// functions, and a regression to either would not necessarily move
// computeEphemeris() far enough to fail a coarse end-to-end check.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

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
