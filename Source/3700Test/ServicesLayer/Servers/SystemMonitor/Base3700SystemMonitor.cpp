//******************************************************************************
//
// $Archive: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/Base3700SystemMonitor.cpp $
// $Author: Mmcnamar $
//   $Date: 7/12/07 12:34p $
//
// Description:
//  Base 3700 System Monitor Task
//
//
//==============================================================================
//
//     Copyright © 2009- Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/Base3700SystemMonitor.cpp $
//           Originally Copied from Tesla install
//
//******************************************************************************
/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
	
#ifdef __USAGE

Base 3700 System Monitor Task

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
	- Registers name as 'rolls/Base3700SystemMonitor' by default
	- Must be run as Super User.
	- $Revision: 8 $
	- $Date: 7/12/07 12:34p $

#endif

*******************************************************************************/

/*******************
 include files
 ******************/
#include "Base3700SystemMonitor.h"

#include <fcntl.h>	// O_RDONLY
//////////////////////////
// Class Implementation //
//////////////////////////

/**
 * Default constructor. Set debug to false, channel ID's to -1, etc
 */
Base3700SystemMonitor::Base3700SystemMonitor() : SystemMonitor(), 
	m_ICMSpeedLocation( "/dev/Icm/Block/Speed"), m_rollerCount( 4), m_currentDriveAxle(""), 
	m_testsBetweenLossComp( 0), m_startTestArmed(false)
{
}

/**
 * Constructor from command line args
 *
 * @param argc   Number of command line arguments
 * @param argv   Array of command line args
 */
Base3700SystemMonitor::Base3700SystemMonitor( int argc, char *argv[]) : SystemMonitor( argc, argv),
	m_ICMSpeedLocation( "/dev/Icm/Block/Speed"), m_rollerCount( 4), m_currentDriveAxle(""),
	m_testsBetweenLossComp(0)
{
}

/**
 * Class destructor
 */
Base3700SystemMonitor::~Base3700SystemMonitor()
{
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void Base3700SystemMonitor::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter Base3700SystemMonitor::Initialize()\n");

	// initialize the base BepServer
	SystemMonitor::Initialize(document);
	
    // get the ICM speed location
    try
    {                                  // Read the valid vin needed for start flag
        if(document)
        {
            const XmlNode *speedLocation = document->getChild(XML_T("Setup/ICMWheelSpeeds"));
            m_ICMSpeedLocation = speedLocation->getValue();
        }
        else
        {                              // default location
            m_ICMSpeedLocation = "/dev/Icm/Block/Speed";
        }
    }
    catch(XmlException &err)
    {                                  // If no flag specified, default location
        m_ICMSpeedLocation = "/dev/Icm/Block/Speed";
    }

    // get the roller counts
    try
    {
        if(document)
        {
            const XmlNode *rollerCount = document->getChild(XML_T("Setup/RollerCount"));
            // if a tandem machine
            if(rollerCount->getValue() == "6")      SetRollerCount(6);
            else                                    SetRollerCount(4);

        }
        else
        {
            SetRollerCount(4);
        }
    }
    catch(XmlException &err)
    {
        SetRollerCount(4);
    }

	try
	{
		if(document)
		{
			const XmlNode *testCount = document->getChild(XML_T("Setup/LossCompensationCycleLimit"));
			m_testsBetweenLossComp = atoi( testCount->getValue().c_str());
		}
		else
		{
			m_testsBetweenLossComp = 0;
		}
	}
	catch(XmlException &err)
	{
		m_testsBetweenLossComp = 0;
	}

	Log( LOG_FN_ENTRY, "Exit Base3700SystemMonitor::Initialize()\n");
}

