#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "QDebug"
#include <QDate>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include "QTimer"
#include "types.h"

using namespace std;

static NET_DVR_USER_LOGIN_INFO user_login_infor = {0};
static NET_DVR_DEVICEINFO_V40 device_info = {0};
bool connect_btn_flag = false;
LONG user_id;
LONG lRealPlayHandle  = -1;


const QString m_red_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px; border:1px solid red;background:red";
const QString m_green_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid green;background:green";
const QString m_grey_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid gray;background:grey";
const QString m_yellow_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px;  border:1px solid yellow;background:yellow";

/**
 * @brief 
 * 
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*设置界面风格为暗黑*/
    qApp->setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(15, 15, 15));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::white);

    ui->DebugTextBrowser->setStyleSheet("background:while");

    qApp->setPalette(palette);
    Sleep(10000);

    /*速度设定定时器*/
    speed_set_timer = new QTimer(this);
    connect(speed_set_timer,&QTimer::timeout,this,&MainWindow::PlatformVelSet);
    speed_set_timer->start(20);

    /*命令传输定时器*/
    cmd_transimit =  new QTimer(this);
    connect(cmd_transimit,&QTimer::timeout,this,&MainWindow::TransimitCmdByUdp);
    cmd_transimit->start(50);

    udp_socket = new QUdpSocket;
    qDebug()<< "QHostAddress::Any"<<QHostAddress::Any;
    udp_socket->bind(QHostAddress::Any,7788); //设置upd监听端口
    connect(udp_socket,SIGNAL(readyRead()),this,SLOT(UdpRxHandle()));

//    /*udp 定时器*/
//    udp_timer = new QTimer(this);
//    connect(udp_timer,&QTimer::timeout,this,&MainWindow::ScanfUdplPort);
//    udp_timer->start(1000);

    ui->camear_status->setStyleSheet(m_red_SheetStyle);
    ui->platform_online_status->setStyleSheet(m_red_SheetStyle);
}

/**
 * @brief Destroy the Main Window:: Main Window object
 * 
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief 
 * 
 */
void MainWindow::on_connection_btn_pressed()
{


}

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 * @return unsigned char 
 */
unsigned char MainWindow::UdpMsgChecksum(char *data, unsigned char len )
{
    unsigned char checksum = 0x00;

    for( unsigned char i = 0; i < ( len - 1 ); i++ )
    {
        checksum += data[i];
    }

    return checksum & 0xff;
}

/**
 * @brief 
 * 
 * @param s_frame 
 */
void MainWindow::TransMakePKG(SerialCtrlFrame_t s_frame)
{
   unsigned char check_sum = 0x00;

   check_sum = UdpMsgChecksum(s_frame.row, sizeof(s_frame.frame));
   s_frame.frame.check_sum = check_sum;

   //"192.168.31.255",7780
   //("119.29.174.98"),8801
   udp_socket->writeDatagram(s_frame.row,sizeof(s_frame.frame),QHostAddress("119.29.174.98"),8801);
}

/**
 * @brief 
 * 
 */
void MainWindow::CtrlCmdToPlatform(void)
{
    SerialCtrlFrame_t ctrl_frame;
    ctrl_frame.frame.sof = SERIAL_SOF;
    ctrl_frame.frame.lens = 0x07;
    ctrl_frame.frame.cmd_types = eSerialCtrCmd;
    ctrl_frame.frame.ctrl_id = eSerialCtrl_VelCMD;
    ctrl_frame.frame.data[0] = cmd.cmd_mode;
    ctrl_frame.frame.data[1] = cmd.linear;
    ctrl_frame.frame.data[2] = cmd.angular;
    TransMakePKG(ctrl_frame);
}

/**
 * @brief 
 * 
 */
