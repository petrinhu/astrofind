#pragma once

#include "core/Overlay.h"
#include "core/FitsImage.h"

#include <QWidget>
#include <QTableWidget>

/// Dock widget showing all objects detected in the active image:
/// detected stars, catalog reference stars, and known solar-system objects.
/// Selecting a row emits centreOnObject() so the image can pan to it.
class ImageCatalogTable : public QWidget {
    Q_OBJECT
public:
    explicit ImageCatalogTable(QWidget* parent = nullptr);

    /// Populate the table from a FitsImage's overlay data.
    void setImage(const core::FitsImage& img);

    /// Clear all rows.
    void clear();

    /// Programmatically select the row whose pixel coords are nearest to (px, py).
    /// Toggles off if that row was already selected. Returns true if an object was selected.
    bool selectRowByPixel(double px, double py);

    /// Clear the current row selection without emitting objectSelected.
    void clearSelection();

signals:
    void centreOnObject(double pixX, double pixY);
    void objectSelected(double pixX, double pixY);
    void objectDeselected();

private slots:
    void onRowDoubleClicked(int row, int col);

private:
    QTableWidget* table_       = nullptr;
    int           selectedRow_ = -1;
};
