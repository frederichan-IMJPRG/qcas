/*   F.Han
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

#include "WizardAlgo.h"
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

WizardAlgo::WizardAlgo(MainWindow *parent)
    :QWidget(parent){
    mainWindow=parent;
    createGui();
}
void WizardAlgo::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange)
        {
            retranslate();
        }
    QWidget::changeEvent(event);
}
void WizardAlgo::retranslate(){
    list->setItemText(0,tr("Fonction"));
    list->setItemText(1,tr("Boucle: Pour"));

}
void WizardAlgo::createGui(){
    pages=new QStackedWidget;
    list=new QComboBox;
    list->addItem("");
    FunctionPanel *functionPanel=new FunctionPanel(this);

    list->addItem("");
    LoopPanel *loopPanel=new LoopPanel(this);


    pages->addWidget(functionPanel);
    pages->addWidget(loopPanel);

    QVBoxLayout *layout=new QVBoxLayout;
    layout->addWidget(list);
    layout->addWidget(pages);
    this->setLayout(layout);
    connect(list,SIGNAL(activated(int)),pages,SLOT(setCurrentIndex(int)));

    retranslate();
}
void WizardAlgo::sendCommand(const QString &s){
    mainWindow->sendText(s);
}

AlgoTabChild::AlgoTabChild(QWidget *parent):QWidget(parent){
}
void FunctionPanel::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange)
        {
            retranslate();
        }
    QWidget::changeEvent(event);
}
void FunctionPanel::retranslate(){
    prototype->setToolTip(tr("Saisir le nom et les arguments<br> <b>Exemple: </b> f(a,b)"));
    labelProto->setText(tr("Fonction"));
    varloc->setToolTip(tr("Saisir les noms de variables utilisées uniquement à l'intérieur du programme.<br>NB: Votre programme devra les initialiser.<br> <b> Exemple:</b> j,k"));
    labelVarloc->setText(tr("Variables locales:"));
    symbloc->setToolTip(tr("Un symbole est une lettre utilisable dans un calcul mathématique. On peut utiliser une variable globale non initialisée. En revanche si l'on souhaite avoir des symboles locaux, il faut les déclarer ici.<br> <b>Exemple:</b> t,u"));
    labelSymbloc->setText(tr("Symboles locaux:"));
    retour->setToolTip(tr("Objet retourné par la fonction. On peut ne rien retourner, ou retourner une liste ou une séquence."));
    labelRetour->setText(tr("Retour:"));
    instructions->setToolTip(tr("Vous pouvez si vous le souhaitez utiliser cette partie pour compléter votre programme."));
}

FunctionPanel::FunctionPanel(WizardAlgo *parent):AlgoTabChild(parent){
        algoPanel=parent;

        prototype=new QLineEdit;
        labelProto=new QLabel("");
        labelProto->setBuddy(prototype);

        varloc=new QLineEdit("");
        labelVarloc=new QLabel("");
        labelVarloc->setBuddy(varloc);

        symbloc=new QLineEdit("");
        labelSymbloc=new QLabel("");
        labelSymbloc->setBuddy(symbloc);


        retour=new QLineEdit("");
        labelRetour=new QLabel("");
        labelRetour->setBuddy(retour);


        QPushButton *button=new QPushButton;
        button->setIcon(QIcon(":/images/right.png"));


        QGroupBox *group=new QGroupBox();
        QHBoxLayout *layout=new QHBoxLayout;
        instructions=new QPlainTextEdit(this);
        layout->addWidget(instructions);
        //group->setMaximumWidth(200);
        group->setLayout(layout);

        QGridLayout *grid=new QGridLayout();
        grid->addWidget(labelProto,0,0);
        grid->addWidget(prototype,0,1);
        grid->addWidget(labelVarloc,1,0);
        grid->addWidget(varloc,1,1);
        grid->addWidget(labelSymbloc,2,0);
        grid->addWidget(symbloc,2,1);
        grid->addWidget(group,3,0,1,2);
        grid->addWidget(labelRetour,4,0);
        grid->addWidget(retour,4,1);
        grid->addWidget(button,5,1,Qt::AlignVCenter);
        this->setLayout(grid);

        connect(button,SIGNAL(clicked()),this,SLOT(sendCommand()));

    retranslate();
   }

void FunctionPanel::sendCommand(){
    QString s=tr("fonction ");//trad: proc
    QString tmp;
    QStringList listsymb;
    QString indent="\t";
    int i;
    s.append(prototype->text()).append(("\n"));

    tmp=varloc->text().trimmed();
    if(!tmp.isEmpty()){
      s.append(indent+"local ");
      if(!tmp.endsWith(";"))
          s.append(tmp).append(";\n");
    }

    tmp=symbloc->text().trimmed();
    if(!tmp.isEmpty()){
        listsymb=tmp.split(",");
        for(i=0;i<listsymb.length();i++){
            s.append(indent+tr("supposons("));//trad assume
            s.append(listsymb.at(i)).append(",symbol);\n");
        }
    }

    tmp=(instructions->document()->toPlainText()).trimmed();
    if(!tmp.isEmpty()){
        s.append(indent+tmp+"\n");
    }
    s.append(indent+"\n");
    tmp=retour->text();
    if(!tmp.trimmed().isEmpty()){
        s.append(indent+"retourne ").append(tmp).append(";\n");
    }
    s.append(tr("ffonction;"));// trad: end_proc
    algoPanel->sendCommand(s);

}

void LoopPanel::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange)
        {
            retranslate();
        }
    QWidget::changeEvent(event);
}

void LoopPanel::retranslate(){
    varname->setToolTip(tr("Saisir le nom de la variable. (Attention, i est reservé pour le nombre complexe)<br> <b>Exemple:</b> j"));
    labelVarname->setText(tr("Pour:"));
    deb->setToolTip(tr("Valeur de départ:<br> <b>Exemple:</b> 0"));
    labelDeb->setText(tr("de"));
    fin->setToolTip(tr("valeur de fin"));
    labelFin->setText(tr("jusque"));
    pas->setToolTip(tr("Précisez le pas s'il est différent de 1"));
    labelPas->setText(tr("pas"));

    instructions->setToolTip(tr("Vous pouvez si vous le souhaitez utiliser cette partie pour le corps de la boucle."));
    group->setTitle(tr("faire"));
}

LoopPanel::LoopPanel(WizardAlgo *parent):AlgoTabChild(parent){
        algoPanel=parent;

        varname=new QLineEdit;
        labelVarname=new QLabel("");
        labelVarname->setBuddy(varname);

        deb=new QLineEdit;
        labelDeb=new QLabel("");
        labelDeb->setBuddy(deb);

        fin=new QLineEdit;
        labelFin=new QLabel("");
        labelFin->setBuddy(varname);

        pas=new QLineEdit;
        labelPas=new QLabel("");
        labelPas->setBuddy(varname);

        QPushButton *button=new QPushButton;
        button->setIcon(QIcon(":/images/right.png"));


        group=new QGroupBox();
        QHBoxLayout *layout=new QHBoxLayout;
        instructions=new QPlainTextEdit(this);
        layout->addWidget(instructions);
        //group->setMaximumWidth(200);
        group->setLayout(layout);

        QGridLayout *grid=new QGridLayout();
        grid->addWidget(labelVarname,0,0);
        grid->addWidget(varname,0,1);

        grid->addWidget(labelDeb,1,0);
        grid->addWidget(deb,1,1);
        grid->addWidget(labelFin,2,0);
        grid->addWidget(fin,2,1);
        grid->addWidget(labelPas,3,0);
        grid->addWidget(pas,3,1);
        grid->addWidget(group,4,0,1,2);
        grid->addWidget(button,5,1,Qt::AlignVCenter);
        this->setLayout(grid);

        connect(button,SIGNAL(clicked()),this,SLOT(sendCommand()));

    retranslate();


}

void LoopPanel::sendCommand(){
    QString s="pour ";
    QString tmp;
    QString indent="\t";

    s.append(varname->text());
    s.append(" de ").append(deb->text());
    s.append(" jusque ").append(fin->text());
    tmp=pas->text();
    if(!tmp.trimmed().isEmpty()){
        s.append(" pas "+tmp+" ");
    }
    s.append(" faire\n");
    tmp=instructions->document()->toPlainText();
    if(!tmp.trimmed().isEmpty()){
        s.append(indent+tmp+"\n");
    }
    s.append("fpour;");
    algoPanel->sendCommand(s);
}
