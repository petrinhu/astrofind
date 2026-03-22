#pragma once

#include <QDialog>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <vector>

#include "FitsImageView.h"
#include "core/FitsImage.h"

/// Non-modal dialog to adjust display stretch (background + range) with live histogram.
/// Changes are applied live to the FitsImageView; Cancel reverts to original values.
class BackgroundRangeDialog : public QDialog {
    Q_OBJECT
public:
    explicit BackgroundRangeDialog(FitsImageView* view,
                                   core::FitsImage* image,
                                   QWidget* parent = nullptr);

signals:
    /// Emitted on OK. applyAll = true means caller should replicate stretch to all images.
    void stretchChanged(float min, float max,
                        core::StretchMode mode, core::ColorLut lut,
                        bool applyAll);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
    void paintEvent(QPaintEvent* e) override;

private slots:
    void onMinChanged(int val);
    void onMaxChanged(int val);
    void onAutoClicked();
    void onApplyClicked();
    void onRejectClicked();

private:
    void buildHistogram();
    void updatePreview();
    float sliderToValue(int sliderVal) const;

    FitsImageView*   view_   = nullptr;
    core::FitsImage* image_  = nullptr;

    float             origMin_  = 0.0f;
    float             origMax_  = 1.0f;
    core::StretchMode origMode_ = core::StretchMode::Linear;
    core::ColorLut    origLut_  = core::ColorLut::Grayscale;
    float             dataMin_  = 0.0f;
    float             dataMax_  = 1.0f;

    QSlider*   sldMin_      = nullptr;
    QSlider*   sldMax_      = nullptr;
    QLabel*    lblMin_      = nullptr;
    QLabel*    lblMax_      = nullptr;
    QComboBox* modeCombo_   = nullptr;
    QComboBox* lutCombo_    = nullptr;
    QCheckBox* chkApplyAll_ = nullptr;
    QWidget*   histWidget_  = nullptr;

    std::vector<float> histBins_;
};
