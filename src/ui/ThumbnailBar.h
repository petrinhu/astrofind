#pragma once

#include "core/FitsImage.h"
#include <QWidget>
#include <QLabel>
#include <QVector>
#include <array>

/// Dockable bottom bar showing thumbnails of all loaded images with click-to-navigate.
class ThumbnailBar : public QWidget {
    Q_OBJECT
public:
    explicit ThumbnailBar(QWidget* parent = nullptr);

    void setImages(const QVector<core::FitsImage>& images);
    void setActiveIndex(int idx);
    void clear();

signals:
    void imageActivated(int index);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    struct Slot {
        QLabel*  thumb = nullptr;
        QLabel*  label = nullptr;
        QWidget* frame = nullptr;
    };
    static constexpr int kMaxImages = 4;
    std::array<Slot, kMaxImages> slots_;

    void updateSlot(int i, const core::FitsImage& img);
    void clearSlot(int i);

    int activeIdx_ = -1;
};
