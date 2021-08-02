#include "hosetest.h"


void TestMyThread(int holdTime, int holdPressure, int maxPressure, int sensorNo, bool* release, double *TP, double *TT, long *TotalTestTime, long *GoodReadTime){
    // initialize the test thread. we'll output data using an external thread as before.
    // m_Release can be defined to BLOCK the code. we'll also get the time as well as the Test Pressure

 //  qWarning() << "Start External Thread." << QString::number(maxPressure) << QString::number(holdPressure);

    const double minBuffer = 5.0;

    int* sensorNumbers = new int[2];

        sensorNumbers[0] = 0;
        sensorNumbers[1] = 1; // for whatever reason, why can't we send as a non-array? Not a huge issue

        int succ = FindSensors(sensorNumbers);

        //qWarning() << "Succ: " << succ;

        // choose a sensor to use. In this case, it should always be user defined.
        UseSensor(sensorNo);

        // declare an int to be used in effect as a bool. Not sure why they didn't set this themselves....
        int* Used = new int{ 0 };

        IsSensorUsed(sensorNo, Used);

        if(*Used != 1){
            // this is a failed result. error code?
            qWarning() << "ERROR: failed to find sensor - " << *Used << " Succ: " << succ;
            *release = true; // this is done now.
        }


        //qWarning() << "Sensor #0 is used: " << *Used << '\n';

        float* pressureReading = new float{ 0.0f };
        float* temperatureReading = new float{ 0.0f };

        // read temperature of the current sensor, a requirement for the pressure sensor
        int getTemp = ReadTemperature(0, 0, temperatureReading);

        // read the pressure sensor. This will give a one-time reading.
        int getRead = Read(0, 2, false, *temperatureReading, pressureReading); // initial read is kept but never counted.

        // output a line of temperature and pressure.
       // qWarning() << *temperatureReading << "C " << *pressureReading << "PSI \n";

        TT[*TotalTestTime] = std::round(*temperatureReading * 100) / 100.0;
        TP[*TotalTestTime] = std::round(*pressureReading * 100) / 100.0;

        int goodRead{0}; // number of reads above m_TargetLoad.

       // qWarning() << "Data Pushed";


        // for new timer, the START component
        auto sTime = std::chrono::steady_clock::now();

        while (*release == false) {
            // chrono time delay allows us to waits milliseconds between calls.

          // qWarning() << "Run While Loop";



            //std::this_thread::sleep_for(std::chrono::seconds(1)); // use seconds for better calls.

            /**
             * CODE EDIT HERE FOR BETTER TIMING.
             *
             */


            auto eTime = std::chrono::steady_clock::now();
                    //std::cout << nowTime;

            while (std::chrono::duration_cast<std::chrono::milliseconds>(eTime - sTime).count() < 970) {
                // for every 970 milliseconds, check, then after that you can display.
                std::this_thread::sleep_for(std::chrono::milliseconds(100)); // use seconds for better calls.

                eTime = std::chrono::steady_clock::now();

            }

            sTime = std::chrono::steady_clock::now(); // set it at completion, this should then count correctly.



            // END OF CODE EDIT. Process occurs now, to limit bad sleep time.

            getTemp = ReadTemperature(0, 0, temperatureReading);
            getRead = Read(0, 2, false, *temperatureReading, pressureReading);

           // qWarning() << *temperatureReading << "C " << *pressureReading << "PSI \n";

            ++*TotalTestTime; // increase total test time.


            if(*TotalTestTime >= MaximumTestTime){
                // this has gone for longer than the array, close this out.
                break; // exit the while, and close out.
            }

          //  qWarning() << "Iterated to: " << *TotalTestTime;

            // log the result values in the vector. Round to 2 decimal places for all values, we don't need more than that for accuracy.
            TT[*TotalTestTime] = std::round(*temperatureReading * 100) / 100.0;
            TP[*TotalTestTime] = std::round(*pressureReading * 100) / 100.0;

            // minus min buffer from holdpressure to protect against close calls.
            if(*pressureReading > holdPressure - (1 + minBuffer) && *pressureReading <= maxPressure){
                // this is a good read.
           //     qWarning() << "Good Temperature Read " << *pressureReading << " > " << (holdPressure - 1);
                ++*GoodReadTime;
                if(++goodRead >= holdTime){
                    // we've had enough successful reads to consider this test done.
                 //   qWarning() << "Release the loop, " << goodRead << " readings pass";
                    *release = true;
                }
            }
        }

        ++*TotalTestTime; // one final to have space for every saved value.

        *release = false; // reset m_Release. Only safe place to do this.
        // release the sensor from use. must be called in cleanup.
        ReleaseSensor(sensorNo);

       // qWarning() << "Clear resources";

         // clean up all testing information.
        delete[] sensorNumbers;
        delete Used;
        delete pressureReading;
        delete temperatureReading;

        // ESI defined cleanup of internal values.
        CleanUp();

}


