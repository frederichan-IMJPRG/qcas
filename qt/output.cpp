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

#include <QHBoxLayout>
#include <QPainter>
#include <QPointF>
#include <QMenu>
#include <QLabel>
#include <QMouseEvent>
#include <giac/giac.h>
#include <QFileDialog>
#include <QFile>
#include <QTreeWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QClipboard>
#include <QApplication>
#include <QSpinBox>
#include <QMessageBox>
#include <QToolButton>
#include <QButtonGroup>
#include <QToolTip>
#include <QListWidget>
#include <QSlider>
#include <QComboBox>
#include <QColorDialog>
#include <QPushButton>
#include "gui/FormalLine.h"
#include "config.h"
#include "gui/qtmmlwidget.h"
#include "output.h"
#include "vecteur.h"
#include "MainWindow.h"
#include "gui/plotfunctiondialog.h"
using namespace giac;


IconSize::IconSize(QWindowsStyle *parent) : QWindowsStyle()
   {
   Q_UNUSED (parent);
   }

int IconSize::pixelMetric(PixelMetric metric, const QStyleOption * option, const QWidget * widget)const{
   int s = QWindowsStyle::pixelMetric(metric, option, widget);
   if (metric == QStyle::PM_SmallIconSize) {
       s = 32;
   }
   return s;
}


OutputWidget::OutputWidget(QWidget* p):QWidget(p){

}
void OutputWidget::setLine(Line* l){
    line=l;
}
void OutputWidget::toXML(QDomElement &){
}
FormulaWidget::FormulaWidget(QWidget *p):OutputWidget(p){
    formula=giac::undef;
    context=0;
    initGui();
}
FormulaWidget::FormulaWidget(const giac::gen &g,giac::context *c){
    formula=g;
    context=c;
    initGui();
}
void FormulaWidget::initGui(){
//    qDebug()<<"----------------------";
//    qDebug()<<m;
//    qDebug()<<"----------------------";

    mmlWidget=new QtMmlWidget(this);
    if (context!=0)    updateFormula(formula,context);
    QPalette p=mmlWidget->palette();
    p.setColor(QPalette::WindowText,QColor::fromRgb(0,0,255));
    mmlWidget->setPalette(p);
    mmlWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    menu=new QMenu(mmlWidget);
    copyAction=new QAction(tr("Copier"),menu);
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    toLatexAction=new QAction(tr("Copier vers LaTeX"),menu);
    toLatexAction->setIcon(QIcon(":/images/tex.png"));
    toMathmlAction=new QAction(tr("Copier vers Mathml"),menu);
    toMathmlAction->setIcon(QIcon(":/images/mathml.png"));
    menu->addAction(copyAction);
    menu->addAction(toLatexAction);
    menu->addAction(toMathmlAction);

    QHBoxLayout *layout=new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(mmlWidget);
    setLayout(layout);
    connect(mmlWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayMenu(QPoint)));
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
    connect(toLatexAction,SIGNAL(triggered()),this,SLOT(copyToLaTeX()));
    connect(toMathmlAction,SIGNAL(triggered()),this,SLOT(copyToMathml()));
}

void FormulaWidget::copy(){
    QClipboard* clip=QApplication::clipboard();
    clip->setText(QString::fromStdString(giac::print(formula,context)));
}
void FormulaWidget::copyToLaTeX(){
    QClipboard* clip=QApplication::clipboard();
    clip->setText(QString::fromStdString(gen2tex(formula,context)));
}
void FormulaWidget::copyToMathml(){
    QClipboard* clip=QApplication::clipboard();
    clip->setText(QString::fromStdString(gen2mathml(formula,context)));
}
void FormulaWidget::displayMenu(QPoint p){
    menu->popup(mapToGlobal(p));
}
void FormulaWidget::setGen(const gen & g){
    formula=g;
}
void FormulaWidget::updateFormula(const QString  s){
//    qDebug()<<s;
    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(s, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {

        qWarning("MathML error: %s, Line: %d, Column: %d",
                 errorMsg.constData(), errorLine, errorColumn);
      }
//      mmlWidget->setSizePolicy(QSizePolicy::Mi,QSizePolicy::Minimum);
      mmlWidget->updateGeometry();
      resize(mmlWidget->size());
}
QSize FormulaWidget::sizeHint(){
    return mmlWidget->size();
}
void FormulaWidget::toXML(QDomElement & top){

    QDomElement output=top.ownerDocument().createElement("formula");
    QString mathml (QString::fromStdString(giac::print(formula,context)));

    QDomText text=top.ownerDocument().createTextNode(mathml);
    output.appendChild(text);
    top.appendChild(output);
}


void FormulaWidget::updateFormula(const gen & g,giac::context* c){
    context=c;
    formula=g;
    QString m("<math mode=\"display\">\n");
//    qDebug()<<QString::fromStdString(print(g,context));
    m.append(QString::fromStdString(giac::gen2mathml(formula,context)));
    m.append("\n</math>");
//    qDebug()<<m;
    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(m, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {

        qWarning("MathML error: %s, Line: %d, Column: %d",
                 errorMsg.constData(), errorLine, errorColumn);
      }
      mmlWidget->updateGeometry();
     resize(mmlWidget->size());
}


CursorPanel::CursorPanel(const QString &Name, const double &Min, const double & Max, const double & Step, const double & Default, CursorItem *p){
    owner=p;
    name=Name;
    min=Min;
    max=Max;
    step=Step;
    defaultValue=Default;
    initGui();
}
CursorItem* CursorPanel::getOwner(){
    return owner;
}
void CursorPanel::initGui(){
    QGridLayout* layout=new QGridLayout(this);
    labMin=new QLabel;
    labMin->setText(QString::number(min));
    labMax=new QLabel;
    labMax->setText(QString::number(max));
    labName=new QLabel;
    labName->setText(QString("<b>%1:</b>").arg(name));
    labValue=new QLabel;
    labValue->setText(QString::number(defaultValue));
    slider=new QSlider(Qt::Horizontal,this);
    slider->setMinimum(0);
    slider->setSingleStep(1);
    double tickNumber=(max-min)/step;
    if (((int)tickNumber)==tickNumber) slider->setMaximum(tickNumber);
    else slider->setMaximum(tickNumber+1);

    slider->setValue((int)((defaultValue-min)/step));

    deleteButton=new QPushButton;
    deleteButton->setIcon(QIcon(":/images/delete.png"));
    layout->addWidget(labName,0,0);
    layout->addWidget(labValue,0,1);
    layout->addWidget(deleteButton,0,2);
    layout->addWidget(labMin,1,0,Qt::AlignRight);
    layout->addWidget(slider,1,1,Qt::AlignCenter);
    layout->addWidget(labMax,1,2,Qt::AlignLeft);

    setLayout(layout);
    connect(deleteButton,SIGNAL(clicked()),this,SIGNAL(deletePanel()));
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateValue(int)));
    connect(slider,SIGNAL(valueChanged(int)),this,SIGNAL(valueChanged()));
}
void CursorPanel::updateValue(int c){
    labValue->setText(QString::number(getValue()));
}
double CursorPanel::getValue() const{
    return min+slider->value()*step;
}

/**
 * @brief GraphWidget::GraphWidget
 *        This constructor is used for 2D graĥ
 * @param context The current context
 * @param b is Widget Interactive ?
 * @param main The Main Window
 */

GraphWidget::GraphWidget(giac::context * context,bool b,MainWindow* main){
        mainWindow=main;
        isInteractiveWidget=b;
        canvas=new Canvas2D(this,context);
        canvas->updatePixmap(true);
        canvas->repaint();
        initGui();
}
/**
 * @brief GraphWidget::GraphWidget
 *        This constructor is used for 2D graĥ
 * @param g The gen to draw
 * @param context The current context
 * @param b is Widget Interactive ?
 */
GraphWidget::GraphWidget(const giac::gen & g, giac::context * context,bool b){
    isInteractiveWidget=b;
    canvas=new Canvas2D(this,context);
    initGui();

    canvas->createScene(g);

    propPanel->updateAllCategories();
       /*   QPalette p=palette();
       p.setColor(QPalette::Window,Qt::white);
       setPalette(p);
    */
       /*   if (ortho)
            orthonormalize();
        }
        find_ylegende();
        y_tick=find_tick(window_ymax-window_ymin);
        redraw();
        push_cfg();




        if (Config::ortho)
            tmp->orthonormalize();
        }
        tmp->add(evaled_g);
    if (anim)
      tmp->animation_dt=1./5;
    if (Xcas_config.autoscale)
      tmp->autoscale();
    tmp->update_infos(evaled_g,contextptr);
    g->end();
    change_group_fontsize(g,w->labelsize());
    return g;*/

 }
void GraphWidget::initGui(){
    propPanel=new PanelProperties(canvas);
    propPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Minimum);
//    canvas->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QWidget* canvasWidget=new QWidget(this);
    QVBoxLayout* vbox=new QVBoxLayout(canvasWidget);
    if (isInteractiveWidget){
        createToolBar();
        vbox->addWidget(toolPanel);//,Qt::AlignLeft|Qt::AlignTop);
    }

    vbox->addWidget(canvas);//,1,Qt::AlignLeft|Qt::AlignTop);
    if (isInteractiveWidget){
        sliderPanel=new QWidget(canvasWidget);
        sliderPanel->setLayout(new QHBoxLayout);
        sliderPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
        vbox->addWidget(sliderPanel);
    }
    vbox->setSizeConstraint(QLayout::SetMinimumSize);
    canvasWidget->setLayout(vbox);
    canvasWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

    QHBoxLayout *hbox=new QHBoxLayout(this);
    hbox->addWidget(canvasWidget);
    hbox->addWidget(propPanel);
    hbox->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(hbox);

}
/**
 * @brief GraphWidget::createToolBar
 * In interactive mode2D , adds the Tool bar
 */

