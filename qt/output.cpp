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
#include <QSpinBox>
#include <QSlider>
#include <QComboBox>
#include <QColorDialog>
#include <QPushButton>
#include "gui/FormalLine.h"
#include "config.h"
#include "gui/qtmmlwidget.h"
#include "output.h"

using namespace giac;

OutputWidget::OutputWidget(QWidget*):QWidget(){

}
void OutputWidget::setLine(Line* l){
    line=l;
}

FormulaWidget::FormulaWidget(QtMmlWidget *mml){
    QHBoxLayout *layout=new QHBoxLayout;
    layout->addWidget(mml);
    setLayout(layout);
}

void FormulaWidget::toXML(QDomElement& d){

}

GraphWidget::GraphWidget(const giac::gen & g, giac::context * context){

          canvas=new Canvas2D(this,g,context);


       initGui();


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
    QHBoxLayout *hbox=new QHBoxLayout;
    propPanel=new PanelProperties(canvas);
    canvas->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    hbox->addWidget(canvas,0,Qt::AlignLeft|Qt::AlignTop);

    hbox->addWidget(propPanel);
    hbox->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(hbox);
}
QList<MyItem*> GraphWidget::getTreeSelectedItems(){
    return propPanel->getTreeSelectedItems();

}
void GraphWidget::clearSelection(){
    propPanel->clearSelection();
}

