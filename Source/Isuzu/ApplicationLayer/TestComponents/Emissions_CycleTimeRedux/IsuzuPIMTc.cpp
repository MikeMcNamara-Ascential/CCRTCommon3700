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
                else if(!GetTestStepName().compare("BASLearn")) testResult = BrakeApplySensorLearn();
                else if(!GetTestStepName().compare("UnlockPIM")) testResult = UnlockPIM();
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
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    //Short Circuit not needed - if test previously passed will skip in CommandTestStepFunction
    //if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    //{   // Read the current fault count from the module
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
                try
                {   // Tell the module to clear faults
                    moduleStatus = m_vehicleModule.ClearFaults();
                    // Determine the test results
                    if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                    testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                    testDescription = 
                        (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                    // Log the data
                    Log(LOG_DEV_DATA, "Clear Faults: %s - status: %s\n",
                        testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                }
                catch(ModuleException &moduleException)
                {
                    Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                        GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                    testResult = testSoftwareFail;
                    testResultCode = GetFaultCode("SoftwareFailure");
                    testDescription = GetFaultDescription("SoftwareFailure");
                }
                // Send the test result
                SendTestResult(testResult, testDescription, testResultCode);
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
    //}
    //else
    //{   // Need to skip this step
    //    Log(LOG_FN_ENTRY, "Skipping ClearFaults");
    //    testResult = testSkip;
    //}
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuPIMTc::ClearFaults() - Exit");
    return testResult;

}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuPIMTc<ModuleType>::BrakeApplySensorLearn(){
 Log(LOG_FN_ENTRY, "IsuzuPIMTc::ClearFaults() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    //BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    if(!ShortCircuitTestStep())
    {
        DisplayPrompt(GetPromptBox("BASLearnInProgress"), GetPrompt("BASLearnInProgress"), 
                      GetPromptPriority("BASLearnInProgress"), "white");
        DisplayPrompt(GetPromptBox("KeepFootOffBrake"), GetPrompt("KeepFootOffBrake"), 
                      GetPromptPriority("KeepFootOffBrake"), "white");
        //check if the PIM Is locked. 
        bool isLocked = true;
        bool isLearned = false;
        // Attempt to read the locked status from the module
        try
        {   // Read the locked status from the module
            m_vehicleModule.ReadModuleData("IsModuleLocked", isLocked);
        }
        catch (ModuleException &exception)
        {   // Exception reading data
            Log(LOG_ERRORS, "Module exception in BrakeApplySensorLearn() while reading IsModuleLocked - %s\n", exception.message().c_str());
            isLocked = true;
        }
        //Check if the BAS is Learned
        try
        {   // Read the locked status from the module
            m_vehicleModule.ReadModuleData("BASLearned", isLearned);
        }
        catch (ModuleException &exception)
        {   // Exception reading data
            Log(LOG_ERRORS, "Module exception in BrakeApplySensorLearn() while reading BASLearned - %s\n", exception.message().c_str());
            isLearned = true;
        }
        //if both are false Learn the BAS
        if (!isLocked && !isLearned) {
            Log(LOG_DEV_DATA, "PIM unlocked, Initiating BAS Learn.");
            try
            {   // Tell the PIM to Learn the BAS
                testResult =(m_vehicleModule.CommandModule("BASLearn") == BEP_STATUS_SUCCESS) ? testPass : testFail;
                //might need to loop or wait for a time?
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            catch (ModuleException &exception)
            {   // Exception reading data
                Log(LOG_ERRORS, "Module exception in BrakeApplySensorLearn() while Learning BAS - %s\n", exception.message().c_str());
                isLocked = true;
            }
        }
        else if (isLearned) {
            Log(LOG_DEV_DATA, "BAS Already Learned, Passing the test.");
            testResult = testPass;
        }
        //Lock the PIM is the BAS is learned and not locked.

        //if PIM is locked and BAS is learned, PASS
        
        RemovePrompt(GetPromptBox("BASLearnInProgress"), GetPrompt("BASLearnInProgress"), GetPromptPriority("BASLearnInProgress"));
        RemovePrompt(GetPromptBox("KeepFootOffBrake"), GetPrompt("KeepFootOffBrake"), GetPromptPriority("KeepFootOffBrake"));
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }

    Log(LOG_FN_ENTRY, "IsuzuPIMTc::BrakeApplySensorLearn() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuPIMTc<ModuleType>::UnlockPIM(){

    Log(LOG_FN_ENTRY, "IsuzuPIMTc::UnlockPIM() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    //BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    if(!ShortCircuitTestStep())
    {

        UINT32 Key = 0x0000;
        UINT32 seed = 0x0000;
        try
        {  
            //Send The Tester Present to the Module
            m_vehicleModule.CommandModule("ModuleKeepAlive");
            BposSleep(500);
            // Read the security seed from the module
            if (m_vehicleModule.ReadModuleData("ReadSeed", seed) == BEP_STATUS_SUCCESS) {

                Log(LOG_DEV_DATA,"Read Security Seed: 0x%X",seed);
                Key = 0x1611 - seed;
                Log(LOG_DEV_DATA,"Calcuated Key: 0x%X",Key);
                
                //Key is now calculated.  Get ready to send it!
                union _keyData
                {
                    UINT32 fullKey;
                    struct
                    {
                        UINT8 key_0;
                        UINT8 key_1;
                        UINT8 key_2;
                        UINT8 key_3;
                    } keyChars;
                } keyData;
                keyData.keyChars.key_0 = (Key & 0x000000FF);
        		keyData.keyChars.key_1 = ((Key & 0x0000FF00) >> 8);
        		keyData.keyChars.key_2 = ((Key & 0x00FF0000) >> 16);
        		keyData.keyChars.key_3 = ((Key & 0xFF000000) >> 24);
        		keyData.fullKey = Key;
        		Log(LOG_DEV_DATA, "Calcualted key: 0x%08X  ($%02X $%02X $%02X $%02X)", keyData.fullKey, keyData.keyChars.key_3, keyData.keyChars.key_2, keyData.keyChars.key_1, keyData.keyChars.key_0);


                SerialArgs_t keyArgs;
        		//keyArgs.push_back(keyData.keyChars.key_3);
        		//keyArgs.push_back(keyData.keyChars.key_2);
        		keyArgs.push_back(keyData.keyChars.key_1);
        		keyArgs.push_back(keyData.keyChars.key_0);

        		// Write the key to the module
        		 if(m_vehicleModule.CommandModule("UnlockSecurity", &keyArgs) == BEP_STATUS_SUCCESS){
                    Log(LOG_FN_ENTRY, "PIM Unlocked.");
                    testResult = testPass;
                 } 
                 else 
                 {
                    Log(LOG_FN_ENTRY, "Key Not Correct, PIM not Unlocked.");
                    testResult = testFail;
                 }
            }
            else
            {
                Log(LOG_FN_ENTRY, "Error reading the Security Seed from the Module.");
                testResult = testFail;
            }

        }
        catch (ModuleException &exception)
        {   // Exception reading data
            Log(LOG_ERRORS, "Module exception in UnlockPIM() while reading security seed - %s\n", exception.message().c_str());
            seed = 0x0000;
            testResult = testFail;
        }
    }
    Log(LOG_FN_ENTRY, "IsuzuPIMTc::UnlockPIM() - Exit");
    return testResult;
}
