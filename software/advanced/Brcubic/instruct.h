#ifndef INSTRUCT_H
#define INSTRUCT_H

#include <QWidget>

namespace Ui {
class Instruct;
}

class Instruct : public QWidget
{
    Q_OBJECT

public:
    explicit Instruct(QWidget *parent = 0);
    ~Instruct();

private:
    Ui::Instruct *ui;
};

#endif // INSTRUCT_H
