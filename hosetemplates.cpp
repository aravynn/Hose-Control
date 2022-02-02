#include "hosetemplates.h"

HoseTemplates::HoseTemplates(){
    // default constructor required for hose. Honestly probably this shouldn't exist however the format caused issues.
    m_db = new SQLDatabase();
    m_PK = -1; // will use this to prevent re-dipping starter call after the fact.
}

HoseTemplates::HoseTemplates(CompanyID id){
    // instantiate the template from the given template id.

    m_db = new SQLDatabase(); // instantiate the database object.

    m_PK = id;

    // using the given ID, get all of the pertinent data for the template.
    QString table = "HoseTemplates";

    std::vector<QString> columns = {"PartNumber", "Name", "Manufacturer", "Description", "DistributorRef",
                                    "InnerDiameter", "OverallLength", "CutLength", "WorkingPres", "TestPres",
                                    "TestTime", "CouplingAPK", "CouplingBPK", "Image", "Notes"};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_PK};

    //qWarning() << "FILTER: " << filter.at(0).first << " " << filter.at(0).second.num;

    StrPair returns = m_db->selectAction(columns, table, filter, 1);

    // take returns, and apply them all to the various values of the template.

    m_PartNumber = returns.at(0).second;
    m_Name = returns.at(1).second;
    m_Manufacturer = returns.at(2).second;
    m_Description = returns.at(3).second;
    m_DistributorRefNumber = returns.at(4).second;
    m_InnerDiameter = returns.at(5).second;
    m_OverallLength = returns.at(6).second;
    m_CutLength = returns.at(7).second;
    m_WorkingPressure = returns.at(8).second.toInt();
    m_TestPressure = returns.at(9).second.toInt();
    m_TestTime = returns.at(10).second.toInt();

    if(returns.at(11).second != "" && returns.at(12).second != ""){
        m_End1 = new FittingTemplate(returns.at(11).second.toInt());
        m_End2 = new FittingTemplate(returns.at(12).second.toInt());
        m_HasEnds = true;
    }
    m_Image = returns.at(13).second;
    m_Notes = returns.at(14).second;


}

HoseTemplates::HoseTemplates(QString partNumber, QString name, QString manufacturer, QString description, QString distRefNo,
    QString innerDiameter, QString length, QString cutLength, int WP, int TP, int TestTime, int End1, int End2, QString image, QString notes) :
    m_PartNumber{partNumber}, m_Name{name}, m_Manufacturer{manufacturer}, m_Description{description}, m_DistributorRefNumber{distRefNo},
    m_InnerDiameter{innerDiameter}, m_OverallLength{length}, m_CutLength{cutLength}, m_WorkingPressure{WP}, m_TestPressure{TP}, m_TestTime{TestTime}, m_Notes{notes} {

       // Create a new hose template from given data.

    m_db = new SQLDatabase;

    // set the current locations and information for the other parts not easily set.
    m_End1 = new FittingTemplate(End1); // use the id of the fitting template, entire thing.
    m_End2 = new FittingTemplate(End2);
    m_HasEnds = true;

    m_Image = moveImageToFolder(image); // we don't need to remove the old image.

    //qWarning() << "image location: " << m_Image;

    // insert the information into the DB.
    QString table = "HoseTemplates";
    std::vector<std::pair<QString, DataPass>> additions;
    additions.resize(15);

    additions.at(0) = {"PartNumber", m_PartNumber};
    additions.at(1) = {"Name", m_Name};
    additions.at(2) = {"Manufacturer", m_Manufacturer};
    additions.at(3) = {"Description", m_Description};
    additions.at(4) = {"DistributorRef", m_DistributorRefNumber};
    additions.at(5) = {"InnerDiameter", m_InnerDiameter};
    additions.at(6) = {"OverallLength", m_OverallLength};
    additions.at(7) = {"CutLength", m_CutLength};
    additions.at(8) = {"WorkingPres", (CompanyID)m_WorkingPressure};
    additions.at(9) = {"TestPres", (CompanyID)m_TestPressure};
    additions.at(10) = {"TestTime", (CompanyID)m_TestTime};
    additions.at(11) = {"CouplingAPK", m_End1->GetID()};
    additions.at(12) = {"CouplingBPK", m_End2->GetID()};
    additions.at(13) = {"Image", moveImageToFolder(m_Image)};
    additions.at(14) = {"Notes", m_Notes};

    // set the new PK
    m_PK = m_db->getNewPK(table);

    // insert into database.
    m_db->insertAction(table, additions);

}

