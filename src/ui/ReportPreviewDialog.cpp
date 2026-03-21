#include "ReportPreviewDialog.h"

#include "core/FitsImage.h"

#include <QTabWidget>
#include <QSettings>
#include <QPlainTextEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QFont>
#include <QPdfWriter>
#include <QTextDocument>
#include <QPageSize>
#include <QPageLayout>
#include <QDesktopServices>
#include <QUrl>
#include <QUrlQuery>
#include <QImage>
#include <QPainter>
#include <QBuffer>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <cmath>

// ── Static helpers ─────────────────────────────────────────────────────────

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
    return QString("%1%2 %3' %4\"")
        .arg(sign)
        .arg(dd, 2, 10, QLatin1Char('0'))
        .arg(mm, 2, 10, QLatin1Char('0'))
        .arg(ss, 4, 'f', 1, QLatin1Char('0'));
}

static QImage renderFitsThumbnail(const core::FitsImage& img,
                                   const QVector<core::Observation>& imgObs,
                                   int maxSize)
{
    if (!img.isValid()) return {};
    const double scale = std::min(double(maxSize) / img.width, double(maxSize) / img.height);
    const int tw = std::max(1, static_cast<int>(img.width  * scale));
    const int th = std::max(1, static_cast<int>(img.height * scale));

    QImage gray(tw, th, QImage::Format_Grayscale8);
    for (int y = 0; y < th; ++y) {
        uchar* line = gray.scanLine(y);
        const int sy = std::min(static_cast<int>(y / scale), img.height - 1);
        for (int x = 0; x < tw; ++x) {
            const int sx = std::min(static_cast<int>(x / scale), img.width - 1);
            line[x] = core::stretchPixel(img.pixelAt(sx, sy), img.displayMin, img.displayMax);
        }
    }

    QImage thumb = gray.convertToFormat(QImage::Format_RGB32);

    if (!imgObs.isEmpty() && img.wcs.solved) {
        QPainter p(&thumb);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(QPen(QColor(255, 220, 0), 1.5));
        p.setBrush(Qt::NoBrush);
        for (const auto& ob : imgObs) {
            double px = 0.0, py = 0.0;
            img.wcs.skyToPix(ob.ra, ob.dec, px, py);
            const int tx = static_cast<int>(px * scale);
            const int ty = static_cast<int>(py * scale);
            if (tx >= 3 && tx < tw - 3 && ty >= 3 && ty < th - 3)
                p.drawEllipse(QPoint(tx, ty), 7, 7);
        }
    }
    return thumb;
}

static QString imageToDataUrl(const QImage& img)
{
    if (img.isNull()) return {};
    QByteArray ba;
    QBuffer    buf(&ba);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "PNG");
    return QStringLiteral("data:image/png;base64,") + QString::fromLatin1(ba.toBase64());
}

// ── Constructor ────────────────────────────────────────────────────────────

