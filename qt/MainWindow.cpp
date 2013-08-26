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
#include <QSettings>
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
# include <QDomDocument>
# include <QDomText>
#include "output.h"


#include "CasManager.h"
#include <QCompleter>
#include <giac/giac.h>
#include "gui/prefdialog.h"

#include "qt/gui/plotfunctiondialog.h"
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

    setWindowIcon(QIcon(":/images/icon.png"));

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
    //saveAsAction->setShortcut(tr("Ctrl+Maj+S"));//Gives a conflict with Ctrl+S
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
    redoAction->setShortcut(tr("Shift+Ctrl+Z"));
    redoAction->setStatusTip(tr("Rétablir"));
    redoAction->setIcon(QIcon(":/images/edit-redo.png"));
    connect(redoAction,SIGNAL(triggered()),this,SLOT(redo()));

    insertlineAction=new QAction(tr("&Nouvelle Entrée"),this);
    insertlineAction->setShortcut(tr("Ctrl+I"));
    insertlineAction->setStatusTip(tr("Nouvelle Entrée"));
    insertlineAction->setIcon(QIcon(":/images/add.png"));
    connect(insertlineAction,SIGNAL(triggered()),this,SLOT(insertline()));

    deleteLevelAction=new QAction(tr("&Effacer les lignes sélectionnées"),this);
    deleteLevelAction->setStatusTip(tr("Efface les niveaux sélectionnés"));
    deleteLevelAction->setIcon(QIcon(":/images/delete.png"));
    connect(deleteLevelAction,SIGNAL(triggered()),this,SLOT(deleteSelectedLevels()));


    evaluateAction=new QAction(tr("&Evaluer"),this);
    evaluateAction->setShortcut(tr("Shift+Entrée"));
    evaluateAction->setStatusTip(tr("Evaluer"));
    evaluateAction->setIcon(QIcon(":/images/evaluate.png"));
    connect(evaluateAction,SIGNAL(triggered()),this,SLOT(evaluate()));

    htmlhelpAction=new QAction(tr("&Aide Html"),this);
    htmlhelpAction->setStatusTip(tr("Aide html de Giac/Qcas"));
    connect(htmlhelpAction,SIGNAL(triggered()),this,SLOT(htmlhelp()));

    aboutAction=new QAction(tr("&A propos"),this);
    aboutAction->setStatusTip(tr("Principales informations concernant QCAS"));
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(about()));


    stopButton=new QToolButton;
    connect(stopButton,SIGNAL(clicked()),this,SLOT(killThread()));
    stopButton->setIcon(QIcon(":/images/stop.png"));
    stopButton->setToolTip(tr("Interrompre le calcul en cours"));
    stopButton->setStatusTip(tr("Interrompre le calcul en cours"));



    prefAction=new QAction(tr("&Préférences..."),this);
    prefAction->setIcon(QIcon(":/images/configure.png"));
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
    toolBar->addAction(insertlineAction);
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

    editMenu=menuBar()->addMenu(tr("Edition"));
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addAction(insertlineAction);
    editMenu->addAction(deleteLevelAction);

    optionsMenu=menuBar()->addMenu(tr("&Options"));
    optionsMenu->addAction(prefAction);

    helpMenu=menuBar()->addMenu(tr("&Aide"));
    helpMenu->addAction(htmlhelpAction);
    helpMenu->addAction(aboutAction);
}
void MainWindow::createContextMenu(){


}

/**
 * @brief MainWindow::clearWorkspace
 *        Purge all variable in Giac.
 *        Delete all tabs.
 *
 */
void MainWindow::clearWorkspace(){

    for (int i=tabPages->count()-2;i>=0;--i){
        tabPages->removeTab(i);
    }
    delete cas;
    cas=new CasManager(this);

    tabPages->addFormalSheet();
}


