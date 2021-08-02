#ifndef HOSE_H
#define HOSE_H

#include "hosetemplates.h"
#include <vector>
#include <QString>
#include <QSqlError>
#include <filesystem>
#include <fstream>
#include <QFile>
#include <QDateTime>

#include "timeconvert.h"
#include "TrackerEnums.h" // Enumerations for address.
#include "sqldatabase.h" // company will extensively use SQL, but should not directly call SQL.
#include "TrackerStructs.h" // for address, and probably other functions
#include "fittingtemplate.h"
#include "htdirectorycontrol.h" // for hose image.
#include "company.h"

// will be for all hoses that are manually created. Since all data is based on a template, only the remaining and override information needs to be accessed here.

class Hose : public HoseTemplates
{
private:
    CompanyID m_HoseID;
    SQLDatabase *m_db;

    QString m_AssetNumber;
    QString m_ChipID;
    int m_Location; // load via ID, through the Company object.
    QDate m_MFGDate;
    QDate m_EXPDate;
    QString m_CreatedBy;
    QString m_Status;
   // QString m_AttachA; // part of fitting objects.
   // QString m_AttachB; // part of fitting objects.
    QString m_HoseNotes;

    // new additions to the class.
    QString m_Assembler;
    QString m_PONumber;
    bool m_DoRetest;

    QString m_CompanyIDNumber;
    //QString m_OwnerID; // owner PK of this hose, will load that user in tandem? or should we extend them as well?
    Company *m_Owner; // we can call information from here if required.

    // These are protected in the template class.
    //QString m_OverallLength;        //
    //QString m_CutLength;            //
    //int m_WorkingPressure;          //
    //int m_TestPressure;             //
    //FittingTemplate *m_End1;        // Declared as pointers since they otherwise MUST be instantiated.
    //FittingTemplate *m_End2;        //


public:
    Hose(CompanyID id); // create from existing
    Hose(QString assetNumber, QString chipID, int locationID, CompanyID ownerID, QDate mfgDate, QDate expDate, QString assembler, QString PONumber, bool DoRetest, QString createdBy,
         QString status, QString AttachA, QString AttachB, QString hoseNotes, QString overallLength, QString cutLength, int WP, int TP, CompanyID End1, CompanyID End2,
         CompanyID hoseTemplate, QString CompanyIDNumber); // create a new.
    ~Hose();

    // considerations: What can actually change after a hose is created?
    bool UpdateHose(QString assetID, QString chipID, int ownerID, int locationID, QString overallLength, QString cutLength,
                    int end1ID, int end2ID, QString end1Attach, QString end2Attach, QDate expDate, QString status, QString hoseNotes,
                    int WP, int TP, QString CompanyIDNumber);
    bool DecommissionHose(); // set status to decommissioned and expiry to today.
    bool RenewHose(); //reset expiry to +1 year, status is pass or active.
    bool AddNewChip(QString chipID); // replace only the chip ID.


    CompanyID getHoseID();
    QString getAssetNumber();
    QString getChipID();
    CompanyID GetLocationID();
    Address getLocation();
    CompanyID getOwnerID();
    QString getOwnerName();
    QDate getMFGDate();
    QDate getEXPDate();
    QString getCreator();
    QString getStatus();
    QString getEnd1Attachment();
    QString getEnd2Attachment();
    QString getHoseNotes(); // since getNotes already exists for the template

    QString getHoseAssembler();
    QString getPONumber();
    QString getCustomerIDNumber();
    bool getDoRetest();
    // all further functions come from template.
};

#endif // HOSE_H
