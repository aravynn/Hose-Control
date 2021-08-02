#include "hose.h"

Hose::Hose(CompanyID id) : HoseTemplates(){
    // use the ID to get all information from the DB. start first by initializing the hose template first
    // then implement everything from the hose iteself, since some information overrides others on template.
    m_HoseID = id; // not using m_PK to avoid conflict.
    m_db = new SQLDatabase();


    QString table = "Hoses";
    std::vector<QString> columns = {"AssetNumber", "ChipID", "OwnerPK", "locationPK", "TemplatePK", "MFGDate", "EXPDate", "CreatedBy", "PONumber", "CustomerIDNumber", "DoRetest", "CreatedBy", "HoseStatus",
                                    "OverallLength", "CutLength", "WorkingPres", "TestPres", "CouplingAPK", "CouplingBPK", "AttachA", "AttachB", "Notes"};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", id};

    StrPair results = m_db->selectAction(columns, table, filter, 1); // select the one result we need.

    // take the results, and start the setup.
    loadFromID(results.at(4).second.toInt()); // fourth ID is the hosetemplate

    // set the remaining information, overloading anything that needs to be edited.

    m_AssetNumber = results.at(0).second;
    m_ChipID = results.at(1).second;

    m_Owner = new Company(results.at(2).second.toInt());

    int locCount = m_Owner->GetLocationCount();
    int locationPK = results.at(3).second.toInt();
    for(int i{0}; i < locCount; ++i){
        if(m_Owner->GetLocationByNumber(i).id == locationPK){
            // this is the ID.
            m_Location = i;
            break;
        }
    }
    TimeConvert t;

    m_MFGDate = t.convert(results.at(5).second.toLongLong());
    m_EXPDate = t.convert(results.at(6).second.toLongLong());

    m_Assembler = results.at(7).second;
    m_PONumber = results.at(8).second;

    m_CompanyIDNumber = results.at(9).second;

    m_DoRetest = (bool)results.at(10).second.toInt();

    m_CreatedBy = results.at(11).second;
    m_Status = results.at(12).second;

    m_OverallLength = results.at(13).second; // inherited
    m_CutLength = results.at(14).second; // inherited

    m_WorkingPressure = results.at(15).second.toInt(); // inherited
    m_TestPressure = results.at(16).second.toInt(); // inherited

    delete m_End1; // delete previous assignments, rebuild.
    delete m_End2; // delete previous assignments, rebuild.
    m_End1 = new FittingTemplate(results.at(17).second.toInt()); // inherited, deleted by inherited class
    m_End2 = new FittingTemplate(results.at(18).second.toInt()); // inherited, deleted by inherited class

    m_End1->setAttachmentMethod(results.at(19).second);
    m_End2->setAttachmentMethod(results.at(20).second);
    m_HoseNotes = results.at(21).second;

} // create from existing

