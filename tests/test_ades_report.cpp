#include <catch2/catch_test_macros.hpp>
#include "core/AdesReport.h"
#include "core/Measurement.h"

static core::AdesContext makeCtx()
{
    core::AdesContext ctx;
    ctx.mpcCode      = "568";
    ctx.observerName = "J. Smith";
    ctx.measurer     = "J. Smith";
    ctx.telescope    = "0.5m f/8 CCD";
    ctx.catalog      = "Gaia3";
    ctx.software     = "AstroFind";
    ctx.timePrecision = 3;
    return ctx;
}

static core::Observation makeCeresObs()
{
    core::Observation obs;
    obs.objectName = "(1) Ceres";
    obs.mpcNumber  = 1;
    obs.ra         = 180.123456789;
    obs.dec        = -10.987654321;
    obs.raErr      = 0.12;
    obs.decErr     = 0.15;
    obs.jd         = 2460450.5;   // 2024-May-20 00:00 UTC
    obs.mag        = 7.50;
    obs.magErr     = 0.03;
    obs.magBand    = "V";
    return obs;
}

// ── jdToIso8601 ──────────────────────────────────────────────────────────────

TEST_CASE("jdToIso8601 - J2000.0 epoch", "[ades]")
{
    // JD 2451545.0 = 2000-01-01T12:00:00Z (J2000.0)
    const QString s = core::jdToIso8601(2451545.0, 3);
    CHECK(s == "2000-01-01T12:00:00.000Z");
}

TEST_CASE("jdToIso8601 - Unix epoch", "[ades]")
{
    // JD 2440587.5 = 1970-01-01T00:00:00Z (Unix epoch)
    const QString s = core::jdToIso8601(2440587.5, 0);
    CHECK(s == "1970-01-01T00:00:00Z");
}

TEST_CASE("jdToIso8601 - decimal places", "[ades]")
{
    const double jd = 2451545.0;  // J2000.0 noon
    CHECK(core::jdToIso8601(jd, 0).endsWith("00Z"));
    CHECK(core::jdToIso8601(jd, 1).contains('.'));
    CHECK(core::jdToIso8601(jd, 2).contains('.'));
    CHECK(core::jdToIso8601(jd, 3).contains(".000Z"));
}

// Qt 6 "z" prints the milliseconds without trailing zeros (1 to 3 digits),
// so "Time Precision: 1" could produce three decimals; and the fraction was
// truncated, not rounded. Unix epoch + 0.456 s and + 59.96 s:
TEST_CASE("jdToIso8601 - exact number of decimals and rounding", "[ades]")
{
    const double t1 = 2440587.5 + 0.456 / 86400.0;
    CHECK(core::jdToIso8601(t1, 0).toStdString() == "1970-01-01T00:00:00Z");
    CHECK(core::jdToIso8601(t1, 1).toStdString() == "1970-01-01T00:00:00.5Z");
    CHECK(core::jdToIso8601(t1, 2).toStdString() == "1970-01-01T00:00:00.46Z");
    CHECK(core::jdToIso8601(t1, 3).toStdString() == "1970-01-01T00:00:00.456Z");

    // Rounding carries into the next minute.
    const double t2 = 2440587.5 + 59.96 / 86400.0;
    CHECK(core::jdToIso8601(t2, 0).toStdString() == "1970-01-01T00:01:00Z");
    CHECK(core::jdToIso8601(t2, 1).toStdString() == "1970-01-01T00:01:00.0Z");
    CHECK(core::jdToIso8601(t2, 2).toStdString() == "1970-01-01T00:00:59.96Z");
}

// ── generateAdesXml ───────────────────────────────────────────────────────────

TEST_CASE("generateAdesXml - well-formed XML structure", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString xml = core::generateAdesXml(obs, makeCtx());

    REQUIRE_FALSE(xml.isEmpty());
    // Well-formedness: must start with XML declaration
    CHECK(xml.startsWith("<?xml"));
    // Root element
    CHECK(xml.contains("<ades"));
    CHECK(xml.contains("version=\"2022\""));
    // Context block
    CHECK(xml.contains("<obsContext>"));
    CHECK(xml.contains("<mpcCode>568</mpcCode>"));
    CHECK(xml.contains("<obsBlock>"));
    CHECK(xml.contains("<obsData>"));
    // Observation data
    CHECK(xml.contains("<optical>"));
    CHECK(xml.contains("<permID>1</permID>"));
    CHECK(xml.contains("<mode>CCD</mode>"));
    CHECK(xml.contains("<stn>568</stn>"));
    CHECK(xml.contains("<astCat>Gaia3</astCat>"));
    CHECK(xml.contains("<band>V</band>"));
    // All elements closed
    CHECK(xml.contains("</ades>"));
    CHECK(xml.contains("</obsData>"));
    CHECK(xml.contains("</obsBlock>"));
}

