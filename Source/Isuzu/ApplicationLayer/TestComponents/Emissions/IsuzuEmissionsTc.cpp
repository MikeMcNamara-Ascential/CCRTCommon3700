//******************************************************************************
// Description:
// Test steps specific to Isuzu Emissions testing.
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
#include "IsuzuEmissionsTc.h"
#include <fstream.h>
#include <dirent.h>

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuEmissionsTc<ModuleType>::IsuzuEmissionsTc() : GenericEmissionsTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
IsuzuEmissionsTc<ModuleType>::~IsuzuEmissionsTc()
{   // Empty the switch monitors if any were created
    for (BackgroundSwitchMonitorVectorItr iter = m_switchMonitors.begin();
        iter != m_switchMonitors.end(); iter++)
    {
        if (*iter != NULL)  delete *iter;
        *iter = NULL;
    }
    m_switchMonitors.clear();
    // Clear the monitored switches
    m_switchMonitorItems.clear(true);

    // Empty the pid range check monitors if any were created
    for (BackgroundRangeCheckMonitorVectorItr iter = m_rangeCheckMonitors.begin();
        iter != m_rangeCheckMonitors.end(); iter++)
    {
        if (*iter != NULL)  delete *iter;
        *iter = NULL;
    }
    m_rangeCheckMonitors.clear();
    // Clear the monitored switches
    m_rangeCheckMonitorItems.clear(true);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string IsuzuEmissionsTc<ModuleType>::CommandTestStep(const string &value)
{
    string testResult(BEP_TESTING_STATUS);
    // Make sure it is OK to perform this test
    if (BEP_STATUS_SUCCESS == StatusCheck())
    {   // Perform the current test step
        if (GetTestStepResult().compare(testPass) || GetTestStepInfoBool("AlwaysPerform"))
        {   // Only perform the step if it did not pass
            if (!GetTestStepName().compare("AcceleratorPedalWOTTest"))                  testResult = AcceleratorPedalWOTTest();
            else if (!GetTestStepName().compare("CheckAcStatus"))                       testResult = CheckAirConditionCompressor();
            else if (!GetTestStepName().compare("CheckBarometricPressure"))             testResult = CheckSensorRangeUint8(value);
            else if (!GetTestStepName().compare("CheckBatteryVoltage"))                 testResult = CheckSensorRangeFloat(value);
            else if (!GetTestStepName().compare("CheckCASELearn"))                      testResult = CheckCASELearn();
            else if (!GetTestStepName().compare("CheckEgrError"))                       testResult = CheckSensorRangeFloat(value);
            else if (!GetTestStepName().compare("CheckEngineCoolantTemperature"))       testResult = CheckSensorRangeInt(value);
            else if (!GetTestStepName().compare("CheckEngineFan"))                      testResult = CheckEngineFan();
            else if (!GetTestStepName().compare("CheckEngineOilLevelLow"))              testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckEngineOilPressure"))              testResult = CheckSensorRangeUint8(value);
            else if (!GetTestStepName().compare("CheckEngineOilPressureLow"))           testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckEngineOilTemperature"))           testResult = CheckSensorRangeInt(value);
            else if (!GetTestStepName().compare("CheckFuelLevelPrimaryTankLevel"))      testResult = CheckSensorRangeFloat(value);
            else if (!GetTestStepName().compare("CheckFuelLevelSecondaryTankLevel"))    testResult = CheckSensorRangeFloat(value);
            else if (!GetTestStepName().compare("CheckFuelRailPressure"))               testResult = CheckSensorRangeUint8(value);
            else if (!GetTestStepName().compare("CheckIdleFuelTrim"))                   testResult = CheckFuelTrim(true);
            else if (!GetTestStepName().compare("CheckIdleRPM"))                        testResult = CheckSensorRangeUint16(value);
            else if (!GetTestStepName().compare("CheckKnockSensor1Status"))             testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckKnockSensor2Status"))             testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckMAFResidual"))                    testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckManifoldAbsolutePressureAtIdle")) testResult = CheckSensorRangeAtIdleFloat(value);
            else if (!GetTestStepName().compare("CheckMAP1Residual"))                   testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckMAP2Residual"))                   testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckMassAirFlowAtIdle"))              testResult = CheckSensorRangeAtIdleFloat(value);
            else if (!GetTestStepName().compare("CheckMisfireCatalystFailureStatus"))   testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckMisfireEmissionsFailureStatus"))  testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckNeutralRPMOverspeed"))            testResult = CheckNeutralRPMOverspeed();
            else if (!GetTestStepName().compare("CheckFuelTankVaporPressure"))          testResult = CheckSensorRangeFloat(value);
            else if (!GetTestStepName().compare("CheckPedalPositionSensorsAgree"))      testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckSteadySpeedFuelTrim"))            testResult = CheckFuelTrim(false);
            else if (!GetTestStepName().compare("CheckThrottlePositionSensorsAgree"))   testResult = CheckSensorRangeBool(value);
            else if (!GetTestStepName().compare("CheckVehicleSpeedSensor"))             testResult = CheckVehicleSpeedSensor();
            else if (!GetTestStepName().compare("ReadFaultCount"))                      testResult = CheckFaultCount();
            else if (!GetTestStepName().compare("StartRangeCheckMonitors"))             testResult = StartRangeCheckMonitors();
            else if (!GetTestStepName().compare("StartSwitchMonitors"))                 testResult = StartSwitchMonitors();
            else if (!GetTestStepName().compare("StopSwitchMonitors"))                  testResult = StopSwitchMonitors();
            else if (!GetTestStepName().compare("StopRangeCheckMonitors"))              testResult = StopRangeCheckMonitors();
            else if (!GetTestStepName().compare("WaitForEngineTemperature"))            testResult = WaitForOperatingTemperature();
            else if (!GetTestStepName().compare("DelayBeforeDtcRead"))                  testResult = DelayBeforeDtcRead();
            else if (!GetTestStepName().compare("CheckOxygenSensors"))                  testResult = CheckOxygenSensors();
            else if (!GetTestStepName().compare("KeyOffEngineOffKeyOn"))                testResult = KeyOffEngineOffKeyOn();
            else if (!GetTestStepName().compare("ClearFaultsFinal"))                    testResult = ClearFaults();
            else if (!GetTestStepName().compare("CheckSerialNumber"))                   testResult = CheckSerialNumber();
            else  testResult = GenericEmissionsTCTemplate<ModuleType>::CommandTestStep(value);
        }
        else
        {
            Log(LOG_DEV_DATA, "%s already passed, not running again!", GetTestStepName().c_str());
            testResult = GetTestStepResult();
        }
        // Check if testing should abort on a failure
        if (GetParameterBool("AbortOnTestStepFailure") && GetTestStepInfoBool("AbortIfFailed") &&
            testResult.compare(testPass) && testResult.compare(testSkip))
        {   // Tell ourselves to abort and then tell the system to abort.
            Abort();
            SystemWrite(ABORT_DATA_TAG, string("1"));
        }
    }
    else
    {   // Not OK to perform the current test
        Log(LOG_ERRORS, "System status is not good, skipping %s", GetTestStepName().c_str());
        testResult = testSkip;
    }
    // Return the test result
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const INT32 IsuzuEmissionsTc<ModuleType>::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 status = BEP_STATUS_ERROR;
    switch (code)
    {
    case ISUZU_EMISSIONS_PULSE_CODE:
        {
            switch (value)
            {
            case SWITCH_MON_PULSE:
                {   // Check each of the switches that have been configured
                    bool allMonitorsAreComplete = true;
                    status = BEP_STATUS_SUCCESS;
                    for (UINT32 index = 0; (index < m_switchMonitors.size()) && (BEP_STATUS_SUCCESS == status); index++)
                    {   // Read the switch state and check if the monitor is complete
                        status = m_switchMonitors[index]->ReadCurrentSwitchState();
                        if (allMonitorsAreComplete)  allMonitorsAreComplete = m_switchMonitors[index]->SwitchMonitoringComplete();
                    }
                    // Check if switch monitoring should continue
                    if (allMonitorsAreComplete)
                    {
                        Log(LOG_DEV_DATA, "All switch monitors have completed, stopping switch monitor timer");
                        m_switchMonitorTimer.Stop();
                    }
                }
                break;

            case SENSOR_MON_PULSE:
                // Check each sensor that has been configured
                status = BEP_STATUS_SUCCESS;
                for (UINT32 index = 0; (index < m_rangeCheckMonitors.size()) && (BEP_STATUS_SUCCESS == status); index++)
                {   // Read the sensor data from the module
                    status = m_rangeCheckMonitors[index]->ReadCurrentSensorValue();
                }
                break;

            default:
                status = BEP_STATUS_SOFTWARE;
                Log(LOG_ERRORS, "Unhandled Isuzu Emissions Pulse Value - %d", value);
                break;
            }
        }
        break;

    default:
        status = BepServer::HandlePulse(code, value);
        break;
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void IsuzuEmissionsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericEmissionsTCTemplate<ModuleType>::InitializeHook(config);
    // Setup the switch monitor timer
    SetupSwitchMonitorTimer(config->getChild("Setup/Parameters/BackgroundSwitchMonitoring"));
    SetupSensorMonitorTimer(config->getChild("Setup/Parameters/BackgroundPidValueRangesMonitoring"));
    // Save switch monitor items
    m_switchMonitorItems.DeepCopy(config->getChild("Setup/Parameters/BackgroundSwitchMonitoring/MonitoredSwitches")->getChildren());
    // Save pid value range monitor items
    m_rangeCheckMonitorItems.DeepCopy(config->getChild("Setup/Parameters/BackgroundPidValueRangesMonitoring/MonitoredPidValueRanges")->getChildren());
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckAirConditionCompressor(void)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckAirConditionCompressor() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Prompt the operator to turn on the A/C
        DisplayPrompt(GetPromptBox("TurnOnAc"), GetPrompt("TurnOnAc"), GetPromptPriority("TurnOnAc"));
        // Monitor the AC status until it is complete
        bool acCheckComplete = false;
        BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
        do
        {   // Read the A/C status from the module
            status = m_vehicleModule.ReadModuleData("ReadAcStatus", acCheckComplete);
            // If the A/C check is not complete, then wait a bit before checking again
            if (!acCheckComplete)  BposSleep(GetTestStepInfoInt("ScanDelay"));
            // Keep checking until the A/C check is complete, or time expires
        } while (!acCheckComplete && TimeRemaining() && (BEP_STATUS_SUCCESS == status) && (BEP_STATUS_SUCCESS == StatusCheck()));
        // Determine the overall result
        result = acCheckComplete ? testPass : testFail;
        Log(LOG_DEV_DATA, "A/C Check Complete: %s", result.c_str());
        // Report the result
        SendTestResult(result, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping CheckAirConditionCompressor");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckAirConditionCompressor() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckFaultCount(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckFaultCount() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the current fault count from the module
        UINT8 faultCount = 0xFF;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
        if (BEP_STATUS_SUCCESS == status)
        {
            result = (faultCount == 0x00) ? testPass : testFail;
            Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
            // Determine if testing should be aborted
            if ((faultCount > 0) && GetParameterBool("AbortIfDTCPresent"))
            {
                Log(LOG_DEV_DATA, "DTCs present - aborting test!");
                SystemWrite(ABORT_DATA_TAG, true);
            }
        }
        else
        {
            Log(LOG_ERRORS, "Could not read fault count from the module: %s", ConvertStatusToResponse(status).c_str());
            result = testFail;
        }
        // Report the result
        char buff[8];
        SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000",
                                 "DtcCount", CreateMessage(buff, sizeof(buff), "%d", faultCount), "");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping CheckFaultCount");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckFaultCount() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeBool(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeBool(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the sensor from the module
        bool sensorValue = 0;
        bool valueToCompare = GetParameterBool(sensorName + "CompareValue");

        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName, sensorValue);
        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");
        if (BEP_STATUS_SUCCESS == status)
        {   // Good data from the module, evaluate against parameters
            testResult = (valueToCompare == sensorValue) ? testPass : testFail;

            Log(LOG_DEV_DATA, "%s value: %02X (%d) - compare to: %02X (%d) - result: %s",
                sensorName.c_str(), sensorValue, sensorValue, valueToCompare, valueToCompare, testResult.c_str());
            if (testResult.compare(testPass))
            {   // Sensor value out of range
                testDescription = GetFaultDescription(sensorName + "OutOfRange");
                testCode = GetFaultCode(sensorName + "OutofRangeHigh");                
            }
        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                sensorName.c_str(), ConvertStatusToResponse(status).c_str());
        }
        // Report the results
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", (sensorValue == true) ? "True" : "False", sensorValueUnits,
                                 "ValueToCompare", (valueToCompare == true) ? "True" : "False", sensorValueUnits);
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeBool(%s)", sensorName.c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeBool(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeInt(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeInt(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the sensor from the module
        INT32 sensorValue = 0;
        INT32 minValue = GetParameterInt("Minimum" + sensorName);
        INT32 maxValue = GetParameterInt("Maximum" + sensorName);
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");
        if (BEP_STATUS_SUCCESS == status)
        {   // Good data from the module, evaluate against parameters
            testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
            Log(LOG_DEV_DATA, "%s value: %d - range: %d - %d - result: %s",
                sensorName.c_str(), sensorValue, minValue, maxValue, testResult.c_str());
            if (testResult.compare(testPass))
            {   // Sensor value out of range
                if (sensorValue > maxValue)
                {   // Sensor reading out of range - high
                    testDescription = GetFaultDescription(sensorName + "OutofRangeHigh");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
                else
                {   // Sensor reading out of range - low
                    testDescription = GetFaultDescription(sensorName + "OutofRangeLow");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
            }
        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                sensorName.c_str(), ConvertStatusToResponse(status).c_str());
        }
        // Report the results
        char buffer[32];
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", CreateMessage(buffer, sizeof(buffer), "%d", sensorValue), sensorValueUnits,
                                 "MinParam", GetParameter("Minimum" + sensorName), sensorValueUnits,
                                 "MaxParam", GetParameter("Maximum" + sensorName), sensorValueUnits);
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeInt(%s)", sensorName.c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeInt(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeUint8(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeUint8(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the sensor from the module
        UINT8 sensorValue = 0;
        UINT8 minValue = GetParameterInt("Minimum" + sensorName);
        UINT8 maxValue = GetParameterInt("Maximum" + sensorName);
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");
        if (BEP_STATUS_SUCCESS == status)
        {   // Good data from the module, evaluate against parameters
            testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
            Log(LOG_DEV_DATA, "%s value: %02X (%d) - range: %02X (%d) - %02X (%d) - result: %s",
                sensorName.c_str(), sensorValue, sensorValue, minValue, minValue, maxValue, maxValue, testResult.c_str());
            if (testResult.compare(testPass))
            {   // Sensor value out of range
                if (sensorValue > maxValue)
                {   // Sensor reading out of range - high
                    testDescription = GetFaultDescription(sensorName + "OutofRangeHigh");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
                else
                {   // Sensor reading out of range - low
                    testDescription = GetFaultDescription(sensorName + "OutofRangeLow");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
            }
        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                sensorName.c_str(), ConvertStatusToResponse(status).c_str());
        }
        // Report the results
        char buffer[32];
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", CreateMessage(buffer, sizeof(buffer), "%d", sensorValue), sensorValueUnits,
                                 "MinParam", GetParameter("Minimum" + sensorName), sensorValueUnits,
                                 "MaxParam", GetParameter("Maximum" + sensorName), sensorValueUnits);
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeUint8(%s)", sensorName.c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeUint8(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeUint16(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeUint16(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the sensor from the module
        UINT16 sensorValue = 0;
        UINT16 minValue = GetParameterInt("Minimum" + sensorName);
        UINT16 maxValue = GetParameterInt("Maximum" + sensorName);
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");
        if (BEP_STATUS_SUCCESS == status)
        {   // Good data from the module, evaluate against parameters
            testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
            Log(LOG_DEV_DATA, "%s value: %04X (%d) - range: %04X (%d) - %04X (%d) - result: %s",
                sensorName.c_str(), sensorValue, sensorValue, minValue, minValue, maxValue, maxValue, testResult.c_str());
            if (testResult.compare(testPass))
            {   // Sensor value out of range
                if (sensorValue > maxValue)
                {   // Sensor reading out of range - high
                    testDescription = GetFaultDescription(sensorName + "OutofRangeHigh");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
                else
                {   // Sensor reading out of range - low
                    testDescription = GetFaultDescription(sensorName + "OutofRangeLow");
                    testCode = GetFaultCode(sensorName + "OutofRangeHigh");
                }
            }
        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                sensorName.c_str(), ConvertStatusToResponse(status).c_str());
        }
        // Report the results
        char buffer[32];
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", CreateMessage(buffer, sizeof(buffer), "%d", sensorValue), sensorValueUnits,
                                 "MinParam", GetParameter("Minimum" + sensorName), sensorValueUnits,
                                 "MaxParam", GetParameter("Maximum" + sensorName), sensorValueUnits);
    }
    else
    {   // Need to skip this test
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeUint16(%s)", sensorName.c_str());
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeUint16(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckVehicleSpeedSensor(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckVehicleSpeedSensor() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the sensor speed from the module
        float vehicleSpeed = 0.0;
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadVehicleSpeedSensor", vehicleSpeed);
        string code("0000");
        string desc(GetTestStepInfo("Description"));
        // Get the roller speed
        float rollerSpeed = GetRollSpeed();
        float minSpeed = rollerSpeed - GetParameterFloat("VehicleSpeedSensorTolerance");
        float maxSpeed = rollerSpeed + GetParameterFloat("VehicleSpeedSensorTolerance");
        if (BEP_STATUS_SUCCESS == status)
        {
            result = ((minSpeed <= vehicleSpeed) && (vehicleSpeed <= maxSpeed)) ? testPass : testFail;
            Log(LOG_DEV_DATA, "Vehicle Speed Sensor: %.2f,  roller: %.2f, min: %.2f, max: %.2f -- %s",
                vehicleSpeed, rollerSpeed, minSpeed, maxSpeed, result.c_str());
        }
        else
        {   // Could not read the vehicle speed from the module
            Log(LOG_ERRORS, "Could not read vehicle speed from the module: %s", ConvertStatusToResponse(status).c_str());
            code = GetFaultCode("CommunicationFailure");
            desc = GetFaultDescription("CommunicationFailure");
            result = testFail;
        }
        // Report the result
        char buff[16];
        SendTestResultWithDetail(result, code, desc,
                                 "VehicleSpeedSensor", CreateMessage(buff, sizeof(buff), "%.2f", vehicleSpeed), unitsMPH,
                                 "RollerSpeed", CreateMessage(buff, sizeof(buff), "%.2f", rollerSpeed), unitsMPH,
                                 "MinSpeed", CreateMessage(buff, sizeof(buff), "%.2f", minSpeed), unitsMPH,
                                 "MaxSpeed", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeed), unitsMPH);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping CheckVehicleSpeedSensor()");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckVehicleSpeedSensor() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeFloat(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeFloat(%s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {   // Read the current engine coolant temperature
        float sensorValue = 0.0;
        float minValue = GetParameterFloat("Minimum" + sensorName);
        float maxValue = GetParameterFloat("Maximum" + sensorName);
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");
        Log(LOG_DEV_DATA, "Read %s from the module: %.2f - status: %s", sensorName.c_str(), sensorValue,
            ConvertStatusToResponse(status).c_str());
        if (BEP_STATUS_SUCCESS == status)
        {   // Good data from the module, evaluate against parameters
            testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
            Log(LOG_DEV_DATA, "%s value: (%.2f) - range: (%.2f) - (%.2f) - result: %s",
                sensorName.c_str(), sensorValue, minValue, maxValue, testResult.c_str());
            if (testResult.compare(testPass))
            {   // Sensor value out of range
                if (sensorValue > maxValue)
                {   // Sensor reading out of range - high
                    testDescription = GetFaultDescription(sensorName + "OutOfRangeHigh");
                    testCode = GetFaultCode(sensorName + "OutOfRangeHigh");
                }
                else
                {   // Sensor reading out of range - low
                    testDescription = GetFaultDescription(sensorName + "OutOfRangeLow");
                    testCode = GetFaultCode(sensorName + "OutOfRangeLow");
                }
            }
        }
        else
        {   // Could not read the engine temperature from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                sensorName.c_str(), ConvertStatusToResponse(status).c_str());
        }
        // Report the result
        char buffer[32];
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", CreateMessage(buffer, sizeof(buffer), "%.2f", sensorValue), sensorValueUnits,
                                 "MinParam", GetParameter("Minimum" + sensorName), sensorValueUnits,
                                 "MaxParam", GetParameter("Maximum" + sensorName), sensorValueUnits);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeFloat(%s)", sensorName.c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeFloat(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadCurrentSwitchStates(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    bool allMonitorsAreComplete = true;
    for (BackgroundSwitchMonitorVectorItr iter = m_switchMonitors.begin();
        iter != m_switchMonitors.end(); iter++)
    {   // Read the current switch state if this switch has not completed
        if (!*iter.SwitchMonitoringComplete())
        {
            status = ReadCurrentSwitchState();
            if (!*iter.SwitchMonitoringComplete()) allMonitorsAreComplete = false;
        }
    }
    // Check if all monitors have completed
    if (allMonitorsAreComplete)
    {   // Disable the timer
        Log(LOG_DEV_DATA, "All switch monitoring complete, stopping timer");
        m_switchMonitorTimer.Stop();
    }
    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadEngineTemperature(INT32 &engineTemp)
{   // Reset the engine temperature to -1 if there is an error reading the data
    engineTemp = -1;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadEngineTemperature", engineTemp);

    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void IsuzuEmissionsTc<ModuleType>::SetupSensorMonitorTimer(const XmlNode *config)
{
    UINT64 updateRate = 0;
    try
    {
        updateRate = BposReadInt(config->getChild("SensorUpdateRate")->getValue().c_str());
        Log(LOG_DEV_DATA, "Sensor monitor update rate: %d", updateRate);
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Sensor monitor update rate not specified, setting update rate to 0 - %s", excpt.GetReason());
        updateRate = 0;
    }
    // Make sure we have a vaild update rate
    if (updateRate > 0)
    {
        m_sensorMonitorTimer.SetPulseCode(ISUZU_EMISSIONS_PULSE_CODE);
        m_sensorMonitorTimer.SetPulseValue(SENSOR_MON_PULSE);
        m_sensorMonitorTimer.Initialize(GetProcessName(), mSEC_nSEC(updateRate), mSEC_nSEC(updateRate));
        m_sensorMonitorTimer.Stop();
    }
    else
    {
        Log(LOG_ERRORS, "Invalid update rate, %d seconds.  Not starting sensor monitors", updateRate);
    }
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void IsuzuEmissionsTc<ModuleType>::SetupSwitchMonitorTimer(const XmlNode *config)
{
    UINT64 updateRate = 0;
    try
    {
        updateRate = BposReadInt(config->getChild("SwitchUpdateRate")->getValue().c_str());
        Log(LOG_DEV_DATA, "Switch monitor update rate: %d", updateRate);
    }
    catch (XmlException &excpt)
    {
        Log(LOG_ERRORS, "Switch monitor update rate not specified, setting update rate to 0 - %s", excpt.GetReason());
        updateRate = 0;
    }
    // Make sure we have a vaild update rate
    if (updateRate > 0)
    {
        m_switchMonitorTimer.SetPulseCode(ISUZU_EMISSIONS_PULSE_CODE);
        m_switchMonitorTimer.SetPulseValue(SWITCH_MON_PULSE);
        m_switchMonitorTimer.Initialize(GetProcessName(), mSEC_nSEC(updateRate), mSEC_nSEC(updateRate));
        m_switchMonitorTimer.Stop();
    }
    else
    {
        Log(LOG_ERRORS, "Invalid update rate, %d seconds.  Not starting switch monitors", updateRate);
    }
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::StartSwitchMonitors(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartSwitchMonitors() - Enter");
    string testResult(testPass);
    if (!ShortCircuitTestStep())
    {   // Create new switch monitors
        for (XmlNodeMapItr iter = m_switchMonitorItems.begin(); iter != m_switchMonitorItems.end(); iter++)
        {   // Create a new switch monitor
            Log(LOG_DEV_DATA, "Create Switch Monitor for %s", iter->second->getName().c_str());
            BackgroundSwitchMonitor *monitor = new BackgroundSwitchMonitor(iter->second->getName(),
                                                                           iter->second->getValue(),
                                                                           &m_vehicleModule, this);
            m_switchMonitors.push_back(monitor);
        }
        // Start the timer if there are switches to monitor
        if (m_switchMonitors.size() > 0)  m_switchMonitorTimer.Start();
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step IsuzuEmissionsTc::StartSwitchMonitors()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartSwitchMonitors() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::StopSwitchMonitors(void)
{   // Log the entery and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StopSwitchMonitors() - Enter");
    string result(BEP_TESTING_STATUS);
    if (!ShortCircuitTestStep())
    {   // Stop the switch monitor timer
        m_switchMonitorTimer.Stop();
        // Stop each switch monitor
        TestResultDetails details;
        for (UINT32 index = 0; index < m_switchMonitors.size(); index++)
        {   // Make sure both states have been detected
            string switchResult = (m_switchMonitors[index]->SwitchOnDetected() && 
                                   m_switchMonitors[index]->SwitchOffDetected()) ? testPass : testFail;
            Log(LOG_DEV_DATA, "Switch Monitor: %s - ON: %s, OFF: %s - Result: %s",
                m_switchMonitors[index]->SwitchName().c_str(), 
                (m_switchMonitors[index]->SwitchOnDetected() ? testPass : testFail).c_str(), 
                (m_switchMonitors[index]->SwitchOffDetected() ? testPass : testFail).c_str(), switchResult.c_str());
            // Update the overall status
            UpdateResult(switchResult, result);
            details.AddDetail(m_switchMonitors[index]->SwitchName(), switchResult, "");
            // Report the current switch result
            SendSubtestResultWithDetail(m_switchMonitors[index]->SwitchName(), switchResult, 
                                        "Switch status: " + m_switchMonitors[index]->SwitchName(), "0000",
                                        "On", m_switchMonitors[index]->SwitchOnDetected() ? testPass : testFail, "",
                                        "Off", m_switchMonitors[index]->SwitchOffDetected() ? testPass : testFail, "");
        }
        // Send the overall result
        SendTestResultWithDetail(result, details, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this step
        Log(LOG_FN_ENTRY, "Skipping StopSwitchMonitors()");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StopSwitchMonitors() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::StartCaseLearnSequence()
{   // Log the entry
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartCaseLearnSequence() - Enter");
    // Prompt the operator to shift to Park
    DisplayTimedPrompt(GetPrompt("ShiftToPark"), GetPromptBox("ShiftToPark"), GetPromptPriority("ShiftToPark"),
                       GetPromptDuration("ShiftToPark"));
    // Command the module to start the CASE learn process
    string result((m_vehicleModule.CommandModule("StartCaseLearn") == BEP_STATUS_SUCCESS) ? testPass : testFail);
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartCaseLearnSequence() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::StartRangeCheckMonitors()
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartRangeCheckMonitors() - Enter");
    string testResult(testPass);
    if (!ShortCircuitTestStep())
    {   // Create new range check monitors
        for (XmlNodeMapItr iter = m_rangeCheckMonitorItems.begin(); iter != m_rangeCheckMonitorItems.end(); iter++)
        {
            Log(LOG_DEV_DATA, "Create Sensor Range Check Monitor for %s", iter->second->getName().c_str());
            // Create a new switch monitor
            BackgroundRangeCheckMonitor *monitor = new BackgroundRangeCheckMonitor(iter->second->getName(),
                                                                                   iter->second->getValue(),
                                                                                   &m_vehicleModule, this);
            monitor->ReadCurrentSensorValue();
            m_rangeCheckMonitors.push_back(monitor);           
        }
        // If there were sensor monitors started, start the timer
        if (m_rangeCheckMonitors.size() > 0)   m_sensorMonitorTimer.Start();
        // Report the result
        SendTestResult(testResult, GetTestStepInfo("Description"), "0000");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step IsuzuEmissionsTc::StartRangeCheckMonitors()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StartRangeCheckMonitors() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::StopRangeCheckMonitors()
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StopRangeCheckMonitors() - Enter");
    string testResult(testPass);
    string testDescription(GetTestStepInfo("Description"));;
    string testCode("0000");

    if (!ShortCircuitTestStep())
    {
        TestResultDetails details;
        m_sensorMonitorTimer.Stop();
        for (UINT32 index = 0; index < m_rangeCheckMonitors.size(); index++)
        {   // Take the last reading
            m_rangeCheckMonitors[index]->ReadCurrentSensorValue();
            string sensorValueName(m_rangeCheckMonitors[index]->SensorName());
            // Tell it for verify with min delta from config
            float minDelta = GetParameterFloat("Minimum" + sensorValueName + "RangeDelta");
            string sensorResult = m_rangeCheckMonitors[index]->VerifySensorValues(minDelta);
            UpdateResult(sensorResult, testResult);
            details.AddDetail(sensorValueName, sensorResult, "");
            // Report the result
            char buff[32];
            SendSubtestResultWithDetail(sensorValueName, testResult, testDescription, testCode,
                                        "MinSensorValue", CreateMessage(buff, sizeof(buff), "%.2f", 
                                                                        m_rangeCheckMonitors[index]->MinSensorValue()), "",
                                        "MaxSensorValue", CreateMessage(buff, sizeof(buff), "%.2f", 
                                                                        m_rangeCheckMonitors[index]->MaxSensorValue()), "");
        }
        // Report the overall result
        SendTestResultWithDetail(testResult, details, testDescription, testCode);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step IsuzuEmissionsTc::StopRangeCheckMonitors()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::StopRangeCheckMonitors() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::WaitForOperatingTemperature(void)
{   // Log the entry and determine if this step should be performed
    string testResult(BEP_TESTING_RESPONSE);
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::WaitForOperatingTemperature() - Enter");
    if (!ShortCircuitTestStep())
    {   // Keep monitoring until engine temperature reaches operating temperature
        bool engineTempInRange = false;
        INT32 engineTemp = 0;
        INT32 minTemp = GetParameterInt("MinimumEngineCoolantTemperature");
        INT32 maxTemp = GetParameterInt("MaximumEngineCoolantTemperature");
        BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
        DisplayPrompt(GetPromptBox("WaitForEngineTemp"), GetPrompt("WaitForEngineTemp"), 
                      GetPromptPriority("WaitForEngineTemp"), "white", GetParameter("MinimumEngineCoolantTemperature"));
        do
        {   // Read the engine temperature from the module
            moduleStatus = ReadEngineTemperature(engineTemp);
            Log(LOG_DEV_DATA, "Current engine coolant temp: %d [%d  %d]", engineTemp, minTemp, maxTemp);
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Check if the engine coolant temperature is in range
                engineTempInRange = (minTemp <= engineTemp) && (engineTemp <= maxTemp);
                SystemWrite(GetDataTag("EngineCoolantTemp"), engineTemp);
                if (!engineTempInRange) BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
        } while ((BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus) &&
                 !engineTempInRange && (engineTemp <= maxTemp));
        RemovePrompt(GetPromptBox("WaitForEngineTemp"), GetPrompt("WaitForEngineTemp"), 
                     GetPromptPriority("WaitForEngineTemp"));
        // Determine the overall result
        testResult = engineTempInRange ? testPass : testAbort;
        string testDescription(GetTestStepInfo("Description"));;
        string testCode("0000");
        if (!engineTempInRange)
        {   // Figure out the correct fault message to report
            if (BEP_STATUS_SUCCESS != StatusCheck())
            {
                testDescription = GetFaultDescription("SystemStatus");
                testCode = GetFaultCode("SystemStatus");
            }
            else if (BEP_STATUS_SUCCESS != moduleStatus)
            {
                testDescription = GetFaultDescription("CommunicationFailure");
                testCode = GetFaultCode("CommunicationFailure");
            }
            else if (engineTemp < minTemp)
            {
                testDescription = GetFaultDescription("EngineTemperatureTooLow");
                testCode = GetFaultCode("EngineTemperatureTooLow");
                SystemWrite(ABORT_DATA_TAG, string("1"));
            }
            else if (engineTemp > maxTemp)
            {
                testDescription = GetFaultDescription("EngineTemperatureTooHigh");
                testCode = GetFaultCode("EngineTemperatureTooHigh");
                SystemWrite(ABORT_DATA_TAG, string("1"));
                DisplayTimedPrompt(GetPrompt("AbortEngineTemp"), GetPromptBox("AbortEngineTemp"), 
                                   GetPromptPriority("AbortEngineTemp"), GetPromptDuration("AbortEngineTemp"));
            }
        }
        // Report the result
        char buff[32];
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "EngineTemperature", CreateMessage(buff, sizeof(buff), "%d", engineTemp), unitsDEGC,
                                 "MinTemperature", GetParameter("MinimumEngineCoolantTemperature"), unitsDEGC,
                                 "MaxTemperature", GetParameter("MaximumEngineCoolantTemperature"), unitsDEGC);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step WaitForOperatingTemperature()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::WaitForOperatingTemperature() - Exit");
    return testResult;
}

template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadIsSwitchOn(const string &switchPidName, bool &isSwitchOn)
{   
    isSwitchOn = false;
    return m_vehicleModule.ReadModuleData("Read" + switchPidName + "Switch", isSwitchOn);
}

template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadIsModeOn(const string &modePidName, bool &isModeOn)
{   
    isModeOn = false;
    return m_vehicleModule.ReadModuleData("Read" + modePidName + "Mode", isModeOn);
}

template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadIsEngineRunning(bool &isEngineRunning)
{
    isEngineRunning = false;
    UINT16 engineRPM = 0;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineRPM);
    isEngineRunning = (engineRPM > GetParameterInt("MinimumEngineSpeed"));
    return status;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::AcceleratorPedalWOTTest()
{
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));;
    string testCode("0000");

    float acceleratorPedalPos1 = 0;
    float acceleratorPedalPos2 = 0;
    float minWOTPercent = GetParameterFloat("AcceleratorPedalWOTTestMin"); 
    float maxWOTPercent = GetParameterFloat("AcceleratorPedalWOTTestMax"); 

    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::AcceleratorPedalWOTTest() - Enter");
    if (!ShortCircuitTestStep())
    {
        bool isEngineRunning = false;
        BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
        do
        {   // Make sure the engine is not running
            status = ReadIsEngineRunning(isEngineRunning);
            // If the engine is running, prompt the operator to shift to N and turn engine off key on
            if (isEngineRunning && (BEP_STATUS_SUCCESS == status))
            {   // Prompt to turn engine off and key in on position
                DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), 
                              GetPromptPriority("ShiftToNeutral"));
                DisplayPrompt(GetPromptBox("TurnEngineOffKeyOn"), GetPrompt("TurnEngineOffKeyOn"), 
                              GetPromptPriority("TurnEngineOffKeyOn"));
                // Wait before the next check
                BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
            else
            {   // Remove prompt
                RemovePrompt(GetPromptBox("TurnEngineOffKeyOn"), 
                             GetPrompt("TurnEngineOffKeyOn"), GetPromptPriority("TurnEngineOffKeyOn"));
            }
            // Wait before the next check
            if (isEngineRunning)  BposSleep(GetTestStepInfoInt("ScanDelay"));
        }while (isEngineRunning && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && 
                (BEP_STATUS_SUCCESS == status));
        // If all is well
        if (!isEngineRunning && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status))
        {   // Prompt to push pedal to wot
            DisplayPrompt(GetPromptBox("PushAcceleratorPedalToFloor"), GetPrompt("PushAcceleratorPedalToFloor"), 
                          GetPromptPriority("PushAcceleratorPedalToFloor"));
            status = BEP_STATUS_ERROR;
            do
            {   // Read pedal position 1 and 2 to verify
                status = m_vehicleModule.ReadModuleData("ReadAcceleratorPedalSensor1", acceleratorPedalPos1);
                if (BEP_STATUS_SUCCESS == status)
                {   // Compare the two and also with 100% with some tolerance
                    if ( (acceleratorPedalPos1 >= minWOTPercent) && 
                         (acceleratorPedalPos1 <= maxWOTPercent) )
                    {   // Passed within tolerance so check next sensor
                        status = m_vehicleModule.ReadModuleData("ReadAcceleratorPedalSensor2", acceleratorPedalPos2);
                        if (BEP_STATUS_SUCCESS == status)
                        {   // Compare the two and also with 100% with some tolerance
                            if ( (acceleratorPedalPos2 >= minWOTPercent) && 
                                 (acceleratorPedalPos2 <= maxWOTPercent) )
                            {   // Passed within tolerance
                                testResult = testPass;
                            }
                            else
                            {
                                testResult = testFail;
                                Log(LOG_DEV_DATA,"Accelerator Position 2 - (%.2f) - did not meet tolerance - min (%.2f) max (%.2f)", 
                                    acceleratorPedalPos2, minWOTPercent, maxWOTPercent);
                            }
                        }
                        else
                        {   // Could not read from the module
                            testResult = testFail;
                            testDescription = GetFaultDescription("CommunicationFailure");
                            testCode = GetFaultCode("CommunicationFailure");
                            //Log(LOG_DEV_DATA, "Error reading Accelerator Pedal Position E from the module");
                            Log(LOG_DEV_DATA, "Error reading Accelerator Pedal Sensor 2 from the module");
                        }                    
                    }
                    else
                    {   // first pos didn't pass tolerance
                        testResult = testFail;
                        Log(LOG_DEV_DATA,"Accelerator Position 1 - (%.2f) - did not meet tolerance - min (%.2f) max (%.2f)", 
                            acceleratorPedalPos1, minWOTPercent, maxWOTPercent);
                    }
                }
                else
                {   // Could not read from the module
                    testResult = testFail;
                    testDescription = GetFaultDescription("CommunicationFailure");
                    testCode = GetFaultCode("CommunicationFailure");
                    //Log(LOG_DEV_DATA, "Error reading Accelerator Pedal Position D from the module");
                    Log(LOG_DEV_DATA, "Error reading Accelerator Pedal Sensor 1 from the module");
                }
            } while (TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && testResult.compare(testPass) &&
                     (BEP_STATUS_SUCCESS == status));
        }
        // Remove the prompts
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("PushAcceleratorPedalToFloor"), 
                     GetPrompt("PushAcceleratorPedalToFloor"), GetPromptPriority("PushAcceleratorPedalToFloor"));
        // Report the result
        char buff[32];
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "AccleratorPedalPosition1", CreateMessage(buff, sizeof(buff), "%.2f", acceleratorPedalPos1), "%",
                                 "AccleratorPedalPosition2", CreateMessage(buff, sizeof(buff), "%.2f", acceleratorPedalPos2), "%",
                                 "PedalPositionMin", CreateMessage(buff, sizeof(buff), "%.2f", minWOTPercent), "%",
                                 "PedalPositionMax", CreateMessage(buff, sizeof(buff), "%.2f", maxWOTPercent), "%");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step AcceleratorPedalWOTTest()");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::AcceleratorPedalWOTTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::ReadAndVerifySensor(string &sensorName, UINT8 &sensorValue)
{
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ReadAndVerifySensor(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription("Reading and Verifying "+sensorName+" sensor");
    char buffer[8];
    char bufferMin[8];
    char bufferMax[8];

    // Read the sensor from the module
    sensorValue = 0;
    UINT8 minValue = GetParameterInt("Minimum" + sensorName);
    UINT8 maxValue = GetParameterInt("Maximum" + sensorName);
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
    if (BEP_STATUS_SUCCESS == status)
    {   // Good data from the module, evaluate against parameters
        testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
        Log(LOG_DEV_DATA, "%s value: %02X (%d) - range: %02X (%d) - %02X (%d) - result: %s",
            sensorName.c_str(), sensorValue, sensorValue, minValue, minValue, maxValue, maxValue, testResult.c_str());            
    }
    else
    {   // Could not read data from the module
        testResult = testFail;
        Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
            sensorName.c_str(), ConvertStatusToResponse(status).c_str());
    }

    string sensorValueUnits(GetParameter(sensorName + "Units"));
    SendSubtestResultWithDetail(GetTestStepName()+"_"+sensorName, testResult, testDescription, "0000", 
                                "SensorReading", CreateMessage(buffer, sizeof(buffer), "%d", sensorValue), sensorValueUnits, 
                                "MinParam", CreateMessage(bufferMin, sizeof(bufferMin), "%d", minValue), sensorValueUnits,
                                "MaxParam", CreateMessage(bufferMax, sizeof(bufferMax), "%d", maxValue), sensorValueUnits);

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ReadAndVerifySensor(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::ReadAndVerifySensor(string &sensorName, float &sensorValue)
{
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ReadAndVerifySensor(sensorName: %s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription("Reading and Verifying "+sensorName+" sensor");
    char buffer[16];
    char bufferMin[16];
    char bufferMax[16];

    // Read the sensor from the module
    sensorValue = 0.0;
    float minValue = GetParameterFloat("Minimum" + sensorName);
    float maxValue = GetParameterFloat("Maximum" + sensorName);
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
    if (BEP_STATUS_SUCCESS == status)
    {   // Good data from the module, evaluate against parameters
        testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
        Log(LOG_DEV_DATA, "%s value: %.2f - range: %.2f - %.2f - result: %s",
            sensorName.c_str(), sensorValue, minValue, maxValue, testResult.c_str());            
    }
    else
    {   // Could not read data from the module
        testResult = testFail;
        Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
            sensorName.c_str(), ConvertStatusToResponse(status).c_str());
    }

    string sensorValueUnits(GetParameter(sensorName + "Units"));
    SendSubtestResultWithDetail(GetTestStepName()+"_"+sensorName, testResult, testDescription, "0000", 
                                "SensorReading", CreateMessage(buffer, sizeof(buffer), "%.2f", sensorValue), sensorValueUnits, 
                                "MinParam", CreateMessage(bufferMin, sizeof(bufferMin), "%.2f", minValue), sensorValueUnits,
                                "MaxParam", CreateMessage(bufferMax, sizeof(bufferMax), "%.2f", maxValue), sensorValueUnits);

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ReadAndVerifySensor(%s) - Exit", sensorName.c_str());
    return testResult;
}


//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckEngineFan()
{
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));;
    string testCode("0000");

    string actualFanSpeedTestResult(BEP_TESTING_RESPONSE);
    string desiredFanSpeedTestResult(BEP_TESTING_RESPONSE);

    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckEngineFan() - Enter");
    if (!ShortCircuitTestStep())
    {
        // Read and verify the actual fan speed with config min and max
        string actualFanSpeedName("ActualFanSpeed");
        UINT8 actualFanSpeedValue = 0; 
        string desiredFanSpeedName("DesiredFanSpeed");
        UINT8 desiredFanSpeedValue = 0;
        UINT8 toleranceRPM = GetParameterInt("FanSpeedToleranceRPM");

        actualFanSpeedTestResult = ReadAndVerifySensor(actualFanSpeedName, actualFanSpeedValue);

        if (actualFanSpeedTestResult.compare(testFail))
        {
            // No failure and verify of the actual speed sensor
            // Read and verify the desired fan speed with config min and max
            desiredFanSpeedTestResult = ReadAndVerifySensor(desiredFanSpeedName, desiredFanSpeedValue);

            if (desiredFanSpeedTestResult.compare(testFail))
            {
                // Compare the desired fan speed with the actual fan speed and compare with config tolerance
                if ( (actualFanSpeedValue >= (desiredFanSpeedValue - toleranceRPM)) && 
                     (actualFanSpeedValue <= (desiredFanSpeedValue + toleranceRPM)) )
                {
                    // Passed within tolerance
                    testResult = testPass;
                }
                else
                {
                    // Failed to be within tolerance
                    testResult = testFail;
                    testDescription = "Failed to find actual fan speed within tolerance of desired fan speed";
                    Log(LOG_DEV_DATA, "Failed to match actual fan speed (%d) with desired fan speed (%d) with tolerance (%d)",
                        actualFanSpeedValue, desiredFanSpeedValue, toleranceRPM);
                }
            }
            else
            {
                // Failed to read and verify the desired fan speed
                // Could not read data from the module
                testResult = testFail;
                testDescription = GetFaultDescription("CommunicationFailure");
                testCode = GetFaultCode("CommunicationFailure");
                Log(LOG_DEV_DATA, "Error reading desired fan speed from the module");
            }

        }
        else
        {
            //Failed the read and verify of the actual speed sensor
            // Could not read data from the module
            testResult = testFail;
            testDescription = GetFaultDescription("CommunicationFailure");
            testCode = GetFaultCode("CommunicationFailure");
            Log(LOG_DEV_DATA, "Error reading actual fan speed from the module");
        }


        // Report the result
        char buff[32];
        string fanUnits(GetParameter("EngineFanUnits"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "Actual Fan Speed", CreateMessage(buff, sizeof(buff), "%d", actualFanSpeedValue) , fanUnits,
                                 "Desired Fan Speed",CreateMessage(buff, sizeof(buff), "%d", desiredFanSpeedValue), fanUnits,
                                 "Tolerance Fan Speed",CreateMessage(buff, sizeof(buff), "%d", toleranceRPM), fanUnits);

    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step CheckEngineFan()");
        testResult = testSkip;
    }


    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckEngineFan() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckCASELearn()
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckCASELearn() - Enter");
    string result(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));;
    string testCode("0000");
    bool caseLearnInProgress(false);
    if (!ShortCircuitTestStep())
    {   // Check if CASE learn has been completed already
        BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
        UINT8 caseLearnStatus = 0x00;
        status = m_vehicleModule.ReadModuleData("ReadCaseLearnStatus", caseLearnStatus);
        if (caseLearnStatus != GetParameterInt("CaseLearnCompleteValue"))
        {   // command the module to begin the CASE learn procedure after operator is applying brake pedal
            DisplayPrompt(GetPromptBox("CaseLearnInProgress"), GetPrompt("CaseLearnInProgress"), GetPromptPriority("CaseLearnInProgress"));
            DisplayPrompt(GetPromptBox("LeftFootOnBrake"), GetPrompt("LeftFootOnBrake"), GetPromptPriority("LeftFootOnBrake"));
            bool brakeOn = false;
            BEP_STATUS_TYPE brakeStatus = BEP_STATUS_SUCCESS;
            while (!brakeOn && TimeRemaining() && (BEP_STATUS_SUCCESS == brakeStatus) && (BEP_STATUS_SUCCESS == StatusCheck()))
            {
                brakeStatus = m_vehicleModule.ReadModuleData("ReadBrakeSwitchPosition", brakeOn);
                if (!brakeOn)
                {
                    BposSleep(GetTestStepInfoInt("ScanDelay"));
                }
            }
            RemovePrompt(GetPromptBox("LeftFootOnBrake"), GetPrompt("LeftFootOnBrake"), GetPromptPriority("LeftFootOnBrake"));
            // Command the CASE Learn process to begin if all is well
            if (brakeOn && (BEP_STATUS_SUCCESS == brakeStatus) && (BEP_STATUS_SUCCESS == StatusCheck()))
            {
                result = StartCaseLearnSequence();
            }
            // Look for CASE learn complete if the learn process has begun
            if (!result.compare(testPass))
            {   // Prompt the operator to get vehicle conditions correct
                DisplayPrompt(GetPromptBox("PushAcceleratorPedalToFloor"), GetPrompt("PushAcceleratorPedalToFloor"),
                              GetPromptPriority("PushAcceleratorPedalToFloor"));
                // Give the CASE learn process some time to start
                BposSleep(GetParameterInt("CaseLearnStartDelay"));
                UINT8 caseLearnInProgressMask = GetParameterInt("CaseLearnInProgressMask");
                // Wait for CASE Learn to be in progress
                do
                {   // Read the CASE learn status from the module
                    status = m_vehicleModule.ReadModuleData("ReadCaseLearnStatus", caseLearnStatus);
                    // If case learn is not complete, wait a bit before checking again
                    caseLearnInProgress = (caseLearnStatus & caseLearnInProgressMask);
                    if (!caseLearnInProgress)  BposSleep(GetTestStepInfoInt("ScanDelay"));
                    // Keep checking until case learn is complete, or we run out of time
                } while (TimeRemaining() && !caseLearnInProgress && 
                         (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status));
                // Wait for CASE Learn to complete
                if (caseLearnInProgress)
                {
                    do
                    {   // Read the CASE learn status from the module
                        status = m_vehicleModule.ReadModuleData("ReadCaseLearnStatus", caseLearnStatus);
                        // If case learn is not complete, wait a bit before checking again
                        caseLearnInProgress = (caseLearnStatus & caseLearnInProgressMask);
                        if (caseLearnInProgress)  BposSleep(GetTestStepInfoInt("ScanDelay"));
                        // Keep checking until case learn is complete, or we run out of time
                    } while (TimeRemaining() && caseLearnInProgress && 
                             (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status));
                }
                // If case learn has completed, make sure it was successful
                if (!caseLearnInProgress)
                {   // Make sure it is completed

                    result = (caseLearnStatus ==  GetParameterInt("CaseLearnCompleteValue")) ? testPass : testFail;
                    Log(LOG_DEV_DATA, "CASE Learn result: %s", result.c_str());
                }
                else
                {   // Case learn did not complete
                    Log(LOG_ERRORS, "CASE Learn did not complete");
                    result = testFail;
                }
                RemovePrompt(GetPromptBox("PushAcceleratorPedalToFloor"), GetPrompt("PushAcceleratorPedalToFloor"),
                             GetPromptPriority("PushAcceleratorPedalToFloor"));
                DisplayTimedPrompt(GetPrompt("ReleaseThrottle"), GetPromptBox("ReleaseThrottle"), 
                                   GetPromptPriority("ReleaseThrottle"), GetPromptDuration("ReleaseThrottle"));
            }
            else
            {   // Could not start the CASE learn procedure brake switch not pressed
                Log(LOG_ERRORS, "Failed to start the CASE learn procedure in the module");
                result = testTimeout;
                testDescription = "Brake Press: " + GetFaultDescription("TimeoutFailure");
            }
            RemovePrompt(GetPromptBox("CaseLearnInProgress"), GetPrompt("CaseLearnInProgress"), GetPromptPriority("CaseLearnInProgress"));
        }
        else
        {   // Case learn already completed
            Log(LOG_DEV_DATA, "CASE learn already completed");
            result = testPass;
        }
        // Report the result
        SendTestResult(result, testDescription, testCode);
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step CheckCASELearn()");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckCASELearn() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckFuelTrim(const bool &checkAtIdle)
{
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));;
    string testCode("0000");

    string shortTermFuelTrimBank1TestResult(BEP_TESTING_RESPONSE);
    string shortTermFuelTrimBank2TestResult(BEP_TESTING_RESPONSE);
    string longTermFuelTrimBank1TestResult(BEP_TESTING_RESPONSE);
    string longTermFuelTrimBank2TestResult(BEP_TESTING_RESPONSE);

    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckIdleFuelTrim() - Enter");
    if (!ShortCircuitTestStep())
    {
        UINT8 engineSpeedValue = 0;
        string shortTermFuelTrimBank1Name("ShortTermFuelTrimBank1");
        string shortTermFuelTrimBank2Name("ShortTermFuelTrimBank2");
        string longTermFuelTrimBank1Name("LongTermFuelTrimBank1");
        string longTermFuelTrimBank2Name("LongTermFuelTrimBank2");
        float shortTermFuelTrimBank1Value = 0;
        float shortTermFuelTrimBank2Value = 0;
        float longTermFuelTrimBank1Value = 0;
        float longTermFuelTrimBank2Value = 0;
        BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
        bool engineSpeedGood = false;
        if (checkAtIdle)
        {   //Verify we are at idle rpm
            status = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineSpeedValue);
            engineSpeedGood = (engineSpeedValue < GetParameterInt("MaximumEngineSpeed"));
        }
        else
        {
            status = BEP_STATUS_SUCCESS;
            engineSpeedGood = true;
        }
        if ((BEP_STATUS_SUCCESS == status) && engineSpeedGood)
        {   // read short term bank 1
            shortTermFuelTrimBank1TestResult = ReadAndVerifySensor(shortTermFuelTrimBank1Name, shortTermFuelTrimBank1Value);
            //read short term bank 2
            shortTermFuelTrimBank2TestResult = ReadAndVerifySensor(shortTermFuelTrimBank2Name, shortTermFuelTrimBank2Value);            
            //read long term bank 1
            longTermFuelTrimBank1TestResult = ReadAndVerifySensor(longTermFuelTrimBank1Name, longTermFuelTrimBank1Value);
            //read long term bank 2
            longTermFuelTrimBank2TestResult = ReadAndVerifySensor(longTermFuelTrimBank2Name, longTermFuelTrimBank2Value);
        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            Log(LOG_DEV_DATA, "Error reading engine speed from the module or above idle rpm - %d", engineSpeedValue);
        }

        // Look at all test results for an overal pass/fail
        if (testResult == testFail)
        {
            //already failed for no vehicle comms or above idle rpm
            Log(LOG_DEV_DATA, "Already failed for no vehicle comms or above idle rpm");
        }
        else if (shortTermFuelTrimBank1TestResult.compare(testPass) != 0)
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "Failure in short term fuel trim bank 1 test");
        }
        else if (shortTermFuelTrimBank2TestResult.compare(testPass) != 0)
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "Failure in short term fuel trim bank 2 test");
        }
        else if (longTermFuelTrimBank1TestResult.compare(testPass) != 0)
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "Failure in long term fuel trim bank 1 test");
        }
        else if (longTermFuelTrimBank2TestResult.compare(testPass) != 0)
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "Failure in long term fuel trim bank 2 test");
        }
        else
        {
            testResult = testPass;
        }


        // Report the result
        char buff[32];
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "Engine RPM During Test", CreateMessage(buff, sizeof(buff), "%d", engineSpeedValue) , "RPM",
                                 "Short Term Bank 1 Fuel Trim", CreateMessage(buff, sizeof(buff), "%.2f", shortTermFuelTrimBank1Value) , "%",
                                 "Short Term Bank 2 Fuel Trim", CreateMessage(buff, sizeof(buff), "%.2f", shortTermFuelTrimBank2Value) , "%",
                                 "Long Term Bank 1 Fuel Trim", CreateMessage(buff, sizeof(buff), "%.2f", longTermFuelTrimBank1Value) , "%",
                                 "Long Term Bank 2 Fuel Trim", CreateMessage(buff, sizeof(buff), "%.2f", longTermFuelTrimBank2Value) , "%");

    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping test step CheckIdleFuelTrim()");
        testResult = testSkip;
    }


    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckIdleFuelTrim() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSensorRangeAtIdleFloat(const string &sensorName)
{   // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeAtIdleFloat(%s) - Enter", sensorName.c_str());
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep())
    {
        UINT16 engineSpeedValue = 0;
        float sensorValue = 0.0;
        float minValue = GetParameterFloat("Minimum" + sensorName);
        float maxValue = GetParameterFloat("Maximum" + sensorName);

        string testDescription(GetTestStepInfo("Description"));
        string testCode("0000");

        //Verify we are at idle rpm
        BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineSpeedValue);
        if ((BEP_STATUS_SUCCESS == status) && (engineSpeedValue < GetParameterInt("MaximumEngineSpeed")) )
        {
            BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read" + sensorName + "Sensor", sensorValue);
            if (BEP_STATUS_SUCCESS == status)
            {   // Good data from the module, evaluate against parameters
                testResult = ((minValue <= sensorValue) && (sensorValue <= maxValue)) ? testPass : testFail;
                Log(LOG_DEV_DATA, "%s value: (%.2f) - range: (%.2f) - (%.2f) - result: %s",
                    sensorName.c_str(), sensorValue, minValue, maxValue, testResult.c_str());
                if (testResult.compare(testPass))
                {   // Sensor value out of range
                    if (sensorValue > maxValue)
                    {   // Sensor reading out of range - high
                        testDescription = GetFaultDescription(sensorName + "OutOfRangeHigh");
                        testCode = GetFaultCode(sensorName + "OutOfRangeHigh");
                    }
                    else
                    {   // Engine coolant temp too cold
                        testDescription = GetFaultDescription(sensorName + "OutOfRangeLow");
                        testCode = GetFaultCode(sensorName + "OutOfRangeLow");
                    }
                }
            }
            else
            {   // Could not read the engine temperature from the module
                testResult = testFail;
                testDescription = GetFaultDescription("CommunicationFailure");
                testCode = GetFaultCode("CommunicationFailure");
                Log(LOG_DEV_DATA, "Error reading %s from the module - %s", 
                    sensorName.c_str(), ConvertStatusToResponse(status).c_str());
            }


        }
        else
        {   // Could not read data from the module
            testResult = testFail;
            testDescription = "Error reading from module or above idle rpm";
            Log(LOG_DEV_DATA, "Error reading engine speed from the module or above idle rpm - %d", engineSpeedValue);
        }


        // Report the result
        char buffer[32];
        string sensorValueUnits(GetParameter(sensorName + "Units"));
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "SensorReading", CreateMessage(buffer, sizeof(buffer), "%.2f", sensorValue), sensorValueUnits,
                                 "MinParam", GetParameter("Minimum" + sensorName), sensorValueUnits,
                                 "MaxParam", GetParameter("Maximum" + sensorName), sensorValueUnits,
                                 "Engine RPM During Test", CreateMessage(buffer, sizeof(buffer), "%.2f", engineSpeedValue) , "RPM");
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckSensorRangeAtIdleFloat(%s)", sensorName.c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckSensorRangeAtIdleFloat(%s) - Exit", sensorName.c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckNeutralRPMOverspeed()
{
    // Log the entry and check if this step should be performed
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckNeutralRPMOverspeed() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));
    string testCode("0000");

    if (!ShortCircuitTestStep())
    {
        // Keep monitoring until we are in neutral
        bool inNeutral = false;
        string transGear("Unknown");
        BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
        DisplayPrompt(GetPromptBox("WaitForNeutral"), GetPrompt("WaitForNeutral"), 
                      GetPromptPriority("WaitForNeutral"));
        do
        {   // Read the transmission gear from the module
            moduleStatus = m_vehicleModule.ReadModuleData("ReadCurrentGear", transGear);
            Log(LOG_DEV_DATA, "Current transgear: %s", transGear.c_str());
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Check if the engine coolant temperature is in range
                if (transGear.compare("Neutral") == 0)
                {
                    //we are in neutral
                    inNeutral = true;
                }
                if (!inNeutral) BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
        } while ((BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus) &&
                 !inNeutral);
        RemovePrompt(GetPromptBox("WaitForNeutral"), GetPrompt("WaitForNeutral"), 
                     GetPromptPriority("WaitForNeutral"));


        //Keep Monitoring until we are at WOT
        bool inWOT = false;
        float throttlePosition = 0.0;
        float minEngineRPM = 0.0;
        float engineRPM = 0.0;
        moduleStatus = BEP_STATUS_ERROR;
        DisplayPrompt(GetPromptBox("WaitForWOT"), GetPrompt("WaitForWOT"), 
                      GetPromptPriority("WaitForWOT"));
        do
        {   // Read the throttle position from the module
            moduleStatus = m_vehicleModule.ReadModuleData("ReadThrottlePosition", throttlePosition);
            Log(LOG_DEV_DATA, "Current throttle position: %.2f", throttlePosition);
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Check if the throttle position is in range
                if ( throttlePosition >= 90.0 )
                {
                    //we are in neutral
                    inWOT = true;

                    //NOW READ RPM TO VERIFY AGAINST PARAMETER
                    moduleStatus = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineRPM);
                    if (BEP_STATUS_SUCCESS == moduleStatus)
                    {
                        minEngineRPM = GetParameterFloat("MinimumNeutralOverspeedLimitRpm");
                        if (engineRPM < minEngineRPM)
                            testResult = testFail;
                        else
                            testResult = testPass;
                    }
                }
                if (!inWOT) BposSleep(GetTestStepInfoInt("ScanDelay"));
            }
        } while ((BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == moduleStatus) &&
                 !inWOT);
        RemovePrompt(GetPromptBox("WaitForWOT"), GetPrompt("WaitForWOT"), 
                     GetPromptPriority("WaitForWOT"));



        // Report the result
        char buffer[32];
        SendTestResultWithDetail(testResult, testDescription, testCode,
                                 "EngineRPM", CreateMessage(buffer, sizeof(buffer), "%d", engineRPM), "RPM",
                                 "MinParam", CreateMessage(buffer, sizeof(buffer), "%d", minEngineRPM), "RPM");
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping IsuzuEmissionsTc::CheckNeutralRPMOverspeed()");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckNeutralRPMOverspeed() - Exit");
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::ReadFaults(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    FaultStatusVector_t moduleFaults;
    string foundFaults;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter IsuzuEmissionsTc::ReadFaults()\n");
    if (!ShortCircuitTestStep())
    {   // Do not need to skip this step
        try
        {   // Try to read the module faults
            moduleStatus = m_vehicleModule.ReadFaults(moduleFaults);
            // Check the status of the operation
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {   // Good read, evaluate the data
                bool faultsRecorded = false;
                Log(LOG_DEV_DATA, "Found %d faults recorded in the module\n", moduleFaults.size());
                // Only check for faults if faults were read from the module
                if (moduleFaults.size() > 0)
                {
                    for (UINT32 faultIndex = 0; faultIndex < moduleFaults.size(); faultIndex++)
                    {   // Determine if this fault should be ignored
                        string faultTag = "ModuleFault_" + moduleFaults[faultIndex].m_code;
                        INT32 faultCode = atoh(moduleFaults[faultIndex].m_code.c_str());
                        string faultStatus("Ignored");
                        UINT8 statusMask = 0x00;
                        UINT8 dtcStatus = atoh(moduleFaults[faultIndex].m_status.c_str());;
                        bool ignored;
                        try
                        {
                            statusMask = atoh(GetFaultFailureStatusMask(faultTag).c_str()); 
                            ignored = !(statusMask & dtcStatus);
                        }
                        catch (...)
                        {
                            ignored = false;
                        }
                        Log(LOG_DEV_DATA, "Ignored from dtc status byte? %s - status mask [%02X] dtc status [%02X]\n", 
                            ignored ? "True" : "False", statusMask, dtcStatus);
                        ignored = (ignored ? ignored : !(m_ignoreFaults.find(faultTag) == m_ignoreFaults.end()));
                        if (((faultCode != 0) && !ignored) || ((faultCode != 0) && GetParameterBool("ReportIgnoredFaults")))
                        {   // This is a fault to report
                            if (!ignored)
                            {
                                faultStatus = "Reported";
                                faultsRecorded = true;
                            }

                            //Set strDtcStatus to indicate the DTC status
                            string strDtcStatus = "";
                            UINT8 dtcActiveBit = (UINT8)GetParameterInt("DtcStatusActiveBit");
                            UINT8 dtcHistoryBit = (UINT8)GetParameterInt("DtcStatusHistoryBit");
                            UINT8 dtcNotPassBit = (UINT8)GetParameterInt("DtcStatusNotPassBit");
                            bool checkDtcActive = (statusMask & (1<<dtcActiveBit)) ? true : false;
                            bool checkDtcHistory = (statusMask & (1<<dtcHistoryBit)) ? true : false;
                            bool checkDtcNotPass = (statusMask & (1<<dtcNotPassBit)) ? true : false;
                            bool dtcActiveBitOn = (dtcStatus & (1<<dtcActiveBit)) ? true : false;
                            bool dtcHistoryBitOn = (dtcStatus & (1<<dtcHistoryBit)) ? true : false;
                            bool dtcNotPassBitOn = (dtcStatus & (1<<dtcNotPassBit)) ? true : false;

                            if (checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn &&
                                checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active, History, and Not Passed";
                            else if (checkDtcActive && dtcActiveBitOn && checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "Active and History";
                            else if (checkDtcActive && dtcActiveBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Active and Not Passed";
                            else if (checkDtcHistory && dtcHistoryBitOn && checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "History and Not Passed";
                            else if (checkDtcActive && dtcActiveBitOn)
                                strDtcStatus = "Active";
                            else if (checkDtcHistory && dtcHistoryBitOn)
                                strDtcStatus = "History";
                            else if (checkDtcNotPass && dtcNotPassBitOn)
                                strDtcStatus = "Not Passed";
                            else
                                strDtcStatus = "SOFTWARE FAILURE!";


                            ReportDTC(faultTag, moduleFaults[faultIndex].m_code, "Code="+GetFaultCode(faultTag)+" - "+strDtcStatus+" - "+GetFaultDescription(faultTag));
                        }
                        // Log the fault read from the module
                        Log(LOG_DEV_DATA, "Module Fault %d - %s - %s\n", faultIndex+1,
                            moduleFaults[faultIndex].m_code.c_str(), faultStatus.c_str());
                    }
                    // Do special handling if faults have been recorded
                    if (faultsRecorded)
                    {   // Check if test sequence should be aborted if faults are present
                        if (GetParameterBool("AbortIfDTCPresent"))
                        {
                            testResult = testFail;
                            SystemWrite(ABORT_DATA_TAG, "1");
                            Log(LOG_ERRORS, "Faults found in the module - ABORT all testing!\n");
                        }
                        else
                        {
                            testResult = testFail;
                            Log(LOG_ERRORS, "Faults found in the module - continue to test!\n");
                        }
                    }
                    // Store the data indicating if faults are present in the module
                    DTCsInModule(faultsRecorded);
                }
                // Set the test status
                testResult = faultsRecorded ? testResult : testPass;
                testResultCode = faultsRecorded ? GetFaultCode("ModuleFaults") : "0000";
                testDescription = faultsRecorded ? GetFaultDescription("ModuleFaults") : GetTestStepInfo("Description");
                Log(LOG_DEV_DATA, "Read Module Faults: %s\n", testResult.c_str());
            }
            else
            {   // Error reading faults from the module
                Log(LOG_ERRORS, "Error reading module faults - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                SetCommunicationFailure(true);
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
        // Report the results
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit GenericTCTemplate::ReadFaults()\n");
    return testResult;
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::DelayBeforeDtcRead(void)
{
    string result(BEP_TESTING_RESPONSE);

    if (GetParameterInt("DelayBeforeDtcRead") != 0)
    {
        DisplayPrompt(GetPromptBox("DelayBeforeDtcRead"), GetPrompt("DelayBeforeDtcRead"), GetPromptPriority("DelayBeforeDtcRead"));
        BposSleep(GetParameterInt("DelayBeforeDtcRead"));
        RemovePrompt(GetPromptBox("DelayBeforeDtcRead"), GetPrompt("DelayBeforeDtcRead"), GetPromptPriority("DelayBeforeDtcRead"));
        result = testPass;
    }
    else
        result = testSkip;

    return result;
}



//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckOxygenSensors(void)
{
    string testResult(BEP_TESTING_RESPONSE);
    string testDescription(GetTestStepInfo("Description"));
    string testCode("0000");

    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckOxygenSensors() - Enter");

    if (!ShortCircuitTestStep())
    {
        UINT16 engineSpeedValue = 0;
        UINT16 engineMaxIdleSpeedValue = GetParameterInt("MaximumEngineSpeed");
        UINT32 numSamples = GetParameterInt("OxygenSensorSamples");
        UINT32 sleepTime = GetParameterInt("OxygenSensorPeriodTime");
        string s1Name = GetDataTag("OxygenSensor1Name");
        string s2Name = GetDataTag("OxygenSensor2Name");
        string s3Name = GetDataTag("OxygenSensor3Name");
        string s4Name = GetDataTag("OxygenSensor4Name");
        string s1LastBand = "";
        string s2LastBand = "";
        string s3LastBand = "";
        string s4LastBand = "";

        // Run the sensor test based only on the change from the
        // first value to the last value
        bool s1TestDelta = GetParameterBool(s1Name+"TestDelta");
        bool s2TestDelta = GetParameterBool(s2Name+"TestDelta");
        bool s3TestDelta = GetParameterBool(s3Name+"TestDelta");
        bool s4TestDelta = GetParameterBool(s4Name+"TestDelta");

        // The sXBandLimits array is (Low Band Limit, Open Band Lower Limit,
        // Open Band Upper Limit, and High Band Limit}
        float s1BandLimits[4];
        float s2BandLimits[4];
        float s3BandLimits[4];
        float s4BandLimits[4];

        // Array for the first and last values for the O2 sensors
        float s1FLVals[2];
        float s2FLVals[2];
        float s3FLVals[2];
        float s4FLVals[2];

        // The sXCounts array is {Low Band Count, Open Band Count, High Band Count, Band Switch Count}
        int s1Counts[] = {0, 0, 0, 0};
        int s2Counts[] = {0, 0, 0, 0};
        int s3Counts[] = {0, 0, 0, 0};
        int s4Counts[] = {0, 0, 0, 0};

        DisplayPrompt(GetPromptBox("CheckingO2Sensors"), GetPrompt("CheckingO2Sensors"), 
                      GetPromptPriority("CheckingO2Sensors"), "", "0");
        DisplayPrompt(GetPromptBox("EngineMustIdle"), GetPrompt("EngineMustIdle"), 
                      GetPromptPriority("EngineMustIdle"));


        if (!s1TestDelta)
        {
            GetO2SensorParams(s1Name, s1BandLimits);
            Log(LOG_DEV_DATA, "Oxygen Sensor #1 Info: Name=%s - BandLimits=%f, %f, %f, %f", 
                s1Name.c_str(), s1BandLimits[0], s1BandLimits[1], s1BandLimits[2], s1BandLimits[3]);
        }
        if (!s2TestDelta)
        {
            GetO2SensorParams(s2Name, s2BandLimits);
            Log(LOG_DEV_DATA, "Oxygen Sensor #2 Info: Name=%s - BandLimits=%f, %f, %f, %f",
                s2Name.c_str(), s2BandLimits[0], s2BandLimits[1], s2BandLimits[2], s2BandLimits[3]);
        }
        if (!s3TestDelta)
        {
            GetO2SensorParams(s3Name, s3BandLimits);
            Log(LOG_DEV_DATA, "Oxygen Sensor #3 Info: Name=%s - BandLimits=%f, %f, %f, %f",
                s3Name.c_str(), s3BandLimits[0], s3BandLimits[1], s3BandLimits[2], s3BandLimits[3]);
        }
        if (!s4TestDelta)
        {
            GetO2SensorParams(s4Name, s4BandLimits);
            Log(LOG_DEV_DATA, "Oxygen Sensor #4 Info: Name=%s - BandLimits=%f, %f, %f, %f",
                s4Name.c_str(), s4BandLimits[0], s4BandLimits[1], s4BandLimits[2], s4BandLimits[3]);
        }


        SetStartTime();
        //Verify we are at idle rpm
        BEP_STATUS_TYPE engineStatus = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineSpeedValue);
        BEP_STATUS_TYPE s1Status = BEP_STATUS_SUCCESS;
        BEP_STATUS_TYPE s2Status = BEP_STATUS_SUCCESS;
        BEP_STATUS_TYPE s3Status = BEP_STATUS_SUCCESS;
        BEP_STATUS_TYPE s4Status = BEP_STATUS_SUCCESS;
        UINT32 i=0;
        char buffer[8];
        UINT8 percent = 0;

        while (engineStatus==BEP_STATUS_SUCCESS && i<numSamples && TimeRemaining() && s1Status==BEP_STATUS_SUCCESS && 
               s2Status==BEP_STATUS_SUCCESS && s3Status==BEP_STATUS_SUCCESS && s4Status==BEP_STATUS_SUCCESS)
        {
            //Update engine speed to make sure that we are idling
            engineStatus = m_vehicleModule.ReadModuleData("ReadEngineSpeedSensor", engineSpeedValue);

            if (engineSpeedValue<=engineMaxIdleSpeedValue)
            { //Engine is idling so read sensors
                //Read and process Oxygen Sensor #1 data
                if (!s1TestDelta)
                    s1Status = ReadAndProcessO2Sensor(s1Name, s1BandLimits, s1Counts, s1LastBand);
                else if (i==0 || i==(numSamples-1))
                    s1Status = ReadAndProcessO2Sensor(s1Name, s1FLVals, i);

                //Read and process Oxygen Sensor #2 data
                if (!s2TestDelta)
                    s2Status = ReadAndProcessO2Sensor(s2Name, s2BandLimits, s2Counts, s2LastBand);
                else if (i==0 || i==(numSamples-1))
                    s2Status = ReadAndProcessO2Sensor(s2Name, s2FLVals, i);

                //Read and process Oxygen Sensor #3 data
                if (!s3TestDelta)
                    s3Status = ReadAndProcessO2Sensor(s3Name, s3BandLimits, s3Counts, s3LastBand);
                else if (i==0 || i==(numSamples-1))
                    s3Status = ReadAndProcessO2Sensor(s3Name, s3FLVals, i);

                //Read and process Oxygen Sensor #4 data
                if (!s4TestDelta)
                    s4Status = ReadAndProcessO2Sensor(s4Name, s4BandLimits, s4Counts, s4LastBand);
                else if (i==0 || i==(numSamples-1))
                    s4Status = ReadAndProcessO2Sensor(s4Name, s4FLVals, i);

                i++;
                // Display percent complete
                percent = i*100/numSamples;
                if (percent%10 == 0)
                {
                    RemovePrompt(GetPromptBox("CheckingO2Sensors"), GetPrompt("CheckingO2Sensors"), GetPromptPriority("CheckingO2Sensors"));
                    DisplayPrompt(GetPromptBox("CheckingO2Sensors"), GetPrompt("CheckingO2Sensors"), 
                                  GetPromptPriority("CheckingO2Sensors"), "", CreateMessage(buffer, sizeof(buffer), "%d", percent));

                }

            }
            else
            {
                Log(LOG_DEV_DATA, "Not reading Oxygen Sensors because engine RPM is too high.");
            }

            BposSleep(sleepTime);
        }

        // Check for errors
        if (!TimeRemaining())
        {
            Log(LOG_ERRORS,"Timeout reading Oxygen Sensors");
            testDescription = "Oxygen Sensor Read: "+GetFaultDescription("TimeoutFailure");
            testResult = testTimeout;
        }
        if (s1Status!=BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS,"Error reading data from %s", s1Name.c_str());
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }
        if (s2Status!=BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS,"Error reading data from %s", s2Name.c_str());
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }
        if (s3Status!=BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS,"Error reading data from %s", s3Name.c_str());
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }
        if (s4Status!=BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS,"Error reading data from %s", s4Name.c_str());
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }
        if (engineStatus!=BEP_STATUS_SUCCESS)
        {
            Log(LOG_ERRORS,"Error reading engine RPM");
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }


        if (testResult.compare(BEP_TESTING_RESPONSE) == 0)
        { //No error yet, keep going...
            bool s1Passed, s2Passed, s3Passed, s4Passed;

            if (!s1TestDelta)    testResult = AnalyzeO2SensorData(s1Name, s1Counts);
            else                testResult = AnalyzeO2SensorDelta(s1Name, s1FLVals);
            Log(LOG_DEV_DATA, "%s: %s finished analysis with a result of %s", GetTestStepName().c_str(), s1Name.c_str(), testResult.c_str());
            s1Passed = testResult.compare(testPass)==0 ? true : false;

            if (!s2TestDelta)    testResult = AnalyzeO2SensorData(s2Name, s2Counts);
            else                testResult = AnalyzeO2SensorDelta(s2Name, s2FLVals);
            Log(LOG_DEV_DATA, "%s: %s finished analysis with a result of %s", GetTestStepName().c_str(), s2Name.c_str(), testResult.c_str());
            s2Passed = testResult.compare(testPass)==0 ? true : false;

            if (!s3TestDelta)    testResult = AnalyzeO2SensorData(s3Name, s3Counts);
            else                testResult = AnalyzeO2SensorDelta(s3Name, s3FLVals);
            Log(LOG_DEV_DATA, "%s: %s finished analysis with a result of %s", GetTestStepName().c_str(), s3Name.c_str(), testResult.c_str());
            s3Passed = testResult.compare(testPass)==0 ? true : false;

            if (!s4TestDelta)    testResult = AnalyzeO2SensorData(s4Name, s4Counts);
            else                testResult = AnalyzeO2SensorDelta(s4Name, s4FLVals);
            Log(LOG_DEV_DATA, "%s: %s finished analysis with a result of %s", GetTestStepName().c_str(), s4Name.c_str(), testResult.c_str());
            s4Passed = testResult.compare(testPass)==0 ? true : false;


            if (!s1Passed || !s2Passed || !s3Passed || !s4Passed)
            {
                testResult = testFail;
                testDescription = GetFaultDescription("OxygenSensorsFailedAnalysis");
                testCode = GetFaultCode("OxygenSensorsFailedAnalysis");
            }
        }

        RemovePrompt(GetPromptBox("CheckingO2Sensors"), GetPrompt("CheckingO2Sensors"), 
                     GetPromptPriority("CheckingO2Sensors"));
        RemovePrompt(GetPromptBox("EngineMustIdle"), GetPrompt("EngineMustIdle"), 
                     GetPromptPriority("EngineMustIdle"));

    }
    else
    {
        testResult = testSkip;
    }

    SendTestResult(testResult, testDescription, testCode);

    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::CheckOxygenSensors() - Exit");
    return testResult;
}

template<class ModuleType>
void IsuzuEmissionsTc<ModuleType>::GetO2SensorParams(string O2Sensor, float limits[])
{
    limits[0] = GetParameterFloat(O2Sensor+"LBandLimit");
    limits[1] = GetParameterFloat(O2Sensor+"OBandLowLimit");
    limits[2] = GetParameterFloat(O2Sensor+"OBandHighLimit");
    limits[3] = GetParameterFloat(O2Sensor+"HBandLimit");

}


template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadAndProcessO2Sensor(string O2Name, float limits[], int counts[], string &lastBand)
{
    float sensorValue = 0.0;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read"+O2Name+"Sensor", sensorValue);
    if (status==BEP_STATUS_SUCCESS)
    {
        if (sensorValue<=limits[0])
        { //In Low Band
            Log(LOG_DEV_DATA, "%s Value=%.3f, Band=Low", O2Name.c_str(), sensorValue);
            counts[0]++; //Increment Low Band count
            if (lastBand.compare("Low") != 0)
            {
                if (lastBand.compare("") != 0)
                    counts[3]++; //Increment Band Switch count
                lastBand = "Low";
            }
        }
        else if (limits[1]<=sensorValue && sensorValue<=limits[2])
        { //In Open Band
            Log(LOG_DEV_DATA, "%s Value=%.3f, Band=Open", O2Name.c_str(), sensorValue);
            counts[1]++; //Increment Open Band count
            // This is commented out because we only we to count band switches from
            // Low to High or from High to Low
            /**
            if(lastBand.compare("Open") != 0){
                if(lastBand.compare("") != 0)
                    counts[3]++; //Increment Band Switch count
                lastBand = "Open";
            }
            */
        }
        else if (limits[3]<=sensorValue)
        { //In High Band
            Log(LOG_DEV_DATA, "%s Value=%.3f, Band=High", O2Name.c_str(), sensorValue);
            counts[2]++; //Increment High Band count
            if (lastBand.compare("High") != 0)
            {
                if (lastBand.compare("") != 0)
                    counts[3]++; //Increment Band Switch count
                lastBand = "High";
            }
        }
        else
        { //Not in any of the bands
            Log(LOG_DEV_DATA, "%s Value=%.3f, Band=None", O2Name.c_str(), sensorValue);
        }
    }

    return status;
}


template<class ModuleType>
BEP_STATUS_TYPE IsuzuEmissionsTc<ModuleType>::ReadAndProcessO2Sensor(string O2Name, float FLVals[], int sampleNum)
{
    float sensorValue = 0.0;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("Read"+O2Name+"Sensor", sensorValue);
    if (status==BEP_STATUS_SUCCESS)
    {
        if (sampleNum == 0)
        {
            FLVals[0] = sensorValue;
            Log(LOG_DEV_DATA, "%s Value=%.3f, First Value", O2Name.c_str(), sensorValue);
        }
        else
        {
            FLVals[1] = sensorValue;
            Log(LOG_DEV_DATA, "%s Value=%.3f, Last Value", O2Name.c_str(), sensorValue);
        }
    }

    return status;
}


template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::AnalyzeO2SensorData(string O2Name, int counts[])
{
    string testResult = BEP_TESTING_RESPONSE;
    string testDescription = O2Name+" Passed";
    INT32 numSamples = GetParameterInt("OxygenSensorSamples");
    char lowCountStr[16];
    char openCountStr[16];
    char highCountStr[16];
    char switchCountStr[16];

    if (counts[0]*100/numSamples > GetParameterInt(O2Name+"MaxPercentLow"))
    {
        testDescription = O2Name+"  has too high percent in Low Band";
        Log(LOG_DEV_DATA, "%s has too high percent in Low Band - Count=%d, Samples=%d, Max=%d%%", 
            O2Name.c_str(), counts[0], numSamples, GetParameterInt(O2Name+"MaxPercentLow"));
        testResult = testFail;
    }
    if (counts[1]*100/numSamples > GetParameterInt(O2Name+"MaxPercentOpen"))
    {
        testDescription = O2Name+"  has too high percent in Open Band";
        Log(LOG_DEV_DATA, "%s has too high percent in Open Band - Count=%d, Samples=%d, Max=%d%%", 
            O2Name.c_str(), counts[1], numSamples, GetParameterInt(O2Name+"MaxPercentOpen"));
        testResult = testFail;
    }
    if (counts[2]*100/numSamples > GetParameterInt(O2Name+"MaxPercentHigh"))
    {
        testDescription = O2Name+"  has too high percent in High Band";
        Log(LOG_DEV_DATA, "%s has too high percent in High Band - Count=%d, Samples=%d, Max=%d%%", 
            O2Name.c_str(), counts[2], numSamples, GetParameterInt(O2Name+"MaxPercentHigh"));
        testResult = testFail;
    }
    if (counts[3] < GetParameterInt(O2Name+"MinBandSwitches"))
    {
        testDescription = O2Name+"  does not have enough Band switches";
        Log(LOG_DEV_DATA, "%s does not have enough Band switches - Count=%d, Min=%d", 
            O2Name.c_str(), counts[3], GetParameterInt(O2Name+"MinBandSwitches"));
        testResult = testFail;
    }

    if (testResult != testFail)
        testResult = testPass;

    SendSubtestResultWithDetail(GetTestStepName()+"_"+O2Name, testResult, testDescription, "0000", 
                                "LowBandCount", CreateMessage(lowCountStr, sizeof(lowCountStr), "%d", counts[0]), "Count", 
                                "OpenBandCount", CreateMessage(openCountStr, sizeof(openCountStr), "%d", counts[1]), "Count",
                                "HighBandCount", CreateMessage(highCountStr, sizeof(highCountStr), "%d", counts[2]), "Count",
                                "BandSwitches", CreateMessage(switchCountStr, sizeof(switchCountStr), "%d", counts[3]), "Count");

    return testResult;

}


template<class ModuleType>
string IsuzuEmissionsTc<ModuleType>::AnalyzeO2SensorDelta(string O2Name, float FLVals[])
{
    string testResult = BEP_TESTING_RESPONSE;
    string testDescription = O2Name+" Passed";
    float minPercent = GetParameterFloat(O2Name+"MinDeltaPercent");
    float maxPercent = GetParameterFloat(O2Name+"MaxDeltaPercent");
    char firstValStr[16];
    char lastValStr[16];
    char minPercentStr[16];
    char maxPercentStr[16];
    char deltaPercentStr[16];


    float delta = FLVals[1]-FLVals[0];
    float deltaPercent = delta*100/FLVals[0];

    if (deltaPercent<minPercent)
    {
        testDescription = O2Name+"  has a delta percentage too low";
        Log(LOG_DEV_DATA, "%s has a delta percentage too low - First=%.3f, Last=%.3f, Min%%=%.2f%%", 
            O2Name.c_str(), FLVals[0], FLVals[1], minPercent);
        testResult = testFail;
    }
    if (deltaPercent>maxPercent)
    {
        testDescription = O2Name+"  has a delta percentage too high";
        Log(LOG_DEV_DATA, "%s has a delta percentage too high - First=%.3f, Last=%.3f, Max%%=%.2f%%", 
            O2Name.c_str(), FLVals[0], FLVals[1], maxPercent);
        testResult = testFail;
    }

    if (testResult != testFail)
        testResult = testPass;

    SendSubtestResultWithDetail(GetTestStepName()+"_"+O2Name, testResult, testDescription, "0000", 
                                "FirstValue", CreateMessage(firstValStr, sizeof(firstValStr), "%.3f", FLVals[0]), "V", 
                                "LastValue", CreateMessage(lastValStr, sizeof(lastValStr), "%.3f", FLVals[1]), "V",
                                "MinDeltaPercent", CreateMessage(minPercentStr, sizeof(minPercentStr), "%.2f", minPercent), "%",
                                "MaxDeltaPercent", CreateMessage(maxPercentStr, sizeof(maxPercentStr), "%.2f", maxPercent), "%",
                                "DeltaPercent", CreateMessage(deltaPercentStr, sizeof(deltaPercentStr), "%.2f", deltaPercent), "%");

    return testResult;

}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::ClearFaults(void)
{
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ClearFaults() - Enter");
    string result(BEP_TESTING_RESPONSE);
    string testResult(BEP_TESTING_RESPONSE);
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    //Short Circuit not needed - if test previously passed will skip in CommandTestStepFunction
    //if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    //{   // Read the current fault count from the module
    //UINT8 faultCount = 0xFF;
    //BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("ReadFaultCount", faultCount);
    //if(BEP_STATUS_SUCCESS == status)
    //{
    bool isLocked = true;
    // Attempt to read the locked status from the module
    try
    {   // Read the locked status from the module
        m_vehicleModule.ReadModuleData("IsModuleLocked", isLocked);
    }
    catch (ModuleException &exception)
    {   // Exception reading data
        Log(LOG_ERRORS, "Module exception in ClearFaults() while reading IsModuleLocked - %s\n", exception.message().c_str());
        isLocked = true;
    }
    result = testPass;
    //Log(LOG_DEV_DATA, "DTC Count: %d", faultCount);
    Log(LOG_DEV_DATA, "Module Locked: %s", (isLocked ? "True" : "False"));
    // Determine if testing should be aborted
    //if(faultCount > 0 && !isLocked)
    if ((!isLocked || GetTestStepInfoBool("IgnoreModuleLockedStatus")) && 
        (!GetTestStepInfoBool("CheckCompletePassStatus") || 
         ((GetOverallResult() == testPass) && SystemRead("CurrentTestStatus") == BEP_PASS_STATUS)))
    {
        try
        {   // Tell the module to clear faults
            moduleStatus = m_vehicleModule.ClearFaults();
            // Determine the test results
            if (moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
            testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = 
            (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
            // Log the data
            Log(LOG_DEV_DATA, "Clear Faults: %s - status: %s\n",
                testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
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
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        testResult = testSkip;
    }
    //}
    //else
    //{
    //    Log(LOG_ERRORS, "Could not read fault count from the module: %s", ConvertStatusToResponse(status).c_str());
    //    result = testFail;
    //    testResult = testFail;
    //    testResultCode = GetFaultCode("CommunicationFailure");
    //    testDescription = GetFaultDescription("CommunicationFailure");
    //    SendTestResult(testResult, testDescription, testResultCode);
    //}
    // Report the result
    //char buff[8];
    //SendSubtestResultWithDetail(GetTestStepName() + "ReadFaultCount", result, GetTestStepInfo("Description"), "0000",
    //                         "DtcCount", CreateMessage(buff, sizeof(buff), "%d", faultCount), "");
    //}
    //else if(!GetTestStepResult().compare(testPass))
    //{   // This test already passed, skip it but leave it marked as pass
    //    Log(LOG_DEV_DATA, "Already passed clear faults, not testing again");
    //    testResult = testPass;
    //}
    //else
    //{   // Need to skip this step
    //    Log(LOG_FN_ENTRY, "Skipping clear faults");
    //    testResult = testSkip;
    //}
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "IsuzuEmissionsTc::ClearFaults() - Exit");
    return testResult;

}
//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::GetSerialNumberFromFile(void)
{
    string serialNumber = "";
    string serialNumberDirectory = GetParameter("SerialNumberDirectory");
    string vin = ReadSubscribeData(GetDataTag("VehicleVIN"));
    DIR *snDir;
    struct dirent *dirEntry;
    FILE *snFile;
    char buff[20];

    Log(LOG_FN_ENTRY, "Enter IsuzuEmissionsTc::GetSerialNumberFromFile() serial number directory: %s vin: %s\n",
        serialNumberDirectory.c_str(), vin.c_str());
    try
    {
        //get list of files in directory
        // Attempt to open the directory
        if ((snDir = opendir(serialNumberDirectory.c_str())) != NULL)
        {   // Get each file in the directory

            while ((dirEntry = readdir(snDir)) != NULL)
            {   //check if name of file start matches current vin
                string name(dirEntry->d_name);
                if (!name.compare(0,17,vin))
                {//match
                    name = serialNumberDirectory + name;
                    Log(LOG_DEV_DATA, "Opening file: %s", name.c_str());
                    if ((snFile = fopen(name.c_str(), "r")) != NULL)
                    {   // Add the header to the file
                        //serialNumber = name.substr(17,16);
                        if ( fgets (buff , 17 , snFile) != NULL )
                        {
                            serialNumber = buff;
                            Log(LOG_DEV_DATA, "SerialNumberExtracted: %s", serialNumber.c_str());
                            fclose(snFile);
                            return serialNumber;
                        }
                        else
                        {
                            Log(LOG_ERRORS, "Failed to get sn from file\n");
                        }
                    }
                    else
                    {
                        Log(LOG_ERRORS, "Failed to open sn file: %s\n", name.c_str());
                    }
                }
            }
        }
        else
        {   // Error opening the directory
            Log(LOG_ERRORS, "Failed to open directory: %s\n", serialNumberDirectory.c_str());
        }
    }
    catch (...)
    {
        Log(LOG_ERRORS, "Exception occured when trying to obtain serial number\n");
    }

    Log(LOG_FN_ENTRY, "Exit IsuzuEmissionsTc::GetSerialNumberFromFile() serial number: %s\n",
        serialNumber.c_str());
    return serialNumber;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::CheckSerialNumber(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string moduleSerialNumber;
    string leadingCharacters = ReadSubscribeData("ESNLeadingCharacters");
    string fileSerialNumber = "";

    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter IsuzuEmissionsTc::CheckSerialNumber()\n");
    if (!ShortCircuitTestStep())
    {   // Do not need to skip
        try
        {//check if necessary   
            if (atob(ReadSubscribeData("WriteESN").c_str()))
            {
                // Read the part number from the module
                fileSerialNumber = GetSerialNumberFromFile();
				INT32 engineCodeStartLocation = 0;
				// Determine if the serial number needs to have a space removed
				if(fileSerialNumber[0] == '1')
				{   // Remove space from serial number
					fileSerialNumber = fileSerialNumber.substr(0,5) + fileSerialNumber.substr(6,10);
					engineCodeStartLocation = GetParameterInt("EngineCodeStartLocation_1D");
				}
				else
				{
					engineCodeStartLocation = GetParameterInt("EngineCodeStartLocation_2D");
				}
				Log(LOG_DEV_DATA, "Checking for engine code %s starting at index %d",
					leadingCharacters.c_str(), engineCodeStartLocation);

                if (fileSerialNumber.length() == GetParameterInt("ESNLength"))                
                {
                    if (!fileSerialNumber.compare(engineCodeStartLocation,leadingCharacters.length(),leadingCharacters))
                    {
                        moduleStatus = m_vehicleModule.ReadModuleData("ReadSerialNumber", moduleSerialNumber);
                        // Check the status of the data
                        if (BEP_STATUS_SUCCESS == moduleStatus)
                        {   // Good data, Check serial number
                            if (moduleSerialNumber == fileSerialNumber)
                            {   // Part numbers match, test passes
                                testResult = testPass;
                            }
                            else
                            {   // Part number do not match, test fails
                                testResult = testFail;
                            }
                            // Log the data
                            Log(LOG_DEV_DATA, "Serial Number Verification: %s - file: %s, Module: %s\n",
                                testResult.c_str(), fileSerialNumber.c_str(), moduleSerialNumber.c_str());
                            testResultCode = (testResult == testPass ? "0000" : GetFaultCode("SerialNumberMismatch"));
                            testDescription = 
                            (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("SerialNumberMismatch"));
                        }
                        else
                        {   // Error getting data from the module
                            testResult = testFail;
                            testResultCode = GetFaultCode("CommunicationFailure");
                            testDescription = GetFaultDescription("CommunicationFailure");
                            SetCommunicationFailure(true);
                            Log(LOG_ERRORS, "Error reading module serial number - status: %s\n",
                                ConvertStatusToResponse(moduleStatus).c_str());
                        }
                    }
                    else
                    {//ESN leading characters do not match -fail
                        testResult = testFail;
                        testResultCode = GetFaultCode("ESNLeadingCharacterMismatch");
                        testDescription = GetFaultDescription("ESNLeadingCharacterMismatch");
                        Log(LOG_ERRORS, "ESN Leading Characters not correct\n");
                    }
                }
                else
                {//ESN Length not correct
                    testResult = testFail;
                    testResultCode = GetFaultCode("ESNLengthNotCorrect");
                    testDescription = GetFaultDescription("ESNLengthNotCorrect");
                    Log(LOG_ERRORS, "ESN Length not correct\n");
                }
            }
            else
            {//ESN Check not necessary
                testResult = testSkip;
                Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
            }
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testFail;
        }
        // Send the test result
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "RequiredLeadingCharacters", leadingCharacters, "",
                                 "ModuleSerialNumber", moduleSerialNumber, "",
                                 "FileSerialNumber", fileSerialNumber, "");
        //abort after result is sent so failure is recorded
        if (GetParameterBool("AbortOnSerialMismatch") && (testResult == testFail))
        {   // Part number do not match, test aborts
            testResult = testAbort;
        }
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    if (testResult == testAbort)
    {
        SystemWrite(ABORT_DATA_TAG, "1");
    }
    Log(LOG_FN_ENTRY, "Exit IsuzuEmissionsTc::CheckSerialNumber()\n");
    // Return the status
    return(testResult);
}


//-----------------------------------------------------------------------------
template <class ModuleType>
string IsuzuEmissionsTc<ModuleType>::KeyOffEngineOffKeyOn(void)
{
    string testResult(BEP_TESTING_RESPONSE);

    if (!IsRetest())
    {
        testResult = GenericTCTemplate<ModuleType>::KeyOffEngineOffKeyOn();
    }
    else
    {
        testResult = BEP_SKIP_RESPONSE;
    }

    return testResult;
}
