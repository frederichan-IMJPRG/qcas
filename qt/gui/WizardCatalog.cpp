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
#include <QFile>
#include <QAction>
#include <QDebug>
#include "WizardCatalog.h"
#include "../MainWindow.h"
#include "../CasManager.h"
#include "../config.h"
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
    /*zone->setText(tr("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
"<center><h3><font color=\"#40A497\">Recherche par mot-clé</font></h3></center>"
                  "<hr> <center>Cet outil vous permet de naviguer aisément parmi les différents commandes ainsi que leurs descriptions.</center>"));
    */
    QString giacdoc;
    if (Config::XcasRoot != ""){
        giacdoc=Config::XcasRoot;
    }
    else{
    giacdoc=QString::fromStdString(giac::giac_aide_dir());//the xcas doc
    }
    if(! giacdoc.endsWith("/"))
        giacdoc.append("/");
    QString lg=Config::GiacStrLanguage;
    giacdoc.append("doc/"+lg+"/cascmd_"+lg+"");
    QStringList docpaths;
    docpaths<<":doc/"+lg+"/"<< giacdoc;

    zone->setSearchPaths(docpaths);
    zone->reload();
    homeAction->setText(tr("Page d'accueil"));
    home();

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
    zone->setOpenLinks(false);//we take care of html links in newPage

    findAction=new QAction("",this);
    findAction->setIcon(QIcon(":/images/edit-find.png"));

    homeAction=new QAction("",this);
    homeAction->setIcon(QIcon(":/images/home.png"));

    previousAction=new QAction("",this);
    previousAction->setIcon(QIcon(":/images/previous.png"));


    nextAction=new QAction("",this);
    nextAction->setIcon(QIcon(":/images/next.png"));

    connect(nextAction,SIGNAL(triggered()),this,SLOT(goNext()));
    connect(findAction,SIGNAL(triggered()),this,SLOT(find()));
    connect(homeAction,SIGNAL(triggered()),this,SLOT(home()));
    connect(previousAction,SIGNAL(triggered()),this,SLOT(goBack()));

    QToolBar *bar=new QToolBar;
    bar->addAction(homeAction);
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
    connect(zone,SIGNAL(anchorClicked(QUrl)),this,SLOT(newPage(QUrl)));//we don't systematically follow url

    retranslate();
}
void WizardCatalog::find(){
    QString keyWord(lineEdit->text());
    addHistory("seek "+keyWord);
    zone->setText(mainWindow->getCommandInfo()->seekForKeyword(keyWord));
}

void WizardCatalog::home(){
    //zone->setSource(QUrl(tr("index.html")));
    QString startpage=("menu.html");
    addHistory(startpage);
    zone->setSource(QUrl(startpage));
}

void WizardCatalog::displayPage(QUrl url){
    QString keyWord=url.path();

    if (url.toString().startsWith("?")){
        mainWindow->displayXcasHelp(url.toString());
    }
    else{
        if(keyWord.contains("html"))
           zone->setSource(url);
        else
           zone->setText("");
        if(zone->toPlainText().isEmpty()){
           zone->setText(mainWindow->getCommandInfo()->displayPage(keyWord));
        }
        //if the url is not a valid xcas htmldoc nor a valid keyword, we send it to the search engine.
        if(zone->toPlainText().trimmed().isEmpty()){
           zone->setText(mainWindow->getCommandInfo()->seekForKeyword(keyWord));
        }
    }
}

void WizardCatalog::displayHome(){
    home();
}

void WizardCatalog::newPage(QUrl url){
    //QString keyWord=url.path();
    QString keyWord=url.toString();//to keep the fragments (#...)
    //qDebug()<<"history:"<<keyWord<<"tostring"<<url.toString();

    if (keyWord.startsWith("!")){
        keyWord.remove(QRegExp("^!"));
        mainWindow->sendText(keyWord);
    }
    else if (url.toString().startsWith("?")){
            addHistory(url.toString());
            mainWindow->displayXcasHelp(url.toString());

    }
    else{
    addHistory(keyWord);
    zone->setSource(url);

    /*if(zone->toPlainText().isEmpty()){
        QFile fichressources(":doc/"+url.path());
        fichressources.open(QIODevice::ReadOnly);
            QTextStream stream(& fichressources);
            stream.setCodec("UTF-8");
            zone->setText(stream.readAll());
    }*/

    if(zone->toPlainText().isEmpty()){
        zone->setText(mainWindow->getCommandInfo()->displayPage(keyWord));
        //mainWindow->sendText(keyWord);

    }
    }
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
    //else zone->setText(mainWindow->getCommandInfo()->displayPage(url));
    else displayPage(url);
    updateButtons();
}
void WizardCatalog::goNext(){
    ++historyIndex;
    QString url=history->at(historyIndex);
    if (url.startsWith("seek ")){
        zone->setText(mainWindow->getCommandInfo()->seekForKeyword(url.remove(0,5)));
    }
    //else zone->setText(mainWindow->getCommandInfo()->displayPage(url));
    else displayPage(url);
    updateButtons();

}
