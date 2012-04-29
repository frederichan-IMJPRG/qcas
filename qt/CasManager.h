#ifndef CASMANAGER_H
#define CASMANAGER_H

#include <streambuf>
#include "EvaluationThread.h"
#include "giac/gen.h"
#include <QGraphicsScene>
class OutputWidget;
/**
 the class mybuf is used to redirect the standard std::cout
 We can catch print("....") commands, and all comment for libgiac
  **/

class mybuf: public std::streambuf{
 private:
   void    put_buffer(void);
   void    put_char(int);
   EvaluationThread *ev;

   protected:
   int    overflow(int);
   int    sync();
   public:
   mybuf(EvaluationThread *ev,int = 0);
   // ~mybuf();
 };
class MyStream: public std::ostream{
 public:
   MyStream(EvaluationThread *ev,int = 0);
 };

/**
  The main class which handles with the libGiac
  This is an interface for main Window to deal with the libGiac (run command, add to history ...)
  **/
class CasManager :QObject{
public:
    CasManager(EvaluationThread* );
    void evaluate();
    void killThread();
    giac::context* getContext() const;
    giac::gen getAnswer() const;
    EvaluationThread::warning initExpression(const QString *str);
    OutputWidget* createDisplay();
    //    QString& getVariableName();
//    QString& getVariableValue();

private:
    QString gen2mathml(const giac::gen &);
    EvaluationThread *ev;
    bool testExpression(const giac::gen& );
    giac::gen expression;
    giac::gen answer;
    giac::context *context;
    void info(giac::gen &, int decal ) const;
    OutputWidget* formula2Widget(const QString &mathml);
    OutputWidget* graph2Widget(const giac::gen&);
    void drawOnScene(QGraphicsScene &,const giac::gen &);
    QString displayType(int c) const;
    QString displaySubType(int c) const;

 };


#endif // CASMANAGER_H