void MainWindow::CtrlBroadcastToPlatform(void)
{
    static qint32 cnt = 0;

    SerialCtrlFrame_t ctrl_frame;
    ctrl_frame.frame.sof = SERIAL_SOF;
    ctrl_frame.frame.lens = 0x07;
    ctrl_frame.frame.cmd_types = eSerialCtrCmd;
    ctrl_frame.frame.ctrl_id = eSerialCtrl_BroadcastCMD;
    ctrl_frame.frame.data[0] = 0x00;
    ctrl_frame.frame.data[1] = 0x00;
    ctrl_frame.frame.data[2] = 0x00;
    TransMakePKG(ctrl_frame);

    if(is_get_robot_platform_udp_response == false)
    {

        if(cnt % 2 == 0)
        {
            ui->platform_online_status->setStyleSheet(m_red_SheetStyle);
        }
        else if (cnt % 2 == 1)
        {
            ui->platform_online_status->setStyleSheet(m_green_SheetStyle);
        }
        cnt ++;
    }
    qDebug()<<"Client had send the Broadcast cmd";
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::Onconnection_btnpressed()
{  
    NET_DVR_USER_LOGIN_INFO user_login_infor;

    strcpy((char *)user_login_infor.sDeviceAddress, ui->sets_ip->text().toUtf8().data()); //设备IP地址
    strcpy((char *)user_login_infor.sUserName, ui->user_name->text().toUtf8().data());    //设备登录用户名
    strcpy((char *)user_login_infor.sPassword, ui->passward->text().toUtf8().data());     //设备登录密码
    user_login_infor.wPort = ui->sets_ip_ports->text().toShort();
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_connection_btn_released()
{
    //NET_DVR_PTZControlWithSpeed(lRealPlayHandle,PAN_RIGHT,1,0);
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_left_pressed()
{
    unsigned int error_code;
    error_code = NET_DVR_PTZControlWithSpeed(lRealPlayHandle, PAN_LEFT, 0, ui->pan_speed_set->value());

    qDebug() << "error code is" << error_code;
}
/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_left_released()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, PAN_LEFT, 1, 0);
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_right_pressed()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, PAN_RIGHT, 0, ui->pan_speed_set->value());
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_right_released()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, PAN_LEFT, 1, 0);
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_up_pressed()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, TILT_UP, 0, ui->pan_speed_set->value());
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_up_released()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, TILT_UP, 1, 0);
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_dowm_pressed()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, TILT_DOWN, 0, ui->pan_speed_set->value());
}

/**
 * @brief 
 * 
 * @return ** void 
 */
void MainWindow::on_pan_dowm_released()
{
    NET_DVR_PTZControlWithSpeed(lRealPlayHandle, TILT_DOWN, 1, 0);
}

void MainWindow::on_radioButton_pressed()
{

}

/**
 * @brief 
 * 
 * @param checked 
 */
void MainWindow::on_open_preview_clicked(bool checked)
{
    NET_DVR_Init(); //设置连接时间

    NET_DVR_SetConnectTime(2000, 1);
    NET_DVR_SetReconnect(10000, true);

    strcpy((char *)user_login_infor.sDeviceAddress, ui->sets_ip->text().toUtf8().data()); //设备IP地址
    strcpy((char *)user_login_infor.sUserName, ui->user_name->text().toUtf8().data());    //设备登录用户名
    strcpy((char *)user_login_infor.sPassword, ui->passward->text().toUtf8().data());     //设备登录密码
    user_login_infor.wPort = ui->sets_ip_ports->text().toShort();
    user_login_infor.bUseAsynLogin = 0; //同步登录，登录接口返回成功即登录成功

    if(user_login_infor.sDeviceAddress == NULL)
    {
        qDebug() << "Please input the device IP address";
        return;
    }

    if(user_login_infor.sUserName == NULL)
    {
        qDebug() <<"Please input the user name";
        return;
    }

    if(user_login_infor.sPassword == NULL)
    {
         qDebug() <<"Please input the password";
         return;
    }

    if(user_login_infor.wPort == '\0')
    {
        qDebug() <<"Please input the port number";
        return;
    }

    qDebug() << "sDeviceAddress" << user_login_infor.sDeviceAddress;
    qDebug() << "wPort" << user_login_infor.wPort;
    qDebug() << "sUserName" << user_login_infor.sUserName;
    qDebug() << "passward" << user_login_infor.sPassword;

    user_id = NET_DVR_Login_V40(&user_login_infor, &device_info);

    if (user_id < 0)
    {
        qDebug() << "NET_DVR_Login_V40 failed, error code:" << NET_DVR_GetLastError();
        ui->DebugTextBrowser->setText( "NET_DVR_Login_V40 failed, error code:" + NET_DVR_GetLastError());
        NET_DVR_Cleanup();
        return;
    }
    else
    {
        qDebug()<<"LOG in sussessed\n";
        ui->DebugTextBrowser->setText("LOG in sussesed");
    }

    HWND hWnd = (HWND)ui->video_label->winId(); //获取窗口句柄
    static NET_DVR_PREVIEWINFO play_infor = {0};
    play_infor.hPlayWnd = hWnd;  //需要SDK解码时句柄设为有效值，仅取流不解码时可设为空
    play_infor.lChannel = 1;     //预览通道号
    play_infor.dwStreamType = 0; //0-主码流，1-子码流，2-码流3，3-码流4，以此类推
    play_infor.dwLinkMode = 0;   //0- TCP方式，1- UDP方式，2- 多播方式，3- RTP方式，4-RTP/RTSP，5-RSTP/HTTP
    play_infor.bBlocked = 0;     //0- 非阻塞取流，1- 阻塞取流

    lRealPlayHandle = NET_DVR_RealPlay_V40(user_id, &play_infor, NULL, NULL);

    ui->camear_status->setStyleSheet(m_green_SheetStyle);

    if (lRealPlayHandle < 0)
    {
        qDebug()<<"NET_DVR_RealPlay_V40 error";
        NET_DVR_Logout(user_id);
        NET_DVR_Cleanup();
        return;
    }

}

