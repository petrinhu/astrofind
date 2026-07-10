// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#include "FitsTableReader.h"

// CCfits is a C++ wrapper around cfitsio.
// The include dir points to the CCfits-2.7/ source directory, so class headers
// are included directly (without the installed "CCfits/" prefix).
#include "FITS.h"
#include "BinTable.h"
#include "Column.h"
#include "FitsError.h"
#include "ColumnData.h"
#include "ColumnVectorData.h"
#include "ColumnT.h"
#include "FITSUtilT.h"
#include "KeyData.h"

#include <spdlog/spdlog.h>

#include <cmath>

namespace core {

// ── FitsTableResult helpers ───────────────────────────────────────────────────

QVector<QVariant> FitsTableResult::column(const QString& name) const
{
    for (const auto& col : columns)
        if (col.name == name)
            return col.values;
    return {};
}

QMap<QString, QVariant> FitsTableResult::row(int i) const
{
    QMap<QString, QVariant> m;
    for (const auto& col : columns)
        if (i >= 0 && i < col.values.size())
            m[col.name] = col.values[i];
    return m;
}

// ── Internal helpers ──────────────────────────────────────────────────────────

namespace {

/// Read one CCfits column into a FitsTableColumn.
/// Numeric types are read as double; string types as std::string → QString.
///
/// AUD-INPUT-5: previously any CCfits::FitsException thrown by col.read() was
/// swallowed here and replaced with a vector of NaN (numeric) / empty strings
/// (string) sized `nRows` — nRows being NAXIS2 from the BINTABLE header,
/// i.e. attacker-controlled. A header declaring 1000 rows with only 3 real
/// values on disk would silently succeed with 997 fabricated NaN rows and
/// no error signal at all. Propagate the failure to the caller instead.
std::expected<FitsTableColumn, QString> readColumn(CCfits::Column& col, long nRows)
{
    FitsTableColumn out;
    out.name = QString::fromStdString(col.name());
    out.unit = QString::fromStdString(col.unit());
    out.values.reserve(static_cast<int>(nRows));

    const CCfits::ValueType vt = col.type();
    const bool isString = (vt == CCfits::Tstring || vt == CCfits::VTstring);

    try {
        if (isString) {
            std::vector<std::string> vals;
            col.read(vals, 1L, nRows);
            for (const auto& s : vals)
                out.values.append(QString::fromStdString(s));
        } else {
            std::vector<double> vals;
            col.read(vals, 1L, nRows);
            for (double v : vals)
                out.values.append(v);
        }
    } catch (const CCfits::FitsException& ex) {
        return std::unexpected(
            QStringLiteral("column '%1' declares %2 row(s) in the header but its data "
                            "could not be read (%3)")
                .arg(out.name).arg(nRows).arg(QString::fromStdString(ex.message())));
    }

    // Cross-check the declared row count (NAXIS2) against what was actually
    // read back — belt-and-suspenders in case a future CCfits/cfitsio
    // version returns a short read without throwing.
    if (static_cast<long>(out.values.size()) != nRows) {
        return std::unexpected(
            QStringLiteral("column '%1' declares %2 row(s) but only %3 were read back")
                .arg(out.name).arg(nRows).arg(out.values.size()));
    }

    return out;
}

} // anonymous namespace

// ── Public API ────────────────────────────────────────────────────────────────

std::expected<FitsTableResult, QString>
readFitsTable(const QString& filePath, const QString& hduName)
{
    CCfits::FITS::setVerboseMode(false);

    try {
        // Open file; readDataFlag=false to defer reading image/table data.
        CCfits::FITS fits(filePath.toStdString(), CCfits::Read, false);

        // ── Locate the requested BINTABLE HDU ─────────────────────────────────
        CCfits::BinTable* tbl = nullptr;
        const CCfits::ExtMap& extMap = fits.extension();

        if (!hduName.isEmpty()) {
            // Search by EXTNAME
            const std::string key = hduName.toStdString();
            auto range = extMap.equal_range(key);
            for (auto it = range.first; it != range.second; ++it) {
                tbl = dynamic_cast<CCfits::BinTable*>(it->second);
                if (tbl) break;
            }
            if (!tbl)
                return std::unexpected(
                    QStringLiteral("No BINTABLE named '%1' in '%2'")
                        .arg(hduName, filePath));
        } else {
            // First BINTABLE
            for (auto& [name, extPtr] : extMap) {
                tbl = dynamic_cast<CCfits::BinTable*>(extPtr);
                if (tbl) break;
            }
            if (!tbl)
                return std::unexpected(
                    QStringLiteral("No BINTABLE found in '%1'").arg(filePath));
        }

        // ── Read all column data ───────────────────────────────────────────────
        tbl->makeThisCurrent();
        const long nRows = tbl->rows();
        CCfits::ColMap& colMap = tbl->column();

        FitsTableResult result;
        result.hduName = QString::fromStdString(tbl->name());
        result.nRows   = static_cast<int>(nRows);
        result.columns.reserve(static_cast<int>(colMap.size()));

        for (auto& [colName, colPtr] : colMap) {
            auto colResult = readColumn(*colPtr, nRows);
            if (!colResult)
                return std::unexpected(
                    QStringLiteral("BINTABLE '%1' in '%2': %3")
                        .arg(result.hduName, filePath, colResult.error()));
            result.columns.append(std::move(*colResult));
        }

        spdlog::info("FitsTableReader: '{}' — {} rows × {} columns from '{}'",
                     result.hduName.toStdString(),
                     result.nRows,
                     result.columns.size(),
                     filePath.toStdString());
        return result;

    } catch (const CCfits::FITS::CantOpen& ex) {
        return std::unexpected(
            QStringLiteral("Cannot open '%1': %2")
                .arg(filePath, QString::fromStdString(ex.message())));
    } catch (const CCfits::FitsException& ex) {
        return std::unexpected(QString::fromStdString(ex.message()));
    }
}

std::expected<QVector<DetectedStar>, QString>
importDaophotTable(const QString& filePath, const QString& hduName)
{
    auto tblResult = readFitsTable(filePath, hduName);
    if (!tblResult) return std::unexpected(tblResult.error());
    const auto& tbl = *tblResult;

    auto findCol = [&](std::initializer_list<const char*> names) -> QVector<QVariant> {
        for (const char* n : names) {
            const QString qn = QString::fromLatin1(n);
            for (const auto& col : tbl.columns)
                if (col.name.compare(qn, Qt::CaseInsensitive) == 0)
                    return col.values;
        }
        return {};
    };

    const auto xs     = findCol({"X_IMAGE","XPOS","X_CENTER","X"});
    const auto ys     = findCol({"Y_IMAGE","YPOS","Y_CENTER","Y"});
    const auto fluxes = findCol({"FLUX_ISO","FLUX_APER","FLUX_AUTO","FLUX","NET"});
    const auto mags   = findCol({"MAG","MAG_AUTO","ISOMAG","MAG_ISO"});
    const auto as_    = findCol({"A_IMAGE","AWIN_IMAGE","A"});
    const auto bs_    = findCol({"B_IMAGE","BWIN_IMAGE","B"});
    const auto thetas = findCol({"THETA_IMAGE","THETAWIN_IMAGE","THETA"});

    if (xs.isEmpty() || ys.isEmpty())
        return std::unexpected(QObject::tr("No X/Y position columns found in BINTABLE"));

    QVector<DetectedStar> stars;
    stars.reserve(tbl.nRows);
    int skippedNonFinite = 0;
    for (int i = 0; i < tbl.nRows; ++i) {
        const double x = xs[i].toDouble() - 1.0;   // FITS 1-based → 0-based pixels
        const double y = ys[i].toDouble() - 1.0;
        // AUD-INPUT-5 defense-in-depth: readColumn now refuses to fabricate
        // NaN rows on a read failure, but a required position field can
        // still legitimately be non-finite (e.g. TNULLn/blank cell). Skip
        // rather than hand a NaN centroid downstream.
        if (!std::isfinite(x) || !std::isfinite(y)) { ++skippedNonFinite; continue; }
        DetectedStar s;
        s.x    = x;
        s.y    = y;
        s.flux = fluxes.isEmpty() ? 0.0  : fluxes[i].toDouble();
        s.a    = as_.isEmpty()    ? 2.0  : as_[i].toDouble();
        s.b    = bs_.isEmpty()    ? 2.0  : bs_[i].toDouble();
        s.theta = thetas.isEmpty()? 0.0  : thetas[i].toDouble() * M_PI / 180.0;
        if (!mags.isEmpty()) {
            const double mag = mags[i].toDouble();
            if (mag < 90.0) s.snr = std::pow(10.0, (15.0 - mag) / 2.5);
        }
        stars.append(s);
    }
    if (skippedNonFinite > 0)
        spdlog::warn("importDaophotTable: skipped {} row(s) with non-finite X/Y in '{}'",
                     skippedNonFinite, filePath.toStdString());
    spdlog::info("importDaophotTable: {} stars from '{}'", stars.size(), filePath.toStdString());
    return stars;
}

std::expected<QVector<CatalogStar>, QString>
readLocalCatalogTable(const QString& filePath,
                      double centerRA, double centerDec, double maxRadiusDeg)
{
    auto tblResult = readFitsTable(filePath, {});
    if (!tblResult) return std::unexpected(tblResult.error());
    const auto& tbl = *tblResult;

    auto findCol = [&](std::initializer_list<const char*> names) -> QVector<QVariant> {
        for (const char* n : names) {
            const QString qn = QString::fromLatin1(n);
            for (const auto& col : tbl.columns)
                if (col.name.compare(qn, Qt::CaseInsensitive) == 0)
                    return col.values;
        }
        return {};
    };

    const auto ras    = findCol({"RA","RAJ2000","RA_ICRS","ALPHA_J2000","ra"});
    const auto decs   = findCol({"DEC","DEJ2000","DECJ2000","DE_ICRS","DELTA_J2000","dec"});
    const auto mags   = findCol({"MAG","Vmag","Gmag","phot_g_mean_mag","R1mag","RMAG","mag"});
    const auto pmras  = findCol({"pmRA","pmra","PMRA","PM_RA"});
    const auto pmdecs = findCol({"pmDE","pmdec","PMDEC","PM_DEC","pmDEC"});
    const auto ids    = findCol({"ID","source_id","NAME","UCAC4","RECNO"});

    if (ras.isEmpty() || decs.isEmpty())
        return std::unexpected(QObject::tr("No RA/Dec columns found in local catalog"));

    const double maxR2 = maxRadiusDeg * maxRadiusDeg;
    const double cosDec = std::cos(centerDec * M_PI / 180.0);

    QVector<CatalogStar> stars;
    int skippedNonFinite = 0;
    for (int i = 0; i < tbl.nRows; ++i) {
        const double ra  = ras[i].toDouble();
        const double dec = decs[i].toDouble();
        // AUD-INPUT-5 defense-in-depth: skip rows whose required RA/Dec are
        // non-finite rather than letting a NaN cone-search comparison (which
        // is always false) silently drop or corrupt catalog entries.
        if (!std::isfinite(ra) || !std::isfinite(dec)) { ++skippedNonFinite; continue; }
        const double dra  = (ra - centerRA) * cosDec;
        const double ddec = dec - centerDec;
        if (dra*dra + ddec*ddec > maxR2) continue;
        CatalogStar s;
        s.ra    = ra;
        s.dec   = dec;
        s.mag   = mags.isEmpty()   ? 99.0 : mags[i].toDouble();
        s.pmRA  = pmras.isEmpty()  ? 0.0  : pmras[i].toDouble();
        s.pmDec = pmdecs.isEmpty() ? 0.0  : pmdecs[i].toDouble();
        s.id    = ids.isEmpty() ? QStringLiteral("L%1").arg(i) : ids[i].toString();
        stars.append(s);
    }
    if (skippedNonFinite > 0)
        spdlog::warn("readLocalCatalogTable: skipped {} row(s) with non-finite RA/Dec in '{}'",
                     skippedNonFinite, filePath.toStdString());
    spdlog::info("readLocalCatalogTable: {}/{} stars in cone from '{}'",
                 stars.size(), tbl.nRows, filePath.toStdString());
    return stars;
}

std::expected<QVector<DetectedStar>, QString>
importReductionTable(const QString& filePath, const QString& hduName)
{
    auto tblResult = readFitsTable(filePath, hduName);
    if (!tblResult) return std::unexpected(tblResult.error());
    const auto& tbl = *tblResult;

    auto findCol = [&](std::initializer_list<const char*> names) -> QVector<QVariant> {
        for (const char* n : names) {
            const QString qn = QString::fromLatin1(n);
            for (const auto& col : tbl.columns)
                if (col.name.compare(qn, Qt::CaseInsensitive) == 0)
                    return col.values;
        }
        return {};
    };

    // ── Pixel columns — IRAF + Astropy + SExtractor ───────────────────────────
    const auto xs = findCol({"XCENTER","xcentroid","X_WORLD","X_IMAGE","XPOS","X_CENTER","X"});
    const auto ys = findCol({"YCENTER","ycentroid","Y_WORLD","Y_IMAGE","YPOS","Y_CENTER","Y"});

    // ── Sky columns — IRAF + photutils + SExtractor ───────────────────────────
    const auto ras  = findCol({"RA","sky_centroid_ra","ra_centroid","RAJ2000","RA_ICRS",
                                "ALPHA_J2000","ALPHA","ra"});
    const auto decs = findCol({"DEC","sky_centroid_dec","dec_centroid","DEJ2000","DE_ICRS",
                                "DELTA_J2000","DELTA","dec"});

    // ── Flux / magnitude ──────────────────────────────────────────────────────
    const auto fluxes = findCol({"FLUX","FLUX_FIT","flux","segment_flux","kron_flux",
                                  "FLUX_ISO","FLUX_AUTO","NET"});
    const auto mags   = findCol({"MAG","mag","mag_fit","MAG_AUTO","ISOMAG","MAG_ISO"});
    const auto as_    = findCol({"A_IMAGE","AWIN_IMAGE","A","semimajor_sigma"});
    const auto bs_    = findCol({"B_IMAGE","BWIN_IMAGE","B","semiminor_sigma"});
    const auto thetas = findCol({"THETA_IMAGE","THETAWIN_IMAGE","THETA","orientation"});

    const bool hasPixel = !xs.isEmpty() && !ys.isEmpty();
    const bool hasSky   = !ras.isEmpty() && !decs.isEmpty();

    if (!hasPixel && !hasSky)
        return std::unexpected(QObject::tr(
            "No position columns found (tried XCENTER/xcentroid/X_IMAGE for pixels, "
            "RA/sky_centroid_ra for sky)"));

    QVector<DetectedStar> stars;
    stars.reserve(tbl.nRows);
    int skippedNonFinite = 0;
    for (int i = 0; i < tbl.nRows; ++i) {
        double x = -1.0, y = -1.0;
        if (hasPixel) {
            x = xs[i].toDouble() - 1.0;   // FITS 1-based → 0-based
            y = ys[i].toDouble() - 1.0;
        }
        double ra = 0.0, dec = 0.0;
        if (hasSky) {
            ra  = ras[i].toDouble();
            dec = decs[i].toDouble();
        }
        // AUD-INPUT-5 defense-in-depth: skip rows whose required position
        // field(s) — whichever of pixel/sky is actually present — are
        // non-finite, rather than letting a NaN centroid/coordinate through.
        if (hasPixel && (!std::isfinite(x) || !std::isfinite(y))) { ++skippedNonFinite; continue; }
        if (hasSky   && (!std::isfinite(ra) || !std::isfinite(dec))) { ++skippedNonFinite; continue; }

        DetectedStar s;
        s.x = x;
        s.y = y;
        if (hasSky) {
            s.ra      = ra;
            s.dec     = dec;
            s.matched = true;
        }
        s.flux  = fluxes.isEmpty() ? 0.0  : fluxes[i].toDouble();
        s.a     = as_.isEmpty()    ? 2.0  : as_[i].toDouble();
        s.b     = bs_.isEmpty()    ? 2.0  : bs_[i].toDouble();
        s.theta = thetas.isEmpty() ? 0.0  : thetas[i].toDouble() * M_PI / 180.0;
        if (!mags.isEmpty()) {
            const double mag = mags[i].toDouble();
            if (mag < 90.0) s.snr = std::pow(10.0, (15.0 - mag) / 2.5);
        }
        stars.append(s);
    }
    if (skippedNonFinite > 0)
        spdlog::warn("importReductionTable: skipped {} row(s) with non-finite position field(s) in '{}'",
                     skippedNonFinite, filePath.toStdString());
    spdlog::info("importReductionTable: {} entries from '{}' (pixel={} sky={})",
                 stars.size(), filePath.toStdString(), hasPixel, hasSky);
    return stars;
}

} // namespace core
