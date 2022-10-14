#include "viewparameter.h"
#include "ui_viewparameter.h"

viewparameter::viewparameter(QString name, double min_value, double max_value, double step_value, double current_value, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::viewparameter)
{
    ui->setupUi(this);


    // 初始化
    setObjectName(name);

    ui->label->setText(name);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum((max_value-min_value) / step_value);
    ui->doubleSpinBox->setMinimum(min_value);
    ui->doubleSpinBox->setMaximum(max_value);
    ui->doubleSpinBox->setSingleStep(step_value);

    // 内部链接 滑动条和数字框联动
    QObject::connect(ui->horizontalSlider, &QSlider::valueChanged, this,
                     [this](int value){ui->doubleSpinBox->setValue(value*ui->doubleSpinBox->singleStep()+ui->doubleSpinBox->minimum());});
    QObject::connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                     [this](double value){ui->horizontalSlider->setValue((value-ui->doubleSpinBox->minimum())/ui->doubleSpinBox->singleStep());});
    // 外部链接
    QObject::connect(ui->doubleSpinBox, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this,
                     [this](double value){emit valueChanged(objectName(), value);});
    // set value
    ui->doubleSpinBox->setValue(current_value);

}

viewparameter::~viewparameter()
{
    delete ui;
}


void viewparameter::setValue(double value)
{
    ui->doubleSpinBox->setValue(value);
}

double viewparameter::value() const
{
    return ui->doubleSpinBox->value();
}




