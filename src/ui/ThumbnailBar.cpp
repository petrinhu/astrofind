#include "ThumbnailBar.h"
#include "FitsImageView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <spdlog/spdlog.h>

ThumbnailBar::ThumbnailBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(86);

    // Scroll area fills the bar
    scrollArea_ = new QScrollArea(this);
    scrollArea_->setFrameShape(QFrame::NoFrame);
    scrollArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea_->setWidgetResizable(false);
    scrollArea_->viewport()->installEventFilter(this);

    // Content widget holds the actual slot frames
    content_ = new QWidget;
    layout_  = new QHBoxLayout(content_);
    layout_->setContentsMargins(8, 4, 8, 4);
    layout_->setSpacing(8);
    layout_->addStretch();

    scrollArea_->setWidget(content_);

    auto* outerLay = new QHBoxLayout(this);
    outerLay->setContentsMargins(0, 0, 0, 0);
    outerLay->addWidget(scrollArea_);

    applyTheme(true);
}

void ThumbnailBar::setImages(const QVector<core::FitsImage>& images)
{
    // Remove all existing slot widgets
    for (auto& s : slots_)
        delete s.frame;
    slots_.clear();

    // Remove stretch, add slots, re-add stretch
    // layout_ currently has a stretch at index 0 (after clear)
    while (layout_->count())
        layout_->takeAt(0);

    spdlog::info("ThumbnailBar::setImages: {} images", images.size());
    for (int i = 0; i < images.size(); ++i)
        buildSlot(i, images[i]);

    layout_->addStretch();
    spdlog::info("ThumbnailBar: layout count={} sizeHint={}x{}",
        layout_->count(), layout_->sizeHint().width(), layout_->sizeHint().height());
    // adjustSize() uses sizeHint() but can misbehave inside a QScrollArea;
    // force the correct size directly.
    const int n = slots_.size();
    const int cw = 8 + n * 68 + (n > 1 ? (n - 1) * 8 : 0) + 8;
    const int ch = 4 + 76 + 4;
    content_->resize(cw, ch);
    content_->update();
    spdlog::info("ThumbnailBar: content resized to {}x{}", cw, ch);

    activeIdx_ = -1;
    setActiveIndex(images.isEmpty() ? -1 : 0);
}

void ThumbnailBar::setActiveIndex(int idx)
{
    activeIdx_ = idx;
    for (int i = 0; i < slots_.size(); ++i)
        applyFrameBorder(i, i == idx);

    if (idx >= 0 && idx < slots_.size())
        scrollArea_->ensureWidgetVisible(slots_[idx].frame);
}

void ThumbnailBar::clear()
{
    setImages({});
}

void ThumbnailBar::buildSlot(int i, const core::FitsImage& img)
{
    Slot s;
    s.frame = new QWidget(content_);
    s.frame->setObjectName(QStringLiteral("thumbSlot"));
    s.frame->setFixedSize(68, 76);
    s.frame->setCursor(Qt::PointingHandCursor);
    s.frame->setProperty("slotIndex", i);
    s.frame->installEventFilter(this);

    auto* vl = new QVBoxLayout(s.frame);
    vl->setContentsMargins(3, 3, 3, 3);
    vl->setSpacing(2);

    s.thumb = new QLabel(s.frame);
    s.thumb->setObjectName(QStringLiteral("thumbImage"));
    s.thumb->setFixedSize(62, 52);
    s.thumb->setAlignment(Qt::AlignCenter);
    s.thumb->setStyleSheet(night_
        ? "QLabel#thumbImage { background:#0d1117; border:none; }"
        : "QLabel#thumbImage { background:#e8eef8; border:none; }");
    s.thumb->setProperty("slotIndex", i);
    s.thumb->installEventFilter(this);

    const QImage th = FitsImageView::toThumbnail(img, 62);
    spdlog::info("buildSlot {}: imgValid={} thNull={} thSize={}x{}",
        i, img.isValid(), th.isNull(), th.width(), th.height());
    s.thumb->setPixmap(QPixmap::fromImage(th));

    s.label = new QLabel(s.frame);
    s.label->setAlignment(Qt::AlignCenter);
    s.label->setProperty("slotIndex", i);
    s.label->installEventFilter(this);

    QString name = img.fileName;
    if (name.length() > 10) name = name.left(8) + QString::fromUtf8("\xe2\x80\xa6");
    s.label->setText(name);
    s.label->setStyleSheet(night_ ? "color:#8b949e; font-size:8px;" : "color:#445566; font-size:8px;");

    s.frame->setToolTip(img.fileName + (img.jd > 0
        ? QString("\nJD %1").arg(img.jd, 0, 'f', 4) : QString()));

    vl->addWidget(s.thumb);
    vl->addWidget(s.label);

    // Insert before the trailing stretch
    layout_->insertWidget(i, s.frame);
    slots_.append(s);
}

void ThumbnailBar::applyFrameBorder(int i, bool active)
{
    if (i < 0 || i >= slots_.size()) return;
    if (night_) {
        slots_[i].frame->setStyleSheet(active
            ? "QWidget#thumbSlot { background:#0d1117; border:2px solid #58a6ff; border-radius:4px; }"
            : "QWidget#thumbSlot { background:#0d1117; border:1px solid #21262d; border-radius:4px; }");
    } else {
        slots_[i].frame->setStyleSheet(active
            ? "QWidget#thumbSlot { background:#ffffff; border:2px solid #3060a0; border-radius:4px; }"
            : "QWidget#thumbSlot { background:#ffffff; border:1px solid #b0b8c8; border-radius:4px; }");
    }
}

void ThumbnailBar::applyTheme(bool night)
{
    night_ = night;
    if (night) {
        setStyleSheet("ThumbnailBar { background:#161b22; border-top:1px solid #21262d; }");
        scrollArea_->setStyleSheet("QScrollArea { background:#161b22; }");
        content_->setStyleSheet("background:#161b22;");
        for (auto& s : slots_)
            s.thumb->setStyleSheet("QLabel#thumbImage { background:#0d1117; border:none; }");
    } else {
        setStyleSheet("ThumbnailBar { background:#dde3ed; border-top:1px solid #b0b8c8; }");
        scrollArea_->setStyleSheet("QScrollArea { background:#dde3ed; }");
        content_->setStyleSheet("background:#dde3ed;");
        for (auto& s : slots_)
            s.thumb->setStyleSheet("QLabel#thumbImage { background:#e8eef8; border:none; }");
    }
    setActiveIndex(activeIdx_);
    for (int i = 0; i < slots_.size(); ++i) {
        slots_[i].label->setStyleSheet(night
            ? "color:#8b949e; font-size:8px;"
            : "color:#445566; font-size:8px;");
    }
}

bool ThumbnailBar::eventFilter(QObject* obj, QEvent* ev)
{
    // Forward wheel events on the viewport to horizontal scrollbar
    if (obj == scrollArea_->viewport() && ev->type() == QEvent::Wheel) {
        auto* we = static_cast<QWheelEvent*>(ev);
        QScrollBar* hbar = scrollArea_->horizontalScrollBar();
        const int delta = we->angleDelta().y();
        hbar->setValue(hbar->value() - delta / 4);
        return true;
    }

    // Click on any slot child → emit imageActivated
    if (ev->type() == QEvent::MouseButtonPress) {
        bool ok = false;
        const int idx = obj->property("slotIndex").toInt(&ok);
        if (ok && idx >= 0 && idx < slots_.size()) {
            emit imageActivated(idx);
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}
