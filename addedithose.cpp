#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Add or edit page for hoses. will come probaby from a customer or a template page.
 *
 */

void HoseTracker::loadAddHose(int id, IDSendType idtype){
    // load the hose page.

    //m_IDPass = id; // DON'T set the ID. with this. this should be the Hose id (if set)
    m_IDPass = -1;
    m_IDSequence = 1;

    // clear the page for the next load.
    AddHoseClearPage();

    switch(idtype){
        case IDSendType::Hose:
        //    qWarning() << "add/load hose";
            AddHoseLoadHose(id);
            break;
        case IDSendType::HoseTemplate:
            //SetHoseTemplate(id);
          //  qWarning() << "load by template";
            ui->HoseTemplate_AddHose->setCurrentIndex(getTemplateDropdownIndex(id));
            break;
        case IDSendType::Customer:
            //SetCompany(id);
           // qWarning() << "load by customer";
            ui->HoseOwner_AddHose->setCurrentIndex(getOwnerDropdownIndex(id));
            break;
        default:
           // qWarning() << "load blank page";
            break;
    }
}

void HoseTracker::AddHoseClearPage(){
 //   qWarning() << "add hose clear page";
    // clear the form entirely.
    ui->AssetNumbers_AddHose->setText("");
    ui->ChipIds_AddHose->setText("");

    loadOwnerDropdowns();

    ui->HoseLocation_addHose->clear();

    loadTemplateDropdowns();

    ui->PONumber_AddHose->setText("");
    ui->AssembledBy_AddHose->setText("");
    ui->ManufactureDate_AddHose->clear();
    ui->ExpiryDate_AddHose->clear();
    ui->RetestCheckBox_AddHose->setChecked(false);
    ui->TemplatePartNumber_AddHose->setText("");
    ui->TemplateHoseName_AddHose->setText("");
    ui->HoseManufacturer_AddHose_2->setText("");
    ui->TemplateDescription_AddHose->setText("");
    ui->TemplateDistRef_AddHose->setText("");
    ui->TemplateModelNumber_AddHose->setText("");
    ui->TemplateHoseID_AddHose->setText("");
    ui->TemplateOverallLength_AddHose->setText("");
    ui->TemplateCutLength_AddHose->setText("");
    ui->TemplateWP_AddHose->setText("");
    ui->TemplateTestPressure_AddHose->setText("");
    ui->TestTimeMinutes_AddHose->setValue(0);
    ui->TestTimeSeconds_AddHose->setValue(0);
    ui->End1Coupling_AddHose->clear();
    ui->End1Attachment_AddHose->setText("");
    ui->End1Coupling_AddHose->clear();
    ui->End2Attachment_AddHose->setText("");
    ui->HoseNotes_AddHose->setPlainText("");
    ui->CustomerIDNumber_AddHose->setText("");

    ui->AssembledBy_AddHose->setText("");
    ui->PONumber_AddHose->setText("");
    ui->RetestCheckBox_AddHose->setChecked(false);

    loadFittingDropdowns();

    ui->HoseStatus_AddEnd->setText("N/A");
    ui->RetestDate_AddHose->clear();

    QDate today;
    today = today.currentDate();

    // set default dates.
    ui->ManufactureDate_AddHose->setDate(today);

    today = today.addYears(1);
    ui->ExpiryDate_AddHose->setDate(today);
    ui->RetestDate_AddHose->setDate(today);
    // clear table data.
    tableClear();

    // enable all fields
    ui->HoseTemplate_AddHose->setEnabled(true);
    ui->ManufactureDate_AddHose->setEnabled(true);
    ui->TemplatePartNumber_AddHose->setEnabled(true);
    ui->TemplateHoseName_AddHose->setEnabled(true);
    ui->HoseManufacturer_AddHose_2->setEnabled(true);
    ui->TemplateDescription_AddHose->setEnabled(true);
    ui->TemplateDistRef_AddHose->setEnabled(true);
    ui->TemplateModelNumber_AddHose->setEnabled(true);
    ui->TemplateHoseID_AddHose->setEnabled(true);
    ui->TestTimeMinutes_AddHose->setEnabled(true);
    ui->TestTimeSeconds_AddHose->setEnabled(true);
    ui->AssembledBy_AddHose->setEnabled(true);
    ui->PONumber_AddHose->setEnabled(true);
    ui->RetestCheckBox_AddHose->setEnabled(true);



}
void HoseTracker::SetHoseTemplate(CompanyID id){
  //  qWarning() << "set hose template" << QString::number(id);
    // set the hose template dropdown option. This will need to account for missing ID's
    if(id > -1){
        // confirm that an ID is actually given.
        HoseTemplates *h = new HoseTemplates(id);

        // update the dropdown position for the hose template.
        //ui->HoseTemplate_AddHose->setCurrentIndex(getTemplateDropdownIndex(id));

        //getTemplateDropdownIndex

        // set the default fields

        ui->TemplatePartNumber_AddHose->setText(h->GetPartNumber());
        ui->TemplateHoseName_AddHose->setText(h->GetName());
        ui->HoseManufacturer_AddHose_2->setText(h->GetManufacturer());
        ui->TemplateDescription_AddHose->setText(h->GetDescription());
        ui->TemplateDistRef_AddHose->setText(h->GetDistRefNumber());

        ui->TemplateHoseID_AddHose->setText(h->GetInnerDiameter());
        ui->TemplateModelNumber_AddHose->setText(h->GetPartNumber());
        ui->TemplateOverallLength_AddHose->setText(h->GetOverallLength());
        ui->TemplateCutLength_AddHose->setText(h->GetCutLength());
        ui->TemplateWP_AddHose->setText(QString::number(h->GetWP()));
        ui->TemplateTestPressure_AddHose->setText(QString::number(h->getTestPressure()));

        int totalTime = h->getTestTime();
        int minutes = floor(totalTime / 60); // floor value is minutes.
        int seconds = totalTime % 60; // remainder is seconds.

        ui->TestTimeMinutes_AddHose->setValue(minutes);
        ui->TestTimeSeconds_AddHose->setValue(seconds);

        // set the default ends and attachment methods.
        ui->End1Coupling_AddHose->setCurrentIndex(h->GetEnd1()->GetID() + 1);
        ui->End2Coupling_AddHose->setCurrentIndex(h->GetEnd2()->GetID() + 1);

        ui->End1Attachment_AddHose->setText(h->GetEnd1()->GetAttachMethod());
        ui->End2Attachment_AddHose->setText(h->GetEnd2()->GetAttachMethod());

        QString directory = h->GetImageLocation();

        if(directory != ""){
             QPixmap pix(directory);
            ui->HoseImage_AddHose->setPixmap(pix.scaledToWidth(120));
        } else {

            ui->HoseImage_AddHose->clear();
        }

        delete h;
    }
}
void HoseTracker::SetCompany(CompanyID id){
 //   qWarning() << "set company";
    // set the company dropdown option. This will need to account for missing ID's

    if(id < 0)
        return;

//    qWarning() << "SetCompany()" << QString::number(id);

    //ui->HoseOwner_AddHose->setCurrentIndex(getOwnerDropdownIndex(id));

    // load the locations for this owner.
     ui->HoseLocation_addHose->clear();

     ui->HoseLocation_addHose->addItem("- Choose Location -");

     Company *c = new Company(id);

     int count = c->GetLocationCount();

     for(int i{0}; i < count; ++i){
         ui->HoseLocation_addHose->addItem(c->GetLocationByNumber(i).Name + " (" + c->GetLocationByNumber(i).Addr1 + ")");
     }
     delete c;

}
void HoseTracker::AddHoseLoadHose(CompanyID id, int offset){
    // load the hose by ID.

    if(id == -1) return;

    // load all hose inputs

    m_IDPass = id; // set for future use.

    Hose *h = new Hose(id);

    ui->AssetNumbers_AddHose->setText(h->getAssetNumber());
    ui->ChipIds_AddHose->setText(h->getChipID());

    ui->HoseOwner_AddHose->setCurrentIndex(getOwnerDropdownIndex(h->getOwnerID()));
    SetCompany(h->getOwnerID());

    ui->HoseLocation_addHose->setCurrentIndex(h->GetLocationID() + 1); // this is probably where we'll see an issue.

    ui->HoseTemplate_AddHose->setCurrentIndex(getTemplateDropdownIndex(h->getID()));


    ui->ManufactureDate_AddHose->setDate(h->getMFGDate());
    ui->ExpiryDate_AddHose->setDate(h->getEXPDate());

    ui->TemplatePartNumber_AddHose->setText(h->GetPartNumber());
    //ui->TemplatePartNumber_AddHose->setEnabled(false);
    ui->TemplateHoseName_AddHose->setText(h->GetName());
    ui->HoseManufacturer_AddHose_2->setText(h->GetManufacturer());
    ui->TemplateDescription_AddHose->setText(h->GetDescription());
    ui->TemplateDistRef_AddHose->setText(h->GetDistRefNumber());
    ui->TemplateModelNumber_AddHose->setText(h->GetPartNumber());
    ui->TemplateHoseID_AddHose->setText(h->GetInnerDiameter());
    ui->TemplateOverallLength_AddHose->setText(h->GetOverallLength());
    ui->TemplateCutLength_AddHose->setText(h->GetCutLength());
    ui->TemplateWP_AddHose->setText(QString::number(h->GetWP()));
    ui->TemplateTestPressure_AddHose->setText(QString::number(h->getTestPressure()));
    ui->CustomerIDNumber_AddHose->setText(h->getCustomerIDNumber());

    ui->HoseNotes_AddHose->setPlainText(h->getHoseNotes());

    ui->AssembledBy_AddHose->setText(h->getHoseAssembler());
    ui->PONumber_AddHose->setText(h->getPONumber());
    ui->RetestCheckBox_AddHose->setChecked(h->getDoRetest());


    int totalTime = h->getTestTime();

    int minutes = floor(totalTime / 60); // floor value is minutes.
    int seconds = totalTime % 60; // remainder is seconds.

    ui->TestTimeMinutes_AddHose->setValue(minutes);
    ui->TestTimeSeconds_AddHose->setValue(seconds);

    ui->End1Coupling_AddHose->setCurrentIndex(h->GetEnd1()->GetID() + 1);
    ui->End1Attachment_AddHose->setText(h->GetEnd1()->GetAttachMethod());
    ui->End2Coupling_AddHose->setCurrentIndex(h->GetEnd2()->GetID() + 1);
    ui->End2Attachment_AddHose->setText(h->GetEnd2()->GetAttachMethod());

    // disable uneditable all fields
    ui->HoseTemplate_AddHose->setEnabled(false);
    ui->ManufactureDate_AddHose->setEnabled(false);
    ui->TemplatePartNumber_AddHose->setEnabled(false);
    ui->TemplateHoseName_AddHose->setEnabled(false);
    ui->HoseManufacturer_AddHose_2->setEnabled(false);
    ui->TemplateDescription_AddHose->setEnabled(false);
    ui->TemplateDistRef_AddHose->setEnabled(false);
    ui->TemplateModelNumber_AddHose->setEnabled(false);
    ui->TemplateHoseID_AddHose->setEnabled(false);
    ui->TestTimeMinutes_AddHose->setEnabled(false);
    ui->TestTimeSeconds_AddHose->setEnabled(false);
    ui->AssembledBy_AddHose->setEnabled(false);
    ui->PONumber_AddHose->setEnabled(false);
    ui->RetestCheckBox_AddHose->setEnabled(false);

    ui->HoseStatus_AddEnd->setText(h->getStatus());
    //ui->RetestDate_AddHose->clear(); // SET THIS

    // load any/all previous hose tests for this hose.

    QString directory = h->GetImageLocation();

    if(directory != ""){
         QPixmap pix(directory);
        ui->HoseImage_AddHose->setPixmap(pix.scaledToWidth(120));
    } else {

        ui->HoseImage_AddHose->clear();
    }



    StrPair tests = getTestPKs(id, m_TableLimit, offset);

    int count = (int)tests.size();

    for(int i{0}; i < count; ++i){
        // for each, output to the chart.
        // date Test# TestBy Result (note: no buttons here)
        if(tests.at(i).second == ""){
            // there is no result
            continue;
        }
        HoseTest *ht = new HoseTest(tests.at(i).second.toInt());

        m_TableLabels[m_LabelsExist] = new QLabel();
        m_TableLabels[m_LabelsExist + 1] = new QLabel();
        m_TableLabels[m_LabelsExist + 2] = new QLabel();
        m_TableLabels[m_LabelsExist + 3] = new QLabel();

        m_TableButtons[m_ButtonsExist] = new QPushButton();

        TimeConvert t;

        m_TableLabels[m_LabelsExist]->setText(t.print(ht->getDate()));
        m_TableLabels[m_LabelsExist + 1]->setText(ht->getTestNumber());
        m_TableLabels[m_LabelsExist + 2]->setText(ht->getCreator());
        m_TableLabels[m_LabelsExist + 3]->setText(ht->getTestResult());

        m_TableButtons[m_ButtonsExist]->setText("View Test");

        ui->HoseTestsArea_AddHose->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
        ui->HoseTestsArea_AddHose->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
        ui->HoseTestsArea_AddHose->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);
        ui->HoseTestsArea_AddHose->addWidget(m_TableLabels[m_LabelsExist + 3], m_TableLines + 1, 4);
        ui->HoseTestsArea_AddHose->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);

        connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
            loadPage(Pages::TestResults, tests.at(i).second.toInt(), IDSendType::HoseTest, 1);
        });

        // iterate counters
        m_LabelsExist += 4;
        ++m_ButtonsExist;
        ++m_TableLines;

        delete ht;
    }
    StrPair alltests = getTestPKs(id);
    int totalPages = alltests.size() / m_TableLimit;

    if(alltests.size() % m_TableLimit > 0){ totalPages++; }
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentTests_AddHose->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                AddHoseLoadHose(id, i * m_TableLimit);
             });

             ++m_ButtonsExist;
        }
    }
    delete h;
}

