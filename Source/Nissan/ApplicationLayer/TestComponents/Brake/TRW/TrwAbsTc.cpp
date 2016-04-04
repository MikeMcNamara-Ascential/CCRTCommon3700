//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the TRW ABS.
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
// 
//*************************************************************************
#include "TrwAbsTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TrwAbsTc<ModuleType>::TrwAbsTc() : GenericABSTCTemplate<ModuleType>()
{   
    m_ESPIndex = new BuildReductionIndex[2];   //LFRR and RFLR = 2
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TrwAbsTc<ModuleType>::~TrwAbsTc()
{   
    if(m_ESPIndex != NULL)
    {
        delete[] m_ESPIndex;
        m_ESPIndex = NULL;
    }
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void TrwAbsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericABSTCTemplate<ModuleType>::InitializeHook(config);
    // Default to a 4 channel ABS system
    SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
}


//-------------------------------------------------------------------------------------------------
template <class ModuleType>
const string TrwAbsTc<ModuleType>::CommandTestStep(const string &value)
{
    string status;             // status of the test step

    try
    {
        string step = GetTestStepName(); // get the test step we are on
        Log(LOG_DEV_DATA,"TrwAbsTc::CommandTestStep(%s): Entering test step %s\n",
            value.c_str(),step.c_str());

        if (StatusCheck() != BEP_STATUS_SUCCESS)      // make sure status is OK
        {
            UpdateResult(StatusCheck(),status);
            Log(LOG_ERRORS,"TrwAbsTc::CommandTestStep: StatusCheck() failed: %s\n",
                status.c_str());
            SendTestResult(status,GetTestStepInfo("Description"));
        }
        else if (step == "ESPValveFiringTest") status = ESPValveFiringTest();
        else if (step == "EvaluateESP") status = EvaluateESP();
        else
        {
            status = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
        }
    }
    catch (ModuleException &ex)
    {
        Log(LOG_ERRORS,"TrwAbsTc::CommandTestStep %s ModuleException %s\n",
            GetTestStepName().c_str(),ex.GetReason());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    catch (BepException &ex)
    {
        Log(LOG_ERRORS,"TrwAbsTc::CommandTestStep %s BepException %s\n",
            GetTestStepName().c_str(),ex.what());
        status = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_DEV_DATA,"TrwAbsTc::CommandTestStep(%s): Returning %s\n",
        value.c_str(),status.c_str());
    return(status);
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TrwAbsTc<ModuleType>::ESPValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter TrwAbsTc::ESPValveFiringTest()\n");
    try
    {
        UpdatePrompts();

        m_ESPStartIndex = TagArray("ESPStart");

        // run the individual wheel ESP tests
        testResult = LFRRESPTest();
        if(testResult == testPass) 
        {
            BposSleep(50);
            testResult == RFLRESPTest();
        }

        //BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("CycleEspValves"); //Command to fire ESP valves with a configurable delay before they return to normal.  
        //testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
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


//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TrwAbsTc<ModuleType>::SensorTest(void)
{   // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TrwAbsTc::SensorTest() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
    {   // Get the current drive axle so we can restore when completed
        string originalDriveAxle(SystemRead(DRIVE_AXLE_TAG));
        // Set the drive axle to Rear so fronts can be rotated
        SystemWrite(DRIVE_AXLE_TAG, string(REAR_WHEEL_DRIVE_VALUE));
        // Prompt the operator to shift to neutral and remove foot from brake pedal
        DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep(GetParameterInt("OperatorSetupDelay"));
        // Make sure speed set points are all zero
        m_MotorController.Write("LeftFrontSpeedValue", "0", false);
        m_MotorController.Write("RightFrontSpeedValue", "0", false);
        m_MotorController.Write("LeftRearSpeedValue", "0", false);
        m_MotorController.Write("RightRearSpeedValue", "0", false);
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        // Rotate each wheel and check to make sure the module believes it is the one spinning
        TestResultDetails details;
        testResult = testPass;
        for(UINT8 index = LFWHEEL; (index <= RRWHEEL) && !testResult.compare(testPass); index++)
        {
            testResult = RotateRoller(index, GetParameter("SensorQualitySpeed"), 
                                      GetParameterInt("SensorQualitySpeedDelay"), details);
        }
        // Reset machine to Boost mode and restore the drive axle
        m_MotorController.Write("LeftFrontSpeedValue", "0", false);
        m_MotorController.Write("RightFrontSpeedValue", "0", false);
        m_MotorController.Write("LeftRearSpeedValue", "0", false);
        m_MotorController.Write("RightRearSpeedValue", "0", false);
        m_MotorController.Write(MOTOR_MODE, BOOST_MODE, true);
        BposSleep(2000);
        SystemWrite(DRIVE_AXLE_TAG, originalDriveAxle);
        // Report the result
        SendTestResultWithDetail(testResult, details, GetTestStepInfo("Description"), "0000");
        // Remove the prompts
        RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
        RemovePrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Sensortest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TrwAbsTc::SensorTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TrwAbsTc<ModuleType>::RotateRoller(const UINT8 &rollerIndex, 
                                                const string &rollerSpeed, 
                                                const INT32 &rotationTime, 
                                                TestResultDetails &details)
{   
    string testResult = testFail;
    // Command the roller to speed
    Log(LOG_DEV_DATA, "Commanding %s roller to %s", rollerName[rollerIndex].c_str(), rollerSpeed.c_str());
    // Check if the drive axle should be changed
    if(LRWHEEL == rollerIndex)
    {   // Switch the drive axle so the last two rollers can spin
        m_MotorController.Write("LeftFrontSpeedValue", string("0"), false);
        m_MotorController.Write("RightFrontSpeedValue", string("0"), true);
        BposSleep(1000);
        SystemWrite(DRIVE_AXLE_TAG, string(FRONT_WHEEL_DRIVE_VALUE));
        BposSleep(1000);
    }
    // Command the roller to speed
    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", rollerSpeed, true);
    // Determine the min and max values using the tolerance required
    float minSpeed = (GetParameterFloat("SensorQualitySpeed") * 
                      (1.0 - (GetParameterFloat("SensorQualitySpeedTolerance") / 100.0)));
    float maxSpeed = (GetParameterFloat("SensorQualitySpeed") *
                      (1.0 + (GetParameterFloat("SensorQualitySpeedTolerance") / 100.0)));
    char buff[16];
    details.AddDetail(rollerName[rollerIndex]+"MinSpeedValue", CreateMessage(buff, sizeof(buff), "%.2f", minSpeed), unitsMPH);
    details.AddDetail(rollerName[rollerIndex]+"MaxSpeedValue", CreateMessage(buff, sizeof(buff), "%.2f", maxSpeed), unitsMPH);
    // Wait for the roller to achieve speed before moving on to the next roller
    BposSleep(rotationTime);
    // Read the sensor speeds from the module
    vector<float> sensorSpeeds;
    BEP_STATUS_TYPE status = m_vehicleModule.ReadModuleData("WheelSpeedSensors", sensorSpeeds);
    if(BEP_STATUS_SUCCESS == status)
    {   // Make sure the correct sensor is giving the reading
        if((minSpeed <= sensorSpeeds[rollerIndex]) && (sensorSpeeds[rollerIndex] <= maxSpeed))
        {   // Sensor is in range
            testResult = testPass;
        }
        else
        {   // Sensor is out of range
            testResult = testFail;
        }
        Log(LOG_DEV_DATA, "%s sensor result: %s - sensor: %.2f, [%.2f  %.2f]",
            rollerName[rollerIndex].c_str(), testResult.c_str(), sensorSpeeds[rollerIndex], minSpeed, maxSpeed);
        char buff[8];
        details.AddDetail(rollerName[rollerIndex]+"SensorSpeed", 
                           CreateMessage(buff, sizeof(buff), "%.2f", sensorSpeeds[rollerIndex]), unitsMPH);
    }
    else
    {
        Log(LOG_ERRORS, "Could not read sensor speeds from the module: %s", ConvertStatusToResponse(status).c_str());
        details.AddDetail(rollerName[rollerIndex]+"SensorResult", testFail, "");
        details.AddDetail(rollerName[rollerIndex]+"SensorSpeed", "0", unitsMPH);
    }
    // Command the current roller to stop
    m_MotorController.Write(rollerName[rollerIndex]+"SpeedValue", string("0"), true);
    return testResult;
}



//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TrwAbsTc<ModuleType>::LFRRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter TrwAbsTc::LFRRESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            //NOOP
            break;
        case ESP_PUMP_ON:
            // NOP
            break;
        case ESP_PRIMARY_ON:
            //NOOP
            break;
        case ESP_START:
            m_ESPIndex[0].buildStart = TagArray("LFRRESPBuildStart");
            espCommand = "LFRRESPTest";   //turn LF and RR ESP valve on for 200 ms
            break;
        case ESP_PRIMARY_OFF:
            BposSleep(200);
            m_ESPIndex[0].buildEnd = TagArray("LFRRESPBuildEnd");
            break;
        case ESP_END:
            BposSleep(50);
            m_ESPIndex[0].reductionStart = TagArray("LFRRESPReductionStart");
            espCommand = "LFRRESPEnd";   //turn LF and RR ESP valve off for 200 ms
            break;
        case ESP_FINALIZE:
            BposSleep(200);
            m_ESPIndex[0].reductionEnd = TagArray("LFRRESPReductionEnd");
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
            // Send the command to the module
            moduleStatus = m_vehicleModule.GetInfo(espCommand);

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

    Log(LOG_FN_ENTRY, "Exit TrwAbsTc::LFRRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}

template <class ModuleType>
string TrwAbsTc<ModuleType>::RFLRESPTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    std::string espCommand("");
    bool    done = false;
    int     step;

    Log(LOG_FN_ENTRY, "Enter TrwAbsTc::RFLRESPTest()\n");

    step = ESP_INIT;
    while(done == false)
    {
        switch(step)
        {
        case ESP_INIT:
            //NOOP
            break;
        case ESP_PUMP_ON:
            // NOP
            break;
        case ESP_PRIMARY_ON:
            //NOOP
            break;
        case ESP_START:
            m_ESPIndex[1].buildStart = TagArray("RFLRESPBuildStart");
            espCommand = "RFLRESPTest";   //turn LF and RR ESP valve on for 200 ms
            break;
        case ESP_PRIMARY_OFF:
            BposSleep(200);
            m_ESPIndex[1].buildEnd = TagArray("RFLRESPBuildEnd");
            break;
        case ESP_END:
            BposSleep(50);
            m_ESPIndex[1].reductionStart = TagArray("RFLRESPReductionStart");
            espCommand = "RFLRESPEnd";   //turn LF and RR ESP valve off for 200 ms
            break;
        case ESP_FINALIZE:
            BposSleep(200);
            m_ESPIndex[1].reductionEnd = TagArray("RFLRESPReductionEnd");
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
            // Send the command to the module
            moduleStatus = m_vehicleModule.GetInfo(espCommand);

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

    Log(LOG_FN_ENTRY, "Exit TrwAbsTc::RFLRESPTest() - status: %s\n", testResult.c_str());
    return(testResult);
}


//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TrwAbsTc<ModuleType>::EvaluateESP(void)
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
    Log(LOG_FN_ENTRY, "TrwAbsTc::EvaluateESP - Enter\n");

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
                // changed to 2 to evaluate LFRR and RFLR
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

    Log(LOG_FN_ENTRY, "TrwAbsTc::EvaluateESP - Exit %s\n",testResult.c_str());
    return(testResult);
}


template <class ModuleType>
BEP_STATUS_TYPE TrwAbsTc<ModuleType>::AnalyzeESPBuildForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
    bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

    Log(LOG_FN_ENTRY, "TrwAbsTc::AnalyzeESPBuildForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
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
    Log(LOG_FN_ENTRY, "TrwAbsTc::AnalyzeESPBuildForces() done: %s\n", testResult.c_str());
    return(status);    
}

template <class ModuleType>
BEP_STATUS_TYPE TrwAbsTc<ModuleType>::AnalyzeESPReductionForces(INT32 roller, INT32 start, INT32 end)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;     // the overall status of the analysis
    string testResult = testFail;  // the test status for the check
    string result = "999";        // the result of the individual wheel for display
    string resultBG = "Blue";     // the status (background color) of the individual wheel for display
    DATAARRAY tempDataArray;      // Used to hold the Force values from a single wheel
    bool    includeDrag = GetParameterBool("IncludeDragInBrakeForce");
// 2005.3.3 ews added this section to use recalculated drag for ESP evaluation
    bool useNewDrag = GetParameterBool("RecalculateDragBeforeESP");

    Log(LOG_FN_ENTRY, "TrwAbsTc::AnalyzeESPReductionForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
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
    Log(LOG_FN_ENTRY, "TrwAbsTc::AnalyzeESPReductionForces() done: %s\n", testResult.c_str());
    return(status);    
}