/**
 * Request to write a data tag
 *
 * @param tag    Tag to be written
 * @param value  Data to be written
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Base3700SystemMonitor::Write(const std::string &tag, const std::string &value)
{
    XmlNode node(tag, value);
    return(Write(&node));
}

/**
 * Request to write a data tag
 *
 * @param node    Data to be written
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Base3700SystemMonitor::Write(const XmlNode *node)
{
    std::string status;

    // Due, cant write a start test, trap the write and return an error
    if(node->getName() == START_TEST_DATA_TAG)
    {
        status = BEP_ERROR_RESPONSE;
    }
    else
        status = SystemMonitor::Write(node);

    return(status);
}


/**
 * Request to Perform an action
 *
 * @param tag    Action to be performed
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Base3700SystemMonitor::Command(const std::string &tag, const std::string &value)
{
    XmlNode node(tag, value, NULL, ELEMENT_NODE);
    return(Command(&node));
}

/**
 * Request to Perform an action
 *
 * @param node    Action to be performed
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 * @since Version 2.0
 */
const std::string Base3700SystemMonitor::Command(const XmlNode *node)
{
	Log( LOG_FN_ENTRY, "Enter Command(%s)\n", node->ToString().c_str());
    
    std::string status = BEP_UNSUPPORTED_RESPONSE;

    // if a start test is issued by the PlantHostInbound
    if(node->getName() == START_TEST_DATA_TAG)
    {
        status = BEP_SUCCESS_RESPONSE;
        SetData( START_TEST_DATA_TAG, node->getValue());
    }
    // else handle it normally
    else
        status = SystemMonitor::Command(node);
    
    // if a successful publish update the system
	if (status == BEP_SUCCESS_RESPONSE)
		NotifyDataChanged();

	Log( LOG_FN_ENTRY, "Exit Command() %s\n", status.c_str());

    return(status);
}

/**
 * Monitors for system events such as ABORT, CAL, SIM, START_TEST, etc
 */
void Base3700SystemMonitor::UpdateControl(ControlData *ctrl)
{
	Log( LOG_FN_ENTRY, "Enter base700 UpdateControl()\n");
	
	// if the server is registered, update
	if (IsRegistered())
	{	
		// Sample current values (must be done first)
		CheckOkToRoll(ctrl);
		// Check the machine status to determine if it is good (should be done second)
		CheckMachineStatus( ctrl);
		// Check status of Vehicle present
		CheckVehiclePresent( ctrl);
		// Check cable connect status change
		CheckCableConnect( ctrl);
		// Check ABORT logic
		CheckAbort( ctrl);
		// Check test start logic
		CheckTesting( ctrl);
        // Pass DVT commands to vehicle test
		UpdateCommandPhase( ctrl);
		// Check calibrate logic
		CheckLossCompensation( ctrl);
		// Update control related prompts
		UpdateDriverMonitor( ctrl);
		//Check for "rolling road" and update drive axle
//		UpdateDriveAxle( ctrl);
		// Check for reprint request
		CheckReprintRequest( ctrl);
		// Update data
		*m_oldCtrl = *ctrl;
	}

	Log( LOG_FN_ENTRY, "Exit UpdateControl()\n");
}

//-----------------------------------------------------------------------------
const std::string Base3700SystemMonitor::Publish(const XmlNode *node)
{
    if(!node->getName().compare(ROLLS_DOWN_DATA_TAG) && atob(node->getValue().c_str()))
    {
        m_startTestArmed = true;
        Log(LOG_DEV_DATA, "Publish - start test armed: %s", m_startTestArmed ? "True" : "False");
    }
    // Call the base class to handle the publish
    return SystemMonitor::Publish(node);
}

/**
 * Reads current values of control data items. The m_subscribedData data is
 * updated asynchronously via subscription notifications from the servers to
 * whoim we are subscribed
 *
 * @param ctrl   Control Data structure to fill with values
 */
