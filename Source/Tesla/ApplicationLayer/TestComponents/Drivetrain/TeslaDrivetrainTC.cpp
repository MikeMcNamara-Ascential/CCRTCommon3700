//******************************************************************************
// FILE:
//    $Header: $
//
// FILE DESCRIPTION:
//
//==============================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//    $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "TeslaDrivetrainTC.h"

//-----------------------------------------------------------------------------
TeslaDrivetrainTC::TeslaDrivetrainTC() : GenericTC()
{
    // Nothing
}

//-----------------------------------------------------------------------------
TeslaDrivetrainTC::~TeslaDrivetrainTC()
{
}

//-----------------------------------------------------------------------------
void TeslaDrivetrainTC::Initialize(const XmlNode *config)
{   // Call the base class to do the initialization
    GenericTC::Initialize(config);
    // Signal initialization complete
    InitializationComplete();
}

//-----------------------------------------------------------------------------
const string TeslaDrivetrainTC::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "TeslaDrivetrain::CommandTestStep(%s) - Enter", value.c_str());
    // Make sure it is ok to perform the test step
    if(BEP_STATUS_SUCCESS == StatusCheck())
    {
        if(!GetTestStepName().compare("Cruise"))                    testResult = Cruise();
        else if(!GetTestStepName().compare("BrakeToSpeed"))   
        {
            testResult = SlowDownToTestSpeed(GetParameterFloat("BrakeToSpeedTargetSpeed"), GetParameter("BrakeToSpeedTargetRange"),
                                             GetTestStepInfoInt("ScanDelay"), true, GetParameter("BrakeToTargetSpeedPrompt"));
        }
        else if(!GetTestStepName().compare("ReverseWarning"))       testResult = ReverseWarning();
        else if(!GetTestStepName().compare("DriveProfileSetup"))    testResult = SetupMachineForDriveProfileTest();
        else if(!GetTestStepName().compare("RestoreMachineState"))  testResult = ResetMachineToInitialCondition();
        else if(!GetTestStepName().compare("VehicleCheckoutCycle")) testResult = VehicleCheckOutCycle();
        else if(!GetTestStepName().compare("FinishUp"))
        {   // send the overall test result
            SendOverallResult();
            SendTestResult(testPass, GetTestStepInfo("Description"), "0000");
            testResult = testPass;
        }
        else  testResult = GenericTC::CommandTestStep(value);
    }
    else
    {   // System is not ready to perform the test
        testResult = testSkip;
        Log(LOG_ERRORS, "System status preventing test: %s", ConvertStatusToResponse(StatusCheck()).c_str());
    }
    Log(LOG_FN_ENTRY, "TeslaDrivetrain::CommandTestStep(%s) - returning: %s", value.c_str(), testResult.c_str());
    return testResult;
}

//=============================================================================
const string TeslaDrivetrainTC::SetupMachineForDriveProfileTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::SetupMachineForDriveProfileTest() - Enter");
    if(!ShortCircuitTestStep())
    {   // Store the oringinal drive axle
        string driveAxle(SystemRead(DRIVE_AXLE_TAG));
        OriginalDriveAxle(&driveAxle);
        // Switch the drive axle, but make sure we believe in the real drive axle
        SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
        BposSleep(1000);
        SetData(DRIVE_AXLE_TAG, OriginalDriveAxle());
        // Place the motors in torque mode
        SystemWrite(MOTOR_MODE, string(TORQUE_MODE));
        SystemWrite(COMMAND_TORQUE, GetParameterFloat("DriveProfileTorque"));
        // Report the result
        testResult = testPass;
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping SetupMachineForDriveProfileTest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::SetupMachineForDriveProfileTest() - Exit");
    return testResult;
}

//=============================================================================
const string TeslaDrivetrainTC::ResetMachineToInitialCondition(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::ResetMachineToInitialCondition() - Enter");
    if(!ShortCircuitTestStep())
    {   // Disable the torque setting
        SystemWrite(COMMAND_TORQUE, string("0"));
        // Restore the original drive axle
        SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
        BposSleep(500);
        // Place the motors in Boost mode
        SystemWrite(MOTOR_MODE, string(BOOST_MODE));
        // Report the result
        testResult = testPass;
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping ResetMachineToInitialCondition");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::ResetMachineToInitialCondition() - Exit");
    return testResult;
}

