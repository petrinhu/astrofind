#include "MainWindow_p.h"

void MainWindow::onLoadImages()
{
    const QString ccdDir  = settings_.value(QStringLiteral("paths/ccdDir")).toString();
    const QString lastDir = settings_.value(QStringLiteral("paths/lastImageDir"),
        ccdDir.isEmpty() ? QDir::homePath() : ccdDir).toString();

    const QStringList files = QFileDialog::getOpenFileNames(this,
        tr("Load FITS Images"), lastDir,
        tr("FITS & ZIP (*.fits *.fit *.fts *.FITS *.FIT *.zip *.ZIP);;FITS Images (*.fits *.fit *.fts);;ZIP archives (*.zip);;All files (*)"));

    if (files.isEmpty()) return;

    // Expand any ZIP archives → extract FITS to temp directory
    QStringList expandedFiles;
    QMap<QString, QString> fitsToZip;  // extractedPath → sourceZipPath
    for (const auto& path : files) {
        if (path.endsWith(".zip", Qt::CaseInsensitive)) {
            const auto extracted = expandZip(path);
            for (const auto& ep : extracted) fitsToZip[ep] = path;
            expandedFiles.append(extracted);
        } else {
            expandedFiles.append(path);
        }
    }

    const QString loadedDir = QFileInfo(files.first()).absolutePath();
    settings_.setValue("paths/lastImageDir", loadedDir);
    addToRecentDirs(loadedDir);

    if (!session_->isEmpty()) {
        const LoadChoice choice = askLoadChoice(session_->imageCount());
        if (choice == LoadChoice::Cancel) return;
        if (choice == LoadChoice::NewProject) {
            logPanel_->appendInfo(tr("Novo Projeto: limpando sessão anterior..."));
            onResetFiles();
            resetSessionSettings();
            QCoreApplication::processEvents();  // flush pending Qt deletions
        } else {
            logPanel_->appendInfo(tr("Acréscimo: adicionando à sessão existente."));
        }
    }

    QProgressDialog progress(tr("Loading FITS images..."), tr("Cancel"), 0, static_cast<int>(expandedFiles.size()), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(300);

    int loaded = 0;
    int limitRejected = 0;
    for (const auto& path : expandedFiles) {
        progress.setValue(loaded);
        progress.setLabelText(tr("Loading %1…").arg(QFileInfo(path).fileName()));
        if (progress.wasCanceled()) break;

        auto result = core::loadFits(path);
        if (!result) {
            logPanel_->appendError(tr("Failed to load %1: %2").arg(path, result.error()));
            QMessageBox::warning(this, tr("Load Error"),
                tr("Could not load:\n%1\n\n%2").arg(path, result.error()));
            continue;
        }

        auto& img = *result;
        img.sourceZipPath = fitsToZip.value(path);
        if (!session_->addImage(img)) { ++limitRejected; continue; }

        // Open MDI subwindow
        auto* sw = new FitsSubWindow(img, this);
        auto* mdiSw = mdiArea_->addSubWindow(sw);
        mdiSw->setWindowTitle(img.fileName);
        mdiSw->setWindowIcon(AppIcons::loadImages(16));
        mdiSw->show();

        connect(sw, &FitsSubWindow::cursorMoved, this, [this](double ra, double dec, float val) {
            sbCoords_->setText(tr("RA %1  Dec %2")
                .arg(ra,  0, 'f', 5)
                .arg(dec, 0, 'f', 5));
            sbPixel_->setText(tr("Val %1").arg(val, 0, 'f', 1));
        });

        connect(sw->imageView(), &FitsImageView::pixelClicked, this,
                [this](QPointF px, double ra, double dec, float val) {
                    onPixelClicked(px, ra, dec, val);
                });
        connect(sw->imageView(), &FitsImageView::escapePressed, this, [this]() {
            imageCatalogTable_->clearSelection();
        });
        applyToolCursor();
        connect(sw, &FitsSubWindow::regionSelected, this, &MainWindow::onRegionSelected);

        // ── Context menu signals ──────────────────────────────────────────
        connect(sw, &FitsSubWindow::exportImageRequested, this, [this, sw]() {
            onExportImage(sw);
        });
        connect(sw, &FitsSubWindow::applyDarkRequested, this, [this, sw]() {
            onApplyDarkToWindow(sw);
        });
        connect(sw, &FitsSubWindow::applyFlatRequested, this, [this, sw]() {
            onApplyFlatToWindow(sw);
        });
        connect(sw, &FitsSubWindow::showHistogramRequested, this, [this, sw]() {
            onShowHistogram(sw);
        });
        connect(sw, &FitsSubWindow::showImageCatalogRequested, this, [this]() {
            if (auto* dock = qobject_cast<QDockWidget*>(imageCatalogTable_->parent()))
                dock->show();
        });

        const QString binStr = (img.binningX > 1 || img.binningY > 1)
            ? tr("  bin%1×%2").arg(img.binningX).arg(img.binningY)
            : QString();
        logPanel_->appendInfo(tr("Loaded: %1  [%2×%3]%4  JD=%5")
            .arg(img.fileName)
            .arg(img.width)
            .arg(img.height)
            .arg(binStr)
            .arg(img.jd, 0, 'f', 6));
        if (img.dateObsAmbiguous)
            logPanel_->appendWarning(tr("%1: DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.").arg(img.fileName));

        ++loaded;
    }
    progress.setValue(static_cast<int>(expandedFiles.size()));

    if (limitRejected > 0) {
        logPanel_->appendWarning(tr("%1 image(s) not loaded: session limit of %2 reached.")
            .arg(limitRejected).arg(session_->maxImages()));
        QMessageBox::warning(this, tr("Session Limit Reached"),
            tr("%1 image(s) could not be loaded because the session already contains %2 images (limit: %3).\n\n"
               "To load more images, close the current session or increase the limit in Settings.")
                .arg(limitRejected).arg(session_->imageCount()).arg(session_->maxImages()));
    }

    if (loaded > 0) {
        setProjectModified(true);
        session_->setStep(core::SessionStep::ImagesLoaded);
        mdiArea_->tileSubWindows();
        sbImages_->setText(tr("%1 image(s)").arg(session_->imageCount()));
        sbStep_->setText(tr("Step 1: Images loaded — Run Data Reduction"));

        // Populate thumbnail bar
        QVector<core::FitsImage> imgs;
        imgs.reserve(session_->imageCount());
        for (int i = 0; i < session_->imageCount(); ++i)
            imgs.append(session_->image(i));
        logPanel_->appendInfo(tr("[DBG] setImages(%1 imgs) — session=%2").arg(imgs.size()).arg(session_->imageCount()));
        thumbnailBar_->setImages(imgs);
        logPanel_->appendInfo(tr("[DBG] setImages done"));
        autoFillSettingsFromSession();  // persist detected values to QSettings
        applySettingsToSubsystems();    // update status bar with FITS location info
        updateInfoBar();

        if (workflowPanel_->autoWorkflow())
            QTimer::singleShot(0, this, &MainWindow::onDataReduction);
    }
}

void MainWindow::onSaveAsFits()
{
    auto* fw = activeFitsWindow();
    if (!fw) { statusBar()->showMessage(tr("No active image"), 2000); return; }
    const core::FitsImage& img = fw->fitsImage();

    const QString dest = QFileDialog::getSaveFileName(this,
        tr("Save FITS As"),
        QFileInfo(img.filePath).absolutePath() + "/" + QFileInfo(img.filePath).baseName() + "_wcs.fits",
        tr("FITS Images (*.fits *.fit);;All files (*)"));
    if (dest.isEmpty()) return;

    // Copy original file then update WCS keywords in the copy
    if (!QFile::copy(img.filePath, dest)) {
        if (QFile::exists(dest)) QFile::remove(dest);
        if (!QFile::copy(img.filePath, dest)) {
            QMessageBox::warning(this, tr("Save FITS"), tr("Could not copy file to: %1").arg(dest));
            return;
        }
    }
    if (img.wcs.solved) {
        const QString err = core::saveWcsToFits(dest, img.wcs);
        if (!err.isEmpty())
            logPanel_->appendWarning(tr("WCS write warning: %1").arg(err));
    }
    logPanel_->appendInfo(tr("Saved: %1").arg(dest));
    statusBar()->showMessage(tr("Saved: %1").arg(QFileInfo(dest).fileName()), 4000);
}

void MainWindow::onSaveAllFits()
{
    if (session_->isEmpty()) { statusBar()->showMessage(tr("No images loaded"), 2000); return; }
    const QString dir = QFileDialog::getExistingDirectory(this,
        tr("Save All FITS — choose directory"),
        settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString());
    if (dir.isEmpty()) return;

    int saved = 0;
    for (int i = 0; i < session_->imageCount(); ++i) {
        const core::FitsImage& img = session_->image(i);
        const QString dest = dir + "/" + QFileInfo(img.filePath).baseName() + "_wcs.fits";
        if (QFile::exists(dest)) QFile::remove(dest);
        if (!QFile::copy(img.filePath, dest)) { logPanel_->appendWarning(tr("Could not copy %1").arg(img.fileName)); continue; }
        if (img.wcs.solved) core::saveWcsToFits(dest, img.wcs);
        ++saved;
    }
    logPanel_->appendInfo(tr("Saved %1 FITS files to %2").arg(saved).arg(dir));
    statusBar()->showMessage(tr("Saved %1 FITS files").arg(saved), 4000);
}

void MainWindow::onLoadDarkFrame()
{
    const QString path = QFileDialog::getOpenFileName(this,
        tr("Load Dark Frame"),
        settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString(),
        tr("FITS Images (*.fits *.fit *.fts);;All files (*)"));
    if (path.isEmpty()) return;

    auto result = core::loadFits(path);
    if (!result) {
        QMessageBox::warning(this, tr("Dark Frame"), tr("Cannot load: %1").arg(result.error()));
        return;
    }
    darkFrame_ = std::move(*result);
    logPanel_->appendInfo(tr("Dark frame loaded: %1  [%2×%3]")
        .arg(darkFrame_.fileName).arg(darkFrame_.width).arg(darkFrame_.height));
    statusBar()->showMessage(tr("Dark frame: %1").arg(darkFrame_.fileName), 4000);
    calibPanel_->setDarkStatus(darkFrame_.fileName, darkFrame_.width, darkFrame_.height);
    if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
}

void MainWindow::onLoadFlatField()
{
    const QString path = QFileDialog::getOpenFileName(this,
        tr("Load Flat Field"),
        settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString(),
        tr("FITS Images (*.fits *.fit *.fts);;All files (*)"));
    if (path.isEmpty()) return;

    auto result = core::loadFits(path);
    if (!result) {
        QMessageBox::warning(this, tr("Flat Field"), tr("Cannot load: %1").arg(result.error()));
        return;
    }
    flatFrame_ = std::move(*result);
    logPanel_->appendInfo(tr("Flat field loaded: %1  [%2×%3]")
        .arg(flatFrame_.fileName).arg(flatFrame_.width).arg(flatFrame_.height));
    statusBar()->showMessage(tr("Flat field: %1").arg(flatFrame_.fileName), 4000);
    calibPanel_->setFlatStatus(flatFrame_.fileName, flatFrame_.width, flatFrame_.height);
    if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
}
void MainWindow::onBuildMasterDark()
{
    MasterFrameDialog dlg(MasterFrameDialog::FrameType::Dark, settings_, this);
    if (dlg.exec() != QDialog::Accepted) return;

    darkFrame_ = dlg.result();
    logPanel_->appendInfo(tr("Master dark built: %1  [%2×%3]")
        .arg(darkFrame_.fileName).arg(darkFrame_.width).arg(darkFrame_.height));
    statusBar()->showMessage(tr("Master dark ready: %1").arg(darkFrame_.fileName), 4000);
    calibPanel_->setDarkStatus(darkFrame_.fileName, darkFrame_.width, darkFrame_.height);
    if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
}

void MainWindow::onBuildMasterFlat()
{
    MasterFrameDialog dlg(MasterFrameDialog::FrameType::Flat, settings_, this);
    if (dlg.exec() != QDialog::Accepted) return;

    flatFrame_ = dlg.result();
    logPanel_->appendInfo(tr("Master flat built: %1  [%2×%3]")
        .arg(flatFrame_.fileName).arg(flatFrame_.width).arg(flatFrame_.height));
    statusBar()->showMessage(tr("Master flat ready: %1").arg(flatFrame_.fileName), 4000);
    calibPanel_->setFlatStatus(flatFrame_.fileName, flatFrame_.width, flatFrame_.height);
    if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
}

void MainWindow::onCalibrationWizard()
{
    CalibrationWizard wiz(this);
    if (wiz.exec() != QDialog::Accepted) return;

    bool changed = false;

    if (!wiz.darkPath().isEmpty()) {
        auto result = core::loadFits(wiz.darkPath());
        if (result) {
            darkFrame_ = std::move(*result);
            calibPanel_->setDarkStatus(darkFrame_.fileName, darkFrame_.width, darkFrame_.height);
            logPanel_->appendInfo(tr("Dark frame loaded: %1  [%2×%3]")
                .arg(darkFrame_.fileName).arg(darkFrame_.width).arg(darkFrame_.height));
            changed = true;
        } else {
            QMessageBox::warning(this, tr("Dark Frame"), tr("Cannot load: %1").arg(result.error()));
        }
    }

    if (!wiz.flatPath().isEmpty()) {
        auto result = core::loadFits(wiz.flatPath());
        if (result) {
            flatFrame_ = std::move(*result);
            calibPanel_->setFlatStatus(flatFrame_.fileName, flatFrame_.width, flatFrame_.height);
            logPanel_->appendInfo(tr("Flat field loaded: %1  [%2×%3]")
                .arg(flatFrame_.fileName).arg(flatFrame_.width).arg(flatFrame_.height));
            changed = true;
        } else {
            QMessageBox::warning(this, tr("Flat Field"), tr("Cannot load: %1").arg(result.error()));
        }
    }

    if (changed) {
        if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
    }

    if (wiz.applyNow() && changed && session_->imageCount() > 0) {
        for (int i = 0; i < session_->imageCount(); ++i) {
            core::FitsImage& img = session_->image(i);
            if (darkFrame_.isValid() && darkFrame_.width == img.width && darkFrame_.height == img.height) {
                core::applyDark(img, darkFrame_);
            }
            if (flatFrame_.isValid() && flatFrame_.width == img.width && flatFrame_.height == img.height) {
                core::applyFlat(img, flatFrame_);
            }
            updateImageOverlay(i);
        }
        logPanel_->appendInfo(tr("Calibration applied to all %1 images.").arg(session_->imageCount()));
        showToast(tr("Calibration applied to %1 image(s)").arg(session_->imageCount()));
    }
}

void MainWindow::onViewAdesReport()
{
    const auto& obs = measureTable_->observations();
    if (obs.isEmpty()) {
        QMessageBox::information(this, tr("ADES Report"),
            tr("No observations yet.\n\nUse the Measure tool (M) to click on moving objects first."));
        return;
    }

    core::AdesContext ctx;
    ctx.mpcCode      = settings_.value(QStringLiteral("observer/mpcCode")).toString();
    ctx.observerName = settings_.value(QStringLiteral("observer/name")).toString();
    ctx.telescope    = settings_.value(QStringLiteral("observer/telescope")).toString();
    ctx.measurer        = settings_.value(QStringLiteral("observer/measurer")).toString();
    ctx.contact1        = settings_.value(QStringLiteral("observer/contact1")).toString();
    ctx.contact2        = settings_.value(QStringLiteral("observer/contact2")).toString();
    ctx.email           = settings_.value(QStringLiteral("observer/email")).toString();
    ctx.includeContact  = settings_.value(QStringLiteral("observer/includeContact"), false).toBool();
    ctx.timePrecision   = settings_.value(QStringLiteral("observer/timePrecision"), 1).toInt();
    ctx.catalog  = settings_.value(QStringLiteral("catalog/type"),
                                    QStringLiteral("UCAC4")).toString();
    ctx.software = QStringLiteral("AstroFind");

    // Auto-save to report/outputDir if configured
    const QString outDir = settings_.value(QStringLiteral("report/outputDir")).toString().trimmed();
    if (!outDir.isEmpty()) {
        QDir().mkpath(outDir);
        const QString xmlPath = outDir + QStringLiteral("/ades_report.xml");
        const QString psvPath = outDir + QStringLiteral("/ades_report.psv");
        auto writeFile = [&](const QString& path, const QString& text) {
            QFile f(path);
            if (f.open(QIODevice::WriteOnly | QIODevice::Text))
                QTextStream(&f) << text;
            else
                logPanel_->appendWarning(tr("ADES auto-save failed: %1").arg(f.errorString()));
        };
        writeFile(xmlPath, core::generateAdesXml(obs, ctx));
        writeFile(psvPath, core::generateAdesPsv(obs, ctx));
        logPanel_->appendInfo(tr("ADES report auto-saved to %1").arg(outDir));
    }

    ReportPreviewDialog dlg(obs, ctx, settings_, session_.get(), this);
    dlg.exec();
}

void MainWindow::onViewPhotometryFile()
{
    const auto& obs = measureTable_->observations();
    if (obs.isEmpty()) {
        statusBar()->showMessage(tr("No observations to show"), 3000);
        return;
    }
    // Show the observations dock
    if (auto* dock = qobject_cast<QDockWidget*>(measureTable_->parent()))
        dock->show();
}
void MainWindow::onViewLogFile()  { logPanel_->setVisible(true); }

// ─── Context menu handlers ────────────────────────────────────────────────────

void MainWindow::onExportImage(FitsSubWindow* sw)
{
    if (!sw) return;
    const QString suggested = QFileInfo(sw->fitsImage().filePath).completeBaseName() + ".jpg";
    const QString path = QFileDialog::getSaveFileName(this,
        tr("Export Image"),
        QDir(settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString())
            .filePath(suggested),
        tr("JPEG images (*.jpg *.jpeg);;PNG images (*.png);;BMP images (*.bmp);;All files (*)"));
    if (path.isEmpty()) return;

    const QString err = core::exportImage(sw->fitsImage(), path);
    if (err.isEmpty()) {
        logPanel_->appendInfo(tr("Exported: %1").arg(path));
        showToast(tr("Exported: %1").arg(QFileInfo(path).fileName()));
    } else {
        QMessageBox::warning(this, tr("Export Failed"), err);
    }
}

void MainWindow::onApplyDarkToWindow(FitsSubWindow* sw)
{
    if (!sw) return;
    // Delegate to the File menu action so the loaded dark is stored centrally
    if (!darkFrame_.isValid()) {
        onLoadDarkFrame();
        if (!darkFrame_.isValid()) return;
    }
    // Apply to this specific image in the session
    for (int i = 0; i < session_->imageCount(); ++i) {
        if (session_->image(i).filePath == sw->fitsImage().filePath) {
            core::applyDark(session_->image(i), darkFrame_);
            sw->updateImage(session_->image(i));
            logPanel_->appendInfo(tr("Dark applied to %1").arg(sw->fitsImage().fileName));
            return;
        }
    }
}

void MainWindow::onApplyFlatToWindow(FitsSubWindow* sw)
{
    if (!sw) return;
    if (!flatFrame_.isValid()) {
        onLoadFlatField();
        if (!flatFrame_.isValid()) return;
    }
    for (int i = 0; i < session_->imageCount(); ++i) {
        if (session_->image(i).filePath == sw->fitsImage().filePath) {
            core::applyFlat(session_->image(i), flatFrame_);
            sw->updateImage(session_->image(i));
            logPanel_->appendInfo(tr("Flat applied to %1").arg(sw->fitsImage().fileName));
            return;
        }
    }
}

void MainWindow::onShowHistogram(FitsSubWindow* sw)
{
    if (!sw) return;
    const core::FitsImage& img = sw->fitsImage();
    if (!img.isValid()) return;

    // Build a 256-bin histogram from a sampled subset of pixels
    constexpr int kBins = 256;
    std::array<int, kBins> bins{};
    bins.fill(0);
    const float mn = img.displayMin;
    const float mx = img.displayMax > mn ? img.displayMax : mn + 1.0f;
    const size_t step = std::max<size_t>(1, img.data.size() / 200000);
    for (size_t i = 0; i < img.data.size(); i += step) {
        const int bin = static_cast<int>((img.data[i] - mn) / (mx - mn) * (kBins - 1));
        if (bin >= 0 && bin < kBins) ++bins[bin];
    }

    // Simple histogram dialog — custom paint widget
    auto* dlg   = new QDialog(this, Qt::Tool);
    dlg->setWindowTitle(tr("Histogram — %1").arg(img.fileName));
    dlg->resize(400, 200);
    auto* lay   = new QVBoxLayout(dlg);
    lay->setContentsMargins(4, 4, 4, 4);

    auto* canvas = new QWidget(dlg);
    canvas->setMinimumSize(380, 160);
    const auto capturedBins = bins;   // capture by value for the lambda
    canvas->installEventFilter(dlg);  // paint via eventFilter below
    canvas->setProperty("histBins", QVariant::fromValue<QByteArray>(
        QByteArray(reinterpret_cast<const char*>(capturedBins.data()),
                   static_cast<qsizetype>(capturedBins.size() * sizeof(int)))));

    // Paint the histogram on the canvas
    connect(dlg, &QObject::destroyed, canvas, &QWidget::deleteLater);
    canvas->setAutoFillBackground(true);

    // Use a simple label-based approach: render to QPixmap
    const int W = 380, H = 150;
    QPixmap pm(W, H);
    pm.fill(QColor(20, 20, 30));
    {
        QPainter p(&pm);
        p.setPen(Qt::NoPen);
        const int maxVal = *std::max_element(bins.begin(), bins.end());
        if (maxVal > 0) {
            for (int b = 0; b < kBins; ++b) {
                const int barH = static_cast<int>(static_cast<double>(bins[b]) / maxVal * H);
                const int x = b * W / kBins;
                const int w = std::max(1, W / kBins);
                p.fillRect(x, H - barH, w, barH, QColor(80, 160, 220));
            }
        }
        // Draw displayMin/Max markers
        p.setPen(QPen(QColor(255, 80, 80), 1));
        p.drawLine(0, 0, 0, H);       // left = displayMin
        p.setPen(QPen(QColor(80, 255, 80), 1));
        p.drawLine(W - 1, 0, W - 1, H); // right = displayMax
    }
    auto* imgLabel = new QLabel(dlg);
    imgLabel->setPixmap(pm);
    lay->addWidget(imgLabel);

    auto* bb = new QDialogButtonBox(QDialogButtonBox::Close, dlg);
    lay->addWidget(bb);
    connect(bb, &QDialogButtonBox::rejected, dlg, &QDialog::accept);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}

// ─── Session settings reset ────────────────────────────────────────────────────

void MainWindow::autoFillSettingsFromSession()
{
    if (session_->isEmpty()) return;
    const core::FitsImage& img0 = session_->image(0);

    // ── Câmera — pixel scale e saturação ─────────────────────────────────────
    if (settings_.value(QStringLiteral("camera/pixelScaleX"), 0.0).toDouble() == 0.0
            && img0.pixScaleX > 0.0) {
        settings_.setValue(QStringLiteral("camera/pixelScaleX"), img0.pixScaleX);
        logPanel_->appendInfo(tr("  Auto-fill: escala X = %1\"/px (FITS)").arg(img0.pixScaleX, 0, 'f', 4));
    }
    if (settings_.value(QStringLiteral("camera/pixelScaleY"), 0.0).toDouble() == 0.0
            && img0.pixScaleY > 0.0) {
        settings_.setValue(QStringLiteral("camera/pixelScaleY"), img0.pixScaleY);
    }
    if (settings_.value(QStringLiteral("camera/saturation"), 0.0).toDouble() == 0.0
            && img0.saturation > 0.0 && img0.saturation < 1e9) {
        settings_.setValue(QStringLiteral("camera/saturation"), img0.saturation);
        logPanel_->appendInfo(tr("  Auto-fill: saturação = %1 ADU (FITS)").arg(static_cast<int>(img0.saturation)));
    }

    // ── Telescópio espacial — tem prioridade sobre lat/lon do site ────────────
    if (img0.isSpaceTelescope &&
            settings_.value(QStringLiteral("observer/locationMode")).toString() != QStringLiteral("spacecraft")) {
        settings_.setValue(QStringLiteral("observer/locationMode"),     QStringLiteral("spacecraft"));
        settings_.setValue(QStringLiteral("observer/spacecraftName"),   img0.telescope);
        settings_.setValue(QStringLiteral("observer/spacecraftMpcCode"),img0.mpcCode);
        if (!img0.mpcCode.isEmpty())
            settings_.setValue(QStringLiteral("observer/mpcCode"), img0.mpcCode);
        logPanel_->appendInfo(tr("  Telescópio espacial detectado: %1%2")
            .arg(img0.telescope,
                 img0.mpcCode.isEmpty()
                     ? tr(" (código MPC não identificado — configure manualmente)")
                     : tr(" — código MPC: %1").arg(img0.mpcCode)));
        showToast(tr("Telescópio espacial: %1 — lat/lon não aplicável").arg(img0.telescope),
                  Toast::Type::Info);
        return;   // skip ground-based location auto-fill
    }

    // ── Localização — FITS site coords → configurações ────────────────────────
    if (!std::isnan(img0.siteLat) && !std::isnan(img0.siteLon)) {
        // FITS tem coordenadas do site diretamente
        settings_.setValue(QStringLiteral("observer/latitude"),  img0.siteLat);
        settings_.setValue(QStringLiteral("observer/longitude"), img0.siteLon);
        if (!std::isnan(img0.siteAlt))
            settings_.setValue(QStringLiteral("observer/altitude"), img0.siteAlt);
        settings_.setValue(QStringLiteral("observer/locationMode"), QStringLiteral("fits"));
        logPanel_->appendInfo(tr("  Auto-fill: localização = %1°, %2° (FITS)")
            .arg(img0.siteLat, 0, 'f', 4).arg(img0.siteLon, 0, 'f', 4));
    } else if (!img0.mpcCode.isEmpty()) {
        // Sem coordenadas diretas — tentar via código MPC derivado do TELESCOP/ORIGIN
        const QByteArray codeBA = img0.mpcCode.toLatin1();
        if (const Observatory* o = ObservatoryDatabase::byCode(codeBA.constData())) {
            settings_.setValue(QStringLiteral("observer/latitude"),      o->lat);
            settings_.setValue(QStringLiteral("observer/longitude"),     o->lon);
            settings_.setValue(QStringLiteral("observer/altitude"),      o->alt);
            settings_.setValue(QStringLiteral("observer/presetMpcCode"), img0.mpcCode);
            settings_.setValue(QStringLiteral("observer/locationMode"),  QStringLiteral("preset"));
            logPanel_->appendInfo(tr("  Auto-fill: observatório %1 — %2 (%3°, %4°) do FITS")
                .arg(img0.mpcCode,
                     QString::fromLatin1(o->name),
                     QString::number(o->lat, 'f', 4),
                     QString::number(o->lon, 'f', 4)));
        }
    }

    // ── Fuso horário — deriva da longitude do observatório (longitude / 15) ──
    // Usar o relógio do sistema seria errado: o computador pode estar em fuso
    // diferente do telescópio. A fórmula astronômica correta é longitude/15,
    // que dá o tempo solar local e ignora DST (apropriado para observações).
    if (settings_.value(QStringLiteral("observer/timeOffset"), 0.0).toDouble() == 0.0) {
        double lon = std::numeric_limits<double>::quiet_NaN();

        // 1. Longitude do próprio FITS (se presente)
        if (!std::isnan(img0.siteLon))
            lon = img0.siteLon;

        // 2. Longitude do observatório preset (se configurado)
        if (std::isnan(lon)) {
            const QString code = settings_.value(QStringLiteral("observer/presetMpcCode")).toString();
            if (!code.isEmpty()) {
                const QByteArray ba = code.toLatin1();
                if (const Observatory* o = ObservatoryDatabase::byCode(ba.constData()))
                    lon = o->lon;
            }
        }

        // 3. Longitude manual (se configurada)
        if (std::isnan(lon)) {
            const double manLon = settings_.value(QStringLiteral("observer/longitude"), 0.0).toDouble();
            if (manLon != 0.0) lon = manLon;
        }

        if (!std::isnan(lon)) {
            const double offsetHours = lon / 15.0;
            settings_.setValue(QStringLiteral("observer/timeOffset"), offsetHours);
            logPanel_->appendInfo(tr("  Auto-fill: fuso horário = UTC%1%2h (longitude %3°)")
                .arg(offsetHours >= 0 ? QStringLiteral("+") : QString())
                .arg(offsetHours, 0, 'f', 1)
                .arg(lon, 0, 'f', 2));
        }
    }
}

void MainWindow::resetSessionSettings()
{
    // Clear auto-filled fields so the next FITS set can populate them fresh.
    // Permanent config (API key, observatory presets, report settings) is kept.
    const QStringList keys = {
        QStringLiteral("camera/pixelScaleX"),
        QStringLiteral("camera/pixelScaleY"),
        QStringLiteral("camera/saturation"),
        QStringLiteral("observer/latitude"),
        QStringLiteral("observer/longitude"),
        QStringLiteral("observer/altitude"),
        QStringLiteral("observer/timeOffset"),
    };
    for (const QString& k : keys) settings_.setValue(k, 0.0);
    settings_.setValue(QStringLiteral("observer/locationMode"), QStringLiteral("fits"));
    logPanel_->appendInfo(tr("Configurações de câmera, localização e fuso horário redefinidas."));
}

// ─── Load-choice dialog ────────────────────────────────────────────────────────

MainWindow::LoadChoice MainWindow::askLoadChoice(int currentCount)
{
    const QString noun = currentCount == 1
        ? tr("1 imagem carregada")
        : tr("%1 imagens carregadas").arg(currentCount);

    QMessageBox dlg(this);
    dlg.setWindowTitle(tr("Sessão em andamento"));
    dlg.setText(tr("Há %1 na sessão atual.").arg(noun));
    dlg.setInformativeText(tr("O que deseja fazer com as novas imagens?"));
    dlg.setIcon(QMessageBox::Question);

    QPushButton* addBtn  = dlg.addButton(tr("Acréscimo"),     QMessageBox::AcceptRole);
    QPushButton* newBtn  = dlg.addButton(tr("Novo Projeto"),  QMessageBox::DestructiveRole);
    /*QPushButton* canBtn =*/ dlg.addButton(QMessageBox::Cancel);

    addBtn->setToolTip(tr("Adicionar as novas imagens à sessão atual"));
    newBtn->setToolTip(tr("Descarregar tudo e iniciar sessão limpa (reseta localização, câmera e fuso)"));

    dlg.setDefaultButton(addBtn);
    dlg.exec();

    const QAbstractButton* clicked = dlg.clickedButton();
    if (clicked == addBtn) return LoadChoice::Add;
    if (clicked == newBtn) return LoadChoice::NewProject;
    return LoadChoice::Cancel;
}

void MainWindow::onResetFiles()
{
    if (blinkActive_) onStopBlinking();
    qDeleteAll(mdiArea_->subWindowList());  // destroys and removes from MDI area immediately
    session_->clear();
    thumbnailBar_->clear();
    blinkMdiWin_ = nullptr;
    blinkWidget_ = nullptr;
    reductionQueue_.clear();
    reducingIdx_ = -1;
    sbImages_->setText(tr("No images"));
    sbStep_->setText(tr("Ready"));
    logPanel_->appendInfo(tr("Session reset"));
    if (infoBar_) infoBar_->setVisible(false);
    currentProjectPath_.clear();
    projectModified_ = false;
    updateWindowTitle();
}

// ─── Project management ───────────────────────────────────────────────────────

void MainWindow::updateWindowTitle()
{
    if (currentProjectPath_.isEmpty()) {
        setWindowTitle(tr("AstroFind"));
    } else {
        const QString name = QFileInfo(currentProjectPath_).fileName();
        setWindowTitle(tr("AstroFind — %1%2").arg(name, projectModified_ ? QStringLiteral("*") : QString()));
    }
}

void MainWindow::setProjectModified(bool v)
{
    if (projectModified_ == v) return;
    projectModified_ = v;
    updateWindowTitle();
}

bool MainWindow::maybeSaveProject()
{
    if (!projectModified_ || session_->isEmpty()) return true;

    const QString title = currentProjectPath_.isEmpty()
        ? tr("Sessão não salva")
        : QFileInfo(currentProjectPath_).fileName();

    auto* msgBox = new QMessageBox(this);
    msgBox->setWindowTitle(tr("Salvar projeto?"));
    msgBox->setText(tr("<b>%1</b> foi modificado.<br>Deseja salvar antes de continuar?").arg(title));
    msgBox->setIcon(QMessageBox::Question);
    auto* btnSave    = msgBox->addButton(tr("Salvar"),          QMessageBox::AcceptRole);
    auto* btnDiscard = msgBox->addButton(tr("Não salvar"),      QMessageBox::DestructiveRole);
    msgBox->addButton(tr("Cancelar"),    QMessageBox::RejectRole);
    msgBox->setDefaultButton(btnSave);
    msgBox->exec();

    if (msgBox->clickedButton() == btnSave) {
        onSaveProject();
        // If save was cancelled (e.g. no path chosen), treat as Cancel
        return !projectModified_;
    }
    if (msgBox->clickedButton() == btnDiscard) return true;
    return false;  // Cancel
}

void MainWindow::onSaveProject()
{
    if (session_->isEmpty()) {
        statusBar()->showMessage(tr("Nenhuma sessão ativa para salvar."), 2000);
        return;
    }
    if (currentProjectPath_.isEmpty()) {
        onSaveProjectAs();
        return;
    }
    doSaveProject(currentProjectPath_);
}

void MainWindow::onSaveProjectAs()
{
    if (session_->isEmpty()) {
        statusBar()->showMessage(tr("Nenhuma sessão ativa para salvar."), 2000);
        return;
    }
    const QString defaultDir = settings_.value(QStringLiteral("paths/projectsDir"),
        QDir::homePath() + QStringLiteral("/projects")).toString();
    QDir().mkpath(defaultDir);

    const QString path = QFileDialog::getSaveFileName(this,
        tr("Salvar Projeto"), defaultDir,
        tr("AstroFind Project (*.gus)"));
    if (path.isEmpty()) return;

    settings_.setValue(QStringLiteral("paths/projectsDir"), QFileInfo(path).absolutePath());
    doSaveProject(path);
}

void MainWindow::onOpenProject()
{
    if (!maybeSaveProject()) return;

    const QString defaultDir = settings_.value(QStringLiteral("paths/projectsDir"),
        QDir::homePath() + QStringLiteral("/projects")).toString();

    const QString path = QFileDialog::getOpenFileName(this,
        tr("Abrir Projeto"), defaultDir,
        tr("AstroFind Project (*.gus)"));
    if (path.isEmpty()) return;

    settings_.setValue(QStringLiteral("paths/projectsDir"), QFileInfo(path).absolutePath());
    doLoadProject(path);
}

void MainWindow::onCloseProject()
{
    if (!maybeSaveProject()) return;

    onResetFiles();
    measureTable_->clear();
    undoStack_->clear();
    currentProjectPath_.clear();
    projectModified_ = false;
    updateWindowTitle();
    logPanel_->appendInfo(tr("Projeto fechado."));
}

void MainWindow::doSaveProject(const QString& path)
{
    core::GusProject gus;
    gus.sessionId = session_->sessionId();
    gus.step      = static_cast<int>(session_->step());

    for (int i = 0; i < session_->imageCount(); ++i) {
        core::FitsImage meta = session_->image(i);
        meta.data.clear();  // never serialize pixel data
        gus.images.append(meta);
    }
    gus.observations = measureTable_->observations();

    const QString err = gus.save(path);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Salvar Projeto"),
            tr("Não foi possível salvar o projeto:\n%1").arg(err));
        return;
    }

    currentProjectPath_ = path;
    setProjectModified(false);
    addToRecentProjects(path);
    logPanel_->appendInfo(tr("Projeto salvo: %1").arg(path));
    statusBar()->showMessage(tr("Projeto salvo."), 2000);
}

