//******************************************************************************
//
// $Archive: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/FordSystemMonitor.cpp $
// $Author: Mmcnamar $
//   $Date: 7/12/07 12:34p $
//
// Description:
//  Ford System Monitor Task
//
//
//==============================================================================
//
//     Copyright ?1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /CMake Core/Source/Ford/ServicesLayer/Servers/SystemMonitor/FordSystemMonitor.cpp $
// 
// 8     7/12/07 12:34p Mmcnamar
// Added "Raise Retainers" prompt in CheckOkToRoll() if !RollsUp.
// 
// 7     6/15/06 5:55p Bmeinke
// Set a fault if we get cycle limit exceeded
// 
// 6     3/20/06 10:36a Bmeinke
// Made FordControlData constructor protected
// Added assignment operator to FordControlData
// Added overloaded Assign() method to FordControlData
// 
// 5     5/24/05 4:25p Bmeinke
// Made SystemMonitor a non-template class again
// 
// 4     5/23/05 3:31p Bmeinke
// Renamed SystemMonitor base class to SystemMonitorTemplate
// 
// 3     5/16/05 12:57p Bmeinke
// Base class SystemMonitor is now a templatized class
// No longer using libsysmon.a
// Call SystemMonitor::GetCurrentControlData() to read data expected by
// base class
// 
// 2     4/25/05 2:44p Bmeinke
// Added inter-"loss compensation" test cycle counter
// 
// 1     4/25/05 9:27a Bmeinke
// New source tree structure using CMake and seperate Core and Customer
// trees
// 
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for GM
// 
// 3     8/30/04 1:56p Bmeinke
// Added support for auto-detecting the drive axle
// 
// 2     7/23/04 3:09p Bmeinke
// Moved the WheelbaseInPosition check to the Initialize objective of the
// Machine Component
// 
// 3     6/01/04 9:36p Bmeinke
// In UpdateDriverMonitor(), if the cal key is on, we have vehicle present
// and we have received a DCL prompt, prompt the operator to turn off the
// cal key
// In CheckOkToRoll(), do not send "Ok To Roll" bit to DCL if the cal
// switch is on
// In UpdateCommandPhase(), do not update the commanded phase (from DCL)
// if the cal switch is on
// 
// 2     5/11/04 9:20p Bmeinke
// Added logic to turn on/off the PLC "test complete" and "accept" lamps
// inside Checktesting()
// Added overallPass to the base ControlData structure
// 
// 1     4/12/04 1:21p Bmeinke
// Changing VSS repositories
// 
// 1     3/05/04 2:28p Bmeinke
// Latest release from March 03,2004 trip to Chicago
// 
// 2     3/05/04 2:00p Bmeinke
// Moved __USAGE block to top of file to make diffs easier
// 
// 4     2/04/04 2:33p Bmeinke
// Added checks for WheelbaseInPosition to CheckOkToRoll
// Overrode UpdateDriverMonitor() to display a DCL prompt when the
// Wheelbase is not in the correct position.
// 
// 3     12/09/03 10:18a Bmeinke
// Kevin's latest updates from Dearborn
// Make sure we have rolls up inside UpdateCommandPhase() and
// CheckOkToRoll
// Added rolls up to subscribed data tags and need to read its value
// inside GetCurrentControlData()
// 
// 2     8/07/03 3:10p Khykin
// Updated with changes from Flatrock.
// 
// 1     6/27/03 8:21a Khykin
// Update from SysMon to SystemMonitor.
// 
// 1     6/20/03 9:55a Khykin
// Initial checkin.
// 
// 2     6/20/03 9:08a Khykin
// GM updates.
// 
// 1     6/12/03 3:58p Khykin
// Initial checkin.
// 
//
//******************************************************************************
/*******************************************************************************
	Usage Block
	Information printed when a user types 'use xxxxx' at the QNX prompt
	
#ifdef __USAGE

GM System Monitor Task

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
	- Registers name as 'rolls/GMSystemMonitor' by default
	- Must be run as Super User.
	- $Revision: 8 $
	- $Date: 7/12/07 12:34p $

#endif

*******************************************************************************/

