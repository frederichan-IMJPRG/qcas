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

#ifndef WIZARDMATRIX_H
#define WIZARDMATRIX_H

# include <QWidget>
#include "../MainWindow.h"
class QTableWidget;
class QSpinBox;
class WizardMatrix: public QWidget{
    Q_OBJECT
public:
    WizardMatrix(MainWindow *parent=0);
private:
    void createGui();
    QSpinBox *rowSpin;
    QSpinBox *columnSpin;
    QTableWidget *table;
    MainWindow *mainWindow;
private slots:
    void modifyRow(int);
    void modifyColumn(int);
    void writeMatrix();
};
#endif // WIZARDMATRIX_H
