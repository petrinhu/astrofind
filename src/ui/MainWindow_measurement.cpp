#include "MainWindow_p.h"

// ─── Undo commands ───────────────────────────────────────────────────────────

class AddObservationCmd : public QUndoCommand {
public:
    AddObservationCmd(MeasurementTable* table, const core::Observation& obs)
        : QUndoCommand(QObject::tr("Add Observation")), table_(table), obs_(obs) {}
    void redo() override { table_->addObservation(obs_); }
    void undo() override { table_->removeLastObservation(); }
private:
    MeasurementTable*   table_;
    core::Observation   obs_;
};

class RemoveObservationCmd : public QUndoCommand {
public:
    RemoveObservationCmd(MeasurementTable* table, int row, const core::Observation& obs)
        : QUndoCommand(QObject::tr("Remove Observation")), table_(table), row_(row), obs_(obs) {}
    void redo() override { table_->removeObservationAt(row_); }
    void undo() override { table_->insertObservationAt(row_, obs_); }
private:
    MeasurementTable*   table_;
    int                 row_;
    core::Observation   obs_;
};


void MainWindow::onPixelClicked(QPointF imgPx, double ra, double dec, float /*pixVal*/)
{
    auto* fw = activeFitsWindow();
    if (!fw) return;

    if (toolMode_ == ToolMode::Annotate) {
        bool ok = false;
        const QString text = QInputDialog::getText(this, tr("Add Annotation"),
            tr("Label text:"), QLineEdit::Normal, QString(), &ok);
        if (ok && !text.isEmpty())
            fw->imageView()->addAnnotation(imgPx, text);
        return;
    }

    // In Select mode: click near an overlay marker to select/deselect it
    if (!measureMode_) {
        imageCatalogTable_->selectRowByPixel(imgPx.x(), imgPx.y());
        return;
    }

    // Identify which session image the active window shows
    const QString filePath = fw->fitsImage().filePath;
    int sessionIdx = -1;
    for (int i = 0; i < session_->imageCount(); ++i) {
        if (session_->image(i).filePath == filePath) { sessionIdx = i; break; }
    }
    if (sessionIdx < 0) return;

    runMeasurePipeline(sessionIdx, imgPx, ra, dec);
}

