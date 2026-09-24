// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// Archive extraction for image sets (items 32 / Phase 1 ZIP). Moved out of
// MainWindow_io.cpp (AUD-TEST-6) so it can be exercised by astrofind_tests,
// and therefore by the ASan/UBSan and valgrind runs of the audit workflow,
// without a MainWindow. Behaviour is unchanged; MainWindow keeps the logging.

#include "ArchiveExtractor.h"

#ifdef ASTROFIND_HAS_LIBARCHIVE
#  include <archive.h>
#  include <archive_entry.h>
#endif

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>

#include <sys/stat.h>

namespace core {

bool hasLibArchiveSupport() noexcept
{
#ifdef ASTROFIND_HAS_LIBARCHIVE
    return true;
#else
    return false;
#endif
}

// AUD-INPUT-4: unzip(1) happily recreates a ZIP entry stored as a symlink
// (or, with the Unix extra field, a FIFO/device) under its original name —
// including a name ending in ".fits". QDir::Files/QDirIterator dereferences
// symlinks, so a "*.fits" entry that is actually a symlink to /etc/passwd or
// ~/.ssh/id_rsa would otherwise sail through and be handed to loadImage(),
// which follows the link (arbitrary file read) or hangs (FIFO). lstat (which
// does NOT follow the link) is the only way to see the entry's real type;
// only S_ISREG is accepted.
bool isSafeRegularFile(const QString& path)
{
    struct stat st{};
    if (::lstat(path.toLocal8Bit().constData(), &st) != 0)
        return false;
    return S_ISREG(st.st_mode);
}

// ─── libarchive (TAR.GZ/BZ2/XZ, 7Z, RAR) ─────────────────────────────────────

ArchiveExtraction extractArchiveImages(const QString& archivePath, const QString& destDir)
{
    ArchiveExtraction out;

#ifndef ASTROFIND_HAS_LIBARCHIVE
    (void)archivePath;
    (void)destDir;
    out.status = ArchiveExtraction::Status::NotSupported;
    return out;
#else
    // Image file extensions we want to extract
    static const QStringList kExts = {
        ".fits", ".fit", ".fts", ".ser", ".xisf",
        ".tiff", ".tif", ".png",
        ".img", ".lbl",
        ".cr2", ".cr3", ".nef", ".arw", ".dng", ".raf", ".orf", ".rw2", ".pef"
    };
    auto isImageFile = [&](const char* name) -> bool {
        const QString n = QString::fromUtf8(name).toLower();
        for (const auto& ext : kExts)
            if (n.endsWith(ext)) return true;
        return false;
    };

    struct archive* ar = archive_read_new();
    archive_read_support_filter_all(ar);
    archive_read_support_format_all(ar);

    if (archive_read_open_filename(ar, archivePath.toLocal8Bit().constData(), 65536) != ARCHIVE_OK) {
        out.status = ArchiveExtraction::Status::OpenFailed;
        out.error  = QString::fromLocal8Bit(archive_error_string(ar));
        archive_read_free(ar);
        return out;
    }

    struct archive_entry* entry = nullptr;
    while (archive_read_next_header(ar, &entry) == ARCHIVE_OK) {
        const char* pathname = archive_entry_pathname(entry);
        if (!pathname || !isImageFile(pathname)) {
            archive_read_data_skip(ar);
            continue;
        }

        // AUD-INPUT-4: reject any entry that is not a plain regular file
        // BEFORE recreating it on disk. Without this check, an entry named
        // "*.fits" that is actually a symlink (or FIFO/device) is happily
        // written by archive_write_disk and handed back to loadImage(),
        // which follows the link — arbitrary file read (e.g. /etc/passwd,
        // ~/.ssh/id_rsa) or a hang on a FIFO, all from an untrusted archive.
        // Only AE_IFREG is accepted; symlinks/FIFOs/devices/sockets/dirs
        // are skipped and reported.
        if (archive_entry_filetype(entry) != AE_IFREG) {
            out.skippedNonRegular.append(QString::fromUtf8(pathname));
            archive_read_data_skip(ar);
            continue;
        }

        // AUD-INPUT-4 twin (mutation review, 2026-09-24): a TAR hardlink
        // entry reports filetype AE_IFREG (it looks like a plain file to the
        // check above) but carries a second name via archive_entry_hardlink()
        // that archive_write_disk() resolves with link(2) — the ORIGINAL,
        // unflattened target name, never sanitised the way this entry's own
        // pathname is flattened to destDir a few lines below. A hostile TAR
        // can point that target outside destDir (e.g. "../../etc/passwd" or
        // an absolute path) and get a hard link to it created on disk under
        // an innocuous "*.fits" name — the same arbitrary-file exposure the
        // symlink check above exists to prevent, just one filetype removed.
        // Rejected exactly like symlink/FIFO/device: AE_IFREG alone is not
        // enough, it must ALSO not be a hardlink.
        if (archive_entry_hardlink(entry) != nullptr) {
            out.skippedNonRegular.append(QString::fromUtf8(pathname));
            archive_read_data_skip(ar);
            continue;
        }

        // Flatten to basename — avoids recreating the archive's directory
        // tree, and with it any "../" component of a hostile entry name.
        const QString baseName = QFileInfo(QString::fromUtf8(pathname)).fileName();
        const QString destPath = destDir + QDir::separator() + baseName;
        archive_entry_set_pathname(entry, destPath.toLocal8Bit().constData());

        struct archive* wr = archive_write_disk_new();
        archive_write_disk_set_options(wr,
            ARCHIVE_EXTRACT_TIME | ARCHIVE_EXTRACT_SECURE_NODOTDOT);
        archive_write_disk_set_standard_lookup(wr);

        if (archive_write_header(wr, entry) == ARCHIVE_OK) {
            const void* buf; size_t size; la_int64_t offset;
            while (archive_read_data_block(ar, &buf, &size, &offset) == ARCHIVE_OK)
                archive_write_data_block(wr, buf, size, offset);
            archive_write_finish_entry(wr);
            out.files.append(destPath);
        }
        archive_write_free(wr);
    }
    archive_read_free(ar);

    out.files.sort();
    return out;
#endif
}

// ─── ZIP (system unzip) ──────────────────────────────────────────────────────

ArchiveExtraction extractZipImages(const QString& zipPath, const QString& destDir, int timeoutMs)
{
    ArchiveExtraction out;

    // Extract supported image files from the ZIP using system unzip
    QProcess proc;
    proc.start("unzip", {
        "-o", zipPath,
        "*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT",
        "*.ser", "*.SER",
        "*.xisf", "*.XISF",
        "*.tiff", "*.tif", "*.TIFF", "*.TIF",
        "*.png", "*.PNG",
        "*.img", "*.IMG", "*.lbl", "*.LBL",
        "*.cr2", "*.CR2", "*.cr3", "*.CR3", "*.nef", "*.NEF", "*.arw", "*.ARW",
        "*.dng", "*.DNG", "*.raf", "*.RAF", "*.orf", "*.ORF", "*.rw2", "*.RW2",
        "*.pef", "*.PEF",
        "-d", destDir
    });

    // Also false when unzip is not installed (the process never started).
    if (!proc.waitForFinished(timeoutMs)) {
        out.status = ArchiveExtraction::Status::ToolFailed;
        out.error  = proc.errorString();
        return out;
    }

    // Search only inside this ZIP's own directory
    QDirIterator it(destDir,
                    {"*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT",
                     "*.ser", "*.SER", "*.xisf", "*.XISF",
                     "*.tiff", "*.tif", "*.TIFF", "*.TIF", "*.png", "*.PNG",
                     "*.img", "*.IMG", "*.lbl", "*.LBL",
                     "*.cr2", "*.CR2", "*.cr3", "*.CR3", "*.nef", "*.NEF", "*.arw", "*.ARW",
                     "*.dng", "*.DNG", "*.raf", "*.RAF", "*.orf", "*.ORF", "*.rw2", "*.RW2",
                     "*.pef", "*.PEF"},
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        const QString candidate = it.next();
        // AUD-INPUT-4: reject anything unzip recreated that is not a plain
        // regular file (symlink/FIFO/device) — see isSafeRegularFile above.
        if (!isSafeRegularFile(candidate)) {
            out.skippedNonRegular.append(candidate);
            continue;
        }
        out.files.append(candidate);
    }
    out.files.sort();
    return out;
}

} // namespace core