void GraphWidget::createToolBar(){
    buttonPointer=new QToolButton(this);
    buttonPt=new QToolButton(this);
    buttonLine=new QToolButton(this);
    buttonTool=new QToolButton(this);
    buttonPlot=new QToolButton(this);
    buttonCircle=new QToolButton(this);

    // Menu creation
    QMenu* menuPointer=new QMenu(buttonPointer);
    QMenu* menuPt=new QMenu(buttonPt);
    QMenu* menuLine=new QMenu(buttonLine);
    QMenu* menuPlot=new QMenu(buttonPlot);
    QMenu* menuTool=new QMenu(buttonTool);
    QMenu* menuCircle=new QMenu(buttonCircle);

    // Pointer actions
    select=new QAction(tr("Sélection"),buttonPointer);
    select->setData(canvas->SELECT);
    select->setIcon(QIcon(":/images/select.png"));
    select->setProperty("comment",tr("Sélection || Déplacement"));
    zoomIn=new QAction(tr("Zoom Avant"),buttonPointer);
    zoomIn->setData(canvas->ZOOM_IN);
    zoomIn->setIcon(QIcon(":/images/zoom-in.png"));
    zoomIn->setProperty("comment","");
    zoomOut=new QAction(tr("Zoom Arrière"),buttonPointer);
    zoomOut->setData(canvas->ZOOM_OUT);
    zoomOut->setIcon(QIcon(":/images/zoom-out.png"));
    zoomOut->setProperty("comment","");

    // Point Actions
    singlept=new QAction(tr("Point"),buttonPt);
    singlept->setData(canvas->SINGLEPT);
    singlept->setIcon(QIcon(":/images/point.png"));
    singlept->setProperty("comment","");
    inter=new QAction(tr("Intersection entre deux objets"),buttonPt);
    inter->setIcon(QIcon(":/images/inter.png"));
    inter->setData(canvas->INTER);
    inter->setProperty("comment",tr("2 objets (points exclus)"));
    midpoint=new QAction(tr("Milieu ou centre"),buttonPt);
    midpoint->setIcon(QIcon(":/images/midpoint.png"));
    midpoint->setData(canvas->MIDPOINT);
    midpoint->setProperty("comment",tr("Segment || Deux points||Cercle"));
    pointxy=new QAction(tr("Point repéré"),buttonPt);
    pointxy->setData(canvas->POINT_XY);
    pointxy->setIcon(QIcon(":/images/pointxy.png"));
    pointxy->setProperty("comment","");

    // Line Actions
    line=new QAction(tr("Droite"),buttonLine);
    line->setIcon(QIcon(":/images/line.png"));
    line->setData(canvas->LINE);
    line->setProperty("comment",tr("Deux points"));
    linebyequation=new QAction(tr("Droite par équation"),buttonLine);
    linebyequation->setIcon(QIcon(":/images/linebyequation.png"));
    linebyequation->setData(canvas->LINEBYEQUATION);
    linebyequation->setProperty("comment","");
    segment=new QAction(tr("Segment"),buttonLine);
    segment->setIcon(QIcon(":/images/segment.png"));
    segment->setData(canvas->SEGMENT);
    segment->setProperty("comment",tr("Deux points"));
    halfline=new QAction(tr("Demie-droite"),buttonLine);
    halfline->setIcon(QIcon(":/images/halfline.png"));
    halfline->setData(canvas->HALFLINE);
    halfline->setProperty("comment",tr("Deux points"));
    vectorAc=new QAction(tr("Vecteur"),buttonLine);
    vectorAc->setIcon(QIcon(":/images/vector.png"));
    vectorAc->setData(canvas->VECTOR);
    vectorAc->setProperty("comment",tr("Origine puis extrémité"));
    perpen_bisector=new QAction(tr("Médiatrice d'un segment"),buttonLine);
    perpen_bisector->setIcon(QIcon(":/images/perpenbisector.png"));
    perpen_bisector->setData(canvas->PERPEN_BISECTOR);
    perpen_bisector->setProperty("comment",tr("Segment || Deux points"));
    bisector=new QAction(tr("Bissectrice d'un angle"),buttonLine);
    bisector->setIcon(QIcon(":/images/bisector.png"));
    bisector->setData(canvas->BISECTOR);
    bisector->setProperty("comment",tr("Sommet puis 2 points"));
    tangent=new QAction(tr("Tangente à une courbe ou un cercle"),buttonLine);
    tangent->setIcon(QIcon(":/images/tangent.png"));
    tangent->setData(canvas->TANGENT);
    tangent->setProperty("comment",tr("(Courbe|| Cercle) puis point de contact"));


    // Tool Actions
    parallel=new QAction(tr("Droite parallèle"),buttonTool);
    parallel->setIcon(QIcon(":/images/parallel.png"));
    parallel->setData(canvas->PARALLEL);
    parallel->setProperty("comment",tr("Droite puis point"));
    perpendicular=new QAction(tr("Droite perpendiculaire"),buttonTool);
    perpendicular->setIcon(QIcon(":/images/perpendicular.png"));
    perpendicular->setData(canvas->PERPENDICULAR);
    perpendicular->setProperty("comment",tr("Droite puis point"));
    reflection=new QAction(tr("Symétrie axiale"),buttonTool);
    reflection->setIcon(QIcon(":/images/reflection.png"));
    reflection->setData(canvas->REFLECTION);
    reflection->setProperty("comment",tr("Axe de symétrie puis objet"));
    pointSymmetry=new QAction(tr("Symétrie centrale"),buttonTool);
    pointSymmetry->setIcon(QIcon(":/images/point-symmetry.png"));
    pointSymmetry->setData(canvas->POINT_SYMMETRY);
    pointSymmetry->setProperty("comment",tr("Centre de symétrie puis objet"));
    rotation=new QAction(tr("Rotation"),buttonTool);
    rotation->setIcon(QIcon(":/images/rotation.png"));
    rotation->setData(canvas->ROTATION);
    rotation->setProperty("comment",tr("Centre puis objet"));
    translation=new QAction(tr("Translation"),buttonTool);
    translation->setIcon(QIcon(":/images/translation.png"));
    translation->setData(canvas->TRANSLATION);
    translation->setProperty("comment",tr("Vecteur puis objet"));
    homothety=new QAction(tr("Homothétie"),buttonTool);
    homothety->setIcon(QIcon(":/images/homothety.png"));
    homothety->setData(canvas->HOMOTHETY);
    homothety->setProperty("comment",tr("Centre puis objet"));
    similarity=new QAction(tr("Similitude"),buttonTool);
    similarity->setIcon(QIcon(":/images/similarity.png"));
    similarity->setData(canvas->SIMILARITY);
    similarity->setProperty("comment",tr("Centre puis objet"));

    // Plot and polygon actions
     plotFunction=new QAction(tr("Courbe représentative"),buttonPlot);;
     plotFunction->setData(canvas->PLOT_FUNCTION);
     plotFunction->setIcon(QIcon(":/images/function.png"));
     plotFunction->setProperty("comment","");
     plotBezier=new QAction(tr("Courbe de Bézier"),buttonPlot);;
     plotBezier->setData(canvas->PLOT_BEZIER);
     plotBezier->setIcon(QIcon(":/images/bezier.png"));
     plotBezier->setProperty("comment",tr("Points de contrôle puis clic droit pour le dernier point"));
     polygon=new QAction(tr("Polygône/Ligne brisée"),buttonPlot);;
     polygon->setData(canvas->POLYGON);
     polygon->setIcon(QIcon(":/images/polygon.png"));
     polygon->setProperty("comment",tr("Sommets puis clic droit pour le dernier sommet"));
     regularPolygon=new QAction(tr("Polygône régulier"),buttonPlot);;
     regularPolygon->setData(canvas->REGULAR_POLYGON);
     regularPolygon->setIcon(QIcon(":/images/regularpolygon.png"));
     regularPolygon->setProperty("comment",tr("Deux points"));

     numericCursor=new QAction(tr("Curseur numérique"),buttonPlot);
     numericCursor->setIcon(QIcon(":/images/numericcursor.png"));
     numericCursor->setData(canvas->NUMERIC_CURSOR);
     numericCursor->setProperty("comment","");
     formalCursor=new QAction(tr("Curseur symbolique"),buttonPlot);
     formalCursor->setIcon(QIcon(":/images/formalcursor.png"));
     formalCursor->setData(canvas->FORMAL_CURSOR);
     formalCursor->setProperty("comment","");

    // Circle Actions
    circle2pt=new QAction(tr("Cercle (centre-point)"),buttonCircle);
    circle2pt->setIcon(QIcon(":/images/circle2pt.png"));
    circle2pt->setData(canvas->CIRCLE2PT);
    circle2pt->setProperty("comment","Centre puis point");

    circleRadius=new QAction(tr("Cercle (centre-rayon)"),buttonCircle);
    circleRadius->setIcon(QIcon(":/images/circleRadius.png"));
    circleRadius->setData(canvas->CIRCLE_RADIUS);
    circleRadius->setProperty("comment","Centre");

    circle3pt=new QAction(tr("Cercle (3 points)"),buttonCircle);
    circle3pt->setIcon(QIcon(":/images/circle3pt.png"));
    circle3pt->setData(canvas->CIRCLE3PT);
    circle3pt->setProperty("comment","Trois points");
    arc3pt=new QAction(tr("Arc de cercle (3 points)"),buttonCircle);
    arc3pt->setIcon(QIcon(":/images/arc3pt.png"));
    arc3pt->setData(canvas->ARC3PT);
    arc3pt->setProperty("comment","Trois points");
/*    arcCenter=new QAction(tr(" (3 points)"),buttonCircle);
    arcCenter->setIcon(QIcon(":/images/arc3pt.png"));
    arcCenter->setData(canvas->ARC3PT);
    arcCenter->setProperty("comment","Trois points");*/
    angle=new QAction(tr("Angle"),buttonCircle);
    angle->setIcon(QIcon(":/images/angle.png"));
    angle->setData(canvas->ANGLE);
    angle->setProperty("comment",tr("Sommet de l'angle puis deux points"));


    // Set default actions
    buttonPointer->setProperty("myAction",canvas->SELECT);
    buttonPointer->setProperty("comment","");
    buttonPt->setProperty("myAction",canvas->SINGLEPT);
    buttonPt->setProperty("comment",tr("Sélection || Déplacement"));
    buttonLine->setProperty("myAction",canvas->LINE);
    buttonLine->setProperty("comment",tr("Deux points"));
    buttonTool->setProperty("myAction",canvas->PARALLEL);
    buttonTool->setProperty("comment",tr("Droite puis point"));
    buttonPlot->setProperty("myAction",canvas->PLOT_FUNCTION);
    buttonTool->setProperty("comment","");
    buttonCircle->setProperty("myAction",canvas->CIRCLE2PT);
    buttonCircle->setProperty("comment",tr("Centre puis point"));


    menuPointer->addAction(select);
    menuPointer->addAction(zoomIn);
    menuPointer->addAction(zoomOut);
    menuPointer->setStyle(new IconSize);

    menuPt->addAction(singlept);
    menuPt->addAction(pointxy);
    menuPt->addAction(inter);
    menuPt->addAction(midpoint);
    menuPt->setStyle(new IconSize);

    menuLine->addAction(line);
    menuLine->addAction(linebyequation);
    menuLine->addAction(segment);
    menuLine->addAction(halfline);
    menuLine->addAction(vectorAc);
    menuLine->addAction(bisector);
    menuLine->addAction(perpen_bisector);
    menuLine->addAction(tangent);
    menuLine->setStyle(new IconSize);

    menuTool->addAction(parallel);
    menuTool->addAction(perpendicular);
    menuTool->addAction(reflection);
    menuTool->addAction(pointSymmetry);
    menuTool->addAction(rotation);
    menuTool->addAction(translation);
    menuTool->addAction(homothety);
    menuTool->addAction(similarity);
    menuTool->setStyle(new IconSize);

    menuPlot->addAction(plotFunction);
    menuPlot->addAction(plotBezier);
    menuPlot->addAction(polygon);
    menuPlot->addAction(regularPolygon);
    menuPlot->addAction(numericCursor);
    menuPlot->addAction(formalCursor);
    menuPlot->setStyle(new IconSize);

    menuCircle->addAction(circle2pt);
    menuCircle->addAction(circleRadius);
    menuCircle->addAction(circle3pt);
    menuCircle->addAction(arc3pt);
    menuCircle->addAction(angle);
    menuCircle->setStyle(new IconSize);

    const QSize size(40,40);
    buttonPointer->setIconSize(size);
    buttonPointer->setCheckable(true);
    buttonPointer->setPopupMode(QToolButton::MenuButtonPopup);
    buttonPointer->setMenu(menuPointer);
    buttonPointer->setIcon(QIcon(":/images/select.png"));


    buttonPt->setIconSize(size);
    QString s("QToolButton{margin: 8px;}"
//              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//              "stop: 0 #f6f7fa, stop: 1 #dadbde);}"
//               "QToolButton:pressed {"
//        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//        "stop: 0 #dadbde, stop: 1 #f6f7fa);}"
        "QToolButton:checked {border-left: 2px solid blue;"
        "border-top: 2px solid blue;"
        "border-bottom: 2px solid blue;"

        "border-radius: 6px;}"
        "QToolButton:menu{   subcontrol-position: right bottom;}"
    "QToolButton::menu-indicator{ right bottom}");

    //buttonPt->setStyleSheet( s);
    buttonPt->setCheckable(true);
    buttonPt->setPopupMode(QToolButton::MenuButtonPopup);
    buttonPt->setMenu(menuPt);
   buttonPt->setIcon(QIcon(":/images/point.png"));

    QString t(
            "QToolButton{margin:10px;}"
//            "QToolButton:checked{border: 2px solid blue; }"
              "QToolButton::menu-indicator{subcontrol-origin: margin;subcontrol-position: bottom right;}"
                "QToolButton::menu-button {"
"}");

//            "subcontrol-origin: margin;}"
//            "QToolButton::menu-button {"
//              "border: 2px solid gray;"
//              "width: 30 px;}");
//              "QPushButton::menu-indicator:pressed, QPushButton::menu-indicator:open {"
//                 "position: relative;"
 //                "top: 2px; left: 2px;}");
              // "subcontrol-position: bottom right;"
              //"subcontrol-origin: contents;}"
              //"QToolButton[popupMode=\"1\"]{"
              //"subcontrol-position: bottom right;}"


    buttonLine->setIconSize(size);
    buttonLine->setPopupMode(QToolButton::MenuButtonPopup);
    buttonLine->setCheckable(true);
//    buttonLine->setStyleSheet(t);
    buttonLine->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    buttonLine->setMenu(menuLine);
    buttonLine->setIcon(QIcon(":/images/line.png"));

    buttonTool->setIconSize(size);
    buttonTool->setPopupMode(QToolButton::MenuButtonPopup);
    buttonTool->setMenu(menuTool);
    buttonTool->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    buttonTool->setIcon(QIcon(":/images/parallel.png"));
    buttonTool->setCheckable(true);

    buttonPlot->setIconSize(size);
    buttonPlot->setPopupMode(QToolButton::MenuButtonPopup);
    buttonPlot->setMenu(menuPlot);
    buttonPlot->setIcon(QIcon(":/images/function.png"));
    buttonPlot->setCheckable(true);


    buttonCircle->setIconSize(size);
    buttonCircle->setPopupMode(QToolButton::MenuButtonPopup);
    buttonCircle->setMenu(menuCircle);
    buttonCircle->setIcon(QIcon(":/images/circle2pt.png"));
    buttonCircle->setCheckable(true);

//    toolBar->addWidget(buttonPt);
//    toolBar->addWidget(buttonLine);
 //   toolBar->addWidget(buttonCircle);
  //  toolBar->setIconSize(QSize(48,48));


    QButtonGroup *group=new QButtonGroup(this);
    group->addButton(buttonPointer);
    group->addButton(buttonPt);
    group->addButton(buttonLine);
    group->addButton(buttonTool);
    group->addButton(buttonPlot);
    group->addButton(buttonCircle);

    toolPanel=new QWidget(this);
    toolPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    QHBoxLayout* hbox=new QHBoxLayout(toolPanel);
    hbox->addWidget(buttonPointer,Qt::AlignLeft);
    hbox->addWidget(buttonPt,Qt::AlignLeft);
    hbox->addWidget(buttonLine,Qt::AlignLeft);
    hbox->addWidget(buttonTool,Qt::AlignLeft);
    hbox->addWidget(buttonPlot,Qt::AlignLeft);
    hbox->addWidget(buttonCircle,Qt::AlignLeft);

    toolPanel->setLayout(hbox);

    connect(menuPointer,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuPt,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuLine,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuTool,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuPlot,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuCircle,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(buttonPointer,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonPt,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonLine,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonPlot,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonTool,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonCircle,SIGNAL(clicked()),this,SLOT(selectAction()));


}
/** When the user clicks a button, it propagates action to the canvas
  *
  */
void GraphWidget::selectAction(){

    QToolButton* b=qobject_cast<QToolButton*>(sender());
    b->setChecked(true);
    canvas->setActionTool((Canvas2D::action)(b->property("myAction").toInt()));
    mainWindow->displayInStatusBar(b->property("comment").toString(),"blue");

}


/** Depending on the action selected by the user, changes the button icon and
  * propagates action to the canvas
  **/

void GraphWidget::selectButtonIcon(QAction * ac){
   QToolButton* b=qobject_cast<QToolButton*>(ac->parent());

   b->setChecked(true);

   b->setIcon(ac->icon());

   b->setProperty("myAction",ac->data().toInt());
   b->setProperty("comment",ac->property("comment").toString());
   mainWindow->displayInStatusBar(ac->property("comment").toString(),"blue");

   canvas->setActionTool((Canvas2D::action)ac->data().toInt());
}



 /**
  * Returns true if this Widget is used for interactive geometry
  **/
bool GraphWidget::isInteractive() const{
    return isInteractiveWidget;
}
/**
  * Adds an entry in the tree objects for the  geometry object "item"
  */

void GraphWidget::addToTree(MyItem * item){
    propPanel->addToTree(item);
}

void GraphWidget::renameInTree(MyItem * item){
    propPanel->renameInTree(item);
}

void GraphWidget::updateAllCategories(){
    propPanel->updateAllCategories();
}
void GraphWidget::updateValueInDisplayPanel(){
    propPanel->updateValueInDisplayPanel();
}
void GraphWidget::selectInTree(MyItem * item){
    propPanel->selectInTree(item);
}
void GraphWidget::addCursorPanel(CursorPanel * c){
    sliderPanel->layout()->addWidget(c);

}
void GraphWidget::deleteCursorPanel(CursorPanel * cp){
    sliderPanel->layout()->removeWidget(cp);
}
void GraphWidget::toXML(QDomElement & top){
    QDomElement output=top.ownerDocument().createElement("graph2d");
    canvas->toXML(output);
    top.appendChild(output);

}
void GraphWidget::loadXML(QDomElement & graph2d){
    canvas->setFixedSize(Config::graph_width,Config::graph_width*3/4);
    QDomNode first=graph2d.firstChild();
    while(!first.isNull()){
        QDomElement element=first.toElement();
        if (!element.isNull()) {
            QString tag=element.tagName();
            int att=element.attribute("attributes","0").toInt();
            if (tag=="point"){
                giac::gen g;
                QString legend;
                QDomNodeList list=element.childNodes();
                for (int i=0;i<list.size();++i){
                    QDomElement node=list.at(i).toElement();
                    if (!node.isNull()){
                        if (node.tagName()=="value") g=gen(node.text().toStdString(),mainWindow->getContext());
                        else if (node.tagName()=="legend") legend=node.text();
                    }
                }

                Point* p=new Point(g,canvas);
                p->setLegend(legend);
                p->setAttributes(att);
                addToTree(p);
                canvas->getPointItem()->append(p);

           }
            else if ((tag=="line") || (tag=="halfline")){
                giac::gen g;
                QPointF start;
                QPointF end;
                QDomNodeList list=element.childNodes();
                for (int i=0;i<list.size();++i){
                    QDomElement node=list.at(i).toElement();
                    if (!node.isNull()){
                        if (node.tagName()=="equation") g=gen(node.text().toStdString(),mainWindow->getContext());
                        else if(node.tagName()=="startPoint") {
                            double x=node.attribute("x","0").toDouble();
                            double y=node.attribute("y","0").toDouble();
                            start=QPointF(x,y);

                        }
                        else if(node.tagName()=="endPoint") {
                            double x=node.attribute("x","0").toDouble();
                            double y=node.attribute("y","0").toDouble();
                            end=QPointF(x,y);

                        }
                    }
                }
                //line
                if (tag=="line") {
                    LineItem* l=new LineItem(start,end,canvas);
                    l->setAttributes(att);
                    l->setValue(g);
                    addToTree(l);
                    canvas->getLineItem()->append(l);
                }
                // halfline
                else {
                    HalfLineItem* l=new HalfLineItem(start,end,canvas);
                    l->setAttributes(att);
                    l->setValue(g);
                    addToTree(l);
                    canvas->getLineItem()->append(l);
                }
            }
            else if (tag=="axis"){
                AxisParam axisParam;
                axisParam.isVisible=(element.attribute("isVisible","0")!=0);
                axisParam.legend=element.attribute("legend","");
                axisParam.unitSuffix=element.attribute("unitSuffix","");
                axisParam.max=element.attribute("max","5").toDouble();
                axisParam.min=element.attribute("min",QString::number(axisParam.max-10)).toDouble();

                axisParam.tick=element.attribute("tick","1").toDouble();
                axisParam.color=QColor::fromRgb(element.attribute("color","0").toUInt());
                if (element.attribute("position")=="x") canvas->setXAxisParam(axisParam);
                else canvas->setYAxisParam(axisParam);
            }
            else if (tag=="grid"){
                GridParam grid;
                grid.isVisible=(element.attribute("isVisible","0")!=0);
                grid.isCartesian=(element.attribute("isCartesian","0")!=0);
                //grid.color=QColor::fromRgb(element.attribute("color","0").toInt());

                grid.color=QColor::fromRgb(element.attribute("color","0").toUInt());

                grid.line=element.attribute("line","0").toInt();
                if (grid.isCartesian){
                    grid.x=element.attribute("x","1").toDouble();
                    grid.y=element.attribute("y","1").toDouble();
                }
                else{
                    grid.r=element.attribute("r","1").toDouble();
                    grid.theta=(PolarAngle)element.attribute("theta",QString::number(PI_6)).toInt();
                }
                canvas->setGridParam(grid);
            }
        }
        first=first.nextSibling();
    }

    canvas->setBounds(canvas->getXAxisParam().min,canvas->getXAxisParam().max,canvas->getYAxisParam().min,canvas->getYAxisParam().max);
    canvas->setXYUnit();
    canvas->updatePixmap(true);

}
QList<MyItem*> GraphWidget::getTreeSelectedItems(){
    return propPanel->getTreeSelectedItems();
}
void GraphWidget::removeFromTree(MyItem * item){
    propPanel->removeFromTree(item);
}





/**
  * clear all selected items in the tree
  */
void GraphWidget::clearSelection(){
    propPanel->clearSelection();
}


Canvas2D::Canvas2D(GraphWidget *g2d, giac::context * c){
    parent=g2d;
    context=c;
    giac::decimal_digits(3,c);
    ortho=true;
    selectionRight=false;
    selectionLeft=false;
    focusOwner=0;
    itemPreview=0;
    currentActionTool=SELECT;
    varPt="A";
    varLine="a";
    evaluationLevel=-1;

    // Grid parameters
    gridParam.isVisible=true;
    gridParam.color=Qt::lightGray;
    gridParam.isCartesian=true;
    gridParam.line=1;
    gridParam.x=1;gridParam.y=1;gridParam.r=1;gridParam.theta=PI_6;
    // Axis parameters
    xAxisParam.isVisible=true;
    xAxisParam.color=Qt::black;
    xAxisParam.tick=1;
    yAxisParam.isVisible=true;
    yAxisParam.color=Qt::black;
    yAxisParam.tick=1;

    setMouseTracking(true);
    setBackgroundRole(QPalette::Light);
    setAutoFillBackground(true);


    xmin=giac::gnuplot_xmin;
    xmax=giac::gnuplot_xmax;
    ymin=giac::gnuplot_ymin;
    ymax=giac::gnuplot_ymax;

    giac::global_window_xmin=xmin;
    giac::global_window_xmax=xmax;
    giac::global_window_ymin=ymin;
    giac::global_window_ymax=ymax;

    setXYUnit();
    make_ortho();

    createMenuAction();
    setContextMenuPolicy(Qt::NoContextMenu);
    QSizePolicy p(QSizePolicy::Minimum,QSizePolicy::Minimum);
    p.setHorizontalStretch(1);
    setSizePolicy(p);

 //   setMinimumHeight(100);
}
void Canvas2D::createMenuAction(){

    gridAction=new QAction(tr("Afficher la grille"),this);
    gridAction->setCheckable(true);
    gridAction->setChecked(gridParam.isVisible);
    connect(gridAction,SIGNAL(toggled(bool)),this,SLOT(displayGrid(bool)));

    axisAction=new QAction(tr("Afficher les axes"),this);
    axisAction->setCheckable(true);
    axisAction->setChecked(xAxisParam.isVisible||yAxisParam.isVisible);
    connect(axisAction,SIGNAL(toggled(bool)),this,SLOT(displayAxis(bool)));


    zoomIn=new QAction(tr("Zoom avant"),this);
    zoomIn->setIcon(QIcon(":/images/zoom-in.png"));
   connect(zoomIn,SIGNAL(triggered()),this,SLOT(zoom_In()));

    zoomOut=new QAction(tr("Zoom arrière"),this);
    zoomOut->setIcon(QIcon(":/images/zoom-out.png"));
    connect(zoomOut,SIGNAL(triggered()),this,SLOT(zoom_Out()));

    orthoAction=new QAction(tr("Orthonormé"),this);
    orthoAction->setIcon(QIcon(":/images/ortho.png"));
    connect(orthoAction,SIGNAL(triggered()),this,SLOT(make_ortho()));

    if (isInteractive()) {
        sourceAction=new QAction(tr("Code de la figure"),this);
        sourceAction->setIcon(QIcon(":/images/source.png"));
        connect(sourceAction,SIGNAL(triggered()),this,SLOT(displaySource()));
    }
    orthoAction->setIcon(QIcon(":/images/ortho.png"));
    connect(orthoAction,SIGNAL(triggered()),this,SLOT(make_ortho()));

    exportPNG=new QAction(tr("image au format PNG, JPG ou BMP"),this);
    exportPNG->setIcon(QIcon(":/images/png.png"));
    connect(exportPNG,SIGNAL(triggered()),this,SLOT(exportToPNG()));
    exportSVG=new QAction(tr("image *.svg"),this);
    exportSVG->setIcon(QIcon(":/images/svg.png"));
    exportLatex=new QAction(tr("format PSTricks (LaTeX)"),this);
    exportLatex->setIcon(QIcon(":/images/tex.png"));


   QAction* title=new QAction(tr("Graphique"),this);
   QFont font;
   font.setWeight(QFont::Bold);
   title->setFont(font);




   // create contextual menu
    menuGeneral=new QMenu(this);
    menuGeneral->addAction(title);
    menuGeneral->addSeparator();
    menuGeneral->addAction(gridAction);
    menuGeneral->addAction(axisAction);
    menuGeneral->addAction(zoomIn);
    menuGeneral->addAction(zoomOut);
    menuGeneral->addAction(orthoAction);
    menuGeneral->addSeparator();

    menuExport=new QMenu(tr("Export vers..."),this);
    menuExport->addAction(exportLatex);
    menuExport->addAction(exportPNG);
    menuExport->addAction(exportSVG);
    menuGeneral->addMenu(menuExport);
    if (isInteractive()){
        menuGeneral->addAction(sourceAction);
    }
    objectTitleAction=new QAction(this);
    objectTitleAction->setFont(font);

    displayObjectAction=new QAction(tr("Afficher l'objet"),this);
    displayObjectAction->setCheckable(true);
    connect(displayObjectAction,SIGNAL(triggered(bool)),this,SLOT(displayObject(bool)));

    displayLegendAction=new QAction(tr("Afficher la légende"),this);
    displayLegendAction->setCheckable(true);
    connect(displayLegendAction,SIGNAL(triggered(bool)),this,SLOT(displayLegend(bool)));

    traceAction=new QAction(tr("Trace activée"),this);
    traceAction->setCheckable(true);
    traceAction->setChecked(false);
    connect(traceAction,SIGNAL(triggered(bool)),this,SLOT(trace(bool)));


    deleteAction=new QAction(tr("Supprimer l'objet"),this);
    deleteAction->setIcon(QIcon(":/images/delete.png"));
    connect(deleteAction,SIGNAL(triggered()),this,SLOT(deleteObject()));

    renameAction=new QAction(tr("Renommer l'objet"),this);
    connect(renameAction,SIGNAL(triggered()),this,SLOT(renameObject()));


    menuObject=new QMenu(this);
    menuObject->addAction(objectTitleAction);
    menuObject->addSeparator();
    menuObject->addAction(displayObjectAction);
    menuObject->addAction(displayLegendAction);
    menuObject->addAction(traceAction);
    menuObject->addAction(renameAction);
    menuObject->addAction(deleteAction);


}

void Canvas2D::setActionTool(action a){
    if (itemPreview!=0) delete itemPreview;
    itemPreview=0;
    currentActionTool=a;
    if (currentActionTool==POINT_XY){
        CoordsDialog* dialog=new CoordsDialog(this);
        if (dialog->exec()){
            findFreeVar(varPt);
            Command newCommand;
            QString s(varPt);
            s.append(":=point(");
            s.append(dialog->editX->text());
            s.append(",");
            s.append(dialog->editY->text());
            s.append(");");
            newCommand.command=s;
            newCommand.attributes=0;
            newCommand.isCustom=false;
            evaluationLevel=commands.size();
            gen g(newCommand.command.toStdString(),context);
            QList<MyItem*> v;
            addToVector(protecteval(g,1,context),v);
            if (v.isEmpty()) {
                giac::_purge(gen(varPt.toStdString(),context),context);
                return;
            }
            findIDNT(g,v.at(0));
            v.at(0)->updateScreenCoords(true);
            v.at(0)->setVar(varPt);
            newCommand.item=v.at(0);
            commands.append(newCommand);

            pointItems.append(v.at(0));
            parent->addToTree(v.at(0));
            focusOwner=v.at(0);
            parent->updateAllCategories();
            parent->selectInTree(focusOwner);
            selectedItems.append(focusOwner);
            updatePixmap(false);
            repaint();
        }

    }
        else if(currentActionTool==PLOT_FUNCTION){

            PlotFunctionDialog* dialog=new PlotFunctionDialog(this);


            if (dialog->exec()){
                findFreeVar(varLine);
                QString s(varLine);
                s.append(":=");
                s.append(dialog->getString());
                Command newCommand;
                newCommand.command=s;
                newCommand.attributes=0;
                newCommand.isCustom=false;
                evaluationLevel=commands.size();
                gen g(newCommand.command.toStdString(),context);
        //                    qDebug()<<QString::fromStdString(g.print(context));
                QList<MyItem*> v;
                addToVector(protecteval(g,1,context),v);
                if (v.isEmpty()){
                    giac::_purge(gen(varLine.toStdString(),context),context);

                    return;
                }
                findIDNT(g,v.at(0));
                newCommand.item=v.at(0);
                commands.append(newCommand);

                v.at(0)->setVar(varLine);
                v.at(0)->updateScreenCoords(true);
                lineItems.append(v.at(0));
                parent->addToTree(v.at(0));
                focusOwner=v.at(0);
                parent->updateAllCategories();
                parent->selectInTree(focusOwner);
                selectedItems.append(focusOwner);
                updatePixmap(false);
                repaint();


            }

        delete(dialog);

    }
    else if(currentActionTool==LINEBYEQUATION){
        OneArgDialog* dialog=new OneArgDialog(this,tr("Equation"));
        if (dialog->exec()){
            findFreeVar(varLine);
            QString s(varLine);
            s.append(":=line(");
            s.append(dialog->editRadius->text());
            s.append(");");
            Command newCommand;
            newCommand.command=s;
            newCommand.attributes=0;
            newCommand.isCustom=false;
            evaluationLevel=commands.size();
            gen g(newCommand.command.toStdString(),context);

            QList<MyItem*> v;
            addToVector(protecteval(g,1,context),v);
            if (v.isEmpty()){
                giac::_purge(gen(varLine.toStdString(),context),context);

                return;
            }
            findIDNT(g,v.at(0));
            newCommand.item=v.at(0);
            commands.append(newCommand);

            v.at(0)->setVar(varLine);
            v.at(0)->updateScreenCoords(true);
            lineItems.append(v.at(0));
            parent->addToTree(v.at(0));
            focusOwner=v.at(0);
            parent->updateAllCategories();
            parent->selectInTree(focusOwner);
            selectedItems.append(focusOwner);
            updatePixmap(false);
            repaint();
        }
    delete(dialog);
    }
    else if ((currentActionTool==FORMAL_CURSOR) ||(currentActionTool==NUMERIC_CURSOR)){
        CursorDialog* dialog=new CursorDialog(this);
        if (dialog->exec()){
            QString var=dialog->getVar();
            bool ok=false;
            var.toDouble(&ok);
            if (var.isEmpty()||ok){
                var=varLine;
            }
            findFreeVar(var);
            QString s;
            if (currentActionTool==FORMAL_CURSOR) {
                s.append("assume(");
                s.append(var);
                s.append("=[");
                s.append(dialog->getDefault());
                s.append(",");
                s.append(dialog->getMin());
                s.append(",");
                s.append(dialog->getMax());
                s.append(",");
                s.append(dialog->getStep());
                s.append("]);");
            }
            else{
                s.append(var);
                s.append(":=element(");
                s.append(dialog->getMin());
                s.append("..");
                s.append(dialog->getMax());
                s.append(",");
                s.append(dialog->getDefault());
                s.append(",");
                s.append(dialog->getStep());
                s.append(");");
            }
            Command newCommand;
            newCommand.command=s;
            newCommand.attributes=0;
            newCommand.isCustom=false;
            commands.append(newCommand);
            evaluationLevel=commands.size()-1;
            gen g(newCommand.command.toStdString(),context);
            protecteval(g,1,context);

            CursorItem* cursor;
           if (currentActionTool==FORMAL_CURSOR){
               cursor=new CursorItem(false,this);
           }
           else cursor=new CursorItem(true,this);
           cursor->setVar(var);
           cursor->setLevel(evaluationLevel);
            commands.last().item=cursor;
            CursorPanel* cp=new CursorPanel(var,dialog->getMin().toDouble(),dialog->getMax().toDouble(),
                                            dialog->getStep().toDouble(),dialog->getDefault().toDouble(),cursor);
            cursor->setCursorPanel(cp);
            cursorItems.append(cursor);
            parent->addCursorPanel(cp);
            // Init to first free var
            varLine="a";
            findFreeVar(varLine);
            connect (cp,SIGNAL(valueChanged()),this,SLOT(updateAllChildrenFrom()));
            connect(cp,SIGNAL(deletePanel()),this,SLOT(deleteCursorPanel()));
        }
        delete dialog;
    }

    selectedItems.clear();
}
void Canvas2D::deleteCursorPanel(){
    CursorPanel* cp=dynamic_cast<CursorPanel*>(sender());

    // Remove from cursorItems
    int id=findItemFromVar(cp->getOwner()->getVar(),&cursorItems);
    cursorItems.removeAt(id);

    // init to the first free Var
    varLine="a";
    findFreeVar(varLine);

    // delete from screen
    parent->deleteCursorPanel(cp);

    deleteObject(cp->getOwner());

}

/**
 * @brief Canvas2D::updateAllChildrenFrom
 *                  This method is called when the value from a cursor (numeric or formal)
 *                  has changed, it updates all children from the slider on screen.
 */
void Canvas2D::updateAllChildrenFrom(){
    CursorPanel* cp=dynamic_cast<CursorPanel*>(sender());
    QString s=commands.at(cp->getOwner()->getLevel()).command;
    if (cp->getOwner()->isFormal()){
        int id=s.indexOf("[");
        int id2=s.indexOf(",");
        s.replace(id+1,id2-id-1,QString::number(cp->getValue()));
        gen g(s.toStdString(),context);
        protecteval(g,1,context);
    }
    else{
        int id=s.indexOf(",");
        int id2=s.indexOf(",",id+1);
        s.replace(id+1,id2-id-1,QString::number(cp->getValue()));
        gen g(s.toStdString(),context);
        protecteval(g,1,context);
    }
    updateAllChildrenFrom(cp->getOwner());
    updatePixmap(false);
    parent->updateValueInDisplayPanel();
    repaint();
}

/**
 * @brief Canvas2D::findIDNT Looks for all sub gen of type IDNT in expression
 *                           If this IDNT is a variable name for a geometry MyItem
 *                           then adds as Child
 * @param expression The expression to parse
 * @param item The child created from expression
 */
void Canvas2D::findIDNT(gen &expression,MyItem* item){
    if (expression.type==giac::_SYMB){
        giac::gen g=expression._SYMBptr->feuille;
        findIDNT(g,item);
    }
    else if (expression.type==giac::_VECT){
        giac::vecteur *v=expression._VECTptr;
        vecteur::iterator it;
        for(it=v->begin();it<v->end();it++){
            findIDNT(*it,item);
        }
    }
    else if(expression.type==giac::_IDNT){
        QString s=QString::fromStdString(expression._IDNTptr->name());
        int id=findItemFromVar(s,&pointItems);
        if (id!=-1){
            pointItems.at(id)->addChild(item);
        }
        else {
            id=findItemFromVar(s,&lineItems);
            if (id!=-1){
                lineItems.at(id)->addChild(item);
            }
            else {
                id=findItemFromVar(s,&filledItems);
                if (id!=-1){
                    filledItems.at(id)->addChild(item);
                }
                else{
                    // Look into sliders
                    id=findItemFromVar(s,&cursorItems);
                    if (id!=-1){
                        cursorItems.at(id)->addChild(item);
                    }

                }
            }
        }
    }
}





void Canvas2D::setBounds(const double & xMin, const double &xMax, const double & yMin, const double & yMax){
    xmin=xMin;
    xmax=xMax;
    ymin=yMin;
    ymax=yMax;
}
void Canvas2D::setXAxisParam(const AxisParam & s){
    xAxisParam=s;
}
void Canvas2D::setYAxisParam(const AxisParam & s){
    yAxisParam=s;
}

void Canvas2D::setGridParam(const GridParam & p){
    gridParam=p;
}
AxisParam Canvas2D::getXAxisParam()const{
    return xAxisParam;
}
AxisParam Canvas2D::getYAxisParam() const{
    return yAxisParam;
}
GridParam Canvas2D::getGridParam()const{
    return gridParam;
}
giac::context* Canvas2D::getContext() const{
    return context;
}

void Canvas2D::addToVector(const giac::gen &g,QList <MyItem*> & scene){
//    std::cout<<print(g,context)<<std::endl;
    if (giac::is_undef(g)) {
        scene.append(new UndefItem(this));
        return;
    }
    if (g.type==giac::_VECT){
      giac::vecteur & v =*g._VECTptr;
      const_iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;++it){
            addToVector(*it,scene);
      } // end for it
    }
    if (g.type!=_SYMB)
      return;
    unary_function_ptr s=g._SYMBptr->sommet;
    if (s==at_animation){
        // TODO
        // fltk_draw(Mon_image,plot_i,get_animation_pnt(g,Mon_image.animation_instructions_pos),x_scale,y_scale,clip_x,clip_y,clip_w,clip_h);
      return;
    }
    if (g._SYMBptr->feuille.type!=_VECT)
      return;

    // TODO Attributs
    vecteur f=*g._SYMBptr->feuille._VECTptr;
  //  int mxw=Mon_image.w(),myw=Mon_image.h()-(Mon_image.show_axes?(Mon_image.title.empty()?1:2):0)*Mon_image.labelsize();
  double i0;
  double j0;
  double i0save,j0save,i1,j1;
    int fs=f.size();
    if ((fs==4) && (s==at_parameter)){
      return ;
    }
    std::string the_legend;
    vecteur style(get_style(f,the_legend));
    int styles=style.size();
    // color
    int ensemble_attributs = style.front().val;

    bool hidden_name = false;
    if (style.front().type==_ZINT){
      ensemble_attributs = mpz_get_si(*style.front()._ZINTptr);
      hidden_name=true;
    }
    else
      hidden_name=(ensemble_attributs<0);
    if (hidden_name) ensemble_attributs=-std::abs(ensemble_attributs);
    else ensemble_attributs=std::abs(ensemble_attributs);

    int width           =(ensemble_attributs & 0x00070000) >> 16; // 3 bits
    int epaisseur_point =(ensemble_attributs & 0x00380000) >> 19; // 3 bits
    int type_line       =(ensemble_attributs & 0x01c00000) >> 22; // 3 bits
    if (type_line>4)
      type_line=(type_line-4)<<8;
    int type_point      =(ensemble_attributs & 0x0e000000) >> 25; // 3 bits
    int labelpos        =(ensemble_attributs & 0x30000000) >> 28; // 2 bits
    bool fill_polygon   =(ensemble_attributs & 0x40000000) >> 30;
    int couleur         =(ensemble_attributs & 0x0000ffff);
    epaisseur_point += 2;
    /*
std::pair<Fl_Image *,Fl_Image *> * texture = 0;
    for (int i=2;i<styles;++i){
      gen & attr=style[i];
      if (attr.type==_VECT && attr._VECTptr->size()<=3 ){
        gen attrv0=attr._VECTptr->front();
        if (attrv0.type==_INT_ && attrv0.val==_GL_TEXTURE){
          gen attrv1=(*attr._VECTptr)[1];
          if (attrv1.type==_STRNG){
            get_texture2d(*attrv1._STRNGptr,texture);
          }
          // set texture
          continue;
        } // end attrv0 = gl_texture
      }
    }
*/

    if (s==at_pnt){
      // f[0]=complex pnt or vector of complex pnts or symbolic
      // f[1] -> style
      // f[2] optional=label
      gen point=f[0];

      if (point.type==_VECT && point.subtype==_POINT__VECT)
        return;
      bool isCurve=false;
      if ( (f[0].type==_SYMB) && (f[0]._SYMBptr->sommet==at_curve) && (f[0]._SYMBptr->feuille.type==_VECT) && (f[0]._SYMBptr->feuille._VECTptr->size()) ){
         isCurve=true;

         point=f[0]._SYMBptr->feuille._VECTptr->back();
      }
      if (is_undef(point))
        return;
      /* TODO A comprendre
     if ( equalposcomp(Mon_image.selected,plot_i))
        fl_color(FL_BLUE);
      else
       xcas_color(couleur);
      fl_line_style(type_line,width+1,0);*/
      if (point.type==_SYMB) {
        if (point._SYMBptr->sommet==at_hyperplan || point._SYMBptr->sommet==at_hypersphere)
          return;
        /****  Bezier curve *************
         *
         ********************************/
        if (point._SYMBptr->sommet==at_Bezier){
          point=point._SYMBptr->feuille;
              const_iterateur jt=point._VECTptr->begin(),jtend=point._VECTptr->end();

                bool fillable=false;
                // Cas d'un polygon fermé
                 if ( *jt==*(jtend-1)){
                     fillable=true;
                 }

              if (jt==jtend) return;
              // rest of the path
              QList<QPointF> path;
            gen save2;

              for (;;){
          //      Mon_image.findij(*jt,x_scale,y_scale,i1,j1,context);
                  // segment
                      gen e,f0,f1;
                      evalfdouble2reim(*jt,e,f0,f1,context);
                      // For segment
                      save2=*jt;
                      if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
                           i0=f0._DOUBLE_val;
                           j0=f1._DOUBLE_val;
                            path.append(QPointF(i0,j0));
                       }
                ++jt;
                      if (jt==jtend) break;
          }

              BezierCurve* curve=new BezierCurve(path,this);
              curve->setLevel(evaluationLevel);
              curve->setAttributes(ensemble_attributs);
              scene.append(curve);
            return;


          }
        /*
        *    Circle figures (arc, circle ...)
        *
        */

        else if (point._SYMBptr->sommet==at_cercle){
          vecteur v=*point._SYMBptr->feuille._VECTptr;
          gen diametre=remove_at_pnt(v[0]);
          gen e1=diametre._VECTptr->front().evalf_double(1,context);
          gen e2=diametre._VECTptr->back().evalf_double(1,context);
          gen centre=rdiv(e1+e2,2.0);
          gen e12=e2-e1;
          double ex=evalf_double(re(e12,context),1,context)._DOUBLE_val,ey=evalf_double(im(e12,context),1,context)._DOUBLE_val;
          gen diam=std::sqrt(ex*ex+ey*ey);
          gen angle=std::atan2(ey,ex);
          gen a1=v[1].evalf_double(1,context),a2=v[2].evalf_double(1,context);
          if ( (diam.type==_DOUBLE_) && (a1.type==_DOUBLE_) && (a2.type==_DOUBLE_) ){
            double a1d=a1._DOUBLE_val,a2d=a2._DOUBLE_val,angled=angle._DOUBLE_val;
            bool changer_sens=a1d>a2d;
            if (changer_sens){
              double tmp=a1d;
              a1d=a2d;
              a2d=tmp;
            }
            QPointF c(evalf_double(re(centre,context),1,context)._DOUBLE_val, evalf_double(im(centre,context),1,context)._DOUBLE_val);

            Circle* circle=new Circle(c,diam._DOUBLE_val,angled+a1d,angled+a2d,this);
            circle->setLevel(evaluationLevel);
             circle->setAttributes(ensemble_attributs);
            circle->setLegend(QString::fromStdString(the_legend));
             circle->setValue(_equation(g,context));
            scene.append(circle);

            return;
/*            if (fill_polygon)
              fl_pie(deltax+round(i0-i1),deltay+round(j0-j1),round(2*i1),round(2*j1),(angled+a1d)*180/M_PI,(angled+a2d)*180/M_PI);
            else {
              double anglei=(angled+a1d),anglef=(angled+a2d);
              fl_arc(deltax+round(i0-i1),deltay+round(j0-j1),round(2*i1),round(2*j1),anglei*180/M_PI,anglef*180/M_PI);
              if (v.size()>=4){ // if cercle has the optionnal 5th arg
                if (changer_sens)
                  petite_fleche(i0+i1*std::cos(anglei),j0-j1*std::sin(anglei),-i1*std::sin(anglei),-j1*std::cos(anglei),deltax,deltay,width);
                else
                  petite_fleche(i0+i1*std::cos(anglef),j0-j1*std::sin(anglef),i1*std::sin(anglef),j1*std::cos(anglef),deltax,deltay,width);
              }
            }*/
            // Label a few degrees from the start angle,
            // FIXME should use labelpos
    //        i0=i0+i1*std::cos(angled+a1d+0.3);
    //        j0=j0-j1*std::sin(angled+a1d+0.3);
    //        if (!hidden_name)
    //          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
    //        return;
          }

        } // end circle
        /*
        *    Pixon
        *
        */

        else if (point._SYMBptr->sommet==at_pixon){
        //    pixon (i,j,color)
          if (point._SYMBptr->feuille.type!=_VECT)
            return;
          vecteur &v=*point._SYMBptr->feuille._VECTptr;
          if (v.size()<3 ||  v[2].type!=_INT_)
            return;
          if (v[0].type!=_INT_ || v[1].type!=_INT_){
              double delta_i=v[0]._DOUBLE_val,delta_j=v[1]._DOUBLE_val;
              Pixel* pix=new Pixel(QPointF(delta_i,delta_j),this);
              pix->setAttributes(v[2].val);
              pix->setLevel(evaluationLevel);
             scene.append(pix);
          }
          else{
              int delta_i=v[0].val,delta_j=v[1].val;
              Pixel* pix=new Pixel(QPointF(delta_i,delta_j),this);
              pix->setAttributes(v[2].val);
              pix->setLevel(evaluationLevel);
            scene.append(pix);
          }
          return;
        }
        else if (point._SYMBptr->sommet==at_bitmap){
         /*

            // bitmap(vector of int (1 per line)), 1st line, 1st col, [type]
          if (point._SYMBptr->feuille.type!=_VECT)
            return;
          vecteur &v=*point._SYMBptr->feuille._VECTptr;
          if (v.size()<3 || v[0].type!=_VECT || v[1].type!=_INT_ || v[2].type!=_INT_ )
            return;
          int delta_i=v[1].val,delta_j=v[2].val;
          double xmin=Mon_image.window_xmin,ymin=Mon_image.window_ymin,xmax=Mon_image.window_xmax,ymax=Mon_image.window_ymax;
          //gen psize=_Pictsize(0);
          int bitmap_w=Mon_image.w()-int(Mon_image.ylegende*(Mon_image.show_axes?Mon_image.labelsize():0)),
            bitmap_h=Mon_image.h()-(Mon_image.show_axes?((Mon_image.title.empty()?1:2)*Mon_image.labelsize()):0);
          if (v.size()>8){
            xmin=v[3]._DOUBLE_val;
            xmax=v[4]._DOUBLE_val;
            ymin=v[5]._DOUBLE_val;
            ymax=v[6]._DOUBLE_val;
            bitmap_w=v[7].val;
            bitmap_h=v[8].val;
          }
          double bitmap_scalex=(xmax-xmin)/bitmap_w,scalex=(Mon_image.window_xmax-Mon_image.window_xmin)/(Mon_image.w()-int(Mon_image.ylegende*(Mon_image.show_axes?Mon_image.labelsize():0)));
          double bitmap_scaley=(ymax-ymin)/bitmap_h,scaley=(Mon_image.window_ymax-Mon_image.window_ymin)/(Mon_image.h()-(Mon_image.show_axes?((Mon_image.title.empty()?1:2)*Mon_image.labelsize()):0));
          double X,Y;
          int ii,jj;
          const_iterateur it=v[0]._VECTptr->begin(),itend=v[0]._VECTptr->end();
          for (;it!=itend;++it,++delta_i){
            if (it->type!=_INT_ && it->type!=_ZINT)
              continue;
            gen z=*it;
            mpz_t zz,zr;
            if (it->type==_INT_)
              mpz_init_set_ui(zz,it->val);
            else
              mpz_init_set(zz,*it->_ZINTptr);
            mpz_init(zr);
            for (int j=delta_j;mpz_sgn(zz);++j){
              mpz_tdiv_r_2exp (zr, zz, 1);
              mpz_tdiv_q_2exp (zz, zz, 1);
              if (mpz_sgn(zr)){
                X=xmin+j*bitmap_scalex;
                ii=int(0.5+(X-Mon_image.window_xmin)/scalex);
                Y=ymax-delta_i*bitmap_scaley;
                jj=int(0.5+(Mon_image.window_ymax-Y)/scaley);
                if (ii>0 && ii<mxw && jj>0 && jj<myw)
                  check_fl_point(deltax+ii,deltay+jj,clip_x,clip_y,clip_w,clip_h,0,0);
              }
            }
            mpz_clear(zr);
            mpz_clear(zz);
          }
          return;
          */
        } // end bitmap

        /*
        *    legend
        *    eg: legend([30,100],"Hello");
        */

        else if (point._SYMBptr->sommet==at_legende){
            gen & f=point._SYMBptr->feuille;
            if (f.type==_VECT && f._VECTptr->size()==3){
              vecteur & fv=*f._VECTptr;
              if (fv[0].type==_VECT && fv[0]._VECTptr->size()>=2 && fv[1].type==_STRNG && fv[2].type==_INT_){
                vecteur & fvv=*fv[0]._VECTptr;
                if (fvv[0].type==_INT_ && fvv[1].type==_INT_){
                    LegendItem* item=new LegendItem(QPoint(fvv[0].val,fvv[1].val),QString::fromStdString(*fv[1]._STRNGptr),this);
                    item->setAttributes(ensemble_attributs);
                    item->setLevel(evaluationLevel);
                    scene.append(item);
                }
              }
            }

        }
      } // end point.type==_SYMB
      if (point.type!=_VECT){ // single point
          gen e,f0,f1;

          evalfdouble2reim(point,e,f0,f1,context);
          if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
              Point* pt=new Point(point,this);
              pt->setAttributes(ensemble_attributs);
               pt->setLegend(QString::fromStdString(the_legend));
               pt->setLevel(evaluationLevel);
               scene.append(pt);
          }

/*        Mon_image.findij(point,x_scale,y_scale,i0,j0,context);
        if (i0>0 && i0<mxw && j0>0 && j0<myw)
          fltk_point(deltax,deltay,round(i0),round(j0),epaisseur_point,type_point);
        if (!hidden_name)
          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);*/
        return;
      }
      // path
      const_iterateur jt=point._VECTptr->begin(),jtend=point._VECTptr->end();
      if (jt==jtend)
        return;
  /*    if (texture && jtend-jt>2){
        // use *jt and *(jt+2) for the rectangle texture
        Mon_image.findij(*jt,x_scale,y_scale,i0,j0,context);
        Mon_image.findij(*(jt+2),x_scale,y_scale,i1,j1,context);
        if (i0>i1)
          std::swap(i0,i1);
        if (j0>j1)
          std::swap(j0,j1);
        int tx=int(i0+.5)+deltax;
        int tw=int(i1-i0+.5);
        int ty=int(j0+.5)+deltay;
        int th=int(j1-j0+.5);
        if (texture->second && texture->second->w()==tw && texture->second->h()==th)
          texture->second->draw(tx,ty,tw,th);
        else {
          if (texture->second)
            delete texture->second;
          texture->second=texture->first->copy(tw,th);
          texture->second->draw(tx,ty,tw,th);
        }
        return;
      }*/
      if (jt->type==_VECT)
        return;
      // initial point
      gen e,f0,f1;

      evalfdouble2reim(*jt,e,f0,f1,context);
      if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
           i0=f0._DOUBLE_val;
           j0=f1._DOUBLE_val;
       }
        bool fillable=false;

        // Cas d'un polygon fermé
         if ( *jt==*(jtend-1)){
             fillable=true;
/*        const_iterateur jtsave=jt;
        gen e,f0,f1;
        // Compute matrix for complex drawing
        fl_push_matrix();
        fl_translate(deltax,deltay);
        fl_mult_matrix(x_scale,0,0,-y_scale,0,0);
        fl_translate(-Mon_image.window_xmin,-Mon_image.window_ymax);
        fl_begin_complex_polygon();
        for (;jt!=jtend;++jt){
          evalfdouble2reim(*jt,e,f0,f1,context);
          if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_))
            fl_vertex(f0._DOUBLE_val,f1._DOUBLE_val);
        }
        fl_end_complex_polygon();
        fl_pop_matrix(); // Restore initial matrix
        if (!width){
          if (!hidden_name)
            draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
          return;
        }
        jt=jtsave;
        fl_line_style(type_line,width,0);
        fl_color(epaisseur_point-2+(type_point<<3));*/
      }
         gen save(*jt);

