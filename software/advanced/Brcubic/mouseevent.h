#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

#include <QMainWindow>
#include <QLabel>
#include <QMouseEvent>
#include <QStatusBar>
#include <QListWidget>
class MouseEvent:public QLabel
{
    Q_OBJECT
    public:
        MouseEvent(QWidget *parent=0);
        ~MouseEvent();
    protected:
        virtual void mousePressEvent(QMouseEvent *e);
        virtual void mouseMoveEvent(QMouseEvent *e);
        virtual void mouseReleaseEvent(QMouseEvent *e);
    signals:
        void getPosition(QPointF pt);
    private:
        QLabel *mouseStatus;
};

#endif // MOUSEEVENT_H
