#include <QApplication>
#include "bigxmlmainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow mainWin;
    mainWin.show();
    mainWin.open();
    return app.exec();
}