void MainWindow::doLoadProject(const QString& path)
{
    core::GusProject gus;
    const QString err = gus.load(path);
    if (!err.isEmpty()) {
        QMessageBox::warning(this, tr("Abrir Projeto"),
            tr("Não foi possível abrir o projeto:\n%1").arg(err));
        return;
    }

    onResetFiles();
    measureTable_->clear();
    undoStack_->clear();
    resetSessionSettings();

    QProgressDialog progress(tr("Carregando imagens do projeto..."), tr("Cancelar"),
                             0, static_cast<int>(gus.images.size()), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(300);

    int loaded = 0;
    for (const auto& savedImg : gus.images) {
        progress.setValue(loaded);
        progress.setLabelText(tr("Carregando %1…").arg(savedImg.fileName));
        if (progress.wasCanceled()) break;

        // ── Resolve file path ─────────────────────────────────────────────────
        QString resolvedPath = savedImg.filePath;
        if (!QFile::exists(resolvedPath)) {
            // Try re-extracting from the original ZIP if we know where it came from
            if (!savedImg.sourceZipPath.isEmpty() && QFile::exists(savedImg.sourceZipPath)) {
                const QStringList extracted = expandZip(savedImg.sourceZipPath);
                for (const auto& ep : extracted) {
                    if (QFileInfo(ep).fileName() == savedImg.fileName) {
                        resolvedPath = ep;
                        break;
                    }
                }
            }

            // Still missing → ask the user
            if (!QFile::exists(resolvedPath)) {
                const QString hint = savedImg.sourceZipPath.isEmpty()
                    ? savedImg.filePath
                    : tr("%1\n(do ZIP: %2)").arg(savedImg.filePath, savedImg.sourceZipPath);

                auto* msgBox = new QMessageBox(this);
                msgBox->setWindowTitle(tr("Arquivo não encontrado"));
                msgBox->setIcon(QMessageBox::Warning);
                msgBox->setText(tr("O arquivo <b>%1</b> não foi encontrado:").arg(savedImg.fileName));
                msgBox->setInformativeText(hint);
                auto* btnLocate = msgBox->addButton(tr("Localizar…"),    QMessageBox::ActionRole);
                auto* btnSkip   = msgBox->addButton(tr("Pular"),         QMessageBox::DestructiveRole);
                msgBox->addButton(tr("Cancelar tudo"), QMessageBox::RejectRole);
                msgBox->exec();

                if (msgBox->clickedButton() == btnLocate) {
                    const QString picked = QFileDialog::getOpenFileName(this,
                        tr("Localizar %1").arg(savedImg.fileName),
                        QFileInfo(savedImg.filePath).absolutePath(),
                        tr("FITS & ZIP (*.fits *.fit *.fts *.zip);;FITS (*.fits *.fit *.fts);;ZIP (*.zip);;All files (*)"));
                    if (picked.isEmpty()) { continue; }
                    if (picked.endsWith(".zip", Qt::CaseInsensitive)) {
                        const QStringList extracted = expandZip(picked);
                        for (const auto& ep : extracted) {
                            if (QFileInfo(ep).fileName() == savedImg.fileName) {
                                resolvedPath = ep; break;
                            }
                        }
                        if (!QFile::exists(resolvedPath)) resolvedPath = extracted.value(0);
                    } else {
                        resolvedPath = picked;
                    }
                } else if (msgBox->clickedButton() == btnSkip) {
                    logPanel_->appendWarning(tr("Projeto: imagem ignorada: %1").arg(savedImg.fileName));
                    continue;
                } else {
                    // Cancel all
                    break;
                }
            }
        }

        auto result = core::loadFits(resolvedPath);
        if (!result) {
            logPanel_->appendError(tr("Projeto: falha ao carregar %1: %2")
                .arg(savedImg.fileName, result.error()));
            continue;
        }

        auto img = std::move(*result);
        // Restore saved metadata that may have changed after original load
        img.displayMin     = savedImg.displayMin;
        img.displayMax     = savedImg.displayMax;
        img.wcs            = savedImg.wcs;
        img.detectedStars  = savedImg.detectedStars;
        img.catalogStars   = savedImg.catalogStars;
        img.kooObjects     = savedImg.kooObjects;

        session_->addImage(img);

        const core::FitsImage& addedImg = session_->image(session_->imageCount() - 1);
        auto* sw    = new FitsSubWindow(addedImg, this);
        auto* mdiSw = mdiArea_->addSubWindow(sw);
        mdiSw->setWindowTitle(addedImg.fileName);
        mdiSw->setWindowIcon(AppIcons::loadImages(16));
        mdiSw->show();

        connect(sw, &FitsSubWindow::cursorMoved, this, [this](double ra, double dec, float val) {
            sbCoords_->setText(tr("RA %1  Dec %2").arg(ra, 0, 'f', 5).arg(dec, 0, 'f', 5));
            sbPixel_->setText(tr("Val %1").arg(val, 0, 'f', 1));
        });
        connect(sw->imageView(), &FitsImageView::pixelClicked, this,
                [this](QPointF px, double ra, double dec, float val) { onPixelClicked(px, ra, dec, val); });
        connect(sw->imageView(), &FitsImageView::escapePressed, this, [this]() {
            imageCatalogTable_->clearSelection();
        });
        applyToolCursor();
        connect(sw, &FitsSubWindow::exportImageRequested,      this, [this, sw]() { onExportImage(sw); });
        connect(sw, &FitsSubWindow::applyDarkRequested,        this, [this, sw]() { onApplyDarkToWindow(sw); });
        connect(sw, &FitsSubWindow::applyFlatRequested,        this, [this, sw]() { onApplyFlatToWindow(sw); });
        connect(sw, &FitsSubWindow::showHistogramRequested,    this, [this, sw]() { onShowHistogram(sw); });
        connect(sw, &FitsSubWindow::showImageCatalogRequested, this, [this]() {
            if (auto* dock = qobject_cast<QDockWidget*>(imageCatalogTable_->parent())) dock->show();
        });

        logPanel_->appendInfo(tr("Projeto: %1  [%2×%3]")
            .arg(addedImg.fileName).arg(addedImg.width).arg(addedImg.height));
        if (addedImg.dateObsAmbiguous)
            logPanel_->appendWarning(tr("%1: DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.").arg(addedImg.fileName));
        ++loaded;
    }
    progress.setValue(static_cast<int>(gus.images.size()));

    if (loaded == 0) {
        logPanel_->appendError(tr("Projeto: nenhuma imagem pôde ser carregada."));
        return;
    }

    session_->setSessionId(gus.sessionId);
    session_->setStep(static_cast<core::SessionStep>(gus.step));

    mdiArea_->tileSubWindows();
    sbImages_->setText(tr("%1 image(s)").arg(session_->imageCount()));

    QVector<core::FitsImage> imgs;
    imgs.reserve(session_->imageCount());
    for (int i = 0; i < session_->imageCount(); ++i) imgs.append(session_->image(i));
    thumbnailBar_->setImages(imgs);

    for (const auto& obs : gus.observations)
        measureTable_->addObservation(obs);

    applySettingsToSubsystems();
    updateInfoBar();

    currentProjectPath_ = path;
    projectModified_ = false;
    updateWindowTitle();
    addToRecentProjects(path);

    logPanel_->appendInfo(tr("Projeto aberto: %1  (%2 imagens, %3 medições)")
        .arg(path).arg(loaded).arg(gus.observations.size()));
}

void MainWindow::onReloadMpcOrb()
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                         + "/MPCORB.DAT";
    if (!QFile::exists(path)) {
        logPanel_->appendWarning(tr("MPCORB.DAT not found. Use Internet → Download MPCOrb."));
        return;
    }
    const int n = core::countMpcOrbRecords(path);
    logPanel_->appendInfo(tr("MPCORB.DAT: %1 records at %2").arg(n).arg(path));
    statusBar()->showMessage(tr("MPCOrb loaded: %1 asteroids").arg(n), 4000);
    kooEngine_->setMpcOrbPath(path);
    kooEngine_->setOfflineFallbackEnabled(true);
}
void MainWindow::onSettings()
{
    const QString langBefore = settings_.value(QStringLiteral("ui/language"),
                                               QLocale::system().name()).toString();
    // Auto-fill timezone from system if never configured
    if (!settings_.contains(QStringLiteral("observer/timeZone"))) {
        const int utcSec = QDateTime::currentDateTime().offsetFromUtc();
        settings_.setValue(QStringLiteral("observer/timeZone"),
                           static_cast<double>(utcSec) / 3600.0);
    }

    // Auto-fill camera params from FITS if still at default (0)
    autoFillSettingsFromSession();   // fills camera/location/timezone before showing dialog

    SettingsDialog dlg(settings_, this);

    // Pass location hint (spacecraft takes priority over FITS coords)
    if (!session_->isEmpty()) {
        const auto& img0 = session_->image(0);
        if (img0.isSpaceTelescope) {
            dlg.setSpacecraftHint(true, img0.telescope, img0.mpcCode);
        } else if (!std::isnan(img0.siteLat) && !std::isnan(img0.siteLon)) {
            dlg.setFitsLocationHint(img0.siteLat, img0.siteLon,
                std::isnan(img0.siteAlt) ? 0.0 : img0.siteAlt);
        }
    }

    if (dlg.exec() != QDialog::Accepted) return;

    applySettingsToSubsystems();
    updateInfoBar();
    logPanel_->appendInfo(tr("Settings saved."));
    showToast(tr("Settings saved."));

    const QString langAfter = settings_.value(QStringLiteral("ui/language"),
                                              QLocale::system().name()).toString();
    if (langAfter != langBefore) {
        const auto btn = QMessageBox::question(this, tr("Language Changed"),
            tr("The interface language has been changed.\n"
               "AstroFind needs to restart to apply it.\n\n"
               "Restart now?"),
            QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes) {
            QProcess::startDetached(QApplication::applicationFilePath(),
                                    QApplication::arguments());
            QApplication::quit();
        }
    }
}
void MainWindow::onExit()         { close(); }

