#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QSize>
#include <QPoint>
#include <QString>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    static SettingsManager *instance();

    void setWindowSize(const QSize &size);
    QSize getWindowSize() const;

    void setWindowPosition(const QPoint &position);
    QPoint getWindowPosition() const;

    void setVolume(int volume);
    int getVolume() const;

    void setMute(bool mute);
    bool getMute() const;

    void setLastOpenedFiles(const QStringList &files);
    QStringList getLastOpenedFiles() const;

    void setTheme(const QString &theme);
    QString getTheme() const;

    void setSubtitleSize(int size);
    int getSubtitleSize() const;

    void setSubtitleColor(const QColor &color);
    QColor getSubtitleColor() const;

    void setSubtitlePosition(int position);
    int getSubtitlePosition() const;

    void setSubtitleDelay(double delay);
    double getSubtitleDelay() const;

private:
    explicit SettingsManager(QObject *parent = nullptr);
    ~SettingsManager();

    QSettings *settings;
};

#endif // SETTINGSMANAGER_H