//#include <QtGui>
#include <QMenuBar>
#include <QMessageBox>
//#include <QLabel>
#include <QFileInfo>
#include <QFileDialog>
#include <QSplitter>
#include <QCloseEvent>
#include <QDebug>
//#include <QScrollArea>
#include <QToolBox>
//#include <QToolButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QMenu>
#include <QToolBar>
#include "MainWindow.h"
#include "gui/FormalSheet.h"
#include "gui/WizardEquation.h"
#include "gui/WizardMatrix.h"
#include "gui/WizardCatalog.h"
#include "CasManager.h"
#include "gui/CentralTabWidget.h"
//#include "gui/FormalSheet.h"
//#include <Qsci/qsciscintilla.h>
//#include <Qsci/qscilexerjava.h>
#include <QUrl>


/** MainWindow constructor

  **/


MainWindow::MainWindow(){
    cas=new CasManager;
    createAction();
    createMenus();
    createContextMenu();
    createToolBars();

    readSettings();

//    setWindowIcon(":/images/icon.png");
    setCurrentFile("");
    createGui();
    wizardList->setCurrentRow(2);
    (qobject_cast<FormalWorkSheet*>(tabPages->currentWidget()))->setFocus(Qt::OtherFocusReason);
}

void MainWindow::createAction(){
    newAction=new QAction(tr("&Nouveau"),this);
    newAction->setShortcut(tr("Ctrl+N"));
    newAction->setStatusTip(tr("Créer un nouvel espace de travail"));
    newAction->setIcon(QIcon(":/images/document-new.png"));
    connect(newAction,SIGNAL(triggered()),this,SLOT(newFile()));

    openAction=new QAction(tr("&Ouvrir..."),this);
    openAction->setShortcut(tr("Ctrl+O"));
    openAction->setStatusTip(tr("Ouvrir un nouveau fichier"));
    openAction->setIcon(QIcon(":/images/open.png"));
    connect(openAction,SIGNAL(triggered()),this,SLOT(open()));

    saveAction=new QAction(tr("&Enregistrer"),this);
    saveAction->setShortcut(tr("Ctrl+S"));
    saveAction->setStatusTip(tr("Enregistrer le fichier courant"));
    saveAction->setIcon(QIcon(":/images/document-save.png"));
    connect(saveAction,SIGNAL(triggered()),this,SLOT(save()));

    saveAsAction=new QAction(tr("&Enregistrer sous..."),this);
    saveAsAction->setShortcut(tr("Ctrl+Maj+S"));
    saveAsAction->setStatusTip(tr("Enregistrer sous un nouveau nom de fichier"));
    saveAsAction->setIcon(QIcon(":/images/document-saveas.png"));
    connect(saveAsAction,SIGNAL(triggered()),this,SLOT(saveAs()));

    for (int i=0;i<MaxRecentFiles;++i){
        recentFileActions[i]= new QAction(this);
        recentFileActions[i]->setVisible(false);
        connect(recentFileActions[i],SIGNAL(triggered()),this,SLOT(openRecentFile()));
    }

    exitAction=new QAction(tr("&Quitter"),this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Quitter"));
    exitAction->setIcon(QIcon(":/images/exit.png"));
    connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));

    copyAction=new QAction(tr("&Copier"),this);
    copyAction->setShortcut(tr("Ctrl+C"));
    copyAction->setStatusTip(tr("Copier"));
    copyAction->setIcon(QIcon(":/images/edit-copy.png"));
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copy()));

    cutAction=new QAction(tr("Co&uper"),this);
    cutAction->setShortcut(tr("Ctrl+X"));
    cutAction->setStatusTip(tr("Couper"));
    cutAction->setIcon(QIcon(":/images/edit-cut.png"));
    connect(cutAction,SIGNAL(triggered()),this,SLOT(cut()));

    pasteAction=new QAction(tr("C&oller"),this);
    pasteAction->setShortcut(tr("Ctrl+V"));
    pasteAction->setStatusTip(tr("Coller"));
    pasteAction->setIcon(QIcon(":/images/edit-paste.png"));
    connect(pasteAction,SIGNAL(triggered()),this,SLOT(paste()));

    undoAction=new QAction(tr("A&nnuler"),this);
    undoAction->setShortcut(tr("Ctrl+Z"));
    undoAction->setStatusTip(tr("Annuler"));
    undoAction->setIcon(QIcon(":/images/edit-undo.png"));
    connect(undoAction,SIGNAL(triggered()),this,SLOT(undo()));

    redoAction=new QAction(tr("&Rétablir"),this);
    redoAction->setShortcut(tr("Maj+Ctrl+Z"));
    redoAction->setStatusTip(tr("Rétablir"));
    redoAction->setIcon(QIcon(":/images/edit-redo.png"));
    connect(redoAction,SIGNAL(triggered()),this,SLOT(redo()));

    evaluateAction=new QAction(tr("&Evaluer"),this);
    evaluateAction->setShortcut(tr("Shift+Entrée"));
    evaluateAction->setStatusTip(tr("Evaluer"));
    evaluateAction->setIcon(QIcon(":/images/evaluate.png"));
    connect(evaluateAction,SIGNAL(triggered()),this,SLOT(evaluate()));

    aboutAction=new QAction(tr("&A propos"),this);
    aboutAction->setStatusTip(tr("Principales informations concernant QCAS"));
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(about()));

    prefAction=new QAction(tr("&Préférences..."),this);
    prefAction->setStatusTip(tr("Afficher la fenêtre des préférences"));
    connect(prefAction,SIGNAL(triggered()),this,SLOT(pref()));
}
void MainWindow::createToolBars(){
    toolBar=new QToolBar;
    toolBar->addAction(copyAction);
    toolBar->addAction(cutAction);
    toolBar->addAction(pasteAction);
    toolBar->addAction(undoAction);
    toolBar->addAction(redoAction);
    toolBar->addAction(evaluateAction);
    toolBar->setOrientation(Qt::Vertical);

}
void MainWindow::createMenus(){
    fileMenu=menuBar()->addMenu(tr("&Fichier"));
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    separatorAction=fileMenu->addSeparator();
    for (int i=0;i<MaxRecentFiles;++i){
        fileMenu->addAction(recentFileActions[i]);
    }
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    optionsMenu=menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(prefAction);

    helpMenu=menuBar()->addMenu(tr("&Aide"));
    helpMenu->addAction(aboutAction);
}
void MainWindow::createContextMenu(){


}