//=============================================================================
const string TeslaDrivetrainTC::Cruise(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::Cruise - Enter");
    if(!ShortCircuitTestStep())
    {
        string testResultCode("0000");
        string testDescription(GetTestStepInfo("Description"));
        // Make sure vehicle speed is in range
        testResult = AccelerateToTestSpeed(GetParameterFloat("DriveProfileTargetSpeed"), 
                                           GetParameter("DriveProfileTargetSpeedRange"),
                                           GetTestStepInfoInt("ScanDelay"), false, "AccelerateToTargetSpeed");
        // Make sure conditions are correct to continue this test
        if(!testResult.compare(testPass) && TimeRemaining())
        {   // Prompt the operator to perform the cruise tests
            DisplayPrompt(GetPromptBox("SpeedControlTest"), GetPrompt("SpeedControlTest"), GetPromptPriority("SpeedControlTest"));
            DisplayPrompt(GetPromptBox("PerformSpeedControlTest"), GetPrompt("PerformSpeedControlTest"), GetPromptPriority("PerformSpeedControlTest"));
            BEP_STATUS_TYPE status = StatusSleep(GetParameterInt("CruiseTime"));
            RemovePrompt(GetPromptBox("SpeedControlTest"), GetPrompt("SpeedControlTest"), GetPromptPriority("SpeedControlTest"));
            RemovePrompt(GetPromptBox("PerformSpeedControlTest"), GetPrompt("PerformSpeedControlTest"), GetPromptPriority("PerformSpeedControlTest"));
            // Determine the result
            testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
            Log(LOG_DEV_DATA, "Cruise test status: %s, result: %s", ConvertStatusToResponse(status).c_str(), testResult.c_str());
        }
        else
        {   // Failed to reach the target speed
            Log(LOG_ERRORS, "Failed to reach target speed in time - testResult: %s", testResult.c_str());
        }
        // Report the result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step Cruise");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::Cruise - Exit: %s", testResult.c_str());
    return testResult;
}

//=============================================================================
const string TeslaDrivetrainTC::ReverseWarning(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::ReverseWarning() - Enter");
    if(!ShortCircuitTestStep())
    {   // Make sure we are in the correct speed range
        testResult = AccelerateToTestSpeed(GetParameterFloat("ReverseWarningTargetSpeed"), GetParameter("ReverseWarningSpeedRange"),
                                           GetTestStepInfoInt("ScanDelay"), false);
        // Make sure there is time remaining and we are in the correct speed range
        if(!testResult.compare(testPass) && TimeRemaining())
        {   // Instruct the operator to shift to reverse and check the warning lamp
            SystemWrite(GetDataTag("SpeedTarget"), GetParameter("ReverseWarningTargetSpeed"));
            DisplayPrompt(GetPromptBox("MaintainSpeedShiftToR"), GetPrompt("MaintainSpeedShiftToR"), GetPromptPriority("MaintainSpeedShiftToR"));
            testResult = OperatorPassFail("ReverseWarningLamp", GetParameterInt("ReverseWarningTimeout"));
            Log(LOG_ERRORS, "Reverse warning lamp operator result: %s", testResult.c_str());
            RemovePrompt(GetPromptBox("MaintainSpeedShiftToR"), GetPrompt("MaintainSpeedShiftToR"), GetPromptPriority("MaintainSpeedShiftToR"));
        }   
        else
        {
            Log(LOG_ERRORS, "Failed to reach the target speed: %s", testResult.c_str());
        }
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping reverse warning test");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::ReverseWarning() - Exit");
    return testResult;
}

//=============================================================================
const string TeslaDrivetrainTC::VehicleCheckOutCycle(void)
{
    string testResult(testPass);
    // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::VehicleCheckOutCycle() - Enter");
    if(!ShortCircuitTestStep())
    {   
        for(INT32 cycle = 0; 
             (cycle < GetParameterInt("VehicleCheckOutAccelerationCycles")) && 
             (BEP_STATUS_SUCCESS == StatusCheck()) && !testResult.compare(testPass);
             cycle++)
        {   // Have the operator accelerate to the test speed
            testResult = AccelerateToTestSpeed(GetParameterFloat("VehicleCheckoutTargetSpeed"), 
                                               GetParameter("VehicleCheckoutTargetSpeedRange"),
                                               GetTestStepInfoInt("ScanDelay"), false);
            // Now tell the operator to coast down to speed
            if (testResult == testPass) {
                testResult = SlowDownToTestSpeed(GetParameterFloat("BrakeToSpeedTargetSpeed"), GetParameter("BrakeToSpeedTargetRange"),
                                             GetTestStepInfoInt("ScanDelay"), true, GetParameter("BrakeToTargetSpeedPrompt"));
            }
        }
        // Check the exit condition
        if(BEP_STATUS_SUCCESS != StatusCheck())
        {
            testResult = testFail;
        }
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping vehicle check out cycle");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TeslaDrivetrainTC::VehicleCheckOutCycle() - Exit");
    return testResult;
}

//=============================================================================
inline const string& TeslaDrivetrainTC::OriginalDriveAxle(const string *driveAxle /*= NULL*/)
{
    if(driveAxle != NULL)  m_originalDriveAxle = *driveAxle;
    return m_originalDriveAxle;
}
