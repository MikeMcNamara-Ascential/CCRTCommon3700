//******************************************************************************
// Description:
//  The GenericTCTemplate test class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Common/GenericTcTemplateTest.h $
// 
// 2     2/22/07 11:29a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added InitializeHook_DoNotLoadStaticTestResults().
// - Added m_configPath.
// 
// 1     1/18/07 4:08p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericTcTemplateTest_h
#define GenericTcTemplateTest_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

class GenericTcTemplateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericTcTemplateTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(DeactivateTest);
    CPPUNIT_TEST(InitializeHook_DoNotLoadStaticTestResults);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericTcTemplateTest();
    virtual ~GenericTcTemplateTest();
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
     * Test that will verify Deactivate() properly sets the member variables
     * to their default values.
     */ 
    void DeactivateTest(void);
    /**
     * Test that will verify InitializeHook() does not load the static test
     * results if the parameter is missing from the configuration file.
     */ 
    void InitializeHook_DoNotLoadStaticTestResults(void);

private:

    /** path to the configuration files */
    std::string m_configPath;

    /** Task id for the mock motor controller */
    int m_motorControllerId;
};
#endif //GenericTcTemplateTest_h
