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
#include <QDialog>
#include <QVector>
#include <QDomElement>
#include <QPixmap>
#include <QScrollArea>
#include <QHash>
#include <QGroupBox>
#include <QWindowsStyle>
#include <QTabWidget>
class QMenu;
class QBoxLayout;
class Line;
class QComboBox;
class QListWidget;
class QVBoxLayout;
class QPushButton;
class QCheckBox;
class QTreeWidgetItem;
class QToolButton;
class QLineEdit;
class PanelProperties;
class DisplayProperties;
class DisplayObjectPanel;
class AxisPanel;
class AxisGridPanel;
class GenValuePanel;
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



class IconSize : public QWindowsStyle
 {
   Q_OBJECT
public:
    explicit IconSize(QWindowsStyle *parent = 0);
    int pixelMetric(PixelMetric metric, const QStyleOption * option = 0, const QWidget * widget = 0 ) const;
signals:
public slots:
};




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
    FormulaWidget(const QString &);
    virtual void toXML(QDomElement&);

};

/** This widget is used for 2D graphics rendering.
 *  It supports two modes:  normal or interactive
**/

class GraphWidget:public OutputWidget{
    Q_OBJECT

  public:
    GraphWidget( giac::context*,bool );
    GraphWidget(const  giac::gen &, giac::context*,bool );
    QList<MyItem*> getTreeSelectedItems();
    void clearSelection();
    bool isInteractive() const;
    void addToTree(MyItem*);
    void updateAllCategories();

 private:
    bool isInteractiveWidget;
    Canvas2D* canvas;
    PanelProperties * propPanel;

    QWidget* toolPanel;
    // The menu tool bar in interactive mode
    QToolButton* buttonPt;
    QToolButton* buttonLine;
    QToolButton* buttonCircle;
    QAction* singlept;
    QAction * inter;
    QAction* midpoint;
    QAction* line;
    QAction* halfline;
    QAction* segment;
    QAction* circle2pt;
    QAction* circle3pt;


    void initGui();
    void createToolBar();

 private slots:
    void selectButtonIcon(QAction*);
    void selectAction();
};




// The canvas to draw 2D graphics

class Canvas2D:public QWidget{
    Q_OBJECT
public:
    enum action{SINGLEPT,MIDPOINT,INTER,LINE,HALFLINE,SEGMENT,CIRCLE2PT,CIRCLE3PT};

    Canvas2D(GraphWidget* g2d, giac::context*);
    void createScene(const giac::gen & );
    void toScreenCoord(const double,const double,double& , double&);
    void toXY(const double,const double,double& , double&);
    void toXML();
    QSize sizeHint() const;
    QVector<MyItem*>* getPointItem();
    QVector<MyItem*>* getLineItem();
    QVector<MyItem*>* getFilledItem();
    void setFocusOwner(MyItem*);
    void updatePixmap(const bool &, const int & level=-1 );
    void setActionTool(action);
    void setXYUnit();

    // Getter & Setter
    bool isInteractive() const;
    void setBounds(const double &,const double &,const double &,const double &);
    QString getXAxisLegend() const;
    QString getYAxisLegend() const;
    QString getXUnitSuffix() const;
    QString getYUnitSuffix() const;
    giac::context* getContext() const;
    double getXAxisTick() const;
    double getYAxisTick() const;
    void setXAxisLegend(const QString &);
    void setYAxisLegend(const QString &);
    void setXUnitSuffix(const QString &);
    void setYUnitSuffix(const QString &);
    void setXAxisTick(const double &);
    void setYAxisTick(const double &);
    double getXmin() const;
    double getXmax() const ;
    double getYmin() const;
    double getYmax() const;
    QStringList& getCommands();



protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    bool event(QEvent *);

private:
    giac::context*context;

    // Item which could be highlighted
    MyItem* focusOwner;
    // x,y range and units
    double xmin,xmax,ymin,ymax,xunit,yunit;
    bool ortho;

    // Axis and grid parameters
    QString xAxisLegend,yAxisLegend;
    QString xUnitSuffix,yUnitSuffix;
    double xAxisTick,yAxisTick;

    // vectors to store geometry items
    QVector<MyItem*> lineItems;
    QVector<MyItem*> pointItems;
    QVector<MyItem*> filledItems;

    // A command for each line
    QStringList commands;
    // action selected
    action currentActionTool;

