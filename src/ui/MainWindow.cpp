#include "MainWindow.h"
#include "FitsSubWindow.h"
#include "WorkflowPanel.h"
#include "LogPanel.h"
#include "AboutDialog.h"
#include "BlinkWidget.h"
#include "ThumbnailBar.h"
#include "BackgroundRangeDialog.h"

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QCloseEvent>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>
#include <QKeySequence>
#include <QProgressDialog>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QNetworkAccessManager>

#include "core/StarDetector.h"

#include <spdlog/spdlog.h>

// ─── Constructor / Destructor ─────────────────────────────────────────────────

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , session_(std::make_unique<core::ImageSession>(this))
    , blinkTimer_(new QTimer(this))
{
    setWindowTitle(tr("AstroFind"));
    setMinimumSize(900, 650);

    // MDI area (central widget)
    mdiArea_ = new QMdiArea(this);
    mdiArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea_->setBackground(QBrush(QColor(0x1a, 0x1a, 0x2e)));
    setCentralWidget(mdiArea_);

    // ── Phase 2: network + astrometry clients ─────────────────────────────
    nam_              = new QNetworkAccessManager(this);
    astrometryClient_ = new core::AstrometryClient(nam_, this);
    catalogClient_    = new core::CatalogClient(nam_, this);
    kooEngine_        = new core::KooEngine(nam_, this);

    setupMenus();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
    setupConnections();

    loadSettings();
    onUpdateMenuState();

    spdlog::info("MainWindow created");
}

MainWindow::~MainWindow() = default;

// ─── Setup ────────────────────────────────────────────────────────────────────

