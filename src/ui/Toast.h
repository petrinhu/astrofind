#pragma once

#include <QFrame>

/// Lightweight non-blocking notification overlay that auto-hides after 3 s.
/// Shown as a floating badge in the bottom-right corner of a parent widget.
class Toast : public QFrame {
    Q_OBJECT
public:
    enum class Type { Success, Info, Warning, Error };

    /// Show a toast anchored to `parent` that disappears after 3 seconds.
    static void show(QWidget* parent, const QString& msg, Type type = Type::Success);

private:
    explicit Toast(QWidget* parent, const QString& msg, Type type);
};
