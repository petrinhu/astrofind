#pragma once

#include "core/FitsImage.h"

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QPointF>
#include <QCursor>

/// A text annotation pinned to an image-coordinate position.
struct ImageAnnotation {
    QPointF pos;    ///< Image pixel coordinate
    QString text;
};

/// Widget that displays a single FITS image with zoom, pan, and invert support.
class FitsImageView : public QWidget {
    Q_OBJECT
public:
    explicit FitsImageView(QWidget* parent = nullptr);

    void setFitsImage(const core::FitsImage& img);
    void clearImage();

    void   setZoom(double z);
    double zoom() const noexcept { return zoom_; }
    void   zoomIn()    { setZoom(zoom_ * 1.25); }
    void   zoomOut()   { setZoom(zoom_ / 1.25); }
    void   fitToWidget();

    void setInvert(bool on);
    bool inverted() const noexcept { return invert_; }
    void toggleInvert() { setInvert(!invert_); }

    void setFlipH(bool on);
    void setFlipV(bool on);
    void toggleFlipH() { setFlipH(!flipH_); }
    void toggleFlipV() { setFlipV(!flipV_); }

    /// Pan so that the given image pixel coordinate is centred in the widget.
    void centerOnPixel(QPointF imagePt);

    /// Update display stretch and redraw (does NOT reload FITS from disk).
    void setStretch(float displayMin, float displayMax);
    void resetStretch();  ///< Recompute sigma-clipped auto-stretch

    /// Set a precomputed display image (for fast blink switching).
    /// Metadata (WCS, filename, etc.) is taken from img; pixel data is NOT copied.
    void setPrecomputedImage(const QImage& displayImg, const core::FitsImage& img);

    // ── Static utilities ──────────────────────────────────────────────────
    /// Convert FitsImage pixel data to a display QImage (stretch + transforms).
    static QImage toDisplayImage(const core::FitsImage& img,
                                  bool invert = false,
                                  bool flipH  = false,
                                  bool flipV  = false);

    /// Generate a square thumbnail of the given size.
    static QImage toThumbnail(const core::FitsImage& img, int size = 96);

    /// Pixel coordinates → sky coordinates (requires valid WCS)
    bool pixToSky(QPointF pix, double& ra, double& dec) const noexcept;

    // ── Overlay control ───────────────────────────────────────────────────
    void setShowOverlay(bool on);
    bool showOverlay() const noexcept { return showOverlay_; }
    void toggleOverlay()              { setShowOverlay(!showOverlay_); }

    void setShowDetectedStars(bool on) { showDetected_ = on; update(); }
    void setShowCatalogStars (bool on) { showCatalog_  = on; update(); }
    void setShowKooObjects   (bool on) { showKoo_      = on; update(); }
    void setShowLabels       (bool on) { showLabels_   = on; update(); }

    // ── Catalog highlight ─────────────────────────────────────────────────
    /// Highlight the overlay marker nearest to imgPx (image pixel coords).
    void setHighlight(QPointF imgPx);
    void clearHighlight();

    // ── Annotations ───────────────────────────────────────────────────────
    void addAnnotation(const QPointF& imgPos, const QString& text);
    void clearAnnotations();
    const QVector<ImageAnnotation>& annotations() const noexcept { return annotations_; }

    /// Handle pan/zoom keys — callable from a parent widget (avoids event-system recursion).
    void processKey(QKeyEvent* e);

    // ── View-state accessors (for blink zoom preservation) ────────────────────
    bool    isUserZoomed() const noexcept { return userZoomed_; }
    QPointF panOffset()    const noexcept { return panOff_; }
    /// Restore a saved zoom+pan without triggering fitToWidget.
    void restoreView(double z, QPointF pan) {
        zoom_ = z; panOff_ = pan; userZoomed_ = true; update();
    }

    // ── Tool cursor ───────────────────────────────────────────────────────
    /// Sets the active tool cursor. CrossCursor adapts color to invert state.
    void setToolCursor(Qt::CursorShape shape);

    // ── Magnifying glass ──────────────────────────────────────────────────
    void setShowMagnifier(bool on) { showMagnifier_ = on; update(); }
    bool showMagnifier() const noexcept { return showMagnifier_; }
    void toggleMagnifier() { setShowMagnifier(!showMagnifier_); }

    // ── Region statistics mode ────────────────────────────────────────────
    void setRegionMode(bool on);
    bool regionMode() const noexcept { return regionMode_; }

signals:
    void cursorMoved(double ra, double dec, float pixelValue);
    void pixelClicked(QPointF imagePixel, double ra, double dec, float pixelValue);
    void escapePressed();       ///< Emitted on Escape key — request to clear selection
    void regionSelected(QRect imageRect); ///< Emitted when user finishes rubber-band selection

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void resizeEvent(QResizeEvent*) override;
    void keyPressEvent(QKeyEvent*) override;

private:
    QImage makeDisplayImage() const;
    QPointF widgetToImage(QPointF w) const;
    QPointF imageToWidget(QPointF img) const;
    void    clampPan();

    core::FitsImage fitsImg_;
    QImage          displayImg_;

    double  zoom_      = 1.0;
    bool    userZoomed_ = false; ///< true after explicit wheel/setZoom — suppresses auto-refit on resize
    QPointF panOff_;        ///< Pan offset in widget pixels
    bool    invert_ = false;
    bool    flipH_  = false;
    bool    flipV_  = false;

    bool    panning_    = false;
    QPoint  panStart_;
    QPointF panOffStart_;

    // Left-button drag-to-pan state
    bool    leftDragPanning_ = false;
    QPoint  leftPressPos_;

    bool    showOverlay_  = true;
    bool    showDetected_ = true;
    bool    showCatalog_  = true;
    bool    showKoo_      = true;
    bool    showLabels_   = true;

    void drawDetectedStars(QPainter& p) const;
    void drawCatalogStars(QPainter& p) const;
    void drawKooObjects(QPainter& p) const;
    void drawAnnotations(QPainter& p) const;
    void drawMagnifier(QPainter& p) const;
    void drawRubberBand(QPainter& p) const;

    QVector<ImageAnnotation> annotations_;
    QPointF                  highlightPx_ = {-1e9, -1e9};
    QCursor                  defaultCursor_;

    bool    showMagnifier_   = false;
    QPointF magnifierCursor_;

    // ── Region selection rubber band ──────────────────────────────────────
    bool    regionMode_     = false;
    bool    drawingRegion_  = false;
    QPointF rubberAnchor_;   ///< Image-coord anchor
    QPointF rubberCurrent_;  ///< Image-coord current end
};
