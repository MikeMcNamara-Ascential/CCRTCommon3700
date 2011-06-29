//******************************************************************************
// Description:
//  The Test Class For Testing the Engine Controller Module Interface Class.
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
//    $Log: /Ccrt/Source/Tests/Core/Unit/ApplicationLayer/ModuleInterfaces/EngineController/EngineControllerTest.h $
// 
// 3     2/22/07 11:22a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added assertions for setting m_inFieldModeRegister and
// m_inFieldModeBit.
// 
// - Added ReadSwitches_SkipParkNeutralSwitch().
// - Added ActivateHydraulicCoolingFan().
// - Added IsInFieldMode_True(), IsInFieldMode_False(), and
// IsInFieldMode_ReadError().
// 
// - Added GetInFieldModeRegister(), GetInFieldModeBit(),
// SetInFieldModeRegister(), and SetInFieldModeBit().
// 
// 2     1/18/07 3:34p Rwiersem
// Changes for the 07011901 core release:
//                                                                
// - Added CloseComTest().
// 
// 1     9/06/06 10:11a Rwiersem
// Changes for the 06090601 core release:
// 
// - Initial version
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef EngineControllerTest_h
#define EngineControllerTest_h

#include <cppunit/extensions/HelperMacros.h>
#include <string>
#include <stdint.h>

#include "DebugLevel.h"

class EngineControllerTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(EngineControllerTest);
    CPPUNIT_TEST(ConstructorTest);
    CPPUNIT_TEST(InitializeHook);
    CPPUNIT_TEST(InitializeHook_MissingTags);
    CPPUNIT_TEST(ReadSwitches_SwitchesOnAutomatic);
    CPPUNIT_TEST(ReadSwitches_SwitchesOffAutomatic);
    CPPUNIT_TEST(ReadSwitches_SwitchesOnManual);
    CPPUNIT_TEST(ReadSwitches_SwitchesOffManual);
    CPPUNIT_TEST(ReadSwitches_ClutchOffUpstopOnDetected);
    CPPUNIT_TEST(ReadSwitches_GetModuleDataError);
    CPPUNIT_TEST(ReadSwitches_CaughtException);
    CPPUNIT_TEST(ReadSwitches_SkipParkNeutralSwitch);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication_StatusMismatch);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication_CaughtException);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication_GetModuleDataError);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication_GetModuleDataErrorNegResp);
    CPPUNIT_TEST(StartQuickLearnEngineAuthentication_GetModuleData2Error);
    CPPUNIT_TEST(
        StartQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication_StatusMismatch);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication_CaughtException);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication_GetModuleDataError);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication_GetModuleDataErrorNegResp);
    CPPUNIT_TEST(StopQuickLearnEngineAuthentication_GetModuleData2Error);
    CPPUNIT_TEST(
        StopQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp);
    CPPUNIT_TEST(GetEngineAuthenticationRunningString);
    CPPUNIT_TEST(GetEngineAuthenticationStartPendingString);
    CPPUNIT_TEST(GetEngineAuthenticationStoppedString);
    CPPUNIT_TEST(GetEngineAuthenticationStopPendingString);
    CPPUNIT_TEST(SetEngineAuthenticationRunningString);
    CPPUNIT_TEST(SetEngineAuthenticationStartPendingString);
    CPPUNIT_TEST(SetEngineAuthenticationStoppedString);
    CPPUNIT_TEST(SetEngineAuthenticationStopPendingString);
    CPPUNIT_TEST(CloseComTest);
    CPPUNIT_TEST(ActivateHydraulicCoolingFan);
    CPPUNIT_TEST(IsInFieldMode_True);
    CPPUNIT_TEST(IsInFieldMode_False);
    CPPUNIT_TEST(IsInFieldMode_ReadError);
    CPPUNIT_TEST(GetInFieldModeRegister);
    CPPUNIT_TEST(GetInFieldModeBit);
    CPPUNIT_TEST(SetInFieldModeRegister);
    CPPUNIT_TEST(SetInFieldModeBit);
    CPPUNIT_TEST_SUITE_END();