void MainWindow::newFile(){
    if (okToContinue()){

        setCurrentFile("");
    }

}
bool MainWindow::okToContinue(){
    if (isWindowModified()){
        int r=QMessageBox::warning(this,tr("Espace de travail"),
                                   tr("Le document a été modifié.\n"
                                      "Voulez-vous sauvegarder les modifications?"),
                                   QMessageBox::Yes|QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel||QMessageBox::Escape);
        if (r==QMessageBox::Yes){
            return save();
        }
        else if (r==QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

void MainWindow::open(){
    if (okToContinue()){
        QString fileName=QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"),tr("QCAS files (*.qcas)"));
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::loadFile(const QString &fileName){
    return true;
}
bool MainWindow::save(){
    if (curFile.isEmpty())
        return saveAs();
    else
        return saveFile(curFile);
}
bool MainWindow::saveFile(const QString &fileName){
    return true;
}
bool MainWindow::saveAs(){
    QString fileName=QFileDialog::getSaveFileName(this,tr("Enregistrer sous..."),tr("QCAS files (*.qcas)"));
    if (fileName.isEmpty()){
        return false;
    }
    return saveFile(fileName);
}

void MainWindow::setCurrentFile(const QString &fileName){
    curFile=fileName;
    setWindowModified(false);
    QString shownName="Sans titre";

    if (!curFile.isEmpty()){
        shownName=strippedName(curFile);
        recentFiles.removeAll(curFile);
        recentFiles.prepend(curFile);
        updateRecentFileActions();
        setWindowTitle(tr("%1[*] - %2").arg(shownName.arg(tr("QCAS"))));

    }
}

QString MainWindow::strippedName(const QString &fullFileName){
  return QFileInfo(fullFileName).fileName();

}

void MainWindow::updateRecentFileActions(){
    QMutableStringListIterator i(recentFiles);
    while(i.hasNext()){
        if (!QFile::exists(i.next()))
            i.remove();
    }
    for (int j=0;j<MaxRecentFiles;++j){
        if (j<recentFiles.count()){
            QString text=tr("&%1 %2").arg(j+1).arg(strippedName(recentFiles[j]));
            recentFileActions[j]->setText(text);
            recentFileActions[j]->setData(recentFiles[j]);
            recentFileActions[j]->setVisible(true);
        }
        else{
            recentFileActions[j]->setVisible(false);
        }
    }
    separatorAction->setVisible((!recentFiles.isEmpty()));
}

void MainWindow::openRecentFile(){
    if (okToContinue()){
        QAction *action=qobject_cast<QAction *>(sender());
            if (action)
                loadFile(action->data().toString());
    }
}

void MainWindow::createGui(){
    createWizards();

    tabPages=new MainTabWidget(this);
    QSplitter *vertSplit=new QSplitter(Qt::Horizontal);

    QWidget *leftToppanel=new QWidget;

    QHBoxLayout *h=new QHBoxLayout;
    h->addWidget(wizardList);
    h->addWidget(wizardPages,1);
    QVBoxLayout *leftLayout=new QVBoxLayout;
    leftLayout->addLayout(h);
    leftLayout->addStretch(1);
    leftToppanel->setLayout(leftLayout);

/*    QsciScintilla *text=new QsciScintilla;

    text->setLexer(new QsciLexerJava);
    text->setFolding(QsciScintilla::BoxedFoldStyle,2);

    text->setAutoIndent(true);
    text->setBraceMatching(QsciScintilla::SloppyBraceMatch);
*/
    QWidget *rightPanel=new QWidget;

    QHBoxLayout *rightLayout=new QHBoxLayout;

/*  QGridLayout *right=new QGridLayout;
  right->addWidget(mainWidgetList,0,0,Qt::AlignTop);
    right->addWidget(mainWidgetPages,1,0,Qt::AlignTop);
    right->addWidget(toolBar,1,1,Qt::AlignTop);*/

    rightLayout->addWidget(tabPages);
    rightLayout->addWidget(toolBar);
 //  rightLayout->addStretch(1);
    rightPanel->setLayout(rightLayout);



    this->setCentralWidget(vertSplit);
    vertSplit->addWidget(leftToppanel);
    vertSplit->addWidget(rightPanel);

    vertSplit->setStretchFactor(0,1);
    vertSplit->setStretchFactor(1,8);

}

void MainWindow::createWizards(){
    wizardList=new QListWidget;
    wizardList->setViewMode(QListView::IconMode);
    wizardList->setIconSize(QSize(60,60));
    wizardList->setSpacing(12);
    wizardList->setResizeMode(QListView::Adjust);
    wizardList->setMovement(QListView::Static);
    wizardList->setFixedWidth(105);

    wizardPages=new QStackedWidget;
    wizardPages->addWidget(new WizardMatrix(this));
    wizardPages->addWidget(new WizardEquation(this));
    wizardPages->addWidget(new WizardCatalog(this));

    QListWidgetItem *matrixItem=new QListWidgetItem(QIcon(":/images/matrix.png"),tr("Matrices"),wizardList);
    matrixItem->setTextAlignment(Qt::AlignHCenter);
    matrixItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    QListWidgetItem *equationItem=new QListWidgetItem(QIcon(":/images/equation.png"),tr("Equations"),wizardList);
    equationItem->setTextAlignment(Qt::AlignHCenter);
    equationItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    QListWidgetItem *catalogItem=new QListWidgetItem(QIcon(":/images/book.png"),tr("Catalogue"),wizardList);
    catalogItem->setTextAlignment(Qt::AlignHCenter);
    catalogItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    connect(wizardList,SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),this,SLOT(changeWizard(QListWidgetItem*,QListWidgetItem*)));

}

void MainWindow::changeWizard(QListWidgetItem *current, QListWidgetItem *previous){
    if (!current)
        current=previous;
    wizardPages->setCurrentIndex(wizardList->row(current));
}
void MainWindow::about(){
    QMessageBox::about(this,tr("A propos de QCAS"),tr("<h2>QCAS </h2>"
                                                      "patati patata"));

}
void MainWindow::closeEvent(QCloseEvent *event){
    if (okToContinue()){
        writeSettings();
        event->accept();
    }
    else{
        event->ignore();
    }
}

void MainWindow::pref(){

}
void MainWindow::readSettings(){

}

void MainWindow::writeSettings(){}

CasManager* MainWindow::getCas() const{
    return cas;
}
void MainWindow::copy(){
   MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
   switch(sheet->getType()){
   case MainSheet::FORMAL_TYPE:
       {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
           form->copy();}
       break;
   case MainSheet::SPREADSHEET_TYPE:
       break;
   case MainSheet::PROGRAMMING_TYPE:
       break;
   }
}
void MainWindow::cut(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
            form->cut();}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}
void MainWindow::paste(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
        form->paste();}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }

}
void MainWindow::undo(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
        form->undo();}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}

void MainWindow::redo(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
        form->redo();}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}
void MainWindow::evaluate(){
}
void MainWindow::displayHelp(const QString & keyWord) const{
    qDebug()<<keyWord;
   wizardList->setCurrentRow(2);
   (qobject_cast<WizardCatalog*>(wizardPages->currentWidget()))->displayPage(QUrl(keyWord));
}
void MainWindow::sendText(const QString & s){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
        form->sendText(s);}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}