void MainWindow::newFile(){
    if (okToContinue()){
        clearWorkspace();
        setCurrentFile("");
    }


}
bool MainWindow::okToContinue(){
    if (isWindowModified()){
        /* PB: gives OK, NO, YES and never understand save or cancel.
        int r=QMessageBox::warning(this,tr("Espace de travail"),
                                   tr("Le document a été modifié.\n"
                                      "Voulez-vous sauvegarder les modifications?"),
                                   QMessageBox::Yes|QMessageBox::Default,
                                   QMessageBox::No,
                                   QMessageBox::Cancel||QMessageBox::Escape);
        if (r==QMessageBox::Yes){
            return save();
        */
        QMessageBox msgBox;
        msgBox.setWindowTitle("Espace de travail");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setText(tr("Le document a été modifié."));
        msgBox.setInformativeText(tr("Voulez-vous sauvegarder les modifications?"));
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int r = msgBox.exec();
        if (r==QMessageBox::Save){
            return MainWindow::save();
        }
        else if (r==QMessageBox::Cancel){
            return false;
        }
    }
    return true;
}

void MainWindow::open(){
    if (okToContinue()){
        clearWorkspace();
        QString fileName=QFileDialog::getOpenFileName(this, tr("Ouvrir un fichier"),".qcas",tr("QCAS or Giac/Xcas files (*.qcas *.cas *.xws)"));
        if (!fileName.isEmpty()){
	    loadFile(fileName);
	}
    }
}

bool MainWindow::loadFile(const QString &fileName){
      if (!fileName.isEmpty()){
 	  if (fileName.endsWith(".cas")||(fileName.endsWith(".xws"))){
	    return loadGiacFile(fileName);
 	  }
 	  else 
	    return loadQcasFile(fileName);
      }
}
bool MainWindow::loadQcasFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QDomDocument doc("xml");

    QDataStream dataIn(&file);
    QByteArray compByteArray;
    dataIn >> compByteArray;
    QByteArray xmlByteArray = qUncompress(compByteArray);
    QString xmlString =QString::fromUtf8(xmlByteArray.data(), xmlByteArray.size());
    //qDebug()<<xmlString;//too slow with big files
    if (!doc.setContent(xmlString)){
        file.close();
        return false;
    }
    file.close();


     QDomElement root = doc.documentElement();
     if (root.tagName()!="qcas") return false;

    int tabBeforeLoad=tabPages->count();

     QDomNode node = root.firstChild();
     while(!node.isNull()){
        QDomElement sheet = node.toElement(); // try to convert the node to an element.
         if(!sheet.isNull()) {
             if (sheet.tagName()=="formal"){
                tabPages->addFormalSheet();
                FormalWorkSheet *f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
                QDomNode first=sheet.firstChild();
                bool goToNextLine=true;

                while(!first.isNull()){
                    QDomElement element=first.toElement();
                    if (!element.isNull()) {
                        QString tag=element.tagName();
                        if (tag=="command"){
                            if (goToNextLine) goToNextLine=false;
                            else {
                                f->goToNextLine();
                            }
                            f->sendText(element.text());

                       }
                        else if (tag=="formula"){
                            giac::gen g(element.text().toStdString(),getContext());
                            FormulaWidget* formWidget=new FormulaWidget(g,getContext());
                              f->displayResult(f->getCurrent(),formWidget);
                              goToNextLine=true;
                        }
                        else if (tag=="graph2d"){
                            GraphWidget* graph=new GraphWidget(getContext(),false,this);
                            graph->loadXML(element);
                            graph->updateAllCategories();

                            f->displayResult(f->getCurrent(),graph);

                            goToNextLine=true;

                        }
                    }
                    first=first.nextSibling();
                }
             }
             else if (sheet.tagName()=="interactive2d"){
                 tabPages->addG2dSheet();
                 GraphWidget *f=qobject_cast<GraphWidget*>(tabPages->widget(tabPages->count()-2));
                 f->loadInteractiveXML(sheet);
             }
             else if(sheet.tagName()=="settings"){
                 QDomNode first=sheet.firstChild();
                 while(!first.isNull()){
                     QDomElement element=first.toElement();
                     if (!element.isNull()) {
                         QString tag=element.tagName();
                         if (tag=="cas") cas->loadXML(element);
                         else {
			   if (tag=="general") { cas->loadGeneralXML(element);}
			   else{ if(tag=="context"){cas->loadXML(element,true);}//load giac context
			   }
			 }
		     }
                     first=first.nextSibling();
                 }
             }
         }
         node= node.nextSibling();
     }

    setCurrentFile(fileName);
    return true;
}

