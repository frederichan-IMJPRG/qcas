#ifndef OUTPUT_H
#define OUTPUT_H
#include <QWidget>
#include "geometry.h"
#include "giac/gen.h"
#include <QVector>
#include <QDomElement>
#include <QPixmap>
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
class QtMmlWidget;
class WidthPanel;
class TypePointPanel;
class TypeLinePanel;
class ColorPanel;
class LegendPanel;
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

    initGui();
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
    QVector<MyItem*>* getPointItem() const;
    QVector<MyItem*>* getLineItem() const;
    QVector<MyItem*>* getFilledItem() const;
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
    QAction* ratio;

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
    DisplayProperties* displayPanel=0;
    void fillTree(QVector<MyItem*>* );
    void initGui();
private slots:
    void updateTree();

};
class DisplayProperties:public QWidget{
    Q_OBJECT
public:
    DisplayProperties(Canvas2D* canvas);
    QVBoxLayout *vLayout;
    void updateCanvas();
    QList<MyItem*>* getListItems() const;
    updateDisplayPanel(QList<MyItem*>*);

private:
    Canvas2D* parent;
    QList<MyItem*>* listItems;
    ColorPanel *colorPanel;
    LegendPanel*legendPanel;
    WidthPanel* widthPanel;
    TypePointPanel* typePointPanel;
    TypeLinePanel* typeLinePanel;

    void initGui();
    bool checkForOnlyPoints();
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
    void setLegend(const QString &);
    void setChecked(bool);
//    QString getlegend() const;
private:
    DisplayProperties * parent;
    QWidget* legendPanel;
    QCheckBox* legendCheck;
    QLineEdit *legendEdit;
    QString legend;

    void initGui();
private slots:
    void updateCanvas();

};

class WidthPanel:public QGroupBox{
    Q_OBJECT
public:
    WidthPanel(DisplayProperties* );
    void setWidth(const int);
private:
    void initGui();
    DisplayProperties * parent;
    int width;
    QSlider *slider;
private slots:
    void updateCanvas();

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
    void setStyle(int);
private:
    void initGui();
    DisplayProperties * parent;
    int type;
    QComboBox *combo;
private slots:
    void updateCanvas(int);

};


#endif // OUTPUT_H
