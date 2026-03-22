#include "SettingsDialog.h"

#include <QTabWidget>
#include <QLineEdit>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QComboBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QWidget>
#include <QScrollArea>
#include <QFileDialog>
#include <QStandardPaths>
#include <QLocale>
#include <QFrame>
#include <QButtonGroup>

#include "core/ObservatoryDatabase.h"
#include "core/ApiKeyStore.h"

// ─── Banner helpers ──────────────────────────────────────────────────────────

QWidget* SettingsDialog::makeAdvancedWarning(QWidget* parent)
{
    auto* frame = new QFrame(parent);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setStyleSheet(
        QStringLiteral("QFrame{background:#3a3010;border:1px solid #8a6a00;border-radius:4px;}"
                       "QLabel{color:#f0c040;font-size:10px;background:transparent;border:none;}"));
    auto* icon = new QLabel(QStringLiteral("⚠"), frame);
    icon->setStyleSheet(QStringLiteral("font-size:13px;color:#f0c040;"));
    auto* text = new QLabel(
        QObject::tr("Best values are already set — only change if you are an advanced user."), frame);
    text->setWordWrap(true);
    auto* lay = new QHBoxLayout(frame);
    lay->setContentsMargins(8,5,8,5); lay->setSpacing(6);
    lay->addWidget(icon); lay->addWidget(text, 1);
    return frame;
}

QWidget* SettingsDialog::makeDeprecatedWarning(QWidget* parent)
{
    auto* frame = new QFrame(parent);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setStyleSheet(
        QStringLiteral("QFrame{background:#3a1010;border:1px solid #8a3000;border-radius:4px;}"
                       "QLabel{color:#ff8060;font-size:10px;background:transparent;border:none;}"));
    auto* icon = new QLabel(QStringLiteral("⛔"), frame);
    icon->setStyleSheet(QStringLiteral("font-size:13px;"));
    auto* text = new QLabel(
        QObject::tr("OBSOLETE — These local catalog paths are kept for compatibility only.\n"
                    "AstroFind uses the online VizieR service by default.\n"
                    "Only configure these if you have the catalog files locally and no internet access."),
        frame);
    text->setWordWrap(true);
    auto* lay = new QHBoxLayout(frame);
    lay->setContentsMargins(8,5,8,5); lay->setSpacing(6);
    lay->addWidget(icon); lay->addWidget(text, 1);
    return frame;
}

QWidget* SettingsDialog::makeWarningBadge(const QString& text, QWidget* parent)
{
    auto* frame = new QFrame(parent);
    frame->setFrameShape(QFrame::StyledPanel);
    frame->setStyleSheet(
        QStringLiteral("QFrame{background:#3a3010;border:1px solid #8a6a00;border-radius:4px;}"
                       "QLabel{color:#f0c040;font-size:10px;background:transparent;border:none;}"));
    auto* icon = new QLabel(QStringLiteral("⚠"), frame);
    icon->setStyleSheet(QStringLiteral("font-size:12px;color:#f0c040;"));
    auto* lbl  = new QLabel(text, frame);
    lbl->setWordWrap(true);
    auto* lay = new QHBoxLayout(frame);
    lay->setContentsMargins(6,4,6,4); lay->setSpacing(5);
    lay->addWidget(icon); lay->addWidget(lbl, 1);
    return frame;
}

// ─── Browse helper ───────────────────────────────────────────────────────────

static QHBoxLayout* browseRow(QLineEdit* edit, QObject* receiver,
                               const std::function<void()>& slot, QWidget* parent)
{
    auto* btn = new QPushButton(QStringLiteral("…"), parent);
    btn->setFixedWidth(30);
    QObject::connect(btn, &QPushButton::clicked, receiver, slot);
    auto* row = new QHBoxLayout;
    row->addWidget(edit); row->addWidget(btn);
    return row;
}

// ─── Constructor ─────────────────────────────────────────────────────────────

SettingsDialog::SettingsDialog(QSettings& settings, QWidget* parent)
    : QDialog(parent), settings_(settings)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(560, 540);
    setModal(true);

    auto* mainLay = new QVBoxLayout(this);
    mainLay->setContentsMargins(12,12,12,8);
    mainLay->setSpacing(8);

    auto* tabs = new QTabWidget(this);
    buildObserverTab(tabs);
    buildCameraTab(tabs);
    buildConnectionsTab(tabs);
    buildDetectionTab(tabs);
    buildDisplayTab(tabs);
    buildLegacyTab(tabs);

    auto* resetBtn = new QPushButton(tr("Reset to Defaults"), this);
    resetBtn->setToolTip(tr("Restore all settings to their recommended default values"));
    connect(resetBtn, &QPushButton::clicked, this, &SettingsDialog::onResetDefaults);

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(bb, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccept);
    connect(bb, &QDialogButtonBox::rejected, this, &QDialog::reject);

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(resetBtn); btnRow->addStretch(); btnRow->addWidget(bb);

    mainLay->addWidget(tabs);
    mainLay->addLayout(btnRow);

    loadFromSettings();
}

// ─── FITS location hint ───────────────────────────────────────────────────────

void SettingsDialog::setFitsLocationHint(double lat, double lon, double alt)
{
    fitsLat_ = lat;
    fitsLon_ = lon;
    fitsAlt_ = alt;
    updateLocationWidgets();
}

void SettingsDialog::setSpacecraftHint(bool isSpacecraft, const QString& name, const QString& mpcCode)
{
    spacecraftHint_    = isSpacecraft;
    spacecraftName_    = name;
    spacecraftMpcCode_ = mpcCode;
    updateLocationWidgets();
}

// ─── Tab helpers ─────────────────────────────────────────────────────────────

static QWidget* scrolled(QWidget* inner)
{
    auto* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setFrameShape(QFrame::NoFrame);
    area->setWidget(inner);
    return area;
}

static QFormLayout* stdForm(QWidget* page)
{
    auto* f = new QFormLayout(page);
    f->setContentsMargins(16,14,16,14);
    f->setSpacing(9);
    f->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    return f;
}

// ─── Location group helpers ───────────────────────────────────────────────────

QString SettingsDialog::fitsLocationLabel() const
{
    if (std::isnan(fitsLat_) || std::isnan(fitsLon_))
        return QString();
    const double alt = std::isnan(fitsAlt_) ? 0.0 : fitsAlt_;
    return QStringLiteral("Lat: %1°   Lon: %2°   Alt: %3 m")
        .arg(fitsLat_, 0, 'f', 6)
        .arg(fitsLon_, 0, 'f', 6)
        .arg(static_cast<int>(alt));
}

void SettingsDialog::populateObservatoryCombo(const QString& countryCode)
{
    obsCombo_->clear();
    if (countryCode.isEmpty()) {
        obsCombo_->setEnabled(false);
        return;
    }
    const QByteArray cc = countryCode.toLatin1();
    const QVector<const Observatory*> obs =
        ObservatoryDatabase::forCountry(cc.constData());
    for (const Observatory* o : obs) {
        const QString label = QStringLiteral("%1 — %2 (%3)")
            .arg(QString::fromLatin1(o->mpcCode))
            .arg(QString::fromUtf8(o->name))
            .arg(QString::fromUtf8(o->city));
        obsCombo_->addItem(label, QString::fromLatin1(o->mpcCode));
    }
    obsCombo_->setEnabled(obsCombo_->count() > 0);
}

void SettingsDialog::updateLocationWidgets()
{
    const bool fitsAvailable = !std::isnan(fitsLat_) && !std::isnan(fitsLon_);

    // Enable/disable FITS radio
    locFitsRad_->setEnabled(fitsAvailable && !spacecraftHint_);

    // Enable spacecraft radio only when a spacecraft was detected
    locSpacecraftRad_->setEnabled(spacecraftHint_);

    // FITS sub-widget: badge (shown when FITS mode is unavailable)
    fitsBadge_->setVisible(!fitsAvailable);
    fitsCoordLabel_->setText(fitsLocationLabel());
    fitsCoordLabel_->setVisible(fitsAvailable);

    // Preset/manual warnings (shown when FITS location IS available and not spacecraft)
    if (presetWarning_) presetWarning_->setVisible(fitsAvailable && !spacecraftHint_);
    if (manualWarning_) manualWarning_->setVisible(fitsAvailable && !spacecraftHint_);

    // Update spacecraft info label
    if (spacecraftHint_) {
        const QString mpcStr = spacecraftMpcCode_.isEmpty()
            ? tr("Código MPC: não identificado — configure manualmente no campo acima")
            : tr("Código MPC: %1").arg(spacecraftMpcCode_);
        spacecraftInfoLabel_->setText(
            QStringLiteral("%1\n%2\nLat/Lon: não aplicável (órbita terrestre)")
                .arg(spacecraftName_, mpcStr));
    } else {
        spacecraftInfoLabel_->setText(tr("Nenhum telescópio espacial detectado nas imagens."));
    }

    // Update preset coord label
    if (obsCombo_->currentIndex() >= 0 && obsCombo_->count() > 0) {
        const QString mpcCode = obsCombo_->currentData().toString();
        const QByteArray cc = mpcCode.toLatin1();
        if (const Observatory* o = ObservatoryDatabase::byCode(cc.constData())) {
            presetCoordLabel_->setText(
                QStringLiteral("Lat: %1°   Lon: %2°   Alt: %3 m")
                    .arg(o->lat, 0, 'f', 6)
                    .arg(o->lon, 0, 'f', 6)
                    .arg(static_cast<int>(o->alt)));
        }
    }

    // Show/hide sub-widgets based on selected radio
    const int mode = locationBtnGroup_->checkedId();
    fitsSubWidget_->setVisible(mode == 0);
    presetSubWidget_->setVisible(mode == 1);
    manualSubWidget_->setVisible(mode == 2);
    spacecraftSubWidget_->setVisible(mode == 3);
}

