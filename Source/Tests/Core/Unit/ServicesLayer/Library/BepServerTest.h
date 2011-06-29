//*************************************************************************
// FILE:
//    $Header: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/BepServerTest.h 1     10/26/06 2:08p Rwiersem $
//
// FILE DESCRIPTION:
//  The Test Class For BepServer.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ServicesLayer/Library/BepServerTest.h $
// 
// 1     10/26/06 2:08p Rwiersem
// Changes for the 06102701 core release:
// 
// Initial version.
//
//*************************************************************************
#ifndef BepServerTest_h
#define BepServerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>

#include "DebugLevel.h"

class BepServerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(BepServerTest);
    CPPUNIT_TEST(TestRegister_NoSubscribeTags);
    CPPUNIT_TEST_SUITE_END();
    
public:
    BepServerTest();
    virtual ~BepServerTest();
    
    /**
     * Setup is run before each test, use it to set up the member ivars for use 
     * during the test
     */
    void setUp();
    
    /**
     * TearDown is called after each test, use it if you need to clean things 
     * up after your test
     */
    void tearDown();

    /**
     * Test that will verify the register flag gets set to true when there are
     * no subscribe tags.
     */
    void TestRegister_NoSubscribeTags(void);
};
#endif // BepServerTest_h
