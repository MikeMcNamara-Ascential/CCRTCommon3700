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
//    $Log: /Ccrt/Source/Isuzu/ApplicationLayer/TestComponents/Machine/Common/IsuzuMachineTC.cpp $
// 
//
// 1     7/27/16 2:19p kstutzma
// Initial Checkin.
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "IsuzuMachineTC.h"

//=============================================================================
IsuzuMachineTC::IsuzuMachineTC() : MachineTC()
{
}

//=============================================================================
IsuzuMachineTC::~IsuzuMachineTC()
{
}

//=============================================================================
const std::string IsuzuMachineTC::CommandTestStep(const std::string &value)
{
    std::string status; // the status of the commanded test step

    // Determine if the next test step can be sequenced
    if(StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {
            std::string step = GetTestStepName();       // get the name of the sequenced test step

            Log(LOG_FN_ENTRY, "IsuzuMachineTC::CommandTestStep(%s): %s\n", value.c_str(), step.c_str());

            if(step == "TransitionToRearAxle") status = TransitionToRearAxle();
            else if(step == "ReportSideSlipValue") status = ReportSideSlipValue();
            else if (step == "ReportSteeringWheelAngle") status = ReportSteeringWheelAngle();
            else if (step == "AttachSteeringWheelAngleDevice") status = AttachSteeringWheelAngleDevice();

            // else invalid test step*/
            else status = MachineTC::CommandTestStep(value);

            Log(LOG_FN_ENTRY, "IsuzuMachineTC::CommandTestStep(%s): %s Returned: %s\n",
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

//-------------------------------------------------------------------------------------------------
std::string IsuzuMachineTC::TransitionToRearAxle(void)
{	// Log the entry and determine if this step should be performed
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "IsuzuMachineTC::TransitionToRearAxle() - Enter");
	if(!ShortCircuitTestStep() && !IsRetest())
	{   //Disconnect the cable
        DisplayPrompt(GetPromptBox("DisconnectCable"),GetPrompt("DisconnectCable"),GetPromptPriority("DisconnectCable"));
        while(SystemReadBool("PLCCableConnect") && !CheckAbort())	BposSleep(250);

        DisplayPrompt(GetPromptBox("LowerRetainers"),GetPrompt("LowerRetainers"),GetPromptPriority("LowerRetainers"));
        while(!SystemReadBool(ROLLS_DOWN_DATA_TAG) && !CheckAbort())	BposSleep(250);

		// Prompt the operator to advance the rear axle to the retaining rolls
		DisplayPrompt(GetPromptBox("AdvanceToRearAxle"), GetPrompt("AdvanceToRearAxle"),
					  GetPromptPriority("AdvanceToRearAxle"), GetParameter("DefaultPromptBackgroundColor"));
		// Wait for the vehicle to be on the rolls
		while(SystemReadBool(GetDataTag("VehiclePresent")) && !CheckAbort())  BposSleep(250);
		// Wait a bit more for the front axle of the vehicle to leave the machine
        if(!CheckAbort())
		    BposSleep(500);
        while(!SystemReadBool(GetDataTag("VehiclePresent")) && !CheckAbort())  BposSleep(250);
        // Wait a bit more for the rear axle of the vehicle to sit on the rollers
        if(!CheckAbort())
            BposSleep(3000);
		RemovePrompt(GetPromptBox("AdvanceToRearAxle"), GetPrompt("AdvanceToRearAxle"),
					 GetPromptPriority("AdvanceToRearAxle"));

        DisplayPrompt(GetPromptBox("RaiseRetainers"),GetPrompt("RaiseRetainers"),GetPromptPriority("RaiseRetainers"));
		while(!SystemReadBool(ROLLS_UP_DATA_TAG) && !CheckAbort())	BposSleep(250);

        //Connect Cable
        DisplayPrompt(GetPromptBox("ConnectCable"),GetPrompt("ConnectCable"),GetPromptPriority("ConnectCable"));
		while(!SystemReadBool("PLCCableConnect") && !CheckAbort())	BposSleep(250);
        
        if(!CheckAbort())
            result = testPass;
        else
            result = ConvertStatusToResponse(StatusCheck());

        SendTestResult(result, GetTestStepInfo("Description"));
	}
	else
	{	// Need to skip this step
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping transition to rear axle");
	}
	// log the exit and return the result
	Log(LOG_FN_ENTRY, "IsuzuMachineTC::TransitionToRearAxle() - Exit: %s", result.c_str()); 
	return result;
}

//-------------------------------------------------------------------------------------------------
std::string IsuzuMachineTC::ReportSideSlipValue(void) 
{

    float frontSideSlipResult;
    float sideSlipValueMax;
    float sideSlipValueMin;
    char buff[16];
    string result; 
    string color = "white";

    if(!ShortCircuitTestStep())
	{
        frontSideSlipResult = SystemReadFloat("SideSlipValue"); 
        sideSlipValueMax = GetParameterFloat ("SideSlipValueMax");
        sideSlipValueMin = GetParameterFloat ("SideSlipValueMin");
        //To determine if the test is pass or fail
        if (sideSlipValueMin <= frontSideSlipResult && frontSideSlipResult <= sideSlipValueMax) 
        {    
            //Test Pass
            result = testPass;
            color = "Green";
        } 
        else 
        {
            //Test Fail
            result = testFail;
            color = "Red"; 
        }
    
        SystemWrite(GetDataTag("SideSlipBGColor"), color); 

        SendTestResultWithDetail(result, GetTestStepInfo("Description"),"0000",
                "SideSlipValue",CreateMessage(buff, sizeof(buff), "%.2f", frontSideSlipResult),"m/km",
                "SideSlipMaxValue",CreateMessage(buff, sizeof(buff), "%.2f", sideSlipValueMax),"m/km",
                "SideSlipMinValue",CreateMessage(buff, sizeof(buff), "%.2f", sideSlipValueMin),"m/km");
    }
    else
    {
        result = testSkip;
    }


   return result; 
     
}

//-------------------------------------------------------------------------------------------------
std::string IsuzuMachineTC::ReportSteeringWheelAngle(void) 
{

    float scanDelay = 250;
    float elapsedTime = 0;
    float steeringWheelCollectionInterval;

    int samples = 0;
    float wheelAngleTotal = 0;
    float steeringWheelAngle;

    float steeringWheelAngleMax;
    float steeringWheelAngleMin;

    char buff[16];
    string result; 
    string color = "white";

    if(!ShortCircuitTestStep())
	{

        UpdatePrompts();

        steeringWheelAngleMax = GetParameterFloat ("SteeringWheelAngleMax");
        steeringWheelAngleMin = GetParameterFloat ("SteeringWheelAngleMin");
        steeringWheelCollectionInterval = GetParameterFloat("SteeringWheelCollectionInterval");

        //Get average over a period of time
        while(elapsedTime < steeringWheelCollectionInterval && !CheckAbort())
        {
            wheelAngleTotal += SystemReadFloat("SteeringWheelAngle");
            samples++;
            elapsedTime += scanDelay / 1000;
            BposSleep(scanDelay);
        }

        steeringWheelAngle = wheelAngleTotal / samples;
        
        //To determine if the test is pass or fail
        if (steeringWheelAngleMin <= steeringWheelAngle && steeringWheelAngle <= steeringWheelAngleMax) 
        {    
            //Test Pass
            result = testPass;
            color = "Green";
        } 
        else 
        {
            //Test Fail
            result = testFail;
            color = "Red"; 
        }
    
        SystemWrite(GetDataTag("SteeringWheelAngleBGColor"), color); 

        SendTestResultWithDetail(result, GetTestStepInfo("Description"),"0000",
                "SteeringWheelAngle",CreateMessage(buff, sizeof(buff), "%.2f", steeringWheelAngle),"°",
                "SteeringWheelAngleMaxValue",CreateMessage(buff, sizeof(buff), "%.2f", steeringWheelAngleMax),"°",
                "SteeringWheelAngleMinValue",CreateMessage(buff, sizeof(buff), "%.2f", steeringWheelAngleMin),"°");
    }
    else
    {
        result = testSkip;
    }

    RemovePrompts();
    return result; 
     
}

//-------------------------------------------------------------------------------------------------
std::string IsuzuMachineTC::AttachSteeringWheelAngleDevice(void) 
{
    string result; 
    int scanDelay = 250;
    //string color = "white";

    if(!ShortCircuitTestStep())
	{
        DisplayPrompt(GetPromptBox("AttachSteeringWheelAngleDevice"),GetPrompt("AttachSteeringWheelAngleDevice"),GetPromptPriority("AttachSteeringWheelAngleDevice"));
        DisplayPrompt(GetPromptBox("PressPassWhenFinished"),GetPrompt("PressPassWhenFinished"),GetPromptPriority("PressPassWhenFinished"));

        // Determine where the input will come from
        //if (UsePlcResultButtons())
        //{
        string yesButton = "-1";

        string msgResponse;

        // Loop until the pass button has been pressed
        while (yesButton == "-1" && !CheckAbort() && TimeRemaining()) {

            // Read PLC pass button
            if (SystemReadBool(GetDataTag("PassButton")))
            {
                yesButton = 1;
            }
            BposSleep(scanDelay);
        }

        if (!TimeRemaining()) {
            result = BEP_TIMEOUT_RESPONSE;
        }
        else
        {
            Log(LOG_DEV_DATA, "Yes button: %s", yesButton.c_str());
            result = !yesButton.compare("1") ? BEP_PASS_RESPONSE : BEP_FAIL_RESPONSE;
        }
        //}
        RemovePrompts();
    }
    else
    {
        result = testSkip;
    }

    SendTestResult(result, GetTestStepInfo("Description"));
    return result;
}
                                                  