void SettingsDialog::buildLocationGroup(QWidget* page)
{
    auto* grp  = new QGroupBox(tr("Location (topocentric correction)"), page);
    auto* vlay = new QVBoxLayout(grp);
    vlay->setContentsMargins(12,12,12,10);
    vlay->setSpacing(6);

    locationBtnGroup_ = new QButtonGroup(this);

    // ── Radio 0: Automático (do FITS) ─────────────────────────────────────────
    locFitsRad_ = new QRadioButton(tr("Automático (do FITS)"), grp);
    locFitsRad_->setToolTip(tr("Use the observer location embedded in the FITS headers "
        "(SITELAT/SITELONG or LAT-OBS/LONG-OBS)."));
    locationBtnGroup_->addButton(locFitsRad_, 0);

    fitsSubWidget_ = new QWidget(grp);
    auto* fitsSub  = new QVBoxLayout(fitsSubWidget_);
    fitsSub->setContentsMargins(20,2,0,4); fitsSub->setSpacing(3);

    fitsBadge_ = makeWarningBadge(
        tr("FITS não fornece localização"), fitsSubWidget_);
    fitsCoordLabel_ = new QLabel(fitsSubWidget_);
    fitsCoordLabel_->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));
    fitsSub->addWidget(fitsBadge_);
    fitsSub->addWidget(fitsCoordLabel_);

    vlay->addWidget(locFitsRad_);
    vlay->addWidget(fitsSubWidget_);

    // ── Radio 1: Observatório predefinido ────────────────────────────────────
    locPresetRad_ = new QRadioButton(tr("Observatório predefinido"), grp);
    locPresetRad_->setToolTip(tr("Select a well-known observatory from the built-in database."));
    locationBtnGroup_->addButton(locPresetRad_, 1);

    presetSubWidget_ = new QWidget(grp);
    auto* presetSub  = new QVBoxLayout(presetSubWidget_);
    presetSub->setContentsMargins(20,2,0,4); presetSub->setSpacing(3);

    // Country + observatory combos
    auto* comboRow = new QHBoxLayout;
    countryCombo_ = new QComboBox(presetSubWidget_);
    countryCombo_->setEditable(true);
    countryCombo_->setInsertPolicy(QComboBox::NoInsert);
    countryCombo_->lineEdit()->setPlaceholderText(tr("País…"));

    const QStringList ccs = ObservatoryDatabase::countries();
    for (const QString& cc : ccs) {
        const QByteArray ba = cc.toLatin1();
        countryCombo_->addItem(
            ObservatoryDatabase::countryName(ba.constData()) +
                QStringLiteral(" (") + cc + QStringLiteral(")"),
            cc);
    }
    countryCombo_->setMinimumContentsLength(18);
    countryCombo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    obsCombo_ = new QComboBox(presetSubWidget_);
    obsCombo_->setEditable(true);
    obsCombo_->setInsertPolicy(QComboBox::NoInsert);
    obsCombo_->setEnabled(false);
    // Calculate a reasonable minimum width based on typical longest entry
    obsCombo_->setMinimumContentsLength(38);
    obsCombo_->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);

    comboRow->addWidget(countryCombo_, 1);
    comboRow->addWidget(obsCombo_,     2);

    presetCoordLabel_ = new QLabel(presetSubWidget_);
    presetCoordLabel_->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));

    presetWarning_ = makeWarningBadge(
        tr("Localização do FITS disponível — considere usar o modo Automático."),
        presetSubWidget_);

    presetSub->addLayout(comboRow);
    presetSub->addWidget(presetCoordLabel_);
    presetSub->addWidget(presetWarning_);

    connect(countryCombo_, qOverload<int>(&QComboBox::currentIndexChanged),
            this, &SettingsDialog::onCountryChanged);
    connect(obsCombo_, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int) { updateLocationWidgets(); });

    vlay->addWidget(locPresetRad_);
    vlay->addWidget(presetSubWidget_);

    // ── Radio 2: Coordenadas manuais ─────────────────────────────────────────
    locManualRad_ = new QRadioButton(tr("Coordenadas manuais"), grp);
    locManualRad_->setToolTip(tr("Enter the observer coordinates manually."));
    locationBtnGroup_->addButton(locManualRad_, 2);

    manualSubWidget_ = new QWidget(grp);
    auto* manSub  = new QVBoxLayout(manualSubWidget_);
    manSub->setContentsMargins(20,2,0,4); manSub->setSpacing(3);

    auto* coordForm = new QFormLayout;
    coordForm->setContentsMargins(0,0,0,0); coordForm->setSpacing(6);
    coordForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    latitudeSpin_ = new QDoubleSpinBox(manualSubWidget_);
    latitudeSpin_->setRange(-90,90); latitudeSpin_->setDecimals(6);
    latitudeSpin_->setSuffix(QStringLiteral("  °  (+N)"));
    latitudeSpin_->setToolTip(tr("Latitude geodética do seu observatório.\n"
        "Obrigatório para relatórios ADES precisos.\n"
        "Exemplo: -8.054553 (Recife, Brasil)\n"
        "Valores 0,0 indicam localização não configurada."));
    coordForm->addRow(tr("Latitude:"), latitudeSpin_);

    longitudeSpin_ = new QDoubleSpinBox(manualSubWidget_);
    longitudeSpin_->setRange(-180,180); longitudeSpin_->setDecimals(6);
    longitudeSpin_->setSuffix(QStringLiteral("  °  (+E)"));
    longitudeSpin_->setToolTip(tr("Longitude geodética do seu observatório (positivo = Leste).\n"
        "Obrigatório para relatórios ADES precisos.\n"
        "Exemplo: -34.882830 (Recife, Brasil)\n"
        "Valores 0,0 indicam localização não configurada."));
    coordForm->addRow(tr("Longitude:"), longitudeSpin_);

    altitudeSpin_ = new QSpinBox(manualSubWidget_);
    altitudeSpin_->setRange(-500,8000); altitudeSpin_->setSuffix(QStringLiteral("  m"));
    coordForm->addRow(tr("Altitude:"), altitudeSpin_);

    manualWarning_ = makeWarningBadge(
        tr("Localização do FITS disponível — considere usar o modo Automático."),
        manualSubWidget_);

    manSub->addLayout(coordForm);
    manSub->addWidget(manualWarning_);

    vlay->addWidget(locManualRad_);
    vlay->addWidget(manualSubWidget_);

    // ── Radio 3: Telescópio Espacial ─────────────────────────────────────────
    locSpacecraftRad_ = new QRadioButton(tr("Telescópio Espacial"), grp);
    locSpacecraftRad_->setToolTip(tr(
        "Imagens de um telescópio em órbita (HST, JWST, etc.).\n"
        "Lat/Lon não se aplicam — o código MPC do telescópio é usado nos relatórios.\n"
        "Este modo é habilitado automaticamente quando detectado no cabeçalho FITS."));
    locSpacecraftRad_->setEnabled(false);  // enabled only when spacecraft is detected
    locationBtnGroup_->addButton(locSpacecraftRad_, 3);

    spacecraftSubWidget_ = new QWidget(grp);
    auto* scSub = new QVBoxLayout(spacecraftSubWidget_);
    scSub->setContentsMargins(20,2,0,4); scSub->setSpacing(3);

    spacecraftInfoLabel_ = new QLabel(spacecraftSubWidget_);
    spacecraftInfoLabel_->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));
    spacecraftInfoLabel_->setWordWrap(true);
    spacecraftInfoLabel_->setText(tr("Nenhum telescópio espacial detectado nas imagens."));
    scSub->addWidget(spacecraftInfoLabel_);

    vlay->addWidget(locSpacecraftRad_);
    vlay->addWidget(spacecraftSubWidget_);

    connect(locationBtnGroup_, &QButtonGroup::idToggled,
            this, [this](int, bool checked) { if (checked) onLocationModeChanged(); });

    // Add groupbox to the caller-supplied page layout (caller does form->addRow)
    // We return through the page's layout — see buildObserverTab for usage
    auto* pageLayout = qobject_cast<QFormLayout*>(page->layout());
    if (pageLayout) {
        pageLayout->addRow(grp);
    } else {
        // Fallback: embed via a sub-vlay
        auto* subV = new QVBoxLayout;
        subV->addWidget(grp);
        page->layout()->addItem(subV);
    }
}

// ─── Tab 1 — Observer ────────────────────────────────────────────────────────