//      i0save=i0;
//      j0save=j0;
      ++jt;

      if (jt==jtend){


          // if (i0>0 && i0<width && j0>0 && j0<myw)
            Point* pt=new Point(save,this);
            pt->setAttributes(ensemble_attributs);
            pt->setLegend(QString::fromStdString(the_legend));
            pt->setLevel(evaluationLevel);
            pt->setValue(save);
            scene.append(pt);
            //       if (!hidden_name)
//          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
        return;
      }
      bool seghalfline=(point.subtype==_LINE__VECT || point.subtype==_HALFLINE__VECT) && (point._VECTptr->size()==2);
      if (seghalfline){

          QPointF startPoint,endPoint;

          startPoint=QPointF(i0,j0);
          gen e,f0,f1;
          evalfdouble2reim(*jt,e,f0,f1,context);
          if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
               i0=f0._DOUBLE_val;
               j0=f1._DOUBLE_val;

             //  (QPointF(i0,j0));
           }
          endPoint=QPointF(i0,j0);
          if (point.subtype==_LINE__VECT ) {
              LineItem* line=new LineItem(startPoint,endPoint,this);
              line->setValue(_equation(g,context));
              line->setAttributes(ensemble_attributs);
              line->setLegend(QString::fromStdString(the_legend));
              line->setLevel(evaluationLevel);
              scene.append(line);
          }
          else {
              HalfLineItem* line=new HalfLineItem(startPoint,endPoint,this);
              line->setValue(_equation(g,context));
              line->setAttributes(ensemble_attributs);
              line->setLegend(QString::fromStdString(the_legend));
              line->setLevel(evaluationLevel);
              scene.append(line);
          }

        return;
      }




      // rest of the path

      QPainterPath path;
      if (!seghalfline) {
          path.moveTo(i0,j0);
      }

    gen save2;
      for (;;){
  //      Mon_image.findij(*jt,x_scale,y_scale,i1,j1,context);
          // segment
              gen e,f0,f1;
              evalfdouble2reim(*jt,e,f0,f1,context);
              // For segment
              save2=*jt;
              if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
                   i0=f0._DOUBLE_val;
                   j0=f1._DOUBLE_val;

                   path.lineTo(QPointF(i0,j0));
               }
        ++jt;

        if (jt==jtend){ // label of line at midpoint
        /*  if (point.subtype==_LINE__VECT){
            i0=(6*i1-i0)/5-8;
            j0=(6*j1-j0)/5-8;
          }
          else {
            i0=(i0+i1)/2-8;
            j0=(j0+j1)/2;
          }*/
          break;
        }
        i0=i1;
        j0=j1;
      }


      Curve *curve=new Curve(path,this);


      // For a segment
     if ( (point.subtype==_GROUP__VECT) && (point._VECTptr->size()==2)){

           curve->setValue(giac::_simplify(giac::abs(save2-save,context),context));
    }
       // no legend for segment
      else {
          if (!hidden_name){}
      //    draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
      }

      curve->setAttributes(ensemble_attributs);
      curve->setLegend(QString::fromStdString(the_legend));
      curve->setLevel(evaluationLevel);
      if (point.subtype==_VECTOR__VECT){
              curve->setVector(true);
              curve->setValue(giac::_coordonnees(save2-save,context));
      }
      if (!isCurve&&path.elementCount()!=2){
          curve->setPolygon(true);
          curve->setFillable(fillable);
      }
      if (isCurve){
          QString mml("<math mode=\"display\">\n<mfenced open=\"{\" close=\"\"><mtable>\n<mtr><mtd><mfenced open=\"(\" close=\")\"><mrow>");
          giac::gen first=f[0]._SYMBptr->feuille._VECTptr->front();
          //mml.append(QString::fromStdString(gen2mathml(value[0].re(context),context)));
          mml.append("<mo>;</mo>");
          //mml.append(QString::fromStdString(gen2mathml(value[0].im(context),context)));
          mml.append("</mrow></mfenced></mtd></mtr>\n<mtr><mtd><mrow>");
       //   mml.append(QString::fromStdString(gen2mathml(value[2],context)));
          mml.append("<mo>&lt;</mo>");
        //  mml.append(QString::fromStdString(gen2mathml(value[1],context)));
          mml.append("<mo>&lt;</mo>");
         // mml.append(QString::fromStdString(gen2mathml(value[3].re(context),context)));
          mml.append("</mrow></mtd></mtr>\n</mtable></mfenced></math>");
      //    qDebug()<<mml;
          gen value(makevecteur(first[0].re(context),first[0].im(context)));
          value.subtype=giac::_SEQ__VECT;
          curve->setValue(value);
      }
        scene.append(curve);

    } // end pnt subcase
//*/
  }
double Canvas2D::getXmin() const{
    return xmin;
}
double Canvas2D::getXmax() const{
    return xmax;
}
double Canvas2D::getYmin() const{
    return ymin;
}
double Canvas2D::getYmax() const{
    return ymax;
}
QList<Canvas2D::Command> &Canvas2D::getCommands(){
    return commands;
}

QList<MyItem*>* Canvas2D::getPointItem() {
    return &pointItems;

}
QList<MyItem*>* Canvas2D::getFilledItem(){
    return &filledItems;

}

QList<MyItem*>* Canvas2D::getLineItem(){
    return &lineItems;

}

double Canvas2D::find_tick(double dx){
  double res=std::pow(10.0,std::floor(std::log10(std::abs(dx))));
  int nticks=int(dx/res);
  if (nticks<4)
    res/=5;
  else {
    if (nticks<8)
      res/=2;
  }
  return res;
}
/*QSize Canvas2D::sizeHint() const{
    return QSize(width(),height());
}*/

QSize Canvas2D::minimumSizeHint() const{
    return QSize(200,100);
}




void Canvas2D::createScene(const giac::gen & g){
    setFixedSize(Config::graph_width,Config::graph_width*3/4);

    // Find the largest and lowest x/y/z in objects (except lines/plans)
    std::vector<double> vx,vy,vz;

    ortho=giac::autoscaleg(g,vx,vy,vz,context);



    giac::autoscaleminmax(vx,xmin,xmax);

    giac::autoscaleminmax(vy,ymin,ymax);
//    qDebug()<<xmin<<xmax<<ymin<<ymax;
//    autoscaleminmax(vz,zmin,zmax);


    setXYUnit();
    if (ortho) {
        make_ortho();
        ortho=true;
    }

/*    if (std::isnan(xunit)||std::isnan(yunit)|| (!std::isfinite(xunit)) || (!std::isfinite(yunit))){
      xmin=-5;ymin=-5;xmax=5;ymax=5;
       setXYUnit();
    }*/
    QList<MyItem*> v;
    addToVector(g,v);
    addToScene(v);

     updatePixmap(true);
}
void Canvas2D::addToScene(QList<MyItem *> & v){
    if (parent->isInteractive()){
        if (v.size()!=1) {
        ListItem *list=new ListItem(v,this);
        if (list->isFillable()) filledItems.append(list);
        else if (checkForOnlyPoints(&v)) pointItems.append(list);
        else lineItems.append(list);
        parent->addToTree(list);
        }
        else{
            MyItem *item=v.at(0);
            if (item->isFillable()) filledItems.append(item);
            else if (item->isPoint()) pointItems.append(item);
            else lineItems.append(item);
            focusOwner=item;
            parent->addToTree(item);
        }
    }
    else{
        for (int i=0;i<v.size();++i){
            MyItem * item =v.at(i);
            parent->addToTree(item);

            if (item->isFillable()) filledItems.append(item);
            else if (item->isPoint()) pointItems.append(item);
            else lineItems.append(item);
        }
    }
}


/*****************          **************
  ******            SLOTS       **********
  ****************          **************/

void Canvas2D::zoom_In(){

    if (selectionRight){
        double tmpx,tmpy,tmpx2,tmpy2;
        toXY(startSel.x(),startSel.y(),tmpx,tmpy);
        toXY(endSel.x(),endSel.y(),tmpx2,tmpy2);



        if (tmpx2>tmpx) {
            xmin=tmpx;
            xmax=tmpx2;
        }
        else {
            xmin=tmpx2;
            xmax=tmpx;
        }
        if (tmpy2>tmpy) {
            ymin=tmpy;
            ymax=tmpy2;
        }
        else {
            ymin=tmpy2;
            ymax=tmpy;
        }
    }
    else {
        double xstep=(xmax-xmin)/10;
        double ystep=(ymax-ymin)/10;
        xmin+=xstep;
        xmax-=xstep;
        ymin+=ystep;
        ymax-=ystep;
    }
    selectionRight=false;
    setXYUnit();
    updatePixmap(true);
    this->repaint();

}
void Canvas2D::make_ortho(){

    if (xunit>yunit){
        xunit=yunit;
        double step=((width()-40)/xunit-(xmax-xmin))/2;
        xmin-=step;
        xmax+=step;
    }
    else if (xunit<yunit){
        yunit=xunit;
        double step=((height()-40)/yunit-(ymax-ymin))/2;
        ymin-=step;
        ymax+=step;

    }
    updatePixmap(true);
    repaint();

}
void Canvas2D::zoom_Out(){
    selectionRight=false;
    double xstep=(xmax-xmin)/8;
    double ystep=(ymax-ymin)/8;
    xmin-=xstep;
    xmax+=xstep;
    ymin-=ystep;
    ymax+=ystep;
    setXYUnit();
    updatePixmap(true);
    this->repaint();

}
void Canvas2D::displayAxis(bool b){
    xAxisParam.isVisible=b;
    yAxisParam.isVisible=b;
//    parent->updateValueInDisplayPanel();
    updatePixmap(false);
    repaint();


}
void Canvas2D::displayGrid(bool b){
    gridParam.isVisible=b;
 //   parent->updateValueInDisplayPanel();
     updatePixmap(false);
    repaint();
}
bool lessThan(const MyItem* a, const MyItem*b){
    return a->getLevel()<b->getLevel();
}
int Canvas2D::findItemFromVar(const QString& var,QList<MyItem*>* list){
    for (int i=0;i<list->size();++i){
        if (list->at(i)->getVar()==var) return i;
    }
    return -1;
}


void Canvas2D::renameObject(MyItem * item, const QString & var){
    QString copy(var);
    gen newName(var.toStdString(),context);
    gen value=eval(newName,1,context);
    // If the variable is already busy
    if (value!=newName){
        copy.append("1");
        findFreeVar(copy);
        int id=findItemFromVar(var,&pointItems);
        if (id!=-1){
            renameObject(pointItems.at(id),copy);
        }
        else {
            id=findItemFromVar(var,&lineItems);
            if (id!=-1){
               renameObject(lineItems.at(id),copy);
            }
            else {
                id=findItemFromVar(var,&filledItems);
                if (id!=-1){
                   renameObject(filledItems.at(id),copy);
                }
                else {
                    QMessageBox::warning(this,tr("Impossible de renommer le point"),tr("Le nom de variable choisi est actuellement indisponible.\n"
                                                          "La variable est déjà utilisée pour un objet externe à cette feuille de géométrie."),QMessageBox::Ok,QMessageBox::NoButton);
                    return;
                }
            }
        }

      }
    // modify all references in commands
    Command c=commands.at(item->getLevel());
    QString s(c.command);
    int id=s.indexOf(":=");
    if (id!=-1){
        s=QString(var).append(s.right(s.length()-id));
        c.command=s;
        commands.replace(item->getLevel(),c);
    }
    for (int i=0;i<item->getChildren().size();++i){
        int level=item->getChildAt(i)->getLevel();
         c=commands.at(level);
        s=c.command;
        int id=s.indexOf("(");
        if (id!=-1){
            id=s.indexOf(item->getVar(),id);
            while (id!=-1){
                QChar c=s.at(id+item->getVar().length());
                if (c==')'||c==',') {
                    s.remove(id,item->getVar().length());
                    s.insert(id,var);
                }
                id=s.indexOf(item->getVar(),id+var.length());

            }
           c.command=s;
           commands.replace(item->getChildAt(i)->getLevel(),c);
        }
    }




    // modify var in giac
    gen oldName(item->getVar().toStdString(),context);
    giac::sto(eval(oldName,1,context),newName,context);
    giac::_purge(oldName,context);

    // modify var and Legend
    item->setVar(var);
    item->setLegend(var);


    // Initialize var to first available letter
    if (item->isPoint()){
        varPt="A";
        findFreeVar(varPt);
    }
    else{
        varLine="A";
        findFreeVar(varLine);


    }


    parent->renameInTree(item);
    updatePixmap(false);
    repaint();


}



