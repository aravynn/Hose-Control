#include "company.h"

Company::Company(CompanyID id) : m_PK {id}
{
    // load the company information from the database using the assumed ID given. Should load all information fully.
    // may cause a crash in the event of missing information, which shouldn't be possible when creating a company

    m_Billing.Type = AddrType::Billing; // use the BILLING type, always for this address.

    e_db = new SQLDatabase; // create database for initialization calls.

    // use the given ID to collect all data related to the company, and return it to this state.
    {   // encompass this call to limit usage of the vector. This gets the primary information.
        std::vector<QString> companyDetails = getCompanyByID(id);
       // m_PK = id; // The ID.

    //   for(int i{0}; i < (int)companyDetails.size(); ++i){
      //      qWarning() << i << ": " << companyDetails.at(i);
        //}

        m_Name = companyDetails.at(1); // company name
        m_Billing.Addr1 = companyDetails.at(2);
        m_Billing.Addr2 = companyDetails.at(3);
        m_Billing.City = companyDetails.at(4);
        m_Billing.Province = companyDetails.at(5);
        m_Billing.Postal = companyDetails.at(6);
        m_Billing.Country = companyDetails.at(7);
        m_Notes = companyDetails.at(8);

  //      qWarning() << "Data Load Complete.";
    }

    m_Contact = getContactsByID(id); // this actually returns everything in a form which we require.
    m_Locations = getAddressByID(id); // get the locations.

   // delete db; // remove the database once we are done.

 //   qWarning() << "Database removed, onto next step.";
}

Company::Company(QString CompanyName, QString Notes, Address BillingAddress, std::vector<Address> Locations, std::vector<ContactPerson> Contacts) :
    m_Name{ CompanyName }, m_Billing{BillingAddress}, m_Notes{Notes} {
    // take all new information and insert it into the class as well as the DB.
    // We'll use the internal claass objects for parsing data into the DB.

    m_Billing.Type = AddrType::Billing; // use the BILLING type, always for this address.
    e_db = new SQLDatabase; // create database for initialization calls.

    // Add Base information to the DB
    QString table = "Companies";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(8);
    inserts.at(0) = {"CompanyName", m_Name};
    inserts.at(1) = {"BillAddr1", m_Billing.Addr1};
    inserts.at(2) = {"BillAddr2", m_Billing.Addr2};
    inserts.at(3) = {"BillCity", m_Billing.City};
    inserts.at(4) = {"BillProvince", m_Billing.Province};
    inserts.at(5) = {"BillPostal", m_Billing.Postal};
    inserts.at(6) = {"BillCountry", m_Billing.Country};
    inserts.at(7) = {"Notes", m_Notes};

    m_PK = e_db->getNewPK(table); // get the PK for this person.

    e_db->insertAction(table, inserts); // insert this person into DB.

    // Add Contacts to the DB (must be manually added)
    for(ContactPerson c : Contacts){
        // iterate through and create all people.
        AddContact(c);
    }

    // add the default billing address when adding a new customer.
    BillingAddress.Name = "Billing";
    AddLocation(BillingAddress);

    // Add Adddresses to the DB (Must be manually added)
    for(Address a : Locations){
        //iterate through locations and add all.
        AddLocation(a);
    }
}

Company::~Company(){
    delete e_db;
}
// Company required functions:
// gets (all fields)
QString Company::getName() { return m_Name; } // returns the company name.
Address Company::getBillingAddress() {return m_Billing; }// get billing address.
QString Company::getNotes(){ return m_Notes; }    // get notes
int Company::getContactCount(){ return (int)m_Contact.size(); }   // get contact count
ContactPerson Company::getContactByNumber(int number){ return m_Contact.at(number); }    // get contact by number
int Company::GetLocationCount(){ return (int)m_Locations.size(); }    // get location count. Does not return Billing address.
Address Company::GetLocationByNumber(int number){ return m_Locations.at(number); }    // get location by number
Address Company::GetPrimaryLocation(){
    // get primary location, based on the AddrType::Primary Flag.
    for(int i{0}; i < GetLocationCount(); ++i){
        // search through, find flagged struct.
        if(m_Locations.at(i).Type == AddrType::Primary)
            return m_Locations.at(i);
    }
    return m_Locations.at(0);
}
CompanyID Company::GetCompanyID() { return m_PK; } // get company ID (not likely used)
// updates (all fields)


// update contact by number
bool Company::UpdateContactByNumber(CompanyID id, ContactPerson person){
    // go through all data, and determine what is new vs. old, update accordingly.
    // NOTE: the update object should be a unique object that overtakes the contact AFTER changes are made to DB.

    // check if things need to be fixed, then update as required.
    if(m_Contact.at(id).Name != person.Name){
        //qWarning() << m_Contact.at(id).id << person.Name;

        UpdateContactName(id, person.Name);
    }
   // qWarning() << "1";
    // go through phone numbers, and update by number, or add where required.
    {
        int mcpCount = (int)m_Contact.at(id).PhoneNumbers.size();
        int ppCount = (int)person.PhoneNumbers.size();
        for(int i{0}; i < mcpCount; ++i){
            if(ppCount < i || ppCount == 0){
              //   qWarning() << "Remove List Item";
                RemoveContactListByNumber(m_Contact.at(id).id, m_Contact.at(id).PhoneNumbers.at(i), "phone");
            } else if(m_Contact.at(id).PhoneNumbers.at(i) != person.PhoneNumbers.at(i)){
                UpdateContactList(m_Contact.at(id).id, m_Contact.at(id).PhoneNumbers.at(i), person.PhoneNumbers.at(i), "phone");
            }
        }
        if(ppCount - mcpCount > 0){
            // there are more phone numbers to add.

            for(int i{mcpCount}; i < ppCount; ++i){
                AddContactList(m_Contact.at(id).id, person.PhoneNumbers.at(i), "phone");
            }
        }
    }
    //qWarning() << "2";
    // go through emails, and update by email, or add where required.
    {
        int mcpCount = (int)m_Contact.at(id).Emails.size();
        int ppCount = (int)person.Emails.size();
        for(int i{0}; i < mcpCount; ++i){
            if(ppCount < i || ppCount == 0){
             //   qWarning() << "Remove List Item";
               RemoveContactListByNumber(m_Contact.at(id).id, m_Contact.at(id).Emails.at(i), "email");
            } else if(m_Contact.at(id).Emails.at(i) != person.Emails.at(i)){
                UpdateContactList(m_Contact.at(id).id, m_Contact.at(id).Emails.at(i), person.Emails.at(i), "email");
            }
        }
        if(ppCount - mcpCount > 0){
            // there are more emails to add.
            for(int i{mcpCount}; i < ppCount; ++i){
                AddContactList(m_Contact.at(id).id, person.Emails.at(i), "email");
            }
        }
    }
    //qWarning() << "3";
    // replace the person in the Company object
    m_Contact.at(id) = person;
    return true;
}