void SettingsDialog::buildObserverTab(QTabWidget* tabs)
{
    auto* page = new QWidget;
    auto* form = stdForm(page);

    mpcCodeEdit_ = new QLineEdit(page);
    mpcCodeEdit_->setMaxLength(3);
    mpcCodeEdit_->setPlaceholderText(QStringLiteral("e.g.  568"));
    mpcCodeEdit_->setToolTip(tr("3-character MPC observatory code. "
        "Required for ADES report submission. Apply at minorplanetcenter.net."));
    form->addRow(tr("MPC Station Code:"), mpcCodeEdit_);

    observerEdit_ = new QLineEdit(page);
    observerEdit_->setPlaceholderText(tr("Full name"));
    observerEdit_->setToolTip(tr("Observer name as it will appear in ADES reports."));
    form->addRow(tr("Observer:"), observerEdit_);

    measurerEdit_ = new QLineEdit(page);
    measurerEdit_->setPlaceholderText(tr("Leave blank to use Observer name"));
    form->addRow(tr("Measurer:"), measurerEdit_);

    telescopeEdit_ = new QLineEdit(page);
    telescopeEdit_->setPlaceholderText(tr("e.g.  0.35-m f/7 SCT + CCD"));
    form->addRow(tr("Telescope / Instrument:"), telescopeEdit_);

    form->addRow(new QLabel(tr(""), page));

    // Location group (3-mode widget)
    buildLocationGroup(page);

    form->addRow(new QLabel(tr(""), page));
    form->addRow(new QLabel(tr("Time:"), page));

    timeZoneSpin_ = new QDoubleSpinBox(page);
    timeZoneSpin_->setRange(-14,14); timeZoneSpin_->setDecimals(2);
    timeZoneSpin_->setSuffix(QStringLiteral("  h  (UTC offset)"));
    timeZoneSpin_->setToolTip(tr("Your local timezone offset from UTC. "
        "Used only if FITS headers do not contain UTC timestamps."));
    form->addRow(tr("Time Zone:"), timeZoneSpin_);

    timeOffsetSpin_ = new QDoubleSpinBox(page);
    timeOffsetSpin_->setRange(-999,999); timeOffsetSpin_->setDecimals(2);
    timeOffsetSpin_->setSuffix(QStringLiteral("  s"));
    timeOffsetSpin_->setToolTip(tr("Constant time correction added to all image timestamps. "
        "Use only if your system clock or FITS headers have a known offset."));
    form->addRow(tr("Time Offset:"), timeOffsetSpin_);

    timePrecSpin_ = new QDoubleSpinBox(page);
    timePrecSpin_->setRange(0.001,24.0); timePrecSpin_->setDecimals(3);
    timePrecSpin_->setSuffix(QStringLiteral("  h"));
    timePrecSpin_->setToolTip(tr("Timing precision of your observations, reported in ADES. "
        "Use 1.0 h for typical CCD observations."));
    form->addRow(tr("Time Precision:"), timePrecSpin_);

    form->addRow(new QLabel(tr(""), page));
    form->addRow(new QLabel(tr("Contact (included in ADES submission if enabled):"), page));

    contact1Edit_ = new QLineEdit(page);
    contact1Edit_->setPlaceholderText(tr("Name / organization"));
    form->addRow(tr("Contact 1:"), contact1Edit_);

    contact2Edit_ = new QLineEdit(page);
    contact2Edit_->setPlaceholderText(tr("Address line 2 (optional)"));
    form->addRow(tr("Contact 2:"), contact2Edit_);

    emailEdit_ = new QLineEdit(page);
    emailEdit_->setPlaceholderText(tr("email@example.com"));
    form->addRow(tr("e-Mail:"), emailEdit_);

    inclContactChk_ = new QCheckBox(tr("Include contact info in ADES report"), page);
    form->addRow(inclContactChk_);

    form->addRow(new QLabel(tr(""), page));
    form->addRow(new QLabel(tr("Escola:"), page));
    schoolEmailEdit_ = new QLineEdit(page);
    schoolEmailEdit_->setPlaceholderText(tr("professor@escola.edu.br"));
    schoolEmailEdit_->setToolTip(tr("E-mail do professor ou escola. Se configurado, o botão "
        "\"Enviar para Professor\" aparece na janela de relatório ADES."));
    form->addRow(tr("E-mail do Professor:"), schoolEmailEdit_);

    tabs->addTab(scrolled(page), tr("Observer"));
}

// ─── Tab 2 — Camera ──────────────────────────────────────────────────────────

void SettingsDialog::buildCameraTab(QTabWidget* tabs)
{
    auto* page  = new QWidget;
    auto* vlay  = new QVBoxLayout(page);
    vlay->setContentsMargins(16,14,16,14); vlay->setSpacing(10);

    vlay->addWidget(makeAdvancedWarning(page));

    auto* form = new QFormLayout;
    form->setSpacing(9);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    auto* psNote = new QLabel(
        tr("Pixel scale is used as fallback when no WCS solution is available.\n"
           "Leave at 0.0 to let the plate solver determine it automatically."), page);
    psNote->setWordWrap(true);
    psNote->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));
    form->addRow(psNote);

    pixScaleXSpin_ = new QDoubleSpinBox(page);
    pixScaleXSpin_->setRange(0,1); pixScaleXSpin_->setDecimals(6);
    pixScaleXSpin_->setSuffix(QStringLiteral("  °/px"));
    pixScaleXSpin_->setToolTip(tr("Pixel scale in X (degrees per pixel). 0 = auto."));
    pixScaleXSpin_->setSpecialValueText(tr("Auto"));
    form->addRow(tr("Pixel Scale X:"), pixScaleXSpin_);

    pixScaleYSpin_ = new QDoubleSpinBox(page);
    pixScaleYSpin_->setRange(0,1); pixScaleYSpin_->setDecimals(6);
    pixScaleYSpin_->setSuffix(QStringLiteral("  °/px"));
    pixScaleYSpin_->setToolTip(tr("Pixel scale in Y (degrees per pixel). 0 = auto."));
    pixScaleYSpin_->setSpecialValueText(tr("Auto"));
    form->addRow(tr("Pixel Scale Y:"), pixScaleYSpin_);

    focalLenSpin_ = new QDoubleSpinBox(page);
    focalLenSpin_->setRange(0,20000); focalLenSpin_->setDecimals(1);
    focalLenSpin_->setSuffix(QStringLiteral("  mm"));
    focalLenSpin_->setToolTip(tr("Effective focal length. Used only for display; "
        "the plate solver determines the actual scale."));
    focalLenSpin_->setSpecialValueText(tr("Unknown"));
    form->addRow(tr("Focal Length:"), focalLenSpin_);

    saturationSpin_ = new QSpinBox(page);
    saturationSpin_->setRange(100, 1000000);
    saturationSpin_->setSuffix(QStringLiteral("  ADU"));
    saturationSpin_->setToolTip(tr("CCD saturation level. Pixels above this value are "
        "flagged and excluded from astrometric and photometric fits. "
        "Check your camera datasheet."));
    form->addRow(tr("Saturation Level:"), saturationSpin_);

    auto* dtNote = new QLabel(
        tr("ΔT = TT − UTC: difference between Terrestrial Time and UTC.\n"
           "Used for precise ephemeris calculations. Updated annually by IERS.\n"
           "Current value (2024): ~68 s."), page);
    dtNote->setWordWrap(true);
    dtNote->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));
    form->addRow(dtNote);

    deltaTSpin_ = new QDoubleSpinBox(page);
    deltaTSpin_->setRange(0,200); deltaTSpin_->setDecimals(1);
    deltaTSpin_->setSuffix(QStringLiteral("  s"));
    deltaTSpin_->setToolTip(tr("ΔT = TT − UTC in seconds. "
        "Used to convert image UTC timestamps to Terrestrial Time for ephemeris lookups."));
    form->addRow(tr("ΔT (TT − UTC):"), deltaTSpin_);

    vlay->addLayout(form);

    // ── Bad pixel correction ──────────────────────────────────────────────────
    auto* bpGroup = new QGroupBox(tr("Bad Pixel Correction"), page);
    auto* bpVlay  = new QVBoxLayout(bpGroup);
    bpVlay->setSpacing(8);

    badPixelChk_ = new QCheckBox(tr("Corrigir pixels ruins automaticamente (ativado por padrão)"), bpGroup);
    badPixelChk_->setToolTip(tr("Detecta e interpola hot pixels, bad columns e raios cósmicos "
        "usando estatísticas locais (mediana 3×3). Aplicado após dark/flat, antes da detecção de estrelas."));
    bpVlay->addWidget(badPixelChk_);

    auto* bpSigmaRow = new QHBoxLayout;
    bpSigmaRow->addWidget(new QLabel(tr("Limiar (σ):"), bpGroup));
    badPixelSigmaSpin_ = new QDoubleSpinBox(bpGroup);
    badPixelSigmaSpin_->setRange(2.0, 15.0); badPixelSigmaSpin_->setDecimals(1); badPixelSigmaSpin_->setSingleStep(0.5);
    badPixelSigmaSpin_->setToolTip(tr("Pixels com desvio > N σ da mediana local são corrigidos. "
        "Valores menores corrigem mais pixels (mais agressivo); maior = mais conservador."));
    bpSigmaRow->addWidget(badPixelSigmaSpin_);
    bpSigmaRow->addStretch();
    bpVlay->addLayout(bpSigmaRow);

    badPixelWarnWidget_ = makeWarningBadge(
        tr("ATENÇÃO: desativar a correção de pixels ruins pode gerar falsas detecções "
           "de estrelas, comprometer a astrometria e introduzir erros fotométricos. "
           "Desative apenas se usar uma máscara BPM externa."), bpGroup);
    badPixelWarnWidget_->setVisible(false);
    bpVlay->addWidget(badPixelWarnWidget_);

    connect(badPixelChk_, &QCheckBox::toggled, this, [this](bool checked){
        badPixelSigmaSpin_->setEnabled(checked);
        badPixelWarnWidget_->setVisible(!checked);
    });

    vlay->addWidget(bpGroup);
    vlay->addStretch();

    tabs->addTab(page, tr("Camera"));
}

