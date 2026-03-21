#include "SetupWizard.h"

#include <QWizardPage>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QProgressBar>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QUrl>
#include <QElapsedTimer>

// ─── Page IDs ────────────────────────────────────────────────────────────────
enum { PageObserver = 0, PageApiKey = 1, PageMpcOrb = 2 };

// ─── Shared style helpers ────────────────────────────────────────────────────

static QLabel* makeNote(const QString& text, QWidget* parent)
{
    auto* lbl = new QLabel(text, parent);
    lbl->setWordWrap(true);
    lbl->setStyleSheet(QStringLiteral("color:#8899aa; font-size:10px;"));
    return lbl;
}

static QLabel* makeHint(const QString& text, QWidget* parent)
{
    auto* lbl = new QLabel(text, parent);
    lbl->setWordWrap(true);
    lbl->setStyleSheet(QStringLiteral(
        "background:#1a3a1a; color:#80cc80; border:1px solid #3a6a3a;"
        " border-radius:4px; padding:6px; font-size:11px;"));
    return lbl;
}

// ─── Page 1 — Observer ───────────────────────────────────────────────────────

class ObserverPage : public QWizardPage {
    Q_OBJECT
public:
    explicit ObserverPage(QSettings& s, QWidget* parent = nullptr)
        : QWizardPage(parent), settings_(s)
    {
        setTitle(QObject::tr("Step 1 of 3 — Observer Identity"));
        setSubTitle(QObject::tr(
            "These details identify you in every ADES report submitted to the MPC."));

        auto* form = new QFormLayout;
        form->setSpacing(10);

        mpcCode_ = new QLineEdit(this);
        mpcCode_->setMaxLength(3);
        mpcCode_->setPlaceholderText(QStringLiteral("e.g.  568"));
        mpcCode_->setText(s.value(QStringLiteral("observer/mpcCode")).toString());
        form->addRow(QObject::tr("MPC Station Code:"), mpcCode_);
        form->addRow(makeNote(
            QObject::tr("3-character code assigned by the MPC. "
                        "Apply at minorplanetcenter.net if you don't have one yet."), this));

        observerName_ = new QLineEdit(this);
        observerName_->setPlaceholderText(QObject::tr("Full name"));
        observerName_->setText(s.value(QStringLiteral("observer/name")).toString());
        form->addRow(QObject::tr("Your name:"), observerName_);

        telescope_ = new QLineEdit(this);
        telescope_->setPlaceholderText(QObject::tr("e.g.  0.35-m f/7 SCT + CCD"));
        telescope_->setText(s.value(QStringLiteral("observer/telescope")).toString());
        form->addRow(QObject::tr("Telescope:"), telescope_);

        showOnStartupChk_ = new QCheckBox(
            QObject::tr("Show this wizard every time AstroFind starts"), this);
        showOnStartupChk_->setChecked(true);

        auto* vlay = new QVBoxLayout(this);
        vlay->addLayout(form);
        vlay->addSpacing(8);
        vlay->addWidget(makeHint(
            QObject::tr("You can change these values at any time in\n"
                        "Settings → Observer."), this));
        vlay->addSpacing(8);
        vlay->addWidget(showOnStartupChk_);
    }

    bool showOnStartup() const { return showOnStartupChk_ && showOnStartupChk_->isChecked(); }

    void save(QSettings& s) const {
        if (!mpcCode_->text().trimmed().isEmpty())
            s.setValue(QStringLiteral("observer/mpcCode"), mpcCode_->text().trimmed().toUpper());
        if (!observerName_->text().trimmed().isEmpty())
            s.setValue(QStringLiteral("observer/name"), observerName_->text().trimmed());
        if (!telescope_->text().trimmed().isEmpty())
            s.setValue(QStringLiteral("observer/telescope"), telescope_->text().trimmed());
    }

