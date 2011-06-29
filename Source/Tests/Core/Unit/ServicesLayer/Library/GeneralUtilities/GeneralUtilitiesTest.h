//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTest.h 1     10/31/07 10:24a Rwiersem $
//
// FILE DESCRIPTION:
//  The GeneralUtilities test class.
//  
//===========================================================================
// Copyright (c) 2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/GeneralUtilities/GeneralUtilitiesTest.h $
// 
// 1     10/31/07 10:24a Rwiersem
// Changes for the 07103101 core release:
// 
// - Initial version.
//
//*************************************************************************
#ifndef GeneralUtilitesTest_h
#define GeneralUtilitesTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "DebugLevel.h"

class GeneralUtilitiesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GeneralUtilitiesTest);
    CPPUNIT_TEST(ApplyForceMultiplierTest);
    CPPUNIT_TEST(ApplyForceMultiplierTest_ZeroMultiplier);
    CPPUNIT_TEST_SUITE_END();
    
public:
    /**
     * Class constructor
     */
    GeneralUtilitiesTest();

    /**
     * Class destructor
     */
    virtual ~GeneralUtilitiesTest();
    
    /**
     * Setup is run before each test, use it to set up the member ivars for use 
     * during the test.
     */
    void setUp();
    
    /**
     * TearDown is called after each test, use it if you need to clean things 
     * up after your test.
     */
    void tearDown();

    /**
     * Test that will verify ApplyForceMultiplier() returns the correct result
     * when the mulitiplier is not 0.
     */
    void ApplyForceMultiplierTest(void);

    /**
     * Test that will verify ApplyForceMultiplier() returns the correct result
     * when the multiplier is 0.
     */ 
    void ApplyForceMultiplierTest_ZeroMultiplier(void);
};
#endif // GeneralUtilitesTest_h
