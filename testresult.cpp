#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Test Results page related functions.
 *
 *
 */

void HoseTracker::loadTestResults(IDType id, int count, int offset){
    // load the test results of the associated test.
    // typically called on pass or fail.

    // set default values.
    m_IDPass = id;
    m_IDSequence = count;

    tableClear(); // clear all tables so we can set up test results.

    // load resource pointers for the test, company and hose. Will only load the first of each.
    m_ht = new HoseTest(id); // load the first test, the rest are technically the same.
    m_htExist = true;
    Company *c = new Company(m_ht->getOwnerID());
    Hose *h = new Hose(m_ht->getHoseID());

    // assign all visible elements of the page.
    ui->HoseID_TestResults->setText(h->getAssetNumber());
    ui->HoseOwner_TestResults->setText(c->getName());
    ui->TestResult_TestResults->setText(m_ht->getTestResult());
    ui->HoseLocation_TestResults->setText((h->getLocation().Name != "") ? h->getLocation().Name : h->getLocation().Addr1);

    // load the labels for all testing data.
    int testcount = m_ht->getTestLength();

    for(int i{offset}; (i < testcount && i < m_TableLimit + offset); ++i){
        // for each element of test length, output 3 labels - Second, Pressure, and Temperature.
        m_TableLabels[m_LabelsExist] = new QLabel();
        m_TableLabels[m_LabelsExist + 1] = new QLabel();
        m_TableLabels[m_LabelsExist + 2] = new QLabel();

        m_TableLabels[m_LabelsExist]->setText(QString::number(i)); // time in seconds.
        m_TableLabels[m_LabelsExist + 1]->setText(QString::number(m_ht->getPressureAtID(i))); // pressure rating (RAW)
        m_TableLabels[m_LabelsExist + 2]->setText(QString::number(m_ht->getTemperatureAtID(i))); // temperature RAW

        ui->TestData_TestResults->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
        ui->TestData_TestResults->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
        ui->TestData_TestResults->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);

        // iterate counters
        m_LabelsExist += 3;
        ++m_TableLines;

    }

    int totalPages = testcount / m_TableLimit;
    if(testcount % m_TableLimit > 0) totalPages++; // increment for last page.
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             m_TableButtons[m_ButtonsExist]->setMaximumWidth(20);
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentRaw_TestResults->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                loadTestResults(id, count, i * m_TableLimit);
             });

             ++m_ButtonsExist;
        }
    }

    // draw the graph.

    //m_ht = new HoseTest(id); // load the last test
    //qWarning() << "Load the Graph Here";
    m_graph = new GraphView(m_ht);
    m_graphExist = true;
    ui->TestGraph_TestResults->addWidget(m_graph);

    // clean up resources.
    //delete ht; // we're using a universal definer here, we need to delete somehow.
    delete c;
    delete h;
    //qWarning() << "Test Center Incomplete" << QString::number(id) << QString::number(count);
}

void HoseTracker::on_RetestHose_clicked(){
    // go back to hose test.
   // HoseTest *ht = new HoseTest(m_IDPass);
    CompanyID hoseid = m_ht->getHoseID();

    delete m_ht; // must be done BEFORE loading new page.
    m_htExist = false;
   // qWarning() << "load retest page from ID " << QString::number(hoseid);

    loadPage(Pages::TestingCenter, hoseid, IDSendType::Hose, 1); // send back a single hose only, not for multitesting purposes.
   // qWarning() << "retest loaded successfully.";
}

void HoseTracker::on_ViewPDF_clicked(){

    ui->StatusBar->setText("Status: Creating PDF");

    // create and view the PDF.
    PDFCreator p{m_IDPass}; // pdf for the given ID.

    if(!p.PDFExists()){
        p.MakePDF();
    }
    QDesktopServices::openUrl(QUrl(p.GetPDFPath()));

    ui->StatusBar->setText("Status: Ready");
}



