//******************************************************************************
//
// $Archive$
// $Author$
//   $Date$
//
// Description:
//    China System Monitor Task
//
//==============================================================================
//
//     Copyright 2007 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log$
//
//******************************************************************************

/*******************
 include files
 ******************/
#include "ChinaSystemMonitor.h"

//-----------------------------------------------------------------------------
ChinaControlData::ChinaControlData() : ControlData(), rollDrivesEnabled(false)
{
}

//-----------------------------------------------------------------------------
ChinaControlData::~ChinaControlData()
{
}

//-----------------------------------------------------------------------------
ChinaSystemMonitor::ChinaSystemMonitor() : SystemMonitor(), m_rollerCount(4),
m_currentDriveAxle(""), m_updateTestConditionsTime(0), m_startVehicleTestFlag(false),
m_clearResultsOnRollsDown(false)
{
}

//-----------------------------------------------------------------------------
ChinaSystemMonitor::ChinaSystemMonitor( int argc, char *argv[]) : SystemMonitor( argc, argv),
m_rollerCount(4), m_currentDriveAxle(""), m_updateTestConditionsTime(0), m_startVehicleTestFlag(false),
m_clearResultsOnRollsDown(false)
{
}

//-----------------------------------------------------------------------------
ChinaSystemMonitor::~ChinaSystemMonitor()
{

}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter ChinaSystemMonitor Initialize()\n");

    // initialize the base SystemMonitor
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

    // retrieve the update timer time
    try
    {                                  // Make sure valid document to parse
        if(document)
        {                              // if so, set up the timer
            const XmlNode *node = document->getChild( XML_T("Setup"))->getChild( XML_T("UpdateTestConditionsTime"));
            m_updateTestConditionsTime = atol( node->getValue().c_str());
        }
        else
        {                              // If no hysterisis specified, default to 0 seconds
            Log(LOG_ERRORS, "Warning! No UpdateTestConditionsTime Specified, Defaulting to 1sec\n");
            m_updateTestConditionsTime = 1000;
        }
    }
    catch(XmlException &err)
    {                                  // If no hysterisis specified, default to 0 seconds
        Log(LOG_ERRORS, "Warning! No UpdateTestConditionsTime Specified, Defaulting to 1sec\n");
        m_updateTestConditionsTime = 1000;
    }
    // set the update time
    Log( LOG_DEV_DATA, "Using UpdateTestConditionsTime = %f\n", m_updateTestConditionsTime);
    if(m_updateTestConditionsTime)
    {                                  // convert the time to seconds
        m_updateTestConditionsTimer.SetPulseCode( SYS_MON_PULSE_CODE);
        m_updateTestConditionsTimer.SetPulseValue( UPDATE_TEST_CONDITIONS_PULSE); 
        m_updateTestConditionsTimer.Initialize(GetProcessName().c_str(), (UINT64)(m_updateTestConditionsTime * 1000000), 
                                               (UINT64)(m_updateTestConditionsTime * 1000000));
        m_updateTestConditionsTimer.Stop();
    }

    // get the clear test results on rolls down flag
    try
    {
        if(document)
        {
            const XmlNode *clearResultsOnRollsDownFlag =
                document->getChild(XML_T("Setup"))->getChild(XML_T("ClearTestResultsOnRollsDown"));
            m_clearResultsOnRollsDown = (clearResultsOnRollsDownFlag->getValue() == "1");
        }
        else
        {
            m_clearResultsOnRollsDown = false;
        }
    }
    catch(XmlException &err)    // if tag not defined, default to false
    {
        m_clearResultsOnRollsDown = false;
    }
    Log(LOG_DEV_DATA,"m_clearResultsonRollsDown = %d\n",m_clearResultsOnRollsDown);

    Log( LOG_FN_ENTRY, "Exit Korea Initialize()\n");
}

