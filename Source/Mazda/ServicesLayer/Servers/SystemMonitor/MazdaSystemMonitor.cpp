//*************************************************************************
// FILE DESCRIPTION:
//  The MazdaSystemMonitor monitors the system to determine when a vehicle
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
#include "MazdaSystemMonitor.h"

//-------------------------------------------------------------------------------------------------
MazdaSystemMonitor::MazdaSystemMonitor() : SystemMonitor(), m_buildDataLoaded(false)
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaSystemMonitor::MazdaSystemMonitor(int argc, char *argv[]) : SystemMonitor(argc, argv)
{	// Nothing special to do here
}

//-------------------------------------------------------------------------------------------------
MazdaSystemMonitor::~MazdaSystemMonitor()
{	// Nothing special to do here
}

//-----------------------------------------------------------------------------
void MazdaSystemMonitor::Initialize(const XmlNode *document)
{
	Log( LOG_FN_ENTRY, "Enter MazdaSystemMonitor Initialize()\n");

	// initialize the base SystemMonitor
	SystemMonitor::Initialize(document);


	// read the plc bits that should be reset on system restart
	try
	{
		m_plcClearBits.DeepCopy(document->getChild("Setup/PlcBitsToClearOnStartTest")->getChildren());
		Log(LOG_DEV_DATA,"deepcopy worked\n");
	}
	catch(XmlException &excpt)
	{
		Log(LOG_ERRORS, "Failure to find the PlcVehicleBuildBits children: %s\n", excpt.GetReason());
	}


	for(XmlNodeMapCItr iter = m_plcClearBits.begin();
	   (iter != m_plcClearBits.end());
	   iter++)
	{
		try
		{

			std::string clearBit = iter->second->getValue();
			Log(LOG_DEV_DATA, "m_plcClearBits: %s", clearBit.c_str());
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Issue reading PLC map: %s\n", excpt.GetReason());
		}
	}


	Log( LOG_FN_ENTRY, "Exit MazdaSystemMonitor Initialize()\n");
}
//-------------------------------------------------------------------------------------------------
const string MazdaSystemMonitor::Publish(const XmlNode *node)
{	// Call the base class to start the publication handling
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::Publish(tag: %s) - Enter", node->getName().c_str());
	// Check if a new vehicle type has been entered
	if(!node->getName().compare(VIN_READ_STATUS_TAG) && 
	   !node->getValue().compare(VALID_VEHICLE_VIN) && (m_dataBroker != NULL))
	{	// New vehicle type has been entered, load the vehicle build data
		Log(LOG_DEV_DATA, "New vehicle type entered, commanding VDB to load build data");
		CommandNdbData(READ_LATEST_BUILD_DATA_TAG, "1");
		DisplayPrompt(1, "WheelbaseIncorrect");
		BposSleep(250);
		VehicleBuildDataLoaded(&buildDataLoaded);
	}
	else if(m_dataBroker == NULL)
	{	// Registration is not complete, cannot command NDB data
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
            StartRetRollPositionMove(LOWER_ROLLS);
        }
		else
		{	// Test did not pass, do not home the machine
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
	{	// Got the complete button press, determine if we need to abort or raise retainers
		if(m_oldCtrl->testInProgress)
		{	// Send the Abort tag
			BepServer::Write(ABORT_DATA_TAG, "1");
		}
		else
		{	// Test not in progress, raise the retainers
			VehicleBuildDataLoaded(&buildDataNotLoaded);
			WriteNdbData(BODY_STYLE_TAG, GetDataTag("ClearBodyStyle"));
            StartRetRollPositionMove(LOWER_ROLLS);
        }
    }
	else if(!node->getName().compare(GetDataTag("WbInPosition")) && atob(node->getValue().c_str()))
	{
		RemovePrompt(1, "WheelbaseIncorrect");
		DisplayPrompt(2, "MoveToTestPosition");
	}
    else if(!node->getName().compare(GetDataTag("CommandSysMonLowerRolls")) && atob(node->getValue().c_str()))
	{
         StartRetRollPositionMove(LOWER_ROLLS);
	}
    else if(!node->getName().compare(GetDataTag("CommandSysMonRaiseRolls")) && atob(node->getValue().c_str()))
	{
         StartRetRollPositionMove(RAISE_ROLLS);
	}
	// Return the result
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::Publish(tag: %s) - Exit", node->getName().c_str());
	return SystemMonitor::Publish(node);
}

//-------------------------------------------------------------------------------------------------
void MazdaSystemMonitor::CheckTesting(ControlData *ctrl)
{
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::CheckTesting() - Enter");

	Log(LOG_DEV_DATA, "\tmachineFault: %d", ctrl->machineFault);
	Log(LOG_DEV_DATA, "\ttestInProgress: %d", ctrl->testInProgress);
	Log(LOG_DEV_DATA, "\toldTestInProgress: %d", m_oldCtrl->testInProgress);
	Log(LOG_DEV_DATA, "\trollsDown: %d", ctrl->rollsDown);
	Log(LOG_DEV_DATA, "\tvehiclePresent: %d", ctrl->vehiclePresent);
	Log(LOG_DEV_DATA, "\tvehicleBuildDataLoaded: %d", VehicleBuildDataLoaded());
	Log(LOG_DEV_DATA, "\tvehVinReadStatus: %s", ctrl->vehVinReadStatus.c_str());
	Log(LOG_DEV_DATA, "\twheelbaseInPosition: %d", ctrl->wheelbaseInPosition);

	// Make sure there is not a machine fault that would prevent testing
	if(!ctrl->machineFault)
	{	// Check if a test is in progress
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
		// If AON scanned and good build data available
		else if(!ctrl->testInProgress && ctrl->rollsDown && !ctrl->vehiclePresent && VehicleBuildDataLoaded())
		{
			ClearMazdaAlcBits();
			// Make sure wheelbase is in position
			if(!ctrl->wheelbaseInPosition)
			{	//  Command the wheelbase to move
				StartWheelbaseAdjust();
			}
		}
		// If no test in progress and rolls are down and vehicle present and we have a valid vehicle type
		else if(!ctrl->testInProgress && ctrl->rollsDown && ctrl->vehiclePresent && VehicleBuildDataLoaded())
		{
			Log(LOG_DEV_DATA, "Will only start test with valid vehicle info");
			if(ctrl->vehVinReadStatus == VALID_VEHICLE_VIN)
			{	// Raise the retainers
				Log(LOG_DEV_DATA, "Vehicle present, rolls down and valid vehicle type - raising rolls and starting test");
				RemovePrompt(1, "EnterVehicleType");
				RemovePrompt(2, "MoveToTestPosition");
				WriteNdbData(string("PromptBox2BGColor"), string("white"));
				// Start the test sequence
				CommandNdbData(START_VEHICLE_TEST_DATA_TAG, true);
				// Invalidate the build record status so we do not restart this same test
				WriteSubscribeData(VIN_READ_STATUS_TAG, INVALID_VEHICLE_VIN);
				ctrl->vehVinReadStatus = ReadSubscribeData(VIN_READ_STATUS_TAG);
			}
			else
			{	// No valid vehicle type, ask for one
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
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::CheckTesting() - Exit");
}

//-------------------------------------------------------------------------------------------------
void MazdaSystemMonitor::ClearMazdaAlcBits(void)
{
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::ClearMazdaAlcBits() - Enter");


	std::string clearBit = "";

	for(XmlNodeMapCItr iter = m_plcClearBits.begin();
	   (iter != m_plcClearBits.end());
	   iter++)
	{
		try
		{
			clearBit = iter->second->getValue();
			Log(LOG_DEV_DATA, "clearing plc bit: %s, writing 0 to it", clearBit.c_str());
			WriteDataTag(clearBit,  "0");
		}
		catch(XmlException &excpt)
		{
			Log(LOG_ERRORS, "Issue reading PLC map: %s\n", excpt.GetReason());
		}
	}
	Log(LOG_FN_ENTRY, "MazdaSystemMonitor::ClearMazdaAlcBits() - Exit");
}

//-------------------------------------------------------------------------------------------------
string MazdaSystemMonitor::WriteDataTag(string tag, string value)
{
	std::string response;
	if(m_dataBroker != NULL)
	{
		INT32 status = m_dataBroker->Write(tag, value, response, true);

		// check for errors
		if(status != BEP_STATUS_SUCCESS)
		{
			Log(LOG_ERRORS, "WriteDataTag - Error Writing: %s, %d\n", tag.c_str(), status);
			value = "ERROR";
		}
	}
	else
	{
		Log(LOG_ERRORS, "MazdaSystemMonitor::WriteDataTag() - m_dataBroker object is NULL!");
		value = BEP_UNAVAILABLE_RESPONSE;
	}
	return(value);
}

//-------------------------------------------------------------------------------------------------
void MazdaSystemMonitor::CheckAbort( ControlData *ctrl)
{	// If there is a machine fault during the test, abort the test sequence
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
const bool& MazdaSystemMonitor::VehicleBuildDataLoaded(const bool *loaded /*= NULL*/)
{
	if(loaded != NULL)	m_buildDataLoaded = *loaded;
	return m_buildDataLoaded;
}