void MainWindow::setupMenus()
{
    // ── File ─────────────────────────────────────────────────────────────────
    fileMenu_ = menuBar()->addMenu(tr("&File"));

    actLoadImages_ = fileMenu_->addAction(tr("&Load Images..."), QKeySequence("Ctrl+L"), this, &MainWindow::onLoadImages);
    actLoadImages_->setStatusTip(tr("Load FITS images for a new observation session"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Save &all FITS"), this, &MainWindow::onSaveAllFits)
        ->setStatusTip(tr("Save all open images as FITS files"));
    fileMenu_->addAction(tr("&Save as FITS..."), this, &MainWindow::onSaveAsFits)
        ->setStatusTip(tr("Save the active image as a FITS file"));

    auto* exportMenu = fileMenu_->addMenu(tr("&Export Image to..."));
    exportMenu->addAction(tr("JPEG..."));
    exportMenu->addAction(tr("Bitmap..."));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Load &Dark Frame..."), this, &MainWindow::onLoadDarkFrame)
        ->setStatusTip(tr("Load a dark frame for calibration"));
    fileMenu_->addAction(tr("Load &Flat Field..."), this, &MainWindow::onLoadFlatField)
        ->setStatusTip(tr("Load a flat field for calibration"));

    fileMenu_->addSeparator();
    actViewAdes_ = fileMenu_->addAction(tr("View &ADES Report File"), this, &MainWindow::onViewAdesReport);
    actViewAdes_->setStatusTip(tr("View the generated ADES astrometry report"));
    actViewPhot_ = fileMenu_->addAction(tr("View P&hotometry File"), this, &MainWindow::onViewPhotometryFile);
    actViewPhot_->setStatusTip(tr("View the photometry measurements file"));
    fileMenu_->addAction(tr("View Lo&g File"), this, &MainWindow::onViewLogFile)
        ->setStatusTip(tr("View the session log file"));

    fileMenu_->addSeparator();
    actResetFiles_ = fileMenu_->addAction(tr("&Reset Files"), QKeySequence("Ctrl+R"), this, &MainWindow::onResetFiles);
    actResetFiles_->setStatusTip(tr("Reset session and close all images"));
    fileMenu_->addAction(tr("Reload MPC&Orb"), this, &MainWindow::onReloadMpcOrb)
        ->setStatusTip(tr("Reload the MPCOrb asteroid database from disk"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Se&ttings..."), QKeySequence("Ctrl+,"), this, &MainWindow::onSettings)
        ->setStatusTip(tr("Edit program settings"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("E&xit"), QKeySequence::Quit, this, &MainWindow::onExit);

    // ── Astrometry ────────────────────────────────────────────────────────────
    astrometryMenu_ = menuBar()->addMenu(tr("&Astrometry"));

    actDataReduction_ = astrometryMenu_->addAction(tr("&Data Reduction..."), QKeySequence("Ctrl+A"), this, &MainWindow::onDataReduction);
    actDataReduction_->setStatusTip(tr("Perform astrometric data reduction: detect stars and solve plate"));

    actMOD_ = astrometryMenu_->addAction(tr("&Moving Object Detection..."), this, &MainWindow::onMovingObjectDetection);
    actMOD_->setStatusTip(tr("Automatically detect moving objects across images"));

    actTrackStack_ = astrometryMenu_->addAction(tr("Track && &Stack..."), QKeySequence("Ctrl+T"), this, &MainWindow::onTrackAndStack);
    actTrackStack_->setStatusTip(tr("Stack images with asteroid tracking to reveal faint objects"));

    // ── Images ────────────────────────────────────────────────────────────────
    imagesMenu_ = menuBar()->addMenu(tr("&Images"));

    imagesMenu_->addAction(tr("&Edit Image Parameters..."), this, &MainWindow::onEditImageParameters);
    imagesMenu_->addAction(tr("&Display Header..."),        this, &MainWindow::onDisplayHeader);
    imagesMenu_->addAction(tr("Background and &Range..."),  this, &MainWindow::onBackgroundAndRange);
    imagesMenu_->addAction(tr("&Re-Stack Images"),          this, &MainWindow::onReStackImages);

    imagesMenu_->addSeparator();
    actZoomIn_  = imagesMenu_->addAction(tr("Zoom &In"),  QKeySequence::ZoomIn,  this, &MainWindow::onZoomIn);
    actZoomOut_ = imagesMenu_->addAction(tr("Zoom &Out"), QKeySequence::ZoomOut, this, &MainWindow::onZoomOut);
    imagesMenu_->addAction(tr("&Fit Window Size"), QKeySequence("Ctrl+F"), this, &MainWindow::onFitWindowSize);

    imagesMenu_->addSeparator();
    imagesMenu_->addAction(tr("Select &Markings..."), this, &MainWindow::onSelectMarkings);
    imagesMenu_->addAction(tr("&Invert Display"),     QKeySequence("Ctrl+I"), this, &MainWindow::onInvertDisplay);
    imagesMenu_->addAction(tr("Flip &Horizontal"),    this, &MainWindow::onFlipHorizontal);
    imagesMenu_->addAction(tr("Flip &Vertical"),      this, &MainWindow::onFlipVertical);

    imagesMenu_->addSeparator();
    imagesMenu_->addAction(tr("&Magnifying Glass"), QKeySequence("Ctrl+G"), this, &MainWindow::onMagnifyingGlass);

    // ── Tools ─────────────────────────────────────────────────────────────────
    toolsMenu_ = menuBar()->addMenu(tr("&Tools"));

    actBlink_ = toolsMenu_->addAction(tr("&Blink Images"), QKeySequence("Ctrl+B"), this, &MainWindow::onBlinkImages);
    actBlink_->setStatusTip(tr("Start blinking through all loaded images to detect moving objects"));

    actStopBlink_ = toolsMenu_->addAction(tr("S&top Blinking"), QKeySequence("Ctrl+B"), this, &MainWindow::onStopBlinking);
    actStopBlink_->setStatusTip(tr("Stop the blink animation"));
    actStopBlink_->setEnabled(false);

    toolsMenu_->addSeparator();
    actKOO_ = toolsMenu_->addAction(tr("&Known Object Overlay"), QKeySequence("Ctrl+K"), this, &MainWindow::onKnownObjectOverlay);
    actKOO_->setStatusTip(tr("Mark known solar system objects on the images"));

    // ── Internet ──────────────────────────────────────────────────────────────
    internetMenu_ = menuBar()->addMenu(tr("Inter&net"));

    internetMenu_->addAction(tr("&Download MPCOrb"), this, &MainWindow::onDownloadMpcOrb)
        ->setStatusTip(tr("Download the complete MPCOrb asteroid database (~500 MB)"));
    internetMenu_->addAction(tr("&Update MPCOrb"), this, &MainWindow::onUpdateMpcOrb)
        ->setStatusTip(tr("Download the daily MPCOrb update"));
    internetMenu_->addSeparator();
    internetMenu_->addAction(tr("&AstroFind Web Page"), this, &MainWindow::onAstrometricaWebPage);

    // ── Windows ───────────────────────────────────────────────────────────────
    windowMenu_ = menuBar()->addMenu(tr("&Windows"));

    windowMenu_->addAction(tr("&Tile Windows"),    this, &MainWindow::onTileWindows);
    windowMenu_->addAction(tr("&Cascade Windows"), this, &MainWindow::onCascadeWindows);
    windowMenu_->addAction(tr("&Arrange all Windows"), this, &MainWindow::onArrangeWindows);
    windowMenu_->addSeparator();
    windowMenu_->addAction(tr("Close all &Images"),  this, &MainWindow::onCloseAllImages);
    windowMenu_->addAction(tr("Close &all Windows"), this, &MainWindow::onCloseAllWindows);

    // ── Help ──────────────────────────────────────────────────────────────────
    helpMenu_ = menuBar()->addMenu(tr("&Help"));

    helpMenu_->addAction(tr("&Help Contents"), QKeySequence::HelpContents, this, &MainWindow::onHelpContents);
    helpMenu_->addSeparator();
    helpMenu_->addAction(tr("&Registration..."), this, &MainWindow::onRegistration);
    helpMenu_->addSeparator();
    helpMenu_->addAction(tr("&About..."), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    auto* tb = addToolBar(tr("Main Toolbar"));
    tb->setObjectName("MainToolBar");
    tb->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tb->setIconSize(QSize(24, 24));

    auto addBtn = [&](const QString& text, const QString& tip, auto* slot) {
        auto* act = tb->addAction(text);
        act->setStatusTip(tip);
        return act;
    };

    tb->addAction(actLoadImages_);
    tb->addSeparator();
    tb->addAction(actDataReduction_);
    tb->addAction(actKOO_);
    tb->addAction(actTrackStack_);
    tb->addSeparator();
    tb->addAction(actBlink_);
    tb->addAction(actStopBlink_);
    tb->addSeparator();

    auto* actSettings = tb->addAction(tr("Settings"));
    actSettings->setStatusTip(tr("Edit program settings"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);
}

void MainWindow::setupStatusBar()
{
    sbStep_   = new QLabel(tr("Ready"), this);
    sbCoords_ = new QLabel(tr("RA -- Dec --"), this);
    sbPixel_  = new QLabel(tr("Value: --"), this);
    sbImages_ = new QLabel(tr("No images"), this);

    sbStep_->setMinimumWidth(200);
    sbCoords_->setMinimumWidth(220);
    sbPixel_->setMinimumWidth(120);

    statusBar()->addWidget(sbStep_);
    statusBar()->addPermanentWidget(sbPixel_);
    statusBar()->addPermanentWidget(sbCoords_);
    statusBar()->addPermanentWidget(sbImages_);
}

void MainWindow::setupDockWidgets()
{
    // ── Workflow panel (left) ─────────────────────────────────────────────────
    workflowPanel_ = new WorkflowPanel(this);
    auto* wfDock = new QDockWidget(tr("Workflow"), this);
    wfDock->setObjectName("WorkflowDock");
    wfDock->setWidget(workflowPanel_);
    wfDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    wfDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    addDockWidget(Qt::LeftDockWidgetArea, wfDock);

    // ── Log panel (bottom) ────────────────────────────────────────────────────
    logPanel_ = new LogPanel(this);
    auto* logDock = new QDockWidget(tr("Log"), this);
    logDock->setObjectName("LogDock");
    logDock->setWidget(logPanel_);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    logDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);
    logDock->setMaximumHeight(160);

    // ── Thumbnail bar (bottom, above log) ─────────────────────────────────────
    thumbnailBar_ = new ThumbnailBar(this);
    auto* thumbDock = new QDockWidget(tr("Images"), this);
    thumbDock->setObjectName("ThumbnailDock");
    thumbDock->setWidget(thumbnailBar_);
    thumbDock->setAllowedAreas(Qt::BottomDockWidgetArea);
    thumbDock->setFeatures(QDockWidget::DockWidgetMovable);
    thumbDock->setMaximumHeight(100);
    addDockWidget(Qt::BottomDockWidgetArea, thumbDock);

    connect(thumbnailBar_, &ThumbnailBar::imageActivated, this, [this](int idx) {
        const auto wins = allFitsWindows();
        if (idx < wins.size()) {
            for (auto* sw : mdiArea_->subWindowList()) {
                if (sw->widget() == wins[idx]) {
                    mdiArea_->setActiveSubWindow(sw);
                    break;
                }
            }
        }
    });

    // Make panels accessible from View menu
    windowMenu_->addSeparator();
    windowMenu_->addAction(wfDock->toggleViewAction());
    windowMenu_->addAction(thumbDock->toggleViewAction());
    windowMenu_->addAction(logDock->toggleViewAction());
}

void MainWindow::setupConnections()
{
    connect(blinkTimer_, &QTimer::timeout, this, &MainWindow::onBlinkTick);
    connect(mdiArea_, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);
    connect(session_.get(), &core::ImageSession::stepChanged, this, &MainWindow::onSessionStepChanged);
    connect(session_.get(), &core::ImageSession::stepChanged, workflowPanel_, &WorkflowPanel::setStep);
    connect(session_.get(), &core::ImageSession::imagesChanged, this, &MainWindow::onUpdateMenuState);
}

// ─── Settings ─────────────────────────────────────────────────────────────────

void MainWindow::saveSettings()
{
    settings_.setValue("window/geometry", saveGeometry());
    settings_.setValue("window/state",    saveState());
}

void MainWindow::loadSettings()
{
    if (settings_.contains("window/geometry"))
        restoreGeometry(settings_.value("window/geometry").toByteArray());
    else
        resize(1024, 720);

    if (settings_.contains("window/state"))
        restoreState(settings_.value("window/state").toByteArray());

    blinkIntervalMs_ = settings_.value("blink/intervalMs", 500).toInt();
}

// ─── Helper ───────────────────────────────────────────────────────────────────

FitsSubWindow* MainWindow::activeFitsWindow() const
{
    if (auto* sw = mdiArea_->activeSubWindow())
        return qobject_cast<FitsSubWindow*>(sw->widget());
    return nullptr;
}

QList<FitsSubWindow*> MainWindow::allFitsWindows() const
{
    QList<FitsSubWindow*> list;
    for (auto* sw : mdiArea_->subWindowList()) {
        if (auto* fw = qobject_cast<FitsSubWindow*>(sw->widget()))
            list.append(fw);
    }
    return list;
}

// ─── Close event ──────────────────────────────────────────────────────────────

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (blinkActive_) onStopBlinking();

    if (!session_->isEmpty()) {
        const auto btn = QMessageBox::question(this, tr("Exit AstroFind"),
            tr("There is an active session. Close anyway?"),
            QMessageBox::Yes | QMessageBox::No);
        if (btn != QMessageBox::Yes) { event->ignore(); return; }
    }
    saveSettings();
    event->accept();
}

// ─── File menu slots ──────────────────────────────────────────────────────────

void MainWindow::onLoadImages()
{
    const QString lastDir = settings_.value("paths/lastImageDir",
        QDir::homePath()).toString();

    const QStringList files = QFileDialog::getOpenFileNames(this,
        tr("Load FITS Images"), lastDir,
        tr("FITS & ZIP (*.fits *.fit *.fts *.FITS *.FIT *.zip *.ZIP);;FITS Images (*.fits *.fit *.fts);;ZIP archives (*.zip);;All files (*)"));

    if (files.isEmpty()) return;

    // Expand any ZIP archives → extract FITS to temp directory
    QStringList expandedFiles;
    for (const auto& path : files) {
        if (path.endsWith(".zip", Qt::CaseInsensitive))
            expandedFiles.append(expandZip(path));
        else
            expandedFiles.append(path);
    }

    settings_.setValue("paths/lastImageDir", QFileInfo(files.first()).absolutePath());

    if (!session_->isEmpty()) {
        const auto btn = QMessageBox::question(this, tr("Load Images"),
            tr("Replace the current session with new images?"),
            QMessageBox::Yes | QMessageBox::No);
        if (btn == QMessageBox::Yes) onResetFiles();
    }

    QProgressDialog progress(tr("Loading FITS images..."), tr("Cancel"), 0, expandedFiles.size(), this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(300);

    int loaded = 0;
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
        if (!session_->addImage(img)) {
            logPanel_->appendWarning(tr("Session full (max 4 images). Skipping %1.")
                .arg(img.fileName));
            break;
        }

        // Open MDI subwindow
        auto* sw = new FitsSubWindow(img, this);
        auto* mdiSw = mdiArea_->addSubWindow(sw);
        mdiSw->setWindowTitle(img.fileName);
        mdiSw->show();

        connect(sw, &FitsSubWindow::cursorMoved, this, [this](double ra, double dec, float val) {
            sbCoords_->setText(tr("RA %1  Dec %2")
                .arg(ra,  0, 'f', 5)
                .arg(dec, 0, 'f', 5));
            sbPixel_->setText(tr("Val %1").arg(val, 0, 'f', 1));
        });

        logPanel_->appendInfo(tr("Loaded: %1  [%2×%3]  JD=%.6f")
            .arg(img.fileName)
            .arg(img.width)
            .arg(img.height)
            .arg(img.jd));

        ++loaded;
    }
    progress.setValue(expandedFiles.size());

    if (loaded > 0) {
        session_->setStep(core::SessionStep::ImagesLoaded);
        mdiArea_->tileSubWindows();
        sbImages_->setText(tr("%1 image(s)").arg(session_->imageCount()));
        sbStep_->setText(tr("Step 1: Images loaded — Run Data Reduction"));

        // Populate thumbnail bar
        QVector<core::FitsImage> imgs;
        imgs.reserve(session_->imageCount());
        for (int i = 0; i < session_->imageCount(); ++i)
            imgs.append(session_->image(i));
        thumbnailBar_->setImages(imgs);
    }
}

void MainWindow::onSaveAllFits()  { /* Phase 2 */ statusBar()->showMessage(tr("Save all FITS — coming soon"), 3000); }
void MainWindow::onSaveAsFits()   { /* Phase 2 */ statusBar()->showMessage(tr("Save as FITS — coming soon"), 3000); }
void MainWindow::onLoadDarkFrame(){ /* Phase 2 */ statusBar()->showMessage(tr("Load dark frame — coming soon"), 3000); }
void MainWindow::onLoadFlatField(){ /* Phase 2 */ statusBar()->showMessage(tr("Load flat field — coming soon"), 3000); }
void MainWindow::onViewAdesReport(){ /* Phase 5 */ statusBar()->showMessage(tr("View ADES report — Phase 5"), 3000); }
void MainWindow::onViewPhotometryFile(){ /* Phase 4 */ statusBar()->showMessage(tr("View photometry — Phase 4"), 3000); }
void MainWindow::onViewLogFile()  { logPanel_->setVisible(true); }

void MainWindow::onResetFiles()
{
    if (blinkActive_) onStopBlinking();
    for (auto* sw : mdiArea_->subWindowList())
        sw->close();
    session_->clear();
    thumbnailBar_->clear();
    blinkMdiWin_ = nullptr;
    blinkWidget_ = nullptr;
    sbImages_->setText(tr("No images"));
    sbStep_->setText(tr("Ready"));
    logPanel_->appendInfo(tr("Session reset"));
}

void MainWindow::onReloadMpcOrb() { statusBar()->showMessage(tr("Reload MPCOrb — Phase 3"), 3000); }
void MainWindow::onSettings()     { statusBar()->showMessage(tr("Settings dialog — Phase 6"), 3000); }
void MainWindow::onExit()         { close(); }

// ─── Astrometry slots ─────────────────────────────────────────────────────────

void MainWindow::onDataReduction()
{
    if (session_->isEmpty()) return;

    if (astrometryClient_->isBusy()) {
        QMessageBox::information(this, tr("Data Reduction"),
            tr("Plate solving is already in progress. Please wait."));
        return;
    }

    // Ensure API key is configured
    QString apiKey = settings_.value("astrometry/apiKey").toString();
    if (apiKey.isEmpty()) {
        bool ok = false;
        apiKey = QInputDialog::getText(this,
            tr("Astrometry.net API Key"),
            tr("Enter your free API key from nova.astrometry.net:"),
            QLineEdit::Normal, QString(), &ok);
        if (!ok || apiKey.trimmed().isEmpty()) return;
        settings_.setValue("astrometry/apiKey", apiKey.trimmed());
    }
    astrometryClient_->setApiKey(apiKey.trimmed());

    // Connect client signals (disconnect first to avoid duplicates)
    disconnect(astrometryClient_, nullptr, this, nullptr);
    connect(astrometryClient_, &core::AstrometryClient::progress,
            this, [this](const QString& msg, int pct) {
        logPanel_->appendInfo(tr("[Astrometry] %1 (%2%)").arg(msg).arg(pct));
        statusBar()->showMessage(msg);
    });
    connect(astrometryClient_, &core::AstrometryClient::solved,
            this, &MainWindow::onAstrometrySolved);
    connect(astrometryClient_, &core::AstrometryClient::failed,
            this, &MainWindow::onAstrometryFailed);

    // Build reduction queue
    reductionQueue_.clear();
    for (int i = 0; i < session_->imageCount(); ++i)
        reductionQueue_.append(i);

    logPanel_->appendInfo(tr("Starting data reduction for %1 image(s)...")
                          .arg(session_->imageCount()));
    processNextReduction();
}

void MainWindow::processNextReduction()
{
    if (reductionQueue_.isEmpty()) {
        logPanel_->appendInfo(tr("Data reduction complete."));
        session_->setStep(core::SessionStep::DataReduced);
        onUpdateMenuState();
        return;
    }

    reducingIdx_ = reductionQueue_.takeFirst();
    core::FitsImage& img = session_->image(reducingIdx_);

    // ── Star detection (synchronous, fast) ────────────────────────────────
    logPanel_->appendInfo(tr("Detecting stars in %1...").arg(img.fileName));
    auto result = core::detectStars(img);
    if (result) {
        img.detectedStars = *result;
        logPanel_->appendInfo(tr("  Found %1 stars").arg(img.detectedStars.size()));
        updateImageOverlay(reducingIdx_);
    } else {
        logPanel_->appendWarning(tr("  Star detection failed: %1").arg(result.error()));
    }

    // ── Plate solving (async, astrometry.net) ─────────────────────────────
    logPanel_->appendInfo(tr("Submitting %1 to astrometry.net...").arg(img.fileName));
    astrometryClient_->solveFits(img.filePath, img.ra, img.dec);
}

void MainWindow::onAstrometrySolved(const core::PlateSolution& wcs, int /*jobId*/)
{
    if (reducingIdx_ < 0 || reducingIdx_ >= session_->imageCount()) return;

    core::FitsImage& img = session_->image(reducingIdx_);
    img.wcs = wcs;

    // Update pixel scale
    img.pixScaleX = std::abs(std::hypot(wcs.cd1_1, wcs.cd2_1)) * 3600.0;
    img.pixScaleY = std::abs(std::hypot(wcs.cd1_2, wcs.cd2_2)) * 3600.0;

    logPanel_->appendInfo(tr("  Solved! RA=%.4f Dec=%.4f scale=%.3f\"/px")
        .arg(wcs.crval1, 0, 'f', 4)
        .arg(wcs.crval2, 0, 'f', 4)
        .arg(img.pixScaleX, 0, 'f', 3));

    updateImageOverlay(reducingIdx_);
    processNextReduction();
}

void MainWindow::onAstrometryFailed(const QString& reason)
{
    logPanel_->appendWarning(tr("  Plate solving failed: %1").arg(reason));
    processNextReduction();   // continue with next image
}

void MainWindow::onMovingObjectDetection(){ statusBar()->showMessage(tr("Moving Object Detection — Phase 7"), 3000); }
void MainWindow::onTrackAndStack()        { statusBar()->showMessage(tr("Track & Stack — Phase 7"), 3000); }

// ─── Images slots ─────────────────────────────────────────────────────────────

void MainWindow::onEditImageParameters() { statusBar()->showMessage(tr("Edit image parameters — Phase 2"), 3000); }
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
        this, [this](float mn, float mx, bool all) {
            if (!all) return;
            for (auto* fsw : allFitsWindows()) {
                for (int i = 0; i < session_->imageCount(); ++i) {
                    if (session_->image(i).filePath == fsw->fitsImage().filePath) {
                        session_->image(i).displayMin = mn;
                        session_->image(i).displayMax = mx;
                        fsw->imageView()->setStretch(mn, mx);
                        break;
                    }
                }
            }
        });
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
}
void MainWindow::onReStackImages()       { statusBar()->showMessage(tr("Re-Stack — Phase 7"), 3000); }

void MainWindow::onZoomIn()
{
    if (auto* fw = activeFitsWindow()) fw->zoomIn();
}
void MainWindow::onZoomOut()
{
    if (auto* fw = activeFitsWindow()) fw->zoomOut();
}
void MainWindow::onFitWindowSize()
{
    if (auto* fw = activeFitsWindow()) fw->fitToWindow();
}
void MainWindow::onSelectMarkings()  { statusBar()->showMessage(tr("Select markings — Phase 3"), 3000); }
void MainWindow::onInvertDisplay()
{
    if (auto* fw = activeFitsWindow()) fw->toggleInvert();
}
void MainWindow::onFlipHorizontal()
{
    if (auto* fw = activeFitsWindow()) fw->flipHorizontal();
}
void MainWindow::onFlipVertical()
{
    if (auto* fw = activeFitsWindow()) fw->flipVertical();
}
void MainWindow::onMagnifyingGlass() { statusBar()->showMessage(tr("Magnifying glass — Phase 1"), 3000); }

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
            this, [this](QPointF /*pos*/, double /*ra*/, double /*dec*/,
                         float /*val*/, int imgIdx) {
                thumbnailBar_->setActiveIndex(imgIdx);
                logPanel_->appendInfo(tr("Object selected in image %1 — "
                    "measurement mode coming in Phase 4").arg(imgIdx + 1));
                session_->setStep(core::SessionStep::Measuring);
            });
    }

    // Load session images into blink widget
    QVector<core::FitsImage> imgs;
    imgs.reserve(session_->imageCount());
    for (int i = 0; i < session_->imageCount(); ++i)
        imgs.append(session_->image(i));
    blinkWidget_->setImages(imgs);
    blinkWidget_->setInterval(blinkIntervalMs_);

    // Show as maximized MDI child
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
    });
    connect(kooEngine_, &core::KooEngine::objectsReady,
            this, &MainWindow::onKooReady);
    connect(kooEngine_, &core::KooEngine::failed,
            this, [this](const QString& r) {
        logPanel_->appendWarning(tr("SkyBoT query failed: %1").arg(r));
    });

    // Use the first image's solved WCS for the field center
    const core::FitsImage& ref = session_->image(0);
    const double ra      = ref.wcs.solved ? ref.wcs.crval1 : ref.ra;
    const double dec     = ref.wcs.solved ? ref.wcs.crval2 : ref.dec;
    const double fovDeg  = ref.wcs.solved
        ? std::max(ref.pixScaleX, ref.pixScaleY) * std::max(ref.width, ref.height) / 3600.0 * 0.7
        : 0.5;
    const double jd      = ref.jd;

    logPanel_->appendInfo(tr("Querying VizieR (UCAC4) and SkyBoT at RA=%.4f Dec=%.4f r=%.2f°...")
        .arg(ra, 0, 'f', 4).arg(dec, 0, 'f', 4).arg(fovDeg, 0, 'f', 2));

    catalogClient_->queryRegion(ra, dec, fovDeg);
    if (jd > 2400000.0)
        kooEngine_->queryField(ra, dec, fovDeg, jd);
}

