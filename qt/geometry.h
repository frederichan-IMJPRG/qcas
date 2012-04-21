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

  virtual void draw(QPainter*) const =0 ;
  virtual void updateScreenCoords(const bool)=0;
  virtual QString getType()=0;
  virtual bool isUnderMouse(const QRectF &p) const;
  QTreeWidgetItem * getTreeItem();
  void setColor(QColor &);
  virtual void setWidth(const int);
  virtual int getPenWidth();

  void setHighLighted(const bool& );
  void setLabelVisible(const bool);
//  void setLabelPos(const int );


  Qt::PenCapStyle getPenStyle();
  Qt::PenStyle getLineType();
  void setAttributes(const int &);
  QColor getColor() const;
  bool labelVisible() const;
  void setLegend(const QString &) ;
  QString getLegend() const;



protected:
  Canvas2D* g2d;
  int attributes;
    bool highLighted;
  private:

    QString legend;
    QTreeWidgetItem* treeItem;
};
class Point:public MyItem{
public:
    Point(const int x,const int y,Canvas2D*);
    double getX();
    double getY();
    virtual bool isPoint() const;
    virtual void draw(QPainter*) const;
    virtual void updateScreenCoords(const bool);
    virtual bool isUnderMouse(const QRectF & p) const;
    virtual void setWidth(const int);
    virtual QString getType();
    int getPointStyle();
    virtual int getPenWidth();

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
    virtual QString getType();


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
    virtual QString getType();

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
    virtual QString getType();

    void setVector(const bool&);
    bool isSegment() const;
    bool isVector() const;

private:
    bool vector;
    QPainterPathStroker stroke;
    QPainterPath pathScreen;
    QPainterPath path;
    QPainterPath envelop;
//    mutable double xScreen,yScreen;

};



#endif // GEOMETRY_H
