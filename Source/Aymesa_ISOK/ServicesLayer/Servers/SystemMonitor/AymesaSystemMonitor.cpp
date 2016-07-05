//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Aymesa.  The system monitor will determine when
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
#include "AymesaSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
AymesaSystemMonitor::AymesaSystemMonitor() : SystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
AymesaSystemMonitor::AymesaSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
AymesaSystemMonitor::~AymesaSystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void AymesaSystemMonitor::CheckTesting(ControlData *ctrl)
{   // Log the entry
    static bool testSelected = false;
    Log(LOG_FN_ENTRY, "AymesaSystemMonitor::CheckTesting() - Enter");
    // Make sure there are no machine faults that would prevent testing
    if(!ctrl->machineFault && !ctrl->calSwitch)
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
			DisplayPrompt(1, "ConnectCableIfAbs");
            DisplayPrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
        }
        // No test in progress, rolls just opened and vehicle present
        else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && !testSelected)
        {   // Remove prompts and command test to start
            RemovePrompt(2, "RaiseRetainers");
            // Start the test sequence 
            //CommandNdbData(string(COMMAND_TEST), string(TEST_BASE_BRAKE));
            CommandNdbData(string(START_VEHICLE_TEST_DATA_TAG), string("1"));
            testSelected = true;
        }
#if 0
        // No test in progress, rolls open and no vehicle, start loss compensation sequence
        else if(!ctrl->testInProgress && ctrl->rollsUp && !ctrl->vehiclePresent && !testSelected)
        {   // Only start the test if the operator is pressing the button
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("losscompensation"));
            CommandNdbData(string(COMMAND_TEST), string(TEST_LOSS_COMPENSATION));
            RemovePrompt(2, "RaiseRetainers");
            testSelected = true;
        }
#endif
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
            DisplayPrompt(1, "MachineReady");
        }
    }
    else
    {   // Machine faulted, cannot start test
        Log(LOG_ERRORS, "Machine faulted, cannot start test sequence");
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "AymesaSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string AymesaSystemMonitor::Publish(const XmlNode *node)
{   // If this is the vehicle type, load the build data
    Log(LOG_FN_ENTRY, "AymesaSystemMonitor::Publish(%s, %s) - Enter", node->getName().c_str(), node->getValue().c_str());
    if(!node->getName().compare("VehicleTypeFromPLC"))
    {   
        CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
    }
    Log(LOG_FN_ENTRY, "AymesaSystemMonitor::Publish(%s, %s) - Exit", node->getName().c_str(), node->getValue().c_str());
    // Finish the normal publish
    return SystemMonitor::Publish(node);
}
