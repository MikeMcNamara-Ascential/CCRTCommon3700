//******************************************************************************
// Description:
//   Test class for testing the Machine/Common class.
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Machine/Common/CommonMachineTest.cpp $
// 
// 4     5/01/07 1:25p Rwiersem
// Changes for the 07050201 core release:
// 
// Added DriverNumberNotComplete tests.
// 
// 3     1/18/07 4:17p Rwiersem
// Changes for the 07011901 core release:
// 
// - Updated to use the common MockPromptServer.
// - Added TestStepInitializeTest(), TestStepInitializeTest_Timeout(),
// TestStepInitializeTest_Fail(), TestStepInitializeTest_Abort(),
// TestStepInitializeTest_ExceptionThrown(), and
// CheckTestStepInitializeTestResult().
// 
// 2     10/26/06 1:52p Rwiersem
// Changes for the 06102701 core release:
// 
// Added a MockTestResultServer object.
// 
// 1     9/06/06 10:17a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
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
#include "CommonMachineTest.h"
#include "MachineTC.h"
#include "MockPromptServer.h"
#include "MockTestResultServer.h"
#include "MockNamedDataBroker.h"

//=============================================================================
CommonMachineTest::CommonMachineTest(void)
{
}

//=============================================================================
CommonMachineTest::~CommonMachineTest(void)
{
}

//=============================================================================
void CommonMachineTest::setUp()
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
    m_configDir = topTestDir +
        "/Core/Unit/ApplicationLayer/TestComponents/Machine/" +
        "Common/Configuration/";

    // register the name "core/MotorController" so machineTC can
    // create a mock MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");

    // register the name "core/NamedDataBroker" so machineTC can
    // create a mock NamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    // create the test object
    mtc = new MachineTC();

    // create a mock test result server object
    mtc->m_testResult = new MockTestResultServer();
    mtrs = (MockTestResultServer*)mtc->m_testResult;

    // create a mock prompt server object
    mtc->m_prompt = new MockPromptServer();
    mps = (MockPromptServer*)mtc->m_prompt;
}

//=============================================================================
void CommonMachineTest::tearDown()
{
    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);

    // delete the mock prompt server object
    delete mtc->m_prompt;
    mtc->m_prompt = NULL;

    // delete the mock test result object
    delete mtc->m_testResult;
    mtc->m_testResult = NULL;

    delete mtc;
}

//=============================================================================
void CommonMachineTest::ConstructorTest(void)
{
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         mtc->m_semKeyPress.CriticalSectionTryEnter());
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         mtc->m_semDriverNumber.CriticalSectionTryEnter());
                         
    CPPUNIT_ASSERT_EQUAL(false,mtc->m_requestKeyPressInput);
    CPPUNIT_ASSERT_EQUAL(false,mtc->m_requestDriverNumber);
}

//=============================================================================
void CommonMachineTest::TestStepInitializeTest(void)
{
    std::string retVal;

    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Initialize");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // set the machine number and software version
    mtc->SetData("MachineNumber","1234");
    mtc->SetData("SoftwareID","5678");

    // set the data needed by the mock prompt server
    for(int ii=0; ii<2; ii++)
    {
        mps->m_clearPromptBoxResponse.push_back("PASS");
        mps->m_clearPromptBoxReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = mtc->TestStepInitialize("Test");

    // make sure the test step passed
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_RESPONSE),retVal);

    // make sure both prompt boxes were cleared
    CPPUNIT_ASSERT_EQUAL(2,mps->m_clearPromptBoxCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_clearPromptBoxParamPromptBox[1]);

    // check the written test results
    CheckTestStepInitializeTestResult("Pass");
}

//=============================================================================
void CommonMachineTest::TestStepInitializeTest_Timeout(void)
{
    std::string retVal;
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Initialize");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // set the machine number and software version
    mtc->SetData("MachineNumber","1234");
    mtc->SetData("SoftwareID","5678");

    // set the data needed by the mock prompt server
    mps->m_clearPromptBoxResponse.push_back("Timeout");
    mps->m_clearPromptBoxReturnValue.push_back((INT32)BEP_STATUS_TIMEOUT);

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = mtc->TestStepInitialize("Test");

    // make sure the test step returned timeout
    CPPUNIT_ASSERT_EQUAL(string(BEP_TIMEOUT_RESPONSE),retVal);

    // make sure ClearPromptBox() was only called once
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxParamPromptBox[0]);

    // check the written test results
    CheckTestStepInitializeTestResult("Timeout");
}

