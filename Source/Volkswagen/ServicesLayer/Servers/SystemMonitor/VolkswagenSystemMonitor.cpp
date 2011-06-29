//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Volkswagen.  The system monitor will determine when
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
#include "VolkswagenSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
VolkswagenSystemMonitor::VolkswagenSystemMonitor() : SystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
VolkswagenSystemMonitor::VolkswagenSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
VolkswagenSystemMonitor::~VolkswagenSystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void VolkswagenSystemMonitor::CheckTesting(ControlData *ctrl)
{   // Log the entry
    static bool testSelected = false;
    Log(LOG_FN_ENTRY, "VolkswagenSystemMonitor::CheckTesting() - Enter");
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
        else if(!ctrl->testInProgress && ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent && 
                ctrl->rollsDown && !testSelected)
        {   // Prompt operator to open rollers to start test
            DisplayPrompt(1, "ConnectCable");
            DisplayPrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
        }
        // No test in progress, rolls just opened and vehicle present
        else if(!ctrl->testInProgress && ctrl->rollsUp && ctrl->vehiclePresent && !testSelected)
        {   // Remove prompts and command test to start
            RemovePrompt(1, "ConnectCable");
            RemovePrompt(2, "RaiseRetainers");
            DisplayPrompt(1, "PressButtonToStartTest");
            // Start the test sequence after the operator presses the start test button
            if(ctrl->startTestPb && !m_oldCtrl->startTestPb)
            {
                RemovePrompt(1, "PressButtonToStartTest");
                CommandNdbData(READ_LATEST_BUILD_DATA_TAG, true);
                BposSleep(1000);
                CommandNdbData(START_VEHICLE_TEST_DATA_TAG, true);
                testSelected = true;
            }
        }
        // No Test in progress, vehicle present and rolls now down - test ended
        else if(!ctrl->testInProgress && ctrl->rollsDown && ctrl->vehiclePresent && !m_oldCtrl->rollsDown)
        {
            RemovePrompt(1, "PressButtonToStartTest");
        }
        // No test in progress, rolls open and no vehicle, start loss compensation sequence
        else if(!ctrl->testInProgress && ctrl->rollsUp && !ctrl->vehiclePresent && !testSelected)
        {   // Only start the test if the operator is pressing the button
            DisplayPrompt(1, "StartLossComp");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("losscompensation"));
            if(ctrl->startTestPb && !m_oldCtrl->startTestPb)
            {
                CommandNdbData(string(COMMAND_TEST), string(LOSSCOMPENSATION_TAG));
                RemovePrompt(1, "StartLossComp");
                RemovePrompt(2, "RaiseRetainers");
                testSelected = true;
            }
        }
        // No test in progress, rolls closed and no vehicle, prompt to enter VIN
        else if(!ctrl->testInProgress && ctrl->rollsDown && !ctrl->vehiclePresent)
        {
            DisplayPrompt(1, "EnterVIN");
            RemovePrompt(2, "RaiseRetainers");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, string("frontpanel"));
            testSelected = false;
        }
    }
    else
    {   // Machine faulted, cannot start test
        Log(LOG_ERRORS, "Machine faulted, cannot start test sequence");
    }
    // Log the exit
    Log(LOG_FN_ENTRY, "VolkswagenSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
const string VolkswagenSystemMonitor::Publish(const XmlNode *node)
{   // If this is the vehicle type, load the build data
    Log(LOG_FN_ENTRY, "VolkswagenSystemMonitor::Publish(%s, %s) - Enter", node->getName().c_str(), node->getValue().c_str());
    if(!node->getName().compare("VehicleTypeFromPLC"))
    {   
        CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1")); 
    }
    Log(LOG_FN_ENTRY, "VolkswagenSystemMonitor::Publish(%s, %s) - Exit", node->getName().c_str(), node->getValue().c_str());
    // Finish the normal publish
    return SystemMonitor::Publish(node);
}
