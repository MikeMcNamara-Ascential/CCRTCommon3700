//******************************************************************************
// Description:
//   Test class for testing the GenericABSModuleTemplate class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Brake/GenericABSModuleTemplateTest.cpp $
// 
// 2     1/18/07 3:22p Rwiersem
// Changes for the 07011901 core release:
//                                                            
// - Added OpenComTest() and CloseComTest().
// 
// 1     10/26/06 1:35p Rwiersem
// Initial version.
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
#include "GenericABSModuleTemplate.cpp"
#include "GenericABSModuleTemplateTest.h"
#include "KeywordProtocolFilter.h"

//=============================================================================
GenericABSModuleTemplateTest::GenericABSModuleTemplateTest(void)
{
}

//=============================================================================
GenericABSModuleTemplateTest::~GenericABSModuleTemplateTest(void)
{
}

//=============================================================================
void GenericABSModuleTemplateTest::setUp()
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
        "/Core/Unit/ApplicationLayer/ModuleInterfaces/Brake/Configuration/";
}

//=============================================================================
void GenericABSModuleTemplateTest::tearDown()
{
}

//=============================================================================
void GenericABSModuleTemplateTest::ConstructorTest(void)
{
    GenericABSModuleTemplate<KeywordProtocolFilter> gamt;

    CPPUNIT_ASSERT_EQUAL(string("ReadBrakeSwitch"),gamt.m_readBrakeSwitch);
    CPPUNIT_ASSERT_EQUAL(string("ReadSensorSpeeds"),gamt.m_readSensorSpeeds);
    CPPUNIT_ASSERT_EQUAL(string("ActuateReduction"),gamt.m_actuateReduction);
    CPPUNIT_ASSERT_EQUAL(string("ActuateRecovery"),gamt.m_actuateRecovery);
    CPPUNIT_ASSERT_EQUAL(string("CurrentBrakeSwitchState"),
                         gamt.m_currentBrakeSwitchStateMethod);
    CPPUNIT_ASSERT_EQUAL(string("SawBrakeSwitchOff"),
                         gamt.m_sawBrakeSwitchOffMethod);
    CPPUNIT_ASSERT_EQUAL(string("SawBrakeSwitchOn"),
                         gamt.m_sawBrakeSwitchOnMethod);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_currentBrakeSwitchState);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOn);
}

//=============================================================================
void GenericABSModuleTemplateTest::ParseSensorSpeedsTest(void)
{
    BEP_STATUS_TYPE retval = BEP_STATUS_FAILURE;
    GenericABSModuleTemplate<KeywordProtocolFilter> gamt;
    WheelSpeeds_t resultSpeeds;

    // set up the response string
    uint8_t responseString[] = {0x01,0x02,0x03,0x04};
    SerialString_t response = 
        SerialString_t(responseString,sizeof(responseString));

    // parse the configuration file
    XmlParser parser;
    string configFile = m_configPath + "GenericABSModuleTemplateConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gamt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gamt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // call the method under test
    retval = gamt.ParseSensorSpeeds(response,resultSpeeds);

    // make sure the result is success
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SUCCESS,retval);

    // make sure the wheel speeds are correct
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1,resultSpeeds[LFWHEEL],0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(2.2,resultSpeeds[RFWHEEL],0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.3,resultSpeeds[LRWHEEL],0.1);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(4.4,resultSpeeds[RRWHEEL],0.1);
}

//=============================================================================
void GenericABSModuleTemplateTest::ParseSensorSpeedsTest_ExceptionThrown(void)
{
    BEP_STATUS_TYPE retval = BEP_STATUS_FAILURE;
    GenericABSModuleTemplate<KeywordProtocolFilter> gamt;
    WheelSpeeds_t resultSpeeds;

    // set up the response string
    uint8_t responseString[] = {0x01,0x02,0x03,0x04};
    SerialString_t response = 
        SerialString_t(responseString,sizeof(responseString));

    // parse the configuration file
    XmlParser parser;
    string configFile = m_configPath + "GenericABSModuleTemplateConfigNotFloat.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // setup the serial message and reply interpretations
    gamt.m_vehicleMessagesNode = 
        document->getChild("SerialMessages/VehicleMessages")->Copy();
    gamt.m_replyInterpretationNode = 
        document->getChild("SerialMessages/ReplyInterpretations")->Copy();

    // this should fail because the reply interpretation in not a float value
    retval = gamt.ParseSensorSpeeds(response,resultSpeeds);

    // make sure the correct result is returned
    CPPUNIT_ASSERT_EQUAL(BEP_STATUS_SOFTWARE,retval);
}

//=============================================================================
void GenericABSModuleTemplateTest::OpenComTest(void)
{
    bool status;
    GenericABSModuleTemplate<KeywordProtocolFilter> gamt;

    status = gamt.OpenCom();

    CPPUNIT_ASSERT_EQUAL(false,status);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_currentBrakeSwitchState);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOn);
}

//=============================================================================
void GenericABSModuleTemplateTest::CloseComTest(void)
{
    GenericABSModuleTemplate<KeywordProtocolFilter> gamt;

    gamt.CloseCom();

    CPPUNIT_ASSERT_EQUAL(false,gamt.m_currentBrakeSwitchState);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOff);
    CPPUNIT_ASSERT_EQUAL(false,gamt.m_sawBrakeSwitchOn);
}
