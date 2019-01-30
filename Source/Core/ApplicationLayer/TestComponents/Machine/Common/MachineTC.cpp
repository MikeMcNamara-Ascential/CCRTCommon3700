//*************************************************************************
// Description:
// This file contains the functions necessary to calibrate the rolls
// tester.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Machine/Common/MachineTC.cpp $
// 
// 13    5/01/07 11:20a Rwiersem
// Changes for the 07050201 core release:
// 
// Corrected the logic in DriverNumberNotComplete().
// 
// 12    2/22/07 10:49a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed ResetVehicleTestStatus().
// - Make sure the input server is in DriverNumberMode before reading the
// driver number in GetDriverNumber().
// 
// - Added TestStepMultiDriverQuestion().
// - Added a separator between the methods.
// 
// 11    1/17/07 3:40p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added WarmInitialize().
// - Added the date, time, machine number, and software version to the
// test result in TestStepInitialize().
// - Added a parameter DcxStyleSpeedoTest to run the DCX style speedometer
// test in TestStepSpeedometer().
// - Added a check for YesValue and NoValue in GetKeyPress().
// 
// 10    9/05/06 3:53p Rwiersem
// Changes for the 06090601 core release:
// 
// - Added TestStepReverse()
// 
// 9     4/13/06 3:26a Cward
// Updated SelectDriverSide to allways set the symbolic link to the
// Gryphon box.
//
// 1     2/14/06 1:16p Derickso
// Updated SelectDriverSide to allways set the symbolic link to the
// Gryphon box.
//
// 8     7/20/05 8:38p Cward
// Added call to base class method in Command Test Step
//
// 7     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 6     4/29/05 8:00a GPattison
// Added functionality to allow the Machine Component to Set the
// System Max Boost Pressure to a value specified in the configuration
// file.  If no parameter exists in the configuration file to set the
// pressure, then nothing is done.
//
// 6     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 6     4/07/05 1:01a GPattiso
// Added tests for CheckSpeedometer and AxleRatio for compatiblity with Hmma
// software needs.  Also removed the GetWheelSpeeds method because that was
// promoted to GenericTC
//
// 5     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 4     6/24/04 12:43a Cward
// Removed pound if 0 blocks. UPdated to new standard header comment
// block.
//
// 3     5/25/04 3:52p Cward
// Updated the Hydraulic Pump Motor  function to toggle the bit for the
// pump motor when enablinig the hydraulik pump motor.
//
// 2     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 1     12/21/03 6:33p Cward
//
// 3     11/13/03 7:12p Derickso
// Removed requirement for vehicle present during status check.
//
// 2     9/22/03 5:35p Derickso
// Updated to use GenericTC instead of TestComponent.
//
// 1     8/21/03 9:00a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:54a Khykin
// Updated with new naming convention from TestComponent to TC.
//
// 1     7/08/03 1:04p Derickso
// Converted to new directory structure.
//
// 1     7/03/03 5:27p Khykin
// Initial checkin
//
// 1     6/20/03 9:45a Khykin
// Initial checkin
//
// 1     6/20/03 9:20a Khykin
// Initial checkin.
//
// 3     5/28/03 5:32p Khykin
// Updating to not report results for displaying prompts.
//
// 2     4/30/03 9:07a Khykin
// Added code to ensure that after a brake to stop, the motor mode is
// returned to Boost mode.
//
// 1     4/22/03 10:30a Khykin
// Initial Working Checkin.
//
// 2     4/07/03 9:19a Khykin
// Updating with latest code from Pit machine debug.
//    Revision 1.1.1.1  2003/02/15 00:03:04  tbochene
//
//    PooBag
//
//
// 1     1/27/03 11:13a Khykin
// Initial Checkin
//
// 1     1/18/03 7:22a Khykin
// Initial Checkin
//
// 14    11/18/02 11:58a Khykin
// Updating the finishup return.
//
// 13    11/12/02 8:51a Khykin
// Updated the AccelerateToSpeed TestStep.
//
// 12    11/11/02 3:06p Khykin
// Update the wheel reads to use direct access.
//
// 11    11/04/02 6:36a Khykin
// Updating the component with the capability of resetting the
// communication hardware.
//
// 10    10/22/02 12:55p Khykin
// Updating test result reporting.
//
// 8     10/21/02 11:11a Khykin
// Added an initialize test step.
//
// 7     9/26/02 3:13p Tbochene
// Removed compiler warnings
//
// 6     8/23/02 8:06a Kh
// Synchronizing the pit machine with VSS.
//
// 4     5/24/02 1:48p Kh
// Updating due to addition of system tags and debugging.
//
// 3     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
//
// 2     3/06/02 2:02p Kh
// Updating initialization.
//
// 1     2/06/02 12:19a Kh
// Initial Checkin.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include <math.h>
#include "MachineTC.h"
#include "BepTimer.h"
#include "SerialDefs.h"

//=============================================================================
MachineTC::MachineTC() : GenericTC(), m_semKeyPress(0), m_semDriverNumber(0), m_speedoTestSpeedValue(-999.99)
{
    m_requestKeyPressInput = false;         // initialize keypress info flag
    m_requestDriverNumber = false;          // initialize the driver id flag
}

//=============================================================================
MachineTC::~MachineTC()
{
}

//=============================================================================
void MachineTC::Initialize(const XmlNode *config)
{
    // call the base class initialize
    GenericTC::Initialize(config);

    // Initialize the base component
    Log(LOG_FN_ENTRY, "Initializing The MachineTC Component\n");

    // If the MachineComponent configuration file contains a MaxHydrualicBoostPressure element, then command
    //    the MotorController task to set the system MaxBoostPressure to the value in the config file.
    // NOTE:  Do not use GetParameter or the max pressure will be set to zero if the parameter does not exist!!
    try
    {
        SetMaxHydraulicBoostPressure(config->getChild("Setup/Parameters/MaxHydraulicBoostPressure")->getValue());
    }
    catch(...)
    {
    }

    // Store the machine conditions to be validated
    try
    {
        m_machineConditions.DeepCopy(config->getChild("Setup/Parameters/MachineConditions")->getChildren());
    }
    catch(XmlException &excpt)
    {
        m_machineConditions.clear(true);
    }

    Log(LOG_FN_ENTRY, "Done Initializing MachineTC\n");

    // signal that initialization is complete
    InitializationComplete();
}

//=============================================================================
void MachineTC::WarmInitialize(void)
{
    m_requestKeyPressInput = false;         // initialize keypress info flag
    m_requestDriverNumber = false;          // initialize the driver id flag

    // If the MachineComponent configuration file contains a MaxHydrualicBoostPressure element, then command
    //    the MotorController task to set the system MaxBoostPressure to the value in the config file
    // NOTE:  Do not use GetParameter or the max pressure will be set to zero if the parameter does not exist!!
    try
    {
        SetMaxHydraulicBoostPressure(m_configCopy->getChild("Setup/Parameters/MaxHydraulicBoostPressure")->
                                     getValue());
    }
    catch(...)
    {
    }

    GenericTC::WarmInitialize();
}

//=============================================================================
const std::string MachineTC::Register(void)
{
    std::string status = GenericTC::Register();
    if(status == BEP_SUCCESS_RESPONSE)
    {
        // if the hardware should be initialized upon startup
        if(GetParameter("ResetCommHardware") == "Yes")
        {
            std::string response;
            Log(LOG_DEV_DATA, "Initializing the serial communication hardware\n");
            IBepCommunication commObj;
            commObj.Initialize(SER_SRVR_NAME, "Client", IsDebugOn());

            std::string tag("Reset");
            std::string value("Reset");
            if(commObj.Command(tag, value, response, true))
                status = BEP_FAILURE_RESPONSE;
            Log(LOG_DEV_DATA, "Initialization complete\n");
        }
    }

    return(status);
}

