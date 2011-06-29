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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Brake/Teves/TevesMk20EModuleTest.h $
// 
// 1     9/07/07 10:08a Dkrause
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
#ifndef TevesMk20EModuleTest_h
#define TevesMk20EModuleTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class TevesMk20EModuleTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TevesMk20EModuleTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST_SUITE_END();

public:
    TevesMk20EModuleTest();
    virtual ~TevesMk20EModuleTest();
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

    /** The path to the configuration files. */
    std::string m_configPath;
};
#endif //TevesMk25EModuleTest_h