ReportPreviewDialog::ReportPreviewDialog(const QVector<core::Observation>& obs,
                                          const core::AdesContext& ctx,
                                          QSettings& settings,
                                          const core::ImageSession* session,
                                          QWidget* parent)
    : QDialog(parent)
    , obs_(obs), ctx_(ctx), imgSession_(session)
    , settings_(settings)
{
    setWindowTitle(tr("ADES 2022 Report Preview"));
    setMinimumSize(780, 560);
    resize(920, 640);

    xmlText_ = core::generateAdesXml(obs, ctx);
    psvText_ = core::generateAdesPsv(obs, ctx);

    // Summary
    summaryLabel_ = new QLabel(
        tr("%1 observation(s) — Station: %2")
            .arg(obs.size())
            .arg(ctx.mpcCode.isEmpty() ? tr("(not set)") : ctx.mpcCode),
        this);

    // Monospace font for the text editors
    QFont mono(QStringLiteral("Monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    mono.setPointSize(9);

    // XML tab
    xmlEdit_ = new QPlainTextEdit(xmlText_, this);
    xmlEdit_->setReadOnly(true);
    xmlEdit_->setFont(mono);
    xmlEdit_->setLineWrapMode(QPlainTextEdit::NoWrap);

    // PSV tab
    psvEdit_ = new QPlainTextEdit(psvText_, this);
    psvEdit_->setReadOnly(true);
    psvEdit_->setFont(mono);
    psvEdit_->setLineWrapMode(QPlainTextEdit::NoWrap);

    tabs_ = new QTabWidget(this);
    tabs_->addTab(xmlEdit_, tr("XML"));
    tabs_->addTab(psvEdit_, tr("PSV"));

    // MPC Submitter
    submitter_ = new core::MpcSubmit(this);
    const QString submitUrl = settings.value(
        QStringLiteral("mpc/submitUrl"),
        QStringLiteral("https://www.minorplanetcenter.net/report_ades")).toString();
    submitter_->setEndpoint(submitUrl);
    connect(submitter_, &core::MpcSubmit::submitted,
            this, &ReportPreviewDialog::onSubmitted);
    connect(submitter_, &core::MpcSubmit::failed,
            this, &ReportPreviewDialog::onSubmitFailed);
    connect(submitter_, &core::MpcSubmit::timedOut,
            this, &ReportPreviewDialog::onSubmitTimedOut);

    // Buttons — row 1: Save / Copy / PDF
    auto* btnSave       = new QPushButton(tr("Save…"),             this);
    auto* btnSaveFolder = new QPushButton(tr("Save to Reports Folder"), this);
    auto* btnCopy       = new QPushButton(tr("Copy"),              this);
    btnExportPdf_        = new QPushButton(tr("Export PDF…"),       this);
    auto* btnClose      = new QPushButton(tr("Close"),             this);

    // Buttons — row 2: Submit
    btnSubmitHttp_   = new QPushButton(tr("Submit to MPC (HTTP)"),  this);
    btnSubmitEmail_  = new QPushButton(tr("Submit to MPC (Email)"), this);
    btnSendToSchool_ = new QPushButton(tr("Enviar para Professor"), this);

    const QString reportDir = settings.value(
        QStringLiteral("paths/reportDir"), QDir::homePath()).toString();
    btnSave->setToolTip(tr("Save the currently visible tab (XML or PSV) to a chosen location"));
    btnSaveFolder->setToolTip(
        tr("Save both XML and PSV files to the reports folder:\n%1").arg(reportDir));
    btnExportPdf_->setToolTip(tr("Export a PDF summary with images and observations table"));
    btnSubmitHttp_->setToolTip(
        tr("POST the PSV report to the MPC via HTTP.\nTimeout: %1 s\nEndpoint: %2")
            .arg(submitter_->timeoutMs() / 1000).arg(submitUrl));
    btnSubmitEmail_->setToolTip(
        tr("Open your email client pre-addressed to obs@minorplanetcenter.net"));
    {
        const QString schoolEmail = settings.value(
            QStringLiteral("school/recipientEmail")).toString().trimmed();
        btnSendToSchool_->setVisible(!schoolEmail.isEmpty());
        btnSendToSchool_->setToolTip(
            tr("Envia o relatório PSV para: %1").arg(schoolEmail));
    }

    connect(btnSave,        &QPushButton::clicked, this, &ReportPreviewDialog::onSave);
    connect(btnSaveFolder,  &QPushButton::clicked, this, &ReportPreviewDialog::onSaveToFolder);
    connect(btnCopy,        &QPushButton::clicked, this, &ReportPreviewDialog::onCopy);
    connect(btnExportPdf_,  &QPushButton::clicked, this, &ReportPreviewDialog::onExportPdf);
    connect(btnSubmitHttp_, &QPushButton::clicked, this, &ReportPreviewDialog::onSubmitHttp);
    connect(btnSubmitEmail_,  &QPushButton::clicked, this, &ReportPreviewDialog::onSubmitEmail);
    connect(btnSendToSchool_, &QPushButton::clicked, this, &ReportPreviewDialog::onSendToSchool);
    connect(btnClose,         &QPushButton::clicked, this, &QDialog::accept);

    auto* topRow = new QHBoxLayout;
    topRow->addWidget(summaryLabel_);
    topRow->addStretch();
    topRow->addWidget(btnSave);
    topRow->addWidget(btnSaveFolder);
    topRow->addWidget(btnCopy);
    topRow->addWidget(btnExportPdf_);

    auto* botRow = new QHBoxLayout;
    botRow->addStretch();
    botRow->addWidget(btnSendToSchool_);
    botRow->addWidget(btnSubmitHttp_);
    botRow->addWidget(btnSubmitEmail_);
    botRow->addWidget(btnClose);

    auto* lay = new QVBoxLayout(this);
    lay->addWidget(tabs_);
    lay->addLayout(topRow);
    lay->addLayout(botRow);
}

// ── Private helpers ─────────────────────────────────────────────────────────

QString ReportPreviewDialog::smartBaseName() const
{
    const QString station = ctx_.mpcCode.isEmpty()
                                ? QStringLiteral("UNKNOWN")
                                : ctx_.mpcCode;

    // Use JD of first observation to get the date
    if (!obs_.isEmpty()) {
        // QDate::fromJulianDay uses the proleptic Julian calendar integer day.
        // JD starts at noon, so +0.5 converts to the correct calendar day.
        const QDate d = QDate::fromJulianDay(
            static_cast<qint64>(obs_.first().jd + 0.5));
        if (d.isValid())
            return QStringLiteral("ADES_%1_%2")
                       .arg(station)
                       .arg(d.toString(QStringLiteral("yyyyMMdd")));
    }

    // Fallback: today's date
    return QStringLiteral("ADES_%1_%2")
               .arg(station)
               .arg(QDate::currentDate().toString(QStringLiteral("yyyyMMdd")));
}

void ReportPreviewDialog::saveFile(const QString& path, const QString& content)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Save Failed"),
            tr("Could not write to:\n%1\n\n%2").arg(path, f.errorString()));
        return;
    }
    QTextStream out(&f);
    out.setEncoding(QStringConverter::Utf8);
    out << content;
}

