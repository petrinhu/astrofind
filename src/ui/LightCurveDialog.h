#pragma once

#include "core/Measurement.h"

#include <QDialog>
#include <QVector>

class QComboBox;
class QLabel;
class LightCurvePlot;

/// Non-modal dialog showing a magnitude-vs-time light curve for all current
/// observations.  Clicking a data point activates the corresponding image.
class LightCurveDialog : public QDialog {
    Q_OBJECT
public:
    explicit LightCurveDialog(QWidget* parent = nullptr);

    /// Replace the full observation list and refresh the plot.
    void setObservations(const QVector<core::Observation>& obs);

    /// Highlight the point that belongs to @p imageIndex.
    void setActiveImage(int imageIndex);

signals:
    /// Emitted when the user clicks a data point.
    void imageActivated(int imageIndex);

private slots:
    void onFilterChanged();
    void onExportPng();

private:
    QComboBox*     filterCombo_ = nullptr;
    QLabel*        statusLabel_ = nullptr;
    LightCurvePlot* plot_       = nullptr;

    QVector<core::Observation> allObs_;
};
