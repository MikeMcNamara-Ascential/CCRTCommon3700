//******************************************************************************
// Description:
//  Generic Emissions Test Component.
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
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/TransferCase/Common/ElectronicTcaseTC.cpp $
// 
// 1     11/02/07 8:56a Derickso
// Removed InitializeHook and WarmInitialize as these functions only
// called the base class.
// 
// 7     10/30/07 2:00p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed some typos in the comments.
// 
// 6     1/17/07 3:43p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// 
// 5     2/15/06 7:25p Cward
// Added New Test steps
//
// 4     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 3     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     9/01/04 9:20p Cward
// Added deletion of tcase tool in destructor. Removed inlines from
// prototypes.
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ElectronicTcaseTC.h"
#include "ModuleException.h"

template <class TcaseModuleType>
ElectronicTcaseTC<TcaseModuleType>::ElectronicTcaseTC() : GenericTCTemplate<TcaseModuleType>()
{
    //make a generic tool tcase component
    m_tcaseTool = new ElectronicTcaseTool<ElectronicTcaseTC>(this);
}

template <class TcaseModuleType>
ElectronicTcaseTC<TcaseModuleType>::~ElectronicTcaseTC()
{
    //delete the generic tool tcase componet
    if (m_tcaseTool != NULL)
    {
        delete m_tcaseTool;
    }
    m_tcaseTool = NULL;
}

template <class TcaseModuleType>
const string ElectronicTcaseTC<TcaseModuleType>::CommandTestStep(const string &value)
{
    string status;
    try
    {  // Get the test step name to perform
        string step = GetTestStepName();
        Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Entering test step %s\n",
            GetComponentName().c_str(), value.c_str(), step.c_str());
        // Check the status to ensure it is alright to run this test step
        if (StatusCheck() != BEP_STATUS_SUCCESS)
        {  // Not OK to run the test step
            UpdateResult(StatusCheck(), status);
            Log(LOG_ERRORS, (GetComponentName() + ".CommandTestStep: StatusCheck() failed: %s\n").c_str(),
                status.c_str());
            status = testSkip;   // Set result to Skip since this step will not be performed
            SendTestResult(status, GetTestStepInfo("Description"));
        }
        // Ensure the ignition is in the on or run position
        else if (GetTestStepName() == "CheckIgnitionOn")     status = CheckIgnitionOn() ? testPass : testAbort;
        // Return switch control to manual
        else if (GetTestStepName() == "ReturnSwitchControl") status = ReturnSwitchControl();
        // Call TcaseTool for addition command steps
        else if (GetTestStepName() == "CheckShiftNeutral")   status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to Low.
        else if (GetTestStepName() == "CheckShiftLow")       status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to AWD.
        else if (GetTestStepName() == "CheckShiftAWD")       status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to 2WD.
        else if (GetTestStepName() == "CheckShift2WD")       status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to 4WD.
        else if (GetTestStepName() == "CheckShift4WD")       status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to 4 LOW.
        else if (GetTestStepName() == "CheckShift4HI")       status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to 4 HI.
        else if (GetTestStepName() == "CheckShift4LOW")      status = m_tcaseTool->CheckTcaseShift();
       // Verify the operator can shift the T-case to 4 Lock.
        else if (GetTestStepName() == "CheckShift4Lock")     status = m_tcaseTool->CheckTcaseShift();
        // Verify the operator can shift the T-case to 4 Part Time.
        else if (GetTestStepName() == "CheckShift4PartTime") status = m_tcaseTool->CheckTcaseShift();
        // Verify Axle Ratio with in limits at desired speed.
        else if (GetTestStepName() == "CheckAxleRatio")      status = m_tcaseTool->CheckAxleRatio();
        // Check the axle balance
        else if (GetTestStepName() == "CheckAxleBalance")    status = m_tcaseTool->CheckAxleBalance();
        // Check the 4Hi axle balance
        else if (GetTestStepName() == "Check4HiAxleBalance") status = m_tcaseTool->CheckAxleBalance();
        // Check the AWD axle balance
        else if (GetTestStepName() == "CheckAWDAxleBalance") status = m_tcaseTool->CheckAxleBalance();
        // Call GenericTCTemplate for addition command steps
        else
            status = GenericTCTemplate<TcaseModuleType>::CommandTestStep(value);
    }
	catch (ModuleException &caughtModuleException)
	{
		Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), caughtModuleException.message().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
    catch (BepException &e)
    {
        Log(LOG_DEV_DATA, "%s CommandTestStep %s Exception: %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n", GetComponentName().c_str(), value.c_str(), status.c_str());
    return(status);
}

