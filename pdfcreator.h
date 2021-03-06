#ifndef PDFCREATOR_H
#define PDFCREATOR_H

#include <vector>
#include <QString>
#include <QSqlError>
#include <fstream>
#include <QDate>
#include <QDebug>
#include <QPixmap>

#include "timeconvert.h" // converts QDate for current date.
#include "TrackerEnums.h" // Enumerations for address.
#include "TrackerStructs.h" // for address, and probably other functions

#include "hosetemplates.h"
#include "hosetest.h"
#include "hose.h"
#include "company.h"
#include "htdirectorycontrol.h" // for hose image.
#include "fittingtemplate.h"

#include "pdf-vulcaneware/pdf.h" // for PDF generator. We might need to fix sourcing here.

/**
 * @brief The PDFCreator class
 *
 * This class manages and creates the PDF's direct from hose control for
 * all tests. The structure is a container class for the Vulkanware software.
 *
 * Note: At least one known issue exists which loads an error on printing PDFs.
 * It is currently unknown if the issue is related to our code or not.
 * The issue may also come from the existing PDF library.
 *
 */


class PDFCreator
{
private:
    HoseTest *m_HoseTest;
    Hose *m_Hose;
    Company *m_Company;
    PDF m_pdf;

    bool getImageArray(QString filepath, Image &pixels); // only to be used by internal functions.

    std::string DBLtoSTR(double number);

    bool makeHeader();
    bool makeChart(TestType type);
    int makeInfoArea(QString Title, std::vector<std::pair<QString, DataPass>> inserts, int xPos, int yPos, int passfail = -1);
    bool makeFooter();

    bool makeImagePage();


public:
    PDFCreator(CompanyID id); // id is for the hose test.
    ~PDFCreator();

    bool MakePDF();
    bool PDFExists();
    QString GetPDFPath(bool blankPath = false);
};

#endif // PDFCREATOR_H
