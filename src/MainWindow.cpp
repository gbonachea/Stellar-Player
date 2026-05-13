#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MpvWidget.h"
#include "PlayerController.h"
#include "PlaylistModel.h"
#include "SubtitleSettingsDialog.h"
#include "SettingsManager.h"
#include "ThemeManager.h"

#include <QApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>
#include <QCloseEvent>
#include <QShowEvent>
#include <QHideEvent>
#include <QEvent>
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
#include <QTimer>
#include <QEventLoop>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>
#include <QReadWriteLock>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui_MainWindow)
    , mpvWidget(nullptr)
    , playerController(nullptr)
    , playlistModel(nullptr)
    , subtitleDialog(nullptr)
    , settingsManager(nullptr)
    , themeManager(nullptr)
    , trayIcon(nullptr)
    , trayMenu(nullptr)
    , controlsTimer(nullptr)
    , controlsAnimation(nullptr)
    , timeLabel(nullptr)
    , durationLabel(nullptr)
    , progressSlider(nullptr)
    , volumeSlider(nullptr)
    , playPauseButton(nullptr)
    , stopButton(nullptr)
    , nextButton(nullptr)
    , previousButton(nullptr)
    , muteButton(nullptr)
    , fullscreenButton(nullptr)
    , controlsWidget(nullptr)
    , titleBar(nullptr)
    , titleLabel(nullptr)
    , minButton(nullptr)
    , maxButton(nullptr)
    , closeButton(nullptr)
    , dragging(false)
    , isFullscreen(false)
    , controlsVisible(false)
    , muted(false)
    , seeking(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);

    setupUI();
    setupTray();
    setupAnimations();
    loadSettings();

    connect(themeManager, &ThemeManager::themeChanged, this, [this]() { this->update(); });
}

MainWindow::~MainWindow()
{
    saveSettings();
    delete ui;
}

