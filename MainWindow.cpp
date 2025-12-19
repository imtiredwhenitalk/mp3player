#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QCloseEvent>

#include <QWidget>
#include <QDir>
#include <QLabel>
#include <QMovie>
#include <QStackedLayout>
#include <QResizeEvent>
#include <QPixmap>
#include <QSizePolicy>
#include <QPalette>
#include <QFileInfo>
#include <QFont>

#include <QVideoWidget>
#include <QVideoSink>
#include <QVideoFrame>

#include <limits>

static QString formatTimeMs(qint64 ms) {
    if (ms < 0) ms = 0;
    const qint64 totalSeconds = ms / 1000;
    const qint64 minutes = totalSeconds / 60;
    const qint64 seconds = totalSeconds % 60;
    return QString::number(minutes) + ":" + QString::number(seconds).rightJustified(2, '0');
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new QMediaPlayer(this);
    audio = new QAudioOutput(this);
    player->setAudioOutput(audio);

    bgPlayer = new QMediaPlayer(this);
    bgAudio = new QAudioOutput(this);
    bgAudio->setVolume(0.0);
    bgPlayer->setAudioOutput(bgAudio);

    bgSink = new QVideoSink(this);
    bgPlayer->setVideoOutput(bgSink);

    bgMode = BackgroundMode::None;
    gifMovie = nullptr;

    list = new QListWidget(this);
    slider = new QSlider(Qt::Horizontal, this);

    auto *btnAdd = new QPushButton("Додати MP3", this);
    btnPrev = new QPushButton("Prev", this);
    btnPlay = new QPushButton("Play / Pause", this);
    btnNext = new QPushButton("Next", this);
    auto *btnStop = new QPushButton("Stop", this);
    auto *btnBg = new QPushButton("Фон", this);

    resize(1600, 900);

    root = new QWidget(this);
    rootStack = new QStackedLayout(root);
    rootStack->setStackingMode(QStackedLayout::StackAll);

    backgroundLayer = new QWidget(root);
    backgroundLayer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    backgroundStack = new QStackedLayout(backgroundLayer);
    backgroundStack->setContentsMargins(0, 0, 0, 0);

    auto *bgEmpty = new QWidget(backgroundLayer);

    imageLabel = new QLabel(backgroundLayer);
    imageLabel->setScaledContents(true);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    gifLabel = new QLabel(backgroundLayer);
    gifLabel->setScaledContents(false);
    gifLabel->setAlignment(Qt::AlignCenter);
    gifLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    videoWidget = new QVideoWidget(backgroundLayer);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(bgSink, &QVideoSink::videoFrameChanged, this, [this](const QVideoFrame &frame) {
        if (!imageLabel) return;
        if (!frame.isValid()) return;
        QVideoFrame copy(frame);
        const QImage img = copy.toImage();
        if (img.isNull()) return;
        imageLabel->setPixmap(QPixmap::fromImage(img));
    });

    backgroundStack->addWidget(bgEmpty);
    backgroundStack->addWidget(imageLabel);
    backgroundStack->addWidget(gifLabel);
    backgroundStack->addWidget(videoWidget);
    backgroundStack->setCurrentWidget(bgEmpty);

    controlsLayer = new QWidget(root);
    controlsLayer->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    controlsLayer->setAttribute(Qt::WA_StyledBackground, true);

    auto *layout = new QVBoxLayout();
    layout->setContentsMargins(16, 16, 16, 16);

    controlsPanel = new QWidget(controlsLayer);
    controlsPanel->setAutoFillBackground(true);
    {
        QPalette pal = controlsPanel->palette();
        QColor panelColor = pal.color(QPalette::Base);
        panelColor.setAlpha(210);
        pal.setColor(QPalette::Window, panelColor);
        controlsPanel->setPalette(pal);
    }

    auto *panelLayout = new QVBoxLayout(controlsPanel);
    panelLayout->setContentsMargins(22, 22, 22, 22);
    panelLayout->setSpacing(14);

    lblNowPlaying = new QLabel("Now Playing", controlsPanel);
    {
        QFont f = lblNowPlaying->font();
        f.setPointSize(f.pointSize() + 4);
        f.setBold(true);
        lblNowPlaying->setFont(f);
    }

    coverLabel = new QLabel(controlsPanel);
    coverLabel->setScaledContents(true);
    coverLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    coverLabel->setMinimumHeight(380);
    coverLabel->setMaximumHeight(520);

    {
        list->setAutoFillBackground(true);
        QPalette pal = list->palette();
        QColor base = pal.color(QPalette::Base);
        base.setAlpha(220);
        pal.setColor(QPalette::Base, base);
        list->setPalette(pal);
    }

    lblTitle = new QLabel("—", controlsPanel);
    {
        QFont f = lblTitle->font();
        f.setPointSize(f.pointSize() + 2);
        f.setBold(true);
        lblTitle->setFont(f);
        lblTitle->setWordWrap(true);
    }
    lblArtist = new QLabel("", controlsPanel);
    lblArtist->setWordWrap(true);

    lblTimeCur = new QLabel("0:00", controlsPanel);
    lblTimeDur = new QLabel("0:00", controlsPanel);

    lblVol = new QLabel("Vol", controlsPanel);
    sliderVol = new QSlider(Qt::Horizontal, controlsPanel);
    sliderVol->setRange(0, 100);
    sliderVol->setMaximumWidth(220);

    auto *timeRow = new QHBoxLayout();
    timeRow->addWidget(lblTimeCur);
    timeRow->addStretch(1);
    timeRow->addWidget(lblTimeDur);

    panelLayout->addWidget(lblNowPlaying);
    panelLayout->addWidget(coverLabel);
    panelLayout->addWidget(lblTitle);
    panelLayout->addWidget(lblArtist);
    panelLayout->addWidget(slider);
    panelLayout->addLayout(timeRow);

    auto *controls = new QHBoxLayout();
    controls->addWidget(btnAdd);
    controls->addWidget(btnPrev);
    controls->addWidget(btnPlay);
    controls->addWidget(btnNext);
    controls->addWidget(btnStop);
    controls->addWidget(btnBg);
    controls->addStretch(1);
    controls->addWidget(lblVol);
    controls->addWidget(sliderVol);
    panelLayout->addLayout(controls);

    list->setMaximumHeight(220);
    panelLayout->addWidget(list);

    layout->addWidget(controlsPanel);
    controlsLayer->setLayout(layout);

    rootStack->addWidget(backgroundLayer);
    rootStack->addWidget(controlsLayer);
    rootStack->setCurrentWidget(controlsLayer);
    setCentralWidget(root);
    setWindowTitle("MP3 Player");

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addMusic);
    connect(btnPrev, &QPushButton::clicked, this, &MainWindow::prevTrack);
    connect(btnPlay, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(btnNext, &QPushButton::clicked, this, &MainWindow::nextTrack);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::stop);
    connect(btnBg, &QPushButton::clicked, this, &MainWindow::chooseBackground);

    connect(list, &QListWidget::itemDoubleClicked, this, [this] { playSelected(); });

    connect(slider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);

    connect(sliderVol, &QSlider::valueChanged, this, &MainWindow::setVolume);

    QSettings s("mp3player", "settings");
    bgPath = s.value("bg/path").toString();
    if (!bgPath.isEmpty()) {
        applyBackground(bgPath);
    }

    const int vol = s.value("audio/volume", 70).toInt();
    sliderVol->setValue(vol);
    setVolume(vol);
}

