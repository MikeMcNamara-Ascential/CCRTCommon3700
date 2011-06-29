//*************************************************************************
// FILE:
//
// FILE DESCRIPTION:
//  Tandem Locking Differential Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
// 
// 
//*************************************************************************
#include "TandemLockingDifferentialTC.h"
//*************************************************************************

TandemLockingDifferentialTC::TandemLockingDifferentialTC() : 
GenericTC(), m_isRearAndTandemLongEquipped(false), m_isFrontAndRearLongEquipped(false),
m_isLongitudinalLockingUndefined(false), m_isRearTransEquipped(false), m_isRearAndTandemTransEquipped(false),
m_isFrontAndRearTransEquipped(false), m_isTransversalLockingUndefined(false)
{	// Nothing to do here.
}

TandemLockingDifferentialTC::~TandemLockingDifferentialTC()
{
}

void TandemLockingDifferentialTC::Initialize(const string &fileName)
{	// call the base class to perfrom the initialization
	GenericTC::Initialize(fileName);
}

const string TandemLockingDifferentialTC::Register(void)
{
	if (IsLockingDifferentialEquipped())
	{
		Log(LOG_DEV_DATA, "Locking differential equipped, starting test\n");
		UpdateTestStatus(TEST_STATUS_IN_PROGRESS);
	}
	else
	{
		Log(LOG_DEV_DATA, "Locking differential not equipped, skipping test\n");
		UpdateTestStatus(BEP_TEST_NOT_STARTED);
	}
	return BepServer::Register();
}

