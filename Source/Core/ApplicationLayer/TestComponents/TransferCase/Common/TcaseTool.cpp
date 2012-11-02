//******************************************************************************
// Description:
//  Transfer Case Tool Kit for testing Transfer Case Test Components.
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
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Common/TcaseTool.cpp $
// 
// 10    10/30/07 3:17p Rwiersem
// Changes for the 07103101 core release:
// 
// - Removed a commented out code section.
// - Moved single lines that went off the screen to multiple lines.
// 
// 9     4/13/06 3:27a Cward
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 2     3/20/06 10:48a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 8     2/15/06 7:25p Cward
// Added New Test steps
//
// 7     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 6     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 5     8/05/04 2:38a Cward
// Refactored Template class includes (App layer only), Made Module
// Interfaces and Protocol Filters extensible.
//
// 4     5/25/04 3:58p Cward
// Updated Axle Ratio Test to allow multiple attempts prior to failing
// test
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 2     11/29/03 1:27p Derickso
// Latest updates from testing at JTE and merging with Korea and Ford.
//
// 1     10/28/03 7:54a Derickso
// Created.
//
// 9     9/30/03 7:19p Bbarrett
// Change and update methods
//
// 8     9/29/03 6:03p Bbarrett
// Continue development
//
// 7     9/17/03 2:59p Bbarrett
// Add get ratio (samples / time).
//
// 6     9/11/03 3:44p Bbarrett
// Clean up code. Change JavaDoc comments.  Version 1.0 done.  Not
// compiled yet.
//
// 5     9/10/03 1:18p Bbarrett
// Add comments.
//
// 4     9/09/03 4:41p Bbarrett
// Add axle ratio methods. Not complete yet
//
// 3     8/29/03 2:58p Bbarrett
// Add more shift test. reconfigure test.  Not complete yet.
//
// 2     8/25/03 5:21p Bbarrett
// Add in ShiftToLow method.
//
// 1     8/22/03 5:31p Bbarrett
// Initial work on Tcase
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericTC.h"
#include "TcaseTool.h"

TcaseTool::TcaseTool(GenericTC *tc)
{
    if (tc == NULL)
        throw BepException();
    m_component = tc;
}

TcaseTool::~TcaseTool()
{
    // reset the system tags that are specified in the component list
    m_component->ResetSystemTags();

    m_component->Log(LOG_FN_ENTRY, "Generic Tcase Tool Destructor complete\n");
}

string TcaseTool::CheckTcaseShift (void)
{
    // variables used for reporting test step results
    string testDescription(m_component->GetTestStepName()); //default
    string testResult(testFail);        //Default test to fail
    string testStatus(testFail);        //Default check to fail
    string testResultCode("0000");      //Default test code to NULL

    //Log enter
    m_component->Log(LOG_FN_ENTRY, "%s: ENTER\n", m_component->GetTestStepName().c_str());
    //determine if step needs to be ran
    if (m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_DEV_DATA, "%s: Skip Test\n", m_component->GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {
        //Verify shift occurs at desired speed and or transmission gear
        testStatus = PreconditionsForShift();
        if (testStatus == testPass)
        {   //Shift Tcase - if vehicle conditions are correct
            string verify = m_component->GetParameter(m_component->GetTestStepName()+"VerifyMethod");
            // If manual transmission, have operator apply clutch
            bool clutchPromptDisplayed = false;
            if(!m_component->SystemRead(m_component->GetDataTag("Transmission")).compare(m_component->GetDataTag("ManualTransType")))
            {   // Display apply clutch prompt
                m_component->DisplayPrompt(m_component->GetPromptBox("ApplyClutch"), m_component->GetPrompt("ApplyClutch"));
                clutchPromptDisplayed = true;
            }
            // Determine how to shift the t-case
            if ((verify != "Driver") && (verify != "NotRequired") && (verify != "Prompt"))
            {   // Need to call the ShiftTcase method
                testStatus = ShiftTcase();
            }
            else
            {   // Operator will be prompted in VerifyTcaseShift()
                testStatus = testPass;
            }
            // Check the result of the ShiftTcase method
            if (testStatus == testPass)
            {   //Verify shift happens
                testStatus = VerifyTcaseShift(m_component->GetParameter(m_component->GetTestStepName()+"VerifyMethod"));
                if (testStatus == testPass)
                {//Pass - Shift and verify Tcase to desired position
                    m_component->Log(LOG_DEV_DATA, "%s: Test Pass\n", m_component->GetTestStepName().c_str());
                    testResult = testPass;      //Return PASS
                    testDescription = m_component->GetTestStepName();
                    testResultCode ="0000";     //Default test code to NULL
                }//Fail verify Tcase shift
                else
                {   //Fail - Verify Tcase shifts to desired position
                    m_component->Log(LOG_ERRORS, "%s: Fail Verify Tcase Shift\n", m_component->GetTestStepName().c_str());
                    testStatus = testFail;
                    testDescription = m_component->GetFaultDescription(m_component->GetTestStepName()+"VerifyShiftFailure");
                    testResultCode = m_component->GetFaultCode(m_component->GetTestStepName()+"VerifyShiftFailure");
                }
            }
            else
            {   //Fail - shift Tcase to desired position
                m_component->Log(LOG_ERRORS, "%s: Fail to Shift Tcase\n", m_component->GetTestStepName().c_str());
                testStatus = testFail;
                testDescription = m_component->GetFaultDescription(m_component->GetTestStepName()+"ShiftTcaseFailure");
                testResultCode = m_component->GetFaultCode(m_component->GetTestStepName()+"ShiftTcaseFailure");
            }
            // Check if the Apply Clutch prompt should be removed
            if(clutchPromptDisplayed)
            {
                m_component->RemovePrompt(m_component->GetPromptBox("ApplyClutch"), m_component->GetPrompt("ApplyClutch"));
            }
        }
        else
        {   //Fail - get vehicle ready for Tcase shift
            m_component->Log(LOG_ERRORS, "%s: Fail to Shift Tcase\n", m_component->GetTestStepName().c_str());
            testStatus = testFail;
            testDescription = m_component->GetFaultDescription(m_component->GetTestStepName()+"VehicleConditionsFailure");
            testResultCode = m_component->GetFaultCode(m_component->GetTestStepName()+"VehicleConditionsFailure");
        }

        //Report test result to sever
        m_component->SendTestResult(testResult, testDescription, testResultCode);
    }

    //Log exit
    m_component->Log(LOG_FN_ENTRY, "%s: Exit\n", m_component->GetTestStepName().c_str());
    //return result to test step sequence
    return(testResult);
}

string TcaseTool:: PreconditionsForShift ( void )
{
    string testStatus(testPass);        //testing status - default to Pass
    string throttle("NotRequired");     //Desired throttle - default Skip
    string transGear("NotRequired");    //Desired transmission Gear - default Skip
    string engineStatus("NotRequired"); //Desired engine status - default Skip
    string keyStatus("NotRequired");    //Desired key status - default Skip
    float shiftSpeed = 0.0;             //Desired vehicle speed - default to 0.0

    m_component->Log(LOG_FN_ENTRY, "%s: Enter PreConditions\n", m_component->GetTestStepName().c_str());

    //set Minimum desired test speed or '0' - ZeroSpeed
    shiftSpeed = m_component->GetParameterFloat("MinimumShiftSpeed");
    //Display testing Tcase or whatever..
    m_component->DisplayPrompt(m_component->GetPromptBox("TcaseTesting"), m_component->GetPrompt("TcaseTesting"), 
                               m_component->GetPromptPriority("TcaseTesting"));
    testStatus = CheckVehicleSpeed();
    if (testStatus != testPass)
    {
        m_component->Log(LOG_ERRORS, "%s: Fail TcaseShift get to Speed %3.2f\n", m_component->GetTestStepName().c_str(), shiftSpeed);
        testStatus = testFail;
    }
    else
    {
        //check if Engine needs to be Running or Off
        engineStatus = m_component->GetParameter(m_component->GetTestStepName() + "EngineRunning");
        if ((engineStatus != "NotRequired") && (engineStatus != "Skip"))
        {   //Check for Engine On /Off
            testStatus = EngineOnOff(engineStatus);
            if (testStatus != testPass)
            {   //Fail engine On/Off check
                testStatus = testFail;
                m_component->Log(LOG_ERRORS, "%s: Fail Engine %s check\n", m_component->GetTestStepName().c_str(), engineStatus.c_str());
            }
        }
        else
        {   //Skip engine On/Off check
            m_component->Log(LOG_DEV_DATA, "%s: Skip Check Engine %s\n", m_component->GetTestStepName().c_str(), engineStatus.c_str());
        }
        //if engine is in desired at desired state
        if (testStatus == testPass)
        {   //check if Key needs to be On/Off
            keyStatus = m_component->GetParameter(m_component->GetTestStepName() + "KeyPosition");
            if ((keyStatus != "NotRequired") && (keyStatus != "Skip"))
            {   //Check for key On/Off
                testStatus = TurnKeyOnOff(keyStatus);
                if (testStatus != testPass)
                {   //Fail Key On /Off check
                    testStatus = testFail;
                    m_component->Log(LOG_ERRORS, "%s: Fail Key %s check\n", m_component->GetTestStepName().c_str(), keyStatus.c_str());
                }
                else
                {
                    testStatus = testPass;
                }
            }
            else
            {   //Skip key On/Off check
                testStatus = testPass;
                m_component->Log(LOG_DEV_DATA, "%s: Skip Check Key %s\n", m_component->GetTestStepName().c_str(), keyStatus.c_str());
            }
        }
        // If engine and ignition is in desired position / state
        if (testStatus == testPass)
        {   //check if a specific transmission gear is required for Tcase shift
            transGear = m_component->GetParameter(m_component->GetTestStepName() + "TransmissionGear");
            if ((transGear != "NotRequired") && (transGear != "Skip"))
            {   //shift tranmsission
                testStatus = ShiftTransmission(transGear);
                if (testStatus != testPass)
                {
                    m_component->Log(LOG_ERRORS, "%s: Fail  Transmission shift %s\n", m_component->GetTestStepName().c_str(), transGear.c_str());
                    testStatus = testFail;
                }
            }
            else
            {
                m_component->Log(LOG_DEV_DATA, "%s: Skip Check Transmission Gear %s\n", m_component->GetTestStepName().c_str(), transGear.c_str());
            }
        }
        //if Transmission is in desired Gear
        if (testStatus == testPass)
        {   //check if a specific throttle positon is required for Tcase shift
            throttle = m_component->GetParameter(m_component->GetTestStepName() + "ThrottlePosition");
            if ((throttle != "NotRequired") && (throttle != "Skip"))
            {
                testStatus = CheckThrottlePosition(throttle);
                if (testStatus != testPass)
                {
                    m_component->Log(LOG_ERRORS, "%s: Fail Throttle check %s\n", m_component->GetTestStepName().c_str(), throttle.c_str());
                    testStatus = testFail;
                }
            }
            else
            {
                m_component->Log(LOG_DEV_DATA, "%s: Skip Check Throttle %s\n", m_component->GetTestStepName().c_str(), throttle.c_str());
            }
        }
    }
    //Remove prompt
    m_component->RemovePrompt(m_component->GetPromptBox("TcaseTesting"), m_component->GetPrompt("TcaseTesting"), 
                              m_component->GetPromptPriority("TcaseTesting"));
    //Log exit
    m_component->Log(LOG_FN_ENTRY, "%s: Exit PreConditions\n", m_component->GetTestStepName().c_str());
    //return result to test step sequence
    return(testStatus);
}

string TcaseTool::CheckVehicleSpeed ( void )
{
    string testStatus(testFail);     //track get speed result

    //Log Enter
    m_component->Log(LOG_DEV_DATA, "TcaseTool::CheckVehicleSpeed, test step %s, - Enter\n",
                     m_component->GetTestStepName().c_str());
    //set Minimum desired test speed or '0' - ZeroSpeed
    float vehicleSpeed = m_component->GetParameterFloat(m_component->GetTestStepName() + "MinimumShiftSpeed");
    //Check vehcile is at speed - Prompt to correct speed
    if (vehicleSpeed <= 0)
    {   //Verify vehcile is at zero Speed
        if (m_component->CheckZeroSpeed() == true)
        {   //vehicle at zero speed
            m_component->Log(LOG_DEV_DATA, "%s: TcaseTool::CheckVehicleSpeed Vehicle at Zero Speed\n", m_component->GetTestStepName().c_str());
            testStatus = testPass;
        }
        else
        {
            //Fail vehicle at zero speed
            m_component->Log(LOG_ERRORS, "%s: Fail TcaseTool::CheckVehicleSpeed Brake to Stop\n", m_component->GetTestStepName().c_str());
            testStatus = testFail;
        }
    }
    else
    {   //Prompt and verify vehicle is at speed
        testStatus = m_component->WaitAndPromptForSpeed(m_component->GetTestStepInfoFloat("MinimumSpeed"),
                                                        m_component->GetTestStepInfoFloat("MaximumSpeed"),
                                                        m_component->GetTestStepInfoInt("SpeedSampleDelay"));

        if (testStatus == testPass)
        {   //Fail - get to speed
            m_component->Log(LOG_DEV_DATA, "%s: TcaseTool::CheckVehicleSpeed Vehcile at speed\n", m_component->GetTestStepName().c_str());
            testStatus = testPass;
        }
        else
        {   //Fail - get to speed
            m_component->Log(LOG_ERRORS, "%s: TcaseTool::CheckVehicleSpeed Fail Tcase shift speed\n", m_component->GetTestStepName().c_str());
            testStatus = testFail;
        }
    }
    //Log exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::CheckVehicleSpeed, test step %s, - Exit - Returning: %s\n",
                     m_component->GetTestStepName().c_str(), testStatus.c_str());
    return testStatus;
}

