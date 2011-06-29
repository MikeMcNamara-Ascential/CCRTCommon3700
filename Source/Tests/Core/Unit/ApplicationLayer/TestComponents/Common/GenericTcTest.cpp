//******************************************************************************
// Description:
//   GenericTC test class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Common/GenericTcTest.cpp $
// 
// 3     5/01/07 1:17p Rwiersem
// Changes for the 07050201 core release:
// 
// Added GetNamedDataBrokerTest().
// 
// 2     1/18/07 4:08p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_configCopy equals NULL assertion in ConstructorTest().
// - Added assertions to make sure the configuration node was copied 
// successfully in TestInitialize_ConfigFile().
// - Updated InitializeObject() to set the configuration file and
// modification
// time.
// 
// 1     10/26/06 1:48p Rwiersem
// Changes for the 06102701 core release:
// 
// Intial version.
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
#include "GenericTcTest.h"
#include "GenericTC.h"
#include "MockTestResultServer.h"
#include "MockNamedDataBroker.h"

//=============================================================================
GenericTcTest::GenericTcTest(void)
{
    // Make sure the testing path is set
    if(getenv("TEST_DIR_PATH") == NULL)
    {
        cerr << "ERROR: TEST_DIR_PATH must be set to top testing dir" << endl;
        exit(1);
    }

    // create the path to the configuration files
    string topTestDir = getenv("TEST_DIR_PATH");
    m_configDir = topTestDir + "/Core/Unit/ApplicationLayer/" + 
        "TestComponents/Common/Configuration/";

    // set the configuration file name
    m_configFile = m_configDir + "GenericTcConfig.xml";

    // register the IPC name
    m_regName = CreateUniqueIpcName( std::string( __progname), m_configFile);

    const XmlNode *doc = m_xmlParser.ReturnXMLDocument(m_configFile);

    // get the IPC registration alias
    m_regAlias = doc->getChild( "Setup/Communication/Name")->getValue();
}

//=============================================================================
GenericTcTest::~GenericTcTest(void)
{
}

//=============================================================================
void GenericTcTest::setUp()
{
    // register the name "core/MotorController" so GenericTC can
    // create a MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");

    // register the name "core/NamedDataBroker" so GenericTC can
    // create an INamedDataBroker interface
    m_ndbId = BposRegisterTaskName("core/NamedDataBroker");

    // register the name "core/PromptServer" so GenericTC can
    // create a PromptServer interface
    m_promptServerId = BposRegisterTaskName("core/PromptServer");

    // register the name "core/FaultServer" so GenericTC can
    // create a FaultServer interface
    m_faultServerId = BposRegisterTaskName("core/FaultServer");

    // register the name "core/TestResultServer" so GenericTC can
    // create a TestResultServer interface
    m_testResultServerId = BposRegisterTaskName("core/TestResultServer");

    // register the name "mgr/ICM" so GenericTC can create an IcmMgr interface
    m_icmId = BposRegisterTaskName("mgr/ICM");

    // register the name "component/GenericTc" so an IPC channel can be created
    m_componentId = BposRegisterTaskName("component/GenericTc");

    // create the test object
    gtc = new GenericTC();
}

//=============================================================================
void GenericTcTest::tearDown()
{
    // unregister "component/GenericTc"
    BposUnRegisterTaskName(m_componentId);

    // unregister "mgr/ICM"
    BposUnRegisterTaskName(m_icmId);

    // unregister "core/TestResultServer"
    BposUnRegisterTaskName(m_testResultServerId);

    // unregister "core/FaultServer"
    BposUnRegisterTaskName(m_faultServerId);

    // unregister "core/PromptServer"
    BposUnRegisterTaskName(m_promptServerId);

    // unregister "core/NamedDataBroker"
    BposUnRegisterTaskName(m_ndbId);

    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);

    delete gtc;
}

