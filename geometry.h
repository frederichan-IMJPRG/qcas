#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <QVector>
#include <QColor>
#include <QPainterPath>
class QPainter;
class GraphWidget;
class MyItem{
public:
  MyItem(GraphWidget*);
  virtual bool isPoint() const;
  virtual bool isLine() const;
  virtual bool isSegment() const;
  virtual bool isHalfLine() const;
  virtual bool isCurve() const;
  virtual bool isCircle() const;
  virtual void draw(QPainter*) const =0 ;
  void setColor(const QColor&);
  void setWidth(const int);
  void setVisible(const bool);
  void setLabelPos(const int );
protected:
  GraphWidget* g2d;
  QColor color;
  int penWidth;
  bool visible;
  int labelPos;
};
class Point:public MyItem{
public:
    Point(const int x,const int y,GraphWidget*);
    double getX();
    double getY();
    virtual bool isPoint() const;
    virtual void draw(QPainter*) const;
    void setPointStyle(const int);
    void setPointWidth(const int);
private:
    double x,y;
    mutable double xScreen,yScreen;
    int width;
    int style;
};

class Curve:public MyItem{
public:
    Curve(const QPainterPath & ,GraphWidget*);
  //  double getX();
  //  double getY();
    void addPath();
    virtual bool isCurve() const;
    virtual void draw(QPainter*) const;
    void updatePath();
//    void setPath(const & QPainterPath);
//    void setPointWidth(const int);
private:
  //  double x,y;
    QPainterPath pathScreen;
    QPainterPath path;
    mutable double xScreen,yScreen;
//    int width;
//    int style;
};




#endif // GEOMETRY_H
