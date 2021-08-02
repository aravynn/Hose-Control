#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Functions for the adding/editing templates page.
 *
 */

void HoseTracker::loadAddEditTemplatePage(int id){
    // if there is no ID, then we'll simply clear all and do nothing.
    m_IDPass = id; // for the save or change function.
    m_IDSequence = 1;

    // first, clear all text fields.
    ui->HoseName_AddTemplate->setText("");
    ui->PartNumber_AddTemplate->setText("");
    ui->HoseManufacturer_AddTemplate->setText("");
    ui->HoseDescription_AddTemplate->setText("");
    ui->DistributorReference_AddTemplate->setText("");
    ui->ModelNumber_AddTemplate->setText("");
    ui->HoseInnerDiameter_AddTemplate->setText("");
    ui->OverallLength_AddTemplate->setText("");
    ui->CutLength_AddTemplate->setText("");
    ui->WorkingPressure_AddTemplate->setText("");
    ui->TestPressure_AddTemplate->setText("");
    ui->End1AttachMethod_AddTemplate->setText("");
    ui->End2AttachMethod_AddTemplate->setText("");
    ui->ImageLocation_AddTemplate->setText("");

    // clear the imaage
    ui->HoseImage_AddTemplate->clear();


    // next, clear/reset all other fields.
    ui->TestTimeMinutes_AddTemplate->setValue(0);
    ui->TestTimeSeconds_AddTemplate->setValue(0);
    ui->Notes_AddTemplate->setPlainText("");
//  ui->HoseImage_AddTemplate-> // WE'LL SETUP HOSE IMAGES LATER -----------------------------------------------------------------------!!


   // qWarning() << "Start Fitting Load";
   loadFittingDropdowns();
   // qWarning() << "End Fitting Load";

    // check if ID exists, if it does, load the appropriate information.
    if(id > -1){
      //  qWarning() << "Start Template Load";


        HoseTemplates * ht = new HoseTemplates(id);

       //  qWarning() << "Start Template Load 2";

        ui->HoseName_AddTemplate->setText(ht->GetName());
        ui->PartNumber_AddTemplate->setText(ht->GetPartNumber());
        ui->HoseManufacturer_AddTemplate->setText(ht->GetManufacturer());
        ui->HoseDescription_AddTemplate->setText(ht->GetDescription());
        ui->DistributorReference_AddTemplate->setText(ht->GetDistRefNumber());
        ui->ModelNumber_AddTemplate->setText(ht->GetPartNumber());
        ui->HoseInnerDiameter_AddTemplate->setText(ht->GetInnerDiameter());
        ui->OverallLength_AddTemplate->setText(ht->GetOverallLength());
        ui->CutLength_AddTemplate->setText(ht->GetCutLength());
        ui->WorkingPressure_AddTemplate->setText(QString::number(ht->GetWP()));
        ui->TestPressure_AddTemplate->setText(QString::number(ht->getTestPressure()));
        ui->End1AttachMethod_AddTemplate->setText(ht->GetEnd1()->GetAttachMethod());
        ui->End2AttachMethod_AddTemplate->setText(ht->GetEnd2()->GetAttachMethod());
        ui->ImageLocation_AddTemplate->setText(ht->GetImageLocation());

      //  qWarning() << "Start Template Load 3";

        int totalTime = ht->getTestTime();

        int minutes = floor(totalTime / 60); // floor value is minutes.
        int seconds = totalTime % 60; // remainder is seconds.


        ui->TestTimeMinutes_AddTemplate->setValue(minutes);
        ui->TestTimeSeconds_AddTemplate->setValue(seconds);

        ui->Notes_AddTemplate->setPlainText(ht->GetNotes());
    //  ui->HoseImage_AddTemplate-> // WE'LL SETUP HOSE IMAGES LATER -----------------------------------------------------------------------!!



        ui->CouplingEnd1_AddTemplate->setCurrentIndex(ht->GetEnd1()->GetID() + 1);
        ui->CouplingEnd2_AddTemplate->setCurrentIndex(ht->GetEnd2()->GetID() + 1);


        QString directory = ht->GetImageLocation();

        if(directory != ""){
            ui->ImageLocation_AddTemplate->setText(directory);

            QPixmap pix(directory);

            ui->HoseImage_AddTemplate->setPixmap(pix.scaledToWidth(120));
        } else {
            ui->HoseImage_AddTemplate->clear();
        }

        delete ht;
    }
}