void Canvas2D::renameObject(){
    OneArgDialog *dialog=new OneArgDialog(this,tr("Nouveau nom:"));
    if (dialog->exec()){
        renameObject(focusOwner,dialog->editRadius->text());

    }
    delete dialog;
}
void Canvas2D::trace(bool b) {
    focusOwner->setTraceActive(b);
    if (b)
        traceVector.append(focusOwner);
    else {
        int id=traceVector.indexOf(focusOwner);
        if (id!=-1)
            traceVector.removeAt(id);
       repaint();
    }
}

void Canvas2D::deleteObject(){
    deleteObject(focusOwner);
}
void Canvas2D::deleteObject(MyItem * focus){
    // Display warning about deleting objects
    if (focus->hasChildren()){
        int r=QMessageBox::warning(this,tr("Confirmation"),tr("Vous allez supprimer cet objet ainsi \n"
                                            "que toutes ses dépendances. \n Voulez-vous poursuivre?"),QMessageBox::Yes|QMessageBox::Default,QMessageBox::Cancel|QMessageBox::Escape);
        if (r!=QMessageBox::Yes) return;
    }

    int start=focus->getLevel();

   // First, find if this item comes from an intersection
    if (focus->isFromInter()){
        for (int i=0;i<pointItems.size();++i){
            MyItem * item=pointItems.at(i);
            if (item->isInter()&&item->getChildren().contains(focus)){
                // Only one child, we delete the intersection point too
                if (item->getChildren().size()==1) {
                        for (int i=0;i<start;++i){
                            MyItem* tmp=commands.at(i).item;
                            tmp->deleteChild(item);
                        }
                        pointItems.removeAt(pointItems.indexOf(item));
//                        commands.removeAt(start);
                        delete item;
                 }
                // Several childs, only delete this point
                  else{

                    item->deleteChild(focus);
                }
                break;
                // End only one child
            }
        }
    }


    // Find all children and delete them
    QList<MyItem*> v;

    v.append(focus);
    refreshFromItem(focus,v,true);
    qSort(v.begin(),v.end(),lessThan);

    for (int i=v.size()-1;i>=0;--i){
        int level=v.at(i)->getLevel();
        // First, disconnect object from all its potential parents
        for (int j=0;j<level;++j){
                MyItem* item=commands.at(j).item;
                item->deleteChild(v.at(i));
         }

        // delete the command
        if (!v.at(i)->isFromInter())
            commands.removeAt(level);
        // delete from tree
        parent->removeFromTree(v.at(i));

        //delete from pointItems, lineItems, vectorItems
        if (v.at(i)->isPoint()){
            int id=pointItems.indexOf(v.at(i));
            if (id!=-1)  pointItems.removeAt(id);
            id= traceVector.indexOf(v.at(i));
            if (id!=-1)  traceVector.removeAt(id);

        }
        else {
            int id=lineItems.indexOf(v.at(i));
            if (id!=-1)  lineItems.removeAt(id);
            id=filledItems.indexOf(v.at(i));
            if (id!=-1)  filledItems.removeAt(id);

        }
        // delete from giac memory

        giac::_purge(gen(v.at(i)->getVar().toStdString(),context),context);

        // And then delete the pointer
        delete v.at(i);
    }

    // Finally, updates all level
    for (int i=start;i<commands.size();++i){
        MyItem *item=commands.at(i).item;
        item->setLevel(i);
        if (item->isInter()){
            for (int j=0;j<item->getChildren().size();++j){
                item->getChildAt(j)->setLevel(i);
            }
        }
    }

    selectedItems.clear();
    focusOwner=0;
    updatePixmap(false);
    repaint();
    varPt="A";
    varLine="a";
    findFreeVar(varPt);
    findFreeVar(varLine);
    parent->updateValueInDisplayPanel();
}




void Canvas2D::displayLegend(bool b){
    focusOwner->setLegendVisible(b);
//    parent->updateValueInDisplayPanel();
     updatePixmap(false);
     repaint();
}
void Canvas2D::exportToPNG(){
    QString fileName=QFileDialog::getSaveFileName(this,tr("Enregistrer sous..."),".png");
    if (fileName.isEmpty()){
        return;
    }
    pixmap.save(fileName);

}
void Canvas2D::displayObject(bool b){
    focusOwner->setVisible(b);
     updatePixmap(false);
     repaint();

}
void Canvas2D::displaySource(){
    SourceDialog* sourceDialog=new SourceDialog(this);
    sourceDialog->exec();
    delete(sourceDialog);
}

//void GraphWidget::zoom_Factor(const int &){}


//void Canvas2D::displayContextMenu(const QPoint &pos){
//
//}
void Canvas2D::updatePixmap(const bool &compute){
    pixmap=QPixmap(size());
    pixmap.fill(this,0,0);
    QPainter painter(&pixmap);    
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setClipRect(20,20,width()-40,height()-40);
    drawGrid(&painter);
    drawElements(filledItems,&painter,compute);
    drawAxes(&painter);
    drawElements(lineItems,&painter,compute);
    drawElements(pointItems,&painter,compute);
}


void Canvas2D::drawAxes(QPainter * painter){

    if ((xmin<0)& (xmax>0) && (yAxisParam.isVisible)){
        double a,b;
        painter->setPen(QPen(yAxisParam.color,1, Qt::SolidLine ,Qt::RoundCap));
        double step=yAxisParam.tick;
        if (!isInteractive()){
            step=(ymax-ymin)/8;
            double tenPower=pow(10,floor(log10(step)));
            int first=floor(step/tenPower);
            if (step/tenPower-first>0.5) step=(first+1)*tenPower;
            else step=first*tenPower;
        }
        for (int i=floor(ymin/step);i<=floor(ymax/step);i++){
            double grad=step*i;
            if (grad>ymin && grad<ymax){
                toScreenCoord(0,grad,a,b);
                QPointF p1=QPointF(a,b);
                QPointF p2=QPointF(a+3,b);
                painter->drawLine(p1,p2);
                QString s=QString::number((grad));
                if (!yAxisParam.unitSuffix.isEmpty()) s.append(" ").append(yAxisParam.unitSuffix);
                if (i%2==0) {
                    //Problem of the origin
                    if ((ymin<0) & (ymax>0) && (grad==0)){
                        //TODO
                        painter->drawText(QPointF(a+6,b-painter->fontMetrics().height()+painter->fontMetrics().ascent()/2.0),s);
                    }
                    else painter->drawText(QPointF(a+6,b+painter->fontMetrics().ascent()/2.0),s);

                }
            }

        }
        // Draw axis
        toScreenCoord(0,ymin,a,b);
        QPointF p1=QPointF(a,b);
        toScreenCoord(0,ymax,a,b);
        QPointF p2=QPointF(a,b);
        painter->drawLine(p1,p2);

        QPainterPath p;
        p.moveTo(p2);
        p.lineTo(p2.x()+6,p2.y()+12);
        p.lineTo(p2.x()-6,p2.y()+12);
        p.closeSubpath();
        painter->setBrush(QBrush(yAxisParam.color,Qt::SolidPattern));
        painter->drawPath(p);
        // Draw Legend
        if (!yAxisParam.legend.isEmpty()){
            p2=QPointF(p2.x()+10,p2.y()+painter->fontMetrics().height());
            painter->drawText(p2,yAxisParam.legend);
        }



    }
    if ((ymin<0) && (ymax>0) && (xAxisParam.isVisible)){
        double a,b;
        painter->setPen(QPen(xAxisParam.color,1, Qt::SolidLine ,Qt::RoundCap));
        double step=xAxisParam.tick;
        if (!isInteractive()){
            step=(xmax-xmin)/8;
            double tenPower=pow(10,floor(log10(step)));
            int first=floor(step/tenPower);
            if (step/tenPower-first>0.5) step=(first+1)*tenPower;
            else step=first*tenPower;
        }

        for (int i=floor(xmin/step);i<=floor(xmax/step);i++){
            double grad=step*i;
            if (grad>xmin && grad<xmax){
                toScreenCoord(grad,0,a,b);
                QPointF p1=QPointF(a,b);
                QPointF p2=QPointF(a,b-3);
                painter->drawLine(p1,p2);
                QString s=QString::number((grad));
                if (!xAxisParam.unitSuffix.isEmpty()) s.append(" ").append(xAxisParam.unitSuffix);
                if (i%2==0) {
                    //Problem of the origin
                    if ((xmin<0) & (xmax>0) && (grad==0)){
                        //TODO
                        painter->drawText(QPointF(a+5,b+6+painter->fontMetrics().height()/2),s);

                    }
                    else painter->drawText(QPointF(a-painter->fontMetrics().width(s)/2,b+6+painter->fontMetrics().height()/2),s);
                }
            }
        }
        // Draw axis
        toScreenCoord(xmin,0,a,b);
        QPointF p1=QPointF(a,b);
        toScreenCoord(xmax,0,a,b);
        QPointF p2=QPointF(a,b);
        painter->drawLine(p1,p2);

        QPainterPath p;
        p.moveTo(p2);
        p.lineTo(p2.x()-12,p2.y()-6);
        p.lineTo(p2.x()-12,p2.y()+6);
        p.closeSubpath();
        painter->setBrush(QBrush(xAxisParam.color,Qt::SolidPattern));
        painter->drawPath(p);
        // Draw Legend
        if (!xAxisParam.legend.isEmpty()){
            p2=QPointF(p2.x()-painter->fontMetrics().width(xAxisParam.legend),p2.y()-painter->fontMetrics().height()-5);
            painter->drawText(p2,xAxisParam.legend);
        }
    }
}
void Canvas2D::drawGrid(QPainter * painter){
    if (!gridParam.isVisible) return;
    QPen pen(gridParam.color,1);
    if (gridParam.line==0) pen.setStyle(Qt::SolidLine);
    else if (gridParam.line==1) pen.setStyle(Qt::DashLine);
    else if (gridParam.line==2) pen.setStyle(Qt::DotLine);
    else if (gridParam.line==3) pen.setStyle(Qt::DashDotLine);
    else if (gridParam.line==4) pen.setStyle(Qt::DashDotDotLine);
    else if (gridParam.line==5) pen.setCapStyle(Qt::FlatCap);
    else if (gridParam.line==6) pen.setCapStyle(Qt::RoundCap);
    else if (gridParam.line==7) pen.setCapStyle(Qt::SquareCap);
    painter->setPen(pen);
    if (gridParam.isCartesian){
        double i=(int)(xmin/gridParam.x)*gridParam.x;
        double a,b,c,d;
        toScreenCoord(0,ymin,c,a);
        toScreenCoord(0,ymax,c,b);
        while (i<xmax){
            toScreenCoord(i,0,c,d);
            painter->drawLine(c,a,c,b);
            i+=gridParam.x;
        }
        i=(int)(ymin/gridParam.y)*gridParam.y;
        toScreenCoord(xmin,0,a,c);
        toScreenCoord(xmax,0,b,c);

        while (i<ymax){
            double c,d;
            toScreenCoord(0,i,c,d);
            painter->drawLine(a,d,b,d);
            i+=gridParam.y;
        }

    }
    else {
        double X=std::max(std::abs(xmin),std::abs(xmax));
        double Y=std::max(std::abs(ymax),std::abs(ymin));
        double i=gridParam.r;

        while( i<std::sqrt(X*X+Y*Y)){
            double a,b;
            toScreenCoord(-i,i,a,b);
            double w,h;
            toScreenCoord(i,-i,w,h);
            w=w-a;
            h=h-b;
            painter->setClipRect(20,20,width()-40,height()-40);
            painter->drawEllipse(a,b,w,h);
             i+=gridParam.r;
        }
        switch(gridParam.theta){
            case PI_12:drawPolarLine(12,painter);
            break;
        case PI_8:drawPolarLine(8,painter);
            break;
           case PI_6:drawPolarLine(6,painter);
            break;
            case PI_4:drawPolarLine(4,painter);
            break;
            case PI_3:drawPolarLine(3,painter);
            break;
            case PI_2:drawPolarLine(2,painter);
            break;
        }

    }
}

void Canvas2D::drawPolarLine(const int &frac,QPainter * painter ){
    QVector<double> v;
    double theta=3.14159265/frac;
    double angle=0;

     for (int i=0;i<frac;++i){

         if ((2*i)!=frac) {

             double tan=std::tan(angle);
             if (isInScene(xmin,tan*xmin)){
                v.append(xmin);v.append(tan*xmin);
            }
            if (isInScene(xmax,tan*xmax)){
                v.append(xmax);v.append(tan*xmax);
             }
            if (isInScene(ymin/tan,ymin)){
                v.append(ymin/tan);v.append(ymin);
            }
            if (isInScene(ymax/tan,ymax)){
                v.append(ymax/tan);v.append(ymax);
            }

            if (v.size()==4){
                double a,b,c,d;
                toScreenCoord(v.at(0),v.at(1),a,b);
                toScreenCoord(v.at(2),v.at(3),c,d);

                 painter->drawLine(QPointF(a,b),QPointF(c,d));
           }
           v.clear();
         }
         else {
             double a,b,c,d;
             toScreenCoord(0,ymin,a,b);
             toScreenCoord(0,ymax,c,d);
             painter->drawLine(QPointF(a,b),QPointF(c,d));
            }
         angle+=theta;
     }

}


bool Canvas2D::isInScene(const double & x,const double & y){
//    qDebug()<<x<<y<<(((x>=xmin)&&(x<=xmax))&&((y>=ymin)&&(y<=ymax)));
    return ((x>=xmin)&&(x<=xmax))&&((y>=ymin)&&(y<=ymax));
}


void Canvas2D::drawElements(QList<MyItem*> & v,QPainter* painter,const bool &compute){
//    painter->setClipping(true);
//    painter->setClipRect(20,20,width()-20,height()-20);
    if (v.isEmpty()) return;
    for(int i=0;i<v.size();++i){
        if (!v.at(i)->isUndef())
        {
            v.at(i)->updateScreenCoords(compute);
            v.at(i)->draw(painter);
        }
    }
}
/**
  Convert pixel screen coords (xscreen,yscreen) into coord (x,y)
  **/

void Canvas2D::toXY(const double xscreen,const double yscreen,double&  x, double& y){
    x=xmin+(xscreen-20)/xunit;
    y=ymax-(yscreen-20)/yunit;

}

/**
  Convert coord (x,y) in pixel screen coords (xscreen,yscreen)
  **/
void Canvas2D::toScreenCoord(const double x,const double y,double& xScreen, double& yScreen){
//    qDebug()<<"unités"<<xmin<<xmax<<xunit<<yunit<<zoom;
    xScreen=20+(x-xmin)*xunit;
    yScreen=20+(ymax-y)*yunit;
}

void Canvas2D::setXYUnit(){
    xunit=(width()-40)/(xmax-xmin);
    yunit=(height()-40)/(ymax-ymin);
}
/**
  * In interactive mode: Points are labeled consecutively as follow:
  *     A,B,...,Z,A1,....,Z1, A2 .....
  *
  * Same model for lines but in lower case
  **/
void Canvas2D::findFreeVar(QString & var){
    gen g(var.toStdString(),context);
    while ((eval(g,1,context)!=g)||(findItemFromVar(var,&cursorItems)!=-1)) {
        incrementVariable(var);
        g=gen(var.toStdString(),context);
    }
}
/**
  * Increments the label for points or lines
  *   eg: D4 becomes D5
  *       z becomes a1
  */

void Canvas2D::incrementVariable(QString & var){
    QChar c=var.at(0);
    if (c=='Z'){
        QString s="A";
        s.append(QString::number(QString(var.right(var.length()-1)).toInt()+1));
        var=s;
    }
    else if (c=='z'){
        QString s="a";
        s.append(QString::number(QString(var.right(var.length()-1)).toInt()+1));
        var=s;
    }
    // letter i can't label a variable, it stands for complex number
    else if (c=='h'&&var.length()==1) var="j";
    // letter e can't label a variable, it stands for function exp
    else if  (c=='d'&&var.length()==1) {var="f";}
    else var[0]=QChar(c.toAscii()+1);
}

bool Canvas2D::checkUnderMouse(QList<MyItem*>* v, const QPointF & p){
    QRectF r(p.x(),p.y(),5,5);
    r.adjust(-2.5,-2.5,-2.5,-2.5);

    for(int i=0;i<v->size();i++){
        if (v->at(i)->isInter()){
            for (int j=0;j<v->at(i)->getChildren().size();j++){
                MyItem* tmp=v->at(i)->getChildAt(j);
                if ((!tmp->isUndef())&&(tmp->isUnderMouse(r))){
                    if (focusOwner!=tmp){
                        focusOwner=tmp;
                        repaint();
                    }
                    return true;
                }
            }
        }
        else if ((!v->at(i)->isUndef())&&v->at(i)->isUnderMouse(r)&& checkForValidAction(v->at(i))) {
            if (focusOwner!=v->at(i)){
                focusOwner=v->at(i);
                repaint();
            }

            return true;
        }

    }
    return false;
}


void Canvas2D::updateAllChildrenFrom(MyItem* item){
    QList<MyItem*> v;
    refreshFromItem(item,v);
    qSort(v.begin(),v.end(),lessThan);


    for (int i=0;i<v.size();++i){
        int level=v.at(i)->getLevel();
        Command c=commands.at(level);
        gen g(c.command.toStdString(),context);

        gen answer=protecteval(g,1,context);
        if (v.at(i)->isAngleItem()){
            AngleItem* angle=dynamic_cast<AngleItem*>(v.at(i));
            if (answer.type!=giac::_VECT) return;
            //AngleItem* angle=new AngleItem(this);
            vecteur* vect=answer._VECTptr;
            vecteur::iterator it=vect->begin();
            angle->setValue(giac::simplify(*it,context));
            QList<MyItem*> vv;
            ++it;
            addToVector(*it,vv);
            angle->getCircle()->updateValueFrom(vv.at(0));
            vv.clear();
            ++it;
            addToVector(*it,vv);
            angle->getCurve()->updateValueFrom(vv.at(0));
            continue;
        }
        QList<MyItem*> vv;
        addToVector(answer,vv);
        // Not the particular case of an intersection point or tangent lines
        if (!v.at(i)->isInter()){
            v.at(i)->updateValueFrom(vv.at(0));
            delete vv.at(0);
        }
        else{
            QVector<MyItem*> children=v.at(i)->getChildren();
            for (int j=0;j<vv.size();++j){
                // an intersection point already exists
                if (children.size()>j){
                    children.at(j)->updateValueFrom(vv.at(j));
                    // Update value stored in giac
                    if (v.at(i)->getType()=="Intersection")
                        giac::sto(giac::_point(vv.at(j)->getValue(),context),giac::gen(children.at(j)->getVar().toStdString(),context),context);
                    else giac::sto(giac::_droite(vv.at(j)->getValue(),context),giac::gen(children.at(j)->getVar().toStdString(),context),context);
                    delete vv.at(j);
                }
                // There are not enough points
                else{
                    findFreeVar(varPt);
                    vv.at(j)->setLegend(varPt);
                    vv.at(j)->setVar(varPt);
                    if (v.at(i)->getType()=="Intersection")
                        giac::sto(giac::_point(vv.at(j)->getValue(),context),giac::gen(QString(varPt).toStdString(),context),context);
                    else giac::sto(giac::_droite(vv.at(j)->getValue(),context),giac::gen(children.at(j)->getVar().toStdString(),context),context);
                    pointItems.append(vv.at(j));
                    v.at(j)->updateScreenCoords(true);
                    parent->addToTree(vv.at(j));
                    v.at(i)->addChild(vv.at(j));
                }
             }
            // All older points become undef
            for (int j=vv.size();j<children.size();++j){
                giac::sto(giac::undef,giac::gen(QString(v.at(i)->getChildAt(j)->getVar()).toStdString(),context),context);
                v.at(i)->getChildAt(j)->setUndef(true);
            }

        }
    }
}
void Canvas2D::moveItem(MyItem* item,const QPointF &p){
    QString s(item->getVar());
    if (item->isPointElement()){
        PointElement* pe=dynamic_cast<PointElement*>(item);

        s=(commands.at(item->getLevel())).command;
        s.append("+");
        s.append(pe->getTranslation(p));
    }
    else if (item->isPoint()){
        s.append(commandFreePoint(p,0));
    }
    gen g(s.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);

    item->updateValueFrom(v.at(0));
    delete v.at(0);
    if (item->hasChildren()){
        updateAllChildrenFrom(item);
    }
    updatePixmap(false);
    parent->updateValueInDisplayPanel();
    repaint();
}

/**
 * @brief Canvas2D::refreshFromItem
 * @param item Root Item
 * @param list to store all chlidren
 *
 * Record in the list all the children from a root element.
 */
void Canvas2D::refreshFromItem(MyItem * item, QList<MyItem*>& list, bool evenInter){
    QVector<MyItem*> v=item->getChildren();
    for (int i=0;i<v.size();++i){
//        qDebug()<<v.at(i)->getVar()<<v.at(i)->hasChildren();
        if (!list.contains(v.at(i))){
            if (evenInter) list.append(v.at(i));
            else if (!item->isInter()) list.append(v.at(i));
            if (v.at(i)->hasChildren()) refreshFromItem(v.at(i),list);
        }
    }
}

    //gen g(commands.at(level).toStdString(),context);
    //gen answer=protecteval(g,1,context);





/**
 * @brief Canvas2D::checkForValidAction
 * @param item
 * @return true if this item can be selected for the the current action tool
 * (Eg: If selected action tool is "Segment", it is waiting only for points. Then, we don't highlight circle for example.
 */
bool Canvas2D::checkForValidAction(MyItem * item){
    if (!parent->isInteractive()) return true;
    switch(currentActionTool){
    case  SINGLEPT:
        return true;
    case INTER:
        return (!item->isPoint());
        break;
    case PERPEN_BISECTOR:{
        if (item->isPoint()||item->isSegment()) return true;
        else return false;
    }
        break;
    case MIDPOINT:{
        if (item->isPoint()||item->isSegment()||item->isCircle()) return true;
        else return false;
    }
    break;
    case PERPENDICULAR:
    case PARALLEL:{
        if (item->isPoint()){
            if (selectedItems.isEmpty()) return true;
            else if (selectedItems.at(0)->isPoint()) return false;
            return true;
        }
        else if (item->isLine()||item->isHalfLine()|| item->isSegment()||item->isVector()){
            if (selectedItems.isEmpty()) return true;
            else if (selectedItems.at(0)->isPoint()) return true;
            return false;
        }
        return false;
    }
    break;

    case SEGMENT:
    case LINE:
    case HALFLINE:
    case CIRCLE2PT:
    case CIRCLE3PT:
    case ARC3PT:
    case CIRCLE_RADIUS:
    case VECTOR:
    case BISECTOR:
    case POLYGON:
    case PLOT_BEZIER:
    case REGULAR_POLYGON:
    case ANGLE:
        return item->isPoint();
    case TRANSLATION:{
        if (selectedItems.isEmpty()) return item->isVector();
        else return true;
    }
    case REFLECTION:{
        if (selectedItems.isEmpty()) return (item->isLine()||item->isHalfLine()||item->isSegment());
        else return true;
    }
    case HOMOTHETY:
    case SIMILARITY:
    case ROTATION:
    case POINT_SYMMETRY:{
        if (selectedItems.isEmpty()) return (item->isPoint());
        else return true;
    }
    case TANGENT:{
        if (selectedItems.isEmpty()) {
            if (item->isCircle()) return true;
            else if (item->isCurve()){
                if (item->isSegment()||item->isVector()) return false;
                Curve* c=dynamic_cast<Curve*>(item);
                if (c->isPolygon()) return false;
                return true;
            }
            else return false;
        }
        else {
            return item->isPoint();
        }
    }
        break;



    default:
        return true;
    }
}


/**
 * @brief Canvas2D::checkForCompleteAction
 * @return true if current tool has enough selected items to proceed
 */
bool Canvas2D::checkForCompleteAction(){
    switch(currentActionTool){
        case SEGMENT:
        case LINE:
        case HALFLINE:
        case CIRCLE2PT:
        case INTER:
        case PARALLEL:
        case PERPENDICULAR:
        case VECTOR:
        case REFLECTION:
        case POINT_SYMMETRY:
        case TRANSLATION:
        case HOMOTHETY:
        case SIMILARITY:
        case ROTATION:
        case REGULAR_POLYGON:
        case TANGENT:
        return (selectedItems.size()==2);
        case CIRCLE3PT:
        case ARC3PT:
        case BISECTOR:
        case ANGLE:
        return (selectedItems.size()==3);
        case CIRCLE_RADIUS:
        case SELECT:
        return (selectedItems.size()==1);
        case PERPEN_BISECTOR:
        case MIDPOINT:{
            if (selectedItems.size()==1 && (selectedItems.at(0)->isSegment()||selectedItems.at(0)->isCircle())) return true;
            else if (selectedItems.size()==2 && checkForOnlyPoints(&selectedItems)) return true;
            else return false;
        }
        break;
/*    case PLOT_BEZIER:
    case POLYGON:{
        int size=selectedItems.size();
        if (size>2){
            if (selectedItems.last()==selectedItems.at(size-2)) {
                selectedItems.removeLast();
                return true;
            }
            else return false;
        }
        else return false;
    }
        break;*/
    default:
        return false;
    }
}
/**
 * @brief Canvas2D::checkForPointWaiting
 * @param p
 * @return true if the current tool is waiting for a point selection
 *returns false otherwise
 */
