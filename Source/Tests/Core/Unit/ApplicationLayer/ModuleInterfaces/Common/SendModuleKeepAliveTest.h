//******************************************************************************
// Description:
//  The test class for testing the SendModuleKeepAlive Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/SendModuleKeepAliveTest.h $
// 
// 1     10/26/06 1:40p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef SendModuleKeepAliveTest_h
#define SendModuleKeepAliveTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class SendModuleKeepAlive;
class KeywordProtocolFilter;

class SendModuleKeepAliveTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(SendModuleKeepAliveTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST_SUITE_END();

public:
    SendModuleKeepAliveTest();
    virtual ~SendModuleKeepAliveTest();
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

private:

    /** SendModuleKeepAlive object under test */
    SendModuleKeepAlive *smka;
    /** ProtocolFilter object used to construct SendModuleKeepAlive */
    KeywordProtocolFilter *pf;
};
#endif //SendModuleKeepAliveTest_h
