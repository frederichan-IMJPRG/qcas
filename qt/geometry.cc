#include "geometry.h"
#include "giac/dispatch.h"
#include "output.h"
#include <QPainter>
#include <QTreeWidgetItem>
#include <QDebug>
MyItem::MyItem(Canvas2D *graph){
    g2d=graph;
    highLighted=false;
    treeItem=new QTreeWidgetItem;


}
void MyItem::setColor(QColor &c ){
    int r=c.red()/16;
    int g=c.green()/16;
    int b=c.blue()/16;
    int alpha=c.alpha()/16;
    attributes=(0xffff0000&attributes)+(r<<12)+(g<<8)+(b<<4)+alpha;
}

void MyItem::setHighLighted(const bool &b){
    highLighted=b;
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
void MyItem::setWidth(const int w){
     attributes=(attributes & 0xfff8ffff)+ (w<< 16);
}
QString MyItem::getLegend() const{
    return legend;
}
void MyItem::setLegend(const QString &s){
    legend=s;
}
void MyItem::setLabelVisible(const bool b){
    if (b) {
        attributes=std::abs(attributes);
    }
    else attributes=-std::abs(attributes);
}


QTreeWidgetItem * MyItem::getTreeItem(){
    return treeItem;
}

bool MyItem::isUnderMouse(const QRectF& p) const{
    return false;
}
Qt::PenCapStyle MyItem::getPenStyle(){
   int type_line=((attributes & 0x01c00000));
   if (type_line==giac::_CAP_FLAT_LINE) return Qt::FlatCap;
   else if (type_line==giac::_CAP_SQUARE_LINE) return Qt::SquareCap;
   return Qt::RoundCap;

}
Qt::PenStyle MyItem::getLineType(){
    int type_line=((attributes & 0x01c00000));
//    qDebug()<<" type ligne"<<type_line;
    if (type_line==giac::_DASH_LINE) return Qt::DashLine;
    else if (type_line==giac::_DOT_LINE) return Qt::DotLine;
    else if (type_line==giac::_DASHDOT_LINE) return Qt::DashDotLine;
    else if (type_line==giac::_DASHDOTDOT_LINE) return Qt::DashDotDotLine;

    return Qt::SolidLine;

}
int MyItem::getPenWidth(){
    return ((attributes & 0x00070000) >> 16);
}
bool MyItem::labelVisible()const{
    return (attributes<0);
}
void MyItem::setAttributes(const int& c){
    attributes=c;
}


QColor arcenciel(int k){
    int r,g,b;
    k+=21;
    k%=126;
    if (k<0) k+=126;
    if (k<21){
        r=251;g=0;b=12*k;
    }
    if (k>=21 && k<42){
        r=251-(12*(k-21)); g=0; b=251;
      }
      if (k>=42 && k<63){
        r=0; g=(k-42)*12; b=251;
      }
      if (k>=63 && k<84){
        r=0; g=251; b=251-(k-63)*12;
      }
      if (k>=84 && k<105){
        r=(k-84)*12; g=251; b=0;
      }
      if (k>=105 && k<126){
        r=251; g=251-(k-105)*12; b=0;
      }
      return QColor(r,g,b);
}

QColor MyItem::getColor() const{
    int color=(attributes & 0x0000ffff);
    int r=(color>>12)*16;
    int g=((color& 0x0f00)>>8)*16;
    int b=((color&0x00f0)>>4)*16;
    int alpha=255; //(color&0x000f)*16;
    return QColor(r,g,b,alpha);

/*    switch(color){
        case 0:
        {return Qt::black;}
        break;
        case 1:
        {return Qt::red;}
        break;
        case 2:
        {return Qt::green;}
        break;
        case 3:
        {return Qt::yellow;}
       break;
        case 4:
        {return Qt::blue;}
        break;
        case 5:
        {return Qt::magenta;}
        break;
        case 6:
        {return Qt::cyan;}
        break;
        case 7:
        {return Qt::white;}
        break;
    default:
    {
        if (color>0x100&& color<0x17e ){
            return arcenciel(color);

        }

    }
    }
    return Qt::black;*/
}
Point::Point(const int a,const int b,Canvas2D* graph):MyItem(graph){
    x=a;
    y=b;
}
int Point::getPenWidth(){
    return ((attributes& 0x00380000) >> 19);
}
double Point::getX() {
    return x;
}
double Point::getY() {
    return y;
}
bool Point::isUnderMouse(const QRectF& p) const{
    return recSel.intersects(p);
}
QString Point::getType(){
    return QObject::tr("Point");
}


bool Point::isPoint()  const{
    return true;
}

void Point::updateScreenCoords(const bool compute){
    if (compute) g2d->toScreenCoord(x,y,xScreen,yScreen);
    int width=getPenWidth()+3;
    recSel=QRectF(xScreen,yScreen,width,width);
    recSel.adjust(-width/2,-width/2,-width/2,-width/2);

}

void Point::draw(QPainter * painter) const{

    int width=getPenWidth()+3;
    QColor color=getColor();

    if (highLighted) {
        width+=4;
        color.setAlpha(100);
    }

    painter->setPen(QPen(color,width/2.0,Qt::SolidLine,Qt::RoundCap));
    switch(getPointStyle()){
    case giac::_POINT_LOSANGE:
    {
        painter->setBrush(QBrush());
        QPainterPath p;
        p.moveTo(xScreen,yScreen-width);
        p.lineTo(xScreen+width,yScreen);
        p.lineTo(xScreen,yScreen+width);
        p.lineTo(xScreen-width,yScreen);
        p.closeSubpath();
        painter->drawPath(p);}
        break;
    case giac::_POINT_CARRE:
    {
        painter->setBrush(QBrush());
        QPainterPath p;
        width*=0.707;
        p.moveTo(xScreen-width,yScreen-width);
        p.lineTo(xScreen-width,yScreen+width);
        p.lineTo(xScreen+width,yScreen+width);
        p.lineTo(xScreen+width,yScreen-width);
        p.closeSubpath();
        painter->drawPath(p);


    }
        break;
    case giac::_POINT_ETOILE:
 {
        painter->setBrush(QBrush());
        painter->drawLine(QPointF(xScreen,yScreen-width),QPointF(xScreen,yScreen+width));
        painter->drawLine(QPointF(xScreen+0.866*width,yScreen-width/2),QPointF(xScreen-0.866*width,yScreen+width/2));
        painter->drawLine(QPointF(xScreen-0.866*width,yScreen-width/2),QPointF(xScreen+0.866*width,yScreen+width/2));

    }


        break;
    case giac::_POINT_PLUS:
    {
        painter->setBrush(QBrush());
        painter->drawLine(QPointF(xScreen+width,yScreen),QPointF(xScreen-width,yScreen));
        painter->drawLine(QPointF(xScreen,yScreen+width),QPointF(xScreen,yScreen-width));
    }
        break;
    case giac::_POINT_TRIANGLE:
    {
        painter->setBrush(QBrush());
        QPainterPath p;
        p.moveTo(xScreen,yScreen-width);
        p.lineTo(xScreen+width*0.866,yScreen+width/2);
        p.lineTo(xScreen-width*0.866,yScreen+width/2);
        p.closeSubpath();
        painter->drawPath(p);
    }
        break;
    case giac::_POINT_POINT:
    {
        painter->setBrush(QBrush(getColor(),Qt::SolidPattern));
        painter->drawEllipse(QPointF(xScreen,yScreen),width,width);
    }

        break;
    case giac::_POINT_INVISIBLE:
        break;
        // CROSS_POINT
    default:
       {         painter->setBrush(QBrush());

        width*=0.707;
        painter->drawLine(QPointF(xScreen+width,yScreen+width),QPointF(xScreen-width,yScreen-width));
        painter->drawLine(QPointF(xScreen-width,yScreen+width),QPointF(xScreen+width,yScreen-width));

    }
        break;

    }
}
int Point::getPointStyle(){
    return (attributes & 0x0e000000);

}
void Point::setWidth(const int w){
     attributes=(attributes & 0xffc7ffff)+ (w<< 19);
}




LineItem::LineItem(const QPointF& p1,const QPointF& p2,Canvas2D *graph):MyItem(graph){
    startPoint=p1;
    endPoint=p2;
}

void LineItem::updateScreenCoords(const bool compute){
    if (compute) {
    QPointF startScreen, endScreen;
    if (startPoint.x()==endPoint.x()){
        double a,b;
        g2d->toScreenCoord(startPoint.x(),g2d->getYmin(),a,b);
        startScreen=QPointF(a,b);
        g2d->toScreenCoord(startPoint.x(),g2d->getYmax(),a,b);
        endScreen=QPointF(a,b);
    }
    else {
        double tmpx,tmpy;
        double a=(endPoint.y()-startPoint.y())/(endPoint.x()-startPoint.x());
        double b=endPoint.y()-a*endPoint.x();

        double y=a*g2d->getXmax()+b;

        if (y<g2d->getYmin()){
            g2d->toScreenCoord((g2d->getYmin()-b)/a,g2d->getYmin(),tmpx,tmpy);
            endScreen=QPointF(tmpx,tmpy);
        }
        else if (y>g2d->getYmax()){
            g2d->toScreenCoord((g2d->getYmax()-b)/a,g2d->getYmax(),tmpx,tmpy);
            endScreen=QPointF(tmpx,tmpy);

        }
        else {
            g2d->toScreenCoord(g2d->getXmax(),y,tmpx,tmpy);
            endScreen=QPointF(tmpx,tmpy);
        }

        y=a*g2d->getXmin()+b;

        if (y<g2d->getYmin()){
            g2d->toScreenCoord((g2d->getYmin()-b)/a,g2d->getYmin(),tmpx,tmpy);
            startScreen=QPointF(tmpx,tmpy);
        }
        else if (y>g2d->getYmax()){
            g2d->toScreenCoord((g2d->getYmax()-b)/a,g2d->getYmax(),tmpx,tmpy);
            startScreen=QPointF(tmpx,tmpy);

        }
        else {
            g2d->toScreenCoord(g2d->getXmin(),y,tmpx,tmpy);
            startScreen=QPointF(tmpx,tmpy);
        }
    }
    p=QPainterPath();
    p.moveTo(startScreen);
    p.lineTo(endScreen);

    }
    QPainterPathStroker stroke;
    stroke.setWidth(getPenWidth());
    envelop=stroke.createStroke(p);

}
bool LineItem::isLine() const{
    return true;
}

QString LineItem::getType(){
    return QObject::tr("Droite");
}
void LineItem::draw(QPainter* painter) const{
    QColor color=getColor();
    int width=1;
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }
    painter->setPen(QPen(color,width,  getLineType() ,getPenStyle()));
    painter->setBrush(QBrush(color,Qt::SolidPattern));
    painter->drawPath(envelop);
}

