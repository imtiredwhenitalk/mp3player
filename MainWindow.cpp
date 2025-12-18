#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QUrl>
#include <QCloseEvent>

#include <QWidget>
#include <QDir>

#include <limits>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    player = new QMediaPlayer(this);
    audio = new QAudioOutput(this);
    player->setAudioOutput(audio);

    list = new QListWidget(this);
    slider = new QSlider(Qt::Horizontal, this);

    // Let the window background image show through.
    list->setStyleSheet("background: transparent;");

    auto *btnAdd = new QPushButton("Додати MP3", this);
    btnPlay = new QPushButton("Play / Pause", this);
    auto *btnStop = new QPushButton("Stop", this);
    auto *btnBg = new QPushButton("Фон", this);

    auto *layout = new QVBoxLayout();
    layout->addWidget(list);
    layout->addWidget(slider);

    auto *controls = new QHBoxLayout();
    controls->addWidget(btnAdd);
    controls->addWidget(btnPlay);
    controls->addWidget(btnStop);
    controls->addWidget(btnBg);

    layout->addLayout(controls);

    auto *w = new QWidget(this);
    w->setLayout(layout);
    setCentralWidget(w);
    setWindowTitle("MP3 Player");

    w->setAttribute(Qt::WA_StyledBackground, true);

    connect(btnAdd, &QPushButton::clicked, this, &MainWindow::addMusic);
    connect(btnPlay, &QPushButton::clicked, this, &MainWindow::playPause);
    connect(btnStop, &QPushButton::clicked, this, &MainWindow::stop);
    connect(btnBg, &QPushButton::clicked, this, &MainWindow::chooseBackground);

    connect(slider, &QSlider::sliderMoved, this, &MainWindow::setPosition);
    connect(player, &QMediaPlayer::positionChanged, this, &MainWindow::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &MainWindow::updateDuration);

    QSettings s("mp3player", "settings");
    QString bg = s.value("bg").toString();
    if (!bg.isEmpty()) {
        const QString bgPath = QDir::fromNativeSeparators(bg);
        if (auto *cw = centralWidget()) {
            cw->setStyleSheet(QString(
                "QWidget{background-image:url(\"%1\");background-position:center;background-repeat:no-repeat;}")
                                    .arg(bgPath));
        }
    }
}

void MainWindow::addMusic() {
    QString f = QFileDialog::getOpenFileName(this, "MP3", "", "MP3 (*.mp3)");
    if (f.isEmpty()) return;
    list->addItem(f);
    player->setSource(QUrl::fromLocalFile(f));
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
}

void MainWindow::updateDuration(qint64 d) {
    const qint64 maxInt = static_cast<qint64>(std::numeric_limits<int>::max());
    slider->setRange(0, d > maxInt ? std::numeric_limits<int>::max() : static_cast<int>(d));
}

void MainWindow::chooseBackground() {
    QString img = QFileDialog::getOpenFileName(this, "Фон", "", "Images (*.png *.jpg *.jpeg *.bmp *.webp)");
    if (img.isEmpty()) return;
    QSettings s("mp3player", "settings");
    s.setValue("bg", img);
    const QString imgPath = QDir::fromNativeSeparators(img);
    if (auto *cw = centralWidget()) {
        cw->setStyleSheet(QString(
            "QWidget{background-image:url(\"%1\");background-position:center;background-repeat:no-repeat;}")
                                .arg(imgPath));
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    QMainWindow::closeEvent(event);
}