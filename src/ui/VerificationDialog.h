#pragma once

#include "core/FitsImage.h"
#include "core/Measurement.h"
#include "core/Overlay.h"

#include <QDialog>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>

/// Non-modal dialog shown after each measurement.
/// Displays a 4× zoom of the measured region and a list of nearby known
/// solar-system objects for quick identification.
class VerificationDialog : public QDialog {
    Q_OBJECT
public:
    explicit VerificationDialog(QWidget* parent = nullptr);

    /// Update with new measurement data (can be called repeatedly).
    void setMeasurement(const core::FitsImage& img,
                        const core::Observation& obs,
                        const QVector<core::KooObject>& nearbyKoo);

signals:
    /// Emitted when the user confirms the measurement (Accept button).
    void accepted(const core::Observation& obs);
    /// Emitted when the user rejects the measurement (Reject button).
    void rejected();

private slots:
    void onAccept();
    void onAcceptAndClose();
    void onReject();
    void onKooItemSelected();

private:
    void updateZoom();

    // Zoom canvas
    QLabel*      zoomCanvas_  = nullptr;

    // Info labels
    QLabel*      raLabel_     = nullptr;
    QLabel*      decLabel_    = nullptr;
    QLabel*      magLabel_    = nullptr;
    QLabel*      fwhmLabel_   = nullptr;
    QLabel*      snrLabel_    = nullptr;

    // Designation editor
    QLineEdit*   nameEdit_    = nullptr;

    // Nearby KOO objects
    QListWidget* kooList_     = nullptr;

    // Current state
    core::Observation           obs_;
    const core::FitsImage*      img_      = nullptr;
    QVector<core::KooObject>    nearbyKoo_;

    static constexpr int kZoomFactor = 4;
    static constexpr int kZoomRadius = 32;   ///< Half-size of zoom crop (pixels)
};
