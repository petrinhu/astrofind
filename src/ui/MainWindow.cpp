#include "MainWindow_p.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , blinkTimer_(new QTimer(this))
    , session_(std::make_unique<core::ImageSession>(this))
{
    setWindowTitle(QStringLiteral("AstroFind %1").arg(QApplication::applicationVersion()));
    setMinimumSize(900, 650);
    setAcceptDrops(true);

    // MDI area + info bar wrapped in a container widget
    mdiArea_ = new QMdiArea(this);
    mdiArea_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    infoBar_ = new SessionInfoBar(this);
    connect(infoBar_, &SessionInfoBar::openSettingsRequested,
            this, &MainWindow::onSettings);

    auto* centralContainer = new QWidget(this);
    auto* centralLayout    = new QVBoxLayout(centralContainer);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(infoBar_);
    centralLayout->addWidget(mdiArea_, 1);
    setCentralWidget(centralContainer);

    // ── Phase 2: network + astrometry clients ─────────────────────────────
    nam_              = new QNetworkAccessManager(this);
    astrometryClient_ = new core::AstrometryClient(nam_, this);
    astapSolver_      = new core::AstapSolver(this);
    catalogClient_    = new core::CatalogClient(nam_, this);
    kooEngine_        = new core::KooEngine(nam_, this);

    undoStack_ = new QUndoStack(this);

    setupMenus();
    setupToolBar();
    setupStatusBar();
    setupDockWidgets();
    setupConnections();

    loadSettings();  // also calls applySettingsToSubsystems()

    applyTheme();
    onUpdateMenuState();

    // React to OS dark/light changes while app is running (Qt 6.5+)
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged,
            this, [this](Qt::ColorScheme) {
        if (themeMode_ == Theme::Mode::Auto)
            applyTheme();
    });
