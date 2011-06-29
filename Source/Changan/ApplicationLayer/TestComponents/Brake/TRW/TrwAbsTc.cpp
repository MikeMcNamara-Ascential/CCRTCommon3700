//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the TRW ABS.
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
#include "TrwAbsTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TrwAbsTc<ModuleType>::TrwAbsTc() : GenericABSTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TrwAbsTc<ModuleType>::~TrwAbsTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void TrwAbsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericABSTCTemplate<ModuleType>::InitializeHook(config);
    // Default to a 4 channel ABS system
    SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TrwAbsTc<ModuleType>::SensorTest(void)
{   // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TrwAbsTc::SensorTest() - Enter");
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
        Log(LOG_FN_ENTRY, "Skipping Sensortest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TrwAbsTc::SensorTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TrwAbsTc<ModuleType>::RotateRoller(const UINT8 &rollerIndex, 
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