// ─── Tab 3 — Connections ─────────────────────────────────────────────────────

void SettingsDialog::buildConnectionsTab(QTabWidget* tabs)
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);
    vlay->setContentsMargins(16,14,16,14); vlay->setSpacing(10);

    // Plate solving — backend selector
    auto* psGroup = new QGroupBox(tr("Plate Solving"), page);
    auto* psForm  = new QFormLayout(psGroup);
    psForm->setContentsMargins(12,12,12,12); psForm->setSpacing(8);
    psForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    solverBackendCombo_ = new QComboBox(psGroup);
    solverBackendCombo_->addItem(tr("astrometry.net (online)"), QStringLiteral("astrometry_net"));
    solverBackendCombo_->addItem(tr("ASTAP (local, offline)"),  QStringLiteral("astap"));
    psForm->addRow(tr("Backend:"), solverBackendCombo_);

    // ASTAP path row (shown when ASTAP is selected)
    astapPathEdit_ = new QLineEdit(psGroup);
    astapPathEdit_->setPlaceholderText(tr("/usr/bin/astap  ou  C:\\Program Files\\astap\\astap.exe"));
    astapPathEdit_->setToolTip(tr("Caminho completo para o executável ASTAP."));
    psForm->addRow(tr("Executável ASTAP:"),
        browseRow(astapPathEdit_, this, [this] {
            const QString p = QFileDialog::getOpenFileName(this, tr("Localizar ASTAP"), QString());
            if (!p.isEmpty()) astapPathEdit_->setText(p);
        }, psGroup));

    // Separator label
    auto* netLabel = new QLabel(tr("— astrometry.net —"), psGroup);
    netLabel->setAlignment(Qt::AlignCenter);
    netLabel->setStyleSheet(QStringLiteral("color: gray; font-size:10px;"));
    psForm->addRow(netLabel);

    auto* keyRow = new QHBoxLayout;
    apiKeyEdit_ = new QLineEdit(psGroup);
    apiKeyEdit_->setEchoMode(QLineEdit::Password);
    apiKeyEdit_->setPlaceholderText(tr("Paste your key here"));
    showKeyBtn_ = new QPushButton(tr("Show"), psGroup);
    showKeyBtn_->setCheckable(true); showKeyBtn_->setFixedWidth(60);
    connect(showKeyBtn_, &QPushButton::toggled, this, &SettingsDialog::onToggleApiKeyVisibility);
    keyRow->addWidget(apiKeyEdit_); keyRow->addWidget(showKeyBtn_);
    psForm->addRow(tr("API Key:"), keyRow);

    // Security storage badge
    {
        auto* badge = new QLabel(psGroup);
        badge->setWordWrap(true);
        badge->setStyleSheet(QStringLiteral("font-size:9px;"));
        if (core::ApiKeyStore::isSecure()) {
            badge->setText(tr(
                "<span style='color:#60c060;'>&#128274; Stored in system keychain (KWallet / SecretService)</span>"));
        } else {
            badge->setText(tr(
                "<span style='color:#c0a020;'>&#9888; Stored in plain text — install "
                "<b>qtkeychain-qt6-devel</b> + <b>libsecret-devel</b> for secure storage</span>"));
        }
        psForm->addRow(QString(), badge);
    }

    auto* link = new QLabel(
        QStringLiteral("<a href='https://nova.astrometry.net/api_help'>")
        + tr("Get a free key at nova.astrometry.net") + QStringLiteral("</a>"), psGroup);
    link->setOpenExternalLinks(true);
    link->setStyleSheet(QStringLiteral("font-size:10px;color:#6699cc;"));
    psForm->addRow(QString(), link);

    serverUrlEdit_ = new QLineEdit(psGroup);
    serverUrlEdit_->setToolTip(tr("Leave as default unless you run a local astrometry.net instance."));
    psForm->addRow(tr("Server URL:"), serverUrlEdit_);

    timeoutSpin_ = new QSpinBox(psGroup);
    timeoutSpin_->setRange(60,600); timeoutSpin_->setSuffix(QStringLiteral("  s"));
    psForm->addRow(tr("Timeout:"), timeoutSpin_);

    vlay->addWidget(psGroup);

    // Paths
    auto* pathGroup = new QGroupBox(tr("Paths"), page);
    auto* pathForm  = new QFormLayout(pathGroup);
    pathForm->setContentsMargins(12,12,12,12); pathForm->setSpacing(8);
    pathForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    mpcOrbPathEdit_ = new QLineEdit(pathGroup);
    mpcOrbPathEdit_->setToolTip(tr("Path to MPCORB.DAT. Use Internet → Download MPCOrb to get it."));
    pathForm->addRow(tr("MPCORB.DAT:"),
        browseRow(mpcOrbPathEdit_, this, [this]{ onBrowseMpcOrb(); }, pathGroup));

    ccdDirEdit_ = new QLineEdit(pathGroup);
    ccdDirEdit_->setToolTip(tr("Default folder opened by File → Load Images."));
    pathForm->addRow(tr("Default image folder:"),
        browseRow(ccdDirEdit_, this, [this]{ onBrowseCcdDir(); }, pathGroup));

    reportDirEdit_ = new QLineEdit(pathGroup);
    reportDirEdit_->setToolTip(tr("Default folder for saving ADES reports."));
    pathForm->addRow(tr("Report output folder:"),
        browseRow(reportDirEdit_, this, [this]{ onBrowseReportDir(); }, pathGroup));

    saveWcsChk_ = new QCheckBox(tr("Save WCS back to FITS file after plate solve"), pathGroup);
    saveWcsChk_->setToolTip(tr("When enabled, the plate solution is written back into "
        "the FITS header of the original file (modifies the file on disk)."));
    pathForm->addRow(saveWcsChk_);

    vlay->addWidget(pathGroup);

    // VizieR — advanced
    auto* vizGroup = new QGroupBox(tr("Star Catalog (VizieR)"), page);
    auto* vizForm  = new QFormLayout(vizGroup);
    vizForm->setContentsMargins(12,12,12,12); vizForm->setSpacing(8);
    vizForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    vizForm->addRow(makeAdvancedWarning(vizGroup));

    catalogTypeCombo_ = new QComboBox(vizGroup);
    catalogTypeCombo_->addItem(QStringLiteral("UCAC4 (recommended)"), QStringLiteral("UCAC4"));
    catalogTypeCombo_->addItem(QStringLiteral("Gaia DR3"),            QStringLiteral("GaiaDR3"));
    catalogTypeCombo_->setToolTip(tr("Astrometric reference catalog downloaded from VizieR. "
        "UCAC4 includes proper motions; Gaia DR3 is deeper but slower."));
    vizForm->addRow(tr("Catalog:"), catalogTypeCombo_);

    vizierEdit_ = new QLineEdit(vizGroup);
    vizierEdit_->setToolTip(tr("VizieR mirror server hostname. "
        "Change only if the default server is unreachable from your location."));
    vizForm->addRow(tr("VizieR mirror:"), vizierEdit_);

    catalogSourceCombo_ = new QComboBox(vizGroup);
    catalogSourceCombo_->addItem(tr("VizieR (online, recommended)"), QStringLiteral("vizier"));
    catalogSourceCombo_->addItem(tr("Local FITS BINTABLE"),           QStringLiteral("local"));
    catalogSourceCombo_->setToolTip(tr("Where to fetch astrometric reference stars.\n"
        "Local FITS: use a pre-downloaded FITS BINTABLE file (USNO-B, UCAC, Gaia)."));
    vizForm->addRow(tr("Source:"), catalogSourceCombo_);

    auto* localRow = new QHBoxLayout;
    localCatalogPathEdit_ = new QLineEdit(vizGroup);
    localCatalogPathEdit_->setPlaceholderText(tr("/path/to/catalog.fits"));
    localCatalogBrowse_ = new QPushButton(tr("Browse…"), vizGroup);
    localRow->addWidget(localCatalogPathEdit_);
    localRow->addWidget(localCatalogBrowse_);
    vizForm->addRow(tr("Local catalog:"), localRow);
    connect(localCatalogBrowse_, &QPushButton::clicked, this, &SettingsDialog::onBrowseLocalCatalog);
    connect(catalogSourceCombo_, qOverload<int>(&QComboBox::currentIndexChanged),
            this, [this](int) {
        const bool isLocal = (catalogSourceCombo_->currentData().toString() == "local");
        localCatalogPathEdit_->setEnabled(isLocal);
        localCatalogBrowse_->setEnabled(isLocal);
    });

    vlay->addWidget(vizGroup);

    // MPC Submission — advanced
    auto* mpcSubGroup = new QGroupBox(tr("MPC Submission"), page);
    auto* mpcSubForm  = new QFormLayout(mpcSubGroup);
    mpcSubForm->setContentsMargins(12,12,12,12); mpcSubForm->setSpacing(8);
    mpcSubForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    mpcSubForm->addRow(makeAdvancedWarning(mpcSubGroup));

    mpcSubmitEdit_ = new QLineEdit(mpcSubGroup);
    mpcSubmitEdit_->setToolTip(tr("HTTP endpoint for ADES report submission. "
        "Leave as default unless the MPC changes the URL."));
    mpcSubForm->addRow(tr("Submit URL:"), mpcSubmitEdit_);

    vlay->addWidget(mpcSubGroup);
    vlay->addStretch();

    tabs->addTab(scrolled(page), tr("Connections"));
}

