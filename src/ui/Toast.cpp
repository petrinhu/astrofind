#include "Toast.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>

Toast::Toast(QWidget* parent, const QString& msg, Type type)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("toastFrame"));

    QString icon, bg, border;
    switch (type) {
    case Type::Success: icon = QStringLiteral("✓"); bg = QStringLiteral("#0f2318"); border = QStringLiteral("#44cc88"); break;
    case Type::Info:    icon = QStringLiteral("ℹ"); bg = QStringLiteral("#0d1b2e"); border = QStringLiteral("#4488ff"); break;
    case Type::Warning: icon = QStringLiteral("⚠"); bg = QStringLiteral("#261e08"); border = QStringLiteral("#ffaa44"); break;
    case Type::Error:   icon = QStringLiteral("✗"); bg = QStringLiteral("#250d0d"); border = QStringLiteral("#ff5555"); break;
    }

    setStyleSheet(QString(
        "QFrame#toastFrame {"
        "  background: %1;"
        "  border: 1px solid %2;"
        "  border-radius: 8px;"
        "}").arg(bg, border));

    auto* iconLabel = new QLabel(icon, this);
    iconLabel->setStyleSheet(
        QString("color: %1; font-size: 15px; font-weight: bold;").arg(border));

    auto* msgLabel = new QLabel(msg, this);
    msgLabel->setStyleSheet(QStringLiteral("color: #e6edf3; font-size: 13px;"));
    msgLabel->setWordWrap(false);

    auto* lay = new QHBoxLayout(this);
    lay->setContentsMargins(12, 9, 16, 9);
    lay->setSpacing(8);
    lay->addWidget(iconLabel);
    lay->addWidget(msgLabel);

    adjustSize();

    // Bottom-right corner of the parent, with margins for status bar and border
    const QSize ps = parent->size();
    move(ps.width() - width() - 16, ps.height() - height() - 48);

    QFrame::show();
    raise();

    QTimer::singleShot(3000, this, &QWidget::deleteLater);
}

void Toast::show(QWidget* parent, const QString& msg, Type type)
{
    if (!parent) return;
    new Toast(parent, msg, type);
}
