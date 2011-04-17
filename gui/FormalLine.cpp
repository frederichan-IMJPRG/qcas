#include <QCheckBox>
#include <QGridLayout>
#include <QScrollArea>
#include "../CasManager.h"
#include "../MainWindow.h"
#include "FormalLineWidgets.h"
#include "FormalSheet.h"
#include "FormalLine.h"
#include "src/qtmmlwidget.h"

Line::Line(int id,FormalWorkSheet *parent):QWidget(parent){
    this->id=id;
    out=NULL;
    workSheet=parent;
   QString str;
   str.setNum(id+1);
   check=new QCheckBox(str);
   input=new TextInput(this);


   mainLayout=new QGridLayout;
   mainLayout->addWidget(input,0,1,Qt::AlignTop);
   mainLayout->addWidget(check,0,0,Qt::AlignTop);
   setLayout(mainLayout);

}
int Line::getId()const{
    return id;
}
TextInput* Line::getTextInput() const{
    return input;
}

FormalWorkSheet* Line::getWorkSheet() const{
    return workSheet;
}
void Line::output(const QString & formula){
    if (out!=NULL){
        mainLayout->removeWidget(out);
        delete out;
    }

    out=getWorkSheet()->getApp()->getCas()->evaluate(formula);
    out->setMinimumSize(out->sizeHint());
    out->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);

    mainLayout->addWidget(out,1,1,Qt::AlignLeft);
    out->show();

  /*  QString mathml("<math mode=\"display\">\n"
                   "<msqrt><mn>2</mn></msqrt><mo>+</mo><mn>2</mn><msqrt><mn>2</mn></msqrt>\n"
                   "</math>");
    for (int i=0;i<mathml.length();++i){
        qDebug()<<mathml.at(i);
    }*/
    //QPalette pal=->palette();
    //pal.setColor(w->backgroundRole(),Qt::yellow);
    //w->setPalette(pal);

}
