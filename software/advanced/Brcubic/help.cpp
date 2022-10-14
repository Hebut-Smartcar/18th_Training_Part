#include "help.h"
#include "ui_help.h"
#include <QDesktopServices>
#include <QUrl>

Help::Help(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("Help"));  //设置标题
    this->setWindowIcon(QIcon(":/image/m16.png"));
}

Help::~Help()
{
    delete ui;
}

void Help::on_githubButton_clicked()
{
    QDesktopServices::openUrl(QUrl("https://github.com/BriMonzZY", QUrl::TolerantMode));
}