void MainWindow::runMeasurePipeline(int sessionIdx, QPointF imgPx, double ra, double dec)
{
    const core::FitsImage& img = session_->image(sessionIdx);

    // ── 1. Subpixel centroid ──────────────────────────────────────────────────
    const auto centroid = core::findCentroidPsf(img, imgPx.x(), imgPx.y());
    if (!centroid) {
        logPanel_->appendWarning(tr("Centroid failed — no source found at click position"));
        return;
    }

    // ── 2. Refined sky position from centroid ─────────────────────────────────
    double raFinal = ra, decFinal = dec;
    if (img.wcs.solved)
        img.wcs.pixToSky(centroid->x, centroid->y, raFinal, decFinal);

    // ── 3. Aperture photometry ────────────────────────────────────────────────
    const double fwhmPx = 0.5 * (centroid->fwhmX + centroid->fwhmY);
    const bool apAuto = settings_.value(QStringLiteral("photometry/apertureAuto"), true).toBool();
    const double rAp = apAuto
        ? std::max(3.0, 2.0 * fwhmPx)
        : settings_.value(QStringLiteral("photometry/apertureManPx"), 8.0).toDouble();
    const auto   phot   = core::aperturePhotometry(img, centroid->x, centroid->y, rAp);

    // ── 4. Differential photometric calibration ───────────────────────────────
    auto diffZp = core::computeDifferentialZeroPoint(img, centroid->x, centroid->y, rAp);

    // Fallback to global ZP when plate is not solved or too few isolated stars
    double zpSigma = 0.0;
    std::optional<double> zpFallback;
    if (!diffZp)
        zpFallback = core::computeZeroPoint(img, rAp, &zpSigma);

    // ── 5. Airmass + atmospheric extinction correction ────────────────────────
    const double kExt = settings_.value(
        QStringLiteral("photometry/extinctionCoeff"), 0.0).toDouble();
    double airmass = 0.0;
    double extCorr = 0.0;
    if (img.jd > 2400000.0) {
        const SiteLocation site = effectiveSiteLocation();
        airmass = core::computeAirmass(raFinal, decFinal, img.jd, site.lat, site.lon);
        if (airmass > 0.0 && kExt > 0.0)
            extCorr = -kExt * airmass;  // sign: subtract extinction to get corrected mag
    }

    // ── 6. Build Observation ──────────────────────────────────────────────────
    core::Observation obs;
    obs.pixX       = centroid->x;
    obs.pixY       = centroid->y;
    obs.ra         = raFinal;
    obs.dec        = decFinal;
    obs.jd         = img.jd;
    obs.imagePath  = img.filePath;
    obs.imageIndex = sessionIdx;
    obs.snr        = centroid->snr;
    obs.airmass    = airmass;

    // FWHM in arcsec (use pixel scale from WCS or header)
    const double pixScale = (img.pixScaleX > 0.0) ? img.pixScaleX : 1.0;
    obs.fwhm   = fwhmPx * pixScale;
    // Positional uncertainties from WCS residuals (arcsec); 0 means ADES uses default 0.5"
    obs.raErr  = img.wcs.rms;
    obs.decErr = img.wcs.rms;

    if (phot) {
        if (diffZp) {
            obs.mag    = phot->magInst + diffZp->zp + extCorr;
            obs.magErr = std::sqrt(phot->magErr * phot->magErr
                                   + diffZp->sigma * diffZp->sigma);
        } else if (zpFallback) {
            obs.mag    = phot->magInst + *zpFallback + extCorr;
            obs.magErr = std::sqrt(phot->magErr * phot->magErr + zpSigma * zpSigma);
        } else {
            obs.mag    = phot->magInst + extCorr;
            obs.magErr = phot->magErr;
        }
    }

    // Log photometry quality
    if (phot) {
        if (airmass > 0.0) {
            const QString extNote = (kExt > 0.0)
                ? tr("  Airmass X=%1  k=%2  corr=%3 mag")
                      .arg(airmass, 0, 'f', 2).arg(kExt, 0, 'f', 3).arg(extCorr, 0, 'f', 3)
                : tr("  Airmass X=%1  (extinction correction disabled)")
                      .arg(airmass, 0, 'f', 2);
            logPanel_->appendInfo(extNote);
        }
        if (diffZp)
            logPanel_->appendInfo(tr("  Photometry (differential): ZP=%1±%2  %3 comp stars  mag=%4±%5")
                .arg(diffZp->zp,    0, 'f', 3)
                .arg(diffZp->sigma, 0, 'f', 3)
                .arg(diffZp->nStars)
                .arg(obs.mag < 90.0 ? QString::number(obs.mag,    'f', 2) : QStringLiteral("?"))
                .arg(obs.mag < 90.0 ? QString::number(obs.magErr, 'f', 2) : QStringLiteral("?")));
        else if (zpFallback)
            logPanel_->appendInfo(tr("  Photometry (global ZP): ZP=%1±%2  mag=%3±%4")
                .arg(*zpFallback, 0, 'f', 3)
                .arg(zpSigma,     0, 'f', 3)
                .arg(obs.mag < 90.0 ? QString::number(obs.mag,    'f', 2) : QStringLiteral("?"))
                .arg(obs.mag < 90.0 ? QString::number(obs.magErr, 'f', 2) : QStringLiteral("?")));
        else
            logPanel_->appendWarning(tr("  Photometry: no catalog stars matched — using instrumental mag"));
    }

    // ── 7. Auto-identify from known objects already in the overlay ────────────
    const double searchRadDeg = 10.0 * pixScale / 3600.0;  // 10 pixels → degrees
    for (const auto& koo : img.kooObjects) {
        const double dra  = (koo.ra  - raFinal) * std::cos(decFinal * M_PI / 180.0);
        const double ddec =  koo.dec - decFinal;
        if (std::sqrt(dra*dra + ddec*ddec) < searchRadDeg) {
            obs.objectName = koo.name;
            obs.objectType = koo.type;
            obs.mpcNumber  = koo.number;
            break;
        }
    }
    if (obs.objectName.isEmpty())
        obs.objectType = QStringLiteral("unknown");

    const QString defBand = settings_.value(QStringLiteral("photometry/defaultBand"), QStringLiteral("C")).toString();
    obs.magBand = img.filter.isEmpty() ? defBand : img.filter;
    if (!settings_.value(QStringLiteral("report/includeMag"), true).toBool())
        obs.mag = 99.0;  // sentinel: magnitude excluded from ADES report

    // ── 7. Show Verification Window ───────────────────────────────────────────
    if (!verifyDlg_) {
        verifyDlg_ = new VerificationDialog(this);
        connect(verifyDlg_, &VerificationDialog::accepted,
                this, [this](const core::Observation& confirmed) {
            undoStack_->push(new AddObservationCmd(measureTable_, confirmed));
            setProjectModified(true);
            if (auto* dock = qobject_cast<QDockWidget*>(measureTable_->parent()))
                dock->show();
            actUndo_->setEnabled(undoStack_->canUndo());
            actRedo_->setEnabled(undoStack_->canRedo());
            logPanel_->appendInfo(tr("Measurement confirmed: %1  RA=%2  Dec=%3  mag=%4")
                .arg(confirmed.objectName.isEmpty() ? tr("unnamed") : confirmed.objectName)
                .arg(confirmed.ra,  0, 'f', 5)
                .arg(confirmed.dec, 0, 'f', 5)
                .arg(confirmed.mag < 90.0 ? QString::number(confirmed.mag, 'f', 2) : QStringLiteral("?")));
            onSelectTool();  // return button to neutral state after measurement
        });
        connect(verifyDlg_, &VerificationDialog::rejected, this, [this]() {
            logPanel_->appendInfo(tr("Measurement rejected."));
            onSelectTool();  // return button to neutral state after rejection
        });
    }

    verifyDlg_->setMeasurement(img, obs, img.kooObjects);
}

