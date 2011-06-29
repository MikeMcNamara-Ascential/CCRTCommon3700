//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Mercedes Speed Control.
//
//===========================================================================
// Copyright (c) 2011- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MercedesSpeedControlTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MercedesSpeedControlTc<ModuleType>::MercedesSpeedControlTc() : GenericSpeedControlTC<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MercedesSpeedControlTc<ModuleType>::~MercedesSpeedControlTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string MercedesSpeedControlTc<ModuleType>::CheckSetEngagement(void)
{
    string testResultCode  = "0000";
    string testDescription = GetTestStepInfo("Description");
    string speedInRange    = testFail;
    string operatorInput;
    bool   cruiseSet       = false;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if this test step should be performed
    if (!ShortCircuitTestStep())
    {   // Make sure the ON test step has passed
        if (!m_testStepStatus.verifyOnSwitch.compare(testPass) || GetParameterBool("SkipOnTestResult"))
        {   // Get maximum vehicle speed from the build data and convert to mph
            float maxVehicleSpeed = SystemReadFloat(GetDataTag("VehicleMaxSpeedTag")) / MphToKph;
            if(maxVehicleSpeed == 0.0)  maxVehicleSpeed = GetParameterFloat("SpeedControlMaxSpeed");
            float minVehicleSpeed = maxVehicleSpeed - GetParameterFloat("SpeedControlTargetRange");
            // Wait for vehicle speed to be in range
            speedInRange = WaitAndPromptForSpeed(minVehicleSpeed, maxVehicleSpeed, GetParameterInt("SpeedRangeCheckDelay"));
            // Check if speed range was achieved
            if (testPass == speedInRange)
            {   // Speed is in range, set the cruise
                DisplayPrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
                DisplayPrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
                              GetPromptPriority("RemoveFootPrompt"));
                // Setup the InputServer to get operator input
                SetupForOperatorInput();
                // Set the timer so we can look for the SET switch
                SetStartTime();
                // Wait for the operator to press the set button
                status = WaitForSetButton(cruiseSet, operatorInput);
                // Check the exit conditions
                if (!TimeRemaining() && !cruiseSet && (operatorInput.compare(testFail) != 0))
                {   // Timeout waiting for SET button to be pressed
                    Log(LOG_ERRORS, "Timeout waiting for speed control to be SET\n");
                    m_testStepStatus.verifySetEnable = testTimeout;
                    testResultCode = GetFaultCode("SpeedControlSetTimeout");
                    testDescription = GetFaultDescription("SpeedControlSetTimeout");
                }
                else if ((BEP_STATUS_SUCCESS != StatusCheck()) && !cruiseSet && (operatorInput.compare(testFail) != 0))
                {   // Bad system status waiting for cruise set
                    Log(LOG_ERRORS, "Bad system status waiting for speed control SET - status: %s\n",
                        ConvertStatusToResponse(StatusCheck()).c_str());
                    m_testStepStatus.verifySetEnable = testFail;
                    testResultCode = GetFaultCode("SystemStatus");
                    testDescription = GetFaultDescription("SystemStatus");
                }
                else if (BEP_STATUS_SUCCESS != status)
                {   // Communication failure waiting for cruise set
                    Log(LOG_ERRORS, "Communication Failure checking for speed control SET - status: %s\n",
                        ConvertStatusToResponse(status).c_str());
                    m_testStepStatus.verifySetEnable = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                }
                else if (!cruiseSet && (operatorInput.compare(testFail) == 0))
                {   // Operator pressed the fail button
                    Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
                    m_testStepStatus.verifySetEnable = testFail;
                    testResultCode = GetFaultCode("OperatorFailSet");
                    testDescription = GetFaultDescription("OperatorFailSet");
                }
                else
                {   // SET button pressed, look for speed stabilization
                    bool zeroThrottle = false;
                    RemovePrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
                    DisplayPrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
                                  GetPromptPriority("CheckStableSpeed"));
                    // Store the target speed
                    status = SetSpeedControlTargetSpeed();
                    // Check the status of the read
                    if (BEP_STATUS_SUCCESS == status)
                    {   // Check for stable speed
                        if (IsVehicleSpeedStable(GetSpeedControlTargetSpeed(), zeroThrottle))
                        {   // Test passed
                            m_testStepStatus.verifySetEnable = testPass;
                            testResultCode = "0000";
                            testDescription = GetTestStepInfo("Description");
                        }
                        else
                        {   // Test failed
                            m_testStepStatus.verifySetEnable = testFail;
                            testResultCode = zeroThrottle ? GetFaultCode("VehicleSpeedNotStable") : GetFaultCode("ThrottleApplied");
                            testDescription = zeroThrottle ? GetFaultDescription("VehicleSpeedNotStable") : GetFaultDescription("ThrottleApplied");
                        }
                        Log("Cruise SET feature functional - %s\n", m_testStepStatus.verifySetEnable.c_str());
                    }
                    else
                    {   // Communication error
                        m_testStepStatus.verifySetEnable = testFail;
                        testResultCode = GetFaultCode("CommunicationFailure");
                        testDescription = GetFaultDescription("CommunicationFailure");
                        Log("Error reading target speed for SET - status: %s\n", ConvertStatusToResponse(status).c_str());
                    }
                }
                // Remove all prompts
                RemovePrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
                RemovePrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
                             GetPromptPriority("CheckStableSpeed"));
                RemovePrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
                             GetPromptPriority("RemoveFootPrompt"));
            }
            else
            {   // Speed was not in range
                m_testStepStatus.verifySetEnable = testFail;
                testResultCode = GetFaultCode("SpeedNotInRange");
                testDescription = GetFaultDescription("SpeedNotInRange");
                Log("Vehicle Speed out of range to check SET engagement\n");
            }
        }
        else
        {   // Cruise was not ON, cannot perform this step
            Log("Error - cruise ON mode not seen prior to SET test\n");
            m_testStepStatus.verifySetEnable = testSkip;
            testResultCode = GetFaultCode("ConditionsNotCorrect");
            testDescription = GetFaultDescription("ConditionsNotCorrect");
        }
        // Report the results
        char temp[16];
        SendSubtestResultWithDetail(GetTestStepName(), m_testStepStatus.verifySetEnable, testDescription, testResultCode,
                                    "TargetSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", GetSpeedControlTargetSpeed()), unitsMPH);
    }
    else
    {   // Skipped test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        // Only update the test step if not already pass
        if (m_testStepStatus.verifySetEnable != testPass)  m_testStepStatus.verifySetEnable = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return m_testStepStatus.verifySetEnable;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
BEP_STATUS_TYPE MercedesSpeedControlTc<ModuleType>::WaitForSetButton(bool &cruiseSet, string &operatorInput)
{
    BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
    UINT16 cruiseStatus     = 0x0000;
    operatorInput           = "Unknown";
    // If we have module comms, wait for the operator to press a button
    if (!GetParameterBool("NoModuleComms"))
    {   // Wait for the operator to press the set button
        do
        {   // Look for the SET switch
            status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadSetButton"), cruiseStatus);
            // Check the status of the read
            if (BEP_STATUS_SUCCESS == status)
            {   // Good read, check if SET or operator fail
                cruiseSet = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlSetMask").c_str()), ALL_BITS_SET);
                operatorInput = CheckForOperatorFail();
                if (!cruiseSet && (operatorInput.compare(testFail) != 0))
                {   // Give a little time before the next check
                    BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
                }
            }
            else
            {   // Communication error
                Log("Error reading switch status for SET - status: %s\n", ConvertStatusToResponse(status).c_str());
            }
            // Keep checking while time remaining, good system status, good comms and set mode not seen
        } while (TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
                 !cruiseSet && (operatorInput.compare(testFail) != 0));
    }
    else
    {   // Assume the set button has been pressed if the vehicle speed is stable
        float maxVehicleSpeed = SystemReadFloat(GetDataTag("VehicleMaxSpeedTag")) / MphToKph;
        if(maxVehicleSpeed == 0.0)  maxVehicleSpeed = GetParameterFloat("SpeedControlMaxSpeed");
        float minVehicleSpeed = maxVehicleSpeed - GetParameterFloat("SpeedControlTargetRange");
        float targetSpeed = (maxVehicleSpeed + minVehicleSpeed) / 2.0;
        bool zeroThrottle;
        cruiseSet = IsVehicleSpeedStable(targetSpeed, zeroThrottle);
        operatorInput = testPass;
        status = BEP_STATUS_SUCCESS;
    }
    // Return the status
    return status;
}
