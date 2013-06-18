//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Mahindra.  The system monitor will determine when
//  conditions are correct for starting a vehicle test as well as determining
//  when a test has completed and handle the system cleanup.
//
//===========================================================================
// Copyright (c) 2010- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//*************************************************************************
#include "MahindraSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
MahindraSystemMonitor::MahindraSystemMonitor() : SystemMonitor()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MahindraSystemMonitor::MahindraSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MahindraSystemMonitor::~MahindraSystemMonitor()
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void MahindraSystemMonitor::CheckTesting(ControlData *ctrl)
{	// Log the entry
	static bool testSelected = false;
	Log(LOG_FN_ENTRY, "MahindraSystemMonitor::CheckTesting() - Enter");
	//Log the values for debugging
	Log(LOG_DEV_DATA, "ctrl->       testInProgress value: %s", ctrl->testInProgress ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "             testSelected value:   %s", testSelected ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "ctrl->       calSwitch value:      %s", ctrl->calSwitch ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "ctrl->       machineFault value:   %s", ctrl->machineFault ? "TRUE" : "FALSE" );        
	Log(LOG_DEV_DATA, "ctrl->       rollsDown value:      %s", ctrl->rollsDown ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "ctrl->       rollsUp value:        %s", ctrl->rollsUp ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "ctrl->       vehiclePresent value: %s", ctrl->vehiclePresent ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "m_oldCtrl->  vehiclePresent value: %s", m_oldCtrl->vehiclePresent ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "ctrl->       vehVinReadStatus != VALID_VEHICLE_VIN) value:  %s", (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) ? "TRUE" : "FALSE" );
	Log(LOG_DEV_DATA, "m_oldCtrl->  vehVinReadStatus != VALID_VEHICLE_VIN) value:  %s", (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN) ? "TRUE" : "FALSE" );
	// Make sure there are no machine faults that would prevent testing
	if(!ctrl->machineFault && !ctrl->calSwitch)
	{	//  If a test is in progress, there is nothing to do
		if(ctrl->testInProgress)
		{	// Test in progress, nothing to do
			Log(LOG_DEV_DATA, "Test in progress, nothing to do.");
			RemovePrompt(1, "StartLossComp");
			RemovePrompt(2, "RaiseRetainers");
		}
		// No test in progress, rolls closed and no valid vehicle build, prompt to enter VIN
		else if(!ctrl->testInProgress && ctrl->rollsDown && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))
		{
			string tempState;
			DisplayPrompt(1, "EnterVIN", "white");
			RemovePrompt(2, "RaiseRetainers");
			WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
			if(ReadNdbData(INPUT_SERVER_STATE, tempState).compare(INPUT_SERVER_VIN_STATE))
			{
				Log(LOG_DEV_DATA, "Setting %s to %s", INPUT_SERVER_STATE, INPUT_SERVER_VIN_STATE);
				WriteNdbData(INPUT_SERVER_STATE, string(INPUT_SERVER_VIN_STATE));
			}
			testSelected = false;
		}
		// Load the build data
		// - Rolls down
		// - Build data just loaded
		else if(ctrl->rollsDown && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && !ctrl->vehiclePresent &&
				(m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN))
		{
			BposSleep(500);   // Wait for the planthost inbound to load the data
			CommandNdbData(READ_LATEST_BUILD_DATA_TAG, true);
			DisplayPrompt(1, "MachineReady");
		}
		// No test in progress and vehicle present and rolls down
		else if(!ctrl->testInProgress && ctrl->vehiclePresent &&  
				(ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && ctrl->rollsDown)
		{	// Prompt operator to open rollers to start test
			RemovePrompt(1, "EnterVIN");
			DisplayPrompt(1, "MachineReady");
			CommandNdbData(READ_LATEST_BUILD_DATA_TAG, true);
			BposSleep(5000);
			RemovePrompt(1, "MachineReady");
			DisplayPrompt(2, "RaiseRetainers");
			WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
		}
		// No test in progress, rolls just opened and vehicle present
		else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && 
				(ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && !testSelected)
		{	// Remove prompts and command test to start
			RemovePrompt(2, "RaiseRetainers");
			// Start the test sequence
			CommandNdbData(START_VEHICLE_TEST_DATA_TAG, true);
			// Invalidate the build data
			WriteNdbData(VIN_READ_STATUS_TAG, string(INVALID_VEHICLE_VIN));
			testSelected = true;
		}
	}
	else
	{	// Machine faulted, cannot start test
		Log(LOG_ERRORS, "Machine faulted, cannot start test sequence");
	}
	// Log the exit
	Log(LOG_FN_ENTRY, "MahindraSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string MahindraSystemMonitor::Publish(const XmlNode *node)
{	// If this is the vehicle type, load the build data
	Log(LOG_FN_ENTRY, "MahindraSystemMonitor::Publish(%s, %s) - Enter", node->getName().c_str(), node->getValue().c_str());
	if(!node->getName().compare("VehicleTypeFromPLC"))
	{
		CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
	}
	//if the rolls are down, update vehVinReadStatus to invalid
	else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && (!node->getValue().compare("1")))
	{
		// Invalidate the build data
		WriteNdbData(VIN_READ_STATUS_TAG, string(INVALID_VEHICLE_VIN));
		WriteNdbData(VINDISPLAY_DATA_TAG, string(" "));
	}
	Log(LOG_FN_ENTRY, "MahindraSystemMonitor::Publish(%s, %s) - Exit", node->getName().c_str(), node->getValue().c_str());
	// Finish the normal publish
	return SystemMonitor::Publish(node);
}

//-------------------------------------------------------------------------------------------------
const string MahindraSystemMonitor::Register(void)
{   // Call the base class to begin the registration
	string result = SystemMonitor::Register();
	// Make sure the enter vin prompt is displayed at system startup
	DisplayPrompt(1, "EnterVIN", "white");
	return result;
}
