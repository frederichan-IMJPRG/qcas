#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QCheckBox>
#include "FormalSheet.h"
#include <QScrollBar>
//#include "src/qtmmlwidget.h"
#include <QDebug>
#include <QScrollArea>
//#include <QSyntaxHighlighter>
//#include <QGridLayout>
//#include <QCompleter>
#include <QThread>
#include "FormalLineWidgets.h"
#include "FormalLine.h"
#include "../MainWindow.h"
#include "../CasManager.h"


Highlighter::Highlighter(QTextDocument *parent,CasManager* cas)
    :QSyntaxHighlighter(parent){
    this->cas=cas;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    quotationFormat.setForeground(Qt::darkGreen);

    commentFormat.setForeground(Qt::darkGray);
}
void Highlighter::highlightBlock(const QString &text){
    int start=0;
    // First, we look for previous comment not finished
    if (previousBlockState()==comment){
        int close=text.indexOf("*/");
        if (close==-1) {
            setFormat(0,text.length(),commentFormat);
            setCurrentBlockState(comment);
            return;
        }
        else {
            setFormat(0,close+3,commentFormat);
            start=close+2;
        }
    }

    QChar c;
    int  key=-1;
    int slash=-1;
    bool backslash=false;
    int quote=-1;

    for (int i=start;i<text.length();++i){
        c=text.at(i);
        // Found / symbol
        if (c==QChar('/')&&quote==-1){
            // Found // --> Comment the whole line
            if (slash!=-1) {
                bool find_return=false;
                for(int j=i+1;i<text.length();j++){
                    if (text.at(j)==QChar('\n')){
                        setFormat(slash,j-slash+1,commentFormat);
                        find_return=true;
                        break;
                    }
                    if (!find_return) setFormat(slash,text.length()-slash+1,commentFormat);
                }
                slash=-1;
            }
            else slash=i;
            backslash=false;
            key=-1;
        }
        // Found symbol /* for multiline comment
        else if (c==QChar('*')&&slash!=-1){
            int close=text.indexOf("*/",i);
            if (close==-1) {
                setFormat(slash,text.length()-slash+1,commentFormat);                
                i=text.length()-1;
                setCurrentBlockState(comment);
            }
            else {
                setFormat(slash,close+2-slash+1,commentFormat);
                i=close+2;
            }
            slash=-1;
            backslash=false;
        }
        else if (c==QChar('"')){
            // Character \"
            if (backslash) backslash=false;
            // First quotation symbol "
            else if (quote==-1) quote=i;
            // Closing quotation symbol
            else{
                setFormat(quote,i-quote+1,quotationFormat);
                quote=-1;
            }
            backslash=false;
            key=-1;
            slash=-1;
        }
        else if (c==QChar('\\')){
            if (backslash){
                backslash=false;
            }
            else backslash=true;
            key=-1;
            slash=-1;
        }
        else if (c.category()==QChar::Letter_Lowercase
                 ||c.category()==QChar::Letter_Uppercase
                 || c.category()==QChar::Number_DecimalDigit){
            if (key==-1){
                key=i;
            }
            slash=-1;
            backslash=false;
        }
        else{
            if( key!=-1){
                QString s=text.mid(key,i-key);
                if (cas->isCommand(s)){
                    setFormat(key,i-key,keywordFormat);
                }
            }
            key=-1;
        }

    }
    if( key!=-1){
        QString s=text.mid(key,text.length()-key);
        if (cas->isCommand(s)){
            setFormat(key,text.length()-key,keywordFormat);
        }
    }


}