#endif

    // Show setup wizard on startup if the user hasn't disabled it
    if (settings_.value(QStringLiteral("display/showWizardOnStartup"), true).toBool()) {
        QTimer::singleShot(200, this, &MainWindow::onSetupWizard);
    }

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

    recentMenu_ = fileMenu_->addMenu(tr("Open &Recent..."));
    updateRecentMenu();

    fileMenu_->addSeparator();
    actSaveProject_ = fileMenu_->addAction(tr("&Save Project"), QKeySequence("Ctrl+S"), this, &MainWindow::onSaveProject);
    actSaveProject_->setStatusTip(tr("Save the current session as a .gus project file"));
    actSaveProjectAs_ = fileMenu_->addAction(tr("Save Project &As..."), QKeySequence("Ctrl+Shift+S"), this, &MainWindow::onSaveProjectAs);
    actSaveProjectAs_->setStatusTip(tr("Save the current session to a new .gus project file"));
    actOpenProject_ = fileMenu_->addAction(tr("&Open Project..."), QKeySequence("Ctrl+O"), this, &MainWindow::onOpenProject);
    actOpenProject_->setStatusTip(tr("Open a previously saved .gus project file"));
    recentProjectsMenu_ = fileMenu_->addMenu(tr("Projetos &Recentes"));
    updateRecentProjectsMenu();
    actCloseProject_ = fileMenu_->addAction(tr("&Close Project"), this, &MainWindow::onCloseProject);
    actCloseProject_->setStatusTip(tr("Close the current project and reset the session"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Save &all FITS"), this, &MainWindow::onSaveAllFits)
        ->setStatusTip(tr("Save all open images as FITS files"));
    fileMenu_->addAction(tr("&Save as FITS..."), this, &MainWindow::onSaveAsFits)
        ->setStatusTip(tr("Save the active image as a FITS file"));

    auto* exportMenu = fileMenu_->addMenu(tr("&Export Image to..."));
    exportMenu->addAction(tr("JPEG / PNG / BMP…"), this, [this]() {
        onExportImage(activeFitsWindow());
    });
    exportMenu->addAction(tr("JPEG (quick)…"), this, [this]() {
        auto* sw = activeFitsWindow();
        if (!sw) return;
        const QString suggested = QFileInfo(sw->fitsImage().filePath).completeBaseName() + ".jpg";
        const QString path = QFileDialog::getSaveFileName(this,
            tr("Export as JPEG"),
            QDir(settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString())
                .filePath(suggested),
            tr("JPEG images (*.jpg *.jpeg)"));
        if (path.isEmpty()) return;
        const QString err = core::exportImage(sw->fitsImage(), path);
        if (err.isEmpty()) logPanel_->appendInfo(tr("Exported: %1").arg(path));
        else QMessageBox::warning(this, tr("Export Failed"), err);
    });
    exportMenu->addAction(tr("PNG…"), this, [this]() {
        auto* sw = activeFitsWindow();
        if (!sw) return;
        const QString suggested = QFileInfo(sw->fitsImage().filePath).completeBaseName() + ".png";
        const QString path = QFileDialog::getSaveFileName(this,
            tr("Export as PNG"),
            QDir(settings_.value(QStringLiteral("paths/lastImageDir"), QDir::homePath()).toString())
                .filePath(suggested),
            tr("PNG images (*.png)"));
        if (path.isEmpty()) return;
        const QString err = core::exportImage(sw->fitsImage(), path);
        if (err.isEmpty()) logPanel_->appendInfo(tr("Exported: %1").arg(path));
        else QMessageBox::warning(this, tr("Export Failed"), err);
    });

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Load &Dark Frame..."), this, &MainWindow::onLoadDarkFrame)
        ->setStatusTip(tr("Load a dark frame for calibration"));
    fileMenu_->addAction(tr("Load &Flat Field..."), this, &MainWindow::onLoadFlatField)
        ->setStatusTip(tr("Load a flat field for calibration"));
    fileMenu_->addAction(tr("Calibration &Wizard…"), this, &MainWindow::onCalibrationWizard);

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
    fileMenu_->addAction(tr("&Fechar Imagens"), QKeySequence::Close,
                         this, &MainWindow::onCloseAllImages)
        ->setStatusTip(tr("Fechar todas as imagens e reiniciar a sessão"));
    fileMenu_->addAction(tr("Reload MPC&Orb"), this, &MainWindow::onReloadMpcOrb)
        ->setStatusTip(tr("Reload the MPCOrb asteroid database from disk"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Se&ttings..."), QKeySequence("Ctrl+,"), this, &MainWindow::onSettings)
        ->setStatusTip(tr("Edit program settings"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("Exportar Configuração da Escola..."), this, &MainWindow::onExportSchoolConfig)
        ->setStatusTip(tr("Exporta todas as configurações para um arquivo .ini (para distribuir a alunos)"));
    fileMenu_->addAction(tr("Importar Configuração da Escola..."), this, &MainWindow::onImportSchoolConfig)
        ->setStatusTip(tr("Importa configurações de um arquivo .ini fornecido pelo professor"));

    fileMenu_->addSeparator();
    fileMenu_->addAction(tr("E&xit"), QKeySequence::Quit, this, &MainWindow::onExit);

    // ── Edit ──────────────────────────────────────────────────────────────────
    editMenu_ = menuBar()->addMenu(tr("&Edit"));

    actUndo_ = editMenu_->addAction(tr("&Undo"), QKeySequence::Undo, this, &MainWindow::onUndo);
    actRedo_ = editMenu_->addAction(tr("&Redo"), QKeySequence::Redo, this, &MainWindow::onRedo);
    editMenu_->addSeparator();
    actClearMarkings_ = editMenu_->addAction(tr("&Clear All Markings"), this, &MainWindow::onClearMarkings);
    actClearMarkings_->setStatusTip(tr("Remove all measurement markings from all images"));
    editMenu_->addSeparator();
    editMenu_->addAction(tr("&Copy Image"), QKeySequence::Copy, this, &MainWindow::onCopyImage)
        ->setStatusTip(tr("Copy the active image to the clipboard"));
    editMenu_->addSeparator();

    // ── Tool mode actions (mutually exclusive) ────────────────────────────────
    auto* toolGroup = new QActionGroup(this);
    toolGroup->setExclusive(true);

    actSelectTool_ = editMenu_->addAction(tr("&Select Tool"), QKeySequence("S"), this, &MainWindow::onSelectTool);
    actSelectTool_->setStatusTip(tr("Default selection/pan mode"));
    actSelectTool_->setCheckable(true);
    actSelectTool_->setChecked(true);
    toolGroup->addAction(actSelectTool_);

    actApertureTool_ = editMenu_->addAction(tr("&Aperture Tool"), QKeySequence("A"), this, &MainWindow::onApertureTool);
    actApertureTool_->setStatusTip(tr("Click on an object to measure its position and magnitude"));
    actApertureTool_->setCheckable(true);
    toolGroup->addAction(actApertureTool_);

    actAnnotateTool_ = editMenu_->addAction(tr("A&nnotate Tool"), QKeySequence("N"), this, &MainWindow::onAnnotateTool);
    actAnnotateTool_->setStatusTip(tr("Click to add a text annotation at any image position"));
    actAnnotateTool_->setCheckable(true);
    toolGroup->addAction(actAnnotateTool_);

    // ── Astrometry ────────────────────────────────────────────────────────────
    astrometryMenu_ = menuBar()->addMenu(tr("&Astrometry"));

    actDataReduction_ = astrometryMenu_->addAction(tr("&Data Reduction..."), QKeySequence("Ctrl+A"), this, &MainWindow::onDataReduction);
    actDataReduction_->setStatusTip(tr("Perform astrometric data reduction: detect stars and solve plate"));
    actStopReduction_ = astrometryMenu_->addAction(tr("&Stop Data Reduction"), QKeySequence("Ctrl+."), this, &MainWindow::onStopReduction);
    actStopReduction_->setStatusTip(tr("Abort the ongoing data reduction"));
    actStopReduction_->setEnabled(false);

    actMOD_ = astrometryMenu_->addAction(tr("&Moving Object Detection..."), QKeySequence("Ctrl+M"), this, &MainWindow::onMovingObjectDetection);
    actMOD_->setStatusTip(tr("Automatically detect moving objects across images"));

    actTrackStack_ = astrometryMenu_->addAction(tr("Stack &Images..."), QKeySequence("Ctrl+T"), this, &MainWindow::onTrackAndStack);
    actTrackStack_->setStatusTip(tr("Stack images with asteroid tracking to reveal faint objects"));

    astrometryMenu_->addSeparator();
    astrometryMenu_->addAction(tr("Import Detected Stars (DAOPHOT/SExtractor)…"),
                               this, &MainWindow::onImportDaophotTable);
    astrometryMenu_->addAction(tr("Import Reduction Table (IRAF/Astropy)…"),
                               this, &MainWindow::onImportReductionTable);

    // ── Images ────────────────────────────────────────────────────────────────
    imagesMenu_ = menuBar()->addMenu(tr("&Images"));

    imagesMenu_->addAction(tr("&Edit Image Parameters..."), this, &MainWindow::onEditImageParameters);
    imagesMenu_->addAction(tr("&Display Header..."),        this, &MainWindow::onDisplayHeader);
    imagesMenu_->addAction(tr("Background and &Range..."),  this, &MainWindow::onBackgroundAndRange);
    imagesMenu_->addAction(tr("&Re-Stack Images"),          this, &MainWindow::onReStackImages);

    imagesMenu_->addSeparator();
    actZoomIn_  = imagesMenu_->addAction(tr("Zoom &In"),  this, &MainWindow::onZoomIn);
    actZoomIn_->setShortcuts({QKeySequence::ZoomIn, QKeySequence(Qt::Key_Plus), QKeySequence(Qt::Key_Equal)});
    actZoomOut_ = imagesMenu_->addAction(tr("Zoom &Out"), this, &MainWindow::onZoomOut);
    actZoomOut_->setShortcuts({QKeySequence::ZoomOut, QKeySequence(Qt::Key_Minus)});
    imagesMenu_->addAction(tr("&Fit Window Size"), this, &MainWindow::onFitWindowSize)
        ->setShortcuts({QKeySequence("Ctrl+F"), QKeySequence(Qt::Key_0)});

    imagesMenu_->addSeparator();
    imagesMenu_->addAction(tr("Select &Markings..."), this, &MainWindow::onSelectMarkings);
    actInvertDisplay_ = new QAction(tr("&Invert Display"), this);
    actInvertDisplay_->setShortcut(QKeySequence("Ctrl+I"));
    actInvertDisplay_->setCheckable(true);
    connect(actInvertDisplay_, &QAction::triggered, this, &MainWindow::onInvertDisplay);
    imagesMenu_->addAction(actInvertDisplay_);
    imagesMenu_->addAction(tr("Flip &Horizontal"),    this, &MainWindow::onFlipHorizontal);
    imagesMenu_->addAction(tr("Flip &Vertical"),      this, &MainWindow::onFlipVertical);

    imagesMenu_->addSeparator();
    imagesMenu_->addAction(tr("&Magnifying Glass"), QKeySequence("Ctrl+G"), this, &MainWindow::onMagnifyingGlass);
    imagesMenu_->addAction(tr("Region &Statistics"), QKeySequence("Ctrl+Shift+R"), this, &MainWindow::onRegionStatsTool);

    // ── Tools ─────────────────────────────────────────────────────────────────
    toolsMenu_ = menuBar()->addMenu(tr("&Tools"));

    actBlink_ = toolsMenu_->addAction(tr("&Blink Images"), QKeySequence("Ctrl+B"), this, &MainWindow::onBlinkImages);
    actBlink_->setStatusTip(tr("Start blinking through all loaded images to detect moving objects"));

    actStopBlink_ = toolsMenu_->addAction(tr("S&top Blinking"), QKeySequence("Ctrl+F9"), this, &MainWindow::onStopBlinking);
    actStopBlink_->setStatusTip(tr("Stop the blink animation"));
    actStopBlink_->setEnabled(false);

    toolsMenu_->addSeparator();
    actKOO_ = toolsMenu_->addAction(tr("&Known Object Overlay"), QKeySequence("Ctrl+K"), this, &MainWindow::onKnownObjectOverlay);
    actKOO_->setStatusTip(tr("Mark known solar system objects on the images"));

    actEcliptic_ = toolsMenu_->addAction(tr("&Ecliptic / Galactic Overlay"), QKeySequence("Ctrl+E"),
        this, [this]() {
            for (auto* fsw : allFitsWindows())
                fsw->imageView()->setShowEcliptic(actEcliptic_->isChecked());
        });
    actEcliptic_->setCheckable(true);
    actEcliptic_->setStatusTip(tr("Show ecliptic and galactic plane overlays; warns when field is near the galactic plane (high dust extinction)"));

    toolsMenu_->addSeparator();
    toolsMenu_->addAction(tr("&Light Curve…"), QKeySequence("Ctrl+L"), this, &MainWindow::onLightCurve)
        ->setStatusTip(tr("Show magnitude vs. time plot for all current observations"));

    toolsMenu_->addAction(tr("&Growth Curve…"), QKeySequence("Ctrl+Shift+G"), this, &MainWindow::onGrowthCurve)
        ->setStatusTip(tr("Show aperture growth curve to select optimal photometry aperture"));

    toolsMenu_->addAction(tr("C&ompare Sessions…"), QKeySequence("Ctrl+Shift+C"), this, &MainWindow::onCompareSessions)
        ->setStatusTip(tr("Subtract a reference FITS image to detect new or moving objects"));

    // ── Internet ──────────────────────────────────────────────────────────────
    internetMenu_ = menuBar()->addMenu(tr("Inter&net"));

    internetMenu_->addAction(tr("&Download MPCOrb"), this, &MainWindow::onDownloadMpcOrb)
        ->setStatusTip(tr("Download the complete MPCOrb asteroid database (~500 MB)"));
    internetMenu_->addAction(tr("&Update MPCOrb"), this, &MainWindow::onUpdateMpcOrb)
        ->setStatusTip(tr("Download the daily MPCOrb update"));
    internetMenu_->addSeparator();
    internetMenu_->addAction(tr("Query &Horizons…"), this, &MainWindow::onQueryHorizons)
        ->setStatusTip(tr("Look up a solar system object on JPL Horizons and add it to the overlay"));
    internetMenu_->addSeparator();
    internetMenu_->addAction(tr("&Minor Planet Center…"), this, &MainWindow::onAstrometricaWebPage);

    // ── Windows ───────────────────────────────────────────────────────────────
    windowMenu_ = menuBar()->addMenu(tr("&Windows"));

    windowMenu_->addAction(tr("&Tile Windows"),    this, &MainWindow::onTileWindows);
    windowMenu_->addAction(tr("&Cascade Windows"), this, &MainWindow::onCascadeWindows);
    windowMenu_->addAction(tr("&Arrange all Windows"), this, &MainWindow::onArrangeWindows);
    windowMenu_->addSeparator();
    windowMenu_->addAction(tr("Close all &Images"),  this, &MainWindow::onCloseAllImages);
    windowMenu_->addAction(tr("Close &all Windows"), this, &MainWindow::onCloseAllWindows);
    windowMenu_->addSeparator();
    windowMenu_->addAction(tr("Toggle &Day/Night Mode"), QKeySequence("Ctrl+Shift+T"),
                           this, &MainWindow::onToggleTheme);

    // ── Help ──────────────────────────────────────────────────────────────────
    helpMenu_ = menuBar()->addMenu(tr("&Help"));

    helpMenu_->addAction(tr("&Help Contents"), QKeySequence::HelpContents, this, &MainWindow::onHelpContents);
    helpMenu_->addAction(tr("&Setup Wizard…"), this, &MainWindow::onSetupWizard);
    helpMenu_->addSeparator();
    helpMenu_->addAction(tr("&Registration..."), this, &MainWindow::onRegistration);
    helpMenu_->addSeparator();
    helpMenu_->addAction(tr("&About..."), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar()
{
    const QSize iconSz(22, 22);

    // ─────────────────────────────────────────────────────────────────────────
    // Standard Toolbar
    // ─────────────────────────────────────────────────────────────────────────
    auto* stdTb = addToolBar(tr("Standard"));
    stdTb->setObjectName("StandardToolBar");
    stdTb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    stdTb->setIconSize(iconSz);
    stdTb->setFixedHeight(36);

    // Settings
    auto* actSettings = stdTb->addAction(AppIcons::settings(), tr("Settings"));
    actSettings->setToolTip(tr("Settings (Ctrl+,)"));
    connect(actSettings, &QAction::triggered, this, &MainWindow::onSettings);

    // Day / Night theme toggle
    actThemeToggle_ = new QAction(this);
    actThemeToggle_->setToolTip(tr("Toggle Day/Night mode (Ctrl+Shift+T)"));
    actThemeToggle_->setShortcut(QKeySequence("Ctrl+Shift+T"));
    connect(actThemeToggle_, &QAction::triggered, this, &MainWindow::onToggleTheme);
    stdTb->addAction(actThemeToggle_);

    stdTb->addSeparator();

    // Load Images / Dark / Flat
    actLoadImages_->setIcon(AppIcons::loadImages());
    actLoadImages_->setToolTip(tr("Load Images (Ctrl+L)"));
    stdTb->addAction(actLoadImages_);

    auto* actLoadDark = new QAction(AppIcons::loadDark(), tr("Load Dark Frame"), this);
    actLoadDark->setToolTip(tr("Load dark frame for calibration"));
    connect(actLoadDark, &QAction::triggered, this, &MainWindow::onLoadDarkFrame);
    stdTb->addAction(actLoadDark);

    auto* actLoadFlat = new QAction(AppIcons::loadFlat(), tr("Load Flat Field"), this);
    actLoadFlat->setToolTip(tr("Load flat field for calibration"));
    connect(actLoadFlat, &QAction::triggered, this, &MainWindow::onLoadFlatField);
    stdTb->addAction(actLoadFlat);

    stdTb->addSeparator();

    // Data Reduction / MOD / Stack Images
    actDataReduction_->setIcon(AppIcons::dataReduction());
    actDataReduction_->setToolTip(tr("Data Reduction (Ctrl+A)"));
    stdTb->addAction(actDataReduction_);

    actMOD_->setIcon(AppIcons::movingObjectDetect());
    actMOD_->setToolTip(tr("Moving Object Detection (Ctrl+M)"));
    stdTb->addAction(actMOD_);

    actTrackStack_->setIcon(AppIcons::stackImages());
    actTrackStack_->setToolTip(tr("Stack Images (Ctrl+T)"));
    stdTb->addAction(actTrackStack_);

    // Measure tool — toggle measure-click mode
    actMeasure_ = new QAction(AppIcons::measure(), tr("Measure Object"), this);
    actMeasure_->setToolTip(tr("Click on a moving object to measure its position and magnitude (M)"));
    actMeasure_->setShortcut(QKeySequence("M"));
    actMeasure_->setCheckable(true);
    connect(actMeasure_, &QAction::toggled, this, [this](bool on) {
        if (on) onApertureTool();
        else    onSelectTool();
    });
    stdTb->addAction(actMeasure_);
    // Highlight the button background when measure mode is active
    if (auto* btn = qobject_cast<QToolButton*>(stdTb->widgetForAction(actMeasure_))) {
        btn->setStyleSheet(
            "QToolButton:checked {"
            "  background: rgba(255,140,20,0.22);"
            "  border: 1px solid rgba(255,140,20,0.65);"
            "  border-radius: 3px;"
            "}");
    }

    stdTb->addSeparator();

    // View Report / Close Images / Close All
    actViewAdes_->setIcon(AppIcons::viewReport());
    actViewAdes_->setToolTip(tr("View ADES Report"));
    stdTb->addAction(actViewAdes_);

    auto* actCloseImages = new QAction(AppIcons::closeImages(), tr("Close all Images"), this);
    actCloseImages->setToolTip(tr("Close all image windows"));
    connect(actCloseImages, &QAction::triggered, this, &MainWindow::onCloseAllImages);
    stdTb->addAction(actCloseImages);

    auto* actCloseAll = new QAction(AppIcons::closeAll(), tr("Close all Windows"), this);
    actCloseAll->setToolTip(tr("Close all windows"));
    connect(actCloseAll, &QAction::triggered, this, &MainWindow::onCloseAllWindows);
    stdTb->addAction(actCloseAll);

    // ─────────────────────────────────────────────────────────────────────────
    // Display Toolbar
    // ─────────────────────────────────────────────────────────────────────────
    auto* dispTb = addToolBar(tr("Display"));
    dispTb->setObjectName("DisplayToolBar");
    dispTb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    dispTb->setIconSize(iconSz);
    dispTb->setFixedHeight(36);

    actBgRange_ = new QAction(AppIcons::backgroundRange(), tr("Background && Range"), this);
    actBgRange_->setToolTip(tr("Adjust background and display range"));
    connect(actBgRange_, &QAction::triggered, this, &MainWindow::onBackgroundAndRange);
    dispTb->addAction(actBgRange_);

    auto* actSelMarkings = new QAction(AppIcons::selectMarkings(), tr("Select Markings"), this);
    actSelMarkings->setToolTip(tr("Select markings"));
    connect(actSelMarkings, &QAction::triggered, this, &MainWindow::onSelectMarkings);
    dispTb->addAction(actSelMarkings);

    actInvertDisplay_->setIcon(AppIcons::invertDisplay());
    actInvertDisplay_->setToolTip(tr("Invert display (Ctrl+I)"));
    dispTb->addAction(actInvertDisplay_);

    actKOO_->setIcon(AppIcons::knownObjectOverlay());
    actKOO_->setToolTip(tr("Known Object Overlay (Ctrl+K)"));
    dispTb->addAction(actKOO_);

    dispTb->addSeparator();

    actZoomIn_->setIcon(AppIcons::zoomIn());
    actZoomIn_->setToolTip(tr("Zoom In"));
    dispTb->addAction(actZoomIn_);

    actZoomOut_->setIcon(AppIcons::zoomOut());
    actZoomOut_->setToolTip(tr("Zoom Out"));
    dispTb->addAction(actZoomOut_);

    auto* actFitWin = new QAction(AppIcons::zoomFit(), tr("Fit Window Size"), this);
    actFitWin->setToolTip(tr("Fit image to window (Ctrl+F)"));
    connect(actFitWin, &QAction::triggered, this, &MainWindow::onFitWindowSize);
    dispTb->addAction(actFitWin);

    // ─────────────────────────────────────────────────────────────────────────
    // Blink Toolbar
    // ─────────────────────────────────────────────────────────────────────────
    auto* blinkTb = addToolBar(tr("Blink"));
    blinkTb->setObjectName("BlinkToolBar");
    blinkTb->setToolButtonStyle(Qt::ToolButtonIconOnly);
    blinkTb->setIconSize(iconSz);
    blinkTb->setFixedHeight(36);

    actBlink_->setIcon(AppIcons::blinkImages());
    actBlink_->setToolTip(tr("Blink Images (Ctrl+B)"));
    blinkTb->addAction(actBlink_);

    // Blink delay spinbox
    blinkDelayBox_ = new QSpinBox(blinkTb);
    blinkDelayBox_->setRange(1, 9);
    blinkDelayBox_->setValue(blinkIntervalMs_ / 100);
    blinkDelayBox_->setSuffix(tr("×0.1s"));
    blinkDelayBox_->setToolTip(tr("Blink interval (tenths of a second)"));
    blinkDelayBox_->setFixedSize(70, 28);
    connect(blinkDelayBox_, &QSpinBox::valueChanged, this, [this](int v) {
        blinkIntervalMs_ = v * 100;
        if (blinkWidget_)
            blinkWidget_->setInterval(blinkIntervalMs_);
    });
    blinkTb->addWidget(blinkDelayBox_);

    actStopBlink_->setIcon(AppIcons::blinkStop());
    actStopBlink_->setToolTip(tr("Stop Blinking (Ctrl+F9)"));
    blinkTb->addAction(actStopBlink_);

    auto* actBlinkPrev = new QAction(AppIcons::blinkPrev(), tr("Step Back"), this);
    actBlinkPrev->setToolTip(tr("Step to previous image (Shift+Ctrl+F10)"));
    actBlinkPrev->setShortcut(QKeySequence("Shift+Ctrl+F10"));
    connect(actBlinkPrev, &QAction::triggered, this, &MainWindow::onBlinkPrev);
    blinkTb->addAction(actBlinkPrev);

    auto* actBlinkStep = new QAction(AppIcons::blinkStep(), tr("Step Forward"), this);
    actBlinkStep->setToolTip(tr("Step to next image (Ctrl+F10)"));
    actBlinkStep->setShortcut(QKeySequence("Ctrl+F10"));
    connect(actBlinkStep, &QAction::triggered, this, &MainWindow::onBlinkNext);
    blinkTb->addAction(actBlinkStep);

    auto* actBlinkPlay = new QAction(AppIcons::blinkPlay(), tr("Play"), this);
    actBlinkPlay->setToolTip(tr("Start blink playback (Ctrl+F11)"));
    actBlinkPlay->setShortcut(QKeySequence("Ctrl+F11"));
    connect(actBlinkPlay, &QAction::triggered, this, &MainWindow::onBlinkImages);
    blinkTb->addAction(actBlinkPlay);

    // Toolbar visibility toggles in Windows menu
    stdTb->toggleViewAction()->setText(tr("View &Standard Toolbar"));
    dispTb->toggleViewAction()->setText(tr("View &Display Toolbar"));
    blinkTb->toggleViewAction()->setText(tr("View &Blink Toolbar"));
    windowMenu_->addSeparator();
    windowMenu_->addAction(stdTb->toggleViewAction());
    windowMenu_->addAction(dispTb->toggleViewAction());
    windowMenu_->addAction(blinkTb->toggleViewAction());
}

void MainWindow::setupStatusBar()
{
    sbStep_        = new QLabel(tr("Ready"), this);
    sbCoords_      = new QLabel(tr("RA -- Dec --"), this);
    sbPixel_       = new QLabel(tr("Value: --"), this);
    sbImages_      = new QLabel(tr("No images"), this);
    sbObservatory_ = new QLabel(this);

    sbStep_->setMinimumWidth(200);
    sbCoords_->setMinimumWidth(220);
    sbPixel_->setMinimumWidth(120);
    sbObservatory_->setStyleSheet(QStringLiteral("color:#8899aa;font-size:10px;"));
    sbObservatory_->setVisible(false);

    statusBar()->addWidget(sbStep_);
    statusBar()->addPermanentWidget(sbObservatory_);
    statusBar()->addPermanentWidget(sbPixel_);
    statusBar()->addPermanentWidget(sbCoords_);
    statusBar()->addPermanentWidget(sbImages_);
}

#include "DockTitleBar.h"

void MainWindow::setupDockWidgets()
{
    // ── Workflow panel (left) ─────────────────────────────────────────────────
    workflowPanel_ = new WorkflowPanel(this);
    auto* wfDock = new QDockWidget(tr("Workflow"), this);
    wfDock->setObjectName("WorkflowDock");
    wfDock->setTitleBarWidget(makeDockTitleBar(tr("Workflow"), wfDock));
    wfDock->setWidget(workflowPanel_);
    wfDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    wfDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, wfDock);

    // ── Calibration panel (left, below workflow) ──────────────────────────────
    calibPanel_ = new CalibrationPanel(this);
    auto* calibDock = new QDockWidget(tr("Calibration"), this);
    calibDock->setObjectName("CalibrationDock");
    calibDock->setTitleBarWidget(makeDockTitleBar(tr("Calibration"), calibDock));
    calibDock->setWidget(calibPanel_);
    calibDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    calibDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::LeftDockWidgetArea, calibDock);
    calibDock->hide();   // shown on demand

    connect(calibPanel_, &CalibrationPanel::loadDarkRequested,  this, &MainWindow::onLoadDarkFrame);
    connect(calibPanel_, &CalibrationPanel::loadFlatRequested,  this, &MainWindow::onLoadFlatField);
    connect(calibPanel_, &CalibrationPanel::buildMasterDarkRequested, this, &MainWindow::onBuildMasterDark);
    connect(calibPanel_, &CalibrationPanel::buildMasterFlatRequested, this, &MainWindow::onBuildMasterFlat);
    connect(calibPanel_, &CalibrationPanel::clearDarkRequested, this, [this]() {
        darkFrame_ = core::FitsImage{};
        calibPanel_->clearDark();
        logPanel_->appendInfo(tr("Dark frame cleared."));
    });
    connect(calibPanel_, &CalibrationPanel::clearFlatRequested, this, [this]() {
        flatFrame_ = core::FitsImage{};
        calibPanel_->clearFlat();
        logPanel_->appendInfo(tr("Flat field cleared."));
    });
    connect(calibPanel_, &CalibrationPanel::applyAllRequested, this, [this]() {
        if (!darkFrame_.isValid() && !flatFrame_.isValid()) {
            statusBar()->showMessage(tr("No calibration frames loaded"), 3000);
            return;
        }
        for (int i = 0; i < session_->imageCount(); ++i) {
            core::FitsImage& img = session_->image(i);
            if (darkFrame_.isValid() && darkFrame_.width == img.width && darkFrame_.height == img.height)
                core::applyDark(img, darkFrame_);
            if (flatFrame_.isValid() && flatFrame_.width == img.width && flatFrame_.height == img.height)
                core::applyFlat(img, flatFrame_);
            updateImageOverlay(i);
        }
        logPanel_->appendInfo(tr("Calibration applied to all %1 images.").arg(session_->imageCount()));
    });

    // ── Log panel (bottom) ────────────────────────────────────────────────────
    logPanel_ = new LogPanel(this);
    auto* logDock = new QDockWidget(tr("Log"), this);
    logDock->setObjectName("LogDock");
    logDock->setTitleBarWidget(makeDockTitleBar(tr("Log"), logDock));
    logDock->setWidget(logPanel_);
    logDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    logDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, logDock);
    logPanel_->setMinimumHeight(115);
    logPanel_->setMaximumHeight(300);
    logDock->setMinimumHeight(148);     // 23 (title bar) + 115 (content) + 10 (top margin)

    // ── Thumbnail bar (bottom, above log) ─────────────────────────────────────
    thumbnailBar_ = new ThumbnailBar(this);
    auto* thumbDock = new QDockWidget(tr("Images"), this);
    thumbDock->setObjectName("ThumbnailDock");
    thumbDock->setTitleBarWidget(makeDockTitleBar(tr("Images"), thumbDock));
    thumbDock->setWidget(thumbnailBar_);
    thumbDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    thumbDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    // ThumbnailBar has setFixedHeight(86) — no maxHeight needed on the dock itself
    // (setting it on the dock includes the title bar and causes height shifts on theme toggle)
    addDockWidget(Qt::BottomDockWidgetArea, thumbDock);
    thumbDock->setMinimumHeight(114);   // 28 (title bar) + 86 (ThumbnailBar fixed)

    connect(thumbnailBar_, &ThumbnailBar::imageActivated, this, [this](int idx) {
        thumbnailBar_->setActiveIndex(idx);
        const auto wins = allFitsWindows();
        if (idx < wins.size()) {
            for (auto* sw : mdiArea_->subWindowList()) {
                if (sw->widget() == wins[idx]) {
                    sw->raise();
                    mdiArea_->setActiveSubWindow(sw);
                    wins[idx]->imageView()->setFocus();
                    break;
                }
            }
        }
    });

    // ── Measurement table (bottom-right, initially hidden) ────────────────────
    measureTable_ = new MeasurementTable(this);
    auto* measDock = new QDockWidget(tr("Observations"), this);
    measDock->setObjectName("MeasurementDock");
    measDock->setTitleBarWidget(makeDockTitleBar(tr("Observations"), measDock));
    measDock->setWidget(measureTable_);
    measDock->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::RightDockWidgetArea);
    measDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::BottomDockWidgetArea, measDock);
    splitDockWidget(logDock, measDock, Qt::Horizontal);
    measureTable_->setMinimumHeight(86); // consistent minimum with other bottom docks
    measDock->hide();   // hidden until first measurement

    // ── Image Catalog (right side, initially hidden) ──────────────────────────
    imageCatalogTable_ = new ImageCatalogTable(this);
    auto* catDock = new QDockWidget(tr("Image Catalog"), this);
    catDock->setObjectName("ImageCatalogDock");
    catDock->setTitleBarWidget(makeDockTitleBar(tr("Image Catalog"), catDock));
    catDock->setWidget(imageCatalogTable_);
    catDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    catDock->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetClosable);
    addDockWidget(Qt::RightDockWidgetArea, catDock);
    catDock->hide();   // shown automatically after Data Reduction

    connect(imageCatalogTable_, &ImageCatalogTable::centreOnObject,
            this, [this](double px, double py) {
        FitsImageView* v = nullptr;
        if (auto* fw = activeFitsWindow()) v = fw->imageView();
        else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
        if (v) v->centerOnPixel(QPointF(px, py));
    });
    connect(imageCatalogTable_, &ImageCatalogTable::objectSelected,
            this, [this](double px, double py) {
        for (auto* sw : mdiArea_->subWindowList()) {
            if (auto* fw = qobject_cast<FitsSubWindow*>(sw->widget()))
                fw->imageView()->setHighlight(QPointF(px, py));
        }
        if (blinkActive_ && blinkWidget_)
            blinkWidget_->imageView()->setHighlight(QPointF(px, py));
    });
    connect(imageCatalogTable_, &ImageCatalogTable::objectDeselected,
            this, [this]() { clearAllHighlights(); });

    // Rename toggle actions for clarity, then expose in Windows menu
    wfDock->toggleViewAction()->setText(tr("View &Workflow Panel"));
    thumbDock->toggleViewAction()->setText(tr("View &Images Bar"));
    logDock->toggleViewAction()->setText(tr("View &Log Panel"));
    measDock->toggleViewAction()->setText(tr("View &Observations"));
    catDock->toggleViewAction()->setText(tr("View Image &Catalog"));
    calibDock->toggleViewAction()->setText(tr("View &Calibration Panel"));
    windowMenu_->addSeparator();
    windowMenu_->addAction(wfDock->toggleViewAction());
    windowMenu_->addAction(calibDock->toggleViewAction());
    windowMenu_->addAction(thumbDock->toggleViewAction());
    windowMenu_->addAction(logDock->toggleViewAction());
    windowMenu_->addAction(measDock->toggleViewAction());
    windowMenu_->addAction(catDock->toggleViewAction());
}

