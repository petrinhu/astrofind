#pragma once

#include <QFrame>
#include <QLabel>
#include <QToolButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

namespace core { class ImageSession; }

/// Non-blocking notification bar shown after images are loaded.
/// Summarises what was auto-detected from the FITS headers and
/// flags anything the user still needs to configure.
class SessionInfoBar : public QFrame {
    Q_OBJECT
public:
    explicit SessionInfoBar(QWidget* parent = nullptr);

    /// Populate and show the bar from a loaded session.
    /// @param locationMode   current "observer/locationMode" setting
    /// @param locationOk     true when location resolved to non-zero coords
    void showForSession(const core::ImageSession& session,
                        const QString& locationMode,
                        bool locationOk);

    void applyTheme(bool night);

signals:
    void openSettingsRequested();

private:
    void rebuild();

    // ── Widgets ──────────────────────────────────────────────────────────────
    QLabel*      summary_  = nullptr;   ///< Row 1: "N imagens — Telescópio · Data · …"
    QWidget*     chipsRow_ = nullptr;   ///< Row 2: detected ✓ and ⚠ chips
    QHBoxLayout* chipsLay_ = nullptr;
    QToolButton* closeBtn_ = nullptr;

    // ── State ─────────────────────────────────────────────────────────────────
    bool night_ = true;

    struct Info {
        int     imageCount    = 0;
        QString telescope;
        QString dateObs;
        QString filter;
        double  expTime       = 0.0;
        int     width         = 0;
        int     height        = 0;
        double  pixScale      = 0.0;   ///< arcsec/px; 0 = not detected
        bool    wcsPresolved  = false;
        double  saturation    = 0.0;   ///< ADU; 0 = not detected
        bool    dateTimeOk    = false;
        bool    locationOk    = false;
        QString locationMode;
    } info_;

    // ── Helpers ───────────────────────────────────────────────────────────────
    QLabel* makeChip(const QString& text, bool ok);
    void    clearChips();
};