void Base3700SystemMonitor::GetCurrentControlData( ControlData *ctrl)
{
	Base3700ControlData *Base3700Ctrl = (Base3700ControlData *)ctrl;
	Log( LOG_FN_ENTRY, "Enter Base3700 GetCurrentControlData()\n");

	SystemMonitor::GetCurrentControlData( ctrl);
	
	///////////////////////
	// Tesla Additions

	ctrl->startTestCommand = (bool)(ReadSubscribeData(START_TEST_DATA_TAG) == "1");
	ctrl->cableConnect = (bool)(ReadSubscribeData( PLC_CBL_CONNECT) == "1");
	ctrl->zerospeed = (bool)(ReadSubscribeData( ZEROSPEED_TAG) == "1");
	Base3700Ctrl->okToRoll = (bool) (ReadSubscribeData(OKTOROLL_TAG) == "1");
	Base3700Ctrl->testsSinceLossComp = atoi( ReadSubscribeData( TESTS_SINCE_LOSS_COMP_TAG).c_str());
	Base3700Ctrl->cycleLimitExceeded = (bool) (ReadSubscribeData( GetDataTag("CycleLimitExceeded")) == "1");
	///////////////////////

	Log( LOG_FN_ENTRY, "Exit Base3700 GetCurrentControlData()\n");
}

//-----------------------------------------------------------------------------
const string Base3700SystemMonitor::Register(void)
{   // Call the base class to begin the registration
    string result(SystemMonitor::Register());
    // Determine if a test should be armed
    m_startTestArmed = ReadNdbData(ROLLS_DOWN_DATA_TAG, m_startTestArmed);
    Log(LOG_DEV_DATA, "Register - start test armed: %s", m_startTestArmed ? "True" : "False");
    // return the result
    return result;
}

/**
 * Changes state of Drivers Monitor and put up some prompts
 *
 * @param ctrl Current value of control data needed for system control events
 */
void Base3700SystemMonitor::UpdateDriverMonitor(ControlData *ctrl)
{
	// If we have vehicle present
	if( ctrl->vehiclePresent)
	{
		// If DVT sent us a phase and we have vehicle present
		if( m_commandedPhase.empty() == false)
		{
			// IF cal key was just turned off
			if( (m_oldCtrl->calSwitch == true) && ( ctrl->calSwitch == false))
			{
				Log( LOG_DEV_DATA, "Removing prompt <TurnLossCompensationSwitchOff> because LC switch just turned off\n");
				// Remove prompt telling operator to turn off cal key
				RemovePrompt( 2,  "TurnLossCompensationSwitchOff", "white");
			}
			// If the cal key is on
			else if( ctrl->calSwitch == true)
			{
				Log( LOG_DEV_DATA, "Not commanding phase %s because loss comp switch is on\n", m_commandedPhase.c_str());
				// Tell operator to turn off cal key
				DisplayPrompt( 2,  "TurnLossCompensationSwitchOff", "white");
			}
		}
	}

	SystemMonitor::UpdateDriverMonitor( ctrl);
}

/**
 * Check to determine if the FACTS test is good to go and publish the OkToRoll tag
 * to indicate the status.
 * 
 * @param ctrl Current value of control data needed for system control events
 */
