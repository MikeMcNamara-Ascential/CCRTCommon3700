//*************************************************************************
// FILE DESCRIPTION:
//  System monitor for Isuzu.  The system monitor will determine when
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
#include "IsuzuSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
IsuzuSystemMonitor::IsuzuSystemMonitor() : SystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
IsuzuSystemMonitor::IsuzuSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
IsuzuSystemMonitor::~IsuzuSystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
void IsuzuSystemMonitor::CheckTesting( ControlData *ctrl)
{   // Log the entry and make sure there are no machine faults that prevent testing
    Log(LOG_FN_ENTRY, "IsuzuSystemMonitor::CheckTesting() - Enter");
    if(!ctrl->machineFault)
    {   // If there is a test in progress, there is nothing to do
        if(ctrl->testInProgress)
        {
            Log(LOG_DEV_DATA, "Test in progress, nothing to do");
        }
        // Loss compensation requested
        // - Loss comp key on
        else if(ctrl->calSwitchToPLC && !m_oldCtrl->calSwitchToPLC)
        {   // Raise the retaining rollers
            WriteNdbData(RAISE_ROLLS, string("1"));
        }
        // Loss compensation completed
        // - Loss comp key off
        else if(!ctrl->calSwitchToPLC && m_oldCtrl->calSwitchToPLC)
        {   // Raise the retaining rollers
            WriteNdbData(RAISE_ROLLS, string("0"));
        }
        // Prompt to enter VIN
        // - No build data waiting
        // - Rolls down
        // - ?! bar code scanner is at driver's interface removing requirement - No vehicle present
        // - rolls were not just lowered
        else if((ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) && (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
                ctrl->rollsDown && m_oldCtrl->rollsDown && (ctrl->rollsDown && !m_oldCtrl->rollsDown)) //&& !ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent)
        {
            DisplayPrompt(1, "EnterVIN", "white");
            // Return the Input server to VIN state
            if(ctrl->inputServerState != INPUT_SERVER_VIN_STATE)
            {
                WriteNdbData(string(INPUT_SERVER_STATE), string(INPUT_SERVER_VIN_STATE));
            }
        }
        // Prompt good build data waiting
        // - good scan just received
        else if((ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN))
        {   // Prompt good data received
            RemovePrompt(1, "EnterVIN", "white");
            DisplayPrompt(2, "GoodScan", "white");
            // Load the build data
            CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1"));
			BposSleep(1000);   // Wait for the build data to be processed
            AdjustWheelbase();
            // Prepare the input server
            if(ctrl->inputServerState != INPUT_SERVER_TEST_RESULT_STATE)
            {
                WriteNdbData(string(INPUT_SERVER_STATE), string(INPUT_SERVER_TEST_RESULT_STATE));
            }
        }
        // Load vehicle build data and prompt to connect cable
        // - Vehicle Present
        // - Good vehicle build data waiting
        // - Rolls down
        else if(ctrl->vehiclePresent && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && !ctrl->cableConnect &&
                ctrl->rollsDown && m_oldCtrl->rollsDown)
        {   // Command the vehicle data broker to get the vehicle build
            RemovePrompt(1, "EnterVIN");
            RemovePrompt(2, "PressButtonToStartTest");
            DisplayPrompt(1, "ConnectCable", "white");
            CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1"));
            BposSleep(1000);   // Wait for the build data to be processed
            AdjustWheelbase();
        }
        // Cable connected, auto raise the retainers
        else if(!m_oldCtrl->cableConnect && ctrl->cableConnect)
        {
            RemovePrompt(1, "ConnectCable", "white");
            //removed per request - prompt operator to raise retainers
            WriteNdbData(RAISE_ROLLS, string("1"));
            DisplayPrompt(1, "RaiseRetainers", "white");
        }         
        // Start the test sequence
        // - Vehicle present
        // - Retainers up
        // - Build data loaded
        // - Test not in progress
        else if(ctrl->vehiclePresent && ctrl->rollsUp && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) &&
                !ctrl->testInProgress && !m_oldCtrl->testInProgress)
        {
            RemovePrompt(1, "RaiseRetainers");
            RemovePrompt(2, "PressButtonToStartTest");
            // Read the build data in case the operator did not lower the rollers
            CommandNdbData(string(READ_LATEST_BUILD_DATA_TAG), string("1"));
            BposSleep(1000);   // Wait for the build data to be processed
			AdjustWheelbase();
            // Start the vehicle test
            CommandNdbData(string(START_VEHICLE_TEST_DATA_TAG), string("1"));
            RemovePrompt(2, "GoodScan", "white");
        }
        // Prompt to Disconnect cable
        // - Cable Connected
        // - Test just ended
        else if(ctrl->cableConnect && !ctrl->testInProgress && m_oldCtrl->testInProgress)
        {   // Clear the VIN Read status
            WriteNdbData(string(VIN_READ_STATUS_TAG), string(INVALID_VEHICLE_VIN));
            DisplayPrompt(2, "DisconnectCable", "white");
        }
        // Clear the disconnect cable prompt
        // - Cable just disconnected
        else if(m_oldCtrl->cableConnect && !ctrl->cableConnect)
        {
            RemovePrompt(2, "DisconnectCable", "white");
            // Auto lower the retainers
            WriteNdbData(RAISE_ROLLS, string("0"));
        }
        // Make sure the retaining rolls are down no matter what
        // - Cable not connected
        // - Test just completed
        // - Retainers up
        else if(!ctrl->cableConnect && !m_oldCtrl->cableConnect && m_oldCtrl->testInProgress && ctrl->rollsUp)
        {   // Auto lower the retainers
            WriteNdbData(RAISE_ROLLS, string("0"));
        }
        // Prompt OK to leave
        // - Rolls just lowered
        // - Vehicle present
        else if(ctrl->vehiclePresent && ctrl->rollsDown && !m_oldCtrl->rollsDown)
        {   // Clear the VIN Read status
            //wait for cable disconnect
            if((bool)(ReadSubscribeData( PLC_CBL_CONNECT) == "1"))
            {
                DisplayPrompt(2,"DisconnectCable", "white");
                while((bool)(ReadSubscribeData( PLC_CBL_CONNECT) == "1"))
                {
                    BposSleep(500);
                }
                RemovePrompt(2,"DisconnectCable", "white");
            }
            WriteNdbData(string(VIN_READ_STATUS_TAG), string(INVALID_VEHICLE_VIN));
            DisplayPrompt(2, "ClearToExit", "white", true);
        }
        // Clear the build data
        // - Vehicle present just lost
        // - Valid Build data
        // - InputServer not in Normal state
        else if(!ctrl->vehiclePresent && m_oldCtrl->vehiclePresent) //&& (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))
        {   
            DisplayPrompt(1, "EnterVIN", "white");
            RemovePrompt(1, "RaiseRetainers");
            RemovePrompt(2, "ClearToExit", "white");
            RemovePrompt(2, "GoodScan", "white");
            RemovePrompt(2, "ClearToExit", "white");
            // Return the Input server to Normal state
            if(ctrl->inputServerState != INPUT_SERVER_NORMAL_STATE)
            {
                WriteNdbData(string(INPUT_SERVER_STATE), string(INPUT_SERVER_NORMAL_STATE));
            }
        }
    }
    else
    {   // Machine faulted, cannot do anything
        Log(LOG_ERRORS, "Machine faulted, nothing to do!!");
    }
}

