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

#include <QGroupBox>
#include <QSpinBox>
#include <iostream>
#include <QLabel>
#include <QTableWidget>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "WizardMatrix.h"

WizardMatrix::WizardMatrix(MainWindow *parent)
    : QWidget(parent)
{
    mainWindow=parent;
    createGui();
}
void WizardMatrix::changeEvent(QEvent *event){
    if(event->type() == QEvent::LanguageChange)
        {
            retranslate();
        }
    QWidget::changeEvent(event);
}
void WizardMatrix::retranslate(){
    box->setTitle(tr("Dimensions:"));
    rowLabel->setText(tr("Lignes:"));
    columnLabel->setText(tr("Colonnes:"));
}
void WizardMatrix::createGui(){
    box=new QGroupBox;
    rowLabel=new QLabel(box);
    columnLabel=new QLabel(box);


    rowSpin=new QSpinBox(this);
    rowSpin->setMaximum(10);
    rowSpin->setMinimum(2);
    rowSpin->setValue(2);

    columnSpin=new QSpinBox(this);
    columnSpin->setMaximum(10);
    columnSpin->setMinimum(2);
    columnSpin->setValue(2);

    QGridLayout *grid=new QGridLayout;
    grid->addWidget(rowLabel,0,0,Qt::AlignLeft);
    grid->addWidget(columnLabel,1,0,Qt::AlignLeft);
    grid->addWidget(rowSpin,0,1,Qt::AlignLeft);
    grid->addWidget(columnSpin,1,1,Qt::AlignLeft);

    QPushButton* button=new QPushButton;
    button->setIcon(QIcon(":/images/right.png"));

    QHBoxLayout *hLayout=new QHBoxLayout;
    hLayout->addWidget(box);
    hLayout->addWidget(button);

    table=new QTableWidget;
    table->setRowCount(0);
    table->setColumnCount(0);

    table->setRowCount(2);
    table->setColumnCount(2);;
    QHBoxLayout *bottom=new QHBoxLayout;
    table->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);
    bottom->addWidget(table);

    QVBoxLayout *vLayout=new QVBoxLayout;
    box->setLayout(grid);
    vLayout->addLayout(hLayout);
    vLayout->addLayout(bottom);

    this->setLayout(vLayout);

    connect(rowSpin,SIGNAL(valueChanged(int)),this,SLOT(modifyRow(int)));
    connect(columnSpin,SIGNAL(valueChanged(int)),this,SLOT(modifyColumn(int)));
    connect(button,SIGNAL(clicked()),this,SLOT(writeMatrix()));

    retranslate();

    show();
}

void WizardMatrix::modifyRow(int row){
    if (row>table->rowCount()){
        table->insertRow(row-1);
    }
    else {
        table->removeRow(row);
    }
}
void WizardMatrix::modifyColumn(int col){
    if (col>table->columnCount()){
        table->insertColumn(col-1);
    }
    else {
        table->removeColumn(col);
    }
}

void WizardMatrix::writeMatrix(){
    QString s("[");
    for (int i=0;i<table->rowCount();++i){
        s.append("[");
        for(int j=0;j<table->columnCount();++j){
            if (table->item(i,j)!=0)
                s.append(table->item(i,j)->text());
            if (j!=table->columnCount()-1){
                s.append(",");
            }
        }
        s.append("]");
        if (i!=table->rowCount()-1){
            s.append(",");
        }
    }
    s.append("]");
    mainWindow->sendText(s);
}
