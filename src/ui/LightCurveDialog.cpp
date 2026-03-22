#include <QTimeZone>
#include "LightCurveDialog.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QDateTime>
#include <QToolTip>
#include <QImage>
#include <cmath>
#include <algorithm>

// ─── Colour palette for objects ───────────────────────────────────────────────

static const QColor kPalette[] = {
    QColor(255,  85,  85),   // red
    QColor( 85, 170, 255),   // sky blue
    QColor( 85, 220,  85),   // green
    QColor(255, 200,  55),   // amber
    QColor(200, 100, 255),   // violet
    QColor(255, 145,  40),   // orange
    QColor( 55, 210, 190),   // teal
    QColor(220, 140, 180),   // rose
};
static constexpr int kPaletteSize = static_cast<int>(std::size(kPalette));

// ─── LightCurvePlot ───────────────────────────────────────────────────────────

class LightCurvePlot : public QWidget {
    Q_OBJECT
public:
    explicit LightCurvePlot(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        setMinimumSize(420, 280);
    }

    void setData(const QVector<core::Observation>& obs,
                 const QMap<QString, QColor>& colorMap)
    {
        obs_      = obs;
        colorMap_ = colorMap;
        hovered_  = -1;
        active_   = -1;
        computeRange();
        update();
    }

    void setActive(int imageIndex)
    {
        active_ = imageIndex;
        update();
    }

    QImage renderImage() const
    {
        QImage img(size(), QImage::Format_RGB32);
        img.fill(palette().color(QPalette::Base));
        QPainter p(&img);
        drawAll(p, rect());
        return img;
    }

signals:
    void pointClicked(int imageIndex);
    void pointHovered(const QString& info);

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), palette().color(QPalette::Base));
        drawAll(p, rect());
    }

    void mouseMoveEvent(QMouseEvent* ev) override
    {
        const int prev = hovered_;
        hovered_ = hitTest(ev->pos());
        if (hovered_ != prev) {
            update();
            if (hovered_ >= 0) {
                const auto& ob = obs_[hovered_];
                emit pointHovered(
                    QString("%1  JD %2  mag %3±%4")
                        .arg(ob.objectName)
                        .arg(ob.jd,     0, 'f', 5)
                        .arg(ob.mag,    0, 'f', 2)
                        .arg(ob.magErr, 0, 'f', 2));
            } else {
                emit pointHovered(QString());
            }
        }
    }

    void mousePressEvent(QMouseEvent* ev) override
    {
        if (ev->button() != Qt::LeftButton) return;
        const int idx = hitTest(ev->pos());
        if (idx >= 0)
            emit pointClicked(obs_[idx].imageIndex);
    }

    void leaveEvent(QEvent*) override
    {
        hovered_ = -1;
        update();
        emit pointHovered(QString());
    }

    void resizeEvent(QResizeEvent*) override { update(); }

