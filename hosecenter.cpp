#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Hose Center page functions, for loading data, will need some flexibilty as it has a search function.
 *
 */

void HoseTracker::loadHoseCenter(QString SearchTerm, IDType id, int offset){
    // load the hose center information, search results etc. all in one.

    m_IDPass = id;
    m_IDSequence = 1;

    ui->SearchBar_HoseC->setText(SearchTerm);

    std::vector<QString> getColumns{"PK"};
    QString table = "Hoses";

    StrPair data; // defined for future use.

    int totalPages = 0;

    if(SearchTerm != ""){



        // there is a search term, as we'll use it to get the hoses.
       // qWarning() << SearchTerm;
        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);

        // if a company ID is given
        if(id > -1){
            filter.at(0) = {"OwnerPK", (CompanyID)id};
        } else {
            filter.at(0) = {"",""};
        }
        // search CUSTOMERS for matches, return list of PK's
        //searchAction(std::vector<QString> getColumns, std::vector<QString> searchColumns, QString table, QString searchTerm, std::vector<std::pair<QString, DataPass>> filter)
        std::vector<QString> searchColumns{"CompanyName", "Notes"};

        StrPair companyData = db->searchAction(getColumns, searchColumns, "Companies", "%" + SearchTerm + "%", filter);

        // search TEMPLATES for matches, return list of PK's
        searchColumns.clear();
        searchColumns = {"PartNumber", "Name", "Description", "Notes"};

        StrPair templateData = db->searchAction(getColumns, searchColumns, "HoseTemplates", "%" + SearchTerm + "%", filter);

        // lastly, take the 2 PK lists, and concatenate them into a filter list to add to the HOSES search.
        searchColumns.clear();
        searchColumns = {"AssetNumber", "Notes"};

        // create filter range.
        filter.clear();
        filter.resize(companyData.size() + templateData.size());
        // iterate through both arrays and build new array. OwnerPK and TemplatePK
        int comlen = (int)companyData.size();
        int temlen = (int)templateData.size();

        int ccount{0};
        for(int i{0}; i < comlen; ++i){
            // OwnerPK setup.
            if(companyData.at(i).first != ""){ // ensure not a "null" value.
                filter.at(i) = {"OwnerPK", (CompanyID)companyData.at(i).second.toLong()};
            } else {
                --ccount;
            }
        }

        for(int i{0}; i < temlen; ++i){
            if(templateData.at(i).first != ""){ // ensure not a "null" value.
                filter.at(i + comlen + ccount) = {"TemplatePK", (CompanyID)templateData.at(i).second.toLong()};
            }
        }

        data.clear();


        StrPair datalen = db->searchAction({"PK"}, searchColumns, table, "%" + SearchTerm + "%", filter);
        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        data = db->searchAction(getColumns, searchColumns, table, "%" + SearchTerm + "%", filter, m_TableLimit, offset);

    } else {

        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);

        // if a company ID is given.
        if(id > -1){
            filter.at(0) = {"OwnerPK", (CompanyID)id};
        } else {
            filter.at(0) = {"",""};
        }

        // get the most recent hoses created.
        StrPair datalen = db->selectAction({"PK"}, table, filter);
        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        data = db->selectAction(getColumns, table, filter, m_TableLimit, offset, true);
    }

    // use data to output hose information.
    int dataSize = data.size();
    for(int i{0}; i < dataSize; ++i){
        if(data.at(i).first == "")
            continue;

        Hose *h = new Hose(data.at(i).second.toInt()); // load the hose

        m_TableLabels[m_LabelsExist] = new QLabel();
        m_TableLabels[m_LabelsExist + 1] = new QLabel();
        m_TableLabels[m_LabelsExist + 2] = new QLabel();
        m_TableLabels[m_LabelsExist + 3] = new QLabel();
        m_TableButtons[m_ButtonsExist] = new QPushButton();
        m_TableButtons[m_ButtonsExist + 1] = new QPushButton();

        m_TableLabels[m_LabelsExist]->setText(h->getOwnerName());
        m_TableLabels[m_LabelsExist + 1]->setText(h->GetName());
        m_TableLabels[m_LabelsExist + 2]->setText("ID# " + h->getAssetNumber());
        m_TableLabels[m_LabelsExist + 3]->setText(h->getStatus());
        m_TableButtons[m_ButtonsExist]->setText("Edit");
        m_TableButtons[m_ButtonsExist + 1]->setText("Test");

        ui->SearchResultsArea_HoseC->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
        ui->SearchResultsArea_HoseC->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
        ui->SearchResultsArea_HoseC->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);
        ui->SearchResultsArea_HoseC->addWidget(m_TableLabels[m_LabelsExist + 3], m_TableLines + 1, 4);

        ui->SearchResultsArea_HoseC->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
        ui->SearchResultsArea_HoseC->addWidget(m_TableButtons[m_ButtonsExist + 1], m_TableLines + 1, 6);

        connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
            loadPage(Pages::AddHose, data.at(i).second.toInt(), IDSendType::Hose);
        });

        connect(m_TableButtons[m_ButtonsExist + 1], &QPushButton::clicked, [=]{
            loadPage(Pages::TestingCenter, data.at(i).second.toInt(), IDSendType::Hose, 1);
        });

        // iterate counters
        m_ButtonsExist += 2;
        m_LabelsExist += 4;
        ++m_TableLines;

        delete h; // delete the hose.
    }
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentHoses_HoseCenter->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                loadHoseCenter(SearchTerm, id, m_TableLimit * i);
             });

             ++m_ButtonsExist;
        }
    }
}

void HoseTracker::on_HCSearch_clicked(){
    //SIGNAL loader function
    // perform search with the given text
    tableClear();
    QString data = ui->SearchBar_HoseC->text();
    loadHoseCenter(data, m_IDPass); // enter the term in the search field here.
}
