#pragma once

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QLabel>
#include <QTimer>
#include <QSettings>
#include <memory>

#include "core/ImageSession.h"

class WorkflowPanel;
class LogPanel;
class FitsSubWindow;

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
    QTimer* blinkTimer_       = nullptr;
    int     blinkCurrentIdx_  = 0;
    int     blinkIntervalMs_  = 500;
    bool    blinkActive_      = false;

    // ── Session ───────────────────────────────────────────────────────────────
    std::unique_ptr<core::ImageSession> session_;

    QSettings settings_;
};
