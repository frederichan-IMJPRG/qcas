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

#ifndef PREFDIALOG_H
#define PREFDIALOG_H
#include <QDialog>
class MainWindow;
class QStackedWidget;
class QListWidget;
class PrefDialog;
class QComboBox;
class QPushButton;
class QLineEdit;
class QCheckBox;
class QSpinBox;




class CasPanel:public QWidget{
    Q_OBJECT
public:
    CasPanel(MainWindow*);
    void initValue();
    void apply();


private:
    MainWindow* mainWindow;
    QWidget* mainPanel;
    QWidget* advancedPanel;
    QComboBox * comboProg;
    QComboBox * comboFloat;
    QComboBox * comboBasis;
    QLineEdit * editDigits;
    QCheckBox * checkSymbolic;
    QCheckBox * checkRadian;
    QCheckBox * checkComplex;
    QCheckBox * checkComplexVar;
    QCheckBox * checkPolynomialDecrease;
    QCheckBox * checkAlltrig;
    QCheckBox * checkSqrt;
    QPushButton* buttonAdvanced;

    QLineEdit*editEpsilon;
    QLineEdit*editProbaEpsilon;
    QSpinBox* spinRecursProg;
    QSpinBox* spinRecursEval;
    QSpinBox* spinEvalInProg;
    QSpinBox* spinNewton;
    QLineEdit* editDebugInfo;


    QPushButton* backButton;


    void initGui();

private slots:
    void switchPanel();
};

class GeneralPanel:public QWidget{
public:
    GeneralPanel();
    void initValue();
    void apply();
private:
    QLineEdit* editWidth;
    QComboBox* comboLanguage;
    QLineEdit* editXMin;
    QLineEdit* editXMax;
    QLineEdit* editYMin;
    QLineEdit* editYMax;
    QLineEdit* editZMin;
    QLineEdit* editZMax;
    QLineEdit* editTMin;
    QLineEdit* editTMax;
    QCheckBox* checkAutoScale;

    void initGui();


};
class PrefDialog: public QDialog{
    Q_OBJECT
public:
    PrefDialog(MainWindow* );
    void initValue();
private:
    MainWindow* mainWindow;
    QListWidget* listWidget;
    QStackedWidget* stackWidget;
    CasPanel* casPanel;
    GeneralPanel* generalPanel;
    QWidget* spreadSheetPanel;
    QWidget* interactive2dPanel;
    QPushButton* okButton;
    QPushButton* cancelButton;
    void initGui();
private slots:
    void apply();
};



#endif // PREFDIALOG_H
