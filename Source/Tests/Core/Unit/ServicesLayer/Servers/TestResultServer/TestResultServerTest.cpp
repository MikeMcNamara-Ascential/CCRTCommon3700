//*************************************************************************
// Description:
//   Test Result Server unit test class.
//
//===========================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/TestResultServer/TestResultServerTest.cpp $
// 
// 4     5/01/07 1:50p Rwiersem
// Changes for the 07050201 core release:
// 
// Added printing of the current test name to the screen.  Added
// TestPublish_RelayFile().
// 
// 3     10/26/06 3:13p Rwiersem
// Changes for the 06102701 core release:
// 
// Added tests for setting the fault and accept PLC lamp bits.
// 
// 2     9/22/06 11:28a Rwiersem
// Changes for the 06092201 core release:
//     - Added assertions to TestConstructor()
//     - Added assertions to TestLoadAdditionalConfigurationItems()
//     - Added TestReportResult()
//     - Added TestInitialize()
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#include "TestResultServerTest.h"
#include "TestResultServer.h"
#include "ITestResultThreadPool.h"
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

using namespace std;

//-----------------------------------------------------------------------------
TestResultServerTest::TestResultServerTest()
{
    // make sure the TEST_DIR_PATH is valid
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to the top testing directory" 
            << endl;
        exit(1);
    }

    // set the path to the test configuration files
    m_configPath = getenv("TEST_DIR_PATH");
    m_configPath += 
        "/Core/Unit/ServicesLayer/Servers/TestResultServer/Configuration/";
}

//-----------------------------------------------------------------------------
TestResultServerTest::~TestResultServerTest()
{
}

//-----------------------------------------------------------------------------
void TestResultServerTest::setUp()
{
    trs = new TestResultServer();
}

//-----------------------------------------------------------------------------
void TestResultServerTest::tearDown()
{
    delete trs;

    // move to the next line for the next test name
    cout << endl;
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestConstructor()
{
    cout << " TestConstructor ";

    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_testResultFile);
    CPPUNIT_ASSERT_EQUAL((INT16)0,trs->m_numberOfTests);
    CPPUNIT_ASSERT_EQUAL((INT16)0,trs->m_sequenceNumber);

    // The count on the semaphore should be set to 1.  So, the first 
    // CriticalSectionEnter() should be successful and the second one should
    // fail
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,
                         trs->m_semVehicleBuild.CriticalSectionEnter());
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         trs->m_semVehicleBuild.CriticalSectionEnter(1));

    CPPUNIT_ASSERT(trs->m_vehicleBuild == NULL);
    CPPUNIT_ASSERT_EQUAL(string("temp"),trs->m_allFailures.getName());
    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_allFailures.getValue());
    CPPUNIT_ASSERT_EQUAL(string(DTC_TAG),trs->m_reportedDTC.getName());
    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_reportedDTC.getValue());
    CPPUNIT_ASSERT_EQUAL(false,trs->m_testInProgress);
    CPPUNIT_ASSERT_EQUAL(false,trs->m_forceArrayInResults);
    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_forceArrayFile);
    CPPUNIT_ASSERT_EQUAL(false,trs->m_speedArrayInResults);
    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_speedArrayFile);
    CPPUNIT_ASSERT_EQUAL(false,trs->m_updatePlcLamps);
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestLoadAdditionalConfigurationItems(void)
{
    cout << " TestLoadAdditionalConfigurationItems ";

    XmlParser parser;
    string configFile = m_configPath + "TestResultServerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    trs->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(
        string(TEST_RESULT_SERVER_NAME),
        document->getChild("Setup/Communication/Name")->getValue());
    CPPUNIT_ASSERT_EQUAL(getenv("FTP_ROOT") + string("/TestResults/Vehicle/"),
                         trs->m_testResultPath);
    CPPUNIT_ASSERT_EQUAL(string("/TestResults/Vehicle/"),
                         trs->m_testResultPublshedPath);
    CPPUNIT_ASSERT_EQUAL(string("TestResultIndex"),trs->m_testResultFile);
    CPPUNIT_ASSERT_EQUAL((INT16)1000,trs->m_numberOfTests);
    CPPUNIT_ASSERT_EQUAL(false,trs->m_clearBuildOnOverall);
    CPPUNIT_ASSERT_EQUAL(string("VIN1"),trs->GetVinTag());
    CPPUNIT_ASSERT_EQUAL(string("StartVehicleTest1"),trs->GetStartTestTag());
    CPPUNIT_ASSERT_EQUAL(string("AllFailures1"),trs->GetAllFailuresTag());
    CPPUNIT_ASSERT_EQUAL(string("OverallTest1"),trs->GetOverallTestTag());
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuild1"),trs->GetVehicleBuildTag());
    CPPUNIT_ASSERT_EQUAL(string("ClearScrollBox1"),trs->GetClearScrollBoxTag());
    CPPUNIT_ASSERT_EQUAL(string("DriveCurve1"),trs->GetDriveCurveTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumber1"),trs->GetDriverNumberTag());
    CPPUNIT_ASSERT_EQUAL(
        string("DriveCurve"),
        trs->m_vehiclebuildItems.getNode("DriveCurve")->getName());
    CPPUNIT_ASSERT_EQUAL(string("KeyPress"),trs->GetKeyPressTag());
    CPPUNIT_ASSERT_EQUAL(11,trs->GetVINLength());
    CPPUNIT_ASSERT_EQUAL(false,trs->m_includeAbortInAllFailures);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_forceArrayInResults);
    CPPUNIT_ASSERT_EQUAL(
        getenv("FTP_ROOT") + 
        string("/TestResults/Machine/BrakeForceData/BrakeForce1.log"),
        trs->m_forceArrayFile);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_speedArrayInResults);
    CPPUNIT_ASSERT_EQUAL(
        getenv("FTP_ROOT") + 
        string("/TestResults/Machine/BrakeForceData/Speed1.log"),
        trs->m_speedArrayFile);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_updatePlcLamps);
}

