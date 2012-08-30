//******************************************************************************
// Description:
//  Generic Emissions Test Component.
//
//===========================================================================
// Copyright (c) 2003- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/TransferCase/Common/GenericTcaseTC.cpp $
// 
// 5     10/30/07 2:16p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added std:: to the value parameter of CommandTestStep() to match the
// header file.
// 
// 4     9/01/04 9:22p Cward
// Added deletion of t-case tool in destructor. Updated to new header
// block comment format.
//
// 3     2/26/04 6:32p Cward
// Changes for Feb 26, 2004.
//
// 2     1/23/04 9:24a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:33p Cward
//
// 1     10/28/03 7:54a Derickso
// Created.
//
// 9     9/30/03 7:19p Bbarrett
// Change and update methods
//
// 8     9/29/03 6:03p Bbarrett
// Continue development
//
// 7     9/17/03 2:59p Bbarrett
// Add get ratio (samples / time).
//
// 6     9/11/03 3:44p Bbarrett
// Clean up code. Change JavaDoc comments.  Version 1.0 done.  Not
// compiled yet.
//
// 5     9/10/03 1:18p Bbarrett
// Add comments.
//
// 4     9/09/03 4:41p Bbarrett
// Add axle ratio methods. Not complete yet
//
// 3     8/29/03 2:58p Bbarrett
// Add more shift test. reconfigure test.  Not complete yet.
//
// 2     8/25/03 5:21p Bbarrett
// Add in ShiftToLow method.
//
// 1     8/22/03 5:31p Bbarrett
// Initial work on Tcase
//
// 6     8/13/03 4:22p Derickso
// Renamed to follow new naming convention.
//
// 5     8/12/03 3:45p Derickso
// Split implementation out into cpp file.
//
// 4     7/08/03 10:23a Derickso
// Updated parameter macros to be more readable.
// Restructured data in the config files.
// Compiles, not tested.
//
// 3     6/09/03 8:05a Derickso
// Corrected compiler errors.
// Compiles, not tested.
//
// 2     5/28/03 9:14a Derickso
// Converted to template.
//
// 1     5/27/03 3:31p Derickso
// Periodic backup.
//
// 1     5/23/03 5:02p Derickso
// Periodic backup
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericTcaseTC.h"

//-----------------------------------------------------------------------------
GenericTcaseTC::GenericTcaseTC() : GenericTC()
{
	m_tcaseTool = new TcaseTool(this);
}

//-----------------------------------------------------------------------------
GenericTcaseTC::~GenericTcaseTC()
{
	// Delete the base brake utility object
	if(m_tcaseTool != NULL)
	{
		delete m_tcaseTool;
	}
	m_tcaseTool = NULL;
	Log(LOG_FN_ENTRY, "Generic Tcase Destructor complete\n");
}

