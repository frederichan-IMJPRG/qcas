#include "WizardEquation.h"
#include "../MainWindow.h"
#include <QStackedWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QGridLayout>
#include <QString>
#include <iostream>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPlainTextEdit>
#include <QStringList>
#include <QSpinBox>

WizardEquation::WizardEquation(MainWindow *parent)
    :QWidget(parent){
    mainWindow=parent;
    createGui();
}

void WizardEquation::createGui(){
    pages=new QStackedWidget;
    list=new QComboBox;
    list->addItem(tr("Equation/inéquation"));
    list->addItem(tr("Equation différentielle"));
    list->addItem(tr("Système d'équations"));

    EqPanel *eqPanel=new EqPanel(this);
    DiffPanel *diffPanel=new DiffPanel(this);
    SystPanel *systPanel=new SystPanel(this);


    pages->addWidget(eqPanel);
    pages->addWidget(diffPanel);
    pages->addWidget(systPanel);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(list);
    layout->addWidget(pages);
    this->setLayout(layout);
    connect(list,SIGNAL(activated(int)),pages,SLOT(setCurrentIndex(int)));

}
void WizardEquation::sendEquation(const QString &s){
//    QString s=qobject_cast<TabChild*>(pages->currentWidget())->sendEquation();
    mainWindow->sendText(s);
}


TabChild::TabChild(QWidget *parent):QWidget(parent){
}
EqPanel::EqPanel(WizardEquation *parent):TabChild(parent){
        eqPanel=parent;
        QLabel *labelEq=new QLabel(tr("&Equation:"));
        eq=new QLineEdit;
        labelEq->setBuddy(eq);

        eq->setToolTip(tr("Saisir l'équation à résoudre <br> <b>Exemple: </b> x^2+2*x=x^3"));
        QLabel *labelVar=new QLabel(tr("&Variable:"));
        var=new QLineEdit("x");
        labelVar->setBuddy(var);

        numeric=new QCheckBox(tr("Résolution numérique"));
        inC=new QCheckBox(tr("Solutions complexes"));

        QPushButton *button=new QPushButton(tr("Go!"));
        QGridLayout *grid=new QGridLayout();
        grid->addWidget(labelEq,0,0);
        grid->addWidget(eq,0,1);
        grid->addWidget(labelVar,1,0);
        grid->addWidget(var,1,1);
        grid->addWidget(numeric,2,0,2,1);
        grid->addWidget(inC,3,0,2,1);
        grid->addWidget(button,4,1);
        this->setLayout(grid);

        connect(button,SIGNAL(clicked()),this,SLOT(sendEquation()));
        connect(numeric,SIGNAL(stateChanged(int)),this,SLOT(numericCheck(int)));
        connect(inC,SIGNAL(stateChanged(int)),this,SLOT(inC_Check(int)));
    }
void EqPanel::inC_Check(int a){
    if (a==Qt::Checked){
        if (numeric->isChecked()) {
            numeric->setChecked(false);
        }
    }
}

void EqPanel::numericCheck(int a){
    if(a==Qt::Checked){
        if (inC->isChecked()){
            inC->setChecked(false);
        }
    }
}

void EqPanel::sendEquation(){
    QString s;
    if (inC->isChecked()) {
        s.append("csolve(");
    }
    else if (numeric->isChecked()){
        s.append("fsolve(");
    }
    else s.append("solve(");
    s.append(eq->text());
    s.append(",");
    s.append(var->text());
    s.append(");");
    eqPanel->sendEquation(s);
}

DiffPanel::DiffPanel(WizardEquation *parent):TabChild(parent){
    eqPanel=parent;
    QLabel *labelEq=new QLabel(tr("&Equation:"));
    eq=new QLineEdit;
    labelEq->setBuddy(eq);

    eq->setToolTip(tr("Saisir l'équation différentielle<br> <b>Exemple: </b> y''+y=sin(x)"));
    QLabel *labelVar=new QLabel(tr("&Variable:"));
    var=new QLineEdit("y");
    labelVar->setBuddy(var);

    QGroupBox *group=new QGroupBox(tr("Conditions initiales"));
    QHBoxLayout *layout=new QHBoxLayout;
    editor=new QPlainTextEdit();
    editor->setToolTip(tr("<b>Exemples:</b><br> y(0)=1<br>y'(0)=2"));
    layout->addWidget(editor);
    group->setLayout(layout);

    QPushButton *button=new QPushButton(tr("Go!"));

    QGridLayout *grid=new QGridLayout();
    grid->addWidget(labelEq,0,0);
    grid->addWidget(eq,0,1);
    grid->addWidget(labelVar,1,0);
    grid->addWidget(var,1,1);
    grid->addWidget(group,2,0,2,2);
    grid->addWidget(button,4,1);
    this->setLayout(grid);

    connect(button,SIGNAL(clicked()),this,SLOT(sendEquation()));
}

