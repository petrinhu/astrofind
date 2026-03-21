#include "FitsImageView.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QCursor>
#include <QKeyEvent>

/// Builds a crosshair cursor with the given line color and outline color.
/// Normal (dark image): white line + black outline.
/// Inverted (light image): black line + white outline.
static QCursor makeCrossCursor(bool inverted)
{
    const QColor line    = inverted ? QColor(0,   0,   0)   : QColor(255, 255, 255);
    const QColor outline = inverted ? QColor(255, 255, 255) : QColor(0,   0,   0);
    const int sz = 32, cx = sz / 2;
    QPixmap pm(sz, sz);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setPen(QPen(outline, 3, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(cx, 2, cx, sz - 3);
    p.drawLine(2, cx, sz - 3, cx);
    p.setPen(QPen(line, 1, Qt::SolidLine, Qt::FlatCap));
    p.drawLine(cx, 2, cx, sz - 3);
    p.drawLine(2, cx, sz - 3, cx);
    return QCursor(pm, cx, cx);
}

FitsImageView::FitsImageView(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setMinimumSize(200, 200);
    setAttribute(Qt::WA_OpaquePaintEvent);
    defaultCursor_ = makeCrossCursor(false);
    setCursor(defaultCursor_);
}

void FitsImageView::setFitsImage(const core::FitsImage& img)
{
    fitsImg_    = img;
    displayImg_ = makeDisplayImage();
    userZoomed_ = false;
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
    // Use width/height directly — data may be cleared in precomputed-image mode
    if (fitsImg_.width <= 0 || fitsImg_.height <= 0 || width() <= 0 || height() <= 0) { zoom_ = 1.0; return; }
    const double zx = static_cast<double>(width())  / fitsImg_.width;
    const double zy = static_cast<double>(height()) / fitsImg_.height;
    zoom_       = std::min(zx, zy);
    userZoomed_ = false;
    panOff_     = QPointF(
        (width()  - fitsImg_.width  * zoom_) / 2.0,
        (height() - fitsImg_.height * zoom_) / 2.0
    );
    update();
}

void FitsImageView::centerOnPixel(QPointF imagePt)
{
    if (!fitsImg_.isValid()) return;
    const QPointF center(width() / 2.0, height() / 2.0);
    panOff_ = center - QPointF(imagePt.x() * zoom_, imagePt.y() * zoom_);
    clampPan();
    update();
}

void FitsImageView::setZoom(double z)
{
    if (fitsImg_.width <= 0 || fitsImg_.height <= 0) return;
    z = std::clamp(z, 0.05, 32.0);
    // Zoom around widget center
    const QPointF center(width() / 2.0, height() / 2.0);
    const QPointF imgPt = widgetToImage(center);
    zoom_       = z;
    userZoomed_ = true;
    panOff_ = center - QPointF(imgPt.x() * zoom_, imgPt.y() * zoom_);
    clampPan();
    update();
}

void FitsImageView::setInvert(bool on)
{
    if (invert_ == on) return;
    invert_     = on;
    displayImg_ = makeDisplayImage();
    // Keep cross cursor visible on both dark and inverted images
    if (defaultCursor_.shape() == Qt::CustomCursor || defaultCursor_.shape() == Qt::CrossCursor) {
        defaultCursor_ = makeCrossCursor(on);
        setCursor(defaultCursor_);
    }
    update();
}

void FitsImageView::setToolCursor(Qt::CursorShape shape)
{
    if (shape == Qt::CrossCursor)
        defaultCursor_ = makeCrossCursor(invert_);
    else
        defaultCursor_ = QCursor(shape);
    setCursor(defaultCursor_);
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

    if (showOverlay_) {
        p.setRenderHint(QPainter::Antialiasing, true);
        drawCatalogStars(p);
        drawDetectedStars(p);
        drawKooObjects(p);
    }
    drawAnnotations(p);
    drawMagnifier(p);
}

void FitsImageView::mouseMoveEvent(QMouseEvent* e)
{
    const QPointF imgPos = widgetToImage(e->position());

    // Middle-button drag pan
    if (panning_ && (e->buttons() & Qt::MiddleButton)) {
        panOff_ = panOffStart_ + QPointF(e->pos() - panStart_);
        clampPan();
        update();
    }
    // Left-button drag pan (activates after 4 px movement)
    if (e->buttons() & Qt::LeftButton) {
        if (!leftDragPanning_ && (e->pos() - leftPressPos_).manhattanLength() > 4) {
            leftDragPanning_ = true;
        }
        if (leftDragPanning_) {
            panOff_ = panOffStart_ + QPointF(e->pos() - panStart_);
            clampPan();
            update();
        }
    }

    if (fitsImg_.isValid()) {
        const int ix = static_cast<int>(imgPos.x());
        const int iy = static_cast<int>(imgPos.y());
        const float val = fitsImg_.pixelAt(ix, iy);
        double ra = 0.0, dec = 0.0;
        pixToSky(imgPos, ra, dec);
        emit cursorMoved(ra, dec, val);
    }

    if (showMagnifier_) {
        magnifierCursor_ = e->position();
        update();
    }
}

void FitsImageView::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        // Start tracking — drag > 4px becomes a pan, otherwise it's a click
        leftPressPos_    = e->pos();
        leftDragPanning_ = false;
        panStart_        = e->pos();
        panOffStart_     = panOff_;
        setFocus();
    } else if (e->button() == Qt::MiddleButton) {
        panning_     = true;
        panStart_    = e->pos();
        panOffStart_ = panOff_;
        setCursor(Qt::ClosedHandCursor);
    }
}

