#include "AboutDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QTabWidget>
#include <QTextBrowser>
#include <QFont>
#include <QApplication>
#include <QSettings>
#include <QLocale>
#include <QDesktopServices>

// ─── PolyForm Noncommercial License text ──────────────────────────────────────

static const char* kLicenseEn =
    "PolyForm Noncommercial License 1.0.0\n\n"
    "Copyright (c) 2024\u20132026 Petrus Silva Costa\n"
    "Required Notice: Copyright (c) 2024\u20132026 Petrus Silva Costa\n\n"
    "Acceptance\n\n"
    "In order to get any license under these terms, you must agree to them as both "
    "strict obligations and conditions to all your licenses.\n\n"
    "Copyright License\n\n"
    "The licensor grants you a copyright license for the software to do everything "
    "you might do with the software that would otherwise infringe the licensor's "
    "copyright in it for any permitted purpose. However, you may only distribute "
    "the software according to the Distribution License and you may not use the "
    "software to provide services to others according to the Service License.\n\n"
    "Noncommercial Purposes\n\n"
    "Any noncommercial purpose is a permitted purpose.\n\n"
    "Personal Uses\n\n"
    "Personal use for research, experiment, and testing for the benefit of public "
    "knowledge, personal study, private entertainment, hobby projects, amateur "
    "pursuits, or religious observance, without any anticipated commercial "
    "application, is use for a permitted purpose.\n\n"
    "Noncommercial Organizations\n\n"
    "Use by any charitable organization, educational institution, public research "
    "organization, public safety or health organization, environmental protection "
    "organization, or government institution is use for a permitted purpose "
    "regardless of the source of funding or obligations resulting from the funding.\n\n"
    "No Liability\n\n"
    "As far as the law allows, the software comes as is, without any warranty or "
    "condition, and the licensor will not be liable to you for any damages arising "
    "out of these terms or the use or nature of the software, under any kind of "
    "legal claim.\n\n"
    "Full license text: https://polyformproject.org/licenses/noncommercial/1.0.0/";

static const char* kLicensePt =
    "Licen\u00e7a PolyForm Noncommercial 1.0.0\n"
    "(tradu\u00e7\u00e3o informativa \u2014 a vers\u00e3o em ingl\u00eas \u00e9 o texto juridicamente vinculante)\n\n"
    "Copyright (c) 2024\u20132026 Petrus Silva Costa\n\n"
    "Aceita\u00e7\u00e3o\n\n"
    "Para obter qualquer licen\u00e7a sob estes termos, voc\u00ea deve concordar com eles "
    "tanto como obriga\u00e7\u00f5es estritas quanto como condi\u00e7\u00f5es de todas as suas licen\u00e7as.\n\n"
    "Finalidades N\u00e3o Comerciais\n\n"
    "Qualquer finalidade n\u00e3o comercial \u00e9 uma finalidade permitida.\n\n"
    "Usos Pessoais\n\n"
    "O uso pessoal para pesquisa, experimento e teste em benef\u00edcio do conhecimento "
    "p\u00fablico, estudo pessoal, entretenimento privado, projetos de hobby, atividades "
    "amadoras ou observ\u00e2ncia religiosa, sem qualquer aplica\u00e7\u00e3o comercial prevista, "
    "\u00e9 uso para finalidade permitida.\n\n"
    "Organiza\u00e7\u00f5es Sem Fins Lucrativos\n\n"
    "O uso por qualquer organiza\u00e7\u00e3o de caridade, institui\u00e7\u00e3o de ensino, organiza\u00e7\u00e3o "
    "de pesquisa p\u00fablica, organiza\u00e7\u00e3o de seguran\u00e7a ou sa\u00fade p\u00fablica, organiza\u00e7\u00e3o de "
    "prote\u00e7\u00e3o ambiental ou institui\u00e7\u00e3o governamental \u00e9 uso para finalidade permitida.\n\n"
    "Sem Responsabilidade\n\n"
    "Na medida permitida pela lei, o software \u00e9 fornecido como est\u00e1, sem qualquer "
    "garantia ou condi\u00e7\u00e3o, e o licenciante n\u00e3o ser\u00e1 respons\u00e1vel perante voc\u00ea por "
    "quaisquer danos decorrentes destes termos ou do uso ou natureza do software.\n\n"
    "Texto completo: https://polyformproject.org/licenses/noncommercial/1.0.0/";

// ─── License dialog ───────────────────────────────────────────────────────────

static void showLicense(QWidget* parent)
{
    auto* dlg = new QDialog(parent);
    dlg->setWindowTitle(QObject::tr("PolyForm Noncommercial License 1.0.0"));
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->resize(520, 440);

    auto* tabs = new QTabWidget(dlg);

    auto makeBrowser = [](const char* text) {
        auto* tb = new QTextBrowser();
        tb->setPlainText(QString::fromUtf8(text));
        tb->setReadOnly(true);
        return tb;
    };

    tabs->addTab(makeBrowser(kLicensePt), QObject::tr("Português (BR)"));
    tabs->addTab(makeBrowser(kLicenseEn), QObject::tr("English"));

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok, dlg);
    QObject::connect(bb, &QDialogButtonBox::accepted, dlg, &QDialog::accept);

    auto* lay = new QVBoxLayout(dlg);
    lay->addWidget(tabs);
    lay->addWidget(bb);

    dlg->exec();
}

