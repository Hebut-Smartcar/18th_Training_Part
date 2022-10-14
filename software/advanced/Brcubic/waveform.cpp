#include "waveform.h"
#include "ui_waveform.h"
#include <serialworker.h>
#include <QVector>
#include <QMessageBox>
WaveForm::WaveForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WaveForm)
{
    ui->setupUi(this);
    setupPlot();
    this->setWindowTitle(QString("Wave show"));  //设置标题
    //this->setWindowIcon(QIcon(":/Image/8315/m16.png"));
    mystarttime = QDateTime::currentDateTime();//图像横坐标初始值参考点，读取初始时间
}

WaveForm::~WaveForm()
{
    delete ui;
    qDebug()<<"waveend";

}

void WaveForm::setupPlot()
{
    //设置曲线一

    ui->widget_plot->addGraph();//添加一条曲线
    QPen pen;
    pen.setWidth(1);//设置画笔线条宽度
    pen.setColor(Qt::blue);
    ui->widget_plot->graph(0)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(0)->setName("0-100");
    ui->widget_plot->graph(0)->setAntialiasedFill(false);
    ui->widget_plot->graph(0)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    ui->widget_plot->addGraph();//添加一条曲线
    pen.setColor(Qt::red);
    ui->widget_plot->graph(1)->setPen(pen);//设置画笔颜色
    ui->widget_plot->graph(1)->setBrush(QBrush(QColor(0, 0, 255, 20))); //设置曲线画刷背景
    ui->widget_plot->graph(1)->setName("0-100");
    ui->widget_plot->graph(1)->setAntialiasedFill(false);
    ui->widget_plot->graph(1)->setLineStyle((QCPGraph::LineStyle)1);//曲线画笔
    ui->widget_plot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone,5));//曲线形状

    ui->widget_plot->xAxis->setLabel(QStringLiteral("时间/s"));//设置x坐标轴名称
    ui->widget_plot->xAxis->setLabelColor(QColor(20,20,20));//设置x坐标轴名称颜色
    ui->widget_plot->xAxis->setAutoTickStep(false);//设置是否自动分配刻度间距
    ui->widget_plot->xAxis->setTickStep(2);//设置刻度间距5
    ui->widget_plot->xAxis->setRange(0,30);//设定x轴的范围

    ui->widget_plot->yAxis->setLabel(QStringLiteral("paraments"));//设置y坐标轴名称
    ui->widget_plot->yAxis->setLabelColor(QColor(20,20,20));//设置y坐标轴名称颜色
    ui->widget_plot->yAxis->setAutoTickStep(false);//设置是否自动分配刻度间距
    ui->widget_plot->yAxis->setTickStep(10);//设置刻度间距1
    ui->widget_plot->yAxis->setRange(0,100);//设定y轴范围

    ui->widget_plot->axisRect()->setupFullAxesBox(true);//设置缩放，拖拽，设置图表的分类图标显示位置
    ui->widget_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom| QCP::iSelectAxes);
    ui->widget_plot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop | Qt::AlignRight);//图例显示位置右上
    ui->widget_plot->legend->setVisible(true);//显示图例

    ui->widget_plot->replot();
}

void WaveForm::AnalyzeData(QByteArray data)
{
    QByteArray mytemp = data;//定义mytemp为串口读取的所有数据
    static QString StrI1;
    static QString StrI2;
//    qDebug()<<"mytemp:"<<mytemp;
    //int waveflag=0;
    if(!mytemp.isEmpty())
    {
        if(mytemp.indexOf("P")-mytemp.indexOf("T")>0)
             StrI1=tr(mytemp.mid(mytemp.indexOf("T")+1,mytemp.indexOf("P")-mytemp.indexOf("T")-1));//自定义了简单协议，通过前面字母读取需要的数据

        if(mytemp.indexOf("H")-mytemp.indexOf("I")>0)
             StrI2=tr(mytemp.mid(mytemp.indexOf("H")+1,mytemp.indexOf("I")-mytemp.indexOf("H")-1));

        float dataI1=StrI1.toFloat();//将字符串转换成float类型进行数据处理
        float dataI2=StrI2.toFloat();//将字符串转换成float类型进行数据处理

        mycurrenttime = QDateTime::currentDateTime();//获取系统时间
        double xzb = mystarttime.msecsTo(mycurrenttime)/1000.0;//获取横坐标，相对时间就是从0开始
//        qDebug()<<"xzb:"<<xzb;        //注：下位机每隔10ms发送一次数据
        ui->line_pra1->setText(StrI1);//显示读取温度值
        ui->line_pra2->setText(StrI2);//显示读取湿度值
        ui->widget_plot->graph(0)->addData(xzb,dataI1);//添加数据1到曲线1
        ui->widget_plot->graph(1)->addData(xzb,dataI2);//添加数据1到曲线1
        if(xzb>30)
        {
            ui->widget_plot->xAxis->setRange((double)qRound(xzb-30),xzb);//设定x轴的范围
        }
        else ui->widget_plot->xAxis->setRange(0,30);//设定x轴的范围
        ui->widget_plot->replot();//每次画完曲线一定要更新显示

    }
}
