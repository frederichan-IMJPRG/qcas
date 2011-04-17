#ifndef FORMALLINEWIDGETS_H
#define FORMALLINEWIDGETS_H
#include <QWidget>
#include <QTextEdit>
class QtMmlWidget;
class QCompleter;
class Line;
class QFocusEvent;
class OutputWidget:public QWidget{
public:
    OutputWidget(QWidget* widget=0);
    void setLine(Line* );
private:
    Line* line;
};
class FormulaWidget :public OutputWidget{
public:
    FormulaWidget(QtMmlWidget*);
};


class TextInput : public QTextEdit{
    Q_OBJECT
public:
    TextInput(Line *parent=0);
//    void setCompleter(QCompleter *c);
protected:
    void keyPressEvent(QKeyEvent *e);
    bool event(QEvent*e);
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);


private slots:
    void addMultiLines(int,int,int);
    void matchBraces();
    void insertCompletion(const QString &completion);
private:
    bool goDown();
    bool goUp();
    void updateCompleter();
    void installCompleter();
    QString textUnderCursor() const;
    Line* line;
    QCompleter* completer;
};



#endif // FORMALLINEWIDGETS_H
