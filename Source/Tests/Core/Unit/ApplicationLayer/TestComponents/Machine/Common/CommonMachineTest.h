//******************************************************************************
// Description:
//  The Test Class For Testing the Machine/Common Class.
//
//==============================================================================
// Copyright (c) 2005 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Machine/Common/CommonMachineTest.h $
// 
// 4     5/01/07 1:26p Rwiersem
// Changes for the 07050201 core release:
// 
// Added DriverNumberNotComplete tests.
// 
// 3     1/18/07 4:18p Rwiersem
// Changes for the 07011901 core release:
// 
// - Updated to use the common MockPromptServer.
// - Added TestStepInitializeTest(), TestStepInitializeTest_Timeout(),
// TestStepInitializeTest_Fail(), TestStepInitializeTest_Abort(),
// TestStepInitializeTest_ExceptionThrown(), and 
// CheckTestStepInitializeTestResult().
// 
// 2     10/26/06 1:52p Rwiersem
// Changes for the 06102701 core release:
// 
// Added a MockTestResultServer object.
// 
// 1     9/06/06 10:17a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonMachineTest_h
#define CommonMachineTest_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class MachineTC;
class MockTestResultServer;
class MockPromptServer;

class CommonMachineTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonMachineTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(TestStepInitializeTest);
    CPPUNIT_TEST(TestStepInitializeTest_Timeout);
    CPPUNIT_TEST(TestStepInitializeTest_Fail);
    CPPUNIT_TEST(TestStepInitializeTest_Abort);
    CPPUNIT_TEST(TestStepInitializeTest_ExceptionThrown);
    CPPUNIT_TEST(TestStepReverseTest);
    CPPUNIT_TEST(TestStepReverse_FirstStatusCheckFail);
    CPPUNIT_TEST(TestStepReverse_SystemWriteFail);
    CPPUNIT_TEST(TestStepReverse_SecondStatusCheckFail);
    CPPUNIT_TEST(TestStepReverse_Timeout);
    CPPUNIT_TEST(TestStepReverse_ExceptionThrown);
    CPPUNIT_TEST(DriverNumberNotCompleteTest_DefaultDriverNumber);
    CPPUNIT_TEST(DriverNumberNotCompleteTest_EmptyDriverNumber);
    CPPUNIT_TEST(DriverNumberNotCompleteTest_False);
    CPPUNIT_TEST_SUITE_END();

public:
    CommonMachineTest();
    virtual ~CommonMachineTest();
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
     * Test that will verify TestStepInitialize() works as planned in the 
     * passing case.
     */
    void TestStepInitializeTest(void);
    /**
     * Test that will verify TestStepInitialize() works as planned when clearing
     * the prompt boxes times out.
     */ 
    void TestStepInitializeTest_Timeout(void);
    /**
     * Test that will verify TestStepInitialize() works as planned when clearing
     * the prompt boxes fails.
     */ 
    void TestStepInitializeTest_Fail(void);
    /**
     * Test that will verify TestStepInitialize() works as planned when the
     * test is aborted.
     */ 
    void TestStepInitializeTest_Abort(void);
    /**
     * Test that will verify TestStepInitialize() works as planned when clearing
     * the prompt boxes throws an exception.
     */ 
    void TestStepInitializeTest_ExceptionThrown(void);
    /**
     * Helper method to check the test results written by TestStepInitialize().
     * 
     * @param result - the result returned from TestStepInitialize()
     */ 
    void CheckTestStepInitializeTestResult(std::string result);
    /**
     * Test that will verify the TestStepReverse() method works as planned in
     * the passing case.
     */ 
    void TestStepReverseTest(void);
    /**
     * Test that will verify the TestStepReverse() method properly handles the 
     * case where the first StatusCheck() fails.
     */ 
    void TestStepReverse_FirstStatusCheckFail(void);
    /**
     * Test that will verify the TestStepReverse() method properly handles the
     * case where SystemWrite() fails.
     */ 
    void TestStepReverse_SystemWriteFail(void);
    /**
     * Test that will verify the TestStepReverse() method properly handles the 
     * case where the second StatusCheck() fails.
     */ 
    void TestStepReverse_SecondStatusCheckFail(void);
    /**
     * Test that will verify the TestStepReverse() method properly handles the
     * case where the test step times out.
     */ 
    void TestStepReverse_Timeout(void);
    /**
     * Test that will verify the TestStepReverse() method properly handles the
     * case where an exception is thrown.
     */ 
    void TestStepReverse_ExceptionThrown(void);
    /**
     * Test that will verify DriverNumberNotComplete() properly returns true
     * if the driver number is the default driver number.
     */ 
    void DriverNumberNotCompleteTest_DefaultDriverNumber(void);
    /**
     * Test that will verify DriverNumberNotComplete() properly returns true
     * if the driver number is empty.
     */ 
    void DriverNumberNotCompleteTest_EmptyDriverNumber(void);
    /**
     * Test that will verify DriverNumberNotComplete() properly returns false
     * if the driver number is valid.
     */ 
    void DriverNumberNotCompleteTest_False(void);
private:
    /** The directory to find the configuration files. */
    std::string m_configDir;

    /** Machine test component under test */
    MachineTC* mtc;

    /** Task id for the mock motor controller */
    int m_motorControllerId;

    /** Task id for the mock named data broker */
    int m_ndbId;

    /** MockTestResultServer object to use for testing */
    MockTestResultServer *mtrs;

    /** MockPromptServer object to use for testing */
    MockPromptServer *mps;
};
#endif //CommonMachineTest_h