//=============================================================================
void GenericTcTest::ConstructorTest(void)
{
    CPPUNIT_ASSERT_EQUAL(true,gtc->m_foregroundTest);

    // the critical section enter should fail because the count is 0
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         gtc->m_semInitialized.CriticalSectionEnter(1));

    // the critical section enter should fail because the count is 0
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         gtc->m_semCommand.CriticalSectionEnter(1));

    // The count on the semaphore should be set to 1.  So, the first 
    // CriticalSectionEnter() should be successful and the second one should
    // fail
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,
                         gtc->m_semReady.CriticalSectionEnter(1));
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         gtc->m_semReady.CriticalSectionEnter(1));

    // the critical section enter should fail because the count is 0
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,
                         gtc->m_semResponse.CriticalSectionEnter(1));

    CPPUNIT_ASSERT_EQUAL(false,gtc->m_fatalFailure);
    CPPUNIT_ASSERT_EQUAL(-1,gtc->m_speedFile);
    CPPUNIT_ASSERT_EQUAL(false,gtc->m_speedFileOpen);
    CPPUNIT_ASSERT_EQUAL(-1,gtc->m_wheelDistanceFile);
    CPPUNIT_ASSERT_EQUAL(false,gtc->m_wheelDistanceFileOpen);
    CPPUNIT_ASSERT_EQUAL(string(""),gtc->m_faultFile);
    CPPUNIT_ASSERT_EQUAL((time_t)0,gtc->m_faultFileTime);
    CPPUNIT_ASSERT_EQUAL(string(""),gtc->m_regName);
    CPPUNIT_ASSERT_EQUAL(string(""),gtc->m_regAlias);
    CPPUNIT_ASSERT(gtc->m_ndb == NULL);
    CPPUNIT_ASSERT(gtc->m_prompt == NULL);
    CPPUNIT_ASSERT(gtc->m_fault == NULL);
    CPPUNIT_ASSERT(gtc->m_testResult == NULL);
    CPPUNIT_ASSERT_EQUAL(false,gtc->m_retest);
    CPPUNIT_ASSERT_EQUAL(string(""),gtc->m_command);
    CPPUNIT_ASSERT_EQUAL(string(""),gtc->m_value);
    CPPUNIT_ASSERT_EQUAL(string(BEP_TESTING_RESPONSE),gtc->m_overallResult);
    CPPUNIT_ASSERT_EQUAL(string(BEP_UNTESTED_RESPONSE),gtc->m_previousResult);
    CPPUNIT_ASSERT_EQUAL(string(BEP_INITIALIZED_STATUS),gtc->GetStatus());
    CPPUNIT_ASSERT(gtc->m_configCopy == NULL);
}