bool MainWindow::loadGiacFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return false;
    QTextStream dataIn(&file);
    if (fileName.endsWith(".xws")){
    //xcas .xws file
	QString xcasline = dataIn.readLine();
	QString fltktag="newformal";//with this flag, a formal sheet must be opened before sending a formal line
	FormalWorkSheet *f=0;//
	GraphWidget * g2d=0;
	while (!xcasline.isNull()&&!xcasline.startsWith("// context")) {
	  //qDebug()<<xcasline;
	  if(xcasline.startsWith("// fltk")){
	    if((xcasline.contains("_Tile"))||(xcasline.contains("History"))){
		  //drop those cases.
		  xcasline=dataIn.readLine();//drop the bracket
	    }
	    if((xcasline.contains("Equation"))||(xcasline.contains("Output"))||(xcasline.contains("Mouse"))||(xcasline.contains("Button"))||(xcasline.contains("History"))||(xcasline.contains("Menu"))||(xcasline.contains("Scroll"))||(xcasline.contains("N4xcas7Graph3dE"))||(xcasline.contains("N4xcas7Graph2dE"))){
		  //drop those cases.
		  xcasline=dataIn.readLine();//drop the nextline (results)
	    }
	    if(xcasline.contains("N4xcas6FigureE")){
	      //Open Interactive 2D geometry
	      fltktag="Geo2D";
	      tabPages->addG2dSheet();
	      g2d=qobject_cast<GraphWidget*>(tabPages->widget(tabPages->count()-2));
	      //g2d interactive not ready, convert to formal.
	      //tabPages->addFormalSheet();
	      //f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
	    }
	    if(xcasline.contains("N4xcas5Geo2dE")){
	      //quit geo 2d later.
	      xcasline=dataIn.readLine();//drop the next line
	      fltktag="newformal";
	      //fin 
	    }
	    //Fixme: add the spreadsheet and other non formal case before new formal.
	    
	    if(xcasline.contains("Comment")){
	      xcasline=dataIn.readLine();
	      xcasline.prepend("/* ");
	      xcasline.append(" */");
	      //xcasline.replace('£',"\n");//FIXME: Problem with the newline symbol
	      if(fltktag=="newformal"){
		fltktag="";
		qDebug()<<xcasline;
		tabPages->addFormalSheet();
		f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
	      }
	      f->sendText(xcasline);
	      f->goToNextLine();
	    }
	    if((xcasline.contains("N4xcas7EditeurE"))||(xcasline.contains("Xcas_Text_EditorE"))){
	      int nbcar=(dataIn.readLine()).remove(",").toInt();
	      if(nbcar>0){
		xcasline=dataIn.read(nbcar);
		if(fltktag=="Geo2D"){
		  g2d->sendText(xcasline);
		}
		else{
		  if(fltktag=="newformal"){
		    fltktag="";
		    qDebug()<<xcasline;
		    tabPages->addFormalSheet();
		    f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
		  }
		  f->sendText(xcasline);
		  f->goToNextLine();
		}
	      }
	    }
	  }
	  else{   
	    if((fltktag=="")||(fltktag=="newformal")){
	      if((xcasline!="]")&&(xcasline!="[")&&(xcasline!=",")&&(xcasline!="")){
		if(fltktag=="newformal"){
		  fltktag="";
		  qDebug()<<xcasline;
		  tabPages->addFormalSheet();
		  f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
		}
		f->sendText(xcasline);
		f->goToNextLine();
	      }
	    }
	    if(fltktag=="Geo2D"){
	      if((xcasline!="]")&&(xcasline!="[")&&(xcasline!=",")&&(xcasline!=(""))){
		//if (!(xcasline.contains(QRegExp(";\s*$")))){
		//    xcasline.append(";");
		//  }
	        g2d->sendText(xcasline);
	      }
	    }
	  }
	  xcasline = dataIn.readLine();
	}


      }
    else{
    //giac .cas file
        QString giacline = dataIn.readLine();
	tabPages->addFormalSheet();
	FormalWorkSheet *f=qobject_cast<FormalWorkSheet*>(tabPages->widget(tabPages->count()-2));
	while (!giacline.isNull()) {
	  f->sendText(giacline);
	  f->goToNextLine();
	  giacline = dataIn.readLine();
	}
      }
    file.close();
    //setCurrentFile(fileName.replace(".cas",".qcas"));PB si le fichier existe deja.
    return true;
}



