//******************************************************************************
// Description:
//   Test class for testing the Engine Controller Module Interface class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/EngineController/EngineControllerTest.cpp $
// 
// 4     2/22/07 11:22a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added assertions for setting m_inFieldModeRegister and
// m_inFieldModeBit.
// 
// - Added ReadSwitches_SkipParkNeutralSwitch().
// - Added ActivateHydraulicCoolingFan().
// - Added IsInFieldMode_True(), IsInFieldMode_False(), and
// IsInFieldMode_ReadError().
// 
// - Added GetInFieldModeRegister(), GetInFieldModeBit(),
// SetInFieldModeRegister(), and SetInFieldModeBit().
// 
// 3     1/18/07 3:34p Rwiersem
// Changes for the 07011901 core release:
//                                                                
// - Updated to use the common MockProtcolFilter class.
// - Added CloseComTest().
// 
// 2     10/26/06 1:41p Rwiersem
// Changes for the 06102701 core release:
// 
// Added a keep alive timer parameter to the protocol filters.
// 
// 1     9/06/06 10:11a Rwiersem
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
#include "EngineControllerTest.h"
#include "GenericEmissionsModuleTemplate.cpp"
#include "MockProtocolFilter.h"

//=============================================================================
EngineControllerTest::EngineControllerTest(void)
{
}

//=============================================================================
EngineControllerTest::~EngineControllerTest(void)
{
}

//=============================================================================
void EngineControllerTest::setUp()
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
        "/Core/Unit/ApplicationLayer/ModuleInterfaces/EngineController/" + 
        "Configuration/";
}

//=============================================================================
void EngineControllerTest::tearDown()
{
}

//=============================================================================
void EngineControllerTest::ConstructorTest(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    CPPUNIT_ASSERT(gemt.m_tpsMonitor == NULL);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_boolMonitorRedundantBrakeSwitch);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_boolMonitorParkNeutralSwitch);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchOffUpstopOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_boolMonitorClutchUpstopSwitch);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_initialTPS,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_finalTPSReading,0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_observedDeltaTPS,0.1);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_skipParkNeutralSwitchCheck);
    CPPUNIT_ASSERT(gemt.m_tpsMonitorComm != NULL);
}

//=============================================================================
void EngineControllerTest::InitializeHook(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    gemt.m_configNode = document->Copy();

    try
    {
        gemt.InitializeHook(document);
    }
    catch(BepException& ex)
    {
        // Calling GenericEmissionsModuleTemplate::InitializeHook() will call
        // ProtocolFilter::Initialize().  Since the logical ports are not setup
        // it will throw an exception.  So, we will catch the exception here
        // and check our assertions below.
    }

    CPPUNIT_ASSERT_EQUAL(string("TestStopped"),gemt.m_authenticationStopped);
    CPPUNIT_ASSERT_EQUAL(string("TestRunning"),gemt.m_authenticationRunning);
    CPPUNIT_ASSERT_EQUAL(string("TestStopPending"),
                         gemt.m_authenticationStopPending);
    CPPUNIT_ASSERT_EQUAL(string("TestStartPending"),
                         gemt.m_authenticationStartPending);
    CPPUNIT_ASSERT_EQUAL((UINT16)1,gemt.m_inFieldModeRegister);
    CPPUNIT_ASSERT_EQUAL((INT32)1,gemt.m_inFieldModeBit);
}

