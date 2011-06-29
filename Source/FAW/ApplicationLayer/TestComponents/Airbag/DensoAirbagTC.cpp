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
#include "DensoAirbagTC.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
template<class ModuleType>
DensoAirbagTC<ModuleType>::DensoAirbagTC() : GenericTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
DensoAirbagTC<ModuleType>::~DensoAirbagTC()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string DensoAirbagTC<ModuleType>::CommandTestStep(const string &value)
{
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "DensoAirbagTC::CommandTestStep(value: %s) - Step: %s - Enter", 
        value.c_str(), GetTestStepName().c_str());
    // Make sure system status will allow testing
    if (BEP_STATUS_SUCCESS != StatusCheck())
    {
        UpdateResult(StatusCheck(), result);
        Log(LOG_ERRORS, "DensoAirbagTC::CommandTestStep: StatusCheck() failed: %s", result.c_str());
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    // Link up with the module
    else if (!GetTestStepName().compare("PerformModuleLinkup"))   result = PerformModuleLinkup();
    // Call the base class to perform the test step
    else     result = GenericTCTemplate<ModuleType>::CommandTestStep(value);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "DensoAirbagTC::CommandTestStep(value: %s) - Step: %s - Exit: %s", 
        value.c_str(), GetTestStepName().c_str(), result.c_str());
    return result;
}

//-----------------------------------------------------------------------------
template<class ModuleType>
string DensoAirbagTC<ModuleType>::PerformModuleLinkup(void)
{
    string testResult(BEP_TESTING_STATUS);
    // Log the entry and determine if the test step should be performed
    Log(LOG_FN_ENTRY, "DensoAirbagTC::PerformModuleLinkup() - Enter");
    if (!ShortCircuitTestStep())
    {   // Command the module to perform a low baud init sequence
        string resultCode("0000");
        string description(GetTestStepInfo("Description"));
        if (CheckCableConnect() || GetParameterBool("DeveloperTest"))
        {
            BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
            try
            {
                UINT8 retry = 3;
                while (retry-- && (status != BEP_STATUS_SUCCESS))
                {
                    status = m_vehicleModule.PerformModuleLinkup();
                    if (status != BEP_STATUS_SUCCESS)
                    {   //prompt driver to restart
                        DisplayTimedPrompt("KeyOff", "1", 0, 10000);
                        DisplayTimedPrompt("StartVehicle", "1", 0, 6000);
                        status = m_vehicleModule.PerformModuleLinkup();
                    }
                }
                Log(LOG_ERRORS, "Module linkup status: %s", ConvertStatusToResponse(status).c_str());
                testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
                resultCode = !testResult.compare(testPass) ? resultCode : GetFaultCode("ModuleLinkupFailure");
                description = !testResult.compare(testPass) ? description : GetFaultDescription("ModuleLinkupFailure");
            }
            catch (ModuleException &excpt)
            {
                Log(LOG_ERRORS, "ModuleException in DensoAirbagTC::PerformModuleLinkup() - %s", excpt.GetReason());
                testResult = testSoftwareFail;
                resultCode = GetFaultCode("SoftwareFailure");
                description = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {   // Timeout waiting for the cable to be connected
            Log(LOG_ERRORS, "Timeout waiting for the operator to connect the cable");
            testResult = testTimeout;
            resultCode = GetFaultCode("CableConnectTimeout");
            description = GetFaultDescription("CableConnectTimeout");
        }
        // Report the test result
        SendTestResult(testResult, description, resultCode);
    }
    else
    {   // Need to skip the test
        Log(LOG_FN_ENTRY, "Skipping PerformModuleLinkup");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "DensoAirbagTC::PerformModuleLinkup() - Exit");
    return testResult;
}

