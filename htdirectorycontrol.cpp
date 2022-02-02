#include "htdirectorycontrol.h"

HTDirectoryControl::HTDirectoryControl()
{
    // check if the core directory exists. If it does not, create it anew.
    if(!QDir(QDir::homePath() + "/Documents/Hosetracker").exists()){
        // folder does not exist. we'll need to generate it. This make be moved later to a generic directory management class.
        generateDirectory();
    }

    // Set the location for m_CoreDirectory
    m_CoreDirectory = QDir::homePath() + "/Documents/Hosetracker";
}


void HTDirectoryControl::generateDirectory(){
    // this is the actual code for creating the directory structure.
    //  - PDF/      // for storing PDF's created for users.
    //  - Images/   // for storing images generated for users.
    //  - DB/       // for storing the central database, data.sqlite

    QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker");
    QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker/PDF");
    QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker/Images");
    QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker/DB");

}

void HTDirectoryControl::generateCompanyDirectory(QString folderName){
    // generate company directories in the appropriate folders. Generates all folders required.
    // each folder is checked individually in case someone attempts to override and create one folder but not the other.
    if(!QDir(QDir::homePath() + "/Documents/Hosetracker/PDF/" + folderName).exists()){
        QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker/PDF/" + folderName);
    }
    if(!QDir(QDir::homePath() + "/Documents/Hosetracker/Images/" + folderName).exists()){
        QDir().mkdir(QDir::homePath() + "/Documents/Hosetracker/Images/" + folderName);
    }
}

QString HTDirectoryControl::cleanName(QString &name){
    // clean a string and make it acceptable for use as a directory name
    // limit string length to 20 characters, regardless of string length.
    // cannot use: /\?%*:|"<>. also space
    // will limit to ascii 48-57 or 65-90 or 97-122
    std::stringstream ss;
    std::string base = name.toStdString();
    int counter = 20;

    for(int i{0}; i < name.length(); ++i){
        // iterate though, and clear all non 0-9a-zA-z, feed into ss.
       // if(base.at(i) == )
        char q = base.at(i);

        if(checkChar(q)){
            ss << base.at(i);
            --counter;
        }
        if(counter == 0)
            break;
    }
    return QString::fromStdString(ss.str());
}

bool HTDirectoryControl::moveFile(QString src, QString dest){
    // move file from source to destination. return true if it worked.
    if(src != "" && dest != ""){
        QFile::copy(src, dest);

    }
   return isFileExisting(dest);
}

bool HTDirectoryControl::deleteFile(QString src){
    //remove the file if exists.

    return QFile::remove(src);
}


ImageType HTDirectoryControl::getFileType(QString file){
    // get the file type of the uploaded file, and return the type or a ERR if the file is invalid.

        std::string png = "89504e470d0a1a0a"; // allegedly the string starter for the png. 8 bits as hex.
        std::string jpg = "ffd8ffd9"; // opener and closer of jpg.
        std::string gif87a = "4749463837613b"; //gif87a; the ; is the last bit of the file.
        std::string gif89a = "4749463839613b"; //gif89a;
        std::string bmp = "424d"; // the first 2 bits.

        // takes the file, opens it, looks for the information we need then bounce.
        std::ifstream myfile(file.toStdString(), std::ios::binary); // load file in binary mode.

        if(!myfile.good()){
            // bad filetype, return nothing.
            return ImageType::ERR;
        }

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(myfile), {});

       // for (int i{ 0 }; i < buffer.size(); ++i) {
       //     std::cout << buffer.at(i);
       // }

        std::stringstream ss;

        // 8 bit buffer, PNG
        for (int i{ 0 }; i < 8; ++i)
            if( (int)buffer.at(i) < 16 )
                ss << '0' << std::hex << (int)buffer.at(i);
            else
                ss << std::hex << (int)buffer.at(i);

        if (ss.str() == png)
            return ImageType::PNG;

        //std::cout << ss.str() << '\n';

        ss.clear();
        ss.str(std::string());

        // 4 bit buffer, JPG
        ss << std::hex << (int)buffer.at(0) << (int)buffer.at(1);
        ss << std::hex << (int)buffer.at(buffer.size() - 2) << (int)buffer.at(buffer.size() - 1);

        if (ss.str() == jpg)
            return ImageType::JPG;

        ss.clear();
        ss.str(std::string());

        // 7 bit buffer, gif, first 6 and lst bit.
        for (int i{ 0 }; i < 6; ++i)
            ss << std::hex << (int)buffer.at(i);

        ss << std::hex << (int)buffer.at(buffer.size() - 1);

        if (ss.str() == gif87a || ss.str() == gif89a)
            return ImageType::GIF;

        ss.clear();
        ss.str(std::string());

        //2 bit buffer, first 2 bits.
        for (int i{ 0 }; i < 2; ++i)
            ss << std::hex << (int)buffer.at(i);

        if (ss.str() == bmp)
            return ImageType::BMP;

        // if nothing matched, then it is not a valid image.
        return ImageType::ERR;

}

bool HTDirectoryControl::isFileExisting(QString src){
    // check if file exists.

   // qWarning() << src;

    return QFile::exists(src);
}


bool HTDirectoryControl::checkChar(char c){
    // check if character is valid or not, return true if it is.
    int d = (int)c;

    if(d > 47 && d < 58 ){
        return true;
    }
    if(d > 64 && d < 91 ){
        return true;
    }
    if(d > 96 && d < 123 ){
        return true;
    }
    return false;
}

QString HTDirectoryControl::getRootDirectory(){
    // return the root directory for all files.
    return m_CoreDirectory;
}

QString HTDirectoryControl::getPDFFolder(QString &company, bool fileRoot){
    // return the company-specific folder for storing PDFs. If fileRoot, return the base directory instead.
    if(!fileRoot){

        QString folderName = cleanName(company);

        if(!QDir(QDir::homePath() + "/Documents/Hosetracker/PDF/" + folderName).exists()){
            generateCompanyDirectory(folderName);
        }

        return m_CoreDirectory + "/PDF/" + folderName;

    } else {

        return m_CoreDirectory + "/PDF";

    }
}

QString HTDirectoryControl::getImagesFolder(QString &company, bool fileRoot){
    // return the company-specific folder for storing images. If fileRoot, return the base directory instead.
    if(!fileRoot){

        QString folderName = cleanName(company);

        if(!QDir(QDir::homePath() + "/Documents/Hosetracker/Images/" + folderName).exists()){
            generateCompanyDirectory(folderName);
        }

        return m_CoreDirectory + "/Images/" + folderName;

    } else {

        return m_CoreDirectory + "/Images";

    }
}

QString HTDirectoryControl::getDatabaseFolder(){
    // return the database folder.
    return m_CoreDirectory + "/DB";
}