void MainWindow::setupConnections()
{
    connect(blinkTimer_, &QTimer::timeout, this, &MainWindow::onBlinkTick);
    connect(mdiArea_, &QMdiArea::subWindowActivated, this, &MainWindow::onSubWindowActivated);

    // Auto-refresh light curve dialog whenever observations change
    connect(measureTable_, &MeasurementTable::observationsChanged, this, [this] {
        if (lightCurveDlg_)
            lightCurveDlg_->setObservations(measureTable_->observations());
    });
    connect(session_.get(), &core::ImageSession::stepChanged, this, &MainWindow::onSessionStepChanged);

    // Undo/Redo button state
    connect(undoStack_, &QUndoStack::canUndoChanged, actUndo_, &QAction::setEnabled);
    connect(undoStack_, &QUndoStack::canRedoChanged, actRedo_, &QAction::setEnabled);
    connect(session_.get(), &core::ImageSession::stepChanged, workflowPanel_, &WorkflowPanel::setStep);
    connect(session_.get(), &core::ImageSession::imagesChanged, this, &MainWindow::onUpdateMenuState);

    // WorkflowPanel "▶ Do it" buttons → main actions
    connect(workflowPanel_, &WorkflowPanel::stepActionRequested, this,
            [this](core::SessionStep step) {
                switch (step) {
                case core::SessionStep::ImagesLoaded:   onLoadImages();         break;
                case core::SessionStep::DataReduced:
                    // Button doubles as Stop when astrometry is running
                    if (astrometryClient_->isBusy()) {
                        astrometryClient_->cancel();
                        workflowPanel_->setStepStatus(1, WorkflowPanel::Status::Idle);
                    } else {
                        onDataReduction();
                    }
                    break;
                case core::SessionStep::OverlayApplied: onKnownObjectOverlay(); break;
                case core::SessionStep::Blinking:       onBlinkImages();        break;
                case core::SessionStep::Measuring:      onApertureTool();       break;
                case core::SessionStep::ReportReady:    onViewAdesReport();     break;
                default: break;
                }
            });
    connect(workflowPanel_, &WorkflowPanel::autoWorkflowChanged,
            this, [this](bool on) {
                settings_.setValue(QStringLiteral("ui/autoWorkflow"), on);
            });

    // KooEngine offline scan progress → status bar
    connect(kooEngine_, &core::KooEngine::scanProgress, this,
            [this](int done, int total) {
                if (total > 0)
                    statusBar()->showMessage(
                        tr("Scanning MPCORB... %1 / %2 k").arg(done/1000).arg(total/1000));
            });
}

