#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <dirent.h>
#include <QTimer>
#include <stdlib.h>
#include <QByteArray>
#include <QDebug>
#include <QVector>
#include <QMutex>
#include <QList>
#include <QPainter>

struct getCurrentInfo
{
    QString Song;
    QString Singer;
    QString Album;
    QString CurrentTime;
    QString TotalTime;
    QString lyric;
    int progress;
    int currentTime;
    int totalTime;
};


namespace Ui {
class Player;
}

//extern void *getTimeMsg(void *arg);
//extern void sendMsgToPlayer(char *val);
//extern void *MySendMsgToMplayer(void *arg);

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

    char buf[128];
    int fd;
    void getItemOfSong();
    QVector<QString>songname;
    QVector<QString>::iterator iter;
    getCurrentInfo getInfo;
    void infoPrinter(void);
    QVector<QString>lrcStr;
    QVector<int>lrcRow;
    QVector<int>lrcTime;
    int imageRotate;
    QPixmap disc;
    int playflag = 0;



public slots:
    void playOrPause(void);
    void musicFront(void);
    void musicNext(void);
    void btnMute(void);;
//    void SetTimeQstring(float val,QString &val1);
    void cutLrc(void);
    void printLrc(void);
    void paintEvent(QPaintEvent *);

private:
    Ui::Player *ui;
};

#endif // PLAYER_H