//=============================================================================
const std::string MachineTC::CommandTestStep(const std::string &value)
{
    std::string status; // the status of the commanded test step

    // Determine if the next test step can be sequenced
    if(StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {
            std::string step = GetTestStepName();       // get the name of the sequenced test step

            Log(LOG_FN_ENTRY, "MachineTC::CommandTestStep(%s): %s\n", value.c_str(), step.c_str());

            if(step == "Initialize")                    // if "Initialize" sequenced
                status = TestStepInitialize(value);
            else if(step == "DriverQuestion")           // if "DriverQuestion" sequenced
                status = TestStepDriverQuestion(value);
            else if(step == "PromptDriver")             // else if "PromptDriver" sequenced
                status = TestStepPromptDriver(value);
            else if(step == "PromptDriverAlways")       // else if "PromptDriverAlways" sequenced
                status = TestStepPromptDriverAlways(value);
            else if(step == "DriverNumber")             // else if "DriverNumber" sequenced
                status = TestStepDriverNumber(value);
            else if(step == "AccelerateToSpeed")        // else if "AccelerateToSpeed" sequenced
                status = TestStepAccelerateToSpeed(value);
            else if(step == "BrakeToStop")              // else if "BrakeToStop" sequenced
                status = TestStepBrakeToStop(value);
            else if(step == "QuickStop")                // else if "QuickStop" sequenced
                status = TestStepQuickStop(value);
            else if(step == "ConnectCable")             // else if "ConnectCable" sequenced
                status = TestStepConnectCable(value);
            else if(step == "MachineReady")             // else if "MachineReady" sequenced
                status = TestStepMachineReady(value);
            // Disable the hydraulic pump motor
            else if(step == "DisableHydraulicPumpMotor") status = HydraulicPumpMotor(HYDRAULIC_PUMP_MOTOR_OFF);
            // Enable the hydraulic pump motor
            else if(step == "EnableHydraulicPumpMotor")  status = HydraulicPumpMotor(HYDRAULIC_PUMP_MOTOR_ON);
            // Disengage the machine
            else if(step == "DisengageMachine")          status = DisengageMachine();
            // Engage the machine
            else if(step == "EngageMachine")             status = EngageMachine();
            // Point the comms to the correct location based on which cable was connected
            else if(step == "SelectDriverSide")          status = TestStepSelectDriverSide(value);
            // Finishup sequence
            else if(step=="FinishUp")                   // else if "FinishUp" sequenced
                status = TestStepFinishUp(value);
            else if(step=="AxleRatio")                  // else if "AxleRatio" sequenced
                status = TestStepAxleRatio(value);
            else if(step=="Speedometer")                // else if "Speedometer" sequenced
                status = TestStepSpeedometer(value);
            else if(step=="Reverse")                    // else if "Reverse" sequenced
                status = TestStepReverse(value);
            else if(step == "MultiDriverQuestion")          // if "MultiDriverQuestion" sequenced
                status = TestStepMultiDriverQuestion(value);
            else if(!step.compare("VerifyMachineConditions"))
                status = ValidateMachineConditions(value);
            else if(!step.compare("SetInputServerState"))  status = SetInputServerState(value);
            else if(step == "RaiseRollsLowerElevators")    status = RaiseRollsLowerElevators();
            else if(step == "LowerRollsRaiseElevators")    status = LowerRollsRaiseElevators();
			else if(step == "PerformNvhCycle")             status = PerformNvhCycle(value);
            else                                        // else invalid test step
                status = GenericTC::CommandTestStep(value);

            Log(LOG_FN_ENTRY, "MachineTC::CommandTestStep(%s): %s Returned: %s\n",
                value.c_str(), step.c_str(), status.c_str());
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), e.what());
            status = BEP_ERROR_RESPONSE;
        }
    }
    else
    {   // Status of the system is not right to continue testing
        Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
        status = BEP_SKIP_RESPONSE;
    }

    return(status);
}

