#include "VerificationDialog.h"
#include "FitsImageView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QPainter>
#include <QImage>
#include <cmath>

VerificationDialog::VerificationDialog(QWidget* parent)
    : QDialog(parent, Qt::Tool | Qt::WindowStaysOnTopHint)
{
    setWindowTitle(tr("Verification"));
    setMinimumSize(420, 380);
    setAttribute(Qt::WA_DeleteOnClose, false);   // reuse across measurements

    auto* mainLay = new QHBoxLayout(this);

    // ── Left: zoom canvas ─────────────────────────────────────────────────
    auto* leftLay = new QVBoxLayout;
    zoomCanvas_ = new QLabel(this);
    const int canvasSize = kZoomRadius * 2 * kZoomFactor;
    zoomCanvas_->setFixedSize(canvasSize, canvasSize);
    zoomCanvas_->setAlignment(Qt::AlignCenter);
    zoomCanvas_->setStyleSheet(QStringLiteral("background:#0a0a14; border:1px solid #334;"));
    leftLay->addWidget(zoomCanvas_);
    leftLay->addStretch();
    mainLay->addLayout(leftLay);

    // ── Right: info + KOO list ────────────────────────────────────────────
    auto* rightLay = new QVBoxLayout;

    auto* infoBox = new QGroupBox(tr("Measurement"), this);
    auto* form    = new QFormLayout(infoBox);
    form->setSpacing(4);
    raLabel_   = new QLabel(infoBox);
    decLabel_  = new QLabel(infoBox);
    magLabel_  = new QLabel(infoBox);
    fwhmLabel_ = new QLabel(infoBox);
    snrLabel_  = new QLabel(infoBox);
    form->addRow(tr("RA:"),   raLabel_);
    form->addRow(tr("Dec:"),  decLabel_);
    form->addRow(tr("Mag:"),  magLabel_);
    form->addRow(tr("FWHM:"), fwhmLabel_);
    form->addRow(tr("SNR:"),  snrLabel_);

    nameEdit_ = new QLineEdit(infoBox);
    nameEdit_->setPlaceholderText(tr("Object designation (optional)"));
    form->addRow(tr("Object:"), nameEdit_);
    rightLay->addWidget(infoBox);

    auto* kooBox = new QGroupBox(this);
    auto* kooLay = new QVBoxLayout(kooBox);
    kooLay->setSpacing(4);
    auto* kooLabel = new QLabel(tr("Objetos conhecidos próximos:"), kooBox);
    kooLabel->setStyleSheet(QStringLiteral("font-size:10px; color:#8ab4cc;"));
    kooLay->addWidget(kooLabel);
    kooList_ = new QListWidget(kooBox);
    kooList_->setMaximumHeight(110);
    kooList_->setToolTip(tr("Clique duplo para usar como designação do objeto"));
    kooLay->addWidget(kooList_);
    rightLay->addWidget(kooBox);

    auto* btnLay = new QHBoxLayout;
    btnLay->setSpacing(6);
    auto* acceptBtn      = new QPushButton(tr("✓ Aceitar"), this);
    auto* acceptCloseBtn = new QPushButton(tr("✓ Aceitar e Fechar"), this);
    auto* rejectBtn      = new QPushButton(tr("✗ Rejeitar"), this);

    acceptBtn->setDefault(true);
    // Green — accept and keep dialog open for next measurement
    acceptBtn->setStyleSheet(
        "QPushButton { background:#174717; color:#7ed87e; border:1px solid #2b6e2b; border-radius:3px; padding:4px 8px; }"
        "QPushButton:hover { background:#1f5c1f; }");
    // Teal — accept and dismiss
    acceptCloseBtn->setStyleSheet(
        "QPushButton { background:#133848; color:#5bcad8; border:1px solid #1e5a6a; border-radius:3px; padding:4px 8px; }"
        "QPushButton:hover { background:#1a4a5c; }");
    // Red — reject
    rejectBtn->setStyleSheet(
        "QPushButton { background:#471717; color:#d87e7e; border:1px solid #6e2b2b; border-radius:3px; padding:4px 8px; }"
        "QPushButton:hover { background:#5c1f1f; }");

    btnLay->addWidget(acceptBtn);
    btnLay->addWidget(acceptCloseBtn);
    btnLay->addWidget(rejectBtn);
    rightLay->addLayout(btnLay);
    rightLay->addStretch();

    mainLay->addLayout(rightLay);

    connect(acceptBtn,      &QPushButton::clicked,           this, &VerificationDialog::onAccept);
    connect(acceptCloseBtn, &QPushButton::clicked,           this, &VerificationDialog::onAcceptAndClose);
    connect(rejectBtn,      &QPushButton::clicked,           this, &VerificationDialog::onReject);
    connect(kooList_,       &QListWidget::itemDoubleClicked, this, &VerificationDialog::onKooItemSelected);
}

