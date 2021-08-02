#ifndef SQLDATABASE_H
#define SQLDATABASE_H

// define an SQL database for the app to run. Will work with the embedded module for SQLite.
// files will be stored in the C:/Documents/Hosetracker folder. To reset, for now we'll delete the
// folder and that will force a reset. See reading list in case you lose the link for tutorial.
// if embedded fails, we'll fall back to the DLL.

#include <vector>
#include <string>
#include <utility>
#include <algorithm>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QVariant>
#include <QSqlQuery>
#include <QHash>
#include <QDebug>
#include <QDir>
//#include <Qfile>
#include <QStringList>

#include <htdirectorycontrol.h> // our directory program.
#include <TrackerEnums.h>
#include <TrackerStructs.h>

class SQLDatabase
{

private:
    QSqlDatabase db;
    void dummyData();       // dummy data for testing, not longer relevant.
    bool testConnection();  // test to see if the DB exists.
    void addSyncTable();    // adds the sync table, if it does not exist.

    QString whereString(std::vector<std::pair<QString, DataPass>> filter, QString separator);
    bool Bind(QSqlQuery &query, std::vector<std::pair<QString, DataPass>> filter);
public:
    SQLDatabase();
    ~SQLDatabase();
    void initializeDatabase(); // create the database if not existing.
    void startDB(); // start the actual database processes.

    StrPair selectAction(std::vector<QString> columns, QString table, std::vector<std::pair<QString, DataPass>> filter, int limit = -1, int offset = -1, bool desc = false);
    StrPair searchAction(std::vector<QString> getColumns, std::vector<QString> searchColumns, QString table, QString searchTerm, std::vector<std::pair<QString, DataPass>> filter, int limit = -1, int offset = -1);
    bool updateAction(QString table, std::vector<std::pair<QString, DataPass>> updates, std::vector<std::pair<QString, DataPass>> filter);
    bool insertAction(QString table, std::vector<std::pair<QString, DataPass>> inserts);
    bool deleteAction(QString table, std::vector<std::pair<QString, DataPass>> filter);

    bool ImportFile(QString file, QString table, char separator);

    CompanyID getNewPK(QString table);

    // general function
    int getQueryLength(QSqlQuery &query);

    // SQL Sync
    bool AddSyncLog(QString table, CompanyID ID, Sync SyncType);
};

#endif // SQLDATABASE_H