private:
    // ── Data ────────────────────────────────────────────────────────────
    QVector<core::Observation> obs_;
    QMap<QString, QColor>      colorMap_;
    int hovered_ = -1;
    int active_  = -1;   ///< image index to highlight (-1 = none)

    // ── Computed range ──────────────────────────────────────────────────
    double jdMin_  = 0.0, jdMax_  = 1.0;
    double magLow_ = 0.0, magHigh_= 1.0;  // magLow = brightest (smallest number)

    // ── Screen points (parallel to obs_) ────────────────────────────────
    mutable QVector<QPointF> pts_;
    mutable QRectF           plotRect_;

    // ── Layout constants ─────────────────────────────────────────────────
    static constexpr int kML = 58, kMR = 16, kMT = 18, kMB = 46;

    // ─────────────────────────────────────────────────────────────────────

    void computeRange()
    {
        if (obs_.isEmpty()) return;
        jdMin_ = jdMax_ = obs_[0].jd;
        magLow_ = magHigh_ = obs_[0].mag;
        for (const auto& ob : obs_) {
            jdMin_   = std::min(jdMin_,  ob.jd);
            jdMax_   = std::max(jdMax_,  ob.jd);
            magLow_  = std::min(magLow_,  ob.mag - ob.magErr);
            magHigh_ = std::max(magHigh_, ob.mag + ob.magErr);
        }
        // Ensure non-degenerate ranges
        const double jdSpan  = std::max(jdMax_ - jdMin_, 1.0 / 1440.0); // min 1 min
        const double magSpan = std::max(magHigh_ - magLow_, 0.3);
        jdMin_   -= jdSpan  * 0.08;
        jdMax_   += jdSpan  * 0.08;
        magLow_  -= magSpan * 0.12;
        magHigh_ += magSpan * 0.12;
    }

    QRectF computePlotRect(const QRectF& widgetRect) const
    {
        return QRectF(widgetRect.left()  + kML,
                      widgetRect.top()   + kMT,
                      widgetRect.width() - kML - kMR,
                      widgetRect.height()- kMT - kMB);
    }

    QPointF toScreen(double jd, double mag, const QRectF& pr) const
    {
        const double x = pr.left() + (jd  - jdMin_) / (jdMax_  - jdMin_)  * pr.width();
        // Y: magLow (brightest) → top, magHigh (faintest) → bottom
        const double y = pr.top()  + (mag - magLow_) / (magHigh_- magLow_) * pr.height();
        return {x, y};
    }

    int hitTest(const QPointF& pos) const
    {
        constexpr double kRadius = 9.0;
        int   best = -1;
        double bestD = kRadius * kRadius;
        for (int i = 0; i < pts_.size(); ++i) {
            const double dx = pos.x() - pts_[i].x();
            const double dy = pos.y() - pts_[i].y();
            const double d  = dx*dx + dy*dy;
            if (d < bestD) { bestD = d; best = i; }
        }
        return best;
    }

    void drawAll(QPainter& p, const QRect& widgetRect) const
    {
        const QColor fgColor  = palette().color(QPalette::Text);
        const QColor gridColor= fgColor.darker(300);

        if (obs_.isEmpty()) {
            p.setPen(gridColor);
            p.setFont(QFont(font().family(), 11));
            p.drawText(widgetRect, Qt::AlignCenter,
                       tr("No photometric observations.\n"
                          "Use the Measure tool (M) to click on objects."));
            return;
        }

        plotRect_ = computePlotRect(QRectF(widgetRect));

        // ── Plot background ─────────────────────────────────────────────
        const QColor plotBg = palette().color(QPalette::Base).darker(115);
        p.fillRect(plotRect_, plotBg);

        // ── Grid + Y-axis ticks ─────────────────────────────────────────
        const double magSpan = magHigh_ - magLow_;
        double tickStep = 0.1;
        for (double s : {2.0, 1.0, 0.5, 0.2, 0.1}) {
            if (magSpan / s <= 8) { tickStep = s; break; }
        }

        const double firstTick = std::ceil(magLow_ / tickStep) * tickStep;
        const QFont  smallFont(font().family(), 8);
        p.setFont(smallFont);
        p.setPen(gridColor);

        for (double m = firstTick; m <= magHigh_ + 1e-9; m += tickStep) {
            const double y = plotRect_.top() + (m - magLow_) / (magHigh_ - magLow_)
                             * plotRect_.height();
            if (y < plotRect_.top() || y > plotRect_.bottom()) continue;
            p.setPen(QPen(gridColor, 1, Qt::DashLine));
            p.drawLine(QPointF(plotRect_.left(), y), QPointF(plotRect_.right(), y));
            p.setPen(fgColor);
            p.drawText(QRectF(0, y - 10, kML - 4, 20),
                       Qt::AlignRight | Qt::AlignVCenter,
                       QString::number(m, 'f', tickStep < 0.5 ? 1 : 0));
        }

        // ── Y-axis label ────────────────────────────────────────────────
        p.save();
        p.translate(10, plotRect_.center().y());
        p.rotate(-90);
        p.setPen(fgColor);
        p.setFont(QFont(font().family(), 9));
        p.drawText(QRectF(-60, -10, 120, 20), Qt::AlignCenter, tr("Magnitude"));
        p.restore();

        // ── X-axis ticks (UTC time) ─────────────────────────────────────
        const double jdSpan = jdMax_ - jdMin_;
        // Choose step so we get 4-7 ticks
        const double stepCandidates[] = {
            1.0/1440, 2.0/1440, 5.0/1440, 10.0/1440, 15.0/1440,
            30.0/1440, 1.0/24, 2.0/24, 4.0/24, 6.0/24, 12.0/24, 1.0
        };
        double xStep = stepCandidates[0];
        for (double s : stepCandidates) {
            if (jdSpan / s >= 4 && jdSpan / s <= 8) { xStep = s; break; }
            if (jdSpan / s < 4) { xStep = s; break; }
        }

        const auto jdToQDT = [](double jd) {
            return QDateTime::fromMSecsSinceEpoch(
                qint64((jd - 2440587.5) * 86400000.0), QTimeZone(0));
        };

        const double jdFirst = std::ceil(jdMin_ / xStep) * xStep;
        p.setFont(smallFont);
        for (double jd = jdFirst; jd <= jdMax_ + 1e-9; jd += xStep) {
            const double x = plotRect_.left() + (jd - jdMin_) / jdSpan * plotRect_.width();
            if (x < plotRect_.left() || x > plotRect_.right()) continue;
            p.setPen(QPen(gridColor, 1, Qt::DashLine));
            p.drawLine(QPointF(x, plotRect_.top()), QPointF(x, plotRect_.bottom()));
            const QString label = jdToQDT(jd).toString(
                xStep >= 1.0 ? "dd MMM" : (xStep >= 1.0/24 ? "HH:mm" : "HH:mm:ss"));
            p.setPen(fgColor);
            p.drawText(QRectF(x - 35, plotRect_.bottom() + 4, 70, 20),
                       Qt::AlignHCenter | Qt::AlignTop, label);
        }

        // X-axis label
        p.setFont(QFont(font().family(), 9));
        p.setPen(fgColor);
        p.drawText(QRectF(plotRect_.left(), plotRect_.bottom() + 26,
                          plotRect_.width(), 18),
                   Qt::AlignCenter, tr("Time (UTC)"));

        // ── Border ──────────────────────────────────────────────────────
        p.setPen(QPen(fgColor, 1));
        p.setBrush(Qt::NoBrush);
        p.drawRect(plotRect_);

        // ── Data points ─────────────────────────────────────────────────
        pts_.resize(obs_.size());
        for (int i = 0; i < obs_.size(); ++i) {
            const auto& ob = obs_[i];
            pts_[i] = toScreen(ob.jd, ob.mag, plotRect_);
        }

        for (int i = 0; i < obs_.size(); ++i) {
            const auto& ob  = obs_[i];
            const QPointF pt = pts_[i];
            const QColor  col = colorMap_.value(ob.objectName, kPalette[0]);

            // Error bar
            if (ob.magErr > 0.0) {
                const QPointF top = toScreen(ob.jd, ob.mag - ob.magErr, plotRect_);
                const QPointF bot = toScreen(ob.jd, ob.mag + ob.magErr, plotRect_);
                p.setPen(QPen(col.lighter(150), 1.2));
                p.drawLine(top, bot);
                p.drawLine(QPointF(pt.x()-3, top.y()), QPointF(pt.x()+3, top.y()));
                p.drawLine(QPointF(pt.x()-3, bot.y()), QPointF(pt.x()+3, bot.y()));
            }

            // Point
            const bool isActive  = (ob.imageIndex == active_);
            const bool isHovered = (i == hovered_);
            const double r = isHovered ? 6.5 : 4.5;

            p.setPen(QPen(isHovered ? Qt::white : col.lighter(170), 1.0));
            p.setBrush(isActive ? Qt::white : col);
            p.drawEllipse(pt, r, r);
        }

        // ── Legend ──────────────────────────────────────────────────────
        const QStringList names = colorMap_.keys();
        if (names.size() > 1) {
            const QFont legFont(font().family(), 8);
            p.setFont(legFont);
            int lx = static_cast<int>(plotRect_.right()) - 8;
            int ly = static_cast<int>(plotRect_.top()) + 6;
            for (const QString& n : names) {
                const QColor c = colorMap_.value(n);
                const QFontMetrics fm(legFont);
                const int tw = fm.horizontalAdvance(n);
                const int bw = tw + 22;
                p.fillRect(lx - bw, ly, bw, 14, plotBg);
                p.setBrush(c);
                p.setPen(Qt::NoPen);
                p.drawEllipse(QRectF(lx - bw + 4, ly + 3, 8, 8));
                p.setPen(fgColor);
                p.drawText(lx - bw + 16, ly + 11, n);
                ly += 17;
            }
        }
    }
};

