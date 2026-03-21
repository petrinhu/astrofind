#include "MasterFrameDialog.h"

#include "core/FitsImage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListWidget>
#include <QComboBox>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QDir>

MasterFrameDialog::MasterFrameDialog(FrameType type,
                                       QSettings& settings,
                                       QWidget* parent)
    : QDialog(parent)
    , type_(type)
    , settings_(settings)
{
    const bool isDark = (type_ == FrameType::Dark);
    setWindowTitle(isDark ? tr("Build Master Dark") : tr("Build Master Flat"));
    setMinimumSize(500, 380);

    // ── File list ─────────────────────────────────────────────────────────
    auto* listLabel = new QLabel(
        isDark ? tr("Dark frames to combine (≥ 2):")
               : tr("Flat frames to combine (≥ 2):"),
        this);
    fileList_ = new QListWidget(this);
    fileList_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    fileList_->setAlternatingRowColors(true);

    auto* addBtn    = new QPushButton(tr("Add Files…"), this);
    auto* removeBtn = new QPushButton(tr("Remove Selected"), this);
    auto* fileRow   = new QHBoxLayout;
    fileRow->addWidget(addBtn);
    fileRow->addWidget(removeBtn);
    fileRow->addStretch();

    // ── Method ────────────────────────────────────────────────────────────
    auto* methodLabel = new QLabel(tr("Combination method:"), this);
    methodCombo_ = new QComboBox(this);
    methodCombo_->addItem(tr("Average"),  static_cast<int>(core::MasterMode::Average));
    methodCombo_->addItem(tr("Median"),   static_cast<int>(core::MasterMode::Median));
    // Default: Median for darks (better outlier rejection), Average for flats
    methodCombo_->setCurrentIndex(isDark ? 1 : 0);

    auto* methodRow = new QHBoxLayout;
    methodRow->addWidget(methodLabel);
    methodRow->addWidget(methodCombo_);
    methodRow->addStretch();

    // ── Progress / status ─────────────────────────────────────────────────
    progress_ = new QProgressBar(this);
    progress_->setRange(0, 100);
    progress_->setValue(0);
    progress_->setVisible(false);

    statusLabel_ = new QLabel(this);
    statusLabel_->setWordWrap(true);

    // ── Buttons ───────────────────────────────────────────────────────────
    buildBtn_ = new QPushButton(tr("Build Master"), this);
    buildBtn_->setEnabled(false);
    buildBtn_->setDefault(true);
    closeBtn_ = new QPushButton(tr("Close"), this);

    auto* btnRow = new QHBoxLayout;
    btnRow->addStretch();
    btnRow->addWidget(buildBtn_);
    btnRow->addWidget(closeBtn_);

    // ── Layout ────────────────────────────────────────────────────────────
    auto* lay = new QVBoxLayout(this);
    lay->addWidget(listLabel);
    lay->addWidget(fileList_);
    lay->addLayout(fileRow);
    lay->addLayout(methodRow);
    lay->addWidget(progress_);
    lay->addWidget(statusLabel_);
    lay->addLayout(btnRow);

    // ── Connections ───────────────────────────────────────────────────────
    connect(addBtn,    &QPushButton::clicked, this, &MasterFrameDialog::onAddFiles);
    connect(removeBtn, &QPushButton::clicked, this, &MasterFrameDialog::onRemoveSelected);
    connect(buildBtn_, &QPushButton::clicked, this, &MasterFrameDialog::onBuild);
    connect(closeBtn_, &QPushButton::clicked, this, &QDialog::reject);
    connect(fileList_, &QListWidget::itemSelectionChanged, this, &MasterFrameDialog::updateBuildButton);
}

MasterFrameDialog::~MasterFrameDialog()
{
    if (watcher_ && watcher_->isRunning())
        watcher_->cancel();
}

