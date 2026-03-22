#pragma once

#include <QPalette>
#include <QString>
#include <QColor>

class QApplication;
class QMdiArea;

namespace Theme {

enum class Mode { Night, Day, Auto };

/// Apply the palette + stylesheet to the entire QApplication.
void apply(QApplication* app, Mode mode);

/// Background color for the MDI workspace.
QColor mdiBackground(Mode mode);

/// Persist / restore helpers.
QString toString(Mode m);
Mode    fromString(const QString& s, Mode fallback = Mode::Night);

/// Scale a pixel value proportionally to the screen's logical DPI.
/// At 96 DPI (standard desktop) returns px unchanged.
/// On higher-DPI or larger-scale screens, scales proportionally.
int dp(int px);

// ── Marking / overlay colors ──────────────────────────────────────────────────
// Single source of truth for colors used in BOTH the image overlay (FitsImageView)
// and the Image Catalog table (ImageCatalogTable).
// "Dark"  variants: optimised for visibility on dark FITS images.
// "Light" variants: adapted for readability on light table backgrounds.

inline QColor markDetectedStarDark()   { return QColor(  0, 210, 255); }  // cyan         — dark bg ≥8:1
inline QColor markDetectedStarLight()  { return QColor(  0, 100, 160); }  // deep blue     — light bg ≥6:1
inline QColor markCatalogStarDark()    { return QColor(255, 210,   0); }  // yellow        — dark bg ≥9:1
inline QColor markCatalogStarLight()   { return QColor(140,  80,   0); }  // deep amber    — light bg ≥6:1
inline QColor markKooAsteroidDark()    { return QColor( 60, 240, 100); }  // green         — dark bg ≥8:1
inline QColor markKooAsteroidLight()   { return QColor(  0, 100,  40); }  // deep green    — light bg ≥7:1
inline QColor markKooPlanet()          { return QColor(120, 180, 255); }  // light blue    — overlay only
inline QColor markKooComet()           { return QColor(255, 160,  80); }  // orange        — overlay only

} // namespace Theme
