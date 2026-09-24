// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

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
///   .img / .lbl                    → NASA PDS3 (attached or detached label)
///   .xml (PDS4 product label)      → NASA PDS4
///   .cr2 .cr3 .nef .arw .dng ...   → DSLR RAW via LibRaw (when built with it)
std::expected<FitsImage, QString> loadImage(const QString& filePath);

/// Load a DSLR RAW file (item 21.1) with LibRaw: linear CFA data, 2x2
/// superpixel luminance in data[], simple demosaic in dataR/G/B, EXIF exposure
/// and timestamp (camera clock: dateObsAmbiguous is set). Returns an error
/// explaining the missing dependency when AstroFind was built without LibRaw.
std::expected<FitsImage, QString> loadDslrRaw(const QString& filePath);

/// True for camera RAW extensions routed to loadDslrRaw (case-insensitive).
bool isDslrRawExtension(const QString& ext);

/// True when this build links LibRaw.
bool hasDslrRawSupport() noexcept;

} // namespace core
