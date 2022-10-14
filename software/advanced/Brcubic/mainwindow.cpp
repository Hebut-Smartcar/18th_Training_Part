#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <serialworker.h>
#include "imageshow.h"
#include "waveform.h"
#include "parameter.h"
#include <QDebug>
#include <QLabel>
#include <QPoint>

#include "parameter.h"
#include "viewparameter.h"
#include "setting.h"

#include <QtCore/QProcess>

extern int image_enable;
extern int param_enable;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Brcubic"));  //设置标题
    this->setWindowIcon(QIcon(":/image/m17.png"));

    myserial = new QSerialPort;
    timer = new QTimer();
    ui->menuBar->setVisible(false);



/*************线程***************/
    // 1.新建串口处理子线程
    SerialWorker *serialWorker = new SerialWorker(myserial);
    serialWorker->moveToThread(&chithread);
    // 2.连接信号和槽
    connect(&chithread, &QThread::finished,
            serialWorker, &QObject::deleteLater);           // 线程结束，自动删除对象
    connect(this, &MainWindow::serialDataSend,
            serialWorker, &SerialWorker::doDataSendWork);   // 主线程串口数据发送的信号
    connect(myserial, &QSerialPort::readyRead,
            serialWorker, &SerialWorker::doDataReciveWork);
    connect(serialWorker, &SerialWorker::sendResultToGui,
            this, &MainWindow::handleResults);              // 主线程收到数据结果的信号

    // 3.开始运行子线程
    chithread.start();                   // 线程开始运行


/*************线程***************/


    logo= new Logoshow(this);
    logo->setAttribute(Qt::WA_DeleteOnClose);
    ui->tabWidget->addTab(logo,"");
    ui->tabWidget->tabBar()->hide();
    ui->tabWidget->setVisible(true);


    QLabel *aixLabel = new QLabel("BriMon");
    aixLabel->setAlignment(Qt::AlignHCenter);
    statusBar()->addWidget(aixLabel, 1);

    param_enable = 0;


//    QStringList str;
//    str.clear();
//    str << "a" << "b" ;
//    QProcess *process = new QProcess();
//    process->start("C:/Users/brimon/Desktop/helloworld.c", str);
//    process->waitForStarted();
//    process->waitForFinished();

//    QByteArray qb = process->readAll();
//    QString str22(qb);
//    QTextStream cout(stdout);
//    qDebug() << str22;

}

void MainWindow::paintEvent(QPaintEvent *event)
{

}

MainWindow::~MainWindow()
{
    // 退出串口1子线程
    chithread.quit();
    chithread.wait();
    qDebug()<<"mainend";
    delete ui;
}

void MainWindow::on_btn_seacher_clicked()
{
    ui->com_choose->clear();  //清空串口列表

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())//读取串口信息
    {
        myserial->setPort(info);                        //这里相当于自动识别串口号之后添加到了cmb，如果要手动选择可以用下面列表的方式添加进去

        if(myserial->open(QIODevice::ReadWrite)) {
            ui->com_choose->addItem(myserial->portName());//将串口号添加到cmb
            myserial->close();                            //关闭串口等待人为(打开串口按钮)打开
        }
     }
}

void MainWindow::on_btn_open_clicked()
{
    if(ui->btn_open->text()=="打开串口") {

        myserial->setPortName(ui->com_choose->currentText());
        myserial->setBaudRate(ui->box_baud->currentText().toInt()); //设置波特

        /* 设置数据位 */
        qint32 dataBitIndex = ui->dataBitBox->currentText().toInt();
        switch(dataBitIndex) {
            case 5: myserial->setDataBits(QSerialPort::Data5); break;
            case 6: myserial->setDataBits(QSerialPort::Data6); break;
            case 7: myserial->setDataBits(QSerialPort::Data7); break;
            case 8: myserial->setDataBits(QSerialPort::Data8); break;
        }

        /* 设置校验位 */
        QString parityBitIndex = ui->parityBitBox->currentText();
        if(parityBitIndex == "None") myserial->setParity(QSerialPort::NoParity);
        else if(parityBitIndex == "Mark") myserial->setParity(QSerialPort::MarkParity);
        else if(parityBitIndex == "Odd") myserial->setParity(QSerialPort::OddParity);
        else if(parityBitIndex == "Even") myserial->setParity(QSerialPort::EvenParity);

        /* 设置停止位 */
        QString stopBitIndex = ui->stopBitBox->currentText();
        if(stopBitIndex == "One") myserial->setStopBits(QSerialPort::OneStop);
        else if(stopBitIndex == "Two") myserial->setStopBits(QSerialPort::TwoStop);
        else if(stopBitIndex == "OneAndHalf") myserial->setStopBits(QSerialPort::OneAndHalfStop);

        myserial->setFlowControl(QSerialPort::NoFlowControl);//非流控制

        if(myserial->open(QIODevice::ReadWrite)){
            qDebug()<<(ui->com_choose->currentText())<<"串口打开成功";
        }
        else {
            QMessageBox::critical(this, "warning", "failed to open the serial, The serial port may be occupied now");
            myserial->close();
            return;
        }


        ui->btn_open->setText("关闭串口");
        ui->btn_seacher->setDisabled(true);
        ui->com_choose->setDisabled(true);
    }


    else if(ui->btn_open->text()=="关闭串口") {

        myserial->close();
        qDebug()<<(ui->com_choose->currentText())<<"串口已经关闭";
        ui->btn_seacher->setDisabled(false);
        ui->com_choose->setDisabled(false);
        ui->btn_open->setText("打开串口");


    }

}



