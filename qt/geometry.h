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
  virtual bool isPointElement() const;
  virtual bool isLine() const;
  virtual bool isSegment() const;
  virtual bool isVector() const;
  virtual bool isHalfLine() const;
  virtual bool isCurve() const;
  virtual bool isBezierCurve() const;
  virtual bool isInter() const;
  virtual bool isList() const;
  virtual bool isAngleItem() const;
  virtual bool isCircle() const;
  virtual bool isPixel() const;
  virtual bool isFillable() const;
  virtual bool isLegendItem() const;
  virtual bool isUndef() const; 
  void setUndef(const bool& );
  virtual void setValue(const giac::gen & ) ;
  virtual void updateValueFrom(MyItem*);
  virtual QString getDisplayValue();
  bool isMovable() const;
  void setMovable(const bool &);
  bool isFromInter() const;
  void setFromInter(const bool &);
  bool isTraceActive() const;
  virtual void setTraceActive(const bool &);

  virtual void draw(QPainter*) const =0 ;

  virtual void updateScreenCoords(const bool)=0;
  virtual QString getType()const =0 ;
  virtual bool isUnderMouse(const QRectF &p) const;
  virtual void drawTrace(QPainter*);

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
  MyItem* getChildAt(const int&);
  bool hasChildren() const;
  void addChild(MyItem * );
  void deleteChild(MyItem*);

  QVector<MyItem*>  getParents();
  MyItem* getParentAt(const int&);
  bool hasParents() const;
  void addParent(MyItem * );


  void setHighLighted(const bool& );
  void setLegendPos(const int &);
   giac::gen &getValue();
   void setVar(const QString &);
   QString getVar() const;
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
  bool undef;

private:
  bool traceActive;
  bool fromInter;
  bool movable;
  int level;
  QString var;
  QVector<MyItem*> children;
  QVector<MyItem*> parents;
};
class Point:public MyItem{
public:
    Point(Canvas2D*);
    Point(const giac::gen &,Canvas2D *graph );
   virtual void updateValueFrom(MyItem *);
    virtual bool isPoint() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF & p) const;
    virtual void setWidth(const int);
    virtual void setTraceActive(const bool &);
    virtual QString getType() const;
    virtual void drawTrace(QPainter *);
    int getPointStyle() const;
    virtual int getPenWidth() const;
    virtual void setValue(const giac::gen & );
    virtual QString  getDisplayValue();
protected:
    double x,y;

private:
    QRectF recSel;
    double xScreen,yScreen;
    // To store trace
    QList<QPointF> tracePoints;

    friend class PointElement;
};
class PointElement:public Point{
public:
    PointElement(Point*,Canvas2D *graph );
    QPointF getOrigin() const;
    void setorigin(const QPointF &);
    QString getTranslation(const QPointF &);
    virtual bool isPointElement() const;

private:
    QPointF origin;
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
    virtual bool isSegment() const;
    virtual bool isVector() const;
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
    QPainterPath pathArrow;
};
class BezierCurve:public MyItem{
public:
    BezierCurve(const QList<QPointF> & , Canvas2D*);
//    void addPath();
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isBezierCurve() const;
    virtual void draw(QPainter*) const;
    virtual QString getType() const;
    virtual bool isFillable() const;
    void setFillable(const bool &);
    virtual QString getDisplayValue();
    virtual void updateValueFrom(MyItem *);
    QList <QPointF> getControlPoints() const;
//    QPainterPath getPath() const;
private:
    bool fillable;
    QPainterPathStroker stroke;
    QList<QPointF> pointsScreen;
    QList<QPointF> points;

    QPainterPath bezierCurve;
    QPainterPath envelop;
//    QPainterPath pathArrow;
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
    virtual void updateValueFrom(MyItem*);
    QPointF getCenter() const;
    double getDiametre() const;
    double getStartAngle() const;
    double getEndAngle() const;
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
class UndefItem:public MyItem{
public:
    UndefItem(Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual QString getType() const;
    virtual bool isUndef() const;

};

class InterItem:public MyItem{
public:
    InterItem(const bool &, Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual bool isInter() const;
    virtual QString getType() const;

private:
    bool tangent;
};

class AngleItem:public MyItem{
public:
    AngleItem(Canvas2D*);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual void updateScreenCoords(const bool);
    virtual void draw(QPainter*) const;
    virtual bool isAngleItem() const;
    virtual QString getType() const;
    void setCircle(MyItem*);
    void setCurve(MyItem*);
    MyItem* getCircle();
    MyItem* getCurve();
    virtual QString getDisplayValue();
private:
    MyItem* arc;
    MyItem* curve;

};

#endif // GEOMETRY_H
