#pragma once

#include <QPalette>
#include <QString>
#include <QColor>

class QApplication;
class QMdiArea;

namespace Theme {

enum class Mode { Night, Day, Auto };

/// Apply the palette + stylesheet to the entire QApplication.
void apply(QApplication* app, Mode mode);

/// Background color for the MDI workspace.
QColor mdiBackground(Mode mode);

/// Persist / restore helpers.
QString toString(Mode m);
Mode    fromString(const QString& s, Mode fallback = Mode::Night);

} // namespace Theme