string TcaseTool::ShiftTransmission ( const string transGear)
{
    string testResult(testFail);        //Default test to fail

    //Log Enter
    m_component->Log(LOG_DEV_DATA, "TcaseTool::ShiftTransmission, test step %s, - Enter\n",
                     m_component->GetTestStepName().c_str());
    //check to see what is the prompt hold time
    INT32 transDelay = m_component->GetTestStepInfoInt("TransmissionMessageDelay");
    //Hold prompt for time and exit
    m_component->Log(LOG_DEV_DATA, "%s: TcaseTool::ShiftTransmission No verify - prompt for %d msec\n", 
                     m_component->GetTestStepName().c_str(), transDelay);
    m_component->DisplayPrompt(m_component->GetPromptBox(m_component->GetTestStepName() + "ShiftTransmission"),
                              m_component->GetPrompt(m_component->GetTestStepName() + "ShiftTransmission"),
                              m_component->GetPromptPriority(m_component->GetTestStepName() + "ShiftTransmission"));
    // Determine if the brake should be applied for this shift
    if (m_component->GetParameterBool(m_component->GetTestStepName() + "ApplyBrakeShiftTransmission"))
    {   // Prompt operator to apply brake pedal
        m_component->DisplayPrompt(m_component->GetPromptBox("ApplyBrakePedal"), m_component->GetPrompt("ApplyBrakePedal"), 
                                   m_component->GetPromptPriority("ApplyBrakePedal"));
    }
    BposSleep(transDelay);
    if (m_component->StatusCheck() == BEP_STATUS_SUCCESS)
    {
        testResult = testPass;
    }
    else
    {
        testResult = ConvertStatusToResponse(m_component->StatusCheck());
    }
    //remove prompt
    m_component->RemovePrompt(m_component->GetPromptBox(m_component->GetTestStepName() + "ShiftTransmission"),
                              m_component->GetPrompt(m_component->GetTestStepName() + "ShiftTransmission"),
                              m_component->GetPromptPriority(m_component->GetTestStepName() + "ShiftTransmission"));
    //Log exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::ShiftTransmission, test step %s, - Exit - Returning: %s\n",
                     m_component->GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::TurnKeyOnOff (const string keyPosition)
{
    string testResult(BEP_TESTING_STATUS);
    //Log Enter
    m_component->Log(LOG_DEV_DATA, "TcaseTool::TurnKeyOnOff, test step %s, - Enter", m_component->GetTestStepName().c_str());
    // Display the prompt for the specified amount of time
    string promptTag(m_component->GetTestStepName() + "KeyOnOff");
    BEP_STATUS_TYPE status = m_component->DisplayTimedPrompt(m_component->GetPrompt(promptTag),
                                                             m_component->GetPromptBox(promptTag),
                                                             m_component->GetPromptPriority(promptTag),
                                                             m_component->GetTestStepInfoInt("KeyOnOffMessageDelay"));
    testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    //Log Exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::TurnKeyOnOff, test step %s, - Exit - Returning: %s",
                     m_component->GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::EngineOnOff (const string engineRun)
{
    string testResult(BEP_TESTING_STATUS);
    //Log Enter
    m_component->Log(LOG_DEV_DATA, "TcaseTool::EngineOnOff, test step %s, - Enter", m_component->GetTestStepName().c_str());
    // Display the prompt for the specified amount of time
    string promptTag(m_component->GetTestStepName() + "EngineOnOff");
    BEP_STATUS_TYPE status = m_component->DisplayTimedPrompt(m_component->GetPrompt(promptTag),
                                                             m_component->GetPromptBox(promptTag),
                                                             m_component->GetPromptPriority(promptTag),
                                                             m_component->GetTestStepInfoInt("EngineOnOffMessageDelay"));
    testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    //Log Exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::EngineOnOff, test step %s, - Exit - Returning: %s",
                     m_component->GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::CheckThrottlePosition (const string throttlePosition )
{
    string testResult(BEP_TESTING_STATUS);
    //Log Enter
    m_component->Log(LOG_DEV_DATA, "TcaseTool::CheckThrottlePosition, test step %s, - Enter",
                     m_component->GetTestStepName().c_str());
    // Display the prompt for the specified amount of time
    string promptTag(m_component->GetTestStepName() + "ThrottlePosition");
    BEP_STATUS_TYPE status = m_component->DisplayTimedPrompt(m_component->GetPrompt(promptTag),
                                                             m_component->GetPromptBox(promptTag),
                                                             m_component->GetPromptPriority(promptTag),
                                                             m_component->GetTestStepInfoInt("ThrottleMessageDelay"));
    testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    //Log Exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::CheckThrottlePosition, test step %s, - Exit - Returning: %s",
                     m_component->GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::ShiftTcase ( void )
{
    string testResult(testFail);        //Default test to fail
    //Log Tcase
    m_component->Log(LOG_DEV_DATA, "%s: TcaseTool::ShiftTcase: Enter", m_component->GetTestStepName().c_str());
    // Display the prompt for the specified amount of time
    string promptTag(m_component->GetTestStepName() + "ShiftTcase");
    BEP_STATUS_TYPE status = m_component->DisplayTimedPrompt(m_component->GetPrompt(promptTag),
                                                             m_component->GetPromptBox(promptTag),
                                                             m_component->GetPromptPriority(promptTag),
                                                             m_component->GetTestStepInfoInt("CommandMessageDelay"),
                                                             m_component->GetTestStepInfo("TcaseGear"));
    testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    //Log Exit
    m_component->Log(LOG_DEV_DATA, "TcaseTool::ShiftTcase test step %s, - Exit - Returning: %s\n",
                     m_component->GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::VerifyTcaseShift (const string &verifyMethod)
{
    string testResult(testFail);        //Default test to fail
    //Log Tcase verify method
    m_component->Log(LOG_FN_ENTRY, "%s: Tcase Use < %s > to Verify\n", 
                     m_component->GetTestStepName().c_str(), verifyMethod.c_str());
    //Verify Tcase shifted
    if (verifyMethod == "Driver")
    {
        //Prompt Driver to use pendant and verify shift happened
        testResult = TcaseCheckDriver();
    }
    else if (verifyMethod == "Prompt")
    { //Prompt Driver to Shift, no verification
        testResult = TcasePromptDriver();
    }
    else
    {   //Undefined verification method
        m_component->Log(LOG_ERRORS, "%s: Undefined Tcase Check type < %s >\n", 
                         m_component->GetTestStepName().c_str(), verifyMethod.c_str());
        testResult = testFail;
    }
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::TcaseCheckDriver(void)
{
    string testResult(testFail);    // Used to report the result from method
    string status(BEP_NO_DATA);     // Used to hold return value from OperatorYesNo()

    // Log method entry
    m_component->Log(LOG_FN_ENTRY, "%s:TcaseCheckDriver Enter\n", m_component->GetTestStepName().c_str());

    //prompt shift tcase
    m_component->DisplayPrompt(m_component->GetPromptBox(m_component->GetTestStepName() + "ShiftTcase"),
                               m_component->GetPrompt(m_component->GetTestStepName() + "ShiftTcase"),
                               m_component->GetPromptPriority(m_component->GetTestStepName() + "ShiftTcase"),
                               "White", m_component->GetTestStepInfo("TcaseGear"));

    // Get Yes or No response from operator on pendant
    status = m_component->OperatorYesNo();

    // Setup testResults variable
    testResult = status == "Yes" ? testPass : status;

    //remove prompt
    m_component->RemovePrompt(m_component->GetPromptBox(m_component->GetTestStepName() + "ShiftTcase"),
                              m_component->GetPrompt(m_component->GetTestStepName() + "ShiftTcase"),
                              m_component->GetPromptPriority(m_component->GetTestStepName() + "ShiftTcase"));

    // Log method exit
    m_component->Log(LOG_FN_ENTRY, "%s:TcaseCheckDriver Exit\n", m_component->GetTestStepName().c_str());

    //return from method
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::TcasePromptDriver (void)
{//prompt shift tcase
    string testResult(BEP_TESTING_STATUS);
    string promptTag(m_component->GetTestStepName() + "ShiftTcase");
    //Prompt Shift
    BEP_STATUS_TYPE status = m_component->DisplayTimedPrompt(m_component->GetPrompt(promptTag),
                                                             m_component->GetPromptBox(promptTag), 
                                                             m_component->GetPromptPriority(promptTag),
                     m_component->GetTestStepInfoInt("TcaseMessageDelay"));
    testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    //return from method
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::CheckAxleRatio ( void )
{
    // variables used for reporting test step results
    string testDescription(m_component->GetTestStepName());
    string testResult(testFail);        //Default test to fail
    string testStatus(testPass);        //Default check to fail
    INT32 motorStatus(BEP_STATUS_FAILURE);      //Default ICM read to fail
    string testResultCode("0000");      //Default test code to NULL
    char   tempCharArray[32];           // used to convert a floating number into a string
    string MinimumAxleRatioLabel("MinimumAxleRatio");
    string MaximumAxleRatioLabel("MaximumAxleRatio");
    string ActualAxleRatioLabel("ActualAxleRatio");
    bool disEngaged = false;

    // variables used to determine required speed
    float axleRatio = 0.0;              //used to store ratio from GetAxleRatio

    //log Enter test
    m_component->Log(LOG_FN_ENTRY, "%s: ENTER\n", m_component->GetTestStepName().c_str());

    if (m_component->ShortCircuitTestStep())
    {   //Log Skip step
        m_component->Log(LOG_DEV_DATA, "%s: Skip Test\n", m_component->GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {
        // set the machine into the correct conditions for testing
        motorStatus = Disengage();
        testStatus = motorStatus == BEP_STATUS_SUCCESS ? testPass : ConvertStatusToResponse(motorStatus);
        if (testStatus == testPass)
        {
            disEngaged = true;
            m_component->Log(LOG_DEV_DATA, "%s: Motors Dis-Engaged\n", m_component->GetTestStepName().c_str());
            //Prompt and verify vehcile is at speed
            float minimumSpeed = m_component->GetTestStepInfoFloat("MinimumSpeed");
            GenericTC::Direction dir = GenericTC::UP;
            string promptTag = "ShiftToDrive";
            if (m_component->GetParameter("AxleRatioDirection") == "Reverse")
            {   // Set up to perform test in reverse
                dir = GenericTC::DOWN;
                minimumSpeed = -minimumSpeed;
                promptTag = "ShiftToReverse";
            }
            // Set up the green band for the operator
            string greenBand = m_component->GetTestStepInfo("MinimumSpeed") + " " + m_component->GetTestStepInfo("MaximumSpeed");
            m_component->SystemWrite(m_component->GetDataTag("TargetSpeed"), greenBand);
            // Allow multiple attempts to pass the axle ratio test
            INT32 currentAttempt = 1;
            do
            {
                // Display the prompts for the operator
                m_component->DisplayPrompt(m_component->GetPromptBox(promptTag), m_component->GetPrompt(promptTag), 
                                           m_component->GetPromptPriority(promptTag));
                m_component->DisplayPrompt(m_component->GetPromptBox("AccelerateToTargetSpeed"), 
                                           m_component->GetPrompt("AccelerateToTargetSpeed"), 
                                           m_component->GetPromptPriority("AccelerateToTargetSpeed"));
                // Wait for the target speed
                BEP_STATUS_TYPE speedStatus = m_component->WaitForTargetSpeed(minimumSpeed, dir, 
                                                                              m_component->GetTestStepInfoInt("SpeedSampleDelay"));
                // Remove the prompts
                m_component->RemovePrompt(m_component->GetPromptBox(promptTag), 
                                          m_component->GetPrompt(promptTag), m_component->GetPromptPriority(promptTag));
                m_component->RemovePrompt(m_component->GetPromptBox("AccelerateToTargetSpeed"), 
                                          m_component->GetPrompt("AccelerateToTargetSpeed"), 
                                          m_component->GetPromptPriority("AccelerateToTargetSpeed"));
                // do axle ratio test
                if (BEP_STATUS_SUCCESS == speedStatus)
                {
                    //Prompt the operater to maintain speed
                    m_component->DisplayPrompt(m_component->GetPromptBox("MaintainSpeed"), 
                                               m_component->GetPrompt("MaintainSpeed"), m_component->GetPromptPriority("MaintainSpeed"));
                    //get axle ratio
                    testStatus = GetAxleRatio(axleRatio);
                    if (testStatus == testPass)
                    {
                        //Check axle ratio against parameters
                        if (testStatus == testPass)
                        {
                            if ((m_component->GetParameterFloat("AxleRatioMinimum") < axleRatio) && 
                                (m_component->GetParameterFloat("AxleRatioMaximum") > axleRatio))
                            {
                                testResult = testPass;
                                m_component->Log(LOG_DEV_DATA, "%s: Axle ratio Pass\n", m_component->GetTestStepName().c_str());
                            }
                            else
                            {
                                //Fail - axle ratio out of limits
                                m_component->Log(LOG_ERRORS, "%s: Fail Axle Ratio\n", m_component->GetTestStepName().c_str());
                                testResult = testFail;
                                testDescription = m_component->GetFaultDescription("FailRatio");
                                testResultCode = m_component->GetFaultCode("FailRatio");
                            }

                            //Log ratio
                            m_component->Log(LOG_DEV_DATA, "%s: min:%.5f  ratio: %.5f  max:%.5f\n", m_component->GetTestStepName().c_str(),
                                             m_component->GetParameterFloat("AxleRatioMinimum"),
                                             axleRatio,m_component->GetParameterFloat("AxleRatioMaximum"));
                        }
                    }
                    else
                    {
                        //Fail - read axle ratio
                        m_component->Log(LOG_ERRORS, "%s: Fail get Axle Ratio\n", m_component->GetTestStepName().c_str());
                        testResult = testFail;
                        testDescription = m_component->GetFaultDescription("ReadRatio");
                        testResultCode = m_component->GetFaultCode("ReadRatio");
                    }
                    // remove the prompts
                    m_component->RemovePrompt(m_component->GetPromptBox("MaintainSpeed"), 
                                              m_component->GetPrompt("MaintainSpeed"), m_component->GetPromptPriority("MaintainSpeed"));
                }
                else
                {   //Timeout - waiting for test speed
                    m_component->Log(LOG_ERRORS, "%s: Timeout Waiting for Target Speed\n", m_component->GetTestStepName().c_str());
                    testResult = testTimeout;
                    testDescription = m_component->GetFaultDescription("TargetSpeedTimeout");
                    testResultCode = m_component->GetFaultCode("TargetSpeedTimeout");
                }
                // If not pass, get to zero speed to check again
                if (testPass != testResult) m_component->CheckZeroSpeed();
            } while((m_component->StatusCheck() == BEP_STATUS_SUCCESS) && (testPass != testResult) &&
                    (currentAttempt++ < m_component->GetParameterInt("MaximumAxleRatioAttempts")));
            // remove the speed target from the gauge
            m_component->SystemWrite(m_component->GetDataTag("TargetSpeed"), string("0 0"));
        }
        else
        {   //Fail - disable motor and declutch
            m_component->Log(LOG_ERRORS, "%s: Fail to DisEngage Motors\n", m_component->GetTestStepName().c_str());
            testResult = testFail;
            testDescription = m_component->GetFaultDescription("DisengageMotor");
            testResultCode = m_component->GetFaultCode("DisengageMotor");
        }

        // Only wait for zero speed if specified
        if(!m_component->GetParameterBool("AxleRatioBrakeToStopNotRequired"))
        {
        if (m_component->CheckZeroSpeed() == true)
        {   //vehicle at zero speed
            m_component->Log(LOG_DEV_DATA, "%s: Vehicle at Zero Speed\n", m_component->GetTestStepName().c_str());
            if (disEngaged == true)
            {
                m_component->Log(LOG_DEV_DATA, "%s: Motors Were Dis-Engaged\n", m_component->GetTestStepName().c_str());
            }
            // set the machine into the correct conditions for testing
            motorStatus = Engage();
            testStatus = motorStatus == BEP_STATUS_SUCCESS ? testPass : ConvertStatusToResponse(motorStatus);
            if (testStatus == testPass)
            {
                disEngaged = false;
                m_component->Log(LOG_DEV_DATA, "%s: Motors Engaged\n", m_component->GetTestStepName().c_str());
            }
            else
            {
                m_component->Log(LOG_ERRORS, "%s: Fail to Engage Motors\n", m_component->GetTestStepName().c_str());
            }
        }
        else
        {
            if (testResult != testPass)
            {
                //Timeout - waiting for zero speed - Report for entire test results and sub result since
                    //      this is the only failure in current test step
                m_component->Log(LOG_ERRORS, "%s: Timeout Waiting for Zero Speed\n", m_component->GetTestStepName().c_str());
                testResult = testTimeout;
                testDescription = m_component->GetFaultDescription("ZeroSpeed");
                testResultCode = m_component->GetFaultCode("ZeroSpeed");
                m_component->SendSubtestResult("AxleRatioZeroSpeed", testResult, testDescription, testResultCode);
            }
            else
            {
                // Timeout - waiting for zero speed - reporting only for subtest results to prevent
                    //      overwriting other failure
                testResultCode = "0000";
                testDescription = (m_component->GetTestStepName());
                m_component->SendSubtestResult("AxleRatioZeroSpeed", testResult, testDescription, testResultCode);
            }
        }
        }

        // Place numerical data from test into corresponding result reporting variables
        MinimumAxleRatioLabel = CreateMessage(tempCharArray, sizeof(tempCharArray), "%03.5f", m_component->GetParameterFloat("AxleRatioMinimum"));
        MaximumAxleRatioLabel = CreateMessage(tempCharArray, sizeof(tempCharArray), "%03.5f", m_component->GetParameterFloat("AxleRatioMaximum"));
        ActualAxleRatioLabel = CreateMessage(tempCharArray, sizeof(tempCharArray), "%03.5f", axleRatio);
        //Report test result to sever
        m_component->SendTestResultWithDetail(testResult, testDescription, testResultCode,"AxleRatioMin",
                                              MinimumAxleRatioLabel,"%","AxleRatioMax",
                                              MaximumAxleRatioLabel, "%", "ActualAxleRatio", ActualAxleRatioLabel, "%");
        m_component->SystemWrite("AxleRatioValue", axleRatio);
        string bgColor = !testResult.compare(testPass) ? "Green" : "Red";
        m_component->Log(LOG_DEV_DATA, "Setting axle ratio value color to %s", bgColor.c_str());
        m_component->SystemWrite("AxleRatioValueBGColor", bgColor);
    }
    //Log exit
    m_component->Log(LOG_FN_ENTRY, "%s: Exit\n", m_component->GetTestStepName().c_str());
    //return result to test step sequence
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::CheckAxleBalance( void )
{   // Variables used for reporting test step results
    string testResult = testPass;
    string testDescription = m_component->GetTestStepInfo("Description");
    string testResultCode = "0000";
    float frontSpeed       = 0.0;  //front axle speed
    float rearSpeed        = 0.0;  //rear axle speed
    float driveAxleSpeed   = 0.0;  //driven axle speed
    float frontAxleDelta   = 0.0;  //front left wheel to right wheel speed delta
    float rearAxleDelta    = 0.0;  //rear left wheel to right wheel speed delta
    float frontToRearDelta = 0.0;  //front to rear axle speed delta
    string MaximumLeftToRightDeltaLable("MaximumLeftToRightDelta");
    string FrontLeftToRightDeltaLable("FrontLeftToRightDelta");
    string RearLeftToRightDeltaLable("RearLeftToRightDelta");
    string MaximumFrontToRearDeltaLable("MaximumFrontToRearDelta");
    string FrontToRearDeltaLable("FrontToRearDelta");
    string speedRange = "0 0";         // Used to set the green band
    string eraseSpeedRange = "0 0";
    char  tempCharArray[32];           //used to convert a floating number into a string
    BEP_STATUS_TYPE machineStatus = BEP_STATUS_ERROR;
    WHEELINFO rollerSpeeds;            //Verify Axle speeds are with in defined delta

    //Log enter
    m_component->Log(LOG_FN_ENTRY, "%s: ENTER TcaseTool::CheckAxleBalance() \n", m_component->GetTestStepName().c_str());
    //determine if step needs to be ran
    if (m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_DEV_DATA, "%s: Skip TcaseTool::CheckAxleBalance() Test\n", m_component->GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {   // Paramters used to calculate ratio
        float maxFrontToRearDeltaPercent;
        float maxFrontToRearDelta;
        bool  usePercentageDelta = false;
        if(0 == (maxFrontToRearDeltaPercent = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxFrontToRearDeltaPercent"))))
        {
            // we're using an absolute speed delta
            maxFrontToRearDelta = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxFrontToRearDelta"));
        }
        else
        {
            // we're using a percentage speed delta
            usePercentageDelta = true; 
        } 
        float maxLeftToRightDelta = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxLeftToRightDelta"));
        INT32 targetSpeed = (m_component->GetParameterInt(m_component->GetTestStepName()+"TargetSpeed"));
        INT32 sampleDelay = m_component->GetTestStepInfoInt("ScanDelay");
        bool testAbortOnFail = m_component->GetTestStepInfoBool("AbortOnFail");

        // set up string for passing proper speed range
        speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray), "%d %d",
                                   (m_component->GetParameterInt(m_component->GetTestStepName()+"MinimumSpeed")),
                                   (m_component->GetParameterInt(m_component->GetTestStepName()+"MaximumSpeed")));
        //prompt to speed
        m_component->DisplayPrompt(m_component->GetPromptBox("ShiftToDrive"),
                                   m_component->GetPrompt("ShiftToDrive"),
                                   m_component->GetPromptPriority("ShiftToDrive"));
        m_component->DisplayPrompt(m_component->GetPromptBox("AccelerateToTargetSpeed"),
                                   m_component->GetPrompt("AccelerateToTargetSpeed"),
                                   m_component->GetPromptPriority("AccelerateToTargetSpeed"));
        // update the target green bands
        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), speedRange);
        // determine the vehicle drive wheel
        m_component->m_driveAxle = (m_component->ReadSubscribeData(m_component->GetDataTag("DriveAxle")) ==
                                    m_component->GetDataTag("Rear")) ? m_component->RWD : m_component->FWD;
        //set testResult to Pass so loop goes
        testResult == testPass;
        //Monitor Axle speeds and Verify driven to Non-driven Axle delta is within range
        do
        {
            machineStatus = m_component->GetWheelSpeeds(rollerSpeeds);
            // Check status
            if (BEP_STATUS_SUCCESS == machineStatus)
            {
                m_component->Log(LOG_DEV_DATA, "%s: Read roller speeds -- LF: %.2f, RF: %.2f, LR: %.2f, RR: %.2f\n",
                                 m_component->GetTestStepName().c_str(), rollerSpeeds.lfWheel, rollerSpeeds.rfWheel,
                                 rollerSpeeds.lrWheel, rollerSpeeds.rrWheel);
                // Average the speeds to get the roller speed
                frontSpeed = (rollerSpeeds.lfWheel + rollerSpeeds.rfWheel) / 2.0;
                rearSpeed =  (rollerSpeeds.lrWheel + rollerSpeeds.rrWheel) / 2.0;
                if (m_component->m_driveAxle == m_component->RWD)
                    driveAxleSpeed = rearSpeed;
                else
                    driveAxleSpeed = frontSpeed;
                if(!m_component->GetParameterBool(m_component->GetTestStepName()+"OnlyTestFrontRearRatio"))
                {
                //Check front Side to Side delta is in range
                frontAxleDelta = abs(rollerSpeeds.lfWheel - rollerSpeeds.rfWheel);
                //frontAxleDelta = (rollerSpeeds.lfWheel > rollerSpeeds.rfWheel) ? (rollerSpeeds.lfWheel - rollerSpeeds.rfWheel) :
                //                                                                 (rollerSpeeds.rfWheel > rollerSpeeds.lfWheel);
                if (frontAxleDelta > maxLeftToRightDelta)
                {   //Stop test - Delta out of range
                    testResult = testFail;
                    m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail Front LeftToRight Delta - %3.5f\n",
                                     m_component->GetTestStepName().c_str(), frontAxleDelta);
                }
                //Check rear Side to Side delta is in range
                rearAxleDelta =  abs(rollerSpeeds.lrWheel - rollerSpeeds.rrWheel);
                //rearAxleDelta =  (rollerSpeeds.lrWheel > rollerSpeeds.rrWheel) ? (rollerSpeeds.lrWheel - rollerSpeeds.rrWheel) :
                //                                                                 (rollerSpeeds.rrWheel > rollerSpeeds.lrWheel);
                if (rearAxleDelta > maxLeftToRightDelta)
                {   //Stop test - Delta out of range

                    testResult = testFail;
                    m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail Rear LeftToRight Delta - %3.5f\n",
                                     m_component->GetTestStepName().c_str(), rearAxleDelta);
                }
                }
                //Check Front to Rear delta is in range
                if(usePercentageDelta)
                {
                    // if we're using the percentage variable, calculate max deviation from rear axle speed
                    // otherwise absolute maxFrontToRearDeltat will be used
                    maxFrontToRearDelta = (maxFrontToRearDeltaPercent / 100) * rearSpeed;
                }
                frontToRearDelta = abs(frontSpeed - rearSpeed);
                //frontToRearDelta = (frontSpeed > rearSpeed) ? (frontSpeed - rearSpeed) :
                //                                              (rearSpeed - frontSpeed);
                if (frontToRearDelta > maxFrontToRearDelta)
                {   //Stop test - Delta out of range
                    testResult = testFail;
                    m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail FrontToRear Delta - %3.5f\n",
                                     m_component->GetTestStepName().c_str(), frontToRearDelta);
                }
                //delay before next read
                BposSleep(sampleDelay);
            }
            else
            {   // Error reading wheel speeds
                testResult = testFail;
                m_component->Log(LOG_ERRORS, "%s: Error reading Roll Speed - status: %s\n",
                                 m_component->GetTestStepName().c_str(), ConvertStatusToResponse(machineStatus).c_str());
            }
        }while ((m_component->StatusCheck() == BEP_STATUS_SUCCESS) && (driveAxleSpeed <= targetSpeed) &&
                (testResult == testPass) && m_component->TimeRemaining() && (BEP_STATUS_SUCCESS == machineStatus));
        // Check exit condition
        if (BEP_STATUS_SUCCESS != m_component->StatusCheck())
        {   //System not ready
            testResult = testFail;
            testResultCode = m_component->GetFaultCode("SystemStatus");
            testDescription = m_component->GetFaultDescription("SystemStatus");
            m_component->Log(LOG_ERRORS, "System error during %s - status: %s\n", m_component->GetTestStepName().c_str(),
                             ConvertStatusToResponse(m_component->StatusCheck()).c_str());
        }
        else if (machineStatus != BEP_STATUS_SUCCESS)
        {   // Bad comms with the module
            testResult = testFail;
            testResultCode = m_component->GetFaultCode("SystemReadFail");
            testDescription = m_component->GetFaultDescription("SystemReadFail");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance Error reading Roll Speed\n",m_component->GetTestStepName().c_str());
        }
        else if(!m_component->TimeRemaining() && (driveAxleSpeed <= targetSpeed))
        {   // Driver did not accelerate, FAIL him miserably!!!!
            testResult = testFail;
            testResultCode = m_component->GetFaultCode("SpeedFail");
            testDescription = m_component->GetFaultDescription("SpeedFail");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance - Failed to reach target speed - vehicle speed: %.2f, target speed: %d",
                             m_component->GetTestStepName().c_str(), driveAxleSpeed, targetSpeed);
        }
        else if (!m_component->TimeRemaining() && (testResult != testPass))
        {   // timeout
            testResult = testTimeout;
            testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Timeout");
            testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Timeout");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance Timeout waiting for target Speed\n", m_component->GetTestStepName().c_str());
        }
        else if (testResult == testPass)
        {
            testResult = testPass;
            testResultCode = "0000";
            testDescription = m_component->GetTestStepInfo("Description");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance OK\n", m_component->GetTestStepName().c_str());
        }
        else if (!testAbortOnFail && (testResult != testPass))
        {
            testResult = testFail;
            testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Fail");
            testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Fail");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance FAIL\n", m_component->GetTestStepName().c_str());
        }
        else
        {
            testResult = testAbort;
            testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Abort");
            testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Abort");
            m_component->SystemWrite(ABORT_DATA_TAG, 1);
            m_component->Log(LOG_ERRORS, "%s: AxleBalance set ABORT ALL test\n", m_component->GetTestStepName().c_str());
        }
        // remove the target green bands
        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), eraseSpeedRange);
        // remove the prompts
        m_component->RemovePrompt(m_component->GetPromptBox("ShiftToDrive"),
                                  m_component->GetPrompt("ShiftToDrive"),
                                  m_component->GetPromptPriority("ShiftToDrive"));
        m_component->RemovePrompt(m_component->GetPromptBox("AccelerateToTargetSpeed"),
                                  m_component->GetPrompt("AccelerateToTargetSpeed"),
                                  m_component->GetPromptPriority("AccelerateToTargetSpeed"));
        // Place numerical data from test into corresponding result reporting variables
        MaximumLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%f", maxLeftToRightDelta);
        FrontLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", frontAxleDelta);
        RearLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", rearAxleDelta);
        MaximumFrontToRearDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%f", maxFrontToRearDelta);
        FrontToRearDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", frontToRearDelta);
        //Report test result to sever
        m_component->SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                              "MaxLeftToRightWheelDelta",   MaximumLeftToRightDeltaLable, unitsMPH,
                                              "FrontLeftToRightWheelDelta", FrontLeftToRightDeltaLable,   unitsMPH,
                                              "RearLeftToRightWheelDelta",  RearLeftToRightDeltaLable,    unitsMPH,
                                              "MaxFrontToRearAxleDelta",    MaximumFrontToRearDeltaLable, unitsMPH,
                                              "FrontToRearAxleDelta",       FrontToRearDeltaLable,        unitsMPH);
    }
    //Log exit
    m_component->Log(LOG_FN_ENTRY, "%s: Exit TcaseTool::CheckAxleBalance()\n", m_component->GetTestStepName().c_str());
    //return result to test step sequence
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::WaitForAxleBalance( void )
{   // Variables used for reporting test step results
    string testResult = testPass;
    string testDescription = m_component->GetTestStepInfo("Description");
    string testResultCode = "0000";
    float frontSpeed       = 0.0;  //front axle speed
    float rearSpeed        = 0.0;  //rear axle speed
    float driveAxleSpeed   = 0.0;  //driven axle speed
    float frontAxleDelta   = 0.0;  //front left wheel to right wheel speed delta
    float rearAxleDelta    = 0.0;  //rear left wheel to right wheel speed delta
    float frontToRearDelta = 0.0;  //front to rear axle speed delta
    string MaximumLeftToRightDeltaLable("MaximumLeftToRightDelta");
    string FrontLeftToRightDeltaLable("FrontLeftToRightDelta");
    string RearLeftToRightDeltaLable("RearLeftToRightDelta");
    string MaximumFrontToRearDeltaLable("MaximumFrontToRearDelta");
    string FrontToRearDeltaLable("FrontToRearDelta");
    string speedRange = "0 0";         // Used to set the green band
    string eraseSpeedRange = "0 0";
    char  tempCharArray[32];           //used to convert a floating number into a string
    BEP_STATUS_TYPE machineStatus = BEP_STATUS_ERROR;
    WHEELINFO rollerSpeeds;            //Verify Axle speeds are with in defined delta
    INT32 waitForAxleBalanceTime = m_component->GetParameterInt("WaitForAxleBalanceTime");

    //Log enter
    m_component->Log(LOG_FN_ENTRY, "%s: ENTER TcaseTool::CheckAxleBalance() \n", m_component->GetTestStepName().c_str());
    //determine if step needs to be ran
    if(m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_DEV_DATA, "%s: Skip TcaseTool::CheckAxleBalance() Test\n", m_component->GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {   // Paramters used to calculate ratio
        float maxFrontToRearDeltaPercent;
        float maxFrontToRearDelta;
        bool  usePercentageDelta = false;
        if(0 == (maxFrontToRearDeltaPercent = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxFrontToRearDeltaPercent"))))
        {
            // we're using an absolute speed delta
            maxFrontToRearDelta = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxFrontToRearDelta"));
        }
        else
        {
            // we're using a percentage speed delta
            usePercentageDelta = true; 
        } 
        float maxLeftToRightDelta = (m_component->GetParameterFloat(m_component->GetTestStepName()+"MaxLeftToRightDelta"));
        INT32 targetSpeed = (m_component->GetParameterInt(m_component->GetTestStepName()+"TargetSpeed"));
        INT32 sampleDelay = m_component->GetTestStepInfoInt("ScanDelay");
        bool testAbortOnFail = m_component->GetTestStepInfoBool("AbortOnFail");
        //prompt to speed
        m_component->DisplayPrompt(m_component->GetPromptBox("ShiftToDrive"),
                                   m_component->GetPrompt("ShiftToDrive"),
                                   m_component->GetPromptPriority("ShiftToDrive"));
        //Accelerate to minimum speed
        testResult = m_component->AccelerateToTestSpeed(targetSpeed,speedRange,sampleDelay);
        if(testResult == testPass)
        {
            // prompt to maintain speed while 
            // set up string for passing proper speed range
            speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray), "%d %d",
                                       (m_component->GetParameterInt(m_component->GetTestStepName()+"MinimumSpeed")),
                                       (m_component->GetParameterInt(m_component->GetTestStepName()+"MaximumSpeed")));
            //prompt to speed
            m_component->DisplayPrompt(m_component->GetPromptBox("MaintainSpeed"),
                                       m_component->GetPrompt("MaintainSpeed"),
                                       m_component->GetPromptPriority("MaintainSpeed"));
            // update the target green bands
            m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), speedRange);
            // determine the vehicle drive wheel
            m_component->m_driveAxle = (m_component->ReadSubscribeData(m_component->GetDataTag("DriveAxle")) ==
                                        m_component->GetDataTag("Rear")) ? m_component->RWD : m_component->FWD;
            m_component->SetSecondaryStartTime();
            //monitoring axle balance - if this is within the test limit and speed is correct, pass
            do
            {
                machineStatus = m_component->GetWheelSpeeds(rollerSpeeds);
                testResult = testPass;
                // Check status
                if(BEP_STATUS_SUCCESS == machineStatus)
                {
                    m_component->Log(LOG_DEV_DATA, "%s: Read roller speeds -- LF: %.2f, RF: %.2f, LR: %.2f, RR: %.2f\n",
                                     m_component->GetTestStepName().c_str(), rollerSpeeds.lfWheel, rollerSpeeds.rfWheel,
                                     rollerSpeeds.lrWheel, rollerSpeeds.rrWheel);
                    // Average the speeds to get the roller speed
                    frontSpeed = (rollerSpeeds.lfWheel + rollerSpeeds.rfWheel) / 2.0;
                    rearSpeed =  (rollerSpeeds.lrWheel + rollerSpeeds.rrWheel) / 2.0;
                    if(m_component->m_driveAxle == m_component->RWD)
                        driveAxleSpeed = rearSpeed;
                    else
                        driveAxleSpeed = frontSpeed;
                    if(!m_component->GetParameterBool(m_component->GetTestStepName()+"OnlyTestFrontRearRatio"))
                    {
                        //Check front Side to Side delta is in range
                        frontAxleDelta = abs(rollerSpeeds.lfWheel - rollerSpeeds.rfWheel);
                        //frontAxleDelta = (rollerSpeeds.lfWheel > rollerSpeeds.rfWheel) ? (rollerSpeeds.lfWheel - rollerSpeeds.rfWheel) :
                        //                                                                 (rollerSpeeds.rfWheel - rollerSpeeds.lfWheel);
                        if(frontAxleDelta > maxLeftToRightDelta)
                        {   //Stop test - Delta out of range
                            testResult = testFail;
                            m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail Front LeftToRight Delta - %3.5f\n",
                                             m_component->GetTestStepName().c_str(), frontAxleDelta);
                        }
                        //Check rear Side to Side delta is in range
                        rearAxleDelta =  abs(rollerSpeeds.lrWheel - rollerSpeeds.rrWheel);
                        //rearAxleDelta =  (rollerSpeeds.lrWheel > rollerSpeeds.rrWheel) ? (rollerSpeeds.lrWheel - rollerSpeeds.rrWheel) :
                        //                                                                 (rollerSpeeds.rrWheel - rollerSpeeds.lrWheel);
                        if(rearAxleDelta > maxLeftToRightDelta)
                        {   //Stop test - Delta out of range

                            testResult = testFail;
                            m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail Rear LeftToRight Delta - %3.5f\n",
                                             m_component->GetTestStepName().c_str(), rearAxleDelta);
                        }
                    }
                    //Check Front to Rear delta is in range
                    if(usePercentageDelta)
                    {
                        // if we're using the percentage variable, calculate max deviation from rear axle speed
                        // otherwise absolute maxFrontToRearDeltat will be used
                        maxFrontToRearDelta = (maxFrontToRearDeltaPercent / 100) * rearSpeed;
                    }
                    frontToRearDelta = abs(frontSpeed - rearSpeed);
                    //frontToRearDelta = (frontSpeed > rearSpeed) ? (frontSpeed - rearSpeed) :
                    //                                              (rearSpeed - frontSpeed);
                    if(frontToRearDelta > maxFrontToRearDelta)
                    {   //Stop test - Delta out of range
                        testResult = testFail;
                        m_component->Log(LOG_ERRORS, "%s: AxleBalance Fail FrontToRear Delta - %3.5f\n",
                                         m_component->GetTestStepName().c_str(), frontToRearDelta);
                    }
                    //delay before next read
                    BposSleep(sampleDelay);
                }
                else
                {   // Error reading wheel speeds
                    testResult = testFail;
                    m_component->Log(LOG_ERRORS, "%s: Error reading Roll Speed - status: %s\n",
                                     m_component->GetTestStepName().c_str(), ConvertStatusToResponse(machineStatus).c_str());
                }
            }while((m_component->StatusCheck() == BEP_STATUS_SUCCESS) && ((testResult == testFail) || (driveAxleSpeed < targetSpeed)) &&
                   m_component->TimeRemaining() && m_component->SecondaryTimeRemaining(&waitForAxleBalanceTime) && (BEP_STATUS_SUCCESS == machineStatus));
            // Check exit condition
            if(BEP_STATUS_SUCCESS != m_component->StatusCheck())
            {   //System not ready
                testResult = testFail;
                testResultCode = m_component->GetFaultCode("SystemStatus");
                testDescription = m_component->GetFaultDescription("SystemStatus");
                m_component->Log(LOG_ERRORS, "System error during %s - status: %s\n", m_component->GetTestStepName().c_str(),
                                 ConvertStatusToResponse(m_component->StatusCheck()).c_str());
            }
            else if(machineStatus != BEP_STATUS_SUCCESS)
            {   // Bad comms with the module
                testResult = testFail;
                testResultCode = m_component->GetFaultCode("SystemReadFail");
                testDescription = m_component->GetFaultDescription("SystemReadFail");
                m_component->Log(LOG_ERRORS, "%s: AxleBalance Error reading Roll Speed\n",m_component->GetTestStepName().c_str());
            }
            else if((!m_component->TimeRemaining() || !m_component->SecondaryTimeRemaining(&waitForAxleBalanceTime)) && (driveAxleSpeed < targetSpeed))
            {   // Driver did not accelerate, FAIL him miserably!!!!
                testResult = testFail;
                testResultCode = m_component->GetFaultCode("SpeedFail");
                testDescription = m_component->GetFaultDescription("SpeedFail");
                m_component->Log(LOG_ERRORS, "%s: AxleBalance - Failed to reach target speed - vehicle speed: %.2f, target speed: %d",
                                 m_component->GetTestStepName().c_str(), driveAxleSpeed, targetSpeed);
            }
            else if((!m_component->TimeRemaining() || !m_component->SecondaryTimeRemaining(&waitForAxleBalanceTime))  && (testResult != testPass))
            {   // timeout
                testResult = testTimeout;
                testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Timeout");
                testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Timeout");
                m_component->Log(LOG_ERRORS, "%s: AxleBalance Timeout waiting for axle balance\n", m_component->GetTestStepName().c_str());
            }
            else if(testResult == testPass)
            {
                testResult = testPass;
                testResultCode = "0000";
                testDescription = m_component->GetTestStepInfo("Description");
                m_component->Log(LOG_ERRORS, "%s: AxleBalance OK\n", m_component->GetTestStepName().c_str());
            }
            else if(!testAbortOnFail && (testResult != testPass))
            {
                testResult = testFail;
                testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Fail");
                testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Fail");
                m_component->Log(LOG_ERRORS, "%s: AxleBalance FAIL\n", m_component->GetTestStepName().c_str());
            }
            else
            {
                testResult = testAbort;
                testResultCode = m_component->GetFaultCode(m_component->GetTestStepName() + "Abort");
                testDescription = m_component->GetFaultDescription(m_component->GetTestStepName() + "Abort");
                m_component->SystemWrite(ABORT_DATA_TAG, 1);
                m_component->Log(LOG_ERRORS, "%s: AxleBalance set ABORT ALL test\n", m_component->GetTestStepName().c_str());
            }
            // remove the target green bands
            m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), eraseSpeedRange);
            // remove the prompts
            m_component->RemovePrompt(m_component->GetPromptBox("MaintainSpeed"),
                                      m_component->GetPrompt("MaintainSpeed"),
                                      m_component->GetPromptPriority("MaintainSpeed"));
        }
        else
        {   // Speed acceleration failed
            testResult = testFail;
            testResultCode = m_component->GetFaultCode("SpeedFail");
            testDescription = m_component->GetFaultDescription("SpeedFail");
            m_component->Log(LOG_ERRORS, "%s: AxleBalance - Failed to reach target speed - vehicle speed: %.2f, target speed: %d",
                             m_component->GetTestStepName().c_str(), driveAxleSpeed, targetSpeed);
        }
        m_component->RemovePrompt(m_component->GetPromptBox("ShiftToDrive"),
                                  m_component->GetPrompt("ShiftToDrive"),
                                  m_component->GetPromptPriority("ShiftToDrive"));
        // Place numerical data from test into corresponding result reporting variables
        MaximumLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%f", maxLeftToRightDelta);
        FrontLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", frontAxleDelta);
        RearLeftToRightDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", rearAxleDelta);
        MaximumFrontToRearDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%f", maxFrontToRearDelta);
        FrontToRearDeltaLable = CreateMessage(tempCharArray, sizeof(tempCharArray), "%3.5f", frontToRearDelta);
        //Report test result to sever
        m_component->SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                              "MaxLeftToRightWheelDelta",   MaximumLeftToRightDeltaLable, unitsMPH,
                                              "FrontLeftToRightWheelDelta", FrontLeftToRightDeltaLable,   unitsMPH,
                                              "RearLeftToRightWheelDelta",  RearLeftToRightDeltaLable,    unitsMPH,
                                              "MaxFrontToRearAxleDelta",    MaximumFrontToRearDeltaLable, unitsMPH,
                                              "FrontToRearAxleDelta",       FrontToRearDeltaLable,        unitsMPH);
    }
    //Log exit
    m_component->Log(LOG_FN_ENTRY, "%s: Exit TcaseTool::CheckAxleBalance()\n", m_component->GetTestStepName().c_str());
    //return result to test step sequence
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::GetAxleRatio(float &ratio)
{
    // variables used for reporting test step results
    string testResult(testFail);                //Default test to fail
    INT32 testStatus(BEP_STATUS_FAILURE);       //Default ICM read to fail
    // Paramters used to calculate ratio
    INT32 samples = m_component->GetTestStepInfoInt("RatioSamples");  //Parameter -number of samples
    INT32 sampleTime = m_component->GetTestStepInfoInt("SampleTime");    //Parameter -delay between samples
    // variables used to calculate ratio
    float front = 0.0;      //current Front axle speed
    float rear = 0.0;       //current Rear axle speed
    float tandem = 0.0;     //current Rear axle speed
    float frontAxle = 0.0;  //average of LF + RF rolls speed
    float rearAxle = 0.0;   //average of LR + RR rolls speed
    float tandemAxle = 0.0; //average of LT + RT rolls speed
    float axleRatio = 0.0;  //average axle ratio over samples
    INT16 sampleCount = 0;  //number of samples
    INT32 sampleRate = 0;   //which raw data samples to use
    INT16 arrayIndex = 0;   //index in ICM speed data array.
    INT32 ratioStart = 0;   //Array tag for sample start
    INT32 ratioEnd = 0;     //Array tag for sample end

    //Log enter
    m_component->Log(LOG_FN_ENTRY, "%s: GetAxleRatio ENTER\n", m_component->GetTestStepName().c_str());

    ratio = 0.0;            //default to 0

    //tag the array for sample start
    ratioStart = m_component->TagArray("RatioStart");
    //ensure that all of the samples are taken
    m_component->Log(LOG_DEV_DATA, "GetAxleRatio: Waiting for %dms for %d samples\n", sampleTime, samples);
    BposSleep(sampleTime);
    //tag the array for sample end
    ratioEnd = m_component->TagArray("RatioEnd");
    m_component->Log(LOG_DEV_DATA, "TcaseTool::GetDataArray - RatioStart = %d, RatioEnd = %d\n", ratioStart, ratioEnd);

    //retrieve all samples taken over designated time
    testStatus = m_component->ReadDataArrays(m_component->GetDataTag("IcmSpeedArray"), ratioStart, ratioEnd, m_wheelSpeedArray);
    if (testStatus !=  BEP_STATUS_SUCCESS)
    {   //Log error
        m_component->Log(LOG_ERRORS, "%s: GetAxleRatio read ICM data Failed \n", m_component->GetTestStepName().c_str());
        testResult = testFail;
    }
    else
    {
        //calculate total samples returned by ICM
        sampleCount = (ratioEnd - ratioStart);
        //check desired number of samples is valid
        if (samples > sampleCount)
        {   //set samples = totalSamples - return all
            samples = sampleCount;
            //use all samples
            sampleRate = 1;
            //Log Warning
            m_component->Log(LOG_ERRORS, "%s: GetWheelSpeedSamples Warning Return all: Parameter samples:%d Actual Samples:%d Time:%d\n",
                             m_component->GetTestStepName().c_str(), m_component->GetTestStepInfoInt("RatioSamples"), sampleCount, sampleTime);
        }
        else if (samples > 1)
        {   //calculate which data from ICM to use (total / samples)
            sampleRate = (INT32)(sampleCount / samples);
            //Log sample and time
            m_component->Log(LOG_DEV_DATA, "%s: GetWheelSpeedSamples samples:%d  sampleTime:%d\n", 
                             m_component->GetTestStepName().c_str(), samples, sampleTime);
        }
        else if (samples <= 0)
        {
            //no samples
            sampleRate = 0;
            //Log sample and time
            m_component->Log(LOG_ERRORS, "%s: GetWheelSpeedSamples PARAMETER ERROR samples:%d%\n", m_component->GetTestStepName().c_str(), samples);
        }

        arrayIndex = 0;
        //sum up samples
        for (sampleCount=0; sampleCount < samples; sampleCount++ )
        {
            if (6 == m_component->GetWheelCount())
            {   //get average rolls speed for rear axle
                tandem = ((m_wheelSpeedArray[LTWHEEL][arrayIndex] + m_wheelSpeedArray[RTWHEEL][arrayIndex]) /2);
                //sum tandem roller speeds
                tandemAxle += tandem;
            }
            else
            {   //get average speed for front axle
            front = ((m_wheelSpeedArray[LFWHEEL][arrayIndex] + m_wheelSpeedArray[RFWHEEL][arrayIndex]) /2);
                //sum front roller speeds
                frontAxle += front;
            }
            //get average rolls speed for rear axle
            rear = ((m_wheelSpeedArray[LRWHEEL][arrayIndex] + m_wheelSpeedArray[RRWHEEL][arrayIndex]) /2);
            rearAxle += rear;
            //log average speeds
            m_component->Log(LOG_DEV_DATA, "GetAxleRatio() Sample %d:  FrontSpd: %.2f  RearSpd: %.2f\n", sampleCount+1, front, rear);
            arrayIndex += sampleRate;
        };
        //determine if exit before samples were completed
        if (sampleCount == 0)
        {   //Log error
            m_component->Log(LOG_ERRORS, "%s: GetAxleRatio data Failed samples:%d Actual Samples:%d\n", 
                             m_component->GetTestStepName().c_str(), samples, sampleCount);
            testResult = testFail;
        }
        else
        {
            // determine the vehicle drive wheel for ratio calculations
            // determine the vehicle drive wheel
            m_component->m_driveAxle = (m_component->ReadSubscribeData(m_component->GetDataTag("DriveAxle")) ==
                                        m_component->GetDataTag("Rear")) ? m_component->RWD : m_component->FWD;
            //Average the driven to nonDriven axle ratio
            if (6 == m_component->GetWheelCount())
                axleRatio += (tandemAxle / rearAxle);  //driven axle = rear
            else if (m_component->m_driveAxle == m_component->RWD)
                axleRatio += (frontAxle / rearAxle);  //driven axle = rear
            else
                axleRatio += (rearAxle / frontAxle);  //driven axle = front
            //Convert axle ratio to int with one decimal point 0.9900000550 = 99.00001%
            ratio = (axleRatio * 100) + 0.000005;
            //Log ratio
            m_component->Log(LOG_DEV_DATA, "%s: GetAxleRatio ratio: %3.7f\n", m_component->GetTestStepName().c_str(), axleRatio);
            //Got ratio
            testResult = testPass;
        }
    }
    // Publish the axle ratio to the system
    m_component->SystemWrite("AxleRatioValue", axleRatio);
    //Log exit
    char buff[16];
    m_component->SendTestResultWithDetail(testResult, "Axle Ratio Test", "0000",
                                          "AxleRatioValue", CreateMessage(buff, sizeof(buff), "%3.7f", axleRatio));
    m_component->Log(LOG_FN_ENTRY, "%s: GetAxleRatio EXIT\n", m_component->GetTestStepName().c_str());
    return(testResult);
}