//-----------------------------------------------------------------------------
const std::string ChinaSystemMonitor::Publish(const XmlNode *node)
{   // Check if we need to instruct the operator to drive on
    if(!node->getName().compare(WHEELBASE_IN_POSITION_TAG) && !node->getValue().compare("1"))
    {   // Wheelbase is in position, check if we have a valid scan
        string temp;
        if(ReadNdbData(VIN_READ_STATUS_TAG,temp).compare("Green"))
        {   // Display the drive on message
            DisplayPrompt(1, "DriveOn");
        }
    }
    // Check if the abort button was pressed
    else if(!node->getName().compare("AbortSignalFromPLC") && !node->getValue().compare("1"))
    {
        Log(LOG_ERRORS, "ERROR: Driver ABORTED The Test\n");
        BepServer::Write( ABORT_DATA_TAG, "1");
        DisplayPrompt(1, "OperatorAbort", "Yellow");
        DisplayPrompt(2, "BrakeToStop", "Yellow");
    }
    // Call the base class to any additional handling
    return SystemMonitor::Publish(node);
}

//-----------------------------------------------------------------------------
const std::string ChinaSystemMonitor::Register(void)
{
    // register the base class
    std::string status = SystemMonitor::Register();
    // start the update timer
    m_updateTestConditionsTimer.Start();

    return(status);
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::Run(volatile bool *terminateFlag/*=NULL*/)
{
    Log( LOG_FN_ENTRY, "ChinaSystemMonitor Enter Run()\n");

    const string        cfgFile( GetConfigFile());

    // initialize the change semaphore
    m_updateControl = new BepSemaphore(1, IsDebugOn(), "SystemMonitor");

    // Perform initialization if a configuration file was provided
    if(cfgFile.empty() == false)
    {                                  // call the base class initialize with the file
        BepServer::Initialize( cfgFile);
    }
    else
    {
        Initialize( NULL);
    }

    // Use default Ctrl-C handler
    signal( SIGINT, SIG_DFL);

    // call the BaseServer to wait until a terminate is received
    ControlData *newCtrl = ControlAllocate();   // Current control data
    while(GetStatus() != BEP_TERMINATE)
    {
        WaitForChange();
        if(GetStatus() != BEP_TERMINATE)
        {
            if(IsSystemMonitorRegistered())
            {                          // Sample current values
                Log( LOG_FN_ENTRY, "Run Looped...\n");
                GetCurrentControlData( newCtrl);
                // update the control data
                UpdateControl(newCtrl);
            }
        }
    }

    Log( LOG_FN_ENTRY, "ChinaSystemMonitor Exit Run()\n");
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::GetCurrentControlData( ControlData *ctrl)
{
    ChinaControlData *jianglingCtrl = (ChinaControlData *)ctrl;

    Log( LOG_FN_ENTRY, "Enter China GetCurrentControlData()\n");

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
    ctrl->cableConnect = (bool)(ReadSubscribeData( PLC_CBL_CONNECT) == "1");
    ctrl->inputServerState = ReadSubscribeData( INPUT_SERVER_STATE);
    ctrl->powerSense = (bool)(ReadSubscribeData( PLC_POWER_SENSE) == "1");
    ctrl->rackFault = (bool)(ReadSubscribeData( PLC_RACK_FAULT) == "1");
    ctrl->wheelbaseInPosition = (bool)(ReadSubscribeData( WHEELBASE_IN_POSITION_TAG) == "1");
    ctrl->frontCradleInPosition = (bool)(ReadSubscribeData( FRONT_CRADLE_IN_POSITION_TAG) == "1");
    ctrl->rearCradleInPosition = (bool)(ReadSubscribeData( REAR_CRADLE_IN_POSITION_TAG) == "1");
    ctrl->zerospeed = (bool) (ReadSubscribeData(ZEROSPEED_TAG_TO_PLC) == "1");
    jianglingCtrl->rollDrivesEnabled = (bool) (ReadSubscribeData( ENABLE_ALL_ROLL_DRIVES) == "1");

    // check to determine if a reprint request was received
    // test must not be in progress, the rolls must be down and the current status
    // of the keypress must be F1
    if(!ctrl->testInProgress && ctrl->rollsDown && (ctrl->keyPress == "FunctionKey1"))
    {
        Log(LOG_DEV_DATA, "Request For Reprinting Received\n");
        ctrl->reprintRequest = true;
    }
    else
        ctrl->reprintRequest = false;        

    // If a test was just started and a valid vin does not exist, check the
    // ValidVINNeededForStart flag.  If it is not set, and the rolls are raised
    // indicate a valid vin is avaliable so the test can be started
    if((ctrl->startTestPb) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
       (!m_validVINNeededForStart) && (ctrl->rollsDown))
    {
        ctrl->vehVinReadStatus = VALID_VEHICLE_VIN;
    }
    // clear this flag, so diplay will show "push start button..." (korean: "select test")
    if(ctrl->rollsDown)
    {
        Log( LOG_FN_ENTRY, "Allowing the damm push button prompt");
        m_testWasRun = false;
    }


    // read the pendant keypress value and then clear it out of the internal buffer 
    // so if same key is pressed again, is is processed like new keypress
    ctrl->keyPress = ReadSubscribeData( PENDANT_KEY_PRESS);
    WriteSubscribeData(PENDANT_KEY_PRESS, std::string("Blank"));

    // default to the previous status
    ctrl->simInProgress = m_oldCtrl->simInProgress;
    ctrl->testRequested = m_oldCtrl->testRequested;
    ctrl->calKeyCycled = m_oldCtrl->calKeyCycled;
    ctrl->calConditionsCorrect = m_oldCtrl->calConditionsCorrect;

    Log( LOG_FN_ENTRY, "Exit China GetCurrentControlData()\n");
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::CheckTesting( ControlData *ctrl)
{
    ChinaControlData *jianglingCtrl = (ChinaControlData *)ctrl;

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
    Log( LOG_DEV_DATA, "\trollsUp %d\n", ctrl->rollsUp);
    Log( LOG_DEV_DATA, "\tstartVehicleTestFlag %d\n",m_startVehicleTestFlag);
    Log( LOG_DEV_DATA, "\trollDrivesEnabled %d\n", jianglingCtrl->rollDrivesEnabled);


    // Update this
    // We had a problem with a change from 1 to 0 in wheelbase kicking us off, then
    // we read the wheelbase position before it was set in the PLC.  This isn't a very 
    // elegant solution and should probably be changed.
    BposSleep(100);

    // Read the latest build data.  This is done to update the vehicle type on the driver's monitor.
    if((errno=m_ndbLock.Acquire()) == EOK)
    {
        CommandNdbData( READ_LATEST_BUILD_DATA_TAG, "1");
        m_ndbLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tSYSTEM MONITOR START Error locking ndb mutex: %s\n", strerror( errno));
    }
    // update wheelbase in position background color for driver's monitor
    // if wheelbase in position and wasn't last time
    // 
    // **************** NOTE ************************
    // We couldn't find how to add a tag to the driver's monitor to use for the background color
    // based on wheelbase position.  So, we had to pick one that wasn't being used.  This needs
    // to be changed when we get back.  (RW 09/18/03)
    // **********************************************
    if((ctrl->wheelbaseInPosition) && (!m_oldCtrl->wheelbaseInPosition))
    {
        if((errno=m_ndbLock.Acquire()) == EOK)
        {
            // Update this
            //            WriteNdbData(WHEELBASE_IN_POSITION_STATUS_TAG,std::string("Green"));
            WriteNdbData(DRIVER_NUMBER_DISPLAY_BGCOLOR_DATA_TAG,std::string("Green"));
            m_ndbLock.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tWheelbaseInPositionBGColor Error locking ndb mutex: %s\n", strerror( errno));
        }
    }
    // else if not in position, but was last time
    else if((!ctrl->wheelbaseInPosition) && (m_oldCtrl->wheelbaseInPosition))
    {
        if((errno=m_ndbLock.Acquire()) == EOK)
        {
            // Update this
            //           WriteNdbData(WHEELBASE_IN_POSITION_STATUS_TAG,std::string("Red"));
            WriteNdbData(DRIVER_NUMBER_DISPLAY_BGCOLOR_DATA_TAG,std::string("Red"));
            m_ndbLock.Release();
        }
        else
        {
            Log( LOG_ERRORS, "\tWheelbaseInPositionBGColor Error locking ndb mutex: %s\n", strerror( errno));
        }
    }

    // if there is not a machine fault and the cal switch is off
    if((!ctrl->machineFault)  && (!ctrl->calSwitch))
    {
        // if a test is in progress, make sure no prompting occurs from the SystemMonitor
        if(ctrl->testInProgress)
        {
            Log(LOG_DEV_DATA, "Nothing To Do, Test Is In Progress\n");
        }
        // if rolls were just raised, have the operator press the start cycle button if there is a valid vin scanned
        else if(ctrl->vehiclePresent && !ctrl->rollsUp && !m_oldCtrl->rollsUp && 
                !m_startVehicleTestFlag && !ctrl->vehVinReadStatus.compare("Green"))
        {
            DisplayPrompt(1, "PressButtonToStartTest");
        }
        // else if the start pb is depressed and a vehicle is present and the retainers are raised and not previously started
        else if(ctrl->startTestPb && (!m_oldCtrl->startTestPb) && ctrl->vehiclePresent && (!ctrl->rollsDown) && !m_startVehicleTestFlag)
        {
            if((errno=m_ndbLock.Acquire()) == EOK)
            {
                // load the vehicle build
                Log(LOG_DEV_DATA, "Loading the vehicle build\n");

                // ****** do we need to do this again?                
                //                if(CommandNdbData( READ_LATEST_BUILD_DATA_TAG, "1") == BEP_STATUS_SUCCESS)
                if(1)
                {
                    WriteNdbData("AcceptLamp",false);
                    WriteNdbData("FaultLamp",false);
                    RemovePrompt(1, "PressButtonToStartTest");
                    // start the test
                    CommandNdbData(START_VEHICLE_TEST_DATA_TAG, "1");
                    Log( LOG_DEV_DATA, "SYSTEM MONITOR STARTING A VEHICLE TEST! Issuing %s, %s\n", 
                         START_VEHICLE_TEST_DATA_TAG, "1");
                    m_startVehicleTestFlag = true;
                    // set this flag so display will not immediately revert to "press start" at end of test
                    Log( LOG_FN_ENTRY, "Start button is disabling the damn push start prompt\n");
                    m_testWasRun = true;

                }
                else
                    Log(LOG_ERRORS, "Unable to load the vehicle build, NOT STARTING TEST!\n");

                m_ndbLock.Release();
            }
            else
            {
                Log( LOG_ERRORS, "\tSYSTEM MONITOR START Error locking ndb mutex: %s\n", strerror( errno));
            }

        }
        // else if a test is just finished, check the cable connect
        else if((!ctrl->testInProgress) && m_oldCtrl->testInProgress)
        {                              // if the cable is still connected, prompt to operator to remove the cable
            if(ctrl->cableConnect)
            {
                DisplayPrompt(2, "DisconnectCable");
            }
            // else if the rolls are not down prompt the operator to lower the retainers
            else if(!ctrl->rollsDown)
            {
                if(m_machineType == "2400")    DisplayPrompt( 2,  "RaiseElevators", true);
                else                            DisplayPrompt( 2,  "LowerRetainers", true);

                Log(LOG_DEV_DATA,"Waiting for rollsdown or retest\n");
                while((ReadSubscribeData(ROLLS_DOWN_DATA_TAG) == "0") && 
                      (ReadSubscribeData(RETEST_DATA_TAG) == "0"))
                {
                    BposSleep(100);
                }
            }
        }
        // if the rolls are raised, and the cable was just disconnected
        else if(!ctrl->cableConnect && m_oldCtrl->cableConnect)
        {
            RemovePrompt(2, "DisconnectCable");
            if(!ctrl->rollsDown)
            {
                if(m_machineType == "2400")    DisplayPrompt( 2,  "RaiseElevators", true);
                else                            DisplayPrompt( 2,  "LowerRetainers", true);
            }
        }
        // if the cal and sim switches are off and not in the middle of a test and the rolls are up and not previously started
        //else if (!(ctrl.calSwitch) && !(ctrl.simSwitch) && !(ctrl.testInProgress) && ctrl.rollsDown)
        else if(!(ctrl->calSwitch) && !(ctrl->simSwitch) && !(ctrl->testInProgress) && (ctrl->rollsUp) && !m_startVehicleTestFlag)
        {
            // if vehicle present and we are ready for testing
            if((ctrl->vehiclePresent) && (!ctrl->startTestPb) && (!m_oldCtrl->startTestPb) && 
               ((m_machineType != "3600") || (jianglingCtrl->rollDrivesEnabled)))
            {
                // ews - do not display this prompt if we did a test, and the rolls are still up.
                if(!m_testWasRun)
                {
                    Log(LOG_FN_ENTRY, "Putting up the damn press button prmopt\n");
                    DisplayPrompt( 1,  "PressButtonToStartTest", "white");

                    // Clear test complete to the PLC
                    WriteNdbData("CompleteLamp",false);
                }

                // clear the old test results from the drivers monitor
                if((m_ndbLock.Acquire()) == EOK)
                {
                    WriteNdbData(GetDataTag("TestResultBox1"),std::string("White"));
                    WriteNdbData(GetDataTag("TestResultBox2"),std::string("White"));
                    WriteNdbData(GetDataTag("TestResultBox3"),std::string("White"));
                    WriteNdbData(GetDataTag("TestResultBox4"),std::string("White"));
                    WriteNdbData(GetDataTag("TestResultBox5"),std::string("White"));
                    WriteNdbData(GetDataTag("TestResultBox6"),std::string("White"));
                    WriteNdbData(GetDataTag("CycleTime"), std::string("0"));

                    m_ndbLock.Release();
                }
                else
                {
                    Log(LOG_ERRORS, "\tError locking ndb mutex for clearing old test results\n");
                }
            }
        }

        // If test just finished or retainers lowered, clear the start test tag
        if((!(ctrl->testInProgress) && m_oldCtrl->testInProgress) || ((ctrl->rollsDown) && !(m_oldCtrl->rollsDown)))
        {
            // if test started, stop
            if(BepServer::Read(START_TEST_DATA_TAG) != "0")
            {                          // clear the start test tag
                BepServer::Write( START_TEST_DATA_TAG, "0");
                Log( LOG_DEV_DATA, "Clearing %s\n", START_TEST_DATA_TAG);
            }
        }
    }
    else
        Log( LOG_ERRORS, "Machine Fault, Cant Do JACK SQUAT\n");


    // if a test was just completed, reset the drive axle
    if(!ctrl->testInProgress && m_oldCtrl->testInProgress)
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

    // when the rolls are lowered, clear the start testing flag
    if((ctrl->rollsDown) && !(m_oldCtrl->rollsDown))
    {
        // remove the prompt
        if(m_machineType == "2400")    RemovePrompt( 2,  "RaiseElevators");
        else                            RemovePrompt( 2,  "LowerRetainers");

        if(m_clearResultsOnRollsDown == true)
        {
            // clear the test result
            DisplayPrompt(1,"Blank","white");

            // clear the test results and cycle timer
            if((m_ndbLock.Acquire()) == EOK)
            {
                WriteNdbData(GetDataTag("TestResultBox1"),std::string("White"));
                WriteNdbData(GetDataTag("TestResultBox2"),std::string("White"));
                WriteNdbData(GetDataTag("TestResultBox3"),std::string("White"));
                WriteNdbData(GetDataTag("TestResultBox4"),std::string("White"));
                WriteNdbData(GetDataTag("TestResultBox5"),std::string("White"));
                WriteNdbData(GetDataTag("TestResultBox6"),std::string("White"));
                WriteNdbData(GetDataTag("CycleTime"), std::string("0"));

                m_ndbLock.Release();
            }
            else
            {
                Log(LOG_ERRORS, "\tError locking ndb mutex for clearing old test results\n");
            }
        }

        // remove the stat vehicle flag
        m_startVehicleTestFlag = false;
        // Instruct the operator to drive off the machine
        DisplayPrompt(1, "DriveOff");

        // change the VINReadStatus to red
        WriteNdbData("VINReadStatus","Red");
    }

    // if not running a test and a test has been run
    if(!ctrl->testInProgress && m_startVehicleTestFlag)
    {
        // check for the retest button
        if(ctrl->startTestPb || (ReadSubscribeData(START_SEQUENCE_PB_DATA_TAG) == "1"))
        {
            // remove the stat vehicle flag
            m_startVehicleTestFlag = false;
        }
    }

    Log( LOG_FN_ENTRY, "Exit CheckTesting()\n");
}

//-----------------------------------------------------------------------------
INT32 ChinaSystemMonitor::GetWheelSpeeds(float wheelSpeed[])
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

//-----------------------------------------------------------------------------
const INT32 ChinaSystemMonitor::HandlePulse(const INT32 code, const INT32 value)
{
    INT32 retVal = BEP_STATUS_SUCCESS;

    Log( LOG_FN_ENTRY, "Enter HandlePulse(%d, %d)\n", code, value);

    switch(code)
    {
        case SYS_MON_PULSE_CODE:
            if(value == UPDATE_TEST_CONDITIONS_PULSE)
            {
                ControlData *newCtrl = ControlAllocate();   // Current control data
                GetCurrentControlData( newCtrl);    // update the current control data
                // if vehicle present, retainers raised, check test conditions
                Log(LOG_FN_ENTRY, "Handling The UPDATE_TEST_CONDITIONS_PULSE: vehiclePresent: %d, rollsDown: %d\n",
                    newCtrl->vehiclePresent, newCtrl->rollsDown);
                if(newCtrl->vehiclePresent && !newCtrl->rollsDown)
                {                      // Update the current test conditions, if one has not been previously started and the drive axle has not been selected
                    Log(LOG_FN_ENTRY, "ews - Checking m_startVehicleTestFlag");
                    if((m_startVehicleTestFlag == false) || (m_currentDriveAxle.empty()))
                        UpdateTestConditions(newCtrl);
                }
            }
            else
            {
                retVal = SystemMonitor::HandlePulse(code,value);
            }
            break;
        default:
            retVal = SystemMonitor::HandlePulse( code, value);
            break;
    }

    Log( LOG_FN_ENTRY, "Exit HandlePulse(%d, %d)\n", code, value);

    return( retVal);
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::UpdateTestConditions( ControlData *ctrl)
{
    Log(LOG_FN_ENTRY, "UpdateTestConditions() entry\n");
    // if the machine is not at zerospeed check to start a test and set the drive axle
    if(ctrl->zerospeed == false)
    {
        Log("Loss Of Zerospeed Detected\n");
        std::string value;
        printf("Value of the Read: %s\n", ReadNdbData(START_VEHICLE_TEST_DATA_TAG, value).c_str());

        // if not testing and a test has not been previously started (without lowering the rolls)
        if(( value == "0") && (m_startVehicleTestFlag == false))
        {                              // check the wheel speeds to determine if they are over 2 MPH
            Log("Test Not Started\n");
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
                        m_currentDriveAxle = "Front";
                    }
                    else
                    {
                        m_currentDriveAxle = "Rear";
                    }
                }
                else                   // all wheel drive selected
                {
                    m_currentDriveAxle = "All";
                }

                // first, lock the NDB
                if((errno=m_ndbLock.Acquire()) == EOK)
                {
                    // if a front wheel drive vehicle, update the PLC
                    if(m_currentDriveAxle == "Front")
                    {
                        WriteNdbData("FrontWheelDriveSelectedToPLC",true);
                        WriteNdbData("RearWheelDriveToPLC",false);
                    }
                    // else if it is a rear wheel drive vehicle, update the PLC
                    else if(m_currentDriveAxle == "Rear")
                    {
                        WriteNdbData("RearWheelDriveToPLC",true);
                        WriteNdbData("FrontWheelDriveSelectedToPLC",false);
                    }
                    else               // else if must be an awd vehicle
                    {
                        WriteNdbData("FrontWheelDriveSelectedToPLC",false);
                        WriteNdbData("RearWheelDriveSelectedToPLC",false);
                    }

                    // load the vehicle build
                    Log(LOG_DEV_DATA, "Loading the vehicle build\n");
                    if(CommandNdbData( READ_LATEST_BUILD_DATA_TAG, "1") == BEP_STATUS_SUCCESS)
                    {
                        // update the drive axle
                        Log(LOG_DEV_DATA, "Updated the DriveAxle: %s\n", m_currentDriveAxle.c_str());
                        WriteNdbData( DRIVE_AXLE_TAG, m_currentDriveAxle);

                        // start the test
                        CommandNdbData(START_VEHICLE_TEST_DATA_TAG, "1");
                        Log( LOG_DEV_DATA, "SYSTEM MONITOR STARTING A VEHICLE TEST! Issuing %s, %s\n", 
                             START_VEHICLE_TEST_DATA_TAG, "1");

                        m_startVehicleTestFlag = true;
                        // set this flag so display will not immediately revert to "press start" at end of test
                        Log( LOG_FN_ENTRY, "Disabling the damn push start prompt\n");
                        m_testWasRun = true;

                        // remove the press key prompt
                        RemovePrompt( 1,  "PressButtonToStartTest");
                    }
                    else
                        Log(LOG_ERRORS, "Unable to load the vehicle build, NOT STARTING TEST!\n");
                    // unlock the ndb interface
                    m_ndbLock.Release();
                }
                else
                {
                    Log( LOG_ERRORS, "\tCheckStartTest Error locking ndb mutex: %s\n", strerror( errno));
                }
            }
        }
        // else if a test is in progress and the drive axle has not been selected
        else if(m_currentDriveAxle.empty())
        {
            float wheelSpeeds[GetRollerCount()];
            GetWheelSpeeds(wheelSpeeds);    // get current wheelspeeds

            // if any of the speeds are over the minimum boost speed
            if((wheelSpeeds[0] > 2.0) || (wheelSpeeds[1] > 2.0) || 
               (wheelSpeeds[2] > 2.0) || (wheelSpeeds[3] > 2.0))
            {
                // if all wheel drive not selected
                if(ReadSubscribeData(GetDataTag("AllWheelDrive")) == "0")
                {
                    // if so, determine if the drive axle is front or rear
                    if((wheelSpeeds[0] > 2.0) || (wheelSpeeds[1] > 2.0))
                    {
                        m_currentDriveAxle = "Front";
                    }
                    else
                    {
                        m_currentDriveAxle = "Rear";
                    }
                }
                else                   // all wheel drive selected
                {
                    m_currentDriveAxle = "All";
                }

                // first, lock the NDB
                if((errno=m_ndbLock.Acquire()) == EOK)
                {
                    // if a front wheel drive vehicle, update the PLC
                    if(m_currentDriveAxle == "Front")
                    {
                        WriteNdbData("FrontWheelDriveSelectedToPLC",true);
                        WriteNdbData("RearWheelDriveSelectedToPLC",false);
                    }
                    // else if it is a rear wheel drive vehicle, update the PLC
                    else if(m_currentDriveAxle == "Rear")
                    {
                        WriteNdbData("RearWheelDriveToPLC",true);
                        WriteNdbData("FrontWheelDriveSelectedToPLC",false);
                    }
                    else               // else if must be an awd vehicle
                    {
                        WriteNdbData("FrontWheelDriveSelectedToPLC",false);
                        WriteNdbData("RearWheelDriveSelectedToPLC",false);
                    }

                    // update the drive axle
                    Log(LOG_DEV_DATA, "Updated the DriveAxle: %s\n", m_currentDriveAxle.c_str());
                    WriteNdbData( DRIVE_AXLE_TAG, m_currentDriveAxle);
                    // unlock the ndb interface
                    m_ndbLock.Release();
                }
                else
                {
                    Log( LOG_ERRORS, "\tCheckStartTest Error locking ndb mutex: %s\n", strerror( errno));
                }
            }
        }
    }

    Log(LOG_FN_ENTRY, "UpdateTestConditions() exit\n");
}

//-----------------------------------------------------------------------------
INT32 ChinaSystemMonitor::GetRollerCount( void)
{
    return(m_rollerCount);
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::SetRollerCount( INT32 rollerCount)
{
    m_rollerCount = rollerCount;
}

//-----------------------------------------------------------------------------
ControlData* ChinaSystemMonitor::ControlAllocate() const
{
    ChinaControlData *control = new ChinaControlData;

    return( (ControlData*)control);
}

//-----------------------------------------------------------------------------
void ChinaSystemMonitor::ControlFree(ControlData* &controlData) const
{
    ChinaControlData *control = (ChinaControlData *)controlData;
    if( NULL != control)
    {
        delete control;
        control = NULL;
    }
}

/*******************************************************************************
    Usage Block
    Information printed when a user types 'use xxxxx' at the QNX prompt
    
#ifdef __USAGE

China System Monitor Task

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
    - Registers name as 'rolls/ChinaSystemMonitor' by default
    - Must be run as Super User.
    - $Revision: 5 $
    - $Date: 8/07/06 1:53p $

#endif

*******************************************************************************/

