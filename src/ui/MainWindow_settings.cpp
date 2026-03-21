#include "MainWindow_p.h"

void MainWindow::saveSettings()
{
    settings_.setValue("window/geometry", saveGeometry());
    settings_.setValue("window/state",    saveState(kWindowStateVersion));
    settings_.setValue("ui/theme",        Theme::toString(themeMode_));
}

void MainWindow::loadSettings()
{
    if (settings_.contains("window/geometry"))
        restoreGeometry(settings_.value("window/geometry").toByteArray());
    else
        resize(1024, 720);

    // Dock state is restored in showEvent() to avoid a Qt/Wayland crash that
    // occurs when restoreState() is called before the window receives its first
    // compositor configure event.

    themeMode_ = Theme::fromString(settings_.value(QStringLiteral("ui/theme"), QStringLiteral("night")).toString());
    workflowPanel_->setAutoWorkflow(settings_.value(QStringLiteral("ui/autoWorkflow"), false).toBool());
    session_->setMaxImages(settings_.value(QStringLiteral("session/maxImages"),
                                           core::kDefaultMaxImages).toInt());
    applySettingsToSubsystems();
}

void MainWindow::applyTheme()
{
    // Save explicit dock sizes before the stylesheet changes metrics.
    // resizeDocks() is more reliable than saveState/restoreState here because
    // it doesn't depend on the state-serialisation format reacting to new metrics.
    struct DockSize { QDockWidget* dock; int size; Qt::Orientation orient; };
    QList<DockSize> savedSizes;
    if (isVisible()) {
        for (auto* d : findChildren<QDockWidget*>(Qt::FindDirectChildrenOnly)) {
            if (!d->isVisible() || d->isFloating()) continue;
            const Qt::DockWidgetArea area = dockWidgetArea(d);
            if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
                savedSizes.append({d, d->width(),  Qt::Horizontal});
            else
                savedSizes.append({d, d->height(), Qt::Vertical});
        }
    }

    Theme::apply(qApp, themeMode_);
    mdiArea_->setBackground(QBrush(Theme::mdiBackground(themeMode_)));
    if (thumbnailBar_)
        thumbnailBar_->applyTheme(themeMode_ != Theme::Mode::Day);
    Q_ASSERT(infoBar_);
    infoBar_->applyTheme(themeMode_ != Theme::Mode::Day);

    if (!savedSizes.isEmpty()) {
        // Flush pending StyleChange events so dock metrics (especially title-bar
        // heights) are recalculated before we restore sizes.  This avoids the
        // QTimer::singleShot(100ms) workaround that was unreliable on Wayland.
        QApplication::sendPostedEvents(nullptr, QEvent::StyleChange);
        QList<QDockWidget*> hd, vd;
        QList<int>          hs, vs;
        for (const auto& s : savedSizes) {
            if (s.orient == Qt::Horizontal) { hd << s.dock; hs << s.size; }
            else                            { vd << s.dock; vs << s.size; }
        }
        if (!hd.isEmpty()) resizeDocks(hd, hs, Qt::Horizontal);
        if (!vd.isEmpty()) resizeDocks(vd, vs, Qt::Vertical);
    }

    // Icon shows the CURRENT mode so the user always knows where they are.
    if (actThemeToggle_) {
        if (themeMode_ == Theme::Mode::Night) {
            actThemeToggle_->setIcon(AppIcons::moon());
            actThemeToggle_->setToolTip(tr("Night Mode — click to switch to Day (Ctrl+Shift+T)"));
        } else if (themeMode_ == Theme::Mode::Day) {
            actThemeToggle_->setIcon(AppIcons::sun());
            actThemeToggle_->setToolTip(tr("Day Mode — click to switch to Auto (Ctrl+Shift+T)"));
        } else {
            actThemeToggle_->setIcon(AppIcons::autoTheme());
            actThemeToggle_->setToolTip(tr("Auto Mode — click to switch to Night (Ctrl+Shift+T)"));
        }
    }
}

void MainWindow::onToggleTheme()
{
    switch (themeMode_) {
        case Theme::Mode::Night: themeMode_ = Theme::Mode::Day;   break;
        case Theme::Mode::Day:   themeMode_ = Theme::Mode::Auto;  break;
        case Theme::Mode::Auto:  themeMode_ = Theme::Mode::Night; break;
    }
    applyTheme();
}

// ─── Apply settings to subsystems ────────────────────────────────────────────