/**
 * @brief 
 * 
 * @param checked 
 */
void MainWindow::on_close_preview_clicked(bool checked)
{
    //---------------------------------------
    //关闭预览
    NET_DVR_StopRealPlay(lRealPlayHandle);
    //注销用户
    NET_DVR_Logout(user_id);
    //释放SDK资源
    NET_DVR_Cleanup();


}

/**
 * @brief 
 * 
 * @param checked 
 */
void MainWindow::on_btn_capture_picture_clicked(bool checked)
{
    QByteArray byte_file_names;
    QDir dir;

    /*设置抓图模式，设置为jpeg模式*/
    bool error_code = NET_DVR_SetCapturePictureMode(JPEG_MODE);

    qDebug() << "time : " << QDate::currentDate().toString("yyyy_MM_dd");
    //QString current_pro_dir = QCoreApplication::applicationDirPath();
    QString current_pro_dir = dir.currentPath();
    qDebug()<< "current_pro_dir file name is "<< current_pro_dir;

    QString picture_dirs = "C:/Users/zhiqiang.xie/Desktop/QT_Project/serva_client/capture/" + QDate::currentDate().toString("yyyy_MM_dd");
    QString file_names = picture_dirs + '/' +  QTime::currentTime().toString("HH_mm_ss_zzz") + ".jpeg";

    qDebug()<< "capture file name is "<< file_names;

    if (!dir.exists(picture_dirs))
    {
        bool res = dir.mkpath(picture_dirs);
    }

    if(lRealPlayHandle < 0)
    {
        qDebug()<<"Open proview field";
    }

    /*Qstring 转化为char **/
    byte_file_names = file_names.toLatin1();

    NET_DVR_CapturePicture(lRealPlayHandle,byte_file_names.data());
}


/**
 * @brief 
 * 
 * @param checked 
 */
void MainWindow::on_pushButton_clicked(bool checked)
{
     QFile file("C:/Users/zhiqiang.xie/Desktop/QT_Project/serva_client/config/config.txt");
     QByteArray ip_address_r;
     QByteArray port_r;
     QByteArray user_name_r;
     QByteArray user_password_r;

     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        qDebug()<< "openfile field ";
        ui->DebugTextBrowser->setText("openfile field, please check the file");
        return;
     }

     ui->DebugTextBrowser->setText("open file successed");

     QByteArray line;
     unsigned char i = 0 ;
     while (!file.atEnd())
     {
         switch (i) {
         case 0:
             ip_address_r = file.readLine();
             ip_address_r[ip_address_r.length()-1] = '\0';
             qDebug()<< "ip_address_r is "<< ip_address_r;
             break;
         case 1:
             port_r = file.readLine();
             port_r[port_r.length()-1] = '\0';
             qDebug()<< "port_r is "<< port_r;
             break;
         case 2:
             user_name_r = file.readLine();
             user_name_r[user_name_r.length()-1] = '\0';
             qDebug()<< "user_name_r is "<< user_name_r;
             break;
         case 3:
             user_password_r = file.readLine();
             qDebug()<< "user_password_r is "<< user_password_r;
         default:
             break;
         }
        i ++;
     }

     ui->sets_ip->setText(ip_address_r);
     ui->sets_ip_ports->setText(port_r);
     ui->user_name->setText(user_name_r);
     ui->passward->setText(user_password_r);

}

void MainWindow::keyPressEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_W)
    {
        is_linear_acc = true;
    }
    if(ev->key() == Qt::Key_S)
    {
       is_linear_dec = true;
    }
    if(ev->key() == Qt::Key_D)
    {
        is_argular_acc = true;
    }
    if(ev->key() == Qt::Key_A)
    {
        is_argular_dec = true;
    }

}

void MainWindow::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_W)
    {
       is_linear_acc = false;
    }
    if(ev->key() == Qt::Key_S)
    {
       is_linear_dec = false;
    }
    if(ev->key() == Qt::Key_D)
    {
        is_argular_acc = false;
    }
    if(ev->key() == Qt::Key_A)
    {
        is_argular_dec = false;
    }
}

