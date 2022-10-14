#ifndef IMAGESHOW_H
#define IMAGESHOW_H
#include <QDebug>
#include <QLabel>
#include <QPoint>
#include <QWidget>
#include <QSerialPortInfo>
#include <QThread>
#include <QPointF>
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <iostream>
#include <string>
#include "mainwindow.h"
#include <QPainter>
#include "imageprocess.h"
#include <qcustomplot.h>

#include <qimage.h>

extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/dict.h"
    #include "libswscale/swscale.h"
    #include "libavdevice/avdevice.h"
    #include "libavformat/avio.h"
    #include "libavutil/imgutils.h"
}

using namespace std;

namespace Ui {
class imageshow;
}

class imageshow : public QWidget
{
    Q_OBJECT

public:
    explicit imageshow(QWidget *parent = 0);
    ~imageshow();
    void paintEvent(QPaintEvent *event);
public slots:
    void DataToImage(QByteArray data);
    void getPixPos(QPointF pt);
signals:
    void sendPosToGui(int x,int y, int color);
private slots:
    void on_btn_imagesave_clicked();

    void on_btn_imageread_clicked();

    void on_btn_videosave_clicked();

    void on_btn_videoread_clicked();


    void on_btn_videoshow_clicked();

    void VideoShow();



    void on_PicSlider_valueChanged(int value);

    void on_PicSlider_sliderPressed();

    void on_PicSlider_actionTriggered(int action);

    void on_PicSlider_sliderMoved(int position);

    void on_checkBox_clicked(bool checked);

    void Imhist(QImage *img);

private:
    Ui::imageshow *ui;
    int Imagsave_flag;//保存图像标志位
    int videoToImage(const QString &audioPath,const QString&outputDir);

    QTimer *videotimer;//定时器


    /* 图像保存路径*/
    QString runPath;
    QString hglpName;
    QString hglpPath;

    QString directory;

    ImageProcess imagepro;

    int Pic_num;//视频转换出来的图片数量
    int action_flag;//播放标志
    int Image_process_flag;//图像处理标志位

    QCPBars *bars;




    std::vector<int> data0;
};



#endif // IMAGESHOW_H
