//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Electronic Park Brake.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
// 
//*************************************************************************
#include "ElectronicParkBrakeTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
ElectronicParkBrakeTc<ModuleType>::ElectronicParkBrakeTc() : GenericTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
ElectronicParkBrakeTc<ModuleType>::~ElectronicParkBrakeTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string ElectronicParkBrakeTc<ModuleType>::ParkBrakeTest(void)
{   // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "ElectronicParkBrakeTc::CheckBrakeState() - Enter");
    string testResult = testFail;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   
        // Wait for Zerospeed?
        DisplayPrompt(GetPromptBox("MaintainBrake"), GetPrompt("MaintainBrake"), GetPromptPriority("MaintainBrake"));
        // Brake must be off before performing assembly check
        testResult = ReadBrakeState(GetDataTag("OffState"), GetParameterInt("OffStateReadTime")); 
        if (testResult == testPass)
        {   //Assembly check
            moduleStatus = m_vehicleModule.PerformAssemblyCheck();
            if (moduleStatus == BEP_STATUS_SUCCESS)
            {   
                // EPB parameter arbitration
                moduleStatus = m_vehicleModule.ParameterArbitration();
                if (moduleStatus == BEP_STATUS_SUCCESS)
                {
                    // check the Checksum
                    testResult = testPass;
                }
            }
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");         
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        // If Assembly check passed check on-off functionality
        if (testResult == testPass)
        {
            testResult = ReadBrakeState(GetDataTag("OnState"), GetParameterInt("OnStateReadTime")); 
            if (testResult == testPass)
            {
                testResult = ReadBrakeState(GetDataTag("OffState"), GetParameterInt("OffStateReadTime")); 
            }
        }

        RemovePrompt(GetPromptBox("MaintainBrake"), GetPrompt("MaintainBrake"), GetPromptPriority("MaintainBrake"));

        SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "ParkBrakeTest", testResult, "");
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping CheckBrakeState");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "ElectronicParkBrakeTc::CheckBrakeState() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string ElectronicParkBrakeTc<ModuleType>::ReadBrakeState(const string &requestedBrakeState, const int &brakeReadTime)
{
    string testResult = testTimeout;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string leftBrakeState = "Unknown";
    string rightBrakeState = "Unknown";
    bool displayPrompt = true;

    // Read the park brake state
    SetStartTime();
    do
    {	// Read the brake state from the module
        // BrakeState must be read from both Left and Right
        status = m_vehicleModule.GetInfo(GetDataTag("ReadLeftBrakeState"), leftBrakeState);
        if(BEP_STATUS_SUCCESS == status)
        {
            status = m_vehicleModule.GetInfo(GetDataTag("ReadRightBrakeState"), rightBrakeState);
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");         
            testDescription = GetFaultDescription("CommunicationFailure");
        }
        if(BEP_STATUS_SUCCESS == status)
        {	// Good read, determine brake position for both left and right brakes
            if((leftBrakeState == requestedBrakeState) && (rightBrakeState == requestedBrakeState))
            {   
                testResult = testPass;
                displayPrompt = false;
            }
            else 
            {
                Log(LOG_DEV_DATA, "LeftBrakeState is %s. RightBrakeState is %s. Waiting for both to be %s.\n",leftBrakeState.c_str(),rightBrakeState.c_str(),requestedBrakeState.c_str());
            }
            if(displayPrompt)
            {	// Throw the prompt up for the operator
                DisplayPrompt(GetPromptBox(requestedBrakeState), GetPrompt(requestedBrakeState), GetPromptPriority(requestedBrakeState));
                displayPrompt = false;	 // Prevent prompt from being displayed multiple times (avoid flicker)
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");         
            testDescription = GetFaultDescription("CommunicationFailure");
        }
    } while(TimeRemaining(&brakeReadTime) && (BEP_STATUS_SUCCESS == StatusCheck()) && (testResult != testPass));
    if(!TimeRemaining())
    {	// Timeout waiting for correct brake state
        testResult = testTimeout;
        testResultCode = GetFaultCode("TestTimeout");
        testDescription = GetFaultDescription("TestTimeout");
    }
    else if(BEP_STATUS_SUCCESS != StatusCheck())
    {	// Bad system status
        testResult = testAbort;
        testResultCode = GetFaultCode("SystemStatus");
        testDescription = GetFaultDescription("SystemStatus");
        Log(LOG_ERRORS, "Bad system status, aborting -- status: %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
    }

    RemovePrompt(GetPromptBox(requestedBrakeState), GetPrompt(requestedBrakeState), GetPromptPriority(requestedBrakeState));
    // Send the test results
    SendSubtestResultWithDetail("ReadBrakeState" + requestedBrakeState, testResult, testDescription, testResultCode, "Left Park Brake: ", leftBrakeState, "", "Right Park Brake: ", rightBrakeState, "");

    return testResult;
}