// ─── Recent directories ───────────────────────────────────────────────────────

static constexpr int kMaxRecentDirs = 8;

void MainWindow::addToRecentDirs(const QString& dirPath)
{
    QStringList dirs = settings_.value(QStringLiteral("files/recentDirs")).toStringList();
    dirs.removeAll(dirPath);
    dirs.prepend(dirPath);
    if (dirs.size() > kMaxRecentDirs)
        dirs.resize(kMaxRecentDirs);
    settings_.setValue(QStringLiteral("files/recentDirs"), dirs);
    updateRecentMenu();
}

void MainWindow::updateRecentMenu()
{
    recentMenu_->clear();
    const QStringList dirs = settings_.value(QStringLiteral("files/recentDirs")).toStringList();
    if (dirs.isEmpty()) {
        auto* empty = recentMenu_->addAction(tr("(no recent files)"));
        empty->setEnabled(false);
        return;
    }
    for (const QString& d : dirs) {
        const QString label = QDir(d).dirName() + QStringLiteral("  [") +
                              QDir::toNativeSeparators(d) + QStringLiteral("]");
        recentMenu_->addAction(label, this, [this, d]() { loadFromDir(d); });
    }
    recentMenu_->addSeparator();
    recentMenu_->addAction(tr("Clear Recent"), this, [this]() {
        settings_.remove(QStringLiteral("files/recentDirs"));
        updateRecentMenu();
    });
}

