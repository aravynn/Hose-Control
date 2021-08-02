#ifndef TIMECONVERT_H
#define TIMECONVERT_H

#include <vector>
#include <QDateTime>
#include <QDateTimeEdit>
#include <TrackerEnums.h>
#include <TrackerStructs.h>
#include <math.h>
#include <QSqlError>
#include <QDebug>

/**
 * @brief The TimeConvert class
 *
 * This class will be used for converting to and from QDateTime objects
 * will convert to TEXT based numberical qstrings, 32bit ints,
 *
 * time must always be considerate of GMT-6 (central) time. Will need at also consider daylight savings.
 * WE WILL ignore the concept of time for this, since the time of creation is useless.
 * Therefore, this will require the creation of a new numerical value, which will be an epoch time, but for days, not seconds
 * daypoch will be equivilant to (year*365) + floor(year/4) + dayofyear
 *
 */

class TimeConvert
{
private:
    // total lengths for each month.
    std::vector<int> monthLengths = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
public:
    TimeConvert();
    QDate convert(int year, int month, int day); //int-based date and time.
    QDate convert(TimeInt daypoch);
    QDate convert(QString datetime = "NOW"); // date/time as a string
    TimeInt convert(QDate datetime); //date/time object
    QString print(QDate datetime); // print date at mm/dd/yyyy
    QDate Today(); // return today.
};

#endif // TIMECONVERT_H
