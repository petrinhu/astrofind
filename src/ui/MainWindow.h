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
#include "core/AstapSolver.h"
#include "core/CatalogClient.h"
#include "core/KooEngine.h"
#include "BlinkWidget.h"
#include "ThumbnailBar.h"
#include "Theme.h"
#include "MeasurementTable.h"
#include "ImageCatalogTable.h"
#include "CalibrationPanel.h"
#include "VerificationDialog.h"
#include "SelectMarkingsDialog.h"
#include "HelpDialog.h"
#include "LightCurveDialog.h"
#include "SessionCompareDialog.h"
#include "Toast.h"

#include <QSpinBox>
#include <QUndoStack>
#include <QFutureWatcher>

class QNetworkAccessManager;

class WorkflowPanel;
class LogPanel;
class SessionInfoBar;
class FitsSubWindow;
class BackgroundRangeDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private slots:
    // ── File menu ─────────────────────────────────────────────────────────────
    void onLoadImages();
    void onSaveProject();
    void onSaveProjectAs();
    void onOpenProject();
    void onCloseProject();
    void onSaveAllFits();
    void onSaveAsFits();
    void onLoadDarkFrame();
    void onLoadFlatField();
    void onBuildMasterDark();
    void onBuildMasterFlat();
    void onCalibrationWizard();
    void onViewAdesReport();
    void onViewPhotometryFile();
    void onViewLogFile();
    void onResetFiles();
    void onReloadMpcOrb();
    void onSettings();
    void onExportSchoolConfig();
    void onImportSchoolConfig();
    void onExit();

    // ── Edit menu ─────────────────────────────────────────────────────────────
    void onUndo();
    void onRedo();
    void onClearMarkings();
    void onCopyImage();

    // ── Tool modes ────────────────────────────────────────────────────────────
    void onSelectTool();
    void onApertureTool();
    void onAnnotateTool();

    // ── Blink control ─────────────────────────────────────────────────────────
    void onBlinkPrev();
    void onBlinkNext();

    // ── Additional view ───────────────────────────────────────────────────────
    void onZoom1to1();
    void onFitAllWindows();

    // ── Astrometry menu ───────────────────────────────────────────────────────
    void onDataReduction();
    void onStopReduction();
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
    void onLightCurve();
    void onGrowthCurve();
    void onCompareSessions();

    // ── Internet menu ─────────────────────────────────────────────────────────
    void onDownloadMpcOrb();
    void onUpdateMpcOrb();
    void onQueryHorizons();
    void onAstrometricaWebPage();

    // ── Windows menu ──────────────────────────────────────────────────────────
    void onTileWindows();
    void onCascadeWindows();
    void onArrangeWindows();
    void onCloseAllImages();
    void onCloseAllWindows();

    // ── Help menu ─────────────────────────────────────────────────────────────
    void onHelpContents();
    void onSetupWizard();
    void onRegistration();
    void onAbout();

    // ── Theme ─────────────────────────────────────────────────────────────────
    void onToggleTheme();

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
    void applyTheme();

    void addToRecentDirs(const QString& dirPath);
    void updateRecentMenu();
    void loadFromDir(const QString& dirPath);
    void applySettingsToSubsystems();

    /// Observer site location resolved according to observer/locationMode setting.
    struct SiteLocation { double lat = 0.0, lon = 0.0, alt = 0.0; };
    SiteLocation effectiveSiteLocation() const;

    bool stateRestored_        = false;  ///< dock state deferred until after showEvent
    bool initialDocksResized_  = false;  ///< true once the fresh-layout resizeDocks() has run

    FitsSubWindow* activeFitsWindow() const;
    QList<FitsSubWindow*> allFitsWindows() const;

    // ── Layout ────────────────────────────────────────────────────────────────
    QMdiArea*       mdiArea_       = nullptr;
    WorkflowPanel*  workflowPanel_ = nullptr;
    LogPanel*       logPanel_      = nullptr;
    SessionInfoBar* infoBar_       = nullptr;

    // ── Status bar labels ─────────────────────────────────────────────────────
    QLabel* sbStep_        = nullptr;   ///< Current workflow step
    QLabel* sbCoords_      = nullptr;   ///< RA / Dec under cursor
    QLabel* sbPixel_       = nullptr;   ///< Pixel value under cursor
    QLabel* sbImages_      = nullptr;   ///< Loaded image count
    QLabel* sbObservatory_ = nullptr;   ///< Active observatory / location mode

    // ── Menus (kept for enable/disable) ──────────────────────────────────────
    QMenu* fileMenu_       = nullptr;
    QMenu* recentMenu_     = nullptr;
    QMenu* editMenu_       = nullptr;
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
    QAction* actDataReduction_  = nullptr;
    QAction* actStopReduction_  = nullptr;
    QAction* actMOD_            = nullptr;
    bool     reductionRunning_  = false;
    int      reductionSolved_   = 0;   ///< Images solved this run
    int      reductionSkipped_  = 0;   ///< Images with pre-existing WCS (skipped)
    int      reductionFailed_   = 0;   ///< Images that failed plate solving
    QAction* actTrackStack_    = nullptr;
    QAction* actBlink_         = nullptr;
    QAction* actStopBlink_     = nullptr;
    QAction* actKOO_           = nullptr;
    QAction* actZoomIn_        = nullptr;
    QAction* actZoomOut_       = nullptr;
    QAction* actUndo_          = nullptr;
    QAction* actRedo_          = nullptr;
    QAction* actClearMarkings_ = nullptr;
    QAction* actSelectTool_    = nullptr;
    QAction* actApertureTool_  = nullptr;
    QAction* actAnnotateTool_  = nullptr;
    QAction* actBgRange_       = nullptr;
    QAction* actZoom1to1_      = nullptr;
    QAction* actFitAllWin_     = nullptr;

    // ── Blink delay spinbox ───────────────────────────────────────────────────
    QSpinBox* blinkDelayBox_ = nullptr;

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
    core::AstapSolver*        astapSolver_      = nullptr;
    core::CatalogClient*      catalogClient_    = nullptr;
    core::KooEngine*          kooEngine_        = nullptr;
    QVector<int>              reductionQueue_;   ///< Images pending astrometry
    int                       reducingIdx_      = -1;
    QFutureWatcher<QVector<core::DetectedStar>>* starWatcher_ = nullptr;

    void showToast(const QString& msg, Toast::Type type = Toast::Type::Success);

    void updateInfoBar();
    void autoFillSettingsFromSession();  ///< Save camera/location/timezone from FITS on load
    void resetSessionSettings();        ///< Clear auto-filled camera/location/timezone settings

    enum class LoadChoice { Add, NewProject, Cancel };
    LoadChoice askLoadChoice(int currentCount);

    void processNextReduction();
    void onAstrometrySolved(const core::PlateSolution& wcs, int jobId);
    void onAstrometryFailed(const QString& reason);
    void onAllStarsDetected();
    void onCatalogReady(const QVector<core::CatalogStar>& stars);
    void onKooReady(const QVector<core::KooObject>& objects);
    void updateImageOverlay(int sessionIdx);
    void clearAllHighlights();

    // ── Project management ────────────────────────────────────────────────────
    void doSaveProject(const QString& path);
    void doLoadProject(const QString& path);
    bool maybeSaveProject();  ///< Ask to save if modified; returns false if user cancelled
    void setProjectModified(bool v);
    void updateWindowTitle();

    // ── Phase 4: Measurement ──────────────────────────────────────────────────
    enum class ToolMode { Select, Aperture, Annotate };

    MeasurementTable*    measureTable_       = nullptr;
    QAction*             actMeasure_         = nullptr;  ///< Toggle measure-click mode
    bool                 measureMode_        = false;
    ToolMode             toolMode_           = ToolMode::Select;

    void applyToolCursor();

    // ── Sprint A/B: Image Catalog + Calibration ───────────────────────────────
    ImageCatalogTable*   imageCatalogTable_  = nullptr;
    CalibrationPanel*    calibPanel_         = nullptr;
    VerificationDialog*  verifyDlg_          = nullptr;  ///< Reused non-modal dialog
    HelpDialog*          helpDialog_         = nullptr;  ///< Reused help window
    LightCurveDialog*    lightCurveDlg_      = nullptr;  ///< Reused non-modal light curve
    MarkingsState        markingsState_;                 ///< Current overlay visibility
    QUndoStack*          undoStack_          = nullptr;
    core::FitsImage      darkFrame_;          ///< Loaded dark frame (empty if none)
    core::FitsImage      flatFrame_;          ///< Loaded flat field (empty if none)

    void onPixelClicked(QPointF imgPx, double ra, double dec, float pixVal);
    void runMeasurePipeline(int sessionIdx, QPointF imgPx, double ra, double dec);

    // ── Context menu handlers ──────────────────────────────────────────────────
    void onExportImage(FitsSubWindow* sw);
    void onApplyDarkToWindow(FitsSubWindow* sw);
    void onApplyFlatToWindow(FitsSubWindow* sw);
    void onShowHistogram(FitsSubWindow* sw);

    // ── Project state ─────────────────────────────────────────────────────────
    QString currentProjectPath_;      ///< Empty when no project file is associated
    bool    projectModified_ = false; ///< True when session has unsaved changes
    QAction* actSaveProject_   = nullptr;
    QAction* actSaveProjectAs_ = nullptr;
    QAction* actOpenProject_   = nullptr;
    QAction* actCloseProject_  = nullptr;

    // ── Session ───────────────────────────────────────────────────────────────
    std::unique_ptr<core::ImageSession> session_;
    std::unique_ptr<QTemporaryDir>      tempDir_;   ///< Temp dir for ZIP extractions

    QSettings    settings_;
    Theme::Mode  themeMode_ = Theme::Mode::Night;
    QAction*     actThemeToggle_ = nullptr;

    /// Extract FITS files from a ZIP archive into a temp directory.
    QStringList expandZip(const QString& zipPath);
};
