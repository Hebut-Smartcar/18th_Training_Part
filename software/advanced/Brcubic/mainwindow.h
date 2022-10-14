#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort>//串口
#include <QDebug>//用于在控制台输出调试信息
#include <QSerialPortInfo>
#include <QThread>
#include <QPointF>
#include <mouseevent.h>
#include "logoshow.h"
#include "instruct.h"
#include "help.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    ~MainWindow();

private slots:
    void on_btn_seacher_clicked();

    void on_btn_open_clicked();

    void on_btn_sendstart_clicked();

    void on_btn_sendstop_clicked();



signals:
    void serialDataSend(const QByteArray data); //串口发送信号
    void sendResultToWave(QByteArray data);     //串口数据波形绘制信号
    void sendDatatoImage(QByteArray data);      //发送给显示器显示图像
    void sendDatatoParam(QByteArray data);      //发送给参数处理


public slots:
    void handleResults(QByteArray result);
private slots:
    void setStatusBar(int x,int y, int color);

    void on_actionwave_triggered();

    void on_actionimage_triggered();

    void on_actioninstruct_triggered();

    void on_actionparameter_triggered();

    void on_setting_triggered();

    void on_actionhelp_triggered();

private:
    Ui::MainWindow *ui;
    QSerialPort *myserial;//声明串口类，myserial是QSerialPort的实例
    bool serial_flag;//串口开关标志
    QTimer *timer;//定时器
    QPixmap *pixmap;
    QByteArray byteArray;
    QMatrix matrix;
    QThread  chithread; // 定义子线程chithread
    Logoshow *logo;



};

#endif // MAINWINDOW_H
