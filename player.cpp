#include "player.h"
#include "ui_player.h"

QMutex mutex;

Player::Player(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Player)
{
    ui->setupUi(this);
    ui->volumeslider->setSliderPosition(50);
    ui->volumeslider->setValue(50);
    ui->spinBox_volume->setValue(99);
    ui->spinBox_volume->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->spinBox_volume->setReadOnly(1);

    QListWidgetItem *item = new QListWidgetItem;
    QWidget *wgt = new QWidget;
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,wgt);
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
        char buff[128]= "loadfile ../Audio_Player/Song/";
        QByteArray ba = ui->listWidget->currentItem()->text().toUtf8();
        strcpy(buf,ba.data());
        strcat(buff,buf);
        strcat(buff,"\n");
        write(fd,buff,strlen(buff));
        printf("%s\n",buff);
        fflush(stdout);
    });

    connect(ui->volumeslider, &QSlider::valueChanged, ui->spinBox_volume, &QSpinBox::setValue);
        connect(ui->volumeslider,&QSlider::valueChanged,[=]{
            int val = ui->spinBox_volume->value();
            char buff1[128] ="volume ";
            char a[20];
            sprintf(a,"%d",val);
            strcat(buff1,a);
            strcat(buff1," 1");
            strcat(buff1,"\n");
            write(fd,buff1,strlen(buff1));

        });

    getItemOfSong();
    ui->listWidget->setCurrentRow(6);
    infoPrinter();
}

Player::~Player()
{
    delete ui;
}

void Player::getItemOfSong()
{
    DIR *dir = opendir("../Audio_Player/Song");
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
    char buff[128]= "loadfile ../Audio_Player/Song/";
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
    char buff[128]= "loadfile ../Audio_Player/Song/";
    QByteArray ba = ui->listWidget->currentItem()->text().toUtf8();
    strcpy(buf,ba.data());
    strcat(buff,buf);
    strcat(buff,"\n");
    write(fd,buff,strlen(buff));
    printf("%s\n",buff);
    fflush(stdout);
    update();
}


void Player::btnMute()
{
    int flag_mute = 1;
    char buf_mute[128]="";
    if(flag_mute == 1)
    {
        sprintf(buf_mute,"mute 0\n");
        write(fd,buf_mute,strlen(buf_mute));
        flag_mute = 0;
    }
    else if(flag_mute == 0)
    {
        sprintf(buf_mute,"mute 1\n");
        write(fd,buf_mute,strlen(buf_mute));
        qDebug() << buf_mute <<endl;
        flag_mute = 1;
    }
}

void Player::SetTimeQstring(float val,QString &val1)
{
    char buff[128] = {0};
    int i = val;

    int minute = i/60;
    int second = i%60;
    //int msec = ((int)(val*10))%10;
    sprintf(buff,"%02d:%02d",minute,second);
    val1 = QString(buff);
}

void Player::infoPrinter()
{
    ui->horizontalSlider->setMaximum(1000);
    ui->l_album->setText(getInfo.Album);
    ui->l_name->setText(ui->listWidget->currentItem()->text().toUtf8());
    ui->l_artist->setText(getInfo.Singer);
    ui->l_totaltime->setText(getInfo.TotalTime);
    ui->l_nowtime->setText(getInfo.CurrentTime);
    ui->horizontalSlider->setValue(1000*getInfo.progress);
}

void *getTimeMsg(void *arg)
{
    int fd2 = open("fifo_back",O_RDONLY);
    char val[128] = "";
    char buf[128] = "";
    float val1;
    char cmd[128] = "";
    Player *m = (Player *)arg;
    while(1)
    {
        bzero(val,sizeof(val));
        bzero(buf,sizeof (buf));
        bzero(cmd,sizeof(cmd));
        val1 =0;
        read(fd2,buf,sizeof (buf));

        sscanf(buf,"%[^=]",cmd);
        if(strcmp(cmd,"ANS_PERCENT_POSITION") == 0)//百分比
        {
            int percent_pos = 0;
            sscanf(buf,"%*[^=]=%d",&percent_pos);
            m->getInfo.progress = percent_pos;

        }
        else if(strcmp(cmd,"ANS_TIME_POSITION") == 0)//当前时间
        {
            float time_pos = 0;
            sscanf(buf,"%*[^=]=%f", &time_pos);

            m->SetTimeQstring(time_pos,m->getInfo.CurrentTime);
            m->getInfo.currentTime = time_pos;
        }
        else if(strcmp(cmd,"ANS_META_ALBUM") == 0)
        {

            sscanf(buf,"%*[^=]='%s'",val);

            m->getInfo.Album = val;
        }
        else if(strcmp(cmd,"ANS_META_ARTIST") == 0)
        {

            sscanf(buf,"%*[^=]='%s'",val);

            m->getInfo.Singer = val;
        }
        else if(strcmp(cmd,"ANS_LENGTH") == 0)
        {

            sscanf(buf,"%*[^=]=%f",&val1);

            m->SetTimeQstring(val1,m->getInfo.TotalTime);
            m->getInfo.totalTime = val1;
        }
        else
        {
        }
        fflush(stdout);
    }
}

void sendMsgToPlayer(char *val)
{
    int fd = open("fifo_cmd",O_RDWR);
    write(fd,val,strlen(val));
    close(fd);
}

void *MySendMsgToMplayer(void *arg)
{


    usleep(200*10000+800000);
    //不停的给fifo_cmd发送获取当前时间以及进度
    while(1)
    {
        mutex.lock();
        usleep(500*100);//0.05秒发指令
        sendMsgToPlayer("get_percent_pos\n");
        usleep(500*100);
        sendMsgToPlayer("get_time_pos\n");
        usleep(500*100);//0.05秒发指令
        sendMsgToPlayer("get_time_length\n");
        usleep(500*100);//0.05秒发指令
        sendMsgToPlayer("get_meta_artist\n");
        mutex.unlock();
        usleep(500*100);//0.05秒发指令
    }
}
