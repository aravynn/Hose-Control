#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Loading and click functions for testing center for hose.
 *
 */

// thread here that will load test data concurrently.
void loadDataThread(Ui::HoseTracker *ui, HoseTest *ht, bool *release, int testtime){
    // load the thread data and update based on the hose test.

    int tn = -1;
    int tc = 0;

    while(*release == false){
        // load the information into the UI from the test.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        ui->PressureRating_TestCenter->setText( QString::number( ht->getPressureAtID( ht->getTestLength() - 1 ) ) );
        ui->TimeRemaining_TestCenter->setText( QString::number( ht->getTestLength() ) );

      //  qWarning() << "Good Reads: " << QString::number(ht->getGoodReadTime());

        if(tn == ht->getGoodReadTime() && ht->getGoodReadTime() >= testtime){


             // repeat of the same number.
             ++tc;
            if(tc > 3){
                // 5 consecutive seconds means the test must have finished
                *release = true;
              //  qWarning() << "released";
            }
        } else {
            // new number.
            tn = ht->getGoodReadTime();
            tc = 1;
           //  qWarning() << "new NO " << QString::number(tn);
        }
        // reload the graph.


        //qWarning() << "update the graph here, line 38";
        //graph->UpdateView();
        ui->TestGraph_TestCenter->itemAt(0)->widget()->update();
    }
    ui->PressureRating_TestCenter->setText("--");
    ui->TimeRemaining_TestCenter->setText("0");
    if(tc > 3){
        ui->StartButton_TestCenter->click();
    }
  //  qWarning() << "Tracker Complete";

}

void HoseTracker::loadHoseTester(IDType id, int idCount){
    // load the given hose ID's starting from ID up to ID Count. All hoses need to be tested simultaneously.

    m_IDPass = id; // for the ID to use.
    m_IDSequence = idCount; // for how many to test.  - SET UP MULTITEST

    tableClear();

    m_ht = new HoseTest(); // load the hose test. BE SURE TO DELETE THIS BEFORE LEAVING PAGE. ALSO, STOP THE TEST!!!
    m_htExist = true;

    Hose *h = new Hose(id); // load hose 1, since they all need to be the same regardless.

    // load the live graph.
   // qWarning() << "load the graph here, line 66";
    m_graph = new GraphView(m_ht);
    m_graphExist = true; // since it now exists.
    //qWarning() << " place graph";
    ui->TestGraph_TestCenter->addWidget(m_graph);
    //qWarning() << "Graph Complete";

    // Reset all fields.
    ui->PressureRating_TestCenter->setText("--"); // the active pressure in the system.
    ui->MinPressure_TestCenter->setText("0 PSI");
    ui->MaxPressure_TestCenter->setText("0 PSI");
    ui->TargetLoad_TestCenter->setText("");
    ui->PeakLoad_TestCenter->setText("");

    ui->CoverLeaks_TestCenter->setChecked(false);
    ui->CoverDelaminating_TestCenter->setChecked(false);
    ui->CoverWear_TestCenter->setChecked(false);
    ui->CoverGouges_TestCenter->setChecked(false);
    ui->CoverRot_TestCenter->setChecked(false);
    ui->CoverWires_TestCenter->setChecked(false);
    ui->CoverDents_TestCenter->setChecked(false);
    ui->CoverColor_TestCenter->setChecked(false);

    ui->FittingLeaks_TestCenter->setChecked(false);
    ui->FittingCracked_TestCenter->setChecked(false);
    ui->FittingCam_TestCenter->setChecked(false);
    ui->FittingThread_TestCenter->setChecked(false);

    ui->FerruleClamped_TestCenter->setChecked(false);
    ui->FerruleAlignment_TestCenter->setChecked(false);
    ui->FerruleCracked_TestCenter->setChecked(false);

    ui->TubeLamination_TestCenter->setChecked(false);
    ui->TubeColor_TestCenter->setChecked(false);
    ui->TubeCracked_TestCenter->setChecked(false);

    ui->PassPrint_TestCenter->setEnabled(false);
    ui->FailPrint_TestCenter->setEnabled(false);

    ui->PeakLoad_TestCenter->setText("");
    ui->TestTimeMinutes_TestCenter->setValue(0);
    ui->TestTimeSeconds_TestCenter->setValue(0);
    ui->TestNumber_TestCenter->setText(""); // procedural somehow?
    ui->Elongation_TestCenter->setText("");
    ui->Connectivity_TestCenter->setCurrentIndex(0);
    ui->Comments_TestCenter->setText("");
    ui->ProofTestType_TestCenter->setText("");
    ui->OrderNumber_TestCenter->setText("");

    ui->ImageNames_TestCenter->setText("");

    ui->SensorType_TestCenter->setCurrentIndex(0);
    ui->FailPrint_TestCenter->setEnabled(false);

    QDate today;
    today = today.currentDate();

    ui->CurrentDate_TestCenter->setDate(today); // cleared for simplicity, will set default to today.
    ui->EnteredBy_TestCenter->setText("");
    //loadOwnerDropdowns(); replacing with a label.
    ui->HoseOwner_TestCenter->setText("");

    // output whatever information is given by the hose into the hose test form.
    // should be able to set

    ui->MinPressure_TestCenter->setText(QString::number(h->getTestPressure()) + " PSI");

    int max = h->getTestPressure() * 1.1;
    ui->MaxPressure_TestCenter->setText(QString::number(max) + " PSI");
    ui->TargetLoad_TestCenter->setText(QString::number(h->getTestPressure()));
    ui->PeakLoad_TestCenter->setText(QString::number(max));

    int totalTime = h->getTestTime();

    int minutes = floor(totalTime / 60); // floor value is minutes.
    int seconds = totalTime % 60; // remainder is seconds.

//    qWarning() << QString::number(minutes) << QString::number(seconds) << QString::number(totalTime);
    ui->TimeRemaining_TestCenter->setText(QString::number(totalTime));
    ui->TestTimeMinutes_TestCenter->setValue(minutes);
    ui->TestTimeSeconds_TestCenter->setValue(seconds);

    ui->TestNumber_TestCenter->setText(makeTestNumber(db->getNewPK("HoseTests")));

    int ptt = (h->getTestPressure() * 100) / h->GetWP();

    ui->ProofTestType_TestCenter->setText(QString::number(static_cast<double>(ptt / 100.0)) + "x WP");

    ui->HoseOwner_TestCenter->setText(h->getOwnerName());

   // m_TestImages.clear(); // clear any hose images that mght have been saved.

    delete h;

  //  qWarning() << "Load Hose Test " << QString::number(m_IDPass) << QString::number(m_IDSequence);
}

