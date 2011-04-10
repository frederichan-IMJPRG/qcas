#include <QKeyEvent>
#include <QToolTip>
#include <QScrollBar>
#include <QDebug>
#include <QCompleter>
#include <QHBoxLayout>
#include <QAbstractItemView>
#include "FormalSheet.h"
#include "FormalLineWidgets.h"
#include "FormalLine.h"
#include "../CasManager.h"
#include "../MainWindow.h"
#include "src/qtmmlwidget.h"
TextInput::TextInput(Line *parent):QTextEdit(parent){
    line=parent;

    setFixedHeight(fontMetrics().lineSpacing()+fontMetrics().descent()+2*document()->documentMargin());
    connect(document(),SIGNAL(contentsChange(int,int,int)),this,SLOT(addMultiLines(int,int,int)));
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(matchBraces()));

}
void TextInput::installCompleter(){
    completer=line->getWorkSheet()->getApp()->getCas()->getCompleter();
    completer->setWidget(this);
    completer->setCompletionMode(QCompleter::PopupCompletion);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    connect(completer,SIGNAL(activated(QString)),this,SLOT(insertCompletion(QString)));
}

//void TextInput::setCompleter(QCompleter *c){
//    completer=c;
//}

void TextInput::keyPressEvent(QKeyEvent *e){
    if (completer&&completer->popup()->isVisible()){
        switch(e->key()){
            case Qt::Key_Enter:
            case Qt::Key_Return:
            case Qt::Key_Escape:
            case Qt::Key_Tab:
            case Qt::Key_Backtab:
                e->ignore();
                return;
        default:
                break;
        }

    }
    switch(e->key()){
        case Qt::Key_F1:
        if (QToolTip::isVisible()){
            line->getWorkSheet()->getApp()->displayHelp(
                     property("myToolTip").toString());
            setFocus();
        }
        break;
        case Qt::Key_Space:
        if (e->modifiers()&Qt::ControlModifier){
            updateCompleter();
        }
        else QTextEdit::keyPressEvent(e);
        break;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (e->modifiers()&Qt::ShiftModifier) {
                line->output(toPlainText());
                line->getWorkSheet()->goToNextLine();
            }
            else {
                QTextEdit::keyPressEvent(e);
                setFixedHeight(height()+fontMetrics().lineSpacing()+1);
            }
         break;
        case Qt::Key_Delete:
        case Qt::Key_Backspace:
         {
            QTextCursor cursor=this->textCursor();
            QString st;
            if (cursor.hasSelection()){
                st.append(cursor.selectedText());
            }
            else {
                int pos=cursor.position();
                QTextCursor newcursor(document());
                newcursor.setPosition(pos,QTextCursor::MoveAnchor);
                if( e->key()==Qt::Key_Delete)
                    newcursor.movePosition(QTextCursor::Right,QTextCursor::KeepAnchor);
                 else
                     newcursor.movePosition(QTextCursor::Left,QTextCursor::KeepAnchor);
                st.append(newcursor.selectedText());
            }
            int count=st.count(QChar(0x2029));
            setFixedHeight(height()-count*(fontMetrics().lineSpacing()+1));
            QTextEdit::keyPressEvent(e);
            if (completer->popup()->isVisible()){
               updateCompleter();
            }

        break;}
       default:
         QTextEdit::keyPressEvent(e);
         if (completer->popup()->isVisible()){
            updateCompleter();
         }
        }
    }
void TextInput::updateCompleter(){
    QString completionPrefix=textUnderCursor();
    if (completionPrefix !=completer->completionPrefix()){
        completer->setCompletionPrefix(completionPrefix);
        completer->popup()->setCurrentIndex(completer->completionModel()->index(0,0));
    }
    QRect cr=cursorRect();
    cr.setWidth(completer->popup()->sizeHintForColumn(0)
                +completer->popup()->verticalScrollBar()->sizeHint().width());
    completer->complete(cr);
}
void TextInput::focusInEvent(QFocusEvent *e){
    installCompleter();
    line->getWorkSheet()->setCurrent(line->getId());
    QTextEdit::focusInEvent(e);
}
void TextInput::focusOutEvent(QFocusEvent *e){
    QObject::disconnect(completer,0,this,0);

    QTextEdit::focusOutEvent(e);
}
bool TextInput::event(QEvent* event){
        if (event->type() == QEvent::ToolTip)
        {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);
        QTextCursor cursor=cursorForPosition(helpEvent->pos());
        cursor.select(QTextCursor::WordUnderCursor);
        QString st=cursor.selectedText();
        if (!st.isEmpty()&&
            line->getWorkSheet()->getApp()->getCas()->isCommand(st)) {
            setProperty("myToolTip",st);
            st.prepend("<b>");
            st.append("&nbsp;&nbsp;</b><img src=\":/images/f1.png\" align=\"absmiddle\" height=");
            QString s;
            s.setNum(2*QToolTip::font().pointSize());
            st.append(s);
            st.append(">");

            QToolTip::showText(helpEvent->globalPos(), st);
        }
        else
                QToolTip::hideText();
        return true;
        }
        return QTextEdit::event(event);
}


void TextInput::addMultiLines(int pos,int,int charsAdded){
    //User paste a text in the editor
    if (charsAdded>1){
        QTextCursor newcursor(document());
        newcursor.setPosition(pos,QTextCursor::MoveAnchor);
        newcursor.setPosition(pos+charsAdded,QTextCursor::KeepAnchor);
       QString s=newcursor.selectedText();
       int count=s.count(QChar(0x2029));
       setFixedHeight(height()+count*(fontMetrics().lineSpacing()+1));


    }
}
void TextInput::matchBraces(){
    QChar c=document()->characterAt(textCursor().position());
    if (c=='{') qDebug()<<"test";

}
QString TextInput::textUnderCursor() const{
    QTextCursor tc=textCursor();
    tc.select(QTextCursor::WordUnderCursor);
    return tc.selectedText();
}

void TextInput::insertCompletion(const QString &completion){
    QTextCursor tc=textCursor();
    tc.movePosition(QTextCursor::EndOfWord);
    tc.movePosition(QTextCursor::StartOfWord,QTextCursor::KeepAnchor);
    tc.removeSelectedText();
    tc.insertText(completion);
    setTextCursor(tc);
}

/*
void Highlighter::highlightBlock(const QString &text)
{
    TextBlockData *data = new TextBlockData;
    int leftPos = text.indexOf('(');
    while (leftPos != -1) {
        ParenthesisInfo *info = new ParenthesisInfo;
        info->character = '(';
        data->insert(info);
        leftPos = text.indexOf('(', leftPos + 1);
    }
    setCurrentBlockUserData(data);
}
*/



OutputWidget::OutputWidget(QWidget *parent):QWidget(){

}
void OutputWidget::setLine(Line* l){
    line=l;
}

FormulaWidget::FormulaWidget(QtMmlWidget *mml){
    QHBoxLayout *layout=new QHBoxLayout;
    layout->addWidget(mml);
    setLayout(layout);
}

