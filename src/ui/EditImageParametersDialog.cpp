#include "EditImageParametersDialog.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QLabel>

EditImageParametersDialog::EditImageParametersDialog(const core::FitsImage& img,
                                                     QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Edit Image Parameters — %1").arg(img.fileName));
    setMinimumWidth(420);

    // ── Observation info ──────────────────────────────────────────────────────
    auto* grpObs = new QGroupBox(tr("Observation"), this);
    auto* formObs = new QFormLayout(grpObs);

    edObject_   = new QLineEdit(img.objectName, this);
    edFilter_   = new QLineEdit(img.filter,     this);
    edObserver_ = new QLineEdit(img.observer,   this);
    edTelescope_= new QLineEdit(img.telescope,  this);

    formObs->addRow(tr("Object name:"),  edObject_);
    formObs->addRow(tr("Filter / Band:"), edFilter_);
    formObs->addRow(tr("Observer:"),     edObserver_);
    formObs->addRow(tr("Telescope:"),    edTelescope_);

    // ── Coordinates & time ────────────────────────────────────────────────────
    auto* grpCoord = new QGroupBox(tr("Coordinates && Time"), this);
    auto* formCoord = new QFormLayout(grpCoord);

    edRA_  = new QDoubleSpinBox(this);
    edRA_->setRange(0.0, 360.0);
    edRA_->setDecimals(6);
    edRA_->setSuffix(tr("  °"));
    edRA_->setValue(img.ra);

    edDec_ = new QDoubleSpinBox(this);
    edDec_->setRange(-90.0, 90.0);
    edDec_->setDecimals(6);
    edDec_->setSuffix(tr("  °"));
    edDec_->setValue(img.dec);

    edJD_  = new QDoubleSpinBox(this);
    edJD_->setRange(0.0, 9999999.0);
    edJD_->setDecimals(6);
    edJD_->setValue(img.jd);
    edJD_->setToolTip(tr("Julian Date of mid-exposure"));

    edExpTime_ = new QDoubleSpinBox(this);
    edExpTime_->setRange(0.0, 86400.0);
    edExpTime_->setDecimals(3);
    edExpTime_->setSuffix(tr("  s"));
    edExpTime_->setValue(img.expTime);

    formCoord->addRow(tr("RA:"),            edRA_);
    formCoord->addRow(tr("Dec:"),           edDec_);
    formCoord->addRow(tr("Julian Date:"),   edJD_);
    formCoord->addRow(tr("Exposure time:"), edExpTime_);

    // ── Instrument ────────────────────────────────────────────────────────────
    auto* grpInstr = new QGroupBox(tr("Instrument"), this);
    auto* formInstr = new QFormLayout(grpInstr);

    edPixScaleX_ = new QDoubleSpinBox(this);
    edPixScaleX_->setRange(0.0, 999.0);
    edPixScaleX_->setDecimals(4);
    edPixScaleX_->setSuffix(tr("  \"/px"));
    edPixScaleX_->setValue(img.pixScaleX);

    edPixScaleY_ = new QDoubleSpinBox(this);
    edPixScaleY_->setRange(0.0, 999.0);
    edPixScaleY_->setDecimals(4);
    edPixScaleY_->setSuffix(tr("  \"/px"));
    edPixScaleY_->setValue(img.pixScaleY);

    edGain_ = new QDoubleSpinBox(this);
    edGain_->setRange(0.01, 100.0);
    edGain_->setDecimals(3);
    edGain_->setSuffix(tr("  e⁻/ADU"));
    edGain_->setValue(img.gain);

    edSaturation_ = new QDoubleSpinBox(this);
    edSaturation_->setRange(1.0, 2097152.0);
    edSaturation_->setDecimals(0);
    edSaturation_->setSuffix(tr("  ADU"));
    edSaturation_->setValue(img.saturation);

    formInstr->addRow(tr("Plate scale X:"), edPixScaleX_);
    formInstr->addRow(tr("Plate scale Y:"), edPixScaleY_);
    formInstr->addRow(tr("Gain:"),          edGain_);
    formInstr->addRow(tr("Saturation:"),    edSaturation_);

    // ── Buttons ───────────────────────────────────────────────────────────────
    auto* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* lay = new QVBoxLayout(this);
    lay->addWidget(grpObs);
    lay->addWidget(grpCoord);
    lay->addWidget(grpInstr);
    lay->addWidget(buttons);
}

core::FitsImage EditImageParametersDialog::apply(core::FitsImage img) const
{
    img.objectName  = edObject_->text().trimmed();
    img.filter      = edFilter_->text().trimmed();
    img.observer    = edObserver_->text().trimmed();
    img.telescope   = edTelescope_->text().trimmed();
    img.ra          = edRA_->value();
    img.dec         = edDec_->value();
    img.jd          = edJD_->value();
    img.expTime     = edExpTime_->value();
    img.pixScaleX   = edPixScaleX_->value();
    img.pixScaleY   = edPixScaleY_->value();
    img.gain        = edGain_->value();
    img.saturation  = edSaturation_->value();
    return img;
}