void MainWindow::addToRecentProjects(const QString& path)
{
    QStringList list = settings_.value(QStringLiteral("files/recentProjects")).toStringList();
    list.removeAll(path);
    list.prepend(path);
    if (list.size() > 5)
        list.resize(5);
    settings_.setValue(QStringLiteral("files/recentProjects"), list);
    updateRecentProjectsMenu();
}

void MainWindow::updateRecentProjectsMenu()
{
    recentProjectsMenu_->clear();
    const QStringList list = settings_.value(QStringLiteral("files/recentProjects")).toStringList();
    if (list.isEmpty()) {
        auto* empty = recentProjectsMenu_->addAction(tr("(nenhum projeto recente)"));
        empty->setEnabled(false);
        return;
    }
    for (const QString& p : list) {
        const QString label = QFileInfo(p).fileName() +
                              QStringLiteral("  [") + QDir::toNativeSeparators(p) + QStringLiteral("]");
        recentProjectsMenu_->addAction(label, this, [this, p]() {
            if (!QFile::exists(p)) {
                QMessageBox::warning(this, tr("Projetos Recentes"),
                    tr("Arquivo não encontrado:\n%1").arg(p));
                return;
            }
            if (!maybeSaveProject()) return;
            doLoadProject(p);
        });
    }
    recentProjectsMenu_->addSeparator();
    recentProjectsMenu_->addAction(tr("Limpar Recentes"), this, [this]() {
        settings_.remove(QStringLiteral("files/recentProjects"));
        updateRecentProjectsMenu();
    });
}