//=============================================================================
const std::string MachineTC::TestStepInitialize(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepInitialize()\n");
    std::string status = BEP_NONE;  // set test step status to no response
    string machineNo, softwareVersion;
    char testDate[256], testTime[256];
    struct tm *localTime;
    time_t currentTime;
    INT32 localStatus;

    try
    {
        // clear the prompt box queues when we initialize
        std::string response;
        localStatus = m_prompt->ClearPromptBox(1, response);
        if(localStatus == BEP_STATUS_SUCCESS)
        {
            localStatus = m_prompt->ClearPromptBox(2, response);
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error Clearing Prompts: %s\n", e.what());
        localStatus = BEP_STATUS_FAILURE;
    }

    // update the result
    if(localStatus == BEP_STATUS_SUCCESS) status = BEP_PASS_RESPONSE;
    else if(localStatus == BEP_STATUS_TIMEOUT) status = BEP_TIMEOUT_RESPONSE;
    else status = BEP_FAILURE_RESPONSE;

    // Get the time and machine number for reporting purposes
    currentTime = time(NULL);
    localTime = localtime(&currentTime);
    machineNo = ReadSubscribeData(GetDataTag("MachineNumber"));
    softwareVersion = ReadSubscribeData(GetDataTag("SoftwareVersion"));

    // if the DateFormat parameter doesn't exist, default to %F
    string dateFormat = GetParameter("DateFormat");
    if(dateFormat == "") dateFormat = "%F";

    // if the TimeFormat parameter doesn't exist, default to %H:%M:%S
    string timeFormat = GetParameter("TimeFormat");
    if(timeFormat == "") timeFormat = "%H:%M:%S";

    strftime(testDate, sizeof(testDate), dateFormat.c_str(), localTime);
    strftime(testTime, sizeof(testTime), timeFormat.c_str(), localTime);

    // check to make sure an abort did not happen, if so flag it
    if(CheckAbort())    status = BEP_ABORT_STATUS;

    // update the test results
    if(SendTestResultWithDetail(status, GetTestStepInfo("Description"), "0000",
                                "Date", testDate, "",
                                "Time", testTime, "",
                                "MachineNumber", machineNo, "",
                                "Version", softwareVersion, "") != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    // reset all of the system tags
    ResetSystemTags();

    return(status);             // return the status
}

//=============================================================================
const std::string MachineTC::TestStepDriverQuestion(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::DriverQuestion(): %s\n", value.c_str());
    std::string status = BEP_NONE;  // set test step status to no response
    std::string testResultName;
    // update the current test step base on the value
    SetCurrentTestStep(m_testSteps.getNode(GetTestStepName())->getChild(value));
    testResultName = GetTestStepInfo("TestResultName");

    try
    {   // if the conditions are correct to perform the task
        if((StatusCheck() == BEP_STATUS_SUCCESS) || (GetTestStepName() == "DriverPassFail"))
        {
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // change the pendant state into TestResult mode
            SystemWrite(GetDataTag("InputServerState"), GetDataTag("TestResultMode"));
            INT32 timeout = max(0, GetTestStepInfoInt("Timeout")/1000);   // the timeout time
            Log(LOG_DEV_DATA, "Setting Timeout Time To: %d\n", timeout);
            // get the input from the pendant
            std::string keyValue = GetKeyPress(timeout);
            if(keyValue == GetDataTag("YesValue"))  status = BEP_PASS_STATUS;
            else                                    status = BEP_FAIL_STATUS;
        }
        // else the conditions are not correct, indicate not started
        else
            status = BEP_TEST_NOT_STARTED;

        // check to make sure an abort did not happen, if so flag it
        if(CheckAbort())    status = BEP_ABORT_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC DriverQuestion BepException: %s\n", e.what());
        status = BEP_FAIL_STATUS;
    }

    RemovePrompts();

    // change the InputServer state into Normal mode
    SystemWrite(GetDataTag("InputServerState"), GetDataTag("NormalMode"));

    // update the test results
    if(SendSubtestResult(testResultName, status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    Log(LOG_FN_ENTRY, "TestStepDriverQuestion: %s, Result: %s\n", GetTestStepName().c_str(), status.c_str());

    return(status);             // return the status
}

//=============================================================================
const std::string MachineTC::TestStepPromptDriver(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::PromptDriver()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    // update the current test step base on the value
    SetCurrentTestStep(m_testSteps.getNode(GetTestStepName())->getChild(value));

    try
    {   // if the conditions are correct to perform the task or if should always prompt
        if(GetTestStepResult() != "Pass")
        {
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                // put up the question
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // get the timeout time to determine how long to put the prompt on the screen for
                if((GetTestStepInfo("Timeout") != "") && (GetTestStepInfo("Timeout") != "0"))
                {
                    while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining())
                        BposSleep(100);
                    if(StatusCheck() != BEP_STATUS_SUCCESS)
                        Log(LOG_ERRORS, "Removing Prompt Due to Machine Error\n");
                }
                // if a timeout is not 0 and a value has been entered
                if(GetTestStepInfo("Timeout").compare("0") && !GetTestStepInfo("Timeout").empty())
                {   // remove the question
                    RemovePrompts();
                }
                else
                    Log(LOG_DEV_DATA, "Not Removing Prompt: %s\n", value.c_str());

                status = BEP_PASS_STATUS;
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;
        }
        else
        {
            status = BEP_PASS_STATUS;
            Log(LOG_DEV_DATA, "Previous Pass not prompting\n");
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC PromptDriver::%s Exception: %s\n", GetTestStepName().c_str(), e.what());
        status = BEP_FAIL_STATUS;
    }

    // check to make sure an abort did not happen, if so flag it
    if(CheckAbort())    status = BEP_ABORT_STATUS;

    return(status);             // return the status
}

//=============================================================================
const std::string MachineTC::TestStepPromptDriverAlways(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::PromptDriverAlways()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    // update the current test step base on the value
    SetCurrentTestStep(m_testSteps.getNode(GetTestStepName())->getChild(value));

    try
    {   // if the conditions are correct to perform the task or if should always prompt
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {
            // put up the question
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // get the timeout time to determine how long to put the prompt on the screen for
            if((GetTestStepInfo("Timeout") != "") && (GetTestStepInfo("Timeout") != "0"))
            {
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining())
                    BposSleep(100);
                if(StatusCheck() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Removing Prompt Due to Machine Error\n");
            }
            // if a timeout is not 0 and a value has been entered
            if(GetTestStepInfo("Timeout").compare("0") && !GetTestStepInfo("Timeout").empty())
            {   // remove the question
                RemovePrompts();
            }
            else
                Log(LOG_DEV_DATA, "Not Removing Prompt: %s\n", value.c_str());

            status = BEP_PASS_STATUS;
        }
        // else the conditions are not correct, indicate not started
        else
            status = BEP_TEST_NOT_STARTED;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC PromptDriverAlways::%s Exception: %s\n", GetTestStepName().c_str(), e.what());
        status = BEP_FAIL_STATUS;
    }

    // send the status of prompting the driver
    SendSubtestResult(GetTestStepName(), status, GetTestStepInfo("Description"));
    // check to make sure an abort did not happen, if so flag it
    if(CheckAbort())    status = BEP_ABORT_STATUS;

    return(status);             // return the status
}

//=============================================================================
const std::string MachineTC::TestStepDriverNumber(const std::string &value)
{
    std::string status = BEP_NONE;  // set test step status to no response
    std::string driverNumber;       // the driver number that was entered

    Log(LOG_FN_ENTRY, "Entering DriverNumber(%s)\n", value.c_str());

    // look to determine if a valid driver id has already been entered
    bool driverNumberStatus;
    // if the test must validate the driver id
    if(GetTestStepInfo("Validate") == "Yes")
    {
        driverNumberStatus = ValidateDriverNumber(GetTestStepInfo("DriverNumberFile"), 
                                                  ReadSubscribeData(GetDataTag("DriverNumber")));
    }
    // else if it is to skip the driver id
    else if(GetTestStepInfo("Validate") == "Skip")
    {
        WriteSubscribeData(GetDataTag("DriverNumber"), GetTestStepInfo("DefaultDriverNumber"));
        driverNumberStatus = true;
    }
    // else just read what it is and report it
    else
        driverNumberStatus = !DriverNumberNotComplete(ReadSubscribeData(GetDataTag("DriverNumber")));

    // if a valid driver number has not been entered
    if((!driverNumberStatus) && (StatusCheck() == BEP_STATUS_SUCCESS))
    {   // if not, prompt the driver to input their driver ID
        if(UpdatePrompts() != BEP_STATUS_SUCCESS)
            Log(LOG_ERRORS, "Unable to Update Prompts\n");
        // change the pendant state to DriverNumber mode
        SystemWrite(GetDataTag("InputServerState"), GetDataTag("DriverNumberMode"));
        // wait for a driver number to be entered
        INT32 timeout = max(0, GetTestStepInfoInt("Timeout")/1000);
        Log(LOG_DEV_DATA, "Setting Timeout Time To: %d\n", timeout);

        do
        {
            try
            {   // update the status of the input server to DriverNumber
                if(SystemWrite(GetDataTag("InputServerState"), GetDataTag("ServerStateDriverNumber")) != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Error Updateing The InputServer state to %s\n", 
                        GetDataTag("ServerStateDriverNumber").c_str());
                // read the driver number until it is completed or a timeout occurrs
                Log(LOG_DEV_DATA, "Waiting For Complete Driver Number\n");
                driverNumber = GetDriverNumber(timeout);
                Log(LOG_DEV_DATA, "Driver Number Received: %s\n", driverNumber.c_str());
                // update the status of the input server to Normal
                SystemWrite(GetDataTag("InputServerState"), GetDataTag("ServerStateNormal"));
                // analyze the results
                if(GetTestStepInfo("Validate") == "Yes")
                {
                    Log(LOG_DEV_DATA, "Validating The Driver Number\n");
                    driverNumberStatus = ValidateDriverNumber(GetTestStepInfo("DriverNumberFile"), driverNumber);
                    if(driverNumberStatus)  status = BEP_PASS_RESPONSE;
                    else                    status = BEP_FAIL_RESPONSE;
                }
                else // set the Driver ID Background Color to Green
                {
                    Log(LOG_DEV_DATA, "Not Validating The Driver Number\n");
                    if(driverNumber != "Error") status = BEP_PASS_RESPONSE;
                    else                        status = BEP_FAIL_RESPONSE;
                }
            }
            catch(BepException &e)
            {
                Log(LOG_ERRORS, "MachineTC::DriverNumber Unexpected Exception: %s\n", e.what());
                status = BEP_FAIL_RESPONSE;
            }

            // if a bad driver number, indicate status
            if(status != BEP_PASS_RESPONSE)
                SystemWrite(GetDataTag("DriverNumberBGColor"), GetDataTag("Fail"));

            // if there is not a timeout for this step, wait until a valid driver number is input
        } while((status != BEP_PASS_RESPONSE) && (StatusCheck() == BEP_STATUS_SUCCESS) && !timeout);

        // change the pendant state to Normal mode
        SystemWrite(GetDataTag("InputServerState"), GetDataTag("NormalMode"));

        RemovePrompts();
    }
    // else if we are ready to go
    else if(driverNumberStatus)
    {
        driverNumber = ReadSubscribeData(GetDataTag("DriverNumber"));
        status = BEP_PASS_RESPONSE;
    }
    // else get out
    else if(!CheckAbort())
    {
        driverNumber = "";
        status = BEP_UNTESTED_RESPONSE;
    }

    // check to make sure an abort did not happen, if so flag it
    if(CheckAbort())    status = BEP_ABORT_STATUS;

    // if good driver number, indicate status
    if(status == BEP_PASS_RESPONSE)
        SystemWrite(GetDataTag("DriverNumberBGColor"), GetDataTag("Pass"));

    // update the test results
    if(SendTestResultWithDetail(status, GetTestStepInfo("Description"), "0000", "DriverNumber", driverNumber, "string")
       != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::DriverNumber Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    Log(LOG_FN_ENTRY, "DriverNumber Entry Complete: %s, %s\n", status.c_str(), driverNumber.c_str());

    return(status);
}

//=============================================================================
const std::string MachineTC::TestStepAccelerateToSpeed(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepAccelerateToSpeed(%s)\n", value.c_str());
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the conditions are correct to perform the task
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {   // put up the question
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // Wait for the operator to attain the specified speed limit
            float speed = atof(value.c_str());
            Log(LOG_DEV_DATA, "Converted Commanded Speed To: %f\n", speed);
            char buffer[20];
            memset(buffer, 0, sizeof(buffer));
            status = AccelerateToTestSpeed(speed, 
                                           CreateMessage(buffer, sizeof(buffer), "%.2f %.2f", speed, speed+10.0),
                                           250, false);
        }
        // else the conditions are not correct, indicate not started
        else
        {
            status = BEP_TEST_NOT_STARTED;
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepAccelerateToSpeed Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }
    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepAccelerateToSpeed Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }
    RemovePrompts();    // remove the prompts from the screen
    return(status);     // return the status
}

//=============================================================================
const std::string MachineTC::TestStepBrakeToStop(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepBrakeToStop()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the conditions are correct to perform the task
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {
            bool done = false;  // status of the braking
            //remove any speed bands
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            // check if at zerospeed
            if(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
            {
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // Set the start time
                SetStartTime();
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")   BposSleep(250);
                    else                                                    done = true;
                }
            }
            // else flag done
            else
                done = true;

            // update the status of the test
            if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
            else if(!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
            else if(!done)                          status = BEP_FATALFAIL_STATUS;
            else                                    status = BEP_PASS_STATUS;

            //if successful, ensure that the motor mode is Boost mode
            if(status == BEP_PASS_STATUS)
                SystemWrite(MOTOR_MODE, string(BOOST_MODE));
        }
        // else the conditions are not correct, indicate not started
        else
            status = BEP_TEST_NOT_STARTED;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepBrakeToStop Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepBrakeToStop Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    return(status);     // return the status
}

//=============================================================================
const string MachineTC::TestStepQuickStop (const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepQuickStop()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the conditions are correct to perform the task
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedTarget"));
            if(SystemWrite(MOTOR_MODE, string(QUICK_BRAKE_MODE)) == BEP_STATUS_SUCCESS)
            {
                bool done = false;
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")   BposSleep(250);
                    else                                                    done = true;
                }
                // update the status of the test
                if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if(!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
                else if(!done)                          status = BEP_FATALFAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            else
                status = BEP_FATALFAIL_STATUS;
        }
        // else the conditions are not correct, indicate not started
        else
            status = BEP_TEST_NOT_STARTED;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepQuickStop Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // return the system to Boost Mode
    SystemWrite(MOTOR_MODE, string(BOOST_MODE));

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepBrakeToStop Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "MachineTC::TestStepQuickStop(): %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string MachineTC::TestStepConnectCable (const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepConnectCable()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the cable is not connected
        bool connectStatus = (bool)(ReadSubscribeData(GetDataTag("CableConnect")) == "1");
        Log(LOG_FN_ENTRY, "TestStepConnectCable connect status: %d\n", connectStatus);
        if(!connectStatus)
        {   // if the machine is in the expected state
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                // put up the question
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                bool done = false;
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if(ReadSubscribeData(GetDataTag("CableConnect")) != "1")    BposSleep(250);
                    else
                    {
                        Log(LOG_FN_ENTRY, "TestStepConnectCable Cable Connected\n");
                        done = true;
                    }
                }
                // update the status of the test
                if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if((!TimeRemaining()) || (!done))  status = BEP_FATALFAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;
        }
        else
            status = BEP_PASS_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepConnectCable Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // if the result of this test step is not pass, ABORT
    if(status != BEP_PASS_STATUS)   SystemWrite(ABORT_DATA_TAG, "1");

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepConnectCable Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "MachineTC::TestStepConnectCable(): %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string MachineTC::TestStepMachineReady (const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepMachineReady()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the cable is not connected
        Log(LOG_FN_ENTRY, "TestStepMachineReady status: %d\n", StatusCheck());
        // if the machine is not in a valid status, wait till it is
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {
            // put up prompts if necessary
            if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            do
            {
                BposSleep(100);
                // while the machine is not ready, a timeout has not occurred and not aborted
            }while((StatusCheck() != BEP_STATUS_SUCCESS) && TimeRemaining() && !CheckAbort());

            // update the status of the test
            if(StatusCheck() == BEP_STATUS_SUCCESS) status = BEP_PASS_STATUS;
            else                                    status = BEP_FATALFAIL_STATUS;
        }
        else
            status = BEP_PASS_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepMachineReady Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // if the result of this test step is not pass, ABORT
    if(status != BEP_PASS_STATUS)   SystemWrite(ABORT_DATA_TAG, "1");

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepMachineReady Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "MachineTC::TestStepMachineReady(): %s\n", status.c_str());

    return(status);
}

//=============================================================================
inline const string MachineTC::HydraulicPumpMotor(const string &state)
{   // Somtimes the motor must be disabled then re-enabled in order to turn on
    if(!state.compare(HYDRAULIC_PUMP_MOTOR_ON))
    {
        SystemWrite(GetDataTag("HydraulicPumpMotor"), HYDRAULIC_PUMP_MOTOR_OFF);
        BposSleep(GetTestStepInfoInt("PumpStatusChangeDelay"));
    }
    string testResult = SystemWrite(GetDataTag("HydraulicPumpMotor"), state) == BEP_STATUS_SUCCESS ? testPass : testFail;
    string testResultCode = testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure");
    string testDescription = 
    testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure");
    Log(LOG_DEV_DATA, "Commanded Hydraulic Pump Motor %s: %s\n", state.c_str(), testResult.c_str());
    SendTestResult(testResult, testDescription, testResultCode);
    // Allow some extra time for the PLC to change state
    BposSleep(GetTestStepInfoInt("PumpStatusChangeDelay"));
    return testResult;
}

//=============================================================================
INT32 MachineTC::SetMaxHydraulicBoostPressure(const string &pressure)
{
    INT32 status = SystemCommand(MAX_BOOST_PRESSURE_VALUE, pressure);
    Log(LOG_DEV_DATA, "Commanded Max Hydraulic Boost Pressure to: %s, Status of write: %s\n",
        pressure.c_str(), ConvertStatusToResponse(status).c_str());
    return(status);
}

//=============================================================================
const std::string MachineTC::TestStepFinishUp(const std::string &value)
{
    // update the prompts to the user
    if(UpdatePrompts() != BEP_STATUS_SUCCESS)
        Log(LOG_ERRORS, "Unable to Update Prompts\n");
    // send the overall test result
    SendOverallResult();
    // always return success
    return(GetOverallResult());
}

//=============================================================================
const std::string MachineTC::TestStepAxleRatio(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepAxleRatio(%s)\n", value.c_str());
    std::string status = BEP_NONE;  // set test step status to no response
    std::string result = BEP_NONE;    // set result to no response
    if(!GetParameterBool("CheckFourWheelDriveSelected") || 
       (GetParameterBool("CheckFourWheelDriveSelected") && SystemReadBool("FourWheelDrive")))
    {
        float currentLFSpeed = 0.0;
        float currentRFSpeed = 0.0;
        float currentLRSpeed = 0.0;
        float currentRRSpeed = 0.0;
        float frontSum = 0.0;
        float rearSum = 0.0;
        float axleRatio = -1;
        int targetSpeed = GetTestStepInfoInt("TargetSpeed");
        Log(LOG_DEV_DATA,"Axle ratio target speed = %d\n",targetSpeed);

        try
        {
            std::string msgResponse;

            // if the conditions are correct to perform the task
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                // Make sure we are at zerospeed
                if(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
                {
                    // Tell driver to stop
                    DisplayPrompt(1, GetParameter("BrakeToStopPrompt"), 0, "","");
                    DisplayPrompt(2, "Blank", 0, "","");
                }

                while(ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
                {
                    BposSleep(200);
                    if((!TimeRemaining()) || (StatusCheck() != BEP_STATUS_SUCCESS)) break;
                }

                // Send 4wd selected to the PLC
                SystemWrite(GetDataTag("FourWDSelected"),true);
                SystemWrite(GetDataTag("Start4WDTestSignalToPLC"),true);

                // Setup the speed gauge target
                SystemWrite(GetDataTag("SpeedTarget"), GetTestStepInfo("SpeedTarget"));

                // Prompt the driver to shift to 4x4 low
                DisplayPrompt(1, GetParameter("SetOperatorConsoleTo4WDPrompt"), 0, "","");
                DisplayPrompt(2, "Blank", 0, "","");
                BposSleep(3000);    // give driver time to shift to 4WD
                DisplayPrompt(1, GetParameter("AccelerateToTargetSpeedPrompt"), 0, "","");

                Log(LOG_DEV_DATA,"Waiting for sample speed\n");
                // Wait for the sample speed
                while(TimeRemaining() && StatusCheck() == BEP_STATUS_SUCCESS && 
                      (currentLFSpeed < targetSpeed) && (currentRFSpeed < targetSpeed) &&
                      (currentLRSpeed < targetSpeed) && (currentRRSpeed < targetSpeed))
                {
                    // Sleep between speed samples
                    BposSleep(200);
                    // Resample the speeds
                    currentLFSpeed = SystemReadFloat(GetDataTag("LFSpeed"));
                    currentRFSpeed = SystemReadFloat(GetDataTag("RFSpeed"));
                    currentLRSpeed = SystemReadFloat(GetDataTag("LRSpeed"));
                    currentRRSpeed = SystemReadFloat(GetDataTag("RRSpeed"));
                }
                Log(LOG_DEV_DATA,"Sample speed reached\n");

                if(TimeRemaining())
                {
                    // Take roller samples
                    for(int i = 0; i < 5; i++)
                    {
                        frontSum += (SystemReadFloat(GetDataTag("LFSpeed")) +
                                     SystemReadFloat(GetDataTag("RFSpeed"))) / 2.0;
                        rearSum += (SystemReadFloat(GetDataTag("LRSpeed")) +
                                    SystemReadFloat(GetDataTag("RRSpeed"))) / 2.0;
                        
                        BposSleep(20);
                    }
                    Log(LOG_DEV_DATA,"Axle ratio sampling complete\n");

                    // Calculate the axle ratio (front to rear)
                    axleRatio = (frontSum / rearSum) * 100.0;
                    Log(LOG_DEV_DATA,"Axle ratio = %f; range = %f to %f\n",axleRatio,
                        GetTestStepInfoFloat("AxleRatioMin"),GetTestStepInfoFloat("AxleRatioMax"));
                    // Publish the axle ratio to the system
                    SystemWrite(GetDataTag("AxleRatio"), axleRatio);

                    if(axleRatio < GetTestStepInfoFloat("AxleRatioMin") ||
                       axleRatio > GetTestStepInfoFloat("AxleRatioMax"))
                    {
                        result = "FAIL";
                    }
                    else result = "PASS";
                }

                // Clear 4wd selected to PLC
                SystemWrite(GetDataTag("FourWDSelected"),false);
                SystemWrite(GetDataTag("Start4WDTestSignalToPLC"),false);

                // update the status of the test
                if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if(!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
                else if(result == "FAIL")               status = BEP_FAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "MachineTC::TestStepAxleRatio Exception: %s\n", e.what());
            status = BEP_SOFTWAREFAIL_STATUS;
        }

        // update the test result
        if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS, "MachineTC::TestStepAxleRatio: %s, %s\n",
                GetTestStepName().c_str(), status.c_str());
        }

        RemovePrompts();    // remove the prompts from the screen
    }
    else
    {
        Log(LOG_DEV_DATA, "Skipping axle ratio check");
        status = testSkip;
    }

    return(status);     // return the status
}

//=============================================================================
const std::string MachineTC::TestStepSpeedometer(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepSpeedometer\n");
    std::string status = BEP_NONE;  // set test step status to no response
    float lowLimit = GetParameterFloat("SpeedometerTestLowLimit");
    float highLimit = GetParameterFloat("SpeedometerTestHighLimit");
    float currentSpeed=0.0;
    float speedometerTestResult=0.0;
    float speedometerDiff = 999.0;
    float speedometerRatio = 0.0;
    bool activateSpeed = true;

	if(((GetTestStepResult() != testPass) && IsRetest()) || !IsRetest())
	{
		try
		{
			// if the conditions are correct to perform the task
			if(StatusCheck() == BEP_STATUS_SUCCESS)
			{
				SystemWrite(GetDataTag("TestResultBox3"),colorBlue);
				// prompt the driver to accelerate to the test speed
				if(UpdatePrompts() != BEP_STATUS_SUCCESS)
					Log(LOG_ERRORS, "Unable to Update Prompts\n");

				if(SystemWrite(GetDataTag("SpeedTarget"),GetParameter("SpeedometerSpeedTarget")) == BEP_STATUS_SUCCESS)
				{
					bool done = false;
					bool promptDisplayed = false;
					float wheelSpeeds[GetRollerCount()];
					if(GetParameterBool("Prompt40KPHTest"))
					{
						DisplayPrompt(GetPromptBox("AccelerateTo40KPH"),GetPrompt("AccelerateTo40KPH"),GetPromptPriority("AccelerateTo40KPH"));
					}
                    if(GetParameterBool("Prompt88KPHTest"))
					{
						DisplayPrompt(GetPromptBox("AccelerateTo88KPH"),GetPrompt("AccelerateTo88KPH"),GetPromptPriority("AccelerateTo88KPH"));
					}
					while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
					{
						// read the wheel speeds
						if(GetWheelSpeeds(wheelSpeeds) == BEP_STATUS_SUCCESS)
						{
							// store the speed of the left roll of the driven
							// axle as the speedometer test result
							if((ReadSubscribeData(GetDataTag("DriveAxle")) == "Rear") && !SystemReadBool("SingleAxleMachine"))
							{
								currentSpeed = wheelSpeeds[LRWHEEL];
							}
							else if(SystemReadBool("SingleAxleMachine"))
							{
								currentSpeed = wheelSpeeds[LFWHEEL];
							}
							else
							{
								currentSpeed = wheelSpeeds[LFWHEEL];
							}
						}
						if(GetParameterBool("BlindSpeedoTest"))
						{
							// Disable Speedometer
							SystemWrite(GetDataTag("SpeedActive"), !activateSpeed);
						}
						if(!GetParameterBool("DcxStyleSpeedoTest"))
						{
							// if the current speed is within the limits, send the
							// horn signal bit to the PLC
							if((currentSpeed >= lowLimit) && (currentSpeed <= highLimit))
							{
								SystemWrite(GetDataTag("HornSignal"),true);
								if(!promptDisplayed && !GetParameterBool("DisableSpeedoButtonPrompting"))
								{
									DisplayPrompt(GetPromptBox("PressSpeedoButtonAtSpeed"),GetPrompt("PressSpeedoButtonAtSpeed"),GetPromptPriority("PressSpeedoButtonAtSpeed"));
									promptDisplayed = true;
								}
							}
							else
							{
								SystemWrite(GetDataTag("HornSignal"),false);
							}

							// if the 40 KPH button is pressed
							if(ReadSubscribeData(GetDataTag("Start40kphTest")) == "1")
							{
								speedometerTestResult = currentSpeed;
								speedometerDiff = fabs(GetParameterFloat("TargetSpeed40kph") - currentSpeed);
								done = true;
								SpeedometerResultSpeed(&currentSpeed);
							}
						}
						else
						{	// If speed is in range, prompt operator to press the button
							if((lowLimit <= currentSpeed) && (currentSpeed <= highLimit))
							{	// Speed is in range, wait for the specified time limit
								if(testPass == MaintainSpeedForTime(GetParameterInt("SpeedometerStableTime"), (INT32)lowLimit, (INT32)highLimit))
								{
									done = true;
									// read the wheel speeds
									if(GetWheelSpeeds(wheelSpeeds) == BEP_STATUS_SUCCESS)
									{
										// store the speed of the left roll of the driven
										// axle as the speedometer test result
										if(ReadSubscribeData(GetDataTag("DriveAxle")) == "Rear")
										{
											currentSpeed = wheelSpeeds[LRWHEEL];
										}
										else
										{
											currentSpeed = wheelSpeeds[LFWHEEL];
										}
									}
									// Capture the current speed and delta speed from 40kph
									speedometerTestResult = currentSpeed;
									speedometerDiff = fabs(GetParameterFloat("TargetSpeed40kph") - currentSpeed);
									speedometerRatio = (100 * (speedometerDiff / GetParameterFloat("TargetSpeed40kph")));
									Log(LOG_DEV_DATA,"Speedo Speed: %5.2f, Ratio: %5.1f",
										speedometerTestResult,speedometerRatio);
									// Convert the values to kph
									speedometerTestResult *= 1.609344;
									speedometerDiff *= 1.609344;
									lowLimit *= 1.609344;
									highLimit *= 1.609344;
									Log(LOG_DEV_DATA,"(After Conversion) Speedo Speed: %5.2f, low: %5.2f, high: %5.2f",
										speedometerTestResult,lowLimit,highLimit);
								}
							}
						}

						if(!done) BposSleep(100);
					}

					if(!GetParameterBool("DcxStyleSpeedoTest"))
					{
						// make sure the horn bit is off
						SystemWrite(GetDataTag("HornSignal"),false);
						if(promptDisplayed && !GetParameterBool("DisableSpeedoButtonPrompting"))
						{
							RemovePrompt(GetPromptBox("PressSpeedoButtonAtSpeed"), GetPrompt("PressSpeedoButtonAtSpeed"), GetPromptPriority("PressSpeedoButtonAtSpeed"));
						}
					}

					if(GetParameterBool("BlindSpeedoTest"))
					{
						// activate speedometer
						SystemWrite(GetDataTag("SpeedActive"), activateSpeed);
					}

					// update the status of the test
					if(StatusCheck() != BEP_STATUS_SUCCESS)	UpdateResult(StatusCheck(), status);
					else if(!TimeRemaining())
					{
						status = BEP_TIMEOUT_STATUS;
						SystemWrite(GetDataTag("TestResultBox3"),GetDataTag("Fail"));
					}
					else if(!done)							status = BEP_FATALFAIL_STATUS;
					else
					{
						SystemWrite("SpeedoTestValue",speedometerTestResult);  
						// check if the test result is within the limits
						if((speedometerTestResult >= lowLimit) &&
						   (speedometerTestResult <= highLimit))
						{
							status = BEP_PASS_STATUS;
							SystemWrite(GetDataTag("TestResultBox3"),GetDataTag("Pass"));
						}
						else
						{
							status = BEP_FAIL_STATUS;
							SystemWrite(GetDataTag("TestResultBox3"),GetDataTag("Fail"));
						}
					}
					if(GetParameterBool("Prompt40KPHTest"))
					{
						RemovePrompt(GetPromptBox("AccelerateTo40KPH"),GetPrompt("AccelerateTo40KPH"),GetPromptPriority("AccelerateTo40KPH"));
					}
                    if(GetParameterBool("Prompt88KPHTest"))
					{
						RemovePrompt(GetPromptBox("AccelerateTo88KPH"),GetPrompt("AccelerateTo88KPH"),GetPromptPriority("AccelerateTo88KPH"));
					}
				}
				else status	= BEP_SOFTWAREFAIL_STATUS;
			}
			// else the conditions are not correct, indicate not started
			else status	= BEP_TEST_NOT_STARTED;
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "MachineTC::TestStepSpeedometer Exception: %s\n", e.what());
			status = BEP_SOFTWAREFAIL_STATUS;
		}

		char buffer[16];
		if(!GetParameterBool("DcxStyleSpeedoTest"))
		{
			SendSubtestResultWithDetail("Speedometer",status,"Speedometer Test Result","0000",
										"SpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",speedometerTestResult),"MPH",
										"MinSpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",lowLimit),"MPH",
										"MaxSpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",highLimit),"MPH",
										"SpeedometerDiff",CreateMessage(buffer,sizeof(buffer),"%.2f",speedometerDiff),"MPH");
		}
		else
		{
			SendSubtestResultWithDetail("Speedometer",status,"Speedometer Test Result","0000",
										"SpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",speedometerTestResult),"KPH",
										"MinSpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",lowLimit),"KPH",
										"MaxSpeedometerSpeed",CreateMessage(buffer,sizeof(buffer),"%.2f",highLimit),"KPH",
										"SpeedometerRatio",CreateMessage(buffer,sizeof(buffer),"%.1f",speedometerRatio),"%");
		}

		RemovePrompts();	// remove the prompts from the screen
	}
	else
	{
		status = GetTestStepResult();
	}
    return(status);     // return the status
}

