//******************************************************************************
// Description:
//      This file contains the functions necessary to perform a base brake test.
//
//===========================================================================
// Copyright (c) 1999 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ApplicationLayer/TestComponents/Brake/Common/GenericBaseBrakeTC.cpp $
// 
// 8     10/30/07 1:41p Rwiersem
// Changes for the 07103101 core release:
// 
// - Added std:: to the value parameter in CommandTestStep() to match the
// header file.
// 
// 7     2/22/07 8:49a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added a DisableForceMeter test step.
// 
// 6     1/17/07 3:12p Rwiersem
// Changes for the 07011901 core release:
// 
// - Removed external ForceThread() declaration.  It is now part of the
// class.
// - Added calls to m_baseBrakeTool->WarmInitialize() and
// GenericTC::WarmInitialize() in WarmInitialize().
// - Added a call to m_baseBrakeTool->Deactivate() in Deactivate().
// 
// 5     10/25/06 3:48p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added Deactivate() and WarmInitialize().  These methods are used
// to          keep the test component running when a new drive curve
// starts.
// 
// 4     4/13/06 3:19a Cward
// Added Coast to Stop command
//
// 3     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 2     4/29/05 8:00a Derickson
// Added Coast Test into the CommandTestStep() method
//
// 2     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 1     12/21/03 6:33p Cward
//
// 5     11/13/03 7:05p Derickso
// Updated reset method to call the base class.
//
// 4     10/29/03 6:21a Derickso
// Moved call to GenericTC::Initialize from InitializeHook()
// to Initialize().
//
// 3     10/27/03 4:10p Derickso
// Corrected recursion issues.
// Latest working version from PIT debug and testing.
//
// 1     8/21/03 8:59a Derickso
// 05WK CCRT Project
//
// 1     8/14/03 7:42a Khykin
// Initial checkin after name conversion from TestComponent to TC.
//
// 2     8/08/03 11:00a Khykin
// Updated with changes from Flatrock.
//
// 1     7/03/03 9:49a Mmcnamar
// initial check-in
//    Revision 1.5  2003/06/24 14:27:01  tenharmsel
//    Updating from FordChicago
//
//    Revision 1.2  2003/05/14 20:02:34  swieton
//    Pit conditionals working
//
//    Revision 1.1  2003/04/04 21:57:01  bereza
//    added
//
//
// 1     1/27/03 11:12a Khykin
// Initial Checkin
//
// 1     1/18/03 7:20a Khykin
// Initial Checkin
//
// 12    11/18/02 11:59a Khykin
// Updating test reporting.
//
// 11    11/12/02 8:48a Khykin
// Performing checkin of latest system due to loss of vehicle.
//
// 10    11/11/02 3:05p Khykin
// Updated the basebrake analysis.
//
// 9     11/04/02 6:34a Khykin
// Updated the component with methods to manage the force target.  This
// included a background thread to update the forces and methods to update
// the target and domain values.
//
// 8     10/28/02 6:27a Khykin
// Updating test reporting.
//
// 7     10/21/02 11:12a Khykin
// Updated the name of the BrakeBalanceTest to BalanceTest.
//
// 6     9/10/02 11:26a Khykin
// Updating the SendSubtestResultWithDetail useage.
//
// 5     8/23/02 8:03a Kh
// Synchronizing the pit machine with VSS.
//
// 4     5/24/02 1:48p Kh
// Updating due to addition of system tags and debugging.
//
// 3     3/25/02 6:18p Kh
// Updating due to changes in the base classes.
//
// 2     3/06/02 1:57p Kh
// Updating for changes due to debugging on the machine.
//
// 1     2/06/02 12:15a Kh
// Initial checkin.
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericBaseBrakeTC.h"

//-------------------------------------------------------------------------------------------------
GenericBaseBrakeTC::GenericBaseBrakeTC() : GenericTC()
{
	m_baseBrakeTool = new BaseBrakeTool(this);
}

