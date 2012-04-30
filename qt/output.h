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

#ifndef OUTPUT_H
#define OUTPUT_H
#include "geometry.h"
#include "giac/gen.h"
#include <QWidget>
#include <QVector>
#include <QDomElement>
#include <QPixmap>
#include <QScrollArea>
#include <QHash>
#include <QGroupBox>

class QMenu;
class QHBoxLayout;
class Line;
class QComboBox;
class QVBoxLayout;
class QPushButton;
class QCheckBox;
class QLineEdit;
class PanelProperties;
class DisplayProperties;
class QTreeWidget;
class QSlider;
class QSpinBox;
class QtMmlWidget;
class WidthPanel;
class TypePointPanel;
class TypeLinePanel;
class ColorPanel;
class LegendPanel;
class AlphaFillPanel;
class OutputWidget:public QWidget{
public:
    OutputWidget(QWidget* widget=0);
    void setLine(Line* );
//    virtual void toXML(QDomElement&);

private:
    Line* line;
};
class FormulaWidget :public OutputWidget{
public:
    FormulaWidget(QtMmlWidget*);
    virtual void toXML(QDomElement&);

};

// This widget is used for rendering 2D graphics

class GraphWidget:public OutputWidget{
    Q_OBJECT

  public:
    GraphWidget(const  giac::gen &, giac::context*);
    QList<MyItem*> getTreeSelectedItems();
    void clearSelection();

 private:
    Canvas2D* canvas;
    PanelProperties * propPanel;

    void initGui();
};




// The canvas to draw 2D graphics

class Canvas2D:public QWidget{
    Q_OBJECT
public:
    Canvas2D(GraphWidget* g2d,const  giac::gen &, giac::context*);
    void toScreenCoord(const double,const double,double& , double&);
    void toXY(const double,const double,double& , double&);
    virtual void toXML();
    QSize sizeHint() const;
    double getXmin() const;
    double getXmax() const ;
    double getYmin() const;
    double getYmax() const;
    QVector<MyItem*>* getPointItem();
    QVector<MyItem*>* getLineItem();
    QVector<MyItem*>* getFilledItem();
    void setFocusOwner(MyItem*);
    void updatePixmap(bool compute);


protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:

    MyItem* focusOwner;
    // x,y range and units
    double xmin,xmax,ymin,ymax,xunit,yunit;
    // vectors to store geometry items
    QVector<MyItem*> lineItems;
    QVector<MyItem*> pointItems;
    QVector<MyItem*> filledItems;

    QMenu * menuGeneral;
    QAction* zoomIn;
    QAction* zoomOut;
    QAction* orthoAction;

    GraphWidget *parent;
    // To draw a rectangle selection
    bool selection;
    QPoint startSel;
    QPoint endSel;
    // The Pixmap on which all Items are drawn.
    QPixmap pixmap;


    void setXYUnit();
    void createScene(const giac::gen &,giac::context*);
    double find_tick(double);
    void drawAxes(QPainter*);
    void drawGrid(QPainter*);
    void drawElements(QVector<MyItem*> &, QPainter*, bool compute);
    bool checkUnderMouse(QVector<MyItem*>* v, const QPointF &);

private slots:
    void zoom_In();
    void zoom_Out();
    void make_ortho();
    //    void zoom_Factor(const int&);
};

class PanelProperties:public QWidget{
    Q_OBJECT
public:
    PanelProperties(Canvas2D *);
    QList<MyItem*> getTreeSelectedItems();
    void clearSelection();
private:
    Canvas2D* parent;
    QTreeWidget* tree;
    QTreeWidgetItem* nodePoint;
    QTreeWidgetItem* nodeCurve;
    QTreeWidgetItem* nodeVector;
    QTreeWidgetItem* nodeLine;
    QTreeWidgetItem* nodeSegment;
    QTreeWidgetItem* nodeHalfLine;
    QTreeWidgetItem* nodePolygon;
    QTreeWidgetItem* nodeCircle;

    QHash<QTreeWidgetItem*,MyItem*> nodeLinks;
    QHBoxLayout*  hbox;
    DisplayProperties* displayPanel;
    void fillTree(QVector<MyItem*>* );
    void initGui();
private slots:
    void updateTree();

};
class DisplayProperties:public QScrollArea{
    Q_OBJECT
public:
    DisplayProperties(Canvas2D* canvas);
    QVBoxLayout *vLayout;
    void updateCanvas();
    QList<MyItem*>* getListItems() const;
    void updateDisplayPanel(QList<MyItem*>*);

private:
    Canvas2D* parent;
    QList<MyItem*>* listItems;
    ColorPanel *colorPanel;
    LegendPanel*legendPanel;
    WidthPanel* widthPanel;
    TypePointPanel* typePointPanel;
    TypeLinePanel* typeLinePanel;
    AlphaFillPanel* alphaFillPanel;

    void initGui();
    bool checkForOnlyPoints() const;
    bool checkForOnlyLines() const;
    bool checkForOnlyFillables() const;

};
class ColorPanel:public QWidget{
    Q_OBJECT
public:
    ColorPanel(DisplayProperties* );
   void setColor(const QColor &);
private:
    DisplayProperties * parent;
    QColor color;
    QPushButton* preview;
    void initGui();
    void updateButton();
private slots:
    void chooseColor();

};
class LegendPanel:public QWidget{
    Q_OBJECT
public:
    LegendPanel(DisplayProperties *p );
    bool hasLegend() const;
    void setLegend(const bool &,const QString & s="");
    void setChecked(bool);
    void setLegendPosition(const int &);

private:
    DisplayProperties * parent;
    QWidget* legendPanel;
    QWidget* legendPosPanel;
    QCheckBox* legendCheck;
    QLineEdit *legendEdit;
    QString legend;
    QComboBox* comboPos;
    QSpinBox* spinAnglePos;
    void initGui();
private slots:
    void updateCanvas();

};

class SliderPanel:public QGroupBox{
    Q_OBJECT
public:
    SliderPanel(DisplayProperties* ,const QString &);
    void setValue(const int);
private:
    void initGui(const QString &);
protected:
    DisplayProperties * parent;
    int value;
    QSlider *slider;
protected slots:
    virtual void updateCanvas()=0;

};
class WidthPanel:public SliderPanel{
    Q_OBJECT
public:
    WidthPanel(DisplayProperties* ,const QString &);
protected slots:
    virtual void updateCanvas();

};
class AlphaFillPanel:public SliderPanel{
    Q_OBJECT
public:
    AlphaFillPanel(DisplayProperties* ,const QString & );
protected slots:
    virtual void updateCanvas();

};



class TypePointPanel:public QWidget{
    Q_OBJECT
public:
    TypePointPanel(const int,DisplayProperties* );
    void setStyle(int);
private:
    void initGui();
    DisplayProperties * parent;
    int type;
    QComboBox *combo;
private slots:
    void updateCanvas(int);


};
class TypeLinePanel:public QWidget{
    Q_OBJECT
public:
    TypeLinePanel(const int,DisplayProperties* );
    void setStyle(const int&);
private:
    void initGui();
    DisplayProperties * parent;
    int type;
    QComboBox *combo;
private slots:
    void updateCanvas(int);

};
/*class AlphaFillPanel{
public:
    AlphaFillPanel(DisplayProperties* );
    void setAlpha(const int&);
private:
    void initGui();
    DisplayProperties* parent;

    int alpha;
private slots:
    void updateCanvas(int );

};*/

#endif // OUTPUT_H