//-----------------------------------------------------------------------------
void 
TestResultServerTest::TestLoadAdditionalConfigurationItems_MissingTags(void)
{
    cout << " TestLoadAdditionalConfigurationItems_MissingTags ";

    XmlParser parser;
    string configFile = m_configPath + "TestResultServerConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    trs->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_testResultPath);
    CPPUNIT_ASSERT_EQUAL(string(""),trs->m_testResultFile);
    CPPUNIT_ASSERT_EQUAL((INT16)0,trs->m_numberOfTests);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_clearBuildOnOverall);
    CPPUNIT_ASSERT_EQUAL(string("VIN"),trs->GetVinTag());
    CPPUNIT_ASSERT_EQUAL(string("StartVehicleTest"),trs->GetStartTestTag());
    CPPUNIT_ASSERT_EQUAL(string("AllFailures"),trs->GetAllFailuresTag());
    CPPUNIT_ASSERT_EQUAL(string("OverallTest"),trs->GetOverallTestTag());
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuild"),trs->GetVehicleBuildTag());
    CPPUNIT_ASSERT_EQUAL(string("ClearScrollBox"),trs->GetClearScrollBoxTag());
    CPPUNIT_ASSERT_EQUAL(string("DriveCurve"),trs->GetDriveCurveTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumber"),trs->GetDriverNumberTag());

    // The node map should be empty.  So, trying to get a node should throw
    // an XmlException
    CPPUNIT_ASSERT_THROW(trs->m_vehiclebuildItems.getNode(0),XmlException);

    CPPUNIT_ASSERT_EQUAL((INT32)defaultVINLength,trs->GetVINLength());
    CPPUNIT_ASSERT_EQUAL(true,trs->m_includeAbortInAllFailures);
    CPPUNIT_ASSERT_EQUAL(false,trs->m_updatePlcLamps);
}

