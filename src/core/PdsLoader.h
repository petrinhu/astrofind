// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

#include "FitsImage.h"
#include <expected>
#include <QString>

namespace core {

/// Load a NASA PDS3 image (item 21.2).
///   - attached label: `.img` starting with PDS_VERSION_ID / ODL_VERSION_ID
///   - detached label: `.lbl` (or an `.img` with a sibling `.lbl`)
/// Reads the first band of the IMAGE object: 8/16/32-bit signed/unsigned
/// integers (MSB/LSB) and 32/64-bit IEEE reals (MSB/LSB), applying
/// SCALING_FACTOR/OFFSET and mapping MISSING_CONSTANT to NaN.
/// Metadata: START_TIME/STOP_TIME, EXPOSURE_DURATION, TARGET_NAME,
/// INSTRUMENT_NAME, RIGHT_ASCENSION/DECLINATION (degrees) when present.
std::expected<FitsImage, QString> loadPds3(const QString& filePath);

/// Load a NASA PDS4 image from its XML label (item 21.2): the first
/// Array_2D_Image of the File_Area_Observational ("Last Index Fastest"),
/// any PDS4 numeric data_type up to 64 bits, scaling_factor/value_offset and
/// Special_Constants/missing_constant. A FITS data file is delegated to loadFits.
std::expected<FitsImage, QString> loadPds4(const QString& labelPath);

/// True when `filePath` looks like a PDS4 product label (XML whose root
/// element is Product_Observational). Cheap: reads only the first few KB.
bool isPds4Label(const QString& filePath);

} // namespace core
