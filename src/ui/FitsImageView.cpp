#include "FitsImageView.h"
#include "Theme.h"
#include "core/Astronomy.h"

#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QCursor>
#include <QKeyEvent>
#include <QFontMetrics>
#include <cmath>

/// Distinct colour used to mark NaN / undefined pixels (IEEE 754 float NaN).
/// Magenta is the convention in DS9, siril, and other astronomical tools.
static constexpr QRgb kNanColor = 0xFFFF00FF;   // #FF00FF — magenta, full opacity

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
    invert_ = on;
    if (fitsImg_.data.empty()) {
        // Precomputed-image mode (blink): raw data was cleared to save memory.
        // Apply the delta transform directly to the displayed image.
        displayImg_.invertPixels();
    } else {
        displayImg_ = makeDisplayImage();
    }
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
    flipH_ = on;
    if (fitsImg_.data.empty())
        displayImg_ = displayImg_.mirrored(true, false);
    else
        displayImg_ = makeDisplayImage();
    update();
}

void FitsImageView::setFlipV(bool on)
{
    if (flipV_ == on) return;
    flipV_ = on;
    if (fitsImg_.data.empty())
        displayImg_ = displayImg_.mirrored(false, true);
    else
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
        if (showEcliptic_)
            drawEclipticOverlay(p);
    }
    drawAnnotations(p);
    drawMagnifier(p);
    drawRubberBand(p);
}

void FitsImageView::setRegionMode(bool on)
{
    regionMode_    = on;
    drawingRegion_ = false;
    update();
}

void FitsImageView::mouseMoveEvent(QMouseEvent* e)
{
    // Region rubber band takes priority over pan
    if (drawingRegion_) {
        rubberCurrent_ = widgetToImage(e->position());
        update();
        return;
    }

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
    if (regionMode_ && e->button() == Qt::LeftButton) {
        drawingRegion_ = true;
        rubberAnchor_  = widgetToImage(e->position());
        rubberCurrent_ = rubberAnchor_;
        update();
        return;
    }
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
    if (regionMode_ && drawingRegion_ && e->button() == Qt::LeftButton) {
        drawingRegion_ = false;
        const QRectF rf = QRectF(rubberAnchor_, rubberCurrent_).normalized();
        emit regionSelected(rf.toAlignedRect());
        update();
        return;
    }
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
    case Qt::Key_Escape:
        if (regionMode_) { drawingRegion_ = false; regionMode_ = false; update(); }
        emit escapePressed();
        break;
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

void FitsImageView::setStretchMode(core::StretchMode mode)
{
    fitsImg_.stretchMode = mode;
    update();
}

void FitsImageView::setColorLut(core::ColorLut lut)
{
    fitsImg_.colorLut = lut;
    displayImg_ = makeDisplayImage();
    update();
}

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
    // Re-apply active display transforms so that invert/flip persist across
    // frame switches in blink mode (raw data is not available for recompute).
    if (invert_) displayImg_.invertPixels();
    if (flipH_)  displayImg_ = displayImg_.mirrored(true,  false);
    if (flipV_)  displayImg_ = displayImg_.mirrored(false, true);
    // Copy metadata but avoid holding a second copy of the heavy pixel buffer.
    fitsImg_ = img;
    fitsImg_.data.clear();
    fitsImg_.data.shrink_to_fit();
    // Do NOT call fitToWidget() here — blink switches images on every tick and
    // the caller (BlinkWidget) manages fit/zoom state explicitly.
    update();
}

// ─── Rubber band ──────────────────────────────────────────────────────────────

void FitsImageView::drawRubberBand(QPainter& p) const
{
    if (!regionMode_ || !drawingRegion_) return;

    const QPointF wA = imageToWidget(rubberAnchor_);
    const QPointF wC = imageToWidget(rubberCurrent_);
    const QRectF  wr = QRectF(wA, wC).normalized();

    // Semi-transparent fill
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(80, 180, 255, 28));
    p.drawRect(wr);

    // Dashed cyan border
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(0, 200, 255, 200), 1.0, Qt::DashLine));
    p.drawRect(wr);

    // Size readout
    const int iw = qAbs(qRound(rubberCurrent_.x() - rubberAnchor_.x()));
    const int ih = qAbs(qRound(rubberCurrent_.y() - rubberAnchor_.y()));
    if (iw < 2 || ih < 2) return;
    const QString sz = QStringLiteral("%1 × %2 px").arg(iw).arg(ih);
    p.setFont(QFont(QStringLiteral("Monospace"), 8));
    const QPointF tl = wr.topLeft() + QPointF(4, 14);
    p.setPen(QColor(0, 0, 0, 160));
    p.drawText(tl + QPointF(1, 1), sz);
    p.setPen(QColor(200, 230, 255, 220));
    p.drawText(tl, sz);
}

