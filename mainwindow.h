void on_bt_disconnect_platform_clicked();
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HIKVision_SDK/head/HCNetSDK.h"
#include <qstylefactory.h>
#include <QUdpSocket>
#include <QTimer>
#include<QKeyEvent>
#include "types.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_radioButton_pressed();

    void on_connection_btn_pressed();
    void Onconnection_btnpressed();
    void on_connection_btn_released();

    void on_pan_left_pressed();

    void on_pan_left_released();

    void on_pan_right_pressed();

    void on_pan_right_released();

    void on_pan_up_pressed();

    void on_pan_up_released();

    void on_pan_dowm_pressed();

    void on_pan_dowm_released();

    void on_open_preview_clicked(bool checked);

    void on_close_preview_clicked(bool checked);

    void on_btn_capture_picture_clicked(bool checked);

    void on_pushButton_clicked(bool checked);

    void ScanfUdplPort();
    void UdpRxHandle();
    void PlatformVelSet();
    void TransimitCmdByUdp();

    void on_bt_connect_platform_clicked();

    void on_bt_disconnect_platform_clicked();

    void on_platform_forward_clicked();

    void on_platform_back_clicked();

    void on_platform_turn_left_clicked();

    void on_platform_trun_right_clicked();

    void on_platform_stop_clicked();

private:

    Ui::MainWindow *ui;
    QUdpSocket *udp_socket;

    QTimer *udp_timer;
    QTimer *speed_set_timer;
    QTimer *cmd_transimit;
    QTimer *broadcast_timer;

    /*底盘状态反馈*/
    ChassisStatusFeedback_t chassis_status_fdbk;

    /*机器人底盘IP地址*/
    QHostAddress platform_ip_address;

    /*设置线速度和角速度相关*/
    bool is_linear_acc = false;
    bool is_linear_dec = false;

    bool is_argular_acc = false;
    bool is_argular_dec = false;

    /*机器人底盘是否的广播是否得到回应*/
    bool is_get_robot_platform_udp_response = false;
    bool is_broadcast_timer_had_start = false;

    /*机器人底盘是否为在线状态*/
    bool is_platform_online = false;

    ChassisCMD_t cmd;
    unsigned char UdpMsgChecksum(char *data, unsigned char len);
    void TransMakePKG(SerialCtrlFrame_t s_frame);
    void CtrlCmdToPlatform(void);
    void CtrlBroadcastToPlatform(void);
    void ProcessUdpMsg(unsigned char* data, unsigned char len,QHostAddress addr);

    virtual void keyPressEvent(QKeyEvent *ev);
    virtual void keyReleaseEvent(QKeyEvent *ev);
};

#endif // MAINWINDOW_H
