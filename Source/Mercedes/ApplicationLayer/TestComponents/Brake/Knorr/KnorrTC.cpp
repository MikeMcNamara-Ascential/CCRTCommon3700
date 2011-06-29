//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Knorr/KnorrTC.cpp 2     
//
// FILE DESCRIPTION:
//		ABS test component for the Knorr ABS system.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Brake/Knorr/KnorrTC.cpp $
// 
//
//*************************************************************************
#include "KnorrTC.h"
//-------------------------------------------------------------------------
template <class ModuleType>
KnorrTC<ModuleType>::KnorrTC() : GenericABSTCTemplate<ModuleType>()
{	// Nothing special to do here
}

template <class ModuleType>
KnorrTC<ModuleType>::~KnorrTC()
{	// Nothing special to do here
}

template <class ModuleType>
void KnorrTC<ModuleType>::InitializeHook(const XmlNode *config)
{
	Log(LOG_FN_ENTRY, "Entering KnorrTC::InitializeHook");
	// call parent initialize hook
	GenericABSTCTemplate<ModuleType>::InitializeHook(config);
	try
	{
		m_abs4ChannelTandemFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS4ChannelTandemFiringSequence")->getChildren());
	}
	catch (XmlException &err)
	{
		Log(LOG_ERRORS, "Firing seuquence not specified for 4-channel Tandem system - %s\n", err.GetReason());
	}
	if (GetParameterInt("NumberOfChannels") > 0)
	{
		SetNumberOfChannels(GetParameterInt("NumberOfChannels"));		//DCBR
	}
	else
	{
		SetNumberOfChannels(4);
		Log(LOG_DEV_DATA, "Parameter \"NumberOfChannels\" not found, defaulting to 4 channels.\n");
	}
	// Store tandem ABS test types
	try
	{
		SetTestTandemAbs(false);
		m_testTandemAbsTypes.DeepCopy(config->getChild("Setup/TandemAbsTypes")->getChildren());
		for (XmlNodeMapItr iter = m_testTandemAbsTypes.begin(); iter != m_testTandemAbsTypes.end(); iter++)
		{
			Log(LOG_DEV_DATA, "Config Output: %s; Subscription Output: %s\n", iter->second->getValue().c_str(), SystemRead("Traction").c_str());
			if (iter->second->getValue() == SystemRead("Traction"))
			{
				SetTestTandemAbs(true);
				Log(LOG_DEV_DATA, "This is a Tandem Axle ABS Equipped truck; test it as such.\n");
			}
		}
		// Tandem Park Brake types:
		m_isTandemAxle = false;
		m_testTandemParkBrakeTypes.DeepCopy(config->getChild("Setup/TandemParkBrakeTypes")->getChildren());
		for (XmlNodeMapItr iter = m_testTandemParkBrakeTypes.begin(); iter != m_testTandemParkBrakeTypes.end(); iter++)
		{
			Log(LOG_DEV_DATA, "Config Output: %s; Subscription Output: %s\n", iter->second->getValue().c_str(), SystemRead("Traction").c_str());
			if (iter->second->getValue() == SystemRead("Traction"))
			{
				m_isTandemAxle = true;
				Log(LOG_DEV_DATA, "This is a Tandem Axle Park Brake Equipped truck; test it as such.\n");
			}
		}
	}
	catch (XmlException &excpt)
	{	// Error loading sensor register table
		Log(LOG_ERRORS, "XmlException loading tandem types data: %s\n", excpt.GetReason());
		m_testTandemAbsTypes.clear(true);
		m_testTandemParkBrakeTypes.clear(true);
	}

	Log(LOG_FN_ENTRY, "Exiting KnorrTC::InitializeHook");
}

template <class ModuleType>
const string KnorrTC<ModuleType>::CommandTestStep(const string &value)
{
	string status;
	try
	{	// Get the name of the test step
		string step = GetTestStepName();
		// Log the entry
		Log(LOG_DEV_DATA, "CommandTestStep(%s): Entering test step %s\n", value.c_str(), step.c_str());
		// Ensure it is OK to test
		if (StatusCheck() != BEP_STATUS_SUCCESS)
		{
			status = ConvertStatusToResponse(StatusCheck());
			Log(LOG_ERRORS,  "CommandTestStep: StatusCheck() failed: %s\n", status.c_str());
			SendTestResult(status, GetTestStepInfo("Description"));
		}
		// Initialize the gateway module for comms
		else if (step == "InitializeGateway")			status = InitializeGateway(value);
		// Disable INS communication
		else if (step == "DisableInsCommunication")		status = DisableInsCommunication();
		// Perform the parking brake test
		else if (step == "ParkBrake")					status = TestStepParkBrake(value);
		// Perform the manual parking brake test
		else if (step == "ManualParkBrake")				status = TestStepManualParkBrake(value);
		// Perform the ISO-K Module Wake Up routine
		else if (step == "ModuleWakeUp")				status = ModuleWakeUp();
		// Apply torque to rollers
		else if (step == "ApplyTorqueToRollers")		status = ApplyTorqueToRollers();
		// Set the rolls to zero torque
		else if (step == "SetRollsToZeroTorque")		status = SetRollsToZeroTorque();
		// Perform static ABS test
		else if (step == "StaticAbsTest")				status = TestStepStaticAbs();
		// Perform Retarder Positions test
		else if (step == "RetarderMultipositionalTest")	status = RetarderMultipositionalTest();
		// Perform Retarder Limiter switch test
		else if (step == "RetarderLimiterTest")			status = RetarderLimiterTest();
		// Engage machine
		else if (step == "EngageMachine")				status = EngageMachine();
		// Disengage machine
		else if (step == "DisengageMachine")			status = DisengageMachine();
		// Perform the parking brake test
		else if (step == "AnalyzeParkBrake")			status = GenericTC::AnalyzeParkBrakeTest(value, m_isTandemAxle);
        // Analyze the stopping distance
        else if(step == "AnalyzeStoppingDistance")      status = m_baseBrakeTool->AnalyzeStoppingDistance();
		// No special method, try the base class
		else  status = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
	}
	catch (BepException &err)
	{	// Log the exception and return the error
		Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
		status = BEP_ERROR_RESPONSE;
	}
	// Return the result of the test
	Log(LOG_DEV_DATA, "CommandTestStep(%s) returning %s\n", value.c_str(), status.c_str());
	return status;
}

