#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "QDebug"
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>

using namespace std;

static NET_DVR_USER_LOGIN_INFO user_login_infor = {0};
static NET_DVR_DEVICEINFO_V40 device_info = {0};
bool connect_btn_flag = false;
LONG user_id;
LONG lRealPlayHandle  = -1;

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
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, QColor(15, 15, 15));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(palette);
    Sleep(10000);
}

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

    if (lRealPlayHandle < 0)
    {
        printf("NET_DVR_RealPlay_V40 error\n");
        NET_DVR_Logout(user_id);
        NET_DVR_Cleanup();
        return;
    }

}

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

void MainWindow::on_btn_capture_picture_clicked(bool checked)
{
    QByteArray byte_file_names;
    QDir dir;


    /*设置抓图模式，设置为jpeg模式*/
    bool error_code = NET_DVR_SetCapturePictureMode(JPEG_MODE);

    qDebug() << "time : " << QDate::currentDate().toString("yyyy_MM_dd");
    QString picture_dirs = "C:/Users/zhiqiang.xie/Desktop/QT_Project/hello_world/capture/" + QDate::currentDate().toString("yyyy_MM_dd");
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