void MainWindow::setupUI()
{
    ui->setupUi(this);

    // Create central widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- Title bar ---
    titleBar = new QWidget(this);
    titleBar->setFixedHeight(40);
    titleBar->setStyleSheet("background-color: transparent;");

    QHBoxLayout *titleLayout = new QHBoxLayout(titleBar);
    titleLayout->setContentsMargins(10, 0, 0, 0);

    titleLabel = new QLabel("Stellar Player", this);
    titleLabel->setStyleSheet("color: white; font-size: 14px; font-weight: bold;");
    titleLayout->addWidget(titleLabel);

    QString menuStyle =
        "QPushButton {"
        "  background-color: transparent;"
        "  color: white;"
        "  border: none;"
        "  font-size: 13px;"
        "  padding: 0 10px;"
        "  min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 255, 255, 0.1);"
        "}";

    menuArchivo = new QPushButton("Archivo", this);
    menuArchivo->setStyleSheet(menuStyle);
    connect(menuArchivo, &QPushButton::clicked, this, [this]() { showMenu(menuArchivo, 0); });
    titleLayout->addWidget(menuArchivo);

    menuReproduccion = new QPushButton("Reproducción", this);
    menuReproduccion->setStyleSheet(menuStyle);
    connect(menuReproduccion, &QPushButton::clicked, this, [this]() { showMenu(menuReproduccion, 1); });
    titleLayout->addWidget(menuReproduccion);

    menuSubtitulos = new QPushButton("Subtítulos", this);
    menuSubtitulos->setStyleSheet(menuStyle);
    connect(menuSubtitulos, &QPushButton::clicked, this, [this]() { showMenu(menuSubtitulos, 2); });
    titleLayout->addWidget(menuSubtitulos);

    menuVer = new QPushButton("Ver", this);
    menuVer->setStyleSheet(menuStyle);
    connect(menuVer, &QPushButton::clicked, this, [this]() { showMenu(menuVer, 3); });
    titleLayout->addWidget(menuVer);

    menuAyuda = new QPushButton("Ayuda", this);
    menuAyuda->setStyleSheet(menuStyle);
    connect(menuAyuda, &QPushButton::clicked, this, [this]() { showMenu(menuAyuda, 4); });
    titleLayout->addWidget(menuAyuda);

    titleLayout->addStretch();

    QString tbStyle =
        "QPushButton {"
        "  background-color: transparent;"
        "  color: white;"
        "  border: none;"
        "  font-size: 16px;"
        "  min-width: 40px;"
        "  min-height: 40px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 255, 255, 0.1);"
        "}"
        "QPushButton#closeButton:hover {"
        "  background-color: #e81123;"
        "}";

    minButton = new QPushButton("🗕", this);
    minButton->setObjectName("minButton");
    minButton->setFixedSize(40, 40);
    minButton->setStyleSheet(tbStyle);
    connect(minButton, &QPushButton::clicked, this, &MainWindow::onMinimizeClicked);
    titleLayout->addWidget(minButton);

    maxButton = new QPushButton("🗖", this);
    maxButton->setObjectName("maxButton");
    maxButton->setFixedSize(40, 40);
    maxButton->setStyleSheet(tbStyle);
    connect(maxButton, &QPushButton::clicked, this, &MainWindow::onMaximizeClicked);
    titleLayout->addWidget(maxButton);

    closeButton = new QPushButton("✕", this);
    closeButton->setObjectName("closeButton");
    closeButton->setFixedSize(40, 40);
    closeButton->setStyleSheet(tbStyle);
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseClicked);
    titleLayout->addWidget(closeButton);

    mainLayout->addWidget(titleBar);

    // Background image (shown when no video is playing)
    backgroundLabel = new QLabel(this);
    backgroundLabel->setPixmap(QPixmap(":/icons/StellarPlayerAbout.png"));
    backgroundLabel->setAlignment(Qt::AlignCenter);
    backgroundLabel->setStyleSheet("background-color: #000000;");
    backgroundLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    backgroundLabel->installEventFilter(this);
    mainLayout->addWidget(backgroundLabel, 1);

    // Create MpvWidget
    mpvWidget = new MpvWidget(this);
    mpvWidget->initializeMpv();
    mpvWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(mpvWidget, 1);
    mpvWidget->hide();

    // Create controls widget
    controlsWidget = new QWidget(this);
    controlsWidget->setObjectName("controlsWidget");
    controlsWidget->setFixedHeight(60);
    controlsWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.7); border-radius: 10px;");

    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    controlsLayout->setSpacing(10);

    // Previous button
    previousButton = new QPushButton("⏮", this);
    previousButton->setFixedSize(40, 40);
    connect(previousButton, &QPushButton::clicked, this, &MainWindow::onPreviousClicked);
    controlsLayout->addWidget(previousButton);

    // Play/Pause button
    playPauseButton = new QPushButton("▶", this);
    playPauseButton->setFixedSize(40, 40);
    connect(playPauseButton, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);
    controlsLayout->addWidget(playPauseButton);

    // Stop button
    stopButton = new QPushButton("⏹", this);
    stopButton->setFixedSize(40, 40);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopClicked);
    controlsLayout->addWidget(stopButton);

    // Next button
    nextButton = new QPushButton("⏭", this);
    nextButton->setFixedSize(40, 40);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    controlsLayout->addWidget(nextButton);

    // Time label
    timeLabel = new QLabel("00:00", this);
    timeLabel->setStyleSheet("color: white; font-size: 12px;");
    controlsLayout->addWidget(timeLabel);

    // Progress slider
    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setRange(0, 100);
    connect(progressSlider, &QSlider::sliderPressed, this, [this]() { seeking = true; });
    connect(progressSlider, &QSlider::sliderReleased, this, [this]() { seeking = false; });
    connect(progressSlider, &QSlider::sliderMoved, this, &MainWindow::onSeek);
    controlsLayout->addWidget(progressSlider);

    // Duration label
    durationLabel = new QLabel("00:00", this);
    durationLabel->setStyleSheet("color: white; font-size: 12px;");
    controlsLayout->addWidget(durationLabel);

    // Volume slider
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);
    volumeSlider->setFixedWidth(80);
    connect(volumeSlider, &QSlider::valueChanged, this, &MainWindow::onVolumeChanged);
    controlsLayout->addWidget(volumeSlider);

    // Mute button
    muteButton = new QPushButton("🔊", this);
    muteButton->setFixedSize(40, 40);
    connect(muteButton, &QPushButton::clicked, this, &MainWindow::onMuteClicked);
    controlsLayout->addWidget(muteButton);

    // Fullscreen button
    fullscreenButton = new QPushButton("⛶", this);
    fullscreenButton->setFixedSize(40, 40);
    connect(fullscreenButton, &QPushButton::clicked, this, &MainWindow::onFullscreenClicked);
    controlsLayout->addWidget(fullscreenButton);

    mainLayout->addWidget(controlsWidget);

    // Initialize models and controllers
    playlistModel = new PlaylistModel(this);
    playerController = new PlayerController(mpvWidget, playlistModel, this);
    subtitleDialog = new SubtitleSettingsDialog(this);
    settingsManager = SettingsManager::instance();
    themeManager = ThemeManager::instance();

    // Connect signals
    connect(playerController, &PlayerController::durationChanged, this, &MainWindow::onDurationChanged);
    connect(playerController, &PlayerController::positionChanged, this, &MainWindow::onPositionChanged);
    connect(playerController, &PlayerController::stateChanged, this, &MainWindow::onPlayerStateChanged);
    connect(playerController, &PlayerController::errorOccurred, this, &MainWindow::onPlayerError);
    connect(mpvWidget, &MpvWidget::errorOccurred, this, &MainWindow::onPlayerError);
    connect(mpvWidget, &MpvWidget::fileLoaded, this, [this]() {
        backgroundLabel->hide();
        mpvWidget->show();
    });
    connect(mpvWidget, &MpvWidget::fileClosed, this, [this]() {
        backgroundLabel->show();
        mpvWidget->hide();
    });

    // Status bar: remaining time + system clock
    statusBar()->setStyleSheet(
        "QStatusBar { background-color: #2a2a2a; color: #cccccc; font-size: 12px; padding: 0 8px; }"
        "QStatusBar::item { border: none; }");

    remainingLabel = new QLabel(this);
    remainingLabel->setStyleSheet("color: #cccccc; font-size: 12px; padding: 0 4px;");
    statusBar()->addWidget(remainingLabel);

    statusBar()->addPermanentWidget(new QWidget(this), 1);

    clockLabel = new QLabel(this);
    clockLabel->setStyleSheet("color: #cccccc; font-size: 12px; padding: 0 4px;");
    statusBar()->addPermanentWidget(clockLabel);

    QTimer *clockTimer = new QTimer(this);
    connect(clockTimer, &QTimer::timeout, this, [this]() {
        clockLabel->setText(QDateTime::currentDateTime().toString("HH:mm"));
    });
    clockTimer->start(10000);
    clockLabel->setText(QDateTime::currentDateTime().toString("HH:mm"));

    // Set window properties
    setMinimumSize(800, 600);
    setWindowTitle("Stellar Player");
    setAcceptDrops(true);

    // Initialize controls visibility
    controlsVisible = true;
    controlsTimer = new QTimer(this);
    controlsTimer->setSingleShot(true);
    controlsTimer->setInterval(3000);
    connect(controlsTimer, &QTimer::timeout, this, &MainWindow::onControlsTimeout);

    setMouseTracking(true);
    centralWidget->setMouseTracking(true);
    mpvWidget->setMouseTracking(true);
}