//=============================================================================
void EngineControllerTest::InitializeHook_MissingTags(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);
    gemt.m_configNode = document->Copy();

    try
    {
        gemt.InitializeHook(document);
    }
    catch(BepException& ex)
    {
        // Calling GenericEmissionsModuleTemplate::InitializeHook() will call
        // ProtocolFilter::Initialize().  Since the logical ports are not setup
        // it will throw an exception.  So, we will catch the exception here
        // and check our assertions below.
    }

    CPPUNIT_ASSERT_EQUAL(string("Routine Not Running"),
                         gemt.m_authenticationStopped);
    CPPUNIT_ASSERT_EQUAL(string("Routine Running"),
                         gemt.m_authenticationRunning);
    CPPUNIT_ASSERT_EQUAL(string("Stop Request Pending"),
                         gemt.m_authenticationStopPending);
    CPPUNIT_ASSERT_EQUAL(string("Start Request Pending"),
                         gemt.m_authenticationStartPending);
    CPPUNIT_ASSERT_EQUAL((UINT16)0,gemt.m_inFieldModeRegister);
    CPPUNIT_ASSERT_EQUAL((INT32)0,gemt.m_inFieldModeBit);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_SwitchesOnAutomatic(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // use the redundant brake switch check
    gemt.m_boolMonitorRedundantBrakeSwitch = true;
    // set the automatic transmission flag
    gemt.m_automaticTransmission = true;
    // tell the mock protocol filter to turn the switches on
    uint8_t temp[] = {0x30,0x06,0x0C};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the switch states are correct
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_parkNeutralSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOff);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_SwitchesOffAutomatic(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // use the redundant brake switch check
    gemt.m_boolMonitorRedundantBrakeSwitch = true;
    // set the automatic transmission flag
    gemt.m_automaticTransmission = true;
    // set the data need by the MockProtocolFilter
    uint8_t temp[] = {0x30,0x00,0x00};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the switch states are correct
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_parkNeutralSwitchOff);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_SwitchesOnManual(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // use the redundant brake switch check
    gemt.m_boolMonitorRedundantBrakeSwitch = true;
    // set the automatic transmission flag to false
    gemt.m_automaticTransmission = false;
    // use the clutch upstop switch check
    gemt.m_boolMonitorClutchUpstopSwitch = true;
    // tell the mock protocol filter to turn the switches on
    uint8_t temp[] = {0x30,0x06,0x0C};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the switch states are correct
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_clutchSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_clutchUpstopSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchOffUpstopOn);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_SwitchesOffManual(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // use the redundant brake switch check
    gemt.m_boolMonitorRedundantBrakeSwitch = true;
    // set the automatic transmission flag to false
    gemt.m_automaticTransmission = false;
    // use the clutch upstop switch check
    gemt.m_boolMonitorClutchUpstopSwitch = true;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x30,0x00,0x00};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the switch states are correct
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_clutchSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOn);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_clutchUpstopSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchOffUpstopOn);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_ClutchOffUpstopOnDetected(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the automatic transmission flag to false
    gemt.m_automaticTransmission = false;
    // use the clutch upstop switch check
    gemt.m_boolMonitorClutchUpstopSwitch = true;
    // set up the correct switch values
    uint8_t temp[] = {0x30,0x04,0x00};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the switch states are correct
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_clutchOffUpstopOn);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_GetModuleDataError(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x30};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the GetModuleData() return value
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_CaughtException(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x30};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SOFTWARE,status);
}

//=============================================================================
void EngineControllerTest::ReadSwitches_SkipParkNeutralSwitch(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;

    // set the correct switch state values
    gemt.m_brakeSwitchOn = true;
    gemt.m_brakeSwitchOff = true;
    gemt.m_boolMonitorRedundantBrakeSwitch = false;
    gemt.m_automaticTransmission = true;
    gemt.m_parkNeutralSwitchOn = false;
    gemt.m_skipParkNeutralSwitchCheck = true;

    // call the method under test
    status = gemt.ReadSwitches(&mpf);

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure that GetModuleData() was not called
    CPPUNIT_ASSERT_EQUAL(0,mpf.m_getModuleDataCalled);
}

//=============================================================================
void EngineControllerTest::StartQuickLearnEngineAuthentication(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the running string
    gemt.SetEngineAuthenticationRunningString("R");

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);
}

//=============================================================================
void 
EngineControllerTest::StartQuickLearnEngineAuthentication_StatusMismatch(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the running string
    gemt.SetEngineAuthenticationRunningString("S");

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void 
EngineControllerTest::StartQuickLearnEngineAuthentication_CaughtException(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::
StartQuickLearnEngineAuthentication_GetModuleDataError(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the negative response value
    mpf->SetNegativeResponseValue(0x00);
    mpf->SetSentMessageIdentifierIndex(1);

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_ERROR,status);
}

//=============================================================================
void EngineControllerTest::
StartQuickLearnEngineAuthentication_GetModuleDataErrorNegResp(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x4E);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::
StartQuickLearnEngineAuthentication_GetModuleData2Error(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x00);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_ERROR,status);
}

