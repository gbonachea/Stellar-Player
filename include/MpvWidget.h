#ifndef MPVWIDGET_H
#define MPVWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QSocketNotifier>
#include <mpv/client.h>
#include <mpv/render_gl.h>

class MpvWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    MpvWidget(QWidget *parent = nullptr);
    ~MpvWidget();

    void initializeMpv();
    void terminateMpv();
    void loadFile(const QString &file);
    void loadUrl(const QString &url);
    void play();
    void pause();
    void stop();
    void seek(double position);
    void setVolume(int volume);
    void setMute(bool mute);
    void setSpeed(double speed);
    void setLoop(bool loop);
    void setFullscreen(bool fullscreen);
    void setAspectRatio(const QString &ratio);
    void rotate(int degrees);
    void screenshot();
    void loadSubtitles(const QString &file);
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

signals:
    void durationChanged(double duration);
    void positionChanged(double position);
    void stateChanged();
    void errorOccurred(const QString &error);
    void fileLoaded();
    void fileClosed();

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;

private slots:
    void triggerUpdate();

private:
    static void on_update(void *ctx);
    void handle_mpv_events();

    mpv_handle *mpv;
    mpv_render_context *mpv_gl;
    QSocketNotifier *notifier;
    bool initialized;
};

#endif // MPVWIDGET_H