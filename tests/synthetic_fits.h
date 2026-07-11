// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa
//
// AUD-TEST-2 / AUD-TEST-3 remediation: header-only helpers that synthesize
// small, VALID FITS fixtures on disk using cfitsio directly, so functional
// tests exercise the real loadFits()/loadFitsHdu()/loadFitsCube()/
// readFitsTable() disk round-trip without depending on external files that
// only exist on the developer's machine (the bug this replaces: 10 test
// cases SKIPping unconditionally outside /tmp/fits_test/, Catch2 counting
// SKIP as pass).
//
// Every writer below returns bool / QString error and asserts nothing itself
// — callers REQUIRE() the result so a fixture-generation failure surfaces as
// a normal test failure, not a silent SKIP.

#pragma once

#include <fitsio.h>

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDir>

#include <cmath>
#include <cstdio>
#include <random>
#include <vector>

namespace testutil {

/// Add deterministic pseudo-random Gaussian noise (fixed seed -> reproducible
/// across runs/CI) to a pixel buffer in place. Real sky background always has
/// read/shot noise; without it computeAutoStretch()'s sigma-clip can collapse
/// to zero (displayMin == displayMax), which is itself unrealistic.
inline void addDeterministicNoise(std::vector<float>& data, float sigma, unsigned seed = 12345u)
{
    std::mt19937 rng(seed);
    std::normal_distribution<float> dist(0.0f, sigma);
    for (auto& v : data) v += dist(rng);
}

/// One Gaussian "star" to stamp onto a synthetic image.
struct StarSpec {
    double x, y;
    float  amplitude;
    float  sigma;
};

/// Render `stars` as Gaussian blobs additive over a flat sky background.
/// Row-major, width*height floats (matches core::FitsImage::data layout).
inline std::vector<float> renderGaussianImage(int w, int h, float sky,
                                               const std::vector<StarSpec>& stars)
{
    std::vector<float> data(static_cast<size_t>(w) * static_cast<size_t>(h), sky);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float v = sky;
            for (const auto& s : stars) {
                const float dx = static_cast<float>(x) - static_cast<float>(s.x);
                const float dy = static_cast<float>(y) - static_cast<float>(s.y);
                v += s.amplitude * std::exp(-(dx * dx + dy * dy) / (2.0f * s.sigma * s.sigma));
            }
            data[static_cast<size_t>(y) * static_cast<size_t>(w) + static_cast<size_t>(x)] = v;
        }
    }
    return data;
}

/// Decimal degrees RA -> "HH MM SS.sss" (OBJCTRA convention parsed by
/// core::parseRaHMS in FitsImage.cpp).
inline QByteArray raToObjctra(double raDeg)
{
    double h = raDeg / 15.0;
    if (h < 0) h += 24.0;
    const int hh = static_cast<int>(h);
    const double mFull = (h - hh) * 60.0;
    const int mm = static_cast<int>(mFull);
    const double ss = (mFull - mm) * 60.0;
    return QStringLiteral("%1 %2 %3")
        .arg(hh, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 0, 'f', 3)
        .toLatin1();
}

/// Decimal degrees Dec -> "+/-DD MM SS.sss" (OBJCTDEC convention parsed by
/// core::parseDecDMS in FitsImage.cpp).
inline QByteArray decToObjctdec(double decDeg)
{
    const bool neg = decDeg < 0.0;
    double d = std::fabs(decDeg);
    const int dd = static_cast<int>(d);
    const double mFull = (d - dd) * 60.0;
    const int mm = static_cast<int>(mFull);
    const double ss = (mFull - mm) * 60.0;
    return QStringLiteral("%1%2 %3 %4")
        .arg(neg ? QStringLiteral("-") : QStringLiteral("+"))
        .arg(dd, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 0, 'f', 3)
        .toLatin1();
}

/// Header metadata for a synthetic 2-D image.
struct SynthHeader {
    QString objectName = QStringLiteral("SYNTH-TEST");
    QString dateObsIso = QStringLiteral("2024-03-15T04:12:00Z"); // 'Z' -> unambiguous UTC
    double  expTimeSec = 30.0;
    double  raDeg      = 180.0;
    double  decDeg     = 20.0;
    double  gain       = 1.0;
    long    binning    = 1;
};

inline QString cfitsioErrorString(int status)
{
    char buf[FLEN_STATUS] = {};
    fits_get_errstatus(status, buf);
    return QString::fromLatin1(buf);
}