#include "LightCurveDialog.moc"

// ─── LightCurveDialog ─────────────────────────────────────────────────────────

LightCurveDialog::LightCurveDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Light Curve"));
    setMinimumSize(520, 360);
    resize(680, 420);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);

    // ── Controls row ──────────────────────────────────────────────────────
    auto* filterLabel = new QLabel(tr("Object:"), this);
    filterCombo_ = new QComboBox(this);
    filterCombo_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto* exportBtn = new QPushButton(tr("Export PNG…"), this);
    auto* closeBtn  = new QPushButton(tr("Close"), this);
    connect(exportBtn, &QPushButton::clicked, this, &LightCurveDialog::onExportPng);
    connect(closeBtn,  &QPushButton::clicked, this, &QDialog::accept);

    auto* topRow = new QHBoxLayout;
    topRow->addWidget(filterLabel);
    topRow->addWidget(filterCombo_, 1);
    topRow->addStretch();
    topRow->addWidget(exportBtn);
    topRow->addWidget(closeBtn);

    // ── Plot ─────────────────────────────────────────────────────────────
    plot_ = new LightCurvePlot(this);
    plot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // ── Status bar ────────────────────────────────────────────────────────
    statusLabel_ = new QLabel(this);
    statusLabel_->setStyleSheet(QStringLiteral("font-size:9pt; color: gray;"));

    auto* lay = new QVBoxLayout(this);
    lay->addLayout(topRow);
    lay->addWidget(plot_, 1);
    lay->addWidget(statusLabel_);

    connect(filterCombo_, &QComboBox::currentTextChanged,
            this, &LightCurveDialog::onFilterChanged);
    connect(plot_, &LightCurvePlot::pointClicked,
            this, &LightCurveDialog::imageActivated);
    connect(plot_, &LightCurvePlot::pointHovered,
            statusLabel_, &QLabel::setText);
}

