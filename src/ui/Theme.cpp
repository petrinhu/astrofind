#include "Theme.h"

#include <QApplication>
#include <QGuiApplication>
#include <QStyleHints>
#include <QPalette>
#include <QStyle>

// ─── Palettes ─────────────────────────────────────────────────────────────────

static QPalette nightPalette()
{
    QPalette p;

    // ── Active / Inactive ────────────────────────────────────────────────────
    const QColor win   (0x1e, 0x1e, 0x2e);   // main window bg
    const QColor base  (0x12, 0x12, 0x20);   // text-input bg (QLineEdit, lists)
    const QColor altBase(0x1a, 0x1a, 0x30);  // alternate row bg
    const QColor btn   (0x2a, 0x2a, 0x42);   // button / toolbar bg
    const QColor text  (0xd0, 0xd0, 0xe0);   // normal text
    const QColor bright(0xff, 0xff, 0xff);
    const QColor mid   (0x30, 0x30, 0x50);
    const QColor dark  (0x10, 0x10, 0x1c);
    const QColor shadow(0x08, 0x08, 0x14);
    const QColor hi    (0x40, 0x60, 0xb0);   // selection highlight
    const QColor hiTxt (0xff, 0xff, 0xff);
    const QColor link  (0x60, 0xa0, 0xff);
    const QColor tip   (0x22, 0x26, 0x3c);

    for (auto group : { QPalette::Active, QPalette::Inactive }) {
        p.setColor(group, QPalette::Window,          win);
        p.setColor(group, QPalette::WindowText,      text);
        p.setColor(group, QPalette::Base,            base);
        p.setColor(group, QPalette::AlternateBase,   altBase);
        p.setColor(group, QPalette::Text,            text);
        p.setColor(group, QPalette::BrightText,      bright);
        p.setColor(group, QPalette::Button,          btn);
        p.setColor(group, QPalette::ButtonText,      text);
        p.setColor(group, QPalette::Mid,             mid);
        p.setColor(group, QPalette::Dark,            dark);
        p.setColor(group, QPalette::Shadow,          shadow);
        p.setColor(group, QPalette::Highlight,       hi);
        p.setColor(group, QPalette::HighlightedText, hiTxt);
        p.setColor(group, QPalette::Link,            link);
        p.setColor(group, QPalette::LinkVisited,     QColor(0xa0, 0x70, 0xff));
        p.setColor(group, QPalette::ToolTipBase,     tip);
        p.setColor(group, QPalette::ToolTipText,     text);
    }

    // ── Disabled ─────────────────────────────────────────────────────────────
    const QColor disText(0x60, 0x60, 0x80);
    p.setColor(QPalette::Disabled, QPalette::WindowText,  disText);
    p.setColor(QPalette::Disabled, QPalette::Text,        disText);
    p.setColor(QPalette::Disabled, QPalette::ButtonText,  disText);
    p.setColor(QPalette::Disabled, QPalette::Highlight,   QColor(0x28, 0x38, 0x60));
    p.setColor(QPalette::Disabled, QPalette::HighlightedText, disText);

    return p;
}

// ─── Stylesheets ──────────────────────────────────────────────────────────────