void DiffPanel::sendEquation(){
    QString s("desolve(");
    QString init=editor->document()->toPlainText();
    if (init.isEmpty()){
        s.append(eq->text());
    }
    else{
        s.append("[");
        s.append(eq->text());
        s.append(",");
        QStringList list=init.split("\n",QString::SkipEmptyParts);
        for(int i=0;i<list.size();++i){
            s.append(list.at(i));
            if (i!=list.size()-1){
                s.append(",");
            }
        }
        s.append("]");
    }
    s.append(",");
    s.append(var->text());
    s.append(");");
    eqPanel->sendEquation(s);
}
MySpin::MySpin(QWidget *parent):QSpinBox(parent){
    setMinimum(2);
    setMaximum(10);
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    lineEdit()->setReadOnly(true);

}
SystPanel::SystPanel(WizardEquation *parent):TabChild(parent){
    eqPanel=parent;
    labelSpin=new QLabel(tr("Nombre d'équations:"));
    spin=new MySpin;
    QLineEdit *eq1=new QLineEdit;
    eq1->setToolTip(tr("<b>Exemple:<br> </b> 2*x+3*y=10"));

    QLineEdit *eq2=new QLineEdit;
    eq1->setToolTip(tr("<b>Exemple: </b><br> x^2-y=3"));

    label=new QVector<QLabel*>;
    edit=new QVector<QLineEdit*>;
    label->append(new QLabel(tr("Equation n°1:")));
    label->append(new QLabel(tr("Equation n°2:")));
    edit->append(eq1);
    edit->append(eq2);

    labelVar=new QLabel(tr("&Variable:"));
    var=new QLineEdit("x y");
    labelVar->setBuddy(var);

    check=new QCheckBox(tr("Systèmes linéaires"));
    button=new QPushButton(tr("Go!"));

    grid=new QGridLayout;
    updateGrid();

    setLayout(grid);

    connect(button,SIGNAL(clicked()),this,SLOT(sendEquation()));
    connect(spin,SIGNAL(valueChanged(int)),this,SLOT(addEquations(int)));

}
void SystPanel::updateGrid(){
    int line=0;
    grid->addWidget(labelSpin,line,0);
    grid->addWidget(spin,line,1);
    ++line;
    for(int i=0;i<edit->size();++i){
        grid->addWidget(label->at(i),line,0);
        grid->addWidget(edit->at(i),line,1);
        ++line;
    }
    grid->addWidget(labelVar,line,0);
    grid->addWidget(var,line,1);
    ++line;
    grid->addWidget(check,line,0,1,2);
    ++line;
    grid->addWidget(button,line,1);
}
void SystPanel::addEquations(int id){
    delete layout();
    // Add equations
    if (id>edit->size()){
        QString s=tr("Equation n°");
        QString str;
        str.setNum(id);
        s.append(str);
        s.append(":");
        label->append(new QLabel(s));
        edit->append(new QLineEdit);
    }
    // remove equations
    else {
        delete label->at(label->size()-1);
        delete edit->at(edit->size()-1);
        label->remove(label->size()-1);
        edit->remove(edit->size()-1);
    }
    grid=new QGridLayout;
    updateGrid();
    setLayout(grid);
}
void SystPanel::sendEquation(){
   QString s;
   if (check->isChecked()){
        s.append("linsolve([");
   }
   else {
    s.append("solve([");
   }
   for(int i=0;i<edit->size();++i){
        s.append(edit->at(i)->text());
        if (i!=edit->size()-1) {
            s.append(",");
        }
   }
   s.append("],[");
   QStringList list=var->text().split(" ",QString::SkipEmptyParts);
   for(int i=0;i<list.size();++i){
       s.append(list.at(i));
       if (i!=list.size()-1){
           s.append(",");
       }
   }
   s.append("]);");
    eqPanel->sendEquation(s);;
}