void FitsImageView::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton) {
        if (!leftDragPanning_ && fitsImg_.isValid()) {
            // Tap (no drag) → emit click
            const QPointF imgPos = widgetToImage(e->position());
            const int ix = static_cast<int>(imgPos.x());
            const int iy = static_cast<int>(imgPos.y());
            const float val = fitsImg_.pixelAt(ix, iy);
            double ra = 0.0, dec = 0.0;
            pixToSky(imgPos, ra, dec);
            emit pixelClicked(imgPos, ra, dec, val);
        }
        leftDragPanning_ = false;
        setCursor(defaultCursor_);
    } else if (e->button() == Qt::MiddleButton) {
        panning_ = false;
        setCursor(defaultCursor_);
    }
}

void FitsImageView::wheelEvent(QWheelEvent* e)
{
    const double factor = e->angleDelta().y() > 0 ? 1.15 : 1.0 / 1.15;
    // Zoom around cursor position
    const QPointF wp  = e->position();
    const QPointF ip  = widgetToImage(wp);
    zoom_ = std::clamp(zoom_ * factor, 0.05, 32.0);
    panOff_ = wp - QPointF(ip.x() * zoom_, ip.y() * zoom_);
    userZoomed_ = true;
    clampPan();
    update();
    e->accept();
}

void FitsImageView::resizeEvent(QResizeEvent* /*e*/)
{
    if (displayImg_.isNull()) return;
    // Refit unless the user has explicitly zoomed — avoids resetting after MDI tile
    if (!userZoomed_) fitToWidget();
}

void FitsImageView::processKey(QKeyEvent* e)
{
    const double step = 10.0;
    bool handled = true;
    switch (e->key()) {
    case Qt::Key_Left:  panOff_.rx() -= step; clampPan(); update(); break;
    case Qt::Key_Right: panOff_.rx() += step; clampPan(); update(); break;
    case Qt::Key_Up:    panOff_.ry() -= step; clampPan(); update(); break;
    case Qt::Key_Down:  panOff_.ry() += step; clampPan(); update(); break;
    case Qt::Key_Plus:
    case Qt::Key_Equal: userZoomed_ = true; zoomIn();  break;
    case Qt::Key_Minus: userZoomed_ = true; zoomOut(); break;
    case Qt::Key_0:      userZoomed_ = false; fitToWidget(); update(); break;
    case Qt::Key_Escape: emit escapePressed(); break;
    default: handled = false; break;
    }
    if (handled) e->accept();
}

