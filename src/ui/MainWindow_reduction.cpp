#include "MainWindow_p.h"

void MainWindow::onDataReduction()
{
    if (session_->isEmpty()) return;

    if (astrometryClient_->isBusy()) {
        QMessageBox::information(this, tr("Data Reduction"),
            tr("Plate solving is already in progress. Please wait."));
        return;
    }

    // Count images that still need plate solving (no pre-existing WCS)
    int needsSolving = 0;
    for (int i = 0; i < session_->imageCount(); ++i)
        if (!session_->image(i).wcs.solved) ++needsSolving;

    const QString backend = settings_.value(
        QStringLiteral("astrometry/backend"), QStringLiteral("astrometry_net")).toString();
    const bool useAstap = (backend == QStringLiteral("astap"));

    // Validate solver configuration before queuing
    if (needsSolving > 0) {
        if (useAstap) {
            const QString astapPath = settings_.value(QStringLiteral("astrometry/astapPath")).toString();
            if (astapPath.isEmpty()) {
                QMessageBox::warning(this, tr("ASTAP não configurado"),
                    tr("O caminho do executável ASTAP não está configurado.\n\n"
                       "Configure em Arquivo → Configurações → Conexões → ASTAP."));
                return;
            }
            astapSolver_->setAstapPath(astapPath);
        } else {
            // astrometry.net — ensure API key
            QString apiKey = core::ApiKeyStore::read();
            if (apiKey.isEmpty()) {
                bool ok = false;
                apiKey = QInputDialog::getText(this,
                    tr("Astrometry.net API Key"),
                    tr("Enter your free API key from nova.astrometry.net:"),
                    QLineEdit::Normal, QString(), &ok);
                if (!ok || apiKey.trimmed().isEmpty()) return;
                core::ApiKeyStore::write(apiKey.trimmed());
            }
            astrometryClient_->setApiKey(apiKey.trimmed());
        }
    }

    if (needsSolving == 0)
        logPanel_->appendInfo(tr("Todas as imagens já têm solução WCS — plate solving ignorado."));

    // Connect solver signals (disconnect first to avoid duplicates)
    auto connectSolver = [this](auto* solver, const QString& tag) {
        disconnect(solver, nullptr, this, nullptr);
        connect(solver, &std::remove_pointer_t<decltype(solver)>::progress,
                this, [this, tag](const QString& msg, int pct) {
            logPanel_->appendInfo(tr("[%1] %2 (%3%)").arg(tag, msg).arg(pct));
            statusBar()->showMessage(msg);
            workflowPanel_->setStepStatus(1, WorkflowPanel::Status::InProgress);
        });
        connect(solver, &std::remove_pointer_t<decltype(solver)>::solved,
                this, &MainWindow::onAstrometrySolved);
        connect(solver, &std::remove_pointer_t<decltype(solver)>::failed,
                this, &MainWindow::onAstrometryFailed);
    };
    connectSolver(astrometryClient_, QStringLiteral("Astrometry.net"));
    connectSolver(astapSolver_,      QStringLiteral("ASTAP"));

    // Warn if observer location resolves to 0,0 (geocenter).
    const SiteLocation site = effectiveSiteLocation();
    if (site.lat == 0.0 && site.lon == 0.0) {
        const auto btn = QMessageBox::warning(this, tr("Localização não configurada"),
            tr("A latitude e longitude do observador estão em 0°.\n\n"
               "Isso significa que as observações serão reportadas como se fossem feitas do\n"
               "geocentro (Oceano Atlântico), o que é incorreto para astrometria precisa.\n\n"
               "Configure sua localização em Arquivo → Configurações → Observador.\n\n"
               "Deseja continuar assim mesmo?"),
            QMessageBox::Yes | QMessageBox::No);
        if (btn != QMessageBox::Yes) {
            if (QMessageBox::question(this, tr("Abrir Configurações?"),
                    tr("Deseja abrir Configurações agora para configurar a localização?"),
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                onSettings();
            return;
        }
    }
    // Apply effective location to the KOO engine for this reduction pass
    kooEngine_->setObserverLocation(site.lat, site.lon, site.alt);
    {
        const QString mode = settings_.value(
            QStringLiteral("observer/locationMode"), QStringLiteral("fits")).toString();
        logPanel_->appendInfo(tr("Observer location (%1): lat=%2  lon=%3  alt=%4 m")
            .arg(mode)
            .arg(site.lat, 0, 'f', 4)
            .arg(site.lon, 0, 'f', 4)
            .arg(static_cast<int>(site.alt)));
    }

    reductionRunning_ = true;
    actStopReduction_->setEnabled(true);
    workflowPanel_->setStepStatus(1, WorkflowPanel::Status::InProgress);
    const int N = session_->imageCount();
    logPanel_->appendInfo(tr("Starting data reduction for %1 image(s)...").arg(N));

    // ── Step 1: apply settings + calibration to every image (UI thread) ──────
    for (int i = 0; i < N; ++i) {
        core::FitsImage& img = session_->image(i);

        img.saturation = settings_.value(QStringLiteral("camera/saturation"), 60000).toDouble();
        const double timeOffset = settings_.value(QStringLiteral("observer/timeOffset"), 0.0).toDouble();
        if (timeOffset != 0.0 && img.jd > 0.0)
            img.jd += timeOffset / 86400.0;
        const double deltaT = settings_.value(QStringLiteral("camera/deltaT"), 68.0).toDouble();
        if (img.jd > 0.0)
            img.jd += deltaT / 86400.0;
        if (img.pixScaleX == 0.0) {
            const double sx = settings_.value(QStringLiteral("camera/pixelScaleX"), 0.0).toDouble();
            const double sy = settings_.value(QStringLiteral("camera/pixelScaleY"), 0.0).toDouble();
            if (sx > 0.0) img.pixScaleX = sx * img.binningX;
            if (sy > 0.0) img.pixScaleY = sy * img.binningY;
        }
        if (darkFrame_.isValid()) {
            if (darkFrame_.width == img.width && darkFrame_.height == img.height)
                core::applyDark(img, darkFrame_);
            else
                logPanel_->appendWarning(tr("Image %1: dark frame size mismatch — skipped").arg(i+1));
        }
        if (flatFrame_.isValid()) {
            if (flatFrame_.width == img.width && flatFrame_.height == img.height)
                core::applyFlat(img, flatFrame_);
            else
                logPanel_->appendWarning(tr("Image %1: flat field size mismatch — skipped").arg(i+1));
        }
        if (settings_.value(QStringLiteral("camera/badPixelCorrection"), true).toBool()) {
            const double bpSigma = settings_.value(QStringLiteral("camera/badPixelSigma"), 5.0).toDouble();
            const int nFixed = core::applyBadPixelCorrection(img, bpSigma);
            if (nFixed > 0)
                logPanel_->appendInfo(tr("  Image %1: %2 bad pixel(s) corrected (σ=%3)")
                    .arg(i+1).arg(nFixed).arg(bpSigma, 0, 'f', 1));
        }
        if (settings_.value(QStringLiteral("detection/backgroundSubtraction"), false).toBool()) {
            const int tileSize = settings_.value(QStringLiteral("detection/backgroundTileSize"), 64).toInt();
            core::subtractBackground(img, tileSize);
        }
    }

    // ── Step 2: star detection on all images in parallel ─────────────────────
    logPanel_->appendInfo(tr("Detecting stars in %1 image(s) (parallel)...").arg(N));
    statusBar()->showMessage(tr("Detecting stars..."));

    core::StarDetectorConfig cfg;
    cfg.threshold           = settings_.value(QStringLiteral("detection/sigmaLimit"),      4.0).toDouble();
    cfg.streakMinElongation = settings_.value(QStringLiteral("detection/streakElongation"), 3.0).toDouble();
    const double minFwhm    = settings_.value(QStringLiteral("detection/minFwhm"),          0.70).toDouble();

    QVector<int> indices(N);
    std::iota(indices.begin(), indices.end(), 0);

    delete starWatcher_;
    starWatcher_ = new QFutureWatcher<QVector<core::DetectedStar>>(this);
    connect(starWatcher_, &QFutureWatcher<QVector<core::DetectedStar>>::finished,
            this, &MainWindow::onAllStarsDetected);
    connect(starWatcher_, &QFutureWatcher<QVector<core::DetectedStar>>::progressValueChanged,
            this, [this, N](int v) {
                statusBar()->showMessage(tr("Detecting stars... %1/%2").arg(v).arg(N));
            });

    starWatcher_->setFuture(QtConcurrent::mapped(indices,
        [this, cfg, minFwhm](int idx) -> QVector<core::DetectedStar> {
            const core::FitsImage& img = session_->image(idx);
            auto result = core::detectStars(img, cfg);
            if (!result) return {};
            QVector<core::DetectedStar> stars = *result;
            if (minFwhm > 0.0 && img.pixScaleX > 0.0) {
                const double minFwhmPx = minFwhm / img.pixScaleX;
                stars.erase(std::remove_if(stars.begin(), stars.end(),
                    [minFwhmPx](const core::DetectedStar& s){ return s.fwhm() < minFwhmPx; }),
                    stars.end());
            }
            return stars;
        }));
}

void MainWindow::onAllStarsDetected()
{
    if (!reductionRunning_) return;  // cancelled before star detection finished

    const auto results = starWatcher_->future().results();
    for (int i = 0; i < results.size(); ++i) {
        session_->image(i).detectedStars = results[i];

        const int nStreaks = static_cast<int>(std::count_if(
            results[i].begin(), results[i].end(),
            [](const core::DetectedStar& s){ return s.streak; }));

        if (nStreaks > 0)
            logPanel_->appendInfo(tr("Image %1: %2 stars found (%3 streak/trail candidate(s) — shown in orange)")
                .arg(i + 1).arg(results[i].size()).arg(nStreaks));
        else
            logPanel_->appendInfo(tr("Image %1: %2 stars found")
                .arg(i + 1).arg(results[i].size()));

        updateImageOverlay(i);
    }
    statusBar()->clearMessage();

    // Build plate-solving queue and start
    reductionQueue_.clear();
    reductionSolved_  = 0;
    reductionSkipped_ = 0;
    reductionFailed_  = 0;
    for (int i = 0; i < session_->imageCount(); ++i)
        reductionQueue_.append(i);
    processNextReduction();
}

void MainWindow::processNextReduction()
{
    if (reductionQueue_.isEmpty()) {
        reductionRunning_ = false;
        actStopReduction_->setEnabled(false);

        // ── Sumário de plate solving ───────────────────────────────────────
        const int total = session_->imageCount();
        if (reductionSkipped_ == total) {
            logPanel_->appendInfo(tr("Redução concluída: todas as %1 imagens tinham WCS pré-existente.").arg(total));
        } else {
            QString summary = tr("Redução concluída: %1 resolvidas").arg(reductionSolved_);
            if (reductionSkipped_ > 0)
                summary += tr(", %1 com WCS pré-existente").arg(reductionSkipped_);
            if (reductionFailed_ > 0) {
                summary += tr(", %1 falhou").arg(reductionFailed_);
                logPanel_->appendWarning(summary);
                if (reductionFailed_ == total - reductionSkipped_) {
                    logPanel_->appendWarning(tr("Todas as submissões falharam. Verifique a chave API e a conexão com a internet."));
                    // Suggest switching to ASTAP if it is configured and not already in use
                    const QString currentBackend = settings_.value(
                        QStringLiteral("astrometry/backend"), QStringLiteral("astrometry_net")).toString();
                    const QString astapPath = settings_.value(
                        QStringLiteral("astrometry/astapPath")).toString();
                    if (currentBackend != QStringLiteral("astap") && !astapPath.isEmpty())
                        logPanel_->appendInfo(tr("Dica: ASTAP está configurado em seu sistema. "
                            "Mude o backend para ASTAP em Arquivo → Configurações → Conexões para resolver sem internet."));
                }
            } else {
                logPanel_->appendInfo(summary);
            }
        }

        session_->setStep(core::SessionStep::DataReduced);
        onUpdateMenuState();
        if (workflowPanel_->autoWorkflow())
            QTimer::singleShot(0, this, &MainWindow::onKnownObjectOverlay);
        return;
    }

    reducingIdx_ = reductionQueue_.takeFirst();
    core::FitsImage& img = session_->image(reducingIdx_);

    // ── Pular se a imagem já tem WCS (FITS pré-resolvido) ─────────────────
    if (img.wcs.solved) {
        logPanel_->appendInfo(tr("  %1: WCS pré-existente — plate solving ignorado.").arg(img.fileName));
        ++reductionSkipped_;
        workflowPanel_->setStepStatus(1, WorkflowPanel::Status::Ok);
        updateImageOverlay(reducingIdx_);
        processNextReduction();
        return;
    }

    // ── Plate solving — choose backend ────────────────────────────────────
    const QString backend = settings_.value(
        QStringLiteral("astrometry/backend"), QStringLiteral("astrometry_net")).toString();

    if (backend == QStringLiteral("astap")) {
        logPanel_->appendInfo(tr("  %1: enviando ao ASTAP (local)…").arg(img.fileName));
        astapSolver_->solve(img.filePath, img.ra, img.dec);
    } else {
        if (astrometryClient_->apiKey().isEmpty()) {
            logPanel_->appendWarning(tr("  %1: chave API ausente — plate solving ignorado.").arg(img.fileName));
            ++reductionFailed_;
            processNextReduction();
            return;
        }
        logPanel_->appendInfo(tr("  %1: submetendo ao astrometry.net…").arg(img.fileName));
        astrometryClient_->solveFits(img.filePath, img.ra, img.dec);
    }
}

void MainWindow::onAstrometrySolved(const core::PlateSolution& wcs, int /*jobId*/)
{
    if (reducingIdx_ < 0 || reducingIdx_ >= session_->imageCount()) return;

    core::FitsImage& img = session_->image(reducingIdx_);
    img.wcs = wcs;

    // Update pixel scale
    img.pixScaleX = std::abs(std::hypot(wcs.cd1_1, wcs.cd2_1)) * 3600.0;
    img.pixScaleY = std::abs(std::hypot(wcs.cd1_2, wcs.cd2_2)) * 3600.0;

    logPanel_->appendInfo(tr("  Solved! RA=%1 Dec=%2 scale=%3\"/px")
        .arg(wcs.crval1, 0, 'f', 4)
        .arg(wcs.crval2, 0, 'f', 4)
        .arg(img.pixScaleX, 0, 'f', 3));

    if (settings_.value(QStringLiteral("astrometry/saveWcs"), false).toBool()) {
        const QString err = core::saveWcsToFits(img.filePath, wcs);
        if (err.isEmpty())
            logPanel_->appendInfo(tr("  WCS written back to FITS file."));
        else
            logPanel_->appendWarning(tr("  Could not save WCS: %1").arg(err));
    }

    ++reductionSolved_;
    workflowPanel_->setStepStatus(1, WorkflowPanel::Status::Ok);
    updateImageOverlay(reducingIdx_);
    processNextReduction();
}

void MainWindow::onAstrometryFailed(const QString& reason)
{
    logPanel_->appendWarning(tr("  Plate solving falhou: %1").arg(reason));
    ++reductionFailed_;
    workflowPanel_->setStepStatus(1, WorkflowPanel::Status::Error);
    processNextReduction();
}

void MainWindow::onStopReduction()
{
    if (!reductionRunning_) return;

    // Stop star detection if still running
    if (starWatcher_ && starWatcher_->isRunning()) {
        disconnect(starWatcher_, nullptr, this, nullptr);
        starWatcher_->cancel();
    }
    // Cancel whichever solver is active
    astrometryClient_->cancel();
    astapSolver_->cancel();

    reductionQueue_.clear();
    reducingIdx_      = -1;
    reductionRunning_ = false;
    actStopReduction_->setEnabled(false);
    workflowPanel_->setStepStatus(1, WorkflowPanel::Status::Idle);
    logPanel_->appendWarning(tr("Data reduction stopped by user."));
    onUpdateMenuState();
}

void MainWindow::onMovingObjectDetection()
{
    if (!session_ || session_->imageCount() < 2) {
        QMessageBox::information(this, tr("Moving Object Detection"),
            tr("Load at least 2 images with completed data reduction first."));
        return;
    }

    // Check that at least some images have detected stars
    QVector<core::FitsImage> imgs;
    for (int i = 0; i < session_->imageCount(); ++i)
        imgs.append(session_->image(i));

    bool hasStars = false;
    for (const auto& img : imgs)
        if (!img.detectedStars.isEmpty()) { hasStars = true; break; }

    if (!hasStars) {
        QMessageBox::information(this, tr("Moving Object Detection"),
            tr("Run Data Reduction first to detect stars in the images."));
        return;
    }

    QProgressDialog prog(tr("Detecting moving objects…"), QString(), 0, 0, this);
    prog.setWindowModality(Qt::WindowModal);
    prog.show();
    QApplication::processEvents();

    const double tolerancePx = settings_.value("detection/modTolerance", 2.0).toDouble();
    const int    minFrames   = settings_.value("detection/modMinFrames", 3).toInt();
    const double minSnr      = settings_.value("detection/minSnr",       5.0).toDouble();

    auto result = core::detectMovingObjects(imgs, tolerancePx, minFrames, minSnr);
    prog.close();

    if (!result) {
        QMessageBox::warning(this, tr("Moving Object Detection"), result.error());
        return;
    }

    const auto& candidates = *result;
    if (candidates.isEmpty()) {
        QMessageBox::information(this, tr("Moving Object Detection"),
            tr("No moving objects detected across %1 images.").arg(imgs.size()));
        return;
    }

    // Overlay candidates on each image as KooObjects
    // First clear any previous MOD overlays
    for (int fi = 0; fi < session_->imageCount(); ++fi) {
        auto& koos = session_->image(fi).kooObjects;
        koos.erase(std::remove_if(koos.begin(), koos.end(),
            [](const core::KooObject& k){ return k.type == QStringLiteral("moving_candidate"); }),
            koos.end());
    }

    for (int i = 0; i < candidates.size(); ++i) {
        const auto& c = candidates[i];
        // positions[0] was detected in frame 0; use linear motion to predict all frames
        const QPointF p0 = c.positions[0];
        for (int fi = 0; fi < session_->imageCount(); ++fi) {
            core::FitsImage& img = session_->image(fi);
            if (!img.wcs.solved) continue;
            const double px = p0.x() + fi * c.dxPerFrame;
            const double py = p0.y() + fi * c.dyPerFrame;
            if (px < 0 || px >= img.width || py < 0 || py >= img.height) continue;
            double ra = 0.0, dec = 0.0;
            img.wcs.pixToSky(px, py, ra, dec);
            core::KooObject koo;
            koo.name   = tr("Cand #%1").arg(i + 1);
            koo.type   = QStringLiteral("moving_candidate");
            koo.ra     = ra;
            koo.dec    = dec;
            koo.dRA    = c.dxPerFrame * img.pixScaleX * 3600.0;  // arcsec/frame → approx
            koo.dDec   = c.dyPerFrame * img.pixScaleY * 3600.0;
            img.kooObjects.append(koo);
        }
    }
    for (int fi = 0; fi < session_->imageCount(); ++fi)
        updateImageOverlay(fi);

    // Refresh blink widget if active (it holds a stale copy of the images)
    if (blinkActive_ && blinkWidget_) {
        QVector<core::FitsImage> imgs;
        imgs.reserve(session_->imageCount());
        for (int i = 0; i < session_->imageCount(); ++i)
            imgs.append(session_->image(i));
        blinkWidget_->updateImageMetadata(imgs);
    }

    // Report results
    QStringList lines;
    lines << tr("Found %1 candidate(s):").arg(candidates.size()) << QString();
    for (int i = 0; i < candidates.size(); ++i) {
        const auto& c = candidates[i];
        lines << tr("  #%1  dx=%2  dy=%3 px/frame  speed=%4 px/frame  SNR≥%5  frames=%6")
                    .arg(i + 1)
                    .arg(c.dxPerFrame, 0, 'f', 2)
                    .arg(c.dyPerFrame, 0, 'f', 2)
                    .arg(c.speed,      0, 'f', 2)
                    .arg(c.snr,        0, 'f', 1)
                    .arg(c.positions.size());
    }
    QMessageBox::information(this, tr("Moving Object Detection"), lines.join('\n'));
    spdlog::info("MOD: {} candidate(s) found", static_cast<int>(candidates.size()));
}

void MainWindow::onTrackAndStack()
{
    if (!session_ || session_->imageCount() < 2) {
        QMessageBox::information(this, tr("Track & Stack"),
            tr("Load at least 2 images first."));
        return;
    }

    // Ask for motion vector
    bool ok = false;
    const double dx = QInputDialog::getDouble(this, tr("Track & Stack"),
        tr("Pixel shift per frame in X (dX/frame):"),
        0.0, -500.0, 500.0, 2, &ok);
    if (!ok) return;
    const double dy = QInputDialog::getDouble(this, tr("Track & Stack"),
        tr("Pixel shift per frame in Y (dY/frame):"),
        0.0, -500.0, 500.0, 2, &ok);
    if (!ok) return;

    bool modeOk = false;
    const QStringList modeNames = { tr("Average"), tr("Median"), tr("Add") };
    const QString chosen = QInputDialog::getItem(this, tr("Track & Stack"),
        tr("Combination method:"), modeNames, 0, false, &modeOk);
    if (!modeOk) return;
    const core::StackMode mode = (chosen == tr("Median"))  ? core::StackMode::Median
                               : (chosen == tr("Add"))     ? core::StackMode::Add
                                                           : core::StackMode::Average;

    QProgressDialog prog(tr("Stacking images…"), QString(), 0, 100, this);
    prog.setWindowModality(Qt::WindowModal);
    prog.show();

    QVector<core::FitsImage> imgs;
    for (int i = 0; i < session_->imageCount(); ++i)
        imgs.append(session_->image(i));

    auto result = core::trackAndStack(imgs, dx, dy, mode);
    prog.setValue(100);
    prog.close();

    if (!result) {
        QMessageBox::warning(this, tr("Track & Stack"), result.error());
        return;
    }

    auto& stacked = result->image;
    stacked.fileName = QStringLiteral("track_stacked.fits");

    auto* sw    = new FitsSubWindow(stacked, this);
    auto* mdiSw = mdiArea_->addSubWindow(sw);
    mdiSw->setWindowTitle(stacked.fileName);
    mdiSw->setWindowIcon(AppIcons::loadImages(16));
    mdiSw->show();
    // Defer geometry: MDI area overrides geometry set before show()
    QTimer::singleShot(0, this, [this, mdiSw, sw]() {
        mdiSw->setGeometry(QRect(0, 0, mdiArea_->width(), mdiArea_->height()));
        mdiArea_->setActiveSubWindow(mdiSw);
        sw->imageView()->setFocus();
    });

    spdlog::info("Track&Stack: {} images combined (dx={:.2f}, dy={:.2f})",
                 static_cast<int>(imgs.size()), dx, dy);
}

// ─── Images slots ─────────────────────────────────────────────────────────────

void MainWindow::onEditImageParameters()
{
    auto* fw = activeFitsWindow();
    if (!fw) { statusBar()->showMessage(tr("No active image"), 2000); return; }

    // Find matching session image
    int sessionIdx = -1;
    for (int i = 0; i < session_->imageCount(); ++i) {
        if (session_->image(i).filePath == fw->fitsImage().filePath) {
            sessionIdx = i; break;
        }
    }

    EditImageParametersDialog dlg(fw->fitsImage(), this);
    if (dlg.exec() != QDialog::Accepted) return;

    // Apply to session image and refresh subwindow
    if (sessionIdx >= 0)
        session_->image(sessionIdx) = dlg.apply(session_->image(sessionIdx));
    fw->updateImage(dlg.apply(fw->fitsImage()));
    statusBar()->showMessage(tr("Image parameters updated"), 3000);
}
void MainWindow::onDisplayHeader()
{
    auto* fw = activeFitsWindow();
    if (fw) fw->showFitsHeader();
}
void MainWindow::onBackgroundAndRange()
{
    auto* fw = activeFitsWindow();
    if (!fw) { statusBar()->showMessage(tr("No active image"), 2000); return; }

    // Find corresponding FitsImage in session (match by file path)
    core::FitsImage* imgPtr = nullptr;
    for (int i = 0; i < session_->imageCount(); ++i) {
        if (session_->image(i).filePath == fw->fitsImage().filePath) {
            imgPtr = &session_->image(i);
            break;
        }
    }
    if (!imgPtr) return;

    auto* dlg = new BackgroundRangeDialog(fw->imageView(), imgPtr, this);
    connect(dlg, &BackgroundRangeDialog::stretchChanged,
        this, [this](float mn, float mx, core::StretchMode mode, core::ColorLut lut, bool all) {
            if (!all) return;
            for (auto* fsw : allFitsWindows()) {
                for (int i = 0; i < session_->imageCount(); ++i) {
                    if (session_->image(i).filePath == fsw->fitsImage().filePath) {
                        session_->image(i).displayMin  = mn;
                        session_->image(i).displayMax  = mx;
                        session_->image(i).stretchMode = mode;
                        session_->image(i).colorLut    = lut;
                        fsw->imageView()->setStretchMode(mode);
                        fsw->imageView()->setColorLut(lut);
                        fsw->imageView()->setStretch(mn, mx);
                        break;
                    }
                }
            }
        });
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
void MainWindow::onReStackImages()
{
    if (!session_ || session_->imageCount() < 2) {
        QMessageBox::information(this, tr("Re-Stack Images"),
            tr("Load at least 2 images first."));
        return;
    }

    bool modeOk = false;
    const QStringList modeNames = { tr("Average"), tr("Median"), tr("Add") };
    const QString chosen = QInputDialog::getItem(this, tr("Re-Stack Images"),
        tr("Combination method:"), modeNames, 0, false, &modeOk);
    if (!modeOk) return;
    const core::StackMode mode = (chosen == tr("Median"))  ? core::StackMode::Median
                               : (chosen == tr("Add"))     ? core::StackMode::Add
                                                           : core::StackMode::Average;

    // Alignment: use star catalog when all images have detected stars
    const bool allHaveStars = [&] {
        for (int i = 0; i < session_->imageCount(); ++i)
            if (session_->image(i).detectedStars.isEmpty()) return false;
        return true;
    }();
    const core::AlignMode align = allHaveStars ? core::AlignMode::StarCatalog
                                               : core::AlignMode::FFT;

    QProgressDialog prog(tr("Stacking images…"), QString(), 0, 100, this);
    prog.setWindowModality(Qt::WindowModal);
    prog.show();

    QVector<core::FitsImage> imgs;
    for (int i = 0; i < session_->imageCount(); ++i)
        imgs.append(session_->image(i));

    auto result = core::stackImages(imgs, mode, align, [&](int pct){ prog.setValue(pct); QApplication::processEvents(); });
    prog.setValue(100);
    prog.close();

    if (!result) {
        QMessageBox::warning(this, tr("Re-Stack Images"), result.error());
        return;
    }

    auto& stacked = result->image;
    stacked.fileName = QStringLiteral("stacked.fits");

    // Log shifts
    for (int i = 0; i < result->shifts.size(); ++i)
        spdlog::info("Stack: image {} shift ({:.2f}, {:.2f})",
                     i, static_cast<double>(result->shifts[i].x()),
                        static_cast<double>(result->shifts[i].y()));

    auto* sw    = new FitsSubWindow(stacked, this);
    auto* mdiSw = mdiArea_->addSubWindow(sw);
    mdiSw->setWindowTitle(stacked.fileName);
    mdiSw->setWindowIcon(AppIcons::loadImages(16));
    mdiSw->show();
    QTimer::singleShot(0, this, [this, mdiSw, sw]() {
        mdiSw->setGeometry(QRect(0, 0, mdiArea_->width(), mdiArea_->height()));
        mdiArea_->setActiveSubWindow(mdiSw);
        sw->imageView()->setFocus();
    });

    spdlog::info("Re-Stack: {} images combined", static_cast<int>(imgs.size()));
}

void MainWindow::onZoomIn()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) { v->zoomIn(); v->setFocus(); }
}
void MainWindow::onZoomOut()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) { v->zoomOut(); v->setFocus(); }
}
void MainWindow::onFitWindowSize()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) { v->fitToWidget(); v->setFocus(); }
}
void MainWindow::onSelectMarkings()
{
    SelectMarkingsDialog dlg(markingsState_, this);
    if (dlg.exec() != QDialog::Accepted) return;
    markingsState_ = dlg.state();
    for (auto* fw : allFitsWindows()) {
        fw->imageView()->setShowDetectedStars(markingsState_.detectedStars);
        fw->imageView()->setShowCatalogStars (markingsState_.catalogStars);
        fw->imageView()->setShowKooObjects   (markingsState_.kooObjects);
        fw->imageView()->setShowLabels       (markingsState_.labels);
    }
}
void MainWindow::onInvertDisplay()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) {
        v->toggleInvert();
        actInvertDisplay_->setChecked(v->inverted());
    }
}
void MainWindow::onFlipHorizontal()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) v->toggleFlipH();
}
void MainWindow::onFlipVertical()
{
    FitsImageView* v = nullptr;
    if (auto* fw = activeFitsWindow()) v = fw->imageView();
    else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
    if (v) v->toggleFlipV();
}
void MainWindow::onMagnifyingGlass()
{
    // Toggle magnifier on ALL open images
    const bool newState = [this]() {
        auto wins = allFitsWindows();
        if (wins.isEmpty()) return false;
        return !wins.first()->imageView()->showMagnifier();
    }();

    for (auto* fw : allFitsWindows())
        fw->imageView()->setShowMagnifier(newState);

    statusBar()->showMessage(
        newState ? tr("Magnifying glass ON — move cursor over image  (Ctrl+G to toggle)")
                 : tr("Magnifying glass OFF"), 3000);
}

