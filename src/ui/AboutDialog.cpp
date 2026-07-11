// SPDX-License-Identifier: AGPL-3.0-or-later
// Copyright (C) 2026 Petrus Silva Costa

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

// ─── AGPL-3.0 License text ──────────────────────────────────────────────────

static const char* kLicenseEn =
    "GNU Affero General Public License v3.0\n\n"
    "Copyright (C) 2024\u20132026 Petrus Silva Costa\n\n"
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU Affero General Public License as\n"
    "published by the Free Software Foundation, either version 3 of the\n"
    "License, or (at your option) any later version.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
    "GNU Affero General Public License for more details.\n\n"
    "You should have received a copy of the GNU Affero General Public\n"
    "License along with this program. If not, see\n"
    "https://www.gnu.org/licenses/agpl-3.0.html";

static const char* kLicensePt =
    "Licen\u00e7a GNU AGPL v3.0\n"
    "(tradu\u00e7\u00e3o informativa \u2014 a vers\u00e3o em ingl\u00eas \u00e9 o texto juridicamente vinculante)\n\n"
    "Copyright (C) 2024\u20132026 Petrus Silva Costa\n\n"
    "Este programa \u00e9 software livre; voc\u00ea pode redistribu\u00ed-lo e/ou\n"
    "modific\u00e1-lo sob os termos da Licen\u00e7a P\u00fablica Geral Affero da GNU,\n"
    "conforme publicada pela Free Software Foundation; tanto a vers\u00e3o 3\n"
    "da Licen\u00e7a como (a seu crit\u00e9rio) qualquer vers\u00e3o mais recente.\n\n"
    "Texto completo: https://www.gnu.org/licenses/agpl-3.0.html";

// ─── License dialog ───────────────────────────────────────────────────────────

static void showLicense(QWidget* parent)
{
    auto* dlg = new QDialog(parent);
    dlg->setWindowTitle(QObject::tr("GNU Affero General Public License v3.0"));
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
           "<a href=\"license\">AGPL-3.0</a>."), this);
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
