//******************************************************************************
// Description:
//  The Test Class For Testing the Machine/Common Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Transmission/Common/CommonTransmissionTest.h $
// 
// 1     9/06/06 10:22a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonTransmissionTest_h
#define CommonTransmissionTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class CommonTransmissionTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonTransmissionTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST_SUITE_END();

public:
    CommonTransmissionTest();
    virtual ~CommonTransmissionTest();
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

    /** The directory to find the configuration files. */
    std::string m_configPath;

    /** Task id for the mock motor controller */
    int m_motorControllerId;
};
#endif //CommonTransmissionTest_h
