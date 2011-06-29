//******************************************************************************
//
// $Archive: /CCRT/Common/ServicesLayer/Servers/SystemMonitor/MercedesSystemMonitor.cpp $
// $Author: Derickso $
//   $Date: 3/21/05 11:14a $
//
// Description:
//	Mercedes System Monitor
//
//
//==============================================================================
//
//     Copyright © 2003 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CCRT/Common/ServicesLayer/Servers/SystemMonitor/MercedesSystemMonitor.cpp $
// 
// 3     3/21/05 11:14a Derickso
// Made checking of E-Stop buttons optional.  also made which E-Stop
// buttons to check configurable.
// 
// 2     2/22/05 4:03p Derickso
// Added feature to display prompts on the screen if an E-Stop button is
// pressed.
// 
// 1     12/08/04 4:19p Derickso
// Created new VSS database.
// 
// 2     11/01/04 3:30p Derickso
// Updated to allow loading of test components when a vehicle build is
// received.
// 
// 1     8/04/04 1:16p Derickso
// 
// 1     1/27/04 9:48a Derickso
// Core Release 04012302 plus latest DCX specific code.
// A few bug fixes have also been incorporated.
// 
// 4     10/20/03 10:11a Derickso
// Latest ServicesLayer update to correct for memory leaks and crashes.
// 
// 3     10/07/03 1:03p Derickso
// Synched up with Kevin's latest build. 
// Corrections for many lockups and abnormalities.
// 
// 2     9/24/03 7:01a Derickso
// Added CheckTesting method.
// 
// 2     9/05/03 1:38p Khykin
// Added the method to kick off a test.
// 
// 1     7/09/03 2:31p Rwiersem
// Initial version.
// 
// 1     7/03/03 5:20p Khykin
// Initial checkin
// 
// 1     6/27/03 8:21a Khykin
// Update from SysMon to SystemMonitor.
// 
// 1     6/20/03 9:55a Khykin
// Initial checkin.
// 
//
//******************************************************************************

/*******************
 include files
 ******************/
#include "MercedesSystemMonitor.h"

//////////////////////////
// Class Implementation //
//////////////////////////

/**
 * Default constructor. Set debug to false, channel ID's to -1, etc
 */
MercedesSystemMonitor::MercedesSystemMonitor() : SystemMonitor(), 
m_vehicleBuildRecordRead(false), m_testDataLoaded(false)
{
}

/**
 * Constructor from command line args
 *
 * @param argc   Number of command line arguments
 * @param argv   Array of command line args
 */
MercedesSystemMonitor::MercedesSystemMonitor( int argc, char *argv[]) : 
	SystemMonitor( argc, argv), m_vehicleBuildRecordRead(false), m_testDataLoaded(false)
{
}

/**
 * Class destructor
 */
MercedesSystemMonitor::~MercedesSystemMonitor()
{	// Nothing to do here
}

