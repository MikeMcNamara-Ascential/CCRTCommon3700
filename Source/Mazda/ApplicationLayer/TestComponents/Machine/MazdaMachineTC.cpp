//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
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
#include "MazdaMachineTC.h"

//-------------------------------------------------------------------------------------------------
MazdaMachineTC::MazdaMachineTC() : MachineTC()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaMachineTC::~MazdaMachineTC()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const INT32 MazdaMachineTC::HandlePulse(const INT32 code, const INT32 value)
{
	INT32 retVal = BEP_STATUS_FAILURE;
	Log(LOG_FN_ENTRY, "MazdaMachineTC::HandlePulse(code: %d, value: %d) - Enter", code, value);
	switch(code)
	{
	case MAZDA_MACHINE_TC_PULSE_CODE:
		switch(value)
		{
		case MAX_SPEED_CHECK_PULSE:
			retVal = CheckForMaxSpeed();
			break;

		case ODOMETER_UPDATE_PULSE:
			retVal = UpdateDistanceTraveled();
			break;

		default:
			Log(LOG_ERRORS, "Pulse value %d is not supported", value);
			break;
		}
		break;

	default:
		retVal = BepServer::HandlePulse(code, value);
		break;
	}
	Log(LOG_FN_ENTRY, "MazdaMachineTC::HandlePulse(code: %d, value: %d) - Exit", code, value);
	return retVal;
}

