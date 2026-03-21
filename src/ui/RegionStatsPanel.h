#pragma once

#include <QWidget>
#include <QLabel>
#include <vector>

/// Compact floating overlay panel that shows pixel statistics for a selected image region.
/// Parented directly to the FitsImageView — lives inside the image area, no separate window.
class RegionStatsPanel : public QWidget {
    Q_OBJECT
public:
    explicit RegionStatsPanel(QWidget* parent = nullptr);

    /// Compute and display statistics from raw float data for the given image rect.
    void showStats(const std::vector<float>& data, int width, int height, QRect rect);

    /// Hide the panel (does not delete it).
    void clear();

signals:
    void closeRequested();

protected:
    void paintEvent(QPaintEvent*) override;

private:
    struct Stats {
        int    count  = 0;
        float  min    = 0.0f, max    = 0.0f;
        double mean   = 0.0, median = 0.0, stddev = 0.0;
    };

    static Stats compute(const std::vector<float>& data, int w, int h, QRect r);

    QLabel* lblRegion_  = nullptr;
    QLabel* lblCount_   = nullptr;
    QLabel* lblMin_     = nullptr;
    QLabel* lblMax_     = nullptr;
    QLabel* lblMean_    = nullptr;
    QLabel* lblMedian_  = nullptr;
    QLabel* lblStddev_  = nullptr;
};
