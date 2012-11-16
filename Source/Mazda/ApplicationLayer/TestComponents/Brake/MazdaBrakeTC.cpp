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

//-------------------------------------------------------------------------------------------------
MazdaBrakeTC::MazdaBrakeTC() : GenericBaseBrakeTC()
{   // Nothing special to do here
    
}

//-------------------------------------------------------------------------------------------------
MazdaBrakeTC::~MazdaBrakeTC()
{   // Nothing special to do here
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
        if(!GetTestStepName().compare("AccelerateRollers"))  
        {   // Set the brake ground colors to indicate testing
            SystemWrite(GetDataTag("LFBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
            SystemWrite(GetDataTag("RFBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
            SystemWrite(GetDataTag("LRBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
            SystemWrite(GetDataTag("RRBrakeDistanceColorTag"), GetParameter("TestInProgressColor"));
            testResult = AccelerateVehicleToSpeed(atof(GetVehicleParameter("BrakeDistanceTestSpeed").c_str()), true, true);
        }
        else if(!GetTestStepName().compare("BrakeStoppingDistanceTest"))   testResult = BrakeDistanceTest();
        else if(!GetTestStepName().compare("MaxBrakeForceMeasurement"))    testResult = MaxBrakeForceVerification();
        //else if(!GetTestStepName().compare("MaxBrakeForceCalibration"))    testResult = MaxBrakeForceCalibration();
        else if(!GetTestStepName().compare("ParkBrakeForceMeasurement"))   testResult = ParkBrakeForceVerification();
        //else if(!GetTestStepName().compare("ParkBrakeForceCalibration"))   testResult = ParkBrakeForceCalibration();
        else if(!GetTestStepName().compare("PreShiftCheckout"))            testResult = PreShiftCheck();
        else if(!GetTestStepName().compare("TractionControlTest"))         testResult = TractionControlCheck();

        // mazda drag test
        else if(!GetTestStepName().compare("MazdaDragTest"))               testResult = MazdaDragTest();
        else if(!GetTestStepName().compare("AnalyzeMazdaDragTest"))        testResult = AnalyzeMazdaDragTest();

        // mazda std brake (front axle)
        else if(!GetTestStepName().compare("MazdaFrontBrakeForce"))        testResult = MazdaBrakeForceTest("Front");
        else if(!GetTestStepName().compare("AnalyzeMazdaFrontBrakeForce")) testResult = MazdaAnalyzeBrakeForce("Front");

        // mazda park brake (rear axle)
        else if(!GetTestStepName().compare("MazdaRearBrakeForce"))         testResult = MazdaBrakeForceTest("Rear");
        else if(!GetTestStepName().compare("AnalyzeMazdaRearBrakeForce"))  testResult = MazdaAnalyzeBrakeForce("Rear");
                                                                                                                                  
        // mazda park brake (rear axle)
        else if(!GetTestStepName().compare("MazdaParkBrakeForce"))         testResult = MazdaBrakeForceTest("ParkBrake");
        else if(!GetTestStepName().compare("AnalyzeMazdaParkBrakeForce"))  testResult = MazdaAnalyzeBrakeForce("ParkBrake");
        
        else if(!GetTestStepName().compare("DistanceValidationTest"))            testResult = DistanceValidationTest();
        else if(!GetTestStepName().compare("SpeedSensorCheck"))            testResult = SpeedSensorCheck();
        else if(!GetTestStepName().compare("FinishUp"))                    testResult = FinishUp();
        else if(!GetTestStepName().compare("Initialize"))                   testResult = InitializeBrake();
        else 
            testResult = GenericBaseBrakeTC::CommandTestStep(value);
    }
    else
    {   // System is not OK to perform brake test
        testResult = testSkip;
        Log(LOG_ERRORS, "System status is preventing test: %s", ConvertStatusToResponse(StatusCheck()).c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::CommandTestStep(%s) - Exit", value.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::InitializeBrake()
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::Initialize() - Enter");
    string result(BEP_TESTING_STATUS);
    string testResult(BEP_TESTING_STATUS);

    // setup variables
    m_continueBrakeTest = true;


    if(FullAutoMode())
    {
        Log(LOG_DEV_DATA, "Using full auto mode. Reading the weight from the PLC");
        // get mass of axle
        m_frontAxleMass = SystemReadFloat("FrontAxleMass");
        m_rearAxleMass = SystemReadFloat("RearAxleMass");
    }
    else
    {
        // TODO: Implement half auto keypad. Pull axle mass from build info
    }


    Log(LOG_DEV_DATA, "Front Axle Mass: %.2f   Rear Axle Mass: %.2f", m_frontAxleMass, m_rearAxleMass);

    // do the standard finish up stuff
    ResetSystemTags();
    testResult = TestStepInitialize();


    Log(LOG_FN_ENTRY, "MazdaBrakeTC::Initialize() - Exit result: %s", testResult.c_str());

    return(testResult);
}

bool MazdaBrakeTC::FullAutoMode()
{
    // TODO implment bit from PLC telling us if it is in full auto mode
    return true;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::FinishUp()
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::FinishUp() - Enter");
    string result(BEP_TESTING_STATUS);
    string testResult(BEP_TESTING_STATUS);

    if(!ShortCircuitTestStep())
    {
        // turn the motors off
        ZeroMotorSpeedandTorque();

        // make sure the rollers are at zerospeed before you try to complete the finish up routine.
        if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(0.00, 0.00, 0.00, 0.00, 30000))
        {
            // give it a little more time to settle
            delay(300);

            // verify that the machine says we are at zerospeed
            if(ReadSubscribeData(GetDataTag("Zerospeed")) == "1")
            {
                // do the standard finish up stuff
                testResult = GenericBaseBrakeTC::TestStepFinishUp();
            }
            else
            {
                Log(LOG_ERRORS, "machine says we are not at zerospeed");
            }
        }
        else
        {
            Log(LOG_ERRORS, "System was not able to reach zerospeed");
        }

    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping FinishUp test");
    }

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::FinishUp() - Exit result: %s", testResult.c_str());

    return(testResult);
}


//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::AnalyzeMazdaDragTest()
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeMazdaDragTest() - Enter");
    string result(BEP_TESTING_STATUS);
    string testResult(BEP_TESTING_STATUS);

    float flAvg = -1.0;
    float frAvg = -1.0;
    float rlAvg = -1.0;
    float rrAvg = -1.0;

    float flMax = -1.0;
    float frMax = -1.0;
    float rlMax = -1.0;
    float rrMax = -1.0;

    if(!ShortCircuitTestStep())
    {
        Log(LOG_DEV_DATA, "AnalyzeMazdaDragTest size of force arrays: LF size: %d  RF size: %d    LR size: %d   RR size: %d", 
                                        m_flForceValue.size(), m_frForceValue.size(), m_rlForceValue.size(), m_rrForceValue.size());

        flAvg = computeListAverage(m_flForceValue, flMax);
        frAvg = computeListAverage(m_frForceValue, frMax);
        rlAvg = computeListAverage(m_rlForceValue, rlMax);
        rrAvg = computeListAverage(m_rrForceValue, rrMax);



        Log(LOG_DEV_DATA, "AnalyzeMazdaDragTest average forces: LF: %.2f  RF: %.2f    LR: %.2f   RR: %.2f", flAvg, frAvg, rlAvg, rrAvg); 

        m_frontAxleAvgDrag = flAvg + frAvg;
        m_rearAxleAvgDrag = rlAvg + rrAvg;

        string frontTestResult = AnalyzeDragTestResults("Front", flAvg, frAvg);
        string rearTestResult = AnalyzeDragTestResults("Rear", rlAvg, rrAvg);

        
        if(BEP_PASS_RESPONSE == frontTestResult && BEP_PASS_RESPONSE == rearTestResult)
        {
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
        }

        // clear the force lists for the next trial
        m_flForceValue.clear();
        m_frForceValue.clear();
        m_rlForceValue.clear();
        m_rrForceValue.clear();



    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping AnalyzeMazdaDragTest test");
    }

    char buff[60];

    SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                             "LfAverageDragForce", CreateMessage(buff, sizeof(buff), "%.2f", flAvg), "N",
                             "RfAverageDragForce", CreateMessage(buff, sizeof(buff), "%.2f", frAvg), "N",
                             "LrAverageDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rlAvg), "N",
                             "LfAverageDragForce", CreateMessage(buff, sizeof(buff), "%.2f", flAvg), "N",
                             "RfMaxDragForce", CreateMessage(buff, sizeof(buff), "%.2f", frAvg), "N",
                             "LrMaxDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rlAvg), "N",
                             "RrMaxDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rrAvg), "N",
                             "RrMaxDragForce", CreateMessage(buff, sizeof(buff), "%.2f", rrAvg), "N");

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeMazdaDragTest() - Exit result: %s", testResult.c_str());

    return(testResult);
}

string MazdaBrakeTC::AnalyzeDragTestResults(string axle, float rightWheelAvg, float leftWheelAvg)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeAndSendDragTestResults - Enter");

    string testResult(BEP_TESTING_STATUS);

    bool axleDragSumPassed = false;
    bool axleDragDiffPassed = false;

    float sum = rightWheelAvg + leftWheelAvg;

    if ((GetParameterFloat(axle+"AxleMinDrag") > (sum)) &&
        (sum) < GetParameterFloat(axle+"AxleMaxDrag"))
    {
        axleDragSumPassed = true;
    }

    float diff = fabs(rightWheelAvg - leftWheelAvg);
    if ((GetParameterFloat(axle+"AxleMinDragDiff") > (sum)) &&
        (sum) < GetParameterFloat(axle+"AxleMaxDragDiff"))
    {
        axleDragDiffPassed = true;
    }

     Log(LOG_DEV_DATA, "Axle: %s   right: %.2f  left: %.2f  MaxDrag: %.2f  MaxDragDiff: %.2f",
                                        axle.c_str(), rightWheelAvg, leftWheelAvg, 
                                        GetParameterFloat(axle+"AxleMaxDrag"),GetParameterFloat(axle+"AxleMaxDragDiff"));  

    testResult = (axleDragSumPassed + axleDragDiffPassed) ? BEP_PASS_RESPONSE : BEP_FAIL_STATUS; 


    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeAndSendDragTestResults - Exit (%s)", testResult.c_str() );

    return (testResult);
}
//-------------------------------------------------------------------------------------------------
float MazdaBrakeTC::computeListAverage(list<signed int> arr, float *max)
{   // Log the entry and determine if this should be checked

    
    std::list<signed int>::iterator it;
    float sum = 0.0;
    float average = -1.0;
    float maxVal = -1.0;
    int s = arr.size();
    for(it=arr.begin() ; it != arr.end(); it++)
    {
        if( *it > maxVal )
            max = maxVal;

        sum += *it;
    }

    if(arr.size() > 0)
        average = sum/((float) arr.size());

    Log(LOG_DEV_DATA, "computeListAverage - size: %d  sum: %.2f   average: %.2f   max: %.2f", arr.size(), sum, average, max);
    return average;
}


//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaAnalyzeBrakeForce(string axle)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAnalyzeBrakeForce(%s) - Enter", axle.c_str());
    string result(BEP_TESTING_STATUS);
    string testResult(BEP_TESTING_STATUS);

    float flAvg = -1.0;
    float frAvg = -1.0;
    float rlAvg = -1.0;
    float rrAvg = -1.0;

    float flMax = -1.0;
    float frMax = -1.0;
    float rlMax = -1.0;
    float rrMax = -1.0;


    if(!ShortCircuitTestStep() && m_continueBrakeTest)
    {
        Log(LOG_DEV_DATA, "MazdaAnalyzeBrakeForce (%s) size of force arrays: LF size: %d  RF size: %d    LR size: %d   RR size: %d", 
                                        axle.c_str(), m_flForceValue.size(), m_frForceValue.size(), m_rlForceValue.size(), m_rrForceValue.size());

        flAvg = computeListAverage(m_flForceValue);
        frAvg = computeListAverage(m_frForceValue);
        rlAvg = computeListAverage(m_rlForceValue);
        rrAvg = computeListAverage(m_rrForceValue);

        Log(LOG_DEV_DATA, "MazdaAnalyzeBrakeForce (%s) average forces: LF: %.2f  RF: %.2f    LR: %.2f   RR: %.2f", axle.c_str(), flAvg, frAvg, rlAvg, rrAvg); 

        string frontTestResult = AnalyzeBrakeTestResults(axle, flAvg, frAvg);
        string rearTestResult = AnalyzeBrakeTestResults(axle, rlAvg, rrAvg);

        if(BEP_PASS_RESPONSE == frontTestResult && BEP_PASS_RESPONSE == rearTestResult)
        {
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
        }

        // clear the force lists for the next trial
        m_flForceValue.clear();
        m_frForceValue.clear();
        m_rlForceValue.clear();
        m_rrForceValue.clear();



    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping AnalyzeMazdaDragTest test    continue: %d", m_continueBrakeTest);
    }

    char buff[60];

    SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                     "Lf"+axle+"BrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", flAvg), "N",
                     "Rf"+axle+"BrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", frAvg), "N",
                     "Lr"+axle+"BrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rlAvg), "N",
                     "Rr"+axle+"BrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", rrAvg), "N");


    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAnalyzeBrakeForce() - Exit result: %s", testResult.c_str());

    return(testResult);
}