// ─── Static utilities ─────────────────────────────────────────────────────────

/// Map a stretched 8-bit greyscale value to an RGB colour via a false-colour LUT.
/// Viridis uses 8 sampled keypoints with linear interpolation (≈matplotlib viridis).
static QRgb applyColorLut(uint8_t v, core::ColorLut lut) noexcept
{
    switch (lut) {
    case core::ColorLut::Hot: {
        // black → red (0-84) → yellow (85-169) → white (170-255)
        const int r = std::min(255, v * 3);
        const int g = std::clamp(v * 3 - 255, 0, 255);
        const int b = std::clamp(v * 3 - 510, 0, 255);
        return qRgb(r, g, b);
    }
    case core::ColorLut::Cool: {
        // black → dark blue → cyan → white
        const int b = std::min(255, v * 2);
        const int g = std::clamp(v * 2 - 255, 0, 255);
        const int r = std::clamp((v - 192) * 4, 0, 255);
        return qRgb(r, g, b);
    }
    case core::ColorLut::Viridis: {
        // 8 control points from matplotlib viridis, linear interpolation
        static constexpr uint8_t kR[] = { 68,  72,  62,  49,  53, 104, 180, 253};
        static constexpr uint8_t kG[] = {  1,  40,  83, 120, 152, 180, 207, 231};
        static constexpr uint8_t kB[] = { 84, 120, 140, 139, 121,  98,  70,  37};
        const float t = static_cast<float>(v) / 255.0f * 7.0f;
        const int   i = std::clamp(static_cast<int>(t), 0, 6);
        const float f = t - static_cast<float>(i);
        return qRgb(static_cast<int>(kR[i] + f * (kR[i+1] - kR[i])),
                    static_cast<int>(kG[i] + f * (kG[i+1] - kG[i])),
                    static_cast<int>(kB[i] + f * (kB[i+1] - kB[i])));
    }
    case core::ColorLut::Grayscale:
    default:
        return qRgb(v, v, v);
    }
}

