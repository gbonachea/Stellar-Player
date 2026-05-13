#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QFileDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QPoint>
#include <QRect>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
class Ui_MainWindow;
QT_END_NAMESPACE

class MpvWidget;
class PlayerController;
class PlaylistModel;
class SubtitleSettingsDialog;
class SettingsManager;
class ThemeManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void moveEvent(QMoveEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onPlayPauseClicked();
    void onStopClicked();
    void onNextClicked();
    void onPreviousClicked();
    void onSeek(int position);
    void onVolumeChanged(int volume);
    void onMuteClicked();
    void onFullscreenClicked();
    void onOpenFile();
    void onLoadSubtitles();
    void onSettings();
    void onAbout();
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void onControlsTimeout();
    void onPlayerStateChanged();
    void onDurationChanged(double duration);
    void onPositionChanged(double position);
    void onPlayerError(const QString &error);
    void onSubtitleSettings();
    void onAlwaysOnTopToggled(bool checked);
    void onMinimizeClicked();
    void onMaximizeClicked();
    void onCloseClicked();

private:
    void setupUI();
    void showMenu(QPushButton *button, int index);
    void setupTray();
    void setupAnimations();
    void loadSettings();
    void saveSettings();
    void updateControlsVisibility();
    void updateTimeLabels();
    void updateWindowTitle();

    Ui_MainWindow *ui;
    MpvWidget *mpvWidget;
    QLabel *backgroundLabel;
    PlayerController *playerController;
    PlaylistModel *playlistModel;
    SubtitleSettingsDialog *subtitleDialog;
    SettingsManager *settingsManager;
    ThemeManager *themeManager;

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;

    QTimer *controlsTimer;
    QPropertyAnimation *controlsAnimation;

    QLabel *timeLabel;
    QLabel *durationLabel;
    QLabel *remainingLabel;
    QLabel *clockLabel;
    QSlider *progressSlider;
    QSlider *volumeSlider;
    QPushButton *playPauseButton;
    QPushButton *stopButton;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QPushButton *muteButton;
    QPushButton *fullscreenButton;
    QWidget *controlsWidget;

    // Title bar
    QWidget *titleBar;
    QLabel *titleLabel;
    QPushButton *menuArchivo;
    QPushButton *menuReproduccion;
    QPushButton *menuSubtitulos;
    QPushButton *menuVer;
    QPushButton *menuAyuda;
    QPushButton *minButton;
    QPushButton *maxButton;
    QPushButton *closeButton;
    QPoint dragPosition;
    bool dragging;

    bool isFullscreen;
    bool controlsVisible;
    bool muted;
    bool seeking;
};

#endif // MAINWINDOW_H