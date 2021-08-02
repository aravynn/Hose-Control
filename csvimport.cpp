#include "sqldatabase.h"

bool SQLDatabase::ImportFile(QString file, QString table, char separator){
    //import CSV files from external sources, parse, then import to the appropriate table in the DB.
    // generic name in case we need to expand for XML or other file types in the future.
    // ASSUMED: " for delimiter and , for separator. This is really internal only so it shouldn't be an issue.

    //qWarning() << "import @ 1";
    // PARSE HERE, INTO QSTRING ARRAY
    std::vector<std::vector<QString>> data;
    QFile datafile(file);
    if(!datafile.open(QIODevice::ReadOnly)){
   //     //qWarning() << datafile.errorString();
        return false; // it failed to open the file.
    }
    int i = 0;

    //qWarning() << "import @ 2";

    while(!datafile.atEnd()){

        QList<QByteArray> t = datafile.readLine().split(separator);
        data.push_back({});

        for(QByteArray v : t){
            data.at(i).push_back(v);
        }

        ++i;
    }

    // data has been created into an array for import. it's not perfect but it'll work well enough.
    //qWarning() << "import @ 3";

    // internal vectors used for storing the table columns and types.
    std::vector<QString> columns;
    std::vector<ColumnType> columnType;
    QString subtable = ""; // only used if secondary tables are required.
    std::vector<QString> subColumns;
    std::vector<ColumnType> subColumnType;

    //qWarning() << "import @ 4";

    if(table == "Companies"){
        // add the company information. Will do this in 3 parts,
        //qWarning() << "import company @ a";
        for(std::vector d : data){
            // for each, go through and insert.

            if(d.size() < 9){
                return false; // get out, this is no good.
            }

            // get the id for other inserts.
            CompanyID id = getNewPK(table);

            // required fields: CompanyName|Bill1|Bill2|BillCity|BillProvince|BillPostal|BillCountry|ContactName|Phone|Email

            //qWarning() << "import company @ b";

            // insert the company data.
            std::vector<std::pair<QString, DataPass>> inserts;
            inserts.resize(9);
            inserts.at(0) = {"PK", id};
            inserts.at(1) = {"CompanyName", d.at(0)};
            inserts.at(2) = {"BillAddr1", d.at(1)};
            inserts.at(3) = {"BillAddr2", d.at(2)};
            inserts.at(4) = {"BillCity", d.at(3)};

            //qWarning() << "import company @ b2";

            inserts.at(5) = {"BillProvince", d.at(4)};
            inserts.at(6) = {"BillPostal", d.at(5)};
            inserts.at(7) = {"BillCountry", d.at(6)};
            inserts.at(8) = {"Notes", ""}; // no notes to enter.



            insertAction(table, inserts); // insert this person into DB.

            // insert the customer.
            subtable = "Contacts";

            CompanyID cid = getNewPK(subtable); // for the customer info.

            //qWarning() << "import company @ c";

            inserts.clear();
            inserts.resize(3);
            inserts.at(0) = {"PK", cid};
            inserts.at(1) = {"CompanyPK", id};
            inserts.at(2) = {"Name", d.at(7)};
            insertAction(subtable, inserts);

            subtable = "ContactPoint";

            CompanyID qid = getNewPK(subtable);

            //qWarning() << "import company @ d";

            // insert contact lists phone
            inserts.clear();
            inserts.resize(4);
            inserts.at(0) = {"PK", qid};
            inserts.at(1) = {"ContactPK", cid};
            inserts.at(2) = {"List", d.at(8)};
            inserts.at(3) = {"Type", "phone"};
            insertAction(subtable, inserts);

            //qWarning() << "import company @ e";

            // insert contact lists email
            inserts.clear();
            inserts.resize(4);
            inserts.at(0) = {"PK", qid + 1};
            inserts.at(1) = {"ContactPK", cid};
            inserts.at(2) = {"List", d.at(9)};
            inserts.at(3) = {"Type", "email"};
            insertAction(subtable, inserts);

            //qWarning() << "import company @ f";

            // add the billing address as a location.
            subtable = "Locations";

            qid = getNewPK(subtable);

            inserts.clear();
            inserts.resize(10);
            inserts.at(0) = {"PK", qid};
            inserts.at(1) = {"CompanyPK", id};
            inserts.at(2) = {"isPrimary", (CompanyID)0};
            inserts.at(3) = {"ShipAddr1", d.at(1)};
            inserts.at(4) = {"ShipAddr2", d.at(2)};
            inserts.at(5) = {"ShipCity", d.at(3)};
            inserts.at(6) = {"ShipProvince", d.at(4)};
            inserts.at(7) = {"ShipPostal", d.at(5)};
            inserts.at(8) = {"ShipCountry", d.at(6)};
            inserts.at(9) = {"ShipName", "Billing"};
            insertAction(subtable, inserts);
        }

        //qWarning() << "import company @ g";


    }

    if(table == "HoseTemplates"){
        // add the company information. Will do this in 3 parts,
        //qWarning() << "import template @ a";
        for(std::vector d : data){
            // for each, go through and insert.

            if(d.size() < 13){
                return false;
            }

            // required fields: Part Number, Name, Manufacturer, Description, DistributorRef, Inner Dia, Overall Length, Cut Length, Working Pres, Test Pres, Test Time, Image, Notes
            CompanyID id = getNewPK(table);

            //qWarning() << "import template @ a1";

            // insert the template data.
            std::vector<std::pair<QString, DataPass>> inserts;
            inserts.resize(16);
            inserts.at(0) = {"PK", id};
            inserts.at(1) = {"PartNumber", d.at(0)};
            inserts.at(2) = {"Name", d.at(1)};
            inserts.at(3) = {"Manufacturer", d.at(2)};
            inserts.at(4) = {"Description", d.at(3)};
            inserts.at(5) = {"DistributorRef", d.at(4)};
            inserts.at(6) = {"InnerDiameter", d.at(5)};
            inserts.at(7) = {"OverallLength", d.at(6)};
            inserts.at(8) = {"CutLength", d.at(7)};
            inserts.at(9) = {"WorkingPres", (CompanyID)d.at(8).toLong()};
            inserts.at(10) = {"TestPres", (CompanyID)d.at(9).toLong()};
            inserts.at(11) = {"TestTime", (CompanyID)d.at(10).toLong()};
            inserts.at(12) = {"CouplingAPK", (CompanyID)0};
            inserts.at(13) = {"CouplingBPK", (CompanyID)0};

            //qWarning() << "import template @ a2";

            HTDirectoryControl dir;
            QString extension;
            switch(dir.getFileType(d.at(11))){
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
                //qWarning() << "Error: Image Failure for " << d.at(11);
                break;
            }

            //qWarning() << "import template @ a3";

            QString company = "MEPBrothers";
            QString directoryRoot = dir.getImagesFolder(company, false); // for hose template images, we'll always use the MEPBrothers root folder.
            QString imageName = dir.cleanName(d.at(1)); // clean the hose template name to set for the file transfer.
            QString finalpath = directoryRoot + "/" +  imageName + extension;

            //qWarning() << "import template @ a4";

            int i{1};
            while(dir.isFileExisting(finalpath)){
               // take the existing filepath and append numerals until we find a non-existing one.
               finalpath = directoryRoot + "/" +  imageName + "_" + QString::number(i) + extension;
               ++i;
            }

            //qWarning() << "import template @ a5";

            if(dir.moveFile(d.at(11), finalpath)){
                inserts.at(14) = {"Image", finalpath };
            } else {
                inserts.at(14) = {"Image", ""};
            }

            //qWarning() << "import template @ a6";

            inserts.at(15) = {"Notes", d.at(12)};
            insertAction(table, inserts); // insert this person into DB.

        }
         //qWarning() << "import template @b";
    }

    if(table == "FittingTemplates"){
        // add the company information. Will do this in 3 parts,
         //qWarning() << "import fittign @ a";
        for(std::vector d : data){
            // for each, go through and insert.

            if(d.size() < 4){
                return false;
            }

            // required fields: Name, Part Number, Description, Attach Method
            CompanyID id = getNewPK(table);

            // insert the template data.
            std::vector<std::pair<QString, DataPass>> inserts;
            inserts.resize(5);
            inserts.at(0) = {"PK", id};
            inserts.at(1) = {"Name", d.at(0)};
            inserts.at(2) = {"PartNumber", d.at(1)};
            inserts.at(3) = {"Description", d.at(2)};
            inserts.at(4) = {"AttachMethod", d.at(3)};

            insertAction(table, inserts); // insert this person into DB.
        }

         //qWarning() << "import fitting @ b";
    }
    //qWarning() << "import @ 5";
    return true;

}
