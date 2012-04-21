#include <QTabBar>
#include <QDebug>
#include <QToolButton>
#include <QMenu>
#include <QLabel>
#include "MainWindow.h"
#include "FormalSheet.h"
#include "CentralTabWidget.h"

MainTabWidget::MainTabWidget(MainWindow *main):QTabWidget(){
    mainWindow=main;

    setTabsClosable(true);
    setIconSize(QSize(20,20));
    add=new QToolButton;
    add->setIcon(QIcon(":/images/add.png"));
    add->setPopupMode(QToolButton::InstantPopup);
    formalAction=new QAction(tr("Calcul formel"),add);
    formalAction->setIcon(QIcon(":/images/formal.png"));

    spreadsheetAction=new QAction(tr("Tableur"),add);
    spreadsheetAction->setIcon(QIcon(":/images/spreadsheet.png"));
    programmingAction=new QAction(tr("Editeur de programmes"),add);
    programmingAction->setIcon(QIcon(":/images/programming.png"));

    menu=new QMenu;
    menu->addAction(formalAction);
    menu->addAction(spreadsheetAction);
    menu->addAction(programmingAction);
    add->setMenu(menu);

    connect(formalAction,SIGNAL(triggered()),this,SLOT(addFormalSheet()));
    connect(spreadsheetAction,SIGNAL(triggered()),this,SLOT(addSpreadSheet()));
    connect(programmingAction,SIGNAL(triggered()),this,SLOT(addProgrammingSheet()));

    addTab(new FormalWorkSheet(mainWindow),QIcon(":/images/formal.png"),tr("Feuille n°1"));
    addTab(new QLabel(""),"");
    tabBar()->setTabButton(1,QTabBar::RightSide,add);

    connect(this,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
}
void MainTabWidget::closeTab(int id){
    MainSheet* sheet=dynamic_cast<MainSheet*>(widget(id));
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(widget(id));
            this->removeTab(id);
            delete form;
        }
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
    for (int i=id;i<=count()-2;i++){
        tabBar()->setTabText(i,tr("Feuille n°")+QString::number(i+1));
    }

}
void MainTabWidget::addFormalSheet(){
    this->insertTab(count()-1,new FormalWorkSheet(mainWindow),QIcon(":/images/formal.png"),tr("Feuille n°")+QString::number(count()));
    this->setCurrentIndex(count()-2);
    (qobject_cast<FormalWorkSheet*>(widget(count()-2)))->setFocus(Qt::OtherFocusReason);
}
void MainTabWidget::addSpreadSheet(){
    qDebug()<<"spreadsheet added";
}

void MainTabWidget::addProgrammingSheet(){
    qDebug()<<"programmaing editor added";
}
MainSheet::MainSheet(sheetType s){
    type=s;
}
MainSheet::sheetType MainSheet::getType()const{
return type;
}