// ─── Edit menu slots ──────────────────────────────────────────────────────────

void MainWindow::onUndo()
{
    if (undoStack_->canUndo()) {
        undoStack_->undo();
        statusBar()->showMessage(tr("Undone: %1").arg(undoStack_->undoText()), 2000);
    }
}
void MainWindow::onRedo()
{
    if (undoStack_->canRedo()) {
        undoStack_->redo();
        statusBar()->showMessage(tr("Redone: %1").arg(undoStack_->redoText()), 2000);
    }
}
void MainWindow::onClearMarkings()
{
    measureTable_->clear();
    for (auto* fw : allFitsWindows())
        fw->imageView()->clearAnnotations();
    statusBar()->showMessage(tr("All markings cleared"), 2000);
}
void MainWindow::onCopyImage()
{
    auto* fw = activeFitsWindow();
    if (fw) {
        QApplication::clipboard()->setPixmap(fw->grab());
        statusBar()->showMessage(tr("Image copied to clipboard"), 2000);
    }
}

// ─── Tool mode slots ──────────────────────────────────────────────────────────

void MainWindow::applyToolCursor()
{
    Qt::CursorShape shape = Qt::ArrowCursor;
    switch (toolMode_) {
        case ToolMode::Select:   shape = Qt::ArrowCursor;   break;
        case ToolMode::Aperture: shape = Qt::CrossCursor;   break;
        case ToolMode::Annotate: shape = Qt::IBeamCursor;   break;
    }
    for (auto* fw : allFitsWindows())
        fw->imageView()->setToolCursor(shape);
}

void MainWindow::onSelectTool()
{
    toolMode_    = ToolMode::Select;
    measureMode_ = false;
    if (actMeasure_->isChecked()) actMeasure_->setChecked(false);
    if (!actSelectTool_->isChecked()) actSelectTool_->setChecked(true);
    applyToolCursor();
    statusBar()->showMessage(tr("Select tool"), 2000);
}

void MainWindow::onApertureTool()
{
    toolMode_    = ToolMode::Aperture;
    measureMode_ = true;
    if (!actMeasure_->isChecked()) actMeasure_->setChecked(true);
    if (!actApertureTool_->isChecked()) actApertureTool_->setChecked(true);
    applyToolCursor();
    statusBar()->showMessage(tr("Aperture tool: click on a moving object to measure"), 3000);
}

void MainWindow::onAnnotateTool()
{
    toolMode_    = ToolMode::Annotate;
    measureMode_ = false;
    if (actMeasure_->isChecked()) actMeasure_->setChecked(false);
    if (!actAnnotateTool_->isChecked()) actAnnotateTool_->setChecked(true);
    applyToolCursor();
    statusBar()->showMessage(tr("Annotate tool: click on the image to add a text label"), 3000);
}

// ─── Blink step slots ─────────────────────────────────────────────────────────

void MainWindow::onBlinkPrev()
{
    if (blinkWidget_) blinkWidget_->showPrevious();
}
void MainWindow::onBlinkNext()
{
    if (blinkWidget_) blinkWidget_->showNext();
}

// ─── Extra zoom slots ─────────────────────────────────────────────────────────

