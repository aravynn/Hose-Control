#ifndef HTDIRECTORYCONTROL_H
#define HTDIRECTORYCONTROL_H

// This class controls all directory management for the app, in locations where directory control will be an issue.
// this will likely be an abstract class for the file management system, mostly used to return file system
// strings for PDFs, PNGs, and SQLite DBs

// directory setup, in c:/users/<username>/documents/
//hosetracker/
//  - PDF/      // for storing PDF's created for tests. If they exist they will not rewrite
//  - Images/   // for storing images generated for users.
//  - DB/       // for storing the central database, data.sqlite, as well as the config file for database sync.

#include <QDebug>
#include <QDir>
#include <QString>
#include <sstream> // for name validation.

#include <filesystem>
#include <system_error>
#include <fstream>
#include <QFile>
#include "TrackerEnums.h"

class HTDirectoryControl
{
private:
    QString m_CoreDirectory;

    void generateDirectory();

    bool checkChar(char c);
    void generateCompanyDirectory(QString folderName);
public:
    HTDirectoryControl();

    QString cleanName(QString &name);

    QString getRootDirectory();
    QString getPDFFolder(QString &company, bool fileRoot = false);
    QString getImagesFolder(QString &company, bool fileRoot = false);
    QString getDatabaseFolder();

    ImageType getFileType(QString image);

    bool moveFile(QString src, QString dest);
    bool deleteFile(QString src);
    bool isFileExisting(QString src);
};

#endif // HTDIRECTORYCONTROL_H
