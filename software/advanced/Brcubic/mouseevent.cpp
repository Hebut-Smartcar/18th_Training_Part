#include "mouseevent.h"
#include <QString>
MouseEvent::MouseEvent(QWidget *parent)
:QLabel(parent)
{

}

MouseEvent::~MouseEvent( )
{

}

void MouseEvent::mousePressEvent(QMouseEvent *e)
{

}

void MouseEvent::mouseMoveEvent(QMouseEvent *e)
{
    QPointF position = e->localPos();//获取光标位置
    emit getPosition(position);//发射信号
}

void MouseEvent::mouseReleaseEvent(QMouseEvent *e)
{

}