HoseTest::HoseTest() // default creator. We don't need information until later.
{
    m_db = new SQLDatabase(); // initialize the database.
    m_Release = new bool(false);
    m_TestTemperatures = new double[MaximumTestTime];
    m_TestPressure = new double[MaximumTestTime];
    TotalTestTime = new long(0);
    m_GoodReadTime = new long(0);

}




HoseTest::HoseTest(CompanyID id){ // this test is already complete.
    m_db = new SQLDatabase();
    m_Release = new bool(false);
    m_TestTemperatures = new double[MaximumTestTime];
    m_TestPressure = new double[MaximumTestTime];
    TotalTestTime = new long(0);
    m_GoodReadTime = new long(0); // so we have something to delete.
    m_PK = id;

    QString table = "HoseTests";
    std::vector<QString> columns = {"HosePK", "OrderNumber", "Date", "ProofTestType", "TargetLoad", "MaxLoad", "TargetLoadHoldTime", "Elongation", "TestNumber", "OwnerPK",
                                    "CreatedBy", "TestResult", "Connectivity", "Comments", "VisualCover", "VisualTube", "VisualFitting", "VisualCrimp", "TestType"};

    std::vector<std::pair<QString, DataPass>> filter;
    filter.resize(1);
    filter.at(0) = {"PK", m_PK};

    StrPair results = m_db->selectAction(columns, table, filter, 1);

    m_hose = new Hose(results.at(0).second.toLong());
    m_hoseExists = true;
    m_HoseID = results.at(0).second.toLong();
    m_OrderNumber = results.at(1).second;

    TimeConvert t;

 //   qWarning() << "T Loaded";

    m_Date = t.convert(results.at(2).second);


  //  qWarning() << "Date Converted";


    m_ProofTestType = results.at(3).second;
    m_TargetLoad = results.at(4).second.toInt();

    m_MaxPressure = results.at(5).second.toInt(); // should be 10% up.

    m_HoldTimeInSeconds = results.at(6).second.toInt(); // stored in seconds.
    m_Elongation = results.at(7).second;
    m_TestNumber = results.at(8).second;
    m_OwnerID = results.at(9).second.toLongLong();
    m_CreatedBy = results.at(10).second;
    m_TestResult = results.at(11).second;
    m_Connectivity = results.at(12).second;
    m_Comments = results.at(13).second;
    m_VisualCover = results.at(14).second;
    m_VisualTube = results.at(15).second;
    m_VisualFitting = results.at(16).second;
    m_VisualCrimp = results.at(17).second;
    m_testtype = static_cast<TestType>(results.at(18).second.toInt());

    table = "TestData";

    filter.at(0) = {"TestPK", m_PK};

    columns.clear();
    columns.push_back("Temperature");
    columns.push_back("Pressure");
    columns.push_back("IntervalNumber");

    results.clear();
    results = m_db->selectAction(columns, table, filter);

    int tLength = (int)results.size();

   // int ResultCount{0};

    for(int i{0}; i < tLength; i+=3){
        // for every 3, insert values into the arrays.
        m_TestTemperatures[results.at(i+2).second.toInt()] = results.at(i).second.toDouble();
        m_TestPressure[results.at(i+2).second.toInt()] = results.at(i+1).second.toDouble();
        ++*TotalTestTime; // iterate up the total test time.
    }

    // get any damage images
    table = "TestImages";
    columns.clear();
    columns.push_back("Image");

    filter.clear();
    filter.push_back({"TestPK", id});

    results.clear();
    results = m_db->selectAction(columns, table, filter);

    tLength = (int)results.size();
    for(int i{0}; i < tLength; ++i){
        //qWarning() << results.at(i).second;
        m_DamageImages.push_back(results.at(i).second);
    }
   // qWarning() << "Test Time: " << *TotalTestTime;

} // load in information, will probably be used on the results page and for PDF.

