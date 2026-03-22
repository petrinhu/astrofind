#include "BlinkWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QMouseEvent>

#include <algorithm>
#include <cmath>
#include <vector>

BlinkWidget::BlinkWidget(QWidget* parent)
    : QWidget(parent)
    , blinkTimer_(new QTimer(this))
{
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("BlinkWidget { background: #0d1117; }");
    buildUi();
    connect(blinkTimer_, &QTimer::timeout, this, &BlinkWidget::onBlinkTick);
}

void BlinkWidget::buildUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Top info bar ───────────────────────────────────────────────────────
    auto* infoBar = new QWidget(this);
    infoBar->setFixedHeight(28);
    infoBar->setStyleSheet("QWidget { background:#161b22; }");
    auto* infoLay = new QHBoxLayout(infoBar);
    infoLay->setContentsMargins(8, 0, 8, 0);
    infoLay->setSpacing(16);

    lblCounter_  = new QLabel(this);
    lblCounter_->setStyleSheet("color:#58a6ff; font-weight:bold;");
    lblFilename_ = new QLabel(this);
    lblFilename_->setStyleSheet("color:#e6edf3; font-weight:bold;");
    lblJD_       = new QLabel(this);
    lblJD_->setStyleSheet("color:#8b949e;");
    lblRADec_    = new QLabel(this);
    lblRADec_->setStyleSheet("color:#3fb950; font-family:monospace;");

    infoLay->addWidget(lblCounter_);
    infoLay->addWidget(lblFilename_);
    infoLay->addWidget(lblJD_);
    infoLay->addStretch();
    infoLay->addWidget(lblRADec_);

    // ── Image view ─────────────────────────────────────────────────────────
    view_ = new FitsImageView(this);
    connect(view_, &FitsImageView::pixelClicked, this, &BlinkWidget::onImageClicked);
    connect(view_, &FitsImageView::cursorMoved,  this, &BlinkWidget::onCursorMoved);

    // ── Bottom control bar ─────────────────────────────────────────────────
    auto* ctrlBar = new QWidget(this);
    ctrlBar->setFixedHeight(56);
    ctrlBar->setStyleSheet("QWidget { background:#161b22; }");
    auto* ctrlLay = new QHBoxLayout(ctrlBar);
    ctrlLay->setContentsMargins(8, 4, 8, 4);
    ctrlLay->setSpacing(6);

    const QString btnStyle =
        "QPushButton { background:#21262d; color:#e6edf3; border:1px solid #30363d;"
        " border-radius:4px; font-size:13px; padding:0 6px; }"
        "QPushButton:hover { background:#30363d; }"
        "QPushButton:pressed { background:#0d1117; }";

    btnPrev_ = new QPushButton(tr("◀"), ctrlBar);   btnPrev_->setFixedSize(32, 32); btnPrev_->setStyleSheet(btnStyle);
    btnPlay_ = new QPushButton(tr("⏸"), ctrlBar);   btnPlay_->setFixedSize(40, 32); btnPlay_->setStyleSheet(btnStyle);
    btnNext_ = new QPushButton(tr("▶"), ctrlBar);   btnNext_->setFixedSize(32, 32); btnNext_->setStyleSheet(btnStyle);
    btnStop_ = new QPushButton(tr("■ Stop"), ctrlBar); btnStop_->setFixedSize(72, 32);
    btnStop_->setStyleSheet(btnStyle + "QPushButton { color:#f85149; border-color:#f85149; }");

    btnSharpen_ = new QPushButton(tr("Sharpen: Off"), ctrlBar);
    btnSharpen_->setFixedHeight(32);
    btnSharpen_->setStyleSheet(btnStyle);
    btnSharpen_->setToolTip(tr("Cycle: Off → Unsharp Mask → Laplacian → Off"));

    lblSpeed_ = new QLabel(tr("Speed: 500ms"), ctrlBar);
    lblSpeed_->setStyleSheet("color:#8b949e; min-width:100px;");

    sldSpeed_ = new QSlider(Qt::Horizontal, ctrlBar);
    sldSpeed_->setRange(50, 2000);
    sldSpeed_->setValue(500);
    sldSpeed_->setFixedWidth(150);
    sldSpeed_->setInvertedAppearance(true);
    sldSpeed_->setStyleSheet(
        "QSlider::groove:horizontal { height:4px; background:#30363d; border-radius:2px; }"
        "QSlider::handle:horizontal { width:14px; height:14px; background:#58a6ff;"
        " border-radius:7px; margin:-5px 0; }"
        "QSlider::sub-page:horizontal { background:#1f6feb; border-radius:2px; }");

    // Thumbnail strip
    auto* thumbWidget = new QWidget(ctrlBar);
    auto* thumbLay    = new QHBoxLayout(thumbWidget);
    thumbLay->setContentsMargins(0, 0, 0, 0);
    thumbLay->setSpacing(4);
    for (int i = 0; i < 4; ++i) {
        auto* lbl = new QLabel(thumbWidget);
        lbl->setFixedSize(42, 42);
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setStyleSheet("background:#0d1117; border:1px solid #30363d; border-radius:3px; color:#555;");
        lbl->setText(QString::number(i + 1));
        lbl->setCursor(Qt::PointingHandCursor);
        lbl->setProperty("thumbIndex", i);
        lbl->installEventFilter(this);
        thumbLabels_.append(lbl);
        thumbLay->addWidget(lbl);
    }

    ctrlLay->addWidget(btnPrev_);
    ctrlLay->addWidget(btnPlay_);
    ctrlLay->addWidget(btnNext_);
    ctrlLay->addWidget(btnStop_);
    ctrlLay->addSpacing(8);
    ctrlLay->addWidget(btnSharpen_);
    ctrlLay->addSpacing(16);
    ctrlLay->addWidget(lblSpeed_);
    ctrlLay->addWidget(sldSpeed_);
    ctrlLay->addStretch();
    ctrlLay->addWidget(thumbWidget);

    root->addWidget(infoBar);
    root->addWidget(view_, 1);
    root->addWidget(ctrlBar);

    // Connections
    connect(btnPrev_, &QPushButton::clicked, this, &BlinkWidget::showPrevious);
    connect(btnNext_, &QPushButton::clicked, this, &BlinkWidget::showNext);
    connect(btnStop_, &QPushButton::clicked, this, &BlinkWidget::stopRequested);
    connect(btnPlay_, &QPushButton::clicked, this, [this] {
        if (isBlinking()) stopBlink(); else startBlink();
    });
    connect(sldSpeed_, &QSlider::valueChanged, this, &BlinkWidget::onSpeedChanged);
    connect(btnSharpen_, &QPushButton::clicked, this, [this] {
        // Cycle: None → UnsharpMask → Laplacian → None
        switch (sharpenMode_) {
        case BlinkSharpenMode::None:
            sharpenMode_ = BlinkSharpenMode::UnsharpMask;
            btnSharpen_->setText(tr("Sharpen: USM"));
            break;
        case BlinkSharpenMode::UnsharpMask:
            sharpenMode_ = BlinkSharpenMode::Laplacian;
            btnSharpen_->setText(tr("Sharpen: LoG"));
            break;
        case BlinkSharpenMode::Laplacian:
            sharpenMode_ = BlinkSharpenMode::None;
            btnSharpen_->setText(tr("Sharpen: Off"));
            break;
        }
        reapplySharpen();
    });
}