// ─── Tab 4 — Detection / Photometry ──────────────────────────────────────────

void SettingsDialog::buildDetectionTab(QTabWidget* tabs)
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);
    vlay->setContentsMargins(16,14,16,14); vlay->setSpacing(10);

    // Background subtraction
    auto* bgGroup = new QGroupBox(tr("Sky Background"), page);
    auto* bgVlay  = new QVBoxLayout(bgGroup);
    bgVlay->setContentsMargins(12,12,12,12); bgVlay->setSpacing(8);

    bgSubtractChk_ = new QCheckBox(tr("Subtrair modelo de fundo antes da detecção"), bgGroup);
    bgSubtractChk_->setToolTip(tr("Estima o céu de fundo por mediana deslizante (grade de tiles com\n"
        "sigma-clipping 3σ) e o subtrai dos dados antes de detectar estrelas.\n"
        "Útil para imagens com gradientes de brilho de céu, vinheta ou nebulosidade difusa."));
    bgVlay->addWidget(bgSubtractChk_);

    auto* tileRow = new QHBoxLayout;
    tileRow->setContentsMargins(20,0,0,0);
    auto* tileLbl = new QLabel(tr("Tamanho do tile:"), bgGroup);
    bgTileSizeSpin_ = new QSpinBox(bgGroup);
    bgTileSizeSpin_->setRange(16, 512); bgTileSizeSpin_->setSingleStep(16);
    bgTileSizeSpin_->setSuffix(QStringLiteral("  px"));
    bgTileSizeSpin_->setToolTip(tr("Lado do tile em pixels. Tiles menores seguem gradientes mais finos\n"
        "mas podem confundir nebulosidades com céu. 64–128 px é um bom ponto de partida."));
    tileRow->addWidget(tileLbl); tileRow->addWidget(bgTileSizeSpin_); tileRow->addStretch();
    bgVlay->addLayout(tileRow);

    connect(bgSubtractChk_, &QCheckBox::toggled, this, [this](bool on){
        bgTileSizeSpin_->setEnabled(on);
    });

    vlay->addWidget(bgGroup);

    // Star detection (advanced)
    auto* detGroup = new QGroupBox(tr("Star Detection"), page);
    auto* detForm  = new QFormLayout(detGroup);
    detForm->setContentsMargins(12,12,12,12); detForm->setSpacing(8);
    detForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    detForm->addRow(makeAdvancedWarning(detGroup));

    sigmaLimitSpin_ = new QDoubleSpinBox(detGroup);
    sigmaLimitSpin_->setRange(1.0,20.0); sigmaLimitSpin_->setDecimals(1);
    sigmaLimitSpin_->setSuffix(QStringLiteral("  σ"));
    sigmaLimitSpin_->setToolTip(tr("Detection threshold: source must be this many sigma above "
        "the local background. Lower = more detections, more false positives."));
    detForm->addRow(tr("Detection threshold:"), sigmaLimitSpin_);

    minFwhmSpin_ = new QDoubleSpinBox(detGroup);
    minFwhmSpin_->setRange(0.1,10.0); minFwhmSpin_->setDecimals(2);
    minFwhmSpin_->setSuffix(QStringLiteral("  px"));
    minFwhmSpin_->setToolTip(tr("Minimum FWHM for a source to be accepted as a star. "
        "Sources smaller than this are likely cosmic rays."));
    detForm->addRow(tr("Minimum FWHM:"), minFwhmSpin_);

    minSnrSpin_ = new QDoubleSpinBox(detGroup);
    minSnrSpin_->setRange(1.0, 50.0); minSnrSpin_->setDecimals(1);
    minSnrSpin_->setSuffix(QStringLiteral("  σ"));
    minSnrSpin_->setToolTip(tr("Minimum signal-to-noise ratio for moving object detection (MOD). "
        "Higher values reduce false positives but may miss faint asteroids."));
    detForm->addRow(tr("MOD min. SNR:"), minSnrSpin_);

    streakElongSpin_ = new QDoubleSpinBox(detGroup);
    streakElongSpin_->setRange(1.5, 20.0); streakElongSpin_->setDecimals(1); streakElongSpin_->setSingleStep(0.5);
    streakElongSpin_->setToolTip(tr("Elongation threshold (a/b) above which a detected source is "
        "classified as a streak or trail (fast asteroid, satellite, tracking smear). "
        "Streaks are shown in orange in the overlay. Set to 1.5 to catch mild tracking drift; "
        "3.0 catches only clear trails. Higher values = fewer, more obvious streaks only."));
    detForm->addRow(tr("Streak threshold (a/b):"), streakElongSpin_);

    vlay->addWidget(detGroup);

    // Photometry
    auto* photGroup = new QGroupBox(tr("Photometry"), page);
    auto* photForm  = new QFormLayout(photGroup);
    photForm->setContentsMargins(12,12,12,12); photForm->setSpacing(8);
    photForm->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    bandCombo_ = new QComboBox(photGroup);
    bandCombo_->addItems({ QStringLiteral("C — Clear / unfiltered"),
                           QStringLiteral("V — Johnson V"),
                           QStringLiteral("R — Cousins R"),
                           QStringLiteral("B — Johnson B"),
                           QStringLiteral("g — Sloan g'"),
                           QStringLiteral("r — Sloan r'"),
                           QStringLiteral("i — Sloan i'"),
                           QStringLiteral("w — Pan-STARRS w") });
    bandCombo_->setToolTip(tr("Default photometric band used when no filter information "
        "is found in the FITS header."));
    photForm->addRow(tr("Default band:"), bandCombo_);

    magLimitSpin_ = new QDoubleSpinBox(photGroup);
    magLimitSpin_->setRange(10.0,21.0); magLimitSpin_->setDecimals(1);
    magLimitSpin_->setSuffix(QStringLiteral("  mag  (faint limit)"));
    magLimitSpin_->setToolTip(tr("Faintest catalog stars downloaded for astrometric and "
        "photometric calibration."));
    photForm->addRow(tr("Catalog mag (faint):"), magLimitSpin_);

    maxMagSpin_ = new QDoubleSpinBox(photGroup);
    maxMagSpin_->setRange(0.0,15.0); maxMagSpin_->setDecimals(1);
    maxMagSpin_->setSuffix(QStringLiteral("  mag  (bright limit)"));
    maxMagSpin_->setToolTip(tr("Brightest catalog stars used for calibration. "
        "Very bright stars are often saturated and should be excluded."));
    photForm->addRow(tr("Catalog mag (bright):"), maxMagSpin_);

    includeMagChk_ = new QCheckBox(tr("Include magnitude in ADES report"), photGroup);
    includeMagChk_->setToolTip(tr("When checked, the measured magnitude is included "
        "in every observation line of the ADES report."));
    photForm->addRow(includeMagChk_);

    // Aperture sub-group (advanced)
    auto* apGroup = new QGroupBox(tr("Aperture Radius"), photGroup);
    auto* apVlay  = new QVBoxLayout(apGroup);
    apVlay->setContentsMargins(10,10,10,10); apVlay->setSpacing(5);

    apertureAutoRad_ = new QRadioButton(tr("Automatic  (2 × FWHM — recommended)"), apGroup);
    apertureManRad_  = new QRadioButton(tr("Fixed:"), apGroup);
    apertureManSpin_ = new QDoubleSpinBox(apGroup);
    apertureManSpin_->setRange(1.0,50.0); apertureManSpin_->setDecimals(1);
    apertureManSpin_->setSuffix(QStringLiteral(" px")); apertureManSpin_->setEnabled(false);

    auto* manRow = new QHBoxLayout;
    manRow->setContentsMargins(18,0,0,0);
    manRow->addWidget(apertureManRad_); manRow->addWidget(apertureManSpin_); manRow->addStretch();

    apVlay->addWidget(apertureAutoRad_);
    apVlay->addLayout(manRow);
    apVlay->addWidget(makeAdvancedWarning(apGroup));

    connect(apertureAutoRad_, &QRadioButton::toggled, this, &SettingsDialog::onApertureToggled);
    connect(apertureManRad_,  &QRadioButton::toggled, this, &SettingsDialog::onApertureToggled);

    photForm->addRow(apGroup);

    // Atmospheric extinction coefficient
    extinctionSpin_ = new QDoubleSpinBox(photGroup);
    extinctionSpin_->setRange(0.0, 1.0);
    extinctionSpin_->setDecimals(3);
    extinctionSpin_->setSingleStep(0.01);
    extinctionSpin_->setSuffix(QStringLiteral(" mag/airmass"));
    extinctionSpin_->setToolTip(tr(
        "First-order atmospheric extinction coefficient k.\n"
        "Corrected magnitude = raw mag \xe2\x88\x92 k \xc3\x97 X  (X = airmass).\n"
        "Typical values: V \xe2\x89\x88 0.15, R \xe2\x89\x88 0.11, B \xe2\x89\x88 0.25.\n"
        "Set to 0 to disable correction."));
    photForm->addRow(tr("Extinction coeff k:"), extinctionSpin_);

    vlay->addWidget(photGroup);
    vlay->addStretch();

    tabs->addTab(scrolled(page), tr("Detection"));
}

