#pragma once

#include "core/FitsImage.h"

#include <QWidget>
#include <QImage>
#include <QPoint>
#include <QPointF>

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

signals:
    void cursorMoved(double ra, double dec, float pixelValue);
    void pixelClicked(QPointF imagePixel, double ra, double dec, float pixelValue);

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void resizeEvent(QResizeEvent*) override;

private:
    QImage makeDisplayImage() const;
    QPointF widgetToImage(QPointF w) const;
    QPointF imageToWidget(QPointF img) const;
    void    clampPan();

    core::FitsImage fitsImg_;
    QImage          displayImg_;

    double  zoom_   = 1.0;
    QPointF panOff_;        ///< Pan offset in widget pixels
    bool    invert_ = false;
    bool    flipH_  = false;
    bool    flipV_  = false;

    bool    panning_    = false;
    QPoint  panStart_;
    QPointF panOffStart_;

    bool    showOverlay_ = true;

    void drawDetectedStars(QPainter& p) const;
    void drawCatalogStars(QPainter& p) const;
    void drawKooObjects(QPainter& p) const;
};
