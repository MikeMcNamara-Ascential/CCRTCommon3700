//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
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
#include "MazdaMachineTC.h"

//-------------------------------------------------------------------------------------------------
MazdaMachineTC::MazdaMachineTC() : MachineTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaMachineTC::~MazdaMachineTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "MazdaMachineTC::CommandTestStep(value: %s) - Enter", value.c_str());
    if(BEP_STATUS_SUCCESS == StatusCheck())
    {
        try
        {                                    
            if(!GetTestStepName().compare("WaitForAcceleration"))  testResult = WaitToStart();
            if(!GetTestStepName().compare("CableConnect"))  testResult = CableConnect();
            if(!GetTestStepName().compare("StartTest"))  testResult = StartTest();
            else   testResult = MachineTC::CommandTestStep(value);
        }
        catch(BepException &excpt)
        {
            Log(LOG_ERRORS, "BepException sequencing test step %s - %s", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testFail;
        }
    }
    else
    {
        Log(LOG_ERRORS, "Bad system status, not starting test step %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaMachineTC::CommandTestStep(value: %s) - Exit", value.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepSpeedometer(const string &value)
{
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepSpeedometer() - Enter");
    string testDescription(GetTestStepInfo("Description"));
    // Set the result box color to let operator know testing has started
    SystemWrite(GetDataTag("SpeedometerTestSpeedBGColor"), colorYellow);
    // Get the vehicle build data
    bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
    float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
    string speedoType(convertSpeed ? "KPH" : "MPH");
    float minSpeed = GetVehicleParameter("SpeedTargets/"+speedoType+"/SpeedometerSpeedTargetLow", float(0.0)) * speedScaling;
    float maxSpeed = GetVehicleParameter("SpeedTargets/"+speedoType+"/SpeedometerSpeedTargetHi", float(0.0)) * speedScaling;
    char buff[32];
    string dispMaxSpeed(CreateMessage(buff, sizeof(buff), "%.0f", maxSpeed));
    string dispMinSpeed(CreateMessage(buff, sizeof(buff), "%.0f", minSpeed));
    string speedRange(dispMinSpeed + " " + dispMaxSpeed);
    // Wait for the operator to get to the correct speed range - divide by speed scale since the base class functions in mph.
    DisplayPrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
    DisplayPrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"),
                  colorWhite, string(dispMinSpeed + " - " + dispMaxSpeed));
    string testResult = (WaitForTargetSpeed(maxSpeed + 10.0, DOWN, GetTestStepInfoInt("ScanDelay")) ==
                         BEP_STATUS_SUCCESS) ? testPass : testFail;
    SystemWrite(GetDataTag("SpeedTarget"), speedRange);
    // Get the wheel speeds
    float currentSpeed = GetRollSpeed();
    // Make sure operator is in the correct speed range
    if(!testResult.compare(testPass))
    {   // Vehicle is in speed range, prompt operator to flash headlamps
        SetStartTime();
        bool done = false;
        // Wait for the operator to flash high beams
        while(!done && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS))
        {   // Get the current wheel speeds
            currentSpeed = GetRollSpeed();
            // Check if the highbeams have been flashed yet
            if(!ReadSubscribeData(GetDataTag("StartSpeedoTest")).compare("1"))
            {   // Highbeams flashed, update the value on the screen
                SystemWrite(GetDataTag("SpeedometerTestResultSpeed"), currentSpeed);
                // Determine if the test passed or failed
                string resultColor;
                if((minSpeed <= currentSpeed) && (currentSpeed <= maxSpeed))
                {   // Test has passed
                    resultColor = colorGreen;
                    testResult = testPass;
                    done = true;
                }
                else
                {   // Test failed
                    resultColor = colorRed;
                    testResult = testFail;
                    testDescription = "Speedometer out of range";
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
                // Update screen colors
                SystemWrite(GetDataTag("SpeedometerTestResultBox"), resultColor);
                SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), resultColor);
            }
            else
            {   // Need to wait a bit
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
        }
        // Check if there was an abnormal exit condition
        if(!done)
        {   // Set test to fail
            SystemWrite(GetDataTag("SpeedometerTestResultBox"), colorRed);
            SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), colorRed);
            testResult = testFail;
            testDescription = "Speedometer test timeout";
            Log(LOG_ERRORS, "Speedometer test incomplete - TimeRemaining: %s, StatusCheck: %s", 
                TimeRemaining() ? "True" : "False", ConvertStatusToResponse(StatusCheck()).c_str());
        }
        // Remove prompts
        RemovePrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
        RemovePrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"));
    }
    else
    {   // Error accelerating to speed
        Log(LOG_ERRORS, "Could not achieve speed range, not performing speedometer test");
    }
    // Remove the speed target
    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
    // Report the result, log the exit and return
    SendTestResultWithDetail(testResult, testDescription, "0000",
                             "RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", GetRollSpeed()),
                             (speedScaling != 1.0) ? unitsKPH : unitsMPH);
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepSpeedometer() - Exit");
    return testResult;
}