//=============================================================================
const std::string MachineTC::TestStepSelectDriverSide(const std::string &value)
{
    string testResult(BEP_TESTING_STATUS);  // set test step status to no response
    string testResultCode("0000");
    string testDescription(GetTestStepInfo("Description"));
    string desiredLink = "";
    string commonLink = "";
    bool rightSideSelected = false;
    // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "MachineTC::TestStepSelectDriverSide()\n");
    if(!ShortCircuitTestStep())
    {
        try
        {   // check bit from PLC
            rightSideSelected = SystemReadBool(GetDataTag("RighthandDriveSelected"));
            Log(LOG_FN_ENTRY, "MachineTC::TestStepSelectDriverSide: RighthandDriveSelected = \"%s\"\n",
                rightSideSelected ? "True" : "False");
        }
        catch(...)
        {   // Could not get data, assuming left side is connected
            Log(LOG_ERRORS,"MachineTC::TestStepSelectDriverSide: Could not read selection from PLC\n");
            rightSideSelected = false;
        }
        try
        {
            desiredLink = rightSideSelected ? GetTestStepInfo("RightLogicalPort") : GetTestStepInfo("LeftLogicalPort");
            commonLink = GetTestStepInfo("CommmonLogicalPort");
            if(!desiredLink.empty() && !commonLink.empty())
            {   // Move the link to the correct side of the vehicle based on cable connect
                unlink(commonLink.c_str());
                testResult = !symlink(desiredLink.c_str(),commonLink.c_str()) ? testPass : testFail;
                if(testResult.compare(testPass))
                {
                    Log(LOG_ERRORS, "Could not create sym link: %s", strerror(errno));
                }
                testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("CommLinkFailure");
                testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("CommLinkFailure");
                Log(LOG_DEV_DATA, "Created symbolic link %s to %s: %s", 
                    commonLink.c_str(), desiredLink.c_str(), testResult.c_str());
            }
            else
            {
                Log(LOG_ERRORS,"MachineTC::TestStepSelectDriverSide: Link Info Missing in Test Data\n");
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("LinkInfoMissing");
                testDescription = GetFaultDescription("LinkInfoMissing");
            }
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "MachineTC::TestStepSelectDriverSide Exception: %s\n", e.what());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the results
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "CommonLink", commonLink, "",
                                 "CommPort", desiredLink, "");
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping test step: %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    return testResult;
}

