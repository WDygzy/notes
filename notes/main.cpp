#include "mainwindow.h"
#include "selfdrive.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    // SetProcessAutoRunSelf();

    MainWindow w;
    //设置透明度
    w.setWindowOpacity(0.8);
    w.setWindowTitle("notes");

    w.show();
    return a.exec();
}