// ─── Region statistics ────────────────────────────────────────────────────────

void MainWindow::onRegionStatsTool()
{
    auto* fw = activeFitsWindow();
    if (!fw) return;

    auto* view = fw->imageView();
    const bool newState = !view->regionMode();

    // Disable region mode on all windows, then enable only on the active one
    for (auto* w : allFitsWindows())
        w->imageView()->setRegionMode(false);

    if (newState) {
        view->setRegionMode(true);
        view->setToolCursor(Qt::CrossCursor);
        statusBar()->showMessage(
            tr("Estatísticas de Região: arraste um retângulo na imagem  (Esc para cancelar)"),
            5000);
    } else {
        view->setToolCursor(Qt::ArrowCursor);
        if (regionStatsPanel_) regionStatsPanel_->clear();
    }
}

void MainWindow::onRegionSelected(QRect imageRect)
{
    auto* fw = activeFitsWindow();
    if (!fw || !fw->fitsImage().isValid()) return;

    auto*                   view = fw->imageView();
    const core::FitsImage&  img  = fw->fitsImage();

    // Create panel lazily, parented to the FitsImageView so it stays inside the image
    if (!regionStatsPanel_) {
        regionStatsPanel_ = new RegionStatsPanel(view);
        // Auto-null when panel is destroyed (e.g. when the subwindow closes)
        connect(regionStatsPanel_, &QObject::destroyed, this, [this]() {
            regionStatsPanel_ = nullptr;
        });
        connect(regionStatsPanel_, &RegionStatsPanel::closeRequested, this, [this, view]() {
            if (regionStatsPanel_) regionStatsPanel_->clear();
            view->setRegionMode(false);
            view->setToolCursor(Qt::ArrowCursor);
        });
    }

    regionStatsPanel_->showStats(img.data, img.width, img.height, imageRect);

    // Position in bottom-left of the view
    constexpr int kMargin = 8;
    regionStatsPanel_->move(kMargin,
        view->height() - regionStatsPanel_->height() - kMargin);
}

