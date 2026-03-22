#include "ImageCatalogTable.h"
#include "Theme.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QGuiApplication>
#include <QPalette>
#include <cmath>

// Column indices
enum Col { ColType, ColRA, ColDec, ColMag, ColFwhm, ColSnr, ColX, ColY, ColCount };

static const char* kHeaders[] = {
    "Tipo", "AR (°)", "Dec (°)", "Mag", "FWHM\"", "SNR", "X px", "Y px"
};

ImageCatalogTable::ImageCatalogTable(QWidget* parent)
    : QWidget(parent)
{
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 10, 0, 0);

    table_ = new QTableWidget(0, ColCount, this);
    for (int c = 0; c < ColCount; ++c)
        table_->setHorizontalHeaderItem(c, new QTableWidgetItem(tr(kHeaders[c])));

    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    table_->verticalHeader()->setVisible(false);
    table_->horizontalHeader()->setStretchLastSection(false);
    table_->setFont(QFont(QStringLiteral("Monospace"), 8));

    lay->addWidget(table_);

    connect(table_, &QTableWidget::cellClicked,
            this,   &ImageCatalogTable::onRowDoubleClicked);
    connect(table_, &QTableWidget::cellDoubleClicked,
            this,   &ImageCatalogTable::onRowDoubleClicked);
}

static QTableWidgetItem* cell(const QString& text, const QColor& color = QColor(),
                              const QString& tooltip = QString())
{
    auto* it = new QTableWidgetItem(text);
    it->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    if (color.isValid()) it->setForeground(color);
    if (!tooltip.isEmpty()) it->setToolTip(tooltip);
    return it;
}

// Returns true when the application palette is dark-themed.
static bool isDarkTheme()
{
    const QColor win = QGuiApplication::palette().color(QPalette::Window);
    return win.lightness() < 128;
}

// Picks a color pair {dark-mode, light-mode} based on current theme.
// Dark-mode: bright/saturated (readable on dark bg).
// Light-mode: deeper/darker shade of same hue (readable on light bg).
static QColor themeColor(const QColor& dark, const QColor& light)
{
    return isDarkTheme() ? dark : light;
}

void ImageCatalogTable::setImage(const core::FitsImage& img)
{
    selectedRow_ = -1;
    table_->setUpdatesEnabled(false);
    table_->setRowCount(0);

    auto addRow = [&](const QString& type, const QColor& color, const QString& tooltip,
                      double ra, double dec, double mag,
                      double fwhm, double snr,
                      double x, double y) {
        const int row = table_->rowCount();
        table_->insertRow(row);
        table_->setItem(row, ColType, cell(type, color, tooltip));
        table_->setItem(row, ColRA,   cell(QString::number(ra,  'f', 5), color));
        table_->setItem(row, ColDec,  cell(QString::number(dec, 'f', 5), color));
        table_->setItem(row, ColMag,  mag > 0 ? cell(QString::number(mag, 'f', 1), color)
                                               : cell(QStringLiteral("—"), color));
        table_->setItem(row, ColFwhm, fwhm > 0 ? cell(QString::number(fwhm, 'f', 2), color)
                                                : cell(QStringLiteral("—"), color));
        table_->setItem(row, ColSnr,  snr  > 0 ? cell(QString::number(snr,  'f', 1), color)
                                                : cell(QStringLiteral("—"), color));
        table_->setItem(row, ColX,    cell(QString::number(x, 'f', 1), color));
        table_->setItem(row, ColY,    cell(QString::number(y, 'f', 1), color));
    };

    const QColor colEstrela   = themeColor(Theme::markDetectedStarDark(),  Theme::markDetectedStarLight());
    const QColor colGuia      = themeColor(Theme::markCatalogStarDark(),   Theme::markCatalogStarLight());
    const QColor colAsteroide = themeColor(Theme::markKooAsteroidDark(),   Theme::markKooAsteroidLight());

    const QString ttEstrela   = tr("Estrela detectada na imagem pela análise de brilho");
    const QString ttGuia      = tr("Estrela conhecida do catálogo UCAC4, usada como referência para calcular a posição dos objetos");
    const QString ttAsteroide = tr("Objeto do Sistema Solar já catalogado (asteroide, planeta ou cometa)");

    // Detected stars
    for (const auto& s : img.detectedStars) {
        double ra = 0.0, dec = 0.0;
        if (img.wcs.solved)
            img.wcs.pixToSky(s.x, s.y, ra, dec);
        const double fwhmArcsec = (img.pixScaleX > 0) ? s.fwhm() * img.pixScaleX : 0.0;
        addRow(tr("Estrela"), colEstrela, ttEstrela, ra, dec, 0.0, fwhmArcsec, s.snr, s.x, s.y);
    }

    // Catalog reference stars
    for (const auto& c : img.catalogStars) {
        double px = 0.0, py = 0.0;
        if (img.wcs.solved)
            img.wcs.skyToPix(c.ra, c.dec, px, py);
        addRow(tr("Estrela Guia"), colGuia, ttGuia, c.ra, c.dec, c.mag, 0.0, 0.0, px, py);
    }

    // Known solar-system objects
    for (const auto& k : img.kooObjects) {
        double px = 0.0, py = 0.0;
        if (img.wcs.solved)
            img.wcs.skyToPix(k.ra, k.dec, px, py);
        addRow(tr("Asteroide Conhecido"), colAsteroide, ttAsteroide, k.ra, k.dec, k.mag, 0.0, 0.0, px, py);
    }

    table_->resizeColumnsToContents();
    table_->setUpdatesEnabled(true);
}