//-----------------------------------------------------------------------------
INT32 TcaseTool::Disengage(void)
{
#if 1
    return m_component->Disengage();
#else
    string tag, value, response;        // communication strings
    INT32 status = BEP_STATUS_FAILURE;

    m_component->Log(LOG_FN_ENTRY, "Disengaging System\n");

    try
    {   // command the motor mode to Torque Mode

        if(m_component->SystemCommand(MOTOR_MODE, string(TORQUE_MODE)) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               MOTOR_MODE, TORQUE_MODE);
        m_component->Log(LOG_DEV_DATA, "Motor Mode Is Torque Mode\n");

        // command the motor to 0 Torque
        if (m_component->SystemCommand(COMMAND_TORQUE, 0) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               COMMAND_TORQUE, "0");
        m_component->Log(LOG_DEV_DATA, "Torque Commanded To 0\n");

        // tell the motor controller to declutch the system
        if (m_component->SystemCommand(COMMAND_CLUTCH, "0") != BEP_STATUS_SUCCESS)
            throw BepException("Could Not Declutch The Rollers\n");

        m_component->Log(LOG_DEV_DATA, "Clutches disengaged\n");

        // give time for the system to obtain the correct state
        int count = 20;
        while ((m_component->StatusCheck() != BEP_STATUS_SUCCESS) && (count-- > 0))
        {
            m_component->Log(LOG_DEV_DATA, "Waiting For System To Disengage\n");
            BposSleep(100);
        }

        m_component->Log(LOG_DEV_DATA, "System disengaged\n");

        if (count)   status = BEP_STATUS_SUCCESS;
        else        status = BEP_STATUS_FAILURE;
    }
    catch (BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Disengage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }
    catch (...)
    {
        m_component->Log(LOG_ERRORS, "Disengage Unknown Exception\n");
        status = BEP_STATUS_FAILURE;
    }

    m_component->Log(LOG_FN_ENTRY, "TcaseTool::Disengage - Clutch = %s, Declutch = %s\n",
                     m_component->ReadSubscribeData(MACHINE_CLUTCH).c_str(),
                     m_component->ReadSubscribeData(MACHINE_DECLUTCH).c_str());

    return(status);
#endif 
}

