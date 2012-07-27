//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Teves MK70 ABS.
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
#include "TevesMk70AbsTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TevesMk70AbsTc<ModuleType>::TevesMk70AbsTc() : GenericABSTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
TevesMk70AbsTc<ModuleType>::~TevesMk70AbsTc()
{   // Nothing special to do here
}
template <class ModuleType>
const string TevesMk70AbsTc<ModuleType>::TevesMk70AbsTc<ModuleType>::CommandTestStep(const string &value)
{
    string status;
    try
    {
        string step = GetTestStepName(); // Get the name of the test step
        Log(LOG_DEV_DATA, "TevesMk70AbsTc::CommandTestStep(%s): Entering test step %s\n", 
            value.c_str(),step.c_str());

        // make sure it is OK to test
        if (StatusCheck() != BEP_STATUS_SUCCESS)
        {
            status = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS,"TevesMk70AbsTc::CommandTestStep: StatusCheck() failed: %s\n",
                status.c_str());
            SendTestResult(status,GetTestStepInfo("Description"));
        } 
        else if (step == "InitWinCcrtInterface")    status = InitWinCcrtInterface();
        else if (step == "CloseWinCcrtInterface")   status = CloseWinCcrtInterface();
        else if (step == "StartModuleKeepAlive")    status = StartModuleKeepAlive();
        else if (step == "StopModuleKeepAlive")     status = StopModuleKeepAlive();
        else if (step == "RequestCoding")           status = RequestCoding();
        else if (step == "SendCoding")              status = SendCoding();
        else if (step == "StartEolSession")         status = StartEolSession();
        else if (step == "ReadEcuId")               status = ReadEcuId();
        else if (step == "IsCableConnected")               status = IsCableConnected();

        // Call the base class to handle the test step
        else status = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
    } catch (BepException &err)
    {
        Log("BoschABS::CommandTestStep %s BepException: %s\n",
            GetTestStepName().c_str(),err.what());
        status = BEP_ERROR_RESPONSE;
    }Log(LOG_DEV_DATA,"Bosch8TC::CommandTestStep(%s) returning %s\n",
         value.c_str(),status.c_str());

    return(status);
}

template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::InitWinCcrtInterface(void)
{  // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    const string testDescription( GetTestStepInfo("Description"));
    string testResultCode( "0000");
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::InitWinCcrtInterface() - Enter\n");
    UINT16 brakeInitStatus = 0;

    
    string tevesInitType = "";
    string tevesBrakeName = "";
    tevesInitType = GetParameter("BrakeInitType");
    tevesBrakeName = GetParameter("SetAbsType");
    
    Log(LOG_DEV_DATA, "InitBrakeTc - Using BrakeInitType: %s  SetAbsType: %s\n", tevesInitType.c_str(), tevesBrakeName.c_str());
    
    //how do you send a command and not look for a response? Do that here.
    // SetAbsType:TevesMk70. Need to verify there is brake module named TevesMk70 
    // in WinCcrt windows app.
    // options are: FiveBaud, Fast
    status = m_vehicleModule.ReadModuleData(tevesBrakeName, brakeInitStatus);

    // check to see if the brake was set correctly
    if (BEP_STATUS_SUCCESS == status)
    {
        //status = m_vehicleModule.ReadModuleData("WinCcrtBrakeStatus", brakeInitStatus);
        Log(LOG_DEV_DATA, "InitBrakeTc - BrakeInitStatus(a): %d\n", brakeInitStatus);
        BposSleep(500);
        if (BEP_STATUS_SUCCESS == status && brakeInitStatus == 0)
        { // brakes were set up. now send command to initialize the modules.
            status = m_vehicleModule.ReadModuleData(tevesInitType, brakeInitStatus);
            BposSleep(500); 
            if(BEP_STATUS_SUCCESS == status)
            { // check to see if the module was intialized
              //status = m_vehicleModule.ReadModuleData("WinCcrtConnectStatus", brakeInitStatus);

              Log(LOG_DEV_DATA, "InitBrakeTc - BrakeInitStatus(b): %d  status(needs: %d)  %d\n", brakeInitStatus, BEP_STATUS_SUCCESS, status);
              //BposSleep(500);
              if (BEP_STATUS_SUCCESS == status && brakeInitStatus == 0)
              {
                  Log(LOG_DEV_DATA, "InitBrakeTc - A good connection was made with the %s module.\n", tevesBrakeName.c_str());
                  testResult = testPass;
                  
              }
              else
              {
                  Log(LOG_DEV_DATA, "InitBrakeTc - Couldn't connect to the module");
                  testResult = testFail;
              }
            }
            else
            {
                testResult = testFail;
                Log(LOG_DEV_DATA, "InitBrakeTc - the attempt to have WinCcrt set up module failed.");
            }
        }
        else
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "InitBrakeTc - WinCcrt reported failed brake test.");
        }
    }
    else
    {
        testResult = testFail;
        Log(LOG_DEV_DATA, "InitBrakeTc - Failed to send %s to WinCcrt", tevesInitType.c_str());
    }    
    
    SendTestResult(testResult, testDescription, testResultCode);
    
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::InitWinCcrtInterface() - Exit, status=%s\n", testResult.c_str());
    
    return(testResult);
}

