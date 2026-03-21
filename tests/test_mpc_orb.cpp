#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/MpcOrb.h"

// Lines built with exact column offsets matching parseMpcOrbLine():
//   num[0-6]  H[8-12]  G[14-18]  epoch[20-24]  M[26-34]  omega[37-45]
//   Omega[48-56]  incl[59-67]  e[70-78]  n[80-90]  a[92-102]  name[166+]
//
// (1) Ceres — epoch K245K = 2024-May-20 → JDN 2460451 (Meeus Gregorian noon)
// Columns verified with Python: num[0-6] H[8-12] G[14-18] epoch[20-24] M[26-34]
// omega[37-45] Omega[48-56] incl[59-67] e[70-78] n[80-90] a[92-102] name[166+]
static const char kCeresLine[] =
    "00001    3.34  0.12 K245K  13.14780   72.52236   80.30553   10.58573"
    "  0.0760091  0.21406063   2.7659858                                  "
    "                                                               (1) Ceres";

// (4) Vesta — same epoch
static const char kVestaLine[] =
    "00004    3.20  0.32 K245K 150.38287  151.20013  103.85358    7.14188"
    "  0.0887873  0.27151922   2.3615176                                  "
    "                                                               (4) Vesta";

TEST_CASE("parseMpcOrbLine - rejects bad lines", "[mpcorb]")
{
    CHECK_FALSE(core::parseMpcOrbLine(QString()).has_value());
    CHECK_FALSE(core::parseMpcOrbLine(QString("# comment line")).has_value());
    CHECK_FALSE(core::parseMpcOrbLine(QString("  header line with spaces")).has_value());
    CHECK_FALSE(core::parseMpcOrbLine(QString(90, 'X')).has_value());  // too short (<100)
}

TEST_CASE("parseMpcOrbLine - Ceres fields", "[mpcorb]")
{
    const auto rec = core::parseMpcOrbLine(QString::fromLatin1(kCeresLine));
    REQUIRE(rec.has_value());

    CHECK(rec->number == 1);
    CHECK(rec->H == Catch::Approx(3.34f).margin(0.01f));
    CHECK(rec->G == Catch::Approx(0.12f).margin(0.01f));

    // K245K = 2024-May-20: Meeus Gregorian→JDN gives 2460451
    // Function returns the JDN (noon-based), not JD midnight (2460450.5)
    CHECK(rec->epoch == Catch::Approx(2460451.0).margin(0.01));

    CHECK(rec->M     == Catch::Approx(13.14780).margin(1e-4));
    CHECK(rec->omega == Catch::Approx(72.52236).margin(1e-4));
    CHECK(rec->Omega == Catch::Approx(80.30553).margin(1e-4));
    CHECK(rec->incl  == Catch::Approx(10.58573).margin(1e-4));
    CHECK(rec->e     == Catch::Approx(0.0760091).margin(1e-6));
    CHECK(rec->n     == Catch::Approx(0.21406063).margin(1e-7));
    CHECK(rec->a     == Catch::Approx(2.7659858).margin(1e-6));

    REQUIRE_FALSE(rec->name.isEmpty());
    CHECK(rec->name.contains("Ceres"));
}

TEST_CASE("parseMpcOrbLine - Vesta fields", "[mpcorb]")
{
    const auto rec = core::parseMpcOrbLine(QString::fromLatin1(kVestaLine));
    REQUIRE(rec.has_value());

    CHECK(rec->number == 4);
    CHECK(rec->H == Catch::Approx(3.20f).margin(0.01f));
    // Same epoch K245K → same JDN
    CHECK(rec->epoch == Catch::Approx(2460451.0).margin(0.01));
    CHECK(rec->a     == Catch::Approx(2.3615176).margin(1e-6));
    CHECK(rec->name.contains("Vesta"));
}

TEST_CASE("parseMpcOrbLine - epoch century letters", "[mpcorb]")
{
    // Helper: build a minimal valid line (len > 100) with a specific 5-char epoch
    auto makeLine = [](const char* epoch5) -> QString {
        // Use a fixed number '1' at position 0 (not '#' or ' ')
        // All numeric fields default to spaces → parse as 0.0 (acceptable here)
        QString line(200, ' ');
        line[0] = '1';
        for (int i = 0; i < 5; ++i)
            line[20 + i] = QLatin1Char(epoch5[i]);
        return line;
    };

    {
        // J century (1900): J991C — J=1900, 99→1999, month='1'=Jan, day='C'=12
        // a=(14-1)/12=1, y=1999+4800-1=6798, m=1+12-3=10
        // jdn = 12 + (153*10+2)/5 + 365*6798 + 6798/4 - 6798/100 + 6798/400 - 32045
        //     = 12 + 306 + 2481270 + 1699 - 67 + 16 - 32045 = 2451191
        const auto rec = core::parseMpcOrbLine(makeLine("J991C"));
        REQUIRE(rec.has_value());
        CHECK(rec->epoch == Catch::Approx(2451191.0).margin(0.01));
    }

    {
        // I century (1800): I991C — same offsets, century=1800 → year=1899
        // y=1899+4800-1=6698
        // jdn = 12 + 306 + 365*6698 + 6698/4 - 6698/100 + 6698/400 - 32045
        //     = 12 + 306 + 2444770 + 1674 - 66 + 16 - 32045 = 2414667
        const auto rec = core::parseMpcOrbLine(makeLine("I991C"));
        REQUIRE(rec.has_value());
        CHECK(rec->epoch == Catch::Approx(2414667.0).margin(0.01));
    }

    {
        // Invalid century letter → epoch returned as 0.0 (line itself is still valid)
        const auto rec = core::parseMpcOrbLine(makeLine("X991C"));
        REQUIRE(rec.has_value());
        CHECK(rec->epoch == 0.0);
    }

    {
        // Invalid month '0' → epoch = 0.0
        const auto rec = core::parseMpcOrbLine(makeLine("K2400"));
        REQUIRE(rec.has_value());
        CHECK(rec->epoch == 0.0);
    }
}
