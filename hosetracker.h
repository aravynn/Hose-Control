#ifndef HOSETRACKER_H
#define HOSETRACKER_H

#include <QMainWindow>
#include <QSqlError>
#include <QFileDialog>
#include <QMimeDatabase>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QCryptographicHash>
#include <QProcess>
#include <vector>

#include "TrackerEnums.h"
#include "sqldatabase.h"
#include "TrackerStructs.h"
#include "company.h"
#include "hosetemplates.h"
#include "fittingtemplate.h"
#include "timeconvert.h"
#include "hose.h"
#include "hosetest.h"
#include "pdfcreator.h"
#include "databutton.h"
#include "graphview.h"

#include "webconnector.h"

#include <QDesktopServices>
#include <QUrl>

QT_BEGIN_NAMESPACE
namespace Ui { class HoseTracker; }
QT_END_NAMESPACE

/**
 * @brief The HoseTracker class
 *
 * This is the main UI element for the application
 * This class handles all button presses and other actions performed.
 *
 */

class HoseTracker : public QMainWindow
{
    Q_OBJECT
    using IDType = int;
public:
    HoseTracker(QWidget *parent = nullptr);
    ~HoseTracker();

private slots:
    void on_GoToHome_clicked();
    void on_GoToAddCustomer_clicked();
    void on_GoToTemplateCenter_clicked();
    void on_GoToAddHoseTemplate_clicked();
    void on_GoToAddFittingTemplate_clicked();
    void on_GoToAddHose_clicked();
    void on_GoToHelp_clicked();
    void on_GoToCustomerCenter_clicked();
    void on_GoToHoseCenter_clicked();
    void on_GoToAdmin_clicked();

    void on_Quit_clicked();
    void on_PageBack_clicked();

    // homepage functions - in homepage.cpp
    void loadSyncResults();
    void on_Sync_clicked();


    // hose center functions - in hosecenter.cpp
    void on_HCSearch_clicked();

    // customer center functions - in customercenter.cpp
    void on_CCSearch_clicked();

    //Add customer functions - in addeditcustomer.cpp
    void loadAddCustomerPage(int id, int offset = 0);
    void on_CustomerNewHose_clicked();
    void on_CustomerViewHoses_clicked();
    void on_CustomerSaveChanges_clicked();
    void on_CustomerEmail1_clicked();
    void on_CustomerEmail2_clicked();
    void on_CustomerAddLocation_clicked();



    // Template center functions - in templatecenter.cpp
    void on_TCSearch_clicked();
    void on_TCEdit_clicked(IDType i);
    void on_TCNewHose_clicked(IDType i);

    // add/edit templates - in addedittemplate.cpp
    void on_CouplingTemplate_Change(int c);
    void on_SaveChangesTemplate_clicked();
    void on_CreateHoseTemplate_clicked();
    void on_AddEnd_clicked();
    void loadFittingDropdowns();
    void on_FindImage_clicked();

    // add fitting templates - in addfittingtemplate.cpp
    void on_SaveFittingTemplate_clicked();

    // add edit hose  - in addedithose.cpp
    void AddHoseClearPage();
    void SetHoseTemplate(CompanyID id);
    void SetCompany(CompanyID id);
    void AddHoseLoadHose(CompanyID id, int offset = 0);
    void loadTemplateDropdowns();
    void loadOwnerDropdowns();
    void on_HoseCouplingTemplate_Change(int c);
    void on_HoseOwner_change(int c);
    void on_HoseTemplate_change(int c);
    void on_HoseSaveChanges_clicked();
    void on_HoseGoToTest_clicked();
    void loadHoseTestResult(CompanyID id);
    void on_AddAssetNumber_clicked();

    // Test Center - in testcenter.cpp
    void loadHoseTester(IDType id, int idCount);
    void on_FailPrint_clicked();
    void on_PassPrint_clicked();
    void on_TargetLoad_changed(QString s);
    void on_PeakLoad_changed(QString s);
    void on_TestTime_changed(int t);
    void on_StartButton_clicked();
    void on_FindDamageImage_clicked();
    void on_TestType_changed(int type);

    // Test Resukts - in testresults.cpp
    void loadTestResults(IDType id, int count, int offset = 0);
    void on_RetestHose_clicked();
    void on_ViewPDF_clicked();
    void updateGraph();

    // admin page - in adminpage.cpp
    void on_CSVChooseFile_clicked();
    void on_CSVUploadFile_clicked();

private:
    Ui::HoseTracker *ui;
    SQLDatabase *db;

    HoseTest *m_ht; // for loading and using the hose test object we NEED to clear this at the end of every test.
    bool m_htExist = false;
    bool *m_lock; // for locking the hose test down.
    // create generic variables to hold the ID's of database information. Reset with ResetIDs()

    IDType m_IDPass{-1}; // pass the first ID of a sequence.
    int m_IDSequence{1}; // also if at a hose test, add these ID's as well.

    // default page history for back button. Limit 5.
    //Pages pageHistory[5]{Pages::Home, Pages::Home, Pages::Home, Pages::Home, Pages::Home};
    PageHistory m_PageHistory[5]; // trust in default constructors and values.


    // table loading data, will be used by all tables on each page load. CONST limit.
    // IF YOU EVER ADJUST THIS, ALSO ADJUST THE TABLECLEAR FUCNTION
    int m_ButtonsExist = 0; // limit of CONST items to load per table.
    int m_LabelsExist = 0;
    int m_TableLines = 0;
    bool m_graphExist = false;

    std::vector<QString> m_assetNumbers; // default asset numbers.

    // max per page
    int m_TableLimit = 100; // seems like a large enough number to rarely come up.

    // images for the test center
    //std::vector<QString> m_TestImages;

    QPushButton * m_TableButtons[MaximumTableLines]; // adjust in trackerstructs if needed.
    QLabel * m_TableLabels[MaximumTableLines];
    GraphView * m_graph; // graph for the hose test and test results pages.

    void loadPage(Pages page, int id = -1, IDSendType idtype = IDSendType::NA, int idCount = 1, bool isBack = false); // load a page, and pass ID if required.
    void loadHomePage(); // load the home page fields.
    void loadHoseCenter(QString SearchTerm, IDType id = -1, int offset = -1); // load hose center fields.
    void loadCustomerCenter(QString SearchTerm, int offset = -1); // load customer center fields
    void loadTemplateCenter(QString SearchTerm, int offset = -1); // load template center fields
    void loadAddEditTemplatePage(int id);
    void loadAddHose(int id, IDSendType idtype);
    int getTemplateDropdownIndex(CompanyID id); // get the converted index.
    CompanyID getTemplateIDFromIndex(int index); // get the converted ID
    StrPair getHoseTemplatePKs(); // get all PK values for hose templates.
    StrPair getOwnerPKs(); // get all PK values for companies.
    int getOwnerDropdownIndex(CompanyID id);
    StrPair getTestPKs(CompanyID hose, int limit = -1, int offset = -1); // get all PKs for hose tests.
    CompanyID getOwnerIDFromIndex(int index);
    void loadRecentHoses(int offset = -1);
    void resetIDs(IDS id = IDS::ResetAll);
    void tableClear();

    // hose test page.
    QString makeTestNumber(CompanyID testid);
};
#endif // HOSETRACKER_H