void Base3700SystemMonitor::CheckOkToRoll( ControlData *ctrl)
{
	Base3700ControlData *Base3700Ctrl = (Base3700ControlData *)ctrl;
	// if the vehicle is present, the rolls are up, a test is in progress,
	// there are no fatal failures, the wheelbase is in position, cal switch is off
	// and we have not exceeded the loss somp cycle limit set ok to roll, else set not.
	if(	(ctrl->vehiclePresent) && !(ctrl->rollsDown) && (ctrl->rollsUp) &&
		!(ctrl->machineFault) && !(ReadSubscribeData( ABORT_DATA_TAG) == "1") &&
		(false == ctrl->calSwitch) && 
		// Exceeded loss comp cycle limit OR no loss comp cycle limit
		((Base3700Ctrl->testsSinceLossComp < m_testsBetweenLossComp) || (0 == m_testsBetweenLossComp))
		)
	{
		if(true != Base3700Ctrl->okToRoll)
		{
			Log(LOG_DEV_DATA, "Updating OkToRoll to TRUE\n");
			Write(OKTOROLL_TAG, "1");
		}
		else
		{
			Log(LOG_DEV_DATA, "Not Updating OkToRoll to TRUE\n");
		}
	}
	// else we are not ok to roll
	else
	{
        if (!(ctrl->vehiclePresent))
        {
            Log(LOG_DEV_DATA, "CheckOkToRoll(): No Vehicle Displaying ScanVin prompt\n");
            DisplayPrompt( 1, "ScanVIN", false);
        }/*
        //MAM 7/11/07 - show "Raise Retainers" prompt if not rollsup
        else if (!(ctrl->rollsUp) && ctrl->rollsDown)
        {
            Log(LOG_DEV_DATA, "CheckOkToRoll(): Roll Not Up! Displaying RaiseRetainers prompt\n");
            DisplayPrompt( 1,  "RaiseRetainers", false);
        }
        else
        {
            Log(LOG_DEV_DATA, "CheckOkToRoll(): Roll Up! Removing RaiseRetainers prompt\n");
            RemovePrompt( 1,  "RaiseRetainers");
        }*/

        // If we have exceeded the inter loss comp test cycle limit
		if((Base3700Ctrl->testsSinceLossComp >= m_testsBetweenLossComp) && (m_testsBetweenLossComp != 0))
		{
			// If we are starting a loss comp
			if( true == ctrl->calSwitch)
			{
				// If PLC diag is currently set
				if( true == Base3700Ctrl->cycleLimitExceeded)
				{
					// Clear the PLC diag bit (will get set next time we try to run a test if the loss comp does
					//	not complete)
					Log(LOG_ERRORS, "Setting CycleLimitExceeded = 0\n");
					Write( GetDataTag("CycleLimitExceeded"), "0");
				}
			}
			// Check if the inter-loss comp test cycle count has been exceeded
			else if(false == Base3700Ctrl->cycleLimitExceeded)
			{
				Log(LOG_ERRORS, "Setting CycleLimitExceeded = 1\n");
				Write( GetDataTag("CycleLimitExceeded"), "1");
			}
		}

		// Set/clear a fault if the cycle limit exceeded status has changed
		Base3700ControlData *oldCtrl = (Base3700ControlData*)m_oldCtrl;
		if( oldCtrl->cycleLimitExceeded != Base3700Ctrl->cycleLimitExceeded)
		{
			if( m_faultServer != NULL)
			{
				// get the fault info and update the system
				const XmlNode *faultNode = NULL;
				std::string response;
				try
				{
					faultNode = m_faultInformation->getChild("LossCompensationCycleLimit");
					const std::string &faultTag = faultNode->getAttribute("Name")->getValue();
					if( Base3700Ctrl->cycleLimitExceeded == true)
					{
						// get the fault info and update the system
						m_faultServer->SetFault(faultTag, 
												faultNode->getAttribute("Description")->getValue(),
												GetProcessName(),
												response, 
												true, 
												atoi(faultNode->getAttribute("Severity")->getValue().c_str()));
						Log(LOG_ERRORS, "LossCompensationCycleLimit Fault set (tag: %s)\n", faultTag.c_str());
					}
					else
					{
						m_faultServer->ClearFault( faultNode->getAttribute("Name")->getValue(), response, true);
						Log(LOG_ERRORS, "LossCompensationCycleLimit Fault cleared (tag: %s)\n", faultTag.c_str());
					}
				}
				catch( ...)
				{
					if( Base3700Ctrl->cycleLimitExceeded == true)
					{
						m_faultServer->SetFault("LossCompensationCycleLimit", 
												"Loss Compensation Cycle Limit Reached",
												GetProcessName(),
												response, true, 
												9);
						Log(LOG_ERRORS, "LossCompensationCycleLimit Fault set\n");
					}
					else
					{
						m_faultServer->ClearFault( "LossCompensationCycleLimit", response, true);
						Log(LOG_ERRORS, "LossCompensationCycleLimit fault cleared\n");
					}
				}
			}
		}
		
		if(false != Base3700Ctrl->okToRoll)
		{
			Log(LOG_DEV_DATA, "Updating OkToRoll to FALSE\n");
			Write(OKTOROLL_TAG, "0");
		}
		else
		{
			Log(LOG_DEV_DATA, "Not Updating OkToRoll to FALSE\n");
		}
	}
}

