#include <QStackedWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QCheckBox>
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include "../MainWindow.h"
#include "prefdialog.h"
#include "global.h"
#include <QDebug>;
CasPanel::CasPanel(MainWindow *parent):QWidget(parent){
    mainWindow=parent;
    initGui();
}
void CasPanel::initGui(){
    QGridLayout* grid=new QGridLayout;

    QLabel* labelProg=new QLabel(tr("Langage utilisé"));
    labelProg->setToolTip(tr("<ul>"
                             "<li><b>XCAS:</b><br> <tt> maple_mode(0)</tt></li>"
                             "<li><b>Maple:</b><br> <tt> maple_mode(1)</tt></li>"
                             "<li><b>MuPad:</b><br> <tt> maple_mode(2)</tt></li>"
                             "<li><b>TI 89/92:</b><br> <tt> maple_mode(3)</tt></li>"
                             "</ul>"));
    comboProg=new QComboBox(this);
    comboProg->addItem(tr("XCAS"));
    comboProg->addItem(tr("Maple"));
    comboProg->addItem(tr("MuPad"));
    comboProg->addItem(tr("TI 89/92"));

    QLabel* labelFloat=new QLabel(tr("Format des nombres à virgule"));
    labelFloat->setToolTip("<p><ul>"
                           "<li><b>Standard:</b><p>150.12 s'affiche 150.12</p></li>"
                           "<li><b>Scientifique:</b><p> 150.12 s'affiche 1.5012e+2</p></li>"
                           "<li><b>Standard:</b><p> 150.12 s'affiche 150.12e+0</p></li>"
                           "</ul></p>");
    comboFloat=new QComboBox(this);
    comboFloat->addItem(tr("Standard"));
    comboFloat->addItem(tr("Scientifique"));
    comboFloat->addItem(tr("Ingénieur"));


    QLabel* labelBasis=new QLabel(tr("Entiers en base ..."));
    comboBasis=new QComboBox(this);
    comboBasis->addItem(tr("10"));
    comboBasis->addItem(tr("16"));
    comboBasis->addItem(tr("8"));

    QLabel* labelDigits=new QLabel(tr("Nombre de décimales"));
    editDigits=new QLineEdit(this);
    QIntValidator* validator=new QIntValidator;
    editDigits->setValidator(validator);

    checkSymbolic=new QCheckBox(tr("Calcul symbolique"));
    checkSymbolic->setToolTip(tr("<p><b>Calcul en mode exact ou approché.</b></p> <p><tt>approx_mode:=1 </tt> (mode approximatif)</p><p><tt>approx_mode:=0 </tt> (mode exact)</p>"));
    checkRadian=new QCheckBox(tr("Angle en radian"));
    checkRadian->setToolTip(tr("<p><b>Spécifie l'unité choisie pour les angles.</b></p>"
                               "<p><tt>angle_radian:=1</tt> (radians)</p>"
                               "<p><tt>angle_radian:=0</tt> (degrés)</p>"
                               ));
    checkComplex=new QCheckBox(tr("Calcul en mode complexe"));
    checkComplex->setToolTip(tr("<p><b>Spécifie si l'on travaille en mode réel ou complexe.</b></p>"
                               "<p><tt>complex_mode:=1</tt> (mode complexe)</p>"
                               "<p><tt>complex_mode:=0</tt> (mode réel)</p>"
                                "<b>Exemple: </b> <tt>factor(x^4-1)</tt>"));
    checkComplexVar=new QCheckBox(tr("Variables à valeur complexe"));
    checkComplexVar->setToolTip(tr("<p><b>Spécifie si l'on travaille avec des variables réelles ou complexes.</b></p>"
                               "<p><tt>complex_variables:=1</tt> (mode complexe)</p>"
                               "<p><tt>complex_variables:=0</tt> (mode réel)</p>"));

    checkPolynomialDecrease=new QCheckBox(tr("Affichage des polynômes en puissances décroissantes"));
    checkPolynomialDecrease->setToolTip(tr("<p></p>"));

    checkAlltrig=new QCheckBox(tr("Solutions trigonométriques générales"));
    checkAlltrig->setToolTip(tr("<p><ul>"
                                "<li><tt>solve(cos (x)=1)</tt><br><tt><b>Réponse:</b> [0]</tt></li>"
                                "<li>Sinon,<br><tt>solve(cos (x)=1)</tt><br><b>Réponse:</b> <tt>[2*n*pi]</tt></li>"
                                "</ul></p>"));
    checkSqrt=new QCheckBox(tr("factorisation avec racines carrées"));
    buttonAdvanced=new QPushButton(tr("Avancé..."));
    buttonAdvanced->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    grid->addWidget(labelProg,0,0);
    grid->addWidget(comboProg,0,1);
    grid->addWidget(labelFloat,1,0);
    grid->addWidget(comboFloat,1,1);
    grid->addWidget(labelBasis,2,0);
    grid->addWidget(comboBasis,2,1);
    grid->addWidget(labelDigits,3,0);
    grid->addWidget(editDigits,3,1);
    grid->addWidget(checkSymbolic,4,0);
    grid->addWidget(checkRadian,5,0);
    grid->addWidget(checkComplex,6,0);
    grid->addWidget(checkComplexVar,7,0);
    grid->addWidget(checkPolynomialDecrease,4,1);
    grid->addWidget(checkAlltrig,5,1);
    grid->addWidget(checkSqrt,6,1);
    grid->addWidget(buttonAdvanced,7,1,Qt::AlignCenter);


    setLayout(grid);
}