//=============================================================================
const std::string MachineTC::TestStepReverse(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::TestStepReverse\n");
    std::string status = BEP_NONE;  // set test step status to no response
    BEP_STATUS_TYPE speedStatus = BEP_STATUS_FAILURE;   // Used to store return result from WaitForTargetSpeed()
    float testSpeed = GetParameterFloat("ReverseTestSpeed");
    string eraseSpeedRange = "0 0";

    try
    {
        // if the conditions are correct to perform the task
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {
            DisplayPrompt(1,"ShiftToReverse",0);
            BposSleep(2000);
            RemovePrompt(1,"ShiftToReverse",0);

            if(SystemWrite(GetDataTag("SpeedTarget"),GetParameter("ReverseSpeedTarget")) == BEP_STATUS_SUCCESS)
            {
                DisplayPrompt(1,"AccelerateToReverseTargetSpeed",0);
                SetStartTime();

                // Wait for the vehicle to achieve target speed
                bool testComplete = false;
                do
                {
                    speedStatus = WaitForTargetSpeed(-testSpeed, DOWN, 100);
                    if(BEP_STATUS_SUCCESS == speedStatus) testComplete = true;

                } while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !testComplete);

                // Set the test step result
                if(StatusCheck() != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_ERRORS, "ERROR GenericTC::ReverseTestStep() - StatusCheck() result - %s\n",
                        ConvertStatusToResponse(StatusCheck()).c_str());
                    status = BEP_FAIL_STATUS;
                }
                else if(!TimeRemaining() || (speedStatus != BEP_STATUS_SUCCESS))
                {
                    Log(LOG_ERRORS,"Timeout waiting for Reverse Gear or required speed\n");
                    status = BEP_TIMEOUT_STATUS;
                }
                else
                {
                    status = BEP_PASS_STATUS;
                }
            }
            else
            {
                status = BEP_SOFTWAREFAIL_STATUS;
            }

            RemovePrompt(1,"AccelerateToReverseTargetSpeed",0);

            // Remove the target green bands
            SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
        }
        else
        {
            status = BEP_TEST_NOT_STARTED;
        }
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepReverse Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::TestStepReverse Could Not Send Test Result: %s, %s\n", 
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    return(status);     // return the status
}