bool Canvas2D::checkForPointWaiting(){
    if (currentActionTool==PARALLEL||currentActionTool==PERPENDICULAR){
        if (selectedItems.isEmpty()) return true;
        else if (selectedItems.at(0)->isLine()||selectedItems.at(0)->isHalfLine()||selectedItems.at(0)->isSegment()
                 ||selectedItems.at(0)->isVector()) return true;
        else return false;
    }
    if (currentActionTool==TANGENT&& selectedItems.size()==1) return true;
    return (currentActionTool==SINGLEPT||currentActionTool==SEGMENT||currentActionTool==HALFLINE||currentActionTool==LINE||currentActionTool==POINT_XY
            ||currentActionTool==CIRCLE2PT||currentActionTool==CIRCLE3PT||currentActionTool==CIRCLE_RADIUS||currentActionTool==PERPEN_BISECTOR
            || (currentActionTool==POLYGON)|| (currentActionTool==PLOT_BEZIER)||(currentActionTool==BISECTOR)||(currentActionTool==VECTOR)||(currentActionTool==REGULAR_POLYGON)
            ||(currentActionTool==ARC3PT)||(currentActionTool==ANGLE));
}

bool Canvas2D::checkForOneMissingPoint(){
    switch(currentActionTool){
        case SEGMENT:
        case LINE:
        case HALFLINE:
        case CIRCLE2PT:
        case PERPEN_BISECTOR:
        case VECTOR:
        return (selectedItems.size()==1);
        case CIRCLE3PT:
        case BISECTOR:
        case ARC3PT:
        return (selectedItems.size()==2);
        case PARALLEL:
        case PERPENDICULAR:
    {   if (selectedItems.isEmpty()) return false;
        else if (selectedItems.at(0)->isLine()||selectedItems.at(0)->isHalfLine()||selectedItems.at(0)->isSegment()
                 ||selectedItems.at(0)->isVector()) return true;
        else  return false;
    }
    case PLOT_BEZIER:
    case POLYGON: {
        if (selectedItems.size()>=1) return true;
        else return false;
    }
    default:
        return false;
    }
}
/**
 * @brief Canvas2D::addNewPoint
 * @param p the current mouse position
 * @return  adds this point to the canvas
 *
 */
void Canvas2D::addNewPoint(const QPointF p){
   findFreeVar(varPt);
   Command newCommand;
   QString s(varPt);
   s.append(commandFreePoint(p,0));
   newCommand.command=s;
   newCommand.attributes=0;
   newCommand.isCustom=false;
   evaluationLevel=commands.size();
   gen g(s.toStdString(),context);
               //    qDebug()<<QString::fromStdString(g.print(context));
   QList<MyItem*> v;
   addToVector(protecteval(g,1,context),v);

   v.at(0)->updateScreenCoords(true);
   v.at(0)->setVar(varPt);
   v.at(0)->setMovable(true);
   pointItems.append(v.at(0));
   newCommand.item=v.at(0);
   commands.append(newCommand);


   parent->addToTree(v.at(0));
   focusOwner=v.at(0);
   parent->updateAllCategories();
   parent->selectInTree(focusOwner);
   selectedItems.append(focusOwner);
   updatePixmap(false);
   // In case of open polygon, we have to redraw the preview before repaint (else itemPreview=0)
   if (currentActionTool==POLYGON) {
       QString s=commandFreePoint(p,0);
       int id=s.indexOf(":=");
       s=s.mid(id+2,s.length()-id-3);
       missingPoint=s;
      addNewPolygon(true);
      repaint();
   }
   else repaint();

}


