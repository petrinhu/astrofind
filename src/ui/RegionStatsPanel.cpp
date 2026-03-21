#include "RegionStatsPanel.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QToolButton>

#include <algorithm>
#include <cmath>
#include <numeric>

// ─── Construction ─────────────────────────────────────────────────────────────

RegionStatsPanel::RegionStatsPanel(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setFixedWidth(220);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(10, 8, 10, 10);
    outer->setSpacing(4);

    // ── Title row ─────────────────────────────────────────────────────────────
    auto* titleRow = new QHBoxLayout;
    titleRow->setContentsMargins(0, 0, 0, 0);
    titleRow->setSpacing(4);

    auto* titleLbl = new QLabel(tr("Estatísticas de Região"), this);
    titleLbl->setStyleSheet("color:#aabbcc; font-weight:bold; font-size:8px;");

    auto* closeBtn = new QToolButton(this);
    closeBtn->setText(QStringLiteral("×"));
    closeBtn->setFixedSize(14, 14);
    closeBtn->setStyleSheet(
        "QToolButton { color:#667788; border:none; background:transparent;"
        " font-size:11px; padding:0; }");
    connect(closeBtn, &QToolButton::clicked, this, &RegionStatsPanel::closeRequested);

    titleRow->addWidget(titleLbl);
    titleRow->addStretch();
    titleRow->addWidget(closeBtn);
    outer->addLayout(titleRow);

    // ── Stats grid ────────────────────────────────────────────────────────────
    auto* grid = new QGridLayout;
    grid->setContentsMargins(0, 2, 0, 0);
    grid->setVerticalSpacing(3);
    grid->setHorizontalSpacing(6);

    const QString keyStyle = QStringLiteral("color:#667788; font-size:8px;");
    const QString valStyle = QStringLiteral(
        "color:#c9d1d9; font-family:Monospace; font-size:8px;");

    auto key = [&](const QString& t) {
        auto* l = new QLabel(t, this);
        l->setStyleSheet(keyStyle);
        return l;
    };
    auto val = [&]() {
        auto* l = new QLabel(QStringLiteral("—"), this);
        l->setStyleSheet(valStyle);
        return l;
    };

    int r = 0;
    grid->addWidget(key(tr("Região")),   r, 0); grid->addWidget(lblRegion_  = val(), r++, 1);
    grid->addWidget(key(tr("Pixels")),   r, 0); grid->addWidget(lblCount_   = val(), r++, 1);
    grid->addWidget(key(tr("Mín")),      r, 0); grid->addWidget(lblMin_     = val(), r++, 1);
    grid->addWidget(key(tr("Máx")),      r, 0); grid->addWidget(lblMax_     = val(), r++, 1);
    grid->addWidget(key(tr("Média")),    r, 0); grid->addWidget(lblMean_    = val(), r++, 1);
    grid->addWidget(key(tr("Mediana")),  r, 0); grid->addWidget(lblMedian_  = val(), r++, 1);
    grid->addWidget(key(tr("Desvio")),   r, 0); grid->addWidget(lblStddev_  = val(), r++, 1);
    grid->setColumnStretch(1, 1);
    outer->addLayout(grid);

    adjustSize();
    hide();
}

// ─── Painting ─────────────────────────────────────────────────────────────────

void RegionStatsPanel::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    // Dark rounded background
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(13, 17, 23, 215));
    p.drawRoundedRect(rect(), 6, 6);
    // Subtle border
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(QColor(48, 54, 61, 200), 1.0));
    p.drawRoundedRect(rect().adjusted(0, 0, -1, -1), 6, 6);
}

// ─── Statistics computation ───────────────────────────────────────────────────

RegionStatsPanel::Stats RegionStatsPanel::compute(
    const std::vector<float>& data, int w, int h, QRect r)
{
    r = r.intersected(QRect(0, 0, w, h));
    Stats s;
    if (r.isEmpty() || data.empty()) return s;

    std::vector<float> pixels;
    pixels.reserve(static_cast<size_t>(r.width()) * r.height());
    for (int y = r.top(); y <= r.bottom(); ++y)
        for (int x = r.left(); x <= r.right(); ++x)
            pixels.push_back(data[static_cast<size_t>(y) * w + x]);

    s.count = static_cast<int>(pixels.size());
    if (s.count == 0) return s;

    s.min = *std::min_element(pixels.begin(), pixels.end());
    s.max = *std::max_element(pixels.begin(), pixels.end());

    const double sum = std::accumulate(pixels.begin(), pixels.end(), 0.0);
    s.mean = sum / s.count;

    // Median — O(N) via nth_element
    const auto mid = pixels.size() / 2;
    std::nth_element(pixels.begin(),
                     pixels.begin() + static_cast<std::ptrdiff_t>(mid),
                     pixels.end());
    s.median = pixels[mid];
    if (pixels.size() % 2 == 0 && mid > 0) {
        const float prevMax = *std::max_element(
            pixels.begin(), pixels.begin() + static_cast<std::ptrdiff_t>(mid));
        s.median = (s.median + prevMax) / 2.0;
    }

    // Std deviation
    double sq = 0.0;
    for (float v : pixels) { const double d = v - s.mean; sq += d * d; }
    s.stddev = std::sqrt(sq / s.count);

    return s;
}

// ─── Public API ───────────────────────────────────────────────────────────────

void RegionStatsPanel::showStats(
    const std::vector<float>& data, int width, int height, QRect rect)
{
    const Stats s  = compute(data, width, height, rect);
    const QRect cr = rect.intersected(QRect(0, 0, width, height));

    lblRegion_->setText(QStringLiteral("%1,%2–%3,%4 (%5×%6)")
        .arg(cr.left()).arg(cr.top())
        .arg(cr.right()).arg(cr.bottom())
        .arg(cr.width()).arg(cr.height()));
    lblCount_ ->setText(QStringLiteral("%1").arg(s.count));
    lblMin_   ->setText(QString::number(static_cast<double>(s.min),    'f', 1));
    lblMax_   ->setText(QString::number(static_cast<double>(s.max),    'f', 1));
    lblMean_  ->setText(QString::number(s.mean,   'f', 2));
    lblMedian_->setText(QString::number(s.median, 'f', 2));
    lblStddev_->setText(QString::number(s.stddev, 'f', 2));

    adjustSize();
    show();
    raise();
}

void RegionStatsPanel::clear()
{
    hide();
}