void MainWindow::onZoom1to1()
{
    if (auto* fw = activeFitsWindow()) fw->imageView()->setZoom(1.0);
}
void MainWindow::onFitAllWindows()
{
    for (auto* fw : allFitsWindows()) fw->fitToWindow();
}

// ─── Internet slots ───────────────────────────────────────────────────────────

void MainWindow::onDownloadMpcOrb()
{
    const auto btn = QMessageBox::question(this, tr("Download MPCORB"),
        tr("MPCORB.DAT is approximately 200 MB.\n\nDownload now?"),
        QMessageBox::Yes | QMessageBox::No);
    if (btn != QMessageBox::Yes) return;

    const QString destDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(destDir);
    const QString destPath = destDir + "/MPCORB.DAT";

    logPanel_->appendInfo(tr("Downloading MPCORB.DAT to %1 ...").arg(destPath));

    // Progress bar injected into the status bar while download runs
    auto* progressBar = new QProgressBar(statusBar());
    progressBar->setRange(0, 0);
    progressBar->setFixedWidth(220);
    statusBar()->addPermanentWidget(progressBar);
    statusBar()->showMessage(tr("Downloading MPCORB.DAT…"));

    // Open output file up front so we can stream each chunk
    auto* outFile = new QFile(destPath);
    if (!outFile->open(QIODevice::WriteOnly)) {
        logPanel_->appendError(tr("Cannot open %1 for writing: %2").arg(destPath, outFile->errorString()));
        delete outFile;
        statusBar()->removeWidget(progressBar);
        progressBar->deleteLater();
        return;
    }

    auto* reply = nam_->get(QNetworkRequest(
        QUrl(QStringLiteral("https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT"))));

    // Track speed
    auto elapsed = std::make_shared<QElapsedTimer>();
    elapsed->start();
    auto lastBytes   = std::make_shared<qint64>(0);
    auto lastElapsed = std::make_shared<qint64>(0);

    // Stream-write each chunk as it arrives — avoids 200 MB RAM spike
    connect(reply, &QNetworkReply::readyRead, this, [reply, outFile]() {
        outFile->write(reply->readAll());
    });

    connect(reply, &QNetworkReply::downloadProgress, this,
        [this, progressBar, elapsed, lastBytes, lastElapsed](qint64 recv, qint64 total) {
            if (total > 0) {
                progressBar->setRange(0, static_cast<int>(total / 1024));
                progressBar->setValue(static_cast<int>(recv / 1024));
            }
            const qint64 now = elapsed->elapsed();
            const qint64 dt  = now - *lastElapsed;
            QString spd;
            if (dt >= 500) {
                const double speed = static_cast<double>(recv - *lastBytes) * 1000.0 / static_cast<double>(dt);
                *lastBytes   = recv;
                *lastElapsed = now;
                spd = speed > 1e6
                    ? tr("  %1 MB/s").arg(speed / 1e6, 0, 'f', 1)
                    : tr("  %1 KB/s").arg(speed / 1e3, 0, 'f', 0);
            }
            const QString msg = total > 0
                ? tr("Downloading MPCORB.DAT: %1 / %2 MB%3")
                      .arg(recv / 1'000'000).arg(total / 1'000'000).arg(spd)
                : tr("Downloading MPCORB.DAT: %1 MB%2")
                      .arg(recv / 1'000'000).arg(spd);
            statusBar()->showMessage(msg);
        });

    connect(reply, &QNetworkReply::finished, this,
        [this, reply, destPath, outFile, progressBar]() {
            reply->deleteLater();
            outFile->close();
            statusBar()->removeWidget(progressBar);
            progressBar->deleteLater();

            if (reply->error() != QNetworkReply::NoError) {
                outFile->remove();
                delete outFile;
                logPanel_->appendError(tr("Download failed: %1").arg(reply->errorString()));
                statusBar()->showMessage(tr("Download failed"), 4000);
                return;
            }
            delete outFile;
            const int n = core::countMpcOrbRecords(destPath);
            logPanel_->appendInfo(tr("MPCORB.DAT saved: %1 records").arg(n));
            statusBar()->showMessage(tr("MPCORB.DAT downloaded: %1 asteroids").arg(n), 5000);
        });
}
void MainWindow::onUpdateMpcOrb()
{
    const QString destDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString dailyPath = destDir + QStringLiteral("/DAILY.DAT");
    const QString mpcOrbPath = destDir + QStringLiteral("/MPCORB.DAT");

    if (!QFile::exists(mpcOrbPath)) {
        QMessageBox::warning(this, tr("Update MPCOrb"),
            tr("MPCORB.DAT not found.\nDownload it first via Internet → Download MPCOrb."));
        return;
    }

    statusBar()->showMessage(tr("Downloading DAILY.DAT…"));
    logPanel_->appendInfo(tr("Downloading DAILY.DAT from MPC…"));

    auto* reply = nam_->get(QNetworkRequest(
        QUrl(QStringLiteral("https://www.minorplanetcenter.net/iau/MPCORB/DAILY.DAT"))));

    connect(reply, &QNetworkReply::finished, this, [this, reply, dailyPath, mpcOrbPath]() {
        reply->deleteLater();
        if (reply->error() != QNetworkReply::NoError) {
            logPanel_->appendWarning(tr("DAILY.DAT download failed: %1").arg(reply->errorString()));
            statusBar()->showMessage(tr("Update failed"), 3000);
            return;
        }

        // Save DAILY.DAT
        QDir().mkpath(QFileInfo(dailyPath).absolutePath());
        QFile df(dailyPath);
        if (!df.open(QIODevice::WriteOnly)) {
            logPanel_->appendWarning(tr("Cannot write DAILY.DAT: %1").arg(df.errorString()));
            return;
        }
        df.write(reply->readAll());
        df.close();

        // Merge: append DAILY.DAT lines not already in MPCORB.DAT (match by packed designation)
        // Strategy: build set of packed IDs already present, then append new lines
        QFile orbFile(mpcOrbPath);
        QFile dailyFile(dailyPath);
        if (!orbFile.open(QIODevice::ReadOnly) || !dailyFile.open(QIODevice::ReadOnly)) {
            logPanel_->appendWarning(tr("Merge failed — cannot open files"));
            return;
        }

        QSet<QString> existingIds;
        {
            QTextStream in(&orbFile);
            // Skip header lines (first 43 lines in standard MPCORB)
            int lineNo = 0;
            while (!in.atEnd()) {
                const QString line = in.readLine();
                if (++lineNo > 43 && line.size() >= 7)
                    existingIds.insert(line.left(7).trimmed());
            }
        }
        orbFile.close();

        // Re-open for append
        if (!orbFile.open(QIODevice::Append | QIODevice::Text)) {
            logPanel_->appendWarning(tr("Cannot open MPCORB.DAT for appending"));
            return;
        }
        QTextStream out(&orbFile);
        QTextStream daily(&dailyFile);
        int added = 0, skipped = 0;
        while (!daily.atEnd()) {
            const QString line = daily.readLine();
            if (line.size() < 7) continue;
            const QString id = line.left(7).trimmed();
            if (existingIds.contains(id)) { ++skipped; continue; }
            out << line << "\n";
            existingIds.insert(id);
            ++added;
        }
        orbFile.close();

        logPanel_->appendInfo(tr("MPCOrb updated: %1 new records added (%2 already present)")
            .arg(added).arg(skipped));
        statusBar()->showMessage(tr("MPCOrb updated: +%1 records").arg(added), 5000);
        kooEngine_->setMpcOrbPath(mpcOrbPath);
    });
}
void MainWindow::onAstrometricaWebPage(){ QDesktopServices::openUrl(QUrl("https://www.minorplanetcenter.net")); }

