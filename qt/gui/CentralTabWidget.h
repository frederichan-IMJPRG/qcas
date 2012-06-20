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
#ifndef CENTRALTABWIDGET_H
#define CENTRALTABWIDGET_H

#include <QTabWidget>
class QToolButton;
class QMenu;
class MainWindow;

class MainSheet{
public:
    enum sheetType{FORMAL_TYPE,G2D_TYPE,SPREADSHEET_TYPE,PROGRAMMING_TYPE};
    MainSheet(sheetType);
    sheetType getType() const;
protected:
    sheetType type;

};
class MainTabWidget:public QTabWidget{
    Q_OBJECT
public:
    MainTabWidget(MainWindow *main=0);
private:
    QToolButton *add;
    QMenu* menu;
    QAction* formalAction;
    QAction* spreadsheetAction;
    QAction* programmingAction;
    QAction* g2dAction;

    MainWindow *mainWindow;
public slots:
    void addFormalSheet();
    void addSpreadSheet();
    void addProgrammingSheet();
    void addG2dSheet();
    void closeTab(int);
    void changeTab(int);
};


#endif // CENTRALTABWIDGET_H