HoseTest::~HoseTest(){
    // clear the created objects.
    delete m_db;
   // qWarning() << "1";
    if(m_hoseExists){
        delete m_hose;
    }
   // qWarning() << "2";
    delete m_Release;
   // qWarning() << "3";
   // m_TestTemperatures->clear();
   // m_TestPressure->clear();

   delete[] m_TestTemperatures;
   // qWarning() << "4";
   delete[] m_TestPressure;
   // qWarning() << "5";
   delete TotalTestTime;
  //  qWarning() << "6";
   delete m_GoodReadTime;
  //  qWarning() << "7";
}

void HoseTest::StartTest(CompanyID hoseID, int count, int holdTime, int targetLoad, int maxPressure, int sensorID){
    // start by assigning the given values, but not sending anything to the DB. We don't do so until the test is completed and passed.
    m_hose = new Hose(hoseID);
    m_hoseExists = true;
    m_HoseID = hoseID;
    m_HoseCount = count; // SET TO WORK FOR MANY HOSES ----------------------------------------------------------------------------------------------------!!

    m_HoldTimeInSeconds = holdTime;
    m_TargetLoad = targetLoad;
    m_MaxPressure = maxPressure;

    //int sensorID links to determine which sensor we are to read from. default to 0, for the first sensor found. 
    // we'll need to test this a few times for confirmation it works as expected.
    m_Sensor = sensorID;
    
    m_isActive = true;
    // once information is saved, we'll need to load the sensor and run the sensor test.
    // we're gonna need to error code the shit outta this.
    //qWarning() << "Hold Time: " << m_HoldTimeInSeconds << " Target: " << m_TargetLoad << " Sensor: " << m_Sensor << " TotalTestTime " << *TotalTestTime;

    // this is our thread call. must call "this" in order to use this object.

    //(int holdTime, int holdPressure, int sensorNo, bool* release, std::vector<float>* TP, std::vector<float>* TT){

    std::thread th1( TestMyThread, m_HoldTimeInSeconds, m_TargetLoad, m_MaxPressure, m_Sensor, m_Release, m_TestPressure, m_TestTemperatures, TotalTestTime, m_GoodReadTime );

    // detach the thread
    th1.detach(); // this prevents it screwing up in the background. Can continue running freely without destroying resources.
    //th1.join(); // this joins to the main thread, however it becomes a dependency, and pauses the main thread from the looks of it.
   // qWarning() << "Thread Detached";
    // any calls to destroy the thread will now be handled externally.

} // start test, we'll need some data here.

bool HoseTest::StopTest(){
    *m_Release = true; // stop the test process, it will kill itself.
    m_isActive = false;
    return true;
} // end the test. we don't need more information.

bool HoseTest::ResetTest(){
    // remove all data from the test information. We can continue to run the test after this.
    delete[] m_TestPressure;
    delete[] m_TestTemperatures;
    delete TotalTestTime;

    m_TestTemperatures = new double[MaximumTestTime];
    m_TestPressure = new double[MaximumTestTime];
    TotalTestTime = new long(0);

    return true;
} // reset saved testing data.