void HoseTracker::loadTemplateDropdowns(){
   // qWarning() << "load template dropdowns";
    // load the fitting dropdowns for ALL pages.
    // get the list of options for the dropdowns, and spit them out.

    // get the most recent hoses created.
    StrPair data = getHoseTemplatePKs();

    int dataSize = data.size();

    ui->HoseTemplate_AddHose->clear();

    ui->HoseTemplate_AddHose->addItem("- Choose Hose Template -");

    if(data.at(0).first == "")
        return;

    for(int i{0}; i < dataSize; ++i){
        // set the dropdown values.
        HoseTemplates *ft = new HoseTemplates(data.at(i).second.toInt());
        ui->HoseTemplate_AddHose->addItem(ft->GetName());
        delete ft;
    }
}

int HoseTracker::getTemplateDropdownIndex(CompanyID id){
  //  qWarning() << "get template dropdown index";
    // take the given ID of the hose, then convert that to the INDEX of the dropdown.
    // We'll need to take the total ID's, find the ID, and return the index number from the results.
    // get the data, we'll use for comparison
    StrPair data = getHoseTemplatePKs();
    int dlen = (int)data.size();
    for(int i{0}; i < dlen; ++i){
        // check each for a matching value.
        if(id == data.at(i).second.toInt())
            return i + 1;
    }

}