void MainWindow::showMenu(QPushButton *button, int index)
{
    QMenu menu(this);
    menu.setStyleSheet("QMenu { background-color: #2d2d2d; color: white; border: 1px solid #555; }"
                       "QMenu::item:selected { background-color: #404040; }");

    switch (index) {
    case 0: { // Archivo
        QAction *openFile = menu.addAction("Abrir archivo...");
        connect(openFile, &QAction::triggered, this, &MainWindow::onOpenFile);
        menu.addSeparator();
        QAction *exit = menu.addAction("Salir");
        connect(exit, &QAction::triggered, this, &QWidget::close);
        break;
    }
    case 1: { // Reproducción
        QAction *playPause = menu.addAction("Reproducir/Pausar");
        connect(playPause, &QAction::triggered, this, &MainWindow::onPlayPauseClicked);
        QAction *stop = menu.addAction("Detener");
        connect(stop, &QAction::triggered, this, &MainWindow::onStopClicked);
        menu.addSeparator();
        QAction *next = menu.addAction("Siguiente");
        connect(next, &QAction::triggered, this, &MainWindow::onNextClicked);
        QAction *prev = menu.addAction("Anterior");
        connect(prev, &QAction::triggered, this, &MainWindow::onPreviousClicked);
        break;
    }
    case 2: { // Subtítulos
        QAction *loadSubs = menu.addAction("Cargar subtítulos...");
        connect(loadSubs, &QAction::triggered, this, &MainWindow::onLoadSubtitles);
        QAction *subSettings = menu.addAction("Ajustes de subtítulos...");
        connect(subSettings, &QAction::triggered, this, &MainWindow::onSubtitleSettings);
        break;
    }
    case 3: { // Ver
        QAction *fullscreen = menu.addAction("Pantalla completa");
        connect(fullscreen, &QAction::triggered, this, &MainWindow::onFullscreenClicked);
        QAction *alwaysOnTop = menu.addAction("Siempre encima");
        alwaysOnTop->setCheckable(true);
        connect(alwaysOnTop, &QAction::toggled, this, &MainWindow::onAlwaysOnTopToggled);
        break;
    }
    case 4: { // Ayuda
        QAction *about = menu.addAction("Acerca de");
        connect(about, &QAction::triggered, this, &MainWindow::onAbout);
        break;
    }
    }

    QPoint pos = button->mapToGlobal(QPoint(0, button->height()));
    menu.exec(pos);
}

