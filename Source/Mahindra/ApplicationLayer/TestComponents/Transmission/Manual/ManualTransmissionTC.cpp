//******************************************************************************
// Description:
//  Manual transmission component class
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "ManualTransmissionTC.h"

//-------------------------------------------------------------------------------------------------
ManualTransmissionTC::ManualTransmissionTC() : GenericTC(),
m_accelTestStartTime( 0)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
ManualTransmissionTC::~ManualTransmissionTC()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void ManualTransmissionTC::Initialize(const string &fileName)
{
    // call the base class to perfrom the initialization
    GenericTC::Initialize(fileName);
}

//-------------------------------------------------------------------------------------------------
void ManualTransmissionTC::Initialize(const XmlNode *config) 
{
    try
    {
        //Iniitalize base class
        GenericTC::Initialize(config);

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

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::CommandTestStep(const string &value)
{
    string testResult = testFail;
    const string &step = GetTestStepName();

    // Determine if the next test step can be sequenced
    if (StatusCheck() == BEP_STATUS_SUCCESS)
    {   // System is fine to continue testing
        try
        {   // Get the next test step to perform
            Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
            // Check the PRNDL Status
            if (step == "AccelerateInReverse")
            {
                testResult = AccelerateInReverse(GetParameterFloat("ReverseSpeedMinimum"), 
                                                 GetParameterFloat("ReverseSpeedMaximum"), 
                                                 GetTestStepInfoInt("ScanDelay"));
            }
            // Check Transmission shifting
			else if(step == "BlingGearSpeedTest")        testResult = BlindGearSpeedCheck(value);
            else if ( step == "GearTest_1")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_2")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_3")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_4")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_5")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_6")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_7")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_8")              testResult = TestStepShiftTest( value);
            else if ( step == "GearTest_9")              testResult = TestStepShiftTest( value);
            else if ( step == "SpeedBasedGearTest_1")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_2")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_3")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_4")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_5")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_6")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_7")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_8")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedGearTest_9")              testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "SpeedBasedShiftTest")     testResult = TestStepSpeedBasedShiftTest( value);
            else if ( step == "AccelerationTest")        testResult = TestStepAccelerationTest( value);
            else if ( step == "BrakeToStop")             testResult = TestStepBrakeToStop(value);
            else if ( step == "FinishUp")                testResult = TestStepFinishUp( value);
            else if ( step == "CheckShifting")
            {
                testResult = OperatorPassFail(GetDataTag("CheckShiftingPrompt"), GetTestStepInfoInt("Timeout"));
                SendSubtestResult("ReverseGearObserved", testResult, "Reverse Gear Observed", "0000");
            }
            else if (step == "VerifyIdleRPM")            testResult = TestStepVerifyIdleRPM(value);
            // No defined test step, try the base class
            else      testResult = GenericTC::CommandTestStep(value);
        }
        catch (BepException &err)
        {   // Log the exception and return the error
            Log("CommandTestStep %s BepException: %s\n", step.c_str(), err.what());
            testResult = BEP_ERROR_RESPONSE;
        }
    }
    else
    {   // Status of the system is not right to continue testing
        Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
    }
    // Return the result of the test
    Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::BlindGearSpeedCheck(const string &gear)
{
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "ManualTransmissionTC::BlindGearSpeedCheck(Gear: %s) - Enter", gear.c_str());
	string targetSpeedParamName(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + 
								string("SpeedBasedGearTest_") + gear + string("/") + "MaxSpeedTarget");
	string toleranceParamName(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + 
								string("SpeedBasedGearTest_") + gear + string("/") + "MaxSpeedTolerance");
	float targetSpeed = GetVehicleParameter(targetSpeedParamName, (float)(0.0));
	float speedTolerance = GetVehicleParameter(toleranceParamName, (float)(0.0));
	char buff[32];
	string speedRange(CreateMessage(buff, sizeof(buff), "%.2f %.2f", 
									(targetSpeed - speedTolerance), (targetSpeed + speedTolerance)));
	if(!ShortCircuitTestStep())
	{   // Disable the speedometer
		SystemWrite(GetDataTag("SpeedActive"), false);
		// Prompt the operator to shift to gear and accelerate to maximum gear speed
		if(GetParameterBool("DisplayTargetForBlindCheck"))
		{
			SystemWrite("SpeedTarget", speedRange);
		}
		DisplayPrompt(GetPromptBox("ShiftToGearAndAccelerate"), GetPrompt("ShiftToGearAndAccelerate"),
					  GetPromptPriority("ShiftToGearAndAccelerate"), "white", gear);
		// Wait for the operator to complete
		INT32 timeout = GetTestStepInfoInt("Timeout");
		result = OperatorPassFail("PressPassWhenComplete", timeout);
		float testSpeed = GetRollSpeed();
		// Re-enable the speedometer
		SystemWrite(GetDataTag("SpeedActive"), true);
		if(GetParameterBool("DisplayTargetForBlindCheck"))
		{
			SystemWrite( "SpeedTarget", string("0 0"));
		}
		// Report the result
		SendSubtestResultWithDetail(GetTestStepName() + "_" + gear, testPass, GetTestStepInfo("Description"), "0000",
									"MaxSpeed", CreateMessage(buff, sizeof(buff), "%.4f", testSpeed), unitsMPH,
									"RealResult", result, "");
		// NOTE: Setting the result here to PASS is intentional and requested by the customer.
		//       It is reported as is in the result under the RealResult item so we can track it if needed.
		//       - Dan Erickson 2012.05.21
		result = testPass;
	}
	else
	{   // Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping Blind Gear speed check - Gear: %s", gear.c_str());
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "ManualTransmissionTC::BlindGearSpeedCheck(Gear: %s) - Exit", gear.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::Publish(const XmlNode *node)
{
    const string   &tag = node->getName();
    const string   &value = node->getValue();
    string         result;

    if ( (ZEROSPEED_TAG == tag) && ("0" == value))
    {
        m_accelTestStartTime = time( NULL);
        Log( LOG_DEV_DATA, "Accel Test start time set to %d\n", m_accelTestStartTime);
    }
    result = BepServer::Publish( node);

    return( result);
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepBrakeToStop(const string &value)
{
    bool done = false;  // status of the braking
    string status = BEP_NONE;  // set test step status to no response

    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::TestStepBrakeToStop()\n");

    try
    {   // if the conditions are correct to perform the task
        if (StatusCheck() == BEP_STATUS_SUCCESS)
        {
            // check if at zerospeed
            if (ReadSubscribeData(GetDataTag("Zerospeed")) != "1")
            {
                // if not, put up the question and wait
                if (UpdatePrompts() != BEP_STATUS_SUCCESS)
                    Log(LOG_ERRORS, "Unable to Update Prompts\n");
                // Set the start time
                SetStartTime();
                while ((StatusCheck() == BEP_STATUS_SUCCESS) && TimeRemaining() && !done)
                {
                    if (ReadSubscribeData(GetDataTag("Zerospeed")) != "1")   BposSleep(250);
                    else                                                    done = true;
                }
            }
            // else flag done
            else
            {
                done = true;
            }
			Log(LOG_DEV_DATA, "Vehicle at zero speed: %s", done ? "True" : "False");
            // update the status of the test
            if (StatusCheck() != BEP_STATUS_SUCCESS) UpdateResult(StatusCheck(), status);
            else if (!TimeRemaining())               status = BEP_TIMEOUT_STATUS;
            else if (!done)                          status = BEP_FATALFAIL_STATUS;
            else                                    status = BEP_PASS_STATUS;

            //if successful, ensure that the motor mode is Boost mode
            if (status == BEP_PASS_STATUS)
            {
				Log(LOG_DEV_DATA, "Setting motor mode to %s", string(BOOST_MODE).c_str());
                SystemWrite(MOTOR_MODE, string(BOOST_MODE));
            }
        }
        // else the conditions are not correct, indicate not started
        else
        {
            status = BEP_TEST_NOT_STARTED;
        }
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "ManualTransmissionTC::TestStepBrakeToStop Exception: %s\n", e.what());
        status = BEP_SOFTWAREFAIL_STATUS;
    }

    // update the test result
    if (SendTestResult(status, GetTestStepInfo("Description")) != BEP_STATUS_SUCCESS)
    {
        Log(LOG_ERRORS, "ManualTransmissionTC::TestStepBrakeToStop Could Not Send Test Result: %s, %s\n",
            GetTestStepName().c_str(), status.c_str());
    }

    RemovePrompts();    // remove the prompts from the screen

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::TestStepBrakeToStop(), status=%s\n",
        status.c_str());

    return(status);     // return the status
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepShiftTest(const string &value)
{
    string testStatus( BEP_NONE);  // set test step status to no response
    string targetGear( GetTestStepInfo( "TargetGear"));
    int         scanDelay = GetTestStepInfoInt( "ScanDelay");
    float       targetSpeed = GetParameterFloat( "AccelerationTestMaxSpeed");
    int         numberOfGears = GetParameterInt( "NumberOfFwdGears");
    int         gearFactor = atoi( targetGear.c_str());
    int         status = StatusCheck();
    char        buff[ 256];
    float       topSpeed = 0;

    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::TestStepShiftTest(%s)\n", targetGear.c_str());
                             
    //Get Parameters from build record
    SetShiftTestParameters(targetGear, scanDelay, targetSpeed, numberOfGears, gearFactor);

    if ( scanDelay <= 0) scanDelay = 250;
    if ( numberOfGears <= 0) numberOfGears = 1;

    UpdatePrompts();

    // Set speed target to be a proportional to the current gear
    targetSpeed = (targetSpeed * gearFactor) / numberOfGears;
    sprintf( buff, "%.02f %.02f", targetSpeed, targetSpeed+5.0);
    string speedRange = buff;
    SystemWrite( "SpeedTarget", speedRange);

    while ( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status))
    {
        status = StatusSleep( scanDelay);
        topSpeed = GetRollSpeed();
        Log( LOG_DEV_DATA, "Gear: %s, Speed=%.02f\n", targetGear.c_str(), topSpeed);
    }

    RemovePrompts();

    if ( BEP_STATUS_SUCCESS == status)   testStatus = testPass;
    else                                testStatus = testFail;

    SendTestResultWithDetail( testStatus, GetTestStepInfo( "Description"), "0000",
                              "Gear", targetGear, "",
                              "TopSpeed", CreateMessage( buff, sizeof( buff), "%.02f", topSpeed), unitsMPH);

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::TestStepShiftTest(%s), status=%s\n", 
        targetGear.c_str(), testStatus.c_str());

    return( testStatus);
}