/**
 * @brief 
 * 
 */
void MainWindow::ScanfUdplPort()
{
    //QUdpSocket *udp_bak = new QUdpSocket;
    //const char sof_connection[] = {0x55,0x55,0x55,0x55,0x00};//QHostAddress("192.168.137.1")
    //udp_socket->writeDatagram(sof_connection,QHostAddress("192.168.31.255"),7780);//QHostAddress::Broadcast

    //qDebug()<<"Broadcast the cmd successd";
}

//sof 0x5a a5
//len 0x
//cmd_type 0xaa
//data data[7]
    // boardcast 0x00 0x5a 0x5a 0x5a 0x5a 0x5a 0x5a
    // status
                 //msg_id 0x01
                 //ctrl_mode
                 //voltage h_byte
                 //voltagt l_byte
                 //linear  percent
                 //argular percent
                 //reserver

//checksum
/**
 * @brief 
 * 
 * @param char 
 * @param len 
 * @param addr 
 */
void MainWindow::ProcessUdpMsg(unsigned char* data, unsigned char len,QHostAddress addr)
{
    /*判断数据起始位*/
    if(data[0] == 0x5A && data[1] == 0xA5)
    {
        switch (data[4]) {
        case eUdpFdbk_BroacastCmd:

            qDebug()<<"Get the udp feedback broadcast command";
            if(data[11] ==UdpMsgChecksum((char *)data, len))
            {
                if(data[5] == 0x5a &&\
                        data[6] == 0x5a &&\
                        data[7] == 0x5a &&\
                        data[8] == 0x5a &&\
                        data[9] == 0x5a &&\
                        data[10] == 0x5a)
                {
                    is_get_robot_platform_udp_response = true;
                    is_platform_online = true;

                    platform_ip_address = addr;

                    ui->lineEdit_platformip_addr->setText(platform_ip_address.toString().mid(7));

                    if(is_broadcast_timer_had_start == true)
                    {
                        /*停止广播*/
                        broadcast_timer->stop();
                    }

                    is_broadcast_timer_had_start = false;

                    ui->DebugTextBrowser->append("Get the platform udp response");
                    qDebug()<<"Get the platform udp response";
                }
            }
            break;

        case eUdpFdbk_Status:

            qDebug()<<"Get the robot platform status feedback";
            if(data[11] ==UdpMsgChecksum((char *)data, len))
            {
                chassis_status_fdbk.ctrl_mode = data[5];
                chassis_status_fdbk.volt = ((data[6]&0x00ff)<<8)|data[7];
                chassis_status_fdbk.linear = data[8];
                chassis_status_fdbk.angular = data[9];
                chassis_status_fdbk.resever = data[10];

                /*设置UI界面*/
                ui->lineEdit_status_fdbk->setText(QString::number(chassis_status_fdbk.ctrl_mode));
                ui->lineEdit_voltage_fdbk->setText(QString::number(chassis_status_fdbk.volt));
                ui->lineEdit_linear_fdbk->setText(QString::number(chassis_status_fdbk.linear));
                ui->lineEdit_argular_dfbk->setText(QString::number(chassis_status_fdbk.angular));

                //ui->DebugTextBrowser->append("Get the robot platform status feedback");
            }

            break;

        default:
            qDebug()<<"default end !!!!!!!";
            break;
        }
    }
}

/**
 * @brief 
 * 
 */
void MainWindow::UdpRxHandle()
{
    char udp_rec_buf[1024] = {0};
    QHostAddress addr; //对方的ip
    quint16 port; //对方的端口
    qint64 udp_rec_data_length;
    udp_rec_data_length = udp_socket->readDatagram(udp_rec_buf, sizeof(udp_rec_buf), &addr, &port);
    {
        if(udp_rec_data_length >= 9)
        {
            QString str = QString("[%1:%2]  %3").arg(addr.toString()).arg(port).arg(udp_rec_buf);
            ProcessUdpMsg((unsigned char *)udp_rec_buf,udp_rec_data_length,addr);
            qDebug()<<"listen is "<< str;
        }
    }
}

/**
 * @brief 
 * 
 */
