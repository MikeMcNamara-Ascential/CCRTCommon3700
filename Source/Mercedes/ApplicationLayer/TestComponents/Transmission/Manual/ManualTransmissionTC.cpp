//*************************************************************************
// FILE:
//    $Header: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Manual/ManualTransmissionTC.cpp 1     2/22/05 3:28p Derickso $
//
// FILE DESCRIPTION:
//  42RE Transmission Test Component.
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /CCRT/Common/ApplicationLayer/TestComponents/Transmission/Manual/ManualTransmissionTC.cpp $
// 
// 1     2/22/05 3:28p Derickso
// New transmission type.
// 
// 2     12/07/04 12:28p Mkelly
// 
// 1     11/08/04 1:17p Mkelly
// 
// 1     11/01/04 3:26p Mkelly
// 
// 1     8/04/04 12:38p Derickso
// Dcx common transmission test components
// 
// 2     1/29/04 8:53a Derickso
// Updated 2-3 shift test to latch data when an RPM drop is detected whlie
// the vehicle is in second gear and a steady throttle is applied.
// 
// 1     1/27/04 9:35a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 3     1/20/04 5:49a Derickso
// Moved loading of external fault list to GenericTC class.
// NOTE:  Faults node in test component config file is no longer
// supported.
// 
// 2     1/10/04 10:20a Derickso
// Latest updates from development testing.
// 
//*************************************************************************
#include "ManualTransmissionTC.h"
//*************************************************************************

ManualTransmissionTC::ManualTransmissionTC() : GenericTC()
{	// Nothing special to do here
}
	
ManualTransmissionTC::~ManualTransmissionTC()
{	// Nothing special to do here
}

void ManualTransmissionTC::Initialize(const string &fileName)
{	// call the base class to perfrom the initialization
	GenericTC::Initialize(fileName);
}

void ManualTransmissionTC::Initialize(const XmlNode *config) 
{
	try
	{
		//Iniitalize base class
		GenericTC::Initialize(config);
		
		// Call the initialize complete method
		Log(LOG_DEV_DATA, (GetComponentName() + ": calling InitializationComplete()").c_str());
		InitializationComplete();
		
		// Reset the data tags
		ResetSystemTags();	
		
		Log(LOG_FN_ENTRY, (GetComponentName() + ": Initialize: EXIT").c_str());
	}
	catch (XmlException &e)
	{
		Log(LOG_ERRORS, "XML Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
	catch (BepException &e)
	{
		Log(LOG_ERRORS, "Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
}

const string ManualTransmissionTC::CommandTestStep(const string &value)
{
	string testResult = testFail;
	// Determine if the next test step can be sequenced
	if (StatusCheck() == BEP_STATUS_SUCCESS)
	{	// System is fine to continue testing
		try
		{	// Get the next test step to perform
			Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
			// Check the PRNDL Status
			if (GetTestStepName() == "AccelerateInReverse")
			{
				testResult = GenericTC::AccelerateInReverse(
								GetParameterFloat("ReverseSpeedMinimum"), 
								GetParameterFloat("ReverseSpeedMaximum"), 
								GetTestStepInfoInt("ScanDelay"));
				SendSubtestResult("ReverseGearObserved", testResult, "Reverse Gear Observed", "0000");
			}
			else if (GetTestStepName() == "ShiftThrough")
			{
				testResult = GenericTC::AccelerateToTestSpeed(
								GetParameterFloat("TopSpeedMinimum"), 
								GetParameter("TopSpeedMinimum") + " " + GetParameter("TopSpeedMaximum"), 
								GetTestStepInfoInt("ScanDelay"),
								true,
								"ShiftThrough");
				if(testPass == testResult)
				{
					testResult = OperatorPassFail("ShiftThroughPassFail",GetTestStepInfoInt("Timeout"));
				}
				SendSubtestResult("ShiftThrough", testResult, "Accelerated through all gears", "0000");
			}
			// Check Transmission shifting
			else if (GetTestStepName() == "CheckShifting")
			{
				testResult = OperatorPassFail(GetDataTag("CheckShiftingPrompt"), GetTestStepInfoInt("Timeout"));
				SendSubtestResult("ReverseGearObserved", testResult, "Reverse Gear Observed", "0000");
			}
			else if (GetTestStepName() == "FinishUp")
			{
				testResult = testPass;
				SendOverallResult(testResult);
			}
			// No defined test step, try the base class
			else      testResult = GenericTC::CommandTestStep(value);
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
