//*************************************************************************
// Description:
//   Test component for testing Tachograph systems.
//
//===========================================================================
// Copyright (c) 2007- Burke E. Porter Machinery
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
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#include "TachographTC.h"
#include "KeywordProtocolFilter.h"
#include "InsModule.cpp"
//------------------------------------------------------------------------------

TachographTC::TachographTC() : GenericTC(), m_tachCommConfig(NULL), m_tachographComm(NULL)
{	// Nothing special to be done here
}

TachographTC::~TachographTC()
{	// Clear the switch settings
	m_calSwitchSettings.clear();
	// Clear the tachogrpah types
	m_switchTypeTacs.clear(true);
	m_insTypeTacs.clear(true);
	// Delete tachograph comm unit
	if (m_tachographComm != NULL) delete m_tachographComm;
	m_tachographComm = NULL;
	// Delete the comm config
	if (m_tachCommConfig != NULL)  delete m_tachCommConfig;
	m_tachCommConfig = NULL;
}

void TachographTC::Initialize(const XmlNode *config)
{	// Call the base class to complete the 
	GenericTC::Initialize(config);
	// Load the switch settings
	try
	{
		LoadLookupTable(config->getChild("Setup/Parameters/SwitchSettingsFile")->getValue());
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not load switch settings from file - %s", excpt.GetReason());
		m_calSwitchSettings.clear();
	}
	// Load the tachograph types that require switch manipulation
	try
	{
		m_switchTypeTacs.DeepCopy(config->getChild("Setup/Parameters/SwitchTypeTachographs")->getChildren());
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not load tachograph types - %s", excpt.GetReason());
		m_switchTypeTacs.clear(true);
	}
	// Load the tachograph types that require INS communication
	try
	{
		m_insTypeTacs.DeepCopy(config->getChild("Setup/Parameters/InsTypeTacographs")->getChildren());
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not load INS tachograph types - %s", excpt.GetReason());
		m_insTypeTacs.clear(true);
	}
	// Load the tachograph comm config
	try
	{
		m_tachCommConfig = config->getChild("Setup/TachographCommConfig")->Copy();
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find comm config - %s", excpt.GetReason());
		m_tachCommConfig = NULL;
	}
	try
	{
		m_tachographTypes00.DeepCopy(config->getChild("Setup/Parameters/TachographPlcControl00")->getChildren());
		m_tachographTypes01.DeepCopy(config->getChild("Setup/Parameters/TachographPlcControl01")->getChildren());
		m_tachographTypes10.DeepCopy(config->getChild("Setup/Parameters/TachographPlcControl10")->getChildren());
		m_tachographTypes11.DeepCopy(config->getChild("Setup/Parameters/TachographPlcControl11")->getChildren());
	}
	catch (XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not load TachographPlcControlxx types - %s", excpt.GetReason());
		m_tachographTypes00.clear(true);
		m_tachographTypes01.clear(true);
		m_tachographTypes10.clear(true);
		m_tachographTypes11.clear(true);
	}
	// signal that initialization is complete
	Log(LOG_FN_ENTRY, "Done Initializing TachographTC\n");
	InitializationComplete();
}

const string TachographTC::CommandTestStep(const string &value)
{
	string testResult = testFail;
	// Determine if the next test step can be sequenced
	if (StatusCheck() == BEP_STATUS_SUCCESS)
	{	// System is fine to continue testing
		try
		{	// Get the next test step to perform
			Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
			// Perform the tachograph test
			if (GetTestStepName() == "TachographTest")	testResult = TachographDistance();
			// Adjust the mux in the "Black Box" accordingly...
			else if (GetTestStepName() == "BlackBoxCircuitSelect")	testResult = BlackBoxCircuitSelect();
			// Utility for testing tachograph interface
			else if (GetTestStepName() == "TestTachographInterface")  testResult = TestInterface(value);
			// No special method available, try the base class// Finish up any needed testing
			else if (GetTestStepName() == "FinishUp")
			{
				testResult = FinishUp();
				SendOverallResult(testResult);
			}
			// Call the base class to handle the test step
			else   testResult = GenericTC::CommandTestStep(value);
		}
		catch (BepException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
			testResult = BEP_ERROR_RESPONSE;
		}
	}
	else
	{	// Status of the system is not right to continue testing
		Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
	}
	// Return the result of the test
	Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
	return testResult;
}

