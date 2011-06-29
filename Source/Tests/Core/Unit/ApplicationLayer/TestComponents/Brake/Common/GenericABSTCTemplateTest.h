//******************************************************************************
// Description:
//  The test class for testing the GenericABSTCTemplate class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Brake/Common/GenericABSTCTemplateTest.h $
// 
// 1     1/18/07 4:04p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef GenericABSTCTemplate_h
#define GenericABSTCTemplate_h
//==============================================================================

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

/**
 * Test class for GenericABSTCTemplate
 */
class GenericABSTCTemplateTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericABSTCTemplateTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(SetESPEquippedTest);
    CPPUNIT_TEST(IsESPEquippedTest);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericABSTCTemplateTest();
    virtual ~GenericABSTCTemplateTest();

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
    /** Test that will verify SetESPEquipped() sets the proper value. */
    void SetESPEquippedTest(void);
    /** Test that will verify IsESPEquipped() returns the proper value. */
    void IsESPEquippedTest(void);

private:
    /** The directory to find the configuration files in. */
    std::string m_configDir;

    /** Task id for the mock motor controller */
    int m_motorControllerId;
};
#endif //GenericABSTCTemplate_h
