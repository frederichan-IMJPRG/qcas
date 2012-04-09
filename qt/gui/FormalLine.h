#ifndef FORMALLINE_H
#define FORMALLINE_H
#include <QWidget>
#include "giac/gen.h"
class QGridLayout;
class TextInput;
class FormalWorkSheet;
class OutputWidget;
class QToolButton;
class QCheckBox;

class Line: public QWidget{
    Q_OBJECT
public:
    Line(int id,FormalWorkSheet* parent=0);
    ~Line();
    FormalWorkSheet* getWorkSheet() const;
    TextInput* getTextInput() const;
    int getId() const;
    OutputWidget* getOuputWidget();
    void evaluate(const QString&);
    void displayResult(OutputWidget*);
    OutputWidget* gen2Widget(const QString &);
    void addStopButton(QToolButton* stop);
    void removeStopButton();
private:
    int id;
    FormalWorkSheet* workSheet;
    QGridLayout *mainLayout;
    QCheckBox *check;
    TextInput *input;
    OutputWidget *out;

private slots:
    void selectLevel();
};

#endif // FORMALLINE_H
