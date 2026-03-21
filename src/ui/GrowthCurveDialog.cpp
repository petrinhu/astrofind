#include "GrowthCurveDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QPushButton>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>
#include <QTimer>
#include <QToolTip>
#include <cmath>
#include <algorithm>

// ─── Plot widget ──────────────────────────────────────────────────────────────

class GrowthCurveDialog::Plot : public QWidget {
    Q_OBJECT
public:
    explicit Plot(QWidget* parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
        setMinimumSize(380, 220);
    }

    void setData(const QVector<core::GrowthPoint>& pts,
                 double optimalRadius,
                 double fwhmPx)
    {
        pts_           = pts;
        optimalRadius_ = optimalRadius;
        fwhmPx_        = fwhmPx;
        hovered_       = -1;
        update();
    }

    double optimalRadius() const { return optimalRadius_; }

signals:
    void pointHovered(const QString& info);

protected:
    void paintEvent(QPaintEvent*) override {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        p.fillRect(rect(), QColor(13, 17, 23));
        draw(p);
    }

    void mouseMoveEvent(QMouseEvent* ev) override {
        const int prev = hovered_;
        hovered_ = hitTest(ev->pos());
        if (hovered_ != prev) {
            update();
            if (hovered_ >= 0) {
                const auto& pt = pts_[hovered_];
                emit pointHovered(
                    tr("r=%1 px  flux=%2  frac=%3  mag=%4±%5")
                        .arg(pt.radius,   0, 'f', 1)
                        .arg(pt.flux,     0, 'f', 0)
                        .arg(pt.fluxFrac, 0, 'f', 3)
                        .arg(pt.magInst,  0, 'f', 2)
                        .arg(pt.magErr,   0, 'f', 3));
            } else {
                emit pointHovered(QString());
            }
        }
    }

    void leaveEvent(QEvent*) override {
        hovered_ = -1;
        update();
        emit pointHovered(QString());
    }

    void resizeEvent(QResizeEvent*) override { update(); }

private:
    static constexpr int kM = 38;  // margin px

    QRectF plotRect() const {
        return QRectF(kM, 10, width() - kM - 12, height() - kM - 10);
    }

    QPointF toScreen(const QRectF& pr, double r, double frac) const {
        if (pts_.isEmpty()) return {};
        const double rMin  = pts_.first().radius;
        const double rMax  = pts_.last().radius;
        const double rSpan = rMax - rMin;
        const double px = (rSpan > 0) ? pr.left() + (r - rMin) / rSpan * pr.width()  : pr.center().x();
        const double py = pr.bottom() - frac * pr.height();
        return QPointF(px, py);
    }

    int hitTest(const QPoint& pos) const {
        if (pts_.isEmpty()) return -1;
        const QRectF pr = plotRect();
        for (int i = 0; i < pts_.size(); ++i) {
            const QPointF sp = toScreen(pr, pts_[i].radius, pts_[i].fluxFrac);
            if (QPointF(pos - sp).manhattanLength() < 8.0) return i;
        }
        return -1;
    }