TEST_CASE("generateAdesXml - obsTime format", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString xml = core::generateAdesXml(obs, makeCtx());

    // obsTime should be ISO 8601 UTC: "2024-05-20T..."
    CHECK(xml.contains("<obsTime>2024-05-20T"));
    CHECK(xml.contains("Z</obsTime>"));
}

TEST_CASE("generateAdesXml - RA/Dec precision", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString xml = core::generateAdesXml(obs, makeCtx());

    // 9 decimal places in RA and Dec
    CHECK(xml.contains("<ra>180.123456789</ra>"));
    CHECK(xml.contains("<dec>-10.987654321</dec>"));
}

TEST_CASE("generateAdesXml - provisional asteroid ID", "[ades]")
{
    core::Observation obs = makeCeresObs();
    obs.objectName = "2023 AB1";
    obs.mpcNumber  = 0;

    const QString xml = core::generateAdesXml({obs}, makeCtx());
    CHECK(xml.contains("<provID>2023 AB1</provID>"));
    CHECK_FALSE(xml.contains("<permID>"));
}

TEST_CASE("generateAdesXml - comet permID", "[ades]")
{
    core::Observation obs = makeCeresObs();
    obs.objectName = "1P/Halley";
    obs.mpcNumber  = 0;

    const QString xml = core::generateAdesXml({obs}, makeCtx());
    CHECK(xml.contains("<permID>1P</permID>"));
}

TEST_CASE("generateAdesXml - no mag when faint", "[ades]")
{
    core::Observation obs = makeCeresObs();
    obs.mag = 99.9;  // sentinel for "no magnitude"

    const QString xml = core::generateAdesXml({obs}, makeCtx());
    CHECK_FALSE(xml.contains("<mag>"));
    CHECK_FALSE(xml.contains("<band>"));
}

// ── generateAdesPsv ───────────────────────────────────────────────────────────

TEST_CASE("generateAdesPsv - header and data row", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString psv = core::generateAdesPsv(obs, makeCtx());

    REQUIRE_FALSE(psv.isEmpty());
    const QStringList lines = psv.split('\n', Qt::SkipEmptyParts);
    REQUIRE(lines.size() >= 2);  // header + at least one data row

    // PSV format: first line is "# version=2022", second is the column header
    REQUIRE(lines.size() >= 3);  // version comment + column header + data
    CHECK(lines[0] == "# version=2022");
    const QString& header = lines[1];
    CHECK(header.startsWith("permID|"));
    CHECK(header.contains("|ra|"));
    CHECK(header.contains("|dec|"));
    CHECK(header.contains("|obsTime|"));
}

// ── AUD-CORR-14: sys=ICRF frame declaration ──────────────────────────────────
//
// ADES 2022 §4.2 requires an explicit `sys` field declaring the reference
// frame of RA/Dec. AstroFind's positions are astrometric ICRF (fitted
// against an ICRS catalog plate solution; aberration/nutation are only
// logged, never applied — see AUD-CORR-4/AUD-CORR-7), so `sys` must always
// read "ICRF", in both the XML and PSV report formats. This had no assertion
// before this remediation: a regression that dropped the field, or wrote
// the wrong frame name (e.g. "J2000" or "apparent"), would have gone
// unnoticed by the existing structural tests above.

TEST_CASE("generateAdesXml - sys is ICRF", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString xml = core::generateAdesXml(obs, makeCtx());
    CHECK(xml.contains("<sys>ICRF</sys>"));
}

TEST_CASE("generateAdesPsv - sys column is ICRF", "[ades]")
{
    const QVector<core::Observation> obs = { makeCeresObs() };
    const QString psv = core::generateAdesPsv(obs, makeCtx());

    const QStringList lines = psv.split('\n', Qt::SkipEmptyParts);
    REQUIRE(lines.size() >= 3);
    const QString& header = lines[1];
    CHECK(header.contains("|sys|"));

    const QStringList headerCols = header.split('|');
    const int sysCol = headerCols.indexOf(QStringLiteral("sys"));
    REQUIRE(sysCol >= 0);

    const QStringList dataCols = lines[2].split('|');
    REQUIRE(dataCols.size() > sysCol);
    CHECK(dataCols[sysCol] == QStringLiteral("ICRF"));
}