    bool hasInput() const {
        return !mpcCode_->text().trimmed().isEmpty() ||
               !observerName_->text().trimmed().isEmpty();
    }

private:
    QSettings&  settings_;
    QLineEdit*  mpcCode_           = nullptr;
    QLineEdit*  observerName_      = nullptr;
    QLineEdit*  telescope_         = nullptr;
    QCheckBox*  showOnStartupChk_  = nullptr;
};

// ─── Page 2 — API Key ────────────────────────────────────────────────────────

class ApiKeyPage : public QWizardPage {
    Q_OBJECT
public:
    explicit ApiKeyPage(QSettings& s, QWidget* parent = nullptr)
        : QWizardPage(parent)
    {
        setTitle(QObject::tr("Step 2 of 3 — Plate-solving API Key"));
        setSubTitle(QObject::tr(
            "AstroFind uses astrometry.net to automatically identify star fields.\n"
            "A free account gives you unlimited submissions."));

        auto* vlay = new QVBoxLayout(this);
        vlay->setSpacing(10);

        auto* keyRow = new QHBoxLayout;
        apiKey_ = new QLineEdit(this);
        apiKey_->setEchoMode(QLineEdit::Password);
        apiKey_->setPlaceholderText(QObject::tr("Paste your key here"));
        apiKey_->setText(s.value(QStringLiteral("astrometry/apiKey")).toString());
        auto* showBtn = new QPushButton(QObject::tr("Show"), this);
        showBtn->setCheckable(true);
        showBtn->setFixedWidth(60);
        connect(showBtn, &QPushButton::toggled, this, [this, showBtn](bool on) {
            apiKey_->setEchoMode(on ? QLineEdit::Normal : QLineEdit::Password);
            showBtn->setText(on ? QObject::tr("Hide") : QObject::tr("Show"));
        });
        keyRow->addWidget(apiKey_);
        keyRow->addWidget(showBtn);
        vlay->addLayout(keyRow);

        auto* link = new QLabel(
            QStringLiteral("<a href='https://nova.astrometry.net/api_help'>")
            + QObject::tr("Register for a free API key at nova.astrometry.net")
            + QStringLiteral("</a>"), this);
        link->setOpenExternalLinks(true);
        link->setStyleSheet(QStringLiteral("font-size:11px; color:#6699cc;"));
        vlay->addWidget(link);

        vlay->addSpacing(8);
        vlay->addWidget(makeHint(
            QObject::tr("Without an API key, you can still use AstroFind —\n"
                        "plate solving will be unavailable, but you can measure\n"
                        "objects and edit coordinates manually."), this));
        vlay->addStretch();
    }

    void save(QSettings& s) const {
        if (!apiKey_->text().trimmed().isEmpty())
            s.setValue(QStringLiteral("astrometry/apiKey"), apiKey_->text().trimmed());
    }

private:
    QLineEdit* apiKey_ = nullptr;
};

// ─── Page 3 — MPCORB ─────────────────────────────────────────────────────────