/// Write a plain single-HDU 2-D FITS image (BITPIX=-32) with `data`
/// (row-major, width*height) and the header fields in `hdr`.
inline bool writeSynthImage(const QString& path, int w, int h,
                             const std::vector<float>& data,
                             const SynthHeader& hdr,
                             QString* errOut = nullptr)
{
    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;

    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long naxes[2] = { w, h };
    fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status);
    fits_write_img(fptr, TFLOAT, 1, static_cast<LONGLONG>(data.size()),
                   const_cast<float*>(data.data()), &status);

    const QByteArray objName = hdr.objectName.toLatin1();
    fits_update_key(fptr, TSTRING, "OBJECT", const_cast<char*>(objName.constData()),
                     "synthetic test object", &status);
    QByteArray ra  = raToObjctra(hdr.raDeg);
    QByteArray dec = decToObjctdec(hdr.decDeg);
    fits_update_key(fptr, TSTRING, "OBJCTRA",  ra.data(),  "RA  (HH MM SS.sss)",  &status);
    fits_update_key(fptr, TSTRING, "OBJCTDEC", dec.data(), "Dec (+DD MM SS.sss)", &status);
    QByteArray dateObs = hdr.dateObsIso.toLatin1();
    fits_update_key(fptr, TSTRING, "DATE-OBS", dateObs.data(), "start of exposure (UTC)", &status);
    double expTime = hdr.expTimeSec;
    fits_update_key(fptr, TDOUBLE, "EXPTIME", &expTime, "exposure time [s]", &status);
    double gain = hdr.gain;
    fits_update_key(fptr, TDOUBLE, "GAIN", &gain, "detector gain", &status);
    long binning = hdr.binning;
    fits_update_key(fptr, TLONG, "XBINNING", &binning, "CCD binning X", &status);
    fits_update_key(fptr, TLONG, "YBINNING", &binning, "CCD binning Y", &status);

    const int saveStatus = status;
    fits_close_file(fptr, &status);
    if (saveStatus) {
        if (errOut) *errOut = cfitsioErrorString(saveStatus);
        return false;
    }
    return true;
}

/// Write a multi-HDU FITS file: an empty primary HDU followed by `planes.size()`
/// 2-D image extensions (each width x height), mimicking a real multi-extension
/// RGB/mosaic file (as scanImageHdus()/loadFitsHdu() expect).
inline bool writeSynthMultiHdu(const QString& path, int w, int h,
                                const std::vector<std::vector<float>>& planes,
                                QString* errOut = nullptr)
{
    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;

    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long emptyAxes[1] = { 0 };
    fits_create_img(fptr, BYTE_IMG, 0, emptyAxes, &status); // empty primary

    for (size_t i = 0; i < planes.size(); ++i) {
        long naxes[2] = { w, h };
        fits_create_img(fptr, FLOAT_IMG, 2, naxes, &status);
        fits_write_img(fptr, TFLOAT, 1, static_cast<LONGLONG>(planes[i].size()),
                       const_cast<float*>(planes[i].data()), &status);
        const QByteArray extname = QStringLiteral("PLANE%1").arg(i).toLatin1();
        fits_update_key(fptr, TSTRING, "EXTNAME", const_cast<char*>(extname.constData()),
                         "extension name", &status);
    }

    const int saveStatus = status;
    fits_close_file(fptr, &status);
    if (saveStatus) {
        if (errOut) *errOut = cfitsioErrorString(saveStatus);
        return false;
    }
    return true;
}

/// Write a single-HDU FITS cube (NAXIS3 = depth, depth != 3 so it is not
/// mistaken for an RGB image) — the shape loadFitsCube() expects.
/// `planes[k]` is the k-th w*h plane, k = 0..depth-1.
inline bool writeSynthCube(const QString& path, int w, int h,
                            const std::vector<std::vector<float>>& planes,
                            QString* errOut = nullptr)
{
    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;

    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    const long depth = static_cast<long>(planes.size());
    long naxes[3] = { w, h, depth };
    fits_create_img(fptr, FLOAT_IMG, 3, naxes, &status);

    std::vector<float> flat;
    flat.reserve(static_cast<size_t>(w) * static_cast<size_t>(h) * static_cast<size_t>(depth));
    for (const auto& plane : planes)
        flat.insert(flat.end(), plane.begin(), plane.end());

    fits_write_img(fptr, TFLOAT, 1, static_cast<LONGLONG>(flat.size()), flat.data(), &status);

    QByteArray dateObs = QByteArrayLiteral("2024-03-15T04:12:00Z");
    fits_update_key(fptr, TSTRING, "DATE-OBS", dateObs.data(), "start of exposure (UTC)", &status);
    double expTime = 10.0;
    fits_update_key(fptr, TDOUBLE, "EXPTIME", &expTime, "exposure time [s]", &status);

    const int saveStatus = status;
    fits_close_file(fptr, &status);
    if (saveStatus) {
        if (errOut) *errOut = cfitsioErrorString(saveStatus);
        return false;
    }
    return true;
}