void MainWindow::onCatalogReady(const QVector<core::CatalogStar>& stars)
{
    logPanel_->appendInfo(tr("Catalog: %1 UCAC4 stars").arg(stars.size()));
    for (int i = 0; i < session_->imageCount(); ++i) {
        session_->image(i).catalogStars = stars;
        updateImageOverlay(i);
    }
    if (session_->step() < core::SessionStep::OverlayApplied)
        session_->setStep(core::SessionStep::OverlayApplied);
    onUpdateMenuState();
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
}

void MainWindow::updateImageOverlay(int idx)
{
    if (idx < 0 || idx >= session_->imageCount()) return;
    const core::FitsImage& img = session_->image(idx);

    // Find the FitsSubWindow displaying this image and refresh its overlay
    for (auto* sw : mdiArea_->subWindowList()) {
        auto* fw = qobject_cast<FitsSubWindow*>(sw->widget());
        if (fw && fw->fitsImage().filePath == img.filePath) {
            fw->updateImage(img);
            break;
        }
    }
}

// ─── Internet slots ───────────────────────────────────────────────────────────

void MainWindow::onDownloadMpcOrb()     { statusBar()->showMessage(tr("Download MPCOrb — Phase 3"), 3000); }
void MainWindow::onUpdateMpcOrb()       { statusBar()->showMessage(tr("Update MPCOrb — Phase 3"), 3000); }
void MainWindow::onAstrometricaWebPage(){ QDesktopServices::openUrl(QUrl("https://github.com")); }

