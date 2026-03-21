/// @file MainWindow_p.h
/// Private implementation header — included by all MainWindow_*.cpp TUs.
/// Centralises the heavy include list so each split file only needs one line.
#pragma once

#include "MainWindow.h"
#include "FitsSubWindow.h"
#include "WorkflowPanel.h"
#include "LogPanel.h"
#include "AboutDialog.h"
#include "BlinkWidget.h"
#include "ThumbnailBar.h"
#include "BackgroundRangeDialog.h"

#include <QApplication>
#include <QStyleHints>
#include <QClipboard>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QLineEdit>
#include <QCloseEvent>
#include <QDockWidget>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDesktopServices>
#include <QUrl>
#include <QKeySequence>
#include <QProgressDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QNetworkAccessManager>
#include <QElapsedTimer>
#include <QProgressBar>

#include "AppIcons.h"
#include "Toast.h"
#include "Theme.h"
#include "SessionInfoBar.h"
#include "SettingsDialog.h"
#include "MeasurementTable.h"
#include "ImageCatalogTable.h"
#include "CalibrationPanel.h"
#include "VerificationDialog.h"
#include "ReportPreviewDialog.h"
#include "SetupWizard.h"
#include "core/StarDetector.h"
#include "core/ObservatoryDatabase.h"
#include "core/MpcOrb.h"
#include "core/Centroid.h"
#include "core/Photometry.h"
#include "core/AdesReport.h"
#include "core/ExportImage.h"
#include "core/Calibration.h"
#include "core/ImageStacker.h"
#include "core/MovingObjectDetector.h"
#include "core/GusProject.h"
#include "core/ApiKeyStore.h"
#include "EditImageParametersDialog.h"
#include "HelpDialog.h"
#include "CalibrationWizard.h"
#include "MasterFrameDialog.h"
#include "FitsImageView.h"
#include "HorizonsQueryDialog.h"
#include "GrowthCurveDialog.h"
#include "LightCurveDialog.h"
#include "SessionCompareDialog.h"
#include "RegionStatsPanel.h"

#include <QPainter>
#include <QUndoCommand>
#include <QActionGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>
#include <QLabel>
#include <QStyle>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <array>
#include <cmath>

#include <QNetworkReply>
#include <QStandardPaths>

#include <spdlog/spdlog.h>
#include <QtConcurrent/QtConcurrent>

// ─── Shared file-scope constants ─────────────────────────────────────────────

/// Version tag embedded in saveState()/restoreState() data.
/// Increment when dock/toolbar layout changes incompatibly.
static constexpr int kWindowStateVersion = 2;
