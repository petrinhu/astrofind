#include "HistogramDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPainter>
#include <QPainterPath>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace {

constexpr int kBins = 256;

// ─── HistData — pre-computed histogram + Gaussian fit ────────────────────────

struct HistData {
    std::array<float, kBins> counts{};       ///< raw linear counts per bin
    std::array<float, kBins> logNorm{};      ///< log1p-normalised [0,1] for display
    std::array<float, kBins> gaussLogNorm{}; ///< Gaussian curve in same log-norm space

    float dmin        = 0.0f;   ///< physical value of bin 0
    float dmax        = 1.0f;   ///< physical value of bin 255
    float meanPhys    = 0.0f;   ///< sigma-clipped Gaussian μ (physical)
    float sigmaPhys   = 0.0f;   ///< sigma-clipped Gaussian σ (physical)
    float medianPhys  = 0.0f;   ///< median pixel value (physical)
    int   sampledN    = 0;
    bool  fitValid    = false;  ///< false if σ < 0.5 bins
};

HistData buildHistData(const core::FitsImage& img)
{
    HistData h;
    h.dmin = img.displayMin;
    h.dmax = img.displayMax > h.dmin ? img.displayMax : h.dmin + 1.0f;

    // ── Collect counts (subsample large images) ───────────────────────────────
    const size_t step = std::max<size_t>(1, img.data.size() / 500000);
    for (size_t i = 0; i < img.data.size(); i += step) {
        const float v = img.data[i];
        if (!std::isfinite(v)) continue;
        const int bin = static_cast<int>((v - h.dmin) / (h.dmax - h.dmin) * (kBins - 1));
        if (bin >= 0 && bin < kBins) {
            h.counts[bin] += 1.0f;
            ++h.sampledN;
        }
    }

    // ── Sigma-clipped Gaussian fit (weighted moments, 3 iterations) ───────────
    // Using bin indices [0..255] as x-axis; convert to physical at the end.
    auto weightedMoments = [&](float lo, float hi, float& mu_out, float& sig_out) {
        double sumW = 0.0, sumWX = 0.0, sumWX2 = 0.0;
        for (int b = 0; b < kBins; ++b) {
            if (b < lo || b > hi) continue;
            sumW   += h.counts[b];
            sumWX  += h.counts[b] * static_cast<double>(b);
            sumWX2 += h.counts[b] * static_cast<double>(b) * b;
        }
        if (sumW <= 0.0) return;
        mu_out  = static_cast<float>(sumWX / sumW);
        const double var = sumWX2 / sumW - (sumWX / sumW) * (sumWX / sumW);
        sig_out = static_cast<float>(std::sqrt(std::max(0.0, var)));
    };

    float mu = kBins / 2.0f, sig = kBins / 4.0f;
    weightedMoments(0.0f, kBins - 1.0f, mu, sig);

    for (int iter = 0; iter < 3 && sig > 0.5f; ++iter) {
        float newMu = mu, newSig = sig;
        weightedMoments(mu - 3.0f * sig, mu + 3.0f * sig, newMu, newSig);
        mu = newMu; sig = newSig;
    }

    h.fitValid = (sig >= 0.5f);

    // ── Convert to physical units ─────────────────────────────────────────────
    const float binScale = (h.dmax - h.dmin) / (kBins - 1);
    h.meanPhys  = h.dmin + mu  * binScale;
    h.sigmaPhys = sig * binScale;

    // ── Median from CDF ───────────────────────────────────────────────────────
    float totalCounts = 0.0f;
    for (float c : h.counts) totalCounts += c;
    const float half = totalCounts * 0.5f;
    float cdf = 0.0f;
    for (int b = 0; b < kBins; ++b) {
        cdf += h.counts[b];
        if (cdf >= half) { h.medianPhys = h.dmin + b * binScale; break; }
    }

    // ── Log-normalise histogram for display ───────────────────────────────────
    float maxLog = 1.0f;
    for (int b = 0; b < kBins; ++b) {
        h.logNorm[b] = std::log1p(h.counts[b]);
        maxLog = std::max(maxLog, h.logNorm[b]);
    }
    for (int b = 0; b < kBins; ++b) h.logNorm[b] /= maxLog;

    // ── Gaussian curve in same log-norm space ─────────────────────────────────
    // Build synthetic counts from the Gaussian, then apply the same transform.
    if (h.fitValid) {
        const int peakBin = std::clamp(static_cast<int>(std::round(mu)), 0, kBins - 1);
        const float peakCount = h.counts[peakBin];

        for (int b = 0; b < kBins; ++b) {
            const float t = (b - mu) / sig;
            const float gaussCount = peakCount * std::exp(-0.5f * t * t);
            h.gaussLogNorm[b] = std::log1p(gaussCount) / maxLog;
        }
    }

    return h;
}

// ─── HistWidget ───────────────────────────────────────────────────────────────