// ─── Settings ─────────────────────────────────────────────────────────────────

// Bump this constant whenever dock object names or the toolbar layout change.
// Any state saved with a different version will be silently discarded.

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

void MainWindow::showEvent(QShowEvent* event)
{
    QMainWindow::showEvent(event);
    if (!stateRestored_) {
        stateRestored_ = true;
        // Defer to the next event-loop iteration so Wayland's initial configure
        // has already been processed — restoring state before that causes a crash
        // in QDockAreaLayoutInfo::next() on Qt/Wayland.
        QTimer::singleShot(0, this, [this]() {
            bool stateLoaded = false;
            if (settings_.contains("window/state")) {
                stateLoaded = restoreState(settings_.value("window/state").toByteArray(),
                                           kWindowStateVersion);
                if (!stateLoaded)
                    settings_.remove("window/state");
            }
            if (!stateLoaded) {
                // Defer fresh-layout dock sizing until resizeEvent() fires,
                // which guarantees the window has its real compositor-committed
                // size (important on Wayland where singleShot(N ms) is fragile).
                initialDocksResized_ = false;
            }
        });
    }
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    if (!initialDocksResized_ && stateRestored_ && isVisible()) {
        initialDocksResized_ = true;
        // The window now has its real compositor-committed geometry.
        // Use singleShot(0) to run after the current layout pass completes.
        QTimer::singleShot(0, this, [this]() {
            auto* logDock   = findChild<QDockWidget*>(QStringLiteral("LogDock"));
            auto* thumbDock = findChild<QDockWidget*>(QStringLiteral("ThumbnailDock"));
            QList<QDockWidget*> btm;
            QList<int>          hts;
            // Heights = title bar (28) + content minimum
            if (logDock)   { btm << logDock;   hts << 148; }  // 23 + 115 + 10
            if (thumbDock) { btm << thumbDock; hts << 114; }  // 28 + 86
            if (!btm.isEmpty())
                resizeDocks(btm, hts, Qt::Vertical);
        });
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (blinkActive_) onStopBlinking();

    if (!maybeSaveProject()) { event->ignore(); return; }

    saveSettings();
    event->accept();
}

// ─── Drag & drop ─────────────────────────────────────────────────────────────

void MainWindow::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasUrls()) {
        const auto urls = event->mimeData()->urls();
        for (const auto& url : urls) {
            if (url.isLocalFile()) {
                const QString f = url.toLocalFile().toLower();
                if (f.endsWith(".fits") || f.endsWith(".fit") || f.endsWith(".fts")
                        || f.endsWith(".zip") || isLibArchiveFormat(f)) {
                    event->acceptProposedAction();
                    return;
                }
            }
        }
    }
}