FormalWorkSheet::FormalWorkSheet(MainWindow *parent):QScrollArea(),MainSheet(MainSheet::FORMAL_TYPE){

    current=0;
    mainWindow=parent;

    mainPanel=new QWidget;

    vLayout=new QVBoxLayout;

    lines=new QVector<Line*>;
    lines->append(new Line(0,this));
    vLayout->addWidget(lines->at(0));

    mainPanel->setLayout(vLayout);
    mainPanel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    vLayout->setSizeConstraint(QLayout::SetMinimumSize);

    setWidget(mainPanel);
    setWidgetResizable(true);


    lines->last()->getTextInput()->setFocus(Qt::OtherFocusReason);

}
/*Highlighter* FormalWorkSheet::getHighlighter() const{
    return highlighter;
}

    QObject* formulaInterface =new FormulaObject;
    document()->documentLayout()->registerHandler(FormulaFormat,formulaInterface);

    QTextCharFormat formulaChar;
    formulaChar.setObjectType(FormulaFormat);

    QString mmlText("<math>"
                    "<msqrt><mn>2</mn></msqrt><mo>+</mo><mn>2</mn><msqrt><mn>2</mn></msqrt>"
                    "</math>");

    QWidget *w=new QWidget;
    QPalette pal=w->palette();
    pal.setColor(w->backgroundRole(),Qt::yellow);
w->setPalette(pal);
    QtMmlWidget *mmlWidget=new QtMmlWidget(w);
    //    mmlWidget->setsetFontName(QtMmlWidget::SansSerifFont,);
    QString errorMsg;
    int errorLine;
    int errorColumn;
    bool ok = mmlWidget->setContent(mmlText, &errorMsg, &errorLine, &errorColumn);
    if (!ok) {
      qWarning("MathML error: %s, Line: %d, Column: %d",
      errorMsg.toLatin1(), errorLine, errorColumn);
    }    
    QSize s=mmlWidget->sizeHint();

    std::cout<<s.width()<<s.height()<<std::endl;
    mmlWidget->setBaseFontPointSize(80);
    mmlWidget->adjustSize();
    std::cout<<s.width()<<s.height()<<std::endl;
    this->adjustSize();

    //    std::cout<<qPrintable(mmlWidget->sizeHint())<<std::endl;

    QPixmap pix=QPixmap::grabWidget(mmlWidget);
    formulaChar.setProperty(MML,pix.toImage());

    QTextCursor cursor=this->textCursor();
    cursor.insertText(QString(QChar::ObjectReplacementCharacter),formulaChar);
    setTextCursor(cursor);

}

*/
MainWindow* FormalWorkSheet::getApp(){
    return mainWindow;
}
void FormalWorkSheet::goToNextLine(){
    if (current<lines->size()-1){
        current++;
        lines->at(current)->getTextInput()->setFocus(Qt::OtherFocusReason);
    }
    else {
        current++;
        lines->append(new Line(current,this));
        vLayout->addWidget(lines->at(current));
        //        this->setWidget(takeWidget());

        lines->at(current)->getTextInput()->setFocus(Qt::OtherFocusReason);
        lines->at(current)->show();
/*        for(int i=0;i<=current;++i){
            lines->at(i)->info();
            qDebug()<<"geometry "<<lines->at(i)->geometry();
    }*/
    }
    QRect r=lines->at(current)->geometry();
    ensureVisible(r.x(),r.y()+r.height());
}
void FormalWorkSheet::goToNextExistingLine(){
    if (current<lines->size()-1){
        current++;
        lines->at(current)->getTextInput()->setFocus(Qt::OtherFocusReason);
    }
    QRect r=lines->at(current)->geometry();
    ensureVisible(r.x(),r.y()+r.height());

}
void FormalWorkSheet::goToPreviousExistingLine(){
    if (current>0){
        current--;
        lines->at(current)->getTextInput()->setFocus(Qt::OtherFocusReason);
    }
    QRect r=lines->at(current)->geometry();
    ensureVisible(r.x(),r.y()+r.height());
}
int FormalWorkSheet::getCurrent() const{
    return current;
}
void FormalWorkSheet::setCurrent(const int c){
    current=c;
}
void FormalWorkSheet::setFocus(Qt::FocusReason reason){
    QScrollArea::setFocus(reason);
    lines->at(current)->getTextInput()->setFocus();
}
void FormalWorkSheet::copy(){
    lines->at(current)->getTextInput()->copy();
}
void FormalWorkSheet::cut(){
    lines->at(current)->getTextInput()->cut();
}
void FormalWorkSheet::paste(){
    lines->at(current)->getTextInput()->paste();
}
void FormalWorkSheet::redo(){
    lines->at(current)->getTextInput()->redo();
}
void FormalWorkSheet::undo(){
    lines->at(current)->getTextInput()->undo();

}
void FormalWorkSheet::sendText(const QString & s){
    lines->at(current)->getTextInput()->append(s);
}

