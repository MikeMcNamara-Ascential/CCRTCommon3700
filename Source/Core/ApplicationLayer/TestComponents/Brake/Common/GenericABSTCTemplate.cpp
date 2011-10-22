//******************************************************************************
// Description:
// This class extends the functionality of the TestComponent to be able to
// retrieve force, acceleration, speed, ... arrays for performing analysis.
//
//==============================================================================
// Copyright (c) 2004-2007 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CCRT-cmake/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Brake/Common/GenericABSTCTemplate.cpp $
// 
// 2     3/07/08 9:55a Derickso
// Added parameters to allow prompting the operator to maintain speed
// during sensor testing.
// 
// 1     11/01/07 2:07p Derickso
// Added optional delay after commanding the pump motor to run.
// Added option to always reset the module even if ShortCircuitTestStep
// returns true.
// 
// 2     10/01/07 1:59p Derickso
// Added Option to always reset module.  Also added post pump run delay.
// 
// 16    5/01/07 11:03a Rwiersem
// Changes for the 07050201 core release:
// 
// Broke single lines that were longer than the viewable screen area into
// multiple lines.
// 
// 15    1/17/07 3:11p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_isESPEquipped.
// - Added calls to m_baseBrakeTool->Deactivate() and 
//   GenericTCTemplate<VehicleModuleType>::Deactivate() in Deactivate().
// - Added a call to m_baseBrakeTool->WarmInitialize() in WarmInitialize().
// - Added ReloadConfiguration().
// - Added a check to see if the vehicle is equipped with ESP in 
//   DetermineSystemConfiguration().
// - Added SetESPEquipped() and IsESPEquipped().
// 
// 14    10/25/06 3:47p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Changed m_reduxRecovIndex to a <vector>
//     - Added Deactivate() and WarmInitialize().  These methods are used to 
//       keep the test component running when a new drive curve starts.
// 
// 13    6/07/06 5:33p Cward
// Propagating the determination of whether the calibrate gsensor test
// should be run or not to the generic ABS level.
//
// 12    5/08/06 7:06p Cward
// Add ability to wait for ABS sensor data to catch up to the actual roll
// speeds. Added wait for stable sensor speeds method. Updated Evaluate
// ABS, and Analyze Sensor Cross methods
//
// 11    4/13/06 3:18a Cward
// Checked in changes below
//
// 2     3/20/06 10:44a Bmeinke
// Changed hard coded unit strings with new units strings from GenericTC.h
// Added method GetAbsReferenceForce() to allow FordTRW ABS to use 'n'
// samples just prior to sending the reduction command as the reference
// force for ABS calculations
//
// 2     2/24/06 1:15p Bmeinke
// Added new GetAbsReferenceForce() method. Returns the brake force value
// that should be used as the base brake reference force when calculating
// ABS reduction/recovery percentages
//
// 10    11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 9     5/25/05 8:55p Cward
// Updated AnalyzeValveCross and updated the structure of the Input Server
//
// 8     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 7     4/28/05 8:00a Derickson
// Added Coast Test into the CommandTestStep() method
//
// 7     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 8     4/07/05 5:29p Gswope
// Added methods AllInletValvesOff() and AllInletValvesOn().
//
// 7     4/04/05 5:29p Derickso
// Customer request that we have the option to not fail if the
// module responds with a negative response when we run the
// pump motor.  Updated the code with optional parameter for
// this in RunPumpMotor()
//
// 6     3/30/05 5:29p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 3     1/31/05 9:09a Bmeinke
// Inside ExcessiveBrakeForceCheck(): Read
// m_baseBrakeTool->GetBrakeStart() into a local variable instead of
// calling GetBrakeStart() multiple times
//
// 2     12/22/04 8:03p Bmeinke
// Removed the drag and machine loss compensation from
// AnalyzeReductionForces() and AnalyzeRecoveryForces() methods
// Fixed the call to min_element() and max_element() to actually look up
// to AND INCLUDING the final redcution/recovery sample
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     9/29/04 10:11p Cward
// Added stop pump motor command
//
// 2     8/18/04 11:09p Cward
// added include to resovle compilation errors
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "GenericABSTCTemplate.h"
#include "DataAnalysis.h"
#include <math.h>

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericABSTCTemplate<VehicleModuleType>::GenericABSTCTemplate() :
GenericTCTemplate<VehicleModuleType>(), m_isFourChannelSystem(false), m_absStartIndex(0),
m_absEndIndex(0),m_isESPEquipped(false), m_evalAbsResult(BEP_TESTING_RESPONSE)
{	// Create a BaseBrakeTool utility object
	m_baseBrakeTool = new BaseBrakeTool(this);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericABSTCTemplate<VehicleModuleType>::~GenericABSTCTemplate()
{	// Delete the base brake utility object
	Log(LOG_DEV_DATA, "Delete base brake tool ...\n");
	if(m_baseBrakeTool != NULL)	delete m_baseBrakeTool;
	m_baseBrakeTool = NULL;
	// Clear the abs firing order
	Log(LOG_DEV_DATA, "Clear 3 channel and 4 channel firing orders...\n");
	m_abs3ChannelFiringOrder.clear(true);
	m_abs4ChannelFiringOrder.clear(true);
	m_abs6ChannelFiringOrder.clear(true);  
	// Get rid of the indices
	//Log(LOG_DEV_DATA, "Delete Reduction/Recovery index...\n");
	//if (m_reduxRecovIndex != NULL) delete m_reduxRecovIndex;
	//m_reduxRecovIndex = NULL;
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate destructor complete!\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::Deactivate(void)
{
	m_baseBrakeTool->Deactivate();
	GenericTCTemplate<VehicleModuleType>::Deactivate();
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::WarmInitialize(void)
{
	Log( LOG_FN_ENTRY, "Enter GenericABSTCTemplate::WarmInitialize()\n");
	// Call the base class to complete initialization
	GenericTCTemplate<VehicleModuleType>::WarmInitialize();
	// launch the force thread
	m_baseBrakeTool->WarmInitialize();

	try
	{
		// Check if vehicle is equipped with G_Sensor Component
		// and set member variable
		CheckForValidGSensorType();
	}
	catch(...)
	{	//error loading Valid4x4GSensorTypes
		try
		{	// Error loading Valid ETC Speed Control Types
			Log(LOG_ERRORS, "XmlException loading Valid4x4GSensorTypes, looking for NoTransferCaseTestCode\n");
			SetGSensorEquipped(SystemRead(GetDataTag("TransferCaseTestCode")).compare(GetDataTag("NoTransferCaseTestCode")));
		}
		catch(...)
		{	// Default to not ETC type
			Log(LOG_ERRORS, "XmlException loading NoTransferCaseTestCode, default to Not G-Sensor Equipped\n");
			SetGSensorEquipped(false);
		}
	}
	// Clear the reduction recovery indices
	for(INT32 wheelIndex = 0; (UINT16)wheelIndex < MAXWHEELS; wheelIndex++)
	{
		m_reduxRecovIndex[wheelIndex].recoveryEnd = 0;
		m_reduxRecovIndex[wheelIndex].recoveryStart = 0;
		m_reduxRecovIndex[wheelIndex].reductionEnd = 0;
		m_reduxRecovIndex[wheelIndex].reductionStart = 0;
	}
	m_evalAbsResult = BEP_TESTING_RESPONSE;
	Log( LOG_FN_ENTRY, "Exit GenericABSTCTemplate::WarmInitialize()\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericABSTCTemplate<VehicleModuleType>::CommandTestStep(const string &value)
{
	// Status of the test step
	string status;

	try
	{	// Get the test step we're on
		string step = GetTestStepName();
		// Log it
		Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Entering test step %s\n",
			GetComponentName().c_str(), value.c_str(), step.c_str());
		// Ensure the status is good before we sequence a test step
		if(StatusCheck() != BEP_STATUS_SUCCESS)
		{ // IF the overall status is bad
			UpdateResult(StatusCheck(), status);
			Log(LOG_ERRORS, "%s.CommandTestStep: StatusCheck() failed: %s\n", GetComponentName().c_str(), status.c_str());
			status = testSkip;	 // Set result to skip since this step will not be performed
			SendTestResult(status, GetTestStepInfo("Description"));
		}
		// Run the setup method
		else if(step == "Setup")						 status = Setup();
		// Determine the system configuration - 3 channel or 4 channel system
		else if(step == "DetermineSystemConfiguration")	 status = DetermineSystemConfiguration();
		// Perform the G-sensor calibration for 4x4 vehicles
		else if(step == "CalibrateGSensor")				 status = CalibrateGSensor();
		// Check for G-sensor cross
		else if(step == "CheckGSensorCross")			 status = CheckForGSensorCross();
		// Check the vehicle for the presence of a G-switch
		else if(step == "CheckForGSwitch")				 status = CheckForGSwitch();
		// Check to ensure the fluid fill switch is off
		else if(step == "CheckFluidFillSwitch")			 status = CheckFluidFillSwitch();
		// Program the tire size into the module if required
		else if(step == "ProgramTireSize")				 status = ProgramTireSize();
		// Perform a power-on reset of the module
		else if(step == "ResetModule")					 status = ResetModule();
		// Run the pump motor to ensure fluid is distributed properly
		else if(step == "RunPumpMotor")					 status = RunPumpMotor();
		// Stop running the pump motor
		else if(step == "StopPumpMotor")				 status = StopPumpMotor();
		// Run the brake switch test
		else if(step == "BrakeSwitchTest")				 status = BrakeSwitchTest();
		// Check for the brake switch in the On position
		else if(step == "BrakeSwitchOnTest")			 status = BrakeSwitchTest(GetDataTag("SwitchOn"));
		// Check for the brake switch in the Off position
		else if(step == "BrakeSwitchOffTest")			 status = BrakeSwitchTest(GetDataTag("SwitchOff"));
		// Perform a high speed wheel sensor validation test
		else if(step == "HighSpeedSensorTest")			 status = HighSpeedSensorTest();
		// Perform a low speed wheel sensor validation test
		else if(step == "LowSpeedSensorTest")			 status = LowSpeedSensorTest();
		// Perform a low speed wheel sensor validation test
		else if(step == "ZeroSpeedSensorTest")			  status = SensorTest();
		// Dump and isolate brake force at all wheels
		else if(step == "DumpAndIsolateAllWheels")		 status = DumpAndIsolateAllWheels();
		// Perform the ABS reduction/recovery sequence
		else if(step == "ValveFiringTest")				 status = ValveFiringTest();
		// Recover brake force at all wheels
		else if(step == "RecoverAllWheels")				 status = RecoverAllWheels();
		// Take module out of roll test mode
		else if(step == "StopRollTestMode")				 status = StopRollTestMode();
		// Evaluate the ABS reduction/recovery results
		else if(step == "EvaluateABS")					 status = EvaluateABS();
		// Analyze the wheel speed data for wheel speed sensor cross
		else if(step == "AnalyzeSensorCross")			 status = AnalyzeSensorCross();
		// Initialize the Rolls test status in the module to fail
		else if(step == "UpdateShipStatusFail")			status = UpdateShipStatus(testFail);
		// Update the Rolls test status in the module
		else if(step == "UpdateShipStatus")
		{
			status = UpdateShipStatus(GetOverallResult() == testPass ? testPass : testFail);
		}
		// Force the ABS test to complete - Development testing only
		else if(step == "SetShipStatusPass")			status = UpdateShipStatus(testPass);
		// Reduce the left front wheel
		else if(step == "FireLFReduction")				 status = Reduction(rollerName[LFWHEEL],
																			m_reduxRecovIndex[LFWHEEL].reductionStart,
																			m_reduxRecovIndex[LFWHEEL].reductionEnd);
		// Reduce the right front wheel
		else if(step == "FireRFReduction")				 status = Reduction(rollerName[RFWHEEL],
																			m_reduxRecovIndex[RFWHEEL].reductionStart,
																			m_reduxRecovIndex[RFWHEEL].reductionEnd);
		// Reduce the left rear wheel
		else if(step == "FireLRReduction")				 status = Reduction(rollerName[LRWHEEL],
																			m_reduxRecovIndex[LRWHEEL].reductionStart,
																			m_reduxRecovIndex[LRWHEEL].reductionEnd);
		// Reduce the right rear wheel
		else if(step == "FireRRReduction")				 status = Reduction(rollerName[RRWHEEL],
																			m_reduxRecovIndex[RRWHEEL].reductionStart,
																			m_reduxRecovIndex[RRWHEEL].reductionEnd);
		// Recover the left front wheel
		else if(step == "FireLFRecovery")				 status = Recovery(rollerName[LFWHEEL],
																		   m_reduxRecovIndex[LFWHEEL].recoveryStart,
																		   m_reduxRecovIndex[LFWHEEL].recoveryEnd);
		// Recover the right front wheel
		else if(step == "FireRFRecovery")				 status = Recovery(rollerName[RFWHEEL],
																		   m_reduxRecovIndex[RFWHEEL].recoveryStart,
																		   m_reduxRecovIndex[RFWHEEL].recoveryEnd);
		// Recover the left rear wheel
		else if(step == "FireLRRecovery")				 status = Recovery(rollerName[LRWHEEL],
																		   m_reduxRecovIndex[LRWHEEL].recoveryStart,
																		   m_reduxRecovIndex[LRWHEEL].recoveryEnd);
		// Recover the right rear wheel
		else if(step == "FireRRRecovery")				 status = Recovery(rollerName[RRWHEEL],
																		   m_reduxRecovIndex[RRWHEEL].recoveryStart,
																		   m_reduxRecovIndex[RRWHEEL].recoveryEnd);
		// Open all ABS inlet valves
		else if(step == "AllInletValvesOn")				 status = AllInletValvesOn();
		// Close all ABS inlet valves
		else if(step == "AllInletValvesOff")			 status = AllInletValvesOff();
		// Check for excessive brake force
		else if(step == "TireChunkingTest")				 status = m_baseBrakeTool->ExcessiveBrakeForceCheck();
		else if(step == "ExcessiveBrakeForceCheck")		 status = m_baseBrakeTool->ExcessiveBrakeForceCheck();
		// Initialize the test component
		else if(step == "Initialize")					 status = m_baseBrakeTool->TestStepInitialize();
		// Accelerate to brake test speed
		else if(step == "AccelerateToBrakeSpeed")		 status = m_baseBrakeTool->TestStepAccelerate();
		// Coast down to drag speed
		else if(step == "Coast")						 status = m_baseBrakeTool->TestStepCoast();
		// Perform the drag test
		else if(step== "DragTest")						 status = m_baseBrakeTool->TestStepDrag();
		// Analyze the drag test results
		else if(step == "AnalyzeDragTest")				 status = m_baseBrakeTool->TestStepAnalyzeDrag();
		// Perform the base brake test
		else if(step == "BrakeTest")					 status = m_baseBrakeTool->TestStepBaseBrake();
		// Perform the base brake test
		else if(step == "BrakeToThreshhold")			 status = m_baseBrakeTool->BrakeToThreshhold();
		// Analyze the base brake results
		else if(step == "AnalyzeBrakeTest")				 status = m_baseBrakeTool->TestStepAnalyzeBaseBrake();
		// Analyze the brake balances
		else if(step == "AnalyzeBalance")				 status = m_baseBrakeTool->TestStepBalance();
		// Have operator brake to stop
		else if(step == "BrakeToStop")					 status = m_baseBrakeTool->TestStepBrakeToStop();
		// Disable the brake force meter
		else if(step == "DisableForceMeter")
		{	// Disable force updates and deactivate the force meter
			SystemWrite(GetDataTag("BrakeActive"), 0);
			status = BEP_STATUS_SUCCESS == m_baseBrakeTool->UpdateTarget(false) ? testPass : testFail;
		}
		// Verify Evac and Fill procedure was completed prior to rolls testing
		else if(step == "CheckEvacAndFill")				status = CheckEvacAndFill();
		// Disable the diagnostic speed limit so faults are not set in the module
		else if(step == "DisableDiagnosticSpeedLimit")	status = DisableDiagSpeedLimit();
		// Accelerate to dynamic park brake test speed
		else if("AccelerateToParkBrakeSpeed" == step)  status = m_baseBrakeTool->TestStepAccelerateToParkBrakeSpeed();
		// Perform dynamic park brake test
		else if("DynamicParkBrakeTest" == step)		   status = m_baseBrakeTool->TestStepDynamicParkBrake();
		// Analyze dynamic park brake test
		else if("AnalyzeDynamicParkBrake" == step)	   status = m_baseBrakeTool->AnalyzeDynamicParkBrake();
		else if("SendKeepAliveMessage" == step)			status = SendKeepAliveMessage();
		// Finish up all testing
		else if(step == "FinishUp")
		{	// Run the finish up method
			status = FinishUp();
			// Send the test result
			SendOverallResult(status);
		}
		else
		{ // Since it's not a test step we've defined, call the parent
			status = GenericTCTemplate<VehicleModuleType>::CommandTestStep(value);
		}
	}
	catch(ModuleException &ex)
	{
		Log(LOG_ERRORS, "ModuleException in %s:%s - %s", GetComponentName().c_str(), GetTestStepName().c_str(), ex.GetReason());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	catch(BepException &ex)
	{
		Log(LOG_ERRORS, "%s CommandTestStep %s Exception: %s\n",
			GetComponentName().c_str(), GetTestStepName().c_str(), ex.what());
		status = BEP_SOFTWAREFAIL_RESPONSE;
	}
	// This will be logged more than once perhaps if we called our parent's CommandTestStep() method
	Log(LOG_DEV_DATA, "%s.CommandTestStep(%s): Returning %s\n",
		GetComponentName().c_str(), value.c_str(), status.c_str());
	// return the status
	return(status);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericABSTCTemplate<VehicleModuleType>::IsFourChannelSystem(void)
{
	return(m_numberOfChannels == 4);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericABSTCTemplate<VehicleModuleType>::IsFourWheelDrive(void)
{
	return(m_numberOfDrivenWheels == 4);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const int GenericABSTCTemplate<VehicleModuleType>::GetNumberOfChannels(void)
{
	return m_numberOfChannels;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const int GenericABSTCTemplate<VehicleModuleType>::GetNumberOfDrivenWheels(void)
{
	return m_numberOfDrivenWheels;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::SetNumberOfChannels(const int numberOfChannels)
{
	m_numberOfChannels = numberOfChannels;
	return;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::SetNumberOfDrivenWheels(const int numberOfDrivenWheels)
{
	m_numberOfDrivenWheels = numberOfDrivenWheels;
	return;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::InitializeHook(const XmlNode *config)
{
	Log(LOG_FN_ENTRY, "Entering GenericABSTCTemplate::InitializeHook");
	// call parent initialize hook
	GenericTCTemplate<VehicleModuleType>::InitializeHook(config);
	// Launch the Force thread to update the meter
	m_baseBrakeTool->LaunchForceThread();
	// Create a new array of reduction/recovery indices
	m_reduxRecovIndex.reserve(MAXWHEELS);
	m_reduxRecovIndex.resize(MAXWHEELS);

	// Load the wheel fire Sequences
	try
	{
		m_abs3ChannelFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS3ChannelFiringSequence")->getChildren());
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, "Firing seuquence not specified for 3-channel system - %s\n", err.GetReason());
	}
	try
	{
		m_abs4ChannelFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS4ChannelFiringSequence")->getChildren());
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, "Firing seuquence not specified for 4-channel system - %s\n", err.GetReason());
	}
	try
	{
		m_abs6ChannelFiringOrder.DeepCopy(config->getChild("Setup/Parameters/ABS6ChannelFiringSequence")->getChildren());
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, "Firing seuquence not specified for 6-channel system - %s\n", err.GetReason());
	}
	// Clear the reduction recovery indices
	for(INT32 wheelIndex = 0; (UINT16)wheelIndex < MAXWHEELS; wheelIndex++)
	{
		m_reduxRecovIndex[wheelIndex].recoveryEnd = 0;
		m_reduxRecovIndex[wheelIndex].recoveryStart = 0;
		m_reduxRecovIndex[wheelIndex].reductionEnd = 0;
		m_reduxRecovIndex[wheelIndex].reductionStart = 0;
	}
	// Populate the roller index
	for(INT32 rollerIndex = LFWHEEL; (UINT16)rollerIndex < MAXWHEELS; rollerIndex++)
	{	// Insert the index mapping into the translation table
		m_rollerIndex.insert(RollerIndexValue(rollerName[rollerIndex], rollerIndex));
	}
	try
	{
		const XmlNode *valid4x4 = config->getChild("Setup/Parameters/Valid4x4GSensorTypes");
		m_validGSensorTypes.DeepCopy(valid4x4->getChildren());
		// Check if vehicle is equipped with G_Sensor Component and set member variable
		CheckForValidGSensorType();
	}
	catch(...)
	{	//error loading Valid4x4GSensorTypes
		try
		{	// Error loading Valid ETC Speed Control Types
			Log(LOG_ERRORS, "XmlException loading Valid4x4GSensorTypes, looking for NoTransferCaseTestCode\n");
			SetGSensorEquipped(SystemRead(GetDataTag("TransferCaseTestCode")).compare(GetDataTag("NoTransferCaseTestCode")));
		}
		catch(...)
		{	// Default to not ETC type
			Log(LOG_ERRORS, "XmlException loading NoTransferCaseTestCode, default to Not G-Sensor Equipped\n");
			SetGSensorEquipped(false);
		}
	}
	// Set the size of the ABS stat data
	m_absStats.reserve(GetRollerCount());
	m_absStats.resize(GetRollerCount());
	Log(LOG_FN_ENTRY, "Exiting GenericABSTCTemplate::InitializeHook");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::ReloadConfiguration(void)
{
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::ReloadConfiguration()\n");
	//Clear all items that were copied
	m_abs3ChannelFiringOrder.clear(true);
	m_abs4ChannelFiringOrder.clear(true);
	m_abs6ChannelFiringOrder.clear(true);
	m_validGSensorTypes.clear(true);
	//call base class
	GenericTCTemplate<VehicleModuleType>::ReloadConfiguration();
	Log(LOG_FN_ENTRY, "Exit GenericABSTCTemplate::ReloadConfig() done\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::ResetModule(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep() || GetParameterBool("AlwaysResetModule"))
	{	// Attempt to command the module
		try
		{
			moduleStatus = m_vehicleModule.PerformPowerOnReset();
			// Check the status of the command
			Log(LOG_DEV_DATA, "Power on reset: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
			// Wait for module reset to complete, defaults to 0
			BposSleep(GetParameterInt("ModuleResetDelay"));
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::RunPumpMotor(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if the test should be performed
	if(!ShortCircuitTestStep())
	{	// Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.RunPumpMotor();
			// Check the status of the command
			Log(LOG_DEV_DATA, "Run pump motor: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			delay(GetParameterInt("PostPumpRunDelay"));
			if(!GetParameterBool("RunPumpMotorSkipModuleValidation"))
			{
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
				testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
			}
			else
			{
				Log(LOG_DEV_DATA, "Skipping module response validation per parameter");
				testResult = testPass;
			}
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::StopPumpMotor(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if the test should be performed
	if(!ShortCircuitTestStep())
	{
		try
		{
			moduleStatus = m_vehicleModule.StopPumpMotor();
			// Check the status of the command
			Log(LOG_DEV_DATA, "Stop the pump motor: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::DetermineSystemConfiguration(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	UINT16 moduleConfig          = 0x0000;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Attempt to read the system config from the module
		try
		{
			moduleStatus = m_vehicleModule.GetInfo("ReadModuleConfiguration", moduleConfig);
			// Check the status of the read
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
			Log(LOG_ERRORS, "Read module config status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			// Determine the number of channels
			DataAnalysis analyze;
			if(GetParameterBool("IsFourChannelSystem"))
			{
				Log(LOG_DEV_DATA, "IsFourChannelSystem parameter set, calling SetFourChannelSystem(true)\n");
				SetFourChannelSystem(true);
			}
			else
			{
				SetFourChannelSystem(analyze.CompareData(moduleConfig, (UINT16)atoh(GetParameter("FourChannelSystemCode").c_str()), 
														 ALL_BITS_SET));
			}


			if(GetParameterBool("UseValidGSensorTypes"))
			{
				SetFourWheelDrive(IsGSensorEquipped());
			}
			else
			{
				SetFourWheelDrive(analyze.CompareData(moduleConfig, (UINT16)atoh(GetParameter("FourWheelDriveCode").c_str()), 
													  ALL_BITS_SET));
			}
			// Log the data and return the test result
			Log(LOG_DEV_DATA, "UseValidGSensorTypes: %s Four wheel drive equipped: %s", 
				GetParameterBool("UseValidGSensorTypes") ? "True" : "False", IsFourWheelDrive() ? "True" : "False");
			if(GetParameterBool("CheckForESP"))
			{
				SetESPEquipped(analyze.CompareData(moduleConfig, (UINT16)atoh(GetParameter("ESPSystemCode").c_str()), 
												   ALL_BITS_SET));
				// Log the data and return the test result
				Log(LOG_DEV_DATA, "Is ESP Equipped: %s", IsESPEquipped() ? "True" : "False");
			}
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
	}
	else
	{	// Skipping the test step
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::UpdateShipStatus(const string &result)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the function entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step, write the result to the module
		try
		{	// Check if the init message should be sent first
			if(GetParameterBool("SendInitMessageBeforeUpdate"))
			{
				moduleStatus = m_vehicleModule.CommandModule("SendInitMessage"+result);
			}
			// Write the result to the module
			moduleStatus = m_vehicleModule.UpdateRollsStatusByte(result);
			// Update the test results
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
			Log(LOG_DEV_DATA, "Module result update: %s\n", testResult.c_str());
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "Error updating test result status - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "ResultWrittenToModule", result, "");
	}
	else
	{	// Skip this step
		Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::CheckForGSensorCross(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if the test step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step
		try
		{	// Wait for zero speed
			if(CheckZeroSpeed())
			{	// At zero speed, read the wheel speed sensors
				WheelSpeeds_t wheelSensors;
				moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadSensorSpeeds"), wheelSensors);
				// Check the status of the module read
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good read, verify each sensor is reading zero speed
					bool gSensorCross = false;
					for(UINT16 wheelIndex = LFWHEEL; (wheelIndex < GetRollerCount()) && !gSensorCross; wheelIndex++)
					{	// Check the current wheel
						gSensorCross = (short)wheelSensors[wheelIndex] != 0;
						if(gSensorCross)
						{	// G Sensor crossed with the current wheel
							Log(LOG_ERRORS, "G-Sensor crossed with %s\n", rollerName[wheelIndex].c_str());
							testResult = testFail;
							testResultCode = GetFaultCode("GSensorCrossedWith"+rollerName[wheelIndex]);
							testDescription = GetFaultDescription("GSensorCrossedWith"+rollerName[wheelIndex]);
						}
					}
					// Determine the result of the test
					testResult = BEP_TESTING_STATUS == testResult ? testPass : testResult;
				}
				else
				{	// Communications failure
					Log(LOG_ERRORS, "Communication failure reading wheel speed sensors - status: %s\n",
						ConvertStatusToResponse(moduleStatus).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			else
			{	// Timeout waiting for zero speed
				Log(LOG_ERRORS, "Timeout waiting for zero speed!\n");
				testResult = testTimeout;
				testResultCode = GetFaultCode("ZeroSpeedTimeout");
				testDescription = GetFaultDescription("ZeroSpeedTimeout");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException in %s, status: %s\n", GetTestStepName().c_str(),
				ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::CalibrateGSensor(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test step should be performed
	if(!ShortCircuitTestStep() && ((GetNumberOfDrivenWheels() == 4) || GetParameterBool("AlwaysCalibrateGSensor")))
	{	// Wait for the vehicle to be at zero speed
		if(CheckZeroSpeed())
		{	// Calibrate the G-sensor
			try
			{
				moduleStatus = m_vehicleModule.CalibrateGSensor();
				// Determine the result of the operation
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CalibrateGSensorFail");
				testDescription = testPass == testResult ? testDescription : GetFaultDescription("CalibrateGSensorFail");
				Log(LOG_DEV_DATA, "Calibrate G-Sensor module status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			}
			catch(ModuleException &excpt)
			{
				Log(LOG_ERRORS, "ModuleExcpetion calibrating G-Sensor: %s\n", excpt.GetReason());
				testResult = testSoftwareFail;
				testResultCode = GetFaultCode("SoftwareFailure");
				testDescription = GetFaultDescription("SoftwareFailure");
			}
		}
		else
		{	// timeout waiting for zero speed
			testResult = testTimeout;
			testResultCode = GetFaultCode("ZeroSpeedTimeout");
			testDescription = GetFaultDescription("ZeroSpeedTimeout");
			Log(LOG_ERRORS, "Timeout waiting for zero speed\n");
		}
		// Report the test results
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this step
		Log(LOG_FN_ENTRY, "Skipping test step - %s -- ShortCircuit: %s, WheelDrive: %d",
			GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
			GetNumberOfDrivenWheels());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::CheckForGSwitch(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Need to perform the test
		bool gSwitchPresent = false;
		try
		{	// Wait for zero-speed prior to checking
			if(CheckZeroSpeed())
			{	// Look for the G-switch
				moduleStatus = m_vehicleModule.GetInfo(GetDataTag("IsGSwitchPresent"), gSwitchPresent);
				// Check the status of the operation
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good read, check for the presence of the g-switch
					testResult = gSwitchPresent ? testPass : testFail;
					testResultCode = gSwitchPresent ? testResultCode : GetFaultCode("GSwitchMissing");
					testDescription = gSwitchPresent ? testDescription : GetFaultDescription("GSwitchMissing");
				}
				else
				{	// Communication failure reading g-switch status
					Log(LOG_ERRORS, "Commuincation failure reading presence of G-switch - status: %s\n",
						ConvertStatusToResponse(moduleStatus).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			else
			{	// Timeout waiting for zero speed
				Log(LOG_ERRORS, "Timeout waiting for zero speed\n");
				testResult = testTimeout;
				testResultCode = GetFaultCode("ZeroSpeedTimeout");
				testDescription = GetFaultDescription("ZeroSpeedTimeout");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException in %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the result of the test
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "GSwitchPresent", gSwitchPresent ? "Yes" : "No", "");
	}
	else
	{	// Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::CheckFluidFillSwitch(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Need to perform the test
		bool fluidFillSwitchOn = true;
		try
		{	// Wait for vehicle to be at zero speed
			if(CheckZeroSpeed())
			{	// Look for the fluid fill switch status
				moduleStatus = m_vehicleModule.GetInfo("IsFluidFillSwitchOn", fluidFillSwitchOn);
				// Check the status of the operation
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good read, check for the fluid fill switch to be off
					testResult = !fluidFillSwitchOn ? testPass : testFail;
					testResultCode = !fluidFillSwitchOn ? testResultCode : GetFaultCode("FluidFillSwitchOn");
					testDescription = !fluidFillSwitchOn ? testDescription : GetFaultDescription("FluidFillSwitchOn");
				}
				else
				{	// Communication failure reading fluid fill switch position
					Log(LOG_ERRORS, "Commuincation failure reading status of fluid fill switch - status: %s\n",
						ConvertStatusToResponse(moduleStatus).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			else
			{	// Timeout waiting for zero speed
				Log(LOG_ERRORS, "Timeout waiting for zero speed\n");
				testResult = testTimeout;
				testResultCode = GetFaultCode("ZeroSpeedTimeout");
				testDescription = GetFaultDescription("ZeroSpeedTimeout");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException in %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the result of the test
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "FluidFillSwitch", fluidFillSwitchOn ? "On" : "Off", "");
	}
	else
	{	// Need to skip this test
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::ProgramTireSize()
{
	string testResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Log the entry and determine if this step should be performed
	Log(LOG_FN_ENTRY, "%s::%s - Enter", GetComponentName().c_str(), GetTestStepName().c_str());
	if(!ShortCircuitTestStep())
	{	// Check the tire size in the build data first
		UINT16 tireSize = atoh(ReadSubscribeData(GetDataTag("TireSizeDataTag")).c_str());
		UINT16 moduleTireSize = 0x0000;
		if(0x0000 != tireSize)
		{	// Tire size was provided in build data, check if same as module
			try
			{	// Read the tire size from the module first
				moduleStatus = m_vehicleModule.ReadCurrentTireSize(moduleTireSize);
				Log(LOG_DEV_DATA, "Module Tire Size: %04X, Broadcast Tire Size: %04X", moduleTireSize, tireSize);
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good read, evaluate the data and program if necessary
					if(tireSize != moduleTireSize)
					{	// Tire sizes differ, program the build data value
						moduleStatus = m_vehicleModule.ProgramTireSize(tireSize);
						Log(LOG_ERRORS, "Programmed tire size into module: %s", ConvertStatusToResponse(moduleStatus).c_str());
						testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
						testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("CommunicationFailure");
						testDescription = 
						!testResult.compare(testPass) ? testDescription : GetFaultDescription("CommunicationFailure");
					}
					else
					{	// Tire sizes match, skip programming
						Log(LOG_DEV_DATA, "Module Tire size matches build data");
						testResult = testPass;
					}
				}
				else
				{	// Communication failure reading tire size
					Log(LOG_ERRORS, "Communication failure reading tire size - status: %s",
						ConvertStatusToResponse(moduleStatus).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			catch(ModuleException &excpt)
			{
				Log(LOG_ERRORS, "Module Exception programming tire size: %s", excpt.what());
				testResult = testSoftwareFail;
				testResultCode = GetFaultCode("SoftwareFailure");
				testDescription = GetFaultDescription("SoftwareFailure");
			}
		}
		else
		{	// No tire size provided in build data, skipping programming
			testResult = GetParameterBool("FailIfNoTireSize") ? testFail : testSkip;
			testResultCode = !testResult.compare(testPass) ? testResultCode : GetFaultCode("NoTireSize");
			testDescription = !testResult.compare(testPass) ? testDescription : GetFaultDescription("NoTireSize");
		}
		// Report the result
		char buf[16];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "ModuleTireSize", CreateMessage(buf, sizeof(buf), "%04X", moduleTireSize), "",
								 "BroadcastTireSize", ReadSubscribeData(GetDataTag("TireSizeDataTag")), "");
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step - %s", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::BrakeSwitchTest()
{	// Set up some variables
	string testResult = testFail;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	bool brakeSwitchOn = false;
	bool brakeSwitchOff = false;
	bool displayPrompt = false;
	// Log the function entry
	Log(LOG_FN_ENTRY, "%s:%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step.
		string switchPosition = "Unknown";
		string promptTag;
		// Wait for both brake switch positions to be seen
		SetStartTime();
		do
		{	// Read the brake switch position
			status = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), switchPosition);
			// Check the status of the switch
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, determine switch position
				if(("On" == switchPosition) && !brakeSwitchOn)
				{	// Switch is on, prompt for Off
					brakeSwitchOn = true;
					// Determine if a prompt should be displayed
					if(!brakeSwitchOff)
					{	// Brake switch off not seen, prompt driver
						promptTag = "RemoveFootFromBrake";
						displayPrompt = true;
					}
					Log(LOG_DEV_DATA, "Brake switch is ON. Waiting for OFF.\n");
				}
				else if(("Off" == switchPosition) && !brakeSwitchOff)
				{	// Switch is Off, prompt for On
					brakeSwitchOff = true;
					// Determine if the prompt should be displayed
					if(!brakeSwitchOn)
					{	// Brake switch on not seen, prompt driver
						displayPrompt = true;
						promptTag = "DepressBrake";
					}
					Log(LOG_DEV_DATA, "Brake switch is OFF. Waiting for ON.\n");
				}
				else if((("On" == switchPosition) && brakeSwitchOn) ||
						(("Off" == switchPosition) && brakeSwitchOff))
				{	// Already detected the switch in the current position, log the data
					Log(LOG_DEV_DATA, "Brake switch is still %s\n", switchPosition.c_str());
					BposSleep(GetTestStepInfoInt("ScanDelay"));
				}
				else
				{	// Unkown switch position
					Log(LOG_ERRORS, "Unknown switch position: %s  (On: %s, Off: %s, status: %s)\n", switchPosition.c_str(),
						(brakeSwitchOn ? "True" : "False"), (brakeSwitchOff ? "True" : "False"),
						ConvertStatusToResponse(status).c_str());
				}
				// Determine if a prompt should be displayed
				if(displayPrompt)
				{	// Throw the prompt up for the operator
					DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
					displayPrompt = false;	 // Prevent prompt from being displayed multiple times (avoid flicker)
				}
			}
			else
			{
				SetCommunicationFailure(true);
				break;
			}
			// Keep looping while good status, time remaining and not both brake switch positions
		} while(TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()) && (!brakeSwitchOff || !brakeSwitchOn));
		// Determine the results of the test
		if(!TimeRemaining())
		{	// Timeout waiting for both switch states
			testResult = testTimeout;
			testResultCode = GetFaultCode("TestTimeout");
			testDescription = GetFaultDescription("TestTimeout");
			Log(LOG_ERRORS, "Timeout waiting for both switch states -- On: %s, Off: %s\n",
				(brakeSwitchOn ? "True" : "False"), (brakeSwitchOff ? "True" : "False"));
		}
		else if(BEP_STATUS_SUCCESS != StatusCheck())
		{	// Bad system status
			testResult = testAbort;
			testResultCode = GetFaultCode("SystemStatus");
			testDescription = GetFaultDescription("SystemStatus");
			Log(LOG_ERRORS, "Bad system status, aborting -- status: %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
		}
		else
		{	// Check for both switch states
			testResult = (brakeSwitchOn && brakeSwitchOff) ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("BrakeSwitchStatesNotSeen");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("BrakeSwitchStatesNotSeen");
			Log(LOG_DEV_DATA, "%s both brake switch states -- On: %s, Off: %s\n",
				(brakeSwitchOn && brakeSwitchOff) ? "Detected" : "Failed to detect",
				brakeSwitchOn ? testPass.c_str() : testFail.c_str(),
				brakeSwitchOff ? testPass.c_str() : testFail.c_str());
		}
		// Remove the prompt from the display
		RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
		// Send the test results
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "BrakeSwitchOn", brakeSwitchOn ? testPass : testFail, "",
								 "BrakeSwitchOff", brakeSwitchOff ? testPass : testFail, "");
	}
	else
	{	// Skipping test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step: %s\n", GetTestStepName().c_str());
	}
	// Log the function exit
	Log(LOG_FN_ENTRY, "%s:%s - Exit (testResult: %s)\n",
		GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	// Return the result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::BrakeSwitchTest(const string& position)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	bool brakeSwitchOn           = false;
	bool compare                 = true;
	// Log the function entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test step should be performed
	if(!ShortCircuitTestStep())
	{	// Attempt to observe the brake switch in the on position
		try
		{	// Determine what type of comparison to perform based on requested position
			// NOTE:  brakeSwitchOn is set to the opposite state to ensure we do not get a false pass
			brakeSwitchOn = GetDataTag("SwitchOn") == position ? false : true;
			compare       = GetDataTag("SwitchOn") == position ? true  : false;
			SetStartTime();
			// Look for the brake switch in the desired position
			do
			{
				moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadBrakeSwitch"), brakeSwitchOn);
				if(!brakeSwitchOn)	BposSleep(GetTestStepInfoInt("ScanDelay"));
				// Keep checking while time remaining, brake switch not in desired position, good system status and good comms 
				// with module
			} while(TimeRemaining() && (compare != brakeSwitchOn) && (BEP_STATUS_SUCCESS == StatusCheck()) &&
					(BEP_STATUS_SUCCESS == moduleStatus));
			// Check the exit condition
			if(!TimeRemaining() && (brakeSwitchOn != compare))
			{	// Timeout waiting for brake switch in the desired position
				Log(LOG_ERRORS, "%s: Timeout while waiting for brake switch %s\n",
					GetTestStepName().c_str(), position.c_str());
				testResult = testTimeout;
				testResultCode = GetFaultCode("BrakeSwitchTimeout");
				testDescription = GetFaultDescription("BrakeSwitchTimeout");
			}
			else if(BEP_STATUS_SUCCESS != StatusCheck())
			{	// Bad system status
				Log(LOG_ERRORS, "Bad system status while looking for brake switch %s - status: %s\n",
					position.c_str(), ConvertStatusToResponse(StatusCheck()).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("SystemStatus");
				testDescription = GetFaultDescription("SystemStatus");
			}
			else if(moduleStatus != BEP_STATUS_SUCCESS)
			{	// Bad comms with the module
				Log(LOG_ERRORS, "%s: Communication failure reading brake switch position - status: %s\n",
					GetTestStepName().c_str(), ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
			else if(brakeSwitchOn == compare)
			{	// Good condition, no timeout, no module comm error
				testResult = testPass;
			}
			// Log the data from the module
			Log(LOG_DEV_DATA, "Observed brake switch %s: %s\n",
				position.c_str(), (brakeSwitchOn == compare) ? testPass.c_str() : testFail.c_str());
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS,"ModuleException reading Brake Switch Position: %s\n", excpt.GetReason());
			testResult = testSoftwareFail;
			testDescription = GetFaultDescription("SoftwareFailure");
			testResultCode = GetFaultCode("SoftwareFailure");
		}
		// Report the result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Do not need to perform this step
		Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit: %s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline string GenericABSTCTemplate<VehicleModuleType>::LowSpeedSensorTest(void)
{	// Call the convenience method
	return SensorTest();
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline string GenericABSTCTemplate<VehicleModuleType>::HighSpeedSensorTest(void)
{	// Call the convenience method
	return SensorTest();
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::SensorTest(void)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	string testResult      = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	string wheelResult[GetRollerCount()];
	float initialRollerSpeeds[GetRollerCount()];
	float finalRollerSpeeds[GetRollerCount()];
	WheelSpeeds_t moduleSpeeds;
	Log(LOG_DEV_DATA, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test should be performed
	if(!ShortCircuitTestStep())
	{
		try
		{	// Wait for the vehicle to be in speed range
			string speedCheck = AccelerateToTestSpeed(GetParameterFloat(GetTestStepName()+"TargetSpeed"),
													  GetParameter(GetTestStepName()+"TargetSpeedRange"),
													  GetParameterInt(GetTestStepName()+"TargetSpeedScanDelay"), false);
			// Check if the speed range has been achieved
			if(testPass == speedCheck)
			{	// Rolls Speed in range, Wait for some steady samples
				if(GetParameterBool("DiplayOperatorInstructions") && GetTestStepName() == "HighSpeedSensorTest")
				{
					DisplayPrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"), "White");
				}
				WaitForSteadySpeed(GetParameterInt(GetTestStepName()+"SteadySpeedSamples"), 
								   GetParameterInt(GetTestStepName()+"SteadySpeedTimeout"),
								   GetParameterFloat(GetTestStepName()+"MinSteadyWheelSpeed"), 
								   GetParameterFloat(GetTestStepName()+"MaxSteadyWheelSpeed"));
				if(GetParameterBool("DiplayOperatorInstructions") && GetTestStepName() == "HighSpeedSensorTest")
				{
					RemovePrompt(GetPromptBox("MaintainSpeed"), GetPrompt("MaintainSpeed"), GetPromptPriority("MaintainSpeed"));
				}

				// Monitor sensor speeds to give them time to match the rolls speed
				if(GetParameterBool(GetTestStepName()+ "WaitForStableSensors"))
				{	// Due to module sensor lagging actual wheel speeds, wait for the module to catch up.
					DisplayPrompt(GetPromptBox("MaintainSpeed"),GetPrompt("MaintainSpeed"),GetPromptPriority("MaintainSpeed"));
					// Wait for sensor speeds to be within allowed tolerance "devaition" from rolls wheel speeds
					WaitForStableSensorSpeeds(GetParameterInt(GetTestStepName()+"StableSensorSamples"), 
											  GetParameterInt(GetTestStepName()+"StableSensorTimeout"),
											  GetParameterFloat(GetTestStepName()+"Tolerance"));
					// Remove maintain speed prompt
					RemovePrompt(GetPromptBox("MaintainSpeed"),GetPrompt("MaintainSpeed"),GetPromptPriority("MaintainSpeed"));
				}
				// Get the initial roller speeds
				status = (BEP_STATUS_TYPE)m_baseBrakeTool->GetISpeeds(initialRollerSpeeds);
				// Check the status of obtaining the roller speeds
				if(BEP_STATUS_SUCCESS == status)
				{	// Got initial roller speeds, now get the module wheel speed readings
					status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
					// Check the status of the module read
					if(BEP_STATUS_SUCCESS == status)
					{	// Good read, get the final roller speeds
						status = (BEP_STATUS_TYPE)m_baseBrakeTool->GetISpeeds(finalRollerSpeeds);
						// Check the status of obtaining the roller speeds
						if(BEP_STATUS_SUCCESS == status)
						{	// Good read, evaluate the sensors readings.
							for(UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
							{	// Calculate the upper and lower wheel speed limits
								float averageRollerSpeed = 
								(initialRollerSpeeds[wheelIndex] + finalRollerSpeeds[wheelIndex])/2.0;
								float lowerLimit = 
								averageRollerSpeed * ((100.0 - GetParameterFloat(GetTestStepName()+"Tolerance")) / 100.0);
								float upperLimit =
								averageRollerSpeed * ((100.0 + GetParameterFloat(GetTestStepName()+"Tolerance")) / 100.0);
								// Check the wheel speed sensor against the limits
								if(moduleSpeeds[wheelIndex] > upperLimit)
								{	// Sensor reading too high
									wheelResult[wheelIndex] = testFail;
									testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
									testDescription = 
									GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorHigh");
								}
								else if(moduleSpeeds[wheelIndex] < lowerLimit)
								{	// Sensor reading too low
									wheelResult[wheelIndex] = testFail;
									testResultCode = GetFaultCode(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
									testDescription = 
									GetFaultDescription(rollerName[wheelIndex]+GetTestStepName()+"SensorLow");
								}
								else
								{	// Sensor reading just right
									wheelResult[wheelIndex] = testPass;
									testResultCode = "0000";
									testDescription = rollerName[wheelIndex] + " wheel speed sensor in tolerance\n";
								}
								// Log the data and report the result for this wheel
								Log(LOG_DEV_DATA, "%s wheel speed sensor %s \n\t\t\tLower Limit: %.2f\n\t\t\tUpper Limit: %.2f"
									"\n\t\t\tAverage Roll Speed: %.2f\n\t\t\tSensor Speed: %.2f\n",
									rollerName[wheelIndex].c_str(), wheelResult[wheelIndex].c_str(), lowerLimit, upperLimit,
									averageRollerSpeed, moduleSpeeds[wheelIndex]);
								char temp[16];
								SendSubtestResultWithDetail(
														   rollerName[wheelIndex]+GetTestStepName(), wheelResult[wheelIndex],
														   testDescription, testResultCode,
														   "SensorSpeed", CreateMessage(temp,sizeof(temp),"%.2f", moduleSpeeds[wheelIndex]), unitsMPH,
														   "RollSpeed", CreateMessage(temp, sizeof(temp), "%.2f", averageRollerSpeed), unitsMPH,
														   "LowerLimit", CreateMessage(temp, sizeof(temp), "%.2f", lowerLimit), unitsMPH,
														   "UpperLimit", CreateMessage(temp, sizeof(temp), "%.2f", upperLimit), unitsMPH,
														   "Tolerance", GetParameter(GetTestStepName()+"Tolerance"), "Percent");
								// Update the overall result
								testResult = testPass == wheelResult[wheelIndex] ? testResult : wheelResult[wheelIndex];
							}
							// Determine the description and code to report
							testResult = BEP_TESTING_STATUS == testResult ? testPass : testFail;
							testResultCode = testPass == testResult ? "0000" : GetFaultCode(GetTestStepName()+"Fail");
							testDescription = 
							testPass == testResult ? GetTestStepInfo("Description") : 
							GetFaultDescription(GetTestStepName()+"Fail");
						}
						else
						{	// Error reading roller speeds
							Log(LOG_ERRORS, "Failed to get final roller speeds - status: %s\n", 
								ConvertStatusToResponse(status).c_str());
							testResult = testFail;
							testResultCode = GetFaultCode(GetTestStepName()+"SystemCommFailure");
							testDescription = GetFaultDescription(GetTestStepName()+"SystemCommFailure");
						}
					}
					else
					{	// Error reading the wheel speed sensors
						Log(LOG_ERRORS, "Error reading wheel speeds from the module - status: %s\n", 
							ConvertStatusToResponse(status).c_str());
						testResult = testFail;
						testResultCode = GetFaultCode("CommunicationFailure");
						testDescription = GetFaultDescription("CommunicationFailure");
					}
				}
				else
				{	// Error reading initial roller speeds
					Log(LOG_ERRORS, "Failed to get initial roller speeds - status: %s\n", 
						ConvertStatusToResponse(status).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode(GetTestStepName()+"SystemCommFailure");
					testDescription = GetFaultDescription(GetTestStepName()+"SystemCommFailure");
				}
			}
			else
			{	// Timeout waiting for target speed range
				Log(LOG_ERRORS, "Timeout waiting for target speed!\n");
				testResult = testTimeout;
				testResultCode = GetFaultCode("TargetSpeedTimeout");
				testDescription = GetFaultDescription("TargetSpeedTimeout");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the test result
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "LeftFrontSensor", wheelResult[LFWHEEL], "",
								 "RightFrontSensor", wheelResult[RFWHEEL], "",
								 "LeftRearSensor", wheelResult[LRWHEEL], "",
								 "RightRearSensor", wheelResult[RRWHEEL], "");
	}
	else
	{	// Test step should be skipped
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::DumpAndIsolateAllWheels(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Command the module to dump all valves
		try
		{	// Determine if the force meter should be updated during valve firing
			if(GetParameterBool("DisableMeterForValveFiring"))
				m_baseBrakeTool->DisableForceUpdates();
			else
				m_baseBrakeTool->EnableForceUpdates();
			// Dump brake pressure at all wheels
			moduleStatus = m_vehicleModule.DumpValves("All");
			Log(LOG_ERRORS, "Dump all valves status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			// Check the status of the command
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// All valves dumped, hold all
				BposSleep(GetParameterInt("DumpDelay"));
				if(!GetParameterBool("SkipHoldAllValves"))
				{
					moduleStatus = m_vehicleModule.HoldAllValves();
					delay(GetParameterInt("DumpAndHoldDelay"));
					Log(LOG_ERRORS, "Hold all valves status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
				}
				// Determine the results of the test step
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				testResultCode = testPass == testResult ? testResultCode : GetFaultCode("HoldAllValvesFail");
				testDescription = testPass == testResult ? testDescription : GetFaultDescription("HoldAllValvesFail");
			}
			else
			{	// Error dumping all wheels
				Log(LOG_ERRORS, "Failed to dump all wheels - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("DumpAllValvesFail");
				testDescription = GetFaultDescription("DumpAllValvesFail");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "Error Dumping and Isolating all valves: %s\n", excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the result of the test
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip this test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::RecoverAllWheels(void)
{
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{
		try
		{	// Determine the results of the test
			testResult = BEP_STATUS_SUCCESS == m_vehicleModule.RecoverAllValves() ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("RecoverAllValvesFail");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("RecoverAllValvesFail");
			Log(LOG_ERRORS, "Recover all valves status: %s\n", testResult.c_str());
			// Determine if the force meter should be updated during valve firing
			if(GetParameterBool("DisableMeterForValveFiring"))	   m_baseBrakeTool->EnableForceUpdates();
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException recovering all wheels: %s\n", excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the results of the test step
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::StopRollTestMode(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this test should be performed
	if(!ShortCircuitTestStep())
	{
		try
		{	// Determine the results of the test
			moduleStatus = m_vehicleModule.StopRollTestMode();
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("StopRollTestModeFail");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("StopRollTestModeFail");
			Log(LOG_ERRORS, "Stop Roll Test Mode status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "ModuleException stopping roll test mode: %s\n", excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the results of the test step
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::ValveFiringTest()
{
	string testResult             = BEP_TESTING_STATUS;		// Used to report test step results
	string testResultCode         = "0000";
	string testDescription        = GetTestStepInfo("Description");
	string overallReductionResult = BEP_TESTING_STATUS;
	string overallRecoveryResult  = BEP_TESTING_STATUS;
	INT32  sensorSpeedIndex       = 0;
	// Define a structure for storing the local results
	struct _valveFiringResults
	{
		string reductionResult;
		string recoveryResult;
	} valveFiringResults[GetRollerCount()];
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Verify it is ok to test -- not skip, brake testing in progress or development testing
	if((!ShortCircuitTestStep() && (m_baseBrakeTool->GetBrakeTestingStatus() == BEP_TESTING_RESPONSE)) ||
	   GetParameterBool("DeveloperTest"))
	{	// Instruct operator and update GUI
		UpdatePrompts();
		// Determine if the force meter should be updated during valve firing
		if(GetParameterBool("DisableMeterForValveFiring"))
			m_baseBrakeTool->DisableForceUpdates();
		else
			m_baseBrakeTool->EnableForceUpdates();
		// Determine if recovery or reduction should be performed first
		bool reductionFirst = GetParameterBool("PerformReductionBeforeRecovery");
		// Tag the start of the reduction/recovery sequence
		m_absStartIndex = TagArray("ABSStart");
		// Determine which firing sequence to use
		XmlNodeMap *firingOrder;
		if(GetNumberOfChannels() == 3)		firingOrder = &m_abs3ChannelFiringOrder;
		else if(GetNumberOfChannels() == 4)	firingOrder = &m_abs4ChannelFiringOrder;
		else if(GetNumberOfChannels() == 6)	firingOrder = &m_abs6ChannelFiringOrder;
		else								 firingOrder = &m_abs4ChannelFiringOrder;
		// Get the initial speeds if doing sensor cross check
		if(GetParameterBool("CollectSensorSpeedData"))
		{	// Wait for forces to stabilize
			BposSleep(GetParameterInt("BrakeForceStabilizeTime"));
			// Get the roller speeds
			m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
			// Get the wheel speeds from the module
			m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
			sensorSpeedIndex++;
		}
		// Fire each wheel in the sequence
		for(XmlNodeMapItr iter = firingOrder->begin(); iter != firingOrder->end(); iter++)
		{	// Get the index for the current wheel
			RollerIndexItr rollerIter;
			if(((rollerIter = m_rollerIndex.find(iter->second->getValue())) != m_rollerIndex.end()) ||
			   (iter->second->getValue() == "Rear"))
			{
				INT32 wheelNameIndex;
				// Determine the index to use
				if( (GetNumberOfChannels() == 3) && (iter->second->getValue() == "Rear"))
				{	// Three channel system, left rear and right rear have same indices.
					// NOTE: Left rear will be used for indices.  Right rear will be updated after the
					//       firing for the rears is complete.
					wheelNameIndex = LRWHEEL;
				}
				else
				{
					wheelNameIndex = rollerIter->second;
				}
				// Provide a short delay between valves -- NOTE: use delay instead of BposSleep(), timing is critical!
				delay(GetParameterInt("InterWheelGapDelay"));
				// Determine if reduction or recovery should be performed first
				if(reductionFirst)
				{	// Reduce the wheel
					valveFiringResults[wheelNameIndex].reductionResult = 
					Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
							  m_reduxRecovIndex[wheelNameIndex].reductionEnd);
					// Recover the wheel
					valveFiringResults[wheelNameIndex].recoveryResult = 
					Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
							 m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
				}
				else
				{
					// Recover the wheel
					valveFiringResults[wheelNameIndex].recoveryResult = 
					Recovery(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].recoveryStart,
							 m_reduxRecovIndex[wheelNameIndex].recoveryEnd);
					// Reduce the wheel
					valveFiringResults[wheelNameIndex].reductionResult = 
					Reduction(iter->second->getValue(),m_reduxRecovIndex[wheelNameIndex].reductionStart,
							  m_reduxRecovIndex[wheelNameIndex].reductionEnd);
				}
				// Get the wheel speed data if performing a wheel speeds sensor cross test
				if(GetParameterBool("CollectSensorSpeedData"))
				{	// Get the roller speeds
					Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
						sensorSpeedIndex);
					m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
					// Get the wheel speeds from the module
					Log(LOG_DEV_DATA, "Collecting Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
						sensorSpeedIndex);
					m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
					sensorSpeedIndex++;
				}
				if(GetParameterBool("SendKeepAliveDurringValveFire"))
				{//send single keep alive message
					SendKeepAliveMessage();
				}
			}
			else
			{	// No roller index specified
				Log(LOG_ERRORS, "No roller index defined for wheel: %s -- Not firing valves\n", 
					iter->second->getValue().c_str());
			}
		}
		// Reenable force updates once the valve firing is complete
		m_baseBrakeTool->EnableForceUpdates();
		// Get the final speed data if performing a wheel speeds sensor cross test
		if(GetParameterBool("CollectSensorSpeedData"))
		{	// Get the roller speeds
			Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting roller speeds - sensorSpeedIndex: %d\n", 
				sensorSpeedIndex);
			m_baseBrakeTool->GetISpeeds(m_sensorCrossCheckData[sensorSpeedIndex].rollerSpeeds);
			// Get the wheel speeds from the module
			Log(LOG_DEV_DATA, "Collecting Final Sensor Speed Data - Getting sensor speeds - sensorSpeedIndex: %d\n", 
				sensorSpeedIndex);
			m_vehicleModule.GetInfo("ReadSensorSpeeds", m_sensorCrossCheckData[sensorSpeedIndex].sensorSpeeds);
		}
		// Tag the end of the ABS sequence
		m_absEndIndex = TagArray("ABSEnd");
		// If this is a three channel system, update the indices so the left rear and right rear
		// wheels have the same index
		if(GetNumberOfChannels() == 3)
		{
			m_reduxRecovIndex[RRWHEEL].reductionStart = m_reduxRecovIndex[LRWHEEL].reductionStart;
			m_reduxRecovIndex[RRWHEEL].reductionEnd   = m_reduxRecovIndex[LRWHEEL].reductionEnd;
			m_reduxRecovIndex[RRWHEEL].recoveryStart  = m_reduxRecovIndex[LRWHEEL].recoveryStart;
			m_reduxRecovIndex[RRWHEEL].recoveryEnd    = m_reduxRecovIndex[LRWHEEL].recoveryEnd;
		}
		// Remove the operator prompts
		RemovePrompts();
		// Assume all wheels have passed unless otherwise indicated
		overallReductionResult = testPass;
		overallRecoveryResult  = testPass;
		// Determine the results of the test
		for(UINT32 wheelIndex = 0; wheelIndex < firingOrder->size(); wheelIndex++)
		{	// Check if the reduction sequence on this wheel failed
			if(testPass != valveFiringResults[wheelIndex].reductionResult)	overallReductionResult = testFail;
			// Check if the recovery sequence on this wheel failed
			if(testPass != valveFiringResults[wheelIndex].recoveryResult)	overallRecoveryResult  = testFail;
		}
		// Send the results for this step
		testResult = ((overallReductionResult == testPass) && (overallRecoveryResult == testPass)) ? testPass : testFail;
		testResultCode = testPass == testResult ? testResultCode : GetFaultCode("ReduxRecovSequenceFail");
		testDescription = testPass == testResult ? testDescription : GetFaultDescription("ReduxRecovSequenceFail");
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OverallReductionSequence", overallReductionResult, "",
								 "OverallRecoverySequence", overallRecoveryResult, "");
	}
	else
	{	// Test step is being skipped
		Log(LOG_FN_ENTRY, "Skipping %s -- \n\t\tShortCircuitTestStep: %s\n\t\tBrakeTestingStatus: %s\n\t\tDeveloperTest: %s\n",
			GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
			m_baseBrakeTool->GetBrakeTestingStatus().c_str(), GetParameterBool("DeveloperTest") ? "True" : "False");
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//----------------------------------------------------------------------------
template<class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::SendKeepAliveMessage(void)
{
	string result(BEP_TESTING_STATUS);
	// Log the entry and check if the test step should be performed
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::SendKeepAliveMessage() - Enter");
	BEP_STATUS_TYPE status = m_vehicleModule.SendKeepAliveNoResponse();
	result = (BEP_STATUS_SUCCESS == status) ? testPass : testFail;
	Log(LOG_ERRORS, "Send Single Keepalive message: %s (status: %s)", 
		result.c_str(), ConvertStatusToResponse(status).c_str());
	// Report the result
	SendTestResult(result, GetTestStepInfo("Description"));
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::SendKeepAliveMessage() - Exit");
	return result;
}

template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::EvaluateABS(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	string wheelReduxResults[GetRollerCount()];
	string wheelRecovResults[GetRollerCount()];
	string overallRedux = testPass, overallRecov = testPass;
	string valveCrossResult = BEP_TESTING_STATUS;
	bool okayToRunAnalyzeValveCross = true;
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if t his step should be performed
	if((!ShortCircuitTestStep() && (BEP_TESTING_STATUS == m_baseBrakeTool->GetBrakeTestingStatus())) ||
	   GetParameterBool("DeveloperTest"))
	{	// Make sure some samples were taken
		if(m_absEndIndex > m_absStartIndex)
		{	// Read the data from the brake force array
			INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"),
																		   m_absStartIndex, m_absEndIndex);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == dataStatus)
			{	// Evaluate the reduction and recovery values
				// NOTE: The analyze methods report detailed results for each wheel
				for(UINT32 wheelIndex = 0; wheelIndex < GetRollerCount(); wheelIndex++)
				{	// Find the actual start and end reduction indices.
					// The stored indices are absolute, whereas the wheel force array is relative to start of test
					INT32 reductionStart = m_reduxRecovIndex[wheelIndex].reductionStart - m_absStartIndex;
					INT32 reductionEnd   = m_reduxRecovIndex[wheelIndex].reductionEnd   - m_absStartIndex;
					if(reductionStart >= reductionEnd)
					{
						okayToRunAnalyzeValveCross = false;
					}
					Log(LOG_DEV_DATA, "%s Reduction -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
						reductionStart, reductionEnd);
					BEP_STATUS_TYPE reductionStatus = AnalyzeReductionForces(wheelIndex, reductionStart, reductionEnd);
					wheelReduxResults[wheelIndex] = BEP_STATUS_SUCCESS == reductionStatus ? testPass : testFail;
					// Find the actual start and end recovery indices.
					// The stored indices are absolute, whereas the wheel force array is relative to start of test
					INT32 recoveryStart = m_reduxRecovIndex[wheelIndex].recoveryStart - m_absStartIndex;
					INT32 recoveryEnd   = m_reduxRecovIndex[wheelIndex].recoveryEnd   - m_absStartIndex;
					if(recoveryStart >= recoveryEnd)
					{
						okayToRunAnalyzeValveCross = false;
					}
					Log(LOG_DEV_DATA, "%s Recovery -- Start: %d, End: %d\n", rollerName[wheelIndex].c_str(),
						recoveryStart, recoveryEnd);
					BEP_STATUS_TYPE recoveryStatus = AnalyzeRecoveryForces(wheelIndex, recoveryStart, recoveryEnd);
					wheelRecovResults[wheelIndex] = BEP_STATUS_SUCCESS == recoveryStatus ? testPass : testFail;
					// Determine the average force for the stats
					m_absStats[wheelIndex].averageValue = (m_absStats[wheelIndex].minValue + m_absStats[wheelIndex].maxValue) / 2.0;
					// Update the overall results
					overallRedux = ((testPass == wheelReduxResults[wheelIndex]) && 
									((testPass == overallRedux) || (BEP_TESTING_STATUS == overallRedux))) ? overallRedux : testFail;
					overallRecov = 
					((testPass == wheelRecovResults[wheelIndex]) && 
					 ((testPass == overallRecov) || (BEP_TESTING_STATUS == overallRecov))) ? overallRecov : testFail;
					Log(LOG_DEV_DATA, "%s Reducion: %s, Recovery: %s\n", rollerName[wheelIndex].c_str(),
						wheelReduxResults[wheelIndex].c_str(), wheelRecovResults[wheelIndex].c_str());
					// Report the ABS stats for this wheel
					char buff[8];
					SendSubtestResultWithDetail(rollerName[wheelIndex]+"AbsStats", testPass, "ABS Stats", "0000",
												"MinReduction", CreateMessage(buff, sizeof(buff), "%.2f", m_absStats[wheelIndex].minValue), unitsLBF,
												"MaxRecovery", CreateMessage(buff, sizeof(buff), "%.2f", m_absStats[wheelIndex].maxValue), unitsLBF,
												"MinReductionIndex", CreateMessage(buff, sizeof(buff), "%d", m_absStats[wheelIndex].minValueIndex+m_reduxRecovIndex[wheelIndex].reductionStart), "",
												"MaxRecoveryIndex", CreateMessage(buff, sizeof(buff), "%d", m_absStats[wheelIndex].maxValueIndex+m_reduxRecovIndex[wheelIndex].recoveryStart), "",
												"Average", CreateMessage(buff, sizeof(buff), "%.2f", m_absStats[wheelIndex].averageValue), unitsLBF);
					// Report the values to the system for display
					SystemWrite(rollerName[wheelIndex]+"AbsMinReductionValue", m_absStats[wheelIndex].minValue);
					SystemWrite(rollerName[wheelIndex]+"AbsMaxRecoveryValue", m_absStats[wheelIndex].maxValue);
					SystemWrite(rollerName[wheelIndex]+"AbsAverageValue", m_absStats[wheelIndex].averageValue);
					SystemWrite(rollerName[wheelIndex]+"AbsMinReductionIndex", m_absStats[wheelIndex].minValueIndex+m_reduxRecovIndex[wheelIndex].reductionStart);
					SystemWrite(rollerName[wheelIndex]+"AbsMaxRecoveryIndex", m_absStats[wheelIndex].maxValueIndex+m_reduxRecovIndex[wheelIndex].recoveryStart);
				}
				// Perform the valve cross check
				BEP_STATUS_TYPE valveCrossStatus;
				if(okayToRunAnalyzeValveCross)
				{
					valveCrossStatus = AnalyzeValveCross();
				}
				else
				{
					valveCrossStatus = BEP_STATUS_SKIP;
				}
				// Translate the status to a result
				switch(valveCrossStatus)
				{
				case BEP_STATUS_SKIP:         valveCrossResult = testSkip;  break;
				case BEP_STATUS_SUCCESS:      valveCrossResult = testPass;  break;
				default:                      valveCrossResult = testFail;  break;
				}
				Log(LOG_DEV_DATA, "Valve Cross: %s\n", valveCrossResult.c_str());
				// Determine the overall result of the testing
				testResult = ((overallRedux == testPass) && (overallRecov == testPass) &&
							  ((valveCrossResult == testPass) || (valveCrossResult == testSkip))) ? testPass : testFail;
				testResultCode = testResult == testPass ? "0000" : GetFaultCode("ABSFailure");
				testDescription = testResult == testPass ? testDescription : GetFaultDescription("ABSFailure");
			}
			else
			{	// Error reading brake force data
				Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("WheelForceDataReadError");
				testDescription = GetFaultDescription("WheelForceDataReadError");
			}
		}
		else
		{	// No samples were taken
			Log(LOG_ERRORS, "Not evaluating ABS reduction/recovery data - NO SAMPLES TAKEN!\n");
			testResult = testFail;
			testResultCode = GetFaultCode("NoABSData");
			testDescription = GetFaultDescription("NoABSData");
		}
		// Report the results of the test
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OverallReduction", overallRedux, "",
								 "OverallRecovery", overallRecov, "",
								 "OverallValveCross", valveCrossResult, "");
	}
	else
	{	// Need to skip this step
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping %s\n\t\tShort Circuit Test Step: %s\n\t\tTesting Status: %s\n\t\tDeveloper Test: %s\n",
			GetTestStepName().c_str(), ShortCircuitTestStep() ? "True" : "False",
			m_baseBrakeTool->GetBrakeTestingStatus().c_str(),
			GetParameterBool("DeveloperTest") ? "True" : "False");
	}
	// Log the function exit and return the result
	m_evalAbsResult = testResult;
	Log(LOG_DEV_DATA, "Generic ABS setting Eval ABS result to %s", m_evalAbsResult.c_str());
	Log(LOG_FN_ENTRY, "%s::%s - Exit, result:%s\n", GetComponentName().c_str(), GetTestStepName().c_str(), testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::AnalyzeSensorCross(void)
{
	string testResult      = BEP_TESTING_STATUS;
	string testResultCode  = "0000";
	string testDescription = GetTestStepInfo("Description");
	XmlNodeMap *firingSequence;
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed, results are not reliable if valve firing test has failed
	Log(LOG_DEV_DATA, "Checking Evaluate ABS Result: %s", m_evalAbsResult.c_str());
	if(!ShortCircuitTestStep() && (GetOverallResult() == testPass) && !m_evalAbsResult.compare(testPass))
	{	// Determine which firing sequence was used
		firingSequence = IsFourChannelSystem() ? &m_abs4ChannelFiringOrder : &m_abs3ChannelFiringOrder;
		// If three channel system, do not need to evaluate Right rear since it will be the same as left rear
		INT32 rollerCount = IsFourChannelSystem() ? GetRollerCount() : GetRollerCount() - 1;
		// Log the speed samples collected
		for(INT32 index = 0; (UINT16)index < (GetRollerCount()+2); index++)
		{
			Log(LOG_DEV_DATA, "Sample %d: \n\t\t\tRoller Speeds -- LF: %.2f   RF: %.2f   LR: %.2f   RR: %.2f"
				"\n\t\t\tSensor Speeds -- LF: %.2f   RF: %.2f   LR: %.2f   RR: %.2f\n", index,
				m_sensorCrossCheckData[index].rollerSpeeds[LFWHEEL],
				m_sensorCrossCheckData[index].rollerSpeeds[RFWHEEL],
				m_sensorCrossCheckData[index].rollerSpeeds[LRWHEEL],
				m_sensorCrossCheckData[index].rollerSpeeds[RRWHEEL],
				m_sensorCrossCheckData[index].sensorSpeeds[LFWHEEL],
				m_sensorCrossCheckData[index].sensorSpeeds[RFWHEEL],
				m_sensorCrossCheckData[index].sensorSpeeds[LRWHEEL],
				m_sensorCrossCheckData[index].sensorSpeeds[RRWHEEL]);
		}
		// Check for wheel speed senspr cross.
		// NOTE: The roller with the greatest speed delta must correspond to the wheel being fired.
		//       To accomplish this:
		//           1. Check for the roller with the greatest speed delta
		//           2. Check for the wheel speed sensor with the greatest delta
		//           3. The wheels in steps 1 and 2 must match, else sensors are crossed
		for(INT32 sample = 0; (UINT16)sample < firingSequence->size(); sample++)
		{
			INT32 rollerWithGreatestDelta = 0;
			INT32 sensorWithGreatestDelta = 0;
			INT32 rollerWithLeastDelta = 0;
			INT32 sensorWithLeastDelta = 0;
			float maxRollerDelta          = 0.0;
			float minRollerDelta          = 999.9;
			float rollerDelta             = 0.0;
			float maxSensorDelta          = 0.0;
			float minSensorDelta          = 999.9;
			float sensorDelta             = 0.0;
			// Determine which wheel had the greatest delta speed
			for(INT32 rollerIndex = 0; rollerIndex < rollerCount; rollerIndex++)
			{	// Determine the current deltas
				rollerDelta = fabs(m_sensorCrossCheckData[sample].rollerSpeeds[rollerIndex] -
								  m_sensorCrossCheckData[sample+1].rollerSpeeds[rollerIndex]);
				sensorDelta = fabs(m_sensorCrossCheckData[sample].sensorSpeeds[rollerIndex] -
								  m_sensorCrossCheckData[sample+1].sensorSpeeds[rollerIndex]);
				// Check if the current roller delta is greater than the max delta
				if(rollerDelta > maxRollerDelta)
				{	// Save the new max delta
					maxRollerDelta = rollerDelta;
					// Save the wheel index this is from
					rollerWithGreatestDelta = rollerIndex;
				}
				// Check if the current roller delta is less than the min delta
				if(rollerDelta < minRollerDelta)
				{	// Save the new min delta
					minRollerDelta = rollerDelta;
					// Save the wheel index this is from
					rollerWithLeastDelta = rollerIndex;
				}
				// Check if the current sensor delta is greater than the max delta
				if(sensorDelta > maxSensorDelta)
				{	// Save the new max delta
					maxSensorDelta = sensorDelta;
					// Save the sensor index this is from
					sensorWithGreatestDelta = rollerIndex;
				}
				// Check if the current sensor delta is less than the min delta
				if(sensorDelta < minSensorDelta)
				{	// Save the new min delta
					minSensorDelta = sensorDelta;
					// Save the sensor index this is from
					sensorWithLeastDelta = rollerIndex;
				}
			}
			// Log the data
			Log(LOG_DEV_DATA, "%s -- \n\t\tMax Roller: %s - delta: %.2f, Max Sensor: %s - delta: %.2f"
				"\n\t\tMin Roller: %s - delta: %.2f, Min Sensor: %s - delta: %.2f",
				firingSequence->getNode(sample)->getValue().c_str(),
				rollerName[rollerWithGreatestDelta].c_str(), maxRollerDelta,
				rollerName[sensorWithGreatestDelta].c_str(), maxSensorDelta,
				rollerName[rollerWithLeastDelta].c_str(), minRollerDelta,
				rollerName[sensorWithLeastDelta].c_str(), minSensorDelta);
			// Update the test result if the max does not gel
			bool sensorCrossCheckPass = ((rollerWithGreatestDelta == sensorWithGreatestDelta) &&
										 (firingSequence->getNode(sample)->getValue() == rollerName[rollerWithGreatestDelta]) &&
										 (firingSequence->getNode(sample)->getValue() == rollerName[sensorWithGreatestDelta]));
			if(!sensorCrossCheckPass) testResult = testFail;
			// Report the results for this wheel
			string faultTag = firingSequence->getNode(sample)->getValue() + "SensorCrossedWith" + 
							  rollerName[sensorWithGreatestDelta];
			string desc = firingSequence->getNode(sample)->getValue() + "Sensor Cross Check";
			SendSubtestResult(firingSequence->getNode(sample)->getValue()+"SensorCrossCheck",
							  sensorCrossCheckPass ? testPass : testFail,
							  sensorCrossCheckPass ? desc : GetFaultDescription(faultTag),
							  sensorCrossCheckPass ? "0000" : GetFaultCode(faultTag));
		}
		// Determine the overall results
		testResult = testResult != testFail ? testPass : testResult;
		testResultCode = testPass == testResult ? testResultCode : GetFaultCode("SensorCrossCheckFail");
		testDescription = testPass == testResult ? testDescription : GetFaultDescription("SensorCrossCheckFail");
		// Report the result of the test
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_FN_ENTRY, "Skipping test step: %s\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::Reduction(const string &wheel, INT32 &startIndex, INT32 &stopIndex)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Performing reduction for " + wheel;
	// Log the function entry
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::Redcution(%s) -- Enter\n", wheel.c_str());
	try
	{	// Tag the data array with the start of the reduction sequence
		startIndex = TagArray(wheel + "ReduxStart");
		// Command the module to reduce
		moduleStatus = m_vehicleModule.ActuateReduction(wheel);
		// Check the status of the command
		if(BEP_STATUS_SUCCESS == moduleStatus)
		{	// Delay for the fire time - do not use BposSleep since timing is CRITICAL!
			delay(GetParameterInt(wheel+"ReductionPulseLength"));
			// Determine if valves must be held
			if(GetParameterBool("HoldReduction")) m_vehicleModule.HoldAllValves();
			// Set the test result to pass and tag the end of the reduction pulse
			testResult = testPass;
			stopIndex = TagArray(wheel + "ReduxStop");
		}
		else
		{	// Error command module to reduce the wheel
			Log(LOG_ERRORS, "Communication failure performing reduction for %s -- status: %s\n",
				wheel.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Reduction(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the result of the test
	SendSubtestResult(wheel + "Reduction", testResult, testDescription, testResultCode);
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::Reduction(%s) -- Exit\n", wheel.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::Recovery(const string &wheel, INT32 &startIndex, INT32&stopIndex)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Performing recovery for " + wheel;
	// Log the function entry
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::Recovery(%s) -- Enter\n", wheel.c_str());
	try
	{	// Tag the data array with the start of the recovery sequence
		startIndex = TagArray(wheel + "RecovStart");
		// Command the module to recover the wheel
		moduleStatus = m_vehicleModule.ActuateRecovery(wheel);
		// check the status of the command
		if(BEP_STATUS_SUCCESS == moduleStatus)
		{	// Delay for the recovery time -- do NOT use BposSleep since timing is CRITICAL!
			delay(GetParameterInt(wheel+"RecoveryPulseLength"));
			// Determine if valves must be held
			if(GetParameterBool("HoldRecovery"))  m_vehicleModule.HoldAllValves();
			// Set the test result and tag the end of the recovery period
			testResult = testPass;
			stopIndex = TagArray(wheel + "RecovStop");
		}
		else
		{	// Error commanding the module to recover the wheel
			Log(LOG_ERRORS, "Communication failure performing recovery for %s -- status: %s\n",
				wheel.c_str(), ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testFail;
			testResultCode = GetFaultCode("CommunicationFailure");
			testDescription = GetFaultDescription("CommunicationFailure");
		}
	}
	catch(ModuleException &err)
	{
		Log(LOG_ERRORS, "ModuleException in Recovery(%s) - %s\n", wheel.c_str(), err.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the result of the test
	SendSubtestResult(wheel + "Recovery", testResult, testDescription, testResultCode);
	// Log the function exit and return the result
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::Recovery(%s) -- Exit\n", wheel.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::AllInletValvesOn(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if the test should be performed
	if(!ShortCircuitTestStep())
	{	// Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.AllInletValvesOn();
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves on: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericABSTCTemplate<VehicleModuleType>::AllInletValvesOff(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if the test should be performed
	if(!ShortCircuitTestStep())
	{	// Attempt to run the motor
		try
		{
			moduleStatus = m_vehicleModule.AllInletValvesOff();
			// Check the status of the command
			Log(LOG_DEV_DATA, "All inlet valves off: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
			testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
			testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
		}
		catch(ModuleException &excpt)
		{	// Something bad happened
			Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericABSTCTemplate<VehicleModuleType>::AnalyzeReductionForces(INT32 roller, INT32 start, INT32 end)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;		// the overall status of the analysis
	std::string testResult = testFail;				// the test status for the check
	std::string result;								// the result of the individual wheel for display
	std::string resultBG;							// the status (background color) of the individual wheel for display
	DATAARRAY tempDataArray;						// Used to hold the Force values from a single wheel
	// Log the function entry
	Log(LOG_FN_ENTRY, "AnalyzeReductionForces(Wheel:%s, StartIndex:%d, EndIndex:%d)\n", rollerName[roller].c_str(), start, end);
	// Get the Wheel Force Array Data For a Single Wheel
	m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
	// if samples taken
	if(end > start)
	{
		DATAARRAYITR reductionIter = min_element(tempDataArray.begin()+start, tempDataArray.begin()+end+1);
		float reductionValue = *reductionIter;
		m_absStats[roller].minValue = reductionValue;
		m_absStats[roller].minValueIndex = distance(tempDataArray.begin()+start, reductionIter);
		Log(LOG_DEV_DATA, "GenericABSTCTemplate -- Reduction Value: %.2f - Drag Force: %.2f\n",
			reductionValue, m_baseBrakeTool->GetDragForceValue(roller));
		// range check
		if(reductionValue < 0)	reductionValue = 0;
		// range check it against the parameter * 0.01 * brake force for the wheel
		// NOTE: The value in the config file is a percentage.  Need to divide parameter by 100
		float reductionMaxValue = 
		GetParameterFloat(rollerName[roller]+"MinReductionPercent") * 0.01 * GetAbsReferenceForce(roller);
		// Verify the reduction is acceptable
		if(reductionValue < reductionMaxValue)
		{	// report the status as a pass
			testResult = testPass;
			resultBG = colorGreen;
		}
		else
		{	// report the status as a fail
			testResult = testFail;
			resultBG = colorRed;
		}
		// Log the data
		Log(LOG_DEV_DATA, "AnalyzeReductionForces Reduction -- Limit: %.2f, Values: %.2f\n", reductionMaxValue, reductionValue);
		// build the result string
		char temp[256];
		if(m_baseBrakeTool->GetBrakeForceValue(roller) <= 0.0)
		{
			result = "-1";
		}
		else
		{	// determine the percent
			float percent = (reductionValue / m_baseBrakeTool->GetBrakeForceValue(roller)) * 100.0;
			Log(LOG_DEV_DATA, "GenericABSTCTemplate: Reduction Value Percent [%.2f]\n", percent);
			// create the result
			result = CreateMessage(temp, sizeof(temp), "%.2f", percent);
		}
		// update the GUI
		SystemWrite(GetDataTag(rollerName[roller] + "ABSReductionValue"),   result);
		SystemWrite(GetDataTag(rollerName[roller] + "ABSReductionBGColor"), resultBG);
		// Update the fault data to be reported with the test result
		string faultTag = rollerName[roller]+"ReductionFail";
		string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
		string testDescription = 
		(testResult == testPass ? rollerName[roller]+" Reduction Test" : GetFaultDescription(faultTag));
		// send the test results to the TestResultServer
		SendSubtestResultWithDetail(
								   rollerName[roller] + "ABSReduction", testResult, testDescription, testResultCode,
								   rollerName[roller] + "ABSReductionPercent", result, "Percent",
								   rollerName[roller] + "ReductionForceValue", CreateMessage(temp, sizeof(temp), "%.2f", reductionValue), unitsLBF,
								   rollerName[roller] + "ReductionMinParameter", GetParameter(rollerName[roller]+"MinReductionPercent"), "Percent");
		// Determine the status to return
		status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
	}
	else
	{
		status = BEP_STATUS_SKIP;
		Log(LOG_FN_ENTRY, "Skipping AnalyReductionForces -- End index is not greater than Start index!\n");
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "AnalyzeReductionForces() done: %s\n", testResult.c_str());
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericABSTCTemplate<VehicleModuleType>::AnalyzeRecoveryForces(INT32 roller, INT32 start, INT32 end)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;	// the overall status of the analysis
	string testResult = testFail;				// the test status for the check
	string result = "999";						// the result of the individual wheel for display
	string resultBG = "Blue";					// the status (background color) of the individual wheel for display
	DATAARRAY tempDataArray;					// Used to hold the Force values from a single wheel
	// Log the entry
	Log(LOG_FN_ENTRY, "AnalyzeRecoveryForces(Wheel:%s, StartIndex: %d, EndIndex: %d)\n", 
		rollerName[roller].c_str(), start, end);
	// if samples taken
	if(end > start)
	{
		// Get the Wheel Force Array Data For a Single Wheel
		m_baseBrakeTool->GetWheelForceArray(roller,tempDataArray);
		DATAARRAYITR recoveryIter = max_element(tempDataArray.begin()+start, tempDataArray.begin()+end+1);
		float recoveryValue = *recoveryIter;
		m_absStats[roller].maxValue = recoveryValue;
		m_absStats[roller].maxValueIndex = distance(tempDataArray.begin()+start, recoveryIter);
		// range check it against the parameter * 0.01 * brake force for the wheel
		// NOTE: the parameter is a percent, so it must be divided by 100 prior to usage
		float recoveryMinValue = (GetParameterFloat(rollerName[roller]+"MinRecoveryPercent") * 
								  .01 * m_baseBrakeTool->GetBrakeForceValue(roller));
		// Determine the result of the test
		if(recoveryValue > recoveryMinValue)
		{	// report the status as a pass
			testResult = testPass;
			resultBG = colorGreen;
		}
		else
		{	// report the status as a fail
			testResult = testFail;
			resultBG = colorRed;
		}
		Log(LOG_DEV_DATA, "AnalyzeRecoveryForces Recovery -- Limit: %.2f, Value: %.2f\n", recoveryMinValue, recoveryValue);
		// build the result string
		char temp[256];
		if(m_baseBrakeTool->GetBrakeForceValue(roller) <= 0.0)
		{  // if the base brake force is less than 0, set to -1
			result = "-1";
		}
		else
		{	// else update the value with the %
			float percent = (recoveryValue / m_baseBrakeTool->GetBrakeForceValue(roller)) * 100.0;
			// create the result
			result = CreateMessage(temp, sizeof(temp), "%.2f", percent);
			Log(LOG_DEV_DATA, "AnalyzeRecoveryForces: Recovery value being reported - %s\n", result.c_str());
		}
		// update the GUI
		SystemWrite(GetDataTag(rollerName[roller] + "ABSRecoveryValue"),   result);
		SystemWrite(GetDataTag(rollerName[roller] + "ABSRecoveryBGColor"), resultBG);
		// Update the fault data to be sent with the test result
		string faultTag = rollerName[roller]+"RecoveryFail";
		string testResultCode = (testResult == testPass ? "0000" : GetFaultCode(faultTag));
		string faultDesc = (testResult == testPass ? "Brake Force Recovery Test" : GetFaultDescription(faultTag));
		// send the test results to the TestResultServer
		SendSubtestResultWithDetail(
								   rollerName[roller] + "ABSRecovery", testResult, faultDesc, testResultCode,
								   rollerName[roller] + "ABSRecoveryValue", result, "Percent",
								   rollerName[roller] + "RecoveryForceValue", CreateMessage(temp, sizeof(temp), "%.2f", recoveryValue), unitsLBF,
								   rollerName[roller] + "RecoveryMinPercent", GetParameter(rollerName[roller]+"MinRecoveryPercent"), "Percent");
		// Determine the status to return
		status = testResult == testPass ? BEP_STATUS_SUCCESS : BEP_STATUS_FAILURE;
	}
	else
	{
		status = BEP_STATUS_SKIP;
		Log(LOG_FN_ENTRY, "Skipping AnalyzeRecoveryForces -- End index is not greater than Start index!\n");
	}
	// Log the exit and return the status
	Log(LOG_FN_ENTRY, "AnalyzeRecoveryForces() done: %s\n", testResult.c_str());
	return(status);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
float GenericABSTCTemplate<VehicleModuleType>::GetAbsReferenceForce( INT32 roller)
{
	return( m_baseBrakeTool->GetBrakeForceValue(roller));
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericABSTCTemplate<VehicleModuleType>::AnalyzeValveCross(void)
{
	RollerIndexItr rollerIter;	// used to find the string equivalent for each wheel index
	INT32 wheelCount;	// used to increment through each wheel that was fired during the test
	INT32 innerWheelCount;	// used to increment through the rest of the wheels for each wheelCount
	double valveForceData[MAXWHEELS];	// store the force deltas for each wheel during each valve fire
	INT32 firingOrder[MAXWHEELS];	// The firing order that the wheels should have followed
	INT32 recoveryStart;   // Store the start index for the current force array being analyzed
	INT32 recoveryEnd;	   // Store the end index for the current force array being analyzed
	INT32 wheelIndex;		// The index of the current wheel being analyzed
	INT32 innerWheelIndex;	// The index of the other wheels that are rotated through for each wheelIndex
	INT32 actualOrderedFired[MAXWHEELS];	// the firing order actually seen during the test
	INT32 minForceToIndicateValveFired = 75;	// minimum force delta required to indicate that a valve fired
	INT32 numberOfWheelsCrossed = 0;	// Number of wheels that have been determined to be crossed
	INT32 maxDeltaWheelIndex = -1;	// Store the wheel index of the wheel with the greatest force change
	BEP_STATUS_TYPE status = BEP_STATUS_FAILURE;	// store the status of the method for return value
	string wheelCrossMessage = "ABS Valves are crossed";	// Message used to report the results to the test result server
	string testResult = testFail;	// Result of test
	string testResultCode = "0000";	// Failure code for test
	string testDescription = "Test the ABS system for Valve Cross";	// Description of test

	// Determine which firing sequence to use
	XmlNodeMap *firingOrderMap = IsFourChannelSystem() ? &m_abs4ChannelFiringOrder : &m_abs3ChannelFiringOrder;
	// Log method entry
	Log(LOG_FN_ENTRY, "GenericABSTCTemplate::AnalyzeValveCross() - Enter\n");
	// Initialize arrays to value of -1
	for(wheelCount = 0;wheelCount < MAXWHEELS;wheelCount++)
	{
		firingOrder[wheelCount] = -1;
		actualOrderedFired[wheelCount] = -1;
	}
	// Log for debugging purposes, can be removed once code is tested and stable
	Log(LOG_DETAILED_DATA, "GenericABSTCTemplate::AnalyzeValveCross() - Initialized firing order arrays\n");

	try
	{
		// Try and get the parameter from the configuration file
		GetParameter("MinForceToIndicateValveFired", minForceToIndicateValveFired);
		// Log for debugging purposes, can be removed once code is tested and stable
		Log(LOG_DETAILED_DATA, "GenericABSTCTemplate::AnalyzeValveCross() - minForceToIndicateValveFired = %d\n",
			minForceToIndicateValveFired);
	}
	catch(XmlException ex)
	{  // Set to default value
		Log(LOG_ERRORS, "GenericABSTCTemplate::AnalyzeValveCross() - missing parameter "
			"for MinForceToIndicateValveFired, default of %d used\n", minForceToIndicateValveFired);
	}
	// Set wheelCount to 0, not done in the for loop initialize because it causes
	//   a compile error for some reason
	wheelCount = 0;
	// Get the firing order that we will test against to ensure the valves fired correctly
	for(XmlNodeMapItr iter = firingOrderMap->begin();
	   (iter != firingOrderMap->end()) && (wheelCount < MAXWHEELS);
	   iter++, wheelCount++)
	{
		Log(LOG_DETAILED_DATA, 
			"GenericABSTCTemplate::AnalyzeValveCross() - For Loop for Firing Order; Count = %d, Wheel = %s\n",
			wheelCount,iter->second->getValue().c_str());

		if(((rollerIter = m_rollerIndex.find(iter->second->getValue())) != m_rollerIndex.end()) ||
		   (iter->second->getValue() == "Rear"))
		{
			// Determine the index to use
			if(!IsFourChannelSystem() && (iter->second->getValue() == "Rear"))
			{	// Three channel system, left rear and right rear have same indices.
				// NOTE: Left rear will be used for indices.  Right rear will be updated after the
				//       firing for the rears is complete.
				firingOrder[wheelCount] = (INT32)LRWHEEL;
			}
			else
			{
				firingOrder[wheelCount] = rollerIter->second;
			}
		}
	}
	// Log for debugging purposes, can be removed once code is tested and stable
	Log(LOG_DETAILED_DATA, "GenericABSTCTemplate::AnalyzeValveCross() - m_absEndIndex = %d, m_absStartIndex = %d\n",
		m_absEndIndex, m_absStartIndex);
	// Make sure some samples were taken
	if(m_absEndIndex > m_absStartIndex)
	{
		// Read the data from the brake force array
		INT32 dataStatus = m_baseBrakeTool->ReadDataForWheelForceArray(GetParameter("IcmForceArray"),
																	   m_absStartIndex, m_absEndIndex);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS == dataStatus)
		{

			// Loop through each wheel and determine when it was suppose to be fired.  Get the
			//   force data from that period and then find the change in force for each wheel
			//   during that period.  The wheel that had the greatest change in force will
			//   be assumed to have fired (if its change in force is above the minimum required
			//   to indicate a valve fire occurance)
			for(wheelCount = 0; (wheelCount < (INT32)GetRollerCount()) && (firingOrder[wheelCount] != -1); wheelCount++)
			{
				wheelIndex = firingOrder[wheelCount];
				// Log for debugging purposes, can be removed once code is tested and stable
				Log(LOG_DETAILED_DATA,"Getting data during valve fire for %s\n",
					rollerName[wheelIndex].c_str());
				// The stored indices are absolute, where as the wheel force array is relative to start of test
				recoveryStart = m_reduxRecovIndex[wheelIndex].recoveryStart - m_absStartIndex;
				recoveryEnd   = m_reduxRecovIndex[wheelIndex].recoveryEnd   - m_absStartIndex;
				// Log for debugging purposes, can be removed once code is tested and stable
				Log(LOG_DETAILED_DATA,"ReductionStart = %d, ReductionStop = %d\n",
					recoveryStart,recoveryEnd);
				// Find delta change for all wheels coresponding to the index of the greatest change with the
				//   'fired wheel'
				GetValveCrossForceDelta(recoveryStart,recoveryEnd,valveForceData);
				// Loop is for debugging Purposes only, can be removed once code is tested and stable
				for(int ii = 0; ii < MAXWHEELS;ii++)
				{
					Log(LOG_DETAILED_DATA,"Force Delta for wheel %d: %.2f\n",
						ii,valveForceData[ii]);
				}
				// Store the wheel that had the greatest change in force and is above the minimum required amount
				//  of change (prevent against false valve cross if no valve fired at all).
				if(valveForceData[wheelIndex] > minForceToIndicateValveFired)
				{
					// Log for debugging purposes, can be removed once code is tested and stable
					Log(LOG_DETAILED_DATA,"Valve Force for current wheel: "
						" %.2f, Min Force: %d\n", valveForceData[wheelIndex], minForceToIndicateValveFired);
					// Log for debugging purposes, can be removed once code is tested and stable
					Log(LOG_DETAILED_DATA,"Storing initial wheel index = %d\n",wheelIndex);
					// The wheel that was suppose to fire met the minimum force required
					//   to inicate a valve fire occurance.  Store as the starting point
					actualOrderedFired[wheelCount] = wheelIndex;
				}
				// Else statement is for debugging purposes, can be removed once code is tested and stable
				else
				{
					Log(LOG_DETAILED_DATA,"Valve Force for current wheel: "
						" %.2f, Min Force: %d\n", valveForceData[wheelIndex], minForceToIndicateValveFired);
					Log(LOG_DETAILED_DATA,"Valve didn't fire, not storing\n");
				}
				// Store the current wheel index as the maxDeltaWheelIndex initially
				maxDeltaWheelIndex = wheelIndex;
				// loop on rest of channels to see if the current wheel being analyzed
				//    had the greatest force change or not
				for(innerWheelCount = 0; 
				   (innerWheelCount < (INT32)GetRollerCount()) && (firingOrder[innerWheelCount] != -1); 
				   innerWheelCount++)
				{
					// Store the wheel that had the greatest change in force and is above the minimum required amount
					//  of change (prevent against false valve cross if no valve fired at all.
					innerWheelIndex = firingOrder[innerWheelCount];
					// Log for debugging purposes, can be removed once code is tested and stable
					Log(LOG_DETAILED_DATA,
						"\n\tInnerWheelLoop\n\t--------------\n\tinnerWheelCount = %d\n\tinnerWheelIndex = %d\n\t"
						"maxDeltaWheelIndex = %d\n\tmaxDeltaWheelIndex = %.2f\n\tinnerWheelIndexForce = %.2f\n",
						innerWheelCount,innerWheelIndex,maxDeltaWheelIndex,valveForceData[maxDeltaWheelIndex],
						valveForceData[innerWheelIndex]);
					// Check if the wheel for the innerWheelIndex is our max force change
					if((valveForceData[maxDeltaWheelIndex] < valveForceData[innerWheelIndex])  &&
					   (valveForceData[innerWheelIndex] > minForceToIndicateValveFired))
					{
						// Log for debugging purposes, can be removed once code is tested and stable
						Log(LOG_DETAILED_DATA,"Storing innerWheelIndex = %d in wheelCount %d\n",
							innerWheelIndex,wheelCount);
						// Store the innerWheelIndex as our greatest force change
						actualOrderedFired[wheelCount] = innerWheelIndex;
						// set the variable used to keep track of the wheel with the greatest force change
						maxDeltaWheelIndex = innerWheelIndex;
					}
				}
			}
			Log(LOG_DETAILED_DATA,"\n\tDetermine If wheels are Crossed\n");
			// Determine what valves are crossed by checking the proposed fire order
			//   vs. what actually occurred
			for(wheelCount = 0;(wheelCount < MAXWHEELS) && (firingOrder[wheelCount] != -1);wheelCount++)
			{

				// Log for debugging purposes, can be removed once code is tested and stable
				Log(LOG_DETAILED_DATA,"\n\tWheelCount = %d\n\tSupposeToFire = %d\n\tActuallyFired = %d\n",
					wheelCount,firingOrder[wheelCount],actualOrderedFired[wheelCount]);
				// If the correct valve didn't fire and another valve was seen to have
				//   fired above the minimum required force to indicate a valve fire
				if((firingOrder[wheelCount] != actualOrderedFired[wheelCount]) &&
				   (actualOrderedFired[wheelCount] != -1))
				{
					// Log the problem
					Log(LOG_ERRORS,"Wrong Valve fired!, Check if valve fired at another time\n");
					// loop through the actual valve fired array to see if the wheel
					//   fired at another time which would indicate that it was crossed
					for(innerWheelCount = 0;(innerWheelCount < (INT32)GetRollerCount()) &&
					   (firingOrder[innerWheelCount] != -1);innerWheelCount++)
					{
						// Log for debugging purposes, can be removed once code is tested and stable
						Log(LOG_DETAILED_DATA,"wheelcount = %d, innerwheelcount = %d\n",
							wheelCount,innerWheelCount);
						// don't bother to compare against itself
						if(innerWheelCount == wheelCount) continue;
						// Log for debugging purposes, can be removed once code is tested and stable
						Log(LOG_DETAILED_DATA,"SupposeToHaveFired = %d\n\tActuallyFired = %d\n",
							firingOrder[wheelCount],actualOrderedFired[innerWheelCount]);
						// The valve was seen to have fired during another period
						if(actualOrderedFired[innerWheelCount] == firingOrder[wheelCount])
						{
							// increment the counter used to keep track of the number
							//  of wheels crossed (must be at least 2 to cause test to fail
							numberOfWheelsCrossed++;
							// update the message used to report the failure to the test
							//    result server
							wheelCrossMessage = rollerName[firingOrder[wheelCount]] + " " + wheelCrossMessage;
							// Log for debugging purposes, can be removed once code is tested and stable
							Log(LOG_DETAILED_DATA,"NumberOfWheelsCrossed = %d, WheelCrossMessage = %s\n",
								numberOfWheelsCrossed,wheelCrossMessage.c_str());
						}
					}
				}

			}
			// Fail if the number of wheels crossed is greater than 1 otherwise pass
			if(numberOfWheelsCrossed > 1)
			{
				// Valve cross test fails
				status = BEP_STATUS_FAILURE;
				testResult = testFail;
				testDescription = wheelCrossMessage;
				testResultCode = GetFaultCode("ABSValveCross");
				SendSubtestResult("ABSValveCrossCheck", testResult, testDescription, testResultCode);
			}
			else
			{
				status = BEP_STATUS_SUCCESS;
				testResult = testPass;
				SendSubtestResult("ABSValveCrossCheck", testResult, testDescription, testResultCode);
			}
		}
		else
		{	// Error reading brake force data
			Log(LOG_ERRORS, "Error reading brake force data - status: %s\n", ConvertStatusToResponse(dataStatus).c_str());
			testResult = testFail;
			testResultCode = GetFaultCode("WheelForceDataReadError");
			testDescription = GetFaultDescription("WheelForceDataReadError");
		}
	}
	else
	{	// No samples were taken
		Log(LOG_ERRORS, "Not evaluating ABS reduction/recovery data - NO SAMPLES TAKEN!\n");
		testResult = testFail;
		testResultCode = GetFaultCode("NoABSData");
		testDescription = GetFaultDescription("NoABSData");
	}
	Log(LOG_FN_ENTRY,"GenericABSTCTemplate::AnalyzeValveCross() - Exit, %s\n",ConvertStatusToResponse(status).c_str());
	// Return the result of the test
	return(status);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::GetValveCrossForceDelta(INT32 recoveryStart,
																	  INT32 recoveryEnd,
																	  double *innerValveForceData)
{
	UINT32 wheelCount = 0;	// Used to increment through the wheels
	DATAARRAY tempDataArray;  // Used to hold the Force values from a single wheel

	// Find the minimum and maximum values during the period and report the change
	//    in force for each wheel
	for(wheelCount = 0;(wheelCount < GetRollerCount());wheelCount++)
	{
		// Get the Wheel Force Array Data For a Single Wheel
		m_baseBrakeTool->GetWheelForceArray(wheelCount,tempDataArray);
		float maxValue = *(max_element(tempDataArray.begin()+recoveryStart, tempDataArray.begin()+recoveryEnd+1));
		float minValue = *(min_element(tempDataArray.begin()+recoveryStart, tempDataArray.begin()+recoveryEnd+1));
		innerValveForceData[wheelCount] = abs(maxValue - minValue);
		Log(LOG_DEV_DATA, "GetValveCrossDelta -- Min Value: %.2f - Max Value: %.2f\n",
			minValue, maxValue);
	}
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::WaitForSteadySpeed(const INT32 &speedSamples, const INT32 &timeout,
																 const float &minWheelSpeed, const float &maxWheelSpeed)
{
	WHEELINFO currentWheelSpeeds;
	INT32 totalSamplesInRange = 0;
	float frontSpeed = 0.0;
	float rearSpeed = 0.0;
	do
	{	// Get the wheel speeds
		GetWheelSpeeds(currentWheelSpeeds);
		// Find average axle speeds
		frontSpeed = (currentWheelSpeeds.lfWheel + currentWheelSpeeds.rfWheel) / 2.0;
		rearSpeed = (currentWheelSpeeds.lrWheel + currentWheelSpeeds.rrWheel) / 2.0;
		// Check if it is in range
		if(((minWheelSpeed <= frontSpeed) && (frontSpeed <= maxWheelSpeed)) &&
		   ((minWheelSpeed <= rearSpeed)  && (rearSpeed < maxWheelSpeed)))
			totalSamplesInRange++;
		else
			totalSamplesInRange	= 0;
		// Delay before checking again
		if(totalSamplesInRange < speedSamples) BposSleep(GetTestStepInfoInt("ScanDelay"));
	} while(TimeRemaining(&timeout) && (BEP_STATUS_SUCCESS == StatusCheck()) && (totalSamplesInRange < speedSamples));
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericABSTCTemplate<VehicleModuleType>::WaitForStableSensorSpeeds(const INT32 &speedSamples, const INT32 &timeout,
																		const float &tolerance)
{
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	float initialRollerSpeeds[GetRollerCount()];
	float finalRollerSpeeds[GetRollerCount()];
	WheelSpeeds_t moduleSpeeds;

	//declare limits for testing
	float averageRollerSpeed = 0;
	float lowerLimit = 0;
	float upperLimit = 0;
	float lowerTolerance = (100.0 - tolerance) / 100.0;
	float upperTolerance = (100.0 + tolerance) / 100.0;
	INT32 totalSamplesInRange = 0;
	bool allSensorsInRange = false;
	//what are we doing
	Log(LOG_ERRORS, "Enter WaitForStableSensorSpeeds: samples %d: timeout %d: tolerance %3.2f\n", 
		speedSamples, timeout, tolerance);
	SetSecondaryStartTime();
	do
	{	// Get the initial roller speeds
		status = (BEP_STATUS_TYPE)m_baseBrakeTool->GetISpeeds(initialRollerSpeeds);
		// Check the status of obtaining the roller speeds
		if(BEP_STATUS_SUCCESS == status)
		{	// Got initial roller speeds, now get the module wheel speed readings
			status = m_vehicleModule.GetInfo("ReadSensorSpeeds", moduleSpeeds);
			// Check the status of the module read
			if(BEP_STATUS_SUCCESS == status)
			{	// Good read, get the final roller speeds
				status = (BEP_STATUS_TYPE)m_baseBrakeTool->GetISpeeds(finalRollerSpeeds);
				// Check the status of obtaining the roller speeds
				if(BEP_STATUS_SUCCESS == status)
				{	// Good read, evaluate the sensors readings.
					allSensorsInRange = true;
					averageRollerSpeed = 999.99;
					for(UINT32 wheelIndex = 0; (wheelIndex < GetRollerCount()) && (allSensorsInRange == true); wheelIndex++)
					{	// Calculate the upper and lower wheel speed limits
						averageRollerSpeed = (initialRollerSpeeds[wheelIndex] + finalRollerSpeeds[wheelIndex])/2.0;
						lowerLimit = averageRollerSpeed * lowerTolerance;
						upperLimit = averageRollerSpeed * upperTolerance;
						// Check the wheel speed sensor against the limits
						if((moduleSpeeds[wheelIndex] <= lowerLimit) || (moduleSpeeds[wheelIndex] >= upperLimit))
						{	// Sensor out of range
							allSensorsInRange = false;
							totalSamplesInRange = 0;
						}
					}
					// Count sample if sensors are with in range
					if(allSensorsInRange == true)
					{	// All sensor readings in range
						totalSamplesInRange++;
					}
				}
				else
				{	// Failed system roller speeds
					Log(LOG_ERRORS, "ABS WaitForStableSensorSpeeds: Get Final Roller Speeds failed\n");
				}
			}
			else
			{	// Failed Module roller speeds
				Log(LOG_ERRORS, "ABS WaitForStableSensorSpeeds: Read Module sensor speeds failed\n");
			}
		}
		else
		{	// Failed system roller speeds
			Log(LOG_ERRORS, "ABS WaitForStableSensorSpeeds: Get Init Roller Speeds failed\n");
		}
		//  Wait between scans
		if(totalSamplesInRange < speedSamples) BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Exit if time out, reads fail, or saw desired number of good samples.
	}while(SecondaryTimeRemaining(&timeout) && (BEP_STATUS_SUCCESS == StatusCheck()) && (BEP_STATUS_SUCCESS == status) &&
		   (totalSamplesInRange < speedSamples));
	Log(LOG_ERRORS, "Exit WaitForStableSensorSpeeds: %s samples %d: timeout %d: tolerance %3.2f\n", 
		allSensorsInRange ? "Pass" : "Fail", speedSamples, timeout, tolerance);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
INT32 GenericABSTCTemplate<VehicleModuleType>::StatusCheck(void)
{
	Log(LOG_DEV_DATA, "Entering GenericABSTCTemplate Status Check\n");
	INT32 status = BEP_STATUS_ERROR;
	// Always return success if development testing
	if(GetParameterBool("DeveloperTest"))
	{
		status = BEP_STATUS_SUCCESS;
	}
	else
	{
		Log(LOG_DEV_DATA, "GenericABSTCTemplate: Calling GenericBaseBrakeTC StatusCheck\n");
		status = GenericTCTemplate<VehicleModuleType>::StatusCheck();
	}
	return status;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string GenericABSTCTemplate<ModuleType>::DisableDiagSpeedLimit(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Attempt to command the module
	try
	{
		moduleStatus = m_vehicleModule.DisableDiagnosticSpeedLimit();
		// Check the status of the command
		Log(LOG_DEV_DATA, "Disable diagnostic speed limit: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
		testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
		testResultCode = testPass == testResult ? testResultCode : GetFaultCode("CommunicationFailure");
		testDescription = testPass == testResult ? testDescription : GetFaultDescription("CommunicationFailure");
	}
	catch(ModuleException &excpt)
	{	// Something bad happened
		Log(LOG_ERRORS, "ModuleException during %s: %s\n", GetTestStepName().c_str(), excpt.GetReason());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Send the test result, log the exit and return the result
	SendTestResult(testResult, testDescription, testResultCode);
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class ModuleType>
string GenericABSTCTemplate<ModuleType>::CheckEvacAndFill(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult            = BEP_TESTING_STATUS;
	string testResultCode        = "0000";
	string testDescription       = GetTestStepInfo("Description");
	bool evacAndFillComplete = false;
	// Log the function entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Determine if this step should be performed
	if(!ShortCircuitTestStep())
	{	// Perform the test step
		try
		{	// Get the Evac and Fill information from the module
			moduleStatus = m_vehicleModule.GetInfo("IsEvacAndFillComplete", evacAndFillComplete);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Good read, check the data
				testResult = evacAndFillComplete ? testPass : testFail;
				testResultCode = evacAndFillComplete ? testResultCode : GetFaultCode("EvacAndFillIncomplete");
				testDescription = evacAndFillComplete ? testDescription : GetFaultDescription("EvacAndFillIncomplete");
				// Check if we should force an update
				// NOTE: Do not fail if module cannot be updated since this is intnded for development and pilot
				//       builds only.  Log errors or failure, but do not report
				if(GetParameterBool("UpdateEvacAndFill"))
				{	// Update the module
					moduleStatus = m_vehicleModule.UpdateEvacAndFillResult();
					Log(LOG_DEV_DATA, "Updated Evac and Fill information: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
				}
			}
			else
			{	// Error reading evac and fill information
				Log(LOG_ERRORS, "Error reading Evac and Fill information - status: %s\n", 
					ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
		}
		catch(ModuleException &excpt)
		{
			Log(LOG_ERRORS, "Error reading Evac and Fill information - status: %s\n", 
				ConvertStatusToResponse(moduleStatus).c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the test result
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "EvacAndFillComplete", evacAndFillComplete ? testPass : testFail, "");
	}
	else
	{	// Skip the test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step: %s\n", GetTestStepName().c_str());
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericABSTCTemplate<VehicleModuleType>::SetFourChannelSystem(const bool &isFourChannelABS)
{
	m_isFourChannelSystem = isFourChannelABS;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericABSTCTemplate<VehicleModuleType>::SetFourWheelDrive(const bool &isFourWheelDrive)
{
	m_isFourWheelDrive = isFourWheelDrive;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericABSTCTemplate<VehicleModuleType>::IsGSensorEquipped(void)
{
	return m_isGSensorEquipped;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericABSTCTemplate<VehicleModuleType>::SetGSensorEquipped(const bool &equipped)
{
	Log(LOG_DEV_DATA, "G-Sensor Equipped: %s", equipped ? "True" : "False");
	m_isGSensorEquipped = equipped;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericABSTCTemplate<VehicleModuleType>::CheckForValidGSensorType()
{
	bool runGSensorTest = false;	// Determine if the entire test should run or skip
	XmlNodeMapItr iter;			// Iterate through the possible valid ETC types for this test

	try
	{
		for(iter = m_validGSensorTypes.begin(); (iter != m_validGSensorTypes.end()) && !runGSensorTest; iter++)
		{	// Compare the broadcast ETC Type against the current config file valid ETC Types
			runGSensorTest = !iter->second->getValue().compare(SystemRead(GetDataTag("TransferCaseTestCode")));
		}
	}
	catch(XmlException &err)
	{
		Log(LOG_ERRORS, 
			"XmlException loading DataTag TransferCaseTestCode, default to GSensor Not equipped - %s\n", err.what());
		runGSensorTest = false;
	}
	SetGSensorEquipped(runGSensorTest);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericABSTCTemplate<VehicleModuleType>::SetESPEquipped(const bool &isESPEquipped)
{
	m_isESPEquipped = isESPEquipped;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericABSTCTemplate<VehicleModuleType>::IsESPEquipped(void)
{
	return m_isESPEquipped;
}
