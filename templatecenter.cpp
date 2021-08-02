#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Customer Center page functions, for loading data, will need some flexibilty as it has a search function.
 *
 */

void HoseTracker::loadTemplateCenter(QString SearchTerm, int offset){
    // load the hose center information, search results etc. all in one.

    m_IDPass = -1;
    m_IDSequence = 1;

    ui->SearchBar_TemplateC->setText(SearchTerm);

    std::vector<QString> getColumns{"PK"};
    QString table = "HoseTemplates";

    StrPair data; // defined for future use.

    int totalPages = 0;

    if(SearchTerm != ""){
        // there is a search term, as we'll use it to get the hoses.
        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);
        filter.at(0) = {"",""};

        // search TEMPLATES for matches, return list of PK's
        std::vector<QString> searchColumns = {"PartNumber", "Name", "Description", "Notes"};

        StrPair datalen = db->searchAction(getColumns, searchColumns, table, "%" + SearchTerm + "%", filter);
        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        data = db->searchAction(getColumns, searchColumns, table, "%" + SearchTerm + "%", filter, m_TableLimit, offset);
    } else {
        std::vector<std::pair<QString, DataPass>> filter;
        filter.resize(1);
        filter.at(0) = {"", ""}; // nothing.

        StrPair datalen = db->selectAction(getColumns, table, filter);
        totalPages = datalen.size() / m_TableLimit; // +1 in the likely event of
        if( datalen.size() % m_TableLimit > 0 ) totalPages++;

        // get the most recent hoses created.
        data = db->selectAction(getColumns, table, filter, m_TableLimit, offset, true);
    }

    int dataSize = data.size();


   // qWarning() << "loading....";

    for(int i{0}; i < dataSize; ++i){
        if(data.at(i).first == "")
            continue;

        HoseTemplates *ht = new HoseTemplates(data.at(i).second.toInt());

        m_TableLabels[m_LabelsExist] = new QLabel();
        m_TableLabels[m_LabelsExist + 1] = new QLabel();
        m_TableLabels[m_LabelsExist + 2] = new QLabel();
        m_TableButtons[m_ButtonsExist] = new QPushButton(); // edit button
        m_TableButtons[m_ButtonsExist + 1] = new QPushButton(); // new hose with template button.

        m_TableLabels[m_LabelsExist]->setText(ht->GetName());
        m_TableLabels[m_LabelsExist + 1]->setText(ht->GetInnerDiameter());
        m_TableLabels[m_LabelsExist + 2]->setText(QString::number(ht->GetWP()) + " PSI");
        m_TableButtons[m_ButtonsExist]->setText("Edit");
        m_TableButtons[m_ButtonsExist + 1]->setText("New Hose");

        // connect buttons to user functions.
        connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
           on_TCEdit_clicked((CompanyID)data.at(i).second.toLong());
        });

        connect(m_TableButtons[m_ButtonsExist + 1], &QPushButton::clicked, [=]{
           on_TCNewHose_clicked((CompanyID)data.at(i).second.toLong());
        });

        ui->SearchResultsArea_TemplateC->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
        ui->SearchResultsArea_TemplateC->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
        ui->SearchResultsArea_TemplateC->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);

        ui->SearchResultsArea_TemplateC->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
        ui->SearchResultsArea_TemplateC->addWidget(m_TableButtons[m_ButtonsExist + 1], m_TableLines + 1, 6);

        // iterate counters
        m_ButtonsExist += 2;
        m_LabelsExist += 3;
        ++m_TableLines;

        delete ht; // delete the hose.

    }
    if(totalPages > 1){
        for(int i{0}; i < totalPages; ++i){
            // for each, create a button.
             m_TableButtons[m_ButtonsExist] = new QPushButton();
             m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
             //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
             ui->RecentTemplates_TemplateCenter->addWidget(m_TableButtons[m_ButtonsExist]);

             connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                tableClear();
                loadTemplateCenter(SearchTerm, i * m_TableLimit);
             });

             ++m_ButtonsExist;
        }
    }
}

void HoseTracker::on_TCSearch_clicked(){
    //SIGNAL loader function
    tableClear();
    QString data = ui->SearchBar_TemplateC->text();
    loadTemplateCenter(data); // enter the term in the search field here.
}

void HoseTracker::on_TCEdit_clicked(IDType i){
    // use the given ID to load the add/edit template page.

    // set template, then load the Add/Edit template page.
    //m_IDPass = i;
    loadPage(Pages::AddHoseTemplate, i); // pass the page type and ID for load.

}

void HoseTracker::on_TCNewHose_clicked(IDType i){
    // use the given ID to load the new hose page

    loadPage(Pages::AddHose, i, IDSendType::HoseTemplate);

}
