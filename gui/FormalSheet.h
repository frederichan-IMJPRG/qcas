#ifndef FORMALSHEET_H
#define FORMALSHEET_H

#include <QVector>
//#include <QTextEdit>
#include <QScrollArea>
#include "CentralTabWidget.h"

class QCheckBox;
class QGridLayout;
class QVBoxLayout;
class QSyntaxHighlighter;
class QTextDocument;
class MainWindow;
class QCompleter;
class Line;
/*class Highlighter : public QSyntaxHighlighter
{
Q_OBJECT
public:
    Highlighter(QTextDocument *document);
protected:
    void highlightBlock(const QString &text);
};
struct ParenthesisInfo
{
    char character;
    int position;
};
class TextBlockData : public QTextBlockUserData
{
public:

    TextBlockData();

    QVector<ParenthesisInfo *> parentheses();
    void insert(ParenthesisInfo *info);

private:

    QVector<ParenthesisInfo *> m_parentheses;

};

*/


class FormalWorkSheet: public QScrollArea, public MainSheet{
    Q_OBJECT
public:
    FormalWorkSheet(MainWindow *parent=0);
    MainWindow* getApp();
    void goToNextLine();
    void setCurrent(const int);
    int getCurrent() const;
    void setFocus(Qt::FocusReason);
    void copy();
    void cut();
    void paste();
    void undo();
    void redo();
    void sendText(const QString &);
private:
    QWidget *mainPanel;
    int current;
    QVector <Line*>*lines;
    QVBoxLayout* vLayout;
    QVBoxLayout* mainLayout;
    MainWindow *mainWindow;
};


#endif // FORMALSHEET_H