bool HoseTest::UpdateTest(CompanyID OwnerID, QString orderNumber, QDate date, QString proofTestType, QString Elongation,
                          QString testNumber, QString createdBy, QString testResult, QString Connectivity, QString comments,
                          QString visualCover, QString visualTube, QString visualFitting, QString visualCrimp, TestType testtype){

    // a test is only done and saved *once* after it is saved, it needs to be redone as a new test.
    // set all variables for this test.
    m_OwnerID = OwnerID;
    m_OrderNumber = orderNumber;
    m_Date = date;
    m_ProofTestType = proofTestType;
    m_Elongation = Elongation;
    m_TestNumber = testNumber;
    m_CreatedBy = createdBy;
    m_TestResult = testResult;
    m_Connectivity = Connectivity;
    m_Comments = comments;

    m_VisualCover = visualCover;
    m_VisualTube = visualTube;
    m_VisualFitting = visualFitting;
    m_VisualCrimp = visualCrimp;

    m_testtype = testtype;

    // for loop here enacts on all hose tests.
    for(int i{0}; i < m_HoseCount; ++i){

        // delete, then recreate, each hose in the series.
        delete m_hose;
        m_hose = new Hose(m_HoseID + i); // 1 for each hose in created.
       // inserts.at(0).second = m_HoseID + i;

        // despite being called an update, this is an *INSERT*
        QString table = "HoseTests";
        std::vector<std::pair<QString, DataPass>> inserts;
        inserts.resize(19);
        inserts.at(0) = {"HosePK", m_HoseID + i};
        inserts.at(1) = {"OrderNumber", m_OrderNumber};

        TimeConvert t;
        inserts.at(2) = {"Date", (CompanyID)t.convert(m_Date)};
        inserts.at(3) = {"ProofTestType", m_ProofTestType};
        inserts.at(4) = {"TargetLoad", (CompanyID)m_TargetLoad};

        inserts.at(5) = {"MaxLoad", (CompanyID)m_MaxPressure};
        // m_MaxPressure needs to be saved as well.

        inserts.at(6) = {"TargetLoadHoldTime", (CompanyID)m_HoldTimeInSeconds};
        inserts.at(7) = {"Elongation", m_Elongation};
        inserts.at(8) = {"TestNumber", m_TestNumber};
        inserts.at(9) = {"OwnerPK", m_OwnerID};
        inserts.at(10) = {"CreatedBy", m_CreatedBy};
        inserts.at(11) = {"TestResult", m_TestResult};
        inserts.at(12) = {"Connectivity", m_Connectivity};
        inserts.at(13) = {"Comments", m_Comments};
        inserts.at(14) = {"VisualCover", m_VisualCover};
        inserts.at(15) = {"VisualTube", m_VisualTube};
        inserts.at(16) = {"VisualFitting", m_VisualFitting};
        inserts.at(17) = {"VisualCrimp", m_VisualCrimp};
        inserts.at(18) = {"TestType", static_cast<CompanyID>(m_testtype)};

        m_PK = m_db->getNewPK(table);

        m_db->insertAction(table, inserts);
        // once all information is inserted for the test, insert test results as well.

        if(m_TestResult == "PASS"){
            m_hose->RenewHose();
        } else if(m_TestResult == "FAIL"){
            m_hose->DecommissionHose();
        } else {
         //   qWarning() << "Error at 350(Hose): Test result unknown." << m_TestResult;
        }
        // reset variables and add the test results.
        table = "TestData";
        inserts.clear();
        inserts.resize(4);

        if(m_testtype == TestType::UL){
            *TotalTestTime = 66; // we're overriding existing values.
        }

        if(m_testtype == TestType::CSA){
            *TotalTestTime = 26; // we're overriding values again.
        }

        for(int i{0}; i < *TotalTestTime; ++i){
            // each test result needs to be encoded and inserted.
            inserts.at(0) = {"TestPK", m_PK};

            switch(m_testtype){
                case TestType::UL:
                    inserts.at(1) = {"Temperature", 21.0};
                    inserts.at(2) = {"Pressure", ULPressure[(i < 6 ? i : 5)]};
                    break;
                case TestType::CSA:
                    inserts.at(1) = {"Temperature", 21.0};
                    inserts.at(2) = {"Pressure", CSAPressure[(i < 6 ? i : 5)]};
                    break;
            case TestType::Sensor1:
            case TestType::Sensor2:
            default:
                inserts.at(1) = {"Temperature", (double)m_TestTemperatures[i]};
                inserts.at(2) = {"Pressure", (double)m_TestPressure[i]};
                break;
            }

            inserts.at(3) = {"IntervalNumber", (CompanyID)i};

            //qWarning() << "Insert Values: " << inserts.at(1).second.dbl << " " << inserts.at(2).second.dbl;

            m_db->insertAction(table, inserts); // insert all data for each line

        }
    }
    // data is all saved, move to print.

    return true;
} // used for both PASS and FAIL