//-----------------------------------------------------------------------------
INT32 TcaseTool::Engage()
{
#if 1
    return m_component->Engage();
#else
    string tag, value, response;        // communication strings
    INT32 status = BEP_STATUS_FAILURE;
    // Log the entry
    m_component->Log(LOG_FN_ENTRY, "Engaging System\n");
    try
    {   // Clutch the rollers
        m_component->Log(LOG_DEV_DATA, "Checking if rollers need to be clutched...\n");
        if((m_component->ReadSubscribeData(MACHINE_CLUTCH) != "1") || 
           (m_component->ReadSubscribeData(MACHINE_DECLUTCH) != "0"))
        {   // make sure machine at Zerospeed
            m_component->Log(LOG_DEV_DATA, "\t\tChecking for Zero speed before clutching rollers\n");
            if (m_component->ReadSubscribeData(m_component->GetDataTag("Zerospeed")) == "1")
            {   // tell the motor controller to clutch the system
                m_component->Log(LOG_DEV_DATA, "Clutching Rollers\n");
                if (m_component->SystemCommand(COMMAND_CLUTCH, "1") != BEP_STATUS_SUCCESS)
                    throw BepException("Could Not Clutch The Rollers\n");
            }
            else
            {
                m_component->Log(LOG_ERRORS, "Error, Can Not Engage, Not At Zerospeed\n");
            }
        }
        // Command the motor mode to Boost Mode
        m_component->Log(LOG_DEV_DATA, "Commanding MotorController to Boost Mode\n");
        if(m_component->SystemCommand(MOTOR_MODE, string(BOOST_MODE)) != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n",
                               MOTOR_MODE, BOOST_MODE);
        // set Boost to enable
        m_component->Log(LOG_DEV_DATA, "Enabling to Boost \n");
        if(m_component->SystemWrite(BOOST_MODE, "1") != BEP_STATUS_SUCCESS)
            throw BepException("Error: Could Not Command %s, %s\n", BOOST_MODE, "1");
        // give time for the system to obtain the correct state
        int count = 20;
        while ((m_component->StatusCheck() != BEP_STATUS_SUCCESS) &&
               (m_component->StatusCheck() != BEP_STATUS_ABORT) && (count-- > 0))
        {
            m_component->Log(LOG_DEV_DATA, "Waiting For System To Engage\n");
            BposSleep(100);
        }

        if (count) status = BEP_STATUS_SUCCESS;
        else           status = BEP_STATUS_FAILURE;

    }
    catch (BepException &e)
    {
        m_component->Log(LOG_ERRORS, "Engage Exception: %s\n", e.what());
        status = BEP_STATUS_FAILURE;
    }

    return(status);
#endif 
}

