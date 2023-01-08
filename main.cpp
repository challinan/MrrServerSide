#include <QCoreApplication>
#include "mainprocess.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    MainProcess mp = MainProcess(&a, argc, argv);

    return a.exec();
}