// ─── Tools slots ──────────────────────────────────────────────────────────────

void MainWindow::onBlinkImages()
{
    if (session_->imageCount() < 2) {
        QMessageBox::information(this, tr("Blink Images"),
            tr("Load at least 2 images to use blink mode."));
        return;
    }

    if (!blinkWidget_) {
        blinkWidget_ = new BlinkWidget(nullptr);
        connect(blinkWidget_, &BlinkWidget::stopRequested,
                this, &MainWindow::onStopBlinking);
        connect(blinkWidget_, &BlinkWidget::imageChanged,
                thumbnailBar_, &ThumbnailBar::setActiveIndex);
        connect(blinkWidget_, &BlinkWidget::objectClicked,
            this, [this](QPointF pos, double ra, double dec,
                         float /*val*/, int imgIdx) {
                thumbnailBar_->setActiveIndex(imgIdx);
                if (measureMode_) {
                    // If blink is playing, ask user whether to stop and measure
                    if (blinkWidget_->isBlinking()) {
                        QMessageBox dlg(this);
                        dlg.setWindowTitle(tr("Blink em execução"));
                        dlg.setText(tr("O blink está em execução.\n\n"
                                       "Para medir um objeto é necessário pausar o blink."));
                        auto* btnStop = dlg.addButton(tr("Parar e Medir"), QMessageBox::AcceptRole);
                        dlg.addButton(tr("Continuar Blink"), QMessageBox::RejectRole);
                        dlg.setDefaultButton(btnStop);
                        dlg.exec();
                        if (dlg.clickedButton() != btnStop) return;
                        blinkWidget_->stopBlink();
                    }
                    session_->setStep(core::SessionStep::Measuring);
                    runMeasurePipeline(imgIdx, pos, ra, dec);
                } else {
                    imageCatalogTable_->selectRowByPixel(pos.x(), pos.y());
                }
            });
        connect(blinkWidget_->imageView(), &FitsImageView::escapePressed, this, [this]() {
            imageCatalogTable_->clearSelection();
        });
    }

    // Load session images into blink widget
    QVector<core::FitsImage> imgs;
    imgs.reserve(session_->imageCount());
    for (int i = 0; i < session_->imageCount(); ++i)
        imgs.append(session_->image(i));
    blinkWidget_->setImages(imgs);
    blinkWidget_->setInterval(blinkIntervalMs_);

    if (!blinkMdiWin_) {
        blinkMdiWin_ = mdiArea_->addSubWindow(blinkWidget_,
            Qt::SubWindow | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
        blinkMdiWin_->setWindowTitle(tr("Blink View"));
    }
    blinkMdiWin_->showMaximized();
    blinkWidget_->startBlink();

    blinkActive_     = true;
    blinkCurrentIdx_ = 0;
    actBlink_->setEnabled(false);
    actStopBlink_->setEnabled(true);
    session_->setStep(core::SessionStep::Blinking);
    sbStep_->setText(tr("Blinking — Space = pause, Left/Right = step, click object to measure"));
    logPanel_->appendInfo(tr("Blink started (%1 ms interval)").arg(blinkIntervalMs_));
    onUpdateMenuState();
}

void MainWindow::onStopBlinking()
{
    if (blinkWidget_)
        blinkWidget_->stopBlink();

    if (blinkMdiWin_) {
        blinkMdiWin_->close();
        blinkMdiWin_ = nullptr;
        blinkWidget_ = nullptr;
    }

    blinkActive_ = false;
    blinkTimer_->stop();
    actBlink_->setEnabled(true);
    actStopBlink_->setEnabled(false);
    sbStep_->setText(tr("Blink stopped — click on a moving object to measure"));
    logPanel_->appendInfo(tr("Blink stopped"));
    onUpdateMenuState();
}

void MainWindow::onKnownObjectOverlay()
{
    if (session_->isEmpty()) return;

    disconnect(catalogClient_, nullptr, this, nullptr);
    disconnect(kooEngine_,     nullptr, this, nullptr);

    connect(catalogClient_, &core::CatalogClient::starsReady,
            this, &MainWindow::onCatalogReady);
    connect(catalogClient_, &core::CatalogClient::failed,
            this, [this](const QString& r) {
        logPanel_->appendWarning(tr("Catalog download failed: %1").arg(r));
        workflowPanel_->setStepStatus(2, WorkflowPanel::Status::Error);
    });
    connect(kooEngine_, &core::KooEngine::objectsReady,
            this, &MainWindow::onKooReady);
    connect(kooEngine_, &core::KooEngine::failed,
            this, [this](const QString& r) {
        logPanel_->appendWarning(tr("SkyBoT query failed: %1").arg(r));
        workflowPanel_->setStepStatus(2, WorkflowPanel::Status::Error);
    });

    // Use the first image's solved WCS for the field center
    const core::FitsImage& ref = session_->image(0);
    const double ra      = ref.wcs.solved ? ref.wcs.crval1 : ref.ra;
    const double dec     = ref.wcs.solved ? ref.wcs.crval2 : ref.dec;
    const double fovDeg  = ref.wcs.solved
        ? std::max(ref.pixScaleX, ref.pixScaleY) * std::max(ref.width, ref.height) / 3600.0 * 0.7
        : 0.5;
    const double jd      = ref.jd;

    const QString catType = settings_.value(QStringLiteral("catalog/type"),
                                             QStringLiteral("UCAC4")).toString();
    catalogClient_->setCatalogType(catType);

    workflowPanel_->setStepStatus(2, WorkflowPanel::Status::InProgress);
    const double magLim = settings_.value(QStringLiteral("catalog/magLimit"), 16.0).toDouble();

    const QString catSource = settings_.value(QStringLiteral("catalog/source"),
                                              QStringLiteral("vizier")).toString();
    if (catSource == QStringLiteral("local")) {
        const QString localPath = settings_.value(QStringLiteral("catalog/localPath")).toString();
        if (localPath.isEmpty()) {
            QMessageBox::warning(this, tr("Local Catalog"),
                tr("No local catalog file configured. Set it in Settings → Connections."));
        } else {
            auto localResult = core::readLocalCatalogTable(localPath, ra, dec, fovDeg);
            if (!localResult) {
                logPanel_->appendWarning(tr("Local catalog error: %1").arg(localResult.error()));
                workflowPanel_->setStepStatus(2, WorkflowPanel::Status::Error);
            } else {
                logPanel_->appendInfo(tr("Local catalog: %1 stars from '%2'")
                    .arg(localResult->size()).arg(QFileInfo(localPath).fileName()));
                onCatalogReady(*localResult);
            }
        }
        if (jd > 2400000.0)
            kooEngine_->queryField(ra, dec, fovDeg, jd);
        return;
    }
    // original VizieR path follows...
    logPanel_->appendInfo(tr("Querying VizieR (%1) and SkyBoT at RA=%2 Dec=%3 r=%4°...")
        .arg(catType)
        .arg(ra, 0, 'f', 4).arg(dec, 0, 'f', 4).arg(fovDeg, 0, 'f', 2));

    catalogClient_->queryRegion(ra, dec, fovDeg, magLim);
    if (jd > 2400000.0)
        kooEngine_->queryField(ra, dec, fovDeg, jd);
}

void MainWindow::onLightCurve()
{
    if (!lightCurveDlg_) {
        lightCurveDlg_ = new LightCurveDialog(this);
        lightCurveDlg_->setAttribute(Qt::WA_DeleteOnClose);
        connect(lightCurveDlg_, &LightCurveDialog::destroyed,
                this, [this]{ lightCurveDlg_ = nullptr; });
        // Helper: session index from a FitsSubWindow (match by filePath)
        auto sessionIdxOf = [this](const FitsSubWindow* sw) {
            const QString& path = sw->fitsImage().filePath;
            for (int i = 0; i < session_->imageCount(); ++i)
                if (session_->image(i).filePath == path) return i;
            return -1;
        };

        connect(lightCurveDlg_, &LightCurveDialog::imageActivated,
                this, [this](int imgIdx) {
            for (QMdiSubWindow* mdiSw : mdiArea_->subWindowList()) {
                auto* fsw = qobject_cast<FitsSubWindow*>(mdiSw->widget());
                if (!fsw) continue;
                const QString& path = fsw->fitsImage().filePath;
                if (imgIdx >= 0 && imgIdx < session_->imageCount() &&
                    session_->image(imgIdx).filePath == path)
                {
                    mdiArea_->setActiveSubWindow(mdiSw);
                    mdiSw->showNormal();
                    break;
                }
            }
        });
        // Keep plot in sync with active sub-window
        connect(mdiArea_, &QMdiArea::subWindowActivated, lightCurveDlg_,
                [this, sessionIdxOf](QMdiSubWindow* w) {
            if (!w || !lightCurveDlg_) return;
            auto* fsw = qobject_cast<FitsSubWindow*>(w->widget());
            if (fsw) lightCurveDlg_->setActiveImage(sessionIdxOf(fsw));
        });
    }
    lightCurveDlg_->setObservations(measureTable_->observations());
    lightCurveDlg_->show();
    lightCurveDlg_->raise();
    lightCurveDlg_->activateWindow();
}

void MainWindow::onGrowthCurve()
{
    // Get the active FitsSubWindow
    QMdiSubWindow* active = mdiArea_->activeSubWindow();
    if (!active) {
        QMessageBox::information(this, tr("Growth Curve"),
                                 tr("Open a FITS image first."));
        return;
    }
    auto* fsw = qobject_cast<FitsSubWindow*>(active->widget());
    if (!fsw) return;

    const core::FitsImage& img = fsw->fitsImage();
    if (img.detectedStars.isEmpty()) {
        QMessageBox::information(this, tr("Growth Curve"),
                                 tr("No stars detected in this image.\n"
                                    "Run plate solving or star detection first."));
        return;
    }

    auto* dlg = new GrowthCurveDialog(img, settings_, this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void MainWindow::onCompareSessions()
{
    if (!session_ || session_->imageCount() == 0) {
        QMessageBox::information(this, tr("Compare Sessions"),
                                 tr("Load images into the active session first."));
        return;
    }

    auto* dlg = new SessionCompareDialog(session_.get(), this);
    connect(dlg, &SessionCompareDialog::compareFinished,
            this, [this](core::CompareResult result) {
        // Create a new FitsSubWindow for the residual image
        core::computeAutoStretch(result.residual);
        auto* sw = new FitsSubWindow(result.residual, this);
        auto* mdiSw = mdiArea_->addSubWindow(sw);
        mdiSw->setWindowTitle(tr("Residual"));
        mdiSw->resize(600, 500);
        mdiSw->show();
        logPanel_->appendInfo(tr("Session comparison: %1 residual source(s) detected")
                                  .arg(result.detectionCount));
    });
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

void MainWindow::onCatalogReady(const QVector<core::CatalogStar>& stars)
{
    // Apply bright-limit filter (saturation protection)
    const double maxMag = settings_.value(QStringLiteral("catalog/maxMag"), 10.0).toDouble();
    QVector<core::CatalogStar> filtered;
    filtered.reserve(stars.size());
    for (const auto& s : stars)
        if (s.mag >= maxMag)
            filtered.append(s);

    const QString catType = settings_.value(QStringLiteral("catalog/type"),
                                             QStringLiteral("UCAC4")).toString();
    logPanel_->appendInfo(tr("Catalog: %1 %2 stars (of %3 total, bright limit mag %4)")
        .arg(filtered.size()).arg(catType).arg(stars.size()).arg(maxMag, 0, 'f', 1));

    for (int i = 0; i < session_->imageCount(); ++i) {
        session_->image(i).catalogStars = filtered;
        updateImageOverlay(i);
    }

    // Compute WCS astrometric residuals per image (match detected stars → catalog)
    for (int i = 0; i < session_->imageCount(); ++i) {
        core::FitsImage& img = session_->image(i);
        if (!img.wcs.solved || img.detectedStars.isEmpty() || filtered.isEmpty()) continue;

        const double pixScale = (img.pixScaleX > 0.0) ? img.pixScaleX : 1.0;
        double sumSq = 0.0;
        int    count = 0;

        for (const auto& cat : filtered) {
            double cpx = 0.0, cpy = 0.0;
            img.wcs.skyToPix(cat.ra, cat.dec, cpx, cpy);
            if (cpx < 0 || cpx >= img.width || cpy < 0 || cpy >= img.height) continue;

            double best = 5.0;  // pixel match threshold
            for (const auto& det : img.detectedStars) {
                const double dx = det.x - cpx;
                const double dy = det.y - cpy;
                const double d  = std::sqrt(dx * dx + dy * dy);
                if (d < best) best = d;
            }
            if (best < 5.0) {
                sumSq += best * best;
                ++count;
            }
        }

        if (count >= 3) {
            img.wcs.rms = std::sqrt(sumSq / count) * pixScale;
            logPanel_->appendInfo(tr("  Image %1: WCS RMS = %2\" (%3 matched stars)")
                .arg(i + 1).arg(img.wcs.rms, 0, 'f', 3).arg(count));
        }
    }

    workflowPanel_->setStepStatus(2, WorkflowPanel::Status::Ok);
    if (session_->step() < core::SessionStep::OverlayApplied)
        session_->setStep(core::SessionStep::OverlayApplied);
    onUpdateMenuState();
    if (workflowPanel_->autoWorkflow() && !blinkActive_)
        QTimer::singleShot(0, this, &MainWindow::onBlinkImages);
}

void MainWindow::onKooReady(const QVector<core::KooObject>& objects)
{
    logPanel_->appendInfo(tr("SkyBoT: %1 known object(s) in field").arg(objects.size()));
    for (int i = 0; i < session_->imageCount(); ++i) {
        session_->image(i).kooObjects = objects;
        updateImageOverlay(i);
    }
    if (!objects.isEmpty()) {
        QStringList names;
        for (const auto& obj : objects)
            names.append(obj.name);
        logPanel_->appendInfo(tr("  Objects: %1").arg(names.join(", ")));
    }
    workflowPanel_->setStepStatus(2, WorkflowPanel::Status::Ok);
}

void MainWindow::updateImageOverlay(int idx)
{
    if (idx < 0 || idx >= session_->imageCount()) return;
    const core::FitsImage& img = session_->image(idx);

    // Find the FitsSubWindow displaying this image and refresh its overlay
    FitsSubWindow* activeFw = nullptr;
    for (auto* sw : mdiArea_->subWindowList()) {
        auto* fw = qobject_cast<FitsSubWindow*>(sw->widget());
        if (fw && fw->fitsImage().filePath == img.filePath) {
            fw->updateImage(img);
            if (sw == mdiArea_->activeSubWindow()) activeFw = fw;
            break;
        }
    }

    // Refresh Image Catalog dock if it is visible and shows this image
    if (activeFw) {
        if (auto* dock = qobject_cast<QDockWidget*>(imageCatalogTable_->parent())) {
            if (dock->isVisible()) {
                imageCatalogTable_->setImage(img);
                clearAllHighlights();
            } else if (!img.detectedStars.isEmpty()) {
                // Auto-show dock the first time stars are detected
                imageCatalogTable_->setImage(img);
                dock->show();
                clearAllHighlights();
            }
        }
    }
}

void MainWindow::updateInfoBar()
{
    if (!infoBar_ || session_->isEmpty()) return;
    const SiteLocation site = effectiveSiteLocation();
    const bool locationOk   = (site.lat != 0.0 || site.lon != 0.0);
    const QString mode = settings_.value(
        QStringLiteral("observer/locationMode"), QStringLiteral("fits")).toString();
    infoBar_->showForSession(*session_, mode, locationOk);
}

void MainWindow::clearAllHighlights()
{
    for (auto* sw : mdiArea_->subWindowList()) {
        if (auto* fw = qobject_cast<FitsSubWindow*>(sw->widget()))
            fw->imageView()->clearHighlight();
    }
    if (blinkActive_ && blinkWidget_)
        blinkWidget_->imageView()->clearHighlight();
}

// ─── Phase 4: Measurement pipeline ───────────────────────────────────────────

