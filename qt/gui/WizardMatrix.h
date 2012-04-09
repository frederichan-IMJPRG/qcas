#ifndef WIZARDMATRIX_H
#define WIZARDMATRIX_H

# include <QWidget>
#include "../MainWindow.h"
class QTableWidget;
class QSpinBox;
class WizardMatrix: public QWidget{
    Q_OBJECT
public:
    WizardMatrix(MainWindow *parent=0);
private:
    void createGui();
    QSpinBox *rowSpin;
    QSpinBox *columnSpin;
    QTableWidget *table;
    MainWindow *mainWindow;
private slots:
    void modifyRow(int);
    void modifyColumn(int);
    void writeMatrix();
};
#endif // WIZARDMATRIX_H
