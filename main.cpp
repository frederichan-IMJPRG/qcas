#include <QApplication>
#include <QTextCodec>
#include "MainWindow.h"

int main(int argc, char * argv []){
    QApplication app(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    MainWindow win;
    win.show();

    return app.exec();

}
