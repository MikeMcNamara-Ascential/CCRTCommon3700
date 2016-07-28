//******************************************************************************
// Description:
//  ABS Test Component specialized for TRW CAN applications
//
//==============================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CMake Core 062909/Ford/Source/FORD/ApplicationLayer/TestComponents/Brake/TRW/CAN/FordTRWCANABSTC.cpp $
// 
// 1     2/07/11 4:48p Mmcnamar
// backup c drive source for cross compiler
// 
// 2     9/20/10 2:54p Mmcnamar
// Using delay() instead of BposSleep() for valve activation delays.
// 
// 1     5/10/10 2:07p Mmcnamar
// initial check-in for new core.
// 
// 10    3/29/10 4:09p Mmcnamar
// Reading wheel speeds at end of Reduction() and Recovery() to set valve
// cross end markers if wheel speeds drop below 10 mph.
// 
// 9     3/17/10 1:24p Mmcnamar
// Added comm fault log statements in reduction() & recovery().
// 
// 8     3/10/09 3:21p Mmcnamar
// Updated CloseRelay(): Waiting for "ABSTestStartSpeed" before sending
// ABS cmd.
// 
// 7     2/20/09 3:46p Mmcnamar
// Changed Delay() to BposSleep() in CloseRelay() and IsolateAll().
// 
// 6     10/21/08 11:08a Mmcnamar
// Added SendTestResult() at end of CloseRelay(); Using BposSleep()
// instead of Delay() in Reduction() & Recovery().
// 
// 5     2/24/06 1:24p Bmeinke
// Changed many GetInfo() calls with CommandModule() calls
// 
// 4     2/20/06 12:49p Bmeinke
// Modified the Reduction() and Recovery() commands to send a "Prepare"
// command prior to the actual reduction/recovery commands
// 
// 3     6/13/05 9:12a Bmeinke
// Inside the CloseRelay() method: changed GetInfo("CloseRelay") to
// GetInfo("RelayOn")
// Added test step methods for FrontInletsOn and RearInletsOn
// 
// 2     6/09/05 4:45p Bmeinke
// Removed the individual Reduction and Recovery methods and replaced them
// with generic versions
// Changes the names of the valve firing delay parameters
// 
// 1     5/04/05 12:36p Bmeinke
// TRW CAN ABS component files with overloaded valve firing sequence
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "FordTRWCANABSTC.h"
#include "FordABSModuleTemplate.cpp"
#include "FordABSTCTemplate.cpp"

FordTRWCANABSTC::FordTRWCANABSTC() : FordCanAbsComp_t()
{
}

FordTRWCANABSTC::~FordTRWCANABSTC()
{
}

