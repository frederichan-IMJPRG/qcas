#include <QDebug>
#include <ostream>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "CasManager.h"
#include "giac/giac.h"
#include "EvaluationThread.h"
#include "gui/FormalLineWidgets.h"
#include "gui/src/qtmmlwidget.h"
#include "output.h"
#include "config.h"

using namespace std;
using namespace giac;
mybuf::mybuf(EvaluationThread *eval,int bsize): streambuf(){
    ev=eval;
    if (bsize)
    {
      char	*ptr = new char[bsize];
      setp(ptr, ptr + bsize);
    }
  else
    setp(0, 0);

  setg(0, 0, 0);
}

int mybuf::overflow(int c){
  put_buffer();
  // {}{}{} warning!
  if (c != EOF){
    if (pbase() == epptr())
      put_char(c);
    else
        sputc(c);
    }
  return 0;
}


int    mybuf::sync(){
  put_buffer();
  return 0;
}

/** Append the character chr in the Giac messages zone
  @param chr: the character to add
  **/
void    mybuf::put_char(int chr){
 //   qDebug()<<(chr=='\n');
    QChar c=QChar(chr);
    ev->appendPrintCache(c);
}


void     mybuf::put_buffer(){
    if (pbase() != pptr())
  {
    int     len = (pptr() - pbase());
    char    * buffer = new char[len + 1];
    strncpy(buffer, pbase(), len);
    buffer[len] = 0;
    // mettre ici le code de sortie java de la chaine buffer
    setp(pbase(), epptr());
    cerr << "1 chaine " << endl;
    qDebug()<<"debug "<<buffer;
    delete [] buffer;
  }
}


MyStream::MyStream(EvaluationThread *ev,int bsize):ostream(new mybuf(ev,bsize)) {}


CasManager::CasManager(EvaluationThread *ev){
    this->ev=ev;
    signal(SIGINT,giac::ctrl_c_signal_handler);
    giac::child_id=1;

    context=new giac::context;
   logptr(new MyStream(ev),context);
}
bool CasManager::testExpression(const giac::gen & exp){

    if (exp.is_symb_of_sommet(giac::at_equal)){
        return false;
    }
    return true;

}
EvaluationThread::warning CasManager::initExpression(const QString *str){
    expression=giac::gen(str->toStdString(),context);
    bool b=testExpression(expression);
    if (!b){
        return EvaluationThread::WARNING;
    }



    return EvaluationThread::NO_WARNING;
}

void CasManager::evaluate(){
    giac::ctrl_c=true;

    answer=protecteval(expression,25,context);


    // Add result to history
    giac::history_in(context).push_back(expression);
    giac::history_out(context).push_back(answer);
}
giac::gen CasManager::getAnswer() const{
    return answer;
}

QString CasManager::gen2mathml(const giac::gen &result){
    return QString(giac::gen2mathml(result,context).c_str());
}
void CasManager::killThread(){
    if (giac::is_context_busy(context))
        giac::kill_thread(true,context);
}


QString CasManager::displayType(int c) const{
    switch(c){
    case 0:
        return "_INT";
     break;
    case 1:
     return "_DOUBLE_"; // double _DOUBLE_val
     break;
    case 2:
       return "_ZINT"; // mpz_t * _ZINTptr
    break;
    case 3:
    return "_REAL";
    break;
    case 4:
    return "_CPLX";
    break;
    case 5:
    return "_POLY";
    break;
    case 6:
    return "_IDNT";
    break;
    case 7:
    return "_VECT";
    break;
    case 8:
    return "_SYMB";
    break;
    case 9:
    return "SPLOY1";
    break;
    case 10:
    return "FRAC";
    break;
    case 11:
    return "EXT";
    break;
    case 12:
    return "STRNG";
    break;
    default: return QString::number(c);
    }

}

