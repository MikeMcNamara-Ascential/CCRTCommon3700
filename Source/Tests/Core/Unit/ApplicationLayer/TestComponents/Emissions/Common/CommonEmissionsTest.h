//******************************************************************************
// Description:
//  The Test Class For Testing the Emissions/Common Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/TestComponents/Emissions/Common/CommonEmissionsTest.h $
// 
// 2     5/01/07 1:23p Rwiersem
// Changes for the 07050201 core release:
// 
// Added StartHydraulicCoolingFanTest_StartModuleKeepAlive().  Added
// StopHydraulicCoolingFanTest_StopModuleKeepAlive().  Added
// DetermineModuleConfig() tests.
// 
// 1     1/18/07 4:12p Rwiersem
// Changes for the 07011901 core release:
// 
// - Initial version.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef CommonEmissionsTest_h
#define CommonEmissionsTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class CommonEmissionsTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CommonEmissionsTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_Pass);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_Skip);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_FailNoAbort);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_FailAbort);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_ModuleException);
    CPPUNIT_TEST(StartHydraulicCoolingFanTest_StartModuleKeepAlive);
    CPPUNIT_TEST(StopHydraulicCoolingFanTest_Pass);
    CPPUNIT_TEST(StopHydraulicCoolingFanTest_Skip);
    CPPUNIT_TEST(StopHydraulicCoolingFanTest_Fail);
    CPPUNIT_TEST(StopHydraulicCoolingFanTest_ModuleException);
    CPPUNIT_TEST(StopHydraulicCoolingFanTest_StopModuleKeepAlive);
    CPPUNIT_TEST(NeedToRunHydraulicCoolingFanTest_True);
    CPPUNIT_TEST(NeedToRunHydraulicCoolingFanTest_False);
    CPPUNIT_TEST(DetermineModuleConfigTest_Pass);
    CPPUNIT_TEST(DetermineModuleConfigTest_EngineSizeFail);
    CPPUNIT_TEST(DetermineModuleConfigTest_VehicleTypeFail);
    CPPUNIT_TEST(DetermineModuleConfigTest_EmissionsYearFail);
    CPPUNIT_TEST(DetermineModuleConfigTest_EmissionsTypeFail);
    CPPUNIT_TEST(DetermineModuleConfigTest_TestSkip);
    CPPUNIT_TEST_SUITE_END();

public:
    CommonEmissionsTest();
    virtual ~CommonEmissionsTest();
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
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * successful case.
     */ 
    void StartHydraulicCoolingFanTest_Pass(void);
    /**
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * test skipped case.
     */ 
    void StartHydraulicCoolingFanTest_Skip(void);
    /**
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * fail case with the parameter set to not abort.
     */ 
    void StartHydraulicCoolingFanTest_FailNoAbort(void);
    /**
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * fail case with the parameter set to abort.
     */ 
    void StartHydraulicCoolingFanTest_FailAbort(void);
    /**
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * case where a module exception is thrown.
     */ 
    void StartHydraulicCoolingFanTest_ModuleException(void);
    /**
     * Test that will verify StartHydraulicCoolingFan() works properly for the
     * case where the module keep alive should be started.
     */ 
    void StartHydraulicCoolingFanTest_StartModuleKeepAlive(void);
    /**
     * Test that will verify StopHydraulicCoolingFan() works properly for the
     * successful case.
     */ 
    void StopHydraulicCoolingFanTest_Pass(void);
    /**
     * Test that will verify StopHydraulicCoolingFan() works properly for the
     * test skipped case.
     */ 
    void StopHydraulicCoolingFanTest_Skip(void);
    /**
     * Test that will verify StopHydraulicCoolingFan() works properly for the
     * fail case.
     */ 
    void StopHydraulicCoolingFanTest_Fail(void);
    /**
     * Test that will verify StopHydraulicCoolingFan() works properly for the
     * case where a module exception is thrown.
     */ 
    void StopHydraulicCoolingFanTest_ModuleException(void);
    /**
     * Test that will verify StopHydraulicCoolingFan() works properly for the
     * case where the keep alive thread should be stopped.
     */ 
    void StopHydraulicCoolingFanTest_StopModuleKeepAlive(void);
    /**
     * Test that will verify NeedToRunHydraulicCoolingFan() returns true
     * when the hydraulic cooling fan should run.
     */ 
    void NeedToRunHydraulicCoolingFanTest_True(void);
    /**
     * Test that will verify NeedToRunHydraulicCoolingFan() returns false
     * when the hydraulic cooling fan should not run.
     */
    void NeedToRunHydraulicCoolingFanTest_False(void);
    /**
     * Test that will verify DetermineModuleConfig() works properly for the
     * successful case.
     */ 
    void DetermineModuleConfigTest_Pass(void);
    /**
     * Test that will verify DetermineModuleConfig() properly handles the case
     * where reading the engine size from the module fails.
     */ 
    void DetermineModuleConfigTest_EngineSizeFail(void);
    /**
     * Test that will verify DetermineModuleConfig() properly handles the case
     * where reading the vehicle type from the module fails.
     */ 
    void DetermineModuleConfigTest_VehicleTypeFail(void);
    /**
     * Test that will verify DetermineModuleConfig() properly handles the case
     * where reading the emissions year from the module fails.
     */ 
    void DetermineModuleConfigTest_EmissionsYearFail(void);
    /**
     * Test that will verify DetermineModuleConfig() properly handles the case
     * where reading the emissions type from the module fails.
     */ 
    void DetermineModuleConfigTest_EmissionsTypeFail(void);
    /**
     * Test that will verify DetermineModuleConfig() properly handles the case
     * where the test is skipped.
     */ 
    void DetermineModuleConfigTest_TestSkip(void);

private:

    /** The directory to find the configuration files. */
    std::string m_configPath;

    /** Task id for the mock motor controller */
    int m_motorControllerId;

    /** Task id for the mock named data broker */
    int m_ndbId;
};
#endif //CommonEmissionsTest_h