void MainWindow::addMusic() {
    const QStringList files = QFileDialog::getOpenFileNames(this, "MP3", "", "Audio (*.mp3 *.wav *.flac *.ogg *.m4a)");
    if (files.isEmpty()) return;

    for (const auto &f : files) {
        list->addItem(f);
    }

    if (list->currentRow() < 0 && list->count() > 0) {
        list->setCurrentRow(0);
    }
}

void MainWindow::playPause() {
    if (player->playbackState() == QMediaPlayer::PlayingState)
        player->pause();
    else
        player->play();
}

void MainWindow::stop() {
    player->stop();
}

void MainWindow::setPosition(int p) {
    player->setPosition(static_cast<qint64>(p));
}

void MainWindow::updatePosition(qint64 p) {
    const qint64 maxInt = static_cast<qint64>(std::numeric_limits<int>::max());
    slider->setValue(p > maxInt ? std::numeric_limits<int>::max() : static_cast<int>(p));
    if (lblTimeCur) lblTimeCur->setText(formatTimeMs(p));
}

void MainWindow::updateDuration(qint64 d) {
    const qint64 maxInt = static_cast<qint64>(std::numeric_limits<int>::max());
    slider->setRange(0, d > maxInt ? std::numeric_limits<int>::max() : static_cast<int>(d));
    if (lblTimeDur) lblTimeDur->setText(formatTimeMs(d));
}