// ─── Windows slots ────────────────────────────────────────────────────────────

void MainWindow::onTileWindows()    { mdiArea_->tileSubWindows(); }
void MainWindow::onCascadeWindows() { mdiArea_->cascadeSubWindows(); }
void MainWindow::onArrangeWindows() { mdiArea_->tileSubWindows(); }
void MainWindow::onCloseAllImages()
{
    for (auto* sw : mdiArea_->subWindowList()) sw->close();
}
void MainWindow::onCloseAllWindows() { mdiArea_->closeAllSubWindows(); }

// ─── Help slots ───────────────────────────────────────────────────────────────

void MainWindow::onHelpContents()  { statusBar()->showMessage(tr("Help — coming soon"), 3000); }
void MainWindow::onRegistration()  { statusBar()->showMessage(tr("Registration — N/A (MIT open source)"), 3000); }
void MainWindow::onAbout()
{
    AboutDialog dlg(this);
    dlg.exec();
}

// ─── Blink timer ──────────────────────────────────────────────────────────────

void MainWindow::onBlinkTick()
{
    const auto wins = allFitsWindows();
    if (wins.isEmpty()) { onStopBlinking(); return; }

    blinkCurrentIdx_ = (blinkCurrentIdx_ + 1) % wins.size();
    const auto& fw = wins[blinkCurrentIdx_];
    if (auto* sw = mdiArea_->activeSubWindow())
        sw->widget()->hide();

    // Bring the target window to front
    for (auto* msw : mdiArea_->subWindowList()) {
        if (msw->widget() == fw) {
            mdiArea_->setActiveSubWindow(msw);
            break;
        }
    }
}

