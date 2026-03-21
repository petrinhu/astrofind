#pragma once

#include "core/SessionComparator.h"
#include "core/FitsImage.h"

#include <QDialog>
#include <QFutureWatcher>

class QLineEdit;
class QDoubleSpinBox;
class QPushButton;
class QLabel;
class QProgressBar;

namespace core { struct ImageSession; }

class SessionCompareDialog : public QDialog {
    Q_OBJECT
public:
    explicit SessionCompareDialog(const core::ImageSession* session,
                                  QWidget* parent = nullptr);

signals:
    /// Emitted when comparison succeeds; caller should open the residual image.
    void compareFinished(core::CompareResult result);

private slots:
    void onBrowse();
    void onCompare();
    void onWatcherFinished();

private:
    const core::ImageSession* session_ = nullptr;

    QLineEdit*      refPathEdit_      = nullptr;
    QDoubleSpinBox* thresholdSpin_    = nullptr;
    QPushButton*    compareBtn_       = nullptr;
    QLabel*         statusLabel_      = nullptr;
    QProgressBar*   progressBar_      = nullptr;

    QFutureWatcher<core::CompareResult>* watcher_ = nullptr;
};
