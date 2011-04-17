#ifndef FORMALSHEET_H
#define FORMALSHEET_H

#include <QVector>
#include <QScrollArea>
#include "CentralTabWidget.h"
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class QCheckBox;
class QGridLayout;
class QVBoxLayout;
class QSyntaxHighlighter;
class QTextDocument;
class MainWindow;
class QCompleter;
class Line;
class CasManager;
/**
  This class is used to highlight QTextEdit
  it supports
  - Comments  //...  or / * ... * /
  - quotation "..."
  - keyword highlighting
  **/

class Highlighter:public QSyntaxHighlighter{
    Q_OBJECT
public:
    Highlighter(QTextDocument * parent=0,CasManager* cas=0);
protected:
    void highlightBlock(const QString &text);
private:
    enum style{
     keyword=1,comment=2,
     quotation=3
    };
    CasManager* cas;
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat quotationFormat;

};



class FormalWorkSheet: public QScrollArea, public MainSheet{
    Q_OBJECT
public:
    FormalWorkSheet(MainWindow *parent=0);
    MainWindow* getApp();
    void goToNextLine();
    void goToNextExistingLine();
    void goToPreviousExistingLine();
    void setCurrent(const int);
    int getCurrent() const;
    void setFocus(Qt::FocusReason);
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void sendText(const QString &);
  //  Highlighter* getHighlighter() const;
private:
    QWidget *mainPanel;
    int current;
    QVector <Line*>*lines;
    QVBoxLayout* vLayout;
//    QVBoxLayout* mainLayout;
    MainWindow *mainWindow;
//    Highlighter* highlighter;
};


#endif // FORMALSHEET_H
