#include "HorizonsQueryDialog.h"

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QNetworkAccessManager>
#include <QApplication>

static QString raToHms(double raDeg)
{
    const double h  = raDeg / 15.0;
    const int    hh = static_cast<int>(h);
    const double mf = (h - hh) * 60.0;
    const int    mm = static_cast<int>(mf);
    const double ss = (mf - mm) * 60.0;
    return QString("%1h %2m %3s")
        .arg(hh, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 5, 'f', 2, QLatin1Char('0'));
}

static QString decToDms(double decDeg)
{
    const char   sign = (decDeg >= 0) ? '+' : '-';
    const double a    = std::abs(decDeg);
    const int    dd   = static_cast<int>(a);
    const double mf   = (a - dd) * 60.0;
    const int    mm   = static_cast<int>(mf);
    const double ss   = (mf - mm) * 60.0;
    return QString("%1%2° %3' %4\"")
        .arg(sign)
        .arg(dd, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 4, 'f', 1, QLatin1Char('0'));
}

HorizonsQueryDialog::HorizonsQueryDialog(QNetworkAccessManager* nam,
                                          double jd,
                                          QWidget* parent)
    : QDialog(parent), jd_(jd)
{
    setWindowTitle(tr("Query JPL Horizons"));
    setMinimumWidth(380);

    client_ = new core::HorizonsClient(nam, this);
    connect(client_, &core::HorizonsClient::resultReady,
            this, &HorizonsQueryDialog::onResultReady);
    connect(client_, &core::HorizonsClient::failed,
            this, &HorizonsQueryDialog::onFailed);

    // ── Query row ─────────────────────────────────────────────────────────
    auto* targetLabel = new QLabel(tr("Target:"), this);
    targetEdit_ = new QLineEdit(this);
    targetEdit_->setPlaceholderText(tr("e.g.  433,  Eros,  2023 DW"));
    targetEdit_->setToolTip(tr(
        "MPC number, name, or provisional designation.\n"
        "Examples:  433  |  Eros  |  2023 DW  |  C/2020 F3"));

    queryBtn_ = new QPushButton(tr("Query"), this);
    queryBtn_->setDefault(true);

    auto* queryRow = new QHBoxLayout;
    queryRow->addWidget(targetLabel);
    queryRow->addWidget(targetEdit_, 1);
    queryRow->addWidget(queryBtn_);

    // JD label
    auto* jdLabel = new QLabel(
        tr("Epoch: JD %1").arg(jd_, 0, 'f', 4), this);
    jdLabel->setStyleSheet(QStringLiteral("color: gray; font-size: 9pt;"));

    // Status
    statusLabel_ = new QLabel(this);
    statusLabel_->setWordWrap(true);

    // ── Result group ──────────────────────────────────────────────────────
    resultGroup_ = new QGroupBox(tr("Result"), this);
    resultGroup_->setVisible(false);

    nameLabel_   = new QLabel(this);
    raLabel_     = new QLabel(this);
    decLabel_    = new QLabel(this);
    magLabel_    = new QLabel(this);
    motionLabel_ = new QLabel(this);

    auto* form = new QFormLayout(resultGroup_);
    form->addRow(tr("Object:"),  nameLabel_);
    form->addRow(tr("RA:"),      raLabel_);
    form->addRow(tr("Dec:"),     decLabel_);
    form->addRow(tr("Mag:"),     magLabel_);
    form->addRow(tr("Motion:"),  motionLabel_);

    addOverlayBtn_ = new QPushButton(tr("Add to Overlay"), this);
    addOverlayBtn_->setVisible(false);

    // ── Close button ──────────────────────────────────────────────────────
    auto* closeBtn = new QPushButton(tr("Close"), this);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);

    auto* btnRow = new QHBoxLayout;
    btnRow->addWidget(addOverlayBtn_);
    btnRow->addStretch();
    btnRow->addWidget(closeBtn);

    // ── Main layout ───────────────────────────────────────────────────────
    auto* lay = new QVBoxLayout(this);
    lay->addLayout(queryRow);
    lay->addWidget(jdLabel);
    lay->addWidget(statusLabel_);
    lay->addWidget(resultGroup_);
    lay->addLayout(btnRow);

    // ── Connections ───────────────────────────────────────────────────────
    connect(queryBtn_,     &QPushButton::clicked, this, &HorizonsQueryDialog::onQuery);
    connect(targetEdit_,   &QLineEdit::returnPressed, this, &HorizonsQueryDialog::onQuery);
    connect(addOverlayBtn_, &QPushButton::clicked, this, [this] {
        if (lastMatch_)
            emit overlayRequested(*lastMatch_);
    });
}

void HorizonsQueryDialog::onQuery()
{
    const QString target = targetEdit_->text().trimmed();
    if (target.isEmpty()) return;
    if (client_->isBusy()) return;

    resultGroup_->setVisible(false);
    addOverlayBtn_->setVisible(false);
    lastMatch_.reset();

    statusLabel_->setText(tr("Querying Horizons for \"%1\"…").arg(target));
    queryBtn_->setEnabled(false);
    QApplication::processEvents();

    client_->query(target, jd_);
}

void HorizonsQueryDialog::onResultReady(const core::EphemerisMatch& match)
{
    queryBtn_->setEnabled(true);
    statusLabel_->clear();
    lastMatch_ = match;

    nameLabel_->setText(match.name.isEmpty() ? targetEdit_->text() : match.name);
    raLabel_->setText(raToHms(match.ra)
                      + QStringLiteral("  (%1°)").arg(match.ra, 0, 'f', 5));
    decLabel_->setText(decToDms(match.dec)
                       + QStringLiteral("  (%1°)").arg(match.dec, 0, 'f', 5));
    magLabel_->setText(match.mag < 90.0
                       ? QString::number(match.mag, 'f', 1)
                       : tr("n/a"));
    motionLabel_->setText(
        QString("dRA=%1\"/hr  dDec=%2\"/hr")
            .arg(match.dRA,  0, 'f', 2)
            .arg(match.dDec, 0, 'f', 2));

    resultGroup_->setVisible(true);
    addOverlayBtn_->setVisible(true);
    adjustSize();
}

void HorizonsQueryDialog::onFailed(const QString& reason)
{
    queryBtn_->setEnabled(true);
    statusLabel_->setText(tr("<span style='color:red;'>%1</span>").arg(reason.toHtmlEscaped()));
    resultGroup_->setVisible(false);
    addOverlayBtn_->setVisible(false);
}