// ─── AboutDialog ─────────────────────────────────────────────────────────────

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("About AstroFind"));
    setFixedSize(440, 500);
    setModal(true);

    auto* lay = new QVBoxLayout(this);
    lay->setSpacing(8);
    lay->setContentsMargins(24, 20, 24, 14);

    // ── Title ────────────────────────────────────────────────────────────────
    auto* titleLabel = new QLabel("AstroFind", this);
    QFont tf = titleLabel->font();
    tf.setPointSize(22);
    tf.setBold(true);
    titleLabel->setFont(tf);
    titleLabel->setStyleSheet("color:#4488ff;");
    titleLabel->setAlignment(Qt::AlignCenter);

    auto* versionLabel = new QLabel(
        tr("Version %1").arg(QApplication::applicationVersion()), this);
    versionLabel->setAlignment(Qt::AlignCenter);

    auto* descLabel = new QLabel(
        tr("Modern asteroid detection software for citizen science.\n"
           "Compatible with the IASC / MPC workflow.\n"
           "Outputs ADES 2022 format reports."), this);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);

    // ── Dedication ───────────────────────────────────────────────────────────
    const QString configLang = QSettings().value(
        QStringLiteral("ui/language"), QLocale::system().name()).toString();
    const bool isPt = configLang.startsWith(QStringLiteral("pt"));

    const QString dedicText = isPt
        ? QStringLiteral("\u201cPara meu filho Guga, usu\u00e1rio de Linux como eu\n"
                         "que estava sem uma boa op\u00e7\u00e3o para buscar asteroides.\u201d")
        : QStringLiteral("\u201cTo my son Guga, a Linux user like me\n"
                         "who had no good option for finding asteroids.\u201d");

    auto* dedicLabel = new QLabel(dedicText, this);
    dedicLabel->setWordWrap(true);
    dedicLabel->setAlignment(Qt::AlignCenter);
    dedicLabel->setStyleSheet("font-style:italic; font-size:11px; margin:2px 8px;");

    // ── Credits ──────────────────────────────────────────────────────────────
    // "MIT License" is a clickable link that opens the bilingual license dialog.
    auto* credLabel = new QLabel(
        tr("Inspired by <a href=\"http://www.astrometrica.at/\">Astrometrica</a> by Herbert Raab.<br>"
           "Built with Qt6, cfitsio, Eigen3, SEP.<br>"
           "Licensed under the "
           "<a href=\"license\">PolyForm Noncommercial 1.0.0</a>."), this);
    credLabel->setTextFormat(Qt::RichText);
    credLabel->setOpenExternalLinks(false);
    credLabel->setWordWrap(true);
    credLabel->setAlignment(Qt::AlignCenter);
    credLabel->setStyleSheet("font-size:11px;");
    connect(credLabel, &QLabel::linkActivated, this, [this](const QString& url) {
        if (url == QStringLiteral("license"))
            showLicense(this);
        else
            QDesktopServices::openUrl(QUrl(url));
    });

    // ── Reference implementations ─────────────────────────────────────────────
    auto* refTitle = new QLabel(tr("Reference implementations"), this);
    refTitle->setAlignment(Qt::AlignCenter);
    QFont rf = refTitle->font();
    rf.setBold(true);
    rf.setPointSize(rf.pointSize() - 1);
    refTitle->setFont(rf);

    auto* refLabel = new QLabel(
        tr("<a href=\"https://github.com/Bill-Gray/find_orb\"><b>find_orb</b></a> (Project Pluto / Bill Gray) \u2014 ADES, MPC, orbit<br>"
           "<a href=\"https://github.com/mostanes/umbrella2\"><b>umbrella2</b></a> (CSCF) \u2014 moving object detection, tracklets<br>"
           "<a href=\"https://gitlab.com/free-astro/siril\"><b>Siril</b></a> (Free-Astro team) \u2014 PSF, background, photometry<br>"
           "<a href=\"https://github.com/dstndstn/astrometry.net\"><b>astrometry.net</b></a> (Dustin Lang et al.) \u2014 plate-solving API<br>"
           "<a href=\"https://github.com/iraf-community/iraf\"><b>IRAF</b></a> (NOAO) \u2014 aperture photometry concepts"), this);
    refLabel->setTextFormat(Qt::RichText);
    refLabel->setOpenExternalLinks(true);
    refLabel->setWordWrap(true);
    refLabel->setAlignment(Qt::AlignCenter);
    refLabel->setStyleSheet("font-size:10px;");

    // ── Separators ────────────────────────────────────────────────────────────
    auto makeSep = [this]() {
        auto* s = new QLabel(this);
        s->setFixedHeight(1);
        s->setStyleSheet("background: palette(mid);");
        return s;
    };

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Ok, this);
    connect(bb, &QDialogButtonBox::accepted, this, &QDialog::accept);

    lay->addWidget(titleLabel);
    lay->addWidget(versionLabel);
    lay->addWidget(descLabel);
    lay->addWidget(makeSep());
    lay->addWidget(dedicLabel);
    lay->addWidget(makeSep());
    lay->addWidget(credLabel);
    lay->addWidget(makeSep());
    lay->addWidget(refTitle);
    lay->addWidget(refLabel);
    lay->addStretch();
    lay->addWidget(bb);
}
