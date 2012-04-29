#ifndef EVALUATIONTHREAD_H
#define EVALUATIONTHREAD_H
#include <QThread>
#include <QStringList>
#include "global.h"
class OutputWidget;
class CasManager;

class EvaluationThread:public QThread{
    Q_OBJECT
public:
    enum warning{NO_WARNING=0,WARNING=1,ERROR=2};
    EvaluationThread();
    warning setCommand(const QString & command);
    OutputWidget* displayResult();
    void appendPrintCache(const QChar& );
    QStringList& getGiacDisplay() ;
    void killThread();
    giac::context* getContext()const;

//    QString& getVariableName();
//    QString& getVariableValue();
protected:
    void run();
private:
    QString printCache;
    QString command;
    CasManager *cas;
    QStringList fullDisplay;

};
#endif // EVALUATIONTHREAD_H
