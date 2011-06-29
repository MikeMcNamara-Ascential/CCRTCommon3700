//******************************************************************************
// Description:
//  The test class for testing the ProtocolFilter Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ProtocolFilters/Common/ProtocolFilterTest.h $
// 
// 2     2/22/07 11:25a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added TestCloseCom().
// 
// 1     10/26/06 1:45p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef ProtocolFilterTest_h
#define ProtocolFilterTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"
#include "ProtocolFilter.h"

class MockProtocolFilter;

class ProtocolFilterTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(ProtocolFilterTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(TestSetAndGetLastTxTime);
    CPPUNIT_TEST(TestCloseCom);
    CPPUNIT_TEST_SUITE_END();

public:
    ProtocolFilterTest();
    virtual ~ProtocolFilterTest();
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
     * Test that will verify SetLastTxTime() properly sets the last transmit 
     * time and GetLastTxtTime() properly returns the last transmit time.
     */ 
    void TestSetAndGetLastTxTime(void);
    /**
     * Test that will verify CloseCom() works as expected.
     */ 
    void TestCloseCom(void);

private:

    /** ProtocolFilter object under test */
    MockProtocolFilter *pf;

    /** Keep alive timer object to use when constructing the protocol filter */
    KeepAliveTimer_t messageTimer;
};
#endif //ProtocolFilterTest_h