QString HoseTest::VisualCoverResult(bool Leaks, bool Delaminating, bool Wear, bool Gouges, bool Rot, bool Wires, bool Dents, bool Color){
    // Bools relate to checkboxes on hosetest screen. This will take the checkbox data and convert the results to a string for the PDF results.
    if(!Leaks && !Delaminating && !Wear && !Gouges && !Rot && !Wires && !Dents && !Color){
        return "Good";
    }
    // There is an issue in which "Good" is insufficient.
    std::stringstream ss;
    if(Leaks){ ss << "Signs of leaking"; }
    if( Delaminating ){ ss << ((ss.str() != "") ? ", " : "") << "Cover damage or delamination"; }
    if( Wear ){ ss << ((ss.str() != "") ? ", " : "") << "Wear, blisters, bulging, or soft spots"; }
    if( Gouges ){ ss << ((ss.str() != "") ? ", " : "") << "Gouging"; }
    if( Rot ){ ss << ((ss.str() != "") ? ", " : "") << "Dry rot or weathering"; }
    if( Wires ){ ss << ((ss.str() != "") ? ", " : "") << "Compromised reinforcing wires"; }
    if( Dents ){ ss << ((ss.str() != "") ? ", " : "") << "Dents, kinks or hose twists"; }
    if( Color ){ ss << ((ss.str() != "") ? ", " : "") << "Discoloration"; }

    return QString::fromStdString(ss.str());
}
QString HoseTest::VisualFittingResult(bool Leaks, bool Cracked, bool Cam, bool Thread){
    // Bools relate to checkboxes on hosetest screen. This will take the checkbox data and convert the results to a string for the PDF results.
    if(!Leaks && !Cracked && !Cam && !Thread){
        return "Good";
    }
    std::stringstream ss;
    if( Leaks ){ ss << "Signs of leaking"; }
    if( Cracked ){ ss << ((ss.str() != "") ? ", " : "") << "Cracked, damaged or deformed"; }
    if( Cam ){ ss << ((ss.str() != "") ? ", " : "") << "Cam & Groove issues"; }
    if( Thread ){ ss << ((ss.str() != "") ? ", " : "") << "Thread/seat issues"; }

    return QString::fromStdString(ss.str());
}
QString HoseTest::VisualFerruleResult(bool Clamped, bool Alignment, bool Cracked){
    // Bools relate to checkboxes on hosetest screen. This will take the checkbox data and convert the results to a string for the PDF results.
    if( !Clamped && !Alignment && !Cracked){
        return "Good";
    }
    std::stringstream ss;
    if( Clamped ){ ss << "Poorly crimped/clamped fitting"; }
    if( Alignment ){ ss << ((ss.str() != "") ? ", " : "") << "Clamps aligned"; }
    if( Cracked ){ ss << ((ss.str() != "") ? ", " : "") << "Cracked/damaged ferrule"; }

    return QString::fromStdString(ss.str());

}
QString HoseTest::VisualTubeResult(bool Delaminating, bool Color, bool Cracked){
    // Bools relate to checkboxes on hosetest screen. This will take the checkbox data and convert the results to a string for the PDF results.
    if(!Delaminating && !Color && !Cracked){
        return "Good";
    }

    // if not, we'll need to run the string and return another result.
    std::stringstream ss;
    if(Delaminating){ ss << "Delaminating"; }
    if( Color ){ ss << ((ss.str() != "") ? ", " : "") << "Discoloration (Chemical)"; }
    if(Cracked){ ss << ((ss.str() != "") ? ", " : "") << "Cracked/Flaking"; }
    return QString::fromStdString(ss.str());
}

