#ifndef CENTRALTABWIDGET_H
#define CENTRALTABWIDGET_H

#include <QTabWidget>
class QToolButton;
class QMenu;
class MainWindow;

class MainSheet{
public:
    enum sheetType{FORMAL_TYPE=0,SPREADSHEET_TYPE=1,PROGRAMMING_TYPE=2,G2D=3};
    MainSheet(sheetType);
    sheetType getType() const;
protected:
    sheetType type;

};
class MainTabWidget:public QTabWidget{
    Q_OBJECT
public:
    MainTabWidget(MainWindow *main=0);
private:
    QToolButton *add;
    QMenu* menu;
    QAction* formalAction;
    QAction* spreadsheetAction;
    QAction* programmingAction;
    QAction* g2dAction;

    MainWindow *mainWindow;
public slots:
    void addFormalSheet();
    void addSpreadSheet();
    void addProgrammingSheet();
    void addG2dSheet();
    void closeTab(int);
};


#endif // CENTRALTABWIDGET_H