//=============================================================================
void CommonMachineTest::TestStepInitializeTest_Fail(void)
{
    std::string retVal;
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Initialize");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // set the machine number and software version
    mtc->SetData("MachineNumber","1234");
    mtc->SetData("SoftwareID","5678");

    // set the data needed by the mock prompt server
    mps->m_clearPromptBoxResponse.push_back("Fail");
    mps->m_clearPromptBoxReturnValue.push_back((INT32)BEP_STATUS_FAILURE);

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = mtc->TestStepInitialize("Test");

    // make sure the test step returned timeout
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAILURE_RESPONSE),retVal);

    // make sure ClearPromptBox() was only called once
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxParamPromptBox[0]);

    // check the written test results
    CheckTestStepInitializeTestResult("Failure");
}

//=============================================================================
void CommonMachineTest::TestStepInitializeTest_Abort(void)
{
    std::string retVal;
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Initialize");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // set the machine number and software version
    mtc->SetData("MachineNumber","1234");
    mtc->SetData("SoftwareID","5678");

    // set the data needed by the mock prompt server
    mps->m_clearPromptBoxResponse.push_back("Pass");
    mps->m_clearPromptBoxReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_clearPromptBoxResponse.push_back("Pass");
    mps->m_clearPromptBoxReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the status to abort
    mtc->SetStatus(BEP_ABORT_STATUS);

    // call the method under test
    retVal = mtc->TestStepInitialize("Test");

    // make sure the test step returned timeout
    CPPUNIT_ASSERT_EQUAL(string(BEP_ABORT_RESPONSE),retVal);

    // make sure both prompt boxes were cleared
    CPPUNIT_ASSERT_EQUAL(2,mps->m_clearPromptBoxCalled);
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(2,mps->m_clearPromptBoxParamPromptBox[1]);

    // check the written test results
    CheckTestStepInitializeTestResult("Abort");
}

//=============================================================================
void CommonMachineTest::TestStepInitializeTest_ExceptionThrown(void)
{
    std::string retVal;
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Initialize");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // set the machine number and software version
    mtc->SetData("MachineNumber","1234");
    mtc->SetData("SoftwareID","5678");

    // set the data needed by the mock prompt server
    mps->m_clearPromptBoxShouldThrow = true;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    retVal = mtc->TestStepInitialize("Test");

    // make sure the test step returned timeout
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAILURE_RESPONSE),retVal);

    // make sure the first prompt box was told to clear
    CPPUNIT_ASSERT_EQUAL(1,mps->m_clearPromptBoxParamPromptBox[0]);

    // check the written test results
    CheckTestStepInitializeTestResult("Failure");
}