    void draw(QPainter& p) const {
        const QRectF pr = plotRect();

        // Grid
        p.setPen(QPen(QColor(33, 38, 45), 1));
        for (int i = 0; i <= 4; ++i) {
            const double y = pr.bottom() - i * pr.height() / 4.0;
            p.drawLine(QPointF(pr.left(), y), QPointF(pr.right(), y));
        }

        // Axes
        p.setPen(QPen(QColor(139, 148, 158), 1));
        p.drawLine(QPointF(pr.left(),  pr.top()),    QPointF(pr.left(),  pr.bottom()));
        p.drawLine(QPointF(pr.left(),  pr.bottom()), QPointF(pr.right(), pr.bottom()));

        // Y axis labels (0.0, 0.25, 0.5, 0.75, 1.0)
        p.setFont(QFont(QStringLiteral("sans-serif"), 7));
        p.setPen(QColor(139, 148, 158));
        for (int i = 0; i <= 4; ++i) {
            const double frac = i * 0.25;
            const double y    = pr.bottom() - frac * pr.height();
            p.drawText(QRectF(0, y - 7, kM - 4, 14),
                       Qt::AlignRight | Qt::AlignVCenter,
                       QString::number(frac, 'f', 2));
        }

        // X axis labels
        if (!pts_.isEmpty()) {
            const double rMin = pts_.first().radius;
            const double rMax = pts_.last().radius;
            for (int i = 0; i <= 4; ++i) {
                const double r = rMin + i * (rMax - rMin) / 4.0;
                const QPointF sp = toScreen(pr, r, 0.0);
                p.drawText(QRectF(sp.x() - 16, pr.bottom() + 3, 32, 14),
                           Qt::AlignHCenter | Qt::AlignTop,
                           QString::number(r, 'f', 1));
            }
            // X axis title
            p.drawText(QRectF(pr.left(), pr.bottom() + 18, pr.width(), 14),
                       Qt::AlignCenter, tr("Aperture radius (px)"));
        }

        // Y axis title (rotated)
        p.save();
        p.translate(8, pr.center().y());
        p.rotate(-90);
        p.drawText(QRectF(-50, -8, 100, 14), Qt::AlignCenter, tr("Flux fraction"));
        p.restore();

        if (pts_.isEmpty()) return;

        // FWHM-based aperture suggestion line (cyan dashed)
        if (fwhmPx_ > 0.0) {
            const QPointF sp = toScreen(pr, fwhmPx_, 1.0);
            p.setPen(QPen(QColor(55, 210, 190), 1, Qt::DashLine));
            p.drawLine(QPointF(sp.x(), pr.top()), QPointF(sp.x(), pr.bottom()));
            p.setPen(QColor(55, 210, 190));
            p.setFont(QFont(QStringLiteral("sans-serif"), 7));
            p.drawText(QRectF(sp.x() + 2, pr.top(), 40, 12), Qt::AlignLeft,
                       tr("2×FWHM"));
        }

        // Optimal aperture line (amber dashed)
        if (optimalRadius_ > 0.0) {
            const QPointF sp = toScreen(pr, optimalRadius_, 1.0);
            p.setPen(QPen(QColor(255, 200, 55), 1, Qt::DashLine));
            p.drawLine(QPointF(sp.x(), pr.top()), QPointF(sp.x(), pr.bottom()));
            p.setPen(QColor(255, 200, 55));
            p.setFont(QFont(QStringLiteral("sans-serif"), 7));
            p.drawText(QRectF(sp.x() + 2, pr.top() + 12, 40, 12), Qt::AlignLeft,
                       tr("opt"));
        }

        // Growth curve line
        p.setPen(QPen(QColor(85, 170, 255), 2));
        for (int i = 1; i < pts_.size(); ++i) {
            p.drawLine(toScreen(pr, pts_[i-1].radius, pts_[i-1].fluxFrac),
                       toScreen(pr, pts_[i].radius,   pts_[i].fluxFrac));
        }

        // Data points
        for (int i = 0; i < pts_.size(); ++i) {
            const QPointF sp = toScreen(pr, pts_[i].radius, pts_[i].fluxFrac);
            if (i == hovered_) {
                p.setPen(QPen(Qt::white, 1));
                p.setBrush(QColor(255, 255, 100));
                p.drawEllipse(sp, 5.0, 5.0);
                QToolTip::showText(mapToGlobal(sp.toPoint()),
                    tr("r=%1 px\nflux frac=%2\nmag=%3±%4")
                        .arg(pts_[i].radius,   0, 'f', 1)
                        .arg(pts_[i].fluxFrac, 0, 'f', 3)
                        .arg(pts_[i].magInst,  0, 'f', 2)
                        .arg(pts_[i].magErr,   0, 'f', 3));
            } else {
                p.setPen(QPen(QColor(85, 170, 255), 1));
                p.setBrush(QColor(13, 17, 23));
                p.drawEllipse(sp, 3.5, 3.5);
            }
        }
    }

    QVector<core::GrowthPoint> pts_;
    double optimalRadius_ = 0.0;
    double fwhmPx_        = 0.0;
    int    hovered_       = -1;
};

#include "GrowthCurveDialog.moc"

// ─── Helpers ──────────────────────────────────────────────────────────────────

static double findOptimalAperture(const QVector<core::GrowthPoint>& pts)
{
    if (pts.size() < 2) return pts.isEmpty() ? 0.0 : pts.first().radius;
    // Find first radius where incremental gain drops below 2% of total flux
    for (int i = 1; i < pts.size(); ++i) {
        if ((pts[i].fluxFrac - pts[i-1].fluxFrac) < 0.02)
            return pts[i-1].radius;
    }
    return pts.last().radius;
}