//=============================================================================
const std::string MachineTC::GetKeyPress(INT32 timeout)
{
    std::string status = BEP_ERROR_RESPONSE;
    // set the request for key input information
    m_requestKeyPressInput = true;
    // block on the keypress semaphore until keypress or timeout
    INT32 actualTime;
    if(timeout <= 0)    actualTime = -1;        // default to no timeout
    else                actualTime = timeout;
    do
    {   // look for the driver number to be entered in 1 second increments
        if(m_semKeyPress.CriticalSectionEnter(1) == BEP_STATUS_SUCCESS)
        {   // read the value of the Key Press
            if(GetParameterBool("UsePlcYesNoButtons"))
            {
                Log(LOG_DEV_DATA, "Checking PLC button presses");
                string yesButton(ReadSubscribeData(GetDataTag("YesButton")));
                string noButton(ReadSubscribeData(GetDataTag("NoButton")));
                status = !yesButton.compare("1") ? GetDataTag("YesValue") : !noButton.compare("1") ? GetDataTag("NoValue") : "";
                Log(LOG_DEV_DATA, "PLC Buttons: Yes - %s, No - %s, status - %s",
                    yesButton.c_str(), noButton.c_str(), status.c_str());
            }
            else
            {
                Log(LOG_DEV_DATA, "NOT Checking PLC button presses");
                status = ReadSubscribeData(GetDataTag("KeyPress"));
            }
        }
        if(actualTime > 0)  actualTime--;   // decrement the time waited if one provided
    }// while the system is still ok, the DriverNumber has not been received and the time has not expired
    while((StatusCheck() == BEP_STATUS_SUCCESS) && (actualTime != 0) &&
          (((status != GetDataTag("YesValue")) && (status != GetDataTag("NoValue"))) || (status == BEP_ERROR_RESPONSE)));

    // signal not looking for a keypress
    m_requestKeyPressInput = false;
    // return the value of the keypress
    return(status);
}

//=============================================================================
void MachineTC::SetKeyPress(void)
{   // if looking for keypress info tell the process that data is ready
    if(m_requestKeyPressInput == true)
    {
        m_semKeyPress.CriticalSectionExit();
        if(GetParameterBool("UsePlcYesNoButtons"))
        {
            Log(LOG_DEV_DATA, "New Button Press: Yes button - %s, No button - %s", 
                ReadSubscribeData(GetDataTag("YesButton")).c_str(), ReadSubscribeData(GetDataTag("NoButton")).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "New KeyPress: %s\n", ReadSubscribeData(GetDataTag("KeyPress")).c_str());
        }
    }
}

//=============================================================================
const std::string MachineTC::GetDriverNumber(INT32 timeout)
{
    std::string status = BEP_ERROR_RESPONSE;
    // set the request for key input information
    m_requestDriverNumber = true;
    // block on the keypress semaphore until Driver ID or timeout
    INT32 actualTime;
    if(timeout <= 0)    actualTime = -1;        // default to no timeout
    else                actualTime = timeout;
    do
    {   // look for the driver number to be entered in 1 second increments
        Log(LOG_DEV_DATA, "Waiting for Driver to enter number");
        // Make sure the input server is in driver number mode
        if(SystemRead(INPUT_SERVER_STATE).compare(INPUT_SERVER_DRIVER_NUMBER_STATE))
        {   //set inputserver to driver number mode
            Log(LOG_DEV_DATA, "InputServer not in DriverNumber Mode... setting");
            SystemWrite(string(INPUT_SERVER_STATE), string(INPUT_SERVER_DRIVER_NUMBER_STATE));
        }
        if(m_semDriverNumber.CriticalSectionEnter(1) == BEP_STATUS_SUCCESS)
        {   // read the value of the Driver ID
            status = ReadSubscribeData(GetDataTag("DriverNumber"));
        }
        if(actualTime > 0)  actualTime--;   // decrement the time waited if one provided
    }// while the system is still ok, the DriverNumber has not been received and the time has not expired
    while((StatusCheck() == BEP_STATUS_SUCCESS) && ( status == BEP_ERROR_RESPONSE) && (actualTime != 0));

    // signal not looking for a Driver ID
    m_requestDriverNumber = false;
    Log(LOG_FN_ENTRY, "Driver Number Received: %s\n", status.c_str());
    return(status);
}

//=============================================================================
void MachineTC::SetDriverNumber(void)
{   // if looking for keypress info tell the process that data is ready
    if(m_requestDriverNumber == true)
        m_semDriverNumber.CriticalSectionExit();
    Log(LOG_FN_ENTRY, "DriverNumber Is Ready\n");
}

//=============================================================================
inline string MachineTC::SetInputServerState(const string &state)
{
    return(BEP_STATUS_SUCCESS == SystemWrite(INPUT_SERVER_STATE, state)) ? testPass : testFail;
}

//=============================================================================
const std::string MachineTC::Publish(const XmlNode *node)
{   // set the result
    std::string result = SetData(node);
    // if the data was a keypress, notify the process
    if(node->getName() == GetDataTag("KeyPress"))
        SetKeyPress();
    // new PLC button press, notify the process
    else if(!node->getName().compare(GetDataTag("YesButton")) || !node->getName().compare(GetDataTag("NoButton")))
    {
        SetKeyPress();
    }
    // else if the data was a driver number, notify the process
    else if(node->getName() == GetDataTag("DriverNumber"))
        SetDriverNumber();

    Log(LOG_FN_ENTRY, "MachineTC::Publish(%s)", node->getName().c_str());
    return(result);
}

