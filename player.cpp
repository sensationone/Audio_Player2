#include "player.h"
#include "ui_player.h"

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    ui->volumeslider->setSliderPosition(20);
    ui->volumeslider->setValue(20);
    /**********************************************************/
    //set background picture
    this->setAutoFillBackground(true);
    QPalette palette = this->palette();
    palette.setBrush(QPalette::Window,QBrush(QPixmap(":/Bg/4.jpg").scaled(this->size(),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    // 使用平滑的缩放方式
    this->setPalette(palette);
    //open the fifo_cmd at read-only
    fd = open("fifo_cmd",O_RDWR);
    if(fd < 0){
        perror("open wronly fifo");
    }

    //connect items at Slots
    connect(ui->btn_playpause,SIGNAL(clicked()),this,SLOT(playOrPause()));//
    connect(ui->btn_front,SIGNAL(clicked()),this,SLOT(musicFront()));
    connect(ui->btn_next,SIGNAL(clicked()),this,SLOT(musicNext()));
    connect(ui->btn_mute,SIGNAL(clicked()),this,SLOT(btnMute()));
    connect(ui->listWidget,&QListWidget::doubleClicked,[=]{
        char buff[128]= "loadfile ../Audio_Player2/Song/";
        QByteArray ba = ui->listWidget->currentItem()->text().toUtf8();
        strcpy(buf,ba.data());
        strcat(buff,buf);
        strcat(buff,"\n");
        write(fd,buff,strlen(buff));
        printf("%s\n",buff);
        fflush(stdout);
    });

    getItemOfSong();
    ui->listWidget->setCurrentRow(ui->listWidget->count());
}

Player::~Player()
{
    delete ui;
}

void Player::getItemOfSong()
{
    DIR *dir = opendir("../Audio_Player2/Song");
    int i = 0;
    ui->listWidget->clear();
    while (1) {
        struct dirent* dirp = readdir(dir);

        if(dirp == NULL){
            break;
        }
        else if(dirp->d_type == DT_REG){
            ui->listWidget->addItem(new QListWidgetItem(dirp->d_name));
            strcpy(buf,dirp->d_name);
            songname.append(buf);
        }
        i++;
    }
    closedir(dir);
}



void Player::playOrPause()
{
    write(fd,"pause\n",strlen("pause\n"));
}

void Player::musicFront()
{
    if(ui->listWidget->currentRow() == 0)//当光标在第一个文件时，点击上一个光标移动到最下面的文件，不播放
    {
        ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
    }
    else
    {
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()-1);
    }
    char buff[128]= "loadfile ../Audio_Player2/Song/";
    QByteArray ba = ui->listWidget->currentItem()->text().toUtf8();
    strcpy(buf,ba.data());
    strcat(buff,buf);
    strcat(buff,"\n");
    write(fd,buff,strlen(buff));
    printf("%s\n",buff);
    fflush(stdout);
}

void Player::musicNext()
{
    if(ui->listWidget->currentRow() == ui->listWidget->count()-1)
    {
        ui->listWidget->setCurrentRow(0);//当光标在最后一个文件时，点击下一个，光标移动到第一个，不播放
    }
    else
    {
        ui->listWidget->setCurrentRow(ui->listWidget->currentRow()+1);
    }
    char buff[128]= "loadfile ../Audio_Player2/Song/";
    QByteArray ba = ui->listWidget->currentItem()->text().toUtf8();
    strcpy(buf,ba.data());
    strcat(buff,buf);
    strcat(buff,"\n");
    write(fd,buff,strlen(buff));
    printf("%s\n",buff);
    fflush(stdout);
}

void Player::VolumeSlider(int position)
{
    //write(fd,QString("volume " + QString::number(position) +" 2\n").toUtf8(),strlen(QString("volume " + QString::number(position) +" 2\n").toUtf8()));
}

void Player::btnMute()
{
    int flag_mute = 1;
    if(flag_mute == 1)
    {
        write(fd,"mute 0\n",strlen("mute 0\n"));
        flag_mute = 0;
    }
    else if(flag_mute == 0)
    {
        write(fd,"mute 1\n",strlen("mute 1\n"));
        qDebug();
        flag_mute = 1;
    }
}
