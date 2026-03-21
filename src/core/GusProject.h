#pragma once

#include "FitsImage.h"
#include "Measurement.h"
#include <QString>
#include <QVector>

namespace core {

/// Serializes/deserializes an AstroFind session to a .gus JSON project file.
/// Pixel data is NOT stored — images are reloaded from their original paths on open.
struct GusProject {
    static constexpr int VERSION = 1;

    QString              sessionId;
    int                  step = 0;           ///< SessionStep cast to int
    QVector<FitsImage>   images;             ///< Metadata only — data vector is empty
    QVector<Observation> observations;

    /// Serialize to JSON. Returns error string (empty = success).
    QString save(const QString& path) const;

    /// Deserialize from JSON. Returns error string (empty = success).
    QString load(const QString& path);
};

} // namespace core