QString HoseTracker::makeTestNumber(CompanyID testid){
    // create the MD5 hose test number in format:
    //ffff-ffff-ffffffff
    /*
     QByteArray hash = QCryptographicHash::hash(QString::number(testid).toLocal8Bit(), QCryptographicHash::Md5).toHex();

    int size = hash.length();
    std::stringstream ss;
    //for(int i{0}; i < size; ++i){
    //    ss << hash[i];
    //}

    ss << hash[0] << hash[1] << hash[2] << hash[3] << "-";
    ss << hash[4] << hash[5] << hash[6] << hash[7] << "-";
    ss << hash[size - 8] << hash[size - 7] << hash[size - 6] << hash[size - 5]<< hash[size - 4] << hash[size - 3] << hash[size - 2] << hash[size - 1];

    QString i = QString::fromStdString(ss.str());
    */

    // cool as this testID system is, jeff would like this to work as a standardized number plus 1000. thats too easy.
    return QString::number(testid + 1000);
}

// slot functions for all buttons etc.

void HoseTracker::on_TargetLoad_changed(QString s){
    // update target load number for the window UI
    ui->MinPressure_TestCenter->setText(s + " PSI");
}
void HoseTracker::on_PeakLoad_changed(QString s){
    //update peak load for the UI
    ui->MaxPressure_TestCenter->setText(s + " PSI");
}

void HoseTracker::on_TestTime_changed(int t){
   // int seconds = ui->TestTimeMinutes_TestCenter->value() * 60 + ui->TestTimeSeconds_TestCenter->value();
    Q_UNUSED(t);

    // force a reset of time remaining when we alter the testtime.
    ui->TimeRemaining_TestCenter->setText("0");
}

