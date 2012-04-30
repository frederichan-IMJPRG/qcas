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

#ifndef WIZARDCATALOG_H
#define WIZARDCATALOG_H
#include <QWidget>
#include <QUrl>
class QLineEdit;
class QTextBrowser;
class QStringList;
class MainWindow;
class QAction;

class WizardCatalog :public QWidget{
    Q_OBJECT
public:
    WizardCatalog(MainWindow *parent=0);
private:
    void createGui();
    void addHistory(const QString &st);
    void updateButtons();
    MainWindow* mainWindow;
    QStringList *history;
    int historyIndex;
    QLineEdit* lineEdit;
    QTextBrowser *zone;
    QAction* previousAction;
    QAction* nextAction;
public slots:
   void displayPage(QUrl);

private slots:
    void find();
    void goBack();
    void goNext();
};


#endif // WIZARDCATALOG_H
