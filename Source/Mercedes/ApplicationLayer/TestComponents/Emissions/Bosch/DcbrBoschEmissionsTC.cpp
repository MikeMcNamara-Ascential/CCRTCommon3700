//*************************************************************************
// FILE DESCRIPTION:
//  DCBR Bosch Emissions Test Component.
//
//===========================================================================
// Copyright (c) 2005- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Emissions/DcbrBosch/DcbrBoschEmissionsTC.cpp $
// 
// 
//*************************************************************************

#include "DcbrBoschEmissionsTC.h"

template <class VehicleModuleType>
DcbrBoschEmissionsTC<VehicleModuleType>::DcbrBoschEmissionsTC() : GenericEmissionsTCTemplate<VehicleModuleType>()
{	// Nothing special to do
}

template <class VehicleModuleType>
DcbrBoschEmissionsTC<VehicleModuleType>::~DcbrBoschEmissionsTC()  
{	// Nothing special to do
}

template <class VehicleModuleType>
const string DcbrBoschEmissionsTC<VehicleModuleType>::CommandTestStep(const string &value)
{
	string testResult = testFail;
	// Determine if the next test step can be sequenced
	if (StatusCheck() == BEP_STATUS_SUCCESS)
	{	// System is fine to continue testing
		try
		{	// Get the next test step to perform
			Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
			// Wake up the module
			if (GetTestStepName() == "ModuleWakeUp")					testResult = ModuleWakeUp();
			// Add loading and verify vehicle operation at speed
			else if (GetTestStepName() == "PerformEngineTorqueTest")	testResult = EngineTorqueTest();
			// Request Operator Input
			else if (GetTestStepName() == "CheckEngineTorquePerformace")	testResult = CheckEngineTorquePerformace();
			// Perform max speed test
			else if (GetTestStepName() == "PerformMaxSpeedTest")		testResult = MaxSpeedTest();
			// No special method, use the base class
			else   testResult = GenericEmissionsTCTemplate<VehicleModuleType>::CommandTestStep(value);
		}
		catch (BepException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
			testResult = BEP_ERROR_RESPONSE;
		}
	}
	else
	{	// Status of the system is not right to continue testing
		Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
	}
	// Return the result of the test
	Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
	return testResult;
}

template <class VehicleModuleType>
void DcbrBoschEmissionsTC<VehicleModuleType>::InitializeHook(const XmlNode *config)
{	// Call the base class InitializeHook
	GenericEmissionsTCTemplate<VehicleModuleType>::InitializeHook(config);  
	// Store the comm bus
	string commBus("CAN");
	try
	{
		commBus = config->getChild("Setup/CommunicationBus")->getValue();
		WhichCommBus(&commBus);
	}
	catch (XmlException &excpt)
	{
		commBus = "CAN";
		Log(LOG_ERRORS, "Comm Bus Type not Found, default to CAN: %s", excpt.GetReason());
		WhichCommBus(&commBus);
	}
	// Store normal t-case torque types
	try
	{
		m_normalTorqueTypes.DeepCopy(config->getChild("Setup/NormalTorqueTypes")->getChildren());
	}
	catch (XmlException &excpt)
	{	// Error loading sensor register table
		Log(LOG_ERRORS, "XmlException loading normal torque types data: %s\n", excpt.GetReason());
		m_normalTorqueTypes.clear(true);
	}
	// Store max speed test enabled types
	try
	{
		m_maxSpeedTestEnabledTypes.DeepCopy(config->getChild("Setup/MaxSpeedTestEnabledTypes")->getChildren());
	}
	catch (XmlException &excpt)
	{	// Error loading sensor register table
		Log(LOG_ERRORS, "XmlException loading max speed test enabled types data: %s\n", excpt.GetReason());
		m_maxSpeedTestEnabledTypes.clear(true);
	}
}

template <class VehicleModuleType>
string DcbrBoschEmissionsTC<VehicleModuleType>::ModuleWakeUp(void)
{	// Establish comms with the module
	return BEP_STATUS_SUCCESS == m_vehicleModule.SendModuleWakeUpMessage() ? testPass : testFail;
}

template <class VehicleModuleType>
inline const string DcbrBoschEmissionsTC<VehicleModuleType>::CreateTestResultName(const std::string name = "")
{
	string resultName = GenericTC::CreateTestResultName(name);
	// Only add prefix if parameter is set and this is not the overall result
	if (GetParameterBool("AddPrefixToTestResultName") && (resultName.find_first_of("OverallTest") == resultName.npos))
	{
		resultName = GetParameter("TestResultPrefix");
	}
	return resultName;
}

template <class VehicleModuleType>
inline const string& DcbrBoschEmissionsTC<VehicleModuleType>::WhichCommBus(const string *commBus) /*= NULL*/
{
	// Store the comm bus if necessary
	if (commBus != NULL)  m_commBus = *commBus;
	// Return the comm bus type
	return m_commBus;
}

