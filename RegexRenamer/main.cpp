#include "RegexRenamer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RegexRenamer w;
    w.show();
    return a.exec();
}