string MazdaBrakeTC::AnalyzeBrakeTestResults(string axle, float rightWheelAvg, float leftWheelAvg)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeBrakeTestResults - Enter");

    string testResult(BEP_TESTING_STATUS);

    bool axleDragSumPassed = false;
    bool axleDragDiffPassed = false;
    float dragCoeff = -1.0;
    float axleWeight = -1.0;
    float value;
    float sum = rightWheelAvg + leftWheelAvg;
    

    // if it is the front wheel, use the front drag. Else use the rears (it will cover the "ParkBrake" and "Rear" cases)
    dragCoeff = (axle == "Front") ? m_frontAxleAvgDrag : m_rearAxleAvgDrag;
    axleWeight = (axle == "Front") ? m_frontAxleMass : m_rearAxleMass;

    Log(LOG_DEV_DATA, "sum: %.2f   dragCoeff: %.2f   WeightBrakeForce: %f   axle weight: %.2f", sum, dragCoeff, GetTestStepInfo("WeightBrakeForce", value), axleWeight);
    sum = sum - dragCoeff;
    
    if( sum > (axleWeight * (GetTestStepInfo("WeightBrakeForce", value))/100.0))
    {
        axleDragSumPassed = true;
    }

    float diff = fabs(rightWheelAvg - leftWheelAvg);
    Log(LOG_DEV_DATA, "diff: %.2f   dragCoeff: %.2f   BrakeForceDiff: %f", diff, dragCoeff, GetTestStepInfo("BrakeForceDiff", value));

    diff = diff - dragCoeff;


    if (diff < (sum * (GetTestStepInfo("BrakeForceDiff", value)/100.0)))
    {
        axleDragDiffPassed = true;
    }

     Log(LOG_DEV_DATA, "Axle: %s   right: %.2f  left: %.2f", axle.c_str(), rightWheelAvg, leftWheelAvg);  

    testResult = (axleDragSumPassed + axleDragDiffPassed) ? BEP_PASS_RESPONSE : BEP_FAIL_STATUS; 


    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AnalyzeBrakeTestResults - Exit (%s)", testResult.c_str() );

    return (testResult);
}


