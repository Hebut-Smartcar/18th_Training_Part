#ifndef WAVEFORM_H
#define WAVEFORM_H


#include <QWidget>
/*------------------------用户代码头文件---------------------------*/
#include <QtSerialPort/QSerialPort>//串口
#include <QtSerialPort/QSerialPortInfo>//串口
#include <QDebug>//用于在控制台输出调试信息
#include <QTime>//定时器
#include <QPainter>//坐标系绘图
#include <QWidget>

namespace Ui {
class WaveForm;
}

class WaveForm : public QWidget
{
    Q_OBJECT

public:
    explicit WaveForm(QWidget *parent = 0);
    ~WaveForm();
    void setupPlot();   //初始化
public slots:
    void AnalyzeData(QByteArray data); //数据读取


private:
    Ui::WaveForm *ui;    //绘图函数
    QDateTime mycurrenttime;//系统当前时间
    QDateTime mystarttime;//系统开始时间


};

#endif // WAVEFORM_H
