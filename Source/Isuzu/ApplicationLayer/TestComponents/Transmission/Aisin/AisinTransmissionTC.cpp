//*************************************************************************
// FILE DESCRIPTION:
//  Aisin transmission Test Component.
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
#include "AisinTransmissionTC.h"
//*************************************************************************

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
AisinTransmissionTC<VehicleModuleType>::AisinTransmissionTC() : 
GenericTransmissionTCTemplate<VehicleModuleType>()
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
AisinTransmissionTC<VehicleModuleType>::~AisinTransmissionTC()
{   // clear out the PRNDL position data
    m_prndlPositions.clear(true);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string AisinTransmissionTC<VehicleModuleType>::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_RESPONSE);
    // Determine if the next test step can be sequenced
    if(StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {   
            Log(LOG_DEV_DATA, "AisinTransmissionTC::CommandTestStep(%s) - Current Result: %s,  Always Perform: %s",
                value.c_str(), GetTestStepResult().c_str(), GetTestStepInfoBool("AlwaysPerform") ? "True" : "False");
            // Get the next test step to perform
            if(GetTestStepResult().compare(testPass) || GetTestStepInfoBool("AlwaysPerform"))
            {
                Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
                if(!GetTestStepName().compare("EvaluateTccSlipError"))  testResult = EvaluateTccSlipError();
                else if(!GetTestStepName().compare("ReadFaultCount"))   testResult = CheckFaultCount();
                else if(!GetTestStepName().compare("ShiftLeverTest"))   testResult = ShiftLeverTest();
                else if(!GetTestStepName().compare("ClearFaultsFinal")) testResult = ClearFaults();
                // No specific method, try the base class
                else testResult = GenericTransmissionTCTemplate<VehicleModuleType>::CommandTestStep(value);
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

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void AisinTransmissionTC<VehicleModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to do it's initializations
    GenericTransmissionTCTemplate<VehicleModuleType>::InitializeHook(config);
    // Load the PRNDL positions
    m_prndlPositions.DeepCopy(config->getChild("Setup/Parameters/PrndlPositions")->getChildren());
}

//-------------------------------------------------------------------------------------------------
template<class VehicleModuleType>
string AisinTransmissionTC<VehicleModuleType>::CheckFaultCount(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::CheckFaultCount() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Read the current fault count from the module
        UINT8 faultCount = 0xFF;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
        if(BEP_STATUS_SUCCESS == status)
        {
            result = (faultCount == 0x00) ? testPass : testFail;
            Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
            // Determine if testing should be aborted
            if((faultCount > 0) && GetParameterBool("AbortIfDTCPresent"))
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
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::CheckFaultCount() - Exit");
    return result;
}


//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string AisinTransmissionTC<VehicleModuleType>::EvaluateTccSlipError(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::EvaluateTccSlipError() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Make sure the vehicle is in the correct speed range
        float minTccSlip(GetParameterFloat("MinimumTccSlipError"));
        float maxTccSlip(GetParameterFloat("MaximumTccSlipError"));
        float actualSlip = -999.9;
        if(!WaitAndPromptForSpeed(GetParameterFloat("TccSlipErrorMinSpeed"), GetParameterFloat("TccSlipErrorMaxSpeed"), 
                                  GetTestStepInfoInt("ScanDelay")).compare(testPass))
        {   // Display prompts and targets for the operator
            SystemWrite(GetDataTag("SpeedTarget"), GetParameter("MaxGearSpeedRange"));
            DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"));
            DisplayPrompt(GetPromptBox("SteadyThrottle"), GetPrompt("SteadyThrottle"), GetPromptPriority("SteadyThrottle"));
            // Wait for the slip to be in range
            BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
            bool slipErrorComplete = false;
            do
            {   // Read the slip error from the module
                status = m_vehicleModule.ReadModuleData("ReadTccSlipError", actualSlip);
                // Determine if the slip error analysis is complete
                slipErrorComplete = (minTccSlip <= actualSlip) && (actualSlip <= maxTccSlip);
                if(!slipErrorComplete)  BposSleep(GetTestStepInfoInt("ScanDelay"));
                // Keep checking until time expires or slip error is complete
            } while(TimeRemaining() && !slipErrorComplete &&
                    (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status));
            // Determine the overall result
            result = slipErrorComplete ? testPass : testFail;
            Log(LOG_DEV_DATA, "Slip error complete: %s - actual slip: %.4f [min: %.4f  max: %.4f]",
                result.c_str(), actualSlip, minTccSlip, maxTccSlip);
            // Remove prompts and targets
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"));
            RemovePrompt(GetPromptBox("SteadyThrottle"), GetPrompt("SteadyThrottle"), GetPromptPriority("SteadyThrottle"));
        }
        else
        {   // Could not reach target speed
            Log(LOG_ERRORS, "Could not reach required speed range - not performing TCC slip error analysis");
            result = testFail;
        }
        // Report the results
        char buff[32];
        SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000",
                                 "ActualTccSlipError", CreateMessage(buff, sizeof(buff), "%.4f", actualSlip), "rpm",
                                 "MinimumTccSlipError", GetParameter("MinimumTccSlipError"), "rpm", 
                                 "MaximumTccSlipError", GetParameter("MaximumTccSlipError"), "rpm");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping EvaluateTccSlipError()");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::EvaluateTccSlipError() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string AisinTransmissionTC<VehicleModuleType>::ShiftLeverTest(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::ShiftLeverTest() - Enter");
    string result(testPass);
    if(!ShortCircuitTestStep())
    {   // Check for each PRNDL position
        TestResultDetails details;
        // Have the operator apply the brake pedal during the shifting
        DisplayPrompt(GetPromptBox("FootOnBrake"), GetPrompt("FootOnBrake"), GetPromptPriority("FootOnBrake"));
        for(XmlNodeMapCItr iter = m_prndlPositions.begin(); 
             (iter != m_prndlPositions.end()) && !result.compare(testPass); iter++)
        {   // Reset the start time for this position so we do not timeout before all positions are seen
            SetStartTime();
            string prndlPos(iter->second->getAttribute("Value")->getValue());
            string posResult;
            if(iter->second->getValue().compare(testPass))
            {
                posResult = RequestLeverShift(prndlPos, "ShiftTo"+prndlPos) ? testPass : testFail;
            }
            else
            {
                posResult = iter->second->getValue();
            }
            Log(LOG_DEV_DATA, "PRNDL - %s: %s", prndlPos.c_str(), posResult.c_str());
            // Update the overall result
            UpdateResult(posResult, result);
            // Store the details of this position
            details.AddDetail("PRNDL_"+prndlPos, posResult, "");
            XmlNode *prndlNode = const_cast<XmlNode *>(iter->second);
            prndlNode->setValue(posResult);
        }
        RemovePrompt(GetPromptBox("FootOnBrake"), GetPrompt("FootOnBrake"), GetPromptPriority("FootOnBrake"));
        // Report the result
        Log(LOG_DEV_DATA, "Shift Lever Test: %s", result.c_str());
        SendTestResultWithDetail(result, details, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping ShiftLeverTest");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::ShiftLeverTest() - Exit");
    return result;
}

//-----------------------------------------------------------------------------
template <class VehicleModuleType>
string AisinTransmissionTC<VehicleModuleType>::ClearFaults(void)
{
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::ClearFaults() - Enter");
    string result(BEP_TESTING_RESPONSE);
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   // Read the current fault count from the module
        //UINT8 faultCount = 0xFF;
        //BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
        //if(BEP_STATUS_SUCCESS == status)
        //{
            bool isLocked = true;
            // Attempt to read the locked status from the module
            try
            {   // Read the locked status from the module
                m_vehicleModule.ReadModuleData("IsModuleLocked", isLocked);
            }
            catch(ModuleException &exception)
            {   // Exception reading data
                Log(LOG_ERRORS, "Module exception in ClearFaults() while reading IsModuleLocked - %s\n", exception.message().c_str());
                isLocked = true;
            }
            result = testPass;
            //Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
            Log(LOG_DEV_DATA, "Module Locked: %s", (isLocked ? "True" : "False"));
            // Determine if testing should be aborted
            //if(faultCount > 0 && !isLocked)
            if((!isLocked || GetTestStepInfoBool("IgnoreModuleLockedStatus")) && 
               (!GetTestStepInfoBool("CheckCompletePassStatus") || 
                ((GetOverallResult() == testPass) && SystemRead("CurrentTestStatus") == BEP_PASS_STATUS)))
            {
                testResult = GenericTCTemplate<VehicleModuleType>::ClearFaults();
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
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed clear faults, not testing again");
        testResult = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping clear faults");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AisinTransmissionTC::ClearFaults() - Exit");
    return testResult;

}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline string AisinTransmissionTC<VehicleModuleType>::UpdateEOLStatusByte(const UINT16 &statusRegister, 
                                                                          const UINT8 &testBit, 
                                                                          const string &testResult)
{   // Currently not updating status bytes in the module
    return testPass;
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string AisinTransmissionTC<ModuleType>::ReadFaults(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    FaultStatusVector_t moduleFaults;
    string foundFaults;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter AisinTransmissionTC::ReadFaults()\n");
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
                        catch(...)
                        {
                            ignored = false;
                        }
                        Log(LOG_DEV_DATA, "Ignored from dtc status byte? %s - status mask [%02X] dtc status [%02X]\n", 
                            ignored ? "True" : "False", statusMask, dtcStatus);
                        ignored = (ignored ? ignored : !(m_ignoreFaults.find(faultTag) == m_ignoreFaults.end()));
                        if (((faultCode != 0) && !ignored) || ((faultCode != 0) && GetParameterBool("ReportIgnoredFaults")))
                        {   // This is a fault to report
                            if(!ignored)
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

                            if(checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn &&
                                        checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active, History, and Not Passed";
                            else if(checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "Active and History";
                            else if(checkDtcActive && dtcActiveBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active and Not Passed";
                            else if(checkDtcHistory && dtcHistoryBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "History and Not Passed";
                            else if(checkDtcActive && dtcActiveBitOn)
                                strDtcStatus = "Active";
                            else if(checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "History";
                            else if(checkDtcNotPass && dtcNotPassBitOn)
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
    Log(LOG_FN_ENTRY, "Exit AisinTransmissionTC::ReadFaults()\n");
    return testResult;
}

