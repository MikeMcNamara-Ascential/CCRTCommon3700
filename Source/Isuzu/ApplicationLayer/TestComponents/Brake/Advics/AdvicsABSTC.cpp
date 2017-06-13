//*************************************************************************
// FILE:
//    $Header$
//
// FILE DESCRIPTION:
//      Advics ABS test component.
//
//===========================================================================
// Copyright (c) 2008- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log$
//
//*************************************************************************
#include "AdvicsABSTC.h"

#define CHECK_CABLE_CONNECT 1


//-----------------------------------------------------------------------------
template <class ModuleType>
AdvicsABSTC<ModuleType>::AdvicsABSTC() : GenericABSTCTemplate<ModuleType>()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template <class ModuleType>
AdvicsABSTC<ModuleType>::~AdvicsABSTC()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::CommandTestStep(const string &value)
{
    string result = BEP_TESTING_RESPONSE;
    string testStep(GetTestStepName());
    Log(LOG_FN_ENTRY, "AdvicsABSTC::CommandTestStep(%s) - Entering test step: %s", value.c_str(), testStep.c_str());
    try
    {   // Ensure system status will allow a test step to be performed
        if(StatusCheck() == BEP_STATUS_SUCCESS)
        {   // Determine which test step to perform
            // Establish Module Comms
            if(!testStep.compare("PerformModuleLinkup"))           result = PerformModuleLinkup();
            // Read sensor speeds from the module
            else if(!testStep.compare("ReadSensorSpeeds"))         result = ReadSensorSpeeds();
            // test the ABS messages
            else if(!testStep.compare("TestAbsMessages"))          result = AbsTestMessages();
            else if(!testStep.compare("SensorTest"))	           result = SensorTest();

            // Torque the rollers
            else if(!testStep.compare("ApplyTorqueToAllRollers"))  result = TestStepApplyTorqueToAllRollers(value);
            // Do the low speed ABS test
            else if(!testStep.compare("LowSpeedValveFiringTest"))  result = TestStepLowSpeedValveFiringTest(value);
            // Zero torque the rollers
            else if(!testStep.compare("TorqueZero"))               result = TestStepTorqueZero(value);

            else if(!testStep.compare("ABSValveFiringTest")) result = ABSValveFiringTest();
            else if(!testStep.compare("DynamicABSValveFiringTest")) result = DynamicABSValveFiringTest();
            else if(!testStep.compare("DisableSpeedLimit"))  result = DisableSpeedLimit();
            else if(!testStep.compare("DisableValveRelayShutdown")) result = DisableValveRelayShutdown();
            else if(!testStep.compare("EnableSpeedLimit")) result = EnableSpeedLimit();
            else if(!testStep.compare("EnableValveRelayShutdown"))  result = EnableValveRelayShutdown();
            else if(!testStep.compare("CheckRelayState")) result = CheckRelayState();
            else if(!testStep.compare("CheckPumpState")) result = CheckPumpState();
            else if(!testStep.compare("UnlockModuleSecurity"))  result = UnlockModuleSecurity();
            else if(!testStep.compare("LFSensorTest"))  result = LFSensorTest();
            else if(!testStep.compare("RFSensorTest"))  result = RFSensorTest();
            else if(!testStep.compare("LRSensorTest"))  result = LRSensorTest();
            else if(!testStep.compare("RRSensorTest"))  result = RRSensorTest();
            else if(!testStep.compare("CheckPartNumber"))  result = CheckPartNumber();
            else if(!testStep.compare("BatteryVoltageCheck")) result = BatteryVoltageCheck();
            else if (!testStep.compare("WriteVacuumFillingEndFlag"))   result = TestStepWriteVacuumFillingEndFlag();
            // Try the base class
            else                                  result = GenericABSTCTemplate<ModuleType>::CommandTestStep(value);
        }
        else
        {   // Bad system status, not performing the test step.
            result = ConvertStatusToResponse(StatusCheck());
            Log(LOG_ERRORS, "AdvicsABSTC::CommandTestStep: Status check failed: %s", result.c_str());
        }
    }
    catch(BepException &excpt)
    {
        Log(LOG_ERRORS, "AdvicsABSTC::CommandTestStep() - Exception from step %s - %s", testStep.c_str(), excpt.GetReason());
        result = BEP_ERROR_RESPONSE;
    }
    Log(LOG_FN_ENTRY, "AdvicsABSTC::CommandTestStep returning: %s", result.c_str());
    return result;
}
template <class ModuleType>
string AdvicsABSTC<ModuleType>::ExitDiagnosticMode(void)
{   // Over loaded to always pass - module will not reply to this message
    // chicken and the egg
    string testResult = BEP_PASS_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if(!ShortCircuitTestStep())
    {
        try
        {   // Try to exit diagnostic mode
            m_vehicleModule.CommandModule("ExitDiagnosticMode");
        }
        catch(ModuleException& caughtModuleException)
        {
            Log(LOG_ERRORS, "%s.%s: %s\n", 
                GetComponentName().c_str(), GetTestStepName().c_str(), caughtModuleException.message().c_str());
            testResult = testSoftwareFail;
            testResultCode = GetFaultCode("SoftwareFailure");
            testDescription = GetFaultDescription("SoftwareFailure");
        }
        // Report the result of the test
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
    }
    // Log the function exit
    Log(LOG_FN_ENTRY, "%s::%s - Exit - %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
    // return the test result
    return testResult;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::PerformModuleLinkup(void)
{
    string testResult(BEP_TESTING_STATUS);
    string resultCode("0000");
    string description(GetTestStepInfo("Description"));
    INT32 newPriority = 0;
    INT32 oldPriority = 0;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Log the entry and determine if the test should be performed
    Log(LOG_FN_ENTRY, "AdvicsABSTC::PerformModuleLinkup() - Enter");
    if(!ShortCircuitTestStep())
    {   // Ensure the cable is connected
        if(CheckCableConnect())
        {   // Reset the system priority so we can accurately perform the slow baud init
            Log(LOG_DEV_DATA, "Setting process priority to %s", GetTestStepInfo("TestStepPriority").c_str());
            // If the priority was set, change the priority
            if(newPriority != 0)    oldPriority = BposSetPriority(0, newPriority);
            try
            {
                int retry = 3;
                while (retry-- && status != BEP_STATUS_SUCCESS)
                {
                    Log(LOG_DEV_DATA, "Module init method: %s", GetParameterBool("UseFastInitLinkup") ? "Fast Init" : "Low baud");
                    if(GetParameterBool("UseFastInitLinkup"))
                    {
                        status = m_vehicleModule.PerformFastInitWakeup();
                    }
                    else
                    {
                        status = m_vehicleModule.PerformModuleLinkup();
                    }

    
                    if (status != BEP_STATUS_SUCCESS)
                    {
                        //prompt driver to restart

                        DisplayTimedPrompt("KeyOff", "1", 0, 10000);
                        DisplayTimedPrompt("StartVehicle", "1", 0, 6000);
                        status = m_vehicleModule.PerformModuleLinkup();
                    }
                }
                Log(LOG_ERRORS, "Module linkup status: %s", ConvertStatusToResponse(status).c_str());
                testResult = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
                resultCode = !testResult.compare(testPass) ? resultCode : GetFaultCode("ModuleLinkupFailure");
                description = !testResult.compare(testPass) ? description : GetFaultDescription("ModuleLinkupFailure");
            }
            catch(ModuleException &excpt)
            {  
                Log(LOG_ERRORS, "ModuleException in Apg3550::PerformModuleLinkup() - %s", excpt.GetReason());
                testResult = testSoftwareFail;
                resultCode = GetFaultCode("SoftwareFailure");
                description = GetFaultDescription("SoftwareFailure");
            }
            // Reset the priority if needed
            if((newPriority != 0) && (oldPriority != -1))  BposSetPriority(0, oldPriority);
        }
        else
        {   // Timeout waiting for the cable to be connected
            Log(LOG_ERRORS, "Timeout waiting for the operator to connect the cable");
            testResult = testTimeout;
            resultCode = GetFaultCode("CableConnectTimeout");
            description = GetFaultDescription("CableConnectTimeout");
        }
        // Report the test result
        SendTestResult(testResult, description, resultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AdvicsABSTC::PerformModuleLinkup() - Exit");
    return testResult;
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::TestStepApplyTorqueToAllRollers(const string &value)
{
    return ApplyTorqueToRollers(GetParameter("LowSpeedAbs_LeftFrontTorque"), 
                                GetParameter("LowSpeedAbs_RightFrontTorque"), 
                                GetParameter("LowSpeedAbs_LeftRearTorque"), 
                                GetParameter("LowSpeedAbs_RightRearTorque"));
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::TestStepLowSpeedValveFiringTest(const string &value)
{
    string testResult = testPass;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_DEV_DATA,"Enter TestStepLowSpeedValveFiringTest()");
    DetermineSystemConfiguration();

    if (!ShortCircuitTestStep())
    {
        string reductionResult = testFail;
        string recoveryResult = testFail;
        string initialConditionsResult = testFail;
        string faultTag;

        //testing forces
        m_absStartIndex = TagArray("ABSStart");

        // Prompt operator to apply brake pedal
        DisplayPrompt(GetPromptBox("DepressBrake"), GetPrompt("DepressBrake"), GetPromptPriority("DepressBrake"));
        StatusSleep(3000);
        // Determine firing order - Always use 4-channel
        XmlNodeMap *valveFiringSequence = &m_abs4ChannelFiringOrder;
        BEP_STATUS_TYPE status = m_vehicleModule.CommandModule("AbsTestMsg");
        status = m_vehicleModule.CommandModule("AbsTestMsg");
        //if vehicle is three channel, need to send out commands for all channels - do not fail LR
        for (XmlNodeMapItr iter = valveFiringSequence->begin(); iter != valveFiringSequence->end(); iter++)
        {
            status = m_vehicleModule.CommandModule("AbsTestMsg");   // Fire wheel
            if (status == BEP_STATUS_SUCCESS)
            {
                m_vehicleModule.HoldAllValves();                        // Hold valves
                initialConditionsResult = VerifyWheelStopped(iter->second->getValue());
                if (initialConditionsResult == testPass)
                {
                    RollerIndexItr rollerIter = m_rollerIndex.find(iter->second->getValue());
                    INT32 wheelNameIndex = (rollerIter != m_rollerIndex.end()) ? rollerIter->second : 0;
                    m_baseBrakeTool->SetBrakeForceValue(wheelNameIndex, GetParameterFloat("LowSpeedAbs_LeftFrontTorque"));
                    Log(LOG_DEV_DATA, "Begin reduction for %s\n", iter->second->getValue().c_str());

                    reductionResult = Reduction(iter->second->getValue(),
                                                m_reduxRecovIndex[wheelNameIndex].reductionStart,
                                                m_reduxRecovIndex[wheelNameIndex].reductionEnd);
                    if ((!IsFourChannelSystem() && (iter->second->getValue() != "LeftRear")) || IsFourChannelSystem())
                    {
                        if (reductionResult == testPass)
                        {   // Gice the roller time to spin up
                            BposSleep(GetParameterInt("ReductionDelay"));
                            reductionResult = VerifyWheelSpeed(iter->second->getValue());
                            Log(LOG_DEV_DATA, "Reduction wheelspeed result: %s\n", reductionResult.c_str());

                            // if the reduction test passed, check for a sensor cross
                            if((reductionResult == testPass) && GetParameterBool("CheckSensorCross"))
                            {
                                CheckSensorCross(iter->second->getValue());
                            }
                        }
                        else
                        {
                            Log(LOG_DEV_DATA, "Reduction message failure\n");
                            testResult = testFail;
                        }
                    }
                    else 
                    {
                        BposSleep(500);
                    }
                    Log(LOG_DEV_DATA, "Begin recovery for %s\n", iter->second->getValue().c_str());
                    recoveryResult = Recovery(iter->second->getValue(),
                                              m_reduxRecovIndex[wheelNameIndex].recoveryStart,
                                              m_reduxRecovIndex[wheelNameIndex].recoveryEnd);

                    if ((!IsFourChannelSystem() && (iter->second->getValue() != "LeftRear")) || IsFourChannelSystem())
                    {
                    if (recoveryResult == testPass)
                    {
                        BposSleep(GetParameterInt("RecoveryDelay"));
                        recoveryResult = VerifyWheelStopped(iter->second->getValue());
                        Log(LOG_DEV_DATA, "Recovery wheelspeed result: %s\n", recoveryResult.c_str());
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "Recovery message failure\n");
                        testResult = testFail;
                    }
                    }
                    else 
                    {
                        BposSleep(500);
                    }

                    if(reductionResult != testPass || recoveryResult != testPass)
                    {
                        Log(LOG_DEV_DATA, "Reduction/Recovery wheel speed verification failure\n");
                        testResult = testFail;
                    }

                    faultTag = iter->second->getValue()+"ReductionFail";
                    testResultCode = (reductionResult == testPass ? "0000" : GetFaultCode(faultTag));
                    testDescription = (reductionResult == testPass ? iter->second->getValue()+" Reduction Test" : GetFaultDescription(faultTag));
                    SendSubtestResult(iter->second->getValue() + "ABSReduction", reductionResult, testDescription, testResultCode);

                    faultTag = iter->second->getValue()+"RecoveryFail";
                    testResultCode = (recoveryResult == testPass ? "0000" : GetFaultCode(faultTag));
                    testDescription = (recoveryResult == testPass ? iter->second->getValue()+" Recovery Test" : GetFaultDescription(faultTag));
                    SendSubtestResult(iter->second->getValue() + "ABSRecovery", recoveryResult, testDescription, testResultCode);
                }
                else
                {
                    Log(LOG_DEV_DATA, "TestStepLowSpeedValveFiringTest(), Initial conditions not correct (rollers speed over minimum)\n");
                    testResult = testFail;
                    testResultCode = GetFaultCode(iter->second->getValue() + "ABSInitialConditionsFail");
                    testDescription = GetFaultDescription(iter->second->getValue() + "ABSInitialConditionsFail");
                    SendSubtestResult(iter->second->getValue() + "ABSInitialConditions", initialConditionsResult, GetFaultCode(faultTag), 
                                      GetFaultDescription(faultTag));
                }
                // Send out the inter-wheel messages
                for(INT16 index = 0; (index < GetParameterInt("EndMessageCount")) && (BEP_STATUS_SUCCESS == status); index++)
                {
                    status = m_vehicleModule.CommandModule("AbsTestMsg");
                }
            }
        }
        SendTestResult(testResult, testDescription, testResultCode);
        // Remove the prompt
        RemovePrompt(GetPromptBox("DepressBrake"), GetPrompt("DepressBrake"), GetPromptPriority("DepressBrake"));
        if (!IsFourChannelSystem())
        {
            m_reduxRecovIndex[LRWHEEL].reductionStart = m_reduxRecovIndex[RRWHEEL].reductionStart; 
            m_reduxRecovIndex[LRWHEEL].reductionEnd = m_reduxRecovIndex[RRWHEEL].reductionEnd;
            m_reduxRecovIndex[LRWHEEL].recoveryStart = m_reduxRecovIndex[RRWHEEL].recoveryStart;  
            m_reduxRecovIndex[LRWHEEL].recoveryEnd = m_reduxRecovIndex[RRWHEEL].recoveryEnd;    
        }

        m_absEndIndex = TagArray("ABSEnd");
    }
    else
    {   
        Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
        testResult = testSkip;
    }
    Log(LOG_DEV_DATA,"Exit AdvicsABSTC::TestStepLowSpeedValveFiringTest()");
    return testResult;
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::TestStepTorqueZero(const string &value)
{
    SetMotorTorque("0.0", "0.0", "0.0", "0.0");

    RemovePrompt(GetPromptBox("TestingAbs"),GetPrompt("TestingAbs"),GetPromptPriority("TestingAbs"));
    RemovePrompt(GetPromptBox("ShiftToNeutralMaintainBrake"),GetPrompt("ShiftToNeutralMaintainBrake"),GetPromptPriority("ShiftToNeutralMaintainBrake"));

    return testPass;
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::ApplyTorqueToRollers(string lfTorque, string rfTorque, 
                                                       string lrTorque, string rrTorque)
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

//-------------------------------------------------------------------------
template <class ModuleType>
void AdvicsABSTC<ModuleType>::SetMotorTorque(string lf, string rf, string lr, string rr)
{
    // Update MotorMode
    m_MotorController.Write(MOTOR_MODE,TORQUE_MODE,true);

    // Apply Torque
    m_MotorController.Write("LeftFrontTorqueValue",lf.c_str());
    m_MotorController.Write("RightFrontTorqueValue",rf.c_str());
    m_MotorController.Write("LeftRearTorqueValue",lr.c_str());
    m_MotorController.Write("RightRearTorqueValue",rr.c_str(),true);
}

//-------------------------------------------------------------------------
template <class ModuleType>
bool AdvicsABSTC<ModuleType>::IsRunawayPresent(int delay, int maxSpeed)
{
    // Delay for a moment, then verify that all rollers are met with enough resistance
    // to indicate a vehicle is present (to prevent "runaways")
    WHEELINFO wheelSpeed;

    BposSleep(delay);
    GetWheelSpeeds(wheelSpeed);

    return(wheelSpeed.lfWheel > maxSpeed ||
           wheelSpeed.lrWheel > maxSpeed ||
           wheelSpeed.rfWheel > maxSpeed ||
           wheelSpeed.rrWheel > maxSpeed);
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::VerifyWheelStopped(string wheel)
{
    float wheelSpeed[GetRollerCount()];
    string testResult; 

    GetWheelSpeeds(wheelSpeed);
    testResult = wheelSpeed[GetRollerNumber(wheel)] < GetParameterInt("LowSpeedAbs_ZeroSpeedTolerance") ? testPass : testFail;

    Log(LOG_DEV_DATA, "VerifyWheelSpeed - WheelSpeed: %f, ZeroSpeedTolerance: %d, Result: %s\n", 
        wheelSpeed[GetRollerNumber(wheel)], GetParameterInt("LowSpeedAbs_ZeroSpeedTolerance"), testResult.c_str());

    return(testResult);
}

//-------------------------------------------------------------------------
template <class ModuleType>
const string AdvicsABSTC<ModuleType>::VerifyWheelSpeed(string wheel)
{
    float wheelSpeed[GetRollerCount()];
    string testResult;

    GetWheelSpeeds(wheelSpeed);
    testResult = wheelSpeed[GetRollerNumber(wheel)] > GetParameterInt("LowSpeedAbs_WheelSpeedMinimum") ? testPass : testFail;

    Log(LOG_DEV_DATA, "VerifyWheelSpeed - WheelSpeed: %f, MinWheelSpeed: %d, Result: %s\n", 
        wheelSpeed[GetRollerNumber(wheel)], GetParameterInt("LowSpeedAbs_WheelSpeedMinimum"), testResult.c_str());

    return(testResult);
}

//-------------------------------------------------------------------------
template <class ModuleType>
void AdvicsABSTC<ModuleType>::CheckSensorCross(string wheel)
{
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    WheelSpeeds_t speeds;
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = wheel + " Sensor Cross Test";

    Log(LOG_FN_ENTRY,"Enter ::CheckSensorCross()");

    // read the sensor speeds from the module
    moduleStatus = m_vehicleModule.ReadSensorSpeeds(speeds);
    if(moduleStatus == BEP_STATUS_SUCCESS)
    {
        if(wheel == "LeftFront")
        {
            if((speeds[0] <= speeds[1]) || (speeds[0] <= speeds[2]) || (speeds[0] <= speeds[3]))
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("LeftFrontSensorCross");
                testDescription = GetFaultDescription("LeftFrontSensorCross");
            }
            else testResult = testPass;
        }
        else if(wheel == "RightFront")
        {
            if((speeds[1] <= speeds[0]) || (speeds[1] <= speeds[2]) || (speeds[1] <= speeds[3]))
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("RightFrontSensorCross");
                testDescription = GetFaultDescription("RightFrontSensorCross");
            }
            else testResult = testPass;
        }
        else if(wheel == "LeftRear")
        {
            if((speeds[2] <= speeds[0]) || (speeds[2] <= speeds[1]) || (speeds[2] <= speeds[3]))
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("LeftRearSensorCross");
                testDescription = GetFaultDescription("LeftRearSensorCross");
            }
            else testResult = testPass;
        }
        else if(wheel == "RightRear")
        {
            if((speeds[3] <= speeds[0]) || (speeds[3] <= speeds[1]) || ((speeds[3] <= speeds[2]) && IsFourChannelSystem()))
            {
                testResult = testFailure;
                testResultCode = GetFaultCode("RightRearSensorCross");
                testDescription = GetFaultDescription("RightRearSensorCross");
            }
            else testResult = testPass;
        }
    }
    else
    {
        testResult = testFailure;
        testResultCode = GetFaultCode("CommunicationFailure");
        testDescription = GetFaultDescription("CommunicationFailure");
    }

    SendSubtestResult(wheel + "SensorCrossTest",testResult,testDescription,testResultCode);

    Log(LOG_FN_ENTRY,"Exit CheckSensorCross()");
}

//-------------------------------------------------------------------------
template <class ModuleType>
const INT32 AdvicsABSTC<ModuleType>::GetRollerNumber(string rollerName)
{
    INT32 rollerNumber = 0;

    if (rollerName == "LeftFront")
        rollerNumber = 0;
    else if (rollerName == "RightFront")
        rollerNumber = 1;
    else if (rollerName == "LeftRear")
        rollerNumber = 2;
    else if (rollerName == "RightRear")
        rollerNumber = 3;

    return rollerNumber;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::ReadSensorSpeeds(void)
{
    WheelSpeeds_t moduleSpeeds;
    // Read the sensor speeds from the module
    BEP_STATUS_TYPE status = m_vehicleModule.ReadSensorSpeeds(moduleSpeeds);
    if(BEP_STATUS_SUCCESS == status)
    {   // Print each wheel speed
        Log(LOG_DEV_DATA, "Left  Front Speed: %.2f", moduleSpeeds[0]);
        Log(LOG_DEV_DATA, "Right Front Speed: %.2f", moduleSpeeds[1]);
        Log(LOG_DEV_DATA, "Left  Rear  Speed: %.2f", moduleSpeeds[2]);
        Log(LOG_DEV_DATA, "Right Rear  Speed: %.2f", moduleSpeeds[3]);
    }
    else
    {
        Log(LOG_ERRORS, "Could not read sensor speeds - %s", ConvertStatusToResponse(status).c_str());
    }
    return (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::AbsTestMessages(void)
{
    BEP_STATUS_TYPE status = BEP_STATUS_SUCCESS;
    for(INT16 index = 0; (index < GetParameterInt("AbsMessageCount")) && (BEP_STATUS_SUCCESS == status); index++)
    {
        status = m_vehicleModule.CommandModule("AbsTestMsg");
        BposSleep(GetParameterInt("TestMessageGap"));
    }
    return (status == BEP_STATUS_SUCCESS) ? testPass : testFail;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::SensorTest(void)
{
    string testResult(testPass);
    // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "Apg3550::SensorTest() - Enter");
    if(!ShortCircuitTestStep())
    {   
        string testResultCode("0000");
        string testDescription(GetTestStepInfo("Description"));
        // Clear out any commanded history in the motors before switching modes
        m_MotorController.Write("LeftFrontTorqueValue", "0.00", false);
        m_MotorController.Write("RightFrontTorqueValue", "0.00", false);
        m_MotorController.Write("LeftRearTorqueValue", "0.00", false);
        m_MotorController.Write("RightRearTorqueValue", "0.00", false);
        m_MotorController.Write("LeftFrontSpeedValue", "0.00", false);
        m_MotorController.Write("RightFrontSpeedValue", "0.00", false);
        m_MotorController.Write("LeftRearSpeedValue", "0.00", false);
        m_MotorController.Write("RightRearSpeedValue", "0.00", true);
        DisplayPrompt(GetPromptBox("LowSpeedSensorTest"), GetPrompt("LowSpeedSensorTest"), GetPromptPriority("LowSpeedSensorTest"));
        DisplayPrompt(GetPromptBox("FootOffBrake"), GetPrompt("FootOffBrake"), GetPromptPriority("FootOffBrake"));
        BposSleep( 100);
        // Set the motor controller to speed mode
        m_MotorController.Write(MOTOR_MODE, SPEED_MODE, true);
        BposSleep(100);
        // Check each wheel
        for(UINT8 index = 0; (index < GetRollerCount()) && !testResult.compare(testPass); index++)
        {   // Command the roller to speed
            m_MotorController.Write(rollerName[index]+"SpeedValue", GetParameter("LowSpeedSensorTestRollerSpeed"), true);
            BposSleep(100);
            // Wait for the roller to achieve the desired speed
            bool rollerAtSpeed = false;
            float wheelSpeeds[GetRollerCount()];
            while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !rollerAtSpeed)
            {   // Check the current wheel speed
                GetWheelSpeeds(wheelSpeeds);
                rollerAtSpeed = (wheelSpeeds[index] >= (GetParameterFloat("LowSpeedSensorTestRollerSpeed") - GetParameterFloat("LowSpeedSensorTestRollerOffset")));
                if(!rollerAtSpeed)  BposSleep(GetParameterInt("LowSpeedSensorSpeedCheckDelay"));
            }
            // Proceed only if the roller speed is in range
            if(rollerAtSpeed)
            {   // Read the sensor speed from the module
                WheelSpeeds_t moduleSpeeds;
                BEP_STATUS_TYPE status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
                if(BEP_STATUS_SUCCESS == status)
                {   // Check the module speed against the roller speed
                    GetWheelSpeeds(wheelSpeeds);
                    float lowerLimit = wheelSpeeds[index] * ((100.0 - GetParameterFloat("SensorTolerance")) / 100.0);
                    float upperLimit = wheelSpeeds[index] * ((100.0 + GetParameterFloat("SensorTolerance")) / 100.0);
                    testResult = (lowerLimit < moduleSpeeds[index] < upperLimit) ? testPass : testFail;
                    testResultCode = !testResult.compare(testPass) ? "0000" : GetFaultCode("SensorOutOfTolerance");
                    testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("SensorOutOfTolerance");
                    Log(LOG_DEV_DATA, "%s:  Roller Speed: %.2f,  Sensor Speed: %.2f - %s  [%.2f, %.2f]",
                        rollerName[index].c_str(), wheelSpeeds[index], moduleSpeeds[index], testResult.c_str(),
                        lowerLimit, upperLimit);
                    // Report the result
                    char temp[16];
                    SendSubtestResultWithDetail(rollerName[index]+"SensorTest", testResult, testDescription, testResultCode,
                                                "SensorSpeed", CreateMessage(temp,sizeof(temp),"%.2f", moduleSpeeds[index]), unitsMPH,
                                                "RollSpeed", CreateMessage(temp, sizeof(temp), "%.2f", wheelSpeeds[index]), unitsMPH,
                                                "LowerLimit", CreateMessage(temp, sizeof(temp), "%.2f", lowerLimit), unitsMPH,
                                                "UpperLimit", CreateMessage(temp, sizeof(temp), "%.2f", upperLimit), unitsMPH,
                                                "Tolerance", GetParameter("SensorTolerance"), "Percent");
                }
                else
                {   // Could not read sensor speeds from the module
                    testResult = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    Log(LOG_ERRORS, "Could not read sensor speeds from the module - status: %s", 
                        ConvertStatusToResponse(status).c_str());
                }
                // Command the roller back to zero speed
                m_MotorController.Write(rollerName[index]+"SpeedValue", "0.00", true);
                BposSleep(100);
                // Wait for the roller to achieve the desired speed
                rollerAtSpeed = false;
                while(TimeRemaining() && (StatusCheck() == BEP_STATUS_SUCCESS) && !rollerAtSpeed)
                {   // Check the current wheel speed
                    GetWheelSpeeds(wheelSpeeds);
                    rollerAtSpeed = (wheelSpeeds[index] <= 1.0);
                    if(!rollerAtSpeed)  BposSleep(GetParameterInt("LowSpeedSensorSpeedCheckDelay"));
                }
            }
            else
            {   // Failed to command roller to speed
                testResult = testFail;
                testResultCode = GetFaultCode("TargetSpeedTimeout");
                testDescription = GetFaultDescription("TargetSpeedTimeout");
                Log(LOG_ERRORS, "Roller speed not in target range - cannot perform the sensor test (%s)", rollerName[index].c_str());
            }
        }

        RemovePrompt(GetPromptBox("LowSpeedSensorTest"),GetPrompt("LowSpeedSensorTest"),GetPromptPriority("LowSpeedSensorTest"));
        RemovePrompt(GetPromptBox("FootOffBrake"),GetPrompt("FootOffBrake"),GetPromptPriority("FootOffBrake"));
        SendTestResult(testResult, testDescription, testResultCode);
    }
    else
    {   // Need to skip this test step
        testResult = testSkip;
        Log(LOG_FN_ENTRY, "Skipping sensor test");
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "AdvicsABSTC::SensorTest() - Exit");
    return testResult;
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::DetermineSystemConfiguration(void)
{
    string testResult = testPass;
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Determine if this step should be performed
    if (!ShortCircuitTestStep() && (testPass != GetTestStepResult()) && (testSkip != GetTestStepResult()))
    {   // Attempt to read the system config from the module
        try
        {
            SetFourChannelSystem(GetParameterBool("IsFourChannelSystem"));
        }
        catch (...)
        {
            SetFourChannelSystem(false);
        }
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::ValveFiringTest()
{
    string testResult             = BEP_TESTING_STATUS;     // Used to report test step results
    string testResultCode         = "0000";
    string testDescription        = GetTestStepInfo("Description");
    string overallReductionResult = BEP_TESTING_STATUS;
    string overallRecoveryResult  = BEP_TESTING_STATUS;
    INT32  sensorSpeedIndex       = 0;
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    // Define a structure for storing the local results
    struct _valveFiringResults
    {
        string reductionResult;
        string recoveryResult;
    } valveFiringResults[GetRollerCount()];
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    // Verify it is ok to test -- not skip, brake testing in progress or development testing
    if ((!ShortCircuitTestStep() && (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)) ||
        GetParameterBool("DeveloperTest"))
    {   // Instruct operator and update GUI
        UpdatePrompts();
        // Determine if the force meter should be updated during valve firing
        if (GetParameterBool("DisableMeterForValveFiring"))
            m_baseBrakeTool->DisableForceUpdates();
        else
            m_baseBrakeTool->EnableForceUpdates();
        // Determine if recovery or reduction should be performed first
        bool reductionFirst = GetParameterBool("PerformReductionBeforeRecovery");
        // Tag the start of the reduction/recovery sequence
        m_absStartIndex = TagArray("ABSStart");
        // Determine which firing sequence to use
        XmlNodeMap *firingOrder = IsFourChannelSystem() ? &m_abs4ChannelFiringOrder : &m_abs3ChannelFiringOrder;
        // Get the initial speeds if doing sensor cross check
        if (GetParameterBool("CollectSensorSpeedData"))
        {   // Wait for forces to stabilize
            BposSleep(GetParameterInt("BrakeForceStabilizeTime"));
            // Get the roller speeds
            m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
            // Get the wheel speeds from the module
            m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
            sensorSpeedIndex++;
        }
        //apg necessary
        status = m_vehicleModule.CommandModule("AbsTestMsg");
        if (status == BEP_STATUS_SUCCESS)
        {
            status = m_vehicleModule.CommandModule("AbsTestMsg");
            if (status == BEP_STATUS_SUCCESS)
            {
            // Fire each wheel in the sequence
            for (XmlNodeMapItr iter = firingOrder->begin(); iter != firingOrder->end() && status == BEP_STATUS_SUCCESS; iter++)
            {   // Get the index for the current wheel
                RollerIndexItr rollerIter;
                if (((rollerIter = m_rollerIndex.find(iter->second->getValue())) != m_rollerIndex.end()) ||
                    (iter->second->getValue() == "Rear"))
                {
                    //abstestmsg

                    m_vehicleModule.CommandModule("AbsTestMsg");
                    //HoldValves

                    m_vehicleModule.HoldAllValves();                        // Hold valves

                    INT32 wheelNameIndex;
                    // Determine the index to use
                    if (!IsFourChannelSystem() && (iter->second->getValue() == "Rear"))
                    {   // Three channel system, left rear and right rear have same indices.
                        // NOTE: Left rear will be used for indices.  Right rear will be updated after the
                        //       firing for the rears is complete.
                        wheelNameIndex = LRWHEEL;
                    }
                    else
                    {
                        wheelNameIndex = rollerIter->second;
                    }
                    // Provide a short delay between valves -- NOTE: use delay instead of BposSleep(), timing is critical!
                    delay(GetParameterInt("InterWheelGapDelay"));
                    // Determine if reduction or recovery should be performed first
                    if (reductionFirst)
                    {   // Reduce the wheel
                        valveFiringResults[wheelNameIndex].reductionResult = 
                            Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
                                      m_reduxRecovIndex[wheelNameIndex].reductionEnd);

                        if (!IsFourChannelSystem() && (iter->second->getValue() == "LeftRear"))
                        {
                            //BposSleep(500);
                        }
                        // Recover the wheel
                        valveFiringResults[wheelNameIndex].recoveryResult = 
                            Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
                                     m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
                        if (!IsFourChannelSystem() && (iter->second->getValue() == "LeftRear"))
                        {
                            //BposSleep(500);
                        }
                    }
                    else
                    {
                        // Recover the wheel
                        valveFiringResults[wheelNameIndex].recoveryResult = 
                            Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
                                     m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
                        // Reduce the wheel
                        valveFiringResults[wheelNameIndex].reductionResult = 
                            Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
                                      m_reduxRecovIndex[wheelNameIndex].reductionEnd);
                    }
                    // Get the wheel speed data if performing a wheel speeds sensor cross test
                    if (GetParameterBool("CollectSensorSpeedData"))
                    {   // Get the roller speeds
                        Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
                            sensorSpeedIndex);
                        m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
                        // Get the wheel speeds from the module
                        Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
                            sensorSpeedIndex);
                        m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
                        sensorSpeedIndex++;
                    }
                }
                else
                {   // No roller index specified
                    Log(LOG_ERRORS, "No roller index defined for wheel: %s -- Not firing valves\n", 
                        iter->second->getValue().c_str());
                }
                // Send out the inter-wheel messages
                for(INT16 index = 0; (index < GetParameterInt("EndMessageCount")) && status == BEP_STATUS_SUCCESS; index++)
                {
                    status = m_vehicleModule.CommandModule("AbsTestMsg");
                }
            }
            }
        }
        // Get the final speed data if performing a wheel speeds sensor cross test
        if (GetParameterBool("CollectSensorSpeedData"))
        {   // Get the roller speeds
            Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
                sensorSpeedIndex);
            m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
            // Get the wheel speeds from the module
            Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
                sensorSpeedIndex);
            m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
        }
        // Tag the end of the ABS sequence
        m_absEndIndex = TagArray("ABSEnd");
        // If this is a three channel system, update the indices so the left rear and right rear
        // wheels have the same index
        if (!IsFourChannelSystem())
        {
            m_reduxRecovIndex[LRWHEEL].reductionStart = m_reduxRecovIndex[RRWHEEL].reductionStart;
            m_reduxRecovIndex[LRWHEEL].reductionEnd = m_reduxRecovIndex[RRWHEEL].reductionEnd;  
            m_reduxRecovIndex[LRWHEEL].recoveryStart = m_reduxRecovIndex[RRWHEEL].recoveryStart; 
            m_reduxRecovIndex[LRWHEEL].recoveryEnd = m_reduxRecovIndex[RRWHEEL].recoveryEnd;   
        }
        // Remove the operator prompts
        RemovePrompts();
        // Assume all wheels have passed unless otherwise indicated
        overallReductionResult = testPass;
        overallRecoveryResult  = testPass;
        // Determine the results of the test
        for (UINT32 wheelIndex = 0; wheelIndex < firingOrder->size(); wheelIndex++)
        {   // Check if the reduction sequence on this wheel failed
            if (testPass != valveFiringResults[wheelIndex].reductionResult)  overallReductionResult = testFail;
            // Check if the recovery sequence on this wheel failed
            if (testPass != valveFiringResults[wheelIndex].recoveryResult)   overallRecoveryResult  = testFail;
        }
        if (status != BEP_STATUS_SUCCESS)
        {
                testResult = testFailure;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
        }
        // Send the results for this step
        testResult = ((overallReductionResult == testPass) && (overallRecoveryResult == testPass)) ? testPass : testFail;
        testResultCode = testPass == testResult ? testResultCode : GetFaultCode("ReduxRecovSequenceFail");
        testDescription = testPass == testResult ? testDescription : GetFaultDescription("ReduxRecovSequenceFail");
        SendTestResultWithDetail(testResult, testDescription, testResultCode,
                                 "OverallReductionSequence", overallReductionResult, "",
                                 "OverallRecoverySequence", overallRecoveryResult, "");
    }
    else
    {   // Test step is being skipped
        Log(LOG_FN_ENTRY, "Skipping %s -- \n\t\tShortCircuitTestStep: %s\n\t\tBrakeTestingStatus: %s\n\t\tDeveloperTest: %s\n",
            GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
            m_baseBrakeTool->GetBrakeTestingStatus().c_str(), GetParameterBool("DeveloperTest") ? "True" : "False");
        testResult = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LFSensorTest(void)
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

	Log(LOG_DEV_DATA, "AdvicsABSTC::LFSensorTest - Enter\n");

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
			UpdatePrompts();		// "Shift to neutral and release brake"
			BposSleep(startDelay);	   // give driver time to shift

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
			{	// Command all rolls to speed mode
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
			while(m_MotorController.GetNext(tag, value) > 0)
			{
				Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
			}

			// wait for the left front roll to reach the sensor test speed
			rollSpeeds[ 0] = 0;
			while(rollSpeeds[0] < (sensorTestSpeedVal))
			{
				BposSleep(100);
				m_baseBrakeTool->GetISpeeds(rollSpeeds);
				if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
			}

			// Ask the module for sensor speeds
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

			if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
			{
				// check front sensor cross side to side
				if(moduleSpeeds[LFWHEEL] <= moduleSpeeds[RFWHEEL])
				{
					testResult = testFail;
				}
				else testResult	  = testPass;
			}
			else				 // error reading the sensors
			{
				SetCommunicationFailure(true);	   // set comm fault flag
				testResult = testFail;
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

		if(testResult == testPass)	  // if the test step passed
		{
			SendTestResult(testPass,testDescription);
		}
		else					// test step failed
		{
			if(GetCommunicationFailure() == false)	 // if no comm error
			{
				SendTestResultWithDetail(testResult,testDescription,
										 GetFaultCode(testDescription),
										 "LeftFrontSensorFail",
										 GetFaultDescription(testDescription));
			}
		}
	}

	Log(LOG_DEV_DATA, "AdvicsABSTC::LFSensorTest - Exit %s\n",testResult.c_str());

	return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RFSensorTest(void)
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

	Log(LOG_DEV_DATA, "AdvicsABSTC::RFSensorTest - Enter\n");

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
			UpdatePrompts();		// "Shift to neutral and release brake"

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
			while(m_MotorController.GetNext(tag, value) > 0)
			{
				Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
			}

			// wait for the right front roll to reach the sensor test speed
			rollSpeeds[ 1] = 0;
			while(rollSpeeds[1] < (sensorTestSpeedVal - 2))
			{
				BposSleep(100);
				m_baseBrakeTool->GetISpeeds(rollSpeeds);
				if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
			}

			// Ask the module for sensor speeds
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

			if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
			{
				// check front to rear sensor cross
				if((moduleSpeeds[RFWHEEL] <= moduleSpeeds[RRWHEEL]) ||
				   (moduleSpeeds[RFWHEEL] <= moduleSpeeds[LRWHEEL]))
				{
					testResult = testFail;
				}
				else testResult	  = testPass;
			}
			else				 // error reading the sensors
			{
				SetCommunicationFailure(true);	   // set comm fault flag
				testResult = testFail;
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

		if(testResult == testPass)	  // if the test step passed
		{
			SendTestResult(testPass,testDescription);
		}
		else					// test step failed
		{
			if(GetCommunicationFailure() == false)	 // if no comm error
			{
				SendTestResultWithDetail(testResult,testDescription,
										 GetFaultCode(testDescription),
										 "RightFrontSensorFail",
										 GetFaultDescription(testDescription));
			}
		}
	}
	Log(LOG_DEV_DATA, "AdvicsABSTC::RFSensorTest - Exit %s\n",testResult.c_str());

	return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LRSensorTest(void)
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

	Log(LOG_DEV_DATA, "Apg3550::LRSensorTest - Enter\n");

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
			UpdatePrompts();		// "Shift to neutral and release brake"

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
			while(m_MotorController.GetNext(tag, value) > 0)
			{
				Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
			}

			// wait for the left rear roll to reach the sensor test speed
			rollSpeeds[ 2] = 0;
			while(rollSpeeds[2] < (sensorTestSpeedVal - 2))
			{
				BposSleep(100);
				m_baseBrakeTool->GetISpeeds(rollSpeeds);
				if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)	break;
			}

			// Ask the module for sensor speeds
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

			if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
			{
				// check rear sensor cross side to side
				if(moduleSpeeds[LRWHEEL] <= moduleSpeeds[RRWHEEL])
				{
					testResult = testFail;
				}
				else testResult	  = testPass;
			}
			else				 // error reading the sensors
			{
				SetCommunicationFailure(true);	   // set comm fault flag
				testResult = testFail;
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

		if(testResult == testPass)	  // if the test step passed
		{
			SendTestResult(testPass,testDescription);
		}
		else					// test step failed
		{
			if(GetCommunicationFailure() == false)	 // if no comm error
			{
				SendTestResultWithDetail(testResult,testDescription,
										 GetFaultCode(testDescription),
										 "LeftRearSensorFail",
										 GetFaultDescription(testDescription));
			}
		}
	}

	Log(LOG_DEV_DATA, "AdvicsABSTC::LRSensorTest - Exit %s\n",testResult.c_str());

	return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RRSensorTest(void)
{
	string testResult      = BEP_TESTING_STATUS;
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	float rollSpeeds[4];
	WheelSpeeds_t moduleSpeeds;

	// sensor test speed
	std::string sensorTestSpeed = GetParameter("SensorTestSpeed");
	float sensorTestSpeedVal = GetParameterFloat("SensorTestSpeed");

	Log(LOG_DEV_DATA, "AdvicsABSTC::RRSensorTest - Enter\n");

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
			UpdatePrompts();		// "Shift to neutral and release brake"

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
			while(m_MotorController.GetNext(tag, value) > 0)
			{
				Log("Tag: %s, value: %s\n", tag.c_str(), value.c_str());
			}

			// wait for the right rear roll to reach the sensor test speed
			rollSpeeds[ 3] = 0;
			while(rollSpeeds[3] < (sensorTestSpeedVal - 1))
			{
				BposSleep(100);
				m_baseBrakeTool->GetISpeeds(rollSpeeds);
				if(!TimeRemaining() || StatusCheck() != BEP_STATUS_SUCCESS)
				{
					testResult = testFail;
					break;
				}
			}

            // Ask the module for sensor speeds
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);

			if(status == BEP_STATUS_SUCCESS) // if sensors read successfully
			{
                testResult	  = testPass;
			}
			else				 // error reading the sensors
			{
				SetCommunicationFailure(true);	   // set comm fault flag
				testResult = testFail;
				testDescription = GetFaultDescription("CommunicationFailure");
			};

			// if the test result was not set to failed, now set to pass
			if(testResult == BEP_TESTING_STATUS) testResult = testPass;
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}

		if(testResult == testPass)	  // if the test step passed
		{
			SendTestResult(testPass,testDescription);
		}
		else					// test step failed
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

	Log(LOG_DEV_DATA, "AdvicsABSTC::RRSensorTest - Exit %s\n",testResult.c_str());

	return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LFWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::LFABSTest()\n");

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
    delay(GetParameterInt("LFReductionPulse"));
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
                delay(GetParameterInt("LFRecoveryPulse"));
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::LFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RFABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RFWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::RFABSTest()\n");

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
    delay(GetParameterInt("RFReductionPulse"));
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::RFABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LRWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::LRABSTest()\n");

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
    delay(GetParameterInt("LRReductionPulse"));
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


    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::LRABSTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RRABSTest(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RRWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::RRABSTest()\n");

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
    delay(GetParameterInt("RRReductionPulse"));
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::RRABsTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LFABSTest2(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LFWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::LFABSTest2()\n");

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("LFABSReductionStart");
    moduleStatus = m_vehicleModule.CommandModule("LFABSReduction");
    delay(GetParameterInt("LFReductionPulse"));
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"LF ABS reduction OK\n");
        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("LFABSReductionEnd");
        moduleStatus = m_vehicleModule.CommandModule("LFABSHold");
        delay(GetParameterInt("ABSHoldDelay"));
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

        if(testResult == testPass)
        {
        
            m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("LFABSRecoveryStart");
            // Try to start the LF ABS recovery
            moduleStatus = m_vehicleModule.CommandModule("ABSPressureRelease");
            delay(GetParameterInt("LFRecoveryPulse"));

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
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error LF ABS Hold - status: %s\n", 
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::LFABSTest2()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RFABSTest2(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RFWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::RFABSTest2()\n");

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("RFABSReductionStart");
    moduleStatus = m_vehicleModule.CommandModule("RFABSReduction");    
    delay(GetParameterInt("RFReductionPulse"));
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"RF ABS reduction OK\n");
        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("RFABSReductionEnd");
        moduleStatus = m_vehicleModule.CommandModule("RFABSHold");
        delay(GetParameterInt("ABSHoldDelay"));
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;

        if(testResult == testPass)
        {
        
            m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("RFABSRecoveryStart");
            // Try to start the RF ABS recovery
            moduleStatus = m_vehicleModule.CommandModule("ABSPressureRelease");
            delay(GetParameterInt("RFRecoveryPulse"));

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
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error RF ABS Hold - status: %s\n", 
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::RFABSTest2()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::LRABSTest2(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = LRWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::LRABSTest2()\n");

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("LRABSReductionStart");
    moduleStatus = m_vehicleModule.CommandModule("LRABSReduction");
    delay(GetParameterInt("LRReductionPulse"));
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"LR ABS reduction OK\n");
        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("LRABSReductionEnd");
        moduleStatus = m_vehicleModule.CommandModule("LRABSHold");
        delay(GetParameterInt("ABSHoldDelay"));
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
        
            m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("LRABSRecoveryStart");
            // Try to start the LR ABS recovery
            moduleStatus = m_vehicleModule.CommandModule("ABSPressureRelease");
            delay(GetParameterInt("LRRecoveryPulse"));

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
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error LR ABS Hold - status: %s\n", 
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::LRABSTest2()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::RRABSTest2(void)
{
    string testResult(BEP_TESTING_STATUS);
    string outletResult(BEP_UNTESTED_STATUS);
    string inletResult(BEP_UNTESTED_STATUS);
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    const int wheelIdx = RRWHEEL;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::RRABSTest2()\n");

    m_reduxRecovIndex[wheelIdx].reductionStart = TagArray("RRABSReductionStart");
    moduleStatus = m_vehicleModule.CommandModule("RRABSReduction");    
    delay(GetParameterInt("RRReductionPulse"));
    // Determine the test result
    testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
    if(testResult == testPass)
    {
        outletResult = testPass;
        Log(LOG_DEV_DATA,"RR ABS reduction OK\n");
        m_reduxRecovIndex[wheelIdx].reductionEnd = TagArray("RRABSReductionEnd");
        moduleStatus = m_vehicleModule.CommandModule("RRABSHold");
        delay(GetParameterInt("ABSHoldDelay"));
        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass)
        {
        
            m_reduxRecovIndex[wheelIdx].recoveryStart = TagArray("RRABSRecoveryStart");
            // Try to start the RF ABS recovery
            moduleStatus = m_vehicleModule.CommandModule("ABSPressureRelease");
            delay(GetParameterInt("RRRecoveryPulse"));

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
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error RR ABS Hold - status: %s\n", 
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

    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::RRABSTest2()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::ABSValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::ABSValveFiringTest()\n");
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
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::ABSValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::ABSValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::DynamicABSValveFiringTest(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::DynamicABSValveFiringTest()\n");
    try
    {
        m_absStartIndex = TagArray("ABSStart");

        m_vehicleModule.CommandModule("ABSPressureRelease");
        delay(GetParameterInt("ABSInitialHoldDelay"));

        // run the individual wheel ABS tests
        testResult = LFABSTest2();
        // fixed testResult setting for subsequent tests
        if(testResult == testPass) testResult = RFABSTest2();
        if(testResult == testPass) testResult = LRABSTest2();
        if(testResult == testPass) testResult = RRABSTest2();

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
        Log(LOG_ERRORS, "Module Exception in Apg3500TC::DynamicABSValveFiringTest() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    SendTestResult(testResult, testDescription, testResultCode);

    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::DynamicABSValveFiringTest()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::WaitForControlComplete()
{
    string testResult;
    bool isControlComplete = false;

    // Log the entry
    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::WaitForControlComplete()\n");


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
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::WaitForControlComplete(), isControlComplete:%s status=%s\n", 
        isControlComplete ? "true" : "false", testResult.c_str());
    return(testResult);
}
template <class ModuleType>
string AdvicsABSTC<ModuleType>::DisableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::DisableSpeedLimit()\n");
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
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::DisableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::DisableSpeedLimit()\n");
    return(testResult);
}
template <class ModuleType>
string AdvicsABSTC<ModuleType>::EnableSpeedLimit(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::EnableSpeedLimit()\n");
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
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::EnableSpeedLimit() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::EnableSpeedLimit()\n");
    return(testResult);
}


template <class ModuleType>
string AdvicsABSTC<ModuleType>::DisableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::DisableValveRelayShutdown()\n");
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
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::DisableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::DisableValveRelayShutdown()\n");
    return(testResult);
}
template <class ModuleType>
string AdvicsABSTC<ModuleType>::EnableValveRelayShutdown(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    bool controlInProgress = true;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::EnableValveRelayShutdown()\n");
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
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::EnableValveRelayShutdown() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::EnableValveRelayShutdown()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::CheckRelayState(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::CheckRelayState()\n");
    try
    {
        // Read the Valve Relay State
        moduleStatus = m_vehicleModule.GetInfo("ReadRelayState");

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass) Log(LOG_DEV_DATA,"Relay status acquired\n");
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error acquiring relay status - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Relay Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::CheckRelayState() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::CheckRelayState()\n");
    return(testResult);
}
template <class ModuleType>
string AdvicsABSTC<ModuleType>::CheckPumpState(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::CheckPumpState()\n");
    try
    {
        // Read the Pump Motor State
        moduleStatus = m_vehicleModule.GetInfo("ReadPumpMotorState");

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass) Log(LOG_DEV_DATA,"Pump status acquired\n");
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error acquiring pump status - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Pump Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::CheckPumpState() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::CheckPumpState()\n");
    return(testResult);
}

template <class ModuleType>
string AdvicsABSTC<ModuleType>::UnlockModuleSecurity(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;

    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::UnlockModuleSecurity()\n");
    try
    {
        // Try to disable the speed limit
        moduleStatus = m_vehicleModule.GetInfo("UnlockModuleSecurity");

        // Determine the test result
        testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
        if(testResult == testPass) Log(LOG_DEV_DATA,"Module Security Unlocked\n");
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            SetCommunicationFailure(true);
            Log(LOG_ERRORS, "Error unlocking security - status: %s\n", 
                ConvertStatusToResponse(moduleStatus).c_str());
        }

        Log(LOG_DEV_DATA, "Unlock Module Security Status: %s - status: %s\n", 
            testResult.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in AdvicsABSTC::UnlockModuleSecurity() - %s\n", 
            moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);
    // Return the test result
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::UnlockModuleSecurity()\n");
    return(testResult);
}

//=============================================================================
template <class ModuleType>
string& AdvicsABSTC<ModuleType>::OriginalDriveAxle(const string *driveAxle /*= NULL*/)
{
    if(driveAxle != NULL)  m_originalDriveAxle = *driveAxle;
    return m_originalDriveAxle;
}
//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::CheckPartNumber(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string modulePartNumber;
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    // Check if this step needs to be performed
    Log(LOG_FN_ENTRY, "Enter AdvicsABSTC::CheckPartNumber()\n");
    if(!ShortCircuitTestStep())
    {                       // Do not need to skip
        try
        {                    // Read the part number from the module
            moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModulePartNumber"), modulePartNumber);
            // Check the status of the data
            if(BEP_STATUS_SUCCESS == moduleStatus)
            {
                if(modulePartNumber == GetParameter("ModulePartNumber"))
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
        catch(ModuleException &moduleException)
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
    Log(LOG_FN_ENTRY, "Exit AdvicsABSTC::CheckPartNumber()\n");
    // Return the status
    return(testResult);
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::BatteryVoltageCheck(void)
{
    string testResult = BEP_TESTING_STATUS;
    string testResultCode = "0000";
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    char buf[16];
    float voltage, voltageMin = GetParameterFloat("BatteryVoltageCheckMin"), 
                                voltageMax = GetParameterFloat("BatteryVoltageCheckMax");
    // Log the entry
    Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
    try
    {
        moduleStatus = m_vehicleModule.ReadModuleData("ReadSupplyVoltage", voltage);
        // Determine the test result
        if(BEP_STATUS_SUCCESS == moduleStatus)
        {
            if(voltage >= voltageMin)
            {
                if(voltage <= voltageMax)
                // If the read voltage falls within the specified range...
                {
                    testResult = testPass;
                    Log(LOG_FN_ENTRY, "%s::%s - Success!!!\n", GetComponentName().c_str(), GetTestStepName().c_str());
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("BatteryVoltageCheckHigh");
                    testDescription = GetFaultDescription("BatteryVoltageCheckHigh");
                    Log(LOG_FN_ENTRY, "%s::%s - BatteryVoltageCheckHigh\n", GetComponentName().c_str(), GetTestStepName().c_str());
                }
            }
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("BatteryVoltageCheckLow");
                testDescription = GetFaultDescription("BatteryVoltageCheckLow");
                Log(LOG_FN_ENTRY, "%s::%s - BatteryVoltageCheckLow\n", GetComponentName().c_str(), GetTestStepName().c_str());
            }
        }
        else
        {   // Communication failure reading MDS data from the vehicle
            Log(LOG_ERRORS, "%s::%s - Communication failure reading battery voltage\n",GetComponentName().c_str(), GetTestStepName().c_str());
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
        }
    }
    catch(ModuleException &moduleException)
    {
        Log(LOG_ERRORS, "Module Exception in %s::BatteryVoltageCheck - %s\n", GetTestStepName().c_str(), moduleException.message().c_str());
        testResult = testSoftwareFail;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }
    SendTestResultWithDetail(testResult, testDescription, testResultCode,
                             "VoltageMax", CreateMessage(buf, sizeof(buf), "%3.2f",voltageMax), "",
                             "VoltageMin", CreateMessage(buf, sizeof(buf), "%3.2f",voltageMin), "",
                             "BatteryVoltage", CreateMessage(buf, sizeof(buf), "%3.2f",voltage), "");
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}
//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string AdvicsABSTC<ModuleType>::TestStepWriteVacuumFillingEndFlag(void)
{
    const string   moduleTag("WriteVacuumFillingEndFlag");
    string         moduleData;
    string         faultTag( "NoFault");
    BEP_STATUS_TYPE     moduleStatus;
    string         result( testPass);
    string testResultCode        = "0000";
    string testDescription       = GetTestStepInfo("Description");
    bool  fillFlagValue = false;

    Log( LOG_FN_ENTRY, "Enter AdvicsABSTC::TestStepWriteVacuumFillingEndFlag()\n");

    if(!ShortCircuitTestStep())
	{
        do
        {  
            Log(LOG_DEV_DATA,"Writing the Vacuum fill end flag");
            moduleStatus = m_vehicleModule.GetInfo(moduleTag);
	    	if(moduleStatus == BEP_STATUS_SUCCESS)
            {
                Log(LOG_DEV_DATA,"Reading the Vacuum fill end flag");
                moduleStatus = m_vehicleModule.GetInfo("ReadVacuumFillingEndFlag",fillFlagValue);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    result = fillFlagValue ? testPass : testFail;
                }
                else
                {
                    SetCommunicationFailure(true);
                    result = testFail;
                    testResultCode = GetFaultCode("CommunicationFailure");
                    testDescription = GetFaultDescription("CommunicationFailure");
                    Log(LOG_ERRORS, "Error reading vacuum fill end flag - status: %s\n", 
                        ConvertStatusToResponse(moduleStatus).c_str());
                }
            }
            else
            {
                SetCommunicationFailure(true);
                result = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error writing vacuum fill end flag - status: %s\n", 
                    ConvertStatusToResponse(moduleStatus).c_str());
            }
        }while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && !fillFlagValue);
        if (StatusCheck() != BEP_STATUS_SUCCESS) 
        {
            result = ConvertStatusToResponse(StatusCheck());
        }
        else if (!TimeRemaining())
        {
            Log(LOG_DEV_DATA, "Timeout trying to write Vacuum filling end flag");
            result = BEP_TIMEOUT_RESPONSE;          
        }
        else
        {
            result = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
            testResultCode = (result == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
            testDescription = (result == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
        }
        // Send the test result
        SendTestResult(result, testDescription, testResultCode);
    }
    else
    {
        Log(LOG_DEV_DATA, "Skipping writing the Vacuum Filling End Flag");
        result = testSkip;
    }
    Log(LOG_FN_ENTRY, "AdvicsABSTC::TestStepWriteVacuumFillingEndFlag() - Exit");
    return result;
}



