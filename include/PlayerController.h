#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>

class MpvWidget;
class PlaylistModel;

class PlayerController : public QObject
{
    Q_OBJECT

public:
    explicit PlayerController(MpvWidget *mpvWidget, PlaylistModel *playlistModel, QObject *parent = nullptr);
    ~PlayerController();

    void openFile(const QString &filePath);
    void openUrl(const QString &url);
    void play();
    void pause();
    void stop();
    void next();
    void previous();
    void seek(double position);
    void setVolume(int volume);
    void setMute(bool mute);
    void setSpeed(double speed);
    void setLoop(bool loop);
    void setFullscreen(bool fullscreen);
    void setAspectRatio(const QString &ratio);
    void rotate(int degrees);
    void screenshot();
    void loadSubtitles(const QString &filePath);
    void setSubtitleTrack(int track);
    void setSubtitleDelay(double delay);
    void setSubtitleSize(int size);
    void setSubtitleColor(const QColor &color);
    void setSubtitlePosition(int position);

    double getDuration() const;
    double getPosition() const;
    int getVolume() const;
    bool getMute() const;
    double getSpeed() const;
    bool getLoop() const;
    QString getAspectRatio() const;
    int getRotation() const;
    int getSubtitleTrack() const;
    double getSubtitleDelay() const;
    int getSubtitleSize() const;
    QColor getSubtitleColor() const;
    int getSubtitlePosition() const;

    void addToPlaylist(const QString &filePath);
    void removeFromPlaylist(int index);
    void clearPlaylist();
    void shufflePlaylist();
    void setRepeatMode(bool repeat);
    void setShuffleMode(bool shuffle);

signals:
    void durationChanged(double duration);
    void positionChanged(double position);
    void stateChanged();
    void errorOccurred(const QString &error);
    void playlistChanged();
    void currentTrackChanged(int index);

private slots:
    void onMpvDurationChanged(double duration);
    void onMpvPositionChanged(double position);
    void onMpvStateChanged();
    void onMpvErrorOccurred(const QString &error);

private:
    MpvWidget *mpvWidget;
    PlaylistModel *playlistModel;
    int currentTrackIndex;
    bool repeatMode;
    bool shuffleMode;
};

#endif // PLAYERCONTROLLER_H