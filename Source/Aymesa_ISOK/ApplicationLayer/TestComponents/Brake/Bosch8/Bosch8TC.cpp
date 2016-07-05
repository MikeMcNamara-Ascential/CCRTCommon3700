//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Bosch8TC.cpp 3     5/23/06 11:28a Gswope $
//
// FILE DESCRIPTION:
//		ABS test component for the Bosch8 system.
//
//===========================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Bosch8TC.cpp $
// 
// 3     5/23/06 11:28a Gswope
// 1) Added CMABSValveFiringTest test step and associated function. this test
//  fires the rear valves first. It is NOT being used at present, as there was
//  no improvement over the previous method.
// 2) Modified default values for initial speed deltas to a realistic value
//  This allows the read to be commented out to simplify debug of valve firing
// 
// 2     5/15/06 8:32a Gswope
// fixed cascade of fail logic in ABSValveFiringTest()
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 3     1/30/06 3:41p Gswope
// in EvaluateSensorCross(), modified second comparisons (two places) to 
// improve quality of evaluation
// 
// 2     3/02/05 11:17a Gswope
// Emergency request from HMMA to remove rear valve firing from ESP test
// 2005.02.28 ews removed at HMMA emergency request
// changed ESP valve firing to use only front wheels in method
// ESPValveFiringTest()
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 5     11/24/04 11:03a Bmeinke
// Updated the individual ABS test methods to report inlet/outlet valve
// actuation results as required per the HMMA QMS spec
// Updated the EvaluateSensorCross() method to report front sensor cross,
// rear sensor cross and front-to-rear sensor cross results as required
// per the HMMA QMS spec
// General beautification
// 
// 3     11/15/04 9:21a Bmeinke
// Added ESPSteps_t enum to define the steps required during an ESP valve
// firing test
// Allocate the m_ESPIndex array in the constructor and delete it in the
// destructor
// Added handlers for EvaluateSensorCross, ReadSpeedDeltas and
// StopESPPumpMotor inside CommandTestStep
// Added ReadSpeedDeltas() method
// Call UpdatePrompts()/RemovePrompts() at the beginning/end of the
// ESPValveFiringTest() method
// Changed the **ESPTest() methods into state machines using the new
// ESPTestSteps_t
// Removed the local redSpeeds and recSpeeds variables inside the
// **ABSTest() methods. Use the class variable m_absSpeedDeltas instead
// Added new EvaluateSensorCross() method to perform an integrated sensor
// cross check using the speed delta values returned during ABS valve
// actuation.
// Override StopPumpMotor() and BrakeSwitchTest() to call
// UpdatePrompts()/RemovePrompts() at the beginning.end of the test step.
// Added m_initSpeedDelta data member to record the initial decel rate at
// the end of base brake. Used as a reference for the sensor cross
// evaluation
// Added m_absSpeedDeltas array to hold the decel rates during ABS valve
// actuation. Used to evaluate sensor cross.
// 
// 3     11/10/04 1:35a Bmeinke
// Added defines (RED_DELTA_IDX and REC_DELTA_IDX) for the wheel speed
// delta array indices
// Need to allocate the m_ESPIndex array in the constructor and delete it
// in the destructor
// Fixed the m_ESPIndex[LFWHEEL} typo in RFESPTest()
// Changed the integrated sensor cross algorithm to use the speed deltas
// obtained during the recovery cycles instead of the ones received during
// the reduction cycle
// 
// 2     10/27/04 8:59p Bmeinke
// Added EvaluateSensorCross test step and method.
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 2     10/26/04 1:54p Bmeinke
// Fixed the names of the parameters used for the SensorQuality test
// Declutch the machine at the end of the SensorQuality test (instead of
// commanding the motors to zero speed) to avoid huge force spikes
// In LFAbsTest(), RFAbsTest(), LRAbsTest() and RRAbsTest(), we need to do
// a signed comparison of the wheel speed deltas to properly evaluate
// sensor cross
// ALWAYS tag the end of the recovery- even if testResult is not testPass
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
//
//*************************************************************************
#include "Bosch8TC.h"

template <class ModuleType>
Bosch8TC<ModuleType>::Bosch8TC() : KoreaAbsTcTemplate<ModuleType>()
{
    m_ESPIndex = new BuildReductionIndex[RRWHEEL+1];
}

template <class ModuleType>
Bosch8TC<ModuleType>::~Bosch8TC()
{
    if(m_ESPIndex != NULL)
    {
        delete[] m_ESPIndex;
        m_ESPIndex = NULL;
    }
}