QString HoseTest::AddTestImage(QString image, QString CompanyName){
    // add the hose test image to the Database.
  //  qWarning() << "1";
    HTDirectoryControl dir;
 //   qWarning() << "2";

    QString extension;

    switch(dir.getFileType(image)){
    case ImageType::BMP:
       extension = ".bmp";
       break;
    case ImageType::GIF:
       extension = ".gif";
       break;
    case ImageType::JPG:
        extension = ".jpg";
        break;
    case ImageType::PNG:
        extension = ".png";
        break;
    case ImageType::ERR:
    default:
       return "";
        break;
    }

    /*if(dir.getFileType(image) == ImageType::ERR){
        return ""; // if the file is not a valid image, fail to use or transfer.
    }*/
 //   qWarning() << "3";
    //Company *c = new Company(m_OwnerID);

    QString tname = QString::number(m_db->getNewPK("HoseTests") + 1000);

    //delete c;
 //   qWarning() << "4";
    QString company = dir.cleanName(CompanyName);
    QString directoryRoot = dir.getImagesFolder(company, false); // for test images, use the company file
    QString imageName = dir.cleanName(tname); // clean the hose template name to set for the file transfer.
    QString finalpath = directoryRoot + "/" +  imageName + extension;
 //   qWarning() << "5";
    int i{1};
    while(dir.isFileExisting(finalpath)){
       // take the existing filepath and append numerals until we find a non-existing one.
       finalpath = directoryRoot + "/" +  imageName + "_" + QString::number(i) + extension;
       ++i;
    }
  //  qWarning() << "6";
    if(dir.moveFile(image, finalpath)){
        //return finalpath;
        // save the filepath to the DB for future use.
        QString table = "TestImages";
        std::vector<std::pair<QString, DataPass>> inserts;
        inserts.resize(2);
        inserts.at(0) = {"TestPK", m_db->getNewPK("HoseTests")}; // save to next PK location.
        inserts.at(1) = {"Image", finalpath};
    //    qWarning() << "7";
        bool ret = m_db->insertAction(table, inserts); // in case insert fails for any reason.

        if(ret){
            return finalpath;
        } else {
            return "";
        }

    } else {
        return ""; // transfer failed. return nothing so sytem knows.
    }

}



CompanyID HoseTest::getID(){ return m_PK; }
CompanyID HoseTest::getHoseID(){ return m_HoseID; }
CompanyID HoseTest::getOwnerID(){ return m_OwnerID; }
QString HoseTest::getOrderNumber(){ return m_OrderNumber; }
QDate HoseTest::getDate(){ return m_Date; }
QString HoseTest::getProofTestType(){ return m_ProofTestType; }
int HoseTest::getTargetLoad(){ return m_TargetLoad; }
int HoseTest::getMaxPressure(){ return m_MaxPressure; }
int HoseTest::HoseTest::getHoldTime(){ return m_HoldTimeInSeconds; }
QString HoseTest::getElongation(){ return m_Elongation; }
QString HoseTest::getTestNumber(){ return m_TestNumber; }
QString HoseTest::getCreator(){ return m_CreatedBy; }
QString HoseTest::getTestResult(){ return m_TestResult; }
QString HoseTest::getConnectivity(){ return m_Connectivity; }
QString HoseTest::getComments(){ return m_Comments; }
std::vector<QString> HoseTest::getDamageImages() { return m_DamageImages; }

QString HoseTest::getVisualCover(){ return m_VisualCover; }
QString HoseTest::getVisualTube(){ return m_VisualTube; }
QString HoseTest::getVisualFitting(){ return m_VisualFitting; }
QString HoseTest::getVisualCrimp(){ return m_VisualCrimp; }

long HoseTest::getTestLength(){ return *TotalTestTime; }
long HoseTest::getGoodReadTime(){ return *m_GoodReadTime; }
double HoseTest::getPressureAtID(long id){ return m_TestPressure[id]; }
double HoseTest::getTemperatureAtID(long id){ return m_TestTemperatures[id]; }

bool HoseTest::GetActive() {return m_isActive;}

TestType HoseTest::getTestType(){ return m_testtype; }