string TachographTC::TachographDistance(void)
{
	string testResult(BEP_TESTING_STATUS);
	// Log the entry and determine if this test should be performed
	Log(LOG_FN_ENTRY, "TachographTC::TachographDistance() - Enter");
	if (!ShortCircuitTestStep())
	{
		string testResultCode("0000");
		string testDescription(GetTestStepInfo("Description"));
		// Prompt the operator to speed
		if (testPass == AccelerateToTestSpeed(GetParameterFloat("TachographTestSpeed"), 
											  GetParameter("TachographSpeedRange"),
											  GetTestStepInfoInt("ScanDelay"), false))
		{
			bool abortTest = false;
			//float initRollDist = 0.0;
			long int initTachoCount = 0;
			// Restart the test step timer
			SetStartTime();
			DisplayPrompt(GetPromptBox("SteadySpeed"),GetPrompt("SteadySpeed"),GetPromptPriority("SteadySpeed"));
			DisplayPrompt(GetPromptBox("TestingTachograph"),GetPrompt("TestingTachograph"),GetPromptPriority("TestingTachograph"));
			//SystemWrite(GtDataTag("SpeedTarget"), GetParameter("TachographSpeedRange"));
			// Get initial distance/counts
			try
			{	// Reset the tachograph count if required
				if (GetParameterBool("ResetTachographCount"))
				{
					SystemWrite(GetDataTag("ResetTachCountTag"), 1);
					BposSleep(GetParameterInt("ResetTachCountPulseLength"));
					SystemWrite(GetDataTag("ResetTachCountTag"), 0);
				}
				// Get the initial distances - Rear wheel, since it is the drive axle
				// read the current distance of the wheels
				GetWheelDistances(m_initialWheelDistances);
				//initRollDist = SystemReadFloat(GetDataTag("RollerDistanceTag"));
				Log(LOG_DEV_DATA, "Tachograph - Initial LR Distance = %f", m_initialWheelDistances.lrWheel);
				initTachoCount = ReadTachographPulses();
			}
			catch (BepException &excpt)
			{
				Log(LOG_ERRORS, "Could not read initial roller distances - aborting: %s", excpt.GetReason());
				testResult = testHardwareFail;
				abortTest = true;
				testResultCode = GetFaultCode("ReadDistanceFailure");
				testDescription = GetFaultDescription("ReadDistanceFailure");
			}
			// Check if we can continue
			if (!abortTest)
			{	// Wait for the distance to be achieved
				float distanceTraveled = 0.0;
				float distanceTraveledInMiles = 0.0;
				do
				{	// Get the current distance traveled
					BposSleep(GetTestStepInfoInt("ScanDelay"));
					// read the current distance of the wheels
					GetWheelDistances(m_finalWheelDistances);
					distanceTraveled = m_finalWheelDistances.lrWheel - m_initialWheelDistances.lrWheel;
					Log(LOG_DEV_DATA, "Tachograph - LR Distance Traveled #1 = %f pulses", distanceTraveled);
					distanceTraveledInMiles = ((((distanceTraveled * 111.9192) / 12) / 5280) / 1250);
					Log(LOG_DEV_DATA, "Tachograph - LR Distance Traveled #1 = %f miles", distanceTraveledInMiles);
					// MATH:
					// 111.9192" = (35.625"/2)*2*PI - Roller circumference - Inches per revolution
					// 12 = twelve inches per foot
					// 5280 = 5280 feet per mile
					// 1250 = 1250 pulses counted per revolution
					// Keep checking while:
					//        - System status is good
					//        - Test has not timed out
					//        - Required distance not achieved
				} while ( (BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && 
						  (distanceTraveledInMiles < GetParameterFloat("MinTachographDistance")) );
				// Get the final tachogrpah pulse cout and wheel distance
				long int tachoCount;
				long int finalTachoCount = ReadTachographPulses();
				if (finalTachoCount > initTachoCount)
				{
					//Everything is normal
					tachoCount = finalTachoCount - initTachoCount;
					Log(LOG_DEV_DATA, "Tachograph - Read: Normal");
					Log(LOG_DEV_DATA, "init=%d, final=%d, Count=%d\n", 
						initTachoCount, finalTachoCount, tachoCount);
				}
				else
				{
					//We had a PLC rollover situation.
					//Add 2^24 to the final number, and we shall be good.
					tachoCount = (finalTachoCount + 16777216) - initTachoCount;
					Log(LOG_DEV_DATA, "Tachograph - Read: Rollover");
					Log(LOG_DEV_DATA, "init=%d, final=%d, Count=%d\n", 
						initTachoCount, finalTachoCount, tachoCount);
				}
				distanceTraveled = m_finalWheelDistances.lrWheel - m_initialWheelDistances.lrWheel;
				Log(LOG_DEV_DATA, "Tachograph - LR Distance Traveled #2 = %f", distanceTraveled);
				distanceTraveledInMiles = ((((distanceTraveled * 111.9192) / 12) / 5280) / 1250);
				Log(LOG_DEV_DATA, "Tachograph - LR Distance Traveled #2 = %f miles", distanceTraveledInMiles);
				// MATH:
				// 111.9192" = (35.625"/2)*2*PI - Roller circumference - Inches per revolution
				// 12 = twelve inches per foot
				// 5280 = 5280 feet per mile
				// 1250 = 1250 pulses counted per revolution
				// Check the exit conditions

				RemovePrompt(GetPromptBox("SteadySpeed"),GetPrompt("SteadySpeed"),GetPromptPriority("SteadySpeed"));
				RemovePrompt(GetPromptBox("TestingTachograph"),GetPrompt("TestingTachograph"),GetPromptPriority("TestingTachograph"));
				//SystemWrite(GetDataTag("SpeedTarget"), "0 0");
				if (distanceTraveledInMiles >= GetParameterFloat("MinTachographDistance"))
				{	// Enough distance covered, calculate constant -- #pulses/distance(km)
					float distKm = distanceTraveledInMiles * 1.609344;	  // 1.609344km per mile 
					float calConst = tachoCount / distKm;
					Log(LOG_DEV_DATA, "Calculated cal constant: W = %d / %6.2f = %7.2f", tachoCount, distKm, calConst);
					if ((calConst < GetParameterInt("MinCalConst")) || (calConst > GetParameterInt("MaxCalConst")))
					{
						Log(LOG_DEV_DATA, "Const=%f, Min=%d, Max=%d", tachoCount, 
							GetParameterInt("MinCalConst"), GetParameterInt("MaxCalConst"));
						testResult = testFail;
						testResultCode = GetFaultCode("TachographConstOutOfRange");
						testDescription = GetFaultDescription("TachographConstOutOfRange");
					}
					else
					{
						// Write the cal constant to the tachograph unit
						testResult = UpdateCalConstant(calConst);
						// Report the detailed result
						char buff[16];
						SendSubtestResultWithDetail("TachoDetail", testResult, testDescription, testResultCode,
													"PulseCount", CreateMessage(buff, sizeof(buff), "%d", tachoCount), "",
													"MilesTraveled", CreateMessage(buff, sizeof(buff), "%7.2f", distanceTraveled), "miles",
													"KmTraveled", CreateMessage(buff, sizeof(buff), "%7.2f", distKm), "Km",
													"W", CreateMessage(buff, sizeof(buff), "%7.2f", calConst), "");
					}
				}
				else if (!TimeRemaining())
				{	// Timeout waiting for distance
					testResult = testTimeout;
					testResultCode = GetFaultCode("TimeoutWaitingForDistance");
					testDescription = GetFaultDescription("TimeoutWaitingForDistance");
					Log(LOG_ERRORS, "Timeout waiting for required distance: %5.2f [min: %s]", 
						distanceTraveled, GetParameter("MinTachographDistance").c_str());
				}
				else
				{	// Bad system status
					testResult = testFail;
					testResultCode = GetFaultCode("SystemStatus");
					testDescription = GetFaultDescription("SystemStatus");
					Log(LOG_ERRORS, "Bad system status waiting for distance - %s", ConvertStatusToResponse(StatusCheck()).c_str());
				}
			}
		}
		else
		{	// Timeout waiting for speed to be in range
			Log(LOG_ERRORS, "Timeout waiting for speed to be in range");
			testResult = testFail;
			testResultCode = GetFaultCode("VehicleSpeedTimeout");
			testDescription = GetFaultDescription("VehicleSpeedTimeout");
		}
		// Report the results
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping test step - TachographTC::TachographDistance()");
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "TachographTC::TachographDistance() - Exit");
	return testResult;
}

string TachographTC::UpdateCalConstant(const float &calConst)
{
	string updateResult(BEP_TESTING_STATUS);
	int roundedCalConst = (int)(calConst + 0.5);
	Log(LOG_DEV_DATA, "Rounded cal const from %7.2f to %d for range lookup", calConst, roundedCalConst);
	if (LookupSwitchSettings())
	{	// Use the lookup table to determine the switch settings
		string switchSettings = FindSwitchSettings(calConst);
		// Display the switch settings for the operator and wait for them to continue
		DisplayPrompt(GetPromptBox("SwitchSettings"), GetPrompt("SwitchSettings"), GetPromptPriority("SwitchSettings"),
					  "white", switchSettings);
		BposSleep(GetParameterInt("SwitchSettingPromptDelay"));
		RemovePrompt(GetPromptBox("SwitchSettings"), GetPrompt("SwitchSettings"), GetPromptPriority("SwitchSettings"));
		updateResult = testPass;
		SendSubtestResultWithDetail("SwitchSettings", testPass, GetTestStepInfo("Description"), "0000",
									"SwitchSettings", switchSettings, "");
	}
	else
	{
		UINT16 kConst = UINT16(calConst + 0.5);	 // Round the cal constant
		char kString[10];
		sprintf(kString, "%d", (int)kConst);
		string kConstString (kString);
		DisplayPrompt(GetPromptBox("WritingCalData"),GetPrompt("WritingCalData"),GetPromptPriority("WritingCalData"),
					  "white", kConstString.c_str());
		DisplayPrompt(GetPromptBox("ZeroSpeed"),GetPrompt("ZeroSpeed"),GetPromptPriority("ZeroSpeed"));
		if (WriteCalConstantToInsModule())
		{	// Create a new INs module Interface
			Log(LOG_DEV_DATA, "We must write to the INS module directly...");
			string tacType = SystemRead(GetDataTag("TachographType"));
			InsModule<KeywordProtocolFilter> insModule;
			XmlParser parser;
			insModule.Initialize(parser.ReturnXMLDocument(getenv("USR_ROOT") + GetParameter("ConfigFile")));
			// Write the cal constant to the module
			Log(LOG_DEV_DATA, "kConst = %d", kConst);
			updateResult = (insModule.UpdateKFactor(kConst, tacType) == BEP_STATUS_SUCCESS) ? testPass : testFail;
		}
		else
		{	// Write the constant directly to the unit
			if ((m_tachographComm == NULL) && (m_tachCommConfig != NULL))
			{	// Create a new tacho graph comm object and initialize it
				m_tachographComm = new SerialChannel();
				m_tachographComm->Initialize(m_tachCommConfig);
			}
			if (m_tachographComm != NULL)
			{	// Build and send the message
				char buff[64];
				BEP_STATUS_TYPE status;
				string message = CreateMessage(buff, sizeof(buff), "%s;%s;%d", 
											   GetDataTag("CalConstantMsgTag").c_str(), 
											   ReadSubscribeData(VIN_DATA_TAG).c_str(), roundedCalConst);
				Log(LOG_DEV_DATA, "Sent string: %s", message.c_str());
				message += (char(0x00) + "\n");
				m_tachographComm->ResetConnection();
				status = m_tachographComm->Send(message);
				if (BEP_STATUS_SUCCESS == status)
				{	// Get the response
					string rspns;
					BposSleep(GetParameterInt("TachographUpdateDelay"));
					int rxLength;
					bool responseFound = false;

					do
					{	// Keep looking for the correct response
						rxLength = m_tachographComm->Get(rspns);
						Log(LOG_DEV_DATA, "Evaluating Response: %s", rspns.c_str());
						responseFound = rspns.find(GetDataTag("CalConstantMsgTag")) < rspns.length();
					} while ((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !responseFound);

					if (rxLength > 0)
					{	// Check for a success response
						Log(LOG_DEV_DATA, "Response: %s", rspns.c_str());
						updateResult = (rspns.find(GetDataTag("TachUpdateSuccessValue")) < rspns.length()) ? testPass : testFail;
					}
					else
					{	// No response from update message
						Log(LOG_ERRORS, "No response from cal constant update message - (bytes rx: %d)", rxLength);
						updateResult = testFail;
					}
				}
				else
				{	// Could not write cal constant to the module
					Log(LOG_ERRORS, "Could not write cal constant to tachograph - %s", ConvertStatusToResponse(status).c_str());
					updateResult = ConvertStatusToResponse(status);
				}
			}
			else
			{
				Log(LOG_ERRORS, "Could not establish comms with tachograph update process!");
				updateResult = testFail;
			}
		}

		RemovePrompt(GetPromptBox("WritingCalData"),GetPrompt("WritingCalData"),GetPromptPriority("WritingCalData"));
		RemovePrompt(GetPromptBox("ZeroSpeed"),GetPrompt("ZeroSpeed"),GetPromptPriority("ZeroSpeed"));
	}
	Log(LOG_DEV_DATA, "Cal constant update: %s", updateResult.c_str());
	return updateResult;
}

bool TachographTC::LookupSwitchSettings(void)
{	// Read the tachograph type from the system
	string tacType = SystemRead(GetDataTag("TachographType"));
	bool switchType = false;
	// Check if it is a switch type
	for (XmlNodeMapItr iter = m_switchTypeTacs.begin(); (iter != m_switchTypeTacs.end()) && !switchType; iter++)
	{
		switchType = !tacType.compare(iter->second->getValue());
	}
	Log(LOG_DEV_DATA, "Tac Type: %s - Switch type: %s", tacType.c_str(), switchType ? "True" : "False");
	return switchType;
}

bool TachographTC::WriteCalConstantToInsModule(void)
{	// Read the tachograph type from the system
	string tacType = SystemRead(GetDataTag("TachographType"));
	bool insType = false;
	// Check if it is a switch type
	for (XmlNodeMapItr iter = m_insTypeTacs.begin(); (iter != m_insTypeTacs.end()) && !insType; iter++)
	{
		insType = !tacType.compare(iter->second->getValue());
	}
	Log(LOG_DEV_DATA, "Tac Type: %s - INS type: %s", tacType.c_str(), insType ? "True" : "False");
	return insType;
}

string TachographTC::FindSwitchSettings(const int &calIndex)
{
	string switchSettings("Oops!");
	SwitchSettingLookupTableItr iter;
	// Find lower bound of search area
	iter = m_calSwitchSettings.lower_bound(calIndex);
	iter--;	  // Lower bound found the element 1 passed what we really want.
	// Ensure we found the correct settings
	if ((iter->second.minW <= calIndex) && (calIndex <= iter->second.maxW))
	{	// Found the value
		switchSettings = iter->second.switchSettings;
	}
	return switchSettings;
}

void TachographTC::LoadLookupTable(const string &dataFile)
{	// Add the path to the specified file
	string fileName(getenv("USR_ROOT") + dataFile);
	// Attempt to open the specified data file
	FILE *file;
	if ((file = fopen(fileName.c_str(), "r")) != NULL)
	{
		char line[32];
		while (fgets(line, sizeof(line), file) != NULL)
		{	// Process the current line if valid.  All valid lines begin with a number
			if (isdigit(line[0]))
			{
				int minVal = atoi(strtok(line, " "));
				int maxVal = atoi(strtok(NULL, " "));
				string switchSettings(strtok(NULL, " "));

				SwitchSettings settings;
				settings.minW = minVal;
				settings.maxW = maxVal;
				settings.switchSettings = switchSettings;

				m_calSwitchSettings.insert(SwitchSettingLookupTableVal(minVal, settings));
			}
		}
		fclose(file);
	}
	else
	{	// Could not open data file
		Log(LOG_ERRORS, "Could not open data file! - %s", strerror(errno));
		m_calSwitchSettings.clear();
	}
}

string TachographTC::TestInterface(const string &calConst)
{
	float fCalConst = atof(calConst.c_str());
	Log(LOG_DEV_DATA, "Testing DLL interface using cal constant: %f", fCalConst);
	string result = UpdateCalConstant(fCalConst);
	Log(LOG_DEV_DATA, "Updated cal constant: %s", result.c_str());
	return result;
}

int TachographTC::ReadTachographPulses(void)
{
	int tachHi = SystemReadInt(GetDataTag("TachographCountHiTag"));
	Log(LOG_DEV_DATA, "TachographCountHiTag = %d", tachHi);
	int tachLo = SystemReadInt(GetDataTag("TachographCountLoTag"));
	Log(LOG_DEV_DATA, "TachographCountLoTag = %d", tachLo);
	return((tachHi << 16) + tachLo);
}

inline string TachographTC::FinishUp(void)
{
	return(testPass);
}

string TachographTC::BlackBoxCircuitSelect(void)
{
	Log(LOG_DEV_DATA, "TachographTC::BlackBoxCircuitSelect() - Enter");
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	string testResult(BEP_TESTING_STATUS);
	XmlNodeMap tachographTypes00, tachographTypes01, tachographTypes10, tachographTypes11;
	bool scanStatus = false;
	scanStatus = BlackBoxCircuitSelectScan(m_tachographTypes00, 0, 0);
	if (!scanStatus)
		scanStatus = BlackBoxCircuitSelectScan(m_tachographTypes01, 0, 1);
	if (!scanStatus)
		scanStatus = BlackBoxCircuitSelectScan(m_tachographTypes10, 1, 0);
	if (!scanStatus)
		scanStatus = BlackBoxCircuitSelectScan(m_tachographTypes11, 1, 1);
	if (!scanStatus)
	{
		testResult = testFail;
		Log(LOG_DEV_DATA, "No matches found for TachographType!");
		testResultCode = GetFaultCode("TachographType");
		testDescription = GetFaultDescription("TachographType");
	}
	else
	{
		testResult = testPass;
	}
	// Report the results
	SendTestResult(testResult, testDescription, testResultCode);
	Log(LOG_DEV_DATA, "TachographTC::BlackBoxCircuitSelect() - Exit");
	return testResult;
}

bool TachographTC::BlackBoxCircuitSelectScan(XmlNodeMap tachographTypesXX, int ctrlHi, int ctrlLo)
{
	bool scanStatus = false;
	string tacType = SystemRead(GetDataTag("TachographType"));
	for (XmlNodeMapItr iter = tachographTypesXX.begin(); (iter != tachographTypesXX.end()); iter++)
	{
		Log(LOG_DEV_DATA, "PLC%d%d - Broadcast: %s - ListedType: %s", 
			ctrlHi, ctrlLo, tacType.c_str(), iter->second->getValue().c_str());
		if (!tacType.compare(iter->second->getValue()))
		{
			Log(LOG_DEV_DATA, "PLC%d%d - A match!", ctrlHi, ctrlLo);
			SystemWrite(GetDataTag("TachographCircuitSelectLo"), ctrlLo);
			SystemWrite(GetDataTag("TachographCircuitSelectHi"), ctrlHi);
			scanStatus = true;
		}
	}
	return scanStatus;
}

void TachographTC::Abort(void)
{
	GenericTC::Abort();
	Log(LOG_DEV_DATA, "Tachograph test aborting!\n");
}