//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaBrakeForceTest(string axle)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeForceTest(%s) - Enter", axle.c_str());
    string result(BEP_TESTING_STATUS);
    string testResult(BEP_TESTING_STATUS);
    bool keepTesting = true;
    if(!ShortCircuitTestStep() && m_continueBrakeTest)
    {
        do
        {
            if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("FLSpeed"+axle+"Force"), 
                                                                    GetParameterFloat("FRSpeed"+axle+"Force"), 
                                                                    GetParameterFloat("RLSpeed"+axle+"Force"), 
                                                                    GetParameterFloat("RRSpeed"+axle+"Force"), GetParameterInt("Wheelspeed"+axle+"ForceTestAccelTimeout")))
            {
                if(UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
    
                Log(LOG_DEV_DATA, "Have the operator apply the brake. Force should be seen on %s axle", axle.c_str());
    
                int samples = GetParameterFloat("Number"+axle+"Samples");
                INT32 tempVar = 0;
                int16_t tempVarShort = 0;
                // gather all of the samples that are specified
                while(0 < samples)
                {
                    Log(LOG_DEV_DATA, "Continuing to sample forces. %d samples remaining", samples);
    
                    tempVar = SystemReadInt(GetDataTag("LFCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "LF Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_flForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("RFCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "RF Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_frForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("LRCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "LR Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_rlForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("RRCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "RR Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_rrForceValue.push_back(tempVarShort);
    
    
                    BposSleep(GetParameterFloat(axle+"SampleRate"));
    
                    samples--;
                }
    
                //compute test results
                testResult = MazdaAnalyzeBrakeForce(axle);

                if(testPass == testResult)
                {
                    keepTesting = false;
                }
                else
                {
                    if(OperatorYesNo() != "Yes")
                    {
                        keepTesting = false;
                        testResult = testAbort;
                        m_continueBrakeTest = false;
                    }

                }

            }
            else
            {
                Log(LOG_DEV_DATA, "MazdaBrakeTC::MazdaBrakeForceTest - Couldn't reach required wheelspeeds");
            }
        }while(keepTesting  || testPass == testResult);
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping MazdaBrakeForceTest test    %d",m_continueBrakeTest);
    }

    RemovePrompts();

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaBrakeForceTest() - Exit result: %s", testResult.c_str());

    return(testResult);
}

void MazdaBrakeTC::ZeroMotorSpeedandTorque(void)
{

    // Go to torque 0 first to clear any command history in the motor controller
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);
    BposSleep( 100);

}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaDragTest(void)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaDragTest() - Enter");

    string testResult(BEP_TESTING_RESPONSE);
    string result(BEP_TESTING_STATUS);
    bool keepTesting = true;


    if(!ShortCircuitTestStep() && m_continueBrakeTest)
    { 
        do
        {
            ZeroMotorSpeedandTorque();
    
    
            // Display prompts for accelerating the wheels (will be cleared automatically)
            DisplayAccelPrompts();


            if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("FLSpeedBaseDrag"), 
                                                                    GetParameterFloat("FRSpeedBaseDrag"), 
                                                                    GetParameterFloat("RLSpeedBaseDrag"), 
                                                                    GetParameterFloat("RRSpeedBaseDrag"), GetParameterInt("WheelspeedDragTestAccelTimeout")))
            {
                Log(LOG_DEV_DATA, "Wheelspeeds are in range. Now calling base drag test.");
    
                Log(LOG_DEV_DATA, "Begining to coast down.");
    
                // turn off the motors to get an awesome coasting event
                //Disengage();
                //ZeroMotorSpeedandTorque();
    
                DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                              GetPromptPriority("RemoveFootFromBrake"));
    
                int samples = 50;
                INT32 tempVar = 0;
                int16_t tempVarShort = 0;
    
    
                // maybe start this in another thread so you can do fancy stuff
                while(samples > 0)
                {
                    Log(LOG_DEV_DATA, "Continuing to sample forces. %d samples remaining", samples);
    
                    tempVar = SystemReadInt(GetDataTag("LFCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "LF Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_flForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("RFCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "RF Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_frForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("LRCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "LR Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_rlForceValue.push_back(tempVarShort);
    
                    tempVar = SystemReadInt(GetDataTag("RRCurrentForce"));
                    tempVarShort = (int16_t)tempVar;
                    Log(LOG_DEV_DATA, "RR Force: %d (%x)   %d (%x)",tempVarShort, tempVarShort, tempVar, tempVar);
                    m_rrForceValue.push_back(tempVarShort);
    
                    BposSleep(100);
    
                    samples--;
                }
    
                RemovePrompt(GetPrompt("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
    
                // run the full test analysis
                testResult = AnalyzeMazdaDragTest();
                if(testPass == testResult)
                {
                    keepTesting = false;
                }
                else
                {
                    if(OperatorYesNo() != "Yes")
                    {
                        keepTesting = false;
                        testResult = testAbort;
                        m_continueBrakeTest = false;
                    }

                }


            }
            else
            {
                Log(LOG_DEV_DATA, "MazdaBrakeTC::MazdaDragTest() - Could not reach speeds");
            }
        }while(keepTesting || testPass == testResult);


        
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping MazdaDragTest test   continue: %d", m_continueBrakeTest);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaDragTest() - Exit");
    return testResult;
}

void MazdaBrakeTC::DisplayAccelPrompts(void)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::DisplayAccelPrompts() - Enter");

    DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                  GetPromptPriority("RemoveFootFromBrake"));
    DisplayTimedPrompt(GetPrompt("NeutralPrompt"), GetPromptBox("NeutralPrompt"), GetPromptPriority("NeutralPrompt"),
                       GetPromptDuration("NeutralPrompt"));
    RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                 GetPromptPriority("RemoveFootFromBrake"));

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::DisplayAccelPrompts() - Exit");
}

//-------------------------------------------------------------------------------------------------
BEP_STATUS_TYPE MazdaBrakeTC::WaitForWheelSpeedsToBeReached(float FLTargetSpeed,
                                                            float FRTargetSpeed,
                                                            float RLTargetSpeed,
                                                            float RRTargetSpeed, int timeout)
{

    BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;

    const int pollrate = 50;
    bool lfSpeedReached = false;
    bool rfSpeedReached = false;
    bool lrSpeedReached = false;
    bool rrSpeedReached = false;

    char    temp[256];

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForWheelSpeedsToBeReached() - Enter");

    // set the motor mode
    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightRearMotorMode", SPEED_MODE, false);

    // set the motor speeds
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", FLTargetSpeed), false);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", FRTargetSpeed), false);
    m_MotorController.Write("LeftRearSpeedValue", CreateMessage(temp, 256, "%.2f", RLTargetSpeed), false);
    m_MotorController.Write("RightRearSpeedValue", CreateMessage(temp, 256, "%.2f", RRTargetSpeed), true);

    // sleep a little for good measure
    BposSleep( 100);

    Log(LOG_DEV_DATA, "WaitForWheelSpeedsToBeReached: LF: %.2f  RF: %.2f  LR: %.2f  RR: %.2f", FLTargetSpeed, FRTargetSpeed, RLTargetSpeed, RRTargetSpeed);

    while(timeout > 0)
    {

        // check if all of the wheel speeds are in range
        lfSpeedReached = IsWheelInSpeedRange(GetDataTag("LFSpeed"), FLTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        rfSpeedReached = IsWheelInSpeedRange(GetDataTag("RFSpeed"), FRTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        lrSpeedReached = IsWheelInSpeedRange(GetDataTag("LRSpeed"), RLTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));
        rrSpeedReached = IsWheelInSpeedRange(GetDataTag("RRSpeed"), RRTargetSpeed, GetParameterFloat("SetWheelspeedTolerance"));

        Log(LOG_DEV_DATA, "Wheelspeeds reached: LF: %d  RF: %d  LR: %d  RR: %d", lfSpeedReached, rfSpeedReached, lrSpeedReached, rrSpeedReached);

        if (lfSpeedReached && rfSpeedReached && lrSpeedReached && rrSpeedReached)
        {
            status = BEP_STATUS_SUCCESS;
            break;
        }
        else
        {
            Log(LOG_DEV_DATA, "Not all wheels are in speed range");
        }
        
        timeout -= pollrate;
        Log(LOG_DEV_DATA, "Delaying for another %dms. %dms remaining", pollrate, timeout);
        delay(pollrate);
    }


    Log(LOG_FN_ENTRY, "MazdaBrakeTC::WaitForWheelSpeedsToBeReached() - Exit    status: %s", ConvertStatusToResponse(status).c_str());
    

    return (status);
}