static const char* nightQss()
{
    return R"(
QMainWindow, QDialog {
    background: #1e1e2e;
}
QMdiArea {
    background: #121222;
}
QMenuBar {
    background: #16162a;
    color: #d0d0e0;
    border-bottom: 1px solid #30304c;
}
QMenuBar::item:selected {
    background: #2e2e50;
}
QMenu {
    background: #22223a;
    color: #d0d0e0;
    border: 1px solid #3a3a5c;
}
QMenu::item:selected {
    background: #3c3c68;
}
QMenu::separator {
    height: 1px;
    background: #3a3a5c;
    margin: 3px 6px;
}
QToolBar {
    background: #1e1e2e;
    border: none;
    spacing: 2px;
}
QToolBar::separator {
    background: #3a3a5c;
    width: 1px;
    margin: 4px 2px;
}
QToolButton {
    background: transparent;
    border: 1px solid transparent;
    border-radius: 4px;
    padding: 3px;
    margin: 1px;
}
QToolButton:hover {
    background: #404058;
    border-color: #6868a0;
}
QToolButton:pressed {
    background: #0e0e20;
    border-top-color: #08080e;
    border-left-color: #08080e;
    border-bottom-color: #383858;
    border-right-color: #383858;
}
QToolButton:disabled {
    color: #585870;
}
QDockWidget {
    color: #b0b0cc;
    titlebar-close-icon: url(none);
}
QDockWidget::title {
    background: #1a1a30;
    padding: 4px 6px;
    border-bottom: 1px solid #30304c;
}
QDockWidget::close-button, QDockWidget::float-button {
    border: none;
    background: transparent;
    padding: 0px;
}
QDockWidget::close-button:hover, QDockWidget::float-button:hover {
    background: #2e2e50;
    border-radius: 3px;
}
QWidget#dockTitleBar {
    background: #1a1a30;
    border-bottom: 1px solid #30304c;
}
QWidget#dockTitleBar QLabel#dockTitleLabel {
    color: #b0b0cc;
    font-size: 11px;
    font-weight: 500;
}
QWidget#dockTitleBar QToolButton#dockCloseButton {
    background: transparent;
    border: none;
    color: #7080a0;
}
QWidget#dockTitleBar QToolButton#dockCloseButton:hover {
    background: #2e2e50;
    border-radius: 3px;
}
QStatusBar {
    background: #141422;
    color: #808090;
    border-top: 1px solid #2a2a40;
}
QStatusBar::item {
    border: none;
}
QLabel {
    color: #d0d0e0;
    background: transparent;
}
QSpinBox {
    background: #22223a;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    border-radius: 4px;
    padding: 1px 4px;
    selection-background-color: #4060b0;
}
QSpinBox::up-button, QSpinBox::down-button {
    background: #2a2a44;
    border: none;
    width: 14px;
}
QSpinBox::up-button:hover, QSpinBox::down-button:hover {
    background: #383860;
}
QScrollBar:vertical, QScrollBar:horizontal {
    background: #1a1a2e;
    width: 10px;
    height: 10px;
}
QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
    background: #3a3a60;
    border-radius: 5px;
    min-height: 24px;
    min-width: 24px;
}
QScrollBar::handle:hover {
    background: #5050a0;
}
QScrollBar::add-line, QScrollBar::sub-line { background: none; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }
QSplitter::handle {
    background: #2a2a44;
}
QAbstractScrollArea {
    background: #12122a;
}
QTextEdit, QPlainTextEdit {
    background: #12122a;
    color: #c8c8e0;
    border: 1px solid #2e2e50;
    selection-background-color: #4060b0;
}
QListWidget, QTreeWidget, QTableWidget {
    background: #12122a;
    alternate-background-color: #1a1a30;
    color: #c8c8e0;
    border: 1px solid #2e2e50;
    gridline-color: #2a2a44;
}
QHeaderView::section {
    background: #22223a;
    color: #b0b0cc;
    border: 1px solid #2e2e50;
    padding: 3px 6px;
}
QPushButton {
    background: #2a2a44;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    border-radius: 4px;
    padding: 4px 12px;
}
QPushButton:hover {
    background: #363660;
    border-color: #5050a0;
}
QPushButton:pressed {
    background: #222240;
}
QPushButton:default {
    border-color: #4060b0;
}
QGroupBox {
    color: #a0a0c0;
    border: 1px solid #2e2e50;
    border-radius: 5px;
    margin-top: 1em;
    padding-top: 6px;
}
QGroupBox::title {
    subcontrol-origin: margin;
    subcontrol-position: top left;
    padding: 0 4px;
    color: #8080b0;
}
QTabWidget::pane {
    border: 1px solid #2e2e50;
    background: #1a1a2e;
}
QTabBar::tab {
    background: #1e1e30;
    color: #9090b0;
    border: 1px solid #2e2e50;
    padding: 4px 10px;
    border-bottom: none;
}
QTabBar::tab:selected {
    background: #2a2a44;
    color: #d0d0e0;
}
QTabBar::tab:hover {
    background: #262642;
}
QLineEdit {
    background: #12122a;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    border-radius: 4px;
    padding: 2px 6px;
    selection-background-color: #4060b0;
}
QComboBox {
    background: #22223a;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    border-radius: 4px;
    padding: 2px 6px;
}
QComboBox::drop-down {
    background: #2a2a44;
    border: none;
    width: 18px;
}
QComboBox QAbstractItemView {
    background: #22223a;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    selection-background-color: #4060b0;
}
QCheckBox, QRadioButton {
    color: #d0d0e0;
}
QCheckBox::indicator, QRadioButton::indicator {
    border: 1px solid #4a4a70;
    background: #12122a;
}
QCheckBox::indicator:checked {
    background: #4060b0;
    border-color: #5070c0;
}
QProgressBar {
    background: #1a1a2e;
    color: #d0d0e0;
    border: 1px solid #2e2e50;
    border-radius: 4px;
    text-align: center;
}
QProgressBar::chunk {
    background: #4060b0;
    border-radius: 3px;
}
QToolTip {
    background: #22263c;
    color: #d0d0e0;
    border: 1px solid #3a3a60;
    padding: 3px 6px;
    border-radius: 3px;
}
QMdiSubWindow {
    background: #1e1e2e;
}
QMdiSubWindow::title {
    background: #22223a;
    color: #b0b0cc;
}
)";
}