bool LineItem::isUnderMouse(const QRectF &r) const{
    return envelop.intersects(r);
}


HalfLineItem::HalfLineItem(const QPointF& p1,const QPointF& p2,Canvas2D *graph):MyItem(graph){
    startPoint=p1;
    endPoint=p2;
}

void HalfLineItem::updateScreenCoords(const bool compute){
    if (compute){
    QPointF startScreen,endScreen;
    if (startPoint.x()==endPoint.x()){
        double a,b;
        g2d->toScreenCoord(startPoint.x(),startPoint.y(),a,b);
        startScreen=QPointF(a,b);
        if (endPoint.y()>startPoint.y()){
            g2d->toScreenCoord(startPoint.x(),g2d->getYmax(),a,b);
        }
        else
            g2d->toScreenCoord(startPoint.x(),g2d->getYmin(),a,b);

        endScreen=QPointF(a,b);
    }
    else {
        double tmpx,tmpy,y;
        g2d->toScreenCoord(startPoint.x(),startPoint.y(),tmpx,tmpy);
        startScreen=QPointF(tmpx,tmpy);

        double a=(endPoint.y()-startPoint.y())/(endPoint.x()-startPoint.x());
        double b=endPoint.y()-a*endPoint.x();


        if (endPoint.x()>startPoint.x()){
            y=a*g2d->getXmax()+b;

            if (y<g2d->getYmin()){
                g2d->toScreenCoord((g2d->getYmin()-b)/a,g2d->getYmin(),tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);
            }
            else if (y>g2d->getYmax()){
                g2d->toScreenCoord((g2d->getYmax()-b)/a,g2d->getYmax(),tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);

            }
            else {
                g2d->toScreenCoord(g2d->getXmax(),y,tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);
            }

        }
        else{

            y=a*g2d->getXmin()+b;

            if (y<g2d->getYmin()){
                g2d->toScreenCoord((g2d->getYmin()-b)/a,g2d->getYmin(),tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);
            }
            else if (y>g2d->getYmax()){
                g2d->toScreenCoord((g2d->getYmax()-b)/a,g2d->getYmax(),tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);

            }
            else {
                g2d->toScreenCoord(g2d->getXmin(),y,tmpx,tmpy);
                endScreen=QPointF(tmpx,tmpy);
            }

        }
    }
    p=QPainterPath();
    p.moveTo(startScreen);
    p.lineTo(endScreen);

    }
    QPainterPathStroker stroke;
    stroke.setWidth(getPenWidth());
    envelop=stroke.createStroke(p);
}
bool HalfLineItem::isHalfLine() const{
    return true;
}

