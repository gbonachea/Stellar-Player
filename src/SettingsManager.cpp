#include "SettingsManager.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDir>

SettingsManager *SettingsManager::instance()
{
    static SettingsManager instance;
    return &instance;
}

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent)
    , settings(new QSettings(QSettings::IniFormat, QSettings::UserScope,
                             QApplication::organizationName(), QApplication::applicationName(), this))
{
}

SettingsManager::~SettingsManager()
{
}

void SettingsManager::setWindowSize(const QSize &size)
{
    settings->setValue("window/size", size);
}

QSize SettingsManager::getWindowSize() const
{
    return settings->value("window/size", QSize(800, 600)).toSize();
}

void SettingsManager::setWindowPosition(const QPoint &position)
{
    settings->setValue("window/position", position);
}

QPoint SettingsManager::getWindowPosition() const
{
    return settings->value("window/position", QPoint(100, 100)).toPoint();
}

void SettingsManager::setVolume(int volume)
{
    settings->setValue("audio/volume", volume);
}

int SettingsManager::getVolume() const
{
    return settings->value("audio/volume", 50).toInt();
}

void SettingsManager::setMute(bool mute)
{
    settings->setValue("audio/mute", mute);
}

bool SettingsManager::getMute() const
{
    return settings->value("audio/mute", false).toBool();
}

void SettingsManager::setLastOpenedFiles(const QStringList &files)
{
    settings->setValue("files/lastOpened", files);
}

QStringList SettingsManager::getLastOpenedFiles() const
{
    return settings->value("files/lastOpened").toStringList();
}

void SettingsManager::setTheme(const QString &theme)
{
    settings->setValue("appearance/theme", theme);
}

QString SettingsManager::getTheme() const
{
    return settings->value("appearance/theme", "dark").toString();
}

void SettingsManager::setSubtitleSize(int size)
{
    settings->setValue("subtitles/size", size);
}

int SettingsManager::getSubtitleSize() const
{
    return settings->value("subtitles/size", 100).toInt();
}

void SettingsManager::setSubtitleColor(const QColor &color)
{
    settings->setValue("subtitles/color", color);
}

QColor SettingsManager::getSubtitleColor() const
{
    return settings->value("subtitles/color", QColor(Qt::white)).value<QColor>();
}

void SettingsManager::setSubtitlePosition(int position)
{
    settings->setValue("subtitles/position", position);
}

int SettingsManager::getSubtitlePosition() const
{
    return settings->value("subtitles/position", 100).toInt();
}

void SettingsManager::setSubtitleDelay(double delay)
{
    settings->setValue("subtitles/delay", delay);
}

double SettingsManager::getSubtitleDelay() const
{
    return settings->value("subtitles/delay", 0.0).toDouble();
}