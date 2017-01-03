//******************************************************************************
// Description:
// Test steps specific to Isuzu Mimamori Module testing.
//
//===========================================================================
// Copyright (c) 2016- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "MimamoriTc.h"

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MimamoriTc<ModuleType>::MimamoriTc() : GenericTCTemplate<ModuleType>()
{
    m_dtcCleared = false;

    m_dataCleared = false;
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
MimamoriTc<ModuleType>::~MimamoriTc()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const bool MimamoriTc<ModuleType>::CheckCableConnect(void)
{
	bool cableConnected = false;  // Used to store return result from IsCableConnected()
	bool promptDisplayed = false; // Used to determine if prompts have been displayed
	UINT32 delay = 100;  // Used to store amount of delay from TestStep info in config file

	// Set delay in do-while loop using BposSleep() function call
	delay = GetTestStepInfoInt("ScanDelay");
	// Start the timer
	SetStartTime();
	// Wait for the cable to be connected
	do
	{   // Check if the cable is connected and prompt if neccessary
		cableConnected = IsCableConnected();
		Log(LOG_DEV_DATA, "CheckCableConnect - IsCableConnected: %s -- {ReadSubscribeData(MimamoriCableConnect): %s}\n",
			cableConnected ? "Yes" : "No", SystemReadBool("MimamoriCableConnect") ? "True" : "False");
		if(!cableConnected)
		{   // Check if bkground test
			if( !IsForegroundTest())
			{   // Resets the step start time since the step should not prompt until
				//     its TestDomain is set to foreground
				SetStartTime();
			}
			// if TestDomain is foreground, display prompt if not already done
			else if(!promptDisplayed)
			{   // Cable not connected, prompt the operator
				DisplayPrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
				promptDisplayed = true;
			}
			// Wait before checking again
			BposSleep(delay);
		}
	} while(TimeRemaining() && !cableConnected && (BEP_STATUS_SUCCESS == StatusCheck()));

	// Check if the prompt needs to be removed
	if(promptDisplayed)
	{   // Remove the prompt from the display
		RemovePrompt(GetPromptBox("CableConnect"), GetPrompt("CableConnect"), GetPromptPriority("CableConnect"));
	}
	// Log the status
	if(!cableConnected) Log(LOG_ERRORS, "Timeout waiting for cable connect!\n");
	else                 Log(LOG_DEV_DATA, "Cable Connected\n");
	// Return the status of cable connect
	return(cableConnected);
}

//-------------------------------------------------------------------------------------------------
template<class ModuleType>
const bool MimamoriTc<ModuleType>::IsCableConnected(void)
{
//	return SystemReadBool("MimamoriCableConnect");
	return SystemReadBool("CableConnect");
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string MimamoriTc<ModuleType>::CommandTestStep(const string &value)
{
	string status;

	try
	{	// Get the test step name to perform
		string step = GetTestStepName();
		Log(LOG_DEV_DATA, "MimamoriTc.CommandTestStep(%s): Entering test step %s\n",
			GetComponentName().c_str(), value.c_str(), step.c_str());
		// Check the status to ensure it is alright to run this test step
		if(StatusCheck() != BEP_STATUS_SUCCESS)
		{	// Not OK to run the test step
			UpdateResult(StatusCheck(), status);
			Log(LOG_ERRORS, (GetComponentName() +
							 ".CommandTestStep: StatusCheck() failed: %s\n").c_str(),status.c_str());
			// Set status to Skip since this test step will not be performed
			status = testSkip;
			SendTestResult(testSkip, GetTestStepInfo("Description"));
		}
		// Setup the module for testing
		else if(step == "CheckDataRecordMemory")     status = CheckDataRecordMemory();
		// Place the module into diagnostic mode
		else if(step == "CheckFuelEconomy")		     status = CheckFuelEconomy();
		// Command the module to exit diagnostic mode
		else if(step == "CheckOilTemperature")		 status = CheckOilTemperature();
		// Validate the module part number
		else if(step == "CheckTotalMileage")	     status = CheckTotalMileage();
		// Validate the module part number
		else if(step == "ClearHistory")			     status = ClearHistory();
        //Clear Faults
        else if(step == "ClearFaults")               status = ClearFaults();
        //Read Faults
        else if(step == "ReadFaults")                status = ReadFaults();
		// Unknown test step
		else
		{
			status = GenericTCTemplate<ModuleType>::CommandTestStep(value);
		}
	}
	catch(ModuleException &caughtModuleException)
	{
		Log(LOG_ERRORS, "%s.%s: %s", GetComponentName().c_str(),
			GetTestStepName().c_str(), caughtModuleException.message().c_str());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &e)
	{
		Log(LOG_DEV_DATA, "%s CommandTestStep %s Exception: %s\n",
			GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n",
		GetComponentName().c_str(), value.c_str(), status.c_str());
	return(status);
}

template<class ModuleType>
const string MimamoriTc<ModuleType>::CheckDataRecordMemory()
{   // Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string value;
    bool engineDataCleared = false;
    bool atDataCleared = false;
    bool scrDataCleared = false;
    bool brakeDataCleared = false;
    int i = 0;
    Log(LOG_FN_ENTRY, "Enter MimamoriTc::CheckDataRecordMemory()\n");
    if(!ShortCircuitTestStep())
	{
        do
        {

            try
		    {
			    moduleStatus = m_vehicleModule.GetInfo("ReadDrmEngine", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    for(i = 0; i < value.length(); i++)
                    {
                        if(value.at(i) != '\0')
                            break;
                    }
                    if(i == value.length())
                        engineDataCleared = true;
                }
            }
		    catch(ModuleException &exception)
		    {	// Exception during security unlock
		    	Log(LOG_ERRORS, "Module Exception during CheckDataRecordMemory() - %s\n", exception.message().c_str());
	    		moduleStatus = BEP_STATUS_SOFTWARE;
		    }
            try
            {
                moduleStatus = m_vehicleModule.GetInfo("ReadDrmAt", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    for(i = 0; i < value.length(); i++)
                    {
                        if(value.at(i) != '\0')
                            break;
                    }
                    if(i == value.length())
                        atDataCleared = true;
                }                                
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during CheckDataRecordMemory() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
            }
            try
            {
                moduleStatus = m_vehicleModule.GetInfo("ReadDrmScr", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    for(i = 0; i < value.length(); i++)
                    {
                        if(value.at(i) != '\0')
                            break;
                    }
                    if(i == value.length())
                        scrDataCleared = true;
                }
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during CheckDataRecordMemory() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
            }
            try
            {
                moduleStatus = m_vehicleModule.GetInfo("ReadDrmBrake", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    for(i = 0; i < value.length(); i++)
                    {
                        if(value.at(i) != '\0')
                            break;
                    }
                    if(i == value.length())
                        brakeDataCleared = true;
                }
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during CheckDataRecordMemory() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
            }

            if((engineDataCleared && atDataCleared && scrDataCleared && brakeDataCleared) || !m_dataCleared)
            {
                testResult = testPass;
            }
            else
            {
                Log(LOG_DEV_DATA,"engineData: %s, atData: %s, scrData: %s, brakeData: %s", engineDataCleared ? "true":"false", atDataCleared ? "true":"false", scrDataCleared ? "true":"false", brakeDataCleared ? "true":"false");
                testResult = testFail;
                testResultCode = GetFaultCode("DRMNotCleared");
	            testDescription = GetFaultDescription("DRMNotCleared");
            }
        } while(TimeRemaining() && testResult == BEP_TESTING_STATUS && (BEP_STATUS_SUCCESS == StatusCheck()));
    }
    else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}

    SendTestResult(testResult, testDescription, testResultCode);

	// Return the test result
	Log(LOG_FN_ENTRY, "Exit MimamoriTc::CheckDataRecordMemory()\n");
    return testResult;
}
template<class ModuleType>
const string MimamoriTc<ModuleType>::CheckFuelEconomy()
{   // Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float value;
    Log(LOG_FN_ENTRY, "Enter MimamoriTc::CheckFuelEconomy()\n");
    if(!ShortCircuitTestStep())
	{
        do
        {
            try
		    {
		    	moduleStatus = m_vehicleModule.GetInfo("ReadFuelEconomy", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    if(value == 0 || !m_dataCleared)
                        testResult = testPass;
                    else
                    {
                        testResult = testFail;
                        testResultCode = GetFaultCode("FuelEconomyNotCleared");
	                    testDescription = GetFaultDescription("FuelEconomyNotCleared");
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("FuelEconomyNotCleared");
	                testDescription = GetFaultDescription("FuelEconomyNotCleared");
                }
		    }
		    catch(ModuleException &exception)
            {
                Log(LOG_ERRORS, "Module Exception during CheckFuelEconomy() - %s\n", exception.message().c_str());
			    moduleStatus = BEP_STATUS_SOFTWARE;
                testResult = testFail;
                testResultCode = GetFaultCode("FuelEconomyNotCleared");
	            testDescription = GetFaultDescription("FuelEconomyNotCleared");
	        }
        } while(TimeRemaining() && testResult == BEP_TESTING_STATUS && (BEP_STATUS_SUCCESS == StatusCheck()));
    }
    else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}

    SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit MimamoriTc::CheckFuelEconomy()\n");
    return testResult;
}
template<class ModuleType>
const string MimamoriTc<ModuleType>::CheckOilTemperature()
{   // Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string value;
    int i = 0;
    Log(LOG_FN_ENTRY, "Enter MimamoriTc::CheckOilTemperature()\n");
    if(!ShortCircuitTestStep())
	{
        do
        {
            try
            {
                moduleStatus = m_vehicleModule.GetInfo("ReadDrmEdrOilTemp", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    for(i = 0; i < value.length(); i++)
                    {
                        if(value.at(i) != '\0')
                            break;
                    }
                    if(i == value.length() || !m_dataCleared)
                        testResult = testPass;
                    else
                    {
                        testResult = testFail;
                        testResultCode = GetFaultCode("ATOilTemperatureNotCleared");
	                    testDescription = GetFaultDescription("ATOilTemperatureNotCleared");
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("ATOilTemperatureNotCleared");
	                testDescription = GetFaultDescription("ATOilTemperatureNotCleared");
                }
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during CheckOilTemperature() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
                testResult = testFail;
                testResultCode = GetFaultCode("ATOilTemperatureNotCleared");
	            testDescription = GetFaultDescription("ATOilTemperatureNotCleared");
            }
        } while(TimeRemaining() && testResult == BEP_TESTING_STATUS && (BEP_STATUS_SUCCESS == StatusCheck()));
    }
    else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}


    SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit MimamoriTc::CheckOilTemperature()\n");
    RemovePrompts();
    return testResult;
}
template<class ModuleType>
const string MimamoriTc<ModuleType>::CheckTotalMileage()
{   // Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    float value;
    Log(LOG_FN_ENTRY, "Enter MimamoriTc::CheckTotalMileage()\n");
    if(!ShortCircuitTestStep())
	{
        do
        {
            try
            {
                moduleStatus = m_vehicleModule.GetInfo("ReadTotalMileage", value);
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    m_vehicleModule.CommandModule("ReadTriggerResponse");
                    if(value == 0 || !m_dataCleared)
                        testResult = testPass;
                    else
                    {
                        testResult = testFail;
                        testResultCode = GetFaultCode("TotalMileageNotCleared");
	                    testDescription = GetFaultDescription("TotalMileageNotCleared");
                    }
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("TotalMileageNotCleared");
	                testDescription = GetFaultDescription("TotalMileageNotCleared");
                }
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during CheckTotalMileage() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
                testResult = testFail;
                testResultCode = GetFaultCode("TotalMileageNotCleared");
	            testDescription = GetFaultDescription("TotalMileageNotCleared");
            }
        } while(TimeRemaining() && testResult == BEP_TESTING_STATUS && (BEP_STATUS_SUCCESS == StatusCheck()));
    }
    else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}


    SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit Mimamori::CheckTotalMileage()\n");
    return testResult;
}
template<class ModuleType>
const string MimamoriTc<ModuleType>::ClearHistory()
{   // Set up some variables
	string testResult = BEP_TESTING_STATUS;
	string testResultCode("0000");
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    string value;
    Log(LOG_FN_ENTRY, "Enter MimamoriTc::ClearHistory()\n");
    UpdatePrompts();
    if(!ShortCircuitTestStep())
	{
        do
        {
            try
            {
                moduleStatus = m_vehicleModule.CommandModule("ClearHistory");
                if(moduleStatus == BEP_STATUS_SUCCESS)
                {
                    testResult = testPass;
                    m_dataCleared = true;
                    delay(GetParameterInt("ClearHistoryDelay"));
                }
                else
                {
                    testResult = testFail;
                    testResultCode = GetFaultCode("HistoryNotCleared");
	                testDescription = GetFaultDescription("HistoryNotCleared");
                }
            }
            catch(ModuleException &exception)
            {	// Exception during security unlock
                Log(LOG_ERRORS, "Module Exception during ClearHistory() - %s\n", exception.message().c_str());
                moduleStatus = BEP_STATUS_SOFTWARE;
                testResult = testFail;
                testResultCode = GetFaultCode("HistoryNotCleared");
	            testDescription = GetFaultDescription("HistoryNotCleared");
            }
        } while(TimeRemaining() && testResult == BEP_TESTING_STATUS && (BEP_STATUS_SUCCESS == StatusCheck()));
    }
    else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}

    SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit MimamoriTc::ClearHistory()\n");
    return testResult;
}

template<class ModuleType>
string MimamoriTc<ModuleType>::ClearFaults()
{
    string testResult = GenericTCTemplate<ModuleType>::ClearFaults();
    if(testResult == testPass)
        m_dtcCleared = true;
    return testResult;
}

template<class ModuleType>
string MimamoriTc<ModuleType>::ReadFaults()
{
    UpdatePrompts();
    string testResult = GenericTCTemplate<ModuleType>::ReadFaults();
    if(!m_dtcCleared || testResult == testPass)
        return testPass;
    return testResult;
}