//=============================================================================
void MazdaMachineTC::Initialize(const XmlNode *config)
{
	// call the base class initialize
	MachineTC::Initialize(config);

	// Initialize the base component
	Log(LOG_FN_ENTRY, "Initializing The MazdaMachineTC Component\n");

	try
	{
		SetupTimer(config->getChild("Setup/Parameters/MaxSpeedUpdateTimer"), 
				   m_maxSpeedTimer, MAX_SPEED_CHECK_PULSE);
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error getting max speed update rate, using 500ms: %s", excpt.GetReason());
		SetupTimer(500, m_maxSpeedTimer, MAX_SPEED_CHECK_PULSE);
	}
	// Set the maximum front and rear axle speeds observed
	m_maxFrontSpeedObserved = 0.0;
	m_maxRearSpeedObserved = 0.0;

	try 
	{
		SetupTimer(config->getChild("Setup/Parameters/CurrentDistanceTraveledUpdateTimer"), 
				   m_currentDistTimer, ODOMETER_UPDATE_PULSE);
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Error getting timer update rate for distance traveled, using 1000ms - %s", excpt.GetReason());
		SetupTimer(1000, m_currentDistTimer, ODOMETER_UPDATE_PULSE);
	}

	try
	{
		m_sideSlipResultData.DeepCopy(m_parameters.getNode("SideSlipData")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Side slip result data not defined: %s", excpt.GetReason());
		m_sideSlipResultData.clear(true);
	}

	Log(LOG_FN_ENTRY, "Done Initializing MazdaMachineTC\n");

}

//-------------------------------------------------------------------------------------------------
INT32 MazdaMachineTC::CheckForMaxSpeed(void)
{
	WHEELINFO rollerSpeeds;
	BEP_STATUS_TYPE status = GetWheelSpeeds(rollerSpeeds);
	if(BEP_STATUS_SUCCESS == status)
	{	// Take the average axle speed
		float frontAvg = (rollerSpeeds.lfWheel + rollerSpeeds.rfWheel) / 2.0;
		float rearAvg = (rollerSpeeds.lrWheel + rollerSpeeds.rrWheel) / 2.0;
		// Determine if we have a new maximum
		if(frontAvg > m_maxFrontSpeedObserved)
		{
			m_maxFrontSpeedObserved = frontAvg;
			Log(LOG_DEV_DATA, "Set new max front axle speed: %.2f", m_maxFrontSpeedObserved);
		}
		if(rearAvg > m_maxRearSpeedObserved)
		{
			m_maxRearSpeedObserved = rearAvg;
			Log(LOG_DEV_DATA, "Set new max rear axle speed: %.2f", m_maxRearSpeedObserved);
		}
	}
	else
	{
		Log(LOG_ERRORS, "Could not read roller speeds");
	}
	return status;
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineTC::CheckForValidRLSType()
{
	bool runRlsTest = atob(SystemRead(GetDataTag("RlsEquippedTag")).c_str());
	Log(LOG_DEV_DATA, "Vehicle equipped with RLS: %s", runRlsTest ? "True" : "False");
	SetRLSEquipped(runRlsTest);
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::CommandTestStep(const string &value)
{
	string testResult(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "MazdaMachineTC::CommandTestStep(value: %s) - Enter", value.c_str());
	if(BEP_STATUS_SUCCESS == StatusCheck())
	{
		try
		{
			if(!GetTestStepName().compare("WaitForAcceleration"))				testResult = WaitToStart();
			else if(!GetTestStepName().compare("RainLightSensorVerification"))	testResult = TestStepRainLightSensorVerification();
			else if(!GetTestStepName().compare("ReportSideSlipResults"))		testResult = ReportSideSlipResults();
			else if(!GetTestStepName().compare("StartMaxSpeedObservation"))
			{
				m_maxFrontSpeedObserved = 0.0;
				m_maxRearSpeedObserved = 0.0;
				m_maxSpeedTimer.Start();
				testResult = testPass;
			}
			else if(!GetTestStepName().compare("StartOdometerTest"))
			{	// Set the initial roller distances
				GetWheelDistances(m_odoStartDistance);
				m_currentDistTimer.Start();
				Log(LOG_DEV_DATA, "Started odometer update timer");
				testResult = testPass;
			}
			else if(!GetTestStepName().compare("StartTest"))					testResult = StartTest();
			else if(!GetTestStepName().compare("StopMaxSpeedObservation"))		testResult = StopMaxAxleSpeedObservation();
			else if(!GetTestStepName().compare("StopOdometerTest"))				testResult = StopOdometerTest();
			else if(!GetTestStepName().compare("VehicleDisconnect"))			testResult = VehicleDisconnect();
			else if(!GetTestStepName().compare("VehicleTestSetup"))				testResult = VehicleSetup();
			else   testResult = MachineTC::CommandTestStep(value);
		}
		catch(BepException &excpt)
		{
			Log(LOG_ERRORS, "BepException sequencing test step %s - %s", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testFail;
		}
	}
	else
	{
		Log(LOG_ERRORS, "Bad system status, not starting test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MazdaMachineTC::CommandTestStep(value: %s) - Exit", value.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaMachineTC::ReportSideSlipResults(void)
{
	Log(LOG_FN_ENTRY, "MazdaMachineTC::ReportSideSlipResults() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{	// Get the AON for locating the side slip results
		string path = getenv("USR_ROOT") + GetParameter("SideSlipResultPath");
		XmlParser parser;
		TestResultDetails details;
		try
		{
			const XmlNode *ssResults = parser.ReturnXMLDocument(path + "/" + SystemRead("Aon"));
			for(XmlNodeMapCItr iter = m_sideSlipResultData.begin(); iter != m_sideSlipResultData.end(); iter++)
			{	// Report each node
				string nodeName(iter->second->getAttribute("Node")->getValue());
				string resultAttrib(iter->second->getAttribute("ResultAttribute")->getValue());
				string reportTag(iter->second->getAttribute("ReportTag")->getValue());
				string unitsAttrib(iter->second->getAttribute("UnitsAttribute")->getValue());
				string resultValue;
				string reportResultValue;
				string units;
				if(!resultAttrib.compare(BEP_RESULT))
				{
					resultValue = ssResults->getChild(nodeName)->getAttribute(resultAttrib)->getValue();
					reportResultValue = resultValue;
					resultValue = !resultValue.compare(testPass) ? "1" : "0";
					units = "";
				}
				else
				{
					resultValue = ssResults->getChild(nodeName)->getValue();
					reportResultValue = resultValue;
					units = ssResults->getChild(nodeName)->getAttribute(unitsAttrib)->getValue();
				}
				SystemWrite(reportTag, resultValue);
				Log(LOG_DEV_DATA, "Reported %s = %s", reportTag.c_str(), resultValue.c_str());
				details.AddDetail(reportTag, reportResultValue, units);
			}
		}
		catch(XmlException &xmlExcpt)
		{
			Log(LOG_ERRORS, "XmlException processing side slip results - %s", xmlExcpt.GetReason());
		}
		catch(BepException &bepExcpt)
		{
			Log(LOG_ERRORS, "BepException processing side slip results - %s", bepExcpt.GetReason());
		}
		// Report the results
		SendTestResultWithDetail(testPass, details, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_FN_ENTRY, "Not reporting side slip results");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "MazdaMachineTC::ReportSideSlipResults() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaMachineTC::StopMaxAxleSpeedObservation(void)
{
	Log(LOG_DEV_DATA, "MazdaMachineTC::StopMaxAxleSpeedObservation() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{	// Stop the timer
		m_maxSpeedTimer.Stop();
		// Report the maximum axle speeds
		Log(LOG_DEV_DATA, "Max front axle speed: %.2f \t Max rear axle speed: %.2f", 
			m_maxFrontSpeedObserved, m_maxRearSpeedObserved);
		char buff[32];
		SendTestResultWithDetail(testPass, GetTestStepInfo("Description"), "0000",
								 "MaxFrontAxleSpeed", CreateMessage(buff, sizeof(buff), "%.2f", m_maxFrontSpeedObserved), unitsMPH,
								 "MaxRearAxleSpeed", CreateMessage(buff, sizeof(buff), "%.2f", m_maxRearSpeedObserved), unitsMPH);
		// Write the data to the PLC in km/h * 10
		UINT16 frontSpeed = (UINT16)((m_maxFrontSpeedObserved * KPH_MPH * 10.0) + 0.5);
		UINT16 rearSpeed = (UINT16)((m_maxRearSpeedObserved * KPH_MPH * 10.0) + 0.5);
		Log(LOG_DEV_DATA, "Writing speed to PLC - Front %d [0x%02X],  Rear: %d [0x%02X]",
			frontSpeed, frontSpeed, rearSpeed, rearSpeed);
		SystemWrite(GetDataTag("FrontMaxSpeed"), frontSpeed);
		SystemWrite(GetDataTag("RearMaxSpeed"), rearSpeed);
		result = testPass;
	}
	else
	{	// Do not need to perform this test
		Log(LOG_FN_ENTRY, "Skipping %s", GetTestStepName().c_str());
		result = testSkip;
	}
	Log(LOG_DEV_DATA, "MazdaMachineTC::StopMaxAxleSpeedObservation() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaMachineTC::StopOdometerTest(void)
{
	Log(LOG_DEV_DATA, "MazdaMachineTC::StopOdometerTest() - Enter");
	string result(BEP_TESTING_RESPONSE);
	m_currentDistTimer.Stop();
	if(!ShortCircuitTestStep())
	{	// Get the current roller distance
		WHEELINFO finalDistance, totalDistance;
		GetWheelDistances(finalDistance);
		// Calculate the distance traveled
		GetTotalDistances(totalDistance, m_odoStartDistance, finalDistance);
		float driveWheelDist = !SystemRead(DRIVE_AXLE_TAG).compare(FRONT_WHEEL_DRIVE_VALUE) ? totalDistance.lfWheel : totalDistance.lrWheel; 
		float odometer = ConvertPulsesToMiles(driveWheelDist) * KM_MILES;
		Log(LOG_DEV_DATA, "Total km driven: %.2f", odometer);
		INT16 distanceTraveled = (INT16)((odometer * 10.0) + 0.5);
		SystemWrite("TestDistanceTraveled", distanceTraveled);
		result = testPass;
		char buff[16];
		SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000", 
								 "DistanceTraveled", CreateMessage(buff, sizeof(buff), "%.2f", odometer), unitsKM);
	}
	else
	{
		result = testSkip;
		Log(LOG_DEV_DATA, "Skipping odometer calculation");
	}
	Log(LOG_DEV_DATA, "MazdaMachineTC::StopOdometerTest() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepSpeedometer(const string &value)
{
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepSpeedometer() - Enter");
	string testDescription(GetTestStepInfo("Description"));
	// Set the result box color to let operator know testing has started
	SystemWrite(GetDataTag("SpeedometerTestSpeedBGColor"), colorYellow);
	// Get the vehicle build data
	float minSpeed = GetParameter("SpeedometerSpeedTargetLow");
	float maxSpeed = GetParameter("SpeedometerSpeedTargetHi");
	char buff[32];
	string dispMaxSpeed(CreateMessage(buff, sizeof(buff), "%.0f", maxSpeed));
	string dispMinSpeed(CreateMessage(buff, sizeof(buff), "%.0f", minSpeed));
	string speedRange(dispMinSpeed + " " + dispMaxSpeed);
	// Wait for the operator to get to the correct speed range - divide by speed scale since the base class functions in mph.
	DisplayPrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
	DisplayPrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"),
				  colorWhite, string(dispMinSpeed + " - " + dispMaxSpeed));
	string testResult = (WaitForTargetSpeed(maxSpeed + 10.0, DOWN, GetTestStepInfoInt("ScanDelay")) ==
						 BEP_STATUS_SUCCESS) ? testPass : testFail;
	SystemWrite(GetDataTag("SpeedTarget"), speedRange);
	// Get the wheel speeds
	float currentSpeed = GetRollSpeed();
	// Make sure operator is in the correct speed range
	if(!testResult.compare(testPass))
	{	// Vehicle is in speed range, prompt operator to flash headlamps
		SetStartTime();
		bool done = false;
		// Wait for the operator to flash high beams
		while(!done && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS))
		{	// Get the current wheel speeds
			currentSpeed = GetRollSpeed();
			// Check if the highbeams have been flashed yet
			if(!ReadSubscribeData(GetDataTag("StartSpeedoTest")).compare("1"))
			{	// Highbeams flashed, update the value on the screen
				SystemWrite(GetDataTag("SpeedometerTestResultSpeed"), currentSpeed);
				// Determine if the test passed or failed
				string resultColor;
				if((minSpeed <= currentSpeed) && (currentSpeed <= maxSpeed))
				{	// Test has passed
					resultColor = colorGreen;
					testResult = testPass;
					done = true;
				}
				else
				{	// Test failed
					resultColor = colorRed;
					testResult = testFail;
					testDescription = "Speedometer out of range";
					BposSleep(GetTestStepInfoInt("ScanDelay"));
				}
				// Update screen colors
				SystemWrite(GetDataTag("SpeedometerTestResultBox"), resultColor);
				SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), resultColor);
			}
			else
			{	// Need to wait a bit
				BposSleep(GetTestStepInfoInt("ScanDelay"));
			}
		}
		// Check if there was an abnormal exit condition
		if(!done)
		{	// Set test to fail
			SystemWrite(GetDataTag("SpeedometerTestResultBox"), colorRed);
			SystemWrite(GetDataTag("SpeedometerTestResultSpeedBgColor"), colorRed);
			testResult = testFail;
			testDescription = "Speedometer test timeout";
			Log(LOG_ERRORS, "Speedometer test incomplete - TimeRemaining: %s, StatusCheck: %s", 
				TimeRemaining() ? "True" : "False", ConvertStatusToResponse(StatusCheck()).c_str());
		}
		// Remove prompts
		RemovePrompt(GetPromptBox("FlashHighbeams1"), GetPrompt("FlashHighbeams1"), GetPromptPriority("FlashHighbeams1"));
		RemovePrompt(GetPromptBox("FlashHighbeams2"), GetPrompt("FlashHighbeams2"), GetPromptPriority("FlashHighbeams2"));
	}
	else
	{	// Error accelerating to speed
		Log(LOG_ERRORS, "Could not achieve speed range, not performing speedometer test");
	}
	// Remove the speed target
	SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
	// Report the result, log the exit and return
	SendTestResultWithDetail(testResult, testDescription, "0000",
							 "RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", GetRollSpeed()),
							 (speedScaling != 1.0) ? unitsKPH : unitsMPH);
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepSpeedometer() - Exit");
	return testResult;
}

const string MazdaMachineTC::StartTest(void)
{

	Log(LOG_FN_ENTRY, "MazdaMachineTC::StartTest() - Enter");

	string testResult(BEP_TESTING_RESPONSE);
	DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
	testResult = GenericTC::OperatorPassFail(GetPrompt("StartTest"), GetParameterInt("StartTestTimeoutPrompt")); 
	RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));

	testResult = GenericTC::OperatorPassFail(GetPrompt("StartTest"), GetParameterInt("StartTestTimeoutPrompt")); 

	Log(LOG_FN_ENTRY, "MazdaMachineTC::StartTest() - Exit - %s", testResult.c_str());

	return BEP_PASS_RESPONSE;
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::VehicleDisconnect(void)
{
	Log(LOG_FN_ENTRY, "MazdaMachineTC::VehicleDisconnect() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{	// Prompt and wait for ignition off
		DisplayPrompt(GetPromptBox("TurnOffIgnition"), GetPrompt("TurnOffIgnition"), GetPromptPriority("TurnOffIgnition"));
		// Wait for the engine to be off
		bool engineRunning = true;
		do
		{	// Wait before the next check
			BposSleep(GetTestStepInfoInt("ScanDelay"));
			engineRunning = SystemReadBool(GetDataTag("EngineRunningTag"));
		} while(engineRunning && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
		RemovePrompt(GetPromptBox("TurnOffIgnition"), GetPrompt("TurnOffIgnition"), GetPromptPriority("TurnOffIgnition"));
		DisplayPrompt(GetPromptBox("DisconnectCable"), GetPrompt("DisconnectCable"), GetPromptPriority("DisconnectCable"));
		// Check the exit condition
		if(!engineRunning)
		{	// Wait for cable disconnect
			while(IsCableConnected() && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
			{	// Wait before the next check
				BposSleep(GetTestStepInfoInt("ScanDelay"));
			}
			result = !IsCableConnected() ? testPass : testFail;
		}
		else
		{
			Log(LOG_ERRORS, "Timeout waiting for the engine to be off");
			result = testFail;
		}
		RemovePrompt(GetPromptBox("DisconnectCable"), GetPrompt("DisconnectCable"), GetPromptPriority("DisconnectCable"));
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping vehicle disconnect sequence");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "MazdaMachineTC::VehicleDisconnect() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::VehicleSetup(void)
{
	Log(LOG_FN_ENTRY, "MazdaMachineTC::VehicleSetup() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{	// Prompt the operator to turn off the ignition and connect the diagnostic cable
		DisplayPrompt(GetPromptBox("TurnOffIgnition"), GetPrompt("TurnOffIgnition"), GetPromptPriority("TurnOffIgnition"));
		DisplayPrompt(GetPromptBox("ConnectCable"), GetPrompt("ConnectCable"), GetPromptPriority("ConnectCable"));
		bool cableConnected = CheckCableConnect();
		RemovePrompt(GetPromptBox("TurnOffIgnition"), GetPrompt("TurnOffIgnition"), GetPromptPriority("TurnOffIgnition"));
		RemovePrompt(GetPromptBox("ConnectCable"), GetPrompt("ConnectCable"), GetPromptPriority("ConnectCable"));
		// Wait for cable connect
		if(cableConnected)
		{	// Cable is connected, prompt for foot on brake and start the engine
			DisplayPrompt(GetPromptBox("ApplyBrakePedal"), GetPrompt("ApplyBrakePedal"), GetPromptPriority("ApplyBrakePedal"));
			DisplayPrompt(GetPromptBox("StartEngine"), GetPrompt("StartEngine"), GetPromptPriority("StartEngine"));
			// Wait for the engine to be running
			bool engineRunning = false;
			do
			{	// Wait before the next check
				BposSleep(GetTestStepInfoInt("ScanDelay"));
				engineRunning = SystemReadBool(GetDataTag("EngineRunningTag"));
			} while(!engineRunning && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
			// Check the state of the engine running
			result = engineRunning ? testPass : testAbort;
			if(!engineRunning)
			{
				Log(LOG_DEV_DATA, "Engine not running, abort the test sequence");
				SystemWrite(ABORT_DATA_TAG, true);
			}
			RemovePrompt(GetPromptBox("ApplyBrakePedal"), GetPrompt("ApplyBrakePedal"), GetPromptPriority("ApplyBrakePedal"));
			RemovePrompt(GetPromptBox("StartEngine"), GetPrompt("StartEngine"), GetPromptPriority("StartEngine"));
		}
		else
		{
			Log(LOG_DEV_DATA, "Cable not connected, abort the test sequence");
			result = testAbort;
			SystemWrite(ABORT_DATA_TAG, true);
		}
		// Report the result
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping Vehicle Setup");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "MazdaMachineTC::VehicleSetup() - Exit");
	return result;
}


//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepReverse(const string &value)
{	// Log the entry and prompt the operator to accelerate in reverse
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepReverse(value: %s) - Enter", value.c_str());
	string testResult(BEP_TESTING_RESPONSE);
	bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
	float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
	float reverseSpeed = GetVehicleParameter("ReverseTestSpeed", float(0.0)) * speedScaling;
	// Wait for the operator to reach zeroish speed
	while((GetRollSpeed() > (GetParameterFloat("PromptReverseTestSpeed") * speedScaling)) &&
		  (BEP_STATUS_SUCCESS == StatusCheck()))
	{	// Wait just a bit so we do not hog the CPU - also reset the start time since there is no way to tell
		// when the operator will stop
		BposSleep(1000);
		SetStartTime();
	}
	// Display the target for the operator
	char buff[32];
	string reverseTarget(CreateMessage(buff, sizeof(buff), "%.2f", reverseSpeed));
	// Prompt the operator to accelerate to the target
	DisplayPrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
	DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"),
				  string("white"), reverseTarget);
	// Wait for the target speed to be reached
	while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (GetRollSpeed() > reverseSpeed)) BposSleep(250);
	testResult = (GetRollSpeed() < reverseSpeed) ? testPass : testFail;
	SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	// Remove the prompts
	RemovePrompt(GetPromptBox("ShiftToReverse"), GetPrompt("ShiftToReverse"), GetPromptPriority("ShiftToReverse"));
	RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepReverse(value: %s) - Exit", value.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaMachineTC::WaitToStart(void)
{	// Log the entry and wait for the loss of zero speed
	Log(LOG_FN_ENTRY, "MazdaMachineTC::WaitToStart() - Enter");
	while((GetRollSpeed() < GetParameterFloat("VehicleSpeedNeededForStart")) && (BEP_STATUS_SUCCESS == StatusCheck()))
	{	// Wait for a bit
		BposSleep(1000);
		SetStartTime();
	}
	// Log the exit and return pass
	Log(LOG_FN_ENTRY, "MazdaMachineTC::WaitToStart() - Exit");
	return testPass;
}

//-------------------------------------------------------------------------------------------------
inline float MazdaMachineTC::GetRollSpeed(void)
{
	return GetParameterBool("SingleEncoder") ? SystemReadFloat(GetDataTag("MaximumRollerSpeedTag")) : GenericTC::GetRollSpeed();
}

//-------------------------------------------------------------------------------------------------
const string MazdaMachineTC::TestStepAccelerateToSpeed(const string &value)
{	// Log the entry and prompt the operator
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Enter", value.c_str());
	bool convertSpeed = SystemReadBool(GetDataTag("SpeedDisplayScaling"));
	float speedScaling = convertSpeed ? GetParameterFloat("SpeedConversionFactor") : 1.0;
	char buff[32];
	string dispSpeed(CreateMessage(buff, sizeof(buff), "%.0f", atof(value.c_str()) * speedScaling));
	DisplayPrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"), 
				  string("white"), dispSpeed);
	// Wait for the operator to achieve speed
	float speedTarget = atof(value.c_str()) * speedScaling;
	while((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && (GetRollSpeed() < speedTarget))  BposSleep(250);
	// Make sure speed was schieved and return the result
	Log(LOG_FN_ENTRY, "MazdaMachineTC::TestStepAccelerateToSpeed(speed: %s) - Exit", value.c_str());
	RemovePrompt(GetPromptBox("AccelAboveSpeed"), GetPrompt("AccelAboveSpeed"), GetPromptPriority("AccelAboveSpeed"));
	return(GetRollSpeed() >= speedTarget) ? testPass : testFail;
}


const string MazdaMachineTC::TestStepRainLightSensorVerification(void)
{

	Log(LOG_FN_ENTRY, "MazdaMachineTC::StartRainLightSensorTest() - Enter");

	string testResult(BEP_SKIP_RESPONSE);
	INT32 status;
	CheckForValidRLSType();
	if(IsRLSEquipped())
	{
		testResult = GenericTC::OperatorPassFail(GetPrompt("StartRLSTest"), GetParameterInt("StartRLSTestPromptTimeout")); 
		if(testResult == BEP_PASS_RESPONSE)
		{
			//command plc to turn on light and spray devices

			SystemWrite(GetDataTag("EnableLightAndSprayDevice"), true);
			status = StatusSleep( GetParameterInt("LightAndSprayDeviceActuationTime"));
			SystemWrite(GetDataTag("EnableLightAndSprayDevice"), false);

			//Operater pass / fail 
			testResult = status == BEP_STATUS_SUCCESS ? GenericTC::OperatorPassFail(GetPrompt("VerifyRLSOperation"), 
																					GetParameterInt("VerifyRLSOperationPromptTimeout")) : 
						 testFail;
		}
		else
		{//test skipped by operator

			string testResult = BEP_SKIP_RESPONSE;
		}
		SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
	}

	Log(LOG_FN_ENTRY, "MazdaMachineTC::StartRainLightSensorTest() - Exit - %s", testResult.c_str());

	return testResult;
}

//-------------------------------------------------------------------------------------------------
inline const bool& MazdaMachineTC::IsRLSEquipped(void)
{
	return m_rlsEquipped;
}

//-------------------------------------------------------------------------------------------------
inline void MazdaMachineTC::SetRLSEquipped(const bool &equipped)
{
	Log(LOG_DEV_DATA, "RLS Equipped: %s", equipped ? "True" : "False");
	m_rlsEquipped = equipped;
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineTC::SetupTimer(const XmlNode *timerSetupNode, BepTimer &timer, INT32 pulseValue)
{
	SetupTimer(atol(timerSetupNode->getValue().c_str()), timer, pulseValue);
}

//-------------------------------------------------------------------------------------------------
void MazdaMachineTC::SetupTimer(UINT64 updateRate, BepTimer &timer, INT32 pulseValue)
{
	Log(LOG_DEV_DATA, "Setting up timer with update rate of %dms", updateRate);
	timer.SetPulseCode(MAZDA_MACHINE_TC_PULSE_CODE);
	timer.SetPulseValue(pulseValue);
	timer.Initialize(GetProcessName(), mSEC_nSEC(updateRate), mSEC_nSEC(updateRate));
	timer.Stop();
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE MazdaMachineTC::UpdateDistanceTraveled(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	WHEELINFO currentPulseCounts, currentDistance;
	status = ConvertIntToBepStatus(GetWheelDistances(currentPulseCounts));
	if(BEP_STATUS_SUCCESS == status)
	{   // Calculate the current distance traveled
		GetTotalDistances(currentDistance, m_odoStartDistance, currentPulseCounts);
		float driveWheelDist = !SystemRead(DRIVE_AXLE_TAG).compare(FRONT_WHEEL_DRIVE_VALUE) ? currentDistance.lfWheel : currentDistance.lrWheel;
		float odometer = ConvertPulsesToMiles(driveWheelDist);
		SystemWrite("CurrentOdometer", odometer);
		Log(LOG_DEV_DATA, "Setting current odometer value to: %.4f miles", odometer);
	}
	else
	{
		Log(LOG_ERRORS, "Error getting current pulse counts");
	}
	return status;
}
