#pragma once
#include <QString>
#include <optional>

namespace core {

struct AsteroidRecord {
    int     number   = 0;
    QString name;
    float   H        = 99.0f;
    float   G        = 0.15f;
    double  epoch    = 0.0;    // Julian Date
    double  M        = 0.0;    // Mean anomaly (degrees)
    double  omega    = 0.0;    // Argument of perihelion (degrees)
    double  Omega    = 0.0;    // Longitude of ascending node (degrees)
    double  incl     = 0.0;    // Inclination (degrees)
    double  e        = 0.0;    // Eccentricity
    double  n        = 0.0;    // Mean daily motion (deg/day)
    double  a        = 0.0;    // Semi-major axis (AU)
};

std::optional<AsteroidRecord> parseMpcOrbLine(const QString& line);
int countMpcOrbRecords(const QString& filePath);

} // namespace core