const string MazdaMachineTC::StartTest(void)
{

    Log(LOG_FN_ENTRY, "MazdaMachineTC::StartTest() - Enter");

    string testResult(BEP_TESTING_RESPONSE);

    testResult = GenericTC::OperatorPassFail(GetPrompt("StartTest"), GetParameterFloat("StartTestTimeoutPrompt")); 


    Log(LOG_FN_ENTRY, "MazdaMachineTC::StartTest() - Exit - %s", testResult.c_str());

    return BEP_PASS_RESPONSE;
}

const string MazdaMachineTC::CableConnect(void)
{
    return testPass;
}


//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepReverse(const string &value)
{   // Log the entry and prompt the operator to accelerate in reverse
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepReverse(value: %s) - Enter", value.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
    float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
    float reverseSpeed = GetVehicleParameter("ReverseTestSpeed", float(0.0)) * speedScaling;
    // Wait for the operator to reach zeroish speed
    while((GetRollSpeed() > (GetParameterFloat("PromptReverseTestSpeed") * speedScaling)) &&
          (BEP_STATUS_SUCCESS == StatusCheck()))
    {   // Wait just a bit so we do not hog the CPU - also reset the start time since there is no way to tell
        // when the operator will stop
        BposSleep(1000);
        SetStartTime();
    }
    // Display the target for the operator
    char buff[32];
    string reverseTarget(CreateMessage(buff, sizeof(buff), "%.2f", reverseSpeed));
    // Prompt the operator to accelerate to the target
    DisplayPrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
    DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"),
                  string("white"), reverseTarget);
    // Wait for the target speed to be reached
    while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (GetRollSpeed() > reverseSpeed)) BposSleep(250);
    testResult = (GetRollSpeed() < reverseSpeed) ? testPass : testFail;
    SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    // Remove the prompts
    RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
    RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepReverse(value: %s) - Exit", value.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaMachineTC::WaitToStart(void)
{   // Log the entry and wait for the loss of zero speed
    Log(LOG_FN_ENTRY, "MazdaMachineTC::WaitToStart() - Enter");
    while((GetRollSpeed() < GetParameterFloat("VehicleSpeedNeededForStart")) && (BEP_STATUS_SUCCESS == StatusCheck()))
    {   // Wait for a bit
        BposSleep(1000);
        SetStartTime();
    }
    // Log the exit and return pass
    Log(LOG_FN_ENTRY, "MazdaMachineTC::WaitToStart() - Exit");
    return testPass;
}

//-------------------------------------------------------------------------------------------------
inline float MazdaMachineTC::GetRollSpeed(void)
{
    return GetParameterBool("SingleEncoder") ? SystemReadFloat(GetDataTag("MaximumRollerSpeedTag")) : GenericTC::GetRollSpeed();
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepAccelerateToSpeed(const string &value)
{   // Log the entry and prompt the operator
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Enter", value.c_str());
    bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
    float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
    char buff[32];
    string dispSpeed(CreateMessage(buff, sizeof(buff), "%.0f", atof(value.c_str()) * speedScaling));
    DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"), 
                  string("white"), dispSpeed);
    // Wait for the operator to achieve speed
    float speedTarget = atof(value.c_str()) * speedScaling;
    while((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && (GetRollSpeed() < speedTarget))  BposSleep(250);
    // Make sure speed was schieved and return the result
    Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Exit", value.c_str());
    RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
    return (GetRollSpeed() >= speedTarget) ? testPass : testFail;
}

const string MazdaMachineTC::TestStepRainLightSensorVerification(void)
{

    Log(LOG_FN_ENTRY, "MazdaMachineTC::StartRainLightSensorTest() - Enter");

    string testResult(BEP_SKIP_RESPONSE);
    INT32 status;

    //To do: determine if rls is equipped from build data

    testResult = GenericTC::OperatorPassFail(GetPrompt("StartRLSTest"), GetParameterFloat("StartRLSTestPromptTimeout")); 
    if (testResult == BEP_PASS_RESPONSE)
    {
        //command plc to turn on light and spray devices

        SystemWrite(GetDataTag("EnableLightAndSprayDevice"), true);
        status = StatusSleep( GetParameterInt("LightAndSprayDeviceActuationTime"););

        //Operater pass / fail 
        testResult = status == BEP_STATUS_SUCCESS ? GenericTC::OperatorPassFail(GetPrompt("VerifyRLSOperation"), GetParameterFloat("VerifyRLSOperationPromptTimeout")) : 
            testFail;
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }

    Log(LOG_FN_ENTRY, "MazdaMachineTC::StartRainLightSensorTest() - Exit - %s", testResult.c_str());

    return testResult;
}

