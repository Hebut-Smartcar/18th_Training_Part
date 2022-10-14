#ifndef LOGOSHOW_H
#define LOGOSHOW_H

#include <QWidget>
#include <QPainter>
namespace Ui {
class Logoshow;
}

class Logoshow : public QWidget
{
    Q_OBJECT

public:
    explicit Logoshow(QWidget *parent = 0);
    ~Logoshow();
    void paintEvent(QPaintEvent *event);

private:
    Ui::Logoshow *ui;
};

#endif // LOGOSHOW_H
