#include "geometry.h"
#include "giac/dispatch.h"
#include "output.h"
#include <QPainter>
#include <QDebug>
MyItem::MyItem(GraphWidget *graph){
    g2d=graph;
}

bool MyItem::isPoint() const{
    return false;
}
bool MyItem::isLine() const{
    return false;
}
bool MyItem::isSegment() const{
    return false;
}
bool MyItem::isHalfLine() const{
    return false;
}
bool MyItem::isCurve() const{
    return false;
}
bool MyItem::isCircle() const{
    return false;
}
void MyItem::setColor(const QColor & c){
    color=c;
}
void MyItem::setWidth(int w){
    penWidth=w;
}
void MyItem::setLabelPos(const int p){
    labelPos=p;
}

void MyItem::setVisible(const bool b){
    visible=b;
}
Point::Point(const int a,const int b,GraphWidget* graph):MyItem(graph){
    x=a;
    y=b;
}
void Point::setPointWidth(const int w){
    width=w;
}
double Point::getX() {
    return x;
}
double Point::getY() {
    return y;
}
bool Point::isPoint()  const{
    return true;
}

void Point::draw(QPainter * painter) const{
    g2d->toScreenCoord(x,y,xScreen,yScreen);

    switch(style>>25){
    case giac::_POINT_LOSANGE:
        break;
    case giac::_POINT_CARRE:
        break;
    case giac::_POINT_ETOILE:
        break;
    case giac::_POINT_PLUS:
        break;
    case giac::_POINT_TRIANGLE:
        break;
    case giac::_POINT_POINT:
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->setPen(QPen(Qt::black));
        painter->drawEllipse(QPointF(xScreen,yScreen),width,width);
        break;
    case giac::_POINT_INVISIBLE:
        break;
        // CROSS_POINT
    default:
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->setPen(QPen(Qt::black));
        painter->drawEllipse(QPoint(xScreen,yScreen),width,width);

        break;

    }
}
void Point::setPointStyle(const int a ){
    style=a;
}
Curve::Curve(const QPainterPath &p,GraphWidget* graph):MyItem(graph){
    path=p;
    updatePath();
}
void Curve::draw(QPainter *painter) const{
    painter->drawPath(pathScreen);
}
bool Curve::isCurve() const{
    return true;
}
void Curve::updatePath(){
    double xtmp,ytmp;
    pathScreen=QPainterPath();
    for (int i=0;i<path.elementCount();++i){
        g2d->toScreenCoord(path.elementAt(i).x,path.elementAt(i).y,xtmp,ytmp);
        qDebug()<<path.elementAt(i).x<<path.elementAt(i).y<<xtmp<<ytmp;
        if (i==0) pathScreen.moveTo(xtmp,ytmp);
        else pathScreen.lineTo(xtmp,ytmp);
    }
}