template <class TcaseModuleType>
string ElectronicTcaseTC<TcaseModuleType>::ReturnSwitchControl(void)
{
    string testResult = BEP_TESTING_STATUS; //data returned
    string testResultCode("0000");
    string testDescription(BEP_TESTING_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;    //track status of module reads

    //Log enter
    Log(LOG_FN_ENTRY, "%s: ENTER\n", GetTestStepName().c_str());
    //determine if step needs to be ran
    if (ShortCircuitTestStep())
    {   //log skip step
        Log(LOG_DEV_DATA, "%s: Skip Test\n", GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {
        try
        {
            //Send Command to Tcase
            moduleStatus = m_vehicleModule.ReturnSwitchInputControl();
            // Check the status of the operation
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Success reading data from module
                testResult = testPass;
                Log(LOG_DEV_DATA, "%s: Returned Switch Control\n", GetTestStepName().c_str());
            }
            else
            {   // Failure reading data from module
                testResult = testFail;
                testResultCode = GetFaultCode("TcaseCommunicationError");
                testDescription = GetFaultDescription("TcaseCommunicationError");
                Log(LOG_ERRORS, "Error reading data from Tcase module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }
        catch (ModuleException &moduleException)
        {
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
        }
        // Report the results of the in GEAR test
        SendSubtestResult(testResult, testDescription, testResultCode);
    }
    //Log exit
    Log(LOG_FN_ENTRY, "%s: EXIT\n", GetTestStepName().c_str());
    // return the test result
    return(testResult);
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::CheckTcaseShift ( void )
{
    return m_tcaseTool->CheckTcaseShift ();
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::ShiftTransmission (const string transGear)
{
    return m_tcaseTool->ShiftTransmission (transGear );
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::TurnKeyOnOff (const string keyPosition)
{
    return m_tcaseTool->TurnKeyOnOff (keyPosition );
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::EngineOnOff (const string engineRun)
{
    return m_tcaseTool->EngineOnOff (engineRun );
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::CheckThrottlePosition ( const string throttlePosition )
{
    return m_tcaseTool->CheckThrottlePosition(throttlePosition );
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::ShiftTcase ( void )
{
    return m_tcaseTool->ShiftTcase();
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::VerifyTcaseShift ( const string &verifyMethod )
{
    return m_tcaseTool->VerifyTcaseShift(verifyMethod );
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::TcaseCheckDriver ( void )
{
    return m_tcaseTool->TcaseCheckDriver();
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::TcasePromptDriver ( void )
{
    return m_tcaseTool->TcasePromptDriver ();
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::CheckAxleRatio ( void )
{
    return m_tcaseTool->CheckAxleRatio ();
}

template <class TcaseModuleType>
string ElectronicTcaseTC<TcaseModuleType>::CommandShiftTcase (void)
{
    string testResult = BEP_TESTING_STATUS;                         //data returned
    string testResultCode("0000");
    string testDescription(BEP_TESTING_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;                //track status of module reads
    string shiftLeverPosition;                                      //track shift lever postion
    INT32 tcaseShiftDelay = GetTestStepInfoInt("TcaseShiftTime");   //delay between repeated commands
    string tcaseGear("NotRequired");                                //Desired gear status - default Skip

    Log(LOG_FN_ENTRY, "%s: Enter CommandShiftTcase\n", GetTestStepName().c_str());

    try
    {
        //check to see Tcase gear needed
        tcaseGear = GetTestStepInfo("TcaseGear");
        //Prompt Operator
        DisplayPrompt(GetPromptBox("CommandShiftTcase"), GetPrompt("CommandShiftTcase"),
                      GetPromptPriority("CommandShiftTcase"), "White", tcaseGear);
        DisplayPrompt(GetPromptBox("FootOnBrake"), GetPrompt("FootOnBrake"), GetPromptPriority("FootOnBrake"));
        // Send Tcase shift to Gear command
        SetStartTime();
        do
        {   //Send "Command Tcase to desired Gear"
            moduleStatus = m_vehicleModule.CommandTransferCaseShift(tcaseGear);
            // Wait before sending the message again
            delay(GetParameterInt("CommandShiftMessageGap"));
            // Keep checking while time remaining, good system status and good comms
        }while (TimeRemaining(&tcaseShiftDelay) && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == moduleStatus));
        // Check exit condition
        if (!TimeRemaining())
        {   // test failed - timed out waiting for gear
            testResult = testTimeout;
            testResultCode = GetFaultCode("TcaseCommandShiftTimeout");
            testDescription = GetFaultDescription("TcaseCommandShiftTimeout");
            Log(LOG_ERRORS, "%s: FAIL <timeout> waiting for %s.\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else if (BEP_STATUS_SUCCESS != StatusCheck())
        {   //System Fail ready
            testResult = testFatalFail;
            testResultCode = GetFaultCode("SystemFail");
            testDescription = GetFaultDescription("SystemFail");
            Log(LOG_ERRORS, "%s: System  Error durring %s\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else if (moduleStatus != BEP_STATUS_SUCCESS)
        {
            testResult = testFail;
            testResultCode = GetFaultCode("TcaseCommandShiftFail");
            testDescription = GetFaultDescription("TcaseCommandShiftFail");
            Log(LOG_ERRORS, "%s: FAIL durring Command Tcase <%s>.\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else
        {   // test PASS
            testResult = testPass;
            testResultCode = "0000";
            testDescription = GetTestStepInfo("Description");
            Log(LOG_ERRORS, "%s: Saw %s Gear .\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        //Remove prompt
        RemovePrompt(GetPromptBox("CommandShiftTcase"), GetPrompt("CommandShiftTcase"), GetPromptPriority("CommandShiftTcase"));
        RemovePrompt(GetPromptBox("FootOnBrake"), GetPrompt("FootOnBrake"), GetPromptPriority("FootOnBrake"));
    }
    catch (ModuleException &moduleException)
    {
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
        Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
    }

    // Report the results of the in GEAR test
    SendSubtestResult(testResult, testDescription, testResultCode);
    //Log exit
    Log(LOG_FN_ENTRY, "%s: EXIT CommandShiftTcase\n", GetTestStepName().c_str());
    // return the test result
    return(testResult);
}

template <class TcaseModuleType>
string ElectronicTcaseTC<TcaseModuleType>::TcaseCheckGear ( void )
{
    string testResult = BEP_TESTING_STATUS; //data returned
    string testResultCode("0000");
    string testDescription("Verify transfercase is in expected gear");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;    //track status of module reads
    bool inGear = false;                                //command success - default NO
    string tcaseGear = GetTestStepInfo("TcaseGear");    //Get the gear to check for
    string currentGear("NotRequired");                  //Desired gear status - default Skip
    INT32 tcaseDelay = 0;                   //delay between repeated commands

    Log(LOG_FN_ENTRY, "%s: Enter TcaseCheckGear\n", GetTestStepName().c_str());
    try
    {   //check to see what is the delay bewteen command sends
        tcaseDelay = GetTestStepInfoInt("TcaseMessageDelay");
        //Prompt Operator
        DisplayPrompt(GetPromptBox("TcaseCheckGear"), GetPrompt("TcaseCheckGear"), GetPromptPriority("TcaseCheckGear"), "White", tcaseGear);
        do
        {   // Read Tcase Gear
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadTcaseStatus"), currentGear);
            // Check the status of the operation
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Success reading data from module
                inGear = currentGear == tcaseGear;
                // If not in gear, wait before reading again
                if (!inGear)  BposSleep(tcaseDelay);
            }
            else
            {   // Failure reading data from module
                testResult = testFail;
                testResultCode = GetFaultCode("TcaseCommunicationError");
                testDescription = GetFaultDescription("TcaseCommunicationError");
                Log(LOG_ERRORS, "Error reading data from Tcase module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }while ( (StatusCheck() == BEP_STATUS_SUCCESS) && !inGear && TimeRemaining() && (BEP_STATUS_SUCCESS == moduleStatus));
        // Check exit condition
        if (BEP_STATUS_SUCCESS != StatusCheck())
        {   //System Fail ready
            testResult = testFatalFail;
            testResultCode = GetFaultCode("SystemFail");
            testDescription = GetFaultDescription("SystemFail");
            Log(LOG_ERRORS, "%s: System  Error durring %s\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else if (!TimeRemaining() && !inGear)
        {   // test failed - timed out waiting for gear
            testResult = testTimeout;
            testResultCode = GetFaultCode("TcaseCheckGearTimeout");
            testDescription = GetFaultDescription("TcaseCheckGearTimeout");
            Log(LOG_ERRORS, "%s: FAIL <timeout> waiting for %s.\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else if (inGear && (moduleStatus == BEP_STATUS_SUCCESS))
        {   // test PASS
            testResult = testPass;
            testResultCode = "0000";
            testDescription = GetTestStepInfo("Description");
            Log(LOG_ERRORS, "%s: Saw %s TcaseGear .\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("TcaseCheckGearFail");
            testDescription = GetFaultDescription("TcaseCheckGearFail");
            Log(LOG_ERRORS, "%s: FAIL ShiftTransmission to <%s>.\n",GetTestStepName().c_str(), tcaseGear.c_str());
        }
        //prompt is still displayed, so remove it
        RemovePrompt(GetPromptBox("TcaseCheckGear"), GetPrompt("TcaseCheckGear"), GetPromptPriority("TcaseCheckGear"));
    }
    catch (ModuleException &moduleException)
    {
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode(GetDataTag("SoftwareFailure"));
        testDescription = GetFaultDescription(GetDataTag("SoftwareFailure"));
        Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
    }
    // Report the results of the in GEAR test
    SendSubtestResult("CheckShiftTo"+tcaseGear, testResult, testDescription, testResultCode);
    //Log exit
    Log(LOG_FN_ENTRY, "%s: EXIT TcaseCheckGear\n", GetTestStepName().c_str());
    // return the test result
    return(testResult);
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::TcaseCheckSwitch ( void )
{
    string testResult(testFail);
    string testResultCode("0000");
    string testDescription("Verify transfercase is in expected gear");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string currentGear;
    string expectedGear;

    if (GetTestStepName() == "CheckShiftNeutral")        expectedGear = "Neutral";
    else if (GetTestStepName() == "CheckShiftLow")       expectedGear = "Low";
    else if (GetTestStepName() == "CheckShiftAWD")       expectedGear = "AWD";
    else if (GetTestStepName() == "CheckShift2WD")       expectedGear = "2WD";
    else if (GetTestStepName() == "CheckShift4WD")       expectedGear = "4WD";
    else if (GetTestStepName() == "CheckShift4HI")       expectedGear = "4HI";
    else if (GetTestStepName() == "CheckShift4LOW")      expectedGear = "4LOW";
    else if (GetTestStepName() == "CheckShift4Lock")     expectedGear = "4Lock";
    else if (GetTestStepName() == "CheckShift4PartTime") expectedGear = "4PartTime";

    Log(LOG_DEV_DATA, "Enter TcaseCheckSwitch: expectedGear = \"%s\"\n", expectedGear.c_str());

    //prompt shift tcase
    string promptTag = GetTestStepName() + "ShiftTcase";
    DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag), "White", GetTestStepInfo("TcaseGear"));
    //Hold prompt until switch is seen or timeout
    SetStartTime();
    do{
        moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadTransferCaseSwitch"), currentGear);
        // Check the status of the operation
        if (BEP_STATUS_SUCCESS == moduleStatus)
        {   // Success reading data from module
            Log(LOG_DEV_DATA, "ReadTransferCaseSwitch:\n\texpectedGear = \"%s\"\n\tcurrentGear = \"%s\"\n\n", 
                expectedGear.c_str(), currentGear.c_str());
            if(currentGear == expectedGear)
            {
                testResult = testPass;
            }
            else
            {
                BposSleep(250);
            }
        }
        else
        {   // Failure reading data from module
            testResult = testFail;
            testResultCode = GetFaultCode("TcaseCommunicationError");
            testDescription = GetFaultDescription("TcaseCommunicationError");
            Log(LOG_ERRORS, "Error reading data from Tcase module - status: %s\n",
                    ConvertStatusToResponse(moduleStatus).c_str());
        }
    }while ( (StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && testResult != testPass && (BEP_STATUS_SUCCESS == moduleStatus));
    if (!TimeRemaining() && testResult != testPass)
    {   // test failed - timed out waiting for gear
        testResult = testTimeout;
        testResultCode = GetFaultCode("TcaseCheckGearTimeout");
        testDescription = GetFaultDescription("TcaseCheckGearTimeout");
        Log(LOG_ERRORS, "%s: FAIL <timeout> waiting for %s.\n",GetTestStepName().c_str(), expectedGear.c_str());
    }
    //remove prompt
    RemovePrompt(GetPromptBox(GetTestStepName() + "ShiftTcase"), 
                 GetPrompt(GetTestStepName() + "ShiftTcase"), 
                 GetPromptPriority(GetTestStepName() + "ShiftTcase"));
    return(testResult);
}

template <class TcaseModuleType>
inline string ElectronicTcaseTC<TcaseModuleType>::TcaseCheckCluster ( void )
{
    Log(LOG_ERRORS, "ERROR - ElectronicTcaseTC::TcaseCheckCluster, Method Not implemented - Returning Software Failure\n");
    return(testSoftwareFail);
}

