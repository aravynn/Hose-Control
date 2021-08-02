#include "hosetracker.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HoseTracker w;
    w.show();
    return a.exec();
}
