#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <QVector>
#include <QColor>
#include <QPainterPath>


class QPainter;
class Canvas2D;
class QTreeWidgetItem ;



class MyItem{
public:
  MyItem(Canvas2D*);
  virtual bool isPoint() const;
  virtual bool isLine() const;
  virtual bool isSegment() const;
  virtual bool isHalfLine() const;
  virtual bool isCurve() const;
  virtual bool isCircle() const;
  virtual bool isPixel() const;
  virtual bool isFillable() const;

  virtual void draw(QPainter*) const =0 ;
  virtual void updateScreenCoords(const bool)=0;
  virtual QString getType()const =0 ;
  virtual bool isUnderMouse(const QRectF &p) const;
  QTreeWidgetItem * getTreeItem();
  void setColor(const QColor &) ;
  void setPointStyle(const int );
  void setStyle(const int);
  void setFilled(const bool);
  int getStyle();
  int getQuadrant() const;
  virtual void setWidth(const int);
  virtual int getPenWidth() const;

  void setHighLighted(const bool& );
 void setLegendPos(const int &);


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



protected:
  Canvas2D* g2d;
  unsigned int attributes;
  bool highLighted;
  double angleLegend;
  QString legend;

  private:

    QTreeWidgetItem* treeItem;
};
class Point:public MyItem{
public:
    Point(const double x,const double y,Canvas2D*);
    double getX();
    double getY();
    virtual bool isPoint() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF & p) const;
    virtual void setWidth(const int);
    virtual QString getType() const;
    int getPointStyle() const;
    virtual int getPenWidth() const;

private:
    QRectF recSel;
    double x,y;
    double xScreen,yScreen;
};

class LineItem:public MyItem{
public:
    LineItem(const QPointF&,const QPointF&,Canvas2D*);
    virtual bool isLine() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF& p) const;
    virtual QString getType() const;


private:
    QPointF startPoint;
    QPointF endPoint;
    QPainterPath p;
    QPainterPath envelop;


};


class HalfLineItem:public MyItem{
public:
    HalfLineItem(const QPointF&,const QPointF&,Canvas2D*);
    virtual bool isHalfLine() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool );
    virtual QString getType() const;

    virtual bool isUnderMouse(const QRectF& p) const;

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

private:
    bool vector;
    bool polygon;
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

private:
    QPointF pixel;
    QPointF pixelScreen;
};



#endif // GEOMETRY_H
