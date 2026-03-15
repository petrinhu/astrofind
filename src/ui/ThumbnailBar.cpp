#include "ThumbnailBar.h"
#include "FitsImageView.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMouseEvent>

ThumbnailBar::ThumbnailBar(QWidget* parent)
    : QWidget(parent)
{
    setFixedHeight(86);
    setStyleSheet("ThumbnailBar { background:#161b22; border-top:1px solid #21262d; }");

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(8, 4, 8, 4);
    lay->setSpacing(8);

    for (int i = 0; i < kMaxImages; ++i) {
        auto& s = slots_[i];

        s.frame = new QWidget(this);
        s.frame->setFixedSize(68, 76);
        s.frame->setStyleSheet("QWidget { background:#0d1117; border:1px solid #21262d; border-radius:4px; }");
        s.frame->setCursor(Qt::PointingHandCursor);
        s.frame->setProperty("slotIndex", i);
        s.frame->installEventFilter(this);

        auto* vl = new QVBoxLayout(s.frame);
        vl->setContentsMargins(3, 3, 3, 3);
        vl->setSpacing(2);

        s.thumb = new QLabel(s.frame);
        s.thumb->setFixedSize(62, 52);
        s.thumb->setAlignment(Qt::AlignCenter);
        s.thumb->setStyleSheet("background:#0d1117; color:#30363d;");
        s.thumb->setText(QString::number(i + 1));

        s.label = new QLabel(s.frame);
        s.label->setAlignment(Qt::AlignCenter);
        s.label->setStyleSheet("color:#555; font-size:8px;");
        s.label->setText(QString("--- %1 ---").arg(i + 1));

        vl->addWidget(s.thumb);
        vl->addWidget(s.label);

        lay->addWidget(s.frame);
    }
    lay->addStretch();
}

void ThumbnailBar::setImages(const QVector<core::FitsImage>& images)
{
    for (int i = 0; i < kMaxImages; ++i) {
        if (i < images.size())
            updateSlot(i, images[i]);
        else
            clearSlot(i);
    }
    setActiveIndex(images.isEmpty() ? -1 : 0);
}

void ThumbnailBar::setActiveIndex(int idx)
{
    activeIdx_ = idx;
    for (int i = 0; i < kMaxImages; ++i) {
        const bool active = (i == idx);
        slots_[i].frame->setStyleSheet(active
            ? "QWidget { background:#0d1117; border:2px solid #58a6ff; border-radius:4px; }"
            : "QWidget { background:#0d1117; border:1px solid #21262d; border-radius:4px; }");
    }
}

void ThumbnailBar::clear()
{
    for (int i = 0; i < kMaxImages; ++i) clearSlot(i);
    activeIdx_ = -1;
}

void ThumbnailBar::updateSlot(int i, const core::FitsImage& img)
{
    auto& s = slots_[i];
    const QImage th = FitsImageView::toThumbnail(img, 62);
    s.thumb->setPixmap(QPixmap::fromImage(th));
    s.label->setStyleSheet("color:#8b949e; font-size:8px;");

    // Truncate filename to fit
    QString name = img.fileName;
    if (name.length() > 10) name = name.left(8) + QString::fromUtf8("\xe2\x80\xa6"); // ellipsis
    s.label->setText(name);
    s.frame->setToolTip(img.fileName + (img.jd > 0
        ? QString("\nJD %1").arg(img.jd, 0, 'f', 4) : QString()));
}

void ThumbnailBar::clearSlot(int i)
{
    auto& s = slots_[i];
    s.thumb->clear();
    s.thumb->setText(QString::number(i + 1));
    s.thumb->setStyleSheet("background:#0d1117; color:#30363d;");
    s.label->setText(QString("--- %1 ---").arg(i + 1));
    s.label->setStyleSheet("color:#30363d; font-size:8px;");
    s.frame->setToolTip(QString());
}

bool ThumbnailBar::eventFilter(QObject* obj, QEvent* ev)
{
    if (ev->type() == QEvent::MouseButtonPress) {
        bool ok = false;
        const int idx = obj->property("slotIndex").toInt(&ok);
        if (ok && idx >= 0 && idx < kMaxImages) {
            emit imageActivated(idx);
            return true;
        }
    }
    return QWidget::eventFilter(obj, ev);
}
