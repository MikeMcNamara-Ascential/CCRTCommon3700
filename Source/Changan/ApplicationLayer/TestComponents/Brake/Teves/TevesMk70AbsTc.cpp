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

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
void TevesMk70AbsTc<ModuleType>::InitializeHook(const XmlNode *config)
{   // Call the base class to begin the initialization
    GenericABSTCTemplate<ModuleType>::InitializeHook(config);
    // Default to a 4 channel ABS system
    SetFourChannelSystem(GetParameterBool("AbsFourChannelSystem"));
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const string Mk70AbsTc<VehicleModuleType>::CommandTestStep(const string &value)
{
    string status;

    try
    {
        string step = GetTestStepName();
        Log(LOG_DEV_DATA, "CommandTestStep(%s): Entering test step %s\n", value.c_str(), step.c_str());

        if (StatusCheck() != BEP_STATUS_SUCCESS)
        {
            status = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS,  "CommandTestStep: StatusCheck() failed: %s\n", status.c_str());
            SendTestResult(status, GetTestStepInfo("Description"));
        }
        else if (step == "PerformModuleLinkup")     status = PerformModuleLinkup();
        else if (step == "BeginEolMode")            status = TestStepBeginEolMode(value);
        else if (step == "EolCoding")               status = TestStepEolCoding(value);
        else if (step == "EnterEolMode")            status = TestStepEnterEolMode(value);
        else if (step == "ApplyTorqueToAllRollers") status = TestStepApplyTorqueToAllRollers(value);
        else if (step == "LowSpeedValveFiringTest") status = TestStepLowSpeedValveFiringTest(value);
        else if (step == "TorqueZero")              status = TestStepTorqueZero(value);
        else if (step == "EngageMachine")              status = EngageMachine();
        else  status = GenericABSTCTemplate<VehicleModuleType>::CommandTestStep(value);
    }
    catch (BepException &err)
    {
        Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
        status = BEP_ERROR_RESPONSE;
    }
    Log(LOG_DEV_DATA, "CommandTestStep(%s) returning %s\n", value.c_str(), status.c_str());

    return(status);
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
string Mk70AbsTc<VehicleModuleType>::PerformModuleLinkup(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    int newPriority=0,oldPriority=0;

    Log(LOG_FN_ENTRY, "Enter Mk70AbsTc::PerformModuleLinkup()\n");

    if (!ShortCircuitTestStep())
    {
        // make sure the cable is connected
        //if(CheckCableConnect())
        //{
        try
        {
            // Read in the updated priority
            newPriority = GetTestStepInfoInt("TestStepPriority");
            Log(LOG_DEV_DATA,"PerformModuleLinkup priority = %d\n",newPriority);

            // if a priority was specified, change to the new priority
            if (newPriority != 0) oldPriority = BposSetPriority(0,newPriority);

            int retry = 3;
            while (retry-- && moduleStatus != BEP_STATUS_SUCCESS && StatusCheck() == BEP_STATUS_SUCCESS)
            {
                moduleStatus = m_vehicleModule.PerformModuleLinkup();

                if (moduleStatus != BEP_STATUS_SUCCESS)
                {
                    //prompt driver to restart

                    DisplayTimedPrompt("KeyOff", "1", 0, 10000);
                    DisplayTimedPrompt("StartVehicle", "1", 0, 6000);
                    moduleStatus = m_vehicleModule.PerformModuleLinkup();
                }
            }

            // since this will fail if we have already linked up, this should pass if there are no exceptions
            //^ not true
            Log(LOG_ERRORS, "Module linkup status: %s", ConvertStatusToResponse(moduleStatus).c_str());
            testResult = (BEP_STATUS_SUCCESS == moduleStatus) ? testPass : testFail;
            testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("ModuleLinkupFailure");
            testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("ModuleLinkupFailure");
            Log(LOG_DEV_DATA, "Exit PerformModuleLinkup: %s - status: %s\n",testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
        }
        catch (ModuleException &moduleException)
        {
            Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n", 
                GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }

        // if a priority was specified and the priority was changed, return to the old priority
        if ((newPriority != 0) && (oldPriority != -1)) BposSetPriority(0,oldPriority);
        /*}
        else
        {
            Log(LOG_ERRORS, "Mk70AbsTc::PerformModuleLinkup() - Timeout waiting for Cable Connect");
            testResult = testTimeout;
            testResultCode = GetFaultCode("TimeoutFailure");
            testDescription = GetFaultDescription("TimeoutFailure");
        }*/

        SetFourChannelSystem(true);
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    Log(LOG_FN_ENTRY, "Exit Mk70AbsTc::PerformModuleLinkup()\n");
    return(testResult);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string TevesMk70AbsTc<ModuleType>::SensorTest(void)
{   // Log the entry and determine if this test should be performed
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::SensorTest() - Enter");
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
    Log(LOG_FN_ENTRY, "TevesMk70AbsTc::SensorTest() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const std::string Mk70AbsTc<VehicleModuleType>::TestStepBeginEolMode(const std::string &value)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Mk70AbsTc::TestStepBeginEolMode()\n");

    if (!ShortCircuitTestStep())
    {
        try
        {
            moduleStatus = m_vehicleModule.BeginEolMode();
            if (moduleStatus == BEP_STATUS_SUCCESS)
            {
                testResult = testPass;
            }
            else
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
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

        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    Log(LOG_FN_ENTRY, "Exit Mk70AbsTc::TestStepBeginEolMode()\n");

    return(testResult);
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const std::string Mk70AbsTc<VehicleModuleType>::TestStepEolCoding(const std::string &value)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Mk70AbsTc::TestStepEolCoding()\n");

    if (!ShortCircuitTestStep())
    {
        try
        {
            moduleStatus = m_vehicleModule.EolCoding();
            if (moduleStatus == BEP_STATUS_SUCCESS)
            {
                testResult = testPass;
            }
            else
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
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

        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    Log(LOG_FN_ENTRY, "Exit Mk70AbsTc::TestStepEolCoding()\n");

    return(testResult);
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const std::string Mk70AbsTc<VehicleModuleType>::TestStepEnterEolMode(const std::string &value)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter Mk70AbsTc::TestStepEnterEolMode()\n");

    if (!ShortCircuitTestStep())
    {
        try
        {
            moduleStatus = m_vehicleModule.EnterEolMode();
            if (moduleStatus == BEP_STATUS_SUCCESS)
            {
                testResult = testPass;
            }
            else
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
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

        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {
        testResult = testSkip;
        Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
    }

    Log(LOG_FN_ENTRY, "Exit Mk70AbsTc::TestStepEnterEolMode()\n");

    return(testResult);
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const std::string Mk70AbsTc<VehicleModuleType>::TestStepApplyTorqueToAllRollers(const std::string &value)
{
    return ApplyTorqueToRollers(GetParameter("LowSpeedAbs_LeftFrontTorque"), 
                                GetParameter("LowSpeedAbs_RightFrontTorque"), 
                                GetParameter("LowSpeedAbs_LeftRearTorque"), 
                                GetParameter("LowSpeedAbs_RightRearTorque"));
}

//-------------------------------------------------------------------------
template <class VehicleModuleType>
const std::string Mk70AbsTc<VehicleModuleType>::ApplyTorqueToRollers(string lfTorque, string rfTorque, string lrTorque, string rrTorque)
{
    // Required test step variables and their correct initialization
    string testResult(testFail);
    string testDescription(GetTestStepInfo("Description"));
    string testResultCode("0000");

    Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    if (ShortCircuitTestStep())
    {
        // Skip test because of previous pass
        Log(LOG_DEV_DATA,"%s:%s skipped\n", GetComponentName().c_str(), GetTestStepName().c_str());
        testResult = testSkip;
    }
    else
    {
        DisplayPrompt(GetPromptBox("TestingAbs"),GetPrompt("TestingAbs"),GetPromptPriority("TestingAbs"));
        DisplayPrompt(GetPromptBox("ShiftToNeutralMaintainBrake"),GetPrompt("ShiftToNeutralMaintainBrake"),
                      GetPromptPriority("ShiftToNeutralMaintainBrake"));
        BposSleep(GetParameterInt("LowSpeedAbs_PromptDelay"));
        try
        {
            if (CheckZeroSpeed() == true)
            {
                testResult = EngageMachine();
                if (testResult == testPass)
                {
                    Log(LOG_DEV_DATA, "Applying torque to all rollers. LF Torque = %s, RF Torque = %s, LR Torque = %s, RR Torque = %s\n", 
                        lfTorque.c_str(), rfTorque.c_str(), lrTorque.c_str(), rrTorque.c_str());
                    SetMotorTorque(lfTorque, rfTorque, lrTorque, rrTorque);

                    // Safety check to verify vehicle is present and providing enough force to resist the torque
                    if (!IsRunawayPresent(GetParameterInt("LowSpeedAbs_SafetyDelay"), GetParameterInt("LowSpeedAbs_SafetyMaxSpeed")))
                    {
                        Log(LOG_DEV_DATA, "Torque command successful\n");
                        testResult = testPass;
                    }
                    else
                    {
                        // Possibe "runaway", set rolls to torque 0 mode
                        Log(LOG_ERRORS, "Possible \"runaway\", setting torque to 0\n");
                        SetMotorTorque("0.0", "0.0", "0.0", "0.0");
                        testResult = testFail;
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "%s.%s - Failed to Engage machine\n",
                        GetComponentName().c_str(), GetTestStepName().c_str());
                    testResult = testSoftwareFail;
                    testDescription = GetFaultDescription("SoftwareFailure");
                    testResultCode = GetFaultCode("SoftwareFailure");
                }
            }
            else
            {
                testResult = testTimeout;
                testDescription = GetFaultDescription("ZeroSpeedFailure");
                testResultCode = GetFaultCode("ZeroSpeedFailure");
            }
        }
        catch (ModuleException &err)
        {
            // Log the exception and return the error
            Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
            testResult = testSoftwareFail;
            testDescription = GetFaultDescription("SoftwareFailure");
            testResultCode = GetFaultCode("SoftwareFailure");
        }

        // Send test results to server
        SendTestResult(testResult, testDescription, testResultCode);
    }

    Log(LOG_FN_ENTRY, "%s:%s - Exit, %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
    return(testResult);
}


//To do 
// Mark abs start reduction / recovery for each wheel
// Check speed - start = 0  reduction at speed recover = 0
//-------------------------------------------------------------------------
template <class VehicleModuleType>
const string Mk70AbsTc<VehicleModuleType>::TestStepLowSpeedValveFiringTest(const string &value)
{
    string testResult = testPass;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    string absStatus = "";
    string absSequenceCode = "", currentABSSequence = "";
    string wheelSpeedResult = testPass;
    XmlNodeMapItr iter;
    //Flag indicating wheel speed must be checked
    bool initialState = true;
    int scanDelay = GetTestStepInfoInt("ScanDelay");
    m_vehicleModule.CommandModule("StartActuatorTest");   
    m_absStartIndex = TagArray("ABSStart");
    Log(LOG_FN_ENTRY, "Enter Mk70AbsTc::TestStepLowSpeedValveFiringTest()\n");
    XmlNodeMap *valveFiringSequence = &m_abs4ChannelFiringOrder;
    if (CheckZeroSpeed() == true)
    {
        while (status == BEP_STATUS_SUCCESS && BEP_STATUS_SUCCESS == StatusCheck() && TimeRemaining() &&
               absStatus != "TesterSwitchFurther" && absStatus != "FunctionTerminatedByController" &&
               wheelSpeedResult == testPass)
        {
            m_vehicleModule.ReadModuleData("AbsTestMsg", absStatus);
            Log(LOG_DEV_DATA, "AbsTestMsg: %s\n", absStatus.c_str());
            m_vehicleModule.CommandModule("ContinueAbsTestMsg");
            BposSleep(scanDelay);
        }
        absStatus = "";

        if (CheckZeroSpeed() == true)
        {
            //for each wheel
            for (iter = valveFiringSequence->begin(); iter != valveFiringSequence->end() && 
                (absStatus != "FunctionTerminatedByController") && (wheelSpeedResult == testPass); iter++)
            {
                while (status == BEP_STATUS_SUCCESS && BEP_STATUS_SUCCESS == StatusCheck() && TimeRemaining() &&
                       absStatus != "TesterSwitchFurther" && absStatus != "FunctionTerminatedByController"
                       && (wheelSpeedResult == testPass))
                {
                    m_vehicleModule.ReadModuleData("AbsTestMsg", absStatus);
                    Log(LOG_DEV_DATA, "AbsTestMsg: %s\n", absStatus.c_str());
                    if (absStatus == "ECUAutoPilot")
                    {
                        m_vehicleModule.ReadModuleData("ReadAbsSequenceCode", absSequenceCode);
                        Log(LOG_DEV_DATA, "ReadAbsSequenceCode: %s\n", absSequenceCode.c_str());
                        //Note: Read gives you the next sequence
                        if (absSequenceCode != currentABSSequence)
                        {//new step - handle sequence
                            if (currentABSSequence == "None")
                            {
                                if (!initialState)
                                {
                                    TagArray(iter->second->getValue() + "RecovStart");
                                    Log(LOG_FN_ENTRY, "Tagging RecovStart\n");
                                }
                                else
                                {
                                    delay(GetParameterInt("InletValveDelay"));
                                }
                                if (!initialState && wheelSpeedResult == testPass) 
                                {
                                    delay(GetParameterInt("RecoveryDelay"));
                                    TagArray(iter->second->getValue() + "RecovStop");
                                    Log(LOG_FN_ENTRY, "Tagging RecovStop\n");
                                }
                                wheelSpeedResult = VerifyWheelStopped(iter->second->getValue());
                                initialState =false;
                            }
                            else if (currentABSSequence == "InletValve1")
                            {
                                delay(GetParameterInt("InletValveDelay"));
                                wheelSpeedResult = VerifyWheelStopped(iter->second->getValue());
                            }
                            else if (currentABSSequence == "InletValve2" )
                            {
                                CheckSensorCross(iter->second->getValue());
                                delay(GetParameterInt("InletValveDelay"));
                                wheelSpeedResult = VerifyWheelSpeed(iter->second->getValue());
                            }
                            else if (currentABSSequence == "InletOutletPump")
                            {
                                TagArray(iter->second->getValue() + "ReduxStart");
                                Log(LOG_FN_ENTRY, "Tagging ReduxStart\n");
                                delay(GetParameterInt("ReductionDelay"));
                                wheelSpeedResult = VerifyWheelSpeed(iter->second->getValue());
                                TagArray(iter->second->getValue() + "ReduxStop");
                                Log(LOG_FN_ENTRY, "Tagging ReduxStop\n");
                            }
                            else if (currentABSSequence == "WheelComplete" && !initialState)
                            {//move on to next wheel
                                absStatus = "TesterSwitchFurther";
                                initialState = true;
                            }
                            else 
                            {//intermediate step - do nothing
                                Log(LOG_DEV_DATA, "Intermediate step\n");
                            }
                        }
                        currentABSSequence = absSequenceCode;
                    }
                    m_vehicleModule.CommandModule("ContinueAbsTestMsg");
                    BposSleep(scanDelay);
                }
                if (absStatus != "FunctionTerminatedByController")
                {
                    absStatus = "";
                }
            }
            m_absEndIndex = TagArray("ABSEnd");
            if (!TimeRemaining())
            {//checktimeout
                testResult = testTimeout;
                testResultCode = GetFaultCode("TimeoutFailure");
                testDescription = GetFaultDescription("TimeoutFailure");
            }
            else if ((wheelSpeedResult != testPass))
            {//wheel speed failure
                if (absSequenceCode == "None")
                {
                    if (!initialState) absSequenceCode = iter->second->getValue() + "Recovery";
                    else absSequenceCode = iter->second->getValue() + "InitialZeroSpeed";
                }
                else if (absSequenceCode == "InletValve2")
                {
                    absSequenceCode = iter->second->getValue() + "Reduction";
                }
                else if(!initialState)
                {      
                    absSequenceCode = iter->second->getValue() + "Recovery";
                }
                testResult = testFail;
                testResultCode = GetFaultCode("WheelSpeedFailure" + absSequenceCode);
                testDescription = GetFaultDescription("WheelSpeedFailure" + absSequenceCode);
            }
            else if (absStatus == "FunctionTerminatedByController" && iter != valveFiringSequence->end())
            {//Ended by ecu abnormally
                testResult = testFail;
                testResultCode = GetFaultCode("ECUTerminatedAbnormally");
                testDescription = GetFaultDescription("ECUTerminatedAbnormally");
            }
            else
            {//if status good pass
                testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
                testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("CommunicationFailure");
                testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("CommunicationFailure");
            }
        }
    }
    SendTestResult(testResult, testDescription, testResultCode);
    Log(LOG_DEV_DATA, "CommandTestStep(TestStepLowSpeedValveFiringTest()) returning %s\n", testResult.c_str());
    Log(LOG_FN_ENTRY,"Exit TestStepLowSpeedValveFiringTest()");
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
