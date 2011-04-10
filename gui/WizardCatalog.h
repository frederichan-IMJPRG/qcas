#ifndef WIZARDCATALOG_H
#define WIZARDCATALOG_H
#include <QWidget>
#include <QUrl>
class QLineEdit;
class QTextBrowser;
class QStringList;
class MainWindow;
class QAction;

class WizardCatalog :public QWidget{
    Q_OBJECT
public:
    WizardCatalog(MainWindow *parent=0);
private:
    void createGui();
    void addHistory(const QString &st);
    void updateButtons();
    MainWindow* mainWindow;
    QStringList *history;
    int historyIndex;
    QLineEdit* lineEdit;
    QTextBrowser *zone;
    QAction* previousAction;
    QAction* nextAction;
public slots:
   void displayPage(QUrl);

private slots:
    void find();
    void goBack();
    void goNext();
};


#endif // WIZARDCATALOG_H
