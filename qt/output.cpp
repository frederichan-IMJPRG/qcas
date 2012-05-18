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
#include <QTreeWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QClipboard>
#include <QApplication>
#include <QSpinBox>
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
FormulaWidget::FormulaWidget(QWidget *p):OutputWidget(p){
    formula=gen(1);
    context=new giac::context;
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
    updateFormula(formula,context);
    QPalette p=mmlWidget->palette();
    p.setColor(QPalette::WindowText,QColor::fromRgb(0,0,255));
    mmlWidget->setPalette(p);
    mmlWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    menu=new QMenu(mmlWidget);
    copyAction=new QAction(tr("Copier"),menu);
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    toLatexAction=new QAction(tr("Copier vers LaTeX"),menu);
    toLatexAction->setIcon(QIcon(":/images/tex.png"));
    menu->addAction(copyAction);
    menu->addAction(toLatexAction);

    QHBoxLayout *layout=new QHBoxLayout(this);
    layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->addWidget(mmlWidget);
    setLayout(layout);
    connect(mmlWidget,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(displayMenu(QPoint)));
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));
    connect(toLatexAction,SIGNAL(triggered()),this,SLOT(copyToLaTeX()));
}
void FormulaWidget::copy(){
    QClipboard* clip=QApplication::clipboard();
    clip->setText(QString::fromStdString(giac::print(formula,context)));
}
void FormulaWidget::copyToLaTeX(){
    QClipboard* clip=QApplication::clipboard();
    clip->setText(QString::fromStdString(gen2tex(formula,context)));

}
void FormulaWidget::displayMenu(QPoint p){
    menu->popup(mapToGlobal(p));
}
void FormulaWidget::updateFormula(const QString  s){
    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(s, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {
        qWarning("MathML error: %s, Line: %d, Column: %d",
                 errorMsg.constData(), errorLine, errorColumn);
      }
      mmlWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
      mmlWidget->updateGeometry();
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
      mmlWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
      mmlWidget->updateGeometry();
}

void FormulaWidget::toXML(QDomElement& d){

}


GraphWidget::GraphWidget(giac::context * context,bool b){
        isInteractiveWidget=b;
        canvas=new Canvas2D(this,context);
        canvas->updatePixmap(true);
        canvas->repaint();
        initGui();
}

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
    canvas->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    QWidget* canvasWidget=new QWidget(this);
    QVBoxLayout* vbox=new QVBoxLayout(canvasWidget);
    if (isInteractiveWidget){
        createToolBar();
        vbox->addWidget(toolPanel);//,Qt::AlignLeft|Qt::AlignTop);
    }
    vbox->addWidget(canvas);//,1,Qt::AlignLeft|Qt::AlignTop);
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
    buttonPt=new QToolButton(this);
    buttonLine=new QToolButton(this);
    buttonCircle=new QToolButton(this);

    // Menu creation
    QMenu* menuPt=new QMenu(buttonPt);
    QMenu* menuLine=new QMenu(buttonLine);
    QMenu* menuCircle=new QMenu(buttonCircle);



    // Point Actions
    singlept=new QAction(tr("Point"),buttonPt);
    singlept->setData(canvas->SINGLEPT);
    singlept->setIcon(QIcon(":/images/point.png"));
    inter=new QAction(tr("Intersection entre deux objets"),buttonPt);
    inter->setIcon(QIcon(":/images/inter.png"));
    inter->setData(canvas->INTER);
    midpoint=new QAction(tr("Milieu ou centre"),buttonPt);
    midpoint->setIcon(QIcon(":/images/midpoint.png"));
    midpoint->setData(canvas->MIDPOINT);
    pointxy=new QAction(tr("Point repéré"),buttonPt);
    pointxy->setData(canvas->POINT_XY);
    pointxy->setIcon(QIcon(":/images/pointxy.png"));


    // Line Actions
    line=new QAction(tr("Droite"),buttonLine);
    line->setIcon(QIcon(":/images/line.png"));
    line->setData(canvas->LINE);
    segment=new QAction(tr("Segment"),buttonLine);
    segment->setIcon(QIcon(":/images/segment.png"));
    segment->setData(canvas->SEGMENT);
    halfline=new QAction(tr("Demie-droite"),buttonLine);
    halfline->setIcon(QIcon(":/images/halfline.png"));
    halfline->setData(canvas->HALFLINE);

    // Circle Actions
    circle2pt=new QAction(tr("Cercle (centre-point)"),buttonCircle);
    circle2pt->setIcon(QIcon(":/images/circle2pt.png"));
    circle2pt->setData(canvas->CIRCLE2PT);
    circleRadius=new QAction(tr("Cercle (centre-point)"),buttonCircle);
    circleRadius->setIcon(QIcon(":/images/circleRadius.png"));
    circleRadius->setData(canvas->CIRCLE_RADIUS);
    circle3pt=new QAction(tr("Cercle (3 points)"),buttonCircle);
    circle3pt->setIcon(QIcon(":/images/circle3pt.png"));
    circle3pt->setData(canvas->CIRCLE3PT);

    // Set default actions
    buttonPt->setProperty("myAction",canvas->SINGLEPT);
    buttonLine->setProperty("myAction",canvas->LINE);
    buttonCircle->setProperty("myAction",canvas->CIRCLE2PT);


    menuPt->addAction(singlept);
    menuPt->addAction(pointxy);
    menuPt->addAction(inter);
    menuPt->addAction(midpoint);
    menuPt->setStyle(new IconSize);

    menuLine->addAction(line);
    menuLine->addAction(segment);
    menuLine->addAction(halfline);
    menuLine->setStyle(new IconSize);

    menuCircle->addAction(circle2pt);
    menuCircle->addAction(circleRadius);
    menuCircle->addAction(circle3pt);
    menuCircle->setStyle(new IconSize);

    const QSize size(40,40);
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
    group->addButton(buttonPt);
    group->addButton(buttonLine);
    group->addButton(buttonCircle);

    toolPanel=new QWidget(this);
    toolPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    QHBoxLayout* hbox=new QHBoxLayout(toolPanel);
    hbox->addWidget(buttonPt,Qt::AlignLeft);
    hbox->addWidget(buttonLine,Qt::AlignLeft);
    hbox->addWidget(buttonCircle,Qt::AlignLeft);
    toolPanel->setLayout(hbox);

    connect(menuPt,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuLine,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(menuCircle,SIGNAL(triggered(QAction*)),this,SLOT(selectButtonIcon(QAction*)));
    connect(buttonPt,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonLine,SIGNAL(clicked()),this,SLOT(selectAction()));
    connect(buttonCircle,SIGNAL(clicked()),this,SLOT(selectAction()));


}
/** When the user select a button, it propagates action to the canvas
  *
  */
void GraphWidget::selectAction(){

    QToolButton* b=qobject_cast<QToolButton*>(sender());
    b->setChecked(true);
    canvas->setActionTool((Canvas2D::action)(b->property("myAction").toInt()));
}


/** Depending on the action selected by the user, changes the button icon and
  * propagates action to the canvas
  **/

void GraphWidget::selectButtonIcon(QAction * ac){
   QToolButton* b=qobject_cast<QToolButton*>(ac->parent());

   b->setChecked(true);

   b->setIcon(ac->icon());

   b->setProperty("myAction",ac->data().toInt());

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

void GraphWidget::updateAllCategories(){
    propPanel->updateAllCategories();
}
void GraphWidget::updateValueInDisplayPanel(){
    propPanel->updateValueInDisplayPanel();
}
void GraphWidget::selectInTree(MyItem * item){
    propPanel->selectInTree(item);
}
QList<MyItem*> GraphWidget::getTreeSelectedItems(){
    return propPanel->getTreeSelectedItems();
}
/**
  * clear all selected items in the tree
  */
void GraphWidget::clearSelection(){
    propPanel->clearSelection();
}
void Canvas2D::setActionTool(action a){
    currentActionTool=a;
    if (currentActionTool==POINT_XY){
        CoordsDialog* dialog=new CoordsDialog(this);
        if (dialog->exec()){
            findFreeVar(varPt);
            Command newCommand;
            newCommand.var=QString(varPt);
            QString s(varPt);
            s.append(":=point(");
            s.append(dialog->editX->text());
            s.append(",");
            s.append(dialog->editY->text());
            s.append(");");
            newCommand.command=s;
            newCommand.attributes=0;
            newCommand.isCustom=false;
            commands.append(newCommand);
            evaluationLevel=commands.size()-1;
            gen g(newCommand.command.toStdString(),context);
    //                    qDebug()<<QString::fromStdString(g.print(context));
            QList<MyItem*> v;
            addToVector(protecteval(g,1,context),v);
            v.at(0)->updateScreenCoords(true);
            pointItems.append(v.at(0));
            parent->addToTree(v.at(0));
            focusOwner=v.at(0);
            v.at(0)->setMovable(false);
            parent->updateAllCategories();
            parent->selectInTree(focusOwner);
            selectedItems.append(focusOwner);
            updatePixmap(false);
            repaint();
        }
        delete(dialog);

    }
    selectedItems.clear();
}

void Canvas2D::setBounds(const double & xMin, const double &xMax, const double & yMin, const double & yMax){
    xmin=xMin;
    xmax=xMax;
    ymin=yMin;
    ymax=yMax;
}
void Canvas2D::setXAxisLegend(const QString &s){
    xAxisLegend=s;
}
void Canvas2D::setYAxisLegend(const QString &s){
    yAxisLegend=s;
}
void Canvas2D::setXUnitSuffix(const QString &s){
    xUnitSuffix=s;
}
void Canvas2D::setYUnitSuffix(const QString &s){
    yUnitSuffix=s;
}
void Canvas2D::setXAxisTick(const double &d){
    xAxisTick=d;
}
void Canvas2D::setYAxisTick(const double &d){
    yAxisTick=d;
}
QString Canvas2D::getXAxisLegend() const{
    return xAxisLegend;
}
QString Canvas2D::getYAxisLegend() const{
    return yAxisLegend;
}
QString Canvas2D::getXUnitSuffix() const{
    return xUnitSuffix;
}
QString Canvas2D::getYUnitSuffix() const{
    return yUnitSuffix;
}

double Canvas2D::getXAxisTick() const{
    return xAxisTick;
}
double Canvas2D::getYAxisTick() const{
    return yAxisTick;
}
giac::context* Canvas2D::getContext() const{
    return context;
}


void Canvas2D::addToVector(const giac::gen &g,QList <MyItem*> & scene){
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

        /*
        *    Circle figures (arc, circle ...)
        *
        */
        if (point._SYMBptr->sommet==at_cercle){
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
         if (fill_polygon && *jt==*(jtend-1)){
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
     if ( (point.subtype==_GROUP__VECT) && (point._VECTptr->size()==2)){

           curve->setValue(giac::abs(save2-save,context));
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
QSize Canvas2D::sizeHint() const{
    return QSize(width(),height());
}

Canvas2D::Canvas2D(GraphWidget *g2d, giac::context * c){
    parent=g2d;
    context=c;
    ortho=false;
    selectionRight=false;
    selectionLeft=false;
    focusOwner=0;
    currentActionTool=SINGLEPT;
    varPt="A";
    varLine="a";
    evaluationLevel=-1;


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

    createMenuAction();
    setContextMenuPolicy(Qt::NoContextMenu);
}
void Canvas2D::createMenuAction(){
    zoomIn=new QAction(tr("Zoom avant"),this);

//   zoomIn->setShortcut(tr("Ctrl++"));
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

   QAction* title=new QAction(tr("Graphique"),this);
   QFont font;
   font.setWeight(QFont::Bold);
   title->setFont(font);


   // create contextual menu
    menuGeneral=new QMenu(this);
    menuGeneral->addAction(title);
    menuGeneral->addSeparator();
    menuGeneral->addAction(zoomIn);
    menuGeneral->addAction(zoomOut);
    menuGeneral->addAction(orthoAction);
    if (isInteractive()){
        menuGeneral->addSeparator();
        menuGeneral->addAction(sourceAction);
    }

}



void Canvas2D::createScene(const giac::gen & g){
    setFixedSize(Config::graph_width,Config::graph_width*3/4);

    // Find the largest and lowest x/y/z in objects (except lines/plans)
    std::vector<double> vx,vy,vz;

    bool ortho=giac::autoscaleg(g,vx,vy,vz,context);



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
    drawGrid(&painter);
    drawElements(filledItems,&painter,compute);
    if (show_axes(context)) drawAxes(&painter);
    drawElements(lineItems,&painter,compute);
    drawElements(pointItems,&painter,compute);
}


void Canvas2D::drawAxes(QPainter * painter){
    if ((xmin<0)& (xmax>0)){
        double a,b;

        painter->setPen(QPen(Qt::black,1, Qt::SolidLine ,Qt::RoundCap));
        double step=(ymax-ymin)/8;
        double tenPower=pow(10,floor(log10(step)));
        int first=floor(step/tenPower);


        if (step/tenPower-first>0.5) step=(first+1)*tenPower;
        else step=first*tenPower;

        for (int i=floor(ymin/step);i<=floor(ymax/step);i++){
            double grad=step*i;
            if (grad>ymin && grad<ymax){
                toScreenCoord(0,grad,a,b);
                QPointF p1=QPointF(a,b);
                QPointF p2=QPointF(a+3,b);
                painter->drawLine(p1,p2);
                QString s=QString::number((grad));
                if (i%2==0) {
                    //Problem of the origin
                    if ((ymin<0) & (ymax>0)){
                        //TODO

                    }
                    painter->drawText(QPointF(a+6,b+painter->fontMetrics().ascent()/2.0),s);

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
            painter->setBrush(QBrush(Qt::black,Qt::SolidPattern));
            painter->drawPath(p);



        }


    }
    if ((ymin<0) & (ymax>0)){
        double a,b;
        painter->setPen(QPen(Qt::black,1, Qt::SolidLine ,Qt::RoundCap));
        double step=(xmax-xmin)/8;
        double tenPower=pow(10,floor(log10(step)));
        int first=floor(step/tenPower);


        if (step/tenPower-first>0.5) step=(first+1)*tenPower;
        else step=first*tenPower;

        for (int i=floor(xmin/step);i<=floor(xmax/step);i++){
            double grad=step*i;
            if (grad>xmin && grad<xmax){
                toScreenCoord(grad,0,a,b);
                QPointF p1=QPointF(a,b);
                QPointF p2=QPointF(a,b-3);
                painter->drawLine(p1,p2);
                QString s=QString::number((grad));
                if (i%2==0) painter->drawText(QPointF(a-painter->fontMetrics().width(s)/2,b+6+painter->fontMetrics().height()/2),s);

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
            painter->setBrush(QBrush(Qt::black,Qt::SolidPattern));
            painter->drawPath(p);
        }
    }
}
void Canvas2D::drawGrid(QPainter * painter){


}
void Canvas2D::drawElements(QList<MyItem*> & v,QPainter* painter,const bool &compute){
//    painter->setClipping(true);
//    painter->setClipRect(20,20,width()-20,height()-20);
    if (v.isEmpty()) return;
    for(int i=0;i<v.size();++i){
        v.at(i)->updateScreenCoords(compute);
        v.at(i)->draw(painter);

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
    while (eval(g,1,context)!=g){
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
        if (v->at(i)->isUnderMouse(r)&& checkForValidAction(v->at(i))) {


            if (focusOwner!=v->at(i)){
                focusOwner=v->at(i);
                repaint();
            }

            return true;
        }

    }
    return false;

}
/**
 * @brief Canvas2D::checkForValidAction
 * @param item
 * @return true if this item can be selected for the the current action tool
 * (Eg: If selected action tool is "Segment", it is waiting only for points. Then, we don't highlight circle for example.
 */
bool Canvas2D::checkForValidAction(const MyItem * item){
    if (!parent->isInteractive()) return true;
    switch(currentActionTool){
    case  SINGLEPT:
        return true;
    case SEGMENT:
    case LINE:
    case HALFLINE:
    case CIRCLE2PT:
    case CIRCLE3PT:
    case CIRCLE_RADIUS:
        if (item->isPoint()) return true;
        else return false;
    default:
        return true;
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
    if (parent->isInteractive()&&focusOwner!=0&& selectionLeft){
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

//    repaint();

}

bool lessThan(const MyItem* a, const MyItem*b){
    return a->getLevel()<b->getLevel();
}
void Canvas2D::moveItem(MyItem* item,const QPointF &p){
    if (item->isPoint()){
//        int eval_save=evaluationLevel;
  //      evaluationLevel=item->getLevel();
        Command c=commands.at(item->getLevel());
        gen g(c.var.append(commandFreePoint(p,0)).toStdString(),context);
        QList<MyItem*> v;
        addToVector(protecteval(g,1,context),v);
    //    evaluationLevel=eval_save;
        item->updateValueFrom(v.at(0));
        delete v.at(0);

        if (item->hasChildren()){
            v.clear();
            refreshFromItem(item,v);
            qSort(v.begin(),v.end(),lessThan);

            for (int i=0;i<v.size();++i){
                //eval_save=evaluationLevel;
                int level=v.at(i)->getLevel();

                Command c=commands.at(level);
                gen g(c.command.toStdString(),context);

                QList<MyItem*> vv;
//                evaluationLevel=level;
                addToVector(protecteval(g,1,context),vv);

//                qDebug()<<QString::fromStdString(protecteval(g,1,context).print(context));
//                qDebug()<<vv.at(0)->getType();
//                evaluationLevel=eval_save;
                v.at(i)->updateValueFrom(vv.at(0));
                delete vv.at(0);


            }
        }
        updatePixmap(false);
        parent->updateValueInDisplayPanel();
        repaint();
    }
}


void Canvas2D::refreshFromItem(MyItem * item,QList<MyItem*>& list){
    QVector<MyItem*> v=item->getChildren();
    for (int i=0;i<v.size();++i){
        if (!list.contains(v.at(i))){
            list.append(v.at(i));
            if (v.at(i)->hasChildren()) refreshFromItem(v.at(i),list);
        }
    }
}

    //gen g(commands.at(level).toStdString(),context);
    //gen answer=protecteval(g,1,context);





void Canvas2D::mouseReleaseEvent(QMouseEvent *e){
    Qt::MouseButton b=e->button();
    if (b==Qt::RightButton&& selectionRight) {
                endSel=e->pos();
                setMouseTracking(true);
                QRect r(startSel,endSel);
                if (selectionRight&& std::abs(r.width())>10&& std::abs(r.height())>10){
                    zoom_In();
                }
                else     menuGeneral->popup(this->mapToGlobal(e->pos()));

                selectionRight=false;
    }
    // Left Button
    else if (b==Qt::LeftButton){
        selectionLeft=false;
        // An object is already highlighted
        if (focusOwner!=0){
            if (parent->isInteractive()){
                selectedItems<<focusOwner;
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
                if (!addNewPoint(e->posF())) parent->clearSelection();
                // Else, after this point is added on drawing screen, is selectedItem full?
                else if (checkForCompleteAction()){

                    executeMyAction();
                }
            }
            else  parent->clearSelection();
        }
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
        return (selectedItems.size()==2);
        case CIRCLE3PT:
        return (selectedItems.size()==3);
        case CIRCLE_RADIUS:
        return (selectedItems.size()==1);
    default:
        return false;
    }
}
void Canvas2D::addNewCircle(){
    findFreeVar(varLine);
    Command c;
    c.var=QString(varLine);
    c.attributes=0;
    c.command=QString(c.var);
    // CIRCLE RADIUS
    if (selectedItems.size()==1){
        RadiusDialog *dialog=new RadiusDialog(this);
        if (dialog->exec()){
            QString first(commands.at(selectedItems.at(0)->getLevel()).var);
            commandTwoArgs("circle",first,dialog->editRadius->text(),c.command);
        }
        else {
            selectedItems.clear();
            delete dialog;
            return;
        }
        delete dialog;
    }
    else if (selectedItems.size()==2){
        QString first(commands.at(selectedItems.at(0)->getLevel()).var);
        QString second(commands.at(selectedItems.at(1)->getLevel()).var);
        second.append("-");
        second.append(first);
        commandTwoArgs("circle",first,second,c.command);
    }
    else{
        QString first(commands.at(selectedItems.at(0)->getLevel()).var);
        QString second(commands.at(selectedItems.at(1)->getLevel()).var);
        QString third(commands.at(selectedItems.at(2)->getLevel()).var);
        c.command.append((":=circumcircle("));
        c.command.append(first);
        c.command.append(",");
        c.command.append(second);
        c.command.append(",");
        c.command.append(third);
        c.command.append(");");
    }
    c.isCustom=false;
    commands.append(c);
    evaluationLevel=commands.size()-1;
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;
    addToVector(protecteval(g,1,context),v);
    filledItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    for (int i=0;i<selectedItems.size();++i){
        selectedItems.at(i)->addChild(v.at(0));
    }
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(true);
    repaint();
}
void Canvas2D::addNewLine(const QString & type){
    findFreeVar(varLine);
    Command c;
    c.var=QString(varLine);
    c.attributes=0;
    c.command=QString(c.var);
    commandTwoArgs(type,(commands.at(selectedItems.at(0)->getLevel())).var,
                   (commands.at(selectedItems.at(1)->getLevel())).var,c.command);
    c.isCustom=false;
    commands.append(c);
    evaluationLevel=commands.size()-1;
    gen g(c.command.toStdString(),context);
    QList<MyItem*> v;

    addToVector(protecteval(g,1,context),v);
    selectedItems.at(0)->addChild(v.at(0));
    selectedItems.at(1)->addChild(v.at(0));
    lineItems.append(v.at(0));
    parent->addToTree(v.at(0));
    focusOwner=v.at(0);
    parent->updateAllCategories();
    parent->selectInTree(focusOwner);
    updatePixmap(true);
    repaint();
}
void Canvas2D::executeMyAction(){
    switch(currentActionTool){
        case SEGMENT:  addNewLine("segment");
        break;
        case LINE: addNewLine("line");
        break;
        case HALFLINE: addNewLine("half_line");
        break;
        case CIRCLE2PT:
        case CIRCLE3PT:
        case CIRCLE_RADIUS: addNewCircle();
        break;
    default:
        qDebug()<<"coucou";
    }

    selectedItems.clear();

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
/**
 * @brief Canvas2D::addNewPoint
 * @param p the current mouse position
 * @return true if the current tool is waiting for a point and adds this point,
 * returns false otherwise
 */
bool Canvas2D::addNewPoint(const QPointF & p){
    // application is currently waiting for a point selection
    if (currentActionTool==SINGLEPT||currentActionTool==SEGMENT||currentActionTool==HALFLINE||currentActionTool==LINE||currentActionTool==POINT_XY
            ||currentActionTool==CIRCLE2PT||currentActionTool==CIRCLE3PT||currentActionTool==CIRCLE_RADIUS){
        findFreeVar(varPt);
        Command newCommand;
        newCommand.var=QString(varPt);
        QString s(varPt);
        s.append(commandFreePoint(p,0));
        newCommand.command=s;
        newCommand.attributes=0;
        newCommand.isCustom=false;
        commands.append(newCommand);
        evaluationLevel=commands.size()-1;
        gen g(newCommand.command.toStdString(),context);
//                    qDebug()<<QString::fromStdString(g.print(context));
        QList<MyItem*> v;
        addToVector(protecteval(g,1,context),v);
        v.at(0)->updateScreenCoords(true);
        pointItems.append(v.at(0));
        parent->addToTree(v.at(0));
        focusOwner=v.at(0);
        parent->updateAllCategories();
        parent->selectInTree(focusOwner);
        selectedItems.append(focusOwner);
        updatePixmap(false);
        repaint();
        return true;
    }
    return false;

}

QString  Canvas2D::commandFreePoint(const QPointF & p,const int attributes){
    double a,b;
    toXY(p.x(),p.y(),a,b);
    QString newCommand(":=point([");
    newCommand.append(QString::number(a));
    newCommand.append(",");
    newCommand.append(QString::number(b));
    newCommand.append("],display=");
    newCommand.append(QString::number(attributes));
    newCommand.append(");");
    return newCommand;
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

    // draw selection
    if (selectionRight){
        QColor fill=QColor(50,0,255,50);
        painter.setPen(fill);
        QBrush brush(fill,Qt::SolidPattern);
        painter.fillRect(QRect(startSel,endSel),brush);
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
}
void Canvas2D::resizeEvent(QResizeEvent * ev){
    xmax=xmin+(ev->size().width()-40)/xunit;
    ymin=ymax-(ev->size().height()-40)/yunit;

    updatePixmap(false);
    repaint();

}
void  Canvas2D::toXML(){
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
    nodeList=new QTreeWidgetItem;

    nodeAxis->setText(0,tr("Axes/grille"));
    nodePoint->setText(0,tr("Point"));
    nodeCurve->setText(0,tr("Courbe"));
    nodeVector->setText(0,tr("Vecteur"));
    nodeLine->setText(0,tr("Droite"));
    nodeSegment->setText(0,tr("Segment"));
    nodeHalfLine->setText(0,tr("Demie-droite"));
    nodePolygon->setText(0,tr("Polygône"));
    nodeCircle->setText(0,tr("Cercle"));
    nodeList->setText(0,tr("Liste"));


    tree->addTopLevelItem(nodeAxis);
    tree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    if (!parent->isInteractive()){
        hbox=new QHBoxLayout;
    }
    else{
        hbox=new QVBoxLayout;
    }
    hbox->addWidget(tree);
    hbox->addWidget(displayPanel,0,Qt::AlignLeft|Qt::AlignTop);
    hbox->addWidget(axisGridPanel,0,Qt::AlignLeft|Qt::AlignTop);
    hbox->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(hbox);
    connect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));
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
    else if(item->isCircle()){
        nodeCircle->addChild(treeItem);

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
           parent->setFocusOwner(myItem);
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
            tree->removeItemWidget(node,0);
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
    if (updateCategory(nodeHalfLine,id)) id++;
    if (updateCategory(nodeList,id)) id++;
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
    listItems=new QList<MyItem*>;
}
void DisplayProperties::updateDisplayPanel(QList<MyItem *> * l){
    setVisible(true);
    delete listItems;
    listItems=l;
    if (l->count()>1) valuePanel->setVisible(false);
   else if (!QString::fromStdString(giac::print(listItems->at(0)->getValue(),parent->getContext())).trimmed().isEmpty()){
        valuePanel->setValue(listItems->at(0)->getDisplayValue());
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
    if (!listItems->isEmpty())
        valuePanel->setValue(listItems->at(0)->getDisplayValue());
}
void DisplayProperties::initGui(){
    QWidget * generalPanel=new QWidget;
    vLayoutGeneral=new QVBoxLayout;
    vLayoutGeneral->setSizeConstraint(QLayout::SetFixedSize);
    valuePanel=new GenValuePanel(parent);
    valuePanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    displayObjectPanel=new DisplayObjectPanel(this);
    displayObjectPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    legendPanel=new LegendPanel(this);
    legendPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    vLayoutGeneral->addWidget(valuePanel);
    vLayoutGeneral->addWidget(legendPanel);
    vLayoutGeneral->addWidget(displayObjectPanel);
    generalPanel->setLayout(vLayoutGeneral);


    QWidget * attributesPanel=new QWidget;
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


}


void DisplayProperties::updateCanvas(){

    parent->updatePixmap(false);
    parent->repaint();
}
QList<MyItem*>* DisplayProperties::getListItems() const{
    return listItems;
}


ColorPanel::ColorPanel(DisplayProperties *p):QWidget(p){
    parent=p;
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
        for (int i=0;i<parent->getListItems()->count();++i){
            QColor c(color);
            c.setAlpha(parent->getListItems()->at(i)->getColor().alpha());
            parent->getListItems()->at(i)->setColor(c);
        }
        parent->updateCanvas();
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
TypePointPanel::TypePointPanel(const int t, DisplayProperties * p):QWidget(p){
    type=t;
    parent=p;
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

TypeLinePanel::TypeLinePanel(const int t, DisplayProperties * p):QWidget(p){
    type=t;
    parent=p;
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

    connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));
}


void TypeLinePanel::updateCanvas(int c){
    for (int i=0;i<parent->getListItems()->count();++i){
        parent->getListItems()->at(i)->setStyle(c);
    }
    parent->updateCanvas();

}
void TypeLinePanel::setStyle(const int &c){
    disconnect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));
    combo->setCurrentIndex(c);
    connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(updateCanvas(int)));

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
    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(layout);
}
void GenValuePanel::setValue(const QString  s){
      formulaWidget->updateFormula(s);
      formulaWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
}

AxisGridPanel::AxisGridPanel(Canvas2D * p):QTabWidget(p){
    parent=p;
    initGui();
}
void AxisGridPanel::initGui(){
    xPanel=new AxisPanel(this);
    yPanel=new AxisPanel(this);
    addTab(xPanel,tr("Axe (Ox)"));
    addTab(yPanel,tr("Axe (Oy)"));
}

void AxisGridPanel::initValue(){
    xPanel->initValue(parent->getXAxisLegend(),parent->getXUnitSuffix(),parent->getXmin(),parent->getXmax(),giac::show_axes(parent->getContext()));
    yPanel->initValue(parent->getYAxisLegend(),parent->getYUnitSuffix(),parent->getYmin(),parent->getYmax(),giac::show_axes(parent->getContext()));
}


void AxisGridPanel::updateCanvas(const bool &b){
     parent->setXAxisTick(xPanel->editDistance->text().toDouble());
     parent->setYAxisTick(yPanel->editDistance->text().toDouble());
     parent->setXAxisLegend(xPanel->editLabel->text());
     parent->setYAxisLegend(yPanel->editLabel->text());
     parent->setXUnitSuffix(xPanel->editUnitLabel->text());
     parent->setYUnitSuffix(yPanel->editUnitLabel->text());
     if (xPanel->editMin->text().toDouble()<xPanel->editMax->text().toDouble()&&yPanel->editMin->text().toDouble()<yPanel->editMax->text().toDouble())
         parent->setBounds(xPanel->editMin->text().toDouble(),xPanel->editMax->text().toDouble(),yPanel->editMin->text().toDouble(),yPanel->editMax->text().toDouble());
     giac::show_axes(xPanel->showAxis->isChecked()&& yPanel->showAxis->isChecked(),parent->getContext());
     parent->setXYUnit();
     parent->updatePixmap(b);
     parent->repaint();
}

AxisPanel::AxisPanel(AxisGridPanel* p):QWidget(p){
    parent=p;
    initGui();

}

void AxisPanel::initGui(){
    QGridLayout *grid=new QGridLayout(this);

    showAxis=new QCheckBox(tr("Afficher l'axe"),this);
    QLabel * labelMin=new QLabel(tr("Minimum:"),this);
    editMin=new QLineEdit(this);
    editMin->setValidator(new QDoubleValidator);
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

    connect(showAxis,SIGNAL(clicked()),this,SLOT(updateCanvas()));
    connect(editMin,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editMax,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editUnitLabel,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
    connect(editDistance,SIGNAL(editingFinished()),this,SLOT(updateCanvas()));
}
void AxisPanel::updateCanvas(){
    QLineEdit* edit=qobject_cast<QLineEdit*>(sender());
            bool b=false;
    if (editMin==edit|| editMax==edit) b=true;
    parent->updateCanvas(b);
}

void AxisPanel::initValue(const QString & axisLabel, const QString &suffix, const double & min, const double &max, const bool & b){
    editLabel->setText(axisLabel);
    editUnitLabel->setText(suffix);
    editMin->setText(QString::number(min));
    editMax->setText(QString::number(max));
    showAxis->setChecked(b);
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
    for (int i=0;i<commands.size();++i){
        list.append(commands.at(i).command);
    }

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
    delete listWidget->item(id);
    parent->getCommands().removeAt(id);
    parent->updatePixmap(true);
    parent->repaint();

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
    setLayout(grid);

    connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

}
RadiusDialog::RadiusDialog(Canvas2D* p):QDialog (p){
    initGui();
}


void RadiusDialog::initGui(){
    QGridLayout* grid=new QGridLayout(this);
    QLabel* labelRadius=new QLabel(tr("Rayon:"),this);
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

    connect(ok,SIGNAL(clicked()),this,SLOT(accept()));
    connect(cancel,SIGNAL(clicked()),this,SLOT(reject()));

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