template <class VehicleModuleType>
string DcbrBoschEmissionsTC<VehicleModuleType>::EngineTorqueTest(void)
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	INT32 scanDelay = 0;
	string setMotor("Enabled");
	float torqueConstant = 0.0;
	//float accelConstant = 0.0;
	//float speedConstant = 0.0;
	float torqueScalingValue = 0;
	float enginePowerValue = 0;
	float percentIncrease = GetParameterFloat("PercentIncrease");
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep() && ShouldPerformEngineTorqueTest())
	{	// Read the Engine Torque Scaling factor provided in the config file, and cut it in half for 4x4 and 6x4 trucks. 
		SystemWrite(GetDataTag("TorqueConstant"), (float)0.0);
		SystemWrite(GetDataTag("TorqueConstantEnable"), true);
		torqueScalingValue = GetParameterFloat("EngineTorqueScalingFactor") / 2;
		for (XmlNodeMapItr iter = m_normalTorqueTypes.begin(); iter != m_normalTorqueTypes.end(); iter++)
		{
			Log(LOG_DEV_DATA, "Config Output: %s; Subscription Output: %s\n", iter->second->getValue().c_str(), SystemRead("Traction").c_str());
			if (iter->second->getValue() == SystemRead("Traction"))
			{
				torqueScalingValue *= 2;
				Log(LOG_DEV_DATA, "This is a 4x2 or 6x2 truck.  Do not divide the torque.\n");
			}
		}
		// Read the Engine Horse Power value provided by broadcast
		enginePowerValue = atof(SystemRead(GetDataTag("EngineHP")).c_str());
		torqueConstant = torqueScalingValue * enginePowerValue;
		Log(LOG_DEV_DATA, "torqueScalingValue = %f, enginePowerValue = %f, torqueConstant = %f", torqueScalingValue, enginePowerValue, torqueConstant);
		DisplayPrompt(GetPromptBox("EngineTorqueTest"), GetPrompt("EngineTorqueTest"), GetPromptPriority("EngineTorqueTest"));
		//Save delay time between module reads
		scanDelay = GetTestStepInfoInt("ScanDelay");
		// Prompt the operator to speed
		testResult = WaitAndPromptForSpeed(GetParameterFloat("EngineTorqueMinSpeed"), 
										   GetParameterFloat("EngineTorqueMaxSpeed"),
										   scanDelay);

        if (BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status
            testResult = testAbort;
            testResultCode = GetFaultCode("SystemStatus");
            testDescription = GetFaultDescription("SystemStatus");
            Log(LOG_ERRORS, "Bad system status, aborting -- status: %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else if (testResult == testPass)
		{	//hold steady speed
			DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"));
			//SLOWLY enable and increase motor loading
			// hardcoded percentage, since this is very unlikely to change 
			// (0.05 = 5% increase per second, it will take 20 seconds to get to 100% load)
			for (int iterationCount = 0; (iterationCount * percentIncrease) < 1; iterationCount++)
			{
				Log(LOG_DEV_DATA, "New \"Torque Constant\" System Tag = %f",
					(iterationCount * percentIncrease * torqueConstant));
				SystemWrite(GetDataTag("TorqueConstant"), (iterationCount * percentIncrease * torqueConstant));
				BposSleep(1000);
			} 

			//EnableMotorLoading(accelConstant, speedConstant, torqueConstant, 0.05);
			//Prompt operator to input pass/fail when test is complete.  EngineTorqueTestTime is the maximum amount of time to wait
			string status = OperatorPassFail(GetPrompt("EngineTorquePassFail"),GetParameterFloat("EngineTorqueTestTime"));
			// Setup testResults variable
			testResult = status == "Pass" ? testPass : testFail;
			// test done
			RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"));
		}
		else
		{	//failed to ereach target speed
			testResult = testFail;
			testResultCode = GetFaultCode("EngineTorqueSpeedFailure");
			testDescription = GetFaultDescription("EngineTorqueSpeedFailure");
			Log(LOG_ERRORS, "Failed to reach test speed %3.2f", GetParameterFloat("EngineTorqueMinSpeed"));
		}
		//turn off motor loading
		setMotor = "disable";
		if (CheckZeroSpeed())
		{
			DisableMotorLoading(setMotor);
		}
		RemovePrompt(GetPromptBox("EngineTorqueTest"), GetPrompt("EngineTorqueTest"), GetPromptPriority("EngineTorqueTest"));
		// Report the results
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	SystemWrite(GetDataTag("TorqueConstantEnable"), false);
	SystemWrite(GetDataTag("TorqueConstant"), (float)0.0);
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class VehicleModuleType>
string DcbrBoschEmissionsTC<VehicleModuleType>::CheckEngineTorquePerformace(void)
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	string operatorStatus (BEP_TESTING_STATUS);

	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	if (!ShortCircuitTestStep() && ShouldPerformEngineTorqueTest())
	{
		try
		{	//Request operator input for Torque test
			string operatorStatus = OperatorPassFail(GetDataTag("CheckTorquePrompt"), GetTestStepInfoInt("Timeout"));
			//report operator input
            if (BEP_STATUS_SUCCESS != StatusCheck())
            {   // Bad system status
                testResult = testAbort;
                testResultCode = GetFaultCode("SystemStatus");
                testDescription = GetFaultDescription("SystemStatus");
                Log(LOG_ERRORS, "Bad system status, aborting -- status: %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
            }
            else if (operatorStatus != testPass)
            {
				testResult = testFail;
				testResultCode = GetFaultCode("EngineTorqueOperatorFailure");
				testDescription = GetFaultDescription("EngineTorqueOperatorFailure");
				Log(LOG_ERRORS, "EngineTorque: Failed by Operator %s", operatorStatus.c_str());
			}

			Log(LOG_DEV_DATA, "Engine Torque: Operator %s", (operatorStatus == testPass) ? "Pass" : "Fail");
		}
		catch (ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException setting up dynamic LID: %s", excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the results
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class VehicleModuleType>
string DcbrBoschEmissionsTC<VehicleModuleType>::MaxSpeedTest(void)
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	string speedRange = "0 0";
	char tempCharArray[20];
	INT32 scanDelay;
	string maxSpeed = SystemRead(GetDataTag("MaxSpeed"));
	bool fourByFourEquipped = true;
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	for (XmlNodeMapItr iter = m_maxSpeedTestEnabledTypes.begin(); iter != m_maxSpeedTestEnabledTypes.end(); iter++)
	{
		Log(LOG_DEV_DATA, "Config Output: %s; Subscription Output: %s\n", iter->second->getValue().c_str(), SystemRead("Traction").c_str());
		if (iter->second->getValue() == SystemRead("Traction"))
		{
			fourByFourEquipped = false;
			Log(LOG_DEV_DATA, "This is not a 4x4 truck, so we'll run DcbrBoschEmissionsTC::MaxSpeedTest().\n");
		}
	}
	if ((!ShortCircuitTestStep()) && ((maxSpeed != "Undefined") && (fourByFourEquipped == false || atob(GetParameter("AlwaysRunMaxSpeedTest").c_str()))))
	{	//Save delay time between module reads
		double maxSpeedInMph = atof(maxSpeed.c_str()) * 0.6213712;
		scanDelay = GetTestStepInfoInt("ScanDelay");
		DisplayPrompt(GetPromptBox("MaxSpeedTest"), GetPrompt("MaxSpeedTest"), GetPromptPriority("MaxSpeedTest"));
		// Prompt the operator to speed
		// 		Speed is between maxSpeedInKph and maxSpeedInKph + 10%
		testResult = WaitAndPromptForSpeed(maxSpeedInMph, 
										   maxSpeedInMph * 1.1,
										   scanDelay);
        if (BEP_STATUS_SUCCESS != StatusCheck())
        {   // Bad system status
            testResult = testAbort;
            testResultCode = GetFaultCode("SystemStatus");
            testDescription = GetFaultDescription("SystemStatus");
            Log(LOG_ERRORS, "Bad system status, aborting -- status: %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
        }
        else if (testResult == testPass)
        {
			//Prompt operator to input pass/fail when test is complete.  EngineTorqueTestTime is the maximum amount of time to wait
			//string status = OperatorYesNo(GetPrompt("MaxSpeedPassFail"),GetParameterFloat("MaxSpeedTestTime"));
			// Setup testResults variable
			//testResult = status == "Yes" ? testPass : testFail;
		}
		else
		{	//failed to reach target speed
			testResult = testFail;
			testResultCode = GetFaultCode("MaxSpeedFailure");
			testDescription = GetFaultDescription("MaxSpeedFailure");
		}
		speedRange = CreateMessage(tempCharArray,sizeof(tempCharArray),"%d %d", (int) maxSpeedInMph, (int) (maxSpeedInMph * 1.1));
		SystemWrite(GetDataTag("SpeedTarget"), speedRange);
		BposSleep(GetParameterInt("PostMaxSpeedDelay"));
		SystemWrite(GetDataTag("SpeedTarget"), "0 0");
		RemovePrompt(GetPromptBox("MaxSpeedTest"), GetPrompt("MaxSpeedTest"), GetPromptPriority("MaxSpeedTest"));

	}
	else
	{
		Log(LOG_FN_ENTRY, "Skipping test step %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

template <class VehicleModuleType>
void DcbrBoschEmissionsTC<VehicleModuleType>::Abort(void)
{
	GenericTC::Abort();
	Log(LOG_DEV_DATA, "Dcbr Bosch emissions test aborting!\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline bool DcbrBoschEmissionsTC<VehicleModuleType>::ShouldPerformEngineTorqueTest()
{   // Read the build item to see if it is set to a valid value
    bool performTest = SystemRead(GetDataTag("EngineTorqueBuildOption")).compare("NoEngineTorqueTestValue");
    Log(LOG_DEV_DATA, "Perform engine torque test; %s", performTest ? "TRUE" : "FALSE");
    return performTest;
}