//-------------------------------------------------------------------------------------------------
const string IsuzuSystemMonitor::Publish(const XmlNode *node)
{
    Log(LOG_FN_ENTRY, "IsuzuSystemMonitor::Publish(%s:%s) - Enter", node->getName().c_str(), node->getValue().c_str());
    bool updateControl = true;
    string result(BEP_ERROR_RESPONSE);
    /* if(!node->getName().compare(WHEELBASE_IN_POSITION_TAG))
    {   // Handle the wheelbase adjusting prompts
        if(!atob(node->getValue().c_str()))  
        {
            DisplayPrompt(1, "WheelbaseIncorrect");
            updateControl = false;
            result = BEP_SUCCESS_RESPONSE;
        }
        else
        {
            RemovePrompt(1, "WheelbaseIncorrect");
        }
    } */
    // Determine if the base class should be called
    if(updateControl)
    {
        result = SystemMonitor::Publish(node);
    }
    Log(LOG_FN_ENTRY, "IsuzuSystemMonitor::Publish(%s:%s) - Exit", node->getName().c_str(), node->getValue().c_str());
    // Call the base class to handle the rest of the publish
    return result;
}

//-------------------------------------------------------------------------------------------------
void IsuzuSystemMonitor::UpdateDriverMonitor( ControlData *ctrl)
{
    Log( LOG_FN_ENTRY, "Enter IsuzuSystemMonitor::UpdateDriverMonitor()");
    // if in a LC operation
    if (ctrl->calSwitchToPLC || ctrl->calCycle)
    {   // if a LC test is not being performed
        if (!ctrl->testInProgress)
        {   // if the cable is connected and a test has not been started
            if (ctrl->cableConnect && !ctrl->calKeyCycled)
            {
                DisplayPrompt( 2,  "DisconnectCable", false);
            }
            // else if vehicle present, prompt to remove vehicle if a test has not been started
            else if (ctrl->vehiclePresent && !ctrl->calKeyCycled)
            {
                DisplayPrompt( 2,  "PleaseRemoveVehicle", false);
            }
            // else if the rolls are still down and a test has not been started
            else if (ctrl->rollsDown && !ctrl->calKeyCycled)
            {
                DisplayPrompt( 2,  "CycleLossCompensationSwitch", false);
            }
            // else if not already prompted to cycle because conditions are not correct, prompt to cycle the key
            else if (!ctrl->calConditionsCorrect)
            {
                ctrl->calConditionsCorrect = true;
                DisplayPrompt( 2,  "CycleLossCompensationSwitch", true);
            }
            else if (ctrl->calCycle && !ctrl->calKeyCycled)
            {
                ctrl->calKeyCycled = true;
                // remove all prompts from second box
                DisplayPrompt(2, "Blank");
            }
            // if LC just completed
            else if (!ctrl->testInProgress && m_oldCtrl->testInProgress)
            {
                // if the retainers are up
                if (!(ctrl->rollsDown))
                {   // perform check for machine type
                    DisplayPrompt( 2,  "TurnLossCompensationSwitchOff", false);
                }
                // if the key is on
                else if (ctrl->calSwitchToPLC)
                {   // Prompt operator to cycle switch
                    DisplayPrompt( 2,  "TurnLossCompensationSwitchOff", true);
                }
            }
            // else if the rolls are still down and a test has not been started
            else if (ctrl->rollsDown && !m_oldCtrl->rollsDown)
            {
                DisplayPrompt( 2,  "TurnLossCompensationSwitchOff", true);
            }

            // Note: because the LC key drops out when cycled, this is after the
            // prompt box 2 decisions
            // If LC just started
            if (ctrl->calSwitchToPLC && !m_oldCtrl->calSwitchToPLC)
            {   // display the LC prompt
                DisplayPrompt( 1,  "LossCompensationInProgress", "white", true);
            }
        }
        // if LC just started
        else if (ctrl->testInProgress && !m_oldCtrl->testInProgress)
        {
            // remove the LC in-progress prompt
            RemovePrompt( 1,  "LossCompensationInProgress");
            // clear out the second box
            DisplayPrompt(2, "Blank");
        }
    }
    // if the rolls are down
    else if (ctrl->rollsDown)
    {
        // if they were just lowered
        if (!m_oldCtrl->rollsDown)
        {
            // Remove prompt telling operator to cycle switch
            RemovePrompt( 2,  "LowerRetainers");
        }
        RemovePrompt( 2,  "TurnLossCompensationSwitchOff");
        // reset the calibrate conditions when the rolls are lowered
        ctrl->calConditionsCorrect = false;
        ctrl->calKeyCycled = false;
    }

    Log( LOG_FN_ENTRY, "Exit IsuzuSystemMonitor::UpdateDriverMonitor()");
}

