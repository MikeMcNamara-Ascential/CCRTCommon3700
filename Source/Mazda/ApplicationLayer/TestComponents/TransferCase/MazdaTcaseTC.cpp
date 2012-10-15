//****************************************************************************
// DESCRIPTION:
//    Test specific to the Mazda AWD.
//
//============================================================================
//
//    Copyright (c) 2010- Burke E. Porter Machinery Company
//    All Rights Reserved
//
//    This file contains confidential information of Burke E. Porter Machinery
//    and is not to be used in any way detrimental to the interests thereof.
//    Unauthorized use, distribution, copying, or transmittal of this file in
//    any way is strictly prohibited.
//*************************************************************************
#include "MazdaTcaseTC.h"

//-------------------------------------------------------------------------------------------------
MazdaTcaseTC::MazdaTcaseTC() : GenericTcaseTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaTcaseTC::~MazdaTcaseTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
string MazdaTcaseTC::CheckAxleRatio(void)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "MazdaTcaseTC::CheckAxleRatio() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Determine if the vehicle is equipped with AWD
        if(!SystemRead(GetDataTag("AwdEquippedFromPlc")).compare("1"))
        {   // Wait for zero speed
            SystemWrite(GetDataTag("AxleRatioBackgroundColorTag"), GetParameter("TestInProgressColor"));
            CheckZeroSpeed();
            SetStartTime();
            // Prompt the operator to accelerate to speed
            float awdSpeed = GetVehicleParameter("AwdTestSpeed", float(0.0));
            char buff[32];
            string speedRange(CreateMessage(buff, sizeof(buff), "%.2f", awdSpeed));
            DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"),
                          string("white"), speedRange);
            float axleDiff = -99.9;
            if(WaitForTargetSpeed(awdSpeed, UP) == BEP_STATUS_SUCCESS)
            {
                WHEELINFO currentWheelSpeeds;
                do
                {   // Get the roller speeds
                    GetWheelSpeeds(currentWheelSpeeds);
                    float frontSpeed = (currentWheelSpeeds.lfWheel + currentWheelSpeeds.rfWheel) / 2.0;
                    float rearSpeed  = (currentWheelSpeeds.lrWheel + currentWheelSpeeds.rrWheel) / 2.0;
                    axleDiff = (frontSpeed > rearSpeed) ? (frontSpeed - rearSpeed) : (rearSpeed - frontSpeed);
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                } while(TimeRemaining() && (axleDiff >= GetVehicleParameter("AwdMaxAxleDiff", float(0.0))) && 
                        (BEP_STATUS_SUCCESS == StatusCheck()));
                // Update the background color
                testResult = (axleDiff <= GetVehicleParameter("AwdMaxAxleDiff", float(0.0))) ? testPass : testFail;
            }
            else
            {   // Timeout waiting for speed
                testResult = testFail;
                Log(LOG_DEV_DATA, "Timeout waiting for vehicle speed to be in range");
            }
            SystemWrite(GetDataTag("AxleRatioTag"), axleDiff);
            SystemWrite(GetDataTag("AxleRatioBackgroundColorTag"), 
                        !testResult.compare(testPass) ? string("green") : string("red"));
            SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                                     "FrontToRearSpeedDiff", CreateMessage(buff, sizeof(buff), "%.2f", axleDiff), unitsMPH);
            RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
            // Wait for Zero Speed
            SetStartTime();
            CheckZeroSpeed();
        }
        else
        {
            Log(LOG_DEV_DATA, "Vehicle not equipped with AWD, skipping axle ratio test");
            testResult = testSkip;
        }
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping test step CheckAxleRatio");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaTcaseTC::CheckAxleRatio() - Exit");
    return testResult;
}
