//******************************************************************************
// Description:
//  Generic Emissions Test Component.
//
//==============================================================================
// Copyright (c) 2004 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//==============================================================================
//
// LOG:
//     $Log: /CCRT/UpdatesToCoreSoftware/Core.07103101/Source/Core/ApplicationLayer/TestComponents/Emissions/Common/GenericEmissionsTCTemplate.cpp $
// 
// 1     11/01/07 2:49p Derickso
// Removed Activate and Deactivate as all they did was call the base
// class.
// Updated to use atob instead of writing out logic for strcmp.  
// Updated RedundataBrakeSwitch testing and ClutchUpstop testing.
// 
// 15    10/30/07 1:46p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed some typos in the comments.
// 
// 1     10/15/07 10:19a Derickso
// Removed functions that only called the base class.  No need to overload
// thses functions since there is no specific implementation in this
// class.  For example, Activate and Deactivate.
// 
// Refactored some code to make more readable.
// 
// 13    2/22/07 8:57a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Removed the intial timeout time in VehicleAtIdleState().  The
// individual methods check for a timeout.
// 
// - Don't start the timeout timer in AISIdleConditions() until
// VehicleAtIdleState is true.
// 
// - Added a configurable check to make sure the vehicle is in field mode
// in DeterminePreviousPass().
// 
// 12    1/17/07 3:30p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added m_isPreviousPass.
// - Added Activate() and Deactivate().
// - Added ReloadConfiguration().
// - Set the member variables to their default values in WarmInitialize().
// - Added a parameter AbortIgnitionNotOn to abort the test if the vehicle
// cable is not connect in Setup().
// - Added a parameter NoAbortForHydraulicCoolingFan to not abort the test
// if the hydraulic cooling fan cannot be started in
// StartHydraulicCoolingFan().
// - Added a parameter AlwaysRunCoolingFan to tell if the hydraulic
// cooling fan should always be run.
// - Check NeedToRunHydraulicCoolingFan() before stopping the hydraulic
// cooling fan.
// - Added NeedToRunHydraulicCoolingFan() with a new parameter
// EngineSizeForHydraulicCoolingFanTest to tell if the hydraulic cooling
// fan needs to be run.
// 
// 11    10/25/06 3:55p Rwiersem
// Changes for the 06102701 core release:
// 
//     - Added WarmInitialize().  This method is used to keep the test
// component running when a new drive curve starts.
// 
// 10    5/08/06 7:09p Cward
// Added ability to iterate through list of valid ETC types
//
// 9     4/13/06 3:25a Cward
// Added logic to optionally skip Park/Neutral switch testing.
//
// 1     2/14/06 11:04a Derickso
// Added logic to optionally skip Park/Neutral switch testing.
//
// 8     11/11/05 4:55p Cward
// Changes for 0511101 iteration
//
// 7     8/17/05 8:13p Cward
// Corrected bug in Wait For Engine Running method.
//
// 6     4/13/05 7:00p Cward
// Updates for the 05041301 build
//
// 6     4/04/05 1:01a Derickso
// Changed from using the engine size to determine if the vehicle
// is equipped with ETC (Electronic Throttle Control) to looking at the
// build data.  A member variable and system read was added to determine
// and store the build data.
//
// 5     3/03/05 1:01a Cward
// Changes for the 050303 core iteration
//
// 4     10/14/04 6:39p Cward
// Many changes for the Oct 14 iteration. Mostly fixes for problems found
// at various plants.
//
// 3     9/29/04 10:17p Cward
// Added log call to Is Idle RPM method, Fix log call in Evaluate Odometer
// Integerity routine made changes to the Monitor MAP Sensor method
//
// 2     9/01/04 9:09p Cward
// Added functions to start and stop the hydraulic cooling fan. Added
// delay between throttle reads in Check Closed Throttle. Updated
// preconditions met flag in Monitor MAP Sensor method. Updated
// preconditions met flag in Check Idle speed method. Added delay between
// switch reads in Check Transmission In Neutral method.
//
// 1     8/05/04 2:31a Cward
// Refactored Template class includes
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "GenericEmissionsTCTemplate.h"
#include "ModuleException.h"

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericEmissionsTCTemplate<VehicleModuleType>::GenericEmissionsTCTemplate() :
GenericTCTemplate<VehicleModuleType>(),engine57LV8("5.7L V8"), 
engine47LV8("4.7L V8"),engine37LV6("3.7L V6"),m_isPreviousPass(false), 
m_etcVehicle(false),m_engineSize("Unknown"),m_vehicleType("Unknown"),
m_emissionsType("Unknown"),m_emissionsYear("Unknown"),
m_moduleType("Unknown"),m_currentOdometerReading(0x00),
m_boolTestRedundantBrakeSwitch(false),m_boolTestClutchUpstopSwitch(false),m_gatewayModule(NULL)
{   
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
GenericEmissionsTCTemplate<VehicleModuleType>::~GenericEmissionsTCTemplate()
{	// Nothing special to do
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericEmissionsTCTemplate<VehicleModuleType>::Deactivate(void)
{
	if(m_gatewayModule != NULL)
	{
		m_gatewayModule->CloseCom();
		delete m_gatewayModule;
		m_gatewayModule = NULL;
	}
	GenericTCTemplate<VehicleModuleType>::Deactivate();
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const string GenericEmissionsTCTemplate<VehicleModuleType>::CommandTestStep(const string &value)
{
	string testResult = testFail;
	// Determine if the next test step can be sequenced
	if(StatusCheck() == BEP_STATUS_SUCCESS)
	{	// System is fine to continue testing
		try
		{	// Get the next test step to perform
			Log(LOG_DEV_DATA, "Running test step %s\n", GetTestStepName().c_str());
			// Check the idle engine rpm
			if(GetTestStepName() == "CheckIdleSpeed")				  testResult = CheckIdleSpeed();
			// Check the upline registers
			else if(GetTestStepName() == "CheckUplineRegisters")	  testResult = CheckUplineRegisters();
			// Unlock security to the module
			else if(GetTestStepName() == "CommandSecurityUnlock")	  testResult = CommandSecurityUnlock();
			// Retrieve the module configuration
			else if(GetTestStepName() == "DetermineModuleConfig")	  testResult = DetermineModuleConfig();
			// Check if the vehicle has previously passed emissions
			else if(GetTestStepName() == "DeterminePreviousPass")	  testResult = DeterminePreviousPass();
			// Evaluate the odometer integrity
			else if(GetTestStepName() == "EvaluateOdometerIntegrity") testResult = EvaluateOdometerIntegrity();
			// Check the MAP sensor
			else if(GetTestStepName() == "MonitorMAPSensor")		  testResult = MonitorMAPSensor();
			// Start the hydraulic cooling fan
			else if(GetTestStepName() == "StartHydraulicCoolingFan")  testResult = StartHydraulicCoolingFan();
			// Stop the hydraulic cooling fan
			else if(GetTestStepName() == "StopHydraulicCoolingFan")	  testResult = StopHydraulicCoolingFan();
			// Reset the Rolls affected EOL status bits
			else if(GetTestStepName() == "ProgramEOLStatus")		  testResult = ProgramEOLStatus();
			// Start the switch monitor
			else if(GetTestStepName() == "StartSwitchMonitor")		  testResult = StartSwitchMonitor();
			// Start the TPS Monitor
			else if(GetTestStepName() == "StartTPSMonitor")			  testResult = StartTPSMonitor();
			// Stop the switch monitor
			else if(GetTestStepName() == "StopSwitchMonitor")		  testResult = StopSwitchMonitor();
			// Stop the TPS Monitor
			else if(GetTestStepName() == "StopTPSMonitor")			  testResult = StopTPSMonitor();
			// Validate the type of module being tested
			else if(GetTestStepName() == "VerifyModuleType")		  testResult = VerifyModuleType();
			// Reset the registers to first run status for development testing
			else if(GetTestStepName() == "SetStatusRegistersFirstRun")	   testResult = ResetStatusRegisters(&m_defaultRegisters);
			// Reset the registers to previous pass for development testing
			else if(GetTestStepName() == "SetStatusRegistersPreviousPass") testResult = ResetStatusRegisters(&m_resetPCMRegisters);
			// Ensure the engine is running
			else if(GetTestStepName() == "WaitForEngineRunning")	  testResult = WaitForEngineRunning();
			// No specific method, try the base class
			else testResult	= GenericTCTemplate<VehicleModuleType>::CommandTestStep(value);
		}
		catch(BepException &err)
		{	// Log the exception and return the error
			Log("CommandTestStep %s BepException: %s\n", GetTestStepName().c_str(), err.what());
			testResult = BEP_ERROR_RESPONSE;
		}
	}
	else
	{	// Status of the system is not right to continue testing
		Log(LOG_ERRORS, "Cannot continue testing - StatusCheck(): %s\n", ConvertStatusToResponse(StatusCheck()).c_str());
		testResult = testSkip;
	}
	// Return the result of the test
	Log("CommandTestStep(%s) returning %s\n", value.c_str(), testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::VehicleAtIdleState(void)
{
	bool zeroSpeed, engineRunning, throttleClosed;
	bool preconditionsSatisfied = false;
	// Start a timer so we do not wait forever for the conditions to be satisfied
	SetStartTime();
	zeroSpeed      = CheckZeroSpeed();		 // Check for zero speed
	engineRunning  = CheckEngineRunning();	 // Check for engine running
	throttleClosed = CheckClosedThrottle();	 // Check for closed throttle
	// Verify all preconditions satisied
	preconditionsSatisfied = zeroSpeed && engineRunning && throttleClosed;
	// Log the status of all conditions
	Log(LOG_DEV_DATA, "All preconditions %s satisfied - "
		"\n\tZero Speed: %s\n\tEngine Running: %s\n\tThrottle Closed: %s\n",
		(preconditionsSatisfied ? "are" : "NOT"), (zeroSpeed ? "True" : "False"),
		(engineRunning ? "True" : "False"), (throttleClosed ? "True" : "False"));
	// Return the status of the conditions
	return preconditionsSatisfied;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::VehicleConnected(void)
{
	bool ignitionOn;
	bool preconditionsSatisfied = false;
	// Start a timer so we do not wait forever for the conditions to be satisfied
	SetStartTime();
	do
	{	// Grab the status of all the preconditions
		ignitionOn   = CheckIgnitionOn();	 // Check for ignition on or engine running
	} while(TimeRemaining() && !ignitionOn && (BEP_STATUS_SUCCESS == StatusCheck()));
	// Verify all preconditions satisied
	preconditionsSatisfied = ignitionOn;
	// Log the status of all conditions
	Log(LOG_DEV_DATA, "All preconditions %s satisfied - \n\tIgnition On: %s\n",
		(preconditionsSatisfied ? "are" : "NOT"), (ignitionOn ? "True" : "False"));
	// Return the status of the conditions
	return preconditionsSatisfied;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline string GenericEmissionsTCTemplate<VehicleModuleType>::StopBackgroundMonitors(void)
{	// Stop the switch monitor and tps monitor
	BEP_STATUS_TYPE tpsStatus = m_vehicleModule.StopTPSMonitor();
	BEP_STATUS_TYPE switchStatus = m_vehicleModule.StopSwitchMonitor();
	if(GetParameterBool("CheckGatewayForFanType"))
	{
		if(m_gatewayModule != NULL)
		{
			m_gatewayModule->StopModuleKeepAlive();
		}
	}
	// Call the base class to stop any other monitors
	string baseStatus = GenericTCTemplate<VehicleModuleType>::StopBackgroundMonitors();
	return((BEP_STATUS_SUCCESS == tpsStatus) &&
		   (BEP_STATUS_SUCCESS == switchStatus) &&
		   (testPass == baseStatus) ? testPass : testFail);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline string GenericEmissionsTCTemplate<VehicleModuleType>::ResumeBackgroundMonitors(void)
{	// Start any background monitors that were stopped, but only if not previous pass
	BEP_STATUS_TYPE tpsStatus = BEP_STATUS_ERROR;
	BEP_STATUS_TYPE switchStatus = BEP_STATUS_ERROR;
	if(!IsPreviousPass())
	{	// Determine the throttle position message to send
		if(!m_etcVehicle)
		{
			tpsStatus = m_vehicleModule.StartTPSMonitor(GetDataTag("ReadThrottlePositionNonETC"), true);
		}
		else
		{
			Log(LOG_DEV_DATA, "ETC vehicle, not starting TPS monitor");
			tpsStatus = BEP_STATUS_SUCCESS;
		}
		switchStatus = m_vehicleModule.StartSwitchMonitor();
	}
	else
	{	// Previous pass, do not need to start
		tpsStatus = BEP_STATUS_SUCCESS;
		switchStatus = BEP_STATUS_SUCCESS;
	}
	// Call the base class to restart all other monitors
	string baseStatus = GenericTCTemplate<VehicleModuleType>::ResumeBackgroundMonitors();
	return((BEP_STATUS_SUCCESS == tpsStatus) &&
		   (BEP_STATUS_SUCCESS == switchStatus) &&
		   (testPass == baseStatus) ? testPass : testFail);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericEmissionsTCTemplate<VehicleModuleType>::InitializeHook(const XmlNode *config)
{
	XmlParser  tempParser;

	// Call the base class InitializeHook
	Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::InitializeHook() - Enter\n");
	GenericTCTemplate<VehicleModuleType>::InitializeHook(config);
	Log(LOG_DEV_DATA, "GenericTCTemplate::InitializeHook() complete\n");
	// Load the EOL Status bytes and masks
	try
	{
		const XmlNode *statusBytes = config->getChild("Setup/Parameters/ResetEOLStatusBytes");
		m_resetEOLStatusRegisters.DeepCopy(statusBytes->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading EOL status bytes to reset
		Log(LOG_ERRORS, "XmlException loading ResetEOLStatusBytes - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading EOL status bytes to reset
		Log(LOG_ERRORS, "BepException loading ResetEOLStatusBytes - %s\n", err.what());
	}
	catch(...)
	{	// Error loading EOL status bytes to reset
		Log(LOG_ERRORS, "Unknown Exception loading ResetEOLStatusBytes\n");
	}
	// Load the registers to reset at the end of a test
	try
	{
		m_resetPCMRegisters.DeepCopy(config->getChild("Setup/Parameters/ResetPCMRegisters")->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading registers to reset at the end of a test
		Log(LOG_ERRORS, "XmlException loading ResetPCMRegisters - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading registers to reset at the end of a test
		Log(LOG_ERRORS, "BepException loading ResetPCMRegisters - %s\n", err.what());
	}
	catch(...)
	{	// Error loading registers to reset at the end of a test
		Log(LOG_ERRORS, "Error loading ResetPCMRegisters\n");
	}
	// Load the registers to reset for development testing
	try
	{
		m_defaultRegisters.DeepCopy(config->getChild("Setup/Parameters/DevelopmentRegisters")->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading registers to reset at the end of a test
		Log(LOG_ERRORS, "XmlException loading DevelopmentRegisters - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading registers to reset for development testing
		Log(LOG_ERRORS, "BepException loading DevelopmentRegisters - %s\n", err.what());
	}
	catch(...)
	{	// Error loading registers to reset for development testing
		Log(LOG_ERRORS, "Error loading DevelopmentRegisters\n");
	}
	// Load the initial SRI mileage register pattern
	try
	{
		m_SRIInitialSetting.DeepCopy(config->getChild("Setup/Parameters/SRIInitialSetting")->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading initial SRI mileage register pattern
		Log(LOG_ERRORS, "XmlException loading SRIInitialSetting - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading initial SRI mileage register pattern
		Log(LOG_ERRORS, "BepException loading SRIInitialSetting - %s\n", err.what());
	}
	catch(...)
	{	// Error loading initial SRI mileage register pattern
		Log(LOG_ERRORS, "Error loading SRIInitialSetting\n");
	}
	// Load the first tumble SRI mileage register pattern
	try
	{
		m_SRIFirstTumble.DeepCopy(config->getChild("Setup/Parameters/SRIFirstTumble")->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading first tumble SRI mileage register pattern
		Log(LOG_ERRORS, "XmlException loading SRIFirstTumble - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading first tumble SRI mileage register pattern
		Log(LOG_ERRORS, "BepException loading SRIFirstTumble - %s\n", err.what());
	}
	catch(...)
	{	// Error loading first tumble SRI mileage register pattern
		Log(LOG_ERRORS, "Error loading SRIFirstTumble\n");
	}
	// Load the second tumble SRI mileage register pattern
	try
	{
		m_SRISecondTumble.DeepCopy(config->getChild("Setup/Parameters/SRISecondTumble")->getChildren());
	}
	catch(XmlException &err)
	{	// Error loading second tumble SRI mileage register pattern
		Log(LOG_ERRORS, "XmlException loading SRISecondTumble - %s\n", err.what());
	}
	catch(BepException &err)
	{	// Error loading second tumble SRI mileage register pattern
		Log(LOG_ERRORS, "BepException loading SRISecondTumble - %s\n", err.what());
	}
	catch(...)
	{	// Error loading second tumble SRI mileage register pattern
		Log(LOG_ERRORS, "Error loading SRISecondTumble\n");
	}
	// Get the various types of manual transmissions
	try
	{
		m_manualTransmissionTypes.DeepCopy(config->getChild("Setup/Parameters/ManualTransmissionTypes")->getChildren());
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Could not find manual transmission types, just using Manual");
		XmlNode *trans = new XmlNode("Manual","");
		m_manualTransmissionTypes.addNode(trans);
	}
	// Load the Valid ETC Speed Control Types from the configuration file
	try
	{
		const XmlNode *validEtcTypes = config->getChild("Setup/Parameters/ValidETCSpeedControlTypes");
		m_validETCSpeedControlTypes.DeepCopy(validEtcTypes->getChildren());
		// Check if vehicle is equipped with ETC Speed Control Component
		// and set member variable.
		CheckForValidETCType();
	}
	catch(XmlException &err)
	{
		try
		{	// Error loading Valid ETC Speed Control Types
			Log(LOG_ERRORS, "XmlException loading ValidETCSpeedControlTypes, looking for ETCBuildOption tag - %s\n", err.what());
			SetETCEquipped(!SystemRead(GetDataTag("ETCBuildOption")).compare(GetDataTag("ETCEquippedValue")));
		}
		catch(XmlException &err)
		{	// Default to not ETC type
			Log(LOG_ERRORS, "XmlException loading ETCBuildOption, default to Not ETC Equipped - %s\n", err.what());
			SetETCEquipped(false);
		}
	}

	// Check to see if Configuration file parameter is set to background monitor testing for redundant brake switch
	try
	{
		SetTestRedundantBrakeSwitch(GetParameterBool("SetRedundantBrakeSwitchEquipped"));
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlException loading value for TestRedundantBrakeMonitor, setting to false: %s\n", excpt.GetReason());
		SetTestRedundantBrakeSwitch(false);
	}
	// Check to see if Configuration file parameter is set to background monitor testing for clutch Upstop switch
	try
	{
		SetTestClutchUpstopSwitch(GetParameterBool("SetClutchUpstopSwitchEquipped"));
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlException loading value for TestClutchUpstopMonitor, setting to false: %s\n", excpt.GetReason());
		SetTestClutchUpstopSwitch(false);
	}

	Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::InitializeHook() - Exit\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericEmissionsTCTemplate<VehicleModuleType>::ReloadConfiguration(void)
{
	Log(LOG_FN_ENTRY, "GenericEmissionsTCTemplate::ReloadConfiguration()\n");
	//Clear all items that were copied
	m_resetEOLStatusRegisters.clear(true);
	m_resetPCMRegisters.clear(true);
	m_defaultRegisters.clear(true);
	m_SRIInitialSetting.clear(true);
	m_SRIFirstTumble.clear(true);
	m_SRISecondTumble.clear(true);
	m_manualTransmissionTypes.clear(true);
	m_validETCSpeedControlTypes.clear(true);
	//call baseclass
	GenericTCTemplate<VehicleModuleType>::ReloadConfiguration();
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::ReloadConfig() done\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericEmissionsTCTemplate<VehicleModuleType>::WarmInitialize()
{
	Log( LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::WarmInitialize()\n");

	// Reset data members that will get 'set' from test steps
	m_isPreviousPass = false;
	m_etcVehicle = false;
	m_engineSize = std::string("Unknown");
	m_vehicleType = std::string("Unknown");
	m_emissionsType = std::string("Unknown");
	m_emissionsYear = std::string("Unknown");
	m_moduleType = std::string("Unknown"),
				   m_currentOdometerReading = 0x00;

	// Call the base class to complete initialization
	GenericTCTemplate<VehicleModuleType>::WarmInitialize();

	// Load the Valid ETC Speed Control Types from the configuration file
	try
	{
		const XmlNode *validEtcTypes = m_parameters.getNode("ValidETCSpeedControlTypes");
		m_validETCSpeedControlTypes.DeepCopy(validEtcTypes->getChildren());
		// Check if vehicle is equipped with ETC Speed Control Component
		// and set member variable.
		CheckForValidETCType();
	}
	catch(XmlException &err)
	{
		try
		{	// Error loading Valid ETC Speed Control Types
			Log(LOG_ERRORS, "XmlException loading ValidETCSpeedControlTypes, looking for ETCBuildOption tag - %s\n", err.what());
			SetETCEquipped(!SystemRead(GetDataTag("ETCBuildOption")).compare(GetDataTag("ETCEquippedValue")));
		}
		catch(XmlException &err)
		{	// Default to not ETC type
			Log(LOG_ERRORS, "XmlException loading ETCBuildOption, default to Not ETC Equipped - %s\n", err.what());
			SetETCEquipped(false);
		}
	}
	//initialize to not previously passed
	SetPreviousPassStatus(false);
	Log( LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::WarmInitialize()\n");
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
BEP_STATUS_TYPE GenericEmissionsTCTemplate<VehicleModuleType>::InitializeModuleInterface(void)
{
	XmlParser parser;
	BEP_STATUS_TYPE status;
	if(GetParameterBool("LoadGatewayModuleConfig"))
	{
		try
		{
			if(m_gatewayModule != NULL)
			{
				m_gatewayModule->CloseCom();
				delete m_gatewayModule;
				m_gatewayModule = NULL;
			}
			m_gatewayModule = new VehicleModuleType();
			XmlNode *gatewayConfigFile = NULL;
			gatewayConfigFile = const_cast<XmlNode *>(parser.ReturnXMLDocument(GetRootDir() + GetParameter("GatewayModuleConfigFile")));
			m_gatewayModule->Initialize(gatewayConfigFile);
		}
		catch(BepException &err)
		{
			Log(LOG_ERRORS, "BepException in TpimTC::InitializeHook(): Mcpa Config file %s\n", err.GetReason());
		}
	}
	status = GenericTCTemplate<VehicleModuleType>::InitializeModuleInterface();
	return status;
}

template <class VehicleModuleType>
inline const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsPreviousPass()
{
	return m_isPreviousPass;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
void GenericEmissionsTCTemplate<VehicleModuleType>::CheckForValidETCType()
{
	bool runEtcTest = false;	// Determine if is ETC test should run or Non-ETC reads (test) used
	XmlNodeMapItr iter;			// Iterate through the possible valid ETC Speed Control types for this test

	for(iter = m_validETCSpeedControlTypes.begin(); (iter != m_validETCSpeedControlTypes.end()) && !runEtcTest; iter++)
	{	// Compare the broadcast Speed Control Type against the current config file valid Speed Control Types
		runEtcTest = !iter->second->getValue().compare(SystemRead(GetDataTag("BroadcastSpeedControlType")));
	}
	SetETCEquipped(runEtcTest);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsIdleRPM(void)
{
	bool engineAtIdle = false;
	INT32 engineRPM = 0;
	// Attempt to read the engine RPM from the module
	try
	{	// Read the engine RPM from the module
		BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag("EngineRunningMessage"), engineRPM);
		// Determine if the engine is running
		engineAtIdle = ((BEP_STATUS_SUCCESS == moduleStatus) &&
						m_analyze.CompareData(engineRPM, GetParameterInt("MaximumIdleRPM"), LESS_EQUAL) &&
						m_analyze.CompareData(engineRPM, GetParameterInt("MinimumIdleRPM"), GREATER_EQUAL));
		Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::IsIdleRPM() - Min = %d, Max = %d, Actual = %d\n",
			GetParameterInt("MinimumIdleRPM"),GetParameterInt("MaximumIdleRPM"),engineRPM);
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in IsIdleRPM() - %s\n", exception.message().c_str());
		engineAtIdle = false;
	}
	// Return the status
	return engineAtIdle;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsThrottleClosed(void)
{
	bool throttleClosed = false;
	float tps = 0.0;
	float tpsSecondary = 0.0;
	// Determine which message to send
	string dataTag = (m_etcVehicle ? "ThrottleClosedVoltageMessage5.7L" : "ReadThrottlePositionNonETC");
	string closedThrottleReading = (m_etcVehicle ? "ClosedThrottleVoltage5.7L" : "ClosedThrottleVoltage");
	string closedThrottleSecondaryReading = (m_etcVehicle ? "SecondaryClosedThrottleVoltage5.7L" : "SecondaryClosedThrottleVoltage");
	// Attempt to read the throttle position from the module
	try
	{	// Read the throttle position from the module
		BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag(dataTag), tps);
		if(GetParameterBool("CheckSecondaryTPSSensor") && (moduleStatus == BEP_STATUS_SUCCESS))
		{
			dataTag = (m_etcVehicle ? "SecondaryThrottleClosedVoltageMessage5.7L" : "SecondaryReadThrottlePositionNonETC");
			moduleStatus = m_vehicleModule.ReadModuleData(GetDataTag(dataTag), tpsSecondary);
			tps = (tps >= tpsSecondary) ? tps : tpsSecondary;
		}
		SystemWrite(GetDataTag("TPSValue"), tps);
		// Determine if the throttle is closed
		throttleClosed = ((BEP_STATUS_SUCCESS == moduleStatus) &&
						  m_analyze.CompareData(tps, GetParameterFloat(closedThrottleReading), LESS_EQUAL));
		if(GetParameterBool("CheckSecondaryTPSSensor") && throttleClosed)
		{
			if(GetParameterBool("CheckSecondaryClosedThrottleVoltage"))
			{
				closedThrottleReading = (m_etcVehicle ? "SecondaryClosedThrottleVoltage5.7L" : "SecondaryClosedThrottleVoltage");
			}
			throttleClosed = ((BEP_STATUS_SUCCESS == moduleStatus) &&
							  m_analyze.CompareData(tps, GetParameterFloat(closedThrottleReading), LESS_EQUAL));
		}
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in IsThrottleClosed() - %s\n", exception.message().c_str());
		throttleClosed = false;
	}
	// Return the status
	return throttleClosed;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsThrottleAtoDClosed(void)
{
	bool throttleClosed = false;
	float tps = 0.0;
	// Determine which message to send
	string dataTag = (m_etcVehicle ? "ThrottleClosedVoltageMessage5.7L" : "ThrottleClosedVoltageMessage");
	string closedThrottleReading = (m_etcVehicle ? "ClosedThrottleVoltage5.7L" : "ClosedThrottleAtoDVoltage");
	// Attempt to read the throttle position from the module
	try
	{	// Read the throttle position from the module
		BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag(dataTag), tps);
		SystemWrite(GetDataTag("TPSValue"), tps);
		// Determine if the throttle is closed
		throttleClosed = ((BEP_STATUS_SUCCESS == moduleStatus) &&
						  m_analyze.CompareData(tps, GetParameterFloat(closedThrottleReading), LESS_EQUAL));
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in IsThrottleClosed() - %s\n", exception.message().c_str());
		throttleClosed = false;
	}
	// Return the status
	return throttleClosed;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsTransmissionInNeutral(void)
{
	bool transmissionInNeutral = false;
	UINT8 neutralCellID = 0x00;
	// Attempt to read the neutral status from the module
	try
	{	// Read the neutral cell id from the module
		BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadNeutralCellID"), neutralCellID);
		// Determine if the transmission is in Neutral
		transmissionInNeutral = ((BEP_STATUS_SUCCESS == moduleStatus) &&
								 m_analyze.CompareData(neutralCellID, (UINT8)atoh(GetParameter("NeutralCellID").c_str()), EQUAL));
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in IsTransmissionInNeutral() - %s\n", exception.message().c_str());
		transmissionInNeutral = false;
	}
	// Return the result
	return transmissionInNeutral;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsAirConditioningOff(void)
{
	bool acIsOff = false;
	string acStatus = "Unknown";
	// Attempt to read the A/C status fro mthe module
	try
	{	// Read the A/C status from the module
		BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadAirConditionerStatus"), acStatus);
		acIsOff = ((BEP_STATUS_SUCCESS == moduleStatus) && m_analyze.CompareData(acStatus, "Off", EQUAL));
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in IsAirConditioningOff() - %s\n", exception.message().c_str());
		acIsOff = false;
	}
	// Return the result
	return acIsOff;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::IsManualTransmission(void)
{
	return(m_manualTransmissionTypes.find(ReadSubscribeData(GetDataTag("TransmissionType"))) != m_manualTransmissionTypes.end());
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::CheckClosedThrottle(void)
{
	bool throttleClosed = false;
	bool promptDisplayed = false;
	// Start the timer
	SetStartTime();
	// Wait for closed throttle
	do
	{	// Check if the throttle is closed and prompt if neccessary
		throttleClosed = IsThrottleClosed();
		if(!throttleClosed && !promptDisplayed)
		{	// Throttle not closed, prompt the operator
			DisplayPrompt(GetPromptBox("ThrottleClosed"), GetPrompt("ThrottleClosed"),
						  GetPromptPriority("ThrottleClosed"));
			promptDisplayed = true;
		}
		else if(promptDisplayed && throttleClosed)
		{	// Remove the prompt if it has been displayed
			RemovePrompt(GetPromptBox("ThrottleClosed"), GetPrompt("ThrottleClosed"),
						 GetPromptPriority("ThrottleClosed"));
			promptDisplayed = false;
		}
		if(!throttleClosed)	 BposSleep(GetTestStepInfoInt("ScanDelay"));
	} while(TimeRemaining() && !throttleClosed);
	// Check if prompt should be removed
	if(promptDisplayed)
	{	// Remove the prompt from the display
		RemovePrompt(GetPromptBox("ThrottleClosed"), GetPrompt("ThrottleClosed"),
					 GetPromptPriority("ThrottleClosed"));
		promptDisplayed = false;
	}
	// Log the status
	if(!throttleClosed)	Log(LOG_ERRORS, "Timeout waiting for closed throttle!\n");
	else				 Log(LOG_DEV_DATA, "Throttle is closed\n");
	// Return the status of closed throttle
	return throttleClosed;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const bool GenericEmissionsTCTemplate<VehicleModuleType>::AISIdleConditions(void)
{
	bool aisIdleCondition = false;
	// Ensure the vehicle is at idle conditions
	if(VehicleAtIdleState())
	{	// Vehicle at idle, check for neutral gear and A/C off
		SetStartTime();
		bool transmissionPromptDisplayed = false;
		bool acPromptDisplayed = false;
		bool transmissionInPosition = false;
		bool acOff = false;
		string transmissionpromptTag = GetParameterBool("AISTransmissionInNeutral") ? "ShiftToNeutral" : "ShiftToDrive";
		do
		{	// Check for transmission in position
			if(GetParameterInt("SkipNeutralCheck"))
			{
				transmissionInPosition = true;
			}
			else
			{
				transmissionInPosition = GetParameterBool("AISTransmissionInNeutral") ? IsTransmissionInNeutral() : !IsTransmissionInNeutral();
			}
			// Check if the prompt should be displayed or removed
			if(!transmissionInPosition && !transmissionPromptDisplayed)
			{	// Display the prompt for the operator
				DisplayPrompt(GetPromptBox(transmissionpromptTag), GetPrompt(transmissionpromptTag), GetPromptPriority(transmissionpromptTag));
				transmissionPromptDisplayed = true;
			}
			else if(transmissionInPosition && transmissionPromptDisplayed)
			{	// Remove the prompt from the display
				RemovePrompt(GetPromptBox(transmissionpromptTag), GetPrompt(transmissionpromptTag), GetPromptPriority(transmissionpromptTag));
				transmissionPromptDisplayed = false;
			}
			if(GetParameterInt("SkipAirConditioningCheck"))
			{
				acOff = true;
			}
			else
			{	// Check if the A/C is off
				acOff = IsAirConditioningOff();
			}
			// Check of the operator needs to be prompted
			if(!acOff && !acPromptDisplayed)
			{	// Display the prompt for the operator
				DisplayPrompt(GetPromptBox("TurnOffAC"), GetPrompt("TurnOffAC"), GetPromptPriority("TurnOffAC"));
				acPromptDisplayed = true;
			}
			else if(acOff && acPromptDisplayed)
			{	// Reomve the prompt from the display
				RemovePrompt(GetPromptBox("TurnOffAC"), GetPrompt("TurnOffAC"), GetPromptPriority("TurnOffAC"));
				acPromptDisplayed = false;
			}
			// Decide if we need to keep checking
		} while(TimeRemaining() && (!transmissionInPosition || !acOff));
		// Check if any prompts need to be removed
		if(transmissionPromptDisplayed)
		{	// Remove the prompt from the display
			RemovePrompt(GetPromptBox(transmissionpromptTag), GetPrompt(transmissionpromptTag), GetPromptPriority(transmissionpromptTag));
			transmissionPromptDisplayed = false;
		}
		if(acPromptDisplayed)
		{	// Remove the prompt from the display
			RemovePrompt(GetPromptBox("TurnOffAC"), GetPrompt("TurnOffAC"), GetPromptPriority("TurnOffAC"));
			acPromptDisplayed = false;
		}
		// Check to make sure all conditions are satisfied
		aisIdleCondition = transmissionInPosition && acOff;
		// Log the Data
		Log(LOG_DEV_DATA, "AIS Idle conditions satisfied: %s\n\tTransmission in Neutral: %s\n\tA/C Off: %s\n",
			(aisIdleCondition ? testPass.c_str() : testFail.c_str()),
			(transmissionInPosition ? testPass.c_str() : testFail.c_str()),
			(acOff ? testPass.c_str() : testFail.c_str()));
	}
	else
	{	// Vehicle not at idle, fail
		Log(LOG_ERRORS, "Error - Vehicle not at idle\n");
	}
	// Return the result
	return aisIdleCondition;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
bool GenericEmissionsTCTemplate<VehicleModuleType>::FinalAISIdleCondtions(void)
{
	bool transInNeutral = IsTransmissionInNeutral();
	return(IsIdleRPM() && IsThrottleAtoDClosed() && IsAirConditioningOff() &&
		   (transInNeutral || (!GetParameterBool("AISTransmissionInNeutral") && !transInNeutral)));
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::WaitForEngineRunning(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	// Log the entry
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Wait for the operator to start the vehicle
	testResult = CheckEngineRunning() ? testPass : testFail;
	testResultCode = testPass == testResult ? testResultCode : GetFaultCode("EngineRunningTimeout");
	testDescription = testPass == testResult ? testDescription : GetFaultDescription("EngineRunningTimeout");
	// Report the result of the test
	SendTestResult(testResult, testDescription, testResultCode);
	//Checking to make sure engine RPMs are idling before testing if engine is running
	INT32 maxIdleChecks = GetParameterInt("MaximumIdleBeforeRunningChecks");
	INT32 idleWaitTime = GetParameterInt("IdleBeforeRunningWaitTime");

	INT32 idleCount = 0;
	while((idleCount < maxIdleChecks) && TimeRemaining() && (BEP_STATUS_SUCCESS == StatusCheck()))
	{	//Check if engine is idle
		if(IsIdleRPM())
		{
			idleCount++;
		}
		else
		{
			idleCount = 0;
		}
		BposSleep(idleWaitTime);
	}

	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::UpdateEOLStatusRegister(const UINT16 &statusRegister,
																			  const UINT16 &testBit,
																			  const string &testResult)
{	// Translate the test result into a format for the module
	EEPROM_TEST_RESULT eepromResult = (testResult == BEP_PASS_STATUS ? EEPROM_PASS : EEPROM_FAIL);
	BEP_STATUS_TYPE status = BEP_STATUS_ERROR;
	// Attempt to update the result in the module
	try
	{
		status = m_vehicleModule.UpdateEEPROMTestBit(statusRegister, testBit, eepromResult);
		// Log the status of the update
		Log(LOG_DEV_DATA, "Update EEPROM Test Bit status - %s\n", ConvertStatusToResponse(status).c_str());
	}
	catch(ModuleException &exception)
	{	// Exception reading data
		Log(LOG_ERRORS, "Module exception in UpdateEOLStatusRegister() - %s\n", exception.message().c_str());
		status = BEP_STATUS_SOFTWARE;
	}
	// Return the status of the update
	return(BEP_STATUS_SUCCESS == status ? testPass : testFail);
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::Setup(void)
{	// Ensure the vehicle is connected
	Log(LOG_FN_ENTRY, "%s::%s - Enter\n", GetComponentName().c_str(), GetTestStepName().c_str());
	string testResult(BEP_TESTING_STATUS);
	string vehicleConnectedResult(BEP_TESTING_STATUS);
	string testResultCode("0000");
	string testDescription(GetTestStepInfo("Description"));
	if(CheckCableConnect())
	{	//Check if we need to set up communications link
		if(GetParameterBool("DelayModuleInitialization"))
		{	//Set up comminictions link (right or left)
			testResult = GenericTCTemplate<VehicleModuleType>::Setup();
			testResultCode = (testPass == testResult ? "0000" : GetFaultCode("SoftwareFailure"));
			testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("SoftwareFailure"));
			Log(LOG_ERRORS, "Module Interface initialization: %s", testResult.c_str());
		}
		else
		{
			Log(LOG_DEV_DATA, "Module Interface already initialized.");
			testResult = testPass;
		}
		// Determine if we are ready to start talking to the module
		if(!testResult.compare(testPass))
		{
			testResult = VehicleConnected() ? testPass : testFail;
			testResultCode = testPass == testResult ? "0000" : GetFaultCode("VehicleNotConnected");
			testDescription = testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("VehicleNotConnected");
			vehicleConnectedResult = testResult;
		}
		else
		{
			Log(LOG_ERRORS, "Could not initialize module interface!  ABORT!");
		}
	}
	else
	{
		Log(LOG_ERRORS, "Timeout waiting for cable to be connected");
		testResult = testTimeout;
		testResultCode = GetFaultCode("PreconditionTimeout");
		testDescription = GetFaultDescription("PreconditionTimeout");
	}
	// Report the result
	SendTestResult(testResult, testDescription, testResultCode);
	if(GetParameterBool("AbortIgnitionNotOn") && vehicleConnectedResult != testPass)
	{//Abort all testing
		Log(LOG_DEV_DATA, "Vehicle Connection Failure result: %s:", vehicleConnectedResult.c_str());
		//Prompt Driver
		DisplayPrompt(GetPromptBox("PCMCommFailure"), GetPrompt("PCMCommFailure"), GetPromptPriority("PCMCommFailure"));
		testResult = testAbort;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s::%s - Exit\n", GetComponentName().c_str(), GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckPartNumber()
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update Part number module result";
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckPartNumber()\n");
	// Check if this step should be performed
	if(!ShortCircuitTestStep())
	{	// All preconditions satisfied, call base class to check the Part Number
		testResult = GenericTCTemplate<VehicleModuleType>::CheckPartNumber();
		Log(LOG_DEV_DATA, "Check Part Number: %s\n", testResult.c_str());
		// Determine if we need to skip the module update
		if(!GetParameterBool("SkipPartNumberResultUpdate"))
		{
			// Update the result in the module if not previous pass or if previous pass and fail
			if(!IsPreviousPass() || (IsPreviousPass() && (testFail == testResult)))
			{	// Update the module result
				UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLPartNumberResultRegister")).c_str());
				UINT16 partNumberTestBit = GetParameterInt("PartNumberResultBit");
				moduleResult = UpdateEOLStatusRegister(eolStatusRegister, partNumberTestBit, testResult);
			}
			else
			{	// Skip the module update
				moduleResult = testSkip;
			}
		}
		else
		{
			Log(LOG_DEV_DATA, "Skipping part number result update to module by parameter.");
			moduleResult = testSkip;
		}
		// Check the status of the update
		moduleCode = ((moduleResult == testPass) || (moduleResult == testSkip)) ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
		moduleDescription = ((moduleResult == testPass) || 
							 (moduleResult == testSkip)) ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
		Log(LOG_DEV_DATA, "Module update: %s\n", moduleResult.c_str());
		// Send the test results
		SendSubtestResult("UpdateModulePartNumberResult", moduleResult, moduleDescription, moduleCode);
		// Determine the overall result of the step
		if((testResult == testPass) && ((moduleResult != testPass) && (moduleResult != testSkip)))	testResult = moduleResult;
	}
	else
	{	// Skip this step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "%s test step has been short circuited\n", GetTestStepName().c_str());
	}
	// Return the results
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckPartNumber(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CommandSecurityUnlock(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Test in progress";
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CommandSecurityUnlock()\n");
	// Vehicle is connected, unlock security
	for(INT32 unlockAttempt = 0; (unlockAttempt < GetParameterInt("SecurityUnlockAttempts")) &&
	   (moduleStatus != BEP_STATUS_SUCCESS); unlockAttempt++)
	{	// Command the module to unlock security
		try
		{
			moduleStatus = m_vehicleModule.UnlockModuleSecurity();
			// If unlock was not successful, wait before attempting again
			if(BEP_STATUS_SUCCESS != moduleStatus) BposSleep(GetParameterInt("SecurityUnlockDelay"));
		}
		catch(ModuleException &exception)
		{	// Exception during security unlock
			Log(LOG_ERRORS, "Module Exception during CommandSecurityUnlock() - %s\n", exception.message().c_str());
			moduleStatus = BEP_STATUS_SOFTWARE;
		}
	}
	// Determine the test result
	testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testAbort);
	testResultCode = (testResult == testPass ? "0000" : GetFaultCode("SecurityUnlockFailed"));
	testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("SecurityUnlockFailed"));
	// Log the status
	Log(LOG_DEV_DATA, "Module Security Unlocked - %s\n", testResult.c_str());
	// Send the test result
	SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CommandSecurityUnlock()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::DeterminePreviousPass(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Test In Progress";
	bool isPreviousPass = false;
	bool isInFieldMode = false;
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::DeterminePreviousPass()\n");
	if(!IsPreviousPass())
	{	// Vehicle is connected, determine if the vehicle is previous pass
		try
		{
			moduleStatus = m_vehicleModule.GetInfo("IsPreviousPass", isPreviousPass);
		}
		catch(ModuleException &exception)
		{	// Exception during is previous pass
			Log(LOG_ERRORS, "Module Exception during DeterminePreviousPass() - %s\n", exception.message().c_str());
			moduleStatus = BEP_STATUS_SOFTWARE;
		}
		if(GetParameterBool("CheckFieldModeBit"))
		{
			if(moduleStatus == BEP_STATUS_SUCCESS)
			{	// Store the previous pass status and log the data
				try
				{
					moduleStatus = m_vehicleModule.GetInfo("IsInFieldMode", isInFieldMode);
				}
				catch(ModuleException &exception)
				{	// Exception checking field mode
					Log(LOG_ERRORS, "Module Exception during DeterminePreviousPass() - %s\n", exception.message().c_str());
					moduleStatus = BEP_STATUS_SOFTWARE;
				}
			}
			else
			{	// Communication failure reading previous pass status
				Log(LOG_ERRORS, "Communication error attempting to determine previous pass status\n");
			}
		}
		// Check the status of the module read
		testResult = (BEP_STATUS_SUCCESS == moduleStatus ? testPass : testAbort);
		testResultCode = (testResult == testPass ? "0000" : GetFaultCode("CommunicationFailure"));
		testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
		if(testPass == testResult)
		{	// Store the previous pass status and log the data
			if(!GetParameterBool("CheckFieldModeBit"))
			{// Store the previous pass status if test is pass
				SetPreviousPassStatus(isPreviousPass);
				Log(LOG_DEV_DATA, "Emissions Previous Pass: %s\n", isPreviousPass ? "True" : "False");
			}
			else
			{
				if(isInFieldMode && isPreviousPass)
				{// Store the previous pass status if test is pass
					SetPreviousPassStatus(isPreviousPass);
					Log(LOG_DEV_DATA, "Emissions Previous Pass: %s\n", isPreviousPass ? "True" : "False");
				}
				else if(isInFieldMode && !isPreviousPass)
				{//bad state display prompt
					DisplayPrompt(GetPromptBox("PreviousPcmFailure"), GetPrompt("PreviousPcmFailure"), GetPromptPriority("PreviousPcmFailure"));
					Log(LOG_ERRORS, "Previous PCM Failure, (vehicle in field mode and not ok to ship), aborting\n");
					testResult = testFail;
					testResultCode = GetFaultCode("PreviousPcmFailure");
					testDescription = GetFaultDescription("PreviousPcmFailure");
					if(GetParameterBool("AbortForBadStatus"))
					{
						SendTestResultWithDetail(testResult, testDescription, testResultCode,
												 "PreviousPass", (isPreviousPass ? "True" : "False"), "bool",
												 "InFieldMode", (isInFieldMode ? "True" : "False"), "bool");
						testResult = testAbort;
					}
				}
				else
				{//Not in field mode run all tests
					SetPreviousPassStatus(false);
				}
			}
		}
		else
		{	// Communication failure reading previous pass status
			Log(LOG_ERRORS, "Communication error attempting to determine previous pass status\n");
		}
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "PreviousPass", (isPreviousPass ? "True" : "False"), "bool",
								 "InFieldMode", (isInFieldMode ? "True" : "False"), "bool");
	}
	else
	{	// Skip the test
		testResult = testSkip;
		Log(LOG_FN_ENTRY, "Skipping test step - %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::IsPreviousPass() - %s\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::VerifyModuleType(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if this step should be skipped
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::VerifyModuleType()\n");
	if(!ShortCircuitTestStep())
	{	// Preconditions have been satisfied
		try
		{	// Read the module type from the module
			moduleStatus = m_vehicleModule.GetInfo(GetDataTag("ReadModuleType"), m_moduleType);
			// Check the status of the read
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Good data, verify module type
				testResult = m_analyze.CompareData(m_moduleType, GetParameter("ModuleType"), EQUAL) ? testPass : testAbort;
				testResultCode = (testPass == testResult ? "0000" : GetFaultCode("IncorrectModuleType"));
				testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("IncorrectModuleType"));
				Log(LOG_DEV_DATA, "Verify Module Type: %s - Expected: %s, Actual: %s\n",
					testResult.c_str(), GetParameter("ModuleType").c_str(), m_moduleType.c_str());
			}
			else
			{	// Communication failure reading module type
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
				Log(LOG_ERRORS, "Error reading module type - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			}
		}
		catch(ModuleException &exception)
		{	// Module exception
			Log(LOG_ERRORS, "Module Exception in CheckUplineRegisters - %s\n", exception.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Report the test results
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "ActualModuleType", m_moduleType, "",
								 "ExpectedModuleType", GetParameter("ModuleType"), "");
	}
	else
	{	// Skip this step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "%s test step has been short circuited\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::VerifyModuleType()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::DetermineModuleConfig(void)
{
	string testResult = testPass;
	string testDescription = GetTestStepInfo("Description");
	string testResultCode = "0000";
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Determine if this step should be skipped
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::DetermineModuleConfig()\n");
	if(!ShortCircuitTestStep())
	{	// Preconditions satisfied, attempt to read the data from the module
		moduleStatus = m_vehicleModule.GetInfo(GetDataTag("EngineConfiguration"), m_engineSize);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS != moduleStatus)
		{	// Error reading engine configuration
			testResult = testFail;
			Log(LOG_ERRORS, "Error reading engine configuration - status: %s\n",
				ConvertStatusToResponse(moduleStatus).c_str());
		}
		// Read the vehicle type from the module
		moduleStatus = m_vehicleModule.GetInfo(GetDataTag("VehicleType"), m_vehicleType);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS != moduleStatus)
		{	// Error reading engine configuration
			testResult = testFail;
			Log(LOG_ERRORS, "Error reading vehicle type - status: %s\n",
				ConvertStatusToResponse(moduleStatus).c_str());
		}
		// Read the emissions model year
		moduleStatus = m_vehicleModule.GetInfo(GetDataTag("EmissionsModelYear"), m_emissionsYear);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS != moduleStatus)
		{	// Error reading engine configuration
			testResult = testFail;
			Log(LOG_ERRORS, "Error reading emissions model year - status: %s\n",
				ConvertStatusToResponse(moduleStatus).c_str());
		}
		// Read the emissions configuration
		moduleStatus = m_vehicleModule.GetInfo(GetDataTag("EmissionsConfiguration"), m_emissionsType);
		// Check the status of the read
		if(BEP_STATUS_SUCCESS != moduleStatus)
		{	// Error reading engine configuration
			testResult = testFail;
			Log(LOG_ERRORS, "Error reading emissions configuration - status: %s\n",
				ConvertStatusToResponse(moduleStatus).c_str());
		}
		// Determine the test results
		testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
		testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
		Log(LOG_DEV_DATA, "Read Module Configuration: %s\n\tEngine Type:%s\n\tVehicle Type: %s\n\t"
			"Emissions Year: %s\n\tEmissions Config: %s\n",
			testResult.c_str(), m_engineSize.c_str(), m_vehicleType.c_str(),
			m_emissionsYear.c_str(), m_emissionsType.c_str());
		// Send the test results
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "EmissionsModelYear", m_emissionsYear, "",
								 "EmissionsType", m_emissionsType, "",
								 "EngineType", m_engineSize, "",
								 "VehicleType", m_vehicleType, "");

		// Send the EngineSize to the CCRT Client
		if( BEP_STATUS_SUCCESS != SystemWrite(GetDataTag("EngineSize"), m_engineSize) )
		{
			// In the event of a failure to write EngineSize, Log an error
			Log(LOG_ERRORS, "Unable to publish EngineSize: %s to CCRT Client\n", m_engineSize.c_str() );
		}
	}
	else
	{	// Skip this step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "%s test step has been short circuited\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::DetermineModuleConfig()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckUplineRegisters(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;
	bool vinUplineComplete = false;
	bool odometerUplineComplete = false;
	UINT16 uplineRegister = 0xFFFF;
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckUplineRegisters()\n");
	// Check if the module is previous pass
	if(!IsPreviousPass() && !ShortCircuitTestStep())
	{	// Preconditions met, read upline register
		try
		{	// Read the upline register
			moduleStatus = m_vehicleModule.GetInfo("ReadUplineRegister", uplineRegister);
			// Determine the test result
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Good data, check the bits
				if(!m_analyze.CompareData(uplineRegister, (UINT16)atoh(GetParameter("UplineTestComplete").c_str()), ANY_BITS_SET))
				{	// All upline tests complete
					testResult = testPass;
					vinUplineComplete = true;
					odometerUplineComplete = true;
				}
				else
				{	// Upline tests not complete
					testResult = testFail;
					vinUplineComplete = !m_analyze.CompareData(uplineRegister,
															   (UINT16)atoh(GetParameter("VINUplineComplete").c_str()),
															   ANY_BITS_SET);
					odometerUplineComplete = !m_analyze.CompareData(uplineRegister,
																	(UINT16)atoh(GetParameter("OdometerUplineComplete").c_str()),
																	ANY_BITS_SET);
				}
				Log(LOG_DEV_DATA, "Upline tests complete: %s - VIN: %s, Odometer: %s\n", testResult.c_str(),
					(vinUplineComplete ? testPass.c_str() : testFail.c_str()),
					(odometerUplineComplete ? testPass.c_str() : testFail.c_str()));
			}
			else
			{	// Communication failure
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
		}
		catch(ModuleException &exception)
		{	// Module exception
			Log(LOG_ERRORS, "Module Exception in CheckUplineRegisters - %s\n", exception.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		char temp[16];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OdometerUplineComplete", (odometerUplineComplete ? testPass : testFail), "string",
								 "VINUplineComplete", (vinUplineComplete ? testPass : testFail), "string",
								 "UplineRegister", CreateMessage(temp, sizeof(temp), "0x%04X", uplineRegister), "");
	}
	else
	{	// Previous pass, skip this step
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing CheckUplineRegisters test\n");
		testResult = testSkip;
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckUplineRegisters()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::ProgramEOLStatus(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Determine if this test step should be performed
	if(!IsPreviousPass() && ShortCircuitTestStep())
	{	// Attempt to update all status registers
		try
		{	// Update all EOL Status registers
			for(XmlNodeMapItr iter = m_resetEOLStatusRegisters.begin();
			   (iter != m_resetEOLStatusRegisters.end()) && (BEP_STATUS_SUCCESS == moduleStatus);
			   iter++)
			{	// Read the status register from the module
				UINT16 readStatusRegister = 0x0000;
				UINT16 writeStatusRegister = 0x0000;
				string readStatusRegisterTag = "Read" + iter->second->getName();
				string writeStatusRegisterTag = "Write" + iter->second->getName();
				moduleStatus = m_vehicleModule.GetInfo(readStatusRegisterTag, readStatusRegister);
				// Check the status of the read
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good data, update the status byte
					writeStatusRegister = m_analyze.ConvertData((UINT32)readStatusRegister,
																(UINT32)atoh(iter->second->getValue().c_str()),
																SET_BITS);
					// Write the updated status byte back to the module
					moduleStatus = m_vehicleModule.UpdateEOLStatusByte(atoh(iter->second->getValue().c_str()), writeStatusRegister);
					Log(LOG_DEV_DATA, "%s -- Initial: %04X, Updated: %04X\n", iter->second->getName().c_str(),
						readStatusRegister, writeStatusRegister);
					// Check the status of the write
					if(BEP_STATUS_SUCCESS != moduleStatus)
					{	// Error updating the status byte
						testResult = testFail;
						Log(LOG_ERRORS, "Error writing %s - status: %s\n", iter->second->getName().c_str(),
							ConvertStatusToResponse(moduleStatus).c_str());
					}
				}
				else
				{	// Error getting data
					testResult = testFail;
					Log(LOG_ERRORS, "Error reading %s - status: %s\n", iter->second->getName().c_str(),
						ConvertStatusToResponse(moduleStatus).c_str());
				}
			}
			// Check the overall status
			if((BEP_STATUS_SUCCESS == moduleStatus) && (testResult != testFail))
			{	// Module updates have completed successfully
				testResult = testPass;
			}
			else
			{	// Failures updating the module
				testResult = testFail;
			}
			testResultCode = (testPass == testResult ? "0000" : GetFaultCode("CommunicationFailure"));
			testDescription = (testPass == testResult ? GetTestStepInfo("Description") : GetFaultDescription("CommunicationFailure"));
			Log(LOG_DEV_DATA, "Program EOL Status Bytes: %s\n", testResult.c_str());
		}
		catch(ModuleException &exception)
		{	// Module exception
			Log(LOG_ERRORS, "Module Exception in ProgramEOLStatus - %s\n", exception.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
	}
	else
	{	// Previous pass, skip this step
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing ProgramEOLStatus test\n");
		testResult = testSkip;
	}
	// Report the test results
	SendTestResult(testResult, testDescription, testResultCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::ProgramEOLStatus()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::EvaluateOdometerIntegrity(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = "Test In Progress";
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_FAILURE;
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::EvaluateOdometerIntegrity()\n");
	// Check if the module is previous pass
	if(!IsPreviousPass() && (!ShortCircuitTestStep()))
	{	// Preconditions met, read odometer registers
		vector<UINT16> odometerRegisters;
		try
		{	// Read the odometer registers
			moduleStatus = m_vehicleModule.GetInfo("ReadOdometerRegisters", odometerRegisters);
			// Check the status of the module
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Good data, evaluate
				Log(LOG_DEV_DATA, "Captured good data, evaluate\n");
				testResult = testPass;
				// Check for initial setting
				for(UINT32 ii = 0; (ii < odometerRegisters.size()) && (testPass == testResult); ii++)
				{	// If a register is not correct, fail
					if(odometerRegisters[ii] != (UINT16)atoh(m_SRIInitialSetting.getNode(ii)->getValue().c_str()))
					{	// Odometer register is not set properly
						testResult = testFail;
					}
				}
				// Store EEROM odometer register used if initial setting
				if(!testResult.compare(testPass))
				{
					StoreCurrentEEROMOdometerReading(odometerRegisters[GetParameterInt("InitialSRIOdometerRegisterUsed")]);
				}
				// If not initial, check for first tumble
				if(testFail == testResult)
				{	// Check for first tumble setting
					testResult = testPass;
					for(UINT32 ii = 0; (ii < odometerRegisters.size()) && (testPass == testResult); ii++)
					{	// If a register is not correct, fail
						if(odometerRegisters[ii] != (UINT16)atoh(m_SRIFirstTumble.getNode(ii)->getValue().c_str()))
						{	// Odometer register not correct
							testResult = testFail;
						}
					}
					// Store EEROM odometer register used if initial setting
					if(!testResult.compare(testPass))
					{
						StoreCurrentEEROMOdometerReading(odometerRegisters[GetParameterInt("FirstTumbleSRIOdometerRegisterUsed")]);
					}
				}
				// If not first tumble, check for second tumble
				if(testFail == testResult)
				{	// Check for first tumble setting
					testResult = testPass;
					for(UINT32 ii = 0; (ii < odometerRegisters.size()) && (testPass == testResult); ii++)
					{	// If a register is not correct, fail
						if(odometerRegisters[ii] != (UINT16)atoh(m_SRISecondTumble.getNode(ii)->getValue().c_str()))
						{	// Odometer register not correct
							testResult = testFail;
						}
					}
					// Store EEROM odometer register used if initial setting
					if(!testResult.compare(testPass))
					{
						StoreCurrentEEROMOdometerReading(odometerRegisters[GetParameterInt("SecondTumbleSRIOdometerRegisterUsed")]);
					}
				}
				// Check for round robin
				if(testFail == testResult)
				{
					testResult = testPass;
					for(UINT32 ii = 0; (ii < odometerRegisters.size()-1) && (testPass == testResult); ii++)
					{	// Next register should be one greater
						if((odometerRegisters[ii]+1) == odometerRegisters[ii+1])
							testResult = testPass;
						// Else the next register should be the number of registers -1 less than the current
						else if((odometerRegisters[ii]-(odometerRegisters.size()-1)) == odometerRegisters[ii+1])
							testResult = testPass;
						// Else this is not a round robin sequence
						else
							testResult = testFail;
					}
					// Check the wrap-around to the first odometer register
					if((testFail == testResult) &&
					   (((odometerRegisters[odometerRegisters.size()-1]+1) == odometerRegisters[0]) ||
						((odometerRegisters[odometerRegisters.size()-1]-(odometerRegisters.size()-1)) == odometerRegisters[0])))
						testResult = testPass;
					if(!testResult.compare(testPass))
					{
						StoreCurrentEEROMOdometerReading(*(max_element(odometerRegisters.begin(), odometerRegisters.end())));
					}
				}
				// Log the register values
				string logMessage;
				char temp[256];
				for(UINT32 ii = 0; ii < odometerRegisters.size(); ii++)
					logMessage += CreateMessage(temp, sizeof(temp), "\tOdometer Register %d: 0x%04X\n", ii, odometerRegisters[ii]);
				Log(LOG_DEV_DATA, "SRI Mileage register validation: %s\n%s", testResult.c_str(), logMessage.c_str());
				Log(LOG_DEV_DATA, "Current EEROM odometer register value: %02X", GetCurrentOdometerEEROMReading());
				// Determine the test results
				testResultCode = (testResult == testPass ? "0000" : GetFaultCode("SRIRegistersInvalid"));
				testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("SRIRegistersInvalid"));
			}
			else
			{	// Communication failure getting data
				testResult = testFail;
				testResultCode = GetFaultCode("CommunicationFailure");
				testDescription = GetFaultDescription("CommunicationFailure");
			}
		}
		catch(ModuleException &exception)
		{
			Log(LOG_ERRORS, "Module Exception in EvaluateOdometerIntegrity - %s\n", exception.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		char temp[16];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "OdometerRegister1", CreateMessage(temp, sizeof(temp), "%04X", odometerRegisters[0]), "",
								 "OdometerRegister2", CreateMessage(temp, sizeof(temp), "%04X", odometerRegisters[1]), "",
								 "OdometerRegister3", CreateMessage(temp, sizeof(temp), "%04X", odometerRegisters[2]), "",
								 "OdometerRegister4", CreateMessage(temp, sizeof(temp), "%04X", odometerRegisters[3]), "");
	}
	else
	{	// Previous pass, skip this step
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing EvaluateOdometerIntegrity test\n");
		testResult = testSkip;
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::EvaluateOdometerIntegrity()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::MonitorMAPSensor(void)
{
	string testResult = BEP_TESTING_STATUS, moduleUpdateResult = BEP_TESTING_STATUS;
	string testDescription = "Test in progress", moduleUpdateDescription = "Test in progress";
	string testResultCode = "0000", moduleUpdateCode = "0000";
	float mapSensorReading = 0.0;
	bool preconditionsMet = false;
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if the vehicle is previous pass and if the engine is at idle
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::MonitorMAPSensor()\n");
	if(!IsPreviousPass() && !ShortCircuitTestStep())
	{	// Moved the IsIdleRPM() check to its own separte if-else statement because if
		//   it failed, the teststep was just skipping and on a re-test it would not perform
		//   the test.   Greg Pattison 08/19/2004
		if(!IsThrottleAtoDClosed())
		{	//Foot must be off the Throttle.
			testResult = testFail;
			Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::MonitorMAPSensor() - Failed because Throttle not in range\n");
			testDescription = GetFaultDescription("MapSensorThrotlleNotCorrect");
			testResultCode = GetFaultCode("MapSensorThrotlleNotCorrect");
			preconditionsMet = false;
		}
		else if(!IsIdleRPM())
		{	//RPM must be with in Idle RPM range
			testResult = testFail;
			Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::MonitorMAPSensor() - Failed because Idle RPM not in range\n");
			testDescription = GetFaultDescription("MapSensorIdleRPMNotCorrect");
			testResultCode = GetFaultCode("MapSensorIdleRPMNotCorrect");
			preconditionsMet = false;
		}
		else
		{	// All preconditions met
			preconditionsMet = true;
			try
			{	// Read the map sensor
				moduleStatus = m_vehicleModule.GetInfo("ReadMAPSensor", mapSensorReading);
				// Check the status of the read
				if(BEP_STATUS_SUCCESS == moduleStatus)
				{	// Good data from the module, verify against parameters
					if(m_analyze.CompareData(mapSensorReading, GetParameterFloat("MinimumMAPReading"), GREATER_EQUAL))
					{	// MAP sensor reading acceptable
						testResult = testPass;
					}
					else
					{	// Map Sensor reading out of range
						testResult = testFail;
					}
					// Update the test result code and description
					testResultCode = (testResult == testPass ? "0000" : GetFaultCode("MAPSensorTooLow"));
					testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("MAPSensorTooLow"));
					// Log the test result data
					Log(LOG_DEV_DATA, "Map Sensor reading %s - MAP: %5.2f, Minimum: %5.2f\n",
						(testResult == testPass ? "acceptable" : "too low"), mapSensorReading,
						GetParameterFloat("MinimumMAPReading"));
					// Update the result in the module
					UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLMAPSensorResultRegister")).c_str());
					UINT16 idleRPMTestBit = GetParameterInt("MapSensorResultBit");
					moduleUpdateResult = UpdateEOLStatusRegister(eolStatusRegister, idleRPMTestBit, testResult);
					// Check the status of the update
					moduleUpdateCode = moduleUpdateResult == testPass ? "0000" : GetFaultCode("ModuleUpdateCommunicationFailure");
					moduleUpdateDescription = 
					moduleUpdateResult == testPass ? "Update MAP results" : GetFaultDescription("ModuleUpdateCommunicationFailure");
					Log(LOG_DEV_DATA, "Module update: %s\n", moduleUpdateResult.c_str());
				}
				else
				{	// Error reading data from the module
					Log(LOG_ERRORS, "Error reading MAP sensor from the module - status: %s\n",
						ConvertStatusToResponse(moduleStatus).c_str());
					testResult = testFail;
					testResultCode = GetFaultCode("CommunicationFailure");
					testDescription = GetFaultDescription("CommunicationFailure");
				}
			}
			catch(ModuleException &exception)
			{
				Log(LOG_ERRORS, "Module Exception in MonitorMAPSensor - %s\n", exception.message().c_str());
				testResult = testSoftwareFail;
				testResultCode = GetFaultCode("SoftwareFailure");
				testDescription = GetFaultDescription("SoftwareFailure");
			}
		}
		// Report the results
		char temp[16];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "MAPSensor", CreateMessage(temp, sizeof(temp), "%5.2f", mapSensorReading), "kPa",
								 "MinimumMAPReading", GetParameter("MinimumMAPReading"), "kPa",
								 "PreconditionsMet", preconditionsMet ? "True" : "False");
		SendSubtestResult("MAPSensorModuleUpdate", moduleUpdateResult, moduleUpdateDescription, moduleUpdateCode);
		// Determine the overall result
		if((testResult == testPass) && (moduleUpdateResult != testPass)) testResult = moduleUpdateResult;
	}
	else
	{	// Vehicle is previous pass, skip this test
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing MonitorMAPSensor test\n");
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::MonitorMAPSensor(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StartHydraulicCoolingFan(void)
{
	string testResult = testFail;
	string testDescription(BEP_NO_DATA);
	string testResultCode("0000");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	// Log the function entry
	Log(LOG_DEV_DATA, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());

	if(((!IsPreviousPass() && 
		 !ShortCircuitTestStep()) || 
		GetParameterBool("AlwaysRunCoolingFan")) && 
	   NeedToRunHydraulicCoolingFan())
	{	// Ok to run this step
		try
		{	// Ensure the keep alive is going (for retest scenario)
			if(ShortCircuitTestStep())	m_vehicleModule.StartModuleKeepAlive();
			if(GetParameterBool("CheckGatewayForFanType"))
			{
				string vehicleFanType;
				m_gatewayModule->EnterDiagnosticMode();
				m_gatewayModule->ReadModuleData(GetDataTag("RadiatorFanType"),vehicleFanType);
				moduleStatus = m_gatewayModule->CommandModule("Start" + vehicleFanType);
				m_gatewayModule->StartModuleKeepAlive();
			}
			else moduleStatus = m_vehicleModule.StartRadiatorFan("HydraulicCooling");
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	//Cooling fan started
				testResult = testPass;
				testResultCode = "0000";
				testDescription = GetTestStepInfo("Description");
			}
			else
			{	//failed to start cooling fan.
				Log(LOG_ERRORS, "Error starting Hydraulic Cooling fan - status: %s\n",
					ConvertStatusToResponse(moduleStatus).c_str());
				if(GetParameterBool("NoAbortForHydraulicCoolingFan"))
				{	// Keep testing if StartHydraulicCoolingFan fails
					testResult = testFail;
				}
				else
				{	// StartHydraulicCoolingFan failed, abort now!
					SystemWrite(ABORT_DATA_TAG, "1");
					Log(LOG_DEV_DATA, "ABORT - StartHydraulicCoolingFan failed!");
					testResult = testAbort;
				}
				testResultCode = GetFaultCode("StartHydraulicCoolingFanFailed");
				testDescription = GetFaultDescription("StartHydraulicCoolingFanFailed");
			}
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing %s test\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s: EXIT\n", GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StopHydraulicCoolingFan(void)
{
	string testResult = testFail;
	string testDescription(BEP_NO_DATA);
	string testResultCode("0000");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	// Log the function entry
	Log(LOG_DEV_DATA, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
	if((!ShortCircuitTestStep() || GetParameterBool("AlwaysRunCoolingFan")) && 
	   NeedToRunHydraulicCoolingFan())
	{	// Ok to run this step
		try
		{	// Ensure module keep alive is stopped (restest scenario)
			//if(ShortCircuitTestStep())  m_vehicleModule.StopModuleKeepAlive(); // keep alive should be stopped by Finishup Objective JW 2011-03-31
			if(GetParameterBool("CheckGatewayForFanType"))
			{
				string vehicleFanType;
				m_gatewayModule->ReadModuleData(GetDataTag("RadiatorFanType"),vehicleFanType);
				moduleStatus = m_gatewayModule->CommandModule("Stop" + vehicleFanType);
				m_gatewayModule->StopModuleKeepAlive();
			}
			else moduleStatus = m_vehicleModule.StopRadiatorFan("HydraulicCooling");
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	//Cooling fan stopped
				testResult = testPass;
				testResultCode = "0000";
				testDescription = GetTestStepInfo("Description");
			}
			else
			{	//failed to stop cooling fan.
				Log(LOG_ERRORS, "Error stopping Hydraulic Cooling fan - status: %s\n",
					ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("StopHydraulicCoolingFanFailed");
				testDescription = GetFaultDescription("StopHydraulicCoolingFanFailed");
			}
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing %s test\n", GetTestStepName().c_str());
		testResult = testSkip;
	}
	// Log the exit and return the result
	Log(LOG_FN_ENTRY, "%s: EXIT\n", GetTestStepName().c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StartSwitchMonitor(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if we need to skip this step
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::StartSwitchMonitor()\n");
	if(!ShortCircuitTestStep() && !IsPreviousPass())
	{	// Need to perform this test step
		try
		{	//Test parameter to determine if Redundant Brake Switch test is required.
			//Read value from the TC parameter <SetRedundantBrakeSwitchEquipped>. Defaulted to false.
			bool testRedundantBrakeSwitch = IsRedundantBrakeSwitchEquipped();
			//Enable background test monitoring
			//The GenericEmissionsModuleTemplate defaults to false - no test.
			m_vehicleModule.SetRedundantBrakeSwitchEquipped(testRedundantBrakeSwitch);
			//Test parameter determines if Clutch Upstop Switch test is required.
			//Read value from the TC parameter <SetClutchUpstopSwitchEquipped>. Defaulted to false.
			bool testClutchUpstopSwitch = IsClutchUpstopSwitchEquipped();
			//Enable background test monitoring
			//The GenericEmissionsModuleTemplate defaults to false - no test.
			m_vehicleModule.SetClutchUpstopSwitchEquipped(testClutchUpstopSwitch);
			// Try to start the switch monitor thread
			moduleStatus = m_vehicleModule.StartSwitchMonitor();
			// Check the status of the operation
			if(BEP_STATUS_SUCCESS == moduleStatus)
			{	// Switch monitor started, test passes
				testResult = testPass;
			}
			else
			{	// switch monitor not started, test fails
				testResult = testFail;
				Log(LOG_ERRORS, "Error starting switch monitor - status: %s\n",
					ConvertStatusToResponse(moduleStatus).c_str());
			}
			// Set the rest of the data
			Log(LOG_DEV_DATA, "Start Switch Monitor: %s\n", testResult.c_str());
			testResultCode = (testResult == testPass ? "0000" : GetFaultCode("StartSwitchMonitorFail"));
			testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("StartSwitchMonitorFail"));
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::StartSwitchMonitor()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StopSwitchMonitor(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if this step should be skipped
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::StopSwitchMonitor()\n");
	if(!ShortCircuitTestStep() && !IsPreviousPass())
	{	// Need to perform this test step
		string parkNeutralSwitchResult = testSkip;
		string clutchSwitchResult = testSkip;
		string brakeSwitchResult = testSkip;
		string redundantBrakeSwitchResult = testSkip;
		string clutchUpstopSwitchResult = testSkip;
		try
		{
			// Determine wether the Redundant brake switch background test
			//    was started (module set to equipped)
			bool testRedundantBrakeSwitch =  m_vehicleModule.IsRedundantBrakeSwitchEquipped();
			// Determine wether the clutch Upstop switch background test
			//   was started (module set to equipped)
			bool testClutchUpstopSwitch =  m_vehicleModule.IsClutchUpstopSwitchEquipped();
			// Get the status of the brake switch
			brakeSwitchResult = CheckBrakeSwitchResults();
			// Determine if Redundant Brake Switch monitoring was to be performed
			//    Warning: Ensure that the parameter in the Module Configuration is
			//    also set to perform the module read for this test.
			if(testRedundantBrakeSwitch)
			{	// Check redundant brake switch results
				redundantBrakeSwitchResult = CheckRedundantBrakeSwitchResults();
			}
			else
			{	// Configuration set to NOT test redundant brake switch
				Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::StopSwitchMonitor(), Redundant Brake Switch test turned off in configuration\n");
			}
			// Determine if Park/Neutral switch or Clutch switch needs to be checked
			if(!IsManualTransmission())
			{	// Determine if the park/neutral switch should be checked
				if(!GetParameterBool("SkipParkNeutralSwitch"))
				{	// Get the status of the park/neutral switch test
					parkNeutralSwitchResult = CheckParkNeutralSwitchResults();
				}
				else
				{	// Skipping p/n switch result
					Log(LOG_DEV_DATA, "Park/Neutral switch results skipped by parameter.");
					parkNeutralSwitchResult = testSkip;
				}
				Log(LOG_DEV_DATA, "Automatic transmission, not checking clutch switch results\n");
			}
			else
			{	// Get the status of the clutch switch test
				clutchSwitchResult = CheckClutchSwitchResults();
				// Determine if Clutch Upstop Switch monitoring was to be performed
				if(testClutchUpstopSwitch)
				{	// Check Clucth Upstop switch results
					clutchUpstopSwitchResult = CheckClutchUpstopSwitchResults();
				}
				else
				{	// Configuration set to NOT test clutch Upstop switch
					Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::StopSwitchMonitor(), Clutch Upstop Switch test turned off in configuration\n");
				}
				// Log not doing Park Neutral test
				Log(LOG_DEV_DATA, "Manual transmission, not checking Park/Neutral switch results\n");
			}
			Log(LOG_DEV_DATA, "Switch Test Results -- \n\tBrake: %s\n\tRedundantBrakeSwitch: %s\n\t"
				"Park/Neutral: %s\n\tClutch: %s\n",
				brakeSwitchResult.c_str(), redundantBrakeSwitchResult.c_str(), parkNeutralSwitchResult.c_str(), clutchSwitchResult.c_str());
			// Try to stop the switch monitor thread
			moduleStatus = m_vehicleModule.StopSwitchMonitor();
			Log(LOG_ERRORS, "Stopped switch monitor - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
			// Check the status of the module operation
			if(BEP_STATUS_SUCCESS != moduleStatus)
			{	// Failed to stop the module switch monitor
				Log(LOG_ERRORS, "GenericEmissionsTCTemplate::StopSwitchMonitor() - Error Stopping Module switch monitor, status: %s\n",
					ConvertStatusToResponse(moduleStatus).c_str());
				testResult = testFail;
				testResultCode = GetFaultCode("StopSwitchMonitor");
				testDescription = GetFaultDescription("StopSwitchMonitor");
			}
			else if(((testSkip != parkNeutralSwitchResult) && (testPass != parkNeutralSwitchResult)) ||
					((testSkip != clutchSwitchResult) && (testPass != clutchSwitchResult)) ||
					((testSkip != brakeSwitchResult) && (testPass != brakeSwitchResult)) ||
					((testSkip != redundantBrakeSwitchResult) && (testPass != redundantBrakeSwitchResult)) ||
					((testSkip != clutchUpstopSwitchResult) && (testPass != clutchUpstopSwitchResult)))
			{
				Log(LOG_ERRORS, "GenericEmissionsTCTemplate::StopSwitchMonitor() - StopSwitchMonitor failed for individual switch test failure\n");
				testResult = testFail;
				testResultCode = GetFaultCode("SwitchMonitorSwitchTestFailure");
				testDescription = GetFaultDescription("SwitchMonitorSwitchTestFailure");
			}
			else
			{	// Switch Monitors all passed
				testResult = testPass;
			}
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::StopSwitchMonitor()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StartTPSMonitor(void)
{
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	// Check if we need to skip this step
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::StartTPSMonitor()\n");
	if(!ShortCircuitTestStep() && !IsPreviousPass() && !m_etcVehicle)
	{	// Need to perform this test step
		try
		{	// Try to start the TPS monitor thread
			if(!IsThrottleAtoDClosed())
			{	//Foot must be off the Throttle.
				testResult = testFail;
				Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::StartTPSMonitor() - Failed because Throttle not in range\n");
				testDescription = GetFaultDescription("InitialTpsThrotlleNotCorrect");
				testResultCode = GetFaultCode("InitialTpsThrotlleNotCorrect");
			}
			else
			{	//Initial conditions correct
				string messageTag = GetDataTag("ReadThrottlePositionNonETC");
				moduleStatus = m_vehicleModule.StartTPSMonitor(messageTag);
				// Check the status of the operation
				testResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
				Log(LOG_ERRORS, "Started TPS monitor - status: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
				// Set the rest of the data
				Log(LOG_DEV_DATA, "Start TPS Monitor: %s\n", testResult.c_str());
				testResultCode = (testResult == testPass ? "0000" : GetFaultCode("StartTPSMonitorFail"));
				testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("StartTPSMonitorFail"));
			}
		}
		catch(ModuleException &moduleException)
		{
			Log(LOG_ERRORS, "Module Exception in %s::%s - %s\n",
				GetComponentName().c_str(), GetTestStepName().c_str(), moduleException.message().c_str());
			testResult = testSoftwareFail;
			testResultCode = GetFaultCode("SoftwareFailure");
			testDescription = GetFaultDescription("SoftwareFailure");
		}
		// Send the test result
		SendTestResult(testResult, testDescription, testResultCode);
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::StartTPSMonitor()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::StopTPSMonitor(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_ERROR;
	string testResult = BEP_TESTING_STATUS;
	string testResultCode = "0000";
	string testDescription = GetTestStepInfo("Description");
	// Check if this step should be skipped
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::StopTPSMonitor()\n");
	if(!ShortCircuitTestStep() && !IsPreviousPass() && !m_etcVehicle)
	{	// Need to perform this test step
		SetStartTime();
		// Keep looking for delta until observed or timeout
		bool requiredTPSObserved = false;
		bool promptDisplayed = false;
		string promptTag = m_etcVehicle ? "AccelerateToTargetSpeed" : "PressAcceleratorPedal";
		do
		{	// Get the observed delta
			requiredTPSObserved = m_analyze.CompareData(m_vehicleModule.GetObservedDeltaTPS(),
														GetParameterFloat("RequiredDeltaTPS"), GREATER_EQUAL);
			// Determine if the operator should be prompted
			if(!requiredTPSObserved && !promptDisplayed)
			{	// Determine how to force a TPS change
				if(m_etcVehicle)
				{
					SystemWrite(GetDataTag("SpeedTarget"), GetParameter("DeltaTPSSpeedRange"));
				}
				// Display the prompt
				DisplayPrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
				promptDisplayed = true;
			}
			else if(requiredTPSObserved && promptDisplayed)
			{
				RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
				promptDisplayed = false;
			}
			// If required delta not observed, wait before checking again
			if(!requiredTPSObserved)  BposSleep(GetTestStepInfoInt("ScanDelay"));
			// Check if we still need to wait
		} while(TimeRemaining() && !requiredTPSObserved);
		// Check if the prompt should be removed
		if(promptDisplayed)
		{	// Remove the prompt from the display
			RemovePrompt(GetPromptBox(promptTag), GetPrompt(promptTag), GetPromptPriority(promptTag));
			promptDisplayed = false;
		}
		// Wait for zero speed
		CheckZeroSpeed();
		// Set the results
		testResult = requiredTPSObserved ? testPass : testFail;
		testResultCode = (testPass == testResult ? "0000" : GetFaultCode("DeltaTPSFail"));
		testDescription = (testPass == testResult ? "Monitor Delta TPS" : GetFaultDescription("DeltaTPSFail"));
		// Log the results
		Log(LOG_DEV_DATA, "Delta TPS Test: %s - Initial: %5.2f\n\tFinal: %5.2f\n\tDelta: %5.2f\n",
			testResult.c_str(), m_vehicleModule.GetInitialTPSReading(), m_vehicleModule.GetFinalTPSReading(),
			m_vehicleModule.GetObservedDeltaTPS());
		// Report the test results
		char temp[16];
		SendSubtestResultWithDetail("DeltaTPSTest", testResult, testDescription, testResultCode,
									"InitialTPSReading", CreateMessage(temp, sizeof(temp), "%5.2f",m_vehicleModule.GetInitialTPSReading()), "Volts",
									"FinalTPSReading", CreateMessage(temp, sizeof(temp), "%5.2f",m_vehicleModule.GetFinalTPSReading()), "Volts",
									"DeltaTPS", CreateMessage(temp, sizeof(temp), "%5.2f",m_vehicleModule.GetObservedDeltaTPS()), "Volts");
		// Update the result in the module
		UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLTPSResultRegister")).c_str());
		UINT16 deltaTPSTestBit = GetParameterInt("ThrottlePositionSensorResultBit");
		string updateResult = UpdateEOLStatusRegister(eolStatusRegister, deltaTPSTestBit, testResult);
		// Check the status of the update
		if(testPass != updateResult)
		{	// There was a communication failure
			testResult = updateResult;
			testResultCode = GetFaultCode("ModuleUpdateCommunicationFailure");
			testDescription = GetFaultDescription("ModuleUpdateCommunicationFailure");
			Log(LOG_ERRORS, "Communication failure updating TPS test result in module\n");
		}
		else
		{	// Update the description for the test report
			testDescription = "Update Delta TPS module results";
		}
		// Report the module update result
		SendSubtestResult("DeltaTPSModuleUpdate", testResult, testDescription, testResultCode);
		// Stop the TPS monitor
		moduleStatus = m_vehicleModule.StopTPSMonitor();
		// Check the result of stopping the monitor
		Log(LOG_DEV_DATA, "Stopped TPS monitor: %s\n", ConvertStatusToResponse(moduleStatus).c_str());
		string stopResult = BEP_STATUS_SUCCESS == moduleStatus ? testPass : testFail;
		testResultCode = testResult == testPass ? "0000" : GetFaultCode("StopTPSMonitor");
		testDescription = testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("StopTPSMonitor");
		// Report the overall test result
		SendTestResult(stopResult, testDescription, testResultCode);
		// Update the overall result
		if(testPass != stopResult)
		{
			testResult = stopResult;
		}
	}
	else
	{	// Need to skip this test step
		testResult = testSkip;
		Log(LOG_DEV_DATA, "Skipping test step %s\n", GetTestStepName().c_str());
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::StopTPSMonitor()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckIdleSpeed(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testDescription = "Test in progress", moduleDescription = "Test in progress";
	string testResultCode = "0000", moduleCode = "0000";
	INT32 engineIdleRPM = 0;
	bool preconditionsMet = false;
	// Check if vehicle is previous pass
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckIdleSpeed()\n");
	if(!IsPreviousPass() && !ShortCircuitTestStep())
	{	// All precondition satisfied, read the engine Idle RPM
		if(!IsThrottleClosed())
		{	//The throttle needs to be at the closed "foot off" position.
			testResult = testFail;
			Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::CheckIdleSpeed() - Failed because Throttle not in range\n");
			testDescription = GetFaultDescription("IdleSpeedThrotlleNotCorrect");
			testResultCode = GetFaultCode("IdleSpeedThrotlleNotCorrect");
			preconditionsMet = false;
		}
		else
		{// determine if neutral check is required
			if(GetParameterBool("CheckIdleSpeedNRequired") && !IsManualTransmission())
			{//prompt and verify vehicle is in neutral
				preconditionsMet = IsTransmissionInNeutral();
			}
			else preconditionsMet = true;
			if(GetParameterBool("IdleRPMCheckEngineRunning") && preconditionsMet)
			{
				preconditionsMet = CheckEngineRunning();
			}
			if(preconditionsMet)
			{
				try
				{	// Read the engine Idle RPM
					BEP_STATUS_TYPE moduleStatus = m_vehicleModule.GetInfo("ReadEngineIdleRPM", engineIdleRPM);
					// Check the status of the read
					if(BEP_STATUS_SUCCESS == moduleStatus)
					{	// Good data from the module, verify against parameters
						if(m_analyze.CompareData(engineIdleRPM, GetParameterInt("MinimumIdleRPM"), GREATER_EQUAL) &&
						   m_analyze.CompareData(engineIdleRPM, GetParameterInt("MaximumIdleRPM"), LESS_EQUAL))
						{	// Idle RPM in range
							Log(LOG_DEV_DATA, "Engine Idle RPM Pass - %d [%d %d]\n", engineIdleRPM,
								GetParameterInt("MinimumIdleRPM"), GetParameterInt("MaximumIdleRPM"));
							testResult = testPass;
						}
						else
						{	// Idle rpm out of range
							Log(LOG_ERRORS, "Engine Idle RPM out of range - %d [%d %d]\n", engineIdleRPM,
								GetParameterInt("MinimumIdleRPM"), GetParameterInt("MaximumIdleRPM"));
							testResult = testFail;
						}
						// Set the result code and description
						testResultCode = (testResult == testPass ? "0000" : GetFaultCode("EngineIdleRPMOutOfRange"));
						testDescription = (testResult == testPass ? GetTestStepInfo("Description") : GetFaultDescription("EngineIdleRPMOutOfRange"));
						// Update the result in the module
						UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLIdleRPMResultRegister")).c_str());
						UINT16 idleRPMTestBit = GetParameterInt("IdleRPMResultBit");
						moduleResult = UpdateEOLStatusRegister(eolStatusRegister, idleRPMTestBit, testResult);
						// Check the status of the update
						moduleCode = moduleResult == testPass ? "0000" : GetFaultCode("ModuleUpdateCommunicationFailure");
						moduleDescription = 
						moduleResult == testPass ? "Update Module Idle Speed Result" : GetFaultDescription("ModuleUpdateCommunicationFailure");
						Log(LOG_DEV_DATA, "Module Update: %s\n", moduleResult.c_str());
					}
					else
					{	// Error getting data from the module
						Log(LOG_ERRORS, "Error getting idle RPM from the module - status: %s\n",
							ConvertStatusToResponse(moduleStatus).c_str());
						testResult = testFail;
						testResultCode = GetFaultCode("CommunicationFailure");
						testDescription = GetFaultDescription("CommunicationFailure");
					}
				}
				catch(ModuleException &moduleException)
				{
					Log(LOG_ERRORS, "Module Exception in CheckIdleSpeed - %s\n", moduleException.message().c_str());
					testResult = testSoftwareFail;
					testResultCode = GetFaultCode("SoftwareFailure");
					testDescription = GetFaultDescription("SoftwareFailure");
				}
			}
			else
			{//preconditions not met
				Log(LOG_DEV_DATA, "GenericEmissionsTCTemplate::CheckIdleSpeed() - Failed because Transmission not in neutral\n");
				testDescription = GetFaultDescription("IdleSpeedGearNotCorrect");
				testResultCode = GetFaultCode("IdleSpeedGearNotCorrect");
			}
		}
		// Report the results
		char temp[16];
		SendTestResultWithDetail(testResult, testDescription, testResultCode,
								 "EngineRPM", CreateMessage(temp, sizeof(temp), "%04d", engineIdleRPM), "RPM",
								 "MinimumIdleRPM", GetParameter("MinimumIdleRPM"), "RPM",
								 "MaximumIdleRPM", GetParameter("MaximumIdleRPM"), "RPM",
								 "PreconditionsMet", (preconditionsMet ? "True" : "False"), "bool");
		SendSubtestResult("IdleSpeedModuleUpdate", moduleResult, moduleDescription, moduleCode);
		// Determine the overall result
		if((testResult == testPass) && (moduleResult != testPass))	 testResult = moduleResult;
	}
	else
	{	// Vehicle is previous pass, do not test
		Log(LOG_DEV_DATA, "Emissions Previous Pass, not performing CheckIdleSpeed test\n");
		testResult = testSkip;
	}
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckIdleSpeed(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckBrakeSwitchResults(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update module brake switch result";
	string switchOnResult = BEP_TESTING_STATUS;
	string switchOffResult = BEP_TESTING_STATUS;
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckBrakeSwitchResults()\n");
	try
	{	// Attempt to get the switch states
		switchOnResult = CheckBrakeSwitchOnResult();
		switchOffResult = CheckBrakeSwitchOffResult();
		// Determine the test result
		testResult = (((switchOnResult == testPass) && (switchOffResult == testPass)) ? testPass : testFail);
		Log(LOG_DEV_DATA, "Brake switch result: %s\n\tOn: %s\n\tOff: %s\n",
			testResult.c_str(), switchOnResult.c_str(), switchOffResult.c_str());
		// Check if a state was not detected
		if(testPass != testResult)
		{	// Determine which state was not detected
			if((testPass != switchOnResult) && (testPass == switchOffResult))
			{	// Failed to see ON state
				testResultCode = GetFaultCode("BrakeSwitchOnFail");
				testDescription = GetFaultDescription("BrakeSwitchOnFail");
			}
			else if((testPass == switchOnResult) && (testPass != switchOffResult))
			{	// Failed to see OFF state
				testResultCode = GetFaultCode("BrakeSwitchOffFail");
				testDescription = GetFaultDescription("BrakeSwitchOffFail");
			}
			else
			{	// Failed to see both states
				testResultCode = GetFaultCode("BrakeSwitchFail");
				testDescription = GetFaultDescription("BrakeSwitchFail");
			}
		}
		else
		{	// All good
			testResultCode = "0000";
			testDescription = "Monitor for brake switch On and Off";
		}
		// Update the result in the module
		UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLBrakeSwitchResultRegister")).c_str());
		UINT16 brakeSwitchTestBit = GetParameterInt("BrakeSwitchResultBit");
		moduleResult = UpdateEOLStatusRegister(eolStatusRegister, brakeSwitchTestBit, testResult);
		// Check the status of the update
		moduleCode = moduleResult == testPass ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
		moduleDescription = moduleResult == testPass ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
		Log(LOG_DEV_DATA, "Module Update: %s\n", moduleResult.c_str());
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in CheckBrakeSwitchResults - %s\n", moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the results
	SendSubtestResultWithDetail("MonitorBrakeSwitch", testResult, testDescription, testResultCode,
								"BrakeSwitchOn", switchOnResult, "", "BrakeSwitchOff", switchOffResult, "");
	SendSubtestResult("BrakeSwitchModuleUpdate", moduleResult, moduleDescription, moduleCode);
	// Determine the overall result
	if((testResult == testPass) && (moduleResult != testPass))	testResult = moduleResult;
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckBrakeSwitchResults(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckParkNeutralSwitchResults(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update module P/N result";
	string switchOnResult = BEP_TESTING_STATUS;
	string switchOffResult = BEP_TESTING_STATUS;
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckParkNeutralSwitchResults()\n");
	try
	{	// Attempt to get the switch states
		switchOnResult = CheckParkNeutralSwitchOnResult();
		switchOffResult = CheckParkNeutralSwitchOffResult();
		// Determine the test result
		testResult = (((switchOnResult == testPass) && (switchOffResult == testPass)) ? testPass : testFail);
		Log(LOG_DEV_DATA, "Park/Neutral switch result: %s\n\tOn: %s\n\tOff: %s\n",
			testResult.c_str(), switchOnResult.c_str(), switchOffResult.c_str());
		// Check if a state was not detected
		if(testPass != testResult)
		{	// Determine which state was not detected
			if((testPass != switchOnResult) && (testPass == switchOffResult))
			{	// Failed to see ON state
				testResultCode = GetFaultCode("ParkNeutralSwitchOnFail");
				testDescription = GetFaultDescription("ParkNeutralSwitchOnFail");
			}
			else if((testPass == switchOnResult) && (testPass != switchOffResult))
			{	// Failed to see OFF state
				testResultCode = GetFaultCode("ParkNeutralSwitchOffFail");
				testDescription = GetFaultDescription("ParkNeutralSwitchOffFail");
			}
			else
			{	// Failed to see both states
				testResultCode = GetFaultCode("ParkNeutralSwitchFail");
				testDescription = GetFaultDescription("ParkNeutralSwitchFail");
			}
		}
		else
		{	// All good
			testResultCode = "0000";
			testDescription = "Monitor for Park/Neutral switch On and Off";
		}
		// Update the result in the module
		UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLParkNeutralSwitchResultRegister")).c_str());
		UINT16 parkNeutralSwitchTestBit = GetParameterInt("ParkNeutralSwitchResultBit");
		moduleResult = UpdateEOLStatusRegister(eolStatusRegister, parkNeutralSwitchTestBit, testResult);
		// Check the status of the update
		moduleCode = moduleResult == testPass ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
		moduleDescription = moduleResult == testPass ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
		Log(LOG_DEV_DATA, "Module update: %s\n", moduleResult.c_str());
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in CheckParkNeutralSwitchResults - %s\n", moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the results
	SendSubtestResultWithDetail("MonitorParkNeutralSwitch", testResult, testDescription, testResultCode,
								"ParkNeutralSwitchOn", switchOnResult, "",
								"ParkNeutralSwitchOff", switchOffResult, "");
	SendSubtestResult("ParkNeutralModuleUpdate", moduleResult, moduleDescription, moduleCode);
	// Determine the overall result
	if((testResult == testPass) && (moduleResult != testPass))	 testResult = moduleResult;
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckParkNeutralSwitchResults(%s)\n", moduleResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchSwitchResults(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update module clutch switch result";
	string switchOnResult = BEP_TESTING_STATUS;
	string switchOffResult = BEP_TESTING_STATUS;
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchSwitchResults()\n");
	try
	{	// Attempt to get the switch states
		switchOnResult = CheckClutchSwitchOnResult();
		switchOffResult = CheckClutchSwitchOffResult();
		// Determine the test result
		testResult = (((switchOnResult == testPass) && (switchOffResult == testPass)) ? testPass : testFail);
		Log(LOG_DEV_DATA, "Clutch switch result: %s\n\tOn: %s\n\tOff: %s\n",
			testResult.c_str(), switchOnResult.c_str(), switchOffResult.c_str());
		// Check if a state was not detected
		if(testPass != testResult)
		{	// Determine which state was not detected
			if((testPass != switchOnResult) && (testPass == switchOffResult))
			{	// Failed to see ON state
				testResultCode = GetFaultCode("ClutchSwitchOnFail");
				testDescription = GetFaultDescription("ClutchSwitchOnFail");
			}
			else if((testPass == switchOnResult) && (testPass != switchOffResult))
			{	// Failed to see OFF state
				testResultCode = GetFaultCode("ClutchSwitchOffFail");
				testDescription = GetFaultDescription("ClutchSwitchOffFail");
			}
			else
			{	// Failed to see both states
				testResultCode = GetFaultCode("ClutchSwitchFail");
				testDescription = GetFaultDescription("ClutchSwitchFail");
			}
		}
		else
		{	// All good
			testResultCode = "0000";
			testDescription = "Monitor for clutch switch On and Off";
		}
		// Update the result in the module
		UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLClutchSwitchResultRegister")).c_str());
		UINT16 clutchSwitchTestBit = GetParameterInt("ClutchSwitchResultBit");
		moduleResult = UpdateEOLStatusRegister(eolStatusRegister, clutchSwitchTestBit, testResult);
		// Check the status of the update
		moduleCode = moduleResult == testPass ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
		moduleDescription = moduleResult == testPass ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
		Log(LOG_DEV_DATA, "Module update: %s\n", moduleResult.c_str());
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in CheckClutchSwitchResults - %s\n", moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the results
	SendSubtestResultWithDetail("MonitorClutchSwitch", testResult, testDescription, testResultCode,
								"ClutchSwitchOn", switchOnResult, "", "ClutchSwitchOff", switchOffResult, "");
	SendSubtestResult("ClutchSwithcModuleUpdate", moduleResult, moduleDescription, moduleCode);
	// Determine the overall result
	if((testResult == testPass) && (moduleResult != testPass))	 testResult = moduleResult;
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchSwitchResults()\n");
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckBrakeSwitchOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOn = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckBrakeSwitchOnResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch on status
		switchOn = m_vehicleModule.DetectedBrakeSwitchOn();
		// Determine if the operator should be prompted
		if(!switchOn && !promptDisplayed)
		{	// Instruct operator to press the brake pedal
			DisplayPrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
						  GetPromptPriority("PressBrakePedal"));
			promptDisplayed = true;
		}
		else if(switchOn && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
						 GetPromptPriority("PressBrakePedal"));
			promptDisplayed = false;
		}
		// If the switch is not on, delay before checking again
		if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not on
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
					 GetPromptPriority("PressBrakePedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOn ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Brake Switch On: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckBrakeSwitchOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckBrakeSwitchOffResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckBrakeSwitchOffResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch on status
		switchOff = m_vehicleModule.DetectedBrakeSwitchOff();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to release the brake pedal
			DisplayPrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
						  GetPromptPriority("ReleaseBrakePedal"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
						 GetPromptPriority("ReleaseBrakePedal"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
					 GetPromptPriority("ReleaseBrakePedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Brake Switch Off: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckBrakeSwitchOffResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckParkNeutralSwitchOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOn = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckParkNeutralSwitchOnResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch on status
		switchOn = m_vehicleModule.DetectedParkNeutralSwitchOn();
		// Determine if the operator should be prompted
		if(!switchOn && !promptDisplayed)
		{	// Instruct operator to shift to Park
			DisplayPrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
			promptDisplayed = true;
		}
		else if(switchOn && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
			promptDisplayed = false;
		}
		// If the switch is not on, delay before checking again
		if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not on
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ShiftToPark"), GetPrompt("ShiftToPark"), GetPromptPriority("ShiftToPark"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOn ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Park/Neutral Switch On: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckParkNeutralSwitchOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckParkNeutralSwitchOffResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckParkNeutralSwitchOffResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch on status
		switchOff = m_vehicleModule.DetectedParkNeutralSwitchOff();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to shift to Drive
			DisplayPrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if the prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ShiftToDrive"), GetPrompt("ShiftToDrive"), GetPromptPriority("ShiftToDrive"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the off switch test
	Log(LOG_DEV_DATA, "Saw Park/Neutral Switch Off: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckParkNeutralSwitchOffResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchSwitchOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOn = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchSwitchOnResult()\n");
	SetStartTime();
	do
	{	// Get the clutch switch on status
		switchOn = m_vehicleModule.DetectedClutchSwitchOn();
		// Determine if the operator should be prompted
		if(!switchOn && !promptDisplayed)
		{	// Instruct operator to press the brake pedal
			DisplayPrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
						  GetPromptPriority("PressClutchPedal"));
			promptDisplayed = true;
		}
		else if(switchOn && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
						 GetPromptPriority("PressClutchPedal"));
			promptDisplayed = false;
		}
		// If the switch is not on, delay before checking again
		if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not on
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
	// Check if the prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
					 GetPromptPriority("PressClutchPedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOn ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Clutch Switch On: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchSwitchOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchSwitchOffResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchSwitchOffResult()\n");
	SetStartTime();
	do
	{	// Get the clutch switch on status
		switchOff = m_vehicleModule.DetectedClutchSwitchOff();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to press the brake pedal
			DisplayPrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						  GetPromptPriority("ReleaseClutchPedal"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						 GetPromptPriority("ReleaseClutchPedal"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if the prompt needs to be removed
	if(promptDisplayed)
	{	// Remove the prompt from the display
		RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
					 GetPromptPriority("ReleaseClutchPedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the off switch test
	Log(LOG_DEV_DATA, "Saw Clutch Switch Off: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchSwitchOffResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericEmissionsTCTemplate<VehicleModuleType>::SetTestClutchUpstopSwitch(const bool &testClutchUpstop)
{
	m_boolTestClutchUpstopSwitch = testClutchUpstop;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericEmissionsTCTemplate<VehicleModuleType>::IsClutchUpstopSwitchEquipped(void)
{
	return m_boolTestClutchUpstopSwitch;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchUpstopSwitchResults(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update module clutch switch result";
	string switchOnResult = BEP_TESTING_STATUS;
	string switchOffResult = BEP_TESTING_STATUS;
	string switchOnClutchResult =  BEP_TESTING_STATUS;
	string switchOffClutchResult =  BEP_TESTING_STATUS;
	string switchOffUpstopOnResult = BEP_TESTING_STATUS;
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchUpstopSwitchResult()\n");
	try
	{	// Determine if the clutch upstop switch is active
		if(IsClutchUpstopSwitchEquipped())
		{	// Attempt to get the switch states
			switchOnResult = CheckClutchUpstopSwitchOnResult();
			switchOffResult = CheckClutchUpstopSwitchOffResult();
			switchOffUpstopOnResult =  CheckClutchOffUpstopOnResult();
			// Do not overwrite standard clutch switch test failure
			switchOnClutchResult = CheckClutchSwitchOnResult();
			switchOffClutchResult = CheckClutchSwitchOffResult();
			// Determine the test result
			testResult = (((switchOnResult == testPass) && (switchOffResult == testPass) &&
						   (switchOffUpstopOnResult == testPass) &&
						   (switchOnClutchResult == testPass) && 
						   (switchOffClutchResult == testPass)) ? testPass : testFail);
			Log(LOG_DEV_DATA, "Clutch Upstop switch result: %s\n\tOn: %s\n\tOff: %s\n",
				testResult.c_str(), switchOnResult.c_str(), switchOffResult.c_str());
			// Check if a state was not detected
			if(testPass != testResult)
			{	// Determine which state was not detected
				if((testPass != switchOnResult) && (testPass == switchOffResult))
				{	// Failed to see ON state
					testResultCode = GetFaultCode("ClutchUpstopSwitchOnFail");
					testDescription = GetFaultDescription("ClutchUpstopSwitchOnFail");
				}
				else if((testPass == switchOnResult) && (testPass != switchOffResult))
				{	// Failed to see OFF state
					testResultCode = GetFaultCode("ClutchUpstopSwitchOffFail");
					testDescription = GetFaultDescription("ClutchUpstopSwitchOffFail");
				}
				else if((testPass == switchOnResult) && (testPass != switchOffUpstopOnResult))
				{ //Saw Upstop On but not Upstop On and Clutch switch Off at the same time
					testResultCode = GetFaultCode("ClutchOffUpstopOnFail");
					testDescription = GetFaultDescription("ClutchOffUpstopOnFail");
				}
				else
				{	// Failed to see both states
					testResultCode = GetFaultCode("ClutchUpstopSwitchFail");
					testDescription = GetFaultDescription("ClutchUpstopSwitchFail");
				}
			}
			else
			{	// All good
				testResultCode = "0000";
				testDescription = "Monitor clutch Upostop switch On and Off";
			}
			// Verify Clutch and clutch Upstop state is correctly reported
			UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLClutchSwitchResultRegister")).c_str());
			UINT16 clutchSwitchTestBit = GetParameterInt("ClutchSwitchResultBit");
			moduleResult = UpdateEOLStatusRegister(eolStatusRegister, clutchSwitchTestBit, testResult);
			// Check the status of the update
			moduleCode = moduleResult == testPass ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
			moduleDescription = moduleResult == testPass ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
			Log(LOG_DEV_DATA, "Module update: %s\n", moduleResult.c_str());
		}
		else
		{
			Log(LOG_DEV_DATA, "Not equipped with Upstop, not checking Clutch Upstop Switch");
			testResult = testSkip;
		}
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in CheckClutchUpstopSwitchResults - %s\n", moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the results
	SendSubtestResultWithDetail("MonitorClutchUpstopSwitch", testResult, testDescription, testResultCode,
								"ClutchUpstopSwitchOn", switchOnResult, "", "ClutchUpstopSwitchOff", switchOffResult, "");
	SendSubtestResult("ClutchSwithcModuleUpdate", moduleResult, moduleDescription, moduleCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchUpstopSwitchResult(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchUpstopSwitchOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOn = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchUpstopSwitchOnResult()\n");
	SetStartTime();
	do
	{	// Get the clutch upstop on status
		switchOn = m_vehicleModule.DetectedClutchUpstopSwitchOn();
		// Determine if the operator should be prompted
		if(!switchOn && !promptDisplayed)
		{	// Instruct operator to press the clutch
			DisplayPrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
						  GetPromptPriority("PressClutchPedal"));
			promptDisplayed = true;
		}
		else if(switchOn && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
						 GetPromptPriority("PressClutchPedal"));
			promptDisplayed = false;
		}
		// If the switch is not on, delay before checking again
		if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not on
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("PressClutchPedal"), GetPrompt("PressClutchPedal"),
					 GetPromptPriority("PressClutchPedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOn ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Clucth Upstop Switch On: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchUpstopSwitchOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchUpstopSwitchOffResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchUpstopSwitchOffResult()\n");
	SetStartTime();
	do
	{	// Get the clutch upstop switch on status
		switchOff = m_vehicleModule.DetectedClutchUpstopSwitchOff();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to release the clutch pedal
			DisplayPrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						  GetPromptPriority("ReleaseClutchPedal"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						 GetPromptPriority("ReleaseBrakePedal"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
					 GetPromptPriority("ReleaseClutchPedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Clutch Upstop Switch Off: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchUpstopSwitchOffResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckClutchOffUpstopOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckClutchOffUpstopOnResult()\n");
	SetStartTime();
	do
	{	// Get the clutch switch on status
		switchOff = m_vehicleModule.DetectedClutchOffUpstopOn();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to press the brake pedal
			DisplayPrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						  GetPromptPriority("ReleaseClutchPedal"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
						 GetPromptPriority("ReleaseClutchPedal"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if the prompt needs to be removed
	if(promptDisplayed)
	{	// Remove the prompt from the display
		RemovePrompt(GetPromptBox("ReleaseClutchPedal"), GetPrompt("ReleaseClutchPedal"),
					 GetPromptPriority("ReleaseClutchPedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the off switch test
	Log(LOG_DEV_DATA, "Saw Clutch Off Upstop On Switch states: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckClutchOffUpstopOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericEmissionsTCTemplate<VehicleModuleType>::SetTestRedundantBrakeSwitch(const bool &testRedundantBrake)
{
	m_boolTestRedundantBrakeSwitch = testRedundantBrake;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericEmissionsTCTemplate<VehicleModuleType>::IsRedundantBrakeSwitchEquipped(void)
{
	return m_boolTestRedundantBrakeSwitch;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckRedundantBrakeSwitchResults(void)
{
	string testResult = BEP_TESTING_STATUS, moduleResult = BEP_TESTING_STATUS;
	string testResultCode = "0000", moduleCode = "0000";
	string testDescription = GetTestStepInfo("Description"), moduleDescription = "Update module brake switch result";
	string switchOnResult = BEP_TESTING_STATUS;
	string switchOffResult = BEP_TESTING_STATUS;
	string switchOnBrakeResult = BEP_TESTING_STATUS;
	string switchOffBrakeResult = BEP_TESTING_STATUS;
	// Log the entry
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchResults()\n");
	try
	{	// Determine if the redundant brake switch is active
		if(!m_etcVehicle)
		{	// Attempt to get the switch states
			switchOnResult = CheckRedundantBrakeSwitchOnResult();
			switchOffResult = CheckRedundantBrakeSwitchOffResult();
			// Consider standard brake switch result
			switchOnBrakeResult = CheckBrakeSwitchOnResult();
			switchOffBrakeResult = CheckBrakeSwitchOffResult();
			// Determine the test result
			testResult = (((switchOnResult == testPass) && (switchOffResult == testPass) &&
						   (switchOnBrakeResult == testPass) && 
						   (switchOffBrakeResult == testPass)) ? testPass : testFail);
			Log(LOG_DEV_DATA, "Redundant Brake switch result: %s\n\tOn: %s\n\tOff: %s\n",
				testResult.c_str(), switchOnResult.c_str(), switchOffResult.c_str());
			// Check if a state was not detected
			if(testPass != testResult)
			{	// Determine which state was not detected
				if((testPass != switchOnResult) && (testPass == switchOffResult))
				{	// Failed to see ON state
					testResultCode = GetFaultCode("RedundantBrakeSwitchOnFail");
					testDescription = GetFaultDescription("RedundantBrakeSwitchOnFail");
				}
				else if((testPass == switchOnResult) && (testPass != switchOffResult))
				{	// Failed to see OFF state
					testResultCode = GetFaultCode("RedundantBrakeSwitchOffFail");
					testDescription = GetFaultDescription("RedundantBrakeSwitchOffFail");
				}
				else
				{	// Failed to see both states
					testResultCode = GetFaultCode("RedundantBrakeSwitchFail");
					testDescription = GetFaultDescription("RedundantBrakeSwitchFail");
				}
			}
			else
			{	// All good
				testResultCode = "0000";
				testDescription = "Monitor for Redundant brake switch On and Off";
			}
			// Update the result in the module
			UINT16 eolStatusRegister = (UINT16)atoh(GetParameter(GetDataTag("EOLBrakeSwitchResultRegister")).c_str());
			UINT16 brakeSwitchTestBit = GetParameterInt("BrakeSwitchResultBit");
			moduleResult = UpdateEOLStatusRegister(eolStatusRegister, brakeSwitchTestBit, testResult);
			// Check the status of the update
			moduleCode = moduleResult == testPass ? moduleCode : GetFaultCode("ModuleUpdateCommunicationFailure");
			moduleDescription = moduleResult == testPass ? moduleDescription : GetFaultDescription("ModuleUpdateCommunicationFailure");
			Log(LOG_DEV_DATA, "Module Update: %s\n", moduleResult.c_str());
		}
		else
		{
			Log(LOG_DEV_DATA, "5.7L Engine with ETC, not checking Redundant BrakeSwitch");
			testResult = testSkip;
		}
	}
	catch(ModuleException &moduleException)
	{
		Log(LOG_ERRORS, "Module Exception in CheckRedundantBrakeSwitchResults - %s\n", moduleException.message().c_str());
		testResult = testSoftwareFail;
		testResultCode = GetFaultCode("SoftwareFailure");
		testDescription = GetFaultDescription("SoftwareFailure");
	}
	// Report the results
	SendSubtestResultWithDetail("MonitorRedundantBrakeSwitch", testResult, testDescription, testResultCode,
								"RedundantBrakeSwitchOn", switchOnResult, "", "RedundantBrakeSwitchOff", switchOffResult, "");
	SendSubtestResult("BrakeSwitchModuleUpdate", moduleResult, moduleDescription, moduleCode);
	// Return the test result
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchResults(%s)\n", testResult.c_str());
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckRedundantBrakeSwitchOnResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOn = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchOnResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch on status
		switchOn = m_vehicleModule.DetectedRedundantBrakeSwitchOn();
		// Determine if the operator should be prompted
		if(!switchOn && !promptDisplayed)
		{	// Instruct operator to press the brake pedal
			DisplayPrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
						  GetPromptPriority("PressBrakePedal"));
			promptDisplayed = true;
		}
		else if(switchOn && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
						 GetPromptPriority("PressBrakePedal"));
			promptDisplayed = false;
		}
		// If the switch is not on, delay before checking again
		if(!switchOn)  BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not on
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOn && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("PressBrakePedal"), GetPrompt("PressBrakePedal"),
					 GetPromptPriority("PressBrakePedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOn ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Redundant Brake Switch On: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchOnResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::CheckRedundantBrakeSwitchOffResult(void)
{
	string testResult = BEP_TESTING_STATUS;
	bool switchOff = false;
	bool promptDisplayed = false;
	// Start the timer
	Log(LOG_FN_ENTRY, "Enter GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchOffResult()\n");
	SetStartTime();
	do
	{	// Get the brake switch off status
		switchOff = m_vehicleModule.DetectedRedundantBrakeSwitchOff();
		// Determine if the operator should be prompted
		if(!switchOff && !promptDisplayed)
		{	// Instruct operator to release the brake pedal
			DisplayPrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
						  GetPromptPriority("ReleaseBrakePedal"));
			promptDisplayed = true;
		}
		else if(switchOff && promptDisplayed)
		{	// Remove the prompt
			RemovePrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
						 GetPromptPriority("ReleaseBrakePedal"));
			promptDisplayed = false;
		}
		// If the switch is not off, delay before checking again
		if(!switchOff)	BposSleep(GetTestStepInfoInt("ScanDelay"));
		// Keep checking while good system status, time remaining and switch not off
	} while((BEP_STATUS_SUCCESS == StatusCheck()) && !switchOff && TimeRemaining());
	// Check if prompt must be removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ReleaseBrakePedal"), GetPrompt("ReleaseBrakePedal"),
					 GetPromptPriority("ReleaseBrakePedal"));
		promptDisplayed = false;
	}
	// Determine the test result to return
	testResult = (switchOff ? testPass : testFail);
	// Log the status of the on switch test
	Log(LOG_DEV_DATA, "Saw Redundant Brake Switch Off: %s\n", testResult.c_str());
	Log(LOG_FN_ENTRY, "Exit GenericEmissionsTCTemplate::CheckRedundantBrakeSwitchOffResult()\n");
	// Return the test result
	return testResult;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
bool GenericEmissionsTCTemplate<VehicleModuleType>::CheckTransmissionInNeutral(void)
{
	bool transInNeutral = false;
	bool promptDisplayed = false;
	// Look for transmisison to be in Neutral
	do
	{	// Get the transmission neutral status
		transInNeutral = IsTransmissionInNeutral();
		// Determine if prompt should be displayed
		if(!transInNeutral && !promptDisplayed)
		{	// Display the prompt for the operator to shift to neutral
			DisplayPrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
			promptDisplayed = true;
		}
		else if(transInNeutral && promptDisplayed)
		{	// Remove the prompt from the display
			RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
			promptDisplayed = false;
		}
		if(!transInNeutral)	 BposSleep(GetTestStepInfoInt("ScanDelay"));
	} while(TimeRemaining() && !transInNeutral && (BEP_STATUS_SUCCESS == StatusCheck()));
	// Check if the prompt did not get removed
	if(promptDisplayed)
	{	// Remove the prompt
		RemovePrompt(GetPromptBox("ShiftToNeutral"), GetPrompt("ShiftToNeutral"), GetPromptPriority("ShiftToNeutral"));
		promptDisplayed = false;
	}
	// Log the data and return the result
	Log(LOG_DEV_DATA, "Transmission in Neutral: %s\n", transInNeutral ? "True" : "False");
	return transInNeutral;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::ResetStatusRegisters(const XmlNodeMap *pcmRegisters)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	// Log the function entry
	Log(LOG_DEV_DATA, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{	// Reset all of the registers to default values for development testing
		for(XmlNodeMapCItr iter = pcmRegisters->begin();
		   (BEP_STATUS_SUCCESS == moduleStatus) && (iter != pcmRegisters->end()); iter++)
		{	// Write the value to the register
			moduleStatus = m_vehicleModule.UpdateEOLStatusByte(atoh(iter->second->getValue().c_str()),
															   atoh(iter->second->getAttribute("Value")->getValue().c_str()));
			// Log the status of the update
			Log(LOG_DEV_DATA, "Updated register %s to %s -- %s\n", iter->second->getValue().c_str(),
				iter->second->getAttribute("Value")->getValue().c_str(), ConvertStatusToResponse(moduleStatus).c_str());
		}
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlExcpetion resetting status registers: %s\n", excpt.GetReason());
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BepException resetting status registers: %s\n", excpt.GetReason());
	}
	// Log the function exit
	Log(LOG_DEV_DATA, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Return the result of the step
	return testPass;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
string GenericEmissionsTCTemplate<VehicleModuleType>::ReadStatusRegisters(void)
{
	BEP_STATUS_TYPE moduleStatus = BEP_STATUS_SUCCESS;
	// Log the function entry
	Log(LOG_DEV_DATA, "Enter %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
	try
	{	// Read all the status registers
		Log(LOG_DEV_DATA, "Reading %d status registers...\n", m_defaultRegisters.size());
		for(XmlNodeMapCItr iter = m_defaultRegisters.begin();
		   (BEP_STATUS_SUCCESS == moduleStatus) && (iter != m_defaultRegisters.end()); iter++)
		{	// Read the register from the module
			string statusRegisterTag = "ReadStatusRegister_" + iter->second->getValue();
			UINT16 statusRegisterValue = 0x0000;
			moduleStatus = m_vehicleModule.GetInfo(statusRegisterTag, statusRegisterValue);
			// Log the data
			Log(LOG_DEV_DATA, "Status Register %s: 0x%04X\n", iter->second->getValue().c_str(), statusRegisterValue);
		}
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "XmlExcpetion reading status registers: %s\n", excpt.GetReason());
	}
	catch(BepException &excpt)
	{
		Log(LOG_ERRORS, "BepException reading status registers: %s\n", excpt.GetReason());
	}
	// Log the function exit
	Log(LOG_DEV_DATA, "Exit %s::%s()\n", GetComponentName().c_str(), GetTestStepName().c_str());
	// Return the result of the step
	return testPass;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericEmissionsTCTemplate<VehicleModuleType>::SetPreviousPassStatus(const bool &previousPassStatus)
{	// Save the previous pass status
	m_isPreviousPass = previousPassStatus;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericEmissionsTCTemplate<VehicleModuleType>::StoreCurrentEEROMOdometerReading(const UINT16 &currentReading)
{
	m_currentOdometerReading = currentReading;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
const UINT16& GenericEmissionsTCTemplate<VehicleModuleType>::GetCurrentOdometerEEROMReading(void)
{
	return m_currentOdometerReading;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline const bool& GenericEmissionsTCTemplate<VehicleModuleType>::IsETCEquipped(void)
{
	return m_etcVehicle;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline void GenericEmissionsTCTemplate<VehicleModuleType>::SetETCEquipped(const bool &equipped)
{
	Log(LOG_DEV_DATA, "Emission ETC Vehicle: %s", equipped ? "True" : "False");
	m_etcVehicle = equipped;
}

//-------------------------------------------------------------------------------------------------
template <class VehicleModuleType>
inline bool GenericEmissionsTCTemplate<VehicleModuleType>::NeedToRunHydraulicCoolingFan(void)
{
	return(m_engineSize == GetParameter("EngineSizeForHydraulicCoolingFanTest"));
}