bool HalfLineItem::isUnderMouse(const QRectF &r) const{
    return envelop.intersects(r);
}

void HalfLineItem::draw(QPainter* painter) const{
    QColor color=getColor();
    int width=1;
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }

    painter->setPen(QPen(color,width,  getLineType() ,getPenStyle()));
    painter->setBrush(QBrush(color,Qt::SolidPattern));

    painter->drawPath(envelop);
}
QString HalfLineItem::getType(){
    return QObject::tr("Demie-droite");
}



Curve::Curve(const QPainterPath &p,Canvas2D *graph):MyItem(graph){
    vector=false;
    path=p;
}
void Curve::draw(QPainter *painter) const{
    int width=1;
    QColor color=getColor();
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }

    painter->setBrush(QBrush(color,Qt::SolidPattern));
    painter->setPen(QPen(color,width,  getLineType() ,getPenStyle()));

    painter->drawPath(envelop);
}
bool Curve::isCurve() const{
    return true;
}
bool Curve::isUnderMouse(const QRectF &p) const{
    return envelop.intersects(p);
}

void Curve::setVector(const bool & b){
    vector=b;
}

void Curve::updateScreenCoords(const bool compute){
    if (compute){
        double xtmp,ytmp;
        pathScreen=QPainterPath();
        for (int i=0;i<path.elementCount();++i){
            g2d->toScreenCoord(path.elementAt(i).x,path.elementAt(i).y,xtmp,ytmp);
            if (i==0) pathScreen.moveTo(xtmp,ytmp);
            else pathScreen.lineTo(xtmp,ytmp);
        }
        if (vector){
            double x1=pathScreen.elementAt(0).x;
            double y1=pathScreen.elementAt(0).y;

            double x2=pathScreen.elementAt(1).x;
            double y2=pathScreen.elementAt(1).y;

            double l=std::sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
            int w=6;
            int h=12;
//        QPointF control(x2+(x1-x2)*h/l,y2+(y1-y2)*h/l);
            QPointF second(x2+(x1-x2)*h/l+(y2-y1)*w/l,y2+(y1-y2)*h/l+(x1-x2)*w/l);
            QPointF third(x2+(x1-x2)*h/l-(y2-y1)*w/l,y2+(y1-y2)*h/l-(x1-x2)*w/l);

            pathScreen.lineTo(second);
            pathScreen.lineTo(third);
            pathScreen.lineTo(x2,y2);
        }
    }
    int width=getPenWidth();
    stroke.setWidth(width);
    envelop=stroke.createStroke(pathScreen);

}

bool Curve::isVector() const{
    return vector;
}

bool Curve::isSegment() const{
    if (vector){
        return false;
    }
    else return (path.elementCount()==2);
}

QString Curve::getType(){
    if (vector) return QObject::tr("Vecteur");
    else if (isSegment()) return QObject::tr("Segment");
    return QObject::tr("Courbe");
}




