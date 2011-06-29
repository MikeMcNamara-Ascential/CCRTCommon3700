//*************************************************************************
// FILE DESCRIPTION:
//  Module Interface for the Bosch ABS.
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
#include "BoschAbsTc.h"

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
BoschAbsTc<AbsModuleType>::BoschAbsTc() : GenericABSTCTemplate<AbsModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
BoschAbsTc<AbsModuleType>::~BoschAbsTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
const string BoschAbsTc<AbsModuleType>::CommandTestStep(const string &value)
{   // Log the entry and determine if system is OK to continue testing
    Log(LOG_FN_ENTRY, "BoschAbsTc::CommandTestStep(%s) - Enter", value.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    try
    {   // Make sure system status os ok to run this test step
        if(BEP_STATUS_SUCCESS == StatusCheck())
        {
            if(!GetTestStepName().compare("CheckSteeringAngle"))                testResult = CheckSteeringAngleSensor();
            else if(!GetTestStepName().compare("PerformSensorQualityTest"))     testResult = PerformSensorQualityTest();
            else if(!GetTestStepName().compare("DisableDiagnosticSpeedLimit"))  testResult = DisableDiagnosticSpeedLimit();
            else           testResult = GenericABSTCTemplate<AbsModuleType>::CommandTestStep(value);
        }
        else
        {   // Bad system status, skip the test step
            testResult = testSkip;
            Log(LOG_ERRORS, "BoschAbsTc: Bad system status - %s, skipping %s", 
                ConvertStatusToResponse(StatusCheck()).c_str(), GetTestStepName().c_str());
        }
    }
    catch(ModuleException &excpt)
    {
        Log(LOG_ERRORS, "BoschAbsTc: Module Exception attempting to run %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        testResult = testSoftwareFail;
    }
    catch(BepException &excpt)
    {
        Log(LOG_ERRORS, "BoschAbsTc: BepException attempting to run %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        testResult = testSoftwareFail;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschAbsTc::CommandTestStep(%s) - Exit", value.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
void BoschAbsTc<AbsModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericABSTCTemplate<AbsModuleType>::InitializeHook(config);
    // Default to a 4 channel ABS system
    SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
string BoschAbsTc<AbsModuleType>::CheckSteeringAngleSensor()
{   // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "BoschAbsTc::CheckSteeringAngleSensor() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Read the steering angle sensor from the module
        INT32 angle = -99;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadSteeringAngleSensor", angle);
        if(BEP_STATUS_SUCCESS == status)
        {   // Check if the angle is in the proper range
            testResult = ((GetParameterInt("MinimumSteeringAngle") <= angle) && 
                          (angle <= GetParameterInt("MaximumSteeringAngle"))) ? testPass : testFail;
            Log(LOG_DEV_DATA, "Steering angle: %d [%d %d] - %s", angle, GetParameterInt("MinimumSteeringAngle"),
                GetParameterInt("MaximumSteeringAngle"), testResult.c_str());
        }
        else
        {   // Could not read the steering angle from the module
            testResult = testFail;
            Log(LOG_ERRORS, "Could not read steering angle from the module: %s", ConvertStatusToResponse(status).c_str());
        }
        // Report the results
        char buff[16];
        SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                                 "Angle", CreateMessage(buff, sizeof(buff), "%d", angle), "degrees",
                                 "MinAngle", GetParameter("MinimumSteeringAngle"), "degrees",
                                 "MaxAngle", GetParameter("MaximumSteeringAngle"), "degrees");
    }
    else
    {   // Skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping CheckSteeringAngleSensor()");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschAbsTc::CheckSteeringAngleSensor() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
string BoschAbsTc<AbsModuleType>::DisableDiagnosticSpeedLimit(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "BoschAbsTc::DisableDiagnosticSpeedLimit() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Command the module to disable the diagnostic speed limit
        testResult = m_vehicleModule.CommandModule("DisableDiagnosticSpeedLimit") == BEP_STATUS_SUCCESS ? testPass : testFail;
        Log(LOG_ERRORS, "Disabled diagnostic speed limit: %s", testResult.c_str());
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else
    {
        Log(LOG_DEV_DATA, "Skipping DisableDiagnosticSpeedLimit");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschAbsTc::DisableDiagnosticSpeedLimit() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
string BoschAbsTc<AbsModuleType>::PerformSensorQualityTest()
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "BoschAbsTc::PerformSensorQualityTest() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   // Get the current drive axle so we can restore when completed
        string originalDriveAxle(SystemRead(DRIVE_AXLE_TAG));
        // Set the drive axle to Rear so fronts can be rotated
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        // Prompt the operator to shift to neutral and remove foot from brake pedal
        DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep(GetParameterInt("OperatorSetupDelay"));
        // Make sure speed set points are all zero
        m_MotorController.Write("LeftFrontSpeedValue", "0", false);
        m_MotorController.Write("RightFrontSpeedValue", "0", false);
        m_MotorController.Write("LeftRearSpeedValue", "0", false);
        m_MotorController.Write("RightRearSpeedValue", "0", false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        // Rotate each wheel and check to make sure the module believes it is the one spinning
        TestResultDetails details;
        testResult = testPass;
        for(UINT8 index = LFWHEEL; (index <= RRWHEEL) && !testResult.compare(testPass); index++)
        {
            testResult = RotateRoller(index, GetParameter("SensorQualitySpeed"), 
                                      GetParameterInt("SensorQualitySpeedDelay"), details);
        }
        // Reset machine to Boost mode and restore the drive axle
        m_MotorController.Write("LeftFrontSpeedValue", "0", false);
        m_MotorController.Write("RightFrontSpeedValue", "0", false);
        m_MotorController.Write("LeftRearSpeedValue", "0", false);
        m_MotorController.Write("RightRearSpeedValue", "0", false);
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        BposSleep(2000);
        SystemWrite(DRIVE_AXLE_TAG, originalDriveAxle);
        // Report the result
        SendTestResultWithDetail(testResult, details, GetTestStepInfo("Description"), "0000");
        // Remove the prompts
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping PerformSensorQualityTest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschAbsTc::PerformSensorQualityTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class AbsModuleType>
string BoschAbsTc<AbsModuleType>::RotateRoller(const UINT8 &rollerIndex, 
                                             const string &rollerSpeed, 
                                             const INT32 &rotationTime, 
                                             TestResultDetails &details)
{   
    string testResult = testFail;
    // Command the roller to speed
    Log(LOG_DEV_DATA, "Commanding %s roller to %s", rollerName[rollerIndex].c_str(), rollerSpeed.c_str());
    // Check if the drive axle should be changed
    if(LRWHEEL == rollerIndex)
    {   // Switch the drive axle so the last two rollers can spin
        m_MotorController.Write("LeftFrontSpeedValue", string("0"), false);
        m_MotorController.Write("RightFrontSpeedValue", string("0"), true);
        BposSleep(1000);
        SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
        BposSleep(1000);
    }
    // Command the roller to speed
    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", rollerSpeed, true);
    // Determine the min and max values using the tolerance required
    float minSpeed = (GetParameterFloat("SensorQualitySpeed") * 
                      (1.0 - (GetParameterFloat("SensorQualitySpeedTolerance") / 100.0)));
    float maxSpeed = (GetParameterFloat("SensorQualitySpeed") *
                      (1.0 + (GetParameterFloat("SensorQualitySpeedTolerance") / 100.0)));
    char buff[16];
    details.AddDetail(rollerName[rollerIndex]+"MinSpeedValue", CreateMessage(buff, sizeof(buff), "%.2f", minSpeed), unitsMPH);
    details.AddDetail(rollerName[rollerIndex]+"MaxSpeedValue", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeed), unitsMPH);
    // Wait for the roller to achieve speed before moving on to the next roller
    BposSleep(rotationTime);
    // Read the sensor speeds from the module
    vector<float> sensorSpeeds;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("WheelSpeedSensors", sensorSpeeds);
    if(BEP_STATUS_SUCCESS == status)
    {   // Make sure the correct sensor is giving the reading
        if((minSpeed <= sensorSpeeds[rollerIndex]) && (sensorSpeeds[rollerIndex] <= maxSpeed))
        {   // Sensor is in range
            testResult = testPass;
        }
        else
        {   // Sensor is out of range
            testResult = testFail;
        }
        Log(LOG_DEV_DATA, "%s sensor result: %s - sensor: %.2f, [%.2f  %.2f]",
            rollerName[rollerIndex].c_str(), testResult.c_str(), sensorSpeeds[rollerIndex], minSpeed, maxSpeed);
        char buff[8];
        details.AddDetail(rollerName[rollerIndex]+"SensorSpeed", 
                           CreateMessage(buff, sizeof(buff), "%.2f", sensorSpeeds[rollerIndex]), unitsMPH);
    }
    else
    {
        Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
        details.AddDetail(rollerName[rollerIndex]+"SensorResult", testFail, "");
        details.AddDetail(rollerName[rollerIndex]+"SensorSpeed", "0", unitsMPH);
    }
    // Command the current roller to stop
    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", string("0"), true);
    return testResult;
}
