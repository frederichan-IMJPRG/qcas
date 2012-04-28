#include "Interactive2d.h"
#include "iconsize.h"
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QAction>
#include <QHBoxLayout>
Interactive2d::Interactive2d(MainWindow *parent):MainSheet(MainSheet::G2D){
    mainWindow=parent;
    initGui();
}
void Interactive2d::initGui(){
    singlept=new QAction(tr("Point"),this);
    singlept->setIcon(QIcon(":/images/point.png"));

    inter=new QAction(tr("Intersection entre deux objets"),this);
    midpoint=new QAction(tr("Milieu ou centre"),this);
    line=new QAction(tr("Droite"),this);
    segment=new QAction(tr("Segment"),this);
    halfline=new QAction(tr("Demie-droite"),this);
    circle2pt=new QAction(tr("Cercle (centre-point)"),this);
    circle3pt=new QAction(tr("Cercle (3 points)"),this);

    QMenu* menuPt=new QMenu;
    QMenu* menuLine=new QMenu;
    QMenu* menuCircle=new QMenu;

    menuPt->addAction(singlept);
    menuPt->addAction(inter);
    menuPt->addAction(midpoint);
    menuPt->setStyle(new IconSize);
    menuLine->addAction(line);
    menuLine->addAction(segment);
    menuLine->addAction(halfline);
    menuCircle->addAction(circle2pt);
    menuCircle->addAction(circle3pt);

    toolBar=new QToolBar(this);
    buttonPt=new QToolButton(toolBar);
    buttonPt->setPopupMode(QToolButton::InstantPopup);
    buttonPt->setMenu(menuPt);
    buttonPt->setIcon(QIcon(":/images/point.png"));
    buttonLine=new QToolButton(toolBar);
    buttonLine->setPopupMode(QToolButton::InstantPopup);
    buttonLine->setMenu(menuLine);
    buttonCircle=new QToolButton(toolBar);
    buttonCircle->setPopupMode(QToolButton::InstantPopup);
    buttonCircle->setMenu(menuCircle);

    toolBar->addWidget(buttonPt);
    toolBar->addWidget(buttonLine);
    toolBar->addWidget(buttonCircle);
    toolBar->setIconSize(QSize(48,48));


    QHBoxLayout* hbox=new QHBoxLayout;
    canvas=new QWidget;


    hbox->addWidget(toolBar);
    hbox->addWidget(canvas);
    setLayout(hbox);
}
