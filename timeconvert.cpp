#include "timeconvert.h"

//daypoch will be equivilant to (year*365) + floor(year/4) + dayofyear

TimeConvert::TimeConvert()
{
 // no initializer needed.
}

QDate TimeConvert::convert(int year, int month, int day){ // assume works.
    // convert the ints to a datetime object
    return QDate(year, month, day);

} //int-based date and time.

QDate TimeConvert::convert(TimeInt daypoch){ // works

    // takes the integer value for days(daypoch) and convert that number into a QDate object.

    // take time / 365 to get years
    int years = floor(daypoch / 365.25 );

    // take remaining time
    int remaining = daypoch % years;

    remaining -= floor(years / 4);

    int month = 0;
    for(int i{0}; i < 12; ++i){
        if(remaining - monthLengths.at(i) > 0){
            // this is more than a month
            remaining -= monthLengths.at(i);

            if(i == 1 && years % 4 == 0){
                // leap year, minus an extra day.
                --remaining;
            }

            ++month;
        } else {
            ++month;
            break;
        }
    }

    return QDate(years, month, remaining); // remaining seems to return 1 day ahead of testing date. this should compensate.
}

QDate TimeConvert::convert(QString datetime){ // works

    // convert datetime string to QDate

    TimeInt daypoch = datetime.toLongLong(); // convert to long long for useage in coversion.

  //  qWarning() << "Daypoch: " << daypoch;

    return convert(daypoch);
} // date/time as a string

TimeInt TimeConvert::convert(QDate datetime){ // works

    // convert to integer "daypoch"

    long long years = datetime.year();
    long long days = datetime.dayOfYear();

    return ( years * 365 ) + (int)floor( years / 4.0 ) + days; // alleged correct daypoch
} //date/time object

QDate TimeConvert::Today(){ // works
    // probably the most common use.
    return QDate::currentDate();
}

QString TimeConvert::print(QDate datetime){ // works
    // print formatted time as month/day/year.
    return datetime.toString("MM/dd/yyyy");

}