void HoseTracker::on_FailPrint_clicked(){
    // this is a failed test, so end the existing test and save the result to the DB

   // qWarning() << "fail Print start";
    // stop the test, if active.
    if(*m_lock == false){
        // test is still active. Stop it first.
        ui->StartButton_TestCenter->click();
    }

    ui->StatusBar->setText("Status: Saving");

    Hose *h = new Hose(m_IDPass);

    // save the test results.
    m_ht->UpdateTest(
                h->getOwnerID(),
                ui->OrderNumber_TestCenter->text(),
                ui->CurrentDate_TestCenter->date(),
                ui->ProofTestType_TestCenter->text(),
                ui->Elongation_TestCenter->text(),
                ui->TestNumber_TestCenter->text(),
                ui->EnteredBy_TestCenter->text(),
                "FAIL",
                ui->Connectivity_TestCenter->currentText(),
                ui->Comments_TestCenter->text(),
                m_ht->VisualCoverResult(ui->CoverLeaks_TestCenter->isChecked(), ui->CoverDelaminating_TestCenter->isChecked(), ui->CoverWear_TestCenter->isChecked(),
                                        ui->CoverGouges_TestCenter->isChecked(), ui->CoverRot_TestCenter->isChecked(), ui->CoverWires_TestCenter->isChecked(),
                                        ui->CoverDents_TestCenter->isChecked(), ui->CoverColor_TestCenter->isChecked()),
                m_ht->VisualTubeResult(ui->TubeLamination_TestCenter->isChecked(), ui->TubeColor_TestCenter->isChecked(), ui->TubeCracked_TestCenter->isChecked()),
                m_ht->VisualFittingResult(ui->FittingLeaks_TestCenter->isChecked(), ui->FittingCracked_TestCenter->isChecked(), ui->FittingCam_TestCenter->isChecked(),
                                          ui->FittingThread_TestCenter->isChecked()),
                m_ht->VisualFerruleResult(ui->FerruleClamped_TestCenter->isChecked(), ui->FerruleAlignment_TestCenter->isChecked(), ui->FerruleCracked_TestCenter->isChecked()),
                static_cast<TestType>(ui->SensorType_TestCenter->currentIndex())
                );

    // load the result page.
    CompanyID newID = m_ht->getID();

    if(m_htExist){
        delete m_ht;
        m_htExist = false;
    }

    delete h;

    ui->StatusBar->setText("Status: Ready");

    loadPage(Pages::TestResults, newID, IDSendType::HoseTest, m_IDSequence);
   // std::this_thread::sleep_for(std::chrono::seconds(3));
    // clean resources
    //delete m_ht;

   // qWarning() << "Fail Print clicked";
}

void HoseTracker::on_PassPrint_clicked(){
    // this is a passed test, save the result to the DB

    //qWarning() << "Pass Print start";
    // stop the test, if active.
    if(*m_lock == false){
        // test is still active. Stop it first.
        ui->StartButton_TestCenter->click();
    }

    ui->StatusBar->setText("Status: Saving");

    Hose *h = new Hose(m_IDPass);

    // save the test results.
    m_ht->UpdateTest(
                h->getOwnerID(),
                ui->OrderNumber_TestCenter->text(),
                ui->CurrentDate_TestCenter->date(),
                ui->ProofTestType_TestCenter->text(),
                ui->Elongation_TestCenter->text(),
                ui->TestNumber_TestCenter->text(),
                ui->EnteredBy_TestCenter->text(),
                "PASS",
                ui->Connectivity_TestCenter->currentText(),
                ui->Comments_TestCenter->text(),
                m_ht->VisualCoverResult(ui->CoverLeaks_TestCenter->isChecked(), ui->CoverDelaminating_TestCenter->isChecked(), ui->CoverWear_TestCenter->isChecked(),
                                        ui->CoverGouges_TestCenter->isChecked(), ui->CoverRot_TestCenter->isChecked(), ui->CoverWires_TestCenter->isChecked(),
                                        ui->CoverDents_TestCenter->isChecked(), ui->CoverColor_TestCenter->isChecked()),
                m_ht->VisualTubeResult(ui->TubeLamination_TestCenter->isChecked(), ui->TubeColor_TestCenter->isChecked(), ui->TubeCracked_TestCenter->isChecked()),
                m_ht->VisualFittingResult(ui->FittingLeaks_TestCenter->isChecked(), ui->FittingCracked_TestCenter->isChecked(), ui->FittingCam_TestCenter->isChecked(),
                                          ui->FittingThread_TestCenter->isChecked()),
                m_ht->VisualFerruleResult(ui->FerruleClamped_TestCenter->isChecked(), ui->FerruleAlignment_TestCenter->isChecked(), ui->FerruleCracked_TestCenter->isChecked()),
                static_cast<TestType>(ui->SensorType_TestCenter->currentIndex())
                );

    // load the result page.

   CompanyID newID = m_ht->getID();

   if(m_htExist){
       delete m_ht;
       m_htExist = false;
   }

   delete h;

   ui->StatusBar->setText("Status: Ready");

   loadPage(Pages::TestResults, newID, IDSendType::HoseTest, m_IDSequence);
   // std::this_thread::sleep_for(std::chrono::seconds(3));
    // clean resources


}

