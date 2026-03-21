#pragma once

#include "FitsImage.h"
#include <QString>

namespace core {

/// Export a FITS image as a raster image (JPEG, PNG, BMP) using the current
/// display stretch. Returns an empty string on success or an error message.
QString exportImage(const FitsImage& img, const QString& filePath);

} // namespace core
