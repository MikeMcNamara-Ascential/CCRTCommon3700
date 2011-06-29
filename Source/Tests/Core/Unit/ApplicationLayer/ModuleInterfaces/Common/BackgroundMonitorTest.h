//******************************************************************************
// Description:
//  The test class for testing the BackgroundMonitor Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/BackgroundMonitorTest.h $
// 
// 1     10/26/06 1:40p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BackgroundMonitorTest_h
#define BackgroundMonitorTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class MockBackgroundMonitor;
class KeywordProtocolFilter;

class BackgroundMonitorTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BackgroundMonitorTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(SetInterMessageGapTest);
    CPPUNIT_TEST_SUITE_END();

public:
    BackgroundMonitorTest();
    virtual ~BackgroundMonitorTest();
    /**
     * Setup is run before each test, use it to set up the member ivars for use 
     * during the test
     */
    void setUp();
    /**
     * TearDown is called after each test, use it if you need to clean things up 
     * after your test
     */
    void tearDown();
    /**
     * Test that will verify that the object is correctly initialized on 
     * construction.
     */
    void ConstructorTest(void);
    /**
     * Test that willv verify SetInterMessageGap() properly sets the
     * inter-message gap.
     */ 
    void SetInterMessageGapTest(void);

private:

    /** MockBackgroundMonitor object under test */
    MockBackgroundMonitor *mbm;

    /** 
     * KeywordProtocolFilter object to use when creating the 
     * MockBackgroundMonitor.
     */
    KeywordProtocolFilter *pf;
};
#endif //BackgroundMonitorTest_h
