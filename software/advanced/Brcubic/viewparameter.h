#ifndef VIEWPARAMETER_H
#define VIEWPARAMETER_H

#include <QWidget>

namespace Ui {
class viewparameter;
}

class viewparameter : public QWidget
{
    Q_OBJECT



signals:
    void valueChanged(QString obj_name, double value);

public slots:
    void setValue(double value);

public:
    double value() const;


public:
    explicit viewparameter(QString name, double min_value, double max_value, double step_value, double current_value, QWidget *parent = nullptr);
    ~viewparameter();

private:
    Ui::viewparameter *ui;
};

#endif // VIEWPARAMETER_H