void MainWindow::PlatformVelSet()
{
    if(is_linear_acc == true)
    {
        cmd.linear = cmd.linear + KEY_CTRL_STEP;

        if(cmd.linear >= 100)
        {
            cmd.linear = 100;
        }
    }
    else if(is_linear_dec == true)
    {
        cmd.linear = cmd.linear - KEY_CTRL_STEP;

        if(cmd.linear <= -100)
        {
            cmd.linear = -100;
        }
    }
    else if(is_linear_acc == false && is_linear_dec == false)
    {
        if(cmd.linear > 0 )
        {
            cmd.linear = cmd.linear - KEY_CTRL_STEP;
            if(cmd.linear < 0 )
                cmd.linear = 0;
        }
        else if(cmd.linear < 0)
        {
            cmd.linear = cmd.linear + KEY_CTRL_STEP;
            if(cmd.linear > 0 )
                cmd.linear = 0;
        }
    }

    if(is_argular_acc == true)
    {
        cmd.angular = cmd.angular + KEY_CTRL_STEP;

        if(cmd.angular >= 100)
        {
            cmd.angular = 100;
        }
    }
    else if(is_argular_dec == true)
    {
        cmd.angular = cmd.angular - KEY_CTRL_STEP;

        if(cmd.angular <= -100)
        {
            cmd.angular = -100;
        }
    }
    else if(is_argular_acc == false && is_argular_dec == false)
    {
        if(cmd.angular > 0 )
        {
            cmd.angular = cmd.angular - KEY_CTRL_STEP;
            if(cmd.angular < 0 )
                cmd.angular = 0;
        }
        else if(cmd.angular < 0)
        {
            cmd.angular = cmd.angular + KEY_CTRL_STEP;
            if(cmd.angular > 0 )
                cmd.angular = 0;
        }
    }

    ui->linear_set->clear();
    ui->linear_set->setText(QString::number(cmd.linear));
    ui->argular_set->clear();
    ui->argular_set->setText(QString::number(cmd.angular));

    cmd.cmd_mode = (CmdModdeStutus_t)ui->ctrl_mode->currentIndex();
}

/**
 * @brief 
 * 
 */
void MainWindow::TransimitCmdByUdp()
{
    if(is_platform_online == true)
    {
        ui->platform_online_status->setStyleSheet(m_green_SheetStyle);
        CtrlCmdToPlatform();
    }
}

/**
 * @brief 
 * 
 */
void MainWindow::on_bt_connect_platform_clicked()
{
    if(is_get_robot_platform_udp_response == false && is_broadcast_timer_had_start == false)
    {
        broadcast_timer = new QTimer(this);
        connect(broadcast_timer,&QTimer::timeout,this,&MainWindow::CtrlBroadcastToPlatform);
        broadcast_timer->start(500);

        is_broadcast_timer_had_start = true;
    }
}

/**
 * @brief
 *
 */
void MainWindow::on_bt_disconnect_platform_clicked()
{
    if(is_get_robot_platform_udp_response == false && is_broadcast_timer_had_start == true)
    {
        broadcast_timer->stop();

        ui->platform_online_status->setStyleSheet(m_red_SheetStyle);
    }

    is_get_robot_platform_udp_response = false;
    is_platform_online = false;
}

/**
 * @brief 
 * 
 */
void MainWindow::on_platform_forward_clicked()
{
    cmd.linear = cmd.linear + KEY_CTRL_STEP;

    if(cmd.linear >= 100)
    {
        cmd.linear = 100;
    }

    ui->linear_set->clear();
    ui->linear_set->setText(QString::number(cmd.linear));
}

/**
 * @brief 
 * 
 */
void MainWindow::on_platform_back_clicked()
{
    cmd.linear = cmd.linear - KEY_CTRL_STEP;

    if(cmd.linear <= -100)
    {
        cmd.linear = -100;
    }

    ui->linear_set->clear();
    ui->linear_set->setText(QString::number(cmd.linear));
}

/**
 * @brief 
 * 
 */
void MainWindow::on_platform_turn_left_clicked()
{
    cmd.angular = cmd.angular + KEY_CTRL_STEP;

    if(cmd.angular >= 100)
    {
        cmd.angular = 100;
    }

    ui->argular_set->clear();
    ui->argular_set->setText(QString::number(cmd.angular));
}

/**
 * @brief 
 * 
 */
void MainWindow::on_platform_trun_right_clicked()
{
    cmd.angular = cmd.angular - KEY_CTRL_STEP;

    if(cmd.angular <= -100)
    {
        cmd.angular = -100;
    }

    ui->argular_set->clear();
    ui->argular_set->setText(QString::number(cmd.angular));
}

/**
 * @brief 
 * 
 */
void MainWindow::on_platform_stop_clicked()
{
    cmd.linear = 0 ;
    cmd.angular = 0;

    ui->linear_set->clear();
    ui->linear_set->setText(QString::number(cmd.linear));
    ui->argular_set->clear();
    ui->argular_set->setText(QString::number(cmd.angular));
}
