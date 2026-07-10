// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>

/// Bottom dock panel showing session log messages.
class LogPanel : public QWidget {
    Q_OBJECT
public:
    explicit LogPanel(QWidget* parent = nullptr);

    void appendInfo(const QString& msg);
    void appendWarning(const QString& msg);
    void appendError(const QString& msg);
    void clear();

private:
    QTextEdit* log_ = nullptr;
};