// ─── GrowthCurveDialog ───────────────────────────────────────────────────────

GrowthCurveDialog::GrowthCurveDialog(const core::FitsImage& img,
                                       QSettings& settings,
                                       QWidget* parent)
    : QDialog(parent)
    , img_(img)
    , settings_(settings)
{
    setWindowTitle(tr("Aperture Growth Curve — %1").arg(img.fileName));
    setMinimumSize(560, 560);

    // ── Star selector ─────────────────────────────────────────────────────
    starCombo_ = new QComboBox(this);
    if (img_.detectedStars.isEmpty()) {
        starCombo_->addItem(tr("Image centre (no stars detected)"));
        starCombo_->setEnabled(false);
    } else {
        // Sort by flux descending so the brightest star is first
        QVector<int> idx(img_.detectedStars.size());
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(), [&](int a, int b) {
            return img_.detectedStars[a].flux > img_.detectedStars[b].flux;
        });
        for (int i : idx) {
            const auto& s = img_.detectedStars[i];
            starCombo_->addItem(
                tr("Star %1  (x=%2, y=%3, SNR=%4)")
                    .arg(i + 1)
                    .arg(s.x, 0, 'f', 1)
                    .arg(s.y, 0, 'f', 1)
                    .arg(s.snr, 0, 'f', 1),
                i);
        }
    }

    // ── Radii controls ────────────────────────────────────────────────────
    const double defaultRMax = [&]() -> double {
        if (!img_.detectedStars.isEmpty()) {
            // Suggest ~4× FWHM of brightest star
            const auto& s = img_.detectedStars[0];
            return std::max(15.0, std::min(40.0, 4.0 * s.fwhm()));
        }
        return 20.0;
    }();

    rMinSpin_ = new QDoubleSpinBox(this);
    rMinSpin_->setRange(1.0, 50.0);
    rMinSpin_->setValue(1.0);
    rMinSpin_->setSuffix(tr(" px"));

    rMaxSpin_ = new QDoubleSpinBox(this);
    rMaxSpin_->setRange(2.0, 100.0);
    rMaxSpin_->setValue(defaultRMax);
    rMaxSpin_->setSuffix(tr(" px"));

    nStepsSpin_ = new QSpinBox(this);
    nStepsSpin_->setRange(4, 50);
    nStepsSpin_->setValue(15);

    auto* form = new QFormLayout;
    form->addRow(tr("Star:"),       starCombo_);
    form->addRow(tr("Min radius:"), rMinSpin_);
    form->addRow(tr("Max radius:"), rMaxSpin_);
    form->addRow(tr("Steps:"),      nStepsSpin_);

    // ── Plot ──────────────────────────────────────────────────────────────
    plot_ = new Plot(this);

    statusLabel_ = new QLabel(this);
    statusLabel_->setWordWrap(true);
    connect(plot_, &Plot::pointHovered, statusLabel_, &QLabel::setText);

    // ── Table ─────────────────────────────────────────────────────────────
    table_ = new QTableWidget(0, 5, this);
    table_->setHorizontalHeaderLabels({
        tr("Radius (px)"), tr("Pixels"), tr("Flux (ADU)"),
        tr("Flux frac."),  tr("Mag_inst")
    });
    table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setMaximumHeight(160);
    table_->setAlternatingRowColors(true);

    // ── Buttons ───────────────────────────────────────────────────────────
    computeBtn_  = new QPushButton(tr("Compute"), this);
    computeBtn_->setDefault(true);
    useApertBtn_ = new QPushButton(tr("Use optimal aperture"), this);
    useApertBtn_->setEnabled(false);
    useApertBtn_->setToolTip(
        tr("Set this as the manual aperture radius in Settings"));
    auto* closeBtn = new QPushButton(tr("Close"), this);

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(computeBtn_);
    btnRow->addWidget(useApertBtn_);
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);

    // ── Layout ────────────────────────────────────────────────────────────
    auto* controlBox = new QGroupBox(tr("Parameters"), this);
    controlBox->setLayout(form);

    auto* lay = new QVBoxLayout(this);
    lay->addWidget(controlBox);
    lay->addWidget(plot_, 1);
    lay->addWidget(statusLabel_);
    lay->addWidget(table_);
    lay->addLayout(btnRow);

    connect(computeBtn_,  &QPushButton::clicked, this, &GrowthCurveDialog::onCompute);
    connect(useApertBtn_, &QPushButton::clicked, this, &GrowthCurveDialog::onUseAperture);
    connect(closeBtn,     &QPushButton::clicked, this, &QDialog::reject);

    // Auto-compute on open
    QTimer::singleShot(0, this, &GrowthCurveDialog::onCompute);
}

