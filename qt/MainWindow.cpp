#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QSplitter>
#include <QCloseEvent>
#include <QDebug>
#include <QLabel>
#include <QToolBox>
#include <QWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QListWidgetItem>
#include <QStackedWidget>
#include <QStatusBar>
#include <QMenu>
#include <QScrollBar>
#include <QPlainTextEdit>
#include <QToolBar>
#include "MainWindow.h"
#include "gui/FormalSheet.h"
#include "gui/WizardEquation.h"
#include "gui/WizardMatrix.h"
#include "gui/WizardCatalog.h"
#include "gui/CentralTabWidget.h"
#include "gui/FormalLine.h"
#include "gui/FormalLineWidgets.h"
#include <QToolButton>
#include <QTime>
//#include "gui/FormalSheet.h"
//#include <Qsci/qsciscintilla.h>
//#include <Qsci/qscilexerjava.h>
#include <QUrl>
#include <QGroupBox>
#include "EvaluationThread.h"
#include <QCompleter>
#include <giac/giac.h>


/** MainWindow constructor

  **/


MainWindow::MainWindow(){
    displayTimeAfterProcess=true;
    time=new QTime;

    commandInfo=new CommandInfo;
    taskProperties.firstPrintMessage=true;
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


    stopButton=new QToolButton;
    connect(stopButton,SIGNAL(clicked()),this,SLOT(killThread()));
    stopButton->setIcon(QIcon(":/images/stop.png"));
    stopButton->setToolTip(tr("Interrompre le calcul en cours"));
    stopButton->setStatusTip(tr("Interrompre le calcul en cours"));



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

    QWidget *leftPanel=new QWidget;

    QHBoxLayout *h=new QHBoxLayout;
    h->addWidget(wizardList);
    h->addWidget(wizardPages,1);

    giacMessages=new QPlainTextEdit;
    giacMessages->setReadOnly(true);
    QPalette p=giacMessages->palette();
    p.setColor(QPalette::Base,QColor::fromRgb(251,251,113,128));
    giacMessages->setPalette(p);
    giacMessages->setMaximumBlockCount(1000);
    QGroupBox *giacPanel=new QGroupBox;

    giacPanel->setTitle(tr("Messages Giac"));
    QVBoxLayout *giacLayout=new QVBoxLayout;
    giacLayout->addWidget(giacMessages);
    giacPanel->setLayout(giacLayout);

    QVBoxLayout *leftLayout=new QVBoxLayout;
    leftLayout->addLayout(h);
    leftLayout->addStretch(1);
    leftLayout->addWidget(giacPanel);
    leftPanel->setLayout(leftLayout);





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
    vertSplit->addWidget(leftPanel);
    vertSplit->addWidget(rightPanel);

    vertSplit->setStretchFactor(0,1);
    vertSplit->setStretchFactor(1,8);

    warningFirstEvaluation=new QLabel(tr("<b><font color=\"red\">Shift+Entrée pour évaluer</font></b>"));
    warningFirstEvaluation->setAlignment(Qt::AlignRight);
    warningFirstEvaluation->setIndent(20);
    warningFirstEvaluation->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    statusBar()->addWidget(warningFirstEvaluation,1);
    warningFirstEvaluation->show();
    connect(&ev,SIGNAL(finished()),this,SLOT(displayResult()));
}
void MainWindow::printHeader(){
    if (ev.getGiacDisplay().isEmpty()&&!displayTimeAfterProcess) return;

    if (taskProperties.firstPrintMessage) {
        QString title=tr("Feuille ")+QString::number(taskProperties.currentSheet+1)+tr(", ligne ")+QString::number(taskProperties.currentLine+1)+":";
        giacMessages->appendHtml("<b><u><font color=\"#40A497\">"+title+"</font></u></b>");
        taskProperties.firstPrintMessage=false;
    }

}

void MainWindow::displayGiacMessages(){
    printHeader();
    QStringList list=ev.getGiacDisplay();
    for (int  i=0;i<list.count();++i){
        giacMessages->appendHtml(list.at(i));
    }
    giacMessages->appendHtml(tr("<br><font color=\"gray\">Temps mis:")+QString::number(time->elapsed())+" ms</font><br>");
    giacMessages->verticalScrollBar()->setValue(giacMessages->verticalScrollBar()->maximum());
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
void MainWindow::killThread(){

    ev.killThread();
}
void MainWindow::evaluate(const QString &formula){
    if (warningFirstEvaluation!=NULL) {
        statusBar()->removeWidget(warningFirstEvaluation);
        delete warningFirstEvaluation;
        warningFirstEvaluation=NULL;
    }

    taskProperties.firstPrintMessage=true;
    taskProperties.currentSheet=tabPages->currentIndex();
    taskProperties.currentLine=-1;
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
            form->getCurrentLine()->addStopButton(stopButton);
            taskProperties.currentLine=form->getCurrentLine()->getId();
            EvaluationThread::warning id=ev.setCommand(formula);
            if (id==EvaluationThread::WARNING){
                QString s("<font color=\"red\"");
                s.append(tr("Attention! <br> Pour affecter une valeur à une variable, vous devez utiliser le symbole :="));
                s.append("<br><u>Exemple</u>: a:=2 ou f(x):=x^2");
                s.append("</font>");
                printHeader();
                giacMessages->appendHtml(s);
                form->getCurrentLine()->getTextInput()->setFocus();
            }
            ev.start();
            time->start();
        }
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}
void MainWindow::displayResult(){
    displayGiacMessages();
    tabPages->setCurrentIndex(taskProperties.currentSheet);
    // Formal Sheet
    if (taskProperties.currentLine>-1){
        FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->widget(taskProperties.currentSheet));

        form->removeStop(taskProperties.currentLine);
        stopButton->setParent(this);

//        return form->getLineAt(taskProperties.currentLine)->getOuputWidget();
        form->displayResult(taskProperties.currentLine,ev.displayResult());

    }
}
QToolButton* MainWindow::getStopButton() const{
    return stopButton;
}


