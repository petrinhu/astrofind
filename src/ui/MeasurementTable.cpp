#include "MeasurementTable.h"

#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidgetItem>
#include <QFileInfo>
#include <QSet>

// Column indices
enum Col {
    ColNum    = 0,
    ColObject = 1,
    ColRA     = 2,
    ColDec    = 3,
    ColMag    = 4,
    ColBand   = 5,
    ColFWHM   = 6,
    ColSNR    = 7,
    ColAirmass= 8,
    ColJD     = 9,
    ColImage  = 10,
    ColCount  = 11
};

static const char* kHeaders[] = {
    "#", "Object", "RA (°)", "Dec (°)", "Mag", "Band", "FWHM\"", "SNR", "Airmass", "JD", "Image"
};

MeasurementTable::MeasurementTable(QWidget* parent)
    : QWidget(parent)
{
    table_ = new QTableWidget(0, ColCount, this);  // ColCount = 11
    for (int c = 0; c < ColCount; ++c)
        table_->setHorizontalHeaderItem(c, new QTableWidgetItem(tr(kHeaders[c])));

    table_->horizontalHeader()->setSectionResizeMode(ColObject, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(ColImage,  QHeaderView::Stretch);
    table_->verticalHeader()->hide();
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

    // Make only the Object and Band columns user-editable; all others read-only.
    // Enforced in rebuildRow() via Qt::ItemIsEditable flag.

    btnRemove_ = new QPushButton(tr("Remove"), this);
    btnRemove_->setEnabled(false);

    auto* btnRow = new QHBoxLayout;
    btnRow->addStretch();
    btnRow->addWidget(btnRemove_);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(4, 4, 4, 4);
    lay->setSpacing(4);
    lay->addWidget(table_);
    lay->addLayout(btnRow);

    connect(table_,    &QTableWidget::itemSelectionChanged, this, [this](){
        btnRemove_->setEnabled(!table_->selectedItems().isEmpty());
    });
    connect(btnRemove_, &QPushButton::clicked, this, &MeasurementTable::onRemoveSelected);
    connect(table_,    &QTableWidget::cellChanged, this, &MeasurementTable::onNameEdited);
}

void MeasurementTable::addObservation(const core::Observation& obs)
{
    const int row = table_->rowCount();
    table_->insertRow(row);
    obs_.append(obs);
    rebuildRow(row);
    emit observationsChanged();
}

void MeasurementTable::insertObservationAt(int row, const core::Observation& obs)
{
    row = std::clamp(row, 0, static_cast<int>(obs_.size()));
    table_->insertRow(row);
    obs_.insert(row, obs);
    rebuildRow(row);
    emit observationsChanged();
}

void MeasurementTable::removeLastObservation()
{
    if (obs_.isEmpty()) return;
    table_->removeRow(table_->rowCount() - 1);
    obs_.removeLast();
    emit observationsChanged();
}

void MeasurementTable::removeObservationAt(int row)
{
    if (row < 0 || row >= obs_.size()) return;
    table_->removeRow(row);
    obs_.removeAt(row);
    emit observationsChanged();
}

void MeasurementTable::clear()
{
    table_->setRowCount(0);
    obs_.clear();
    emit observationsChanged();
}

static QTableWidgetItem* roItem(const QString& text)
{
    auto* item = new QTableWidgetItem(text);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);
    return item;
}

void MeasurementTable::rebuildRow(int row)
{
    if (row < 0 || row >= obs_.size()) return;
    const auto& o = obs_[row];

    // Block cellChanged while we fill the row
    table_->blockSignals(true);

    table_->setItem(row, ColNum,    roItem(QString::number(row + 1)));
    // Object name — editable
    auto* nameItem = new QTableWidgetItem(o.objectName.isEmpty() ? tr("?") : o.objectName);
    table_->setItem(row, ColObject, nameItem);

    table_->setItem(row, ColRA,   roItem(QString::number(o.ra,  'f', 6)));
    table_->setItem(row, ColDec,  roItem(QString::number(o.dec, 'f', 6)));

    if (o.mag < 90.0)
        table_->setItem(row, ColMag, roItem(QString::number(o.mag, 'f', 2)));
    else
        table_->setItem(row, ColMag, roItem(QStringLiteral("—")));

    // Band — editable
    auto* bandItem = new QTableWidgetItem(o.magBand.isEmpty() ? QStringLiteral("C") : o.magBand);
    table_->setItem(row, ColBand, bandItem);

    table_->setItem(row, ColFWHM,    roItem(o.fwhm > 0.0 ? QString::number(o.fwhm, 'f', 1) : QStringLiteral("—")));
    table_->setItem(row, ColSNR,     roItem(o.snr  > 0.0 ? QString::number(o.snr,  'f', 1) : QStringLiteral("—")));
    table_->setItem(row, ColAirmass, roItem(o.airmass > 0.0 ? QString::number(o.airmass, 'f', 3) : QStringLiteral("—")));
    table_->setItem(row, ColJD,      roItem(QString::number(o.jd, 'f', 6)));
    table_->setItem(row, ColImage, roItem(o.imagePath.isEmpty() ? QStringLiteral("—")
                                         : QFileInfo(o.imagePath).fileName()));

    table_->blockSignals(false);
}

void MeasurementTable::onRemoveSelected()
{
    const auto sel = table_->selectedItems();
    if (sel.isEmpty()) return;

    // Collect unique rows in reverse order so removing doesn't shift indices
    QSet<int> rows;
    for (auto* item : sel)
        rows.insert(item->row());

    QList<int> sorted = rows.values();
    std::sort(sorted.rbegin(), sorted.rend());

    for (int row : sorted) {
        table_->removeRow(row);
        emit observationRemoved(row);
        obs_.removeAt(row);
    }

    // Renumber column 0
    table_->blockSignals(true);
    for (int r = 0; r < table_->rowCount(); ++r)
        if (auto* it = table_->item(r, ColNum))
            it->setText(QString::number(r + 1));
    table_->blockSignals(false);
}

void MeasurementTable::onNameEdited(int row, int col)
{
    if (col != ColObject && col != ColBand) return;
    if (row < 0 || row >= obs_.size()) return;

    auto* item = table_->item(row, col);
    if (!item) return;

    if (col == ColObject)
        obs_[row].objectName = item->text();
    else
        obs_[row].magBand = item->text();
}