class MpcOrbPage : public QWizardPage {
    Q_OBJECT
public:
    explicit MpcOrbPage(QWidget* parent = nullptr)
        : QWizardPage(parent)
        , nam_(new QNetworkAccessManager(this))
    {
        setTitle(QObject::tr("Step 3 of 3 — Asteroid Catalog (optional)"));
        setSubTitle(QObject::tr(
            "Downloading MPCORB.DAT enables offline identification of known asteroids\n"
            "when the SkyBoT service is not reachable (~200 MB)."));

        statusLabel_ = new QLabel(QObject::tr("Not downloaded"), this);
        progressBar_ = new QProgressBar(this);
        progressBar_->setVisible(false);
        speedLabel_  = new QLabel(this);
        speedLabel_->setVisible(false);
        speedLabel_->setStyleSheet(QStringLiteral("color:#8899aa; font-size:10px;"));

        auto* btnRow = new QHBoxLayout;
        downloadBtn_ = new QPushButton(QObject::tr("Download now (~200 MB)"), this);
        cancelBtn_   = new QPushButton(QObject::tr("Cancel"), this);
        cancelBtn_->setVisible(false);
        btnRow->addWidget(downloadBtn_);
        btnRow->addWidget(cancelBtn_);
        btnRow->addStretch();

        skipNote_ = makeNote(
            QObject::tr("You can download it later via  Internet → Download MPCOrb."), this);

        auto* vlay = new QVBoxLayout(this);
        vlay->setSpacing(10);
        vlay->addWidget(statusLabel_);
        vlay->addLayout(btnRow);
        vlay->addWidget(progressBar_);
        vlay->addWidget(speedLabel_);
        vlay->addWidget(skipNote_);
        vlay->addStretch();

        // Check if already downloaded
        destPath_ = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
                  + QStringLiteral("/MPCORB.DAT");
        if (QFile::exists(destPath_)) {
            statusLabel_->setText(QObject::tr("✓  MPCORB.DAT already present"));
            statusLabel_->setStyleSheet(QStringLiteral("color:#80cc80;"));
            downloadBtn_->setEnabled(false);
        }

        connect(downloadBtn_, &QPushButton::clicked, this, &MpcOrbPage::onDownload);
        connect(cancelBtn_,   &QPushButton::clicked, this, &MpcOrbPage::onCancel);
    }

    bool isComplete() const override { return complete_; }

private slots:
    void onDownload()
    {
        downloadBtn_->setVisible(false);
        cancelBtn_->setVisible(true);
        progressBar_->setVisible(true);
        progressBar_->setRange(0, 0);   // indeterminate until Content-Length arrives
        speedLabel_->setVisible(true);
        speedLabel_->setText({});
        statusLabel_->setText(QObject::tr("Connecting…"));

        // Block "Next" while download is in progress
        complete_ = false;
        emit completeChanged();

        QDir().mkpath(QFileInfo(destPath_).absolutePath());
        outFile_ = new QFile(destPath_, this);
        if (!outFile_->open(QIODevice::WriteOnly)) {
            statusLabel_->setText(
                QObject::tr("Cannot open file: %1").arg(outFile_->errorString()));
            delete outFile_; outFile_ = nullptr;
            resetButtons();
            return;
        }

        elapsed_.start();
        lastBytes_   = 0;
        lastElapsedMs_ = 0;

        activeReply_ = nam_->get(QNetworkRequest(
            QUrl(QStringLiteral("https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT"))));

        // Stream-write each chunk as it arrives — avoids 200 MB RAM spike
        connect(activeReply_, &QNetworkReply::readyRead, this, [this]() {
            if (outFile_) outFile_->write(activeReply_->readAll());
        });

        connect(activeReply_, &QNetworkReply::downloadProgress,
                this, [this](qint64 rx, qint64 total) {
                    if (total > 0) {
                        progressBar_->setRange(0, static_cast<int>(total / 1024));
                        progressBar_->setValue(static_cast<int>(rx / 1024));
                    }
                    // Update speed label every ~500 ms
                    const qint64 now = elapsed_.elapsed();
                    const qint64 dt  = now - lastElapsedMs_;
                    if (dt >= 500) {
                        const double speed = static_cast<double>(rx - lastBytes_) * 1000.0 / static_cast<double>(dt); // bytes/sec
                        lastBytes_     = rx;
                        lastElapsedMs_ = now;
                        const QString spd = speed > 1e6
                            ? QObject::tr("%1 MB/s").arg(speed / 1e6, 0, 'f', 1)
                            : QObject::tr("%1 KB/s").arg(speed / 1e3, 0, 'f', 0);
                        speedLabel_->setText(spd);
                    }
                    const QString status = total > 0
                        ? QObject::tr("Downloading… %1 / %2 MB")
                              .arg(rx / 1'000'000).arg(total / 1'000'000)
                        : QObject::tr("Downloading… %1 MB").arg(rx / 1'000'000);
                    statusLabel_->setText(status);
                });

        connect(activeReply_, &QNetworkReply::finished, this, &MpcOrbPage::onFinished);
    }

