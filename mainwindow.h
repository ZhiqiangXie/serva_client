#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "HIKVision_SDK/head/HCNetSDK.h"
#include <qstylefactory.h>


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
    void on_pushButton_text_pressed();

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

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
