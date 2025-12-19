#pragma once
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidget>
#include <QSlider>
#include <QPushButton>
#include <QSettings>

class QCloseEvent;
class QStackedLayout;
class QStackedWidget;
class QLabel;
class QMovie;
class QVideoWidget;
class QVideoSink;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void addMusic();
    void playPause();
    void stop();
    void setPosition(int);
    void updatePosition(qint64);
    void updateDuration(qint64);
    void chooseBackground();

    void setVolume(int);

    void playSelected();
    void nextTrack();
    void prevTrack();

private:
    enum class BackgroundMode {
        None,
        Image,
        Gif,
        Video,
    };

    void applyBackground(const QString &path);
    void clearBackground();
    void updateGifScale();

    QMediaPlayer *player;
    QAudioOutput *audio;
    QListWidget *list;
    QSlider *slider;
    QPushButton *btnPlay;

    QPushButton *btnPrev;
    QPushButton *btnNext;

    QLabel *lblNowPlaying;
    QLabel *coverLabel;
    QLabel *lblTitle;
    QLabel *lblArtist;
    QLabel *lblTimeCur;
    QLabel *lblTimeDur;

    QLabel *lblVol;
    QSlider *sliderVol;

    QWidget *root;
    QStackedLayout *rootStack;
    QWidget *backgroundLayer;
    QStackedLayout *backgroundStack;
    QWidget *controlsLayer;
    QWidget *controlsPanel;

    QLabel *imageLabel;
    QLabel *gifLabel;
    QMovie *gifMovie;

    QVideoWidget *videoWidget;
    QVideoSink *bgSink;
    QMediaPlayer *bgPlayer;
    QAudioOutput *bgAudio;

    BackgroundMode bgMode;
    QString bgPath;
};