//=============================================================================
void EngineControllerTest::
StartQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x4E);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StartQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::StopQuickLearnEngineAuthentication(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the running string
    gemt.SetEngineAuthenticationStoppedString("R");

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);
}

//=============================================================================
void 
EngineControllerTest::StopQuickLearnEngineAuthentication_StatusMismatch(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = m_configPath + "EngineControllerModuleConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the running string
    gemt.SetEngineAuthenticationStoppedString("S");

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void 
EngineControllerTest::StopQuickLearnEngineAuthentication_CaughtException(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfigMissingTags.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::
StopQuickLearnEngineAuthentication_GetModuleDataError(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the negative response value
    mpf->SetNegativeResponseValue(0x00);
    mpf->SetSentMessageIdentifierIndex(1);

    gemt.m_protocolFilter = mpf;
    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_ERROR,status);
}

//=============================================================================
void EngineControllerTest::
StopQuickLearnEngineAuthentication_GetModuleDataErrorNegResp(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x4E);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::
StopQuickLearnEngineAuthentication_GetModuleData2Error(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x00);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_ERROR,status);
}

//=============================================================================
void EngineControllerTest::
StopQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x52,0x4E};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the GetModuleData() return value
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // set the negative response value
    mpf->SetNegativeResponseValue(0x4E);
    mpf->SetSentMessageIdentifierIndex(1);

    // call the method under test
    status = gemt.StopQuickLearnEngineAuthentication();

    // make sure the status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::GetEngineAuthenticationRunningString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_authenticationRunning = "TestRunning";

    CPPUNIT_ASSERT_EQUAL(string("TestRunning"),
                         gemt.GetEngineAuthenticationRunningString());
}

//=============================================================================
void EngineControllerTest::GetEngineAuthenticationStartPendingString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_authenticationStartPending = "TestStartPending";

    CPPUNIT_ASSERT_EQUAL(string("TestStartPending"),
                         gemt.GetEngineAuthenticationStartPendingString());
}

//=============================================================================
void EngineControllerTest::GetEngineAuthenticationStoppedString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_authenticationStopped = "TestStopped";

    CPPUNIT_ASSERT_EQUAL(string("TestStopped"),
                         gemt.GetEngineAuthenticationStoppedString());
}

//=============================================================================
void EngineControllerTest::GetEngineAuthenticationStopPendingString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_authenticationStopPending = "TestStopPending";

    CPPUNIT_ASSERT_EQUAL(string("TestStopPending"),
                         gemt.GetEngineAuthenticationStopPendingString());
}

//=============================================================================
void EngineControllerTest::SetEngineAuthenticationRunningString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetEngineAuthenticationRunningString("TestRunning");

    CPPUNIT_ASSERT_EQUAL(string("TestRunning"),gemt.m_authenticationRunning);
}

//=============================================================================
void EngineControllerTest::SetEngineAuthenticationStartPendingString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetEngineAuthenticationStartPendingString("TestStartPending");

    CPPUNIT_ASSERT_EQUAL(string("TestStartPending"),
                         gemt.m_authenticationStartPending);
}

//=============================================================================
void EngineControllerTest::SetEngineAuthenticationStoppedString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetEngineAuthenticationStoppedString("TestStopped");

    CPPUNIT_ASSERT_EQUAL(string("TestStopped"),
                         gemt.m_authenticationStopped);
}

//=============================================================================
void EngineControllerTest::SetEngineAuthenticationStopPendingString(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetEngineAuthenticationStopPendingString("TestStopPending");

    CPPUNIT_ASSERT_EQUAL(string("TestStopPending"),
                         gemt.m_authenticationStopPending);
}

