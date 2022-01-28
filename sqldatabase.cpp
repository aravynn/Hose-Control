#include "sqldatabase.h"

SQLDatabase::SQLDatabase()
{

}

void SQLDatabase::startDB(){
    const QString DRIVER ("QSQLITE");
    const QString DBNAME {"HOSETRACKER"};
    if(QSqlDatabase::isDriverAvailable(DRIVER)){
        // the application can run.

        // <needs confirmation> Creates the database, or loads it if already created.
        HTDirectoryControl directory = HTDirectoryControl();

        db = QSqlDatabase::addDatabase(DRIVER);
        db.setDatabaseName(directory.getDatabaseFolder() + "/data.sqlite");
         //qWarning() << "PATH: " << directory.getDatabaseFolder();
        if(!db.open()){
            qWarning() << "ERROR @ DB OPEN: " << db.lastError();   // normally check for an error, but this is a coding issue to resolve.
        } else {
            qWarning() << "DB Opened Successfully";

            if(!testConnection()){
                initializeDatabase();
              //  dummyData(); // only for testing purposes, should be able to do full load without this.
            } else {
                qWarning() << "Database already initialized";
            }

            addSyncTable(); // add the tables, if not already existing. This separation allows us to maintain original content.
        }
    }
}

SQLDatabase::~SQLDatabase(){
    // delete the database, and close the class.
    db.close();
}

bool SQLDatabase::testConnection(){
    // test the connection and return TRUE or FALSE
    QSqlQuery query;
    query.prepare("SELECT User FROM USERS WHERE User = 'DB' LIMIT 1");

    if(!query.exec())
        qWarning() << "ERROR testConnection 1: " << query.lastError().text(); // ensure no errors occur during request.

    while(query.next()){
        //results.at(i).Addr1 = query.value(1).toString();
        if(query.value(0).toString() == "DB"){
            return true;
        }
    }
    return false;
}

