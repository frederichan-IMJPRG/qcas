#ifndef FORMALSHEET_H
#define FORMALSHEET_H

#include <QVector>
#include <QScrollArea>
#include "CentralTabWidget.h"
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QDomElement>
class OutputWidget;
class QCheckBox;
class QGridLayout;
class QVBoxLayout;
class QSyntaxHighlighter;
class QTextDocument;
class MainWindow;
class QCompleter;
class Line;
class CommandInfo;


struct DelimiterInfo
{
    QChar character;
    int position;
};
class TextBlockData : public QTextBlockUserData
{
public:
    TextBlockData();
    DelimiterInfo* infoAt(const int);
    void insert(DelimiterInfo *info);
    int size() const;
private:
    QVector<DelimiterInfo *> delimiters;
};


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
    Highlighter(QTextDocument * parent=0,CommandInfo* cas=0);
//    static bool isQuotationFormat(const QTextCharFormat &) ;

protected:
    void highlightBlock(const QString &text);
private:
    enum style{
     keyword=1,comment=2,
     quotation=3
    };
    CommandInfo* commandInfo;

    QTextCharFormat quotationFormat;
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;

};



class FormalWorkSheet: public QScrollArea, public MainSheet{
    Q_OBJECT

public:
    FormalWorkSheet(MainWindow *parent=0);
    ~FormalWorkSheet();
    MainWindow* getApp();
    void goToNextLine();
    void goToNextExistingLine();
    void goToPreviousExistingLine();
    void setCurrent(const int);
    int getCurrent() const;
    Line* getCurrentLine();
    Line* getLineAt(int);
    void setFocus(Qt::FocusReason);
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void sendText(const QString &);
    void displayResult(int line,OutputWidget* );
    void addSelectedLevel(int);
    void removeStop(int);
    void toXML(QDomElement & root);
protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private:
    bool shift;
    QWidget *mainPanel;
    int current;
    QVector <Line*>*lines;
    QVector<int>selectedLevels;
    QVBoxLayout* vLayout;
    MainWindow *mainWindow;
};

#endif // FORMALSHEET_H