#if 0
/**
 * Test ABORT logic
 *
 * @param ctrl Current value of control data needed for system control events
 */
void Base3700SystemMonitor::CheckAbort( ControlData *ctrl)
{
//	bool	invalidLicense = (bool)(ReadSubscribeData(INVALID_LICENSE_FATAL_FAULT_TAG) == "1");
	//bool    retainersUp = true;

	Log( LOG_FN_ENTRY, "Enter Base3700SystemMonitor::CheckAbort()\n");

	SystemMonitor::CheckAbort(ctrl);

	if(true == invalidLicense)
	{
		if( "0" == Read(ABORT_DATA_TAG))
		{
			DisplayPrompt( 1,  INVALID_LICENSE_FATAL_FAULT_TAG);
			BepServer::Write( ABORT_DATA_TAG, "1");
		}
	}
	// If currently testing something
	else if ((ctrl->testInProgress) || (ctrl->calInProgress))
	{
		// if the retainers are lowered during a test, abort
		if( (ctrl->testInProgress) && (ctrl->rollsDown) && (!(m_oldCtrl->rollsDown)) )
		{
			Log(LOG_ERRORS, "ERROR: Driver Lowered Retainers and ABORTED The Test\n");
			BepServer::Write( ABORT_DATA_TAG, "1");
		}        
	}
	// else if the rolls are lowered and not in a test, clear the abort
	else if (ctrl->rollsDown)
	{
		// and an abort occurred, clear the ABORT condition
		if (Read(ABORT_DATA_TAG) == "1")
			BepServer::Write( ABORT_DATA_TAG, "0");
	}
	Log( LOG_FN_ENTRY, "Exit Base3700SystemMonitor::CheckAbort()\n");
}
#endif

/*
void Base3700SystemMonitor::StackLightCheck( ControlData *ctrl)
{   // perform stack light checks here
	// if the rolls were just raised, set the stack light to Blue
	if(!ctrl->rollsDown && m_oldCtrl->rollsDown)
	{
		Log(LOG_DEV_DATA, "Enabling the blue stack light and turning all else off\n");
		WriteNdbData(STACK_LIGHT_GREEN_TAG, false);
		WriteNdbData(STACK_LIGHT_YELLOW_TAG, false);
		WriteNdbData(STACK_LIGHT_RED_TAG, false);
		WriteNdbData(STACK_LIGHT_BLUE_TAG, true);
	}
	// if vehicle present just lost, and the rolls are not raised set the stack light to Green
	else if(!ctrl->vehiclePresent && ctrl->rollsDown)
	{
		Log(LOG_DEV_DATA, "Enabling the green stack light and turning all else off\n");
		WriteNdbData(STACK_LIGHT_GREEN_TAG, true);
		WriteNdbData(STACK_LIGHT_YELLOW_TAG, false);
		WriteNdbData(STACK_LIGHT_RED_TAG, false);
		WriteNdbData(STACK_LIGHT_BLUE_TAG, false);
	}
}
*/
  