const string FordTRWCANABSTC::CommandTestStep(const string &value)
{
	// Status of the test step
	string status;
	const std::string step(GetTestStepName());

	Log(LOG_DEV_DATA, "Enter FordTRWCANABSTC::CommandTestStep(%s): Entering test step %s\n",
		value.c_str(), step.c_str());
	try
	{	
		// Log it
		// Ensure the status is good before we sequence a test step
		if (StatusCheck() != BEP_STATUS_SUCCESS)
		{	// IF the overall status is bad
			UpdateResult(StatusCheck(), status);
			Log(LOG_ERRORS, "%s.CommandTestStep: StatusCheck() failed: %s\n", GetComponentName().c_str(), status.c_str());
			SendTestResult(status, GetTestStepInfo("Description"));
		}

		//********************************************************************
		//	Add new test step handlers here

		// Run the CloseRelay method
		else if( "CloseRelay" == step)				status = CloseRelay();
		// Open front ABS outlet valves
		else if( "FrontInletValvesOn" == step)      status = FrontInletValvesOn();
		// Open front ABS outlet valves
		else if( "RearInletValvesOn" == step)       status = RearInletValvesOn();
		// Open all ABS outlet valves
		else if( "AllOutletValvesOn" == step)       status = AllOutletValvesOn();
		// Close all ABS outlet valves
		else if( "AllOutletValvesOff" == step)      status = AllOutletValvesOff();

		// Since it's not a test step we've defined, call the parent
		else								status = FordCanAbsComp_t::CommandTestStep(value);

		//
		//********************************************************************
	}
	catch (ModuleException &ex)
	{
		Log(LOG_ERRORS, "MOduleException in %s:%s - %s", GetComponentName().c_str(), GetTestStepName().c_str(), ex.GetReason());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch (BepException &ex)
	{
		Log(LOG_ERRORS, "%s CommandTestStep %s Exception: %s\n",
			GetComponentName().c_str(), GetTestStepName().c_str(), ex.what());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	// This will be logged more than once perhaps if we called our parent's CommandTestStep() method
	Log(LOG_DEV_DATA, "Exit FordTRWCANABSTC::CommandTestStep(%s): TestStep %s, Returning %s\n",
		value.c_str(), step.c_str(), status.c_str());
	
	// return the status
	return(status);
}

string FordTRWCANABSTC::IsolateAll()
{
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter FordTRWCANABSTC::IsolateAll()\n");
	
	testResult = FordCanAbsComp_t::IsolateAll();
	if( testPass == testResult)
	{
		//Delay( GetParameterInt( "AllIsolationDelay"));
        //MAM 2/20/09
        //BposSleep( GetParameterInt( "AllIsolationDelay"));
        delay( GetParameterInt( "AllIsolationDelay"));  //MAM 9/2/10
	}
	Log(LOG_FN_ENTRY, "Exit FordTRWCANABSTC::IsolateAll(), status=%s\n", testResult.c_str());

	return( testResult);
}

std::string FordTRWCANABSTC::CloseRelay()
{
	std::string testResult = BEP_TESTING_STATUS;
	std::string testResultCode( "0000");
	std::string testDescription( "Close Pump Motor Relay");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
    //MAM 3/4/09
    float       startSpeed; 
    std::string status( BEP_FAIL_STATUS);

    //MAM 3/4/09
    // The TRW module needs to be below 55 mph and this objective comes before the ValveFiringTest objective in
    // the config file.
    startSpeed = GetParameterFloat( "ABSTestStartSpeed");
    if ( startSpeed > 0) status = WaitForTargetSpeed( startSpeed, DOWN);
    else                status = BEP_STATUS_SUCCESS;

	try
	{
		moduleStatus = m_vehicleModule.CommandModule( "RelayOn");
		if(BEP_STATUS_SUCCESS == moduleStatus)
		{
			testResult = testPass;
			//Delay( 10);
            //MAM 2/20/09
            //BposSleep( 10);
            delay( 10); //MAM 9/2/10
		}
		else
		{
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch( ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in CloseResly() - %s\n", err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}

    //MAM 10/16/08
    // Send the test result
    SendTestResult(testResult, testDescription, testResultCode);

	return( testResult);
}

string FordTRWCANABSTC::Reduction(const string &wheel, INT32 &startIndex, INT32 &stopIndex)
{
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
	std::string testResult = BEP_TESTING_STATUS;
	std::string testResultCode = "0000";
	std::string testDescription = "Performing reduction for " + wheel;
	const std::string	prepareCmd( "Prepare"+wheel+"Reduction");
	const std::string	reductionCmd("Actuate"+wheel+"Reduction");
	const std::string	holdCmd("Hold"+wheel+"Reduction");
	const INT32			interWheelGap = GetParameterInt( "InterWheelGapDelay");
	const INT32			dumpTime = GetParameterInt(wheel+"ReductionPulseLength");
	const INT32			holdTime = GetParameterInt(wheel+"ReductionHoldDelay");
    //MAM 3/26/10
    float               wheelSpeeds[ 4];
    int                 ii;
    //MAM 9/29/10
    bool  valveCrossSpeedCheck = GetParameterBool("PerformValveCrossSpeedCheck");
    
    startIndex = TagArray(wheel + "ReduxStart");    

	try
	{
		// Prepare for reduction
		status = m_vehicleModule.CommandModule( prepareCmd);
		if(BEP_STATUS_SUCCESS == status)
		{
			// Wait for inter-wheel gap time
			//if( interWheelGap > 0)	Delay( interWheelGap);
            //if( interWheelGap > 0)	BposSleep( interWheelGap);
            if( interWheelGap > 0)	delay( interWheelGap);  //MAM 9/2/10

			// Open the outlet
			status = m_vehicleModule.CommandModule( reductionCmd);
			if(BEP_STATUS_SUCCESS == status)
			{
				// Ensure open long enough
				//if( dumpTime > 0)	Delay( dumpTime);
                //if( dumpTime > 0)	BposSleep( dumpTime);
                if( dumpTime > 0)	delay( dumpTime);   //MAM 9/2/10

				// If we need to hold the reduction
				if( holdTime > 0)
				{
					// Hold the reduction
					status = m_vehicleModule.CommandModule(holdCmd);
					if(BEP_STATUS_SUCCESS == status)
					{
						// Hold for desired time
						//Delay( holdTime);
                        //BposSleep( holdTime);
                        delay( holdTime);   //MAM 9/2/10
					}
				}
			}
		}

		if( BEP_STATUS_SUCCESS != status)
		{
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
            Log(LOG_ERRORS, "FordTRWCANABSTC::Reduction() - CommunicationFailure \n");  //MAM 3/11/10
            //MAM 2/13/13
            m_absCommFault = true;
		}
		else
		{
			testResult = testPass;
            //stopIndex = TagArray(wheel + "ReduxStop");
		}
	}
	catch( ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Recovery(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
    //MAM 1/24/11
    stopIndex = TagArray(wheel + "ReduxStop");

    //MAM 9/29/10 - Check based on parameter in config file because we may need to do this for trucks but not cars.
    if ((true == valveCrossSpeedCheck)&&(testPass == testResult))
    {

        //MAM 3/26/10 - Make sure each wheel is above 10 mph for the valve cross check    
        if (BEP_STATUS_SUCCESS == m_baseBrakeTool->GetISpeeds(wheelSpeeds))
        {
            for ( ii=0; ii<4; ii++)
            {
                if ( wheelSpeeds[ ii] > 10)     m_valveCrossEnd[ii] = stopIndex;
                //MAM 4/28/10 - debug
                //Log( LOG_DEV_DATA, "end index for wheel %d = %2.2f  speed = %2.2f \n", ii, m_valveCrossEnd[ii], wheelSpeeds[ ii]);
                Log( LOG_DEV_DATA, "reduction end index for wheel %d = %d  speed = %2.2f \n", ii, m_valveCrossEnd[ii], wheelSpeeds[ ii]);
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Could not get wheel speeds for reduction end index! \n");
        }

    }    

	// Report the result of the test
    //MAM 9/1/10 - testing - commented out for now.
	SendSubtestResult(wheel + "Reduction", testResult, testDescription, testResultCode);
	
	return( testResult);
}

#define RECOVERY_PREP	1
#define RECOVERY_HOLD	1

string FordTRWCANABSTC::Recovery(const string &wheel, INT32 &startIndex, INT32 &stopIndex)
{
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;
	std::string testResult = BEP_TESTING_STATUS;
	std::string testResultCode = "0000";
	std::string testDescription = "Performing recovery for " + wheel;
	const std::string	prepareCmd( "Prepare"+wheel+"Recovery");
	const std::string	recoveryCmd("Actuate"+wheel+"Recovery");
	const INT32			interWheelGap = GetParameterInt( "InterWheelGapDelay");
	const INT32			dumpTime = GetParameterInt(wheel+"RecoveryPulseLength");
    //MAM 3/26/10
    float               wheelSpeeds[ 4];  //MAM 9/2/10
    int                 ii;
    //MAM 9/29/10
    bool  valveCrossSpeedCheck = GetParameterBool("PerformValveCrossSpeedCheck");

	startIndex = TagArray(wheel + "RecovStart");
	try
	{
		// Prepare for recovery
#if RECOVERY_PREP
		status = m_vehicleModule.CommandModule( prepareCmd);
		if(BEP_STATUS_SUCCESS == status)
#endif
		{
			// Wait for inter-wheel gap time
			//if( interWheelGap > 0)	Delay( interWheelGap);
            //if( interWheelGap > 0)	BposSleep( interWheelGap);
            if( interWheelGap > 0)	delay( interWheelGap);  //MAM 9/2/10

			// Open the outlet
			status = m_vehicleModule.CommandModule( recoveryCmd);
			if(BEP_STATUS_SUCCESS == status)
			{
				// Ensure open long enough
				//if( dumpTime > 0)	Delay( dumpTime);
                //if( dumpTime > 0)	BposSleep( dumpTime);
                if( dumpTime > 0)	delay( dumpTime);   //MAM 9/2/10

#if RECOVERY_HOLD
				const std::string	holdCmd("Hold"+wheel+"Recovery");
				const INT32			holdTime = GetParameterInt(wheel+"RecoveryHoldDelay");
				// If we need to hold the recovery
				if( holdTime > 0)
				{
					// Hold the recovery
					status = m_vehicleModule.CommandModule(holdCmd);
					if(BEP_STATUS_SUCCESS == status)
					{
						// Hold for desired time
						//Delay( holdTime);                        
                        //BposSleep( holdTime); //MAM 10/02/08
                        delay( holdTime); //MAM 9/2/10
					}
				}
#endif
			}
		}

		if( BEP_STATUS_SUCCESS != status)
		{
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
            Log(LOG_ERRORS, "FordTRWCANABSTC::Recovery() - CommunicationFailure \n");  //MAM 3/11/10
            //MAM 2/13/13
            m_absCommFault = true;
		}
		else
		{
			testResult = testPass;
            //stopIndex = TagArray(wheel + "RecovStop");
		}
	}
	catch( ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Recovery(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
    //MAM 1/24/11
    stopIndex = TagArray(wheel + "RecovStop"); //MAM 9/2/10

    //MAM 9/29/10 - Check based on parameter in config file because we may need to do this for trucks but not cars.
    if ((true == valveCrossSpeedCheck)&&(testPass == testResult))
    {

        //MAM 3/26/10 - Make sure each wheel is above 10 mph for the valve cross check    
        if (BEP_STATUS_SUCCESS == m_baseBrakeTool->GetISpeeds(wheelSpeeds))
        {
            for ( ii=0; ii<4; ii++)
            {
                if ( wheelSpeeds[ ii] > 10)     m_valveCrossEnd[ii] = stopIndex;
                //MAM 4/28/10 - debug
                //Log( LOG_DEV_DATA, "end index for wheel %d = %2.2f  speed = %2.2f \n", ii, m_valveCrossEnd[ii], wheelSpeeds[ ii]);
                Log( LOG_DEV_DATA, "recovery end index for wheel %d = %d  speed = %2.2f \n", ii, m_valveCrossEnd[ii], wheelSpeeds[ ii]);
            }
        }
        else
        {
            Log( LOG_DEV_DATA, "Could not get wheel speeds for recovery end index! \n");
        }

    }

	// Report the result of the test
    //MAM 9/1/10 - testing - commented out for now.
	SendSubtestResult(wheel + "Recovery", testResult, testDescription, testResultCode);

	return( testResult);
}


void FordTRWCANABSTC::Delay( int delayMs) const
{
	uint64_t	delayTime = mSEC_nSEC( delayMs);

	TimerTimeout( CLOCK_REALTIME, 0, NULL, &delayTime, NULL);
}

string FordTRWCANABSTC::FrontInletValvesOn()
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0069";
	string testDescription       = GetTestStepInfo("Description");

	// Log the entry
	Log(LOG_FN_ENTRY, "Enter FordTRWCANABSTC::FrontInletValvesOn()\n");
	// Determine if the test should be performed
	if (!ShortCircuitTestStep())
	{   // Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.CommandModule( "FrontInletValvesOn");
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves on: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch (ModuleException &excpt)
		{   // Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{   // Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Exit FordTRWCANABSTC::FrontInletValvesOn()\n");

	return testResult;
}

string FordTRWCANABSTC::RearInletValvesOn()
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0069";
	string testDescription       = GetTestStepInfo("Description");

	// Log the entry
	Log(LOG_FN_ENTRY, "Enter FordTRWCANABSTC::RearInletValvesOn()\n");
	// Determine if the test should be performed
	if (!ShortCircuitTestStep())
	{   // Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.CommandModule( "RearInletValvesOn");
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves on: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch (ModuleException &excpt)
		{   // Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{   // Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Exit FordTRWCANABSTC::RearInletValvesOn()\n");

	return testResult;
}

string FordTRWCANABSTC::AllOutletValvesOn()
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0069";
	string testDescription       = GetTestStepInfo("Description");

	// Log the entry
	Log(LOG_FN_ENTRY, "Enter FordTRWCANABSTC::AllOutletValvesOn()\n");
	// Determine if the test should be performed
	if (!ShortCircuitTestStep())
	{   // Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.CommandModule( "AllOutletValvesOn");
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves on: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch (ModuleException &excpt)
		{   // Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{   // Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Exit FordTRWCANABSTC::AllOutletValvesOn()\n");

	return testResult;
}

string FordTRWCANABSTC::AllOutletValvesOff()
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0069";
	string testDescription       = GetTestStepInfo("Description");

	// Log the entry
	Log(LOG_FN_ENTRY, "Enter FordTRWCANABSTC::AllOutletValvesOff()\n");
	// Determine if the test should be performed
	if (!ShortCircuitTestStep())
	{   // Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.CommandModule( "AllOutletValvesOff");
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves on: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch (ModuleException &excpt)
		{   // Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{   // Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "Exit FordTRWCANABSTC::AllOutletValvesOff()\n");

	return testResult;
}