bool MainWindow::save(){
  if ((curFile.isEmpty())||!(curFile.endsWith(".qcas")))
        return saveAs();
    else
        return saveFile(curFile);
}
bool MainWindow::saveFile(const QString &fileName){

    QDomDocument doc;

    QDomElement root=doc.createElement("qcas");
    // write cas configuration
    cas->toXML(root,true);// the true option save also the  giac context
    for (int i=0;i<tabPages->count()-1;++i){
        MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->widget(i));
        switch(sheet->getType()){
        case MainSheet::FORMAL_TYPE:
            {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->widget(i));
            form->toXML(root);
        }
            break;
        case MainSheet::SPREADSHEET_TYPE:
            break;
        case MainSheet::PROGRAMMING_TYPE:
            break;
        case MainSheet::G2D_TYPE:
        {GraphWidget *graph=qobject_cast<GraphWidget*>(tabPages->widget(i));
        graph->toInteractiveXML(root);
    }
         break;

        }

   }
    doc.appendChild(root);
//    qDebug()<<root.oString();
//    qDebug()<<doc.toString();
//    qDebug()<<"Nom du fichier"<<fileName;
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QString xmlString;
        QTextStream textOut(&xmlString);
        doc.save(textOut,1);
        QByteArray xmlByteArray = qCompress(xmlString.toUtf8());
        QDataStream dataOut(&file);
        dataOut.writeBytes(xmlByteArray.data(), xmlByteArray.size());

//   QTextStream out(&file);
//   doc.save(out, 4);

    file.close();
/*    -------------------------------------


    Uncompressing xml-Data to QDomDocument doc:
    ===========================================
    QDataStream dataIn(&inFile);
    QByteArray compByteArray;
    dataIn >> compByteArray;
    QByteArray xmlByteArray = qUncompress(compByteArray);
    QString xmlString =
        QString::fromUtf8(xmlByteArray.data(), xmlByteArray.size());
    doc.setContent(xmlString, &errorMsg, &errorLine, &errorColumn);


    ------------------------------
    file.open(QIODevice::ReadOnly);
    QByteArray ba = file.readAll();
    QFile out("test.gz");
         file.open(QIODevice::WriteOnly);
         QDataStream out(&amp;file);
         out << qCompress(ba);
  */


    }
    else
    return(false);





    setCurrentFile(fileName);

    return true;
}
bool MainWindow::saveToGiacFile(const QString &fileName){

    QDomElement rootold;
    QString root;
    // TODO: write cas configuration
    for (int i=0;i<tabPages->count()-1;++i){
        MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->widget(i));
        switch(sheet->getType()){
        case MainSheet::FORMAL_TYPE:
            {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->widget(i));
            if(fileName.endsWith(".cas"))
	      form->toGIAC(root);
	    else
	      form->toXCAS(root);
        }
            break;
        case MainSheet::SPREADSHEET_TYPE:
            break;
        case MainSheet::PROGRAMMING_TYPE:
            break;
        case MainSheet::G2D_TYPE:
        {GraphWidget *graph=qobject_cast<GraphWidget*>(tabPages->widget(i));
	      if(fileName.endsWith(".xws"))
	          graph->toInteractiveXCAS2D(root);
	    
	}
         break;

        }

    }
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        QTextStream out(&file);
	out<<root;
	file.close();
    }
    else
    return(false);

    setCurrentFile(fileName);
    setWindowModified(true);

    return true;
}
bool MainWindow::saveAs(){

  QFileDialog rep(this,tr("Enregistrer sous..."),"",tr("QCAS files (*.qcas);;XCAS files (*.xws);;GIAC files (*.cas)"));
  rep.setAcceptMode(QFileDialog::AcceptSave);

  if(rep.exec()){
     
     QString fileName = rep.selectedFiles()[0];
     if((fileName.endsWith(".xws"))||(fileName.endsWith(".cas")))
           return saveToGiacFile(fileName);
     else{
        if(fileName.endsWith(".qcas"))
	   return saveFile(fileName);
        else{
	  if (rep.selectedNameFilter() == "XCAS files (*.xws)")
	    return saveToGiacFile(fileName.append(".xws"));
	  if (rep.selectedNameFilter() == "GIAC files (*.cas)")
	    return saveToGiacFile(fileName.append(".cas"));
	  else
	    return saveFile(fileName.append(".qcas"));
	}
     }
  }
  else
    return false;
}

