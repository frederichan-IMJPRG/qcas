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
void WizardCatalog::createGui(){
    lineEdit=new QLineEdit;
    zone=new QTextBrowser;

    zone->setText(tr("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">"
"<center><h3><font color=\"#40A497\">Recherche par mot-clé</font></h3></center>"
                  "<hr> <center>Cet outil vous permet de naviguer aisément parmi les différents commandes ainsi que leurs descriptions.</center>"));

    zone->setOpenLinks(false);

    QAction *findAction=new QAction(tr("Rechercher"),this);
    findAction->setShortcut(tr("Entrée"));
    findAction->setStatusTip(tr("Rechercher"));
    findAction->setStatusTip(tr("Recherche la présence du mot-clé dans la description des différentes commandes"));
    findAction->setIcon(QIcon(":/images/edit-find.png"));

    previousAction=new QAction(tr("&Précédent"),this);
    previousAction->setShortcut(tr("Alt+Gauche"));
    previousAction->setStatusTip(tr("Page précédente"));
    previousAction->setIcon(QIcon(":/images/previous.png"));


    nextAction=new QAction(tr("&Suivant"),this);
    nextAction->setShortcut(tr("Alt+Droite"));
    nextAction->setStatusTip(tr("Page suivante"));
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
