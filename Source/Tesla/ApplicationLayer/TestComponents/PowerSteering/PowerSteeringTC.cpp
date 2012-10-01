//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: $
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "PowerSteeringTC.h"
#include "ModuleException.h"

//-----------------------------------------------------------------------------
template<class ModuleType>
PowerSteeringTC<ModuleType>::PowerSteeringTC() : GenericTCTemplate<ModuleType>()
{	// Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
PowerSteeringTC<ModuleType>::~PowerSteeringTC()
{	// Nothing special to do here
}

//-----------------------------------------------------------------------------
template<class ModuleType>
const string PowerSteeringTC<ModuleType>::CommandTestStep(const string &value)
{
	string result(BEP_TESTING_STATUS);
	Log(LOG_FN_ENTRY, "PowerSteeringTC::CommandTestStep(value: %s) - Step: %s - Enter", 
		value.c_str(), GetTestStepName().c_str());
	// Make sure system status will allow testing
	if(BEP_STATUS_SUCCESS != StatusCheck())
	{
		UpdateResult(StatusCheck(), result);
		Log(LOG_ERRORS, "PowerSteeringTC::CommandTestStep: StatusCheck() failed: %s", result.c_str());
		SendTestResult(result, GetTestStepInfo("Description"));
	}
	// Link up with the module
	else if(!GetTestStepName().compare("LockSteeringTune"))	  result = LockSteeringTune();
	// Check the airbag lamp status
	// Call the base class to perform the test step
	else	 result	= GenericTCTemplate<ModuleType>::CommandTestStep(value);
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "PowerSteeringTC::CommandTestStep(value: %s) - Step: %s - Exit: %s", 
		value.c_str(), GetTestStepName().c_str(), result.c_str());
	return result;
}

//-----------------------------------------------------------------------------
template <class ModuleType>
string PowerSteeringTC<ModuleType>::ProgramVIN(void)
{
	string testStatus = testFail;
	BEP_STATUS_TYPE status;
	string faultTag("CommunicationFailure");
	bool routineComplete = false;
	string routineStatus("Unknown");

	Log(LOG_FN_ENTRY, "Enter PowerSteeringTC::ProgramVIN()\n");
	if(ShortCircuitTestStep())
	{
		Log(LOG_FN_ENTRY, "Skipping test step PowerSteeringTC::ProgramVin()");
		testStatus = testSkip;
	}
	else
	{
		try
		{	// Actually program the vin
			status = m_vehicleModule.ProgramVIN();
			if(status == BEP_STATUS_SUCCESS)
			{
				if(GetParameterBool("RequestVinRoutineStatusFromModule"))
				{
					BposSleep(GetParameterInt("DelayBeforeRoutineStatus"));
					status = m_vehicleModule.ReadModuleData("GetVinRoutineStatus",routineStatus);
					for(INT32 tries=0; (tries < GetParameterInt("RoutineRequestRetries")) && !routineComplete && (status == BEP_STATUS_SUCCESS); tries++)
					{
						if((!routineStatus.compare("BadVINReceived") || !routineStatus.compare("Timeout") || !routineStatus.compare("WriteError")) && (status == BEP_STATUS_SUCCESS))
						{
							status = m_vehicleModule.ProgramVIN();
							if(status == BEP_STATUS_SUCCESS)
							{
								BposSleep(GetParameterInt("VinRetryDelay"));
								status = m_vehicleModule.ReadModuleData("GetVinRoutineStatus",routineStatus);
							}
						}
						else
						{
							routineComplete = true;
						}
					}
					if(!routineStatus.compare("VINLearned") || !routineStatus.compare("VINAlreadyLearned"))
					{
						testStatus = testPass;
					}
				}
				else
				{
					testStatus = testPass;
				}
				SendTestResultWithDetail((testStatus == testPass) ? testPass : testFail, 
										 (testStatus == testPass) ? GetTestStepInfo("Description") : GetFaultDescription("VINRoutineFailure"),
										 (testStatus == testPass) ? "0000" : GetFaultCode("VINRoutineFailure"),
										 "RoutineStatus",routineStatus);
			}
			else
			{
				testStatus = testFail;
				SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
										 GetFaultCode(faultTag), GetFaultName(faultTag),
										 GetFaultDescription(faultTag));
			}
		}
		catch(ModuleException& caughtModuleException)
		{
			Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
				caughtModuleException.message().c_str());
			testStatus = testFail;
		}
	}
	// Log the function exit
	Log(LOG_FN_ENTRY, "Exit PowerSteeringTC::ProgramVIN(), status=%s\n", testStatus.c_str());
	// Return the test result
	return(testStatus);
} 

//-----------------------------------------------------------------------------
template <class ModuleInterface>
string PowerSteeringTC<ModuleInterface>::LockSteeringTune(void)
{
	string testStatus = testFail;
	BEP_STATUS_TYPE status;
	string faultTag("TuneRoutineFailure");
	string routineStatus("Unknown");

	Log(LOG_FN_ENTRY, "Enter PowerSteeringTC::LockSteeringTune()\n");
	if(ShortCircuitTestStep())
	{
		Log(LOG_FN_ENTRY, "Skipping test step PowerSteeringTC::LockSteeringTune()");
		testStatus = testSkip;
	}
	else
	{


		try
		{	// Actually program the vin
			status = m_vehicleModule.CommandModule("LockSteeringTune");
			if(status == BEP_STATUS_SUCCESS) status = m_vehicleModule.ReadModuleData("GetTuneRoutineStatus",routineStatus);
			if((status == BEP_STATUS_SUCCESS) && (!routineStatus.compare("TuneLearned")))
			{
				testStatus = testPass;
				SendTestResult(testPass, GetTestStepInfo("Description"));
			}
			else
			{
				testStatus = testFail;
				SendTestResultWithDetail(testStatus, GetTestStepInfo("Description"),
										 GetFaultCode(faultTag), GetFaultName(faultTag),
										 GetFaultDescription(faultTag));
			}
		}
		catch(ModuleException& caughtModuleException)
		{
			Log(LOG_ERRORS, "%s.%s: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(),
				caughtModuleException.message().c_str());
			testStatus = testFail;
		}
	}
	// Log the function exit
	Log(LOG_FN_ENTRY, "Exit PowerSteeringTC::LockSteeringTune(), status=%s\n", testStatus.c_str());
	// Return the test result
	return(testStatus);
} 