public:
    EngineControllerTest();
    virtual ~EngineControllerTest();
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
     * Test that will verify InitializeHook() properly loads the configuration
     * values.
     */ 
    void InitializeHook(void);
    /**
     * Test that will verify InitializeHook() properly loads the default values
     * if the tags are missing from the configuration file.
     */ 
    void InitializeHook_MissingTags(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where the
     * switch states should be on and the automatic transmission flag is set.
     */
    void ReadSwitches_SwitchesOnAutomatic(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where the
     * switch states should be off and the automatic transmission flag is set.
     */
    void ReadSwitches_SwitchesOffAutomatic(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where the
     * switch states should be on and the automatic transmission flag is not 
     * set.
     */
    void ReadSwitches_SwitchesOnManual(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where the
     * switch states should be off and the automatic transmission flag is not 
     * set.
     */
    void ReadSwitches_SwitchesOffManual(void);
    /**
     * Test that will verify ReadSwitches() properly detects the clutch off
     * upstop on case when the condidtions are correct.
     */ 
    void ReadSwitches_ClutchOffUpstopOnDetected(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where
     * GetModuleData() returns an error.
     */ 
    void ReadSwitches_GetModuleDataError(void);
    /**
     * Test that will verify ReadSwitches() proplerly catches and handles an
     * exception been thrown.
     */ 
    void ReadSwitches_CaughtException(void);
    /**
     * Test that will verify ReadSwitches() properly handles the case where
     * SkipParkNeutralSwitch() is true.
     */ 
    void ReadSwitches_SkipParkNeutralSwitch(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() works in the
     * successful case.
     */ 
    void StartQuickLearnEngineAuthentication(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the status strings don't match.
     */ 
    void StartQuickLearnEngineAuthentication_StatusMismatch(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where it catches an exception.
     */ 
    void StartQuickLearnEngineAuthentication_CaughtException(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the first call to GetModuleData() fails.
     */ 
    void StartQuickLearnEngineAuthentication_GetModuleDataError(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the first call to GetModuleData() fails and 
     * the response is a negative response.
     */ 
    void StartQuickLearnEngineAuthentication_GetModuleDataErrorNegResp(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the second call to GetModuleData() fails.
     */ 
    void StartQuickLearnEngineAuthentication_GetModuleData2Error(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the second call to GetModuleData() fails and 
     * the response is a negative response.
     */ 
    void StartQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp(void);
    /**
     * Test that will verify StopQuickLearnEngineAuthentication() works in the
     * successful case.
     */ 
    void StopQuickLearnEngineAuthentication(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the status strings don't match.
     */ 
    void StopQuickLearnEngineAuthentication_StatusMismatch(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where it catches an exception.
     */ 
    void StopQuickLearnEngineAuthentication_CaughtException(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the first call to GetModuleData() fails.
     */ 
    void StopQuickLearnEngineAuthentication_GetModuleDataError(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the first call to GetModuleData() fails and 
     * the response is a negative response.
     */ 
    void StopQuickLearnEngineAuthentication_GetModuleDataErrorNegResp(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the second call to GetModuleData() fails.
     */ 
    void StopQuickLearnEngineAuthentication_GetModuleData2Error(void);
    /**
     * Test that will verify StartQuickLearnEngineAuthentication() properly
     * handles the case where the second call to GetModuleData() fails and 
     * the response is a negative response.
     */ 
    void StopQuickLearnEngineAuthentication_GetModuleData2ErrorNegResp(void);
    /**
     * Test that will verify GetEngineAuthenticationRunningString() returns
     * the correct value.
     */ 
    void GetEngineAuthenticationRunningString(void);
    /**
     * Test that will verify GetEngineAuthenticationStartPendingString() returns
     * the correct value.
     */
    void GetEngineAuthenticationStartPendingString(void);
    /**
     * Test that will verify GetEngineAuthenticationStoppedString() returns
     * the correct value.
     */
    void GetEngineAuthenticationStoppedString(void);
    /**
     * Test that will verify GetEngineAuthenticationStopPendingString() returns
     * the correct value.
     */
    void GetEngineAuthenticationStopPendingString(void);
    /**
     * Test that will verify SetEngineAuthenticationRunningString() properly
     * sets the member variable.
     */ 
    void SetEngineAuthenticationRunningString(void);
    /**
     * Test that will verify SetEngineAuthenticationStartPendingString() 
     * properly sets the member variable.
     */ 
    void SetEngineAuthenticationStartPendingString(void);
    /**
     * Test that will verify SetEngineAuthenticationStoppedString() properly 
     * sets the member variable.
     */ 
    void SetEngineAuthenticationStoppedString(void);
    /**
     * Test that will verify SetEngineAuthenticationStopPendingString() properly 
     * sets the member variable.
     */ 
    void SetEngineAuthenticationStopPendingString(void);
    /**
     * Test that will verify CloseCom() properly sets the member variables to
     * their default values.
     */ 
    void CloseComTest(void);
    /**
     * Test that will verify ActivateHydraulicCoolingFan() works as expected.
     */ 
    void ActivateHydraulicCoolingFan(void);
    /**
     * Test that will verify IsInFieldMode() properly returns true when the
     * module is in field mode.
     */ 
    void IsInFieldMode_True(void);
    /**
     * Test that will verify IsInFieldMode() properly returns false when the
     * module is not in field mode.
     */ 
    void IsInFieldMode_False(void);
    /**
     * Test that will verify IsInFieldMode() properly handles the case where
     * there is an error reading the data from the module.
     */ 
    void IsInFieldMode_ReadError(void);
    /**
     * Test that will verify GetInFieldModeRegister() properly returns the
     * value of the member variable that holds the in field mode register.
     */ 
    void GetInFieldModeRegister(void);
    /**
     * Test that will verify GetInFieldModeBit() properly returns the value of
     * the member variable that holds the in field mode bit.
     */ 
    void GetInFieldModeBit(void);
    /**
     * Test that will verify SetInFieldModeRegister() properly sets the value of
     * the member variable that holds the in field mode register.
     */ 
    void SetInFieldModeRegister(void);
    /**
     * Test that will verify SetInFieldModeBit() properly sets the value of the
     * member variable that holds the in field mode bit.
     */ 
    void SetInFieldModeBit(void);

private:
    /** The path to the configuration files. */
    std::string m_configPath;
};
#endif //EngineControllerTest_h