void FitsImageView::keyPressEvent(QKeyEvent* e)
{
    processKey(e);
    if (!e->isAccepted()) QWidget::keyPressEvent(e);
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
    if (fitsImg_.width <= 0 || fitsImg_.height <= 0) return;
    const double iw = fitsImg_.width  * zoom_;
    const double ih = fitsImg_.height * zoom_;
    // When image fits inside the viewport, keep it centered
    if (iw <= width())
        panOff_.setX((width()  - iw) / 2.0);
    else
        panOff_.setX(std::clamp(panOff_.x(), -iw * 0.8, width()  - iw * 0.2));
    if (ih <= height())
        panOff_.setY((height() - ih) / 2.0);
    else
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
    // Do NOT call fitToWidget() here — blink switches images on every tick and
    // the caller (BlinkWidget) manages fit/zoom state explicitly.
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

// ─── Overlay control ──────────────────────────────────────────────────────────

void FitsImageView::setShowOverlay(bool on)
{
    if (showOverlay_ == on) return;
    showOverlay_ = on;
    update();
}

// ─── Overlay drawing ──────────────────────────────────────────────────────────

// Returns true if imgPt is within kHighlightThreshold pixels of the highlight position.
static constexpr double kHighlightThreshold = 5.0;
static inline bool isHighlighted(QPointF imgPt, QPointF h)
{
    const double dx = imgPt.x() - h.x(), dy = imgPt.y() - h.y();
    return dx * dx + dy * dy <= kHighlightThreshold * kHighlightThreshold;
}

void FitsImageView::drawDetectedStars(QPainter& p) const
{
    if (!showDetected_ || fitsImg_.detectedStars.isEmpty()) return;

    p.setBrush(Qt::NoBrush);
    for (const auto& star : fitsImg_.detectedStars) {
        const QPointF imgPt(star.x, star.y);
        const QPointF w = imageToWidget(imgPt);
        const double  r = std::max(4.0, star.a * zoom_);
        const bool    hi = isHighlighted(imgPt, highlightPx_);
        p.setPen(QPen(hi ? QColor(255, 60, 60) : QColor(0, 220, 255), hi ? 2.5 : 1.5));
        p.drawEllipse(w, hi ? r + 3.0 : r, hi ? r + 3.0 : r);
    }
}

void FitsImageView::drawCatalogStars(QPainter& p) const
{
    if (!showCatalog_ || fitsImg_.catalogStars.isEmpty() || !fitsImg_.wcs.solved) return;

    for (const auto& star : fitsImg_.catalogStars) {
        double px = 0.0, py = 0.0;
        // skyToPix returns 1-based FITS coords → subtract 1 for 0-based display
        fitsImg_.wcs.skyToPix(star.ra, star.dec, px, py);
        const QPointF imgPt(px - 1.0, py - 1.0);
        const QPointF w = imageToWidget(imgPt);
        // Skip stars outside the visible image area
        if (!rect().marginsAdded(QMargins(20, 20, 20, 20)).contains(w.toPoint())) continue;
        const bool    hi = isHighlighted(imgPt, highlightPx_);
        const double  s = hi ? 7.0 : 4.0;
        p.setPen(QPen(hi ? QColor(255, 60, 60) : QColor(255, 220, 0), hi ? 2.0 : 1.0));
        p.drawLine(w + QPointF(-s, 0), w + QPointF(s, 0));
        p.drawLine(w + QPointF(0, -s), w + QPointF(0, s));
    }
}

// ─── Annotations ──────────────────────────────────────────────────────────────

void FitsImageView::setHighlight(QPointF imgPx)
{
    highlightPx_ = imgPx;
    update();
}

void FitsImageView::clearHighlight()
{
    highlightPx_ = {-1e9, -1e9};
    update();
}

void FitsImageView::addAnnotation(const QPointF& imgPos, const QString& text)
{
    annotations_.append({ imgPos, text });
    update();
}

void FitsImageView::clearAnnotations()
{
    annotations_.clear();
    update();
}

void FitsImageView::drawAnnotations(QPainter& p) const
{
    if (annotations_.isEmpty()) return;

    const QFont font(QStringLiteral("sans-serif"), 9, QFont::Bold);
    p.setFont(font);

    for (const auto& ann : annotations_) {
        const QPointF w = imageToWidget(ann.pos);

        // Small diamond marker
        p.setPen(QPen(QColor(255, 80, 255), 1.5));   // magenta
        p.setBrush(Qt::NoBrush);
        const double d = 5.0;
        const QPolygonF diamond = {
            w + QPointF(0, -d), w + QPointF(d, 0),
            w + QPointF(0,  d), w + QPointF(-d, 0)
        };
        p.drawPolygon(diamond);

        // Text with shadow for readability
        const QPointF textPos = w + QPointF(d + 3.0, 4.0);
        p.setPen(QColor(0, 0, 0, 180));
        p.drawText(textPos + QPointF(1, 1), ann.text);
        p.setPen(QColor(255, 80, 255));
        p.drawText(textPos, ann.text);
    }
}

// ─── Magnifying glass ─────────────────────────────────────────────────────────

void FitsImageView::drawMagnifier(QPainter& p) const
{
    if (!showMagnifier_ || displayImg_.isNull()) return;

    static constexpr int    kRadius    = 72;   // magnifier circle radius, px
    static constexpr double kMagFactor = 4.0;  // zoom factor relative to current zoom

    // Position magnifier above-right of cursor; clamp to widget bounds
    QPointF mc = magnifierCursor_ + QPointF(kRadius + 18, -(kRadius + 18));
    mc.setX(std::clamp(mc.x(), static_cast<double>(kRadius + 2),
                                static_cast<double>(width()  - kRadius - 2)));
    mc.setY(std::clamp(mc.y(), static_cast<double>(kRadius + 2),
                                static_cast<double>(height() - kRadius - 2)));

    // Source rectangle in display image pixels centered on cursor
    const QPointF imgPos = widgetToImage(magnifierCursor_);
    const double  srcHalf = kRadius / kMagFactor;
    const QRectF  srcRect(imgPos.x() - srcHalf, imgPos.y() - srcHalf,
                          srcHalf * 2.0,         srcHalf * 2.0);
    const QRectF  dstRect(mc.x() - kRadius, mc.y() - kRadius,
                          kRadius * 2,        kRadius * 2);

    // Clip to circle
    QPainterPath clip;
    clip.addEllipse(mc, kRadius, kRadius);
    p.save();
    p.setClipPath(clip);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.drawImage(dstRect, displayImg_, srcRect);
    p.restore();

    // Border + shadow
    p.setPen(QPen(QColor(0, 0, 0, 120), 5.0));
    p.setBrush(Qt::NoBrush);
    p.drawEllipse(mc, kRadius, kRadius);

    p.setPen(QPen(QColor(0, 232, 255), 2.0));
    p.drawEllipse(mc, kRadius, kRadius);

    // Crosshair at center
    const double ch = 9.0;
    p.setPen(QPen(QColor(255, 80, 80), 1.2));
    p.drawLine(mc + QPointF(-ch, 0), mc + QPointF(ch, 0));
    p.drawLine(mc + QPointF(0, -ch), mc + QPointF(0, ch));

    // Pixel coordinates label
    const int ix = static_cast<int>(imgPos.x());
    const int iy = static_cast<int>(imgPos.y());
    const QString label = QStringLiteral("(%1, %2)").arg(ix).arg(iy);
    p.setFont(QFont(QStringLiteral("monospace"), 7));
    p.setPen(QColor(0, 0, 0, 160));
    p.drawText(QPointF(mc.x() - kRadius + 5 + 1, mc.y() + kRadius - 6 + 1), label);
    p.setPen(QColor(200, 200, 200));
    p.drawText(QPointF(mc.x() - kRadius + 5, mc.y() + kRadius - 6), label);
}

void FitsImageView::drawKooObjects(QPainter& p) const
{
    if (!showKoo_ || fitsImg_.kooObjects.isEmpty() || !fitsImg_.wcs.solved) return;

    static const QColor kAsteroidColor(80, 255, 120);   // green
    static const QColor kPlanetColor(120, 180, 255);    // light blue
    static const QColor kCometColor(255, 160, 80);      // orange

    const QFont labelFont(QStringLiteral("monospace"), 7);
    p.setFont(labelFont);

    for (const auto& obj : fitsImg_.kooObjects) {
        double px = 0.0, py = 0.0;
        fitsImg_.wcs.skyToPix(obj.ra, obj.dec, px, py);
        const QPointF imgPt(px - 1.0, py - 1.0);
        const QPointF w = imageToWidget(imgPt);
        if (!rect().marginsAdded(QMargins(20, 20, 20, 20)).contains(w.toPoint())) continue;

        const bool    hi  = isHighlighted(imgPt, highlightPx_);
        const QColor  col = hi ? QColor(255, 60, 60)
                          : (obj.type == QLatin1String("planet")) ? kPlanetColor
                          : (obj.type == QLatin1String("comet"))  ? kCometColor
                          : kAsteroidColor;
        const double r = hi ? 13.0 : 10.0;
        p.setPen(QPen(col, hi ? 2.5 : 2.0));
        p.setBrush(Qt::NoBrush);
        p.drawEllipse(w, r, r);

        // Label (name or MPC number)
        if (showLabels_) {
            p.setPen(col);
            const QString label = obj.number > 0
                ? QString::number(obj.number)
                : obj.name;
            p.drawText(w + QPointF(r + 2.0, 4.0), label);
        }
    }
}
