// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#include "Spectrum1D.h"
#include <fitsio.h>
#include <spdlog/spdlog.h>
#include <QFile>
#include <QFileInfo>

#include <cstdlib>
#include <exception>

namespace core {

namespace {

// AUD-INPUT-8: a 2880-byte file can declare NAXIS1=1e8 and the loader would
// allocate the raw buffer (plus two QVector<double> of the same length)
// before a single sample is read. Real 1-D spectra — even merged echelle or
// radio spectra — are far below 2^24 channels; the ceiling is generous but
// ~6x below the audit payload, and the file-size cross-check below is what
// actually defeats a lying header of any size.
constexpr long kMaxSpectrumChannels = 16'777'216L; // 2^24

/// True when the on-disk bytes are a plain (uncompressed) FITS stream, i.e.
/// the file size is directly comparable to the declared data size. cfitsio
/// transparently opens gzip/compress/bzip2 files, whose on-disk size says
/// nothing about the decompressed payload.
bool isPlainFitsOnDisk(const QString& filePath)
{
    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly)) return false;
    return f.read(6) == QByteArrayLiteral("SIMPLE");
}

} // namespace

std::expected<Spectrum1D, QString> loadSpectrum1D(const QString& filePath)
{
    fitsfile* fptr = nullptr;
    int status = 0;

    if (fits_open_file(&fptr, filePath.toLocal8Bit().constData(), READONLY, &status)) {
        char errtxt[FLEN_ERRMSG] = {};
        fits_get_errstatus(status, errtxt);
        return std::unexpected(QString::fromLatin1(errtxt));
    }
    struct Guard { fitsfile* f; int& s; ~Guard() { fits_close_file(f, &s); } } guard{fptr, status};

    int naxis = 0;
    fits_get_img_dim(fptr, &naxis, &status);
    if (status || naxis != 1)
        return std::unexpected(QObject::tr("Not a 1-D spectrum (NAXIS=%1)").arg(naxis));

    long naxes[1] = {0};
    fits_get_img_size(fptr, 1, naxes, &status);
    const long N = naxes[0];
    if (N <= 0)
        return std::unexpected(QObject::tr("Empty spectrum (NAXIS1=0)"));
    if (N > kMaxSpectrumChannels)
        return std::unexpected(QObject::tr("Spectrum too large in '%1' (NAXIS1=%2, ceiling %3)")
                                   .arg(filePath).arg(N).arg(kMaxSpectrumChannels));

    // Cross-check the declared data size against the file on disk (same idea
    // as validateImageDims in FitsImage.cpp, but exact: BITPIX gives the
    // sample size and the HDU address gives where the data starts).
    int bitpix = 0;
    LONGLONG headStart = 0, dataStart = 0, dataEnd = 0;
    fits_get_img_type(fptr, &bitpix, &status);
    fits_get_hduaddrll(fptr, &headStart, &dataStart, &dataEnd, &status);
    if (status)
        return std::unexpected(QObject::tr("Cannot read spectrum header in '%1'").arg(filePath));
    if (isPlainFitsOnDisk(filePath)) {
        const long long needed = static_cast<long long>(dataStart)
                               + static_cast<long long>(N) * (std::abs(bitpix) / 8);
        const qint64 fileSize = QFileInfo(filePath).size();
        if (needed > static_cast<long long>(fileSize))
            return std::unexpected(QObject::tr("Declared spectrum size (%1 channels) in '%2' exceeds "
                                               "the %3-byte file on disk (lying/corrupt header)")
                                       .arg(N).arg(filePath).arg(fileSize));
    }

    Spectrum1D spec;
    spec.filePath = filePath;
    spec.fileName = QFileInfo(filePath).fileName();

    auto readDouble = [&](const char* key, double def) -> double {
        int st = 0; double v = def;
        fits_read_key(fptr, TDOUBLE, key, &v, nullptr, &st);
        return v;
    };
    auto readStr = [&](const char* key) -> QString {
        int st = 0;
        char val[FLEN_VALUE] = {};
        fits_read_key(fptr, TSTRING, key, val, nullptr, &st);
        return QString::fromLatin1(val).trimmed().remove('\'').trimmed();
    };

    spec.crval1 = readDouble("CRVAL1", 1.0);
    spec.cdelt1 = readDouble("CDELT1", 1.0);
    spec.crpix1 = readDouble("CRPIX1", 1.0);
    spec.ctype1 = readStr("CTYPE1");
    spec.cunit1 = readStr("CUNIT1");
    spec.bunit  = readStr("BUNIT");

    // No exception may escape into the Qt slot that calls us (AUD-INPUT-2).
    std::vector<float> raw;
    try {
        raw.resize(static_cast<size_t>(N));
        spec.flux.reserve(static_cast<int>(N));
        spec.wavelength.reserve(static_cast<int>(N));
    } catch (const std::exception& e) {
        return std::unexpected(QObject::tr("Out of memory allocating spectrum '%1' (%2 channels): %3")
                                   .arg(filePath).arg(N).arg(QString::fromLocal8Bit(e.what())));
    }
    long fpixel[1] = {1};
    float nullval = 0.0f;
    int anynull = 0;
    fits_read_pix(fptr, TFLOAT, fpixel, N, &nullval, raw.data(), &anynull, &status);
    if (status) {
        char errtxt[FLEN_ERRMSG] = {};
        fits_get_errstatus(status, errtxt);
        return std::unexpected(QString::fromLatin1(errtxt));
    }

    for (long i = 0; i < N; ++i) {
        spec.wavelength.append(spec.crval1 + spec.cdelt1 * (static_cast<double>(i + 1) - spec.crpix1));
        spec.flux.append(static_cast<double>(raw[static_cast<size_t>(i)]));
    }

    spdlog::info("loadSpectrum1D: '{}' — {} channels  ctype1={} cdelt1={}",
                 spec.fileName.toStdString(), N,
                 spec.ctype1.toStdString(), spec.cdelt1);
    return spec;
}

} // namespace core
