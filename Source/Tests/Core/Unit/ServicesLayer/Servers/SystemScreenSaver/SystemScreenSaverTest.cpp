//******************************************************************************
// Description:
//    System screen saver test class.
//
//==============================================================================
//
//     Copyright (c) 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/SystemScreenSaver/SystemScreenSaverTest.cpp $
// 
// 1     5/01/07 1:48p Rwiersem
// Changes for the 07050201 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include <cppunit/extensions/HelperMacros.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "SystemScreenSaver.h"
#include "SystemScreenSaverTest.h"

using namespace std;

//=============================================================================
SystemScreenSaverTest::SystemScreenSaverTest()
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
        "/Core/Unit/ServicesLayer/Servers/SystemScreenSaver/Configuration/";
}

//=============================================================================
SystemScreenSaverTest::~SystemScreenSaverTest()
{
}

//=============================================================================
void SystemScreenSaverTest::setUp()
{
    sss = new SystemScreenSaver();
}

//=============================================================================
void SystemScreenSaverTest::tearDown()
{
    delete sss;
}

//=============================================================================
void SystemScreenSaverTest::TestConstructor(void)
{
    // Make sure the XmlNodeMaps are empty.
    // If the maps are cleared, getting the node at index 0 should throw an
    // XmlException.
    CPPUNIT_ASSERT_THROW(sss->m_triggers.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(sss->m_actions.getNode(0),XmlException);
    CPPUNIT_ASSERT_THROW(sss->m_videoControl.getNode(0),XmlException);
    CPPUNIT_ASSERT_EQUAL((UINT64)1000,sss->m_videoTimerTime);
    CPPUNIT_ASSERT_EQUAL(string("Normal"),sss->m_currentMode);
    CPPUNIT_ASSERT_EQUAL((INT64)0,sss->m_modeAccumulator);
    CPPUNIT_ASSERT_EQUAL((INT64)0,sss->m_waitCount);
    CPPUNIT_ASSERT_EQUAL((INT64)300,sss->m_switchDelay);
    CPPUNIT_ASSERT_EQUAL((INT64)3600,sss->m_okDelta);
}

//=============================================================================
void SystemScreenSaverTest::ReadParametersTest(void)
{
    XmlParser parser;
    string configFile = m_configPath + "SystemScreenSaverConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // call the method under test
    sss->ReadParameters(document);

    // make sure the triger values were read in correctly
    CPPUNIT_ASSERT_EQUAL(
        string("NormalScreen"),
        sss->m_triggers.getNode(0)->getChild("Value0")->getValue());

    CPPUNIT_ASSERT_EQUAL(
        string("NoAction"),
        sss->m_triggers.getNode(0)->getChild("Default")->getValue());

    // make sure the action translations were read in correclty
    CPPUNIT_ASSERT_EQUAL(
        string("NormalVideo"),
        sss->m_actions.getNode(0)->getChild("Message")->getValue());

    CPPUNIT_ASSERT_EQUAL(
        string("Normal"),
        sss->m_actions.getNode(0)->getChild("ScreenStatus")->getValue());

    // make sure the video controls were read in correctly
    // node 1 = "PanasonicSerialControl"
    CPPUNIT_ASSERT_EQUAL(
        string("True"),
        sss->m_videoControl.getNode(1)->getChild("Active")->getValue());

    // node 0 = "IpcControl"
    CPPUNIT_ASSERT_EQUAL(
        string("False"),
        sss->m_videoControl.getNode(0)->getChild("Active")->getValue());

    // make sure the mode switch time was read in correctly
    CPPUNIT_ASSERT_EQUAL((INT64)60,sss->m_switchDelay);

    // make sure the acceptable mode switching delta time was read in correctly
    CPPUNIT_ASSERT_EQUAL((INT64)120,sss->m_okDelta);
}

//=============================================================================
void SystemScreenSaverTest::DoActionTest_ActionFound(void)
{
    string retVal;
    XmlParser parser;
    string configFile = m_configPath + "SystemScreenSaverConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // read in the configuration items
    sss->ReadParameters(document);

    // call the method under test
    retVal = sss->DoAction("NormalScreen");

    // make sure the correct value was returned
    CPPUNIT_ASSERT_EQUAL(string("NormalVideo"),retVal);

    // make sure the current mode is correct
    CPPUNIT_ASSERT_EQUAL(string("Normal"),sss->m_currentMode);

    // make sure the wait count was reset
    CPPUNIT_ASSERT_EQUAL((INT64)0,sss->m_waitCount);
}

//=============================================================================
void SystemScreenSaverTest::DoActionTest_ActionNotFound(void)
{
    string retVal;
    XmlParser parser;
    string configFile = m_configPath + "SystemScreenSaverConfig.xml";

    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    // read in the configuration items
    sss->ReadParameters(document);

    // call the method under test
    retVal = sss->DoAction("InvertedScreen");

    // make sure the correct value was returned
    CPPUNIT_ASSERT_EQUAL(string(""),retVal);
}

