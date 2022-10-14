#include "instruct.h"
#include "ui_instruct.h"

Instruct::Instruct(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Instruct)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Instruct"));  //设置标题
    //this->setWindowIcon(QIcon(":/Image/8315/m16.png"));
}

Instruct::~Instruct()
{
    delete ui;
}