CompanyID HoseTracker::getTemplateIDFromIndex(int index){
    qWarning() << "get template id from index";
    // take the given index and return the ID of the appropriate company.
    // this'll be a simple function.
    StrPair data = getHoseTemplatePKs();

    return data.at(index - 1).second.toLong();
}

StrPair HoseTracker::getHoseTemplatePKs(){
   // qWarning() << "get hose template pks";
    // return a list of hose template PK's in a std::vector. Will be used by multiple locations
    std::vector<QString> getColumns{"PK"};
    QString table = "HoseTemplates";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""}; // nothing.

    // get the most recent hoses created.
    return db->selectAction(getColumns, table, filter);
}


StrPair HoseTracker::getOwnerPKs(){
  //  qWarning() << "get owner pks";
    std::vector<QString> getColumns{"PK"};
    QString table = "Companies";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""}; // nothing.

    // get the most recent hoses created.
     return db->selectAction(getColumns, table, filter);

}

StrPair HoseTracker::getTestPKs(CompanyID hose, int limit, int offset){
  //  qWarning() << "get test pks";
    // load all tests from the given ID.
    std::vector<QString> getColumns{"PK"};
    QString table = "HoseTests";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"HosePK", hose}; // nothing.

    // get the most recent hoses created.
     return db->selectAction(getColumns, table, filter, limit, offset, true);
}


