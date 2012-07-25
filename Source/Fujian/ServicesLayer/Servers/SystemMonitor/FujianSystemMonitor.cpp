//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Fujian.  The system monitor will determine when
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
#include "FujianSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
FujianSystemMonitor::FujianSystemMonitor() : SystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
FujianSystemMonitor::FujianSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
FujianSystemMonitor::~FujianSystemMonitor()
{   // Nothing special to do here
}



//------------------------------------------------------------------------------------------------- 
void FujianSystemMonitor::CheckTesting(ControlData *ctrl) 
{ // Log the entry 
    static bool testSelected = false; 
    Log(LOG_FN_ENTRY, "FujianSystemMonitor::CheckTesting() - Enter"); 

    //Log the values for debugging
        Log(LOG_DEV_DATA, "ctrl->       testInProgress value: %s", ctrl->testInProgress ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "             testSelected value:   %s", testSelected ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "ctrl->       calSwitch value:      %s", ctrl->calSwitch ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "ctrl->       machineFault value:   %s", ctrl->machineFault ? "TRUE" : "FALSE" );        
        Log(LOG_DEV_DATA, "ctrl->       rollsDown value:      %s", ctrl->rollsDown ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "ctrl->       rollsUp value:        %s", ctrl->rollsUp ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "ctrl->       vehiclePresent value: %s", ctrl->vehiclePresent ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "m_oldCtrl->  vehiclePresent value: %s", m_oldCtrl->vehiclePresent ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "(ctrl->      vehVinReadStatus != VALID_VEHICLE_VIN) value:  %s", (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) ? "TRUE" : "FALSE" );
        Log(LOG_DEV_DATA, "(m_oldCtrl-> vehVinReadStatus != VALID_VEHICLE_VIN) value:  %s", (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN) ? "TRUE" : "FALSE" );

    // Make sure there are no machine faults that would prevent testing
    if(!ctrl->machineFault && !ctrl->calSwitch)
    {   
        Log(LOG_DEV_DATA, "1 Enter: if(!ctrl->machineFault && !ctrl->calSwitch)");
                //  If a test is in progress, there is nothing to do
        if(ctrl->testInProgress)
        {   // Test in progress, nothing to do
            Log(LOG_DEV_DATA, "2 Enter: if(ctrl->testInProgress) ");
            Log(LOG_DEV_DATA, "Test in progress, nothing to do.");
            RemovePrompt(1, "StartLossComp");
            RemovePrompt(2, "RaiseRetainers");
        }
        // No test in progress and vehicle just arrived and rolls down
        else if(!ctrl->testInProgress && ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent && ctrl->rollsDown)
        {   // Prompt operator to open rollers to start test
            Log(LOG_DEV_DATA, "3 Enter: else if(!ctrl->testInProgress && ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent && ctrl->rollsDown)");
            RemovePrompt(1, "EnterVIN");
            RemovePrompt(1, "MachineReady");
            DisplayPrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
        }
        // No test in progress, rolls just opened and vehicle present
        else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && !testSelected)
        {   // Remove prompts and command test to start
            Log(LOG_DEV_DATA, "4 Enter: else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && !testSelected)");
            RemovePrompt(2, "RaiseRetainers");
            // Start the test sequence 
            CommandNdbData(string(COMMAND_TEST), string(TEST_BASE_BRAKE));
            // Invalidate the build data
            WriteNdbData(VIN_READ_STATUS_TAG, string(INVALID_VEHICLE_VIN));
            testSelected = true;
        }
    #if 0
        // No test in progress, rolls open and no vehicle, start loss compensation sequence
        else if(!ctrl->testInProgress && ctrl->rollsUp && !ctrl->vehiclePresent && !testSelected)
        {   // Only start the test if the operator is pressing the button
            Log(LOG_DEV_DATA, "5 Enter: else if(!ctrl->testInProgress && ctrl->rollsUp && !ctrl->vehiclePresent && !testSelected)");
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
            Log(LOG_DEV_DATA, "6 Enter: else if(!ctrl->testInProgress && ctrl->rollsDown && !ctrl->vehiclePresent && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))");
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
            Log(LOG_DEV_DATA, "7 Enter: else if(ctrl->rollsDown && !ctrl->vehiclePresent && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN))");
            CommandNdbData(READ_LATEST_BUILD_DATA_TAG, true);
            DisplayPrompt(1, "MachineReady");
        }
    }
    else
    {   // Machine faulted, cannot start test
        Log(LOG_DEV_DATA, "8 Enter: else");
        Log(LOG_ERRORS, "Machine faulted, cannot start test sequence");
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "FujianSystemMonitor::CheckTesting() - Exit");
}
//-------------------------------------------------------------------------------------------------
const string FujianSystemMonitor::Publish(const XmlNode *node)
{   // If this is the vehicle type, load the build data
    Log(LOG_FN_ENTRY, "FujianSystemMonitor::Publish(%s, %s) - Enter", node->getName().c_str(), node->getValue().c_str());
    if(!node->getName().compare("VehicleTypeFromPLC"))
    {   
        CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
    } 
    //if the rolls are down, update vehVinReadStatus to invalid
    else if (node->getName().compare("RollsDownElevUp") && (node->getValue().compare("1")))
    { 
        Log(LOG_DEV_DATA, "Publish Enter: (node->getName().compare(\"RollsDownElevUp\") && (node->getValue().compare(\"1\")))");
        // Invalidate the build data
        WriteNdbData(VIN_READ_STATUS_TAG, string(INVALID_VEHICLE_VIN));
    }
    Log(LOG_FN_ENTRY, "FujianSystemMonitor::Publish(%s, %s) - Exit", node->getName().c_str(), node->getValue().c_str());
    // Finish the normal publish
    return SystemMonitor::Publish(node);
}
