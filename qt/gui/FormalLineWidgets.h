#ifndef FORMALLINEWIDGETS_H
#define FORMALLINEWIDGETS_H
#include <QWidget>
#include <QPlainTextEdit>
class QtMmlWidget;
class QCompleter;
class Line;
class QFocusEvent;
class Highlighter;

class TextInput : public QPlainTextEdit{
    Q_OBJECT

public:
    TextInput(Line *parent=0);
    ~TextInput();
    //    void setCompleter(QCompleter *c);
protected:
    void keyPressEvent(QKeyEvent *e);
    bool event(QEvent*e);
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);


private slots:
    void addMultiLines(int,int,int);
    void insertCompletion(const QString &completion);
    void matchDelimiters();
private:
    bool goDown();
    bool goUp();
    void updateCompleter();
    void installCompleter();
    bool matchRightDelimiter(QChar,QChar,QTextBlock, int , int);
    bool matchLeftDelimiter(QChar,QChar, QTextBlock, int , int);
    void createDelimiterSelection(int,bool);

    QString textUnderCursor() const;
    Line* line;
    QCompleter* completer;
    Highlighter* highlighter;
};



#endif // FORMALLINEWIDGETS_H
