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

#include <QDebug>
#include <ostream>
#include <QGraphicsScene>
#include <QGraphicsView>
#include "CasManager.h"
#include "giac/giac.h"
#include "global.h"
//#include "EvaluationThread.h"
#include "gui/FormalLineWidgets.h"
#include "gui/src/qtmmlwidget.h"
#include "output.h"
#include "config.h"
#include "CasManager.h"
#include <QMessageBox>
using namespace std;
using namespace giac;

giac::gen CasManager::answer=giac::gen("",context0);


MonitorThread::MonitorThread(giac::context * c){
    contextptr=c;
}
void MonitorThread::run(){
    while(true){
        if (check_thread(contextptr)==1)  QThread::msleep(50);
        else break;
    }
}
StopThread::StopThread(giac::context * c){
    contextptr=c;
}
void StopThread::run(){
    // First try with ctrl_c=true;
    giac::ctrl_c=true;
    sleep(2);
    // Finally, try to kill thread  (dirty...), be carefull with crash!!
    if (check_thread(contextptr)==1) {
        qDebug()<<"Dirty try to interrupt thread!!!";
        go=false;
        emit(startDirtyInterrupt());
        while(!go){
            msleep(20);
        }
        kill_thread(true,contextptr);
    }
    else qDebug()<<"Clean interruption";
}
void StopThread::setContinueTrue(){
    go=true;
}