void GrowthCurveDialog::onCompute()
{
    // Determine centroid
    double cx = img_.width  * 0.5;
    double cy = img_.height * 0.5;
    double fwhmPx = 0.0;

    if (!img_.detectedStars.isEmpty()) {
        const int idx = starCombo_->currentData().toInt();
        if (idx >= 0 && idx < img_.detectedStars.size()) {
            cx     = img_.detectedStars[idx].x;
            cy     = img_.detectedStars[idx].y;
            fwhmPx = img_.detectedStars[idx].fwhm() * 2.0; // 2×FWHM suggestion line
        }
    }

    const double rMin = rMinSpin_->value();
    const double rMax = rMaxSpin_->value();
    const int    n    = nStepsSpin_->value();

    if (rMin >= rMax) {
        statusLabel_->setText(tr("<span style='color:#e05050;'>Min radius must be less than max radius.</span>"));
        return;
    }

    // Build evenly-spaced radii
    QVector<double> radii;
    radii.reserve(n);
    for (int i = 0; i < n; ++i)
        radii.append(rMin + i * (rMax - rMin) / (n - 1));

    const QVector<core::GrowthPoint> pts =
        core::multiAperturePhotometry(img_, cx, cy, radii);

    if (pts.isEmpty()) {
        statusLabel_->setText(tr("<span style='color:#e05050;'>No valid measurements — star may be too close to the image edge.</span>"));
        return;
    }

    optimalRadius_ = findOptimalAperture(pts);
    plot_->setData(pts, optimalRadius_, fwhmPx);
    populateTable(pts);
    useApertBtn_->setEnabled(true);
    useApertBtn_->setText(tr("Use optimal aperture (%1 px)")
                              .arg(optimalRadius_, 0, 'f', 1));
    statusLabel_->setText(
        tr("Optimal aperture: <b>%1 px</b> — hover over points for details")
            .arg(optimalRadius_, 0, 'f', 1));
}

void GrowthCurveDialog::onUseAperture()
{
    settings_.setValue(QStringLiteral("photometry/apertureManPx"), optimalRadius_);
    settings_.setValue(QStringLiteral("photometry/apertureAuto"),  false);
    statusLabel_->setText(
        tr("<span style='color:#80cc80;'>✓ Aperture set to %1 px in Settings "
           "(manual mode enabled).</span>")
            .arg(optimalRadius_, 0, 'f', 1));
}

void GrowthCurveDialog::populateTable(const QVector<core::GrowthPoint>& pts)
{
    table_->setRowCount(pts.size());
    for (int i = 0; i < pts.size(); ++i) {
        const auto& pt = pts[i];
        auto item = [](const QString& s) {
            auto* it = new QTableWidgetItem(s);
            it->setTextAlignment(Qt::AlignCenter);
            return it;
        };
        table_->setItem(i, 0, item(QString::number(pt.radius,   'f', 1)));
        table_->setItem(i, 1, item(QString::number(pt.nPix)));
        table_->setItem(i, 2, item(QString::number(pt.flux,     'f', 1)));
        table_->setItem(i, 3, item(QString::number(pt.fluxFrac, 'f', 4)));
        table_->setItem(i, 4, item(pt.magInst < 90.0
                                       ? QString::number(pt.magInst, 'f', 3)
                                       : QStringLiteral("—")));

        // Highlight row nearest optimal aperture
        if (std::abs(pt.radius - optimalRadius_) < 0.5) {
            for (int c = 0; c < 5; ++c) {
                if (table_->item(i, c))
                    table_->item(i, c)->setBackground(QColor(40, 55, 30));
            }
        }
    }
}