template <class ModuleType>
const string Bosch8TC<ModuleType>::Bosch8TC<ModuleType>::CommandTestStep(const string &value)
{
    string status;
    try
    {
        string step = GetTestStepName(); // Get the name of the test step
        Log(LOG_DEV_DATA, "Bosch8TC::CommandTestStep(%s): Entering test step %s\n", 
            value.c_str(),step.c_str());

        // make sure it is OK to test
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {
            status = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS,"Bosch8TC::CommandTestStep: StatusCheck() failed: %s\n",
                status.c_str());
            SendTestResult(status,GetTestStepInfo("Description"));
        }
        else if(step == "CheckRelayState") status = CheckRelayState();
        else if(step == "CheckSupplyVoltage") status = CheckSupplyVoltage();
        else if(step == "CheckPumpMotor") status = CheckPumpMotor();
        else if(step == "CheckParkBrakeSignal")   status = CheckParkBrakeSignal();
        else if(step == "CheckEngineSpeed") status = CheckEngineSpeed();
        else if(step == "CheckThrottlePosition") status = CheckThrottlePosition();
        else if(step == "CheckShiftLeverPosition") status = CheckShiftLeverPosition();
        else if(step == "CheckPassiveSwitch") status = CheckPassiveSwitch();
        else if(step == "CheckSteeringWheelAngle") status = CheckSteeringWheelAngle();
        else if(step == "CheckLateralAcceleration")  status = CheckLateralAcceleration();
        else if(step == "CheckYawRate")  status = CheckYawRate();
        else if(step == "CheckPressureSensor") status = CheckPressureSensor();
        else if(step == "CheckAYSensorStatus") status = CheckAYSensorStatus();
        else if(step == "SensorQualityTest") status = SensorQualityTest();
        else if(step == "ESPValveFiringTest") status = ESPValveFiringTest();
        else if(step == "ABSValveFiringTest") status = ABSValveFiringTest();
        else if(step == "DisableSpeedLimit") status = DisableSpeedLimit();
        else if(step == "DisableValveRelayShutdown") status = DisableValveRelayShutdown();
        else if(step == "EnableSpeedLimit") status = EnableSpeedLimit();
        else if(step == "EnableValveRelayShutdown")  status = EnableValveRelayShutdown();
        else if(step == "CheckUplineProcessByte") status = CheckUplineProcessByte();
        else if(step == "ReadSensorSpeeds") status = ReadSensorSpeeds();
        else if(step == "IgnitionOff") status = IgnitionOff();
        else if(step == "IgnitionOn") status = IgnitionOn();
        else if(step == "EvaluateSensorCross") status = EvaluateSensorCross();
        else if(step == "ReadSpeedDeltas") status = ReadSpeedDeltas();
        else if(step == "StopESPPumpMotor") status = StopPumpMotor();
        else status = KoreaAbsTcTemplate<ModuleType>::CommandTestStep(value);
    }
    catch(BepException &err)
    {
        Log("Bosch8::CommandTestStep %s BepException: %s\n",
            GetTestStepName().c_str(),err.what());
        status = BEP_ERROR_RESPONSE;
    }
    Log(LOG_DEV_DATA,"Bosch8TC::CommandTestStep(%s) returning %s\n",
        value.c_str(),status.c_str());

    return(status);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::EnterDiagnosticMode(void)
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter EnterDiagnosticMode::EnterDiagnosticMode()\n");

    if(CheckZeroSpeed())
    {
        UpdatePrompts();

        // Lety the driver react to the prompts
        BposSleep(2000);

        testResult = KoreaAbsTcTemplate<ModuleType>::EnterDiagnosticMode();
        RemovePrompts();
    }
    else
    {
        Log( LOG_ERRORS, "Failed to enter zerospeed in Bosch8TC::EnterDiagnosticMode()\n");
        testResult = BEP_FAIL_STATUS;
    }

    Log(LOG_FN_ENTRY, "Exit EnterDiagnosticMode::EnterDiagnosticMode()\n");

    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckRelayState(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    vector<bool> relays;
    bool motorRelay;
    bool valveRelay;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckRelayState()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadRelayState",relays);

        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            motorRelay = relays[0];
            valveRelay = relays[1];

            Log(LOG_DEV_DATA,"Read Relay State: motor=%d valve=%d\n",motorRelay,valveRelay);

            if(!motorRelay && valveRelay)
            {
                Log(LOG_DEV_DATA,"Correct relay state\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_ERRORS,"Incorrect relay state\n");
                testResult = testFail;
            }
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadValveState"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadValveState"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading valve state - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Valve State: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckRelayState - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckRelayState()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckSupplyVoltage(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float supplyVoltage;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckSupplyVoltage()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadSupplyVoltage",supplyVoltage);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Supply Voltage: %.2f V\n",supplyVoltage);

            if((supplyVoltage >= 9.4) && (supplyVoltage <= 16))
            {
                Log(LOG_DEV_DATA,"Supply Voltage pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Supply Voltage fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadSupplyVoltage"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadSupplyVoltage"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading supply voltage - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Supply Voltage: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::SupplyVoltage - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::SupplyVoltage()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckPumpMotor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool pumpState;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckPumpMotor()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadPumpStatus",pumpState);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Pump Status: %d\n",pumpState);

            if(pumpState == false)
            {
                Log(LOG_DEV_DATA,"Pump Motor State pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Pump Motor State fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadPumpStatus"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadPumpStatus"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading supply voltage - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Pump Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckPumpMotor - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckPumpMotor()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckParkBrakeSignal(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool parkBrakeSignal;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckParkBrakeSignal()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadParkBrakeSignal",parkBrakeSignal);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Park Brake Signal: %d\n",parkBrakeSignal);

            if(parkBrakeSignal == false)
            {
                Log(LOG_DEV_DATA,"Park Brake Signal off pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Park Brake Signal off fail\n");
                testResult = testFail;
            }

            if(testResult == testPass)
            {
                DisplayPrompt(1,"ApplyParkingBrake");
                BposSleep(2000);

                // Try to read the system data
                moduleStatus = m_vehicleModule.GetInfo("ReadParkBrakeSignal",parkBrakeSignal);

                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                if(testResult == testPass)
                {
                    Log(LOG_DEV_DATA,"Park Brake Signal: %d\n",parkBrakeSignal);

                    if(parkBrakeSignal == true)
                    {
                        Log(LOG_DEV_DATA,"Park Brake Signal on pass\n");
                        testResult = testPass;
                    }
                    else
                    {
                        Log(LOG_DEV_DATA,"Park Brake Signal on fail\n");
                        testResult = testFail;
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading park brake signal - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading park brake signal - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadParkBrakeSignal"));
        testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadParkBrakeSignal"));

        RemovePrompt(1,"ApplyParkingBrake");
        DisplayPrompt(1,"ReleaseParkingBrake");
        BposSleep(2000);
        RemovePrompt(1,"ReleaseParkingBrake");

        Log(LOG_DEV_DATA, "Read Park Brake Signal: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckParkBrakeSignal - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckParkBrakeSignal()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckEngineSpeed(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float engineSpeed;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckEngineSpeed()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadEngineSpeed",engineSpeed);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Engine Speed: %.2f\n",engineSpeed);

            if(ReadSubscribeData("Zerospeed") == "1")
            {
                if(engineSpeed == 0)
                {
                    Log(LOG_DEV_DATA,"Engine Speed Zerospeed pass\n");
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_DEV_DATA,"Engine Speed Zerospeed fail\n");
                    testResult = testFail;
                }
            }
            else
            {
                if(engineSpeed > 0)
                {
                    Log(LOG_DEV_DATA,"Engine Speed non-Zerospeed pass\n");
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_DEV_DATA,"Engine Speed non-Zerospeed fail\n");
                    testResult = testFail;
                }
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadEngineSpeed"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadEngineSpeed"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading engine speed - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Engine Speed: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckEngineSpeed - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckEngineSpeed()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::IgnitionOff()
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float minEngineRpm = GetParameterFloat( "IgnitionOffRpm");
    float engineRpm = minEngineRpm + 1;
    bool sawEngineOff = false;

    // Check if we need to skip this step
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::IgnitionOff()\n");
    if(!ShortCircuitTestStep() && CheckCableConnect())
    {  // Need to perform this test step
        try
        {
            UpdatePrompts();

            // Wait for timeout or for engine RPM to drop off
            while((TimeRemaining()) && (sawEngineOff == false) && (BEP_STATUS_SUCCESS == StatusCheck()))
            {
                // Try to read the engine RPM
                moduleStatus = m_vehicleModule.GetInfo( GetDataTag("ReadEngineSpeed"), engineRpm);
                switch(moduleStatus)
                {
                case BEP_STATUS_SUCCESS:
                    // We got a valid reply...engine must be on
                    Log( LOG_DEV_DATA, "IgnitionOff: RPM=%.2f, min=%.2f\n", engineRpm, minEngineRpm);
                    if(engineRpm < minEngineRpm)
                    {
                        sawEngineOff = true;
                    }
                    break;
                case BEP_STATUS_SOFTWARE:
                    // Error trying to parse the response...keep trying
                    Log( LOG_ERRORS, "IgnitionOff: software error\n");
                    break;
                case BEP_STATUS_ERROR:
                    // No response...

                    // Make sure that the cable is still connected
                    if(CheckCableConnect())
                    {
                        Log( LOG_ERRORS, "IgnitionOff: no response -> engine is off\n");
                        moduleStatus = BEP_STATUS_SUCCESS;
                        sawEngineOff = true;
                    }
                    break;
                default:
                    break;
                }

                // Make sure that CheckCableConnect does not overwrite our prompts
                UpdatePrompts();
            }
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            // Set the rest of the data
            Log(LOG_DEV_DATA, "IgnitionOff: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // Remove our prompts
        RemovePrompts();

        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {  // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::IgnitionOff()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::IgnitionOn()
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float minEngineRpm = GetParameterFloat( "IgnitionOnRpm");
    float engineRpm = minEngineRpm + 1;
    bool sawEngineOn = false;

    // Check if we need to skip this step
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::IgnitionOn()\n");
    if(!ShortCircuitTestStep() && CheckCableConnect())
    {  // Need to perform this test step
        try
        {
            UpdatePrompts();

            // Wait for timeout or for engine RPM to drop off
            while((TimeRemaining()) && (sawEngineOn == false) && (BEP_STATUS_SUCCESS == StatusCheck()))
            {
                // Try to read the engine RPM
                moduleStatus = m_vehicleModule.GetInfo( GetDataTag("ReadEngineSpeed"), engineRpm);
                switch(moduleStatus)
                {
                case BEP_STATUS_SUCCESS:
                    // We got a valid reply...engine must be on
                    Log( LOG_DEV_DATA, "IgnitionOn: RPM=%.2f, min=%.2f\n", engineRpm, minEngineRpm);
                    if(engineRpm >= minEngineRpm)
                    {
                        sawEngineOn = true;
                    }
                    break;
                case BEP_STATUS_SOFTWARE:
                    // Error trying to parse the response...keep trying
                    Log( LOG_ERRORS, "IgnitionOn: software error\n");
                    break;
                case BEP_STATUS_ERROR:
                    // No response...
                    Log( LOG_ERRORS, "IgnitionOn: no response\n");
                    // Make sure that the cable is still connected
                    (void)CheckCableConnect();
                    break;
                default:
                    break;
                }

                // Make sure that CheckCableConnect does not overwrite our prompts
                UpdatePrompts();
            }
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            // Set the rest of the data
            Log(LOG_DEV_DATA, "IgnitionOn: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
        }
        catch(ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // Remove our prompts
        RemovePrompts();

        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {  // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::IgnitionOn()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckThrottlePosition(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float throttlePosition;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckThrottlePosition()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadThrottlePosition",throttlePosition);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Throttle Position: %.2f\n",throttlePosition);

            if(throttlePosition <= 4)
            {
                Log(LOG_DEV_DATA,"Throttle Position off pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Throttle Position off fail\n");
                testResult = testFail;
            }

            if(testResult == testPass)
            {
                DisplayPrompt(1,"FullThrottle");
                BposSleep(2000);

                // Try to read the system data
                moduleStatus = m_vehicleModule.GetInfo("ReadThrottlePosition",throttlePosition);

                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                if(testResult == testPass)
                {
                    Log(LOG_DEV_DATA,"Throttle Position: %.2f\n",throttlePosition);

                    if(throttlePosition >= 96)
                    {
                        Log(LOG_DEV_DATA,"Throttle Position on pass\n");
                        testResult = testPass;
                    }
                    else
                    {
                        Log(LOG_DEV_DATA,"Throttle Position on fail\n");
                        testResult = testFail;
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading throttle position - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading throttle position - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadParkBrakeSignal"));
        testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadParkBrakeSignal"));

        RemovePrompt(1,"FullThrottle");

        Log(LOG_DEV_DATA, "Read Throttle Position: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckThrottlePosition - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckThrottlePosition()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckShiftLeverPosition(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT16 shiftLeverPosition;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckShiftLeverPosition()\n");
    try
    {
        DisplayPrompt(1,"ShiftToPark");
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadShiftLeverPosition",shiftLeverPosition);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Shift Lever Position: 0x%2x\n",shiftLeverPosition);

            if(shiftLeverPosition == 0x01)
            {
                Log(LOG_DEV_DATA,"Shift Lever Position Park pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Shift Lever Position Park fail\n");
                testResult = testFail;
            }

            if(testResult == testPass)
            {
                RemovePrompt(1,"ShiftToPark");
                DisplayPrompt(1,"ShiftToReverse");
                BposSleep(2000);

                // Try to read the system data
                moduleStatus = m_vehicleModule.GetInfo("ReadShiftLeverPosition",shiftLeverPosition);

                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                if(testResult == testPass)
                {
                    Log(LOG_DEV_DATA,"Shift Lever Position: 0x%2x\n",shiftLeverPosition);

                    if(shiftLeverPosition == 0x02)
                    {
                        Log(LOG_DEV_DATA,"Shift Lever Position Reverse pass\n");
                        testResult = testPass;
                    }
                    else
                    {
                        Log(LOG_DEV_DATA,"Shift Lever Position Reverse fail\n");
                        testResult = testFail;
                    }

                    if(testResult == testPass)
                    {
                        RemovePrompt(1,"ShiftToReverse");
                        DisplayPrompt(1,"ShiftToDrive");
                        BposSleep(2000);

                        // Try to read the system data
                        moduleStatus = m_vehicleModule.GetInfo("ReadShiftLeverPosition",shiftLeverPosition);

                        // Determine the test result
                        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                        if(testResult == testPass)
                        {
                            Log(LOG_DEV_DATA,"Shift Lever Position: 0x%2x\n",shiftLeverPosition);

                            if(shiftLeverPosition == 0x04)
                            {
                                Log(LOG_DEV_DATA,"Shift Lever Position Drive pass\n");
                                testResult = testPass;
                            }
                            else
                            {
                                Log(LOG_DEV_DATA,"Shift Lever Position Drive fail\n");
                                testResult = testFail;
                            }
                        }
                        else
                        {
                            testResult = testFail;
                            testResultCode = GetFaultCode("CommunicationFailure");
                            testDescription = GetFaultDescription("CommunicationFailure");
                            SetCommunicationFailure(true);
                            Log(LOG_ERRORS, "Error reading throttle position - status: %s\n", 
                                ConvertStatusToResponse(moduleStatus).c_str());
                        }
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading throttle position - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading throttle position - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadShiftLeverPosition"));
        testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadShiftLeverPosition"));

        RemovePrompt(1,"ShiftToDriver");

        Log(LOG_DEV_DATA, "Read Shift Lever Position: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckShiftLeverPosition - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckShiftLeverPosition()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckPassiveSwitch(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool passiveSwitchState;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckPassiveSwitch()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadPassiveSwitchState",passiveSwitchState);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Passive switch state: %d\n",passiveSwitchState);

            if(passiveSwitchState == 0)
            {
                Log(LOG_DEV_DATA,"Passive Switch off pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Passive Switch off fail\n");
                testResult = testFail;
            }

            if(testResult == testPass)
            {
                DisplayPrompt(1,"TurnPassiveSwitchOn");
                BposSleep(2000);

                moduleStatus = m_vehicleModule.GetInfo("ReadPassiveSwitchState",passiveSwitchState);

                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                if(testResult == testPass)
                {
                    Log(LOG_DEV_DATA,"Passive switch state: %d\n",passiveSwitchState);

                    if(passiveSwitchState == 1)
                    {
                        Log(LOG_DEV_DATA,"Passive Switch on pass\n");
                        testResult = testPass;
                    }
                    else
                    {
                        Log(LOG_DEV_DATA,"Passive Switch on fail\n");
                        testResult = testFail;
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading passive switch - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }                    
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading passive switch - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadPassiveSwitch"));
        testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadPassiveSwitch"));

        RemovePrompt(1,"TurnPassiveSwitchOn");
        DisplayPrompt(1,"TurnPassiveSwitchOff");
        BposSleep(2000);
        RemovePrompt(1,"TurnPassiveSwitchOff");

        Log(LOG_DEV_DATA, "Read Passive Switch: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckPassiveSwitch - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckPassiveSwitch()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckSteeringWheelAngle(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float steeringWheelAngle;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckSteeringWheelAngle()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadSteeringWheelAngle",steeringWheelAngle);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Steering Wheel Angle: %.2f\n",steeringWheelAngle);

            // if steering wheel angle is between -15 and 15
            if((steeringWheelAngle >= -15) && (steeringWheelAngle <= 15))
            {
                Log(LOG_DEV_DATA,"Steering wheel angle pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Steering wheel angle fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadSteeringWheelAngle"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadSteeringWheelAngle"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading steering wheel angle - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Steering Wheel Angle: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckSteeringWheelAngle - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckSteeringWheelAngle()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckLateralAcceleration(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float lateralAcceleration;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckLateralAcceleration()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadLateralAcceleration",lateralAcceleration);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Lateral Acceleration: %.2f\n",lateralAcceleration);

            // if lateral acceleration is between -0.09 and 0.09
            if((lateralAcceleration >= -0.09) && (lateralAcceleration <= 0.09))
            {
                Log(LOG_DEV_DATA,"Lateral acceleration pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Lateral acceleration fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadLateralAcceleration"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadLateralAcceleration"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading lateral acceleration - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Lateral Acceleration: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckLateralAcceleration - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckLateralAcceleration()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckYawRate(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float yawRate;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckYawRate()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadYawRate",yawRate);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Yaw Rate: %.2f\n",yawRate);

            // if lateral acceleration is between -4 and 4
            if((yawRate >= -4) && (yawRate <= 4))
            {
                Log(LOG_DEV_DATA,"Yaw rate pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Yaw rate fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadYawRate"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadYawRate"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading yaw rate - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Yaw Rate: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckYawRate - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckYawRate()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckPressureSensor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float pressure;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckPressureSensor()\n");
    try
    {
        DisplayPrompt(1,"DepressBrake");
        BposSleep(2000);

        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadPressureSensor",pressure);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Pressure: %.2f\n",pressure);

            if(pressure > 15)
            {
                Log(LOG_DEV_DATA,"Pressure on pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"Pressure on fail\n");
                testResult = testFail;
            }

            if(testResult == testPass)
            {
                RemovePrompt(1,"DepressBrake");
                DisplayPrompt(1,"FootOffBrake");
                BposSleep(2000);

                // Try to read the system data
                moduleStatus = m_vehicleModule.GetInfo("ReadPressureSensor",pressure);

                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                if(testResult == testPass)
                {
                    Log(LOG_DEV_DATA,"Pressure: %.2f\n",pressure);
                    if((pressure >= 0) && (pressure <= 15))
                    {
                        Log(LOG_DEV_DATA,"Pressure pass\n");
                        testResult = testPass;
                    }
                    else
                    {
                        Log(LOG_DEV_DATA,"Pressure fail\n");
                        testResult = testFail;
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    SetCommunicationFailure(true);
                    Log(LOG_ERRORS, "Error reading pressure sensor - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadPressure"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadPressure"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading pressure sensor - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read Pressure Sensor: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckPressureSensor - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckPressureSensor()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckAYSensorStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool sensorStatus;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckAYSensorStatus()\n");
    try
    {
        // Try to read the system data
        moduleStatus = m_vehicleModule.GetInfo("ReadAYSensorStatus",sensorStatus);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"AY Sensor Status: %d\n",sensorStatus);

            if(sensorStatus == 0)
            {
                Log(LOG_DEV_DATA,"AY Sensor status pass\n");
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"AY Sensor status fail\n");
                testResult = testFail;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("ReadAYSensorStatus"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("ReadAYSensorStatus"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading AY sensor status - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Read AY Sensor Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::CheckAYSensorStatus() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckAYSensorStatus()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::DisableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::DisableSpeedLimit()\n");
    try
    {
        // Try to disable the speed limit
        moduleStatus = m_vehicleModule.GetInfo("DisableSpeedLimit");

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass) Log(LOG_DEV_DATA,"Speed Limit disabled\n");
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error disabling speed limit - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Disable Speed Limit Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::DisableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::DisableSpeedLimit()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::DisableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::DisableValveRelayShutdown()\n");
    try
    {
        // Try to disable the valve shutdown
        moduleStatus = m_vehicleModule.GetInfo("DisableValveRelayShutdown");

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass) Log(LOG_DEV_DATA,"Valve Shutdown disabled\n");
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error disabling valve shutdown - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Disable Valve Shutdown Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::DisableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::DisableValveRelayShutdown()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::ReadSpeedDeltas(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter Bosch8TC::ReadSpeedDeltas()\n");

    // Initialize the initial speed delta values
    m_initSpeedDelta[ LFWHEEL] = 1.0;
    m_initSpeedDelta[ RFWHEEL] = 1.0;
    m_initSpeedDelta[ LRWHEEL] = 1.0;
    m_initSpeedDelta[ RRWHEEL] = 1.0;




    if(!GetParameterBool("CheckResponseInProgress"))
    {
        // Read current decel rate
        moduleStatus = m_vehicleModule.GetInfo("ReadSpeedDeltas", m_initSpeedDelta);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    else
    {
        moduleStatus = m_vehicleModule.CommandModule("ReadSpeedDeltas");
        moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_initSpeedDelta);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    if(testResult == testPass)
    {
        Log(LOG_DEV_DATA, "Initial Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_initSpeedDelta[LFWHEEL], m_initSpeedDelta[RFWHEEL], 
            m_initSpeedDelta[LRWHEEL], m_initSpeedDelta[RRWHEEL]);
    }
    else
    {
        testResultCode = GetFaultCode("CommunicationFailure");
        testDescription = GetFaultDescription("CommunicationFailure");
        SetCommunicationFailure(true);
        Log(LOG_ERRORS, "Error reading ABS speed deltas - status: %s\n", 
            ConvertStatusToResponse(moduleStatus).c_str());
    }

    SendTestResult(testResult, testDescription, testResultCode);

    Log( LOG_FN_ENTRY, "Exit Bosch8TC::ReadSpeedDeltas(), status=%s\n", testResult.c_str());

    return( testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::SensorQualityTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    vector<WheelSpeeds_t> speeds;
    WheelSpeeds_t wheelSpeedsMin;
    WheelSpeeds_t wheelSpeedsMax;
    string wheelResult[4];
    const float minTolerance = GetParameterFloat("SensorQualityMinTolerance");
    const float maxTolerance = GetParameterFloat("SensorQualityMaxTolerance");
    const float targetSpeed = GetParameterFloat("SensorQualitySpeed");

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::SensorQualityTest()\n");

    try
    {
        UpdatePrompts();

        // If we are at zerospeed, we need to drive the rollers up to speed
        if(ReadSubscribeData(GetDataTag("Zerospeed")) == "1")
        {
            SystemCommand(COMMAND_SPEED, targetSpeed);
        }

        // Try to start the sensor quality test
        moduleStatus = m_vehicleModule.GetInfo("StartSensorQualityTest",speeds);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA,"Sensor quality test complete\n");

            wheelSpeedsMin = speeds[0];
            wheelSpeedsMax = speeds[1];

            Log(LOG_DEV_DATA, "Sensor Quality Min Speeds: LF = %g RF = %g LR = %g RR = %g\n",
                wheelSpeedsMin[LFWHEEL],wheelSpeedsMin[RFWHEEL],
                wheelSpeedsMin[LRWHEEL],wheelSpeedsMin[RRWHEEL]);

            Log(LOG_DEV_DATA, "Sensor Quality Max Speeds: LF = %g RF = %g LR = %g RR = %g\n",
                wheelSpeedsMax[LFWHEEL],wheelSpeedsMax[RFWHEEL],
                wheelSpeedsMax[LRWHEEL],wheelSpeedsMax[RRWHEEL]);

            for(UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
            {
                // Calculate the upper and lower wheel speed limits
                float lowerLimit = targetSpeed - minTolerance;
                float upperLimit = targetSpeed + maxTolerance;

                // Check the wheel speed sensor against the limits
                if(wheelSpeedsMax[wheelIndex] > upperLimit)
                {
                    Log( LOG_DEV_DATA, "Wheel %d failed sensor quality for upper limit\n", wheelIndex);
                    wheelResult[wheelIndex] = testFail;
                    testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                    testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                }
                else if(wheelSpeedsMin[wheelIndex] < lowerLimit)
                {
                    Log( LOG_DEV_DATA, "Wheel %d failed sensor quality for lower limit\n", wheelIndex);
                    wheelResult[wheelIndex] = testFail;
                    testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                    testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                }
                else
                {
                    Log( LOG_DEV_DATA, "Wheel %d passed sensor quality\n", wheelIndex);
                    wheelResult[wheelIndex] = testPass;
                    testResultCode = "0000";
                    testDescription = rollerName[wheelIndex] + " wheel speed sensor in tolerance\n";
                }

                // Log the data and report the result for this wheel
                Log(LOG_DEV_DATA, "%s wheel speed sensor %s Lower Limit: %.2f Upper Limit: %.2f"
                    "\nMin Sensor Speed: %.2f\nMax Sensor Speed: %.2f\n",
                    rollerName[wheelIndex].c_str(), wheelResult[wheelIndex].c_str(), lowerLimit, upperLimit, 
                    wheelSpeedsMin[wheelIndex],wheelSpeedsMax[wheelIndex]);

                char temp[16];
                SendSubtestResultWithDetail(rollerName[wheelIndex]+GetTestStepName(), wheelResult[wheelIndex], 
                                            testDescription, testResultCode,
                                            "MinSensorSpeed", CreateMessage(temp, sizeof(temp), "%.2f", wheelSpeedsMin[wheelIndex]), "MPH",
                                            "MaxSensorSpeed", CreateMessage(temp, sizeof(temp), "%.2f", wheelSpeedsMax[wheelIndex]), "MPH",
                                            "LowerLimit", CreateMessage(temp, sizeof(temp), "%.2f", lowerLimit), "MPH",
                                            "UpperLimit", CreateMessage(temp, sizeof(temp), "%.2f", upperLimit), "MPH");
                // Update the overall result
                if(testPass != wheelResult[wheelIndex])
                {
                    testResult = wheelResult[wheelIndex];
                    Log(LOG_DEV_DATA, "Sensor Quality Test Status: %s\n", testResult.c_str());
                }
            }

            // Determine the description and code to report
            if(testPass == testResult)
            {
                testResult = testPass;
                testResultCode = "0000";
                testDescription = GetTestStepInfo("Description");
            }
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode(GetTestStepName()+"Fail");
                testDescription = GetFaultDescription(GetTestStepName()+"Fail");
            }
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error starting sensor quality test - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Sensor Quality Test Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::SensorQualityTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Declutch the machine so the driver can stop the vehicle
    Log(LOG_DEV_DATA, "Declutching\n");
    Disengage();

    RemovePrompts();

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::SensorQualityTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::ESPValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::ESPValveFiringTest()\n");
    try
    {
        UpdatePrompts();

        m_ESPStartIndex = TagArray("ESPStart");

        // run the individual wheel ESP tests
        testResult = LFESPTest();
        if(testResult == testPass) RFESPTest();

        // 2005.02.28 ews removed at HMMA emergency request
        //			if(testResult == testPass) LRESPTest();
        //			if(testResult == testPass) RRESPTest();

        m_ESPEndIndex = TagArray("ESPEnd");

        // Determine the description and code to report
        if(GetCommunicationFailure() == true)
        {
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
        else
        {
            testResultCode = testPass == testResult ? "0000" : GetFaultCode(GetTestStepName()+"Fail");
            testDescription = testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription(GetTestStepName()+"Fail");
        }
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::ESPValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    RemovePrompts();

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::ESPValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::ABSValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::ABSValveFiringTest()\n");
    try
    {

        UpdatePrompts();
        // Determine if the force meter should be updated during valve firing
        if (GetParameterBool("DisableMeterForValveFiring"))
            m_baseBrakeTool->DisableForceUpdates();
        else
            m_baseBrakeTool->EnableForceUpdates();
        m_absStartIndex = TagArray("ABSStart");
        // run the individual wheel ABS tests
        // run the individual wheel ABS tests
        testResult = LFABSTest();
        // fixed testResult setting for subsequent tests
        if(testResult == testPass) testResult = RFABSTest();
        if(testResult == testPass) testResult = LRABSTest();
        if(testResult == testPass) testResult = RRABSTest();
        m_absEndIndex = TagArray("ABSEnd");

        // Determine the description and code to report
        if(GetCommunicationFailure() == true)
        {
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
        else
        {
            // added parens (they are FREE!) to next two lines, to make it easier to read
            testResultCode  = ((testPass == testResult) ? "0000" : GetFaultCode(GetTestStepName()+"Fail"));
            testDescription = ((testPass == testResult) ? GetTestStepInfo("Description") : GetFaultDescription(GetTestStepName()+"Fail"));
        }
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::ABSValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Remove the operator prompts
    RemovePrompts();
    m_baseBrakeTool->EnableForceUpdates();

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::ABSValveFiringTest()\n");
    return(testResult);
}


template <class ModuleType>
string Bosch8TC<ModuleType>::EnableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::EnableSpeedLimit()\n");
    try
    {
        if(GetParameterBool("CheckResponseInProgress"))
        {
            m_vehicleModule.ReadModuleData("ReadEnableSpeedLimitInProgress",controlInProgress);
        }
        if(controlInProgress)
        {
            // Try to disable the speed limit
            moduleStatus = m_vehicleModule.GetInfo("EnableSpeedLimit");

            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            if(testResult == testPass) Log(LOG_DEV_DATA,"Speed Limit enabled\n");
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
                Log(LOG_ERRORS, "Error enabling speed limit - status: %s\n", 
                    ConvertStatusToResponse(moduleStatus).c_str());
            }

            Log(LOG_DEV_DATA, "Enable Speed Limit Status: %s - status: %s\n", 
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "Valve Relay Shutdown not in progress skip\n"); 
            testResult = testSkip;
        }
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::EnableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::EnableSpeedLimit()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::EnableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::EnableValveRelayShutdown()\n");
    try
    {
        if(GetParameterBool("CheckResponseInProgress"))
        {
            m_vehicleModule.ReadModuleData("ReadValveRelayShutdownInProgress",controlInProgress);
        }
        if(controlInProgress)
        {
            //if control in progress or not checking control
            // Try to disable the valve shutdown
            moduleStatus = m_vehicleModule.GetInfo("EnableValveRelayShutdown");

            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            if(testResult == testPass) Log(LOG_DEV_DATA,"Valve Shutdown enabled\n");
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
                Log(LOG_ERRORS, "Error enabling valve shutdown - status: %s\n", 
                    ConvertStatusToResponse(moduleStatus).c_str());
            }

            Log(LOG_DEV_DATA, "Enable Valve Shutdown Status: %s - status: %s\n", 
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
        }
        else
        {
            Log(LOG_DEV_DATA, "Valve Relay Shutdown not in progress skip\n"); 
            testResult = testSkip;
        }
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::EnableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::EnableValveRelayShutdown()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::CheckUplineProcessByte(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT8 processByte;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::CheckUplineProcessByte()\n");
    try
    {
        // Try to disable the valve shutdown
        moduleStatus = m_vehicleModule.GetInfo("CheckProcessByte", processByte);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA, "Process byte: $%02hhX\n");
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error enabling valve shutdown - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Enable Valve Shutdown Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in Bosch8TC::EnableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::CheckUplineProcessByte()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::ReadSensorSpeeds(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    WheelSpeeds_t moduleSpeeds;

    Log(LOG_DEV_DATA, "Enter KoreaAbsTcTemplate::ReadSensorSpeeds()\n");

    try
    {
        // Ask the module for sensor speeds
        status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

        if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
        {
            Log( LOG_DEV_DATA, "LF Sensor Speed: %.2f\n", moduleSpeeds[LFWHEEL]);
            Log( LOG_DEV_DATA, "RF Sensor Speed: %.2f\n", moduleSpeeds[RFWHEEL]);
            Log( LOG_DEV_DATA, "LR Sensor Speed: %.2f\n", moduleSpeeds[LRWHEEL]);
            Log( LOG_DEV_DATA, "RR Sensor Speed: %.2f\n", moduleSpeeds[RRWHEEL]);
            testResult = testPass;
        }
        else                 // error reading the sensors
        {
            SetCommunicationFailure(true);     // set comm fault flag
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

    if(testResult == testPass)    // if the test step passed
    {
        SendTestResult(testPass,testDescription);
    }
    else                    // test step failed
    {
        if(GetCommunicationFailure() == false)   // if no comm error
        {
            SendTestResultWithDetail(testResult,testDescription,
                                     GetFaultCode(testDescription),
                                     GetFaultName(testDescription),
                                     GetFaultDescription(testDescription));
        }
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::ReadSensorSpeeds()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::LFESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::LFESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            espCommand = "LFESPInit";
            break;
        case ESP_PUMP_ON:
            espCommand = "ESPPumpOn1";
            break;
        case ESP_PRIMARY_ON:
            // NOP
            break;
        case ESP_START:
            m_ESPIndex[LFWHEEL].buildStart = TagArray("LFESPBuildStart");
            espCommand = "LFESPStart";
            break;
        case ESP_PRIMARY_OFF:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LFWHEEL].buildEnd = TagArray("LFESPBuildEnd");
            espCommand = "ESPPrimaryValve1Off";
            break;
        case ESP_END:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LFWHEEL].reductionStart = TagArray("LFESPReductionStart");
            else m_ESPIndex[LFWHEEL].buildEnd = TagArray("LFESPBuildEnd");
            espCommand = "LFESPEnd";
            break;
        case ESP_FINALIZE:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LFWHEEL].reductionEnd = TagArray("LFESPReductionEnd");
            else m_ESPIndex[LFWHEEL].reductionStart = TagArray("LFESPReductionStart");
            espCommand = "LFESPFinalize";
            break;
        case ESP_DONE:
            if(GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LFWHEEL].reductionEnd = TagArray("LFESPReductionEnd");
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {

            moduleStatus = m_vehicleModule.GetInfo(espCommand);
            if(espCommand == "LFESPEnd" && GetParameterBool("CheckResponseInProgress"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && espCommand != "LFESPEnd")
            {//New Bosch module does not send busy response
                WaitForControlComplete();
                /*if(espCommand  == "LFESPEnd")
                {
                    delay(GetParameterInt("LFReductionESPPulse"));
                }
                else if(espCommand == "LFESPStart")
                {
                    delay(GetParameterInt("LFRecoveryESPPulse"));
                }*/
            }
            if(moduleStatus != BEP_STATUS_SUCCESS)
            {
                SetCommunicationFailure(true);
                Log( LOG_ERRORS, "Error sending %s - status: %s\n", espCommand.c_str(), 
                     ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                done = true;
            }
        }
        step++;
        espCommand = "";
    }
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::LFESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::RFESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false; 
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::RFESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            espCommand = "RFESPInit";
            break;
        case ESP_PUMP_ON:
            // NOP
            break;
        case ESP_PRIMARY_ON:
            espCommand = "ESPPrimaryValve2On";
            break;
        case ESP_START:
            m_ESPIndex[RFWHEEL].buildStart = TagArray("RFESPBuildStart");
            espCommand = "RFESPStart";
            break;
        case ESP_PRIMARY_OFF:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RFWHEEL].buildEnd = TagArray("RFESPBuildEnd");
            espCommand = "ESPPrimaryValve2Off";
            break;
        case ESP_END:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RFWHEEL].reductionStart = TagArray("RFESPReductionStart");
            else m_ESPIndex[RFWHEEL].buildEnd = TagArray("RFESPBuildEnd");
            espCommand = "RFESPEnd";
            break;
        case ESP_FINALIZE:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RFWHEEL].reductionEnd = TagArray("RFESPReductionEnd");
            else m_ESPIndex[RFWHEEL].reductionStart = TagArray("RFESPReductionStart");
            // 2005.02.28 ews added at HMMA emergency request
            espCommand = "LRESPFinalize";
            break;
        case ESP_DONE:
            if(GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RFWHEEL].reductionEnd = TagArray("RFESPReductionEnd");
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {
            moduleStatus = m_vehicleModule.GetInfo(espCommand);
            if(espCommand == "RFESPEnd" && GetParameterBool("CheckResponseInProgress"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && espCommand != "RFESPEnd")
            {//New Bosch module does not send busy response
                WaitForControlComplete();
                /*if(espCommand  == "RFESPEnd")
                {
                    delay(GetParameterInt("RFReductionESPPulse"));
                }
                else if(espCommand == "RFESPStart")
                {
                    delay(GetParameterInt("RFRecoveryESPPulse"));
                }*/
            }

            // Check if we sent the command successfully
            if(moduleStatus != BEP_STATUS_SUCCESS)
            {
                SetCommunicationFailure(true);
                Log( LOG_ERRORS, "Error sending %s - status: %s\n", espCommand.c_str(), 
                     ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                done = true;
            }
        }
        step++;
        espCommand = "";
    }
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::RFESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::LRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::LRESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            // NOP
            break;
        case ESP_PUMP_ON:
            // NOP
            break;
        case ESP_PRIMARY_ON:
            espCommand = "ESPPrimaryValve2On";
            break;
        case ESP_START:
            m_ESPIndex[LRWHEEL].buildStart = TagArray("LRESPBuildStart");
            espCommand = "LRESPStart";
            break;
        case ESP_PRIMARY_OFF:
            m_ESPIndex[LRWHEEL].buildEnd = TagArray("LRESPBuildEnd");
            espCommand = "ESPPrimaryValve2Off";
            break;
        case ESP_END:
            m_ESPIndex[LRWHEEL].reductionStart = TagArray("LRESPReductionStart");
            espCommand = "LRESPEnd";
            break;
        case ESP_FINALIZE:
            m_ESPIndex[LRWHEEL].reductionEnd = TagArray("LRESPReductionEnd");
            espCommand = "LRESPFinalize";
            break;
        case ESP_DONE:
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {
            moduleStatus = m_vehicleModule.GetInfo(espCommand);
            if(espCommand == "LRESPEnd" && GetParameterBool("CheckResponseInProgress"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && espCommand != "LRESPEnd")
            {//New Bosch module does not send busy response
                WaitForControlComplete();
                /*if(espCommand  == "LRESPEnd")
                {
                    delay(GetParameterInt("LRReductionESPPulse"));
                }
                else if(espCommand == "LRESPStart")
                {
                    delay(GetParameterInt("LRRecoveryESPPulse"));
                }*/
            }

            // Check if we sent the command successfully
            if(moduleStatus != BEP_STATUS_SUCCESS)
            {
                SetCommunicationFailure(true);
                Log( LOG_ERRORS, "Error sending %s - status: %s\n", espCommand.c_str(), 
                     ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                done = true;
            }
            // Clear the previous command
            espCommand = "";
        }
        step++;
    }
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::LRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::RRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::RRESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            espCommand = "RRESPInit";
            break;
        case ESP_PUMP_ON:
            // NOP
            break;
        case ESP_PRIMARY_ON:
            espCommand = "ESPPrimaryValve1On";
            break;
        case ESP_START:
            m_ESPIndex[RRWHEEL].buildStart = TagArray("RRESPBuildStart");
            espCommand = "RRESPStart";
            break;
        case ESP_PRIMARY_OFF:
            m_ESPIndex[RRWHEEL].buildEnd = TagArray("RRESPBuildEnd");
            espCommand = "ESPPrimaryValve1Off";
            break;
        case ESP_END:
            m_ESPIndex[RRWHEEL].reductionStart = TagArray("RRESPReductionStart");
            espCommand = "RRESPEnd";
            break;
        case ESP_FINALIZE:
            m_ESPIndex[RRWHEEL].reductionEnd = TagArray("RRESPReductionEnd");
            espCommand = "RRESPFinalize";
            break;
        case ESP_DONE:
            /** 
             * 11/11/2004 BRM
             * Removed this and added a StopPumpMotor test step after the ESP test
             */
            //				espCommand = "ESPPumpOff";
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {
            moduleStatus = m_vehicleModule.GetInfo(espCommand);
            if(espCommand == "RRESPEnd" && GetParameterBool("CheckResponseInProgress"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && espCommand != "RRESPEnd")
            {//New Bosch module does not send busy response
                WaitForControlComplete();
                /*if(espCommand  == "RRESPEnd")
                {
                    delay(GetParameterInt("RRReductionESPPulse"));
                }
                else if(espCommand == "RRESPStart")
                {
                    delay(GetParameterInt("RRRecoveryESPPulse"));
                }*/
            }

            // Check if we sent the command successfully
            if(moduleStatus != BEP_STATUS_SUCCESS)
            {
                SetCommunicationFailure(true);
                Log( LOG_ERRORS, "Error sending %s - status: %s\n", espCommand.c_str(), 
                     ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                done = true;
            }
        }
        step++;
        espCommand = "";
    }
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::RRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::LFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LFWHEEL;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::LFABSTest()\n");

    // Initialize the reduction speed delta values
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RRWHEEL] = 0.0;

    // Initialize the recovery speed delta values
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RRWHEEL] = 0.0;

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("LFABSReductionStart");

    if(!GetParameterBool("CheckResponseInProgress"))
    {
        moduleStatus = m_vehicleModule.GetInfo("LFABSReduction", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    else
    {
        moduleStatus = m_vehicleModule.CommandModule("LFABSReduction");
        testResult = WaitForControlComplete();
        if(testResult == testPass)
        {
            moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }

    }

    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"LF ABS reduction OK\n");
        Log(LOG_DEV_DATA, "Reduction Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RRWHEEL]);

        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("LFABSReductionEnd");
        m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("LFABSRecoveryStart");
        // Try to start the LF ABS recovery
        if(!GetParameterBool("CheckResponseInProgress"))
        {
            moduleStatus = m_vehicleModule.GetInfo("LFABSRecovery", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }
        else
        {
            moduleStatus = m_vehicleModule.CommandModule("LFABSRecovery");
            testResult = WaitForControlComplete();
            if(testResult == testPass)
            {
                moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            }

        }

        Log(LOG_DEV_DATA, "Recovery Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RRWHEEL]);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            inletResult = testPass;
            Log(LOG_DEV_DATA,"LF ABS recovery OK\n");
        }
        else
        {
            inletResult = testFail;
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error LF ABS recovery - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    else
    {
        outletResult = testFail;
        SetCommunicationFailure(true);
        Log(LOG_ERRORS, "Error LF ABS reduction - status: %s\n", 
            ConvertStatusToResponse(moduleStatus).c_str());
    }
    m_reduxRecovIndex[wheelIdx].recoveryEnd = TagArray("LFABSRecoveryEnd");

    SendSubtestResult(rollerName[wheelIdx] + "InletValveActuation", inletResult, 
                      rollerName[wheelIdx] + "InletValveActuation", "0000");
    SendSubtestResult(rollerName[wheelIdx] + "OutletValveActuation", outletResult, 
                      rollerName[wheelIdx] + "OutletValveActuation", "0000");

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::LFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::RFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RFWHEEL;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::RFABSTest()\n");

    // Initialize the reduction speed delta values
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RRWHEEL] = 0.0;

    // Initialize the recovery speed delta values
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RRWHEEL] = 0.0;

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("RFABSReductionStart");
    // Try to start the RF ABS reduction

    if(!GetParameterBool("CheckResponseInProgress"))
    {
        moduleStatus = m_vehicleModule.GetInfo("RFABSReduction", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    else
    {
        moduleStatus = m_vehicleModule.CommandModule("RFABSReduction");
        testResult = WaitForControlComplete();
        if(testResult == testPass)
        {
            moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }

    }

    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"RF ABS reduction OK\n");
        Log(LOG_DEV_DATA, "Reduction Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RRWHEEL]);

        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("RFABSReductionEnd");
        m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("RFABSRecoveryStart");
        // Try to start the RF ABS recovery
        if(!GetParameterBool("CheckResponseInProgress"))
        {
            moduleStatus = m_vehicleModule.GetInfo("RFABSRecovery", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }
        else
        {
            moduleStatus = m_vehicleModule.CommandModule("RFABSRecovery");
            testResult = WaitForControlComplete();
            if(testResult == testPass)
            {
                moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            }

        }
        Log(LOG_DEV_DATA, "Recovery Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RRWHEEL]);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            inletResult = testPass;
            Log(LOG_DEV_DATA,"RF ABS recovery OK\n");
        }
        else
        {
            inletResult = testFail;
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error RF ABS recovery - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    else
    {
        outletResult = testFail;
        SetCommunicationFailure(true);
        Log(LOG_ERRORS, "Error RF ABS reduction - status: %s\n", 
            ConvertStatusToResponse(moduleStatus).c_str());
    }
    m_reduxRecovIndex[wheelIdx].recoveryEnd = TagArray("RFABSRecoveryEnd");

    SendSubtestResult(rollerName[wheelIdx] + "InletValveActuation", inletResult, 
                      rollerName[wheelIdx] + "InletValveActuation", "0000");
    SendSubtestResult(rollerName[wheelIdx] + "OutletValveActuation", outletResult, 
                      rollerName[wheelIdx] + "OutletValveActuation", "0000");

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::RFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::LRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LRWHEEL;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::LRABSTest()\n");

    // Initialize the reduction speed delta values
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RRWHEEL] = 0.0;

    // Initialize the recovery speed delta values
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RRWHEEL] = 0.0;

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("LRABSReductionStart");
    // Try to start the LR ABS reduction
    if(!GetParameterBool("CheckResponseInProgress"))
    {
        moduleStatus = m_vehicleModule.GetInfo("LRABSReduction", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    else
    {
        moduleStatus = m_vehicleModule.CommandModule("LRABSReduction");
        testResult = WaitForControlComplete();
        if(testResult == testPass)
        {
            moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }

    }

    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"LR ABS reduction OK\n");
        Log(LOG_DEV_DATA, "Reduction Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RRWHEEL]);

        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("LRABSReductionEnd");
        m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("LRABSRecoveryStart");

        // Try to start the LR ABS recovery
        if(!GetParameterBool("CheckResponseInProgress"))
        {
            moduleStatus = m_vehicleModule.GetInfo("LRABSRecovery", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }
        else
        {
            moduleStatus = m_vehicleModule.CommandModule("LRABSRecovery");
            testResult = WaitForControlComplete();
            if(testResult == testPass)
            {
                moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            }

        }
        Log(LOG_DEV_DATA, "Recovery Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RRWHEEL]);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            inletResult = testPass;
            Log(LOG_DEV_DATA,"LR ABS recovery OK\n");
        }
        else
        {
            inletResult = testFail;
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error LR ABS recovery - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    else
    {
        outletResult = testFail;
        SetCommunicationFailure(true);
        Log(LOG_ERRORS, "Error LR ABS reduction - status: %s\n", 
            ConvertStatusToResponse(moduleStatus).c_str());
    }
    m_reduxRecovIndex[wheelIdx].recoveryEnd = TagArray("LRABSRecoveryEnd");

    SendSubtestResult(rollerName[wheelIdx] + "InletValveActuation", inletResult, 
                      rollerName[wheelIdx] + "InletValveActuation", "0000");
    SendSubtestResult(rollerName[wheelIdx] + "OutletValveActuation", outletResult, 
                      rollerName[wheelIdx] + "OutletValveActuation", "0000");


    Log(LOG_FN_ENTRY, "Exit Bosch8TC::LRABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::RRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RRWHEEL;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::RRABSTest()\n");

    // Initialize the reduction speed delta values
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][ RRWHEEL] = 0.0;

    // Initialize the recovery speed delta values
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RFWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ LRWHEEL] = 0.0;
    m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][ RRWHEEL] = 0.0;

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("RRABSReductionStart");
    // Try to start the RR ABS reduction

    if(!GetParameterBool("CheckResponseInProgress"))
    {
        moduleStatus = m_vehicleModule.GetInfo("RRABSReduction", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    }
    else
    {
        moduleStatus = m_vehicleModule.CommandModule("RRABSReduction");
        testResult = WaitForControlComplete();
        if(testResult == testPass)
        {
            moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }

    }

    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"RR ABS reduction OK\n");
        Log(LOG_DEV_DATA, "Reduction Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX][RRWHEEL]);

        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("RRABSReductionEnd");
        m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("RRABSRecoveryStart");
        // Try to start the RR ABS recovery
        if(!GetParameterBool("CheckResponseInProgress"))
        {
            moduleStatus = m_vehicleModule.GetInfo("RRABSRecovery", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
            // Determine the test result
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        }
        else
        {
            moduleStatus = m_vehicleModule.CommandModule("RRABSRecovery");
            testResult = WaitForControlComplete();
            if(testResult == testPass)
            {
                moduleStatus = m_vehicleModule.ABSValveActuation("ReadWheelSpeedValues", m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX]);
                // Determine the test result
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            }

        }
        Log(LOG_DEV_DATA, "Recovery Speed Deltas: %.2f %.2f %.2f %.2f\n", 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RFWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][LRWHEEL], 
            m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX][RRWHEEL]);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            inletResult = testPass;
            Log(LOG_DEV_DATA,"RR ABS recovery OK\n");
        }
        else
        {
            inletResult = testFail;
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error RR ABS recovery - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }
    }
    else
    {
        outletResult = testFail;
        SetCommunicationFailure(true);
        Log(LOG_ERRORS, "Error RR ABS reduction - status: %s\n", 
            ConvertStatusToResponse(moduleStatus).c_str());
    }
    m_reduxRecovIndex[wheelIdx].recoveryEnd = TagArray("RRABSRecoveryEnd");

    SendSubtestResult(rollerName[wheelIdx] + "InletValveActuation", inletResult, 
                      rollerName[wheelIdx] + "InletValveActuation", "0000");
    SendSubtestResult(rollerName[wheelIdx] + "OutletValveActuation", outletResult, 
                      rollerName[wheelIdx] + "OutletValveActuation", "0000");

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::RRABsTest()\n");
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::EvaluateSensorCross(void)
{
    string testResult(testPass);
    const string testDescription( GetTestStepInfo("Description"));
    string testResultCode( "0000");
    string faultTag("");
    int ii, wheelIdx;
    string frontCross(testPass), rearCross(testPass), frontRearCross( testPass);
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::EvaluateSensorCross()\n");

    /**
     * Loop through LF, RF and LR (don't need RR in outer loop because
     * it will be covered by comparing t he first 3 wheels):
     * 		LFWHEEL: LF/RF, LF/LR, LF/RR
     * 		RFWHEEL: RF/LR, RF/RR
     * 		LRWHEEL: LR/RR
     */
    for(wheelIdx=LFWHEEL; wheelIdx<=RRWHEEL; wheelIdx++)
    {
        // Use the speed delta from the recovery cycle
        WheelSpeeds_t   &redDeltas = m_absSpeedDeltas[ wheelIdx][RED_DELTA_IDX];
        WheelSpeeds_t   &recDeltas = m_absSpeedDeltas[ wheelIdx][REC_DELTA_IDX];

        Log(LOG_DEV_DATA, "Reduction Speed Deltas %d: %.2f %.2f %.2f %.2f\n", wheelIdx,
            redDeltas[LFWHEEL], redDeltas[RFWHEEL], 
            redDeltas[LRWHEEL], redDeltas[RRWHEEL]);
        Log(LOG_DEV_DATA, "  Initial Speed Deltas %d: %.2f %.2f %.2f %.2f\n", wheelIdx,
            m_initSpeedDelta[LFWHEEL], m_initSpeedDelta[RFWHEEL], 
            m_initSpeedDelta[LRWHEEL], m_initSpeedDelta[RRWHEEL]);
        Log(LOG_DEV_DATA, " Recovery Speed Deltas %d: %.2f %.2f %.2f %.2f\n", wheelIdx,
            recDeltas[LFWHEEL], recDeltas[RFWHEEL], 
            recDeltas[LRWHEEL], recDeltas[RRWHEEL]);

        // If the reduction speed delta is greater than or equal to the init delta OR
        if((redDeltas[wheelIdx] >= m_initSpeedDelta[wheelIdx]) ||
           // if the recovery speed delta is less than or equal to the reduction delta
           (recDeltas[wheelIdx] <= redDeltas[wheelIdx]))
        /*  
        was:
        // if the recovery speed delta is less than or equal to the init delta
        (recDeltas[wheelIdx] <= m_initSpeedDelta[wheelIdx]) )
        */
        {
            testResult = testFail;
            /**
             * Look for the other wheel that this wheel is crossed with:
             * Loop through RF, LR and RR (don't need RR in outer loop because
             * it will be covered by comparing t he first 3 wheels):
             * 		wheelIdx |   ii
             * 		=========|==========================
             * 		LFWHEEL  | RFWHEEL, LRWHEEL, RRWHEEL
             * 		RFWHEEL  | LRWHEEL, RRWHEEL
             * 		LRWHEEL  | RRWHEEL
             */
            for(ii=wheelIdx+1; ii<=RRWHEEL; ii++)
            {
                // If the reduction speed delta is less than the init delta AND
                if((redDeltas[ii] > m_initSpeedDelta[ii]) &&
                   // if the recovery speed delta is greater than the init delta
                   (recDeltas[ii] < redDeltas[ii]))
                /*  
                was:
                // if the recovery speed delta is greater than the init delta
                   (recDeltas[ii] < m_initSpeedDelta[ii]))
                */
                {
                    faultTag = rollerName[wheelIdx] + std::string("CrossedWith") + rollerName[ii];
                    Log( LOG_ERRORS, "Sensor cross: %s to %s\n", rollerName[wheelIdx].c_str(),rollerName[ii].c_str());
                    // If the front sensors are crossed
                    if((wheelIdx < LRWHEEL) && (ii < LRWHEEL))
                    {
                        frontCross = testFail;
                    }
                    // If the rear sensors are crossed
                    else if((wheelIdx >= LRWHEEL) && (ii >= LRWHEEL))
                    {
                        rearCross = testFail;
                    }
                    // Must be front crossed with rear
                    else
                    {
                        frontRearCross = testFail;
                    }

                    break;
                }
            }

            // If we could not find the wheel that we are crossed with
            if(faultTag.empty() == true)
            {
                Log( LOG_ERRORS, "Unable to determine sensor cross; sending generic sensor test failure\n");
                faultTag = std::string("HighSpeedSensorTestFail");
                SendSubtestResult(faultTag, testResult,
                                            rollerName[wheelIdx] + GetFaultDescription(faultTag),
                                            GetFaultCode(faultTag));
                /*  This just adds confusion
                if(wheelIdx < LRWHEEL)
                {
                    frontCross = testFail;
                }
                else
                {
                    rearCross = testFail;
                }
                */
            }     
            faultTag.clear();
        }
    }
    if((rearCross == testFail) || (frontCross == testFail))
    {
        // Send the Sensor Cross Results
        SendSubtestResult("FrontSensorCross", frontCross, "Front ABS sensors crossed", "0000");
        SendSubtestResult("RearSensorCross", rearCross, "Rear ABS sensors crossed", "0000");
        SendSubtestResult("FrontToRearSensorCross", frontRearCross, "Front ABS sensors crossed with rears", "0000");
    }

    SendTestResult(testResult, testDescription, testResultCode);

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::EvaluateSensorCross(), status=%s\n", testResult.c_str());

    return( testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::StopPumpMotor(void)
{
    string testResult;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    bool havePrompts = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::StopPumpMotor()\n");

    // Determine if the test should be performed
    if(!ShortCircuitTestStep())
    {
        if((GetTestStepInfo("Prompt1").size()) || (GetTestStepInfo("Prompt2").size()))
        {
            havePrompts = true;
            UpdatePrompts();
        }

        testResult = KoreaAbsTcTemplate<ModuleType>::StopPumpMotor();
        //if(GetParameterBool("CheckResponseInProgress") && testResult == testPass)
        //{
        //    testResult = WaitForControlComplete();
        //    testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
        //    testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
        //    // Send the test result
        //    SendTestResult(testResult, testDescription, testResultCode);
        //}
        if(havePrompts)
        {
            RemovePrompts();
        }
    }
    else
    {  // Skip the test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::StopPumpMotor(), status=%s\n", testResult.c_str());

    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::BrakeSwitchTest(const string& position)
{
    string testResult;
    bool havePrompts = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::BrakeSwitchTest(%s)\n", position.c_str());

    if(!ShortCircuitTestStep() && (testPass != GetTestStepResult()) && (testSkip != GetTestStepResult()))
    {
        if((GetTestStepInfo("Prompt1").size()) || (GetTestStepInfo("Prompt2").size()))
        {
            havePrompts = true;
            UpdatePrompts();
        }

        testResult = KoreaAbsTcTemplate<ModuleType>::BrakeSwitchTest(position);
        if(testPass == testResult)
        {
            BposSleep(GetTestStepInfoInt("PostDelayTime"));
        }

        if(havePrompts)
        {
            RemovePrompts();
        }
    }
    else
    {
        // Do not need to perform this step
        Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::BrakeSwitchTest(%s), status=%s\n", position.c_str(), testResult.c_str());
    return(testResult);
}


template <class ModuleType>
string Bosch8TC<ModuleType>::WaitForControlComplete()
{
    string testResult;
    bool isControlComplete = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter Bosch8TC::WaitForControlComplete()\n");


    // Wait for timeout or control complete
    while((TimeRemaining()) && (isControlComplete == false) && (BEP_STATUS_SUCCESS == StatusCheck()))
    {
        m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
    }
    if(isControlComplete)
    {
        testResult = testPass;
    }
    else
    {
        testResult = testFail;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "Exit Bosch8TC::WaitForControlComplete(), isControlComplete:%s status=%s\n", 
        isControlComplete ? "true" : "false", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string Bosch8TC<ModuleType>::AllInletValvesOn(void)
{
    string testResult            = BEP_TESTING_STATUS;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // call base class
    testResult = GenericABSTCTemplate<ModuleType>::AllInletValvesOn();
    if(GetParameterBool("CheckResponseInProgress") && testResult == testPass)
    {
        testResult = WaitForControlComplete();
        testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
        testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

template <class ModuleType>
string Bosch8TC<ModuleType>::AllInletValvesOff(void)
{
    string testResult            = BEP_TESTING_STATUS;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // call base class
    testResult = GenericABSTCTemplate<ModuleType>::AllInletValvesOff();
    if(GetParameterBool("CheckResponseInProgress") && testResult == testPass)
    {
        testResult = WaitForControlComplete();
        testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
        testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

template <class ModuleType>
string Bosch8TC<ModuleType>::RunPumpMotor(void)
{
    string testResult            = BEP_TESTING_STATUS;
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // call base class
    testResult = GenericABSTCTemplate<ModuleType>::RunPumpMotor();
    if(GetParameterBool("CheckResponseInProgress") && testResult == testPass)
    {
        testResult = WaitForControlComplete();
        testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
        testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}
//=============================================================================
template <class ModuleType>
const string Bosch8TC<ModuleType>::PerformPBTorqueTest(const std::string &direction)
{
    string  status;         // the current test status
    BEP_STATUS_TYPE  brakePressTestResult = BEP_STATUS_SUCCESS;         // the current test status
    string switchPosition = "Unknown";
    const int rollerCount = GetRollerCount();
    float   torqueVal = 
    ((GetParameterFloat("ParkBrakeApplyForce") != 0) ? GetParameterFloat("ParkBrakeApplyForce") : 350) * 0.5;
    float   speedVal = (GetParameterFloat("ParkBrakeSpeedValue") != 0) ? GetParameterFloat("ParkBrakeSpeedValue") : 5;
    string  frontMotorMode, rearMotorMode;
    string  lfMotorValTag, rfMotorValTag;
    string  lrMotorValTag, rrMotorValTag;
    string  faultCode("0000");;
    float   wheelSpeeds[rollerCount];
    char    temp[256];
    int     testTime = GetTestStepInfoInt("TestTime");
    INT32   statusCheck;
    float   torqueValLeft = 
    ((GetParameterFloat("ParkBrakeApplyForceRight") != 0) ? GetParameterFloat("ParkBrakeApplyForceRight") : 350) * 0.5;
    float   torqueValRight = 
    ((GetParameterFloat("ParkBrakeApplyForceLeft") != 0) ? GetParameterFloat("ParkBrakeApplyForceLeft") : 350) * 0.5;

    Log(LOG_FN_ENTRY, "Enter Bosch8TC::PerformPBTorqueTest(%s)\n", direction.c_str());

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

    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueVal > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueVal < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueVal, -torqueVal);
        torqueVal *= -1.0;
    }
    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueValLeft > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueValLeft < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueValLeft, -torqueValLeft);
        torqueValLeft *= -1.0;
    }
    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueValRight > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueValRight < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueValRight, -torqueValRight);
        torqueValRight *= -1.0;
    }

    // If we want the wheels to turn forward, we need a positive speed
    if( ((direction == "Forward") && (speedVal < 0)) ||
        // If we want the wheels to turn in reverse, we need a negative speed
        ((direction == "Reverse") && (speedVal > 0)) )
    {
        Log( LOG_DEV_DATA, "Changing speed value from %.02f to %.02f\n", speedVal, -speedVal);
        speedVal *= -1.0;
    }

    // read the current distance of the wheels
    GetWheelDistances( m_parkBrakeDistances[ 0]);

    // Tell the motor to turn for the park brake test
    Log( LOG_DEV_DATA, "Commanding Front Motors to %.2f MPH and Rear Motors %.2f LBS\n",
         speedVal, torqueVal);

    m_MotorController.Write("LeftFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("RightFrontMotorMode", SPEED_MODE, false);
    m_MotorController.Write("LeftRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("RightRearMotorMode", TORQUE_MODE, false);
    m_MotorController.Write("LeftFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    m_MotorController.Write("RightFrontSpeedValue", CreateMessage(temp, 256, "%.2f", speedVal), false);
    if(GetParameterBool("ParkBrakeUseLRTorqueValues"))
    {
        m_MotorController.Write("LeftRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueValLeft), false);
        m_MotorController.Write("RightRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueValRight), true);
    }
    else
    {
        m_MotorController.Write("LeftRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), false);
        m_MotorController.Write("RightRearTorqueValue", CreateMessage(temp, 256, "%.2f", torqueVal), true);
    }
    //Check brake pedal status
    if(GetParameterBool("ParkBrakeCheckBrakeSwitch"))
    {
        brakePressTestResult = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), switchPosition);
        if(BEP_STATUS_SUCCESS == brakePressTestResult)
        {   // Good read, determine switch position
            if("On" == switchPosition)
            {   // Switch is on, prompt for Off
                DisplayPrompt(GetPromptBox("RemoveFootFromBrake"), GetPrompt("RemoveFootFromBrake"), GetPromptPriority("RemoveFootFromBrake"));
                do
                {
                    brakePressTestResult = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), switchPosition);
                    Log(LOG_DEV_DATA, "Brake switch is ON. Waiting for OFF.\n");
                    BposSleep(250);
                } while((BEP_STATUS_SUCCESS == brakePressTestResult) && TimeRemaining() && ("On" == switchPosition));
                RemovePrompt(GetPromptBox("RemoveFootFromBrake"),GetPrompt("RemoveFootFromBrake"));
            }
        }
    }
    // wait the specified amount of time
    SetStartTime();
    while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) )
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        if(GetParameterBool("ParkBrakeCheckBrakeSwitch") && (brakePressTestResult == BEP_STATUS_SUCCESS))
        {
            brakePressTestResult = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), switchPosition);
            if(BEP_STATUS_SUCCESS == brakePressTestResult)
            {   // Good read, determine switch position
                if("On" == switchPosition)
                {   // Switch is on fail test
                    brakePressTestResult = BEP_STATUS_FAILURE;
                }
            }
        }
        BposSleep(100);     // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }

    // Make sure we clear the speed/torque command values
    m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
    m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
    m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
    m_MotorController.Write("RightRearTorqueValue", "0.00", false);
    m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
    m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
    m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
    m_MotorController.Write("RightRearSpeedValue", "0.00", true);

    // command the drives to zero torque
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);

    // command the drives to zero speed
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");

    // once stopped, check for pass/fail

    // if and error did not occur and not aborted
    statusCheck = GenericTC::StatusCheck();
    if( statusCheck == BEP_STATUS_SUCCESS)
    {
        // read the current distance of the rear wheels
        GetWheelDistances( m_parkBrakeDistances[ 1]);

        // Analyze the distances that the wheels travelled
        status = ValidateParkBrakeTestDistances(m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1]);
    }
    else
    {
        Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
        status = ConvertStatusToResponse( statusCheck);
        faultCode = GetFaultCode("ParkBrakeFault");
    }
    if(GetParameterBool("ParkBrakeCheckBrakeSwitch") && (brakePressTestResult != BEP_STATUS_SUCCESS))
    {//brake pressed during test
        status = BEP_FAIL_RESPONSE;
    }

    // Send the test result info
    SendTestResult( GetTestStepName(), status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "Exit Bosch8TC::PerformPBTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return( status);
}

template <class ModuleType>
const std::string Bosch8TC<ModuleType>::TestStepElectricParkBrake(const std::string &value)
{
    Log(LOG_FN_ENTRY, "Bosch8TC::TestStepElectricParkBrake(%s)\n", value.c_str());
    std::string status = BEP_FAIL_RESPONSE;     // the overall test status
    std::string operatorRetestResponse = BEP_FAIL_RESPONSE;     // pb response
    int retry = GetParameterInt("ElectricParkBrakeAttempts");
    do
    {
        // call the machine component HydraulicParkBrake
        status = GenericTC::TestStepElectricParkBrake(value);

        // update the test result box
        if(status == BEP_PASS_RESPONSE)
        {
            SystemWrite(GetDataTag("TestResultBox4"), colorGreen);
        }
        else
        {
            SystemWrite(GetDataTag("TestResultBox4"), colorRed);
            if(GetParameterBool("CheckParkBrakeRetest") && (retry))
            {//add logic to try retest with button press
                operatorRetestResponse = OperatorYesNo("ParkBrakeRetest",GetParameterInt("ParkBrakeRetestPromptTimeout"));
            }
        }
    }while(retry-- && (status != BEP_PASS_RESPONSE) && 
           (operatorRetestResponse == BEP_PASS_RESPONSE));
    Log(LOG_FN_ENTRY, "Bosch8TC::TestStepElectricParkBrake(%s) done\n", status.c_str());

    return(status);     // return the status
} 
