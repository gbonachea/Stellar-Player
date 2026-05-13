#include "MpvWidget.h"

#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QResizeEvent>
#include <QTimer>
#include <QDebug>

static void *get_proc_address_mpv(void *ctx, const char *name)
{
    Q_UNUSED(ctx);
    QOpenGLContext *glctx = QOpenGLContext::currentContext();
    if (!glctx) return nullptr;
    return reinterpret_cast<void*>(glctx->getProcAddress(name));
}

MpvWidget::MpvWidget(QWidget *parent)
    : QOpenGLWidget(parent)
    , mpv(nullptr)
    , mpv_gl(nullptr)
    , notifier(nullptr)
    , initialized(false)
{
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_OpaquePaintEvent);
}

MpvWidget::~MpvWidget()
{
    terminateMpv();
}

void MpvWidget::initializeMpv()
{
    if (initialized) {
        qDebug() << "initializeMpv: already initialized";
        return;
    }

    qDebug() << "initializeMpv: creating mpv instance...";

    mpv = mpv_create();
    if (!mpv) {
        qCritical() << "initializeMpv: mpv_create failed";
        return;
    }

    mpv_set_option_string(mpv, "hwdec", "no");
    mpv_set_option_string(mpv, "profile", "fast");
    mpv_set_option_string(mpv, "vo", "libmpv");
    mpv_set_option_string(mpv, "gpu-context", "auto");
    mpv_set_option_string(mpv, "scale", "bilinear");
    mpv_set_option_string(mpv, "cscale", "bilinear");
    mpv_set_option_string(mpv, "dscale", "bilinear");
    mpv_set_option_string(mpv, "vd-lavc-threads", "0");
    mpv_set_option_string(mpv, "keep-open", "yes");
    mpv_set_option_string(mpv, "osd-level", "0");
    mpv_request_log_messages(mpv, "info");

    qDebug() << "initializeMpv: calling mpv_initialize...";
    if (mpv_initialize(mpv) < 0) {
        qCritical() << "initializeMpv: mpv_initialize failed";
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
        return;
    }

    qDebug() << "initializeMpv: mpv initialized, setting up wakeup pipe...";

    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "duration", MPV_FORMAT_DOUBLE);
    mpv_observe_property(mpv, 0, "frame-drop-count", MPV_FORMAT_INT64);

    int fd = mpv_get_wakeup_pipe(mpv);
    qDebug() << "initializeMpv: wakeup pipe fd =" << fd;
    if (fd >= 0) {
        notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
        connect(notifier, &QSocketNotifier::activated, this, &MpvWidget::handle_mpv_events);
    } else {
        qWarning() << "initializeMpv: mpv_get_wakeup_pipe returned" << fd;
    }

    initialized = true;
    qDebug() << "initializeMpv: mpv fully initialized";
}

void MpvWidget::terminateMpv()
{
    if (notifier) {
        delete notifier;
        notifier = nullptr;
    }

    if (mpv_gl) {
        mpv_render_context_set_update_callback(mpv_gl, nullptr, nullptr);
        mpv_render_context_free(mpv_gl);
        mpv_gl = nullptr;
    }

    if (mpv) {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }

    initialized = false;
}

void MpvWidget::loadFile(const QString &file)
{
    if (!initialized) {
        qWarning() << "loadFile: mpv not initialized";
        return;
    }
    qDebug() << "loadFile:" << file;
    QByteArray utf8 = file.toUtf8();
    const char *cmd[] = {"loadfile", utf8.constData(), nullptr};
    int ret = mpv_command_async(mpv, 1, cmd);
    if (ret < 0)
        qWarning() << "mpv_command_async loadFile failed:" << mpv_error_string(ret);
}

