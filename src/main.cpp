#include "ui/MainWindow.h"

#include <QApplication>
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
    window.show();

    return app.exec();
}
