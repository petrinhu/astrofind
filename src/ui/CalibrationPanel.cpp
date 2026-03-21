#include "CalibrationPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

static QLabel* statusLabel(QWidget* parent)
{
    auto* lbl = new QLabel(QObject::tr("Not loaded"), parent);
    lbl->setStyleSheet(QStringLiteral("color:#888; font-size:10px; font-style:italic;"));
    lbl->setWordWrap(true);
    return lbl;
}

CalibrationPanel::CalibrationPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* vlay = new QVBoxLayout(this);
    vlay->setContentsMargins(6, 6, 6, 6);
    vlay->setSpacing(8);

    // ── Dark frame ────────────────────────────────────────────────────────
    auto* darkBox  = new QGroupBox(tr("Dark Frame"), this);
    auto* darkLay  = new QVBoxLayout(darkBox);
    darkLay->setSpacing(4);

    darkLabel_ = statusLabel(darkBox);
    darkLay->addWidget(darkLabel_);

    auto* darkBtnRow   = new QHBoxLayout;
    auto* loadDarkBtn  = new QPushButton(tr("Load…"),          darkBox);
    auto* buildDarkBtn = new QPushButton(tr("Build Master…"),  darkBox);
    clearDarkBtn_      = new QPushButton(tr("Clear"),          darkBox);
    clearDarkBtn_->setEnabled(false);
    buildDarkBtn->setToolTip(tr("Stack multiple dark FITS frames into a master dark"));
    darkBtnRow->addWidget(loadDarkBtn);
    darkBtnRow->addWidget(buildDarkBtn);
    darkBtnRow->addWidget(clearDarkBtn_);
    darkLay->addLayout(darkBtnRow);
    vlay->addWidget(darkBox);

    // ── Flat field ────────────────────────────────────────────────────────
    auto* flatBox  = new QGroupBox(tr("Flat Field"), this);
    auto* flatLay  = new QVBoxLayout(flatBox);
    flatLay->setSpacing(4);

    flatLabel_ = statusLabel(flatBox);
    flatLay->addWidget(flatLabel_);

    auto* flatBtnRow   = new QHBoxLayout;
    auto* loadFlatBtn  = new QPushButton(tr("Load…"),          flatBox);
    auto* buildFlatBtn = new QPushButton(tr("Build Master…"),  flatBox);
    clearFlatBtn_      = new QPushButton(tr("Clear"),          flatBox);
    clearFlatBtn_->setEnabled(false);
    buildFlatBtn->setToolTip(tr("Stack multiple flat FITS frames into a master flat"));
    flatBtnRow->addWidget(loadFlatBtn);
    flatBtnRow->addWidget(buildFlatBtn);
    flatBtnRow->addWidget(clearFlatBtn_);
    flatLay->addLayout(flatBtnRow);
    vlay->addWidget(flatBox);

    // ── Apply all ─────────────────────────────────────────────────────────
    auto* applyBtn = new QPushButton(tr("Apply to All Images"), this);
    applyBtn->setToolTip(tr("Re-apply dark and flat to every loaded image and recompute stretch"));
    vlay->addWidget(applyBtn);
    vlay->addStretch();

    // Connections
    connect(loadDarkBtn,  &QPushButton::clicked, this, &CalibrationPanel::loadDarkRequested);
    connect(loadFlatBtn,  &QPushButton::clicked, this, &CalibrationPanel::loadFlatRequested);
    connect(buildDarkBtn, &QPushButton::clicked, this, &CalibrationPanel::buildMasterDarkRequested);
    connect(buildFlatBtn, &QPushButton::clicked, this, &CalibrationPanel::buildMasterFlatRequested);
    connect(clearDarkBtn_,&QPushButton::clicked, this, &CalibrationPanel::clearDarkRequested);
    connect(clearFlatBtn_,&QPushButton::clicked, this, &CalibrationPanel::clearFlatRequested);
    connect(applyBtn,     &QPushButton::clicked, this, &CalibrationPanel::applyAllRequested);
}

void CalibrationPanel::setDarkStatus(const QString& fileName, int w, int h)
{
    darkLabel_->setText(tr("✓ %1\n%2 × %3 px").arg(fileName).arg(w).arg(h));
    darkLabel_->setStyleSheet(QStringLiteral("color:#80cc80; font-size:10px;"));
    clearDarkBtn_->setEnabled(true);
}

void CalibrationPanel::clearDark()
{
    darkLabel_->setText(tr("Not loaded"));
    darkLabel_->setStyleSheet(QStringLiteral("color:#888; font-size:10px; font-style:italic;"));
    clearDarkBtn_->setEnabled(false);
}

void CalibrationPanel::setFlatStatus(const QString& fileName, int w, int h)
{
    flatLabel_->setText(tr("✓ %1\n%2 × %3 px").arg(fileName).arg(w).arg(h));
    flatLabel_->setStyleSheet(QStringLiteral("color:#80cc80; font-size:10px;"));
    clearFlatBtn_->setEnabled(true);
}

void CalibrationPanel::clearFlat()
{
    flatLabel_->setText(tr("Not loaded"));
    flatLabel_->setStyleSheet(QStringLiteral("color:#888; font-size:10px; font-style:italic;"));
    clearFlatBtn_->setEnabled(false);
}