void MainWindow::dropEvent(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls()) return;

    QStringList fitsPaths;
    for (const auto& url : event->mimeData()->urls()) {
        if (url.isLocalFile()) {
            const QString f = url.toLocalFile();
            const QString fl = f.toLower();
            if (fl.endsWith(".fits") || fl.endsWith(".fit") || fl.endsWith(".fts")
                    || fl.endsWith(".zip") || isLibArchiveFormat(f))
                fitsPaths << f;
        }
    }
    if (fitsPaths.isEmpty()) return;

    // If a single FITS is dropped and images are already loaded → ask intent
    if (fitsPaths.size() == 1 && !session_->isEmpty()) {
        QMenu menu(this);
        menu.addAction(tr("Open as Science Image"),   [&]{ /* fall through to load */ });
        auto* darkAct = menu.addAction(tr("Use as Dark Frame"));
        auto* flatAct = menu.addAction(tr("Use as Flat Field"));
        menu.addSeparator();
        menu.addAction(tr("Cancel"));

        QAction* chosen = menu.exec(QCursor::pos());
        if (chosen == darkAct) {
            auto result = core::loadFits(fitsPaths.first());
            if (result) {
                darkFrame_ = std::move(*result);
                calibPanel_->setDarkStatus(darkFrame_.fileName, darkFrame_.width, darkFrame_.height);
                if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
                logPanel_->appendInfo(tr("Dark frame loaded (drag & drop): %1").arg(darkFrame_.fileName));
            }
            event->acceptProposedAction();
            return;
        }
        if (chosen == flatAct) {
            auto result = core::loadFits(fitsPaths.first());
            if (result) {
                flatFrame_ = std::move(*result);
                calibPanel_->setFlatStatus(flatFrame_.fileName, flatFrame_.width, flatFrame_.height);
                if (auto* dock = qobject_cast<QDockWidget*>(calibPanel_->parent())) dock->show();
                logPanel_->appendInfo(tr("Flat field loaded (drag & drop): %1").arg(flatFrame_.fileName));
            }
            event->acceptProposedAction();
            return;
        }
        if (!chosen || chosen->text() == tr("Cancel")) {
            event->ignore();
            return;
        }
        // "Open as Science Image" falls through
    }

    // Load all dropped files as science images
    event->acceptProposedAction();
    QStringList expanded;
    QMap<QString, QString> fitsToZip;  // extractedPath → sourceZipPath
    for (const auto& p : fitsPaths) {
        if (p.endsWith(".zip", Qt::CaseInsensitive)) {
            const auto extracted = expandZip(p);
            for (const auto& ep : extracted) fitsToZip[ep] = p;
            expanded.append(extracted);
        } else if (isLibArchiveFormat(p)) {
            const auto extracted = expandArchive(p);
            for (const auto& ep : extracted) fitsToZip[ep] = p;
            expanded.append(extracted);
        } else {
            expanded.append(p);
        }
    }

    if (!session_->isEmpty()) {
        const LoadChoice choice = askLoadChoice(session_->imageCount());
        if (choice == LoadChoice::Cancel) return;
        if (choice == LoadChoice::NewProject) {
            onResetFiles();
            resetSessionSettings();
        }
        // LoadChoice::Add → fall through and append
    }

    int dropLimitRejected = 0;
    for (const auto& path : expanded) {
        auto result = core::loadFits(path);
        if (!result) { logPanel_->appendError(tr("Drop failed: %1").arg(result.error())); continue; }
        auto& img = *result;
        img.sourceZipPath = fitsToZip.value(path);
        if (!session_->addImage(img)) { ++dropLimitRejected; continue; }
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
                [this](QPointF px, double ra, double dec, float val) { onPixelClicked(px, ra, dec, val); });
        connect(sw->imageView(), &FitsImageView::escapePressed, this, [this]() {
            imageCatalogTable_->clearSelection();
        });
        applyToolCursor();
        connect(sw, &FitsSubWindow::regionSelected,           this, &MainWindow::onRegionSelected);
        connect(sw, &FitsSubWindow::exportImageRequested,     this, [this, sw]() { onExportImage(sw); });
        connect(sw, &FitsSubWindow::applyDarkRequested,       this, [this, sw]() { onApplyDarkToWindow(sw); });
        connect(sw, &FitsSubWindow::applyFlatRequested,       this, [this, sw]() { onApplyFlatToWindow(sw); });
        connect(sw, &FitsSubWindow::showHistogramRequested,   this, [this, sw]() { onShowHistogram(sw); });
        connect(sw, &FitsSubWindow::showImageCatalogRequested,this, [this]() {
            if (auto* dock = qobject_cast<QDockWidget*>(imageCatalogTable_->parent())) dock->show();
        });
        logPanel_->appendInfo(tr("Dropped: %1  [%2×%3]").arg(img.fileName).arg(img.width).arg(img.height));
        if (img.dateObsAmbiguous)
            logPanel_->appendWarning(tr("%1: DATE-OBS sem fuso horário e sem TIMESYS — assumido UTC; verifique se a câmera grava hora local.").arg(img.fileName));
    }
    if (dropLimitRejected > 0) {
        logPanel_->appendWarning(tr("%1 image(s) not loaded: session limit of %2 reached.")
            .arg(dropLimitRejected).arg(session_->maxImages()));
        QMessageBox::warning(this, tr("Session Limit Reached"),
            tr("%1 image(s) could not be loaded because the session already contains %2 images (limit: %3).\n\n"
               "To load more images, close the current session or increase the limit in Settings.")
                .arg(dropLimitRejected).arg(session_->imageCount()).arg(session_->maxImages()));
    }
    if (!session_->isEmpty()) {
        session_->setStep(core::SessionStep::ImagesLoaded);
        mdiArea_->tileSubWindows();
        sbImages_->setText(tr("%1 image(s)").arg(session_->imageCount()));
        QVector<core::FitsImage> imgs;
        for (int i = 0; i < session_->imageCount(); ++i) imgs.append(session_->image(i));
        thumbnailBar_->setImages(imgs);
        autoFillSettingsFromSession();  // persist detected values to QSettings
        applySettingsToSubsystems();
        updateInfoBar();
        if (workflowPanel_->autoWorkflow())
            QTimer::singleShot(0, this, &MainWindow::onDataReduction);
    }
}