int HoseTracker::getOwnerDropdownIndex(CompanyID id){
 //   qWarning() << "get owner dropdown index";
    StrPair data = getOwnerPKs();
    int dlen = (int)data.size();
    for(int i{0}; i < dlen; ++i){
        // check each for a matching value.
        if(id == data.at(i).second.toInt())
            return i + 1;
    }
}

CompanyID HoseTracker::getOwnerIDFromIndex(int index){
    StrPair data = getOwnerPKs();

    return data.at(index - 1).second.toLong();
}

void HoseTracker::loadOwnerDropdowns(){
    // load the fitting dropdowns for ALL pages.
    // get the list of options for the dropdowns, and spit them out.
  //  qWarning() << "Load Owner Dropdowns";
    // get the most recent hoses created.
    StrPair data = getOwnerPKs();

    int dataSize = data.size();

    ui->HoseOwner_AddHose->clear();
   // ui->HoseOwner_TestCenter->clear();

    ui->HoseOwner_AddHose->addItem("- Choose Company -");
  //  ui->HoseOwner_TestCenter->addItem("- Choose Company -");

    if(data.at(0).first == "")
        return;

    for(int i{0}; i < dataSize; ++i){
        // set the dropdown values.
        Company *ft = new Company(data.at(i).second.toInt());
        ui->HoseOwner_AddHose->addItem(ft->getName());
      //  ui->HoseOwner_TestCenter->addItem(ft->getName());
        delete ft;
    }
}