//-------------------------------------------------------------------------------------------------
void ManualTransmissionTC::SetShiftTestParameters(string &targetGear, int &scanDelay, float &targetSpeed, 
                                                          int &numberOfGears, int &gearFactor)
{
    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::SetTestStepShiftTestParameters\n");

    Log( LOG_DEV_DATA, "Default Values: \n\t targetGear: %s \n\t scanDelay: %d \n\t targetSpeed: %.02f" 
         "\n\t numberOfGears: %d \n\t gearFactor: %d", targetGear.c_str(), scanDelay, 
         targetSpeed, numberOfGears, gearFactor);

    targetGear = GetVehicleParameter(GetComponentName() + string("/") + "ShiftTestParameters" + string("/") + GetTestStepName() + 
                                     string("/") + "TargetGear", targetGear);
    scanDelay = GetVehicleParameter(GetComponentName() + string("/") + "ShiftTestParameters" + string("/") + GetTestStepName() + 
                                    string("/") + "ScanDelay", scanDelay);
    targetSpeed = GetVehicleParameter(GetComponentName() + string("/") + "ShiftTestParameters" + string("/") + GetTestStepName() + 
                                      string("/") + "AccelerationTestMaxSpeed", targetSpeed);
    numberOfGears = GetVehicleParameter(GetComponentName() + string("/") + "NumberOfFwdGears", numberOfGears);
    gearFactor = atoi( targetGear.c_str());

    Log( LOG_DEV_DATA, "Values Set: \n\t targetGear: %s \n\t scanDelay: %d \n\t targetSpeed: %.02f" 
         "\n\t numberOfGears: %d \n\t gearFactor: %d", targetGear.c_str(), scanDelay, 
         targetSpeed, numberOfGears, gearFactor);

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::SetTestStepShiftTestParameters\n");
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepSpeedBasedShiftTest(const string &value)
{
    string testStatus( BEP_NONE);  // set test step status to no response
    int         targetGear = atoi(GetTestStepInfo( "TargetGear").c_str());
    float       targetSpeed = 0.0;
    float       targetSpeedTolerance = 0.0;
    float       targetSpeedMin = 0.0;
    float       targetSpeedMax = 0.0;
    int         scanDelay = GetTestStepInfoInt( "ScanDelay");
    int         numberOfGears = GetParameterInt("NumberOfFwdGears");
    int         status = StatusCheck();
    char        buff[ 256];
    float       topSpeed = 0.0;
    string      parameter;

    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::TestStepSpeedBasedShiftTest(), NumberOfFwdGears=%d\n", numberOfGears);

    // Set speed target to be a proportional to the current gear
    if ( scanDelay <= 0) scanDelay = 250;
    if ( numberOfGears <= 0) numberOfGears = 1;
    targetSpeed = GetParameterFloat(CreateMessage( buff, sizeof( buff), "MaxSpeedTarget_Gear%d", targetGear));
    targetSpeedTolerance = GetParameterFloat(CreateMessage( buff, sizeof( buff), "MaxSpeedTolerance_Gear%d", targetGear));
    targetSpeedMin = targetSpeed - targetSpeedTolerance;
    targetSpeedMax = targetSpeed + targetSpeedTolerance;
    int maxSpeedDelay = GetParameterInt(CreateMessage( buff, sizeof( buff), "MaxSpeedDelay_Gear%d", targetGear));
            
    //Get Parameters from build record
    SetSpeedBasedShiftTestParameters(targetGear, targetSpeed, targetSpeedTolerance, targetSpeedMin, 
                                     targetSpeedMax, scanDelay, numberOfGears, maxSpeedDelay);

    if (numberOfGears >= targetGear)
    {
        UpdatePrompts();
        sprintf( buff, "%.02f %.02f", targetSpeed - targetSpeedTolerance, targetSpeed + targetSpeedTolerance);
        string speedRange = buff;
        SystemWrite( "SpeedTarget", speedRange);
        Log(LOG_DEV_DATA, "SpeedTarget = %s, speedRange = %s\n", buff, speedRange.c_str());

        topSpeed = GetRollSpeed();
        while ( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) && topSpeed < targetSpeedMin)
        {
            status = StatusSleep( scanDelay);
            topSpeed = GetRollSpeed();
            Log( LOG_DEV_DATA, "Gear: %d, Speed=%.02f\n", targetGear, topSpeed);
        }

        if (!TimeRemaining())
        {
            testStatus = testTimeout;
        }
        else if (BEP_STATUS_SUCCESS != status)
        {
            testStatus = testFail;
        }
        else
        {
            // Delay for a moment to allow vehicle to accelerate further, then take max speed sample
            BposSleep(maxSpeedDelay);
            topSpeed = GetRollSpeed();
            if (topSpeed > targetSpeedMin && topSpeed < targetSpeedMax)
            {
                testStatus = testPass;
            }
            else
            {
                testStatus = testFail;
            }
        }

        RemovePrompts();
		SystemWrite( "SpeedTarget", string("0 0"));

        string testResultName(GetTestStepName());
        if(!SystemReadBool("FrontAxleTestSelected") && SystemReadBool("LiftAxleTestSelected"))
        {
            testResultName = GetDataTag("SecondaryRearAxleTag") + testResultName;
        }
        SendSubtestResultWithDetail(testResultName, testStatus, GetTestStepInfo( "Description"), "0000",
                                    "Gear", CreateMessage( buff, sizeof( buff), "%d", targetGear), "",
                                    "TopSpeed", CreateMessage( buff, sizeof( buff), "%.02f", topSpeed), unitsMPH,
                                    "Min", CreateMessage( buff, sizeof( buff), "%.02f", targetSpeedMin), unitsMPH,
                                    "Max", CreateMessage( buff, sizeof( buff), "%.02f", targetSpeedMax), unitsMPH);
    }
    else
    {
        testStatus = testSkip;
    }

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::TestStepSpeedBasedShiftTest(%d), status=%s\n", 
        targetGear, testStatus.c_str());

    return( testStatus);
}