void MpvWidget::loadUrl(const QString &url)
{
    if (!initialized) {
        qWarning() << "loadUrl: mpv not initialized";
        return;
    }
    qDebug() << "loadUrl:" << url;
    QByteArray utf8 = url.toUtf8();
    const char *cmd[] = {"loadfile", utf8.constData(), nullptr};
    int ret = mpv_command_async(mpv, 1, cmd);
    if (ret < 0)
        qWarning() << "mpv_command_async loadUrl failed:" << mpv_error_string(ret);
}

void MpvWidget::play()
{
    if (!initialized) return;
    const char *cmd[] = {"cycle", "pause", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::pause()
{
    if (!initialized) return;
    const char *cmd[] = {"set", "pause", "yes", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::stop()
{
    if (!initialized) return;
    const char *cmd[] = {"stop", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::seek(double position)
{
    if (!initialized) return;
    QString pos = QString::number(position);
    const char *cmd[] = {"seek", pos.toUtf8().constData(), "absolute", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setVolume(int volume)
{
    if (!initialized) return;
    QString vol = QString::number(volume);
    const char *cmd[] = {"set", "volume", vol.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setMute(bool mute)
{
    if (!initialized) return;
    const char *cmd[] = {"set", "mute", mute ? "yes" : "no", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSpeed(double speed)
{
    if (!initialized) return;
    QString spd = QString::number(speed);
    const char *cmd[] = {"set", "speed", spd.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setLoop(bool loop)
{
    if (!initialized) return;
    const char *cmd[] = {"set", "loop", loop ? "inf" : "no", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setFullscreen(bool fullscreen)
{
    if (!initialized) return;
    const char *cmd[] = {"set", "fullscreen", fullscreen ? "yes" : "no", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setAspectRatio(const QString &ratio)
{
    if (!initialized) return;
    const char *cmd[] = {"set", "video-aspect", ratio.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::rotate(int degrees)
{
    if (!initialized) return;
    QString rot = QString::number(degrees);
    const char *cmd[] = {"set", "video-rotate", rot.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::screenshot()
{
    if (!initialized) return;
    const char *cmd[] = {"screenshot", nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::loadSubtitles(const QString &file)
{
    if (!initialized) return;
    const char *cmd[] = {"sub-add", file.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSubtitleTrack(int track)
{
    if (!initialized) return;
    QString trk = QString::number(track);
    const char *cmd[] = {"set", "sid", trk.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSubtitleDelay(double delay)
{
    if (!initialized) return;
    QString dly = QString::number(delay);
    const char *cmd[] = {"set", "sub-delay", dly.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSubtitleSize(int size)
{
    if (!initialized) return;
    QString sz = QString::number(size);
    const char *cmd[] = {"set", "sub-font-size", sz.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSubtitleColor(const QColor &color)
{
    if (!initialized) return;
    QString col = QString("#%1%2%3")
                      .arg(color.red(), 2, 16, QChar('0'))
                      .arg(color.green(), 2, 16, QChar('0'))
                      .arg(color.blue(), 2, 16, QChar('0'));
    const char *cmd[] = {"set", "sub-color", col.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

void MpvWidget::setSubtitlePosition(int position)
{
    if (!initialized) return;
    QString pos = QString::number(position);
    const char *cmd[] = {"set", "sub-pos", pos.toUtf8().constData(), nullptr};
    mpv_command_async(mpv, 0, cmd);
}

double MpvWidget::getDuration() const
{
    if (!initialized) return 0.0;
    double duration = 0.0;
    mpv_get_property(mpv, "duration", MPV_FORMAT_DOUBLE, &duration);
    return duration;
}

double MpvWidget::getPosition() const
{
    if (!initialized) return 0.0;
    double position = 0.0;
    mpv_get_property(mpv, "time-pos", MPV_FORMAT_DOUBLE, &position);
    return position;
}

int MpvWidget::getVolume() const
{
    if (!initialized) return 0;
    int64_t volume = 0;
    mpv_get_property(mpv, "volume", MPV_FORMAT_INT64, &volume);
    return static_cast<int>(volume);
}

bool MpvWidget::getMute() const
{
    if (!initialized) return false;
    int mute = 0;
    mpv_get_property(mpv, "mute", MPV_FORMAT_FLAG, &mute);
    return mute != 0;
}

double MpvWidget::getSpeed() const
{
    if (!initialized) return 1.0;
    double speed = 1.0;
    mpv_get_property(mpv, "speed", MPV_FORMAT_DOUBLE, &speed);
    return speed;
}

bool MpvWidget::getLoop() const
{
    if (!initialized) return false;
    char *loop = nullptr;
    mpv_get_property(mpv, "loop", MPV_FORMAT_STRING, &loop);
    bool isLoop = loop && QString(loop) == "inf";
    mpv_free(loop);
    return isLoop;
}

QString MpvWidget::getAspectRatio() const
{
    if (!initialized) return "auto";
    char *aspect = nullptr;
    mpv_get_property(mpv, "video-aspect", MPV_FORMAT_STRING, &aspect);
    QString result = aspect ? QString(aspect) : "auto";
    mpv_free(aspect);
    return result;
}

int MpvWidget::getRotation() const
{
    if (!initialized) return 0;
    int64_t rotation = 0;
    mpv_get_property(mpv, "video-rotate", MPV_FORMAT_INT64, &rotation);
    return static_cast<int>(rotation);
}

int MpvWidget::getSubtitleTrack() const
{
    if (!initialized) return 0;
    int64_t track = 0;
    mpv_get_property(mpv, "sid", MPV_FORMAT_INT64, &track);
    return static_cast<int>(track);
}

double MpvWidget::getSubtitleDelay() const
{
    if (!initialized) return 0.0;
    double delay = 0.0;
    mpv_get_property(mpv, "sub-delay", MPV_FORMAT_DOUBLE, &delay);
    return delay;
}

int MpvWidget::getSubtitleSize() const
{
    if (!initialized) return 100;
    int64_t size = 100;
    mpv_get_property(mpv, "sub-font-size", MPV_FORMAT_INT64, &size);
    return static_cast<int>(size);
}

QColor MpvWidget::getSubtitleColor() const
{
    if (!initialized) return QColor(Qt::white);
    char *color = nullptr;
    mpv_get_property(mpv, "sub-color", MPV_FORMAT_STRING, &color);
    QColor result = color ? QColor(color) : QColor(Qt::white);
    mpv_free(color);
    return result;
}

int MpvWidget::getSubtitlePosition() const
{
    if (!initialized) return 100;
    int64_t position = 100;
    mpv_get_property(mpv, "sub-pos", MPV_FORMAT_INT64, &position);
    return static_cast<int>(position);
}

void MpvWidget::initializeGL()
{
    qDebug() << "initializeGL: setting up OpenGL functions...";
    initializeOpenGLFunctions();

    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *vendor = glGetString(GL_VENDOR);
    qDebug() << "initializeGL: OpenGL vendor =" << (vendor ? reinterpret_cast<const char*>(vendor) : "unknown");
    qDebug() << "initializeGL: OpenGL renderer =" << (renderer ? reinterpret_cast<const char*>(renderer) : "unknown");

    if (!mpv) {
        qWarning() << "initializeGL: mpv not created yet";
        return;
    }
    if (mpv_gl) {
        qDebug() << "initializeGL: render context already exists";
        return;
    }

    qDebug() << "initializeGL: creating mpv render context...";
    mpv_opengl_init_params gl_init_params{get_proc_address_mpv, nullptr};
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_API_TYPE, const_cast<char*>(MPV_RENDER_API_TYPE_OPENGL)},
        {MPV_RENDER_PARAM_OPENGL_INIT_PARAMS, &gl_init_params},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    if (mpv_render_context_create(&mpv_gl, mpv, params) < 0) {
        qCritical() << "initializeGL: mpv_render_context_create failed";
        return;
    }

    mpv_render_context_set_update_callback(mpv_gl, MpvWidget::on_update, this);
    qDebug() << "initializeGL: render context created successfully";
}

void MpvWidget::paintGL()
{
    if (!mpv_gl) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        return;
    }

    mpv_opengl_fbo fbo;
    fbo.fbo = static_cast<int>(defaultFramebufferObject());
    fbo.w = static_cast<int>(width() * devicePixelRatio());
    fbo.h = static_cast<int>(height() * devicePixelRatio());
    fbo.internal_format = 0;

    int flip_y = 1;
    mpv_render_param params[] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &fbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr}
    };

    int ret = mpv_render_context_render(mpv_gl, params);
    if (ret < 0) {
        static bool warned = false;
        if (!warned) {
            qWarning() << "paintGL: mpv_render_context_render failed:" << mpv_error_string(ret);
            warned = true;
        }
    }
}

void MpvWidget::resizeGL(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
}

void MpvWidget::triggerUpdate()
{
    static int frameCount = 0;
    frameCount++;
    update();
}

void MpvWidget::on_update(void *ctx)
{
    MpvWidget *widget = static_cast<MpvWidget*>(ctx);
    QMetaObject::invokeMethod(widget, "triggerUpdate", Qt::QueuedConnection);
}

void MpvWidget::handle_mpv_events()
{
    if (!mpv) return;

    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE) break;

        switch (event->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            mpv_event_property *prop = static_cast<mpv_event_property*>(event->data);
            if (prop->format == MPV_FORMAT_DOUBLE && prop->data) {
                double val = *static_cast<double*>(prop->data);
                if (strcmp(prop->name, "time-pos") == 0)
                    emit positionChanged(val);
                else if (strcmp(prop->name, "duration") == 0)
                    emit durationChanged(val);
            } else if (prop->format == MPV_FORMAT_INT64 && prop->data) {
                int64_t val = *static_cast<int64_t*>(prop->data);
                if (strcmp(prop->name, "frame-drop-count") == 0 && val > 0)
                    qDebug() << "[mpv] frames dropped:" << val;
            }
            break;
        }
        case MPV_EVENT_START_FILE:
            qDebug() << "[mpv] START_FILE";
            emit stateChanged();
            break;
        case MPV_EVENT_FILE_LOADED:
            qDebug() << "[mpv] FILE_LOADED";
            emit durationChanged(getDuration());
            emit fileLoaded();
            emit stateChanged();
            break;
        case MPV_EVENT_PLAYBACK_RESTART:
            qDebug() << "[mpv] PLAYBACK_RESTART";
            emit stateChanged();
            break;
        case MPV_EVENT_END_FILE: {
            mpv_event_end_file *ef = static_cast<mpv_event_end_file*>(event->data);
            qDebug() << "[mpv] END_FILE reason:" << ef->reason;
            if (ef->reason == MPV_END_FILE_REASON_ERROR) {
                emit errorOccurred("Error al reproducir el archivo/URL");
            }
            emit fileClosed();
            emit stateChanged();
            break;
        }
        case MPV_EVENT_COMMAND_REPLY: {
            mpv_event_command *cmd = static_cast<mpv_event_command*>(event->data);
            if (cmd->result.format == MPV_FORMAT_INT64) {
                int64_t err = cmd->result.u.int64;
                qDebug() << "[mpv] COMMAND_REPLY result:" << err;
                if (err < 0) {
                    emit errorOccurred(QString::fromUtf8(mpv_error_string(static_cast<int>(err))));
                }
            }
            break;
        }
        case MPV_EVENT_LOG_MESSAGE: {
            mpv_event_log_message *msg = static_cast<mpv_event_log_message*>(event->data);
            if (msg->log_level >= MPV_LOG_LEVEL_WARN) {
                qWarning() << "[mpv]" << msg->prefix << msg->text;
            }
            break;
        }
        default:
            break;
        }
    }
}