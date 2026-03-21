#include "SessionCompareDialog.h"

#include "core/FitsImage.h"
#include "core/ImageSession.h"

#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFuture>
#include <QFutureWatcher>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QtConcurrent/QtConcurrent>
#include <QVBoxLayout>

SessionCompareDialog::SessionCompareDialog(const core::ImageSession* session,
                                           QWidget* parent)
    : QDialog(parent)
    , session_(session)
{
    setWindowTitle(tr("Compare Sessions"));
    setMinimumWidth(480);

    auto* mainLayout = new QVBoxLayout(this);
    auto* form       = new QFormLayout;
    mainLayout->addLayout(form);

    // Reference FITS path
    auto* pathRow  = new QHBoxLayout;
    refPathEdit_   = new QLineEdit;
    refPathEdit_->setPlaceholderText(tr("Reference FITS file or directory…"));
    auto* browseBtn = new QPushButton(tr("Browse…"));
    pathRow->addWidget(refPathEdit_);
    pathRow->addWidget(browseBtn);
    form->addRow(tr("Reference image:"), pathRow);
    connect(browseBtn, &QPushButton::clicked, this, &SessionCompareDialog::onBrowse);

    // Active (new) image info
    int newCount = session_ ? session_->imageCount() : 0;
    form->addRow(tr("Active session images:"),
                 new QLabel(tr("%1 image(s)").arg(newCount)));

    // Detection threshold
    thresholdSpin_ = new QDoubleSpinBox;
    thresholdSpin_->setRange(1.0, 20.0);
    thresholdSpin_->setSingleStep(0.5);
    thresholdSpin_->setValue(4.0);
    thresholdSpin_->setSuffix(tr(" σ"));
    form->addRow(tr("Detection threshold:"), thresholdSpin_);

    // Progress / status
    progressBar_ = new QProgressBar;
    progressBar_->setRange(0, 0);   // indeterminate
    progressBar_->setVisible(false);
    mainLayout->addWidget(progressBar_);

    statusLabel_ = new QLabel;
    statusLabel_->setWordWrap(true);
    mainLayout->addWidget(statusLabel_);

    // Buttons
    compareBtn_      = new QPushButton(tr("Compare"));
    auto* cancelBtn  = new QPushButton(tr("Close"));
    auto* btnBox     = new QHBoxLayout;
    btnBox->addStretch();
    btnBox->addWidget(compareBtn_);
    btnBox->addWidget(cancelBtn);
    mainLayout->addLayout(btnBox);

    connect(compareBtn_, &QPushButton::clicked, this, &SessionCompareDialog::onCompare);
    connect(cancelBtn,   &QPushButton::clicked, this, &QDialog::reject);

    // Future watcher
    watcher_ = new QFutureWatcher<core::CompareResult>(this);
    connect(watcher_, &QFutureWatcher<core::CompareResult>::finished,
            this, &SessionCompareDialog::onWatcherFinished);
}

void SessionCompareDialog::onBrowse()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        tr("Select Reference FITS Image"),
        QString(),
        tr("FITS images (*.fits *.fit *.fts);;All files (*)"));
    if (!path.isEmpty())
        refPathEdit_->setText(path);
}

void SessionCompareDialog::onCompare()
{
    if (!session_ || session_->imageCount() == 0) {
        QMessageBox::warning(this, tr("No Images"),
                             tr("Load images into the active session first."));
        return;
    }

    const QString refPath = refPathEdit_->text().trimmed();
    if (refPath.isEmpty()) {
        QMessageBox::warning(this, tr("No Reference"),
                             tr("Please select a reference FITS image."));
        return;
    }

    // Load reference
    auto refResult = core::loadFits(refPath);
    if (!refResult) {
        QMessageBox::critical(this, tr("Load Error"),
                              tr("Could not load reference image:\n%1").arg(refResult.error()));
        return;
    }
    core::FitsImage refImg = std::move(*refResult);

    // Use the first loaded image as "new" image
    const core::FitsImage& newImg = session_->image(0);
    const float threshold = static_cast<float>(thresholdSpin_->value());

    compareBtn_->setEnabled(false);
    progressBar_->setVisible(true);
    statusLabel_->setText(tr("Running comparison…"));

    // Capture by value so lambda is safe in another thread
    QFuture<core::CompareResult> future = QtConcurrent::run(
        [newImg, refImg, threshold]() -> core::CompareResult {
            return core::compareImages(newImg, refImg, threshold);
        });

    watcher_->setFuture(future);
}

void SessionCompareDialog::onWatcherFinished()
{
    progressBar_->setVisible(false);
    compareBtn_->setEnabled(true);

    core::CompareResult result = watcher_->result();

    if (!result.error.isEmpty()) {
        statusLabel_->setText(tr("Error: %1").arg(result.error));
        QMessageBox::critical(this, tr("Comparison Failed"), result.error);
        return;
    }

    const QString method = result.usedWcs ? tr("WCS warp") : tr("direct subtraction");
    statusLabel_->setText(
        tr("Done — %1 residual source(s) detected (method: %2)")
            .arg(result.detectionCount)
            .arg(method));

    emit compareFinished(std::move(result));
    accept();
}
