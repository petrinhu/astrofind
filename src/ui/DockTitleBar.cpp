#include "DockTitleBar.h"
#include "Theme.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>
#include <QToolButton>

// ─── Positioner ───────────────────────────────────────────────────────────────
// Intercepts ResizeEvents on the bar widget and sets child geometries directly,
// so the title is always centred regardless of stylesheet margin overrides.

class DockTitleBarPositioner : public QObject {
public:
    DockTitleBarPositioner(QLabel* lbl, QToolButton* btn, QObject* parent)
        : QObject(parent), lbl_(lbl), btn_(btn) {}

    bool eventFilter(QObject* obj, QEvent* ev) override
    {
        if (ev->type() == QEvent::Resize || ev->type() == QEvent::Show) {
            auto* bar = static_cast<QWidget*>(obj);
            const int w = bar->width();
            const int h = bar->height();
            if (w > 0 && h > 0) {
                const int btnSz  = Theme::dp(16);
                const int margin = Theme::dp(4);
                lbl_->setGeometry(0, 0, w, h);                              // full bar — AlignCenter handles both axes
                btn_->setGeometry(w - margin - btnSz, (h - btnSz) / 2,     // right margin, vertically centred
                                  btnSz, btnSz);
            }
        }
        return false;
    }

private:
    QLabel*      lbl_;
    QToolButton* btn_;
};

// ─── Factory ──────────────────────────────────────────────────────────────────

QWidget* makeDockTitleBar(const QString& title, QDockWidget* dock)
{
    auto* bar = new QWidget(dock);
    bar->setObjectName(QStringLiteral("dockTitleBar"));
    // bar->setFixedHeight(23);   // reference px value
    bar->setFixedHeight(Theme::dp(23));

    auto* lbl = new QLabel(title, bar);
    lbl->setObjectName(QStringLiteral("dockTitleLabel"));
    lbl->setAlignment(Qt::AlignCenter);

    auto* closeBtn = new QToolButton(bar);
    closeBtn->setObjectName(QStringLiteral("dockCloseButton"));
    closeBtn->setIcon(dock->style()->standardIcon(
        QStyle::SP_DockWidgetCloseButton, nullptr, dock));
    // closeBtn->setFixedSize(16, 16);   // reference px value
    closeBtn->setFixedSize(Theme::dp(16), Theme::dp(16));
    closeBtn->setAutoRaise(true);
    QObject::connect(closeBtn, &QToolButton::clicked, dock, &QDockWidget::close);

    // Minimal layout so that bar->sizeHint() returns a valid height and the
    // dock widget actually renders the title bar.  The positioner below then
    // overrides the child geometries on every resize.
    auto* lay = new QHBoxLayout(bar);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->addWidget(lbl, 1);
    lay->addWidget(closeBtn);

    bar->installEventFilter(new DockTitleBarPositioner(lbl, closeBtn, bar));
    closeBtn->raise();  // keep button above the full-width label
    return bar;
}
