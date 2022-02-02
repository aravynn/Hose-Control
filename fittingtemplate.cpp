#include "fittingtemplate.h"

FittingTemplate::FittingTemplate(CompanyID id) : m_PK{id} {
    // get the information from the DB.
    // this loads all of the fitting information for an existing fitting

    m_db = new SQLDatabase;
    // get the information from the DB, and return to the variables in the class

    QString table = "FittingTemplates";
    std::vector<QString> columns = {"Name", "PartNumber", "Description", "AttachMethod"};
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_PK};

    StrPair returns = m_db->selectAction(columns, table, filter, 1);

    m_Name = returns.at(0).second;
    m_PartNumber = returns.at(1).second;
    m_Description = returns.at(2).second;
    m_AttachMethod = returns.at(3).second;

}

FittingTemplate::FittingTemplate(QString name, QString partNumber, QString description, QString AttachMethod) : m_Name{name}, m_PartNumber{partNumber}, m_Description{description}, m_AttachMethod(AttachMethod){
    // create and insert the new template into the DB.
     m_db = new SQLDatabase;

    // get the new ID for this new template.
     QString table = "FittingTemplates";

     m_PK = m_db->getNewPK(table);

     // insert the information into the DB.
    std::vector<std::pair<QString, DataPass>> inserts;

    inserts.resize(4);
    inserts.at(0) = {"Name", m_Name};
    inserts.at(1) = {"PartNumber", m_PartNumber};
    inserts.at(2) = {"Description", m_Description};
    inserts.at(3) = {"AttachMethod", m_AttachMethod};

    m_db->insertAction(table, inserts);
}

FittingTemplate::~FittingTemplate(){
    delete m_db; // close DB connection.
}

bool FittingTemplate::updateTemplate(QString name, QString partNumber, QString description, QString AttachMethod){
    // update the information on the DB main table.

    QString table = "FittingTemplates";
    std::vector<std::pair<QString, DataPass>> updates;

    if(name != ""){
        updates.push_back({"Name", m_Name});
        m_Name = name;
    }

    if(partNumber != ""){
        updates.push_back({"PartNumber", m_PartNumber});
        m_PartNumber = partNumber;
    }

    if(description != ""){
        updates.push_back({"Description", m_Description});
        m_Description = description;
    }

    if(AttachMethod != ""){
        updates.push_back({"AttachMethod", m_AttachMethod});
        m_AttachMethod = AttachMethod;
    }

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_PK};

    if(updates.size() < 1){
        return false; // there is nothing to actually update here...
    }

    return m_db->updateAction(table, updates, filter);

}

void FittingTemplate::setAttachmentMethod(QString method){
    // This is an override for the hose class, not to be used elsewhere.
    m_AttachMethod = method;
}

QString FittingTemplate::GetName() { return m_Name; }      //get the template name

QString FittingTemplate::GetPartNumber() { return m_PartNumber; }   // get the part number associated with this template

CompanyID FittingTemplate::GetID() { return m_PK; } // get the database key for this template

QString FittingTemplate::GetDescription() { return m_Description; } // gets the description for this template. Likely not used, as descriptions are not displayed.

QString FittingTemplate::GetAttachMethod() { return m_AttachMethod; }   // get the attachment method.
