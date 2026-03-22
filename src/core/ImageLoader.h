#pragma once

#include "FitsImage.h"
#include <expected>
#include <QString>

namespace core {

/// Unified image loader. Dispatches to the appropriate reader by file extension:
///   .fits / .fit / .fts            → loadFits() (CFITSIO)
///   .ser                           → SER binary reader (first frame)
///   .xisf                          → XISF 1.0 reader (PixInsight)
///   .tiff / .tif / .png / .bmp
///   .jpg / .jpeg                   → Qt QImage reader (8 / 16-bit)
std::expected<FitsImage, QString> loadImage(const QString& filePath);

} // namespace core
