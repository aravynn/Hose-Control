#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * page specifically for loading/using home page functions.
 *
 */

void HoseTracker::loadRecentHoses(int offset){
    // load the recent hoses loaded onto the site.

    //  ui->RecentHosesArea_Home->addItem()
    /*
     * int m_LinesPerTable = 0; // limit of CONST items to load per table.
    QPushButton * m_TableButtons[MaximumTableLines]; // adjust in trackerstructs if needed.
    QLabel * m_TableCustomerName[MaximumTableLines];
    QLabel * m_TableHoseType[MaximumTableLines];
    QLabel * m_TableHoseID[MaximumTableLines];
    QLabel * m_TableDate[MaximumTableLines];
    QLabel * m_TableStatus[MaximumTableLines];*/

    // first get the hose Test data so we can load the recent information.
   // HoseTest *ht = new HoseTest();

    std::vector<QString> columns{"PK"};
    QString table = "Hoses";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""}; // nothing.
    int limit = m_TableLimit;
    //int offset = 0;
    bool desc = true;

    StrPair datalen = db->selectAction(columns, table, filter);

    int totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of

    if( datalen.size() % m_TableLimit > 0 ) totalPages++;

    //qWarning() << "one";

    StrPair data = db->selectAction(columns, table, filter, limit, offset, desc);


   // qWarning() << QString::number(data.size());

    // qWarning() << "two";

    int dataSize = data.size();

    // check if no data present, force to 0
    if(data.at(0).first == "")
           dataSize = 0;

        // take each option and output the data to qWarning for now.
    for(int i{0}; i < dataSize; ++i){
      //qWarning() << data.at(i).first << data.at(i).second;
      // using given PK, load the various hose tests so we can output the table.
       //HoseTest *ht = new HoseTest((CompanyID)data.at(i).second.toInt());
       //Company *c = new Company(ht->getOwnerID());
       Hose *h = new Hose((CompanyID)data.at(i).second.toInt());

       int HoseID = (CompanyID)data.at(i).second.toInt();

       // out put all data as required.
       m_TableLabels[m_LabelsExist] = new QLabel();
       m_TableLabels[m_LabelsExist + 1] = new QLabel();
       m_TableLabels[m_LabelsExist + 2] = new QLabel();
       m_TableLabels[m_LabelsExist + 3] = new QLabel();
       //m_TableLabels[m_LabelsExist + 4] = new QLabel();
       m_TableButtons[m_ButtonsExist] = new QPushButton();

       m_TableLabels[m_LabelsExist]->setText(h->getOwnerName());
       m_TableLabels[m_LabelsExist + 1]->setText(h->GetName());
       m_TableLabels[m_LabelsExist + 2]->setText("ID# " + h->getAssetNumber());
       //m_TableLabels[m_LabelsExist + 3]->setText(ht->getDate().toString("MM/dd/yyyy"));
       m_TableLabels[m_LabelsExist + 3]->setText(h->getStatus());
       m_TableButtons[m_ButtonsExist]->setText("View Hose");

       ui->RecentHosesArea_Home->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
       ui->RecentHosesArea_Home->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
       ui->RecentHosesArea_Home->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);
       ui->RecentHosesArea_Home->addWidget(m_TableLabels[m_LabelsExist + 3], m_TableLines + 1, 4);
      // ui->RecentHosesArea_Home->addWidget(m_TableLabels[m_LabelsExist + 4], m_TableLines + 1, 5);
       ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);


       //qWarning() << "Hose ID" << QString::number(ht->getHoseID());

       connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
          loadPage(Pages::AddHose, HoseID, IDSendType::Hose);
       });


       // link all buttons and add the data transfer requiremnts.
      // connect(m_TableButtons[m_LinesPerTable], SIGNAL(click()), this, SLOT()); ----------------------------------------------------!!

       // at last, iterate the counter
       ++m_ButtonsExist;
       m_LabelsExist+=4;
       ++m_TableLines;
       // clean up each load.
      // delete ht;
       //delete c;
       delete h;
    }

    // use the totalPages value to create buttons.
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentHoses_HomePage->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                loadRecentHoses(i * m_TableLimit);
             });

             ++m_ButtonsExist;
        }
    }

}

void HoseTracker::loadHomePage(){
    // load the widgets etc on the home page. This is a fairly static loader.
    m_IDPass = -1;
    loadRecentHoses();
    loadSyncResults();
    // load sync time here.
}

void HoseTracker::on_Sync_clicked(){
    // load the SYNC program, allow it to run sync protocol.
    /*
    qWarning() << "Load the sync program";

    // load the sync application into the main window. C:/Users/aravy/Documents/Hosetracker/Sync/Database-Sync.exe

    QString CMD = "cmd";
    QString file = QDir::homePath() + QString("/Documents/Hosetracker/Sync/Database-Sync.exe");

    QStringList args;

    args << file;

    if(!QProcess::startDetached(CMD, args)){
        //qWarning() << "ERROR: failed to start program at " << file;
        ui->StatusBar->setText("ERROR: failed to start program at " + file);
    }
    */

   // QString file = QDir::homePath() + QString("/Documents/Hosetracker/Sync/Database-Sync.exe");

    //ShellExecute(nullptr, nullptr, file.toStdWString().c_str(), nullptr, nullptr, 5 );

}

void HoseTracker::loadSyncResults(){
    // load the sync results into the SyncTimestamp_Hose textbox.

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"", ""};

    StrPair Ret = db->selectAction({"PK"}, "DataSync", filter);

    if (Ret.size() == 1){
        if(Ret.at(0).first == ""){
            ui->SyncTimestamp_Home->setText("No lines to sync");
        } else {
            ui->SyncTimestamp_Home->setText("1 line to sync");
        }
    } else {
        ui->SyncTimestamp_Home->setText(QString::number(Ret.size()) + " lines to sync");
    }



}
