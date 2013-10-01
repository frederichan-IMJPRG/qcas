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

#include <QTextBrowser>
#include <QToolButton>
#include <QToolBar>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStringList>
#include <QAction>
#include <QDebug>
#include "WizardCatalog.h"
#include "../MainWindow.h"
#include "../CasManager.h"
WizardCatalog::WizardCatalog(MainWindow *parent):QWidget(parent){
    mainWindow=parent;
    historyIndex=-1;
    history=new QStringList;
    createGui();
}
void WizardCatalog::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange)
        {
            retranslate();
        }
    QWidget::changeEvent(event);
}
void WizardCatalog::retranslate(){
    zone->setText(tr("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
"<center><h3><font color=\"#40A497\">Recherche par mot-clé</font></h3></center>"
                  "<hr> <center>Cet outil vous permet de naviguer aisément parmi les différents commandes ainsi que leurs descriptions.</center>"));

    findAction->setText(tr("Rechercher"));
    findAction->setShortcut(tr("Entrée"));
    findAction->setStatusTip(tr("Recherche la présence du mot-clé dans la description des différentes commandes"));
   
    previousAction->setText(tr("&Précédent"));
    previousAction->setShortcut(tr("Alt+Gauche"));
    previousAction->setStatusTip(tr("Page précédente"));

    nextAction->setText(tr("&Suivant"));
    nextAction->setShortcut(tr("Alt+Droite"));
    nextAction->setStatusTip(tr("Page suivante"));

}
void WizardCatalog::createGui(){
    lineEdit=new QLineEdit;
    zone=new QTextBrowser;

 
    zone->setOpenLinks(false);

    findAction=new QAction("",this);
    findAction->setIcon(QIcon(":/images/edit-find.png"));

    previousAction=new QAction("",this);
    previousAction->setIcon(QIcon(":/images/previous.png"));


    nextAction=new QAction("",this);
    nextAction->setIcon(QIcon(":/images/next.png"));

    connect(nextAction,SIGNAL(triggered()),this,SLOT(goNext()));
    connect(findAction,SIGNAL(triggered()),this,SLOT(find()));
    connect(previousAction,SIGNAL(triggered()),this,SLOT(goBack()));

    QToolBar *bar=new QToolBar;
    bar->addAction(findAction);
    bar->addAction(previousAction);
    bar->addAction(nextAction);

    QHBoxLayout *hlayout=new QHBoxLayout;
    hlayout->addWidget(lineEdit);
    hlayout->addWidget(bar);
    previousAction->setEnabled(false);
    nextAction->setEnabled(false);

    QVBoxLayout *vlayout=new QVBoxLayout;
    vlayout->addLayout(hlayout);
    vlayout->addWidget(zone);

    setLayout(vlayout);
    connect(lineEdit,SIGNAL(returnPressed()),this,SLOT(find()));
    connect(zone,SIGNAL(anchorClicked(QUrl)),this,SLOT(displayPage(QUrl)));

    retranslate();
}
void WizardCatalog::find(){
    QString keyWord(lineEdit->text());
    addHistory("seek "+keyWord);
    zone->setText(mainWindow->getCommandInfo()->seekForKeyword(keyWord));
}


void WizardCatalog::displayPage(QUrl url){
    QString keyWord=url.path();
    addHistory(keyWord);
    zone->setText(mainWindow->getCommandInfo()->displayPage(keyWord));
}
void WizardCatalog::updateButtons(){
    previousAction->setEnabled(historyIndex>0);
    nextAction->setEnabled(historyIndex<history->size()-1);

}
void WizardCatalog::addHistory(const QString &st){
    ++historyIndex;
    if (historyIndex<history->size()){
        history->replace(historyIndex,st);
    }
    else history->append(st);
    updateButtons();
}
void WizardCatalog::goBack(){
    --historyIndex;
    QString url=history->at(historyIndex);
    if (url.startsWith("seek ")){
        zone->setText(mainWindow->getCommandInfo()->seekForKeyword(url.remove(0,5)));
    }
    else zone->setText(mainWindow->getCommandInfo()->displayPage(url));
    updateButtons();
}
void WizardCatalog::goNext(){
    ++historyIndex;
    QString url=history->at(historyIndex);
    if (url.startsWith("seek ")){
        zone->setText(mainWindow->getCommandInfo()->seekForKeyword(url.remove(0,5)));
    }
    else zone->setText(mainWindow->getCommandInfo()->displayPage(url));
    updateButtons();

}
