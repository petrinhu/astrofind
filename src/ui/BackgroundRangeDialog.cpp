#include "BackgroundRangeDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPainter>
#include <QPaintEvent>
#include <QLabel>
#include <QPushButton>
#include <algorithm>
#include <cmath>

BackgroundRangeDialog::BackgroundRangeDialog(FitsImageView* view,
                                              core::FitsImage* image,
                                              QWidget* parent)
    : QDialog(parent)
    , view_(view)
    , image_(image)
    , origMin_(image->displayMin)
    , origMax_(image->displayMax)
    , origMode_(image->stretchMode)
    , origLut_(image->colorLut)
{
    setWindowTitle(tr("Background and Range \xe2\x80\x94 %1").arg(image_->fileName));
    setModal(false);
    setMinimumWidth(440);

    // Compute data range from pixel data
    if (!image_->data.empty()) {
        dataMin_ = *std::min_element(image_->data.begin(), image_->data.end());
        dataMax_ = *std::max_element(image_->data.begin(), image_->data.end());
    }
    if (dataMax_ <= dataMin_) dataMax_ = dataMin_ + 1.0f;

    buildHistogram();

    auto* root = new QVBoxLayout(this);
    root->setSpacing(10);
    root->setContentsMargins(12, 12, 12, 12);

    // ── Histogram widget ───────────────────────────────────────────────────
    histWidget_ = new QWidget(this);
    histWidget_->setFixedHeight(80);
    histWidget_->setMinimumWidth(400);
    histWidget_->setAttribute(Qt::WA_OpaquePaintEvent);
    histWidget_->installEventFilter(this);

    // ── Sliders ────────────────────────────────────────────────────────────
    constexpr int kSliderRange = 10000;

    auto makeSlider = [&](float val) {
        auto* s = new QSlider(Qt::Horizontal, this);
        s->setRange(0, kSliderRange);
        s->setValue(static_cast<int>((val - dataMin_) / (dataMax_ - dataMin_) * kSliderRange));
        return s;
    };

    sldMin_ = makeSlider(image_->displayMin);
    sldMax_ = makeSlider(image_->displayMax);

    lblMin_ = new QLabel(QString::number(image_->displayMin, 'g', 6), this);
    lblMax_ = new QLabel(QString::number(image_->displayMax, 'g', 6), this);
    lblMin_->setMinimumWidth(90);
    lblMax_->setMinimumWidth(90);
    lblMin_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    lblMax_->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto* grid = new QGridLayout;
    grid->setHorizontalSpacing(8);
    grid->setVerticalSpacing(6);
    grid->addWidget(new QLabel(tr("Background (min):"), this), 0, 0);
    grid->addWidget(sldMin_, 0, 1);
    grid->addWidget(lblMin_, 0, 2);
    grid->addWidget(new QLabel(tr("Range (max):"), this),      1, 0);
    grid->addWidget(sldMax_, 1, 1);
    grid->addWidget(lblMax_, 1, 2);

    // ── Transfer function ──────────────────────────────────────────────────
    modeCombo_ = new QComboBox(this);
    modeCombo_->addItem(tr("Linear"),                  static_cast<int>(core::StretchMode::Linear));
    modeCombo_->addItem(tr("Logarítmica"),             static_cast<int>(core::StretchMode::Log));
    modeCombo_->addItem(tr("Raiz Quadrada (Sqrt)"),    static_cast<int>(core::StretchMode::Sqrt));
    modeCombo_->addItem(tr("Asinh"),                   static_cast<int>(core::StretchMode::Asinh));
    modeCombo_->addItem(tr("Equalização de Histograma"), static_cast<int>(core::StretchMode::HistEq));
    modeCombo_->setToolTip(tr(
        "Transfer function applied to pixel values before display.\n"
        "Linear: direct mapping.\n"
        "Log / Sqrt / Asinh: compress bright highlights, reveal faint detail.\n"
        "HistEq: maximises local contrast (slow on large images)."));
    // Initialise combo from current image mode
    {
        const int idx = modeCombo_->findData(static_cast<int>(image_->stretchMode));
        modeCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
    }
    connect(modeCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
        [this](int) {
            const auto mode = static_cast<core::StretchMode>(modeCombo_->currentData().toInt());
            image_->stretchMode = mode;
            view_->setStretchMode(mode);
        });

    auto* modeRow = new QHBoxLayout;
    modeRow->addWidget(new QLabel(tr("Função de transferência:"), this));
    modeRow->addWidget(modeCombo_, 1);

    // ── False-colour LUT ───────────────────────────────────────────────────
    lutCombo_ = new QComboBox(this);
    lutCombo_->addItem(tr("Cinza (Grayscale)"),  static_cast<int>(core::ColorLut::Grayscale));
    lutCombo_->addItem(tr("Quente (Hot)"),        static_cast<int>(core::ColorLut::Hot));
    lutCombo_->addItem(tr("Frio (Cool)"),         static_cast<int>(core::ColorLut::Cool));
    lutCombo_->addItem(tr("Viridis"),              static_cast<int>(core::ColorLut::Viridis));
    lutCombo_->setToolTip(tr(
        "False-colour look-up table applied after the transfer function.\n"
        "Grayscale: standard black-to-white.\n"
        "Quente: black → red → yellow → white (thermal).\n"
        "Frio: black → dark blue → cyan → white.\n"
        "Viridis: perceptually uniform purple → teal → yellow."));
    lutCombo_->setEnabled(!image_->isColor);   // LUT is meaningless for RGB images
    {
        const int idx = lutCombo_->findData(static_cast<int>(image_->colorLut));
        lutCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
    }
    connect(lutCombo_, qOverload<int>(&QComboBox::currentIndexChanged), this,
        [this](int) {
            const auto lut = static_cast<core::ColorLut>(lutCombo_->currentData().toInt());
            image_->colorLut = lut;
            view_->setColorLut(lut);
        });

    auto* lutRow = new QHBoxLayout;
    lutRow->addWidget(new QLabel(tr("Cor falsa (LUT):"), this));
    lutRow->addWidget(lutCombo_, 1);

    auto* autoBtn = new QPushButton(tr("\xe2\x9f\xb3 Auto"), this);
    autoBtn->setFixedWidth(80);
    connect(autoBtn, &QPushButton::clicked, this, &BackgroundRangeDialog::onAutoClicked);

    auto* hRow = new QHBoxLayout;
    hRow->addStretch();
    hRow->addWidget(autoBtn);

    chkApplyAll_ = new QCheckBox(tr("Apply to all images in session"), this);

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bb, &QDialogButtonBox::accepted, this, &BackgroundRangeDialog::onApplyClicked);
    connect(bb, &QDialogButtonBox::rejected, this, &BackgroundRangeDialog::onRejectClicked);

    root->addWidget(new QLabel(tr("<b>Histogram</b>"), this));
    root->addWidget(histWidget_);
    root->addLayout(grid);
    root->addLayout(modeRow);
    root->addLayout(lutRow);
    root->addLayout(hRow);
    root->addWidget(chkApplyAll_);
    root->addWidget(bb);

    connect(sldMin_, &QSlider::valueChanged, this, &BackgroundRangeDialog::onMinChanged);
    connect(sldMax_, &QSlider::valueChanged, this, &BackgroundRangeDialog::onMaxChanged);
    // Repaint histogram whenever sliders move
    connect(sldMin_, &QSlider::valueChanged, histWidget_, qOverload<>(&QWidget::update));
    connect(sldMax_, &QSlider::valueChanged, histWidget_, qOverload<>(&QWidget::update));
}

