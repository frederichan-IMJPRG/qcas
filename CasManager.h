#ifndef CASMANAGER_H
#define CASMANAGER_H
#include <QStringList>
#include <streambuf>
#include "giac/gen.h"
class OutputWidget;
class MainWindow;
class QCompleter;
class FormalWorkSheet;
class Highlighter;
/**
 the class mybuf is used to redirect the standard std::cout
 We can catch print("....") commands, and all comment for libgiac
  **/

class mybuf: public std::streambuf{
 private:
   void    put_buffer(void);
   void    put_char(int);
   protected:
   int    overflow(int);
   int    sync();
   public:
   mybuf(int = 0);
   // ~mybuf();
 };
class MyStream: public std::ostream{
 public:
   MyStream(int = 0);
 };

/**
  The main class which handles with the libGiac
  This is an interface for main Window to deal with the libGiac (run command, add to history ...)
  **/
class CasManager{
public:
    CasManager();
    QString seekForKeyword(const QString &) const;
    QString displayPage(const QString &) const;
    QCompleter* getCompleter() const;
    QStringList getCommands();
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
