#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
class QAction;
class CasManager;
class QListWidget;
class QStackedWidget;
class QMenu;
class QListWidgetItem;
class QToolBar;
class MainTabWidget;

class MainWindow :public QMainWindow {
    Q_OBJECT
 public:
    MainWindow();    
    CasManager* getCas() const;
    void displayHelp(const QString &) const;
    void sendText(const QString &);
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
    CasManager *cas;

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

    QListWidget *wizardList;
    QStackedWidget *wizardPages;
    MainTabWidget *tabPages;

    QToolBar* toolBar;



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
};


#endif // MAINWINDOW_H
