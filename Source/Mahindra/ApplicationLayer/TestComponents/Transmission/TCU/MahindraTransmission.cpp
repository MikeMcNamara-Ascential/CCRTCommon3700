//******************************************************************************
// Description:
//  RWAL Brake test component class
//
//==============================================================================
// Copyright (c) 2012 - Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
#include "MahindraTransmission.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MahindraTransmission<ModuleType>::MahindraTransmission() : GenericTransmissionTCTemplate<ModuleType>() {   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MahindraTransmission<ModuleType>::~MahindraTransmission() {
}

template<class ModuleType>
void MahindraTransmission<ModuleType>::InitializeHook(const XmlNode *config) {
    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::InitializeHook()\n");
    try
    {
        XmlParser parser, parser2, parser3;

        //Initialize the tcu module object
        try
        {
            const XmlNode *m_tcuConfig = parser.ReturnXMLDocument(GetRootDir() + GetParameter("TcuConfigFile"));
            // Initialize the tmu module object
            m_tcuModule.Initialize(m_tcuConfig);

        } catch (BepException &err)
        {
            Log(LOG_ERRORS, "BepException during MahindraTransmission():InitializeHook TCU Module - %s\n", err.GetReason());
        }

        //Initialize the cruise module object
        try
        {
            const XmlNode *m_cruiseConfig = parser2.ReturnXMLDocument(GetRootDir() + GetParameter("CruiseConfigFile"));
            // Initialize the cruise module object
            m_cruiseModule.Initialize(m_cruiseConfig);

        } catch (BepException &err)
        {
            Log(LOG_ERRORS, "BepException during MahindraTransmission():InitializeHook Cruise Module - %s\n", err.GetReason());
        }

        //Initialize the tpms module object
        try
        {
            const XmlNode *m_tpmsConfig = parser3.ReturnXMLDocument(GetRootDir() + GetParameter("TpmsConfigFile"));
            // Initialize the tpms module object
            m_tpmsModule.Initialize(m_tpmsConfig);

        } catch (BepException &err)
        {
            Log(LOG_ERRORS, "BepException during MahindraTransmission():InitializeHook TPMS Module - %s\n", err.GetReason());
        }

    } catch (BepException &err)
    {
        Log(LOG_ERRORS, "Exception during initialization: %s\n", err.GetReason());
        //throw;
    } catch (XmlException &ex)
    {
        Log(LOG_ERRORS, "xml Exception during initialization: %s\n", ex.GetReason());

    } catch (...)
    {

        Log(LOG_ERRORS, "unknown Exception during initialization\n");
    }

    GenericTransmissionTCTemplate<ModuleType>::InitializeHook(config);

    Log(LOG_FN_ENTRY, "Exit MahindraTransmission::InitializeHook()\n");
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const string MahindraTransmission<ModuleType>::CommandTestStep(const string &value) {
    string result(BEP_TESTING_STATUS);
    Log(LOG_FN_ENTRY, "MahindraTransmission::CommandTestStep(value: %s) - Enter", value.c_str());
    try
    {   // Make sure the system status is ok before trying to run this test step
        if (BEP_STATUS_SUCCESS != StatusCheck())
        {   // System status is not ok, do not perform the test step
            result = testSkip;
            SendTestResult(result, GetTestStepInfo("Description"));
            Log(LOG_ERRORS, "MahindraTransmission::CommandTestStep() - Status Check failed (%s), not performing %s",
                ConvertStatusToResponse(StatusCheck()).c_str(), GetTestStepName().c_str());
        }

        else if (!GetTestStepName().compare("CheckTpmsStatus"))
            result = CheckTpmsStatus();

        else if (!GetTestStepName().compare("CruiseTest"))
            result = CruiseTest();

        else if (!GetTestStepName().compare("ATLearnTest"))
            result = ATLearnTest();

        //Run the base class function
        else
            result = GenericTransmissionTCTemplate<ModuleType>::CommandTestStep(value);

    } catch (ModuleException &excpt)
    {
        Log(LOG_ERRORS, "ModuleException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        result = BEP_SOFTWAREFAIL_RESPONSE;
    } catch (BepException &excpt)
    {
        Log(LOG_ERRORS, "BepException in %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        result = BEP_SOFTWAREFAIL_RESPONSE;
    }
    Log(LOG_FN_ENTRY, "MahindraTransmission::CommandTestStep(value: %s) - Exit", value.c_str());
    return result;
}


//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string MahindraTransmission<ModuleType>::CheckTpmsStatus(void) {

    string testResult = testFail;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    string moduleStatus = BEP_TESTING_STATUS;
    bool tpmsStatus = false;

    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::CheckTpmsStatus()\n");

    //Have the vehicle reach the target speed
    string speedCheck = AccelerateToTestSpeed(GetParameterFloat("TpmsTestSpeed"),
                                              GetParameter("TpmsTestSpeedRange"),
                                              GetParameterInt("TpmsTestTargetSpeedScanDelay"), false);

    //Once vehicle has reached speed, maintain it for 1 minute
    if (speedCheck == testPass)
    {
        string maintainCheck = MaintainSpeedForTime(GetParameterInt("TpmsTimeout"), GetParameterInt("TpmsTestSpeed"));

        //If speed was maintained for 1 min
        if (testPass == maintainCheck)
        {
            //Check the TPMS status
            moduleStatus = m_tpmsModule.ReadModuleData("CheckTpmsStatus", tpmsStatus);

            if (moduleStatus == testPass)
            {
                if (tpmsStatus)
                {
                    Log(LOG_DEV_DATA, "Tpms status passed.");
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_DEV_DATA, "Tpms status failed.");
                    testResult = testFail;
                }
            }
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error reading TPMS status \n");
            }
        }
    }

    //Fill it out later

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());

    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string MahindraTransmission<ModuleType>::CruiseTest(void) {

    string testResult = testFail;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
    string cruiseStatus = "";

    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::CruiseTest()\n");

    //Have the vehicle reach the target speed
    string speedCheck = AccelerateToTestSpeed(GetParameterFloat("CruiseControlTestSpeed"),
                                              GetParameter("CruiseControlTestSpeedRange"),
                                              GetParameterInt("CruiseControlSpeedScanDelay"), false);

    //Takes place after AT learn test so we should be slowing down to speed
    string speedCheck2 = SlowDownToTestSpeed(GetParameterFloat("CruiseControlTestSpeed"),
                                             GetParameter("CruiseControlTestSpeedRange"),
                                             GetParameterInt("CruiseControlSpeedScanDelay"), false);

    //Once vehicle has reached speed, maintain it for 1 minute
    if (speedCheck == testPass)
    {
        Log(LOG_DEV_DATA, "Speed has been reached. Performing Cruise Control Test");

        //Enter Diagnostic mode prior to reading cruise status
        status = m_cruiseModule.CommandModule("EnterDiagnosticMode");

        //Prompt driver to press cruise button
        DisplayPrompt(GetPromptBox("CruiseEngage"), GetPrompt("CruiseEngage"), GetPromptPriority("CruiseEngage"));

        //Set the timeout
        INT32 preconditionTimeout = GetTestStepInfoInt("PreconditionTimeout") > 0 ? GetTestStepInfoInt("PreconditionTimeout") : 10000;
        SetSecondaryStartTime();

        do
        {
            status = m_cruiseModule.ReadModuleData("CruiseStatus", cruiseStatus);
            //Give time before reading cruise status
            BposSleep(500);

        } while ((SecondaryTimeRemaining(&preconditionTimeout)) && (cruiseStatus != "Engaged") && (BEP_STATUS_SUCCESS == StatusCheck()));

        if (cruiseStatus == "Engaged")
        {
            Log(LOG_DEV_DATA, "Cruise has been engaged.. pass whole test for now.");
            testResult = testPass;
        }
        else
        {
            testResult = testFail;
            testResultCode = GetFaultCode("CommunicationFailure");
            testDescription = GetFaultDescription("CommunicationFailure");
            Log(LOG_ERRORS, "Failed to press cruise control button \n");

        }
    }

    //Remove cruise prompts
    RemovePrompt(GetPromptBox("CruiseEngage"), GetPrompt("CruiseEngage"), GetPromptPriority("CruiseEngage"));

    //Brake to stop.. Brake test should be after cruise test so parameterize this
    CheckZeroSpeed();

    //Fill it out later

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string MahindraTransmission<ModuleType>::ATLearnTest(void) {

    string testResult = testFail;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    BEP_STATUS_TYPE moduleStatus2 = BEP_STATUS_ERROR;

    //Speed & Throttle pedal posision range
    float AtLearnTargetSpeed = GetParameterFloat("ATLearnTargetSpeed");
    int minPedalPos = GetParameterInt("ATMinAccelPos");
    int maxPedalPos = GetParameterInt("ATMaxAccelPos");
    int newMinPedalPos;
    int newMaxPedalPos;
    char buff[16];
    string ATThrottleRange = CreateMessage(buff, sizeof(buff), "%d %d",
                                           (INT32)minPedalPos, (INT32)maxPedalPos);

    //AT learn item variables
    int ATCycleTimout = GetParameterInt("ATCycleTimout");
    int targetGear = GetParameterInt("ATLearnTargetGear");
    bool inGreenBand = false;
    bool beyondGreenBand = false;
    string converterStatus = "", gearResponse;
    float accelPedalPos;  //in %
    int currentGear, nextGear;
    int prevGear = 1;
    bool promptDisplayed = false;


    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::ATLearnTest()\n");

    //Have operator shift to Drive first

    //Start AT learn test
    SystemWrite(GetDataTag("TPSTarget"), ATThrottleRange);
    SystemWrite(GetDataTag("TPSActive"), true);
    DisplayPrompt(GetPromptBox("AcceleratePedalInGreen"), GetPrompt("AcceleratePedalInGreen"), GetPromptPriority("AcceleratePedalInGreen"));

    try
    {
        do
        {   // Wait for throttle in target speed range until first shift request
            moduleStatus = m_vehicleModule.MonitorATLearnData(accelPedalPos, converterStatus, gearResponse);
            if (moduleStatus == BEP_STATUS_SUCCESS)
            {
                //Get the current gear of the vehicle
                currentGear = atoi(gearResponse.c_str());

                //Update GUI with throttle position
                SystemWrite(GetDataTag("TPSValue"), (int)accelPedalPos);

                //Check if throttle gauge needs to be updated
                if (inGreenBand == false && nextGear != prevGear)
                {
                    if (accelPedalPos > minPedalPos && accelPedalPos < maxPedalPos)
                    {
                        int tolerence = GetParameterInt("ATLearnAccelTolerence");
                        newMinPedalPos = (int)accelPedalPos - tolerence;
                        newMaxPedalPos = (int)accelPedalPos + tolerence;
                        ATThrottleRange = CreateMessage(buff, sizeof(buff), "%d %d",
                                                        (INT32)newMinPedalPos, (INT32)newMaxPedalPos);
                        SystemWrite(GetDataTag("TPSTarget"), ATThrottleRange);
                        inGreenBand = true;
                        Log(LOG_DEV_DATA, "Pedal In Initial Range, nextGear %f", nextGear);
                    }
                    else
                    {
                        Log(LOG_DEV_DATA, "Pedal NOT In Initial Range, nextGear %f", nextGear);
                    }
                }
                // Display prompt for pedal out of range
                if ((accelPedalPos < newMinPedalPos || accelPedalPos > newMaxPedalPos) && !promptDisplayed && currentGear > 1)
                {
                    DisplayPrompt(GetPromptBox("AccelNotInRange"), GetPrompt("AccelNotInRange"), GetPromptPriority("AccelNotInRange"));
                    promptDisplayed = true;
                    Log(LOG_DEV_DATA, "Pedal not in Range, promptDisplayed = true");
                }
                // Remove propmpt when pedal is back in range
                if (accelPedalPos > newMinPedalPos && accelPedalPos < newMaxPedalPos)
                {
                    RemovePrompt(GetPromptBox("AccelNotInRange"), GetPrompt("AccelNotInRange"), GetPromptPriority("AccelNotInRange"));
                    promptDisplayed = false;
                    Log(LOG_DEV_DATA, "Pedal in Range, promptDisplayed = false");
                }

                //Check if the vehicle changed gears
                if (prevGear < currentGear)
                {
                    Log(LOG_DEV_DATA, "Gear transition spotted, prevGear %d - currentGear %d", prevGear, currentGear);
                    prevGear = currentGear;

                    //IF the vehicle is in Gear 3 or above then check clutch status
                    if (prevGear >= 3)
                    {
                        //moduleStatus2 = m_vehicleModule.ReadModuleData("ConverterStatus", converterStatus);
                        Log(LOG_DEV_DATA, "Checking Clutch Coverter status");
                        if (converterStatus == "Locked")
                        {
                            Log(LOG_DEV_DATA, "Converter is locked");
                        }
                        else Log(LOG_DEV_DATA, "Clutch Coverter status: $s", converterStatus.c_str());
                    }
                }

            }
            else
            {
                // Module communication failure.
                DisplayPrompt(GetPromptBox("ModuleCommFail"), GetPrompt("ModuleCommFail"), GetPromptPriority("ModuleCommFail"));
                Log(LOG_ERRORS, "Module Communicaiton failure");
            }

            BposSleep(GetTestStepInfoInt("ScanDelay"));
        } while (moduleStatus == BEP_STATUS_SUCCESS && (currentGear < targetGear) && TimeRemaining(&ATCycleTimout) && StatusCheck() == BEP_STATUS_SUCCESS &&
                 (GetRollSpeed() < AtLearnTargetSpeed));


        BposSleep(1000); // Kill 1 second after 3-4 shift to allow learn time

        if (moduleStatus != BEP_STATUS_SUCCESS)
        {
            testResult = testFail;
            Log(LOG_DEV_DATA, "Module Comm Failure");
        }
        else if (!TimeRemaining(&ATCycleTimout))
        {
            DisplayPrompt(GetPromptBox("TimeoutOnTargetGear"), GetPrompt("TimeoutOnTargetGear"), GetPromptPriority("TimeoutOnTargetGear"));
            testResult = testTimeout;
            testDescription = "Timeout waiting for target gear";
            Log(LOG_DEV_DATA, "LTIExit TimeoutOnTargetGear");
        }
        else if (StatusCheck() != BEP_STATUS_SUCCESS)
        {
            Log(LOG_DEV_DATA, "AT Learn Bad StatusCheck");
            if (StatusCheck() == BEP_STATUS_ABORT)
            {
                testResult = testAbort;
                testDescription = "Test Aborted by Operator";
            }
            else
            {
                testResult = testFail;
                testDescription = "Bad machine status";
            }
        }
        else if (currentGear >= targetGear)
        {
            Log(LOG_DEV_DATA, "Vehicle successfully went through gear sequence");
            testResult = testPass;
        }
        else
        {
            Log(LOG_DEV_DATA, "AT Learn Exit IDFK");
        }
    } catch (ModuleException &e)
    {   // Module exception
        Log(LOG_ERRORS, "Module Exception in %s - %s\n", GetTestStepName().c_str(), e.message().c_str());
        testResult = testAbort;
        testResultCode = GetFaultCode("SoftwareFailure");
        testDescription = GetFaultDescription("SoftwareFailure");
    }

    // Turn off throttle gauge for brake to stop.
    SystemWrite(GetDataTag("TPSTarget"), string("0 0"));
    SystemWrite(GetDataTag("TPSActive"), false);

    //Remove prompts
    RemovePrompts();
    DisplayPrompt(2, "Blank", 0);

    //Fill it out later

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
string MahindraTransmission<ModuleType>::MisfireTest(void) {

    string testResult = testFail;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_TESTING_STATUS;
    bool misfireStatus = false;

    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::MisfireTest()\n");

    //Have the vehicle reach the target speed
    string speedCheck = AccelerateToTestSpeed(GetParameterFloat("MisfireTestSpeed"),
                                              GetParameter("MisfireTestSpeedRange"),
                                              GetParameterInt("MisfireSpeedScanDelay"), false);


    //Once vehicle has reached speed, maintain it for 1 minute
    if (speedCheck == testPass)
    {
        //have vehicle decelerate to 40 kph ( ~25 mph)
        //string maintainCheck = MaintainSpeedForTime(GetParameterInt("TpmsTimeout"), GetParameterInt("TpmsTestSpeed"));

        //If decel speed was reached
        if (testPass /*== maintainCheck */)
        {
            //Check the Misfire results status
            moduleStatus = m_cruiseModule.ReadModuleData("CheckMisfireStatus", misfireStatus);

            if (moduleStatus == BEP_STATUS_SUCCESS)
            {
                if (misfireStatus)
                {
                    Log(LOG_DEV_DATA, "Misfire status passed.");
                    testResult = testPass;
                }
                else
                {
                    Log(LOG_DEV_DATA, "Misfire status failed.");
                    testResult = testFail;
                }
            }
            else
            {
                testResult = testFail;
                testResultCode = GetFaultCode("CommunicationFailure");
                testDescription = GetFaultDescription("CommunicationFailure");
                Log(LOG_ERRORS, "Error reading Misfire status \n");
            }
        }
    }

    //Fill it out later

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
    return testResult;
}
