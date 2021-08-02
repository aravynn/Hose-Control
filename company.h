#ifndef COMPANY_H
#define COMPANY_H


#include <vector>
#include <QString>
#include <QSqlError>

#include "TrackerEnums.h" // Enumerations for address.
#include "sqldatabase.h" // company will extensively use SQL, but should not directly call SQL.
#include "TrackerStructs.h" // for address, and probably other functions

/*
 * Create customer class object for use in getting and returning customer data, updating information, and general site queries.
 * This is intended to be used as a pointer object and will therefore be treated as such.
 */
class Company
{
private:
    // data from Companies table.
    QString m_Name;
    Address m_Billing;
    QString m_Notes;
    CompanyID m_PK; // The ID.

     SQLDatabase *e_db; // External database object pointer.

    // data from Contacts table, contact points table.
    std::vector<ContactPerson> m_Contact; // this encompasses all contacts, emails, phone numbers.

    // data from locations table.
    std::vector<Address> m_Locations; // non-billing locations.


protected: // company SQL queries will occur as protected in case this function needs to ever be extended in the future.
    std::vector<QString> getCompanyByID(CompanyID id);
    std::vector<ContactPerson> getContactsByID(CompanyID id);
    std::vector<QString> getContactPointsByID(CompanyID id, QString type);
    std::vector<Address> getAddressByID(CompanyID id);
    bool UpdateContactName(CompanyID id, QString name);
    bool UpdateContactList(CompanyID ContactID, QString OldList, QString NewList, QString type);
    bool AddContactList(CompanyID ContactID, QString List, QString Type);
public:
    Company(CompanyID id);
    Company(QString CompanyName, QString Notes, Address BillingAddress, std::vector<Address> Locations, std::vector<ContactPerson> Contacts);
    ~Company();

    QString getName();
    Address getBillingAddress();
    QString getNotes();
    int getContactCount();
    ContactPerson getContactByNumber(int number);
    int GetLocationCount();
    Address GetLocationByNumber(int number);
    Address GetPrimaryLocation();
    CompanyID GetCompanyID();

    // updates
    bool UpdateCoreData(QString name, QString notes);
    bool UpdateBillingAddress(Address billing);
    bool UpdateContactByNumber(CompanyID id, ContactPerson person);
    bool UpdateAddressByNumber(CompanyID AddressID, Address addr);

    //inserts.
    bool AddContact(ContactPerson person);
    bool AddLocation(Address addr);

    // deletes
    bool RemoveContactListByNumber(CompanyID ContactID, QString List, QString Type);
    bool RemoveContactByNumber(CompanyID id);
    bool RemoveAddressByNumber(CompanyID id);
};

#endif // COMPANY_H