// ─── Tab 5 — Display ─────────────────────────────────────────────────────────

void SettingsDialog::buildDisplayTab(QTabWidget* tabs)
{
    auto* page = new QWidget;
    auto* form = stdForm(page);

    blinkIntervalSpin_ = new QSpinBox(page);
    blinkIntervalSpin_->setRange(100,2000); blinkIntervalSpin_->setSuffix(QStringLiteral("  ms"));
    blinkIntervalSpin_->setToolTip(tr("Default interval between images in blink mode."));
    form->addRow(tr("Blink interval:"), blinkIntervalSpin_);

    themeCombo_ = new QComboBox(page);
    themeCombo_->addItem(tr("Night (dark)"),          QStringLiteral("night"));
    themeCombo_->addItem(tr("Day (light)"),            QStringLiteral("day"));
    themeCombo_->addItem(tr("Auto (follow system)"),   QStringLiteral("auto"));
    themeCombo_->setToolTip(tr("Colour theme. Auto follows the operating system dark/light setting."));
    form->addRow(tr("Theme:"), themeCombo_);

    showWizardCheck_ = new QCheckBox(tr("Show setup wizard when the application starts"), page);
    showWizardCheck_->setToolTip(
        tr("Uncheck this once the initial configuration is complete.\n"
           "The wizard is always available via Help → Setup Wizard."));
    form->addRow(showWizardCheck_);

    showColorWarningCheck_ = new QCheckBox(tr("Avisar ao carregar se imagens são PB ou coloridas"), page);
    showColorWarningCheck_->setToolTip(
        tr("Exibe um aviso informando se as imagens carregadas são em preto e branco\n"
           "ou coloridas (NAXIS3=3). Pode ser desativado também no próprio aviso."));
    form->addRow(showColorWarningCheck_);

    langCombo_ = new QComboBox(page);
    langCombo_->addItem(QStringLiteral("English"),        QStringLiteral("en"));
    langCombo_->addItem(QStringLiteral("Português (BR)"), QStringLiteral("pt_BR"));
    langCombo_->setToolTip(tr("Interface language — takes effect after restarting AstroFind."));
    auto* langNote = new QLabel(tr("⟳ Restart required to apply language change."), page);
    langNote->setStyleSheet(QStringLiteral("color:#8899aa; font-size:10px;"));
    form->addRow(tr("Language:"), langCombo_);
    form->addRow(langNote);

    tabs->addTab(page, tr("Display"));
}

// ─── Tab 6 — Legacy Catalogs ─────────────────────────────────────────────────

void SettingsDialog::buildLegacyTab(QTabWidget* tabs)
{
    auto* page = new QWidget;
    auto* vlay = new QVBoxLayout(page);
    vlay->setContentsMargins(16,14,16,14); vlay->setSpacing(10);

    vlay->addWidget(makeDeprecatedWarning(page));

    auto* form = new QFormLayout;
    form->setSpacing(9);
    form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    auto makeDirRow = [&](QLineEdit*& edit, const QString& label, const QString& tip) {
        edit = new QLineEdit(page);
        edit->setToolTip(tip);
        edit->setPlaceholderText(tr("(not configured)"));
        form->addRow(label, browseRow(edit, this,
            [this, &edit]{ onBrowseLegacyDir(edit); }, page));
    };

    makeDirRow(usnoA2DirEdit_, tr("USNO-A2.0 directory:"),
        tr("Local directory containing USNO-A2.0 catalog zones (*.cat). "
           "USNO-A2.0 is obsolete; use VizieR for UCAC4 instead."));
    makeDirRow(ucac2DirEdit_, tr("UCAC-2 directory:"),
        tr("Local directory for UCAC-2 catalog files. "
           "UCAC-2 is obsolete; UCAC-4 via VizieR is the current standard."));
    makeDirRow(ucac3DirEdit_, tr("UCAC-3 directory:"),
        tr("Local directory for UCAC-3 catalog files. Superseded by UCAC-4."));
    makeDirRow(cmc14DirEdit_, tr("CMC-14 directory:"),
        tr("Local directory for Carlsberg Meridian Catalogue 14. Superseded by Gaia DR3."));

    auto* footNote = new QLabel(
        tr("These catalogs require several GB of disk space and are no longer "
           "updated by their publishers. Configure only if you have no internet "
           "access and the files are already available locally."), page);
    footNote->setWordWrap(true);
    footNote->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));

    vlay->addLayout(form);
    vlay->addSpacing(8);
    vlay->addWidget(footNote);
    vlay->addStretch();

    tabs->addTab(page, tr("Legacy"));
}

// ─── Load ────────────────────────────────────────────────────────────────────