//=============================================================================
void EngineControllerTest::CloseComTest(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.CloseCom();

    CPPUNIT_ASSERT(gemt.m_tpsMonitor == NULL);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_brakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_redundantBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_boolMonitorRedundantBrakeSwitch);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_parkNeutralSwitchOff);
    CPPUNIT_ASSERT_EQUAL(true,gemt.m_boolMonitorParkNeutralSwitch);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchUpstopSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_clutchOffUpstopOn);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_boolMonitorClutchUpstopSwitch);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_initialTPS,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_finalTPSReading,0.01);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gemt.m_observedDeltaTPS,0.01);
    CPPUNIT_ASSERT_EQUAL(false,gemt.m_skipParkNeutralSwitchCheck);

    // Make sure the XmlNodeMaps are empty.
    // If the maps are cleared, getting the node at index 0 should throw an
    // XmlException.
    CPPUNIT_ASSERT_THROW(gemt.m_engineTypes.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(gemt.m_vehicleTypes.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(gemt.m_emissionsTypes.getNode(0),XmlException);
}

//=============================================================================
void EngineControllerTest::ActivateHydraulicCoolingFan(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter mpf(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    
    // set the mock protocol filter response
    uint8_t temp[] = {0x01,0x02,0x03};
    mpf.m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the return value from GetModuleData()
    mpf.m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // call the method under test
    status = gemt.ActivateHydraulicCoolingFan(&mpf);

    // make sure the response is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the correct data was passed to the mock protocol filter
    CPPUNIT_ASSERT_EQUAL(1,mpf.m_getModuleDataCalled);
    CPPUNIT_ASSERT_EQUAL(string("StartHydraulicCoolingFan"),
                         mpf.m_getModuleDataParamMessageTag[0]);
}

//=============================================================================
void EngineControllerTest::IsInFieldMode_True(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfig.xml";
    bool inFieldMode=false;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x01,0x02,0x00,0x02};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the in field mode bit
    gemt.m_inFieldModeBit = 1;

    // call the method under test
    status = gemt.IsInFieldMode(inFieldMode);

    // make sure the return status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the in field mode variable is set correctly
    CPPUNIT_ASSERT_EQUAL(true,inFieldMode);
}

//=============================================================================
void EngineControllerTest::IsInFieldMode_False(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    XmlParser parser;
    string configFile = 
        m_configPath + "EngineControllerModuleConfig.xml";
    bool inFieldMode=true;

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gemt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gemt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x01,0x02,0x00,0x00};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_SUCCESS);

    // set the in field mode bit
    gemt.m_inFieldModeBit = 1;

    // call the method under test
    status = gemt.IsInFieldMode(inFieldMode);

    // make sure the return status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,status);

    // make sure the in field mode variable is set correctly
    CPPUNIT_ASSERT_EQUAL(false,inFieldMode);
}

//=============================================================================
void EngineControllerTest::IsInFieldMode_ReadError(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;
    // The object below will be deleted by the destructor
    KeepAliveTimer_t    messageTimer;
    MockProtocolFilter* mpf = new MockProtocolFilter(messageTimer);
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    bool inFieldMode=true;

    // set the protocol filter
    gemt.m_protocolFilter = mpf;

    // set the data needed by the MockProtocolFilter
    uint8_t temp[] = {0x01,0x02,0x00,0x00};
    mpf->m_getModuleDataReply.push_back(SerialString_t(temp,sizeof(temp)));

    // set the return value from GetModuleData()
    mpf->m_getModuleDataReturnValue.push_back(BEP_STATUS_FAILURE);

    // call the method under test
    status = gemt.IsInFieldMode(inFieldMode);

    // make sure the return status is correct
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_FAILURE,status);
}

//=============================================================================
void EngineControllerTest::GetInFieldModeRegister(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_inFieldModeRegister = 1;

    CPPUNIT_ASSERT_EQUAL((UINT16)1,gemt.GetInFieldModeRegister());
}

//=============================================================================
void EngineControllerTest::GetInFieldModeBit(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.m_inFieldModeBit = 1;

    CPPUNIT_ASSERT_EQUAL((INT32)1,gemt.GetInFieldModeBit());
}

//=============================================================================
void EngineControllerTest::SetInFieldModeRegister(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetInFieldModeRegister(1);

    CPPUNIT_ASSERT_EQUAL((UINT16)1,gemt.m_inFieldModeRegister);
}

//=============================================================================
void EngineControllerTest::SetInFieldModeBit(void)
{
    GenericEmissionsModuleTemplate<MockProtocolFilter> gemt;

    gemt.SetInFieldModeBit(1);

    CPPUNIT_ASSERT_EQUAL((INT32)1,gemt.m_inFieldModeBit);
}

