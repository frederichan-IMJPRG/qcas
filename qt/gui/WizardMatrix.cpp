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
void WizardMatrix::createGui(){
    QGroupBox *box=new QGroupBox;
    box->setTitle(tr("Dimensions:"));
    QLabel *rowLabel=new QLabel(box);
    rowLabel->setText(tr("Lignes:"));
    QLabel *columnLabel=new QLabel(box);
    columnLabel->setText(tr("Colonnes:"));

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
    button->setText(tr("Go!"));

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
