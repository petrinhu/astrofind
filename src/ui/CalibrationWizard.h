#pragma once

#include <QWizard>

class QLabel;
class QLineEdit;
class QPushButton;
class QCheckBox;

namespace core { struct FitsImage; }

/// Step-by-step wizard that guides the user through loading and applying
/// dark frame and flat field calibration images.
class CalibrationWizard : public QWizard {
    Q_OBJECT
public:
    explicit CalibrationWizard(QWidget* parent = nullptr);

    /// Returns the path selected for the dark frame, or empty if none.
    QString darkPath()  const { return darkPath_; }
    /// Returns the path selected for the flat field, or empty if none.
    QString flatPath()  const { return flatPath_; }
    /// Returns true if the user wants calibration applied immediately.
    bool    applyNow()  const;

signals:
    void darkSelected(const QString& path);
    void flatSelected(const QString& path);

private:
    void buildIntroPage();
    void buildDarkPage();
    void buildFlatPage();
    void buildFinishPage();

    QString darkPath_;
    QString flatPath_;

    QLabel*      darkStatusLabel_  = nullptr;
    QLabel*      flatStatusLabel_  = nullptr;
    QPushButton* darkBrowseBtn_    = nullptr;
    QPushButton* flatBrowseBtn_    = nullptr;
    QCheckBox*   applyNowChk_      = nullptr;
};