QString CasManager::displaySubType(int c) const{
    switch(c){
    case 1:
        return "_SEQ__VECT";
     break;
    case 2:
     return "_SET__VECT";
     break;
    case 3:
       return "_RPN_FUNC__VECT"; // mpz_t * _ZINTptr
    break;
    case 4:
    return "_STACK_FUNC_VECT";
    break;
    case 5:
    return "_GROUP__VECT";
    break;
    case 6:
    return "_LINE__VECT";
    break;
    case 7:
    return "_VECTOR__VECT";
    break;
    case 8:
    return "_PNT__VECT_||_CURVE_VECT";
    break;
    case 9:
    return "_HALF_LINE__VECT";
    break;
    case 10:
    return "_HALF_LINE__VECT";
    break;
    default: return QString::number(c);
    }
}

void CasManager::info(giac::gen & gg,int decal) const{
        QString s;

        for (int j=0;j<decal;j++){
            s.append(" ");
        }

    if (gg.type==giac::_SYMB){
        qDebug()<< s << "_SYMB Sommet";
        qDebug()<< s << QString::fromStdString(gg._SYMBptr->sommet.ptr()->print(context));
        giac::gen g=gg._SYMBptr->feuille;
        qDebug()<< s << "_SYMB feuille";
        info(g,decal);
    }
    else if (gg.type==giac::_VECT){
        decal=decal+5;

        giac::vecteur *v=gg._VECTptr;
        qDebug()<< s << "_VECT subtype"<< displaySubType(gg.subtype);
        qDebug()<< s << "size"<<QString::number(v->size());
        vecteur::iterator it;
        unsigned int i=0;
        for(it=v->begin();it<v->end();it++){


            qDebug()<< s<< "element"<<i;
            info(*it,decal);
            i++;
        }
        qDebug()<< s <<"//fin vect";
        decal=decal-5;
    }
    else     {
        qDebug()<< s << "autres";
        qDebug()<< s << displayType(gg.type);
        qDebug()<< s << QString::fromStdString(gg.print());
    }

    }