// ADD AN "on change" function for when the coupling end  is changed, update attachment method
void HoseTracker::on_CouplingTemplate_Change(int c){
    // take the indexes of the couplings and update the attachment methods
   // qWarning() << "Change crimps";

    Q_UNUSED(c);

    if(ui->CouplingEnd1_AddTemplate->count() < 2 || ui->CouplingEnd2_AddTemplate->count() < 2){
        return; // there is no fittings to work with.
    }

    int ida = ui->CouplingEnd1_AddTemplate->currentIndex() - 1;
    int idb = ui->CouplingEnd2_AddTemplate->currentIndex() - 1;



    if(ida < 0){
        ida = 0;
    }

    if(idb < 0){
        idb = 0;
    }

    //qWarning() << QString::number(ida) << QString::number(idb);

    FittingTemplate *fta = new FittingTemplate(ida);
    FittingTemplate *ftb = new FittingTemplate(idb);

    ui->End1AttachMethod_AddTemplate->setText(fta->GetAttachMethod());
    ui->End2AttachMethod_AddTemplate->setText(ftb->GetAttachMethod());

    delete fta;
    delete ftb;
}

void HoseTracker::on_SaveChangesTemplate_clicked(){
    // take the saved changes and either add or update the hose.
   // qWarning() << "Save Changes - Not Complete.";

    if(ui->CouplingEnd1_AddTemplate->currentIndex() < 1 || ui->CouplingEnd2_AddTemplate->currentIndex() < 1){
        // no fitting
        ui->StatusBar->setText("Error: Please select ends");
        return;
    }

    if(m_IDPass > -1){
        //Update hose, this is pre-existing.
        HoseTemplates *ht = new HoseTemplates(m_IDPass);

        int testtime = ui->TestTimeMinutes_AddTemplate->text().toInt() * 60 + ui->TestTimeSeconds_AddTemplate->text().toInt();

        ui->StatusBar->setText("Status: Updating");
        ht->UpdateTemplate(
                    ui->PartNumber_AddTemplate->text(),
                    ui->HoseName_AddTemplate->text(),
                    ui->HoseManufacturer_AddTemplate->text(),
                    ui->HoseDescription_AddTemplate->text(),
                    ui->DistributorReference_AddTemplate->text(),
                    ui->HoseInnerDiameter_AddTemplate->text(),
                    ui->OverallLength_AddTemplate->text(),
                    ui->CutLength_AddTemplate->text(),
                    ui->WorkingPressure_AddTemplate->text().toInt(),
                    ui->TestPressure_AddTemplate->text().toInt(),
                    testtime,
                    ui->CouplingEnd1_AddTemplate->currentIndex() - 1,
                    ui->CouplingEnd2_AddTemplate->currentIndex() - 1,
                    ui->Notes_AddTemplate->toPlainText());

        ui->StatusBar->setText("Status: Updating Image");

        if(ui->ImageLocation_AddTemplate->text() != ""){
            ht->UpdateImage(ui->ImageLocation_AddTemplate->text());
        }

        ui->StatusBar->setText("Status: Ready");

        delete ht;
    } else {
        // Save as a new hose template

        int testtime = ui->TestTimeMinutes_AddTemplate->text().toInt() * 60 + ui->TestTimeSeconds_AddTemplate->text().toInt();

       // qWarning() << "time is: " << QString::number(testtime);
        ui->StatusBar->setText("Status: Updating");
        HoseTemplates *ht = new HoseTemplates(
                    ui->PartNumber_AddTemplate->text(),
                    ui->HoseName_AddTemplate->text(),
                    ui->HoseManufacturer_AddTemplate->text(),
                    ui->HoseDescription_AddTemplate->text(),
                    ui->DistributorReference_AddTemplate->text(),
                    ui->HoseInnerDiameter_AddTemplate->text(),
                    ui->OverallLength_AddTemplate->text(),
                    ui->CutLength_AddTemplate->text(),
                    ui->WorkingPressure_AddTemplate->text().toInt(),
                    ui->TestPressure_AddTemplate->text().toInt(),
                    testtime,
                    ui->CouplingEnd1_AddTemplate->currentIndex() - 1,
                    ui->CouplingEnd2_AddTemplate->currentIndex() - 1,
                    ui->ImageLocation_AddTemplate->text(),
                    ui->Notes_AddTemplate->toPlainText());
                    // get the image and enter it on second last line. --------------------------------------------------------!!

        // ID will be saved in case we are doing a "create hose" action.
        m_IDPass = (int)ht->getID();

        // resource cleanup. Man I hope I catch it all.
        delete ht;

        ui->StatusBar->setText("Status: Ready");
    }
}