void HoseTracker::on_HoseCouplingTemplate_Change(int c){
    // take the indexes of the couplings and update the attachment methods
  //  qWarning() << "Coupling Template Change";


    if(ui->End1Coupling_AddHose->count() < 2 || ui->End2Coupling_AddHose->count() < 2){
        return; // there is no fittings to work with.
    }

    int ida = ui->End1Coupling_AddHose->currentIndex() - 1;
    int idb = ui->End2Coupling_AddHose->currentIndex() - 1;

    if(ida < 0){
        ida = 0;
    }

    if(idb < 0){
        idb = 0;
    }

    //qWarning() << QString::number(ida) << QString::number(idb);

    FittingTemplate *fta = new FittingTemplate(ida);
    FittingTemplate *ftb = new FittingTemplate(idb);

    ui->End1Attachment_AddHose->setText(fta->GetAttachMethod());
    ui->End2Attachment_AddHose->setText(ftb->GetAttachMethod());

    delete fta;
    delete ftb;
}

void HoseTracker::on_HoseOwner_change(int c){
    // change the owner information.4
  //  qWarning() << "Hose Owner Change";
    SetCompany(c - 1);

}

void HoseTracker::on_HoseTemplate_change(int c){
   // qWarning() << "Hose Template Change";
    SetHoseTemplate(c - 1);
}

