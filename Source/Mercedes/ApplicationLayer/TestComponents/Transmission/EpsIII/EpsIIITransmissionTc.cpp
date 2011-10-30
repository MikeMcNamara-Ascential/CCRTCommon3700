//*************************************************************************
// FILE DESCRIPTION:
//  Test Component for the Mercedes EPS-III Transmission.
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
#include "EpsIIITransmissionTc.h"

//-------------------------------------------------------------------------------------------------
EpsIIITransmissionTc::EpsIIITransmissionTc() : GenericTC(), m_isEpsTransEquipped(true)
{   // Nothing special to do
}

//-------------------------------------------------------------------------------------------------
EpsIIITransmissionTc::~EpsIIITransmissionTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string EpsIIITransmissionTc::CommandTestStep(const string &value)
{   // Log the entry and determine if system is OK to continue testing
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::CommandTestStep(%s) - Enter", value.c_str());
    string result(BEP_TESTING_RESPONSE);
    try
    {
        if(BEP_STATUS_SUCCESS == StatusCheck())
        {
            if(!GetTestStepName().compare("CheckShifting"))                 result = CheckShifting();
            else if(!GetTestStepName().compare("CheckTransmissionType"))
            {
                CheckIfEpsTransmissionEquipped();
                result = testPass;
            }
            else if(!GetTestStepName().compare("FinishUp"))
            {
                result = testPass;
                SendOverallResult(result);
            }
            else if(!GetTestStepName().compare("SelectTransmissionMode"))   result = SelectMode(value);
            else if(!GetTestStepName().compare("SpeedBasedGearTest"))       result = SpeedBasedGearTest(value);
			else if(!GetTestStepName().compare("MaximumGearTest"))          result = MaximumGearTest();
			else if(!GetTestStepName().compare("AccelerateToTestSpeed") && EpsTransmissionEquipped())
			{
				result = AccelerateToTestSpeed(GetParameterFloat("AutoModeTargetSpeed"), GetParameter("AutoModeSpeedRange"),
											   GetTestStepInfoInt("ScanDelay"), true, "AccelerateToAutoModeSpeed");
			}
            else                                                            result = GenericTC::CommandTestStep(value);
        }
        else
        {   // Cannot perform the test step
            Log(LOG_ERRORS, "Cannot continue testing due to system status: %s", ConvertStatusToResponse(StatusCheck()).c_str());
            result = testSkip;
        }
    }
    catch(XmlException &excpt)
    {
        Log(LOG_ERRORS, "EpsIIITransmissionTc - XmlException during %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        result = testSoftwareFail;
    }
    catch(BepException &excpt)
    {
        Log(LOG_ERRORS, "EpsIIITransmissionTc - BepException during %s - %s", GetTestStepName().c_str(), excpt.GetReason());
        result = testSoftwareFail;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::CommandTestStep(%s) - Exit", value.c_str());
    return result;
}

//-------------------------------------------------------------------------------------------------
void EpsIIITransmissionTc::Initialize(const XmlNode *config)
{   // Call the base clas to begin the initialization and then log the entry to our section
    GenericTC::Initialize(config);
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::InitializeHook() - Enter");
    // Reset system tags
    ResetSystemTags();
	// Store the list of valid EPS-III transmission types
	try
	{
		m_epsTransTypes.DeepCopy(config->getChild("Setup/Parameters/EpsIIITransmissionTypes")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "List of EPS-III not specified!  EPS-III transmission will not be tested!! (%s)", excpt.GetReason());
	}
    // Set Initialization Complete and log the exit
    InitializationComplete();
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::InitializeHook() - Exit");
}

//-------------------------------------------------------------------------------------------------
string EpsIIITransmissionTc::CheckShifting(void)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::CheckShifting() - Enter");
    string result(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep() && !EpsTransmissionEquipped())
    {   // Prompt the operator to enter the result
        result = OperatorPassFail(GetDataTag("CheckShiftingPrompt"), GetTestStepInfoInt("Timeout"));
        SendSubtestResult("ReverseGearObserved", result, "Reverse Gear Observed", "0000");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping CheckShifting");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::CheckShifting() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
string EpsIIITransmissionTc::MaximumGearTest(void)
{   // Log the entry and deterine if this test should be performed
	Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::MaximumGearTest() - Enter");
	string result(BEP_TESTING_RESPONSE);
	if(!ShortCircuitTestStep() && EpsTransmissionEquipped())
	{   // Prompt the operator and then wait for maximum gear
		DisplayPrompt(GetPromptBox("ShiftThroughGears"), GetPrompt("ShiftThroughGears"), GetPromptPriority("ShiftThroughGears"));
		float targetSpeed = (SystemReadFloat("MaxSpeed") * 0.621371192) - GetParameterFloat("MaxSpeedTolerance");
		char buff[32];
		string speedRange = CreateMessage(buff, sizeof(buff), "%.2f %.2f", targetSpeed, targetSpeed + (GetParameterFloat("MaxSpeedTolerance") * 2));
		result = AccelerateToTestSpeed(targetSpeed, speedRange, GetTestStepInfoInt("ScanDelay"), true);
		RemovePrompt(GetPromptBox("ShiftThroughGears"), GetPrompt("ShiftThroughGears"), GetPromptPriority("ShiftThroughGears"));
	}
	else
	{   // Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping MaximumGearTest()");
		result = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::MaximumGearTest() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
string EpsIIITransmissionTc::SelectMode(const string &mode)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::SelectMode(mode: %s) - Enter", mode.c_str());
    string result(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep() && EpsTransmissionEquipped())
    {   // Prompt the operator to shift to manual mode
        DisplayTimedPrompt(GetPrompt("ShiftToMode"), GetPromptBox("ShiftToMode"), 
                           GetPromptPriority("ShiftToMode"), GetPromptDuration("ShiftToMode"), mode);
		// Check if we need to prompt the operator about the ASR switch
		string asrEquipped("No");
		// Determine how to handle the wheel boosting
        if(!mode.compare("Auto"))
        {
			EnableMotorLoading(mode);
			asrEquipped = OperatorYesNo("AsrEquipped", GetTestStepInfoInt("Timeout"));
			SystemWrite(string("BoostRearAndTandemOnly"), asrEquipped.compare("No"));
			if(!asrEquipped.compare("No"))
			{   // Limit speed to 60 km/h
				string orgSpeed = SystemRead("MaxSpeed");
				OriginalMaximumSpeed(&orgSpeed);
				SystemWrite(string("MaxSpeed"), GetParameter("NonAsrMaximumSpeed"));
				Log(LOG_DEV_DATA, "Transmission not equipped with ASR button, changing Max Speed to %s km/h",
					GetParameter("NonAsrMaximumSpeed").c_str());
			}
        }
        else
        {
            DisableMotorLoading(mode);
			SystemWrite(string("MaxSpeed"), OriginalMaximumSpeed());
            SystemWrite("BoostRearAndTandemOnly", true);
        }
        result = testPass;
        // Report the result
        SendTestResultWithDetail(result, GetTestStepInfo("Description"), "0000", "Mode", mode, "");
    }
    else
    {   // Need to skip this test step
        Log(LOG_FN_ENTRY, "Skipping SelectMode()");
        result = testSkip;
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::SelectMode() - Exit");
    return result;
}

//-------------------------------------------------------------------------------------------------
string EpsIIITransmissionTc::SpeedBasedGearTest(const string &gear)
{   // Log the entry and determine if this step should be performed
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::SpeedBasedGearTest(gear: %s) - Enter", gear.c_str());
    string result(BEP_TESTING_RESPONSE);
    if(!ShortCircuitTestStep() && EpsTransmissionEquipped())
    {   // Make sure this is a valid gear
        INT32 maxGear = GetVehicleParameter("Transmission/NumberOfFwdGears", int(0));
        if(BposReadInt(gear.c_str()) <= maxGear)
        {   // Display the target speed for the operator and instruct the operator to speed
            string minSpeedTarget(GetVehicleParameter("Transmission/MinSpeedTarget_" + gear, string("")));
            string maxSpeedTarget(GetVehicleParameter("Transmission/MaxSpeedTarget_" + gear, string("")));
            string speedRange(minSpeedTarget + " " + maxSpeedTarget);
            DisplayPrompt(GetPromptBox("ShiftToGear"), GetPrompt("ShiftToGear"), GetPromptPriority("ShiftToGear"),
                          "white", gear);
            // Wait for the operator to accelerate to speed
            Log(LOG_DEV_DATA, "Prompting the operator to min speed: %s, with range: %s", minSpeedTarget.c_str(), speedRange.c_str());
            result = AccelerateToTestSpeed(atof(minSpeedTarget.c_str()), speedRange, GetTestStepInfoInt("ScanDelay"), false);
            if(!result.compare(testPass))
            {   // Wait a few seconds for further speed increase
                StatusSleep(GetParameterInt("MaxSpeedDelay"));
            }
            // Analyze the final result
            float topSpeed = GetRollSpeed();
            result = ((atof(minSpeedTarget.c_str()) <= topSpeed) && 
                      (topSpeed <= atof(maxSpeedTarget.c_str()))) ? testPass : testFail;
            // Report the result of the verification
            char buff[32];
            SendSubtestResultWithDetail("Gear_" + gear + "_Verification", result, GetTestStepInfo("Description"), "0000",
                                        "TopSpeed", CreateMessage(buff, sizeof(buff), "%.2f", topSpeed), unitsMPH,
                                        "MinSpeed", minSpeedTarget, unitsMPH,
                                        "MaxSpeed", maxSpeedTarget, unitsMPH);
        }
        else
        {   // Transmission does not have the specified gear
            Log(LOG_DEV_DATA, "Transmission does not have the specified gear: %s,  number of gears: %d", 
                gear.c_str(), maxGear);
            result = testSkip;
            SendSubtestResult(result, GetTestStepInfo("Description"), "0000");
        }
    }
    else
    {   // Need to skip this test.
        result = testSkip;
        Log(LOG_FN_ENTRY, "Skipping SpeedBasedGearTest(gear: %s)", gear.c_str());
    }
    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "EpsIIITransmissionTc::SpeedBasedGearTest(gear: %s) - Exit", gear.c_str());
    return result;
}

//-------------------------------------------------------------------------------------------------
void EpsIIITransmissionTc::CheckIfEpsTransmissionEquipped(void)
{
    string transType(SystemRead(TRANSFER_CASE_TAG));   // NOTE: T-case type was always 00, so DCBR decided to re-use
	bool equipped = false;
#if 0
	equipped = !transType.compare(GetDataTag("EpsIIITransmissionValue"));
    Log(LOG_DEV_DATA, "Transmission Type: %s  [EPS-III value: %s]", 
        transType.c_str(), GetDataTag("EpsIIITransmissionValue").c_str());
#else
	for(XmlNodeMapItr iter = m_epsTransTypes.begin(); (iter != m_epsTransTypes.end()) && !equipped; iter++)
	{
		equipped = !transType.compare(iter->second->getValue());
	}
	Log(LOG_DEV_DATA, "Transmission Type: %s - EPS-III type: %s", transType.c_str(), equipped ? "True" : "False");
#endif 
    // Store the flag indicating if this is an EPS-III transmission
    EpsTransmissionEquipped(&equipped);
}

//-------------------------------------------------------------------------------------------------
inline const bool& EpsIIITransmissionTc::EpsTransmissionEquipped(const bool *equipped /*= NULL*/)
{
    if(equipped != NULL)  m_isEpsTransEquipped = *equipped;
    return m_isEpsTransEquipped;
}

//-------------------------------------------------------------------------------------------------
const string& EpsIIITransmissionTc::OriginalMaximumSpeed(const string *orgMaxSpeed /*= NULL*/)
{
	if(orgMaxSpeed != NULL)  m_originalSpeed = *orgMaxSpeed;
	return m_originalSpeed;
}
