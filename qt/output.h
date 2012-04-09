#ifndef OUTPUT_H
#define OUTPUT_H
#include <QWidget>
#include "geometry.h"
#include "giac/gen.h"
#include <QVector>
class Line;
class QtMmlWidget;
class OutputWidget:public QWidget{
public:
    OutputWidget(QWidget* widget=0);
    void setLine(Line* );
private:
    Line* line;
};
class FormulaWidget :public OutputWidget{
public:
    FormulaWidget(QtMmlWidget*);
};
class GraphWidget:public OutputWidget{
public:
    GraphWidget(const  giac::gen &, giac::context*);
    void toScreenCoord(const double,const double,double& , double&);
    QSize sizeHint() const;
protected:
    void paintEvent(QPaintEvent *);
private:
    QVector<MyItem*> lineItems;
    QVector<MyItem*> pointItems;
    QVector<MyItem*> filledItems;
    void setXYUnit();
    double xmin,xmax,ymin,ymax,xunit,yunit,zoom;
    double find_tick(double);
    void createScene(const giac::gen &,giac::context*);
    void drawAxes();
    void drawGrid();
    void drawElements(QVector<MyItem*> &, QPainter*);
};



#endif // OUTPUT_H