void ImageCatalogTable::clear()
{
    selectedRow_ = -1;
    table_->setRowCount(0);
}

void ImageCatalogTable::clearSelection()
{
    selectedRow_ = -1;
    table_->clearSelection();
    table_->setCurrentItem(nullptr);
    emit objectDeselected();
}

bool ImageCatalogTable::selectRowByPixel(double px, double py)
{
    static constexpr double kThresh = 5.0;
    double bestDist = kThresh + 1.0;
    int    bestRow  = -1;

    for (int r = 0; r < table_->rowCount(); ++r) {
        auto* xi = table_->item(r, ColX);
        auto* yi = table_->item(r, ColY);
        if (!xi || !yi) continue;
        const double dx = xi->text().toDouble() - px;
        const double dy = yi->text().toDouble() - py;
        const double d  = std::sqrt(dx * dx + dy * dy);
        if (d < bestDist) { bestDist = d; bestRow = r; }
    }

    if (bestRow < 0 || bestDist > kThresh) return false;

    // Toggle: clicking the already-selected object deselects it
    if (bestRow == selectedRow_) {
        clearSelection();
        return false;
    }

    selectedRow_ = bestRow;
    table_->setCurrentCell(bestRow, 0);
    table_->scrollToItem(table_->item(bestRow, 0), QAbstractItemView::PositionAtCenter);

    auto* xi = table_->item(bestRow, ColX);
    auto* yi = table_->item(bestRow, ColY);
    emit centreOnObject(xi->text().toDouble(), yi->text().toDouble());
    emit objectSelected(xi->text().toDouble(), yi->text().toDouble());
    return true;
}

void ImageCatalogTable::onRowDoubleClicked(int row, int /*col*/)
{
    // Toggle: clicking the already-selected row deselects it
    if (row == selectedRow_) {
        clearSelection();
        return;
    }

    auto* xItem = table_->item(row, ColX);
    auto* yItem = table_->item(row, ColY);
    if (!xItem || !yItem) return;

    selectedRow_ = row;
    const double px = xItem->text().toDouble();
    const double py = yItem->text().toDouble();
    emit centreOnObject(px, py);
    emit objectSelected(px, py);
}
