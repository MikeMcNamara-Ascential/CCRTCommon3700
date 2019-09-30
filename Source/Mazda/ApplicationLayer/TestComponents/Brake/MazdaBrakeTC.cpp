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
    if (BEP_STATUS_SUCCESS == StatusCheck())
    {
        if (!GetTestStepName().compare("AbsLfTest"))
            testResult = MazdaAbsValveTest("LfAbsTest", LFWHEEL, RFWHEEL);
        else if (!GetTestStepName().compare("AbsLrTest"))
            testResult = MazdaAbsValveTest("LrAbsTest", LRWHEEL, RRWHEEL);
        else if (!GetTestStepName().compare("AbsRfTest"))
            testResult = MazdaAbsValveTest("RfAbsTest", RFWHEEL, LFWHEEL);
        else if (!GetTestStepName().compare("AbsRrTest"))
            testResult = MazdaAbsValveTest("RrAbsTest", RRWHEEL, LRWHEEL);
        else if (!GetTestStepName().compare("BrakeTestComplete"))
            testResult = BrakeTestingComplete();
        else if (!GetTestStepName().compare("BrakeTestStart"))
        {
            DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
            testResult = SystemWrite(GetDataTag("RunBrakeTestTag"), true) == BEP_STATUS_SUCCESS ? testPass : testFail;
        }
        else if (!GetTestStepName().compare("BrakeTestStop"))
        {
            RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
            testResult = SystemWrite(GetDataTag("RunBrakeTestTag"), false) == BEP_STATUS_SUCCESS ? testPass : testFail;
        }
        else if (!GetTestStepName().compare("ClearDiagnosticRoutine"))
            testResult = ClearDiagnostics();
        else if (!GetTestStepName().compare("FaultCheckRoutine"))
            testResult = FaultCheck();
        else if (!GetTestStepName().compare("MazdaFrontBrakeForce"))
            testResult = MazdaBrakeForceTest("Front");
        else if (!GetTestStepName().compare("MazdaRearBrakeForce"))
            testResult = MazdaBrakeForceTest("Rear");
        else if (!GetTestStepName().compare("MazdaDragTest"))
            testResult = MazdaDragTest();
        else if (!GetTestStepName().compare("MazdaParkBrakeForce"))
            testResult = MazdaBrakeForceTest("ParkBrake");
        else if (!GetTestStepName().compare("ReportOverallBrakeResults"))
            testResult = ReportOverallBrakeResults();
        else if (!GetTestStepName().compare("SpeedSensorCheck"))
            testResult = SpeedSensorCheck();
        else if (!GetTestStepName().compare("StartBrakeSwitchTest"))
            testResult = MazdaBrakeSwitchTest("Start");
        else if (!GetTestStepName().compare("StopBrakeSwitchTest"))
            testResult = MazdaBrakeSwitchTest("Stop");
        else if (!GetTestStepName().compare("StopRollers"))
        {
            testResult = (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(0.0, 0.0, 0.0, 0.0)) ? testPass : testFail;
        }
        else if (!GetTestStepName().compare("TestHeadWait"))
            testResult = WaitForMazdaTester(BposReadInt(value.c_str()));
        else if (!GetTestStepName().compare("SwapTest"))
            testResult = MazdaSwapTest(value);
        else if (!GetTestStepName().compare("RunBrakeTest") || !GetTestStepName().compare("RunSwapTest"))
            testResult = RunBrakeTest();
        else if (!GetTestStepName().compare("SwapTestByAxle"))
            testResult = SwapTestByAxle(value);
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
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "ERROR: Could not load Mazda Test Head Test Steps: %s", excpt.GetReason());
    }
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::BrakeTestingComplete(void)
{
    DisableRollMotors(SPEED_MODE, GetParameterBool("DisableMotorsWaitForZeroSpeed"));
    DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"),
                  GetPromptPriority("RemoveFootFromBrake"));
    return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::ClearDiagnostics(void)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::ClearDiagnostics() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
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
    if (!ShortCircuitTestStep())
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
    if (!ShortCircuitTestStep())
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
        if (!dumpResult.compare(testPass))
        {   // Wait for the ABS Build to complete
            BposSleep(GetParameterInt("AbsBuildDelay"));
            buildResult = WaitForAbsBuild(testingWheel, oppositeWheel);
            Log(LOG_DEV_DATA, "ABS Build complete: %s", buildResult.c_str());
        }
        else
        {
            if (GetTestStepInfoInt("RunBrakeTestReleaseDelay") > 0)
            {
                Log(LOG_DEV_DATA, "Waiting a bit.. RunBrakeTestReleaseDelay: %d", GetTestStepInfoInt("RunBrakeTestReleaseDelay"));
                BposSleep(GetTestStepInfoInt("RunBrakeTestReleaseDelay"));
            }
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
    if (!axle.compare("Front"))
    {
        leftWheel = LFWHEEL;
        rightWheel = RFWHEEL;
        resultType = BRAKE_RESULTS_FRONT;
    }
    else if (!axle.compare("Rear"))
    {
        leftWheel = LRWHEEL;
        rightWheel = RRWHEEL;
        resultType = BRAKE_RESULTS_REAR;
    }
    else if (!axle.compare("ParkBrake"))
    {
        leftWheel = LRWHEEL;
        rightWheel = RRWHEEL;
        resultType = PARK_BRAKE_RESULTS;
        applyBrakePrompt = "ApplyParkBrake";
        DisplayPrompt(1, GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
    }
    string result(BEP_TESTING_STATUS);
    string loadCellTags[] = { GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"),
        GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce") };
    string displayTags[] = { GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"),
        GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag") };
    MaxBrakeData brakeData[GetRollerCount()];
    TestResultDetails testDetails;
    if (!ShortCircuitTestStep())
    {   // Setup the roller speeds
        if (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat(axle + "AxleBrakeForceSpeedLf"),
                                                                GetParameterFloat(axle + "AxleBrakeForceSpeedRf"),
                                                                GetParameterFloat(axle + "AxleBrakeForceSpeedLr"),
                                                                GetParameterFloat(axle + "AxleBrakeForceSpeedRr")))
        {   // Tell the operator to apply the brake pedal
            DisplayPrompt(GetPromptBox(applyBrakePrompt), GetPrompt(applyBrakePrompt), GetPromptPriority(applyBrakePrompt));
            BposSleep(GetParameterInt("OperatorReactionTime"));
            if (!axle.compare("ParkBrake"))
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
            } while (!measurementComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Analyze the exit status
            if (measurementComplete)
            {   // Calculate the limits
                float sumLimit = GetAxleWeight(axle) * GetParameterFloat(axle + "BrakeLimit");
                float diffLimit = GetAxleWeight(axle) * GetParameterFloat(axle + "BrakeDiffLimit");

                // Measurement complete, analyze the brake
                result = AnalyzeForceResults(brakeData, leftWheel, rightWheel, resultType,
                                             sumLimit, diffLimit, LESS, testDetails, axle,
                                             GetDataTag(axle + "LeftForceValue"), GetDataTag(axle + "RightForceValue"),
                                             GetDataTag(axle + "ForceSumValue"), GetDataTag(axle + "ForceDiffValue"),
                                             GetDataTag(axle + "ForceSumBgColor"), GetDataTag(axle + "ForceDiffBgColor"));
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
        if (!axle.compare("ParkBrake"))
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
    if (!ShortCircuitTestStep())
    {   // Prompt the operator to remove foot from brake pedal
        if (!action.compare("Start"))
        {
            DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
            DisplayPrompt(GetPromptBox("SpeedSensorCheck"), GetPrompt("SpeedSensorCheck"), GetPromptPriority("SpeedSensorCheck"));
        }
        else
        {
            DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
        }
        if (GetParameterBool("WaitForOperator"))
        {
            BposSleep(GetParameterInt("OperatorReactionTime"));
        }
        // Run the test head test
        result = PerformTestHeadTest(GetTestStepInfo("TestHeadTestName"), !action.compare("Start"));
        Log(LOG_DEV_DATA, "Test Head Brake switch test % completed, result: %s", action.c_str(), result.c_str());
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"), "0000");
        if (!action.compare("Start"))
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
    string loadCellTags[] = { GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"),
        GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce") };
    string displayTags[] = { GetDataTag("LfDragDisplayTag"), GetDataTag("RfDragDisplayTag"),
        GetDataTag("LrDragDisplayTag"), GetDataTag("RrDragDisplayTag") };
    if (!ShortCircuitTestStep())
    {   // Make sure the operator keeps foot off brake
        DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
        if (GetParameterBool("WaitForOperator"))
        {
            BposSleep(GetParameterInt("OperatorReactionTime"));
        }
        TestResultDetails testDetails;
        // Command the rollers to the test speed
        if (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("LfDragTestSpeed"),
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
            } while (!measurementComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
            // Analyze the exit status
            if (measurementComplete)
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
        if (GetParameterBool("DisableMotorsAfterDragTest"))
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
    if (StartTestHeadTest(testStep))
    {
        if (waitForTestComplete)
        {   // Wait for the test step to complete
            const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
            while (!testComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
            {   // Wait for a bit for the test step to complete
                BposSleep(GetParameterInt("TestCompleteCheckDelay"));
                // Check if the test has completed yet
                testComplete = SystemReadBool(testData->second->getAttribute("TestCompleteTag")->getValue());
            }
            if (testComplete)
            {   // Test step completed, set the result
                /*
                if (GetTestStepInfoInt("TestResultCheckDelay") > 0)
                {
                    Log(LOG_DEV_DATA, "Waiting a bit.. TestResultCheckDelay: %d", GetTestStepInfoInt("TestResultCheckDelay"));
                    BposSleep(GetTestStepInfoInt("TestResultCheckDelay"));
                }*/
                bool resultFromMazda_OK; 
                Log(LOG_DEV_DATA, "Checking result tag: %s", testData->second->getAttribute("TestResultTag")->getValue().c_str());
                if (GetTestStepInfoBool("WaitForResult"))
                {
                    
                    while (result != testPass && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
                    {   // Wait for a bit for the test step to complete
                        bool resultFromMazda_OK = SystemReadBool(testData->second->getAttribute("TestResultTag")->getValue());
                        BposSleep(GetParameterInt("TestCompleteCheckDelay"));
                        // Check if the test has completed yet
                        Log(LOG_DEV_DATA, "%s: %s", testData->second->getAttribute("TestResultTag")->getValue().c_str(), resultFromMazda_OK ? "OK" : "NG");
                        result = resultFromMazda_OK  ? testPass : testFail;
                    }
                }
                else
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
    if (GetParameterBool("UseAxleWeightsForTotalBrakeForceResult"))
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
    if (!ShortCircuitTestStep())
    {   // Instruct operator to remove foot from brake pedal
        DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
        BposSleep(GetParameterInt("OperatorReactionTime"));
        // Spin up the rollers
        if (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("LfSpeedSensorCheckSpeed"),
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
    if (!ShortCircuitTestStep())
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
    if (forceData[leftWheel].forceSamples.size() > 0)
    {
        leftAvg = accumulate(forceData[leftWheel].forceSamples.begin(), forceData[leftWheel].forceSamples.end(), 0.0);
        leftAvg /= forceData[leftWheel].forceSamples.size();
    }
    if (forceData[rightWheel].forceSamples.size() > 0)
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
    bool diffGood = analyze.CompareData(diffLimit, fabs(diff), GREATER);
    Log(LOG_DEV_DATA, "Calculated results -  Sum: %.2f (%s)     Diff: %.2f (%s)",
        sum, sumGood ? testPass.c_str() : testFail.c_str(), diff, diffGood ? testPass.c_str() : testFail.c_str());
    // Store the details
    char buff[32];
    details.AddDetail(axle + "LeftAverage", CreateMessage(buff, sizeof(buff), "%.2f", leftAvg), unitsN);
    details.AddDetail(axle + "RightAverage", CreateMessage(buff, sizeof(buff), "%.2f", rightAvg), unitsN);
    details.AddDetail(axle + "Sum", CreateMessage(buff, sizeof(buff), "%.2f", sum), unitsN);
    details.AddDetail(axle + "Diff", CreateMessage(buff, sizeof(buff), "%.2f", diff), unitsN);
    details.AddDetail(axle + "SumResult", sumGood ? testPass : testFail, "");
    details.AddDetail(axle + "DiffResult", diffGood ? testPass : testFail, "");
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
    if (!axle.compare("Front"))
        axleNumber = FRONT_AXLE;
    else if (!axle.compare("Rear"))
        axleNumber = REAR_AXLE;
    if (axleNumber != -1)
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
    if (avgForce != NULL)
        m_avgBrakeForces[wheel] = *avgForce;
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
    if (!motorMode.compare(SPEED_MODE))
    {
        lfValue = "LeftFrontSpeedValue";
        rfValue = "RightFrontSpeedValue";
        lrValue = "LeftRearSpeedValue";
        rrValue = "RightRearSpeedValue";
    }
    else if (!motorMode.compare(TORQUE_MODE))
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
    if (waitForZeroSpeed)
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
    if (axle.compare("ParkBrake"))
    {
        axleWeight = SystemReadFloat(GetDataTag(axle + "AxleWeightTag"));
        if (axleWeight == 0.0)
        {   // No axle weight provided, use the default
            axleWeight = GetParameterFloat(GetDataTag(axle + "AxleWeightDefaultParameter"));
            Log(LOG_DEV_DATA, "Using default axle weight");
        }
    }
    else
    {
        axleWeight = SystemReadFloat(GetDataTag("FrontAxleWeightTag")) + SystemReadFloat(GetDataTag("RearAxleWeightTag"));
        if (axleWeight == 0.0)
        {   // No axle weight provided, use the default
            axleWeight = GetParameterFloat(GetDataTag("FrontAxleWeightDefaultParameter")) + GetParameterFloat(GetDataTag("RearAxleWeightDefaultParameter"));
        }
    }
    float axleWeightKg = axleWeight;
    //convert kg to newtons
    axleWeight = axleWeight * 9.80665002864;
    Log(LOG_DEV_DATA, "Using %.2f (%.2fkg) for %s axle weight", axleWeight, axleWeightKg, axle.c_str());
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
    return(status);
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
        for (int roller = startingRoller; (roller <= stoppingRoller) && allForcesAboveLimit; roller++)
        {
            allForcesAboveLimit = (brakeData[roller].currentForce >= requiredForce);
        }
        // update the number of samples above the limit
        currentSamplesAboveLimit = allForcesAboveLimit ? currentSamplesAboveLimit + 1 : 0;
        // Analyze the data
        for (int roller = startingRoller; roller <= stoppingRoller; roller++)
        {   // Display the current force
            SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
            if (allForcesAboveLimit)
            {   // Check if this is a max value
                if (brakeData[roller].currentForce > requiredForce)
                {
                    if (currentSamplesAboveLimit >= samplesAboveLimit)
                    {   // We have enough samples over the threshold, start averaging
                        brakeData[roller].forceSamples.push_back(brakeData[roller].currentForce);
                        if (brakeData[roller].currentForce > brakeData[roller].maxForce)
                        {
                            brakeData[roller].maxForce = brakeData[roller].currentForce;
                            Log(LOG_DEV_DATA, "New max force for %s: %.2f", rollerName[roller].c_str(), brakeData[roller].maxForce);
                        }
                        // Check if measurement is complete on this wheel
                        if (brakeData[roller].forceSamples.size() > samplesToAverage)
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
        for (int roller = startingRoller; (roller <= stoppingRoller) && measurementComplete; roller++)
        {
            measurementComplete = brakeData[roller].measurementComplete;
        }
        Log(LOG_DEV_DATA, "All measurements complete: %s", measurementComplete ? "True" : "False");
        // Wait a bit before the next measurement
        if (!measurementComplete)
            BposSleep(GetParameterInt("LoadCellReadingDelay"));
        // Keep checking until all min required forces are satisfied
    } while ((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && !measurementComplete);
    // Return the completion flag
    return measurementComplete;
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::ReadCurrentLoadCellValues(MaxBrakeData *brakeData, UINT16 startingRoller)
{
    for (UINT8 roller = startingRoller; roller <= RRWHEEL; roller++)
    {
        brakeData[roller].currentForce = (atof(SystemRead(brakeData[roller].currentForceTag).c_str()));
    }
    Log(LOG_DEV_DATA, "ReadCurrentLoadCellValues - %5.2f, %5.2f, %5.2f, %5.2f", brakeData[LFWHEEL].currentForce,
        brakeData[RFWHEEL].currentForce, brakeData[LRWHEEL].currentForce, brakeData[RRWHEEL].currentForce);
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::SetupLoadCellDataStructure(MaxBrakeData dataStruct[], string loadCellTags[], string displayTags[])
{   // Make sure data fields are initialized
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SetupLoadCellDataStructure -- Enter");
    for (INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
    {
        dataStruct[roller].currentForce = 0.0;
        dataStruct[roller].currentForceTag = loadCellTags[roller];
        dataStruct[roller].displayTag = displayTags[roller];
        dataStruct[roller].maxForce = 0.0;
        dataStruct[roller].measurementComplete = false;
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SetupLoadCellDataStructure -- Exit");
}

//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::StartTestHeadTest(const string &testStep)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StartTestHeadTest() -- Enter");
    bool testStarted = false;
    // Get the test information
    const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
    if (testData != m_testHeadTestSteps.end())
    {   // Command the test step to start
        Log(LOG_FN_ENTRY, "Setting signal: %s to true",
            testData->second->getAttribute("StartTag")->getValue().c_str()); 
        SystemWrite(testData->second->getAttribute("StartTag")->getValue(), true);
        testStarted = true;
    }
    else
    {
        Log(LOG_DEV_DATA, "%s not found in defined test head test steps", testStep.c_str());
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StartTestHeadTest -- Exit");
    return testStarted;
}

//-------------------------------------------------------------------------------------------------
void MazdaBrakeTC::StopTestHeadTest(const string &testStep)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StopTestHeadTest() -- Enter");
    // Get the test information
    const XmlNodeMapItr testData = m_testHeadTestSteps.find(testStep);
    if (testData != m_testHeadTestSteps.end())
    {   // Command the test step to start
        if (GetTestStepInfoInt("StopTestHeadDelay") > 0)
        {
            Log(LOG_DEV_DATA, "Waiting a bit.. StopTestHeadDelay: %d", GetTestStepInfoInt("StopTestHeadDelay"));
            BposSleep(GetTestStepInfoInt("StopTestHeadDelay"));
        }
        Log(LOG_FN_ENTRY, "Setting signal: %s to false",
            testData->second->getAttribute("StartTag")->getValue().c_str()); 
        SystemWrite(testData->second->getAttribute("StartTag")->getValue(), false);
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StopTestHeadTest -- Exit");
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
    string loadCellTags[] = { GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"),
        GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce") };
    string displayTags[] = { GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"),
        GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag") };
    SetupLoadCellDataStructure(forceData, loadCellTags, displayTags);

    while (SystemReadBool(GetDataTag("AbsWheelStart")) && (BEP_STATUS_SUCCESS == StatusCheck()))
    {
        BposSleep(GetTestStepInfoInt("ScanDelay"));
    }

    if (AverageBrakeForce(testingWheel) > 0.0)
    {
        BposSleep(GetParameterInt("AbsBuildTime"));
        // Read the current forces
        ReadCurrentLoadCellValues(forceData, LFWHEEL);
        if (forceData[oppositeWheel].currentForce > 0.0)
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
    string loadCellTags[] = { GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"),
        GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce") };
    string displayTags[] = { GetDataTag("LfBrakeDisplayTag"), GetDataTag("RfBrakeDisplayTag"),
        GetDataTag("LrBrakeDisplayTag"), GetDataTag("RrBrakeDisplayTag") };
    SetupLoadCellDataStructure(forceData, loadCellTags, displayTags);

    while (!SystemReadBool(GetDataTag("AbsWheelStart")) && !SystemReadBool("CurrentWheelAbsTestComplete") &&
           (BEP_STATUS_SUCCESS == StatusCheck()))
    {
        BposSleep(GetTestStepInfoInt("ScanDelay"));
    }
    // Wait a bit for the dump to complete
    BposSleep(GetParameterInt("AbsDumpTime"));
    // Read the current forces
    ReadCurrentLoadCellValues(forceData, LFWHEEL);
    float dumpPercent = forceData[oppositeWheel].currentForce / AverageBrakeForce(oppositeWheel);
    result = ((GetParameterFloat("MinAbsDumpPercent") < dumpPercent) &&
              (dumpPercent < GetParameterFloat("MaxAbsDumpPercent"))) ? testPass : testFail;
    Log(LOG_DEV_DATA, "ABS Dump result - %.2f  [%s %s]  -  %s", dumpPercent,
        GetParameter("MinAbsDumpPercent").c_str(), GetParameter("MaxAbsDumpPercent").c_str(), result.c_str());
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
    while (TimeRemaining() && !allSpeedsReached && (BEP_STATUS_SUCCESS == StatusCheck()))
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
    return(status);
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaSwapTest(string targetWheel)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaSwapTest(targetWheel: %s) - Enter", targetWheel.c_str());
    string result(BEP_TESTING_STATUS);
    TestResultDetails testDetails;
    string description = GetTestStepInfo("Description");
    string loadCellTags[] = { GetDataTag("LfCurrentForce"), GetDataTag("RfCurrentForce"),
        GetDataTag("LrCurrentForce"), GetDataTag("RrCurrentForce") };
    string displayTags[] = { GetDataTag("LfSwapDisplayTag"), GetDataTag("RfSwapDisplayTag"),
        GetDataTag("LrSwapDisplayTag"), GetDataTag("RrSwapDisplayTag") };
    bool brakeForceOk = false;
    bool crossCheckWarning = false;
    SystemWrite(GetDataTag("SwapTestResultToPLC_OK"), false);
    SystemWrite(GetDataTag("SwapTestResultToPLC_NOK"), false);
    // RunBrakeTest Funct should have already waited for confirm -- will alter later maybe to drop signal
    SystemWrite(GetDataTag("RunBrakeTestTag"), false);
    char buff[128];
    string targetWheelAbrv;
    INT16 rollerIndex = 0;
    // Sort this out
    if ((strnicmp("lf", targetWheel.c_str(), 2) == 0) ||
        (strnicmp("leftfront", targetWheel.c_str(), 9) == 0))
    {
        targetWheel = "LeftFront";
        targetWheelAbrv = "Lf";
        rollerIndex = 0;
    }
    else if ((strnicmp("rf", targetWheel.c_str(), 2) == 0) ||
             (strnicmp("rightfront", targetWheel.c_str(), 9) == 0))
    {
        targetWheel = "RightFront";
        targetWheelAbrv = "Rf";
        rollerIndex = 1;
    }
    else if ((strnicmp("lr", targetWheel.c_str(), 2) == 0) ||
             (strnicmp("leftrear", targetWheel.c_str(), 9) == 0))
    {
        targetWheel = "LeftRear";
        targetWheelAbrv = "Lr";
        rollerIndex = 2;
    }
    else if ((strnicmp("rr", targetWheel.c_str(), 2) == 0) ||
             (strnicmp("rightrear", targetWheel.c_str(), 9) == 0))
    {
        targetWheel = "RightRear";
        targetWheelAbrv = "Rr";
        rollerIndex = 3;
    }
    else
    {
        Log(LOG_ERRORS, "Target wheel not recognized. Unable to proceed.");
        result = testFail;
    }

    string testHeadTestStep = targetWheelAbrv + "AbsTest"; //"SwapTest"
    const XmlNodeMapItr testData = m_testHeadTestSteps.find(testHeadTestStep);

    description = targetWheel + " wheel Swap Test ";
    if (result != testFail && !ShortCircuitTestStep())
    {   // Make sure the operator keeps foot off brake
        DisplayPrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));

        SetStartTime();
        //SystemWrite(GetDataTag("RunBrakeTestTag"), true);
        //StartTestHeadTest("EnableSwapTest");

        if (GetTestStepInfoBool("WaitForOperator") && 
            !(GetParameterBool("SkipOperatorWaitOnConsecutiveWheel") && (rollerIndex % 2 != 0)) && 
            !SystemReadBool(GetDataTag("IncycleRetest")))
        {
            BposSleep(GetParameterInt("OperatorReactionTime"));
        }
        TestResultDetails testDetails;
        // Command the rollers to the test speed
        if (BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("LfSwapTestSpeed"),
                                                                GetParameterFloat("RfSwapTestSpeed"),
                                                                GetParameterFloat("LrSwapTestSpeed"),
                                                                GetParameterFloat("RrSwapTestSpeed")))
        {   // Roller speeds are good, take drag force readings
            RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
            DisplayPrompt(GetPromptBox("SwapTestInProgress"), GetPrompt("SwapTestInProgress"), GetPromptPriority("SwapTestInProgress"));
            MaxBrakeData swapData[GetRollerCount()];
            // Setup the data structure
            SetupLoadCellDataStructure(swapData, loadCellTags, displayTags);
            // Fire up test head

            //Can't cuz no timing confirmed from Mazda on confirmation.. Is it after actuation?
            /*if (SendSignalToMazdaTestHead(testData->second->getAttribute("StartTag")->getValue(), 
                                          testData->second->getAttribute("TestCompleteTag")->getValue()) == BEP_STATUS_SUCCESS)*/ 

            if (StartTestHeadTest(testHeadTestStep))
            {
                bool measurementComplete = false;

                string axle = rollerIndex > 1 ? "Rear" : "Front";
                float minBrakeForce = GetAxleWeight(axle) * GetParameterFloat("SwapTest" + axle + "BrakeLimit");
                float minSampleForce = GetParameterFloat("SwapTestMinSampleForce") > 0 ?
                                       GetParameterFloat("SwapTestMinSampleForce") : minBrakeForce;
                measurementComplete = MonitorSwapTestForces(rollerIndex, minSampleForce, swapData,
                                                            GetParameterInt("NumberOfSwapSamples"), testData, testHeadTestStep);

                // Analyze the exit status
                if (measurementComplete)
                {   // Measurement complete, analyze the data
                    float averages[GetRollerCount()];
                    for (int roller = 0; roller < GetRollerCount(); roller++)
                    {
                        averages[roller] = accumulate(swapData[roller].forceSamples.begin(), swapData[roller].forceSamples.end(), 0.0);
                        averages[roller] /= swapData[roller].forceSamples.size();
                    }

                    Log(LOG_DEV_DATA, "Calculated averages (targetWheel: %s) -- %5.2f, %5.2f, %5.2f, %5.2f", targetWheel.c_str(),
                        averages[LeftFront], averages[RightFront], averages[LeftRear], averages[RightRear]);

                    Log(LOG_DEV_DATA, "Max Recorded Forces (targetWheel: %s) -- %5.2f, %5.2f, %5.2f, %5.2f", targetWheel.c_str(),
                        swapData[LeftFront].maxForce, swapData[RightFront].maxForce,
                        swapData[LeftRear].maxForce, swapData[RightRear].maxForce);

                    float maxAvgForce = 0, maxMaxForce = 0;
                    int maxAvgForceLocation = 0, maxMaxForceLocation = 0;
                    for (int roller = 0; roller < GetRollerCount(); roller++)
                    {
                        if (averages[roller] > maxAvgForce)
                        {
                            maxAvgForce =  averages[roller];
                            maxAvgForceLocation = roller;
                        }

                        if (swapData[roller].maxForce > maxMaxForce)
                        {
                            maxMaxForce =  swapData[roller].maxForce;
                            maxMaxForceLocation = roller;
                        }
                    }

                    if (maxAvgForceLocation != rollerIndex)
                    {
                        crossCheckWarning = true;
                        Log(LOG_DEV_DATA, "Cross Check Warning: %s average force is greater than target wheel",
                            rollerName[maxAvgForceLocation].c_str());
                    }
                    else if (maxMaxForceLocation != rollerIndex)
                    {
                        crossCheckWarning = true;
                        Log(LOG_DEV_DATA, "Cross Check Warning: %s max force is greater than target wheel",
                            rollerName[maxMaxForceLocation].c_str());
                    }

                    if (GetParameterBool("SwapTestUseMaxBrakeForce"))
                    {
                        brakeForceOk = swapData[rollerIndex].maxForce > minBrakeForce;
                    }
                    else
                    {
                        brakeForceOk = averages[rollerIndex] > minBrakeForce;
                    }

                    testDetails.AddDetail("AverageRecordedForce", CreateMessage(buff, sizeof(buff), "%.2f", averages[rollerIndex]), unitsN);
                    testDetails.AddDetail("MaxRecordedForce", CreateMessage(buff, sizeof(buff), "%.2f", swapData[rollerIndex].maxForce), unitsN);
                    testDetails.AddDetail("MinBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", minBrakeForce), unitsN);
                    testDetails.AddDetail("ValveCrossDetected", CreateMessage(buff, sizeof(buff), "%s", crossCheckWarning ? "Yes" : "No"), "");

                    result = (brakeForceOk && !crossCheckWarning) ? testPass : testFail;
                    Log(LOG_DEV_DATA, "Overall swap test result (%s): %s", targetWheel.c_str(), result.c_str());
                }
                else
                {
                    result = testFail;
                    Log(LOG_ERRORS, "Swap testing failed because measurements were not completed");
                }

                if (result != testPass)
                    description += "Failed -- Insufficient brake force"; 

                /*
                if (!GetTestStepInfoBool("SkipWaitForTestStepComplete")) //This is really just a test step request confirm from Mazda
                {
                    bool testComplete;

                    while (!testComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
                    {   // Wait for a bit for the test step to complete
                        BposSleep(GetParameterInt("TestCompleteCheckDelay"));
                        // Check if the test has completed yet
                        testComplete = SystemReadBool(testData->second->getAttribute("TestCompleteTag")->getValue());
                    }
                    // Disable the test request

                }
                */
                //Not necessary but it can stay for now
                StopTestHeadTest(testHeadTestStep);
                RemovePrompt(GetPromptBox("SwapTestInProgress"), GetPrompt("SwapTestInProgress"), GetPromptPriority("SwapTestInProgress"));
                //StopTestHeadTest("EnableSwapTest");

            }
            else
            {
                Log(LOG_ERRORS, "Error sending swap test signal for wheel: %s", targetWheel.c_str());
                description += "Failed -- Error sending swapt test signal to Mazda"; 
            }
        }
        else
        {   // Timeout waiting for rollers to accelerate to speed
            RemovePrompt(GetPromptBox("NeutralPrompt"), GetPrompt("NeutralPrompt"), GetPromptPriority("NeutralPrompt"));
            Log(LOG_ERRORS, "Timeout waiting for rollers to be at speed");
            result = testFail;
            description += "Failed -- Timeout waiting for roller speed";  
        }

        if (GetParameterBool("DisableMotorsAfterSwapTest"))
        {
            DisableRollMotors(SPEED_MODE);
        }
        RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
    }
    else
    {   // Need to skip this step
        result = testSkip;
        Log(LOG_DEV_DATA, "Skipping MazdaSwapTest");
    }
// Log the exit and return the result
    if (result == testPass || GetTestStepInfoBool("InfoOnly"))
    {
        SendSignalToMazdaTestHead(GetDataTag("SwapTestResultToPLC_OK"), GetDataTag("SwapTestResultConfirm"));
        /*
        SystemWrite(GetDataTag("SwapTestResultToPLC_OK"), true);
        SystemWrite(GetDataTag("SwapTestResultToPLC_NOK"), false);
        */
        result = testPass;
    }
    else
    {
        SendSignalToMazdaTestHead(GetDataTag("SwapTestResultToPLC_NOK"), GetDataTag("SwapTestResultConfirm"));
        /*
        SystemWrite(GetDataTag("SwapTestResultToPLC_NOK"), true);
        SystemWrite(GetDataTag("SwapTestResultToPLC_OK"), false)
        */
    }

    /*
    if (!GetParameterBool("SkipWaitForSwapTestCompleteConfirm"))
    {
        bool testCompleteConfirm;
        do
        {
            testCompleteConfirm = SystemReadBool(GetDataTag("SwapTestResultConfirm"));
            if (!testCompleteConfirm)
            {
                Log(LOG_DEV_DATA, "Waiting for brake test complete confirm from Mazda Test Head");
                BposSleep(50);
            }
        }while ((StatusCheck() == BEP_STATUS_SUCCESS) && !testCompleteConfirm && TimeRemaining());
    }

    SystemWrite(GetDataTag("SwapTestResultToPLC_NOK"), false);
    SystemWrite(GetDataTag("SwapTestResultToPLC_OK"), false);
    */

    SendSubtestResultWithDetail(targetWheel + "SwapTest", result, testDetails, description, "0000");
    if (GetTestStepInfoBool("DisableRollDrives") || (BEP_STATUS_SUCCESS != StatusCheck()))
    {
        DisableRollMotors(SPEED_MODE, GetParameterBool("DisableMotorsWaitForZeroSpeed")); 
    }

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaSwapTest() - Exit");
    return result;
}


//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::MonitorSwapTestForces(INT16 targetWheel, float requiredForce,
                                         MaxBrakeData *brakeData, UINT16 samplesToAverage,
                                         const XmlNodeMapItr &testData, string testHeadTestStep)
{ // Front Axle Swap Test (similar to a TCS function)
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MonitorSwapTestForces(targetWheel: %s, minSampleForce: %f) - Enter",
        rollerName[targetWheel].c_str(), requiredForce);
    bool measurementComplete = false;
    UINT16 samplesAboveLimit = GetParameterInt("SwapTestSamplesAboveLimitToStartAverage");
    UINT16 currentSamplesAboveLimit = 0;
    bool testComplete = false;
    INT32 timeout = GetTestStepInfoInt("SampleForceTimeout") > 0 ? GetTestStepInfoInt("SampleForceTimeout") : 3000;
    bool fireRequestConfirmed;
    bool sufficientMaxForce;
    SetSecondaryStartTime();
    do
    {
        // This is basically info only as long as !SwapTestSampleUntilTestHeadComplete
        if (!testComplete)
        {
            testComplete = SystemReadBool(testData->second->getAttribute("TestCompleteTag")->getValue());
            if (testComplete)
            {
                Log(LOG_DEV_DATA, "MonitorSwapTestForces: Test Complete from Mazda Test Head");
                StopTestHeadTest(testHeadTestStep);
            }
        }
        

        // Get the current force for each wheel
        ReadCurrentLoadCellValues(brakeData, LeftFront);
        // update the number of samples above the limit
        currentSamplesAboveLimit = (brakeData[targetWheel].currentForce >= requiredForce) ? currentSamplesAboveLimit + 1 : 0;
        //testComplete = SystemReadBool(testData->second->getAttribute("TestCompleteTag")->getValue());
        for (int roller = LeftFront; roller <= RightRear; roller++)
        {   // Display the current force - is this necessary??
            SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
            if (brakeData[roller].currentForce > brakeData[roller].maxForce)
            {
                brakeData[roller].maxForce = brakeData[roller].currentForce;
                Log(LOG_DEV_DATA, "MonitorSwapTestForces: New max force for %s: %.2f", rollerName[roller].c_str(), brakeData[roller].maxForce);
            }
            if (brakeData[targetWheel].currentForce >= requiredForce)
            {
                if (currentSamplesAboveLimit >= samplesAboveLimit)
                {   // We have enough samples over the threshold, start averaging
                    brakeData[roller].forceSamples.push_back(brakeData[roller].currentForce);
                    //Log(LOG_DEV_DATA, "Begin sampling...");
                    
                    // Check if measurement is complete on this wheel
                    if (brakeData[roller].forceSamples.size() > samplesToAverage)
                    {   // Remove old samples to keep a moving sample window
                        //brakeData[roller].forceSamples.pop_front();
                        measurementComplete = true;
                        brakeData[roller].measurementComplete = true; //GetTestStepInfoBool("SwapTestSampleUntilTestHeadComplete") ? testComplete : true;
                        Log(LOG_DEV_DATA, "MonitorSwapTestForces: Measurement complete for %s - max target wheel force: %.2f, min samples: %d%s",
                            rollerName[roller].c_str(), brakeData[targetWheel].maxForce, samplesToAverage,
                            (GetParameterBool("SwapTestSampleUntilTestHeadComplete") && !testComplete) ? " - waiting for test complete..." : "");
                    }
                }
            }
            else if (brakeData[roller].forceSamples.size() > samplesToAverage ||
                     (GetParameterBool("SwapTestUseMaxBrakeForce") && brakeData[roller].maxForce > requiredForce))
            {
                Log(LOG_DEV_DATA, "MonitorSwapTestForces: Force below threshold, but min samples already reached or required force met... We're ending this.");
                measurementComplete = true;
            }
            else if (currentSamplesAboveLimit > 0 && GetParameterBool("RequireContinuousSamplesOverMinimum"))
            {
                Log(LOG_DEV_DATA, "MonitorSwapTestForces: Force below threshold. Clearing samples...");
                brakeData[roller].forceSamples.clear();
                currentSamplesAboveLimit = 0;
            }

            if (!sufficientMaxForce && 
                (GetParameterBool("SwapTestUseMaxBrakeForce") && brakeData[targetWheel].maxForce >= requiredForce) )
            {
                // Using a different flag here so that we can keep measuring
                sufficientMaxForce = true;
                brakeData[roller].measurementComplete = true; 
            }
        }

        if (!measurementComplete && GetParameterBool("SwapTestSampleUntilTestHeadComplete"))
            measurementComplete = (brakeData[targetWheel].measurementComplete && testComplete);
        else
            measurementComplete = (brakeData[targetWheel].measurementComplete);

        // Wait a bit before the next measurement
        if (!measurementComplete)
            BposSleep(GetParameterInt("LoadCellReadingDelay"));
        // Keep checking until all min required forces are satisfied
    } while ((BEP_STATUS_SUCCESS == StatusCheck()) && SecondaryTimeRemaining(&timeout) && !measurementComplete);

    if (GetParameterBool("SwapTestUseMaxBrakeForce") && sufficientMaxForce)
        measurementComplete = true;

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MonitorSwapTestForces - Exit");
    // Return the completion flag
    return measurementComplete;
}


//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::RunBrakeTest()
{
    string result(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::RunBrakeTest(wait time: %d) - Enter", GetTestStepInfoInt("Timeout"));
    bool skipOnIncycleRetest = GetTestStepInfoBool("SkipOnIncycleRetest");
    SetStartTime();
    if (!ShortCircuitTestStep() && 
        ((skipOnIncycleRetest && !SystemReadBool(GetDataTag("IncycleRetest"))) || !skipOnIncycleRetest))
    {
        SystemWrite(GetDataTag("RunBrakeTestTag"), true);
        // Wait for brake test confirm
        bool brakeTestConfirm;
        do
        {
            brakeTestConfirm = SystemReadBool(GetDataTag("RunBrakeTestConfirm"));
            if (!brakeTestConfirm)
            {
                Log(LOG_DEV_DATA, "Waiting for brake test requirement confirm from Mazda Test Head");
                BposSleep(50);
            }
        }while ((StatusCheck() == BEP_STATUS_SUCCESS) && !brakeTestConfirm && TimeRemaining());
        if (brakeTestConfirm)
            result = testPass;
        else
            result = testFail;
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping RunBrakeTest %s",
            (skipOnIncycleRetest && SystemReadBool(GetDataTag("IncycleRetest"))) ? "-- skipping on Incycle Retest" : "");
        result = testSkip;
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::RunBrakeTest() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE MazdaBrakeTC::SendSignalToMazdaTestHead(const string outgoingSignalTag, string confirmationSignal /* = "Undefined"*/, 
                                                        bool signalStatus /*= true*/)
{
    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE; 
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StartTestHeadTest (signal: %s, status: %s) -- Enter", 
        outgoingSignalTag.c_str(), signalStatus ? "true" : "false");

    INT32 timeout = GetParameterInt("MazdaTestHeadSignalHandshakeTimeout") > 0 ? GetTestStepInfoInt("MazdaTestHeadSignalHandshakeTimeout") : 10000;
    SetSecondaryStartTime(); 

    // Send outgoing
    status = SystemWrite(GetDataTag(outgoingSignalTag), signalStatus) == BEP_STATUS_SUCCESS ? 
        BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    
    if (status == BEP_STATUS_SUCCESS && confirmationSignal != "Undefined")
    {
        Log(LOG_DEV_DATA, "Waiting for confirmation: %s", confirmationSignal.c_str());
        bool confirmationReceived;
        do
        {
            confirmationReceived = SystemReadBool(GetDataTag(confirmationSignal));
            if (!confirmationReceived)
            {
                //Log(LOG_DEV_DATA, "Waiting for brake test complete confirm from Mazda Test Head");
                BposSleep(50);
            }
        }while ((StatusCheck() == BEP_STATUS_SUCCESS) && !confirmationReceived && SecondaryTimeRemaining(&timeout));

        if (confirmationReceived || GetParameterBool("AlwaysReverseTestHeadSignal"))
        {
            Log(LOG_DEV_DATA, "Confirmation %s received, reversing outgoing signal: %s", confirmationReceived ? "" : "NOT", outgoingSignalTag.c_str());
            // Reverse outgoing
            SystemWrite(GetDataTag(outgoingSignalTag), !signalStatus); 
            status = BEP_STATUS_SUCCESS;
        }
        else
            Log(LOG_ERRORS, "No confirmation signal received... not reversing outgoing signal!");
    }
    else
    {
        Log(LOG_ERRORS, "Error sending outgoing signal!"); 
    }
    
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::StartTestHeadTest -- Exit");
    return status;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::SwapTestByAxle(const string axle)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SwapTestByAxle(%s) -- Enter", axle.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {
        if (strnicmp("front", axle.c_str(), 5) == 0)
        {
            RunBrakeTest();
            testResult = MazdaSwapTest("LeftFront");
            if (testResult == testPass)
            {
                RunBrakeTest(); 
                testResult = MazdaSwapTest("RightFront");
            }
            else
            {
                Log(LOG_ERRORS, "Failed swap test for LeftFront wheel skipping RightFront!");
            }
        }
        else
        {
            // Assume Rear
            RunBrakeTest(); 
            testResult = MazdaSwapTest("LeftRear");
            if (testResult == testPass)
            {
                RunBrakeTest(); 
                testResult = MazdaSwapTest("RightRear");
            }
            else
            {
                Log(LOG_ERRORS, "Failed swap test for LeftRear wheel skipping RightRear!");
            }
        }
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SwapTestByAxle(%s) -- Exit", axle.c_str());
    return testResult;
}


