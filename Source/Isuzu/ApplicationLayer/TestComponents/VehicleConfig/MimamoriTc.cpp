//******************************************************************************
// Description:
// Test steps specific to Isuzu Mimamori Module testing.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MimamoriTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MimamoriTc<ModuleType>::MimamoriTc() : GenericTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MimamoriTc<ModuleType>::~MimamoriTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const bool MimamoriTc<ModuleType>::CheckCableConnect(void)
{
	bool cableConnected = false;  // Used to store return result from IsCableConnected()
	bool promptDisplayed = false; // Used to determine if prompts have been displayed
	UINT32 delay = 100;  // Used to store amount of delay from TestStep info in config file

	// Set delay in do-while loop using BposSleep() function call
	delay = GetTestStepInfoInt("ScanDelay");
	// Start the timer
	SetStartTime();
	// Wait for the cable to be connected
	do
	{   // Check if the cable is connected and prompt if neccessary
		cableConnected = IsCableConnected();
		Log(LOG_DEV_DATA, "CheckCableConnect - IsCableConnected: %s -- {ReadSubscribeData(MimamoriCableConnect): %s}\n",
			cableConnected ? "Yes" : "No", SystemReadBool("MimamoriCableConnect") ? "True" : "False");
		if(!cableConnected)
		{   // Check if bkground test
			if( !IsForegroundTest())
			{   // Resets the step start time since the step should not prompt until
				//     its TestDomain is set to foreground
				SetStartTime();
			}
			// if TestDomain is foreground, display prompt if not already done
			else if(!promptDisplayed)
			{   // Cable not connected, prompt the operator
				DisplayPrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
				promptDisplayed = true;
			}
			// Wait before checking again
			BposSleep(delay);
		}
	} while(TimeRemaining() && !cableConnected && (BEP_STATUS_SUCCESS == StatusCheck()));

	// Check if the prompt needs to be removed
	if(promptDisplayed)
	{   // Remove the prompt from the display
		RemovePrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
	}
	// Log the status
	if(!cableConnected) Log(LOG_ERRORS, "Timeout waiting for cable connect!\n");
	else                 Log(LOG_DEV_DATA, "Cable Connected\n");
	// Return the status of cable connect
	return(cableConnected);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const bool MimamoriTc<ModuleType>::IsCableConnected(void)
{
	return SystemReadBool("MimamoriCableConnect");
}