void MainWindow::onQueryHorizons()
{
    // Use the JD of the first image in the session, or current time if no session.
    double jd = 0.0;
    if (session_ && session_->imageCount() > 0)
        jd = session_->image(0).jd;
    if (jd == 0.0) {
        // Fallback: approximate current JD
        const double unixSec = static_cast<double>(QDateTime::currentSecsSinceEpoch());
        jd = 2440587.5 + unixSec / 86400.0;
    }

    auto* dlg = new HorizonsQueryDialog(nam_, jd, this);

    // "Add to Overlay": convert EphemerisMatch → KooObject and push to all images
    connect(dlg, &HorizonsQueryDialog::overlayRequested,
            this, [this](const core::EphemerisMatch& match) {
        const core::KooObject koo = core::ephemerisMatchToKoo(match);
        for (int i = 0; i < session_->imageCount(); ++i) {
            session_->image(i).kooObjects.append(koo);
            updateImageOverlay(i);
        }
        logPanel_->appendInfo(
            tr("Horizons: added \"%1\" to overlay (RA=%2° Dec=%3°)")
                .arg(koo.name)
                .arg(koo.ra,  0, 'f', 4)
                .arg(koo.dec, 0, 'f', 4));
    });

    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

// ─── Windows slots ────────────────────────────────────────────────────────────

