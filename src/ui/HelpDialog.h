#pragma once

#include <QDialog>

class QTextBrowser;
class QComboBox;

/// Modeless help window that displays local HTML documentation.
/// Supports English and Portuguese (BR) via a language selector.
class HelpDialog : public QDialog {
    Q_OBJECT
public:
    explicit HelpDialog(QWidget* parent = nullptr);

    /// Open help at a specific anchor (e.g. "stacking", "shortcuts")
    void jumpTo(const QString& anchor);

private slots:
    void onLanguageChanged(int index);

private:
    void loadPage(const QString& resourcePath);

    QTextBrowser* browser_  = nullptr;
    QComboBox*    langBox_  = nullptr;
};
