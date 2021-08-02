#ifndef FITTINGTEMPLATE_H
#define FITTINGTEMPLATE_H

#include <vector>
#include <QString>
#include <QSqlError>

#include "TrackerEnums.h" // Enumerations for address.
#include "sqldatabase.h" // company will extensively use SQL, but should not directly call SQL.
#include "TrackerStructs.h" // for address, and probably other functions

// fitting template is a simple class, used mostly as an imported object for templates and hoses.

class FittingTemplate
{
private:
    SQLDatabase *m_db;
    CompanyID m_PK; // fitting template ID #
    QString m_Name;
    QString m_PartNumber;
    QString m_Description;
    QString m_AttachMethod;

public:
    FittingTemplate(CompanyID id); // id number.
    FittingTemplate(QString name, QString partNumber, QString description, QString AttachMethod);
    ~FittingTemplate();
    bool updateTemplate(QString name ="", QString partNumber = "", QString description = "", QString AttachMethod = ""); // "" will denote no change. However, will always pull the data from the DB.
    QString GetName();
    QString GetPartNumber();
    CompanyID GetID();
    QString GetDescription();
    QString GetAttachMethod();

    void setAttachmentMethod(QString method);
};

#endif // FITTINGTEMPLATE_H

// required functions
// getName
// getPartNumber
// getID
// getDescription
// getAttachMethod

// updateTemplate -- single function, covers all data.
// FittingTemplate(id) // instantiate from DB
// FittingTemplate(all data) // create in DB.
