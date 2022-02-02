#include "hosetracker.h"
#include "ui_hosetracker.h"

/**
 *
 * Add or edit page for customers.
 *
 */

void HoseTracker::loadAddCustomerPage(int id, int offset){
    // take the ID and load the customer, or a blank page.

    // first, clear all old data.
    tableClear(); // clear all table data.

    // set the ID.
    m_IDPass = id;
    m_IDSequence = 1;

    ui->CompanyName_AddCustomer->setText("");
    ui->CustomerName_AddCustomer->setText("");
    ui->MainPhone_AddCustomer->setText("");
    ui->SecPhone_AddCustomer->setText("");
    ui->MainEmail_AddCustomer->setText("");
    ui->SecEmail_AddCustomer->setText("");
    ui->BillingAddress1_AddCustomer->setText("");
    ui->BillingAddress2_AddCustomer->setText("");
    ui->BillingCity_AddCustomer->setText("");
    ui->BillingState_AddCustomer->setText("");
    ui->BillingCountry_AddCustomer->setText("");
    ui->BillingZip_AddCustomer->setText("");
    ui->Notes_AddCustomer->setPlainText("");

    ui->LocationName_AddCustomer->setText("");
    ui->LocationAddress1_AddCustomer->setText("");
    ui->LocaitonAddress2_AddCustomer->setText("");
    ui->LocationCity_AddCustomer->setText("");
    ui->LocationState_AddCustomer->setText("");
    ui->LocationCountry_AddCustomer->setText("");
    ui->LocationZip_AddCustomer->setText("");

    if(id > -1){
        // load the customer data and use that.
        Company *c = new Company(id);

        ui->CompanyName_AddCustomer->setText(c->getName());

        ContactPerson cp = c->getContactByNumber(0); // 0 defaults as primary.

        ui->CustomerName_AddCustomer->setText(cp.Name);
        if(cp.PhoneNumbers.size() > 0) ui->MainPhone_AddCustomer->setText(cp.PhoneNumbers.at(0));
        if(cp.PhoneNumbers.size() > 1) ui->SecPhone_AddCustomer->setText(cp.PhoneNumbers.at(1));
        if(cp.Emails.size() > 0) ui->MainEmail_AddCustomer->setText(cp.Emails.at(0));
        if(cp.Emails.size() > 1) ui->SecEmail_AddCustomer->setText(cp.Emails.at(1));

        Address a = c->getBillingAddress();

        ui->BillingAddress1_AddCustomer->setText(a.Addr1);
        ui->BillingAddress2_AddCustomer->setText(a.Addr2);
        ui->BillingCity_AddCustomer->setText(a.City);
        ui->BillingState_AddCustomer->setText(a.Province);
        ui->BillingCountry_AddCustomer->setText(a.Country);
        ui->BillingZip_AddCustomer->setText(a.Postal);

        ui->Notes_AddCustomer->setPlainText(c->getNotes());

        // next, load any of the addresses that exist.
        int count = c->GetLocationCount();
        for(int i{offset}; (i < count && i < offset + m_TableLimit); ++i){
            Address l = c->GetLocationByNumber(i);
            // output l to the table.

            m_TableLabels[m_LabelsExist] = new QLabel(); //Name
            m_TableLabels[m_LabelsExist + 1] = new QLabel(); //addr1
            m_TableLabels[m_LabelsExist + 2] = new QLabel(); //addr2
            m_TableLabels[m_LabelsExist + 3] = new QLabel(); //city
            m_TableLabels[m_LabelsExist + 4] = new QLabel(); //province
            m_TableLabels[m_LabelsExist + 5] = new QLabel(); //country
            m_TableLabels[m_LabelsExist + 6] = new QLabel(); // postal
            m_TableButtons[m_ButtonsExist] = new QPushButton(); // delete

            m_TableLabels[m_LabelsExist]->setText(l.Name);
            m_TableLabels[m_LabelsExist + 1]->setText(l.Addr1);
            m_TableLabels[m_LabelsExist + 2]->setText(l.Addr2);
            m_TableLabels[m_LabelsExist + 3]->setText(l.City);
            m_TableLabels[m_LabelsExist + 4]->setText(l.Province);
            m_TableLabels[m_LabelsExist + 5]->setText(l.Country);
            m_TableLabels[m_LabelsExist + 6]->setText(l.Postal);
            m_TableButtons[m_ButtonsExist]->setText("Delete");

            // issues here????
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist], m_TableLines + 1, 1);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 1], m_TableLines + 1, 2);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 2], m_TableLines + 1, 3);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 3], m_TableLines + 1, 4);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 4], m_TableLines + 1, 5);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 5], m_TableLines + 1, 6);
            ui->LocationsArea_AddCustomer->addWidget(m_TableLabels[m_LabelsExist + 6], m_TableLines + 1, 7);

            ui->LocationsArea_AddCustomer->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 8);



            connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                // delete the location from the user, then reload the table.
                //int locationID = l.id; // the id to be removed.
                Company *com = new Company(id);

                com->RemoveAddressByNumber(i);

                delete com;

                loadPage(Pages::AddCustomer, id);
            });

        // iterate counters
        m_ButtonsExist += 1;
        m_LabelsExist += 7;
        ++m_TableLines;

        }

        int totalPages = c->GetLocationCount() / m_TableLimit;
        if(c->GetLocationCount() % m_TableLimit > 0) totalPages++; // increment for last page.
        if(totalPages > 1){
            for(int i{0}; i < totalPages; ++i){
                // for each, create a button.
                 m_TableButtons[m_ButtonsExist] = new QPushButton();
                 m_TableButtons[m_ButtonsExist]->setText(QString::number(i));
                 //ui->RecentHosesArea_Home->addWidget(m_TableButtons[m_ButtonsExist], m_TableLines + 1, 5);
                 ui->RecentLocations_AddCustomer->addWidget(m_TableButtons[m_ButtonsExist]);

                 connect(m_TableButtons[m_ButtonsExist], &QPushButton::clicked, [=]{
                    tableClear();
                    loadAddCustomerPage(id, i * m_TableLimit);
                 });

                 ++m_ButtonsExist;
            }
        }
        delete c;
    }
}

