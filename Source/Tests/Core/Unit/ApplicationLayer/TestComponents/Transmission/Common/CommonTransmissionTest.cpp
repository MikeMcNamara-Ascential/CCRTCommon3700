//******************************************************************************
// Description:
//   Test class for testing the Transmission/Common class.
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
//     $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Transmission/Common/CommonTransmissionTest.cpp $
// 
// 1     9/06/06 10:22a Rwiersem
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
#include "CommonTransmissionTest.h"
#include "GenericTransmissionTCTemplate.cpp"
#include "GenericTransmissionModuleTemplate.cpp"
#include "KeywordProtocolFilter.h"

//=============================================================================
CommonTransmissionTest::CommonTransmissionTest(void)
{
}

//=============================================================================
CommonTransmissionTest::~CommonTransmissionTest(void)
{
}

//=============================================================================
void CommonTransmissionTest::setUp()
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
        "/Core/Unit/ApplicationLayer/TestComponents/Transmission/" +
        "Common/Configuration/";

    // register the name "core/MotorController" so machineTC can
    // create a mock MotorController interface
    m_motorControllerId = BposRegisterTaskName("core/MotorController");
}

//=============================================================================
void CommonTransmissionTest::tearDown()
{
    // unregister "core/MotorController"
    BposUnRegisterTaskName(m_motorControllerId);
}

//=============================================================================
void CommonTransmissionTest::ConstructorTest(void)
{
    GenericTransmissionTCTemplate
        <GenericTransmissionModuleTemplate<KeywordProtocolFilter> > gt;

    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_firstGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_secondGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_thirdGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_fourthGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_fifthGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_sixthGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_parkGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_reverseGearUpdated);
    CPPUNIT_ASSERT_EQUAL(testSkip,gt.m_neutralGearUpdated);
    CPPUNIT_ASSERT_EQUAL(false,gt.m_autostickEquipped);
    CPPUNIT_ASSERT_EQUAL(false,gt.m_quickLearnRunning);
}