bool IsuzuSystemMonitor::AdjustWheelbase()
{
    // If the wheelbase is not in position, prompt operator that wheelbase is moving
    bool wheelbaseInPosition = false;
    ReadNdbData(WHEELBASE_IN_POSITION_TAG, wheelbaseInPosition);
    if (!wheelbaseInPosition)
    {
        DisplayPrompt(1, "WheelbaseIncorrect");

        WriteNdbData(WHEELBASE_MOVE_TAG, true);
        BposSleep(2000);
        WriteNdbData(WHEELBASE_MOVE_TAG, false);
        // Wait for the wheelbase to be in position
        INT16 wheelbaseDelay = 100;   // Wait up to 10 seconds for wheelbase in position
        do
        {   // Check if the wheelbase is in position
            BposSleep(100);
            ReadNdbData(WHEELBASE_IN_POSITION_TAG, wheelbaseInPosition);
            Log(LOG_DEV_DATA, "Wheelbase In Position: %s\n", wheelbaseInPosition ? "True" : "False");
        } while(!wheelbaseInPosition && wheelbaseDelay--);
        // Remove the prompt
        RemovePrompt(1, "WheelbaseIncorrect");
    }
    // Only continue if wheelbase is in position
    ReadNdbData(WHEELBASE_IN_POSITION_TAG, wheelbaseInPosition);
    return wheelbaseInPosition; 
}
