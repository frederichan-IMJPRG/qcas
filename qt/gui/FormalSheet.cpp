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
#include "output.h"
TextBlockData::TextBlockData():QTextBlockUserData(){
}
DelimiterInfo* TextBlockData::infoAt(const int i){
    return delimiters.at(i);

}

void TextBlockData::insert(DelimiterInfo *info){
    int i = 0;
    while (i < delimiters.size() && info->position > delimiters.at(i)->position)
        ++i;

    delimiters.insert(i, info);
}
int TextBlockData::size() const{
    return delimiters.size();
}

Highlighter::Highlighter(QTextDocument *parent,CommandInfo* commandInfo)
    :QSyntaxHighlighter(parent){
    this->commandInfo=commandInfo;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    quotationFormat.setForeground(Qt::darkGreen);
    commentFormat.setForeground(Qt::darkGray);
}

void Highlighter::highlightBlock(const QString &text){
    TextBlockData * data=new TextBlockData;

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

                for(int j=i+1;j<text.length();j++){
                    if (text.at(j)==QChar('\n')){
                        setFormat(slash,j-slash+1,commentFormat);
                        find_return=true;
                        break;
                    }
                }
                if (!find_return) setFormat(slash,text.length()-slash+1,commentFormat);
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
        // Delimiters symbol
        else if (QString("()[]{}").contains(c)&& quote==-1){
            DelimiterInfo* info=new DelimiterInfo;
            info->character=c;
            info->position=i;

            data->insert(info);
            if( key!=-1){
                QString s=text.mid(key,i-key);
                if (commandInfo->isCommand(s)){
                    setFormat(key,i-key,keywordFormat);
                }
            }
            key=-1;


            //            qDebug()<< c <<"inserted at "<<i;
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
                if (commandInfo->isCommand(s)){
                    setFormat(key,i-key,keywordFormat);
                }
            }
            key=-1;
        }

    }
    if( key!=-1){
        QString s=text.mid(key,text.length()-key);
        if (commandInfo->isCommand(s)){
            setFormat(key,text.length()-key,keywordFormat);
        }
    }
    setCurrentBlockUserData(data);
}

FormalWorkSheet::~FormalWorkSheet(){
    for(int i=lines->size()-1;i>=0;--i){
        delete lines->at(i);
        lines->remove(i);
    }
    delete vLayout;
    delete mainPanel;

}
void FormalWorkSheet::removeStop(int a){
    Line* l=lines->at(a);
    l->removeStopButton();
}

FormalWorkSheet::FormalWorkSheet(MainWindow *parent):QScrollArea(),MainSheet(MainSheet::FORMAL_TYPE){

    current=0;
    mainWindow=parent;
    selectedLevels.clear();

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
void FormalWorkSheet::addSelectedLevel(int level){
  /*  if (shift){
        if (selectedLevels.empty()) selectedLevels.append(level);
        else{
            int last=selectedLevels.last();
            selectedLevels.clear();
            if (last<level) {
                int tmp=level;
                level=last;
                last=tmp;
            }
            for (int i=level;i<=last;++i){
                selectedLevels.append(i);
            }
        }
    }
    else {*/
        int index=selectedLevels.indexOf(level);
        if (index!=-1){
            selectedLevels.remove(index);
        }
        else selectedLevels.append(level);
    //}
}
void FormalWorkSheet::keyPressEvent(QKeyEvent *e){
    if (e->key()==Qt::Key_Shift)
        shift=true;
    else shift=false;
//    qDebug()<<"press"<<shift;
}
void FormalWorkSheet::keyReleaseEvent(QKeyEvent *){
    shift=false;
 //   qDebug()<<"release"<<shift;

}
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
void FormalWorkSheet::deleteSelectedLevels(){
    for (int i=selectedLevels.size()-1;i>=0;--i){
        Line* line=lines->at(selectedLevels.at(i));
        vLayout->removeWidget(line);
        lines->remove(selectedLevels.at(i));
        delete line;
    }
    selectedLevels.clear();
    for (int i=0;i<lines->size();++i){
        lines->at(i)->setId(i);
    }
    current=0;


}

void FormalWorkSheet::undo(){
    lines->at(current)->getTextInput()->undo();

}

void FormalWorkSheet::sendText(const QString & s){
    lines->at(current)->getTextInput()->insertPlainText(s);
}
Line* FormalWorkSheet::getCurrentLine(){
    return lines->at(current);
}
void FormalWorkSheet::displayResult(int lineIndex, OutputWidget* result){
    lines->at(lineIndex)->displayResult(result);
}
Line* FormalWorkSheet::getLineAt(int i){
    return lines->at(i);
}
void  FormalWorkSheet::toXML(QDomElement & root){
    QDomElement formal=root.ownerDocument().createElement("formal");

    for (int i=0;i<lines->size();++i){
        Line* line=lines->at(i);
        QDomElement command=root.ownerDocument().createElement("command");
        QDomText text=root.ownerDocument().createTextNode(line->getTextInput()->toPlainText());
        command.appendChild(text);
        formal.appendChild(command);
        OutputWidget* out=line->getOuputWidget();
        if (out!=0) out->toXML(formal);

    }
    root.appendChild(formal);


}
