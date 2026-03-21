#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>

/// Dock panel showing calibration frame status (dark / flat).
/// Emits signals when the user clicks "Load" or "Clear" buttons.
class CalibrationPanel : public QWidget {
    Q_OBJECT
public:
    explicit CalibrationPanel(QWidget* parent = nullptr);

    /// Update the dark frame status label.
    void setDarkStatus(const QString& fileName, int w, int h);
    void clearDark();

    /// Update the flat field status label.
    void setFlatStatus(const QString& fileName, int w, int h);
    void clearFlat();

signals:
    void loadDarkRequested();
    void loadFlatRequested();
    void clearDarkRequested();
    void clearFlatRequested();
    void applyAllRequested();
    void buildMasterDarkRequested();
    void buildMasterFlatRequested();

private:
    QLabel*      darkLabel_    = nullptr;
    QLabel*      flatLabel_    = nullptr;
    QPushButton* clearDarkBtn_ = nullptr;
    QPushButton* clearFlatBtn_ = nullptr;
};