// ── Image management ──────────────────────────────────────────────────────────

void BlinkWidget::setImages(const QVector<core::FitsImage>& images)
{
    stopBlink();
    images_      = images;
    precomputed_.clear();
    precomputed_.reserve(images_.size());

    for (const auto& img : images_) {
        precomputed_.append(applySharpening(FitsImageView::toDisplayImage(img), sharpenMode_));
    }

    for (int i = 0; i < thumbLabels_.size(); ++i) {
        if (i < images_.size()) {
            const QImage thumb = FitsImageView::toThumbnail(images_[i], 42);
            thumbLabels_[i]->setPixmap(QPixmap::fromImage(thumb));
            thumbLabels_[i]->setToolTip(images_[i].fileName);
        } else {
            thumbLabels_[i]->clear();
            thumbLabels_[i]->setText(QString::number(i + 1));
            thumbLabels_[i]->setStyleSheet(
                "background:#0d1117; border:1px solid #30363d; border-radius:3px; color:#555;");
        }
    }

    currentIdx_ = 0;
    if (!images_.isEmpty()) showImage(0);
}

void BlinkWidget::updateImageMetadata(const QVector<core::FitsImage>& images)
{
    // Update only the metadata (overlays, WCS) without recomputing pixel data.
    const int n = static_cast<int>(std::min(images.size(), images_.size()));
    for (int i = 0; i < n; ++i)
        images_[i] = images[i];
    // Refresh the currently displayed frame so overlays update immediately.
    if (currentIdx_ < images_.size())
        view_->setPrecomputedImage(precomputed_[currentIdx_], images_[currentIdx_]);
}

