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
    void clearGiacDisplay();

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