void Base3700SystemMonitor::UpdateCommandPhase( ControlData *ctrl)
{
	Log(LOG_FN_ENTRY, "Enter UpdateCommandPhase()\n");

    // if there is a test in progress
	if( ctrl->testInProgress)
    {
		// Do not test if the cal switch is on
		if( ctrl->calSwitch == false)
		{
			// if a phase has been commanded
			if(!m_commandedPhase.empty())
			{
				// Must wait for rolls to be fully raised before starting a test
				if( ctrl->rollsUp)
				{
					Log(LOG_DEV_DATA, "Commanding the phase: %s\n", m_commandedPhase.c_str());
					CommandNdbData( COMMAND_PHASE_TAG, m_commandedPhase);
					m_commandedPhase.clear();
				}
				else
				{
					Log(LOG_DEV_DATA, "No ROLLS_UP....not commanding phase '%s'\n", m_commandedPhase.c_str());
				}
			}
		}
	    else
	    {
	        Log(LOG_DEV_DATA, "Not Commanding the phase <%s> because LC switch is on\n", m_commandedPhase.c_str());
	    }
    }
    else
        Log(LOG_DEV_DATA, "Not Commanding the phase: %s\n", m_commandedPhase.c_str());

    Log(LOG_FN_ENTRY, "Exit UpdateCommandPhase()\n");
}

/**
 * Testing control logic
 *
 * @param ctrl Current value of control data needed for system control events
 */
void Base3700SystemMonitor::CheckTesting( ControlData *ctrl)
{
	Base3700ControlData *Base3700Ctrl = (Base3700ControlData *)ctrl;

	Log( LOG_FN_ENTRY, "Enter Base3700SystemMonitor::CheckTesting()\n");

    // if not currently testing, the machine is ok, and a new test was issued
    if((!ctrl->testInProgress) && (!ctrl->machineFault) && (ctrl->startTestCommand) && !m_oldCtrl->startTestCommand)
    {   
		if( Base3700Ctrl->okToRoll)
		{
			// Command VehicleDataBroker to read the latest build data in preperation
	        //	of a test start
	        Log( LOG_DEV_DATA, "Commanding VehicleDataBroker to Load VehicleBuild\n");
	        CommandNdbData( READ_LATEST_BUILD_DATA_TAG, "1");
	    
	        // Command the start of a test
	        Log( LOG_DEV_DATA, "Commanding Vehicle Tester to Run A Vehicle Test\n");
	        CommandNdbData( START_VEHICLE_TEST_DATA_TAG, "1");
		}
		else
		{
			Log( LOG_DEV_DATA, "Not commanding Start Test because OkToRoll == 0\n");
		}
    }

    // if a test was just completed, reset the drive axle
    if( (!(ctrl->testInProgress)) && (m_oldCtrl->testInProgress) && (m_currentDriveAxle.empty() == false) )
    {
        m_currentDriveAxle.erase();
        if((errno=m_ndbLock.Acquire()) == EOK)
        {
            WriteNdbData("FrontWheelDriveSelectedToPLC",false);
            WriteNdbData("RearWheelDriveToPLC",false);
            m_ndbLock.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tClear DriveSelectionToPLC Error locking ndb mutex: %s\n", strerror( errno));
        }
    }

    // If a vehicle just pulled up, prompt to connect cable before raising retainers
    if(ctrl->vehiclePresent && !m_oldCtrl->vehiclePresent)
    {   
        RemovePrompt(1, "ScanVIN");
        DisplayPrompt(1, "ConnectCable");
    }
    // After the cable is connected
    else if(ctrl->cableConnect && !m_oldCtrl->cableConnect)
    {   
        RemovePrompt(1, "ConnectCable");
        DisplayPrompt(1, "RaiseRetainers");
    }

    // Set the start test flag if the retainers just raised
    Log(LOG_DEV_DATA, "CheckTesting - start test armed: %s", m_startTestArmed ? "True" : "False");
    if(ctrl->rollsUp && !m_oldCtrl->rollsUp && m_startTestArmed)
    {   // If a vehicle is present, need to read build data
        if(ctrl->vehiclePresent && ctrl->cableConnect)
        {
            RemovePrompt(1,"RaiseRetainers");
            Log( LOG_DEV_DATA, "Commanding VehicleDataBroker to Load VehicleBuild\n");
            CommandNdbData( READ_LATEST_BUILD_DATA_TAG, "1");
            // Command the start of a test
            Log( LOG_DEV_DATA, "Commanding Vehicle Tester to Run A Vehicle Test\n");
            CommandNdbData( START_VEHICLE_TEST_DATA_TAG, "1");
        }
        else
        {   // Signal test in progress since this is a loss comp sequence
            WriteNdbData(TEST_IN_PROG_DATA_TAG, true);
        }
        Log(LOG_DEV_DATA, "Setting %s to 1 for start of test", START_TEST_DATA_TAG);
        Command(START_TEST_DATA_TAG, "1");
        // Disarm the start test flag
        m_startTestArmed = false;
    }
    else if(!ctrl->testInProgress && m_oldCtrl->testInProgress)
    {   // Test just completed, disconnect cable if still connected
        Log(LOG_DEV_DATA, "Test just completed, checking cable connect");
        if(ctrl->cableConnect)
        {
            DisplayPrompt(2, "DisconnectCable");
        }
        else if(!ctrl->rollsDown)
        {   // Cable already disconnected, release vehicle
            DisplayPrompt( 2,  "LowerRetainers", true);
        }
    }
    else if(!ctrl->cableConnect && m_oldCtrl->cableConnect)
    {   // Cable just disconnected, remove prompt and instruct to close rollers
        Log(LOG_DEV_DATA, "Rolls are open, cable was just disconnected");
        RemovePrompt(2, "DisconnectCable");
        DisplayPrompt( 2,  "LowerRetainers", true);
    }
    else if(!ctrl->rollsUp && ctrl->rollsDown && !m_oldCtrl->rollsDown)
    {   // Rolls just lowered, clear the start test flag
        Log(LOG_DEV_DATA, "Setting %s to 0 for end of test", START_TEST_DATA_TAG);
        Command(START_TEST_DATA_TAG, "0");
        RemovePrompt( 2,  "LowerRetainers");
        // If this is a loss comp sequence, clear test inprogress data tag
        WriteNdbData(TEST_IN_PROG_DATA_TAG, false);
        if(!ctrl->vehiclePresent)
        {
            Log(LOG_DEV_DATA, "Loss Comp completed, setting %s to 0", TEST_IN_PROG_DATA_TAG);
        }
        else
        {
            Log(LOG_DEV_DATA, "Vehicle present, allowing VehicleTest to handle %s state", TEST_IN_PROG_DATA_TAG);
        }
    }

	Log( LOG_FN_ENTRY, "Exit Base3700SystemMonitor::CheckTesting()\n");
}

