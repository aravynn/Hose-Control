#include "company.h"

// GET functions here.

std::vector<QString> Company::getCompanyByID(const CompanyID id){
    // Take the given ID, and return a vector of data from the database. This will be parsed by the company object and then used.


    //std::vector<std::pair<QString, QString>> SQLDatabase::selectAction(,, std::vector<std::pair<QString, QString>> filter, int limit, int offset)

    std::vector<QString> columns = {"PK", "CompanyName", "BillAddr1", "BillAddr2", "BillCity", "BillProvince", "BillPostal", "BillCountry", "Notes"};
    QString table = "Companies";

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0).first = "PK";
    filter.at(0).second = id;

    std::vector<std::pair<QString, QString>> result = e_db->selectAction(columns, table, filter, 1);

    std::vector<QString> returns;
    returns.resize(result.size());

    int i{0};
    for(std::pair<QString, QString> r : result){

      //  qWarning() << "getCompanyByID: " << r.first << " " << r.second;

        returns.at(i) = r.second;
        ++i;
    }
    return returns;
}

std::vector<ContactPerson> Company::getContactsByID(CompanyID id){
    // return all of the contact information for the company by the given ID. Will return a vector of ContactPersons for easy transfer.
    // will likely call a sub-function to manage the contactpoint information, as it is in another database table.

    std::vector<QString> columns = {"PK", "Name"};
    QString table = "Contacts";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0).first = "CompanyPK";
    filter.at(0).second = id;

    StrPair selectResult = e_db->selectAction(columns, table, filter); // no limit or offset.

    std::vector<ContactPerson> results;

    int colCount = (int)columns.size();
    int rowCount = (int)selectResult.size() / colCount;

    results.resize(rowCount); // gives a count of number of vectors.

   // qWarning() << results.size();

    for(int i{0}; i < rowCount; ++i){
        // PK is result 0 but unused.
        results.at(i).id = selectResult.at(i * colCount).second.toInt(); // get the ID for future manipulation
        results.at(i).Name = selectResult.at( i * colCount + 1).second;
        results.at(i).Emails = getContactPointsByID(selectResult.at(i * colCount).second.toInt(), "email");
        results.at(i).PhoneNumbers = getContactPointsByID(selectResult.at(i * colCount).second.toInt(), "phone");

    }

    return results;
}

std::vector<QString> Company::getContactPointsByID(CompanyID id, QString type){
    // use the parameters for the query string to get contact points relevant to contact id.

   // qWarning() << "CONTACT POINT: " << id << " " << type;

    std::vector<QString> columns = {"List"};
    QString table = "ContactPoint";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(2);

    filter.at(0).first = "ContactPK";
    filter.at(0).second = id;

    filter.at(1).first = "Type";
    filter.at(1).second = type;

    StrPair selectResult = e_db->selectAction(columns, table, filter); // no limit or offset.

    std::vector<QString> results;

  // qWarning() << selectResult.at(0).first << selectResult.at(0).second;

    results.resize(selectResult.size()); // resize to size of returned results.

    for(int i{0}; i < (int)results.size(); ++i){

      //  qWarning() << selectResult.at(i).first << " " << selectResult.at(i).second;

        results.at(i) = selectResult.at(i).second; // get only the required result.
    }

    return results;
}

std::vector<Address> Company::getAddressByID(CompanyID id){
    // get and return the full list of addresses required by the system.

    std::vector<QString> columns = {"PK", "isPrimary", "ShipAddr1", "ShipAddr2", "ShipCity", "ShipProvince", "ShipPostal", "ShipCountry", "ShipName"};
    QString table = "Locations";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0).first = "CompanyPK";
    filter.at(0).second = id;

    StrPair selectResult = e_db->selectAction(columns, table, filter); // no limit or offset.

    std::vector<Address> results; // return a list of address objects.

    // get an int for col length and row length
    int colCount = (int)columns.size();
    int rowCount = (int)selectResult.size() / colCount;

    results.resize(rowCount); // make results at length of rows.

    for(int i{0}; i < rowCount; ++i){
        // go through each row and parse the results.
        results.at(i).id = selectResult.at(i*colCount).second.toInt();
        results.at(i).Addr1 = selectResult.at(i*colCount + 2).second;
        results.at(i).Addr2 = selectResult.at(i*colCount + 3).second;
        results.at(i).City = selectResult.at(i*colCount + 4).second;
        results.at(i).Province = selectResult.at(i*colCount + 5).second;
        results.at(i).Postal = selectResult.at(i*colCount + 6).second;
        results.at(i).Country = selectResult.at(i*colCount + 7).second;
        results.at(i).Name = selectResult.at(i*colCount + 8).second;

        //qWarning() << results.at(i).Name;

        if( selectResult.at(i*colCount + 1).second.toInt() == 1){
            results.at(i).Type = AddrType::Primary;
        } else {
            results.at(i).Type = AddrType::Secondary;
        }

    }
    return results;
}