//-----------------------------------------------------------------------------
bool TcaseTool::ShortCircuitTestStep(void)
{
    bool skip = false;
    bool parameterSkip = m_component->GetParameterBool(m_component->GetTestStepName() + "SkipIfPreviousTCaseFail");
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::ShortCircuitTestStep() OverallResult: %s\n", m_component->GetOverallResult().c_str());
    if(!parameterSkip || (parameterSkip && ((m_component->GetOverallResult() == testPass) || m_component->GetOverallResult() == BEP_TESTING_RESPONSE)))
    {// check component short circuit
        skip = m_component->ShortCircuitTestStep();
    }
    else skip = true;   
    // return the result
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::ShortCircuitTestStep() returning %s\n", skip ? "Skip" : "Perform");
    return(skip);
}

//-----------------------------------------------------------------------------
string TcaseTool::GetTcasePassFail(void)
{
    string testResult = testFail;
    // Log the function entry
    m_component->Log(LOG_FN_ENTRY, "%s::%s - Enter\n", m_component->GetComponentName().c_str(), m_component->GetTestStepName().c_str());
    //determine if step needs to be ran
    if (m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_DEV_DATA, "%s Skip %s Test\n",  m_component->GetComponentName().c_str(), m_component->GetTestStepName().c_str());
        testResult = testSkip;          //Return SKIP
    }
    else
    {   // Test Name
        string  testStepName = m_component->GetTestStepName().c_str();
        // Prompt and get operator input
        testResult = m_component->OperatorPassFail(m_component->GetDataTag(testStepName + "Prompt"), m_component->GetTestStepInfoInt("Timeout"));
        //Send Resutlts
        m_component->SendTestResult(testStepName, testResult, m_component->GetTestStepInfo("Description"));
    }
    // Log the function exit
    m_component->Log(LOG_FN_ENTRY, "%s::%s - Exit\n", m_component->GetComponentName().c_str(), m_component->GetTestStepName().c_str());
    // Return the results
    return(testResult);
}

