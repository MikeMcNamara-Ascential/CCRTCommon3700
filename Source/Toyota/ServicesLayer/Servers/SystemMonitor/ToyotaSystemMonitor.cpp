//*************************************************************************
// FILE DESCRIPTION:
//  The ToyotaSystemMonitor monitors the system to determine when a vehicle
//  test should start and what should be done once vehicle testing is complete.
//
//===========================================================================
// Copyright (c) 2009- Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
#include "ToyotaSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
ToyotaSystemMonitor::ToyotaSystemMonitor() : SystemMonitor(), m_buildDataLoaded(false)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
ToyotaSystemMonitor::ToyotaSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
ToyotaSystemMonitor::~ToyotaSystemMonitor()
{   // Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
const string ToyotaSystemMonitor::Publish(const XmlNode *node)
{   // Call the base class to start the publication handling
    Log(LOG_FN_ENTRY, "ToyotaSystemMonitor::Publish(tag: %s) - Enter", node->getName().c_str());
    // Check if a new vehicle type has been entered
    if(!node->getName().compare(VIN_READ_STATUS_TAG) && 
       !node->getValue().compare(VALID_VEHICLE_VIN) && (m_dataBroker != NULL))
    {   // New vehicle type has been entered, load the vehicle build data
        Log(LOG_DEV_DATA, "New vehicle type entered, commanding VDB to load build data");
        CommandNdbData(READ_LATEST_BUILD_DATA_TAG, "1");
        BposSleep(250);
        VehicleBuildDataLoaded(&buildDataLoaded);
    }
    else if(m_dataBroker == NULL)
    {   // Registration is not complete, cannot command NDB data
        Log(LOG_ERRORS, "INamedDataBroker not created yet!");
    }
    else if(!node->getName().compare(VIN_READ_STATUS_TAG) && node->getValue().compare(VALID_VEHICLE_VIN))
    {
        VehicleBuildDataLoaded(&buildDataNotLoaded);
    }
    else if(!node->getName().compare(GetDataTag("OverallTestTag")) && node->getValue().compare(BEP_TESTING_RESPONSE))
    {
        Log(LOG_DEV_DATA, "Test just completed, clear vehicle type");
        VehicleBuildDataLoaded(&buildDataNotLoaded);
        WriteNdbData(BODY_STYLE_TAG, GetDataTag("ClearBodyStyle"));
        if(!node->getValue().compare(BEP_PASS_RESPONSE))
        {
            WriteNdbData(RAISE_ROLLS, false);
        }
        else
        {   // Test did not pass, do not home the machine
            Log(LOG_ERRORS, "Test failed, not commanding machine to home state");
        }
    }
    else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && !node->getValue().compare("1") &&
            !ReadSubscribeData("OverallTest").compare(BEP_PASS_RESPONSE))
    {
        DisplayPrompt(2, "AdvanceOk");
        WriteNdbData(string("PromptBox2BGColor"), string("green"));
    }
    else if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && !node->getValue().compare("0"))
    {
        RemovePrompt(2, "AdvanceOk");
        WriteNdbData(string("PromptBox2BGColor"), string("white"));
    }
    else if(!node->getName().compare(GetDataTag("TestCompleteDataTag")) && !node->getValue().compare("1"))
    {   // Got the complete button press, determine if we need to abort or raise retainers
        if(m_oldCtrl->testInProgress)
        {   // Send the Abort tag
            BepServer::Write(ABORT_DATA_TAG, "1");
        }
        else
        {   // Test not in progress, raise the retainers
            VehicleBuildDataLoaded(&buildDataNotLoaded);
            WriteNdbData(BODY_STYLE_TAG, GetDataTag("ClearBodyStyle"));
            WriteNdbData(RAISE_ROLLS, false);
        }
    }
    // Return the result
    Log(LOG_FN_ENTRY, "ToyotaSystemMonitor::Publish(tag: %s) - Exit", node->getName().c_str());
    return SystemMonitor::Publish(node);
}

