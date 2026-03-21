#pragma once

#include "core/HorizonsClient.h"
#include "core/Ephemeris.h"

#include <QDialog>
#include <optional>

class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;
class QNetworkAccessManager;

/// Dialog for querying JPL Horizons for a single solar-system object.
/// Displays predicted RA/Dec/magnitude and lets the user add the result
/// to the current KOO overlay.
class HorizonsQueryDialog : public QDialog {
    Q_OBJECT
public:
    /// @param nam      Network manager (shared with the rest of the app).
    /// @param jd       Julian Date to use for the ephemeris query.
    /// @param parent   Parent widget.
    explicit HorizonsQueryDialog(QNetworkAccessManager* nam,
                                  double jd,
                                  QWidget* parent = nullptr);

signals:
    /// Emitted when the user clicks "Add to Overlay" after a successful query.
    void overlayRequested(const core::EphemerisMatch& match);

private slots:
    void onQuery();
    void onResultReady(const core::EphemerisMatch& match);
    void onFailed(const QString& reason);

private:
    core::HorizonsClient* client_   = nullptr;
    double                jd_       = 0.0;

    QLineEdit*   targetEdit_    = nullptr;
    QPushButton* queryBtn_      = nullptr;
    QLabel*      statusLabel_   = nullptr;
    QGroupBox*   resultGroup_   = nullptr;
    QLabel*      nameLabel_     = nullptr;
    QLabel*      raLabel_       = nullptr;
    QLabel*      decLabel_      = nullptr;
    QLabel*      magLabel_      = nullptr;
    QLabel*      motionLabel_   = nullptr;
    QPushButton* addOverlayBtn_ = nullptr;

    std::optional<core::EphemerisMatch> lastMatch_;
};
