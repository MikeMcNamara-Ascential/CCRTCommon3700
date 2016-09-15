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

            Log(LOG_FN_ENTRY, "MachineTC::CommandTestStep(%s): %s\n", value.c_str(), step.c_str());

            if(step == "TransitionToRearAxle") status = TransitionToRearAxle();
            if(step == "ReportSideSlipValue") status = ReportSideSlipValue();
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
	if(!ShortCircuitTestStep())
	{   // Have the operator press the pass button to lower the retaining rollers
        result = OperatorPassFail("LowerRetainers");
        // Wait for the operator to press the button
        if(!result.compare(testPass))
        {
			MachineTC::LowerRollsRaiseElevators();
            while(!SystemReadBool(ROLLS_DOWN_DATA_TAG) && (BEP_STATUS_SUCCESS == StatusCheck()))	BposSleep(250);
		}
		else
		{	// Something is wrong, abort the sequence
			SystemWrite(ABORT_DATA_TAG, true);
		}

		// Prompt the operator to advance the rear axle to the retaining rolls
		DisplayPrompt(GetPromptBox("AdvanceToRearAxle"), GetPrompt("AdvanceToRearAxle"),
					  GetPromptPriority("AdvanceToRearAxle"), GetParameter("DefaultPromptBackgroundColor"));
		// Wait for the vehicle to be on the rolls
		while(SystemReadBool(GetDataTag("VehiclePresent")) && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(250);
		// Wait a bit more for the front axle of the vehicle to leave the machine
		BposSleep(500);
        while(!SystemReadBool(GetDataTag("VehiclePresent")) && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(250);
        // Wait a bit more for the rear axle of the vehicle to sit on the rollers
		BposSleep(3000);
		RemovePrompt(GetPromptBox("AdvanceToRearAxle"), GetPrompt("AdvanceToRearAxle"),
					 GetPromptPriority("AdvanceToRearAxle"));
		// Have the operator press the pass button to deploy the retaining rollers
		result = OperatorPassFail("RaiseRetainers");
		// Wait for the operator to press the button
		if(!result.compare(testPass))
		{
			MachineTC::RaiseRollsLowerElevators();
			while(!SystemReadBool(ROLLS_UP_DATA_TAG) && (BEP_STATUS_SUCCESS == StatusCheck()))	BposSleep(250);
		}
		else
		{	// Something is wrong, abort the sequence
			SystemWrite(ABORT_DATA_TAG, true);
		}
	}
	else
	{	// Need to skip this step
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping transition to rear axle");
	}
	// log the exit and return the result
	Log(LOG_FN_ENTRY, "IsuzuMachineTC::TransitionToRearAxle() - Exit"); 
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
        } 
        else 
        {
            //Test Fail
            result = testFail;
        }
    }

    SendTestResultWithDetail(result, GetTestStepInfo("Description"),"0000",
                "SideSlipValue",CreateMessage(buff, sizeof(buff), "%.2f", frontSideSlipResult),"m/km",
                "SideSlipMaxValue",CreateMessage(buff, sizeof(buff), "%.2f", sideSlipValueMax),"m/km",
                "SideSlipMinValue",CreateMessage(buff, sizeof(buff), "%.2f", sideSlipValueMin),"m/km");

   return result; 
     
}
                                                  
