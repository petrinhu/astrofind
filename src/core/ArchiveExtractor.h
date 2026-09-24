// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

#include <QString>
#include <QStringList>

namespace core {

/// Outcome of extracting the image files of one archive into a directory.
/// Pure data: the caller (MainWindow) owns the temp directory and turns this
/// into log messages, so the extraction itself is unit-testable (AUD-TEST-6).
struct ArchiveExtraction {
    enum class Status {
        Ok,            ///< Archive read; `files` may still be empty (no images inside)
        NotSupported,  ///< Built without libarchive (expandArchive only)
        OpenFailed,    ///< Archive could not be opened (libarchive), see `error`
        ToolFailed,    ///< unzip(1) did not start or did not finish in time (ZIP only)
    };

    Status      status = Status::Ok;
    QStringList files;              ///< Extracted image files (regular files only), sorted
    QStringList skippedNonRegular;  ///< Entries refused as symlink/FIFO/device (AUD-INPUT-4)
    QString     error;              ///< Library/tool error text when status != Ok
};

/// True when this build links libarchive (TAR.GZ/BZ2/XZ, 7Z, RAR).
bool hasLibArchiveSupport() noexcept;

/// True only for a plain regular file. Uses lstat(), which does NOT follow a
/// symlink, so a "*.fits" entry that is really a symlink/FIFO/device is seen
/// for what it is (AUD-INPUT-4).
bool isSafeRegularFile(const QString& path);

/// Extract the image entries of a TAR.GZ/BZ2/XZ, 7Z or RAR archive (libarchive,
/// format detected from the content) into `destDir`, flattened to their base
/// names. Only regular-file entries are written; symlinks, FIFOs, devices and
/// directories are skipped and listed in `skippedNonRegular`. `destDir` must
/// exist. Returns Status::NotSupported when built without libarchive.
ArchiveExtraction extractArchiveImages(const QString& archivePath, const QString& destDir);

/// Extract the image entries of a ZIP file into `destDir` with the system
/// unzip(1), then keep only regular files (unzip recreates symlink entries as
/// symlinks, which are refused and listed in `skippedNonRegular`). `destDir`
/// must exist.
ArchiveExtraction extractZipImages(const QString& zipPath, const QString& destDir,
                                   int timeoutMs = 30000);

} // namespace core
