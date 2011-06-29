//******************************************************************************
// Description:
// Test steps specific to Isuzu Emissions testing.
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
#include "IsuzuPIMTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuPIMTc<ModuleType>::IsuzuPIMTc() : GenericEmissionsTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuPIMTc<ModuleType>::~IsuzuPIMTc()
{
}

template<class ModuleType>
const string IsuzuPIMTc<ModuleType>::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_RESPONSE);
    // Determine if the next test step can be sequenced
    if(StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {   
            Log(LOG_DEV_DATA, "IsuzuPIMTc::CommandTestStep(%s) - Current Result: %s,  Always Perform: %s",
                value.c_str(), GetTestStepResult().c_str(), GetTestStepInfoBool("AlwaysPerform") ? "True" : "False");
            // Get the next test step to perform
            if(GetTestStepResult().compare(testPass) || GetTestStepInfoBool("AlwaysPerform"))
            {
                Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
                if(!GetTestStepName().compare("ClearFaultsFinal")) testResult = ClearFaults();
                // No specific method, try the base class
                else testResult = GenericEmissionsTCTemplate<ModuleType>::CommandTestStep(value);
            }
            else
            {
                Log(LOG_DEV_DATA, "%s already passed, not running again!", GetTestStepName().c_str());
                testResult = GetTestStepResult();
            }
            // Check if testing should abort on a failure
            if(GetParameterBool("AbortOnTestStepFailure") && GetTestStepInfoBool("AbortIfFailed") &&
               testResult.compare(testPass) && testResult.compare(testSkip))
            {   // Make sure we abort and then tell the system to abort
                Abort();
                SystemWrite(ABORT_DATA_TAG, string("1"));
            }
        }
        catch(BepException &err)
        {   // Log the exception and return the error
            Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
            testResult = BEP_ERROR_RESPONSE;
        }
    }
    else
    {   // Status of the system is not right to continue testing
        Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
        testResult = testSkip;
    }
    // Return the result of the test
    Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuPIMTc<ModuleType>::ClearFaults(void)
{
    Log(LOG_FN_ENTRY, "IsuzuPIMTc::ClearFaults() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    if(!ShortCircuitTestStep())
    {   // Read the current fault count from the module
        //UINT8 faultCount = 0xFF;
        //BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
        //if(BEP_STATUS_SUCCESS == status)
        //{
            //Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
            // Determine if testing should be aborted
            //if(faultCount > 0 && !isLocked)
            if(!GetTestStepInfoBool("CheckCompletePassStatus") || 
                ((GetOverallResult() == testPass) && SystemRead("CurrentTestStatus") == BEP_PASS_STATUS))
            {
                testResult = GenericTCTemplate<ModuleType>::ClearFaults();
            }
            else
            {
                testResult = testSkip;
            }
        //}
        //else
        //{
        //    Log(LOG_ERRORS, "Could not read fault count from the module: %s", ConvertStatusToResponse(status).c_str());
        //    result = testFail;
        //    testResult = testFail;
        //    testResultCode = GetFaultCode("CommunicationFailure");
        //    testDescription = GetFaultDescription("CommunicationFailure");
        //    SendTestResult(testResult, testDescription, testResultCode);
        //}
        // Report the result
        //char buff[8];
        //SendSubtestResultWithDetail(GetTestStepName() + "ReadFaultCount", result, GetTestStepInfo("Description"), "0000",
        //                         "DtcCount", CreateMessage(buff, sizeof(buff), "%d", faultCount), "");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping ClearFaults");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::ClearFaults() - Exit");
    return testResult;

}