// ── Histogram ─────────────────────────────────────────────────────────────────

void BackgroundRangeDialog::buildHistogram()
{
    if (image_->data.empty()) return;
    constexpr int kBins = 256;
    histBins_.assign(kBins, 0.0f);

    const float range = dataMax_ - dataMin_;
    if (range <= 0.0f) return;

    for (float v : image_->data) {
        const int bin = std::clamp(
            static_cast<int>((v - dataMin_) / range * (kBins - 1)),
            0, kBins - 1);
        histBins_[bin] += 1.0f;
    }

    // Log-scale + normalize
    float maxVal = 1.0f;
    for (auto& b : histBins_) {
        b = std::log1p(b);
        maxVal = std::max(maxVal, b);
    }
    for (auto& b : histBins_) b /= maxVal;
}

// ── eventFilter (histogram paint) ────────────────────────────────────────────

bool BackgroundRangeDialog::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == histWidget_ && ev->type() == QEvent::Paint) {
        QPainter p(histWidget_);
        p.fillRect(histWidget_->rect(), QColor(0x0d, 0x11, 0x17));

        const int w = histWidget_->width();
        const int h = histWidget_->height();

        // Draw histogram bars
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0x1f, 0x6f, 0xeb, 160));
        const int bins = static_cast<int>(histBins_.size());
        for (int i = 0; i < bins; ++i) {
            const int x  = i * w / bins;
            const int bw = std::max(1, w / bins);
            const int bh = static_cast<int>(histBins_[i] * static_cast<float>(h - 4));
            p.drawRect(x, h - bh, bw, bh);
        }

        // Draw min (red) and max (green) cursor lines
        const int sliderRange = sldMin_->maximum();
        const int minX = sldMin_->value() * w / sliderRange;
        const int maxX = sldMax_->value() * w / sliderRange;

        p.setPen(QPen(QColor(0xf8, 0x51, 0x49), 2));
        p.drawLine(minX, 0, minX, h);
        p.setPen(QPen(QColor(0x3f, 0xb9, 0x50), 2));
        p.drawLine(maxX, 0, maxX, h);

        return true;
    }
    return QDialog::eventFilter(obj, ev);
}

