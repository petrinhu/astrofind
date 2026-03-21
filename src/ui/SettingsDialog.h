#pragma once

#include <QDialog>
#include <QSettings>
#include <cmath>

class QTabWidget;
class QLineEdit;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class QPushButton;
class QComboBox;
class QRadioButton;
class QGroupBox;
class QButtonGroup;
class QLabel;
class QWidget;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QSettings& settings, QWidget* parent = nullptr);

    /// Provide the FITS location hint before calling exec().
    /// Pass NaN values (or don't call) to indicate no FITS location is available.
    void setFitsLocationHint(double lat, double lon, double alt);

private slots:
    void onAccept();
    void onResetDefaults();
    void onToggleApiKeyVisibility();
    void onBrowseMpcOrb();
    void onBrowseReportDir();
    void onBrowseCcdDir();
    void onBrowseLegacyDir(QLineEdit* target);
    void onApertureToggled();
    void onLocationModeChanged();
    void onCountryChanged(int idx);

private:
    void buildObserverTab(QTabWidget* tabs);
    void buildCameraTab(QTabWidget* tabs);
    void buildConnectionsTab(QTabWidget* tabs);
    void buildDetectionTab(QTabWidget* tabs);
    void buildDisplayTab(QTabWidget* tabs);
    void buildLegacyTab(QTabWidget* tabs);

    void buildLocationGroup(QWidget* page);
    void updateLocationWidgets();
    void populateObservatoryCombo(const QString& countryCode);
    QString fitsLocationLabel() const;

    void loadFromSettings();
    void saveToSettings();
    void resetToDefaults();

    static QWidget* makeAdvancedWarning(QWidget* parent);
    static QWidget* makeDeprecatedWarning(QWidget* parent);
    static QWidget* makeWarningBadge(const QString& text, QWidget* parent);

    QSettings& settings_;

    // ── FITS location hint ─────────────────────────────────────────────────────
    double fitsLat_ = std::numeric_limits<double>::quiet_NaN();
    double fitsLon_ = std::numeric_limits<double>::quiet_NaN();
    double fitsAlt_ = std::numeric_limits<double>::quiet_NaN();

    // ── Observer ──────────────────────────────────────────────────────────────
    QLineEdit*      mpcCodeEdit_     = nullptr;
    QLineEdit*      observerEdit_    = nullptr;
    QLineEdit*      measurerEdit_    = nullptr;
    QLineEdit*      telescopeEdit_   = nullptr;

    // Location mode widgets
    QButtonGroup*   locationBtnGroup_  = nullptr;
    QRadioButton*   locFitsRad_        = nullptr;
    QRadioButton*   locPresetRad_      = nullptr;
    QRadioButton*   locManualRad_      = nullptr;

    // FITS sub-row
    QWidget*        fitsBadge_         = nullptr;   ///< "FITS não fornece localização"
    QLabel*         fitsCoordLabel_    = nullptr;   ///< Lat/Lon/Alt from FITS (readonly)
    QWidget*        fitsSubWidget_     = nullptr;

    // Preset sub-row
    QComboBox*      countryCombo_      = nullptr;
    QComboBox*      obsCombo_          = nullptr;
    QLabel*         presetCoordLabel_  = nullptr;
    QWidget*        presetWarning_     = nullptr;   ///< "Localização FITS disponível"
    QWidget*        presetSubWidget_   = nullptr;

    // Manual sub-row
    QDoubleSpinBox* latitudeSpin_    = nullptr;
    QDoubleSpinBox* longitudeSpin_   = nullptr;
    QSpinBox*       altitudeSpin_    = nullptr;
    QWidget*        manualWarning_   = nullptr;     ///< "Localização FITS disponível"
    QWidget*        manualSubWidget_ = nullptr;

    QDoubleSpinBox* timeZoneSpin_    = nullptr;
    QDoubleSpinBox* timeOffsetSpin_  = nullptr;
    QDoubleSpinBox* timePrecSpin_    = nullptr;
    QLineEdit*      contact1Edit_    = nullptr;
    QLineEdit*      contact2Edit_    = nullptr;
    QLineEdit*      emailEdit_       = nullptr;
    QCheckBox*      inclContactChk_  = nullptr;
    QLineEdit*      schoolEmailEdit_ = nullptr;  ///< school/recipientEmail

    // ── Camera ────────────────────────────────────────────────────────────────
    QDoubleSpinBox* pixScaleXSpin_   = nullptr;
    QDoubleSpinBox* pixScaleYSpin_   = nullptr;
    QDoubleSpinBox* focalLenSpin_    = nullptr;
    QSpinBox*       saturationSpin_  = nullptr;
    QDoubleSpinBox* deltaTSpin_      = nullptr;

    // ── Connections ───────────────────────────────────────────────────────────
    QComboBox*   solverBackendCombo_ = nullptr;
    QLineEdit*   apiKeyEdit_     = nullptr;
    QPushButton* showKeyBtn_     = nullptr;
    QLineEdit*   serverUrlEdit_  = nullptr;
    QSpinBox*    timeoutSpin_    = nullptr;
    QLineEdit*   astapPathEdit_  = nullptr;
    QLineEdit*   mpcOrbPathEdit_ = nullptr;
    QLineEdit*   ccdDirEdit_     = nullptr;
    QLineEdit*   reportDirEdit_  = nullptr;
    QCheckBox*   saveWcsChk_     = nullptr;
    QLineEdit*   vizierEdit_          = nullptr;
    QComboBox*   catalogTypeCombo_    = nullptr;
    QLineEdit*   mpcSubmitEdit_       = nullptr;

    // ── Detection / Photometry ────────────────────────────────────────────────
    QDoubleSpinBox* sigmaLimitSpin_  = nullptr;
    QDoubleSpinBox* minFwhmSpin_     = nullptr;
    QDoubleSpinBox* minSnrSpin_      = nullptr;   // MOD: minimum SNR per detection
    QDoubleSpinBox* magLimitSpin_    = nullptr;   // faint (MinMag)
    QDoubleSpinBox* maxMagSpin_      = nullptr;   // bright (MaxMag)
    QComboBox*      bandCombo_       = nullptr;
    QCheckBox*      includeMagChk_   = nullptr;
    QRadioButton*   apertureAutoRad_ = nullptr;
    QRadioButton*   apertureManRad_  = nullptr;
    QDoubleSpinBox* apertureManSpin_ = nullptr;
    QDoubleSpinBox* extinctionSpin_  = nullptr;

    // ── Display ───────────────────────────────────────────────────────────────
    QSpinBox*  blinkIntervalSpin_  = nullptr;
    QComboBox* themeCombo_         = nullptr;
    QCheckBox* showWizardCheck_    = nullptr;
    QComboBox* langCombo_          = nullptr;

    // ── Legacy ────────────────────────────────────────────────────────────────
    QLineEdit* usnoA2DirEdit_  = nullptr;
    QLineEdit* ucac2DirEdit_   = nullptr;
    QLineEdit* ucac3DirEdit_   = nullptr;
    QLineEdit* cmc14DirEdit_   = nullptr;
};