void HoseTracker::on_HoseSaveChanges_clicked(){
    // save changes or add a new hose, depending on the selected action.
   // qWarning() << "Save Hose Changes";

    if(ui->HoseOwner_AddHose->currentIndex() < 1 || ui->HoseLocation_addHose->currentIndex() < 1 || ui->HoseTemplate_AddHose->currentIndex() < 1){
        // not everything selected
        ui->StatusBar->setText("Error: Please Select Template, Owner and Location");
        return;
    }

    Company *c = new Company(getOwnerIDFromIndex(ui->HoseOwner_AddHose->currentIndex()));

    if(m_IDPass < 0){
        // save as a new hose

       // qWarning() << "1";
        ui->StatusBar->setText("Status: Saving");

        // if this is a new hose, we need to explode the asset number and enter each individually.
        QStringList assetno = ui->AssetNumbers_AddHose->text().split("|");
        QStringList chips = ui->ChipIds_AddHose->text().split("|");
        QStringList cidno = ui->CustomerIDNumber_AddHose->text().split("|");
        int count = (int)assetno.size();

        m_IDSequence = count;

       // qWarning() << "2";
        CompanyID newid = -1;
        for(int i{0}; i < count; ++i){
            // for each, add to system.

       //     qWarning() << "3 loop";

            Hose *h = new Hose(
                        assetno.at(i),
                        ((chips.size() > i) ? chips.at(i) : ""),
                        c->GetLocationByNumber(ui->HoseLocation_addHose->currentIndex() - 1).id,
                        getOwnerIDFromIndex(ui->HoseOwner_AddHose->currentIndex()),
                        ui->ManufactureDate_AddHose->date(),
                        ui->ExpiryDate_AddHose->date(),
                        ui->AssembledBy_AddHose->text(),
                        ui->PONumber_AddHose->text(),
                        ui->RetestCheckBox_AddHose->isChecked(),
                        ui->AssembledBy_AddHose->text(),
                        "Assembled",
                        ui->End1Attachment_AddHose->text(),
                        ui->End2Attachment_AddHose->text(),
                        ui->HoseNotes_AddHose->toPlainText(),
                        ui->TemplateOverallLength_AddHose->text(),
                        ui->TemplateCutLength_AddHose->text(),
                        ui->TemplateWP_AddHose->text().toInt(),
                        ui->TemplateTestPressure_AddHose->text().toInt(),
                        ui->End1Coupling_AddHose->currentIndex() - 1,
                        ui->End2Coupling_AddHose->currentIndex() - 1,
                        getTemplateIDFromIndex(ui->HoseTemplate_AddHose->currentIndex()),
                        ((cidno.size() > i) ? cidno.at(i) : ""));

        //    qWarning() << "4 loop";

            if(newid == -1){
                newid = h->getHoseID(); // get the hose ID of the first hose for future use.
            }
            delete h;
        }
      //  qWarning() << "5";
        m_IDPass = newid;
        ui->StatusBar->setText("Status: Ready");


    } else {
        // update the existing hose.

        ui->StatusBar->setText("Status: Updating");

        Hose *h = new Hose(m_IDPass);

      //  qWarning() << QString::number( getOwnerIDFromIndex(ui->HoseOwner_AddHose->currentIndex() ));
        h->UpdateHose(
                    ui->AssetNumbers_AddHose->text(),
                    ui->ChipIds_AddHose->text(),
                    getOwnerIDFromIndex(ui->HoseOwner_AddHose->currentIndex()),
                    c->GetLocationByNumber(ui->HoseLocation_addHose->currentIndex() - 1).id,
                    ui->TemplateOverallLength_AddHose->text(),
                    ui->TemplateCutLength_AddHose->text(),
                    ui->End1Coupling_AddHose->currentIndex() - 1,
                    ui->End2Coupling_AddHose->currentIndex() - 1,
                    ui->End1Attachment_AddHose->text(),
                    ui->End2Attachment_AddHose->text(),
                    ui->ExpiryDate_AddHose->date(),
                    ui->HoseStatus_AddEnd->text(),
                    ui->HoseNotes_AddHose->toPlainText(),
                    ui->TemplateWP_AddHose->text().toInt(),
                    ui->TemplateTestPressure_AddHose->text().toInt(),
                    ui->CustomerIDNumber_AddHose->text());
        delete h;

        ui->StatusBar->setText("Status: Ready");
    }
    delete c;

   // qWarning() << "6";
}

void HoseTracker::on_HoseGoToTest_clicked(){
    // load the test hose page with this hose ID.

    // figure out how/if we need to send a multi-test
    loadPage(Pages::TestingCenter, m_IDPass, IDSendType::Hose, m_IDSequence);

}

void HoseTracker::loadHoseTestResult(CompanyID id){
    // load the test result of ID.
    // apparently this is superfluous.
}