void BackgroundRangeDialog::paintEvent(QPaintEvent* e) { QDialog::paintEvent(e); }

// ── Helpers ───────────────────────────────────────────────────────────────────

float BackgroundRangeDialog::sliderToValue(int sliderVal) const
{
    return dataMin_ + (dataMax_ - dataMin_) * static_cast<float>(sliderVal) / static_cast<float>(sldMin_->maximum());
}

void BackgroundRangeDialog::updatePreview()
{
    view_->setStretch(image_->displayMin, image_->displayMax);
}

// ── Slots ─────────────────────────────────────────────────────────────────────

void BackgroundRangeDialog::onMinChanged(int val)
{
    const float v = sliderToValue(val);
    lblMin_->setText(QString::number(v, 'g', 6));
    image_->displayMin = v;
    view_->setStretch(v, image_->displayMax);
}

void BackgroundRangeDialog::onMaxChanged(int val)
{
    const float v = sliderToValue(val);
    lblMax_->setText(QString::number(v, 'g', 6));
    image_->displayMax = v;
    view_->setStretch(image_->displayMin, v);
}

void BackgroundRangeDialog::onAutoClicked()
{
    core::computeAutoStretch(*image_);
    const int range = sldMin_->maximum();
    sldMin_->setValue(static_cast<int>(
        (image_->displayMin - dataMin_) / (dataMax_ - dataMin_) * static_cast<float>(range)));
    sldMax_->setValue(static_cast<int>(
        (image_->displayMax - dataMin_) / (dataMax_ - dataMin_) * static_cast<float>(range)));
    updatePreview();
}

void BackgroundRangeDialog::onApplyClicked()
{
    const auto mode = static_cast<core::StretchMode>(modeCombo_->currentData().toInt());
    const auto lut  = static_cast<core::ColorLut>(lutCombo_->currentData().toInt());
    emit stretchChanged(image_->displayMin, image_->displayMax, mode, lut,
                        chkApplyAll_->isChecked());
    accept();
}

void BackgroundRangeDialog::onRejectClicked()
{
    // Revert to original values
    image_->displayMin  = origMin_;
    image_->displayMax  = origMax_;
    image_->stretchMode = origMode_;
    image_->colorLut    = origLut_;
    view_->setStretchMode(origMode_);
    view_->setColorLut(origLut_);
    view_->setStretch(origMin_, origMax_);
    reject();
}