void Canvas2D::addNewCircle(const bool & onlyForPreview){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    c.command=QString(varLine);
    // CIRCLE RADIUS

    if (selectedItems.size()==1 && !onlyForPreview){
        OneArgDialog *dialog=new OneArgDialog(this,tr("Rayon:"));
        if (dialog->exec()){
            QString first(selectedItems.at(0)->getVar());
            commandTwoArgs("circle",first,dialog->editRadius->text(),c.command);
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
    }
    // CENTER-POINT
    else if (((selectedItems.size()==2) && !onlyForPreview) || (onlyForPreview&& (selectedItems.size()==1))){
        QString first(selectedItems.at(0)->getVar());
        QString second;
        if (onlyForPreview) second=missingPoint;
        else  second=selectedItems.at(1)->getVar();
        second.append("-");
        second.append(first);
        commandTwoArgs("circle",first,second,c.command);
    }
    // CIRCLE 3 POINTS
    else{
        QString first(selectedItems.at(0)->getVar());
        QString second(selectedItems.at(1)->getVar());
        QString third;
        if (onlyForPreview) third=missingPoint;
        else third=selectedItems.at(2)->getVar();
        c.command.append((":=circumcircle("));
        c.command.append(first);
        c.command.append(",");
        c.command.append(second);
        c.command.append(",");
        c.command.append(third);
        c.command.append(");");
    }
    evaluationLevel=commands.size();
    // If preview, removes the beginning of the string
    // Eg: "A:=circle(1+i,3);" becomes "circle(1+i,3);"
    if (onlyForPreview){
        int id=c.command.indexOf(":=");
        c.command=c.command.mid(id+2,c.command.length()-id-2);
    }
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);

    if (onlyForPreview) {
        if (v.at(0)->isUndef()) itemPreview=0;
        else {
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    c.item=v.at(0);
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        filledItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    v.at(0)->updateScreenCoords(true);
    v.at(0)->setVar(varLine);
    filledItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();
}
void Canvas2D::addNewLine(const QString & type, const bool & onlyForPreview){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    c.command=QString(varLine);
    if (!onlyForPreview)
        commandTwoArgs(type,selectedItems.at(0)->getVar(),
                       selectedItems.at(1)->getVar(),c.command);
    // Preview mode
    else {
        // For this command point argument is before line argument
        // We have to swap.
        if ((currentActionTool==PARALLEL)||currentActionTool==PERPENDICULAR){
            commandTwoArgs(type,missingPoint,selectedItems.at(0)->getVar(),c.command);
        }
        else{
            commandTwoArgs(type,selectedItems.at(0)->getVar(),missingPoint,c.command);
        }
        int id=c.command.indexOf(":=");
        c.command=c.command.mid(id+2,c.command.length()-2);
    }
    evaluationLevel=commands.size();
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;

    addToVector(protecteval(g,1,context),v);


    if (onlyForPreview){
        if (v.at(0)->isUndef()){
            itemPreview=0;
        }
        else{
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    c.item=v.at(0);
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        lineItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    selectedItems.at(0)->addChild(v.at(0));
    selectedItems.at(1)->addChild(v.at(0));
    v.at(0)->setVar(varLine);
    v.at(0)->updateScreenCoords(true);
    lineItems.append(v.at(0));
    parent->addToTree(v.at(0));
    parent->updateAllCategories();
    parent->selectInTree(v.at(0));
    updatePixmap(false);
    repaint();
}
void Canvas2D::addNewPolygon(const bool & onlyForPreview, const bool &iso){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    QString s(varLine);
    if (iso) s.append(":=isopolygon(");
    else if (selectedItems.at(0)==selectedItems.at(selectedItems.size()-1)) s.append(":=polygon(");
    else  s.append(":=open_polygon(");
    for (int i=0;i<selectedItems.size();++i){
        s.append(selectedItems.at(i)->getVar());
        if (i!=selectedItems.size()-1) s.append(",");
        else if (onlyForPreview) {
            s.append(",");
            s.append(missingPoint);
            int id=s.indexOf(":=");
            s=s.mid(id+2,s.length()-2);
        }
    }
    if (iso){
        OneArgDialog* dialog=new OneArgDialog(this, tr("Nombre de côtés:"));
        if (dialog->exec()){
            s.append(",");
            s.append(dialog->editRadius->text());
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }

    }

    s.append(");");

    c.command=s;
    evaluationLevel=commands.size();
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    if (onlyForPreview){
        if (v.at(0)->isUndef()){
            itemPreview=0;
        }
        else{
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    c.item=v.at(0);
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        lineItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }

    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    v.at(0)->setVar(varLine);
    v.at(0)->updateScreenCoords(true);
    filledItems.append(v.at(0));
    parent->addToTree(v.at(0));
    parent->updateAllCategories();
    parent->selectInTree(v.at(0));
    updatePixmap(false);
    repaint();


}
void Canvas2D::addNewBezier(const bool & onlyForPreview){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    QString s(varLine);
    s.append(":=bezier(");
    for (int i=0;i<selectedItems.size();++i){
        s.append(selectedItems.at(i)->getVar());
        if (i!=selectedItems.size()-1) s.append(",");
        else if (onlyForPreview) {
            s.append(",");
            s.append(missingPoint);
            int id=s.indexOf(":=");
            s=s.mid(id+2,s.length()-2);
        }
    }
    s.append(");");

    c.command=s;
    evaluationLevel=commands.size();
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    if (onlyForPreview){
        if (v.at(0)->isUndef()){
            itemPreview=0;
        }
        else{
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    c.item=v.at(0);
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        lineItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    v.at(0)->setVar(varLine);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    v.at(0)->updateScreenCoords(true);
    lineItems.append(v.at(0));
    parent->addToTree(v.at(0));
    parent->updateAllCategories();
    parent->selectInTree(v.at(0));
    updatePixmap(false);
    repaint();



}
void Canvas2D::addNewArc(const bool & onlyForPreview){

    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    c.command=QString(varLine);


    QString first(selectedItems.at(0)->getVar());
    QString second(selectedItems.at(1)->getVar());
    QString third;
    if (onlyForPreview) third=missingPoint;
    else third=selectedItems.at(2)->getVar();

    QString test("is_collinear(");
    test.append(first);
    test.append(",");
    test.append(second);
    test.append(",");
    test.append(third);
    test.append("))");

    gen g(test.toStdString(),context);
    gen answer=protecteval(g,1,context);

   // 3 points on a same line
    if (answer==gen(1)) {
        if (onlyForPreview) {
            itemPreview=0;
            return;
        }
        else {
            UndefItem* undef=new UndefItem(this);
            undef->setVar(varLine);
            filledItems.append(undef);
            parent->addToTree(undef);
            parent->updateAllCategories();
            parent->selectInTree(undef);
            return;
        }

    }
    c.command.append(":=arc(");
    c.command.append(first);
    c.command.append(",");
    c.command.append(second);
    c.command.append(",");
    c.command.append(third);
    c.command.append(");");
    evaluationLevel=commands.size();

    if (onlyForPreview){
        int id=c.command.indexOf(":=");
        c.command=c.command.mid(id+2,c.command.length()-id-2);
    }
    g=gen(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    if (onlyForPreview) {
        if (v.at(0)->isUndef()) itemPreview=0;
        else {
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        filledItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    v.at(0)->updateScreenCoords(true);
    v.at(0)->setVar(varLine);
    filledItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();

}


void Canvas2D::addMidpoint(){
    findFreeVar(varPt);
    Command newCommand;
    QString s(varPt);
    newCommand.attributes=0;
    QString type("midpoint");
    if (selectedItems.size()==1 && selectedItems.first()->isCircle()) type="center";
    if (selectedItems.size()==2) commandTwoArgs(type,selectedItems.at(0)->getVar(),
                    selectedItems.at(1)->getVar(),s);
    else  {
        s.append(":=");
        s.append(type);
        s.append("(");
        s.append(selectedItems.at(0)->getVar());
        s.append(");");
    }
    newCommand.isCustom=false;
    newCommand.command=s;

    evaluationLevel=commands.size();
    gen g(newCommand.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    newCommand.item=v.at(0);
    commands.append(newCommand);

    selectedItems.at(0)->addChild(v.at(0));
    if (selectedItems.size()==2) selectedItems.at(1)->addChild(v.at(0));
    v.at(0)->setVar(varPt);
    v.at(0)->updateScreenCoords(true);
    pointItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();
}
void Canvas2D::addBisector(const bool & onlyForPreview){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    c.command=QString(varLine);
    QString first(selectedItems.at(0)->getVar());
    QString second(selectedItems.at(1)->getVar());
    c.command.append(":=bisector(");
    c.command.append(first);
    c.command.append(",");
    c.command.append(second);
    c.command.append(",");
    if (selectedItems.size()==2)
        c.command.append(missingPoint);
    else c.command.append(selectedItems.at(2)->getVar());
    c.command.append(");");
    evaluationLevel=commands.size();
    // If preview, removes the beginning of the string
    // Eg: "A:=circle(1+i,3);" becomes "circle(1+i,3);"
    if (onlyForPreview){
        int id=c.command.indexOf(":=");
        c.command=c.command.mid(id+2,c.command.length()-id-2);
    }
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);

    if (onlyForPreview) {
        if (v.at(0)->isUndef()) itemPreview=0;
        else {
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    c.isCustom=false;
    c.item=v.at(0);
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        filledItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    v.at(0)->updateScreenCoords(true);
    v.at(0)->setVar(varLine);
    lineItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();
}
void Canvas2D::addTransformObject(const QString & type){
    QString s;
    if (selectedItems.at(1)->isPoint()) {
        findFreeVar(varPt);
        s=varPt;
    }
    else {
        findFreeVar(varLine);
        s=varLine;
    }
    QString first(selectedItems.at(0)->getVar());
    QString second(selectedItems.at(1)->getVar());

    if (type=="rotation"){
        s.append(":=rotation(");
        s.append(first);
        s.append(",");
        OneArgDialog* dialog=new OneArgDialog(this, tr("Angle:"));
        if (dialog->exec()){
            s.append(dialog->editRadius->text());
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
        s.append(",");
        s.append(second);
        s.append(");");
    }
    else if (type=="homothety"){
        s.append(":=homothety(");
        s.append(first);
        s.append(",");
        OneArgDialog* dialog=new OneArgDialog(this, tr("Rapport:"));
        if (dialog->exec()){
            s.append(dialog->editRadius->text());
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
        s.append(",");
        s.append(second);
        s.append(");");
    }
    else if (type=="similarity"){
        s.append(":=similarity(");
        s.append(first);
        s.append(",");
        OneArgDialog* dialog=new OneArgDialog(this, tr("Rapport:"));
        if (dialog->exec()){
            s.append(dialog->editRadius->text());
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
        s.append(",");

        dialog=new OneArgDialog(this, tr("Angle:"));
        if (dialog->exec()){
            s.append(dialog->editRadius->text());
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
        s.append(",");
        s.append(second);
        s.append(");");

    }
    else{
        commandTwoArgs(type,first,second,s);
    }
    Command c;
    c.attributes=0;
    c.command=s;
    evaluationLevel=commands.size();

    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);

    c.item=v.at(0);
    c.isCustom=false;
    commands.append(c);

    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        filledItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }
    v.at(0)->updateScreenCoords(true);
    if (selectedItems.at(1)->isPoint()) {
        v.at(0)->setVar(varPt);
        pointItems.append(v.at(0));
    }
    else {
        v.at(0)->setVar(varLine);
        lineItems.append(v.at(0));
    }

    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();

}




void Canvas2D::addNewPointElement(const QPointF &pos){
//    if (focusOwner->isLine()|| focusOwner->isHalfLine()){
        findFreeVar(varPt);
        Command newCommand;
        QString s(varPt);
        s.append(":=element(");
        s.append(focusOwner->getVar());
        s.append(")");
        gen g(s.toStdString(),context);
        QList<MyItem*> v;
        addToVector(protecteval(g,1,context),v);

        PointElement* p=0;
        Point* origin=dynamic_cast<Point*>(v.at(0));
        if (origin !=0){
            p=new PointElement(origin,this);
//            qDebug()<<"origine"<<origin->x()<<origin->y();
        }
        delete origin;
        if (p==0) return;
        newCommand.command=s;
        newCommand.attributes=0;
        newCommand.isCustom=false;
        s.append("+");
        s.append(p->getTranslation(pos));

        evaluationLevel=commands.size();
        g=giac::gen(s.toStdString(),context);
        v.clear();
        addToVector(protecteval(g,1,context),v);

        newCommand.item=v.at(0);
        commands.append(newCommand);
        p->setAttributes(0);
        p->updateValueFrom(v.at(0));
        p->setLevel(evaluationLevel);
        p->setLegend(v.at(0)->getLegend());
        p->updateScreenCoords(true);
        p->setVar(varPt);
        p->setMovable(true);
        selectedItems.at(0)->addChild(p);

        pointItems.append(p);
        parent->addToTree(p);
        focusOwner=p;
        parent->updateAllCategories();
        parent->selectInTree(focusOwner);
        selectedItems.append(focusOwner);
        updatePixmap(false);
         repaint();


 //   }




}
void Canvas2D::addPerpenBisector(const bool &onlyForPreview){


    findFreeVar(varPt);
    Command newCommand;
    QString s(varPt);
    newCommand.attributes=0;
    if (selectedItems.size()==2) commandTwoArgs(QString("perpen_bisector"),selectedItems.at(0)->getVar(),
                    selectedItems.at(1)->getVar(),s);
    else  {
        s.append(":=");
        s.append(QString("perpen_bisector"));
        s.append("(");
        s.append(selectedItems.at(0)->getVar());
        if (onlyForPreview){
            s.append(",");
            s.append(missingPoint);
            int id=s.indexOf(":=");
            s=s.mid(id+2,s.length()-2);

        }
        s.append(");");
    }
    newCommand.isCustom=false;
    newCommand.command=s;
    evaluationLevel=commands.size();
    gen g(newCommand.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    if (onlyForPreview){
        if (v.at(0)->isUndef()){
            itemPreview=0;
        }
        else{
            itemPreview=v.at(0);
            itemPreview->updateScreenCoords(true);
        }
        return;
    }
    newCommand.item=v.at(0);
    commands.append(newCommand);
    if (v.at(0)->isUndef()){
        UndefItem* undef=new UndefItem(this);
        undef->setVar(varLine);
        lineItems.append(undef);
        parent->addToTree(undef);
        parent->updateAllCategories();
        parent->selectInTree(undef);
        return;
    }


    selectedItems.at(0)->addChild(v.at(0));
    if (selectedItems.size()==2) selectedItems.at(1)->addChild(v.at(0));
    v.at(0)->setVar(varPt);
    v.at(0)->updateScreenCoords(true);
    pointItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();






}

void Canvas2D::addNewAngle(){
    findFreeVar(varLine);
    Command c;
    c.attributes=0;
    c.command=QString(varLine);

    QString first(selectedItems.at(0)->getVar());
    QString second(selectedItems.at(1)->getVar());
    QString third=selectedItems.at(2)->getVar();

    c.command.append(":=angle(");
    c.command.append(first);
    c.command.append(",");
    c.command.append(second);
    c.command.append(",");
    c.command.append(third);
    c.command.append(",\"\");");
    evaluationLevel=commands.size();
    gen g(c.command.toStdString(),context);

    gen answer=protecteval(g,1,context);

    if (answer.type!=giac::_VECT) return;
    AngleItem* angle=new AngleItem(this);

    vecteur* vect=answer._VECTptr;

    vecteur::iterator it=vect->begin();
    vecteur::iterator itend=vect->end();





    angle->setValue(*it);
//    qDebug()<<QString::fromStdString(print(*it,context));
    QList<MyItem*> v;
    ++it;
    addToVector(*it,v);
    angle->setCircle(v.at(0));
    v.clear();
    ++it;
    addToVector(*it,v);
    angle->setCurve(v.at(0));

    angle->setLevel(evaluationLevel);
    c.isCustom=false;
    commands.append(c);

    angle->updateScreenCoords(true);
    angle->setVar(varLine);
    filledItems.append(angle);
    parent->addToTree(angle);
    focusOwner=angle;
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(angle);
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(false);
    repaint();



}


void Canvas2D::addInter(const QString & type){
    Command newCommand;
    newCommand.attributes=0;
    QString s;
    commandTwoArgs(type,selectedItems.at(0)->getVar(),
                selectedItems.at(1)->getVar(),s);
    s=s.right(s.length()-2);
    newCommand.isCustom=false;
    newCommand.command=s;
    evaluationLevel=commands.size();
    gen g(newCommand.command.toStdString(),context);
    InterItem* inter=0;
    if (type=="inter")
        inter=new InterItem(false,this);
    else inter=new InterItem(true,this);
    inter->setLevel(evaluationLevel);
    newCommand.item=inter;
    commands.append(newCommand);
    // inter command
    if (type=="inter")
        pointItems.append(inter);
    // tangent command
    else lineItems.append(inter);

    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);

    for (int i=0;i<v.size();++i){
        if (type=="inter"){
            findFreeVar(varPt);
            v.at(i)->setLegend(varPt);
            v.at(i)->setVar(varPt);
            giac::sto(giac::_point(v.at(i)->getValue(),context),giac::gen(QString(varPt).toStdString(),context),context);
        }
        else{
            findFreeVar(varLine);
            v.at(i)->setLegend(varLine);
            v.at(i)->setVar(varLine);


            giac::sto(giac::_droite(v.at(i)->getValue(),context),giac::gen(QString(varLine).toStdString(),context),context);
        }
        v.at(i)->setFromInter(true);
        if (type=="inter")
            pointItems.append(v.at(i));
        else lineItems.append(v.at(i));

        v.at(i)->updateScreenCoords(true);
        parent->addToTree(v.at(i));
        selectedItems.append(v.at(i));
        inter->addChild(v.at(i));
     }
    parent->updateAllCategories();

    selectedItems.at(0)->addChild(inter);
    selectedItems.at(1)->addChild(inter);
    updatePixmap(false);
    repaint();
}
void Canvas2D::executeMyAction(bool onlyForPreview=false){
        if (itemPreview!=0) delete itemPreview;
         itemPreview=0;

    switch(currentActionTool){
        case SELECT: parent->selectInTree(focusOwner);
        break;
        case INTER: addInter("inter");
        break;
        case PERPEN_BISECTOR: addPerpenBisector(onlyForPreview);
        break;
        case MIDPOINT: addMidpoint();
        break;
        case SEGMENT:  addNewLine("segment",onlyForPreview);
        break;
        case LINE: addNewLine("line",onlyForPreview);
        break;
        case HALFLINE: addNewLine("half_line",onlyForPreview);
        break;
        case VECTOR: addNewLine("vector",onlyForPreview);
        break;
        case PERPENDICULAR:
        case PARALLEL:{
        if ((!selectedItems.at(0)->isPoint())&&(!onlyForPreview)){
            selectedItems.swap(0,1);
        }
        if (currentActionTool==PARALLEL){
            addNewLine("parallel",onlyForPreview);
        }
        else if (currentActionTool==PERPENDICULAR){
            addNewLine("perpendicular",onlyForPreview);
        }

        }
        break;
        case CIRCLE2PT:
        case CIRCLE3PT:
        case CIRCLE_RADIUS: addNewCircle(onlyForPreview);
        break;
        case BISECTOR: addBisector(onlyForPreview);
        break;
        case POLYGON: addNewPolygon(onlyForPreview);
        break;
        case TRANSLATION: addTransformObject("translation");
        break;
        case ROTATION:  addTransformObject("rotation");
        break;
        case POINT_SYMMETRY:
        case REFLECTION: addTransformObject("reflection");
        break;
        case HOMOTHETY: addTransformObject("homothety");
        break;
        case SIMILARITY: addTransformObject("similarity");
        break;
        case REGULAR_POLYGON: addNewPolygon(false,true);
        break;
        case ARC3PT: addNewArc(onlyForPreview);
        break;
    case PLOT_BEZIER:   addNewBezier(onlyForPreview);
        break;
    case TANGENT: addInter("tangent");
        break;
    case ANGLE: addNewAngle();
    default:{}

    }

    if (!onlyForPreview)    selectedItems.clear();

}
void Canvas2D::commandTwoArgs(const QString &command,const QString &first,const QString &second,QString  & result){
    result.append(":=");
    result.append(command);
    result.append("(");
    result.append(first);
    result.append(",");
    result.append(second);
    result.append(");");
}

QString  Canvas2D::commandFreePoint(const QPointF & p,const int attributes){
     QString newCommand(":=point([");
    if (Config::gridAttraction&&gridParam.isVisible){
        if (gridParam.isCartesian){
            double xcoord,ycoord;
            toXY(p.x(),p.y(),xcoord,ycoord);
            double a,b;
            int quotient=(int)(xcoord/gridParam.x);
            double between=std::abs(xcoord-quotient*gridParam.x);
            if (between<gridParam.x*0.15){
                a=quotient*gridParam.x;
            }
            else if (between>0.85*gridParam.x){
                if (xcoord>0) a=(quotient+1)*gridParam.x;
                else a=(quotient-1)*gridParam.x;

            }
            else a=xcoord;

            quotient=(int)(ycoord/gridParam.y);
            between=std::abs(ycoord-quotient*gridParam.y);
            if (between<gridParam.y*0.15){
                b=quotient*gridParam.y;
            }
            else if (between>0.85*gridParam.y){
                if (ycoord>0) b=(quotient+1)*gridParam.y;
                else b=(quotient-1)*gridParam.y;
            }
            else b=ycoord;
            newCommand.append(QString::number(a));
            newCommand.append(",");
            newCommand.append(QString::number(b));
            newCommand.append("],display=0);");
        }

    }
    else{
        double a,b;
        toXY(p.x(),p.y(),a,b);
        newCommand.append(QString::number(a));
        newCommand.append(",");
        newCommand.append(QString::number(b));
        newCommand.append("],display=");
        newCommand.append(QString::number(attributes));
        newCommand.append(");");
    }
       return newCommand;
}
void Canvas2D::mouseReleaseEvent(QMouseEvent *e){
    Qt::MouseButton b=e->button();
    if (b==Qt::RightButton&& selectionRight) {
                endSel=e->pos();
                setMouseTracking(true);
                QRect r(startSel,endSel);

                if (selectionRight&& std::abs(r.width())>10&& std::abs(r.height())>10){
                    zoom_In();
                }
                else  {
                    // defining the last point for a polygon or a bezier curve:
                    if ((currentActionTool==POLYGON)||(currentActionTool==PLOT_BEZIER)){
                        if (focusOwner!=0) selectedItems<<focusOwner;
                        else{
                            addNewPoint(e->pos());
                        }
                        executeMyAction();
                    }
                    // Display context menu for Objects
                    else if (focusOwner!=0) {

                        QString s=focusOwner->getType();
                        s.append(" ");
                        s.append(focusOwner->getVar());
                        objectTitleAction->setText(s);
                        displayObjectAction->setChecked(focusOwner->isVisible());
                        displayLegendAction->setChecked(focusOwner->legendVisible());
                        if (focusOwner->isPoint()){
                            traceAction->setChecked(focusOwner->isTraceActive());
                            traceAction->setVisible(true);
                        }
                        else traceAction->setVisible(false);

                        menuObject->popup(this->mapToGlobal(e->pos()));
                    }
                    // display classic context menu
                    else menuGeneral->popup(this->mapToGlobal(e->pos()));
                }
                selectionRight=false;
    }
    // Left Button
    else if (b==Qt::LeftButton){
        selectionLeft=false;
        // An object is already highlighted
        if (focusOwner!=0){
            // Here, a very fast clic (for example double clic) can cause problems
            // Create a segment. First point then double clic for the second point.
            // the segment is focused after creation, but unchecked for selection... and goes to selectedItems
            if (parent->isInteractive()){
                // To improve (In most cases, the foolowing line is a  double check)
                if (checkForValidAction(focusOwner)) {
                    selectedItems<<focusOwner;
                    if (currentActionTool==PLOT_BEZIER){
                        addNewBezierControlPoint();
                    }
                    else if (currentActionTool==SINGLEPT) addNewPointElement(e->posF());
                }


                if (checkForCompleteAction()){
                      executeMyAction();
                }
            }
            else parent->selectInTree(focusOwner);
        }
        else {
            if (parent->isInteractive()){
                // No focusOwner
                // First, Does the selected action tool allow us to add a new point?
                // If not:
                if (!checkForPointWaiting()) parent->clearSelection();
                // Else, after this point is added on drawing screen, is selectedItem full?
                else {
                    addNewPoint(e->pos());
                    if (checkForCompleteAction()) executeMyAction();
                    if (currentActionTool==PLOT_BEZIER) addNewBezierControlPoint();
                }
            }
            else  parent->clearSelection();
        }
    }
}
void Canvas2D::addNewBezierControlPoint(){
    int size=selectedItems.size();
    if ((size>1) && ((size-1)%3==0)) {
        QString center=selectedItems.last()->getVar();
        QString pt=selectedItems.at(size-2)->getVar();

         findFreeVar(varPt);
         QString s(varPt);
         commandTwoArgs("reflection",center,pt,s);
        Command c;
        c.attributes=0;
        c.command=s;
        evaluationLevel=commands.size();

        gen g(c.command.toStdString(),context);
        QList<MyItem*> v;
        addToVector(protecteval(g,1,context),v);

        c.item=v.at(0);
        c.isCustom=false;
        commands.append(c);

        if (v.at(0)->isUndef()){
            UndefItem* undef=new UndefItem(this);
            undef->setVar(varLine);
            filledItems.append(undef);
            parent->addToTree(undef);
            parent->updateAllCategories();
            parent->selectInTree(undef);
            return;
        }
        v.at(0)->updateScreenCoords(true);
        v.at(0)->setVar(varPt);
        pointItems.append(v.at(0));

        parent->addToTree(v.at(0));
        focusOwner=v.at(0);

        selectedItems.at(size-1)->addChild(v.at(0));
        selectedItems.at(size-2)->addChild(v.at(0));
        selectedItems.append(v.at(0));
        parent->updateAllCategories();
        parent->selectInTree(focusOwner);
        updatePixmap(false);
        repaint();

    }

}
void Canvas2D::mouseMoveEvent(QMouseEvent *e){
    if (selectionRight&&!hasMouseTracking()) {
        endSel=e->pos();
        repaint();
        return;
    }
    selectionRight=false;

    QPointF mousePos(e->posF());
    // try to move and object
    if (parent->isInteractive()&&focusOwner!=0&& selectionLeft&&(currentActionTool==SELECT)){

        if (focusOwner->isMovable()) moveItem(focusOwner,mousePos);
        return;
    }

    // Detects objects under mouse
    bool objectUnderMouse=checkUnderMouse(&pointItems,mousePos) || checkUnderMouse(&lineItems,mousePos) ||   checkUnderMouse(&filledItems,mousePos);
    // Case: No object
    if (!objectUnderMouse){
        if (focusOwner!=0){
            focusOwner=0;
            repaint();
        }
    }
    else repaint();


    // When a point is just missing for complete action, draws the preview
    if (isInteractive()&&checkForOneMissingPoint()){
        QString s=commandFreePoint(mousePos,0);
        int id=s.indexOf(":=");
        s=s.mid(id+2,s.length()-id-3);
        missingPoint=s;
        executeMyAction(true);
        repaint();
    }


//    repaint();

}


void Canvas2D::mousePressEvent(QMouseEvent *e){
    Qt::MouseButton b=e->button();
    if (b==Qt::RightButton){
            setMouseTracking(false);
            selectionRight=true;
            startSel=e->pos();
    }
    else if(b==Qt::LeftButton){
        if (parent->isInteractive()) selectionLeft=true;
    }
}
bool Canvas2D::event(QEvent * ev){
    if (ev->type() == QEvent::ToolTip){
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(ev);
        QPointF p=helpEvent->pos();
        // Display real coords near mouse position
        double a,b;
        toXY(p.x(),p.y(),a,b);
        QString s(QString("(").append(QString::number(a,'g',4)).append(";").append(QString::number(b,'g',4)).append(QString(")")));
        QToolTip::showText(helpEvent->globalPos(),s);
        return true;
    }
    else return QWidget::event(ev);

}
/**
  * Draws the geometry scene
  *
  * First, draw the pixmap
  * Then, highlights some selected objects
  */
void Canvas2D::paintEvent(QPaintEvent * ){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.drawPixmap(0,0,pixmap);
    painter.setClipRect(20,20,width()-40,height()-40);

    // Draw eventual traces
    if (!traceVector.isEmpty()) {
        for (int i=0;i<traceVector.size();++i){
            traceVector.at(i)->drawTrace(&painter);
        }

    }


    // draw selection
    if (selectionRight){
        QColor fill=QColor(50,0,255,50);
        painter.setPen(fill);
        QBrush brush(fill,Qt::SolidPattern);
        painter.fillRect(QRect(startSel,endSel),brush);
    }
    // draws the preview
    if (itemPreview!=0) {
        itemPreview->draw(&painter);
        // remove last point & preview
        delete itemPreview;
        itemPreview=0;
    }

     // Highlight selected objects
    QList<MyItem*> selectedItems=parent->getTreeSelectedItems();
    for (int i=0;i<selectedItems.size();++i){
        MyItem* item =selectedItems.at(i);
        item->setHighLighted(true);
        item->draw(&painter);
        item->setHighLighted(false);
    }
    // If there is another object under mouse
    if (focusOwner!=0){
        focusOwner->setHighLighted(true);
        focusOwner->draw(&painter);
        focusOwner->setHighLighted(false);
    }
    for (int i=0;i<pointItems.size();++i){
        if (pointItems.at(i)->isTraceActive()) pointItems.at(i)->draw(&painter);
    }

}
void Canvas2D::resizeEvent(QResizeEvent * ev){
      if (isInteractive()){
        xmax=xmin+(ev->size().width()-40)/xunit;
        ymin=ymax-(ev->size().height()-40)/yunit;
        setXYUnit();
        updatePixmap(false);
        repaint();
    }

}
/**
 * @brief Canvas2D::toXML
 * @param top The top element in XML hierarchy (a "graph2d" element)
 */
void  Canvas2D::toXML(QDomElement & top){
    QDomElement interactive=top.ownerDocument().createElement("interactive");
    QDomText text;
    if (isInteractive())
        text=top.ownerDocument().createTextNode("true");
    else text=top.ownerDocument().createTextNode("false");
    interactive.appendChild(text);
    top.appendChild(interactive);
    // export axis and grid

    QDomElement xaxis=top.ownerDocument().createElement("axis");
    xaxis.setAttribute("position","x");
    xaxis.setAttribute("color",xAxisParam.color.rgb());
    xaxis.setAttribute("isVisible",xAxisParam.isVisible);
    xaxis.setAttribute("legend",xAxisParam.legend);
    xaxis.setAttribute("unitSuffix",xAxisParam.unitSuffix);
    xaxis.setAttribute("tick",xAxisParam.tick);
    xaxis.setAttribute("min",xmin);
    xaxis.setAttribute("max",xmax);

    QDomElement yaxis=top.ownerDocument().createElement("axis");
    yaxis.setAttribute("position","y");
    yaxis.setAttribute("color",yAxisParam.color.rgb());
    yaxis.setAttribute("isVisible",yAxisParam.isVisible);
    yaxis.setAttribute("legend",yAxisParam.legend);
    yaxis.setAttribute("unitSuffix",yAxisParam.unitSuffix);
    yaxis.setAttribute("tick",yAxisParam.tick);
    yaxis.setAttribute("min",ymin);
    yaxis.setAttribute("max",ymax);

    QDomElement grid=top.ownerDocument().createElement("grid");
    grid.setAttribute("color",gridParam.color.rgb());
    grid.setAttribute("line",gridParam.line);
    grid.setAttribute("isVisible",gridParam.isVisible);
    grid.setAttribute("isCartesian",gridParam.isCartesian);
    if (gridParam.isCartesian){
        grid.setAttribute("x",gridParam.x);
        grid.setAttribute("y",gridParam.y);
    }
    else{
        grid.setAttribute("r",gridParam.x);
        grid.setAttribute("theta",gridParam.y);
    }
    top.appendChild(grid);
    top.appendChild(xaxis);
    top.appendChild(yaxis);

    // Export all "fillable" Items
    for (int i=0;i<filledItems.size();++i){
        filledItems.at(i)->toXML(top);
    }
    // Export all "line" Items
    for (int i=0;i<lineItems.size();++i){
        lineItems.at(i)->toXML(top);
    }
    // Export all "point" Items
    for (int i=0;i<pointItems.size();++i){
        pointItems.at(i)->toXML(top);
    }
}


void Canvas2D::setFocusOwner(MyItem * item){
    item->setHighLighted(true);
    if (focusOwner!=0 && focusOwner!=item ){
        focusOwner->setHighLighted(false);
    }
    focusOwner=item;
    repaint();
}

bool Canvas2D::isInteractive() const{
    return parent->isInteractive();
}
bool Canvas2D::checkForOnlyPoints(const QList<MyItem *> *list) const{

    for (int i=0;i<list->size();++i){
        if (!list->at(i)->isPoint()) return false;
    }
    return true;

}
bool Canvas2D::checkForOnlyFillables(const QList<MyItem *> *list) const{
    for (int i=0;i<list->size();++i){
        if (!list->at(i)->isFillable()) return false;
    }
    return true;
}


bool Canvas2D::checkForOnlyLines(const QList<MyItem *> *list) const{

    for (int i=0;i<list->size();++i){
        if (list->at(i)->isPoint()) return false;
    }
    return true;

}
void Canvas2D::getDisplayCommands(QStringList & list){
    QString tmp;
    for (int i=0;i<commands.size();++i){
        QString s=commands.at(i).command;
        MyItem* item=commands.at(i).item;
        if (item->isUndef()) continue;
        if (item->isInter()){
            tmp.clear();
            for (int j=0;j<item->getChildren().size();++j){
                if (!item->getChildAt(j)->isUndef()) {
                    tmp.append(item->getChildAt(j)->getVar());
                    tmp.append(",");
                }
            }
            tmp.remove(tmp.length()-1,1);
            tmp.append(":=");
            s.prepend(tmp);
        }
        list.append(s);
    }

}
PanelProperties::PanelProperties(Canvas2D* c){
    parent=c;
    initGui();
}
void PanelProperties::initGui(){
    displayPanel=new DisplayProperties(parent);
    axisGridPanel=new AxisGridPanel(parent);

    displayPanel->hide();
    axisGridPanel->hide();

    tree=new QTreeWidget;
    tree->setColumnCount(1);
    tree->headerItem()->setTextAlignment(0,Qt::AlignCenter);
    tree->headerItem()->setText(0,tr("Objects"));
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->clearSelection();

    nodeAxis=new QTreeWidgetItem;
    nodePoint=new QTreeWidgetItem;
    nodeCurve=new QTreeWidgetItem;
    nodeVector=new QTreeWidgetItem;
    nodeLine=new QTreeWidgetItem;
    nodeSegment=new QTreeWidgetItem;
    nodeHalfLine=new QTreeWidgetItem;
    nodeCircle=new QTreeWidgetItem;
    nodePolygon=new QTreeWidgetItem;
    nodeAngle=new QTreeWidgetItem;
    nodeList=new QTreeWidgetItem;
    nodeUndef=new QTreeWidgetItem;

    nodeAxis->setText(0,tr("Axes/grille"));
    nodePoint->setText(0,tr("Point"));
    nodeCurve->setText(0,tr("Courbe"));
    nodeVector->setText(0,tr("Vecteur"));
    nodeLine->setText(0,tr("Droite"));
    nodeSegment->setText(0,tr("Segment"));
    nodeHalfLine->setText(0,tr("Demie-droite"));
    nodePolygon->setText(0,tr("Polygône"));
    nodeCircle->setText(0,tr("Cercle ou arc"));
    nodeAngle->setText(0,tr("Angle"));
    nodeList->setText(0,tr("Liste"));
    nodeUndef->setText(0,tr("Objets indéfinis"));

    tree->addTopLevelItem(nodeAxis);
    tree->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    if (!parent->isInteractive()){
        hbox=new QHBoxLayout(this);
    }
    else{
        hbox=new QVBoxLayout(this);
    }
    hbox->addWidget(tree);
    if (parent->isInteractive()){
        hbox->addStretch(1);
    }
    hbox->addWidget(displayPanel,0,Qt::AlignLeft|Qt::AlignTop);
    hbox->addWidget(axisGridPanel,0,Qt::AlignLeft|Qt::AlignTop);
//    hbox->setSizeConstraint(QLayout::SetMaximumSize);
    setLayout(hbox);

    QSizePolicy p(QSizePolicy::Fixed,QSizePolicy::Maximum);
    p.setHorizontalStretch(1);
    setSizePolicy(p);
    connect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));
}
/*void PanelProperties::resizeEvent(QResizeEvent * e){
    displayPanel->setMaximumWidth(tree->width());
    displayPanel->updateGeometry();
}*/
void PanelProperties::renameInTree(MyItem * item){
    QTreeWidgetItem* treeItem=nodeLinks.key(item);
    treeItem->setText(0,item->getLegend());
    updateTree();
}



void PanelProperties::addToTree( MyItem * item){
    QTreeWidgetItem* treeItem=new QTreeWidgetItem;

    if (item->isPixel()) return;

    if (item->isPoint()){
       nodePoint->addChild(treeItem);

    }
    else if(item->isLine()){
        nodeLine->addChild(treeItem);
    }
    else if(item->isHalfLine()){
        nodeHalfLine->addChild(treeItem);
    }
    else if(item->isCurve()){

        Curve* c= dynamic_cast<Curve*>(item);
        if (c->isVector())
            nodeVector->addChild(treeItem);
        else if (c->isSegment())
            nodeSegment->addChild(treeItem);
        else if (c->isPolygon())
            nodePolygon->addChild(treeItem);
        else
            nodeCurve->addChild(treeItem);
    }
    else if(item->isBezierCurve()){
            nodeCurve->addChild(treeItem);
    }
    else if(item->isCircle()){
        nodeCircle->addChild(treeItem);
    }
    else if (item->isAngleItem()){
        nodeAngle->addChild(treeItem);
    }
    else if (item->isUndef()){
        nodeUndef->addChild(treeItem);

    }
    else if(item->isList()){
        nodeList->addChild(treeItem);
    }
    nodeLinks.insert(treeItem,item);
    QString legend=item->getLegend();
    if (legend.trimmed().isEmpty()){
        treeItem->setText(0,item->getType().append(" ").append(QString::number(treeItem->parent()->childCount())));
    }
    else treeItem->setText(0,legend);
}


void PanelProperties::updateTree(){
   QList<QTreeWidgetItem*> list=tree->selectedItems();
   if (list.isEmpty()) return;

   disconnect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));

   QList<MyItem*>* listItems=new QList<MyItem*>;
   for (int i=0;i<list.size();++i){
        // A category item has been selected
       if (list.at(i)->parent()==0){
           QTreeWidgetItem* treeItem=list.at(i);
           for (int j=0;j<treeItem->childCount();++j){
               treeItem->child(j)->setSelected(true);
               listItems->append(nodeLinks.value(treeItem->child(j)));
           }
       }
       else if(nodeLinks.contains(list.at(i))) {
           MyItem * myItem=nodeLinks.value(list.at(i));

           if (!myItem->isUndef())     parent->setFocusOwner(myItem);
           listItems->append(myItem);

       }
   }
   parent->repaint();
   if (listItems->size()!=0){
       displayPanel->updateDisplayPanel(listItems);
       axisGridPanel->setVisible(false);

   } // Category Grid/axis has been selected
   else {
    displayPanel->setVisible(false);
    axisGridPanel->initValue();
    axisGridPanel->setVisible(true);
   }
  connect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));

}

void PanelProperties::removeFromTree(MyItem * item){
    // Remove from listItem (if it appears)
    int id=displayPanel->getListItems()->indexOf(item);
    if (id!=-1) displayPanel->getListItems()->removeAt(id);




    QTreeWidgetItem* treeWidgetItem=nodeLinks.key(item);
    nodeLinks.remove(treeWidgetItem);
    delete treeWidgetItem;
    updateAllCategories();
    tree->collapseAll();
}
QList<MyItem*> PanelProperties::getTreeSelectedItems(){
    QList<QTreeWidgetItem*> list=tree->selectedItems();
    QList<MyItem*> listItems=QList<MyItem*>();
    for (int i=0;i<list.size();++i){
        if(nodeLinks.contains(list.at(i))) {
            MyItem * myItem=nodeLinks.value(list.at(i));
            listItems.append(myItem);
        }
    }
    return listItems;

}
bool PanelProperties::updateCategory(QTreeWidgetItem* node,const int & id){
    if (tree->indexOfTopLevelItem(node)!=-1){
        if (node->childCount()>0) return true;
        else {
            tree->takeTopLevelItem(tree->indexOfTopLevelItem(node));
            return false;
        }
    }
    else {
        if (node->childCount()>0) {
            tree->insertTopLevelItem(id,node);
            return true;
        }
    }
    return false;
}

void PanelProperties::updateAllCategories(){
    int id=1;
    if (updateCategory(nodePoint,id)) id++;
    if (updateCategory(nodeLine,id)) id++;
    if (updateCategory(nodeSegment,id)) id++;
    if (updateCategory(nodeCircle,id)) id++;
    if (updateCategory(nodeCurve,id)) id++;
    if (updateCategory(nodePolygon,id)) id++;
    if (updateCategory(nodeVector,id)) id++;
    if (updateCategory(nodeAngle,id)) id++;
    if (updateCategory(nodeHalfLine,id)) id++;
    if (updateCategory(nodeList,id)) id++;
    if (updateCategory(nodeUndef,id)) id++;

}
void PanelProperties::selectInTree(MyItem * item){
   QTreeWidgetItem* treeItem=nodeLinks.key(item);
   tree->collapseAll();
    tree->clearSelection();
    treeItem->setSelected(true);
    treeItem->parent()->setExpanded(true);
}
void PanelProperties::clearSelection(){
    if (!tree->selectedItems().isEmpty()) {
        tree->clearSelection();
        parent->repaint();
        displayPanel->hide();
    }
}
void PanelProperties::updateValueInDisplayPanel(){
    displayPanel->updateValueInDisplayPanel();

}
DisplayProperties::DisplayProperties(Canvas2D *canvas):QTabWidget(canvas){
    parent=canvas;
    initGui();
    listItems=0;
}
void DisplayProperties::updateDisplayPanel(QList<MyItem *> * l){

    setVisible(true);
    delete listItems;
    listItems=l;
    if (l->count()>1) valuePanel->setVisible(false);
   else if (!QString::fromStdString(giac::print(listItems->at(0)->getValue(),parent->getContext())).trimmed().isEmpty()){
        valuePanel->setGenValue(listItems->at(0)->getValue());
        valuePanel->setDisplayValue(listItems->at(0)->getDisplayValue());

        valuePanel->setVisible(true);
    }
    else valuePanel->setVisible(false);

    displayObjectPanel->setChecked(listItems->at(0)->isVisible());

    colorPanel->setColor(listItems->at(0)->getColor());
    legendPanel->setChecked(listItems->at(0)->legendVisible());
    if (l->count()>1){
        legendPanel->setLegend(false);
    }
    else {
        legendPanel->setLegend(true,listItems->at(0)->getLegend());
    }
    bool b=parent->checkForOnlyPoints(listItems);
    if (listItems->at(0)->getAngleLegend()==-1){
        legendPanel->setLegendPosition(listItems->at(0)->getQuadrant());

    }
//    else legendPanel->setLegendPosition(listItems->at(0)->getAngleLegend());

    widthPanel->setValue(listItems->at(0)->getPenWidth()+1);

    if (b){
        typeLinePanel->hide();
        typePointPanel->setStyle(dynamic_cast<Point*>(listItems->at(0))->getPointStyle()>>25);
        typePointPanel->setVisible(true);
    }
    else if (parent->checkForOnlyLines(listItems)) {
        typePointPanel->hide();
        typeLinePanel->setStyle(listItems->at(0)->getStyle());
        typeLinePanel->setVisible(true);
    }
    else{
        typeLinePanel->hide();
        typePointPanel->hide();
    }
    if (parent->checkForOnlyFillables(listItems)) {
        if (listItems->at(0)->isFilled())
        alphaFillPanel->setValue(8-listItems->at(0)->getColor().alpha()/36);
        else alphaFillPanel->setValue(8);
        alphaFillPanel->setVisible(true);

    }


    else alphaFillPanel->setVisible(false);
}
void DisplayProperties::updateValueInDisplayPanel(){
    if (listItems==0) return;
    if (listItems->isEmpty()) {setVisible(false);return;}
        if (!listItems->isEmpty())
        valuePanel->setDisplayValue(listItems->at(0)->getDisplayValue());
        valuePanel->setGenValue(listItems->at(0)->getValue());
        generalPanel->updateGeometry();
}
void DisplayProperties::initGui(){
    setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Maximum);
    generalPanel=new QWidget(this);
    vLayoutGeneral=new QVBoxLayout;
    valuePanel=new GenValuePanel(parent);
    valuePanel->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    displayObjectPanel=new DisplayObjectPanel(this);
    displayObjectPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    legendPanel=new LegendPanel(this);
    legendPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    vLayoutGeneral->addWidget(valuePanel);
    vLayoutGeneral->addWidget(legendPanel);
    vLayoutGeneral->addWidget(displayObjectPanel);
    generalPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    generalPanel->setLayout(vLayoutGeneral);

    attributesPanel=new QWidget;
    vLayoutAttributes=new QVBoxLayout;
    colorPanel=new ColorPanel(this);
    colorPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    widthPanel=new WidthPanel(this,tr("Épaisseur (1-8):"));
    widthPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    typePointPanel=new TypePointPanel(2,this);
    typePointPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    typeLinePanel=new TypeLinePanel(2,this);
    typeLinePanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    alphaFillPanel=new AlphaFillPanel(this,tr("Transparence (0%-100%):"));
    alphaFillPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    vLayoutAttributes->addWidget(colorPanel);
    vLayoutAttributes->addWidget(widthPanel);
    vLayoutAttributes->addWidget(typePointPanel);
    vLayoutAttributes->addWidget(typeLinePanel);
    vLayoutAttributes->addWidget(alphaFillPanel);
    attributesPanel->setLayout(vLayoutAttributes);

    addTab(generalPanel,tr("Général"));
    addTab(attributesPanel,tr("Style"));

    connect(colorPanel,SIGNAL(colorSelected(QColor)),this,SLOT(updateColor(QColor)));
    connect(typeLinePanel,SIGNAL(typeLineSelected(int)),this,SLOT(updateTypeLine(int)));
}
void DisplayProperties::updateColor(QColor color){
    for (int i=0;i<listItems->count();++i){
        QColor c(color);
        c.setAlpha(listItems->at(i)->getColor().alpha());
        listItems->at(i)->setColor(c);
    }
    updateCanvas();
}
void DisplayProperties::updateTypeLine(int c){
    for (int i=0;i<listItems->count();++i){
        listItems->at(i)->setStyle(c);
    }
    updateCanvas();
}

void DisplayProperties::updateCanvas(){

    parent->updatePixmap(false);
    parent->repaint();
}
QList<MyItem*>* DisplayProperties::getListItems() const{
    return listItems;
}


ColorPanel::ColorPanel(QWidget *p):QWidget(p){
    initGui();
}
void ColorPanel::setColor(const QColor & c){
    color=c;
    updateButton();
}
void ColorPanel::initGui(){
    QHBoxLayout* hLayout=new QHBoxLayout;

    QLabel * colorLabel=new QLabel(tr("Couleur:"),this);
    preview=new QPushButton;
    updateButton();
    hLayout->addWidget(colorLabel);
    hLayout->addWidget(preview);
    setLayout(hLayout);

    connect(preview,SIGNAL(clicked()),this,SLOT(chooseColor()));

}
void ColorPanel::chooseColor(){
    QColor newColor=QColorDialog::getColor(color,this);
    if (newColor.isValid()){
        color=newColor;
        updateButton();
        emit colorSelected(color);
    }
}

void ColorPanel::updateButton(){
    QPixmap pixmap=QPixmap(30,30);
    pixmap.fill(color);
    QIcon icon(pixmap);
    preview->setIcon(icon);

}

LegendPanel::LegendPanel(DisplayProperties * p):QWidget(p){
    parent=p;
    initGui();
}
void LegendPanel::setLegend(const bool&b,const QString & s){
    legend=s;
    legendEdit->setText(legend);
    legendPanel->setVisible(b);
}
void LegendPanel::setChecked(bool b){
    legendCheck->setChecked(b);
    legendPanel->setEnabled(b);
}

void LegendPanel::setLegendPosition(const int & index){
    disconnect(comboPos,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas()));
    comboPos->setCurrentIndex(index);
     connect(comboPos,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas()));

}
void LegendPanel::initGui(){
    QHBoxLayout * hbox=new QHBoxLayout;
    QHBoxLayout * hboxPos=new QHBoxLayout;
    QVBoxLayout * vbox=new QVBoxLayout;

    legendCheck=new QCheckBox(tr("Afficher la légende"),this);

    QLabel * legendLabel=new QLabel(tr("Légende:"),this);
    legendEdit=new QLineEdit(this);
    hbox->addWidget(legendLabel);
    hbox->addWidget(legendEdit);
    legendPanel=new QWidget;
    legendPanel->setLayout(hbox);

    QLabel * legendPosLabel=new QLabel(tr("Position:"),this);
    spinAnglePos=new QSpinBox;
    spinAnglePos->setRange(0,359);
    spinAnglePos->setSingleStep(1);
    comboPos=new QComboBox(this);
    comboPos->addItem(tr("quadrant1"));
    comboPos->addItem(tr("quadrant2"));
    comboPos->addItem(tr("quadrant3"));
    comboPos->addItem(tr("quadrant4"));
    comboPos->addItem(tr("personnalisé"));

    hboxPos->addWidget(legendPosLabel);
    hboxPos->addWidget(comboPos);
//    hboxPos->addWidget(spinAnglePos);
    legendPosPanel=new QWidget;
    legendPosPanel->setLayout(hboxPos);

    vbox->addWidget(legendCheck);
    vbox->addWidget(legendPanel);
    vbox->addWidget(legendPosPanel);
    setLayout(vbox);

    connect(legendCheck,SIGNAL(toggled(bool)),legendPanel,SLOT(setEnabled(bool)));
    connect(legendCheck,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    connect(legendEdit,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(comboPos,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas()));
}

void LegendPanel::updateCanvas(){
//    qDebug()<<legendCheck->isChecked()<<legendEdit->text()<<comboPos->currentIndex();
    for (int i=0;i<parent->getListItems()->count();++i){
        parent->getListItems()->at(i)->setLegendVisible(legendCheck->isChecked());
        if (legendPanel->isVisible()) parent->getListItems()->at(i)->setLegend(legendEdit->text());
        parent->getListItems()->at(i)->setLegendPos(comboPos->currentIndex());
    }
    parent->updateCanvas();
}

SliderPanel::SliderPanel(DisplayProperties *p,const QString & s):QGroupBox(p){
    parent=p;
    initGui(s);
}
void SliderPanel::setValue(const int w ){
    value=w;
    disconnect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateCanvas()));
    slider->setValue(value);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateCanvas()));

}
void SliderPanel::initGui(const QString &s){
    setTitle(s);
    QHBoxLayout*hbox=new QHBoxLayout;
    slider=new QSlider(Qt::Horizontal,this);
    slider->setMinimum(1);
    slider->setMaximum(8);
    slider->setSingleStep(1);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksAbove);
    hbox->addWidget(slider);
    setLayout(hbox);
    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(updateCanvas()));

}
WidthPanel::WidthPanel(DisplayProperties* p,const QString & d):SliderPanel(p,d){}
void WidthPanel::updateCanvas(){
    for (int i=0;i<parent->getListItems()->count();++i){
        parent->getListItems()->at(i)->setWidth(slider->value()-1);
    }
    parent->updateCanvas();
}
AlphaFillPanel::AlphaFillPanel(DisplayProperties* p,const QString & d):SliderPanel(p,d){}
void AlphaFillPanel::updateCanvas(){
    for (int i=0;i<parent->getListItems()->count();++i){
        QColor c=parent->getListItems()->at(i)->getColor();
        int alpha=(8-slider->value())*36;
        c.setAlpha(alpha);
        if (alpha!=252) parent->getListItems()->at(i)->setFilled(true);
        parent->getListItems()->at(i)->setColor(c);
    }
    parent->updateCanvas();
}
TypePointPanel::TypePointPanel(const int t, DisplayProperties *p):QWidget(p){
    parent=p;
    type=t;
    initGui();
}
void TypePointPanel::initGui(){
    QHBoxLayout* hbox=new  QHBoxLayout;
    QLabel * labelType=new QLabel(tr("Style:"));
    combo=new QComboBox;

    combo->setIconSize(QSize(40,30));
    QPixmap pixmap(40,30);
    QPainterPath p;

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);

    pixmap.fill(Qt::white);
    p.moveTo(13,8);
    p.lineTo(27,22);
    p.moveTo(13,22);
    p.lineTo(27,8);
    painter.setPen(QPen(Qt::black,2));
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_croix"));


    pixmap.fill(Qt::white);
    p=QPainterPath();
    p.moveTo(20,5);
    p.lineTo(30,15);
    p.lineTo(20,25);
    p.lineTo(10,15);
    p.closeSubpath();
    painter.setPen(QPen(Qt::black,2));
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_losange"));

    pixmap.fill(Qt::white);
    p=QPainterPath();
    p.moveTo(20,5);
    p.lineTo(20,25);
    p.moveTo(10,15);
    p.lineTo(30,15);
    painter.setPen(QPen(Qt::black,2));
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_plus"));


    pixmap.fill(Qt::white);
    p=QPainterPath();
    p.moveTo(13,8);
    p.lineTo(27,8);
    p.lineTo(27,22);
    p.lineTo(13,22);
    p.closeSubpath();
    painter.setPen(QPen(Qt::black,2));
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_square"));

    pixmap.fill(Qt::white);
    combo->addItem(pixmap,tr("point_invisible"));


    pixmap.fill(Qt::white);
    p=QPainterPath();
    p.moveTo(20,5);
    p.lineTo(29,20);
    p.lineTo(11,20);
    p.closeSubpath();
    painter.setPen(QPen(Qt::black,2));
    painter.setBrush(QBrush());
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_triangle"));


    pixmap.fill(Qt::white);
    p=QPainterPath();
    p.moveTo(20,5);
    p.lineTo(20,25);
    p.moveTo(13,8);
    p.lineTo(27,22);
    p.moveTo(27,8);
    p.lineTo(13,22);
    painter.setPen(QPen(Qt::black,2));
    painter.drawPath(p);
    combo->addItem(pixmap,tr("point_etoile"));



    pixmap.fill(Qt::white);

    p=QPainterPath();
    painter.setPen(QPen(Qt::black,2));
    painter.setBrush(Qt::SolidPattern);
    painter.drawEllipse(QPoint(20,15),10,10);
    combo->addItem(pixmap,tr("point_point"));


    hbox->addWidget(labelType);
    hbox->addWidget(combo);
    setLayout(hbox);

    connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));
}