void HoseTracker::on_CustomerNewHose_clicked(){
    // load page to generate new hose

    loadPage(Pages::AddHose, m_IDPass, IDSendType::Customer);
}

void HoseTracker::on_CustomerViewHoses_clicked(){
    // view customer hoses only
    loadPage(Pages::HoseCenter, m_IDPass);
}

void HoseTracker::on_CustomerSaveChanges_clicked(){
    // save an alterations made to customer, unsaved changes are ignored.

    if(ui->CompanyName_AddCustomer->text() == ""){
        // not everything selected
        ui->StatusBar->setText("Error: Company name cannot be blank.");
        return;
    }

    if(ui->BillingAddress1_AddCustomer->text() == ""){
        ui->StatusBar->setText("Error: Billing address cannot be blank.");
        return;
    }

    // load the information into predefined structs.
    ContactPerson p;
    p.Name = ui->CustomerName_AddCustomer->text();
    if(ui->MainEmail_AddCustomer->text() != ""){
        p.Emails.push_back(ui->MainEmail_AddCustomer->text());
    }
    if(ui->SecEmail_AddCustomer->text() != ""){
        p.Emails.push_back(ui->SecEmail_AddCustomer->text());
    }
    if(ui->MainPhone_AddCustomer->text() != ""){
        p.PhoneNumbers.push_back(ui->MainPhone_AddCustomer->text());
    }
    if(ui->SecPhone_AddCustomer->text() != ""){
        p.PhoneNumbers.push_back(ui->SecPhone_AddCustomer->text());
    }

    Address a;
    a.Addr1 = ui->BillingAddress1_AddCustomer->text();
    a.Addr2 = ui->BillingAddress2_AddCustomer->text();
    a.City = ui->BillingCity_AddCustomer->text();
    a.Province = ui->BillingState_AddCustomer->text();
    a.Country = ui->BillingCountry_AddCustomer->text();
    a.Postal = ui->BillingZip_AddCustomer->text();
    a.Type = AddrType::Billing;

    if(m_IDPass > -1){

        ui->StatusBar->setText("Status: Updating");
        // update the company profile.
        Company *c = new Company(m_IDPass); // get the ID.
        c->UpdateCoreData(ui->CompanyName_AddCustomer->text(), ui->Notes_AddCustomer->toPlainText());
        c->UpdateContactByNumber(0, p); // only one contact is used for now. We might update this in the future.
        c->UpdateBillingAddress(a);
        delete c;

        // reload the page to prevent double saves.
        loadPage(Pages::AddCustomer, m_IDPass);

    } else {
        // create a new company.
        ui->StatusBar->setText("Status: Saving");
        // we don't actually need any addresses to add, since they are added and managed independently.
        // this capability function exists however for CSV import.
        std::vector<Address> nulladdr;

        // persons need to be in a vector, however we're only adding one.
        std::vector<ContactPerson> persons {p};

        Company *c = new Company(ui->CompanyName_AddCustomer->text(), ui->Notes_AddCustomer->toPlainText(), a, nulladdr, persons);

        m_IDPass = c->GetCompanyID();

        delete c;

        loadPage(Pages::AddCustomer, m_IDPass);
    }
    ui->StatusBar->setText("Status: Ready");

}

void HoseTracker::on_CustomerEmail1_clicked(){
    // are we going to add this function?
    // if the email application is initiated this will load that for emailing.
    QDesktopServices::openUrl(QUrl("mailto:" +  ui->MainEmail_AddCustomer->text()));

}

void HoseTracker::on_CustomerEmail2_clicked(){
    // are we going to add this function?

    // if the email application is initiated this will load that for emailing.
     QDesktopServices::openUrl(QUrl("mailto:" +  ui->SecEmail_AddCustomer->text()));
}

void HoseTracker::on_CustomerAddLocation_clicked(){
    // save changes of main customer before adding a location (we need data first.)
    on_CustomerSaveChanges_clicked();
    // save new location, and add to customer object.
    Company *c = new Company(m_IDPass);

    Address a;
    a.Name = ui->LocationName_AddCustomer->text();
    a.Addr1 = ui->LocationAddress1_AddCustomer->text();
    a.Addr2 = ui->LocaitonAddress2_AddCustomer->text();
    a.City = ui->LocationCity_AddCustomer->text();
    a.Province = ui->LocationState_AddCustomer->text();
    a.Postal = ui->LocationZip_AddCustomer->text();
    a.Country = ui->LocationCountry_AddCustomer->text();
    a.Type = AddrType::Secondary;

    c->AddLocation(a);

    // clear fields and reload page, (refresh so it is on the list. )
    delete c;
    loadAddCustomerPage(m_IDPass);
}
