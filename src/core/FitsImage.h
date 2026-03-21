#pragma once

#include "Overlay.h"

#include <QString>
#include <QDateTime>
#include <QVector>
#include <vector>
#include <optional>
#include <expected>
#include <limits>

namespace core {

/// Minimal WCS plate solution (filled after astrometry)
struct PlateSolution {
    double crval1 = 0.0;   ///< RA at reference pixel (degrees)
    double crval2 = 0.0;   ///< Dec at reference pixel (degrees)
    double crpix1 = 0.0;   ///< Reference pixel X
    double crpix2 = 0.0;   ///< Reference pixel Y
    double cd1_1  = 0.0;   ///< CD matrix element
    double cd1_2  = 0.0;
    double cd2_1  = 0.0;
    double cd2_2  = 0.0;
    double rms    = 0.0;   ///< Fit RMS (arcseconds)
    bool   solved = false;

    /// Convert pixel (x,y) → (ra,dec) in degrees
    void pixToSky(double px, double py, double& ra, double& dec) const noexcept;
    /// Convert (ra,dec) → pixel (x,y)
    void skyToPix(double ra, double dec, double& px, double& py) const noexcept;
};

/// Raw FITS image data loaded from disk
struct FitsImage {
    // File info
    QString  filePath;
    QString  fileName;
    QString  sourceZipPath;  ///< Original ZIP archive path if extracted from one (empty otherwise)

    // Image data (row-major, float, physical values after BZERO/BSCALE)
    int                  width  = 0;
    int                  height = 0;
    std::vector<float>   data;   ///< width * height floats

    // Display stretch (sigma-clipped)
    float    displayMin = 0.0f;
    float    displayMax = 1.0f;

    // FITS header metadata
    QString  objectName;
    double   ra       = 0.0;   ///< RA from OBJCTRA header (degrees)
    double   dec      = 0.0;   ///< Dec from OBJCTDEC header (degrees)
    double   expTime  = 0.0;   ///< Exposure time (seconds)
    QDateTime dateObs;          ///< DATE-OBS (always UTC after load)
    bool dateObsAmbiguous = false; ///< True when no explicit timezone marker and TIMESYS absent
    double   jd       = 0.0;   ///< Julian Date (mid-exposure)
    QString  filter;
    QString  telescope;
    QString  observer;
    double   gain     = 1.0;
    double   pixScaleX  = 0.0;   ///< arcsec/pixel X (from WCS or config)
    double   pixScaleY  = 0.0;   ///< arcsec/pixel Y
    double   saturation = 65535.0;
    int      binningX   = 1;     ///< CCD binning factor X (from XBINNING header)
    int      binningY   = 1;     ///< CCD binning factor Y (from YBINNING header)

    /// Observer site location from FITS headers (NaN = not present in file).
    /// Keys tried: SITELAT/SITELONG, LAT-OBS/LONG-OBS, OBSGEO-B/OBSGEO-L.
    double   siteLat    = std::numeric_limits<double>::quiet_NaN();
    double   siteLon    = std::numeric_limits<double>::quiet_NaN();
    double   siteAlt    = std::numeric_limits<double>::quiet_NaN();

    // Plate solution (filled after data reduction)
    PlateSolution wcs;

    // Overlay data (filled during data reduction / KOO step)
    QVector<DetectedStar> detectedStars;
    QVector<CatalogStar>  catalogStars;
    QVector<KooObject>    kooObjects;

    bool isValid() const noexcept { return !data.empty() && width > 0 && height > 0; }

    float pixelAt(int x, int y) const noexcept {
        if (x < 0 || x >= width || y < 0 || y >= height) return 0.0f;
        return data[static_cast<size_t>(y) * width + x];
    }
};

/// Load a FITS file from disk. Returns FitsImage on success or error string on failure.
std::expected<FitsImage, QString> loadFits(const QString& filePath);

/// Compute display stretch using sigma-clipping. Fills displayMin/displayMax.
void computeAutoStretch(FitsImage& img, float sigmaLow = 2.0f, float sigmaHigh = 6.0f);

/// Write WCS keywords (CRVAL1/2, CRPIX1/2, CD matrix) back to the original FITS file.
/// Returns an empty string on success, or an error description on failure.
QString saveWcsToFits(const QString& filePath, const PlateSolution& wcs);

/// Convert float data pixel to 8-bit display value using current stretch
inline uint8_t stretchPixel(float val, float dmin, float dmax) noexcept {
    if (dmax <= dmin) return 0;
    const float norm = (val - dmin) / (dmax - dmin);
    const int   clamped = static_cast<int>(norm * 255.0f);
    return static_cast<uint8_t>(std::clamp(clamped, 0, 255));
}

} // namespace core
