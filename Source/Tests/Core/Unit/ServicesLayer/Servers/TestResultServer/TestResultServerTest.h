//*************************************************************************
// Description:
//   Test Result Server unit test class.
//
//===========================================================================
// Copyright (c) 2006 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Servers/TestResultServer/TestResultServerTest.h $
// 
// 4     5/01/07 1:51p Rwiersem
// Changes for the 07050201 core release:
// 
// Added TestPublish_RelayFile().
// 
// 3     10/26/06 3:13p Rwiersem
// Changes for the 06102701 core release:
// 
// Added tests for setting the fault and accept PLC lamp bits.
// 
// 2     9/22/06 11:29a Rwiersem
// Changes for the 06092201 core release:
//     - Added TestInitialize()
//     - Added TestReportResult()
//     - Added ITestResultServer *m_testResult
//
//*************************************************************************
// $NoKeywords: $
//==========================================================================
#ifndef TestResultServerTest_h
#define TestResultServerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

using namespace std;
class TestResultServer;
class ITestResultServer;

class TestResultServerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestResultServerTest);
    CPPUNIT_TEST(TestConstructor);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_MissingTags);
    CPPUNIT_TEST(TestLoadAdditionalConfigurationItems_InvalidName);
    CPPUNIT_TEST(TestHandlePulse);
    CPPUNIT_TEST(TestInitialize);
    CPPUNIT_TEST(TestReportResults);
    CPPUNIT_TEST(TestPublish_PlcLampsCleared);
    CPPUNIT_TEST(TestPublish_PlcLampsTestPass);
    CPPUNIT_TEST(TestPublish_PlcLampsTestFail);
    CPPUNIT_TEST(TestPublish_TestResultsCleared);
    CPPUNIT_TEST(TestPublish_RelayFile);
    CPPUNIT_TEST_SUITE_END();

public:
    TestResultServerTest();
    ~TestResultServerTest();
    
    void setUp();
    void tearDown();

    /**
     * Test that will verify the TestResultServer object is 
     * constructed properly
     */ 
    void TestConstructor(void);
    /**
     * Test that will verify the LoadAdditionsConfigurationItems()
     * method properly reads the TestResultServer configuration items
     * from the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems(void);
    /**
     * Test that will verify the items set in the 
     * LoadAdditionalConfiguraitonItems() method are set to the default
     * if the tag is missing in the configuration file.
     */ 
    void TestLoadAdditionalConfigurationItems_MissingTags(void);
    /**
     * Test that will verify the communication channel name is set to the
     * default if the name specified in the configuration file is invalid.
     */ 
    void TestLoadAdditionalConfigurationItems_InvalidName(void);
    /**
     * Test that will verify the HandlePulse() method properly processes the
     * reload configuration pulse.
     */ 
    void TestHandlePulse(void);
    /**
     * Test that will verify a proper initialize function 
     * call
     */
    void TestInitialize(void);
    /**
     * Test that will open and parse the output test result file
     * to verify that the test result was built properly
     */ 
    void TestReportResults(void);
    /**
     * Test that will verify the PLC lamps are cleared when a new test is 
     * started.
     */ 
    void TestPublish_PlcLampsCleared(void);
    /**
     * Test that will verify the PLC lamps are in the correct state when a test
     * passes.
     */ 
    void TestPublish_PlcLampsTestPass(void);
    /**
     * Test that will verify the PLC lamps are in the correct state when a test
     * fails.
     */ 
    void TestPublish_PlcLampsTestFail(void);
    /**
     * Test that will verify the test results are cleared at the end of a test.
     */ 
    void TestPublish_TestResultsCleared(void);
    /**
     * Test that will verify the test result file name is properly written when
     * "RelayFile" is published.
     */ 
    void TestPublish_RelayFile(void);

    /** The TestResultServer object under test */
    TestResultServer *trs;

    ITestResultServer *m_testResult;

    /** The path to the test configuration files */
    string m_configPath;
};
#endif