void SQLDatabase::addSyncTable(){
    QSqlQuery query;

    query.prepare("SELECT TableName FROM TableFormat WHERE PK = 0 LIMIT 1");

    if(query.exec()){
        while(query.next()){
            if(query.value(0).toString() == "Companies"){
                qWarning() << "Sync Table Exists";
                return; // exit the function if these tables already exist. Explicit check for first value is essential.
            }
        }
    } else {
        if(!query.isActive())
               qWarning() << "ERROR sync : " << db.lastError();
    }

    qWarning() << "create Sync Tables";

    query.exec("CREATE TABLE DataSync("
               "PK INTEGER PRIMARY KEY,"
               "TableName TEXT,"
               "TablePK INTEGER,"
               "Type INTEGER)"); // type stored in an enum

    if(!query.isActive())
           qWarning() << "ERROR sync 1: " << db.lastError();

    query.exec("CREATE TABLE TableFormat("
               "PK INTEGER PRIMARY KEY,"
               "TableName TEXT,"
               "ColumnID INTEGER,"
               "ColumnName TEXT,"
               "ColumnType INTEGER)");

    if(!query.isActive())
           qWarning() << "ERROR sync 2: " << db.lastError();

    // below will insert data to the table.

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (0,'Companies',1,'CompanyName'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (1,'Companies',2,'BillAddr1'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (2,'Companies',3,'BillAddr2'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (3,'Companies',4,'BillCity'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (4,'Companies',5,'BillProvince'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (5,'Companies',6,'BillPostal'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (6,'Companies',7,'BillCountry'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (7,'Companies',8,'Notes'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (8,'Companies',1,'Contacts'," + QString::number((int)ColumnType::CHILD) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (9,'Companies',1,'Locations'," + QString::number((int)ColumnType::CHILD) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (10,'Contacts',1,'CompanyPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (11,'Contacts',2,'Name'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (12,'Contacts',1,'ContactPoint'," + QString::number((int)ColumnType::CHILD) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (13,'ContactPoint',1,'ContactPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (14,'ContactPoint',2,'List'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (15,'ContactPoint',3,'Type'," + QString::number((int)ColumnType::STRING) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (16,'Locations',1,'CompanyPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (17,'Locations',2,'isPrimary'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (18,'Locations',3,'ShipAddr1'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (19,'Locations',4,'ShipAddr2'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (20,'Locations',5,'ShipCity'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (21,'Locations',6,'ShipProvince'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (22,'Locations',7,'ShipPostal'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (23,'Locations',8,'ShipCountry'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (24,'Locations',9,'ShipName'," + QString::number((int)ColumnType::STRING) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (25,'HoseTemplates',1,'PartNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (26,'HoseTemplates',2,'Name'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (27,'HoseTemplates',3,'Manufacturer'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (28,'HoseTemplates',4,'Description'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (29,'HoseTemplates',5,'DistributorRef'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (30,'HoseTemplates',6,'InnerDiameter'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (31,'HoseTemplates',7,'OverallLength'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (32,'HoseTemplates',8,'CutLength'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (33,'HoseTemplates',9,'WorkingPres'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (34,'HoseTemplates',10,'TestPres'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (35,'HoseTemplates',11,'TestTime'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (36,'HoseTemplates',12,'CouplingAPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (37,'HoseTemplates',13,'CouplingBPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (38,'HoseTemplates',14,'Image'," + QString::number((int)ColumnType::IMAGE) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (39,'HoseTemplates',15,'Notes'," + QString::number((int)ColumnType::STRING) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (40,'FittingTemplates',1,'Name'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (41,'FittingTemplates',2,'PartNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (42,'FittingTemplates',3,'Description'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (43,'FittingTemplates',4,'AttachMethod'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (44,'FittingTemplates',12,'HoseTemplates'," + QString::number((int)ColumnType::CHILD) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (45,'FittingTemplates',13,'HoseTemplates'," + QString::number((int)ColumnType::CHILD) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (44,'FittingTemplates',17,'Hoses'," + QString::number((int)ColumnType::CHILD) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (45,'FittingTemplates',18,'Hoses'," + QString::number((int)ColumnType::CHILD) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (46,'Hoses',1,'AssetNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (47,'Hoses',2,'ChipID'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (48,'Hoses',3,'OwnerPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (49,'Hoses',4,'locationPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (50,'Hoses',5,'TemplatePK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (51,'Hoses',6,'MFGDate'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (52,'Hoses',7,'EXPDate'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (53,'Hoses',8,'CreatedBy'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (54,'Hoses',9,'PONumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (55,'Hoses',10,'CustomerIDNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (56,'Hoses',11,'DoRetest'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (57,'Hoses',12,'HoseStatus'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (58,'Hoses',13,'OverallLength'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (59,'Hoses',14,'CutLength'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (60,'Hoses',15,'WorkingPres'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (61,'Hoses',16,'TestPres'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (62,'Hoses',17,'CouplingAPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (63,'Hoses',18,'CouplingBPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (64,'Hoses',19,'AttachA'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (65,'Hoses',20,'AttachB'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (66,'Hoses',21,'Notes'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (67,'Hoses',1,'HoseTests'," + QString::number((int)ColumnType::CHILD) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (68,'HoseTests',1,'HosePK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (69,'HoseTests',2,'OrderNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (70,'HoseTests',3,'Date'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (71,'HoseTests',4,'ProofTestType'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (72,'HoseTests',5,'TargetLoad'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (73,'HoseTests',6,'MaxLoad'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (74,'HoseTests',7,'TargetLoadHoldTime'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (75,'HoseTests',8,'Elongation'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (76,'HoseTests',9,'TestNumber'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (77,'HoseTests',10,'OwnerPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (78,'HoseTests',11,'CreatedBy'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (79,'HoseTests',12,'TestResult'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (80,'HoseTests',13,'Connectivity'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (81,'HoseTests',14,'Comments'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (82,'HoseTests',15,'VisualCover'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (83,'HoseTests',16,'VisualTube'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (84,'HoseTests',17,'VisualFitting'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (85,'HoseTests',18,'VisualCrimp'," + QString::number((int)ColumnType::STRING) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (86,'HoseTests',19,'TestType'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (87,'HoseTests',1,'TestData'," + QString::number((int)ColumnType::CHILD) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (88,'HoseTests',1,'TestImages'," + QString::number((int)ColumnType::CHILD) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (89,'TestData',1,'TestPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (90,'TestData',2,'Temperature'," + QString::number((int)ColumnType::REAL) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (91,'TestData',3,'Pressure'," + QString::number((int)ColumnType::REAL) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (92,'TestData',4,'IntervalNumber'," + QString::number((int)ColumnType::INTEGER) + ")");

    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (93,'TestImages',1,'TestPK'," + QString::number((int)ColumnType::INTEGER) + ")");
    query.exec("INSERT INTO TableFormat (PK, TableName, ColumnID, ColumnName, ColumnType) VALUES (94,'TestImages',2,'Image'," + QString::number((int)ColumnType::IMAGE) + ")");

    // create datasync tables for all new data currently in this app installation.
    // since each app installation is unique, this will upload everything, duplication may be an issue,

    // all tables to be checked.
    std::vector<QString> tables = {"Companies", "Contacts", "ContactPoint", "Locations", "HoseTemplates", "FittingTemplates", "Hoses", "HoseTests", "TestData", "TestImages"};

    int count = (int)tables.size();

   // qWarning() << "1";

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""};

    for(int i{0}; i < count; ++i){
        // for each table, select all lines.
        // after each line, copy to the datasync table.


        StrPair rets = selectAction({"PK"}, tables.at(i), filter);


       // qWarning() << "2";

        int retlen = (int)rets.size();

        qWarning() << "sync for table: " << tables.at(i) << "Count:" << QString::number(retlen);

        if(rets.size() < 1 || rets.at(0).first == ""){
            // it got nothing
            continue;
        }

        qWarning() << "length: " << retlen;
        //INSERT INTO DataSync (TableName, TablePK, Type) SELECT "Companies", PK, 0 FROM Companies
         query.exec("INSERT INTO DataSync (TableName, TablePK, Type) SELECT \"" + tables.at(i) + "\", PK, 0 FROM " + tables.at(i));

       /* for(int q{0}; q < retlen; ++q){
            // for each rets, put the PK into the datasync table.
            AddSyncLog(tables.at(i), rets.at(q).second.toInt(), Sync::INSERT);

        }*/

         //qWarning() << "3";

    }

    // qWarning() << "4";

}

void SQLDatabase::initializeDatabase(){
    // Create all of the tables for the database, and save them to the new database.
    QSqlQuery query;

    query.exec("CREATE TABLE Users("
               "PK INTEGER PRIMARY KEY,"
               "User TEXT,"
               "Pass TEXT,"
               "Salt TEXT,"
               "Date TEXT)");
    // create a null user to confirm DB exists.
    query.exec("INSERT INTO Users VALUES (0, 'DB', 'dbnull', 'dbnull', '0-0-0000')");

    // generate the company table.
    query.exec("CREATE TABLE Companies("
               "PK INTEGER PRIMARY KEY, "
               "CompanyName TEXT, "
               "BillAddr1 TEXT, "
               "BillAddr2 TEXT,"
               "BillCity TEXT,"
               "BillProvince TEXT, "
               "BillPostal TEXT, "
               "BillCountry TEXT, "
               "Notes TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 1: " << db.lastError();

    // generate the contacts within the company table. This is future-proofing in case we need
    // to expand the applications capabilities.
    query.exec("CREATE TABLE Contacts("
               "PK INTEGER PRIMARY KEY,"
               "CompanyPK INTEGER,"
               "Name TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 2: " << db.lastError();

    // Generate emails and phone numbers for contacts. Also Future Proofing.
    query.exec("CREATE TABLE ContactPoint("
               "PK INTEGER PRIMARY KEY,"
               "ContactPK INTEGER,"
               "List TEXT,"
               "Type TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 3: " << db.lastError();

    // Company locations. Each company can have multiple locations.
    query.exec("CREATE TABLE Locations("
               "PK INTEGER PRIMARY KEY,"
               "CompanyPK INTEGER,"
               "isPrimary INTEGER,"
               "ShipAddr1 TEXT,"
               "ShipAddr2 TEXT,"
               "ShipCity TEXT,"
               "ShipProvince TEXT,"
               "ShipPostal TEXT,"
               "ShipCountry TEXT,"
               "ShipName TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 4: " << db.lastError();

    // Hose Templates. Each is a generic filling of basic information. Information is copied from here to the
    // actual hose information, which is a duplication of the template information.
    query.exec("CREATE TABLE HoseTemplates("
               "PK INTEGER PRIMARY KEY,"
               "PartNumber TEXT,"
               "Name TEXT,"
               "Manufacturer TEXT,"
               "Description TEXT,"
               "DistributorRef TEXT,"
               "InnerDiameter TEXT,"
               "OverallLength TEXT,"
               "CutLength TEXT,"
               "WorkingPres INTEGER,"
               "TestPres INTEGER,"
               "TestTime INTEGER,"
               "CouplingAPK INTEGER,"
               "CouplingBPK INTEGER,"
               "Image TEXT,"
               "Notes TEXT)");


    if(!query.isActive())
           qWarning() << "ERROR 5: " << db.lastError();

    // Fitting Templates. Link to PK above, never overwritten. Universal List.
    query.exec("CREATE TABLE FittingTemplates("
               "PK INTEGER PRIMARY KEY,"
               "Name TEXT,"
               "PartNumber TEXT,"
               "Description TEXT,"
               "AttachMethod TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 6: " << db.lastError();

    // Actual Hoses, Will overlap some data from HoseTemplates.
    query.exec("CREATE TABLE Hoses("
               "PK INTEGER PRIMARY KEY,"
               "AssetNumber TEXT,"
               "ChipID TEXT,"
               "OwnerPK INTEGER,"
               "locationPK INTEGER,"
               "TemplatePK INTEGER,"
               "MFGDate INTEGER,"
               "EXPDate INTEGER,"
               "CreatedBy TEXT,"
               "PONumber TEXT,"
               "CustomerIDNumber TEXT,"
               "DoRetest INT,"
               "HoseStatus TEXT,"
               "OverallLength TEXT,"
               "CutLength TEXT,"
               "WorkingPres INTEGER,"
               "TestPres INTEGER,"
               "CouplingAPK INTEGER,"
               "CouplingBPK INTEGER,"
               "AttachA TEXT,"
               "AttachB TEXT,"
               "Notes TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 7: " << db.lastError();

    // Hose Tests, Data stored here is the unique data regarding the hose test, minus the lines themselves.
    query.exec("CREATE TABLE HoseTests("
               "PK INTEGER PRIMARY KEY,"
               "HosePK INTEGER,"
               "OrderNumber TEXT,"
               "Date INTEGER,"
               "ProofTestType TEXT,"
               "TargetLoad INTEGER,"
               "MaxLoad INTEGER,"
               "TargetLoadHoldTime INTEGER,"
               "Elongation TEXT,"
               "TestNumber TEXT,"
               "OwnerPK INTEGER,"
               "CreatedBy TEXT,"
               "TestResult TEXT,"
               "Connectivity TEXT,"
               "Comments TEXT,"
               "VisualCover TEXT,"
               "VisualTube TEXT, "
               "VisualFitting TEXT,"
               "VisualCrimp TEXT,"
               "TestType INTEGER)");

    if(!query.isActive())
           qWarning() << "ERROR 8: " << db.lastError();

    // Testing Data, stores sequential test data, one interval at a time.
    query.exec("CREATE TABLE TestData("
               "PK INTEGER PRIMARY KEY,"
               "TestPK INTEGER,"
               "Temperature REAL,"
               "Pressure REAL,"
               "IntervalNumber INTEGER)");

    if(!query.isActive())
           qWarning() << "ERROR 9: " << db.lastError();

    // table for stored images.
    query.exec("CREATE TABLE TestImages("
               "PK INTEGER PRIMARY KEY,"
               "TestPK INTEGER,"
               "Image TEXT)");

    if(!query.isActive())
           qWarning() << "ERROR 9B: " << db.lastError();

    // We'll need to instigate error checking and make sure the database actually loaded correct.
}

void SQLDatabase::dummyData(){
    // create dummy data for testing, 3 lines per area.
   QSqlQuery query;

    // Companies. We'll have 3 companies entered.
    query.exec("INSERT INTO Companies (PK, CompanyName, BillAddr1, BillAddr2, BillCity, BillProvince, BillPostal, BillCountry, Notes) VALUES (0, "
               "'MEP Brothers',"
               "'725 Century St.',"
               "'',"
               "'Winnipeg',"
               "'MB',"
               "'R3H0M2',"
               "'CA',"
               "'This is our company, This is a sample note about said company.')");

    if(!query.isActive())
           qWarning() << "ERROR 13: " << db.lastError();

    query.exec("INSERT INTO Companies (PK, CompanyName, BillAddr1, BillAddr2, BillCity, BillProvince, BillPostal, BillCountry, Notes) VALUES (1, "
               "'Foo Co.',"
               "'Suite 782',"
               "'123 Hekla Rd.',"
               "'Carberry',"
               "'Mb',"
               "'R3f1r3',"
               "'ca',"
               "'Just another note.')");

    if(!query.isActive())
           qWarning() << "ERROR 14: " << db.lastError();

    query.exec("INSERT INTO Companies (PK, CompanyName, BillAddr1, BillAddr2, BillCity, BillProvince, BillPostal, BillCountry, Notes) VALUES (2, "
               "'Brothers IND',"
               "'68 Brookside Bay',"
               "'-Old Address-',"
               "'Wpg',"
               "'MB',"
               "'r3R3r3',"
               "'CA',"
               "'')");


    if(!query.isActive())
           qWarning() << "ERROR 15: " << db.lastError();

    // Contacts, 1 per company.
    query.exec("INSERT INTO Contacts (PK, CompanyPK, Name) VALUES (0,0,'Jeff Gural')");

    if(!query.isActive())
           qWarning() << "ERROR 16: " << db.lastError();

    query.exec("INSERT INTO Contacts (PK, CompanyPK, Name) VALUES (1,1,'Kevin Jones')");

    if(!query.isActive())
           qWarning() << "ERROR 17: " << db.lastError();

    query.exec("INSERT INTO Contacts (PK, CompanyPK, Name) VALUES (2,2,'Paul Bauer')");

    if(!query.isActive())
           qWarning() << "ERROR 18: " << db.lastError();

    //Contact Details, 1 Phone and email per person.
    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (0,0,'(204) 960-1187','phone')");

    if(!query.isActive())
           qWarning() << "ERROR 19: " << db.lastError();

    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (1,0,'jgural@mepbrothers.com','email')");

    if(!query.isActive())
           qWarning() << "ERROR 20: " << db.lastError();

    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (2,1,'(204) 899-7706','phone')");

    if(!query.isActive())
           qWarning() << "ERROR 21: " << db.lastError();

    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (3,1,'aravynn@gmail.com','email')");

    if(!query.isActive())
           qWarning() << "ERROR 22: " << db.lastError();

    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (4,2,'paul@mailinator.com','email')");

    if(!query.isActive())
           qWarning() << "ERROR 23: " << db.lastError();

    query.exec("INSERT INTO ContactPoint (PK, ContactPK, List, Type) VALUES (5,2,'204-632-4118','phone')");

    if(!query.isActive())
           qWarning() << "ERROR 24: " << db.lastError();


    // Locations, 1 per company.

    query.exec("INSERT INTO Locations (PK, CompanyPK, isPrimary, ShipAddr1, ShipAddr2, ShipCity, ShipProvince, ShipPostal, ShipCountry, ShipName) VALUES (0,0,1,"
               "'725 Century St.',"
               "'',"
               "'Winnipeg',"
               "'MB',"
               "'R3H0M2',"
               "'CA',"
               "'Home')");

    if(!query.isActive())
           qWarning() << "ERROR 25: " << db.lastError();

    query.exec("INSERT INTO Locations (PK, CompanyPK, isPrimary, ShipAddr1, ShipAddr2, ShipCity, ShipProvince, ShipPostal, ShipCountry, ShipName) VALUES (1,1,1,"
               "'1189 Doobley St.',"
               "'Unit 100',"
               "'Winnipeg',"
               "'MB',"
               "'R3R3R3',"
               "'CA',"
               "'Warehouse')");

    if(!query.isActive())
           qWarning() << "ERROR 26: " << db.lastError();

    query.exec("INSERT INTO Locations (PK, CompanyPK, isPrimary, ShipAddr1, ShipAddr2, ShipCity, ShipProvince, ShipPostal, ShipCountry, ShipName) VALUES (2,2,1,"
               "'Unit 778',"
               "'122 Booth Drive',"
               "'Winnipeg',"
               "'Manitoba',"
               "'R3y4F4',"
               "'CA',"
               "'Booth Drive')");

    if(!query.isActive())
           qWarning() << "ERROR 27: " << db.lastError();


    // Hose Templates. Let's make 2.


    /*
     * query.exec("CREATE TABLE HoseTemplates("
               "PK INTEGER PRIMARY KEY,"
               "PartNumber TEXT,"
               "Name TEXT,"
               "Manufacturer TEXT,"
               "Description TEXT,"
               "DistributorRef TEXT,"
               "InnerDiameter TEXT,"
               "OverallLength TEXT,"
               "CutLength TEXT,"
               "WorkingPres INTEGER,"
               "TestPres INTEGER,"
               "CouplingAPK INTEGER,"
               "CouplingBPK INTEGER,"
               "Notes TEXT)");
               */

    query.exec("INSERT INTO HoseTemplates VALUES (0,"
               "'03-INF-300',"
               "'Infinity Hose 3\"',"
               "'Continental',"
               "'Infinity Hose 3\" ID 150 PSI Drop Hose',"
               "'200212312',"
               "'3 Inch',"
               "'20''',"
               "'19.5''',"
               "150,225,300,0,1,'','')");

    if(!query.isActive())
           qWarning() << "ERROR 28: " << db.lastError();

    query.exec("INSERT INTO HoseTemplates VALUES (1,"
               "'03-PRES-G-038-CTL',"
               "'Pressure Washer Hose',"
               "'Continental',"
               "'Gen Spec Continental Pressure Wash Hose',"
               "'20041001',"
               "'3/8\"',"
               "'74''',"
               "'74.5''',"
               "3000,4500,140,2,2,'','General Purpose 75'' Assembly')");

    if(!query.isActive())
           qWarning() << "ERROR 29: " << db.lastError();


    //Fitting Templates. We need 3, C, E, and NPT 3/8"
    query.exec("INSERT INTO FittingTemplates VALUES (0,'3\" C Camlock','31-QC-C-AL-300','3 INCH ALUMINUM C Camlock','CRIMPED')");

    if(!query.isActive())
           qWarning() << "ERROR 30: " << db.lastError();

    query.exec("INSERT INTO FittingTemplates VALUES (1,'3\" E Camlock','31-QC-E-AL-300','3 INCH ALUMINUM E Camlock','CRIMPED')");

    if(!query.isActive())
           qWarning() << "ERROR 31: " << db.lastError();

    query.exec("INSERT INTO FittingTemplates VALUES (2,'3/8\" NPT Male','04-NPT-038-SS','3/8\" Stainless NPT Male','CRIMPS')");

    if(!query.isActive())
           qWarning() << "ERROR 32: " << db.lastError();


    // Actual Hoses. Will Make 3, one for each company.
    query.exec("INSERT INTO Hoses VALUES(0,"
               "'003-00001',"
               "'000000000083753875638382',"
               "0,"
               "0,"
               "0,"
               "'737927',"
               "'738292',"
               "'KJ',"
               "'0234-23',"
               "0,"
               "'PASS',"
               "'20''',"
               "'19.5''',"
               "300,"
               "450,"
               "0,"
               "1,"
               "'CRIMP',"
               "'CRIMP',"
               "'')");

    if(!query.isActive())
           qWarning() << "ERROR 33: " << db.lastError();

     query.exec("INSERT INTO Hoses VALUES(1,"
               "'003-00002',"
               "'0000000000837538753453482',"
               "1,"
               "1,"
               "0,"
                "'737927',"
                "'738292',"
               "'KJ',"
                "'0234-23',"
                "1,"
               "'PASS',"
               "'20''',"
               "'19.5''',"
               "300,"
               "450,"
               "0,"
               "1,"
               "'CRIMP',"
               "'CRIMP',"
               "'')");

     if(!query.isActive())
            qWarning() << "ERROR 34: " << db.lastError();

    query.exec("INSERT INTO Hoses VALUES(2,"
               "'034-00342',"
               "'000000000087346378832633',"
               "2,"
               "2,"
               "1,"
               "'737927',"
               "'738292',"
               "'KJ',"
               "'0234-23',"
               "0,"
               "'RENEW',"
               "'75''',"
               "'74.5''',"
               "3000,"
               "4500,"
               "2,"
               "2,"
               "'CRIMP',"
               "'CRIMP',"
               "'HOSE SHOULD REFERENCE EXPIRY.')");

    if(!query.isActive())
           qWarning() << "ERROR 35: " << db.lastError();


    // Hose Tests, one for each hose.
    query.exec("INSERT INTO HoseTests VALUES (0,0,'PO 23422','737543','1.5x WP',450,695,300,'3%','00001',0,'KJ','PASS','YES','','Good','Good','Average','Good', 0)");

    if(!query.isActive())
           qWarning() << "ERROR 36: " << db.lastError();

    query.exec("INSERT INTO HoseTests VALUES (1,1,'PO 32432','737611','1.5x WP',450,695,300,'3%','00002',1,'KJ','PASS','NO','','Good','Good','Average','Good', 0)");

    if(!query.isActive())
           qWarning() << "ERROR 37: " << db.lastError();

    query.exec("INSERT INTO HoseTests VALUES (2,2,'2432312','737392','1.5x WP',450,495,120,'5%','000003',2,'KJ','PASS','YES','','Good','Good','Average','Good', 0)");

    if(!query.isActive())
           qWarning() << "ERROR 38: " << db.lastError();


    // Hose Test Data, only 10 seconds per test.
    query.exec("INSERT INTO TestData VALUES (0,0,20.5,452,0)");

    if(!query.isActive())
           qWarning() << "ERROR 39: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (1,0,20.4,452,1)");

    if(!query.isActive())
           qWarning() << "ERROR 40: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (2,0,20.6,453,2)");

    if(!query.isActive())
           qWarning() << "ERROR 41: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (3,0,20.5,454,3)");

    if(!query.isActive())
           qWarning() << "ERROR 42: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (4,0,20.2,455,4)");

    if(!query.isActive())
           qWarning() << "ERROR 43: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (5,0,20.7,454,5)");

    if(!query.isActive())
           qWarning() << "ERROR 44: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (6,0,20.7,455,6)");

    if(!query.isActive())
           qWarning() << "ERROR 45: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (7,0,20.8,457,7)");

    if(!query.isActive())
           qWarning() << "ERROR 46: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (8,0,20.5,453,8)");

    if(!query.isActive())
           qWarning() << "ERROR 47: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (9,0,20.9,455,9)");

    if(!query.isActive())
           qWarning() << "ERROR 48: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (10,1,21.1,454,0)");

    if(!query.isActive())
           qWarning() << "ERROR 49: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (11,1,21.9,453,1)");

    if(!query.isActive())
           qWarning() << "ERROR 50: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (12,1,20.5,452,2)");

    if(!query.isActive())
           qWarning() << "ERROR 51: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (13,1,21.3,451,3)");

    if(!query.isActive())
           qWarning() << "ERROR 52: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (14,1,21.2,451,4)");

    if(!query.isActive())
           qWarning() << "ERROR 53: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (15,1,21.0,451,5)");

    if(!query.isActive())
           qWarning() << "ERROR 54: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (16,1,20.9,452,6)");

    if(!query.isActive())
           qWarning() << "ERROR 55: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (17,1,20.7,452,7)");

    if(!query.isActive())
           qWarning() << "ERROR 56: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (18,1,20.5,451,8)");

    if(!query.isActive())
           qWarning() << "ERROR 57: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (19,1,20.3,451,9)");

    if(!query.isActive())
           qWarning() << "ERROR 58: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (20,2,22.2,4503,0)");

    if(!query.isActive())
           qWarning() << "ERROR 59: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (21,2,23.3,4510,1)");

    if(!query.isActive())
           qWarning() << "ERROR 60: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (22,2,23.6,4555,2)");

    if(!query.isActive())
           qWarning() << "ERROR 61: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (23,2,23.8,4545,3)");

    if(!query.isActive())
           qWarning() << "ERROR 62: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (24,2,24.1,4530,4)");

    if(!query.isActive())
           qWarning() << "ERROR 63: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (25,2,25.5,4525,5)");

    if(!query.isActive())
           qWarning() << "ERROR 64: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (26,2,26.6,4530,6)");

    if(!query.isActive())
           qWarning() << "ERROR 65: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (27,2,26.5,4567,7)");

    if(!query.isActive())
           qWarning() << "ERROR 66: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (28,2,26.4,4534,8)");

    if(!query.isActive())
           qWarning() << "ERROR 67: " << db.lastError();

    query.exec("INSERT INTO TestData VALUES (29,2,26.8,4503,9)");

    if(!query.isActive())
           qWarning() << "ERROR 68: " << db.lastError();


/*
    query.exec("INSERT INTO TestData VALUES (0, 0, 20, 45, 0)");
    query.exec("INSERT INTO TestData VALUES (1, 0, 15.71, 45089, 1)");
    query.exec("INSERT INTO TestData VALUES (2, 0, 23.44, 47647, 2)");
    query.exec("INSERT INTO TestData VALUES (3, 0, 24.75, 49205, 3)");
    query.exec("INSERT INTO TestData VALUES (4, 0, 18.27, 48051, 4)");
    query.exec("INSERT INTO TestData VALUES (5, 0, 17.98, 4786, 5)");
    query.exec("INSERT INTO TestData VALUES (6, 0, 18.16, 46122, 6)");
    query.exec("INSERT INTO TestData VALUES (7, 0, 24.95, 46755, 7)");
    query.exec("INSERT INTO TestData VALUES (8, 0, 23.82, 47493, 8)");
    query.exec("INSERT INTO TestData VALUES (9, 0, 18.53, 48612, 9)");



    query.exec("INSERT INTO TestData VALUES (0, 0, 20, 450, 0)");
    query.exec("INSERT INTO TestData VALUES (1, 0, 15.71, 450.89, 1)");
    query.exec("INSERT INTO TestData VALUES (2, 0, 23.44, 476.47, 2)");
    query.exec("INSERT INTO TestData VALUES (3, 0, 24.75, 492.05, 3)");
    query.exec("INSERT INTO TestData VALUES (4, 0, 18.27, 480.51, 4)");
    query.exec("INSERT INTO TestData VALUES (5, 0, 17.98, 478.6, 5)");
    query.exec("INSERT INTO TestData VALUES (6, 0, 18.16, 461.22, 6)");
    query.exec("INSERT INTO TestData VALUES (7, 0, 24.95, 467.55, 7)");
    query.exec("INSERT INTO TestData VALUES (8, 0, 23.82, 474.93, 8)");
    query.exec("INSERT INTO TestData VALUES (9, 0, 18.53, 486.12, 9)");
    query.exec("INSERT INTO TestData VALUES (10, 0, 23.15, 461.91, 10)");
    query.exec("INSERT INTO TestData VALUES (11, 0, 20, 481.76, 11)");
    query.exec("INSERT INTO TestData VALUES (12, 0, 24.01, 461.7, 12)");
    query.exec("INSERT INTO TestData VALUES (13, 0, 16.29, 478.68, 13)");
    query.exec("INSERT INTO TestData VALUES (14, 0, 20.89, 482.44, 14)");
    query.exec("INSERT INTO TestData VALUES (15, 0, 22.31, 465.6, 15)");
    query.exec("INSERT INTO TestData VALUES (16, 0, 15.9, 462.75, 16)");
    query.exec("INSERT INTO TestData VALUES (17, 0, 15.37, 466.81, 17)");
    query.exec("INSERT INTO TestData VALUES (18, 0, 19.87, 475.74, 18)");
    query.exec("INSERT INTO TestData VALUES (19, 0, 19.49, 485.73, 19)");
    query.exec("INSERT INTO TestData VALUES (20, 0, 21.03, 481.72, 20)");
    query.exec("INSERT INTO TestData VALUES (21, 0, 24.55, 473.01, 21)");
    query.exec("INSERT INTO TestData VALUES (22, 0, 20.81, 494.54, 22)");
    query.exec("INSERT INTO TestData VALUES (23, 0, 20.86, 459.42, 23)");
    query.exec("INSERT INTO TestData VALUES (24, 0, 21.61, 484.2, 24)");
    query.exec("INSERT INTO TestData VALUES (25, 0, 19.15, 459, 25)");
    query.exec("INSERT INTO TestData VALUES (26, 0, 16.56, 462.41, 26)");
    query.exec("INSERT INTO TestData VALUES (27, 0, 17.15, 453.37, 27)");
    query.exec("INSERT INTO TestData VALUES (28, 0, 23.42, 474.96, 28)");
    query.exec("INSERT INTO TestData VALUES (29, 0, 23.57, 467.94, 29)");
    query.exec("INSERT INTO TestData VALUES (30, 0, 16.61, 488.23, 30)");
    query.exec("INSERT INTO TestData VALUES (31, 0, 17.44, 481.91, 31)");
    query.exec("INSERT INTO TestData VALUES (32, 0, 21.77, 462.92, 32)");
    query.exec("INSERT INTO TestData VALUES (33, 0, 19.67, 479.9, 33)");
    query.exec("INSERT INTO TestData VALUES (34, 0, 15.79, 465.26, 34)");
    query.exec("INSERT INTO TestData VALUES (35, 0, 17.22, 466.44, 35)");
    query.exec("INSERT INTO TestData VALUES (36, 0, 18.03, 461.05, 36)");
    query.exec("INSERT INTO TestData VALUES (37, 0, 17.63, 478.98, 37)");
    query.exec("INSERT INTO TestData VALUES (38, 0, 23.71, 468.45, 38)");
    query.exec("INSERT INTO TestData VALUES (39, 0, 22.59, 466.67, 39)");
    query.exec("INSERT INTO TestData VALUES (40, 0, 18.89, 453.2, 40)");
    query.exec("INSERT INTO TestData VALUES (41, 0, 23.68, 468.93, 41)");
    query.exec("INSERT INTO TestData VALUES (42, 0, 20.94, 485.96, 42)");
    query.exec("INSERT INTO TestData VALUES (43, 0, 17.64, 472.36, 43)");
    query.exec("INSERT INTO TestData VALUES (44, 0, 17.35, 475.99, 44)");
    query.exec("INSERT INTO TestData VALUES (45, 0, 15.42, 485.58, 45)");
    query.exec("INSERT INTO TestData VALUES (46, 0, 24.13, 475.53, 46)");
    query.exec("INSERT INTO TestData VALUES (47, 0, 23.4, 467.77, 47)");
    query.exec("INSERT INTO TestData VALUES (48, 0, 16.58, 472.55, 48)");
    query.exec("INSERT INTO TestData VALUES (49, 0, 24.81, 484.95, 49)");
    query.exec("INSERT INTO TestData VALUES (50, 0, 22.04, 481.63, 50)");
    query.exec("INSERT INTO TestData VALUES (51, 0, 16.01, 489.11, 51)");
    query.exec("INSERT INTO TestData VALUES (52, 0, 18.36, 485.46, 52)");
    query.exec("INSERT INTO TestData VALUES (53, 0, 22.31, 458.79, 53)");
    query.exec("INSERT INTO TestData VALUES (54, 0, 24.04, 490.5, 54)");
    query.exec("INSERT INTO TestData VALUES (55, 0, 24.68, 467.88, 55)");
    query.exec("INSERT INTO TestData VALUES (56, 0, 21.55, 489.64, 56)");
    query.exec("INSERT INTO TestData VALUES (57, 0, 16.53, 466.58, 57)");
    query.exec("INSERT INTO TestData VALUES (58, 0, 21.33, 468.34, 58)");
    query.exec("INSERT INTO TestData VALUES (59, 0, 22.05, 489.98, 59)");
    query.exec("INSERT INTO TestData VALUES (60, 0, 21.58, 450.97, 60)");
    query.exec("INSERT INTO TestData VALUES (61, 0, 19.06, 485.23, 61)");
    query.exec("INSERT INTO TestData VALUES (62, 0, 21.67, 482.29, 62)");
    query.exec("INSERT INTO TestData VALUES (63, 0, 15.85, 456.8, 63)");
    query.exec("INSERT INTO TestData VALUES (64, 0, 18.92, 479.63, 64)");
    query.exec("INSERT INTO TestData VALUES (65, 0, 18.68, 451.18, 65)");
    query.exec("INSERT INTO TestData VALUES (66, 0, 16.84, 474.07, 66)");
    query.exec("INSERT INTO TestData VALUES (67, 0, 24.21, 453.08, 67)");
    query.exec("INSERT INTO TestData VALUES (68, 0, 22.37, 467.83, 68)");
    query.exec("INSERT INTO TestData VALUES (69, 0, 16.33, 489.53, 69)");
    query.exec("INSERT INTO TestData VALUES (70, 0, 22.45, 463.05, 70)");
    query.exec("INSERT INTO TestData VALUES (71, 0, 21.61, 484.82, 71)");
    query.exec("INSERT INTO TestData VALUES (72, 0, 19.46, 494.03, 72)");
    query.exec("INSERT INTO TestData VALUES (73, 0, 19.24, 490.29, 73)");
    query.exec("INSERT INTO TestData VALUES (74, 0, 21.52, 457.8, 74)");
    query.exec("INSERT INTO TestData VALUES (75, 0, 21.84, 484.47, 75)");
    query.exec("INSERT INTO TestData VALUES (76, 0, 21.14, 479.45, 76)");
    query.exec("INSERT INTO TestData VALUES (77, 0, 24.67, 468.84, 77)");
    query.exec("INSERT INTO TestData VALUES (78, 0, 23.79, 474.93, 78)");
    query.exec("INSERT INTO TestData VALUES (79, 0, 23.59, 487.06, 79)");
    query.exec("INSERT INTO TestData VALUES (80, 0, 21.31, 450.52, 80)");
    query.exec("INSERT INTO TestData VALUES (81, 0, 18.87, 457.48, 81)");
    query.exec("INSERT INTO TestData VALUES (82, 0, 23.56, 491.79, 82)");
    query.exec("INSERT INTO TestData VALUES (83, 0, 21.89, 479.25, 83)");
    query.exec("INSERT INTO TestData VALUES (84, 0, 18.65, 454.91, 84)");
    query.exec("INSERT INTO TestData VALUES (85, 0, 17.46, 490.16, 85)");
    query.exec("INSERT INTO TestData VALUES (86, 0, 15.35, 469.27, 86)");
    query.exec("INSERT INTO TestData VALUES (87, 0, 20.99, 453.89, 87)");
    query.exec("INSERT INTO TestData VALUES (88, 0, 17.48, 490.34, 88)");
    query.exec("INSERT INTO TestData VALUES (89, 0, 15.8, 455.46, 89)");
    query.exec("INSERT INTO TestData VALUES (90, 0, 22.19, 470.6, 90)");
    query.exec("INSERT INTO TestData VALUES (91, 0, 24.87, 468.78, 91)");
    query.exec("INSERT INTO TestData VALUES (92, 0, 17.01, 459.42, 92)");
    query.exec("INSERT INTO TestData VALUES (93, 0, 17.04, 454.65, 93)");
    query.exec("INSERT INTO TestData VALUES (94, 0, 20.12, 475.18, 94)");
    query.exec("INSERT INTO TestData VALUES (95, 0, 15.63, 461.33, 95)");
    query.exec("INSERT INTO TestData VALUES (96, 0, 20.03, 493.62, 96)");
    query.exec("INSERT INTO TestData VALUES (97, 0, 18.68, 476.51, 97)");
    query.exec("INSERT INTO TestData VALUES (98, 0, 19.08, 485.76, 98)");
    query.exec("INSERT INTO TestData VALUES (99, 0, 20.39, 454.31, 99)");
    query.exec("INSERT INTO TestData VALUES (100, 0, 15.56, 473.8, 100)");
    query.exec("INSERT INTO TestData VALUES (101, 0, 22.21, 464.78, 101)");
    query.exec("INSERT INTO TestData VALUES (102, 0, 21.15, 485.39, 102)");
    query.exec("INSERT INTO TestData VALUES (103, 0, 24.6, 491.13, 103)");
    query.exec("INSERT INTO TestData VALUES (104, 0, 18.67, 453.89, 104)");
    query.exec("INSERT INTO TestData VALUES (105, 0, 21.96, 473.42, 105)");
    query.exec("INSERT INTO TestData VALUES (106, 0, 20.76, 489.88, 106)");
    query.exec("INSERT INTO TestData VALUES (107, 0, 15.87, 460.27, 107)");
    query.exec("INSERT INTO TestData VALUES (108, 0, 18.92, 494.78, 108)");
    query.exec("INSERT INTO TestData VALUES (109, 0, 22.75, 467.5, 109)");
    query.exec("INSERT INTO TestData VALUES (110, 0, 23.99, 480.43, 110)");
    query.exec("INSERT INTO TestData VALUES (111, 0, 17.49, 492.95, 111)");
    query.exec("INSERT INTO TestData VALUES (112, 0, 19.97, 489.69, 112)");
    query.exec("INSERT INTO TestData VALUES (113, 0, 21.87, 480.83, 113)");
    query.exec("INSERT INTO TestData VALUES (114, 0, 18.66, 472.06, 114)");
    query.exec("INSERT INTO TestData VALUES (115, 0, 16.09, 450.91, 115)");
    query.exec("INSERT INTO TestData VALUES (116, 0, 19.42, 452.65, 116)");
    query.exec("INSERT INTO TestData VALUES (117, 0, 21.36, 485.26, 117)");
    query.exec("INSERT INTO TestData VALUES (118, 0, 17.35, 468.96, 118)");
    query.exec("INSERT INTO TestData VALUES (119, 0, 15.43, 489.52, 119)");
    query.exec("INSERT INTO TestData VALUES (120, 0, 21.04, 470.8, 120)");
    query.exec("INSERT INTO TestData VALUES (121, 0, 22.27, 485.91, 121)");
    query.exec("INSERT INTO TestData VALUES (122, 0, 15.45, 459.76, 122)");
    query.exec("INSERT INTO TestData VALUES (123, 0, 16.27, 459.77, 123)");
    query.exec("INSERT INTO TestData VALUES (124, 0, 21.81, 490.11, 124)");
    query.exec("INSERT INTO TestData VALUES (125, 0, 17.06, 467.37, 125)");
    query.exec("INSERT INTO TestData VALUES (126, 0, 18.61, 463.7, 126)");
    query.exec("INSERT INTO TestData VALUES (127, 0, 22.68, 491.2, 127)");
    query.exec("INSERT INTO TestData VALUES (128, 0, 21.95, 465.37, 128)");
    query.exec("INSERT INTO TestData VALUES (129, 0, 16.15, 463.29, 129)");
    query.exec("INSERT INTO TestData VALUES (130, 0, 16.47, 489.19, 130)");
    query.exec("INSERT INTO TestData VALUES (131, 0, 17.03, 455.35, 131)");
    query.exec("INSERT INTO TestData VALUES (132, 0, 18.25, 479.89, 132)");
    query.exec("INSERT INTO TestData VALUES (133, 0, 20.95, 478.63, 133)");
    query.exec("INSERT INTO TestData VALUES (134, 0, 19.32, 455.43, 134)");
    query.exec("INSERT INTO TestData VALUES (135, 0, 21.93, 462.86, 135)");
    query.exec("INSERT INTO TestData VALUES (136, 0, 20.27, 458.31, 136)");
    query.exec("INSERT INTO TestData VALUES (137, 0, 21.15, 476.33, 137)");
    query.exec("INSERT INTO TestData VALUES (138, 0, 15.47, 465.49, 138)");
    query.exec("INSERT INTO TestData VALUES (139, 0, 15.75, 469.83, 139)");
    query.exec("INSERT INTO TestData VALUES (140, 0, 16, 455.5, 140)");
    query.exec("INSERT INTO TestData VALUES (141, 0, 24.76, 470, 141)");
    query.exec("INSERT INTO TestData VALUES (142, 0, 18.41, 473.96, 142)");
    query.exec("INSERT INTO TestData VALUES (143, 0, 16.49, 467.9, 143)");
    query.exec("INSERT INTO TestData VALUES (144, 0, 19.78, 470.45, 144)");
    query.exec("INSERT INTO TestData VALUES (145, 0, 16.9, 487.35, 145)");
    query.exec("INSERT INTO TestData VALUES (146, 0, 19.05, 450.95, 146)");
    query.exec("INSERT INTO TestData VALUES (147, 0, 17.97, 481.21, 147)");
    query.exec("INSERT INTO TestData VALUES (148, 0, 19.56, 478.06, 148)");
    query.exec("INSERT INTO TestData VALUES (149, 0, 20.97, 472.71, 149)");
    query.exec("INSERT INTO TestData VALUES (150, 0, 23.31, 492.48, 150)");
    query.exec("INSERT INTO TestData VALUES (151, 0, 22.43, 491.65, 151)");
    query.exec("INSERT INTO TestData VALUES (152, 0, 23.17, 462.84, 152)");
    query.exec("INSERT INTO TestData VALUES (153, 0, 24.25, 479.86, 153)");
    query.exec("INSERT INTO TestData VALUES (154, 0, 24.78, 450.81, 154)");
    query.exec("INSERT INTO TestData VALUES (155, 0, 23.8, 459.25, 155)");
    query.exec("INSERT INTO TestData VALUES (156, 0, 18.01, 455.8, 156)");
    query.exec("INSERT INTO TestData VALUES (157, 0, 16.45, 487.51, 157)");
    query.exec("INSERT INTO TestData VALUES (158, 0, 17.36, 467.61, 158)");
    query.exec("INSERT INTO TestData VALUES (159, 0, 23.55, 474.53, 159)");
    query.exec("INSERT INTO TestData VALUES (160, 0, 16.89, 481.33, 160)");
    query.exec("INSERT INTO TestData VALUES (161, 0, 20.22, 477.2, 161)");
    query.exec("INSERT INTO TestData VALUES (162, 0, 24.8, 474.95, 162)");
    query.exec("INSERT INTO TestData VALUES (163, 0, 17.2, 461.85, 163)");
    query.exec("INSERT INTO TestData VALUES (164, 0, 19.28, 451.05, 164)");
    query.exec("INSERT INTO TestData VALUES (165, 0, 23.57, 476.05, 165)");
    query.exec("INSERT INTO TestData VALUES (166, 0, 21.06, 467.59, 166)");
    query.exec("INSERT INTO TestData VALUES (167, 0, 21.02, 459.49, 167)");
    query.exec("INSERT INTO TestData VALUES (168, 0, 24.1, 493.81, 168)");
    query.exec("INSERT INTO TestData VALUES (169, 0, 15.79, 480.42, 169)");
    query.exec("INSERT INTO TestData VALUES (170, 0, 24.15, 492.16, 170)");
    query.exec("INSERT INTO TestData VALUES (171, 0, 15.85, 482.04, 171)");
    query.exec("INSERT INTO TestData VALUES (172, 0, 22.88, 469.29, 172)");
    query.exec("INSERT INTO TestData VALUES (173, 0, 21.11, 482.6, 173)");
    query.exec("INSERT INTO TestData VALUES (174, 0, 19.84, 457.63, 174)");
    query.exec("INSERT INTO TestData VALUES (175, 0, 18.68, 460.78, 175)");
    query.exec("INSERT INTO TestData VALUES (176, 0, 16.39, 465.42, 176)");
    query.exec("INSERT INTO TestData VALUES (177, 0, 16.45, 494.48, 177)");
    query.exec("INSERT INTO TestData VALUES (178, 0, 19.12, 486.88, 178)");
    query.exec("INSERT INTO TestData VALUES (179, 0, 18.87, 479.63, 179)");
    query.exec("INSERT INTO TestData VALUES (180, 0, 20.08, 459.16, 180)");
    query.exec("INSERT INTO TestData VALUES (181, 0, 23.49, 462.61, 181)");
    query.exec("INSERT INTO TestData VALUES (182, 0, 17.64, 479.04, 182)");
    query.exec("INSERT INTO TestData VALUES (183, 0, 16.5, 459.46, 183)");
    query.exec("INSERT INTO TestData VALUES (184, 0, 22.65, 451.46, 184)");
    query.exec("INSERT INTO TestData VALUES (185, 0, 17.61, 472.03, 185)");
    query.exec("INSERT INTO TestData VALUES (186, 0, 18.29, 493.44, 186)");
    query.exec("INSERT INTO TestData VALUES (187, 0, 18.86, 460.68, 187)");
    query.exec("INSERT INTO TestData VALUES (188, 0, 16.14, 456.02, 188)");
    query.exec("INSERT INTO TestData VALUES (189, 0, 16.55, 466.39, 189)");
    query.exec("INSERT INTO TestData VALUES (190, 0, 18.98, 461.91, 190)");
    query.exec("INSERT INTO TestData VALUES (191, 0, 16.29, 474.57, 191)");
    query.exec("INSERT INTO TestData VALUES (192, 0, 19.86, 492.58, 192)");
    query.exec("INSERT INTO TestData VALUES (193, 0, 17.95, 483.16, 193)");
    query.exec("INSERT INTO TestData VALUES (194, 0, 15.18, 478.41, 194)");
    query.exec("INSERT INTO TestData VALUES (195, 0, 17.88, 480.63, 195)");
    query.exec("INSERT INTO TestData VALUES (196, 0, 15.71, 475.55, 196)");
    query.exec("INSERT INTO TestData VALUES (197, 0, 15.59, 487.54, 197)");
    query.exec("INSERT INTO TestData VALUES (198, 0, 15.2, 480.82, 198)");
    query.exec("INSERT INTO TestData VALUES (199, 0, 19.61, 474.45, 199)");
    query.exec("INSERT INTO TestData VALUES (200, 0, 17.59, 480.25, 200)");
    query.exec("INSERT INTO TestData VALUES (201, 0, 23.2, 465.86, 201)");
    query.exec("INSERT INTO TestData VALUES (202, 0, 20.75, 477.57, 202)");
    query.exec("INSERT INTO TestData VALUES (203, 0, 15.32, 490.67, 203)");
    query.exec("INSERT INTO TestData VALUES (204, 0, 15.47, 460.17, 204)");
    query.exec("INSERT INTO TestData VALUES (205, 0, 22.28, 478.67, 205)");
    query.exec("INSERT INTO TestData VALUES (206, 0, 22.72, 456.38, 206)");
    query.exec("INSERT INTO TestData VALUES (207, 0, 23.14, 453.32, 207)");
    query.exec("INSERT INTO TestData VALUES (208, 0, 17.02, 487.77, 208)");
    query.exec("INSERT INTO TestData VALUES (209, 0, 15.35, 474.64, 209)");
    query.exec("INSERT INTO TestData VALUES (210, 0, 22.05, 468.14, 210)");
    query.exec("INSERT INTO TestData VALUES (211, 0, 24.02, 464.46, 211)");
    query.exec("INSERT INTO TestData VALUES (212, 0, 18.89, 478.63, 212)");
    query.exec("INSERT INTO TestData VALUES (213, 0, 23.26, 450.37, 213)");
    query.exec("INSERT INTO TestData VALUES (214, 0, 19.68, 450.05, 214)");
    query.exec("INSERT INTO TestData VALUES (215, 0, 21.11, 488, 215)");
    query.exec("INSERT INTO TestData VALUES (216, 0, 21.95, 463.63, 216)");
    query.exec("INSERT INTO TestData VALUES (217, 0, 20.29, 486.82, 217)");
    query.exec("INSERT INTO TestData VALUES (218, 0, 20.21, 480.36, 218)");
    query.exec("INSERT INTO TestData VALUES (219, 0, 17.84, 451.7, 219)");
    query.exec("INSERT INTO TestData VALUES (220, 0, 19.68, 485.08, 220)");
    query.exec("INSERT INTO TestData VALUES (221, 0, 16.02, 478.19, 221)");
    query.exec("INSERT INTO TestData VALUES (222, 0, 23.4, 452.73, 222)");
    query.exec("INSERT INTO TestData VALUES (223, 0, 17.46, 486.39, 223)");
    query.exec("INSERT INTO TestData VALUES (224, 0, 22.23, 454.47, 224)");
    query.exec("INSERT INTO TestData VALUES (225, 0, 20.35, 452.19, 225)");
    query.exec("INSERT INTO TestData VALUES (226, 0, 16.49, 459.45, 226)");
    query.exec("INSERT INTO TestData VALUES (227, 0, 22.28, 461.76, 227)");
    query.exec("INSERT INTO TestData VALUES (228, 0, 24.7, 456.58, 228)");
    query.exec("INSERT INTO TestData VALUES (229, 0, 17.86, 460.06, 229)");
    query.exec("INSERT INTO TestData VALUES (230, 0, 18.02, 489.6, 230)");
    query.exec("INSERT INTO TestData VALUES (231, 0, 16.04, 460, 231)");
    query.exec("INSERT INTO TestData VALUES (232, 0, 19.72, 463.98, 232)");
    query.exec("INSERT INTO TestData VALUES (233, 0, 23.98, 466.61, 233)");
    query.exec("INSERT INTO TestData VALUES (234, 0, 15.29, 494.04, 234)");
    query.exec("INSERT INTO TestData VALUES (235, 0, 19.84, 480.44, 235)");
    query.exec("INSERT INTO TestData VALUES (236, 0, 17.64, 469.78, 236)");
    query.exec("INSERT INTO TestData VALUES (237, 0, 16.74, 463.08, 237)");
    query.exec("INSERT INTO TestData VALUES (238, 0, 23.52, 490.08, 238)");
    query.exec("INSERT INTO TestData VALUES (239, 0, 18.6, 455.23, 239)");
    query.exec("INSERT INTO TestData VALUES (240, 0, 23.2, 481.68, 240)");
    query.exec("INSERT INTO TestData VALUES (241, 0, 21.5, 479.76, 241)");
    query.exec("INSERT INTO TestData VALUES (242, 0, 22.19, 486.05, 242)");
    query.exec("INSERT INTO TestData VALUES (243, 0, 24.65, 479.69, 243)");
    query.exec("INSERT INTO TestData VALUES (244, 0, 22.96, 488.82, 244)");
    query.exec("INSERT INTO TestData VALUES (245, 0, 23.67, 490.96, 245)");
    query.exec("INSERT INTO TestData VALUES (246, 0, 17.19, 470.52, 246)");
    query.exec("INSERT INTO TestData VALUES (247, 0, 24.31, 476.91, 247)");
    query.exec("INSERT INTO TestData VALUES (248, 0, 18.43, 465.77, 248)");
    query.exec("INSERT INTO TestData VALUES (249, 0, 23.36, 455.33, 249)");
    query.exec("INSERT INTO TestData VALUES (250, 0, 23.82, 486.2, 250)");
    query.exec("INSERT INTO TestData VALUES (251, 0, 18.98, 472.55, 251)");
    query.exec("INSERT INTO TestData VALUES (252, 0, 17.48, 476.26, 252)");
    query.exec("INSERT INTO TestData VALUES (253, 0, 21.53, 450.08, 253)");
    query.exec("INSERT INTO TestData VALUES (254, 0, 20.51, 492.25, 254)");
    query.exec("INSERT INTO TestData VALUES (255, 0, 20.5, 472.94, 255)");
    query.exec("INSERT INTO TestData VALUES (256, 0, 22.5, 484.54, 256)");
    query.exec("INSERT INTO TestData VALUES (257, 0, 17.83, 460.52, 257)");
    query.exec("INSERT INTO TestData VALUES (258, 0, 23.76, 491.55, 258)");
    query.exec("INSERT INTO TestData VALUES (259, 0, 22.89, 464.6, 259)");
    query.exec("INSERT INTO TestData VALUES (260, 0, 18.28, 488.82, 260)");
    query.exec("INSERT INTO TestData VALUES (261, 0, 22.23, 455.69, 261)");
    query.exec("INSERT INTO TestData VALUES (262, 0, 19.7, 476.93, 262)");
    query.exec("INSERT INTO TestData VALUES (263, 0, 19.49, 485.87, 263)");
    query.exec("INSERT INTO TestData VALUES (264, 0, 17.52, 462.28, 264)");
    query.exec("INSERT INTO TestData VALUES (265, 0, 16.97, 465.87, 265)");
    query.exec("INSERT INTO TestData VALUES (266, 0, 23.51, 484.06, 266)");
    query.exec("INSERT INTO TestData VALUES (267, 0, 15.39, 465.91, 267)");
    query.exec("INSERT INTO TestData VALUES (268, 0, 18.3, 450.71, 268)");
    query.exec("INSERT INTO TestData VALUES (269, 0, 21.29, 480.24, 269)");
    query.exec("INSERT INTO TestData VALUES (270, 0, 20.98, 469.2, 270)");
    query.exec("INSERT INTO TestData VALUES (271, 0, 21.45, 479.1, 271)");
    query.exec("INSERT INTO TestData VALUES (272, 0, 18.57, 473.56, 272)");
    query.exec("INSERT INTO TestData VALUES (273, 0, 16.91, 489.66, 273)");
    query.exec("INSERT INTO TestData VALUES (274, 0, 20.41, 464.98, 274)");
    query.exec("INSERT INTO TestData VALUES (275, 0, 19.35, 462.01, 275)");
    query.exec("INSERT INTO TestData VALUES (276, 0, 23.41, 457.44, 276)");
    query.exec("INSERT INTO TestData VALUES (277, 0, 23.27, 494.5, 277)");
    query.exec("INSERT INTO TestData VALUES (278, 0, 22.68, 476.09, 278)");
    query.exec("INSERT INTO TestData VALUES (279, 0, 22.24, 494.78, 279)");
    query.exec("INSERT INTO TestData VALUES (280, 0, 15.41, 464.17, 280)");
    query.exec("INSERT INTO TestData VALUES (281, 0, 23.56, 494.26, 281)");
    query.exec("INSERT INTO TestData VALUES (282, 0, 21.61, 487.59, 282)");
    query.exec("INSERT INTO TestData VALUES (283, 0, 16.23, 474.47, 283)");
    query.exec("INSERT INTO TestData VALUES (284, 0, 23.03, 464.88, 284)");
    query.exec("INSERT INTO TestData VALUES (285, 0, 21.37, 452.74, 285)");
    query.exec("INSERT INTO TestData VALUES (286, 0, 19.28, 469.23, 286)");
    query.exec("INSERT INTO TestData VALUES (287, 0, 22.32, 471.48, 287)");
    query.exec("INSERT INTO TestData VALUES (288, 0, 22.52, 466.51, 288)");
    query.exec("INSERT INTO TestData VALUES (289, 0, 21.9, 469.45, 289)");
    query.exec("INSERT INTO TestData VALUES (290, 0, 24.51, 477.42, 290)");
    query.exec("INSERT INTO TestData VALUES (291, 0, 16.02, 468.4, 291)");
    query.exec("INSERT INTO TestData VALUES (292, 0, 23.93, 477.44, 292)");
    query.exec("INSERT INTO TestData VALUES (293, 0, 18.47, 480.77, 293)");
    query.exec("INSERT INTO TestData VALUES (294, 0, 18.47, 466.45, 294)");
    query.exec("INSERT INTO TestData VALUES (295, 0, 19.31, 455.86, 295)");
    query.exec("INSERT INTO TestData VALUES (296, 0, 24.79, 452.97, 296)");
    query.exec("INSERT INTO TestData VALUES (297, 0, 18.24, 472.13, 297)");
    query.exec("INSERT INTO TestData VALUES (298, 0, 18.71, 456.69, 298)");
    query.exec("INSERT INTO TestData VALUES (299, 0, 22.75, 486.47, 299)");*/
}

StrPair SQLDatabase::selectAction(std::vector<QString> columns, QString table, std::vector<std::pair<QString, DataPass>> filter, int limit, int offset, bool desc){
    // perform a SELECT action on the Database. use the vector to get the part and assemble the request.

    QString REQUEST = "SELECT ";

    // go though columns and add to the statement
    if(columns.at(0) == ""){
        // there is nothing, use *
        REQUEST.append("* ");
    } else {
        // cycle though, and add the strinng to the final select statement.
        int count = (int)columns.size();
        for(QString col : columns){

            if(--count > 0){
                REQUEST.append(col + ", ");
            } else {
                REQUEST.append(col + " ");
            }
        }
    }

    //qWarning() << "two point 5";

    // add the table name.
    REQUEST.append("FROM " + table);

    // check for and add any filters.

    if(filter.at(0).first != ""){
        // there is a valid filter value.
        REQUEST.append(" WHERE");
        REQUEST.append(whereString(filter, " AND"));
    }

   if(desc){
       // this is a descending query, add orderby
       REQUEST.append(" ORDER BY PK DESC"); // always PK so we can get the newest first.
   }
   // Check if a limit is set, and add it if it.
   if(limit > 0)
       REQUEST.append(" LIMIT " + QString::number(limit));
   if(offset > 0)
       REQUEST.append(" OFFSET " + QString::number(offset));

    //qWarning() << REQUEST; // output request so we can see how it looks.

    QSqlQuery query;

    query.prepare(REQUEST);

    // bind values to DB call
    if(!Bind(query, filter) && (int)filter.size() > 0){
        qWarning() << "ERROR @ Bind: Filter failed to bind.";
    }

  //  qWarning() << "Filter: " << filter.size() << filter.at(0).first << " " << filter.at(0).second.num;

    // execute the query and display a success or failure.
    if(!query.exec()){
        qWarning() << "ERROR @ selectaction: " << query.lastError().text(); // ensure no errors occur during request.
        qWarning() << query.executedQuery();
    } else {
   //     qWarning() << "SUCESS @ selectaction";
    }

    std::vector<std::pair<QString, QString>> results; // always return a specified pair of results, even if they are in order.

    int length = getQueryLength(query);
  //  qWarning() << "Length: " << length;

    query.first();
    query.previous();

    if(length > 0){
        results.resize(length * (int)columns.size());
        // there are query returns to spit out.
        int d{0};
        while(query.next()){

         //   qWarning() << "Output Line #" << d;

            int i{0};
            for(QString col : columns){
                // get an return the value of each column to the pair. Presumably, the order should be maintained.
              //  qWarning() << "Result RAW: " << col << " " << query.value(i).toString();

                results.at(d * (int)columns.size() + i) = {col, query.value(i).toString()};
                ++i;
            }
            ++d;
        }
    } else {
        // return a null response.
        results.resize(1);
        results.at(0) = {"", ""};
    }

    return results; // return the results, regardless of contents.
}

StrPair SQLDatabase::searchAction(std::vector<QString> getColumns, std::vector<QString> searchColumns, QString table, QString searchTerm, std::vector<std::pair<QString, DataPass>> filter, int limit, int offset){
    // perform a SELECT action on the Database. use the vector to get the part and assemble the request.

    QString REQUEST = "SELECT ";

    // go though columns and add to the statement
    if(getColumns.at(0) == ""){
        // there is nothing, use *
        REQUEST.append("* ");
    } else {
        // cycle though, and add the strinng to the final select statement.
        int count = (int)getColumns.size();
        for(QString col : getColumns){

            if(--count > 0){
                REQUEST.append(col + ", ");
            } else {
                REQUEST.append(col + " ");
            }
        }
    }

    // add the table name.
    REQUEST.append("FROM " + table);

    int count = (int)searchColumns.size();
    REQUEST.append(" WHERE ");

    for(QString col : searchColumns){

        if(--count > 0){
            REQUEST.append(col + " LIKE :searchterm OR ");
        } else {
            REQUEST.append(col + " LIKE :searchterm");
        }
    }

    // add additional filters, if warranted.
    if(filter.at(0).first != ""){
        // unfortunately, we need to get this more complicated.
        int filters = filter.size();
        for(int i{0}; i < filters; ++i){
            // for each item add the appropriate value
            if( filter.at(i).first != ""){
                REQUEST.append(" OR " + filter.at(i).first + " = :" + filter.at(i).first + QString::number(i) );
                filter.at(i).first = filter.at(i).first + QString::number(i); // concatenate with the number system.
            }
        }
    }

    // Check if a limit is set, and add it if it.
    if(limit > 0)
        REQUEST.append(" LIMIT " + QString::number(limit));
    if(offset > 0)
        REQUEST.append(" OFFSET " + QString::number(offset));

   // qWarning() << REQUEST; // output request so we can see how it looks.

    QSqlQuery query;

     query.prepare(REQUEST);

     //qWarning() << "request prepared";

     // bind values to DB call
     if(filter.at(0).first == ""){
         filter.clear();
     }

     filter.resize(filter.size() + 1);
     filter.at(filter.size() - 1) = {"searchterm", searchTerm};
/*
     for(int i{0}; i < (int)filter.size(); ++i){
         if(filter.at(i).second.o == DPO::STRING)
            qWarning() << "filter" << QString::number(i) << filter.at(i).first << filter.at(i).second.str;
         if(filter.at(i).second.o == DPO::DOUBLE)
            qWarning() << "filter" << QString::number(i) << filter.at(i).first << filter.at(i).second.dbl;
         if(filter.at(i).second.o == DPO::NUMBER)
            qWarning() << "filter" << QString::number(i) << filter.at(i).first << filter.at(i).second.num;
     }
*/
     if(!Bind(query, filter) && (int)filter.size() > 0){
      //   qWarning() << "ERROR @ search Bind: Filter failed to bind.";
     }

     // execute the query and display a success or failure.
     if(!query.exec()){
    //     qWarning() << "ERROR @ search action: " << query.lastError().text(); // ensure no errors occur during request.
         //qWarning() << query.executedQuery();
     } else {
    //    qWarning() << "SUCESS @ search action";
     }

     std::vector<std::pair<QString, QString>> results; // always return a specified pair of results, even if they are in order.

    int length = getQueryLength(query);
  //  qWarning() << "Length: " << length;

    query.first();
    query.previous();

    if(length > 0){
        results.resize(length * (int)getColumns.size());
        // there are query returns to spit out.
        int d{0};
        while(query.next()){

         //   qWarning() << "Output Line #" << d;

            int i{0};
            for(QString col : getColumns){
                // get an return the value of each column to the pair. Presumably, the order should be maintained.
              //  qWarning() << "Result RAW: " << col << " " << query.value(i).toString();

                results.at(d * (int)getColumns.size() + i) = {col, query.value(i).toString()};
                ++i;
            }
            ++d;
        }
    } else {
        // return a null response.
        results.resize(1);
        results.at(0) = {"", ""};
    }

    return results; // return the results, regardless of contents.

}


CompanyID SQLDatabase::getNewPK(QString table){
    // take the table, and return what would be the next ID on the table. To be used BEFORE inserts
    QString REQUEST = "SELECT PK FROM " + table + " ORDER BY PK DESC LIMIT 1";

    QSqlQuery query;
    query.prepare(REQUEST);

    if(!query.exec()){
    //    qWarning() << "ERROR @ getPK: " << query.lastError().text(); // ensure no errors occur during request.
    //    qWarning() << query.executedQuery();
    } else {
    //    qWarning() << "Success in getting new PK.";
    }



    // new code to get the proper ID
    int length = getQueryLength(query);

    query.first();
    query.previous();

    if(length > 0){
        query.next();
        return query.value(0).toInt() + 1;
    }

    return 0;
}

bool SQLDatabase::updateAction(QString table, std::vector<std::pair<QString, DataPass>> updates, std::vector<std::pair<QString, DataPass>> filter){
    // will use only the table name and the filter file for all update requirements.

    // possible dangerous assumption: all filter uses PK as first value.
    if(filter.at(0).first == "PK"){
        AddSyncLog(table, filter.at(0).second.num, Sync::UPDATE);
    } else{
        qWarning() << "update for" << table << "Is not PK in first value";

        // do a select to find all values that are not PK as the first check, so a SELECT with FILTER to get all PKs
        StrPair rets = selectAction({"PK"}, table, filter);

        for(int i{0}; i < (int)rets.size(); ++i){
            // for each, do the dew
            AddSyncLog(table, rets.at(0).second.toInt(), Sync::UPDATE);
        }

    }

    QString REQUEST = "UPDATE " + table + " SET";

    if(updates.at(0).first == ""){
     //   qWarning() << "ERROR: No provided update information for SQL Update";
        return false;
    } else {
        // there is data for update.
        REQUEST.append(whereString(updates, ", "));
    }

    if(filter.at(0).first == ""){
     //   qWarning() << "ERROR: No provided filter information for SQL Update";
        return false; // we MUST provide filters for update functions.
    } else {
        // apply  the filters.
        REQUEST.append(" WHERE");
        REQUEST.append(whereString(filter, " AND"));
    }

    //qWarning() << REQUEST; // output request so we can see how it looks.
    QSqlQuery query;
    query.prepare(REQUEST);

    if(!Bind(query, updates) && (int)updates.size() > 0){
      //  qWarning() << "ERROR @ Bind: Filter failed to bind.";
        return false;
    }
    if(!Bind(query, filter) && (int)filter.size() > 0){
     //   qWarning() << "ERROR @ Bind: Filter failed to bind.";
        return false;
    }

    // execute the query and display a success or failure.
    if(!query.exec()){
     //   qWarning() << "ERROR @ updateaction: " << query.lastError().text(); // ensure no errors occur during request.
     //   qWarning() << query.executedQuery();
        return false;
    } else {
     //   qWarning() << "SUCESS @ updateaction";
    }

    return true; // return true for a success unless something prevents a successul update.
}

bool SQLDatabase::insertAction(QString table, std::vector<std::pair<QString, DataPass>> inserts){
    // create an INSERT statement for the database.

    CompanyID id = getNewPK(table); // get the next PK, for checking 0 issue.

    AddSyncLog(table, id, Sync::INSERT);

    if(id == 0 && inserts.at(0).first != "PK"){
        // this is the first item to add.
        int insertlength = inserts.size();

        inserts.resize(insertlength + 1); // add length to inserts.

        for(int i{insertlength}; i > 0; --i){
            // reset the sizes of inserts and adjust to make way for PK
            inserts.at(i) = inserts.at(i - 1);

         //   qWarning() << inserts.at(i).first;

        }

        // set the PK value
        inserts.at(0) = {"PK", (CompanyID)0};
    }

     QString REQUEST = "INSERT INTO " + table + " (";
     QString VALUES = ") VALUES (";
        // run though all inserts, add to table. During same time, create secondary values string.
     int inSize = (int)inserts.size();
     for(int i{0}; i < inSize; ++i){
        if(i+1 == inSize){
            // at max value
            REQUEST.append(inserts.at(i).first);
            VALUES.append(":" + inserts.at(i).first);
        } else {
            // normal value.
            REQUEST.append(inserts.at(i).first + ", ");
            VALUES.append(":" + inserts.at(i).first + ", ");
        }
     }

     // create the final query string
     REQUEST.append(VALUES + ")");

     //qWarning() << REQUEST; // output request so we can see how it looks.
     QSqlQuery query;
     query.prepare(REQUEST);

     // bind the values
     if(!Bind(query, inserts) && (int)inserts.size() > 0){
     //    qWarning() << "ERROR @ Bind: Filter failed to bind.";
         return false;
     }

     if(!query.exec()){
     //    qWarning() << "ERROR @ insertaction: " << query.lastError().text(); // ensure no errors occur during request.
     //    qWarning() << query.executedQuery();
         return false;
     }

    return true; // return false if fails at any point.
}

bool SQLDatabase::deleteAction(QString table, std::vector<std::pair<QString, DataPass>> filter){

    QString REQUEST = "DELETE FROM " + table + " WHERE";

    // check filter size
    if(filter.size() < 1){
     //   qWarning() << "ERROR: No filters for remove statement.";
        return false;
    }



    // load filter onto the string.
    REQUEST.append(whereString(filter, " AND"));

  //  qWarning() << REQUEST; // output request so we can see how it looks.

    if(filter.at(0).first == "PK"){
        AddSyncLog(table, filter.at(0).second.num, Sync::DELETE);
    } else{
        qWarning() << "update for" << table << "Is not PK in first value";

        // do a select to find all values that are not PK as the first check, so a SELECT with FILTER to get all PKs
        StrPair rets = selectAction({"PK"}, table, filter);

        for(int i{0}; i < (int)rets.size(); ++i){
            // for each, do the dew
            AddSyncLog(table, rets.at(0).second.toInt(), Sync::DELETE);
        }
    }

    QSqlQuery query;

    query.prepare(REQUEST);

    // bind values to DB call
    if(!Bind(query, filter) && (int)filter.size() > 0){
      //  qWarning() << "ERROR @ Bind: Filter failed to bind.";
        return false;
    }

    // execute the query and display a success or failure.
    if(!query.exec()){
      //  qWarning() << "ERROR @ deleteaction: " << query.lastError().text(); // ensure no errors occur during request.
        return false;
    }

    return true;
}

QString SQLDatabase::whereString(std::vector<std::pair<QString, DataPass>> filter, QString separator){
    // take the filter variable, and filter it into a QString object for return..
    if(filter.at(0).first != ""){
        // there is a filter, we'll iterate through and add them. we'll need to do this a second time for binding.
        QString REQUEST = ""; // NULL value, so it can be used for multiple functions.
        int count = (int)filter.size();
        for(std::pair<QString, DataPass> &f : filter){ // DataPass second value is unused here.

            QString equal = "=";

            if(f.second.notEqual){
              equal = "!=";
            }

            if(--count > 0){
                REQUEST.append(" " + f.first + " " + equal + " :" + f.first + separator); // there are more
            } else {
                REQUEST.append(" " + f.first + " " + equal + " :" + f.first);
            }
        }
        return REQUEST;
    }
    // if no filters exist, bypass and return a NULL value.
    return "";
}

bool SQLDatabase::Bind(QSqlQuery &query, std::vector<std::pair<QString, DataPass>> filter){
    // bind the filter variables to the database call.
    // if there is anything that is NOT in filter, we'll need to add it prior to calling this.

    // return value of true or false on completion and success.
    if(filter.at(0).first != ""){
        for(std::pair<QString, DataPass> &f : filter){
            // iterate through and bind values for the filters.
            QString binder = ":" + f.first;
            if(f.second.o == DPO::NUMBER){
                QVariant val{(long long)f.second.num};
                query.bindValue(binder, val); // bind the NUMBER value.
            } else if(f.second.o == DPO::DOUBLE){
                QVariant val{(double)f.second.dbl};
                query.bindValue(binder, val); // bind the DOUBLE value.
            } else if(f.second.o == DPO::STRING){
                query.bindValue(binder, f.second.str); // bind the STRING value.
            }

         //   qWarning() << "Bound Value for " << binder << ": " << query.boundValue(":" + f.first);
        }
        return true; // values are added and we are done.
    } else {
        return false; // techincal success, though false will declare no values were added.
    }
}

int SQLDatabase::getQueryLength(QSqlQuery &query){
    int totalrows = 0;
    if(query.last()){
        totalrows = query.at() + 1;
        query.first();
       // query.previous();
    }

    return totalrows;
}

bool SQLDatabase::AddSyncLog(QString table, CompanyID ID, Sync SyncType){
    // create an INSERT statement for the database.

    //qWarning() << "Table: " << table << " ID " << ID;

    // check the DB if an action for PK at TABLE exists, if it does, skip this.

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(2);
    filter.at(0) = {"TablePK", ID};
    filter.at(1) = {"TableName", table};

    StrPair rets = selectAction({"PK"}, "DataSync", filter);

    //qWarning() << "SYNC CHECK: " << rets.at(0).first << rets.at(0).second;

    // check the first value, as it is the only one that matters.
    if(rets.at(0).first == "PK"){
    //    qWarning() << "Duplicate found for PK: " << rets.at(0).second;
        return false;
    }





    CompanyID insertid = getNewPK("DataSync");

    QString REQUEST = "INSERT INTO DataSync (PK, TableName, TablePK, Type) VALUES (:PK, :TableName, :TablePK, :Type)";

    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(4);
    inserts.at(0) = {"PK", insertid};
    inserts.at(1) = {"TableName", table};
    inserts.at(2) = {"TablePK", ID};
    inserts.at(3) = {"Type", (CompanyID)SyncType};

    QSqlQuery query;
    query.prepare(REQUEST);

     // bind the values
     if(!Bind(query, inserts) && (int)inserts.size() > 0){
         qWarning() << "ERROR @ Sync Bind: Filter failed to bind.";
         return false;
     }

     if(!query.exec()){
         qWarning() << "ERROR @ insertaction: " << query.lastError().text(); // ensure no errors occur during request.
         qWarning() << query.executedQuery();
         return false;
     }

    return true; // return false if fails at any point.
}