// ─── Status / state update slots ─────────────────────────────────────────────

void MainWindow::onSessionStepChanged(core::SessionStep step)
{
    static const QMap<core::SessionStep, QString> stepLabels = {
        { core::SessionStep::Idle,           tr("Ready — load images to start") },
        { core::SessionStep::ImagesLoaded,   tr("Step 1 done — run Data Reduction (Ctrl+A)") },
        { core::SessionStep::DataReduced,    tr("Step 2 done — run Known Object Overlay (Ctrl+K)") },
        { core::SessionStep::OverlayApplied, tr("Step 3 done — start Blinking (Ctrl+B)") },
        { core::SessionStep::Blinking,       tr("Blinking — click on moving object to measure") },
        { core::SessionStep::Measuring,      tr("Measuring — click objects in all 4 images") },
        { core::SessionStep::ReportReady,    tr("Step 6 done — view and send ADES Report") },
    };
    sbStep_->setText(stepLabels.value(step, tr("Unknown step")));
}

void MainWindow::onSubWindowActivated(QMdiSubWindow* /*w*/)
{
    onUpdateMenuState();
}

void MainWindow::onUpdateMenuState()
{
    const bool hasImages  = !session_->isEmpty();
    const bool hasReduced = session_->step() >= core::SessionStep::DataReduced;
    const bool hasOverlay = session_->step() >= core::SessionStep::OverlayApplied;
    const bool multi      = session_->imageCount() >= 2;
    const bool hasActive  = activeFitsWindow() != nullptr;

    actResetFiles_->setEnabled(hasImages);
    actViewAdes_->setEnabled(hasReduced);
    actViewPhot_->setEnabled(hasReduced);
    actDataReduction_->setEnabled(hasImages);
    actMOD_->setEnabled(hasReduced);
    actTrackStack_->setEnabled(hasImages);
    actBlink_->setEnabled(multi && !blinkActive_);
    actKOO_->setEnabled(hasImages);
    actZoomIn_->setEnabled(hasActive);
    actZoomOut_->setEnabled(hasActive);
}

