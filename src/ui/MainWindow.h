#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <QTemporaryDir>
#include <memory>

#include "core/ImageSession.h"
#include "core/AstrometryClient.h"
#include "core/CatalogClient.h"
#include "core/KooEngine.h"
#include "BlinkWidget.h"
#include "ThumbnailBar.h"

class QNetworkAccessManager;

class WorkflowPanel;
class LogPanel;
class FitsSubWindow;
class BackgroundRangeDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;

private slots:
    // ── File menu ─────────────────────────────────────────────────────────────
    void onLoadImages();
    void onSaveAllFits();
    void onSaveAsFits();
    void onLoadDarkFrame();
    void onLoadFlatField();
    void onViewAdesReport();
    void onViewPhotometryFile();
    void onViewLogFile();
    void onResetFiles();
    void onReloadMpcOrb();
    void onSettings();
    void onExit();

    // ── Astrometry menu ───────────────────────────────────────────────────────
    void onDataReduction();
    void onMovingObjectDetection();
    void onTrackAndStack();

    // ── Images menu ───────────────────────────────────────────────────────────
    void onEditImageParameters();
    void onDisplayHeader();
    void onBackgroundAndRange();
    void onReStackImages();
    void onZoomIn();
    void onZoomOut();
    void onFitWindowSize();
    void onSelectMarkings();
    void onInvertDisplay();
    void onFlipHorizontal();
    void onFlipVertical();
    void onMagnifyingGlass();

    // ── Tools menu ────────────────────────────────────────────────────────────
    void onBlinkImages();
    void onStopBlinking();
    void onKnownObjectOverlay();

    // ── Internet menu ─────────────────────────────────────────────────────────
    void onDownloadMpcOrb();
    void onUpdateMpcOrb();
    void onAstrometricaWebPage();

    // ── Windows menu ──────────────────────────────────────────────────────────
    void onTileWindows();
    void onCascadeWindows();
    void onArrangeWindows();
    void onCloseAllImages();
    void onCloseAllWindows();

    // ── Help menu ─────────────────────────────────────────────────────────────
    void onHelpContents();
    void onRegistration();
    void onAbout();

    // ── Blink timer ───────────────────────────────────────────────────────────
    void onBlinkTick();

    // ── Session/status ────────────────────────────────────────────────────────
    void onSessionStepChanged(core::SessionStep step);
    void onSubWindowActivated(QMdiSubWindow* w);
    void onUpdateMenuState();

private:
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void setupDockWidgets();
    void setupConnections();

    void saveSettings();
    void loadSettings();

    FitsSubWindow* activeFitsWindow() const;
    QList<FitsSubWindow*> allFitsWindows() const;

    // ── Layout ────────────────────────────────────────────────────────────────
    QMdiArea*      mdiArea_       = nullptr;
    WorkflowPanel* workflowPanel_ = nullptr;
    LogPanel*      logPanel_      = nullptr;

    // ── Status bar labels ─────────────────────────────────────────────────────
    QLabel* sbStep_    = nullptr;   ///< Current workflow step
    QLabel* sbCoords_  = nullptr;   ///< RA / Dec under cursor
    QLabel* sbPixel_   = nullptr;   ///< Pixel value under cursor
    QLabel* sbImages_  = nullptr;   ///< Loaded image count

    // ── Menus (kept for enable/disable) ──────────────────────────────────────
    QMenu* fileMenu_       = nullptr;
    QMenu* astrometryMenu_ = nullptr;
    QMenu* imagesMenu_     = nullptr;
    QMenu* toolsMenu_      = nullptr;
    QMenu* internetMenu_   = nullptr;
    QMenu* windowMenu_     = nullptr;
    QMenu* helpMenu_       = nullptr;

    // ── Actions (kept for enable/disable) ────────────────────────────────────
    QAction* actLoadImages_    = nullptr;
    QAction* actResetFiles_    = nullptr;
    QAction* actViewAdes_      = nullptr;
    QAction* actViewPhot_      = nullptr;
    QAction* actDataReduction_ = nullptr;
    QAction* actMOD_           = nullptr;
    QAction* actTrackStack_    = nullptr;
    QAction* actBlink_         = nullptr;
    QAction* actStopBlink_     = nullptr;
    QAction* actKOO_           = nullptr;
    QAction* actZoomIn_        = nullptr;
    QAction* actZoomOut_       = nullptr;

    // ── Blink state ───────────────────────────────────────────────────────────
    QTimer*      blinkTimer_       = nullptr;
    int          blinkCurrentIdx_  = 0;
    int          blinkIntervalMs_  = 500;
    bool         blinkActive_      = false;

    // ── Phase 1: Blink widget + thumbnail bar ─────────────────────────────────
    BlinkWidget*    blinkWidget_   = nullptr;   ///< Shown in MDI during blink mode
    ThumbnailBar*   thumbnailBar_  = nullptr;   ///< Bottom dock strip
    QMdiSubWindow*  blinkMdiWin_   = nullptr;   ///< MDI container for blink widget

    // ── Phase 2: network + astrometry clients ─────────────────────────────────
    QNetworkAccessManager*    nam_              = nullptr;
    core::AstrometryClient*   astrometryClient_ = nullptr;
    core::CatalogClient*      catalogClient_    = nullptr;
    core::KooEngine*          kooEngine_        = nullptr;
    QVector<int>              reductionQueue_;   ///< Images pending astrometry
    int                       reducingIdx_      = -1;

    void processNextReduction();
    void onAstrometrySolved(const core::PlateSolution& wcs, int jobId);
    void onAstrometryFailed(const QString& reason);
    void onCatalogReady(const QVector<core::CatalogStar>& stars);
    void onKooReady(const QVector<core::KooObject>& objects);
    void updateImageOverlay(int sessionIdx);

    // ── Session ───────────────────────────────────────────────────────────────
    std::unique_ptr<core::ImageSession> session_;
    std::unique_ptr<QTemporaryDir>      tempDir_;   ///< Temp dir for ZIP extractions

    QSettings settings_;

    /// Extract FITS files from a ZIP archive into a temp directory.
    QStringList expandZip(const QString& zipPath);
};