void TypePointPanel::updateCanvas(int c){
    for (int i=0;i<parent->getListItems()->count();++i){
        parent->getListItems()->at(i)->setPointStyle(c);
    }
    parent->updateCanvas();

}
void TypePointPanel::setStyle(int c){
    disconnect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));
    combo->setCurrentIndex(c);
    connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));

}

TypeLinePanel::TypeLinePanel(const int t, QWidget* p):QWidget(p){
    type=t;
    initGui();
}
void TypeLinePanel::initGui(){
    QHBoxLayout* hbox=new  QHBoxLayout;
    QLabel * labelType=new QLabel(tr("Style:"));
    combo=new QComboBox;

    combo->setIconSize(QSize(40,30));
    QPixmap pixmap(40,30);
    QPainterPath p;

    QPainter painter(&pixmap);
    pixmap.fill(Qt::white);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setPen(QPen(Qt::black,2));
    painter.setBrush(QBrush(Qt::SolidPattern));
    painter.drawLine(5,15,35,15);
    combo->addItem(pixmap,tr("solid_line"));

    pixmap.fill(Qt::white);
    painter.setPen(QPen(Qt::black,2,Qt::DashLine));
    painter.drawLine(5,15,35,15);
    combo->addItem(pixmap,tr("dash_line"));

    pixmap.fill(Qt::white);
    painter.setPen(QPen(Qt::black,2,Qt::DotLine));
    painter.drawLine(5,15,35,15);
    combo->addItem(pixmap,tr("dot_line"));


    pixmap.fill(Qt::white);
    painter.setPen(QPen(Qt::black,2,Qt::DashDotLine));
    painter.drawLine(5,15,35,15);
    combo->addItem(pixmap,tr("dashdot_line"));

    pixmap.fill(Qt::white);
    painter.setPen(QPen(Qt::black,2,Qt::DashDotDotLine));
    painter.drawLine(5,15,35,15);
    combo->addItem(pixmap,tr("dashdotdot_line"));


    pixmap.fill(Qt::white);
    painter.setPen(QPen(Qt::black,2,Qt::SolidLine));
    painter.fillRect(QRect(10,10,20,10),Qt::SolidPattern);
    combo->addItem(pixmap,tr("cap_flat"));


    pixmap.fill(Qt::white);
    painter.setBrush(QBrush(QColor(0,0,0,100),Qt::SolidPattern));
    painter.setPen(QPen(QColor(0,0,0,100),1,Qt::SolidLine));
    painter.drawEllipse(QPoint(10,15),5,5);
    painter.drawEllipse(QPoint(30,15),5,5);

    painter.setPen(QPen(Qt::black,2,Qt::SolidLine));
    painter.fillRect(QRect(10,10,20,10),Qt::SolidPattern);
    combo->addItem(pixmap,tr("cap_round"));


    pixmap.fill(Qt::white);
    painter.setBrush(QBrush(QColor(0,0,0,100),Qt::SolidPattern));
    painter.setPen(QPen(QColor(0,0,0,100),1,Qt::SolidLine));
    painter.drawRect(5,10,30,10 );
    painter.setPen(QPen(Qt::black,2,Qt::SolidLine));
    painter.fillRect(10,10,20,10,Qt::SolidPattern);
    combo->addItem(pixmap,tr("cap_square"));

    hbox->addWidget(labelType);
    hbox->addWidget(combo);
    setLayout(hbox);

    connect(combo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(typeLineSelected(int)));
}

void TypeLinePanel::setStyle(const int &c){
    disconnect(combo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(typeLineSelected(int)));
    combo->setCurrentIndex(c);
    connect(combo,SIGNAL(currentIndexChanged(int)),this,SIGNAL(typeLineSelected(int)));

}

DisplayObjectPanel::DisplayObjectPanel(DisplayProperties * p):QWidget(p){
    parent=p;
    initGui();
}
void DisplayObjectPanel::initGui(){
    QHBoxLayout* hbox=new QHBoxLayout;
    displayObject=new QCheckBox(tr("Afficher l'objet"));
   hbox->addWidget(displayObject,Qt::AlignLeft);
   setLayout(hbox);
}
void DisplayObjectPanel::setChecked(const bool b){
    disconnect(displayObject,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    displayObject->setChecked(b);
    connect(displayObject,SIGNAL(clicked()),this,SLOT(updateCanvas()));
}

void DisplayObjectPanel::updateCanvas(){
    for (int i=0;i<parent->getListItems()->count();++i){
        parent->getListItems()->at(i)->setVisible(displayObject->isChecked());
    }
    parent->updateCanvas();

}
GenValuePanel::GenValuePanel(Canvas2D * p):QWidget(p){
    parent=p;
    initGui();
}
void GenValuePanel::initGui(){
    layout=new QHBoxLayout(this);
//    label=new QLabel
    formulaWidget=new FormulaWidget(this);
    layout->addWidget(formulaWidget,Qt::AlignLeft);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);
}
void GenValuePanel::setGenValue(const giac::gen  &g){
    formulaWidget->setGen(g);
}
void GenValuePanel::setDisplayValue(const QString  s){

      formulaWidget->updateFormula(s);

//      formulaWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

}

AxisGridPanel::AxisGridPanel(Canvas2D * p):QTabWidget(p){
    parent=p;
    initGui();
}
void AxisGridPanel::initGui(){
    xPanel=new AxisPanel(this);
    yPanel=new AxisPanel(this);
    gridPanel=new GridPanel(this);
    addTab(xPanel,tr("Axe (Ox)"));
    addTab(yPanel,tr("Axe (Oy)"));
    addTab(gridPanel,tr("Grille"));
    connect(gridPanel,SIGNAL(gridUpdated(GridParam)),this,SLOT(updateGrid(GridParam)));
    connect(xPanel,SIGNAL(axisUpdated(AxisParam,bool)),this,SLOT(updateXAxis(AxisParam,bool)));
    connect(yPanel,SIGNAL(axisUpdated(AxisParam,bool)),this,SLOT(updateYAxis(AxisParam,bool)));
}
void AxisGridPanel::updateXAxis(AxisParam p, bool b){
    if (p.min<p.max) {
        parent->setXAxisParam(p);
        parent->setBounds(parent->getXAxisParam().min,parent->getXAxisParam().max,
                    parent->getYmin(),parent->getYmax());
        updateCanvas(b);
    }
}
void AxisGridPanel::updateYAxis(AxisParam p, bool b){
    if (p.min<p.max) {
        parent->setYAxisParam(p);
        parent->setBounds(parent->getXmin(),parent->getXmax(),parent->getYAxisParam().min
                          ,parent->getYAxisParam().max);
        updateCanvas(b);
    }
}
void AxisGridPanel::updateCanvas(const bool & b){

    if (b) parent->setXYUnit();
    parent->updatePixmap(b);
    parent->repaint();
}

void AxisGridPanel::updateGrid(GridParam p){
    parent->setGridParam(p);
    parent->updatePixmap(false);
    parent->repaint();
}

void AxisGridPanel::initValue(){
    xPanel->initValue(parent->getXAxisParam(),parent->getXmin(),parent->getXmax());
    yPanel->initValue(parent->getYAxisParam(),parent->getYmin(),parent->getYmax());
    gridPanel->initValue(parent->getGridParam());

}

GridPanel::GridPanel(QWidget* p):QWidget(p){
        initGui();
}
/**
 * @brief GridPanel::initValue initializes all value in grid Panel
 *@param p All grid parameters
 *
 */
void GridPanel::initValue(const GridParam &p){
    param=p;
    showGrid->setChecked(p.isVisible);
    if (p.isCartesian) comboType->setCurrentIndex(0);
    else comboType->setCurrentIndex(1);
    displayValidPanel(comboType->currentIndex());
    if (p.x<0) comboX->setCurrentIndex((int)(-p.x-1));
    else comboX->setEditText(QString::number(p.x));
    if (p.y<0) comboY->setCurrentIndex((int)(-p.y-1));
    else comboY->setEditText(QString::number(p.y));
    editDistance->setText(QString::number(p.r));
    comboPolarAngle->setCurrentIndex(p.theta);
    colorPanel->setColor(p.color);
    typeLinePanel->setStyle(p.line);

    connect(showGrid,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    connect(comboType,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas()));
    connect(comboX,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateCanvas()));
    connect(comboY,SIGNAL(currentIndexChanged(QString)),this,SLOT(updateCanvas()));
    connect(editDistance,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(colorPanel,SIGNAL(colorSelected(QColor)),this,SLOT(updateColor(QColor)));
    connect(typeLinePanel,SIGNAL(typeLineSelected(int)),this,SLOT(updateLineType(int)));
    connect(comboPolarAngle,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas()));
}
void GridPanel::updateColor(QColor c){
    param.color=c;
    emit gridUpdated(param);
}
void GridPanel::updateLineType(int id){
    param.line=id;
    emit gridUpdated(param);
}
void GridPanel::updateCanvas(){
    param.isVisible=showGrid->isChecked();
    param.isCartesian=(comboType->currentIndex()==0);
    param.x=comboX->currentText().toDouble();
    if (param.x==0) param.x=1;
    param.y=comboY->currentText().toDouble();
    if (param.y==0) param.y=1;
    param.r=editDistance->text().toDouble();
    if (param.r==0) param.r=1;
    param.theta=(PolarAngle)(comboPolarAngle->currentIndex());
    emit gridUpdated(param);
}
void GridPanel::initGui(){
    QVBoxLayout* vbox=new QVBoxLayout(this);
    showGrid=new QCheckBox(tr("Afficher la grille"),this);
    comboType=new QComboBox(this);
    comboType->addItem(tr("Cartésienne"));
    comboType->addItem(tr("Polaire"));

    polarPanel=new QWidget(this);
    QGridLayout* boxPolar=new QGridLayout(polarPanel);
    QLabel* labelPolarDistance=new QLabel(tr("Distance:"),polarPanel);
    QLabel* labelPolarAngle=new QLabel(tr("Angle:"),polarPanel);
    editDistance=new QLineEdit(polarPanel);
    comboPolarAngle=new QComboBox(polarPanel);
    const QChar pi(0x3c0);
    comboPolarAngle->addItem( QString("%1/12").arg(pi));
    comboPolarAngle->addItem( QString("%1/8").arg(pi));
    comboPolarAngle->addItem( QString("%1/6").arg(pi));
    comboPolarAngle->addItem( QString("%1/4").arg(pi));
    comboPolarAngle->addItem( QString("%1/3").arg(pi));
    comboPolarAngle->addItem( QString("%1/2").arg(pi));

    boxPolar->addWidget(labelPolarDistance,0,0);
    boxPolar->addWidget(editDistance,0,1);
    boxPolar->addWidget(labelPolarAngle,1,0);
    boxPolar->addWidget(comboPolarAngle,1,1);
    polarPanel->setLayout(boxPolar);

    cartesianPanel=new QWidget(this);
    QGridLayout* boxCartesian=new QGridLayout(cartesianPanel);
    QLabel* labelCartesianX=new QLabel(tr("x:"),cartesianPanel);
    QLabel* labelCartesianY=new QLabel(tr("y:"),cartesianPanel);
    comboX=new QComboBox(cartesianPanel);
    comboX->addItem("1");
    comboX->addItem( QString("%1/6").arg(pi));
    comboX->addItem( QString("%1/4").arg(pi));
    comboX->addItem( QString("%1/3").arg(pi));
    comboX->addItem( QString("%1/2").arg(pi));
    comboY=new QComboBox(cartesianPanel);
    comboY->addItem("1");
    comboY->addItem( QString("%1/6").arg(pi));
    comboY->addItem( QString("%1/4").arg(pi));
    comboY->addItem( QString("%1/3").arg(pi));
    comboY->addItem( QString("%1/2").arg(pi));
    comboX->setEditable(true);
    comboY->setEditable(true);
    boxCartesian->addWidget(labelCartesianX,0,0);
    boxCartesian->addWidget(comboX,0,1);
    boxCartesian->addWidget(labelCartesianY,1,0);
    boxCartesian->addWidget(comboY,1,1);

    colorPanel=new ColorPanel(this);
    typeLinePanel=new TypeLinePanel(0,this);

    vbox->addWidget(showGrid);
    vbox->addWidget(comboType);
    vbox->addWidget(cartesianPanel);
    vbox->addWidget(polarPanel);
    vbox->addWidget(colorPanel);
    vbox->addWidget(typeLinePanel);
    setLayout(vbox);
    connect(comboType,SIGNAL(currentIndexChanged(int)),this,SLOT(displayValidPanel(int)));
//    QLabel* labelX=new QLabel(tr("x:"),this);
//    QLabel* labelY=new QLabel(tr("y:"),this);

}

void GridPanel::displayValidPanel(int id){
    if (id==0){
        polarPanel->setVisible(false);
        cartesianPanel->setVisible(true);
    }
    else{
        polarPanel->setVisible(true);
        cartesianPanel->setVisible(false);
    }
}

AxisPanel::AxisPanel(QWidget *p):QWidget(p){
    initGui();

}