INT32 Base3700SystemMonitor::GetWheelSpeeds(float wheelSpeed[])
{
	INT32 testStatus = BEP_STATUS_FAILURE;    // overall status
	
	Log(LOG_FN_ENTRY, "GetWheelSpeeds():\n");
	int blockFd = -1;                  // block file descriptor
	try
	{                                  // clear out the wheel speeds
		memset(wheelSpeed, 0, (sizeof(float) * GetRollerCount()));
		// open the speed block
		std::string file = m_ICMSpeedLocation;
		Log(LOG_DEV_DATA, "File Open: %s\n", file.c_str());
		int retry = 3;                 // the number of retrys to attempt
		do
		{
			if((blockFd = open(file.c_str(), O_RDONLY)) != -1)
			{
				UINT32 bytes = read(blockFd, (void *) wheelSpeed, (sizeof(float) * GetRollerCount()));
				if(bytes != (sizeof(float) * GetRollerCount()))
				{                      // if interrupted during a read, read again
					if(errno == EINTR)
						Log(LOG_ERRORS, "Interrupted Trying To Read %s\n", file.c_str());
					// else display the error report
					else
						Log(LOG_ERRORS, "Error Reading The File Information: %d, %s\n", bytes, strerror(errno));
				}
				else
				{
					testStatus = BEP_STATUS_SUCCESS;
					Log(LOG_DEV_DATA, "Read speeds: %.2f, %.2f, %.2f, %.2f\n", 
					wheelSpeed[0], wheelSpeed[1], wheelSpeed[2], wheelSpeed[3]);
				}
				
				// Close the array file
				close(blockFd);
				blockFd = -1;
			}
			else
			{
				Log(LOG_ERRORS, "Could Not Open: %s: %s\n", file.c_str(), strerror(errno));
				testStatus = BEP_STATUS_SOFTWARE;
			}
			// if the file could not be open, give up the time slice
			if(testStatus != BEP_STATUS_SUCCESS)    BposSleep(1);
		}while((testStatus != BEP_STATUS_SUCCESS) && (retry--));
	}
	catch(BepException &e)
	{
		Log(LOG_ERRORS, "GetISpeeds Exception: %s\n", e.what());
		testStatus = BEP_STATUS_ERROR;
		if(blockFd != -1)      close(blockFd);
	}
	
	return(testStatus);
}

