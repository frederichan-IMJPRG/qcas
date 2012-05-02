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


#include "CasManager.h"
#include "EvaluationThread.h"
#include <QDebug>
#include "global.h"
EvaluationThread::EvaluationThread(){
//    cas=new CasManager(this);
//    connect(this,SIGNAL(finished()),this,SLOT(closeTimer()));
}
void EvaluationThread::appendPrintCache(const QChar& c){
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

QStringList& EvaluationThread::getGiacDisplay(){
    return fullDisplay;
}
void EvaluationThread::killThread(){
//        terminate();
        cas->killThread();
}

EvaluationThread::warning EvaluationThread::setCommand(const QString & command){
    this->command=command;
    return 1;//cas->initExpression(&command);
}
/*EvaluationThread::getVariableName(){
    return cas->getVariableName();
}
EvaluationThread::getVariableValue(){
    return cas->getVariableValue();

}*/
void EvaluationThread::run(){

    printCache="";
    cas->evaluate();

    if (!printCache.isEmpty()) fullDisplay.append(printCache);

}
void EvaluationThread::clearGiacDisplay(){
    fullDisplay.clear();
}
giac::context* EvaluationThread::getContext()const{
    return cas->getContext();
}
OutputWidget*  EvaluationThread::displayResult(){
    return cas->createDisplay();
}

