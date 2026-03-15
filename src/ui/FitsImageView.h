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

    /// Pixel coordinates → sky coordinates (requires valid WCS)
    bool pixToSky(QPointF pix, double& ra, double& dec) const noexcept;

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
};
