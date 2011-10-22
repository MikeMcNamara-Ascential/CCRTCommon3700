//*************************************************************************
// FILE:
//    $Header: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/KoreaAbsTcTemplate.cpp 1     3/09/06 3:15p Gswope $
//
// FILE DESCRIPTION:
//    This class extends the functionality of the TestComponent.
//    @author Ross Wiersema
//    @class KoreaABSTCTemplate
//
//    @since 15 December 2003
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
//    $Log: /HMMA/Source/HMMA/ApplicationLayer/TestComponents/Brake/Bosch8/KoreaAbsTcTemplate.cpp $
// 
// 1     3/09/06 3:15p Gswope
// After changes to bring HMMA up to "current core"
// 
// 3     3/16/05 10:49a Gswope
// Added RecalculateDragBeforeEsp option
// Removed call to AnalyzeESPValveCross at customer (Oscar) request
// Re-installed publication of ESP forces to front panel
// 
// 2     3/02/05 11:18a Gswope
// Emergency HMMA request to remove rear valves from ESP test
// 2005.2.28 ews changed per emergency HMMA request
// made ESP valve firing and evaluation use only the front two wheels.
// EvaluateESP()  and  AnalyzeESPValveCross()
// 
// 1     1/18/05 10:49a Gswope
// First Entry into this project in SourceSafe
// 
// 5     11/24/04 11:04a Bmeinke
// Fixed a bug in the EvaluateESP() method where a failed ESP test would
// be reported as a pass
// 
// 3     11/15/04 9:23a Bmeinke
// Changed AnalyzeESPBuildForces() and AnalyzeESPReductionForces() to
// check the IncludeDragInBrakeForce parameter so we optionally remove
// drag force value from the ESP calculations
// 
// 2     10/27/04 8:57p Bmeinke
// AnalyzeESPReductionForces() and AnalyzeESPRecoveryForces() methods need
// to use the IncludeDragInBrake parameter when calculating the ESP forces
// 
// 1     10/27/04 5:40p Bmeinke
// 
// 2     10/26/04 1:58p Bmeinke
// In the SensorTest methods, get a float parameter for SensortestSpeed to
// avoid repetative atoi() calls and initialize the rollSpeed[xx] elements
// before testing to see if it is greater than the sensor test target
// speed
// 
// 1     8/30/04 7:24p Bmeinke
// First successful attempt at talking to the Bosch8 ABS module
//*************************************************************************

#include "KoreaAbsTcTemplate.h"


template <class VehicleModuleType>
KoreaAbsTcTemplate<VehicleModuleType>::KoreaAbsTcTemplate() : GenericABSTCTemplate<VehicleModuleType>()
{
}

template <class VehicleModuleType>
KoreaAbsTcTemplate<VehicleModuleType>::~KoreaAbsTcTemplate()
{
}