//-------------------------------------------------------------------------------------------------
void ToyotaSystemMonitor::CheckTesting(ControlData *ctrl)
{
    Log(LOG_FN_ENTRY, "ToyotaSystemMonitor::CheckTesting() - Enter");
    // Make sure there is not a machine fault that would prevent testing
    if(!ctrl->machineFault)
    {   // Check if a test is in progress
        Log(LOG_DEV_DATA, "No Machine Faults, check what we should do next");
        Log(LOG_DEV_DATA, "TestInProgress: %s, rollsDown: %s, vehiclePresent: %s, VehicleBuildDataLoaded: %s", (ctrl->testInProgress)?"true":"false", (ctrl->rollsDown)?"true":"false", (ctrl->vehiclePresent)?"true":"false", (VehicleBuildDataLoaded())?"true":"false");
        if(ctrl->testInProgress)
        {
            Log(LOG_DEV_DATA, "Test in progress, nothing to do right now");
        }
        // If a test just finished, clear the vehicle type display
        else if(!ctrl->testInProgress && m_oldCtrl->testInProgress)
        {
            Log(LOG_DEV_DATA, "Test just completed, lower rolls and clear vehicle type");
            WriteNdbData(BODY_STYLE_TAG, GetDataTag("ClearBodyStyle"));
        }
        // If no test in progress and rolls are "down" and vehicle present and we have a valid vehicle type
        else if(!ctrl->testInProgress && ctrl->rollsDown && ctrl->vehiclePresent && VehicleBuildDataLoaded())
        {   
            Log(LOG_DEV_DATA, "VehicleVinReadStatus: %s", ctrl->vehVinReadStatus.c_str());
            if(ctrl->vehVinReadStatus == VALID_VEHICLE_VIN)
            {   // Raise the retainers
                Log(LOG_DEV_DATA, "Vehicle present, rolls down and valid vehicle type - raising rolls and starting test");
                RemovePrompt(1, "EnterVehicleType");
                RemovePrompt(2, "AdvanceOk");
                WriteNdbData(string("PromptBox2BGColor"), string("white"));
                // Make sure wheelbase is in position
                StartWheelbaseAdjust();
                    BposSleep(1000);
                bool WBInPosition = false;
                ReadNdbData(WHEELBASE_IN_POSITION_TAG, WBInPosition);
                Log(LOG_DEV_DATA, "Checking WBPosition: %s",(WBInPosition)?"True":"False");
                if(!WBInPosition)
                {   //  Command the wheelbase to move
                    StartWheelbaseAdjust();
                    BposSleep(3000);
                }
                WriteNdbData(RAISE_ROLLS, true);
                // Start the test sequence
                CommandNdbData(START_VEHICLE_TEST_DATA_TAG, true);
                // Invalidate the build record status so we do not restart this same test
                WriteSubscribeData(VIN_READ_STATUS_TAG, INVALID_VEHICLE_VIN);
                ctrl->vehVinReadStatus = ReadSubscribeData(VIN_READ_STATUS_TAG);
            }
            else
            {   // No valid vehicle type, ask for one
                Log(LOG_DEV_DATA, "No vehicle type entered, ask for one");
                DisplayPrompt(1, "EnterVehicleType");
            }
        }
        // If vehicle present, but no build data loaded, instruct operator to select vehicle type
        else if(!ctrl->testInProgress && ctrl->rollsDown && ctrl->vehiclePresent && !VehicleBuildDataLoaded())
        {
            Log(LOG_DEV_DATA, "No vehicle type entered, ask for one");
            DisplayPrompt(1, "EnterVehicleType");
        }
    }
    else
    {
        Log(LOG_ERRORS, "Machine fault - cannot start test");
    }
    Log(LOG_FN_ENTRY, "ToyotaSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
void ToyotaSystemMonitor::CheckAbort( ControlData *ctrl)
{   // If there is a machine fault during the test, abort the test sequence
    static bool testingAborted = false;
    if(ctrl->testInProgress && ctrl->machineFault && !testingAborted)
    {
        Log(LOG_ERRORS, "Machine fault during test sequence, aborting the test");
        BepServer::Write(ABORT_DATA_TAG, "1");
        testingAborted = true;
    }
    else if(!ctrl->machineFault && testingAborted)
    {
        Log(LOG_DEV_DATA, "Machine faults cleared, resetting aborted flag in CheckAbort()");
        testingAborted = false;
    }
    // Call the base class to handle any other Aborts
    SystemMonitor::CheckAbort(ctrl);
}

//-------------------------------------------------------------------------------------------------
const bool& ToyotaSystemMonitor::VehicleBuildDataLoaded(const bool *loaded /*= NULL*/)
{
    if(loaded != NULL)  m_buildDataLoaded = *loaded;
    return m_buildDataLoaded;
}
