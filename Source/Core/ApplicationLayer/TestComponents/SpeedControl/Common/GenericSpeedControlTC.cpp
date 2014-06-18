//******************************************************************************
// Description:
//  Base Cruise testing for all types of vehicles.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/SpeedControl/Common/GenericSpeedControlTC.cpp $
// 
// 10    10/30/07 1:58p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed some typos in the comments.
// 
// 9     5/01/07 11:22a Rwiersem
// Changes for the 07050201 core release:
// 
// Moved the call to
// GenericTCTemplate<VehicleModuleType>::WarmInitialize() to after
// checking for valid speed control types.
// 
// 8     10/25/06 4:03p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added WarmInitialize().  This method is used to keep the test
// component running when a new drive curve starts.
// 
// 7     4/13/06 3:27a Cward
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 2     3/20/06 10:48a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
//
// 6     2/15/06 7:24p Cward
// Added Tap Up Tap Down steps, ability to test speed control without
// module comms.
//
// 5     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 5     4/04/05 1:02a Mkelly
// The WaitForResumeButton() method used the incorrect parameter "SpeedControlSetMask",
// switched to use the "SpeedControlResumeMask" instead
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 2     9/01/04 9:17p Cward
// Added the capability to used a skewed range for determining stable
// speed. Added delays between speed checks.
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericSpeedControlTC.h"
#include "ModuleException.h"
#include <math.h>

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericSpeedControlTC<VehicleModuleType>::GenericSpeedControlTC() :
GenericTCTemplate<VehicleModuleType>(), m_isETCVehicle(false), m_previousPass(false), m_speedControlEquipped(true)
{	// Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericSpeedControlTC<VehicleModuleType>::~GenericSpeedControlTC()
{	// Nothing special to do here
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CommandTestStep(const string &value)
{
	string testResult = testFail;
	// Make sure system status is good before sequencing the next test step
	if(StatusCheck() == BEP_STATUS_SUCCESS)
	{	// Good system status, sequence the next test step
		if(IsCruiseEquipped())
		{
			try
			{	// Get the next test step to perform
				Log("Running test step %s\n", GetTestStepName().c_str());
				// Check the ON functionality
				if(GetTestStepName() == "CheckOnEnable")			  testResult = CheckOnEnable();
				// Check the OFF functionality
				else if(GetTestStepName() == "CheckOffDisable")		  testResult = CheckOffDisable();
				else if(!GetTestStepName().compare("CheckSpeedControlEquipped"))
				{	// Check if vehicle is equipped with the Speed Control Component
					//      and set member variable
					CheckForValidSpeedControlType();
					if(IsCruiseEquipped())
					{
						UpdateTestStatus(TEST_STATUS_IN_PROGRESS);
					}
					else
					{
						UpdateTestStatus(BEP_TEST_NOT_STARTED);
					}
					testResult = testPass;
				}
				// Check Brake cut out
				else if(GetTestStepName() == "CheckBrakeCutOut")	  testResult = CheckBrakeCutOut();
				// Check Cancel cut out
				else if(GetTestStepName() == "CheckCancelCutOut")	  testResult = CheckCancelCutOut();
				// Check Clutch cut out
				else if(GetTestStepName() == "CheckClutchCutOut")	  testResult = CheckClutchCutOut();
				// Check for Resume engagement
				else if(GetTestStepName() == "CheckResumeEngagement") testResult = CheckResumeEngagement();
				// Check for Set engagement
				else if(GetTestStepName() == "CheckSetEngagement")	  testResult = CheckSetEngagement();
				// Check tap up feature
				else if(GetTestStepName() == "CheckTapUpAccel")		  testResult = CheckTapSpeedChange("Up");
				// Check tap down feature
				else if(GetTestStepName() == "CheckTapDownDecel")	  testResult = CheckTapSpeedChange("Down");
				// Check the functionality of the spee limiter
				else if(GetTestStepName() == "CheckSpeedLimiter")	  testResult = CheckSpeedLimiter();
				else if(!GetTestStepName().compare("CheckKickDown"))   testResult = CheckKickDown();
				// Determine if the vehicle is previous pass
				else if(GetTestStepName() == "DeterminePreviousPass") testResult = DeterminePreviousPass();
				// Update the test result in the module
				else if(GetTestStepName() == "UpdateModuleResult")	  testResult = UpdateModuleResult();
				// Get the operator result
				else if(GetTestStepName() == "OperatorResult")		   testResult = SpeedControlOperatorResult();
				else if(!GetTestStepName().compare("VerifyStableSpeed"))  testResult = VerifySteadySpeed();
				// Unknown test step requested - try the base class
				else   testResult = GenericTCTemplate<VehicleModuleType>::CommandTestStep(value);
			}
			catch(BepException &err)
			{	// Log the exception and return the error
				Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
				testResult = BEP_ERROR_RESPONSE;
			}
		}
		else
		{
			Log(LOG_DEV_DATA, "Vehicle not equipped with speed control, skipping test step: %s", GetTestStepName().c_str());
			testResult = testSkip;
		}
	}
	else
	{	// Status of the system is not right to continue testing
		Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
		testResult = testSkip;
	}
	// Return the result of the test
	Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
	return testResult;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const float &GenericSpeedControlTC<VehicleModuleType>::GetSpeedControlTargetSpeed(void)
{
	return m_targetSpeed;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool &GenericSpeedControlTC<VehicleModuleType>::IsETCVehicle(void)
{
	return m_isETCVehicle;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericSpeedControlTC<VehicleModuleType>::Abort(void)
{
	if(IsCruiseEquipped())
	{
		GenericTC::Abort();
	}
	else
	{
		Log(LOG_DEV_DATA, "Vehicle not equipped with speed control, not setting Abort flag!");
	}
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericSpeedControlTC<VehicleModuleType>::InitializeHook(const XmlNode *config)
{	// Initialize all the steps to Testing
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::InitializeHook() - Enter\n");
	m_testStepStatus.verifyOnSwitch     = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyOffSwitch    = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyCancelSwitch = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifySetEnable    = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyResumeEnable = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyBrakeCutOut  = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyClutchCutOut = BEP_TESTING_RESPONSE;
	m_testStepStatus.checkForFaults     = BEP_TESTING_RESPONSE;
	// Call the base class to complete initialization
	GenericTCTemplate<VehicleModuleType>::InitializeHook(config);
	// Load the Valid Speed Control Types from the configuration file
	try
	{
		const XmlNode *validSpeedControl = config->getChild("Setup/Parameters/ValidSpeedControlTypes");
		m_validSpeedControlTypes.DeepCopy(validSpeedControl->getChildren());
		// Check if vehicle is equipped with the Speed Control Component
		//      and set member variable
		CheckForValidSpeedControlType();
	}
	catch(XmlException &err)
	{
		try
		{
			// Error loading Valid Speed Control Types
			Log(LOG_ERRORS, "XmlException loading ValidSpeedControlTypes, looking for NoSpeedControl tag - %s\n", err.what());
			SetCruiseEquipped(SystemRead(GetDataTag("SpeedControl")).compare(GetDataTag("NoSpeedControl")));
		}
		catch(XmlException &err)
		{
			Log(LOG_ERRORS, "XmlException loading NoSpeedControl, default to Speed Control Equipped - %s\n", err.what());
			SetCruiseEquipped(true);
		}
	}
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::InitializeHook() - Exit\n");
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericSpeedControlTC<VehicleModuleType>::WarmInitialize()
{   
	// Initialize all the steps to Testing
	Log( LOG_FN_ENTRY, "Enter GenericSpeedControlTC::WarmInitialize()\n");
	m_testStepStatus.verifyOnSwitch     = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyOffSwitch    = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyCancelSwitch = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifySetEnable    = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyResumeEnable = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyBrakeCutOut  = BEP_TESTING_RESPONSE;
	m_testStepStatus.verifyClutchCutOut = BEP_TESTING_RESPONSE;
	m_testStepStatus.checkForFaults     = BEP_TESTING_RESPONSE;
	// Load the Valid Speed Control Types from the configuration file
	try
	{
		// Check if vehicle is equipped with the Speed Control Component
		//      and set member variable
		CheckForValidSpeedControlType();
	}
	catch(XmlException &err)
	{
		try
		{
			// Error loading Valid Speed Control Types
			Log(LOG_ERRORS, "XmlException loading ValidSpeedControlTypes, looking for NoSpeedControl tag - %s\n", err.what());
			SetCruiseEquipped(SystemRead(GetDataTag("SpeedControl")).compare(GetDataTag("NoSpeedControl")));
		}
		catch(XmlException &err)
		{
			Log(LOG_ERRORS, "XmlException loading NoSpeedControl, default to Speed Control Equipped - %s\n", err.what());
			SetCruiseEquipped(true);
		}
	}
	// Call the base class to complete initialization
	GenericTCTemplate<VehicleModuleType>::WarmInitialize();
	Log( LOG_FN_ENTRY, "Exit GenericSpeedControlTC::WarmInitialize()\n");
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericSpeedControlTC<VehicleModuleType>::CheckForValidSpeedControlType()
{
	bool runSpeedControlTest = false;	// Determine if the entire test should run or skip
	XmlNodeMapItr iter;			// Iterate through the possible valid Speed Control types for this test

	for(iter = m_validSpeedControlTypes.begin(); (iter != m_validSpeedControlTypes.end()) && !runSpeedControlTest; iter++)
	{	// Compare the broadcast Speed Control Type against the current config file valid Speed Control Types
		runSpeedControlTest = !iter->second->getValue().compare(SystemRead(GetDataTag("BroadcastSpeedControlType")));
	}
	SetCruiseEquipped(runSpeedControlTest);
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::Register(void)
{
	m_semInitialized.CriticalSectionEnter();
#if 0
	if(IsCruiseEquipped())
	{
		UpdateTestStatus(TEST_STATUS_IN_PROGRESS);
	}
	else
	{
		UpdateTestStatus(BEP_TEST_NOT_STARTED);
	}
#endif
	return BepServer::Register();
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericSpeedControlTC<VehicleModuleType>::ResetOverallResult(void)
{	// Determine if we are equipped with Satellite Radio.
	if(IsCruiseEquipped()) GenericTC::ResetOverallResult();
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckOnEnable(void)
{	// Read the cruise switch from the module until ON mode is detected
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	string operatorInput;
	bool cruiseOn           = false;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step
		SetStartTime();
		// Setup the InputServer to get operator input
		SetupForOperatorInput();
		// Wait for the operator to switch speed control on
		status = WaitForSpeedControlOn(cruiseOn, operatorInput);
		// Check the exit conditions
		if(!TimeRemaining() && !cruiseOn && (operatorInput.compare(testFail) != 0))
		{	// Timeout waiting for cruise on
			Log(LOG_ERRORS, "Timeout waiting for speed control ON!\n");
			m_testStepStatus.verifyOnSwitch = testTimeout;
			testResultCode = GetFaultCode("SpeedControlOnTimeout");
			testDescription = GetFaultDescription("SpeedControlOnTimeout");
		}
		else if((StatusCheck() != BEP_STATUS_SUCCESS) && !cruiseOn && (operatorInput.compare(testFail) != 0))
		{	// Bad system status
			Log(LOG_ERRORS, "Bad system status waiting for speed control ON - status: %s\n",
				ConvertStatusToResponse(StatusCheck()).c_str());
			m_testStepStatus.verifyOnSwitch = testFail;
			testResultCode = GetFaultCode("SystemStatus");
			testDescription = GetFaultDescription("SystemStatus");
		}
		else if(BEP_STATUS_SUCCESS != status)
		{	// Bad module comms
			Log(LOG_ERRORS, "Communication failure checking for speed control ON - status: %s\n",
				ConvertStatusToResponse(status).c_str());
			m_testStepStatus.verifyOnSwitch = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
		else if(!cruiseOn && (operatorInput.compare(testFail) == 0))
		{	// Operator pressed the fail button
			Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
			m_testStepStatus.verifyOnSwitch = testFail;
			testResultCode = GetFaultCode("OperatorFailOn");
			testDescription = GetFaultDescription("OperatorFailOn");
		}
		else
		{	// Check if speed control was detected ON
			m_testStepStatus.verifyOnSwitch = cruiseOn ? testPass : testFail;
			testResultCode = cruiseOn ? testResultCode : GetFaultCode("CruiseOnNotSeen");
			testDescription = cruiseOn ? testDescription : GetFaultDescription("CruiseOnNotSeen");
			Log(LOG_DEV_DATA, "Speed Control System ON: %s\n", m_testStepStatus.verifyOnSwitch.c_str());
		}
		// Report the results
		SendSubtestResult(GetTestStepName(), m_testStepStatus.verifyOnSwitch, testDescription, testResultCode);
	}
	else
	{	// Test step is skipped - only update the result if not previously passed
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		if(testPass != m_testStepStatus.verifyOnSwitch)	m_testStepStatus.verifyOnSwitch = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifyOnSwitch;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckOffDisable(void)
{	// Read the cruise switch from the module until OFF mode is detected
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	string operatorInput;
	bool cruiseOff          = false;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step
		SetStartTime();
		// Setup the InputServer to get operator input
		SetupForOperatorInput();
		// Wait for the operator to switch speed control off
		status = WaitForSpeedControlOff(cruiseOff, operatorInput);
		// Check the exit conditions
		if(!TimeRemaining() && !cruiseOff && (operatorInput.compare(testFail) != 0))
		{	// Timeout waiting for cruise on
			Log(LOG_ERRORS, "Timeout waiting for speed control OFF!\n");
			m_testStepStatus.verifyOffSwitch = testTimeout;
			testResultCode = GetFaultCode("SpeedControlOffTimeout");
			testDescription = GetFaultDescription("SpeedControlOffTimeout");
		}
		else if((StatusCheck() != BEP_STATUS_SUCCESS) && !cruiseOff && (operatorInput.compare(testFail) != 0))
		{	// Bad system status
			Log(LOG_ERRORS, "Bad system status waiting for speed control OFF - status: %s\n",
				ConvertStatusToResponse(StatusCheck()).c_str());
			m_testStepStatus.verifyOffSwitch = testFail;
			testResultCode = GetFaultCode("SystemStatus");
			testDescription = GetFaultDescription("SystemStatus");
		}
		else if(BEP_STATUS_SUCCESS != status)
		{	// Bad module comms
			Log(LOG_ERRORS, "Communication failure checking for speed control OFF - status: %s\n",
				ConvertStatusToResponse(status).c_str());
			m_testStepStatus.verifyOffSwitch = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
		else if(!cruiseOff && (operatorInput.compare(testFail) == 0))
		{	// Operator pressed the fail button
			Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
			m_testStepStatus.verifyOffSwitch = testFail;
			testResultCode = GetFaultCode("OperatorFailOff");
			testDescription = GetFaultDescription("OperatorFailOff");
		}
		else
		{	// Check if speed control was detected ON
			m_testStepStatus.verifyOffSwitch = cruiseOff ? testPass : testFail;
			testResultCode = cruiseOff ? testResultCode : GetFaultCode("CruiseOffNotSeen");
			testDescription = cruiseOff ? testDescription : GetFaultDescription("CruiseOffNotSeen");
			Log(LOG_DEV_DATA, "Speed Control System OFF: %s\n", m_testStepStatus.verifyOffSwitch.c_str());
		}
		// Report the results
		SendSubtestResult(GetTestStepName(), m_testStepStatus.verifyOffSwitch, testDescription, testResultCode);
	}
	else
	{	// Test step is skipped - only update the result if not previously passed
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		if(testPass != m_testStepStatus.verifyOffSwitch) m_testStepStatus.verifyOffSwitch = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifyOffSwitch;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckSetEngagement(void)
{	// Check for preconditions - cruise must be ON
	string testResultCode  = "0000";
	string testDescription = GetTestStepInfo("Description");
	string speedInRange    = testFail;
	string operatorInput;
	bool   cruiseSet       = false;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Check if this test step should be performed
	if(!ShortCircuitTestStep())
	{	// Make sure the ON test step has passed
		if(!m_testStepStatus.verifyOnSwitch.compare(testPass) || GetParameterBool("SkipOnTestResult"))
		{	// Wait for vehicle speed to be in range
			speedInRange = WaitAndPromptForSpeed(GetParameterFloat("SpeedControlMinSpeed"),
												 GetParameterFloat("SpeedControlMaxSpeed"),
												 GetParameterInt("SpeedRangeCheckDelay"));
			// Set Green Speed Bands for the speed range
			if(GetParameterBool("MarkSpeedDuringStableCheck") == true)
				SystemWrite(GetDataTag("SpeedTarget"), string(GetParameter("SpeedControlMinSpeed")+
															  " "+GetParameter("SpeedControlMaxSpeed")));

			// Check if speed range was achieved
			if(testPass == speedInRange)
			{	// Speed is in range, set the cruise
				if(!GetParameterBool("CombinedSetAndFootoffThrottlePrompts"))
				{
					DisplayPrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
					DisplayPrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
								  GetPromptPriority("RemoveFootPrompt"));
				}
				else
				{
					DisplayPrompt(GetPromptBox("SetAndRemovePrompt"), GetPrompt("SetAndRemovePrompt"), GetPromptPriority("SetAndRemovePrompt"));
				}
				// Setup the InputServer to get operator input
				SetupForOperatorInput();
				// Set the timer so we can look for the SET switch
				SetStartTime();
				// Wait for the operator to press the set button
				status = WaitForSetButton(cruiseSet, operatorInput);
				// Check the exit conditions
				if(!TimeRemaining() && !cruiseSet && (operatorInput.compare(testFail) != 0))
				{	// Timeout waiting for SET button to be pressed
					Log(LOG_ERRORS, "Timeout waiting for speed control to be SET\n");
					m_testStepStatus.verifySetEnable = testTimeout;
					testResultCode = GetFaultCode("SpeedControlSetTimeout");
					testDescription = GetFaultDescription("SpeedControlSetTimeout");
				}
				else if((BEP_STATUS_SUCCESS != StatusCheck()) && !cruiseSet && (operatorInput.compare(testFail) != 0))
				{	// Bad system status waiting for cruise set
					Log(LOG_ERRORS, "Bad system status waiting for speed control SET - status: %s\n",
						ConvertStatusToResponse(StatusCheck()).c_str());
					m_testStepStatus.verifySetEnable = testFail;
					testResultCode = GetFaultCode("SystemStatus");
					testDescription = GetFaultDescription("SystemStatus");
				}
				else if(BEP_STATUS_SUCCESS != status)
				{	// Communication failure waiting for cruise set
					Log(LOG_ERRORS, "Communication Failure checking for speed control SET - status: %s\n",
						ConvertStatusToResponse(status).c_str());
					m_testStepStatus.verifySetEnable = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
				else if(!cruiseSet && (operatorInput.compare(testFail) == 0))
				{	// Operator pressed the fail button
					Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
					m_testStepStatus.verifySetEnable = testFail;
					testResultCode = GetFaultCode("OperatorFailSet");
					testDescription = GetFaultDescription("OperatorFailSet");
				}
				else
				{	// SET button pressed, look for speed stabilization
					bool zeroThrottle = false;
					if(!GetParameterBool("CombinedSetAndFootoffThrottlePrompts"))
					{
						RemovePrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
					}
					else
					{
						RemovePrompt(GetPromptBox("SetAndRemovePrompt"), GetPrompt("SetAndRemovePrompt"), GetPromptPriority("SetAndRemovePrompt"));
					}
					DisplayPrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
								  GetPromptPriority("CheckStableSpeed"));
					// Store the target speed
					status = SetSpeedControlTargetSpeed();
					// Check the status of the read
					if(BEP_STATUS_SUCCESS == status)
					{	// Check for stable speed
						if(IsVehicleSpeedStable(GetSpeedControlTargetSpeed(), zeroThrottle))
						{	// Test passed
							m_testStepStatus.verifySetEnable = testPass;
							testResultCode = "0000";
							testDescription = GetTestStepInfo("Description");
						}
						else
						{	// Test failed
							m_testStepStatus.verifySetEnable = testFail;
							testResultCode = zeroThrottle ? GetFaultCode("VehicleSpeedNotStable") : GetFaultCode("ThrottleApplied");
							testDescription = zeroThrottle ? GetFaultDescription("VehicleSpeedNotStable") : GetFaultDescription("ThrottleApplied");
						}
						Log("Cruise SET feature functional - %s\n", m_testStepStatus.verifySetEnable.c_str());
					}
					else
					{	// Communication error
						m_testStepStatus.verifySetEnable = testFail;
						testResultCode = GetFaultCode("CommunicationFailure");
						testDescription = GetFaultDescription("CommunicationFailure");
						Log("Error reading target speed for SET - status: %s\n", ConvertStatusToResponse(status).c_str());
					}
				}
				// Remove all prompts
				RemovePrompt(GetPromptBox("SetPrompt"), GetPrompt("SetPrompt"), GetPromptPriority("SetPrompt"));
				RemovePrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"),
							 GetPromptPriority("CheckStableSpeed"));
				RemovePrompt(GetPromptBox("RemoveFootPrompt"), GetPrompt("RemoveFootPrompt"),
							 GetPromptPriority("RemoveFootPrompt"));
			}
			else
			{	// Speed was not in range
				m_testStepStatus.verifySetEnable = testFail;
				testResultCode = GetFaultCode("SpeedNotInRange");
				testDescription = GetFaultDescription("SpeedNotInRange");
				Log("Vehicle Speed out of range to check SET engagement\n");
			}
			// Remove Green Speed Bands
			if(GetParameterBool("MarkSpeedDuringStableCheck") == true)
				SystemWrite(GetDataTag("SpeedTarget"), "0 0");
		}
		else
		{	// Cruise was not ON, cannot perform this step
			Log("Error - cruise ON mode not seen prior to SET test\n");
			m_testStepStatus.verifySetEnable = testSkip;
			testResultCode = GetFaultCode("ConditionsNotCorrect");
			testDescription = GetFaultDescription("ConditionsNotCorrect");
		}
		// Report the results
		char temp[16];
		SendSubtestResultWithDetail(GetTestStepName(), m_testStepStatus.verifySetEnable, testDescription, testResultCode,
									"TargetSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", GetSpeedControlTargetSpeed()), unitsMPH);
	}
	else
	{	// Skipped test step
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		// Only update the test step if not already pass
		if(m_testStepStatus.verifySetEnable != testPass)  m_testStepStatus.verifySetEnable = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifySetEnable;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckResumeEngagement(void)
{	// Check preconditions -- Cruise ON and SET must have been performed
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResultCode  = "0000";
	string testDescription = GetTestStepInfo("Description");
	string operatorInput;
	bool   buttonPressed   = false;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test step should be performed
	if(!ShortCircuitTestStep())
	{	// Ensure the ON and SET test steps have passed
		if((m_testStepStatus.verifyOnSwitch == testPass) && (m_testStepStatus.verifySetEnable == testPass))
		{	// Display the prompts to the driver
			DisplayPrompt(GetPromptBox("PressResumePrompt"), GetPrompt("PressResumePrompt"), GetPromptPriority("PressResumePrompt"));
			DisplayPrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Wait for the RESUME button to be pressed or a timeout to occur
			SetStartTime();	  // Start the timer
			// Wait for the operator to press the resume button
			status = WaitForResumeButton(buttonPressed, operatorInput);
			// Check the exit conditions
			if(!TimeRemaining() && !buttonPressed && !buttonPressed && (operatorInput.compare(testFail) != 0))
			{	// Timeout waiting for the RESUME button press
				Log(LOG_ERRORS, "Timeout waiting for RESUME button to be pressed\n");
				m_testStepStatus.verifyResumeEnable = testTimeout;
				testResultCode = GetFaultCode("SpeedControlResumeTimeout");
				testDescription = GetFaultDescription("SpeedControlResumeTimeout");
			}
			else if((BEP_STATUS_SUCCESS != StatusCheck()) && !buttonPressed && (operatorInput.compare(testFail) != 0))
			{	// Bad system status waiting for RESUME button
				Log(LOG_ERRORS, "Bad system status waiting for RESUME mode - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
				m_testStepStatus.verifyResumeEnable = testFail;
				testResultCode = GetFaultCode("SystemStatus");
				testDescription = GetFaultDescription("SystemStatus");
			}
			else if(BEP_STATUS_SUCCESS != status)
			{	// Comm failures looking for RESUME button
				Log(LOG_ERRORS, "Communication failure checking for RESUME mode - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
				m_testStepStatus.verifyResumeEnable = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			else if(!buttonPressed && (operatorInput.compare(testFail) == 0))
			{	// Operator pressed the fail button
				Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
				m_testStepStatus.verifyResumeEnable = testFail;
				testResultCode = GetFaultCode("OperatorFailResume");
				testDescription = GetFaultDescription("OperatorFailResume");
			}
			else
			{	// saw the RESUME button, remove the prompt and check for increasing speed
				RemovePrompt(GetPromptBox("PressResumePrompt"), GetPrompt("PressResumePrompt"),
							 GetPromptPriority("PressResumePrompt"));
				bool speedIncreasing = IsSpeedIncreasing();
				m_testStepStatus.verifyResumeEnable = speedIncreasing ? testPass : testFail;
				testResultCode = speedIncreasing ? testResultCode : GetFaultCode("ResumeSpeedNotIncreasing");
				testDescription = speedIncreasing ? testDescription : GetFaultDescription("ResumeSpeedNotIncreasing");
			}
			// Ensure the prompt has been removed
			RemovePrompt(GetPromptBox("PressResumePrompt"), GetPrompt("PressResumePrompt"), GetPromptPriority("PressResumePrompt"));
			RemovePrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
		}
		else
		{	// Conditions not correct - ON and SET not passed prior to this step
			Log("Error - ON mode and SET mode not passed prior to RESUME test\n");
			m_testStepStatus.verifyResumeEnable = testSkip;
			testResultCode = GetFaultCode("ConditionsNotCorrect");
			testDescription = GetFaultDescription("ConditionsNotCorrect");
		}
		// Report the results
		SendSubtestResult(GetTestStepName(), m_testStepStatus.verifyResumeEnable, testDescription, testResultCode);
	}
	else
	{	// Skipped test step
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		// Only update the test step if not already pass
		if(m_testStepStatus.verifyResumeEnable != testPass)	 m_testStepStatus.verifyResumeEnable = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifyResumeEnable;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckBrakeCutOut(void)
{	// Ensure pre-conditions have been met - ON and SET tests passed
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	string operatorInput;
	float finalVehicleSpeed = 0.0;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Ensure the ON and SET test steps have passed first
		if((m_testStepStatus.verifyOnSwitch == testPass) && (m_testStepStatus.verifySetEnable == testPass))
		{	// Look for brake switch
			bool brakePressed = false;
			//Possible delay before telling the driver to tap the brake
			if(GetParameterInt("DelayBeforeBrakeTap") != 0)
			{
				MaintainSpeedForTime(GetParameterInt("DelayBeforeBrakeTap"), GetParameterInt("SpeedControlMinSpeed"),
									 GetParameterInt("SpeedControlMaxSpeed"), "DelayBeforeTapPrompt", true, false);
			}
			// Instruct operator to tap the brake pedal
			DisplayPrompt(GetPromptBox("TapBrakePrompt"), GetPrompt("TapBrakePrompt"), GetPromptPriority("TapBrakePrompt"));
			DisplayPrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Wait for the operator to tap the brake pedal
			SetStartTime();
			status = WaitForBrakeTap(brakePressed, operatorInput);
			// Check the exit conditions and remove the prompt
			RemovePrompt(GetPromptBox("TapBrakePrompt"), GetPrompt("TapBrakePrompt"), GetPromptPriority("TapBrakePrompt"));
			if(!TimeRemaining() && !brakePressed && (operatorInput.compare(testFail) != 0))
			{	// Timeout waiting for brake tap
				Log(LOG_ERRORS, "Timeout while waiting for brake tap!\n");
				m_testStepStatus.verifyBrakeCutOut = testTimeout;
				testResultCode = GetFaultCode("BrakeTapTimeout");
				testDescription = GetFaultDescription("BrakeTapTimeout");
			}
			else if((BEP_STATUS_SUCCESS != StatusCheck()) && (operatorInput.compare(testFail) != 0))
			{	// Bad system status waiting for brake tap
				Log(LOG_ERRORS, "Bad system status while looking for brake tap - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
				m_testStepStatus.verifyBrakeCutOut = testFail;
				testResultCode = GetFaultCode("SystemStatus");
				testDescription = GetFaultDescription("SystemStatus");
			}
			else if(BEP_STATUS_SUCCESS != status)
			{	// Comm failure with module
				Log(LOG_ERRORS, "Communication Failure while looking for brake tap - status: %s\n",
					ConvertStatusToResponse(status).c_str());
				m_testStepStatus.verifyBrakeCutOut = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			else if(!brakePressed && (operatorInput.compare(testFail) == 0))
			{	// Operator pressed the fail button
				Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
				m_testStepStatus.verifyBrakeCutOut = testFail;
				testResultCode = GetFaultCode("OperatorFailBrake");
				testDescription = GetFaultDescription("OperatorFailBrake");
			}
			else
			{	// Brake pedal was tapped, look for decreasing speed
				bool deltaSpeedAchieved = IsDeltaSpeedAchieved(GetSpeedControlTargetSpeed(), GetParameterFloat("SpeedControlDeltaSpeed"));
				// Get the final vehicle speed
				ReadVehicleSpeed(finalVehicleSpeed);
				// Determine the test results
				m_testStepStatus.verifyBrakeCutOut = deltaSpeedAchieved ? testPass : testFail;
				testResultCode = deltaSpeedAchieved ? testResultCode : GetFaultCode("BrakeSpeedDropOff");
				testDescription = deltaSpeedAchieved ? testDescription :  GetFaultDescription("BrakeSpeedDropOff");
				// Log the data
				Log(LOG_DEV_DATA, "Brake cutout - %s, TargetSpeed: %.2f, FinalSpeed: %.2f, RequiredDelta: %s\n",
					m_testStepStatus.verifyBrakeCutOut.c_str(), GetSpeedControlTargetSpeed(), finalVehicleSpeed,
					GetParameter("SpeedControlDeltaSpeed").c_str());
			}
			RemovePrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
		}
		else
		{	// Conditions not correct for testing
			Log("Error - ON and SET tests not passed prior to checking for Brake Cutout\n");
			m_testStepStatus.verifyBrakeCutOut = testSkip;
			testResultCode = GetFaultCode("ConditionsNotCorrect");
			testDescription = GetFaultDescription("ConditionsNotCorrect");
		}
		// Return the test result
		char temp[16];
		SendSubtestResultWithDetail(GetTestStepName(), m_testStepStatus.verifyBrakeCutOut, testDescription, testResultCode,
									"TargetSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", GetSpeedControlTargetSpeed()), "MPH",
									"DeltaSpeed", GetParameter("SpeedControlDeltaSpeed"), "MPH",
									"FinalVehicleSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", finalVehicleSpeed), "MPH");
	}
	else
	{	// Test step has been skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		// Only update the test result if not previously passed
		if(m_testStepStatus.verifyBrakeCutOut != testPass)	m_testStepStatus.verifyBrakeCutOut = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifyBrakeCutOut;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckClutchCutOut(void)
{	// Ensure pre-conditions have been met - ON and SET tests passed
	string testResultCode    = "0000";
	string testDescription   = GetTestStepInfo("Description");
	string operatorInput;
	float  finalVehicleSpeed = 0.0;
	BEP_STATUS_TYPE status   = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Only perform this step if ON and SET steps have passed
		if((m_testStepStatus.verifyOnSwitch == testPass) && (m_testStepStatus.verifySetEnable == testPass))
		{	// Look for clutch switch
			bool clutchPressed = false;
			// Display the prompt for the driver
			DisplayPrompt(GetPromptBox("PressClutchPrompt"), GetPrompt("PressClutchPrompt"), GetPromptPriority("PressClutchPrompt"));
			DisplayPrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Wait for the operator to press the brake pedal
			SetStartTime();
			status = WaitForClutchPedal(clutchPressed, operatorInput);
			// Check the exit conditions from the loop and remove the prompt
			RemovePrompt(GetPromptBox("PressClutchPrompt"), GetPrompt("PressClutchPrompt"), GetPromptPriority("PressClutchPrompt"));
			if(!TimeRemaining() && !clutchPressed && (operatorInput.compare(testFail) != 0))
			{	// Timeout waiting for the clutch pedal to be pressed
				Log(LOG_ERRORS, "Timeout waiting for operator to press the clutch pedal\n");
				m_testStepStatus.verifyClutchCutOut = testTimeout;
				testResultCode = GetFaultCode("ClutchSwitchTimeout");
				testDescription = GetFaultDescription("ClutchSwitchTimeout");
			}
			else if((BEP_STATUS_SUCCESS == StatusCheck()) && (operatorInput.compare(testFail) != 0))
			{	// Bad system status waiting for clutch switch
				Log(LOG_ERRORS, "Bad system status waiting for operator to press the clutch pedal - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
				m_testStepStatus.verifyClutchCutOut = testFail;
				testResultCode = GetFaultCode("SystemStatus");
				testDescription = GetFaultDescription("SystemStatus");
			}
			else if(BEP_STATUS_SUCCESS == status)
			{	// Communication failure looking for clutch switch
				Log(LOG_ERRORS, "Communication failure waiting for operator to press the clutch pedal - status: %s\n",
					ConvertStatusToResponse(status).c_str());
				m_testStepStatus.verifyClutchCutOut = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			else if(!clutchPressed && (operatorInput.compare(testFail) == 0))
			{	// Operator pressed the fail button
				Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
				m_testStepStatus.verifyClutchCutOut = testFail;
				testResultCode = GetFaultCode("OperatorFailClutch");
				testDescription = GetFaultDescription("OperatorFailClutch");
			}
			else
			{	// Saw the clutch pedal pressed, wait for speed to drop off
				bool deltaSpeedAchieved = IsDeltaSpeedAchieved(GetSpeedControlTargetSpeed(), GetParameterFloat("SpeedControlDeltaSpeed"));
				// Get the final vehicle speed
				ReadVehicleSpeed(finalVehicleSpeed);
				// Determine the test results
				m_testStepStatus.verifyClutchCutOut = deltaSpeedAchieved ? testPass : testFail;
				testResultCode = deltaSpeedAchieved ? testResultCode : GetFaultCode("ClutchSpeedDropOff");
				testDescription = deltaSpeedAchieved ? testDescription : GetFaultDescription("ClutchSpeedDropOff");
				// Log the results
				Log(LOG_DEV_DATA, "Clutch cutout - %s, TargetSpeed: %.2f, FinalSpeed: %.2f, RequiredDelta: %s\n",
					m_testStepStatus.verifyClutchCutOut.c_str(), GetSpeedControlTargetSpeed(),
					finalVehicleSpeed, GetParameter("SpeedControlDeltaSpeed").c_str());
			}
			RemovePrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
		}
		else
		{	// Conditions not correct for testing
			Log("Error - ON and SET tests not passed prior to checking for Clutch Cutout\n");
			m_testStepStatus.verifyClutchCutOut = testSkip;
			testResultCode = GetFaultCode("ConditionsNotCorrect");
			testDescription = GetFaultDescription("ConditionsNotCorrect");
		}
		// Return the test result
		char temp[16];
		SendSubtestResultWithDetail(GetTestStepName(), m_testStepStatus.verifyClutchCutOut, testDescription, testResultCode,
									"TargetSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", GetSpeedControlTargetSpeed()), unitsMPH,
									"DeltaSpeed", GetParameter("SpeedControlDeltaSpeed"), unitsMPH,
									"FinalVehicleSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", finalVehicleSpeed), unitsMPH);
	}
	else
	{	// Skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
		// Only update the result if not previous pass
		if(m_testStepStatus.verifyClutchCutOut != testPass)	 m_testStepStatus.verifyClutchCutOut = testSkip;
	}
	return m_testStepStatus.verifyClutchCutOut;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckCancelCutOut(void)
{	// Ensure pre-conditions have been met - ON and SET tests passed
	string testResultCode    = "0000";
	string testDescription   = GetTestStepInfo("Description");
	string operatorInput;
	float  finalVehicleSpeed = 0.0;
	BEP_STATUS_TYPE status   = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// make sure the ON and SET tests have passed first
		if((m_testStepStatus.verifyOnSwitch == testPass) && (m_testStepStatus.verifySetEnable == testPass))
		{	// Look for cancel switch
			bool cancelPressed = false;
			// Display the prompt for the driver
			DisplayPrompt(GetPromptBox("CancelPrompt"), GetPrompt("CancelPrompt"), GetPromptPriority("CancelPrompt"));
			DisplayPrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Wait for the operator to press the brake pedal
			SetStartTime();
			status = WaitForCancelSwitch(cancelPressed, operatorInput);
			// Remove the prompt from the display
			RemovePrompt(GetPromptBox("CancelPrompt"), GetPrompt("CancelPrompt"), GetPromptPriority("CancelPrompt"));
			// Check the loop exit conditions
			if(!TimeRemaining() && !cancelPressed && (operatorInput.compare(testFail) != 0))
			{	// Timeout waiting for cancel switch
				Log(LOG_ERRORS, "Timeout waiting for cancel switch\n");
				m_testStepStatus.verifyCancelSwitch = testTimeout;
				testResultCode = GetFaultCode("CancelSwitchTimeout");
				testDescription = GetFaultDescription("CancelSwitchTimeout");
			}
			else if((BEP_STATUS_SUCCESS != StatusCheck()) && (operatorInput.compare(testFail) != 0))
			{	// Bad system status looking for cancel button
				Log(LOG_ERRORS, "Bad system status waiting for cancel switch - status: %s\n",
					ConvertStatusToResponse(StatusCheck()).c_str());
				m_testStepStatus.verifyCancelSwitch = testFail;
				testResultCode = GetFaultCode("SystemStatus");
				testDescription = GetFaultDescription("SystemStatus");
			}
			else if(BEP_STATUS_SUCCESS != status)
			{	// Communication failure waiting for cancel switch
				Log(LOG_ERRORS, "Communication failure waiting for cancel switch - status: %s\n",
					ConvertStatusToResponse(status).c_str());
				m_testStepStatus.verifyCancelSwitch = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			else if(!cancelPressed && (operatorInput.compare(testFail) == 0))
			{	// Operator pressed the fail button
				Log(LOG_DEV_DATA, "Operator pressed Fail button during %s\n", GetTestStepName().c_str());
				m_testStepStatus.verifyCancelSwitch = testFail;
				testResultCode = GetFaultCode("OperatorFailCancel");
				testDescription = GetFaultDescription("OperatorFailCancel");
			}
			else
			{	// saw cancel button, look for the speed to drop off
				bool deltaSpeedAchieved = IsDeltaSpeedAchieved(GetSpeedControlTargetSpeed(), GetParameterFloat("SpeedControlDeltaSpeed"));
				// Get the final speed
				ReadVehicleSpeed(finalVehicleSpeed);
				// Determine the test result
				m_testStepStatus.verifyCancelSwitch = deltaSpeedAchieved ? testPass : testFail;
				testResultCode = deltaSpeedAchieved ? testResultCode : GetFaultCode("CancelSpeedDropOff");
				testDescription = deltaSpeedAchieved ? testDescription : GetFaultDescription("CancelSpeedDropOff");
				// Log the data
				Log(LOG_DEV_DATA, "Cancel cutout - %s, TargetSpeed: %.2f, FinalSpeed: %.2f, RequiredDelta: %s\n",
					m_testStepStatus.verifyCancelSwitch.c_str(), GetSpeedControlTargetSpeed(),
					finalVehicleSpeed, GetParameter("SpeedControlDeltaSpeed").c_str());
			}
			RemovePrompt(GetPromptBox("PressFailPrompt"), GetPrompt("PressFailPrompt"), GetPromptPriority("PressFailPrompt"));
		}
		else
		{	// Conditions not correct for testing
			Log("Error - ON and SET tests not passed prior to checking for Cancel Switch\n");
			m_testStepStatus.verifyCancelSwitch = testSkip;
			testResultCode = GetFaultCode("ConditionsNotCorrect");
			testDescription = GetFaultDescription("ConditionsNotCorrect");
		}
		// Report the test result
		char temp[16];
		SendSubtestResultWithDetail(GetTestStepName(), m_testStepStatus.verifyCancelSwitch, testDescription, testResultCode,
									"TargetSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", GetSpeedControlTargetSpeed()), unitsMPH,
									"DeltaSpeed", GetParameter("SpeedControlDeltaSpeed"), unitsMPH,
									"FinalVehicleSpeed", CreateMessage(temp, sizeof(temp), "%4.1f", finalVehicleSpeed), unitsMPH);
	}
	else
	{	// Skipping the test step
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		// Only update the result if this is not previous pass
		if(m_testStepStatus.verifyCancelSwitch != testPass)	 m_testStepStatus.verifyCancelSwitch = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return m_testStepStatus.verifyCancelSwitch;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckTapSpeedChange(const string &direction)
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	string promptTag = !direction.compare("Up") ? "PressTapUpButton" : "PressTapDownButton";
	// Log the entry and determine if this step should be performed
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if(!ShortCircuitTestStep())
	{	// Get the initial vehicle speed
		float finalSpeed = 0.0;
		bool  speedChanged = false;
		float targetSpeed = GetParameterFloat("TapSpeedChangeTarget" + direction);
		if(ReadVehicleSpeed(finalSpeed) == BEP_STATUS_SUCCESS)
		{	// Prompt the operator to press the accel button
			DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
			SystemWrite(GetDataTag("SpeedTarget"), GetParameter("TapSpeedChangeRange" + direction));
			// Setup the InputServer to get operator input
			SetupForOperatorInput();
			// Wait for the vehicle speed to increase
			BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
			string operatorInput   = "Unknown";
			do
			{	// Read the vehicle speed and compare to the initial speed
				status = ReadVehicleSpeed(finalSpeed);
				if(BEP_STATUS_SUCCESS == status)
				{	// Check if the speed diff has been met
					if(!direction.compare("Up"))
					{
						Log(LOG_DEV_DATA, "Waiting for minimum vehicle speed of: %.2f, currently: %.2f", targetSpeed, finalSpeed);
						speedChanged = (finalSpeed >= targetSpeed);
					}
					else
					{
						Log(LOG_DEV_DATA, "Waiting for maximum vehicle speed of: %.2f, currently: %.2f", targetSpeed, finalSpeed);
						speedChanged = (finalSpeed <= targetSpeed);
					}
					// If speed increase not observed, wait a bit
					if(!speedChanged)  BposSleep(GetTestStepInfoInt("ScanDelay"));
				}
				// Check for operator input
				operatorInput = CheckForOperatorFail();
				// Keep checking while good status and speed has not increased
			} while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status) &&
					!speedChanged && operatorInput.compare(testFail));
			// optional post wait time
			BposSleep(GetParameterInt("PostTapSpeedChangeDelay"));
			// Remove the prompts
			RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
			SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
			// Check if we passed or failed
			if(!TimeRemaining())
			{	// We have timed out
				Log(LOG_ERRORS, "Timeout waiting for speed change");
				testResult = testTimeout;
				testResultCode = GetFaultCode("Tap"+direction+"Timeout");
				testDescription = GetFaultDescription("Tap"+direction+"Timeout");
			}
			else if(BEP_STATUS_SUCCESS != StatusCheck())
			{	// Bad system status
				Log(LOG_ERRORS, "System status is bad - %s", ConvertStatusToResponse(StatusCheck()).c_str());
				testResult = testAbort;
				testResultCode = GetFaultCode("StatusCheckFailure");
				testDescription = GetFaultDescription("StatusCheckFailure");
			}
			else if(BEP_STATUS_SUCCESS != status)
			{	// Error reading vehicle speed
				Log(LOG_ERRORS, "Failed to read vehicle speed: %s", ConvertStatusToResponse(status).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("VehicleSpeedRead");
				testDescription = GetFaultDescription("VehicleSpeedRead");
			}
			else if(operatorInput.compare(testPass) && !speedChanged)
			{	// Operator pressed the fail or abort button
				Log(LOG_ERRORS, "Operator pressed the %s button", operatorInput.c_str());
				testResult = operatorInput;
				testResultCode = GetFaultCode("OperatorFailTap"+direction);
				testDescription = GetFaultDescription("OperatorFailTap"+direction);
			}
			else
			{	// Check if speed increased
				testResult = speedChanged ? testPass : testFail;
				testResultCode = speedChanged ? "0000" : GetFaultCode("Tap"+direction+"SpeedChange");
				testDescription = speedChanged ? testDescription : GetFaultDescription("Tap"+direction+"SpeedChange");
				Log(LOG_DEV_DATA, "Tap %s Speed Change: %s", direction.c_str(), testResult.c_str());
			}
		}
		else
		{	// Could not read initial speed
			Log(LOG_DEV_DATA, "Could not read initial vehicle speed.");
			testResult = testFail;
		}
		// Report the results
		char buf[32];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "TargetSpeed", CreateMessage(buf, sizeof(buf), "%05.2f", targetSpeed), unitsMPH,
								 "FinalSpeed", CreateMessage(buf, sizeof(buf), "%05.2f", finalSpeed), unitsMPH,
								 "MinSpeedChange", GetParameter("TapSpeedChangeMinSpeedChange"), unitsMPH);
	}
	else
	{	// Need to skip this test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step: %s", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//---------------------------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckSpeedLimiter(void)
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	float  speedLimit   = 0.0;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Log the entry and check if the test should be performed
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	if(!ShortCircuitTestStep())
	{	// Wait for vehicle speed in range before running the speed limiter test
		float minSpeed = (SystemReadFloat("MaxSpeed") * 0.621371192) - GetParameterFloat("SpeedBelowMaxSpeedForLimiterTest") - GetParameterFloat("SpeedLimiterSpeedRange");
		float maxSpeed = (SystemReadFloat("MaxSpeed") * 0.621371192) - GetParameterFloat("SpeedBelowMaxSpeedForLimiterTest") + GetParameterFloat("SpeedLimiterSpeedRange");
		testResult = WaitAndPromptForSpeed(minSpeed, maxSpeed, GetTestStepInfoInt("ScanDelay"));
		if(!testResult.compare(testPass))
		{	// Prompt the operator to engage the speed limiter
			DisplayPrompt(GetPromptBox("EngageSpeedLimiter"), GetPrompt("EngageSpeedLimiter"), GetPromptPriority("EngageSpeedLimiter"));
			// Prompt the operator to accelerate to the speed limiter
			DisplayPrompt(GetPromptBox("AccelToSpeedLimiter"), GetPrompt("AccelToSpeedLimiter"), GetPromptPriority("AccelToSpeedLimiter"));
			// Wait for the operator to engage the speed limiter
			BposSleep(GetParameterInt("SpeedLimiterEngageDelay"));
			// Determine if we know what the set speed limit is
			if(GetParameterBool("CheckSpeedAgainstLimit"))
			{	// Determine where to get the speed limit from
				if(GetParameterBool("ReadSpeedLimitFromModule"))
				{	// Read the speed limit from the module
					status = m_vehicleModule.ReadModuleData(GetDataTag("ReadSpeedLimit"), speedLimit);
				}
				else
				{	// Use the speed limit from the config file
					speedLimit = GetParameterFloat("SpeedLimiterSpeedLimit");
					status = BEP_STATUS_SUCCESS;
				}
				// Only continue with the test if good status
				RemovePrompt(GetPromptBox("EngageSpeedLimiter"), GetPrompt("EngageSpeedLimiter"), GetPromptPriority("EngageSpeedLimiter"));
				RemovePrompt(GetPromptBox("AccelToSpeedLimiter"), GetPrompt("AccelToSpeedLimiter"), GetPromptPriority("AccelToSpeedLimiter"));
				if(BEP_STATUS_SUCCESS == status)
				{	// Wait for operator to acclerate above the speed limit
					float speedLimitTarget = speedLimit + GetParameterFloat("MinimumSpeedOverLimit");
					testResult = AccelerateToTestSpeed(speedLimitTarget, GetParameter("SpeedLimiterRange"), GetTestStepInfoInt("ScanDelay"), false);
					// Check result of limit testing, if timeout vehicle passes
					testResult = !testResult.compare(testTimeout) ? testPass : testFail;
					testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("SpeedLimiterFail");
					testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("SpeedLimiterFail");
					Log(LOG_DEV_DATA, "Speed Limiter Result: %s", testResult.c_str());
				}
				else
				{	// Could not read speed limit from the module
					Log(LOG_ERRORS, "Could not read speed limit from the module: %s", ConvertStatusToResponse(status).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("VehicleSpeedLimitRead");
					testDescription = GetFaultDescription("VehicleSpeedLimitRead");
				}
			}
			else
			{	// Just prompt the operator
#if 0
				SetupForOperatorInput();
				vector<string> validResponses;
				SetupOperatorResponses(INPUT_SERVER_TEST_RESULT_STATE, validResponses);
				testResult = GetOperatorInput(INPUT_SERVER_TEST_RESULT_STATE, "SpeedLimiterResult", validResponses);
				Log(LOG_DEV_DATA, "Speed Limiter Operator result: %s", testResult.c_str());
#else
				testResult = testPass;
#endif 
				if(!testResult.compare(testPass))
				{
					bool zeroThrottle = false;
					RemovePrompt(GetPromptBox("EngageSpeedLimiter"), GetPrompt("EngageSpeedLimiter"), GetPromptPriority("EngageSpeedLimiter"));
					RemovePrompt(GetPromptBox("AccelToSpeedLimiter"), GetPrompt("AccelToSpeedLimiter"), GetPromptPriority("AccelToSpeedLimiter"));
					DisplayPrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"), GetPromptPriority("CheckStableSpeed"));
					testResult = IsVehicleSpeedStable(GetRollSpeed(), zeroThrottle) ? testPass : testFail;
					RemovePrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"), GetPromptPriority("CheckStableSpeed"));
				}
				// Check the result to set the fault codes
				if(!testResult.compare(testTimeout))
				{	// Tiemout waiting for operator to buyoff feature
					testResultCode = GetFaultCode("OperatorTimeoutSpeedLimiter");
					testDescription = GetFaultDescription("OperatorTimeoutSpeedLimiter");
				}
				else if(!testResult.compare(testFail))
				{	// Operator pressed the fail button
					testResultCode = GetFaultCode("OperatorFailSpeedLimiter");
					testDescription = GetFaultDescription("OperatorFailSpeedLimiter");
				}
				else if(!testResult.compare(testAbort))
				{	// Operator pressed the abort key
					testResultCode = GetFaultCode("OperatorAbortSpeedLimiter");
					testDescription = GetFaultDescription("OperatorAbortSpeedLimiter");
				}
			}
			// Promt the operator to disengage the speed limitor
			RemovePrompt(GetPromptBox("AccelToSpeedLimiter"), GetPrompt("AccelToSpeedLimiter"), GetPromptPriority("AccelToSpeedLimiter"));
			if(GetParameterBool("PromptToDisengageSpeedLimiter"))
			{
				DisplayPrompt(GetPromptBox("DisengageSpeedLimiter"), GetPrompt("DisengageSpeedLimiter"), GetPromptPriority("DisengageSpeedLimiter"));
			}
		}
		else
		{
			Log(LOG_ERRORS, "Vehicle failed to reach speed limiter speed");
		}
		// Report the result
		SendTestResult(testResult, testDescription, testResultCode);
		if(GetParameterBool("PromptToDisengageSpeedLimiter"))
		{
			BposSleep(GetParameterInt("SpeedLimiterEngageDelay"));
			RemovePrompt(GetPromptBox("DisengageSpeedLimiter"), GetPrompt("DisengageSpeedLimiter"), GetPromptPriority("DisengageSpeedLimiter"));
		}
	}
	else
	{	// Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping test step: %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericSpeedControlTC<VehicleModuleType>::CheckKickDown(void)
{	// Log the entry and determine if this step should be performed
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::CheckKickDown() - Enter");
	if(!ShortCircuitTestStep())
	{	// Reset the speed control set point
		if(!GetParameterBool("UseVehicleMaxSpeedForKickdownTarget"))
		{
			SetSpeedControlTargetSpeed();
		}
		else
		{
			m_targetSpeed = SystemReadFloat("MaxSpeed");
		}
		// Prompt the operator to accelerate at full throttle
		DisplayPrompt(GetPromptBox("OperateKickDownPedal"), GetPrompt("OperateKickDownPedal"), GetPromptPriority("OperateKickDownPedal"));
		DisplayPrompt(GetPromptBox("AccelerateAtFullThrottle"), GetPrompt("AccelerateAtFullThrottle"), 
					  GetPromptPriority("AccelerateAtFullThrottle"));
		// Check for speed increasing
		result = IsSpeedIncreasing() ? testPass : testFail;
		BposSleep(GetParameterInt("PostKickdownDelay"));
		RemovePrompt(GetPromptBox("AccelerateAtFullThrottle"), GetPrompt("AccelerateAtFullThrottle"), 
					 GetPromptPriority("AccelerateAtFullThrottle"));
		// Report the result
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping CheckKickDown");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::CheckKickDown() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericSpeedControlTC<VehicleModuleType>::CommandSecurityUnlock(void)
{
	string testResult      = BEP_TESTING_STATUS;
	string testResultCode  = "0000";
	string testDescription = "Test in progress";
	bool vehicleConnected  = false;
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Wait for the preconditions to be satisfied
	if((vehicleConnected = CheckIgnitionOn()) == true)
	{	// Vehicle is connected, unlock security
		for(INT32 unlockAttempt = 0;
		   (unlockAttempt < GetParameterInt("SecurityUnlockAttempts")) && (moduleStatus != BEP_STATUS_SUCCESS);
		   unlockAttempt++)
		{	// Command the module to unlock security
			try
			{
				moduleStatus = m_vehicleModule.UnlockModuleSecurity();
			}
			catch(ModuleException &excpt)
			{	// Exception during security unlock
				Log(LOG_ERRORS, "Module Exception durinh CommandSecurityUnlock() - %s\n", excpt.GetReason());
				moduleStatus = BEP_STATUS_SOFTWARE;
			}
		}
		// Determine the test result
		testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testAbort);
		testResultCode = (testResult == testPass ? "0000" : GetFaultCode("SecurityUnlockFailed"));
		testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("SecurityUnlockFailed"));
		// Log the status
		Log(LOG_DEV_DATA, "Module Security Unlocked - %s\n", testResult.c_str());
	}
	else
	{	// Timeout waiting for vehicle connected
		Log(LOG_ERRORS, "Timeout waiting for preconditions to be satisfied\n");
		testResult = testAbort;
		testResultCode = GetFaultCode("PreconditionTimeout");
		testDescription = GetFaultDescription("PreconditionTimeout");
	}
	// Send the test result
	SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericSpeedControlTC<VehicleModuleType>::DeterminePreviousPass(void)
{
	BEP_STATUS_TYPE moduleStatus;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Check the module for previous pass condition
	try
	{
		bool isPreviousPass = false;
		moduleStatus = m_vehicleModule.GetInfo("IsPreviousPass", isPreviousPass);
		// Check the status of the module operation
		if(BEP_STATUS_SUCCESS == moduleStatus)
		{	// Store the previous pass status
			Log(LOG_DEV_DATA, "Previous pass status: %s\n", isPreviousPass ? "True" : "False");
			SetPreviousPass(isPreviousPass);
			testResult = testPass;
		}
		else
		{	// Communication failure getting previous pass status
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
			Log(LOG_ERRORS, "Error reading previous pass status from the module - status: %s",
				ConvertStatusToResponse(moduleStatus).c_str());
		}
	}
	catch(ModuleException &excep)
	{
		Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), excep.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Send the test result
	SendTestResultWithDetail(testResult, testDescription, testResultCode,
							 "PreviousPass", IsPreviousPass() ? "True" : "False", "");
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericSpeedControlTC<VehicleModuleType>::UpdateModuleResult(void)
{
	// Status Variables
	string testResult      = testPass;
	string testResultCode  = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	// Log the function entry
	Log(LOG_FN_ENTRY, "%s: ENTER\n", GetTestStepName().c_str());
	// Determine the result to be written to the module
	EEPROM_TEST_RESULT speedControlResult = GetOverallResult() == testPass ? EEPROM_PASS : EEPROM_FAIL;
	// Attempt to update the module
	try
	{	// Check that diagnostic cable is connected
		Log(LOG_DEV_DATA, "%s: Checking/waiting for cable connect\n", GetTestStepName().c_str());
		if(CheckCableConnect())
		{	// Cable Connected, Check if vehicle is OkToShip status
			if(!IsPreviousPass())
			{	// Write the result to the module
				if(!GetParameterBool("UpdateResultSkipSecurityUnlock"))
				{
					Log(LOG_DEV_DATA, "%s: Unlocking Module Security\n", GetTestStepName().c_str());
					moduleStatus = m_vehicleModule.UnlockModuleSecurity();
				}
				else
				{
					Log(LOG_DEV_DATA, "GenericSpeedControlTC: Update Module Result - skipping security unlock by parameter!");
					moduleStatus = BEP_STATUS_SUCCESS;
				}
				if(moduleStatus == BEP_STATUS_SUCCESS)
				{
					Log(LOG_DEV_DATA, "Updating speed control status byte to %s\n",
						speedControlResult == EEPROM_PASS ? testPass.c_str() : testFail.c_str());
					// Write the result to the module
					moduleStatus = m_vehicleModule.UpdateEEPROMTestBit(atoh(GetParameter("SpeedControlStatusByte").c_str()),
																	   atoh(GetParameter("SpeedControlTestResultBit").c_str()),
																	   speedControlResult);

					Log(LOG_DEV_DATA, "%s: Successfully Updated Speed Control Status\n", GetTestStepName().c_str());
					testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				}
				else
				{
					Log(LOG_ERRORS, "%s: Unable to unlock module security\n", GetTestStepName().c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			else
			{	// Previous pass, do not update module
				Log(LOG_DEV_DATA, "Emissions is previous pass, not updating Speed Control results in the module!\n");
				testResult = testSkip;
			}
			// Determine the result of the test
			testResultCode  = (testPass == testResult) || (testResult == testSkip) ? "0000" : GetFaultCode("CommunicationFailure");
			testDescription = 
			(testPass == testResult) || (testResult == testSkip) ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure");
			Log(LOG_DEV_DATA, "%s: Updated test status\n", GetTestStepName().c_str());
		}
		else
		{	// Timeout waiting for cable connect
			Log(LOG_ERRORS, "Timeout waiting for cable connect\n");
			testResult = testTimeout;
			testResultCode = GetFaultCode("CableConnectTimeout");
			testDescription = GetFaultDescription("CableConnectTimeout");
		}
	}
	catch(ModuleException &excep)
	{
		Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), excep.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	Log(LOG_DEV_DATA, "%s: Sending test results\n", GetTestStepName().c_str());
	// Send the test results
	SendTestResult(testResult, testDescription, testResultCode);
	// Return the test status
	Log(LOG_FN_ENTRY, "%s: EXIT: %s\n", GetTestStepName().c_str(), testResult.c_str());
	return testResult;
}

//-----------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericSpeedControlTC<VehicleModuleType>::SpeedControlOperatorResult(void)
{
	string testResult(BEP_TESTING_RESPONSE);
	// Log the entry and determine if the test should be performed
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::SpeedControlOperatorResult() - Enter");
	if(!ShortCircuitTestStep())
	{	// Get the result from the operator
		testResult = OperatorPassFail(GetParameter("OverallResultPrompt"));
		Log(LOG_ERRORS, "Operator speed control result: %s", testResult.c_str());
		// Report the result
		SendTestResult(testResult, GetTestStepInfo("Description"));
	}
	else
	{	// Need to skip this test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping operator result");
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::SpeedControlOperatorResult() - Exit");
	return testResult;
}

//-----------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericSpeedControlTC<VehicleModuleType>::IsVehicleSpeedStable(const float &targetSpeed, bool &zeroThrottle,
																		  float minSpeed, float maxSpeed)
{	// Initialize veriables
	string operatorInput("Unknown");
	bool stableSpeed = false;
	zeroThrottle = false;
	// Set allowable speed range
	float maxSpeedAllowed = (maxSpeed == 0.0) ? targetSpeed + GetParameterFloat("StableSpeedMaxVariation") : maxSpeed;
	float minSpeedAllowed = (minSpeed == 0.0) ? targetSpeed - GetParameterFloat("StableSpeedMinVariation") : minSpeed;
	// Determine if we have module comms or just checking state
	bool throttleInPosition = false;
	if(!GetParameterBool("NoModuleComms"))
	{	//Wait for closed throttle or open throttle
		while(((zeroThrottle = IsZeroThrottle()) != GetParameterBool("CheckForClosedThrottle")) && TimeRemaining() &&
			  (StatusCheck() == BEP_STATUS_SUCCESS)) BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Determine if the throttle is in the correct position
		throttleInPosition = (zeroThrottle == GetParameterBool("CheckForClosedThrottle"));
	}
	else
	{	// No module comms
		zeroThrottle = true;
		throttleInPosition = true;
	}
	// Make sure zero throttle was achieved
	if(throttleInPosition && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && operatorInput.compare(testFail))
	{	// Look for stable speed over specified number of samples
		INT16 stableSpeedCount = 0;
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		float vehicleSpeed = 0.0;
		do
		{	// Read the vehicle speed
			status = ReadVehicleSpeed(vehicleSpeed);
			operatorInput = CheckForOperatorFail();
			// Check the status of the read
			if((status == BEP_STATUS_SUCCESS) && operatorInput.compare(testFail))
			{	// Good read, check speed
				if(m_analyze.CompareData(vehicleSpeed, maxSpeedAllowed, LESS_EQUAL) &&
				   m_analyze.CompareData(vehicleSpeed, minSpeedAllowed, GREATER_EQUAL))
				{	// Speed is in the proper range, Check the throttle
					bool throttleApplied = true;
					if(!GetParameterBool("NoModuleComms"))
					{
						if((zeroThrottle = IsZeroThrottle()) != GetParameterBool("CheckForClosedThrottle"))
						{	// Throttle was messed with during the test, start over
							stableSpeedCount = 0;
							Log(LOG_DEV_DATA, "Throttle applied during stable speed check - restarting\n");
							throttleApplied = true;
						}
						else
						{
							throttleApplied = false;
						}
					}
					else
					{	// NO Module Comms, assume no throttle was applied
						throttleApplied = false;
						zeroThrottle = true;
					}
					// Check if the stable speed count can be updated
					if(!throttleApplied)
					{	// No throttle was applied, increment counter
						stableSpeedCount++;
						Log(LOG_DEV_DATA, "Stable speed point: %02d - Vehicle Speed: %4.1f [%4.1f, %4.1f]\n",
							stableSpeedCount, vehicleSpeed, minSpeedAllowed, maxSpeedAllowed);
					}
				}
				else
				{	// Speed out of range, reset counter
					Log(LOG_DEV_DATA, "Speed out of range, resetting counter - Vehicle Speed: %4.1f [%4.1f, %4.1f]\n",
						vehicleSpeed, minSpeedAllowed, maxSpeedAllowed);
					stableSpeedCount = 0;
				}
				// Provide a slight delay before checking again
				BposSleep(GetParameterInt("StableSpeedSampleDelay"));
			}
			else if(!operatorInput.compare(testFail))
			{
				Log(LOG_ERRORS, "operator pressed fail while waiting for stable speed");
			}
			else
			{	// Commnunication failure reading vehicle speed
				Log(LOG_ERRORS, "Communication failure reading vehicle speed from module - status: %s\n",
					ConvertStatusToResponse(status).c_str());
			}
		} while((stableSpeedCount < GetParameterInt("StableSpeedPoints")) && TimeRemaining() &&
				(BEP_STATUS_SUCCESS == status) && (StatusCheck() == BEP_STATUS_SUCCESS) && operatorInput.compare(testFail));
		// Check if speed was stable long enough
		stableSpeed = m_analyze.CompareData(stableSpeedCount, GetParameterInt("StableSpeedPoints"), GREATER_EQUAL);
	}
	else if(!operatorInput.compare(testFail))
	{
		Log(LOG_ERRORS, "Operator pressed fail button waiting for zero throttle");
	}
	else Log(LOG_ERRORS, "Did not observe zero-throttle\n");
	// Return the result of the test
	return stableSpeed;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericSpeedControlTC<VehicleModuleType>::IsDeltaSpeedAchieved(const float &initialSpeed, const float &deltaSpeed)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string operatorInput("Unknown");
	bool deltaSpeedAchieved = false;
	float vehicleSpeed = 0.0;
	// Keep looking for delta speed
	do
	{	// Read the current vehicle speed
		status = ReadVehicleSpeed(vehicleSpeed);
		operatorInput = CheckForOperatorFail();
		// Check the status of the read
		if(BEP_STATUS_SUCCESS == status)
		{	// Check if the delta has been reached
			deltaSpeedAchieved = m_analyze.CompareData(vehicleSpeed, (initialSpeed - deltaSpeed), LESS_EQUAL);
			if(!deltaSpeedAchieved)	BposSleep(GetParameterInt("DeltaSpeedCheckDelay"));
		}
		else if(!operatorInput.compare(testFail))
		{
			Log(LOG_ERRORS, "Operator pressed the fail button waiting for delta speed achieved");
		}
		else
		{	// Comm failure during read
			Log("Communication failure attempting to read vehicle speed - status: %s\n",
				ConvertStatusToResponse(status).c_str());
		}
	} while(!deltaSpeedAchieved && TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && operatorInput.compare(testFail));
	// Return the results
	Log("Delta Speed Achieved: %s - Vehicle Speed: %4.1, Initial Speed: %4.1f, Delta Required: %4.1f\n",
		deltaSpeedAchieved ? "True" : "False", vehicleSpeed, initialSpeed, deltaSpeed);
	return deltaSpeedAchieved;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericSpeedControlTC<VehicleModuleType>::IsZeroThrottle(void)
{
	bool throttleClosed = false;
	float tps = 0.0;
	// Determine if module should be checked for throttle position
	if(!GetParameterBool("NoModuleComms"))
	{
		// Determine which message to send
		string dataTag = IsETCVehicle() ? "ThrottleClosedVoltageMessage5.7L" : "ThrottleClosedVoltageMessage";
		string closedThrottleReading = IsETCVehicle() ? "ClosedThrottleVoltage5.7L" : "ClosedThrottleVoltage";
		// Attempt to read the throttle position from the module
		try
		{	// Read the throttle position from the module
			BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag(dataTag), tps);
			// Determine if the throttle is closed
			throttleClosed = ((BEP_STATUS_SUCCESS == moduleStatus) &&
							  m_analyze.CompareData(tps, GetParameterFloat(closedThrottleReading), LESS_EQUAL));
		}
		catch(ModuleException &excpt)
		{	// Exception reading data
			Log(LOG_ERRORS, "Module exception in IsZeroThrottle() - %s\n", excpt.GetReason());
			throttleClosed = false;
		}
	}
	else
	{
		throttleClosed = true;
	}
	// Return the status
	return throttleClosed;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericSpeedControlTC<VehicleModuleType>::IsSpeedIncreasing(void)
{
	float previousSpeed = 0.0, currentSpeed = 0.0/*, targetSpeed = 0.0*/;
	string operatorInput("Unknown");
	INT32 speedSample = 0;
	bool speedIncreasing = false, targetSpeedAchieved = false;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Determine the acceptable target speed range
	float targetSpeedMin = GetSpeedControlTargetSpeed() - GetParameterFloat("StableSpeedVariation");
	float targetSpeedMax = GetSpeedControlTargetSpeed() + GetParameterFloat("StableSpeedVariation");
	// Set the initial previous speed
	ReadVehicleSpeed(previousSpeed);
	// Keep monitoring speed until increase is determined
	do
	{	// Read the current vehicle speed
		status = ReadVehicleSpeed(currentSpeed);
		// Check for operator Fail
		operatorInput = CheckForOperatorFail();
		// Check read status
		if((BEP_STATUS_SUCCESS == status) && (IsZeroThrottle() == GetParameterBool("CheckForClosedThrottle")) && operatorInput.compare(testFail))
		{	// Determine if this speed is greater than the previous sample
			if(m_analyze.CompareData(currentSpeed, previousSpeed, GREATER))
			{	// Current speed is greater than previous sample
				speedSample++;					// Increment the sample counter
				Log(LOG_DEV_DATA, "Speed Sample: %02d - Previous Sample: %4.1f, Current Sample: %4.1f, Target: %4.1f\n",
					speedSample, previousSpeed, currentSpeed, targetSpeedMin);
				previousSpeed = currentSpeed;	// Save the speed sample
			}
			else if(((m_analyze.CompareData(currentSpeed, targetSpeedMin, GREATER_EQUAL) &&
					  m_analyze.CompareData(currentSpeed, targetSpeedMax, LESS_EQUAL)) ||
					 m_analyze.CompareData(currentSpeed, targetSpeedMax, GREATER)) && !GetTestStepInfoBool("IgnoreTargets"))
			{	// Target speed achieved
				targetSpeedAchieved = true;
				Log(LOG_DEV_DATA, "Target speed achieved, speed will no longer increase\n");
			}
			else if(m_analyze.CompareData(currentSpeed, previousSpeed, LESS))
			{	// Current speed is less than previous sample, restart counter
				Log(LOG_ERRORS, "Current sample less than previous, restarting sample counter - Current: %4.1f, "
					"Previous: %4.1f\n", currentSpeed, previousSpeed);
				speedSample = 0;
			}
			BposSleep(GetTestStepInfoInt("ScanDelay"));
		}
		else if(BEP_STATUS_SUCCESS != status)
		{	// Communication error reading vehicle speed
			Log(LOG_ERRORS, "Communication failure reading vehicle speed - status: %s\n",
				ConvertStatusToResponse(status).c_str());
		}
		else if(!operatorInput.compare(testFail))
		{
			Log(LOG_ERRORS, "Operator pressed the fail key while waiting for vehicle speed to increase");
			speedIncreasing = false;
		}
		else
		{	// Throttle applied during check
			Log(LOG_ERRORS, "Throttle was applied, restarting check for vehicle speed increasing\n");
			speedSample = 0;
		}
		Log(LOG_DEV_DATA, "speedSample: %d, requiredSamples: %d, targetSpeedAchieved: %s, TimeRemaining: %s, StatusCheck: %s\n",
			speedSample, GetParameterInt("SpeedIncreaseSamples"),
			targetSpeedAchieved ? "True" : "False", TimeRemaining() ? "True" : "False",
			ConvertStatusToResponse(StatusCheck()).c_str());
	} while((speedSample < GetParameterInt("SpeedIncreaseSamples")) && !targetSpeedAchieved &&
			TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && operatorInput.compare(testFail));
	// Check results
	speedIncreasing = (m_analyze.CompareData(speedSample, GetParameterInt("SpeedIncreaseSamples"), GREATER_EQUAL) ||
					   targetSpeedAchieved);
	Log(LOG_FN_ENTRY, "Speed Increasing: %s, Target Speed Achieved: %s\n", speedIncreasing ? "True" : "False",
		targetSpeedAchieved ? "True" : "False");
	// Return the results
	return speedIncreasing;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::ReadVehicleSpeed(float &vehicleSpeed)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Determine how to get the speed, roller speeds or module
	if(GetParameterBool("UseModuleSpeed"))
	{	// Read the vehicle speed from the module
		status = m_vehicleModule.GetInfo("ReadVehicleSpeed", vehicleSpeed);
	}
	else
	{	// Use the drive axle roller speeds
		vehicleSpeed = GetRollSpeed();
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForSpeedControlOn(bool &cruiseOn, string &operatorInput)
{
	BEP_STATUS_TYPE status          = BEP_STATUS_ERROR;
	UINT16          cruiseStatus    = 0x0000;
	bool            promptDisplayed = false;
	operatorInput                   = "Unknown";
	if(!GetParameterBool("NoModuleComms"))
	{	// Wait for the speed control to be on
		do
		{	// Read the switch status from the module
			status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadSpeedControlOn"),cruiseStatus);
			// Determine the status of the read
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, evaluate the status
				cruiseOn = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlOnMask").c_str()), ALL_BITS_SET);
				operatorInput = CheckForOperatorFail();
				// Check if prompts need to be displayed
				if(!cruiseOn && !promptDisplayed && (operatorInput.compare(testFail) != 0))
				{	// Instruct operator to switch on speed control system
					DisplayPrompt(GetPromptBox("SpeedControlOn"), GetPrompt("SpeedControlOn"), GetPromptPriority("SpeedControlOn"));
					promptDisplayed = true;
				}
				else if(!cruiseOn && (operatorInput.compare(testFail) != 0))
				{	// Give a little time before the next check
					BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
				}
			}
			else
			{	// Error reading the cruise status
				Log("Error reading Cruise Mode On - status: %s\n", ConvertStatusToResponse(status).c_str());
				m_testStepStatus.verifyOnSwitch = testFail;
			}
			// Continue checking while time remaining, good system status, good commas and the On state has not been seen
		} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (status == BEP_STATUS_SUCCESS) &&
				!cruiseOn && (operatorInput.compare(testFail) != 0));
		// Remove the prompt if it was displayed
		if(promptDisplayed)	 RemovePrompt(GetPromptBox("SpeedControlOn"), GetPrompt("SpeedControlOn"), GetPromptPriority("SpeedControlOn"));
	}
	else
	{	// No module comms, just prompt the operator to turn it on
		DisplayTimedPrompt(GetPrompt("SpeedControlOn"), GetPromptBox("SpeedControlOn"), 
						   GetPromptPriority("SpeedControlOn"), GetPromptDuration("SpeedControlOn"));
		operatorInput = testPass;
		cruiseOn = true;
		status = BEP_STATUS_SUCCESS;
	}
	// Return the module status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForSpeedControlOff(bool &cruiseOff, string &operatorInput)
{
	BEP_STATUS_TYPE status          = BEP_STATUS_ERROR;
	UINT16          cruiseStatus    = 0xFFFF;
	bool            promptDisplayed = false;
	operatorInput                   = "Unknown";
	// Wait for the speed control to be off
	if(!GetParameterBool("NoModuleComms"))
	{
		do
		{	// Read the switch status from the module
			status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadSpeedControlOff"), cruiseStatus);
			// Determine the status of the read
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, evaluate the status
				cruiseOff = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlOffMask").c_str()), EQUAL);
				operatorInput = CheckForOperatorFail();
				// Check if prompts need to be displayed
				if(!cruiseOff && !promptDisplayed && (operatorInput.compare(testFail) != 0))
				{	// Instruct operator to switch on speed control system
					DisplayPrompt(GetPromptBox("SpeedControlOff"), GetPrompt("SpeedControlOff"), GetPromptPriority("SpeedControlOff"));
					promptDisplayed = true;
				}
				else if(!cruiseOff && (operatorInput.compare(testFail) != 0))
				{	// Give a little time before the next check
					BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
				}
			}
			else
			{	// Error reading the cruise status
				Log("Error reading Cruise Mode Off - status: %s\n", ConvertStatusToResponse(status).c_str());
				m_testStepStatus.verifyOffSwitch = testFail;
			}
			// Continue checking while time remaining, good system status, good comms and the Off state has not been seen
		} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (status == BEP_STATUS_SUCCESS) &&
				!cruiseOff && (operatorInput.compare(testFail) != 0));
		// Remove the prompt if it was displayed
		if(promptDisplayed)	 RemovePrompt(GetPromptBox("SpeedControlOff"), GetPrompt("SpeedControlOff"), GetPromptPriority("SpeedControlOff"));
	}
	else
	{
		DisplayTimedPrompt(GetPrompt("SpeedControlOff"), GetPromptBox("SpeedControlOff"), 
						   GetPromptPriority("SpeedControlOff"), GetPromptDuration("SpeedControlOff"));
		operatorInput = testPass;
		cruiseOff = true;
		status = BEP_STATUS_SUCCESS;
	}
	// Return the module status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForSetButton(bool &cruiseSet, string &operatorInput)
{
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	UINT16 cruiseStatus     = 0x0000;
	operatorInput           = "Unknown";
	// If we have module comms, wait for the operator to press a button
	if(!GetParameterBool("NoModuleComms"))
	{	// Wait for the operator to press the set button
		do
		{	// Look for the SET switch
			status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadSetButton"), cruiseStatus);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, check if SET or operator fail
				cruiseSet = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlSetMask").c_str()), ALL_BITS_SET);
				operatorInput = CheckForOperatorFail();
				if(!cruiseSet && (operatorInput.compare(testFail) != 0))
				{	// Give a little time before the next check
					BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
				}
			}
			else
			{	// Communication error
				Log("Error reading switch status for SET - status: %s\n", ConvertStatusToResponse(status).c_str());
			}
			// Keep checking while time remaining, good system status, good comms and set mode not seen
		} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
				!cruiseSet && (operatorInput.compare(testFail) != 0));
	}
	else if(GetParameterBool("SetButtonPromptForTime"))
	{	// Prompt is already displayed by calling function, just wait for a bit
		status = StatusSleep(GetParameterInt("SpeedControlSetPromptTime"));
		operatorInput = testPass;
	}
	else
	{	// Assume the set button has been pressed if the vehicle speed is stable
		float targetSpeed = (GetParameterFloat("SpeedControlMaxSpeed") + GetParameterFloat("SpeedControlMinSpeed")) / 2.0;
		bool zeroThrottle;
		cruiseSet = IsVehicleSpeedStable(targetSpeed, zeroThrottle);
		operatorInput = testPass;
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForResumeButton(bool &cruiseResume, string &operatorInput)
{
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	UINT16 cruiseStatus     = 0x0000;
	operatorInput           = "Unknown";
	// If using module comms, wait for the resume button
	if(!GetParameterBool("NoModuleComms"))
	{	// Wait for the operator to press the resume button
		do
		{	// Look for the RESUME switch
			status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadResumeButton"),cruiseStatus);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, check if RESUME
				cruiseResume = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlResumeMask").c_str()), ALL_BITS_SET);
				operatorInput = CheckForOperatorFail();
				if(!cruiseResume && (operatorInput.compare(testFail) != 0))
				{	// Give a little time before the next check
					BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
				}
			}
			else
			{	// Communication error
				Log("Error reading switch status for RESUME - status: %s\n", ConvertStatusToResponse(status).c_str());
			}
			// Keep checking while time remaining, good system status, good comms and resume mode not seen
		} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
				!cruiseResume && (operatorInput.compare(testFail) != 0));
	}
	else
	{	// Wait for speed increasing
		float vehicleSpeed = 0.0;
		ReadVehicleSpeed(vehicleSpeed);
		cruiseResume = IsSpeedIncreasing();
		operatorInput = testPass;
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForBrakeTap(bool &brakePressed, string &operatorInput)
{
	UINT16 cruiseStatus     = 0x0000;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	operatorInput           = "Unknown";
	// If we are using module comms, wait for the brake switch, otherwise, look for speed decreasing
	if(!GetParameterBool("NoModuleComms"))
	{	// Wait for the brake pedal to be pressed
		do
		{	// Read the cruise status
			status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadBrakeSwitch"), cruiseStatus);
			// Check the status
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, look for brake switch on
				if(GetParameterBool("SpeedControlBrakeMaskNotAnyBitsSet"))
				{
					brakePressed = !(m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlBrakeMask").c_str()), ANY_BITS_SET));
				}
				else
				{
					brakePressed = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlBrakeMask").c_str()), ALL_BITS_SET);
				}
				operatorInput = CheckForOperatorFail();
				if(!brakePressed && (operatorInput.compare(testFail) != 0))
				{	// Give alittle time before the next check
					BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
				}
			}
			else
			{	// Comm failure of some sort
				Log("Communication failure reading cruise status - status: %s\n", ConvertStatusToResponse(status).c_str());
			}
			// Keep checking while time remaining, good system status, good comms and brake not seen
		} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
				!brakePressed && (operatorInput.compare(testFail) != 0));
	}
	else
	{	// Wait for speed to decrease enough
		float vehicleSpeed = 0.0;
		ReadVehicleSpeed(vehicleSpeed);
		brakePressed = IsDeltaSpeedAchieved(vehicleSpeed, GetParameterFloat("SpeedControlDeltaSpeed"));
		operatorInput = testPass;
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForClutchPedal(bool &clutchPressed, string &operatorInput)
{
	UINT16 cruiseStatus     = 0x0000;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	operatorInput           = "Unknown";
	// Wait for the operator to press the clutch pedal
	do
	{	// Read the cruise status
		status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadClutchPedal"), cruiseStatus);
		// Check the status
		if(BEP_STATUS_SUCCESS == status)
		{	// Good read, look for clutch switch
			clutchPressed = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlClutchMask").c_str()), ALL_BITS_SET);
			operatorInput = CheckForOperatorFail();
			if(!clutchPressed && (operatorInput.compare(testFail) != 0))
			{	// Give a little time before the next check
				BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
			}
		}
		else
		{	// Comm failure of some sort
			Log("Communication failure reading cruise status - status: %s\n", ConvertStatusToResponse(status).c_str());
		}
		// Keep checking while time remaining, good system status, good comms and not clutch pressed
	} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
			!clutchPressed && (operatorInput.compare(testFail) != 0));
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::WaitForCancelSwitch(bool &cancelPressed, string &operatorInput)
{
	UINT16 cruiseStatus     = 0x0000;
	BEP_STATUS_TYPE status  = BEP_STATUS_ERROR;
	operatorInput           = "Unknown";
	// Wait for the operator to press the cancel button
	do
	{	// Read the cruise status
		status = m_vehicleModule.ReadCruiseMode(GetDataTag("ReadCancelSwitch"), cruiseStatus);
		// Check the status
		if(BEP_STATUS_SUCCESS == status)
		{	// Good read, look for cancel switch
			cancelPressed = m_analyze.CompareData(cruiseStatus, (UINT16)atoh(GetParameter("SpeedControlCancelMask").c_str()), ALL_BITS_SET);
			operatorInput = CheckForOperatorFail();
			if(!cancelPressed && (operatorInput.compare(testFail) != 0))
			{	// Give a little time before the next check
				BposSleep(GetParameterInt("SpeedControlModeCheckDelay"));
			}
		}
		else
		{	// Comm failure of some sort
			Log("Communication failure reading cruise status - status: %s\n",
				ConvertStatusToResponse(status).c_str());
		}
		// Keep checking while time remaining, good system status, good comms and cancel not pressed
	} while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && (BEP_STATUS_SUCCESS == status) &&
			!cancelPressed && (operatorInput.compare(testFail) != 0));
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const string GenericSpeedControlTC<VehicleModuleType>::CheckForOperatorFail(void)
{	// Determine how to get the operator input
	string result(BEP_UNAVAILABLE_RESPONSE);
	if(GetParameterBool("UsePlcResultButtons"))
	{
		result = SystemReadBool(GetDataTag("FailButton")) ? testFail : testPass;
	}
	else
	{
		result = SystemRead(PENDANT_KEY_PRESS);
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericSpeedControlTC<VehicleModuleType>::SetupForOperatorInput(void)
{
	string msgResponse(testFail);	// Used to store called methods return status
	// Set the InputServer to TestResult state and clear the keypress
	if(m_ndb->Write(INPUT_SERVER_STATE, INPUT_SERVER_TEST_RESULT_STATE, msgResponse, true) == BEP_STATUS_SUCCESS)
	{	// Clear the Last Pendant Key Press
		if(m_ndb->Write(PENDANT_KEY_PRESS, PENDANT_CLEAR_KEY_PRESS, msgResponse, true) == BEP_STATUS_SUCCESS)
		{
			Log(LOG_DEV_DATA, "InputServer setup for operator input.\n");
		}
		else
		{
			Log(LOG_ERRORS, "System Communication failure clearing pendant key press!\n");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Failure to place the InputServer into TestResult state!\n");
	}
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const BEP_STATUS_TYPE GenericSpeedControlTC<VehicleModuleType>::SetSpeedControlTargetSpeed(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Determine how to get the speed, roller speeds or module
	if(GetParameterBool("UseModuleSpeed"))
	{	// Read the target speed from the module
		status = m_vehicleModule.GetInfo("ReadCruiseTargetSpeed", m_targetSpeed);
	}
	else
	{	// Use the drive axle roller speeds
		m_targetSpeed = GetRollSpeed();
		status = BEP_STATUS_SUCCESS;
	}
	// Return the status
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool &GenericSpeedControlTC<VehicleModuleType>::IsPreviousPass(void)
{
	return m_previousPass;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericSpeedControlTC<VehicleModuleType>::SetPreviousPass(const bool &isPreviousPass)
{
	m_previousPass = isPreviousPass;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericSpeedControlTC<VehicleModuleType>::IsCruiseEquipped(void)
{
	return m_speedControlEquipped;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericSpeedControlTC<VehicleModuleType>::SetCruiseEquipped(const bool &equipped)
{
	Log(LOG_DEV_DATA, "Speed Control Equipped: %s", equipped ? "True" : "False");
	m_speedControlEquipped = equipped;
	if(equipped)
	{
		UpdateTestStatus(TEST_STATUS_IN_PROGRESS);
	}
	else
	{
		UpdateTestStatus(BEP_TEST_NOT_STARTED);
	}
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericSpeedControlTC<VehicleModuleType>::VerifySteadySpeed(void)
{	// Log the entry and determine if this step should be performed
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::VerifySteadySpeed() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{	// Check if the speed is stable
		DisplayPrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"), GetPromptPriority("CheckStableSpeed"));
		bool zeroThrottle = false;
		result = IsVehicleSpeedStable(GetRollSpeed(), zeroThrottle) ? testPass : testFail;
		RemovePrompt(GetPromptBox("CheckStableSpeed"), GetPrompt("CheckStableSpeed"), GetPromptPriority("CheckStableSpeed"));
		Log(LOG_DEV_DATA, "Verify steady speed: %s", result.c_str());
		SendTestResult(result,GetTestStepInfo("Description"), "0000");
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping Verify Steady Speed");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GenericSpeedControlTC::VerifySteadySpeed() - Exit");
	return result;
}