// ── Slots ────────────────────────────────────────────────────────────────────

void ReportPreviewDialog::onSave()
{
    const bool    isXml  = (tabs_->currentIndex() == 0);
    const QString ext    = isXml ? QStringLiteral("xml") : QStringLiteral("psv");
    const QString filter = isXml
        ? tr("ADES XML (*.xml);;All files (*)")
        : tr("ADES PSV (*.txt *.psv);;All files (*)");

    const QString lastDir = settings_.value(
        QStringLiteral("paths/reportLastDir"),
        QDir::homePath()).toString();
    const QString defaultPath =
        lastDir + QDir::separator() + smartBaseName() + QLatin1Char('.') + ext;

    const QString path = QFileDialog::getSaveFileName(
        this, tr("Save ADES Report"), defaultPath, filter);
    if (path.isEmpty()) return;

    // Remember the chosen folder for next time
    settings_.setValue(QStringLiteral("paths/reportLastDir"),
                       QFileInfo(path).absolutePath());

    saveFile(path, isXml ? xmlText_ : psvText_);
}

void ReportPreviewDialog::onSaveToFolder()
{
    const QString dirPath = settings_.value(
        QStringLiteral("paths/reportDir"), QDir::homePath()).toString();

    QDir d(dirPath);
    if (!d.exists()) {
        if (!d.mkpath(dirPath)) {
            QMessageBox::critical(this, tr("Save Failed"),
                tr("The reports folder does not exist and could not be created:\n%1")
                    .arg(dirPath));
            return;
        }
    }

    const QString base    = smartBaseName();
    const QString xmlPath = d.filePath(base + QStringLiteral(".xml"));
    const QString psvPath = d.filePath(base + QStringLiteral(".psv"));

    // saveFile() shows its own error dialog on failure
    saveFile(xmlPath, xmlText_);
    saveFile(psvPath, psvText_);

    QMessageBox::information(this, tr("Reports Saved"),
        tr("Both reports saved to:\n\n"
           "XML:  %1\n"
           "PSV:  %2").arg(xmlPath, psvPath));
}

void ReportPreviewDialog::onCopy()
{
    QApplication::clipboard()->setText(
        tabs_->currentIndex() == 0 ? xmlText_ : psvText_);
}

void ReportPreviewDialog::onSubmitHttp()
{
    if (submitter_->isBusy()) return;

    const QString confirm = tr(
        "POST the PSV report to:\n%1\n\n"
        "Make sure your MPC station code and observer details are correct "
        "before submitting.\n\nContinue?").arg(submitter_->endpoint());

    if (QMessageBox::question(this, tr("Submit to MPC"), confirm,
                              QMessageBox::Yes | QMessageBox::No)
        != QMessageBox::Yes)
        return;

    btnSubmitHttp_->setEnabled(false);
    btnSubmitHttp_->setText(tr("Submitting…"));
    submitter_->submitHttp(psvText_, false);
}

void ReportPreviewDialog::onSubmitEmail()
{
    // mailto: body length is OS/client dependent; warn for large reports
    if (psvText_.length() > 2000) {
        const auto choice = QMessageBox::warning(
            this, tr("Large Report"),
            tr("The PSV report is %1 characters long.\n\n"
               "Some email clients truncate long mailto: body fields. "
               "Consider saving the PSV file and attaching it manually "
               "instead of using the body.\n\n"
               "Open email client anyway?")
                .arg(psvText_.length()),
            QMessageBox::Yes | QMessageBox::No);
        if (choice != QMessageBox::Yes) return;
    }

    core::MpcSubmit::submitByEmail(psvText_);
    QMessageBox::information(this, tr("Submit by Email"),
        tr("Your email client has been opened.\n\n"
           "Address: obs@minorplanetcenter.net\n"
           "Attach or paste the PSV report, then send."));
}

