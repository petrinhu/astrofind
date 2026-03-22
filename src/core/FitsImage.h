#pragma once

#include "Overlay.h"

#include <QString>
#include <QDateTime>
#include <QVector>
#include <vector>
#include <optional>
#include <expected>
#include <limits>
#include <cmath>

namespace core {

/// Display transfer function applied when rendering pixel data to screen.
enum class StretchMode {
    Linear,  ///< Linear mapping (default)
    Log,     ///< Logarithmic — compresses bright highlights
    Sqrt,    ///< Square-root — softer compression than log
    Asinh,   ///< Asinh — smooth log-like, preserves dim features (SDSS standard)
    HistEq,  ///< Histogram equalization — maximises local contrast
};

/// False-colour look-up table applied after the transfer function.
/// Maps the 8-bit greyscale output to an RGB colour for display.
/// Only applied to greyscale (B&W) images; colour NAXIS3=3 images are unaffected.
enum class ColorLut {
    Grayscale,  ///< Identity — black-to-white (default)
    Hot,        ///< Thermal heat map: black → red → yellow → white
    Cool,       ///< Cold: black → dark blue → cyan → white
    Viridis,    ///< Perceptually uniform: dark purple → teal → yellow (matplotlib)
};

/// FITS WCS projection type (CTYPE1 keyword, fifth–seventh characters).
enum class WcsProjection {
    TAN,  ///< Gnomonic — default for all narrow-field CCD astrometry (astrometry.net)
    SIN,  ///< Orthographic / synthesis (radio interferometry)
    ARC,  ///< Zenithal equidistant
    STG,  ///< Stereographic
    CAR,  ///< Plate carrée — simple cylindrical (all-sky equatorial maps)
    MER,  ///< Mercator — conformal cylindrical
    GLS,  ///< Global sinusoidal / Sanson-Flamsteed (also SFL)
    AIT,  ///< Hammer-Aitoff — equal-area all-sky
};

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
    WcsProjection projection = WcsProjection::TAN;  ///< Projection type (from CTYPE1)

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
    std::vector<float>   data;   ///< width * height floats — luminance (or grayscale for B&W)

    /// Per-channel pixel data for color images (isColor == true).
    /// Populated from FITS NAXIS3=3 planes in R/G/B order.
    /// Empty for grayscale images — use data[] instead.
    std::vector<float>   dataR, dataG, dataB;

    /// Background model estimated by subtractBackground().
    /// Same size as data[]; empty if background subtraction has not been run.
    /// Values are the additive background that was removed from data[].
    std::vector<float>   background;

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
    QString  origin;      ///< ORIGIN header (observatory/instrument origin)
    QString  observer;
    QString  mpcCode;     ///< MPC observatory code: from MPCCODE/OBSCODE header, or derived from TELESCOP/ORIGIN
    double   gain     = 1.0;
    double   pixScaleX  = 0.0;   ///< arcsec/pixel X (from WCS or config)
    double   pixScaleY  = 0.0;   ///< arcsec/pixel Y
    double   saturation = 65535.0;
    int      binningX   = 1;     ///< CCD binning factor X (from XBINNING header)
    int      binningY   = 1;     ///< CCD binning factor Y (from YBINNING header)

    /// True when NAXIS3 == 3 (RGB color image — three planes R/G/B in the 3rd axis).
    bool        isColor      = false;

    /// NAXIS3 value from the FITS header.
    /// 1 = regular 2D image; 3 = RGB color; N>3 = temporal/spectral cube.
    int         cubeDepth    = 1;

    /// Active display transfer function for this image.
    StretchMode stretchMode  = StretchMode::Linear;

    /// False-colour LUT applied after the transfer function.
    /// Ignored for colour (NAXIS3=3) images.
    ColorLut    colorLut     = ColorLut::Grayscale;

    /// True when the telescope is identified as space-based (HST, JWST, etc.).
    /// When true, siteLat/siteLon are irrelevant; use mpcCode for reporting.
    bool     isSpaceTelescope = false;

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

/// Metadata about one image extension in a multi-HDU FITS file.
struct HduInfo {
    int     hduNumber;   ///< 1-based cfitsio HDU number
    int     width;
    int     height;
    int     naxis3;      ///< Third axis size (1 for grayscale, 3 for NAXIS3=3 color)
    QString name;        ///< EXTNAME, or "HDU N" if absent
};

/// Load a FITS file from disk. Returns FitsImage on success or error string on failure.
std::expected<FitsImage, QString> loadFits(const QString& filePath);

/// Load a specific image HDU (1-based hduNumber) from a FITS file.
/// Uses the single-image path; ignores multi-ext RGB heuristics.
std::expected<FitsImage, QString> loadFitsHdu(const QString& filePath, int hduNumber);

/// Scan all IMAGE_HDU entries (NAXIS≥2) in a FITS file without loading pixel data.
/// Returns an empty vector if the file cannot be opened or has only one image HDU.
QVector<HduInfo> scanImageHdus(const QString& filePath);

/// Load all 2-D planes from a FITS data cube (NAXIS3 > 3) as individual FitsImage objects.
/// Each returned FitsImage contains one temporal/spectral plane with shared header metadata.
/// @param hduNumber  1-based cfitsio HDU number (default = primary HDU).
/// Returns error string when the file is not a cube (NAXIS3 ≤ 1 or NAXIS3 == 3).
std::expected<QVector<FitsImage>, QString>
loadFitsCube(const QString& filePath, int hduNumber = 1);

/// Compute display stretch using sigma-clipping. Fills displayMin/displayMax.
void computeAutoStretch(FitsImage& img, float sigmaLow = 2.0f, float sigmaHigh = 6.0f);

/// Write WCS keywords (CRVAL1/2, CRPIX1/2, CD matrix) back to the original FITS file.
/// Returns an empty string on success, or an error description on failure.
QString saveWcsToFits(const QString& filePath, const PlateSolution& wcs);

/// Convert float data pixel to 8-bit display value using the given stretch mode.
/// HistEq cannot be handled per-pixel (needs CDF); callers should pre-build a LUT instead.
inline uint8_t stretchPixel(float val, float dmin, float dmax,
                             StretchMode mode = StretchMode::Linear) noexcept {
    if (dmax <= dmin) return 0;
    float norm = (val - dmin) / (dmax - dmin);
    norm = std::clamp(norm, 0.0f, 1.0f);

    switch (mode) {
        case StretchMode::Log:
            // log1p(999*norm) / log(1000) compresses ~3 decades of dynamic range
            norm = std::log1p(norm * 999.0f) / 6.9077553f;  // 6.9077... = log(1000)
            break;
        case StretchMode::Sqrt:
            norm = std::sqrt(norm);
            break;
        case StretchMode::Asinh: {
            // asinh(norm/β) / asinh(1/β),  β=0.05 → smooth log-like above ~5% intensity
            constexpr float kBeta = 0.05f;
            constexpr float kNorm = 3.68888f;  // asinh(1/0.05) = asinh(20) ≈ 3.6889
            const float x = norm / kBeta;
            norm = std::log(x + std::sqrt(x * x + 1.0f)) / kNorm;
            break;
        }
        case StretchMode::HistEq:  // handled via LUT in toDisplayImage
        case StretchMode::Linear:
        default:
            break;
    }
    return static_cast<uint8_t>(std::clamp(static_cast<int>(norm * 255.0f), 0, 255));
}

} // namespace core
