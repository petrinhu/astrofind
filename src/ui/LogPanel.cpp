#include "LogPanel.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDateTime>
#include <QPushButton>
#include <QClipboard>
#include <QApplication>

LogPanel::LogPanel(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    log_ = new QTextEdit(this);
    log_->setReadOnly(true);
    log_->setFont(QFont("Monospace", 9));
    log_->setStyleSheet(
        "QTextEdit { background:#0d1117; color:#aabbcc; border:none; }");
    log_->document()->setMaximumBlockCount(2000);
    // Fixed size hint so the dock height is identical across themes.
    log_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);

    auto* btnBar  = new QWidget(this);
    auto* btnLay  = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(4, 2, 4, 2);
    btnLay->setSpacing(4);

    auto* copyBtn  = new QPushButton(tr("Copy all"), btnBar);
    connect(copyBtn, &QPushButton::clicked, this, [this]() {
        QApplication::clipboard()->setText(log_->toPlainText());
    });

    auto* clearBtn = new QPushButton(tr("Clear"), btnBar);
    connect(clearBtn, &QPushButton::clicked, this, &LogPanel::clear);

    btnLay->addStretch();
    btnLay->addWidget(copyBtn);
    btnLay->addWidget(clearBtn);

    lay->addWidget(log_);
    lay->addWidget(btnBar);
}

void LogPanel::appendInfo(const QString& msg)
{
    const QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    log_->append(QString("<span style='color:#445566'>%1</span> "
                         "<span style='color:#aabbcc'>%2</span>").arg(ts, msg.toHtmlEscaped()));
}

void LogPanel::appendWarning(const QString& msg)
{
    const QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    log_->append(QString("<span style='color:#445566'>%1</span> "
                         "<span style='color:#ffaa44'>⚠ %2</span>").arg(ts, msg.toHtmlEscaped()));
}

void LogPanel::appendError(const QString& msg)
{
    const QString ts = QDateTime::currentDateTime().toString("HH:mm:ss");
    log_->append(QString("<span style='color:#445566'>%1</span> "
                         "<span style='color:#ff5555'>✗ %2</span>").arg(ts, msg.toHtmlEscaped()));
}

void LogPanel::clear()
{
    log_->clear();
}