/*******************
 include files
 ******************/
#include "GmSystemMonitor.h"

#include <fcntl.h>	// O_RDONLY
//////////////////////////
// Class Implementation //
//////////////////////////

/**
 * Default constructor. Set debug to false, channel ID's to -1, etc
 */
GMSystemMonitor::GMSystemMonitor() : SystemMonitor(), 
	m_ICMSpeedLocation( "/dev/Icm/Block/Speed"), m_rollerCount( 4), m_currentDriveAxle(""), 
	m_testsBetweenLossComp( 0)
{
}

/**
 * Constructor from command line args
 *
 * @param argc   Number of command line arguments
 * @param argv   Array of command line args
 */
GMSystemMonitor::GMSystemMonitor( int argc, char *argv[]) : SystemMonitor( argc, argv),
	m_ICMSpeedLocation( "/dev/Icm/Block/Speed"), m_rollerCount( 4), m_currentDriveAxle(""),
	m_testsBetweenLossComp(0)
{
}

/**
 * Class destructor
 */
GMSystemMonitor::~GMSystemMonitor()
{
}

/**
 * method for component initialization.
 *
 * @param document Reference to a parsed configuration document.
 * @since Version 1.0
 */
void GMSystemMonitor::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter GMSystemMonitor::Initialize()\n");

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

	Log( LOG_FN_ENTRY, "Exit GMSystemMonitor::Initialize()\n");
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
const std::string GMSystemMonitor::Write(const std::string &tag, const std::string &value)
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
 */
const std::string GMSystemMonitor::Write(const XmlNode *node)
{
    std::string status;

    if(node->getName() == START_TEST_DATA_TAG)
    {
        status = Command(node);
    }
    // Call the base class to make sure all notifications take place
    status = SystemMonitor::Write(node);

    return(status);
}


/**
 * Request to Perform an action
 *
 * @param tag    Action to be performed
 * @return Status of operation
 *         Success, Fail, Invalid, Unavailable
 */
const std::string GMSystemMonitor::Command(const std::string &tag, const std::string &value)
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
const std::string GMSystemMonitor::Command(const XmlNode *node)
{
	Log( LOG_FN_ENTRY, "Enter Command(%s)\n", node->ToString().c_str());
    
    std::string status = BEP_UNSUPPORTED_RESPONSE;

    // if a start test is issued by the GMPlantHostInbound
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
void GMSystemMonitor::UpdateControl(ControlData *ctrl)
{
	Log( LOG_FN_ENTRY, "Enter GM UpdateControl()\n");
	
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
		UpdateDriveAxle( ctrl);
		// Check for reprint request
		CheckReprintRequest( ctrl);
		// Update data
		*m_oldCtrl = *ctrl;
	}

	Log( LOG_FN_ENTRY, "Exit UpdateControl()\n");
}
		
/**
 * Reads current values of control data items. The m_subscribedData data is
 * updated asynchronously via subscription notifications from the servers to
 * whoim we are subscribed
 *
 * @param ctrl   Control Data structure to fill with values
 */
void GMSystemMonitor::GetCurrentControlData( ControlData *ctrl)
{
	GMControlData *GMCtrl = (GMControlData *)ctrl;
	Log( LOG_FN_ENTRY, "Enter GM GetCurrentControlData()\n");

	SystemMonitor::GetCurrentControlData( ctrl);
	
	///////////////////////
	// GM Additions

	ctrl->startTestCommand = (bool)(ReadSubscribeData(START_TEST_DATA_TAG) == "1");
	ctrl->cableConnect = (bool)(ReadSubscribeData( PLC_CBL_CONNECT) == "1");
	ctrl->zerospeed = (bool)(ReadSubscribeData( ZEROSPEED_TAG) == "1");
	GMCtrl->okToRoll = (bool) (ReadSubscribeData(OKTOROLL_TAG) == "1");
	GMCtrl->testsSinceLossComp = atoi( ReadSubscribeData( TESTS_SINCE_LOSS_COMP_TAG).c_str());
	GMCtrl->cycleLimitExceeded = (bool) (ReadSubscribeData( GetDataTag("CycleLimitExceeded")) == "1");
	///////////////////////

	Log( LOG_FN_ENTRY, "Exit GM GetCurrentControlData()\n");
}

