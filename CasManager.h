#ifndef CASMANAGER_H
#define CASMANAGER_H
#include <QStringList>
#include "giac/gen.h"
class OutputWidget;
class MainWindow;
class QCompleter;
class FormalWorkSheet;
class CasManager{
public:
    CasManager();
    QString seekForKeyword(const QString &) const;
    QString displayPage(const QString &) const;
    QCompleter* getCompleter();
    bool isCommand(const QString &) const;
    OutputWidget* evaluate(const QString &);

private:
    MainWindow* mainWidow;
    giac::context *context;
    QCompleter *completer;
    void listAllCommands();
    QStringList commandList;
};
#endif // CASMANAGER_H