// UPDATE functions here.
bool Company::UpdateCoreData(QString name, QString notes){

    // update core data

    m_Name = name;
    m_Notes = notes;

    QString table = "Companies";

    std::vector<std::pair<QString, DataPass>> options;
    options.resize(2);
    options.at(0).first = "CompanyName";
    options.at(0).second = name;

    options.at(1).first = "Notes";
    options.at(1).second = notes;

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0).first = "PK";
    filter.at(0).second = m_PK;

    if( !e_db->updateAction(table, options, filter) ){
        qWarning() << "ERROR: Update failed";
        return false;
    }
    return true;
}

bool Company::UpdateBillingAddress(Address billing){
    // update the current address to the new information.
    m_Billing = billing; // update the address in the object.

    // update the SQL database.
    QString table = "Companies";

    std::vector<std::pair<QString, DataPass>> updates;
    updates.resize(6);
    updates.at(0).first = "BillAddr1";
    updates.at(0).second = billing.Addr1;

    updates.at(1).first = "BillAddr2";
    updates.at(1).second = billing.Addr2;

    updates.at(2).first = "BillCity";
    updates.at(2).second = billing.City;

    updates.at(3).first = "BillProvince";
    updates.at(3).second = billing.Province;

    updates.at(4).first = "BillPostal";
    updates.at(4).second = billing.Postal;

    updates.at(5).first = "BillCountry";
    updates.at(5).second = billing.Country;

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0).first = "PK";
    filter.at(0).second = m_PK;

    if( !e_db->updateAction(table, updates, filter) ){
        //qWarning() << "ERROR: Update billing address failed";
        return false;
    }

    billing.Name = "Billing";

    // update the first address to match billing.
    return UpdateAddressByNumber(0, billing);

    //return true;
}

// update contact name (privatae) main function in company.cpp
bool Company::UpdateContactName(CompanyID contactID, QString name){
    // update the name based on the ID.

    QString table = "Contacts";
    std::vector<std::pair<QString, DataPass>> updates;
    updates.resize(1);
    updates.at(0).first = "Name";
    updates.at(0).second = name;

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(2);
    filter.at(0).first = "PK";
    filter.at(0).second = m_Contact.at(contactID).id; // use contact PK not numerical reference in contact.

    filter.at(1).first = "CompanyPK";
    filter.at(1).second = m_PK;

    if( !e_db->updateAction(table, updates, filter) ){
        //qWarning() << "ERROR: Update Contact Name failed";
        return false;
    }
    return true;
}

// update list by number and contact number and type (private)
bool Company::UpdateContactList(CompanyID ContactID, QString OldList, QString NewList, QString type){
    QString table = "ContactPoint";
    std::vector<std::pair<QString, DataPass>> updates;
    updates.resize(1);
    updates.at(0).first = "List";
    updates.at(0).second = NewList;

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(2);
    filter.at(0).first = "ContactPK";
    filter.at(0).second = ContactID;

    filter.at(1).first = "List";
    filter.at(1).second = OldList;

    filter.at(1).first = "Type";
    filter.at(1).second = type;

    if( !e_db->updateAction(table, updates, filter) ){
        //qWarning() << "ERROR: Update Contact Point failed";
        return false;
    }
    return true;
}

// update address by number
bool Company::UpdateAddressByNumber(CompanyID AddressID, Address addr){
    // take the old address and change it, allow it to be resubmitted.
    // this will also be used in the implication of changing a primary address.

    QString table = "Locations";

    std::vector<std::pair<QString, DataPass>> updates; // isPrimary, ShipAddr1, ShipAddr2, ShipCity, ShipProvince, ShipPostal, ShipCountry, ShipName
    updates.resize(8);
    updates.at(0) = {"isPrimary", static_cast<CompanyID>(addr.Type)};
    updates.at(1) = {"ShipAddr1", addr.Addr1};
    updates.at(2) = {"ShipAddr2", addr.Addr2};
    updates.at(3) = {"ShipCity", addr.City };
    updates.at(4) = {"ShipProvince", addr.Province };
    updates.at(5) = {"ShipPostal", addr.Postal };
    updates.at(6) = {"ShipCountry", addr.Country };
    updates.at(7) = {"ShipName", addr.Name };


    std::vector<std::pair<QString, DataPass>> filter; //CompanyPK, PK
    filter.resize(2);
    filter.at(0) = {"CompanyPK", m_PK};
    filter.at(1) = {"PK", m_Locations.at(AddressID).id}; // use the PK of the address, not the numerical ID.

    m_Locations.at(AddressID) = addr;

    if( !e_db->updateAction(table, updates, filter) ){
        //qWarning() << "ERROR: Update Contact Point failed";
        return false;
    }
    return true;
}