void LightCurveDialog::setObservations(const QVector<core::Observation>& obs)
{
    allObs_ = obs;

    // Rebuild filter combo (preserve selection if possible)
    const QString prev = filterCombo_->currentText();
    filterCombo_->blockSignals(true);
    filterCombo_->clear();
    filterCombo_->addItem(tr("All objects"));

    QStringList names;
    for (const auto& ob : allObs_)
        if (!names.contains(ob.objectName))
            names.append(ob.objectName);
    for (const QString& n : names)
        filterCombo_->addItem(n);

    const int idx = filterCombo_->findText(prev);
    filterCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
    filterCombo_->blockSignals(false);

    onFilterChanged();
}

void LightCurveDialog::setActiveImage(int imageIndex)
{
    plot_->setActive(imageIndex);
}

void LightCurveDialog::onFilterChanged()
{
    const QString filter = filterCombo_->currentText();
    const bool    allObj = (filterCombo_->currentIndex() == 0);

    // Build filtered list
    QVector<core::Observation> filtered;
    for (const auto& ob : allObs_) {
        if (ob.mag >= 90.0) continue;   // skip observations without photometry
        if (!allObj && ob.objectName != filter) continue;
        filtered.append(ob);
    }

    // Assign colours (stable: same object always gets same colour)
    QMap<QString, QColor> colorMap;
    int ci = 0;
    for (const auto& ob : allObs_) {
        if (!colorMap.contains(ob.objectName))
            colorMap[ob.objectName] = kPalette[ci++ % kPaletteSize];
    }

    plot_->setData(filtered, colorMap);
    setWindowTitle(tr("Light Curve — %1 point(s)").arg(filtered.size()));
}

void LightCurveDialog::onExportPng()
{
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export Light Curve"), QStringLiteral("light_curve.png"),
        tr("PNG Images (*.png);;All Files (*)"));
    if (path.isEmpty()) return;

    const QImage img = plot_->renderImage();
    if (!img.save(path))
        QMessageBox::critical(this, tr("Export Failed"),
                              tr("Could not write %1").arg(path));
}