void MainWindow::chooseBackground() {
    const QString path = QFileDialog::getOpenFileName(
        this,
        "Фон",
        "",
        "Background (*.png *.jpg *.jpeg *.bmp *.webp *.gif *.mp4 *.webm *.avi *.mov *.mkv)");
    if (path.isEmpty()) return;

    bgPath = path;
    applyBackground(bgPath);

    QSettings s("mp3player", "settings");
    s.setValue("bg/path", bgPath);
}

void MainWindow::playSelected() {
    const int row = list->currentRow();
    if (row < 0) return;

    const auto *item = list->item(row);
    if (!item) return;
    const QString path = item->text();
    if (path.isEmpty()) return;
    const QFileInfo info(path);
    if (lblTitle) lblTitle->setText(info.completeBaseName());
    if (lblArtist) lblArtist->setText(" ");

    player->setSource(QUrl::fromLocalFile(path));
    player->play();
}

void MainWindow::nextTrack() {
    const int count = list->count();
    if (count <= 0) return;
    int row = list->currentRow();
    row = (row < 0) ? 0 : (row + 1) % count;
    list->setCurrentRow(row);
    playSelected();
}

void MainWindow::prevTrack() {
    const int count = list->count();
    if (count <= 0) return;
    int row = list->currentRow();
    row = (row < 0) ? 0 : (row - 1 + count) % count;
    list->setCurrentRow(row);
    playSelected();
}

void MainWindow::applyBackground(const QString &path) {
    clearBackground();

    const QString lower = path.toLower();
    if (lower.endsWith(".gif")) {
        bgMode = BackgroundMode::Gif;
        gifMovie = new QMovie(path, QByteArray(), gifLabel);
        gifMovie->setCacheMode(QMovie::CacheAll);
        gifLabel->setMovie(gifMovie);
        backgroundStack->setCurrentWidget(gifLabel);
        updateGifScale();
        gifMovie->start();
        if (rootStack && controlsLayer) rootStack->setCurrentWidget(controlsLayer);
        if (controlsLayer) controlsLayer->raise();
        if (controlsPanel) controlsPanel->raise();
        return;
    }

    if (lower.endsWith(".mp4") || lower.endsWith(".webm") || lower.endsWith(".avi") || lower.endsWith(".mov") ||
        lower.endsWith(".mkv")) {
        bgMode = BackgroundMode::Video;
        // Render video frames into imageLabel via QVideoSink so UI can overlay reliably.
        backgroundStack->setCurrentWidget(imageLabel);
        bgPlayer->setSource(QUrl::fromLocalFile(path));

#if QT_VERSION >= QT_VERSION_CHECK(6, 8, 0)
        bgPlayer->setLoops(QMediaPlayer::Infinite);
#else
        disconnect(bgPlayer, &QMediaPlayer::mediaStatusChanged, this, nullptr);
        connect(bgPlayer,
                &QMediaPlayer::mediaStatusChanged,
                this,
                [this](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::EndOfMedia) {
                        bgPlayer->setPosition(0);
                        bgPlayer->play();
                    }
                },
                Qt::UniqueConnection);
#endif
        bgPlayer->play();
        if (rootStack && controlsLayer) rootStack->setCurrentWidget(controlsLayer);
        if (controlsLayer) controlsLayer->raise();
        if (controlsPanel) controlsPanel->raise();
        return;
    }

    bgMode = BackgroundMode::Image;
    imageLabel->setPixmap(QPixmap(path));
    backgroundStack->setCurrentWidget(imageLabel);
    if (rootStack && controlsLayer) rootStack->setCurrentWidget(controlsLayer);
    if (controlsLayer) controlsLayer->raise();
    if (controlsPanel) controlsPanel->raise();
}

void MainWindow::clearBackground() {
    if (gifMovie) {
        gifMovie->stop();
        gifMovie->deleteLater();
        gifMovie = nullptr;
        gifLabel->setMovie(nullptr);
    }

    bgPlayer->stop();
    imageLabel->setPixmap(QPixmap());
    backgroundStack->setCurrentIndex(0);
    bgMode = BackgroundMode::None;
    if (rootStack && controlsLayer) rootStack->setCurrentWidget(controlsLayer);
    if (controlsLayer) controlsLayer->raise();
    if (controlsPanel) controlsPanel->raise();
}

void MainWindow::updateGifScale() {
    if (!gifMovie) return;
    if (!gifLabel) return;
    gifMovie->setScaledSize(gifLabel->size());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateGifScale();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);
}

void MainWindow::setVolume(int v) {
    if (v < 0) v = 0;
    if (v > 100) v = 100;
    audio->setVolume(static_cast<float>(v) / 100.0f);
    QSettings s("mp3player", "settings");
    s.setValue("audio/volume", v);
}