void ReportPreviewDialog::onSendToSchool()
{
    const QString recipient = settings_.value(
        QStringLiteral("school/recipientEmail")).toString().trimmed();
    if (recipient.isEmpty()) return;

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("subject"),
        tr("Relatório ADES — Observação de Asteroides"));
    q.addQueryItem(QStringLiteral("body"), psvText_);
    QUrl url(QStringLiteral("mailto:") + recipient);
    url.setQuery(q);
    QDesktopServices::openUrl(url);
}

void ReportPreviewDialog::onSubmitted(const QString& response)
{
    btnSubmitHttp_->setEnabled(true);
    btnSubmitHttp_->setText(tr("Submit to MPC (HTTP)"));

    QMessageBox::information(this, tr("MPC Submission"),
        tr("Report submitted successfully.\n\nServer response:\n%1")
            .arg(response.left(800)));
}

void ReportPreviewDialog::onSubmitFailed(const QString& reason)
{
    btnSubmitHttp_->setEnabled(true);
    btnSubmitHttp_->setText(tr("Submit to MPC (HTTP)"));

    const auto choice = QMessageBox::critical(this, tr("MPC Submission Failed"),
        tr("HTTP submission failed:\n%1\n\n"
           "Would you like to submit by email instead?").arg(reason),
        QMessageBox::Yes | QMessageBox::No);

    if (choice == QMessageBox::Yes)
        onSubmitEmail();
}

void ReportPreviewDialog::onSubmitTimedOut()
{
    btnSubmitHttp_->setEnabled(true);
    btnSubmitHttp_->setText(tr("Submit to MPC (HTTP)"));

    const auto choice = QMessageBox::warning(
        this, tr("MPC Submission — No Response"),
        tr("The MPC server did not respond within %1 seconds.\n\n"
           "The report may or may not have been received. "
           "Check your MPC account or try again later.\n\n"
           "Would you like to submit by email as a backup?")
            .arg(submitter_->timeoutMs() / 1000),
        QMessageBox::Yes | QMessageBox::No);

    if (choice == QMessageBox::Yes)
        onSubmitEmail();
}

void ReportPreviewDialog::onExportPdf()
{
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export PDF Report"),
        QStringLiteral("astrofind_report.pdf"),
        tr("PDF Files (*.pdf);;All Files (*)"));
    if (path.isEmpty()) return;

    btnExportPdf_->setEnabled(false);
    btnExportPdf_->setText(tr("Generating…"));
    QApplication::processEvents();

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageMargins(QMarginsF(15, 15, 15, 15), QPageLayout::Millimeter);
    writer.setResolution(150);
    writer.setTitle(tr("AstroFind Observation Report"));

    QTextDocument doc;
    doc.setDefaultFont(QFont(QStringLiteral("sans-serif"), 9));
    doc.setHtml(buildPdfHtml());
    doc.print(&writer);

    btnExportPdf_->setEnabled(true);
    btnExportPdf_->setText(tr("Export PDF…"));

    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