void Canvas2D::createScene(const giac::gen &g,giac::context* context){
    if (g.type==giac::_VECT){
      giac::vecteur & v =*g._VECTptr;
      const_iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;++it){
            createScene(*it,context);
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
            circle->setAttributes(ensemble_attributs);
            circle->setLegend(QString::fromStdString(the_legend));
            lineItems.append(circle);





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
              filledItems.append(pix);

          }
          else{
              int delta_i=v[0].val,delta_j=v[1].val;
              Pixel* pix=new Pixel(QPointF(delta_i,delta_j),this);
              pix->setAttributes(v[2].val);
              filledItems.append(pix);

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
                    pointItems.append(item);
                }
              }
            }

        }
      } // end point.type==_SYMB
      if (point.type!=_VECT){ // single point
          gen e,f0,f1;

          evalfdouble2reim(point,e,f0,f1,context);

          if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
              Point* pt=new Point(f0._DOUBLE_val,f1._DOUBLE_val,this);
               pt->setAttributes(ensemble_attributs);
               pt->setLegend(QString::fromStdString(the_legend));
               pointItems.append(pt);
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

//      i0save=i0;
//      j0save=j0;
      ++jt;
      if (jt==jtend){
       // if (i0>0 && i0<width && j0>0 && j0<myw)
            Point* pt=new Point(i0,j0,this);
            pt->setAttributes(ensemble_attributs);
            pt->setLegend(QString::fromStdString(the_legend));
            pointItems.append(pt);
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

               (QPointF(i0,j0));
           }
          endPoint=QPointF(i0,j0);
          if (point.subtype==_LINE__VECT ) {
              LineItem* line=new LineItem(startPoint,endPoint,this);
              line->setAttributes(ensemble_attributs);
              line->setLegend(QString::fromStdString(the_legend));
              lineItems.append(line);
          }
          else {
              HalfLineItem* line=new HalfLineItem(startPoint,endPoint,this);
              line->setAttributes(ensemble_attributs);
              line->setLegend(QString::fromStdString(the_legend));
              lineItems.append(line);
            }

        return;
      }




      // rest of the path

      QPainterPath path;
      if (!seghalfline) {
          path.moveTo(i0,j0);
      }


      for (;;){
  //      Mon_image.findij(*jt,x_scale,y_scale,i1,j1,context);
          // segment
              gen e,f0,f1;
              evalfdouble2reim(*jt,e,f0,f1,context);
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
     if ( (point.subtype==_GROUP__VECT) && (point._VECTptr->size()==2))
        ; // no legend for segment
      else {
          if (!hidden_name){}
      //    draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
      }

      Curve *curve=new Curve(path,this);
      curve->setAttributes(ensemble_attributs);
      curve->setLegend(QString::fromStdString(the_legend));
      if (point.subtype==_VECTOR__VECT){
              curve->setVector(true);
        }
      if (!isCurve&&path.elementCount()!=2){
          curve->setPolygon(true);
          curve->setFillable(fillable);
      }
      lineItems.append(curve);
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

QVector<MyItem*>* Canvas2D::getPointItem() {
    return &pointItems;

}
QVector<MyItem*>* Canvas2D::getFilledItem(){
    return &filledItems;

}

QVector<MyItem*>* Canvas2D::getLineItem(){
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



Canvas2D::Canvas2D(GraphWidget *g2d,const  giac::gen &g, giac::context*context){
    selection=false;
    focusOwner=0;
    parent=g2d;
    setFixedSize(Config::graph_width,Config::graph_width*3/4);
    setMouseTracking(true);
    // Find the largest and lowest x/y/z in objects (except lines/plans)
       std::vector<double> vx,vy,vz;
       bool ortho=giac::autoscaleg(g,vx,vy,vz,context);
       giac::autoscaleminmax(vx,xmin,xmax);
       giac::autoscaleminmax(vy,ymin,ymax);

       setXYUnit();
       if (std::isnan(xunit)||std::isnan(yunit)|| (!std::isfinite(xunit)) || (!std::isfinite(yunit))){
        xmin=-5;ymin=-5;xmax=5;ymax=5;
         setXYUnit();
       }
  //     qDebug()<<xmin<<xmax<<ymin<<ymax<<xunit<<yunit<<width()<<height();

      createScene(g,context);




    setBackgroundRole(QPalette::Light);
   setAutoFillBackground(true);

   zoomIn=new QAction(tr("Zoom avant"),this);
//   zoomIn->setShortcut(tr("Ctrl++"));
   //newAction->setIcon(QIcon(":/images/document-new.png"));
   connect(zoomIn,SIGNAL(triggered()),this,SLOT(zoom_In()));


   zoomOut=new QAction(tr("Zoom arrière"),this);
 //   zoomOut->setShortcut(tr("Ctrl+-"));
    connect(zoomOut,SIGNAL(triggered()),this,SLOT(zoom_Out()));

    orthoAction=new QAction(tr("Orthonormé"),this);
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

    updatePixmap(true);

   // Contextual menu
   setContextMenuPolicy(Qt::NoContextMenu);
}


void Canvas2D::zoom_In(){

    if (selection){
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
    selection=false;
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
    selection=false;
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
//void GraphWidget::zoom_Factor(const int &){}


//void Canvas2D::displayContextMenu(const QPoint &pos){
//
//}
void Canvas2D::updatePixmap(bool compute){
    pixmap=QPixmap(size());
    pixmap.fill(this,0,0);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing,true);
    drawGrid(&painter);
    drawAxes(&painter);
    drawElements(filledItems,&painter,compute);
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
void Canvas2D::drawElements(QVector<MyItem*> & v,QPainter* painter,const bool compute){
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

bool Canvas2D::checkUnderMouse(QVector<MyItem*>* v, const QPointF & p){
    QRectF r(p.x(),p.y(),5,5);
    r.adjust(-2.5,-2.5,-2.5,-2.5);

    for(int i=0;i<v->size();i++){
        if (v->at(i)->isUnderMouse(r)) {
            if (focusOwner!=v->at(i)){
                focusOwner=v->at(i);
                repaint();
            }

            return true;
        }

    }
    return false;

}
void Canvas2D::mouseMoveEvent(QMouseEvent *e){
    if (selection&&!hasMouseTracking()) {
        endSel=e->pos();
        repaint();
        return;
    }
    selection=false;

    QPointF pos=e->posF();
    // Detects objects under mouse
    bool objectUnderMouse=checkUnderMouse(&pointItems,pos) || checkUnderMouse(&lineItems,pos) ||   checkUnderMouse(&filledItems,pos);
    // Case: No object
    if (!objectUnderMouse){
        if (focusOwner!=0){
            focusOwner=0;
            repaint();
        }
    }
    else repaint();
}


void Canvas2D::mouseReleaseEvent(QMouseEvent *e){
    Qt::MouseButton b=e->button();
    if (b==Qt::RightButton&& selection) {
                endSel=e->pos();
                setMouseTracking(true);
                QRect r(startSel,endSel);
                if (selection&& std::abs(r.width())>10&& std::abs(r.height())>10){
                    zoom_In();
                }
                else     menuGeneral->popup(this->mapToGlobal(e->pos()));

                selection=false;
    }
    else if (b==Qt::LeftButton){
        if (focusOwner!=0){
            focusOwner->getTreeItem()->treeWidget()->collapseAll();
            focusOwner->getTreeItem()->treeWidget()->clearSelection();
            focusOwner->getTreeItem()->setSelected(true);
            focusOwner->getTreeItem()->parent()->setExpanded(true);


        }
        else {
            parent->clearSelection();
        }
    }
}
void Canvas2D::mousePressEvent(QMouseEvent *e){
    Qt::MouseButton b=e->button();
    if (b==Qt::RightButton){
            setMouseTracking(false);
            selection=true;
            startSel=e->pos();


    }


}
void Canvas2D::paintEvent(QPaintEvent * ){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.drawPixmap(0,0,pixmap);

    // draw selection
    if (selection){
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

PanelProperties::PanelProperties(Canvas2D* c){
    parent=c;
    initGui();
}
void PanelProperties::initGui(){
    hbox=new QHBoxLayout;
    displayPanel=new DisplayProperties(parent);
    displayPanel->hide();
    tree=new QTreeWidget;
    tree->setColumnCount(1);
    tree->headerItem()->setTextAlignment(0,Qt::AlignCenter);
    tree->headerItem()->setText(0,tr("Objects"));
    tree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree->clearSelection();

    nodePoint=new QTreeWidgetItem();
    nodeCurve=new QTreeWidgetItem();
    nodeVector=new QTreeWidgetItem();
    nodeLine=new QTreeWidgetItem();
    nodeSegment=new QTreeWidgetItem();
    nodeHalfLine=new QTreeWidgetItem();
    nodeCircle=new QTreeWidgetItem();
    nodePolygon=new QTreeWidgetItem();

    nodePoint->setText(0,tr("Point"));
    nodeCurve->setText(0,tr("Courbe"));
    nodeVector->setText(0,tr("Vecteur"));
    nodeLine->setText(0,tr("Droite"));
    nodeSegment->setText(0,tr("Segment"));
    nodeHalfLine->setText(0,tr("Demie-droite"));
    nodePolygon->setText(0,tr("Polygône"));
    nodeCircle->setText(0,tr("Cercle"));


    // Fill the treeWidget
    fillTree(parent->getPointItem());
    fillTree(parent->getLineItem());
    fillTree(parent->getFilledItem());

    QList<QTreeWidgetItem*> topLevel;

    if (nodePoint->childCount()!=0) topLevel.append(nodePoint);
    if (nodeLine->childCount()!=0)  topLevel.append(nodeLine);
    if (nodeSegment->childCount()!=0)  topLevel.append(nodeSegment);
    if (nodeCircle->childCount()!=0)  topLevel.append(nodeCircle);
    if (nodeCurve->childCount()!=0) topLevel.append(nodeCurve);
    if (nodePolygon->childCount()!=0) topLevel.append(nodePolygon);
    if (nodeVector->childCount()!=0) topLevel.append(nodeVector);
    if (nodeHalfLine->childCount()!=0)  topLevel.append(nodeHalfLine);

    tree->addTopLevelItems(topLevel);
    hbox->addWidget(tree);
    tree->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    hbox->addWidget(displayPanel,0,Qt::AlignLeft|Qt::AlignTop);
    hbox->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(hbox);
    connect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));
}
void PanelProperties::fillTree(QVector<MyItem*>* v){

    for (int i=0;i<v->size();++i){
        MyItem* item=v->at(i);
        if (item->isPixel()) continue;

        if (item->isPoint()){
           nodePoint->addChild(item->getTreeItem());

        }
        else if(item->isLine()){
            nodeLine->addChild(item->getTreeItem());

        }
        else if(item->isHalfLine()){
            nodeHalfLine->addChild(item->getTreeItem());
        }
        else if(item->isCurve()){
            Curve* c= dynamic_cast<Curve*>(item);
            if (c->isVector())
                nodeVector->addChild(item->getTreeItem());
            else if (c->isSegment())
                nodeSegment->addChild(item->getTreeItem());
            else if (c->isPolygon())
                nodePolygon->addChild(item->getTreeItem());
            else
                nodeCurve->addChild(item->getTreeItem());
        }
        else if(item->isCircle()){
            nodeCircle->addChild(item->getTreeItem());

        }
        nodeLinks.insert(item->getTreeItem(),item);
        QString legend=item->getLegend();
        if (legend.trimmed().isEmpty()){
            item->getTreeItem()->setText(0,item->getType().append(" ").append(QString::number(item->getTreeItem()->parent()->childCount())));

        }
        else item->getTreeItem()->setText(0,legend);

    }
}
void PanelProperties::updateTree(){
   QList<QTreeWidgetItem*> list=tree->selectedItems();
   if (list.isEmpty()) return;

   disconnect(tree,SIGNAL(itemSelectionChanged()),this,SLOT(updateTree()));
   QList<MyItem*>* listItems=new QList<MyItem*>();
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
   displayPanel->updateDisplayPanel(listItems);
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
void PanelProperties::clearSelection(){
    if (!tree->selectedItems().isEmpty()) {
        tree->clearSelection();
        parent->repaint();
        displayPanel->hide();
    }
}
DisplayProperties::DisplayProperties(Canvas2D *canvas){
    parent=canvas;
    initGui();
}
void DisplayProperties::updateDisplayPanel(QList<MyItem *> * l){
    setVisible(true);
    listItems=l;
    colorPanel->setColor(listItems->at(0)->getColor());
    legendPanel->setChecked(listItems->at(0)->legendVisible());
    if (l->count()>1){
        legendPanel->setLegend(false);
    }
    else {
        legendPanel->setLegend(true,listItems->at(0)->getLegend());
    }
    bool b=checkForOnlyPoints();
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
    else if (checkForOnlyLines()) {
        typePointPanel->hide();
        typeLinePanel->setStyle(listItems->at(0)->getStyle());
        typeLinePanel->setVisible(true);
    }
    else{
        typeLinePanel->hide();
        typePointPanel->hide();
    }
    if (checkForOnlyFillables()) {
        if (listItems->at(0)->isFilled())
        alphaFillPanel->setValue(8-listItems->at(0)->getColor().alpha()/36);
        else alphaFillPanel->setValue(8);
        alphaFillPanel->setVisible(true);

    }
    else alphaFillPanel->setVisible(false);


}
void DisplayProperties::initGui(){
    vLayout=new QVBoxLayout;
    vLayout->setSizeConstraint(QLayout::SetFixedSize);
    colorPanel=new ColorPanel(this);
    colorPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    legendPanel=new LegendPanel(this);
    legendPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    widthPanel=new WidthPanel(this,tr("Épaisseur (1-8):"));
    widthPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    typePointPanel=new TypePointPanel(2,this);
    typePointPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    typeLinePanel=new TypeLinePanel(2,this);
    typeLinePanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    alphaFillPanel=new AlphaFillPanel(this,tr("Transparence (0%-100%):"));
    alphaFillPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    vLayout->addWidget(colorPanel);
    vLayout->addWidget(legendPanel);
    vLayout->addWidget(widthPanel);
    vLayout->addWidget(typePointPanel);
    vLayout->addWidget(typeLinePanel);
    vLayout->addWidget(alphaFillPanel);
    setLayout(vLayout);
}

bool DisplayProperties::checkForOnlyPoints() const{

    for (int i=0;i<listItems->size();++i){
        if (!listItems->at(i)->isPoint()) return false;
    }
    return true;

}
bool DisplayProperties::checkForOnlyFillables() const{
    for (int i=0;i<listItems->size();++i){
        if (!listItems->at(i)->isFillable()) return false;
    }
    return true;
}


bool DisplayProperties::checkForOnlyLines() const{

    for (int i=0;i<listItems->size();++i){
        if (listItems->at(i)->isPoint()) return false;
    }
    return true;

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

