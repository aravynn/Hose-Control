#include "hosetracker.h"
#include "ui_hosetracker.h"

HoseTracker::HoseTracker(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HoseTracker)
{
    ui->setupUi(this);
    db = new SQLDatabase(); // initializes the database for all calls.
    db->startDB();


    WebConnector w;


    loadPage(Pages::Home);

     // Connect all of the "GoToHome" Pages to a single reusable slot.
    connect(ui->GoToHome_Help, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_Admin, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_HoseC, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_AddHose, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_CustomerC, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_TemplateC, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_TestCenter, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_AddCustomer, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_AddTemplate, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->GoToHome_Testresults, SIGNAL(clicked()), this, SLOT(on_GoToHome_clicked()));

    // connect all of the menus to the various required pages.
    connect(ui->actionHome, SIGNAL(triggered()), this, SLOT(on_GoToHome_clicked()));
    connect(ui->actionCustomer, SIGNAL(triggered()), this, SLOT(on_GoToAddCustomer_clicked())); // add customer page
    connect(ui->actionNewHose, SIGNAL(triggered()), this, SLOT(on_GoToAddHoseTemplate_clicked())); // add hose template page
    connect(ui->actionNewFitting, SIGNAL(triggered()), this, SLOT(on_GoToAddFittingTemplate_clicked())); // add fitting template page
    connect(ui->actionHose, SIGNAL(triggered()), this, SLOT(on_GoToAddHose_clicked())); // add new hose page
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(on_GoToHelp_clicked())); // go to help page
    connect(ui->actionTemplates, SIGNAL(triggered()), this, SLOT(on_GoToTemplateCenter_clicked())); // hose template center
    connect(ui->actionCustomers, SIGNAL(triggered()), this, SLOT(on_GoToCustomerCenter_clicked())); // customer center
    connect(ui->actionHoses, SIGNAL(triggered()), this, SLOT(on_GoToHoseCenter_clicked())); // hoses center
    connect(ui->actionAdmin, SIGNAL(triggered()), this, SLOT(on_GoToAdmin_clicked())); // admin center
    connect(ui->actionQuit, SIGNAL(triggered()), this, SLOT(on_Quit_clicked())); // quit Application

    //Home Page Buttons, for main menu.
    connect(ui->GoToTemplateButton_Home, SIGNAL(clicked()), this, SLOT(on_GoToTemplateCenter_clicked())); // hose template center
    connect(ui->GoToCustomerButton_Home, SIGNAL(clicked()), this, SLOT(on_GoToCustomerCenter_clicked())); // customer center
    connect(ui->GoToHoseCenterButton_Home, SIGNAL(clicked()), this, SLOT(on_GoToHoseCenter_clicked())); // hoses center
    connect(ui->AddHoseButton_Home, SIGNAL(clicked()), this, SLOT(on_GoToAddHose_clicked())); // add new hose page
    connect(ui->GoToAdmin_Home, SIGNAL(clicked()), this, SLOT(on_GoToAdmin_clicked())); // admin center
    connect(ui->SyncNow_Home, SIGNAL(clicked()), this, SLOT(on_Sync_clicked())); // load sync app.




    // Go Back Buttons
    connect(ui->GoBack_AddCustomer, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));
    connect(ui->GoBack_AddHose, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));
    connect(ui->GoBack_AddFitting, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));
    connect(ui->GoBack_TestCenter, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));
    connect(ui->GoBack_AddTemplate, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));
    connect(ui->GoBack_TestResults, SIGNAL(clicked()), this, SLOT(on_PageBack_clicked()));

    // Hose Center Page
    connect(ui->CreateNewHose_HoseC, SIGNAL(clicked()), this, SLOT(on_GoToAddHose_clicked())); // add new hose page
    connect(ui->SearchButton_HoseC, SIGNAL(clicked()), this, SLOT(on_HCSearch_clicked())); // search results update.

    // Customer Center Page
    connect(ui->AddCustomer_CustomerC, SIGNAL(clicked()), this, SLOT(on_GoToAddCustomer_clicked())); // add customer page
    connect(ui->SearchButton_CustomerC, SIGNAL(clicked()), this, SLOT(on_CCSearch_clicked())); // search results update.

    // Hose Template Center Page
    connect(ui->CreateNewHose_Template_C, SIGNAL(clicked()), this, SLOT(on_GoToAddHoseTemplate_clicked())); // add new hose page
    connect(ui->SearchButton_TemplateC, SIGNAL(clicked()), this, SLOT(on_TCSearch_clicked())); // search rsults update on template.
    connect(ui->ChooseImageButton_AddTemplate, SIGNAL(clicked()), this, SLOT(on_FindImage_clicked()));

    // add/edit template page
    connect(ui->CouplingEnd1_AddTemplate, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CouplingTemplate_Change(int)));
    connect(ui->CouplingEnd2_AddTemplate, SIGNAL(currentIndexChanged(int)), this, SLOT(on_CouplingTemplate_Change(int)));
    connect(ui->CreateHose_AddTemplate, SIGNAL(clicked()), this, SLOT(on_CreateHoseTemplate_clicked()));
    connect(ui->SaveChangesButton_AddTemplate, SIGNAL(clicked()), this, SLOT(on_SaveChangesTemplate_clicked()));
    connect(ui->End1AddEnd_AddTemplate, SIGNAL(clicked()), this, SLOT(on_AddEnd_clicked()));
    connect(ui->End2AddEnd_AddTemplate, SIGNAL(clicked()), this, SLOT(on_AddEnd_clicked()));

    //add fitting template
    connect(ui->SaveTemplate_AddFitting, SIGNAL(clicked()), this, SLOT(on_SaveFittingTemplate_clicked()));

    // add customer page
    connect(ui->CreateNewHose_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerNewHose_clicked()));
    connect(ui->ViewHoses_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerViewHoses_clicked()));
    connect(ui->SaveChanges_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerSaveChanges_clicked()));
    connect(ui->MainEmailButton_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerEmail1_clicked()));
    connect(ui->SecEmailButton_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerEmail2_clicked()));
    connect(ui->AddLocationButton_AddCustomer, SIGNAL(clicked()), this, SLOT(on_CustomerAddLocation_clicked()));

    // add edit hose page
    connect(ui->End1Coupling_AddHose, SIGNAL(currentIndexChanged(int)), this, SLOT(on_HoseCouplingTemplate_Change(int)));
    connect(ui->End2Coupling_AddHose, SIGNAL(currentIndexChanged(int)), this, SLOT(on_HoseCouplingTemplate_Change(int)));
    connect(ui->HoseOwner_AddHose, SIGNAL(currentIndexChanged(int)), this, SLOT(on_HoseOwner_change(int)));
    connect(ui->HoseTemplate_AddHose, SIGNAL(currentIndexChanged(int)), this, SLOT(on_HoseTemplate_change(int)));
    connect(ui->End1AddEndButton_AddHose, SIGNAL(clicked()), this, SLOT(on_AddEnd_clicked()));
    connect(ui->End2AddEndButton_AddHose, SIGNAL(clicked()), this, SLOT(on_AddEnd_clicked()));
    connect(ui->SaveChanges_AddHose, SIGNAL(clicked()), this, SLOT(on_HoseSaveChanges_clicked()));
    connect(ui->GoToTestHose_AddHose, SIGNAL(clicked()), this, SLOT(on_HoseGoToTest_clicked()));

    // test center page
    connect(ui->FailPrint_TestCenter, SIGNAL(clicked()), this, SLOT(on_FailPrint_clicked()));
    connect(ui->PassPrint_TestCenter, SIGNAL(clicked()), this, SLOT(on_PassPrint_clicked()));
    connect(ui->StartButton_TestCenter, SIGNAL(clicked()), this, SLOT(on_StartButton_clicked()));
    connect(ui->TargetLoad_TestCenter, SIGNAL(textChanged(QString)), this, SLOT(on_TargetLoad_changed(QString)));
    connect(ui->PeakLoad_TestCenter, SIGNAL(textChanged(QString)), this, SLOT(on_PeakLoad_changed(QString)));
    connect(ui->TestTimeMinutes_TestCenter, SIGNAL(valueChanged(int)), this, SLOT(on_TestTime_changed(int)));
    connect(ui->TestTimeSeconds_TestCenter, SIGNAL(valueChanged(int)), this, SLOT(on_TestTime_changed(int)));
    connect(ui->AddImage_TestCenter, SIGNAL(clicked()), this, SLOT(on_FindDamageImage_clicked()));
    m_lock = new bool(false);

    // test result page.
    connect(ui->RetestHose_TestResults, SIGNAL(clicked()), this, SLOT(on_RetestHose_clicked()));
    connect(ui->ViewPDF_TestResults, SIGNAL(clicked()), this, SLOT(on_ViewPDF_clicked()));

    // admin page
    connect(ui->ChooseCSVFileButton_Admin, SIGNAL(clicked()), this, SLOT(on_CSVChooseFile_clicked()));
    connect(ui->CSVUploadButton_Admin, SIGNAL(clicked()), this, SLOT(on_CSVUploadFile_clicked()));

}

