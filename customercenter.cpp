#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Customer Center page functions, for loading data, will need some flexibilty as it has a search function.
 *
 */

void HoseTracker::loadCustomerCenter(QString SearchTerm, int offset){
    // load the hose center information, search results etc. all in one.

  //  qWarning() << "1";

    m_IDPass = -1;
    m_IDSequence = 1;

    ui->SearchBar_CustomerC->setText(SearchTerm);

   // qWarning() << "2";

    std::vector<QString> getColumns{"PK"};
    QString table = "Companies";

    StrPair data; // defined for future use.

    int totalPages = 0;

   // qWarning() << "3";

    if(SearchTerm != ""){
        // there is a search term, as we'll use it to get the hoses.

//        qWarning() << "4";

   //     qWarning() << SearchTerm;
        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);
        filter.at(0) = {"",""};

        std::vector<QString> searchColumns{"CompanyName", "Notes"};

     //   qWarning() << "5";

        StrPair datalen = db->searchAction(getColumns, searchColumns, table, "%" + SearchTerm + "%", filter);
        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        data = db->searchAction(getColumns, searchColumns, table, "%" + SearchTerm + "%", filter, m_TableLimit, offset);
     //   qWarning() << "6";
    } else {

      //     qWarning() << "7";
    //    qWarning() << "No Term Given";
        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);
        filter.at(0) = {"", ""}; // nothing.

        StrPair datalen = db->selectAction(getColumns, table, filter);

    //    qWarning() << "8";

        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        // get the most recent hoses created.
        data = db->selectAction(getColumns, table, filter, m_TableLimit, offset, true);

    //    qWarning() << "9";
    }

    int dataSize = data.size();

  //  qWarning() << "10";

    for(int i{0}; i < dataSize; ++i){

        if(data.at(i).first == "")
            continue;

   //     qWarning() << "a";

        Company *c = new Company(data.at(i).second.toInt());
        m_TableLabels[m_LabelsExist] = new QLabel();
        m_TableLabels[m_LabelsExist + 1] = new QLabel();
        //m_TableLabels[m_LabelsExist + 2] = new QLabel();
        m_TableButtons[m_ButtonsExist] = new QPushButton();
        m_TableButtons[m_ButtonsExist + 1] = new QPushButton();

     //   qWarning() << "b";

        m_TableLabels[m_LabelsExist]->setText(c->getName());

     //   qWarning() << "b1";

        m_TableLabels[m_LabelsExist + 1]->setText(c->getContactByNumber(0).Name);

   //     qWarning() << "b2";
//
        m_TableButtons[m_ButtonsExist]->setText("Edit Company");
        m_TableButtons[m_ButtonsExist + 1]->setText("View Hoses");

    //    qWarning() << "c";

        ui->SearchResultsArea_CustomerC->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
        ui->SearchResultsArea_CustomerC->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
        //ui->SearchResultsArea_CustomerC->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);

        ui->SearchResultsArea_CustomerC->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 3);
        ui->SearchResultsArea_CustomerC->addWidget(m_TableButtons[m_ButtonsExist + 1], m_TableLines + 1, 4);

     //   qWarning() << "d";

        connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
           //on_TCEdit_clicked((CompanyID)data.at(i).second.toLong());
            loadPage(Pages::AddCustomer, data.at(i).second.toInt());
        });

        connect(m_TableButtons[m_ButtonsExist + 1], &QPushButton::clicked, [=]{
           //on_TCEdit_clicked((CompanyID)data.at(i).second.toLong());
            loadPage(Pages::HoseCenter, data.at(i).second.toInt());
        });

    //    qWarning() << "e";

        // iterate counters
        m_ButtonsExist += 2;
        m_LabelsExist += 2;
        ++m_TableLines;

        delete c;

    //    qWarning() << "f";
    }

 //   qWarning() << "11";

    // use the totalPages value to create buttons.
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentCustomers_CustomerCenter->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                loadCustomerCenter(SearchTerm, i * m_TableLimit);
             });

             ++m_ButtonsExist;
        }
    }

  //  qWarning() << "12";
}

void HoseTracker::on_CCSearch_clicked(){
    //SIGNAL loader function
    tableClear();
    QString data = ui->SearchBar_CustomerC->text();
    loadCustomerCenter(data); // enter the term in the search field here.
}