void MainWindow::applySettingsToSubsystems()
{
    // Astrometry client (online)
    astrometryClient_->setApiKey(core::ApiKeyStore::read());
    astrometryClient_->setBaseUrl(
        settings_.value(QStringLiteral("astrometry/baseUrl"),
            QStringLiteral("https://nova.astrometry.net")).toString());

    // ASTAP solver (offline)
    astapSolver_->setAstapPath(
        settings_.value(QStringLiteral("astrometry/astapPath")).toString());

    // KOO engine — observer location + MPCORB path
    const SiteLocation site = effectiveSiteLocation();
    kooEngine_->setObserverLocation(site.lat, site.lon, site.alt);

    // Update observatory status label
    Q_ASSERT(sbObservatory_); {
        const QString mode = settings_.value(
            QStringLiteral("observer/locationMode"), QStringLiteral("fits")).toString();
        QString obsText;
        if (mode == QStringLiteral("fits")) {
            if (!session_->isEmpty()) {
                const auto& img0 = session_->image(0);
                if (!std::isnan(img0.siteLat) && !std::isnan(img0.siteLon))
                    obsText = tr("Obs: auto (%1°, %2°)")
                        .arg(img0.siteLat, 0, 'f', 2).arg(img0.siteLon, 0, 'f', 2);
                else
                    obsText = tr("Obs: auto (sem dados no FITS)");
            } else {
                obsText = tr("Obs: auto");
            }
        } else if (mode == QStringLiteral("preset")) {
            const QString code = settings_.value(
                QStringLiteral("observer/presetMpcCode")).toString();
            if (!code.isEmpty()) {
                const QByteArray ba = code.toLatin1();
                if (const Observatory* o = ObservatoryDatabase::byCode(ba.constData())) {
                    obsText = tr("Obs: %1 (%2)")
                        .arg(QString::fromUtf8(o->name))
                        .arg(code);
                } else {
                    obsText = tr("Obs: %1").arg(code);
                }
            }
        } else {
            const double lat = site.lat;
            const double lon = site.lon;
            if (lat == 0.0 && lon == 0.0) {
                obsText = tr("Obs: (não configurado)");
            } else {
                obsText = tr("Obs: %1°, %2°")
                    .arg(lat, 0, 'f', 4)
                    .arg(lon, 0, 'f', 4);
            }
        }
        sbObservatory_->setText(obsText);
        sbObservatory_->setVisible(!obsText.isEmpty());
    }

    const QString mpcPath = settings_.value(QStringLiteral("catalog/mpcOrbPath"),
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/MPCORB.DAT"))
        .toString();
    kooEngine_->setMpcOrbPath(mpcPath);
    kooEngine_->setOfflineFallbackEnabled(QFile::exists(mpcPath));

    // Catalog client — VizieR mirror
    catalogClient_->setVizierUrl(
        settings_.value(QStringLiteral("catalog/vizierServer"),
            QStringLiteral("https://tapvizier.cds.unistra.fr/TAPVizieR/tap/sync")).toString());

    // Astrometry client — timeout
    astrometryClient_->setTimeout(
        settings_.value(QStringLiteral("astrometry/timeoutSec"), 300).toInt());

    // Blink interval
    blinkIntervalMs_ = settings_.value(QStringLiteral("display/blinkIntervalMs"), 500).toInt();

}

// ─── effectiveSiteLocation ────────────────────────────────────────────────────

MainWindow::SiteLocation MainWindow::effectiveSiteLocation() const
{
    const QString mode = settings_.value(
        QStringLiteral("observer/locationMode"), QStringLiteral("fits")).toString();

    // Helper: resolve preset observatory, returns invalid SiteLocation if not found
    auto tryPreset = [&]() -> SiteLocation {
        const QString code = settings_.value(
            QStringLiteral("observer/presetMpcCode")).toString();
        if (!code.isEmpty()) {
            const QByteArray ba = code.toLatin1();
            if (const Observatory* o = ObservatoryDatabase::byCode(ba.constData()))
                return SiteLocation{ o->lat, o->lon, o->alt };
        }
        return {};   // lat=0, lon=0 — caller checks
    };

    // Helper: manual lat/lon/alt from settings
    auto manualSite = [&]() -> SiteLocation {
        return SiteLocation{
            settings_.value(QStringLiteral("observer/latitude"),  0.0).toDouble(),
            settings_.value(QStringLiteral("observer/longitude"), 0.0).toDouble(),
            settings_.value(QStringLiteral("observer/altitude"),  0.0).toDouble()
        };
    };

    if (mode == QStringLiteral("fits")) {
        // Use location from first FITS image if available
        if (!session_->isEmpty()) {
            const auto& img0 = session_->image(0);
            if (!std::isnan(img0.siteLat) && !std::isnan(img0.siteLon)) {
                return SiteLocation{
                    img0.siteLat,
                    img0.siteLon,
                    std::isnan(img0.siteAlt) ? 0.0 : img0.siteAlt
                };
            }
        }
        // FITS has no location — fall back to preset, then manual
        const SiteLocation preset = tryPreset();
        if (preset.lat != 0.0 || preset.lon != 0.0)
            return preset;
        return manualSite();
    }

    if (mode == QStringLiteral("preset")) {
        const SiteLocation preset = tryPreset();
        if (preset.lat != 0.0 || preset.lon != 0.0)
            return preset;
        // Preset not configured — fall back to manual
    }

    // manual (or fallback from preset)
    return manualSite();
}

// ─── School config export / import ───────────────────────────────────────────

void MainWindow::onExportSchoolConfig()
{
    const QString path = QFileDialog::getSaveFileName(this,
        tr("Exportar Configuração da Escola"),
        QDir::homePath() + QStringLiteral("/astrofind_escola.ini"),
        tr("Config (*.ini)"));
    if (path.isEmpty()) return;
    QSettings dest(path, QSettings::IniFormat);
    dest.clear();
    for (const auto& key : settings_.allKeys())
        dest.setValue(key, settings_.value(key));
    dest.sync();
    logPanel_->appendInfo(tr("Configuração exportada para %1").arg(path));
    showToast(tr("Configuração exportada para %1").arg(QFileInfo(path).fileName()));
}

void MainWindow::onImportSchoolConfig()
{
    const QString path = QFileDialog::getOpenFileName(this,
        tr("Importar Configuração da Escola"),
        QDir::homePath(),
        tr("Config (*.ini)"));
    if (path.isEmpty()) return;
    QSettings src(path, QSettings::IniFormat);
    for (const auto& key : src.allKeys())
        settings_.setValue(key, src.value(key));
    settings_.sync();
    applySettingsToSubsystems();
    logPanel_->appendInfo(tr("Configuração importada de %1").arg(path));
    showToast(tr("Configuração importada de %1").arg(QFileInfo(path).fileName()));
}

