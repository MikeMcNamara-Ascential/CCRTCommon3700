//******************************************************************************
// Description:
//  RWAL Brake test component class
//
//==============================================================================
// Copyright (c) 2012 - Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "RwalBrakeTest.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
RwalBrakeTest<ModuleType>::RwalBrakeTest() : GenericABSTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
RwalBrakeTest<ModuleType>::~RwalBrakeTest()
{
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string RwalBrakeTest<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "RwalBrakeTest::CommandTestStep(value: %s) - Enter", value.c_str());
	try
	{   // Make sure the system status is ok before trying to run this test step
		if(BEP_STATUS_SUCCESS != StatusCheck())
		{   // System status is not ok, do not perform the test step
			result = testSkip;
			SendTestResult(result, GetTestStepInfo("Description"));
			Log(LOG_ERRORS, "RwalBrakeTest::CommandTestStep() - Status Check failed (%s), not performing %s",
				ConvertStatusToResponse(StatusCheck()).c_str(), GetTestStepName().c_str());
		}
		else if(!GetTestStepName().compare("RwalSensorTest"))   result = ValidateRwalSensor();
		else       GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BepException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "RwalBrakeTest::CommandTestStep(value: %s) - Exit", value.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string RwalBrakeTest<ModuleType>::ValidateRwalSensor(void)
{   // Log the entry and check if this step should be performed
	Log(LOG_FN_ENTRY, "RwalBrakeTest::ValidateRwalSensor() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{
	    INT32 sampleCount = 0;
		float currentSample = 0.0;
		float currentRollerSpeed = 0.0;
		float runningTotal = 0.0;
		float runningRollerSpeed = 0.0;
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		TestResultDetails details;
		result = AccelerateToTestSpeed(GetParameterFloat("RwalTestSpeed"), GetParameter("RwalTestSpeedRange"),
									   GetTestStepInfoInt("ScanDelay"), false);
		if(!result.compare(testPass))
		{   // Keep the green band displayed during the test
			DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
						  GetPromptPriority("MaintainSpeed"));
			SystemWrite(GetDataTag("SpeedTarget"), GetParameter("RwalTestSpeedRange"));
			do
			{	// Read the current sample from the module
				currentRollerSpeed = GetRollSpeed();
				status = m_vehicleModule.ReadModuleData(GetDataTag("ReadRwalMessageTag"), currentSample);
				if(BEP_STATUS_SUCCESS == status)
				{	// Log the current sample and add to the running total
					runningTotal += currentSample;
					runningRollerSpeed += currentRollerSpeed;
					Log(LOG_DEV_DATA, "RWAL Sensor Sample %03d:  %.2f,  total: %.2f,  roller Speed: %.2f", 
						sampleCount, currentSample, runningTotal, currentRollerSpeed);
					sampleCount++;
					BposSleep(GetParameterInt("RwalSensorCheckInterval"));
				}
				else
				{
					Log(LOG_ERRORS, "Error reading RWAL sensor from the module: %s", 
						ConvertStatusToResponse(status).c_str());
					result = testFail;
				}
			} while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) &&
					(BEP_STATUS_SUCCESS == status) &&
					(sampleCount < GetParameterInt("TotalRwalSamples")));
			// Determine the result of the sensor test
			if(sampleCount)
			{	// Get the average RWAL speed
				float averageSensorSpeed = runningTotal / (float)sampleCount;
				float averageRollerSpeed = runningRollerSpeed / (float)sampleCount;
				float lowLimit = averageRollerSpeed * ((100 - GetParameterFloat("RwalSensorTolerance")) / 100.0);
				float hiLimit = averageRollerSpeed * ((100 + GetParameterFloat("RwalSensorTolerance")) / 100.0);
				result = ((lowLimit <= averageSensorSpeed) && (averageSensorSpeed <= hiLimit)) ? testPass : testFail;
				Log(LOG_DEV_DATA, "RWAL Sensor:  %.2f  [%.2f  %.2f] - %s,  Average Roller Speed: %.2f",
					averageSensorSpeed, lowLimit, hiLimit, result.c_str(), averageRollerSpeed);
				char buff[64];
				details.AddDetail("RwalSensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", averageSensorSpeed), unitsMPH);
				details.AddDetail("RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", averageRollerSpeed), unitsMPH);
				details.AddDetail("LowLimit", CreateMessage(buff, sizeof(buff), "%.2f", lowLimit), unitsMPH);
				details.AddDetail("HighLimit", CreateMessage(buff, sizeof(buff), "%.2f", hiLimit), unitsMPH);
				details.AddDetail("SampleCount", CreateMessage(buff, sizeof(buff), "%d", sampleCount), "");
			}
			else
			{	// No samples collected, cannot analyze anything
				result = testFail;
				Log(LOG_DEV_DATA, "No RWAL samples collected");
			}
			RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
						 GetPromptPriority("MaintainSpeed"));
			SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
		}
		else
		{   // Could not achieve test speed
			Log(LOG_ERRORS, "Failed to reach RWAL test speed");
			result = testFail;
		}
		// Report the results
		SendTestResultWithDetail(result, details, GetTestStepInfo("Description"), "0000");
		// Make sure to return to zero speed
		CheckZeroSpeed();
	}
	else
	{   // Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping ValidateRwalSensor()");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "RwalBrakeTest::ValidateRwalSensor() - Exit");
	return result;
}