bool MainWindow::saveAsgiacxcas(){
    QString fileName=QFileDialog::getSaveFileName(this,tr("Exporter vers..."),".xws",tr("XCAS files (*.xws);;GIAC files (*.cas)"));
    if (fileName.isEmpty()){
        return false;
    }
    return saveToGiacFile(fileName);
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
        setWindowTitle(tr("%1[*] - %2").arg(shownName).arg("QCAS"));

    }
    else setWindowTitle("QCAS[*]");
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
        if (j<recentFiles.size()){
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


    leftPanel=new QWidget;

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
    leftPanel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);



/*    QsciScintilla *text=new QsciScintilla;

    text->setLexer(new QsciLexerJava);
    text->setFolding(QsciScintilla::BoxedFoldStyle,2);

    text->setAutoIndent(true);
    text->setBraceMatching(QsciScintilla::SloppyBraceMatch);
*/
    rightPanel=new QWidget(this);

    QHBoxLayout *rightLayout=new QHBoxLayout;

/*  QGridLayout *right=new QGridLayout;
  right->addWidget(mainWidgetList,0,0,Qt::AlignTop);
    right->addWidget(mainWidgetPages,1,0,Qt::AlignTop);
    right->addWidget(toolBar,1,1,Qt::AlignTop);*/

    rightLayout->addWidget(tabPages);
    rightLayout->addWidget(toolBar);
 //  rightLayout->addStretch(1);
    rightPanel->setLayout(rightLayout);

    setCentralWidget(vertSplit);

    vertSplit->addWidget(leftPanel);
    vertSplit->addWidget(rightPanel);

//    vertSplit->setStretchFactor(0,1);
    vertSplit->setStretchFactor(1,8);

    labelStatus=new QLabel;
    labelStatus->setAlignment(Qt::AlignRight);
    labelStatus->setIndent(20);
    labelStatus->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    displayInStatusBar(tr("Shift+Entrée pour évaluer"),"red");
    statusBar()->addWidget(labelStatus,1);
    labelStatus->show();



 /*   warningFirstEvaluation=new QLabel(tr("<b><font color=\"red\">Shift+Entrée pour évaluer</font></b>"));
    warningFirstEvaluation->setIndent(20);
    warningFirstEvaluation->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    statusBar()->addWidget(warningFirstEvaluation,1);
    warningFirstEvaluation->show();


    warningStop=new QLabel(tr("<b><font color=\"red\">Tentative d'interruption...</font></b>"));
    warningStop->setAlignment(Qt::AlignRight);
    warningStop->setIndent(20);
    warningStop->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    warningStop->setParent(0);
*/

    prefDialog=new PrefDialog(this);
    cas=new CasManager(this);
