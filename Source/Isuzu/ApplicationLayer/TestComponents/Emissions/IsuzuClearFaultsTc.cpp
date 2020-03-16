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
#include "IsuzuClearFaultsTc.h"
#include <fstream.h>
#include <dirent.h>

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuClearFaultsTc<ModuleType>::IsuzuClearFaultsTc() : GenericEmissionsTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuClearFaultsTc<ModuleType>::~IsuzuClearFaultsTc()
{   
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string IsuzuClearFaultsTc<ModuleType>::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_STATUS);
    // Make sure it is OK to perform this test
    if (BEP_STATUS_SUCCESS == StatusCheck())
    {   // Perform the current test step
        if (GetTestStepResult().compare(testPass) || GetTestStepInfoBool("AlwaysPerform"))
        {   // Only perform the step if it did not pass
            if (!GetTestStepName().compare("ReadFaultCount"))                      testResult = CheckFaultCount();
            else if (!GetTestStepName().compare("DelayBeforeDtcRead"))                  testResult = DelayBeforeDtcRead();
            else if (!GetTestStepName().compare("ClearFaultsFinal"))                    testResult = ClearFaults();
            else if (!GetTestStepName().compare("EnterNormalMode"))                     testResult = EnterNormalMode();
            else if (!GetTestStepName().compare("ReadMemoryLocation"))                  testResult = ReadMemoryLocation();
            else if (!GetTestStepName().compare("DisableNormalComms"))                  testResult = DisableNormalComms();
            
            else  testResult = GenericEmissionsTCTemplate<ModuleType>::CommandTestStep(value);
        }
        else
        {
            Log(LOG_DEV_DATA, "%s already passed, not running again!", GetTestStepName().c_str());
            testResult = GetTestStepResult();
        }
        // Check if testing should abort on a failure
        if (GetParameterBool("AbortOnTestStepFailure") && GetTestStepInfoBool("AbortIfFailed") &&
            testResult.compare(testPass) && testResult.compare(testSkip))
        {   // Tell ourselves to abort and then tell the system to abort.
            Abort();
            SystemWrite(ABORT_DATA_TAG, string("1"));
        }
    }
    else
    {   // Not OK to perform the current test
        Log(LOG_ERRORS, "System status is not good, skipping %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Return the test result
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const INT32 IsuzuClearFaultsTc<ModuleType>::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 status = BEP_STATUS_ERROR;
    switch (code)
    {
    case ISUZU_EMISSIONS_PULSE_CODE:
        {
            switch (value)
            {
            default:
                status = BEP_STATUS_SOFTWARE;
                Log(LOG_ERRORS, "Unhandled Isuzu Emissions Pulse Value - %d", value);
                break;
            }
        }
        break;

    default:
        status = BepServer::HandlePulse(code, value);
        break;
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void IsuzuClearFaultsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericEmissionsTCTemplate<ModuleType>::InitializeHook(config);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::CheckFaultCount(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuClearFaultsTc::CheckFaultCount() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the current fault count from the module
        UINT8 faultCount = 0xFF;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
        if (BEP_STATUS_SUCCESS == status)
        {
            result = (faultCount == 0x00) ? testPass : testFail;
            Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
            // Determine if testing should be aborted
            if ((faultCount > 0) && GetParameterBool("AbortIfDTCPresent"))
            {
                Log(LOG_DEV_DATA, "DTCs present - aborting test!");
                SystemWrite(ABORT_DATA_TAG, true);
            }
        }
        else
        {
            Log(LOG_ERRORS, "Could not read fault count from the module: %s", ConvertStatusToResponse(status).c_str());
            result = testFail;
        }
        // Report the result
        char buff[8];
        SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000",
                                 "DtcCount", CreateMessage(buff, sizeof(buff), "%d", faultCount), "");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping CheckFaultCount");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuClearFaultsTc::CheckFaultCount() - Exit");
    return result;
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::ReadFaults(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    FaultStatusVector_t moduleFaults;
    string foundFaults;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter IsuzuClearFaultsTc::ReadFaults()\n");
    if (!ShortCircuitTestStep())
    {   // Do not need to skip this step
        try
        {   // Try to read the module faults
            moduleStatus = m_vehicleModule.ReadFaults(moduleFaults);
            // Check the status of the operation
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, evaluate the data
                bool faultsRecorded = false;
                Log(LOG_DEV_DATA, "Found %d faults recorded in the module\n", moduleFaults.size());
                // Only check for faults if faults were read from the module
                if (moduleFaults.size() > 0)
                {
                    for (UINT32 faultIndex = 0; faultIndex < moduleFaults.size(); faultIndex++)
                    {   // Determine if this fault should be ignored
                        string faultTag = "ModuleFault_" + moduleFaults[faultIndex].m_code;
                        INT32 faultCode = atoh(moduleFaults[faultIndex].m_code.c_str());
                        string faultStatus("Ignored");
                        UINT8 statusMask = 0x00;
                        UINT8 dtcStatus = atoh(moduleFaults[faultIndex].m_status.c_str());;
                        bool ignored;
                        try
                        {
                            statusMask = atoh(GetFaultFailureStatusMask(faultTag).c_str()); 
                            ignored = !(statusMask & dtcStatus);
                        }
                        catch (...)
                        {
                            ignored = false;
                        }
                        Log(LOG_DEV_DATA, "Ignored from dtc status byte? %s - status mask [%02X] dtc status [%02X]\n", 
                            ignored ? "True" : "False", statusMask, dtcStatus);
                        ignored = (ignored ? ignored : !(m_ignoreFaults.find(faultTag) == m_ignoreFaults.end()));
                        if (((faultCode != 0) && !ignored) || ((faultCode != 0) && GetParameterBool("ReportIgnoredFaults")))
                        {   // This is a fault to report
                            if (!ignored)
                            {
                                faultStatus = "Reported";
                                faultsRecorded = true;
                            }

                            //Set strDtcStatus to indicate the DTC status
                            string strDtcStatus = "";
                            UINT8 dtcActiveBit = (UINT8)GetParameterInt("DtcStatusActiveBit");
                            UINT8 dtcHistoryBit = (UINT8)GetParameterInt("DtcStatusHistoryBit");
                            UINT8 dtcNotPassBit = (UINT8)GetParameterInt("DtcStatusNotPassBit");
                            bool checkDtcActive = (statusMask & (1<<dtcActiveBit)) ? true : false;
                            bool checkDtcHistory = (statusMask & (1<<dtcHistoryBit)) ? true : false;
                            bool checkDtcNotPass = (statusMask & (1<<dtcNotPassBit)) ? true : false;
                            bool dtcActiveBitOn = (dtcStatus & (1<<dtcActiveBit)) ? true : false;
                            bool dtcHistoryBitOn = (dtcStatus & (1<<dtcHistoryBit)) ? true : false;
                            bool dtcNotPassBitOn = (dtcStatus & (1<<dtcNotPassBit)) ? true : false;

                            if (checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn &&
                                checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active, History, and Not Passed";
                            else if (checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "Active and History";
                            else if (checkDtcActive && dtcActiveBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active and Not Passed";
                            else if (checkDtcHistory && dtcHistoryBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "History and Not Passed";
                            else if (checkDtcActive && dtcActiveBitOn)
                                strDtcStatus = "Active";
                            else if (checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "History";
                            else if (checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Not Passed";
                            else
                                strDtcStatus = "SOFTWARE FAILURE!";


                            ReportDTC(faultTag, moduleFaults[faultIndex].m_code, "Code="+GetFaultCode(faultTag)+" - "+strDtcStatus+" - "+GetFaultDescription(faultTag));
                        }
                        // Log the fault read from the module
                        Log(LOG_DEV_DATA, "Module Fault %d - %s - %s\n", faultIndex+1,
                            moduleFaults[faultIndex].m_code.c_str(), faultStatus.c_str());
                    }
                    // Do special handling if faults have been recorded
                    if (faultsRecorded)
                    {   // Check if test sequence should be aborted if faults are present
                        if (GetParameterBool("AbortIfDTCPresent"))
                        {
                            testResult = testFail;
                            SystemWrite(ABORT_DATA_TAG, "1");
                            Log(LOG_ERRORS, "Faults found in the module - ABORT all testing!\n");
                        }
                        else
                        {
                            testResult = testFail;
                            Log(LOG_ERRORS, "Faults found in the module - continue to test!\n");
                        }
                    }
                    // Store the data indicating if faults are present in the module
                    DTCsInModule(faultsRecorded);
                }
                // Set the test status
                testResult = faultsRecorded ? testResult : testPass;
                testResultCode = faultsRecorded ? GetFaultCode("ModuleFaults") : "0000";
                testDescription = faultsRecorded ? GetFaultDescription("ModuleFaults") : GetTestStepInfo("Description");
                Log(LOG_DEV_DATA, "Read Module Faults: %s\n", testResult.c_str());
            }
            else
            {   // Error reading faults from the module
                Log(LOG_ERRORS, "Error reading module faults - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
            }
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the results
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit IsuzuClearFaultsTc::ReadFaults()\n");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::DelayBeforeDtcRead(void)
{
    string result(BEP_TESTING_RESPONSE);

    if (GetParameterInt("DelayBeforeDtcRead") != 0)
    {
        DisplayPrompt(GetPromptBox("DelayBeforeDtcRead"), GetPrompt("DelayBeforeDtcRead"), GetPromptPriority("DelayBeforeDtcRead"));
        BposSleep(GetParameterInt("DelayBeforeDtcRead"));
        RemovePrompt(GetPromptBox("DelayBeforeDtcRead"), GetPrompt("DelayBeforeDtcRead"), GetPromptPriority("DelayBeforeDtcRead"));
        result = testPass;
    }
    else
        result = testSkip;

    return result;
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::ClearFaults(void)
{
    Log(LOG_FN_ENTRY, "IsuzuClearFaultsTc::ClearFaults() - Enter");
    string result(BEP_TESTING_RESPONSE);
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    bool isLocked = false;

    try
    {   // Tell the module to clear faults
        moduleStatus = m_vehicleModule.ClearFaults();
        // Determine the test results
        if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
        testDescription = 
        (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
        // Log the data
        Log(LOG_DEV_DATA, "Clear Faults: %s - status: %s\n",
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
    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);

    Log(LOG_FN_ENTRY, "IsuzuClearFaultsTc::ClearFaults() - Exit");
    return testResult;

}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::EnterNormalMode(void)
{	// Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if this step should be skipped
	Log(LOG_FN_ENTRY, "Enter IsuzuClearFaultsTc::EnterNormalMode()\n");
	if(!ShortCircuitTestStep())
	{	// Need to perform this test step
		if(CheckCableConnect())
		{
			try
			{	// Try to enter diagnostic mode
				moduleStatus = m_vehicleModule.EnterNormalMode();
				// Determine the test result
				if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
				testDescription = 
				(testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
				// Log the data
				Log(LOG_DEV_DATA, "Enter Normal Mode: %s - status: %s\n",
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
		}
		else
		{
			Log(LOG_ERRORS, "IsuzuClearFaultsTc::EnterNormalMode() - Timeout waiting for Cable Connect");
			testResult = testTimeout;
			testResultCode = GetFaultCode("TimeoutFailure");
			testDescription = GetFaultDescription("TimeoutFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit IsuzuClearFaultsTc::EnterNormalMode()\n");
	return(testResult);
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::DisableNormalComms(void)
{	// Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if this step should be skipped
	Log(LOG_FN_ENTRY, "Enter IsuzuClearFaultsTc::DisableNormalComms()\n");
	if(!ShortCircuitTestStep())
	{	// Need to perform this test step
		if(CheckCableConnect())
		{
			try
			{	// Try to enter diagnostic mode
				moduleStatus = m_vehicleModule.DisableNormalComms();
				// Determine the test result
				if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
				testDescription = 
				(testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
				// Log the data
				Log(LOG_DEV_DATA, "Disable Normal Comms: %s - status: %s\n",
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
		}
		else
		{
			Log(LOG_ERRORS, "IsuzuClearFaultsTc::DisableNormalComms() - Timeout waiting for Cable Connect");
			testResult = testTimeout;
			testResultCode = GetFaultCode("TimeoutFailure");
			testDescription = GetFaultDescription("TimeoutFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit IsuzuClearFaultsTc::DisableNormalComms()\n");
	return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::ReadMemoryLocation(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	vector<UINT8> memoryContents;

    // Check if this step needs to be performed
	Log(LOG_FN_ENTRY, "Enter IsuzuClearFaultsTc::ReadMemoryLocation()\n");
	if(!ShortCircuitTestStep())
	{	// Do not need to skip this step
		try
		{	// Try to read the memory
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadMemoryLocation"), memoryContents);
			// Check the status of the operation
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Good read, evaluate the data
                for(UINT32 memoryIndex = 0; memoryIndex < memoryContents.size(); memoryIndex++)
                {
                    // Log the memory value read from the module
				    Log(LOG_DEV_DATA, "Memory Location %d - $%02X\n", memoryIndex+1,
						memoryContents[memoryIndex]);
                }

               // Set the test status
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
				testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
				Log(LOG_DEV_DATA, "Read Memory Location: %s\n", testResult.c_str());
			}
			else
			{	// Error reading faults from the module
				Log(LOG_ERRORS, "Error reading module memory - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
				SetCommunicationFailure(true);
			}
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the results
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit IsuzuClearFaultsTc::ReadMemoryLocation()\n");
	return testResult;
}
//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuClearFaultsTc<ModuleType>::ConditionalFaultClear(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    FaultStatusVector_t moduleFaults;
    bool executeFaultClear = false;
    // Determine if this test step needs to be skipped
    Log(LOG_FN_ENTRY, "Enter IsuzuClearFaultsTc::ConditionalFaultClear()\n");
    if(!ShortCircuitTestStep()  || GetTestStepInfoBool("AlwaysPerform"))
    {   // Do not need to skip this step
        try
        {// Try to read the module faults
            moduleStatus = m_vehicleModule.ReadFaults(moduleFaults); 
            //moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadFaults"), moduleFaults);
            // Check the status of the operation
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, evaluate the data
                Log(LOG_DEV_DATA, "Found %d faults recorded in the module\n", moduleFaults.size());
                // Only check for faults if faults were read from the module
                if(moduleFaults.size() > 0)
                {
                    for(UINT32 faultIndex = 0; faultIndex < moduleFaults.size(); faultIndex++)
                    {   // Determine if this fault should be ignored
                        string faultTag = "ModuleFault_" + moduleFaults[faultIndex].m_code;
                        INT32 faultCode = atoh(moduleFaults[faultIndex].m_code.c_str());
                        bool ignored = false;/*
                        UINT8 statusMask = 0x00;
                        UINT8 dtcStatus = atoh(moduleFaults[faultIndex].m_status.c_str());;
                        try
                        {
                            statusMask = atoh(GetFaultFailureStatusMask(faultTag).c_str()); 
                            ignored = !(statusMask & dtcStatus);
                        }
                        catch (...)
                        {
                            ignored = false;
                        }
                        Log(LOG_DEV_DATA, "Ignored from dtc status byte? %s - status mask [%02X] dtc status [%02X]\n", 
                            ignored ? "True" : "False", statusMask, dtcStatus);
                            */
                        executeFaultClear = (m_clearFaults.find(faultTag) == m_clearFaults.end()) ? executeFaultClear : true;
                        // Log the fault read from the module
                        Log(LOG_DEV_DATA, "Module Fault %d - %s - %s\n", faultIndex+1,
                            moduleFaults[faultIndex].m_code.c_str(), ignored ? "Valid" : "Ignored");
                        Log(LOG_DEV_DATA,"Execute Fault Clear? %s",(executeFaultClear?"True":"False"));
                        //executeFaultClear = !ignored ? executeFaultClear : true;
                    }
                }
                if(executeFaultClear)
                {
                    // Tell the module to clear faults
                    moduleStatus = m_vehicleModule.ClearFaults();
                    // Log the data
                    Log(LOG_DEV_DATA, "Clear Faults: %s - status: %s\n",
                        testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                }
                else
                    Log(LOG_DEV_DATA, "Unregistered faults present. Not commandning fault clear.");
            }

            if(BEP_STATUS_SUCCESS != moduleStatus)
            {   // Error reading faults from the module
                Log(LOG_ERRORS, "Error reading module faults - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
            }
            else
            {
                testResult = testPass;
            }   
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
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit IsuzuClearFaultsTc::ConditionalFaultClear()\n");
    return(testResult);
}

