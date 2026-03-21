#include "SelectMarkingsDialog.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QGroupBox>

SelectMarkingsDialog::SelectMarkingsDialog(const MarkingsState& cur, QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Select Markings"));
    setMinimumWidth(320);
    setSizeGripEnabled(false);

    auto* vlay = new QVBoxLayout(this);

    auto* box = new QGroupBox(tr("Show overlay types"), this);
    auto* blay = new QVBoxLayout(box);

    detectedChk_ = new QCheckBox(tr("Detected stars  (cyan circles)"),   box);
    catalogChk_  = new QCheckBox(tr("Catalog stars   (yellow crosses)"),  box);
    kooChk_      = new QCheckBox(tr("Known objects   (coloured labels)"), box);
    labelsChk_   = new QCheckBox(tr("Object labels"),                      box);

    detectedChk_->setChecked(cur.detectedStars);
    catalogChk_ ->setChecked(cur.catalogStars);
    kooChk_     ->setChecked(cur.kooObjects);
    labelsChk_  ->setChecked(cur.labels);

    blay->addWidget(detectedChk_);
    blay->addWidget(catalogChk_);
    blay->addWidget(kooChk_);
    blay->addWidget(labelsChk_);
    vlay->addWidget(box);

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    vlay->addWidget(bb);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

MarkingsState SelectMarkingsDialog::state() const
{
    return { detectedChk_->isChecked(),
             catalogChk_ ->isChecked(),
             kooChk_     ->isChecked(),
             labelsChk_  ->isChecked() };
}
