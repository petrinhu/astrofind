#include "MpcOrb.h"

#include <QFile>
#include <QTextStream>

namespace core {

// ─── MPC packed epoch → Julian Date ──────────────────────────────────────────
//
// Format: CYYMD
//   C = century letter (I=1800, J=1900, K=2000)
//   YY = two-digit year within century
//   M  = month (1-9 = Jan-Sep, A=Oct, B=Nov, C=Dec)
//   D  = day   (1-9 = 1-9, A=10, B=11 ... Z=35)
//
static double mpcPackedEpochToJD(const QString& packed)
{
    if (packed.length() < 5)
        return 0.0;

    // Century
    int century = 0;
    const QChar c0 = packed[0];
    if      (c0 == QLatin1Char('I')) century = 1800;
    else if (c0 == QLatin1Char('J')) century = 1900;
    else if (c0 == QLatin1Char('K')) century = 2000;
    else return 0.0;

    // Year
    bool ok = false;
    const int yearOffset = packed.mid(1, 2).toInt(&ok);
    if (!ok) return 0.0;
    const int year = century + yearOffset;

    // Month
    const QChar cm = packed[3];
    int month = 0;
    if (cm >= QLatin1Char('1') && cm <= QLatin1Char('9'))
        month = cm.digitValue();
    else if (cm == QLatin1Char('A')) month = 10;
    else if (cm == QLatin1Char('B')) month = 11;
    else if (cm == QLatin1Char('C')) month = 12;
    else return 0.0;

    // Day
    const QChar cd = packed[4];
    int day = 0;
    if (cd >= QLatin1Char('1') && cd <= QLatin1Char('9'))
        day = cd.digitValue();
    else if (cd >= QLatin1Char('A') && cd <= QLatin1Char('Z'))
        day = 10 + (cd.toLatin1() - 'A');
    else if (cd == QLatin1Char('0'))
        day = 0;
    else return 0.0;

    // Convert calendar date to JD using the standard formula
    // (valid for Gregorian calendar dates)
    const int a = (14 - month) / 12;
    const int y = year + 4800 - a;
    const int m = month + 12 * a - 3;
    // Integer divisions intentional: standard Gregorian→JDN algorithm (Meeus).
    // NOLINT(bugprone-integer-division) on each division below.
    const double jdn = day
        + (153 * m + 2) / 5   // NOLINT(bugprone-integer-division)
        + 365 * y
        + y / 4               // NOLINT(bugprone-integer-division)
        - y / 100             // NOLINT(bugprone-integer-division)
        + y / 400             // NOLINT(bugprone-integer-division)
        - 32045;

    // The packed epoch represents noon (0.0 fractional day), i.e. JD = JDN + 0.0
    // Standard JD starts at noon, so JDN is already the Julian Day Number at noon.
    return static_cast<double>(jdn);
}

// ─── parseMpcOrbLine ─────────────────────────────────────────────────────────

std::optional<AsteroidRecord> parseMpcOrbLine(const QString& line)
{
    // Skip short lines, comment lines, and header lines starting with spaces
    if (line.length() < 100)
        return std::nullopt;
    if (line.startsWith(QLatin1Char('#')))
        return std::nullopt;
    if (line.startsWith(QLatin1Char(' ')))
        return std::nullopt;

    AsteroidRecord rec;

    // Cols 0-6: packed number
    {
        bool ok = false;
        const int n = line.mid(0, 7).trimmed().toInt(&ok);
        rec.number = ok ? n : 0;
    }

    // Cols 8-13: H magnitude
    {
        bool ok = false;
        const float h = line.mid(8, 5).trimmed().toFloat(&ok);
        rec.H = ok ? h : 99.0f;
    }

    // Cols 14-19: G slope
    {
        bool ok = false;
        const float g = line.mid(14, 5).trimmed().toFloat(&ok);
        rec.G = ok ? g : 0.15f;
    }

    // Cols 20-25: Epoch (packed MPC date)
    {
        const QString epochStr = line.mid(20, 5).trimmed();
        rec.epoch = mpcPackedEpochToJD(epochStr);
    }

    // Cols 26-35: M (mean anomaly)
    {
        bool ok = false;
        rec.M = line.mid(26, 9).trimmed().toDouble(&ok);
        if (!ok) rec.M = 0.0;
    }

    // Cols 37-46: omega (argument of perihelion)
    {
        bool ok = false;
        rec.omega = line.mid(37, 9).trimmed().toDouble(&ok);
        if (!ok) rec.omega = 0.0;
    }

    // Cols 48-57: Omega (longitude of ascending node)
    {
        bool ok = false;
        rec.Omega = line.mid(48, 9).trimmed().toDouble(&ok);
        if (!ok) rec.Omega = 0.0;
    }

    // Cols 59-68: inclination
    {
        bool ok = false;
        rec.incl = line.mid(59, 9).trimmed().toDouble(&ok);
        if (!ok) rec.incl = 0.0;
    }

    // Cols 70-79: eccentricity
    {
        bool ok = false;
        rec.e = line.mid(70, 9).trimmed().toDouble(&ok);
        if (!ok) rec.e = 0.0;
    }

    // Cols 80-91: n (mean daily motion)
    {
        bool ok = false;
        rec.n = line.mid(80, 11).trimmed().toDouble(&ok);
        if (!ok) rec.n = 0.0;
    }

    // Cols 92-103: a (semi-major axis)
    {
        bool ok = false;
        rec.a = line.mid(92, 11).trimmed().toDouble(&ok);
        if (!ok) rec.a = 0.0;
    }

    // Cols 166+: name/designation
    if (line.length() > 166)
        rec.name = line.mid(166).trimmed();

    return rec;
}

// ─── countMpcOrbRecords ──────────────────────────────────────────────────────

int countMpcOrbRecords(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return 0;

    QTextStream in(&file);
    int count = 0;
    while (!in.atEnd()) {
        const QString line = in.readLine();
        if (line.length() > 100 && !line.startsWith(QLatin1Char('#')))
            ++count;
    }
    return count;
}

} // namespace core
