//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for John Deere.  The system monitor will determine when
//  conditions are correct for starting a vehicle test as well as determining
//  when a test has completed and handle the system cleanup.
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
//*************************************************************************
#include "JohnDeereSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
JohnDeereSystemMonitor::JohnDeereSystemMonitor() : SystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
JohnDeereSystemMonitor::JohnDeereSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
JohnDeereSystemMonitor::~JohnDeereSystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void JohnDeereSystemMonitor::CheckTesting(ControlData *ctrl)
{   // Log the entry
    static bool testSelected = false;
    Log(LOG_FN_ENTRY, "JohnDeereSystemMonitor::CheckTesting() - Enter");
    // Make sure there are no machine faults that would prevent testing
    if(!ctrl->machineFault /*&& !ctrl->calSwitch*/)
    {   //  If a test is in progress, there is nothing to do
        if(ctrl->testInProgress)
        {   // Test in progress, nothing to do
            Log(LOG_DEV_DATA, "Test in progress, nothing to do.");
            RemovePrompt(1, "StartLossComp");
            RemovePrompt(2, "RaiseRetainers");
        }
        // No test in progress and vehicle just arrived and rolls down
        else if(!ctrl->testInProgress && ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent && ctrl->rollsDown)
        {   // Prompt operator to open rollers to start test
            RemovePrompt(1, "EnterVIN");
            RemovePrompt(1, "MachineReady");
            DisplayPrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
        }
        // No test in progress, rolls just opened and vehicle present
        else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && !testSelected)
        {   // Remove prompts and command test to start
            RemovePrompt(2, "RaiseRetainers");
            // Start the test sequence 
            if (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) 
            {
                WriteNdbData( VIN_DATA_TAG, string("BASE_BRAKE_TEST__"));
                CommandNdbData(string(COMMAND_TEST), string(TEST_BASE_BRAKE));
            }
            else
            {
                CommandNdbData(string("ReadLatestBuildData"),string("1"));
                CommandNdbData(string(COMMAND_TEST), string(TEST_START));
            }
            
            testSelected = true;
        }
        /* No test in progress, rolls open and no vehicle, start loss compensation sequence
        else if(!ctrl->testInProgress && ctrl->rollsUp && !ctrl->vehiclePresent && !testSelected && ctrl->calSwitch && !m_oldCtrl->calSwitch)
        {   // Only start the test if the operator is pressing the button
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("losscompensation"));
            CommandNdbData(string(COMMAND_TEST), string(TEST_LOSS_COMPENSATION));
            RemovePrompt(2, "RaiseRetainers");
            testSelected = true;
        }*/
        // No test in progress, rolls closed and no vehicle, prompt to enter VIN
        else if(!ctrl->testInProgress && ctrl->rollsDown && !ctrl->vehiclePresent && 
                (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))
        {
            DisplayPrompt(1, "EnterVIN");
            RemovePrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
            testSelected = false;
        }
        // Load the build data
        // - Rolls down
        // - No vehicle present
        // - Build data just loaded
        else if(ctrl->rollsDown && !ctrl->vehiclePresent && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) &&
                (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN))
        {
            CommandNdbData(READ_LATEST_BUILD_DATA_TAG, true);
			WriteNdbData("StartLamp", true);
            DisplayPrompt(1, "MachineReady");
        }
    }
    else
    {   // Machine faulted, cannot start test
        Log(LOG_ERRORS, "Machine faulted, cannot start test sequence");
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "JohnDeereSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
void JohnDeereSystemMonitor::LoadTestData()
{
	// Command the Vehicle Data Broker to read the build data
	CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
	// Command the Vehicle Test Server to load the test
	CommandNdbData(LOAD_VEHICLE_TEST_DATA_TAG, "1");
}

//-------------------------------------------------------------------------------------------------
const string JohnDeereSystemMonitor::Publish(const XmlNode *node)
{   // If this is the vehicle type, load the build data
    Log(LOG_FN_ENTRY, "JohnDeereSystemMonitor::Publish(%s, %s) - Enter", node->getName().c_str(), node->getValue().c_str());
    if(!node->getName().compare("VehicleTypeFromPLC"))
    {   
        CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
    }
	else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && atob(node->getValue().c_str()))
	{   // Rolls just closed, clear the VIN and VC from the display
		WriteNdbData("VCNumber", string("??"));
		WriteNdbData(VINDISPLAY_DATA_TAG, string("??"));
		WriteNdbData("StartLamp", false);
	}
	else if(!node->getName().compare(VIN_READ_STATUS_TAG) && 
			!node->getValue().compare(VALID_VEHICLE_VIN))
	{
		LoadTestData();
	}
    Log(LOG_FN_ENTRY, "JohnDeereSystemMonitor::Publish(%s, %s) - Exit", node->getName().c_str(), node->getValue().c_str());
    // Finish the normal publish
    return SystemMonitor::Publish(node);
}