//-------------------------------------------------------------------------------------------------
GenericBaseBrakeTC::~GenericBaseBrakeTC()
{
	// Delete the base brake utility object
	delete m_baseBrakeTool;
	Log(LOG_FN_ENTRY, "Base Brake Destructor complete\n");
}

//-------------------------------------------------------------------------------------------------
void GenericBaseBrakeTC::Initialize(const XmlNode *config)
{
	try
	{
		Log(LOG_FN_ENTRY, "%s: GenericBaseBrakeTC::Initialize - Enter\n", GetComponentName().c_str());
		// Call the base class to do its initialize thig
		GenericTC::Initialize(config);
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
}

//-------------------------------------------------------------------------------------------------
const void GenericBaseBrakeTC::InitializeHook(const XmlNode *config)
{
	// Initialize the base component
	Log(LOG_FN_ENTRY, "Initializing The GenericBaseBrakeTC Component\n");

	Log(LOG_FN_ENTRY, ((GetComponentName() + ".InitializeHook: ENTER")).c_str());
	// launch the force thread
	m_baseBrakeTool->LaunchForceThread();

	Log(LOG_FN_ENTRY, ((GetComponentName() + ".InitializeHook: Exit")).c_str());

	Log(LOG_FN_ENTRY, "Done Initializing GenericBaseBrakeTC Component\n");
}

//-------------------------------------------------------------------------------------------------
void GenericBaseBrakeTC::WarmInitialize()
{
	// Re-initialize the base brake tool
	m_baseBrakeTool->WarmInitialize();

	GenericTC::WarmInitialize();
}

//-------------------------------------------------------------------------------------------------
void GenericBaseBrakeTC::Deactivate()
{
	// Shutdown the base brake tool
	m_baseBrakeTool->Deactivate();
	GenericTC::Deactivate();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::CommandTestStep(const std::string &value)
{
	string status = BEP_ERROR_RESPONSE;		// the status of the commanded test step

	try
	{
		// if the component status is ok
		if(StatusCheck() != BEP_STATUS_ABORT)
		{
			// get the name of the sequenced test step
			string step = GetTestStepName();

			if(step == "Initialize")
				status = TestStepInitialize();
			else if(step == "AccelerateToBrakeSpeed")
				status = TestStepAccelerate();
			else if((step == "Coast") || (step == "CoastToStop"))
				status = m_baseBrakeTool->TestStepCoast();
			else if(step== "DragTest")
				status = TestStepDrag();
			else if(step == "AnalyzeDragTest")
				status = TestStepAnalyzeDrag();
			else if(step == "BrakeTest")
				status = TestStepBaseBrake();
			else if(step == "AnalyzeBrakeTest")
				status = TestStepAnalyzeBaseBrake();
			else if(step == "AnalyzeBalance")
				status = TestStepBalance();
			// Analyze stopping distance - Vehicle must be at zero speed before sequencing this test.
			else if(!step.compare("AnalyzeStoppingDistance"))	status = m_baseBrakeTool->AnalyzeStoppingDistance();
			else if(step == "BrakeToStop")			   // else if "BrakeToStop" sequenced
				status = TestStepBrakeToStop();
			else if(step == "FinishUp")
				status = TestStepFinishUp();
			// Disable the brake force meter
			else if(step == "DisableForceMeter")
			{	// Disable force updates and deactivate the force meter
				SystemWrite(GetDataTag("BrakeActive"), 0);
				status = BEP_STATUS_SUCCESS == m_baseBrakeTool->UpdateTarget(false) ? testPass : testFail;
			}
			// Check for excessive brake force
			else if(step == "TireChunkingTest")				 status = m_baseBrakeTool->ExcessiveBrakeForceCheck();
			else if(step == "ExcessiveBrakeForceCheck")		 status = m_baseBrakeTool->ExcessiveBrakeForceCheck();
			// Accelerate to dynamic park brake test speed
			else if("AccelerateToParkBrakeSpeed" == step)	 status = m_baseBrakeTool->TestStepAccelerateToParkBrakeSpeed();
			// Perform dynamic park brake test
			else if("DynamicParkBrakeTest" == step)			 status = m_baseBrakeTool->TestStepDynamicParkBrake();
            // Accelerate to brake test speed
            else if (step == "DisplaySpeedTimeGraph")         status = m_baseBrakeTool->DisplaySpeedTimeGraph();
            // Accelerate to brake test speed
            else if (step == "RemoveSpeedTimeGraph")          status = m_baseBrakeTool->RemoveSpeedTimeGraph();
			// Analyze dynamic park brake test
			else if("AnalyzeDynamicParkBrake" == step)		 status = m_baseBrakeTool->AnalyzeDynamicParkBrake();
			else
				status = GenericTC::CommandTestStep(value);
		}
		else
			status = testSkip;
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "%s CommandTestStep %s Exception: %s\n",
			GetComponentName().c_str(), GetTestStepName().c_str(), e.what());
	}

	return(status);
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepInitialize (void)
{
	return m_baseBrakeTool->TestStepInitialize();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepAccelerate(void)
{
	return m_baseBrakeTool->TestStepAccelerate();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepDrag (void)
{
	return m_baseBrakeTool->TestStepDrag();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepAnalyzeDrag (void)
{
	return m_baseBrakeTool->TestStepAnalyzeDrag();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepBaseBrake (void)
{
	return m_baseBrakeTool->TestStepBaseBrake();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepAnalyzeBaseBrake (void)
{
	return m_baseBrakeTool->TestStepAnalyzeBaseBrake();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepBalance (void)
{
	return m_baseBrakeTool->TestStepBalance();
}

//-------------------------------------------------------------------------------------------------
const std::string GenericBaseBrakeTC::TestStepBrakeToStop(void)
{
	return m_baseBrakeTool->TestStepBrakeToStop();
}

//-------------------------------------------------------------------------------------------------
const string GenericBaseBrakeTC::TestStepFinishUp (void)
{
	return m_baseBrakeTool->TestStepFinishUp();
}

//-------------------------------------------------------------------------------------------------
void GenericBaseBrakeTC::Reset(void)
{	// Have the tool do it's reset thing
	m_baseBrakeTool->Reset();
	// Call the base clas to complete the reset
	GenericTC::Reset();
}

void GenericBaseBrakeTC::Abort(void)
{//Abort Called return motors to boost mode
    Log(LOG_FN_ENTRY,"GenericBaseBrakeTC Abort Called! Resetting motor mode\n");
    // Return the speed set point to 0
    for(UINT8 wheel = 0; wheel < GetRollerCount(); wheel++)
    {   // Clear out any torque or speed values
        m_MotorController.Write(rollerName[wheel]+"TorqueValue", "0.00", false);
        m_MotorController.Write(rollerName[wheel]+"SpeedValue", "0.00", true);
    }
    // Set each roller to boost mode
    for(UINT8 wheel = 0; wheel < GetRollerCount(); wheel++)
    {
        m_MotorController.Write(rollerName[wheel]+"MotorMode", BOOST_MODE, true);
    }


    Log(LOG_FN_ENTRY,"GenericBaseBrakeTC Abort Called! DisplaySpeedTimeGraph Parameter Value: %s \n",GetParameter("DisplaySpeedTimeGraph").c_str());
    //Call to remove the Speed time Graph during abort
    Log(LOG_FN_ENTRY,"GenericBaseBrakeTC Abort Called! Removing Speed Time Graph\n");
    m_baseBrakeTool->RemoveSpeedTimeGraph();
    Log(LOG_FN_ENTRY,"GenericBaseBrakeTC Abort Called! Removed Speed Time Graph\n");
    

    // Call the base class to complete the abort processing
    GenericTC::Abort();
    
}
