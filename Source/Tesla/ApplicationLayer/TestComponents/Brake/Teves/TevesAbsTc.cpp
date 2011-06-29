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
#include "TevesAbsTc.h"

//----------------------------------------------------------------------------
template<class ModuleInterface>
TevesAbsTc<ModuleInterface>::TevesAbsTc() : GenericABSTCTemplate<ModuleInterface>(), m_originalDriveAxle("")
{   // Nothing special to do here
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
TevesAbsTc<ModuleInterface>::~TevesAbsTc()
{   // Nothing special to do here
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string TevesAbsTc<ModuleInterface>::CommandTestStep(const string &value)
{
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "TevesAbsTc::CommandTestStep(%s) - Enter", value.c_str());
    try
    {   // Make sure it is ok to run the test step
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {   // System status is not ok, do not perform test
            UpdateResult(StatusCheck(), result);
            SendTestResult(result, GetTestStepInfo("Description"));
            Log(LOG_ERRORS, "TevesAbsTc::CommandTestStep() - StatusCheck failed (%s), not performing %s",
                result.c_str(), GetTestStepName().c_str());
        }
        // Command the module back to normal mode
        else if(!GetTestStepName().compare("EnterNormalMode"))           result = EnterNormalMode();
        // Disable the force meter
        else if(!GetTestStepName().compare("DisableForceMeter"))         result = DisableForceMeter();
        // Perform the sensor and burnish test
        else if(!GetTestStepName().compare("SensorTest"))                result = IndividualSensorTest();
        // Perform the brake burnish cycle
        else if(!GetTestStepName().compare("BrakeBurnishCycle"))         result = BrakeBurnishCycle();
        else if(!GetTestStepName().compare("StaticBrakeBurnishCycle"))   result = StaticBrakeBurnishCycle();
        else if(!GetTestStepName().compare("AccelerateToBrakeSpeed"))    result = AccelerateToBrakeSpeed();
        // Call the base class to handle the test step
        else    result = GenericABSTCTemplate<ModuleInterface>::CommandTestStep(value);
    }
    catch (ModuleException &excpt)
    {
        Log(LOG_ERRORS, "TevesAbsTc.%s: %s", GetTestStepName().c_str(), excpt.GetReason());
        result = BEP_SOFTWAREFAIL_RESPONSE;
    }
    catch (BepException &excpt)
    {
        Log(LOG_DEV_DATA, "TevesAbsTc.CommandTestStep %s Exception: %s", GetTestStepName().c_str(), excpt.GetReason());
        result = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_FN_ENTRY, "TevesAbsTc::CommandTestStep(%s) - Exit", value.c_str());
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::DetermineSystemConfiguration(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TevesAbsTc::DetermineSystemConfiguration() - Enter");
    if(!ShortCircuitTestStep())
    {
        SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
        result = testPass;
    }
    else
    {   // Need to skip this test step
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping TevesAbsTc::DetermineSystemConfiguration()");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesAbsTc::DetermineSystemConfiguration() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
INT32 TevesAbsTc<ModuleInterface>::ElectricParkBrakeInit(void)
{
    INT32 result = GenericTC::ElectricParkBrakeInit();
    // Store the oringinal drive axle value so we can restore after the test
    string driveAxle(SystemRead(DRIVE_AXLE_TAG));
    OriginalDriveAxle(&driveAxle);
    // Switch the drive axle so the rear set of motors is enabled
    SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string TevesAbsTc<ModuleInterface>::PerformPBTorqueTest(const string &direction)
{
    string status(BEP_TESTING_STATUS);
    // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "Enter TevesAbsTc::PerformPBTorqueTest(%s)\n", direction.c_str());
    float torqueValue = GetParameterFloat("ParkBrakeApplyForce");
    // Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
    m_MotorController.Write(COMMAND_SPEED, "0", false);
    m_MotorController.Write(COMMAND_TORQUE, "0", true);
    BposSleep( 100);
    // Zero check the parameters
    if(torqueValue == 0)  torqueValue = 350;
    // Adjust the torque value
    torqueValue *= 0.5;
    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueValue > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueValue < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueValue, -torqueValue);
        torqueValue *= -1.0;
    }
    // read the current distance of the wheels
    GetWheelDistances( m_parkBrakeDistances[ 0]);
    // Tell the motor to turn for the park brake test
    Log(LOG_DEV_DATA, "Commanding Rear Motors %.2f LBS\n", torqueValue);
    char temp[32];
    m_MotorController.Write(COMMAND_TORQUE, CreateMessage(temp, sizeof(temp), "%.2f", torqueValue), true);
    m_MotorController.Write(MOTOR_MODE, TORQUE_MODE, true);
    // wait the specified amount of time
    SetStartTime();
    bool brakeApplied = false;
    INT32 testTime = GetTestStepInfoInt("TestTime");
    float   wheelSpeeds[GetRollerCount()];
    while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) && !brakeApplied)
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        // Check the brake switch to make sure it is off
        m_vehicleModule.ReadModuleData("ReadBrakeSwitch", brakeApplied);
        if(brakeApplied)
        {
            Log(LOG_ERRORS, "Operator applied brake during park brake test - FAIL");
        }
        BposSleep(100);     // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }
    // Clear the torque value
    m_MotorController.Write(COMMAND_TORQUE, string("0"), true);
    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");
    // if and error did not occur and not aborted
    if(!brakeApplied)
    {
        INT32 statusCheck = GenericTC::StatusCheck();
        if( statusCheck == BEP_STATUS_SUCCESS)
        {   // read the current distance of the rear wheels
            GetWheelDistances( m_parkBrakeDistances[ 1]);
            // Analyze the distances that the wheels travelled
            status = ValidateParkBrakeTestDistances(m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1]);
        }
        else
        {
            Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
            status = ConvertStatusToResponse( statusCheck);
        }
    }
    else
    {
        status = testFail;
        Log(LOG_ERRORS, "Park brake test failed because operator applied brake pedal");
    }
    // Reset to the original drive axle
    SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
    RemovePrompts();
    DisplayTimedPrompt(GetPrompt("ReleaseParkBrake"), GetPromptBox("ReleaseParkBrake"), 
                       GetPromptPriority("ReleaseParkBrake"), GetPromptDuration("ReleaseParkBrake"));
    // Send the test result info
    SendTestResult(status, GetTestStepInfo("Description"));
    Log(LOG_FN_ENTRY, "Exit TevesAbsTc::PerformPBTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return status;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::EnterNormalMode(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and check if the test step should be performed
    Log(LOG_FN_ENTRY, "TevesAbsTc::EnterNormalMode() - Enter");
    if(!ShortCircuitTestStep())
    {   // Command the module to enter normal mode
        BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnterNormalMode");
        result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
        Log(LOG_ERRORS, "Commanded module to enter normal mode: %s (status: %s)", 
            result.c_str(), ConvertStatusToResponse(status).c_str());
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step EnterNormalMode");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesAbsTc::EnterNormalMode() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::DisableForceMeter(void)
{
    // Disable brake force meter updates
    m_baseBrakeTool->DisableForceUpdates();
    // Deactivate the brake force meter
    SystemWrite(GetDataTag("BrakeActive"), 0);
    Log(LOG_FN_ENTRY, "TevesAbsTc::DisableForceMeter() - Disabled brake force meter");
    return testPass;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::IndividualSensorTest(void)
{   // Log the entry and determine if this test should be performed
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "TevesAbsTc::IndividualSensorTest() - Enter");
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   // Store the oringinal drive axle value so we can restore after the test
        string driveAxle(SystemRead(DRIVE_AXLE_TAG));
        OriginalDriveAxle(&driveAxle);
        // Switch the drive axle so the rear set of motors is enabled
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        // Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
        m_MotorController.Write(COMMAND_SPEED, "0", false);
        m_MotorController.Write(COMMAND_TORQUE, "0", false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        BposSleep( 100);
        string sensorResult[GetRollerCount()];
        DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep(GetParameterInt("SensorSpeedDriverDelay"));
        // Check all wheels independently
        for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
        {   // Check if the drive axle must be changed
            if((rollerIndex == LRWHEEL) && ((rollerIndex - 1) == RFWHEEL))
            {
                SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
                BposSleep(1000);
            }
            // Perform the wheel speed sensor test
            // Set the speed of the first roller
            m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", GetParameter("SensorTestSpeed"), true);
            // Wait a bit for the rollers to start accelerating
            BposSleep(GetParameterInt("SensorTestStartDelay"));
            // Read the sensor speeds from the module
            WheelSpeeds_t moduleSpeeds;
            BEP_STATUS_TYPE status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
            if(BEP_STATUS_SUCCESS == status)
            {   // Get the roller speeds
                float rollerSpeeds[GetRollerCount()];
                GetWheelSpeeds(rollerSpeeds);
                // Check all wheels, only current wheel should be spinning, all others should be 0
                float maxSpeed = rollerSpeeds[rollerIndex] * (1.0 + (GetParameterFloat("SensorSpeedTolerance") / 100.0));
                float minSpeed = rollerSpeeds[rollerIndex] * (1.0 - (GetParameterFloat("SensorSpeedTolerance") / 100.0));
                string indResult = (rollerSpeeds[rollerIndex] >= (GetParameterFloat("SensorTestSpeed") - 1.0)) ? testPass : testFail;
                for(UINT8 wheel = LFWHEEL; 
                     (wheel <= RRWHEEL) && !indResult.compare(testPass) && (BEP_STATUS_SUCCESS == StatusCheck()); wheel++)
                {
                    if(wheel == rollerIndex)
                    {   // Make sure this wheel is at the correct speed
                        indResult = ((minSpeed <= moduleSpeeds[wheel]) && (moduleSpeeds[wheel] <= maxSpeed)) ? testPass : testFail;
                    }
                    else
                    {
                        indResult = (moduleSpeeds[wheel] <= GetParameterFloat("ZeroSpeedTolerance")) ? testPass : testFail;
                    }
                }
                sensorResult[rollerIndex] = indResult;
                Log(LOG_DEV_DATA, "%s: %s - Sensor Speed: %.2f, Roller Speed: %.2f, [min: %.2f, max: %.2f]", 
                    rollerName[rollerIndex].c_str(), sensorResult[rollerIndex].c_str(), moduleSpeeds[rollerIndex], 
                    rollerSpeeds[rollerIndex], maxSpeed, minSpeed);
                // Report this roller
                char buff[32];
                SendSubtestResultWithDetail(rollerName[rollerIndex]+"SensorSpeedTest", sensorResult[rollerIndex], 
                                            GetTestStepInfo("Description"), "0000",
                                            "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[rollerIndex]), unitsMPH,
                                            "MinLimit", CreateMessage(buff, sizeof(buff), "%.2f", minSpeed), unitsMPH,
                                            "MaxLimit", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeed), unitsMPH,
                                            "RollerSpeed", GetParameter("SensorTestSpeed"), unitsMPH);
            }
            else
            {   // Could not read sensor speeds
                Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
                sensorResult[rollerIndex] = testFail;
            }
            // Set the motor back to boost and zero speed
            m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", "0", true);
            // Wait for zero speed
            while(!SystemReadBool(ZEROSPEED_TAG) && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(250);
        }
        // Remove the prompts
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        // Reset the drive axle after we reach zero speed
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        CheckZeroSpeed();
        SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
        // Report the overall result
        testResult = (!sensorResult[LFWHEEL].compare(testPass) && !sensorResult[RFWHEEL].compare(testPass) &&
                      !sensorResult[LRWHEEL].compare(testPass) && !sensorResult[RRWHEEL].compare(testPass)) ? testPass : testFail;
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed sensor test, not testing again");
        testResult = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping sensor test");
        testResult = testSkip;
    }
    // Log the exit and return the result.
    Log(LOG_FN_ENTRY, "TevesAbsTc::IndividualSensorTest() - Exit");
    return testResult;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::BrakeBurnishCycle(void)
{
    string result(testPass);
    // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TevesAbsTc::BrakeBurnishCycle() - Enter");
    
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass) && OperatorPassFail("PerformBrakeBurnish").compare(testPass))
    {
        for(INT32 burnishCycles = 0; 
             (burnishCycles < GetParameterInt("BrakeBurnishCycles")) && 
             (BEP_STATUS_SUCCESS == StatusCheck()) && 
             !result.compare(testPass); 
             burnishCycles++)
        {
            for(INT32 burnishIterations = 0;
                 (burnishIterations < GetParameterInt("BurnishIterations")) && 
                 (BEP_STATUS_SUCCESS == StatusCheck()) && 
                 !result.compare(testPass);
                 burnishIterations++)
            {   // Prompt the operator to achieve the desired speed range
                result = AccelerateToTestSpeed(GetParameterFloat("BrakeBurnishStartSpeed"), GetParameter("BrakeBurnishStartSpeedRange"), 
                                               GetTestStepInfoInt("ScanDelay"), false);
                if(!result.compare(testPass))
                {
                    // Disable motor boost so the rollers are in free roll mode
                    if(GetParameterBool("BrakeBurnishDisableBoostOnDecel"))  DisableElectricMotorBoost();
                    // Display the brake force target for the operator
                    string forceTarget = ((burnishIterations == (GetParameterInt("BurnishIterations")-1)) ? 
                                          "BrakeBurnishFinalIterationTarget" : "BrakeBurnishBrakeForceTarget");
                    m_baseBrakeTool->EnableForceUpdates();
                    SystemWrite(GetDataTag("BrakeTarget"), GetParameter(forceTarget));
                    SystemWrite(GetDataTag("BrakeActive"), true);
                    // If the target speed has been reached, brake to the target speed
                    DisplayPrompt(GetPromptBox("BrakeModeratelyInGreenBand"), GetPrompt("BrakeModeratelyInGreenBand"),
                                  GetPromptPriority("BrakeModeratelyInGreenBand"));
                    result = SlowDownToTestSpeed(GetParameterFloat("BrakeBurnishEndSpeed"), 
                                                 GetParameter("BrakeBurnishEndSpeedRange"),
                                                 GetTestStepInfoInt("ScanDelay"), true, 
                                                 GetParameter("BrakeToTargetSpeedPrompt"));
                    // Disable the brake force meter
                    RemovePrompt(GetPromptBox("BrakeModeratelyInGreenBand"), GetPrompt("BrakeModeratelyInGreenBand"),
                                 GetPromptPriority("BrakeModeratelyInGreenBand"));
                    SystemWrite(GetDataTag("BrakeActive"), false);
                    SystemWrite(GetDataTag("BrakeTarget"), "0 0");
                    m_baseBrakeTool->DisableForceUpdates();
                    // Reneable boosting
                    if(GetParameterBool("BrakeBurnishDisableBoostOnDecel"))  EnableElectricMotorBoost();
                }
                else
                {
                    Log(LOG_ERRORS, "Timeout waiting for brake burnish start speed");
                    result = testTimeout;
                }
            }
        }
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Brake burnish cycle");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesAbsTc::BrakeBurnishCycle() - Enter");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::StaticBrakeBurnishCycle(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "TevesAbsTc::StaticBrakeBurnishCycle() - Enter");
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass) && OperatorPassFail("PerformBrakeBurnish").compare(testFail))
    {   // Place the motors in speed mode
        m_MotorController.Write(COMMAND_SPEED, string("0"), false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        // Save the original drive axle
        string driveAxle(SystemRead(DRIVE_AXLE_TAG));
        OriginalDriveAxle(&driveAxle);
        // Switch to control the front motors
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        // Run the front wheels
        for(UINT8 roller = LFWHEEL; (roller <= RFWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); roller++)
        {   // Accelerate the current roller to speed
            DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            BposSleep(GetParameterInt("StaticBrakeBurnishOperatorSetupTime"));
            m_MotorController.Write(rollerName[roller]+"SpeedValue", GetParameter("StaticBrakeBurnishAccelSpeed"), true);
            // Wait for the roller to get to speed
            float rollerSpeeds[GetRollerCount()];
            do
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
                GetWheelSpeeds(rollerSpeeds);
            } while((BEP_STATUS_SUCCESS == StatusCheck()) && 
                    (rollerSpeeds[roller] < (GetParameterFloat("StaticBrakeBurnishAccelSpeed") - 3.0)));
            // Check if we can keep testing
            RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            if(BEP_STATUS_SUCCESS == StatusCheck())
            {   // Have operator apply brake to target speed
                SystemWrite(GetDataTag("SpeedTarget"), GetParameter("StaticBrakeBurnishBrakeSpeedRange"));
                DisplayPrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), GetPromptPriority("BrakeToTargetSpeed"));
                BEP_STATUS_TYPE status = StatusSleep(GetParameterInt("StaticBrakeBurnishTime"));
                RemovePrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), GetPromptPriority("BrakeToTargetSpeed"));
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                if(BEP_STATUS_SUCCESS == status)
                {   // Cool down period
                    DisplayPrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), 
                                  GetPromptPriority("BrakeCoolDown"), GetParameter("BrakeCoolDownBgColor"));
                    DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
                    m_MotorController.Write(rollerName[roller]+"SpeedValue", GetParameter("StaticBrakeBurnishCoolDownSpeed"), true);
                    StatusSleep(GetParameterInt("StaticBrakeBurnishCoolDownTime"));
                    m_MotorController.Write(rollerName[roller]+"SpeedValue", string("0"), true);
                    // Wait for zero speed
                    while(!atob(SystemRead(ZEROSPEED_TAG).c_str())) BposSleep(GetTestStepInfoInt("ScanDelay"));
                    RemovePrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), GetPromptPriority("BrakeCoolDown"));
                    RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
                    SystemWrite(string("PromptBox1BGColor"), string("white"));
                    SystemWrite(string("PromptBox2BGColor"), string("white"));
                }
                else
                {
                    Log(LOG_ERRORS, "Status sleep failed during brake burnish cycle");
                    result = testFail;
                }
            }
            else
            {
                Log(LOG_ERRORS, "Cannot continue testing: StatusCheck - %s", ConvertStatusToResponse(StatusCheck()).c_str());
                result = testFail;
            }
        }
        // Burnish the rears if the fronts have not failed
        if(!result.compare(BEP_TESTING_STATUS))
        {   // Switch the drive axle to control rear rdrives
            SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
            DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            BposSleep(GetParameterInt("StaticBrakeBurnishOperatorSetupTime"));
            // Accelerate the rollers
            m_MotorController.Write(COMMAND_SPEED, GetParameter("StaticBrakeBurnishAccelSpeed"), true);
            // Wait for the roller to get to speed
            float rollerSpeeds[GetRollerCount()];
            do
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
                GetWheelSpeeds(rollerSpeeds);
            } while((BEP_STATUS_SUCCESS == StatusCheck()) && 
                    (rollerSpeeds[LRWHEEL] < (GetParameterFloat("StaticBrakeBurnishAccelSpeed") - 3.0)));
            // Check if we can keep testing
            RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            if(BEP_STATUS_SUCCESS == StatusCheck())
            {   // Have operator apply brake to target speed
                if(GetParameterBool("StaticBrakeBurnishDisplaySpeedTargetForRear"))
                {
                    SystemWrite(GetDataTag("SpeedTarget"), GetParameter("StaticBrakeBurnishBrakeSpeedRange"));
                }
                else
                {
                    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                }
                DisplayPrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), 
                              GetPromptPriority("BrakeToTargetSpeed"));
                BEP_STATUS_TYPE status = StatusSleep(GetParameterInt("StaticBrakeBurnishTime"));
                RemovePrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), 
                             GetPromptPriority("BrakeToTargetSpeed"));
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                if(BEP_STATUS_SUCCESS == status)
                {   // Cool down period
                    DisplayPrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), 
                                  GetPromptPriority("BrakeCoolDown"), GetParameter("BrakeCoolDownBgColor"));
                    DisplayPrompt(GetPromptBox("ReleaseParkBrake"), GetPrompt("ReleaseParkBrake"), GetPromptPriority("ReleaseParkBrake"));
                    m_MotorController.Write(COMMAND_SPEED, GetParameter("StaticBrakeBurnishCoolDownSpeed"), true);
                    StatusSleep(GetParameterInt("StaticBrakeBurnishCoolDownTime"));
                    m_MotorController.Write(COMMAND_SPEED, string("0"), true);
                    // Wait for zero speed
                    while(!atob(SystemRead(ZEROSPEED_TAG).c_str())) BposSleep(GetTestStepInfoInt("ScanDelay"));
                    RemovePrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), GetPromptPriority("BrakeCoolDown"));
                    RemovePrompt(GetPromptBox("ReleaseParkBrake"), GetPrompt("ReleaseParkBrake"), GetPromptPriority("ReleaseParkBrake"));
                    SystemWrite(string("PromptBox1BGColor"), string("white"));
                    SystemWrite(string("PromptBox2BGColor"), string("white"));
                    result = testPass;
                }
                else
                {
                    Log(LOG_ERRORS, "Status sleep failed during brake burnish cycle");
                    result = testFail;
                }
            }
            else
            {
                Log(LOG_ERRORS, "Cannot continue testing: StatusCheck - %s", ConvertStatusToResponse(StatusCheck()).c_str());
                result = testFail;
            }
        }
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
        SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
    }
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed static brake burnish, not testing again");
        result = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping static brake burnish cycle");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesAbsTc::StaticBrakeBurnishCycle() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string& TevesAbsTc<ModuleInterface>::OriginalDriveAxle(const string *driveAxle /*= NULL*/)
{
    if(driveAxle != NULL)  m_originalDriveAxle = *driveAxle;
    return m_originalDriveAxle;
}



//----------------------------------------------------------------------------
template<class ModuleInterface>
string TevesAbsTc<ModuleInterface>::AccelerateToBrakeSpeed(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and check if the test step should be performed
    Log(LOG_FN_ENTRY, "TevesAbsTc::AccelerateToBrakeSpeed() - Enter");
    if(!ShortCircuitTestStep())
    {   // Command the module to enter normal mode
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        result = GenericABSTCTemplate<ModuleInterface>::CommandTestStep("AccelerateToBrakeSpeed");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step AccelerateToBrakeSpeed");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesAbsTc::AccelerateToBrakeSpeed() - Exit");
    return result;
}