//=============================================================================
bool MachineTC::DriverNumberNotComplete(const std::string driverNumber)
{
    bool notComplete = true;
    // if not the default driver number and not empty
    if(driverNumber.compare(DEFAULT_DRIVER_NUMBER) && !driverNumber.empty())
        notComplete = false;

    Log(LOG_DEV_DATA, "Driver Number %s Not Complete Status: %d\n", driverNumber.c_str(), notComplete);

    return(notComplete);
}

//=============================================================================
const bool MachineTC::ValidateDriverNumber(const std::string &fileName, const std::string &driverNumber)
{
    bool valid = false;

    Log(LOG_FN_ENTRY, "Entering ValidateDriverNumber(%s, %s)\n", fileName.c_str(), driverNumber.c_str());

    try
    {   // localize the fileName
        std::string file = GetRootDir() + fileName;
        // create a parser and parse the driver ids
        XmlParser parser;
        const XmlNode *document = parser.ReturnXMLDocument(file);
        // if there are IDs in the list, validate the id that was entered
        if(document->getChildren().size())
        {
            XmlNodeMap drivers = const_cast< XmlNodeMap & >(document->getChildren());
            for(UINT32 ii = 0; (ii < document->getChildren().size()) && (!valid); ii++)
            {   // if the number has been found, we are good do go
                Log(LOG_DEV_DATA, "Comparing Numbers %s and %s\n", 
                    driverNumber.c_str(), drivers.getNode(ii)->getName().c_str());
                if(driverNumber == drivers.getNode(ii)->getName())
                    valid = true;
            }
        }
        else
            Log(LOG_DEV_DATA, "Empty DriverNumber file: %s\n", fileName.c_str());
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "Error: Invalid File Name: %s\n", fileName.c_str());
    }

    return(valid);
}

//=============================================================================
const std::string MachineTC::TestStepMultiDriverQuestion(const std::string &value)
{
    Log(LOG_FN_ENTRY, "MachineTC::MultiDriverQuestion(): %s\n", value.c_str());
    std::string status = BEP_PASS_STATUS;   // set test step status to no response
    std::string testResultName;
    std::string originalTestName = GetTestStepName();
    std::string currentQuestionName;
    std::string buildRecordTagName, compareValue, buildRecordValue, vinString;
    bool isConditionalSelection = false;
    bool skipQuestion = false;
    bool overrideFlag = false;
    string overrideQuestion;
    unsigned int overrideVINIdex;
    char overrideVINChar;
    const XmlNode *testStep = m_testSteps.getNode(GetTestStepName())->getChild(value);
    const XmlNodeMap *questions = &testStep->getChildren();
    SetCurrentTestStep(m_testSteps.getNode(GetTestStepName())->getChild(value));
    GenericTC::UpdateTaskStats(m_taskMonTaskName, "MultiQuestion after Setteststep()");
    // update the current test step base on the value
    Log(LOG_DEV_DATA, "test step name: %s\n", GetTestStepName().c_str());

    if(!ShortCircuitTestStep())
    {
        try
        {
            testResultName = GetTestStepInfo("TestResultName");
        }
        catch(...)
        {
            testResultName = GetTestStepName(); 
            Log(LOG_ERRORS, "TestResultName not found defaulting to: %s\n", testResultName.c_str());
        }
        Log(LOG_DEV_DATA, "TestResultName: %s\n", testResultName.c_str());
        try
        {   // if the conditions are correct to perform the task
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                // check if this is a conditional selection type multi-question
                try
                {
                    if(testStep->getAttribute("ConditionalSelection")->getValue() == "True")
                    {
                        isConditionalSelection = true;
                        // now check to see if this has an override question for certain VIN flags
                        try
                        {
                            // this is the name of the override question
                            overrideQuestion = testStep->getAttribute("OverrideQuestion")->getValue();
                            // this is the index of the character in the VIN to check for a match to OverrideVINChar
                            overrideVINIdex = atoi((testStep->getAttribute("OverrideVINIdex")->getValue()).c_str());
                            // the magical character to match in the VIN that means we always ask the override question
                            overrideVINChar = (testStep->getAttribute("OverrideVINChar")->getValue()).at(0);

                            Log(LOG_DEV_DATA, "Override question set:  Question=%s, VINIdex=%d, VINChar=%c\n", overrideQuestion.c_str(),
                                overrideVINIdex, overrideVINChar);

                            // read the VIN
                            if("ERROR" == (vinString = SystemRead("VIN")))
                                throw( XmlException( XmlString( XML_T("SystemRead failed"))));

                            // compare our overrideVINChar to the character found at overrideVINIdex
                            if(vinString.at(overrideVINIdex) == overrideVINChar)
                            {
                                // they match, this is an override situation
                                overrideFlag = true;
                                Log(LOG_DEV_DATA, "VIN Override, will ask question %s\n", overrideQuestion.c_str());
                            }
                            else
                                // no match, this is a normal conditional MPBO
                                overrideFlag = false;

                        }
                        catch(XmlException &xmlExcpt1)
                        {
                            // no override question (or VIN read error), defaulting to normal conditonal selection
                            overrideFlag = false;
                            Log(LOG_DEV_DATA, "Override question not set, defaulting to normal conditional behaviour\n");
                        }
                    }

                }
                catch(XmlException &xmlExcpt2)
                {
                    // ConditionalSelection attribute not set
                    // for backwards compatibility set to false
                    Log(LOG_DEV_DATA, "ConditionalSelection attribute not set, defaulting to false\n");
                    isConditionalSelection = false;
                }

                XmlNodeMapCItr iter = questions->begin();
                Log(LOG_DEV_DATA, "driverquestion: %s\n", iter->second->getName().c_str());
                while(status == BEP_PASS_STATUS && iter != questions->end())
                {
                    currentQuestionName = iter->second->getName();
                    SetCurrentTestStep(iter->second);
                    try
                    {   // if the conditions are correct to perform the task
                        if((StatusCheck() == BEP_STATUS_SUCCESS) || (GetTestStepName() == "DriverPassFail"))
                        {

                            if(isConditionalSelection)
                            {
                                Log(LOG_DEV_DATA, "driverquestion: %s is Conditional Selection MPBO\n", iter->second->getName().c_str());
                                // this is a conditional selection type multi question
                                try
                                {
                                    if(overrideFlag && (currentQuestionName == overrideQuestion))
                                    {
                                        // override type, this question is the override question and will be asked
                                        skipQuestion = false;
                                        Log(LOG_DEV_DATA, "Override: this is the override question, asking!\n");
                                    }
                                    else if(overrideFlag && (currentQuestionName != overrideQuestion))
                                    {
                                        // override type, this question is NOT the override question
                                        skipQuestion = true;
                                        Log(LOG_DEV_DATA, "Override: this is NOT the override question, skipping\n");
                                    }
                                    else
                                    {
                                        // normal conditional behaviour
                                        buildRecordTagName = iter->second->getAttribute("SelectBRTag")->getValue();
                                        compareValue = iter->second->getAttribute("SelectBRTagValue")->getValue();

                                        // set the skipQuestion flag if compareValue is not equal to data in build record tag
                                        if("ERROR" == (buildRecordValue = SystemRead(buildRecordTagName)))
                                            throw( XmlException( XmlString( XML_T("SystemRead failed"))));

                                        skipQuestion = compareValue != buildRecordValue;
                                        Log(LOG_DEV_DATA, "Normal conditional behaviour: skipQuestion = %s\n", skipQuestion ? "True":"False");
                                    }

                                }
                                catch(XmlException &XmlE)
                                {
                                    Log(LOG_ERRORS, "Exception getting XML data for ConditionalSelection comparison.  Reason: %s\n", XmlE.GetReason());
                                    Log(LOG_ERRORS, "Setting skipQuestion = false due to ConditionalSelection failure!\n");
                                    skipQuestion = false;
                                }
                            }
                            else
                            {
                                Log(LOG_DEV_DATA, "driverquestion: %s is not Conditional Selection MPBO\n", iter->second->getName().c_str());
                            }

                            if(!skipQuestion)
                            {
                                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                                // change the pendant state into TestResult mode
                                SystemWrite(GetDataTag("InputServerState"), GetDataTag("TestResultMode"));
                                INT32 timeout = max(0, GetTestStepInfoInt("Timeout")/1000);   // the timeout time
                                Log(LOG_DEV_DATA, "Setting Timeout Time To: %d\n", timeout);
                                // get the input from the pendant
                                std::string keyValue = GetKeyPress(timeout);
                                if(keyValue == GetDataTag("YesValue"))  status = BEP_PASS_STATUS;
                                else                                    status = BEP_FAIL_STATUS;
                            }
                            else
                            {
                                Log(LOG_DEV_DATA, "Skipping MultiDriverQuestion step: %s\n", currentQuestionName.c_str());
                                status = BEP_PASS_STATUS;
                            }

                        }
                        // else the conditions are not correct, indicate not started
                        else
                            status = BEP_TEST_NOT_STARTED;

                        // check to make sure an abort did not happen, if so flag it
                        if(CheckAbort())    status = BEP_ABORT_STATUS;
                    }
                    catch(BepException &e)
                    {
                        Log(LOG_ERRORS, "MachineTC DriverQuestion BepException: %s\n", e.what());
                        status = BEP_FAIL_STATUS;
                    }

                    RemovePrompts();

                    // change the InputServer state into Normal mode
                    SystemWrite(GetDataTag("InputServerState"), GetDataTag("NormalMode"));
                    iter ++;
                }
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;

            // check to make sure an abort did not happen, if so flag it
            if(CheckAbort())    status = BEP_ABORT_STATUS;
        }
        catch(BepException &e)
        {
            Log(LOG_ERRORS, "MachineTC MultiDriverQuestion BepException: %s\n", e.what());
            status = BEP_FAIL_STATUS;
        }
        // update the test results                                              
        if(SendSubtestResult(testResultName, status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS, "MachineTC Could Not Send Test Result: %s, %s\n",
                testResultName.c_str(), status.c_str());
        }
    }
    else
    {//skip this test
        Log(LOG_FN_ENTRY, "Skipping test step: %s", GetTestStepName().c_str());
        status = BEP_SKIP_STATUS;
    }
    if(GetParameterBool("DoNotFailVehicleForMPBO"))
    {
        Log(LOG_DEV_DATA, "Parameter set to not fail overall vehicle test for MPBO failures.  Updating result from %s to Pass", status.c_str());
        status = BEP_PASS_STATUS;
    }
    Log(LOG_FN_ENTRY, "TestStepMultiDriverQuestion: %s, Result: %s\n", GetTestStepName().c_str(), status.c_str());

    return(status);             // return the status
}