template <class ModuleType>
string KnorrTC<ModuleType>::ModuleWakeUp(void)
{	// Establish comms with the module
	return BEP_STATUS_SUCCESS == m_vehicleModule.SendModuleWakeUpMessage() ? testPass : testFail;
}

template <class ModuleType>
string KnorrTC<ModuleType>::DisableInsCommunication(void)
{
	return BEP_STATUS_SUCCESS == m_vehicleModule.CommandModule("DisableInsCommunication") ? testPass : testFail;
}

template <class ModuleType>
string KnorrTC<ModuleType>::Reduction(const string &wheel)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Performing reduction for " + wheel;
	INT32 startIndex;
	INT32 stopIndex;
	// Log the function entry
	Log(LOG_FN_ENTRY, "KnorrTC::Redcution(%s) -- Enter\n", wheel.c_str());
	try
	{	// Tag the data array with the start of the reduction sequence
		startIndex = TagArray(wheel + "ReduxStart");
		// Command the module to reduce
		moduleStatus = m_vehicleModule.ActuateReduction(wheel);
		// Check the status of the command
		if (BEP_STATUS_SUCCESS == moduleStatus)
		{	// Delay for the fire time - do not use BposSleep since timing is CRITICAL!
			delay(GetParameterInt(wheel+"ReductionPulseLength"));
			// Determine if valves must be held
			if (GetParameterBool("HoldReduction")) m_vehicleModule.HoldValve(wheel);
			// Set the test result to pass and tag the end of the reduction pulse
			testResult = testPass;
			stopIndex = TagArray(wheel + "ReduxStop");
		}
		else
		{	// Error command module to reduce the wheel
			Log(LOG_ERRORS, "Communication failure performing reduction for %s -- status: %s\n",
				wheel.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch (ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Reduction(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the result of the test
	SendSubtestResult(wheel + "Reduction", testResult, testDescription, testResultCode);
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "KnorrTC::Reduction(%s) -- Exit\n", wheel.c_str());
	return testResult;
}

template <class ModuleType>
string KnorrTC<ModuleType>::Recovery(const string &wheel)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Performing recovery for " + wheel;
	INT32 startIndex;
	INT32 stopIndex;
	// Log the function entry
	Log(LOG_FN_ENTRY, "KnorrTC::Recovery(%s) -- Enter\n", wheel.c_str());
	try
	{	// Tag the data array with the start of the recovery sequence
		startIndex = TagArray(wheel + "RecovStart");
		// Command the module to recover the wheel
		moduleStatus = m_vehicleModule.ActuateRecovery(wheel);
		// check the status of the command
		if (BEP_STATUS_SUCCESS == moduleStatus)
		{	// Delay for the recovery time -- do NOT use BposSleep since timing is CRITICAL!
			delay(GetParameterInt(wheel+"RecoveryPulseLength"));
			// Determine if valves must be held
			if (GetParameterBool("HoldRecovery"))  m_vehicleModule.HoldValve(wheel);
			// Set the test result and tag the end of the recovery period
			testResult = testPass;
			stopIndex = TagArray(wheel + "RecovStop");
		}
		else
		{	// Error commanding the module to recover the wheel
			Log(LOG_ERRORS, "Communication failure performing recovery for %s -- status: %s\n",
				wheel.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch (ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Recovery(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the result of the test
	SendSubtestResult(wheel + "Recovery", testResult, testDescription, testResultCode);
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "KnorrTC::Recovery(%s) -- Exit\n", wheel.c_str());
	return testResult;
}

template <class ModuleType>
const std::string KnorrTC<ModuleType>::TestStepParkBrake(const std::string &value)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	Log(LOG_DEV_DATA, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep() && SystemRead(GetDataTag("ParkBrake")) == GetDataTag("ParkBrakeEquipped"))
	{
		testResult = GenericTC::TestStepParkBrake(value, m_isTandemAxle);
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Test step should be skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class ModuleType>
const std::string KnorrTC<ModuleType>::TestStepManualParkBrake(const std::string &value)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	Log(LOG_DEV_DATA, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep() && SystemRead(GetDataTag("ParkBrake")) == GetDataTag("ManualParkBrakeEquipped"))
	{
		DisplayPrompt(GetPromptBox("TestingManualParkBrake"),GetPrompt("TestingManualParkBrake"),GetPromptPriority("TestingManualParkBrake"));
		DisplayPrompt(GetPromptBox("EngageParkBrake3Notches"),GetPrompt("EngageParkBrake3Notches"),GetPromptPriority("EngageParkBrake3Notches"));
		BposSleep(GetParameterInt("ManualParkBrakePromptDelay"));
		string manualParkBrakeTorque = GetParameter("ManualParkBrakeTorque");

		EngageMachine();

		// Configure rolls for torque mode command
		bool motorCommandSuccessful = false;
		Log(LOG_DEV_DATA,"Commanding rollers 1\n");
		motorCommandSuccessful = (
								 m_motorController.Write("MotorMode","Torque") == 0 &&
								 m_motorController.Write("LeftFrontTorqueValue","0.0") == 0 &&
								 m_motorController.Write("RightFrontTorqueValue","0.0") == 0 &&
								 m_motorController.Write("LeftRearTorqueValue",manualParkBrakeTorque.c_str()) == 0 &&
								 m_motorController.Write("RightRearTorqueValue",manualParkBrakeTorque.c_str()) == 0 &&
								 m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
								 m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0
								 );

		// Prompt operator to adjust park brake until wheels stop turning
		RemovePrompt(GetPromptBox("TestingManualParkBrake"),GetPrompt("TestingManualParkBrake"),GetPromptPriority("TestingManualParkBrake"));
		RemovePrompt(GetPromptBox("EngageParkBrake3Notches"),GetPrompt("EngageParkBrake3Notches"),GetPromptPriority("EngageParkBrake3Notches"));
		DisplayPrompt(GetPromptBox("AdjustManualParkBrake"),GetPrompt("AdjustManualParkBrake"),GetPromptPriority("AdjustManualParkBrake"));
		DisplayPrompt(GetPromptBox("ManualParkBrakePassFail"),GetPrompt("ManualParkBrakePassFail"),GetPromptPriority("ManualParkBrakePassFail"));

		string status = OperatorPassFail(GetPrompt("ManualParkBrakePassFail"),GetParameterFloat("ManualParkBrakePassFail"));
		testResult = status == "Yes" ? testPass : testFail;
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Test step should be skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class ModuleType>
const std::string KnorrTC<ModuleType>::InitializeGateway(const std::string &value)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	Log(LOG_DEV_DATA, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep())
	{
		moduleStatus = m_vehicleModule.InitializeGateway();
		if (BEP_STATUS_SUCCESS == moduleStatus)
		{
			testResult = testPass;
		}
		else
		{
			testResult = testFail;
			testResultCode = GetFaultCode("InitializeGateway");
			testDescription = GetFaultDescription("InitializeGateway");
		}
	}
	else
	{	// Test step should be skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class ModuleType>
bool KnorrTC<ModuleType>::GetTestTandemAbs(void)
{
	return m_testTandemAbs;
}

template <class ModuleType>
void KnorrTC<ModuleType>::SetTestTandemAbs(bool testTandemAbs)
{
	m_testTandemAbs = testTandemAbs;
}

template <class ModuleType>
std::string KnorrTC<ModuleType>::ApplyTorqueToRollers(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	WHEELINFO wheelSpeed;
	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (ShortCircuitTestStep())
	{	// Skip test because of previous pass
		Log(LOG_DEV_DATA,"%s:%s skipped\n", GetComponentName().c_str(), GetTestStepName().c_str());
		testResult = testSkip;
	}
	else
	{
		DisplayPrompt(GetPromptBox("TestingAbs"),GetPrompt("TestingAbs"),GetPromptPriority("TestingAbs"));
		DisplayPrompt(GetPromptBox("ShiftToNeutralMaintainBrake"),GetPrompt("ShiftToNeutralMaintainBrake"),GetPromptPriority("ShiftToNeutralMaintainBrake"));
		BposSleep(GetParameterInt("StaticAbsPromptDelay"));
		try
		{
			string commandWheelTorqueFront = GetParameter("StaticAbsWheelTorqueFront");
			string commandWheelTorqueRear = GetParameter("StaticAbsWheelTorqueRear");
			if (CheckZeroSpeed() == true)
			{
				testResult = EngageMachine();
				if (testResult == testPass)
				{	// Configure rolls for individual roll command
					bool motorCommandSuccessful = false;
					if (GetTestTandemAbs())
					{
						Log(LOG_DEV_DATA,"Commanding rollers 1\n");
						motorCommandSuccessful = (
												 m_motorController.Write("MotorMode","Torque") == 0 &&
												 m_motorController.Write("LeftFrontTorqueValue",commandWheelTorqueFront.c_str()) == 0 &&
												 m_motorController.Write("RightFrontTorqueValue",commandWheelTorqueFront.c_str()) == 0 &&
												 m_motorController.Write("LeftRearTorqueValue","0.0") == 0 &&
												 m_motorController.Write("RightRearTorqueValue","0.0") == 0 &&
												 m_motorController.Write("LeftTandemTorqueValue",commandWheelTorqueRear.c_str()) == 0 &&
												 m_motorController.Write("RightTandemTorqueValue",commandWheelTorqueRear.c_str(),true) == 0
												 );
					}
					else
					{
						Log(LOG_DEV_DATA,"Commanding rollers 2\n");
						motorCommandSuccessful = (
												 m_motorController.Write("MotorMode","Torque") == 0 &&
												 m_motorController.Write("LeftFrontTorqueValue",commandWheelTorqueFront.c_str()) == 0 &&
												 m_motorController.Write("RightFrontTorqueValue",commandWheelTorqueFront.c_str()) == 0 &&
												 m_motorController.Write("LeftRearTorqueValue",commandWheelTorqueRear.c_str()) == 0 &&
												 m_motorController.Write("RightRearTorqueValue",commandWheelTorqueRear.c_str()) == 0 &&
												 m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
												 m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0
												 );
						Log(LOG_DEV_DATA,"Commanding rollers 3a\n");
					}
					Log(LOG_DEV_DATA,"Commanding rollers 3\n");
					if (motorCommandSuccessful)
					{
						Log(LOG_DEV_DATA, "Applying torque to all rollers. Front Torque = %s, Rear Torque = %s\n", commandWheelTorqueFront.c_str(), commandWheelTorqueRear.c_str());
						// Delay for a moment, then verify that all rollers are	met with enough resistance
						// to indicate a vehicle is present (to prevent "runaways")
						BposSleep(GetParameterInt("StaticAbsSafetyDelay"));
						GetWheelSpeeds(wheelSpeed);
						int staticAbsSafetyMaxSpeed = GetParameterInt("StaticAbsSafetyMaxSpeed");
						if (wheelSpeed.lfWheel > staticAbsSafetyMaxSpeed ||
							wheelSpeed.lrWheel > staticAbsSafetyMaxSpeed ||
							wheelSpeed.ltWheel > staticAbsSafetyMaxSpeed ||
							wheelSpeed.rfWheel > staticAbsSafetyMaxSpeed ||
							wheelSpeed.rrWheel > staticAbsSafetyMaxSpeed ||
							wheelSpeed.rtWheel > staticAbsSafetyMaxSpeed)
						{
							// Possibe "runaway", set rolls to torque 0 mode
							Log(LOG_ERRORS, "Possible \"runaway\", setting torque to 0\n");
							SetRollsToZeroTorque();
							testResult = testFail;
						}
						else
						{
							Log(LOG_DEV_DATA, "Torque command successful\n");
							testResult = testPass;
						}
					}
					else
					{
						Log(LOG_DEV_DATA,"Commanding rollers 4\n");
						// Stop driving rolls, set all rolls to torque mode, zero torque
						SetRollsToZeroTorque();

						testResult = BEP_SOFTWAREFAIL_RESPONSE;
						testDescription = GetFaultDescription("SoftwareFailure");
						testResultCode = GetFaultCode("SoftwareFailure");
					}
				}
				else
				{
					Log(LOG_ERRORS, "%s.%s - Failed to Engage machine\n",
						GetComponentName().c_str(), GetTestStepName().c_str());
					testResult = testSoftwareFail;
					testDescription = GetFaultDescription("SoftwareFailure");
					testResultCode = GetFaultCode("SoftwareFailure");
				}
			}
			else
			{
				testResult = testTimeout;
				testDescription = GetFaultDescription("ZeroSpeedFailure");
				testResultCode = GetFaultCode("ZeroSpeedFailure");
			}
		}
		catch (ModuleException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
			testResult = testSoftwareFail;
			testDescription = GetFaultDescription("SoftwareFailure");
			testResultCode = GetFaultCode("SoftwareFailure");
		}
		// Send test results to server
		SendTestResult(testResult, testDescription, testResultCode);
	}
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

template <class ModuleType>
std::string KnorrTC<ModuleType>::SetRollsToZeroTorque(void)
{
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Set all motors to Torque mode. Set all torque values to 0.0
	if (m_motorController.Write("MotorMode","Torque") == 0 &&
		m_motorController.Write("LeftFrontTorqueValue","0.0") == 0 &&
		m_motorController.Write("RightFrontTorqueValue","0.0") == 0 &&
		m_motorController.Write("LeftRearTorqueValue","0.0") == 0 &&
		m_motorController.Write("RightRearTorqueValue","0.0") == 0 &&
		m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
		m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0)
	{
		testResult = testPass;
	}
	else
	{
		testResult = BEP_SOFTWAREFAIL_RESPONSE;
		testDescription = GetFaultDescription("SoftwareFailure");
		testResultCode = GetFaultCode("SoftwareFailure");
	}
	RemovePrompt(GetPromptBox("TestingAbs"),GetPrompt("TestingAbs"),GetPromptPriority("TestingAbs"));
	RemovePrompt(GetPromptBox("ShiftToNeutralMaintainBrake"),GetPrompt("ShiftToNeutralMaintainBrake"),GetPromptPriority("ShiftToNeutralMaintainBrake"));
	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

template <class ModuleType>
const string KnorrTC<ModuleType>::VerifyWheelStopped(string wheel)
{
	float wheelSpeed[GetRollerCount()];
	GetWheelSpeeds(wheelSpeed);
	return(wheelSpeed[GetRollerNumber(wheel)] < GetParameterInt("StaticAbsZeroSpeedTolerance") ? testPass : testFail);
}

template <class ModuleType>
const string KnorrTC<ModuleType>::VerifyWheelSpeed(string wheel)
{
	float wheelSpeed[GetRollerCount()];
	GetWheelSpeeds(wheelSpeed);
	return(wheelSpeed[GetRollerNumber(wheel)] > GetParameterInt("StaticAbsWheelSpeedMinimum") ? testPass : testFail);
}

template <class ModuleType>
const std::string KnorrTC<ModuleType>::TestStepStaticAbs()
{
	string testResult = testPass;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	Log(LOG_DEV_DATA, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep())
	{
		string reductionResult = testFail;
		string recoveryResult = testFail;
		string initialConditionsResult = testFail;
		string faultTag;

		XmlNodeMap * valveFiringSequence;
		if (GetTestTandemAbs())
		{
			valveFiringSequence = &m_abs4ChannelTandemFiringOrder;
		}
		else
		{
			valveFiringSequence = &m_abs4ChannelFiringOrder;
		}
		for (XmlNodeMapItr iter = valveFiringSequence->begin(); iter != valveFiringSequence->end(); iter++)
		{
			initialConditionsResult = VerifyWheelStopped(iter->second->getValue());
			if (initialConditionsResult == testPass)
			{
				Log(LOG_DEV_DATA, "Begin reduction for %s\n", iter->second->getValue().c_str());
				reductionResult = Reduction(iter->second->getValue());
				if (reductionResult == testPass)
				{
					reductionResult = VerifyWheelSpeed(iter->second->getValue());
					if (reductionResult != testPass)
					{
						testResult = testFail;
					}
				}
				else
				{
					testResult = testFail;
				}
				Log(LOG_DEV_DATA, "Begin recovery for %s\n", iter->second->getValue().c_str());
				recoveryResult = Recovery(iter->second->getValue());
				if (recoveryResult == testPass)
				{
					recoveryResult = VerifyWheelStopped(iter->second->getValue());
					if (recoveryResult != testPass)
					{
						testResult = testFail;
					}
				}
				else
				{
					testResult = testFail;
				}

				faultTag = iter->second->getValue()+"ReductionFail";
				testResultCode = (reductionResult == testPass ? "0000" : GetFaultCode(faultTag));
				testDescription = (reductionResult == testPass ? iter->second->getValue()+" Reduction Test" : GetFaultDescription(faultTag));
				SendSubtestResult(iter->second->getValue() + "ABSReduction", reductionResult, testDescription, testResultCode);

				faultTag = iter->second->getValue()+"RecoveryFail";
				testResultCode = (recoveryResult == testPass ? "0000" : GetFaultCode(faultTag));
				testDescription = (recoveryResult == testPass ? iter->second->getValue()+" Recovery Test" : GetFaultDescription(faultTag));
				SendSubtestResult(iter->second->getValue() + "ABSRecovery", recoveryResult, testDescription, testResultCode);
			}
			else
			{
				Log(LOG_DEV_DATA, "TestStepStaticAbs(), Initial conditions not correct (rollers speed over minimum)\n");
				testResult = testFail;
				testResultCode = GetFaultCode(iter->second->getValue() + "ABSInitialConditionsFail");
				testDescription = GetFaultDescription(iter->second->getValue() + "ABSInitialConditionsFail");
				SendSubtestResult(iter->second->getValue() + "ABSInitialConditions", initialConditionsResult, GetFaultCode(faultTag), GetFaultDescription(faultTag));
			}
		}
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Test step should be skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class ModuleType>
const int KnorrTC<ModuleType>::GetRollerNumber(string rollerName)
{
	int rollerNumber =0;
	if (rollerName == "LeftFront")
		rollerNumber = 0;
	else if (rollerName == "RightFront")
		rollerNumber = 1;
	else if (rollerName == "LeftRear")
		rollerNumber = 2;
	else if (rollerName == "RightRear")
		rollerNumber = 3;
	else if (rollerName == "LeftTandem")
		rollerNumber = 4;
	else if (rollerName == "RightTandem")
		rollerNumber = 5;

	return rollerNumber;
}

template <class ModuleType>
string KnorrTC<ModuleType>::SensorTest(void)
{
	return testPass;
}

template <class ModuleType>
string KnorrTC<ModuleType>::DumpAndIsolateAllWheels(void)
{
	return testPass;
}

template <class ModuleType>
const string KnorrTC<ModuleType>::RetarderMultipositionalTest(void)
{
	string testResult       = testPass;
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	bool tandemMode, checkForces, alwaysCheckAllPositions;
	int numberOfSwitchPositions, retarderScanDelay;
	string eraseSpeedRange = "0 0";
	float* averageRetarderForce;
	char* forceList;

	Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest() - Enter\n");
	try
	{
		numberOfSwitchPositions = GetParameterInt("RetarderNumberOfSwitchPositions");
		retarderScanDelay = GetParameterInt("RetarderScanDelay");
		tandemMode = GetParameterBool("TandemMode");
		checkForces = GetParameterBool("CheckRetarderForces");
		alwaysCheckAllPositions = GetParameterBool("AlwaysCheckAllPositions");
	}
	catch (XmlException &excpt)
	{
		Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest() - XmlException reading parameters - DEFAULTING!\n");
		numberOfSwitchPositions = 3;
		retarderScanDelay = 100;
		tandemMode = true;
		checkForces = false;
		alwaysCheckAllPositions = true;
	}

	averageRetarderForce = new float[numberOfSwitchPositions];
	forceList = new char[5*numberOfSwitchPositions];
	for (int currentPosition=1; currentPosition<=numberOfSwitchPositions; currentPosition++)
	{
		if ((testResult == testPass) || alwaysCheckAllPositions)
		{
			if ((SinglePosition(currentPosition, averageRetarderForce) != testPass) || (testResult != testPass))
			{
				testResult = testFail;
			}
			if (checkForces)
			{
				Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest(%d) - Checking retarder forces...\n", currentPosition);
				if ((currentPosition-2) > 0)
				// If this is not the first reading, we will have something to compare with
				{
					Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest() - ForcePos #%d=%f, ForcePos #%d=%f\n", 
						currentPosition, averageRetarderForce[currentPosition-1], 
						currentPosition-1, averageRetarderForce[currentPosition-2]);
					if (!(averageRetarderForce[currentPosition-1] >= averageRetarderForce[currentPosition-2]))
					// If the last position was NOT more powerful than the one before it...
					{
						Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest(%d) - Retarder force failure\n", currentPosition);
						testResult = testFail;
						testResultCode = GetFaultCode("MultipositionalForceFailure");
						testDescription = GetFaultDescription("MultipositionalForceFailure");
					}
				}

			}
			else
			{
				Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest(%d) - Not checking retarder forces\n", currentPosition);
			}   
		}
		else
		{
			Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest(%d) - RetarderPositionTest failure\n", currentPosition);
			testResult = testFail;
			testResultCode = GetFaultCode("MultipositionalFailure");
			testDescription = GetFaultDescription("MultipositionalFailure");
		}
	}
	SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
	SendTestResult(testResult, testDescription, testResultCode);
	Log(LOG_DEV_DATA, "KnorrTC::RetarderMultipositionalTest() - Exit\n");
	return(testResult);
}


template <class ModuleType>
const string KnorrTC<ModuleType>::SinglePosition(int currentPosition, float *averageRetarderForce)
{
	string testResult       = testFail;
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	bool tandemMode, checkForces;
	float engageSpeedTarget, measureArmSpeedTarget, measureDisarmSpeedTarget, disengageSpeedTarget;
	int retarderScanDelay, forceCountLimit;
	string engageSpeedRange, disengageSpeedRange;
	string eraseSpeedRange = "0 0";
	BEP_STATUS_TYPE status;
	float wheelSpeed[6], **wheelForce;
	char posCharArray[10];
	sprintf(posCharArray, "%d ", currentPosition);
	string posString (posCharArray);
	averageRetarderForce[currentPosition-1] = 0;

	Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Enter\n", currentPosition);
	try
	{
		engageSpeedTarget = GetParameterFloat("RetarderEngageSpeedTarget");
		measureArmSpeedTarget = GetParameterFloat("RetarderMeasureArmSpeedTarget");
		measureDisarmSpeedTarget = GetParameterFloat("RetarderMeasureDisarmSpeedTarget");
		disengageSpeedTarget = GetParameterFloat("RetarderDisengageSpeedTarget");
		retarderScanDelay = GetParameterInt("RetarderScanDelay");
		forceCountLimit = GetParameterInt("ForceCountLimit");
		engageSpeedRange = GetParameter("RetarderEngageSpeedRange");
		disengageSpeedRange = GetParameter("RetarderDisengageSpeedRange");
		tandemMode = GetParameterBool("TandemMode");
		checkForces = GetParameterBool("CheckRetarderForces");
	}
	catch (XmlException &excpt)
	{
		Log(LOG_DEV_DATA, "KnorrTC::SinglePosition() - XmlException reading parameters - DEFAULTING!\n");
		engageSpeedTarget = 40.0;
		engageSpeedRange = "40 45";
		measureArmSpeedTarget = 30.0;
		measureDisarmSpeedTarget = 20.0;
		disengageSpeedTarget = 10.0;
		disengageSpeedRange = "0 10";
		retarderScanDelay = 100;
		forceCountLimit = 5;
		tandemMode = true;
		checkForces = false;
	}

	testResult = AccelerateToTestSpeed(engageSpeedTarget, engageSpeedRange, retarderScanDelay);
	if (testResult == testPass)
	// Engage the retarder
	{
		//DisplayPrompt(1, GetParameter("EngageRetarder"), 0);
		//DisplayPrompt(2, GetParameter("PositionNumber"), 0, "", posString);
		DisplayPrompt(GetPromptBox("EngageRetarder"),GetPrompt("EngageRetarder"),GetPromptPriority("EngageRetarder"));
		DisplayPrompt(GetPromptBox("PositionNumber"),GetPrompt("PositionNumber"),GetPromptPriority("PositionNumber"),"white",posString.c_str());
		SystemWrite(GetDataTag("SpeedTarget"), disengageSpeedRange);
		if (checkForces)
		// If we want to analyze the retarder forces
		{
			wheelForce = new float *[forceCountLimit];
			int forceCount = 0;
			Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - forceCount=%d, forceCountLimit=%d", currentPosition,
				forceCount, forceCountLimit);
			do
			{
				m_baseBrakeTool->GetISpeeds(wheelSpeed);
				Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - WheelSpeed[2]=%f, WheelSpeed[3]=%f", currentPosition,
					wheelSpeed[2], wheelSpeed[3]);
				if (measureArmSpeedTarget > ((wheelSpeed[2] + wheelSpeed[3])/2))
				// If the average rear wheel speeds have dropped lower than the target
				{
					if (forceCount < forceCountLimit)
					// If we still need to accumulate force data...
					{
						wheelForce[forceCount] = new float [6];
						m_baseBrakeTool->GetIForces(wheelForce[forceCount]);  // Get a full "snapshot"
						if (GetTestTandemAbs())
						// If we need to deal with the tandem axle as well
						{
							wheelForce[forceCount][0] = (wheelForce[forceCount][2] + wheelForce[forceCount][3] + 
														 wheelForce[forceCount][4] + wheelForce[forceCount][5]) / 4;  // average
							Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - 4 - Average Force = %f", currentPosition, wheelForce[forceCount][0]);
						}
						else
						// If we do not need to deal with the tandem axle
						{
							wheelForce[forceCount][0] = (wheelForce[forceCount][2] + wheelForce[forceCount][3]) / 2;  // average
							Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - 2 - Average Force = %f", currentPosition, wheelForce[forceCount][0]);
						}
						forceCount++;
					}
				}
				BposSleep(50);
			}
			// While we are not below the deceleration speed limit, and we are still gathering counts
			while ((measureDisarmSpeedTarget < ((wheelSpeed[2] + wheelSpeed[3])/2)) && (forceCount < forceCountLimit));
			for (forceCount = 0; forceCount < forceCountLimit; forceCount++)
			{
				averageRetarderForce[currentPosition-1] = wheelForce[forceCount][0] + averageRetarderForce[currentPosition-1];
				Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - count=%d, force=%f, curForce=%f", currentPosition, forceCount, wheelForce[forceCount][0],averageRetarderForce[currentPosition-1]);
			}
			averageRetarderForce[currentPosition-1] /= forceCountLimit;
			Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Final Force = %f", currentPosition, averageRetarderForce[currentPosition-1]); 
		}
		else
		{
			Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Not checking retarder forces\n", currentPosition);
			testResult = testPass;
		}
		status = WaitForTargetSpeed(disengageSpeedTarget, DOWN, retarderScanDelay);
		RemovePrompt(GetPromptBox("EngageRetarder"),GetPrompt("EngageRetarder"),GetPromptPriority("EngageRetarder"));
		RemovePrompt(GetPromptBox("PositionNumber"),GetPrompt("PositionNumber"),GetPromptPriority("PositionNumber"));
		if (status != BEP_STATUS_SUCCESS)
		{
			Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Error decelerating to engagement speed\n", currentPosition);
			testResult = testFail;          
			testResultCode = GetFaultCode("DecelFailure");
			testDescription = GetFaultDescription("DecelFailure");    
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Error accelerating to engagement speed\n", currentPosition);
		testResult = testFail;          
		testResultCode = GetFaultCode("AccelFailure");
		testDescription = GetFaultDescription("AccelFailure");    
	}

	Log(LOG_DEV_DATA, "KnorrTC::SinglePosition(%d) - Exit\n", currentPosition);
	return testResult;
}

template <class ModuleType>                    
const string KnorrTC<ModuleType>::RetarderLimiterTest(void)
{
	string testResult       = testFail;
	string testResultCode   = "0000";
	string testDescription  = GetTestStepInfo("Description");
	bool tandemMode;
	float limiterSpeedLimit, limiterForce, limiterEngageSpeedTarget;
	int limiterPromptDelay, limiterTestDuration, retarderScanDelay, percentIncrease;
	string limiterEngageRange, status1, status2;
	string eraseSpeedRange = "0 0"; 
	float wheelSpeed[GetRollerCount()];

	Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Enter\n");
	try
	{
		limiterSpeedLimit = GetParameterFloat("RetarderLimiterSpeedLimit");
		limiterForce = GetParameterFloat("RetarderLimiterForce");
		limiterPromptDelay = GetParameterInt("RetarderLimiterPromptDelay");
		limiterTestDuration = GetParameterInt("RetarderLimiterTestDuration");
		retarderScanDelay = GetParameterInt("RetarderScanDelay");
		limiterEngageRange = GetParameter("RetarderLimiterEngageRange");
		limiterEngageSpeedTarget = GetParameterFloat("RetarderLimiterEngageSpeedTarget");
		tandemMode = GetParameterBool("TandemMode");
		percentIncrease = GetParameterInt("PercentIncrease");
	}
	catch (XmlException &excpt)
	{
		Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - XmlException reading parameters - DEFAULTING!\n");
		limiterEngageRange = "35 40";
		limiterEngageSpeedTarget = 35;
		limiterPromptDelay = 5000;
		limiterSpeedLimit = 45.0;
		limiterTestDuration = 4;
		limiterForce = -50.0;
		retarderScanDelay = 100;
		tandemMode = true;
		percentIncrease = 10;
	}
	char limiterCharArray[10];
	sprintf(limiterCharArray, "%d ", (int)(limiterEngageSpeedTarget * 1.609));
	string limiterSettingString (limiterCharArray);
	DisplayPrompt(GetPromptBox("EngageRetarderLimiter"),GetPrompt("EngageRetarderLimiter"),
				  GetPromptPriority("EngageRetarderLimiter"),"white",limiterSettingString.c_str());
	SystemWrite(GetDataTag("TorqueConstant"), (float)0.0);
	SystemWrite(GetDataTag("TorqueConstantEnable"), true);
	status1 = OperatorPassFail(GetPrompt("LimiterSettingComplete"),GetParameterFloat("LimiterSettingComplete"));
	RemovePrompt(GetPromptBox("EngageRetarderLimiter"),GetPrompt("EngageRetarderLimiter"),
				 GetPromptPriority("EngageRetarderLimiter"));
	if (status1 == "Pass")
	// Operator said that it is engaged
	{
		Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Limiter setting successful\n");
		testResult = AccelerateToTestSpeed(limiterEngageSpeedTarget-2, limiterEngageRange, retarderScanDelay);
		if (testResult == testPass)
		{	// hold steady speed
			DisplayPrompt(GetPromptBox("TestingLimiter"), GetPrompt("TestingLimiter"), GetPromptPriority("TestingLimiter"));
			// SLOWLY increase motor loading
			for (int iterationCount=0; (iterationCount*percentIncrease)<=100; iterationCount++)
			{
				Log(LOG_DEV_DATA, "New \"Torque Constant\" System Tag = %f",
					((iterationCount * percentIncrease * limiterForce)/100));
				SystemWrite(GetDataTag("TorqueConstant"), ((iterationCount * percentIncrease * limiterForce) / 100));
				GetWheelSpeeds(wheelSpeed);
				if ((wheelSpeed[2]>limiterSpeedLimit || wheelSpeed[3]>limiterSpeedLimit) ||
					(tandemMode && (wheelSpeed[4]>limiterSpeedLimit || wheelSpeed[5]>limiterSpeedLimit)))
				{
					SystemWrite(GetDataTag("TorqueConstant"), 0);
					limiterForce = SystemReadFloat(GetDataTag("TorqueConstant"));
					Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Boost loop ended; force=%f", limiterForce);
					testResult = testFail;          
					testResultCode = GetFaultCode("BoostSpeedLimit");
					testDescription = GetFaultDescription("BoostSpeedLimit");
					break;
				}
				BposSleep(1000);
			}
			for (int testTime=0; testTime<limiterTestDuration; testTime++)
			{
				limiterForce = SystemReadFloat(GetDataTag("TorqueConstant"));
				Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Test loop iteration %d; force=%f", testTime, limiterForce);
				GetWheelSpeeds(wheelSpeed);
				if ((wheelSpeed[2]>limiterSpeedLimit || wheelSpeed[3]>limiterSpeedLimit) ||
					(tandemMode && (wheelSpeed[4]>limiterSpeedLimit || wheelSpeed[5]>limiterSpeedLimit)))
				{
					SystemWrite(GetDataTag("TorqueConstant"), 0);
					limiterForce = SystemReadFloat(GetDataTag("TorqueConstant"));
					Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Test loop ended; force=%f", limiterForce);
					testResult = testFail;          
					testResultCode = GetFaultCode("TestSpeedLimit");
					testDescription = GetFaultDescription("TestSpeedLimit");
					break;
				}
				if (testTime == limiterTestDuration-1)
				{
					Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Last test iteration check & disable");                 
					SystemWrite(GetDataTag("TorqueConstant"), 0);
				}
				BposSleep(1000);
			}
			RemovePrompt(GetPromptBox("TestingLimiter"), GetPrompt("TestingLimiter"), GetPromptPriority("TestingLimiter"));
		}
		else
		{
			Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Accelerate to limiter speed failed\n");
			testResult = testFail;          
			testResultCode = GetFaultCode("LimiterAccelFailure");
			testDescription = GetFaultDescription("LimiterAccelFailure");
		}
	}
	else
	{
		Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Limiter setting failed by operator\n");
		testResult = testFail;          
		testResultCode = GetFaultCode("LimiterSetFailure");
		testDescription = GetFaultDescription("LimiterSetFailure");
	}   
	SystemWrite(GetDataTag("TorqueConstantEnable"), false);
	SystemWrite(GetDataTag("SpeedTarget"), eraseSpeedRange);
	CheckZeroSpeed();
	//Prompt operator to input pass/fail when test is complete.
	status2 = OperatorPassFail(GetPrompt("RetarderLimiterPassFail"),GetParameterFloat("RetarderLimiterTestTime"));
	if ((testResult == testPass) && (status2 != "Pass"))
	{
		testResult = testFail;          
		testResultCode = GetFaultCode("LimiterOperatorFailure");
		testDescription = GetFaultDescription("LimiterOperatorFailure");
	}
	SendTestResultWithDetail(testResult, testDescription, testResultCode,
							 "Operator Limiter Set", (status1 == "Yes" ? "Set" : "Not set"), "Button Input",
							 "Operator Limiter P/F", (status2 == "Yes" ? "Passed" : "Failed"), "Button Input");
	Log(LOG_DEV_DATA, "KnorrTC::RetarderLimiterTest() - Exit\n");
	return testResult;
}

template <class ModuleType>                    
void KnorrTC<ModuleType>::Abort(void)
{
	GenericTC::Abort();
	Log(LOG_DEV_DATA, "Knorr brake test aborting!\n");
}








