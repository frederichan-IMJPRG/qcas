#include <QHBoxLayout>
#include <QPainter>
#include <QPointF>
#include <giac/giac.h>
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

double GraphWidget::find_tick(double dx){
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
QSize GraphWidget::sizeHint() const{
    return QSize(width(),height());
}

GraphWidget::GraphWidget(const giac::gen & g,giac::context* context){

    // Find the largest and lowest x/y/z in objects (except lines/plans)
   std::vector<double> vx,vy,vz;
   bool ortho=giac::autoscaleg(g,vx,vy,vz,context);
   giac::autoscaleminmax(vx,xmin,xmax);
   giac::autoscaleminmax(vy,ymin,ymax);
   qDebug()<<"unités"<<xmin<<xmax<<ymin<<ymax;
   zoom=1;
   setFixedSize(Config::graph_width,Config::graph_width*3/4);
   setXYUnit();

   createScene(g,context);

   /*   QPalette p=palette();
   p.setColor(QPalette::Window,Qt::white);
   setPalette(p);
*/
   setBackgroundRole(QPalette::Light);
   setAutoFillBackground(true);
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
void GraphWidget::createScene(const giac::gen &g,giac::context* context){
    if (g.type==_VECT){
      vecteur & v =*g._VECTptr;
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
      hidden_name=ensemble_attributs<0;
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
      if ( (f[0].type==_SYMB) && (f[0]._SYMBptr->sommet==at_curve) && (f[0]._SYMBptr->feuille.type==_VECT) && (f[0]._SYMBptr->feuille._VECTptr->size()) ){
        // Mon_image.show_mouse_on_object=false;
        point=f[0]._SYMBptr->feuille._VECTptr->back();
      }
      if (is_undef(point))
        return;
    point.dbgprint();
    qDebug()<<point.type;
      /* TODO A comprendre
     if ( equalposcomp(Mon_image.selected,plot_i))
        fl_color(FL_BLUE);
      else
       xcas_color(couleur);
      fl_line_style(type_line,width+1,0);*/
      if (point.type==_SYMB) {
        if (point._SYMBptr->sommet==at_hyperplan || point._SYMBptr->sommet==at_hypersphere)
          return;
        if (point._SYMBptr->sommet==at_cercle){
          vecteur v=*point._SYMBptr->feuille._VECTptr;
          gen diametre=remove_at_pnt(v[0]);

            qDebug()<<"loic";
          gen e1=diametre._VECTptr->front().evalf_double(1,context);
          gen e2=diametre._VECTptr->back().evalf_double(1,context);
          gen centre=rdiv(e1+e2,2.0);
          gen e12=e2-e1;
          double ex=evalf_double(re(e12,context),1,context)._DOUBLE_val,ey=evalf_double(im(e12,context),1,context)._DOUBLE_val;
        // TODO A compléter

          /*          Mon_image.findij(centre,x_scale,y_scale,i0,j0,context);
          gen diam=std::sqrt(ex*ex+ey*ey);
          gen angle=std::atan2(ey,ex);
          gen a1=v[1].evalf_double(1,context),a2=v[2].evalf_double(1,context);
          if ( (diam.type==_DOUBLE_) && (a1.type==_DOUBLE_) && (a2.type==_DOUBLE_) ){
            i1=diam._DOUBLE_val*x_scale/2.0;
            j1=diam._DOUBLE_val*y_scale/2.0;
            double a1d=a1._DOUBLE_val,a2d=a2._DOUBLE_val,angled=angle._DOUBLE_val;
            bool changer_sens=a1d>a2d;
            if (changer_sens){
              double tmp=a1d;
              a1d=a2d;
              a2d=tmp;
            }
            if (fill_polygon)
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
            }
            // Label a few degrees from the start angle,
            // FIXME should use labelpos
            i0=i0+i1*std::cos(angled+a1d+0.3);
            j0=j0-j1*std::sin(angled+a1d+0.3);
            if (!hidden_name)
              draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
            return;
          }
          */
        } // end circle

        else if (point._SYMBptr->sommet==at_pixon){
/*          // pixon (i,j,color)
          if (point._SYMBptr->feuille.type!=_VECT)
            return;
          vecteur &v=*point._SYMBptr->feuille._VECTptr;
          if (v.size()<3 || v[0].type!=_INT_ || v[1].type!=_INT_ || v[2].type!=_INT_)
            return;
          int delta_i=v[0].val,delta_j=v[1].val;
          xcas_color(v[2].val);
#ifdef IPAQ
          if (delta_i>0 && delta_i<mxw && delta_j>0 && delta_j<myw)
            check_fl_point(+delta_i,deltay+delta_j,clip_x,clip_y,clip_w,clip_h,0,0);
#else
          delta_i *= 2;
          delta_j *= 2;
          if (delta_i>0 && delta_i<mxw && delta_j>0 && delta_j<myw){
            check_fl_point(deltax+delta_i,deltay+delta_j,clip_x,clip_y,clip_w,clip_h,0,0);
            check_fl_point(deltax+delta_i,deltay+delta_j+1,clip_x,clip_y,clip_w,clip_h,0,0);
            check_fl_point(deltax+delta_i+1,deltay+delta_j,clip_x,clip_y,clip_w,clip_h,0,0);
            check_fl_point(deltax+delta_i+1,deltay+delta_j+1,clip_x,clip_y,clip_w,clip_h,0,0);
          }
#endif
          return;*/
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

        else if (point._SYMBptr->sommet==at_legende){
           /*
          gen & f=point._SYMBptr->feuille;
          if (f.type==_VECT && f._VECTptr->size()==3){
            vecteur & fv=*f._VECTptr;
            if (fv[0].type==_VECT && fv[0]._VECTptr->size()>=2 && fv[1].type==_STRNG && fv[2].type==_INT_){
              vecteur & fvv=*fv[0]._VECTptr;
              if (fvv[0].type==_INT_ && fvv[1].type==_INT_){
                fl_font(FL_HELVETICA,Mon_image.labelsize());
                xcas_color(fv[2].val);
                int dx=0,dy=0;
                string legendes(*fv[1]._STRNGptr);
                find_dxdy(legendes,labelpos,Mon_image.labelsize(),dx,dy);
                fl_draw(legendes.c_str(),deltax+fvv[0].val+dx,deltay+fvv[1].val+dy);
              }
            }
          }*/
        }
      } // end point.type==_SYMB
      if (point.type!=_VECT){ // single point
          gen e,f0,f1;
          evalfdouble2reim(point,e,f0,f1,context);
          if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
               Point* pt=new Point(f0._DOUBLE_val,f1._DOUBLE_val,this);

               pt->setPointWidth(epaisseur_point);
               pt->setPointStyle(type_point);
               pt->setColor(couleur);
               pt->setLabelPos(labelpos);
               pt->setVisible(hidden_name);
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


//        Mon_image.findij(*jt,x_scale,y_scale,i0,j0,context);

        /*   if (fill_polygon && *jt==*(jtend-1)){
        const_iterateur jtsave=jt;
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
        fl_color(epaisseur_point-2+(type_point<<3));
      }*/

//      i0save=i0;
//      j0save=j0;
      ++jt;
      if (jt==jtend){
       // if (i0>0 && i0<width && j0>0 && j0<myw)
            Point* pt=new Point(i0,j0,this);
            pt->setPointWidth(epaisseur_point);
            pt->setPointStyle(type_point);
            pt->setColor(couleur);
            pt->setLabelPos(labelpos);
            pt->setVisible(hidden_name);
            pointItems.append(pt);
 //       if (!hidden_name)
//          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
        return;
      }
      bool seghalfline=(point.subtype==_LINE__VECT || point.subtype==_HALFLINE__VECT) && (point._VECTptr->size()==2);

      // rest of the path
      QPainterPath path;
      if (!seghalfline) {
          path.moveTo(i0,j0);
      }

      for (;;){
  //      Mon_image.findij(*jt,x_scale,y_scale,i1,j1,context);
          // segment
          if (!seghalfline){
              gen e,f0,f1;

              evalfdouble2reim(*jt,e,f0,f1,context);
              if ((f0.type==_DOUBLE_) && (f1.type==_DOUBLE_)){
                   i0=f0._DOUBLE_val;
                   j0=f1._DOUBLE_val;

                   path.lineTo(QPointF(i0,j0));
               }
          if (point.subtype==_VECTOR__VECT){
//            double dx=i0-i1,dy=j0-j1;
//            petite_fleche(i1,j1,dx,dy,deltax,deltay,width);
          }
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
      // check for a segment/halfline/line
/*      if ( seghalfline){
        double deltai=i1-i0save,adeltai=std::abs(deltai);
        double deltaj=j1-j0save,adeltaj=std::abs(deltaj);
        if (point.subtype==_LINE__VECT){
          if (deltai==0)
            fl_line(int(deltax+i1),int(deltay),int(deltax+i1),int(deltay+clip_h));
          else {
            if (deltaj==0)
              fl_line(int(deltax),int(deltay+j1),int(deltax+clip_w),int(deltay+j1));
            else {
              // Find the intersections with the 4 rectangle segments
              // Horizontal x=0 or w =i1+t*deltai: y=j1+t*deltaj
              vector< complex<double> > pts;
              double y0=j1-i1/deltai*deltaj;
              if (y0>=0 && y0<=clip_h)
                pts.push_back(complex<double>(0.0,y0));
              double yw=j1+(clip_w-i1)/deltai*deltaj;
              if (yw>=0 && yw<=clip_h)
                pts.push_back(complex<double>(clip_w,yw));
              // Vertical y=0 or h=j1+t*deltaj, x=i1+t*deltai
              double x0=i1-j1/deltaj*deltai;
              if (x0>0 && x0<=clip_w)
                pts.push_back(complex<double>(x0,0.0));
              double xh=i1+(clip_h-j1)/deltaj*deltai;
              if (xh>=0 && xh<=clip_w)
                pts.push_back(complex<double>(xh,clip_h));
              if (pts.size()>=2)
                fl_line(deltax+int(pts[0].real()+.5),deltay+int(pts[0].imag()+.5),deltax+int(pts[1].real()+.5),deltay+int(pts[1].imag()+.5));
            } // end else adeltai==0 , adeltaj==0
          } // end else adeltai==0
        } // end LINE_VECT
        else {
          double N=1;
          if (adeltai){
            N=clip_w/adeltai+1;
            if (adeltaj)
              N=max(N,clip_h/adeltaj+1);
          }
          else {
            if (adeltaj)
              N=clip_h/adeltaj+1;
          }
          N *= 2; // increase N since rounding might introduce too small clipping
          while (fabs(N*deltai)>10000)
            N /= 2;
          while (fabs(N*deltaj)>10000)
            N /= 2;
          check_fl_line(round(i0save)+deltax,round(j0save)+deltay,round(i1+N*deltai)+deltax,round(j1+N*deltaj)+deltay,clip_x,clip_y,clip_w,clip_h,0,0);
        }
      } // end seghalfline
  */
      if ( (point.subtype==_GROUP__VECT) && (point._VECTptr->size()==2))
        ; // no legend for segment
      else {
        if (!hidden_name);
      //    draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
      }
      lineItems.append(new Curve(path,this));
    } // end pnt subcase
//*/
  }
void GraphWidget::drawAxes(){
    double tick=find_tick(xmax-xmin);
    qDebug()<<tick;
}
void GraphWidget::drawGrid(){

}
void GraphWidget::drawElements(QVector<MyItem*> & v,QPainter* painter){
    if (v.isEmpty()) return;
    for(int i=0;i<v.size();++i){
        v.at(i)->draw(painter);
    }


}
void GraphWidget::toScreenCoord(const double x,const double y,double& xScreen, double& yScreen){
//    qDebug()<<"unités"<<xmin<<xmax<<xunit<<yunit<<zoom;
    xScreen=(x-xmin)*xunit*zoom;
    yScreen=(ymax-y)*yunit*zoom;
}

void GraphWidget::setXYUnit(){
    xunit=width()/(xmax-xmin);
    yunit=height()/(ymax-ymin);
}
void GraphWidget::paintEvent(QPaintEvent * ){
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing,true);
    drawGrid();
    drawAxes();
    drawElements(filledItems,&painter);
    drawElements(lineItems,&painter);
    drawElements(pointItems,&painter);
}