// ─── File menu slots ──────────────────────────────────────────────────────────


void MainWindow::onTileWindows()    { mdiArea_->tileSubWindows(); }
void MainWindow::onCascadeWindows() { mdiArea_->cascadeSubWindows(); }
void MainWindow::onArrangeWindows()
{
    // Restore and fit all sub-windows to their content
    for (auto* sw : mdiArea_->subWindowList()) {
        if (sw->isMinimized() || sw->isMaximized())
            sw->showNormal();
        if (auto* fw = qobject_cast<FitsSubWindow*>(sw->widget()))
            fw->fitToWindow();
    }
}
void MainWindow::onCloseAllImages()
{
    onResetFiles();
}
void MainWindow::onCloseAllWindows() { mdiArea_->closeAllSubWindows(); }

// ─── Help slots ───────────────────────────────────────────────────────────────

void MainWindow::onHelpContents()
{
    if (!helpDialog_) {
        helpDialog_ = new HelpDialog(this);
        helpDialog_->setAttribute(Qt::WA_DeleteOnClose, false);
    }
    helpDialog_->show();
    helpDialog_->raise();
    helpDialog_->activateWindow();
}
void MainWindow::onRegistration()  { statusBar()->showMessage(tr("Registration — N/A (MIT open source)"), 3000); }

