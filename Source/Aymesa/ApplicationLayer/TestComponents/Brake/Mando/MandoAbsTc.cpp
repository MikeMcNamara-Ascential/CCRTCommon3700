//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//  Module Interface for the APG 3550 ABS.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
// 
//*************************************************************************
#include "MandoAbsTc.h"

//-----------------------------------------------------------------------------
template <class ModuleType>
MandoAbsTc<ModuleType>::MandoAbsTc()
{	// Nothing special to do
}

//-----------------------------------------------------------------------------
template <class ModuleType>
MandoAbsTc<ModuleType>::~MandoAbsTc()
{	// Nothing special to do
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const string MandoAbsTc<ModuleType>::CommandTestStep(const string &value)
{
	string result = BEP_TESTING_RESPONSE;
	string testStep(GetTestStepName());
	Log(LOG_FN_ENTRY, "MandoAbsTc::CommandTestStep(%s) - Entering test step: %s", value.c_str(), testStep.c_str());
	try
	{	// Ensure system status will allow a test step to be performed
		if(StatusCheck() == BEP_STATUS_SUCCESS)
		{	// Determine which test step to perform
			// Establish Module Comms
			if(!testStep.compare("CheckEspEquipped"))
			{
				result = CheckEspEquipped();
			}
			else if(!testStep.compare("CheckEolTestByte"))
			{
				result = CheckEolTestByte();
			}
			else if(!testStep.compare("WriteEolTestByte"))
			{
				result = WriteEolTestByte();
			}
			else if(!testStep.compare("VariantCoding"))
			{
				if(IsESPEquipped())
				{
					result = VariantCoding();
				}
				else result	= testPass;
			}
			else if(!testStep.compare("SasCalibration"))
			{
				if(IsESPEquipped())
				{
					result = SasCalibration();
				}
				else result	= testPass;
			}
			else if(!testStep.compare("HdcLightTest"))
			{
				if(isHdc)
				{
					result = HdcLightTest();
				}
				else result	= testPass;
			}
			else if(!testStep.compare("LfSensorTest"))
			{
				result = LfSensorTest(value);
			}
			else if(!testStep.compare("RfSensorTest"))
			{
				result = LrSensorTest(value);
			}
			else if(!testStep.compare("LrSensorTest"))
			{
				result = RfSensorTest(value);
			}
			else if(!testStep.compare("RrSensorTest"))
			{
				result = RrSensorTest(value);
			}
			else if(!testStep.compare("EspValveFiringTest"))
			{
				if(IsESPEquipped())
				{
					result = EspValveFiringTest();
				}
				else result	= testPass;
			}
			else if(!testStep.compare("EvaluateESP"))
			{
				if(IsESPEquipped())
				{
					result = EvaluateEsp();
				}
				else result	= testPass;
			}
			else if(!testStep.compare("WarningLight"))
			{
				result = WarningLight();
			}
			// Try the base class
			else
			{
				result = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
			}
		}
		else
		{	// Bad system status, not performing the test step.
			result = ConvertStatusToResponse(StatusCheck());
			Log(LOG_ERRORS, "MandoAbsTc::CommandTestStep: Status check failed: %s", result.c_str());
		}
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "MandoAbsTc::CommandTestStep() - Exception from step %s - %s", testStep.c_str(), excpt.GetReason());
		result = BEP_ERROR_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "MandoAbsTc::CommandTestStep returning: %s", result.c_str());
	return result;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string MandoAbsTc<ModuleType>::CheckEspEquipped(void)
{	// Log the entry and determine if the test should be performed
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "MandoAbsTc::CheckEspEquipped() - Enter\n");
	// Check if the vehicle is equipped with Turbo option
	try
	{
		bool isEspEquipped = false;
		moduleStatus = m_vehicleModule.CheckEspEquipped(isEspEquipped);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS == moduleStatus)
		{	// Good read, store the data
			SetESPEquipped(isEspEquipped);
			testResult = testPass;
			Log(LOG_DEV_DATA, "Vehicle equipped with Esp: %s\n", isEspEquipped ? "True" : "False");
		}
		else
		{	// Communication failure reading Turbo option
			Log(LOG_ERRORS, "Communication failure reading Esp option from the module - status: %s\n",
				ConvertStatusToResponse(moduleStatus).c_str());
			SetESPEquipped(false);	 // Default to not equipped since we could not determine
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the test result
	SendTestResult(testResult, testDescription, testResultCode);
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::CheckEspEquipped() - Exit\n");
	return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string MandoAbsTc<ModuleType>::CheckEolTestByte(void)
{	// Log the entry and determine if the test should be performed
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "MandoAbsTc::CheckEolTestByte() - Enter");
	if(!ShortCircuitTestStep())
	{	// Command the module to read the ECU
		string testResult = testFail;
		string testResultCode = GetFaultCode("CheckEolTestByte");
		string testDescription = GetFaultDescription("CheckEolTestByte");

		SerialString_t response(255, 0);
		Log(LOG_FN_ENTRY, "Enter CheckEolTestByte()\n");
		BEP_STATUS_TYPE status = m_vehicleModule.ReadEolTestByte(response);

		if(status == BEP_STATUS_SUCCESS)
		{
			int testByte = response[1];

			if(testByte == 0xAA)
			{
				testResult = testPass;
				testResultCode = "0000";
				testDescription = GetTestStepInfo("Description");
			}
			Log(LOG_DEV_DATA, "Check EOL Test Byte: %s", testResult.c_str());
			// Log the result and report to the test result server
			SendTestResult(testResult, testDescription, testResultCode);
		}
		else
		{
			Log(LOG_DEV_DATA, "CheckEolTestByte response failed\n");
		}
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::CheckEolTestByte() - Exit");
	return testResult;
}
//-----------------------------------------------------------------------------
template <class ModuleType>
string MandoAbsTc<ModuleType>::WriteEolTestByte(void)
{	// Log the entry and determine if the test should be performed
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "MandoAbsTc::WriteEolTestByte() - Enter");
	if(!ShortCircuitTestStep())
	{
		string testResult = testFail;
		string testResultCode = GetFaultCode("WriteEolTestByte");
		string testDescription = GetFaultDescription("WriteEolTestByte");

		UINT16 testByte= 0XAA;
		BEP_STATUS_TYPE status = m_vehicleModule.WriteEolTestByte(testByte);
		if(status == BEP_STATUS_SUCCESS)
		{
			testResult = testPass;
			testResultCode = "0000";
			testDescription = GetTestStepInfo("Description");
		}
		Log(LOG_DEV_DATA, "Write EOL Test Byte: %s", testResult.c_str());
		// Report result to the test result server
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::WriteEolTestByte() - Exit");
	return testResult;
}

//=============================================================================
template <class ModuleType>
string MandoAbsTc<ModuleType>::VariantCoding(void)
{
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "MandoAbsTc::VariantCoding() - Enter");
	if(!ShortCircuitTestStep())
	{	// Command the module to enable the diagnostic speed limit
		BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("VariantCoding");
		testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
		string testResultCode(!testResult.compare(testPass) ? "0000" : GetFaultCode("VariantCoding"));
		string testDescription(!testResult.compare(testPass) ? GetTestStepInfo("Description") : GetFaultDescription("VariantCoding"));
		// Log the result and report to the test result server
		Log(LOG_DEV_DATA, "Variant Coding: %s", testResult.c_str());
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::VariantCoding() - Exit");
	return testResult;

}

//=============================================================================
template <class ModuleType>
string MandoAbsTc<ModuleType>::SasCalibration(void)
{
	string testResult(BEP_TESTING_STATUS);
	BEP_STATUS_TYPE status;
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	float sasAngle = 0;
	float sasAnglemin = GetParameterFloat("SASAngleRangeMin");
	float sasAnglemax = GetParameterFloat("SASAngleRangeMax");
	bool inRange = false;

	Log(LOG_FN_ENTRY, "MandoAbsTc::SasCalibration() - Enter");
	if(!ShortCircuitTestStep())
	{
		DisplayPrompt(1,"SteeringWheelinStraightPosition",true);
		BposSleep(1500);
		status = m_vehicleModule.CommandModule("SasDecalibration");
		if(status == BEP_STATUS_SUCCESS)
		{
			try
			{
				status = m_vehicleModule.ReadModuleData("ReadSasAngle", sasAngle);
				if(status == BEP_STATUS_SUCCESS)
				{
					inRange = (sasAngle > sasAnglemin) && (sasAngle < sasAnglemax);
					Log(LOG_DEV_DATA, "SAS Angle: %.1f", sasAngle);
					if(inRange)
					{	// Do the calibration
						status = m_vehicleModule.CommandModule("SasCalibration");
						// set the results
						testResult = (status == BEP_STATUS_SUCCESS ? testPass : testFail);
						testResultCode = (status == BEP_STATUS_SUCCESS ? "0000" : GetFaultCode("CommunicationFailure"));
						testDescription = (status == BEP_STATUS_SUCCESS ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
					}
					else
					{
						testResult =  testFail;
						testResultCode = GetFaultCode("SASAngleOutOfRange");
						testDescription = GetFaultDescription("SASAngleOutOfRange");
					}
				}
				else
				{ // Set the results
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommuncationFailure");
				}
			}
			catch(ModuleException &e)
			{
				Log(LOG_ERRORS, "ModuleException reading SAS Angle - %s", e.GetReason());
				testResult = testSoftwareFail;
				testResultCode = GetFaultCode("SoftwareFailure");
				testDescription = GetFaultDescription("SoftwareFailure");
			}
		}
		else
		{
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommuncationFailure");
		}
		// Log the result and report to the test result server
		Log(LOG_DEV_DATA, "Steering Angle Sensor Calibration: %s", testResult.c_str());
		SendTestResult(testResult, testDescription, testResultCode);
		RemovePrompt(1,"SteeringWheelinStraightPosition",true);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::SasCalibration() - Exit");
	return testResult;

}

//=============================================================================
template <class ModuleType>
string MandoAbsTc<ModuleType>::HdcLightTest(void)
{
	string testResult(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "MandoAbsTc::HdcLightTest() - Enter");
	if(!ShortCircuitTestStep())
	{	// Command the module to enable the diagnostic speed limit
		BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
		string testResultCode = "0000";
		string testDescription = GetTestStepInfo("Description");
		int count = 3;      

		RemovePrompts();
		DisplayPrompt(1,"ReceiptBrakeLightFlashing",true);
		// Blink HDC light on and off at one second intervals for 1/2 second at a time 
		while(testResult == testPass && count > 0)
		{
			status = m_vehicleModule.CommandModule("BrakeLightOn");
			if(status != BEP_STATUS_SUCCESS)
			{
				testResult = testFail;
			}
			BposSleep(500);
			status = m_vehicleModule.CommandModule("BrakeLightOff");
			if(status != BEP_STATUS_SUCCESS)
			{
				testResult = testFail;
			}
			BposSleep(500);
			count--;
		}
		if(testResult == testFail)
		{
			testResultCode = GetFaultCode("HcdLightTest");
			testDescription = GetFaultDescription("HdcLightTest");
		}

		RemovePrompts();
		// Log the result and report to the test result server
		Log(LOG_DEV_DATA, "HDC Light Test: %s", testResult.c_str());
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::HdcLightTest() - Exit");
	return testResult;
}

//=============================================================================
template <class ModuleType>
string MandoAbsTc<ModuleType>::EspValveFiringTest(void)
{
	string testResult             = BEP_TESTING_STATUS;		// Used to report test step results
	string testResultCode         = "0000";
	string testDescription        = GetTestStepInfo("Description");
	string overallReductionResult = BEP_TESTING_STATUS;
	string overallRecoveryResult  = BEP_TESTING_STATUS;
	string actuationResult        = BEP_TESTING_STATUS;
	BEP_STATUS_TYPE status        = BEP_STATUS_ERROR;

	// Clear the reduction recovery indices
	for(INT32 wheelIndex = 0; (UINT16)wheelIndex < MAXWHEELS; wheelIndex++)
	{
		m_espReduxRecovIndex[wheelIndex].recoveryEnd = 0;
		m_espReduxRecovIndex[wheelIndex].recoveryStart = 0;
		m_espReduxRecovIndex[wheelIndex].reductionEnd = 0;
		m_espReduxRecovIndex[wheelIndex].reductionStart = 0;
	}

	// Define a structure for storing the local results
	struct _valveFiringResults
	{
		string reductionResult;
		string recoveryResult;
	} valveFiringResults[2];

	string wheelpairs[] = {"LfRr","RfLr"};// List of wheel names for commandmodule calls

	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Verify it is ok to test -- not skip, brake testing in progress or development testing

	if((!ShortCircuitTestStep() && (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)) ||
	   GetParameterBool("DeveloperTest"))
	{	// Instruct operator and update GUI
		UpdatePrompts();
		// Determine if the force meter should be updated during valve firing
		if(GetParameterBool("DisableMeterForValveFiring"))
			m_baseBrakeTool->DisableForceUpdates();
		else
			m_baseBrakeTool->EnableForceUpdates();

		// Tag the start of the reduction/recovery sequence
		m_espStartIndex = TagArray("ESPStart");

		// Verify that the brake is not applied
		/*
		if (BrakeSwitchTest(GetDataTag("SwitchOff")) != testPass)
		{
			testResult = testFail;
			Log(LOG_DEV_DATA, "Brake switch off: testFail");
		}*/

		// Fire each wheel in the sequence
		for(UINT32 i = 0; i < 2; i++)
		{	// Get the index for the current wheel
			// Provide a short delay between valves -- NOTE: use delay instead of BposSleep(), timing is critical!
			delay(GetParameterInt("InterWheelGapDelay"));

			//--------------------- Start Esp calls for each wheel ----------------
			// Recovery 
			m_espReduxRecovIndex[i].recoveryStart = TagArray(wheelpairs[i] + "EspRecovStart");
			status = m_vehicleModule.EspRecovery(wheelpairs[i]);
			if(status == BEP_STATUS_SUCCESS)
			{
				// wait for the forces to build
				delay(GetParameterInt("EspRecoveryPulseLength"));
				m_espReduxRecovIndex[i].recoveryEnd = TagArray(wheelpairs[i] + "EspRecovStop");
				valveFiringResults[i].recoveryResult = testPass;
				testResultCode = "0000";
				testDescription = "Performing recovery for " + wheelpairs[i];
			}
			else
			{
				valveFiringResults[i].recoveryResult = testFail;
				Log(LOG_ERRORS, "Communication failure performing reduction for %s -- status: %s\n",
					wheelpairs[i].c_str(), ConvertStatusToResponse(status).c_str());
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			SendSubtestResult(wheelpairs[i] + "Recovery",
							  status, testDescription, testResultCode);

			// Reduce the wheel
			m_espReduxRecovIndex[i].reductionStart = TagArray(wheelpairs[i] + "EspReduxStart");
			status = m_vehicleModule.EspReduction(wheelpairs[i]);
			if(status == BEP_STATUS_SUCCESS)
			{
				// wait for Forces to dissipate
				delay(GetParameterInt("EspReductionPulseLength"));
				m_espReduxRecovIndex[i].reductionEnd = TagArray(wheelpairs[i] + "EspReduxStop");
				valveFiringResults[i].reductionResult = testPass;
				testResultCode = "0000";
				testDescription = "Performing reduction for " + wheelpairs[i];
			}
			else
			{
				valveFiringResults[i].reductionResult = testFail;
				Log(LOG_ERRORS, "Communication failure performing reduction for %s -- status: %s\n",
					wheelpairs[i].c_str(), ConvertStatusToResponse(status).c_str());
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			// Report the result of the test
			SendSubtestResult(wheelpairs[i] + "Recovery",
							  status, testDescription, testResultCode);
			//--------------------- End Esp calls for each wheel ----------------
		}
		// Tag the end of the ESP sequence
		m_espEndIndex = TagArray("ESPEnd");

		// Remove the operator prompts
		RemovePrompts();

		// Set wheels 2 & 3 to values of 0 & 1 since values are fired in diagonal pairs
		m_espReduxRecovIndex[3].recoveryStart = m_espReduxRecovIndex[0].recoveryStart;
		m_espReduxRecovIndex[3].recoveryEnd = m_espReduxRecovIndex[0].recoveryEnd;
		m_espReduxRecovIndex[3].reductionStart = m_espReduxRecovIndex[0].reductionStart;
		m_espReduxRecovIndex[3].reductionEnd = m_espReduxRecovIndex[0].reductionEnd;

		m_espReduxRecovIndex[2].recoveryStart = m_espReduxRecovIndex[1].recoveryStart;
		m_espReduxRecovIndex[2].recoveryEnd = m_espReduxRecovIndex[1].recoveryEnd;
		m_espReduxRecovIndex[2].reductionStart = m_espReduxRecovIndex[1].reductionStart;
		m_espReduxRecovIndex[2].reductionEnd = m_espReduxRecovIndex[1].reductionEnd;

		// Assume all wheels have passed unless otherwise indicated
		overallReductionResult = testPass;
		overallRecoveryResult  = testPass;
		testResultCode         = "0000";
		testDescription        = GetTestStepInfo("Description");
		// Determine the results of the test
		for(UINT32 i = 0; i < 2; i++)
		{	// Check if the reduction sequence on this wheel failed
			if(testPass != valveFiringResults[i].reductionResult)  overallReductionResult = testFail;
			// Check if the recovery sequence on this wheel failed
			if(testPass != valveFiringResults[i].recoveryResult)   overallRecoveryResult  = testFail;
		}
		// Send the results for this step
		testResult = ((overallReductionResult == testPass) && (overallRecoveryResult == testPass)) ? testPass : testFail;
		testResultCode = testPass == testResult ? testResultCode : GetFaultCode("ReduxRecovSequenceFail");
		testDescription = testPass == testResult ? testDescription : GetFaultDescription("ReduxRecovSequenceFail");
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OverallReductionSequence", overallReductionResult, "",
								 "OverallRecoverySequence", overallRecoveryResult, "");
	}
	else
	{	// Test step is being skipped
		Log(LOG_FN_ENTRY, "Skipping %s -- \n\t\tShortCircuitTestStep: %s\n\t\tBrakeTestingStatus: %s\n\t\tDeveloperTest: %s\n",
			GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
			m_baseBrakeTool->GetBrakeTestingStatus().c_str(), GetParameterBool("DeveloperTest") ? "True" : "False");
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;

}

//-----------------------------------------------------------------------------
template <class ModuleType>
string MandoAbsTc<ModuleType>::EvaluateEsp(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	string wheelReduxResults[GetRollerCount()];
	string wheelRecovResults[GetRollerCount()];
	string overallRedux = testPass, overallRecov = testPass;
	INT32 endIndexExtentsion = GetParameterInt("EspExtendCalculation");	// gives time for wheels to respond after command is sent

	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if t his step should be performed
	if((!ShortCircuitTestStep() && (BEP_TESTING_STATUS == m_baseBrakeTool->GetBrakeTestingStatus())) ||
	   GetParameterBool("DeveloperTest"))
	{	// Make sure some samples were taken
		if(m_espEndIndex > m_espStartIndex)
		{	// Read the data from the brake force array
			INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"),
																		   m_espStartIndex, m_espEndIndex);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == dataStatus)
			{	// Evaluate the reduction and recovery values
				// NOTE: The analyze methods report detailed results for each wheel
				for(UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
				{	// Find the actual start and end reduction indices.
					// The stored indices are absolute, whereas the wheel force array is relative to start of test
					INT32 reductionStart = m_espReduxRecovIndex[wheelIndex].reductionStart - m_espStartIndex;
					INT32 reductionEnd   = m_espReduxRecovIndex[wheelIndex].reductionEnd   - m_espStartIndex;

					Log(LOG_DEV_DATA, "%s Reduction -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
						reductionStart, reductionEnd);
					BEP_STATUS_TYPE reductionStatus = AnalyzeESPReductionForces(wheelIndex, reductionStart, reductionEnd);
					wheelReduxResults[wheelIndex] = BEP_STATUS_SUCCESS == reductionStatus ? testPass : testFail;
					// Find the actual start and end recovery indices.
					// The stored indices are absolute, whereas the wheel force array is relative to start of test
					INT32 recoveryStart = m_espReduxRecovIndex[wheelIndex].recoveryStart - m_espStartIndex;
					INT32 recoveryEnd   = m_espReduxRecovIndex[wheelIndex].recoveryEnd   - m_espStartIndex + endIndexExtentsion;

					Log(LOG_DEV_DATA, "%s Recovery -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
						recoveryStart, recoveryEnd);
					BEP_STATUS_TYPE recoveryStatus = AnalyzeESPBuildForces(wheelIndex, recoveryStart, recoveryEnd);
					wheelRecovResults[wheelIndex] = BEP_STATUS_SUCCESS == recoveryStatus ? testPass : testFail;
					// Update the overall results
					overallRedux = ((testPass == wheelReduxResults[wheelIndex]) && 
									((testPass == overallRedux) || (BEP_TESTING_STATUS == overallRedux))) ? overallRedux : testFail;
					overallRecov = 
					((testPass == wheelRecovResults[wheelIndex]) && 
					 ((testPass == overallRecov) || (BEP_TESTING_STATUS == overallRecov))) ? overallRecov : testFail;
					Log(LOG_DEV_DATA, "%s Reducion: %s, Recovery: %s\n", rollerName[wheelIndex].c_str(),
						wheelReduxResults[wheelIndex].c_str(), wheelRecovResults[wheelIndex].c_str());
				}
				// Determine the overall result of the testing
				testResult = ((overallRedux == testPass) && (overallRecov == testPass)) ? testPass : testFail;
				testResultCode = testResult == testPass ? "0000" : GetFaultCode("EspFailure");
				testDescription = testResult == testPass ? testDescription : GetFaultDescription("EspFailure");

			}
			else
			{	// Error reading brake force data
				Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("WheelForceDataReadError");
				testDescription = GetFaultDescription("WheelForceDataReadError");
			}
		}
		else
		{	// No samples were taken
			Log(LOG_ERRORS, "Not evaluating ESP reduction/recovery data - NO SAMPLES TAKEN!\n");
			testResult = testFail;
			testResultCode = GetFaultCode("NoESPData");
			testDescription = GetFaultDescription("NoESPData");
		}
		// Report the results of the test
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OverallReduction", overallRedux, "",
								 "OverallRecovery", overallRecov, "");
	}
	else
	{	// Need to skip this step
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping %s\n\t\tShort Circuit Test Step: %s\n\t\tTesting Status: %s\n\t\tDeveloper Test: %s\n",
			GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
			m_baseBrakeTool->GetBrakeTestingStatus().c_str(),
			GetParameterBool("DeveloperTest") ? "True" : "False");
	}
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit, result:%s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return testResult;
}


template <class ModuleType>
BEP_STATUS_TYPE MandoAbsTc<ModuleType>::AnalyzeESPBuildForces(INT32 roller, INT32 start, INT32 end)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;	   // the overall status of the analysis
	string testResult = testFail;  // the test status for the check
	string result = "999";		  // the result of the individual wheel for display
	string resultBG = "Blue";	  // the status (background color) of the individual wheel for display
	DATAARRAY tempDataArray;	  // Used to hold the Force values from a single wheel
	//bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
	//bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

	Log(LOG_FN_ENTRY, "MandoAbsTc::AnalyzeESPBuildForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
		rollerName[roller].c_str(), start, end);

	if(end > start)				  // if samples taken
	{
		// get the machine losses for this wheel
		float machineLoss = SystemReadFloat(GetDataTag(rollerName[roller] + "MachineLoss"));
		// Get the Wheel Force Array Data For a Single Wheel
		m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
		// find the maximum value in the range provided
		DATAARRAYITR maxItr = max_element(tempDataArray.begin()+start, tempDataArray.begin()+end);
		// Subtract out the machine loss to find the actual force
		float buildValue = *maxItr - machineLoss;
		Log(LOG_DEV_DATA, "ESP raw force measured = %f\n", buildValue);

// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
		/*if (true == useNewDrag)
		{
// this can be moved to config file eventually
			const int dragSamplesRequested = 5;

			int dragStart, dragEnd, dragSamples;
			float dragSum = 0.0;

			if (start > dragSamplesRequested)
			{
				Log(LOG_DEV_DATA,"ESP plenty of room for drag resample\n");

				dragEnd = start -1;
				dragStart = dragEnd - dragSamplesRequested;
			}
			else
			{
				Log(LOG_DEV_DATA,"ESP no room for drag resample\n");
				dragStart = 0;
				dragEnd = start - 1;
				if (dragEnd < 1) dragEnd = 1;

			}
			Log(LOG_DEV_DATA,"ESP dragStart = %d, dragEnd = %d\n", 
				dragStart, dragEnd);

			dragSamples = dragEnd - dragStart;

			if (dragSamplesRequested != dragSamples)
				Log(LOG_ERRORS,"ESP Valve firing early, number of drag samples reduced!\n");

			Log(LOG_DEV_DATA,
				"Using Recalculated Drag for ESP evaluation. %d samples requested, %d used.\n",
				dragSamplesRequested, dragSamples);

			for (DATAARRAYITR fPtr = tempDataArray.begin()+dragStart; 
				fPtr != tempDataArray.begin()+dragEnd; 
				fPtr++)
			{
				dragSum += *fPtr;
			}

			m_newDrags[roller] = dragSum / dragSamples;
			Log(LOG_DEV_DATA,
				"dragStart = %d, dragEnd = %d, total = %f, average = %f\n",
				dragStart, dragEnd, dragSum, m_newDrags[roller]);
			buildValue -= m_newDrags[roller];

		}

		else
		{ // if we did the above, don't even check this
			if (includeDrag == false)
			{
				buildValue -= m_baseBrakeTool->GetDragForceValue(roller);
			}
		}*/
		// get the parameter
		float buildMinValue = GetParameterFloat(rollerName[roller]+"MinESPBuildForce");

		if(buildValue > buildMinValue)	   // if value passed
		{
			testResult = testPass;
			resultBG = colorGreen;
		}
		else				 // value failed
		{
			testResult = testFail;
			resultBG = colorRed;
		}

		Log(LOG_DEV_DATA, "AnalyzeESPBuildForces Build -- Limit: %.2f, Value: %.2f\n", buildMinValue, buildValue);
		char temp[256];
		result = CreateMessage(temp,sizeof(temp),"%7.2f",buildValue);

		// update the GUI
		SystemWrite(GetDataTag(rollerName[roller] + "ESPBuildValue"),result);        
		SystemWrite(GetDataTag(rollerName[roller] + "ESPBuildBGColor"), resultBG);

		// Update the fault data to be sent with the test result
		string faultTag = rollerName[roller]+"ESPBuildFail";
		string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
		string faultDesc = (testResult == testPass ? "ESP Build Test" : GetFaultDescription(faultTag));
		// send the test results to the TestResultServer
		SendSubtestResultWithDetail(rollerName[roller] + "ESPBuild", testResult, faultDesc, testResultCode,
									rollerName[roller] + "ESPBuildValue", result, "lbf",
									rollerName[roller] + "ESPBuildMinForce", 
									CreateMessage(temp,sizeof(temp),"%7.2f",buildMinValue), "lbf");
		// Determine the status to return
		status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
	}
	else
	{
		status = BEP_STATUS_SKIP;
		Log(LOG_FN_ENTRY, "Skipping AnalyzeESPBuildForces -- End index is not greater than Start index!\n");
	}
	// Log the exit and return the status
	Log(LOG_FN_ENTRY, "MandoAbsTc::AnalyzeESPBuildForces() done: %s\n", testResult.c_str());
	return(status);    
}

template <class ModuleType>
BEP_STATUS_TYPE MandoAbsTc<ModuleType>::AnalyzeESPReductionForces(INT32 roller, INT32 start, INT32 end)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;	   // the overall status of the analysis
	string testResult = testFail;  // the test status for the check
	string result = "999";		  // the result of the individual wheel for display
	string resultBG = "Blue";	  // the status (background color) of the individual wheel for display
	DATAARRAY tempDataArray;	  // Used to hold the Force values from a single wheel
	//bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
	//bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

	Log(LOG_FN_ENTRY, "MandoAbsTc::AnalyzeESPReductionForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
		rollerName[roller].c_str(), start, end);

	if(end > start)				  // if samples taken
	{
		// get the machine losses for this wheel
		float machineLoss = SystemReadFloat(GetDataTag(rollerName[roller] + "MachineLoss"));
		// Get the Wheel Force Array Data For a Single Wheel
		m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
		// find the minimum value in the range provided
		DATAARRAYITR minItr = min_element(tempDataArray.begin()+start, tempDataArray.begin()+end);
		// Subtract out the machine loss to find the actual force
		float reductionValue = *minItr - machineLoss;
		/*if (true == useNewDrag)
		// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
		{
// note that if this option is selected, you MUST call AnalyzeESPBuildForces()
//      before calling AnalyzeESPReductionForces()

// subtract the calculated new drag value from the max point
			reductionValue -= m_newDrags[roller];
		}

		else
		{ // if we did the above, don't even check this
			if (includeDrag == false)
			{
				reductionValue -= m_baseBrakeTool->GetDragForceValue(roller);
			}
		}*/
		// get the parameter
		float reductionMaxValue = GetParameterFloat(rollerName[roller]+"MaxESPReductionForce");

		// range check
		if(reductionValue < 0)	reductionValue = 0;

		if(reductionValue < reductionMaxValue)	 // if value passed
		{
			testResult = testPass;
			resultBG = colorGreen;
		}
		else				 // value failed
		{
			testResult = testFail;
			resultBG = colorRed;
		}

		Log(LOG_DEV_DATA, "AnalyzeESPReductionForces Reduction -- Limit: %.2f, Value: %.2f\n", reductionMaxValue, reductionValue);
		char temp[256];
		result = CreateMessage(temp,sizeof(temp),"%7.2f",reductionValue);

		// update the GUI
		SystemWrite(GetDataTag(rollerName[roller] + "ESPReductionValue"),result);        
		SystemWrite(GetDataTag(rollerName[roller] + "ESPReductionBGColor"), resultBG);

		// Update the fault data to be sent with the test result
		string faultTag = rollerName[roller]+"ESPReductionFail";
		string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
		string faultDesc = (testResult == testPass ? "ESP Reduction Test" : GetFaultDescription(faultTag));
		// send the test results to the TestResultServer
		SendSubtestResultWithDetail(rollerName[roller] + "ESPReduction", testResult, faultDesc, testResultCode,
									rollerName[roller] + "ESPReductionValue", result, "lbf",
									rollerName[roller] + "ESPReductionMaxForce", 
									CreateMessage(temp,sizeof(temp),"%7.2f",reductionMaxValue), "lbf");
		// Determine the status to return
		status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
	}
	else
	{
		status = BEP_STATUS_SKIP;
		Log(LOG_FN_ENTRY, "Skipping AnalyzeESPReductionForces -- End index is not greater than Start index!\n");
	}
	// Log the exit and return the status
	Log(LOG_FN_ENTRY, "MandoAbsTc::AnalyzeESPReductionForces() done: %s\n", testResult.c_str());
	return(status);    
}


template <class ModuleType>
const std::string MandoAbsTc<ModuleType>::LfSensorTest(const std::string &value)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResult      = BEP_FAIL_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	float rollSpeeds[4];
	WheelSpeeds_t moduleSpeeds;
	std::string sensorResult;

	// delay time before starting the roll
	UINT32 startDelay = GetParameterInt("SensorTestStartDelay");
	// sensor test speed
	std::string sensorTestSpeed = GetParameter("SensorTestSpeed");  
	float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");  

	Log(LOG_DEV_DATA, "TataBrakeTCTemplate<ModuleType>::LfSensorTest() - Enter\n");  

	try
	{
		UpdatePrompts();			   // "Shift to neutral and release brake"
		BposSleep(startDelay);		   // give driver time to shift

		// Command all rolls to speed mode
		m_MotorController.Write(std::string("LeftFrontMotorMode"),std::string("Speed"), false);
		m_MotorController.Write(std::string("RightFrontMotorMode"),std::string("Speed"), false);
		m_MotorController.Write(std::string("LeftRearMotorMode"),std::string("Torque"), false);
		m_MotorController.Write(std::string("RightRearMotorMode"),std::string("Torque"), false);
		// Command the left front roll to the sensor test speed
		m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), false);
		m_MotorController.Write(std::string("LeftRearTorqueValue"),std::string("0"), false);
		m_MotorController.Write(std::string("RightRearTorqueValue"),std::string("0"), true);

		std::string tag, value;
		while(m_MotorController.GetNext(tag, value) > 0)
		{
			Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
		}

		// wait for the left front roll to reach the sensor test speed
		rollSpeeds[LFWHEEL] = 0;
		while(rollSpeeds[LFWHEEL] < sensorTestSpeedVal)
		{
			BposSleep(100);
			m_baseBrakeTool->GetISpeeds(rollSpeeds);
			if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
		}

		// Ask the module for sensor speeds
		status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds);

		if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
		{
			Log(LOG_DEV_DATA, "\n\tLF - Sensor: %f, Rolls: %f\n\tRF - Sensor: %f, Rolls: %f\n\tLR - Sensor: %f, Rolls: %f\n\tRR - Sensor: %f, Rolls: %f\n",
				moduleSpeeds[LFWHEEL], rollSpeeds[LFWHEEL],
				moduleSpeeds[RFWHEEL], rollSpeeds[RFWHEEL],
				moduleSpeeds[LRWHEEL], rollSpeeds[LRWHEEL],
				moduleSpeeds[RRWHEEL], rollSpeeds[RRWHEEL]);
			// make sure the left front sensor is reporting the highest speed
			if((moduleSpeeds[LFWHEEL] <= moduleSpeeds[RFWHEEL]) || (moduleSpeeds[LFWHEEL] <= moduleSpeeds[LRWHEEL]) ||
			   (moduleSpeeds[LFWHEEL] <= moduleSpeeds[RRWHEEL]))
			{
				testResult = testFail;
			}
			else testResult	= testPass;

			SendSubtestResult(string("LfSensorCross"),testResult,GetFaultDescription("LfSensorCross"),GetFaultCode("LfSensorCross"));

			// check the sensor reading against the minimum and maximum parameters
			float minimumSensorSpeed = GetParameterFloat("MinimumSensorSpeed");
			float maximumSensorSpeed = GetParameterFloat("MaximumSensorSpeed");
			char temp[16];

			if((moduleSpeeds[LFWHEEL] < minimumSensorSpeed) || (moduleSpeeds[LFWHEEL] > maximumSensorSpeed))
			{
				sensorResult = testFail;
				testDescription = GetFaultDescription("LfSensorSpeedFault");
				testResultCode = GetFaultCode("LfSensorSpeedFault");
			}
			else
			{
				sensorResult = testPass;
			}

			SendSubtestResultWithDetail(string("LfSensorSpeed"),sensorResult,testDescription,testResultCode,
										"LfSensorSpeed",CreateMessage(temp, sizeof(temp), "%.2f", moduleSpeeds[LFWHEEL]),"MPH",
										"LowerLimit",CreateMessage(temp, sizeof(temp), "%.2f",minimumSensorSpeed),"MPH",
										"UpperLimit",CreateMessage(temp, sizeof(temp), "%.2f",maximumSensorSpeed),"MPH");
		}
		else				 // error reading the sensors
		{
			SetCommunicationFailure(true);	   // set comm fault flag
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}

	if((testResult == testPass) && (sensorResult == testPass))	  // if the test step passed
	{
		SendTestResult(testPass,testDescription);
	}
	else					// test step failed
	{
		if(GetCommunicationFailure() == false)	 // if no comm error
		{
			SendTestResultWithDetail(testResult,testDescription,
									 GetFaultCode(testDescription),
									 GetFaultName(testDescription),
									 GetFaultDescription(testDescription));
		}
	}

	Log(LOG_DEV_DATA, "TataBrakeTCTemplate<ModuleType>::LfSensorTest() - Exit %s\n",testResult.c_str());

	return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const std::string MandoAbsTc<ModuleType>::RfSensorTest(const std::string &value)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResult      = BEP_FAIL_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	float rollSpeeds[4];
	WheelSpeeds_t moduleSpeeds;
	std::string sensorResult;

	// sensor test speed
	std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
	float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

	Log(LOG_DEV_DATA, "MandoAbsTc::RfSensorTest() - Enter\n");

	try
	{
		// Command the right front roll to the sensor test speed
		m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("LeftRearSpeedValue"),std::string("0"), false);
		m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);

		std::string tag, value;
		while(m_MotorController.GetNext(tag, value) > 0)
		{
			Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
		}

		// wait for the right front roll to reach the sensor test speed
		rollSpeeds[RFWHEEL] = 0;
		while(rollSpeeds[RFWHEEL] < sensorTestSpeedVal)
		{
			BposSleep(100);
			m_baseBrakeTool->GetISpeeds(rollSpeeds);
			if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
		}

		// Ask the module for sensor speeds
		status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds); 

		if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
		{
			Log(LOG_DEV_DATA, "\n\tLF - Sensor: %f, Rolls: %f\n\tRF - Sensor: %f, Rolls: %f\n\tLR - Sensor: %f, Rolls: %f\n\tRR - Sensor: %f, Rolls: %f\n",
				moduleSpeeds[LFWHEEL], rollSpeeds[LFWHEEL],
				moduleSpeeds[RFWHEEL], rollSpeeds[RFWHEEL],
				moduleSpeeds[LRWHEEL], rollSpeeds[LRWHEEL],
				moduleSpeeds[RRWHEEL], rollSpeeds[RRWHEEL]);
			// make sure the right front sensor is reporting a higher speed than
			// the other rolls besides the left front
			if((moduleSpeeds[RFWHEEL] <= moduleSpeeds[LRWHEEL]) || (moduleSpeeds[RFWHEEL] <= moduleSpeeds[RRWHEEL]))
			{
				testResult = testFail;
			}
			else testResult	= testPass;

			SendSubtestResult(string("RfSensorCross"),testResult,GetFaultDescription("RfSensorCross"),GetFaultCode("RfSensorCross"));

			// check the sensor reading against the minimum and maximum parameters
			float minimumSensorSpeed = GetParameterFloat("MinimumSensorSpeed");
			float maximumSensorSpeed = GetParameterFloat("MaximumSensorSpeed");
			char temp[16];

			if((moduleSpeeds[RFWHEEL] < minimumSensorSpeed) || (moduleSpeeds[RFWHEEL] > maximumSensorSpeed))
			{
				sensorResult = testFail;
				testDescription = GetFaultDescription("RfSensorSpeedFault");
				testResultCode = GetFaultCode("RfSensorSpeedFault");
			}
			else
			{
				sensorResult = testPass;
			}

			SendSubtestResultWithDetail(string("RfSensorSpeed"),sensorResult,testDescription,testResultCode,
										"RfSensorSpeed",CreateMessage(temp, sizeof(temp), "%.2f", moduleSpeeds[RFWHEEL]),"MPH",
										"LowerLimit",CreateMessage(temp, sizeof(temp), "%.2f",minimumSensorSpeed),"MPH",
										"UpperLimit",CreateMessage(temp, sizeof(temp), "%.2f",maximumSensorSpeed),"MPH");
		}
		else				 // error reading the sensors
		{
			SetCommunicationFailure(true);	   // set comm fault flag
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}

	if((testResult == testPass) && (sensorResult == testPass))	  // if the test step passed
	{
		SendTestResult(testPass,testDescription);
	}
	else					// test step failed
	{
		if(GetCommunicationFailure() == false)	 // if no comm error
		{
			SendTestResultWithDetail(testResult,testDescription,
									 GetFaultCode(testDescription),
									 GetFaultName(testDescription),
									 GetFaultDescription(testDescription));
		}
	}

	Log(LOG_DEV_DATA, "MandoAbsTc::RfSensorTest() - Exit %s\n",testResult.c_str());

	return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const std::string MandoAbsTc<ModuleType>::LrSensorTest(const std::string &value)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResult      = BEP_FAIL_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	float rollSpeeds[4];
	WheelSpeeds_t moduleSpeeds;
	std::string sensorResult;

	// delay time before starting the roll
	UINT32 startDelay = GetParameterInt("SensorTestStartDelay");
	// sensor test speed
	std::string sensorTestSpeed = GetParameter("SensorTestSpeed");  
	float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");  

	Log(LOG_DEV_DATA, "TataBrakeTCTemplate<ModuleType>::LrSensorTest() - Enter\n");  

	try
	{
		UpdatePrompts();			   // "Shift to neutral and release brake"
		BposSleep(startDelay);		   // give driver time to shift

		// Command all rolls to speed mode
		m_MotorController.Write(std::string("LeftFrontMotorMode"),std::string("Speed"), false);
		m_MotorController.Write(std::string("RightFrontMotorMode"),std::string("Speed"), false);
		m_MotorController.Write(std::string("LeftRearMotorMode"),std::string("Torque"), false);
		m_MotorController.Write(std::string("RightRearMotorMode"),std::string("Torque"), false);
		// Command the left front roll to the sensor test speed
		m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), false);
		m_MotorController.Write(std::string("LeftRearTorqueValue"),std::string("0"), false);
		m_MotorController.Write(std::string("RightRearTorqueValue"),std::string("0"), true);

		std::string tag, value;
		while(m_MotorController.GetNext(tag, value) > 0)
		{
			Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
		}

		// wait for the left front roll to reach the sensor test speed
		rollSpeeds[LFWHEEL] = 0;
		while(rollSpeeds[LFWHEEL] < sensorTestSpeedVal)
		{
			BposSleep(100);
			m_baseBrakeTool->GetISpeeds(rollSpeeds);
			if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
		}

		// Ask the module for sensor speeds
		status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds);

		if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
		{
			Log(LOG_DEV_DATA, "\n\tLF - Sensor: %f, Rolls: %f\n\tRF - Sensor: %f, Rolls: %f\n\tLR - Sensor: %f, Rolls: %f\n\tRR - Sensor: %f, Rolls: %f\n",
				moduleSpeeds[LFWHEEL], rollSpeeds[LFWHEEL],
				moduleSpeeds[RFWHEEL], rollSpeeds[RFWHEEL],
				moduleSpeeds[LRWHEEL], rollSpeeds[LRWHEEL],
				moduleSpeeds[RRWHEEL], rollSpeeds[RRWHEEL]);
			// make sure the left tandem sensor is reporting the highest speed
			if(moduleSpeeds[LRWHEEL] <= moduleSpeeds[RRWHEEL])
			{
				testResult = testFail;
			}
			else testResult	= testPass;

			SendSubtestResult(string("LrSensorCross"),testResult,GetFaultDescription("LrSensorCross"),GetFaultCode("LrSensorCross"));

			// check the sensor reading against the minimum and maximum parameters
			float minimumSensorSpeed = GetParameterFloat("MinimumSensorSpeed");
			float maximumSensorSpeed = GetParameterFloat("MaximumSensorSpeed");
			char temp[16];

			if((moduleSpeeds[LRWHEEL] < minimumSensorSpeed) || (moduleSpeeds[LRWHEEL] > maximumSensorSpeed))
			{
				sensorResult = testFail;
				testDescription = GetFaultDescription("LrSensorSpeedFault");
				testResultCode = GetFaultCode("LrSensorSpeedFault");
			}
			else
			{
				sensorResult = testPass;
			}

			SendSubtestResultWithDetail(string("LrSensorSpeed"),sensorResult,testDescription,testResultCode,
										"LrSensorSpeed",CreateMessage(temp, sizeof(temp), "%.2f", moduleSpeeds[LRWHEEL]),"MPH",
										"LowerLimit",CreateMessage(temp, sizeof(temp), "%.2f",minimumSensorSpeed),"MPH",
										"UpperLimit",CreateMessage(temp, sizeof(temp), "%.2f",maximumSensorSpeed),"MPH");
		}
		else				 // error reading the sensors
		{
			SetCommunicationFailure(true);	   // set comm fault flag
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}

	if((testResult == testPass) && (sensorResult == testPass))	  // if the test step passed
	{
		SendTestResult(testPass,testDescription);
	}
	else					// test step failed
	{
		if(GetCommunicationFailure() == false)	 // if no comm error
		{
			SendTestResultWithDetail(testResult,testDescription,
									 GetFaultCode(testDescription),
									 GetFaultName(testDescription),
									 GetFaultDescription(testDescription));
		}
	}

	Log(LOG_DEV_DATA, "TataBrakeTCTemplate<ModuleType>::LrSensorTest() - Exit %s\n",testResult.c_str());

	return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const std::string MandoAbsTc<ModuleType>::RrSensorTest(const std::string &value)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResult      = BEP_FAIL_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	float rollSpeeds[4];
	WheelSpeeds_t moduleSpeeds;
	std::string sensorResult;

	// sensor test speed
	std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
	float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

	Log(LOG_DEV_DATA, "MandoAbsTc::RrSensorTest() - Enter\n");

	try
	{
		// Command the right front roll to the sensor test speed
		m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
		m_MotorController.Write(std::string("LeftRearTorqueValue"),std::string("0"), false);
		m_MotorController.Write(std::string("RightRearTorqueValue"),std::string("0"), true);

		std::string tag, value;
		while(m_MotorController.GetNext(tag, value) > 0)
		{
			Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
		}

		// wait for the right front roll to reach the sensor test speed
		rollSpeeds[RFWHEEL] = 0;
		while(rollSpeeds[RFWHEEL] < sensorTestSpeedVal)
		{
			BposSleep(100);
			m_baseBrakeTool->GetISpeeds(rollSpeeds);
			if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
		}

		// Ask the module for sensor speeds
		status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds); 

		if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
		{
			Log(LOG_DEV_DATA, "\n\tLF - Sensor: %f, Rolls: %f\n\tRF - Sensor: %f, Rolls: %f\n\tLR - Sensor: %f, Rolls: %f\n\tRR - Sensor: %f, Rolls: %f\n",
				moduleSpeeds[LFWHEEL], rollSpeeds[LFWHEEL],
				moduleSpeeds[RFWHEEL], rollSpeeds[RFWHEEL],
				moduleSpeeds[LRWHEEL], rollSpeeds[LRWHEEL],
				moduleSpeeds[RRWHEEL], rollSpeeds[RRWHEEL]);
			// Sensor cross must be OK if LT passes
			testResult = testPass;

			SendSubtestResult(string("RrSensorCross"),testResult,GetFaultDescription("RrSensorCross"),GetFaultCode("RrSensorCross"));

			// check the sensor reading against the minimum and maximum parameters
			float minimumSensorSpeed = GetParameterFloat("MinimumSensorSpeed");
			float maximumSensorSpeed = GetParameterFloat("MaximumSensorSpeed");
			char temp[16];

			if((moduleSpeeds[RRWHEEL] < minimumSensorSpeed) || (moduleSpeeds[RRWHEEL] > maximumSensorSpeed))
			{
				sensorResult = testFail;
				testDescription = GetFaultDescription("RrSensorSpeedFault");
				testResultCode = GetFaultCode("RrSensorSpeedFault");
			}
			else
			{
				sensorResult = testPass;
			}

			SendSubtestResultWithDetail(string("RrSensorSpeed"),sensorResult,testDescription,testResultCode,
										"RrSensorSpeed",CreateMessage(temp, sizeof(temp), "%.2f", moduleSpeeds[RRWHEEL]),"MPH",
										"LowerLimit",CreateMessage(temp, sizeof(temp), "%.2f",minimumSensorSpeed),"MPH",
										"UpperLimit",CreateMessage(temp, sizeof(temp), "%.2f",maximumSensorSpeed),"MPH");
		}
		else				 // error reading the sensors
		{
			SetCommunicationFailure(true);	   // set comm fault flag
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}

	if((testResult == testPass) && (sensorResult == testPass))	  // if the test step passed
	{
		SendTestResult(testPass,testDescription);
	}
	else					// test step failed
	{
		if(GetCommunicationFailure() == false)	 // if no comm error
		{
			SendTestResultWithDetail(testResult,testDescription,
									 GetFaultCode(testDescription),
									 GetFaultName(testDescription),
									 GetFaultDescription(testDescription));
		}
	}

	Log(LOG_DEV_DATA, "MandoAbsTc::RrSensorTest() - Exit %s\n",testResult.c_str());

	return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string MandoAbsTc<ModuleType>::WarningLight(void)
{   
	Log(LOG_FN_ENTRY, "MandoAbsTc::WarningLight() - Enter");
	string testResult = testPass;
	if(!ShortCircuitTestStep())
	{
		string testResultCode = "0000";
		string testDescription= GetTestStepInfo("Description");

		DisplayPrompt(1,"ReceiptWarningLight",true);
		BposSleep(4000);
		RemovePrompts();

		// Log the result and report to the test result server
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MandoAbsTc::WarningLight() - Exit");
	return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
void MandoAbsTc<ModuleType>::InitializeHook(const XmlNode *config)
{
	Log(LOG_FN_ENTRY, "Entering MandoAbsTc::InitializeHook\n");
	// call parent initialize hook
	GenericABSTCTemplate<ModuleType>::InitializeHook(config);
	// These are always four channels
	SetFourChannelSystem(true);

	// Create a new array of reduction/recovery indices
	m_espReduxRecovIndex.reserve(MAXWHEELS);
	m_espReduxRecovIndex.resize(MAXWHEELS);
}
