#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <QCompleter>
#include <ostream>
#include "CasManager.h"
#include "giac/giac.h"
#include "gui/FormalLineWidgets.h"
#include "gui/src/qtmmlwidget.h"
//#include "MainWindow.h"
using namespace giac;
using namespace std;

mybuf::mybuf(int bsize): streambuf(){
  if (bsize)
    {
      char	*ptr = new char[bsize];
      setp(ptr, ptr + bsize);
    }
  else
    setp(0, 0);

  setg(0, 0, 0);
}

int mybuf::overflow(int c){
  put_buffer();
  if (c != EOF)
    if (pbase() == epptr())
      put_char(c);
    else
      sputc(c);
  return 0;
}


int    mybuf::sync(){
  put_buffer();
  return 0;
}


void    mybuf::put_char(int chr){

    cerr << "1 caractere" << char(chr) << endl;

  // mettre ici votre code de sortie java du caractere de code ascii chr
}


void     mybuf::put_buffer(){
if (pbase() != pptr())
  {
    int     len = (pptr() - pbase());
    char    * buffer = new char[len + 1];
    strncpy(buffer, pbase(), len);
    buffer[len] = 0;
    // mettre ici le code de sortie java de la chaine buffer
    setp(pbase(), epptr());
    cerr << "1 chaine " << endl;
    qDebug()<<"debug "<<buffer;
    delete [] buffer;
  }
}


MyStream::MyStream(int bsize):ostream(new mybuf(bsize)) {}


CasManager::CasManager(){

    context=new giac::context;
    listAllCommands();
    completer=new QCompleter(commandList);

   logptr(new MyStream,context);

}


void CasManager::listAllCommands(){
    QFile file("aide_cas");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString line;
    // Only take command with alpanumeric characters
    // For example %{ %} is excluded.

    QRegExp expr("([a-z]|[A-Z]|[_]|[0-9])+");

    while(!stream.atEnd()){
        line=stream.readLine();
        if (line.startsWith("#")) {
            QStringList list=line.remove(0,2).split(" ",QString::SkipEmptyParts);
            for(int i=0;i<list.size();++i){
                QString s(list.at(i));
                if (expr.exactMatch(s))
                    commandList.append(list.at(i));
            }
        }
    }
    commandList.sort();
}



QString CasManager::displayPage(const QString& keyWord) const{
    QFile file(":/aide_cas");
    file.open(QIODevice::ReadOnly);
    QTextStream stream(&file);
    QString command,description;
    QStringList synonym,seeAlso,examples;
    QString line;
    while(!stream.atEnd()){
        line=stream.readLine();
        if (line.startsWith("#")&& (line.contains(" "+keyWord+" ",Qt::CaseSensitive)||line.endsWith(" "+keyWord,Qt::CaseSensitive))) {
            QStringList list=line.split(" ",QString::SkipEmptyParts);
            for(int i=0;i<list.size();++i){
                if (i==1) command=list.at(1);
                else if (i>1) synonym.append(list.at(i));
            }
            do{
                line=stream.readLine();
                if (line.startsWith("#")) break;
                if (line.startsWith("0")){
                    list=line.split(" ",QString::SkipEmptyParts);
                    if (list.size()>1){
                        command.append("(").append(list.at(1)).append(")");
                    }
                }
                else if (line.startsWith("1")){
                    description=line.remove(0,2);
                }
                else if (line.startsWith("-")){
                    list=line.split(" ",QString::SkipEmptyParts);
                    if (list.size()>1) seeAlso.append(list.at(1));
                }
                else if (line.length()>0 ){
                    QChar c=line.at(0);
                    if (!QString("123456789").contains(c))
                     examples.append(line);
                }
            }  while (!stream.atEnd());
            break;
        }
        }
        line.clear();
        line.append("<h3><font color=\"#40A497\">").append(command).append("</font></h3>");
        if (!synonym.isEmpty()){
            line.append("<b>").append(QObject::tr("Synonyme(s):")).append("</b>");
            for( int i=0;i<synonym.size();++i){
                line.append(" ").append(synonym.at(i));
            }
        }
        line.append("<hr>");
        line.append(description);
        line.append("<br>");
        if (!examples.isEmpty()){
            line.append("<br><b>").append(QObject::tr("Exemples:")).append("</b><br>");
            for( int i=0;i<examples.size();++i){
                line.append(examples.at(i)).append("<br>");
            }
        }
        if (!seeAlso.isEmpty()){
            line.append(QObject::tr("Voir aussi: ")).append("<ul>");
            for( int i=0;i<seeAlso.size();++i){
                line.append("<li><a href=\"").append(seeAlso.at(i)).append("\">").append(seeAlso.at(i)).append("</a></li>");
            }
            line.append("</ul>");
        }
        return(line);
    }

QString CasManager::seekForKeyword(const QString & keyWord) const{
    QFile file(":/aide_cas");
        file.open(QIODevice::ReadOnly);
        QTextStream stream(&file);
        QString currentCommand;
        QString html;
        QString line;
        while(!stream.atEnd()){
            line=stream.readLine();
            if (line.startsWith("#")) currentCommand=line.remove(0,2);
            else if (line.startsWith("1")&&
                     (line.contains(keyWord,Qt::CaseInsensitive)||currentCommand.contains(keyWord,Qt::CaseInsensitive))){
                html.append("<a href=\"");
                html.append(currentCommand);
                html.append("\">");
                html.append(currentCommand);
                html.append("</a><br>\n");
                html.append(line.remove(0,2));
                html.append("<br><br>");
            }
        }
        if (html.isEmpty()) {
            return(QObject::tr("Aucun résultat"));
        }
        else return(html);

    }
bool CasManager::isCommand(const QString &st) const{
    return (commandList.contains(st));
}
QCompleter* CasManager::getCompleter() const{
    return completer;
}
QStringList CasManager::getCommands(){
    return commandList;
}

OutputWidget* CasManager::evaluate(const QString &formula){
    signal(SIGINT,giac::ctrl_c_signal_handler);
    giac::child_id=1;
    giac::gen expression(formula.toStdString(),context);
    gen answer=protecteval(expression,25,context);

    // Add result to history
    giac::history_in(context).push_back(expression);
    giac::history_out(context).push_back(answer);

    // translate result to mathml
    QString mathml(giac::gen2mathml(answer,context).c_str());
    mathml.prepend("<math mode=\"display\">\n");
    mathml.append("\n</math>");
    qDebug()<<"----------------------";
    qDebug()<<formula;
    qDebug()<<mathml;
    qDebug()<<"----------------------";
    QtMmlWidget *mmlWidget=new QtMmlWidget;

    QString errorMsg;
      int errorLine;
      int errorColumn;
      bool ok = mmlWidget->setContent(mathml, &errorMsg, &errorLine, &errorColumn);
      if (!ok) {
        qWarning("MathML error: %s, Line: %d, Column: %d",
        errorMsg.toLatin1(), errorLine, errorColumn);
      }
      QPalette p=mmlWidget->palette();
      p.setColor(QPalette::WindowText,QColor::fromRgb(0,0,255));
      mmlWidget->setPalette(p);
      return new FormulaWidget(mmlWidget);


  }
