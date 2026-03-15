#include "BlinkWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QShowEvent>
#include <QMouseEvent>

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
}

// ── Image management ──────────────────────────────────────────────────────────

void BlinkWidget::setImages(const QVector<core::FitsImage>& images)
{
    stopBlink();
    images_      = images;
    precomputed_.clear();
    precomputed_.reserve(images_.size());

    for (const auto& img : images_) {
        precomputed_.append(FitsImageView::toDisplayImage(img));
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
    currentIdx_ = ((index % images_.size()) + images_.size()) % images_.size();

    if (currentIdx_ < precomputed_.size())
        view_->setPrecomputedImage(precomputed_[currentIdx_], images_[currentIdx_]);
    else
        view_->setFitsImage(images_[currentIdx_]);

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

// ── Events ────────────────────────────────────────────────────────────────────

void BlinkWidget::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
    case Qt::Key_Space:
        if (isBlinking()) stopBlink(); else startBlink();
        break;
    case Qt::Key_Right: showNext();     break;
    case Qt::Key_Left:  showPrevious(); break;
    case Qt::Key_Escape: emit stopRequested(); break;
    default: QWidget::keyPressEvent(e);
    }
}

void BlinkWidget::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);
    setFocus();
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
