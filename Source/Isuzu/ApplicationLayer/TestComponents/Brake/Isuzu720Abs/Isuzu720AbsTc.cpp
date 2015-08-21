//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "Isuzu720AbsTc.h"

//----------------------------------------------------------------------------
template<class ModuleInterface>
Isuzu720AbsTc<ModuleInterface>::Isuzu720AbsTc() : GenericABSTCTemplate<ModuleInterface>(), m_originalDriveAxle("")
{	// Nothing special to do here
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
Isuzu720AbsTc<ModuleInterface>::~Isuzu720AbsTc()
{	// Nothing special to do here
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string Isuzu720AbsTc<ModuleInterface>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::CommandTestStep(%s) - Enter", value.c_str());
	try
	{	// Make sure it is ok to run the test step
		if(StatusCheck() != BEP_STATUS_SUCCESS)
		{	// System status is not ok, do not perform test
			result = testSkip;
			SendTestResult(result, GetTestStepInfo("Description"));
			Log(LOG_ERRORS, "Isuzu720AbsTc::CommandTestStep() - StatusCheck failed (%s), not performing %s",
				result.c_str(), GetTestStepName().c_str());
		}
		else if(!GetTestStepName().compare("AccelerateToParkBrakeSpeed"))  result = TestStepAccelerateToParkBrakeSpeed();
		else if(!GetTestStepName().compare("AnalyzeDynamicParkBrake"))	   result = AnalyzeDynamicParkBrake();
		else if(!GetTestStepName().compare("DisableForceMeter"))		   result = DisableForceMeter();
		else if(!GetTestStepName().compare("DynamicParkBrake"))			   result = TestStepDynamicParkBrake();
		else if(!GetTestStepName().compare("EnterNormalMode"))			   result = EnterNormalMode();
		else if(!GetTestStepName().compare("IndividualSensorTest"))		   result = IndividualSensorTest();
		else if(!GetTestStepName().compare("AccelerateToABSSpeed"))
		{
			m_baseBrakeTool->UpdateTarget(false);
			result = m_baseBrakeTool->TestStepAccelerate();
		}
		else if(!GetTestStepName().compare("PreRollBrakePedalPrime"))	   result = PreRollBrakePedalPrime();
		else if(!GetTestStepName().compare("PrimeAbs"))                    result = AbsPrime();
		// Call the base class to handle the test step
		else	result = GenericABSTCTemplate<ModuleInterface>::CommandTestStep(value);
		// Check if testing should abort on a failure
		if(GetParameterBool("AbortOnTestStepFailure") && GetTestStepInfoBool("AbortIfFailed") &&
		   result.compare(testPass) && result.compare(testSkip))
		{	// Make sure we abort and then tell the system to abort
			Abort();
			SystemWrite(ABORT_DATA_TAG, string("1"));
		}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "Isuzu720AbsTc.%s: %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &excpt)
	{
		Log(LOG_DEV_DATA, "Isuzu720AbsTc.CommandTestStep %s Exception: %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::CommandTestStep(%s) - Exit", value.c_str());
	return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::DetermineSystemConfiguration(void)
{
	string result(BEP_TESTING_STATUS);
	// Log the entry and determine if this test should be performed
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::DetermineSystemConfiguration() - Enter");
	if(!ShortCircuitTestStep())
	{
		SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
		result = testPass;
	}
	else
	{	// Need to skip this test step
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping Isuzu720AbsTc::DetermineSystemConfiguration()");
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::DetermineSystemConfiguration() - Exit");
	return result;
}

//----------------------------------------------------------------------------
template <class ModuleInterface>
INT32 Isuzu720AbsTc<ModuleInterface>::DynamicParkBrake(INT32 &brakeStart, INT32 &brakeEnd, const std::string &tagPrefix)
{
	INT32 testStatus = BEP_STATUS_NA;
	float wheelForce[4];

	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::DynamicParkBrake()\n");

	try
	{	// update the the park brake force until the speed is below the min sample speed
		float startSampleSpeed = GetTestStepInfoFloat("StartSampleSpeed");
		float endSampleSpeed = GetTestStepInfoFloat("EndSampleSpeed");
		bool  startSamplingAtMinForce = GetTestStepInfoBool("StartSamplingAtMinimumForce");
		float minimumStartForce = GetTestStepInfoFloat("MinimumStartForce");
		int   forceSampleCount = GetTestStepInfoInt("ForceSampleCount");
		int   validForceSamples;
		float speed=0;
		INT32 safeBrakeStart = 0;

		brakeStart = brakeEnd = 0;	// reset the brake array indexes
		validForceSamples = 0;	// No park brake samples taken yet
		// Insert a "safe" start tag in case the operator does not ever hit the brake pedal
		safeBrakeStart = TagArray( tagPrefix+"Start");
		do
		{
			speed = m_baseBrakeTool->GetSpeed();

			// If we want park brake sampling to start/stop based on force/sample count
			if((0 < forceSampleCount) && (true == startSamplingAtMinForce))
			{
				// check if minimum force for sampling is reached
				m_baseBrakeTool->GetIForces(wheelForce);
				if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
				{
					validForceSamples++;
					// If this is the first sample above min force
					if(validForceSamples == 1)
					{	// Tag the start
						brakeStart = TagArray(tagPrefix+"StartForce");
						Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d\n", brakeStart);
					}
					// Wait for the desired number of samples above minimum before we end
					else if(validForceSamples >= forceSampleCount)
					{	// Tag the end
						brakeEnd = TagArray(tagPrefix+"StopForce");
						Log(LOG_DEV_DATA,"Park Brake sample count reached @ %d\n", brakeEnd);
					}
				}
			}
			else
			{	// If we want to "re-start" sampling park brake at minimum force value AND we haven't reached min force yet
				if((true == startSamplingAtMinForce) && (0 == validForceSamples))
				{
					// check if minimum force for sampling is reached
					m_baseBrakeTool->GetIForces(wheelForce);
					if((wheelForce[LFWHEEL] > minimumStartForce) && (wheelForce[RFWHEEL] > minimumStartForce))
					{	// Tag the start
						brakeStart = TagArray(tagPrefix+"StartForce");
						Log(LOG_DEV_DATA,"Minimum park brake force reached @ %d\n", brakeStart);
						validForceSamples = 1;
					}
				}

				// If we have dropped below start speed and we have not tagged start yet
				if((speed <= startSampleSpeed) && (brakeStart == 0))
				{
					brakeStart = TagArray(tagPrefix+"StartSpeed");
					Log(LOG_DEV_DATA,"Start sample speed reached @ %d\n", brakeStart);
				}
				// If we have dropped below stop speed and we have not tagged stop yet
				else if((speed <= endSampleSpeed) && (brakeEnd == 0))
				{
					brakeEnd = TagArray(tagPrefix+"EndSpeed");
					Log(LOG_DEV_DATA,"End sample speed reached @ %d\n", brakeEnd);
				}
			}

			// only need to check every 1/10th of a second
			if(brakeEnd == 0)	 BposSleep(100);

			// check the system status
			testStatus = m_baseBrakeTool->BrakeTestStatusCheck(false);

		} while((TimeRemaining()) &&  (brakeEnd == 0) && (testStatus == BEP_STATUS_SUCCESS));

		// Make sure we have a valid brake start tag
		if( brakeStart == 0)	brakeStart = safeBrakeStart;

		// if brakeEnd has not been set yet, set to brakeStart
		if(brakeEnd == 0) brakeEnd = brakeStart;
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "DynamicParkBrake Exception: %s\n", e.what());
		testStatus = BEP_STATUS_FAILURE;
	}

	Log( LOG_FN_ENTRY, "Exit Isuzu720AbsTc::DynamicParkBrake( %d, %d), status=%d\n", 
		 brakeStart, brakeEnd, testStatus);

	return(testStatus);
}

//----------------------------------------------------------------------------
template <class ModuleInterface>
const string Isuzu720AbsTc<ModuleInterface>::AnalyzeDynamicParkBrake(void)
{
	INT32 status = BEP_STATUS_FAILURE;		// the status of the test being performed
	string testResult;
	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::AnalyzeDynamicParkBrake()\n");
	if(!ShortCircuitTestStep())
	{
		try
		{	// if data sampled correctly display driver prompts if desired
			UpdatePrompts();
			SetTestStepInfoValue("FrontMinForce", GetParameter("DynamicParkBrakeFrontMinForce"));
			SetTestStepInfoValue("FrontMaxForce", GetParameter("DynamicParkBrakeFrontMaxForce"));
			SetTestStepInfoValue("RearMinForce", GetParameter("DynamicParkBrakeRearMinForce"));
			SetTestStepInfoValue("RearMaxForce", GetParameter("DynamicParkBrakeRearMaxForce"));
			// Analyze the data
			status = AnalyzeParkBrakeForces(m_dynParkBrakeStart, m_dynParkBrakeStop);
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "AnalyzeBaseBrake Exception: %s\n", e.what());
			status = BEP_STATUS_SOFTWARE;
		}
		RemovePrompts();
		// update the test status
		UpdateResult(status, testResult);
		SendTestResult(testResult, GetTestStepInfo("Description"));
		SystemWrite(GetDataTag("TestResultBox2"), !testResult.compare(testPass) ? colorGreen : colorRed);
	}
	else
	{	// Skip the test
		Log(LOG_DEV_DATA, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log( LOG_FN_ENTRY, "Exit Isuzu720AbsTc::AnalyzeDynamicParkBrake(): %s, %d\n", testResult.c_str(), status);
	return(testResult);
}

//----------------------------------------------------------------------------
template <class ModuleInterface>
INT32 Isuzu720AbsTc<ModuleInterface>::AnalyzeParkBrakeForces(INT32 brakeStart, INT32 brakeEnd)
{
	INT32           testStatus;
	INT32           wheelStatus;
	WHEELDATAARRAY  wheelForceArray;
	float           force;		  // the current brake force
	char            buffer[256];	// data array for printing
	string          faultCode;	// Code to report to external systems for a failure condition
	string          faultDesc;	// Description of what the test failed for
	int             roller;

	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::AnalyzeBrakeForces(%d,%d)", brakeStart, brakeEnd);
	// Clear out previous force results
	m_parkBrakeForce.clear();
	// read in the data from the brake force array
	testStatus = ReadDataArrays(GetParameter("IcmForceArray"), brakeStart, brakeEnd, wheelForceArray);
	if(BEP_STATUS_SUCCESS == testStatus)
	{	// Check all wheels.  There is no park brake on the front wheels, so force should not be more than
		// normal drag.
		for(roller=LFWHEEL; roller<=RRWHEEL; roller++)
		{	// calculate the average forces and validate the results
			const DATAARRAY &forceArray = wheelForceArray[roller];
			if( forceArray.size() > 0)
			{
				force = 0;
				DATAARRAY::const_iterator   itr;
				for( itr=forceArray.begin(); itr!=forceArray.end(); itr++)
				{
					force += *itr;
				}
				force /= forceArray.size();
				Log(LOG_DEV_DATA, "Isuzu720AbsTc::AnalyzeParkBrakeForces() - step1: Force - %.2f", force);
				wheelStatus = ValidateParkBrakeForce(roller, force);
			}
			else	// if there is a problem averaging the forces display -1
			{
				Log( LOG_ERRORS, "Force array for roller %d has zero length\n", roller);
				// write the value to -1
				force = -1;
				// update the result and the background color of the result on the GUI
				SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), "-1");
				SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), "Red");
			}
			// place the normalized value into the brake force array
			m_parkBrakeForce.push_back(force);

			Log(LOG_DEV_DATA, "Isuzu720AbsTc::AnalyzeParkBrakeForces() - step2");
			// Set the fault codes and descriptions
			const std::string faultTag(rollerName[roller]+"ParkBrakeForceFault");
			const std::string testResultTag(rollerName[roller] + "ParkBrakeForceTest");
			if( BEP_STATUS_SUCCESS == wheelStatus)
			{
				faultCode = "0000";
				faultDesc = rollerName[roller] + " Park Brake Force Test Result";
			}
			else
			{
				faultCode = GetFaultCode(faultTag);
				faultDesc = GetFaultDescription(faultTag);
			}
			// send the test results to the TestResultServer
			memset(buffer, 0 , sizeof(buffer));		  // clear the data array
			SendSubtestResultWithDetail(testResultTag, wheelStatus, faultDesc, faultCode,
										"ParkBrakeForce", CreateMessage(buffer, sizeof(buffer), "%.2f", force), unitsLBF,
										"MaxParkBrakeForce", (roller < 2) ? GetTestStepInfo("FrontMaxForce") : GetTestStepInfo("RearMaxForce"), unitsLBF,
										"MinParkBrakeForce", (roller < 2) ? GetTestStepInfo("FrontMinForce") : GetTestStepInfo("RearMinForce"), unitsLBF);
			if(wheelStatus != BEP_STATUS_SUCCESS)
			{
				testStatus = wheelStatus;
			}
		}
	}
	else
	{
		Log( LOG_ERRORS, "Error reading force array: %d", testStatus);
	}
	// Log the exit and return the result
	Log( LOG_FN_ENTRY, "Exit BaseBrakeTool::AnalyzeBrakeForces(%d,%d), status=%d", brakeStart, brakeEnd, testStatus);
	return(testStatus);
}

//----------------------------------------------------------------------------
template <class ModuleInterface>
INT32 Isuzu720AbsTc<ModuleInterface>::ValidateParkBrakeForce(INT32 roller, float average)
{
	INT32 testStatus = BEP_STATUS_FAILURE;
	string color("Red");

	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::ValidateParkBrakeForce(%d, %.2f)",roller, average);
	// read the parameters to compare the average too
	float minForce = (roller < 2) ? GetTestStepInfoFloat("FrontMinForce") : GetTestStepInfoFloat("RearMinForce");
	float maxForce = (roller < 2) ? GetTestStepInfoFloat("FrontMaxForce") : GetTestStepInfoFloat("RearMaxForce");

	Log( LOG_DEV_DATA, "ValidateParkBrakeForce(%d, %f), min: %f, max: %f\n", roller, average, minForce, maxForce);

	// if brake force is in the valid limits set ok
	if( ((average <= maxForce) || (maxForce <= 0)) && (average >= minForce))
	{
		testStatus = BEP_STATUS_SUCCESS;
		color = "Green";
	}
	// update the result and the background color of the result on the GUI
	SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceValue"), average);
	SystemWrite(GetDataTag(rollerName[roller] + "ParkBrakeForceBGColor"), color);
	// Log the exit and return the result
	Log( LOG_FN_ENTRY, "Exit Isuzu720AbsTc::ValidateParkBrakeForce(%d, %.2f), status=%d",roller, average, testStatus);
	return(testStatus);
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::EnterNormalMode(void)
{
	string result(BEP_TESTING_STATUS);
	// Log the entry and check if the test step should be performed
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::EnterNormalMode() - Enter");
	if(!ShortCircuitTestStep())
	{	// Command the module to enter normal mode
		BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnterNormalMode");
		result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
		Log(LOG_ERRORS, "Commanded module to enter normal mode: %s (status: %s)", 
			result.c_str(), ConvertStatusToResponse(status).c_str());
		// Report the result
		SendTestResult(result, GetTestStepInfo("Description"));
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping test step EnterNormalMode");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::EnterNormalMode() - Exit");
	return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::DisableForceMeter(void)
{
	// Disable brake force meter updates
	m_baseBrakeTool->DisableForceUpdates();
	// Deactivate the brake force meter
	SystemWrite(GetDataTag("BrakeActive"), 0);
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::DisableForceMeter() - Disabled brake force meter");
	return testPass;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::IndividualSensorTest(void)
{

	// Log the entry and determine if this test should be performed
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::IndividualSensorTest() - Enter");
	if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
	{	// Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
		m_MotorController.Write(COMMAND_SPEED, "0", false);
		m_MotorController.Write(COMMAND_TORQUE, "0", false);
		m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
		BposSleep( 100);
		string sensorResult[GetRollerCount()];
		DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
		DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
		BposSleep(GetParameterInt("IndividualSensorSpeedDriverDelay"));
		// Wait for the operator to remove foot from brake pedal
		bool brakeOn = true;
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		do
		{	// Read the brake switch styatus from the module
			status = m_vehicleModule.ReadModuleData("ReadBrakeSwitch", brakeOn);
			if(brakeOn)	BposSleep(500);
			// Keep checking until time expires or brake off or bad status
		} while(TimeRemaining() && brakeOn && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status));
		// Make sure it is still OK to continue with the test
		if(!brakeOn && (BEP_STATUS_SUCCESS == StatusCheck()))
		{	// Get individual wheel speed from config to send to motor controller instead of above (one speed for all axles)
			for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
			{
				m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", GetParameter("Individual" + rollerName[rollerIndex] + "SensorTestSpeed"), true);
			}
			// Wait a bit for the rollers to start accelerating
			BposSleep(GetParameterInt("IndividualSensorTestStartDelay"));
			// Stop the keep alive
			StopModuleKeepAlive();
			// Read the sensor speeds from the module
			WheelSpeeds_t moduleSpeeds;
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
			// Start the Keep Alive thread
			StartModuleKeepAlive();
			if(BEP_STATUS_SUCCESS == status)
			{	// Get the roller speeds
				float rollerSpeeds[GetRollerCount()];
				GetWheelSpeeds(rollerSpeeds);
				// Check all wheels
				float maxSpeeds[GetRollerCount()];
				float minSpeeds[GetRollerCount()];
				string indResults[GetRollerCount()];	//ERIC: NOT SURE IF I SHOULD HARD CODE A 4 OR LOOP TILL GetRollerCount()
				for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
				{
					maxSpeeds[rollerIndex] = rollerSpeeds[rollerIndex] * (1.0 + (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));
					minSpeeds[rollerIndex] = rollerSpeeds[rollerIndex] * (1.0 - (GetParameterFloat("IndividualSensorSpeedTolerance") / 100.0));
					sensorResult[rollerIndex] = (rollerSpeeds[rollerIndex] >= (GetParameterFloat("Individual" + rollerName[rollerIndex] + "SensorTestSpeed") - 1.0)) ? testPass : testFail;
				}

				for(UINT8 wheel = LFWHEEL; (wheel <= RRWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); wheel++)
				{
					if(!sensorResult[wheel].compare(testPass))
					{
						sensorResult[wheel] = ((minSpeeds[wheel] <= moduleSpeeds[wheel]) && (moduleSpeeds[wheel] <= maxSpeeds[wheel])) ? testPass : testFail;
					}

					// Log and send sub wheel test result
					Log(LOG_DEV_DATA, "%s: %s - Sensor Speed: %.2f, Roller Speed: %.2f, [min: %.2f, max: %.2f]", 
						rollerName[wheel].c_str(), sensorResult[wheel].c_str(), moduleSpeeds[wheel], 
						rollerSpeeds[wheel], minSpeeds[wheel], maxSpeeds[wheel]);
					// Report this roller
					char buff[32];
					SendSubtestResultWithDetail(rollerName[wheel]+"SensorSpeedTest", sensorResult[wheel], 
												GetTestStepInfo("Description"), "0000",
												"SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[wheel]), unitsMPH,
												"MinLimit", CreateMessage(buff, sizeof(buff), "%.2f", minSpeeds[wheel]), unitsMPH,
												"MaxLimit", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeeds[wheel]), unitsMPH,
												"RollerSpeed", GetParameter("Individual" + rollerName[wheel] + "SensorTestSpeed"), unitsMPH);
				}                
			}
			else
			{	// Could not read sensor speeds
				Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
				for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL); rollerIndex++)
				{
					sensorResult[rollerIndex] = testFail;
				}
			}
			// Remove the prompts
			RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
			RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
			// Set the motor back to boost and zero speed
			for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL); rollerIndex++)
			{
				m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", "0", true);
			}
			// Wait for zero speed
			CheckZeroSpeed();
			m_MotorController.Write(COMMAND_SPEED, "0", false);
			m_MotorController.Write(COMMAND_TORQUE, "0", false);
			m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
			BposSleep( 100);
		}
		else
		{	// Could not perform the test
			Log(LOG_ERRORS, "Could not perform sensor test - brake applied: %s,  System Status: %s",
				brakeOn ? "True" : "False", ConvertStatusToResponse(StatusCheck()).c_str());
		}
		// Remove the prompts
		RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
		RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
		// Report the overall result
		testResult = (!sensorResult[LFWHEEL].compare(testPass) && !sensorResult[RFWHEEL].compare(testPass) &&
					  !sensorResult[LRWHEEL].compare(testPass) && !sensorResult[RRWHEEL].compare(testPass)) ? testPass : testFail;
		SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	}
	else if(!GetTestStepResult().compare(testPass))
	{	// This test already passed, skip it but leave it marked as pass
		Log(LOG_DEV_DATA, "Already passed sensor test, not testing again");
		testResult = testPass;
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping sensor test");
		testResult = testSkip;
	}
	// Log the exit and return the result.
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::IndividualSensorTest() - Exit");
	return testResult;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string& Isuzu720AbsTc<ModuleInterface>::OriginalDriveAxle(const string *driveAxle /*= NULL*/)
{
	if(driveAxle != NULL)  m_originalDriveAxle = *driveAxle;
	return m_originalDriveAxle;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string Isuzu720AbsTc<ModuleInterface>::TestStepAccelerateToParkBrakeSpeed(void)
{
	string testStatus(testError);
	string armSpeed( GetParameter("DynamicParkBrakeArmSpeed"));
	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::TestStepAccelerateToParkBrakeSpeed(%s)", armSpeed.c_str());
	// Disable the brake meter while we accelerate
	m_baseBrakeTool->UpdateTarget( false);
	// Make sure the machine is engaged
	if(BEP_STATUS_SUCCESS == Engage())
	{	// Copy the dynamic park brake parameters to test step info
		SetTestStepInfoValue( "ArmSpeed", armSpeed);
		testStatus = m_baseBrakeTool->TestStepAccelerate();
	}
	else
	{
		Log( LOG_ERRORS, "Error clutching the machine");
	}
	// Log the exit and return the result
	Log( LOG_FN_ENTRY, "Exit Isuzu720AbsTc::TestStepAccelerateToParkBrakeSpeed(%s), status=%s", 
		 armSpeed.c_str(), testStatus.c_str());
	return( testStatus);
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string Isuzu720AbsTc<ModuleInterface>::TestStepDynamicParkBrake(void)
{
	INT32   testStatus = BEP_STATUS_SUCCESS;		// the status of the test being performed
	char    buff[ 256];
	float   minForce, maxForce, target;
	string  testResult;

	Log( LOG_FN_ENTRY, "Enter Isuzu720AbsTc::TestStepDynamicParkBrake()");
	if(!ShortCircuitTestStep())
	{
		(void)GetTestStepParameter("DynamicParkBrakeRearMinForce", minForce);
		(void)GetTestStepParameter("DynamicParkBrakeRearMaxForce", maxForce);
		target = (minForce + maxForce) / 2;

		SetTestStepInfoValue( "StartSampleSpeed", GetParameter("DynamicParkBrakeStartSampleSpeed"));
		SetTestStepInfoValue( "EndSampleSpeed", GetParameter("DynamicParkBrakeEndSampleSpeed"));
		SetTestStepInfoValue( "MinimumStartForce", minForce);
		SetTestStepInfoValue( "ForceSampleCount", GetParameter("DynamicParkBrakeSampleCount"));
		SetTestStepInfoValue( "StartSamplingAtMinimumForce", GetParameter("DynamicParkBrakeSampleUseForces"));
		SetTestStepInfoValue( "BrakeDomain", CreateMessage( buff, sizeof( buff), "0 %.02f", 2*target));
		SetTestStepInfoValue( "BrakeTarget", CreateMessage( buff, sizeof( buff), "%.02f %.02f", minForce, maxForce));

		try
		{
			// set up the display (brake force gauge)
			m_baseBrakeTool->UpdateTarget();
			// if gauge set up
			if(testStatus == BEP_STATUS_SUCCESS)
			{	// Disengage the machine
				Disengage();
				while(BEP_STATUS_SUCCESS != m_baseBrakeTool->BrakeTestStatusCheck(false)) BposSleep(100);
				// display driver prompts
				UpdatePrompts();
				// variables to hold the index values for analyzing
				m_dynParkBrakeStart = m_dynParkBrakeStop = 0;
				testStatus = DynamicParkBrake(m_dynParkBrakeStart, m_dynParkBrakeStop, "DynamicParkBrake");
				// Remove the prompts we displayed
				RemovePrompts();
				// Determine if the brake force meter should be disabled
				if(!GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
				{
					m_baseBrakeTool->UpdateTarget(false);
				}
			}
			else
			{
				Log(LOG_ERRORS, "Invalid Machine Status For Isuzu720AbsTc::TestStepDynamicParkBrake %d", testStatus);
			}
		}
		catch(BepException &e)
		{
			Log(LOG_ERRORS, "TestStepDynamicParkBrake Exception: %s\n", e.what());
			testStatus = BEP_STATUS_SOFTWARE;
		}
		// set the brake gauge to inactive if we are not displaying meter until zero speed
		if(!GetParameterBool("DisplayBrakeMeterUntilZeroSpeed"))
		{
			SystemWrite(GetDataTag("BrakeActive"), 0);
		}
		// update the test status
		UpdateResult(testStatus, testResult);
		SendTestResult(testResult, GetTestStepInfo("Description"));
	}
	else
	{
		// Skip the test
		Log(LOG_DEV_DATA, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log( LOG_FN_ENTRY, "Exit Isuzu720AbsTc::TestStepDynamicParkBrake(): %s, %d", testResult.c_str(), testStatus);
	return(testResult);
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::PreRollBrakePedalPrime(void)
{
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::PreRollBrakePedalPrime() - Enter");
	if(!ShortCircuitTestStep() && (GetTestStepResult() != testPass))
	{
		for(int index = 0; 
		   (index < GetParameterInt("PreRollBrakePresses")) && (BEP_STATUS_SUCCESS == StatusCheck());
		   index++)
		{
			DisplayPrompt(GetPromptBox("ApplyBrake1"), GetPrompt("ApplyBrake1"), GetPromptPriority("ApplyBrake1"));
			result = BrakeSwitchTest(GetDataTag("SwitchOn"));
			RemovePrompt(GetPromptBox("ApplyBrake1"), GetPrompt("ApplyBrake1"), GetPromptPriority("ApplyBrake1"));
			DisplayPrompt(GetPromptBox("ReleaseParkBrake1"), GetPrompt("ReleaseParkBrake1"), GetPromptPriority("ReleaseParkBrake1"));
			result = BrakeSwitchTest(GetDataTag("SwitchOff"));
			RemovePrompt(GetPromptBox("ReleaseParkBrake1"), GetPrompt("ReleaseParkBrake1"), GetPromptPriority("ReleaseParkBrake1"));
		}
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping PreRollBrakePedalPrime");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::PreRollBrakePedalPrime() - Exit");
	return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string Isuzu720AbsTc<ModuleInterface>::AbsPrime(void)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::AbsPrime() - Enter");
	if(!ShortCircuitTestStep())
	{	// Wait for zero speed
		if(CheckZeroSpeed())
		{
			BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
			bool brakeSwitchOn = false;
			DisplayPrompt(GetPromptBox("AbsPrime"), GetPrompt("AbsPrime"), GetPromptPriority("AbsPrime"));
			DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
			// Look for the brake switch in the desired position
			while(TimeRemaining() && !brakeSwitchOn && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus))
			{
				moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeSwitchOn);
				if(!brakeSwitchOn)	BposSleep(GetTestStepInfoInt("ScanDelay"));
			} 
			if(moduleStatus != BEP_STATUS_SUCCESS)
			{
				result = testFail;
			}
			else if(!TimeRemaining())
			{
				result = testFail;
			}
			else if(brakeSwitchOn)
			{	
				for(int cycle = 0; 
					 (cycle < GetParameterInt("AbsPrimeCycles")) && 
					 (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus);
					 cycle++)
				{
					BposSleep(GetTestStepInfoInt("ScanDelay"));
					moduleStatus = m_vehicleModule.CommandModule("DumpAllValves");
					BposSleep(GetTestStepInfoInt("ScanDelay"));
					moduleStatus = m_vehicleModule.CommandModule("RecoverAllValves");

				}
				if(moduleStatus == BEP_STATUS_SUCCESS)
				{
					result = testPass;
				}
				else
				{
					result = testFail;
				}
			}
			RemovePrompt(GetPromptBox("AbsPrime"), GetPrompt("AbsPrime"), GetPromptPriority("AbsPrime"));
			RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
		}
		else
		{	// Timeout waiting for zero speed
			result = testTimeout;
			Log(LOG_ERRORS, "Timeout waiting for zero speed");
		}
		Log(LOG_DEV_DATA, "Prime Abs Pump: %s \n", result.c_str());
		// Send the test result
		SendTestResult(result, GetTestStepInfo("Description"), "000");
	}
	else
	{
		Log(LOG_DEV_DATA, "Skipping ABS Prime");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "Isuzu720AbsTc::AbsPrime() - Exit");
	return result;
}