void BlinkWidget::clearImages()
{
    stopBlink();
    images_.clear();
    precomputed_.clear();
    view_->clearImage();
    currentIdx_ = 0;
}

// ── Blink control ─────────────────────────────────────────────────────────────

void BlinkWidget::startBlink()
{
    if (images_.size() < 2) return;
    blinkTimer_->start(sldSpeed_->value());
    btnPlay_->setText(tr("⏸"));
}

void BlinkWidget::stopBlink()
{
    blinkTimer_->stop();
    btnPlay_->setText(tr("▶"));
}

void BlinkWidget::showImage(int index)
{
    if (images_.isEmpty()) return;
    currentIdx_ = ((index % static_cast<int>(images_.size())) + static_cast<int>(images_.size())) % static_cast<int>(images_.size());

    // Preserve zoom/pan across blink ticks so the user can zoom in and blink stays zoomed
    const bool  wasZoomed = view_->isUserZoomed();
    const double savedZ   = view_->zoom();
    const QPointF savedP  = view_->panOffset();

    if (currentIdx_ < precomputed_.size())
        view_->setPrecomputedImage(precomputed_[currentIdx_], images_[currentIdx_]);
    else
        view_->setFitsImage(images_[currentIdx_]);

    if (wasZoomed)
        view_->restoreView(savedZ, savedP);
    else if (view_->width() > 0 && view_->height() > 0)
        view_->fitToWidget();

    updateInfoBar();
    updateThumbnailHighlight();
    emit imageChanged(currentIdx_);
}

void BlinkWidget::showNext()     { showImage(currentIdx_ + 1); }
void BlinkWidget::showPrevious() { showImage(currentIdx_ - 1); }

void BlinkWidget::setInterval(int ms)
{
    sldSpeed_->setValue(ms);
    if (blinkTimer_->isActive()) blinkTimer_->setInterval(ms);
}

// ── Private helpers ───────────────────────────────────────────────────────────

void BlinkWidget::updateInfoBar()
{
    if (currentIdx_ >= images_.size()) return;
    const auto& img = images_[currentIdx_];
    lblCounter_->setText(QString("[%1/%2]").arg(currentIdx_ + 1).arg(images_.size()));
    lblFilename_->setText(img.fileName);
    if (img.jd > 0)
        lblJD_->setText(QString("JD %1").arg(img.jd, 0, 'f', 5));
    else if (img.dateObs.isValid())
        lblJD_->setText(img.dateObs.toString("yyyy-MM-dd HH:mm:ss"));
    else
        lblJD_->clear();
}

