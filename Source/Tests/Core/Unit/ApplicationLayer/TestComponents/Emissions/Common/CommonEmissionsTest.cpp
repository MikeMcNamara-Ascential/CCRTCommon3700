//******************************************************************************
// Description:
//   Test class for testing the Emissions/Common class.
//==============================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Emissions/Common/CommonEmissionsTest.cpp $
// 
// 2     5/01/07 1:22p Rwiersem
// Changes for the 07050201 core release:
// 
// Added printing of the current test name to the screen.  Added
// StartHydraulicCoolingFanTest_StartModuleKeepAlive().  Added
// StopHydraulicCoolingFanTest_StopModuleKeepAlive().  Added
// DetermineModuleConfig() tests.
// 
// 1     1/18/07 4:12p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/dispatch.h>
#include <sys/resource.h>
#include <malloc.h>
#include "CommonEmissionsTest.h"
#include "GenericEmissionsTCTemplate.cpp"
#include "MockProtocolFilter.h"
#include "MockTestResultServer.h"
#include "MockNamedDataBroker.h"
#include "MockGenericEmissionsModuleTemplate.cpp"

//=============================================================================
CommonEmissionsTest::CommonEmissionsTest(void)
{
}

//=============================================================================
CommonEmissionsTest::~CommonEmissionsTest(void)
{
}

//=============================================================================
void CommonEmissionsTest::setUp()
{
    // Set up the path's we'll neeed
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to top testing";
        cerr << "dir, ex: ~/Tests/" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configPath = topTestDir +
        "/Core/Unit/ApplicationLayer/TestComponents/Emissions/" +
        "Common/Configuration/";

    // register the name "core/MotorController" so GenericEmissionsTCTemplate 
    // can create a mock MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");

    // register the name "core/NamedDataBroker" so GenericEmissionsTCTemplate
    // can create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");
}

//=============================================================================
void CommonEmissionsTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);

    // move to the next line for the next test name
    cout << endl;
}

//=============================================================================
void CommonEmissionsTest::ConstructorTest(void)
{
    cout << " ConstructorTest ";

    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    CPPUNIT_ASSERT_EQUAL(string("5.7L V8"),gett.engine57LV8);
    CPPUNIT_ASSERT_EQUAL(string("4.7L V8"),gett.engine47LV8);
    CPPUNIT_ASSERT_EQUAL(string("3.7L V6"),gett.engine37LV6);
    CPPUNIT_ASSERT_EQUAL(false,gett.m_isPreviousPass);
    CPPUNIT_ASSERT_EQUAL(false,gett.m_etcVehicle);
    CPPUNIT_ASSERT_EQUAL(string("Unknown"),gett.m_engineSize);
    CPPUNIT_ASSERT_EQUAL(string("Unknown"),gett.m_vehicleType);
    CPPUNIT_ASSERT_EQUAL(string("Unknown"),gett.m_emissionsType);
    CPPUNIT_ASSERT_EQUAL(string("Unknown"),gett.m_emissionsYear);
    CPPUNIT_ASSERT_EQUAL(string("Unknown"),gett.m_moduleType);
    CPPUNIT_ASSERT_EQUAL((UINT16)0,gett.m_currentOdometerReading);
    CPPUNIT_ASSERT_EQUAL(false,gett.m_boolTestRedundantBrakeSwitch);
    CPPUNIT_ASSERT_EQUAL(false,gett.m_boolTestClutchUpstopSwitch);
}

