#include "FitsImageView.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QCursor>

FitsImageView::FitsImageView(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(200, 200);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

void FitsImageView::setFitsImage(const core::FitsImage& img)
{
    fitsImg_    = img;
    displayImg_ = makeDisplayImage();
    fitToWidget();
    update();
}

void FitsImageView::clearImage()
{
    fitsImg_ = {};
    displayImg_ = {};
    update();
}

QImage FitsImageView::makeDisplayImage() const
{
    return toDisplayImage(fitsImg_, invert_, flipH_, flipV_);
}

void FitsImageView::fitToWidget()
{
    if (!fitsImg_.isValid() || width() <= 0 || height() <= 0) { zoom_ = 1.0; return; }
    const double zx = static_cast<double>(width())  / fitsImg_.width;
    const double zy = static_cast<double>(height()) / fitsImg_.height;
    zoom_   = std::min(zx, zy);
    panOff_ = QPointF(
        (width()  - fitsImg_.width  * zoom_) / 2.0,
        (height() - fitsImg_.height * zoom_) / 2.0
    );
}

void FitsImageView::setZoom(double z)
{
    if (!fitsImg_.isValid()) return;
    z = std::clamp(z, 0.05, 32.0);
    // Zoom around widget center
    const QPointF center(width() / 2.0, height() / 2.0);
    const QPointF imgPt = widgetToImage(center);
    zoom_   = z;
    panOff_ = center - QPointF(imgPt.x() * zoom_, imgPt.y() * zoom_);
    clampPan();
    update();
}

void FitsImageView::setInvert(bool on)
{
    if (invert_ == on) return;
    invert_     = on;
    displayImg_ = makeDisplayImage();
    update();
}

void FitsImageView::setFlipH(bool on)
{
    if (flipH_ == on) return;
    flipH_      = on;
    displayImg_ = makeDisplayImage();
    update();
}

void FitsImageView::setFlipV(bool on)
{
    if (flipV_ == on) return;
    flipV_      = on;
    displayImg_ = makeDisplayImage();
    update();
}

bool FitsImageView::pixToSky(QPointF pix, double& ra, double& dec) const noexcept
{
    if (!fitsImg_.wcs.solved) return false;
    fitsImg_.wcs.pixToSky(pix.x(), pix.y(), ra, dec);
    return true;
}

// ─── Events ───────────────────────────────────────────────────────────────────

void FitsImageView::paintEvent(QPaintEvent* /*e*/)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(0x10, 0x10, 0x18));

    if (displayImg_.isNull()) {
        p.setPen(QColor(0x60, 0x60, 0x80));
        p.drawText(rect(), Qt::AlignCenter,
            tr("No image loaded\nUse File → Load Images…"));
        return;
    }

    const int dw = static_cast<int>(fitsImg_.width  * zoom_);
    const int dh = static_cast<int>(fitsImg_.height * zoom_);
    const QRect dst(static_cast<int>(panOff_.x()),
                    static_cast<int>(panOff_.y()),
                    dw, dh);

    p.setRenderHint(zoom_ >= 2.0 ? QPainter::Antialiasing : QPainter::SmoothPixmapTransform, false);
    p.drawImage(dst, displayImg_);
}

void FitsImageView::mouseMoveEvent(QMouseEvent* e)
{
    const QPointF imgPos = widgetToImage(e->position());

    if (panning_ && (e->buttons() & Qt::MiddleButton || e->buttons() & Qt::RightButton)) {
        panOff_ = panOffStart_ + QPointF(e->pos() - panStart_);
        clampPan();
        update();
    }

    if (fitsImg_.isValid()) {
        const int ix = static_cast<int>(imgPos.x());
        const int iy = static_cast<int>(imgPos.y());
        const float val = fitsImg_.pixelAt(ix, iy);
        double ra = 0.0, dec = 0.0;
        pixToSky(imgPos, ra, dec);
        emit cursorMoved(ra, dec, val);
    }
}

