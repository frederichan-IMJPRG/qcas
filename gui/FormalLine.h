#ifndef FORMALLINE_H
#define FORMALLINE_H
#include <QWidget>


class QGridLayout;
class QCheckBox;
class TextInput;
class FormalWorkSheet;
class OutputWidget;

class Line: public QWidget{
    Q_OBJECT
public:
    Line(int id,FormalWorkSheet* parent=0);
    FormalWorkSheet* getWorkSheet() const;
    TextInput* getTextInput() const;
 //   void setId(int c)const;
    int getId() const;
    void output(const QString&);
private:
    int id;
    FormalWorkSheet* workSheet;
    QGridLayout *mainLayout;
    QCheckBox* check;
    TextInput *input;
    OutputWidget *out;

};

#endif // FORMALLINE_H