// ─── Public API ───────────────────────────────────────────────────────────────

void Theme::apply(QApplication* app, Mode mode)
{
    Mode effective = mode;
    if (mode == Mode::Auto) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        const auto scheme = QGuiApplication::styleHints()->colorScheme();
        effective = (scheme == Qt::ColorScheme::Light) ? Mode::Day : Mode::Night;
#else
        effective = Mode::Night;  // default to dark on Qt < 6.5 (no system query)
#endif
    }
    if (effective == Mode::Night) {
        app->setPalette(nightPalette());
        app->setStyleSheet(QString::fromUtf8(nightQss()));
    } else {
        app->setPalette(app->style()->standardPalette());
        // Mirror the structural properties of the night stylesheet so that
        // toolbar height and dock title height are identical in both modes.
        app->setStyleSheet(QStringLiteral(R"(
QMdiArea {
    background: #c0c8d8;
}
QToolBar {
    spacing: 2px;
}
QToolBar::separator {
    width: 1px;
    margin: 4px 2px;
    background: palette(mid);
}
QToolButton {
    border: 1px solid transparent;
    border-radius: 4px;
    padding: 3px;
    margin: 1px;
}
QToolButton:hover {
    background: palette(midlight);
    border: 1px solid palette(mid);
}
QToolButton:pressed {
    background: palette(dark);
    border-top-color: palette(shadow);
    border-left-color: palette(shadow);
    border-bottom-color: palette(mid);
    border-right-color: palette(mid);
}
QDockWidget::title {
    padding: 4px 6px;
}
QPushButton {
    background: palette(button);
    color: palette(buttonText);
    border: 1px solid palette(mid);
    border-radius: 4px;
    padding: 4px 12px;
    min-height: 0px;
}
QPushButton:hover {
    background: palette(light);
    border-color: palette(dark);
}
QPushButton:pressed {
    background: palette(midlight);
    border-top-color: palette(shadow);
    border-left-color: palette(shadow);
    border-bottom-color: palette(mid);
    border-right-color: palette(mid);
}
QPushButton:disabled {
    color: palette(mid);
    border-color: palette(midlight);
}
QPushButton:default {
    border-color: palette(highlight);
}
QLineEdit {
    border-radius: 4px;
    padding: 2px 6px;
}
QSpinBox {
    border-radius: 4px;
    padding: 1px 4px;
}
QDoubleSpinBox {
    border-radius: 4px;
    padding: 1px 4px;
}
QComboBox {
    border-radius: 4px;
    padding: 2px 6px;
}
QGroupBox {
    margin-top: 1em;
    padding-top: 6px;
}
QHeaderView::section {
    border: 1px solid palette(mid);
    padding: 3px 6px;
}
QScrollBar:vertical {
    width: 10px;
}
QScrollBar:horizontal {
    height: 10px;
}
QScrollBar::handle:vertical, QScrollBar::handle:horizontal {
    border-radius: 5px;
    min-height: 24px;
    min-width: 24px;
}
QScrollBar::add-line, QScrollBar::sub-line { background: none; border: none; }
QScrollBar::add-page, QScrollBar::sub-page { background: none; }
QWidget#dockTitleBar {
    background: palette(mid);
    border-bottom: 1px solid palette(dark);
}
QWidget#dockTitleBar QLabel#dockTitleLabel {
    color: palette(windowText);
    font-size: 11px;
    font-weight: 500;
}
QWidget#dockTitleBar QToolButton#dockCloseButton {
    background: transparent;
    border: none;
}
QWidget#dockTitleBar QToolButton#dockCloseButton:hover {
    background: palette(button);
    border-radius: 3px;
}
)"));
    }
}

QColor Theme::mdiBackground(Mode mode)
{
    Mode effective = mode;
    if (mode == Mode::Auto) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        const auto scheme = QGuiApplication::styleHints()->colorScheme();
        effective = (scheme == Qt::ColorScheme::Light) ? Mode::Day : Mode::Night;
#else
        effective = Mode::Night;
#endif
    }
    return effective == Mode::Night
        ? QColor(0x12, 0x12, 0x22)
        : QColor(0xc0, 0xc8, 0xd8);   // slate-blue gray for day mode
}

QString Theme::toString(Mode m)
{
    switch (m) {
        case Mode::Night: return QStringLiteral("night");
        case Mode::Day:   return QStringLiteral("day");
        case Mode::Auto:  return QStringLiteral("auto");
    }
    return QStringLiteral("night");
}

Theme::Mode Theme::fromString(const QString& s, Mode fallback)
{
    if (s == QLatin1String("day"))   return Mode::Day;
    if (s == QLatin1String("night")) return Mode::Night;
    if (s == QLatin1String("auto"))  return Mode::Auto;
    return fallback;
}
