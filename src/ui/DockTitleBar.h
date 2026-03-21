#pragma once

#include <QDockWidget>
#include <QWidget>

/// Creates a custom title bar for a QDockWidget with the title text
/// centred both horizontally and vertically, and a close button on the right.
///
/// Qt's stylesheet engine can override setContentsMargins() on widgets that
/// match a stylesheet rule, making layout-based vertical centering unreliable.
/// This implementation bypasses that by positioning children geometrically
/// via a ResizeEvent filter instead of using a QLayout for placement.
QWidget* makeDockTitleBar(const QString& title, QDockWidget* dock);