//-----------------------------------------------------------------------------
const float MachineTC::SpeedometerResultSpeed(const float *speed /*= NULL*/)
{
    if(speed != NULL)   m_speedoTestSpeedValue = *speed;
    return m_speedoTestSpeedValue;
}

//-----------------------------------------------------------------------------
string MachineTC::ValidateMachineConditions(const string value)
{   // Log the entry and determine if this step should be performed
    string result(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "MachineTC::ValidateMachineConditions() - Enter");
    if(!SystemRead(MACHINE_TYPE).compare(GetParameter("ValidateMachineConditionsMachineType")))
    {   // Run through the list of items and see if any are in the wrong state
        bool allConditionsCorrect = true;
        for(XmlNodeMapItr iter = m_machineConditions.begin(); iter != m_machineConditions.end(); iter++)
        {
            string systemTag(iter->second->getValue());
            string matchValue(iter->second->getAttribute("Value")->getValue());
            string actualValue(SystemRead(systemTag));
            Log(LOG_DEV_DATA, "Checking %s == %s  [actual: %s]", systemTag.c_str(), matchValue.c_str(), actualValue.c_str());
            if(actualValue.compare(matchValue))
            {   // Actual value does not match desired value, take the necessary action
                string promptTag(iter->second->getAttribute("Prompt")->getValue());
                DisplayTimedPrompt(promptTag, GetPromptBox(promptTag), GetPromptPriority(promptTag), GetPromptDuration(promptTag));
                string mismatchAction(iter->second->getAttribute("MismatchAction")->getValue());
                allConditionsCorrect = false;
                if(!mismatchAction.compare(ABORT_DATA_TAG))
                {
                    Log(LOG_DEV_DATA, "Aborting test sequence due to incorrect machine state");
                    result = testAbort;
                    SystemWrite(ABORT_DATA_TAG, true);
                }
                else
                {   // No specified action, just skip any action and move on
                    Log(LOG_DEV_DATA, "Ignoring incorrect machine state");
                }
            }
        }
        // Determine the overall result
        result = allConditionsCorrect ? testPass : testFail;
        SendTestResult(result, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Invalid machine type
        Log(LOG_ERRORS, "Not configured to run on this machine");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MachineTC::ValidateMachineConditions() - Enter");
    return result;
}


//=============================================================================
const string MachineTC::RaiseRollsLowerElevators (void)
{
    Log(LOG_FN_ENTRY, "MachineTC::RaiseRollsLowerElevators()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the vehicle is not retained
        bool isVehicleRetained = !(bool)(ReadSubscribeData(GetDataTag("RollsDownElevUp")) == "1");
        Log(LOG_FN_ENTRY, "RaiseRollsLowerElevators isVehicleRetained: %d\n", isVehicleRetained);
        if(!isVehicleRetained)
        {   // if the machine is in the expected state
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                //Command System mon to retain vehicle
                SystemWrite(GetDataTag("CommandSysMonRaiseRolls"), true);

                // prompt retainers are being raised /elevators lowered
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                bool done = false;
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if(ReadSubscribeData(ROLLS_UP_DATA_TAG) != "1")    BposSleep(250);
                    else
                    {
                        Log(LOG_FN_ENTRY, "Vehicle retained\n");
                        done = true;
                    }
                }
                // update the status of the test
                if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if((!TimeRemaining()) || (!done))  status = BEP_FATALFAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;
        }
        else
            status = BEP_PASS_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::RaiseRollsLowerElevators Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // if the result of this test step is not pass, ABORT
    if(status != BEP_PASS_STATUS)   SystemWrite(ABORT_DATA_TAG, "1");

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::RaiseRollsLowerElevators Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "MachineTC::RaiseRollsLowerElevators(): %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string MachineTC::LowerRollsRaiseElevators (void)
{
    Log(LOG_FN_ENTRY, "MachineTC::LowerRollsRaiseElevators()\n");
    std::string status = BEP_NONE;  // set test step status to no response

    try
    {   // if the vehicle is not released
        bool isVehicleReleased = !(bool)(ReadSubscribeData(GetDataTag("RollsUpElevDown")) == "1");
        Log(LOG_FN_ENTRY, "LowerRollsRaiseElevators isVehicleReleased: %d\n", isVehicleReleased);
        if(!isVehicleReleased)
        {   // if the machine is in the expected state
            if(StatusCheck() == BEP_STATUS_SUCCESS)
            {
                //Command System mon to retain vehicle
                SystemWrite(GetDataTag("CommandSysMonLowerRolls"), true);

                // prompt retainers are being lowered /elevators raised
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                bool done = false;
                while((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if(ReadSubscribeData(GetDataTag("RollsDownElevUp")) != "1")    BposSleep(250);
                    else
                    {
                        Log(LOG_FN_ENTRY, "Vehicle released\n");
                        done = true;
                    }
                }
                // update the status of the test
                if(StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
                else if((!TimeRemaining()) || (!done))  status = BEP_FATALFAIL_STATUS;
                else                                    status = BEP_PASS_STATUS;
            }
            // else the conditions are not correct, indicate not started
            else
                status = BEP_TEST_NOT_STARTED;
        }
        else
            status = BEP_PASS_STATUS;
    }
    catch(BepException &e)
    {
        Log(LOG_ERRORS, "MachineTC::LowerRollsRaiseElevators Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // if the result of this test step is not pass, ABORT
    if(status != BEP_PASS_STATUS)   SystemWrite(ABORT_DATA_TAG, "1");

    // update the test result
    if(SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "MachineTC::LowerRollsRaiseElevators Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "MachineTC::LowerRollsRaiseElevators(): %s\n", status.c_str());

    return(status);
}

//=============================================================================
const string MachineTC::PerformNvhCycle(const string &powerLevel)
{
	string result = BEP_TESTING_STATUS;
	Log(LOG_FN_ENTRY, "MachineTC::PerformNvhCycle(power level: %s) - Enter", powerLevel.c_str());
	if(!ShortCircuitTestStep())
	{
		if(CheckZeroSpeed())
		{   // Verify the operator wants to perform the NVH test
			if(testPass == OperatorPassFail("PerformNvhTest"))
			{
				// Instruct the operator to select vehicle power level
				DisplayTimedPrompt(GetPrompt("SelectCarPower"), GetPromptBox("SelectCarPower"), 
								   GetPromptPriority("SelectCarPower"), GetPromptDuration("SelectCarPower"), powerLevel);
				// Accelerate to the test speed
				char buff[32];
				float targetSpeed = GetParameterFloat("NvhTargetSpeed");
				string speedRange = CreateMessage(buff, sizeof(buff), "%.2f %.2f", targetSpeed, targetSpeed+5.0);
				result = AccelerateToTestSpeed(targetSpeed, speedRange, 250, false);
				// Return to zero speed
				CheckZeroSpeed();
			}
			else
			{
				Log(LOG_DEV_DATA, "Operator chose to skip NVH cycle");
				result = testSkip;
			}
		}
		else
		{
			result = testTimeout;
			Log(LOG_FN_ENTRY, "Timeout waiting for zero speed");
		}
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping NVH Cycle");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "MachineTC::PerformNvhCycle(power level: %s) - Exit", powerLevel.c_str());
	return result;
}
