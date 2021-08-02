#ifndef TRACKERSTRUCTS_H
#define TRACKERSTRUCTS_H

#include <QString>
#include <vector>
#include "TrackerEnums.h"

// USING statements will be declared here.
using CompanyID = long;
using StrPair = std::vector<std::pair<QString, QString>>;
using TimeInt = long long; // use a long long

// consts definitions
const long MaximumTestTime = 3600;
const int MaximumTableLines = 1000;

// const starter values for bar generation.
const double ULPressure[6] = {5.0, 15.4, 25.0, 65.5, 72.3, 77.0};
const double CSAPressure[6] = {25.5, 84.4, 159.2, 277.8, 339.0, 355.0};

// structs to be declared here.
struct Address{
    // address object for all general address, either billing or location.
    QString Addr1;
    QString Addr2;
    QString City;
    QString Province;
    QString Postal;
    QString Country;
    AddrType Type; // use ENUM for type.
    QString Name;
    CompanyID id; // PK for accessing the SQL database line.

    QString fullInfo(){
       return Addr1 + " | " + Addr2 + ' ' + City + ' ' + Province + ' ' + Postal + ' ' + Country + ' ' + Name;
    }
};

struct ContactPerson{
    CompanyID id; // ID for DB use. non-DB objects will ignore this.
    QString Name;
    std::vector<QString> Emails;
    std::vector<QString> PhoneNumbers;
};

// for passing information to and from SQL Database.
enum class DPO {
    STRING,
    NUMBER,
    DOUBLE
}; // only 3 options for Datapass, since

struct DataPass {
    QString str;
    CompanyID num;
    double dbl;
    DPO o;
    bool notEqual = false; // manually set after declaration, will force the SQL call to state != as opposed to =
    // quick constructors will reduce calls in main code.
    DataPass(QString s) : str{s} { o = DPO::STRING; }
    DataPass(CompanyID n) : num{n} { o = DPO::NUMBER; }
    DataPass(double d) : dbl{d} { o = DPO::DOUBLE; }
    DataPass(){} // default blank constructor.
};

// struct for page back history, will store all in history setup.
struct PageHistory {
    Pages page = Pages::Home;
    int id = 0;
    IDSendType idType = IDSendType::NA;
    int idCount = 1;
};

#endif // TRACKERSTRUCTS_H