//-----------------------------------------------------------------------------
    /***********WARNING, untested, buggy code************/
//-----------------------------------------------------------------------------
string TcaseTool::CoastToStop(void)
{
    string testResult = testFail;
    BEP_STATUS_TYPE machineStatus = BEP_STATUS_ERROR;   // Check for aborts
    WHEELINFO rollerSpeeds;                             // Define roller speed array.
    INT16 arrayIndex = 0;                               // Roller speed arry index
    
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::CoastToStop() ENTER\n");
    if (m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_FN_ENTRY, "Skip TcaseTool::CoastToStop()\n");
        testResult = testSkip;          //Return SKIP
    }
    else
    {   // set desired speed range. Green band low end
        float minSpeedTarget = 0.0;
        // set speed to coast down too. Green band high end.
        float maxSpeedTarget = m_component->GetParameterFloat("CoastToStopSpeed");
        // Used for converting integers to a characters
        char tempCharArray[20];
        // define green band range message
        // set up string for passing proper speed range
        string speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray),"%d %d", (int) minSpeedTarget, (int) maxSpeedTarget);
        // number Of Rollers
        UINT32 wheelCount = m_component->GetWheelCount();
        // current roller value
        UINT32 wheel = 0;
        // track fastest roller speed
        float maxRollerSpeed = 0.0;                         
        // display the speed target green band
        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), speedRange);
        // display Prompt
        m_component->DisplayPrompt(m_component->GetPromptBox("CoastToStop"),m_component->GetPrompt("CoastToStop"),m_component->GetPromptPriority("CoastToStop"));
        //Parameter -delay between samples
        INT32 sampleDelay = m_component->GetTestStepInfoInt("ScanDelay");
        // Prompt and verify coast dwon to desired speed
        do
        {   // Get the current roller speeds
            machineStatus = m_component->GetWheelSpeeds(rollerSpeeds);
            // Check status
            if (BEP_STATUS_SUCCESS == machineStatus)
            {
                for (wheel = 0;  wheel < wheelCount; wheel++)
                {   // track the fastest wheel speed
                    if (m_wheelSpeedArray[wheel][arrayIndex] > maxRollerSpeed)
                    {
                        maxRollerSpeed = abs(m_wheelSpeedArray[wheel][arrayIndex]);
                        //maxRollerSpeed = (m_wheelSpeedArray[wheel][arrayIndex] > 0) ? (m_wheelSpeedArray[wheel][arrayIndex]) :
                        //                                                              (-1.0 * m_wheelSpeedArray[wheel][arrayIndex]);
                    }

                }
                if (maxRollerSpeed > maxSpeedTarget)
                {   // target speed has not been achieved
                    BposSleep(sampleDelay);
                }
            }
            else
            {   // Error reading wheel speeds
                testResult = testFail;
                m_component->Log(LOG_ERRORS, "%s: Error reading Roll Speed - status: %s\n",
                                 m_component->GetTestStepName().c_str(), ConvertStatusToResponse(machineStatus).c_str());
            }
            // Keep checking until target speed achieved, timeout or bad status
        } while ((maxRollerSpeed > maxSpeedTarget) && m_component->TimeRemaining() && (m_component->StatusCheck() == BEP_STATUS_SUCCESS)
            && (BEP_STATUS_SUCCESS == machineStatus));
    
        // remove the speed target from the gauge
        m_component->SystemWrite(m_component->GetDataTag("SpeedTarget"), string("0 0"));
        // remove the prompts
        m_component->RemovePrompt(m_component->GetPromptBox("CoastToStop"),m_component->GetPrompt("CoastToStop"),m_component->GetPromptPriority("CoastToStop"));
        //Verify vehicle reached coast speed.
        if (maxRollerSpeed < maxSpeedTarget)
        {   //check to see if we need to use brake for full stop
            if ((m_component->ReadSubscribeData(ZEROSPEED_TAG) != "1") && m_component->GetParameterBool("CoastToStopAllowBraking"))
            {   //call zerospeed
                if (m_component->CheckZeroSpeed() == true)
                {   //vehicle at zero speed
                    m_component->Log(LOG_DEV_DATA, "%s: TcaseTool::CheckVehicleSpeed Vehicle at Zero Speed\n", m_component->GetTestStepName().c_str());
                    testResult = testPass;
                }
                else
                {
                    //Fail vehicle at zero speed
                    m_component->Log(LOG_ERRORS, "%s: Fail TcaseTool::CheckVehicleSpeed Brake to Stop\n", m_component->GetTestStepName().c_str());
                    testResult = testFail;
                }
            }
            else
                testResult = testPass;
        }
        else
            testResult = testFail;
        //Send Resutlts
        m_component->SendTestResult(m_component->GetTestStepName().c_str(), testResult, m_component->GetTestStepInfo("Description"));
    }
    // Log the function exit
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::CoastToStop() Exit\n");
    // Return the results
    return(testResult);
}

//-----------------------------------------------------------------------------
string TcaseTool::TcaseAccelerateToSpeed(void)
{
    string testResult = testFail;
    // Log the function entry
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::AccelerateToSpeed() ENTER\n");
    if (m_component->ShortCircuitTestStep())
    {   //log skip step
        m_component->Log(LOG_FN_ENTRY, "Skip TcaseTool::AccelerateToSpeed()\n");
        testResult = testSkip;          //Return SKIP
    }
    else
    {   // Accelerate to speed
        testResult = m_component->AccelerateToTestSpeed(m_component->GetParameterFloat(m_component->GetTestStepName()+"TargetSpeed"),
                                           m_component->GetParameter(m_component->GetTestStepName()+"TargetSpeedRange"),
                                                       m_component->GetTestStepInfoInt("ScanDelay"),true);
        //Send test result
        m_component->SendTestResult(m_component->GetTestStepName().c_str(), testResult, m_component->GetTestStepInfo("Description"));
    }
    // Log the function exit
    m_component->Log(LOG_FN_ENTRY, "TcaseTool::AccelerateToSpeed() Exit\n");
    // Return the results
    return(testResult);
}

//*************************************************************************
//end of TcaseTool.cpp
