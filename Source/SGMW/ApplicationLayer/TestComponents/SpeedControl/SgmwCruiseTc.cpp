//******************************************************************************
// Description:
//  SGMW Cruise testing for all types of vehicles.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "SgmwCruiseTc.h"

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
SgmwCruiseTc<ModuleType>::SgmwCruiseTc() : GenericSpeedControlTC<ModuleType>()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
SgmwCruiseTc<ModuleType>::~SgmwCruiseTc()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
const string SgmwCruiseTc<ModuleType>::CheckSetEngagement(void)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CheckSetEngagement() - Enter");
	if(!ShortCircuitTestStep())
	{
		SystemWrite("CruiseTargetSpeed", string("0"));
		result = WaitAndPromptForSpeed(GetParameterFloat("SpeedControlMinSpeed"),
									   GetParameterFloat("SpeedControlMaxSpeed"),
									   GetParameterInt("SpeedRangeCheckDelay"));
		// Show the target green band 
		SystemWrite(GetDataTag("SpeedTarget"), string(GetParameter("SpeedControlMinSpeed")+
													  " "+GetParameter("SpeedControlMaxSpeed")));
		TestResultDetails details;
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		string operatorInput;
		char buff[32];
		if(testPass == result)
		{	// Instruct the operator to press the set button and remove foot from throttle
			DisplayPrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
			DisplayPrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
						  GetPromptPriority("RemoveFootPrompt"));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Set the timer so we can look for the SET switch
			SetStartTime();
			// Wait for the operator to press the set button
			bool cruiseSet = false;
			status = WaitForSetButton(cruiseSet, operatorInput);
			RemovePrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
			// Check the exit conditions
			if(!TimeRemaining() && !cruiseSet && (operatorInput.compare(testFail) != 0))
			{	// Timeout waiting for SET button to be pressed
				Log(LOG_ERRORS, "Timeout waiting for speed control to be SET\n");
			}
			else if((BEP_STATUS_SUCCESS != StatusCheck()) && !cruiseSet && (operatorInput.compare(testFail) != 0))
			{	// Bad system status waiting for cruise set
				Log(LOG_ERRORS, "Bad system status waiting for speed control SET - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
			}
			else if(BEP_STATUS_SUCCESS != status)
			{	// Communication failure waiting for cruise set
				Log(LOG_ERRORS, "Communication Failure checking for speed control SET - status: %s\n",
					ConvertStatusToResponse(status).c_str());
			}
			else if(!cruiseSet && (operatorInput.compare(testFail) == 0))
			{	// Operator pressed the fail button
				Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
			}
			else
			{	// SET button pressed, look for speed stabilization
				DisplayPrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
							  GetPromptPriority("CheckStableSpeed"));
				// Store the target speed
				if(BEP_STATUS_SUCCESS == SetSpeedControlTargetSpeed())
				{
					SystemWrite("CruiseTargetSpeed", GetSpeedControlTargetSpeed());
					details.AddDetail("TargetSpeed", 
									  CreateMessage(buff, sizeof(buff), "%.2f", GetSpeedControlTargetSpeed()),
									  unitsMPH);
					float minLimit = GetSpeedControlTargetSpeed() - GetParameterFloat("StableSpeedMinVariation");
					float maxLimit = GetSpeedControlTargetSpeed() + GetParameterFloat("StableSpeedMaxVariation");
					result = WaitForStableSpeed(minLimit, maxLimit);
					if(testPass == result)
					{	// Check the roller speed against the vehicle target speed and make sure it is within tolerance
						float rollerSpeed = GetRollSpeed();
						details.AddDetail("ActualSpeed", 
										  CreateMessage(buff, sizeof(buff), "%.2f", rollerSpeed),
										  unitsMPH);
						result = ((minLimit <= rollerSpeed) && (rollerSpeed <= maxLimit)) ? testPass : testFail;
						Log(LOG_DEV_DATA, "Cruise set speed: %s - Min: %.2f, Actual: %.2f, Max: %.2f",
							result.c_str(), minLimit, rollerSpeed, maxLimit);
					}
					else
					{
						Log(LOG_ERRORS, "Timeout waiting for stable speed");
					}
				}
				else
				{	// Error reading set speed from the module
					Log(LOG_ERRORS, "Error reading target speed from the module");
					result = testFail;
				}
				RemovePrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
							 GetPromptPriority("CheckStableSpeed"));
			}
			RemovePrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
						 GetPromptPriority("RemoveFootPrompt"));
		}
		else
		{
			Log(LOG_DEV_DATA, "Timeout waiting for speed to be in range");
		}
		// Report the results
		SendTestResultWithDetail(result, details, GetTestStepInfo("Description"), "0000");
		SystemWrite(GetDataTag("SpeedTarget"), "0 0");
	}
	else
	{
		Log(LOG_DEV_DATA, "Skipping SgmwCruiseTc::CheckSetEngagement");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CheckSetEngagement() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
const string SgmwCruiseTc<ModuleType>::CheckTapSpeedChange(const string &direction)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CheckTapSpeedChange(direction: %s) - Enter", direction.c_str());
	if(!ShortCircuitTestStep())
	{
		string promptTag = !direction.compare("Up") ? "PressTapUpButton" : "PressTapDownButton";
		DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
		SystemWrite(GetDataTag("SpeedTarget"), GetParameter("TapSpeedChangeRange" + direction));
		// Setup the InputServer to get operator input
		SetupForOperatorInput();
		DataAnalysis analyze;
		Comparison_t comparison = !direction.compare("Up") ? GREATER : LESS;
		// Wait for the vehicle speed to increase
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		string operatorInput   = "Unknown";
		float currentTargetSpeed = 0.0;
		float targetSpeed = GetParameterFloat("TapSpeedChangeTarget" + direction);
		bool targetAchieved = false;
		do
		{	// Get the current speed from the module
			status = m_vehicleModule.ReadModuleData("ReadCruiseTargetSpeed", currentTargetSpeed);
			if(BEP_STATUS_SUCCESS == status)
			{
				targetAchieved = analyze.CompareData(currentTargetSpeed, targetSpeed, comparison);
				SystemWrite("CruiseTargetSpeed", currentTargetSpeed);
			}
			BposSleep(GetTestStepInfoInt("ScanDelay"));
			operatorInput = CheckForOperatorFail();

		} while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status) &&
				!targetAchieved && operatorInput.compare(testFail));
		RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
		SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
		result = targetAchieved ? testPass : testFail;
		char buff[32];
		SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000",
								 "TargetSpeed", CreateMessage(buff, sizeof(buff), "%.2f", targetSpeed), unitsMPH,
								 "FinalSpeed", CreateMessage(buff, sizeof(buff), "%.2f", currentTargetSpeed), unitsMPH);
	}
	else
	{
		Log(LOG_DEV_DATA, "Skipping Tap Speed Change Check");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CheckTapSpeedChange(direction: %s) - Exit", direction.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
const string SgmwCruiseTc<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CommandTestStep(value: %s) - Enter", value.c_str());
	if(BEP_STATUS_SUCCESS == StatusCheck())
	{
		if(!GetTestStepName().compare("CheckFaultCount"))	 result = ReadFaultCount();
		else
			result = GenericSpeedControlTC<ModuleType>::CommandTestStep(value);
	}
	else
	{
		Log(LOG_ERRORS, "Bad system status, not running test step %s", GetTestStepName().c_str());
		result = ConvertStatusToResponse(StatusCheck());
	}
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::CommandTestStep(value: %s) - Exit", value.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string SgmwCruiseTc<ModuleType>::ReadFaultCount(void)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::ReadFaultCount() - Enter");
	if(!ShortCircuitTestStep())
	{
		UINT16 dtcCount = 0;
		if(BEP_STATUS_SUCCESS == m_vehicleModule.ReadModuleData("ReadDtcCount", dtcCount))
		{
			Log(LOG_DEV_DATA, "Found %d DTCs in the module", dtcCount);
			result = (dtcCount == 0) ? testPass : testFail;
		}
		else
		{
			Log(LOG_ERRORS, "Error reading DTC count from the module");
			result = testFail;
		}
		char buff[32];
		SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000",
								 "DtcCount", CreateMessage(buff, sizeof(buff), "%d", dtcCount), "");
	}
	else
	{
		Log(LOG_DEV_DATA, "Skipping Check For Faults");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::ReadFaultCount() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string SgmwCruiseTc<ModuleType>::WaitForStableSpeed(float minLimit, float maxLimit)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::WaitForStableSpeed(min: %.2f, max: %.2f) - Enter", minLimit, maxLimit);
	float rollSpeed = 0.0;
	INT16 stableSpeedCount = 0;
	string operatorInput;
	do
	{	// Get the current roll speed and make sure it is within limits
		rollSpeed = GetRollSpeed();
		if((minLimit <= rollSpeed) && (rollSpeed <= maxLimit))
		{
			stableSpeedCount++;
			Log(LOG_DEV_DATA, "Speed in stable range, incrementing count to %d", stableSpeedCount);
		}
		else
		{
			Log(LOG_DEV_DATA, "Speed out of range, resetting counter to 0");
			stableSpeedCount = 0;
		}
		operatorInput = CheckForOperatorFail();
		// Provide a slight delay before checking again
		BposSleep(GetParameterInt("StableSpeedSampleDelay"));
	} while((stableSpeedCount < GetParameterInt("StableSpeedPoints")) && TimeRemaining() && 
			(BEP_STATUS_SUCCESS == StatusCheck()) && operatorInput.compare(testFail));
	result = (stableSpeedCount >= GetParameterInt("StableSpeedPoints")) ? testPass : testFail;
	Log(LOG_DEV_DATA, "Vehicle speed stable: %s", result.c_str());
	Log(LOG_FN_ENTRY, "SgmwCruiseTc::WaitForStableSpeed() - Exit");
	return result;
}
