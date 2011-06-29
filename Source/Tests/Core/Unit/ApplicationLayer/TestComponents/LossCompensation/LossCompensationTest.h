//******************************************************************************
// Description:
//  The test class for testing the Loss Compensation class.
//
//==============================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/LossCompensation/LossCompensationTest.h $
// 
// 1     1/18/07 4:14p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef LossCompensationTest_h
#define LossCompensationTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class LossCompensationTC;
class MockFaultServer;
class MockNamedDataBroker;

class LossCompensationTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(LossCompensationTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(AnalyzeForcesTest);
    CPPUNIT_TEST(AnalyzeForcesTest_NoiseError);
    CPPUNIT_TEST(AnalyzeForcesTest_RunoutError);
    CPPUNIT_TEST_SUITE_END();

public:
    LossCompensationTest();
    virtual ~LossCompensationTest();
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
     * Test that will verify AnalyzeForces() works as expected in the 
     * successful case.
     */ 
    void AnalyzeForcesTest(void);
    /**
     * Test that will verify AnalyzeForces() works as expected when there is
     * an encoder noise error.
     */ 
    void AnalyzeForcesTest_NoiseError(void);
    /**
     * Test that will verify AnalyzeForces() works as expected when there is
     * an encoder runout error.
     */ 
    void AnalyzeForcesTest_RunoutError(void);

private:

    /** LossCompensationTC object to use for testing */
    LossCompensationTC *lctc;

    /** Task id for the mock motor controller */
    int m_motorControllerId;

    /** Task id for the mock named data broker */
    int m_ndbId;

    /** The directory to find the configuration files. */
    std::string m_configDir;

    /** mock fault server object to use for testing */
    MockFaultServer *mfs;

    /** mock named data broker object to use for testing */
    MockNamedDataBroker *mndb;
};
#endif //LossCompensationTest_h