// add contact list by number (add to update contact)
bool Company::AddContactList(CompanyID ContactID, QString List, QString Type){
    // create an insert statement and add to the contact sheet.

    QString table = "ContactPoint";

    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(3);
    inserts.at(0) = {"ContactPK", ContactID};
    inserts.at(1) = {"List", List};
    inserts.at(2) = {"Type", Type};

    return e_db->insertAction(table, inserts);
}

bool Company::AddContact(ContactPerson person){
    // create a new contact in the company. After doing so, we should blank then re-get the list of people for this.

    QString table = "Contacts";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(2);
    inserts.at(0) = {"CompanyPK", m_PK};
    inserts.at(1) = {"Name", person.Name};

    //int ContactID = (int)m_Contact.size();

    person.id = e_db->getNewPK(table);

    // add the contact to the company object
    m_Contact.push_back(person);

    // insert the contact Emails
    for(int i{0}; i < (int)person.Emails.size(); ++i){
        // insert each email one by one.
        AddContactList(person.id, person.Emails.at(i), "email");
    }

    // insert the contact Phone Numbers
    for(int i{0}; i < (int)person.PhoneNumbers.size(); ++i){
        // insert each email one by one.
        AddContactList(person.id, person.PhoneNumbers.at(i), "phone");
    }
    return e_db->insertAction(table, inserts);
}

bool Company::AddLocation(Address addr){
    // add a new address for the user.

    QString table = "Locations";
    std::vector<std::pair<QString, DataPass>> inserts;
    inserts.resize(9);
    inserts.at(0) = {"CompanyPK", m_PK};
    inserts.at(1) = {"isPrimary", static_cast<CompanyID>(addr.Type)};
    inserts.at(2) = {"ShipAddr1", addr.Addr1};
    inserts.at(3) = {"ShipAddr2", addr.Addr2};
    inserts.at(4) = {"ShipCity", addr.City};
    inserts.at(5) = {"ShipProvince", addr.Province};
    inserts.at(6) = {"ShipPostal", addr.Postal};
    inserts.at(7) = {"ShipCountry", addr.Country};
    inserts.at(8) = {"ShipName", addr.Name};

    addr.id = e_db->getNewPK(table);
    m_Locations.push_back(addr);

    return e_db->insertAction(table, inserts);
}

bool Company::RemoveContactListByNumber(CompanyID ContactID, QString List, QString Type){
    // use list value, type, and contactid to determine and remove offending rows.

    QString table = "ContactPoint";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(3);
    filter.at(0) = {"ContactPK", ContactID};
    filter.at(1) = {"List", List};
    filter.at(2) = {"Type", Type};
    return e_db->deleteAction(table, filter);
}

bool Company::RemoveContactByNumber(CompanyID id){
    // use the contact number to remove the contact by ID. Will also need to remove ALL lines related to the contact list.

    // remove all list elements related to the user.
    QString table = "ContactPoint";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"ContactPK", m_Contact.at(id).id};
    if( !e_db->deleteAction(table, filter))
        return false;

    // remove the actual contact.
    table = "Contacts"; // change to contact table.
    filter.clear();
    filter.resize(1);
    filter.at(0) = {"PK", m_Contact.at(id).id}; // change filter to PK.


   // qWarning() << table << " " << filter.at(0).first << " " << filter.at(0).second.num;

    if( !e_db->deleteAction(table, filter))
        return false;

    m_Contact.erase(m_Contact.begin() + id); // remove the contact from the object.

    return true;
}

bool Company::RemoveAddressByNumber(CompanyID id){
    // remove the numbered address from the DB.

    QString table = "Locations";
    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_Locations.at(id).id };

    if( !e_db->deleteAction(table, filter))
        return false;

    CompanyID oldID = m_Locations.at(id).id;

    m_Locations.erase(m_Locations.begin() + id); // remove the address from the object.

    // NOW, replace the ID of deleted locations for all hoses that used it with 0
    table = "Hoses";

    filter.clear();
    filter.resize(2);
    filter.at(0) = {"OwnerPK", m_PK};
    filter.at(1) = {"locationPK", oldID};

    std::vector<std::pair<QString, DataPass>> updates;
    updates.resize(1);
    updates.at(0) = {"locationPK", (CompanyID)0}; // set to default of 0

    if(!e_db->updateAction(table, updates, filter))
        return false;

    return true;
}

// OLD TODO LIST ----------------------------------------------------

// create customer
