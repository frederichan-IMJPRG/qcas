/*  Copyright (C) 2001 Le Coq Loïc
*    This file is part of QCAS.
*
*    QCAS is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    QCAS is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with Foobar.  If not, see <http://www.gnu.org/licenses/>
*/


#include "geometry.h"
#include "giac/dispatch.h"
#include "output.h"
#include <QPainter>
#include <QTreeWidgetItem>
#include <QDebug>
MyItem::MyItem(Canvas2D *graph){
    g2d=graph;
    visible=true;
    highLighted=false;
    angleLegend=-1;
    treeItem=new QTreeWidgetItem;
    value="";
    level=-1;
}

void MyItem::setHighLighted(const bool &b){
    highLighted=b;
}
QString  MyItem::getValue() const{
    return value;

}
void MyItem::setValue(const QString & s){
    value=s;
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
bool MyItem::isPixel() const{
        return false;
}
bool MyItem::isLegendItem()const{
    return false;
}
bool MyItem::isFillable() const{
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
void MyItem::setLevel(const int & i){
    level=i;
}
int MyItem::getLevel() const{
    return level;
}

bool MyItem::isVisible() const{
    return visible;
}
void MyItem::setVisible(const bool b){
    visible=b;
}
void MyItem::setLegendVisible(const bool b){
    if (b) {
        attributes=(attributes & 0x7fffffff);
    }
    else attributes=(attributes & 0x7fffffff)+(1<<31);
}

void MyItem::setPointStyle(const int c){
    attributes=(attributes& 0xf1ffffff)+(c<<25);
}

QTreeWidgetItem * MyItem::getTreeItem(){
    return treeItem;
}

bool MyItem::isUnderMouse(const QRectF& p) const{
    return false;
}
void MyItem::setStyle(const int c) {
    attributes=(attributes& 0xfe3fffff)+(c<<22);

}
void MyItem::setFilled(const bool b){
    if (b) attributes=(attributes& 0xbfffffff)+(1<<30);
    else attributes=(attributes& 0xbfffffff);
}
int MyItem::getQuadrant() const{
    return ((attributes & 0x30000000)>>28);
}
double MyItem::getAngleLegend() const{
    if (angleLegend==-1){
        return getQuadrant()*3.14159/2+3.14159/4;
    }
    else return angleLegend;
}
void MyItem::setLegendPos(const int & a){
    attributes=(attributes & 0xcfffffff)+(a<<28);
//    qDebug()<<"Attributs"<<legendVisible()<<getQuadrant();

}

int MyItem::getStyle(){
    return (attributes & 0x01c00000)>>22;
}
bool MyItem::isFilled()const{
    if (1==((attributes & 0x40000000)>>30)) return true;
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
int MyItem::getPenWidth() const{
    return ((attributes & 0x00070000) >> 16);
}
bool MyItem::legendVisible()const{
    if ((attributes>>31)==1) return false;
    return true;
}
void MyItem::setAttributes(const int& c){
    attributes=(unsigned int)c;
    if (isFilled()){
        QColor c=getColor();
        c.setAlpha(36*4);
        setColor(c);
    }

}

QColor MyItem::getFltkColor(int& c) const{
    if (c<16){
        switch(c){
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
            case 8:
             {return QColor(85,85,85);} // 1/3 gray
             break;
        case 9:
         {return QColor(198,113,113);}// salmon? pale red?
         break;
        case 10:
         {return QColor(113,198,113);}// pale green
         break;
        case 11:
         {return QColor(142,142,56);}// khaki
         break;
        case 12:
         {return QColor(113,113,198);}// pale blue
         break;
        case 13:
         {return QColor(142,56,142);}// purple, orchid, pale magenta
         break;
        case 14:
         {return QColor(56,142,142);}// cadet blue, aquamarine, pale cyan
         break;
        case 15:
         {return QColor(170,170,170);}// cadet blue, aquamarine, pale cyan
         break;
        }
    }
    else if (c<32){
        return QColor(85,85,85);
    }
    else if (c<56){
        // initial algorith to populate the table (from fltk source)
        //
           /*

        #define FL_GRAY_RAMP 32
        #define FL_NUM_GRAY  24
        #define FL_GRAY 49

        if (!r) r = 1; else if (r==255) r = 254;
        double powr = log(254/255.0)/log((FL_GRAY-FL_GRAY_RAMP)/(FL_NUM_GRAY-1.0));
        if (!g) g = 1; else if (g==255) g = 254;
        double powg = log(g/255.0)/log((FL_GRAY-FL_GRAY_RAMP)/(FL_NUM_GRAY-1.0));
        if (!b) b = 1; else if (b==255) b = 254;
        double powb = log(b/255.0)/log((FL_GRAY-FL_GRAY_RAMP)/(FL_NUM_GRAY-1.0));
        for (int i = 0; i < FL_NUM_GRAY; i++) {
          double gray = i/(FL_NUM_GRAY-1.0);
          cmap[i+FL_GRAY_RAMP][0] = uchar(pow(gray,powr)*255+.5);
          cmap[i+FL_GRAY_RAMP][1] = uchar(pow(gray,powg)*255+.5);
          cmap[i+FL_GRAY_RAMP][2] = uchar(pow(gray,powb)*255+.5);
        }
        */
        if (c==39) return QColor(85,85,85);
        else if(c==47) return QColor(170,170,170);
        else{
            int r = 170;
            double powr = log(r/255.0)/log(17/23);
            int g=170;
            double powg = log(g/255.0)/log(17/23);
            int b=170;
            double powb = log(b/255.0)/log(17/23);
            c-=32;
            return QColor(uchar(pow(c/23.0,powr)*255+.5),uchar(pow(c/23.0,powg)*255+.5),uchar(pow(c/23.0,powb)*255+.5));
        }
    }
    else if (c<256){
       // initial algorith to populate the table (from fltk source)
       /*
        i = 56;
         for (b=0; b<5; b++)
           for (r=0; r<5; r++)
             for (g=0; g<8; g++) {
           cmap[i][0] = r*255/4;
           cmap[i][1] = g*255/7;
           cmap[i][2] = b*255/4;
           i++;
             }*/
        c-=56;
        int b=c/40;
        int r=(c%40)/5;
        int g=(c%40)%8;
//        qDebug()<<"couleurs composante"<<r<<g<<b;
        return QColor(255*r/4,255*g/7,255*b/4);

    }

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
void MyItem::setColor(const QColor &c ){
    int r=c.red()/16;
    int g=c.green()/16;
    int b=c.blue()/16;
    int alpha=c.alpha()/36;

    attributes=(0xffff0000&attributes)+(1<<15)+(alpha<<12)+(r<<8)+(g<<4)+b;
}

QColor MyItem::getColor() const{
    int color=(attributes & 0x0000ffff);
    if (0==(color>>15)){
        if (color<256){
            QColor tmp=getFltkColor(color);
          //  setColor(tmp);
            return tmp;
        }
        else if (color<0x17e ){
            QColor tmp=arcenciel(color);
            //setColor(tmp);
            return tmp;

        }
    }
    int r=((color& 0x0f00)>>8)*16;
    int g=((color&0x00f0)>>4)*16;
    int b=(color&0x000f)*16;
    int alpha=((color& 0x7000)>>12)*36; //(color&0x000f)*16;
    return QColor(r,g,b,alpha);
}
Point::Point(const double a,const double b,Canvas2D* graph):MyItem(graph){
    x=a;
    y=b;
}
int Point::getPenWidth() const{
    return ((attributes& 0x00380000) >> 19);
}
/*double Point::getX() {
    return x;
}
double Point::getY() {
    return y;
}*/
bool Point::isUnderMouse(const QRectF& p) const{
    return recSel.intersects(p);
}
QString Point::getType() const{
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
    if (!isVisible()) return;
    int width=getPenWidth()+3;
    QColor color=getColor();

    if (highLighted) {
        width+=4;
        color.setAlpha(100);
    }
//    qDebug()<<attributes <<legend;
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
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->drawEllipse(QPointF(xScreen,yScreen),width-2,width-2);
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

    if (legendVisible()){
        if (legend.trimmed().isEmpty()) return;
        int h=painter->fontMetrics().ascent();
        int w=painter->fontMetrics().width(legend);
        double angle=getAngleLegend();

        QPointF p(xScreen+10*std::cos(angle),yScreen-10*sin(angle));
        if ((0<angle)&&(angle<3.14159/2)){
            painter->drawText(p.x(),p.y(),legend);
        }
        else if ((3.14159/2<=angle)&&(angle<3.14159)){
            painter->drawText(p.x()-w,p.y(),legend);

        }
        else if ((3.14159<=angle)&&(angle<3*3.14159/2)){
            painter->drawText(p.x()-w,p.y()+h,legend);

        }
        else{
            painter->drawText(p.x(),p.y()+h,legend);

        }

    }
}


int Point::getPointStyle() const{
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
    stroke.setCapStyle(Qt::FlatCap);
    stroke.setDashPattern(getLineType());
    stroke.setWidth(getPenWidth()+1);
    envelop=stroke.createStroke(p);

}
bool LineItem::isLine() const{
    return true;
}

QString LineItem::getType() const{
    return QObject::tr("Droite");
}
void LineItem::draw(QPainter* painter) const{
    if (!isVisible()) return;

    QColor color=getColor();
    int width=1;
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }
    painter->setPen(QPen(color,width,  Qt::SolidLine));
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
    stroke.setWidth(getPenWidth()+1);
    stroke.setCapStyle(Qt::FlatCap);
    stroke.setDashPattern(getLineType());
    envelop=stroke.createStroke(p);
}
bool HalfLineItem::isHalfLine() const{
    return true;
}

bool HalfLineItem::isUnderMouse(const QRectF &r) const{
    return envelop.intersects(r);
}

void HalfLineItem::draw(QPainter* painter) const{
    if (!isVisible()) return;

    QColor color=getColor();
    int width=1;
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }

    painter->setPen(QPen(color,width,  Qt::SolidLine));
    painter->setBrush(QBrush(color,Qt::SolidPattern));

    painter->drawPath(envelop);
}
QString HalfLineItem::getType() const{
    return QObject::tr("Demie-droite");
}



Curve::Curve(const QPainterPath &p,Canvas2D *graph):MyItem(graph){
    vector=false;
    polygon=false;
    path=p;
    fillable=false;
}
void Curve::draw(QPainter *painter) const{
    if (!isVisible()) return;

    int width=1;
    QColor color=getColor();
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }
    if (isFilled()&&isFillable()&&(!highLighted)){
        QColor f(color);
        f.setAlpha(255);
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->setPen(QPen(f,getPenWidth(),  Qt::SolidLine));
        painter->drawPath(pathScreen);
    }
    else {
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->setPen(QPen(color,width,  Qt::SolidLine));
        painter->drawPath(envelop);
    }

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
    int width=getPenWidth()+1;
    stroke.setCapStyle(Qt::FlatCap);
    stroke.setDashPattern(getLineType());
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

QString Curve::getType() const{
    if (vector) return QObject::tr("Vecteur");
    else if (isSegment()) return QObject::tr("Segment");
    else if (polygon) return QObject::tr("Polygône");
    return QObject::tr("Courbe");
}
bool Curve::isPolygon() const{
    return polygon;
}
bool Curve::isFillable() const{
    return (polygon&&fillable);
}
void Curve::setFillable(const bool & b){
    fillable=b;
}

void Curve::setPolygon(const bool & b){
    polygon=b;
}

Circle::Circle(const QPointF &p, const double &d, const double &st, const double &end, Canvas2D *graph):MyItem(graph){
    center=p;
    diametre=d;
    startAngle=st*180/3.14159;
    endAngle=end*180/3.14159;



}

bool Circle::isCircle() const{
    return true;
}
void Circle::updateScreenCoords(const bool compute){

    if (compute){
        double a,b;
        double r=diametre/2;
        g2d->toScreenCoord(center.x()-r,center.y()+r,a,b);
        QPointF leftUp(a,b);
        g2d->toScreenCoord(center.x()+r,center.y()-r,a,b);
        QPointF rightBottom(a,b);
        g2d->toScreenCoord(center.x()+r*std::cos(startAngle/180*3.14159),center.y()+r*std::sin(startAngle/180*3.14159),a,b);
        QPointF start(a,b);

        p=QPainterPath();
        p.moveTo(start);
        p.arcTo(QRectF(leftUp,rightBottom),startAngle,endAngle-startAngle);
    }
    QPainterPathStroker stroke;
    stroke.setWidth(getPenWidth()+1);
    stroke.setCapStyle(Qt::FlatCap);
    stroke.setDashPattern(getLineType());
    envelop=stroke.createStroke(p);
}


void Circle::draw(QPainter* painter) const{
    if (!isVisible()) return;
    QColor color=getColor();
    int width=1;
    if (highLighted){
        color.setAlpha(100);
        width=3;
    }
    if (isFilled()&&(!highLighted)){
        QColor f(color);
        f.setAlpha(255);
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->setPen(QPen(f,width,  Qt::SolidLine));
        painter->drawPath(p);
    }
    else{
        painter->setPen(QPen(color,width,  Qt::SolidLine));
        painter->setBrush(QBrush(color,Qt::SolidPattern));
        painter->drawPath(envelop);
    }
}
bool Circle::isFillable() const{
    return true;
}
QString Circle::getType() const{
    return QObject::tr("Cercle");
}

bool Circle::isUnderMouse(const QRectF &p) const{
    return envelop.intersects(p);
}
Pixel::Pixel(const QPointF &p, Canvas2D * parent):MyItem(parent){
    pixelScreen=p;
    double x,y;
    g2d->toXY(p.x(),p.y(),x,y);
    pixel=QPointF(x,y);

}
bool Pixel::isUnderMouse(const QRectF &p) const{
    return false;
}

QString Pixel::getType() const{
    return QObject::tr("Pixel");
}
bool Pixel::isPixel() const{
    return true;
}

void Pixel::updateScreenCoords(const bool compute){
    if (compute){
        double x,y;
        g2d->toScreenCoord(pixel.x(),pixel.y(),x,y);
        pixelScreen=QPointF(x,y);

    }
}

void Pixel::draw(QPainter * p) const{
    if (!isVisible()) return;
    p->setPen(getColor());
    p->drawPoint((int)pixelScreen.x(),(int)pixelScreen.y());

}

LegendItem::LegendItem(const QPoint & p,const QString & l, Canvas2D *parent):MyItem(parent){
    legend=l;
    pos=p;
}

bool LegendItem::isUnderMouse(const QRectF& p) const{
    return false;
}
void LegendItem::updateScreenCoords(const bool){}

void LegendItem::draw(QPainter* painter) const{
    if (!isVisible()) return;

    QColor color=getColor();

    painter->setPen(QPen(color,2.0,Qt::SolidLine,Qt::RoundCap));



    if (legend.trimmed().isEmpty()) return;
    int h=painter->fontMetrics().ascent();
    int w=painter->fontMetrics().width(legend);
    double angle=getAngleLegend();

    QPointF p(pos.x()+10*std::cos(angle),pos.y()-10*sin(angle));
    if ((0<angle)&&(angle<3.14159/2)){
        painter->drawText(p.x(),p.y(),legend);
    }
    else if ((3.14159/2<=angle)&&(angle<3.14159)){
        painter->drawText(p.x()-w,p.y(),legend);

    }
    else if ((3.14159<=angle)&&(angle<3*3.14159/2)){
        painter->drawText(p.x()-w,p.y()+h,legend);

    }
    else{
        painter->drawText(p.x(),p.y()+h,legend);

    }

}
bool LegendItem::isLegendItem() const{
    return true;
}
QString LegendItem::getType() const{
    return QObject::tr("Légende");
}
