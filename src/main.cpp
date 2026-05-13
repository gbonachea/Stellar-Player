#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QLocale>
#include <QTranslator>
#include <QIcon>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <clocale>

#include "MainWindow.h"
#include "SettingsManager.h"
#include "ThemeManager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // mpv requires LC_NUMERIC to be "C" - Qt sets it to system locale
    setlocale(LC_NUMERIC, "C");

    // Set application properties
    app.setApplicationName("Stellar Player");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("StellarSoft");
    app.setOrganizationDomain("stellarsoft.com");

    // Set application icon
    app.setWindowIcon(QIcon(":/icons/stellarpalyer.png"));

    // Initialize managers
    SettingsManager::instance();
    ThemeManager::instance();

    // Load translations
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "StellarPlayer_" + QLocale(locale).name().toLower();
        if (translator.load(":/translations/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    // Show splash screen
    QPixmap pixmap(":/icons/splash.png");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    // Create main window
    MainWindow window;

    // Hide splash screen and show main window
    QTimer::singleShot(2000, &splash, &QSplashScreen::close);
    QTimer::singleShot(2000, &window, &MainWindow::show);

    return app.exec();
}