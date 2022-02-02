#include "hosetracker.h"

#include <QApplication>

/**
 * @brief main
 *
 * Like all good C++ projects, the main function is the loading point for this application.
 * No changes should occur here.
 *
 * Ni!
 */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HoseTracker w;
    w.show();
    return a.exec();
}
