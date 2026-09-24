// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-CORR-13: julianDateUtc()/midExposureJd() had zero direct tests before
// this remediation. The bug fixed under AUD-CORR-13 was msecsTo() vs secsTo()
// (secsTo truncates sub-second DATE-OBS fractions to zero) and MJD-OBS not
// being preferred/read at all for XISF — both invisible to any test that
// only checks whole-second timestamps.

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "core/FitsImage.h"

#include <QDateTime>
#include <QTimeZone>

#include <limits>

using Catch::Matchers::WithinAbs;

// ─── julianDateUtc ────────────────────────────────────────────────────────────

TEST_CASE("julianDateUtc: sub-second DATE-OBS fraction is preserved (not truncated)",
          "[fits][time]")
{
    // 2024-01-01T00:00:00.5Z. Independently reproduced with astropy 8.0.1
    // on 2026-09-24: Time("2024-01-01T00:00:00.5", format="isot",
    // scale="utc").jd == 2460310.5000057872 (== 2460310.5 + 0.5/86400).
    // secsTo() (the AUD-CORR-13 bug) would drop the ".5" entirely and give
    // exactly 2460310.5 instead — this test fails immediately under that
    // regression, at a tolerance far tighter than half a second.
    const QDateTime dt(QDate(2024, 1, 1), QTime(0, 0, 0, 500), QTimeZone(0));
    REQUIRE(dt.isValid());
    const double jd = core::julianDateUtc(dt);
    CHECK_THAT(jd, WithinAbs(2460310.5000057872, 1e-9));
    CHECK_THAT(jd, WithinAbs(2460310.5 + 0.5 / 86400.0, 1e-12));
}

TEST_CASE("julianDateUtc: whole-second DATE-OBS matches the textbook JD",
          "[fits][time]")
{
    // J2000.0 epoch itself: 2000-01-01T12:00:00Z == JD 2451545.0 exactly.
    const QDateTime dt(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
    CHECK_THAT(core::julianDateUtc(dt), WithinAbs(2451545.0, 1e-9));
}

TEST_CASE("julianDateUtc: invalid QDateTime returns 0.0 (caller's \"absent\" sentinel)",
          "[fits][time]")
{
    CHECK(core::julianDateUtc(QDateTime()) == 0.0);
}

// ─── midExposureJd ────────────────────────────────────────────────────────────

TEST_CASE("midExposureJd: MJD-OBS is preferred over DATE-OBS even when they disagree",
          "[fits][time]")
{
    // MJD-OBS=60310.25 -> JD = 60310.25 + 2400000.5 = 2460310.75 exactly.
    // DATE-OBS is set to a DIFFERENT (conflicting) instant on purpose: the
    // function must still return the MJD-OBS-derived JD (only logging a
    // warning about the mismatch), proving MJD-OBS wins the precedence
    // instead of being silently overridden or averaged.
    const QDateTime conflicting(QDate(2024, 6, 15), QTime(3, 0, 0), QTimeZone(0));
    const double jd = core::midExposureJd(60310.25, conflicting, /*expTimeSec=*/0.0,
                                          QStringLiteral("test.fits"));
    CHECK_THAT(jd, WithinAbs(2460310.75, 1e-9));
}

TEST_CASE("midExposureJd: falls back to DATE-OBS when MJD-OBS is absent",
          "[fits][time]")
{
    const QDateTime dt(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double jd = core::midExposureJd(nan, dt, /*expTimeSec=*/0.0,
                                          QStringLiteral("test.fits"));
    CHECK_THAT(jd, WithinAbs(2451545.0, 1e-9));
}

TEST_CASE("midExposureJd: adds half the exposure time to reach mid-exposure",
          "[fits][time]")
{
    const QDateTime dt(QDate(2000, 1, 1), QTime(12, 0, 0), QTimeZone(0));
    // 200 s exposure -> +100 s = 100/86400 days at mid-exposure.
    const double jd = core::midExposureJd(std::numeric_limits<double>::quiet_NaN(), dt,
                                          /*expTimeSec=*/200.0, QStringLiteral("test.fits"));
    CHECK_THAT(jd, WithinAbs(2451545.0 + 100.0 / 86400.0, 1e-12));
}

TEST_CASE("midExposureJd: returns 0.0 when both MJD-OBS and DATE-OBS are absent",
          "[fits][time]")
{
    const double nan = std::numeric_limits<double>::quiet_NaN();
    CHECK(core::midExposureJd(nan, QDateTime(), 0.0, QStringLiteral("test.fits")) == 0.0);
}
