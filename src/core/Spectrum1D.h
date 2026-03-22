#pragma once

#include <QString>
#include <QVector>
#include <expected>

namespace core {

/// A 1-D spectrum loaded from a FITS file with NAXIS=1.
struct Spectrum1D {
    QString filePath;
    QString fileName;
    QVector<double> wavelength;  ///< Wavelength/frequency axis (computed from WCS)
    QVector<double> flux;        ///< Flux / intensity values
    QString ctype1;   ///< CTYPE1 keyword (e.g. "WAVE", "VELO", "FREQ")
    QString cunit1;   ///< CUNIT1 keyword (e.g. "Angstrom", "km/s", "Hz")
    QString bunit;    ///< BUNIT keyword (flux units)
    double  crval1 = 0.0; ///< Reference axis value
    double  cdelt1 = 1.0; ///< Axis increment per pixel
    double  crpix1 = 1.0; ///< Reference pixel (1-based)
    bool isValid() const noexcept { return !flux.empty(); }
};

/// Load a 1-D spectrum from a FITS file with NAXIS=1.
/// Returns error string when the file does not contain a 1-D spectrum.
std::expected<Spectrum1D, QString> loadSpectrum1D(const QString& filePath);

} // namespace core