void FitsImageView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::MiddleButton || e->button() == Qt::RightButton) {
        panning_     = true;
        panStart_    = e->pos();
        panOffStart_ = panOff_;
        setCursor(Qt::ClosedHandCursor);
    } else if (e->button() == Qt::LeftButton && fitsImg_.isValid()) {
        const QPointF imgPos = widgetToImage(e->position());
        const int ix = static_cast<int>(imgPos.x());
        const int iy = static_cast<int>(imgPos.y());
        const float val = fitsImg_.pixelAt(ix, iy);
        double ra = 0.0, dec = 0.0;
        pixToSky(imgPos, ra, dec);
        emit pixelClicked(imgPos, ra, dec, val);
    }
}

void FitsImageView::mouseReleaseEvent(QMouseEvent* /*e*/)
{
    panning_ = false;
    setCursor(Qt::CrossCursor);
}

void FitsImageView::wheelEvent(QWheelEvent* e)
{
    const double factor = e->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    // Zoom around cursor position
    const QPointF wp  = e->position();
    const QPointF ip  = widgetToImage(wp);
    zoom_ = std::clamp(zoom_ * factor, 0.05, 32.0);
    panOff_ = wp - QPointF(ip.x() * zoom_, ip.y() * zoom_);
    clampPan();
    update();
    e->accept();
}

void FitsImageView::resizeEvent(QResizeEvent* /*e*/)
{
    if (!fitsImg_.isValid()) return;
    // On first resize after loading, fit to window
    if (zoom_ == 1.0) fitToWidget();
}

// ─── Internal helpers ─────────────────────────────────────────────────────────

QPointF FitsImageView::widgetToImage(QPointF w) const
{
    return QPointF((w.x() - panOff_.x()) / zoom_,
                   (w.y() - panOff_.y()) / zoom_);
}

QPointF FitsImageView::imageToWidget(QPointF img) const
{
    return QPointF(img.x() * zoom_ + panOff_.x(),
                   img.y() * zoom_ + panOff_.y());
}

void FitsImageView::clampPan()
{
    if (!fitsImg_.isValid()) return;
    const double iw = fitsImg_.width  * zoom_;
    const double ih = fitsImg_.height * zoom_;
    // Allow panning so at least 20% of image stays visible
    panOff_.setX(std::clamp(panOff_.x(), -iw * 0.8, width()  - iw * 0.2));
    panOff_.setY(std::clamp(panOff_.y(), -ih * 0.8, height() - ih * 0.2));
}

// ─── New public stretch / precomputed-image methods ───────────────────────────

void FitsImageView::setStretch(float displayMin, float displayMax)
{
    fitsImg_.displayMin = displayMin;
    fitsImg_.displayMax = displayMax;
    displayImg_ = makeDisplayImage();
    update();
}

void FitsImageView::resetStretch()
{
    core::computeAutoStretch(fitsImg_);
    displayImg_ = makeDisplayImage();
    update();
}

void FitsImageView::setPrecomputedImage(const QImage& displayImg, const core::FitsImage& img)
{
    displayImg_ = displayImg;
    // Copy metadata but avoid holding a second copy of the heavy pixel buffer.
    fitsImg_ = img;
    fitsImg_.data.clear();
    fitsImg_.data.shrink_to_fit();
    fitToWidget();
    update();
}

// ─── Static utilities ─────────────────────────────────────────────────────────

QImage FitsImageView::toDisplayImage(const core::FitsImage& img,
                                      bool invert, bool flipH, bool flipV)
{
    if (!img.isValid()) return {};

    const int w = img.width;
    const int h = img.height;
    QImage out(w, h, QImage::Format_Grayscale8);

    const float dmin = img.displayMin;
    const float dmax = img.displayMax;

    for (int y = 0; y < h; ++y) {
        auto* line = out.scanLine(y);
        const float* src = img.data.data() + static_cast<size_t>(y) * w;
        for (int x = 0; x < w; ++x)
            line[x] = core::stretchPixel(src[x], dmin, dmax);
    }

    if (invert) out.invertPixels();
    if (flipH)  out = out.flipped(Qt::Horizontal);
    if (flipV)  out = out.flipped(Qt::Vertical);

    return out;
}

QImage FitsImageView::toThumbnail(const core::FitsImage& img, int size)
{
    if (!img.isValid()) return {};
    const QImage full = toDisplayImage(img);
    if (full.isNull()) return {};
    return full.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