void MainWindow::on_btn_sendstart_clicked()
{
//    timer->start(10);                     //定时器中断1s一次
    //获取界面上的数据并转换成utf8格式的字节流
    QByteArray data;
    data="1";
    emit serialDataSend(data);
    qDebug() <<  "主线程发送信号，线程ID：" << QThread::currentThreadId();



}

void MainWindow::on_btn_sendstop_clicked()
{
    if(timer->isActive())
    {
        timer->stop();
    }

}





/*************************************************  数据处理  *************************************************/
void MainWindow::handleResults(QByteArray result)
{
    QByteArray array;
    QByteArray tempbyte = result;
    QByteArray tempbyte2 = result;

    emit sendResultToWave(tempbyte);  // 示波器
    emit sendDatatoParam(tempbyte2);  // 参数调节   关联到 parameter 的  AnalyzeparamData


#if 1
    if(tempbyte.isEmpty()==false) {
        byteArray.append(tempbyte);
        if(byteArray.contains("START")||byteArray.contains("STOP")){

            if(byteArray.contains("START")&&!byteArray.contains("STOP")) { //只有开始标志
                byteArray = byteArray.right(byteArray.length()-byteArray.indexOf("START")-5);

            }
            if(byteArray.contains("STOP")&&!byteArray.contains("START")) {//只有结束标志
                array = byteArray.left(byteArray.indexOf("STOP"));
                byteArray=byteArray.left(0);
            }

            if(byteArray.contains("STOP")&&byteArray.contains("START")) {//开始结束都存在
                 array = byteArray.left(byteArray.indexOf("STOP"));
                 byteArray=byteArray.right(byteArray.length()-byteArray.indexOf("START")-5);
            }

            //qDebug()<<array.size();


            // 图传键没有按下时不显示图像
            if(image_enable) emit sendDatatoImage(array);  // 连接到imageshow 的 DataToImage

        }
    }
#endif
}

void MainWindow::setStatusBar(int x,int y, int color)
{

    QString strShow1 = QString("Pixle Positon: X:%1 Y:%2  color: %3").arg(x).arg(y).arg(color);
    ui->statusBar->showMessage(strShow1);//将光标坐标显示到状态栏
}


void MainWindow::on_actionwave_triggered()
{
    // 因为串口接收原理不同不能和图传同时打开
    if(image_enable) {
        QMessageBox::critical(this, "warning", "请先关闭图传再打开示波器");
        return;
    }

     WaveForm *waveform = new WaveForm();
     waveform->show();
     waveform->setAttribute(Qt::WA_DeleteOnClose);//关闭的时候调用析构函数

     connect(this, &MainWindow::sendResultToWave, waveform, &WaveForm::AnalyzeData);
     //connect(this, SIGNAL(sendResultToWave(QByteArray)), waveform, SLOT(&WaveForm::AnalyzeData));
}


int image_once_flag = 0;

void MainWindow::on_actionimage_triggered()
{
    if(ui->actionimage->isChecked()) {  // 开始图传
        image_enable = 1;
        if(!image_once_flag) {
            delete logo;
            imageshow *imag = new imageshow(this);
            imag->setAttribute(Qt::WA_DeleteOnClose);
            ui->tabWidget->addTab(imag,"");
            ui->tabWidget->tabBar()->hide();
            ui->tabWidget->setVisible(true);

            connect(this,&MainWindow::sendDatatoImage,imag,&imageshow::DataToImage);
            connect(imag,&imageshow::sendPosToGui,this,&MainWindow::setStatusBar);//信号与槽绑定  鼠标获取坐标事件


            image_once_flag = 1;
        }
    }
    else {  // 停止图传
        image_enable = 0;
        qDebug() << "close image show";
    }
}

void MainWindow::on_actioninstruct_triggered()
{
    Instruct *instruct = new Instruct();
    instruct->show();
    instruct->setAttribute(Qt::WA_DeleteOnClose);//关闭的时候调用析构函数
}

void MainWindow::on_actionhelp_triggered()
{
    Help *help = new Help();
    help->show();
    help->setAttribute(Qt::WA_DeleteOnClose);//关闭的时候调用析构函数
}



void MainWindow::on_actionparameter_triggered()
{
    // 因为串口接收原理不同不能和图传同时打开
    if(image_enable) {
        QMessageBox::critical(this, "warning", "请先关闭图传再打开参数调整工具");
        return;
    }


    parameter *Parameter = new parameter();
    Parameter->show();
    Parameter->setAttribute(Qt::WA_DeleteOnClose);//关闭的时候调用析构函数

    connect(this,&MainWindow::sendDatatoParam,Parameter,&parameter::AnalyzeparamData);
}

void MainWindow::on_setting_triggered()
{
    setting *Setting = new setting();
    Setting->show();
    Setting->setAttribute(Qt::WA_DeleteOnClose);//关闭的时候调用析构函数
}
