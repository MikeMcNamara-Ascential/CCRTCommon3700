//******************************************************************************
// Description:
//  The Test Class For Testing the SpeedControl/Common Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/SpeedControl/Common/CommonSpeedControlTest.h $
// 
// 2     1/18/07 4:44p Rwiersem
// Changes for the 07011901 core release:
//       
// - Removed WarmInitializeTest().
// 
// 1     10/26/06 1:56p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonSpeedControlTest_h
#define CommonSpeedControlTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class MockTestResultServer;
                               
class CommonSpeedControlTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonSpeedControlTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST_SUITE_END();

public:
    CommonSpeedControlTest();
    virtual ~CommonSpeedControlTest();
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

    /** Task id for the mock motor controller */
    int m_motorControllerId;

    /** MockTestResultServer object to use for testing */
    MockTestResultServer *mtrs;
};
#endif //CommonSpeedControlTest_h
