//*************************************************************************
// Description:
// This file contains the functions necessary to test the vehicle.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MazdaBrakeTC.h"
#include <numeric>

//-------------------------------------------------------------------------------------------------
MazdaBrakeTC::MazdaBrakeTC() : GenericBaseBrakeTC()
{   // Nothing special to do here
    
}

//-------------------------------------------------------------------------------------------------
MazdaBrakeTC::~MazdaBrakeTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::Abort(void)
{
	DisableRollMotors(SPEED_MODE);
	GenericBaseBrakeTC::Abort();
}

//-------------------------------------------------------------------------------------------------
const string MazdaBrakeTC::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::CommandTestStep(%s) - Enter", value.c_str());
    // Make sure it is OK to perform the test step
    m_baseBrakeTool->SetBrakeTestingStatus(BEP_TESTING_RESPONSE);
    if(BEP_STATUS_SUCCESS == StatusCheck())
    {
		if(!GetTestStepName().compare("AbsLfTest"))                       testResult = MazdaAbsValveTest("LfAbsTest", LFWHEEL, RFWHEEL);
		else if(!GetTestStepName().compare("AbsLrTest"))                  testResult = MazdaAbsValveTest("LrAbsTest", LRWHEEL, RRWHEEL);
		else if(!GetTestStepName().compare("AbsRfTest"))                  testResult = MazdaAbsValveTest("RfAbsTest", RFWHEEL, LFWHEEL);
		else if(!GetTestStepName().compare("AbsRrTest"))                  testResult = MazdaAbsValveTest("RrAbsTest", RRWHEEL, LRWHEEL);
		else if(!GetTestStepName().compare("BrakeTestComplete"))          testResult = BrakeTestingComplete();
		else if(!GetTestStepName().compare("BrakeTestStart"))
		{
			DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
			testResult = SystemWrite(GetDataTag("RunBrakeTestTag"), true) == BEP_STATUS_SUCCESS ? testPass : testFail;
		}
		else if(!GetTestStepName().compare("BrakeTestStop"))
		{
			RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
			testResult = SystemWrite(GetDataTag("RunBrakeTestTag"), false) == BEP_STATUS_SUCCESS ? testPass : testFail;
		}
		else if(!GetTestStepName().compare("ClearDiagnosticRoutine"))     testResult = ClearDiagnostics();
		else if(!GetTestStepName().compare("FaultCheckRoutine"))          testResult = FaultCheck();
		else if(!GetTestStepName().compare("MazdaFrontBrakeForce"))       testResult = MazdaBrakeForceTest("Front");
		else if(!GetTestStepName().compare("MazdaRearBrakeForce"))        testResult = MazdaBrakeForceTest("Rear");
		else if(!GetTestStepName().compare("MazdaDragTest"))              testResult = MazdaDragTest();
		else if(!GetTestStepName().compare("MazdaParkBrakeForce"))        testResult = MazdaBrakeForceTest("ParkBrake");
		else if(!GetTestStepName().compare("ReportOverallBrakeResults"))  testResult = ReportOverallBrakeResults();
		else if(!GetTestStepName().compare("SpeedSensorCheck"))           testResult = SpeedSensorCheck();
		else if(!GetTestStepName().compare("StartBrakeSwitchTest"))       testResult = MazdaBrakeSwitchTest("Start");
		else if(!GetTestStepName().compare("StopBrakeSwitchTest"))        testResult = MazdaBrakeSwitchTest("Stop");
		else if(!GetTestStepName().compare("StopRollers"))            
		{
			testResult = (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(0.0, 0.0, 0.0, 0.0)) ? testPass : testFail;
		}
		else if(!GetTestStepName().compare("TestHeadWait"))               testResult = WaitForMazdaTester(BposReadInt(value.c_str()));
		else 
			testResult = GenericBaseBrakeTC::CommandTestStep(value);
    }
    else
    {   // System is not OK to perform brake test
        testResult = testSkip;
        Log(LOG_ERRORS, "System status is preventing test: %s", ConvertStatusToResponse(StatusCheck()).c_str());
		// Make sure rolls are disabled
		DisableRollMotors(SPEED_MODE);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::CommandTestStep(%s) - Exit", value.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::Initialize(const XmlNode *config)
{   // Call the base class to begin the initialization
	GenericBaseBrakeTC::Initialize(config);
	// Load the Mazda test head test steps
	try
	{
		m_testHeadTestSteps.DeepCopy(config->getChild("Setup/Parameters/MazdaTestHeadTestSteps")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "ERROR: Could not load Mazda Test Head Test Steps: %s", excpt.GetReason());
	}
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::BrakeTestingComplete(void)
{
	DisableRollMotors(SPEED_MODE, false);
	DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
				  GetPromptPriority("RemoveFootFromBrake"));
	return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::ClearDiagnostics(void)
{
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::ClearDiagnostics() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{
		DisplayPrompt(GetPromptBox("ClearDiagnostics"), GetPrompt("ClearDiagnostics"), GetPromptPriority("ClearDiagnostics"));
		result = PerformTestHeadTest(GetTestStepInfo("TestHeadTestName"), true);
		Log(LOG_DEV_DATA, "Diagnostic clear routine completed, result: %s", result.c_str());
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
		RemovePrompt(GetPromptBox("ClearDiagnostics"), GetPrompt("ClearDiagnostics"), GetPromptPriority("ClearDiagnostics"));
	}
	else
	{
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping diagnostic clear routine");
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::ClearDiagnostics() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::FaultCheck(void)
{
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::FaultCheck() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{
		DisplayPrompt(GetPromptBox("VehicleFaultCheck"), GetPrompt("VehicleFaultCheck"), GetPromptPriority("VehicleFaultCheck"));
		result = PerformTestHeadTest(GetTestStepInfo("TestHeadTestName"), true);
		Log(LOG_DEV_DATA, "Fault Check routine completed, result: %s", result.c_str());
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
		RemovePrompt(GetPromptBox("VehicleFaultCheck"), GetPrompt("VehicleFaultCheck"), GetPromptPriority("VehicleFaultCheck"));
	}
	else
	{
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping fault check");
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::FaultCheck() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaAbsValveTest(string wheelTestStep, INT16 testingWheel, INT16 oppositeWheel)
{   // Log the entry and determine if this test should be performed
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAbsValveTest(wheelTestStep: %s) - Enter", wheelTestStep.c_str());
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{   // Kick off the test head test step
		DisplayPrompt(GetPromptBox("AbsValveFiring"), GetPrompt("AbsValveFiring"), GetPromptPriority("AbsValveFiring"),
					  "white", rollerName[testingWheel]);
		SystemWrite(GetDataTag("AbsDumpOk"), false);
		result = StartTestHeadTest(wheelTestStep);
		Log(LOG_DEV_DATA, "Commanded Mazda to run %s", wheelTestStep.c_str());
		// Wait for the ABS dump to complete
		string dumpResult = WaitForAbsDump(testingWheel, oppositeWheel);
		Log(LOG_DEV_DATA, "ABS Dump complete: %s", dumpResult.c_str());
		string buildResult(testFail);
		if(!dumpResult.compare(testPass))
		{   // Wait for the ABS Build to complete
			BposSleep(GetParameterInt("AbsBuildDelay"));
			buildResult = WaitForAbsBuild(testingWheel, oppositeWheel);
			Log(LOG_DEV_DATA, "ABS Build complete: %s", buildResult.c_str());
		}
		else
		{
			SystemWrite(GetDataTag("RunBrakeTestTag"), false);
		}
		// Determine the overall result
		result = (!dumpResult.compare(testPass) && !buildResult.compare(testPass)) ? testPass : testFail;
		// Report the results
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
		// Wait for a bit to make sure no status bits get stomped
		BposSleep(GetParameterInt("TestHeadStatusBitDelay"));
		// Clear the start command
		StopTestHeadTest(wheelTestStep);
		Log(LOG_DEV_DATA, "Stopped test head test");
		SystemWrite(GetDataTag("AbsDumpOk"), false);
		SystemWrite(GetDataTag("AbsBuildOk"), false);
		SystemWrite(GetDataTag("AbsDumpNok"), false);
		SystemWrite(GetDataTag("AbsBuildNok"), false);
		RemovePrompt(GetPromptBox("AbsValveFiring"), GetPrompt("AbsValveFiring"), GetPromptPriority("AbsValveFiring"));
	}
	else
	{
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping ABS valve test");
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAbsValveTest(wheelTestStep: %s) - Exit", wheelTestStep.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaBrakeForceTest(string axle)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeForceTest(%s) - Enter", axle.c_str());
	INT16 leftWheel = LFWHEEL;
	INT16 rightWheel = RFWHEEL;
	ResultType_t resultType = BRAKE_RESULTS_FRONT;
	string applyBrakePrompt("ApplyBrake");
	if(!axle.compare("Front"))
	{
		leftWheel = LFWHEEL;
		rightWheel = RFWHEEL;
		resultType = BRAKE_RESULTS_FRONT;
	}
	else if(!axle.compare("Rear"))
	{
		leftWheel = LRWHEEL;
		rightWheel = RRWHEEL;
		resultType = BRAKE_RESULTS_REAR;
	}
	else if(!axle.compare("ParkBrake"))
	{
		leftWheel = LRWHEEL;
		rightWheel = RRWHEEL;
		resultType = PARK_BRAKE_RESULTS;
		applyBrakePrompt = "ApplyParkBrake";
		DisplayPrompt(1, GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
	}
    string result(BEP_TESTING_STATUS);
	string loadCellTags[] = {GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"), 
							 GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce")};
	string displayTags[] = {GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"), 
							GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag")};
	MaxBrakeData brakeData[GetRollerCount()];
	TestResultDetails testDetails;
    if(!ShortCircuitTestStep())
    {   // Setup the roller speeds
		if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat(axle+"AxleBrakeForceSpeedLf"),
															   GetParameterFloat(axle+"AxleBrakeForceSpeedRf"),
															   GetParameterFloat(axle+"AxleBrakeForceSpeedLr"),
															   GetParameterFloat(axle+"AxleBrakeForceSpeedRr")))
		{   // Tell the operator to apply the brake pedal
			DisplayPrompt(GetPromptBox(applyBrakePrompt), GetPrompt(applyBrakePrompt), GetPromptPriority(applyBrakePrompt));
			BposSleep(GetParameterInt("OperatorReactionTime"));
			if(!axle.compare("ParkBrake"))
			{   // Remove the prompts
				RemovePrompt(1, GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
			}
			DisplayPrompt(GetPromptBox("BrakeForceTesting"), GetPrompt("BrakeForceTesting"), GetPromptPriority("BrakeForceTesting"),
						  "white", axle);
			// Setup the brake data structure
			SetupLoadCellDataStructure(brakeData, loadCellTags, displayTags);
			bool measurementComplete = false;
			do
			{   // Get the drag force samples
				measurementComplete = MonitorBrakeForces(leftWheel, rightWheel, 0, 0, brakeData, 
														 GetParameterInt("NumberOfBrakeSamples"));
			} while(!measurementComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
			// Analyze the exit status
			if(measurementComplete)
			{   // Calculate the limits
				float sumLimit = GetAxleWeight(axle) * GetParameterFloat(axle+"BrakeLimit");

				// Measurement complete, analyze the brake
				result = AnalyzeForceResults(brakeData, leftWheel, rightWheel, resultType, 
											 sumLimit,
											 GetParameterFloat(axle+"BrakeDiffLimit"), LESS, 
											 testDetails, axle,
											 GetDataTag(axle+"LeftForceValue"), GetDataTag(axle+"RightForceValue"),
											 GetDataTag(axle+"ForceSumValue"), GetDataTag(axle+"ForceDiffValue"),
											 GetDataTag(axle+"ForceSumBgColor"), GetDataTag(axle+"ForceDiffBgColor"));
				Log(LOG_DEV_DATA, "Overall brake test result: %s", result.c_str());
			}
			else
			{
				result = testFail;
				Log(LOG_ERRORS, "Brake testing failed because measurements were not completed");
			}
			RemovePrompt(GetPromptBox("BrakeForceTesting"), GetPrompt("BrakeForceTesting"), GetPromptPriority("BrakeForceTesting"));
		}
		else
		{   // Timeout waiting for rollers to accelerate to speed
			Log(LOG_ERRORS, "Timeout waiting for rollers to be at speed");
			result = testFail;
		}
		// NOTE: Do not disable the motors here as the same speed is required for ABS valve testing
		// Report the results
		SendTestResultWithDetail(result, testDetails, GetTestStepInfo("Description"), "0000");
		if(!axle.compare("ParkBrake"))
		{   // Remove the prompts
			RemovePrompt(1, GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
			RemovePrompt(GetPromptBox(applyBrakePrompt), GetPrompt(applyBrakePrompt), GetPromptPriority(applyBrakePrompt));
			DisplayTimedPrompt(GetPrompt("ReleaseParkBrake"), GetPromptBox("ReleaseParkBrake"), 
							   GetPromptPriority("ReleaseParkBrake"), GetPromptDuration("ReleaseParkBrake"));
		}
	}
	else
	{   // Need to skip this step
		result = testSkip;
		Log(LOG_DEV_DATA, "Skipping %s Mazda Brake Force Test test", axle.c_str());
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeForceTest() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaBrakeSwitchTest(const string &action)
{
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeSwitchTest(action: %s) - Enter", action.c_str());
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{   // Prompt the operator to remove foot from brake pedal
		if(!action.compare("Start"))
		{
			DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
			DisplayPrompt(GetPromptBox("SpeedSensorCheck"), GetPrompt("SpeedSensorCheck"), GetPromptPriority("SpeedSensorCheck"));
		}
		else
		{
			DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
		}
		if(GetParameterBool("WaitForOperator"))
		{
			BposSleep(GetParameterInt("OperatorReactionTime"));
		}
		// Run the test head test
		result = PerformTestHeadTest(GetTestStepInfo("TestHeadTestName"), !action.compare("Start"));
		Log(LOG_DEV_DATA, "Test Head Brake switch test % completed, result: %s", action.c_str(), result.c_str());
		// Report the result
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
		if(!action.compare("Start"))
		{
			RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
			RemovePrompt(GetPromptBox("SpeedSensorCheck"), GetPrompt("SpeedSensorCheck"), GetPromptPriority("SpeedSensorCheck"));
		}
		else
		{
			RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
		}
		// Disable the rollers
//		DisableRollMotors(SPEED_MODE);
	}
	else
	{   // Do not need to run this test
		result = testSkip;
		Log(LOG_FN_ENTRY, "Skipping brake switch test");
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeSwitchTest(action: %s) - Exit", action.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaDragTest(void)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaDragTest() - Enter");
    string result(BEP_TESTING_STATUS);
	string loadCellTags[] = {GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"), 
		                     GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce")};
	string displayTags[] = {GetDataTag("LfDragDisplayTag"), GetDataTag("RfDragDisplayTag"), 
		                    GetDataTag("LrDragDisplayTag"), GetDataTag("RrDragDisplayTag")};
    if(!ShortCircuitTestStep())
    {   // Make sure the operator keeps foot off brake
		DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
		DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
		if(GetParameterBool("WaitForOperator"))
		{
			BposSleep(GetParameterInt("OperatorReactionTime"));
		}
		TestResultDetails testDetails;
		// Command the rollers to the test speed
		if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("LfDragTestSpeed"), 
															   GetParameterFloat("RfDragTestSpeed"), 
															   GetParameterFloat("LrDragTestSpeed"), 
															   GetParameterFloat("RrDragTestSpeed")))
		{   // Roller speeds are good, take drag force readings
			RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
			DisplayPrompt(GetPromptBox("DragForceTest"), GetPrompt("DragForceTest"), GetPromptPriority("DragForceTest"));
			MaxBrakeData dragData[GetRollerCount()];
			// Setup the drag data structure
			SetupLoadCellDataStructure(dragData, loadCellTags, displayTags);
			bool measurementComplete = false;
			do
			{   // Get the drag force samples
				measurementComplete = MonitorBrakeForces(LFWHEEL, RRWHEEL, 0.0, 0.0, dragData, 
														 GetParameterInt("NumberOfDragSamples"));
			} while(!measurementComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
			// Analyze the exit status
			if(measurementComplete)
			{   // Measurement complete, analyze the data
				string axle = "Front";
				string frontResult = AnalyzeForceResults(dragData, LFWHEEL, RFWHEEL, DRAG_RESULTS_FRONT,
														 GetParameterFloat("FrontDragLimit"),
														 GetParameterFloat("FrontDragDiffLimit"), GREATER,
														 testDetails, axle,
														 GetDataTag("LFDragValue"), GetDataTag("RFDragValue"),
														 GetDataTag("FrontDragSum"), GetDataTag("FrontDragBalance"),
														 GetDataTag("FrontDragSumBgColor"), GetDataTag("FrontDragBalanceBgColor"));
				axle = "Rear";
				string rearResult = AnalyzeForceResults(dragData, LRWHEEL, RRWHEEL, DRAG_RESULTS_REAR, 
														GetParameterFloat("RearDragLimit"),
														GetParameterFloat("RearDragDiffLimit"), GREATER, 
														testDetails, axle,
														GetDataTag("LRDragValue"), GetDataTag("RRDragValue"),
														GetDataTag("RearDragSum"), GetDataTag("RearDragBalance"),
														GetDataTag("RearDragSumBgColor"), GetDataTag("RearDragBalanceBgColor"));
				result = (!frontResult.compare(testPass) && !rearResult.compare(testPass)) ? testPass : testFail;
				Log(LOG_DEV_DATA, "Overall drag test result: %s", result.c_str());
			}
			else
			{
				result = testFail;
				Log(LOG_ERRORS, "Drag testing failed because measurements were not completed");
			}
			RemovePrompt(GetPromptBox("DragForceTest"), GetPrompt("DragForceTest"), GetPromptPriority("DragForceTest"));
		}
		else
		{   // Timeout waiting for rollers to accelerate to speed
			RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
			Log(LOG_ERRORS, "Timeout waiting for rollers to be at speed");
			result = testFail;
		}
		if(GetParameterBool("DisableMotorsAfterDragTest"))
		{
			DisableRollMotors(SPEED_MODE);
		}
		RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
		// Report the results
		SendTestResultWithDetail(result, testDetails, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this step
        result = testSkip;
        Log(LOG_DEV_DATA, "Skipping MazdaDragTest test");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaDragTest() - Exit");
    return result;
}


//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::PerformTestHeadTest(const string &testStep, bool waitForTestComplete/*=true*/)
{
	Log(LOG_FN_ENTRY, "Performing test head test: %s", testStep.c_str());
	string result(BEP_ERROR_RESPONSE);
	bool testComplete = false;
	// Reset the test start time
	SetStartTime();
	if(StartTestHeadTest(testStep))
	{   
		if(waitForTestComplete)
		{   // Wait for the test step to complete
			const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
			while(!testComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
			{   // Wait for a bit for the test step to complete
				BposSleep(GetParameterInt("TestCompleteCheckDelay"));
				// Check if the test has completed yet
				testComplete = SystemReadBool(testData->second->getAttribute("TestCompleteTag")->getValue());
			}
			if(testComplete)
			{   // Test step completed, set the result
				result = SystemReadBool(testData->second->getAttribute("TestResultTag")->getValue()) ? testPass : testFail;
				Log(LOG_DEV_DATA, "Test step %s completed, result: %s", testStep.c_str(), result.c_str());
			}
			else
			{   // Timeout waiting for the test to complete
				result = testTimeout;
				Log(LOG_ERRORS, "Timeout waiting for %s to complete", testStep.c_str());
			}
			// Disable the test request
			StopTestHeadTest(testStep);
		}
		else
		{   // Not waiting for test complete
			result = testPass;
		}
	}
	else
	{   // Test step has not been defined
		Log(LOG_ERRORS, "Test step %s has not been defined for the test head", testStep.c_str());
		result = testSoftwareFail;
	}
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::ReportOverallBrakeResults(void)
{   // Report the brake sum
	float totalBrakeForce = m_axleBrakeResults[FRONT_AXLE].axleSum + m_axleBrakeResults[REAR_AXLE].axleSum;
	float totalAXleWeight = GetAxleWeight("Front") + GetAxleWeight("Rear");
	string totalBrakeResult(BEP_TESTING_RESPONSE);
	if(GetParameterBool("UseAxleWeightsForTotalBrakeForceResult"))
	{
		totalBrakeResult = (totalBrakeForce > (totalAXleWeight * GetParameterFloat("TotalBrakeLimit"))) ? testPass : testFail;
	}
	else
	{
		totalBrakeResult = (!m_axleBrakeResults[FRONT_AXLE].axleResult.compare(testPass) &&
							!m_axleBrakeResults[REAR_AXLE].axleResult.compare(testPass)) ? testPass : testFail;
	}
	// Report these value to the PLC
	SystemWrite(GetDataTag("TotalBrakeForceResultTag"), !totalBrakeResult.compare(testPass));
	SystemWrite(GetDataTag("TotalBrakeForceTag"), totalBrakeForce);
	// Determine the overall test result
	SystemWrite(GetDataTag("OverallTestResultTag"), !GetOverallResult().compare(testPass) || !GetOverallResult().compare(BEP_TESTING_RESPONSE));
	Log(LOG_DEV_DATA, "Reporting Total Force: %.2f - Result: %s", totalBrakeForce, totalBrakeResult.c_str());
	return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::SpeedSensorCheck(void)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep())
	{   // Instruct operator to remove foot from brake pedal
		DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
		DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
		BposSleep(GetParameterInt("OperatorReactionTime"));
		// Spin up the rollers
		if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("LfSpeedSensorCheckSpeed"), 
															   GetParameterFloat("RfSpeedSensorCheckSpeed"), 
															   GetParameterFloat("LrSpeedSensorCheckSpeed"), 
															   GetParameterFloat("RrSpeedSensorCheckSpeed")))
		{   
			testResult = testPass;
			StatusSleep(GetParameterInt("SpeedSensorTestTime"));
		}
		else
		{   // Timeout waiting for rollers to accelerate to speed
			Log(LOG_ERRORS, "Timeout waiting for rollers to be at speed");
			testResult = testFail;
		}
		// Report the results
		SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
		RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
		RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping Speed sensor test");
		testResult = testSkip;
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Exit");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::WaitForMazdaTester(const INT32 &waitTime)
{
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForMazdaTester(wait time: %d) - Enter");
	if(!ShortCircuitTestStep())
	{
		result = (BEP_STATUS_SUCCESS == StatusSleep(waitTime)) ? testPass : testFail;
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping Mazda tester wait period");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForMazdaTester(wait time: %d) - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::AnalyzeForceResults(MaxBrakeData *forceData, const INT16 &leftWheel, const INT16 &rightWheel, 
										 const ResultType_t &resultType, const float &sumLimit, const float &diffLimit, 
										 Comparison_t comparison, TestResultDetails &details, string &axle,
										 string leftDisplayTag, string rightDisplayTag, 
										 string sumDisplayTag, string diffDisplayTag,
										 string sumBgColorTag, string diffBgColorTag)
{
	string result(BEP_TESTING_RESPONSE);
	DataAnalysis analyze;
	float leftAvg = 0.0;
	float rightAvg = 0.0;
	// Get the average of the wheels
	if(forceData[leftWheel].forceSamples.size() > 0)
	{
		leftAvg = accumulate(forceData[leftWheel].forceSamples.begin(), forceData[leftWheel].forceSamples.end(), 0.0);
		leftAvg /= forceData[leftWheel].forceSamples.size();
	}
	if(forceData[rightWheel].forceSamples.size() > 0)
	{
		rightAvg = accumulate(forceData[rightWheel].forceSamples.begin(), forceData[rightWheel].forceSamples.end(), 0.0);
		rightAvg /= forceData[rightWheel].forceSamples.size();
	}
	Log(LOG_DEV_DATA, "Calculated averages - Left: %.2f     Right: %.2f", leftAvg, rightAvg);
	// Calculate the sum and difference
	AverageBrakeForce(leftWheel, &leftAvg);
	AverageBrakeForce(rightWheel, &rightAvg);
	float sum = leftAvg + rightAvg;
	float diff = rightAvg - leftAvg;   // result: positive = bias to right, negative = bias to left
	bool sumGood = analyze.CompareData(sumLimit, sum, comparison);
	bool diffGood = analyze.CompareData(sum*diffLimit, fabs(diff), GREATER);
	Log(LOG_DEV_DATA, "Calculated results -  Sum: %.2f (%s)     Diff: %.2f (%s)", 
		sum, sumGood ? testPass.c_str() : testFail.c_str(), diff, diffGood ? testPass.c_str() : testFail.c_str());
	// Store the details
	char buff[32];
	details.AddDetail(axle+"LeftAverage", CreateMessage(buff, sizeof(buff), "%.2f", leftAvg), unitsN);
	details.AddDetail(axle+"RightAverage", CreateMessage(buff, sizeof(buff), "%.2f", rightAvg), unitsN);
	details.AddDetail(axle+"Sum", CreateMessage(buff, sizeof(buff), "%.2f", sum), unitsN);
	details.AddDetail(axle+"Diff", CreateMessage(buff, sizeof(buff), "%.2f", diff), unitsN);
	details.AddDetail(axle+"SumResult", sumGood ? testPass : testFail, "");
	details.AddDetail(axle+"DiffResult", diffGood ? testPass : testFail, "");
	// Write the details to the PLC
	SystemWrite(GetDataTag("VariableDataSumResultTag"), sumGood);
	SystemWrite(GetDataTag("VariableDataBalanceResultTag"), diffGood);
	SystemWrite(GetDataTag("VariableDataSumValueTag"), sum);
	SystemWrite(GetDataTag("VariableDataBalanceValueTag"), diff);
	SystemWrite(GetDataTag("VariableDataTypeTag"), resultType);
	SystemWrite(GetDataTag("VariableDataReadyTag"), true);
	BposSleep(GetParameterInt("VariableDataprocessingDelay"));
	SystemWrite(GetDataTag("VariableDataReadyTag"), false);
	SystemWrite(leftDisplayTag, leftAvg);
	SystemWrite(rightDisplayTag, rightAvg);
	SystemWrite(diffDisplayTag, diff);
	SystemWrite(sumDisplayTag, sum);
	SystemWrite(diffBgColorTag, diffGood ? string("Green") : string("Red"));
	SystemWrite(sumBgColorTag, sumGood ? string("Green") : string("Red"));
	result = (sumGood && diffGood) ? testPass : testFail;
	INT8 axleNumber = -1;
	if(!axle.compare("Front"))      axleNumber = FRONT_AXLE;
	else if(!axle.compare("Rear"))  axleNumber = REAR_AXLE;
	if(axleNumber != -1)
	{
		m_axleBrakeResults[axleNumber].axleResult = result;
		m_axleBrakeResults[axleNumber].axleSum = sum;
	}
	Log(LOG_DEV_DATA, "Force analysis complete - result: %s", result.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
const float MazdaBrakeTC::AverageBrakeForce(const INT16 &wheel, const float *avgForce /*= NULL*/)
{
	if(avgForce != NULL)  m_avgBrakeForces[wheel] = *avgForce;
	return m_avgBrakeForces[wheel];
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE MazdaBrakeTC::DisableRollMotors(const string motorMode, bool waitForZeroSpeed /*= true*/)
{
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::DisableRollMotors(motorMode: %s) - Enter", motorMode.c_str());
	string lfValue;
	string rfValue;
	string lrValue;
	string rrValue;
	if(!motorMode.compare(SPEED_MODE))
	{
		lfValue = "LeftFrontSpeedValue";
		rfValue = "RightFrontSpeedValue";
		lrValue = "LeftRearSpeedValue";
		rrValue = "RightRearSpeedValue";
	}
	else if(!motorMode.compare(TORQUE_MODE))
	{
		lfValue = "LeftFrontTorqueValue";
		rfValue = "RightFrontTorqueValue";
		lrValue = "LeftRearTorqueValue";
		rrValue = "RightRearTorqueValue";
	}
	// set the motor command values
	m_MotorController.Write(lfValue, string("0.0"), true);
	m_MotorController.Write(rfValue, string("0.0"), true);
	m_MotorController.Write(lrValue, string("0.0"), true);
	m_MotorController.Write(rrValue, string("0.0"), true);
	if(waitForZeroSpeed)
	{
		WaitForWheelSpeedsToBeReached(0.0, 0.0, 0.0, 0.0);
	}
	// set the motor mode
	m_MotorController.Write("LeftFrontMotorMode", BOOST_MODE, true);
	m_MotorController.Write("RightFrontMotorMode", BOOST_MODE, true);
	m_MotorController.Write("LeftRearMotorMode", BOOST_MODE, true);
	m_MotorController.Write("RightRearMotorMode", BOOST_MODE, true);
	Log(LOG_FN_ENTRY, "MazdaBrakeTC::DisableRollMotors(motorMode: %s) - Exit", motorMode.c_str());
	return BEP_STATUS_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
float MazdaBrakeTC::GetAxleWeight(string axle)
{   
	float axleWeight = -1.0;
	// Try to get the axle weight from the system
	if(axle.compare("ParkBrake"))
	{
		axleWeight = SystemReadFloat(GetDataTag(axle+"AxleWeightTag"));
		if(axleWeight == 0.0)
		{   // No axle weight provided, use the default
			axleWeight = GetParameterFloat(GetDataTag(axle+"AxleWeightDefaultParameter"));
		}
	}
	else
	{
		axleWeight = SystemReadFloat(GetDataTag("FrontAxleWeightTag")) + SystemReadFloat(GetDataTag("RearAxleWeightTag"));
		if(axleWeight == 0.0)
		{   // No axle weight provided, use the default
			axleWeight = GetParameterFloat(GetDataTag("FrontAxleWeightDefaultParameter")) + GetParameterFloat(GetDataTag("RearAxleWeightDefaultParameter"));
		}
	}
	Log(LOG_DEV_DATA, "Using %.2f for %s axle weight", axleWeight, axle.c_str());
	return axleWeight;
}

//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::IsWheelInSpeedRange(const string &wheelTag, const float &targetSpeed, const float &tolerance)
{
    bool status = false;
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::IsWheelInSpeedRange() - Enter");

    float currSpeed = SystemReadFloat(wheelTag);
	float range = targetSpeed * (tolerance / 100.0);
    float minSpeed = targetSpeed - range;
    float maxSpeed = targetSpeed + range;
    Log(LOG_DEV_DATA, "Verify %s is in acceptable range. minSpeed: %.2f  currSpeed: %.2f  maxSpeed: %.2f", wheelTag.c_str(), minSpeed, currSpeed, maxSpeed);
	status = (minSpeed <= currSpeed) && (currSpeed <= maxSpeed);
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::IsWheelInSpeedRange() - Exit status: %d", status);
    return (status);
}

//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::MonitorBrakeForces(INT16 startingRoller, INT16 stoppingRoller,
									  float minimumRequiredForceFront, float minimumRequiredForceRear,
									  MaxBrakeData *brakeData, UINT16 samplesToAverage)
{
    bool measurementComplete = false;
	UINT16 samplesAboveLimit = GetParameterInt("SamplesAboveLimitToStartAverage");
	UINT16 currentSamplesAboveLimit = 0;
	float requiredForce = (stoppingRoller < LRWHEEL) ? minimumRequiredForceFront : minimumRequiredForceRear;
    do
    {   // Get the current force for each wheel
		ReadCurrentLoadCellValues(brakeData, startingRoller);
		// Check if all monitored rollers are above the limit
		bool allForcesAboveLimit = true;
		for(int roller = startingRoller; (roller <= stoppingRoller) && allForcesAboveLimit; roller++)
		{
			allForcesAboveLimit = (brakeData[roller].currentForce >= requiredForce);
		}
		// update the number of samples above the limit
		currentSamplesAboveLimit = allForcesAboveLimit ? currentSamplesAboveLimit + 1 : 0;
		// Analyze the data
        for(int roller = startingRoller; roller <= stoppingRoller; roller++)
        {   // Display the current force
            SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
			if(allForcesAboveLimit)
			{   // Check if this is a max value
				if(brakeData[roller].currentForce > requiredForce)
				{
					if(currentSamplesAboveLimit >= samplesAboveLimit)
					{   // We have enough samples over the threshold, start averaging
						brakeData[roller].forceSamples.push_back(brakeData[roller].currentForce);
						if(brakeData[roller].currentForce > brakeData[roller].maxForce)  
						{
							brakeData[roller].maxForce = brakeData[roller].currentForce;
							Log(LOG_DEV_DATA, "New max force for %s: %.2f", rollerName[roller].c_str(), brakeData[roller].maxForce);
						}
						// Check if measurement is complete on this wheel
						if(brakeData[roller].forceSamples.size() > samplesToAverage)
						{   // Remove old samples to keep a moving sample window
							brakeData[roller].forceSamples.pop_front();
							brakeData[roller].measurementComplete = true;
							Log(LOG_DEV_DATA, "Measurement complete for %s - reached min samples: %d",
								rollerName[roller].c_str(), samplesToAverage);
						}
					}
				}
			}
			else
			{   // Have not reached the threshold yet, do not start collecting samples
				brakeData[roller].forceSamples.clear();
			}
        }
        // Check if all wheels have been completed
        measurementComplete = true;
        for(int roller = startingRoller; (roller <= stoppingRoller) && measurementComplete; roller++)
        {
            measurementComplete = brakeData[roller].measurementComplete;
        }
        Log(LOG_DEV_DATA, "All measurements complete: %s", measurementComplete ? "True" : "False");
        // Wait a bit before the next measurement
        if(!measurementComplete)  BposSleep(GetParameterInt("LoadCellReadingDelay"));
        // Keep checking until all min required forces are satisfied
    } while((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && !measurementComplete);
    // Return the completion flag
    return measurementComplete;
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::ReadCurrentLoadCellValues(MaxBrakeData *brakeData, UINT16 startingRoller)
{
    for(UINT8 roller = startingRoller; roller <= RRWHEEL; roller++)
    {
        brakeData[roller].currentForce = (atof(SystemRead(brakeData[roller].currentForceTag).c_str()));
    }
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::SetupLoadCellDataStructure(MaxBrakeData dataStruct[], string loadCellTags[], string displayTags[])
{   // Make sure data fields are initialized
	for(INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
	{
		dataStruct[roller].currentForce = 0.0;
		dataStruct[roller].currentForceTag = loadCellTags[roller];
		dataStruct[roller].displayTag = displayTags[roller];
		dataStruct[roller].maxForce = 0.0;
		dataStruct[roller].measurementComplete = false;
	}
}

//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::StartTestHeadTest(const string &testStep)
{
	bool testStarted = false;
	// Get the test information
	const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
	if(testData != m_testHeadTestSteps.end())
	{   // Command the test step to start
		SystemWrite(testData->second->getAttribute("StartTag")->getValue(), true);
		testStarted = true;
	}
	return testStarted;
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::StopTestHeadTest(const string &testStep)
{
	// Get the test information
	const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
	if(testData != m_testHeadTestSteps.end())
	{   // Command the test step to start
		SystemWrite(testData->second->getAttribute("StartTag")->getValue(), false);
	}
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::WaitForAbsBuild(const INT16 &testingWheel, const INT16 &oppositeWheel)
{
	string result(BEP_TESTING_RESPONSE);
	bool buildComplete = false;
	float buildPercent = 0.0;
	float buildMinLimit = GetParameterFloat("AbsBuildMinPercent");
	float buildMaxLimit = GetParameterFloat("AbsBuildMaxPercent");
	MaxBrakeData forceData[GetRollerCount()];
	string loadCellTags[] = {GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"), 
							 GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce")};
	string displayTags[] = {GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"), 
							GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag")};
	SetupLoadCellDataStructure(forceData, loadCellTags, displayTags);

	while(SystemReadBool(GetDataTag("AbsWheelStart")) && (BEP_STATUS_SUCCESS == StatusCheck()))  
		BposSleep(GetTestStepInfoInt("ScanDelay"));

	SetSecondaryStartTime();

	if(AverageBrakeForce(testingWheel) > 0.0)
	{
		do
		{	// Wait a short bit before looking again
			BposSleep(GetParameterInt("AbsMonitorDelay"));
			// Read the current forces
			ReadCurrentLoadCellValues(forceData, LFWHEEL);
			// Check if the build force is in limits
			buildPercent = forceData[testingWheel].currentForce / AverageBrakeForce(testingWheel);
			buildComplete = (buildMinLimit < buildPercent) && (buildPercent < buildMaxLimit);
		} while(!buildComplete && SecondaryTimeRemaining());
		// If the build completed, make sure the force is in limits with the other wheel
		Log(LOG_DEV_DATA, "Done waiting for build to complete - buildComplete: %s", buildComplete ? "True" : "False");
		if(buildComplete)
		{
			if(forceData[oppositeWheel].currentForce > 0.0)
			{
				float buildComparePercent = forceData[oppositeWheel].currentForce / AverageBrakeForce(oppositeWheel);
				Log(LOG_DEV_DATA, "Calculated compare percent: %.2f", buildComparePercent);
				result = ((GetParameterFloat("MinAbsBuildComparePercent") < buildComparePercent) &&
						  (buildComparePercent < GetParameterFloat("MaxAbsBuildComparePercent"))) ? testPass : testFail;
				Log(LOG_DEV_DATA, "ABS Build result - %.2f  [%.2f %.2f]", buildPercent, buildMinLimit, buildMaxLimit);
				Log(LOG_DEV_DATA, "ABS Build Compare result - %.2f  [%s %s]  -  %s", buildComparePercent,
					GetParameter("MinAbsBuildComparePercent").c_str(), GetParameter("MaxAbsBuildComparePercent").c_str(), 
					result.c_str());
			}
			else
			{
				Log(LOG_ERRORS, "Opposite wheel had zero force, can not analyze results");
				result = testFail;
			}
		}
		else
		{
			Log(LOG_ERRORS, "ABS build did not complete - last force reading: %.2f", forceData[testingWheel].currentForce);
			result = testFail;
		}
	}
	else
	{
		Log(LOG_ERRORS, "Average brake force is %.2f, cannot analyze ABS build", AverageBrakeForce(testingWheel));
	}
	string absBuildTag = !result.compare(testPass) ? GetDataTag("AbsBuildOk") : GetDataTag("AbsBuildNok");
	SystemWrite(absBuildTag, true);
	return result;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::WaitForAbsDump(const INT16 &testingWheel, const INT16 &oppositeWheel)
{
	string result(BEP_TESTING_RESPONSE);
	bool dumpComplete = false;
	MaxBrakeData forceData[GetRollerCount()];
	string loadCellTags[] = {GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"), 
							 GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce")};
	string displayTags[] = {GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"), 
							GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag")};
	SetupLoadCellDataStructure(forceData, loadCellTags, displayTags);

	while(!SystemReadBool(GetDataTag("AbsWheelStart")) && !SystemReadBool("CurrentWheelAbsTestComplete") &&
		  (BEP_STATUS_SUCCESS == StatusCheck()))  
		BposSleep(GetTestStepInfoInt("ScanDelay"));

	SetSecondaryStartTime();
	do
	{   // Wait a short time before looking again
		BposSleep(GetParameterInt("AbsMonitorDelay"));
		// Read the current forces
		ReadCurrentLoadCellValues(forceData, LFWHEEL);
		// Check if the dump force is in limits
		dumpComplete = forceData[testingWheel].currentForce < GetParameterFloat("AbsDumpMaxLimit");
		Log(LOG_DEV_DATA, "ABS Dump Complete: %s - Current Force: %.2f, Limit: %.2f",
			dumpComplete ? "True" : "False", forceData[testingWheel].currentForce, GetParameterFloat("AbsDumpMaxLimit"));
	} while(!dumpComplete && SecondaryTimeRemaining());
	// if the dump completed, need to check dump percent with opposite wheel
	if(dumpComplete)
	{
		float dumpPercent = forceData[oppositeWheel].currentForce / AverageBrakeForce(oppositeWheel);
		result = ((GetParameterFloat("MinAbsDumpPercent") < dumpPercent) &&
				  (dumpPercent < GetParameterFloat("MaxAbsDumpPercent"))) ? testPass : testFail;
		Log(LOG_DEV_DATA, "ABS Dump result - %.2f  [%s %s]  -  %s", dumpPercent,
			GetParameter("MinAbsDumpPercent").c_str(), GetParameter("MaxAbsDumpPercent").c_str(), result.c_str());
	}
	else
	{
		Log(LOG_ERRORS, "ABS dump did not complete - last force reading: %.2f", forceData[testingWheel].currentForce);
		result = testFail;
	}
	string absDumpTag = !result.compare(testPass) ? GetDataTag("AbsDumpOk") : GetDataTag("AbsDumpNok");
	SystemWrite(absDumpTag, true);
	return result;
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE MazdaBrakeTC::WaitForWheelSpeedsToBeReached(float lfTargetSpeed, float rfTargetSpeed,
                                                            float lrTargetSpeed, float rrTargetSpeed)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
    bool lfSpeedReached = false;
    bool rfSpeedReached = false;
    bool lrSpeedReached = false;
    bool rrSpeedReached = false;
	bool allSpeedsReached = false;
    char    temp[256];

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForWheelSpeedsToBeReached() - Enter");
    // set the motor mode
    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, true);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, true);
    m_MotorController.Write("LeftRearMotorMode", SPEED_MODE, true);
    m_MotorController.Write("RightRearMotorMode", SPEED_MODE, true);

    // set the motor speeds
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, sizeof(temp), "%.2f", lfTargetSpeed), true);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, sizeof(temp), "%.2f", rfTargetSpeed), true);
    m_MotorController.Write("LeftRearSpeedValue", CreateMessage(temp, sizeof(temp), "%.2f", lrTargetSpeed), true);
    m_MotorController.Write("RightRearSpeedValue", CreateMessage(temp, sizeof(temp), "%.2f", rrTargetSpeed), true);

    // Wait for motors to react
    BposSleep(GetParameterInt("MotorReactionTime"));
    Log(LOG_DEV_DATA, "WaitForWheelSpeedsToBeReached: LF: %.2f  RF: %.2f  LR: %.2f  RR: %.2f", 
		lfTargetSpeed, rfTargetSpeed, lrTargetSpeed, rrTargetSpeed);
    while(TimeRemaining() && !allSpeedsReached)
    {   // check if all of the wheel speeds are in range
        lfSpeedReached = IsWheelInSpeedRange(GetDataTag("LFSpeed"), lfTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        rfSpeedReached = IsWheelInSpeedRange(GetDataTag("RFSpeed"), rfTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        lrSpeedReached = IsWheelInSpeedRange(GetDataTag("LRSpeed"), lrTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        rrSpeedReached = IsWheelInSpeedRange(GetDataTag("RRSpeed"), rrTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        Log(LOG_DEV_DATA, "Wheelspeeds reached: LF: %d  RF: %d  LR: %d  RR: %d", lfSpeedReached, rfSpeedReached, lrSpeedReached, rrSpeedReached);
		allSpeedsReached = lfSpeedReached && rfSpeedReached && lrSpeedReached && rrSpeedReached;
		BposSleep(GetParameterInt("MotorReactionTime"));
    }
	status = allSpeedsReached ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForWheelSpeedsToBeReached() - Exit    status: %s", ConvertStatusToResponse(status).c_str());
    return (status);
}