/**
 * Perform a check to determine if the conditions are correct to start a
 * Vehicle test and or set the DriveAxle.
 * 
 * @param ctrl   Current value of control data needed for system control events
 */
void Base3700SystemMonitor::UpdateDriveAxle( ControlData *ctrl)
{
	Log(LOG_FN_ENTRY, "Enter Base3700SystemMonitor::UpdateDriveAxle()\n");
	
	// if the machine is not at zerospeed check to start a test and set the drive axle
	if( ctrl->zerospeed == false)
	{
		// if not testing and a test has not been previously started (without lowering the rolls)
		if( ctrl->testInProgress == false)
		{                              // check the wheel speeds to determine if they are over 2 MPH
			Log( LOG_DEV_DATA, "Test Not Started\n");
			float wheelSpeeds[GetRollerCount()];
			GetWheelSpeeds(wheelSpeeds);    // get current wheelspeeds
			
			if((wheelSpeeds[0] > 2.0) || (wheelSpeeds[1] > 2.0) || 
				(wheelSpeeds[2] > 2.0) || (wheelSpeeds[3] > 2.0))
			{
				Log("Speed Detected\n");
			
				// if all wheel drive not selected
				if(ReadSubscribeData(GetDataTag("AllWheelDrive")) == "0")
				{
					// if so, determine if the drive axle is front or rear
					if((wheelSpeeds[0] > 2.0) || (wheelSpeeds[1] > 2.0))
					{
						m_currentDriveAxle = REAR_WHEEL_DRIVE_VALUE;
					}
					else
					{
						m_currentDriveAxle = FRONT_WHEEL_DRIVE_VALUE;
					}
				}
				else                   // all wheel drive selected
				{
					m_currentDriveAxle = ALL_WHEEL_DRIVE_TAG;
				}
					
				// first, lock the NDB
				if((errno=m_ndbLock.Acquire()) == EOK)
				{
					// Update drive axle
					WriteNdbData( GetDataTag( "DriveAxle"), m_currentDriveAxle);
				
					// unlock the ndb interface
					m_ndbLock.Release();
				}
				else
				{
					Log( LOG_ERRORS, "\tUpdateDriveAxleError locking ndb mutex: %s\n", strerror( errno));
				}
			}
		}
	}

	Log(LOG_FN_ENTRY, "Exit Base3700SystemMonitor::UpdateDriveAxle()\n");
}


INT32 Base3700SystemMonitor::GetRollerCount( void)
{
	return(m_rollerCount);
}

void Base3700SystemMonitor::SetRollerCount( INT32 rollerCount)
{
	m_rollerCount = rollerCount;
}


ControlData* Base3700SystemMonitor::ControlAllocate() const
{
	Base3700ControlData *control = Base3700ControlData::Allocate();

	return( (ControlData*)control);
}

void Base3700SystemMonitor::ControlFree(ControlData* &controlData) const
{
	Base3700ControlData *control = (Base3700ControlData *)controlData;
	if( NULL != control)
	{
		delete control;
		control = NULL;
	}
}

