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

#ifndef FORMALLINE_H
#define FORMALLINE_H
#include <QWidget>
#include "giac/gen.h"
class QGridLayout;
class TextInput;
class TextEditOutput;
class FormalWorkSheet;
class OutputWidget;
class QToolButton;
class QCheckBox;

class Line: public QWidget{
    Q_OBJECT
public:
    Line(int id,FormalWorkSheet* parent=0);
    ~Line();
    FormalWorkSheet* getWorkSheet() const;
    TextInput* getTextInput() const;
    int getId() const;
    void setId(const int &);
    OutputWidget* getOuputWidget();
    void evaluate(const QString&);
    void displayResult(OutputWidget*);
    void addStopButton(QToolButton* stop);
    void removeStopButton();

private:
    int id;
    FormalWorkSheet* workSheet;
    QGridLayout *mainLayout;
    QCheckBox *check;
    TextInput *input;
    TextEditOutput *textformula;
    OutputWidget *out;
    QCheckBox *checkshowout;


private slots:
    void selectLevel();
    void showhideOutWidgets();
};

#endif // FORMALLINE_H