bool HoseTemplates::loadFromID(CompanyID id){
    // load template from id given.
    if(m_PK != -1){
        return false;
    }

    m_PK = id;

    // using the given ID, get all of the pertinent data for the template.
    QString table = "HoseTemplates";

    std::vector<QString> columns = {"PartNumber", "Name", "Manufacturer", "Description", "DistributorRef",
                                    "InnerDiameter", "OverallLength", "CutLength", "WorkingPres", "TestPres",
                                    "TestTime", "CouplingAPK", "CouplingBPK", "Image", "Notes"};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_PK};

 //   qWarning() << "FILTER: " << filter.at(0).first << " " << filter.at(0).second.num;

    StrPair returns = m_db->selectAction(columns, table, filter, 1);

    // take returns, and apply them all to the various values of the template.

    m_PartNumber = returns.at(0).second;
    m_Name = returns.at(1).second;
    m_Manufacturer = returns.at(2).second;
    m_Description = returns.at(3).second;
    m_DistributorRefNumber = returns.at(4).second;
    m_InnerDiameter = returns.at(5).second;
    m_OverallLength = returns.at(6).second;
    m_CutLength = returns.at(7).second;
    m_WorkingPressure = returns.at(8).second.toInt();
    m_TestPressure = returns.at(9).second.toInt();
    m_TestTime = returns.at(10).second.toInt();

    if(returns.at(11).second != "" && returns.at(12).second != ""){
        m_End1 = new FittingTemplate(returns.at(11).second.toInt());
        m_End2 = new FittingTemplate(returns.at(12).second.toInt());
        m_HasEnds = true;
    }
    m_Image = returns.at(13).second;
    m_Notes = returns.at(14).second;

}

HoseTemplates::~HoseTemplates(){
    // delete dependencies for the template. Since a template is not required to have ends, we must check before delete or errors occur.

    delete m_db;
    if(m_HasEnds){
        delete m_End1;
        delete m_End2;
    }
}

bool HoseTemplates::UpdateTemplate(QString partNumber, QString name, QString manufacturer, QString description, QString distRefNo,
    QString innerDiameter, QString length, QString cutLength, int WP, int TP, int TestTime, int End1, int End2, QString notes){
    // take the given information, and update all fields. Will iignore the "" rule, since this function is only used on one template page.

    m_PartNumber = partNumber;
    m_Name = name;
    m_Manufacturer = manufacturer;
    m_Description = description;
    m_DistributorRefNumber = distRefNo;
    m_InnerDiameter = innerDiameter;
    m_OverallLength = length;
    m_CutLength = cutLength;
    m_WorkingPressure = WP;
    m_TestPressure = TP;
    m_TestTime = TestTime;

    if(m_HasEnds){
        delete m_End1;
        delete m_End2;
    }
    m_End1 = new FittingTemplate(End1); // ends need to already exist before we enter them.
    m_End2 = new FittingTemplate(End2);
    m_HasEnds = true;

    m_Notes = notes;

    // images are always updated separate of the main body.

    QString table = "HoseTemplates";
    std::vector<std::pair<QString, DataPass>> updates;
    updates.resize(14);
    updates.at(0) = {"PartNumber", m_PartNumber};
    updates.at(1) = {"Name", m_Name};
    updates.at(2) = {"Manufacturer", m_Manufacturer};
    updates.at(3) = {"Description", m_Description};
    updates.at(4) = {"DistributorRef", m_DistributorRefNumber};
    updates.at(5) = {"InnerDiameter", m_InnerDiameter};
    updates.at(6) = {"OverallLength", m_OverallLength};
    updates.at(7) = {"CutLength", m_CutLength};
    updates.at(8) = {"WorkingPres", (CompanyID)m_WorkingPressure};
    updates.at(9) = {"TestPres", (CompanyID)m_TestPressure};
    updates.at(10) = {"TestTime", (CompanyID)m_TestTime};
    updates.at(11) = {"CouplingAPK", m_End1->GetID()};
    updates.at(12) = {"CouplingBPK", m_End2->GetID()};
    updates.at(13) = {"Notes", m_Notes};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.push_back({"PK", m_PK});

    return m_db->updateAction(table, updates, filter); // update and return the result.
}