bool MazdaBrakeTC::IsWheelInSpeedRange(string wheelTag, const float targetSpeed, float tolerance)
{
    bool status = false;
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::IsWheelInSpeedRange() - Enter");

    float currSpeed = SystemReadFloat(wheelTag);
    float minSpeed = targetSpeed - (targetSpeed * (tolerance / 100.0));
    float maxSpeed = targetSpeed + (targetSpeed * (tolerance / 100.0));

    Log(LOG_DEV_DATA, "Verify %s is in acceptable range. minSpeed: %.2f  currSpeed: %.2f  maxSpeed: %.2f", wheelTag.c_str(), minSpeed, currSpeed, maxSpeed);

    if (minSpeed <= currSpeed && currSpeed <= maxSpeed)
    {
        status = true;
    }
    else
    {
        Log(LOG_DEV_DATA, "%s is outside acceptable range.", wheelTag.c_str());
    }
    

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::IsWheelInSpeedRange() - Exit status: %d", status);

    return (status);
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::AccelerateVehicleToSpeed(const float targetSpeed, 
                                               const bool startAtZeroSpeed, 
                                               const bool disengageAtSpeed)
{   // Log the entry and make sure the test step should be performed
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AccelerateVehicleToSpeed() - Enter");
    string testResult(BEP_TESTING_STATUS);
    if(!ShortCircuitTestStep() && m_continueBrakeTest)
    {   // Instruct the machine to engage the motors
        SystemWrite(GetDataTag("EngageEbsMotors"), true);
        // Instruct the operator to shift to Neutral and prepare for acceleration
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                      GetPromptPriority("RemoveFootFromBrake"));
        DisplayTimedPrompt(GetPrompt("NeutralPrompt"), GetPromptBox("NeutralPrompt"), GetPromptPriority("NeutralPrompt"),
                           GetPromptDuration("NeutralPrompt"));
        RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                     GetPromptPriority("RemoveFootFromBrake"));
        // Wait for the wheel speeds to reach the target speed
        while((BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining() && (GetRollSpeed() < targetSpeed)) BposSleep(250);
        // Determine the result
        if(BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status
            Log(LOG_ERRORS, "Bad system status during roller acceleration - %s", 
                ConvertStatusToResponse(StatusCheck()).c_str());
            testResult = testFail;
        }
        else if(!TimeRemaining())
        {   // Timeout waiting for target speed
            Log(LOG_ERRORS, "Timeout waiting for rollers to reach target speed - %.2f [current speed: %.2f]",
                targetSpeed, GetRollSpeed());
            testResult = testTimeout;
        }
        else if(GetRollSpeed() >= targetSpeed)
        {
            Log(LOG_DEV_DATA, "Target speed reached");
            testResult = testPass;
        }
        // Disengage the ABS motors once the target speed is reached
        SystemWrite(GetDataTag("EngageEbsMotors"), false);
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping MazdaBrakeTC::AccelerateVehicleToSpeed()   continue: %d", m_continueBrakeTest);
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::AccelerateVehicleToSpeed() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::SpeedSensorCheck(void)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    string result = "";
    if(!ShortCircuitTestStep() && m_continueBrakeTest)
    {  
        // check if ABS is equipped
        if(EquippedABS())
        {
            SystemWrite("M076", true);

            // wait for rollers to accelerate to within range and before the timeout occurs
            if(BEP_STATUS_SUCCESS == WaitForWheelSpeedsToBeReached(GetParameterFloat("FLSpeedSensorCheck"), 
                                                                    GetParameterFloat("FRSpeedSensorCheck"), 
                                                                    GetParameterFloat("RLSpeedSensorCheck"), 
                                                                    GetParameterFloat("RRSpeedSensorCheck"), GetParameterInt("WheelspeedSensorCheckAccelTimeout")))
            {
                // if rollers are within speed range and wait for the confirmation bit was set
                if (BEP_SUCCESS_RESPONSE == SystemReadWaitForResult("M026", "1", 1000.0))
                {
                     result = StartSystemSpeedCheck();
                }

            }

        }
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping SpeedSensorCheck test  continue: %d", m_continueBrakeTest);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Exit");
    return testResult;
}

string MazdaBrakeTC::StartSystemSpeedCheck(void)
{

    std::string result = BEP_ERROR_RESPONSE;
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Enter");

    SystemWrite("M076", true);

    // Wait for software confirmation signal to be set
    if (SystemReadWaitForResult("M026", "1", 1000.0) == BEP_SUCCESS_RESPONSE)
    {
        Log(LOG_DEV_DATA, "M026 was set, spot 1");
        SystemWrite("M068", true);
        SystemWrite("M069", true);
        SystemWrite("M072", true);
        SystemWrite("M073", true);
        SystemWrite("M076", false);

        if (SystemReadWaitForResult("M026", "1", 1000.0) == BEP_SUCCESS_RESPONSE)
        {
            Log(LOG_DEV_DATA, "M026 was set, spot 2");
            SystemWrite("M052", true);
            SystemWrite("M068", false);
            SystemWrite("M069", false);
            SystemWrite("M072", false);
            SystemWrite("M073", false);

            if (SystemReadWaitForResult("M018", "0", 1000.0) == BEP_SUCCESS_RESPONSE &&
                SystemReadWaitForResult("M019", "0", 1000.0) == BEP_SUCCESS_RESPONSE && 
                SystemReadWaitForResult("M022", "0", 1000.0) == BEP_SUCCESS_RESPONSE &&
                SystemReadWaitForResult("M023", "0", 1000.0) == BEP_SUCCESS_RESPONSE )
            {
                SystemWrite("M052", false);
                Log(LOG_DEV_DATA, "Test was passed");
                result = BEP_SUCCESS_RESPONSE;            
            }
            else
            {
                Log(LOG_ERRORS, "Error reading wait for result. %s %s %s %s", SystemRead("M018").c_str(), SystemRead("M019").c_str(), SystemRead("M022").c_str(), SystemRead("M023").c_str());
            }

        }
        else
        {
            Log(LOG_ERRORS, "Error waiting for M026 (posB), bit was never set");
        }   
    }
    else
    {
        Log(LOG_ERRORS, "Error waiting for M026 (posA), bit was never set");
    }


    return (result);
}

bool MazdaBrakeTC::EquippedABS(void)
{
    bool absEquipped = false;
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Enter");

    absEquipped = true;

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::SpeedSensorCheck() - Exit  Abs Equipped: %s", (absEquipped ? "True" : "False"));

    return absEquipped;
}

string MazdaBrakeTC::DistanceValidationTest(void)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::DistanceValidationTest() - Enter");

    float travelDist = GetParameterFloat("TravelDistance");
    float timeout = GetParameterFloat("TravelDistanceTimeout");
    float pollRate = GetParameterFloat("TravelDistancePollRate");

    float distanceTravled = 0.0;

    float circumference = 3.1415927 * GetParameterFloat("RollerDiameter");
    
    WHEELINFO initialDistance, totalDistance, finalDistance;
    GetWheelDistances(initialDistance);

    m_MotorController.Write("LeftFrontMotorMode", BOOST_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", BOOST_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", BOOST_MODE, false);
    m_MotorController.Write("RightRearMotorMode", BOOST_MODE, false);    

    Log(LOG_DEV_DATA, "DistanceValidationTest -- Param. circumference: %.2f", circumference);

    DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));

    while(timeout > 0 && distanceTravled < travelDist)
    {
        GetWheelDistances(finalDistance);
        GetTotalDistances(totalDistance, initialDistance, finalDistance);

        Log(LOG_DEV_DATA, "Dist Values - LF Dist: %.2f  LR Dist: %.2f\n", totalDistance.lfWheel, totalDistance.lrWheel);

        // (# revolutions) * circumfrence
        float aa = (totalDistance.lfWheel / GetParameterFloat("PulsesPerRevolution"));
        float bb = (aa * circumference);
        float lfDist = bb / GetParameterFloat("InchesPerMile");
        Log(LOG_DEV_DATA, "Dist calc: aa: %.2f  bb: %.2f  lfDist:%.2f\n", aa,bb,lfDist);
        float lrDist = ((totalDistance.lrWheel / GetParameterFloat("PulsesPerRevolution")) * circumference) / GetParameterFloat("InchesPerMile");

        distanceTravled = (lfDist + lrDist) / 2.0;

        Log(LOG_DEV_DATA, "LF Dist: %.2f  LR Dist: %.2f    Distance: %.2f / %.2f   Timeout: %.2fms remaining", lfDist, lrDist, distanceTravled,travelDist,timeout);  

        timeout -= pollRate;
        BposSleep((int)pollRate); 
    }

    RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::DistanceValidationTest() - Exit");

    return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::BrakeDistanceTest(void)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::BrakeDistanceTest() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep())
    {   // Wait for vehicle speed to drop a bit
        DisplayPrompt(GetPromptBox("BrakeToStop"), GetPrompt("BrakeToStop"), GetPromptPriority("BrakeToStop"));
        while((GetRollSpeed() > atof(GetVehicleParameter("BrakeDistanceTestStartMeasureSpeed").c_str())) &&
              (BEP_STATUS_SUCCESS == StatusCheck()))
        {
            BposSleep(100);
        }
        // Set the start distance
        WHEELINFO initialDistance;
        GetWheelDistances(initialDistance);
        // Wait for zero speed
        CheckZeroSpeed();
        RemovePrompt(GetPromptBox("ZeroSpeed"), GetPrompt("ZeroSpeed"), GetPromptPriority("ZeroSpeed"));
        // Get the final distances
        WHEELINFO finalDistance;
        GetWheelDistances(finalDistance);
        // Find the delta
        WHEELINFO stoppingDistance;
        GetTotalDistances(stoppingDistance, initialDistance, finalDistance);
        // Convert the data to meters
        float circumference = 3.1415927 * GetParameterFloat("RollerDiameter");

        float revs = stoppingDistance.lfWheel / GetParameterFloat("PulsesPerRevolution");
        float lfDist = (revs * circumference) / 39.36996;
        Log(LOG_DEV_DATA, "LF Stopping Distance: %.2f", lfDist);
        bool lfPass = lfDist < GetVehicleParameter("FrontMaximumStoppingDistance", float(0.0));
        SystemWrite(GetDataTag("LFBrakeDistanceTag"), lfDist);
        SystemWrite(GetDataTag("LFBrakeDistanceColorTag"), lfPass ? string("green") : string("red"));

        revs = stoppingDistance.rfWheel / GetParameterFloat("PulsesPerRevolution");
        float rfDist = (revs * circumference) / 39.36996;
        Log(LOG_DEV_DATA, "RF Stopping Distance: %.2f", rfDist);
        bool rfPass = rfDist < GetVehicleParameter("FrontMaximumStoppingDistance", float(0.0));
        SystemWrite(GetDataTag("RFBrakeDistanceTag"), rfDist);
        SystemWrite(GetDataTag("RFBrakeDistanceColorTag"), rfPass ? string("green") : string("red"));

        revs = stoppingDistance.lrWheel / GetParameterFloat("PulsesPerRevolution");
        float lrDist = (revs * circumference) / 39.36996;
        Log(LOG_DEV_DATA, "LR Stopping Distance: %.2f", lrDist);
        bool lrPass = lrDist < GetVehicleParameter("RearMaximumStoppingDistance", float(0.0));
        SystemWrite(GetDataTag("LRBrakeDistanceTag"), lrDist);
        SystemWrite(GetDataTag("LRBrakeDistanceColorTag"), lrPass ? string("green") : string("red"));

        revs = stoppingDistance.rrWheel / GetParameterFloat("PulsesPerRevolution");
        float rrDist = (revs * circumference) / 39.36996;
        Log(LOG_DEV_DATA, "RR Stopping Distance: %.2f", rrDist);
        bool rrPass = rrDist < GetVehicleParameter("RearMaximumStoppingDistance", float(0.0));
        SystemWrite(GetDataTag("RRBrakeDistanceTag"), rrDist);
        SystemWrite(GetDataTag("RRBrakeDistanceColorTag"), rrPass ? string("green") : string("red"));
        // Determine the overall result
        testResult = (lfPass && rfPass && lrPass && rrPass) ? testPass : testFail;
        char buff[32];
        SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                                 "LfBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", lfDist), unitsMETER,
                                 "RfBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", rfDist), unitsMETER,
                                 "LrBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", lrDist), unitsMETER,
                                 "RrBrakingDistance", CreateMessage(buff, sizeof(buff), "%.2f", rrDist), unitsMETER);
    }
    else
    {   // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Brake distance test");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::BrakeDistanceTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MaxBrakeForceVerification(void)
{   // Log the entry and see if this step should be performed
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MaxBrakeForceVerification() - Enter");
    string testResult(BEP_TESTING_STATUS);
    if(!ShortCircuitTestStep())
    {   // Wait for minimum force on all wheels or timeout
        SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("FrontBalanceBGColor"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("RearBalanceBGColor"), GetParameter("TestInProgressColor"));
        MaxBrakeData brakeData[GetRollerCount()];
        // Assign the data tags
        for(INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
        {
            brakeData[roller].currentForce = 0.0;
            brakeData[roller].maxForce     = 0.0;
            brakeData[roller].measurementComplete = false;
        }
        brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
        brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
        brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
        brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
        brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
        brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
        brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
        brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
        // Command the high torque motors to engage
        SystemWrite(GetDataTag("EngageHighTorqueMotor"), true);
        // Wait for the rollers to be rolling
        WHEELINFO rollerSpeed;
        do
        {
            GetWheelSpeeds(rollerSpeed);
            BposSleep(GetParameterInt("ApplyBrakePedalWaitTime"));
        } while((rollerSpeed.lfWheel < 1.0) && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Instruct operator to apply brake pedal and wait for a bit
        DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
        bool frontMeasurementComplete = false;
        float fBal = GetVehicleParameter("MaxFrontDifference", float(0.0)) + 20.0;
        do
        {   // Make sure the max values are reset
            brakeData[LFWHEEL].maxForce = 0.0;
            brakeData[LFWHEEL].measurementComplete = false;
            brakeData[RFWHEEL].maxForce = 0.0;
            brakeData[RFWHEEL].measurementComplete = false;
            BposSleep(GetParameterInt("FrontMaxBrakeStablizationTime"));
            frontMeasurementComplete = MonitorBrakeForces(LFWHEEL, RFWHEEL,
                                                          GetVehicleParameter("RequiredBrakeForceFront", float(0.0)),
                                                          GetVehicleParameter("RequiredBrakeForceRear", float(0.0)),
                                                          brakeData);
            // Set the result colors on the screen
            SystemWrite(brakeData[LFWHEEL].displayTag, brakeData[LFWHEEL].maxForce);
            SystemWrite(brakeData[RFWHEEL].displayTag, brakeData[RFWHEEL].maxForce);
            SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), 
                        brakeData[LFWHEEL].maxForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0)) ? string("green") : string("red"));
            SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), 
                        brakeData[RFWHEEL].maxForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0)) ? string("green") : string("red"));
            // Set the brake balance for each axle
            fBal = fabs(brakeData[LFWHEEL].maxForce - brakeData[RFWHEEL].maxForce);
            //fBal = (brakeData[LFWHEEL].maxForce > brakeData[RFWHEEL].maxForce) ? (brakeData[LFWHEEL].maxForce - brakeData[RFWHEEL].maxForce) : 
            //                                                                     (brakeData[RFWHEEL].maxForce > brakeData[LFWHEEL].maxForce);
        } while((!frontMeasurementComplete || (fBal > GetVehicleParameter("MaxFrontDifference", float(0.0)))) && TimeRemaining() &&
                (BEP_STATUS_SUCCESS == StatusCheck()));
        // Command the high torque motors to disengage
        SystemWrite(GetDataTag("EngageHighTorqueMotor"), false);
        RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                      GetPromptPriority("RemoveFootFromBrake"));
        SystemWrite(GetDataTag("FrontBalanceValue"), fBal);
        SystemWrite(GetDataTag("FrontBalanceBGColor"),
                    fBal <= GetVehicleParameter("MaxFrontDifference", float(0.0)) ? string("green") : string("red"));
        // Log the test result data
        Log(LOG_DEV_DATA, "LF Max Force: %.2f    RF Max Force: %.2f     Balance: %.2f",
            brakeData[LFWHEEL].maxForce, brakeData[RFWHEEL].maxForce, fBal);
        // Command the high torque motors to engage
        SetStartTime();
        SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
        do
        {
            GetWheelSpeeds(rollerSpeed);
            BposSleep(GetParameterInt("ApplyBrakePedalWaitTime"));
        } while((rollerSpeed.lrWheel < 1.0) && (BEP_STATUS_SUCCESS == StatusCheck()));
        RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), 
                     GetPromptPriority("RemoveFootFromBrake"));
        DisplayPrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));
        SetStartTime();
        bool rearMeasurementComplete = false;
        float rBal = GetVehicleParameter("MaxRearDifference", float(0.0)) + 20.0;
        do
        {   // Make sure the max values are reset
            Log(LOG_DEV_DATA, "Setting rear max brake values to incomplete state");
            brakeData[LRWHEEL].maxForce = 0.0;
            brakeData[LRWHEEL].measurementComplete = false;
            brakeData[RRWHEEL].maxForce = 0.0;
            brakeData[RRWHEEL].measurementComplete = false;
            BposSleep(GetParameterInt("RearMaxBrakeStablizationTime"));
            rearMeasurementComplete = MonitorBrakeForces(LRWHEEL, RRWHEEL,
                                                         GetVehicleParameter("RequiredBrakeForceFront", float(0.0)),
                                                         GetVehicleParameter("RequiredBrakeForceRear", float(0.0)),
                                                         brakeData);
            // Set the result colors on the screen
            SystemWrite(brakeData[LRWHEEL].displayTag, brakeData[LRWHEEL].maxForce);
            SystemWrite(brakeData[RRWHEEL].displayTag, brakeData[RRWHEEL].maxForce);
            SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), 
                        brakeData[LRWHEEL].maxForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0)) ? string("green") : string("red"));
            SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), 
                        brakeData[RRWHEEL].maxForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0)) ? string("green") : string("red"));
            // Set the brake balance for each axle
            rBal = fabs(brakeData[LRWHEEL].maxForce - brakeData[RRWHEEL].maxForce);

            if(rBal > GetVehicleParameter("MaxRearDifference", float(0.0))) 
            {
                Log(LOG_DEV_DATA, "Rear brake difference [%.2f] is out of spec!", rBal);
            }
        } while ((!rearMeasurementComplete || (rBal > GetVehicleParameter("MaxRearDifference", float(0.0)))) && TimeRemaining() &&
                 (BEP_STATUS_SUCCESS == StatusCheck()));
        // Command the high torque motors to disengage
        SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
        SystemWrite(GetDataTag("RearBalanceValue"), rBal);
        SystemWrite(GetDataTag("RearBalanceBGColor"),
                    rBal <= GetVehicleParameter("MaxRearDifference", float(0.0)) ? string("green") : string("red"));
        // Log the test result data
        Log(LOG_DEV_DATA, "LR Max Force: %.2f    RR Max Force: %.2f     Balance: %.2f",
            brakeData[LRWHEEL].maxForce, brakeData[RRWHEEL].maxForce, rBal);

        RemovePrompt(GetPromptBox("ApplyBrake"), GetPrompt("ApplyBrake"), GetPromptPriority("ApplyBrake"));

        // Determine the results
        if(BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status during the test
            Log(LOG_ERRORS, "Bad system status during max brake force testing: %s", 
                ConvertStatusToResponse(StatusCheck()).c_str());
            testResult = testFail;
        }
        else if(!TimeRemaining())
        {   // Timeout during max brake force testing
            Log(LOG_ERRORS, "Timeout waiting for minimum required brake force");
            testResult = testTimeout;
        }
        else if(frontMeasurementComplete && rearMeasurementComplete &&
                (fBal <= GetVehicleParameter("MaxFrontDifference", float(0.0))) &&
                (rBal <= GetVehicleParameter("MaxRearDifference", float(0.0))))
        {   // All brake forces are good
            Log(LOG_DEV_DATA, "All wheels met minimum brake force requirements");
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
        }
        // Report the results
        char buff[32];
        TestResultDetails testData;
        testData.AddDetail("LFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[LFWHEEL].maxForce), unitsKGF);
        testData.AddDetail("RFMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[RFWHEEL].maxForce), unitsKGF);
        testData.AddDetail("LRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[LRWHEEL].maxForce), unitsKGF);
        testData.AddDetail("RRMaxBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[RRWHEEL].maxForce), unitsKGF);
        testData.AddDetail("FrontBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", fBal), unitsKGF);
        testData.AddDetail("RearBrakeDifference", CreateMessage(buff, sizeof(buff), "%.2f", rBal), unitsKGF);
        SendTestResultWithDetail(testResult, testData, GetTestStepInfo("Description"), "0000");
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping MazdaBrakeTC::MaxBrakeForceVerification()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MaxBrakeForceVerification() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MaxBrakeForceCalibration(void)
{
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MaxBrakeForceCalibration() - Enter");
    // Set up the max brake force structure
    MaxBrakeData brakeData[GetRollerCount()];
    // Assign the data tags
    for(INT16 roller = LFWHEEL; roller <= RRWHEEL; roller++)
    {
        brakeData[roller].currentForce = 0.0;
        brakeData[roller].maxForce     = 0.0;
        brakeData[roller].measurementComplete = false;
    }
    brakeData[LFWHEEL].currentForceTag = GetDataTag("LFCurrentForce");
    brakeData[RFWHEEL].currentForceTag = GetDataTag("RFCurrentForce");
    brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
    brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
    brakeData[LFWHEEL].displayTag = GetDataTag("LFMaxBrakeForceTag");
    brakeData[RFWHEEL].displayTag = GetDataTag("RFMaxBrakeForceTag");
    brakeData[LRWHEEL].displayTag = GetDataTag("LRMaxBrakeForceTag");
    brakeData[RRWHEEL].displayTag = GetDataTag("RRMaxBrakeForceTag");
    // Clear initial background colors
    SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("FrontBalanceBGColor"), string("white"));
    SystemWrite(GetDataTag("RearBalanceBGColor"), string("white"));
    // Get the current load cell readings
    bool calibrationComplete = false;
    bool fBalGoodComplete = false;
    bool rBalGoodComplete = false;
    bool fBalBadComplete = false;
    bool rBalBadComplete = false;
    do
    {
        ReadCurrentLoadCellValues(brakeData, LFWHEEL);
        // Handle the display
        for(UINT8 roller = LFWHEEL; roller <= RRWHEEL; roller++)
        {   // Display the current values
            SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
        }
        // Display the side to side diff
        float fBal = fabs(brakeData[LFWHEEL].currentForce - brakeData[RFWHEEL].currentForce);
        //float fBal = (brakeData[LFWHEEL].maxForce > brakeData[RFWHEEL].maxForce) ? (brakeData[LFWHEEL].maxForce - brakeData[RFWHEEL].maxForce) : 
        //                                                                           (brakeData[RFWHEEL].maxForce > brakeData[LFWHEEL].maxForce);
        float rBal = fabs(brakeData[LRWHEEL].currentForce - brakeData[RRWHEEL].currentForce);
        //float rBal = (brakeData[LRWHEEL].maxForce > brakeData[RRWHEEL].maxForce) ? (brakeData[LRWHEEL].maxForce - brakeData[RRWHEEL].maxForce) : 
        //                                                                           (brakeData[RRWHEEL].maxForce > brakeData[LRWHEEL].maxForce);
        SystemWrite(GetDataTag("FrontBalanceValue"), fBal);
        SystemWrite(GetDataTag("RearBalanceValue"), rBal);
        // Figure out which steps are complete
        if(!brakeData[LFWHEEL].measurementComplete)
        {
            brakeData[LFWHEEL].measurementComplete = brakeData[LFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0));
            Log(LOG_DEV_DATA, "LF calibration complete");
        }
        if(!brakeData[RFWHEEL].measurementComplete)
        {
            brakeData[RFWHEEL].measurementComplete = brakeData[RFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0));
            Log(LOG_DEV_DATA, "RF calibration complete");
        }
        if(!brakeData[LRWHEEL].measurementComplete)
        {
            brakeData[LRWHEEL].measurementComplete = brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0));
            Log(LOG_DEV_DATA, "LR calibration complete");
        }
        if(!brakeData[RRWHEEL].measurementComplete)
        {
            brakeData[RRWHEEL].measurementComplete = brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0));
            Log(LOG_DEV_DATA, "RR calibration complete");
        }
        if(!fBalGoodComplete)
        {
            fBalGoodComplete = (brakeData[LFWHEEL].measurementComplete && brakeData[RFWHEEL].measurementComplete &&
                                fBal <= GetVehicleParameter("MaxFrontDifference", float(0.0)));
            Log(LOG_DEV_DATA, "Front diff good complete");
        }
        if(!fBalBadComplete)
        {
            fBalBadComplete = (fBalGoodComplete && (fBal >= GetVehicleParameter("MaxFrontDifference", float(0.0))));
            Log(LOG_DEV_DATA, "Front diff bad complete");
        }
        if(!rBalGoodComplete)
        {
            rBalGoodComplete = (brakeData[LRWHEEL].measurementComplete && brakeData[RRWHEEL].measurementComplete &&
                                rBal <= GetVehicleParameter("MaxRearDifference", float(0.0)));
            Log(LOG_DEV_DATA, "Rear diff good complete");
        }
        if(!rBalBadComplete)
        {
            rBalBadComplete = (rBalGoodComplete && (rBal >= GetVehicleParameter("MaxRearDifference", float(0.0))));
            Log(LOG_DEV_DATA, "Rear diff bad complete");
        }
        calibrationComplete = fBalBadComplete && rBalBadComplete;
        // Set the background colors
        Log(LOG_DEV_DATA, "LF: %.2f    RF: %.2f     Diff: %.2f", 
            brakeData[LFWHEEL].currentForce, brakeData[RFWHEEL].currentForce, fBal);
        Log(LOG_DEV_DATA, "LR: %.2f    RR: %.2f     Diff: %.2f", 
            brakeData[LRWHEEL].currentForce, brakeData[RRWHEEL].currentForce, rBal);
        SystemWrite(GetDataTag("LFMaxBrakeForceColorTag"), 
                    brakeData[LFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("RFMaxBrakeForceColorTag"), 
                    brakeData[RFWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceFront", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("LRMaxBrakeForceColorTag"), 
                    brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("RRMaxBrakeForceColorTag"), 
                    brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredBrakeForceRear", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("FrontBalanceBGColor"),
                    fBal <= GetVehicleParameter("MaxFrontDifference", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("RearBalanceBGColor"),
                    rBal <= GetVehicleParameter("MaxRearDifference", float(0.0)) ? string("green") : string("red"));
        // Wait between updates
        BposSleep(GetTestStepInfoInt("ScanDelay"));
        // Keep looping until complete
    } while(TimeRemaining() && !calibrationComplete && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Log the exit and return Pass
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MaxBrakeForceCalibration() - Enter");
    return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::ParkBrakeForceVerification(void)
{   // Log the entry and make sure we need to perform this step
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::ParkBrakeForceVerification() - Enter");
    string testResult(BEP_TESTING_STATUS);
    if(!ShortCircuitTestStep())
    {   // Prompt operator to engage the parking brake
        DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
        SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), GetParameter("TestInProgressColor"));
        SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"), GetParameter("TestInProgressColor"));
        // Engage the high torque motors
        SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
        StatusSleep(GetParameterInt("ApplyParkBrakeWaitTime"));
        DisplayPrompt(GetPromptBox("ApplyParkBrake"), GetPrompt("ApplyParkBrake"), GetPromptPriority("ApplyParkBrake"));
        // Wait for minimum force on both rear wheels or timeout
        MaxBrakeData brakeData[GetRollerCount()];
        for(INT16 roller = LRWHEEL; roller <= RRWHEEL; roller++)
        {
            brakeData[roller].currentForce = 0.0;
            brakeData[roller].maxForce     = 0.0;
            brakeData[roller].measurementComplete = false;
        }
        brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
        brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
        brakeData[LRWHEEL].displayTag = GetDataTag("LRParkBrakeForceTag");
        brakeData[RRWHEEL].displayTag = GetDataTag("RRParkBrakeForceTag");
        // Monitor max brake force on rear wheels
        bool measurementComplete = MonitorBrakeForces(LRWHEEL, RRWHEEL, 
                                                      GetVehicleParameter("RequiredParkBrakeForce", float(0.0)), 
                                                      GetVehicleParameter("RequiredParkBrakeForce", float(0.0)), brakeData);
        // Command the high torque motors to disengage and remove prompts
        SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
        RemovePrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
        RemovePrompt(GetPromptBox("ApplyParkBrake"), GetPrompt("ApplyParkBrake"), GetPromptPriority("ApplyParkBrake"));
        // Display the results on the screen
        SystemWrite(brakeData[LRWHEEL].displayTag, brakeData[LRWHEEL].maxForce);
        SystemWrite(brakeData[RRWHEEL].displayTag, brakeData[RRWHEEL].maxForce);
        SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), 
                    brakeData[LRWHEEL].maxForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), 
                    brakeData[RRWHEEL].maxForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        float totalForce = brakeData[LRWHEEL].maxForce + brakeData[RRWHEEL].maxForce;
        SystemWrite(GetDataTag("ParkBrakeForceTag"), totalForce);
        SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"), 
                    totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        // Determine the test results
        if(BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status during the test
            Log(LOG_ERRORS, "Bad system status during park brake force testing: %s", 
                ConvertStatusToResponse(StatusCheck()).c_str());
            testResult = testFail;
        }
        else if(!TimeRemaining())
        {   // Timeout during max brake force testing
            Log(LOG_ERRORS, "Timeout waiting for minimum required park brake force");
            testResult = testTimeout;
        }
        else if(measurementComplete)
        {   // All brake forces are good
            Log(LOG_DEV_DATA, "Rear wheels met minimum park brake force requirements");
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
        }
        // Report the results
        char buff[32];
        SendTestResultWithDetail(testResult, GetTestStepInfo("Description"), "0000",
                                 "LRParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[LRWHEEL].maxForce), unitsKGF,
                                 "RRParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", brakeData[RRWHEEL].maxForce), unitsKGF,
                                 "TotalParkBrakeForce", CreateMessage(buff, sizeof(buff), "%.2f", totalForce), unitsKGF);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping MazdaBrakeTC::ParkBrakeForceVerification()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::ParkBrakeForceVerification() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::ParkBrakeForceCalibration(void)
{   
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::ParkBrakeForceCalibration() - Enter");
    // Set up the parameters
    MaxBrakeData brakeData[GetRollerCount()];
    for(INT16 roller = LRWHEEL; roller <= RRWHEEL; roller++)
    {
        brakeData[roller].currentForce = 0.0;
        brakeData[roller].maxForce     = 0.0;
        brakeData[roller].measurementComplete = false;
    }
    brakeData[LRWHEEL].currentForceTag = GetDataTag("LRCurrentForce");
    brakeData[RRWHEEL].currentForceTag = GetDataTag("RRCurrentForce");
    brakeData[LRWHEEL].displayTag = GetDataTag("LRParkBrakeForceTag");
    brakeData[RRWHEEL].displayTag = GetDataTag("RRParkBrakeForceTag");
    // Wait for the rear load cells to read 0
    SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), string("white"));
    SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"), string("white"));
    do
    {
        ReadCurrentLoadCellValues(brakeData, LRWHEEL);
        BposSleep(500);
    } while(((brakeData[LRWHEEL].currentForce > 1.0) || (brakeData[RRWHEEL].currentForce > 1.0)) &&
            (BEP_STATUS_SUCCESS == StatusCheck()));
    SetStartTime();
    // Wait for the calibration to be complete
    bool calibrationComplete = false;
    do
    {   // Get the current load cell readings
        ReadCurrentLoadCellValues(brakeData, LRWHEEL);
        // Display the values
        SystemWrite(brakeData[LRWHEEL].displayTag, brakeData[LRWHEEL].currentForce);
        SystemWrite(brakeData[RRWHEEL].displayTag, brakeData[RRWHEEL].currentForce);
        float totalForce = brakeData[LRWHEEL].currentForce + brakeData[RRWHEEL].currentForce;
        SystemWrite(GetDataTag("ParkBrakeForceTag"), totalForce);
        // Determine colors
        SystemWrite(GetDataTag("LRParkBrakeForceColorTag"), 
                    brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("RRParkBrakeForceColorTag"), 
                    brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        SystemWrite(GetDataTag("ParkBrakeForceBgColorTag"),
                    totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", float(0.0)) ? string("green") : string("red"));
        // Check if we are done
        calibrationComplete = ((brakeData[LRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0))) &&
                               (brakeData[RRWHEEL].currentForce >= GetVehicleParameter("RequiredParkBrakeForce", float(0.0))) &&
                               (totalForce >= GetVehicleParameter("MinimumTotalParkBrakeForce", float(0.0))));
        // Wait a bit before the next run
        BposSleep(GetTestStepInfoInt("ScanDelay"));
    } while(TimeRemaining() && !calibrationComplete && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Log the exit and return pass
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::ParkBrakeForceCalibration() - Exit");
    return testPass;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::PreShiftCheck(void)
{   // Log the entry and enable the high torque motors
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::PreShiftCheck() - Enter");
    SystemWrite(GetDataTag("EngageHighTorqueMotor"), true);
    SystemWrite(GetDataTag("EngageParkBrakeMotors"), true);
    // Make sure both front and rear rollers spin
    bool checkoutComplete = false;
    WHEELINFO rollerSpeed;
    do
    {   // Get the current roller speed
        GetWheelSpeeds(rollerSpeed);
        checkoutComplete = (rollerSpeed.lfWheel > 1.0) && (rollerSpeed.lrWheel > 1.0);
        BposSleep(250);
    } while(!checkoutComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()));
    // Let the rollers run until we timeout
    while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(500);
    // Turn off the high torque motors
    SystemWrite(GetDataTag("EngageHighTorqueMotor"), false);
    SystemWrite(GetDataTag("EngageParkBrakeMotors"), false);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::PreShiftCheck() - Enter");
    return checkoutComplete ? testPass : testFail;
}

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::TractionControlCheck(void)
{   // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::TractionControlCheck() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    bool tractionEquipped = !SystemRead(GetDataTag("TractionControlFromPlc")).compare("1");
    if(!ShortCircuitTestStep() && tractionEquipped && m_continueBrakeTest)
    {   // Make sure the vehicle is at zero speed
        if(CheckZeroSpeed())
        {   // Reset the start time
            SetStartTime();
            // Prompt the operator to enable the traction control and accelerate to the target speed
            DisplayPrompt(GetPromptBox("EnableTractionControl"), GetPrompt("EnableTractionControl"), 
                          GetPromptPriority("EnableTractionControl"));
            float maxSpeed = GetVehicleParameter("TractionControlSpeed", float(0.0)) * 0.6213712;
            char buff[32];
            string speedRange(CreateMessage(buff, sizeof(buff), "%.2f %.2f", maxSpeed, maxSpeed + 5.0));
            testResult = AccelerateToTestSpeed(maxSpeed, speedRange, GetTestStepInfoInt("ScanDelay"), false);
            // Determine the result - Timeout means the traction control prevented us from reaching the target speed
            testResult = !testResult.compare(BEP_TIMEOUT_RESPONSE) ? testPass : testFail;
            SystemWrite(GetDataTag("TractionControlBgColor"), !testResult.compare(testPass) ? "green" : "red");
            Log(LOG_ERRORS, "Traction control result: %s", testResult.c_str());
            RemovePrompt(GetPromptBox("EnableTractionControl"), GetPrompt("EnableTractionControl"), 
                         GetPromptPriority("EnableTractionControl"));
        }
        else
        {
            Log(LOG_ERRORS, "Timeout waiting for zero speed");
            testResult = testFail;
        }
        // Report the test result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else if(!tractionEquipped)
    {
        Log(LOG_DEV_DATA, "Vehicle is not equipped with traction control");
        testResult = testSkip;
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping test step TractionControlCheck() continue: %d", m_continueBrakeTest);
        testResult = testSkip;
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::TractionControlCheck() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
bool MazdaBrakeTC::MonitorBrakeForces(INT16 startingRoller, INT16 stoppingRoller,
                                       float minimumRequiredForceFront, float minimumRequiredForceRear,
                                       MaxBrakeData *brakeData)
{
    bool measurementComplete = false;

    do
    {   // Get the current force for each wheel
        for(int roller = startingRoller; roller <= stoppingRoller; roller++)
        {   // Get the current force
            float requiredForce = (roller < LRWHEEL) ? minimumRequiredForceFront : minimumRequiredForceRear;

            // Display the current force
            SystemWrite(brakeData[roller].displayTag, brakeData[roller].currentForce);
            ReadCurrentLoadCellValues(brakeData, startingRoller);
            // Check if this is a max value
            if(brakeData[roller].currentForce > brakeData[roller].maxForce)  
            {
                brakeData[roller].maxForce = brakeData[roller].currentForce;
                Log(LOG_DEV_DATA, "New max force for %s: %.2f", rollerName[roller].c_str(), brakeData[roller].maxForce);
                // Check if measurement is complete on this wheel
                if(brakeData[roller].maxForce > requiredForce)
                {
                    brakeData[roller].measurementComplete = true;
                    Log(LOG_DEV_DATA, "Measurement complete for %s - reached min required force: %.2f",
                        rollerName[roller].c_str(), requiredForce);
                }
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

/*

//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::FrontLeftABSTest(void)
{   // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::FrontLeftABSTest() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    bool absEquipped = EquippedABS();
    bool keepTesting = true;
    MaxBrakeData beginBrakeForce[GetRollerCount()];
    MaxBrakeData dumpBrakeForce[GetRollerCount()];
    MaxBrakeData recoverBrakeForce[GetRollerCount()];
    if(!ShortCircuitTestStep() && absEquipped && m_continueBrakeTest)
    {   
        SystemWrite("M077",true);
        do
        {
            SystemWrite("StartForceSampling",true);
            BposSleep(GetParameterInt("ForceSampleTime"));
            ReadCurrentLoadCellValues(&beginBrakeForce, LFWHEEL);
            SystemWrite("StartForceSampling",false);
            SystemWrite("M067",true);

            SystemReadWaitForResult("M017","1",10000);
            BposSleep(GetParameterInt("DelayBeforeSampling"));
            SystemWrite("StartForceSampling",true);
            BposSleep(GetParameterInt("ForceSampleTime"));
            ReadCurrentLoadCellValues(&dumpBrakeForce, LFWHEEL);
            SystemWrite("StartForceSampling",false);

            // Inspect dump forces

            if()
            { // Forces good
            }
            else
            {
                string response = OperatorYesNo();
                if(response != "Yes")
                {
                    keepTesting = false;
                    testResult = testAbort;
                }
            }
        }while(keepTesting);
        SystemWrite("M077",false);
        // Report the test result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
        if(!testResult.compare(testAbort))
        {
            SystemWrite(ABORT_DATA_TAG,"1");
        }
    }
    else
    {
        Log(LOG_FN_ENTRY, "Skipping test step FrontLeftABSTest()   continue: %d", m_continueBrakeTest);
        testResult = testSkip;
    }
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::FrontLeftABSTest() - Exit");
    return testResult;
}*/
/*
//-------------------------------------------------------------------------------------------------
string MazdaBrakeTC::MazdaAnalyzeABSDumpForces(MaxBrakeData *dumpForce, MaxBrakeData *startForce, UINT8 actuatedWheel)
{   // Log the entry and determine if this should be checked
    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAnalyzeBrakeForce(%s) - Enter", axle.c_str());
    string testResult(BEP_FAILURE_RESPONSE);

    if(actuatedWheel >= LRWHEEL) // Analyze forces for rear wheels only
    {
    }
    else                         // Analyze forces for front wheels only
    {
    }

    Log(LOG_FN_ENTRY, "MazdaBrakeTC::MazdaAnalyzeBrakeForce() - Exit result: %s", testResult.c_str());

    return(testResult);
} */