HoseTracker::~HoseTracker()
{
    delete ui;
    delete db; // the SQL database will terminate here. use DB for all transactions.
    delete m_lock;
}


void HoseTracker::on_GoToHome_clicked(){
    // generic go to home, common reuse.
    loadHomePage();
    loadPage(Pages::Home);
    resetIDs();

}

void HoseTracker::on_GoToAddCustomer_clicked(){
    loadPage(Pages::AddCustomer);
        resetIDs(); // call IDS::Customer as argument to keep customer information, will need to be unique function call.
}
void HoseTracker::on_GoToTemplateCenter_clicked(){
    loadPage(Pages::TemplateCenter);
    resetIDs();
}
void HoseTracker::on_GoToAddHoseTemplate_clicked(){
    loadPage(Pages::AddHoseTemplate);
         resetIDs(); // call resetIDs(IDS::HoseTemplate); to keep template id
}
void HoseTracker::on_GoToAddFittingTemplate_clicked(){
    loadPage(Pages::AddFittingTemplate);
         resetIDs(); // csll resetIDs(IDS::FittingTemplate); to keep template id
}
void HoseTracker::on_GoToAddHose_clicked(){
    loadPage(Pages::AddHose);
        resetIDs(); // call resetIDs(IDS::Hose); to keep hose ID
}
void HoseTracker::on_GoToHelp_clicked(){
    loadPage(Pages::Help);
    setWindowTitle("Hose Control - Help");
    resetIDs();
}
void HoseTracker::on_GoToCustomerCenter_clicked(){
    loadPage(Pages::CustomerCenter);
    resetIDs();
}
void HoseTracker::on_GoToHoseCenter_clicked(){
    loadPage(Pages::HoseCenter);
    resetIDs();
}
void HoseTracker::on_GoToAdmin_clicked(){
    loadPage(Pages::AdminArea);
    setWindowTitle("Hose Control - Admin");
    resetIDs();
}

