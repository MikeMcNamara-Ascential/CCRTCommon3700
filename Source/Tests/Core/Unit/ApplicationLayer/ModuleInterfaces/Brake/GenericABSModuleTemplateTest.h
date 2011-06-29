//******************************************************************************
// Description:
//  The Test Class For Testing the GenericABSModuleTemplate Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Brake/GenericABSModuleTemplateTest.h $
// 
// 2     1/18/07 3:23p Rwiersem
// Changes for the 07011901 core release:
//                                                            
// - Added OpenComTest() and CloseComTest().
// 
// 1     10/26/06 1:35p Rwiersem
// Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericABSModuleTemplateTest_h
#define GenericABSModuleTemplateTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class GenericABSModuleTemplateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericABSModuleTemplateTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(ParseSensorSpeedsTest);
    CPPUNIT_TEST(ParseSensorSpeedsTest_ExceptionThrown);
    CPPUNIT_TEST(OpenComTest);
    CPPUNIT_TEST(CloseComTest);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericABSModuleTemplateTest();
    virtual ~GenericABSModuleTemplateTest();
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
     * Test that will verify ParseSensorSpeeds() returns the correct speeds for
     * the successful case.
     */ 
    void ParseSensorSpeedsTest(void);
    /**
     * Test that will verify ParseSensorSpeeds() properly handles the case 
     * where an exception is thrown parsing the response.
     */ 
    void ParseSensorSpeedsTest_ExceptionThrown(void);
    /**
     * Test that will verify OpenCom() works as expected in the successful case.
     */
    void OpenComTest(void);
    /**
     * Test that will verify CloseCom() works as expected in the successful 
     * case.
     */
    void CloseComTest(void);

private:

    /** The path to the configuration files. */
    std::string m_configPath;
};
#endif //GenericABSModuleTemplateTest_h
