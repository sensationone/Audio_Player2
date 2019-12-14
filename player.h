#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
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


namespace Ui {
class Player;
}

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

public slots:
    void playOrPause(void);
    void musicFront(void);
    void musicNext(void);
    void btnMute(void);

private:
    Ui::Player *ui;
};

#endif // PLAYER_H
