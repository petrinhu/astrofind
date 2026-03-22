#include "SpectrumDialog.h"

#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>

SpectrumDialog::SpectrumDialog(const core::Spectrum1D& spec, QWidget* parent)
    : QDialog(parent, Qt::Tool)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Spectrum — %1").arg(spec.fileName));

    auto* series = new QLineSeries;
    series->setColor(QColor(100, 200, 255));
    const int N = spec.flux.size();
    for (int i = 0; i < N; ++i)
        series->append(spec.wavelength[i], spec.flux[i]);

    auto* chart = new QChart;
    chart->addSeries(series);
    chart->setBackgroundBrush(QColor(18, 22, 36));
    chart->setPlotAreaBackgroundBrush(QColor(10, 14, 26));
    chart->setPlotAreaBackgroundVisible(true);
    chart->legend()->hide();

    auto* axisX = new QValueAxis;
    const QString xLabel = spec.ctype1.isEmpty()
        ? tr("Channel")
        : (spec.cunit1.isEmpty() ? spec.ctype1
             : QStringLiteral("%1 (%2)").arg(spec.ctype1, spec.cunit1));
    axisX->setTitleText(xLabel);
    axisX->setLabelsColor(QColor(170, 200, 220));
    axisX->setTitleBrush(QColor(170, 200, 220));
    axisX->setGridLineColor(QColor(40, 50, 70));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis;
    const QString yLabel = spec.bunit.isEmpty() ? tr("Flux") : spec.bunit;
    axisY->setTitleText(yLabel);
    axisY->setLabelsColor(QColor(170, 200, 220));
    axisY->setTitleBrush(QColor(170, 200, 220));
    axisY->setGridLineColor(QColor(40, 50, 70));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto* chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(QColor(18, 22, 36));

    QString info = QStringLiteral("%1  %2 channels").arg(spec.fileName).arg(N);
    if (!spec.ctype1.isEmpty())
        info += QStringLiteral("  CRVAL1=%1  CDELT1=%2").arg(spec.crval1).arg(spec.cdelt1);

    auto* infoLabel = new QLabel(info, this);
    infoLabel->setStyleSheet(QStringLiteral("color:#aaaaaa; font-size:11px; padding:2px 4px;"));
    infoLabel->setAlignment(Qt::AlignCenter);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(4);
    lay->addWidget(chartView);
    lay->addWidget(infoLabel);

    resize(680, 420);
}