//=============================================================================
void CommonMachineTest::CheckTestStepInitializeTestResult(std::string result)
{
    // make sure WriteTestResult() was only called once
    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    // make sure the passed in parameters were stored correctly
    CPPUNIT_ASSERT_EQUAL(string("Initialize"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(result,mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Initialize the machine for testing"),
                         mtrs->m_writeTestResultParamDescription[0]);

    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    // get the Date detail and check its values
    time_t currentTime = time(NULL);
    struct tm *localTime = localtime(&currentTime);
    char testDate[256];
    char testTime[256];
    string dateFormat("%F");

    strftime(testDate,sizeof(testDate),dateFormat.c_str(),localTime);
    strftime(testTime,sizeof(testTime),"%H",localTime);

    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("Date"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string(testDate),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the Time detail and check its values
    // NOTE:  This test could fail if the hour changes between the time the
    //        data is stored and evaluated.
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("Time"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string(testTime),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the MachineNumber detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("MachineNumber"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("1234"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // get the SoftwareVersion detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("Version"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("5678"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string(""),detail.detailUnits);

    // check the faultcode parameter
    CPPUNIT_ASSERT_EQUAL(string("0000"),mtrs->m_writeTestResultParamCode[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverseTest(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_FAIL_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // set the data tags
    const XmlNode *dataTags = document->getChild("Setup/DataTags");
    mtc->m_dataTags.DeepCopy(dataTags->getChildren());

    // set the parameters
    const XmlNode *parameters = document->getChild("Setup/Parameters");
    mtc->m_parameters = (XmlNodeMap)(parameters->getChildren());

    // create the mock objects
    mtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)mtc->m_ndb;

    // set the named data broker values
    for(int ii=0;ii<3;ii++)
    {
        mndb->m_readReturnResponse.push_back("PASS");
        mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

        mndb->m_getByTagReturnMessage.push_back("PASS");
        mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }
    
    mndb->m_getByTagReturnDataValue.push_back("0");    // FourWheelDriveEquipped
    mndb->m_getByTagReturnDataValue.push_back("-6");   // LFSpeed
    mndb->m_getByTagReturnDataValue.push_back("-6");   // RFSpeed

    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock prompt server
    for(int ii=0;ii<2;ii++)
    {
        mps->m_displayPromptResponse.push_back("Pass");
        mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
        mps->m_removePromptResponse.push_back("Pass");
        mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }
    
    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure the test passed
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),status);

    // make sure DisplayPrompt() was called twice and the values were correct
    CPPUNIT_ASSERT_EQUAL(2,mps->m_displayPromptCalled);
    CPPUNIT_ASSERT_EQUAL(string("ShiftToReverse"),
                         mps->m_displayPromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(string("AccelerateToReverseTargetSpeed"),
                         mps->m_displayPromptParamPrompt[1]);
    CPPUNIT_ASSERT_EQUAL(INT32(1),mps->m_displayPromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(INT32(1),mps->m_displayPromptParamPromptBox[1]);
    CPPUNIT_ASSERT_EQUAL(INT32(0),mps->m_displayPromptParamPriorityLevel[0]);
    CPPUNIT_ASSERT_EQUAL(INT32(0),mps->m_displayPromptParamPriorityLevel[1]);
    
    // make sure RemovePrompt() was called twice and the values were correct
    CPPUNIT_ASSERT_EQUAL(2,mps->m_removePromptCalled);
    CPPUNIT_ASSERT_EQUAL(string("ShiftToReverse"),
                         mps->m_removePromptParamPrompt[0]);
    CPPUNIT_ASSERT_EQUAL(string("AccelerateToReverseTargetSpeed"),
                         mps->m_removePromptParamPrompt[1]);
    CPPUNIT_ASSERT_EQUAL(INT32(1),mps->m_removePromptParamPromptBox[0]);
    CPPUNIT_ASSERT_EQUAL(INT32(1),mps->m_removePromptParamPromptBox[1]);
    CPPUNIT_ASSERT_EQUAL(INT32(0),mps->m_removePromptParamPriorityLevel[0]);
    CPPUNIT_ASSERT_EQUAL(INT32(0),mps->m_removePromptParamPriorityLevel[1]);

    // make sure the correct name data broker tags and values were written
    CPPUNIT_ASSERT_EQUAL(string("SpeedTarget"),mndb->m_writeParamTag[0]);
    CPPUNIT_ASSERT_EQUAL(string("5 10"),mndb->m_writeParamValue[0]);
    CPPUNIT_ASSERT_EQUAL(string("SpeedTarget"),mndb->m_writeParamTag[1]);
    CPPUNIT_ASSERT_EQUAL(string("0 0"),mndb->m_writeParamValue[1]);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_PASS_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverse_FirstStatusCheckFail(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_FAIL_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the status to abort
    mtc->SetStatus(BEP_ABORT_STATUS);

    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure "TestNotStarted" was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_TEST_NOT_STARTED),status);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_TEST_NOT_STARTED),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverse_SystemWriteFail(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_PASS_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // set the mock objects
    mtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)mtc->m_ndb;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the SystemWrite() return status
    mndb->m_writeReturnResponse.push_back("FAIL");
    mndb->m_writeReturnResponse.push_back("FAIL");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_FAILURE);
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_FAILURE);

    // store the data needed by the mock prompt server
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure "SoftwareFail" was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_STATUS),status);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverse_SecondStatusCheckFail(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_PASS_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // set the status check items
    const XmlNode *statusCheck = document->getChild("Setup/StatusCheckItems");
    mtc->m_statusCheckItems.DeepCopy(statusCheck->getChildren());

    // set the mock objects
    mtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)mtc->m_ndb;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the named data broker values
    for(int ii=0;ii<11;ii++)
    {
        mndb->m_readReturnResponse.push_back("PASS");
        mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

        mndb->m_getByTagReturnMessage.push_back("PASS");
        mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }

    mndb->m_getByTagReturnDataValue.push_back("1");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // FourWheelDriveEquipped
    mndb->m_getByTagReturnDataValue.push_back("-6");   // LFSpeed
    mndb->m_getByTagReturnDataValue.push_back("-6");   // RFSpeed
    mndb->m_getByTagReturnDataValue.push_back("1");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown
    mndb->m_getByTagReturnDataValue.push_back("0");    // RollsUpElevDown

    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock prompt server
    for(int ii=0; ii<2; ii++)
    {
        mps->m_displayPromptResponse.push_back("Pass");
        mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
        mps->m_removePromptResponse.push_back("Pass");
        mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }

    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure "Fail" was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAIL_STATUS),status);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_FAIL_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverse_Timeout(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponentTimeout.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_PASS_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // set the mock objects
    mtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)mtc->m_ndb;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the named data broker values
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getByTagReturnDataValue.push_back("0");    // FourWheelDriveEquipped
    mndb->m_getByTagReturnDataValue.push_back("-6");   // LFSpeed
    mndb->m_getByTagReturnDataValue.push_back("-6");   // RFSpeed
    mndb->m_getByTagDelay = 1;    // this is used to create elapsed time
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock prompt server
    for(int ii=0; ii<2; ii++)
    {
        mps->m_displayPromptResponse.push_back("Pass");
        mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
        mps->m_removePromptResponse.push_back("Pass");
        mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    }

    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure "Timeout" was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_TIMEOUT_STATUS),status);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_TIMEOUT_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::TestStepReverse_ExceptionThrown(void)
{
    XmlParser parser;
    string configFile = m_configDir + "MachineComponent.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    std::string status=BEP_PASS_STATUS;

    // set the current test step
    mtc->m_currentTestStep = document->getChild("TestSteps/Reverse");

    // set the mock objects
    mtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)mtc->m_ndb;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the named data broker return value
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    mndb->m_getByTagReturnDataValue.push_back("0");    // FourWheelDriveEquipped
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_FAILURE);

    mndb->m_writeReturnResponse.push_back("PASS");
    mndb->m_writeReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // store the data needed by the mock prompt server
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_displayPromptResponse.push_back("Pass");
    mps->m_displayPromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mps->m_removePromptResponse.push_back("Pass");
    mps->m_removePromptReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // call the method under test
    status = mtc->TestStepReverse("Test");

    // make sure "SoftwareFail" was returned
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_STATUS),status);

    // make sure the test result was written correctly
    CPPUNIT_ASSERT_EQUAL(string("Reverse"),mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string(BEP_SOFTWAREFAIL_STATUS),
                         mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("Reverse Test Step"),
                         mtrs->m_writeTestResultParamDescription[0]);
}

//=============================================================================
void CommonMachineTest::DriverNumberNotCompleteTest_DefaultDriverNumber(void)
{
    bool retVal=false;

    retVal = mtc->DriverNumberNotComplete(DEFAULT_DRIVER_NUMBER);

    CPPUNIT_ASSERT_EQUAL(true,retVal);
}

//=============================================================================
void CommonMachineTest::DriverNumberNotCompleteTest_EmptyDriverNumber(void)
{
    bool retVal=false;

    retVal = mtc->DriverNumberNotComplete("");

    CPPUNIT_ASSERT_EQUAL(true,retVal);
}

//=============================================================================
void CommonMachineTest::DriverNumberNotCompleteTest_False(void)
{
    bool retVal=true;

    retVal = mtc->DriverNumberNotComplete("T1234567");

    CPPUNIT_ASSERT_EQUAL(false,retVal);
}