template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::StartModuleKeepAlive(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult = BEP_TESTING_STATUS;
    
    status = m_vehicleModule.CommandModule("StartModuleKeepAlive");
    BposSleep(500);
    testResult = (status == BEP_STATUS_SUCCESS) ? testPass : testFail;
    
    return testResult;
}

template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::StopModuleKeepAlive(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string testResult = BEP_TESTING_STATUS;
    
    status = m_vehicleModule.CommandModule("StopModuleKeepAlive");
    BposSleep(500);
    testResult = (status == BEP_STATUS_SUCCESS) ? testPass : testFail;
    
    return testResult;
}
//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::StartEolSession(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter TevesMk70AbsTc::StartEolSession()\n");
    if(!ShortCircuitTestStep())
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            if (GetCoding2Auth())
            {
                try
                {   // Try to enter diagnostic mode
                    moduleStatus = m_vehicleModule.CommandModule("StartEolSession");
                    BposSleep(500);
                    // Determine the test result
                    if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                    testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                    testDescription = 
                        (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                    // Log the data
                    Log(LOG_DEV_DATA, "StartEolSession() - %s - status: %s\n",
                        testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                }
                catch(ModuleException &moduleException)
                {
                    Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                        GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                    testResult = testSoftwareFail;
                    testResultCode = GetFaultCode("SoftwareFailure");
                    testDescription = GetFaultDescription("SoftwareFailure");
                }
            }
            else
            {
                Log(LOG_ERRORS, "TevesMk70AbsTc::StartEolSession() - Vehicle did not gain coding 2 authorization.");
                testResult = testFail;
            }
        }
        else
        {
            Log(LOG_ERRORS, "TevesMk70AbsTc::StartEolSession() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "StartEolSession() - Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit TevesMk70AbsTc::StartEolSession()\n");
    return(testResult);
}


//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::RequestCoding(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter TevesMk70AbsTc::RequestCoding()\n");
    if(!ShortCircuitTestStep())
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.CommandModule("RequestCoding");
                BposSleep(500);
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "RequestCoding() - %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "TevesMk70AbsTc::RequestCoding() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "RequestCoding() - Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit TevesMk70AbsTc::RequestCoding()\n");
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::SendCoding(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter TevesMk70AbsTc::SendCoding()\n");
    if(!ShortCircuitTestStep())
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.CommandModule("SendCoding");
                BposSleep(500);
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "SendCoding() - %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                SetCoding2Auth(true);
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "TevesMk70AbsTc::SendCoding() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "SendCoding() - Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit TevesMk70AbsTc::SendCoding()\n");
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::CloseWinCcrtInterface(void)
{   // Log the entry close down the WinCcrtInterface
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter TevesMk70AbsTc::CloseWinCcrtInterface()\n");
    if(!ShortCircuitTestStep())
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.CommandModule("WinCcrtDisconnect");
                BposSleep(500);
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "CloseWinCcrtInterface() - %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
                SetCoding2Auth(true);
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "TevesMk70AbsTc::CloseWinCcrtInterface() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "CloseWinCcrtInterface() - Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit TevesMk70AbsTc::CloseWinCcrtInterface()\n");
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void TevesMk70AbsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::InitializeHook() - Enter");
    SetCoding2Auth(false);
    
    GenericABSTCTemplate<ModuleType>::InitializeHook(config);
    // Default to a 4 channel ABS system
    SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
    
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::InitializeHook() - Exit");
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string TevesMk70AbsTc<ModuleType>::ReadEcuId(void)
{   // Set up some variables
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string EcuId;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step should be skipped
    Log(LOG_FN_ENTRY, "Enter TevesMk70AbsTc::ReadEcuId()\n");
    if(!ShortCircuitTestStep())
    {   // Need to perform this test step
        if(CheckCableConnect())
        {
            try
            {   // Try to enter diagnostic mode
                moduleStatus = m_vehicleModule.ReadModuleData("ReadEcuId", EcuId);
                BposSleep(500);
                Log(LOG_DEV_DATA, "ReadEcuId() - read EcuId: %s\n", EcuId.c_str());
                
                // Determine the test result
                if(moduleStatus != BEP_STATUS_SUCCESS) SetCommunicationFailure(true);
                testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
                testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
                testDescription = 
                    (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
                // Log the data
                Log(LOG_DEV_DATA, "ReadEcuId() - %s - status: %s\n",
                    testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
            }
            catch(ModuleException &moduleException)
            {
                Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
                    GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
                testResult = testSoftwareFail;
                testResultCode = GetFaultCode("SoftwareFailure");
                testDescription = GetFaultDescription("SoftwareFailure");
            }
        }
        else
        {
            Log(LOG_ERRORS, "TevesMk70AbsTc::ReadEcuId() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }
        // Send the test result
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_DEV_DATA, "ReadEcuId() - Skipping test step %s\n", GetTestStepName().c_str());
    }
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit TevesMk70AbsTc::ReadEcuId()\n");
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TevesMk70AbsTc<ModuleType>::SensorTest(void)
{   // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::SensorTest() - Enter");
    string testResult(BEP_TESTING_RESPONSE);
    if (!ShortCircuitTestStep() && GetTestStepResult().compare(testPass))
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
        for (UINT8 index = LFWHEEL; (index <= RRWHEEL) && !testResult.compare(testPass); index++)
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
    } else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping Sensortest");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::SensorTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TevesMk70AbsTc<ModuleType>::RotateRoller(const UINT8 &rollerIndex, 
                                                const string &rollerSpeed, 
                                                const INT32 &rotationTime, 
                                                TestResultDetails &details)
{   
    string testResult = testFail;
    // Command the roller to speed
    Log(LOG_DEV_DATA, "Commanding %s roller to %s", rollerName[rollerIndex].c_str(), rollerSpeed.c_str());
    // Check if the drive axle should be changed
    if (LRWHEEL == rollerIndex)
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
    if (BEP_STATUS_SUCCESS == status)
    {   // Make sure the correct sensor is giving the reading
        if ((minSpeed <= sensorSpeeds[rollerIndex]) && (sensorSpeeds[rollerIndex] <= maxSpeed))
        {   // Sensor is in range
            testResult = testPass;
        } else
        {   // Sensor is out of range
            testResult = testFail;
        }
        Log(LOG_DEV_DATA, "%s sensor result: %s - sensor: %.2f, [%.2f  %.2f]",
            rollerName[rollerIndex].c_str(), testResult.c_str(), sensorSpeeds[rollerIndex], minSpeed, maxSpeed);
        char buff[8];
        details.AddDetail(rollerName[rollerIndex]+"SensorSpeed", 
                          CreateMessage(buff, sizeof(buff), "%.2f", sensorSpeeds[rollerIndex]), unitsMPH);
    } else
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
template<class ModuleType>
void TevesMk70AbsTc<ModuleType>::SetCoding2Auth(bool val)
{
    m_HaveCoding2Auth = val;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
bool TevesMk70AbsTc<ModuleType>::GetCoding2Auth()
{
    return m_HaveCoding2Auth;
}

template<class ModuleType>
const bool TevesMk70AbsTc<ModuleType>::IsCableConnected(void)
{
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::IsCableConnected() - Enter");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    UINT16 isCableConnected = 0;
    bool rVal = false;

    status = m_vehicleModule.ReadModuleData("IsCableConnected", isCableConnected);
    Log(LOG_DEV_DATA, "isCableConnected: %d  status: %d", isCableConnected, status);
    if (status == BEP_STATUS_SUCCESS)
    {
        Log(LOG_DEV_DATA, "Is the cable connected: %s", (isCableConnected ? "Yes" : "No"));
        rVal = true;
    }
    else
    {
        Log(LOG_DEV_DATA, "Error getting IsCableConnected");
    }

    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::IsCableConnected() - Exit");

    return rVal;
}

