#include "PlayerController.h"
#include "MpvWidget.h"
#include "PlaylistModel.h"
#include <QRandomGenerator>

PlayerController::PlayerController(MpvWidget *mpvWidget, PlaylistModel *playlistModel, QObject *parent)
    : QObject(parent)
    , mpvWidget(mpvWidget)
    , playlistModel(playlistModel)
    , currentTrackIndex(-1)
    , repeatMode(false)
    , shuffleMode(false)
{
    connect(mpvWidget, &MpvWidget::durationChanged, this, &PlayerController::onMpvDurationChanged);
    connect(mpvWidget, &MpvWidget::positionChanged, this, &PlayerController::onMpvPositionChanged);
    connect(mpvWidget, &MpvWidget::stateChanged, this, &PlayerController::onMpvStateChanged);
    connect(mpvWidget, &MpvWidget::errorOccurred, this, &PlayerController::onMpvErrorOccurred);
}

PlayerController::~PlayerController()
{
}

void PlayerController::openFile(const QString &filePath)
{
    mpvWidget->loadFile(filePath);
    addToPlaylist(filePath);
    currentTrackIndex = playlistModel->getCount() - 1;
}

void PlayerController::openUrl(const QString &url)
{
    QUrl qurl(url);
    if (qurl.scheme().isEmpty()) {
        mpvWidget->loadUrl("https://" + url);
    } else {
        mpvWidget->loadUrl(url);
    }
}

void PlayerController::play()
{
    mpvWidget->play();
}

void PlayerController::pause()
{
    mpvWidget->pause();
}

void PlayerController::stop()
{
    mpvWidget->stop();
}

void PlayerController::next()
{
    if (playlistModel->getCount() == 0) return;

    if (shuffleMode) {
        // Implement shuffle logic
        currentTrackIndex = QRandomGenerator::global()->bounded(playlistModel->getCount());
    } else {
        currentTrackIndex = (currentTrackIndex + 1) % playlistModel->getCount();
    }

    QString nextFile = playlistModel->getItem(currentTrackIndex);
    mpvWidget->loadFile(nextFile);
}

void PlayerController::previous()
{
    if (playlistModel->getCount() == 0) return;

    currentTrackIndex = (currentTrackIndex - 1 + playlistModel->getCount()) % playlistModel->getCount();
    QString prevFile = playlistModel->getItem(currentTrackIndex);
    mpvWidget->loadFile(prevFile);
}

void PlayerController::seek(double position)
{
    mpvWidget->seek(position);
}

void PlayerController::setVolume(int volume)
{
    mpvWidget->setVolume(volume);
}

void PlayerController::setMute(bool mute)
{
    mpvWidget->setMute(mute);
}

void PlayerController::setSpeed(double speed)
{
    mpvWidget->setSpeed(speed);
}

void PlayerController::setLoop(bool loop)
{
    mpvWidget->setLoop(loop);
}

void PlayerController::setFullscreen(bool fullscreen)
{
    mpvWidget->setFullscreen(fullscreen);
}

void PlayerController::setAspectRatio(const QString &ratio)
{
    mpvWidget->setAspectRatio(ratio);
}

void PlayerController::rotate(int degrees)
{
    mpvWidget->rotate(degrees);
}

void PlayerController::screenshot()
{
    mpvWidget->screenshot();
}

void PlayerController::loadSubtitles(const QString &filePath)
{
    mpvWidget->loadSubtitles(filePath);
}

void PlayerController::setSubtitleTrack(int track)
{
    mpvWidget->setSubtitleTrack(track);
}

void PlayerController::setSubtitleDelay(double delay)
{
    mpvWidget->setSubtitleDelay(delay);
}

void PlayerController::setSubtitleSize(int size)
{
    mpvWidget->setSubtitleSize(size);
}

void PlayerController::setSubtitleColor(const QColor &color)
{
    mpvWidget->setSubtitleColor(color);
}

void PlayerController::setSubtitlePosition(int position)
{
    mpvWidget->setSubtitlePosition(position);
}

double PlayerController::getDuration() const
{
    return mpvWidget->getDuration();
}

double PlayerController::getPosition() const
{
    return mpvWidget->getPosition();
}

int PlayerController::getVolume() const
{
    return mpvWidget->getVolume();
}

bool PlayerController::getMute() const
{
    return mpvWidget->getMute();
}

double PlayerController::getSpeed() const
{
    return mpvWidget->getSpeed();
}

bool PlayerController::getLoop() const
{
    return mpvWidget->getLoop();
}

QString PlayerController::getAspectRatio() const
{
    return mpvWidget->getAspectRatio();
}

int PlayerController::getRotation() const
{
    return mpvWidget->getRotation();
}

int PlayerController::getSubtitleTrack() const
{
    return mpvWidget->getSubtitleTrack();
}

double PlayerController::getSubtitleDelay() const
{
    return mpvWidget->getSubtitleDelay();
}

int PlayerController::getSubtitleSize() const
{
    return mpvWidget->getSubtitleSize();
}

QColor PlayerController::getSubtitleColor() const
{
    return mpvWidget->getSubtitleColor();
}

int PlayerController::getSubtitlePosition() const
{
    return mpvWidget->getSubtitlePosition();
}

void PlayerController::addToPlaylist(const QString &filePath)
{
    playlistModel->addItem(filePath);
    emit playlistChanged();
}

void PlayerController::removeFromPlaylist(int index)
{
    playlistModel->removeItem(index);
    if (index <= currentTrackIndex) {
        currentTrackIndex = qMax(0, currentTrackIndex - 1);
    }
    emit playlistChanged();
}

void PlayerController::clearPlaylist()
{
    playlistModel->clear();
    currentTrackIndex = -1;
    emit playlistChanged();
}

void PlayerController::shufflePlaylist()
{
    // Implement shuffle logic
    emit playlistChanged();
}

void PlayerController::setRepeatMode(bool repeat)
{
    repeatMode = repeat;
}

void PlayerController::setShuffleMode(bool shuffle)
{
    shuffleMode = shuffle;
}

void PlayerController::onMpvDurationChanged(double duration)
{
    emit durationChanged(duration);
}

void PlayerController::onMpvPositionChanged(double position)
{
    emit positionChanged(position);
}

void PlayerController::onMpvStateChanged()
{
    emit stateChanged();
}

void PlayerController::onMpvErrorOccurred(const QString &error)
{
    emit errorOccurred(error);
}