//=============================================================================
void CommonEmissionsTest::StartHydraulicCoolingFanTest_Pass(void)
{
    cout << " StartHydraulicCoolingFanTest_Pass ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(testPass),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StartHydraulicCoolingFanTest_Skip(void)
{
    cout << " StartHydraulicCoolingFanTest_Skip ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set a non-matching engine size
    gett.m_engineSize = "5.1";

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test was skipped
    CPPUNIT_ASSERT_EQUAL(string(testSkip),retVal);
}

//=============================================================================
void CommonEmissionsTest::StartHydraulicCoolingFanTest_FailNoAbort(void)
{
    cout << " StartHydraulicCoolingFanTest_FailNoAbort ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // set the parameter that says not to abort
    XmlNode noAbortNode("NoAbortForHydraulicCoolingFan","1");
    gett.m_parameters.addNode(&noAbortNode);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/StartHydraulicCoolingFanFailed"));

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAIL_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0001"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StartHydraulicCoolingFanTest_FailAbort(void)
{
    cout << " StartHydraulicCoolingFanTest_FailAbort ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // set the parameter that says not to abort
    XmlNode noAbortNode("NoAbortForHydraulicCoolingFan","0");
    gett.m_parameters.addNode(&noAbortNode);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/StartHydraulicCoolingFanFailed"));

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test aborted
    CPPUNIT_ASSERT_EQUAL(string(testAbort),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_ABORT_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0001"),mtrs->m_writeTestResultParamCode[0]);

    // make sure the abort flag was written
    CPPUNIT_ASSERT_EQUAL(string(ABORT_DATA_TAG),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("1"),mndb->m_writeParamValue[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StartHydraulicCoolingFanTest_ModuleException(void)
{
    cout << " StartHydraulicCoolingFanTest_ModuleException ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    mpf->m_getModuleDataShouldThrow = true;

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/SoftwareFailure"));

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test returned softwareFail
    CPPUNIT_ASSERT_EQUAL(string(testSoftwareFail),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_RESPONSE),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Software Failure"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0002"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void 
CommonEmissionsTest::StartHydraulicCoolingFanTest_StartModuleKeepAlive(void)
{
    cout << " StartHydraulicCoolingFanTest_StartModuleKeepAlive ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the conditions needed to start the keep alive thread
    gett.SetRetestStatus(true);
    gett.SetPreviousOverallResult(testPass);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StartHydraulicCoolingFan");

    // call the method under test
    retVal = gett.StartHydraulicCoolingFan();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(testPass),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);

    // make sure the keep alive thread was started
    CPPUNIT_ASSERT_EQUAL(1,gett.m_vehicleModule.m_startModuleKeepAliveCalled);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StopHydraulicCoolingFanTest_Pass(void)
{
    cout << " StopHydraulicCoolingFanTest_Pass ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StopHydraulicCoolingFan");

    // call the method under test
    retVal = gett.StopHydraulicCoolingFan();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(testPass),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StopHydraulicCoolingFanTest_Skip(void)
{
    cout << " StopHydraulicCoolingFanTest_Skip ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set a non-matching engine size
    gett.m_engineSize = "5.1";

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StopHydraulicCoolingFan");

    // call the method under test
    retVal = gett.StopHydraulicCoolingFan();

    // make sure the test was skipped
    CPPUNIT_ASSERT_EQUAL(string(testSkip),retVal);
}

//=============================================================================
void CommonEmissionsTest::StopHydraulicCoolingFanTest_Fail(void)
{
    cout << " StopHydraulicCoolingFanTest_Fail ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StopHydraulicCoolingFan");

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/StopHydraulicCoolingFanFailed"));

    // call the method under test
    retVal = gett.StopHydraulicCoolingFan();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAIL_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0003"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StopHydraulicCoolingFanTest_ModuleException(void)
{
    cout << " StopHydraulicCoolingFanTest_ModuleException ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    mpf->m_getModuleDataShouldThrow = true;

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StopHydraulicCoolingFan");

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/SoftwareFailure"));

    // call the method under test
    retVal = gett.StopHydraulicCoolingFan();

    // make sure the test returned softwareFail
    CPPUNIT_ASSERT_EQUAL(string(testSoftwareFail),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_RESPONSE),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Software Failure"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0002"),mtrs->m_writeTestResultParamCode[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::StopHydraulicCoolingFanTest_StopModuleKeepAlive(void)
{
    cout << " StopHydraulicCoolingFanTest_StopModuleKeepAlive ";

    std::string retVal;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the parameter to always run the cooling fan
    XmlNode runFanNode("AlwaysRunCoolingFan","1");
    gett.m_parameters.addNode(&runFanNode);

    // set the parameter that tells we need to run the cooling fan
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // set the data needed by the MockProtocolFilter
    MockProtocolFilter *mpf = 
        (MockProtocolFilter*)gett.m_vehicleModule.m_protocolFilter;
    uint8_t temp[] = {0x30};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/StopHydraulicCoolingFan");

    // set the conditions needed to stop the keep alive thread
    gett.SetRetestStatus(true);
    gett.SetPreviousOverallResult(testPass);

    // call the method under test
    retVal = gett.StopHydraulicCoolingFan();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(testPass),retVal);

    // make sure the correct message was sent
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mpf->m_getModuleDataParamMessageTag[0]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("StopHydraulicCoolingFan Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);

    // make sure the keep alive thread was stopped
    CPPUNIT_ASSERT_EQUAL(1,gett.m_vehicleModule.m_stopModuleKeepAliveCalled);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::NeedToRunHydraulicCoolingFanTest_True(void)
{
    cout << " NeedToRunHydraulicCoolingFanTest_True ";

    bool retVal=false;
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // add the parameter node
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set the matching engine size
    gett.m_engineSize = "5.7";

    // call the method under test
    retVal = gett.NeedToRunHydraulicCoolingFan();

    CPPUNIT_ASSERT_EQUAL(true,retVal);
}

//=============================================================================
void CommonEmissionsTest::NeedToRunHydraulicCoolingFanTest_False(void)
{
    cout << " NeedToRunHydraulicCoolingFanTest_False ";

    bool retVal=true;
    GenericEmissionsTCTemplate
        <GenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // add the parameter node
    XmlNode engineSizeNode("EngineSizeForHydraulicCoolingFanTest","5.7");
    gett.m_parameters.addNode(&engineSizeNode);

    // set a non-matching engine size
    gett.m_engineSize = "5.1";

    // call the method under test
    retVal = gett.NeedToRunHydraulicCoolingFan();

    CPPUNIT_ASSERT_EQUAL(false,retVal);
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_Pass(void)
{
    cout << " DetermineModuleConfigTest_Pass ";

    string retVal=testFail;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the mock generic emissions module data
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("5.7");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestVeh");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("07");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestEmiss");
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(testPass),retVal);

    // make sure getInfo() was called the correct number of times
    CPPUNIT_ASSERT_EQUAL(4,gett.m_vehicleModule.m_getInfoCalled);

    // make sure the engine size was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EngineConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[0]);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),gett.m_engineSize);

    // make sure the vehicle type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[1]);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),gett.m_vehicleType);

    // make sure the emissions module year was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[2]);
    CPPUNIT_ASSERT_EQUAL(string("07"),gett.m_emissionsYear);

    // make sure the emissions type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[3]);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),gett.m_emissionsType);

    // make sure the test result was written correctly
    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    CPPUNIT_ASSERT_EQUAL(string(testPass),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("DetermineModuleConfig Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);

    // get the first detail and check its values
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("07"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the second detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the third detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EngineType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the fourth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // make sure the engine size was written
    CPPUNIT_ASSERT_EQUAL(string("EngineSize"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),mndb->m_writeParamValue[0]);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_EngineSizeFail(void)
{
    cout << " DetermineModuleConfigTest_EngineSizeFail ";

    string retVal=testPass;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the mock generic emissions module data
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("5.7");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestVeh");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("07");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestEmiss");
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_FAILURE);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/CommunicationFailure"));

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure getInfo() was called the correct number of times
    CPPUNIT_ASSERT_EQUAL(4,gett.m_vehicleModule.m_getInfoCalled);

    // make sure the vehicle type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[1]);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),gett.m_vehicleType);

    // make sure the emissions module year was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[2]);
    CPPUNIT_ASSERT_EQUAL(string("07"),gett.m_emissionsYear);

    // make sure the emissions type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[3]);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),gett.m_emissionsType);

    // make sure the test result was written correctly
    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    CPPUNIT_ASSERT_EQUAL(string(testFail),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Communication Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0004"),mtrs->m_writeTestResultParamCode[0]);

    // get the first detail and check its values
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("07"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the second detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the third detail 
    // we don't need to check its values since it failed
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);

    // get the fourth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_VehicleTypeFail(void)
{
    cout << " DetermineModuleConfigTest_VehicleTypeFail ";

    string retVal=testPass;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the mock generic emissions module data
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("5.7");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestVeh");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("07");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestEmiss");
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_FAILURE);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/CommunicationFailure"));

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure getInfo() was called the correct number of times
    CPPUNIT_ASSERT_EQUAL(4,gett.m_vehicleModule.m_getInfoCalled);

    // make sure the engine size was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EngineConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[0]);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),gett.m_engineSize);

    // make sure the emissions module year was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[2]);
    CPPUNIT_ASSERT_EQUAL(string("07"),gett.m_emissionsYear);

    // make sure the emissions type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[3]);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),gett.m_emissionsType);

    // make sure the test result was written correctly
    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    CPPUNIT_ASSERT_EQUAL(string(testFail),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Communication Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0004"),mtrs->m_writeTestResultParamCode[0]);

    // get the first detail and check its values
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("07"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the second detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the third detail and check its values 
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EngineType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the fourth detail
    // we don't need to check its values since it failed
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_EmissionsYearFail(void)
{
    cout << " DetermineModuleConfigTest_EmissionsYearFail ";

    string retVal=testPass;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the mock generic emissions module data
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("5.7");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestVeh");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("07");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestEmiss");
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_FAILURE);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/CommunicationFailure"));

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure getInfo() was called the correct number of times
    CPPUNIT_ASSERT_EQUAL(4,gett.m_vehicleModule.m_getInfoCalled);

    // make sure the engine size was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EngineConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[0]);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),gett.m_engineSize);

    // make sure the vehicle type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[1]);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),gett.m_vehicleType);

    // make sure the emissions type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[3]);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),gett.m_emissionsType);

    // make sure the test result was written correctly
    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    CPPUNIT_ASSERT_EQUAL(string(testFail),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Communication Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0004"),mtrs->m_writeTestResultParamCode[0]);

    // get the first detail
    // we don't need to check its values since it failed
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);

    // get the second detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestEmiss"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the third detail and check its values 
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EngineType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the fourth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_EmissionsTypeFail(void)
{
    cout << " DetermineModuleConfigTest_EmissionsTypeFail ";

    string retVal=testPass;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the mock generic emissions module data
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("5.7");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestVeh");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("07");
    gett.m_vehicleModule.m_getInfoReturnDataValueString.push_back("TestEmiss");
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_SUCCESS);
    gett.m_vehicleModule.m_getInfoReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the data needed by the MockTestResultServer
    gett.m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gett.m_testResult;
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the data needed by the MockNamedDataBroker
    gett.m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gett.m_ndb;
    mndb->m_writeReturnResponse.push_back("Pass");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the fault list
    gett.m_faults.addNode(
        document->getChild("Setup/FaultList/CommunicationFailure"));

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test failed
    CPPUNIT_ASSERT_EQUAL(string(testFail),retVal);

    // make sure getInfo() was called the correct number of times
    CPPUNIT_ASSERT_EQUAL(4,gett.m_vehicleModule.m_getInfoCalled);

    // make sure the engine size was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EngineConfiguration"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[0]);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),gett.m_engineSize);

    // make sure the vehicle type was read correctly
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[1]);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),gett.m_vehicleType);

    // make sure the emissions module year was read correctly
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),
                         gett.m_vehicleModule.m_getInfoParamMethodName[2]);
    CPPUNIT_ASSERT_EQUAL(string("07"),gett.m_emissionsYear);

    // make sure the test result was written correctly
    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    CPPUNIT_ASSERT_EQUAL(string(testFail),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Communication Failed"),
                         mtrs->m_writeTestResultParamDescription[0]);
    CPPUNIT_ASSERT_EQUAL(string("0004"),mtrs->m_writeTestResultParamCode[0]);

    // get the first detail and check its values
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("EmissionsModelYear"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("07"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the second detail and check its values
    // we don't need to check its values since it failed
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);

    // get the third detail and check its values 
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("EngineType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("5.7"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the fourth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("VehicleType"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("TestVeh"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // delete the MockTestResultServer
    delete gett.m_testResult;
    gett.m_testResult = NULL;
}

//=============================================================================
void CommonEmissionsTest::DetermineModuleConfigTest_TestSkip(void)
{
    cout << " DetermineModuleConfigTest_TestSkip ";

    string retVal=testPass;
    XmlParser parser;
    string configFile = m_configPath + "CommonEmissionsConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    GenericEmissionsTCTemplate
        <MockGenericEmissionsModuleTemplate<MockProtocolFilter> > gett;

    // set the current test step
    gett.m_currentTestStep = 
        document->getChild("Setup/TestSteps/DetermineModuleConfig");

    // set the current test step result to pass
    gett.SetTestStepResult(testPass);

    // call the method under test
    retVal = gett.DetermineModuleConfig();

    // make sure the test was skipped
    CPPUNIT_ASSERT_EQUAL(string(testSkip),retVal);
}