//-------------------------------------------------------------------------------------------------
void ManualTransmissionTC::SetSpeedBasedShiftTestParameters(int &targetGear, float &targetSpeed, 
                                                            float &targetSpeedTolerance, float &targetSpeedMin, 
                                                            float &targetSpeedMax, int &scanDelay, 
                                                            int &numberOfGears, int &maxSpeedDelay)
{
    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::SetSpeedBasedShiftTestParameters\n");

    Log( LOG_DEV_DATA, "Default Values: \n\t targetGear: %d \n\t targetSpeed: %.02f \n\t" 
         "targetSpeedTolerance: %.02f \n\t targetSpeedMin: %.02f \n\t targetSpeedMax: %.02f \n\t" 
         "scanDelay: %d \n\t numberOfGears: %d ", targetGear, targetSpeed, 
         targetSpeedTolerance, targetSpeedMin, targetSpeedMax, scanDelay, numberOfGears);

    targetGear = GetVehicleParameter(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + GetTestStepName() + 
                                     string("/") + "TargetGear", targetGear);
    targetSpeed = GetVehicleParameter(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + GetTestStepName() + 
                                      string("/") + "MaxSpeedTarget", targetSpeed); 
    targetSpeedTolerance = GetVehicleParameter(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + GetTestStepName() + 
                                               string("/") + "MaxSpeedTolerance", targetSpeedTolerance);
    scanDelay = GetVehicleParameter(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + GetTestStepName() + 
                                    string("/") + "ScanDelay", scanDelay);
    numberOfGears = GetVehicleParameter(GetComponentName() + string("/") + "NumberOfFwdGears", numberOfGears);
    
    maxSpeedDelay = GetVehicleParameter(GetComponentName() + string("/") + "SpeedBasedShiftTestParameters" + string("/") + GetTestStepName() + 
                                     string("/") + "MaxSpeedDelay", maxSpeedDelay);

    targetSpeedMin = targetSpeed - targetSpeedTolerance;
    targetSpeedMax = targetSpeed + targetSpeedTolerance;
    
    if ( scanDelay <= 0) scanDelay = 250;
    if ( numberOfGears <= 0) numberOfGears = 1;

    Log( LOG_DEV_DATA, "Values Set: \n\t targetGear: %d \n\t targetSpeed: %.02f \n\t" 
         "targetSpeedTolerance: %.02f \n\t targetSpeedMin: %.02f \n\t targetSpeedMax: %.02f \n\t" 
         "scanDelay: %d \n\t numberOfGears: %d ", targetGear, targetSpeed, 
         targetSpeedTolerance, targetSpeedMin, targetSpeedMax, scanDelay, numberOfGears);

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::SetSpeedBasedShiftTestParameters\n");
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepAccelerationTest(const string &value)
{
    string testStatus( BEP_NONE);  // set test step status to no response
    string color( "Green");
    string faultTag( "NoFault");
    int         scanDelay = GetTestStepInfoInt( "ScanDelay");
    float       startSpeed = GetParameterFloat( "AccelerationTestStartSpeed" + value);
    float       endSpeed = GetParameterFloat( "AccelerationTestEndSpeed" + value);
    float       maxTime = GetParameterFloat("AccelerationTestMaxTime" + value);
    int         status = StatusCheck();
    time_t      endTime;
    char        buff[ 256];
    float       topSpeed = 0, elapsedTime;

    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::TestStepAccelerationTest(%.02f)\n", endSpeed);
    //Get Parameters from build record
    SetAccelerationTestParameters(scanDelay, startSpeed, endSpeed, maxTime);
    
    if ( scanDelay <= 0) scanDelay = 250;

    UpdatePrompts();

    sprintf( buff, "%.02f %.02f", endSpeed, endSpeed+5.0);
    string speedRange = buff;
    SystemWrite( "SpeedTarget", speedRange);

    while ( (TimeRemaining()) && (BEP_STATUS_SUCCESS == status) && (topSpeed < endSpeed))
    {
        status = StatusSleep( scanDelay);
        topSpeed = GetRollSpeed();
        endTime = time( NULL);
        elapsedTime = difftime( endTime, m_accelTestStartTime);
        // Display results on the GUI
        SystemWrite( "AccelTestMaxSpeed", topSpeed);
        SystemWrite( "AccelTestMaxTime", elapsedTime);
        Log( LOG_DEV_DATA, "Speed=%.02f, target/endspeed=%.02f, time=%d\n", topSpeed, endSpeed, endTime);
    }
    endTime = time( NULL);
    elapsedTime = difftime( endTime, m_accelTestStartTime);
    Log( LOG_DEV_DATA, "End=%d, Start=%d, Elapsed=%.02f\n", m_accelTestStartTime, endTime, elapsedTime);

    RemovePrompts();

    if ( BEP_STATUS_SUCCESS == status)
    {
        if ( topSpeed >= endSpeed && elapsedTime < maxTime)
        {
            color = "Green";
            faultTag = string( "NoFault");
            testStatus = testPass;
        }
        else
        {
            color = "Red";
            faultTag = string( "AccelerationTestMaxSpeedFailure");
            testStatus = testFail;
        }
    }
    else
    {
        color = "Red";
        faultTag = string( "StatusCheckFailure");
        testStatus = testFail;
    }

    // Display results on the GUI
    SystemWrite( "AccelTestMaxSpeed"+value, topSpeed);
    SystemWrite( "AccelTestMaxSpeedBGColor"+value, color);
    SystemWrite( "AccelTestMaxTime"+value, elapsedTime);
    SystemWrite( "AccelTestMaxTimeBGColor"+value, color);

    string testResultName(GetTestStepName()+value);
    if(!SystemReadBool("FrontAxleTestSelected") && SystemReadBool("LiftAxleTestSelected"))
    {
        testResultName = GetDataTag("SecondaryRearAxleTag") + testResultName;
    }
    SendSubtestResultWithDetail(testResultName, testStatus, GetTestStepInfo( "Description"), GetFaultCode( faultTag),
                              "TargetSpeed", CreateMessage( buff, sizeof( buff), "%.02f", endSpeed), unitsMPH, 
                              "TopSpeed", CreateMessage( buff, sizeof( buff), "%.02f", topSpeed), unitsMPH,
                              "MaxTime", CreateMessage( buff, sizeof( buff), "%.03f", maxTime), "seconds",
                              "ElapsedTime", CreateMessage( buff, sizeof( buff), "%.03f", elapsedTime), "seconds");

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::TestStepAccelerationTest(%.02f), status=%s\n", 
        endSpeed, testStatus.c_str());

    return( testStatus);
}