const string MercedesSystemMonitor::Register(void)
{
	string status = SystemMonitor::Register();
	return status;
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void MercedesSystemMonitor::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter Initialize()\n");

	// initialize the base SystemMonitor
	SystemMonitor::Initialize(document);
	// Store panel E-Stop information
	m_checkForEstop.reserve(ESTOP_BUTTON_COUNT);
	bool checkForEstop = false;
	try
	{
		SetPanelEStopPressedPromptTag(document->getChild("Setup/PanelEstopPrompt")->getValue());
		checkForEstop = (!document->getChild("Setup/CheckPanelEstop")->getValue().compare("Yes") ||
						 !document->getChild("Setup/CheckPanelEstop")->getValue().compare("True") ||
						 !document->getChild("Setup/CheckPanelEstop")->getValue().compare("1"));
	}
    catch (XmlException &excpt)
    {
		SetPanelEStopPressedPromptTag("PanelEstopPressed");
		checkForEstop = false;
		Log(LOG_ERRORS, "Could not find tag PanelEstopPrompt, using PanelEstopPressed: %s", excpt.GetReason());
	}
	CheckForEstop(CHECK_PANEL_ESTOP, &checkForEstop);
	// Store left pendant e-stop info
	try
	{
		SetLeftPendantEStopPressedPromptTag(document->getChild("Setup/LeftPendantEstopPrompt")->getValue());
		checkForEstop = (!document->getChild("Setup/CheckLeftPendantEstop")->getValue().compare("Yes") ||
						 !document->getChild("Setup/CheckLeftPendantEstop")->getValue().compare("True") ||
						 !document->getChild("Setup/CheckLeftPendantEstop")->getValue().compare("1"));
	}
    catch (XmlException &excpt)
    {
		SetLeftPendantEStopPressedPromptTag("LeftPendantEstopPressed");
		checkForEstop = false;
		Log(LOG_ERRORS, "Could not find tag LeftPendantEstopPrompt, using LeftPendantEstopPressed: %s", excpt.GetReason());
	}
	CheckForEstop(CHECK_LEFT_PENDANT_ESTOP, &checkForEstop);
	// Store right pendant e-stop info
	try
	{
		SetRightPendantEStopPressedPromptTag(document->getChild("Setup/RightPendantEstopPrompt")->getValue());
		checkForEstop = (!document->getChild("Setup/CheckRightPendantEstop")->getValue().compare("Yes") ||
						 !document->getChild("Setup/CheckRightPendantEstop")->getValue().compare("True") ||
						 !document->getChild("Setup/CheckRightPendantEstop")->getValue().compare("1"));
	}
    catch (XmlException &excpt)
    {
		SetRightPendantEStopPressedPromptTag("RightPendantEstopPressed");
		checkForEstop = false;
		Log(LOG_ERRORS, "Could not find tag RightPendantEstopPrompt, using RightPendantEstopPressed: %s", excpt.GetReason());
	}
	CheckForEstop(CHECK_RIGHT_PENDANT_ESTOP, &checkForEstop);
	// Store other e-stop info
	try
	{
		SetEStopPressedBackgroundColor(document->getChild("Setup/EstopPressedBackgroundColor")->getValue());
	}
    catch (XmlException &excpt)
    {
		SetEStopPressedBackgroundColor("yellow");
		Log(LOG_ERRORS, "Could not find tag EstopPressedBackgroundColor, using yellow: %s", excpt.GetReason());
	}
	try
	{
		SetEStopClearedBackgroundColor(document->getChild("Setup/EstopClearedBackgroundColor")->getValue());
	}
    catch (XmlException &excpt)
    {
		SetEStopClearedBackgroundColor("white");
		Log(LOG_ERRORS, "Could not find tag EstopClearedBackgroundColor, using white: %s", excpt.GetReason());
	}
	try
	{
		SetEStopPromptBox(BposReadInt(document->getChild("Setup/EStopPromptBox")->getValue().c_str()));
	}
    catch (XmlException &excpt)
    {
		Log(LOG_ERRORS, "Could not find prompt box, using 2: %s", excpt.GetReason());
		SetEStopPromptBox(2);
	}
	Log( LOG_FN_ENTRY, "Exit Initialize()\n");
}

const std::string MercedesSystemMonitor::Publish(const XmlNode *node)
{
    Log( LOG_FN_ENTRY, "Enter Publish()\n");

    std::string status = SystemMonitor::Publish(node);

    return status;
}
/**
 * Monitors for system events such as ABORT, CAL, SIM, START_TEST, etc
 */
void MercedesSystemMonitor::UpdateControl(ControlData *ctrl)
{

	Log( LOG_FN_ENTRY, "Enter MercedesSystemMonitor::UpdateControl()\n");
	// if the server is registered, update
	if (IsRegistered())
	{	
		// add any Mercedes specific processing here
		SystemMonitor::UpdateControl(ctrl);
	}

	Log( LOG_FN_ENTRY, "Exit MercedesSystemMonitor::UpdateControl()\n");
}

/**
 * Checking for any Estops being hit so they can be displayed to the driver
 * and we can abort the current test in progress
 *
 * @return bool value which reflects an estop being found/pressed
 */