void MasterFrameDialog::onAddFiles()
{
    const QString settingKey = QStringLiteral("paths/lastCalibDir");
    const QString lastDir = settings_.value(settingKey, QDir::homePath()).toString();

    const QStringList files = QFileDialog::getOpenFileNames(
        this, tr("Select FITS frames"), lastDir,
        tr("FITS Images (*.fits *.fit *.fts);;All files (*)"));
    if (files.isEmpty()) return;

    settings_.setValue(settingKey, QFileInfo(files.first()).absolutePath());

    for (const QString& f : files) {
        // Avoid duplicates
        bool found = false;
        for (int i = 0; i < fileList_->count(); ++i)
            if (fileList_->item(i)->text() == f) { found = true; break; }
        if (!found)
            fileList_->addItem(f);
    }
    updateBuildButton();
}

void MasterFrameDialog::onRemoveSelected()
{
    for (auto* item : fileList_->selectedItems())
        delete item;
    updateBuildButton();
}

void MasterFrameDialog::updateBuildButton()
{
    buildBtn_->setEnabled(fileList_->count() >= 2);
}

void MasterFrameDialog::onBuild()
{
    const int n = fileList_->count();
    if (n < 2) return;

    // Collect paths
    QStringList paths;
    paths.reserve(n);
    for (int i = 0; i < n; ++i)
        paths.append(fileList_->item(i)->text());

    const core::MasterMode mode =
        static_cast<core::MasterMode>(methodCombo_->currentData().toInt());

    buildBtn_->setEnabled(false);
    closeBtn_->setEnabled(false);
    progress_->setValue(0);
    progress_->setVisible(true);
    statusLabel_->setText(tr("Loading %1 frames…").arg(n));

    // Watcher for async result
    watcher_ = new QFutureWatcher<std::expected<core::FitsImage, QString>>(this);
    connect(watcher_, &QFutureWatcher<std::expected<core::FitsImage, QString>>::finished,
            this, &MasterFrameDialog::onBuildFinished);

    // Progress reporting via a queued connection on a shared atomic counter
    auto* progressBar = progress_;  // captured by lambda

    QFuture<std::expected<core::FitsImage, QString>> fut =
        QtConcurrent::run([paths, mode, progressBar]()
            -> std::expected<core::FitsImage, QString>
        {
            // Load all frames
            QVector<core::FitsImage> frames;
            frames.reserve(paths.size());
            for (const QString& p : paths) {
                auto res = core::loadFits(p);
                if (!res)
                    return std::unexpected(
                        QStringLiteral("Cannot load %1: %2")
                            .arg(QFileInfo(p).fileName(), res.error()));
                frames.append(std::move(*res));
            }

            // Build master — use invokeMethod for thread-safe progress updates
            auto progressFn = [progressBar](int pct) {
                QMetaObject::invokeMethod(progressBar, [progressBar, pct]() {
                    progressBar->setValue(pct);
                }, Qt::QueuedConnection);
            };

            return core::buildMasterFrame(frames, mode, progressFn);
        });

    watcher_->setFuture(fut);
}

void MasterFrameDialog::onBuildFinished()
{
    buildBtn_->setEnabled(fileList_->count() >= 2);
    closeBtn_->setEnabled(true);
    progress_->setVisible(false);

    auto res = watcher_->result();
    watcher_->deleteLater();
    watcher_ = nullptr;

    if (!res) {
        statusLabel_->setText(tr("<span style='color:#e05050;'>Error: %1</span>")
                                   .arg(res.error().toHtmlEscaped()));
        return;
    }

    result_ = std::move(*res);

    const QString methodName = methodCombo_->currentText();
    const bool isDark = (type_ == FrameType::Dark);
    result_.fileName = isDark
        ? QStringLiteral("master_dark_%1").arg(methodName.toLower())
        : QStringLiteral("master_flat_%1").arg(methodName.toLower());

    statusLabel_->setText(
        tr("<span style='color:#80cc80;'>✓ Master %1 built — %2×%3 px, method: %4</span>")
            .arg(isDark ? tr("dark") : tr("flat"))
            .arg(result_.width).arg(result_.height)
            .arg(methodName));

    buildBtn_->setText(tr("Rebuild"));

    // Close and return Accepted so the caller can retrieve result()
    accept();
}
