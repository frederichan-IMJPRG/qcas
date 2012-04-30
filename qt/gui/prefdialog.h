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
    QWidget* spreadSheetPanel;
    QWidget* interactive2dPanel;
    QPushButton* okButton;
    QPushButton* cancelButton;
    void initGui();
private slots:
    void apply();
};



#endif // PREFDIALOG_H