    void onCancel()
    {
        if (activeReply_) activeReply_->abort();
    }

    void onFinished()
    {
        const QNetworkReply::NetworkError err = activeReply_->error();
        const QString errStr = activeReply_->errorString();
        activeReply_->deleteLater();
        activeReply_ = nullptr;

        if (outFile_) {
            outFile_->close();
            if (err != QNetworkReply::NoError)
                outFile_->remove();
            delete outFile_;
            outFile_ = nullptr;
        }

        progressBar_->setVisible(false);
        speedLabel_->setVisible(false);

        if (err == QNetworkReply::OperationCanceledError) {
            statusLabel_->setText(QObject::tr("Download cancelled."));
            resetButtons();
            return;
        }
        if (err != QNetworkReply::NoError) {
            statusLabel_->setText(QObject::tr("Download failed: %1").arg(errStr));
            resetButtons();
            return;
        }

        statusLabel_->setText(QObject::tr("✓  Downloaded to %1").arg(destPath_));
        statusLabel_->setStyleSheet(QStringLiteral("color:#80cc80;"));
        cancelBtn_->setVisible(false);
        downloadBtn_->setEnabled(false);
        downloadBtn_->setVisible(true);
        complete_ = true;
        emit completeChanged();
    }

private:
    void resetButtons() {
        cancelBtn_->setVisible(false);
        downloadBtn_->setVisible(true);
        downloadBtn_->setEnabled(true);
        complete_ = true;
        emit completeChanged();
    }

    QNetworkAccessManager* nam_;
    QNetworkReply* activeReply_    = nullptr;
    QFile*         outFile_        = nullptr;
    QLabel*        statusLabel_    = nullptr;
    QProgressBar*  progressBar_    = nullptr;
    QPushButton*   downloadBtn_    = nullptr;
    QPushButton*   cancelBtn_      = nullptr;
    QLabel*        speedLabel_     = nullptr;
    QLabel*        skipNote_       = nullptr;
    QString        destPath_;
    QElapsedTimer  elapsed_;
    qint64         lastBytes_      = 0;
    qint64         lastElapsedMs_  = 0;
    bool           complete_       = true;  ///< Download is optional; blocked only during transfer
};

#include "SetupWizard.moc"

// ─── SetupWizard ─────────────────────────────────────────────────────────────

SetupWizard::SetupWizard(QSettings& settings, QWidget* parent)
    : QWizard(parent)
    , settings_(settings)
{
    setWindowTitle(tr("AstroFind Setup Wizard"));
    setWizardStyle(QWizard::ModernStyle);
    setMinimumSize(560, 420);

    auto* p1 = new ObserverPage(settings, this);
    auto* p2 = new ApiKeyPage(settings, this);
    auto* p3 = new MpcOrbPage(this);

    setPage(PageObserver, p1);
    setPage(PageApiKey,   p2);
    setPage(PageMpcOrb,   p3);

    setStartId(PageObserver);

    // "Skip" = custom button 1, appears on every page
    setOption(QWizard::HaveCustomButton1, true);
    setButtonText(QWizard::CustomButton1, tr("Skip this step"));
    connect(this, &QWizard::customButtonClicked, this, [this](int which) {
        if (which == QWizard::CustomButton1)
            next();
    });

    connect(this, &QWizard::finished, this, &SetupWizard::onFinished);
}

void SetupWizard::onFinished(int result)
{
    if (result != QDialog::Accepted) return;

    // Save each page's values (pages are QWizardPage subclasses with save())
    if (auto* p = qobject_cast<ObserverPage*>(page(PageObserver))) {
        p->save(settings_);
        settings_.setValue(QStringLiteral("display/showWizardOnStartup"), p->showOnStartup());
    }
    if (auto* p = qobject_cast<ApiKeyPage*>(page(PageApiKey)))
        p->save(settings_);

    modified_ = true;
}