/**
 * Changes state of Drivers Monitor and put up some prompts
 *
 * @param ctrl Current value of control data needed for system control events
 */
void GMSystemMonitor::UpdateDriverMonitor(ControlData *ctrl)
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
void GMSystemMonitor::CheckOkToRoll( ControlData *ctrl)
{
	GMControlData *GMCtrl = (GMControlData *)ctrl;
	// if the vehicle is present, the rolls are up, a test is in progress,
	// there are no fatal failures, the wheelbase is in position, cal switch is off
	// and we have not exceeded the loss somp cycle limit set ok to roll, else set not.
	if(	(ctrl->vehiclePresent) && !(ctrl->rollsDown) && (ctrl->rollsUp) &&
		!(ctrl->machineFault) && !(ReadSubscribeData( ABORT_DATA_TAG) == "1") &&
		(false == ctrl->calSwitch) && 
		// Exceeded loss comp cycle limit OR no loss comp cycle limit
		((GMCtrl->testsSinceLossComp < m_testsBetweenLossComp) || (0 == m_testsBetweenLossComp))
		)
	{
		if(true != GMCtrl->okToRoll)
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
        //MAM 7/11/07 - show "Raise Retainers" prompt if not rollsup
        if (!(ctrl->rollsUp))
        {
            if(ctrl->vehiclePresent)
            {
                Log(LOG_DEV_DATA, "CheckOkToRoll(): Roll Not Up! Displaying RaiseRetainers prompt\n");
                DisplayPrompt( 1,  "RaiseRetainers", false);
            }
        }
        else
        {
            Log(LOG_DEV_DATA, "CheckOkToRoll(): Roll Up! Removing RaiseRetainers prompt\n");
            RemovePrompt( 1,  "RaiseRetainers");
        }

        // If we have exceeded the inter loss comp test cycle limit
		if((GMCtrl->testsSinceLossComp >= m_testsBetweenLossComp) && (m_testsBetweenLossComp != 0))
		{
			// If we are starting a loss comp
			if( true == ctrl->calSwitch)
			{
				// If PLC diag is currently set
				if( true == GMCtrl->cycleLimitExceeded)
				{
					// Clear the PLC diag bit (will get set next time we try to run a test if the loss comp does
					//	not complete)
					Log(LOG_ERRORS, "Setting CycleLimitExceeded = 0\n");
					Write( GetDataTag("CycleLimitExceeded"), "0");
				}
			}
			// Check if the inter-loss comp test cycle count has been exceeded
			else if(false == GMCtrl->cycleLimitExceeded)
			{
				Log(LOG_ERRORS, "Setting CycleLimitExceeded = 1\n");
				Write( GetDataTag("CycleLimitExceeded"), "1");
			}
		}

		// Set/clear a fault if the cycle limit exceeded status has changed
		GMControlData *oldCtrl = (GMControlData*)m_oldCtrl;
		if( oldCtrl->cycleLimitExceeded != GMCtrl->cycleLimitExceeded)
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
					if( GMCtrl->cycleLimitExceeded == true)
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
					if( GMCtrl->cycleLimitExceeded == true)
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
		
		if(false != GMCtrl->okToRoll)
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

//-------------------------------------------------------------------------------------------------
void GMSystemMonitor::UpdateCommandPhase( ControlData *ctrl)
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
void GMSystemMonitor::CheckTesting( ControlData *ctrl)
{
	GMControlData *GMCtrl = (GMControlData *)ctrl;

	Log( LOG_FN_ENTRY, "Enter GMSystemMonitor::CheckTesting()\n");

    // if not currently testing, the machine is ok, and a new test was issued
    if((!ctrl->testInProgress) && (!ctrl->machineFault) && (ctrl->startTestCommand) && !m_oldCtrl->startTestCommand)
    {   
		if( GMCtrl->okToRoll)
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

    if(m_oldCtrl->rollsUp && !ctrl->rollsUp)
    {   // Rolls just lowered, clear the start test flag
        Log(LOG_DEV_DATA, "Setting %s to 0 for end of test", START_TEST_DATA_TAG);
        Command(START_TEST_DATA_TAG, "0");
    }

    Log( LOG_FN_ENTRY, "Exit GMSystemMonitor::CheckTesting()\n");
}

INT32 GMSystemMonitor::GetWheelSpeeds(float wheelSpeed[])
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
void GMSystemMonitor::UpdateDriveAxle( ControlData *ctrl)
{
	Log(LOG_FN_ENTRY, "Enter GMSystemMonitor::UpdateDriveAxle()\n");
	
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
                        m_currentDriveAxle = FRONT_WHEEL_DRIVE_VALUE;
					}
					else
					{
                        m_currentDriveAxle = REAR_WHEEL_DRIVE_VALUE;
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

	Log(LOG_FN_ENTRY, "Exit GMSystemMonitor::UpdateDriveAxle()\n");
}


void GMSystemMonitor::CheckLossCompensation( ControlData *ctrl, bool checkStatus  /*= false*/)
{
    Log( LOG_FN_ENTRY, "Enter CheckLossCompensation()\n");
    Log( LOG_DEV_DATA, "\ttestInProgress: %d\n", ctrl->testInProgress);
    Log( LOG_DEV_DATA, "\t calInProgress: %d\n", ctrl->calInProgress);
    Log( LOG_DEV_DATA, "\t     calSwitch: %d\n", ctrl->calSwitch);
    Log( LOG_DEV_DATA, "\tcalSwitchToPLC: %d\n", ctrl->calSwitchToPLC);
    Log( LOG_DEV_DATA, "\t simInProgress: %d\n", ctrl->simInProgress);
    Log( LOG_DEV_DATA, "\t  cableConnect: %d\n", ctrl->cableConnect);
    Log( LOG_DEV_DATA, "\tvehiclePresent: %d\n", ctrl->vehiclePresent);

    bool updateSwitchStatus = false;        // flag to indicate if the switch status should be updated

    // if the switch should be debounced
    if (m_lcSwitchHystTime)
    {   // if the state of the lc switch change, reset the timer
        if ((ctrl->calSwitch != m_oldCtrl->calSwitch) || ctrl->calCycle)
        {
            Log( LOG_DEV_DATA, "Switch Change Detected\n");
            m_lossCompensationTimer.Stop();     // ensure the timer is reset
            m_lossCompensationTimer.Start();        // start the timer
        }
        // else if the debounce timer has elapsed, the status is valid
        else if (checkStatus)
        {
            Log( LOG_DEV_DATA, "Switch Change Handled\n");
            updateSwitchStatus = true;
            m_lossCompensationTimer.Stop();     // stop the timer
        }
    }
    // else use the switch status as it exists
    else
    {   // if the key just changed
        if (ctrl->calSwitch != m_oldCtrl->calSwitch)
            updateSwitchStatus = true;
    }

    // if there is not a machine fault
    //  || 1 debug
    if (!ctrl->machineFault || 1)
    {   // if a change was verified, update the system
        if (updateSwitchStatus)
        {
            // enable/disable LC mode to the PLC
            WriteNdbData( LOSSCOMPENSATION_TAG, ctrl->calSwitch);
			WriteNdbData( LOSSCOMPENSATION_TO_PLC_TAG, ctrl->calSwitch);
            // enable/disable LC switch status to the PLC
            WriteNdbData( LC_SWITCHTOPLC_STATUS_TAG, ctrl->calSwitch);
            std::string screen = (ctrl->calSwitch ? "losscompensation" : "frontpanel");
            WriteNdbData( DISPLAY_RACK_MONITOR_SCREEN, screen);
        }

        // If we are currently running a calibrate
        if (ctrl->calInProgress)
        {   // If cal switch just turned off or invalid LC conditions
            if (((!(ctrl->calSwitchToPLC) && m_oldCtrl->calSwitchToPLC) ||
                 (ctrl->rollsDown) || (ctrl->simInProgress) || (ctrl->cableConnect) ||
                 (ctrl->vehiclePresent)) && ctrl->calInProgress)
            {   // Cancel calibration
                XmlNode dataNode(START_LOSSCOMPENSATION_DATA_TAG, "0");
                BepServer::Write(&dataNode);
            }
        }
        // else if not running a test, Check for LC : JPS Changed Assumption: vehicle
        // present assume test is in progress.
        else if (!(ctrl->testInProgress))
        {   // if not simulating, no vehicle present, and the cable is not connected
            // check to perform LC test
            if (!(ctrl->simInProgress) && !(ctrl->cableConnect) && !(ctrl->vehiclePresent))
            {   // if the LC switch is on and the key is cycled and a calibrate is not in progress
                if (ctrl->calSwitchToPLC && (ctrl->calCycle && !m_oldCtrl->calCycle) && !(ctrl->calInProgress))
                {   // Tell user it is time to perform a LC. The VehicleTest server will
                    //  publish his TestInProgress when he receives this start
                    //  cal data. This will cause us to start looking for end of
                    //  calibrate.
                    ctrl->calKeyCycled = true;
                    // remove all prompts from second box
                    // remove the LC in-progress prompt
                    RemovePrompt( 1,  "LossCompensationInProgress");
                    // clear out the second box
                    DisplayPrompt(2, "Blank");
                    XmlNode dataNode(START_LOSSCOMPENSATION_DATA_TAG, "1");
                    BepServer::Write(&dataNode);
                    // set the VIN to LossCompensation
                    WriteNdbData( VIN_DATA_TAG, std::string("Loss_Compensation"));
                    // set the VINDisplay to LossCompensation
                    WriteNdbData( VINDISPLAY_DATA_TAG, std::string("Loss Compensation"));
                    //Command loss compensation component test
                    system("SendCmd -ncomponent/LossCompensation -tCommand -xLossCompensation"); 
                }
                // else if the LC switch just turned off, or the LC test completed or the rolls were lowered and
                // a LC test is was running
                else if ((!(ctrl->calSwitchToPLC) || (ctrl->testInProgress && !m_oldCtrl->testInProgress) ||
                          ctrl->rollsDown) && (ctrl->calInProgress))
                {
                    XmlNode dataNode(START_LOSSCOMPENSATION_DATA_TAG, "0");
                    BepServer::Write(&dataNode);
                    WriteNdbData( PROMPT_BOX_1_BGCOLOR, std::string("white"));
                    WriteNdbData( PROMPT_BOX_2_BGCOLOR, std::string("white"));
                }
                // if retainers were raised and cal key was on set test in progress to zero
                //else if (ctrl->rollsUp && ctrl->calSwitchToPLC)
                //{
                //    ctrl->calConditionsCorrect = true;
                //    DisplayPrompt( 2,  "CycleLossCompensationSwitch", true);
                //}
            }
        }
    }
    Log( LOG_FN_ENTRY, "Exit CheckLossCompensation()\n");
}


INT32 GMSystemMonitor::GetRollerCount( void)
{
	return(m_rollerCount);
}

void GMSystemMonitor::SetRollerCount( INT32 rollerCount)
{
	m_rollerCount = rollerCount;
}


ControlData* GMSystemMonitor::ControlAllocate() const
{
	GMControlData *control = GMControlData::Allocate();

	return( (ControlData*)control);
}

void GMSystemMonitor::ControlFree(ControlData* &controlData) const
{
	GMControlData *control = (GMControlData *)controlData;
	if( NULL != control)
	{
		delete control;
		control = NULL;
	}
}

