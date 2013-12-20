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

#ifndef WIZARDALGO_H
#define WIZARDALGO_H
#include <QWidget>
#include <QVector>
#include <QSpinBox>
class QComboBox;
class QStackedWidget;
class QLineEdit;
class QCheckBox;
class QPlainTextEdit;
class QGridLayout;
class QLabel;
class QGroupBox;
class QPushButton;
class MainWindow;
class WizardAlgo : public QWidget{
    Q_OBJECT
public:
    WizardAlgo(MainWindow *parent=0);
    void sendCommand(const QString &);
    void cursorUp();
private:
    MainWindow* mainWindow;
    void changeEvent(QEvent *);
    void retranslate();
    void createGui();
    QStackedWidget* pages;
    QComboBox *list;

};

class AlgoTabChild:public QWidget{
    Q_OBJECT
public:
    AlgoTabChild(QWidget *parent=0);
public slots:
    virtual void sendCommand()=0;
};

class FunctionPanel:public AlgoTabChild{
    Q_OBJECT
public:
    FunctionPanel(WizardAlgo *parent=0);
public slots:
    void sendCommand();

private:
    WizardAlgo* algoPanel;
    QLineEdit *prototype;
    QLabel *labelProto;
    QLineEdit *varloc;
    QLabel *labelVarloc;
    QLineEdit *symbloc;
    QLabel *labelSymbloc;
    QPlainTextEdit *instructions;
    QLineEdit *retour;
    QLabel *labelRetour;

    void retranslate();
    void changeEvent(QEvent *);
};

class LoopPanel:public AlgoTabChild{
    Q_OBJECT
public:
    LoopPanel(WizardAlgo *parent=0);
public slots:
    void sendCommand();

private:
    WizardAlgo* algoPanel;
    QLineEdit *varname;
    QLabel *labelVarname;
    QLineEdit *deb;
    QLabel *labelDeb;
    QLineEdit *fin;
    QLabel *labelFin;
    QLineEdit *pas;
    QLabel *labelPas;
    QPlainTextEdit *instructions;
    QGroupBox *group;

    void retranslate();
    void changeEvent(QEvent *);
};



#endif // WIZARDALGO_H
