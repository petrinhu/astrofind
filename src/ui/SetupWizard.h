#pragma once

#include <QWizard>
#include <QSettings>

class QLineEdit;
class QLabel;
class QProgressBar;
class QPushButton;
class QCheckBox;

/// First-run (and on-demand) setup wizard.
/// Guides the user through the three mandatory configuration items:
///   Page 1 — Observer identity (MPC code + name)
///   Page 2 — Plate-solving API key
///   Page 3 — MPCORB.DAT download (optional)
///
/// The wizard is non-blocking: every page has a "Skip" button so the user
/// can dismiss it without filling anything in.  Settings are written to
/// QSettings only when the user clicks "Finish".
class SetupWizard : public QWizard {
    Q_OBJECT
public:
    explicit SetupWizard(QSettings& settings, QWidget* parent = nullptr);

    /// Returns true if any value was changed and saved during this run.
    bool wasModified() const noexcept { return modified_; }

private slots:
    void onFinished(int result);

private:
    QSettings& settings_;
    bool       modified_ = false;
};
