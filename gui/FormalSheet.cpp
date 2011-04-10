#include <QVBoxLayout>
#include <QHBoxLayout>
//#include <QCheckBox>
#include "FormalSheet.h"
//#include <QKeyEvent>
//#include "src/qtmmlwidget.h"
#include <QDebug>
#include <QScrollArea>
//#include <QSyntaxHighlighter>
//#include <QGridLayout>
//#include <QCompleter>
#include "FormalLineWidgets.h"
#include "FormalLine.h"
#include "../MainWindow.h"
#include "../CasManager.h"



FormalWorkSheet::FormalWorkSheet(MainWindow *parent):QScrollArea(),MainSheet(MainSheet::FORMAL_TYPE){
    current=0;
    mainWindow=parent;

    mainPanel=new QWidget;

    vLayout=new QVBoxLayout;
    mainLayout=new QVBoxLayout;

    lines=new QVector<Line*>;
    lines->append(new Line(0,this));
    vLayout->addWidget(lines->at(0));
    mainLayout->addLayout(vLayout);
    mainLayout->addStretch(1);
    mainPanel->setLayout(mainLayout);
    mainPanel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);


    setWidget(mainPanel);
    setWidgetResizable(true);

    lines->last()->getTextInput()->setFocus(Qt::OtherFocusReason);
}

/*
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
        vLayout->addWidget(lines->last());
        lines->last()->getTextInput()->setFocus(Qt::OtherFocusReason);
    }
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

