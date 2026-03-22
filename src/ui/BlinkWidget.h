#pragma once

#include "core/FitsImage.h"
#include "FitsImageView.h"

#include <QWidget>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include <QVector>
#include <QImage>

/// Image sharpening modes available in the blink view.
enum class BlinkSharpenMode { None, UnsharpMask, Laplacian };

/// Full-screen blink view — cycles through session images to reveal moving objects.
/// Keyboard: Space = pause/resume, Left/Right = step, Escape = stop.
class BlinkWidget : public QWidget {
    Q_OBJECT
public:
    explicit BlinkWidget(QWidget* parent = nullptr);

    void setImages(const QVector<core::FitsImage>& images);
    void updateImageMetadata(const QVector<core::FitsImage>& images);
    void clearImages();
    FitsImageView* imageView() noexcept { return view_; }

    void startBlink();
    void stopBlink();
    bool isBlinking() const noexcept { return blinkTimer_->isActive(); }

    void showImage(int index);
    void showNext();
    void showPrevious();
    int  currentIndex() const noexcept { return currentIdx_; }

    void setInterval(int ms);

signals:
    void stopRequested();
    void imageChanged(int index);
    /// Emitted when user clicks on the image (for measurement)
    void objectClicked(QPointF imagePixel, double ra, double dec, float value, int imageIndex);

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void showEvent(QShowEvent* e) override;
    bool eventFilter(QObject* obj, QEvent* ev) override;

private slots:
    void onBlinkTick();
    void onSpeedChanged(int value);
    void onImageClicked(QPointF pos, double ra, double dec, float value);
    void onCursorMoved(double ra, double dec, float value);

private:
    void buildUi();
    void updateInfoBar();
    void updateThumbnailHighlight();
    void reapplySharpen();

    /// Apply Laplacian or unsharp-mask sharpening to a display QImage.
    static QImage applySharpening(const QImage& src, BlinkSharpenMode mode);

    QVector<core::FitsImage> images_;
    QVector<QImage>          precomputed_;
    int                      currentIdx_ = 0;

    FitsImageView*   view_        = nullptr;
    QLabel*          lblFilename_ = nullptr;
    QLabel*          lblJD_       = nullptr;
    QLabel*          lblCounter_  = nullptr;
    QLabel*          lblRADec_    = nullptr;

    QPushButton*     btnPrev_    = nullptr;
    QPushButton*     btnPlay_    = nullptr;
    QPushButton*     btnNext_    = nullptr;
    QPushButton*     btnStop_    = nullptr;
    QPushButton*     btnSharpen_ = nullptr;
    QSlider*         sldSpeed_   = nullptr;
    QLabel*          lblSpeed_   = nullptr;
    QVector<QLabel*> thumbLabels_;

    QTimer*          blinkTimer_   = nullptr;
    BlinkSharpenMode sharpenMode_  = BlinkSharpenMode::None;
};
