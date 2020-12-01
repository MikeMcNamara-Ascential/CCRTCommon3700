//*************************************************************************
// FILE DESCRIPTION:
//  Tire Pressure Monitor Test Component.
//
//===========================================================================
// Copyright (c) 2015- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "TpmsTc.h"

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
TpmsTc<ModuleType>::TpmsTc() : GenericTCTemplate<ModuleType>(), 
	m_timeAtSpeedDone(false), m_remainingTimeAboveSpeed(0.0)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
TpmsTc<ModuleType>::~TpmsTc()
{   // Nothing special to do here
    m_speedCheckTimer.Stop();
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
const string TpmsTc<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::CommandTestStep(value: %s) - Enter", value.c_str());
	try
	{
		//if(TpmEquipped())
		//{
			if(BEP_STATUS_SUCCESS == StatusCheck())
			{
				if(!GetTestStepName().compare("CheckTpmsResults"))             result = CheckTpmsResults();
				else if(!GetTestStepName().compare("StartSpeedMonitor"))       result = StartSpeedTimer();
				else if(!GetTestStepName().compare("WaitForTimeAboveSpeed"))   result = WaitForTimeAtSpeedComplete();
				else      
					result = GenericTCTemplate<ModuleType>::CommandTestStep(value);
			}
			else
			{
				Log(LOG_ERRORS, "Bad system status during test, not performing %s",
					GetTestStepName().c_str());
				result = testSkip;
			}
		//}
		//else
		//{
		//	Log(LOG_DEV_DATA, "Vehicle not equipped with TPM, skipping test step %s", 
		//		GetTestStepName().c_str());
		//	result = testSkip;
		//}
	}
	catch(ModuleException &excpt)
	{
		Log(LOG_ERRORS, "Module exception during %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BEP exception during %s - %s", GetTestStepName().c_str(), excpt.GetReason());
		result = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::CommandTestStep(value: %s) - Exit", value.c_str());
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
const INT32 TpmsTc<ModuleType>::HandlePulse(const INT32 code, const INT32 value)
{
	INT32 status = BEP_STATUS_ERROR;
	switch(code)
	{
	case TPMS_TC_PULSE_CODE:
		switch(value)
		{
		case TPMS_SPEED_TIME_PULSE:
			status = TpmUpdateSpeedTime();
			break;

		default:
			status = BEP_STATUS_SOFTWARE;
			Log(LOG_ERRORS, "Unhandled TPMS pulse value: %d", value);
			break;
		}
		break;

	default:
		status = GenericTCTemplate<ModuleType>::HandlePulse(code, value);
		break;
	}
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
const string TpmsTc<ModuleType>::Register(void)
{
	string result = BepServer::Register();
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::Register() - Enter");
	m_semInitialized.CriticalSectionEnter();
	Log(LOG_FN_ENTRY, "Entered critical section, checking build options.");
	string testStatus = TEST_STATUS_IN_PROGRESS;
	bool tpmEquipped = true;
	//if(!SystemRead(GetDataTag("TpmsBuildItem")).compare(GetDataTag("TPmsEquipped")))
	//{
	//	testStatus = TEST_STATUS_IN_PROGRESS;
	//	tpmEquipped = true;
	//}
	//else
	//{
	//	testStatus = TEST_STATUS_NOT_STARTED;
	//	tpmEquipped = false;
	//}
	//Log(LOG_DEV_DATA, "Setting TPM Equipped to %s", tpmEquipped ? "True" : "False");
	//TpmEquipped(&tpmEquipped);
	Log(LOG_DEV_DATA, "Updating TPM status to %s", testStatus.c_str());
	UpdateTestStatus(testStatus);
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::Register() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
string TpmsTc<ModuleType>::CheckTpmsResults(void)
{
    string testResult = testFail;
    string testResultCode("0000");
    string testDescription = GetTestStepInfo("Description");
    BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;
    bool tpmsStatus = false;

    Log(LOG_FN_ENTRY, "Enter MahindraTransmission::CheckTpmsStatus()\n");
    
    //Check the TPMS status
    moduleStatus = m_vehicleModule.ReadModuleData("CheckTpmsStatus", tpmsStatus);

    if (moduleStatus == BEP_STATUS_SUCCESS)
    {
        if (tpmsStatus)
        {
            Log(LOG_DEV_DATA, "Tpms status passed.");
            testResult = testPass;
        }
        else
        {
            Log(LOG_DEV_DATA, "Tpms status failed.");
            testDescription = "Tpms status failed";
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

    // Report the results
    SendTestResultWithDetail(testResult, testDescription, testResultCode);

    // Log the exit and return the result
    Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());

    return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
string TpmsTc<ModuleType>::StartSpeedTimer(void)
{
	string result(testPass);
	Log(LOG_DEV_DATA, "Mahindra TpmsTc::StartSpeedTimer() - Enter");
	if(!ShortCircuitTestStep())
    {
		// Setup and start the timer
        UINT64 timerDelay = GetParameterInt("VehicleSpeedMonitorTimeDelay");
		if(timerDelay == 0)
		{
			timerDelay = 500;
			Log(LOG_ERRORS, "No speed monitor interval declared, using 500ms");
		}
		Log(LOG_DEV_DATA, "Starting speed monitor timer with interval %dms", timerDelay);
		m_speedCheckTimer.SetPulseCode(TPMS_TC_PULSE_CODE);
		m_speedCheckTimer.SetPulseValue(TPMS_SPEED_TIME_PULSE);
		m_speedCheckTimer.Initialize(GetProcessName(), mSEC_nSEC(timerDelay), mSEC_nSEC(timerDelay));
		m_speedCheckTimer.Start();
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_DEV_DATA, "Not starting speed above threshold timer");
		result = testSkip;
	}
	Log(LOG_DEV_DATA, "Mahindra TpmsTc::StartSpeedTimer() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
string TpmsTc<ModuleType>::WaitForTimeAtSpeedComplete(void)
{
	string result(BEP_TESTING_RESPONSE);
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::WaitForTimeAtSpeedComplete() - Enter");
	if(!ShortCircuitTestStep())
	{
		bool promptDisplayed = false;
		char buff[32];
		string strTimeRemain;
		while(!m_timeAtSpeedDone && (BEP_STATUS_SUCCESS == StatusCheck()) && TimeRemaining())
		{
			if(!promptDisplayed)
			{
				SystemWrite("SpeedTarget", GetParameter("TpmSpeedTarget"));
				promptDisplayed = true;
			}
			strTimeRemain = CreateMessage(buff, sizeof(buff), "%.1f", m_remainingTimeAboveSpeed);
			DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
						  GetPromptPriority("MaintainSpeed"), "white", strTimeRemain);
			BposSleep(GetTestStepInfoInt("ScanDelay"));
		}
		RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), 
					  GetPromptPriority("MaintainSpeed"));
		SystemWrite("SpeedTarget", string("0 0"));
		// Check the exit conditions
		result = m_timeAtSpeedDone ? testPass : testFail;
		Log(LOG_DEV_DATA, "Required time above speed threshold complete - %s", result.c_str());
		SendTestResult(result, GetTestStepInfo("Description"), "0000");
	}
	else
	{
		Log(LOG_DEV_DATA, "Not waiting for require time above speed to complete");
		result = testSkip;
	}
	Log(LOG_FN_ENTRY, "Mahindra TpmsTc::WaitForTimeAtSpeedComplete() - Exit");
	return result;
}

//-------------------------------------------------------------------------------------------------
//template <class ModuleType> 
//const bool& TpmsTc<ModuleType>::TpmEquipped(const bool *equipped /*= NULL*/)
//{
//	if(equipped != NULL)  m_tpmEquipped = *equipped;
//	return m_tpmEquipped;
//}

//-------------------------------------------------------------------------------------------------
template <class ModuleType> 
INT32 TpmsTc<ModuleType>::TpmUpdateSpeedTime(void)
{
	// Get the current speed
    BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	time_t currentTime = time(NULL);

    vector<bool> tpmsStatus;
    status = m_vehicleModule.ReadModuleData("ReadVehicleSpeedThresholdCrossed", tpmsStatus);
    if(BEP_STATUS_SUCCESS == status)
    {
        bool vehSpeedThresholdCrossed = tpmsStatus[0];
        bool allSensorsPass = tpmsStatus[1];
        if (vehSpeedThresholdCrossed) //above speed needed
        {
    		double timeElapsed = difftime(currentTime, m_tpmSpeedStartTime); 
    		m_timeAtSpeedDone = timeElapsed > GetParameterFloat("RequiredTimeAboveSpeed");
    		m_remainingTimeAboveSpeed = max(GetParameterFloat("RequiredTimeAboveSpeed") - timeElapsed, (double)0.0);
    		if(m_timeAtSpeedDone) 
    		{
    			m_speedCheckTimer.Stop();
                Log(LOG_DEV_DATA, "Required time above speed complete, stopping timer");
    		}
            else
            {
                if (allSensorsPass)
                {
                    m_timeAtSpeedDone = true;
        			m_speedCheckTimer.Stop();
                    Log(LOG_DEV_DATA, "all tpms sensors passed, stopping timer");
                }
            }
        }
    	else
    	{
    		if(!m_timeAtSpeedDone)
    		{
    			m_tpmSpeedStartTime = currentTime;
    		}
    	}
    }
	else
	{
        Log(LOG_DEV_DATA, "ERROR reading ReadVehicleSpeedThresholdCrossed");
	}

	return BEP_STATUS_SUCCESS;
}
