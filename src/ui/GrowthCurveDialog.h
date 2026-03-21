#pragma once

#include "core/FitsImage.h"
#include "core/Photometry.h"

#include <QDialog>
#include <QSettings>

class QComboBox;
class QDoubleSpinBox;
class QSpinBox;
class QPushButton;
class QLabel;
class QTableWidget;

/// Dialog that shows the aperture growth curve for a single star.
///
/// Multi-aperture photometry is run at N evenly-spaced radii and the result
/// is displayed as a flux-fraction vs radius plot plus a data table.
/// The "Use this aperture" button writes the selected radius to
/// photometry/apertureManPx and sets photometry/apertureAuto=false.
class GrowthCurveDialog : public QDialog {
    Q_OBJECT
public:
    explicit GrowthCurveDialog(const core::FitsImage& img,
                                QSettings& settings,
                                QWidget* parent = nullptr);

private slots:
    void onCompute();
    void onUseAperture();

private:
    void populateTable(const QVector<core::GrowthPoint>& pts);

    const core::FitsImage& img_;
    QSettings&             settings_;

    QComboBox*        starCombo_    = nullptr;
    QDoubleSpinBox*   rMinSpin_     = nullptr;
    QDoubleSpinBox*   rMaxSpin_     = nullptr;
    QSpinBox*         nStepsSpin_   = nullptr;
    QPushButton*      computeBtn_   = nullptr;
    QPushButton*      useApertBtn_  = nullptr;
    QLabel*           statusLabel_  = nullptr;
    QTableWidget*     table_        = nullptr;

    // Inner plot widget — defined in .cpp
    class Plot;
    Plot* plot_ = nullptr;

    double optimalRadius_ = 0.0;
};