void SettingsDialog::loadFromSettings()
{
    // Observer
    mpcCodeEdit_->setText(settings_.value(QStringLiteral("observer/mpcCode")).toString());
    observerEdit_->setText(settings_.value(QStringLiteral("observer/name")).toString());
    measurerEdit_->setText(settings_.value(QStringLiteral("observer/measurer")).toString());
    telescopeEdit_->setText(settings_.value(QStringLiteral("observer/telescope")).toString());

    // Location mode
    const QString mode = settings_.value(
        QStringLiteral("observer/locationMode"), QStringLiteral("manual")).toString();
    if (mode == QStringLiteral("fits"))
        locFitsRad_->setChecked(true);
    else if (mode == QStringLiteral("preset"))
        locPresetRad_->setChecked(true);
    else if (mode == QStringLiteral("spacecraft"))
        locSpacecraftRad_->setChecked(true);
    else
        locManualRad_->setChecked(true);

    // Manual coordinates
    latitudeSpin_->setValue(settings_.value(QStringLiteral("observer/latitude"),  0.0).toDouble());
    longitudeSpin_->setValue(settings_.value(QStringLiteral("observer/longitude"), 0.0).toDouble());
    altitudeSpin_->setValue(settings_.value(QStringLiteral("observer/altitude"),   0).toInt());

    // Preset: country + observatory
    const QString savedCode = settings_.value(
        QStringLiteral("observer/presetMpcCode"), QString()).toString();
    if (!savedCode.isEmpty()) {
        // Determine country from the saved MPC code
        const QByteArray codeBA = savedCode.toLatin1();
        if (const Observatory* o = ObservatoryDatabase::byCode(codeBA.constData())) {
            const QString cc = QString::fromLatin1(o->country);
            const int ci = countryCombo_->findData(cc);
            if (ci >= 0) {
                countryCombo_->setCurrentIndex(ci);
                populateObservatoryCombo(cc);
                // Select the right observatory
                for (int i = 0; i < obsCombo_->count(); ++i) {
                    if (obsCombo_->itemData(i).toString() == savedCode) {
                        obsCombo_->setCurrentIndex(i);
                        break;
                    }
                }
            }
        }
    }

    // Trigger UI update
    updateLocationWidgets();
    onLocationModeChanged();

    timeZoneSpin_->setValue(settings_.value(QStringLiteral("observer/timeZone"),   0.0).toDouble());
    timeOffsetSpin_->setValue(settings_.value(QStringLiteral("observer/timeOffset"), 0.0).toDouble());
    timePrecSpin_->setValue(settings_.value(QStringLiteral("observer/timePrecision"), 1.0).toDouble());
    contact1Edit_->setText(settings_.value(QStringLiteral("observer/contact1")).toString());
    contact2Edit_->setText(settings_.value(QStringLiteral("observer/contact2")).toString());
    emailEdit_->setText(settings_.value(QStringLiteral("observer/email")).toString());
    inclContactChk_->setChecked(settings_.value(QStringLiteral("observer/includeContact"), false).toBool());
    schoolEmailEdit_->setText(settings_.value(QStringLiteral("school/recipientEmail")).toString());

    // Camera
    pixScaleXSpin_->setValue(settings_.value(QStringLiteral("camera/pixelScaleX"), 0.0).toDouble());
    pixScaleYSpin_->setValue(settings_.value(QStringLiteral("camera/pixelScaleY"), 0.0).toDouble());
    focalLenSpin_->setValue(settings_.value(QStringLiteral("camera/focalLength"),  0.0).toDouble());
    saturationSpin_->setValue(settings_.value(QStringLiteral("camera/saturation"), 60000).toInt());
    deltaTSpin_->setValue(settings_.value(QStringLiteral("camera/deltaT"),          68.0).toDouble());
    {
        const bool bpOn = settings_.value(QStringLiteral("camera/badPixelCorrection"), true).toBool();
        badPixelChk_->setChecked(bpOn);
        badPixelSigmaSpin_->setValue(settings_.value(QStringLiteral("camera/badPixelSigma"), 5.0).toDouble());
        badPixelSigmaSpin_->setEnabled(bpOn);
        badPixelWarnWidget_->setVisible(!bpOn);
    }

    // Connections — plate solving
    {
        const QString backend = settings_.value(QStringLiteral("astrometry/backend"),
                                                 QStringLiteral("astrometry_net")).toString();
        const int idx = solverBackendCombo_->findData(backend);
        solverBackendCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
    }
    astapPathEdit_->setText(settings_.value(QStringLiteral("astrometry/astapPath")).toString());
    apiKeyEdit_->setText(core::ApiKeyStore::read());
    serverUrlEdit_->setText(settings_.value(QStringLiteral("astrometry/baseUrl"),
        QStringLiteral("https://nova.astrometry.net")).toString());
    timeoutSpin_->setValue(settings_.value(QStringLiteral("astrometry/timeoutSec"), 300).toInt());

    const QString defMpc = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                         + QStringLiteral("/MPCORB.DAT");
    mpcOrbPathEdit_->setText(settings_.value(QStringLiteral("catalog/mpcOrbPath"), defMpc).toString());

    const QString defCcd = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    ccdDirEdit_->setText(settings_.value(QStringLiteral("paths/ccdDir"), defCcd).toString());

    const QString defRep = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    reportDirEdit_->setText(settings_.value(QStringLiteral("report/outputDir"), defRep).toString());

    saveWcsChk_->setChecked(settings_.value(QStringLiteral("astrometry/saveWcs"), false).toBool());
    {
        const QString ct = settings_.value(QStringLiteral("catalog/type"),
                                            QStringLiteral("UCAC4")).toString();
        const int ci = catalogTypeCombo_->findData(ct);
        catalogTypeCombo_->setCurrentIndex(ci >= 0 ? ci : 0);
    }
    vizierEdit_->setText(settings_.value(QStringLiteral("catalog/vizierServer"),
        QStringLiteral("vizier.cfa.harvard.edu")).toString());
    mpcSubmitEdit_->setText(settings_.value(QStringLiteral("mpc/submitUrl"),
        QStringLiteral("https://www.minorplanetcenter.net/report_ades")).toString());
    {
        const QString src = settings_.value(QStringLiteral("catalog/source"),
                                            QStringLiteral("vizier")).toString();
        const int idx = catalogSourceCombo_->findData(src);
        catalogSourceCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
        const bool isLocal = (src == "local");
        localCatalogPathEdit_->setEnabled(isLocal);
        if (localCatalogBrowse_) localCatalogBrowse_->setEnabled(isLocal);
    }
    localCatalogPathEdit_->setText(
        settings_.value(QStringLiteral("catalog/localPath")).toString());

    // Detection
    {
        const bool bgOn = settings_.value(QStringLiteral("detection/backgroundSubtraction"), false).toBool();
        bgSubtractChk_->setChecked(bgOn);
        bgTileSizeSpin_->setValue(settings_.value(QStringLiteral("detection/backgroundTileSize"), 64).toInt());
        bgTileSizeSpin_->setEnabled(bgOn);
    }
    sigmaLimitSpin_->setValue(settings_.value(QStringLiteral("detection/sigmaLimit"), 4.0).toDouble());
    minFwhmSpin_->setValue(settings_.value(QStringLiteral("detection/minFwhm"), 0.70).toDouble());
    minSnrSpin_->setValue(settings_.value(QStringLiteral("detection/minSnr"), 5.0).toDouble());
    streakElongSpin_->setValue(settings_.value(QStringLiteral("detection/streakElongation"), 3.0).toDouble());

    const QString band = settings_.value(QStringLiteral("photometry/defaultBand"),
                                          QStringLiteral("C")).toString();
    static const QStringList kBands = {"C","V","R","B","g","r","i","w"};
    const int bi = static_cast<int>(kBands.indexOf(band));
    bandCombo_->setCurrentIndex(bi >= 0 ? bi : 0);

    magLimitSpin_->setValue(settings_.value(QStringLiteral("catalog/magLimit"),  16.0).toDouble());
    maxMagSpin_->setValue(settings_.value(QStringLiteral("catalog/maxMag"),       10.0).toDouble());
    includeMagChk_->setChecked(settings_.value(QStringLiteral("report/includeMag"), true).toBool());

    const bool apAuto = settings_.value(QStringLiteral("photometry/apertureAuto"), true).toBool();
    apertureAutoRad_->setChecked(apAuto);
    apertureManRad_->setChecked(!apAuto);
    apertureManSpin_->setValue(settings_.value(QStringLiteral("photometry/apertureManPx"), 8.0).toDouble());
    apertureManSpin_->setEnabled(!apAuto);
    extinctionSpin_->setValue(settings_.value(QStringLiteral("photometry/extinctionCoeff"), 0.0).toDouble());

    // Display
    blinkIntervalSpin_->setValue(settings_.value(QStringLiteral("display/blinkIntervalMs"), 500).toInt());
    {
        const QString theme = settings_.value(QStringLiteral("ui/theme"), QStringLiteral("night")).toString();
        const int tidx = themeCombo_->findData(theme);
        themeCombo_->setCurrentIndex(tidx >= 0 ? tidx : 0);
    }
    showWizardCheck_->setChecked(settings_.value(QStringLiteral("display/showWizardOnStartup"), true).toBool());
    showColorWarningCheck_->setChecked(settings_.value(QStringLiteral("display/showColorTypeWarning"), true).toBool());
    {
        const QString lang = settings_.value(QStringLiteral("ui/language"),
                                             QLocale::system().name()).toString();
        const int idx = langCombo_->findData(lang.left(5));  // match "pt_BR" or "en"
        langCombo_->setCurrentIndex(idx >= 0 ? idx : 0);
    }

    // Legacy
    usnoA2DirEdit_->setText(settings_.value(QStringLiteral("legacy/usnoA2Dir")).toString());
    ucac2DirEdit_->setText(settings_.value(QStringLiteral("legacy/ucac2Dir")).toString());
    ucac3DirEdit_->setText(settings_.value(QStringLiteral("legacy/ucac3Dir")).toString());
    cmc14DirEdit_->setText(settings_.value(QStringLiteral("legacy/cmc14Dir")).toString());
}

// ─── Save ────────────────────────────────────────────────────────────────────

void SettingsDialog::saveToSettings()
{
    // Observer
    settings_.setValue(QStringLiteral("observer/mpcCode"),       mpcCodeEdit_->text().trimmed().toUpper());
    settings_.setValue(QStringLiteral("observer/name"),          observerEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("observer/measurer"),      measurerEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("observer/telescope"),     telescopeEdit_->text().trimmed());

    // Location mode
    const int modeId = locationBtnGroup_->checkedId();
    const QString modeStr = (modeId == 0) ? QStringLiteral("fits")
                          : (modeId == 1) ? QStringLiteral("preset")
                          : (modeId == 3) ? QStringLiteral("spacecraft")
                                          : QStringLiteral("manual");
    settings_.setValue(QStringLiteral("observer/locationMode"), modeStr);

    // When in spacecraft mode, ensure the spacecraft MPC code is propagated
    if (modeId == 3 && !spacecraftMpcCode_.isEmpty())
        settings_.setValue(QStringLiteral("observer/mpcCode"), spacecraftMpcCode_);

    // Manual coordinates (always saved — used as fallback)
    settings_.setValue(QStringLiteral("observer/latitude"),      latitudeSpin_->value());
    settings_.setValue(QStringLiteral("observer/longitude"),     longitudeSpin_->value());
    settings_.setValue(QStringLiteral("observer/altitude"),      altitudeSpin_->value());

    // Preset MPC code
    const QString presetCode = obsCombo_->count() > 0
        ? obsCombo_->currentData().toString()
        : QString();
    settings_.setValue(QStringLiteral("observer/presetMpcCode"), presetCode);

    settings_.setValue(QStringLiteral("observer/timeZone"),      timeZoneSpin_->value());
    settings_.setValue(QStringLiteral("observer/timeOffset"),    timeOffsetSpin_->value());
    settings_.setValue(QStringLiteral("observer/timePrecision"), timePrecSpin_->value());
    settings_.setValue(QStringLiteral("observer/contact1"),      contact1Edit_->text().trimmed());
    settings_.setValue(QStringLiteral("observer/contact2"),      contact2Edit_->text().trimmed());
    settings_.setValue(QStringLiteral("observer/email"),         emailEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("observer/includeContact"),inclContactChk_->isChecked());
    settings_.setValue(QStringLiteral("school/recipientEmail"), schoolEmailEdit_->text().trimmed());

    // Camera
    settings_.setValue(QStringLiteral("camera/pixelScaleX"), pixScaleXSpin_->value());
    settings_.setValue(QStringLiteral("camera/pixelScaleY"), pixScaleYSpin_->value());
    settings_.setValue(QStringLiteral("camera/focalLength"), focalLenSpin_->value());
    settings_.setValue(QStringLiteral("camera/saturation"),         saturationSpin_->value());
    settings_.setValue(QStringLiteral("camera/deltaT"),             deltaTSpin_->value());
    settings_.setValue(QStringLiteral("camera/badPixelCorrection"), badPixelChk_->isChecked());
    settings_.setValue(QStringLiteral("camera/badPixelSigma"),      badPixelSigmaSpin_->value());

    // Connections
    settings_.setValue(QStringLiteral("astrometry/backend"),    solverBackendCombo_->currentData().toString());
    settings_.setValue(QStringLiteral("astrometry/astapPath"),  astapPathEdit_->text().trimmed());
    core::ApiKeyStore::write(apiKeyEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("astrometry/baseUrl"),    serverUrlEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("astrometry/timeoutSec"), timeoutSpin_->value());
    settings_.setValue(QStringLiteral("catalog/mpcOrbPath"),    mpcOrbPathEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("paths/ccdDir"),          ccdDirEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("report/outputDir"),      reportDirEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("astrometry/saveWcs"),    saveWcsChk_->isChecked());
    settings_.setValue(QStringLiteral("catalog/type"),
                       catalogTypeCombo_->currentData().toString());
    settings_.setValue(QStringLiteral("catalog/vizierServer"),  vizierEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("mpc/submitUrl"),         mpcSubmitEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("catalog/source"),
        catalogSourceCombo_->currentData().toString());
    settings_.setValue(QStringLiteral("catalog/localPath"),
        localCatalogPathEdit_->text().trimmed());

    // Detection
    settings_.setValue(QStringLiteral("detection/backgroundSubtraction"), bgSubtractChk_->isChecked());
    settings_.setValue(QStringLiteral("detection/backgroundTileSize"),    bgTileSizeSpin_->value());
    settings_.setValue(QStringLiteral("detection/sigmaLimit"),      sigmaLimitSpin_->value());
    settings_.setValue(QStringLiteral("detection/minFwhm"),         minFwhmSpin_->value());
    settings_.setValue(QStringLiteral("detection/minSnr"),          minSnrSpin_->value());
    settings_.setValue(QStringLiteral("detection/streakElongation"), streakElongSpin_->value());

    static const QStringList kBands = {"C","V","R","B","g","r","i","w"};
    const int bi = bandCombo_->currentIndex();
    settings_.setValue(QStringLiteral("photometry/defaultBand"),
                       (bi >= 0 && bi < kBands.size()) ? kBands[bi] : QStringLiteral("C"));
    settings_.setValue(QStringLiteral("catalog/magLimit"),           magLimitSpin_->value());
    settings_.setValue(QStringLiteral("catalog/maxMag"),             maxMagSpin_->value());
    settings_.setValue(QStringLiteral("report/includeMag"),          includeMagChk_->isChecked());
    settings_.setValue(QStringLiteral("photometry/apertureAuto"),    apertureAutoRad_->isChecked());
    settings_.setValue(QStringLiteral("photometry/apertureManPx"),   apertureManSpin_->value());
    settings_.setValue(QStringLiteral("photometry/extinctionCoeff"), extinctionSpin_->value());

    // Display
    settings_.setValue(QStringLiteral("display/blinkIntervalMs"),     blinkIntervalSpin_->value());
    settings_.setValue(QStringLiteral("ui/theme"),                     themeCombo_->currentData().toString());
    settings_.setValue(QStringLiteral("display/showWizardOnStartup"),  showWizardCheck_->isChecked());
    settings_.setValue(QStringLiteral("display/showColorTypeWarning"), showColorWarningCheck_->isChecked());
    settings_.setValue(QStringLiteral("ui/language"),                  langCombo_->currentData().toString());

    // Legacy
    settings_.setValue(QStringLiteral("legacy/usnoA2Dir"), usnoA2DirEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("legacy/ucac2Dir"),  ucac2DirEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("legacy/ucac3Dir"),  ucac3DirEdit_->text().trimmed());
    settings_.setValue(QStringLiteral("legacy/cmc14Dir"),  cmc14DirEdit_->text().trimmed());
}

