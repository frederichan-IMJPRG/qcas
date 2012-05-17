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

#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <QVector>
#include <QColor>
#include <QPainterPath>
#include "../giac/gen.h"
#include "tex.h"

class QPainter;
class Canvas2D;
class QTreeWidgetItem ;
//class FormulaWidget;


class MyItem{
public:
  MyItem(Canvas2D*);
  virtual ~MyItem();
  virtual bool isPoint() const;
  virtual bool isLine() const;
  virtual bool isSegment() const;
  virtual bool isHalfLine() const;
  virtual bool isCurve() const;
  virtual bool isList() const;
  virtual bool isCircle() const;
  virtual bool isPixel() const;
  virtual bool isFillable() const;
  virtual bool isLegendItem() const;
  virtual void setValue(const giac::gen & ) ;
  virtual void updateValueFrom(MyItem*);
  virtual QString getDisplayValue();
  bool isMovable() const;
  void setMovable(const bool &);
  virtual void draw(QPainter*) const =0 ;
  virtual void updateScreenCoords(const bool)=0;
  virtual QString getType()const =0 ;
  virtual bool isUnderMouse(const QRectF &p) const;
  void setColor(const QColor &) ;
  void setPointStyle(const int );
  void setStyle(const int);
  void setFilled(const bool);
  bool isVisible()const;
  void setVisible(const bool);
  int getStyle();
  int getQuadrant() const;
  virtual void setWidth(const int);
  virtual int getPenWidth() const;
  QVector<MyItem*>  getChildren();
  bool hasChildren() const;
  void addChild(MyItem * );
  void setHighLighted(const bool& );
  void setLegendPos(const int &);
   giac::gen &getValue();
   void setCommand(const QString &);
   QString getCommand() const;
//   virtual FormulaWidget* getDisplayWidget();

  Qt::PenCapStyle getPenStyle();
  Qt::PenStyle getLineType();
  void setAttributes(const int &);
  bool isFilled() const;
  QColor getColor() const;
  QColor getFltkColor(int &c) const;
  double getAngleLegend() const;
  bool legendVisible() const;
  void setLegendVisible(const bool);
  void setLegend(const QString &) ;
  QString getLegend() const;
  int getLevel() const;
  void setLevel(const int&);


protected:
  Canvas2D* g2d;
  unsigned int attributes;
  giac::gen value;
  bool highLighted;
  double angleLegend;
  QString legend;
  bool visible;
private:
  bool movable;
  int level;

  QString command;
  QVector<MyItem*> children;
};
class Point:public MyItem{
public:
    Point(const giac::gen &,Canvas2D *graph );
   virtual void updateValueFrom(MyItem *);
    virtual bool isPoint() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF & p) const;
    virtual void setWidth(const int);
    virtual QString getType() const;
    int getPointStyle() const;
    virtual int getPenWidth() const;
    virtual void setValue(const giac::gen & );
    virtual QString  getDisplayValue();



private:
    QRectF recSel;
    double x,y;
    double xScreen,yScreen;
};

class LineItem:public MyItem{
public:
    LineItem(const QPointF &, const QPointF &, Canvas2D*);
    virtual bool isLine() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void setValue(const giac::gen &);
    virtual QString getType() const;
    virtual QString getDisplayValue();
    virtual void updateValueFrom(MyItem *);

    QPointF getStartPoint() const;
    QPointF getEndPoint() const;

private:
    QPointF startPoint;
    QPointF endPoint;
    QPainterPath p;
    QPainterPath envelop;


};


class HalfLineItem:public MyItem{
public:
    HalfLineItem(const QPointF &, const QPointF &, Canvas2D*);
    virtual bool isHalfLine() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool );
    virtual QString getType() const;
    virtual QString getDisplayValue();
    virtual void setValue(const giac::gen &);
    virtual void updateValueFrom(MyItem *);
    virtual bool isUnderMouse(const QRectF& p) const;
    QPointF getStartPoint() const;
    QPointF getEndPoint() const;

private:
    QPointF startPoint;
    QPointF endPoint;
    QPainterPath p;
    QPainterPath envelop;

//    QPointF startScreen,endScreen;

};

class Curve:public MyItem{
public:
    Curve(const QPainterPath & ,Canvas2D*);
    void addPath();
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isCurve() const;
    virtual void draw(QPainter*) const;
    virtual QString getType() const;
    void setPolygon(const bool&);
    void setVector(const bool&);
    bool isSegment() const;
    bool isVector() const;
    bool isPolygon() const;
    virtual bool isFillable() const;
    void setFillable(const bool &);
    virtual QString getDisplayValue();
    virtual void updateValueFrom(MyItem *);
    QPainterPath getPath() const;
private:
    bool vector;
    bool polygon;
    bool fillable;
    QPainterPathStroker stroke;
    QPainterPath pathScreen;
    QPainterPath path;
    QPainterPath envelop;
};


class Circle:public MyItem{
public:
    Circle(const QPointF & ,const double&,const double& ,const double   & , Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isCircle() const;
    virtual void draw(QPainter*) const;
    virtual QString getType() const;
    virtual bool isFillable() const;
    virtual QString getDisplayValue();
private:
    QPointF center;
    double diametre;
    double startAngle;
    double endAngle;
    QPainterPath p;
    QPainterPath envelop;
};

class Pixel:public MyItem{
public:
    Pixel(const QPointF & , Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual bool isPixel() const;
    virtual QString getType() const;
    virtual bool isFillable() const;
private:
    QPointF pixel;
    QPointF pixelScreen;
};
class LegendItem:public MyItem{
public:
    LegendItem(const QPoint & ,const QString &, Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual bool isLegendItem() const;
    virtual QString getType() const;
private:
    QPointF pos;
};
class ListItem:public MyItem{
public:
    ListItem(const QList<MyItem*> & , Canvas2D*);
    ~ListItem();
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual bool isList() const;
    virtual QString getType() const;
    virtual bool isFillable() const;

private:
    QList<MyItem*> list;
};

#endif // GEOMETRY_H