//    connect(&ev,SIGNAL(finished()),this,SLOT(displayResult()));
}
void MainWindow::printHeader(){
    if (cas->getGiacDisplay().isEmpty()&&!displayTimeAfterProcess) return;

    if (taskProperties.firstPrintMessage) {
        QString title=tr("Feuille ")+QString::number(taskProperties.currentSheet+1)+tr(", ligne ")+QString::number(taskProperties.currentLine+1)+":";
        giacMessages->appendHtml("<b><u><font color=\"#40A497\">"+title+"</font></u></b>");
        taskProperties.firstPrintMessage=false;
    }

}
void MainWindow::displayInStatusBar(const QString & text, const QString & color){
    labelStatus->setText(QString("<b><font color=\"%1\">%2</font></b>").arg(color,text));
}
void MainWindow::updateInterface(MainSheet::sheetType type){
    switch(type){
        case MainSheet::G2D_TYPE:    {
            giac::decimal_digits(3,getContext());
            copyAction->setVisible(false);
            cutAction->setVisible(false);
            pasteAction->setVisible(false);
            redoAction->setVisible(true);
            undoAction->setVisible(true);
            evaluateAction->setVisible(false);
            insertlineAction->setVisible(false);
            deleteLevelAction->setVisible(false);
            leftPanel->hide();
        }
        break;
        case MainSheet::FORMAL_TYPE:
        default:{
            giac::decimal_digits(decimalDigits,getContext());
            copyAction->setVisible(true);
            cutAction->setVisible(true);
            pasteAction->setVisible(true);
            redoAction->setVisible(true);
            undoAction->setVisible(true);
            insertlineAction->setVisible(true);
            deleteLevelAction->setVisible(true);
            evaluateAction->setVisible(true);
            leftPanel->show();
        }
    }
}
void MainWindow::setRedoButton(bool b){
    redoAction->setEnabled(b);
}
void MainWindow::setUndoButton(bool b){
    undoAction->setEnabled(b);
}
int MainWindow::getDecimalDigit()const{
    return decimalDigits;
}
void MainWindow::setDecimalDigits(const int &a){
    decimalDigits=a;
}

void MainWindow::displayGiacMessages(){
    printHeader();
    QStringList list=cas->getGiacDisplay();
    for (int  i=0;i<list.size();++i){
        giacMessages->appendHtml(list.at(i));
    }
    giacMessages->appendHtml(tr("<br><font color=\"gray\">Temps mis:")+QString::number(time->elapsed())+" ms</font><br>");
    giacMessages->verticalScrollBar()->setValue(giacMessages->verticalScrollBar()->maximum());
    cas->clearGiacDisplay();
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
//    wizardPages->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Ignored);

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
    QMessageBox::about(this,tr("A propos de QCAS"),tr("<center><h1>  QCAS</h1></center>"
                                                      "<hr>"
                                                      "<ul>"
                                                      "<li><b>Licence:</b> GPL </li>"
                                                      "<li><b>Version:</b> 0.2 </li>"
                                                      "<li><b>Sources de QCAS: </b><center><a href=\"http://git.tuxfamily.org/?p=qcas/qcas.git\">http://git.tuxfamily.org/?p=qcas/qcas.git</a></center><br></li>"
						      "<li><b>Binaires de QCAS: </b><center><a href=\"http://www.math.jussieu.fr/~han/qcas\">http://www.math.jussieu.fr/~han/qcas</a></center><br></li>"
                                                      "<li><b>Développeur de QCAS: </b>Loïc Le Coq <center><a href=\"mailto:loic@qcas.tuxfamily.org\"> loic@qcas.tuxfamily.org</a></center><br></li>"
                                                      "<li><b>Développeur de Giac/Xcas: </b>Bernard Parisse</li>"
                                                      "<li><b> Site Web de XCas:  <center><a href=\"http://www-fourier.ujf-grenoble.fr/~parisse/giac_fr.html\">http://www-fourier.ujf-grenoble.fr/~parisse/giac_fr.html</a></center><nr></li>"
                                                      "</ul><hr>"
                                                      ).append(QDate::currentDate().toString()));

}
void MainWindow::htmlhelp(){
    giac::system_browser_command("doc/index.html");
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
    prefDialog->initValue();
    prefDialog->setVisible(true);

}
void MainWindow::readSettings(){
    QSettings settings("http://qcas.tuxfamily.org","QCAS");
    QRect rect=settings.value("geometry",QRect(200,200,600,400)).toRect();
    move(rect.topLeft());
    resize(rect.size());

    recentFiles=settings.value("recentFiles").toStringList();
    updateRecentFileActions();
}

void MainWindow::writeSettings(){
    QSettings settings("http://qcas.tuxfamily.org","QCAS");
    settings.setValue("geometry",geometry());
    settings.setValue("recentFiles",recentFiles);

}

