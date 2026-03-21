#include "SessionInfoBar.h"
#include "core/ImageSession.h"
#include "core/FitsImage.h"

#include <QApplication>
#include <QPushButton>
#include <QFrame>

// ─── Construction ─────────────────────────────────────────────────────────────

SessionInfoBar::SessionInfoBar(QWidget* parent)
    : QFrame(parent)
{
    setObjectName(QStringLiteral("sessionInfoBar"));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setVisible(false);

    auto* outer = new QVBoxLayout(this);
    outer->setContentsMargins(12, 6, 8, 6);
    outer->setSpacing(4);

    // ── Row 1: summary + dismiss ──────────────────────────────────────────────
    auto* row1 = new QHBoxLayout;
    row1->setSpacing(8);

    summary_ = new QLabel(this);
    summary_->setWordWrap(false);
    row1->addWidget(summary_, 1);

    closeBtn_ = new QToolButton(this);
    closeBtn_->setText(QStringLiteral("×"));
    closeBtn_->setObjectName(QStringLiteral("infoBarClose"));
    closeBtn_->setFixedSize(20, 20);
    closeBtn_->setCursor(Qt::PointingHandCursor);
    connect(closeBtn_, &QToolButton::clicked, this, &SessionInfoBar::hide);
    row1->addWidget(closeBtn_, 0, Qt::AlignTop);

    outer->addLayout(row1);

    // ── Row 2: chips ─────────────────────────────────────────────────────────
    chipsRow_ = new QWidget(this);
    chipsLay_ = new QHBoxLayout(chipsRow_);
    chipsLay_->setContentsMargins(0, 0, 0, 0);
    chipsLay_->setSpacing(6);
    chipsLay_->addStretch();
    outer->addWidget(chipsRow_);

    applyTheme(true);
}

// ─── Theme ────────────────────────────────────────────────────────────────────

