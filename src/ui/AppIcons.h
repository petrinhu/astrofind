#pragma once

#include <QIcon>

/// Factory for all application toolbar icons drawn via QPainter.
/// Icons match the original Astrometrica Windows toolbar exactly.
namespace AppIcons {

// ── Standard toolbar ──────────────────────────────────────────────────────────
QIcon settings(int sz = 22);       ///< Blue/cyan wrench-arrow (Settings)
QIcon loadImages(int sz = 22);     ///< Folder + checkerboard FITS images
QIcon loadDark(int sz = 22);       ///< Dark folder (dark frame calibration)
QIcon loadFlat(int sz = 22);       ///< Folder + white flat panel
QIcon dataReduction(int sz = 22);  ///< Gray bg + green precision crosshair
QIcon movingObjectDetect(int sz = 22); ///< Gray bg + red precision crosshair
QIcon stackImages(int sz = 22);    ///< Checkerboard stacked (Stack Images)
QIcon viewReport(int sz = 22);     ///< Document with lines (View ADES Report)
QIcon closeImages(int sz = 22);    ///< Checkerboard + red X (Close all Images)
QIcon closeAll(int sz = 22);       ///< Checkerboard + large X (Close all Windows)

// ── Display toolbar ───────────────────────────────────────────────────────────
QIcon backgroundRange(int sz = 22); ///< Rainbow ring (Background & Range)
QIcon selectMarkings(int sz = 22);  ///< Rainbow ring variant (Select Markings)
QIcon invertDisplay(int sz = 22);   ///< Diagonal B/W split (Invert Display)
QIcon knownObjectOverlay(int sz = 22); ///< Star field + markers (KOO)
QIcon zoomIn(int sz = 22);          ///< Checkerboard + zoom highlight
QIcon zoomOut(int sz = 22);         ///< Checkerboard zoom out
QIcon zoomFit(int sz = 22);         ///< Checkerboard + fit arrows

// ── Blink toolbar ─────────────────────────────────────────────────────────────
QIcon blinkImages(int sz = 22);    ///< Checkerboard + cross (Blink Images)
QIcon blinkStop(int sz = 22);      ///< Dark red circle (Stop Blink)
QIcon blinkPrev(int sz = 22);      ///< Left triangle ◄ (Step Back)
QIcon blinkStep(int sz = 22);      ///< Right triangle ► (Step Forward)
QIcon blinkPlay(int sz = 22);      ///< Green filled ▶ (Play)

// ── Measure tool ──────────────────────────────────────────────────────────────
QIcon measure(int sz = 22);        ///< Crosshair + ruler tick marks (Measure Object)

// ── Theme toggle ──────────────────────────────────────────────────────────────
QIcon sun(int sz = 22);            ///< Yellow sun — switch to Day mode
QIcon moon(int sz = 22);           ///< Blue crescent — switch to Night mode
QIcon autoTheme(int sz = 22);      ///< Half-sun/half-moon — Auto (follow system) mode

} // namespace AppIcons
