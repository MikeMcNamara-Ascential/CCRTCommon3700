//******************************************************************************
// Description:
//  Limited Slip Differential Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//
//
// 1    11/10/10 12:55p KHarman
// Initial creation of file.
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================


#include <cmath>
#include "LsdTc.h"

//-----------------------------------------------------------------------------
LsdTc::LsdTc() : GenericTC()
{
    Log(LOG_FN_ENTRY, "Generic LsdTc Constructor complete\n");
}

//-----------------------------------------------------------------------------
LsdTc::~LsdTc()
{
    Log(LOG_FN_ENTRY, "Generic LsdTc Destructor complete\n");
}

//=============================================================================
void LsdTc::Initialize(const XmlNode *config)
{   // Initialize the base component
    Log(LOG_FN_ENTRY, "Initializing The LsdTc Component:\n%s\n", config->ToString().c_str());
    // call the base class initialize
    GenericTC::Initialize(config);
    // Setup the loss data
    Log(LOG_FN_ENTRY, "Done Initializing LsdTc Component\n");
    // signal that initialization is complete
    InitializationComplete();
}

//-----------------------------------------------------------------------------
const string LsdTc::CommandTestStep(const string &value)
{
    string status;

    try
    {   // Get the test step name to perform
        string step = GetTestStepName();
        Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Entering test step %s\n",
            GetComponentName().c_str(), value.c_str(), step.c_str());
        // Check the status to ensure it is alright to run this test step
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {       // Not OK to run the test step
            UpdateResult(StatusCheck(), status);
            Log(LOG_ERRORS, (GetComponentName() + ".CommandTestStep: StatusCheck() failed: %s\n").c_str(),
                status.c_str());
            SendTestResult(status, GetTestStepInfo("Description"));
        }
        // Start turning one of the rear wheels.
        else if(GetTestStepName() == "TurnWheel")                  status = TurnWheel();
        // Get the test results.
        else if(GetTestStepName() == "GetTestResults")             status = GetTestResults();
        // Finish up anything that needs to be done
        else if(GetTestStepName() == "FinishUp")                   status = FinishUp();
        // Call GenericTCTemplate for addition command steps
        else
            status = GenericTC::CommandTestStep(value);
    } catch(BepException &e)
    {
        Log(LOG_DEV_DATA, "%s CommandTestStep %s Exception: %s\n",
            GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n", GetComponentName().c_str(), value.c_str(), status.c_str());
    return(status);
}

//=============================================================================
string LsdTc::GetTestResults(void)
{
    string results;
    string statDescript = "Axle Passed";
    // The type of axle that the car has
    string axleType(SystemRead(GetDataTag("Axle")));
    bool isLsd = false;
    char RFWheelStr[16];
    char LFWheelStr[16];
    char RRWheelStr[16];
    char LRWheelStr[16];

    if(ShortCircuitTestStep())
        results = testSkip;
    else
    {
        // Put the axle type to all uppercase
        for(unsigned i=0; i<axleType.length(); i++)
            axleType[i] = toupper(axleType[i]);

        isLsd = axleType.compare("STANDARD");

        if(isLsd && SameSpeeds(fltWheelSpeeds) && SameDirections(fltWheelSpeeds))
            results = testPass;
        else if(!isLsd && !SameDirections(fltWheelSpeeds))
            results = testPass;
        else
        {
            if(!SameSpeeds(fltWheelSpeeds))
            {
               Log(LOG_ERRORS, "LsdTc Error: Wheels failed to move at the same speed.\n");
               statDescript = "Wheels not moving at the same speed.";
            }

            if(isLsd && !SameDirections(fltWheelSpeeds))
            {
                Log(LOG_ERRORS, "LsdTc Error: LSD axle failed to move in the correct direction.\n");
                statDescript = "LSD axle failed to move in the correct direction.";
            }
            else if(!isLsd && SameDirections(fltWheelSpeeds))
            {
                Log(LOG_ERRORS, "LsdTc Error: Standard axle failed to move in the correct direction.\n");
                statDescript = "Standard axle failed to move in the correct direction.";
            }

            results = testFail;

        }
    }

    // Send test results
    if(results != testSkip)
    {
        SendTestResultWithDetail(results, statDescript, "0000", 
                             "LF Wheel Speed", CreateMessage(LFWheelStr, sizeof(LFWheelStr), "%.2f", fltWheelSpeeds[LFWHEEL]), "MPH", 
                             "RF Wheel Speed", CreateMessage(RFWheelStr, sizeof(RFWheelStr), "%.2f", fltWheelSpeeds[RFWHEEL]), "MPH",
                             "LR Wheel Speed", CreateMessage(LRWheelStr, sizeof(LRWheelStr), "%.2f", fltWheelSpeeds[LRWHEEL]), "MPH", 
                             "RR Wheel Speed", CreateMessage(RRWheelStr, sizeof(RRWheelStr), "%.2f", fltWheelSpeeds[RRWHEEL]), "MPH");
    }
    else
    {
        SendTestResult(results, statDescript);
    }

    return results;
}


//=============================================================================
inline bool LsdTc::SameSpeeds(float wheelSpeed[])
{
    float LWMin = abs(wheelSpeed[LRWHEEL])-3;
    float LWMax = abs(wheelSpeed[LRWHEEL])+3;

    // Check if the wheels are turning at the same speed (even if opposite directions)
    return LWMin<=abs(wheelSpeed[RRWHEEL]) && LWMax>=abs(wheelSpeed[RRWHEEL]);
}


//=============================================================================
inline bool LsdTc::SameDirections(float wheelSpeed[])
{
    // Check if the wheels are turning at the same speed (even if opposite directions)
    return(wheelSpeed[LRWHEEL]>=0 && wheelSpeed[RRWHEEL]>=0) || (wheelSpeed[LRWHEEL]<0 && wheelSpeed[RRWHEEL]<0);
}


//=============================================================================
string LsdTc::TurnWheel(void)
{
    float   speedVal = (GetParameterFloat("LsdTestSpeedValue") != 0) ? GetParameterFloat("LsdTestSpeedValue") : 5;
    char    temp[256];
    string  status;
    string  statDescript;
    char RFWheelStr[16];
    char LFWheelStr[16];
    char RRWheelStr[16];
    char LRWheelStr[16];


    if(ShortCircuitTestStep())
    {
        status = testSkip;
        statDescript = "Skipped";
    }
    else
    {
        if(CheckZeroSpeed())
        {
            DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"));
            BposSleep(10);
            DisplayTimedPrompt(GetPrompt("ShiftNeutral"), GetPromptBox("ShiftNeutral"), GetPromptPriority("ShiftNeutral"), GetPromptDuration("ShiftNeutral"));

            SetStartTime();
            // Go to torque and speed 0 first to clear any command history in the motor controller
            m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
            m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
            m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
            m_MotorController.Write("RightRearTorqueValue", "0.00", false);
            m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
            m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
            m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
            m_MotorController.Write("RightRearSpeedValue", "0.00", true);
            BposSleep(100);

            Log( LOG_DEV_DATA, "Setting the left rear wheel speed value to %.02f\n", speedVal);

            // Turn on the left rear motor to the designated speed.
            m_MotorController.Write("LeftFrontMotorMode", TORQUE_MODE, false);
            m_MotorController.Write("RightFrontMotorMode", TORQUE_MODE, false);
            m_MotorController.Write("LeftRearMotorMode", SPEED_MODE, false);
            m_MotorController.Write("RightRearMotorMode", TORQUE_MODE, false);
            m_MotorController.Write("LeftRearSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), true);

            // Tell the operators that the system is checking the axle
            DisplayPrompt(GetPromptBox("AxleCheck"), GetPrompt("AxleCheck"));

            // Wait until the wheel speed is close enough to the desired speed or there is a timeout or error.
            while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining())
            {
                GetWheelSpeeds(fltWheelSpeeds);
                if(fltWheelSpeeds[LRWHEEL]>=(speedVal-1) && fltWheelSpeeds[LRWHEEL]<=(speedVal+1))
                    break;
                BposSleep(100);     // wait 1/10th of a second
            }

            // Check for errors/timeout
            if(!TimeRemaining() || GenericTC::StatusCheck()!=BEP_STATUS_SUCCESS)
            {
                status = testFailure;
                statDescript = "Wheel speed timeout or motor error.";
                WheelsStop();
            } 
            else
            {
                BposSleep(2000);
                GetWheelSpeeds(fltWheelSpeeds);
                status = WheelsStop();
                statDescript = "Wheel speeds successfully read.";
            }

            // Remove the prompt that the system is checking the axle
            RemovePrompt(GetPromptBox("AxleCheck"), GetPrompt("AxleCheck"), GetPromptPriority("AxleCheck"));
            RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        }
        else
        {
            status = testFailure;
            statDescript = "Wheels not at zero speed.";
        }

        CheckZeroSpeed();

    }

    // Send test results
    if(status != testSkip)
    {
        SendTestResultWithDetail(status, statDescript, "0000", 
                             "LF Wheel Speed", CreateMessage(LFWheelStr, sizeof(LFWheelStr), "%.2f", fltWheelSpeeds[LFWHEEL]), "MPH", 
                             "RF Wheel Speed", CreateMessage(RFWheelStr, sizeof(RFWheelStr), "%.2f", fltWheelSpeeds[RFWHEEL]), "MPH",
                             "LR Wheel Speed", CreateMessage(LRWheelStr, sizeof(LRWheelStr), "%.2f", fltWheelSpeeds[LRWHEEL]), "MPH", 
                             "RR Wheel Speed", CreateMessage(RRWheelStr, sizeof(RRWheelStr), "%.2f", fltWheelSpeeds[RRWHEEL]), "MPH");
    }
    else
    {
        SendTestResult(status, statDescript);
    }
    

    return status;
}

//=============================================================================
string LsdTc::WheelsStop()
{
    // Make sure we clear the speed/torque command values
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    return (GenericTC::StatusCheck()==BEP_STATUS_SUCCESS) ? testPass : testFailure;
}

//-----------------------------------------------------------------------------
inline string LsdTc::FinishUp(void)
{
    SendTestResult(testPass, GetTestStepInfo("Description"), "0000");
    SendOverallResult(testPass);
    return(testPass);
}
