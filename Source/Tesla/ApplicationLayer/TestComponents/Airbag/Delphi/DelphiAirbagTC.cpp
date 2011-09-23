//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "DelphiAirbagTC.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
template<class ModuleType>
DelphiAirbagTC<ModuleType>::DelphiAirbagTC() : GenericTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
DelphiAirbagTC<ModuleType>::~DelphiAirbagTC()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string DelphiAirbagTC<ModuleType>::CommandTestStep(const string &value)
{
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "DelphiAirbagTC::CommandTestStep(value: %s) - Step: %s - Enter", 
        value.c_str(), GetTestStepName().c_str());
    // Make sure system status will allow testing
    if (BEP_STATUS_SUCCESS != StatusCheck())
    {
        UpdateResult(StatusCheck(), result);
        Log(LOG_ERRORS, "DelphiAirbagTC::CommandTestStep: StatusCheck() failed: %s", result.c_str());
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    // Link up with the module
    else if (!GetTestStepName().compare("CommandSecurityAccess"))   result = CommandSecurityAccess();
    else if (!GetTestStepName().compare("EnableAirBagModule"))   result = EnableAirBagModule();
    // Check the airbag lamp status
    // Call the base class to perform the test step
    else     result = GenericTCTemplate<ModuleType>::CommandTestStep(value);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "DelphiAirbagTC::CommandTestStep(value: %s) - Step: %s - Exit: %s", 
        value.c_str(), GetTestStepName().c_str(), result.c_str());
    return result;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string DelphiAirbagTC<ModuleType>::CommandSecurityAccess(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter DelphiAirbagTC::CommandSecurityAccess\n");
    if (!ShortCircuitTestStep() )
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.GetSecurityAccess();
                // Determine the test result
                if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "CommandSecurityAccess: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "DelphiAirbagTC::CommandSecurityAccess() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit DelphiAirbagTC::CommandSecurityAccess()\n");
    return(testResult);
}

//------------------------------------------------------------------------------
template<class ModuleType>
string DelphiAirbagTC<ModuleType>::EnableAirBagModule(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "DelphiAirbagTC::EnableAirBagModule()\n");
    // a flag that is true if the compared bytes are correct
    bool AirBagEnabled = false;  
    bool Faults = false;
    if (!ShortCircuitTestStep())
    {   // Need to perform this test step
        if (CheckCableConnect())
        {
            try
            {   // Try to read whether the module is enabled or disabled 
                moduleStatus = m_vehicleModule.ReadModuleData("ReadByID", AirBagEnabled);
                // Determine the test result
                if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                Log(LOG_DEV_DATA, "EnableAirBagModule: ReadByID: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());

                //Determines if the module is already turned on 
                if (AirBagEnabled == true && moduleStatus==BEP_STATUS_SUCCESS)
                {
                    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                }
                // if not turns it on
                else
                {
                    //read faults
                    moduleStatus = m_vehicleModule.ReadModuleData("ReadFaults", Faults);
                    if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                    testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                    testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));

                    Log(LOG_DEV_DATA, "EnableAirBagModule: ReadFaults: %s - status: %s\n",
                        testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                    //if faults clear them 
                    if (Faults)
                    {
                        moduleStatus = m_vehicleModule.CommandModule("ClearFaults");
                        if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                        testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));

                        Log(LOG_DEV_DATA, "EnableAirBagModule: ClearFaults: %s - status: %s\n",
                            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                    }
                    moduleStatus = m_vehicleModule.CommandModule("WriteByID");
                    if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                    testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                    testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));

                    Log(LOG_DEV_DATA, "EnableAirBagModule: WriteByID: %s - status: %s\n",
                        testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                    if (moduleStatus == BEP_STATUS_SUCCESS)
                    {
                        // checks to make sure that it is on and if it is not fails the test
                        moduleStatus = m_vehicleModule.ReadModuleData("ReadByID", AirBagEnabled);
                        if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                        testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));

                        Log(LOG_DEV_DATA, "EnableAirBagModule: ReadByID: %s - status: %s\n",
                            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                    }
                    if (AirBagEnabled == true && moduleStatus==BEP_STATUS_SUCCESS)
                    {
                        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                    }
                }

                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription =  (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "EnableAirBagModule: %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch (ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "DelphiAirbagTC::EnableAirBagModule() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit DelphiAirbagTC::EnableAirBagModule\n");
    return(testResult);
}