void SessionInfoBar::applyTheme(bool night)
{
    night_ = night;
    if (night) {
        setStyleSheet(QStringLiteral(
            "QFrame#sessionInfoBar {"
            "  background:#0f1d2e;"
            "  border-bottom:1px solid #1e3a5f;"
            "}"
            "QLabel { background:transparent; color:#90aac0; font-size:11px; }"
            "QLabel#chipOk  { color:#4a9; background:#0d2a1e; border:1px solid #1a5a36;"
            "                 border-radius:10px; padding:1px 8px; font-size:10px; }"
            "QLabel#chipWarn{ color:#d4a030; background:#2a1d08; border:1px solid #6a4010;"
            "                 border-radius:10px; padding:1px 8px; font-size:10px; }"
            "QToolButton#infoBarClose {"
            "  background:transparent; border:none; color:#406080;"
            "  font-size:14px; font-weight:bold;"
            "}"
            "QToolButton#infoBarClose:hover { color:#80b0d0; }"
            "QPushButton#actionBtn {"
            "  background:#162840; color:#60a0d0; border:1px solid #2a5070;"
            "  border-radius:10px; padding:1px 10px; font-size:10px;"
            "}"
            "QPushButton#actionBtn:hover { background:#1e3a58; border-color:#4080a0; }"
        ));
    } else {
        setStyleSheet(QStringLiteral(
            "QFrame#sessionInfoBar {"
            "  background:#eef4fc;"
            "  border-bottom:1px solid #b8d0e8;"
            "}"
            "QLabel { background:transparent; color:#3a5070; font-size:11px; }"
            "QLabel#chipOk  { color:#1a6a3a; background:#d4f0e0; border:1px solid #70c090;"
            "                 border-radius:10px; padding:1px 8px; font-size:10px; }"
            "QLabel#chipWarn{ color:#7a4800; background:#fff0cc; border:1px solid #d0a030;"
            "                 border-radius:10px; padding:1px 8px; font-size:10px; }"
            "QToolButton#infoBarClose {"
            "  background:transparent; border:none; color:#8090a8;"
            "  font-size:14px; font-weight:bold;"
            "}"
            "QToolButton#infoBarClose:hover { color:#304060; }"
            "QPushButton#actionBtn {"
            "  background:#ddeeff; color:#205080; border:1px solid #90b8d8;"
            "  border-radius:10px; padding:1px 10px; font-size:10px;"
            "}"
            "QPushButton#actionBtn:hover { background:#c8e0f4; border-color:#5090c0; }"
        ));
    }
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

QLabel* SessionInfoBar::makeChip(const QString& text, bool ok)
{
    auto* lbl = new QLabel(text, chipsRow_);
    lbl->setObjectName(ok ? QStringLiteral("chipOk") : QStringLiteral("chipWarn"));
    return lbl;
}

void SessionInfoBar::clearChips()
{
    // Remove all items except the trailing stretch
    while (chipsLay_->count() > 1) {
        auto* item = chipsLay_->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
}

// ─── showForSession ───────────────────────────────────────────────────────────

void SessionInfoBar::showForSession(const core::ImageSession& session,
                                    const QString& locationMode,
                                    bool locationOk)
{
    if (session.isEmpty()) { hide(); return; }

    const core::FitsImage& img0 = session.image(0);

    // ── Populate info struct ──────────────────────────────────────────────────
    info_.imageCount   = session.imageCount();
    info_.telescope    = img0.telescope;
    info_.filter       = img0.filter;
    info_.expTime      = img0.expTime;
    info_.width        = img0.width;
    info_.height       = img0.height;
    info_.pixScale     = img0.pixScaleX;
    info_.wcsPresolved = img0.wcs.solved;
    info_.saturation   = img0.saturation;
    info_.dateTimeOk   = img0.dateObs.isValid();
    info_.locationOk   = locationOk;
    info_.locationMode = locationMode;

    if (img0.dateObs.isValid())
        info_.dateObs = img0.dateObs.toString(QStringLiteral("yyyy-MM-dd HH:mm")) + QStringLiteral(" UTC");
    else
        info_.dateObs.clear();

    rebuild();
    setVisible(true);
}

// ─── rebuild ─────────────────────────────────────────────────────────────────

void SessionInfoBar::rebuild()
{
    const auto& i = info_;

    // ── Row 1: summary line ───────────────────────────────────────────────────
    QString row1 = QStringLiteral("<b>%1 %2</b>")
        .arg(i.imageCount)
        .arg(i.imageCount == 1 ? tr("imagem carregada") : tr("imagens carregadas"));

    QStringList meta;
    if (!i.telescope.isEmpty()) meta << i.telescope;
    if (!i.dateObs.isEmpty())   meta << i.dateObs;
    if (!i.filter.isEmpty())    meta << tr("filtro %1").arg(i.filter);
    if (i.expTime > 0.0)        meta << tr("exp %1 s").arg(i.expTime, 0, 'g', 4);
    if (i.width > 0)            meta << QStringLiteral("%1×%2 px").arg(i.width).arg(i.height);
    if (!meta.isEmpty())
        row1 += QStringLiteral("  <span style='color:%1'>%2</span>")
            .arg(night_ ? QStringLiteral("#506070") : QStringLiteral("#7090b0"),
                 meta.join(QStringLiteral("  ·  ")));

    summary_->setText(row1);

    // ── Row 2: chips ─────────────────────────────────────────────────────────
    clearChips();
    int pos = 0;

    auto addOk = [&](const QString& text) {
        chipsLay_->insertWidget(pos++, makeChip(QStringLiteral("✓ ") + text, true));
    };
    auto addWarn = [&](const QString& text, const QString& btnLabel) {
        auto* lbl = makeChip(QStringLiteral("⚠ ") + text, false);
        chipsLay_->insertWidget(pos++, lbl);
        if (!btnLabel.isEmpty()) {
            auto* btn = new QPushButton(btnLabel, chipsRow_);
            btn->setObjectName(QStringLiteral("actionBtn"));
            btn->setCursor(Qt::PointingHandCursor);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            connect(btn, &QPushButton::clicked, this, &SessionInfoBar::openSettingsRequested);
            chipsLay_->insertWidget(pos++, btn);
        }
    };

    // Pixel scale
    if (i.pixScale > 0.0)
        addOk(tr("Escala %1\"/px").arg(i.pixScale, 0, 'f', 3));
    else
        addWarn(tr("Escala de pixel ausente"), tr("Câmera →"));

    // WCS
    if (i.wcsPresolved)
        addOk(tr("WCS pré-resolvido"));

    // Date/time
    if (i.dateTimeOk)
        addOk(tr("Data/hora UTC"));

    // Saturation
    if (i.saturation > 0.0 && i.saturation < 1e9)
        addOk(tr("Saturação %1").arg(static_cast<int>(i.saturation)));

    // Location
    if (i.locationOk) {
        const QString modeStr = (i.locationMode == QStringLiteral("fits"))
            ? tr("loc. FITS")
            : (i.locationMode == QStringLiteral("preset"))
            ? tr("observatório")
            : tr("manual");
        addOk(tr("Localização (%1)").arg(modeStr));
    } else {
        if (i.locationMode == QStringLiteral("fits"))
            addWarn(tr("Localização: ausente no FITS"), tr("Observatório →"));
        else
            addWarn(tr("Localização não configurada"), tr("Configurar →"));
    }
}
