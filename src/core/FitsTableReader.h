#pragma once

#include <QString>
#include <QVector>
#include <QVariant>
#include <QMap>
#include <expected>
#include "Overlay.h"

namespace core {

/// One column from a FITS binary table.
struct FitsTableColumn {
    QString           name;    ///< Column name (TTYPEn keyword)
    QString           unit;    ///< Physical unit string (TUNITn), may be empty
    /// One QVariant per row: numeric types are stored as double, text as QString.
    QVector<QVariant> values;
};

/// Result of reading a FITS BINTABLE extension.
struct FitsTableResult {
    QString                  hduName;   ///< EXTNAME of the HDU that was read
    int                      nRows = 0; ///< Number of rows in the table

    QVector<FitsTableColumn> columns;

    /// Return all values for the named column, or an empty vector if not found.
    QVector<QVariant> column(const QString& name) const;

    /// Return row i as a column-name → value map.
    QMap<QString, QVariant> row(int i) const;
};

/// Read a FITS binary table (BINTABLE extension) using the CCfits library.
///
/// @param filePath  Absolute path to the FITS file.
/// @param hduName   EXTNAME of the table HDU to read; empty string = first
///                  BINTABLE found in the file.
/// @returns FitsTableResult on success, or an error message string on failure.
std::expected<FitsTableResult, QString>
readFitsTable(const QString& filePath, const QString& hduName = {});

/// Import a DAOPHOT/SExtractor FITS BINTABLE as DetectedStar entries.
/// Recognises common column conventions: X_IMAGE/XPOS/X_CENTER/X,
/// Y_IMAGE/YPOS/Y_CENTER/Y, FLUX_ISO/FLUX_AUTO/FLUX, MAG/MAG_AUTO/ISOMAG,
/// A_IMAGE/A, B_IMAGE/B, THETA_IMAGE/THETA.
/// @param hduName  EXTNAME of the BINTABLE; empty = first found.
std::expected<QVector<DetectedStar>, QString>
importDaophotTable(const QString& filePath, const QString& hduName = {});

/// Load a local star catalog from a FITS BINTABLE.
/// Recognises: RA/RAJ2000/RA_ICRS, DEC/DEJ2000/DE_ICRS,
/// MAG/Vmag/Gmag/phot_g_mean_mag, pmRA/pmra, pmDE/pmdec, ID/source_id.
/// Filters rows within maxRadiusDeg of (centerRA, centerDec).
std::expected<QVector<CatalogStar>, QString>
readLocalCatalogTable(const QString& filePath,
                      double centerRA, double centerDec, double maxRadiusDeg);

/// Import an IRAF/Astropy reduction FITS BINTABLE as DetectedStar entries.
///
/// Pixel aliases: XCENTER/YCENTER (IRAF apphot), xcentroid/ycentroid (photutils),
///   X_WORLD/Y_WORLD, plus all X_IMAGE/XPOS aliases from importDaophotTable.
/// Flux/mag aliases: FLUX/FLUX_FIT/flux/segment_flux/kron_flux, MAG/MERR/mag/mag_fit.
/// Sky aliases: RA/DEC, RAJ2000/DEJ2000, sky_centroid_ra/sky_centroid_dec,
///   ra_centroid/dec_centroid (photutils), ALPHA_J2000/DELTA_J2000.
///
/// When sky coords are found the returned DetectedStar entries have .ra/.dec
/// filled and .matched = true. When only sky coords exist (no pixel X/Y) the
/// entries are created with x = -1, y = -1 (off-canvas placeholders).
std::expected<QVector<DetectedStar>, QString>
importReductionTable(const QString& filePath, const QString& hduName = {});

} // namespace core