void VerificationDialog::setMeasurement(const core::FitsImage& img,
                                        const core::Observation& obs,
                                        const QVector<core::KooObject>& nearbyKoo)
{
    img_      = &img;
    obs_      = obs;
    nearbyKoo_= nearbyKoo;

    raLabel_->setText(QString::number(obs.ra,  'f', 6) + QStringLiteral("°"));
    decLabel_->setText(QString::number(obs.dec, 'f', 6) + QStringLiteral("°"));
    magLabel_->setText(obs.mag > 0.0
        ? QString::number(obs.mag, 'f', 2) + QStringLiteral(" (") + obs.magBand + QStringLiteral(")")
        : tr("—"));
    fwhmLabel_->setText(obs.fwhm > 0.0
        ? QString::number(obs.fwhm, 'f', 2) + QStringLiteral("\"")
        : tr("—"));
    snrLabel_->setText(obs.snr > 0.0
        ? QString::number(obs.snr, 'f', 1)
        : tr("—"));
    nameEdit_->setText(obs.objectName);

    // Populate KOO list
    kooList_->clear();
    if (nearbyKoo.isEmpty()) {
        auto* placeholder = new QListWidgetItem(tr("Nenhum objeto conhecido próximo"), kooList_);
        placeholder->setFlags(Qt::NoItemFlags);  // not selectable / not double-clickable
        placeholder->setForeground(kooList_->palette().color(QPalette::Disabled, QPalette::Text));
    } else {
        for (const auto& k : nearbyKoo) {
            const double dra  = (k.ra  - obs.ra)  * std::cos(obs.dec * M_PI / 180.0) * 3600.0;
            const double ddec = (k.dec - obs.dec) * 3600.0;
            const double sep  = std::hypot(dra, ddec);
            kooList_->addItem(tr("%1  (sep %2\")  mag %3")
                .arg(k.name)
                .arg(sep,  0, 'f', 1)
                .arg(k.mag, 0, 'f', 1));
        }
    }

    updateZoom();
    show();
    raise();
    activateWindow();
}

void VerificationDialog::updateZoom()
{
    if (!img_ || !img_->isValid()) {
        zoomCanvas_->clear();
        return;
    }

    const int cx = static_cast<int>(std::round(obs_.pixX));
    const int cy = static_cast<int>(std::round(obs_.pixY));
    const int r  = kZoomRadius;
    const int Z  = kZoomFactor;

    // Build zoomed QImage (nearest-neighbour, apply stretch)
    const int sz = r * 2 * Z;
    QImage out(sz, sz, QImage::Format_RGB32);
    out.fill(Qt::black);

    for (int dy = -r; dy < r; ++dy) {
        for (int dx = -r; dx < r; ++dx) {
            const int sx = cx + dx;
            const int sy = cy + dy;
            QRgb pix = qRgb(0, 0, 0);
            if (sx >= 0 && sx < img_->width && sy >= 0 && sy < img_->height) {
                const float raw = img_->pixelAt(sx, sy);
                if (std::isnan(raw))
                    pix = 0xFFFF00FF;  // magenta — NaN pixel
                else {
                    const uint8_t v = core::stretchPixel(raw, img_->displayMin, img_->displayMax);
                    pix = qRgb(v, v, v);
                }
            }
            const QRect dst((dx + r) * Z, (dy + r) * Z, Z, Z);
            for (int py = dst.top(); py < dst.bottom(); ++py)
                for (int px = dst.left(); px < dst.right(); ++px)
                    out.setPixel(px, py, pix);
        }
    }

    // Draw crosshair at exact centroid position
    QPainter p(&out);
    p.setPen(QPen(QColor(255, 80, 80), 1));
    const int mx = r * Z;
    const int my = r * Z;
    p.drawLine(mx - 8, my, mx + 8, my);
    p.drawLine(mx, my - 8, mx, my + 8);
    // Sub-pixel offset indicator
    const int offX = static_cast<int>((obs_.pixX - cx) * Z);
    const int offY = static_cast<int>((obs_.pixY - cy) * Z);
    p.setPen(QPen(QColor(255, 220, 0), 1));
    p.drawEllipse(QPoint(mx + offX, my + offY), 3, 3);

    zoomCanvas_->setPixmap(QPixmap::fromImage(out));
}

void VerificationDialog::onAccept()
{
    obs_.objectName = nameEdit_->text().trimmed();
    emit accepted(obs_);
}

void VerificationDialog::onAcceptAndClose()
{
    obs_.objectName = nameEdit_->text().trimmed();
    emit accepted(obs_);
    hide();
}

void VerificationDialog::onReject()
{
    emit rejected();
}

void VerificationDialog::onKooItemSelected()
{
    if (auto* item = kooList_->currentItem()) {
        // Extract object name (before first whitespace)
        const QString text = item->text();
        const QString name = text.left(text.indexOf(' '));
        nameEdit_->setText(name);
    }
}