// ─── ZIP extraction ───────────────────────────────────────────────────────────

QStringList MainWindow::expandZip(const QString& zipPath)
{
    QStringList result;

    if (!tempDir_)
        tempDir_ = std::make_unique<QTemporaryDir>();

    if (!tempDir_->isValid()) {
        logPanel_->appendError(tr("Could not create temp directory for ZIP extraction"));
        return result;
    }

    // Extract only FITS files from the ZIP using system unzip
    QProcess proc;
    proc.start("unzip", {
        "-o", zipPath,
        "*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT",
        "-d", tempDir_->path()
    });

    if (!proc.waitForFinished(30000)) {
        logPanel_->appendError(tr("Timed out extracting ZIP: %1").arg(zipPath));
        return result;
    }

    const QDir dir(tempDir_->path());
    const QStringList fitsFiles = dir.entryList(
        {"*.fits", "*.fit", "*.fts", "*.FITS", "*.FIT"},
        QDir::Files, QDir::Name);

    for (const auto& f : fitsFiles)
        result.append(dir.filePath(f));

    if (result.isEmpty())
        logPanel_->appendWarning(tr("No FITS files found in ZIP: %1").arg(QFileInfo(zipPath).fileName()));
    else
        logPanel_->appendInfo(tr("Extracted %1 FITS file(s) from %2")
            .arg(result.size()).arg(QFileInfo(zipPath).fileName()));

    return result;
}