bool HoseTemplates::UpdateImage(QString Image){
    // delete the existing image
    // replace with the new image.
    // save the new image location.
    QString OldImage = m_Image;
    m_Image = moveImageToFolder(Image);

    //qWarning() << "moved image: " << m_Image;

    if(m_Image != ""){
       // qWarning() << "Remove Next";

        QString table = "HoseTemplates";
        std::vector<std::pair<QString, DataPass>> updates;
        updates.resize(1);
        updates.at(0) = {"Image", m_Image};

        std::vector<std::pair<QString, DataPass>> filter;
        filter.push_back({"PK", m_PK});

        if(m_db->updateAction(table, updates, filter)){
            return removeOldImage(OldImage);
        } else {
            return false; // failed update.
        }

    } else {
        m_Image = OldImage; // keep the old image, process failed.
        return false;
    }
    return false;
}

QString HoseTemplates::moveImageToFolder(QString image){
    // save the image to the folder, if not in the folder already.
    // take the given image location "image" and return the new location in the directory folders.
    HTDirectoryControl dir;

    QString extension;

    switch(dir.getFileType(image)){
    case ImageType::BMP:
       extension = ".bmp";
       break;
    case ImageType::GIF:
       extension = ".gif";
       break;
    case ImageType::JPG:
        extension = ".jpg";
        break;
    case ImageType::PNG:
        extension = ".png";
        break;
    case ImageType::ERR:
    default:
       return "";
        break;
    }

    QString company = "MEPBrothers";
    QString directoryRoot = dir.getImagesFolder(company, false); // for hose template images, we'll always use the MEPBrothers root folder.
    QString imageName = dir.cleanName(m_Name); // clean the hose template name to set for the file transfer.
    QString finalpath = directoryRoot + "/" +  imageName + extension;


    int i{1};
    while(dir.isFileExisting(finalpath)){
       // take the existing filepath and append numerals until we find a non-existing one.
       finalpath = directoryRoot + "/" +  imageName + "_" + QString::number(i) + extension;
       ++i;
    }


    if(dir.moveFile(image, finalpath)){
        return finalpath;
    } else {
        return ""; // transfer failed. return nothing so sytem knows.
    }
}

bool HoseTemplates::removeOldImage(QString image){

    // remove an existing image related to the template.

    HTDirectoryControl dir;
    //return true;
    if (image == "" ) return true; // the image is gone.

    return dir.deleteFile(image);
}

// template gets.
QString HoseTemplates::GetPartNumber(){ return m_PartNumber; }
QString HoseTemplates::GetName(){ return m_Name; }
QString HoseTemplates::GetManufacturer(){ return m_Manufacturer; }
QString HoseTemplates::GetDescription(){ return m_Description; }
QString HoseTemplates::GetDistRefNumber(){ return m_DistributorRefNumber; }
QString HoseTemplates::GetInnerDiameter(){ return m_InnerDiameter; }
QString HoseTemplates::GetOverallLength(){ return m_OverallLength; }
QString HoseTemplates::GetCutLength(){ return m_CutLength; }
int HoseTemplates::GetWP(){ return m_WorkingPressure; }
int HoseTemplates::getTestPressure(){ return m_TestPressure; }
int HoseTemplates::getTestTime(){ return m_TestTime; }
FittingTemplate * HoseTemplates::GetEnd1(){ return m_End1; }
FittingTemplate * HoseTemplates::GetEnd2(){ return m_End2; }
QString HoseTemplates::GetImageLocation(){ return m_Image; }
QString HoseTemplates::GetNotes(){ return m_Notes; }
CompanyID HoseTemplates::getID(){ return m_PK; }
