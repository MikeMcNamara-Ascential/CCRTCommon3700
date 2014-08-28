//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/Bosch8TC.cpp 3     5/23/06 11:28a Gswope $
//
// FILE DESCRIPTION:
//      ABS test component for the Bosch8 system.
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
#include "BoschABSTC.h"

template <class ModuleType>
BoschABSTC<ModuleType>::BoschABSTC() : KoreaAbsTcTemplate<ModuleType>()
{
    m_ESPIndex = new BuildReductionIndex[RRWHEEL+1];
    m_isFourChannelSystem = true;
    m_lfWssPass = false;
    m_rfWssPass = false;
    m_lrWssPass = false;
    m_rrWssPass = false;
}

template <class ModuleType>
BoschABSTC<ModuleType>::~BoschABSTC()
{
    if(m_ESPIndex != NULL)
    {
        delete[] m_ESPIndex;
        m_ESPIndex = NULL;
    }
}

template <class ModuleType>
const string BoschABSTC<ModuleType>::BoschABSTC<ModuleType>::CommandTestStep(const string &value)
{
    string status;
    try
    {
        string step = GetTestStepName(); // Get the name of the test step
        Log(LOG_DEV_DATA, "BoschABSTC::CommandTestStep(%s): Entering test step %s\n", 
            value.c_str(),step.c_str());

        // make sure it is OK to test
        if(StatusCheck() != BEP_STATUS_SUCCESS)
        {
            status = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS,"BoschABSTC::CommandTestStep: StatusCheck() failed: %s\n",
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
        else if(step == "CycleIgnition") status = CycleIgnition();
        else if(step == "SensorQualityTest") status = SensorQualityTest();
        else if(step == "ESPValveFiringTest") status = ESPValveFiringTest();
        else if(step == "ABSValveFiringTest") status = ABSValveFiringTest();
        else if(step == "CMABSValveFiringTest") status = CMABSValveFiringTest();
        else if(step == "DisableSpeedLimit") status = DisableSpeedLimit();
        else if(step == "DisableValveRelayShutdown") status = DisableValveRelayShutdown();
        else if(step == "EnableSpeedLimit") status = EnableSpeedLimit();
        else if(step == "EnableValveRelayShutdown")  status = EnableValveRelayShutdown();
        else if(step == "CheckUplineProcessByte") status = CheckUplineProcessByte();
        else if(step == "CheckEolByte") status = CheckEolByte();
        else if(step == "ReadSensorSpeeds") status = ReadSensorSpeeds();
        else if(step == "IgnitionOff") status = IgnitionOff();
        else if(step == "IgnitionOn") status = IgnitionOn();
        else if(step == "EvaluateSensorCross") status = EvaluateSensorCross();
        else if(step == "ReadSpeedDeltas") status = ReadSpeedDeltas();
        else if(step == "StopESPPumpMotor") status = StopPumpMotor();
        else if(step == "WarningLight") status = WarningLight();
        else if(step == "StartAbsTest")  status = StartAbsTest();
        else if(step == "CheckModuleType") status = CheckModuleType();
        else if(step == "ProgramVIN") status = ProgramVIN();
        else if(step == "ResetVIN") status = ResetVIN();
        else if(step == "RunPumpMotor")
        {
            status = RunPumpMotor();
            // Wait a bit before sending the next command
            BposSleep(GetParameterInt("LeftFrontReductionPulseLength"));
        }
        else if(step == "AllInletValvesOn")
        {
            status = AllInletValvesOn();
            // Wait a bit before sending the next command
            BposSleep(GetParameterInt("LeftFrontReductionPulseLength"));
        }
         // Command the module back to normal mode
        else if(!GetTestStepName().compare("EnterNormalMode"))           status = EnterNormalMode();
        // Disable the force meter
        else if(!GetTestStepName().compare("DisableForceMeter"))         status = DisableForceMeter();
        // Perform the sensor and burnish test
        else if(!GetTestStepName().compare("SensorTest"))                status = IndividualSensorTest();
        // Perform the brake burnish cycle
        else if(!GetTestStepName().compare("BrakeBurnishCycle"))         status = BrakeBurnishCycle();
        else if(!GetTestStepName().compare("StaticBrakeBurnishCycle"))   status = StaticBrakeBurnishCycle();
        else if(!GetTestStepName().compare("ElectricParkBrakeBurnishCycle"))   status = ElectricParkBrakeBurnishCycle();
        else if(!GetTestStepName().compare("AccelerateToBrakeSpeed"))    status = AccelerateToBrakeSpeed();
        else if(!GetTestStepName().compare("EnableElectricVacuumPump"))   status = EnableElectricVacuumPump();
        else if(!GetTestStepName().compare("DisableElectricVacuumPump"))   status = DisableElectricVacuumPump();
        else if(!GetTestStepName().compare("LearnPerformanceType"))      status = LearnPerformanceType();
        else if(!GetTestStepName().compare("Delay"))                     status = TestStepDelay();
        // Call the base class to handle the test step
        else status = KoreaAbsTcTemplate<ModuleType>::CommandTestStep(value);
    }
    catch(BepException &err)
    {
        Log("BoschABS::CommandTestStep %s BepException: %s\n",
            GetTestStepName().c_str(),err.what());
        status = BEP_ERROR_RESPONSE;
    }
    Log(LOG_DEV_DATA,"Bosch8TC::CommandTestStep(%s) returning %s\n",
        value.c_str(),status.c_str());

    return(status);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::EnterDiagnosticMode(void)
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;

    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter EnterDiagnosticMode::EnterDiagnosticMode()\n");

    if(CheckZeroSpeed())
    {
        UpdatePrompts();

        // Lety the driver react to the prompts
        //BposSleep( 10000);

        if(!GetParameter("Port").compare("ISOK"))
        {
            testResult = KoreaAbsTcTemplate<ModuleType>::EnterDiagnosticMode();
        }
        else
        {
            testResult = m_vehicleModule.CommandModule("EnterDiagnosticMode") == BEP_STATUS_SUCCESS ? testPass : testFail;
        }
        RemovePrompts();
    }
    else
    {
        Log( LOG_ERRORS, "Failed to enter zerospeed in BoschABSTC::EnterDiagnosticMode()\n");
        testResult = BEP_FAIL_STATUS;
    }

    Log(LOG_FN_ENTRY, "Exit EnterDiagnosticMode::EnterDiagnosticMode()\n");

    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckRelayState(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    vector<bool> relays;
    bool motorRelay;
    bool valveRelay;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckRelayState()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckRelayState - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckRelayState()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckSupplyVoltage(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float supplyVoltage;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckSupplyVoltage()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::SupplyVoltage - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::SupplyVoltage()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckPumpMotor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool pumpState;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckPumpMotor()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckPumpMotor - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckPumpMotor()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckParkBrakeSignal(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool parkBrakeSignal;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckParkBrakeSignal()\n");
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
        if(!GetParameterBool("PBTestAfterCheckPBSignal"))
        {
            DisplayPrompt(1,"ReleaseParkingBrake");
            BposSleep(2000);
            RemovePrompt(1,"ReleaseParkingBrake");
        }

        Log(LOG_DEV_DATA, "Read Park Brake Signal: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckParkBrakeSignal - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckParkBrakeSignal()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckEngineSpeed(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float engineSpeed;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckEngineSpeed()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckEngineSpeed - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckEngineSpeed()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::IgnitionOff()
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float minEngineRpm = GetParameterFloat( "IgnitionOffRpm");
    float engineRpm = minEngineRpm + 1;
    bool sawEngineOff = false;

    // Check if we need to skip this step
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::IgnitionOff()\n");
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
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::IgnitionOff()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::IgnitionOn()
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float minEngineRpm = GetParameterFloat( "IgnitionOnRpm");
    float engineRpm = minEngineRpm + 1;
    bool sawEngineOn = false;

    // Check if we need to skip this step
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::IgnitionOn()\n");
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

        DisplayPrompt(1,"Initialize");
        BposSleep(GetParameterInt("IgnitionOnStartupTime"));
        RemovePrompt(1,"Initialize");

        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {  // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::IgnitionOn()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckThrottlePosition(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float throttlePosition;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckThrottlePosition()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckThrottlePosition - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckThrottlePosition()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckShiftLeverPosition(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT16 shiftLeverPosition;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckShiftLeverPosition()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckShiftLeverPosition - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckShiftLeverPosition()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckPassiveSwitch(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool passiveSwitchState;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckPassiveSwitch()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckPassiveSwitch - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckPassiveSwitch()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckSteeringWheelAngle(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float steeringWheelAngle;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckSteeringWheelAngle()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckSteeringWheelAngle - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckSteeringWheelAngle()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckLateralAcceleration(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float lateralAcceleration;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckLateralAcceleration()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckLateralAcceleration - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckLateralAcceleration()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckYawRate(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float yawRate;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckYawRate()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckYawRate - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckYawRate()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckPressureSensor(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float pressure;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckPressureSensor()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckPressureSensor - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckPressureSensor()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckAYSensorStatus(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool sensorStatus;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckAYSensorStatus()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckAYSensorStatus() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckAYSensorStatus()\n");
    return(testResult);
}

//---------------------------------------------------------------------------------------------------------------------
template <class ModuleType>
string BoschABSTC<ModuleType>::CycleIgnition(void)
{
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "BoschABSTC::CycleIgnition() - Enter");
    if(!ShortCircuitTestStep())
    {   // Wait for the engine to be off
        result = WaitForEngineOffIgnitionOff();
        if(!result.compare(testPass))
        {   // Ignition is off, wait for ignition on
            result = CheckIgnitionOn() ? testPass : testFail;
        }
        else
        {   // Timeout waiting for engine off
            Log(LOG_ERRORS, "Timeout waiting for ignition off");
        }
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"), "0000");
    }
    else
    { 
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Cycle Ignition");
    }
    Log(LOG_FN_ENTRY, "BoschABSTC::CycleIgnition() - Exit");
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
template <class ModuleType>
string BoschABSTC<ModuleType>::DisableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::DisableSpeedLimit()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::DisableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::DisableSpeedLimit()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::DisableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::DisableValveRelayShutdown()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::DisableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::DisableValveRelayShutdown()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::ReadSpeedDeltas(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log( LOG_FN_ENTRY, "Enter BoschABSTC::ReadSpeedDeltas()\n");

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

    Log( LOG_FN_ENTRY, "Exit BoschABSTC::ReadSpeedDeltas(), status=%s\n", testResult.c_str());

    return( testResult);
}

#if 0
template <class ModuleType>
string BoschABSTC<ModuleType>::SensorQualityTest(void)
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

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::SensorQualityTest()\n");

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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::SensorQualityTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Declutch the machine so the driver can stop the vehicle
    Log(LOG_DEV_DATA, "Declutching\n");
    Disengage();

    //MAM 4/16/09 - engage again after vehicle stopped
    if(CheckZeroSpeed())
    {
        Log( LOG_DEV_DATA, "Calling Engage() BoschABSTC::SensorQualityTest()\n");
        Engage();
    }
    else
    {
        Log( LOG_DEV_DATA, "Failed to stop in BoschABSTC::SensorQualityTest()\n");        
    }

    RemovePrompts();

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::SensorQualityTest()\n");
    return(testResult);
}
#else
template <class ModuleType>
string BoschABSTC<ModuleType>::SensorQualityTest(void)
{   // Log the entry and determine if this test needs to be performed
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "BoschABSTC::SensorQualityTest() - Enter");
    if(!ShortCircuitTestStep())
    {   
        string description(GetTestStepInfo("Description"));
        string code("0000");
        vector<string> results;
        results.reserve(GetRollerCount());
        results.resize(GetRollerCount());
        for(UINT8 index = 0; index < GetRollerCount(); index++) results[index] = testSkip;

        if(GetParameterBool("SensorSpeedShiftPrompt"))
        {
            DisplayPrompt(GetPromptBox("ShiftToDrive"),GetPrompt("ShiftToDrive"),GetPromptPriority("ShiftToDrive"));
            BposSleep(GetParameterInt("SensorSpeedShiftPromptDelay"));
            RemovePrompt(GetPromptBox("ShiftToDrive"),GetPrompt("ShiftToDrive"),GetPromptPriority("ShiftToDrive"));
        }
        AccelerateToTestSpeed(GetParameterInt("SensorQualityTestTargetSpeed"), GetParameter("SensorQualityTestSpeedRange"), GetParameterInt("SensorQualityTestScanDelay"),false);
        // Wait a bit for the rollers to start accelerating
        BposSleep(GetParameterInt("SensorSpeedDriverDelay"));

        // Check the results for each wheel
        float rollerSpeeds[GetRollerCount()];
        GetWheelSpeeds(rollerSpeeds);
        WheelSpeeds_t sensorSpeeds;
        BEP_STATUS_TYPE status = m_vehicleModule.GetInfo("ReadSensorSpeeds", sensorSpeeds);
        if(status == BEP_STATUS_SUCCESS)
        {   // Check each wheel against the allowable tolerance
            for(UINT8 index = LFWHEEL; index <= RRWHEEL; index++)
            {
                float upperLimit = rollerSpeeds[index] + ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollerSpeeds[index]);
                float lowerLimit = rollerSpeeds[index] - ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollerSpeeds[index]);
                results[index] = ((sensorSpeeds[index] < upperLimit) && 
                                  (sensorSpeeds[index] > lowerLimit) ? testPass : testFail);
                Log(LOG_DEV_DATA, "Sensor test: %s - sensor speed: %.2f, upper limit: %.2f, lower limit: %.2f - Result: %s",
                    rollerName[index].c_str(), sensorSpeeds[index], upperLimit, lowerLimit,
                    results[index].c_str());
            }
            // Determine the result
            testResult = testPass;
            for(UINT8 index = LFWHEEL; index <= RRWHEEL; index++)
            {
                if(results[index].compare(testPass))
                {
                    testResult = results[index];
                    description = "Sensor speeds out of range";
                }
            }
        }
        else
        {
            testResult = testFail;
            code = GetFaultCode("CommunicationFailure");
            description = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error reading wheel sensor speeds - status: %s\n", 
                ConvertStatusToResponse(status).c_str());
        }

        // Report the results
        SendTestResultWithDetail(testResult, description, code, 
                                 "LfSensorResult", results[0], "",
                                 "RfSensorResult", results[1], "",
                                 "LrSensorResult", results[2], "",
                                 "RrSensorResult", results[3], "");
    }
    else
    {
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Sensor Quality test");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::SensorQualityTest() - Exit, status=%s", testResult.c_str());
    return testResult;
}
#endif 

template <class ModuleType>
string BoschABSTC<ModuleType>::ESPValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::ESPValveFiringTest()\n");
    try
    {
        UpdatePrompts();

        m_ESPStartIndex = TagArray("ESPStart");

        // run the individual wheel ESP tests
        testResult = LFESPTest();
        if(testResult == testPass) RFESPTest();
        if(testResult == testPass) LRESPTest();
        if(testResult == testPass) RRESPTest();

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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::ESPValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    RemovePrompts();

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ESPValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::ABSValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::ABSValveFiringTest()\n");
    try
    {
        m_absStartIndex = TagArray("ABSStart");

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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::ABSValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ABSValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CMABSValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CMABSValveFiringTest()\n");
    try
    {
        m_absStartIndex = TagArray("ABSStart");

        // run the individual wheel ABS tests
        // reordered test to LR RR LF RF
        testResult = LRABSTest();
        if(testResult == testPass) testResult = RRABSTest();
        if(testResult == testPass) testResult = LFABSTest();
        if(testResult == testPass) testResult = RFABSTest();

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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CMABSValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ABSValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::EnableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::EnableSpeedLimit()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::EnableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::EnableSpeedLimit()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::EnableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::EnableValveRelayShutdown()\n");
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
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::EnableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::EnableValveRelayShutdown()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckUplineProcessByte(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT8 processByte;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckUplineProcessByte()\n");
    try
    {
        // 
        moduleStatus = m_vehicleModule.GetInfo("CheckProcessByte", processByte);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA, "Process byte: $%02hhX\n");
            if(processByte != 0xAA)
            {
                Log(LOG_DEV_DATA,"Process Byte fail\n");
                testResult = testFail;
            }
            else
            {
                Log(LOG_DEV_DATA,"Process Byte pass\n");
                testResult = testPass;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CheckProcessByte"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CheckProcessByte"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error Checking Process Byte - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Check Process Byte: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckUplineProcessByte() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckUplineProcessByte()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckEolByte(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    UINT8 eolByte;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckEolByte()\n");
    try
    {
        // 
        moduleStatus = m_vehicleModule.GetInfo("CheckEolByte", eolByte);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA, "Eol Byte: $%02hhX\n");
            if(eolByte != 0xAA)
            {
                Log(LOG_DEV_DATA,"Eol fail\n");
                testResult = testFail;
            }
            else
            {
                Log(LOG_DEV_DATA,"Eol pass\n");
                testResult = testPass;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CheckEolByte"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CheckEolByte"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error checking eol byte - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Check eol byte: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckEolByte() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckEolByte()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::ReadSensorSpeeds(void)
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
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ReadSensorSpeeds()\n");
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string BoschABSTC<ModuleType>::StartAbsTest(void)
{
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "BoschABSTC::StartAbsTest() - Enter");
    // Determine if this test needs to run
    if(!ShortCircuitTestStep())
    {
        testResult = BEP_STATUS_SUCCESS == m_vehicleModule.CommandModule("StartAbsTest") ? testPass : testFail;
        Log(LOG_ERRORS, "Commanded module to start ABS test: %s", testResult.c_str());
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
        // Wait a bit before sending the next command
        BposSleep(GetParameterInt("LeftFrontReductionPulseLength"));
    }
    else
    {   // Need to skip this test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping StartAbsTest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::StartAbsTest() - Enter");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
BEP_STATUS_TYPE BoschABSTC<ModuleType>::AnalyzeValveCross(void)
{   // Call the Generic ABS base class and skip the Korea base class, generic is more up to date
    return GenericABSTCTemplate<ModuleType>::AnalyzeValveCross();
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string BoschABSTC<ModuleType>::LFESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::LFESPTest()\n");

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

            moduleStatus = m_vehicleModule.CommandModule(espCommand);
            if(espCommand == "LFESPEnd" && GetParameterBool("CheckResponseInProgress") && !GetParameterBool("AlwaysWaitForControlComplete"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && ((espCommand != "LFESPEnd") || GetParameterBool("AlwaysWaitForControlComplete")))
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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::LFESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::RFESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::RFESPTest()\n");

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
            espCommand = "RFESPFinalize";
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
            moduleStatus = m_vehicleModule.CommandModule(espCommand);
            if(espCommand == "RFESPEnd" && GetParameterBool("CheckResponseInProgress") && !GetParameterBool("AlwaysWaitForControlComplete"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && ((espCommand != "RFESPEnd") || GetParameterBool("AlwaysWaitForControlComplete")))
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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::RFESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::LRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::LRESPTest()\n");

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
            espCommand = "ESPPrimaryValve2OnRear";
            break;
        case ESP_START:
            m_ESPIndex[LRWHEEL].buildStart = TagArray("LRESPBuildStart");
            espCommand = "LRESPStart";
            break;
        case ESP_PRIMARY_OFF:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LRWHEEL].buildEnd = TagArray("LRESPBuildEnd");
            espCommand = "ESPPrimaryValve2Off";
            break;
        case ESP_END:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LRWHEEL].reductionStart = TagArray("LRESPReductionStart");
            else m_ESPIndex[LRWHEEL].buildEnd = TagArray("LRESPBuildEnd");
            espCommand = "LRESPEnd";
            break;
        case ESP_FINALIZE:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LRWHEEL].reductionEnd = TagArray("LRESPReductionEnd");
            else m_ESPIndex[LRWHEEL].reductionStart = TagArray("LRESPReductionStart");
            espCommand = "LRESPFinalize";
            break;
        case ESP_DONE:
            if(GetParameterBool("CheckResponseInProgress")) m_ESPIndex[LRWHEEL].reductionEnd = TagArray("LRESPReductionEnd");
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {
            moduleStatus = m_vehicleModule.CommandModule(espCommand);
            if(espCommand == "LRESPEnd" && GetParameterBool("CheckResponseInProgress") && !GetParameterBool("AlwaysWaitForControlComplete"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && ((espCommand != "LRESPEnd") || GetParameterBool("AlwaysWaitForControlComplete")))
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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::LRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::RRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    bool isControlComplete = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::RRESPTest()\n");

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
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RRWHEEL].buildEnd = TagArray("RRESPBuildEnd");
            espCommand = "ESPPrimaryValve1Off";
            break;
        case ESP_END:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RRWHEEL].reductionStart = TagArray("RRESPReductionStart");
            else m_ESPIndex[RRWHEEL].buildEnd = TagArray("RRESPBuildEnd");
            espCommand = "RRESPEnd";
            break;
        case ESP_FINALIZE:
            if(!GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RRWHEEL].reductionEnd = TagArray("RRESPReductionEnd");
            else m_ESPIndex[RRWHEEL].reductionStart = TagArray("RRESPReductionStart");
            espCommand = "RRESPFinalize";
            break;
        case ESP_DONE:
            /** 
             * 11/11/2004 BRM
             * Removed this and added a StopPumpMotor test step after the ESP test
             */
            //              espCommand = "ESPPumpOff";
            if(GetParameterBool("CheckResponseInProgress")) m_ESPIndex[RRWHEEL].reductionEnd = TagArray("RRESPReductionEnd");
            done = true;
            break;
        default:
            break;
        }
        // If we have a command to send
        if(espCommand.empty() == false)
        {
            moduleStatus = m_vehicleModule.CommandModule(espCommand);
            if(espCommand == "RRESPEnd" && GetParameterBool("CheckResponseInProgress") && !GetParameterBool("AlwaysWaitForControlComplete"))
            {
                m_vehicleModule.ReadModuleData("ReadControlComplete",isControlComplete);
            }
            if(GetParameterBool("CheckResponseInProgress") && 
               moduleStatus == BEP_STATUS_SUCCESS && ((espCommand != "RRESPEnd") || GetParameterBool("AlwaysWaitForControlComplete")))
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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::RRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::LFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LFWHEEL;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::LFABSTest()\n");

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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::LFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::RFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RFWHEEL;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::RFABSTest()\n");

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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::RFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::LRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LRWHEEL;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::LRABSTest()\n");

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


    Log(LOG_FN_ENTRY, "Exit BoschABSTC::LRABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::RRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RRWHEEL;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::RRABSTest()\n");

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

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::RRABsTest()\n");
    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::EvaluateSensorCross(void)
{
    string testResult(testPass);
    const string testDescription( GetTestStepInfo("Description"));
    string testResultCode( "0000");
    string faultTag("");
    int ii, wheelIdx;
    string frontCross(testPass), rearCross(testPass), frontRearCross( testPass);
    WheelSpeeds_t   deltaSpeeds[ REC_DELTA_IDX+1];
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::EvaluateSensorCross()\n");

    /**
     * Loop through LF, RF and LR (don't need RR in outer loop because
     * it will be covered by comparing t he first 3 wheels):
     *      LFWHEEL: LF/RF, LF/LR, LF/RR
     *      RFWHEEL: RF/LR, RF/RR
     *      LRWHEEL: LR/RR
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
             *      wheelIdx |   ii
             *      =========|==========================
             *      LFWHEEL  | RFWHEEL, LRWHEEL, RRWHEEL
             *      RFWHEEL  | LRWHEEL, RRWHEEL
             *      LRWHEEL  | RRWHEEL
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
                SendSubtestResultWithDetail(faultTag, testResult,
                                            testDescription,
                                            GetFaultCode(faultTag),
                                            faultTag,
                                            GetFaultDescription(faultTag));

                if(wheelIdx < LRWHEEL)
                {
                    frontCross = testFail;
                }
                else
                {
                    rearCross = testFail;
                }
            }
            faultTag.clear();
        }
    }

    // Send the Sensor Cross Results
    SendSubtestResult("FrontSensorCross", frontCross, "Front ABS sensors crossed", "0000");
    SendSubtestResult("RearSensorCross", rearCross, "Rear ABS sensors crossed", "0000");
    SendSubtestResult("FrontToRearSensorCross", frontRearCross, "Front ABS sensors crossed with rears", "0000");

    SendTestResult(testResult, testDescription, testResultCode);

    Log(LOG_FN_ENTRY, "Exit BoschABSTC::EvaluateSensorCross(), status=%s\n", testResult.c_str());

    return( testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::StopPumpMotor(void)
{
    string testResult;
    bool havePrompts = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::StopPumpMotor()\n");

    // Determine if the test should be performed
    if(!ShortCircuitTestStep())
    {
        if((GetTestStepInfo("Prompt1").size()) || (GetTestStepInfo("Prompt2").size()))
        {
            havePrompts = true;
            UpdatePrompts();
        }

        testResult = KoreaAbsTcTemplate<ModuleType>::StopPumpMotor();

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
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::StopPumpMotor(), status=%s\n", testResult.c_str());

    return(testResult);
}

template <class ModuleType>
string BoschABSTC<ModuleType>::BrakeSwitchTest(const string& position)
{
    string testResult;
    bool havePrompts = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::BrakeSwitchTest(%s)\n", position.c_str());

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
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::BrakeSwitchTest(%s), status=%s\n", position.c_str(), testResult.c_str());
    return(testResult);
}


template <class ModuleType>
string BoschABSTC<ModuleType>::WaitForControlComplete()
{
    string testResult;
    bool isControlComplete = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::WaitForControlComplete()\n");


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
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::WaitForControlComplete(), isControlComplete:%s status=%s\n", 
        isControlComplete ? "true" : "false", testResult.c_str());
    return(testResult);
}
//-----------------------------------------------------------------------------
template <class ModuleType>
string BoschABSTC<ModuleType>::WarningLight(void)
{   
    Log(LOG_FN_ENTRY, "BoschABSTC::WarningLight() - Enter");
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
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::WarningLight() - Exit");
    return testResult;
}

template <class ModuleType>
string BoschABSTC<ModuleType>::AllInletValvesOn(void)
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
string BoschABSTC<ModuleType>::AllInletValvesOff(void)
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
string BoschABSTC<ModuleType>::RunPumpMotor(void)
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

template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::EnableElectricVacuumPump(void)
{
    string result(BEP_TESTING_STATUS);
    // Command the module to enter normal mode
    BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnableElectricVacuumPump");
    result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    Log(LOG_ERRORS, "Commanded module to enable electric vacuum pump: %s (status: %s)", 
        result.c_str(), ConvertStatusToResponse(status).c_str());
    // Report the result
    SendTestResult(result, GetTestStepInfo("Description"));
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::EnableElectricVacuumPump() - Exit");
    return result;
}

template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::DisableElectricVacuumPump(void)
{
    string result(BEP_TESTING_STATUS);
    // Command the module to enter normal mode
    BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("DisableElectricVacuumPump");
    result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
    Log(LOG_ERRORS, "Commanded module to enable electric vacuum pump: %s (status: %s)", 
        result.c_str(), ConvertStatusToResponse(status).c_str());
    // Report the result
    SendTestResult(result, GetTestStepInfo("Description"));
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::DisableElectricVacuumPump() - Exit");
    return result;
}

template <class ModuleType>
string BoschABSTC<ModuleType>::CheckModuleType(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool ecuTypeCorrect;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckModuleType()\n");
    try
    {
        // 
        moduleStatus = m_vehicleModule.GetInfo("CheckModuleType", ecuTypeCorrect);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
            Log(LOG_DEV_DATA, "Eol Byte: $%02hhX\n");
            if(!ecuTypeCorrect)
            {
                Log(LOG_DEV_DATA,"Module Type Check fail\n");
                testResult = testFail;
            }
            else
            {
                Log(LOG_DEV_DATA,"Module Type Check pass\n");
                testResult = testPass;
            }

            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CheckModuleType"));
            testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CheckEolByte"));
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error checking eol byte - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Check Module Type: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in BoschABSTC::CheckModuleType() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckModuleType()\n");
    return(testResult);
}
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::EnterNormalMode(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and check if the test step should be performed
    Log(LOG_FN_ENTRY, "BoschABSTC::EnterNormalMode() - Enter");
    if(!ShortCircuitTestStep())
    {   // Command the module to enter normal mode
        BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("EnterNormalMode");
        result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
        Log(LOG_ERRORS, "Commanded module to enter normal mode: %s (status: %s)", 
            result.c_str(), ConvertStatusToResponse(status).c_str());
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step EnterNormalMode");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::EnterNormalMode() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::DisableForceMeter(void)
{
    // Disable brake force meter updates
    m_baseBrakeTool->DisableForceUpdates();
    // Deactivate the brake force meter
    SystemWrite(GetDataTag("BrakeActive"), 0);
    Log(LOG_FN_ENTRY, "BoschABSTC::DisableForceMeter() - Disabled brake force meter");
    return testPass;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::IndividualSensorTest(void)
{   // Log the entry and determine if this test should be performed
    string testResult(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "BoschABSTC::IndividualSensorTest() - Enter");
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   // Store the oringinal drive axle value so we can restore after the test
        string driveAxle(SystemRead(DRIVE_AXLE_TAG));
        OriginalDriveAxle(&driveAxle);
        // Switch the drive axle so the rear set of motors is enabled
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        // Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
        m_MotorController.Write(COMMAND_SPEED, "0", false);
        m_MotorController.Write(COMMAND_TORQUE, "0", false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        BposSleep( 100);
        string sensorResult[GetRollerCount()];
        DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep(GetParameterInt("SensorSpeedDriverDelay"));
        // Check all wheels independently
        for(UINT8 rollerIndex = LFWHEEL; (rollerIndex <= RRWHEEL) && (BEP_STATUS_SUCCESS == StatusCheck()); rollerIndex++)
        {   // Check if the drive axle must be changed
            if((rollerIndex == LRWHEEL) && ((rollerIndex - 1) == RFWHEEL))
            {
                SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
                BposSleep(1000);
            }
            // Perform the wheel speed sensor test
            // Set the speed of the first roller
            m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", GetParameter("SensorTestSpeed"), true);
            // Wait a bit for the rollers to start accelerating
            BposSleep(GetParameterInt("SensorTestStartDelay"));
            // Read the sensor speeds from the module
            WheelSpeeds_t moduleSpeeds;
            BEP_STATUS_TYPE status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
            if(BEP_STATUS_SUCCESS == status)
            {   // Get the roller speeds
                float rollerSpeeds[GetRollerCount()];
                GetWheelSpeeds(rollerSpeeds);
                // Check all wheels, only current wheel should be spinning, all others should be 0
                float maxSpeed = rollerSpeeds[rollerIndex] * (1.0 + (GetParameterFloat("SensorSpeedTolerance") / 100.0));
                float minSpeed = rollerSpeeds[rollerIndex] * (1.0 - (GetParameterFloat("SensorSpeedTolerance") / 100.0));
                string indResult = (rollerSpeeds[rollerIndex] >= (GetParameterFloat("SensorTestSpeed") - 1.0)) ? testPass : testFail;
                for(UINT8 wheel = LFWHEEL; 
                     (wheel <= RRWHEEL) && !indResult.compare(testPass) && (BEP_STATUS_SUCCESS == StatusCheck()); wheel++)
                {
                    if(wheel == rollerIndex)
                    {   // Make sure this wheel is at the correct speed
                        indResult = ((minSpeed <= moduleSpeeds[wheel]) && (moduleSpeeds[wheel] <= maxSpeed)) ? testPass : testFail;
                    }
                    else
                    {
                        indResult = (moduleSpeeds[wheel] <= GetParameterFloat("ZeroSpeedTolerance")) ? testPass : testFail;
                    }
                }
                sensorResult[rollerIndex] = indResult;
                Log(LOG_DEV_DATA, "%s: %s - Sensor Speed: %.2f, Roller Speed: %.2f, [min: %.2f, max: %.2f]", 
                    rollerName[rollerIndex].c_str(), sensorResult[rollerIndex].c_str(), moduleSpeeds[rollerIndex], 
                    rollerSpeeds[rollerIndex], maxSpeed, minSpeed);
                // Report this roller
                char buff[32];
                SendSubtestResultWithDetail(rollerName[rollerIndex]+"SensorSpeedTest", sensorResult[rollerIndex], 
                                            GetTestStepInfo("Description"), "0000",
                                            "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[rollerIndex]), unitsMPH,
                                            "MinLimit", CreateMessage(buff, sizeof(buff), "%.2f", minSpeed), unitsMPH,
                                            "MaxLimit", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeed), unitsMPH,
                                            "RollerSpeed", GetParameter("SensorTestSpeed"), unitsMPH);
            }
            else
            {   // Could not read sensor speeds
                Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
                sensorResult[rollerIndex] = testFail;
            }
            // Set the motor back to boost and zero speed
            m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", "0", true);
            // Wait for zero speed
            while(!SystemReadBool(ZEROSPEED_TAG) && (BEP_STATUS_SUCCESS == StatusCheck()))  BposSleep(250);
        }
        // Remove the prompts
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        // Reset the drive axle after we reach zero speed
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        CheckZeroSpeed();
        SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
        // Report the overall result
        testResult = (!sensorResult[LFWHEEL].compare(testPass) && !sensorResult[RFWHEEL].compare(testPass) &&
                      !sensorResult[LRWHEEL].compare(testPass) && !sensorResult[RRWHEEL].compare(testPass)) ? testPass : testFail;
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed sensor test, not testing again");
        testResult = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping sensor test");
        testResult = testSkip;
    }
    // Log the exit and return the result.
    Log(LOG_FN_ENTRY, "BoschABSTC::IndividualSensorTest() - Exit");
    return testResult;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::BrakeBurnishCycle(void)
{
    string result(testPass);
    // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "BoschABSTC::BrakeBurnishCycle() - Enter");
    
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass) && GetTestStepResult().compare(testSkip) && !OperatorPassFail("PerformBrakeBurnish").compare(testPass))
    {
        // Automatically run Electric Park Brake Burnish cycle
        if(GetParameterBool("BurnishEPBBeforeBrake")) result = PerformElectricParkBrakeBurnishCycle();
        Log(LOG_DEV_DATA, "Performing Brake Burnish Cycle\n");

        for(INT32 burnishCycles = 0; 
             (burnishCycles < GetParameterInt("BrakeBurnishCycles")) && 
             (BEP_STATUS_SUCCESS == StatusCheck()) && 
             !result.compare(testPass); 
             burnishCycles++)
        {
            for(INT32 burnishIterations = 0;
                 (burnishIterations < GetParameterInt("BurnishIterations")) && 
                 (BEP_STATUS_SUCCESS == StatusCheck()) && 
                 !result.compare(testPass);
                 burnishIterations++)
            {   // Prompt the operator to achieve the desired speed range
                result = AccelerateToTestSpeed(GetParameterFloat("BrakeBurnishStartSpeed"), GetParameter("BrakeBurnishStartSpeedRange"), 
                                               GetTestStepInfoInt("ScanDelay"), false, GetPrompt("AccelerateToTargetSpeed"), false);
                //ADDED statement
                while((GetBoostedRollSpeed() < GetParameterFloat("BrakeBurnishMinNonDrivenAxleSpeed")) && (BEP_STATUS_SUCCESS == StatusCheck()))
                {
                    Log(LOG_DEV_DATA, "Brake Burnish Cycle waiting for boosted axle speed to reach BrakeBurnishMinNonDriveAxleSpeed\n");
                    BposSleep(500);
                }
                if(!result.compare(testPass))
                {
                    // Disable motor boost so the rollers are in free roll mode
                    if(GetParameterBool("BrakeBurnishDisableBoostOnDecel"))  DisableElectricMotorBoost();

                                      // Display the brake force target for the operator
                    string forceTarget = ((burnishIterations == (GetParameterInt("BurnishIterations")-1)) ? 
                                          "BrakeBurnishFinalIterationTarget" : "BrakeBurnishBrakeForceTarget");
                    m_baseBrakeTool->EnableForceUpdates();
                    SystemWrite(GetDataTag("BrakeTarget"), GetParameter(forceTarget));
                    SystemWrite(GetDataTag("BrakeActive"), true);
                    // If the target speed has been reached, brake to the target speed
                    DisplayPrompt(GetPromptBox("BrakeModeratelyInGreenBand"), GetPrompt("BrakeModeratelyInGreenBand"),
                                  GetPromptPriority("BrakeModeratelyInGreenBand"));
                    result = SlowDownToTestSpeed(GetParameterFloat("BrakeBurnishEndSpeed"), 
                                                 GetParameter("BrakeBurnishEndSpeedRange"),
                                                 GetTestStepInfoInt("ScanDelay"), true, 
                                                 GetParameter("BrakeToTargetSpeedPrompt"));
                    // Disable the brake force meter
                    RemovePrompt(GetPromptBox("BrakeModeratelyInGreenBand"), GetPrompt("BrakeModeratelyInGreenBand"),
                                 GetPromptPriority("BrakeModeratelyInGreenBand"));
                    SystemWrite(GetDataTag("BrakeActive"), false);
                    SystemWrite(GetDataTag("BrakeTarget"), "0 0");
                    m_baseBrakeTool->DisableForceUpdates();
                    // Reneable boosting
                    if(GetParameterBool("BrakeBurnishDisableBoostOnDecel"))  EnableElectricMotorBoost();
                }
                else
                {
                    Log(LOG_ERRORS, "Timeout waiting for brake burnish start speed");
                    result = testTimeout;
                }
            }
        }
        // Automatically run Electric Park Brake Burnish cycle if everything else passed and not run at the beginning
        if(GetParameterBool("BurnishEPBAfterBrake") && !GetParameterBool("BurnishEPBBeforeBrake") && !result.compare(testPass)) result = PerformElectricParkBrakeBurnishCycle();
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Brake burnish cycle");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::BrakeBurnishCycle() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::ElectricParkBrakeBurnishCycle(void)
{
    string result(testPass);
    // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "BoschABSTC::ElectricParkBrakeBurnishCycle() - Enter");
    
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass) && GetTestStepResult().compare(testSkip) && !OperatorPassFail("PerformParkBrakeBurnish").compare(testPass))
    {
        result = PerformElectricParkBrakeBurnishCycle();
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
    }
    else
    {   // Need to skip this test
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Brake burnish cycle");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::ElectricParkBrakeBurnishCycle() - Enter");
    return result;
}

template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::PerformElectricParkBrakeBurnishCycle(void)
{
    string result(testPass);
    Log(LOG_DEV_DATA, "Performing Electric Park Brake Burnish Cycle\n");

    if(GetParameterBool("EPBBurnishSpeedMatchDisable"))
    {
        // Command rolls to zero torque
        m_MotorController.Write(COMMAND_TORQUE, "0", true);
        m_MotorController.Write(MOTOR_MODE, TORQUE_MODE, true);
    }

    for(INT32 burnishCycles = 0; 
         (burnishCycles < GetParameterInt("EPBBurnishCycles")) && 
         (BEP_STATUS_SUCCESS == StatusCheck()) && 
         !result.compare(testPass); 
         burnishCycles++)
    {
        for(INT32 burnishIterations = 0;
             (burnishIterations < GetParameterInt("EPBBurnishIterations")) && 
             (BEP_STATUS_SUCCESS == StatusCheck()) && 
             !result.compare(testPass);
             burnishIterations++)
        {   // Prompt the operator to achieve the desired speed range
            result = AccelerateToTestSpeed(GetParameterFloat("EPBBurnishStartSpeed"), GetParameter("EPBBurnishStartSpeedRange"), 
                                           GetTestStepInfoInt("ScanDelay"), false);
            if(!result.compare(testPass))
            {
                
                // If the target speed has been reached, brake to the target speed
                DisplayPrompt(GetPromptBox("PressAndHoldParkBrake"), GetPrompt("PressAndHoldParkBrake"),
                              GetPromptPriority("PressAndHoldParkBrake"));
                result = SlowDownToTestSpeed(GetParameterFloat("EPBBurnishEndSpeed"), 
                                             GetParameter("EPBBurnishEndSpeedRange"),
                                             GetTestStepInfoInt("ScanDelay"), true, 
                                             GetParameter("EPBBrakeToTargetSpeedPrompt"));
                // Hold speed while park brake is applied
                //string burnishTime = ((burnishIterations == (GetParameterInt("EPBBurnishIterations")-1)) ? 
                //                          "EPBBurnishFinalIterationHoldTime" : "EPBBurnishHoldTime");
                //BposSleep(GetParameterInt(burnishTime));
                RemovePrompt(GetPromptBox("PressAndHoldParkBrake"), GetPrompt("PressAndHoldParkBrake"),
                              GetPromptPriority("PressAndHoldParkBrake"));
                DisplayPrompt(GetPromptBox("MaintainSpeedInBand"), GetPrompt("MaintainSpeedInBand"),
                              GetPromptPriority("MaintainSpeedInBand"));
                SystemWrite(GetDataTag("SpeedTarget"), GetParameter("EPBBurnishEndSpeedRange"));
                // Hold speed while letting brakes cool down
                string coolDownTime = ((burnishIterations == (GetParameterInt("EPBBurnishIterations")-1)) ? 
                                          "EPBBurnishFinalIterationCoolDownTime" : "EPBBurnishCoolDownTime");
                DisplayPrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"),
                              GetPromptPriority("BrakeCoolDown"), GetParameter("BrakeCoolDownBgColor"));
                BposSleep(GetParameterInt(coolDownTime));
                // Remove prompts and reset speed target
                RemovePrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"),
                              GetPromptPriority("BrakeCoolDown"));
                RemovePrompt(GetPromptBox("MaintainSpeedInBand"), GetPrompt("MaintainSpeedInBand"),
                              GetPromptPriority("MaintainSpeedInBand"));
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                SystemWrite(string("PromptBox1BGColor"), string("white"));
                SystemWrite(string("PromptBox2BGColor"), string("white"));
            }
            else
            {
                Log(LOG_ERRORS, "Timeout waiting for brake burnish start speed");
                result = testTimeout;
            }
        }
    }

    if(GetParameterBool("EPBBurnishSpeedMatchDisable"))
    {
        // Set motor mode back to Boost
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
    }

    Log(LOG_DEV_DATA, "Done Performing Electric Park Brake Burnish Cycle, status=%s\n", result.c_str());
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::StaticBrakeBurnishCycle(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "BoschABSTC::StaticBrakeBurnishCycle() - Enter");
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass) && 
       !OperatorPassFail("PerformBrakeBurnish",(GetParameterInt("BurnishPromptTimeout") ? GetParameterInt("BurnishPromptTimeout") : NULL)).compare(testPass))
    {   // Place the motors in speed mode
        m_MotorController.Write(COMMAND_SPEED, string("0"), false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        // Save the original drive axle
        string driveAxle(SystemRead(DRIVE_AXLE_TAG));
        OriginalDriveAxle(&driveAxle);
        // Switch to control the front motors
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        BposSleep(1000);
        SetData(DRIVE_AXLE_TAG, OriginalDriveAxle());
        
        // Run the front wheels
        // Accelerate the front rollers to speed
        DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep(GetParameterInt("StaticBrakeBurnishOperatorSetupTime"));
        m_MotorController.Write(rollerName[LFWHEEL]+"SpeedValue", GetParameter("StaticBrakeBurnishAccelSpeed"), false);
        m_MotorController.Write(rollerName[RFWHEEL]+"SpeedValue", GetParameter("StaticBrakeBurnishAccelSpeed"), true);
        // Wait for the rollers to get to speed
        float rollerSpeeds[GetRollerCount()];
        do
        {
            BposSleep(GetTestStepInfoInt("ScanDelay"));
            GetWheelSpeeds(rollerSpeeds);
        } while((BEP_STATUS_SUCCESS == StatusCheck()) && 
                (rollerSpeeds[LFWHEEL] < (GetParameterFloat("StaticBrakeBurnishAccelSpeed") - 3.0)));
        // Check if we can keep testing
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        if(BEP_STATUS_SUCCESS == StatusCheck())
        {   // Have operator apply brake to target speed
            SystemWrite(GetDataTag("SpeedTarget"), GetParameter("StaticBrakeBurnishBrakeSpeedRange"));
            DisplayPrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), GetPromptPriority("BrakeToTargetSpeed"));
            BEP_STATUS_TYPE status = StatusSleep(GetParameterInt("StaticBrakeBurnishTime"));
            RemovePrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), GetPromptPriority("BrakeToTargetSpeed"));
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
            if(BEP_STATUS_SUCCESS == status)
            {   // Cool down period
                DisplayPrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), 
                              GetPromptPriority("BrakeCoolDown"), GetParameter("BrakeCoolDownBgColor"));
                DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
                m_MotorController.Write(rollerName[LFWHEEL]+"SpeedValue", GetParameter("StaticBrakeBurnishCoolDownSpeed"), false);
                m_MotorController.Write(rollerName[RFWHEEL]+"SpeedValue", GetParameter("StaticBrakeBurnishCoolDownSpeed"), true);
                StatusSleep(GetParameterInt("StaticBrakeBurnishCoolDownTime"));
                m_MotorController.Write(rollerName[LFWHEEL]+"SpeedValue", string("0"), false);
                m_MotorController.Write(rollerName[RFWHEEL]+"SpeedValue", string("0"), true);
                // Wait for zero speed
                while(!atob(SystemRead(ZEROSPEED_TAG).c_str())) BposSleep(GetTestStepInfoInt("ScanDelay"));
                RemovePrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), GetPromptPriority("BrakeCoolDown"));
                RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
                SystemWrite(string("PromptBox1BGColor"), string("white"));
                SystemWrite(string("PromptBox2BGColor"), string("white"));
            }
            else
            {
                Log(LOG_ERRORS, "Status sleep failed during brake burnish cycle");
                result = testFail;
            }
        }
        else
        {
            Log(LOG_ERRORS, "Cannot continue testing: StatusCheck - %s", ConvertStatusToResponse(StatusCheck()).c_str());
            result = testFail;
        }
        
        // Burnish the rears if the fronts have not failed
        if(!result.compare(BEP_TESTING_STATUS))
        {   // Switch the drive axle to control rear rdrives
            SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
            BposSleep(1000);
            SetData(DRIVE_AXLE_TAG, OriginalDriveAxle());
            DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            BposSleep(GetParameterInt("StaticBrakeBurnishOperatorSetupTime"));
            // Accelerate the rollers
            m_MotorController.Write(COMMAND_SPEED, GetParameter("StaticBrakeBurnishAccelSpeed"), true);
            // Wait for the roller to get to speed
            float rollerSpeeds[GetRollerCount()];
            do
            {
                BposSleep(GetTestStepInfoInt("ScanDelay"));
                GetWheelSpeeds(rollerSpeeds);
            } while((BEP_STATUS_SUCCESS == StatusCheck()) && 
                    (rollerSpeeds[LRWHEEL] < (GetParameterFloat("StaticBrakeBurnishAccelSpeed") - 3.0)));
            // Check if we can keep testing
            RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
            RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
            if(BEP_STATUS_SUCCESS == StatusCheck())
            {   // Have operator apply brake to target speed
                if(GetParameterBool("StaticBrakeBurnishDisplaySpeedTargetForRear"))
                {
                    SystemWrite(GetDataTag("SpeedTarget"), GetParameter("StaticBrakeBurnishBrakeSpeedRange"));
                }
                else
                {
                    SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                }
                DisplayPrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), 
                              GetPromptPriority("BrakeToTargetSpeed"));
                BEP_STATUS_TYPE status = StatusSleep(GetParameterInt("StaticBrakeRearBurnishTime"));
                RemovePrompt(GetPromptBox("BrakeToTargetSpeed"), GetPrompt("BrakeToTargetSpeed"), 
                             GetPromptPriority("BrakeToTargetSpeed"));
                SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
                if(BEP_STATUS_SUCCESS == status)
                {   // Cool down period
                    DisplayPrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), 
                                  GetPromptPriority("BrakeCoolDown"), GetParameter("BrakeCoolDownBgColor"));
                    DisplayPrompt(GetPromptBox("ReleaseParkBrake"), GetPrompt("ReleaseParkBrake"), GetPromptPriority("ReleaseParkBrake"));
                    m_MotorController.Write(COMMAND_SPEED, GetParameter("StaticBrakeBurnishCoolDownSpeed"), true);
                    StatusSleep(GetParameterInt("StaticBrakeBurnishCoolDownTime"));
                    m_MotorController.Write(COMMAND_SPEED, string("0"), true);
                    // Wait for zero speed
                    while(!atob(SystemRead(ZEROSPEED_TAG).c_str())) BposSleep(GetTestStepInfoInt("ScanDelay"));
                    RemovePrompt(GetPromptBox("BrakeCoolDown"), GetPrompt("BrakeCoolDown"), GetPromptPriority("BrakeCoolDown"));
                    RemovePrompt(GetPromptBox("ReleaseParkBrake"), GetPrompt("ReleaseParkBrake"), GetPromptPriority("ReleaseParkBrake"));
                    SystemWrite(string("PromptBox1BGColor"), string("white"));
                    SystemWrite(string("PromptBox2BGColor"), string("white"));
                    result = testPass;
                }
                else
                {
                    Log(LOG_ERRORS, "Status sleep failed during brake burnish cycle");
                    result = testFail;
                }
            }
            else
            {
                Log(LOG_ERRORS, "Cannot continue testing: StatusCheck - %s", ConvertStatusToResponse(StatusCheck()).c_str());
                result = testFail;
            }
        }
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"));
        SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
    }
    else if(!GetTestStepResult().compare(testPass))
    {   // This test already passed, skip it but leave it marked as pass
        Log(LOG_DEV_DATA, "Already passed static brake burnish, not testing again");
        result = testPass;
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping static brake burnish cycle");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::StaticBrakeBurnishCycle() - Exit");
    return result;
}

// Removed because this should be inherited from the parent class KoreaAbsTcTemplate.
// In a test, there end up being two instances of OriginalDriveAxle and one gets set and the other is null
//----------------------------------------------------------------------------
//template<class ModuleInterface>
//string& BoschABSTC<ModuleInterface>::OriginalDriveAxle(const string *driveAxle /*= NULL*/)
//{
//    if(driveAxle != NULL)  m_originalDriveAxle = *driveAxle;
//    return m_originalDriveAxle;
//}



//----------------------------------------------------------------------------
template<class ModuleInterface>
string BoschABSTC<ModuleInterface>::AccelerateToBrakeSpeed(void)
{
    string result(BEP_TESTING_STATUS);
    // Log the entry and check if the test step should be performed
    Log(LOG_FN_ENTRY, "BoschABSTC::AccelerateToBrakeSpeed() - Enter");
    if(!ShortCircuitTestStep())
    {   // Command the module to enter normal mode
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        result = GenericABSTCTemplate<ModuleInterface>::CommandTestStep("AccelerateToBrakeSpeed");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping test step AccelerateToBrakeSpeed");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "BoschABSTC::AccelerateToBrakeSpeed() - Exit");
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
INT32 BoschABSTC<ModuleInterface>::ElectricParkBrakeInit(void)
{
    INT32 result = GenericTC::ElectricParkBrakeInit();
    // Store the oringinal drive axle value so we can restore after the test
    string driveAxle(SystemRead(DRIVE_AXLE_TAG));
    OriginalDriveAxle(&driveAxle);
    // Switch the drive axle so the rear set of motors is enabled
    SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
    BposSleep(1000);
    return result;
}

//----------------------------------------------------------------------------
template<class ModuleInterface>
const string BoschABSTC<ModuleInterface>::PerformPBTorqueTest(const string &direction, bool isTandemAxle/*=false*/)
{
    string status(BEP_TESTING_STATUS);
    // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::PerformPBTorqueTest(%s)\n", direction.c_str());
    float torqueValue = GetParameterFloat("ParkBrakeApplyForce");
    // Make sure the speed setpoints are set to 0 and place the motor controller into speed mode
    m_MotorController.Write(COMMAND_SPEED, "0", false);
    m_MotorController.Write(COMMAND_TORQUE, "0", true);
    BposSleep( 100);
    // Zero check the parameters
    if(torqueValue == 0)  torqueValue = 350;
    // Adjust the torque value
    torqueValue *= 0.5;
    // If we want the wheels to turn forward, we need a negative torque
    if( ((direction == "Forward") && (torqueValue > 0)) ||
        // If we want the wheels to turn in reverse, we need a positive torque value
        ((direction == "Reverse") && (torqueValue < 0)) )
    {
        Log( LOG_DEV_DATA, "Changing torque value from %.02f to %.02f\n", torqueValue, -torqueValue);
        torqueValue *= -1.0;
    }
    // read the current distance of the wheels
    GetWheelDistances( m_parkBrakeDistances[ 0]);
    // Tell the motor to turn for the park brake test
    Log(LOG_DEV_DATA, "Commanding Rear Motors %.2f LBS\n", torqueValue);
    char temp[32];
    m_MotorController.Write(COMMAND_TORQUE, CreateMessage(temp, sizeof(temp), "%.2f", torqueValue), true);
    m_MotorController.Write(MOTOR_MODE, TORQUE_MODE, true);
    // wait the specified amount of time
    SetStartTime();
    bool brakeApplied = false;
    INT32 testTime = GetTestStepInfoInt("TestTime");
    float   wheelSpeeds[GetRollerCount()];
    while((GenericTC::StatusCheck() == BEP_STATUS_SUCCESS) && (testTime > 0) && !brakeApplied)
    {
        GetWheelSpeeds(wheelSpeeds);
        Log(LOG_DEV_DATA, "LF: %f, RF: %f, LR: %f, RR: %f\n", wheelSpeeds[LFWHEEL], wheelSpeeds[RFWHEEL],
            wheelSpeeds[LRWHEEL], wheelSpeeds[RRWHEEL]);
        // Check the brake switch to make sure it is off
        m_vehicleModule.ReadModuleData("ReadBrakeSwitch", brakeApplied);
        if(brakeApplied)
        {
            Log(LOG_ERRORS, "Operator applied brake during park brake test - FAIL");
        }
        BposSleep(100);     // wait 1/10th of a second
        testTime -= 100;    // decrement the time from the total time
    }
    // Clear the torque value
    m_MotorController.Write(COMMAND_TORQUE, string("0"), true);
    Log( LOG_DEV_DATA, "Waiting for zerospeed\n");
    // make sure the vehicle is at zerospeed
    while(TimeRemaining() && (ReadSubscribeData(GetDataTag("Zerospeed")) != "1"))
    {
        BposSleep (100);                // wait a tenth of a second
    }
    Log( LOG_DEV_DATA, "At zerospeed, check distances\n");
    // if and error did not occur and not aborted
    if(!brakeApplied)
    {
        INT32 statusCheck = GenericTC::StatusCheck();
        if( statusCheck == BEP_STATUS_SUCCESS)
        {   // read the current distance of the rear wheels
            GetWheelDistances( m_parkBrakeDistances[ 1]);
            // Analyze the distances that the wheels travelled
            status = ValidateParkBrakeTestDistances(m_parkBrakeDistances[ 0], m_parkBrakeDistances[ 1]);
        }
        else
        {
            Log(LOG_DEV_DATA, (GetTestStepName() + ": Failed Status Check\n").c_str());
            status = ConvertStatusToResponse( statusCheck);
        }
    }
    else
    {
        status = testFail;
        Log(LOG_ERRORS, "Park brake test failed because operator applied brake pedal");
    }
    // Reset to the original drive axle
    BposSleep(1000);
    SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
    RemovePrompts();
    DisplayTimedPrompt(GetPrompt("ReleaseParkBrake"), GetPromptBox("ReleaseParkBrake"), 
                       GetPromptPriority("ReleaseParkBrake"), GetPromptDuration("ReleaseParkBrake"));
    // Send the test result info
    SendTestResult(status, GetTestStepInfo("Description"));
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::PerformPBTorqueTest(%s), status=%s\n", direction.c_str(), status.c_str());

    return status;
}

//-----------------------------------------------------------------------------
template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::ProgramVIN(void)
{
    string testStatus = testFail;
    BEP_STATUS_TYPE status;
    string faultTag("CommunicationFailure");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::ProgramVIN()\n");
    if(ShortCircuitTestStep())
    {
        Log(LOG_FN_ENTRY, "Skipping test step BoschABSTC::ProgramVin()");
        testStatus = testSkip;
    }
    else
    {
        

                try
                {   // Actually program the vin
                    status = m_vehicleModule.ProgramVIN();
                    if(status == BEP_STATUS_SUCCESS)
                    {
                        testStatus = testPass;
                        SendTestResult(testPass, GetTestStepInfo("Description"));
                    }
                    else
                    {
                        testStatus = testFail;
                        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                                 GetFaultCode(faultTag), GetFaultName(faultTag),
                                                 GetFaultDescription(faultTag));
                    }
                }
                catch(ModuleException& caughtModuleException)
                {
                    Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
                        caughtModuleException.message().c_str());
                    testStatus = testFail;
                }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ProgramVIN(), status=%s\n", testStatus.c_str());
    // Return the test result
    return(testStatus);
} 

//-----------------------------------------------------------------------------
template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::ResetVIN(void)
{
    string testStatus = testFail;
    BEP_STATUS_TYPE status;
    string faultTag("CommunicationFailure");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::ResetVIN()\n");
    if(ShortCircuitTestStep())
    {
        Log(LOG_FN_ENTRY, "Skipping test step BoschABSTC::ResetVin()");
        testStatus = testSkip;
    }
    else
    {
        

                try
                {   // Actually program the vin
                    status = m_vehicleModule.CommandModule("ResetVIN");
                    if(status == BEP_STATUS_SUCCESS)
                    {
                        testStatus = testPass;
                        SendTestResult(testPass, GetTestStepInfo("Description"));
                    }
                    else
                    {
                        testStatus = testFail;
                        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                                 GetFaultCode(faultTag), GetFaultName(faultTag),
                                                 GetFaultDescription(faultTag));
                    }
                }
                catch(ModuleException& caughtModuleException)
                {
                    Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
                        caughtModuleException.message().c_str());
                    testStatus = testFail;
                }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::ResetVIN(), status=%s\n", testStatus.c_str());
    // Return the test result
    return(testStatus);
} 

//-----------------------------------------------------------------------------
template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::LearnPerformanceType(void)
{
    string testStatus = testFail;
    BEP_STATUS_TYPE status;
    string faultTag("CommunicationFailure");
    string mismatchFaultTag("PerformanceTypeError");
    string performanceType;
    string code("0000");
    string description = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::LearnPerformanceType()\n");
    if(ShortCircuitTestStep())
    {
        Log(LOG_FN_ENTRY, "Skipping test step BoschABSTC::LearnPerformanceType()");
        testStatus = testSkip;
    }
    else
    {
        try
        {
            status = m_vehicleModule.ReadModuleData("ReadPerformanceType",performanceType);
            if(status == BEP_STATUS_SUCCESS)
            {
                if(!performanceType.compare("NotConfigured") || GetParameterBool("AlwaysLearnPerformanceType"))
                {   // Learn Performance Type
                    status = m_vehicleModule.CommandModule("LearnPerformanceType");
                    if(status == BEP_STATUS_SUCCESS)
                    {
                        BposSleep(GetParameterInt("PerformanceTypeCheckDelay"));
                        status = m_vehicleModule.ReadModuleData("ReadPerformanceType",performanceType);
                        if(status == BEP_STATUS_SUCCESS)
                        {
                            testStatus = (performanceType.compare("NotConfigured")) ? testPass : testFail;
                            code = (testStatus == testPass) ? code : GetFaultCode(mismatchFaultTag);
                            description = (testStatus == testPass) ? description : GetFaultDescription(mismatchFaultTag);
                            SendTestResultWithDetail(testStatus, description, code, "Performance Type", performanceType);
                        }
                        else
                        {
                            testStatus = testFail;
                            SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                                     GetFaultCode(faultTag), GetFaultName(faultTag),
                                                     GetFaultDescription(faultTag));
                        }
                    }
                    else
                    {
                        testStatus = testFail;
                        SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                                 GetFaultCode(faultTag), GetFaultName(faultTag),
                                                 GetFaultDescription(faultTag));
                    }
                }
                else
                {
                    testStatus = testPass;
                    SendTestResultWithDetail(testStatus, description, code, "Performance Type", performanceType);
                }
            }
            else
            {
                testStatus = testFail;
                SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
                                         GetFaultCode(faultTag), GetFaultName(faultTag),
                                         GetFaultDescription(faultTag));
            }
        }
        catch(ModuleException& caughtModuleException)
        {
            Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
                caughtModuleException.message().c_str());
            testStatus = testFail;
        }
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::LearnPerformanceType(), status=%s\n", testStatus.c_str());
    // Return the test result
    return(testStatus);
}

//-----------------------------------------------------------------------------
template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::TestStepDelay(void)
{
    string testStatus = testPass;

    Log(LOG_FN_ENTRY, "Enter BoschABSTC::Delay()\n");
    if(ShortCircuitTestStep())
    {
        Log(LOG_FN_ENTRY, "Skipping test step BoschABSTC::Delay()");
        testStatus = testSkip;
    }
    else
    {
        BposSleep(GetTestStepInfoInt("DelayTime"));
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::Delay(), status=%s\n", testStatus.c_str());
    // Return the test result
    return(testStatus);
} 

template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::CheckPartNumber(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string modulePartNumber;
    string partNumberParameter = GetParameter("ModulePartNumber");
    vector<UINT8> partNumberChars;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter BoschABSTC::CheckPartNumber()\n");
    if (!ShortCircuitTestStep())
    {                       // Do not need to skip
        try
        {                    // Read the part number from the module
            moduleStatus = m_vehicleModule.ReadModuleData(GetDataTag("ReadModulePartNumber"), partNumberChars);
            try
            {
                Log(LOG_DEV_DATA, "Got back part number with %d characters", partNumberChars.size());
                for(int ii = 0; ii < partNumberChars.size(); ii++)
                {
                    Log(LOG_DEV_DATA, "Adding char %c to part number string", partNumberChars.at(ii));
                    modulePartNumber.push_back((char)partNumberChars.at(ii));
                }
            }
            catch(...)
            {
                Log(LOG_ERRORS, "Error adding characters to module part number string. Current string: %s", modulePartNumber.c_str());
            }
            // Check the status of the data
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {
                if(GetParameterBool("BypassPartNumberValidation"))
                {   // Skip Part number verification
                    Log(LOG_DEV_DATA, "Part number verification bypassed by parameter\n");
                    testResult = testPass;
                }
                else if (GetParameterBool("ValidatePartNumberList"))
                {   // Verify the module part number against a list of valid part numbers
                    XmlNodeMapItr iter = m_validPartNumbers.find("PN"+modulePartNumber);
                    testResult = iter != m_validPartNumbers.end() ? testPass : testFail;
                    partNumberParameter = iter != m_validPartNumbers.end() ? iter->second->getValue() : "Not Listed";
                }
                else if(GetParameterBool("PartNumberCompareByPosition"))
                {//check positions individually
                    testResult = testPass;
                    Log(LOG_DEV_DATA, "Iterate through positions to check\n");
                    for(XmlNodeMapItr iter = m_partNumberPositionComparison.begin();
                       (iter != m_partNumberPositionComparison.end());
                       iter++)
                    {   // Send individual details
                        try
                        {
                            char broadcastPNChar = partNumberParameter[atoi(iter->second->getAttribute("BroadcastPNCharPos")->getValue().c_str())];
                            char modulePNChar = modulePartNumber[atoi(iter->second->getAttribute("ModulePNCharPos")->getValue().c_str())];
                            Log(LOG_DEV_DATA, "broadcastChar = [%c] moduleChar = [%c]\n",broadcastPNChar,modulePNChar);
                            if(broadcastPNChar != modulePNChar)
                            {
                                testResult = testFail;
                                break;
                            }
                        }
                        catch(XmlException &ex)
                        {
                            Log(LOG_ERRORS, "XML Error getting part number characters %s: %s\n", GetProcessName().c_str(), ex.what());
                        }
                        Log(LOG_DEV_DATA, "Reporting Module data: %s - Value: %s\n",
                            iter->second->getName().c_str(), iter->second->getValue().c_str());
                    }

                }
                else if (modulePartNumber == partNumberParameter)
                {              // Part numbers match, test passes
                    testResult = testPass;
                }
                else
                {              // Part number do not match, test fails
                    testResult = testFail;
                }
                // Log the data
                Log(LOG_DEV_DATA, "Part Number Verification: %s - Parameter: %s, Module: %s\n",
                    testResult.c_str(), partNumberParameter.c_str(), modulePartNumber.c_str());
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("PartNumberMismatch"));
                testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("PartNumberMismatch"));
            }
            else
            {                 // Error getting data from the module
                SetCommunicationFailure(true);
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error reading module part number - status: %s\n", 
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "ModulePartNumber", modulePartNumber, "",
                                 "PartNumberParameter", partNumberParameter, "");
    }
    else
    {                       // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit BoschABSTC::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}

template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::LFSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    float firstRollSpeeds[4];
    float secondRollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // delay time before starting the roll
    UINT32 startDelay = GetParameterInt("SensorTestStartDelay");
    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "BoschABSTC::LFSensorTest - Enter\n");

    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping LFSensorTest()");
    }
    else if(GetParameterBool("SkipPassedSensorRetest") && IsRetest() && ((GetTestStepResult() == testPass) || (GetTestStepResult() == testSkip)))
    {
        Log(LOG_FN_ENTRY, "SkipPassedSensorRetest=%d, skipping LFSensorTest() using previous status, %s\n", GetParameterBool("SkipPassedSensorRetest"),GetTestStepResult().c_str());
        testResult = GetTestStepResult();
    }
    else
    {
        try
        {
            UpdatePrompts();        // "Shift to neutral and release brake"
            BposSleep(startDelay);     // give driver time to shift
    
            if(!SystemRead(MACHINE_TYPE).compare("3700"))
            {
                // Store the oringinal drive axle
                string driveAxle(SystemRead(DRIVE_AXLE_TAG));
                OriginalDriveAxle(&driveAxle);
                // Switch the drive axle, but make sure we believe in the real drive axle
                SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
                BposSleep(1000);
                SetData(DRIVE_AXLE_TAG, OriginalDriveAxle());
                // Command all rolls to speed mode
                m_MotorController.Write(std::string("LeftFrontMotorMode"),std::string("Speed"), false);
                m_MotorController.Write(std::string("RightFrontMotorMode"),std::string("Speed"), false);
                // Command the left front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), true);
            }
            else
            {   // Command all rolls to speed mode
                m_MotorController.Write(std::string("LeftFrontMotorMode"),std::string("Speed"), false);
                m_MotorController.Write(std::string("RightFrontMotorMode"),std::string("Speed"), false);
                m_MotorController.Write(std::string("LeftRearMotorMode"),std::string("Speed"), false);
                m_MotorController.Write(std::string("RightRearMotorMode"),std::string("Speed"), false);
                // Command the left front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), false);
                m_MotorController.Write(std::string("LeftRearSpeedValue"),std::string("0"), false);
                m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);
            }
    
            std::string tag, value;
            while (m_MotorController.GetNext(tag, value) > 0)
            {
                Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
            }
    
            // wait for the left front roll to reach the sensor test speed
            rollSpeeds[ 0] = 0;
            while (rollSpeeds[0] < (sensorTestSpeedVal))
            {
                BposSleep(100);
                m_baseBrakeTool->GetISpeeds(rollSpeeds);
                if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
            }

            BposSleep(500);
            m_baseBrakeTool->GetISpeeds(firstRollSpeeds);
    
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
    
            m_baseBrakeTool->GetISpeeds(secondRollSpeeds);

            // Average first and second roll speed readings
            for (int i = 0; i < 4; i++)
            {
                rollSpeeds[i] = (firstRollSpeeds[i] + secondRollSpeeds[i]) / 2.0;
            }

            if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
            {
                // check front sensor cross side to side
                if ((moduleSpeeds[LFWHEEL] <= moduleSpeeds[RFWHEEL]) && !GetParameterBool("WSSToleranceOnly"))
                {
                    testResult = testFail;
                }
                else if(GetParameterBool("CombinedWSSTests") || GetParameterBool("WSSToleranceOnly"))
                {
                    float upperLimit = rollSpeeds[LFWHEEL] + ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[LFWHEEL]);
                    float lowerLimit = rollSpeeds[LFWHEEL] - ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[LFWHEEL]);
                    testResult = ((moduleSpeeds[LFWHEEL] < upperLimit) &&
                                  (moduleSpeeds[LFWHEEL] > lowerLimit) ?
                                  testPass : testFail);
                    m_lfWssPass = true;
                    Log(LOG_DEV_DATA, "Sensor test: %s - sensor speed: %.2f, upper limit: %.2f, lower limit: %.2f - Result: %s",
                        rollerName[LFWHEEL].c_str(), moduleSpeeds[LFWHEEL], upperLimit, lowerLimit,
                        testResult.c_str());

                    string description = "";
                    if(!m_lfWssPass) description = "Left Front WSS speed out of tolerance";

                    char buff[16];
                    SendSubtestResultWithDetail("LFSensorToleranceTest", testResult, description, "", 
                                                "RollSpeed", CreateMessage(buff, sizeof(buff), "%.2f", rollSpeeds[LFWHEEL]), "", 
                                                "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[LFWHEEL]), "", 
                                                "UpperLimit", CreateMessage(buff, sizeof(buff), "%.2f", upperLimit), "", 
                                                "LowerLimit",CreateMessage(buff, sizeof(buff), "%.2f", lowerLimit), "");
                }
                else testResult   = testPass;
            }
            else                 // error reading the sensors
            {
                SetCommunicationFailure(true);     // set comm fault flag
                testResult = testFail;
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch (ModuleException &excpt)
        {
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
    
        if (testResult == testPass)    // if the test step passed
        {
            SendTestResult(testPass,testDescription);
        }
        else                    // test step failed
        {
            if (GetCommunicationFailure() == false)   // if no comm error
            {
                SendTestResultWithDetail(testResult,testDescription,
                                         GetFaultCode(testDescription),
                                         "LeftFrontSensorFail",
                                         GetFaultDescription(testDescription));
            }
        }
    }

    Log(LOG_DEV_DATA, "BoschABSTC::LFSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::RFSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    float firstRollSpeeds[4];
    float secondRollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "BoschABSTC::RFSensorTest - Enter\n");

    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping RFSensorTest()");
    }
    else if(GetParameterBool("SkipPassedSensorRetest") && IsRetest() && ((GetTestStepResult() == testPass) || (GetTestStepResult() == testSkip)))
    {
        Log(LOG_FN_ENTRY, "SkipPassedSensorRetest=%d, skipping RFSensorTest() using previous status, %s\n", GetParameterBool("SkipPassedSensorRetest"),GetTestStepResult().c_str());
        testResult = GetTestStepResult();
    }
    else
    {
        try
        {
            UpdatePrompts();        // "Shift to neutral and release brake"
    
            if(!SystemRead(MACHINE_TYPE).compare("3700"))
            {
                // Command the right front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, true);
            }
            else
            {
                // Command the right front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("LeftRearSpeedValue"),std::string("0"), false);
                m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);
            }
    
            std::string tag, value;
            while (m_MotorController.GetNext(tag, value) > 0)
            {
                Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
            }
    
            // wait for the right front roll to reach the sensor test speed
            rollSpeeds[ 1] = 0;
            while (rollSpeeds[1] < (sensorTestSpeedVal))
            {
                BposSleep(100);
                m_baseBrakeTool->GetISpeeds(rollSpeeds);
                if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
            }

            BposSleep(500);
            m_baseBrakeTool->GetISpeeds(firstRollSpeeds);
    
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
    
            m_baseBrakeTool->GetISpeeds(secondRollSpeeds);

            // Average first and second roll speed readings
            for (int i = 0; i < 4; i++)
            {
                rollSpeeds[i] = (firstRollSpeeds[i] + secondRollSpeeds[i]) / 2.0;
            }
            m_baseBrakeTool->GetISpeeds(firstRollSpeeds);
    
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
    
            m_baseBrakeTool->GetISpeeds(secondRollSpeeds);

            // Average first and second roll speed readings
            for (int i = 0; i < 4; i++)
            {
                rollSpeeds[i] = (firstRollSpeeds[i] + secondRollSpeeds[i]) / 2.0;
            }
    
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
    
            if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
            {
                // check front to rear sensor cross
                if (((moduleSpeeds[RFWHEEL] <= moduleSpeeds[RRWHEEL]) ||
                    (moduleSpeeds[RFWHEEL] <= moduleSpeeds[LRWHEEL])) && !GetParameterBool("WSSToleranceOnly"))
                {
                    testResult = testFail;
                }
                else if(GetParameterBool("CombinedWSSTests") || GetParameterBool("WSSToleranceOnly"))
                {
                    float upperLimit = rollSpeeds[RFWHEEL] + ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[RFWHEEL]);
                    float lowerLimit = rollSpeeds[RFWHEEL] - ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[RFWHEEL]);
                    testResult = ((moduleSpeeds[RFWHEEL] < upperLimit) &&
                                  (moduleSpeeds[RFWHEEL] > lowerLimit) ?
                                  testPass : testFail);
                    m_rfWssPass = true;
                    Log(LOG_DEV_DATA, "Sensor test: %s - sensor speed: %.2f, upper limit: %.2f, lower limit: %.2f - Result: %s",
                        rollerName[RFWHEEL].c_str(), moduleSpeeds[RFWHEEL], upperLimit, lowerLimit,
                        testResult.c_str());

                    string description = "";
                    if(!m_rfWssPass) description = "Right Front WSS speed out of tolerance";

                    char buff[16];
                    SendSubtestResultWithDetail("RFSensorToleranceTest", testResult, description, "", 
                                                "RollSpeed", CreateMessage(buff, sizeof(buff), "%.2f", rollSpeeds[RFWHEEL]), "", 
                                                "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[RFWHEEL]), "", 
                                                "UpperLimit", CreateMessage(buff, sizeof(buff), "%.2f", upperLimit), "", 
                                                "LowerLimit",CreateMessage(buff, sizeof(buff), "%.2f", lowerLimit), "");
                }
                else testResult   = testPass;
            }
            else                 // error reading the sensors
            {
                SetCommunicationFailure(true);     // set comm fault flag
                testResult = testFail;
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch (ModuleException &excpt)
        {
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
    
        if (testResult == testPass)    // if the test step passed
        {
            SendTestResult(testPass,testDescription);
        }
        else                    // test step failed
        {
            if (GetCommunicationFailure() == false)   // if no comm error
            {
                SendTestResultWithDetail(testResult,testDescription,
                                         GetFaultCode(testDescription),
                                         "RightFrontSensorFail",
                                         GetFaultDescription(testDescription));
            }
        }
    }
    Log(LOG_DEV_DATA, "BoschABSTC::RFSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::LRSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    float firstRollSpeeds[4];
    float secondRollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "BoschABSTC::LRSensorTest - Enter\n");

    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping LRSensorTest()");
    }
    else if(GetParameterBool("SkipPassedSensorRetest") && IsRetest() && ((GetTestStepResult() == testPass) || (GetTestStepResult() == testSkip)))
    {
        Log(LOG_FN_ENTRY, "SkipPassedSensorRetest=%d, skipping LRSensorTest() using previous status, %s\n", GetParameterBool("SkipPassedSensorRetest"),GetTestStepResult().c_str());
        testResult = GetTestStepResult();
    }
    else
    {
        try
        {
            UpdatePrompts();        // "Shift to neutral and release brake"
        
            if(!SystemRead(MACHINE_TYPE).compare("3700"))
            {
                // Switch the drive axle - set to zero speed first so we do not trip the drives
                m_MotorController.Write("LeftFrontSpeedValue", "0", false);
                m_MotorController.Write("RightFrontSpeedValue", "0", true);
                BposSleep(2000);
                SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
                BposSleep(1000);   //Wait for the relay to switch
                SetData(DRIVE_AXLE_TAG, OriginalDriveAxle());
                // Command all rolls to speed mode
                m_MotorController.Write(std::string("LeftFrontMotorMode"),std::string("Speed"), false);
                m_MotorController.Write(std::string("RightFrontMotorMode"),std::string("Speed"), false);
                // Command the left front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), true);
            }
            else
            {
                // Command the left rear roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("LeftRearSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);
            }
        
            std::string tag, value;
            while (m_MotorController.GetNext(tag, value) > 0)
            {
                Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
            }
        
            // wait for the left rear roll to reach the sensor test speed
            rollSpeeds[ 2] = 0;
            while (rollSpeeds[2] < (sensorTestSpeedVal))
            {
                BposSleep(100);
                m_baseBrakeTool->GetISpeeds(rollSpeeds);
                if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
            }

            BposSleep(500);
            m_baseBrakeTool->GetISpeeds(firstRollSpeeds);
    
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
    
            m_baseBrakeTool->GetISpeeds(secondRollSpeeds);

            // Average first and second roll speed readings
            for (int i = 0; i < 4; i++)
            {
                rollSpeeds[i] = (firstRollSpeeds[i] + secondRollSpeeds[i]) / 2.0;
            }
        
            // Ask the module for sensor speeds
            status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
        
            if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
            {
                // check rear sensor cross side to side
                if ((moduleSpeeds[LRWHEEL] <= moduleSpeeds[RRWHEEL]) && !GetParameterBool("WSSToleranceOnly"))
                {
                    testResult = testFail;
                }
                else if(GetParameterBool("CombinedWSSTests") || GetParameterBool("WSSToleranceOnly"))
                {
                    float upperLimit = rollSpeeds[LRWHEEL] + ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[LRWHEEL]);
                    float lowerLimit = rollSpeeds[LRWHEEL] - ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[LRWHEEL]);
                    testResult = ((moduleSpeeds[LRWHEEL] < upperLimit) &&
                                  (moduleSpeeds[LRWHEEL] > lowerLimit) ?
                                  testPass : testFail);
                    m_lrWssPass = true;
                    Log(LOG_DEV_DATA, "Sensor test: %s - sensor speed: %.2f, upper limit: %.2f, lower limit: %.2f - Result: %s",
                        rollerName[LRWHEEL].c_str(), moduleSpeeds[LRWHEEL], upperLimit, lowerLimit,
                        testResult.c_str());

                    string description = "";
                    if(!m_lrWssPass) description = "Left Rear WSS speed out of tolerance";

                    char buff[16];
                    SendSubtestResultWithDetail("LRSensorToleranceTest", testResult, description, "", 
                                                "RollSpeed", CreateMessage(buff, sizeof(buff), "%.2f", rollSpeeds[LRWHEEL]), "", 
                                                "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[LRWHEEL]), "", 
                                                "UpperLimit", CreateMessage(buff, sizeof(buff), "%.2f", upperLimit), "", 
                                                "LowerLimit",CreateMessage(buff, sizeof(buff), "%.2f", lowerLimit), "");
                }
                else testResult   = testPass;
            }
            else                 // error reading the sensors
            {
                SetCommunicationFailure(true);     // set comm fault flag
                testResult = testFail;
                testDescription = GetFaultDescription("CommunicationFailure");
            }
        }
        catch (ModuleException &excpt)
        {
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        
        if (testResult == testPass)    // if the test step passed
        {
            SendTestResult(testPass,testDescription);
        }
        else                    // test step failed
        {
            if (GetCommunicationFailure() == false)   // if no comm error
            {
                SendTestResultWithDetail(testResult,testDescription,
                                         GetFaultCode(testDescription),
                                         "LeftRearSensorFail",
                                         GetFaultDescription(testDescription));
            }
        }
    }

    Log(LOG_DEV_DATA, "BoschABSTC::LRSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class ModuleInterface>
string BoschABSTC<ModuleInterface>::RRSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    float firstRollSpeeds[4];
    float secondRollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "BoschABSTC::RRSensorTest - Enter\n");

    if(ShortCircuitTestStep())
    {
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping RRSensorTest()");
    }
    else if(GetParameterBool("SkipPassedSensorRetest") && IsRetest() && ((GetTestStepResult() == testPass) || (GetTestStepResult() == testSkip)))
    {
        Log(LOG_FN_ENTRY, "SkipPassedSensorRetest=%d, skipping RRSensorTest() using previous status, %s\n", GetParameterBool("SkipPassedSensorRetest"),GetTestStepResult().c_str());
        testResult = GetTestStepResult();
    }
    else
    {
        try
        {
            UpdatePrompts();        // "Shift to neutral and release brake"
    
            if(!SystemRead(MACHINE_TYPE).compare("3700"))
            {
                // Command the right front roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, true);
            }
            else
            {
                // Command the right rear roll to the sensor test speed
                m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("LeftRearSpeedValue"),sensorTestSpeed, false);
                m_MotorController.Write(std::string("RightRearSpeedValue"),sensorTestSpeed, true);
            }
    
            std::string tag, value;
            while (m_MotorController.GetNext(tag, value) > 0)
            {
                Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
            }
    
            // wait for the right rear roll to reach the sensor test speed
            rollSpeeds[ 3] = 0;
            while (rollSpeeds[3] < (sensorTestSpeedVal))
            {
                BposSleep(100);
                m_baseBrakeTool->GetISpeeds(rollSpeeds);
                if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)
                {
                    testResult = testFail;
                    break;
                }
            }

            BposSleep(500);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
    
            // if the test result was not set to failed, now set to pass
            if (testResult == BEP_TESTING_STATUS) testResult = testPass;
    
            // Ask the module for sensor speeds
            if(GetParameterBool("CombinedWSSTests") || GetParameterBool("WSSToleranceOnly")) 
            {
                m_baseBrakeTool->GetISpeeds(firstRollSpeeds);
        
                // Ask the module for sensor speeds
                status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
        
                m_baseBrakeTool->GetISpeeds(secondRollSpeeds);
    
                // Average first and second roll speed readings
                for (int i = 0; i < 4; i++)
                {
                    rollSpeeds[i] = (firstRollSpeeds[i] + secondRollSpeeds[i]) / 2.0;
                }

                if(status == BEP_STATUS_SUCCESS)
                {
                    float upperLimit = rollSpeeds[RRWHEEL] + ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[RRWHEEL]);
                    float lowerLimit = rollSpeeds[RRWHEEL] - ((GetParameterFloat("SensorSpeedTolerance")/100.0) * rollSpeeds[RRWHEEL]);
                    testResult = ((moduleSpeeds[RRWHEEL] < upperLimit) &&
                                  (moduleSpeeds[RRWHEEL] > lowerLimit) ?
                                  testPass : testFail);
                    m_rrWssPass = true;
                    Log(LOG_DEV_DATA, "Sensor test: %s - sensor speed: %.2f, upper limit: %.2f, lower limit: %.2f - Result: %s",
                        rollerName[RRWHEEL].c_str(), moduleSpeeds[RRWHEEL], upperLimit, lowerLimit,
                        testResult.c_str());
    
                    string description = "";
                    if(!m_rrWssPass) description = "Right Rear WSS speed out of tolerance";
    
                    char buff[16];
                    SendSubtestResultWithDetail("RRSensorToleranceTest", testResult, description, "", 
                                                "RollSpeed", CreateMessage(buff, sizeof(buff), "%.2f", rollSpeeds[RRWHEEL]), "", 
                                                "SensorSpeed", CreateMessage(buff, sizeof(buff), "%.2f", moduleSpeeds[RRWHEEL]), "", 
                                                "UpperLimit", CreateMessage(buff, sizeof(buff), "%.2f", upperLimit), "", 
                                                "LowerLimit",CreateMessage(buff, sizeof(buff), "%.2f", lowerLimit), "");
    
                    string wssqResult = (m_lfWssPass && m_rfWssPass && m_lrWssPass && m_rrWssPass) ? testPass : testFail;
                    SendSubtestResultWithDetail("SensorQualityTest", wssqResult, "Test Sensor Quality", "",
                                                "LfSensorResult", m_lfWssPass ? testPass : testFail, "",
                                                "RfSensorResult", m_rfWssPass ? testPass : testFail, "",
                                                "LrSensorResult", m_lrWssPass ? testPass : testFail, "",
                                                "RrSensorResult", m_rrWssPass ? testPass : testFail, "");
                }
                else                 // error reading the sensors
                {
                    SetCommunicationFailure(true);     // set comm fault flag
                    testResult = testFail;
                    testDescription = GetFaultDescription("CommunicationFailure");
                }
            }
            
        }
        catch (ModuleException &excpt)
        {
            Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
    
        if (testResult == testPass)    // if the test step passed
        {
            SendTestResult(testPass,testDescription);
        }
        else                    // test step failed
        {
            SendTestResultWithDetail(testResult,testDescription,
                                     GetFaultCode(testDescription),
                                     "RightRearSensorFail",
                                     GetFaultDescription(testDescription));
        }
    
        if(GetParameterBool("ResetAxleAfterIndividualSensorTest"))
        {
            // command the drives to zero torque    
            Log(LOG_DEV_DATA, "commanding torque to zero\n");
            SystemCommand(COMMAND_TORQUE, 0);    
        
            // command the drives to zero speed 
            Log(LOG_DEV_DATA, "commanding speed to zero\n");
            SystemCommand(COMMAND_SPEED, 0);
        
            if(!SystemRead(MACHINE_TYPE).compare("3700"))
            {
                Log(LOG_DEV_DATA, "Returning DriveAxle to %s\n",OriginalDriveAxle().c_str());
                SystemWrite(DRIVE_AXLE_TAG, OriginalDriveAxle());
                BposSleep(500);
            }
            // Set motors back to zero speed
            m_MotorController.Write("LeftFrontMotorMode", BOOST_MODE, false);
            m_MotorController.Write("RightFrontMotorMode", BOOST_MODE, false);
            m_MotorController.Write("LeftRearMotorMode", BOOST_MODE, false);
            m_MotorController.Write("RightRearMotorMode", BOOST_MODE, false);
            m_MotorController.Write("LeftFrontSpeedValue", "0", false);
            m_MotorController.Write("RightFrontSpeedValue", "0", false);
            m_MotorController.Write("LeftRearSpeedValue", "0", false);
            m_MotorController.Write("RightRearSpeedValue", "0", true);
    
            RemovePrompts();
        }
    }

    Log(LOG_DEV_DATA, "BoschABSTC::RRSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