template <class VehicleModuleType>
const string KoreaAbsTcTemplate<VehicleModuleType>::CommandTestStep(const string &value)
{
    string status;             // status of the test step

    try
    {
        string step = GetTestStepName(); // get the test step we are on
        Log(LOG_DEV_DATA,"KoreaAbsTcTemplate::CommandTestStep(%s): Entering test step %s\n",
            value.c_str(),step.c_str());

        if (StatusCheck() != BEP_STATUS_SUCCESS)      // make sure status is OK
        {
            UpdateResult(StatusCheck(),status);
            Log(LOG_ERRORS,"KoreaAbsTcTemplate::CommandTestStep: StatusCheck() failed: %s\n",
                status.c_str());
            SendTestResult(status,GetTestStepInfo("Description"));
        }
        else if (step == "CheckPartNumber") status = CheckPartNumber();
        else if (step == "LFSensorTest")  status = LFSensorTest();
        else if (step == "RFSensorTest")  status = RFSensorTest();
        else if (step == "LRSensorTest")  status = LRSensorTest();
        else if (step == "RRSensorTest")  status = RRSensorTest();
        else if (step == "SensorQualityTest") status = SensorQualityTest();
        else if (step == "HydraulicSensorTest") status = HydraulicSensorTest();
        else if (step == "Coast") status = TestStepCoast();
        else if (step == "EvaluateABS") status = EvaluateABS();
        else if (step == "EvaluateTCS") status = EvaluateTCS();
        else if (step == "EvaluateESP") status = EvaluateESP();
        else
        {
            status = GenericABSTCTemplate<VehicleModuleType>::CommandTestStep(value);
        }
    }
	catch (ModuleException &ex)
	{
		Log(LOG_ERRORS,"KoreaAbsTcTemplate::CommandTestStep %s ModuleException %s\n",
			GetTestStepName().c_str(),ex.GetReason());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
    catch (BepException &ex)
    {
        Log(LOG_ERRORS,"KoreaAbsTcTemplate::CommandTestStep %s BepException %s\n",
            GetTestStepName().c_str(),ex.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA,"KoreaAbsTcTemplate::CommandTestStep(%s): Returning %s\n",
        value.c_str(),status.c_str());
    return(status);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::CheckPartNumber(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string modulePartNumber;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter KoreaAbsTcTemplate::CheckPartNumber()\n");
    if (!ShortCircuitTestStep())
    {                       // Do not need to skip
        try
        {                    // Read the part number from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModulePartNumber"), modulePartNumber);
            // Check the status of the data
            if (BEP_STATUS_SUCCESS == moduleStatus)
            {
                if (modulePartNumber == GetParameter("ModulePartNumber"))
                {              // Part numbers match, test passes
                    testResult = testPass;
                }
                else
                {              // Part number do not match, test fails
                    testResult = testFail;
                }
                // Log the data
                Log(LOG_DEV_DATA, "Part Number Verification: %s - Parameter: %s, Module: %s\n",
                    testResult.c_str(), GetParameter("ModulePartNumber").c_str(), modulePartNumber.c_str());
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
                                 "PartNumberParameter", GetParameter("ModulePartNumber"), "");
    }
    else
    {                       // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "Exit KoreaAbsTcTemplate::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::LFSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // delay time before starting the roll
    UINT32 startDelay = GetParameterInt("SensorTestStartDelay");
    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::LFSensorTest - Enter\n");

    try
    {
        UpdatePrompts();        // "Shift to neutral and release brake"
        BposSleep(startDelay);     // give driver time to shift

        // Command all rolls to speed mode
        m_MotorController.Write(std::string("LeftFrontMotorMode"),SPEED_MODE, false);
        m_MotorController.Write(std::string("RightFrontMotorMode"),SPEED_MODE, false);
        m_MotorController.Write(std::string("LeftRearMotorMode"),SPEED_MODE, false);
        m_MotorController.Write(std::string("RightRearMotorMode"),SPEED_MODE, false);
        // Command the left front roll to the sensor test speed
        m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightFrontSpeedValue"),std::string("0"), false);
        m_MotorController.Write(std::string("LeftRearSpeedValue"),std::string("0"), false);
        m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);

        std::string tag, value;
        while (m_MotorController.GetNext(tag, value) > 0)
        {
            Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
        }

        // wait for the left front roll to reach the sensor test speed
        rollSpeeds[ 0] = 0;
        while (rollSpeeds[0] < (sensorTestSpeedVal - 2))
        {
            BposSleep(100);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
            if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
        }

        // Ask the module for sensor speeds
        status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

        if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
        {
            // check front sensor cross side to side
            if (moduleSpeeds[LFWHEEL] <= moduleSpeeds[RFWHEEL])
            {
                testResult = testFail;
            }
            else testResult   = testPass;
        }
        else                 // error reading the sensors
        {
            SetCommunicationFailure(true);     // set comm fault flag
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
                                     GetFaultName(testDescription),
                                     GetFaultDescription(testDescription));
        }
    }

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::LFSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::RFSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::RFSensorTest - Enter\n");

    try
    {
        UpdatePrompts();        // "Shift to neutral and release brake"

        // Command the right front roll to the sensor test speed
        m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("LeftRearSpeedValue"),std::string("0"), false);
        m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);

        std::string tag, value;
        while (m_MotorController.GetNext(tag, value) > 0)
        {
            Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
        }

        // wait for the right front roll to reach the sensor test speed
        rollSpeeds[ 1] = 0;
        while (rollSpeeds[1] < (sensorTestSpeedVal - 2))
        {
            BposSleep(100);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
            if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
        }

        // Ask the module for sensor speeds
        status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

        if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
        {
            // check front to rear sensor cross
            if ((moduleSpeeds[RFWHEEL] <= moduleSpeeds[RRWHEEL]) ||
                (moduleSpeeds[RFWHEEL] <= moduleSpeeds[LRWHEEL]))
            {
                testResult = testFail;
            }
            else testResult   = testPass;
        }
        else                 // error reading the sensors
        {
            SetCommunicationFailure(true);     // set comm fault flag
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
                                     GetFaultName(testDescription),
                                     GetFaultDescription(testDescription));
        }
    }

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::RFSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::LRSensorTest(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];
    WheelSpeeds_t moduleSpeeds;

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::LRSensorTest - Enter\n");

    try
    {
        UpdatePrompts();        // "Shift to neutral and release brake"

        // Command the right front roll to the sensor test speed
        m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("LeftRearSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightRearSpeedValue"),std::string("0"), true);

        std::string tag, value;
        while (m_MotorController.GetNext(tag, value) > 0)
        {
            Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
        }

        // wait for the right front roll to reach the sensor test speed
        rollSpeeds[ 2] = 0;
        while (rollSpeeds[2] < (sensorTestSpeedVal - 2))
        {
            BposSleep(100);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
            if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
        }

        // Ask the module for sensor speeds
        status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

        if (status == BEP_STATUS_SUCCESS) // if sensors read successfully
        {
            // check rear sensor cross side to side
            if (moduleSpeeds[LRWHEEL] <= moduleSpeeds[RRWHEEL])
            {
                testResult = testFail;
            }
            else testResult   = testPass;
        }
        else                 // error reading the sensors
        {
            SetCommunicationFailure(true);     // set comm fault flag
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
                                     GetFaultName(testDescription),
                                     GetFaultDescription(testDescription));
        }
    }

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::LRSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::RRSensorTest(void)
{
    string testResult      = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    float rollSpeeds[4];

    // sensor test speed
    std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
    float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::RRSensorTest - Enter\n");

    try
    {
        UpdatePrompts();        // "Shift to neutral and release brake"

        // Command the right front roll to the sensor test speed
        m_MotorController.Write(std::string("LeftFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightFrontSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("LeftRearSpeedValue"),sensorTestSpeed, false);
        m_MotorController.Write(std::string("RightRearSpeedValue"),sensorTestSpeed, true);

        std::string tag, value;
        while (m_MotorController.GetNext(tag, value) > 0)
        {
            Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
        }

        // wait for the right front roll to reach the sensor test speed
        rollSpeeds[ 3] = 0;
        while (rollSpeeds[3] < (sensorTestSpeedVal - 1))
        {
            BposSleep(100);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
            if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)
            {
                testResult = testFail;
                break;
            }
        }

        // if the test result was not set to failed, now set to pass
        if (testResult == BEP_TESTING_STATUS) testResult = testPass;
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
                                 GetFaultName(testDescription),
                                 GetFaultDescription(testDescription));
    }

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::RRSensorTest - Exit %s\n",testResult.c_str());

    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::SensorQualityTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    WheelSpeeds_t speeds;
    string wheelResult[4];
    const float minTolerance = GetParameterFloat("SensorQualityMinTolerance");
    const float maxTolerance = GetParameterFloat("SensorQualityMaxTolerance");
    const float targetSpeed = GetParameterFloat("SensorQualitySpeed");

    Log(LOG_FN_ENTRY, "Enter KoreaAbsTcTemplate::SensorQualityTest()\n");
    try
    {
        // Try to start the sensor quality test
        moduleStatus = m_vehicleModule.GetInfo("ReadSensorSpeeds",speeds);

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if (testResult == testPass)
        {
            Log(LOG_DEV_DATA, "Sensor Quality Speeds: LF = %g RF = %g LR = %g RR = %g\n",
                speeds[LFWHEEL],speeds[RFWHEEL],speeds[LRWHEEL],speeds[RRWHEEL]);

            for (UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
            {
                // Calculate the upper and lower wheel speed limits
                float lowerLimit = targetSpeed - minTolerance;
                float upperLimit = targetSpeed + maxTolerance;

                // Check the wheel speed sensor against the limits
                if (speeds[wheelIndex] > upperLimit)
                {
                    wheelResult[wheelIndex] = testFail;
                    testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                    testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                }
                else if (speeds[wheelIndex] < lowerLimit)
                {
                    wheelResult[wheelIndex] = testFail;
                    testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                    testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                }
                else
                {
                    wheelResult[wheelIndex] = testPass;
                    testResultCode = "0000";
                    testDescription = rollerName[wheelIndex] + " wheel speed sensor in tolerance\n";
                }

                // Log the data and report the result for this wheel
                Log(LOG_DEV_DATA, "%s wheel speed sensor %s Lower Limit: %.2f Upper Limit: %.2f"
                    "\nSensor Speed: %.2f\n",
                    rollerName[wheelIndex].c_str(), wheelResult[wheelIndex].c_str(), 
                    lowerLimit, upperLimit, speeds[wheelIndex]);

                char temp[16];
                SendSubtestResultWithDetail(rollerName[wheelIndex]+GetTestStepName(), wheelResult[wheelIndex], 
                                            testDescription, testResultCode,
                                            "SensorSpeed", CreateMessage(temp, sizeof(temp), "%.2f", speeds[wheelIndex]), "MPH",
                                            "LowerLimit", CreateMessage(temp, sizeof(temp), "%.2f", lowerLimit), "MPH",
                                            "UpperLimit", CreateMessage(temp, sizeof(temp), "%.2f", upperLimit), "MPH");
                // Update the overall result
                testResult = testPass == wheelResult[wheelIndex] ? testResult : wheelResult[wheelIndex];
            }

            // Determine the description and code to report
            testResult = BEP_TESTING_STATUS == testResult ? testPass : testFail;
            testResultCode = testPass == testResult ? "0000" : GetFaultCode(GetTestStepName()+"Fail");
            testDescription = testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription(GetTestStepName()+"Fail");
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
    catch (ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in KoreaAbsTcTemplate::SensorQualityTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // command the drives to zero torque	
    Log(LOG_DEV_DATA, "commanding torque to zero\n");
    SystemCommand(COMMAND_TORQUE, 0);    

    // command the drives to zero speed	
    Log(LOG_DEV_DATA, "commanding speed to zero\n");
    SystemCommand(COMMAND_SPEED, 0);

    // wait for zerospeed
    while ((ReadSubscribeData(GetDataTag("Zerospeed")) == "1") && 
           (TimeRemaining()) && (StatusCheck() == BEP_STATUS_SUCCESS))
    {
        BposSleep(100);
    }

    // command the motor controller to boost mode    
    SystemCommand(MOTOR_MODE, string(BOOST_MODE));

    RemovePrompts();

    // if communication failure or software failure, send test result
    if ((GetCommunicationFailure() == true) || (testResult == testSoftwareFail))
    {
        SendTestResult(testResult, testDescription, testResultCode);
    }

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit KoreaAbsTcTemplate::SensorQualityTest()\n");
    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::HydraulicSensorTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    WheelSpeeds_t moduleSpeeds;
    string wheelResult[4];
    float rollSpeeds[4] = {0.0,0.0,0.0,0.0};
    const UINT32 startDelay = GetParameterInt("SensorTestStartDelay");
    const float minTolerance = GetParameterFloat("WheelSpeedsMinTolerance");
    const float maxTolerance = GetParameterFloat("WheelSpeedsMaxTolerance");
    const float targetSpeed = GetParameterFloat("WheelSpeedsTargetSpeed");

    Log(LOG_DEV_DATA, "KoreaAbsTcTemplate::HydraulicSensorTest - Enter\n");

    try
    {
        UpdatePrompts();        // "shift to neutral foot off brake"
        BposSleep(startDelay);     // give driver time to react

        // turn on the sensor test bit to the PLC
        SystemWrite(string("StartEBSSensroTest"),true);

        // wait for the front rolls to reach speed
        while ((rollSpeeds[0] < 2) && (rollSpeeds[1] < 2))
        {
            BposSleep(100);
            m_baseBrakeTool->GetISpeeds(rollSpeeds);
            if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
        }

        // Ask the module for sensor speeds
        moduleStatus = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
        if (moduleStatus == BEP_STATUS_SUCCESS)   // if sensors read successfully
        {
            if ((moduleSpeeds[LFWHEEL] < moduleSpeeds[LRWHEEL]) || 
                (moduleSpeeds[LFWHEEL] < moduleSpeeds[RRWHEEL]) ||
                (moduleSpeeds[RFWHEEL] < moduleSpeeds[LRWHEEL]) ||
                (moduleSpeeds[RFWHEEL] < moduleSpeeds[RRWHEEL]))
            {
                testResult = testFail;
                testDescription = "FaultFrontToRearSensorCross";
                SendTestResult(testResult, testDescription, testResultCode);
            }
            else testResult   = testPass;
        }
        else                 // error reading the sensors
        {
            SetCommunicationFailure(true);     // set comm fault flag
            testDescription = GetFaultDescription("CommunicationFailure");
            testResult = testFail;
        }

        if (testResult == testPass) // if okay so far
        {
            // wait for the rear rolls to reach speed
            while ((rollSpeeds[2] < 2) && (rollSpeeds[3] < 2))
            {
                BposSleep(100);
                m_baseBrakeTool->GetISpeeds(rollSpeeds);
                if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
            }
            BposSleep(1000);     // give rolls time to stabilize

            // Ask the module for sensor speeds
            moduleStatus = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
            if (moduleStatus == BEP_STATUS_SUCCESS)   // if sensors read successfully
            {
                // Calculate the upper and lower wheel speed limits
                float lowerLimit = targetSpeed - minTolerance;
                float upperLimit = targetSpeed + maxTolerance;

                for (UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
                {
                    // Check the wheel speed sensor against the limits
                    if (moduleSpeeds[wheelIndex] > upperLimit)
                    {
                        wheelResult[wheelIndex] = testFail;
                        testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                        testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
                        testResult = testFail;
                    }
                    else if (moduleSpeeds[wheelIndex] < lowerLimit)
                    {
                        wheelResult[wheelIndex] = testFail;
                        testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                        testDescription = GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
                        testResult = testFail;
                    }
                    else
                    {
                        wheelResult[wheelIndex] = testPass;
                        testResultCode = "0000";
                        testDescription = rollerName[wheelIndex] + " wheel speed sensor in tolerance\n";
                        testResult = testPass;
                    }

                    // Log the data and report the result for this wheel
                    Log(LOG_DEV_DATA, "%s wheel speed sensor %s Lower Limit: %.2f Upper Limit: %.2f"
                        "\nSensor Speed: %.2f\n",
                        rollerName[wheelIndex].c_str(), wheelResult[wheelIndex].c_str(), 
                        lowerLimit, upperLimit, moduleSpeeds[wheelIndex]);

                    char temp[16];
                    SendSubtestResultWithDetail(rollerName[wheelIndex]+GetTestStepName(), wheelResult[wheelIndex], 
                                                testDescription, testResultCode,
                                                "SensorSpeed", CreateMessage(temp, sizeof(temp), "%.2f", moduleSpeeds[wheelIndex]), "MPH",
                                                "LowerLimit", CreateMessage(temp, sizeof(temp), "%.2f", lowerLimit), "MPH",
                                                "UpperLimit", CreateMessage(temp, sizeof(temp), "%.2f", upperLimit), "MPH");
                }
            }
            else              // error reading the sensors
            {
                SetCommunicationFailure(true);     // set comm fault flag
                testDescription = GetFaultDescription("CommunicationFailure");
                testResult = testFail;
            }

            if (testResult == testPass)   // if okay so far
            {
                // wait for the left side rolls to slow down
                while ((rollSpeeds[0] < 3) && (rollSpeeds[2] < 3))
                {
                    BposSleep(100);
                    m_baseBrakeTool->GetISpeeds(rollSpeeds);
                    if (!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS) break;
                }
                BposSleep(1000);   // give rolls time to stabilize

                // Ask the module for sensor speeds
                moduleStatus = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
                if (moduleStatus == BEP_STATUS_SUCCESS)   // if sensors read successfully
                {
                    if (moduleSpeeds[LFWHEEL] > moduleSpeeds[RRWHEEL])
                    {
                        testResult = testFail;
                        testDescription = "FaultFrontSensorCross";
                        SendTestResult(testResult, testDescription, testResultCode);
                    }
                    else testResult   = testPass;

                    if (moduleSpeeds[LRWHEEL] > moduleSpeeds[RRWHEEL])
                    {
                        testResult = testFail;
                        testDescription = "FaultRearSensorCross";
                        SendTestResult(testResult, testDescription, testResultCode);
                    }
                    else testResult   = testPass;
                }
                else           // error reading the sensors
                {
                    SetCommunicationFailure(true);     // set comm fault flag
                    testDescription = GetFaultDescription("CommunicationFailure");
                    testResult = testFail;
                }
            }
        }

        // Turn off the sensor test bit to the PLC
        SystemWrite(string("StartEBSSensorTest"),false);

        // wait for zerospeed
        while ((ReadSubscribeData(GetDataTag("Zerospeed")) == "1") && 
               (TimeRemaining()) && (StatusCheck() == BEP_STATUS_SUCCESS))
        {
            BposSleep(100);
        }
    }
    catch (ModuleException &excpt)
    {
        Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // if communication failure or software failure, send test result
    if ((GetCommunicationFailure() == true) || (testResult == testSoftwareFail))
    {
        SendTestResult(testResult, testDescription, testResultCode);
    }
    return(testResult);
}

template <class VehicleModuleType>
const string KoreaAbsTcTemplate<VehicleModuleType>::TestStepCoast(void)
{
    INT32 testStatus = BEP_STATUS_SUCCESS;   // the status of the test being performed

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::TestStepCoast()\n");
    try
    {
        // make sure everything is groovy with the machine
        testStatus = StatusCheck();
        if (testStatus == BEP_STATUS_SUCCESS)
        {
            // set up the display (speed gauge)
            testStatus = SystemWrite(GetDataTag("SpeedTarget"), GetParameter("CoastSpeedTarget"));
            // display driver prompts
            if (UpdatePrompts() != BEP_STATUS_SUCCESS)
                Log(LOG_ERRORS, "Unable to Update Prompts\n");
            // perform Coast
            testStatus = Coast();
            // remove the speed target from the gauge
            SystemWrite(GetDataTag("SpeedTarget"), string("0 0"));
        }
        else
            Log(LOG_ERRORS, "Invalid Machine Status For KoreaAbsTcTemplate::TestStepCoast %d\n", testStatus);
    }
    catch (BepException &e)
    {
        Log(LOG_ERRORS, "TestStepCoast Exception: %s\n", e.what());
        testStatus = BEP_STATUS_SOFTWARE;
    }

    // update the test status
    string status;
    UpdateResult(testStatus, status);
    SendTestResult(status, GetTestStepInfo("Description"));

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::TestStepCoast(): %s, %d\n", status.c_str(), testStatus);

    return(status);
}

template <class VehicleModuleType>
INT32 KoreaAbsTcTemplate<VehicleModuleType>::Coast(void)
{
    INT32 testStatus = BEP_STATUS_FAILURE;

    float speed=0;             // speed read from the machine
    bool done = false;            // flag to indicate if done

    // Get the coast speed
    float coastSpeed = GetParameterFloat("CoastEndSpeed");

    // Tag the array so we can see that a coas occured
    TagArray("CoastStart");

    do
    {                       // get the wheel with the slowest speed
        speed = m_baseBrakeTool->GetSpeed();                
        // else check the status of the component and delay
        if (speed < coastSpeed)  done = true;
        else
        {
            testStatus = StatusCheck();
            if (testStatus == BEP_STATUS_SUCCESS)   BposSleep(100);
        }
    }while ((!done) && (testStatus == BEP_STATUS_SUCCESS) && TimeRemaining());

    // if for some reason, the speed never came down before the system had an error or a 
    if (done)             testStatus = BEP_STATUS_SUCCESS;
    else if (!TimeRemaining())  testStatus = BEP_STATUS_TIMEOUT;
    else                 testStatus = BEP_STATUS_FAILURE;

    return(testStatus); 
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::EvaluateABS(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    string wheelReduxResults[GetRollerCount()];
    string wheelRecovResults[GetRollerCount()];
    string overallRedux = testPass, overallRecov = testPass;
    string valveCrossResult = BEP_TESTING_STATUS;
    // Log the function exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if t his step should be performed
    if ((!ShortCircuitTestStep() && (BEP_TESTING_STATUS == m_baseBrakeTool->GetBrakeTestingStatus())) ||
        GetParameterBool("DeveloperTest"))
    {                       // Make sure some samples were taken
        if (m_absEndIndex > m_absStartIndex)
        {                    // Read the data from the brake force array
            INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"),
                                                                           m_absStartIndex, m_absEndIndex);
            // Check the status of the read
            if (BEP_STATUS_SUCCESS == dataStatus)
            {                 // Evaluate the reduction and recovery values
                              // NOTE: The analyze methods report detailed results for each wheel
                for (UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
                {              // Find the actual start and end reduction indices.
                               // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 reductionStart = m_reduxRecovIndex[wheelIndex].reductionStart - m_absStartIndex;
                    INT32 reductionEnd   = m_reduxRecovIndex[wheelIndex].reductionEnd   - m_absStartIndex;
                    Log(LOG_DEV_DATA, "%s Reduction -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
                        reductionStart, reductionEnd);
                    BEP_STATUS_TYPE reductionStatus = AnalyzeReductionForces(wheelIndex, reductionStart, reductionEnd);
                    wheelReduxResults[wheelIndex] = BEP_STATUS_SUCCESS == reductionStatus ? testPass : testFail;
                    // Find the actual start and end recovery indices.
                    // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 recoveryStart = m_reduxRecovIndex[wheelIndex].recoveryStart - m_absStartIndex;
                    INT32 recoveryEnd   = m_reduxRecovIndex[wheelIndex].recoveryEnd   - m_absStartIndex;
                    Log(LOG_DEV_DATA, "%s Recovery -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
                        recoveryStart, recoveryEnd);
                    BEP_STATUS_TYPE recoveryStatus = AnalyzeRecoveryForces(wheelIndex, recoveryStart, recoveryEnd);
                    wheelRecovResults[wheelIndex] = BEP_STATUS_SUCCESS == recoveryStatus ? testPass : testFail;
                    // Update the overall results
                    overallRedux = ((testPass == wheelReduxResults[wheelIndex]) &&
                                    ((testPass == overallRedux) || (BEP_TESTING_STATUS == overallRedux))) ? overallRedux : testFail;
                    overallRecov = ((testPass == wheelRecovResults[wheelIndex]) &&
                                    ((testPass == overallRecov) || (BEP_TESTING_STATUS == overallRecov))) ? overallRecov : testFail;
                    Log(LOG_DEV_DATA, "%s Reducion: %s, Recovery: %s\n", rollerName[wheelIndex].c_str(),
                        wheelReduxResults[wheelIndex].c_str(), wheelRecovResults[wheelIndex].c_str());
                }
                // Perform the valve cross check
                BEP_STATUS_TYPE valveCrossStatus = AnalyzeValveCross();
                // Translate the status to a result
                switch (valveCrossStatus)
                {
                case BEP_STATUS_SKIP:         valveCrossResult = testSkip;  break;
                case BEP_STATUS_SUCCESS:      valveCrossResult = testPass;  break;
                default:                      valveCrossResult = testFail;  break;
                }
                Log(LOG_DEV_DATA, "Valve Cross: %s\n", valveCrossResult.c_str());
                // Determine the overall result of the testing
                testResult = ((overallRedux == testPass) && (overallRecov == testPass) &&
                              ((valveCrossResult == testPass) || (valveCrossResult == testSkip))) ? testPass : testFail;
                testResultCode = testResult == testPass ? "0000" : GetFaultCode("ABSFailure");
                testDescription = testResult == testPass ? testDescription : GetFaultDescription("ABSFailure");
            }
            else
            {                 // Error reading brake force data
                Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("WheelForceDataReadError");
                testDescription = GetFaultDescription("WheelForceDataReadError");
            }
        }
        else
        {                    // No samples were taken
            Log(LOG_ERRORS, "Not evaluating ABS reduction/recovery data - NO SAMPLES TAKEN!\n");
            testResult = testFail;
            testResultCode = GetFaultCode("NoABSData");
            testDescription = GetFaultDescription("NoABSData");
        }
        // Report the results of the test
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "OverallReduction", overallRedux, "",
                                 "OverallRecovery", overallRecov, "",
                                 "OverallValveCross", valveCrossResult, "");
    }
    else
    {                       // Need to skip this step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping %s\n\t\tShort Circuit Test Step: %s\n\t\tTesting Status: %s\n\t\tDeveloper Test: %s\n",
            GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
            m_baseBrakeTool->GetBrakeTestingStatus().c_str(),
            GetParameterBool("DeveloperTest") ? "True" : "False");
    }
    // Log the function exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit, result:%s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::EvaluateTCS(void)
{
    string testResult = testPass;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE buildStatus;
    BEP_STATUS_TYPE reductionStatus;
    string wheelBuildResults[2];
    string wheelReductionResults[2];
    string valveCrossResult;

    // Log the function exit and return the result
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::EvaluateTCS - Enter\n");

    if (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
    {
        if (m_TCSEndIndex > m_TCSStartIndex)    // if there are samples
        {
            // Read the data from the brake force array
            INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"), 
                                                                           m_TCSStartIndex,m_TCSEndIndex);
            if (dataStatus == BEP_STATUS_SUCCESS)
            {
                // Evaluate the build and reduction values
                for (INT32 wheelIndex = 0; wheelIndex < 2; wheelIndex++)
                {
                    // Find the actual start and end build indices.  
                    // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 buildStart = m_TCSIndex[wheelIndex].buildStart - m_TCSStartIndex;
                    INT32 buildEnd   = m_TCSIndex[wheelIndex].buildEnd   - m_TCSStartIndex;
                    Log(LOG_DEV_DATA, "%s Build -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
                        buildStart, buildEnd);

                    buildStatus = AnalyzeTCSBuildForces(wheelIndex,buildStart,buildEnd);
                    wheelBuildResults[wheelIndex] = BEP_STATUS_SUCCESS == buildStatus ? testPass : testFail;

                    // Find the actual start and end reduction indices.  
                    // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 reductionStart = m_TCSIndex[wheelIndex].reductionStart - m_TCSStartIndex;
                    INT32 reductionEnd   = m_TCSIndex[wheelIndex].reductionEnd   - m_TCSStartIndex;
                    Log(LOG_DEV_DATA, "%s Reduction -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(), 
                        reductionStart, reductionEnd);
                    reductionStatus = AnalyzeTCSReductionForces(wheelIndex,reductionStart,reductionEnd);
                    wheelReductionResults[wheelIndex] = BEP_STATUS_SUCCESS == reductionStatus ? testPass : testFail;
                }
                // Perform the valve cross check
                valveCrossResult = AnalyzeTCSValveCross() == BEP_STATUS_SUCCESS ? testPass : testFail;
                Log(LOG_DEV_DATA, "Valve Cross: %s\n", valveCrossResult.c_str());

                // set the overall build and reduction results
                if ((wheelBuildResults[0] == testFail) || (wheelBuildResults[1] == testFail)) buildStatus = BEP_STATUS_FAILURE;
                if ((wheelReductionResults[0] == testFail) || (wheelReductionResults[1] == testFail)) reductionStatus = BEP_STATUS_FAILURE;

                testResult = (buildStatus == BEP_STATUS_SUCCESS) && (reductionStatus == BEP_STATUS_SUCCESS) && (valveCrossResult == testPass) ? testPass : testFail;
            }
            else
            {                 // Error reading brake force data
                Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("WheelForceDataReadError");
                testDescription = GetFaultDescription("WheelForceDataReadError");
            }
        }
        else                 // No samples were taken
        {
            Log(LOG_ERRORS, "Not evaluating TCS build/reduction data - NO SAMPLES TAKEN!\n");
            testResult = testFail;
            testResultCode = GetFaultCode("NoTCSData");
            testDescription = GetFaultDescription("NoTCSData");
        }
    }

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::EvaluateTCS - Exit %s\n",testResult.c_str());
    return(testResult);
}

template <class VehicleModuleType>
string KoreaAbsTcTemplate<VehicleModuleType>::EvaluateESP(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE buildStatus;
    BEP_STATUS_TYPE reductionStatus;
    string wheelBuildResults[4];
    string wheelReductionResults[4];
    string valveCrossResult;
    UINT32 wheelIndex;

    // Log the function exit and return the result
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::EvaluateESP - Enter\n");

    if (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)
    {
        if (m_ESPEndIndex > m_ESPStartIndex)    // if there are samples
        {
            // Read the data from the brake force array
            INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"), 
                                                                           m_ESPStartIndex,m_ESPEndIndex);
            if (dataStatus == BEP_STATUS_SUCCESS)
            {
                // Evaluate the build and reduction values
// 2005.2.28 ews changed per emergency HMMA request
// changed to 2 to only evaluate the front wheels
                for (wheelIndex = 0; wheelIndex < 2; wheelIndex++)
                {
                    // Find the actual start and end build indices.  
                    // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 buildStart = m_ESPIndex[wheelIndex].buildStart - m_ESPStartIndex;
                    INT32 buildEnd   = m_ESPIndex[wheelIndex].buildEnd   - m_ESPStartIndex;
                    Log(LOG_DEV_DATA, "%s Build -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
                        buildStart, buildEnd);

// NOTE:  if the option RecalculateDragBeforeESP is set to 1 (true) in 
//        the config file you MUST call AnalyzeESPBuildForces() 
//        before calling AnalyzeESPReductionForces()


                    buildStatus = AnalyzeESPBuildForces(wheelIndex,buildStart,buildEnd);
                    wheelBuildResults[wheelIndex] = BEP_STATUS_SUCCESS == buildStatus ? testPass : testFail;

                    // Find the actual start and end reduction indices.  
                    // The stored indices are absolute, whereas the wheel force array is relative to start of test
                    INT32 reductionStart = m_ESPIndex[wheelIndex].reductionStart - m_ESPStartIndex;
                    INT32 reductionEnd   = m_ESPIndex[wheelIndex].reductionEnd   - m_ESPStartIndex;
                    Log(LOG_DEV_DATA, "%s Reduction -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(), 
                        reductionStart, reductionEnd);
                    reductionStatus = AnalyzeESPReductionForces(wheelIndex,reductionStart,reductionEnd);
                    wheelReductionResults[wheelIndex] = BEP_STATUS_SUCCESS == reductionStatus ? testPass : testFail;
                }

                // Perform the valve cross check
//               testResult = (AnalyzeESPValveCross() == BEP_STATUS_SUCCESS ? testPass : testFail);
                if (testFail == testResult)
                {
                    Log( LOG_DEV_DATA, "ESP valve cross check failed\n");
                }

                // Evaluate the build and reduction pass/fail
                for (wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
                {
                    if (testFail == wheelBuildResults[wheelIndex])
                    {
                        Log( LOG_DEV_DATA, "%s ESP build failed\n", rollerName[wheelIndex].c_str());
                        testResult = testFail;
                    }
                    if (testFail == wheelReductionResults[wheelIndex])
                    {
                        Log( LOG_DEV_DATA, "%s ESP reduction failed\n", rollerName[wheelIndex].c_str());
                        testResult = testFail;
                    }
                }
            }
            else
            {                 // Error reading brake force data
                Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
                testResult = testFail;
                testResultCode = GetFaultCode("WheelForceDataReadError");
                testDescription = GetFaultDescription("WheelForceDataReadError");
            }
        }
        else                 // No samples were taken
        {
            Log(LOG_ERRORS, "Not evaluating ESP build/reduction data - NO SAMPLES TAKEN!\n");
            testResult = testFail;
            testResultCode = GetFaultCode("NoESPData");
            testDescription = GetFaultDescription("NoESPData");
        }
    }

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::EvaluateESP - Exit %s\n",testResult.c_str());
    return(testResult);
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeESPBuildForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
    bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeESPBuildForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
        rollerName[roller].c_str(), start, end);

    if (end > start)               // if samples taken
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
        if (true == useNewDrag)
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
        }
        // get the parameter
        float buildMinValue = GetParameterFloat(rollerName[roller]+"MinESPBuildForce");

        if (buildValue > buildMinValue)     // if value passed
        {
            testResult = testPass;
            resultBG = colorGreen;
        }
        else                 // value failed
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
                                    rollerName[roller] + "ESPBuildValue", result, "LBF",
                                    rollerName[roller] + "ESPBuildMinForce", 
                                    CreateMessage(temp,sizeof(temp),"%7.2f",buildMinValue), "LBF");
        // Determine the status to return
        status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {
        status = BEP_STATUS_SKIP;
        Log(LOG_FN_ENTRY, "Skipping AnalyzeESPBuildForces -- End index is not greater than Start index!\n");
    }
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeESPBuildForces() done: %s\n", testResult.c_str());
    return(status);    
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeESPReductionForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
    bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeESPReductionForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
        rollerName[roller].c_str(), start, end);

    if (end > start)               // if samples taken
    {
        // get the machine losses for this wheel
        float machineLoss = SystemReadFloat(GetDataTag(rollerName[roller] + "MachineLoss"));
        // Get the Wheel Force Array Data For a Single Wheel
        m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
        // find the minimum value in the range provided
        DATAARRAYITR minItr = min_element(tempDataArray.begin()+start, tempDataArray.begin()+end);
        // Subtract out the machine loss to find the actual force
        float reductionValue = *minItr - machineLoss;
        if (true == useNewDrag)
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
        }
        // get the parameter
        float reductionMaxValue = GetParameterFloat(rollerName[roller]+"MaxESPReductionForce");

        if (reductionValue < reductionMaxValue)   // if value passed
        {
            testResult = testPass;
            resultBG = colorGreen;
        }
        else                 // value failed
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
                                    rollerName[roller] + "ESPReductionValue", result, "LBF",
                                    rollerName[roller] + "ESPReductionMaxForce", 
                                    CreateMessage(temp,sizeof(temp),"%7.2f",reductionMaxValue), "LBF");
        // Determine the status to return
        status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {
        status = BEP_STATUS_SKIP;
        Log(LOG_FN_ENTRY, "Skipping AnalyzeESPReductionForces -- End index is not greater than Start index!\n");
    }
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeESPReductionForces() done: %s\n", testResult.c_str());
    return(status);    
}


template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeTCSBuildForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    float buildMinValue = 0.0;

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeTCSBuildForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
        rollerName[roller].c_str(), start, end);

    if (end > start)               // if samples taken
    {
        // get the machine losses for this wheel
        float machineLoss = SystemReadFloat(GetDataTag(rollerName[roller] + "MachineLoss"));
        // Get the Wheel Force Array Data For a Single Wheel
        m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
        // find the maximum value in the range provided
        DATAARRAYITR maxItr = max_element(tempDataArray.begin()+start, tempDataArray.begin()+end);
        // Subtract out the machine loss to find the actual force
        float buildValue = *maxItr - machineLoss;

        if ((roller == 0) || (roller == 2))    // if LF or LR
        {
            buildMinValue = GetParameterFloat("LeftTCSMinBuildForce");
        }
        else buildMinValue = GetParameterFloat("RightTCSMinBuildForce");

        if (buildValue > buildMinValue)     // if value passed
        {
            testResult = testPass;
            resultBG = colorGreen;
        }
        else                 // value failed
        {
            testResult = testFail;
            resultBG = colorRed;
        }

        Log(LOG_DEV_DATA, "AnalyzeTCSBuildForces Build -- Limit: %.2f, Value: %.2f\n", buildMinValue, buildValue);
        char temp[256];
        result = CreateMessage(temp,sizeof(temp),"%7.2f",buildValue);

        // update the GUI
        SystemWrite(GetDataTag(rollerName[roller] + "TCSBuildValue"),result);        
        SystemWrite(GetDataTag(rollerName[roller] + "TCSBuildBGColor"), resultBG);

        // Update the fault data to be sent with the test result
        string faultTag = rollerName[roller]+"TCSBuildFail";
        string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
        string faultDesc = (testResult == testPass ? "ESP Build Test" : GetFaultDescription(faultTag));
        // send the test results to the TestResultServer
        SendSubtestResultWithDetail(rollerName[roller] + "TCSBuild", testResult, faultDesc, testResultCode,
                                    rollerName[roller] + "TCSBuildValue", result, "LBF",
                                    rollerName[roller] + "TCSBuildMinForce", 
                                    CreateMessage(temp,sizeof(temp),"%7.2f",buildMinValue), "LBF");
        // Determine the status to return
        status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {
        status = BEP_STATUS_SKIP;
        Log(LOG_FN_ENTRY, "Skipping AnalyzeTCSBuildForces -- End index is not greater than Start index!\n");
    }
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeTCSBuildForces() done: %s\n", testResult.c_str());
    return(status);    
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeTCSReductionForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    float reductionMaxValue = 0.0;

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeTCSReductionForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
        rollerName[roller].c_str(), start, end);

    if (end > start)               // if samples taken
    {
        // get the machine losses for this wheel
        float machineLoss = SystemReadFloat(GetDataTag(rollerName[roller] + "MachineLoss"));
        // Get the Wheel Force Array Data For a Single Wheel
        m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
        // find the minimum value in the range provided
        DATAARRAYITR minItr = min_element(tempDataArray.begin()+start, tempDataArray.begin()+end);
        // Subtract out the machine loss to find the actual force
        float reductionValue = *minItr - machineLoss;

        if ((roller == 0) || (roller == 2))    // if LF or LR
        {
            reductionMaxValue = GetParameterFloat("LeftTCSMaxReductionForce");
        }
        else reductionMaxValue = GetParameterFloat("RightTCSMaxReductionForce");

        if (reductionValue < reductionMaxValue)   // if value passed
        {
            testResult = testPass;
            resultBG = colorGreen;
        }
        else                 // value failed
        {
            testResult = testFail;
            resultBG = colorRed;
        }

        Log(LOG_DEV_DATA, "AnalyzeTCSReductionForces Reduction -- Limit: %.2f, Value: %.2f\n", reductionMaxValue, reductionValue);
        char temp[256];
        result = CreateMessage(temp,sizeof(temp),"%7.2f",reductionValue);

        // update the GUI
        SystemWrite(GetDataTag(rollerName[roller] + "TCSReductionValue"),result);        
        SystemWrite(GetDataTag(rollerName[roller] + "TCSReductionBGColor"), resultBG);

        // Update the fault data to be sent with the test result
        string faultTag = rollerName[roller]+"TCSReductionFail";
        string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
        string faultDesc = (testResult == testPass ? "ESP Reduction Test" : GetFaultDescription(faultTag));
        // send the test results to the TestResultServer
        SendSubtestResultWithDetail(rollerName[roller] + "TCSReduction", testResult, faultDesc, testResultCode,
                                    rollerName[roller] + "TCSReductionValue", result, "LBF",
                                    rollerName[roller] + "TCSReductionMaxForce", 
                                    CreateMessage(temp,sizeof(temp),"%7.2f",reductionMaxValue), "LBF");
        // Determine the status to return
        status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
    }
    else
    {
        status = BEP_STATUS_SKIP;
        Log(LOG_FN_ENTRY, "Skipping AnalyzeTCSReductionForces -- End index is not greater than Start index!\n");
    }
    // Log the exit and return the status
    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeTCSReductionForces() done: %s\n", testResult.c_str());
    return(status);    
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeTCSValveCross(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    bool valveCrossPass=0;        // overall status of the check pass/fail
    int ValveStart = m_TCSIndex[LFWHEEL].buildStart - m_TCSStartIndex;  
    int ValveEnd = m_TCSIndex[RFWHEEL].reductionEnd - m_TCSStartIndex;    
    float lfMax=0.0,rfMax=0.0;
    int   lfBuildPoint=0,rfBuildPoint=0;
    DATAARRAY lfForce,rfForce;

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeTCSValveCross()\n");

    // Read the data from the brake force array
    INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"), 
                                                                   m_TCSStartIndex,m_TCSEndIndex);
    m_baseBrakeTool->GetWheelForceArray(LFWHEEL,lfForce);
    m_baseBrakeTool->GetWheelForceArray(RFWHEEL,rfForce);

    if (dataStatus == BEP_STATUS_SUCCESS)
    {
        // search the whole valve firing range
        for (int ii=ValveStart;ii < ValveEnd;ii++)
        {
            Log(LOG_DEV_DATA,"Forces(%d) = %f %f\n",ii,lfForce[ii],rfForce[ii]);

            // if new left front maximum found
            if (lfForce[ii] >= lfMax)
            {
                lfMax = lfForce[ii]; // store new maximum value
                lfBuildPoint = ii; // store new maximum point
            }
            // if new right front maximum found
            if (rfForce[ii] >= rfMax)
            {
                rfMax = rfForce[ii]; // store new maxiumum value
                rfBuildPoint = ii; // store new maximum point
            }
        }

        Log(LOG_DEV_DATA,"Max points = %d %d\n",lfBuildPoint,rfBuildPoint);

        // if the minimum points are in the order the valves were fired
        if (lfBuildPoint < rfBuildPoint)
        {
            valveCrossPass=1;
        }
    }
    else
    {
        Log(LOG_DEV_DATA,"Failed to open wheel speed array\n");
    }

    if (valveCrossPass)            // if valve cross passed
    {
        Log(LOG_DEV_DATA, "TCS valve cross passed.\n");
        status = BEP_STATUS_SUCCESS;
        SendSubtestResult("TCSValveCross", testPass, "TCS Valve Cross");
    }
    else                    // else report fail
    {
        Log(LOG_DEV_DATA, "TCS valve crossed.\n");
        status = BEP_STATUS_FAILURE;
        SendSubtestResultWithDetail("TCSValveCross", testFail, "TCS Valve Cross",
                                    GetFaultCode("TCSFaultValvesCrossed"),
                                    GetFaultName("TCSFaultValvesCrossed"),
                                    GetFaultDescription("TCSFaultValvesCrossed"));
    }

    return(status);
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeESPValveCross(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    bool valveCrossPass=0;        // overall status of the check pass/fail
    int ValveStart = m_ESPIndex[LFWHEEL].buildStart - m_ESPStartIndex;  
    int ValveEnd = m_ESPIndex[RFWHEEL].reductionEnd - m_ESPStartIndex;    
    float lfMax=0.0,rfMax=0.0,lrMax=0.0,rrMax=0.0;
    int   lfBuildPoint=0,rfBuildPoint=0,lrBuildPoint=0,rrBuildPoint=0;
    DATAARRAY lfForce,rfForce,lrForce,rrForce;

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeESPValveCross()\n");

    // Read the data from the brake force array
    INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"), 
                                                                   m_ESPStartIndex,m_ESPEndIndex);
    m_baseBrakeTool->GetWheelForceArray(LFWHEEL,lfForce);
    m_baseBrakeTool->GetWheelForceArray(RFWHEEL,rfForce);
    m_baseBrakeTool->GetWheelForceArray(LRWHEEL,lrForce);
    m_baseBrakeTool->GetWheelForceArray(RRWHEEL,rrForce);


    if (dataStatus == BEP_STATUS_SUCCESS)
    {
        // search the whole valve firing range
        for (int ii=ValveStart;ii < ValveEnd;ii++)
        {
            Log(LOG_DEV_DATA,"Forces(%d) = %f %f %f %f\n",ii,
                lfForce[ii],rfForce[ii],lrForce[ii],rrForce[ii]);

            // if new left front maximum found
            if (lfForce[ii] >= lfMax)
            {
                lfMax = lfForce[ii]; // store new maximum value
                lfBuildPoint = ii; // store new maximum point
            }
            // if new right front maximum found
            if (rfForce[ii] >= rfMax)
            {
                rfMax = rfForce[ii]; // store new maxiumum value
                rfBuildPoint = ii; // store new maximum point
            }
            // if new left rear maximum found
            if (lrForce[ii] >= lrMax)
            {
                lrMax = lrForce[ii]; // store new maximum value
                lrBuildPoint = ii; // store new maximum point
            }
            // if new right rear maximum found
            if (rrForce[ii] >= rrMax)
            {
                rrMax = rrForce[ii]; // store new maxiumum value
                rrBuildPoint = ii; // store new maximum point
            }
        }

        Log(LOG_DEV_DATA,"Max points = %d %d %d %d\n",lfBuildPoint,
            rfBuildPoint,lrBuildPoint,rrBuildPoint);

        // if the minimum points are in the order the valves were fired
        if ((lfBuildPoint < rfBuildPoint)
// 2005.2.28 ews changed per emergency HMMA request
//                && (rfBuildPoint < lrBuildPoint) && 
//			   (lrBuildPoint < rrBuildPoint)
           )
        {
            valveCrossPass=1;
        }
    }
    else
    {
        Log(LOG_DEV_DATA,"Failed to open wheel speed array\n");
    }

    if (valveCrossPass)            // if valve cross passed
    {
        Log(LOG_DEV_DATA, "ESP valve cross passed.\n");
        status = BEP_STATUS_SUCCESS;
        SendSubtestResult("ESPValveCross", testPass, "ESP Valve Cross");
    }
    else                    // else report fail
    {
        Log(LOG_DEV_DATA, "ESP valve crossed.\n");
        status = BEP_STATUS_FAILURE;
        SendSubtestResultWithDetail("ESPValveCross", testFail, "ESP Valve Cross",
                                    GetFaultCode("ESPFaultValvesCrossed"),
                                    GetFaultName("ESPFaultValvesCrossed"),
                                    GetFaultDescription("ESPFaultValvesCrossed"));
    }

    return(status);
}

