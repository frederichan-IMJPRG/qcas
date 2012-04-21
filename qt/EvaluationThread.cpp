#include "CasManager.h"
#include "EvaluationThread.h"
#include <QDebug>
EvaluationThread::EvaluationThread(){
    cas=new CasManager(this);
//    connect(this,SIGNAL(finished()),this,SLOT(closeTimer()));
}
void EvaluationThread::appendPrintCache(const QChar& c){
    qDebug()<<"appendPrintCache: fullldisplay:"<<fullDisplay;
    if(c=='"') printCache.append("&quot;");
    else if(c=='&') printCache.append("&amp;");
    else if(c=='<') printCache.append("&lt;");
    else if(c=='>') printCache.append("&gt;");
    else if(c=='\n') {
        fullDisplay.append(printCache);

        printCache="";
    }
    else printCache.append(c);
 }

QStringList& EvaluationThread::getGiacDisplay(){
    return fullDisplay;
}
void EvaluationThread::killThread(){
        terminate();
        cas->killThread();
}

EvaluationThread::warning EvaluationThread::setCommand(const QString & command){
    this->command=command;
    return cas->initExpression(&command);
}
/*EvaluationThread::getVariableName(){
    return cas->getVariableName();
}
EvaluationThread::getVariableValue(){
    return cas->getVariableValue();

}*/
void EvaluationThread::run(){

    printCache="";
    fullDisplay.clear();
    cas->evaluate();
    if (!printCache.isEmpty()) fullDisplay.append(printCache);
}

OutputWidget*  EvaluationThread::displayResult(){
    return cas->createDisplay();
}