Hose::Hose(QString assetNumber, QString chipID, int locationID, CompanyID ownerID, QDate mfgDate, QDate expDate, QString assembler, QString PONumber,
           bool DoRetest, QString createdBy, QString status, QString AttachA, QString AttachB, QString hoseNotes, QString overallLength, QString cutLength,
           int WP, int TP, CompanyID End1, CompanyID End2, CompanyID hoseTemplate, QString CompanyIDNumber) :
    HoseTemplates(hoseTemplate), m_AssetNumber(assetNumber), m_ChipID(chipID), m_Location{locationID}, m_MFGDate(mfgDate), m_EXPDate(expDate), m_Assembler(assembler), m_PONumber(PONumber), m_DoRetest(DoRetest),
    m_CreatedBy(createdBy), m_Status(status), m_HoseNotes(hoseNotes) {
    // create a new hose in the DB using this function to insert data based on given information. Will largely start like above.
    m_Owner = new Company(ownerID); // instantiate the owner.
    m_Location = locationID;
    delete m_End1;
    delete m_End2;
    m_End1 = new FittingTemplate(End1);
    m_End2 = new FittingTemplate(End2);
    m_End1->setAttachmentMethod(AttachA);
    m_End2->setAttachmentMethod(AttachB);

    m_OverallLength = overallLength;
    m_CutLength = cutLength;
    m_WorkingPressure = WP;
    m_TestPressure = TP;

    m_CompanyIDNumber = CompanyIDNumber;

    // with all information inserted, proceed to insert information to DB.
    m_db = new SQLDatabase();

    QString table = "Hoses";

    m_HoseID = m_db->getNewPK(table); // not using m_PK to avoid conflict.

    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(22);

    inserts.at(0) = {"AssetNumber", m_AssetNumber};
    inserts.at(1) = {"ChipID", m_ChipID};
    inserts.at(2) = {"OwnerPK", ownerID};
    inserts.at(3) = {"locationPK", (CompanyID)m_Location};
    inserts.at(4) = {"TemplatePK", hoseTemplate};

    TimeConvert t;

    inserts.at(5) = {"MFGDate", QString::number(t.convert(m_MFGDate))};
    inserts.at(6) = {"EXPDate", QString::number(t.convert(m_EXPDate))};
    inserts.at(7) = {"CreatedBy", m_CreatedBy};
    inserts.at(8) = {"PONumber", m_PONumber};
    inserts.at(9) = {"CustomerIDNumber", m_CompanyIDNumber};
    inserts.at(10) = {"DoRetest", (CompanyID)m_DoRetest};
    inserts.at(11) = {"CreatedBy", m_CreatedBy};
    inserts.at(12) = {"HoseStatus", m_Status};
    inserts.at(13) = {"OverallLength", m_OverallLength};
    inserts.at(14) = {"CutLength", m_CutLength};
    inserts.at(15) = {"WorkingPres", (CompanyID)m_WorkingPressure};
    inserts.at(16) = {"TestPres", (CompanyID)m_TestPressure};
    inserts.at(17) = {"CouplingAPK", End1};
    inserts.at(18) = {"CouplingBPK", End2};
    inserts.at(19) = {"AttachA", m_End1->GetAttachMethod()};
    inserts.at(20) = {"AttachB", m_End2->GetAttachMethod()};
    inserts.at(21) = {"Notes", m_HoseNotes};

    m_db->insertAction(table, inserts);

} // create a new.

Hose::~Hose(){
    delete m_db;
    delete m_Owner;
}

