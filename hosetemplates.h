#ifndef HOSETEMPLATES_H
#define HOSETEMPLATES_H

#include <vector>
#include <QString>
#include <QSqlError>
#include <filesystem>
#include <fstream>
#include <QFile>

#include "TrackerEnums.h" // Enumerations for address.
#include "sqldatabase.h" // company will extensively use SQL, but should not directly call SQL.
#include "TrackerStructs.h" // for address, and probably other functions
#include "fittingtemplate.h"
#include "htdirectorycontrol.h" // for hose image.

// create the hose template object and store all information related to it. This should be significantly simpler than the company class.

class HoseTemplates
{
private: // only accessable within class.
    SQLDatabase *m_db;

    CompanyID m_PK;
    QString m_PartNumber;           //
    QString m_Name;                 //
    QString m_Manufacturer;         //
    QString m_Description;          //
    QString m_DistributorRefNumber; //
    QString m_InnerDiameter;        //

    bool m_HasEnds = false;

    QString m_Image;                //
    QString m_Notes;                //

    QString moveImageToFolder(QString image); // save the image to the folder, if not in the folder already.
    bool removeOldImage(QString image); // remove the old image
protected: // only accessable within class or in inherited class.
    QString m_OverallLength;        //
    QString m_CutLength;            //
    int m_WorkingPressure;          //
    int m_TestPressure;             //
    int m_TestTime;
    FittingTemplate *m_End1;        // Declared as pointers since they otherwise MUST be instantiated.
    FittingTemplate *m_End2;        //

    bool loadFromID(CompanyID id);
public: // accessable from anywhere.
    HoseTemplates();
    HoseTemplates(CompanyID id);
    HoseTemplates(QString partNumber, QString name, QString manufacturer, QString description, QString distRefNo,
                                 QString innerDiameter, QString length, QString cutLength, int WP, int TP, int TestTime, int End1,
                                 int End2, QString image, QString notes);
    ~HoseTemplates();
    bool UpdateTemplate(QString partNumber, QString name, QString manufacturer, QString description, QString distRefNo,
        QString innerDiameter, QString length, QString cutLength, int WP, int TP, int TestTime, int End1, int End2, QString notes);
    bool UpdateImage(QString Image); // updates only the image so we can set the location and transfer the file.

    QString GetPartNumber();
    QString GetName();
    QString GetManufacturer();
    QString GetDescription();
    QString GetDistRefNumber();
    QString GetInnerDiameter();
    QString GetOverallLength();
    QString GetCutLength();
    int GetWP();
    int getTestPressure();
    int getTestTime();
    FittingTemplate *GetEnd1();
    FittingTemplate *GetEnd2();
    QString GetImageLocation();
    QString GetNotes();
    CompanyID getID();


};

#endif // HOSETEMPLATES_H


// functions required
// HoseTemplate(id) // get data from DB and instantiate.
// HoseTemplate(all data) // create a new hose template
// ~hosetemplate() delete the inevitable DB
// update template() // upate the template design.

// get part numer
// get name
// get mfg
// get description
// get distributor ref number
// get inner diameter
// get overall length
// get cut length
// get working pressure
// get test pressure
// get enda
// get endb
// get image location
// get image file
// get notes
