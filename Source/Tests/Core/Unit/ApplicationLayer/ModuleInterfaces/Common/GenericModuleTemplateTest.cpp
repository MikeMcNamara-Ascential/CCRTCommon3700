//******************************************************************************
// Description:
//  The unit testing class for the GenericModuleTemplateTest Class.
//
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericModuleTemplateTest.cpp $
// 
// 1     1/18/07 3:31p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericModuleTemplate.cpp"
#include "GenericModuleTemplateTest.h"
#include "MockProtocolFilter.h"

//=============================================================================
GenericModuleTemplateTest::GenericModuleTemplateTest(void)
{
}

//=============================================================================
GenericModuleTemplateTest::~GenericModuleTemplateTest(void)
{
}

//=============================================================================
void GenericModuleTemplateTest::setUp()
{
}

//=============================================================================
void GenericModuleTemplateTest::tearDown()
{
}

//=============================================================================
void GenericModuleTemplateTest::ConstructorTest(void)
{
    GenericModuleTemplate<MockProtocolFilter> gmt;

    CPPUNIT_ASSERT_EQUAL(false,gmt.m_initializeHookCalled);
    CPPUNIT_ASSERT_EQUAL(false,gmt.m_initializeCalled);
    CPPUNIT_ASSERT(gmt.m_configNode == NULL);
    CPPUNIT_ASSERT(gmt.m_replyInterpretationNode == NULL);
    CPPUNIT_ASSERT(gmt.m_vehicleMessagesNode == NULL);
    CPPUNIT_ASSERT(gmt.m_moduleKeepAlive == NULL);
    CPPUNIT_ASSERT(gmt.m_switchMonitor == NULL);
    CPPUNIT_ASSERT(gmt.m_moduleInterpreter != NULL);
    CPPUNIT_ASSERT_EQUAL(string("ReadPartNumber"),gmt.m_readPartNumber);
    CPPUNIT_ASSERT_EQUAL(string("UnLockModule"),gmt.m_unLockModule);
    CPPUNIT_ASSERT_EQUAL(string("ProgramVIN"),gmt.m_programVIN);
    CPPUNIT_ASSERT_EQUAL(string("ReadVIN"),gmt.m_readVIN);
    CPPUNIT_ASSERT_EQUAL(string("ReadTraceData"),gmt.m_readTraceData);
    CPPUNIT_ASSERT_EQUAL(string("ReadFaults"),gmt.m_readFaults);
    CPPUNIT_ASSERT_EQUAL(string("StartSwitchMonitor"),gmt.m_startSwitchMonitor);
    CPPUNIT_ASSERT_EQUAL(string("StopSwitchMonitor"),gmt.m_stopSwitchMonitor);
    CPPUNIT_ASSERT_EQUAL(string("StartModuleKeepAlive"),
                         gmt.m_startModuleKeepAlive);
    CPPUNIT_ASSERT_EQUAL(string("StopModuleKeepAlive"),
                         gmt.m_stopModuleKeepAlive);
    CPPUNIT_ASSERT_EQUAL(string("LockModule"),gmt.m_lockModule);
    CPPUNIT_ASSERT_EQUAL(string("ClearFaults"),gmt.m_clearFaults);
    CPPUNIT_ASSERT_EQUAL(string("EnterDiagnosticMode"),
                         gmt.m_enterDiagnosticMode);
    CPPUNIT_ASSERT_EQUAL(string("ExitDiagnosticMode"),gmt.m_exitDiagnosticMode);
    CPPUNIT_ASSERT_EQUAL(0ULL,gmt.m_lastTxTime.GetValue());
    CPPUNIT_ASSERT(gmt.m_switchMonitorComm != NULL);
    CPPUNIT_ASSERT(gmt.m_keepAliveComm != NULL);
    CPPUNIT_ASSERT(gmt.m_protocolFilter != NULL);
}

//=============================================================================
void GenericModuleTemplateTest::CloseComTest(void)
{
    GenericModuleTemplate<MockProtocolFilter> gmt;

    gmt.CloseCom();

    // make sure the module keep alive thread was set to NULL
    CPPUNIT_ASSERT(gmt.m_moduleKeepAlive == NULL);

    // make sure the switch monitor thread was set to NULL
    CPPUNIT_ASSERT(gmt.m_switchMonitor == NULL);
}

//=============================================================================
void GenericModuleTemplateTest::IsInitializedTest(void)
{   
    GenericModuleTemplate<MockProtocolFilter> gmt;

    gmt.m_initializeHookCalled = false;

    CPPUNIT_ASSERT_EQUAL(false,gmt.IsInitialized());
}

//=============================================================================
void GenericModuleTemplateTest::UninitializeTest(void)
{
    GenericModuleTemplate<MockProtocolFilter> gmt;

    gmt.Uninitialize();

    CPPUNIT_ASSERT_EQUAL(false,gmt.m_initializeHookCalled);
}
