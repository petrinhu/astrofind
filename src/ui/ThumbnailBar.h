#pragma once

#include "core/FitsImage.h"
#include <QScrollArea>
#include <QWidget>
#include <QLabel>
#include <QVector>

class QHBoxLayout;

/// Dockable bottom bar showing thumbnails of all loaded images with click-to-navigate.
/// Slots are created dynamically; horizontal scrolling (mouse-wheel or drag) if > ~10 images.
class ThumbnailBar : public QWidget {
    Q_OBJECT
public:
    explicit ThumbnailBar(QWidget* parent = nullptr);

    void setImages(const QVector<core::FitsImage>& images);
    void setActiveIndex(int idx);
    void clear();
    void applyTheme(bool night);

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

    void buildSlot(int i, const core::FitsImage& img);
    void applyFrameBorder(int i, bool active);

    QScrollArea* scrollArea_ = nullptr;
    QWidget*     content_    = nullptr;
    QHBoxLayout* layout_     = nullptr;

    QVector<Slot> slots_;
    int  activeIdx_ = -1;
    bool night_     = true;
};