void TandemLockingDifferentialTC::Initialize(const XmlNode *config) 
{
	try
	{

		//Iniitalize base class
		GenericTC::Initialize(config);

		Log(LOG_DEV_DATA, "Finished GenericTC::Initialize(config)\n");

		//Set Longitudinal Locking Type
		if (GetDataTag("TagRearAndTandem") == SystemRead(GetDataTag("AxleLongLockTag")))
		{
			SetRearAndTandemLongEquipped(true);
		}
		else if (GetDataTag("TagFrontAndRear") == SystemRead(GetDataTag("AxleLongLockTag")))
		{
			SetFrontAndRearLongEquipped(true);
		}
		else
		{//Error valid axle type not specified
			Log(LOG_DEV_DATA, "Longitudinal Lock Type set to undefined: SystemRead(GetDataTag(AxleLongLockTag) = %s,\n", SystemRead(GetDataTag("AxleLongLockTag")).c_str());
			Log(LOG_DEV_DATA, "GetDataTag(TagRearAndTandem) = %s, GetDataTag(TagFrontAndRear) = %s\n", GetDataTag("TagRearAndTandem").c_str(), GetDataTag("TagFrontAndRear").c_str()); 
			SetLongitudinalLockingUndefined(true);
		}

		//Set Transversal Locking Type
		if (GetDataTag("TagRear") == SystemRead(GetDataTag("AxleTransLockTag")))
		{
			SetRearTransEquipped(true);
		}
		else if (GetDataTag("TagRearAndTandem") == SystemRead(GetDataTag("AxleTransLockTag")))
		{
			SetRearAndTandemTransEquipped(true);
		}
		else if (GetDataTag("TagFrontAndRear") == SystemRead(GetDataTag("AxleTransLockTag")))
		{
			SetFrontAndRearTransEquipped(true);
		}
		else
		{//Error valid axle type not specified
			Log(LOG_DEV_DATA, "Transversal Lock Type set to undefined: SystemRead(GetDataTag(AxleTransLockTag) = %s, GetDataTag(TagRear) = %s,\n", SystemRead(GetDataTag("AxleTransLockTag")).c_str(), GetDataTag("TagRear").c_str());
			Log(LOG_DEV_DATA, "GetDataTag(TagRearAndTandem) = %s, GetDataTag(TagFrontAndRear) = %s\n", GetDataTag("TagRearAndTandem").c_str(), GetDataTag("TagFrontAndRear").c_str()); 
			SetTransversalLockingUndefined(true);
		}

		// Call the initialize complete method
		Log(LOG_DEV_DATA, (GetComponentName() + ": calling InitializationComplete()").c_str());
		InitializationComplete();

		// Reset the data tags
		ResetSystemTags();  

		Log(LOG_FN_ENTRY, (GetComponentName() + ": Initialize: EXIT").c_str());
	}
	catch (XmlException &e)
	{
		Log(LOG_ERRORS, "XML Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
	catch (BepException &e)
	{
		Log(LOG_ERRORS, "Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
}

const string TandemLockingDifferentialTC::CommandTestStep(const string &value)
{
	string testResult = testFail;	// used to report test step result
	// Determine if the next test step can be sequenced
	if (StatusCheck() == BEP_STATUS_SUCCESS)
	{	// System is fine to continue testing
		try
		{	// Get the next test step to perform
			Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());

			if (IsLockingDifferentialEquipped())
			{
			// Test the transversal axle lock
			if (GetTestStepName() == "TransversalLockTest")							testResult = TransversalLockTest();
			// Test the longitudinal axle lock
			else if (GetTestStepName() == "LongitudinalLockTest")					testResult = LongitudinalLockTest();
			// Prompt or prompt and verify shift to axle to no lock
			else if (GetTestStepName() == "ShiftToNoLock")							testResult = DisplayPromptOptionalVerify("ShiftToNoLock");
			// Prompt or prompt and verify shift to axle to no lock
			else if (GetTestStepName() == "ShiftToNeutral")							testResult = DisplayPromptOptionalVerify("ShiftToNeutral");
			// Stop driving rolls, set all rolls to torque mode, zero torque
			else if (GetTestStepName() == "SetRollsToTorqueMode")					testResult = SetRollsToTorqueMode();
			// Stop driving rolls, set all rolls to torque mode, zero torque
			else if (GetTestStepName() == "SetRollsToBoostMode")					testResult = SetRollsToBoostMode();
			// Insert a delay in the test
			else if (GetTestStepName() == "Delay")									testResult = Delay();
			// Prompt driver to brake to stop
			else if (GetTestStepName() == "BrakeToStop")							testResult = (CheckZeroSpeed() ? testPass : testFail);
			// No special method available, try the base class// Finish up any needed testing
			else if (GetTestStepName() == "FinishUp")
			{
				testResult = FinishUp();
				SendOverallResult(testResult);
			}
			else   testResult = GenericTC::CommandTestStep(value);

			}
			else
			{
				// Axle lock not equipped, skipping all test steps
				Log(LOG_DEV_DATA, "Vehicle is not equipped with a locking differential, skipping test step: %s", GetTestStepName().c_str());
				testResult = testSkip;
			}
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

string TandemLockingDifferentialTC::DriveRollsTransversalLock(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	WHEELINFO wheelSpeed;
	bool checkRTSpeed = false;
	bool checkRFSpeed = false;
	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{
		string commandWheelSpeed = GetParameter("TransversalLockWheelSpeed");
		string commandWheelTorque = GetParameter("TransversalLockWheelTorque");
		Log(LOG_DEV_DATA, "TransversalLockWheelSpeed: %s, commandWheelSpeed: %s\n", GetParameter("TransversalLockWheelSpeed").c_str(), commandWheelSpeed.c_str());
		// Running this test at high speeds may cause damage to the differential
		if (atof(commandWheelSpeed.c_str()) > 10.0)
		{
			Log(LOG_ERRORS, "ERROR: Running this test at high speeds may cause damage to the differential: Defaulting to 2.0 Mph\n");
			commandWheelSpeed = "2.0";
		}
		testResult = EngageMachine();
		if (testResult == testPass)
		{// Display prompt to notify driver machine will be driving rolls   
			DisplayPrompt(GetPromptBox("DrivingRolls"), GetPrompt("DrivingRolls"), GetPromptPriority("DrivingRolls"));
			// Configure rolls for individual roll command
			//All configurations require left side Torque and RR speed / LR Torque values
			if (m_motorController.Write("LeftFrontMotorMode","Torque") == 0 &&
				m_motorController.Write("LeftRearMotorMode","Torque") == 0 &&
				m_motorController.Write("RightRearMotorMode","Speed") == 0 &&
				m_motorController.Write("LeftTandemMotorMode","Torque") == 0 &&
				m_motorController.Write("LeftRearTorqueValue",commandWheelTorque.c_str()) == 0 &&
				m_motorController.Write("RightRearSpeedValue",commandWheelSpeed.c_str()) == 0)
			{
				if (IsRearAndTandemTransEquipped())
				{//drive RT roller
					if (m_motorController.Write("RightFrontMotorMode","Torque") == 0 &&
						m_motorController.Write("RightTandemMotorMode","Speed") == 0 &&
						m_motorController.Write("RightFrontTorqueValue","0.0") == 0 &&
						m_motorController.Write("LeftFrontTorqueValue","0.0") == 0 &&
						m_motorController.Write("LeftTandemTorqueValue",commandWheelTorque.c_str()) == 0 &&
						m_motorController.Write("RightTandemSpeedValue",commandWheelSpeed.c_str(),true) == 0)
					{
						checkRTSpeed = true;
						testResult = testPass;
					}
					else
					{	// Stop driving rolls, set all rolls to torque mode, zero torque
						SetRollsToTorqueMode();
						Log(LOG_ERRORS, "Unable to set machine to drive RT roll\n");
						testResult = BEP_SOFTWAREFAIL_RESPONSE;
						testDescription = GetFaultDescription("SoftwareFailure");
						testResultCode = GetFaultCode("SoftwareFailure");
					}
				}
				else if (IsFrontAndRearTransEquipped())
				{//drive RF roller
					if (m_motorController.Write("RightFrontMotorMode","Speed") == 0 &&
						m_motorController.Write("RightTandemMotorMode","Torque") == 0 &&
						m_motorController.Write("RightFrontSpeedValue",commandWheelSpeed.c_str()) == 0 &&
						m_motorController.Write("LeftFrontTorqueValue",commandWheelTorque.c_str()) == 0 &&
						m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
						m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0)
					{
						checkRFSpeed = true;
						testResult = testPass;
					}
					else
					{	// Stop driving rolls, set all rolls to torque mode, zero torque
						SetRollsToTorqueMode();
						Log(LOG_ERRORS, "Unable to set machine to drive RF roll\n");
						testResult = BEP_SOFTWAREFAIL_RESPONSE;
						testDescription = GetFaultDescription("SoftwareFailure");
						testResultCode = GetFaultCode("SoftwareFailure");
					}
				}
				else if (IsRearTransEquipped())
				{//Only rear Transversal lock equipped set RT / RF to torque
					if (m_motorController.Write("RightFrontMotorMode","Torque") == 0 &&
						m_motorController.Write("RightTandemMotorMode","Torque") == 0 &&
						m_motorController.Write("RightFrontTorqueValue","0.0") == 0 &&
						m_motorController.Write("LeftFrontTorqueValue","0.0") == 0 &&
						m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
						m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0)
					{
						testResult = testPass;
					}
					else
					{	// Stop driving rolls, set all rolls to torque mode, zero torque
						SetRollsToTorqueMode();
						Log(LOG_ERRORS, "Unable to set torque on RF, RT, LF, and LT rolls\n");
						testResult = BEP_SOFTWAREFAIL_RESPONSE;
						testDescription = GetFaultDescription("SoftwareFailure");
						testResultCode = GetFaultCode("SoftwareFailure");
					}
				}
				else
				{// Error
					Log(LOG_ERRORS, "ERROR valid transversal locking type not specified");
					testResult = BEP_SOFTWAREFAIL_RESPONSE;
					testDescription = GetFaultDescription("SoftwareFailure");
					testResultCode = GetFaultCode("SoftwareFailure");
				}
				if (testResult == testPass)
				{//Motor setup successful continue testing
					Log(LOG_DEV_DATA, "Driving Rr: True Rt: %s Rf: %s Speed: %s Mph\n", checkRTSpeed ? "True" : "False", checkRFSpeed ? "True" : "False", commandWheelSpeed.c_str());
					SetStartTime();
					BposSleep(GetParameterInt("AdditionalTimeToAllowAxleLocking"));
					GetWheelSpeeds(wheelSpeed);
					Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);					// Wait for wheels to drive to speed
					//Monitor Rr for 90% required speed
					while ((wheelSpeed.rrWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && TimeRemaining())
					{
						BposSleep(GetTestStepInfoInt("ScanDelay"));
						GetWheelSpeeds(wheelSpeed);
						Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);
					}
					//if checking Rt wait for required speed
					if (checkRTSpeed)
					{
						Log(LOG_DEV_DATA,"Checking Rt Speed\n");
						while ((wheelSpeed.rtWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && TimeRemaining())
						{
							BposSleep(GetTestStepInfoInt("ScanDelay"));
							GetWheelSpeeds(wheelSpeed);
							Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);
						}
					}
					//if checking Rf wait for required speed
					else if (checkRFSpeed)
					{
						Log(LOG_DEV_DATA,"Checking Rf Speed\n");
						while ((wheelSpeed.rfWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && TimeRemaining())
						{
							BposSleep(GetTestStepInfoInt("ScanDelay"));
							GetWheelSpeeds(wheelSpeed);
							Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);
						}
					}
					if (!TimeRemaining())
					{	// Timeout waiting for wheel speed to reach commandWheelSpeed
						Log(LOG_ERRORS, "Timeout waiting for wheel speed to reach %s\n", commandWheelSpeed.c_str());
						testResult = testTimeout;
						testResultCode = GetFaultCode("DriveRollsTimeoutFailure");
						testDescription = GetFaultDescription("DriveRollsTimeoutFailure");
						// Stop driving rolls, set all rolls to torque mode, zero torque
						SetRollsToTorqueMode();
					}
					else
					{
						testResult = testPass;
					}
				}
				else
				{//Unable to set up motors
					Log(LOG_ERRORS, "Unable to setup Roll motors for testing\n");
				}
			}
			else
			{	// Stop driving rolls, set all rolls to torque mode, zero torque
				SetRollsToTorqueMode();
				Log(LOG_ERRORS, "Unable to set machine to drive RR roll\n");
				testResult = BEP_SOFTWAREFAIL_RESPONSE;
				testDescription = GetFaultDescription("SoftwareFailure");
				testResultCode = GetFaultCode("SoftwareFailure");
			}
			RemovePrompt(GetPromptBox("DrivingRolls"), GetPrompt("DrivingRolls"), GetPromptPriority("DrivingRolls"));
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
	catch (ModuleException &err)
	{	// Log the exception and return the error
		Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
		testResult = testSoftwareFail;
		testDescription = GetFaultDescription("SoftwareFailure");
		testResultCode = GetFaultCode("SoftwareFailure");
	}
	// Send test results to server
	SendSubtestResult("DriveRollsTransversalLock", testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::DriveRollsLongitudinalLock(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	WHEELINFO wheelSpeed;
	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{
		string commandWheelSpeed = GetParameter("LongitudinalLockWheelSpeed");
		string commandWheelTorque = GetParameter("LongitudinalLockWheelTorque");
		Log(LOG_DEV_DATA, "LongitudinalLockWheelSpeed: %s, commandWheelSpeed: %s\n", GetParameter("LongitudinalLockWheelSpeed").c_str(), commandWheelSpeed.c_str());
		// Running this test at high speeds may cause damage to the differential
		if (atof(commandWheelSpeed.c_str()) > 10.0)
		{
			Log(LOG_ERRORS, "ERROR: Running this test at high speeds may cause damage to the differential: Defaulting to 2.0 Mph\n");
			commandWheelSpeed = "2.0";
		}
		testResult = EngageMachine();
		if (testResult == testPass)
		{// Display prompt to notify driver machine will be driving rolls
			DisplayPrompt(GetPromptBox("DrivingRolls"), GetPrompt("DrivingRolls"), GetPromptPriority("DrivingRolls"));
			// Configure rolls for individual roll command
			if (IsRearAndTandemLongEquipped())
			{//drive rt and lt wheels
				if (m_motorController.Write("LeftFrontMotorMode","Torque") == 0 &&
					m_motorController.Write("RightFrontMotorMode","Torque") == 0 &&
					m_motorController.Write("LeftRearMotorMode","Torque") == 0 &&
					m_motorController.Write("RightRearMotorMode","Torque") == 0 &&
					m_motorController.Write("LeftTandemMotorMode","Speed") == 0 &&
					m_motorController.Write("RightTandemMotorMode","Speed") == 0 &&
					m_motorController.Write("LeftFrontTorqueValue","0.0") == 0 &&
					m_motorController.Write("RightFrontTorqueValue","0.0") == 0 &&
					m_motorController.Write("LeftRearTorqueValue",commandWheelTorque.c_str()) == 0 &&
					m_motorController.Write("RightRearTorqueValue",commandWheelTorque.c_str()) == 0 &&
					m_motorController.Write("LeftTandemSpeedValue",commandWheelSpeed.c_str()) == 0 &&
					m_motorController.Write("RightTandemSpeedValue",commandWheelSpeed.c_str(),true) == 0)
				{
					Log(LOG_DEV_DATA, "Commanding Rt and Lt to: %s Mph\n", commandWheelSpeed.c_str());
					SetStartTime();
					BposSleep(GetParameterInt("AdditionalTimeToAllowAxleLocking"));
					GetWheelSpeeds(wheelSpeed);
					Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);					// Wait for wheels to drive to speed
					//Monitor Rr and Lr for 90% required speed
					while ((wheelSpeed.rtWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && (wheelSpeed.ltWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && TimeRemaining())
					{
						BposSleep(GetTestStepInfoInt("ScanDelay"));
						GetWheelSpeeds(wheelSpeed);
						Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);
					}
				}
				else
				{	// Stop driving rolls, set all rolls to torque mode, zero torque
					SetRollsToTorqueMode();
					Log(LOG_ERRORS, "ERROR unable to setup machine to drive RT and LT rolls");
					testResult = BEP_SOFTWAREFAIL_RESPONSE;
					testDescription = GetFaultDescription("SoftwareFailure");
					testResultCode = GetFaultCode("SoftwareFailure");
				}
			}
			else if (IsFrontAndRearLongEquipped())
			{//drive rr and lr wheels
				if (m_motorController.Write("LeftFrontMotorMode","Torque") == 0 &&
					m_motorController.Write("RightFrontMotorMode","Torque") == 0 &&
					m_motorController.Write("LeftRearMotorMode","Speed") == 0 &&
					m_motorController.Write("RightRearMotorMode","Speed") == 0 &&
					m_motorController.Write("LeftTandemMotorMode","Torque") == 0 &&
					m_motorController.Write("RightTandemMotorMode","Torque") == 0 &&
					m_motorController.Write("LeftFrontTorqueValue",commandWheelTorque.c_str()) == 0 &&
					m_motorController.Write("RightFrontTorqueValue",commandWheelTorque.c_str()) == 0 &&
					m_motorController.Write("LeftRearSpeedValue",commandWheelSpeed.c_str()) == 0 &&
					m_motorController.Write("RightRearSpeedValue",commandWheelSpeed.c_str()) == 0 &&
					m_motorController.Write("LeftTandemTorqueValue","0.0") == 0 &&
					m_motorController.Write("RightTandemTorqueValue","0.0",true) == 0)
				{
					Log(LOG_DEV_DATA, "Commanding Rr and Lr to: %s Mph\n", commandWheelSpeed.c_str());
					SetStartTime();
					BposSleep(GetParameterInt("AdditionalTimeToAllowAxleLocking"));
					GetWheelSpeeds(wheelSpeed);
					Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);					// Wait for wheels to drive to speed
					//Monitor Rr and Lr for 90% required speed
					while ((wheelSpeed.rrWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && (wheelSpeed.lrWheel < (0.90 * atof(commandWheelSpeed.c_str()))) && TimeRemaining())
					{
						BposSleep(GetTestStepInfoInt("ScanDelay"));
						GetWheelSpeeds(wheelSpeed);
						Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);
					}
				}
				else
				{	// Stop driving rolls, set all rolls to torque mode, zero torque
					SetRollsToTorqueMode();
					Log(LOG_ERRORS, "ERROR unable to setup machine to drive RR and LR rolls");
					testResult = BEP_SOFTWAREFAIL_RESPONSE;
					testDescription = GetFaultDescription("SoftwareFailure");
					testResultCode = GetFaultCode("SoftwareFailure");
				}
			}
			else
			{// Error
				Log(LOG_ERRORS, "ERROR valid longitudinal locking type not specified");
				testResult = BEP_SOFTWAREFAIL_RESPONSE;
				testDescription = GetFaultDescription("SoftwareFailure");
				testResultCode = GetFaultCode("SoftwareFailure");
			}
			if (testResult != BEP_SOFTWAREFAIL_RESPONSE)
			{
				if (!TimeRemaining())
				{	// Timeout waiting for wheel speed to reach commandWheelSpeed
					Log(LOG_ERRORS, "Timeout waiting for wheel speed to reach %s\n", commandWheelSpeed.c_str());
					testResult = testTimeout;
					testResultCode = GetFaultCode("DriveRollsTimeoutFailure");
					testDescription = GetFaultDescription("DriveRollsTimeoutFailure");
					// Stop driving rolls, set all rolls to torque mode, zero torque
					SetRollsToTorqueMode();
				}
				else
				{
					testResult = testPass;
				}
			}
			RemovePrompt(GetPromptBox("DrivingRolls"), GetPrompt("DrivingRolls"), GetPromptPriority("DrivingRolls"));
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
	catch (ModuleException &err)
	{	// Log the exception and return the error
		Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
		testResult = testSoftwareFail;
		testDescription = GetFaultDescription("SoftwareFailure");
		testResultCode = GetFaultCode("SoftwareFailure");
	}
	// Send test results to server
	SendSubtestResult("DriveRollsLongitudinalLock",testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::CheckWheelSpeedTransversalLock(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string rearResult(testSkip);
	string tandemResult(testSkip);
	string frontResult(testSkip);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	WHEELINFO wheelSpeed;
	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{
		GetWheelSpeeds(wheelSpeed);
		Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);					// Wait for wheels to drive to speed
		//Check LR wheel is within tolerance
		if (wheelSpeed.lrWheel > (wheelSpeed.rrWheel * (1 - GetParameterFloat("TransversalLockWheelSpeedTolerance"))) && 
			wheelSpeed.lrWheel < (wheelSpeed.rrWheel * (1 + GetParameterFloat("TransversalLockWheelSpeedTolerance"))))
		{
			Log(LOG_DEV_DATA, "Rear Transversal wheel speed check passed.\n");
			rearResult = testPass;
		}
		else
		{// Wheel speeds are not within tolerance
			Log(LOG_ERRORS, "LR wheel speed not within tolerance.\n");
			rearResult = testFail;
		}
		if (IsRearAndTandemTransEquipped())
		{//check LT wheel is within tolerance
			if (wheelSpeed.ltWheel > (wheelSpeed.rtWheel * (1 - GetParameterFloat("TransversalLockWheelSpeedTolerance"))) && 
				wheelSpeed.ltWheel < (wheelSpeed.rtWheel * (1 + GetParameterFloat("TransversalLockWheelSpeedTolerance"))))
			{
				Log(LOG_DEV_DATA, "Tandem Transversal wheel speed check passed.\n");
				tandemResult = testPass;
			}
			else
			{// Wheel speeds are not within tolerance
				Log(LOG_ERRORS, "LT wheel speed not within tolerance.\n");
				tandemResult = testFail;
			}
		}
		else if (IsFrontAndRearTransEquipped())
		{//check LF wheel is within tolerance
			if (wheelSpeed.lfWheel > (wheelSpeed.rfWheel * (1 - GetParameterFloat("TransversalLockWheelSpeedTolerance"))) && 
				wheelSpeed.lfWheel < (wheelSpeed.rfWheel * (1 + GetParameterFloat("TransversalLockWheelSpeedTolerance"))))
			{
				Log(LOG_DEV_DATA, "Front Transversal wheel speed check passed.\n");
				frontResult = testPass;
			}
			else
			{	// Wheel speeds are not within tolerance
				Log(LOG_ERRORS, "LF wheel speed not within tolerance.\n");
				frontResult = testFail;
			}
		}
		if ((frontResult == testPass || !IsFrontAndRearTransEquipped()) && (tandemResult == testPass || !IsRearAndTandemTransEquipped()) && rearResult == testPass)
		{
			testResult = testPass;
		}
		else
		{
			testResult = testFail;
			testResultCode = GetFaultCode("TransversalLockFailure");
			testDescription = GetFaultDescription("TransversalLockFailure");
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
	SendSubtestResultWithDetail("CheckWheelSpeedTransversalLock", testResult, testDescription, testResultCode,
								"TransversalLockRear",rearResult,"",
								"TransversalLockTandem",tandemResult,"",
								"TransversalLockFront",frontResult,"");
	// Stop driving rolls, set all rolls to torque mode, zero torque
	SetRollsToTorqueMode();
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::CheckWheelSpeedLongitudinalLock(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	WHEELINFO wheelSpeed;
	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{

		GetWheelSpeeds(wheelSpeed);
		Log(LOG_DEV_DATA, "\nWheel Speeds:\n\tLF: %f\n\tRF: %f\n\tLR: %f\n\tRR: %f\n\tLT: %f\n\tRT: %f\n", wheelSpeed.lfWheel, wheelSpeed.rfWheel, wheelSpeed.lrWheel, wheelSpeed.rrWheel, wheelSpeed.ltWheel, wheelSpeed.rtWheel);					// Wait for wheels to drive to speed
		if (IsFrontAndRearLongEquipped())
		{//check front wheels are within tolerance
			if (wheelSpeed.rfWheel > (wheelSpeed.rrWheel * (1 - GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) && 
				wheelSpeed.rfWheel < (wheelSpeed.rrWheel * (1 + GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) &&
				wheelSpeed.lfWheel > (wheelSpeed.lrWheel * (1 - GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) &&
				wheelSpeed.lfWheel < (wheelSpeed.lrWheel * (1 + GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))))
			{
				Log(LOG_DEV_DATA, "Rear to Front wheel speed check passed.\n");
				testResult = testPass;
			}
			else
			{	// Wheel speeds are not within tolerance
				Log(LOG_ERRORS, "Front wheel speeds not within tolerance.\n");
				testResult = testFail;
				testDescription = GetFaultDescription("LongitudinalLockFailureFrontToRear");
				testResultCode = GetFaultCode("LongitudinalLockFailureFrontToRear");
			}
		}
		else if (IsRearAndTandemLongEquipped())
		{//check rear wheels are within tolerance
			if (wheelSpeed.rrWheel > (wheelSpeed.rtWheel * (1 - GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) && 
				wheelSpeed.rrWheel < (wheelSpeed.rtWheel * (1 + GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) &&
				wheelSpeed.lrWheel > (wheelSpeed.ltWheel * (1 - GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))) &&
				wheelSpeed.lrWheel < (wheelSpeed.ltWheel * (1 + GetParameterFloat("LongitudinalLockWheelSpeedTolerance"))))
			{
				Log(LOG_DEV_DATA, "Tandem to Rear whlle speed check passed.\n");
				testResult = testPass;
			}
			else
			{	// Wheel speeds are not within tolerance
				Log(LOG_ERRORS, "Rear wheel speeds not within tolerance.\n");
				testResult = testFail;
				testDescription = GetFaultDescription("LongitudinalLockFailureRearToTandem");
				testResultCode = GetFaultCode("LongitudinalLockFailureRearToTandem");
			}
		}
		else
		{//longitudinal lock specified not supported
			Log(LOG_ERRORS, "Valid Longitudinal axle lock type not specified.\n");
			testResult = testSoftwareFail;
			testDescription = GetFaultDescription("SoftwareFailure");
			testResultCode = GetFaultCode("SoftwareFailure");
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
	SendSubtestResult("CheckWheelSpeedLongitudinalLock", testResult, testDescription, testResultCode);
	// Stop driving rolls, set all rolls to torque mode, zero torque
	SetRollsToTorqueMode();
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::SetRollsToTorqueMode(void)
{
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Set all motors to Torque mode. Set all torque values to 0.0
	if (m_motorController.Write("LeftFrontMotorMode","Torque") == 0 &&
		m_motorController.Write("RightFrontMotorMode","Torque") == 0 &&
		m_motorController.Write("LeftRearMotorMode","Torque") == 0 &&
		m_motorController.Write("RightRearMotorMode","Torque") == 0 &&
		m_motorController.Write("LeftTandemMotorMode","Torque") == 0 &&
		m_motorController.Write("RightTandemMotorMode","Torque") == 0 &&
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
	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::SetRollsToBoostMode(void)
{
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");

	// return the system to Boost Mode

	if (SystemWrite("MotorMode", "Boost") == 0)
	{
		testResult = testPass;
	}
	else
	{
		testResult = BEP_SOFTWAREFAIL_RESPONSE;
		testDescription = GetFaultDescription("SoftwareFailure");
		testResultCode = GetFaultCode("SoftwareFailure");
	}
	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::Delay(void)
{
	// This test will always pass
	string testResult            = testPass;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");

	// Delay system the specified amount of time
	BposSleep(GetTestStepInfoInt("Delay"));

	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	// Log Function Exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return(testResult);
}

string TandemLockingDifferentialTC::LongitudinalLockTest(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	string status(BEP_NO_DATA);		// Used to hold return value from OperatorYesNo()

	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (ShortCircuitTestStep() || IsLongitudinalLockingUndefined())
	{// Skip test because of previous pass or longitudinal locking is undefined
		Log(LOG_DEV_DATA,"%s:%s skipped\n", GetComponentName().c_str(), GetTestStepName().c_str());
		testResult = testSkip;
	}
	else
	{// Prompt driver to shift to Longitudinal lock
		testResult = DisplayPromptOptionalVerify("ShiftToLongLock");
		if (testResult == testPass)
		{// Drive the right rear roller to speed
			testResult = DriveRollsLongitudinalLock();
			if (testResult == testPass)
			{//ensure wheel speeds are within tolerance
				testResult = CheckWheelSpeedLongitudinalLock();
			}
			// Always set rolls to torque mode after test, abort if step fails
			if (SetRollsToTorqueMode() != testPass)
				testResult = testAbort;
		}
	}
	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	return testResult;
}

string TandemLockingDifferentialTC::TransversalLockTest(void)
{	// Required test step variables and their correct initialization
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	string status(BEP_NO_DATA);		// Used to hold return value from OperatorYesNo()

	// Log Function Entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if (ShortCircuitTestStep() || IsTransversalLockingUndefined())
	{	// Skip test because of previous pass
		Log(LOG_DEV_DATA,"%s:%s skipped\n", GetComponentName().c_str(), GetTestStepName().c_str());
		testResult = testSkip;
	}
	else
	{// Prompt driver to shift to Transversal lock
		testResult = DisplayPromptOptionalVerify("ShiftToTransLock");
		if (testResult == testPass)
		{// Drive the right rear roller to speed
			testResult = DriveRollsTransversalLock();
			if (testResult == testPass)
			{//ensure wheel speeds are within tolerance
				testResult = CheckWheelSpeedTransversalLock();
			}
			// Always set rolls to torque mode after test, abort if step fails
			if (SetRollsToTorqueMode() != testPass)
				testResult = testAbort;
		}
	}
	// Send test results to server
	SendTestResult(testResult, testDescription, testResultCode);
	return testResult;
}

string TandemLockingDifferentialTC::DisplayPromptOptionalVerify(string promptTag)
{
	string testResult(testFail);
	string testDescription(GetTestStepInfo("Description"));
	string testResultCode("0000");
	string status(BEP_NO_DATA);		// Used to hold return value from OperatorYesNo()
	if (ShortCircuitTestStep())
	{	// Skip test because of previous pass
		Log(LOG_DEV_DATA,"%s:%s skipped\n", GetComponentName().c_str(), GetTestStepName().c_str());
		testResult = testSkip;
	}
	else
	{// Prompt driver
		if (GetParameterBool("Verify" + promptTag))
		{//Have the Driver verify
			DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
			status = OperatorPassFail();
			RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
			testResult = status == "Pass" ? testPass : testFail;
		}
		else
		{//Display a timed prompt
			GenericTC::DisplayTimedPrompt(GetPrompt(promptTag), "1", GetPromptPriority(promptTag), GetParameterInt(promptTag + "MessageDelay"));
			testResult = testPass;
		}
	}
	return testResult;
}

void TandemLockingDifferentialTC::SetRearAndTandemLongEquipped(bool isRearAndTandemLongEquipped)
{
	m_isRearAndTandemLongEquipped = isRearAndTandemLongEquipped;
}

bool TandemLockingDifferentialTC::IsRearAndTandemLongEquipped(void)
{
	return m_isRearAndTandemLongEquipped;
}

void TandemLockingDifferentialTC::SetFrontAndRearLongEquipped(bool isFrontAndRearLongEquipped)
{
	m_isFrontAndRearLongEquipped = isFrontAndRearLongEquipped;
}

bool TandemLockingDifferentialTC::IsFrontAndRearLongEquipped(void)
{
	return m_isFrontAndRearLongEquipped;
}

void TandemLockingDifferentialTC::SetLongitudinalLockingUndefined(bool isLongitudinalLockingUndefined)
{
	m_isLongitudinalLockingUndefined = isLongitudinalLockingUndefined;
}

bool TandemLockingDifferentialTC::IsLongitudinalLockingUndefined(void)
{
	return m_isLongitudinalLockingUndefined;
}

void TandemLockingDifferentialTC::SetRearTransEquipped(bool isRearTransEquipped)
{
	m_isRearTransEquipped = isRearTransEquipped;
}

bool TandemLockingDifferentialTC::IsRearTransEquipped(void)
{
	return m_isRearTransEquipped;
}

void TandemLockingDifferentialTC::SetRearAndTandemTransEquipped(bool isRearAndTandemTransEquipped)
{
	m_isRearAndTandemTransEquipped = isRearAndTandemTransEquipped;
}

bool TandemLockingDifferentialTC::IsRearAndTandemTransEquipped(void)
{
	return m_isRearAndTandemTransEquipped;
}

void TandemLockingDifferentialTC::SetFrontAndRearTransEquipped(bool isFrontAndRearTransEquipped)
{
	m_isFrontAndRearTransEquipped = isFrontAndRearTransEquipped;
}

bool TandemLockingDifferentialTC::IsFrontAndRearTransEquipped(void)
{
	return m_isFrontAndRearTransEquipped;
}

void TandemLockingDifferentialTC::SetTransversalLockingUndefined(bool isTransversalLockingUndefined)
{
	m_isTransversalLockingUndefined = isTransversalLockingUndefined;
}

bool TandemLockingDifferentialTC::IsTransversalLockingUndefined(void)
{
	return m_isTransversalLockingUndefined;
}

inline string TandemLockingDifferentialTC::FinishUp( void )
{
	return(testPass);
}

const bool TandemLockingDifferentialTC::IsLockingDifferentialEquipped(void)
{
	bool isEquipped = !(SystemRead(GetDataTag("AxleLongLockTag")) == "Undefined" && SystemRead(GetDataTag("AxleTransLockTag")) == "Undefined");
	Log(LOG_DEV_DATA,"\n\tSystemRead(GetDataTag(\"AxleLongLockTag\")) = %s\n\tSystemRead(GetDataTag(\"AxleTransLockTag\")) = %s\n\tisEquipped = %s\n", SystemRead(GetDataTag("AxleLongLockTag")).c_str(), SystemRead(GetDataTag("AxleTransLockTag")).c_str(), isEquipped ? "True" : "False");
	return(isEquipped);
}

void TandemLockingDifferentialTC::Abort(void)
{
	GenericTC::Abort();
	Log(LOG_DEV_DATA, "Tandem loacking differential test aborting!\n");
}


