
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

#ifndef PLOTFUNCTIONDIALOG_H
#define PLOTFUNCTIONDIALOG_H
#include "gen.h"

#include <QDialog>
class QLineEdit;
class Canvas2D;
class QTabWidget;
class giac::context;
class PlotFunctionDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PlotFunctionDialog(Canvas2D *parent = 0);
    QString getString() const;
private:
    QTabWidget* tabWidget;
    QWidget* cartesianPanel;
    QLineEdit* editF;
    QWidget* polarPanel;
    QLineEdit* editR;
    QWidget* parametricPanel;
    QLineEdit* editX;
    QLineEdit* editY;
    QPushButton* ok;
    QPushButton* cancel;
    QLineEdit* editMin;
    QLineEdit* editMax;
    QString command;
    giac::context* context;
    void initGui();
    bool checkFunction(QLineEdit *);

private slots:
    void closeDialog();
};

#endif // PLOTFUNCTIONDIALOG_H