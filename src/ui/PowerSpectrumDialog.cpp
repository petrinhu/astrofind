#include "PowerSpectrumDialog.h"

#include <fftw3.h>

#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QMouseEvent>
#include <QToolTip>

#include <algorithm>
#include <cmath>
#include <vector>

// ─── Internal widget ─────────────────────────────────────────────────────────

namespace {

/// Compute the 2-D log-power spectrum and return it as a centered grayscale
/// QImage.  Input pixel data is taken from img.data (luminance plane).
/// cW/cH are the FFT compute dimensions (≤ img.width/height).
QImage computeSpectrum(const core::FitsImage& img, int cW, int cH)
{
    const int W = img.width;

    // ── Apply 2-D Hann window and copy to double buffer ───────────────────
    std::vector<double> in(static_cast<size_t>(cW * cH));
    for (int y = 0; y < cH; ++y) {
        const double wy = 0.5 * (1.0 - std::cos(2.0 * M_PI * y / (cH - 1)));
        const int srcY = y * img.height / cH;
        for (int x = 0; x < cW; ++x) {
            const double wx = 0.5 * (1.0 - std::cos(2.0 * M_PI * x / (cW - 1)));
            const int srcX = x * W / cW;
            in[static_cast<size_t>(y * cW + x)] =
                static_cast<double>(img.data[static_cast<size_t>(srcY * W + srcX)]) * wx * wy;
        }
    }

    // ── Forward real-to-complex FFT ────────────────────────────────────────
    const int outCols = cW / 2 + 1;
    std::vector<fftw_complex> out(static_cast<size_t>(cH * outCols));
    fftw_plan plan = fftw_plan_dft_r2c_2d(cH, cW, in.data(), out.data(), FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    // ── Log power spectrum: full W×H using Hermitian symmetry ─────────────
    // r2c output: out[u * outCols + v] for v in [0, outCols).
    // Negative-frequency half: F(H-u mod H, W-v) = conj(F(u,v)).
    std::vector<double> logPow(static_cast<size_t>(cH * cW));
    double maxLP = 0.0;
    for (int u = 0; u < cH; ++u) {
        for (int v = 0; v < cW; ++v) {
            double re, im;
            if (v <= cW / 2) {
                const fftw_complex& c = out[static_cast<size_t>(u * outCols + v)];
                re = c[0];  im = c[1];
            } else {
                // Hermitian conjugate at (H-u mod H, W-v)
                const int cu = (cH - u) % cH;
                const int cv = cW - v;          // cv ∈ [1, W/2]
                const fftw_complex& c = out[static_cast<size_t>(cu * outCols + cv)];
                re = c[0];  im = -c[1];
            }
            const double lp = std::log1p(re * re + im * im);
            logPow[static_cast<size_t>(u * cW + v)] = lp;
            maxLP = std::max(maxLP, lp);
        }
    }

    // ── Render: fftshift + normalise → 8-bit grayscale ────────────────────
    // fftshift moves DC from (0,0) to (cH/2, cW/2).
    QImage out_img(cW, cH, QImage::Format_Grayscale8);
    for (int y = 0; y < cH; ++y) {
        const int sy = (y + cH / 2) % cH;
        uchar* line = out_img.scanLine(y);
        for (int x = 0; x < cW; ++x) {
            const int sx = (x + cW / 2) % cW;
            const double norm = maxLP > 0.0
                ? logPow[static_cast<size_t>(sy * cW + sx)] / maxLP
                : 0.0;
            line[x] = static_cast<uchar>(std::clamp(static_cast<int>(norm * 255.0), 0, 255));
        }
    }
    return out_img;
}

// ─── PsWidget ─────────────────────────────────────────────────────────────────

class PsWidget : public QWidget {
public:
    explicit PsWidget(const core::FitsImage& img, QWidget* parent = nullptr)
        : QWidget(parent)
        , imgW_(img.width), imgH_(img.height)
    {
        // Cap compute size at 1024 for speed; fftw3 is fast but 4K FFT takes ~300 ms
        const int cW = std::min(img.width,  1024);
        const int cH = std::min(img.height, 1024);
        spectrum_ = computeSpectrum(img, cW, cH);

        setMinimumSize(256, 256);
        setMouseTracking(true);

        // Pixel scale for frequency annotation: 1/image_dimension cycles/pixel
        freqScaleX_ = (img.pixScaleX > 0.0) ? img.pixScaleX : 0.0;  // arcsec/pix
        freqScaleY_ = (img.pixScaleY > 0.0) ? img.pixScaleY : 0.0;
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter p(this);
        p.setRenderHint(QPainter::SmoothPixmapTransform, false);

        // Stretch spectrum to fill widget, preserving aspect ratio
        const QSize sz = spectrum_.size().scaled(size(), Qt::KeepAspectRatio);
        const QRect dst((width()  - sz.width())  / 2,
                        (height() - sz.height()) / 2,
                        sz.width(), sz.height());
        p.drawImage(dst, spectrum_);

        // Draw cross-hair at DC centre
        const QPointF dc(dst.left() + sz.width()  / 2.0,
                         dst.top()  + sz.height() / 2.0);
        p.setPen(QPen(QColor(255, 80, 80, 180), 1, Qt::DashLine));
        p.drawLine(QPointF(dst.left(),  dc.y()), QPointF(dst.right(), dc.y()));
        p.drawLine(QPointF(dc.x(), dst.top()),   QPointF(dc.x(), dst.bottom()));

        drawRect_ = dst;
    }

    void mouseMoveEvent(QMouseEvent* ev) override
    {
        if (drawRect_.isEmpty() || !drawRect_.contains(ev->pos())) return;

        const int cW = spectrum_.width();
        const int cH = spectrum_.height();

        // Pixel position in the displayed spectrum image
        const double sx = static_cast<double>(ev->pos().x() - drawRect_.left())
                          * cW / drawRect_.width();
        const double sy = static_cast<double>(ev->pos().y() - drawRect_.top())
                          * cH / drawRect_.height();

        // Frequency after fftshift: fc = (s - N/2) / N cycles/pixel
        const double fx = (sx - cW / 2.0) / cW;  // cycles/pixel
        const double fy = (sy - cH / 2.0) / cH;

        QString tip;
        if (freqScaleX_ > 0.0 && freqScaleY_ > 0.0) {
            // Period in arcseconds: T = 1/|f| * pixScale
            const double periodX = (std::abs(fx) > 1e-6) ? std::abs(1.0 / fx) * freqScaleX_ : 0.0;
            const double periodY = (std::abs(fy) > 1e-6) ? std::abs(1.0 / fy) * freqScaleY_ : 0.0;
            tip = QStringLiteral("fx=%1 cyc/px  Tx=%2\"  |  fy=%3 cyc/px  Ty=%4\"")
                .arg(fx, 0, 'f', 4).arg(periodX, 0, 'f', 1)
                .arg(fy, 0, 'f', 4).arg(periodY, 0, 'f', 1);
        } else {
            tip = QStringLiteral("fx=%1 cyc/px  |  fy=%2 cyc/px")
                .arg(fx, 0, 'f', 4).arg(fy, 0, 'f', 4);
        }
        QToolTip::showText(ev->globalPosition().toPoint(), tip, this);
    }

private:
    QImage  spectrum_;
    QRect   drawRect_;
    int     imgW_ = 0, imgH_ = 0;
    double  freqScaleX_ = 0.0, freqScaleY_ = 0.0;
};

}  // anonymous namespace

// ─── PowerSpectrumDialog ─────────────────────────────────────────────────────

PowerSpectrumDialog::PowerSpectrumDialog(const core::FitsImage& img, QWidget* parent)
    : QDialog(parent, Qt::Tool)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(tr("Power Spectrum — %1").arg(img.fileName));

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(6, 6, 6, 6);
    lay->setSpacing(4);

    // ── Spectrum widget ────────────────────────────────────────────────────
    auto* ps = new PsWidget(img, this);
    ps->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lay->addWidget(ps);

    // ── Info bar ───────────────────────────────────────────────────────────
    const int cW = std::min(img.width,  1024);
    const int cH = std::min(img.height, 1024);
    const bool downsampled = (cW < img.width || cH < img.height);

    QString info = QStringLiteral("Image: %1×%2 px   FFT: %3×%4")
        .arg(img.width).arg(img.height).arg(cW).arg(cH);
    if (downsampled)
        info += tr("  (downsampled for speed)");
    if (img.pixScaleX > 0.0)
        info += QStringLiteral("   Scale: %1\"/px").arg(img.pixScaleX, 0, 'f', 2);
    info += tr("   Hover for frequency");

    auto* infoLabel = new QLabel(info, this);
    infoLabel->setStyleSheet(QStringLiteral(
        "color:#aaaaaa; font-size:11px; padding:2px 4px;"));
    infoLabel->setAlignment(Qt::AlignCenter);
    lay->addWidget(infoLabel);

    resize(480, 500);
}