void MainWindow::evaluate(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
                evaluate(form->getCurrentLine()->getTextInput()->toPlainText());
            }
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }

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

bool MainWindow::isEvaluating(){
    return ev.isRunning();
}
CommandInfo* MainWindow::getCommandInfo()const{
return commandInfo;
}
CommandInfo::CommandInfo(){
    listAllCommands();
    completer=new QCompleter(commandList);

}
void CommandInfo::listAllCommands(){
    QFile file("aide_cas");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString line;
    // Only take command with alpanumeric characters
    // For example %{ %} is excluded.

    QRegExp expr("([a-z]|[A-Z]|[_]|[0-9])+");

    while(!stream.atEnd()){
        line=stream.readLine();
        if (line.startsWith("#")) {
            QStringList list=line.remove(0,2).split(" ",QString::SkipEmptyParts);
            for(int i=0;i<list.size();++i){
                QString s(list.at(i));
                if (expr.exactMatch(s))
                    commandList.append(list.at(i));
            }
        }
    }
    commandList.sort();
}



QString CommandInfo::displayPage(const QString& keyWord) const{
    QFile file(":/aide_cas");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString command,description;
    QStringList synonym,seeAlso,examples;
    QString line;
    while(!stream.atEnd()){
        line=stream.readLine();
        if (line.startsWith("#")&& (line.contains(" "+keyWord+" ",Qt::CaseSensitive)||line.endsWith(" "+keyWord,Qt::CaseSensitive))) {
            QStringList list=line.split(" ",QString::SkipEmptyParts);
            for(int i=0;i<list.size();++i){
                if (i==1) command=list.at(1);
                else if (i>1) synonym.append(list.at(i));
            }
            do{
                line=stream.readLine();
                if (line.startsWith("#")) break;
                if (line.startsWith("0")){
                    list=line.split(" ",QString::SkipEmptyParts);
                    if (list.size()>1){
                        command.append("(").append(list.at(1)).append(")");
                    }
                }
                else if (line.startsWith("1")){
                    description=line.remove(0,2);
                }
                else if (line.startsWith("-")){
                    list=line.split(" ",QString::SkipEmptyParts);
                    if (list.size()>1) seeAlso.append(list.at(1));
                }
                else if (line.length()>0 ){
                    QChar c=line.at(0);
                    if (!QString("123456789").contains(c))
                     examples.append(line);
                }
            }  while (!stream.atEnd());
            break;
        }
        }
        line.clear();
        line.append("<h3><font color=\"#40A497\">").append(command).append("</font></h3>");
        if (!synonym.isEmpty()){
            line.append("<b>").append(QObject::tr("Synonyme(s):")).append("</b>");
            for( int i=0;i<synonym.size();++i){
                line.append(" ").append(synonym.at(i));
            }
        }
        line.append("<hr>");
        line.append(description);
        line.append("<br>");
        if (!examples.isEmpty()){
            line.append("<br><b>").append(QObject::tr("Exemples:")).append("</b><br>");
            for( int i=0;i<examples.size();++i){
                line.append(examples.at(i)).append("<br>");
            }
        }
        if (!seeAlso.isEmpty()){
            line.append(QObject::tr("Voir aussi: ")).append("<ul>");
            for( int i=0;i<seeAlso.size();++i){
                line.append("<li><a href=\"").append(seeAlso.at(i)).append("\">").append(seeAlso.at(i)).append("</a></li>");
            }
            line.append("</ul>");
        }
        return(line);
    }

QString CommandInfo::seekForKeyword(const QString & keyWord) const{
    QFile file(":/aide_cas");
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        QString currentCommand;
        QString html;
        QString line;
        while(!stream.atEnd()){
            line=stream.readLine();
            if (line.startsWith("#")) currentCommand=line.remove(0,2);
            else if (line.startsWith("1")&&
                     (line.contains(keyWord,Qt::CaseInsensitive)||currentCommand.contains(keyWord,Qt::CaseInsensitive))){
                html.append("<a href=\"");
                html.append(currentCommand);
                html.append("\">");
                html.append(currentCommand);
                html.append("</a><br>\n");
                html.append(line.remove(0,2));
                html.append("<br><br>");
            }
        }
        if (html.isEmpty()) {
            return(QObject::tr("Aucun résultat"));
        }
        else return(html);

    }
bool CommandInfo::isCommand(const QString &st) const{
    return (commandList.contains(st));
}
QCompleter* CommandInfo::getCompleter() const{
    return completer;
}
QStringList CommandInfo::getCommands(){
    return commandList;
}