void MainWindow::setupTray()
{
    if (!QSystemTrayIcon::isSystemTrayAvailable()) {
        return;
    }

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/icons/stellarpalyer.png"));
    trayIcon->setToolTip("Stellar Player");

    trayMenu = new QMenu(this);
    QAction *showAction = trayMenu->addAction("Mostrar");
    connect(showAction, &QAction::triggered, this, &QWidget::show);
    QAction *hideAction = trayMenu->addAction("Ocultar");
    connect(hideAction, &QAction::triggered, this, &QWidget::hide);
    trayMenu->addSeparator();
    QAction *quitAction = trayMenu->addAction("Salir");
    connect(quitAction, &QAction::triggered, this, &QWidget::close);

    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::onTrayActivated);

    trayIcon->show();
}

void MainWindow::setupAnimations()
{
    controlsAnimation = new QPropertyAnimation(this);
    controlsAnimation->setDuration(300);
    controlsAnimation->setEasingCurve(QEasingCurve::InOutQuad);
}

void MainWindow::loadSettings()
{
    settingsManager = SettingsManager::instance();
    resize(settingsManager->getWindowSize());
    move(settingsManager->getWindowPosition());
    volumeSlider->setValue(settingsManager->getVolume());
    // Load other settings...
}

void MainWindow::saveSettings()
{
    settingsManager->setWindowSize(size());
    settingsManager->setWindowPosition(pos());
    settingsManager->setVolume(volumeSlider->value());
    // Save other settings...
}

void MainWindow::updateControlsVisibility()
{
    if (isFullscreen) {
        titleBar->setVisible(controlsVisible);
        controlsWidget->setVisible(controlsVisible);
    } else {
        titleBar->setVisible(true);
        controlsWidget->setVisible(true);
    }
}

void MainWindow::updateTimeLabels()
{
    double pos = playerController ? playerController->getPosition() : 0;
    double dur = playerController ? playerController->getDuration() : 0;

    int ph = static_cast<int>(pos) / 3600;
    int pm = (static_cast<int>(pos) % 3600) / 60;
    int ps = static_cast<int>(pos) % 60;

    int dh = static_cast<int>(dur) / 3600;
    int dm = (static_cast<int>(dur) % 3600) / 60;
    int ds = static_cast<int>(dur) % 60;

    if (dh > 0 || ph > 0) {
        timeLabel->setText(QString("%1:%2:%3")
            .arg(ph, 2, 10, QChar('0'))
            .arg(pm, 2, 10, QChar('0'))
            .arg(ps, 2, 10, QChar('0')));
        durationLabel->setText(QString("%1:%2:%3")
            .arg(dh, 2, 10, QChar('0'))
            .arg(dm, 2, 10, QChar('0'))
            .arg(ds, 2, 10, QChar('0')));
    } else {
        timeLabel->setText(QString("%1:%2")
            .arg(pm, 2, 10, QChar('0'))
            .arg(ps, 2, 10, QChar('0')));
        durationLabel->setText(QString("%1:%2")
            .arg(dm, 2, 10, QChar('0'))
            .arg(ds, 2, 10, QChar('0')));
    }
}

void MainWindow::updateWindowTitle()
{
    // Implementation for updating window title
}