    QMenu * menuGeneral;
    QAction* zoomIn;
    QAction* zoomOut;
    QAction* orthoAction;
    QAction* sourceAction;

    GraphWidget *parent;
    // To draw a rectangle selection
    bool selection;
    QPoint startSel;
    QPoint endSel;
    // The Pixmap on which all Items are drawn.
    QPixmap pixmap;
    // Variables names for points and lines.
    QString varPt;
    QString varLine;
//    QString newCommand;

    int evaluationLevel;

    void createMenuAction();
    void addToScene(const giac::gen &);
    double find_tick(double);
    void drawAxes(QPainter*);
    void drawGrid(QPainter*);
    void drawElements(QVector<MyItem*> &, QPainter*, const bool& ,const int&);
    bool checkUnderMouse(QVector<MyItem*>* v, const QPointF &);
    void findFreeVar(QString &);
    void incrementVariable(QString &);

private slots:
    void zoom_In();
    void zoom_Out();
    void make_ortho();
    void displaySource();
    //    void zoom_Factor(const int&);
};

class PanelProperties:public QWidget{
    Q_OBJECT
public:
    PanelProperties(Canvas2D *);
    QList<MyItem*> getTreeSelectedItems();
    void clearSelection();
    void addToTree(MyItem* );
    void updateAllCategories();


private:
    Canvas2D* parent;
    QTreeWidget* tree;
    QTreeWidgetItem* nodeAxis;
    QTreeWidgetItem* nodePoint;
    QTreeWidgetItem* nodeCurve;
    QTreeWidgetItem* nodeVector;
    QTreeWidgetItem* nodeLine;
    QTreeWidgetItem* nodeSegment;
    QTreeWidgetItem* nodeHalfLine;
    QTreeWidgetItem* nodePolygon;
    QTreeWidgetItem* nodeCircle;

    QHash<QTreeWidgetItem*,MyItem*> nodeLinks;
    QBoxLayout*  hbox;
    AxisGridPanel* axisGridPanel;
    DisplayProperties* displayPanel;
    bool updateCategory(QTreeWidgetItem *, const int &);
    void initGui();
private slots:
    void updateTree();

};
class DisplayProperties:public QTabWidget{
    Q_OBJECT
public:
    DisplayProperties(Canvas2D* canvas);
    void updateCanvas();
    QList<MyItem*>* getListItems() const;
    void updateDisplayPanel(QList<MyItem*>*);

private:
    Canvas2D* parent;
    QVBoxLayout *vLayoutGeneral;
    QVBoxLayout *vLayoutAttributes;
    QList<MyItem*>* listItems;
    GenValuePanel* valuePanel;
    DisplayObjectPanel* displayObjectPanel;
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


class DisplayObjectPanel:public QWidget{
    Q_OBJECT
public:
    DisplayObjectPanel(DisplayProperties*);
    void setChecked(const bool);
protected slots:
    virtual void updateCanvas();
private:
     DisplayProperties * parent;
    void initGui();
    QCheckBox * displayObject;


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
class GenValuePanel:public QWidget{
public:
    GenValuePanel(DisplayProperties* );
    void setValue(const QString &);
private:
    QtMmlWidget *mmlWidget;
    QBoxLayout * layout;
    QString value;
    void initGui();
    DisplayProperties* parent;

//private slots:
  //  void updateCanvas(int );

};
class AxisGridPanel: public QTabWidget{
public:
    AxisGridPanel(Canvas2D*);
    void initValue();
    void updateCanvas(const bool &);
private:
    void initGui();
    AxisPanel* xPanel;
    AxisPanel* yPanel;
    Canvas2D* parent;

};
class AxisPanel:public QWidget{
    Q_OBJECT
public:
    AxisPanel(AxisGridPanel* );
    void initValue(const QString&,const QString&,const double&,const double&,const bool&);
private:
    QLineEdit* editLabel;
    QLineEdit* editUnitLabel;
    QLineEdit* editMin;
    QLineEdit* editMax;
    QLineEdit* editDistance;
    QCheckBox* showAxis;
    void initGui();
    friend class AxisGridPanel;

    AxisGridPanel* parent;
private slots:
    void updateCanvas();
};
class SourceDialog:public QDialog{
    Q_OBJECT
public:
    SourceDialog(Canvas2D*);
private:
    Canvas2D* parent;
    void initGui();
    QListWidget* listWidget;
    QPushButton* deleteButton;
private slots:
    void updateCanvas();
};

#endif // OUTPUT_H