void BlinkWidget::updateThumbnailHighlight()
{
    for (int i = 0; i < thumbLabels_.size(); ++i) {
        thumbLabels_[i]->setStyleSheet(i == currentIdx_
            ? "background:#0d1117; border:2px solid #58a6ff; border-radius:3px;"
            : "background:#0d1117; border:1px solid #30363d; border-radius:3px; color:#555;");
    }
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void BlinkWidget::onBlinkTick() { showNext(); }

void BlinkWidget::onSpeedChanged(int value)
{
    if (blinkTimer_->isActive()) blinkTimer_->setInterval(value);
    lblSpeed_->setText(tr("Speed: %1ms").arg(value));
}

void BlinkWidget::onImageClicked(QPointF pos, double ra, double dec, float value)
{
    stopBlink();
    emit objectClicked(pos, ra, dec, value, currentIdx_);
}

void BlinkWidget::onCursorMoved(double ra, double dec, float /*value*/)
{
    if (ra == 0.0 && dec == 0.0)
        lblRADec_->clear();
    else
        lblRADec_->setText(QString("RA %1  Dec %2").arg(ra, 0, 'f', 5).arg(dec, 0, 'f', 5));
}

// ── Sharpening ────────────────────────────────────────────────────────────────

void BlinkWidget::reapplySharpen()
{
    precomputed_.clear();
    precomputed_.reserve(images_.size());
    for (const auto& img : images_)
        precomputed_.append(applySharpening(FitsImageView::toDisplayImage(img), sharpenMode_));
    if (currentIdx_ < precomputed_.size())
        view_->setPrecomputedImage(precomputed_[currentIdx_], images_[currentIdx_]);
}

// Applies sharpening to a display QImage.
// Works on both Grayscale8 and RGB32 formats via per-channel float arrays.
QImage BlinkWidget::applySharpening(const QImage& src, BlinkSharpenMode mode)
{
    if (mode == BlinkSharpenMode::None || src.isNull()) return src;

    const int w = src.width();
    const int h = src.height();
    if (w < 3 || h < 3) return src;

    const bool isGray = (src.format() == QImage::Format_Grayscale8);
    const int  nch    = isGray ? 1 : 3;

    // Extract pixel values into float channels [nch × (h*w)]
    std::vector<std::vector<float>> chan(static_cast<size_t>(nch),
                                        std::vector<float>(static_cast<size_t>(w * h)));
    if (isGray) {
        for (int y = 0; y < h; ++y) {
            const uchar* row = src.constScanLine(y);
            for (int x = 0; x < w; ++x)
                chan[0][static_cast<size_t>(y * w + x)] = static_cast<float>(row[x]);
        }
    } else {
        const QImage rgb = src.convertToFormat(QImage::Format_RGB32);
        for (int y = 0; y < h; ++y) {
            const QRgb* row = reinterpret_cast<const QRgb*>(rgb.constScanLine(y));
            for (int x = 0; x < w; ++x) {
                const size_t i = static_cast<size_t>(y * w + x);
                chan[0][i] = static_cast<float>(qRed(row[x]));
                chan[1][i] = static_cast<float>(qGreen(row[x]));
                chan[2][i] = static_cast<float>(qBlue(row[x]));
            }
        }
    }

    std::vector<std::vector<float>> out(static_cast<size_t>(nch),
                                        std::vector<float>(static_cast<size_t>(w * h)));

    if (mode == BlinkSharpenMode::UnsharpMask) {
        // 3×3 box blur, then: v + 1.5*(v – blurred)
        constexpr float kStrength = 1.5f;
        for (int c = 0; c < nch; ++c) {
            std::vector<float> blurred(static_cast<size_t>(w * h));
            for (int y = 1; y < h - 1; ++y)
                for (int x = 1; x < w - 1; ++x) {
                    float sum = 0.0f;
                    for (int dy = -1; dy <= 1; ++dy)
                        for (int dx = -1; dx <= 1; ++dx)
                            sum += chan[static_cast<size_t>(c)]
                                       [static_cast<size_t>((y + dy) * w + (x + dx))];
                    blurred[static_cast<size_t>(y * w + x)] = sum / 9.0f;
                }
            // Border pixels: copy unchanged
            for (int y = 0; y < h; ++y) {
                blurred[static_cast<size_t>(y * w)]         = chan[static_cast<size_t>(c)][static_cast<size_t>(y * w)];
                blurred[static_cast<size_t>(y * w + w - 1)] = chan[static_cast<size_t>(c)][static_cast<size_t>(y * w + w - 1)];
            }
            for (int x = 0; x < w; ++x) {
                blurred[static_cast<size_t>(x)]               = chan[static_cast<size_t>(c)][static_cast<size_t>(x)];
                blurred[static_cast<size_t>((h - 1) * w + x)] = chan[static_cast<size_t>(c)][static_cast<size_t>((h - 1) * w + x)];
            }
            const size_t n = static_cast<size_t>(w * h);
            for (size_t i = 0; i < n; ++i)
                out[static_cast<size_t>(c)][i] = std::clamp(
                    chan[static_cast<size_t>(c)][i] + kStrength * (chan[static_cast<size_t>(c)][i] - blurred[i]),
                    0.0f, 255.0f);
        }
    } else { // Laplacian: v + 0.5 * (4v – N – S – E – W)
        constexpr float kScale = 0.5f;
        for (int c = 0; c < nch; ++c) {
            // Interior pixels
            for (int y = 1; y < h - 1; ++y)
                for (int x = 1; x < w - 1; ++x) {
                    const float v  = chan[static_cast<size_t>(c)][static_cast<size_t>(y * w + x)];
                    const float lap = 4.0f * v
                                    - chan[static_cast<size_t>(c)][static_cast<size_t>((y - 1) * w + x)]
                                    - chan[static_cast<size_t>(c)][static_cast<size_t>((y + 1) * w + x)]
                                    - chan[static_cast<size_t>(c)][static_cast<size_t>(y * w + x - 1)]
                                    - chan[static_cast<size_t>(c)][static_cast<size_t>(y * w + x + 1)];
                    out[static_cast<size_t>(c)][static_cast<size_t>(y * w + x)] =
                        std::clamp(v + kScale * lap, 0.0f, 255.0f);
                }
            // Border pixels: copy unchanged
            for (int y = 0; y < h; ++y) {
                out[static_cast<size_t>(c)][static_cast<size_t>(y * w)]         = chan[static_cast<size_t>(c)][static_cast<size_t>(y * w)];
                out[static_cast<size_t>(c)][static_cast<size_t>(y * w + w - 1)] = chan[static_cast<size_t>(c)][static_cast<size_t>(y * w + w - 1)];
            }
            for (int x = 0; x < w; ++x) {
                out[static_cast<size_t>(c)][static_cast<size_t>(x)]               = chan[static_cast<size_t>(c)][static_cast<size_t>(x)];
                out[static_cast<size_t>(c)][static_cast<size_t>((h - 1) * w + x)] = chan[static_cast<size_t>(c)][static_cast<size_t>((h - 1) * w + x)];
            }
        }
    }

    // Write back into a copy of src
    QImage result = src.copy();
    if (isGray) {
        for (int y = 0; y < h; ++y) {
            uchar* row = result.scanLine(y);
            for (int x = 0; x < w; ++x)
                row[x] = static_cast<uchar>(out[0][static_cast<size_t>(y * w + x)]);
        }
    } else {
        result = result.convertToFormat(QImage::Format_RGB32);
        for (int y = 0; y < h; ++y) {
            QRgb* row = reinterpret_cast<QRgb*>(result.scanLine(y));
            for (int x = 0; x < w; ++x) {
                const size_t i = static_cast<size_t>(y * w + x);
                row[x] = qRgb(static_cast<int>(out[0][i]),
                               static_cast<int>(out[1][i]),
                               static_cast<int>(out[2][i]));
            }
        }
    }
    return result;
}

// ── Events ────────────────────────────────────────────────────────────────────

void BlinkWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        if (isBlinking()) stopBlink(); else startBlink();
        break;
    case Qt::Key_Right:
        if (view_->isUserZoomed()) view_->processKey(e);
        else showNext();
        break;
    case Qt::Key_Left:
        if (view_->isUserZoomed()) view_->processKey(e);
        else showPrevious();
        break;
    case Qt::Key_Escape: emit stopRequested(); break;
    // Zoom and pan: forward to the image view
    case Qt::Key_Plus:
    case Qt::Key_Equal:
    case Qt::Key_Minus:
    case Qt::Key_0:
    case Qt::Key_Up:
    case Qt::Key_Down:
        view_->processKey(e);
        break;
    default: QWidget::keyPressEvent(e);
    }
}

void BlinkWidget::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    setFocus();
    // fitToWidget() may have been called when size was 0 (before first show).
    // Defer to the next event loop tick so layout has been applied.
    QTimer::singleShot(0, view_, &FitsImageView::fitToWidget);
}

bool BlinkWidget::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() == QEvent::MouseButtonPress) {
        bool ok = false;
        const int idx = obj->property("thumbIndex").toInt(&ok);
        if (ok && idx >= 0 && idx < images_.size()) {
            stopBlink();
            showImage(idx);
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}
