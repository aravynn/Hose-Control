#ifndef HOSETEST_H
#define HOSETEST_H

/**
 * @brief HoseTest::HoseTest
 *
 * Test the hoses through this function, this will handle all functions relating to the
 * Hose tests, saving information and management of the actual data. This will also be
 * Called by the PDF object to get the data related to the tests.
 *
 * Since this is an actual hose test object, it will likely need to independently create
 * the company, hose, and template objects so that it can source information as needed.
 *
 */

#include "hosetemplates.h"
#include <vector>
#include <QString>
#include <QSqlError>
#include <fstream>
#include <QDate>
#include <QDebug>
#include <thread>
#include <cmath>

#include "timeconvert.h"
#include "TrackerEnums.h" // Enumerations for address.
#include "sqldatabase.h" // company will extensively use SQL, but should not directly call SQL.
#include "TrackerStructs.h" // for address, and probably other functions
#include "fittingtemplate.h"
#include "htdirectorycontrol.h" // for hose image.
#include "company.h"
#include "hose.h"
#include "ESI-USB-API.h"

class HoseTest
{
private: // internal
    SQLDatabase *m_db; // external database object.
    Hose *m_hose; // external hose object. I don't know if we'll need more.
    bool m_hoseExists = false;
    CompanyID m_PK; // TEST ID

    CompanyID m_HoseID;
    int m_HoseCount; // for multiple hose objects.
    CompanyID m_OwnerID;

    QString m_OrderNumber;
    QDate m_Date; // datetime of test. probably TODAY always, honestly.
    QString m_ProofTestType;
    int m_TargetLoad;
    int m_MaxPressure;
    int m_HoldTimeInSeconds; // always manage in pure seconds.
    QString m_Elongation;
    QString m_TestNumber;
    QString m_CreatedBy;
    QString m_TestResult; // set after the fact.
    QString m_Connectivity;
    QString m_Comments;

    QString m_VisualCover;
    QString m_VisualTube;
    QString m_VisualFitting;
    QString m_VisualCrimp;

    TestType m_testtype;

    std::vector<QString> m_DamageImages;


    // for test data only: all other data is stored procedurally.
    double *m_TestTemperatures;
    double *m_TestPressure;
    long *TotalTestTime;
    long *m_GoodReadTime;
    int m_Sensor;

    bool *m_Release; // always defaults to false. This is used for stopping the external thread.
    bool m_isActive = false; // checks to see if there is an active test.
public: // external

    HoseTest(); // default creator. We don't need information until later.
    HoseTest(CompanyID id); // load in information, will probably be used on the results page and for PDF.

  /*  HoseTest(QString orderNumber, QDate date, QString proofTestType, int targetLoad, int holdTime,
        QString Elongation, QString testNumber, QString createdBy, QString Connectivity, QString comments); */

    ~HoseTest();

    void StartTest(CompanyID hoseID, int count, int holdTime, int targetLoad, int maxPressure, int sensorID); // start test, we'll need some data here.
    bool StopTest(); // end the test. we don't need more information.
    bool ResetTest(); // reset saved testing data.

    QString VisualCoverResult(bool Leaks, bool Delaminating, bool Wear, bool Gouges, bool Rot, bool Wires, bool Dents, bool Color);
    QString VisualFittingResult(bool Leaks, bool Cracked, bool Cam, bool Thread);
    QString VisualFerruleResult(bool Clamped, bool Alignment, bool Cracked);
    QString VisualTubeResult(bool Delaminating, bool Color, bool Cracked);

    bool UpdateTest(CompanyID OwnerID, QString orderNumber, QDate date, QString proofTestType, QString Elongation,
                    QString testNumber, QString createdBy, QString testResult, QString Connectivity, QString comments,
                    QString visualCover, QString visualTube, QString visualFitting, QString visualCrimp, TestType testtype = TestType::Sensor1); // used for both PASS and FAIL

    QString AddTestImage(QString image, QString CompanyName);

    CompanyID getID();
    CompanyID getHoseID();
    CompanyID getOwnerID();
    QString getOrderNumber();
    QDate getDate();
    QString getProofTestType();
    int getTargetLoad();
    int getMaxPressure();
    int getHoldTime();
    QString getElongation();
    QString getTestNumber();
    QString getCreator();
    QString getTestResult();
    QString getConnectivity();
    QString getComments();

    QString getVisualCover();
    QString getVisualTube();
    QString getVisualFitting();
    QString getVisualCrimp();
    std::vector<QString> getDamageImages();

    TestType getTestType(); // for PDF only, really.

    long getTestLength();
    long getGoodReadTime();
    double getPressureAtID(long id);
    double getTemperatureAtID(long id);

    bool GetActive();
};

#endif // HOSETEST_H