//-----------------------------------------------------------------------------
void 
TestResultServerTest::TestLoadAdditionalConfigurationItems_InvalidName(void)
{
    cout << " TestLoadAdditionalConfigurationItems_InvalidName ";

    XmlParser parser;
    string configFile = m_configPath + "TestResultServerConfigInvalidName.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    trs->LoadAdditionalConfigurationItems(document);

    CPPUNIT_ASSERT_EQUAL(
        string(TEST_RESULT_SERVER_NAME),
        document->getChild("Setup/Communication/Name")->getValue());
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestHandlePulse(void)
{
    cout << " TestHandlePulse ";

    XmlParser parser;
    string configFile = m_configPath + "TestResultServerConfigReload.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set a new configuration file so that we can make sure it was reloaded
    trs->SetConfigFile(configFile);

    // send the reload configuration pulse
    trs->HandlePulse(SYSTEM_PULSE,RELOAD_PULSE);

    // the test result server name is always the same
    CPPUNIT_ASSERT_EQUAL(
        string(TEST_RESULT_SERVER_NAME),
        document->getChild("Setup/Communication/Name")->getValue());
    CPPUNIT_ASSERT_EQUAL(getenv("FTP_ROOT") + string("/TestResults/Vehicle1/"),
                         trs->m_testResultPath);
    CPPUNIT_ASSERT_EQUAL(string("/TestResults/Vehicle1/"),
                         trs->m_testResultPublshedPath);
    CPPUNIT_ASSERT_EQUAL(string("TestResultIndex1"),trs->m_testResultFile);
    CPPUNIT_ASSERT_EQUAL((INT16)1001,trs->m_numberOfTests);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_clearBuildOnOverall);
    CPPUNIT_ASSERT_EQUAL(string("VIN2"),trs->GetVinTag());
    CPPUNIT_ASSERT_EQUAL(string("StartVehicleTest2"),trs->GetStartTestTag());
    CPPUNIT_ASSERT_EQUAL(string("AllFailures2"),trs->GetAllFailuresTag());
    CPPUNIT_ASSERT_EQUAL(string("OverallTest2"),trs->GetOverallTestTag());
    CPPUNIT_ASSERT_EQUAL(string("VehicleBuild2"),trs->GetVehicleBuildTag());
    CPPUNIT_ASSERT_EQUAL(string("ClearScrollBox2"),trs->GetClearScrollBoxTag());
    CPPUNIT_ASSERT_EQUAL(string("DriveCurve2"),trs->GetDriveCurveTag());
    CPPUNIT_ASSERT_EQUAL(string("DriverNumber2"),trs->GetDriverNumberTag());
    CPPUNIT_ASSERT_EQUAL(
        string("DriveCurve2"),
        trs->m_vehiclebuildItems.getNode("DriveCurve2")->getName());
    CPPUNIT_ASSERT_EQUAL(12,trs->GetVINLength());
    CPPUNIT_ASSERT_EQUAL(true,trs->m_includeAbortInAllFailures);
    CPPUNIT_ASSERT_EQUAL(true,trs->m_updatePlcLamps);
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestReportResults(void)
{
    cout << " TestReportResults ";

    m_testResult = new ITestResultServer();

    //Setup the member variables necessary to get known results
    trs->m_testResultPath = "";
    trs->m_testResultPublshedPath = "";
    trs->m_currentVIN = "TestVinTestResult";
    trs->m_lossCompensationVin = "Loss_Comp";
    trs->m_numberOfTests = 2;
    trs->m_vehicleTestResults.push_back("TestReport");

    //fill the subscribe list tags
    // Get the list of tags we will need to subscribe for
    XmlParser parser;
    string configFile = m_configPath + "TestResultServerConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    XmlNode *DataTagNode = const_cast<XmlNode *>(document->getChild("Setup/DataTags"));

    trs->SetVinTag(DataTagNode->getChild("Vin")->getValue());
    trs->SetStartTestTag(DataTagNode->getChild("StartTest")->getValue());
    trs->SetAllFailuresTag(DataTagNode->getChild("AllFailures")->getValue());
    trs->SetOverallTestTag(DataTagNode->getChild("OverallTest")->getValue());
    trs->SetVehicleBuildTag(DataTagNode->getChild("VehicleBuild")->getValue());
    trs->SetClearScrollBoxTag(DataTagNode->getChild("ClearScrollBox")->getValue());
    trs->SetDriveCurveTag(DataTagNode->getChild("DriveCurve")->getValue());
    trs->SetDriverNumberTag(DataTagNode->getChild("DriverNumber")->getValue());
    trs->SetKeyPressTag(DataTagNode->getChild("KeyPress")->getValue());

    //Sending A Fake Test Component Result just for something to do
    XmlNode myNode("BrakeTest","");
    m_testResult->SetResult("Pass",&myNode);
    trs->Write(&myNode);

    //There was a complaint where the keypress tag was being printed out
    //in the test result's file so we better attempt to send a key press
    //to see if she shows up in the output file
    XmlNode myNode1(DataTagNode->getChild("KeyPress")->getValue(),REPRINT_RESULTS);
    trs->Publish(&myNode1);

    //Here Goes the Big Shah Bang!!
    string outputedFile = trs->ReportResults();

    //Now we need to grab that file which was created and parse her to make sure we got
    //everything we expected
    const XmlNode *TestResultDoc = parser.ReturnXMLDocument(outputedFile);

    CPPUNIT_ASSERT_EQUAL(string(BEP_VEHICLE_TEST_FILE),string(TestResultDoc->getName()));

    XmlNode *childNode = const_cast<XmlNode *>(TestResultDoc->getChild(VEHICLE_BUILD_TAG));

    CPPUNIT_ASSERT_EQUAL(string(""),string(childNode->getValue()));

    childNode = const_cast<XmlNode *>(TestResultDoc->getChild(BEP_TEST_RESULT));
    XmlNode *nephewNode = const_cast<XmlNode *>(childNode->getChild("BrakeTest"));

    CPPUNIT_ASSERT_EQUAL(string("Pass"),string(nephewNode->getAttribute(BEP_RESULT)->getValue()));

    nephewNode = const_cast<XmlNode *>(childNode->getChild("DTC"));

    CPPUNIT_ASSERT_EQUAL(string(""),string(nephewNode->getValue()));

    //Should clean up and delete that file which was created
    remove(outputedFile.c_str());

    delete m_testResult;
}
//-----------------------------------------------------------------------------
void TestResultServerTest::TestInitialize(void)
{
    cout << " TestInitialize ";

    //Setup the member variables necessary to get known results
    trs->m_testResultPath = getenv("TEST_DIR_PATH");
    trs->m_testResultPath += "/Core/Unit/ServicesLayer/Servers/TestResultServer/";
    trs->m_testResultFile = "SampleTestResultFile";
    trs->m_numberOfTests = 200;

    trs->OpenAndReadInResultsFile();

    //There was a complaint that the test results index was being read in
    //without adding the previous test names unless it was a previous
    //loss comp test result name, The SampleTestResultFile was taken
    //from the pit machine with 103 tests recorded on it
    CPPUNIT_ASSERT_EQUAL(103,(int)trs->m_vehicleTestResults.size());
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestPublish_PlcLampsCleared(void)
{
    cout << " TestPublish_PlcLampsCleared ";

    string retval=BEP_FAILURE_RESPONSE;

    // create the xml node to publish
    const XmlNode startTest("StartVehicleTest","1",NULL,TEXT_NODE);

    // set the member variables for the case we want to test
    trs->m_startTestTag = "StartVehicleTest";
    trs->m_updatePlcLamps = true;

    // publish the node
    retval = trs->Publish(&startTest);

    // make sure the operation was successful
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);

    // make sure the accept lamp was cleared
    CPPUNIT_ASSERT_EQUAL(string("0"),trs->GetData("AcceptLamp"));

    // make sure the fault lamp was cleared
    CPPUNIT_ASSERT_EQUAL(string("0"),trs->GetData("FaultLamp"));
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestPublish_PlcLampsTestPass(void)
{
    cout << " TestPublish_PlcLampsTestPass ";

    string retval=BEP_FAILURE_RESPONSE;

    // create the xml node to publish
    const XmlNode overallTest("OverallTest",BEP_PASS_RESPONSE,NULL,TEXT_NODE);

    // set the member variables for the case we want to test
    trs->m_overallTestTag = "OverallTest";
    trs->m_updatePlcLamps = true;
    trs->m_clearResultsOnOverall = false;

    // the end of test processing requires a valid m_comm object
    trs->m_comm = new ITestResultServerThreadPool(trs);

    // publish the node
    retval = trs->Publish(&overallTest);

    // make sure the operation was successful
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);

    // make sure the accept lamp is on
    CPPUNIT_ASSERT_EQUAL(string("1"),trs->GetData("AcceptLamp"));

    // make sure the fault lamp is off
    CPPUNIT_ASSERT_EQUAL(string("0"),trs->GetData("FaultLamp"));
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestPublish_PlcLampsTestFail(void)
{
    cout << " TestPublish_PlcLampsTestFail ";

    string retval=BEP_FAILURE_RESPONSE;

    // create the xml node to publish
    const XmlNode overallTest("OverallTest",BEP_FAIL_RESPONSE,NULL,TEXT_NODE);

    // set the member variables for the case we want to test
    trs->m_overallTestTag = "OverallTest";
    trs->m_updatePlcLamps = true;
    trs->m_clearResultsOnOverall = false;

    // the end of test processing requires a valid m_comm object
    trs->m_comm = new ITestResultServerThreadPool(trs);

    // publish the node
    retval = trs->Publish(&overallTest);

    // make sure the operation was successful
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);

    // make sure the accept lamp is off
    CPPUNIT_ASSERT_EQUAL(string("0"),trs->GetData("AcceptLamp"));

    // make sure the fault lamp is on
    CPPUNIT_ASSERT_EQUAL(string("1"),trs->GetData("FaultLamp"));
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestPublish_TestResultsCleared(void)
{
    cout << " TestPublish_TestResultsCleared ";

    string retval=BEP_FAILURE_RESPONSE;

    // create the xml node to publish
    const XmlNode overallTest("OverallTest",BEP_PASS_RESPONSE,NULL,TEXT_NODE);

    // set the member variables for the case we want to test
    trs->m_overallTestTag = "OverallTest";

    // the end of test processing requires a valid m_comm object
    trs->m_comm = new ITestResultServerThreadPool(trs);

    // publish the node
    retval = trs->Publish(&overallTest);

    // make sure the operation was successful
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);

    // make sure the results were cleared
    CPPUNIT_ASSERT_EQUAL((size_t)0,trs->m_data.size());
}

//-----------------------------------------------------------------------------
void TestResultServerTest::TestPublish_RelayFile(void)
{
    cout << " TestPublish_RelayFile ";

    string retval=BEP_FAILURE_RESPONSE;

    // create the xml node to publish
    const XmlNode relayFile("RelayFile","TestResultFile.xml");

    // publish the node
    retval = trs->Publish(&relayFile);

    // make sure the operation was successful
    CPPUNIT_ASSERT_EQUAL(string(BEP_SUCCESS_RESPONSE),retval);

    // make sure the test result filename was written correctly
    CPPUNIT_ASSERT_EQUAL(string("TestResultFile.xml"),
                         trs->GetData(TEST_RESULT_TAG));
}
