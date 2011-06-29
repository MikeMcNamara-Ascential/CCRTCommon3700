//******************************************************************************
// Description:
//  The test class for testing the Transmission module interface class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/Transmission/TransmissionModuleInterfacesTest.h $
// 
// 2     10/31/07 10:16a Rwiersem
// Changes for the 07103101 core release:
// 
// - Added GetShiftLeverLow3StringTest() and
// SetShiftLeverLow3StringTest().
// - Added SetHevLimpInTest() and GetHevLimpIntest().
// 
// 1     1/18/07 3:41p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef TransmissionModuleInterfacesTest_h
#define TransmissionModuleInterfacesTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class TransmissionModuleInterfacesTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TransmissionModuleInterfacesTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(CloseComTest);
    CPPUNIT_TEST(OpenComTest);
    CPPUNIT_TEST(GetShiftLeverLow3StringTest);
    CPPUNIT_TEST(SetShiftLeverLow3StringTest);
    CPPUNIT_TEST(SetHevLimpInTest);
    CPPUNIT_TEST(GetHevLimpInTest);
    CPPUNIT_TEST_SUITE_END();

public:
    TransmissionModuleInterfacesTest();
    virtual ~TransmissionModuleInterfacesTest();
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
     * Test that will verify CloseCom() properly deletes its monitor threads.
     */ 
    void CloseComTest(void);
    /**
     * Test that will verify OpenCom() properly initializes the member 
     * variables to their default values.
     */ 
    void OpenComTest(void);
    /**
     * Test that will verify GetShiftLeverLow3String() returns the proper
     * value.
     */ 
    void GetShiftLeverLow3StringTest(void);
    /**
     * Test that will verify SetShiftLeverLow3String() properly stores the
     * passed in value.
     */
    void SetShiftLeverLow3StringTest(void);
    /**
     * Test that will verify SetHevLimpIn() properly store the passed in value.
     */ 
    void SetHevLimpInTest(void);
    /**
     * Test that will verify GetHevLimpIn() returns the proper value.
     */ 
    void GetHevLimpInTest(void);
};
#endif //TransmissionModuleInterfacesTest_h
