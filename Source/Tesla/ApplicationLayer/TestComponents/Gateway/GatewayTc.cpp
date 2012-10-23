//===========================================================================
// Copyright (c) 2012- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "GatewayTc.h"

//---------------------------------------------------------------------------------------------------------------------
template<class ModuleType>
GatewayTc<ModuleType>::GatewayTc() : GenericTCTemplate<ModuleType>()
{	// Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template<class ModuleType>
GatewayTc<ModuleType>::~GatewayTc()
{	// Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template<class ModuleType>
const string GatewayTc<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "GatewayTc::CommandTestStep() - Enter");
	if(BEP_STATUS_SUCCESS == StatusCheck())
	{
		try
		{
			if(!GetTestStepName().compare("CycleIgnition"))   result = CycleIgnition();
			else   result = GenericTCTemplate<ModuleType>::CommandTestStep(value);
		}
		catch(ModuleException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s ModuleException: %s\n", GetTestStepName().c_str(), err.what());
			result = testSoftwareFail;
		}
		catch(BepException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
			result = testError;
		}
	}
	else
	{
		Log(LOG_ERRORS, "System status error!  Cannot continue testing");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "GatewayTc::CommandTestStep() - Exit");
	return result;
}

//---------------------------------------------------------------------------------------------------------------------
template<class ModuleType>
string GatewayTc<ModuleType>::CycleIgnition(void)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "GatewayTc::CycleIgnition() - Enter");
	if(!ShortCircuitTestStep())
	{   // Wait for the engine to be off
		result = WaitForEngineOffIgnitionOff();
		if(!result.compare(testPass))
		{   // Ignition is off, wait for ignition on
			result = CheckIgnitionOn() ? testPass : testFail;
		}
		else
		{   // Timeout waiting for engine off
			Log(LOG_ERRORS, "Timeout waiting for ignition off");
		}
		// Report the result
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{ 
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping Cycle Ignition");
	}
	Log(LOG_FN_ENTRY, "GatewayTc::CycleIgnition() - Exit");
	return result;
}