// ─── Reset to defaults ────────────────────────────────────────────────────────

void SettingsDialog::resetToDefaults()
{
    // Observer — preserve identity fields, reset numeric/optional
    measurerEdit_->clear();

    // Location: reset to manual, zero coordinates
    locManualRad_->setChecked(true);
    latitudeSpin_->setValue(0.0); longitudeSpin_->setValue(0.0); altitudeSpin_->setValue(0);
    settings_.remove(QStringLiteral("observer/presetMpcCode"));
    onLocationModeChanged();

    timeZoneSpin_->setValue(0.0); timeOffsetSpin_->setValue(0.0); timePrecSpin_->setValue(1.0);
    inclContactChk_->setChecked(false);

    // Camera
    pixScaleXSpin_->setValue(0.0); pixScaleYSpin_->setValue(0.0);
    focalLenSpin_->setValue(0.0); saturationSpin_->setValue(60000); deltaTSpin_->setValue(68.0);
    badPixelChk_->setChecked(true); badPixelSigmaSpin_->setValue(5.0);
    badPixelSigmaSpin_->setEnabled(true); badPixelWarnWidget_->setVisible(false);

    // Connections
    serverUrlEdit_->setText(QStringLiteral("https://nova.astrometry.net"));
    timeoutSpin_->setValue(300);
    mpcOrbPathEdit_->setText(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/MPCORB.DAT"));
    ccdDirEdit_->setText(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation));
    reportDirEdit_->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    saveWcsChk_->setChecked(false);
    catalogTypeCombo_->setCurrentIndex(0);  // UCAC4
    vizierEdit_->setText(QStringLiteral("vizier.cfa.harvard.edu"));
    mpcSubmitEdit_->setText(QStringLiteral("https://www.minorplanetcenter.net/report_ades"));

    // Detection
    bgSubtractChk_->setChecked(false); bgTileSizeSpin_->setValue(64); bgTileSizeSpin_->setEnabled(false);
    sigmaLimitSpin_->setValue(4.0); minFwhmSpin_->setValue(0.70); minSnrSpin_->setValue(5.0);
    streakElongSpin_->setValue(3.0);
    bandCombo_->setCurrentIndex(0); magLimitSpin_->setValue(16.0); maxMagSpin_->setValue(10.0);
    includeMagChk_->setChecked(true);
    apertureAutoRad_->setChecked(true); apertureManSpin_->setValue(8.0); apertureManSpin_->setEnabled(false);
    extinctionSpin_->setValue(0.0);

    // Display
    blinkIntervalSpin_->setValue(500);
    themeCombo_->setCurrentIndex(0);  // Night
    showWizardCheck_->setChecked(true);
    showColorWarningCheck_->setChecked(true);

    // School
    schoolEmailEdit_->clear();

    // Legacy — clear
    usnoA2DirEdit_->clear(); ucac2DirEdit_->clear(); ucac3DirEdit_->clear(); cmc14DirEdit_->clear();
}

// ─── Slots ────────────────────────────────────────────────────────────────────

void SettingsDialog::onAccept()           { saveToSettings(); accept(); }
void SettingsDialog::onResetDefaults()    { resetToDefaults(); }

void SettingsDialog::onToggleApiKeyVisibility()
{
    const bool show = showKeyBtn_->isChecked();
    apiKeyEdit_->setEchoMode(show ? QLineEdit::Normal : QLineEdit::Password);
    showKeyBtn_->setText(show ? tr("Hide") : tr("Show"));
}

void SettingsDialog::onBrowseMpcOrb()
{
    const QString p = QFileDialog::getOpenFileName(this, tr("Select MPCORB.DAT"),
        mpcOrbPathEdit_->text(), QStringLiteral("MPCORB (MPCORB.DAT);;All files (*)"));
    if (!p.isEmpty()) mpcOrbPathEdit_->setText(p);
}

void SettingsDialog::onBrowseReportDir()
{
    const QString p = QFileDialog::getExistingDirectory(this,
        tr("Select report output folder"), reportDirEdit_->text());
    if (!p.isEmpty()) reportDirEdit_->setText(p);
}

void SettingsDialog::onBrowseCcdDir()
{
    const QString p = QFileDialog::getExistingDirectory(this,
        tr("Select default image folder"), ccdDirEdit_->text());
    if (!p.isEmpty()) ccdDirEdit_->setText(p);
}

void SettingsDialog::onBrowseLegacyDir(QLineEdit* target)
{
    const QString p = QFileDialog::getExistingDirectory(this,
        tr("Select catalog directory"), target->text());
    if (!p.isEmpty()) target->setText(p);
}

void SettingsDialog::onBrowseLocalCatalog()
{
    const QString path = QFileDialog::getOpenFileName(this,
        tr("Select Local Star Catalog"), QDir::homePath(),
        tr("FITS BINTABLE (*.fits *.fit *.fts);;All files (*)"));
    if (!path.isEmpty()) localCatalogPathEdit_->setText(path);
}

void SettingsDialog::onApertureToggled()
{
    apertureManSpin_->setEnabled(apertureManRad_->isChecked());
}

void SettingsDialog::onLocationModeChanged()
{
    const int mode = locationBtnGroup_->checkedId();
    fitsSubWidget_->setVisible(mode == 0);
    presetSubWidget_->setVisible(mode == 1);
    manualSubWidget_->setVisible(mode == 2);
    spacecraftSubWidget_->setVisible(mode == 3);

    // If "fits" mode is selected but FITS location is not available, switch to manual
    if (mode == 0 && (std::isnan(fitsLat_) || std::isnan(fitsLon_))) {
        locManualRad_->setChecked(true);
        return;
    }

    // Enable/disable the manual spinboxes based on mode
    const bool manualActive = (mode == 2);
    latitudeSpin_->setEnabled(manualActive);
    longitudeSpin_->setEnabled(manualActive);
    altitudeSpin_->setEnabled(manualActive);
}

void SettingsDialog::onCountryChanged(int idx)
{
    if (idx < 0) {
        obsCombo_->clear();
        obsCombo_->setEnabled(false);
        presetCoordLabel_->clear();
        return;
    }
    const QString cc = countryCombo_->itemData(idx).toString();
    populateObservatoryCombo(cc);
    updateLocationWidgets();
}
