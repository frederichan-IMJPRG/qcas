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
    void displayContextMenu(const QPoint & );
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
    DisplayProperties(QList<MyItem*>*,Canvas2D* canvas);
    QVBoxLayout *vLayout;
    void updateCanvas();
    QList<MyItem*>* getListItems() const;

private:
    Canvas2D* parent;
    QList<MyItem*>* listItems;
    void initGui();
};
class ColorPanel:public QWidget{
    Q_OBJECT
public:
    ColorPanel(const QColor&,DisplayProperties* );
//   QColor getColor() const;
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
    LegendPanel(const bool, const QString&,DisplayProperties* );
    bool hasLegend() const;
//    QString getlegend() const;
private:
    DisplayProperties * parent;

    QString legend;
    QCheckBox* legendCheck;
    QLineEdit* legendEdit;
    void initGui(bool);
private slots:
    void updateCanvas();

};

class WidthPanel:public QGroupBox{
    Q_OBJECT
public:
    WidthPanel(const int,DisplayProperties* );
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

private:
    void initGui();
    DisplayProperties * parent;
    int type;
    QComboBox *combo;
private slots:
    void updateCanvas();

};


#endif // OUTPUT_H
