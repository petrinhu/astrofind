#pragma once

#include "Measurement.h"

#include <QString>
#include <QVector>

namespace core {

/// Observer/instrument context — filled from QSettings (observer/* keys).
struct AdesContext {
    QString mpcCode;        ///< MPC observatory code (3 chars, e.g. "568")
    QString observerName;   ///< Observer name(s)
    QString measurer;       ///< Measurer name (defaults to observerName)
    QString telescope;      ///< Telescope description (aperture, focal ratio, CCD)
    QString catalog;        ///< Astrometric catalog used (default "UCAC4")
    QString software;       ///< Software name (default "AstroFind")
    QString contact1;       ///< Contact name/organisation (included in XML if includeContact)
    QString contact2;       ///< Contact address line 2 (optional)
    QString email;          ///< Contact e-mail address
    bool    includeContact  = false; ///< Include contact block in ADES XML submission
    int     timePrecision   = 1;     ///< Decimal places in ISO 8601 time (0=seconds, 1=tenths, 2=hundredths, 3=milliseconds)
};


/// Generate an ADES 2022 XML report string from a list of observations.
/// Groups observations by object into separate <obsBlock> elements.
QString generateAdesXml(const QVector<Observation>& obs,
                        const AdesContext& ctx);

/// Generate an ADES 2022 PSV (pipe-separated values) report string.
/// One header line + one data line per observation.
QString generateAdesPsv(const QVector<Observation>& obs,
                        const AdesContext& ctx);

/// Convert a Julian Date to an ISO 8601 UTC string.
/// decimalPlaces: 0=seconds, 1=tenths, 2=hundredths, 3=milliseconds
/// e.g. 2451545.0 → "2000-01-01T12:00:00.000Z"
QString jdToIso8601(double jd, int decimalPlaces = 3);

} // namespace core
