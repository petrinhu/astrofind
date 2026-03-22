#include "Spectrum1D.h"
#include <fitsio.h>
#include <spdlog/spdlog.h>
#include <QFileInfo>

namespace core {

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

    std::vector<float> raw(static_cast<size_t>(N));
    long fpixel[1] = {1};
    float nullval = 0.0f;
    int anynull = 0;
    fits_read_pix(fptr, TFLOAT, fpixel, N, &nullval, raw.data(), &anynull, &status);
    if (status) {
        char errtxt[FLEN_ERRMSG] = {};
        fits_get_errstatus(status, errtxt);
        return std::unexpected(QString::fromLatin1(errtxt));
    }

    spec.flux.reserve(static_cast<int>(N));
    spec.wavelength.reserve(static_cast<int>(N));
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
