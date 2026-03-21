#pragma once

#include "core/AdesReport.h"
#include "core/Measurement.h"
#include "core/MpcSubmit.h"
#include "core/ImageSession.h"

#include <QDialog>
#include <QVector>
#include <QSettings>

class QTabWidget;
class QPlainTextEdit;
class QLabel;
class QPushButton;

/// Dialog that shows the generated ADES 2022 report (XML and PSV tabs)
/// with Save, Save to Reports Folder, Copy, Submit HTTP/Email, and Export PDF.
class ReportPreviewDialog : public QDialog {
    Q_OBJECT
public:
    explicit ReportPreviewDialog(const QVector<core::Observation>& obs,
                                  const core::AdesContext& ctx,
                                  QSettings& settings,
                                  const core::ImageSession* session = nullptr,
                                  QWidget* parent = nullptr);

private slots:
    void onSave();
    void onSaveToFolder();
    void onCopy();
    void onSubmitHttp();
    void onSubmitEmail();
    void onSendToSchool();
    void onSubmitted(const QString& response);
    void onSubmitFailed(const QString& reason);
    void onSubmitTimedOut();
    void onExportPdf();

private:
    QString buildPdfHtml() const;
    QString smartBaseName() const;   ///< e.g. "ADES_568_20240520"
    void    saveFile(const QString& path, const QString& content);

    QTabWidget*     tabs_           = nullptr;
    QPlainTextEdit* xmlEdit_        = nullptr;
    QPlainTextEdit* psvEdit_        = nullptr;
    QLabel*         summaryLabel_   = nullptr;
    QPushButton*    btnSubmitHttp_   = nullptr;
    QPushButton*    btnSubmitEmail_  = nullptr;
    QPushButton*    btnSendToSchool_ = nullptr;
    QPushButton*    btnExportPdf_    = nullptr;

    QVector<core::Observation>  obs_;
    core::AdesContext           ctx_;
    const core::ImageSession*   imgSession_  = nullptr;
    QSettings&                  settings_;

    QString          xmlText_;
    QString          psvText_;
    core::MpcSubmit* submitter_ = nullptr;
};
