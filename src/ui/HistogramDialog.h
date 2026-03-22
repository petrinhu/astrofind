#pragma once

#include "core/FitsImage.h"
#include <QDialog>

/// Histogram dialog with Gaussian fit overlay.
/// Shows log-normalised pixel histogram, sigma-clipped Gaussian fit curve,
/// display-range markers, and statistics (μ, σ, median, N).
class HistogramDialog : public QDialog {
    Q_OBJECT
public:
    explicit HistogramDialog(const core::FitsImage& img, QWidget* parent = nullptr);
};
