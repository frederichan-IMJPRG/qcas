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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#include "EvaluationThread.h"
#include <QMainWindow>
#include "global.h"
class QAction;
class QListWidget;
class QStackedWidget;
class QMenu;
class QListWidgetItem;
class QToolBar;
class CasManager;
class MainTabWidget;
class QPlainTextEdit;
class QLabel;
class QCompleter;
class QToolButton;
class QTime;
class PrefDialog;
struct TaskProperties{
    bool firstPrintMessage;
    int currentLine;
    int currentSheet;
};
class CommandInfo{
public:
    CommandInfo();
    QString seekForKeyword(const QString &) const;
    QString displayPage(const QString &) const;
    QCompleter* getCompleter() const;
    QStringList getCommands();
    bool isCommand(const QString &) const;
private:
    QCompleter *completer;
    void listAllCommands();
    QStringList commandList;
};

class MainWindow :public QMainWindow {
    Q_OBJECT
 public:
    MainWindow();    
    void displayHelp(const QString &) const;
    void sendText(const QString &);
    bool isEvaluating();
    CommandInfo* getCommandInfo() const;
    void evaluate(const QString & formula);
    QToolButton* getStopButton() const;
    giac::context * getContext() const;
    void displayStopWarning();


protected:
    void closeEvent(QCloseEvent *event);

private:
    void createGui();
    void createWizards();
    void createAction();
    void createToolBars();
    void createMenus();
    void createContextMenu();
    void readSettings();
    void writeSettings();
    bool okToContinue();
    bool loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString curFile;
    QStringList recentFiles;
    QString strippedName(const QString & fullFileName);
    void displayGiacMessages();

    enum{MaxRecentFiles=5};
    QAction* recentFileActions[MaxRecentFiles];
    QAction* separatorAction;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *selectSubMenu;
    QMenu *toolsMenu;
    QMenu *optionsMenu;
    QMenu *helpMenu;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *undoAction;
    QAction *redoAction;
    QAction *evaluateAction;
    QAction *prefAction;
    QAction *aboutAction;

    QAction *stopAction;
    QToolButton * stopButton;

    QListWidget *wizardList;
    QStackedWidget *wizardPages;

    PrefDialog *prefDialog;
    MainTabWidget *tabPages;
    QPlainTextEdit* giacMessages;
    QToolBar* toolBar;
    QLabel* warningFirstEvaluation;
    QLabel* warningStop;
    CasManager* cas;
    TaskProperties taskProperties;
    CommandInfo* commandInfo;
    QTime* time;
    bool displayTimeAfterProcess;
    void printHeader();

public slots:
    void displayCrashWarning();
    void removeStopWarning();

private slots:
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void pref();
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void evaluate();
    void openRecentFile();
    void changeWizard(QListWidgetItem*,QListWidgetItem*);
    void displayResult();
    void killThread();
signals:
    void hideCrashWarning();
};
#endif // MAINWINDOW_H
