#include "serialworker.h"
#include <QDebug>
#include <QThread>
#include "mainwindow.h"
SerialWorker::SerialWorker(QSerialPort *ser,QObject *parent)
    : QObject(parent),serial(ser)
{
    buffer.reserve(4*(1<<20)); // 4MB
}


void SerialWorker::doDataSendWork(const QByteArray data)
{
    qDebug() <<  "子线程槽函数发送数据：" << data << "线程ID：" << QThread::currentThreadId();

    // 发送数据
    serial->write(data);
}


extern int image_enable;
extern int param_enable;

void SerialWorker::doDataReciveWork()
{
    QByteArray ByteArray = serial->readAll();

    if(image_enable) {
//      qDebug() << ByteArray.size();
//      qDebug() <<  "子线程发送结果信号：" << ByteArray << "线程ID：" << QThread::currentThreadId();
        emit sendResultToGui(ByteArray);  // 将结果发送到主线程
    }
    else {
        for(auto ch: ByteArray){
            if(ch == '\n') {         /* \n代表一帧数据的结尾 */
                qDebug() << buffer;
                emit sendResultToGui(buffer);  // 转到mainwindow 的 handleResults
                buffer.clear();
            }
            else {                   /* 没有读到数据的结尾就一直接收 */
                buffer.append(ch);
            }
        }
    }
}