bool MercedesSystemMonitor::CheckForAnyEstopPressed()
{
    bool eStopPressed = false;
    // Check if the panel E-stop button is pressed
    if (CheckForEstop(CHECK_PANEL_ESTOP))
    {
        if (ReadSubscribeData("PanelEStopPushbuttonDepressed") == "1")
        {   // Prompt the operator that the panel E-stop is pressed
            DisplayPrompt(GetEStopPromptBox(), GetPanelEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
            eStopPressed = true;
        }
        else
        {
            RemovePrompt(GetEStopPromptBox(), GetPanelEStopPressedPromptTag().c_str(), NULL);
        }
    }
    
    // Check if the left driver pendant E-stop button is pressed
    if (CheckForEstop(CHECK_LEFT_PENDANT_ESTOP))
    {
        if (ReadSubscribeData("StopPBDepressedLHPendant") == "1")
        {   // Prompt the operator that the LHD pendant E-stop is pressed
            DisplayPrompt(GetEStopPromptBox(), GetLeftPendantEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
            eStopPressed = true;
        }
        else
        {
            RemovePrompt(GetEStopPromptBox(), GetLeftPendantEStopPressedPromptTag().c_str(), NULL);
        }
    }
    // Check if the right driver pendant E-stop button is pressed
    if (CheckForEstop(CHECK_RIGHT_PENDANT_ESTOP))
    {
        if (ReadSubscribeData("StopPBDepressedRHPendant") == "1")
        {   // Prompt the operator that the RHD pendant E-stop is pressed
            DisplayPrompt(GetEStopPromptBox(), GetRightPendantEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
            eStopPressed = true;
        }
        else
        {
            RemovePrompt(GetEStopPromptBox(), GetRightPendantEStopPressedPromptTag().c_str(), NULL);
        }
    }
   
    if (eStopPressed == true)
    {
        //One of the ESTOPS must of been hit so lets abort everything
        string response;
        m_dataBroker->Write(ABORT_DATA_TAG, "1", response, true);
    }
    return eStopPressed;
}
/**
 * Testing control logic
 *
 * @param ctrl Current value of control data needed for system control events
 */
void MercedesSystemMonitor::CheckTesting( ControlData *ctrl)
{
	Log( LOG_FN_ENTRY, "Enter CheckTesting()\n");
	Log( LOG_DEV_DATA, "\tstartTestPb %d\n", ctrl->startTestPb);
	Log( LOG_DEV_DATA, "\tkeyPress %s\n", ctrl->keyPress.c_str());
	Log( LOG_DEV_DATA, "\tvehiclePresent %d\n", ctrl->vehiclePresent);
	Log( LOG_DEV_DATA, "\tvin %s\n", ctrl->vehVinReadStatus.c_str());
	Log( LOG_DEV_DATA, "\ttestInProgress %d, %d\n", ctrl->testInProgress, m_oldCtrl->testInProgress);
	Log( LOG_DEV_DATA, "\tcalSwitch %d\n", ctrl->calSwitch);
	Log( LOG_DEV_DATA, "\tsimSwitch %d\n", ctrl->simSwitch);
	Log( LOG_DEV_DATA, "\tpowerSense %d\n", ctrl->powerSense);
	Log( LOG_DEV_DATA, "\trackFault %d\n", ctrl->rackFault);
	Log( LOG_DEV_DATA, "\trollsDown %d\n", ctrl->rollsDown);


	// if there is not a machine fault
	if (!ctrl->machineFault)
	{
		// if a test is in progress, make sure no prompting occurs from the SystemMonitor
        //except when an E-Stop is Pressed
		if (ctrl->testInProgress)
		{
            Log(LOG_DEV_DATA, "Nothing To Do except check for E-Stop Pressed, Test Is In Progress\n");
            if (CheckForAnyEstopPressed())
            {
                //One of the ESTOPS must of been hit
                Log(LOG_DEV_DATA, "E-Stop Pressed, Sent an Abort to NDB!\n");
            }
        }
		// else if a test is just finished, check the cable connect
		else if ((!(ctrl->testInProgress) && m_oldCtrl->testInProgress))
		{	// if the cable is still connected, prompt to operator to remove the cable
			Log(LOG_DEV_DATA, "Test just finished, checking Cable Connect\n");
			if (ctrl->cableConnect)
			{
				DisplayPrompt(2, "DisconnectCable");
			}
			// else if the rolls are not down prompt the operator to lower the retainers
			else if (!ctrl->rollsDown)
			{
				if (m_machineType == "2400")
					DisplayPrompt( 2,  "RaiseElevators", true);
				else
					DisplayPrompt( 2,  "LowerRetainers", true);
			}
			SetVehicleBuildRecordRead(false);
			SetLoadVehicleTestData(false);
			CommandNdbData(REMOVE_VEHICLE_TEST_DATA_TAG, "1");
		}
		// if the rolls are raised, and the cable was just disconnected
		else if (!ctrl->cableConnect && m_oldCtrl->cableConnect)
		{
			Log(LOG_DEV_DATA, "Rolls are up, cable was just disconnected\n");
			RemovePrompt(2, "DisconnectCable");
			if (!ctrl->rollsDown)
			{
				if (m_machineType == "2400")
					DisplayPrompt( 2,  "RaiseElevators", true);
				else
					DisplayPrompt( 2,  "LowerRetainers", true);
			}
		}
		// if the cal and sim switches are off and not in the middle of a test and the rolls are down
		else if (!(ctrl->calSwitch) && !(ctrl->simSwitch) && !(ctrl->testInProgress) && ctrl->rollsDown)
		{
			Log(LOG_DEV_DATA, "Cal switch off, Sim switch off, No test in progress and rolls down\n");
			// if the retainers were just lowered, 
			if ((ctrl->rollsDown) && !(m_oldCtrl->rollsDown))
			{
				if (m_machineType == "2400")
					RemovePrompt( 2,  "RaiseElevators");
				else
					RemovePrompt( 2,  "LowerRetainers");
			}
			// if vehicle present and a new vin scanned and we are ready for testing
			if ((ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) && (m_oldCtrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
				(ctrl->vehiclePresent) && (ctrl->rollsDown) && (!ctrl->startTestPb) && (!m_oldCtrl->startTestPb))
			{
				DisplayPrompt( 1,  "PressButtonToStartTest", "white");
				Log(LOG_DEV_DATA, "Vehicle present and a new vin scanned");
				// Command the VehicleDataBroker to get the vehicle build
				if (!VehicleBuildRecordRead())
				{   
					Log(LOG_DEV_DATA, "Vehicle present and build record ready -- Commanding the vehicle build to be read\n");
					CommandNdbData( "ReadLatestBuildData", "1");
	                // Set vehicle build record read flag
	                SetVehicleBuildRecordRead(true);
				}
				// Load the vehicle test data
				LoadTestData(false);
			}
			// if vehicle just arrived and valid VIN and ready to test
			else if ((ctrl->vehiclePresent)  && !(m_oldCtrl->vehiclePresent) && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) &&
					 (!ctrl->startTestPb) && (!m_oldCtrl->startTestPb))
			{
				Log(LOG_DEV_DATA, "Vehicle just arrived and valid vin");
				DisplayPrompt( 1,  "PressButtonToStartTest", "white");
				// Command the VehicleDataBroker to get the vehicle build
				if (!VehicleBuildRecordRead())
				{   
					Log(LOG_DEV_DATA, "Vehicle just arrived and build record ready -- Commanding the vehicle build to be read\n");
					CommandNdbData( "ReadLatestBuildData", "1");
	                // Set vehicle build record read flag
	                SetVehicleBuildRecordRead(true);
				}
				// Load the vehicle test data
				LoadTestData(true);
			}
			// if vehicle arrived and no valid VIN
			else if ((ctrl->vehiclePresent) && (!m_oldCtrl->vehiclePresent) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
					 (!ctrl->startTestPb) && (!m_oldCtrl->startTestPb))
			{
				Log(LOG_DEV_DATA, "Vehicle arrived and no valid vin");
				DisplayPrompt( 1,  "EnterVIN", "white");	// prompt the driver to enter the VIN
			}
			// If no vehicle and valid vin and ready to test
			else if ((!ctrl->vehiclePresent) && (!m_oldCtrl->vehiclePresent) && (!ctrl->startTestPb) && 
					 (!m_oldCtrl->startTestPb) && (ctrl->vehVinReadStatus == VALID_VEHICLE_VIN))
			{   // Command the VehicleDataBroker to get the vehicle build
				Log(LOG_DEV_DATA, "No vehicle and valid vin");
				if (!VehicleBuildRecordRead())
				{   
					Log(LOG_DEV_DATA, "No Vehicle present and build record ready -- Commanding the vehicle build to be read\n");
					CommandNdbData( "ReadLatestBuildData", "1");
	                // Set vehicle build record read flag
	                SetVehicleBuildRecordRead(true);
	         	}
				// Load the vehicle test data
				LoadTestData(true);
			}
			// if vehicle just left and valid VIN and ready to test
			else if (!(ctrl->vehiclePresent) && (m_oldCtrl->vehiclePresent) && (!ctrl->startTestPb) && (!m_oldCtrl->startTestPb))
			{
				Log(LOG_DEV_DATA, "Vehicle just left and valid VIN");
				RemovePrompt( 1,  "PressButtonToStartTest");	// remove the press the start button prompt
				RemovePrompt( 1,  "EnterVIN");					// remove the driver to enter the VIN prompt
//				SetVehicleBuildRecordRead(false);
//				SetLoadVehicleTestData(false);
			}
			// Second VIN scanned because previous vehicle scanned and did a drive through
			else if ((ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) && (m_oldCtrl->vehVinReadStatus == VALID_VEHICLE_VIN))
			{
				Log(LOG_DEV_DATA, "Old vin good, new vin scanned");
				SetVehicleBuildRecordRead(false);
				SetLoadVehicleTestData(false);
				CommandNdbData(REMOVE_VEHICLE_TEST_DATA_TAG, "1");
			}
			// If start test signal and vehicle present and valid vin and not cal or sim or vehicle test and the rolls are down
			else if (((ctrl->startTestPb) || (ctrl->keyPress == PENDANT_START_TEST)) && 
					 (ctrl->vehiclePresent) && (ctrl->rollsDown) && 
					 ((ctrl->vehVinReadStatus == VALID_VEHICLE_VIN) || VehicleBuildRecordRead()))
			{
				// publish the start of test to the subscribers
				if ((ctrl->keyPress == PENDANT_START_TEST) && (BepServer::Read(START_VEHICLE_TEST_DATA_TAG) != "1"))
				{
                    Log( LOG_DEV_DATA, "Telling Vehicle Data Broker to pre-load vehicle build before publishing %s\n",
						 START_VEHICLE_TEST_DATA_TAG);
					// Tell vehicle data broker to read the latest build data in preperation
					//	of a test start if it has not already been read
					if (!VehicleBuildRecordRead()) CommandNdbData( "ReadLatestBuildData", "1");
					// Load the vehicle test data
					LoadTestData(true);
					// remove the press key prompt
					RemovePrompt( 1,  "PressButtonToStartTest");
					// Wait for the wheelbase to be in position
					bool wheelbaseInPosition = false;
					INT16 wheelbaseDelay = 100;   // Wait up to 10 seconds for wheelbase in position
					do
					{   // Check if the wheelbase is in position
						BposSleep(100);
						ReadNdbData(WHEELBASE_IN_POSITION_TAG, wheelbaseInPosition);
						Log(LOG_DEV_DATA, "Wheelbase In Position: %s\n", wheelbaseInPosition ? "True" : "False");
					} while (!wheelbaseInPosition && wheelbaseDelay--);
					// Tell the whelbase to stop moving
					WriteNdbData(WHEELBASE_MOVE_TAG, false);
					// if the retainers are down raise them
					bool status = ctrl->rollsDown;
					if (status)
					{	// raise the retainers and delay so system sees them raised
						// do not remove the BposSleep!
						WriteNdbData(RAISE_ROLLS, true);
						INT32 raiseAttempts = 300;
						bool rollsDown = true;
						// Wait for the rolls to come up or elevators to go down
						while (!ReadNdbData(ROLLS_UP_DATA_TAG, rollsDown) && raiseAttempts--)  BposSleep(100);
					}
					// Start the vehicle test
                    CommandNdbData(START_VEHICLE_TEST_DATA_TAG, "1");
                    // Reset vehicle build record read flag
                    SetVehicleBuildRecordRead(false);
					SetLoadVehicleTestData(false);
					Log( LOG_DEV_DATA, "SYSTEM MONITOR STARTING A TEST! Issuing %s\n", START_VEHICLE_TEST_DATA_TAG);
					// if the command to raise was sent, clear the flag
					if (status)	 WriteNdbData(RAISE_ROLLS, false);
				}
				else if (ctrl->startTestPb && (BepServer::Read(COMMAND_TEST) == TEST_BASE_BRAKE))
				{
					// if the retainers are down raise them
					bool status = ctrl->rollsDown;
					if (status)
					{	// raise the retainers and delay so system sees them raised
						// Wait for the wheelbase to be in position
						bool wheelbaseInPosition = false;
						INT16 wheelbaseDelay = 100;   // Wait up to 10 seconds for wheelbase in position
						do
						{   // Check if the wheelbase is in position
							BposSleep(100);
							ReadNdbData(WHEELBASE_IN_POSITION_TAG, wheelbaseInPosition);
							Log(LOG_DEV_DATA, "Wheelbase In Position: %s\n", wheelbaseInPosition ? "True" : "False");
						} while (!wheelbaseInPosition && wheelbaseDelay--);
						// COmmand the wheelbase to stop moving
						WriteNdbData(WHEELBASE_MOVE_TAG, false);
						// do not remove the BposSleep!
						WriteNdbData(RAISE_ROLLS, true);
						INT32 raiseAttempts = 50;
						bool rollsDown = true;
						// Wait for the rolls to come up or elevators to go down
						while (!ReadNdbData(ROLLS_UP_DATA_TAG, rollsDown) && raiseAttempts--)  BposSleep(100);
					}
					string response;
                    if ( (errno=m_ndbLock.Acquire()) == EOK)
                    {
						m_dataBroker->Command( COMMAND_TEST, TEST_BASE_BRAKE, response, true);
						Log( LOG_DEV_DATA, "SYSTEM MONITOR STARTING A BASE BRAKE TEST! Issuing %s, %s\n", 
							 COMMAND_TEST, TEST_BASE_BRAKE);
						// remove the press key prompt
						RemovePrompt( 1,  "PressButtonToStartTest");
						// if the command to raise was sent, clear the flag
						if (status)	 WriteNdbData(RAISE_ROLLS, false);

						m_ndbLock.Release();
					}
					else
					{
						Log( LOG_ERRORS, "\tError locking ndb mutex: %s\n", strerror( errno));
					}
				}
				else
					Log( LOG_DEV_DATA, "%s Previously Issued\n", START_VEHICLE_TEST_DATA_TAG);
			}
			// If start test and vehicle present and no new vin scanned and not cal or sim or vehicle test
			else if (((ctrl->startTestPb) || (ctrl->keyPress == PENDANT_START_TEST)) && 
					 (ctrl->vehiclePresent) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))
			{
				// if the retainers are not raised and new vehicle arives
				if ((ctrl->inputServerState == INPUT_SERVER_VIN_STATE) && (m_oldCtrl->inputServerState != INPUT_SERVER_VIN_STATE) &&
					(ctrl->vehiclePresent  && !m_oldCtrl->vehiclePresent))
				{
					DisplayPrompt( 1,  "EnterVIN", "white");	// prompt the driver to enter the VIN
				}
			}
			// If not a start test and vehicle present and a no new vin scanned and not cal or sim or vehicle test
			else if (!((ctrl->startTestPb) || (ctrl->keyPress == PENDANT_START_TEST)) && 
					 (ctrl->vehiclePresent) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN))
			{
				// ensure that the vin prompt is removed
				if ((ctrl->inputServerState != INPUT_SERVER_VIN_STATE) && (m_oldCtrl->inputServerState == INPUT_SERVER_VIN_STATE))
					RemovePrompt( 1,  "EnterVIN");
			}
		}

		// If test just finished or retainers lowered, clear the start test tag
		if ((!(ctrl->testInProgress) && m_oldCtrl->testInProgress) || ((ctrl->rollsDown) && !(m_oldCtrl->rollsDown)))
		{
			// if test started, stop
			if (BepServer::Read(START_VEHICLE_TEST_DATA_TAG) != "0")
			{	// clear the start test tag
				BepServer::Write( START_VEHICLE_TEST_DATA_TAG, "0");
				Log( LOG_DEV_DATA, "Clearing %s\n", START_VEHICLE_TEST_DATA_TAG);
			}
		}
	}
	else
		Log( LOG_ERRORS, "Machine Fault, Cant Do JACK SQUAT\n");

	Log( LOG_FN_ENTRY, "Exit CheckTesting()\n");
}

void MercedesSystemMonitor::GetCurrentControlData( ControlData *ctrl)
{
	Log( LOG_FN_ENTRY, "Enter DCXSysMon::GetCurrentControlData()\n");

	// Read latest values of subscribed data
	ctrl->vehVinReadStatus = ReadSubscribeData(VIN_READ_STATUS_TAG);
	ctrl->startTestPb = (bool)(ReadSubscribeData(START_TEST_REQ_DATA_TAG) == "1");
	ctrl->testInProgress = (bool)(ReadSubscribeData( TEST_IN_PROG_DATA_TAG) == "1");
	ctrl->vehiclePresent = (bool)(ReadSubscribeData( INST_VEH_PRESENT_DATA_TAG) == "1");
	ctrl->rollsDown = (bool)(ReadSubscribeData( ROLLS_DOWN_DATA_TAG) == "1");
	ctrl->rollsUp = (bool)(ReadSubscribeData( ROLLS_UP_DATA_TAG) == "1");
	ctrl->calCycle = (bool)(ReadSubscribeData( LOSSCOMPENSATION_CYCLE_FROM_PLC_TAG) == "1");
	ctrl->calSwitch = (bool)(ReadSubscribeData( LC_SWITCH_STATUS_FROM_PLC_TAG) == "1");
	ctrl->calSwitchToPLC= (bool)(ReadSubscribeData( LC_SWITCHTOPLC_STATUS_TAG) == "1");
	ctrl->calInProgress = (bool)(ReadSubscribeData( START_LOSSCOMPENSATION_DATA_TAG) == "1");
	ctrl->simSwitch = (bool)(ReadSubscribeData( SIM_SWITCH_DATA_TAG) == "1");
	ctrl->simCycle = (bool)(ReadSubscribeData( SIM_CYL_DATA_TAG) == "1");
	ctrl->inputServerState = ReadSubscribeData( INPUT_SERVER_STATE);
	ctrl->powerSense = (bool)(ReadSubscribeData( PLC_POWER_SENSE) == "1");
	ctrl->rackFault = (bool)(ReadSubscribeData( PLC_RACK_FAULT) == "1");
	ctrl->wheelbaseInPosition = (bool) (ReadSubscribeData( WHEELBASE_IN_POSITION_TAG) == "1");
	ctrl->vmeSystemReady = (bool) (ReadSubscribeData( VME_SYSTEM_READY_TAG) == "1");
	ctrl->cableConnect = (bool)(ReadSubscribeData(PLC_CBL_CONNECT) == "1");

	// check to determine if a reprint request was received
	// test must not be in progress, the rolls must be down and the current status
	// of the keypress must be F1
    if (!ctrl->testInProgress && ctrl->rollsDown && (ctrl->keyPress == "FunctionKey1"))
    {
		Log(LOG_DEV_DATA, "Request For Reprinting Received\n");
		ctrl->reprintRequest = true;
	}
	else
	{
		ctrl->reprintRequest = false;		
	}
	// If a test was just started and a valid vin does not exist, check the
	// ValidVINNeededForStart flag.  If it is not set, and the rolls are raised
	// indicate a valid vin is avaliable so the test can be started
	if ( (ctrl->startTestPb) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
		 (!m_validVINNeededForStart) && (ctrl->rollsDown) )
	{
		ctrl->vehVinReadStatus = VALID_VEHICLE_VIN;
	}
	// read the pendant keypress value and then clear it out of the internal buffer 
	// so if same key is pressed again, is is processed like new keypress
	ctrl->keyPress = ReadSubscribeData( PENDANT_KEY_PRESS);
	WriteSubscribeData(PENDANT_KEY_PRESS, std::string("Blank"));

	// Check if the panel E-stop button is pressed
	if (CheckForEstop(CHECK_PANEL_ESTOP))
	{
		if (ReadSubscribeData("PanelEStopPushbuttonDepressed") == "1")
		{   // Prompt the operator that the panel E-stop is pressed
			DisplayPrompt(GetEStopPromptBox(), GetPanelEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
		}
		else
		{
			RemovePrompt(GetEStopPromptBox(), GetPanelEStopPressedPromptTag().c_str(), GetEStopClearedBackgroundColor().c_str());
		}
	}
	// Check if the left driver pendant E-stop button is pressed
	if (CheckForEstop(CHECK_LEFT_PENDANT_ESTOP))
	{
		if (ReadSubscribeData("StopPBDepressedLHPendant") == "1")
		{   // Prompt the operator that the LHD pendant E-stop is pressed
			DisplayPrompt(GetEStopPromptBox(), GetLeftPendantEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
		}
		else
		{
			RemovePrompt(GetEStopPromptBox(), GetLeftPendantEStopPressedPromptTag().c_str(), GetEStopClearedBackgroundColor().c_str());
		}
	}
	// Check if the right driver pendant E-stop button is pressed
	if (CheckForEstop(CHECK_RIGHT_PENDANT_ESTOP))
	{
		if (ReadSubscribeData("StopPBDepressedRHPendant") == "1")
		{   // Prompt the operator that the RHD pendant E-stop is pressed
			DisplayPrompt(GetEStopPromptBox(), GetRightPendantEStopPressedPromptTag().c_str(), GetEStopPressedBackgroundColor().c_str(), true);
		}
		else
		{
			RemovePrompt(GetEStopPromptBox(), GetRightPendantEStopPressedPromptTag().c_str(), GetEStopClearedBackgroundColor().c_str());
		}
	}
	// default to the previous status
	ctrl->simInProgress = m_oldCtrl->simInProgress;
	ctrl->testRequested = m_oldCtrl->testRequested;
	ctrl->calKeyCycled = m_oldCtrl->calKeyCycled;
	ctrl->calConditionsCorrect = m_oldCtrl->calConditionsCorrect;

	Log( LOG_FN_ENTRY, "Exit GetCurrentControlData()\n");
}

void MercedesSystemMonitor::SetVehicleBuildRecordRead(const bool& readStatus)
{
	m_vehicleBuildRecordRead = readStatus;
}

const bool& MercedesSystemMonitor::VehicleBuildRecordRead(void)
{
	return m_vehicleBuildRecordRead;
}

void MercedesSystemMonitor::SetLoadVehicleTestData(const bool &testDataLoaded)
{
	m_testDataLoaded = testDataLoaded;
}

bool MercedesSystemMonitor::IsVehicleTestDataLoaded(void)
{
	return(m_testDataLoaded);
}

inline const string& MercedesSystemMonitor::GetEStopPressedBackgroundColor(void)
{
	return m_eStopPressedColor;
}

inline const string& MercedesSystemMonitor::GetEStopClearedBackgroundColor(void)
{
	return m_eStopClearedColor;
}

inline const string& MercedesSystemMonitor::GetPanelEStopPressedPromptTag(void)
{
	return m_panelEstopPrompt;
}

inline const string& MercedesSystemMonitor::GetLeftPendantEStopPressedPromptTag(void)
{
	return m_leftPendantEstopPrompt;
}

inline const string& MercedesSystemMonitor::GetRightPendantEStopPressedPromptTag(void)
{
	return m_rightPendantEstopPrompt;
}

inline const INT32& MercedesSystemMonitor::GetEStopPromptBox(void)
{
	return m_estopPromptBox;
}

const bool MercedesSystemMonitor::CheckForEstop(BEP_CHECK_ESTOP estopButton, const bool *checkForEstop /*= NULL*/)
{
	if (checkForEstop)
	{
		m_checkForEstop[estopButton] = *checkForEstop;
	}
	return m_checkForEstop[estopButton];
}

inline void MercedesSystemMonitor::SetEStopPressedBackgroundColor(const string &color)
{
	m_eStopPressedColor = color;
}

inline void MercedesSystemMonitor::SetEStopClearedBackgroundColor(const string &color)
{
	m_eStopClearedColor = color;
}

inline void MercedesSystemMonitor::SetPanelEStopPressedPromptTag(const string &tag)
{
	m_panelEstopPrompt = tag;
}

inline void MercedesSystemMonitor::SetLeftPendantEStopPressedPromptTag(const string &tag)
{
	m_leftPendantEstopPrompt = tag;
}

inline void MercedesSystemMonitor::SetRightPendantEStopPressedPromptTag(const string &tag)
{
	m_rightPendantEstopPrompt = tag;
}

inline void MercedesSystemMonitor::SetEStopPromptBox(const INT32 &promptBox)
{
	m_estopPromptBox = promptBox;
}

inline void MercedesSystemMonitor::LoadTestData(bool checkIfDataLoaded /*= true*/)
{
	if ((!IsVehicleTestDataLoaded() && checkIfDataLoaded) || !checkIfDataLoaded)
	{
		// Load vehicle test component data
		CommandNdbData(LOAD_VEHICLE_TEST_DATA_TAG, "1");
		// Set load vehicle test data flag to true
		SetLoadVehicleTestData(true);
		// Command the PLC to move the wheelbase
		WriteNdbData(WHEELBASE_MOVE_TAG, true);
	}
}

/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
	
#ifdef __USAGE

Mercedes System Monitor Task

%C [-c/-e][-d] [-ffile] [-py] [-vx] [-r] &

	-c        Enable echo of logger messages to local console
	-d        Run in demo mode
	-e        Enable echo of logger messages to local console
	-ffile    Read config from 'file'
	-py       Run at priority 'y'
	-vx       Use verbose mask 'x' (in hex, see notes)
	-r        Don't wait for Register pulse

	Process Dependencies:
	  Required:
		NamedDataBroker.qxe (needed to read data tags in the system)
		PromptServer.qxe    (needed for prompt display)
		FaultServer.qxe     (fault reporting)
	  Optional:
		PlcMgr.qxe          (needed to supply machine I/O data)
		DioMgr.qxe          (needed to supply machine I/O data)
	
	Notes:
	-Verbose Mask bit definitions (leading 0s optional):
		00000000: No Log Messages
		00000001: Log Errors
		00000002: Log Function Entry/Exit
		00000004: Log I/O message data
		00000008: Log device general messages
		FFFFFFFF: Log everything
	- Registers name as 'rolls/MercedesSystemMonitor' by default
	- Must be run as Super User.
	- $Revision: 7 $
	- $Date: 9/08/05 10:59a $

#endif

*******************************************************************************/



