#ifndef WIZARDEQUATION_H
#define WIZARDEQUATION_H
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
class QPushButton;
class MainWindow;
class WizardEquation : public QWidget{
    Q_OBJECT
public:
    WizardEquation(MainWindow *parent=0);
  void sendEquation(const QString &);
private:
    MainWindow* mainWindow;
    void createGui();
    QStackedWidget* pages;
    QComboBox *list;



};

class TabChild:public QWidget{
    Q_OBJECT
public:
    TabChild(QWidget *parent=0);
public slots:
    virtual void sendEquation()=0;
};

class EqPanel:public TabChild{
    Q_OBJECT
public:
    EqPanel(WizardEquation *parent=0);
public slots:
    void sendEquation();
    void numericCheck(int);
    void inC_Check(int);

private:
    WizardEquation* eqPanel;
    QLineEdit *eq;
    QLineEdit *var;
    QCheckBox *numeric;
    QCheckBox *inC;

};

class DiffPanel:public TabChild{
    Q_OBJECT
public:
    DiffPanel(WizardEquation *parent=0);
public slots:
    void sendEquation();
private:
    WizardEquation* eqPanel;

    QLineEdit *eq;
    QLineEdit *var;
    QPlainTextEdit *editor ;
};
class MySpin:public QSpinBox{
public:
    MySpin(QWidget *parent=0);
};
class SystPanel:public TabChild{
    Q_OBJECT
public:
    SystPanel(WizardEquation *parent=0);
public slots:
    void sendEquation();

private:
    WizardEquation* eqPanel;
    void updateGrid();
    QGridLayout* grid;
    MySpin *spin;
    QLabel *labelSpin,*labelVar;
    QPushButton *button;
    QVector<QLabel*> *label;
    QLineEdit *var;
    QVector<QLineEdit*> *edit;
    QCheckBox *check;
private slots:
    void addEquations(int);
};


#endif // WIZARDEQUATION_H
