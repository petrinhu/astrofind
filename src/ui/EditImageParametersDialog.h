#pragma once

#include "core/FitsImage.h"

#include <QDialog>

class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;

/// Dialog for editing mutable metadata of a loaded FITS image.
/// Fields that affect photometry/astrometry: object name, filter, observer,
/// telescope, JD, plate scale, gain, saturation.
class EditImageParametersDialog : public QDialog {
    Q_OBJECT
public:
    explicit EditImageParametersDialog(const core::FitsImage& img,
                                       QWidget* parent = nullptr);

    /// Returns the image with updated metadata fields.
    core::FitsImage apply(core::FitsImage img) const;

private:
    QLineEdit*      edObject_;
    QLineEdit*      edFilter_;
    QLineEdit*      edObserver_;
    QLineEdit*      edTelescope_;
    QDoubleSpinBox* edRA_;
    QDoubleSpinBox* edDec_;
    QDoubleSpinBox* edJD_;
    QDoubleSpinBox* edExpTime_;
    QDoubleSpinBox* edPixScaleX_;
    QDoubleSpinBox* edPixScaleY_;
    QDoubleSpinBox* edGain_;
    QDoubleSpinBox* edSaturation_;
};
