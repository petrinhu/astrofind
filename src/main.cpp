#include "ui/MainWindow.h"

#include <QApplication>
#include <QIcon>
#include <QLocale>
#include <QTranslator>
#include <QSettings>
#include <QDir>
#include <QStandardPaths>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("AstroFind");
    app.setApplicationVersion("0.1.0");
    app.setOrganizationName("AstroFind");
    app.setOrganizationDomain("astrofind.app");
    // Wayland: links this process to the installed .desktop file so the
    // compositor can resolve the icon from the hicolor theme.
    app.setDesktopFileName(QStringLiteral("astrofind"));

    // Application icon — multi-resolution QIcon from embedded PNGs
    QIcon appIcon;
    appIcon.addFile(QStringLiteral(":/icons/astrofind-16.png"),  QSize(16,  16));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-32.png"),  QSize(32,  32));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-48.png"),  QSize(48,  48));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-64.png"),  QSize(64,  64));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-128.png"), QSize(128, 128));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-256.png"), QSize(256, 256));
    appIcon.addFile(QStringLiteral(":/icons/astrofind-512.png"), QSize(512, 512));
    app.setWindowIcon(appIcon);

    // Logging
    auto logger = spdlog::stdout_color_mt("astrofind");
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::debug);
    spdlog::info("AstroFind {} starting", app.applicationVersion().toStdString());

    // Translations
    QSettings settings;
    QString lang = settings.value("ui/language", QLocale::system().name()).toString();

    QTranslator translator;
    const QStringList searchPaths = {
        QCoreApplication::applicationDirPath() + "/translations",
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/translations",
        ":/translations"
    };
    for (const auto& path : searchPaths) {
        if (translator.load(QString("astrofind_%1").arg(lang), path)) {
            app.installTranslator(&translator);
            spdlog::info("Loaded translation: {}", lang.toStdString());
            break;
        }
    }

    MainWindow window;
    window.setWindowIcon(appIcon);   // X11: must be set per-window, not just on QApplication
    window.show();

    return app.exec();
}