void HoseTracker::on_Quit_clicked(){
    // Quit the application.
    QCoreApplication::quit();
}

void HoseTracker::loadPage(Pages page, int id, IDSendType idtype, int idCount, bool isBack){
    // take the Pages enum, convert to int then load the relevant page.

    ui->StatusBar->setText("Status: Ready");

    if(!isBack){
        m_PageHistory[4] = m_PageHistory[3];
        m_PageHistory[3] = m_PageHistory[2];
        m_PageHistory[2] = m_PageHistory[1];
        m_PageHistory[1] = m_PageHistory[0];

        m_PageHistory[0].page = page;
        m_PageHistory[0].id = id;
        m_PageHistory[0].idType = idtype;
        m_PageHistory[0].idCount = idCount;
    }

    tableClear();
    // load functions based on page chosen.
    switch(page){
        case Pages::HoseCenter:
            loadHoseCenter("", id); // load with a blank area. This calls direct in the case of a search form.
            setWindowTitle("Hose Control - Hose Center");
            break;
        case Pages::CustomerCenter:
            loadCustomerCenter("");
            setWindowTitle("Hose Control - Customer Center");
            break;
        case Pages::TemplateCenter:
            loadTemplateCenter("");
            setWindowTitle("Hose Control - Template Center");
            break;
        case Pages::AddHoseTemplate:
            loadAddEditTemplatePage(id);
            setWindowTitle("Hose Control - Add/Edit Hose Templates");
            break;
        case Pages::Home:
            loadHomePage();
            setWindowTitle("Hose Control");
            break;
        case Pages::AddHose:
            loadAddHose(id, idtype);
            setWindowTitle("Hose Control - Add/Edit Hoses");
            break;
        case Pages::AddCustomer:
            loadAddCustomerPage(id);
            setWindowTitle("Hose Control - Add/Edit Customer");
            break;
        case Pages::TestingCenter:
            loadHoseTester(id, idCount); // we'll also have idtype defined but will not use.
            setWindowTitle("Hose Control - Test Center");
            break;
        case Pages::TestResults:
            loadTestResults(id, idCount); // ID type added but unused in this case.
            setWindowTitle("Hose Control - Test Results");
            break;
        default: // do nothing.
            break;
    }



    ui->mainPages->setCurrentIndex(static_cast<int>(page)); // go to current page.



}

void HoseTracker::on_PageBack_clicked(){
    // breaking the prior design, wthis will manually set us back 1 page, and set history back as well.

    // load from history struct.
    loadPage(m_PageHistory[1].page, m_PageHistory[1].id, m_PageHistory[1].idType, m_PageHistory[1].idCount, true);

    m_PageHistory[0] = m_PageHistory[1];
    m_PageHistory[1] = m_PageHistory[2];
    m_PageHistory[2] = m_PageHistory[3];
    m_PageHistory[3] = m_PageHistory[4];
    m_PageHistory[4] = PageHistory();
}

void HoseTracker::resetIDs(IDS id){
    // reset all ID's for database linking purposes.
    Q_UNUSED(id);
}

void HoseTracker::tableClear(){
    // I guess we need to do a specialized delete for these.

    if(m_graphExist){
        // only clear if something exists, or it'll crash
        m_graph->deleteLater(); // clear this whenever anything else is cleared.
        m_graphExist = false;
    }

    if(m_htExist){
        delete m_ht; // remove this in the case of home buttons.
        m_htExist = false; // reset for next use.
    }

    if(m_TableLines == 0){
        return; // nothing to clear, move along
    }

    for(int i{0}; i < m_ButtonsExist; ++i){
        m_TableButtons[i]->deleteLater();
    }

    for(int i{0}; i < m_LabelsExist; ++i){
        m_TableLabels[i]->deleteLater();
    }

    // zero out counters.
    m_ButtonsExist = 0;
    m_LabelsExist = 0;
    m_TableLines = 0;

}