mybuf::mybuf(CasManager * c,int bsize): streambuf(){
    cas=c;
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
    cas->appendPrintCache(c);
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


MyStream::MyStream(CasManager *cas,int bsize):ostream(new mybuf(cas,bsize)) {

}

CasManager::CasManager(MainWindow* main){
    mainWindow=main;
    giac::child_id=1;
    signal(SIGINT,giac::ctrl_c_signal_handler);

    context=new giac::context;
    mainWindow->setDecimalDigits(giac::decimal_digits(context));
//    giac::language(0,context);

    monitor=new MonitorThread(context);
    stopThread=new StopThread(context);
    connect(stopThread,SIGNAL(startDirtyInterrupt()),mainWindow,SLOT(displayCrashWarning()));
    connect(mainWindow,SIGNAL(hideCrashWarning()),stopThread,SLOT(setContinueTrue()));
    connect(monitor,SIGNAL(finished()),mainWindow,SLOT(removeStopWarning()));
    logptr(new MyStream(this),context);
}
CasManager::~CasManager(){
    delete context;
    delete monitor;
    delete stopThread;
}

bool CasManager::testExpression(const giac::gen & exp){

    if (exp.is_symb_of_sommet(giac::at_equal)){
        return false;
    }
    return true;

}
CasManager::warning CasManager::initExpression(const QString *str){
    expression=giac::gen(str->toStdString(),context);


    bool b=testExpression(expression);
    if (!b){
        return CasManager::Warning;
    }



    return CasManager::No_warning;
}



/*void background_callback(const gen & g,void * newcontextptr){
  if (g.type==_VECT && g._VECTptr->size()==2){
    context * cptr=(giac::context *)newcontextptr;
    if (cptr){
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_lock(cptr->globalptr->_mutex_eval_status_ptr);
  sto(g._VECTptr->back(),g._VECTptr->front(),cptr);
  pthread_mutex_unlock(cptr->globalptr->_mutex_eval_status_ptr);
#endif
    }
  }
}*/
void CasManager::callback(const giac::gen & g,void * newcontextptr) {
    answer=g;
//    context=newcontextptr;
}


/*void callback(const giac::gen & g,void * newcontextptr) {
    CasManager::answer=g;
//    context=newcontextptr;
}*/
/*
void callback(const gen & g,void * newcontextptr){
  if (g.type==_VECT && g._VECTptr->size()==2){
    context * cptr=(giac::context *)newcontextptr;
    if (cptr){
#ifdef HAVE_LIBPTHREAD
  pthread_mutex_lock(cptr->globalptr->_mutex_eval_status_ptr);
  sto(g._VECTptr->back(),g._VECTptr->front(),cptr);
  pthread_mutex_unlock(cptr->globalptr->_mutex_eval_status_ptr);
#endif
    }
  }
}*/



void CasManager::evaluate(){
/*    gen var("A",context);
    gen rep(eval(var,1,context));
            qDebug()<<"test..."<<QString::fromStdString(rep.print());

*/
    if (stopThread->isRunning()){
        stopThread->wait(2000);
    }

    printCache="";

   if (giac::make_thread(expression,eval_level(context),CasManager::callback,(void*)context,context))
    {
       disconnect(monitor,SIGNAL(finished()),mainWindow,SLOT(displayResult()));

        monitor->start();
        connect(monitor,SIGNAL(finished()),mainWindow,SLOT(displayResult()));
    }
    // Thread is already busy
    else {

    }

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

    if (!stopThread->isRunning()) {
        mainWindow->displayStopWarning();
        stopThread->start();
    }
}

giac::context* CasManager::getContext() const{
    return context;

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
    else if(gg.type==giac::_DOUBLE_){
        qDebug()<<"DOUBLE";
        qDebug()<<QString::fromStdString(gg.print(context));
    }
    else     {
        qDebug()<< s << "autres";
        qDebug()<< s << displayType(gg.type);
        qDebug()<< s << QString::fromStdString(gg.print());
    }

    }
bool CasManager::isRunning() const{
    return monitor->isRunning();
}


OutputWidget* CasManager::createDisplay(){
//   info(answer,0);//to have debug xml info
    if (answer.type == _VECT && graph_output_type(answer)){
      if (is3d(answer._VECTptr->back())){
        return new OutputWidget();
      }
      else {
        return new GraphWidget(answer,context,false,mainWindow);
    }
  }
  else if(answer.is_symb_of_sommet(at_pnt)){
        if (is3d(answer)){

        }
        else {
            return new GraphWidget(answer,context,false,mainWindow);
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

//    qDebug()<<gen2mathml(answer);
/*
return formula2Widget("<mrow>"
                      "<mi>A</mi>"
                      "<mo>=</mo>"
                      "<mfenced open=\"[\" close=\"]\">"
        "<mtable>"
                          "<mtr>"
                             "<mtd><mi>x</mi></mtd>"
                             "<mtd><mi>y</mi></mtd>"
                          "</mtr>"
                          "<mtr>"
                             "<mtd><mi>z</mi></mtd>"
                             "<mtd><mi>w</mi></mtd>"
                          "</mtr>"
                        "</mtable>"
                      "</mfenced>"
                    "</mrow>");
*/
/*    return formula2Widget(
    "<mrow>"
    "<apply>"
     " <eq/>"
      "<ci>A</ci>"
      "<matrix>"
       " <matrixrow>"
        "  <ci>x</ci>"
         " <ci>y</ci>"
        "</matrixrow>"
        "<matrixrow>"
         " <ci>z</ci>"
          "<ci>w</ci>"
        "</matrixrow>"
      "</matrix>"
    "</apply>"
    "</mrow>");*/
    return new FormulaWidget(answer,context);
}



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



void CasManager::appendPrintCache(const QChar& c){
    if(c=='"') printCache.append("&quot;");
    else if(c=='&') printCache.append("&amp;");
    else if(c=='<') printCache.append("&lt;");
    else if(c=='>') printCache.append("&gt;");
    else if(c=='\n') {
        fullDisplay.append(printCache);
        printCache="";
    }
    else printCache.append(c);

//    qDebug()<<"fullldisplay:"<<fullDisplay.size()<<fullDisplay;
//    qDebug()<<"printCache"<<printCache;
}

QStringList& CasManager::getGiacDisplay(){
    if (!printCache.isEmpty()) fullDisplay.append(printCache);
    return fullDisplay;
}

/*MonitorThread::getVariableName(){
    return cas->getVariableName();
}
MonitorThread::getVariableValue(){
    return cas->getVariableValue();

}*/
void CasManager::clearGiacDisplay(){
    fullDisplay.clear();
}

void CasManager::toXML(QDomElement & root){
    QDomElement settings=root.ownerDocument().createElement("settings");
    QDomElement cas=root.ownerDocument().createElement("cas");
    cas.setAttribute("xcas_mode",giac::xcas_mode(context));
    cas.setAttribute("scientific_format",giac::scientific_format(context));
    cas.setAttribute("integer_format",giac::integer_format(context));
    cas.setAttribute("digits",mainWindow->getDecimalDigit());
    cas.setAttribute("approx_mode",giac::approx_mode(context));
    cas.setAttribute("radian",giac::angle_radian(context));
    cas.setAttribute("complex_mode",giac::complex_mode(context));
    cas.setAttribute("complex_variables",giac::complex_variables(context));
    cas.setAttribute("increasing_power",giac::increasing_power(context));
    cas.setAttribute("all_trig",giac::all_trig_sol(context));
    cas.setAttribute("with_sqrt",giac::withsqrt(context));
    cas.setAttribute("epsilon",giac::epsilon(context));
    cas.setAttribute("proba_epsilon",giac::proba_epsilon(context));
    cas.setAttribute("recurs_eval",giac::eval_level(context));
    cas.setAttribute("eval_prog",giac::prog_eval_level_val(context));
    cas.setAttribute("recurs_prog",giac::MAX_RECURSION_LEVEL);
    cas.setAttribute("debug",giac::debug_infolevel);
    cas.setAttribute("newton",giac::NEWTON_DEFAULT_ITERATION);
    settings.appendChild(cas);
    QDomElement general=root.ownerDocument().createElement("general");
    general.setAttribute("graph_width",Config::graph_width);
    general.setAttribute("language",Config::language);
    general.setAttribute("xmin",giac::gnuplot_xmin);
    general.setAttribute("xmax",giac::gnuplot_xmax);
    general.setAttribute("ymin",giac::gnuplot_ymin);
    general.setAttribute("ymax",giac::gnuplot_ymax);
    general.setAttribute("zmin",giac::gnuplot_zmin);
    general.setAttribute("zmax",giac::gnuplot_zmax);
    general.setAttribute("tmin",giac::gnuplot_tmin);
    general.setAttribute("tmax",giac::gnuplot_tmax);
    general.setAttribute("autoscale",giac::autoscale);
    general.setAttribute("grid_attraction",Config::gridAttraction);
    settings.appendChild(general);
    root.appendChild(settings);



}

void CasManager::loadXML(const QDomElement& root){
    giac::xcas_mode(root.attribute("xcas_mode","0").toInt(),context);
    giac::scientific_format(root.attribute("scientific_format","0").toInt(),context);
    giac::integer_format(root.attribute("integer_format","10").toInt(),context);
    int d=root.attribute("digits","12").toInt();
    giac::decimal_digits(d,context);
    mainWindow->setDecimalDigits(d);
    giac::approx_mode(root.attribute("approx_mode","0").toInt(),context);
    giac::angle_radian(root.attribute("radian","1").toInt(),context);
    giac::complex_mode(root.attribute("complex_mode","0").toInt(),context);
    giac::complex_variables(root.attribute("complex_variables","0").toInt(),context);
    giac::increasing_power(root.attribute("increasing_power","0").toInt(),context);
    giac::all_trig_sol(root.attribute("all_trig","0").toInt(),context);
    giac::withsqrt(root.attribute("with_sqrt","10").toInt(),context);

    giac::epsilon(root.attribute("epsilon","1e-10").toDouble(),context);
    context->globalptr->_proba_epsilon_=root.attribute("proba_epsilon","1e-15").toDouble();
    context->globalptr->_eval_level=root.attribute("recurs_eval","25").toInt();
    giac::prog_eval_level_val(root.attribute("eval_prog","1").toInt(),context);
    giac::MAX_RECURSION_LEVEL=root.attribute("recurs_prog","100").toInt();
    giac::debug_infolevel=root.attribute("debug","0").toInt();
    giac::NEWTON_DEFAULT_ITERATION=root.attribute("newton","20").toInt();
}
void CasManager::loadGeneralXML(const QDomElement& general){
    Config::graph_width=general.attribute("graph_width","400").toInt();
    Config::language=(Config::LANGUAGES)(general.attribute("language","0").toInt());
    giac::gnuplot_xmin=general.attribute("xmin","-5").toDouble();
    giac::gnuplot_xmax=general.attribute("xmax","5").toDouble();
    giac::gnuplot_ymin=general.attribute("ymin","-5").toDouble();
    giac::gnuplot_ymax=general.attribute("ymax","5").toDouble();
    giac::gnuplot_zmin=general.attribute("zmin","-5").toDouble();
    giac::gnuplot_zmax=general.attribute("zmax","5").toDouble();
    giac::gnuplot_tmin=general.attribute("tmin","-6").toDouble();
    giac::gnuplot_tmax=general.attribute("tmax","6").toDouble();
    giac::autoscale=general.attribute("autoscale","1").toInt();
    Config::gridAttraction=general.attribute("grid_attraction","1").toInt();

}