OutputWidget* CasManager::createDisplay(){
    info(answer,0);
    if (answer.type == _VECT && graph_output_type(answer)){
      if (is3d(answer._VECTptr->back())){
        return new OutputWidget();
      }
      else {
        return new GraphWidget(answer,context);
    }
  }
    else if(answer.is_symb_of_sommet(at_pnt)){
        if (is3d(answer)){

        }
        else {
            return new GraphWidget(answer,context);
        }
    }
/*    if (evaled_g.is_symb_of_sommet(at_pnt) || anim){
      Fl_Tile * g = new Fl_Tile(w->x(),w->y(),w->w(),max(130,w->w()/3));
      g->labelsize(w->labelsize());
      Graph2d3d * res;
      if (is3d(evaled_g)){
        res = new Graph3d(w->x(),w->y(),w->w(),g->h(),"",hp);
        res->show();
      }
      else {
        res=new Graph2d(w->x(),w->y(),w->w(),g->h(),"",hp);
        if (Xcas_config.ortho)
          res->orthonormalize();
      }
      res->add(evaled_g);
      if (anim)
        res->animation_dt=1./5;
      if (Xcas_config.autoscale)
        res->autoscale();
      res->update_infos(evaled_g,contextptr);
      g->end();
      change_group_fontsize(g,w->labelsize());
      return g;
    }


  */

    ////////////////////////////////////////////////

/*      bool graph=false;
    if (answer.is_symb_of_sommet(giac::at_pnt)) graph=true;
    else if(answer.type==giac::_VECT){
        qDebug()<<QString::number(answer.subtype);
        giac::vecteur* v=answer._VECTptr;
        for(int i=0;i<v->size();++i){
            if (v->at(i).is_symb_of_sommet(giac::at_pnt)){
                graph=true;
            }            
        }
    }
*/
//    if (graph) return graph2Widget(answer);
  //  else
    return formula2Widget(gen2mathml(answer));
}
OutputWidget* CasManager::formula2Widget(const QString &mathml){
    QString m("<math mode=\"display\">\n");
    m.append(mathml);
    m.append("\n</math>");


//    qDebug()<<"----------------------";
//    qDebug()<<m;
//    qDebug()<<"----------------------";

    QtMmlWidget *mmlWidget=new QtMmlWidget;

    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(m, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {
        qWarning("MathML error: %s, Line: %d, Column: %d",
                 errorMsg.constData(), errorLine, errorColumn);
      }
      QPalette p=mmlWidget->palette();
      p.setColor(QPalette::WindowText,QColor::fromRgb(0,0,255));
      mmlWidget->setPalette(p);
      return new FormulaWidget(mmlWidget);
}


void CasManager::drawOnScene(QGraphicsScene &scene,const gen & g){
    if (g.type==_VECT){
      vecteur & v =*g._VECTptr;
      const_iterateur it=v.begin(),itend=v.end();
      for (;it!=itend;++it){
            drawOnScene(scene,*it);
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
  /*  int mxw=Mon_image.w(),myw=Mon_image.h()-(Mon_image.show_axes?(Mon_image.title.empty()?1:2):0)*Mon_image.labelsize();
    double i0,j0,i0save,j0save,i1,j1;
    int fs=f.size();
    if ((fs==4) && (s==at_parameter)){
      return ;
    }
    string the_legend;
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
          gen e1=diametre._VECTptr->front().evalf_double(1,context),e2=diametre._VECTptr->back().evalf_double(1,context);
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
            check_fl_point(deltax+delta_i,deltay+delta_j,clip_x,clip_y,clip_w,clip_h,0,0);
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
          }
        }*/
      } // end point.type==_SYMB
      if (point.type!=_VECT){ // single point
          qDebug()<<"coucou";
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
/*      Mon_image.findij(*jt,x_scale,y_scale,i0,j0,context);
      if (fill_polygon && *jt==*(jtend-1)){
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
      }
      i0save=i0;
      j0save=j0;
      ++jt;
      if (jt==jtend){
        if (i0>0 && i0<mxw && j0>0 && j0<myw)
          check_fl_point(deltax+round(i0),deltay+round(j0),clip_x,clip_y,clip_w,clip_h,0,0);
        if (!hidden_name)
          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
        return;
      }
      bool seghalfline=( point.subtype==_LINE__VECT || point.subtype==_HALFLINE__VECT ) && (point._VECTptr->size()==2);
      // rest of the path
      for (;;){
        Mon_image.findij(*jt,x_scale,y_scale,i1,j1,context);
        if (!seghalfline){
          check_fl_line(round(i0)+deltax,round(j0)+deltay,round(i1)+deltax,round(j1)+deltay,clip_x,clip_y,clip_w,clip_h,0,0);
          if (point.subtype==_VECTOR__VECT){
            double dx=i0-i1,dy=j0-j1;
            petite_fleche(i1,j1,dx,dy,deltax,deltay,width);
          }
        }
        ++jt;
        if (jt==jtend){ // label of line at midpoint
          if (point.subtype==_LINE__VECT){
            i0=(6*i1-i0)/5-8;
            j0=(6*j1-j0)/5-8;
          }
          else {
            i0=(i0+i1)/2-8;
            j0=(j0+j1)/2;
          }
          break;
        }
        i0=i1;
        j0=j1;
      }
      // check for a segment/halfline/line
      if ( seghalfline){
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
      if ( (point.subtype==_GROUP__VECT) && (point._VECTptr->size()==2))
        ; // no legend for segment
      else {
        if (!hidden_name)
          draw_legende(f,round(i0),round(j0),labelpos,&Mon_image,clip_x,clip_y,clip_w,clip_h,0,0);
      }
    } // end pnt subcase
*/


  }
  }}

/*void autoscale(){
if (!plot_instructions.empty()){
  // Find the largest and lowest x/y/z in objects (except lines/plans)
  vector<double> vx,vy,vz;
  int s;
  context * contextptr=hp?hp->contextptr:get_context(this);
  bool ortho=autoscaleg(plot_instructions,vx,vy,vz,contextptr);
  autoscaleminmax(vx,window_xmin,window_xmax);
  zoomx(1.0);
  autoscaleminmax(vy,window_ymin,window_ymax);
  zoomy(1.0);
  autoscaleminmax(vz,window_zmin,window_zmax);
  zoomz(1.0);
  if (ortho)
    orthonormalize();
}
find_ylegende();
y_tick=find_tick(window_ymax-window_ymin);
redraw();
push_cfg();
}*/

OutputWidget* CasManager::graph2Widget(const  gen& g){






//     QGraphicsView view(&scene);


    return NULL;

}