void HoseTracker::on_StartButton_clicked(){
     // check to see if this is a START or STOP function.
    // starts the current test using exsiting parameters.

    if(m_ht->GetActive()){
        // this test is actively occuring. stop processes.
        //   m_graph->deleteLater();
        //   m_graphExist = false; // remove the graph, its useless now.

       ui->StatusBar->setText("Status: Ready");

        m_ht->StopTest();
        ui->StartButton_TestCenter->setText("Start");
        *m_lock = true;
    //    qWarning() << "Stop button clicked";
    } else {
        // this is needs to start.
        *m_lock = false;
        m_ht->ResetTest();
        m_ht->StartTest(
                    m_IDPass,
                    m_IDSequence,
                    ui->TestTimeMinutes_TestCenter->value() * 60 + ui->TestTimeSeconds_TestCenter->value(),
                    ui->TargetLoad_TestCenter->text().toInt(),
                    ui->PeakLoad_TestCenter->text().toInt(),
                    0);

        ui->StatusBar->setText("Status: Test Active");

        std::thread th2( loadDataThread, ui, m_ht, m_lock, ui->TestTimeMinutes_TestCenter->value() * 60 + ui->TestTimeSeconds_TestCenter->value() );
        th2.detach(); // this prevents it screwing up in the background. Can continue running freely without destroying resources.
      //  qWarning() << "Thread Detached";


        // We'll load the 0 sensor only for now, until multi-sensor setup is configured.
        ui->StartButton_TestCenter->setText("Stop");
      //  qWarning() << "Start Button Clicked";

    }

    // lock all fields, or unlock
    ui->TargetLoad_TestCenter->setEnabled(*m_lock);
    ui->PeakLoad_TestCenter->setEnabled(*m_lock);
    ui->CoverLeaks_TestCenter->setEnabled(*m_lock);
    ui->CoverDelaminating_TestCenter->setEnabled(*m_lock);
    ui->CoverWear_TestCenter->setEnabled(*m_lock);
    ui->CoverGouges_TestCenter->setEnabled(*m_lock);
    ui->CoverRot_TestCenter->setEnabled(*m_lock);
    ui->CoverWires_TestCenter->setEnabled(*m_lock);
    ui->CoverDents_TestCenter->setEnabled(*m_lock);
    ui->CoverColor_TestCenter->setEnabled(*m_lock);
    ui->FittingLeaks_TestCenter->setEnabled(*m_lock);
    ui->FittingCracked_TestCenter->setEnabled(*m_lock);
    ui->FittingCam_TestCenter->setEnabled(*m_lock);
    ui->FittingThread_TestCenter->setEnabled(*m_lock);
    ui->FerruleClamped_TestCenter->setEnabled(*m_lock);
    ui->FerruleAlignment_TestCenter->setEnabled(*m_lock);
    ui->FerruleCracked_TestCenter->setEnabled(*m_lock);
    ui->TubeLamination_TestCenter->setEnabled(*m_lock);
    ui->TubeColor_TestCenter->setEnabled(*m_lock);
    ui->TubeCracked_TestCenter->setEnabled(*m_lock);
    ui->TestTimeMinutes_TestCenter->setEnabled(*m_lock);
    ui->TestTimeSeconds_TestCenter->setEnabled(*m_lock);
    ui->TestNumber_TestCenter->setEnabled(*m_lock);
    ui->Elongation_TestCenter->setEnabled(*m_lock);
    ui->Connectivity_TestCenter->setEnabled(*m_lock);
    ui->Comments_TestCenter->setEnabled(*m_lock);
    ui->ProofTestType_TestCenter->setEnabled(*m_lock);
    ui->OrderNumber_TestCenter->setEnabled(*m_lock);
    ui->CurrentDate_TestCenter->setEnabled(*m_lock);
    ui->EnteredBy_TestCenter->setEnabled(*m_lock);
    ui->PassPrint_TestCenter->setEnabled(*m_lock);
    ui->FailPrint_TestCenter->setEnabled(*m_lock);
    ui->GoToHome_TestCenter->setEnabled(*m_lock);
    ui->GoBack_TestCenter->setEnabled(*m_lock);
    ui->AddImage_TestCenter->setEnabled(*m_lock);
}

void HoseTracker::updateGraph(){
   // m_graph->UpdateView();
}

void HoseTracker::on_FindDamageImage_clicked(){
    // open a file dialog and get the file. return to the waiting label area.

    QString directory = QFileDialog::getOpenFileName(this, tr("Open File"), "/documents", tr("Images (*.bmp *.gif *.jpg *.png)"));

    if(!directory.isEmpty()){
        QString testImage = m_ht->AddTestImage(directory, ui->HoseOwner_TestCenter->text());

        if(testImage != ""){
            if(ui->ImageNames_TestCenter->text() == ""){
                ui->ImageNames_TestCenter->setText(testImage);
            } else {
                ui->ImageNames_TestCenter->setText(ui->ImageNames_TestCenter->text() + "\n" + testImage);
            }
        }

    }
}

void HoseTracker::on_TestType_changed(int type){
    //qWarning() << "Changed ID";
    // change the test type for other options for hose tests.
    // currently I beleive that only Sensor tests and Rejected tests work, however other options may be
    // in working condition, testing is required to determine.


    if(static_cast<TestType>(type) == TestType::REJECTED){
        on_StartButton_clicked();
        on_StartButton_clicked();

        ui->FailPrint_TestCenter->setEnabled(true);
    } else {
        ui->FailPrint_TestCenter->setEnabled(*m_lock);
    }
}