//-----------------------------------------------------------------------------
void GenericTcaseTC::Initialize(const XmlNode *config)
{
	try
	{
		Log(LOG_FN_ENTRY, "%s: GenericTcaseTC::Initialize - Enter\n", GetComponentName().c_str());
		// Call the child class initialize hook
		Log(LOG_DEV_DATA, "%s: calling InitializeHook()\n", GetComponentName().c_str());
		InitializeHook(config);
		// Call the initialize complete method
		Log(LOG_DEV_DATA, "%s: calling InitializationComplete()\n",GetComponentName().c_str());
		InitializationComplete();
		Log(LOG_FN_ENTRY, "%s: Initialize: EXIT",GetComponentName().c_str());
	}
	catch(XmlException &e)
	{
		Log(LOG_ERRORS, "XML Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "Error Initializing Component %s: %s\n", GetProcessName().c_str(), e.what());
	}
	// Store non t-case types
	try
	{
		SetTcaseEquipped(false);  // Initialize the T-Case status to true, and set it to false if it is not equipped
		Log(LOG_FN_ENTRY, "%s: Initialize: T-Case Equipped #1\n",GetComponentName().c_str());
		m_tcaseTypes.DeepCopy(config->getChild("Setup/TcaseTypes")->getChildren());
		Log(LOG_FN_ENTRY, "%s: Initialize: T-Case Equipped #2\n",GetComponentName().c_str());
		for(XmlNodeMapItr iter = m_tcaseTypes.begin(); iter != m_tcaseTypes.end(); iter++)
		{
			Log(LOG_DEV_DATA, "Config Output: %s; Subscription Output: %s\n", iter->second->getValue().c_str(), SystemRead("Traction").c_str());
			if(iter->second->getValue() == SystemRead("Traction"))
			{
				Log(LOG_DEV_DATA, "This truck has powered front wheels.  Register the T-Case test.\n");
				SetTcaseEquipped(true);
			}
		}
		Log(LOG_FN_ENTRY, "%s: Initialize: T-Case Equipped #3\n",GetComponentName().c_str());
	}
	catch(XmlException &excpt)
	{	// Error loading sensor register table
		Log(LOG_ERRORS, "XmlException loading T-Case types data: %s\n", excpt.GetReason());
		Log(LOG_FN_ENTRY, "Considering this T-Case exception, assume this is a non-DCBR test; m_tcaseEquipped == TRUE.\n");
		m_tcaseTypes.clear(true);
		SetTcaseEquipped(true);	 // This is so that non-DCBR trucks will always run the test steps.
	}
	Log(LOG_FN_ENTRY, "%s: Initialize: EXIT",GetComponentName().c_str());
}

const string GenericTcaseTC::Register(void)
{
	std::string status;			// overall status of registering
	// until the component is up and running, sit here and wait
	m_semInitialized.CriticalSectionEnter();
	// if the component is equipped with speed control, register
	if(IsTcaseEquipped())
	{	// Indiate that the test has started
		Log(LOG_FN_ENTRY, "Testing Generic T-Case\n");
		UpdateTestStatus(TEST_STATUS_IN_PROGRESS);
	}
	else
	{	// Indicate that the test is not started
		Log(LOG_FN_ENTRY, "Not testing Generic T-Case\n");
		UpdateTestStatus(BEP_TEST_NOT_STARTED);
	}
	// Return the result of the base class register
	return(BepServer::Register());
}

//-----------------------------------------------------------------------------
const void GenericTcaseTC::InitializeHook(const XmlNode *config)
{	// Initialize the base component
	Log(LOG_FN_ENTRY, "Initializing The GenericTcaseTC Component\n");

	GenericTC::Initialize(config);

	Log(LOG_FN_ENTRY, ((GetComponentName() + ".InitializeHook: Exit")).c_str());
	return;
}

//-----------------------------------------------------------------------------
const string GenericTcaseTC::CommandTestStep(const string &value)
{
	string status;

	try
	{	// Get the test step name to perform
		if(IsTcaseEquipped())
		{
			string step = GetTestStepName();
			Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Entering test step %s\n",
				GetComponentName().c_str(), value.c_str(), step.c_str());
			// Check the status to ensure it is alright to run this test step
			if(StatusCheck() != BEP_STATUS_SUCCESS)
			{		// Not OK to run the test step
				UpdateResult(StatusCheck(), status);
				Log(LOG_ERRORS, (GetComponentName() + ".CommandTestStep: StatusCheck() failed: %s\n").c_str(),
					status.c_str());
				status = testSkip;	 // Set result to Skip since this step will not be performed
				SendTestResult(status, GetTestStepInfo("Description"));
			}
			// Verify the operator can shift the T-case.
			else if(step.find("CheckShift") != step.npos)			   status = CheckTcaseShift();
			// Verify Axle Ratio with in limits at desired speed.
			else if(GetTestStepName() == "CheckAxleRatio")
			{
				if(GetParameterBool("AxleRatioSkipPreconditions"))
				{
					float ratio = 0;
					status = m_tcaseTool->GetAxleRatio(ratio);
				}
				else
				{
					status = CheckAxleRatio();
				}
			}
			// Check the axle balance
			else if(step.find("AxleBalance") != step.npos)			   status = CheckAxleBalance();
			// Prompt operator to accelerate to test speed
			else if(GetTestStepName() == "AccelerateToTractionSpeed")  status = TcaseAccelerateToSpeed();
			// Prompt operator to coast to stop
			else if(GetTestStepName() == "CoastToStop")				   status = (CheckZeroSpeed() ? testPass : testFail); /*status = CoastToStop();*/
			// Prompt operator to set test result to Pass or Fail
			else if(GetTestStepName() == "GetTractionPassFail")		   status = GetTcasePassFail();
			// Finish up any needed testing
			else if(GetTestStepName() == "FinishUp")
			{
				status = FinishUp();
				SendOverallResult(status);
			}
			// Call GenericTCTemplate for addition command steps
			else
				status = GenericTC::CommandTestStep(value);
		}
		else
		{
			Log(LOG_DEV_DATA, "Vehicle not equipped with a T-Case, skipping test step: %s", GetTestStepName().c_str());
			status = testSkip;
		}
	}
	catch(BepException &e)
	{
		Log(LOG_DEV_DATA, "%s CommandTestStep %s Exception: %s\n",
			GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n", GetComponentName().c_str(), value.c_str(), status.c_str());
	return(status);
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::CheckTcaseShift ( void )
{	// Determine if we need to check the t-case shift
	bool performShift = true;
	if(GetParameterBool("CheckIf4wdEquipped"))
	{
		performShift = !SystemRead(GetDataTag("FourWdTag")).compare(GetParameter("ValidFourWdValue"));
	}
	return performShift ? m_tcaseTool->CheckTcaseShift() : testSkip;
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::PreconditionsForShift ( void )
{
	return m_tcaseTool->PreconditionsForShift ();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::CheckVehicleSpeed ( void )
{
	return m_tcaseTool->CheckVehicleSpeed ();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::ShiftTransmission ( const string transGear )
{
	return m_tcaseTool->ShiftTransmission (transGear );
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::TurnKeyOnOff ( const string keyPosition )
{
	return m_tcaseTool->TurnKeyOnOff (keyPosition );
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::EngineOnOff ( const string engineRun )
{
	return m_tcaseTool->EngineOnOff(engineRun );
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::CheckThrottlePosition ( const string throttlePosition )
{
	return m_tcaseTool->CheckThrottlePosition(throttlePosition );
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::ShiftTcase ( void )
{
	return m_tcaseTool->ShiftTcase();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::VerifyTcaseShift ( const string &verifyMethod )
{
	return m_tcaseTool->VerifyTcaseShift(verifyMethod );
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::TcaseCheckDriver ( void )
{
	return m_tcaseTool->TcaseCheckDriver();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::TcasePromptDriver ( void )
{
	return m_tcaseTool->TcasePromptDriver ();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::CheckAxleRatio ( void )
{	// Determine if we need to check the build data 
	bool performAxleRatio = true;
	if(GetParameterBool("CheckIf4wdEquipped"))
	{
		performAxleRatio = !SystemRead(GetDataTag("FourWdTag")).compare(GetParameter("ValidFourWdValue"));
	}
	return performAxleRatio ? m_tcaseTool->CheckAxleRatio() : testSkip;
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::CheckAxleBalance ( void )
{
	return m_tcaseTool->CheckAxleBalance ();
}

//-----------------------------------------------------------------------------
string GenericTcaseTC::GetAxleRatio ( float &ratio )
{
	return m_tcaseTool->GetAxleRatio (ratio );
}

//-----------------------------------------------------------------------------
inline string GenericTcaseTC::FinishUp(void)
{
	return(testPass);
}

string GenericTcaseTC::TcaseAccelerateToSpeed( void )
{
	return m_tcaseTool->TcaseAccelerateToSpeed ();
}

string GenericTcaseTC::GetTcasePassFail( void )
{
	return m_tcaseTool->GetTcasePassFail ();
}

string GenericTcaseTC::CoastToStop( void )
{
	return m_tcaseTool->CoastToStop ();
}

bool& GenericTcaseTC::IsTcaseEquipped(void)
{
	Log("Get T-Case equipped: %s\n", m_tcaseEquipped ? "TRUE" : "FALSE");
	return m_tcaseEquipped;
}

void GenericTcaseTC::SetTcaseEquipped(const bool &isEquipped)
{
	Log("Set T-Case equipped to %s\n", isEquipped ? "TRUE" : "FALSE");
	m_tcaseEquipped = isEquipped;
}
//*************************************************************************
//end of GenericTcaseTC_cpp