/// Write a BINTABLE with 3 float columns (X, Y, MAG). `nRowsDeclared` is the
/// NAXIS2 the header will claim; `nRowsPhysical` (<= nRowsDeclared) is how
/// many rows of real data actually remain on disk after writing — when it is
/// smaller, the file is truncated right after the physical row data so the
/// header "lies" about having more rows than the file actually contains
/// (regression fixture for AUD-INPUT-5 / AUD-TEST-3: readFitsTable() must
/// return an error in this case, never fabricate NaN/zero rows).
inline bool writeSynthBinTable(const QString& path,
                                long nRowsDeclared, long nRowsPhysical,
                                QString* errOut = nullptr)
{
    if (nRowsPhysical > nRowsDeclared) nRowsPhysical = nRowsDeclared;

    QFile::remove(path);
    fitsfile* fptr = nullptr;
    int status = 0;

    fits_create_file(&fptr, path.toLocal8Bit().constData(), &status);
    long emptyAxes[1] = { 0 };
    fits_create_img(fptr, BYTE_IMG, 0, emptyAxes, &status); // empty primary

    char ttypeX[] = "X", ttypeY[] = "Y", ttypeMag[] = "MAG";
    char tform[]  = "1E";
    char tunit[]  = "";
    char* ttype[3] = { ttypeX, ttypeY, ttypeMag };
    char* tforms[3] = { tform, tform, tform };
    char* tunits[3] = { tunit, tunit, tunit };
    fits_create_tbl(fptr, BINARY_TBL, nRowsDeclared, 3, ttype, tforms, tunits,
                     "TESTTBL", &status);

    LONGLONG headStart = 0, dataStart = 0, dataEnd = 0;
    fits_get_hduaddrll(fptr, &headStart, &dataStart, &dataEnd, &status);

    // Write real, finite data for every declared row (rows beyond
    // nRowsPhysical are discarded by the truncation below, so their values
    // don't matter, but writing them keeps fits_write_col happy about
    // matching the declared row count).
    std::vector<float> xs(static_cast<size_t>(nRowsDeclared));
    std::vector<float> ys(static_cast<size_t>(nRowsDeclared));
    std::vector<float> mags(static_cast<size_t>(nRowsDeclared));
    for (long i = 0; i < nRowsDeclared; ++i) {
        xs[static_cast<size_t>(i)]   = 10.0f + static_cast<float>(i);
        ys[static_cast<size_t>(i)]   = 20.0f + static_cast<float>(i);
        mags[static_cast<size_t>(i)] = 15.0f + 0.01f * static_cast<float>(i);
    }
    fits_write_col(fptr, TFLOAT, 1, 1, 1, nRowsDeclared, xs.data(),   &status);
    fits_write_col(fptr, TFLOAT, 2, 1, 1, nRowsDeclared, ys.data(),   &status);
    fits_write_col(fptr, TFLOAT, 3, 1, 1, nRowsDeclared, mags.data(), &status);

    const int saveStatus = status;
    fits_close_file(fptr, &status);
    if (saveStatus) {
        if (errOut) *errOut = cfitsioErrorString(saveStatus);
        return false;
    }

    if (nRowsPhysical < nRowsDeclared) {
        // 3 columns * TFLOAT(4 bytes) = 12 bytes/row (matches TFORM="1E" above).
        constexpr qint64 kRowWidthBytes = 12;
        const qint64 truncateTo = static_cast<qint64>(dataStart)
                                 + static_cast<qint64>(nRowsPhysical) * kRowWidthBytes;
        QFile f(path);
        if (!f.open(QIODevice::ReadWrite) || !f.resize(truncateTo)) {
            if (errOut) *errOut = QStringLiteral("failed to truncate '%1' to %2 bytes")
                                       .arg(path).arg(truncateTo);
            return false;
        }
    }
    return true;
}

} // namespace testutil
