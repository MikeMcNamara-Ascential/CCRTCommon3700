//******************************************************************************
// Description:
//   GenericTCTemplate test class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Common/GenericTcTemplateTest.cpp $
// 
// 2     2/22/07 11:29a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added reading in a configuration file.
// - Added an assertion for m_staticTesterServerComm == NULL in
// ConstructorTest().
// 
// - Added InitializeHook_DoNotLoadStaticTestResults().
// 
// 1     1/18/07 4:08p Rwiersem
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
#include "GenericTcTemplateTest.h"
#include "GenericTCTemplate.cpp"
#include "GenericModuleTemplate.cpp"
#include "MockProtocolFilter.h"

//=============================================================================
GenericTcTemplateTest::GenericTcTemplateTest(void)
{
}

//=============================================================================
GenericTcTemplateTest::~GenericTcTemplateTest(void)
{
}

//=============================================================================
void GenericTcTemplateTest::setUp()
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
        "/Core/Unit/ApplicationLayer/TestComponents/Common/Configuration/";

    // register the name "core/MotorController" so GenericTC can
    // create a MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");
}

//=============================================================================
void GenericTcTemplateTest::tearDown()
{
    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);
}

//=============================================================================
void GenericTcTemplateTest::ConstructorTest(void)
{
    GenericTCTemplate<GenericModuleTemplate<MockProtocolFilter> > gtct;

    CPPUNIT_ASSERT_EQUAL(false,gtct.m_communicationFailure);
    CPPUNIT_ASSERT_EQUAL(false,gtct.m_initializeHookCalled);
    CPPUNIT_ASSERT_EQUAL(false,gtct.m_initializeCalled);
    CPPUNIT_ASSERT_EQUAL(string(""),gtct.m_protocolFile);
    CPPUNIT_ASSERT_EQUAL(false,gtct.m_abortCalled);
    CPPUNIT_ASSERT_EQUAL(false,gtct.m_faultsInModule);
    CPPUNIT_ASSERT(gtct.m_staticTesterServerComm == NULL);
}

//=============================================================================
void GenericTcTemplateTest::DeactivateTest(void)
{
    GenericTCTemplate<GenericModuleTemplate<MockProtocolFilter> > gtct;

    gtct.Deactivate();

    CPPUNIT_ASSERT_EQUAL(false,gtct.m_communicationFailure);
    CPPUNIT_ASSERT_EQUAL(false,gtct.m_faultsInModule);
}

//=============================================================================
void GenericTcTemplateTest::InitializeHook_DoNotLoadStaticTestResults(void)
{
    GenericTCTemplate<GenericModuleTemplate<MockProtocolFilter> > gtct;
    XmlParser parser;
    string configFile = m_configPath + "GenericTcConfig.xml";
    const XmlNode *document = parser.ReturnXMLDocument(configFile);

    gtct.m_parameters = 
        (XmlNodeMap)(document->getChild("Setup/Parameters")->getChildren());

    gtct.InitializeHook(document);

    // make sure a static test comm object was not created
    CPPUNIT_ASSERT(gtct.m_staticTesterServerComm == NULL);
}
