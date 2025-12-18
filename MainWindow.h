#pragma once
#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidget>
#include <QSlider>
#include <QPushButton>
#include <QSettings>

class QCloseEvent;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void addMusic();
    void playPause();
    void stop();
    void setPosition(int);
    void updatePosition(qint64);
    void updateDuration(qint64);
    void chooseBackground();

private:
    QMediaPlayer *player;
    QAudioOutput *audio;
    QListWidget *list;
    QSlider *slider;
    QPushButton *btnPlay;
};
