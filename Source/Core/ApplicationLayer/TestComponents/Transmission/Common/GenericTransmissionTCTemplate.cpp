//******************************************************************************
// Description:
//  Generic Transmission Test Component.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Transmission/Common/GenericTransmissionTCTemplate.cpp $
// 
// 2     3/18/08 8:31a Derickso
// Added parameter to allow the module a litle extra time to complete the
// quicklearn process after it indicates the process has been completed.
// 
// 1     11/02/07 10:23a Derickso
// Added WaitForLockupWithThrottleGraph.
// 
// 3     10/03/07 11:21a Jsemann
// Resolved issue when displaying throttle graph in tcc lockup test - need
// to update alllockupstates seen.
// 
// 2     10/01/07 2:00p Derickso
// Added retry for TCC full lockup to display a green band for the TPS as
// a driver aid.
// 
// 12    2/22/07 10:53a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added KeyOffEngineOffKeyOn().
// - Added AccelerateInReverse().
// 
// 11    1/17/07 3:47p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// - Added a call to StatusCheck() in RequestStateByPrompt().
// - Removed KeyOffEngineOffKeyOn().
// - Added a call to StatusCheck() in WaitForEngineOffIgnitionOff().
// 
// 10    9/05/06 3:57p Rwiersem
// Changes for the 06090601 core release:
// 
// - Removed QuickLearnAuthentication()
// - Removed unused variable resetRpmTimer in MonitorQuickLearnProcedure()
// - Added KeyOffEngineOffKeyOn()
// - Added WaitForEngineOffIgnitionOff()
// 
// 9     7/05/06 10:02p Cward
// Fix transmission tests to handle operator quickly shifting out of
// Reverse, and also preventing prompt to Reverse or Park while vehicle is
// in motion.
//
// 8     5/08/06 7:15p Cward
// correct bugs in Evaluate Quick Learn, and Monitor Quick Learn
// Procedure, and add authentication start stop method
//
// 7     4/13/06 3:28a Cward
//  Changed hard coded unit strings with new units strings from
// GenericTC.h
//
// 2     3/20/06 10:48a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 6     2/15/06 7:37p Cward
// Added MonitorKickDown and StopQuickLearnProcesure.
//
// 5     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 4     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 3     4/29/05 8:00a Derickson
// Fixed incorrect comparison in MonitorQuickLearnProcedure()
//
// 3     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 3     4/04/05 1:02a Derickso
// Not all transmissions require that all
// torque converter lock states be observed.
// Update the software to make the unlock
// and partial lock states only be required based
// on parameter settings.
//
// 2     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericTransmissionTCTemplate.h"
#include "ModuleException.h"


template <class VehicleModuleType>
GenericTransmissionTCTemplate<VehicleModuleType>::GenericTransmissionTCTemplate() : GenericTCTemplate<VehicleModuleType>(),
m_firstGearUpdated(testSkip), m_secondGearUpdated(testSkip), m_thirdGearUpdated(testSkip), m_fourthGearUpdated(testSkip),
m_fifthGearUpdated(testSkip), m_sixthGearUpdated(testSkip), m_parkGearUpdated(testSkip), m_reverseGearUpdated(testSkip),
m_neutralGearUpdated(testSkip), m_autostickEquipped(false),m_quickLearnRunning(false)
{   // Nothing special to do
}

template <class VehicleModuleType>
GenericTransmissionTCTemplate<VehicleModuleType>::~GenericTransmissionTCTemplate()
{   // Stop any background monitors that may have been started
    StopBackgroundMonitors();
}

template <class VehicleModuleType>
void GenericTransmissionTCTemplate<VehicleModuleType>::WarmInitialize()
{
    m_firstGearUpdated = testSkip;
    m_secondGearUpdated = testSkip;
    m_thirdGearUpdated = testSkip;
    m_fourthGearUpdated = testSkip;
    m_fifthGearUpdated = testSkip;
    m_sixthGearUpdated = testSkip;
    m_parkGearUpdated = testSkip;
    m_reverseGearUpdated = testSkip;
    m_neutralGearUpdated = testSkip;

    m_autostickEquipped = false;
    m_quickLearnRunning = false;

    GenericTCTemplate<VehicleModuleType>::WarmInitialize();
}

