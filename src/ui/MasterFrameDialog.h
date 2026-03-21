#pragma once

#include "core/Calibration.h"
#include "core/FitsImage.h"

#include <QDialog>
#include <QSettings>
#include <expected>

class QListWidget;
class QComboBox;
class QProgressBar;
class QPushButton;
class QLabel;

template<typename T> class QFutureWatcher;

/// Dialog that lets the user stack N dark or flat FITS frames into a master frame.
/// Usage:
///   MasterFrameDialog dlg(MasterFrameDialog::Dark, settings, parent);
///   if (dlg.exec() == QDialog::Accepted)
///       masterDark = dlg.result();
class MasterFrameDialog : public QDialog {
    Q_OBJECT
public:
    enum class FrameType { Dark, Flat };

    explicit MasterFrameDialog(FrameType type,
                                QSettings& settings,
                                QWidget* parent = nullptr);
    ~MasterFrameDialog() override;

    /// Valid only after Accepted.
    const core::FitsImage& result() const { return result_; }

private slots:
    void onAddFiles();
    void onRemoveSelected();
    void onBuild();
    void onBuildFinished();

private:
    void updateBuildButton();

    FrameType         type_;
    QSettings&        settings_;

    QListWidget*      fileList_    = nullptr;
    QComboBox*        methodCombo_ = nullptr;
    QProgressBar*     progress_    = nullptr;
    QPushButton*      buildBtn_    = nullptr;
    QPushButton*      closeBtn_    = nullptr;
    QLabel*           statusLabel_ = nullptr;

    core::FitsImage   result_;
    QFutureWatcher<std::expected<core::FitsImage, QString>>* watcher_ = nullptr;
};