template <class VehicleModuleType>
BEP_STATUS_TYPE KoreaAbsTcTemplate<VehicleModuleType>::AnalyzeValveCross(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    bool valveCrossPass=0;        // overall status of the check pass/fail
    int ValveStart = m_reduxRecovIndex[LFWHEEL].reductionStart - m_absStartIndex;
    int ValveEnd = m_reduxRecovIndex[RRWHEEL].recoveryEnd - m_absStartIndex;
    float lfMin=9999.0,rfMin=9999.0,lrMin=9999.0,rrMin=9999.0;
    int   lfRedPoint=0,rfRedPoint=0,lrRedPoint=0,rrRedPoint=0;
    WHEELDATAARRAY wheelSpeedArray;
    DATAARRAY lfForce,rfForce,lrForce,rrForce;

    Log(LOG_FN_ENTRY, "KoreaAbsTcTemplate::AnalyzeValveCross()\n");

    m_baseBrakeTool->GetWheelForceArray(LFWHEEL,lfForce);
    m_baseBrakeTool->GetWheelForceArray(RFWHEEL,rfForce);
    m_baseBrakeTool->GetWheelForceArray(LRWHEEL,lrForce);
    m_baseBrakeTool->GetWheelForceArray(RRWHEEL,rrForce);

    // read in the data from the speed array	 
    INT32 testStatus = ReadDataArrays(GetParameter("IcmSpeedArray"), 
                                      m_absStartIndex, m_absEndIndex, 
                                      wheelSpeedArray);

    // if successful check for a valve cross
    if (testStatus == BEP_STATUS_SUCCESS)
    {
        // search the whole valve firing range
        for (int ii=ValveStart;ii < ValveEnd;ii++)
        {
            Log(LOG_DEV_DATA,"Forces(%d) = %f %f %f %f\n",ii,lfForce[ii],
                rfForce[ii],lrForce[ii],rrForce[ii]);

            // if new minimum found and wheel speed is greater than 10
            if ((lfForce[ii] <= lfMin) && (wheelSpeedArray[LFWHEEL][ii] > 10))
            {
                lfMin = lfForce[ii]; // store new miniumum value
                lfRedPoint = ii;   // store new minimum point
            }
            // if new minimum found and wheel speed is greater than 10
            if ((rfForce[ii] <= rfMin) && (wheelSpeedArray[RFWHEEL][ii] > 10))
            {
                rfMin = rfForce[ii]; // store new miniumum value
                rfRedPoint = ii;   // store new minimum point
            }
            // if new minimum found and wheel speed is greater than 10
            if ((lrForce[ii] <= lrMin) && (wheelSpeedArray[LRWHEEL][ii] > 10))
            {
                lrMin = lrForce[ii]; // store new miniumum value
                lrRedPoint = ii;   // store new minimum point
            }
            // if new minimum found and wheel speed is greater than 10
            if ((rrForce[ii] <= rrMin) && (wheelSpeedArray[RRWHEEL][ii] > 10))
            {
                rrMin = rrForce[ii]; // store new miniumum value
                rrRedPoint = ii;   // store new minimum point
            }
        }

        Log(LOG_DEV_DATA,"Min points = %d %d %d %d\n",lfRedPoint,rfRedPoint,lrRedPoint,rrRedPoint);

        // if the minimum points are in the order the valves were fired
        if ((lfRedPoint < rfRedPoint) && (rfRedPoint < lrRedPoint) && (lrRedPoint < rrRedPoint))
        {
            valveCrossPass=1;
        }
    }
    else
    {
        Log(LOG_DEV_DATA,"Failed to open wheel speed array\n");
    }

    // if valve cross passed
    if (valveCrossPass)
    {
        Log(LOG_DEV_DATA, "ABS valve cross passed.\n");
        status = BEP_STATUS_SUCCESS;
        SendSubtestResult("ValveCross", testPass, "Valve Cross");
    }
    // else report fail
    else
    {
        Log(LOG_DEV_DATA, "ABS valve crossed.\n");
        status = BEP_STATUS_FAILURE;
        SendSubtestResultWithDetail("ValveCross", testFail, "Valve Cross",
                                    GetFaultCode("ABSFaultValvesCrossed"),
                                    GetFaultName("ABSFaultValvesCrossed"),
                                    GetFaultDescription("ABSFaultValvesCrossed"));
    }

    return(status);
}