void MainWindow::loadFromDir(const QString& dirPath)
{
    if (!QDir(dirPath).exists()) {
        QMessageBox::warning(this, tr("Recent Files"),
            tr("Directory not found:\n%1").arg(dirPath));
        return;
    }

    static const QStringList kFitsFilters = {
        "*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT", "*.FTS"
    };
    const QStringList fitsFiles = QDir(dirPath).entryList(kFitsFilters, QDir::Files, QDir::Name);
    if (fitsFiles.isEmpty()) {
        QMessageBox::information(this, tr("Recent Files"),
            tr("No FITS files found in:\n%1").arg(dirPath));
        return;
    }

    QStringList fullPaths;
    fullPaths.reserve(fitsFiles.size());
    for (const QString& f : fitsFiles)
        fullPaths.append(dirPath + QDir::separator() + f);

    if (!session_->isEmpty()) {
        const LoadChoice choice = askLoadChoice(session_->imageCount());
        if (choice == LoadChoice::Cancel) return;
        if (choice == LoadChoice::NewProject) {
            logPanel_->appendInfo(tr("Novo Projeto: limpando sessão anterior..."));
            onResetFiles();
            resetSessionSettings();
            QCoreApplication::processEvents();
        } else {
            logPanel_->appendInfo(tr("Acréscimo: adicionando à sessão existente."));
        }
    }

    settings_.setValue(QStringLiteral("paths/lastImageDir"), dirPath);
    addToRecentDirs(dirPath);

    QProgressDialog progress(tr("Loading FITS images..."), tr("Cancel"),
                             0, static_cast<int>(fullPaths.size()), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(300);

    int loaded = 0;
    int dirLimitRejected = 0;
    for (const auto& path : fullPaths) {
        progress.setValue(loaded);
        progress.setLabelText(tr("Loading %1…").arg(QFileInfo(path).fileName()));
        if (progress.wasCanceled()) break;

        auto result = core::loadFits(path);
        if (!result) {
            logPanel_->appendError(tr("Failed to load %1: %2").arg(path, result.error()));
            continue;
        }
        auto& img = *result;
        if (!session_->addImage(img)) { ++dirLimitRejected; continue; }

        auto* sw    = new FitsSubWindow(img, this);
        auto* mdiSw = mdiArea_->addSubWindow(sw);
        mdiSw->setWindowTitle(img.fileName);
        mdiSw->setWindowIcon(AppIcons::loadImages(16));
        mdiSw->show();

        connect(sw, &FitsSubWindow::cursorMoved, this, [this](double ra, double dec, float val) {
            sbCoords_->setText(tr("RA %1  Dec %2").arg(ra, 0, 'f', 5).arg(dec, 0, 'f', 5));
            sbPixel_->setText(tr("Val %1").arg(val, 0, 'f', 1));
        });
        connect(sw->imageView(), &FitsImageView::pixelClicked, this,
                [this](QPointF px, double ra, double dec, float val) {
                    onPixelClicked(px, ra, dec, val);
                });
        connect(sw->imageView(), &FitsImageView::escapePressed, this, [this]() {
            imageCatalogTable_->clearSelection();
        });
        applyToolCursor();
        connect(sw, &FitsSubWindow::regionSelected,        this, &MainWindow::onRegionSelected);
        connect(sw, &FitsSubWindow::exportImageRequested,  this, [this, sw]() { onExportImage(sw); });
        connect(sw, &FitsSubWindow::applyDarkRequested,    this, [this, sw]() { onApplyDarkToWindow(sw); });
        connect(sw, &FitsSubWindow::applyFlatRequested,    this, [this, sw]() { onApplyFlatToWindow(sw); });
        connect(sw, &FitsSubWindow::showHistogramRequested,this, [this, sw]() { onShowHistogram(sw); });

        ++loaded;
    }
    progress.setValue(static_cast<int>(fullPaths.size()));

    if (dirLimitRejected > 0) {
        logPanel_->appendWarning(tr("%1 image(s) not loaded: session limit of %2 reached.")
            .arg(dirLimitRejected).arg(session_->maxImages()));
        QMessageBox::warning(this, tr("Session Limit Reached"),
            tr("%1 image(s) could not be loaded because the session already contains %2 images (limit: %3).\n\n"
               "To load more images, close the current session or increase the limit in Settings.")
                .arg(dirLimitRejected).arg(session_->imageCount()).arg(session_->maxImages()));
    }

    if (loaded > 0) {
        onTileWindows();
        session_->setStep(core::SessionStep::ImagesLoaded);
        sbImages_->setText(tr("%1 image(s)").arg(session_->imageCount()));
        logPanel_->appendInfo(tr("Loaded %1 image(s) from %2").arg(loaded).arg(dirPath));
    }
}

// ─── Astrometry slots ─────────────────────────────────────────────────────────


QStringList MainWindow::expandZip(const QString& zipPath)
{
    QStringList result;

    if (!tempDir_)
        tempDir_ = std::make_unique<QTemporaryDir>();

    if (!tempDir_->isValid()) {
        logPanel_->appendError(tr("Could not create temp directory for ZIP extraction"));
        return result;
    }

    // Each ZIP gets its own unique subdirectory so that scanning one ZIP
    // never picks up files extracted from a previous ZIP.
    const QString extractDir = tempDir_->path() + QDir::separator()
        + QFileInfo(zipPath).baseName().left(32)
        + QStringLiteral("_") + QString::number(QDateTime::currentMSecsSinceEpoch());
    QDir().mkpath(extractDir);

    // Extract only FITS files from the ZIP using system unzip
    QProcess proc;
    proc.start("unzip", {
        "-o", zipPath,
        "*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT",
        "-d", extractDir
    });

    if (!proc.waitForFinished(30000)) {
        logPanel_->appendError(tr("Timed out extracting ZIP: %1").arg(zipPath));
        return result;
    }

    // Search only inside this ZIP's own subdirectory
    QDirIterator it(extractDir,
                    {"*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT"},
                    QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        result.append(it.next());
    result.sort();

    if (result.isEmpty())
        logPanel_->appendWarning(tr("No FITS files found in ZIP: %1").arg(QFileInfo(zipPath).fileName()));
    else
        logPanel_->appendInfo(tr("Extracted %1 FITS file(s) from %2")
            .arg(result.size()).arg(QFileInfo(zipPath).fileName()));

    return result;
}
