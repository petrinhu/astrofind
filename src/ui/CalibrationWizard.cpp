#include "CalibrationWizard.h"

#include <QWizardPage>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QPixmap>
#include <QPainter>

// ── Helper: small status label ─────────────────────────────────────────────
static QLabel* makeStatusLabel(QWidget* parent)
{
    auto* lbl = new QLabel(QObject::tr("No file selected"), parent);
    lbl->setStyleSheet("color: #888; font-style: italic;");
    return lbl;
}

CalibrationWizard::CalibrationWizard(QWidget* parent)
    : QWizard(parent)
{
    setWindowTitle(tr("Calibration Wizard"));
    setWizardStyle(QWizard::ModernStyle);
    setMinimumSize(520, 380);

    buildIntroPage();
    buildDarkPage();
    buildFlatPage();
    buildFinishPage();
}

void CalibrationWizard::buildIntroPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Calibration Wizard"));
    page->setSubTitle(tr("This wizard helps you load and apply calibration frames "
                         "to improve measurement accuracy."));

    auto* lay = new QVBoxLayout(page);
    lay->setSpacing(10);

    auto* intro = new QLabel(tr(
        "<p><b>What are calibration frames?</b></p>"
        "<p><b>Dark frames</b> capture the thermal noise and hot pixels of your CCD at "
        "the same temperature and exposure time as your science images. Subtracting a "
        "dark frame removes this fixed-pattern noise, revealing fainter objects.</p>"
        "<p><b>Flat fields</b> are uniform-illumination images (e.g. twilight sky or "
        "light box). Dividing by a normalised flat corrects for vignetting and "
        "pixel-to-pixel sensitivity variations, making your photometry more accurate.</p>"
        "<p>Both frames must have the <b>same dimensions</b> as your science images.</p>"),
        page);
    intro->setWordWrap(true);
    lay->addWidget(intro);
    lay->addStretch();

    addPage(page);
}

void CalibrationWizard::buildDarkPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Step 1 — Dark Frame"));
    page->setSubTitle(tr("Load a dark frame taken at the same temperature and "
                         "exposure time as your science images."));

    auto* lay = new QVBoxLayout(page);
    lay->setSpacing(8);

    auto* hint = new QLabel(tr(
        "A dark frame is typically taken with the telescope cap on, "
        "same duration and temperature as your light frames."), page);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #aaa; font-size: 11px;");
    lay->addWidget(hint);

    auto* row = new QHBoxLayout;
    darkStatusLabel_ = makeStatusLabel(page);
    darkBrowseBtn_ = new QPushButton(tr("Browse…"), page);
    darkBrowseBtn_->setFixedWidth(90);
    row->addWidget(darkStatusLabel_, 1);
    row->addWidget(darkBrowseBtn_);
    lay->addLayout(row);

    auto* skipNote = new QLabel(tr("(Optional — click Next to skip dark frame)"), page);
    skipNote->setStyleSheet("color: #666; font-size: 10px;");
    lay->addWidget(skipNote);
    lay->addStretch();

    connect(darkBrowseBtn_, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(this,
            tr("Select Dark Frame"),
            QString(),
            tr("FITS Images (*.fits *.fit *.fts);;All files (*)"));
        if (path.isEmpty()) return;
        darkPath_ = path;
        darkStatusLabel_->setText(QFileInfo(path).fileName());
        darkStatusLabel_->setStyleSheet("color: #4c4; font-style: normal;");
        emit darkSelected(path);
    });

    addPage(page);
}

void CalibrationWizard::buildFlatPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Step 2 — Flat Field"));
    page->setSubTitle(tr("Load a flat field for pixel response correction."));

    auto* lay = new QVBoxLayout(page);
    lay->setSpacing(8);

    auto* hint = new QLabel(tr(
        "A flat field is a uniformly illuminated image, typically taken during "
        "twilight or with a light box. It corrects for vignetting and pixel sensitivity."), page);
    hint->setWordWrap(true);
    hint->setStyleSheet("color: #aaa; font-size: 11px;");
    lay->addWidget(hint);

    auto* row = new QHBoxLayout;
    flatStatusLabel_ = makeStatusLabel(page);
    flatBrowseBtn_ = new QPushButton(tr("Browse…"), page);
    flatBrowseBtn_->setFixedWidth(90);
    row->addWidget(flatStatusLabel_, 1);
    row->addWidget(flatBrowseBtn_);
    lay->addLayout(row);

    auto* skipNote = new QLabel(tr("(Optional — click Next to skip flat field)"), page);
    skipNote->setStyleSheet("color: #666; font-size: 10px;");
    lay->addWidget(skipNote);
    lay->addStretch();

    connect(flatBrowseBtn_, &QPushButton::clicked, this, [this]() {
        const QString path = QFileDialog::getOpenFileName(this,
            tr("Select Flat Field"),
            QString(),
            tr("FITS Images (*.fits *.fit *.fts);;All files (*)"));
        if (path.isEmpty()) return;
        flatPath_ = path;
        flatStatusLabel_->setText(QFileInfo(path).fileName());
        flatStatusLabel_->setStyleSheet("color: #4c4; font-style: normal;");
        emit flatSelected(path);
    });

    addPage(page);
}

void CalibrationWizard::buildFinishPage()
{
    auto* page = new QWizardPage;
    page->setTitle(tr("Step 3 — Apply"));
    page->setSubTitle(tr("Review your selection and choose how to apply calibration."));
    page->setFinalPage(true);

    auto* lay = new QVBoxLayout(page);
    lay->setSpacing(10);

    auto* summaryLabel = new QLabel(tr("Selected calibration frames:"), page);
    summaryLabel->setStyleSheet("font-weight: bold;");
    lay->addWidget(summaryLabel);

    // Live summary — updated when wizard moves to this page
    auto* darkRow = new QLabel(page);
    auto* flatRow = new QLabel(page);
    connect(this, &QWizard::currentIdChanged, this, [this, darkRow, flatRow](int id) {
        if (id != 3) return;  // only when on last page (0-indexed pages: 0,1,2,3)
        darkRow->setText(tr("  Dark: ") + (darkPath_.isEmpty()
            ? tr("<i>not selected</i>")
            : QStringLiteral("<b>") + QFileInfo(darkPath_).fileName() + QStringLiteral("</b>")));
        darkRow->setTextFormat(Qt::RichText);
        flatRow->setText(tr("  Flat: ") + (flatPath_.isEmpty()
            ? tr("<i>not selected</i>")
            : QStringLiteral("<b>") + QFileInfo(flatPath_).fileName() + QStringLiteral("</b>")));
        flatRow->setTextFormat(Qt::RichText);
    });
    lay->addWidget(darkRow);
    lay->addWidget(flatRow);

    lay->addSpacing(8);

    applyNowChk_ = new QCheckBox(tr("Apply calibration to currently loaded images now"), page);
    applyNowChk_->setChecked(true);
    lay->addWidget(applyNowChk_);

    auto* note = new QLabel(tr(
        "If unchecked, calibration frames are stored and will be applied "
        "automatically during the next Data Reduction."), page);
    note->setWordWrap(true);
    note->setStyleSheet("color: #888; font-size: 10px;");
    lay->addWidget(note);
    lay->addStretch();

    addPage(page);
}

bool CalibrationWizard::applyNow() const
{
    return applyNowChk_ && applyNowChk_->isChecked();
}