// considerations: What can actually change after a hose is created?
bool Hose::UpdateHose(QString assetID, QString chipID, int ownerID, int locationID, QString overallLength, QString cutLength,
                      int end1ID, int end2ID, QString end1Attach, QString end2Attach, QDate expDate, QString status, QString hoseNotes,
                      int WP, int TP, QString CompanyIDNumber){

  //  qWarning() << "Ends:" << QString::number(end1ID) << QString::number(end2ID);

    m_ChipID = chipID;
    m_Location = locationID;
    m_EXPDate = expDate;
    m_Status = status;
    m_HoseNotes = hoseNotes;
    m_WorkingPressure = WP;
    m_TestPressure = TP;

    // new edits.
    delete m_End1;
    delete m_End2;
    m_End1 = new FittingTemplate(end1ID);
    m_End2 = new FittingTemplate(end2ID);
    m_End1->setAttachmentMethod(end1Attach);
    m_End2->setAttachmentMethod(end2Attach);
    m_AssetNumber = assetID;//
    delete m_Owner;
    m_Owner = new Company(ownerID);//
    m_OverallLength = overallLength;//
    m_CutLength = cutLength;//

    m_CompanyIDNumber = CompanyIDNumber;

    QString table = "Hoses";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(16);
    inserts.at(0) = {"AssetNumber", assetID};
    inserts.at(1) = {"ChipID", m_ChipID};
    inserts.at(2) = {"OwnerPK", (CompanyID)ownerID};
    inserts.at(3) = {"locationPK", (CompanyID)m_Location};

    TimeConvert t;

    inserts.at(4) = {"EXPDate", QString::number(t.convert(m_EXPDate))};
    inserts.at(5) = {"CustomerIDNumber", m_CompanyIDNumber};
    inserts.at(6) = {"HoseStatus", m_Status};
    inserts.at(7) = {"OverallLength", overallLength};
    inserts.at(8) = {"CutLength", cutLength};
    inserts.at(9) = {"Notes", m_HoseNotes};
    inserts.at(10) = {"WorkingPres", (CompanyID)m_WorkingPressure};
    inserts.at(11) = {"TestPres", (CompanyID)m_TestPressure};

   // qWarning() << "Ends:" << QString::number(end1ID) << QString::number(end2ID);

    inserts.at(12) = {"CouplingAPK", (CompanyID)end1ID};
    inserts.at(13) = {"CouplingBPK", (CompanyID)end2ID};
    inserts.at(14) = {"AttachA", end1Attach};
    inserts.at(15) = {"AttachB", end2Attach};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_HoseID};

    return m_db->updateAction(table, inserts, filter);

}
bool Hose::DecommissionHose(){

    TimeConvert t;

    m_EXPDate = t.Today();
    m_Status = "Failed";
    m_DoRetest = false; // always remove this.

    QString table = "Hoses";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(3);
    inserts.at(0) = {"EXPDate", QString::number(t.convert(m_EXPDate))};
    inserts.at(1) = {"DoRetest", (CompanyID)m_DoRetest};
    inserts.at(2) = {"HoseStatus", m_Status};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_HoseID};

    return m_db->updateAction(table, inserts, filter);
} // set status to decommissioned and expiry to today.
bool Hose::RenewHose(){

    TimeConvert t;
    QDate today = t.Today();
    today.addYears(1);

    m_EXPDate = today;
    m_Status = "Passed";

    QString table = "Hoses";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(2);
    inserts.at(0) = {"EXPDate", QString::number(t.convert(m_EXPDate))};
    inserts.at(1) = {"HoseStatus", m_Status};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_HoseID};

    return m_db->updateAction(table, inserts, filter);
} //reset expiry to +1 year, status is pass or active.
bool Hose::AddNewChip(QString chipID){
    m_ChipID = chipID;

    QString table = "Hoses";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(1);
    inserts.at(0) = {"ChipID", m_ChipID};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_HoseID};

    return m_db->updateAction(table, inserts, filter);
} // replace only the chip ID.

CompanyID Hose::getHoseID(){ return m_HoseID; }
QString Hose::getAssetNumber(){ return m_AssetNumber; }
QString Hose::getChipID(){ return m_ChipID; }
CompanyID Hose::GetLocationID(){ return m_Location; }
Address Hose::getLocation(){ return m_Owner->GetLocationByNumber(m_Location); }
CompanyID Hose::getOwnerID(){ return m_Owner->GetCompanyID(); }
QString Hose::getOwnerName(){ return m_Owner->getName(); }
QDate Hose::getMFGDate(){ return m_MFGDate; }
QDate Hose::getEXPDate(){ return m_EXPDate; }
QString Hose::getCreator(){ return m_CreatedBy; }
QString Hose::getStatus(){ return m_Status; }
QString Hose::getEnd1Attachment(){ return m_End1->GetAttachMethod(); }
QString Hose::getEnd2Attachment(){ return m_End1->GetAttachMethod(); }
QString Hose::getHoseNotes(){ return m_HoseNotes; } // since getNotes already exists for the template

QString Hose::getHoseAssembler(){ return m_Assembler; }
QString Hose::getPONumber(){ return m_PONumber; }
QString Hose::getCustomerIDNumber() { return m_CompanyIDNumber; }
bool Hose::getDoRetest(){ return m_DoRetest; }
