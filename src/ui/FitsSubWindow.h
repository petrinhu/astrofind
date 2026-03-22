#pragma once

#include "FitsImageView.h"
#include "core/FitsImage.h"

#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

/// MDI child widget that wraps FitsImageView and shows image info.
class FitsSubWindow : public QWidget {
    Q_OBJECT
public:
    explicit FitsSubWindow(const core::FitsImage& img, QWidget* parent = nullptr);

    const core::FitsImage& fitsImage() const noexcept { return img_; }
    FitsImageView*         imageView()       noexcept { return view_; }

    void zoomIn()         { view_->zoomIn(); }
    void zoomOut()        { view_->zoomOut(); }
    void fitToWindow()    { view_->fitToWidget(); }
    void toggleInvert()   { view_->toggleInvert(); }
    void flipHorizontal() { view_->toggleFlipH(); }
    void flipVertical()   { view_->toggleFlipV(); }

    void showFitsHeader();

    /// Refresh overlay data (detectedStars / catalogStars / kooObjects) without reloading pixels.
    void updateImage(const core::FitsImage& img);

signals:
    void cursorMoved(double ra, double dec, float value);
    void regionSelected(QRect imageRect);  ///< Forwarded from FitsImageView

    // ── Context menu actions — connected in MainWindow ────────────────────
    void exportImageRequested();
    void applyDarkRequested();
    void applyFlatRequested();
    void showHistogramRequested();
    void showPowerSpectrumRequested();
    void showCubeAnimationRequested();
    void showImageCatalogRequested();

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
    void keyPressEvent(QKeyEvent* e) override;

private slots:
    void onHduSelected(int comboIndex);

private:
    void buildHduBar();
    void refreshInfoBar();

    core::FitsImage         img_;
    FitsImageView*          view_     = nullptr;
    QLabel*                 infoBar_  = nullptr;

    // ── HDU navigation bar (hidden when file has only one image HDU) ──────────
    QWidget*                hduBar_   = nullptr;
    QComboBox*              hduCombo_ = nullptr;
    QPushButton*            hduPrev_  = nullptr;
    QPushButton*            hduNext_  = nullptr;
    QVector<core::HduInfo>  hduList_;   ///< All image HDUs in the file (empty = single-HDU)
};
