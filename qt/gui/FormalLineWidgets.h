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
  void ressetHistoryLevel();
  void insertIndentedString(const QString &s);

protected:
    void keyPressEvent(QKeyEvent *e);
    bool event(QEvent*e);
    void focusInEvent(QFocusEvent* e);
    void focusOutEvent(QFocusEvent* e);
    void keyReleaseEvent(QKeyEvent *e);



private slots:
    void addMultiLines(int,int,int);
    void insertCompletion(const QString &completion);
    void helpCompletion(const QString &completion);
    void matchDelimiters();
private:
    bool goDown();
    bool goUp();
    void updateCompleter();
    void xcasmenuCompleter(QString section);
    void installCompleter();
    bool matchRightDelimiter(QChar,QChar,QTextBlock, int , int);
    bool matchLeftDelimiter(QChar,QChar, QTextBlock, int , int);
    void createDelimiterSelection(int,bool);

    QString textUnderCursor() const;
    bool isStartCursor() const;
    Line* line;
    QCompleter* completer;
    Highlighter* highlighter;
    int historylevel;
};



#endif // FORMALLINEWIDGETS_H