void AxisPanel::initGui(){
    QGridLayout *grid=new QGridLayout(this);

    showAxis=new QCheckBox(tr("Afficher l'axe"),this);
    QLabel * labelMin=new QLabel(tr("Minimum:"),this);
    editMin=new QLineEdit(this);
    editMin->setValidator(new QDoubleValidator);
    editMin->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Maximum);

    QLabel * labelMax=new QLabel(tr("Maximum:"),this);
    editMax=new QLineEdit(this);
    editMax->setValidator(new QDoubleValidator);

    QLabel * labelLegend=new QLabel(tr("Légende:"),this);
    editLabel=new QLineEdit(this);
    QLabel * labelUnit=new QLabel(tr("Suffixe (unité):"),this);
    editUnitLabel=new QLineEdit(this);
    QLabel * labelDistance=new QLabel(tr("Espace-graduations:"),this);
    editDistance=new QLineEdit(this);
    editDistance->setValidator(new QDoubleValidator);
    colorPanel=new ColorPanel(this);
    grid->addWidget(showAxis,0,0,1,2);
    grid->addWidget(labelMin,1,0);
    grid->addWidget(editMin,1,1);
    grid->addWidget(labelMax,2,0);
    grid->addWidget(editMax,2,1);
    grid->addWidget(labelLegend,3,0);
    grid->addWidget(editLabel,3,1);
    grid->addWidget(labelUnit,4,0);
    grid->addWidget(editUnitLabel,4,1);
    grid->addWidget(labelDistance,5,0);
    grid->addWidget(editDistance,5,1);
    grid->addWidget(colorPanel,6,0,2,1);
}
void AxisPanel::updateCanvas(){
    QLineEdit* edit=qobject_cast<QLineEdit*>(sender());
    bool b=false;
    if (editMin==edit|| editMax==edit) b=true;

    AxisParam p;
    p.tick=editDistance->text().toDouble();
    p.legend=editLabel->text();
    p.unitSuffix=editUnitLabel->text();
    p.isVisible=showAxis->isChecked();
    p.min=editMin->text().toDouble();
    p.max=editMax->text().toDouble();
    p.color=color;


   emit axisUpdated(p,b);
}
void AxisPanel::updateColor(QColor c){
    color=c;
    updateCanvas();
}
void AxisPanel::initValue(const AxisParam& p, const double & min, const double &max){
//    qDebug()<<min<<max;
    disconnect(showAxis,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    disconnect(editMin,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    disconnect(editMax,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    disconnect(editLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    disconnect(editUnitLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    disconnect(editDistance,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    disconnect(colorPanel,SIGNAL(colorSelected(QColor)),this,SLOT(updateColor(QColor)));

    editLabel->setText(p.legend);
    editUnitLabel->setText(p.unitSuffix);
    editMin->setText(QString::number(min));
    editMax->setText(QString::number(max));
    showAxis->setChecked(p.isVisible);
    editDistance->setText(QString::number(p.tick));
    colorPanel->setColor(p.color);
    color=p.color;
    connect(showAxis,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    connect(editMin,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editMax,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editUnitLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editDistance,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(colorPanel,SIGNAL(colorSelected(QColor)),this,SLOT(updateColor(QColor)));
}


SourceDialog::SourceDialog(Canvas2D* p){
    parent=p;
    initGui();
}


void SourceDialog::initGui(){
    QHBoxLayout* hbox=new QHBoxLayout(this);
    listWidget=new QListWidget(this);
    QList<Canvas2D::Command> commands=parent->getCommands();
    QStringList list;
    parent->getDisplayCommands(list);
    listWidget->addItems(list);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    deleteButton=new QPushButton(tr("Supprimer"),this);
    hbox->addWidget(listWidget);
    hbox->addWidget(deleteButton);
    hbox->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(hbox);
    connect(deleteButton,SIGNAL(clicked()),this,SLOT(updateCanvas()));
}
void SourceDialog::updateCanvas(){
    int id=listWidget->currentRow();
    parent->deleteObject(parent->getCommands().at(id).item);
    listWidget->clear();
    QStringList list;
    parent->getDisplayCommands(list);
    listWidget->addItems(list);


}
CoordsDialog::CoordsDialog(Canvas2D* p):QDialog (p){
    initGui();
}


void CoordsDialog::initGui(){

    QGridLayout* grid=new QGridLayout(this);
    QLabel* labelx=new QLabel(tr("Abscisse x:"),this);
    editX=new QLineEdit(this);
    QLabel* labely=new QLabel(tr("Ordonnée y:"),this);
    ok=new QPushButton(tr("Ok"),this);
    cancel=new QPushButton(tr("Annuler"),this);
    editY=new QLineEdit(this);
    setTabOrder(editX,editY);
    setTabOrder(editY,ok);
    setTabOrder(ok,cancel);
    setTabOrder(cancel,editX);
    editX->setFocus();

    grid->addWidget(labelx,0,0);
    grid->addWidget(editX,0,1);
    grid->addWidget(labely,1,0);
    grid->addWidget(editY,1,1);
    grid->addWidget(ok,0,2);
    grid->addWidget(cancel,1,2);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(grid);

    connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

}
OneArgDialog::OneArgDialog(Canvas2D* p, const QString & t):QDialog (p){
    type=t;
    initGui();
}


void OneArgDialog::initGui(){
    QGridLayout* grid=new QGridLayout(this);
    QLabel* labelRadius=new QLabel(type,this);
    editRadius=new QLineEdit(this);
    ok=new QPushButton(tr("Ok"),this);
    cancel=new QPushButton(tr("Annuler"),this);
    setTabOrder(editRadius,ok);
    setTabOrder(ok,cancel);
    setTabOrder(cancel,editRadius);
    editRadius->setFocus();

    grid->addWidget(labelRadius,0,0);
    grid->addWidget(editRadius,0,1);
    grid->addWidget(ok,0,2);
    grid->addWidget(cancel,1,2);
    setLayout(grid);
    grid->setSizeConstraint(QLayout::SetFixedSize);
    connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

}
CursorDialog::CursorDialog(Canvas2D * p):QDialog(p){
    initGui();
}
void CursorDialog::initGui(){
    QGridLayout* grid=new QGridLayout(this);
    QLabel * labVar=new QLabel(tr("Nom:"));
    editVar=new QLineEdit;
    QLabel * labMin=new QLabel(tr("Min:"));
    editMin=new QLineEdit("-5");
    QLabel * labMax=new QLabel(tr("Max:"));
    editMax=new QLineEdit("5");
    QLabel * labStep=new QLabel(tr("Pas:"));
    editStep=new QLineEdit("0.1");
    QLabel * labDefault=new QLabel(tr("Défaut:"));
    editDefault=new QLineEdit("1");

    ok=new QPushButton(tr("Ok"));
    cancel=new QPushButton(tr("Annuler"));
    grid->addWidget(labVar,0,0,Qt::AlignCenter);
    grid->addWidget(editVar,0,1);
    grid->addWidget(labMin,1,0,Qt::AlignCenter);
    grid->addWidget(editMin,1,1);
    grid->addWidget(labMax,2,0,Qt::AlignCenter);
    grid->addWidget(editMax,2,1);
    grid->addWidget(labStep,3,0,Qt::AlignCenter);
    grid->addWidget(editStep,3,1);
    grid->addWidget(labDefault,4,0,Qt::AlignCenter);
    grid->addWidget(editDefault,4,1);
    grid->addWidget(ok,5,0);
    grid->addWidget(cancel,5,1);
    setLayout(grid);
    editVar->setFocus();
    grid->setSizeConstraint(QLayout::SetFixedSize);
    connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

}
QString CursorDialog::getDefault() const{
    return editDefault->text();
}

QString CursorDialog::getMax() const{
    return editMax->text();
}
QString CursorDialog::getMin() const{
    return editMin->text();
}
QString CursorDialog::getStep() const{
    return editStep->text();
}
QString CursorDialog::getVar() const{
    return editVar->text();
}


/*
  void Graph2d3d::update_infos(const gen & g,GIAC_CONTEXT){
    if (g.is_symb_of_sommet(at_equal)){
      // detect a title or a x/y-axis name
      gen & f = g._SYMBptr->feuille;
      if (f.type==_VECT && f._VECTptr->size()==2){
    gen & optname = f._VECTptr->front();
    gen & optvalue= f._VECTptr->back();
    if (optname==at_legende && optvalue.type==_VECT){
      vecteur & optv=(*optvalue._VECTptr);
      int optvs=optv.size();
      if (optvs>=1)
        x_axis_unit=printstring(optv[0],contextptr);
      if (optvs>=2)
        y_axis_unit=printstring(optv[1],contextptr);
      if (optvs>=3)
        z_axis_unit=printstring(optv[2],contextptr);
    }
    if (optname.type==_INT_ && optname.subtype == _INT_PLOT){
      if (optname.val==_GL_TEXTURE){
        if (optvalue.type==_STRNG){
          get_texture2d(*optvalue._STRNGptr,background_image);
        }
        else
          background_image=0;
      }
      if (optname.val==_TITLE )
        title=printstring(optvalue,contextptr);
      if (optname.val==_AXES){
        if (optvalue.type==_INT_)
          show_axes=optvalue.val;
      }
      if (optname.val==_LABELS && optvalue.type==_VECT){
        vecteur & optv=(*optvalue._VECTptr);
        int optvs=optv.size();
        if (optvs>=1)
          x_axis_name=printstring(optv[0],contextptr);
        if (optvs>=2)
          y_axis_name=printstring(optv[1],contextptr);
        if (optvs>=3)
          z_axis_name=printstring(optv[2],contextptr);
      }
      if (optname.val==_GL_ORTHO && optvalue==1)
        orthonormalize();
      if (optname.val==_GL_X_AXIS_COLOR && optvalue.type==_INT_)
        x_axis_color=optvalue.val;
      if (optname.val==_GL_Y_AXIS_COLOR && optvalue.type==_INT_)
        y_axis_color=optvalue.val;
      if (optname.val==_GL_Z_AXIS_COLOR && optvalue.type==_INT_)
        z_axis_color=optvalue.val;
      if (optname.val>=_GL_X && optname.val<=_GL_Z && optvalue.is_symb_of_sommet(at_interval)){
        gen optvf=evalf_double(optvalue._SYMBptr->feuille,1,contextptr);
        if (optvf.type==_VECT && optvf._VECTptr->size()==2){
          gen a=optvf._VECTptr->front();
          gen b=optvf._VECTptr->back();
          if (a.type==_DOUBLE_ && b.type==_DOUBLE_){
        switch (optname.val){
        case _GL_X:
          window_xmin=a._DOUBLE_val;
          window_xmax=b._DOUBLE_val;
          break;
        case _GL_Y:
          window_ymin=a._DOUBLE_val;
          window_ymax=b._DOUBLE_val;
          break;
        case _GL_Z:
          window_zmin=a._DOUBLE_val;
          window_zmax=b._DOUBLE_val;
          break;
        }
          }
        }
      }
      gen optvalf=evalf_double(optvalue,1,contextptr);
      if (optname.val==_GL_XTICK && optvalf.type==_DOUBLE_)
        x_tick=optvalf._DOUBLE_val;
      if (optname.val==_GL_YTICK && optvalf.type==_DOUBLE_)
        y_tick=optvalf._DOUBLE_val;
      if (optname.val==_GL_ZTICK && optvalf.type==_DOUBLE_)
        z_tick=optvalf._DOUBLE_val;
      if (optname.val==_GL_ANIMATE && optvalf.type==_DOUBLE_)
        animation_dt=optvalf._DOUBLE_val;
      if (optname.val==_GL_SHOWAXES && optvalue.type==_INT_)
        show_axes=optvalue.val;
      if (optname.val==_GL_SHOWNAMES && optvalue.type==_INT_)
        show_names=optvalue.val;
      if (optname.val>=_GL_X_AXIS_NAME && optname.val<=_GL_Z_AXIS_UNIT && optvalue.type==_STRNG){
        if (optname.val==_GL_X_AXIS_NAME) x_axis_name=*optvalue._STRNGptr;
        if (optname.val==_GL_Y_AXIS_NAME) y_axis_name=*optvalue._STRNGptr;
        if (optname.val==_GL_Z_AXIS_NAME) z_axis_name=*optvalue._STRNGptr;
        if (optname.val==_GL_X_AXIS_UNIT) x_axis_unit=*optvalue._STRNGptr;
        if (optname.val==_GL_Y_AXIS_UNIT) y_axis_unit=*optvalue._STRNGptr;
        if (optname.val==_GL_Z_AXIS_UNIT) z_axis_unit=*optvalue._STRNGptr;
      }
      if (optname.val==_GL_QUATERNION && optvalf.type==_VECT && optvalf._VECTptr->size()==4){
        vecteur & optvalv=*optvalf._VECTptr;
        if (optvalv[0].type==_DOUBLE_ && optvalv[1].type==_DOUBLE_ &&
        optvalv[2].type==_DOUBLE_ && optvalv[3].type==_DOUBLE_){
          q.x=optvalv[0]._DOUBLE_val;
          q.y=optvalv[1]._DOUBLE_val;
          q.z=optvalv[2]._DOUBLE_val;
          q.w=optvalv[3]._DOUBLE_val;
        }
      }
      if (dynamic_cast<Graph3d *>(this)){
        if (optname.val==_GL_ROTATION_AXIS && optvalf.type==_VECT && optvalf._VECTptr->size()==3){
          vecteur & optvalv=*optvalf._VECTptr;
          if (optvalv[0].type==_DOUBLE_ && optvalv[1].type==_DOUBLE_ &&
          optvalv[2].type==_DOUBLE_ ){
        rotanim_rx=optvalv[0]._DOUBLE_val;
        rotanim_ry=optvalv[1]._DOUBLE_val;
        rotanim_rz=optvalv[2]._DOUBLE_val;
          }
        }
        if (optname.val==_GL_FLAT && optvalue.type==_INT_){
          display_mode &= (0xffff ^ 0x10);
          if (optvalue.val)
        display_mode |= 0x10;
        }
        if (optname.val==_GL_LIGHT && optvalue.type==_INT_){
          display_mode &= (0xffff ^ 0x8);
          if (optvalue.val)
        display_mode |= 0x8;
        }
        if (optname.val==_GL_PERSPECTIVE && optvalue.type==_INT_){
          display_mode &= (0xffff ^ 0x4);
          if (!optvalue.val)
        display_mode |= 0x4;
        }
        // GL_LIGHT_MODEL_COLOR_CONTROL=GL_SEPARATE_SPECULAR_COLOR ||  GL_SINGLE_COLOR
#ifndef WIN32
        if (optname.val==_GL_LIGHT_MODEL_COLOR_CONTROL && optvalue.type==_INT_)
          glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL,optvalue.val);
        /* GL_LIGHT_MODEL_LOCAL_VIEWER=floating-point value that spec-
           ifies how specular reflection angles are computed.  If params
           is 0 (or 0.0),  specular  reflection  angles  take  the  view
           direction  to  be  parallel to and in the direction of the -z
           axis, regardless of the location of the vertex in eye coordi-
           nates.  Otherwise, specular reflections are computed from the
           origin of the eye coordinate system.  The initial value is 0. *//*
        if (optname.val==_GL_LIGHT_MODEL_LOCAL_VIEWER){
          if (optvalf.type==_DOUBLE_)
        glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER,optvalf._DOUBLE_val);
        }
#endif
        // GL_LIGHT_MODEL_TWO_SIDE = true /false /
        if (optname.val==_GL_LIGHT_MODEL_TWO_SIDE && optvalue.type==_INT_){
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,optvalue.val);
        }
        // GL_LIGHT_MODEL_AMBIENT=[r,g,b,a] /
        if (optname.val==_GL_LIGHT_MODEL_AMBIENT && optvalf.type==_VECT && optvalf._VECTptr->size()==4){
          vecteur & w=*optvalf._VECTptr;
          GLfloat tab[4]={w[0]._DOUBLE_val,w[1]._DOUBLE_val,w[2]._DOUBLE_val,w[3]._DOUBLE_val};
          glLightModelfv(GL_LIGHT_MODEL_AMBIENT,tab);
        }
        // gl_blend=[d,s]
        // habituellement gl_blend=[gl_src_alpha,gl_one_minus_src_alpha]
        if (optname.val==_GL_BLEND){
          if (is_zero(optvalue)){
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
          }
          else {
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        if (optvalue.type==_VECT && optvalue._VECTptr->size()==2)
          glBlendFunc(optvalue._VECTptr->front().val,optvalue._VECTptr->back().val);
        if (is_minus_one(optvalue))
          glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
          }
        }
        // gl_light0=[option1=value1,...]
        if (optname.val>=_GL_LIGHT0 && optname.val<=_GL_LIGHT7 && optvalue.type==_VECT){
          int j=optname.val-_GL_LIGHT0;
          // reset light0+j
          light_x[j]=0;light_y[j]=0;light_z[j]=0;light_w[j]=1;
          float di=j?0:1;
          light_diffuse_r[j]=di;light_diffuse_g[j]=di;light_diffuse_b[j]=di;light_diffuse_a[j]=di;
          light_specular_r[j]=di;light_specular_g[j]=di;light_specular_b[j]=di;light_specular_a[j]=di;
          light_ambient_r[j]=0;light_ambient_g[j]=0;light_ambient_b[j]=0;light_ambient_a[j]=1;
          light_spot_x[j]=0;light_spot_y[j]=0;light_spot_z[j]=-1;light_spot_w[j]=0;
          light_spot_exponent[j]=0;light_spot_cutoff[j]=180;
          light_0[j]=1;light_1[j]=0;light_2[j]=0;
          vecteur & optv=*optvalue._VECTptr;
          for (unsigned i=0;i<optv.size();++i){
        gen & optg = optv[i];
        if ( (optg.is_symb_of_sommet(at_equal) || optg.is_symb_of_sommet(at_same) )  && optg._SYMBptr->feuille.type==_VECT && g._SYMBptr->feuille._VECTptr->size()==2){
          gen & optgname = optg._SYMBptr->feuille._VECTptr->front();
          gen optgval = evalf_double(optg._SYMBptr->feuille._VECTptr->back(),1,contextptr);
          bool vect4=optgval.type==_VECT && optgval._VECTptr->size()==4;
          vecteur xyzw;
          if (vect4)
            xyzw=*optgval._VECTptr;
          switch (optgname.val){
          case _GL_AMBIENT:
            light_ambient_r[j]=xyzw[0]._DOUBLE_val;
            light_ambient_g[j]=xyzw[1]._DOUBLE_val;
            light_ambient_b[j]=xyzw[2]._DOUBLE_val;
            light_ambient_a[j]=xyzw[3]._DOUBLE_val;
            break;
          case _GL_SPECULAR:
            light_specular_r[j]=xyzw[0]._DOUBLE_val;
            light_specular_g[j]=xyzw[1]._DOUBLE_val;
            light_specular_b[j]=xyzw[2]._DOUBLE_val;
            light_specular_a[j]=xyzw[3]._DOUBLE_val;
            break;
          case _GL_DIFFUSE:
            light_diffuse_r[j]=xyzw[0]._DOUBLE_val;
            light_diffuse_g[j]=xyzw[1]._DOUBLE_val;
            light_diffuse_b[j]=xyzw[2]._DOUBLE_val;
            light_diffuse_a[j]=xyzw[3]._DOUBLE_val;
            break;
          case _GL_POSITION:
            light_x[j]=xyzw[0]._DOUBLE_val;
            light_y[j]=xyzw[1]._DOUBLE_val;
            light_z[j]=xyzw[2]._DOUBLE_val;
            light_w[j]=xyzw[3]._DOUBLE_val;
            break;
          case _GL_SPOT_DIRECTION:
            light_spot_x[j]=xyzw[0]._DOUBLE_val;
            light_spot_y[j]=xyzw[1]._DOUBLE_val;
            light_spot_z[j]=xyzw[2]._DOUBLE_val;
            light_spot_w[j]=xyzw[3]._DOUBLE_val;
            break;
          case _GL_SPOT_EXPONENT:
            light_spot_exponent[j]=optgval._DOUBLE_val;
            break;
          case _GL_SPOT_CUTOFF:
            light_spot_cutoff[j]=optgval._DOUBLE_val;
            break;
          case _GL_CONSTANT_ATTENUATION:
            light_0[j]=optgval._DOUBLE_val;
            break;
          case _GL_LINEAR_ATTENUATION:
            light_1[j]=optgval._DOUBLE_val;
            break;
          case _GL_QUADRATIC_ATTENUATION:
            light_2[j]=optgval._DOUBLE_val;
            break;
          }
        }
        ;
          } // end for i
        }
      } // end opengl options
    }
      }
    }
    if (g.type==_VECT){
      const_iterateur it=g._VECTptr->begin(),itend=g._VECTptr->end();
      for (;it!=itend;++it)
    update_infos(*it,contextptr);
    }
  }*/





/*  void Graph2d::in_draw(int clip_x,int clip_y,int clip_w,int clip_h,int & vertical_pixels){
    struct timezone tz;
    gettimeofday(&animation_last,&tz);
    gen title_tmp;
    gen plot_tmp;
    History_Pack * hp =get_history_pack(this);
    context * contextptr=hp?hp->contextptr:get_context(this);
    find_title_plot(title_tmp,plot_tmp,contextptr);
    int horizontal_pixels=w()-(show_axes?int(ylegende*labelsize()):0);
    vertical_pixels=h()-((show_axes?1:0)+(!title.empty()))*labelsize();
    int deltax=x(),deltay=y();
    double y_scale=vertical_pixels/(window_ymax-window_ymin);
    double x_scale=horizontal_pixels/(window_xmax-window_xmin);
    // Then redraw the background
    fl_color(FL_WHITE);
    fl_rectf(clip_x, clip_y, clip_w, clip_h);
    fl_color(FL_BLACK);
    if ( !(display_mode & 0x100) )
      fl_rect(x(), y(), horizontal_pixels, vertical_pixels);
    if (background_image){
      if (!background_image->second || background_image->second->w()!=w() || background_image->second->h()!=h()){
    if (background_image->second)
      delete background_image->second;
    background_image->second=background_image->first->copy(w(),h());
      }
      background_image->second->draw(x(),y(),w(),h());
    }
    // History draw
    /***************  //
    int xx,yy,ww,hh;
    fl_clip_box(clip_x,clip_y,horizontal_pixels,vertical_pixels,xx,yy,ww,hh);
    fl_push_clip(xx,yy,ww,hh);
    // fl_push_clip(clip_x,clip_y,horizontal_pixels,vertical_pixels);
    /**************** //
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA,labelsize());
    if ( (display_mode & 2) && !animation_instructions.empty()){
      gen tmp=animation_instructions[animation_instructions_pos % animation_instructions.size()];
      fltk_draw(*this,-1,tmp,x_scale,y_scale,clip_x,clip_y,clip_w,clip_h);
    }
    if ( display_mode & 0x40 ){
      fltk_draw(*this,-1,trace_instructions,x_scale,y_scale,clip_x,clip_y,clip_w,clip_h);
    }
    if (display_mode & 1) {
      const_iterateur at=plot_instructions.begin(),atend=plot_instructions.end(),it,itend;
      for (int plot_i=0;at!=atend;++at,++plot_i){
    if (at->type==_INT_)
      continue;
    update_infos(*at,contextptr);
    if (at->is_symb_of_sommet(at_parameter)){
      gen ff = at->_SYMBptr->feuille;
      vecteur f;
      if (ff.type==_VECT && (f=*ff._VECTptr).size()==4){
        // parameters.push_back(f);
      }
      continue;
    }
    fltk_draw(*this,plot_i,*at,x_scale,y_scale,clip_x,clip_y,clip_w,clip_h);
      } // end for at
    }
    vecteur plot_tmp_v=gen2vecteur(plot_tmp);
    const_iterateur jt=plot_tmp_v.begin(),jtend=plot_tmp_v.end();
    for (;jt!=jtend;++jt){
      gen plot_tmp=*jt;
      if (plot_tmp.is_symb_of_sommet(at_pnt) && plot_tmp._SYMBptr->feuille.type==_VECT && !plot_tmp._SYMBptr->feuille._VECTptr->empty()){
    vecteur & v=*plot_tmp._SYMBptr->feuille._VECTptr;
    // cerr << v << endl;
    if (v[1].type==_INT_)
      plot_tmp=symbolic(at_pnt,makevecteur(v[0],v[1].val | _DOT_LINE | _LINE_WIDTH_2));
    else
      plot_tmp=symbolic(at_pnt,v);
    try {
      fltk_draw(*this,-1,plot_tmp,x_scale,y_scale,clip_x,clip_y,clip_w,clip_h);
    }
    catch (...){
    }
      }
    }
    fl_line_style(0); // back to default line style
    // Draw axis
    double I0,J0;
    findij(zero,x_scale,y_scale,I0,J0,contextptr);
    int i_0=round(I0),j_0=round(J0);
    if ( show_axes &&  (window_ymax>=0) && (window_ymin<=0)){ // X-axis
      fl_color(x_axis_color);
      check_fl_line(deltax,deltay+j_0,deltax+horizontal_pixels,deltay+j_0,clip_x,clip_y,clip_w,clip_h,0,0);
      fl_color(FL_CYAN);
      check_fl_line(deltax+i_0,deltay+j_0,deltax+i_0+int(x_scale),deltay+j_0,clip_x,clip_y,clip_w,clip_h,0,0);
      fl_color(x_axis_color);
      if (x_tick>0 && (horizontal_pixels)/(x_scale*x_tick) < 40){
    double nticks=(horizontal_pixels-I0)/(x_scale*x_tick);
    int count=0;
    for (int ii=int(-I0/(x_tick*x_scale));ii<=nticks && count<25;++ii,++count){
      int iii=int(I0+ii*x_scale*x_tick+.5);
      check_fl_line(deltax+iii,deltay+j_0,deltax+iii,deltay+j_0-4,clip_x,clip_y,clip_w,clip_h,0,0);
    }
      }
      string tmp=x_axis_name.empty()?"x":x_axis_name;
      check_fl_draw(tmp.c_str(),deltax+horizontal_pixels-int(fl_width(tmp.c_str())),deltay+j_0+labelsize(),clip_x,clip_y,clip_w,clip_h,0,0);
    }
    if ( show_axes && (window_xmax>=0) && (window_xmin<=0) ) {// Y-axis
      fl_color(y_axis_color);
      check_fl_line(deltax+i_0,deltay,deltax+i_0,deltay+vertical_pixels,clip_x,clip_y,clip_w,clip_h,0,0);
      fl_color(FL_CYAN);
      check_fl_line(deltax+i_0,deltay+j_0,deltax+i_0,deltay+j_0-int(y_scale),clip_x,clip_y,clip_w,clip_h,0,0);
      fl_color(y_axis_color);
      if (y_tick>0 && vertical_pixels/(y_tick*y_scale) <40 ){
    double nticks=(vertical_pixels-J0)/(y_tick*y_scale);
    int count=0;
    for (int jj=int(-J0/(y_tick*y_scale));jj<=nticks && count<25;++jj,++count){
      int jjj=int(J0+jj*y_scale*y_tick+.5);
      check_fl_line(deltax+i_0,deltay+jjj,deltax+i_0+4,deltay+jjj,clip_x,clip_y,clip_w,clip_h,0,0);
    }
      }
      check_fl_draw(y_axis_name.empty()?"y":y_axis_name.c_str(),deltax+i_0+2,deltay+labelsize(),clip_x,clip_y,clip_w,clip_h,0,0);
    }
    // Ticks
    if (show_axes && (horizontal_pixels)/(x_scale*x_tick) < 40 && vertical_pixels/(y_tick*y_scale) <40  ){
      if (x_tick>0 && y_tick>0 ){
    fl_color(FL_BLACK);
    double nticks=(horizontal_pixels-I0)/(x_scale*x_tick);
    double mticks=(vertical_pixels-J0)/(y_tick*y_scale);
    int count=0;
    for (int ii=int(-I0/(x_tick*x_scale));ii<=nticks;++ii){
      int iii=int(I0+ii*x_scale*x_tick+.5);
      for (int jj=int(-J0/(y_tick*y_scale));jj<=mticks && count<625;++jj,++count){
        int jjj=int(J0+jj*y_scale*y_tick+.5);
        check_fl_point(deltax+iii,deltay+jjj,clip_x,clip_y,clip_w,clip_h,0,0);
      }
    }
      }
    }
    /**************** //
    fl_pop_clip();
    /**************** //
    fl_color(FL_BLACK);
    fl_font(FL_HELVETICA,labelsize());
    if (!args_help.empty() && args_tmp.size()<= args_help.size()){
      fl_draw((gettext("Click ")+args_help[max(1,args_tmp.size())-1]).c_str(),x(),y()+labelsize()-2);
    }
    string mytitle(title);
    if (!is_zero(title_tmp) && function_final.type==_FUNC)
      mytitle=gen(symbolic(*function_final._FUNCptr,title_tmp)).print(contextptr);
    if (!mytitle.empty()){
      int dt=int(fl_width(mytitle.c_str()));
      check_fl_draw(mytitle.c_str(),deltax+(horizontal_pixels-dt)/2,deltay+h()-labelsize()/4,clip_x,clip_y,clip_w,clip_h,0,0);
    }
    // Boundary values
    fl_font(FL_HELVETICA,labelsize());
    if (show_axes){
      int taille,affs,delta;
      vecteur aff;
      string tmp;
      // X
      fl_color(x_axis_color);
      aff=ticks(window_xmin,window_xmax,true);
      affs=aff.size();
      for (int i=0;i<affs;++i){
    double d=evalf_double(aff[i],1,contextptr)._DOUBLE_val;
    tmp=print_DOUBLE_(d);
    delta=int(horizontal_pixels*(d-window_xmin)/(window_xmax-window_xmin));
    taille=int(fl_width(tmp.c_str()));
    if (delta>=taille/2 && delta<=horizontal_pixels){
      fl_line(x()+delta,y()+vertical_pixels,x()+delta,y()+vertical_pixels+3);
      if (args_tmp.empty())
        fl_draw(tmp.c_str(),x()+delta-taille/2,y()+vertical_pixels+labelsize()-2);
    }
      }
      if (args_tmp.empty())
    fl_draw(x_axis_unit.c_str(),x()+horizontal_pixels,y()+vertical_pixels+labelsize()-2);
      // Y
      fl_color(y_axis_color);
      aff=ticks(window_ymin,window_ymax,true);
      affs=aff.size();
      taille=labelsize()/2;
      for (int j=0;j<affs;++j){
    double d=evalf_double(aff[j],1,contextptr)._DOUBLE_val;
    tmp=print_DOUBLE_(d)+y_axis_unit;;
    delta=int(vertical_pixels*(window_ymax-d)/(window_ymax-window_ymin));
    if (delta>=taille && delta<=vertical_pixels-taille){
      fl_line(x()+horizontal_pixels,y()+delta,x()+horizontal_pixels+3,y()+delta);
      fl_draw(tmp.c_str(),x()+horizontal_pixels+3,y()+delta+taille);
    }
      }
    }
  }*/