void MainWindow::copy(){
   MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
   switch(sheet->getType()){
   case MainSheet::FORMAL_TYPE:
       {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
           form->copy();}
       break;
   case MainSheet::SPREADSHEET_TYPE:
   case MainSheet::G2D_TYPE:
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
    case MainSheet::G2D_TYPE:
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
    case MainSheet::G2D_TYPE:
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }

}
void MainWindow::undo(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {

        FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
        form->undo();}
        break;
    case MainSheet::SPREADSHEET_TYPE:
        break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
     case MainSheet::G2D_TYPE:
    {
        GraphWidget* graph=qobject_cast<GraphWidget*>(tabPages->currentWidget());
        graph->undo();
    }
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
     case MainSheet::G2D_TYPE:
    {
        GraphWidget* graph=qobject_cast<GraphWidget*>(tabPages->currentWidget());
        graph->redo();
    }
     break;
    }
}
void MainWindow::insertline(){
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
    {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
            form->insertline();}
    break;
    case MainSheet::SPREADSHEET_TYPE:
    case MainSheet::G2D_TYPE:
    case MainSheet::PROGRAMMING_TYPE:
    break;
    }
}
void MainWindow::deleteSelectedLevels(){
    FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
    if (form==0) return;
    form->deleteSelectedLevels();

}

void MainWindow::killThread(){

    cas->killThread();
}
void MainWindow::evaluate(const QString &formula){
/*    if (warningFirstEvaluation!=NULL) {
        statusBar()->removeWidget(warningFirstEvaluation);
        delete warningFirstEvaluation;
        warningFirstEvaluation=NULL;
    }*/
    setWindowModified(true);
    displayInStatusBar("","black");

    taskProperties.firstPrintMessage=true;
    taskProperties.currentSheet=tabPages->currentIndex();
    taskProperties.currentLine=-1;
    MainSheet* sheet=dynamic_cast<MainSheet*>(tabPages->currentWidget());
    switch(sheet->getType()){
    case MainSheet::FORMAL_TYPE:
        {FormalWorkSheet *form=qobject_cast<FormalWorkSheet*>(tabPages->currentWidget());
            form->getCurrentLine()->addStopButton(stopButton);
            taskProperties.currentLine=form->getCurrentLine()->getId();
            CasManager::warning id=cas->initExpression(&formula);
            if (id==CasManager::Warning){
                QString s("<font color=\"red\"");
                s.append(tr("Attention! <br> Pour affecter une valeur à une variable, vous devez utiliser le symbole :="));
                s.append("<br><u>Exemple</u>: a:=2 ou f(x):=x^2");
                s.append("</font>");
                printHeader();
                giacMessages->appendHtml(s);
                form->getCurrentLine()->getTextInput()->setFocus();
            }
            cas->evaluate();
            time->start();

        }
        break;
    case MainSheet::SPREADSHEET_TYPE:
    case MainSheet::G2D_TYPE:
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
        form->displayResult(taskProperties.currentLine,cas->createDisplay());

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
   case MainSheet::G2D_TYPE:
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }

}
void MainWindow::displayHelp(const QString & keyWord) const{
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
    case MainSheet::G2D_TYPE:
      {GraphWidget * g2d=qobject_cast<GraphWidget*>(tabPages->currentWidget());
	 g2d->sendText(s);}
       break;
    case MainSheet::PROGRAMMING_TYPE:
        break;
    }
}

bool MainWindow::isEvaluating(){
    return cas->isRunning();
}
giac::context* MainWindow::getContext() const{
    return cas->getContext();
}

CommandInfo* MainWindow::getCommandInfo()const{
return commandInfo;
}
void MainWindow::displayStopWarning(){

        displayInStatusBar(tr("Tentative d'interruption..."),"red");
//    statusBar()->addWidget(warningStop,1);
//   warningStop->show();
}
void MainWindow::removeStopWarning(){
    displayInStatusBar("","black");
/*    if (warningStop->parent()!=0){
       statusBar()->removeWidget(warningStop);
        warningStop->setParent(0);
    }*/
}
void MainWindow::displayCrashWarning(){
    QMessageBox::warning(this,tr("Avertissement!"),tr("Risque de crash important lors de l'interruption requise."),QMessageBox::Ok,QMessageBox::NoButton);
    emit(hideCrashWarning());
}


CommandInfo::CommandInfo(){
    listAllCommands();
    completer=new QCompleter(commandList);

}
void CommandInfo::listAllCommands(){
    QFile file(":/aide_cas");
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

