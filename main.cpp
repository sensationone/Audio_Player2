#include "mainwindow.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <QApplication>

int main(int argc, char *argv[])
{
    unlink("fifo_cmd");
    int ret = mkfifo("fifo_cmd",O_CREAT|0664);
    if(ret < 0)
    {
        perror("fail to mkfifo");
    }
    unlink("fifo_back");
    int ret1 = mkfifo("fifo_back",O_CREAT|0664);
    if(ret1 < 0)
    {
        perror("fail to mkfifo");
    }
    pid_t pid=fork();
    if(pid < 0)
    {
        perror("fail to fork");
    }
    else if (pid == 0)
    {
        int fd = open("fifo_cmd",O_RDWR);
        int fd1 = open("fifo_back",O_RDWR);
        close(0);
        dup2(fd,0);
        close(1);
        dup2(fd1,1);
         execlp("mplayer",
                     " mplayer ",
                     "-slave", "-quiet","-idle",
                     "-input", "file=./fifo_cmd",
                     "../Audio_Player2/Song/Calc.mp3", NULL);
    }
    else
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.pid_player = pid;
        w.show();
        return a.exec();
    }
}
