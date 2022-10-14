#include "setting.h"
#include "ui_setting.h"
#include <QIcon>

setting::setting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setting)
{
    ui->setupUi(this);

    this->setWindowTitle(QString("Setting"));  //设置标题
    this->setWindowIcon(QIcon(":/image/m16.png"));
}

setting::~setting()
{
    delete ui;
}