void CasPanel::initValue(){
    giac::context* c=mainWindow->getContext();
    comboProg->setCurrentIndex(giac::xcas_mode(c));
    comboFloat->setCurrentIndex(giac::scientific_format(c));
    comboBasis->setCurrentIndex(giac::integer_format(c));
    editDigits->setText(QString::number(giac::decimal_digits(c)));
    if (giac::approx_mode(c)==0) checkSymbolic->setChecked(true);
    else checkSymbolic->setChecked(false);
    if (giac::angle_radian(c)==1) checkRadian->setChecked(true);
    else checkRadian->setChecked(false);
    if (giac::complex_mode(c)==1) checkComplex->setChecked(true);
    else checkComplex->setChecked(false);
    if (giac::complex_variables(c)==1) checkComplexVar->setChecked(true);
    else checkComplexVar->setChecked(false);
    if (giac::increasing_power(c)==1) checkPolynomialDecrease->setChecked(false);
    else checkPolynomialDecrease->setChecked(true);
    if (giac::all_trig_sol(c)==1) checkAlltrig->setChecked(true);
    else checkAlltrig->setChecked(false);
    if (giac::withsqrt(c)==1) checkSqrt->setChecked(true);
    else checkSqrt->setChecked(false);

}
void CasPanel::apply(){
    giac::context* c=mainWindow->getContext();
    giac::xcas_mode(comboProg->currentIndex(),c);
    giac::scientific_format(comboFloat->currentIndex(),c);
    giac::integer_format(comboBasis->currentIndex(),c);
    int d=editDigits->text().toInt();
    if (d<0) d=12;
     giac::decimal_digits(d,c);
    if (checkSymbolic->isChecked()) giac::approx_mode(0,c);
    else giac::approx_mode(1,c);
    if (checkRadian->isChecked()) giac::angle_radian(1,c);
    else giac::angle_radian(0,c);
    if (checkComplex->isChecked()) giac::complex_mode(1,c);
    else giac::complex_mode(0,c);
    if (checkComplexVar->isChecked()) giac::complex_variables(1,c);
    else giac::complex_variables(0,c);
    if (checkPolynomialDecrease->isChecked()) giac::increasing_power(0,c);
    else giac::increasing_power(1,c);
    if (checkAlltrig->isChecked()) giac::all_trig_sol(1,c);
    else giac::all_trig_sol(0,c);
    if (checkSqrt->isChecked()) giac::withsqrt(1,c);
    else giac::withsqrt(0,c);
}
PrefDialog::PrefDialog(MainWindow * parent){
    mainWindow=parent;
    setVisible(false);
    initGui();
}

void PrefDialog::initGui(){
    casPanel=new CasPanel(mainWindow);
    spreadSheetPanel=new QWidget(this);
    interactive2dPanel=new QWidget(this);

    stackWidget=new QStackedWidget;
    stackWidget->addWidget(casPanel);
    stackWidget->addWidget(spreadSheetPanel);
    stackWidget->addWidget(interactive2dPanel);


    listWidget=new QListWidget;
    listWidget->setIconSize(QSize(48,48));
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setFlow(QListView::LeftToRight);
    listWidget->setViewMode(QListView::IconMode);
//    listWidget->setUniformItemSizes(true);
    QListWidgetItem* casItem=new QListWidgetItem(QIcon(":/images/cas.png"),tr("CAS"));
  casItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
  casItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    QListWidgetItem* spreadItem=new QListWidgetItem(QIcon(":/images/spreadsheet.png"),tr("Tableur"));
    spreadItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    spreadItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);
    QListWidgetItem* interactive2dItem=new QListWidgetItem(QIcon(":/images/line.png"),tr("Géométrie 2D"));
    interactive2dItem->setTextAlignment(Qt::AlignHCenter|Qt::AlignBottom);
    interactive2dItem->setFlags(Qt::ItemIsSelectable|Qt::ItemIsEnabled);

    listWidget->addItem(casItem);
    listWidget->addItem(spreadItem);
    listWidget->addItem(interactive2dItem);
    listWidget->adjustSize();
    listWidget->setCurrentRow(0);
    okButton=new QPushButton(tr("Ok"));
    okButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    cancelButton=new QPushButton(tr("Annuler"));
    cancelButton->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    QWidget* buttonsPanel=new QWidget;
    QHBoxLayout* hbox=new QHBoxLayout;
    hbox->addWidget(okButton,1,Qt::AlignRight);
    hbox->addWidget(cancelButton,0,Qt::AlignRight);
    buttonsPanel->setLayout(hbox);

    QVBoxLayout* vbox=new QVBoxLayout;
    listWidget->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Maximum);
    listWidget->setMaximumHeight(80);
    vbox->addWidget(listWidget);
    vbox->addWidget(stackWidget);
    vbox->addWidget(buttonsPanel);
    setLayout(vbox);
    connect(listWidget,SIGNAL(currentRowChanged(int)),stackWidget,SLOT(setCurrentIndex(int)));
    connect(cancelButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(okButton,SIGNAL(clicked()),this,SLOT(apply()));

}
void PrefDialog::apply(){
    casPanel->apply();

//    giac::
//    giac::set_language(2,mainWindow->getContext());
    close();

}

void PrefDialog::initValue(){
    casPanel->initValue();
}