void HoseTracker::on_CreateHoseTemplate_clicked(){
    // save changes, then send to the add/edit hose page.

    // first, save the data for the test
    on_SaveChangesTemplate_clicked();

    // second, load the create hose page.
    loadPage(Pages::AddHose, m_IDPass, IDSendType::HoseTemplate);

}

void HoseTracker::on_AddEnd_clicked(){
    // load the add hose end page, also, keep the ID of the last template so we can reload it.
    //qWarning() << "Add End, not complete.";
    loadPage(Pages::AddFittingTemplate);

}

void HoseTracker::loadFittingDropdowns(){
    // load the fitting dropdowns for ALL pages.
    // get the list of options for the dropdowns, and spit them out.
    std::vector<QString> getColumns{"PK"};
    QString table = "FittingTemplates";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""}; // nothing.

    // get the most recent hoses created.
    StrPair data = db->selectAction(getColumns, table, filter);

    int dataSize = data.size();

    // clear datasize if nothing returned.
    if(data.at(0).first == "")
        dataSize = 0;

    ui->CouplingEnd1_AddTemplate->clear();
    ui->CouplingEnd2_AddTemplate->clear();
    ui->End1Coupling_AddHose->clear();
    ui->End2Coupling_AddHose->clear();

    ui->CouplingEnd1_AddTemplate->addItem("- Choose Fitting -");
    ui->CouplingEnd2_AddTemplate->addItem("- Choose Fitting -");
    ui->End1Coupling_AddHose->addItem("- Choose Fitting -");
    ui->End2Coupling_AddHose->addItem("- Choose Fitting -");

    for(int i{0}; i < dataSize; ++i){
        // set the dropdown values.
        FittingTemplate *ft = new FittingTemplate(data.at(i).second.toInt());
        ui->CouplingEnd1_AddTemplate->addItem(ft->GetName());
        ui->CouplingEnd2_AddTemplate->addItem(ft->GetName());
        ui->End1Coupling_AddHose->addItem(ft->GetName());
        ui->End2Coupling_AddHose->addItem(ft->GetName());
        delete ft;
    }
}

void HoseTracker::on_FindImage_clicked(){
    // open a file dialog and get the file. return to the waiting label area.
    //qWarning() << "Find Image Clicked";

    QString directory = QFileDialog::getOpenFileName(this, tr("Open File"), "/documents", tr("Images (*.bmp *.gif *.jpg *.png)"));

    if(!directory.isEmpty()){
       // qWarning() << directory; we have an image address, so load that image into the graphics view and save the location to the label.
        ui->ImageLocation_AddTemplate->setText(directory);

        QPixmap pix(directory);
        ui->HoseImage_AddTemplate->setPixmap(pix.scaledToHeight(80));



    }
}
