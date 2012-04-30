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

#include <QCheckBox>
#include <QGridLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QLayoutItem>
#include <QDebug>
#include "../CasManager.h"
#include "../MainWindow.h"
#include "FormalLineWidgets.h"
#include "FormalSheet.h"
#include "FormalLine.h"
#include "src/qtmmlwidget.h"
#include "../output.h"


Line::Line(int id,FormalWorkSheet *parent):QWidget(parent){
    this->id=id;
    workSheet=parent;

    out=NULL;
    check=new QCheckBox(QString::number(id+1));

   input=new TextInput(this);


   mainLayout=new QGridLayout;
   mainLayout->addWidget(input,0,1,Qt::AlignTop);
   mainLayout->addWidget(check,0,0,Qt::AlignTop);
   setLayout(mainLayout);
   connect(check,SIGNAL(clicked()),this,SLOT(selectLevel()));
}
Line::~Line(){
    delete mainLayout;
    delete check;
    delete input;
    delete out;

}
/** When the user selects or unselects the checkBox number
  **/
void Line::selectLevel(){
 //   qDebug()<<"shift"<<check->hasShift();
    getWorkSheet()->addSelectedLevel(id);
   // check->shiftToFalse();
}
/**
  When user starts a new task, add The Stop Button under the QCheckBox
**/
void Line::addStopButton(QToolButton *stop){
    mainLayout->addWidget(stop,1,0);
    stop->show();
}
void Line::removeStopButton(){
    mainLayout->removeWidget(workSheet->getApp()->getStopButton());

//    qDebug()<<workSheet->getApp()->getStopButton()->parentWidget();
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
void Line::evaluate(const QString & formula){
    if (out!=NULL){
        mainLayout->removeWidget(out);
        delete out;
    }
    getWorkSheet()->getApp()->evaluate(formula);

}
OutputWidget* Line::getOuputWidget(){
    return out;
}

void Line::displayResult(OutputWidget* uuu){
    this->out=uuu;
    out->setMinimumSize(out->sizeHint());
    out->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    mainLayout->addWidget(out,1,1,Qt::AlignLeft);
    out->show();
    getWorkSheet()->goToNextLine();

}
OutputWidget* Line::gen2Widget(const QString &mathml){
    QString m("<math mode=\"display\">\n");
    m.append(mathml);
    m.append("\n</math>");


    qDebug()<<"----------------------";
    qDebug()<<m;
    qDebug()<<"----------------------";

    QtMmlWidget *mmlWidget=new QtMmlWidget;

    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(m, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {
        qWarning("MathML error: %s, Line: %d, Column: %d",
        errorMsg.constData()->toLatin1(), errorLine, errorColumn);
      }
      QPalette p=mmlWidget->palette();
      p.setColor(QPalette::WindowText,QColor::fromRgb(0,0,255));
      mmlWidget->setPalette(p);
      return new FormulaWidget(mmlWidget);
}
