#ifndef PARAMETER_H
#define PARAMETER_H

#include <QWidget>

#include "ui_parameter.h"
#include <QIcon>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "viewparameter.h"
#include <qthread.h>

namespace Ui {
class parameter;
}

class parameter : public QWidget
{
    Q_OBJECT

public slots:
        void AnalyzeparamData(QByteArray data);

        void updateParam(const QString &name, double min_value, double max_value, double step_value, double current_value);

public:
         QWidget * paramtmpWidge;
         QVBoxLayout * l;


public:
    explicit parameter(QWidget *parent = nullptr);
    ~parameter();

private:
    Ui::parameter *ui;
};

#endif // PARAMETER_H
