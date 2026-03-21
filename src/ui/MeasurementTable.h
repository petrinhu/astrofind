#pragma once

#include "core/Measurement.h"

#include <QWidget>
#include <QVector>

class QTableWidget;
class QPushButton;

/// Dock widget showing the list of manual observations for the current session.
/// Each row corresponds to one Observation; the object name is editable.
class MeasurementTable : public QWidget {
    Q_OBJECT
public:
    explicit MeasurementTable(QWidget* parent = nullptr);

    void addObservation(const core::Observation& obs);
    void insertObservationAt(int row, const core::Observation& obs);
    void removeLastObservation();
    void removeObservationAt(int row);
    void clear();

    QVector<core::Observation> observations() const { return obs_; }

signals:
    void observationRemoved(int index);
    void observationsChanged();

private slots:
    void onRemoveSelected();
    void onNameEdited(int row, int col);

private:
    void rebuildRow(int row);

    QTableWidget*              table_;
    QPushButton*               btnRemove_;
    QVector<core::Observation> obs_;
};