class HistWidget : public QWidget {
public:
    explicit HistWidget(HistData d, QWidget* parent = nullptr)
        : QWidget(parent), d_(std::move(d))
    {
        setMinimumSize(380, 160);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        const int W = width(), H = height();
        constexpr int padL = 4, padR = 4, padT = 8, padB = 4;
        const int cW = W - padL - padR;
        const int cH = H - padT - padB;

        p.fillRect(rect(), QColor(0x0d, 0x11, 0x17));

        if (cW <= 0 || cH <= 0) return;

        // ── Histogram bars ────────────────────────────────────────────────────
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x1f, 0x6f, 0xeb, 180));
        for (int b = 0; b < kBins; ++b) {
            const int x  = padL + b * cW / kBins;
            const int bw = std::max(1, cW / kBins);
            const int bh = static_cast<int>(d_.logNorm[b] * cH);
            p.drawRect(x, padT + cH - bh, bw, bh);
        }

        // ── Gaussian fit curve ────────────────────────────────────────────────
        if (d_.fitValid) {
            QPainterPath path;
            bool started = false;
            for (int b = 0; b < kBins; ++b) {
                const float xf = padL + static_cast<float>(b) * cW / kBins + 0.5f * cW / kBins;
                const float yf = padT + cH - d_.gaussLogNorm[b] * cH;
                if (!started) { path.moveTo(xf, yf); started = true; }
                else          { path.lineTo(xf, yf); }
            }
            p.setPen(QPen(QColor(0xff, 0xd0, 0x50), 1.8));
            p.setBrush(Qt::NoBrush);
            p.drawPath(path);

            // μ vertical line (dashed, amber)
            const float muX = padL + ((d_.meanPhys - d_.dmin) / (d_.dmax - d_.dmin)) * cW;
            p.setPen(QPen(QColor(0xff, 0xd0, 0x50, 180), 1, Qt::DashLine));
            p.drawLine(QPointF(muX, padT), QPointF(muX, padT + cH));

            // μ ± σ bands (very faint fill)
            const float sigFrac = d_.sigmaPhys / (d_.dmax - d_.dmin);
            const float sigPx   = sigFrac * cW;
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(0xff, 0xd0, 0x50, 18));
            p.drawRect(QRectF(muX - sigPx, padT, sigPx * 2.0f, cH));
        }

        // ── Display-range edge markers ────────────────────────────────────────
        // Left edge = dmin (always x=padL), right edge = dmax (always x=padL+cW-1)
        p.setBrush(Qt::NoBrush);
        p.setPen(QPen(QColor(0xf8, 0x51, 0x49, 180), 1));
        p.drawLine(padL, padT, padL, padT + cH);
        p.setPen(QPen(QColor(0x3f, 0xb9, 0x50, 180), 1));
        p.drawLine(padL + cW - 1, padT, padL + cW - 1, padT + cH);

        // ── Axis labels ───────────────────────────────────────────────────────
        p.setPen(QColor(0x66, 0x77, 0x88));
        const QFont small(QStringLiteral("monospace"), 7);
        p.setFont(small);
        const auto fmtN = [](float v) -> QString {
            if (std::abs(v) < 1e4f && std::abs(v) >= 0.01f)
                return QString::number(static_cast<double>(v), 'f', 1);
            return QString::number(static_cast<double>(v), 'g', 3);
        };
        p.drawText(QPoint(padL + 2, padT + cH - 2), fmtN(d_.dmin));
        const QString maxStr = fmtN(d_.dmax);
        const int maxW = p.fontMetrics().horizontalAdvance(maxStr);
        p.drawText(QPoint(padL + cW - maxW - 2, padT + cH - 2), maxStr);
    }

private:
    HistData d_;
};

} // anonymous namespace

// ─── HistogramDialog ─────────────────────────────────────────────────────────

HistogramDialog::HistogramDialog(const core::FitsImage& img, QWidget* parent)
    : QDialog(parent, Qt::Tool)
{
    setWindowTitle(tr("Histogram — %1").arg(img.fileName));
    resize(460, 310);
    setAttribute(Qt::WA_DeleteOnClose);

    const HistData d = buildHistData(img);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(8, 8, 8, 8);
    lay->setSpacing(6);

    lay->addWidget(new HistWidget(d, this), 1);

    // ── Statistics row ────────────────────────────────────────────────────────
    auto* statsRow = new QHBoxLayout;
    statsRow->setSpacing(0);

    auto addStat = [&](const QString& lbl, const QString& val, const QString& color = {}) {
        auto* col = new QVBoxLayout;
        col->setSpacing(1);
        col->setContentsMargins(8, 2, 8, 2);
        auto* l = new QLabel(lbl, this);
        l->setAlignment(Qt::AlignCenter);
        l->setStyleSheet(QStringLiteral("color:#667788;font-size:10px;"));
        auto* v = new QLabel(val, this);
        v->setAlignment(Qt::AlignCenter);
        const QString style = color.isEmpty()
            ? QStringLiteral("font-weight:bold;font-size:11px;")
            : QStringLiteral("font-weight:bold;font-size:11px;color:%1;").arg(color);
        v->setStyleSheet(style);
        col->addWidget(l); col->addWidget(v);
        statsRow->addLayout(col);
        statsRow->addWidget([&]() -> QFrame* {
            auto* sep = new QFrame(this);
            sep->setFrameShape(QFrame::VLine);
            sep->setStyleSheet(QStringLiteral("color:#222;"));
            return sep;
        }());
    };

    const auto fmt = [](float v) -> QString {
        if (std::abs(v) < 1e5f && std::abs(v) >= 0.001f)
            return QString::number(static_cast<double>(v), 'f', 2);
        return QString::number(static_cast<double>(v), 'g', 4);
    };

    if (d.fitValid) {
        addStat(tr("μ (sky)"),    fmt(d.meanPhys),   QStringLiteral("#ffd050"));
        addStat(tr("σ (noise)"),  fmt(d.sigmaPhys),  QStringLiteral("#ffd050"));
    } else {
        addStat(tr("μ (sky)"),    tr("—"), {});
        addStat(tr("σ (noise)"),  tr("—"), {});
    }
    addStat(tr("Mediana"),    fmt(d.medianPhys), {});
    addStat(tr("Mín display"), fmt(d.dmin),       QStringLiteral("#f85149"));
    addStat(tr("Máx display"), fmt(d.dmax),       QStringLiteral("#3fb950"));
    addStat(tr("Amostras"),   QString::number(d.sampledN), {});

    lay->addLayout(statsRow);

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Close, this);
    lay->addWidget(bb);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::accept);
}