// Event handlers
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QList<QUrl> urls = mimeData->urls();
        if (!urls.isEmpty()) {
            QUrl url = urls.first();
            if (url.isLocalFile()) {
                playerController->openFile(url.toLocalFile());
            } else {
                playerController->openUrl(url.toString());
            }
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    if (trayIcon && trayIcon->isVisible()) {
        hide();
        event->ignore();
    } else {
        event->accept();
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (trayIcon) {
        trayIcon->show();
    }
}

void MainWindow::hideEvent(QHideEvent *event)
{
    QMainWindow::hideEvent(event);
    if (trayIcon) {
        trayIcon->hide();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isFullscreen) {
        QWidget *child = childAt(event->position().toPoint());
        if (child && titleBar->isAncestorOf(child)) {
            dragging = true;
            dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
            event->accept();
            return;
        }
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (dragging && !isFullscreen) {
        move(event->globalPosition().toPoint() - dragPosition);
        event->accept();
        return;
    }
    QMainWindow::mouseMoveEvent(event);
    if (isFullscreen) {
        controlsVisible = true;
        updateControlsVisibility();
        controlsTimer->start();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragging = false;
        event->accept();
        return;
    }
    QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !isFullscreen) {
        QWidget *child = childAt(event->position().toPoint());
        if (child == titleBar || child == titleLabel || titleBar->isAncestorOf(child)) {
            onMaximizeClicked();
            event->accept();
            return;
        }
    }
    QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Space:
        onPlayPauseClicked();
        break;
    case Qt::Key_F:
        onFullscreenClicked();
        break;
    case Qt::Key_Escape:
        if (isFullscreen) {
            onFullscreenClicked();
        }
        break;
    case Qt::Key_Left:
        playerController->seek(playerController->getPosition() - 5);
        break;
    case Qt::Key_Right:
        playerController->seek(playerController->getPosition() + 5);
        break;
    case Qt::Key_Up:
        volumeSlider->setValue(volumeSlider->value() + 5);
        break;
    case Qt::Key_Down:
        volumeSlider->setValue(volumeSlider->value() - 5);
        break;
    case Qt::Key_M:
        onMuteClicked();
        break;
    case Qt::Key_O:
        onOpenFile();
        break;
    case Qt::Key_S:
        onLoadSubtitles();
        break;
    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y() > 0) {
        volumeSlider->setValue(volumeSlider->value() + 5);
    } else {
        volumeSlider->setValue(volumeSlider->value() - 5);
    }
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange) {
        if (isMaximized()) {
            maxButton->setText("🗗");
        } else {
            maxButton->setText("🗖");
        }
        update();
    }
    QMainWindow::changeEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    update();
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == backgroundLabel && event->type() == QEvent::Resize) {
        static QPixmap original(":/icons/StellarPlayerAbout.png");
        if (!original.isNull()) {
            QResizeEvent *re = static_cast<QResizeEvent*>(event);
            QSize sz = re->size();
            QPixmap scaled = original.scaled(sz * 0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            backgroundLabel->setPixmap(scaled);
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    // Update layout if necessary
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    painter.setBrush(QBrush(themeManager->getBackgroundColor()));
    painter.setPen(Qt::NoPen);

    if (isMaximized() || isFullscreen) {
        painter.drawRect(rect);
    } else {
        painter.drawRoundedRect(rect, 10, 10);
    }

    QMainWindow::paintEvent(event);
}

// Slot implementations
void MainWindow::onPlayPauseClicked()
{
    playerController->play();
}

void MainWindow::onStopClicked()
{
    playerController->stop();
}

void MainWindow::onNextClicked()
{
    playerController->next();
}

void MainWindow::onPreviousClicked()
{
    playerController->previous();
}

void MainWindow::onSeek(int position)
{
    playerController->seek(static_cast<double>(position));
}

void MainWindow::onVolumeChanged(int volume)
{
    playerController->setVolume(volume);
}

void MainWindow::onMuteClicked()
{
    muted = !muted;
    playerController->setMute(muted);
    muteButton->setText(muted ? "🔇" : "🔊");
}

void MainWindow::onFullscreenClicked()
{
    if (isFullscreen) {
        showNormal();
        isFullscreen = false;
        controlsVisible = true;
        updateControlsVisibility();
        controlsTimer->stop();
    } else {
        showFullScreen();
        isFullscreen = true;
        controlsVisible = true;
        updateControlsVisibility();
        controlsTimer->start();
    }
}

void MainWindow::onOpenFile()
{
    QFileDialog dialog(this, "Abrir archivo de video", QDir::homePath());
    dialog.setNameFilter("Archivos de video (*.mp4 *.avi *.mkv *.mov *.wmv *.flv *.webm);;Archivos de audio (*.mp3 *.wav *.flac *.aac *.ogg);;Todos los archivos (*)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            playerController->openFile(files.first());
        }
    }
}

void MainWindow::onLoadSubtitles()
{
    QFileDialog dialog(this, "Cargar subtítulos", QDir::homePath());
    dialog.setNameFilter("Archivos de subtítulos (*.srt *.ass *.ssa);;Todos los archivos (*)");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setOption(QFileDialog::DontUseNativeDialog);

    if (dialog.exec() == QDialog::Accepted) {
        QStringList files = dialog.selectedFiles();
        if (!files.isEmpty()) {
            playerController->loadSubtitles(files.first());
        }
    }
}

void MainWindow::onSettings()
{
    // Open settings dialog
}

void MainWindow::onAbout()
{
    QDialog dlg(this);
    dlg.setWindowTitle("Acerca de Stellar Player");
    dlg.setFixedSize(420, 360);

    QVBoxLayout *lay = new QVBoxLayout(&dlg);
    lay->setAlignment(Qt::AlignCenter);

    QLabel *img = new QLabel(&dlg);
    img->setPixmap(QPixmap(":/icons/StellarPlayerAbout.png").scaled(380, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    img->setAlignment(Qt::AlignCenter);
    lay->addWidget(img);

    QLabel *txt = new QLabel("Stellar Player v1.0.0\n\nUn reproductor de video moderno.\nConstruido con Qt 6 y libmpv.", &dlg);
    txt->setAlignment(Qt::AlignCenter);
    txt->setStyleSheet("font-size: 13px; color: #ccc;");
    lay->addWidget(txt);

    QPushButton *btn = new QPushButton("Cerrar", &dlg);
    btn->setFixedWidth(100);
    connect(btn, &QPushButton::clicked, &dlg, &QDialog::accept);
    QHBoxLayout *btnLay = new QHBoxLayout();
    btnLay->setAlignment(Qt::AlignCenter);
    btnLay->addWidget(btn);
    lay->addLayout(btnLay);

    dlg.exec();
}

void MainWindow::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick) {
        show();
        activateWindow();
    }
}

void MainWindow::onControlsTimeout()
{
    if (isFullscreen) {
        controlsVisible = false;
        updateControlsVisibility();
    }
}

void MainWindow::onPlayerStateChanged()
{
    statusBar()->showMessage("Reproduciendo...", 3000);
}

void MainWindow::onDurationChanged(double duration)
{
    progressSlider->setRange(0, static_cast<int>(duration));
    updateTimeLabels();
}

void MainWindow::onPositionChanged(double position)
{
    if (!seeking)
        progressSlider->setValue(static_cast<int>(position));
    updateTimeLabels();

    double dur = playerController ? playerController->getDuration() : 0;
    double remain = dur - position;
    if (remain > 0) {
        int rh = static_cast<int>(remain) / 3600;
        int rm = (static_cast<int>(remain) % 3600) / 60;
        int rs = static_cast<int>(remain) % 60;
        if (rh > 0)
            remainingLabel->setText(QString("-%1:%2:%3")
                .arg(rh, 2, 10, QChar('0'))
                .arg(rm, 2, 10, QChar('0'))
                .arg(rs, 2, 10, QChar('0')));
        else
            remainingLabel->setText(QString("-%1:%2")
                .arg(rm, 2, 10, QChar('0'))
                .arg(rs, 2, 10, QChar('0')));
    } else {
        remainingLabel->setText("");
    }
}

void MainWindow::onPlayerError(const QString &error)
{
    statusBar()->showMessage("Error: " + error, 8000);
    qWarning() << "Player error:" << error;
}

// Additional slots for menu actions
void MainWindow::onSubtitleSettings()
{
    subtitleDialog->exec();
}

void MainWindow::onAlwaysOnTopToggled(bool checked)
{
    Qt::WindowFlags flags = windowFlags();
    if (checked) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }
    setWindowFlags(flags);
    show();
}

void MainWindow::onMinimizeClicked()
{
    showMinimized();
}

void MainWindow::onMaximizeClicked()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

void MainWindow::onCloseClicked()
{
    qApp->quit();
}