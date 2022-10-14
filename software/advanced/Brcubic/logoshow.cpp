#include "logoshow.h"
#include "ui_logoshow.h"

Logoshow::Logoshow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Logoshow)
{
    ui->setupUi(this);
}

Logoshow::~Logoshow()
{
    delete ui;
}


void Logoshow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    painter.drawPixmap(0,0,width(),
                       height(),
                       QPixmap(":/image/m18.jpg"));

}