//-------------------------------------------------------------------------------------------------
void ManualTransmissionTC::SetAccelerationTestParameters(int &scanDelay, float &startSpeed, 
                                                                      float &endSpeed, float &maxTime)
{
    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::GetAccelerationTestParameters\n");

    Log( LOG_DEV_DATA, "Default Values: \n\t scanDelay: %d \n\t startSpeed: %.02f" 
         "\n\t endSpeed: %.02f \n\t maxTime: %.02f", scanDelay, startSpeed, endSpeed, maxTime);
    
    scanDelay = GetVehicleParameter(GetComponentName() + string("/") + GetTestStepName() + 
                                    string("/") + "ScanDelay", scanDelay);
    startSpeed = GetVehicleParameter(GetComponentName() + string("/") + GetTestStepName() + 
                                     string("/") + "StartSpeed", startSpeed);
    endSpeed = GetVehicleParameter(GetComponentName() + string("/") + GetTestStepName() + 
                                   string("/") + "EndSpeed", endSpeed);
    maxTime = GetVehicleParameter(GetComponentName() + string("/") + GetTestStepName() + 
                                  string("/") + "MaxTime", maxTime);
    
    Log( LOG_DEV_DATA, "Values Set: \n\t scanDelay: %d \n\t startSpeed: %.02f" 
         "\n\t endSpeed: %.02f \n\t maxTime: %.02f", scanDelay, startSpeed, endSpeed, maxTime);

    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::GetAccelerationTestParameters\n");
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepFinishUp(const string &value)
{
    // send the overall test result
    SendOverallResult();
    // always return success
    return(GetOverallResult());
}

//-------------------------------------------------------------------------------------------------
const string ManualTransmissionTC::TestStepVerifyIdleRPM(const string &value)
{
    string testResult = testTimeout;
    string keyPress;
    int scanDelay = GetTestStepInfoInt("ScanDelay");
    Log(LOG_FN_ENTRY, "Enter ManualTransmissionTC::TestStepVerifyIdleRPM\n");
    if (!ShortCircuitTestStep())
    {
        //display the prompt
        UpdatePrompts();

        string buttonValue = "";
        bool buttonPressed = false;
        // Poll the START TEST button
        while (TimeRemaining() && (false == buttonPressed))
        {
            buttonValue = SystemRead(GetDataTag("StartTestFromPLC"));
            if ("1"==buttonValue)
            {
                buttonPressed = true;
                testResult = testPass;
            }
            else
            {
                BposSleep(scanDelay);
            }
        }
        RemovePrompts();
    }
    else
    {
        testResult = testSkip;
    }
    SendSubtestResult("VerifyIdleRPM", testResult, "Verify Idle RPM", "0000");

    Log(LOG_DEV_DATA, "ManualTransmissionTC::TestStepVerifyIdleRPM returning %s\n", testResult.c_str());
    Log(LOG_FN_ENTRY, "Exit ManualTransmissionTC::TestStepVerifyIdleRPM\n");
    return testResult;
}
