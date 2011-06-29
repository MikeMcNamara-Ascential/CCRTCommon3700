//******************************************************************************
// Description:
//       Runs all DCX SCI module interface tests
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
//    $Log: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericDCXSCIModuleTest.h $
// 
// 2     3/22/07 2:44p Dkrause
// Initial DCX Version
// 
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
// FILE:
//    $Header: /CCRT-cmake/Source/Tests/DCX/Unit/ApplicationLayer/ModuleInterfaces/Common/GenericDCXSCIModuleTest.h 1     3/22/07 2:09p Dkrause $NoKeywords: $
//==============================================================================
#ifndef GenericDCXSCIModuleTest_h
#define GenericDCXSCIModuleTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class GenericDCXSCIModuleTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(GenericDCXSCIModuleTest);
    CPPUNIT_TEST(ExceptionTest);
    CPPUNIT_TEST_SUITE_END();

public:
    GenericDCXSCIModuleTest();
    virtual ~GenericDCXSCIModuleTest();
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
     test to make sure exceptions work     
     */
    void ExceptionTest(void);
};
#endif //GenericModuleTemplateTest_h