QImage FitsImageView::toDisplayImage(const core::FitsImage& img,
                                      bool invert, bool flipH, bool flipV)
{
    if (!img.isValid()) return {};

    const int   w    = img.width;
    const int   h    = img.height;
    const float dmin = img.displayMin;
    const float dmax = img.displayMax;

    // ── NaN detection ─────────────────────────────────────────────────────────
    // Scan the primary channel for any NaN / undefined float values.
    // When present, the output must be RGB32 so we can render them in a
    // distinct colour (magenta, kNanColor).  The check is O(N) but avoids the
    // ~3× memory overhead of RGB32 for the typical NaN-free case.
    // A non-Grayscale LUT also forces RGB32 for greyscale images.
    const auto& scanData = (img.isColor && !img.dataR.empty()) ? img.dataR : img.data;
    const bool hasNan    = std::any_of(scanData.begin(), scanData.end(),
                                       [](float v){ return std::isnan(v); });
    const bool needsLut  = !img.isColor && img.colorLut != core::ColorLut::Grayscale;

    // ── Color image (NAXIS3=3) — render RGB ──────────────────────────────────
    if (img.isColor && !img.dataR.empty()) {
        QImage colorOut(w, h, QImage::Format_RGB32);

        if (img.stretchMode == core::StretchMode::HistEq) {
            // Build LUT from luminance channel, excluding NaN from histogram
            const float range = dmax - dmin;
            constexpr int kBins = 4096;
            std::vector<uint8_t> lut(kBins, 0);
            if (range > 0.0f) {
                std::vector<int> hist(kBins, 0);
                for (float v : img.data) {
                    if (std::isnan(v)) continue;
                    const int bin = std::clamp(
                        static_cast<int>((v - dmin) / range * static_cast<float>(kBins - 1)),
                        0, kBins - 1);
                    ++hist[bin];
                }
                long cdf = 0, cdfMin = 0;
                const long total = static_cast<long>(img.data.size());
                for (int i = 0; i < kBins; ++i) if (hist[i] > 0) { cdfMin = hist[i]; break; }
                for (int i = 0; i < kBins; ++i) {
                    cdf += hist[i];
                    const long den = total - cdfMin;
                    lut[i] = den > 0 ? static_cast<uint8_t>((cdf - cdfMin) * 255 / den) : 0;
                }
            }
            auto applyLut = [&](float v) -> uint8_t {
                if (dmax <= dmin || std::isnan(v)) return 0;
                const int bin = std::clamp(
                    static_cast<int>((v - dmin) / (dmax - dmin) * static_cast<float>(kBins - 1)),
                    0, kBins - 1);
                return lut[bin];
            };
            for (int y = 0; y < h; ++y) {
                auto* line = reinterpret_cast<QRgb*>(colorOut.scanLine(y));
                const size_t off = static_cast<size_t>(y) * w;
                for (int x = 0; x < w; ++x) {
                    if (hasNan && (std::isnan(img.dataR[off + x]) ||
                                   std::isnan(img.dataG[off + x]) ||
                                   std::isnan(img.dataB[off + x])))
                        line[x] = kNanColor;
                    else
                        line[x] = qRgb(applyLut(img.dataR[off + x]),
                                       applyLut(img.dataG[off + x]),
                                       applyLut(img.dataB[off + x]));
                }
            }
        } else {
            for (int y = 0; y < h; ++y) {
                auto* line = reinterpret_cast<QRgb*>(colorOut.scanLine(y));
                const size_t off = static_cast<size_t>(y) * w;
                for (int x = 0; x < w; ++x) {
                    if (hasNan && (std::isnan(img.dataR[off + x]) ||
                                   std::isnan(img.dataG[off + x]) ||
                                   std::isnan(img.dataB[off + x])))
                        line[x] = kNanColor;
                    else
                        line[x] = qRgb(
                            core::stretchPixel(img.dataR[off + x], dmin, dmax, img.stretchMode),
                            core::stretchPixel(img.dataG[off + x], dmin, dmax, img.stretchMode),
                            core::stretchPixel(img.dataB[off + x], dmin, dmax, img.stretchMode));
                }
            }
        }

        if (invert) colorOut.invertPixels();
        if (flipH)  colorOut = colorOut.mirrored(true, false);
        if (flipV)  colorOut = colorOut.mirrored(false, true);
        return colorOut;
    }

    // ── Grayscale — choose output format based on NaN presence ───────────────
    // NaN-free path uses Grayscale8 (fast, 1 byte/pixel).
    // NaN path upgrades to RGB32 so NaN pixels can be shown in kNanColor.

    // ── Histogram equalization ────────────────────────────────────────────────
    if (img.stretchMode == core::StretchMode::HistEq) {
        const float range = dmax - dmin;

        constexpr int kBins = 4096;
        std::vector<int> hist(kBins, 0);
        // Exclude NaN from histogram so they don't skew the CDF
        for (float v : img.data) {
            if (std::isnan(v)) continue;
            if (range <= 0.0f) continue;
            const int bin = std::clamp(
                static_cast<int>((v - dmin) / range * static_cast<float>(kBins - 1)),
                0, kBins - 1);
            ++hist[bin];
        }
        std::vector<uint8_t> lut(kBins);
        long cdf = 0, cdfMin = 0;
        for (int i = 0; i < kBins; ++i) if (hist[i] > 0) { cdfMin = hist[i]; break; }
        const long total = static_cast<long>(img.data.size());
        for (int i = 0; i < kBins; ++i) {
            cdf += hist[i];
            const long num = cdf - cdfMin;
            const long den = total - cdfMin;
            lut[i] = den > 0 ? static_cast<uint8_t>(num * 255 / den) : 0;
        }

        if (hasNan || needsLut) {
            QImage outRgb(w, h, QImage::Format_RGB32);
            for (int y = 0; y < h; ++y) {
                auto* line = reinterpret_cast<QRgb*>(outRgb.scanLine(y));
                const float* src = img.data.data() + static_cast<size_t>(y) * w;
                for (int x = 0; x < w; ++x) {
                    if (std::isnan(src[x])) { line[x] = kNanColor; continue; }
                    if (range <= 0.0f)      { line[x] = applyColorLut(0, img.colorLut); continue; }
                    const int bin = std::clamp(
                        static_cast<int>((src[x] - dmin) / range * static_cast<float>(kBins - 1)),
                        0, kBins - 1);
                    line[x] = applyColorLut(lut[bin], img.colorLut);
                }
            }
            if (invert) outRgb.invertPixels();
            if (flipH)  outRgb = outRgb.mirrored(true, false);
            if (flipV)  outRgb = outRgb.mirrored(false, true);
            return outRgb;
        }

        if (range <= 0.0f) { QImage out(w, h, QImage::Format_Grayscale8); out.fill(0); return out; }
        QImage out(w, h, QImage::Format_Grayscale8);
        for (int y = 0; y < h; ++y) {
            auto* line = out.scanLine(y);
            const float* src = img.data.data() + static_cast<size_t>(y) * w;
            for (int x = 0; x < w; ++x) {
                const int bin = std::clamp(
                    static_cast<int>((src[x] - dmin) / range * static_cast<float>(kBins - 1)),
                    0, kBins - 1);
                line[x] = lut[bin];
            }
        }
        if (invert) out.invertPixels();
        if (flipH)  out = out.mirrored(true, false);
        if (flipV)  out = out.mirrored(false, true);
        return out;
    }

    // ── All other modes — per-pixel stretchPixel ─────────────────────────────
    if (hasNan || needsLut) {
        QImage outRgb(w, h, QImage::Format_RGB32);
        for (int y = 0; y < h; ++y) {
            auto* line = reinterpret_cast<QRgb*>(outRgb.scanLine(y));
            const float* src = img.data.data() + static_cast<size_t>(y) * w;
            for (int x = 0; x < w; ++x) {
                if (std::isnan(src[x])) { line[x] = kNanColor; continue; }
                line[x] = applyColorLut(
                    core::stretchPixel(src[x], dmin, dmax, img.stretchMode),
                    img.colorLut);
            }
        }
        if (invert) outRgb.invertPixels();
        if (flipH)  outRgb = outRgb.mirrored(true, false);
        if (flipV)  outRgb = outRgb.mirrored(false, true);
        return outRgb;
    }

    QImage out(w, h, QImage::Format_Grayscale8);
    for (int y = 0; y < h; ++y) {
        auto* line = out.scanLine(y);
        const float* src = img.data.data() + static_cast<size_t>(y) * w;
        for (int x = 0; x < w; ++x)
            line[x] = core::stretchPixel(src[x], dmin, dmax, img.stretchMode);
    }

    if (invert) out.invertPixels();
    if (flipH)  out = out.mirrored(true, false);
    if (flipV)  out = out.mirrored(false, true);

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
        const bool    hi = isHighlighted(imgPt, highlightPx_);

        if (star.streak) {
            // ── Streak / trail: rotated ellipse + direction tick ──────────────
            // Colour: orange for normal, red when highlighted
            const QColor streakCol = hi ? QColor(255, 80, 40) : QColor(255, 160, 0);
            p.setPen(QPen(streakCol, hi ? 2.5 : 1.5));

            const double ra = std::max(4.0, star.a * zoom_);
            const double rb = std::max(2.0, star.b * zoom_);
            // SEP theta: CCW from +x in image coords; QPainter rotate: CW in widget coords
            const double thetaDeg = star.theta * (180.0 / M_PI);

            p.save();
            p.translate(w);
            p.rotate(-thetaDeg);          // convert CCW → CW for QPainter (y-down)
            p.drawEllipse(QPointF(0, 0), ra, rb);
            // Tick along major axis to show trail direction
            p.drawLine(QPointF(-ra, 0.0), QPointF(ra, 0.0));
            p.restore();
        } else if (star.blended) {
            // ── Blended source: double circle (inner + outer ring) ─────────────
            // Colour: magenta for normal, red when highlighted
            const QColor blendCol = hi ? QColor(255, 60, 60) : QColor(220, 80, 255);
            const double r = std::max(4.0, star.a * zoom_);
            p.setPen(QPen(blendCol, hi ? 2.5 : 1.5));
            p.drawEllipse(w, r,           r);
            p.drawEllipse(w, r + 4.0,     r + 4.0);
        } else {
            // ── Normal point source: circle ───────────────────────────────────
            const double r = std::max(4.0, star.a * zoom_);
            p.setPen(QPen(hi ? QColor(255, 60, 60) : Theme::markDetectedStarDark(), hi ? 2.5 : 1.5));
            p.drawEllipse(w, hi ? r + 3.0 : r, hi ? r + 3.0 : r);
        }
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
        p.setPen(QPen(hi ? QColor(255, 60, 60) : Theme::markCatalogStarDark(), hi ? 2.0 : 1.0));
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

    static const QColor kAsteroidColor = Theme::markKooAsteroidDark();
    static const QColor kPlanetColor   = Theme::markKooPlanet();
    static const QColor kCometColor    = Theme::markKooComet();

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

// ─── drawEclipticOverlay ──────────────────────────────────────────────────────

void FitsImageView::drawEclipticOverlay(QPainter& p) const
{
    if (!fitsImg_.wcs.solved) return;

    const double margin = 50.0;   // widget-pixel margin for clipping extended segments
    const QRectF visible(-margin, -margin,
                          width()  + 2 * margin,
                          height() + 2 * margin);

    // Helper: ecliptic/galactic longitude (degrees) → widget point via WCS
    auto toWidget = [&](double ra, double dec) -> QPointF {
        double px, py;
        fitsImg_.wcs.skyToPix(ra, dec, px, py);
        return imageToWidget(QPointF(px - 1.0, py - 1.0));  // WCS is 1-based
    };

    // Build a polyline for a great-circle sampled at kSteps points.
    // Breaks the path when consecutive points jump > kMaxJump px (wrap-around).
    auto buildPath = [&](auto coordFn) -> QPainterPath {
        constexpr int    kSteps   = 360;
        constexpr double kMaxJump = 200.0;   // px — larger than the image diagonal is impossible
        QPainterPath path;
        bool started = false;
        QPointF prev;
        for (int i = 0; i <= kSteps; ++i) {
            const double lon = static_cast<double>(i);   // 0–360 degrees
            auto [ra, dec] = coordFn(lon);
            const QPointF w = toWidget(ra, dec);
            if (!visible.contains(w)) { started = false; continue; }
            if (started && QLineF(prev, w).length() > kMaxJump) { started = false; }
            if (!started) { path.moveTo(w); started = true; }
            else            path.lineTo(w);
            prev = w;
        }
        return path;
    };

    // ── Ecliptic plane (β = 0) ────────────────────────────────────────────────
    const QPainterPath eclPath = buildPath([](double lon) -> std::pair<double,double> {
        double ra, dec;
        core::eclipticToEquatorial(lon, 0.0, ra, dec);
        return { ra, dec };
    });

    p.setPen(QPen(QColor(200, 230, 60, 220), 1.5, Qt::DotLine));
    p.setBrush(Qt::NoBrush);
    p.drawPath(eclPath);

    // Label "Eclíptica" at the first visible point
    if (!eclPath.isEmpty()) {
        p.setPen(QColor(200, 230, 60, 200));
        p.setFont(QFont(QStringLiteral("sans"), 8));
        p.drawText(eclPath.elementAt(0).x + 4, eclPath.elementAt(0).y - 4,
                   tr("Eclíptica"));
    }

    // ── Galactic plane (b = 0) ────────────────────────────────────────────────
    // Pre-build equatorial coordinates of b=0 band
    const QPainterPath galPath = buildPath([](double lon) -> std::pair<double,double> {
        // Invert equatorialToGalactic: galactic plane at b=0, l=lon → equatorial
        // Use the standard rotation: RA = αNGP + atan2(cos(b)*sin(l-lSun), ...)
        // Numerically: solve back from the galactic definition.
        // Galactic north pole in equatorial J2000:
        constexpr double kAlphaGP = 192.859508 * M_PI / 180.0;
        constexpr double kDeltaGP =  27.128336 * M_PI / 180.0;
        constexpr double kLcp     = 122.932    * M_PI / 180.0;
        const double l = lon * M_PI / 180.0;    // b = 0, so sin(b)=0, cos(b)=1
        // Forward from galactic (l, b=0): unit vector in equatorial
        // x_G = (cos(b)cos(l), cos(b)sin(l), sin(b))  in galactic frame
        // Rotation from equatorial to galactic:
        //   b = asin(sin(d)*sin(dGP) + cos(d)*cos(dGP)*cos(a - aGP))
        //   l = lCP - atan2(cos(d)*sin(a-aGP), sin(d)*cos(dGP) - cos(d)*sin(dGP)*cos(a-aGP))
        // Inverse (b=0):
        //   sin(d)*sin(dGP) + cos(d)*cos(dGP)*cos(a-aGP) = 0   [sin b = 0]
        //   lCP - atan2(...) = l
        // (psi = kLcp - l not needed in the matrix path below)
        // ψ = atan2(cos(d)*sin(a-aGP),  sin(d)*cos(dGP) - cos(d)*sin(dGP)*cos(a-aGP))
        // From sin(b)=0: sin(d) = -cos(d)*cos(dGP)/sin(dGP) * cos(a-aGP)
        // => tan(d) = -cos(dGP)*cos(a-aGP) / sin(dGP)
        // let u = a - aGP, then:
        //   sin(d) = -cos(dGP)*cos(u) / sqrt(1 + (cos(dGP)*cos(u)/sin(dGP))^2 ) ... complex
        // Simplest: use the rotation matrix directly.
        // Unit vector in galactic frame at (l, b=0):
        const double gx =  std::cos(l);
        const double gy =  std::sin(l);
        const double gz =  0.0;        // sin(b) = 0
        // Galactic → equatorial rotation matrix (transposed of eq→gal):
        // This is the standard IAU matrix M^T where:
        // M = {{-sin(aGP)*sin(lCP)-cos(aGP)*cos(lCP)*sin(dGP),
        //        sin(aGP)*cos(lCP)-cos(aGP)*sin(lCP)*sin(dGP),
        //        cos(aGP)*cos(dGP)},
        //      { cos(aGP)*sin(lCP)-sin(aGP)*cos(lCP)*sin(dGP),
        //       -cos(aGP)*cos(lCP)-sin(aGP)*sin(lCP)*sin(dGP),
        //        sin(aGP)*cos(dGP)},
        //      { cos(lCP)*cos(dGP),
        //        sin(lCP)*cos(dGP),
        //        sin(dGP)}}
        // ex in equatorial = M^T * (gx,gy,gz)
        const double saGP = std::sin(kAlphaGP), caGP = std::cos(kAlphaGP);
        const double sdGP = std::sin(kDeltaGP), cdGP = std::cos(kDeltaGP);
        const double slCP = std::sin(kLcp),     clCP = std::cos(kLcp);
        const double ex = (-saGP*slCP - caGP*clCP*sdGP)*gx
                        + ( caGP*slCP - saGP*clCP*sdGP)*gy
                        + ( clCP*cdGP                 )*gz;
        const double ey = ( saGP*clCP - caGP*slCP*sdGP)*gx
                        + (-caGP*clCP - saGP*slCP*sdGP)*gy
                        + ( slCP*cdGP                 )*gz;
        const double ez = ( caGP*cdGP                 )*gx
                        + ( saGP*cdGP                 )*gy
                        + ( sdGP                      )*gz;
        double ra  = std::fmod(std::atan2(ey, ex) * 180.0 / M_PI + 360.0, 360.0);
        double dec = std::asin(std::clamp(ez, -1.0, 1.0)) * 180.0 / M_PI;
        return { ra, dec };
    });

    p.setPen(QPen(QColor(100, 180, 255, 200), 1.5, Qt::DashDotLine));
    p.drawPath(galPath);

    if (!galPath.isEmpty()) {
        p.setPen(QColor(100, 180, 255, 200));
        p.drawText(galPath.elementAt(0).x + 4, galPath.elementAt(0).y - 4,
                   tr("Via Láctea"));
    }

    // ── Galactic-plane proximity warning ─────────────────────────────────────
    // Compute galactic latitude of the field centre.
    double l_ctr, b_ctr;
    core::equatorialToGalactic(fitsImg_.ra, fitsImg_.dec, l_ctr, b_ctr);

    constexpr double kWarnBDeg = 15.0;   // |b| < 15° → high extinction
    if (std::abs(b_ctr) < kWarnBDeg) {
        const QString msg = tr("⚠  Plano galáctico — alta extinção interestelar  (b = %1°)")
                            .arg(b_ctr, 0, 'f', 1);

        QFont f(QStringLiteral("sans"), 9);
        f.setBold(true);
        const QFontMetrics fm(f);
        const QRect tr2 = fm.boundingRect(msg);
        const int pad = 6;
        const QRect badge(12, height() - tr2.height() - pad * 2 - 12,
                          tr2.width() + pad * 2, tr2.height() + pad * 2);

        p.setFont(f);
        p.setBrush(QColor(0, 0, 0, 160));
        p.setPen(QPen(QColor(255, 180, 0, 220), 1));
        p.drawRoundedRect(badge, 4, 4);
        p.setPen(QColor(255, 200, 60, 230));
        p.drawText(badge.adjusted(pad, pad, -pad, -pad), Qt::AlignVCenter, msg);
    }
}
