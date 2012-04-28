#include "Interactive2d.h"
#include "iconsize.h"
#include <QToolBar>
#include <QMenu>
#include <QToolButton>
#include <QButtonGroup>
#include <QAction>
#include <QPushButton>
#include <QHBoxLayout>
Interactive2d::Interactive2d(MainWindow *parent):MainSheet(MainSheet::G2D){
    mainWindow=parent;
    initGui();
}
void Interactive2d::initGui(){
    singlept=new QAction(tr("Point"),this);
    singlept->setIcon(QIcon(":/images/point.png"));
    inter=new QAction(tr("Intersection entre deux objets"),this);
    inter->setIcon(QIcon(":/images/inter.png"));
    midpoint=new QAction(tr("Milieu ou centre"),this);
    midpoint->setIcon(QIcon(":/images/midpoint.png"));

    line=new QAction(tr("Droite"),this);
    line->setIcon(QIcon(":/images/line.png"));
    segment=new QAction(tr("Segment"),this);
    segment->setIcon(QIcon(":/images/segment.png"));
    halfline=new QAction(tr("Demie-droite"),this);
    halfline->setIcon(QIcon(":/images/halfline.png"));

    circle2pt=new QAction(tr("Cercle (centre-point)"),this);
    circle2pt->setIcon(QIcon(":/images/circle2pt"));
    circle3pt=new QAction(tr("Cercle (3 points)"),this);
    circle3pt->setIcon(QIcon(":/images/circle3pt"));

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
    menuLine->setStyle(new IconSize);

    menuCircle->addAction(circle2pt);
    menuCircle->addAction(circle3pt);
    menuCircle->setStyle(new IconSize);

    toolBar=new QToolBar(this);
    buttonPt=new QToolButton;
    QString s("QToolButton{margin: 8px;padding: 4 px;}"
//              "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//              "stop: 0 #f6f7fa, stop: 1 #dadbde);}"
//               "QToolButton:pressed {"
//        "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
//        "stop: 0 #dadbde, stop: 1 #f6f7fa);}"
            "QToolButton:checked {"
        "border-left: 2px solid blue;"
        "border-top: 2px solid blue;"
        "border-bottom: 2px solid blue;"

        "border-radius: 6px;}"
        "QToolButton:menu{   subcontrol-position: right bottom;}"
    "QToolButton::menu-indicator{ right bottom}");

    buttonPt->setStyleSheet( s);
    buttonPt->setCheckable(true);
    buttonPt->setPopupMode(QToolButton::MenuButtonPopup);
    buttonPt->setMenu(menuPt);
    buttonPt->setIcon(QIcon(":/images/point.png"));

    QString t(
            "QToolButton{margin:10px;}"
            "QToolButton:checked{border: 2px solid blue; }"
                   "QToolButton::menu-indicator{width: 10px;");
//            "subcontrol-origin: margin;}"
//            "QToolButton::menu-button {"
//              "border: 2px solid gray;"
//              "width: 30 px;}");
//              "QPushButton::menu-indicator:pressed, QPushButton::menu-indicator:open {"
//                 "position: relative;"
 //                "top: 2px; left: 2px;}");
              // "subcontrol-position: bottom right;"
              //"subcontrol-origin: contents;}"
              //"QToolButton[popupMode=\"1\"]{"
              //"subcontrol-position: bottom right;}"


    buttonLine=new QToolButton;
    buttonLine->setIconSize(QSize(48,48));
    buttonLine->setPopupMode(QToolButton::MenuButtonPopup);
    buttonLine->setCheckable(true);
    buttonLine->setStyleSheet(t);
    buttonLine->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    buttonLine->setMenu(menuLine);
    buttonLine->setIcon(QIcon(":/images/line.png"));
    buttonCircle=new QToolButton;
    buttonCircle->setPopupMode(QToolButton::InstantPopup);
    buttonCircle->setMenu(menuCircle);
    buttonCircle->setIcon(QIcon(":/images/circle2pt.png"));

//    toolBar->addWidget(buttonPt);
//    toolBar->addWidget(buttonLine);
 //   toolBar->addWidget(buttonCircle);
  //  toolBar->setIconSize(QSize(48,48));


    QButtonGroup *group=new QButtonGroup;
    group->addButton(buttonPt);
    group->addButton(buttonLine);
    group->addButton(buttonCircle);

    QWidget* toolPanel=new QWidget;
    toolPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    QHBoxLayout* hbox=new QHBoxLayout;
    hbox->addWidget(buttonPt,Qt::AlignLeft);
    hbox->addWidget(buttonLine,Qt::AlignLeft);
    hbox->addWidget(buttonCircle,Qt::AlignLeft);
    toolPanel->setLayout(hbox);

    QVBoxLayout* vbox=new QVBoxLayout;
    canvas=new QWidget;


    vbox->addWidget(toolPanel,Qt::AlignLeft);
    vbox->addWidget(canvas);
    setLayout(vbox);
}