void MainWindow::onSetupWizard()
{
    SetupWizard wiz(settings_, this);
    if (wiz.exec() == QDialog::Accepted && wiz.wasModified()) {
        applySettingsToSubsystems();
        updateInfoBar();
        logPanel_->appendInfo(tr("Settings updated via wizard."));
    }
}

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

    blinkCurrentIdx_ = (blinkCurrentIdx_ + 1) % static_cast<int>(wins.size());
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

    // Keep Image Catalog in sync with the active image
    if (auto* dock = qobject_cast<QDockWidget*>(imageCatalogTable_->parent())) {
        if (dock->isVisible()) {
            if (auto* fw = activeFitsWindow()) {
                for (int i = 0; i < session_->imageCount(); ++i) {
                    if (session_->image(i).filePath == fw->fitsImage().filePath) {
                        imageCatalogTable_->setImage(session_->image(i));
                        break;
                    }
                }
            }
        }
    }
}

void MainWindow::onUpdateMenuState()
{
    const bool hasImages  = !session_->isEmpty();
    const bool hasReduced = session_->step() >= core::SessionStep::DataReduced;
    const bool multi      = session_->imageCount() >= 2;
    const bool hasActive  = activeFitsWindow() != nullptr;

    actResetFiles_->setEnabled(hasImages);
    actSaveProject_->setEnabled(hasImages);
    actSaveProjectAs_->setEnabled(hasImages);
    actCloseProject_->setEnabled(hasImages || !currentProjectPath_.isEmpty());
    actViewAdes_->setEnabled(hasReduced);
    actViewPhot_->setEnabled(hasReduced);
    actDataReduction_->setEnabled(hasImages);
    actMOD_->setEnabled(hasReduced);
    actTrackStack_->setEnabled(hasImages);
    actBlink_->setEnabled(multi && !blinkActive_);
    actKOO_->setEnabled(hasImages);
    actZoomIn_->setEnabled(hasActive || blinkActive_);
    actZoomOut_->setEnabled(hasActive || blinkActive_);

    // Sync invert checked state from the currently active view
    {
        FitsImageView* v = nullptr;
        if (auto* fw = activeFitsWindow()) v = fw->imageView();
        else if (blinkActive_ && blinkWidget_) v = blinkWidget_->imageView();
        actInvertDisplay_->setChecked(v && v->inverted());
    }

    // ── WorkflowPanel step 2 label — reflect WCS status ─────────────────────
    if (hasImages) {
        int wcsCount = 0;
        const int total = session_->imageCount();
        for (int i = 0; i < total; ++i)
            if (session_->image(i).wcs.solved) ++wcsCount;
        if (wcsCount == total)
            workflowPanel_->setStepLabel(1, tr("Preparar Imagens"),
                tr("✓ WCS em %1/%1 imagens").arg(total));
        else if (wcsCount > 0)
            workflowPanel_->setStepLabel(1, tr("Preparar Imagens"),
                tr("%1/%2 com WCS — demais serão resolvidas").arg(wcsCount).arg(total));
        else
            workflowPanel_->setStepLabel(1, tr("Reduzir Dados"), tr("Plate solving necessário"));
    } else {
        workflowPanel_->setStepLabel(1, tr("Data Reduction"), tr("Solve plate"));
    }
}

// ─── Toast notifications ──────────────────────────────────────────────────────

void MainWindow::showToast(const QString& msg, Toast::Type type)
{
    Toast::show(this, msg, type);
}

// ─── ZIP extraction ───────────────────────────────────────────────────────────

