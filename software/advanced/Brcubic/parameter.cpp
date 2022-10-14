#include "parameter.h"


parameter::parameter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::parameter)
{
    ui->setupUi(this);
    this->setWindowTitle(QString("parameter"));  //设置标题
    //this->setWindowIcon(QIcon(":/Image/8315/m16.png"));



    paramtmpWidge = new QWidget; // 创建滚动显示区域幕布
    l = new QVBoxLayout(paramtmpWidge); // 创建布局, 用于将控件嵌入幕布中
    ui->paramScrollArea->setWidget(paramtmpWidge);
}

parameter::~parameter()
{
    delete ui;
}


#define PARSE_ASSERT(expr) do{if(!(expr)){qDebug("'"#expr"' assert fail!"); return;}}while(0)
#define PARSE_CHECK(obj, name, type) do{PARSE_ASSERT(obj.contains(name));PARSE_ASSERT(obj[name].is##type());}while(0);

float ty=0;

// 分析数据
void parameter::AnalyzeparamData(QByteArray data)
{
    QByteArray mytemp = data;

//    updateParam("param1", 1, 10, 0.01, ty);  // test
//    updateParam("param2", 1, 20, 0.1, 3);  // test
//    updateParam("param3", 1, 30, 1, 5);  // test


//    auto doc = QJsonDocument::fromJson(mytemp);  /* 将发送过来的json格式数据转换为json内容 */  /* auto关键词：自动推导变量的类型 */
//    PARSE_ASSERT(!doc.isEmpty());  /* 判断数据非空 */
//    auto obj = doc.object();
//    PARSE_CHECK(obj, "type", String);
//    PARSE_CHECK(obj, "body", Object);

//    auto type = obj["type"].toString();
//    auto body = obj["body"].toObject();


//    qDebug() << type;
}


// 绘制变量
void parameter::updateParam(const QString &name, double min_value, double max_value, double step_value, double current_value)
{
    qDebug("[%s]: %d", __FUNCTION__, QThread::currentThread());
    bool found = false;

    for(int i=0; i < l->count(); i++) {
        auto *obj = l->itemAt(i)->widget();
        if(obj->objectName() == name) {
            found = true;
            static_cast<viewparameter*>(obj)->setValue(current_value);
            break;
        }
    }
    if(!found) {
        auto *param = new viewparameter(name, min_value, max_value, step_value, current_value);
        l->addWidget(param);
        ui->paramScrollArea->setWidget(paramtmpWidge);
    }
}