template <class VehicleModuleType>
const string GenericTransmissionTCTemplate<VehicleModuleType>::CommandTestStep(const string &value)
{
    string testResult = testFail;
    // Determine if the next test step can be sequenced
    if(StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {   // Get the next test step to perform
            Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
            // Check the Limp-In status
            if(GetTestStepName() == "CheckLimpInStatus")             testResult = CheckLimpInStatus();
            // Read the Transmission's EOL Status Bytes
            else if(GetTestStepName() == "ReadEOLStatusBytes")       testResult = ReadEOLStatusBytes();
            // Check the module upline register
            else if(GetTestStepName() == "CheckUplineStatus")        testResult = CheckUplineStatus();
            // Clear the diagnostic tell tales
            else if(GetTestStepName() == "ClearDiagnosticTellTale")  testResult = ClearDiagnosticTellTale();
            // Clear the overall status in the module
            else if(GetTestStepName() == "ClearOverallStatus")       testResult = ClearOverallStatus();
            // Run the quicklearn procedure on the transmission
            else if(GetTestStepName() == "EvaluateQuickLearn")       testResult = EvaluateQuickLearn();
            // Perform a speed rationality test
            else if(GetTestStepName() == "EvaluateSpeedRationality") testResult = EvaluateSpeedRationality();
            // Perform the overdrive lockout test
            else if(GetTestStepName() == "MonitorOverdriveLockout")  testResult = MonitorOverdriveLockout();
            // Check for transmission kickdown
            else if(GetTestStepName() == "MonitorKickDown")          testResult = MonitorKickDown();
            // Wait for all gears to be observed
            else if(GetTestStepName() == "WaitForGears")             testResult = WaitForGears();
            // Wait for Torque Converter Clutch to fully lockup
            else if(GetTestStepName() == "WaitForGearsAndLockup")    testResult = WaitForGearsAndLockup();
            // Wait for the transmission to achieve third gear
            else if(GetTestStepName() == "WaitForThirdGear")         testResult = WaitForThirdGear(GetTestStepInfo("SpeedRange"));
            // Reset the status registers in the module
            else if(GetTestStepName() == "ResetTestStatus")          testResult = ResetTestStatus();
            // Start the background monitor for faults and limp-in status
            else if(GetTestStepName() == "StartFaultLimpInMonitor")  testResult = StartFaultLimpInMonitor();
            // Start the background monitor to look for all gears and torque converter clutch lockup states
            else if(GetTestStepName() == "StartGearLockUpMonitor")   testResult = StartGearLockUpMonitor();
            // Start the background monitor to look for all gears
            else if(GetTestStepName() == "StartGearMonitor")         testResult = StartGearMonitor();
            // Start the background monitor to look for the torque converter clutch lockup states
            else if(GetTestStepName() == "StartLockUpMonitor")       testResult = StartLockUpMonitor();
            // Stop the background fault and limp-in monitor
            else if(GetTestStepName() == "StopFaultLimpInMonitor")   testResult = StopFaultLimpInMonitor();
            // Stop the background gear monitor and torque converter clutch lockup monitor
            else if(GetTestStepName() == "StopGearLockUpMonitor")    testResult = StopGearLockUpMonitor();
            // Stop the background gear monitor
            else if(GetTestStepName() == "StopGearMonitor")          testResult = StopGearMonitor();
            // Stop the background torque converter clutch lockup monitor
            else if(GetTestStepName() == "StopLockUpMonitor")        testResult = StopLockUpMonitor();
            // Verify that broadcast and module info for autostick match
            else if(GetTestStepName() == "CheckAutostickBits")       testResult = CheckAutostickBits();
            // Check the shift lever
            else if(GetTestStepName() == "CheckShiftLever")          testResult = CheckShiftLever();
            // Accelerate In Reverse to Required Test Speed
            else if(GetTestStepName() == "AccelerateInReverse")
                testResult = AccelerateInReverse(GetParameterFloat("ReverseSpeedMinimum"),
                                                 GetParameterFloat("ReverseSpeedMaximum"));
            // Start the background monitor to look for all shift lever positions
            else if(GetTestStepName() == "StartShiftLeverMonitor")       testResult = StartShiftLeverMonitor();
            // Stop the background monitor to look for all shift lever positions
            else if(GetTestStepName() == "StopShiftLeverMonitor")       testResult = StopShiftLeverMonitor();
            //Send stop quick learn to module
            else if(GetTestStepName() == "StopQuickLearn")           testResult = StopQuickLearnProcedure();
            // No specific method, try the base class
            else testResult = GenericTCTemplate<VehicleModuleType>::CommandTestStep(value);
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

template <class VehicleModuleType>
inline string GenericTransmissionTCTemplate<VehicleModuleType>::StopBackgroundMonitors(void)
{   // Stop the gear monitor, tcc monitor and fault/limp-in monitor
    BEP_STATUS_TYPE gearStatus = m_vehicleModule.StopGearMonitor();
    BEP_STATUS_TYPE tccStatus = m_vehicleModule.StopLockupMonitor();
    BEP_STATUS_TYPE faultStatus = m_vehicleModule.StopLimpInFaultMonitor();
    BEP_STATUS_TYPE leverStatus = m_vehicleModule.StopShiftLeverMonitor();
    return(((BEP_STATUS_SUCCESS == gearStatus) &&
            (BEP_STATUS_SUCCESS == tccStatus) &&
            (BEP_STATUS_SUCCESS == leverStatus) &&
            (BEP_STATUS_SUCCESS == faultStatus)) ? testPass : testFail);
}

template <class VehicleModuleType>
inline string GenericTransmissionTCTemplate<VehicleModuleType>::ResumeBackgroundMonitors(void)
{   // Start any background monitors that were stopped
    BEP_STATUS_TYPE gearStatus  = BEP_STATUS_ERROR;
    BEP_STATUS_TYPE tccStatus   = BEP_STATUS_ERROR;
    BEP_STATUS_TYPE faultStatus = BEP_STATUS_ERROR;
    BEP_STATUS_TYPE leverStatus = BEP_STATUS_ERROR;
    if(!ShortCircuitTestStep())
    {
        gearStatus  = m_vehicleModule.StartGearMonitor();
        tccStatus   = m_vehicleModule.StartLockupMonitor();
        faultStatus = m_vehicleModule.StartLimpInFaultMonitor(&m_ignoreFaults, &m_faults);
        leverStatus = m_vehicleModule.StartShiftLeverMonitor();
    }
    else
    {
        gearStatus  = BEP_STATUS_SUCCESS;
        tccStatus   = BEP_STATUS_SUCCESS;
        faultStatus = BEP_STATUS_SUCCESS;
        leverStatus = BEP_STATUS_SUCCESS;
    }
    return(((BEP_STATUS_SUCCESS == gearStatus) &&
            (BEP_STATUS_SUCCESS == tccStatus) &&
            (BEP_STATUS_SUCCESS == leverStatus) &&
            (BEP_STATUS_SUCCESS == faultStatus)) ? testPass : testFail);
}

template <class VehicleModuleType>
void GenericTransmissionTCTemplate<VehicleModuleType>::InitializeHook(const XmlNode *config)
{   // Log Method Entry
    Log(LOG_FN_ENTRY, "GenericTransmissionTCTemplate::InitializeHook - Enter\n");
    // Initialize the base class first
    GenericTCTemplate<VehicleModuleType>::InitializeHook(config);
    // Load the EOL status bytes that need to be reset at start of testing
    try
    {
        m_resetEOLStatusBytes.DeepCopy(config->getChild("Setup/Parameters/ResetEOLStatusBytes")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading EOL status bytes to reset
        Log(LOG_ERRORS, "Error loading ResetEOLStatusBytes - %s\n", err.what());
    }
    // Load the bytes that are used to directly write the transmission EOL status registers
    try
    {
        m_writeEOLStatusBytes.DeepCopy(config->getChild("Setup/Parameters/WriteEOLStatusByteValues")->getChildren());
    }
    catch(XmlException &err)
    {   // Error loading the EOL status bytes to reset
        Log(LOG_ERRORS, "Error loading WriteEolStatusBytes - %s\n", err.what());
    }
    // Load the upline tests that need to be complete prior to testing
    try
    {
        m_uplineTestsToCheck.DeepCopy(config->getChild("Setup/Parameters/UplineTestsToCheck")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading upline tests to check
        Log(LOG_ERRORS, "Error loading UplineTestsToCheck - %s\n", err.what());
    }
    // Load the clutch volume tags
    try
    {
        m_clutchVolumeIndex.DeepCopy(config->getChild("Setup/Parameters/ClutchVolumeTags")->getChildren());
    }
    catch(BepException &err)
    {   // Error loading clutch volume tags
        Log(LOG_ERRORS, "Error loading ClutchVolumeTags - %s\n", err.what());
    }
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckLimpInStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool isLimpInMode = true;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if the test step should be skipped
    if(!ShortCircuitTestStep())
    {   // Perform the test step
        try
        {   // Read the Limp-In status from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadLimpInStatus"), isLimpInMode);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good data, check if in limp-in
                testResult = (isLimpInMode ? testAbort : testPass);
                testResultCode = (testPass == testResult ? "0000" : GetFaultCode("LimpInMode"));
                testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("LimpInMode"));
                if(isLimpInMode)
                {   // Send a failure result so that it will show on the printout
                    SendTestResult(testFail, testDescription, testResultCode);
                    // Display the reason for failure
                    DisplayPrompt(GetPromptBox("LimpInMode"), GetPrompt("LimpInMode"), GetPromptPriority("LimpInMode"));
                }
                // Log the data
                Log(LOG_DEV_DATA, "Limp-In Status Check: %s\n", testResult.c_str());
            }
            else
            {   // Error reading the limp-in status from the module
                Log(LOG_ERRORS, "Error reading Limp-In status - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testAbort;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the test results
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "LimpInMode", (isLimpInMode ? testFail : testPass), "");
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ResetTestStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Read the EOL status bytes from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadEOLStatusBytes"), m_eolStatusBytes);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, reset the status bytes
                INT32 index = 0;
                for(XmlNodeMapItr iter = m_resetEOLStatusBytes.begin();
                   (BEP_STATUS_SUCCESS == moduleStatus) && (iter != m_resetEOLStatusBytes.end());
                   iter++, index++)
                {   // Mask in the bits to be reset
                    UINT8 updateByte = m_analyze.ConvertData((UINT32)m_eolStatusBytes[index], (UINT32)atoh(iter->second->getValue().c_str()), SET_BITS);
                    // Write the updated byte to the module
                    UINT16 eolStatusRegister;
                    try
                    {
                        eolStatusRegister = (UINT16)atoh(iter->second->getAttribute("EolRegister")->getValue().c_str());
                    }
                    catch(XmlException &excpt)
                    {
                        eolStatusRegister = atoh(GetParameter("FirstEOLStatusRegister").c_str())+index;
                    }
                    moduleStatus = m_vehicleModule.UpdateEOLStatusByte(eolStatusRegister, updateByte);
                    Log(LOG_DEV_DATA, "Reset EOL Status Byte %d: %s - Initial: %02X, Update: %02X\n",
                        index, ConvertStatusToResponse(moduleStatus).c_str(), m_eolStatusBytes[index], updateByte);
                    // Save the updated byte
                    m_eolStatusBytes[index] = updateByte;
                }
                // Determine the test results
                testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
                testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                Log(LOG_DEV_DATA, "Reset EOL Status Bytes: %s\n", testResult.c_str());
            }
            else
            {   // Error reading the status bytes from the module
                Log(LOG_ERRORS, "Error reading the EOL Status Bytes - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartFaultLimpInMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Perform the step
        try
        {   // Start the Fault/Limp-In monitor -- pass the ignored fault list
            moduleStatus = m_vehicleModule.StartLimpInFaultMonitor(&m_ignoreFaults, &m_faults);
            // Determine the results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommuncationFailure"));
            Log(LOG_DEV_DATA, "Start Limp-In/Fault monitor: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping Test Step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopFaultLimpInMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    try
    {   // Stop the Limp-In/Fault status montor
        moduleStatus = m_vehicleModule.StopLimpInFaultMonitor();
        // Determine the results
        testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
        testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
        testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
        Log(LOG_DEV_DATA, "Stop Limp-In/Fault monitor: %s\n", testResult.c_str());
    }
    catch(ModuleException &e)
    {   // Module exception
        Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
        testResult = testAbort;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartGearLockUpMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if we need to skip this step
    if(!ShortCircuitTestStep())
    {   // Need to perform this step
        try
        {   // Start the gear monitor
            moduleStatus = m_vehicleModule.StartGearLockUpMonitor();
            // Determine the results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommuncationFailure"));
            Log(LOG_DEV_DATA, "Start Gear/TCC Lockup monitor: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopGearLockUpMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string parkResult = testSkip;
    string reverseGearResult = testSkip;
    string neutralGearResult = testSkip;
    string forwardDrivingGearResult = testSkip;
    string firstGearResult = testSkip, secondGearResult = testSkip;
    string thirdGearResult = testSkip, fourthGearResult = testSkip;
    string fifthGearResult = testSkip, sixthGearResult = testSkip;
    string noLockState = testSkip;
    string partialLockState = testSkip;
    string fullLockState = testSkip;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if we need to skip this step
    if(!ShortCircuitTestStep())
    {   // Need to perform this step
        try
        {   // Check if all required gears have been observed
            if(GetParameterBool("CheckForPark")) parkResult = CheckParkGearResults();
            reverseGearResult = CheckReverseGearResults();
            neutralGearResult = CheckNeutralGearResults();
            forwardDrivingGearResult = CheckForwardDrivingGearResults(firstGearResult, secondGearResult,
                                                                      thirdGearResult, fourthGearResult,
                                                                      fifthGearResult, sixthGearResult);
            // Check if all lockup states have been observed
            if(!GetParameterBool("UnlockStateNotRequired"))
            {   // Unlock state must be observed in order to pass the test
                noLockState = CheckTCCNoLockState();
            }
            else
            {   // Unlock state is not required.
                Log(LOG_DEV_DATA, "Unlock state not required, skipping");
                noLockState = testPass;
            }
            if(!GetParameterBool("PartialLockNotRequired"))
            {   // Partial lock state must be observed in order to pass the test
                partialLockState = CheckTCCPartialLockState();
            }
            else
            {   // Partial lock state is not required
                Log(LOG_DEV_DATA, "Partial lock state not required, skipping");
                partialLockState = testPass;
            }
            fullLockState    = CheckTCCFullLockState();
            // Stop the gear / tcc lockup monitor
            m_vehicleModule.StopGearLockUpMonitor();
            // Check the results of the test
            testResult = (!parkResult.compare(testPass) || !parkResult.compare(testSkip)) &&
                         !reverseGearResult.compare(testPass) &&
                         !neutralGearResult.compare(testPass) &&
                         !forwardDrivingGearResult.compare(testPass) &&
                         !noLockState.compare(testPass) &&
                         !partialLockState.compare(testPass) &&
                         !fullLockState.compare(testPass) ? testPass : testFail;
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("AllGearsLockStatesNotSeen"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("AllGearsLockStatesNotSeen"));
            // Report each gear individually
            SendSubtestResult("ParkObserved", parkResult, "Park observed", GetFaultCode("ParkNotSeen"));
            SendSubtestResult("ReverseGearObserved", reverseGearResult, "Reverse Gear observed", GetFaultCode("ReverseGearNotSeen"));
            SendSubtestResult("NeutralGearObserved", neutralGearResult, "Neutral Gear observed", GetFaultCode("NeutralGearNotSeen"));
            SendSubtestResult("FirstGearObserved", firstGearResult, "First Gear observed", GetFaultCode("FirstGearNotSeen"));
            SendSubtestResult("SecondGearObserved", secondGearResult, "Second Gear observed", GetFaultCode("SecondGearNotSeen"));
            SendSubtestResult("ThirdGearObserved", thirdGearResult, "Third Gear observed", GetFaultCode("ThirdGearNotSeen"));
            SendSubtestResult("FourthGearObserved", fourthGearResult, "Fourth Gear observed", GetFaultCode("FourthGearNotSeen"));
            SendSubtestResult("FifthGearObserved", fifthGearResult, "Fifth Gear observed", GetFaultCode("FifthGearNotSeen"));
            SendSubtestResult("SixthGearObserved", sixthGearResult, "Sixth Gear observed", GetFaultCode("SixthGearNotSeen"));
            // Report the results of the individual lock states
            SendSubtestResult("TCCNoLockStateObserved", noLockState, "TCC No-Lock state observed", GetFaultCode("TCCNoLockNotSeen"));
            SendSubtestResult("TCCPartialLockStateObserved", partialLockState, "TCC Partial-Lock state observed", GetFaultCode("TCCPartialLockNotSeen"));
            SendSubtestResult("TCCFullLockStateObserved", fullLockState, "TCC Full-Lock state observed", GetFaultCode("TCCFullLockNotSeen"));
            // Log the data collected
            Log(LOG_DEV_DATA, "Gear Monitor Results: %s\n\tPark: %s\n\tReverse: %s\n\tNeutral: %s\n\tFirst: %s"
                "\n\tSecond: %s\n\tThird: %s\n\tFourth: %s\n\tFifth: %s\n\tSixth: %s\n", testResult.c_str(),
                parkResult.c_str(), reverseGearResult.c_str(), neutralGearResult.c_str(), firstGearResult.c_str(),
                secondGearResult.c_str(), thirdGearResult.c_str(), fourthGearResult.c_str(),
                fifthGearResult.c_str(), sixthGearResult.c_str());
            // Log the results
            Log(LOG_DEV_DATA, "Torque Converter Clutch Lockup Results: %s\n\tNo-Lock: %s\n\tPartial-Lock: %s\n\t"
                "Full-Lock: %s\n", testResult.c_str(), noLockState.c_str(), partialLockState.c_str(),
                fullLockState.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "ParkObserved", parkResult, "",
                                 "ReverseGearObserved", reverseGearResult, "",
                                 "NeutralGearObserved", neutralGearResult, "",
                                 "ForwardDrivingGears", forwardDrivingGearResult, "",
                                 "TCCLockupStates", (!noLockState.compare(testPass) &&
                                                     !partialLockState.compare(testPass) &&
                                                     !fullLockState.compare(testPass) ? testPass : testFail), "");
    }
    else
    {   // Skip the step and just command the gear / lockup monitor to stop
        testResult = testSkip;
        m_vehicleModule.StopGearLockUpMonitor();
        Log(LOG_FN_ENTRY, "Skipping Test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartGearMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if we need to skip this step
    if(!ShortCircuitTestStep())
    {   // Need to perform this step
        try
        {   // Start the gear monitor
            moduleStatus = m_vehicleModule.StartGearMonitor();
            // Determine the results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommuncationFailure"));
            Log(LOG_DEV_DATA, "Start Gear monitor: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopGearMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    //string parkResult = testSkip;
    //string reverseGearResult = testSkip;
    //string neutralGearResult = testSkip;
    string forwardDrivingGearResult = testSkip;
    string firstGearResult = testSkip, secondGearResult = testSkip;
    string thirdGearResult = testSkip, fourthGearResult = testSkip;
    string fifthGearResult = testSkip, sixthGearResult = testSkip;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Check if all required gears have been observed
            //if(GetParameterBool("CheckForPark")) parkResult = CheckParkGearResults();
            //reverseGearResult = CheckReverseGearResults();
            //neutralGearResult = CheckNeutralGearResults();
            forwardDrivingGearResult = CheckForwardDrivingGearResults(firstGearResult, secondGearResult,
                                                                      thirdGearResult, fourthGearResult,
                                                                      fifthGearResult, sixthGearResult);
            // Stop the gear monitor
            m_vehicleModule.StopGearMonitor();
            // Check the results of the test
            //testResult = (!parkResult.compare(testPass) || !parkResult.compare(testSkip)) &&
            //             !reverseGearResult.compare(testPass) &&
            //             !neutralGearResult.compare(testPass) &&
            testResult = !forwardDrivingGearResult.compare(testPass) ? testPass : testFail;
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("AllGearsNotSeen"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("AllGearsNotSeen"));
            // Report each gear individually
//          SendSubtestResult("ParkObserved", parkResult, "Park observed", GetFaultCode("ParkNotSeen"));
//          SendSubtestResult("ReverseGearObserved", reverseGearResult, "Reverse Gear observed", GetFaultCode("ReverseGearNotSeen"));
//          SendSubtestResult("NeutralGearObserved", neutralGearResult, "Neutral Gear observed", GetFaultCode("NeutralGearNotSeen"));
            SendSubtestResult("FirstGearObserved", firstGearResult, "First Gear observed", GetFaultCode("FirstGearNotSeen"));
            SendSubtestResult("SecondGearObserved", secondGearResult, "Second Gear observed", GetFaultCode("SecondGearNotSeen"));
            SendSubtestResult("ThirdGearObserved", thirdGearResult, "Third Gear observed", GetFaultCode("ThirdGearNotSeen"));
            SendSubtestResult("FourthGearObserved", fourthGearResult, "Fourth Gear observed", GetFaultCode("FourthGearNotSeen"));
            SendSubtestResult("FifthGearObserved", fifthGearResult, "Fifth Gear observed", GetFaultCode("FifthGearNotSeen"));
            SendSubtestResult("SixthGearObserved", sixthGearResult, "Sixth Gear observed", GetFaultCode("SixthGearNotSeen"));
            // Log the data collected
//          Log(LOG_DEV_DATA, "Gear Monitor Results: %s\n\tPark: %s\n\tReverse: %s\n\tNeutral: %s\n\tFirst: %s"
//              "\n\tSecond: %s\n\tThird: %s\n\tFourth: %s\n\tFifth: %s\n\tSixth: %s\n", testResult.c_str(),
//              parkResult.c_str(), reverseGearResult.c_str(), neutralGearResult.c_str(), firstGearResult.c_str(),
//              secondGearResult.c_str(), thirdGearResult.c_str(), fourthGearResult.c_str(),
//              fifthGearResult.c_str(), sixthGearResult.c_str());
            Log(LOG_DEV_DATA, "Gear Monitor Results: %s\n\tFirst: %s"
                "\n\tSecond: %s\n\tThird: %s\n\tFourth: %s\n\tFifth: %s\n\tSixth: %s\n", testResult.c_str(),
                firstGearResult.c_str(),
                secondGearResult.c_str(), thirdGearResult.c_str(), fourthGearResult.c_str(),
                fifthGearResult.c_str(), sixthGearResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 //"ParkObserved", parkResult, "",
                                 //"ReverseGearObserved", reverseGearResult, "",
                                 //"NeutralGearObserved", neutralGearResult, "",
                                 "ForwardDrivingGears", forwardDrivingGearResult, "");
    }
    else
    {   // Skip the step and just command the lockup monitor to stop
        testResult = testSkip;
        m_vehicleModule.StopGearMonitor();
        Log(LOG_FN_ENTRY, "Skipping Test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartLockUpMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Start the torque converter clutch lockup monitor
            moduleStatus = m_vehicleModule.StartLockupMonitor();
            // Determine the results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommuncationFailure"));
            Log(LOG_DEV_DATA, "Start Torque Converter Clutch lockup monitor: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip the test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopLockUpMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string noLockState = testSkip;
    string partialLockState = testSkip;
    string fullLockState = testSkip;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Check if all lockup states have been observed
            if(!GetParameterBool("UnlockStateNotRequired"))
            {   // Unlock state must be observed in order to pass the test
                noLockState = CheckTCCNoLockState();
            }
            else
            {   // Unlock state is not required.
                Log(LOG_DEV_DATA, "Unlock state not required, skipping");
                noLockState = testPass;
            }
            if(!GetParameterBool("PartialLockNotRequired"))
            {   // Partial lock state must be observed in order to pass the test
                partialLockState = CheckTCCPartialLockState();
            }
            else
            {   // Partial lock state is not required
                Log(LOG_DEV_DATA, "Partial lock state not required, skipping");
                partialLockState = testPass;
            }
            fullLockState    = CheckTCCFullLockState();
            // Stop the monitor
            m_vehicleModule.StopLockupMonitor();
            // Determine the results
            testResult = (m_analyze.CompareData(noLockState, testPass, EQUAL) &&
                          m_analyze.CompareData(partialLockState, testPass, EQUAL) &&
                          m_analyze.CompareData(fullLockState, testPass, EQUAL) ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("AllLockupStatesNotSeen"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("AllLockupStatesNotSeen"));
            // Report the results of the individual states
            SendSubtestResult("TCCNoLockStateObserved", noLockState, "TCC No-Lock state observed", GetFaultCode("TCCNoLockNotSeen"));
            SendSubtestResult("TCCPartialLockStateObserved", partialLockState, "TCC Partial-Lock state observed", GetFaultCode("TCCPartialLockNotSeen"));
            SendSubtestResult("TCCFullLockStateObserved", fullLockState, "TCC Full-Lock state observed", GetFaultCode("TCCFullLockNotSeen"));
            // Log the results
            Log(LOG_DEV_DATA, "Torque Converter Clutch Lockup Results: %s\n\tNo-Lock: %s\n\tPartial-Lock: %s\n\t"
                "Full-Lock: %s\n", testResult.c_str(), noLockState.c_str(), partialLockState.c_str(),
                fullLockState.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode, "NoLock", noLockState, "",
                                 "PartialLock", partialLockState, "", "FullLock", fullLockState, "");
    }
    else
    {   // Skip the step and just command the lockup monitor to stop
        testResult = testSkip;
        m_vehicleModule.StopLockupMonitor();
        Log(LOG_FN_ENTRY, "Skipping Test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::EvaluateSpeedRationality(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float initialWheelSpeed = 0.0;
    float finalWheelSpeed = 100.0;
    float averageWheelSpeed = 0.0;
    float upperLimit = 0.0;
    float lowerLimit = 0.0;
    float vehicleSpeed = 0.0;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Perform the test
        try
        {   // Have the operator place the vehicle speed in the target range for test step
            testResult = WaitAndPromptForSpeed(GetParameterFloat("SpeedRationalityMinimumSpeed"),
                                               GetParameterFloat("SpeedRationalityMaximumSpeed"), GetTestStepInfoInt("ScanDelay"));
            if(testResult == testPass)
            {   // Grab the initial roller speeds
                initialWheelSpeed = GetRollSpeed();
                // Read the wheel speed from the module
                moduleStatus = ReadVehicleSpeed(vehicleSpeed);
                // Grab the final roller speeds
                finalWheelSpeed = GetRollSpeed();
                // Check the status of the data from the module
                if(BEP_STATUS_SUCCESS == moduleStatus)
                {   // Good read, evaluate the data
                    averageWheelSpeed = (initialWheelSpeed + finalWheelSpeed) / 2.0;
                    // Determine the limits of acceptable speed
                    upperLimit = averageWheelSpeed + (averageWheelSpeed * GetParameterFloat("SpeedTolerance"));
                    lowerLimit = averageWheelSpeed - (averageWheelSpeed * GetParameterFloat("SpeedTolerance"));
                    // Determine if the vehicle speed read from the module is acceptable
                    testResult = ((m_analyze.CompareData(vehicleSpeed, lowerLimit, GREATER_EQUAL) &&
                                   m_analyze.CompareData(vehicleSpeed, upperLimit, LESS_EQUAL)) ? testPass : testFail);
                    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("SpeedRationalityFail"));
                    testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("SpeedRationalityFail"));
                    // Log the data
                    Log(LOG_DEV_DATA, "Speed Rationality: %s\n\tVehicle Speed: %5.2f\n\tRoller Speed: %5.2f"
                        "\n\tUpper Limit: %5.2f\n\tLower Limit: %5.2f\n\tTolerance: %5.2f\n", testResult.c_str(),
                        vehicleSpeed, averageWheelSpeed, upperLimit, lowerLimit, GetParameterFloat("SpeedTolerance"));
                }
                else
                {   // Error reading vehicle speed from the transmission
                    Log(LOG_ERRORS, "Error reading vehicle speed from the transmission - status: %s\n",
                        ConvertStatusToResponse(moduleStatus).c_str());
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationError");
                    testDescription = GetFaultDescription("CommunicationError");
                }
            }
            else
            {   // Timeout waiting for speed to be in range
                Log(LOG_ERRORS, "Timeout waiting for vehicle speed!\n");
                testResult = testTimeout;
                testResultCode = GetFaultCode("SpeedRationalityTimeout");
                testDescription = GetFaultDescription("SpeedRationalityTimeout");
            }
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        char temp[16];
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "VehicleSpeed", CreateMessage(temp, sizeof(temp), "%5.2f", vehicleSpeed), unitsMPH,
                                 "RollerSpeed", CreateMessage(temp, sizeof(temp), "%5.2f", averageWheelSpeed), unitsMPH,
                                 "UpperLimit", CreateMessage(temp, sizeof(temp), "%5.2f", upperLimit), unitsMPH,
                                 "LowerLimit", CreateMessage(temp, sizeof(temp), "%5.2f", lowerLimit), unitsMPH,
                                 "Tolerance", GetParameter("SpeedTolerance"), "");
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
BEP_STATUS_TYPE GenericTransmissionTCTemplate<VehicleModuleType>::ReadVehicleSpeed(float &vehicleSpeed)
{
    return m_vehicleModule.GetInfo(GetDataTag("ReadVehicleSpeed"), vehicleSpeed);
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ClearDiagnosticTellTale(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Need to perform the test step
        try
        {   // Command the module to clear tell tales
            moduleStatus = m_vehicleModule.ClearDiagnosticRetention();
            // Determine the result of the clear command
            if(!GetParameterBool("ForceClearDiagTellTalePass") || 
               !GetParameterBool("AlwaysPassClearDiagnosticTellTale"))
            {
                testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            }
            else
            {
                testResult = testPass;
            }
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
            // Log the results
            Log(LOG_DEV_DATA, "Clear Diagnostic Retention: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckUplineStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT16 uplineRegister = 0xFFFF;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Perform this test
        try
        {   // Read the upline register from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadUplineRegister"), uplineRegister);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, evaluate the data
                Log(LOG_DEV_DATA, "Upline Register: %04X, Upline Complete Mask: %s\n", uplineRegister,
                    GetParameter("UplineRegisterMask").c_str());
                if(m_analyze.CompareData(uplineRegister, (UINT16)atoh(GetParameter("UplineRegisterMask").c_str()), ANY_BITS_SET))
                {   // Upline tests not complete - abort the test
                    testResult = testAbort;
                    // Determine which tests were not complete
                    for(XmlNodeMapItr iter = m_uplineTestsToCheck.begin(); iter != m_uplineTestsToCheck.end(); iter++)
                    {   // Determine if the current upline test is not complete
                        if(m_analyze.CompareData(uplineRegister, (UINT16)atoh(iter->second->getValue().c_str()), ANY_BITS_SET))
                        {   // Current upline test not complete - report the failure
                            SendSubtestResult(iter->second->getName(), testFail,
                                              GetFaultDescription(iter->second->getName()),
                                              GetFaultCode(iter->second->getName()));
                            Log(LOG_ERRORS, "%s not complete upline!\n", iter->second->getName().c_str());
                        }
                        else
                        {   // Current upline test completed - report the pass
                            SendSubtestResult(iter->second->getName(), testPass);
                        }
                    }
                    DisplayTimedPrompt(GetPrompt("TransmissionUplineAbort"), GetPromptBox("TransmissionUplineAbort"),
                                       GetPromptPriority("TransmissionUplineAbort"),
                                       GetPromptDuration("TransmissionUplineAbort"));
                }
                else
                {   // All upline tests complete
                    testResult = testPass;
                }
                // Fill out the remaining report data
                testResultCode = (testPass == testResult ? "0000" : GetFaultCode("UplinesNotComplete"));
                testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("UplinesNotComplete"));
            }
            else
            {   // Error reading upline register from the module
                Log(LOG_ERRORS, "Error reading upline register from the module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &exception)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), exception.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        char temp[16];
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "UplineRegister", CreateMessage(temp, sizeof(temp), "%04X", uplineRegister), "",
                                 "UplineRegisterMask", GetParameter("UplineRegisterMask"), "");
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::MonitorKickDown(void)
{
    string testResult(BEP_TESTING_STATUS);
    string testResultCode("0000");
    string testDescription(GetTestStepInfo("Description"));
    string operatorResult("Unknown");
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::MonitorKickDown()");
    // Prompt the operator to speed
    testResult = WaitAndPromptForSpeed(GetParameterFloat("KickDownSpeedMin"),
                                       GetParameterFloat("KickDownSpeedMax"),
                                       GetParameterInt("KickDownSPeedCheckDelay"));
    if(!testResult.compare(testPass) && TimeRemaining())
    {   // Vehicle is at speed and there is time remaining
                                       // Prompt the operator to perform the kickdown test
        vector<string> validResponses;
        SetupOperatorResponses(INPUT_SERVER_TEST_RESULT_STATE, validResponses);
        testResult = GetOperatorInput(INPUT_SERVER_TEST_RESULT_STATE, GetPrompt("PerformKickDown"), validResponses);
    }
    else
    {   // Vehicle not at speed
        Log(LOG_ERRORS, "Failed while waiting for vehicle to achieve target speed range - Speed Result: %s, Time Remaining: %s",
            testResult.c_str(), TimeRemaining() ? "Yes" : "No");
    }
    // Report the reuslts
    SendTestResult(testResult, testDescription, testResultCode);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::MonitorKickDown()");
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::MonitorOverdriveLockout(void)
{
    string testResult = BEP_TESTING_STATUS, preconditionResult = BEP_TESTING_STATUS;
    string buttonResult = BEP_TESTING_STATUS, gearResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string currentGear = "Unknown";
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Wait for overdrive conditions to be met
            preconditionResult = WaitForOverdriveConditions();
            // Check if preconditions were satisifed
            if(testPass == preconditionResult)
            {   // All conditions correct, wait for operator to push button
                buttonResult = WaitForOverdriveCancelButton();
                // Check result of OD Cancel Button
                if((testPass == buttonResult) && !GetParameterBool("SkipTransGearKickdown"))
                {   // Saw OD Cancel button, look for gear to kickdown
                    gearResult = WaitForOverdriveGearKickdown();
                    // Prompt operator to enable overdrive
                    DisplayPrompt(GetPromptBox("SelectOD"), GetPrompt("SelectOD"), GetPromptPriority("SelectOD"));
                    // Check the result of the gear kickdown
                    if(testFail == gearResult)
                    {   // Communication failure reading current gear
                        Log(LOG_ERRORS, "Communication failure reading curent gear from the module\n");
                        testResultCode = GetFaultCode("CommunicationFailure");
                        testDescription = GetFaultDescription("CommunicationFailure");
                    }
                    else if(testTimeout == gearResult)
                    {   // Timeout waiting for gear kickdown
                        Log(LOG_ERRORS, "Timeout waiting for transmission to kickdown\n");
                        testResultCode = GetFaultCode("ODCancelGearTimeout");
                        testDescription = GetFaultDescription("ODCancelGearTimeout");
                    }
                    // Allow operator time to observe and act on the prompt
                    BposSleep(GetParameterInt("EnableOverdrivePromptDelay"));
                }
                else if(!buttonResult.compare(testPass) && GetParameterBool("SkipTransGearKickdown"))
                {   // Prompt operator to enable overdrive
                    DisplayPrompt(GetPromptBox("SelectOD"), GetPrompt("SelectOD"), GetPromptPriority("SelectOD"));
                    // Allow operator time to observe and act on the prompt
                    BposSleep(GetParameterInt("EnableOverdrivePromptDelay"));
                    // Button press passed and we are not checking for kick down
                    Log(LOG_DEV_DATA, "Detected Tow/Haul gear kick down check per parameter");
                    gearResult = testPass;
                }
                else if(testTimeout == buttonResult)
                {   // Timeout waiting for OD cancel button
                    Log(LOG_ERRORS, "Timeout waiting for OD Cancel Button\n");
                    testResultCode = GetFaultCode("ODCancelButtonTimeout");
                    testDescription = GetFaultDescription("ODCancelButtonTimeout");
                }
                else
                {   // Communication failure
                    Log(LOG_ERRORS, "Communication failure reading OD Cancel button\n");
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                }
                // Remove Prompt
                RemovePrompt(GetPromptBox("MaintainSpeed"),GetPrompt("MaintainSpeed"),GetPromptPriority("MaintainSpeed"));
                RemovePrompt(GetPromptBox("SelectOD"), GetPrompt("SelectOD"), GetPromptPriority("SelectOD"));
            }
            else if(testTimeout == preconditionResult)
            {   // Timeout waiting for overdrive gear
                Log(LOG_ERRORS, "Timeout waiting for OverDrive conditions to be satisfied\n");
                testResultCode = GetFaultCode("PreconditionTimeout");
                testDescription = GetFaultDescription("PreconditionTimeout");
            }
            else
            {   // Communication failure
                Log(LOG_ERRORS, "Communication error reading current gear from the module\n");
                testResultCode = GetFaultCode("CommuncationFailure");
                testDescription = GetFaultDescription("CommuncationFailure");
            }
            // Determine the test result
            testResult = (((testPass == preconditionResult) && (testPass == buttonResult) &&
                           (testPass == gearResult)) ? testPass : testFail);
            // Log the results
            Log(LOG_DEV_DATA, "Monitor Overdrive Lockout: %s\n\tPreconditions: %s\n\tOD Cancel Button: %s\n\t"
                "OD Gear Kickdown: %s\n", testResult.c_str(), preconditionResult.c_str(),
                buttonResult.c_str(), gearResult.c_str());
        }
        catch(ModuleException &exception)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), exception.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "Preconditions", preconditionResult, "",
                                 "ODCancelButton", buttonResult, "",
                                 "ODGearKickdown", gearResult, "");
    }
    else
    {   // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

//---------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForGears(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    bool gear1Observed = false;
    bool gear2Observed = false;
    bool gear3Observed = false;
    bool gear4Observed = false;
    bool gear5Observed = false;
    bool gear6Observed = false;
    bool checkGear5 = !GetParameter("OverdriveGear").compare(m_vehicleModule.GetGear5String()) ||
                      !GetParameter("OverdriveGear").compare(m_vehicleModule.GetGear6String());
    bool checkGear6 = !GetParameter("OverdriveGear").compare(m_vehicleModule.GetGear6String());
    bool allGearsObserved = false;
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Prompt the operator to speed
        SystemWrite(GetDataTag("SpeedTarget"), GetParameter("MaxGearSpeedRange"));
        DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
        // Determine if we need to check for 5th and 6th gears
        gear5Observed = !checkGear5;
        gear6Observed = !checkGear6;
        // Wait for all gears to be observed
        SetStartTime();
        do
        {   // Check if each gear was seen and mark it off in the module
                                       // Check for first gear
            if(!gear1Observed && m_vehicleModule.Gear1Observed())
            {
//              SetFirstGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FirstGearStatusRegister").c_str()),
//                                                           GetParameterInt("FirstGearBit"), testPass));
                SetFirstGearUpdateResult(testPass);
                gear1Observed = true;
                Log(LOG_DEV_DATA, "Updated first gear result: %s\n", GetFirstGearUpdateResult().c_str());
            }
            // Check for second gear
            if(!gear2Observed && m_vehicleModule.Gear2Observed())
            {
//              SetSecondGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("SecondGearStatusRegister").c_str()),
//                                                            GetParameterInt("SecondGearBit"), testPass));
                SetSecondGearUpdateResult(testPass);
                gear2Observed = true;
                Log(LOG_DEV_DATA, "Updated second gear result: %s\n", GetSecondGearUpdateResult().c_str());
            }
            // Check for third gear
            if(!gear3Observed && m_vehicleModule.Gear3Observed())
            {
                //SetThirdGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("ThirdGearStatusRegister").c_str()),
                //                                             GetParameterInt("ThirdGearBit"), testPass));
                SetThirdGearUpdateResult(testPass);
                gear3Observed = true;
                Log(LOG_DEV_DATA, "Updated third gear result: %s\n", GetThirdGearUpdateResult().c_str());
            }
            // Check for fourth gear
            if(!gear4Observed && m_vehicleModule.Gear4Observed())
            {
//              SetFourthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FourthGearStatusRegister").c_str()),
//                                                            GetParameterInt("FourthGearBit"), testPass));
                SetFourthGearUpdateResult(testPass);
                gear4Observed = true;
                Log(LOG_DEV_DATA, "Updated fourth gear result: %s\n", GetFourthGearUpdateResult().c_str());
            }
            // Check for fifth gear if it is equipped
            if(checkGear5 && !gear5Observed && m_vehicleModule.Gear5Observed())
            {
                //SetFifthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FifthGearStatusRegister").c_str()),
                //                                             GetParameterInt("FifthGearBit"), testPass));
                SetFifthGearUpdateResult(testPass);
                gear5Observed = true;
                Log(LOG_DEV_DATA, "Updated fifth gear result: %s\n", GetFifthGearUpdateResult().c_str());
            }
            // Check for sixth gear if it is equipped
            if(checkGear6 && !gear6Observed && m_vehicleModule.Gear6Observed())
            {
                //SetSixthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("SixthGearStatusRegister").c_str()),
                //                                             GetParameterInt("SixthGearBit"), testPass));
                SetSixthGearUpdateResult(testPass);
                gear6Observed = true;
                Log(LOG_DEV_DATA, "Updated sixth gear result: %s\n", GetSixthGearUpdateResult().c_str());
            }
            // Determine if all gears have been observed or the maximum gear has been observed
            allGearsObserved = ((gear1Observed && gear2Observed && gear3Observed && gear4Observed && gear5Observed && gear6Observed) ||
                                (checkGear6 && gear6Observed) ||
                                (checkGear5 && gear5Observed && !checkGear6) ||
                                (!checkGear5 && gear4Observed));
            // Wait between checks
            if(!allGearsObserved) BposSleep(100);
            // Keep checking while time remaining and good system status and not all gears seen
        } while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && !allGearsObserved);
        // Check the exit conditions
        if(!TimeRemaining() && !allGearsObserved)
        {   // Timeout waiting for all gears and lock states
            testResult = testTimeout;
            testResultCode = GetFaultCode("TestStepTimeout");
            testDescription = GetFaultDescription("TestStepTimeout");
            Log(LOG_ERRORS, "Timeout waiting for all gears\n");
        }
        else if(BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status
            testResult = testFail;
            testResultCode = GetFaultCode("StatusCheckFailure");
            testDescription = GetFaultCode("StatusCheckFailure");
            Log(LOG_ERRORS, "Bad system status while monitoring gears - status: %s\n",
                ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else
        {   // Check the results
            testResult = allGearsObserved ? testPass : testFail;
            testResultCode = testPass == testResult ? testResultCode : GetFaultCode("AllGearsAllLocksNotSeen");
            testDescription = testPass == testResult ? testDescription : GetFaultDescription("AllGearsAllLocksNotSeen");
        }
        // Report the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "AllGearsObserved", allGearsObserved ? testPass : testFail, "");
        RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
    }
    else
    {   // Skip this step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // NOTE: Always return pass. When the gear monitor is stopped, it will prompt operator for any
    //       gears that have not been observed.  If the missing gear is observed during that period,
    //       we do not want to cause a flase failure.
    return testPass;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForGearsAndLockup(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Check if we need to wait for all lock states
    bool fullLockObserved = false;
    bool partLockObserved = GetParameterBool("UnlockStateNotRequired");
    bool noLockObserved   = GetParameterBool("PartialLockNotRequired");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Wait for all gears
        WaitForGears();
        // Prompt the operator to speed
        SystemWrite(GetDataTag("SpeedTarget"), GetParameter("MaxGearSpeedRange"));
        DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
        // Wait for the TCC to fully lock up
        do
        {       // Check for TCC lock-up states
            if(!noLockObserved && m_vehicleModule.TCCNoLockupObserved())
            {
                string updateResult(testPass);
                if(!GetParameterBool("SkipUpdateTccNoLockRegister"))
                {
                    updateResult = UpdateEOLStatusByte(atoh(GetParameter("TCCNoLockStatusRegister").c_str()),
                                                       GetParameterInt("TCCNoLockTestBit"), testPass);
                }
                SetUnlockStateUpdateResult(updateResult);
                noLockObserved = true;
                Log(LOG_DEV_DATA, "Updated TCC No-Lock result: %s\n", GetUnlockStateUpdateResult().c_str());
            }
            else if(!noLockObserved && GetParameterBool("UnlockStateNotRequired"))
            {
                Log(LOG_DEV_DATA, "TCC No-Lock State not required");
                noLockObserved = true;
                SetUnlockStateUpdateResult(testPass);
            }
            if(!partLockObserved && m_vehicleModule.TCCPartialLockupObserved())
            {
                string updateResult(testPass);
                if(!GetParameterBool("SkipUpdateTccPartialLockRegister"))
                {
                    updateResult = UpdateEOLStatusByte(atoh(GetParameter("TCCPartialLockStatusRegister").c_str()),
                                                       GetParameterInt("TCCPartialLockTestBit"), testPass);
                }
                SetPartialLockStateUpdateResult(updateResult);
                partLockObserved = true;
                Log(LOG_DEV_DATA, "Updated TCC Partial-Lock result: %s\n", GetPartialLockStateUpdateResult().c_str());
            }
            else if(!partLockObserved && GetParameterBool("PartialLockNotRequired"))
            {
                Log(LOG_DEV_DATA, "TCC Partial Lock State not required");
                partLockObserved = true;
                SetPartialLockStateUpdateResult(testPass);
            }
            if(!fullLockObserved && m_vehicleModule.TCCFullLockupObserved())
            {
                string updateResult(testPass);
                if(!GetParameterBool("SkipUpdateTccFullLockRegister"))
                {
                    updateResult = UpdateEOLStatusByte(atoh(GetParameter("TCCFullLockStatusRegister").c_str()),
                                                       GetParameterInt("TCCFullLockTestBit"), testPass);
                }
                SetFullLockStateUpdateResult(updateResult);
                fullLockObserved = true;
                Log(LOG_DEV_DATA, "Updated TCC Full-Lock result: %s\n", GetFullLockStateUpdateResult().c_str());
            }
            // Determine if all lock states have been observed
            m_vehicleModule.SetAllLockStatesSeen(noLockObserved && partLockObserved && fullLockObserved);
            // Wait between checks
            if(!m_vehicleModule.AllLockStatesObserved())   BposSleep(250);
            // Keep checking while time remaining and good system status and all lock states not seen
        } while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && !m_vehicleModule.AllLockStatesObserved());
        // Check the exit conditions
        if(!TimeRemaining() && !m_vehicleModule.AllLockStatesObserved())
        {   // Timeout waiting for all gears and lock states
            // If full lock state not observed, attempt again with throttle graph
            testResult = testTimeout;
            if(!m_vehicleModule.TCCFullLockupObserved() && GetParameterBool("AttemptFullLockWithThrottleGraph"))
            {
                testResult = WaitForFullLockWithThrottleGraph();
                m_vehicleModule.SetAllLockStatesSeen(noLockObserved && partLockObserved && m_vehicleModule.TCCFullLockupObserved());
            }
            if(!testResult.compare(testTimeout))
            {
                testResultCode = GetFaultCode("TestStepTimeout");
                testDescription = GetFaultDescription("TestStepTimeout");
                Log(LOG_ERRORS, "Timeout waiting for all lock states\n");
            }
            else
            {   // Check the results
                testResult = m_vehicleModule.AllLockStatesObserved() ? testPass : testFail;
                testResultCode = testPass == testResult ? testResultCode : GetFaultCode("AllGearsAllLocksNotSeen");
                testDescription = testPass == testResult ? testDescription : GetFaultDescription("AllGearsAllLocksNotSeen");
            }
        }
        else if(BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status
            testResult = testFail;
            testResultCode = GetFaultCode("StatusCheckFailure");
            testDescription = GetFaultCode("StatusCheckFailure");
            Log(LOG_ERRORS, "Bad system status while monitoring lockup states - status: %s\n",
                ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else
        {   // Check the results
            testResult = m_vehicleModule.AllLockStatesObserved() ? testPass : testFail;
            testResultCode = testPass == testResult ? testResultCode : GetFaultCode("AllGearsAllLocksNotSeen");
            testDescription = testPass == testResult ? testDescription : GetFaultDescription("AllGearsAllLocksNotSeen");
        }
        // Report the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "AllLockStatesSeen", m_vehicleModule.AllLockStatesObserved() ? testPass : testFail, "");
        RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
    }
    else
    {   // Skip the step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // NOTE: Always return pass. When the gear monitor is stopped, it will prompt operator for any
    //       gears that have not been observed.  If the missing gear is observed during that period,
    //       we do not want to cause a flase failure.
    return testPass;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForThirdGear(const string speedRange)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step must be performed
    if(!ShortCircuitTestStep())
    {   // Display the required prompts and target speeds
        DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
        SystemWrite(GetDataTag("SpeedTarget"), speedRange);
        // Keep looking for requested gear until gear is observed or we time out
        while(TimeRemaining() && !m_vehicleModule.Gear3Observed() && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(GetTestStepInfoInt("ScanDelay"));
        // Remove the prompts and target speed
        RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
        SystemWrite(GetDataTag("SpeedTarget"), "0 0");
        Log(LOG_DEV_DATA, "%s::%s gear observed: %s", GetComponentName().c_str(), GetTestStepName().c_str(),
            m_vehicleModule.Gear3Observed() ? "True" : "False");
        SendTestResult(m_vehicleModule.Gear3Observed() ? testPass : testFail, testDescription, testResultCode);
    }
    else
    {   // Need to skip the test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    Log(LOG_FN_ENTRY, "%s::%s - Exit", GetComponentName().c_str(), GetTestStepName().c_str());
    return testPass;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForFullLockWithThrottleGraph(void)
{
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "GenericTransmissionTCTemplate::WaitForFullLockWithThrottleGraph() - Enter");
    // Restart the timer and display TPS meter
    SetStartTime();
    SystemWrite(GetDataTag("TPSTarget"), GetParameter("FullLockTpsRange"));
    SystemWrite(GetDataTag("TPSActive"), true);
    DisplayPrompt(GetPromptBox("ThrottleInGreenBand"), GetPrompt("ThrottleInGreenBand"), GetPromptPriority("ThrottleInGreenBand"));
    // Wait for TCC full lock while displaying the current TPS reading
    float currentTps = 0.0;
    do
    {   // Update the current TPS reading on the display
        m_vehicleModule.ReadModuleData(GetDataTag("CurrentTPS"), currentTps);
        SystemWrite(GetDataTag("TPSValue"), currentTps);
        // Wait a bit before the next update
        BposSleep(GetParameterInt("TpsUpdateDelay"));
        // Keep loop while time remaining, good system status and full lock not observed
    } while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && !m_vehicleModule.TCCFullLockupObserved());
    // Determine the result
    if(m_vehicleModule.TCCFullLockupObserved())
    {   // Observed full lock, update the status
        string updateResult(testPass);
        if(!GetParameterBool("SkipUpdateTccFullLockRegister"))
        {
            updateResult = UpdateEOLStatusByte(atoh(GetParameter("TCCFullLockStatusRegister").c_str()),
                                               GetParameterInt("TCCFullLockTestBit"), testPass);
        }
        SetFullLockStateUpdateResult(updateResult);
        testResult = testPass;
    }
    else if(!TimeRemaining())
    {
        testResult = testTimeout;
    }
    else
    {
        testResult = testFail;
    }
    // Remove the prompt and deactivate the TPS meter
    SystemWrite(GetDataTag("TPSActive"), false);
    SystemWrite(GetDataTag("TPSTarget"), "0 0");
    RemovePrompt(GetPromptBox("ThrottleInGreenBand"), GetPrompt("ThrottleInGreenBand"), GetPromptPriority("ThrottleInGreenBand"));
    Log(LOG_FN_ENTRY, "GenericTransmissionTCTemplate::WaitForFullLockWithThrottleGraph() - Exit");
    return testResult;
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::RequestGearShift(string requestedGear,
                                                                        string promptForGear)
{
    return RequestStateByPrompt(requestedGear, promptForGear,
                                "ReadCurrentGear",
                                GetParameterInt("SuccessiveGearReadMinimum"),
                                GetParameterInt("SuccessiveGearReadDelay"));
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::RequestLeverShift(string requestedLever,
                                                                         string promptForLever)
{
    Log(LOG_DEV_DATA, "Entering GenericTransmissionTCTemplate::RequestLeverShift");
    return RequestStateByPrompt(requestedLever, promptForLever,
                                "ReadCurrentLeverPosition",
                                GetParameterInt("SuccessiveLeverReadMinimum"),
                                GetParameterInt("SuccessiveLeverReadDelay"));
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::RequestRawLeverShift(string requestedLever, 
	string promptForLever) 
{
	return RequestStateByPrompt(requestedLever, promptForLever, 
		"ReadCurrentRawLeverPosition", 
		GetParameterInt("SuccessiveLeverReadMinimum"),
		GetParameterInt("SuccessiveLeverReadDelay"));		
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::RequestAutostickShift(string requestedState,
                                                                             string promptForLever)
{
    return RequestStateByPrompt(requestedState, promptForLever,
                                "ReadCurrentAutostickPosition",
                                GetParameterInt("SuccessiveAutostickReadMinimum"),
                                GetParameterInt("SuccessiveAutostickReadDelay"));
}


template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::RequestStateByPrompt(string requestedState,
                                                                            string promptForState, string currentStateTag, INT32 minimumSuccessiveReads, INT32 successiveReadDelay)
{
    Log(LOG_DEV_DATA, "Entering GenericTransmissionTCTemplate::RequestStateByPrompt");
    INT32 successiveReads = 0;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    string currentState(BEP_NO_DATA);                   // Used to store current state from module read

    //Do not show the prompt unless the current state does not match the request
    bool showingPrompt = false;

    //Continue checking and prompting until the current state matches the requested state
    //Also allow a minimum amount of successive reads to ensure this is not a pass-through state
    while(TimeRemaining() && (successiveReads < minimumSuccessiveReads) && (BEP_STATUS_SUCCESS == StatusCheck()))
    {
        //Get the current state from the module
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag(currentStateTag), currentState);

        if(moduleStatus == BEP_STATUS_SUCCESS)
        {   // Check that the requested state matches the current state
            Log(LOG_DEV_DATA, "Comparing Requested state: %s current state: %s",
                requestedState.c_str(), currentState.c_str());
            if(requestedState == currentState)
            {   //Increment the successive reads until the minimum is met
                successiveReads++;
                if(successiveReads >= minimumSuccessiveReads) break;
            }
            else
            {
                if(!showingPrompt)
                {   //Request the driver to perform action for requested state
                    DisplayPrompt(GetPromptBox(promptForState), GetPrompt(promptForState), GetPromptPriority(promptForState));

                    //Do not show the prompt more than once
                    showingPrompt = true;
                }
                //Reset the successive reads counter when request is not met
                successiveReads = 0;
            }
        }
        else
        {
            Log(LOG_ERRORS, "Failed to read the current state with data tag: %s\n", GetDataTag(currentStateTag).c_str());
        }

        //Wait between successive reads so that a quick shift through is not recorded
        BposSleep(successiveReadDelay);
    }

    if(showingPrompt)
    {   //Remove the prompt from the driver screen if it is being shown
        RemovePrompt(GetPromptBox(promptForState), GetPrompt(promptForState), GetPromptPriority(promptForState));
    }

    //Return true if the requested state was read the minimum amount of times
    bool bStateSeen = (successiveReads >= minimumSuccessiveReads);
    Log(LOG_DEV_DATA, "Requested (%i of minimum %i reads) state: '%s' lever seen: %s", successiveReads, minimumSuccessiveReads,
        requestedState.c_str(), bStateSeen ? "True" : "False");
    return bStateSeen;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckShiftLever()
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step must be performed
    if(!ShortCircuitTestStep())
    {
        bool bAllGearsSeen = true;
        bool bRequestGear = false;
        bool bAllLeversSeen = true;
        bool bRequestLever = false;

        //Check standard PRND
        bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverParkString(), "ShiftToPark");
        if(!bRequestLever) bAllLeversSeen = false;

        bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverReverseString(), "ShiftToReverse");
        if(!bRequestLever) bAllLeversSeen = false;

        bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverNeutralString(), "ShiftToNeutral");
        if(!bRequestLever) bAllLeversSeen = false;

        bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverDriveString(), "ShiftToDrive");
        if(!bRequestLever) bAllLeversSeen = false;

        CheckAutostickBits();
        if(IsAutostick())
        {   //(PRNOA)
            Log(LOG_DEV_DATA, "%s::%s Autostick is equipped", GetComponentName().c_str(), GetTestStepName().c_str());

            //Command operator to move to the autostick position (Sec. 3.20)
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            //Command and verify that Autostick Up Switch= Open and Autostick Down Switch= Open (Sec. 3.24)
            bRequestLever = RequestAutostickShift(m_vehicleModule.GetShiftLeverOpenString(), "ShiftToOpen");
            if(!bRequestLever) bAllLeversSeen = false;

            //check for Gear = 1st
            bRequestGear = RequestGearShift(m_vehicleModule.GetGear1String(), "ShiftToFirst");
            if(!bRequestGear) bAllGearsSeen = false;

            //Command operator to hold the down with the lever
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            //Command and verify that Autostick Up Switch= Open and Autostick Down Switch= Closed (Sec. 3.24).
            bRequestLever = RequestAutostickShift(m_vehicleModule.GetShiftLeverDownString(), "ShiftToDownHold");
            if(!bRequestLever) bAllLeversSeen = false;

            //Command operator to tap the up with the lever
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            bRequestLever = RequestAutostickShift(m_vehicleModule.GetShiftLeverUpString(), "ShiftToUpTap");
            if(!bRequestLever) bAllLeversSeen = false;

            //check for shift lever position = Autostick and Gear = 2nd
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            bRequestGear = RequestGearShift(m_vehicleModule.GetGear2String(), "ShiftToSecond");
            if(!bRequestGear) bAllGearsSeen = false;

            //Command operator to hold the up with the lever
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            //Command and verify that Autostick Up Switch= Closed and Autostick Down Switch= Open
            bRequestLever = RequestAutostickShift(m_vehicleModule.GetShiftLeverUpString(), "ShiftToUpHold");
            if(!bRequestLever) bAllLeversSeen = false;

            //check for Gear = 3rd
            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverAutostickString(), "ShiftToAutostick");
            if(!bRequestLever) bAllLeversSeen = false;
            bRequestGear = RequestGearShift(m_vehicleModule.GetGear3String(), "ShiftToThird");
            if(!bRequestGear) bAllGearsSeen = false;
            //TODO: The tester must check for shift lever position = Autostick and Gear = 2nd
        }
        else
        {   //(PRNO3L)
            Log(LOG_DEV_DATA, "%s::%s Autostick is not equipped", GetComponentName().c_str(), GetTestStepName().c_str());

            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverLowString(), "ShiftToLow");
            if(!bRequestLever) bAllLeversSeen = false;

            bRequestLever = RequestLeverShift(m_vehicleModule.GetShiftLeverThirdString(), "ShiftToThird");
            if(!bRequestLever) bAllLeversSeen = false;
        }


        Log(LOG_DEV_DATA, "%s::%s all levers seen: %s", GetComponentName().c_str(), GetTestStepName().c_str(),
            bAllLeversSeen ? "True" : "False");
        Log(LOG_DEV_DATA, "%s::%s all gears seen: %s", GetComponentName().c_str(), GetTestStepName().c_str(),
            bAllGearsSeen ? "True" : "False");
        if(bAllGearsSeen && bAllLeversSeen)
        {
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip the test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    Log(LOG_FN_ENTRY, "%s::%s - Exit", GetComponentName().c_str(), GetTestStepName().c_str());
    return testPass;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::EvaluateQuickLearn(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    float initialOilTemp = 0.0;
    float finalOilTemp = 0.0;
    bool trackQuickLearnstatus = false;
    string startResult = testFail;
    string monitorResult = testFail;
    string finishResult = testFail;
    // Check if the test step should be skipped
    if(!ShortCircuitTestStep() || GetParameterBool("AlwaysPerformQuicklearn"))
    {   // Attempt to perform the quick learn procedure
        try
        {   // Read the initial oil temp from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadTransaxleOilTemp"), initialOilTemp);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, verify oil temp is good
                Log(LOG_DEV_DATA, "Initial Transaxle Oil Temperature: %5.2f\n", initialOilTemp);
                if(m_analyze.CompareData(initialOilTemp, GetParameterFloat("MinimumOilTemperature"), GREATER_EQUAL) &&
                   m_analyze.CompareData(initialOilTemp, GetParameterFloat("MaximumOilTemperature"), LESS_EQUAL))
                {   // Prompt Operator to Press and Maintain Brake
                    DisplayPrompt(GetPromptBox("MaintainBrake"),GetPrompt("MaintainBrake"),GetPromptPriority("MaintainBrake"));
                    // Prompt Operator to Shift to Neutral
                    DisplayPrompt(GetPromptBox("ShiftToNeutral"),GetPrompt("ShiftToNeutral"),GetPromptPriority("ShiftToNeutral"));
                    // Oil temp is in range, start the procedure
                    trackQuickLearnstatus = false;
                    SetQuickLearnStarted(trackQuickLearnstatus);
                    startResult = StartQuickLearnProcedure();
                    // Check result of the start
                    if(testPass == startResult)
                    {   // Procedure started, monitor for finish
                        trackQuickLearnstatus = true;
                        SetQuickLearnStarted(trackQuickLearnstatus);
                        monitorResult = MonitorQuickLearnProcedure();
                        // Check the result of the monitoring
                        if(testPass == monitorResult)
                        {   // Monitor result is good, finish up
                            finishResult = FinishQuickLearnProcedure();
                            // Check the results of the finish operation
                            if(testPass == finishResult)
                            {   // Finish procedure completed successfully, check oil temp
                                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadTransaxleOilTemp"), finalOilTemp);
                                // Check the status of the read
                                if(BEP_STATUS_SUCCESS == moduleStatus)
                                {   // Good read, verify oil temp is good
                                    Log(LOG_DEV_DATA, "Final Transaxle Oil Temperature: %5.2f\n", finalOilTemp);
                                    testResult = (m_analyze.CompareData(initialOilTemp, GetParameterFloat("MinimumOilTemperature"), GREATER_EQUAL) &&
                                                  m_analyze.CompareData(initialOilTemp, GetParameterFloat("MaximumOilTemperature"), LESS_EQUAL) ?
                                                  testPass : testFail);
                                    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("TransAxleOilTemperatureOutOfRange"));
                                    testDescription = (testPass == testResult ? testDescription : GetFaultDescription("TransAxleOilTemperatureOutOfRange"));
                                }
                                else
                                {   // Communication failure reading the oil temp
                                    Log(LOG_ERRORS, "Error reading final oil temperature - status: %s\n",
                                        ConvertStatusToResponse(moduleStatus).c_str());
                                    testResult = testFail;
                                    testResultCode = GetFaultCode("CommunicationFailure");
                                    testDescription = GetFaultDescription("CommunicationFailure");
                                }
                            }
                            else
                            {   // Failure during finish up
                                Log(LOG_ERRORS, "Failed during quicklearn finish-up\n");
                                testResult = testFail;
                                testResultCode = GetFaultCode("QuickLearnFinishupFail");
                                testDescription = GetFaultDescription("QuickLearnFinishupFail");
                            }
                        }
                        else
                        {   // Failed during monitor
                            Log(LOG_ERRORS, "Failed during monitor QuickLearn Status\n");
                            testResult = testFail;
                            testResultCode = GetFaultCode("QuickLearnMonitorFail");
                            testDescription = GetFaultDescription("QuickLearnMonitorFail");
                        }
                    }
                    else
                    {   // Failed to start quicklearn procedure
                        Log(LOG_ERRORS, "Failed to start the quicklearn procedure\n");
                        testResult = testFail;
                        testResultCode = GetFaultCode("StartQuickLearnFail");
                        testDescription = GetFaultDescription("StartQuickLearnFail");
                    }
                    // Remove Prompt
                    RemovePrompt(GetPromptBox("ShiftToNeutral"),GetPrompt("ShiftToNeutral"),GetPromptPriority("ShiftToNeutral"));
                    // Remove Prompt
                    RemovePrompt(GetPromptBox("MaintainBrake"),GetPrompt("MaintainBrake"),GetPromptPriority("MaintainBrake"));
                }
                else
                {   // Oil temperature is out of range
                    Log(LOG_ERRORS, "ERROR GenericTransmissionTCTemplate::EvaluateQuickLearn - Oil temperature out of range\n");
                    testResult = testFail;
                    testResultCode = GetFaultCode("TransAxleOilTemperatureOutOfRange");
                    testDescription = GetFaultDescription("TransAxleOilTemperatureOutOfRange");
                }
            }
            else
            {   // Error reading initial oil temp from the module
                Log(LOG_ERRORS, "Error reading initial oil temperature - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &exception)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), exception.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test results
        char temp[16];
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "InitialOilTemp", CreateMessage(temp, sizeof(temp), "%5.2f", initialOilTemp), unitsDEGF,
                                 "FinalOilTemp", CreateMessage(temp, sizeof(temp), "%5.2f", finalOilTemp), unitsDEGF,
                                 "StartQuickLearnResult", startResult, "",
                                 "MonitorQuickLearnResult", monitorResult, "",
                                 "FinishQuickLearnResult", finishResult, "");
    }
    else
    {   // Skipping the test step
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ClearOverallStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {   // Update the status byte in the module
        testResult = UpdateEOLStatusByte(atoh(GetParameter("OverallStatusRegister").c_str()),
                                         GetParameterInt("EOLTestStatusBit"), GetOverallResult());
        testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
        testDescription = (testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure"));
        // Report the result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Skip the step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
BEP_STATUS_TYPE GenericTransmissionTCTemplate<VehicleModuleType>::ReadReverseGear( bool &inReverseGear )
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "ReadReverseGear";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    string currentGear(BEP_NO_DATA);    // Used to store current gear from module read
    inReverseGear = false;

    try
    {   //Read the current gear from the module
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadCurrentGear"), currentGear);
        if(moduleStatus == BEP_STATUS_SUCCESS)
        {   // If vehicle is in reverse
            if(currentGear == m_vehicleModule.GetGearReverseString())
            {
                inReverseGear = true;
                // Tell gear monitor that Reverse has been observed
                m_vehicleModule.SetObservedGearReverse(true);
            }
            // Check the results of the test
            testResult = (inReverseGear == true ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("ReverseGearTimeout"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("ReverseGearTimeout"));
            Log(LOG_DEV_DATA, "ReadReverseGear: %s\n", testResult.c_str());
        }
        else if(moduleStatus != BEP_STATUS_SUCCESS)
        { // Determine the results
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommuncationFailure");
            Log(LOG_DEV_DATA, "ReadReverseGear: %s\n", testResult.c_str());
        }
    }
    catch(ModuleException &e)
    {   // Module exception
        Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    // Send the test Results to the test result server
    // Invalid format - reverse gear check ran in loop, handle in calling function
    // SendSubtestResult(testResult,testDescription,testResultCode);

    return moduleStatus;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckParkGearResults(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the Partk gear has already been observed
    if(!m_vehicleModule.GearParkObserved())
    {   // Wait for zero speed
        if(CheckZeroSpeed())
        {
            // Check if Park was observed
            do
            {   // Determine if the operator should be prompted
                if(!m_vehicleModule.GearParkObserved() && !promptDisplayed)
                {   // Prompt operator to shift to Park
                    DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                    promptDisplayed = true;
                }
                else if(m_vehicleModule.GearParkObserved() && promptDisplayed)
                {   // Remove the prompts
                    RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                    promptDisplayed = false;
                }
                else if(!m_vehicleModule.GearParkObserved())
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Keep looking until park is detected or timeout
            } while(!m_vehicleModule.GearParkObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Check if the prompt needs to be removed
            if(promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
            }
        }
    }
    // Check if the result has already been updated
    if(testPass != GetParkGearUpdateResult())
    {
        SetParkGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("ParkStatusRegister").c_str()),
                                                    GetParameterInt("ParkBit"), m_vehicleModule.GearParkObserved() ? testPass : testFail));
    }
    // Check the results of the test
    testResult = (m_vehicleModule.GearParkObserved() && (testPass == GetParkGearUpdateResult())) ? testPass : testFail;
    // Report the result of the gear observation
    SendSubtestResult("ParkGearObserved", m_vehicleModule.GearParkObserved() ? testPass : testTimeout,
                      m_vehicleModule.GearParkObserved() ? "Park Gear Observed" : GetFaultDescription("ParkTimeout"),
                      m_vehicleModule.GearParkObserved() ? "0000" : GetFaultCode("ParkTimeout"));
    // Report the result of the gear update
    SendSubtestResult("ParkGearUpdate", GetParkGearUpdateResult(),
                      testPass == GetParkGearUpdateResult() ? "Update park gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == GetParkGearUpdateResult() ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Park: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckReverseGearResults(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the Reverse gear has already been observed
      if(!m_vehicleModule.GearReverseObserved())
      {   // Verify vehicle is at zero speed before prompting to reverse
          if(CheckZeroSpeed())
          {
              // Wait for Reverse gear to be observed
              do
              {   // Determine if the operator should be prompted
                  if(!m_vehicleModule.GearReverseObserved() && !promptDisplayed)
                  {   // Prompt operator to shift to reverse when vehicle is at 0 speed
                      DisplayPrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
                      promptDisplayed = true;
                  }
                  else if(m_vehicleModule.GearReverseObserved() && promptDisplayed)
                  {   // Remove the prompts
                      RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
                      promptDisplayed = false;
                  }
                  else if(!m_vehicleModule.GearReverseObserved())
                  {
                      BposSleep(GetTestStepInfoInt("ScanDelay"));
                  }
                  // Keep looking until reverse is detected or timeout
              } while(!m_vehicleModule.GearReverseObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
              // Check if the prompt needs to be removed
              if(promptDisplayed)
              {   // Remove the prompt from the display
                  RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
              }
          }
      }
    // Determine if the module has already been updated
    if(testPass != GetReverseGearUpdateResult())
    {   // Update the module with the result
        SetReverseGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("ReverseGearStatusRegister").c_str()),
                                                       GetParameterInt("ReverseGearBit"),
                                                       m_vehicleModule.GearReverseObserved() ? testPass : testFail));
    }
    // Check the results of the test
    testResult = (m_vehicleModule.GearReverseObserved() && (testPass == GetReverseGearUpdateResult()) ? testPass : testFail);
    // Report the result of the gear observation
    SendSubtestResult("ReverseGearObserved", m_vehicleModule.GearReverseObserved() ? testPass : testTimeout,
                      m_vehicleModule.GearReverseObserved() ? "Reverse Gear Observed" : GetFaultDescription("ReverseGearTimeout"),
                      m_vehicleModule.GearReverseObserved() ? "0000" : GetFaultCode("ReverseGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("ReverseGearUpdate", GetReverseGearUpdateResult(),
                      testPass == GetReverseGearUpdateResult() ? "Update reverse gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == GetReverseGearUpdateResult() ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Reverse Gear: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckNeutralGearResults(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Determine if Neutral gear was already observed
    if(!m_vehicleModule.GearNeutralObserved())
    {   // Wait for Neutral gear to be observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.GearNeutralObserved() && !promptDisplayed)
            {   // Prompt operator to shift to Neutral
                DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.GearNeutralObserved() && promptDisplayed)
            {   // Remove the prompts
                RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
                promptDisplayed = true;
            }
            else if(!m_vehicleModule.GearNeutralObserved())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until neutral is detected or timeout
        } while(!m_vehicleModule.GearNeutralObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if the prompt needs to be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        }
    }
    // Determine if we need to update the result in the module
    if(testPass != GetNeutralGearUpdateResult())
    {   // Update the module with the result
        SetNeutralGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("NeutralGearStatusRegister").c_str()),
                                                       GetParameterInt("NeutralGearBit"),
                                                       m_vehicleModule.GearNeutralObserved() ? testPass : testFail));
    }
    // Check the results of the test
    testResult = (m_vehicleModule.GearNeutralObserved() && (testPass == GetNeutralGearUpdateResult()) ? testPass : testFail);
    // Report the result of the gear observation
    SendSubtestResult("NeutralGearObserved", m_vehicleModule.GearNeutralObserved() ? testPass : testTimeout,
                      m_vehicleModule.GearNeutralObserved() ? "Neutral Gear Observed" : GetFaultDescription("NeutralGearTimeout"),
                      m_vehicleModule.GearNeutralObserved() ? "0000" : GetFaultCode("NeutralGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("NeutralGearUpdate", GetNeutralGearUpdateResult(),
                      testPass == GetNeutralGearUpdateResult() ? "Update neutral gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == GetNeutralGearUpdateResult() ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Neutral Gear: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckForwardDrivingGearResults(string &firstGearResult, string &secondGearResult,
                                                                                        string &thirdGearResult, string &fourthGearResult,
                                                                                        string &fifthGearResult, string &sixthGearResult)
{
    string testResult = BEP_TESTING_STATUS;
    string testDescription = "Checking forward driving gears";
    string testResultCode = "0000";
    // Set the timer, each gear check monitors the timer
    SetStartTime();
    // Check lowest to highest, driver can just accelerate up to highest gear if required
    firstGearResult  = CheckFirstGearResult();
    secondGearResult = CheckSecondGearResult();
    thirdGearResult  = CheckThirdGearResult();
    fourthGearResult = CheckFourthGearResult();
    fifthGearResult  = CheckFifthGearResult();
    sixthGearResult  = CheckSixthGearResult();
    // Determine the results
    testResult = (((testPass == firstGearResult) && (testPass == secondGearResult) &&
                   (testPass == thirdGearResult) && (testPass == fourthGearResult) &&
                   ((testPass == fifthGearResult) || (testSkip == fifthGearResult)) &&
                   ((testPass == sixthGearResult) || (testSkip == sixthGearResult))) ? testPass : testFail);
    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("AllGearsNotSeen"));
    testDescription = (testPass == testResult ? "Checking forward driving gears" : GetFaultDescription("AllGearsNotSeen"));
    // Log the data
    Log(LOG_DEV_DATA, "Check forward driving gears: %s\n\tFirst Gear: %s\n\tSecond Gear: %s\n\t"
        "Third Gear: %s\n\tFourth Gear: %s\n\tFifth Gear: %s\n\tSixth Gear: %s\n\t",
        testResult.c_str(), firstGearResult.c_str(), secondGearResult.c_str(), thirdGearResult.c_str(),
        fourthGearResult.c_str(), fifthGearResult.c_str(), sixthGearResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckFirstGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Determine if first gear was observed
    if(!m_vehicleModule.Gear1Observed())
    {   // Wait for first gear to be observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.Gear1Observed() && !promptDisplayed)
            {   // Prompt operator to speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("FirstGearSpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.Gear1Observed() && promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.Gear1Observed())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until timeout or gear seen
        } while(TimeRemaining() && !m_vehicleModule.Gear1Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine if the prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed range
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Check if the result needs to be updated
//  if(testPass != GetFirstGearUpdateResult())
//  {   // Update the result in the module
//      SetFirstGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FirstGearStatusRegister").c_str()),
//                                                   GetParameterInt("FirstGearBit"),
//                                                   m_vehicleModule.Gear1Observed() ? testPass : testFail));
//  }
    // Determine the test results
    testResult = (m_vehicleModule.Gear1Observed()) ? testPass : testFail;
    // Report the result of the gear observation
    SendSubtestResult("FirstGearObserved", m_vehicleModule.Gear1Observed() ? testPass : testTimeout,
                      m_vehicleModule.Gear1Observed() ? "First Gear Observed" : GetFaultDescription("FirstGearTimeout"),
                      m_vehicleModule.Gear1Observed() ? "0000" : GetFaultCode("FirstGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("FirstGearUpdate", testResult,
                      testPass == testResult ? "Update first gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "First gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckSecondGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Determine if we need to prompt the operator for the gear
    if(!m_vehicleModule.Gear2Observed())
    {   // Determine if second gear was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.Gear2Observed() && !promptDisplayed)
            {   // Prompt operator to speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SecondGearSpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.Gear2Observed() && promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
                // Update the module with the result
            }
            else if(!m_vehicleModule.Gear2Observed())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until timeout or gear seen
        } while(TimeRemaining() && !m_vehicleModule.Gear2Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine if the prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed range
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Determine if the result should be updated in the module
//  if(testPass != GetSecondGearUpdateResult())
//  {   // Update the result in the module
//      SetSecondGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("SecondGearStatusRegister").c_str()),
//                                                    GetParameterInt("SecondGearBit"),
//                                                    m_vehicleModule.Gear2Observed() ? testPass : testFail));
//  }
    // Determine the test results
    testResult = (m_vehicleModule.Gear2Observed()) ? testPass : testFail;
    // Report the result of the gear observation
    SendSubtestResult("SecondGearObserved", m_vehicleModule.Gear2Observed() ? testPass : testTimeout,
                      m_vehicleModule.Gear2Observed() ? "Second Gear Observed" : GetFaultDescription("SecondGearTimeout"),
                      m_vehicleModule.Gear2Observed() ? "0000" : GetFaultCode("SecondGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("SecondGearUpdate", testResult,
                      testPass == testResult ? "Update second gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Second gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckThirdGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Determine if we need to prompt the operator to achieve the gear
    if(!m_vehicleModule.Gear3Observed())
    {   // Determine if third gear was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.Gear3Observed() && !promptDisplayed)
            {   // Prompt operator to speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("ThirdGearSpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.Gear3Observed() && promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.Gear3Observed())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until timeout or gear seen
        } while(TimeRemaining() && !m_vehicleModule.Gear3Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine if the prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed range
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Determine if the result needs to be updated in the module
//  if(testPass != GetThirdGearUpdateResult())
//  {   // Update the module with the result
//      SetThirdGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("ThirdGearStatusRegister").c_str()),
//                                                   GetParameterInt("ThirdGearBit"),
//                                                   m_vehicleModule.Gear3Observed() ? testPass : testFail));
//  }
    // Determine the test results
    testResult = (m_vehicleModule.Gear3Observed() && (testPass == GetThirdGearUpdateResult())) ? testPass : testFail;
    // Report the result of the gear observation
    SendSubtestResult("ThirdGearObserved", m_vehicleModule.Gear3Observed() ? testPass : testTimeout,
                      m_vehicleModule.Gear3Observed() ? "Third Gear Observed" : GetFaultDescription("ThirdGearTimeout"),
                      m_vehicleModule.Gear3Observed() ? "0000" : GetFaultCode("ThirdGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("ThirdGearUpdate", testResult,
                      testPass == testResult ? "Update third gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Third gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckFourthGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Determine if we need to prompt the operator to achieve the gear
    if(!m_vehicleModule.Gear4Observed())
    {   // Determine if fourth gear was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.Gear4Observed() && !promptDisplayed)
            {   // Prompt operator to speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("FourthGearSpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.Gear4Observed() && promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.Gear4Observed())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until timeout or gear seen
        } while(TimeRemaining() && !m_vehicleModule.Gear4Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine if the prompt should be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed range
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Determine if the module needs to be updated
//  if(testPass != GetFourthGearUpdateResult())
//  {   // Update the module with the result
//      SetFourthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FourthGearStatusRegister").c_str()),
//                                                    GetParameterInt("FourthGearBit"),
//                                                    m_vehicleModule.Gear4Observed() ? testPass : testFail));
//  }
    // Determine the test results
    testResult = (m_vehicleModule.Gear4Observed() && (testPass == GetFourthGearUpdateResult())) ? testPass : testFail;
    // Report the result of the gear observation
    SendSubtestResult("FourthGearObserved", m_vehicleModule.Gear4Observed() ? testPass : testTimeout,
                      m_vehicleModule.Gear4Observed() ? "Fourth Gear Observed" : GetFaultDescription("FourthGearTimeout"),
                      m_vehicleModule.Gear4Observed() ? "0000" : GetFaultCode("FourthGearTimeout"));
    // Report the result of the gear update
    SendSubtestResult("FourthGearUpdate", testResult,
                      testPass == testResult ? "Update fourth gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                      testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Fourth gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckFifthGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Check if the transmission has a fifth gear
    if((GetParameter("OverdriveGear") == m_vehicleModule.GetGear5String()) ||
       (GetParameter("OverdriveGear") == m_vehicleModule.GetGear6String()))
    {   // Determine if we need to prompt the operator to achieve the gear
        if(!m_vehicleModule.Gear5Observed())
        {   // Determine if fifth gear was observed
            do
            {   // Determine if the operator should be prompted
                if(!m_vehicleModule.Gear5Observed() && !promptDisplayed)
                {   // Prompt operator to speed
                    DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                    DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                    // Set the speed range
                    SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("FifthGearSpeedRange"));
                    promptDisplayed = true;
                }
                else if(m_vehicleModule.Gear5Observed() && promptDisplayed)
                {   // Remove the prompt from the display
                    RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                    RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                    // Remove the speed range
                    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                    promptDisplayed = false;
                }
                else if(!m_vehicleModule.Gear5Observed())
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Keep looking until timeout or gear seen
            } while(TimeRemaining() && !m_vehicleModule.Gear5Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Determine if the prompt should be removed
            if(promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
        }
        // Determine if the module needs to be updated
//      if(testPass != GetFifthGearUpdateResult())
//      {   // Update the module with the result
//          SetFifthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("FifthGearStatusRegister").c_str()),
//                                                       GetParameterInt("FifthGearBit"),
//                                                       m_vehicleModule.Gear5Observed() ? testPass : testFail));
//      }
        // Determine the test results
        testResult = (m_vehicleModule.Gear5Observed() && (testPass == GetFifthGearUpdateResult())) ? testPass : testFail;
        // Report the result of the gear observation
        SendSubtestResult("FifthGearObserved", m_vehicleModule.Gear5Observed() ? testPass : testTimeout,
                          m_vehicleModule.Gear5Observed() ? "Fifth Gear Observed" : GetFaultDescription("FifthGearTimeout"),
                          m_vehicleModule.Gear5Observed() ? "0000" : GetFaultCode("FifthGearTimeout"));
        // Report the result of the gear update
        SendSubtestResult("FifthGearUpdate", testResult,
                          testPass == testResult ? "Update fifth gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                          testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    }
    else
    {   // Transmission not equipped with 5 gears
        Log(LOG_DEV_DATA, "Transmission not equipped with 5 or more gears\n");
        testResult = testSkip;
    }
    // Log the results
    Log(LOG_DEV_DATA, "Fifth gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckSixthGearResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Look for sixth gear";
    bool promptDisplayed = false;
    // Check if the transmission has a sixth gear
    if(GetParameter("OverdriveGear") == m_vehicleModule.GetGear6String())
    {   // Determine if the operator should be prompted to achieve the required gear
        if(!m_vehicleModule.Gear6Observed())
        {   // Determine if sixth gear was observed
            do
            {   // Determine if the operator should be prompted
                if(!m_vehicleModule.Gear6Observed() && !promptDisplayed)
                {   // Prompt operator to speed
                    DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                    DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                    // Set the speed range
                    SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SixthGearSpeedRange"));
                    promptDisplayed = true;
                }
                else if(m_vehicleModule.Gear6Observed() && promptDisplayed)
                {   // Remove the prompt from the display
                    RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                    RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                    // Remove the speed range
                    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                    promptDisplayed = false;
                }
                else if(!m_vehicleModule.Gear6Observed())
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Keep looking until timeout or gear seen
            } while(TimeRemaining() && !m_vehicleModule.Gear6Observed() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Determine if the prompt should be removed
            if(promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed range
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
        }
        // Check if the module should be updated
//      if(testPass != GetSixthGearUpdateResult())
//      {   // Update the module with the result
//          SetSixthGearUpdateResult(UpdateEOLStatusByte(atoh(GetParameter("SixthGearStatusRegister").c_str()),
//                                                       GetParameterInt("SixthGearBit"),
//                                                       m_vehicleModule.Gear6Observed() ? testPass : testFail));
//      }
        // Determine the test results
        testResult = (m_vehicleModule.Gear6Observed() && (testPass == GetSixthGearUpdateResult())) ? testPass : testFail;
        // Report the result of the gear observation
        SendSubtestResult("SixthGearObserved", m_vehicleModule.Gear6Observed() ? testPass : testTimeout,
                          m_vehicleModule.Gear6Observed() ? "Sixth Gear Observed" : GetFaultDescription("SixthGearTimeout"),
                          m_vehicleModule.Gear6Observed() ? "0000" : GetFaultCode("SixthGearTimeout"));
        // Report the result of the gear update
        SendSubtestResult("SixthGearUpdate", testResult,
                          testPass == testResult ? "Update sixth gear result in module" : GetFaultDescription("StatusByteUpdateFailure"),
                          testPass == testResult ? "0000" : GetFaultCode("StatusByteUpdateFailure"));
    }
    else
    {   // Transmission not equipped with 6 gears
        Log(LOG_DEV_DATA, "Transmission not equipped with 6 gears\n");
        testResult = testSkip;
    }
    // Log the results
    Log(LOG_DEV_DATA, "Sixth gear observed: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckTCCNoLockState(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Determine if the operator should be prompted to achieve no lock state
    if(!m_vehicleModule.TCCNoLockupObserved())
    {   // Check if the TCC reported the no-lock state
        do
        {   // Determine if prompts need to be displayed
            if(!m_vehicleModule.TCCNoLockupObserved() && !promptDisplayed)
            {   // Prompt the operator to stop the vehicle
                DisplayPrompt(GetPromptBox("BrakeToStop"), GetPrompt("BrakeToStop"), GetPromptPriority("BrakeToStop"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.TCCNoLockupObserved() && promptDisplayed)
            {   // Saw no-lock state, remove the prompt
                RemovePrompt(GetPromptBox("BrakeToStop"), GetPrompt("BrakeToStop"), GetPromptPriority("BrakeToStop"));
                promptDisplayed = false;
            }
            else
            {   // Wait before next check
                BposSleep(250);
            }
            // Keep checking until timeout, status not good or see no-lock state
        } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !m_vehicleModule.TCCNoLockupObserved());
        // Remove prompt if neccessary
        if(promptDisplayed)
        {
            RemovePrompt(GetPromptBox("BrakeToStop"), GetPrompt("BrakeToStop"), GetPromptPriority("BrakeToStop"));
            promptDisplayed = false;
        }
    }
    // Determine the result of the check
    testResult = m_vehicleModule.TCCNoLockupObserved() ? testPass : testFail;
    // Check if the result should be updated in the module
    if(GetParameterBool("UpdateTCCStatesInModule") && !testResult.compare(testPass))
    {   // Update the status bytes in the module
        testResult = UpdateEOLStatusByte(atoh(GetParameter("TCCNoLockStatusRegister").c_str()),
                                         GetParameterInt("TCCNoLockTestBit"), testResult);
    }
    // Report the result of the TCC observation
    SendSubtestResult("TCCNoLockupObserved", m_vehicleModule.TCCNoLockupObserved() ? testPass : testTimeout,
                      m_vehicleModule.TCCNoLockupObserved() ? "TCC No-Lock Observed" : GetFaultDescription("TCClutchNoLockTimeout"),
                      m_vehicleModule.TCCNoLockupObserved() ? "0000" : GetFaultCode("TCClutchNoLockTimeout"));
    // Log the results of the test
    Log(LOG_DEV_DATA, "Check Torque Converter Clutch No-Lock State: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckTCCPartialLockState(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Check if the operator shouldbe prompted to achieve the desired lock state
    if(!m_vehicleModule.TCCPartialLockupObserved())
    {   // Check if the TCC reported the partial-lock state
        do
        {   // Determine if prompts need to be displayed
            if(!m_vehicleModule.TCCPartialLockupObserved() && !promptDisplayed)
            {   // Prompt the operator to accelerate to the required speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.TCCPartialLockupObserved() && promptDisplayed)
            {   // Saw no-lock state, remove the prompt
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed target
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
            else
            {   // Wait before next check
                BposSleep(250);
            }
            // Keep checking until timeout, status not good or see partial-lock state
        } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !m_vehicleModule.TCCPartialLockupObserved());
        // Remove prompt if neccessary
        if(promptDisplayed)
        {
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed target
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Determine the result of the check
    testResult = m_vehicleModule.TCCPartialLockupObserved() ? testPass : testFail;
    // Check if the result should be updated in the module
    if(GetParameterBool("UpdateTCCStatesInModule") && !testResult.compare(testPass))
    {   // Update the status bytes in the module
        testResult = UpdateEOLStatusByte(atoh(GetParameter("TCCPartialLockStatusRegister").c_str()),
                                         GetParameterInt("TCCPartialLockTestBit"), testResult);
    }
    // Report the result of the TCC observation
    SendSubtestResult("TCCPartialLockupObserved", m_vehicleModule.TCCPartialLockupObserved() ? testPass : testTimeout,
                      m_vehicleModule.TCCPartialLockupObserved() ? "TCC Partial Lock Observed" : GetFaultDescription("TCCPartialLockTimeout"),
                      m_vehicleModule.TCCPartialLockupObserved() ? "0000" : GetFaultCode("TCCPartialLockTimeout"));
    // Log the results of the test
    Log(LOG_DEV_DATA, "Check Torque Converter Clutch Partial-Lock State: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckTCCFullLockState(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Check for Torque Converter Clutch in Full-Lock state";
    bool promptDisplayed = false;
    // Check if the operator should be prompted to schieve the desired lock state
    if(!m_vehicleModule.TCCFullLockupObserved())
    {   // Check if the TCC reported the full-lock state
        do
        {   // Determine if prompts need to be displayed
            if(!m_vehicleModule.TCCFullLockupObserved() && !promptDisplayed)
            {   // Prompt the operator to accelerate to the required speed
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Set the speed range
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedRange"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.TCCFullLockupObserved() && promptDisplayed)
            {   // Saw no-lock state, remove the prompt
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the speed target
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                promptDisplayed = false;
            }
            else
            {   // Wait before next check
                BposSleep(250);
            }
            // Keep checking until timeout, status not good or see full-lock state
        } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !m_vehicleModule.TCCFullLockupObserved());
        // Remove prompt if neccessary
        if(promptDisplayed)
        {
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
            RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"), GetPrompt("AccelerateToTargetSpeed"), GetPromptPriority("AccelerateToTargetSpeed"));
            // Remove the speed target
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            promptDisplayed = false;
        }
    }
    // Determine the result of the check
    testResult = m_vehicleModule.TCCFullLockupObserved() ? testPass : testFail;
    // Check if the result should be updated in the module
    if(GetParameterBool("UpdateTCCStatesInModule") && !testResult.compare(testPass))
    {   // Update the status bytes in the module
        testResult = UpdateEOLStatusByte(atoh(GetParameter("TCCFullLockStatusRegister").c_str()),
                                         GetParameterInt("TCCFullLockTestBit"), testResult);
    }
    // Report the result of the TCC observation
    SendSubtestResult("TCCFullLockupObserved", m_vehicleModule.TCCFullLockupObserved() ? testPass : testTimeout,
                      m_vehicleModule.TCCFullLockupObserved() ? "TCC Full Lock Observed" : GetFaultDescription("TCCFullLockTimeout"),
                      m_vehicleModule.TCCFullLockupObserved() ? "0000" : GetFaultCode("TCCFullLockTimeout"));
    // Log the results of the test
    Log(LOG_DEV_DATA, "Check Torque Converter Clutch Full-Lock State: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForOverdriveConditions(void)
{   // Make sure the vehicle speed is in range
    string testResult = BEP_TESTING_STATUS;
    string currentGear("Unknown");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Store the start time
    SetStartTime();
    // Have the operator place the vehicle speed in the target range for test step
    testResult = WaitAndPromptForSpeed(GetParameterFloat("OverdriveLockoutMinimumSpeed"),
                                       GetParameterFloat("OverdriveLockoutMaximumSpeed"), GetTestStepInfoInt("ScanDelay"));
    if(testResult == testPass)
    {   // Display Prompt to maintain current speed
        DisplayPrompt(GetPromptBox("MaintainSpeed"),GetPrompt("MaintainSpeed"),GetPromptPriority("MaintainSpeed"));
        // Wait for transmission to be in proper gear
        do
        {   // Check the current transmission gear
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadCurrentGear"), currentGear);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS != moduleStatus)
                Log(LOG_ERRORS, "Error reading current gear - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            else
            {   // Wait until next chack
                BposSleep(250);
            }
            // Keep checking until we timeout, see the desired gear, or comm fail
        } while((BEP_STATUS_SUCCESS == moduleStatus) && (BEP_STATUS_SUCCESS == StatusCheck()) &&
                TimeRemaining() && !m_analyze.CompareData(currentGear, GetParameter("OverdriveGear"), EQUAL));
        // Check the status of the precondition
        if((BEP_STATUS_SUCCESS == moduleStatus) &&
           m_analyze.CompareData(currentGear, GetParameter("OverdriveGear"), EQUAL))
        {   // All conditions correct
            testResult = testPass;
        }
        else if(!m_analyze.CompareData(currentGear, GetParameter("OverdriveGear"), EQUAL))
        {   // Timeout waiting for overdrive gear
            testResult = testTimeout;
            Log(LOG_ERRORS, "Timeout waiting for overdrive gear in the transmission\n");
        }
        else
        {   // Communication errors
            testResult = testHardwareFail;
        }
    }
    else if(testResult == testTimeout)
    {   // Timeout waiting for vehicle speed
        Log(LOG_ERRORS, "Timeout waiting for vehicle to achieve lockout speed - status: %s\n", testResult.c_str());
    }
    else
    {   // Failure in WaitAndPromptForSpeed()
        Log(LOG_ERRORS, "ERROR GenericTransmissionTCTemplate::WaitForOverdriveConditions - Failure in WaitAndPromptForSpeed, status = %s\n",
            testResult.c_str());
    }
    Log(LOG_DEV_DATA, "Wait for Overdrive lockout Conditions: %s\n\tVehicle Speed: %5.2f\n\tMinimum Speed: %5.2f\n\tMaximum Speed: %5.2f\n",
        testResult.c_str(), GetRollSpeed(), GetParameterFloat("OverdriveLockoutMinimumSpeed"),
        GetParameterFloat("OverdriveLockoutMaximumSpeed"));
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForOverdriveCancelButton(void)
{
    string testResult = BEP_TESTING_STATUS;
    string odCancelButtonStatus = "Unknown";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Set the timer
    SetStartTime();
    // Display Prompt to press Overdrive Cancel Button
    DisplayPrompt(GetPromptBox("CancelOverdrive"),GetPrompt("CancelOverdrive"),GetPromptPriority("CancelOverdrive"));
    // Wait for the button to be pressed
    do
    {   // Read the button status from the module
        BposSleep(250);   // Wait before the next check
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadODCancelButton"), odCancelButtonStatus);
        // Keep looking until timeout, switch is on, comm failure
    } while((BEP_STATUS_SUCCESS == moduleStatus) && (BEP_STATUS_SUCCESS == StatusCheck()) &&
            TimeRemaining() && !m_analyze.CompareData(odCancelButtonStatus, switchOn, EQUAL));
    // Check the results
    if(m_analyze.CompareData(odCancelButtonStatus, switchOn, EQUAL))
    {   // Button was pressed
        Log(LOG_DEV_DATA, "Saw Overdrive Cancel Button On\n");
        testResult = testPass;
    }
    else if(BEP_STATUS_SUCCESS != moduleStatus)
    {   // Communication failure looking for button
        Log(LOG_ERRORS, "Error reading Overdrive Cancel button - status: %s\n",
            ConvertStatusToResponse(moduleStatus).c_str());
        testResult = testFail;
    }
    else
    {   // Timeout waiting for button press
        Log(LOG_ERRORS, "Timeout waiting for Overdrive Cancel Button\n");
        testResult = testTimeout;
    }
    // Remove Prompt
    RemovePrompt(GetPromptBox("CancelOverdrive"),GetPrompt("CancelOverdrive"),GetPromptPriority("CancelOverdrive"));
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForOverdriveGearKickdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string currentGear = "Unknown";
    // Set the timer
    SetStartTime();
    // Wait for the gear to kickdown
    do
    {   // Read the current gear from the module
        BposSleep(250);
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadCurrentGear"), currentGear);
        // Keep checking until gear kickdown, timeout or comm fail
    } while((BEP_STATUS_SUCCESS == moduleStatus) && (BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() &&
            !m_analyze.CompareData(currentGear, GetParameter("OverdriveCancelGear"), EQUAL));
    // Check the results
    if(m_analyze.CompareData(currentGear, GetParameter("OverdriveCancelGear"), EQUAL))
    {   // Gear kickdown success
        Log(LOG_DEV_DATA, "Saw gear kickdown - gear: %s\n", currentGear.c_str());
        testResult = testPass;
    }
    else if(BEP_STATUS_SUCCESS != moduleStatus)
    {   // Communication failure reading current gear
        Log(LOG_ERRORS, "Error readng current gear from the module - status: %s\n",
            ConvertStatusToResponse(moduleStatus).c_str());
        testResult = testFail;
    }
    else
    {   // Timeout waiting for the gear to kickdown
        Log(LOG_ERRORS, "Error waiting for transmission to kickdown:\n\tCurrent Gear: %s\n\tKickdown Gear: %s\n",
            currentGear.c_str(), GetParameter("OverdriveCancelGear").c_str());
        testResult = testTimeout;
    }
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartQuickLearnProcedure(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Prepare vehicle for quick-learn procedure";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::StartQuickLearnProcedure()\n");
    // Transmission must be in Neutral with operator foot on brake and zero speed
    SetStartTime();
    bool inNeutral = false;
    bool zeroSpeed = false;
    string currentLeverPosition = "Unknown";
    // Check the status of zero speed
    if(CheckZeroSpeed())
    {   // At zero speed, check for Neutral
        zeroSpeed = true;
        // Prompt Operator to Shift to Neutral
        DisplayPrompt(GetPromptBox("ShiftToNeutral"),GetPrompt("ShiftToNeutral"),GetPromptPriority("ShiftToNeutral"));
        do
        {   // Read the transmission gear from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadShiftLeverPosition"), currentLeverPosition);
            // Check the status of the read
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, check for neutral
                inNeutral = (m_analyze.CompareData(currentLeverPosition, m_vehicleModule.GetShiftLeverNeutralString(), EQUAL));
            }
            else
            {   // Error reading current gear
                Log(LOG_ERRORS, "Error reading current gear from the module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
            if(!inNeutral)  BposSleep(GetTestStepInfoInt("ScanDelay"));
            // Keep looking until time-out, comm failure, neutral detected or status check fail
        } while((BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus) &&
                TimeRemaining() && !inNeutral);
        // Check the conditions
        if(inNeutral)
        {   // All conditions correct, start the procedure
            moduleStatus = m_vehicleModule.StartQuickLearn();
            // Determine the test results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("StartQuickLearnFail"));
            testDescription = (testPass == testResult ? testDescription : GetFaultDescription("StartQuickLearnFail"));
            Log(LOG_DEV_DATA, "Start QuickLearn Procedure: %s\n", testResult.c_str());
        }
        else
        {   // Error waiting for neutral gear
            if(!inNeutral)
            {   // Timeout waiting for neutral
                testResult = testTimeout;
                testResultCode = GetFaultCode("QuickLearnNeutralTimeout");
                testDescription = GetFaultDescription("QuickLearnNeutralTimeout");
                Log(LOG_ERRORS, "Timeout waiting for Neutral gear\n");
            }
            else if(BEP_STATUS_SUCCESS != moduleStatus)
            {   // Error reading neutral gear from the module
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error reading neutral gear from the module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }
    }
    else
    {   // Timeout waiting for zero speed
        Log(LOG_ERRORS, "Timeout waiting for zero-speed\n");
        testResult = testTimeout;
        testResultCode = GetFaultCode("QuickLearnZeroSpeedTimeout");
        testDescription = GetFaultDescription("QuickLearnZeroSpeedTimeout");
        zeroSpeed = false;
    }
    // Send the test results
    SendSubtestResult("StartQuickLearnProcedure", testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::StartQuickLearnProcedure()\n");
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopQuickLearnProcedure(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Stop quick-learn procedure";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    if(!ShortCircuitTestStep() && IsQuickLearnStarted())
    {
        // Transmission must be in Neutral with operator foot on brake and zero speed
        SetStartTime();
        bool inGear = false;
        string currentLeverPosition = "Unknown";
        // Check the status of zero speed
        if(CheckZeroSpeed())
        {   // At zero speed, check for Gear
            DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
            do
            {   // Read the transmission gear from the module
                moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadShiftLeverPosition"), currentLeverPosition);
                // Check the status of the read
                if(BEP_STATUS_SUCCESS == moduleStatus)
                {   // Good read, check for neutral
                    inGear = (m_analyze.CompareData(currentLeverPosition, m_vehicleModule.GetShiftLeverParkString(), EQUAL));
                }
                else
                {   // Error reading current gear
                    Log(LOG_ERRORS, "%s: Error reading current gear from the module - status: %s\n",
                        GetTestStepName().c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                }
                if(!inGear) BposSleep(GetTestStepInfoInt("ScanDelay"));
                // Keep looking until time-out, comm failure, neutral detected or status check fail
            } while((BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus) &&
                    TimeRemaining() && !inGear);
            // Check the conditions
            if(inGear)
            {   // All conditions correct, start the procedure
                moduleStatus = m_vehicleModule.StopQuickLearn();
                // Determine the test results
                testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
                testResultCode = (testPass == testResult ? "0000" : GetFaultCode("StopQuickLearnFail"));
                testDescription = (testPass == testResult ? testDescription : GetFaultDescription("StopQuickLearnFail"));
                Log(LOG_DEV_DATA, "%s: %s\n", GetTestStepName().c_str(), testResult.c_str());
            }
            else
            {   // Error waiting for test gear
                if(!inGear)
                {   // Timeout waiting for test gear
                    testResult = testTimeout;
                    testResultCode = GetFaultCode("QuickLearnParkTimeout");
                    testDescription = GetFaultDescription("QuickLearnParkTimeout");
                    Log(LOG_ERRORS, "%s: Timeout waiting for Park\n", GetTestStepName().c_str());
                }
                else if(BEP_STATUS_SUCCESS != moduleStatus)
                {   // Error reading gear from the module
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    Log(LOG_ERRORS, "%s: Error reading gear from the module - status: %s\n",
                        GetTestStepName().c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
            RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        }
        else
        {   // Timeout waiting for zero speed
            testResult = testTimeout;
            testResultCode = GetFaultCode("QuickLearnZeroSpeedTimeout");
            testDescription = GetFaultDescription("QuickLearnZeroSpeedTimeout");
            Log(LOG_ERRORS, "%s: Timeout waiting for zero-speed\n", GetTestStepName().c_str());
        }
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Send the test results
    SendSubtestResult("StopQuickLearnProcedure", testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::MonitorQuickLearnProcedure(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Monitor QuickLearn Procedure Status";
    INT32  restartCounter = 0;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string quicklearnStatus = "Unknown";
    bool monitorQuickLearn = false;
    bool displayRpmPrompt = true;           //Only call display once
    INT32 rpmWaitTimer =0;                  //How long to wait for rpm to rise

    rpmWaitTimer = GetParameterInt("QuickLearmRpmTimeOut");
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::MonitorQuickLearnProcedure()\n");
    // Keep monitoring the status until timeout or procedure is complete
    SetStartTime();
    do
    {   // Read the status from the module
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadQuickLearnStatus"), quicklearnStatus);
        // Determine the status of the read
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {   // Good read, m_analyze the data
            if(m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnOilTooCold"), EQUAL) ||
               m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnOilTooHot"), EQUAL) ||
               m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnFaultsPresent"), EQUAL) ||
               m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnRPMTooLow"), EQUAL))
            {   // Problems with the procedure
                testResult = testFail;
                testResultCode = GetFaultCode(quicklearnStatus);
                testDescription = GetFaultDescription(quicklearnStatus);
                Log(LOG_DEV_DATA, "QuickLearn Procedure Error - %s\n", testDescription.c_str());
                // Stop monitoring quicklearn procedure
                monitorQuickLearn = false;
            }
            else if(m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnBrakesNotApplied"), EQUAL) ||
                    m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnThrottleTooHigh"), EQUAL) ||
                    m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnNotInNeutral"), EQUAL) ||
                    m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnNotInOverdrive"), EQUAL))
            {   // Wait for the operator to shift to Park
                Log(LOG_DEV_DATA, "Restarting QuickLearn procedure: %s\n", quicklearnStatus.c_str());
                testResult = QuickLearnWaitForPark();
                // Determine if Park was not achieved
                if(testPass != testResult)
                {   // Stop the quicklearn procedure
                    monitorQuickLearn = false;
                    Log(LOG_DEV_DATA, "QuickLearn error waiting for Park\n");
                    testResultCode = GetFaultCode("QuickLearnWaitForParkTimeout");
                    testDescription = GetFaultDescription("QuickLearnWaitForParkTimeout");
                }
                else
                {   // Restart the quicklearn procedure
                    testResult = StartQuickLearnProcedure();
                    // Determine the result of starting the procedure
                    if(testPass != testResult)
                    {   // Failure restarting the quicklearn procedure
                        monitorQuickLearn = false;
                        Log(LOG_DEV_DATA, "Failure restarting the quicklearn procedure\n");
                        testResultCode = GetFaultCode("QuickLearnRestartFail");
                        testDescription = GetFaultDescription("QuickLearnRestartFail");
                    }
                    else
                    {   // Keep monitoring quicklearn status
                        monitorQuickLearn = GetParameterInt("QuicklearnRetries") > restartCounter++;
                        testResult = monitorQuickLearn ? testResult : testFail;
                        testResultCode = monitorQuickLearn ? testResultCode : GetFaultCode("QuickLearConditions");
                        testDescription = monitorQuickLearn ? testDescription : GetFaultDescription("QuickLearConditions");
                    }
                }
            }
            else if(m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnShiftToDrive"), EQUAL))
            {   // Prompt operator to shift to Drive - start quicklearn
                monitorQuickLearn = true;
                //if required check the RPM at the start of test only.
                if(GetParameterBool("CheckQuickLearnRPM"))
                {   // Reset Secondary timer
                    SetSecondaryStartTime();
                    displayRpmPrompt = true;
                    //do not shift to drive until the RPM is in range or timer has expired
                    while(!IsQuickLearnRpmInRange() && SecondaryTimeRemaining(&rpmWaitTimer))
                    {
                        if(displayRpmPrompt)
                        {   //Information prompt
                            DisplayPrompt(GetPromptBox("WaitForRpmRise"), GetPrompt("WaitForRpmRise"), GetPromptPriority("WaitForRpmRise"));
                            displayRpmPrompt = false;
                        }
                    }
                }
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                Log(LOG_DEV_DATA, "QuickLearn beginning procedure\n");
            }
            else if(m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnRunning"), EQUAL))
            {   // QuickLearn procedure is still running
                monitorQuickLearn = true;
                Log(LOG_DEV_DATA, "QuickLearn procedure running\n");
            }
            else if(m_analyze.CompareData(quicklearnStatus, GetDataTag("QuickLearnComplete"), EQUAL))
            {   // QuickLearn procedure is complete
                monitorQuickLearn = false;
                Log(LOG_DEV_DATA, "QuickLearn procedure is complete\n");
                testResult = testPass;
            }
            else
            {   // Unknown status from the module
                monitorQuickLearn = false;
                testResult = testFail;
                testResultCode = GetFaultCode("QuickLearnUnknownStatus");
                testDescription = GetFaultDescription("QuickLearnUnknownStatus");
                Log(LOG_ERRORS, "Unknown QuickLearn status\n");
            }
            if(monitorQuickLearn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
        }
        else
        {   // Error reading quicklearn status
            Log(LOG_ERRORS, "Error reading quicklearn status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
            // Need to stop monitoring status
            monitorQuickLearn = false;
        }
        // Keep monitoring status until timeout or failure condition
    } while(TimeRemaining() && monitorQuickLearn && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Report the results of the monitoring
    Log(LOG_DEV_DATA, "QuickLearn Status Monitor: %s\n", testResult.c_str());
    SendSubtestResult("QuickLearnStatusMonitor", testResult, testDescription, testResultCode);
    RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::MonitorQuickLearnProcedure()\n");
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::FinishQuickLearnProcedure(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Finish the quicklearn procedure";
    vector<UINT8> clutchVolumes;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::FinishQuickLearnProcedure()\n");
    // Give the module a little extra time to complete the process even though it already said it was done
    BposSleep(GetParameterInt("ModuleQlFinishUpTime"));
    // Wait for the operator to shift to Park
    if(GetParameterBool("QuickLearnReadParkFromModule"))
    {   // Read the park position from the module
        testResult = QuickLearnWaitForPark();
    }
    else
    {   // Just prompt the operator to shift to park
        DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        // Wait for the operator to see the prompt and act
        BposSleep(GetParameterInt("ModuleBusyMaximumTime"));
        // Remove the prompt
        RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        testResult = testPass;
    }
    // Determine the result of waiting for park
    if(testPass == testResult)
    {   // Park position achieved, Read the clutch volumes
        testResult = QuickLearnReadClutchVolumes(GetParameterBool("ReadIndividualVolumes"), clutchVolumes);
        // Check the result of reading the clutch volumes
        if(testPass == testResult)
        {   // Clutch volumes read successfully, report the volumes
            Log(LOG_DEV_DATA, "Transmission Clutch Volumes: \n");
            char temp[8];
            for(UINT32 clutchIndex = 0; clutchIndex < clutchVolumes.size(); clutchIndex++)
            {   // Report the clutch volume
                SendSubtestResultWithDetail("Read"+m_clutchVolumeIndex.getNode(clutchIndex)->getValue(),
                                            testPass, "Clutch Volume", "0000",
                                            m_clutchVolumeIndex.getNode(clutchIndex)->getValue(),
                                            CreateMessage(temp, sizeof(temp), "%02X", clutchVolumes[clutchIndex]), "");
                // Log the data
                Log(LOG_DEV_DATA, "\t%s - %02X\n", m_clutchVolumeIndex.getNode(clutchIndex)->getValue().c_str(),
                    clutchVolumes[clutchIndex]);
            }
        }
        else
        {   // Failed to read the clutch volumes from the module
            Log(LOG_ERRORS, "Failure reading clutch volumes from the mode\n");
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
    }
    else
    {   // Failed to achieve Park
        Log(LOG_ERRORS, "Failed to achieve Park\n");
        testResultCode = GetFaultCode("QuickLearnWaitForParkTimeout");
        testDescription = GetFaultDescription("QuickLearnWaitForParkTimeout");
    }
    // Report the test results
    SendSubtestResult("FinishQuickLearnProcedure", testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::FinishQuickLearnProcedure()\n");
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::QuickLearnWaitForPark(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Wait for tranmission to be in Park";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool promptDisplayed = false;
    bool transInPark = false;
    INT32 readRetries = 0;
    INT32 scanDelay = 100;
    string currentLeverPosition = "Unknown";
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::QuickLearnWaitForPark()\n");
    readRetries = ((INT32)(GetParameterInt("ModuleBusyMaximumTime") / GetParameterInt("QuickLearnScanDelay")));
    if(readRetries < 0)  readRetries = 0;
    scanDelay = GetParameterInt("QuickLearnScanDelay");
    // Wait for the transmission to be in park
    do
    {   // Wait before checking the gear
        BposSleep(scanDelay);
        // Read the transmission gear from the module
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadShiftLeverPosition"), currentLeverPosition);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {   // Good read, evaluate the data
            transInPark = m_analyze.CompareData(currentLeverPosition, m_vehicleModule.GetShiftLeverParkString(), EQUAL);
            // Determine if the operator should be prompted
            if(!transInPark && !promptDisplayed)
            {   // Display the prompt for the operator
                DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                promptDisplayed = true;
            }
            else if(transInPark && promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                promptDisplayed = false;
            }
        }
        else if(readRetries-- > 0)
        {
            Log(LOG_ERRORS, "QuickLearnWaitForPark - Could not read Gear retry\n");
            moduleStatus = BEP_STATUS_SUCCESS;
        }
        else
        {   // Error reading the current gear from the transmission
            Log(LOG_ERRORS, "Error reading the current gear from the transmission - status: %s\n",
                ConvertStatusToResponse(moduleStatus).c_str());
        }
        // Keep looking until comm failure, system failure, park or timeout
    } while((BEP_STATUS_SUCCESS == moduleStatus) && (BEP_STATUS_SUCCESS == StatusCheck()) &&
            TimeRemaining() && !transInPark);
    // Determine if prompt should be removed
    if(promptDisplayed)
    {   // Remove the prompt from the display
        RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        promptDisplayed = false;
    }
    // Check the results
    if(BEP_STATUS_SUCCESS != moduleStatus)
    {   // Error reading current gear from the module
        Log(LOG_ERRORS, "Error reading current gear from the module - status: %s\n",
            ConvertStatusToResponse(moduleStatus).c_str());
        // Display prompt for operator anyway
        DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        testResult = testFail;
        testResultCode = GetFaultCode("CommunicationFailure");
        testDescription = GetFaultDescription("CommunicationFailure");
    }
    else
    {   // Check the results of the wait for park
        testResult = (transInPark ? testPass : testFail);
        testResultCode = (transInPark ? "0000" : GetFaultCode("QuickLearnWaitForParkTimeout"));
        testDescription = (transInPark ? testDescription : GetFaultDescription("QuickLearnWaitForParkTimeout"));
    }
    // Report the results
    SendSubtestResult("QuickLearnWaitForPark", testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::QuickLearnWaitForPark()\n");
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
const bool GenericTransmissionTCTemplate<VehicleModuleType>::IsQuickLearnRpmInRange (void)
{
    bool rpmAtMinimum = false;
    INT32 engineRPM = 0;
    // Attmpet to read the engine RPM from the module
    try
    {   // Read the engine RPM from the module
        BEP_STATUS_TYPE moduleStatus = m_vehicleModule.ReadModuleData(GetDataTag("QuickLearnRpmMessage"), engineRPM);
        // Determine if the engine is running
        rpmAtMinimum = ((BEP_STATUS_SUCCESS == moduleStatus) &&
                        m_analyze.CompareData(engineRPM, GetParameterInt("MinimumQuickLearnRPM"), GREATER_EQUAL));
        Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::IsIdleRPM() - Min = %d, Actual = %d\n",
            GetParameterInt("MinimumQuickLearnRPM"),engineRPM);
    }
    catch(ModuleException &exception)
    {   // Exception reading data
        Log(LOG_ERRORS, "Module exception in IsQuickLearnRpmInRange() - %s\n", exception.message().c_str());
        rpmAtMinimum = false;
    }
    // Return the status
    return rpmAtMinimum;
}


template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::QuickLearnReadClutchVolumes(const bool &readIndividualVolumes, vector<UINT8> &clutchVolumes)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Read transmissio clutch volumes";
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter GenericTransmissionTCTemplate::QuickLearnReadClutchVolumes()\n");
    // Determine which method to use for reading the clutch volumes
    if(readIndividualVolumes)
    {   // Read each clutch volume individually
        Log(LOG_DEV_DATA, "Reading individual clutch volumes\n");
        testResult = ReadIndividualClutchVolumes(clutchVolumes);
    }
    else
    {   // Read clutch volumes by group
        Log(LOG_DEV_DATA, "Reading clutch volumes by group\n");
        testResult = ReadClutchVolumeGroup(clutchVolumes);
    }
    // Determine the test results
    Log(LOG_DEV_DATA, "Read transmission clutch volumes: %s\n", testResult.c_str());
    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("ReadClutchVolumeFail"));
    testDescription = (testPass == testResult ? testDescription : GetFaultDescription("ReadClutchVolumeFail"));
    // Report the results
    SendSubtestResult("QuickLearnReadClutchVolumes", testResult, testDescription, testResultCode);
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit GenericTransmissionTCTemplate::QuickLearnReadClutchVolumes()\n");
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ReadIndividualClutchVolumes(vector<UINT8> &clutchVolumes)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Read individual clutch volumes";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
    UINT8 currentVolume = 0x00;
    // Get the number of clutch volumes that need to be read
    INT32 numberOfClutchVolumes = GetParameterInt("NumberOfClutchVolumes");
    // Read all the clutch volumes from the module
    for(INT32 clutchIndex = 0;
       (clutchIndex < numberOfClutchVolumes) && (BEP_STATUS_SUCCESS == moduleStatus);
       clutchIndex++)
    {   // Read the current clutch volume from the module
        string messageTag = GetDataTag("Read"+m_clutchVolumeIndex.getNode(clutchIndex)->getValue());
        moduleStatus = m_vehicleModule.GetInfo(messageTag, currentVolume);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {   // Good read, stuff the data into the vector
            Log(LOG_DEV_DATA, "%s - %02X\n", messageTag.c_str(), currentVolume);
            clutchVolumes.push_back(currentVolume);
        }
        else
        {   // Error reading the clutch volume
            Log(LOG_ERRORS, "Error reading %s from the module - status: %s\n",
                m_clutchVolumeIndex.getNode(clutchIndex)->getValue().c_str(),
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    // Determine the test results
    testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
    testDescription = (testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure"));
    // Log the results
    Log(LOG_DEV_DATA, "Read clutch volumes: %s\n", testResult.c_str());
    // Report the results
    SendSubtestResult("ReadIndividualClutchVolumes", testResult, testDescription, testResultCode);
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ReadClutchVolumeGroup(vector<UINT8> &clutchVolumes)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = "Read clutch volume groups";
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    vector<UINT8> clutchVolumes1;
    vector<UINT8> clutchVolumes2;
    // Read the first set of clutch volumes from the module
    moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadClutchVolumes1"), clutchVolumes1);
    // Check the status of the read
    if(BEP_STATUS_SUCCESS == moduleStatus)
    {   // Good read, read the second set of volumes
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadClutchVolumes2"), clutchVolumes2);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {   // Good read, stuff all the volumes into one vector
            clutchVolumes.assign(clutchVolumes1.begin(), clutchVolumes1.end());
            clutchVolumes.insert(clutchVolumes.end(), clutchVolumes2.begin(), clutchVolumes2.end());
        }
        else
        {   // Error reading the clutch volumes from the module
            Log(LOG_ERRORS, "Error reading second clutch volumes from the module - status: %s\n",
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    else
    {   // Error reading the clutch volumes from the module
        Log(LOG_ERRORS, "Error reading first clutch volumes from the module - status: %s\n",
            ConvertStatusToResponse(moduleStatus).c_str());
    }
    // Determine the test results
    testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
    testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
    testDescription = (testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure"));
    Log(LOG_DEV_DATA, "Read Clutch volume group: %s\n", testResult.c_str());
    // Return the results
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StartShiftLeverMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Check if we need to skip this step
    if(!ShortCircuitTestStep())
    {   // Need to perform this step
        try
        {   // Start the ShiftLever monitor
            moduleStatus = m_vehicleModule.StartShiftLeverMonitor();
            // Determine the results
            testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail);
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommuncationFailure"));
            Log(LOG_DEV_DATA, "Start ShiftLever monitor: %s\n", testResult.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::StopShiftLeverMonitor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string parkResult = testSkip;
    string reverseResult = testSkip;
    string neutralResult = testSkip;
    string driveResult = testSkip;
    string lowResult = testSkip;
    string low3Result = testSkip;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Check if all required shift lever positions and displays have been observed
            parkResult = CheckParkShiftLeverResult();
            reverseResult = CheckReverseShiftLeverResult();
            neutralResult = CheckNeutralShiftLeverResult();
            driveResult = CheckDriveShiftLeverResult();
            low3Result = CheckLow3ShiftLeverResult();
            lowResult = CheckLowShiftLeverResult();
            // Stop the ShiftLever monitor
            m_vehicleModule.StopShiftLeverMonitor();
            // Check the results of the test
            testResult = (!parkResult.compare(testPass) &&
                          !reverseResult.compare(testPass) &&
                          !neutralResult.compare(testPass) &&
                          !driveResult.compare(testPass) &&
                          (!lowResult.compare(testPass) || !lowResult.compare(testSkip)) &&
                          (!low3Result.compare(testPass) ||
                           !low3Result.compare(testSkip))) ? testPass : testFail;
            testResultCode = (testPass == testResult ? "0000" : GetFaultCode("AllShiftLeverPositionsNotSeen"));
            testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("AllShiftLeverPositionsNotSeen"));
            // Report each ShiftLever individually
            SendSubtestResult("ParkObserved", parkResult, "Park Shift Lever Position observed", GetFaultCode("ParkShiftLeverPositionNotSeen"));
            SendSubtestResult("ReverseObserved", reverseResult, "Reverse Shift Lever Position observed", GetFaultCode("ReverseShiftLeverPositionNotSeen"));
            SendSubtestResult("NeutralObserved", neutralResult, "Neutral Shift Lever Position observed", GetFaultCode("NeutralShiftLeverPositionNotSeen"));
            SendSubtestResult("LowObserved", lowResult, "Low Shift Lever Position observed", GetFaultCode("LowShiftLeverPositionNotSeen"));
            SendSubtestResult("Low3Observed", lowResult, "Low 3 Shift Lever Position observed", GetFaultCode("Low3ShiftLeverPositionNotSeen"));
            SendSubtestResult("DriveObserved", driveResult, "Drive Shift Lever Position observed", GetFaultCode("DriveShiftLeverPositionNotSeen"));
            // Log the data collected
            Log(LOG_DEV_DATA, "Shift Lever Position Results: %s\n\tPark: %s\n\tReverse: %s\n\tNeutral: %s\n\tDrive: %s"
                "\n\tLow: %s\n\t3 Low: %s\n", testResult.c_str(),
                parkResult.c_str(), reverseResult.c_str(), neutralResult.c_str(), driveResult.c_str(),
                lowResult.c_str(), low3Result.c_str());
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Skip the step and just command the lockup monitor to stop
        testResult = testSkip;
        m_vehicleModule.StopShiftLeverMonitor();
        Log(LOG_FN_ENTRY, "Skipping Test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Return the test result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckParkShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the Park shift lever has already been observed
    if(!m_vehicleModule.ShiftLeverParkObserved())
    {   // Wait for zero speed
        CheckZeroSpeed();
        // Check if Park was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.ShiftLeverParkObserved() && !promptDisplayed)
            {   // Prompt operator to shift to Park
                DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.ShiftLeverParkObserved() && promptDisplayed)
            {   // Remove the prompts
                RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.ShiftLeverParkObserved())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until park is detected or timeout
        } while(!m_vehicleModule.ShiftLeverParkObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if the prompt needs to be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        }
    }
    // Check the results of the test
    testResult = (m_vehicleModule.ShiftLeverParkObserved()) ? testPass : testFail;
    // Report the result of the ShiftLever observation
    SendSubtestResult("ParkShiftLeverObserved", m_vehicleModule.ShiftLeverParkObserved() ? testPass : testTimeout,
                      m_vehicleModule.ShiftLeverParkObserved() ? "Park Shift Lever Position Observed" : GetFaultDescription("ParkShiftLeverPositionTimeout"),
                      m_vehicleModule.ShiftLeverParkObserved() ? "0000" : GetFaultCode("ParkShiftLeverPositionTimeout"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Park: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckNeutralShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the neutral shift lever position has already been observed
    if(!m_vehicleModule.ShiftLeverNeutralObserved())
    {   // Wait for zero speed
        CheckZeroSpeed();
        // Check if Neutral was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.ShiftLeverNeutralObserved() && !promptDisplayed)
            {   // Prompt operator to shift to Neutral
                DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.ShiftLeverNeutralObserved() && promptDisplayed)
            {   // Remove the prompts
                RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.ShiftLeverNeutralObserved())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until Neutral is detected or timeout
        } while(!m_vehicleModule.ShiftLeverNeutralObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if the prompt needs to be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        }
    }
    // Check the results of the test
    testResult = (m_vehicleModule.ShiftLeverNeutralObserved()) ? testPass : testFail;
    // Report the result of the ShiftLever observation
    SendSubtestResult("NeutralShiftLeverObserved", m_vehicleModule.ShiftLeverNeutralObserved() ? testPass : testTimeout,
                      m_vehicleModule.ShiftLeverNeutralObserved() ? "Neutral Shift Lever Position Observed" : GetFaultDescription("NeutralShiftLeverPositionTimeout"),
                      m_vehicleModule.ShiftLeverNeutralObserved() ? "0000" : GetFaultCode("NeutralShiftLeverPositionTimeout"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Neutral: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckReverseShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the Reverse Shift Lever Position has already been observed
    if(!m_vehicleModule.ShiftLeverReverseObserved())
    {   // Wait for zero speed
        CheckZeroSpeed();
        // Check if Reverse was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.ShiftLeverReverseObserved() && !promptDisplayed)
            {   // Prompt operator to shift to Reverse
                DisplayPrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.ShiftLeverReverseObserved() && promptDisplayed)
            {   // Remove the prompts
                RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.ShiftLeverReverseObserved())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until Reverse is detected or timeout
        } while(!m_vehicleModule.ShiftLeverReverseObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if the prompt needs to be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
        }
    }
    // Check the results of the test
    testResult = (m_vehicleModule.ShiftLeverReverseObserved()) ? testPass : testFail;
    // Report the result of the ShiftLever observation
    SendSubtestResult("ReverseShiftLeverObserved", (m_vehicleModule.ShiftLeverReverseObserved() ? testPass : testTimeout),
                      (m_vehicleModule.ShiftLeverReverseObserved() ? "Reverse shift lever position Observed" : GetFaultDescription("ReverseShiftLeverPositionTimeout")),
                      (m_vehicleModule.ShiftLeverReverseObserved() ? "0000" : GetFaultCode("ReverseShiftLeverPositionTimeout")));
    // Log the results
    Log(LOG_DEV_DATA, "Check Reverse: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckDriveShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    // Check if the drive shift lever position has already been observed
    if(!m_vehicleModule.ShiftLeverDriveObserved())
    {   // Wait for zero speed
        CheckZeroSpeed();
        // Check if Drive was observed
        do
        {   // Determine if the operator should be prompted
            if(!m_vehicleModule.ShiftLeverDriveObserved() && !promptDisplayed)
            {   // Prompt operator to shift to Drive
                DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                promptDisplayed = true;
            }
            else if(m_vehicleModule.ShiftLeverDriveObserved() && promptDisplayed)
            {   // Remove the prompts
                RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
                promptDisplayed = false;
            }
            else if(!m_vehicleModule.ShiftLeverDriveObserved())
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            // Keep looking until Drive is detected or timeout
        } while(!m_vehicleModule.ShiftLeverDriveObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Check if the prompt needs to be removed
        if(promptDisplayed)
        {   // Remove the prompt from the display
            RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
        }
    }
    // Check the results of the test
    testResult = (m_vehicleModule.ShiftLeverDriveObserved()) ? testPass : testFail;
    // Report the result of the ShiftLever observation
    SendSubtestResult("DriveShiftLeverObserved", m_vehicleModule.ShiftLeverDriveObserved() ? testPass : testTimeout,
                      m_vehicleModule.ShiftLeverDriveObserved() ? "Drive shift lever position Observed" : GetFaultDescription("DriveShiftLeverPositionTimeout"),
                      m_vehicleModule.ShiftLeverDriveObserved() ? "0000" : GetFaultCode("DriveShiftLeverPositionTimeout"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Drive: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckLowShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    if(IsLowShiftLeverEquipped())
    {
        // Check if the Low Shift Lever Position has already been observed
        if(!m_vehicleModule.ShiftLeverLowObserved())
        {   // Wait for zero speed
            CheckZeroSpeed();
            // Check if Low was observed
            do
            {   // Determine if the operator should be prompted
                if(!m_vehicleModule.ShiftLeverLowObserved() && !promptDisplayed)
                {   // Prompt operator to shift to Low
                    DisplayPrompt(GetPromptBox("ShiftToLow"), GetPrompt("ShiftToLow"), GetPromptPriority("ShiftToLow"));
                    promptDisplayed = true;
                }
                else if(m_vehicleModule.ShiftLeverLowObserved() && promptDisplayed)
                {   // Remove the prompts
                    RemovePrompt(GetPromptBox("ShiftToLow"), GetPrompt("ShiftToLow"), GetPromptPriority("ShiftToLow"));
                    promptDisplayed = false;
                }
                else if(!m_vehicleModule.ShiftLeverLowObserved())
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Keep looking until Low is detected or timeout
            } while(!m_vehicleModule.ShiftLeverLowObserved() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Check if the prompt needs to be removed
            if(promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToLow"), GetPrompt("ShiftToLow"), GetPromptPriority("ShiftToLow"));
            }
        }
        // Check the results of the test
        testResult = (m_vehicleModule.ShiftLeverLowObserved()) ? testPass : testFail;
    }
    else
    {   // Current shift lever does not have a low position
        testResult = testSkip;
    }
    // Report the result of the ShiftLever observation
    SendSubtestResult("LowShiftLeverObserved", (testResult == testFail) ? testTimeout : testResult,
                      m_vehicleModule.ShiftLeverLowObserved() ? "Low shift lever position Observed" : GetFaultDescription("LowShiftLeverPositionTimeout"),
                      m_vehicleModule.ShiftLeverLowObserved() ? "0000" : GetFaultCode("LowShiftLeverPositionTimeout"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Low: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckLow3ShiftLeverResult(void)
{
    string testResult = BEP_TESTING_STATUS;
    bool promptDisplayed = false;
    // Start the timer
    SetStartTime();
    if(IsLow3ShiftLeverEquipped())
    {
        // Check if the Low 3 Shift Lever Position has already been observed
        if(!m_vehicleModule.ShiftLeverLow3Observed())
        {   // Wait for zero speed
            CheckZeroSpeed();
            // Check if Low 3 was observed
            do
            {   // Determine if the operator should be prompted
                if(!m_vehicleModule.ShiftLeverLow3Observed() && !promptDisplayed)
                {   // Prompt operator to shift to Low 3
                    DisplayPrompt(GetPromptBox("ShiftToLow3"), GetPrompt("ShiftToLow3"), GetPromptPriority("ShiftToLow3"));
                    promptDisplayed = true;
                }
                else if(m_vehicleModule.ShiftLeverLow3Observed() && promptDisplayed)
                {   // Remove the prompts
                    RemovePrompt(GetPromptBox("ShiftToLow3"), GetPrompt("ShiftToLow3"), GetPromptPriority("ShiftToLow3"));
                    promptDisplayed = false;
                }
                else if(!m_vehicleModule.ShiftLeverLow3Observed())
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Keep looking until Low 3 is detected or timeout
            } while(!m_vehicleModule.ShiftLeverLow3Observed() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Check if the prompt needs to be removed
            if(promptDisplayed)
            {   // Remove the prompt from the display
                RemovePrompt(GetPromptBox("ShiftToLow3"), GetPrompt("ShiftToLow3"), GetPromptPriority("ShiftToLow3"));
            }
        }
        // Check the results of the test
        testResult = (m_vehicleModule.ShiftLeverLow3Observed()) ? testPass : testFail;
    }
    else
    {   // Current shift lever does not have a low 3 position
        testResult = testSkip;
    }
    // Report the result of the ShiftLever observation
    SendSubtestResult("Low3ShiftLeverObserved", (testResult == testFail) ? testTimeout : testResult,
                      m_vehicleModule.ShiftLeverLow3Observed() ? "Low 3 shift lever position Observed" : GetFaultDescription("Low3ShiftLeverPositionTimeout"),
                      m_vehicleModule.ShiftLeverLow3Observed() ? "0000" : GetFaultCode("Low3ShiftLeverPositionTimeout"));
    // Log the results
    Log(LOG_DEV_DATA, "Check Low 3: %s\n", testResult.c_str());
    // Return the result
    return testResult;
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::IsLowShiftLeverEquipped(void)
{   // Method included in GenericTransmissionTCTemplate to allow it to be overloaded
                                       // by derived classes to include logic for vehicles that have a low shift lever position
    return(false);
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::IsLow3ShiftLeverEquipped(void)
{   // Method included in GenericTransmissionTCTemplate to allow it to be overloaded
                                       // by derived classes to include logic for vehicles that have a low 3 shift lever position
    return(false);
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::ReadEOLStatusBytes(void)
{
    UINT16 cnt; // Counter Variable
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    string testResult(testFail);
    string testDescription(GetTestStepInfo("Description"));
    string testResultCode("0000");
    // Log the method entry
    Log(LOG_FN_ENTRY, "GenericTransmissionTCTemplate::ReadEOLStatusBytes - Enter\n");
    try
    {   // Read the EOL status bytes from the module
        m_eolStatusBytes.clear();
        status = m_vehicleModule.GetInfo("ReadEOLStatusBytes", m_eolStatusBytes);
        // Check the status of the read
        if(BEP_STATUS_SUCCESS == status)
        {   // Log the status bytes for the developer
            for(cnt = 0; cnt < m_eolStatusBytes.size(); cnt++)
                Log(LOG_DEV_DATA, "EOL Status Byte %d = 0x%02X\n", cnt+1, m_eolStatusBytes[cnt]);
            testResult = testPass;
        }
        else
        {   // Error reading the data from the module
            Log(LOG_ERRORS, "Error reading EOL Status bytes from the module - status: %s\n",
                ConvertStatusToResponse(status).c_str());
            testResult = ConvertStatusToResponse(status).c_str();
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
    }
    catch(ModuleException &e)
    {   // Module exception
        Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
        testResult = testAbort;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    // Report results to test results server
    SendTestResult(testResult,testDescription,testResultCode);
    // Log the method exit
    Log(LOG_FN_ENTRY, "GenericTransmissionTCTemplate::ReadEOLStatusBytes - Exit\n");
    // Return the methods result
    return(testResult);
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::CheckAutostickBits(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string broadcastTrans = ReadSubscribeData(GetDataTag("Transmission"));
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool isAutoStick = false;
    // Log the function entry
    Log(LOG_FN_ENTRY, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if the test step should be skipped
    if(!ShortCircuitTestStep())
    {   // Perform the test step
        try
        {   // Read the Feature Byte
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("CheckAutostickBits"), isAutoStick);
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {   //Got the Vehicle type; now compare to Broadcast code.
                Log(LOG_DEV_DATA,"Vehicle Transmission Type is AutoStick: %s\n",isAutoStick ? "Yes" : "No ");
                Log(LOG_DEV_DATA,"Broadcast Transmission Type is: %s\n",broadcastTrans.c_str());
                //TODO: If it is AutoStick, see if it matches one of the defined autostick types AND matches Broadcast code.
                m_autostickEquipped = isAutoStick;
            }
            else
            {   // Error reading the Trans type from the module
                Log(LOG_ERRORS, "Error reading Transmission type - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testAbort;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch(ModuleException &e)
        {   // Module exception
            Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
            testResult = testAbort;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Update status in the module
        string updateResult = UpdateEOLStatusByte(atoh(GetParameter("CheckAutostickBitsStatusRegister").c_str()),
                                                  GetParameterInt("CheckAutostickTestBit"), testResult);
        Log(LOG_DEV_DATA, "Check Autostick Bits Status Result Update: %s", updateResult.c_str());

        // Report the test results
        //SendTestResultWithDetail(testResult, testDescription, testResultCode,"ConfirmModuleType",testResult, "");
        SendSubtestResult("CheckAutostickBits", testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());

    return testResult;
}

template <class VehicleModuleType>
const string GenericTransmissionTCTemplate<VehicleModuleType>::WaitForEngineOffIgnitionOff(void)    
{
    string testResult(testFail);
    bool engineOffIgnitionOff = false;
    bool switchOn = false;
    bool promptDisplayed = false;
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOff()  - Enter");
    SetStartTime();
    //Prompt driver to shift to park
    do
    {                                  // Get the brake switch on status
        switchOn = RequestLeverShift(m_vehicleModule.GetShiftLeverParkString(), "ShiftToPark");

        // Determine if the operator should be prompted
        if(!switchOn && !promptDisplayed)
        {                              // Instruct operator to shift to Park
            DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
            promptDisplayed = true;
        }
        else if(switchOn && promptDisplayed)
        {                              // Remove the prompt
            RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
            promptDisplayed = false;
        }
        // If the switch is not on, delay before checking again
        if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
        // Keep checking while good system status, time remaining and switch not on
    } while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
    // Check if prompt must be removed
    if(promptDisplayed)
    {                                  // Remove the prompt
        RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
        promptDisplayed = false;
    }
    do
    {                                  // Display prompt to put Engine off ignition off
        DisplayPrompt(GetPromptBox("EngineOffIgnitionOff"), GetPrompt("EngineOffIgnitionOff"), GetPromptPriority("EngineOffIgnitionOff"));
        if(GenericTCTemplate<VehicleModuleType>::IsIgnitionOn())
        {                              // Wait before the next check
            BposSleep(GetTestStepInfoInt("ScanDelay"));
        }
        else
        {                              // Ignition in correct position
            engineOffIgnitionOff = true;
        }
    } while(TimeRemaining() && !engineOffIgnitionOff && (BEP_STATUS_SUCCESS == StatusCheck()));

    // Remove the prompt
    RemovePrompt(GetPromptBox("EngineOffIgnitionOff"), GetPrompt("EngineOffIgnitionOff"), GetPromptPriority("EngineOffIgnitionOff"));
    // Check the exit conditions
    if(!TimeRemaining() && !engineOffIgnitionOff)
    {                                  // Timeout waiting engine off, ignition off
        testResult = testTimeout;
        Log(LOG_ERRORS, "Timeout waiting for ignition off engine off\n");
    }
    else
    {                                  // Determine the results of the test
        testResult = testPass;
        Log(LOG_ERRORS, "Ignition is Off and engine is off");
        //engine off ignition off delay
        INT32 ignitionoffWaitTime = GetParameterInt("EngineOffIgnitionOffWaitTime");
        BposSleep(ignitionoffWaitTime);
    }
    Log(LOG_FN_ENTRY, "GenericTCTemplate<ModuleType>::WaitForEngineOffIgnitionOff()  - Exit");
    return testResult;
}

template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::AccelerateInReverse(const float &speedMinimum, 
																			 const float &speedMaximum, 
																			 INT32 scanDelay /*= 100*/)
{
    string eraseSpeedRange = "0 0";         // Used to remove the green band
    BEP_STATUS_TYPE speedStatus = BEP_STATUS_FAILURE;   // Used to store return result from WaitForTargetSpeed()
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;  // Used to store return result for module read
    bool inReverseGear = false;     // Used to tell if the current gear is reverse
    string speedRange(BEP_NO_DATA);     // Used to make the green band
    char tempCharArray[16];         // Used to create a speed range for the green band
    string testResult = testFail;       // Used to report test result
    string testDescription = GetTestStepInfo("Description");    // Used to report test description
    string testResultCode = "0000";     // Used to report test result code

    // Log the function entry
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateInReverse() - Enter\n");

    // Determine if this test step should be performed
    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
    }
    else
    {   // Need to perfrom the test step
        try
        {   // Create the speed range to display for the operator
            speedRange = CreateMessage(tempCharArray, sizeof(tempCharArray), "%d %d", (INT32) speedMinimum, (INT32) speedMaximum);
            if(CheckZeroSpeed())
            {   // Display Prompts
                DisplayPrompt(GetPromptBox("ShiftToReverse"),GetPrompt("ShiftToReverse"),
                              GetPromptPriority("ShiftToReverse"));
                DisplayPrompt(GetPromptBox("AccelerateToTargetSpeed"),GetPrompt("AccelerateToTargetSpeed"),
                              GetPromptPriority("AccelerateToTargetSpeed"));
                // update the target green bands
                SystemWrite(GetDataTag("SpeedTarget"), speedRange);
                SetStartTime();
                // Wait for the vehicle to achieve target speed
                bool testComplete = false;
                do
                {   // Read the current gear from the module. Stub always pass for manual.
                    moduleStatus = ReadReverseGear(inReverseGear);
                    // If vehicle is in reverse, wait for the target speed
                    if((moduleStatus == BEP_STATUS_SUCCESS)  &&  (inReverseGear == true))
                    {
                        if(GetParameterInt("AbsoluteValueSpeed"))
                        {
                            speedStatus = WaitForTargetSpeed(speedMinimum, UP, scanDelay);
                        }
                        else
                        {
                            speedStatus = WaitForTargetSpeed(-speedMinimum, DOWN, scanDelay);
                        }
                        
                    }
                    else
                    {
                        BposSleep(scanDelay);
                    }
                    // Determine if exit conditions have been met
                    if(inReverseGear && (BEP_STATUS_SUCCESS == speedStatus)) testComplete = true;
                    // Keep checking while good system status, good comms, not at speed, time remaining and not in reverse
                } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (moduleStatus == BEP_STATUS_SUCCESS) && !testComplete);
                // Set the test step result
                if(StatusCheck() != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - StatusCheck() result - %s\n",
                        ConvertStatusToResponse(StatusCheck()).c_str());
                    testResult = testFail;
                    testDescription = GetFaultDescription("StatusCheckFailure");
                    testResultCode = GetFaultCode("StatusCheckFailure");
                }
                else if(moduleStatus != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Module read result - %s\n",
                        ConvertStatusToResponse(moduleStatus).c_str());
                    testResult = testFail;
                    testDescription = GetFaultDescription("CommunicationFailure");
                    testResultCode = GetFaultCode("CommunicationFailure");
                }
                else if(!TimeRemaining() || (speedStatus != BEP_STATUS_SUCCESS))
                {
                    Log(LOG_ERRORS,"Timeout waiting for Reverse Gear or required speed\n");
                    testResult = testTimeout;
                    testDescription = GetFaultDescription("AccelerateInReverseTimeout");
                    testResultCode = GetFaultCode("AccelerateInReverseTimeout");
                }
                else if(inReverseGear)
                {
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Unexpect unknown Result\n");
                    testResult = testSoftwareFail;
                    testDescription = GetFaultDescription("UnknownFailure");
                    testResultCode = GetFaultCode("UnknownFailure");
                }
                // Remove Prompts
                RemovePrompt(GetPromptBox("ShiftToReverse"),GetPrompt("ShiftToReverse"),GetPromptPriority("ShiftToReverse"));
                RemovePrompt(GetPromptBox("AccelerateToTargetSpeed"),GetPrompt("AccelerateToTargetSpeed"),GetPromptPriority("AccelerateToTargetSpeed"));
                // Remove the target green bands
                SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
            }
            else
            {   // Failed to achieve Zero Speed
                Log(LOG_ERRORS, "ERROR GenericTC::AccelerateInReverse() - Failed to Achieve Zero Speed\n");
                testResult = testTimeout;
                testDescription = GetFaultDescription("AccelerateInReverseZeroSpeedTimeout");
                testResultCode = GetFaultCode("AccelerateInReverseZeroSpeedTimeout");
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "AccelerateInReverse Exception: %s\n", e.what());
            testResult = BEP_STATUS_SOFTWARE;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // Send the test Results to the test result server
        SendTestResult(testResult,testDescription,testResultCode);
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "GenericTC::AccelerateInReverse() exit: %s\n", testResult.c_str());
    // Return the results
    return(testResult);
}

template <class VehicleModuleType>
bool GenericTransmissionTCTemplate<VehicleModuleType>::IsAutostick(void)
{   // Check if vehicle is equipped with autostick.  If not equipped with autostick then
    //   the vehicle must be equipped with a LowShiftLever position
    return(m_autostickEquipped);
}


template <class VehicleModuleType>
string GenericTransmissionTCTemplate<VehicleModuleType>::UpdateEOLStatusByte(const UINT16 &statusRegister, const UINT8 &testBit, const string &testResult)
{
    Log(LOG_ERRORS, "No generic implementation available.\n");
    return(testSoftwareFail);
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetFirstGearUpdateResult(void)
{
    return m_firstGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetSecondGearUpdateResult(void)
{
    return m_secondGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetThirdGearUpdateResult(void)
{
    return m_thirdGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetFourthGearUpdateResult(void)
{
    return m_fourthGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetFifthGearUpdateResult(void)
{
    return m_fifthGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetSixthGearUpdateResult(void)
{
    return m_sixthGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetParkGearUpdateResult(void)
{
    return m_parkGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetReverseGearUpdateResult(void)
{
    return m_reverseGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetNeutralGearUpdateResult(void)
{
    return m_neutralGearUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetUnlockStateUpdateResult(void)
{
    return m_noLockStateUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetPartialLockStateUpdateResult(void)
{
    return m_partialLockStateUpdated;
}

template <class VehicleModuleType>
inline const string& GenericTransmissionTCTemplate<VehicleModuleType>::GetFullLockStateUpdateResult(void)
{
    return m_fullLockStateUpdated;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetFirstGearUpdateResult(const string &result)
{
    m_firstGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetSecondGearUpdateResult(const string &result)
{
    m_secondGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetThirdGearUpdateResult(const string &result)
{
    m_thirdGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetFourthGearUpdateResult(const string &result)
{
    m_fourthGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetFifthGearUpdateResult(const string &result)
{
    m_fifthGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetSixthGearUpdateResult(const string &result)
{
    m_sixthGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetParkGearUpdateResult(const string &result)
{
    m_parkGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetReverseGearUpdateResult(const string &result)
{
    m_reverseGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetNeutralGearUpdateResult(const string &result)
{
    m_neutralGearUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetUnlockStateUpdateResult(const string &result)
{
    m_noLockStateUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetPartialLockStateUpdateResult(const string &result)
{
    m_partialLockStateUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetFullLockStateUpdateResult(const string &result)
{
    m_fullLockStateUpdated = result;
}

template <class VehicleModuleType>
inline void GenericTransmissionTCTemplate<VehicleModuleType>::SetQuickLearnStarted(bool &quickLearnRunning)
{
    m_quickLearnRunning = quickLearnRunning;
}

template <class VehicleModuleType>
inline bool GenericTransmissionTCTemplate<VehicleModuleType>::IsQuickLearnStarted(void)
{
    return(m_quickLearnRunning);
}