//=============================================================================
void GenericTcTest::SendSubtestResultWithDetailTest(void)
{
    INT32 retval=BEP_STATUS_ERROR;

    // create a mock test result server
    gtc->m_testResult = new MockTestResultServer();
    MockTestResultServer *mtrs = (MockTestResultServer*)gtc->m_testResult;

    // store the data needed by the mock test result server
    mtrs->m_writeTestResultReturnMessage.push_back("Pass");
    mtrs->m_writeTestResultReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    gtc->SetComponentName("GenericTcTest");

    // call the function under test
    retval = gtc->SendSubtestResultWithDetail("TestName","Pass",
                                              "TestDescription","0001",
                                              "detail1","detail1Val",
                                              "detail1Unit","detail2",
                                              "detail2Val","detail2Unit",
                                              "detail3","detail3Val",
                                              "detail3Unit","detail4",
                                              "detail4Val","detail4Unit",
                                              "detail5","detail5Val",
                                              "detail5Unit");

    // make sure it passed
    CPPUNIT_ASSERT_EQUAL((INT32)BEP_STATUS_SUCCESS,retval);
    // make sure the method was only called once
    CPPUNIT_ASSERT_EQUAL(1,mtrs->m_writeTestResultCalled);
    // make sure the passed in parameters were stored correctly
    CPPUNIT_ASSERT_EQUAL(string("GenericTcTestTestName"),
                         mtrs->m_writeTestResultParamTest[0]);
    CPPUNIT_ASSERT_EQUAL(string("Pass"),mtrs->m_writeTestResultParamResult[0]);
    CPPUNIT_ASSERT_EQUAL(string("GenericTcTest"),
                         mtrs->m_writeTestResultParamProcess[0]);
    CPPUNIT_ASSERT_EQUAL(string("TestDescription"),
                         mtrs->m_writeTestResultParamDescription[0]);

    TestResultDetail_t detail;
    TestResultDetailsCItr_t detailItr;

    // get the first detail and check its values
    detailItr = mtrs->m_writeTestResultParamDetails[0].GetFirstDetail(detail);
    CPPUNIT_ASSERT_EQUAL(string("detail1"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("detail1Val"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string("detail1Unit"),detail.detailUnits);

    // get the second detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("detail2"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("detail2Val"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string("detail2Unit"),detail.detailUnits);

    // get the third detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("detail3"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("detail3Val"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string("detail3Unit"),detail.detailUnits);

    // get the fourth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("detail4"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("detail4Val"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string("detail4Unit"),detail.detailUnits);

    // get the fifth detail and check its values
    detailItr = 
        mtrs->m_writeTestResultParamDetails[0].GetNextDetail(detail,detailItr);
    CPPUNIT_ASSERT_EQUAL(string("detail5"),detail.detailTag);
    CPPUNIT_ASSERT_EQUAL(string("detail5Val"),detail.detailValue);
    CPPUNIT_ASSERT_EQUAL(string("detail5Unit"),detail.detailUnits);

    // check the isFinal flag and code parameter
    CPPUNIT_ASSERT_EQUAL(true,(bool)mtrs->m_writeTestResultParamIsFinal[0]);
    CPPUNIT_ASSERT_EQUAL(string("0001"),mtrs->m_writeTestResultParamCode[0]);

    // delete the mock object
    delete gtc->m_testResult;
    gtc->m_testResult = NULL;
}

//=============================================================================
void GenericTcTest::TestInitialize_ConfigFile(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    std::string failureString = std::string( "Config file name <") +  
        gtc->GetConfigFile() + "> is not <" + m_configFile + ">";

    CPPUNIT_ASSERT_MESSAGE(failureString.c_str(),
                           (gtc->GetConfigFile() == m_configFile));

    // make sure the configuration was copied correctly
    CPPUNIT_ASSERT_EQUAL(m_xmlParser.getDocElement()->getName(),
                         gtc->m_configCopy->getName());
    CPPUNIT_ASSERT_EQUAL(m_xmlParser.getDocElement()->getValue(),
                         gtc->m_configCopy->getValue());
}

//=============================================================================
void GenericTcTest::TestInitialize_RegisteredName(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    std::string failureString = std::string( "Registered name <") +  
        gtc->m_regName + "> is not <" + m_regName + ">";
    int nameId = BposGetTaskId( m_regName.c_str());

    CPPUNIT_ASSERT_MESSAGE(failureString.c_str(),(gtc->m_regName == m_regName));
    CPPUNIT_ASSERT_MESSAGE("IPC channel name is not registered",(-1 != nameId));
}

//=============================================================================
void GenericTcTest::TestInitialize_IPCNameAlias(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    std::string fullIpcAlias( ipcNamePrefix + "/" + m_regAlias);
    std::string failureString = std::string( "IPC alias <") +  gtc->m_regAlias + 
        "> is not <" + fullIpcAlias + ">";
    int aliasId = BposGetTaskId( m_regAlias.c_str());

    CPPUNIT_ASSERT_MESSAGE(failureString.c_str(), 
                           (gtc->m_regAlias == fullIpcAlias));
    CPPUNIT_ASSERT_MESSAGE("IPC channel alias is not created", 
                           (-1 != aliasId));
}

//=============================================================================
void GenericTcTest::TestInitialize_ObjectiveSize(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    const XmlNode *document = m_xmlParser.getDocElement();

    CPPUNIT_ASSERT_MESSAGE(
        "m_objectives wrong size",
        (gtc->m_objectives.size() == 
         document->getChild( "Objectives")->getChildren().size()));
}

//=============================================================================
void GenericTcTest::TestInitialize_TestStepsSize(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    const XmlNode *document = m_xmlParser.getDocElement();

    CPPUNIT_ASSERT_MESSAGE(
        "m_testSteps wrong size",
        (gtc->m_testSteps.size() == 
         document->getChild( "TestSteps")->getChildren().size()));
}

//=============================================================================
void GenericTcTest::TestInitialize_ComponentName(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    const XmlNode *document = m_xmlParser.getDocElement();
    std::string failureString = 
        std::string( "Component name <") +  gtc->m_componentName +
         "> is not <" + document->getChild( "Setup/Name")->getValue() + ">";

    CPPUNIT_ASSERT_MESSAGE(
        failureString.c_str(), 
        (gtc->m_componentName == 
         document->getChild( "Setup/Name")->getValue()));
}

//=============================================================================
void GenericTcTest::TestInitialize_ParameterSize(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    const XmlNode *document = m_xmlParser.getDocElement();
    const XmlNode *parameters = NULL;
    uint32_t    paramCount;

    try
    {
        parameters = document->getChild("Setup/Parameters");
        paramCount = parameters->getChildren().size();
    }
    catch( XmlException &err)
    {
        parameters = NULL;
        paramCount = 0;
    }
    CPPUNIT_ASSERT_MESSAGE("m_components wrong size", 
                           (gtc->m_parameters.size() == paramCount));
}

//=============================================================================
void GenericTcTest::TestInitialize_IPromptCreated(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    CPPUNIT_ASSERT_MESSAGE("m_prompt is NULL", (gtc->m_prompt != NULL));
}

//=============================================================================
void GenericTcTest::TestInitialize_IFaultCreated(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    CPPUNIT_ASSERT_MESSAGE("m_fault is NULL", (gtc->m_fault != NULL));
}

//=============================================================================
void GenericTcTest::TestInitialize_ITestResultCreated(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    CPPUNIT_ASSERT_MESSAGE("m_testResult is NULL",(gtc->m_testResult != NULL));
}

//=============================================================================
void GenericTcTest::TestInitialize_ParkBrakeDirections(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    vector<string>::iterator itr;
    bool forwardFound = false;
    bool reverseFound = false;
    bool bothFound = false;

    // search the vector for the valid names
    for(itr = gtc->m_validParkBrakeDirections.begin(); 
        itr != gtc->m_validParkBrakeDirections.end();
        itr++)
    {
        if(*itr == "Forward") forwardFound = true;
        if(*itr == "Reverse") reverseFound = true;
        if(*itr == "Both") bothFound = true;
    }

    CPPUNIT_ASSERT_EQUAL(true,forwardFound);
    CPPUNIT_ASSERT_EQUAL(true,reverseFound);
    CPPUNIT_ASSERT_EQUAL(true,bothFound);
}

//=============================================================================
void GenericTcTest::TestShouldReloadConfigFile_ReloadSameConfigFile(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    CPPUNIT_ASSERT_MESSAGE("Config file should not need to be reloaded", 
                           (false == gtc->ShouldReloadConfigFile()));
}

//=============================================================================
void GenericTcTest::TestShouldReloadConfigFile_ReloadNewConfigFile(void)
{
    // Initialize the GenericTC object
    InitializeObject();

    time_t oldTime = gtc->m_cfgModTime;
    gtc->m_cfgModTime = gtc->m_cfgModTime - 1;

    CPPUNIT_ASSERT_MESSAGE("Config file needs to be reloaded", 
                           (true == gtc->ShouldReloadConfigFile()));

    gtc->m_cfgModTime = oldTime;
}

//=============================================================================
void GenericTcTest::InitializeObject(void)
{
    // create a mock named data broker since the full system isn't running
    gtc->m_ndb = new MockNamedDataBroker();
    MockNamedDataBroker *mndb = (MockNamedDataBroker*)gtc->m_ndb;
    mndb->m_readReturnResponse.push_back("PASS");
    mndb->m_readReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);
    mndb->m_getByTagReturnDataValue.push_back("4");
    mndb->m_getByTagReturnMessage.push_back("PASS");
    mndb->m_getByTagReturnValue.push_back((INT32)BEP_STATUS_SUCCESS);

    // set the configuration file and modification time
    gtc->SetConfigFile(m_configFile);    // Set name of configuration file
    gtc->m_cfgModTime = GetFileModificationTime(m_configFile);

    // initialize the GenericTc
    gtc->Initialize(m_xmlParser.getDocElement());

    // signal that the initialization is complete
    gtc->InitializationComplete();
}

//=============================================================================
void GenericTcTest::GetNamedDataBrokerTest(void)
{
    gtc->m_ndb = new MockNamedDataBroker();

    CPPUNIT_ASSERT(gtc->GetNamedDataBroker() == gtc->m_ndb);
}
