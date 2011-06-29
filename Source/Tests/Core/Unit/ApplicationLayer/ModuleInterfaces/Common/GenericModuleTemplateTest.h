//******************************************************************************
// Description:
//  The unit testing class for the GenericModuleTemplateTest Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericModuleTemplateTest.h $
// 
// 1     1/18/07 3:31p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericModuleTemplateTest_h
#define GenericModuleTemplateTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class GenericModuleTemplateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericModuleTemplateTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(CloseComTest);
    CPPUNIT_TEST(IsInitializedTest);
    CPPUNIT_TEST(UninitializeTest);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericModuleTemplateTest();
    virtual ~GenericModuleTemplateTest();
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
     * Test that will verify CloseCom() works properly for the successful case.
     */ 
    void CloseComTest(void);
    /** 
     * Test that will verify IsInitialized() returns the proper value.
     */ 
    void IsInitializedTest(void);
    /**
     * Test that will verfiy Uninitialize() works properly for the successful
     * case.
     */ 
    void UninitializeTest(void);
};
#endif //GenericModuleTemplateTest_h