QString ReportPreviewDialog::buildPdfHtml() const
{
    const QString genTime = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    QString html =
        "<!DOCTYPE html>\n"
        "<html><head><meta charset=\"utf-8\"><style>\n"
        "body{font-family:sans-serif;font-size:9pt;color:#111;}\n"
        "h1{font-size:13pt;color:#1a3a6c;margin:0 0 2px 0;}\n"
        "h2{font-size:10pt;color:#1a3a6c;border-bottom:1px solid #bbb;"
        "padding-bottom:2px;margin:14px 0 4px 0;}\n"
        "table.meta{border-collapse:collapse;margin-bottom:4px;}\n"
        "table.meta td{padding:1px 10px 1px 0;}\n"
        "table.meta td.k{font-weight:bold;min-width:130px;}\n"
        "table.obs{border-collapse:collapse;width:100%;font-size:7.5pt;}\n"
        "table.obs th{background:#dde8ff;padding:3px 4px;border:1px solid #99aacc;"
        "text-align:center;}\n"
        "table.obs td{padding:2px 4px;border:1px solid #ccc;}\n"
        "table.obs tr.e{background:#f0f5ff;}\n"
        "</style></head><body>\n";

    html += QStringLiteral("<h1>AstroFind \xe2\x80\x94 Observation Report</h1>\n");
    html += QStringLiteral("<p style='color:#777;font-size:7.5pt;'>%1</p>\n").arg(genTime);

    // Observer / session metadata
    html += "<h2>Observer / Session</h2>\n<table class='meta'>\n";
    auto row = [&](const QString& key, const QString& val) {
        if (!val.trimmed().isEmpty())
            html += QString("<tr><td class='k'>%1</td><td>%2</td></tr>\n")
                        .arg(key).arg(val.toHtmlEscaped());
    };
    row(tr("MPC Station Code"), ctx_.mpcCode);
    row(tr("Observer"),         ctx_.observerName);
    row(tr("Measurer"),         ctx_.measurer);
    row(tr("Telescope"),        ctx_.telescope);
    row(tr("Catalog"),          ctx_.catalog);
    row(tr("Software"),         ctx_.software);
    html += "</table>\n";

    // Image thumbnails (one per session image, scaled to 200px, with observation markers)
    if (imgSession_ && imgSession_->imageCount() > 0) {
        html += "<h2>Images</h2>\n<table><tr>\n";
        for (int i = 0; i < imgSession_->imageCount(); ++i) {
            const auto& img = imgSession_->image(i);
            QVector<core::Observation> imgObs;
            for (const auto& ob : obs_)
                if (ob.imageIndex == i) imgObs.append(ob);

            const QImage thumb = renderFitsThumbnail(img, imgObs, 200);
            if (!thumb.isNull()) {
                const QString url   = imageToDataUrl(thumb);
                const QString label = img.fileName.isEmpty()
                                          ? QString("#%1").arg(i + 1)
                                          : img.fileName.toHtmlEscaped();
                const QString ts    = img.dateObs.isValid()
                                          ? img.dateObs.toString("HH:mm:ss 'UTC'")
                                          : QString();
                html += QString(
                    "<td align='center' style='padding:4px;vertical-align:bottom;'>"
                    "<img src='%1'/><br>"
                    "<span style='font-size:7pt;'>%2</span>%3"
                    "</td>\n")
                    .arg(url)
                    .arg(label)
                    .arg(ts.isEmpty() ? QStringLiteral("") :
                         QString("<br><span style='font-size:6.5pt;color:#555;'>%1</span>").arg(ts));
            }
            // 4 images per row
            if ((i + 1) % 4 == 0 && i + 1 < imgSession_->imageCount())
                html += "</tr><tr>\n";
        }
        html += "</tr></table>\n";
    }

    // Observations table
    html += QString("<h2>Observations (%1)</h2>\n").arg(obs_.size());
    html +=
        "<table class='obs'>\n"
        "<tr><th>Object</th><th>UTC Time</th>"
        "<th>RA (J2000)</th><th>Dec (J2000)</th>"
        "<th>Mag</th><th>Band</th>"
        "<th>FWHM\"</th><th>SNR</th></tr>\n";

    bool even = false;
    for (const auto& ob : obs_) {
        const QString utc  = core::jdToIso8601(ob.jd, ctx_.timePrecision);
        const QString mag  = (ob.mag < 90.0) ? QString::number(ob.mag, 'f', 2) : "—";
        const QString merr = (ob.magErr > 0.0)
                           ? QString(" <span style='color:#777;'>\xc2\xb1%1</span>")
                                 .arg(ob.magErr, 0, 'f', 2)
                           : QString();
        const QString fwhm = (ob.fwhm > 0.0) ? QString::number(ob.fwhm, 'f', 1) : "—";
        const QString snr  = (ob.snr  > 0.0) ? QString::number(ob.snr,  'f', 1) : "—";

        html += QString(
                    "<tr class='%1'>"
                    "<td>%2</td><td>%3</td>"
                    "<td>%4</td><td>%5</td>"
                    "<td>%6%7</td><td>%8</td>"
                    "<td>%9</td><td>%10</td>"
                    "</tr>\n")
                .arg(even ? "e" : "")
                .arg(ob.objectName.toHtmlEscaped())
                .arg(utc)
                .arg(raToHms(ob.ra))
                .arg(decToDms(ob.dec))
                .arg(mag)
                .arg(merr)
                .arg(ob.magBand.toHtmlEscaped())
                .arg(fwhm)
                .arg(snr);
        even = !even;
    }
    html += "</table>\n</body></html>";
    return html;
}
