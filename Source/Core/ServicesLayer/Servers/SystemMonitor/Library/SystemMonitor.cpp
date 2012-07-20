//******************************************************************************
// Description:
//    System Monitor Task
//
//==============================================================================
//
//     Copyright © 1999 Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/SystemMonitor/Library/SystemMonitor.cpp $
// 
// 11    2/22/07 11:19a Rwiersem
// Changes for the 07022201 Core Release:
// 
// - Added SetSystemMonitorRegistered() and IsSystemMonitorRegistered().
// - Added a UseFaultServer configuration item.
// - Check if the fault server is in use before sending faults in
// CheckMachineStatus().
// 
// 10    1/18/07 3:11p Rwiersem
// Changes for the 07011901 core release:
//                                                    
// - Added the setting of plcSystemIsReady, lfMotorOperationalFault,
// rfMotorOperationalFault, lrMotorOperationalFault,
// rrMotorOperationalFault, 
// ltMotorOperationalFault, rtMotorOperationalFault, and motorCommFault.
// - Added a parameter PLCSendsPLCSystemIsReady to tell if the PLC sends a
// ready bit.  Added m_plcSendsPLCSystemIsReady to store the value of this
// parameter.
// - Read in the pendant key press value before processing it.
// - If the PLC sends a ready bit, check its value in
// CheckMachineStatus().
// - Added a priority level parameter to DisplayPrompt() and
// RemovePrompt().
// 
// 9     4/13/06 4:29a Cward
// Added an assignment operator and an Assign() method to the ControlData
// class Removed the Revision and Data tags from the USAGE block
//
// 2     3/20/06 11:00a Bmeinke
// Added an assignment operator and an Assign() method to the ControlData
// class
// Removed the VSS Revision and Data tags from the USAGE block
//
// 8     7/20/05 9:29p Cward
// New Control Data Object scheme. Changed BepServer Write calls so
// overloaded Write methods get called correctly.Wrapped BepTimer Start
// and Stop method calls in try/catch blocks
//
// 7     5/11/05 10:04p Cward
// Updated for 05051101 iteration
//
// 6     4/29/05 3:52p Cward
// Updates for April 29, 05 iteration.
//
// 5     4/29/05 8:00a Gswope
// Changed the call to the constructor of BepServer from BepServer(argc,argv)
// to just BepServer() because the BepServer(argc,argv) provided no
// additional functionality and was therefore removed.
//
// 5     4/13/05 7:01p Cward
// Updates for the 05041301 build
//
// 4     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 3     2/16/05 9:36p Cward
// Commented out defaulted argument in run method.
//
// 2     11/15/04 11:13a Bmeinke
// Added an abortPb member to the ControlData class
// Added checks for the Abort pushbutton from the PLC
//
// 2     7/21/04 5:35p Cward
// Updated Module comment header block
//
// 1     7/21/04 10:18a Cward
//
// 4     4/28/04 10:54p Cward
// Changes associated with build 040402801
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:42p Cward
//
// 4     10/24/03 1:51p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 3:07p Khykin
// Updated with changes from Flatrock.
//
// 1     6/27/03 8:21a Khykin
// Update from SysMon to SystemMonitor.
//
// 1     6/20/03 9:55a Khykin
// Initial checkin.
//
// 2     6/20/03 9:08a Khykin
// Ford updates.
//
// 1     6/12/03 3:58p Khykin
// Initial checkin.
//
// 3     5/19/03 12:26p Khykin
// Updated with new cable connect logic.
//
// 2     4/30/03 10:36a Khykin
// Added logic for reprinting of test results.
//
// 1     4/22/03 9:51a Khykin
// Kevins Working Checkin
//
// 2     4/07/03 9:30a Khykin
// Updating with latest code from Pit machine debug.
//     Revision 1.1.1.1  2003/02/15 00:06:34  tbochene
//
//     PooBag
//
//
// 1     1/27/03 7:18a Khykin
// Initial Checkin
//
// 1     1/18/03 8:13a Khykin
// Initial Checkin From DCTC
//
// 23    11/22/02 3:18p Khykin
// Adding the ability to start a test without a valid VIN.
//
// 22    10/21/02 12:15p Khykin
// Updating diagnostics and LC prompts.
//
// 21    10/07/02 3:41p Khykin
// Updating for compiler warnings.
//
// 20    10/07/02 7:00a Khykin
// Updating comments.
//
// 18    8/14/02 12:54p Skypig13
// Removed compiler warnings
//
// 17    7/31/02 12:12p Skypig13
// Latest files from pit machine
//
// 16    6/13/02 3:12p Dan.erickson
// Updated DISPLAY* macros.
//
// 15    5/24/02 3:12p Kh
//
// 14    4/29/02 4:51p Bmeinke
// Replaced VehiclePresentLatch with VEH_PRESENT_DATA_TAG
//
// 13    4/29/02 11:46a Skypig13
// Updated Subscribe() method
//
// 12    4/25/02 4:34p Bmeinke
// Added vehicle present hysterisis logic
//
// 11    4/24/02 5:09p Bmeinke
// Added support for telling GUI to switch to/from Loss Compensation
// screen
//
// 10    3/19/02 11:39a Kh
// Updating due to changes in the BepComponent.
//
// 9     3/15/02 1:56p Bmeinke
// Added support for loading a config file
//
// 8     2/27/02 3:15p Bmeinke
// Made m_dataBroker, m_promptServer and m_faultServer objects dynamically
// allocated so they will find their respective servers properly upon
// instantiation
// Added support for command line option -c to echo logger statements to
// the console
// Added a call to BepComponent::Initialize() to our Initialize() method
// Added a call to BepComponent::HandlePulse() to our HandlePulse() method
// for pulses we don't understand
// Cleaned up processing of Calibration logic
// Write LossCompensation status to PLC
//
// 7     2/19/02 8:09p Bmeinke
// Modified the PublishedTagList class so it can be used for Subscription
// data items as well as items we publish
// Added new rate argument to Read() (it was added to BepComponent)
// Modified AddSubscribedDataItem() method to check the return value of
// the INamedDataBroker::Subscribe() call
// Modified calls to m_publishedData.AddPublishedItem() due to changes in
// the PublishedTagList class
// Cleaned up calibration logic
// Changed "Calibrate" stuff to "LossCompensation" stuff
// Tell the PLC we are entering/leaving calibrate mode inside
// CheckLossCompensation()
// No longer spin a thread to perform notifications (NotificationList now
// uses a thread pool internally)
//
// 6     2/01/02 10:31p Brian.meinke
// Cleaned up command line argument processing
// Added cable connect logic
// Added support for the RESET command
//
// 5     1/09/02 9:30a Brian.meinke
// Changed call to IBepCommunication::Initialize() because its prototype
// changed
//
// 4     10/31/01 2:16p Brian.meinke
// Messaging now uses IBepCommunication objects instead of raw XML
// messages
//
// 3     10/17/01 2:36p Brian.meinke
// Changed SYSTEM_PULSE to SYS_MON_PULSE_CODE
//
// 2     10/09/01 4:20p Brian.meinke
// Call updateDriverMonitor last inside UpdateControl
// Update non-subscription based data items inside GetCurrentControlData
// based on previous values
// Added some informational  Log() calls
//
// 1     9/19/01 11:25p Brian.meinke
// System Monitor task (similar to PDOS house keeping task)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "SystemMonitor.h"
#include "ITestResultServer.h"

//////////////////////////////////////
// ControlData Class Implementation //
//////////////////////////////////////
ControlData* ControlData::Allocate()
{
    return( new ControlData);
}

ControlData::ControlData()
{
    vehVinReadStatus = string(17, ' ');
    startTestPb = false;
    startTestCommand = false;
    vehiclePresent = false;
    rollsDown = false;
    rollsUp = false;
    cableConnect6Pin = false;
    cableConnect8Pin = false;
    cableConnect16Pin = false;
    cableConnect = false;
    calSwitch = false;
    calSwitchToPLC = false;
    calCycle = false;
    calInProgress = false;
    calConditionsCorrect = false;
    calKeyCycled = false;
    simSwitch = false;
    simCycle = false;
    simInProgress = false;
    testStarted = false;
    testInProgress = false;
    testRequested = false;
    inputServerState = "Normal";
    keyPress = "";
    powerSense = false;
    rackFault = false;
    machineFault = false;
    reprintRequest = false;
    wheelbaseInPosition=false;
    vmeSystemReady=false;
    zerospeed=false;
    abortPb = false;
    plcSystemIsReady = false;
    lfMotorOperationalFault = false;
    rfMotorOperationalFault = false;
    lrMotorOperationalFault = false;
    rrMotorOperationalFault = false;
    ltMotorOperationalFault = false;
    rtMotorOperationalFault = false;

    motorCommFault = false;
}

ControlData::~ControlData()
{
}

ControlData& ControlData::operator=( const ControlData& rhs)
{
    return( Assign( rhs));
}

ControlData& ControlData::Assign( const ControlData& rhs)
{
    vehVinReadStatus = rhs.vehVinReadStatus;
    startTestPb = rhs.startTestPb;
    startTestCommand = rhs.startTestCommand;
    vehiclePresent = rhs.vehiclePresent;
    rollsDown = rhs.rollsDown;
    rollsUp = rhs.rollsUp;
    cableConnect6Pin = rhs.cableConnect6Pin;
    cableConnect8Pin = rhs.cableConnect8Pin;
    cableConnect16Pin = rhs.cableConnect16Pin;
    cableConnect = rhs.cableConnect;
    calSwitch = rhs.calSwitch;
    calSwitchToPLC = rhs.calSwitchToPLC;
    calCycle = rhs.calCycle;
    calInProgress = rhs.calInProgress;
    calConditionsCorrect = rhs.calConditionsCorrect;
    calKeyCycled = rhs.calKeyCycled;
    simSwitch = rhs.simSwitch;
    simCycle = rhs.simCycle;
    simInProgress = rhs.simInProgress;
    testStarted = rhs.testStarted;
    testInProgress = rhs.testInProgress;
    testRequested = rhs.testRequested;
    inputServerState = rhs.inputServerState;
    keyPress = rhs.keyPress;
    powerSense = rhs.powerSense;
    rackFault = rhs.rackFault;
    machineFault = rhs.machineFault;
    reprintRequest = rhs.reprintRequest;
    wheelbaseInPosition = rhs.wheelbaseInPosition;
    vmeSystemReady = rhs.vmeSystemReady;
    zerospeed = rhs.zerospeed;
    abortPb = rhs.abortPb;
    plcSystemIsReady = rhs.plcSystemIsReady;

    lfMotorOperationalFault = rhs.lfMotorOperationalFault;
    rfMotorOperationalFault = rhs.rfMotorOperationalFault;
    lrMotorOperationalFault = rhs.lrMotorOperationalFault;
    rrMotorOperationalFault = rhs.rrMotorOperationalFault;
    ltMotorOperationalFault = rhs.ltMotorOperationalFault;
    rtMotorOperationalFault = rhs.rtMotorOperationalFault;

    motorCommFault = rhs.motorCommFault;

    return( *this);
}

/////////////////////////////////////////
// System Monitor Class Implementation //
/////////////////////////////////////////

SystemMonitor::SystemMonitor() : BepServer(),
    m_dataBroker( NULL), m_promptServer( NULL), m_faultServer( NULL),
    m_faultInformation(NULL), m_oldCtrl(NULL), m_useRegisterMssg( true),
    m_vehiclePresHystTime( 0), m_lcSwitchHystTime(0), m_cableConnectHystTime(0),
    m_machineType("3600"),m_updateControl(NULL), m_cableDisconnectCounter(0), 
    m_abortOnAbortPb(false),m_sysMonRegistered(false), m_useFaultServer(false)
{
    SetProcessName( SYS_MON_NAME);
    SetDebug( false);
}

SystemMonitor::SystemMonitor( int argc, char *argv[]) : BepServer(),
    m_dataBroker( NULL), m_promptServer( NULL), m_faultServer( NULL),
    m_faultInformation(NULL), m_oldCtrl(NULL), m_useRegisterMssg( true), 
    m_vehiclePresHystTime( 0),m_lcSwitchHystTime(0), m_cableConnectHystTime(0), 
    m_machineType("3600"),m_updateControl(NULL) , m_disconnectAbort(true), 
    m_abortOnAbortPb(false),m_sysMonRegistered(false), m_useFaultServer(false)
{
    SetProcessName( SYS_MON_NAME);
    SetVerboseMask( (UINT32)-1);

    opterr = 0;

    int optId=0;
    while ((optId=getopt( argc, argv, "cdef:p:rv:")) != -1)
    {
        switch (optId)
        {
        case 'd':
            SetDebug( true);
            EnableLogger();
            Log( LOG_DEV_DATA, "Enable debug\n");
            break;
        case 'e':
        case 'c':
            SetLocalEcho( true);
            EnableLogger();
            Log( LOG_DEV_DATA, "Enable local echo\n");
            break;
        case 'f':
            if (optarg != NULL)
            {
                SetConfigFile( optarg);
                Log( LOG_DEV_DATA, "Use config file %s\n", optarg);
            }
            break;
        case 'p':
            if (optarg != NULL)
            {
                BposSetPriority( 0, BposReadInt( optarg));
                Log( LOG_DEV_DATA, "Set priority %s\n", optarg);
                break;
            }
        case 'r':
            m_useRegisterMssg = false;
            break;
        case 'v':
            if (optarg != NULL)
            {
                char *tmp;
                UINT32  verbose;
                if ((tmp=strstr( optarg, "$")) != NULL)         sscanf( optarg, "$%X", &verbose);
                else if ((tmp=strstr( optarg, "0x")) != NULL)   sscanf( optarg, "0x%X", &verbose);
                else if ((tmp=strstr( optarg, "0X")) != NULL)   sscanf( optarg, "0X%X", &verbose);
                else                                            sscanf( optarg, "%X", &verbose);
                SetVerboseMask( verbose);
                EnableLogger();
                Log(LOG_DEV_DATA, "Verbose: $%08X\n", GetVerboseMask());
            }
            break;
        default:
            if (optId == '?')                       optind++;
            else if (argv[ optind] == NULL)          optind++;
            else if (argv[optind][0] != '-')         optind++;
            else if ((argv[ optind][0] == '-') && (strlen(argv[ optind]) == 1)) optind++;
            break;
        }
        optarg = NULL;
    }
}

SystemMonitor::~SystemMonitor()
{
    if (GetStatus() != BEP_TERMINATE_STATUS)
        Terminate();
    // if the INDB exists, delete it
    if (m_dataBroker != NULL)    delete m_dataBroker;
    // if the IPromptServer exists, delete it
    if (m_promptServer != NULL)  delete m_promptServer;
    // if the IFaultServer exists, delete it
    if (m_faultServer != NULL)   delete m_faultServer;
    // if the changed semaphore exists, delete it
    if (m_updateControl)         delete m_updateControl;
    // if the fault information list exists, delete it
    if (m_faultInformation)      delete m_faultInformation;

    if( NULL != m_oldCtrl)       ControlFree( m_oldCtrl);
}

void SystemMonitor::Run(volatile bool *terminateFlag /*=NULL*/)
{
    Log( LOG_FN_ENTRY, "Enter Run()\n");

    const string        cfgFile( GetConfigFile());

    // initialize the change semaphore
    m_updateControl = new BepSemaphore(1, IsDebugOn(), "SystemMonitor");

    // Perform initialization if a configuration file was provided
    if (cfgFile.empty() == false)
    {
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
    while (GetStatus() != BEP_TERMINATE)
    {
        WaitForChange();
        if (GetStatus() != BEP_TERMINATE)
        {
            if (IsSystemMonitorRegistered())
            {   // Sample current values
                GetCurrentControlData( newCtrl);
                // update the control data
                UpdateControl(newCtrl);
            }
        }
    }

    Log( LOG_FN_ENTRY, "Exit Run()\n");
}

void SystemMonitor::Initialize(const XmlNode *document)
{
    Log( LOG_FN_ENTRY, "Enter Initialize()\n");

    // Allocate our control data structure (deleted in destructor via ControlFree())
    m_oldCtrl = ControlAllocate();

    // initialize the base BepServer
    BepServer::Initialize(document);

    // If we don't want to wait for the Register Message
    if (m_useRegisterMssg == false)
    {
        // Do registration stuff
        Register();
    }

    // Read vehicle present hysterisis time
    try
    {   // Make sure valid document to parse
        if (document)
        {   // if so, set up the timer
            const XmlNode *vpNode = document->getChild( XML_T("Setup"))->getChild( XML_T("VehiclePresentTime"));
            m_vehiclePresHystTime = atol( vpNode->getValue().c_str());
            Log( LOG_DEV_DATA, "Using VehiclePresHystTime = %f\n", m_vehiclePresHystTime);
            if (m_vehiclePresHystTime)
            {   // convert the time to seconds
                m_vehiclePresentTimer.SetPulseCode( SYS_MON_PULSE_CODE);
                m_vehiclePresentTimer.SetPulseValue( VEHICLE_PRESENT_PULSE);
                m_vehiclePresentTimer.Initialize(GetProcessName().c_str(), (unsigned long)(m_vehiclePresHystTime * 1000000),
												 (unsigned long)(m_vehiclePresHystTime * 1000000));
                m_vehiclePresentTimer.Stop();
            }

            try
            {   // set the flag to send an abort when the cable is disconnected during a test
                m_disconnectAbort = (bool) (document->getChild("Setup/DisconnectAbort")->getValue() == "1");
            }
            catch(...)
            {
                m_disconnectAbort = true;
            }
        }
        else
        {   // If no hysterisis specified, default to 0 seconds
            m_vehiclePresHystTime = 0;
        }
    }
    catch (XmlException &err)
    {   // If no hysterisis specified, default to 0 seconds
        m_vehiclePresHystTime = 0;
    }

    try
    {   // Make sure valid document to parse
        if (document)
        {   // if so, set up the timer
            const XmlNode *lcNode = document->getChild( XML_T("Setup"))->getChild( XML_T("LossCompensationSwitchTime"));
            m_lcSwitchHystTime = atol( lcNode->getValue().c_str());
            Log( LOG_DEV_DATA, "Using LCSwitchHystTime = %f\n", m_lcSwitchHystTime);
            if (m_lcSwitchHystTime)
            {   // convert the time to seconds
                m_lossCompensationTimer.SetPulseCode( SYS_MON_PULSE_CODE);
                m_lossCompensationTimer.SetPulseValue( LOSS_COMPENSATION_PULSE);
                m_lossCompensationTimer.Initialize(GetProcessName().c_str(), (unsigned long)(m_lcSwitchHystTime * 1000000),
												   (unsigned long)(m_lcSwitchHystTime * 1000000));
                m_lossCompensationTimer.Stop();
            }
        }
        else
        {   // If no hysterisis specified, default to 0 seconds
            m_lcSwitchHystTime = 0;
        }
    }
    catch (XmlException &err)
    {   // If no hysterisis specified, default to 0 seconds
        m_lcSwitchHystTime = 0;
    }

    try
    {   // Make sure valid document to parse
        if (document)
        {   // if so, set up the timer
            const XmlNode *ccNode = document->getChild( XML_T("Setup"))->getChild( XML_T("CableConnectTime"));
            m_cableConnectHystTime = atol( ccNode->getValue().c_str());
            Log( LOG_DEV_DATA, "Using CableConnectTime = %f\n", m_cableConnectHystTime);
            if (m_cableConnectHystTime)
            {   // convert the time to seconds
                m_cableConnectTimer.SetPulseCode( SYS_MON_PULSE_CODE);
                m_cableConnectTimer.SetPulseValue( CABLE_CONNECT_PULSE);
                m_cableConnectTimer.Initialize(GetProcessName().c_str(), (unsigned long)(m_cableConnectHystTime * 1000000),
											   (unsigned long)(m_cableConnectHystTime * 1000000));
                m_cableConnectTimer.Stop();
            }
        }
        else
        {   // If no hysterisis specified, default to 0 seconds
            m_cableConnectHystTime = 0;
        }
    }
    catch (XmlException &err)
    {   // If no hysterisis specified, default to 0 seconds
        m_cableConnectHystTime = 0;
    }

    try
    {   // Read the valid vin needed for start flag
        if (document)
        {   // if so, set up the timer
            const XmlNode *validVINFlag = document->getChild( XML_T("Setup"))->getChild( XML_T("ValidVINNeededForStart"));
            m_validVINNeededForStart = (validVINFlag->getValue() == "1");
        }
        else
        {   // default to vin needed
            m_validVINNeededForStart = 0;
        }
    }
    catch (XmlException &err)
    {   // If no flag specified, default to vin needed
        m_validVINNeededForStart = 0;
    }

    // get the fault information
    try
    {
        m_faultInformation = const_cast< XmlNode *> (document->getChild( XML_T("Setup"))->getChild( XML_T("SystemFaults"))->Copy());
    }
    catch (XmlException &err)
    {   // If no hysterisis specified, default to 0 seconds
        throw BepException("No SystemFaults node under Setup in the configuration file!\n");
    }

    // Get the parameter that indicates if we should abort if an abort push button is indicated by the PLC
    //   (This parameter was added to provide backwards support to machines that don't have an abort pushbutton
    //    and can't be sure the PLC code is written correctly to not set the abort pushbutton bit in the PLC map)
    try
    {
        m_abortOnAbortPb =
            (document->getChild( XML_T("Setup"))->getChild( XML_T("AbortOnAbortPb"))->getValue() == "1");
    }
    catch (XmlException &err)
    {
        m_abortOnAbortPb = false;
    }
    // Get the parameter that indicates if we should set a fault when PLCReady signal is lost
    //   (This parameter was added to provide backwards support to machines that don't have an HMI interface
    //    as the the PLC code in these systems will not send a PLCReady bit)
    try
    {
        m_plcSendsPLCSystemIsReady =
            (document->getChild( XML_T("Setup"))->getChild( XML_T("PLCSendsPLCSystemIsReady"))->getValue() == "1");
    }
    catch (XmlException &err)
    {
        m_plcSendsPLCSystemIsReady = false;
    }
    // Get the parameter that indicates if we should use the fault server to set 
    // and display specific machine faults
    try
    {
        m_useFaultServer = 
            atob(document->getChild("Setup/UseFaultServer")->getValue().c_str());
    }
    catch (XmlException &err)
    {
        m_useFaultServer = false;
    }

    // check to see if System Monitor is responsible for pulsing the retaining rolls to Re-Relax
    try
    {
        m_systemMonitorFrontReductionPressureAdjust = atob(document->getChild("Setup/SystemMonitorFrontReductionPressureAdjust")->getValue().c_str());
    }
    catch (XmlException &err)
    {
        m_systemMonitorFrontReductionPressureAdjust = false;
    }

    if (m_systemMonitorFrontReductionPressureAdjust)
    {
        // retrieve the RetRolls ReRelax pulse timer time
        try
        {
            if (document)
            {
                const XmlNode *node = document->getChild( XML_T("Setup"))->getChild( XML_T("ReRelaxRetRollsPulseWidth"));
                m_ReRelaxRetRollsPulseTime = atol( node->getValue().c_str());
            }
            else
            {
                Log(LOG_ERRORS, "Warning! No ReRelax Ret Rolls pulse time specified, Defaulting to 1sec\n");
                m_ReRelaxRetRollsPulseTime = 1000;
            }
        }
        catch (XmlException &err)
        {
            Log(LOG_ERRORS, "Warning! Exception reading ReRelax Ret Rolls pulse time, Defaulting to 1sec\n");
            m_ReRelaxRetRollsPulseTime = 1000;
        }

        // set the update time
        Log( LOG_DEV_DATA, "Using ReRelax Ret Rolls pulse time = %f\n", m_ReRelaxRetRollsPulseTime);
        if (m_ReRelaxRetRollsPulseTime)
        {   // convert the time to seconds
            m_ReRelaxRetRollsPulseTimer.SetPulseCode( SYS_MON_PULSE_CODE);
            m_ReRelaxRetRollsPulseTimer.SetPulseValue( STOP_RERELAX_RETROLLS_PULSE); 
            m_ReRelaxRetRollsPulseTimer.Initialize(GetProcessName().c_str(), 
                                             NULL, (unsigned long)(mSEC_nSEC(m_ReRelaxRetRollsPulseTime )));
            m_ReRelaxRetRollsPulseTimer.Stop();
        }
    }

    // check to see if System Monitor is responsible for adjusting wheelbase
    try
    {
        m_systemMonitorWheelbaseAdjust = atob(document->getChild("Setup/SystemMonitorWheelbaseAdjust")->getValue().c_str());
    }
    catch (XmlException &err)
    {
        m_systemMonitorWheelbaseAdjust = false;
    }

    if (m_systemMonitorWheelbaseAdjust)
    {
        // retrieve the wheelbase pulse timer time
        try
        {
            if (document)
            {
                const XmlNode *node = document->getChild( XML_T("Setup"))->getChild( XML_T("MoveWheelbasePulseWidth"));
                m_wheelbasePulseTime = atol( node->getValue().c_str());
            }
            else
            {
                Log(LOG_ERRORS, "Warning! No Wheelbase pulse time specified, Defaulting to 1sec\n");
                m_wheelbasePulseTime = 1000;
            }
        }
        catch (XmlException &err)
        {
            Log(LOG_ERRORS, "Warning! Exception reading Wheelbase pulse time, Defaulting to 1sec\n");
            m_wheelbasePulseTime = 1000;
        }

        // set the update time
        Log( LOG_DEV_DATA, "Using Wheelbase pulse time = %f\n", m_wheelbasePulseTime);
        if (m_wheelbasePulseTime)
        {   // convert the time to seconds
            m_wheelbasePulseTimer.SetPulseCode( SYS_MON_PULSE_CODE);
            m_wheelbasePulseTimer.SetPulseValue( STOP_WB_ADJUST_PULSE); 
            m_wheelbasePulseTimer.Initialize(GetProcessName().c_str(), 
                                             NULL, (unsigned long)(mSEC_nSEC(m_wheelbasePulseTime )));
            m_wheelbasePulseTimer.Stop();
        }
    }

    Log( LOG_FN_ENTRY, "Exit Initialize()\n");
}

const std::string SystemMonitor::Publish(const XmlNode *node)
{
    Log( LOG_FN_ENTRY, "Enter Publish()\n");

    std::string status = BepServer::Publish(node);
    // if a successful publish update the system
    if (status == BEP_SUCCESS_RESPONSE)
        NotifyDataChanged();
    // Only do special stuff if publish was successful
    if (status == BEP_SUCCESS_RESPONSE)
    {
        if (node->getName() == "FrontReductionPressure")
        {
            if (GetSystemMonitorFrontReductionPressureAdjust())
            {
                Log(LOG_DEV_DATA,"New Front Reduction Pressure received telling PLC to ReRelax");
                StartReRelaxRetRollsPulse();
            }
            else
            {
                Log(LOG_DEV_DATA, "SystemMonitor not responsible for adjusting Front Reduction Pressure, ignoring FrontReductionPressure publish!\n");
            }
        }
        else if (node->getName() == "WheelbasePositionInchesX10")
        {
            if (GetSystemMonitorWheelbaseAdjust())
            {
                Log(LOG_DEV_DATA,"Wheelbase/Setup received telling PLC to adjust");
                StartWheelbaseAdjust();
            }
            else
            {
                Log(LOG_DEV_DATA, "SystemMonitor not responsible for adjusting wheelbase, ignoring WheelbasePositionInchesX10 publish!\n");
            }
        }
    }

    Log( LOG_FN_ENTRY, "Exit Publish() %s\n", status.c_str());

    return( status);
}

//-----------------------------------------------------------------------------
const std::string SystemMonitor::Write(const std::string &tag, const std::string &value)
{
    XmlNode temp(tag, value);
    return(Write(&temp));
}

//-----------------------------------------------------------------------------
const std::string SystemMonitor::Write(const XmlNode *node)
{
    Log( LOG_FN_ENTRY, "Enter Write()\n");

    // update the managed data
    std::string status = BepServer::Write(node);

    // if a successful publish update the system
    if (status == BEP_SUCCESS_RESPONSE)
        NotifyDataChanged();

    Log( LOG_FN_ENTRY, "Exit Write() %s\n", status.c_str());

    return(status);
}

//-----------------------------------------------------------------------------
const INT32 SystemMonitor::HandlePulse(const INT32 code, const INT32 value)
{
    ControlData *newCtrl = ControlAllocate();       // Current control data
    INT32 retVal = BEP_STATUS_SUCCESS;

    Log( LOG_FN_ENTRY, "Enter HandlePulse(%d, %d)\n", code, value);

    switch (code)
    {
    case SYS_MON_PULSE_CODE:
        switch( value)
        {
        // If this is our own pulse telling us to update our control logic
        case PROCESS_CONTROL_PULSE:
            NotifyDataChanged();
            break;
        case VEHICLE_PRESENT_PULSE:
            GetCurrentControlData( newCtrl);    // update the current control data
            CheckVehiclePresent(newCtrl, true); // validate that the vehicle is present
            break;
        case LOSS_COMPENSATION_PULSE:
            GetCurrentControlData( newCtrl);        // update the current control data
            CheckLossCompensation(newCtrl, true);   // validate that the vehicle is present
            break;
        case CABLE_CONNECT_PULSE:
            GetCurrentControlData( newCtrl);        // update the current control data
            CheckCableConnect(newCtrl, true);       // validate that the cable is connected
            break;
        case STOP_WB_ADJUST_PULSE:
            StopWheelbaseAdjust();         // Make sure to disable the start wheelbase adjust
            break;
        case STOP_RERELAX_RETROLLS_PULSE:
            StopReRelaxRetRollsPulse();    // Make sure to disable the start ReRelaxRetRolls pulse
            break;
        default:
            retVal = BEP_STATUS_FAILURE;
        break;
        }
        break;
    case SYSTEM_PULSE:
        switch( value)
        {
        case TERMINATE_PULSE:
            Terminate();
            break;
        case RELOAD_PULSE:
            ReloadConfiguration();
            break;
        default:
            retVal = BepServer::HandlePulse( code, value);
            break;
        }
    default:
        retVal = BepServer::HandlePulse( code, value);
        break;
    }

    ControlFree( newCtrl);

    Log( LOG_FN_ENTRY, "Exit HandlePulse(%d, %d)\n", code, value);

    return( retVal);
}

void SystemMonitor::Terminate(void)
{
    Log( LOG_FN_ENTRY, "Enter SystemMonitor::Terminate()\n");
    BepServer::Terminate();
    NotifyDataChanged();
    Log( LOG_FN_ENTRY, "Exit SystemMonitor::Terminate()\n");
}

void SystemMonitor::UpdateControl(ControlData *ctrl)
{

    Log( LOG_FN_ENTRY, "Enter UpdateControl()\n");
    // if the server is registered, update
    if (IsSystemMonitorRegistered())
    {   // Check the machine status to determine if it is good (should be done second)
        CheckMachineStatus( ctrl);
        // Check status of Vehicle present
        CheckVehiclePresent( ctrl);
        // Check cable connect status change
        CheckCableConnect( ctrl);
        // Check ABORT logic
        CheckAbort( ctrl);
        // Check test start logic
        CheckTesting( ctrl);
        // Check calibrate logic
        CheckLossCompensation( ctrl);
        // Update control related prompts
        UpdateDriverMonitor( ctrl);
        // Check for reprint request
        CheckReprintRequest( ctrl);
        // Update data
        *m_oldCtrl = *ctrl;
    }

    Log( LOG_FN_ENTRY, "Exit UpdateControl()\n");
}

void SystemMonitor::GetCurrentControlData( ControlData *ctrl)
{
    Log( LOG_FN_ENTRY, "Enter GetCurrentControlData()\n");

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
    ctrl->simSwitch = false;
    ctrl->simCycle = false;
    ctrl->cableConnect6Pin = (bool)(ReadSubscribeData( CBL_CONNECT_6_PIN) == "1");
    ctrl->cableConnect8Pin = (bool)(ReadSubscribeData( CBL_CONNECT_8_PIN) == "1");
    ctrl->cableConnect16Pin = (bool)(ReadSubscribeData( CBL_CONNECT_16_PIN) == "1");
    ctrl->inputServerState = ReadSubscribeData( INPUT_SERVER_STATE);
    ctrl->powerSense = (bool)(ReadSubscribeData( PLC_POWER_SENSE) == "1");
    ctrl->rackFault = (bool)(ReadSubscribeData( PLC_RACK_FAULT) == "1");
    ctrl->wheelbaseInPosition = (bool) (ReadSubscribeData( WHEELBASE_IN_POSITION_TAG) == "1");
    ctrl->vmeSystemReady = (bool) (ReadSubscribeData( VME_SYSTEM_READY_TAG) == "1");
    ctrl->abortPb = (bool)(ReadSubscribeData( GetDataTag("AbortTestFromPLC")) == "1");
    // Cable connect is any of the possible cable connection modes
    ctrl->cableConnect = atob(ReadSubscribeData(PLC_CBL_CONNECT).c_str()) ||
        atob(ReadSubscribeData( GetDataTag("WirelessCableConnected")).c_str());
    ctrl->plcSystemIsReady = (bool)(ReadSubscribeData( PLC_SYSTEM_IS_READY_TAG) == "1");

    // If a test was just started and a valid vin does not exist, check the
    // ValidVINNeededForStart flag.  If it is not set, and the rolls are raised
    // indicate a valid vin is avaliable so the test can be started
    if ( (ctrl->startTestPb) && (ctrl->vehVinReadStatus != VALID_VEHICLE_VIN) &&
         (!m_validVINNeededForStart) && (ctrl->rollsDown) )
    {
        ctrl->vehVinReadStatus = VALID_VEHICLE_VIN;
    }
    
    ctrl->keyPress = ReadSubscribeData( PENDANT_KEY_PRESS);

    // check to determine if a reprint request was received
    // test must not be in progress, the rolls must be down and the current status
    // of the keypress must be F1
    if(!ctrl->testInProgress && ctrl->rollsDown && (ctrl->keyPress == "FunctionKey1"))
    {
        Log(LOG_DEV_DATA, "Request For Reprinting Received\n");
        ctrl->reprintRequest = true;
    }
    else
    {
        ctrl->reprintRequest = false;
    }

    // clear the keypress so if tjhe same key is pressed again, it is processed 
    // like a new keypress
    WriteSubscribeData(PENDANT_KEY_PRESS, std::string("Blank"));

    // default to the previous status
    ctrl->simInProgress = m_oldCtrl->simInProgress;
    ctrl->testRequested = m_oldCtrl->testRequested;
    ctrl->calKeyCycled = m_oldCtrl->calKeyCycled;
    ctrl->calConditionsCorrect = m_oldCtrl->calConditionsCorrect;

    Log( LOG_FN_ENTRY, "Exit GetCurrentControlData()\n");
}

void SystemMonitor::CheckMachineStatus( ControlData *ctrl)
{
    Log(LOG_FN_ENTRY, "SystemMonitor::CheckMachineStatus() %d, %d, %d\n", 
        ctrl->powerSense, ctrl->rackFault, ctrl->plcSystemIsReady);

    if((ctrl->powerSense == false) || (ctrl->rackFault == true) || 
       (ctrl->plcSystemIsReady == false && m_plcSendsPLCSystemIsReady == true))
    {   
        ctrl->machineFault = true;     // set the machine fault to true
        if(ctrl->vmeSystemReady) WriteNdbData(VME_SYSTEM_READY_TAG, false);

        if (m_useFaultServer)
        {
            std::string fault("");
            // if lost plc ready
            if ((ctrl->plcSystemIsReady == false) && 
                (m_oldCtrl->plcSystemIsReady == true) && 
                (m_plcSendsPLCSystemIsReady == true))
            {
                fault = "PLCSystemIsReady";
                BepServer::Write(ABORT_DATA_TAG,"1");
                DisplayPrompt(2,"PLCSystemIsReady");
                Log(LOG_DEV_DATA,"PLC System Not Ready\n");
            }
            // if power sense is lost, display the machine fault
            else if((ctrl->powerSense==false) && (m_oldCtrl->powerSense==true))
            {
                fault = "PowerSense";
            }
            // if a rack fault is present, display the machine fault
            else if((ctrl->rackFault==true) && (m_oldCtrl->rackFault==false))
            {
                fault = "RackFault";
            }

            // if a fault exists
            if (!fault.empty())
            {   // get the fault info and update the system
                const XmlNode *faultNode = m_faultInformation->getChild(fault);
                std::string response;
                m_faultServer->SetFault(
                    faultNode->getAttribute("Name")->getValue(),
                    faultNode->getAttribute("Description")->getValue(),
                    GetProcessName(),response,true,
                    atoi(faultNode->getAttribute("Severity")->getValue().c_str()));
                Log(LOG_ERRORS, "Machine Fault: %s, %s\n", 
                    faultNode->getName().c_str(),
                    faultNode->getAttribute("Description")->getValue().c_str());
            }
        }
    }
    else
    {
        // if no faults set the machine fault to false
        ctrl->machineFault = false;
        if(!ctrl->vmeSystemReady) WriteNdbData(VME_SYSTEM_READY_TAG, true);

        if (m_useFaultServer)
        {
            // if we just regained plcSystemIsReady
            std::string fault;
            if ((ctrl->plcSystemIsReady == true) && 
                (m_oldCtrl->plcSystemIsReady == false) && 
                (m_plcSendsPLCSystemIsReady == true))
            {
                fault = "PLCSystemIsReady";
                BepServer::Write(ABORT_DATA_TAG, "0");
                RemovePrompt(2,"PLCSystemIsReady");
                Log(LOG_DEV_DATA,"PLC System is Ready\n");
            }
            // if we just regained power sense
            else if((ctrl->powerSense==true) && (m_oldCtrl->powerSense==false))
            {
                fault = "PowerSense";
            }
            // if we just lost the rack fault
            else if((ctrl->rackFault==false) && (m_oldCtrl->rackFault==true))
            {
                fault = "RackFault";
            }
                
            // if a fault just cleared
            if (!fault.empty())
            {   // get the fault info and update the system
                const XmlNode *faultNode = m_faultInformation->getChild(fault);
                std::string response;
                m_faultServer->ClearFault(
                    faultNode->getAttribute("Name")->getValue(),response,true);
                Log(LOG_ERRORS, "Machine Fault Cleared: %s, %s\n", 
                    faultNode->getName().c_str(),
                    faultNode->getAttribute("Description")->getValue().c_str());
            }
        }
    }
    Log( LOG_FN_ENTRY, "SystemMonitor::CheckMachineStatus() exit\n");
}

void SystemMonitor::CheckVehiclePresent( ControlData *ctrl, bool checkStatus /* = false */)
{
    Log( LOG_FN_ENTRY, "Enter CheckVehiclePresent() %d\n", ctrl->vehiclePresent);

    // if vehicle presence lost
    if (!ctrl->vehiclePresent && m_oldCtrl->vehiclePresent)
    {
        Log( LOG_DEV_DATA, "Vehicle presence lost\n");
        XmlNode dataNode(  VEH_PRESENT_DATA_TAG, "0");
        BepServer::Write( &dataNode);
        // if vehicle presence filter set, stop the timer
        if (m_vehiclePresHystTime)
        {
            try
            {
                m_vehiclePresentTimer.Stop();
            }
            catch( BepException &err)
            {
                Log( LOG_ERRORS, "Error stopping vehicle presence debounce timer: %s\n",
                     err.GetReason());
            }
        }
    }
    // else if vehicle is present
    else if (ctrl->vehiclePresent)
    {   // if a hysteresis time set
        if (m_vehiclePresHystTime)
        {   // determine if the switch has been turned on
            if (!m_oldCtrl->vehiclePresent)
            {
                Log( LOG_DEV_DATA, "Debouncing Vehicle is present\n");
                try
                {
                m_vehiclePresentTimer.Stop();       // ensure the timer is stopped
                m_vehiclePresentTimer.Start();      // start the timer
            }
                catch( BepException &err)
                {
                    Log( LOG_ERRORS, "Error starting vehicle presence debounce timer: %s\n",
                         err.GetReason());
                }
            }
            // else if VP timer expired, update the VP
            else if (checkStatus == true)
            {
                Log( LOG_DEV_DATA, "Vehicle is present\n");
                XmlNode dataNode(  VEH_PRESENT_DATA_TAG, "1");
                BepServer::Write( &dataNode);
                try
                {
                m_vehiclePresentTimer.Stop();
            }
                catch( BepException &err)
                {
                    Log( LOG_ERRORS, "Error stopping vehicle presence debounce timer: %s\n",
                         err.GetReason());
                }
            }
        }
        // else if not debouncing and the switch just turned on
        else if (!m_oldCtrl->vehiclePresent)
        {
            XmlNode dataNode(  VEH_PRESENT_DATA_TAG, "1");
            BepServer::Write( &dataNode);
        }
    }

    Log( LOG_FN_ENTRY, "Exit CheckVehiclePresent()\n");
}

void SystemMonitor::CheckCableConnect( ControlData *ctrl, bool checkStatus /* = false */)
{
    Log( LOG_FN_ENTRY, "Enter CheckCableConnect()\n");

    bool cableIsConnected = false;  // flag that indicates that the cable is connected

    // if cable connect is lost, report immediately
    if (!ctrl->cableConnect && m_oldCtrl->cableConnect)
    {
        Log( LOG_DEV_DATA, "Cable Connect Lost\n");
        // tell subscribers that the cable is disconnected
        XmlNode dataNode(CBL_CONNECT_DATA_TAG, "0");
        BepServer::Write( &dataNode);
        // tell the plc the cable is disconnected
        WriteNdbData( PLC_CBL_CONNECT_DATA_TAG, false);
        // if cable connect filter set, stop the timer
        if (m_cableConnectHystTime)  m_cableConnectTimer.Stop();
        // if there is a test in progress and the cable is not connected but it was
        // previously connected and not currently aborted
        if(ctrl->testInProgress && (Read(ABORT_DATA_TAG) == "0"))
        {
            Log(LOG_ERRORS, "ERROR: Cable disconnected in the middle of a test, abort\n");
            if(m_disconnectAbort)
            {
                BepServer::Write( ABORT_DATA_TAG, "1");
                DisplayPrompt(1, "CableDisconnected", "Yellow");
                DisplayPrompt(2, "BrakeToStop", "Yellow");
            }
        }
    }
    // else if the cable is connected
    else if (ctrl->cableConnect)
    {   // if the cable connect should be debounced
        if (m_cableConnectHystTime)
        {   // if the state of the connect changed, reset the timer
            if(!m_oldCtrl->cableConnect)
            {
                Log( LOG_DEV_DATA, "Cable Connect State Changed\n");
                m_cableConnectTimer.Stop();     // ensure the timer is reset
                m_cableConnectTimer.Start();    // start the timer
            }
            // else if the debounce timer has elapsed, the status is valid
            else if (checkStatus == true)
            {
                Log( LOG_DEV_DATA, "Cable Connect Detected\n");
                cableIsConnected = true;
                m_cableConnectTimer.Stop();     // stop the timer
            }
        }
        // else signal cable connected
        else
        {   // if new cable connect detected
            if(ctrl->cableConnect && !m_oldCtrl->cableConnect)
            {
                Log( LOG_DEV_DATA, "Cable Connect Detected (No Debounce)\n");
                cableIsConnected = true;
            }
        }

        // if the cable has been detected as connected
        if(cableIsConnected)
        {
            Log( LOG_DEV_DATA, "Cable Connect Detected\n");
            // tell subscribers that the cable is connected
            BepServer::Write(CBL_CONNECT_DATA_TAG, "1");
            // tell the plc the cable is connected
            WriteNdbData( PLC_CBL_CONNECT_DATA_TAG, true);
        }
    }

    Log( LOG_FN_ENTRY, "Exit CheckCableConnect(): %d\n", cableIsConnected);
}

void SystemMonitor::CheckAbort( ControlData *ctrl)
{
    Log( LOG_FN_ENTRY, "Enter CheckAbort()\n");

    if((ReadSubscribeData(INVALID_LICENSE_FATAL_FAULT_TAG) == "1") && (Read(ABORT_DATA_TAG) == "0"))
    {
        DisplayPrompt( 1,  INVALID_LICENSE_FATAL_FAULT_TAG);
        BepServer::Write( ABORT_DATA_TAG, "1");
    }
    // If currently testing something
    else if ((ctrl->testInProgress) || (ctrl->calInProgress))
    {   // Log the current state
        Log(LOG_DEV_DATA, "\tInputServer state:    %s", ctrl->inputServerState.c_str());
        Log(LOG_DEV_DATA, "\tKeyPress         :    %s", ctrl->keyPress.c_str());
        Log(LOG_DEV_DATA, "\tAbort PB         :    %d", ctrl->abortPb);
        Log(LOG_DEV_DATA, "\tABORT_DATA_TAG   :    %s", Read(ABORT_DATA_TAG).c_str());
        // if the abort has been pressed on the pendant (TestResultState = Abort or NormalState = 0) and currently not aborted
        if ((((ctrl->inputServerState == INPUT_SERVER_TEST_RESULT_STATE) && !(ctrl->keyPress.compare(TEST_ABORT))) ||
             ((ctrl->inputServerState == INPUT_SERVER_NORMAL_STATE) &&  !(ctrl->keyPress.compare("0"))) ||
             ((ctrl->abortPb == true) && m_abortOnAbortPb)) &&
            (Read(ABORT_DATA_TAG) == "0"))
        {
            Log(LOG_ERRORS, "ERROR: Driver ABORTED The Test\n");
			ITestResultServer testResult;
			testResult.Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
			string response;
			testResult.WriteTestResult("OperatorAbort", ABORT_DATA_TAG, GetProcessName(), "Operator pressed the Abort Button", response, true, "0000");
            BepServer::Write( ABORT_DATA_TAG, "1");
        }
        // else if the retainers are lowered during a test, abort
        else if (ctrl->testInProgress && ctrl->rollsDown && m_oldCtrl->rollsDown)
        {
            Log(LOG_ERRORS, "ERROR: Driver Lowered Retainers and ABORTED The Test\n");
//          BepServer::Write( ABORT_DATA_TAG, "1");
        }
        if (( ((ctrl->inputServerState == INPUT_SERVER_TEST_RESULT_STATE) && !(ctrl->keyPress.compare(PENDANT_START_TEST))) ||
              ((ctrl->inputServerState == INPUT_SERVER_NORMAL_STATE) &&  !(ctrl->keyPress.compare("Left")) || 
               ctrl->startTestPb) && Read(ABORT_DATA_TAG) == "1"))

        {//Pass button pressed in abort situation - vehicle test will sequence, so we should clear the test result
			ITestResultServer testResult;
			testResult.Initialize(TEST_RESULT_SERVER_NAME, "Client", IsDebugOn());
			string response;
			testResult.WriteTestResult("OperatorAbort", PENDANT_START_TEST, GetProcessName(), "", response, true, "0000");
        }
    }
    // else if the rolls are lowered and not in a test, clear the abort
    else if (ctrl->rollsDown && (ReadSubscribeData(INVALID_LICENSE_FATAL_FAULT_TAG) != "1"))
    {
        // and an abort occurred, clear the ABORT condition
        if (Read(ABORT_DATA_TAG) == "1")
            BepServer::Write( ABORT_DATA_TAG, "0");
    }

//    printf("SysMon Check Abort: %s\n", ReadSubscribeData("LicenseInvalidFatalFault").c_str());
    Log( LOG_FN_ENTRY, "Exit CheckAbort()\n");
}

void SystemMonitor::UpdateDriverMonitor( ControlData *ctrl)
{
    Log( LOG_FN_ENTRY, "Enter UpdateDriverMonitor()\n");

    // if simulator switch turned ON
    if (ctrl->simSwitch)
    {   // because this has not been implemented
        DisplayPrompt( 1,  "SimulationSwitchOff");
    }
    // Sim key just turned off
    else if (m_oldCtrl->simSwitch && !ctrl->simSwitch)
    {   // Remove prompt telling operator to cycle switch
        RemovePrompt( 1,  "SimulationSwitchOff");
    }
    // if the LC switch is on or being cycled Note: will loose the on state when cycled
    else if (!ctrl->calSwitchToPLC && m_oldCtrl->calSwitchToPLC)
    {
        RemovePrompt( 1,  "LossCompensationInProgress");
        // if the rolls are not down, prompt to lower
        if (!ctrl->rollsDown)
        {   // perform check for machine type
            if (m_machineType == "2400")    DisplayPrompt( 2,  "RaiseElevators", false);
            else                            DisplayPrompt( 2,  "LowerRetainers", false);
        }
        // else remove all prompts
        else
        {   // Remove all secondary prompts
            DisplayPrompt( 2,  "Blank");
        }
    }
    // if in a LC operation
    else if (ctrl->calSwitchToPLC || ctrl->calCycle)
    {
        // if a LC test is not being performed
        if (!ctrl->testInProgress)
        {
            // if the cable is connected and a test has not been started
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
                if (m_machineType == "2400")    DisplayPrompt( 2,  "LowerElevators", false);
                else                            DisplayPrompt( 2,  "RaiseRetainers", false);
            }
            // else if not already prompted to cycle because conditions are not correct, prompt to cycle the key
            else if (!ctrl->calConditionsCorrect && ctrl->rollsUp)
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
                    if (m_machineType == "2400")    DisplayPrompt( 2,  "RaiseElevators", false);
                    else                            DisplayPrompt( 2,  "LowerRetainers", false);
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
            if (m_machineType == "2400")    RemovePrompt( 2,  "RaiseElevators");
            else                            RemovePrompt( 2,  "LowerRetainers");
        }
        // reset the calibrate conditions when the rolls are lowered
        ctrl->calConditionsCorrect = false;
        ctrl->calKeyCycled = false;
    }

    Log( LOG_FN_ENTRY, "Exit UpdateDriverMonitor()\n");
}

void SystemMonitor::CheckReprintRequest(  ControlData *ctrl)
{
    // if a reprint was requested
    if(ctrl->reprintRequest && m_oldCtrl->reprintRequest)
    {   // write TestResultToPrint with the current test result value
        string testResult = Read(TEST_RESULT_TAG);
        Log(LOG_DEV_DATA, "Reprinting ticket for test result: %s\n", testResult.c_str());
        WriteNdbData(TEST_RESULT_TO_PRINT_TAG, testResult);
        ctrl->reprintRequest = false;
    }
}

void SystemMonitor::CheckLossCompensation( ControlData *ctrl, bool checkStatus  /*= false*/)
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
    if (!ctrl->machineFault)
    {   // if a change was verified, update the system
        if (updateSwitchStatus)
        {
            // enable/disable LC mode to the PLC
            WriteNdbData( LOSSCOMPENSATION_TAG, ctrl->calSwitch);
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
        // else if not running a test, Check for LC
        else if (! ctrl->testInProgress)
        {   // if not simulating, no vehicle present, and the cable is not connected
            // check to perform LC test
            if (!(ctrl->simInProgress) && !(ctrl->cableConnect) && !(ctrl->vehiclePresent))
            {   // if the LC switch is on and the key is cycled and a calibrate is not in progress
                if (ctrl->calSwitchToPLC && (ctrl->calCycle && !m_oldCtrl->calCycle) && !(ctrl->calInProgress))
                {   // Tell user it is time to perform a LC. The VehicleTest server will
                    //  publish his TestInProgress when he receives this start
                    //  cal data. This will cause us to start looking for end of
                    //  calibrate.
                    XmlNode dataNode(START_LOSSCOMPENSATION_DATA_TAG, "1");
                    BepServer::Write(&dataNode);
                    // set the VIN to LossCompensation
                    WriteNdbData( VIN_DATA_TAG, std::string("Loss_Compensation"));
                    // set the VINDisplay to LossCompensation
                    WriteNdbData( VINDISPLAY_DATA_TAG, std::string("Loss Compensation"));
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
            }
        }
    }
    Log( LOG_FN_ENTRY, "Exit CheckLossCompensation()\n");
}

void SystemMonitor::CheckSimulate( ControlData *ctrl)
{
    Log( LOG_FN_ENTRY, "Enter CheckSimulate()\n");
    Log( LOG_FN_ENTRY, "Exit CheckSimulate()\n");
}

//-----------------------------------------------------------------------------
void SystemMonitor::DisplayPrompt(INT32 whichBox,
                                  const char *promptTag,
                                  const char *color /* = NULL */,
                                  bool removePrompt /* = true*/,
                                  INT32 priorityLevel /* = MIN_PRIORITY*/)
{
    string response;

    Log(LOG_DEV_DATA, 
        "Displaying prompt %s in object %d\n", promptTag,whichBox);

    // if specified, remove the current prompt
    if (removePrompt)    
    {
        m_promptServer->DisplayPrompt(whichBox,std::string("Blank"),response,
                                      priorityLevel);
    }

    // display the prompt
    m_promptServer->DisplayPrompt(whichBox,std::string(promptTag),response,
                                  priorityLevel);

    // if a color is specified, set the background color
    if (color != NULL)
    {
        m_promptServer->SetBackgroundColor(whichBox,std::string(color),
                                           response);
    }
}

//-----------------------------------------------------------------------------
void SystemMonitor::DisplayPrompt(INT32 whichBox, const char *promptTag, 
                                  bool removePrompt, 
                                  INT32 priorityLevel /* = MIN_PRIORITY*/)
{
    string response;

    Log(LOG_DEV_DATA, 
        "Displaying prompt %s in object %d\n", promptTag,whichBox);

    // if specified, remove the current prompt
    if (removePrompt)
    {
        RemovePrompt(whichBox,"Blank","white",priorityLevel);
    }

    // display the prompt
    m_promptServer->DisplayPrompt(whichBox,std::string(promptTag),response,
                                  priorityLevel);
}

//-----------------------------------------------------------------------------
void SystemMonitor::RemovePrompt(INT32 whichBox, const char *promptTag, 
                                 const char *color /* = NULL */, 
                                 INT32 priorityLevel /* = MIN_PRIORITY*/)
{
    string response;

    Log(LOG_DEV_DATA, 
        "Removing prompt %s from object %d\n", promptTag,whichBox);

    // remove the prompt
    m_promptServer->RemovePrompt(whichBox,promptTag,response,priorityLevel);

    // if a color is specified, set the background color
    if (color != NULL)
    {
        m_promptServer->SetBackgroundColor(whichBox,std::string(color),
                                           response);
    }
}

bool& SystemMonitor::ReadNdbData( const char *tag, bool &val)
{
    string  ndbResp;
    val = (atoi( ReadNdbData(tag, ndbResp).c_str()) == 1);
    return( val);
}

char& SystemMonitor::ReadNdbData( const char *tag, char &val)
{
    string  ndbResp;
    val = ReadNdbData(tag, ndbResp)[ 0];

    return( val);
}

int& SystemMonitor::ReadNdbData( const char *tag, int &val)
{
    string  ndbResp;
    val = atoi( ReadNdbData(tag, ndbResp).c_str());
    return( val);
}

long& SystemMonitor::ReadNdbData( const char *tag, long &val)
{
    string  ndbResp;
    val = atol( ReadNdbData(tag, ndbResp).c_str());
    return( val);
}

float& SystemMonitor::ReadNdbData( const char *tag, float &val)
{
    string  ndbResp;
    val = atof( ReadNdbData(tag, ndbResp).c_str());
    return( val);
}

string& SystemMonitor::ReadNdbData( const char *tag, string &val)
{
    string  ndbResp, ndbMsg;

    if( (errno=m_ndbLock.Acquire()) == EOK)
    {
        if(m_dataBroker->Read( tag, ndbResp, true) == BEP_STATUS_SUCCESS)
        {
            if(m_dataBroker->GetByTag( tag, ndbResp, ndbMsg) == BEP_STATUS_SUCCESS)
                val = ndbResp;
        }

        m_ndbLock.Release();

        Log(LOG_DEV_DATA, "ReadNdbData(%s, %s)\n", tag, val.c_str());
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking the ndb mutex: %s\n", strerror( errno));
        val = BEP_SOFTWAREFAIL_RESPONSE;
    }

    return( val);
}

bool& SystemMonitor::ReadNdbData( const string &tag, bool &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

char& SystemMonitor::ReadNdbData( const string &tag, char &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

int& SystemMonitor::ReadNdbData( const string &tag, int &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

long& SystemMonitor::ReadNdbData( const string &tag, long &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

float& SystemMonitor::ReadNdbData( const string &tag, float &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

string& SystemMonitor::ReadNdbData( const string &tag, string &val)
{
    return( ReadNdbData( tag.c_str(),val));
}

INT32 SystemMonitor::WriteNdbData( const char *tag, const string &val)
{
    INT32 status = BEP_STATUS_FAILURE;
    string  ndbResp;

    Log( LOG_FN_ENTRY, "Enter WriteNdbData( %s, %s)\n", tag, val.c_str());

    if( (errno=m_ndbLock.Acquire()) == EOK)
    {
        status = m_dataBroker->Write( tag, val, ndbResp, true);
        Log( LOG_FN_ENTRY, "Exit WriteNdbData( %s, %s)\n", tag, val.c_str());

        m_ndbLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking the ndb mutex: %s\n", strerror( errno));
    }

    return(status);
}

INT32 SystemMonitor::WriteNdbData( const char *tag, bool val)
{
    if( val)    return(WriteNdbData( tag, string("1")));
    else        return(WriteNdbData( tag, string("0")));
}

INT32 SystemMonitor::WriteNdbData( const char *tag, char val)
{
    string ndbData( val, 1);
    return(WriteNdbData( tag, ndbData));
}

INT32 SystemMonitor::WriteNdbData( const char *tag, int val)
{
    char ndbData[ 11];
    return(WriteNdbData( tag, string( itoa( val, ndbData, 10))));
}

INT32 SystemMonitor::WriteNdbData( const char *tag, long val)
{
    char ndbData[ 11];
    return(WriteNdbData( tag, string( ltoa( val, ndbData, 10))));
}

INT32 SystemMonitor::WriteNdbData( const char *tag, float val)
{
    char ndbData[ 15];
    _tsprintf( ndbData, "+%7.3f", val);
    return(WriteNdbData( tag, string( ndbData)));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, const string &val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, bool val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, char val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, int val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, long val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::WriteNdbData( const string &tag, float val)
{
    return(WriteNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const char *tag, const string &val)
{
    string  ndbResp;
    INT32 status = BEP_STATUS_FAILURE;


    Log( LOG_FN_ENTRY, "Enter CommandNdbData( %s, %s)\n", tag, val.c_str());

    if( (errno=m_ndbLock.Acquire()) == EOK)
    {
        status = m_dataBroker->Command( tag, val, ndbResp, true);
        Log( LOG_FN_ENTRY, "Exit CommandNdbData( %s, %s)\n", tag, val.c_str());

        m_ndbLock.Release();
    }
    else
    {
        Log( LOG_ERRORS, "\tError locking the ndb mutex: %s\n", strerror( errno));
    }

    return(status);
}

INT32 SystemMonitor::CommandNdbData( const char *tag, bool val)
{
    if( val)    return(CommandNdbData( tag, string("1")));
    else        return(CommandNdbData( tag, string("0")));
}

INT32 SystemMonitor::CommandNdbData( const char *tag, char val)
{
    string ndbData( val, 1);
    return(CommandNdbData( tag, ndbData));
}

INT32 SystemMonitor::CommandNdbData( const char *tag, int val)
{
    char ndbData[ 11];
    return(CommandNdbData( tag, string( itoa( val, ndbData, 10))));
}

INT32 SystemMonitor::CommandNdbData( const char *tag, long val)
{
    char ndbData[ 11];
    return(CommandNdbData( tag, string( ltoa( val, ndbData, 10))));
}

INT32 SystemMonitor::CommandNdbData( const char *tag, float val)
{
    char ndbData[ 15];
    _tsprintf( ndbData, "+%7.3f", val);
    return(CommandNdbData( tag, string( ndbData)));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, const string &val)
{
    return(CommandNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, bool val)
{
    return(CommandNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, char val)
{
    return(CommandNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, int val)
{
    return(CommandNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, long val)
{
    return(CommandNdbData( tag.c_str(), val));
}

INT32 SystemMonitor::CommandNdbData( const string &tag, float val)
{
    return(CommandNdbData( tag.c_str(), val));
}

const std::string SystemMonitor::Register(void)
{
    string retVal( BEP_SUCCESS_RESPONSE);

    Log( LOG_FN_ENTRY, "Enter SystemMonitor::Register()\n");

    // create a named data broker interface
    if (m_dataBroker != NULL)
    {
        delete m_dataBroker;
        m_dataBroker = NULL;
    }
    m_dataBroker = new INamedDataBroker;

    // call the base class
    BepServer::Register();

    // create a prompt server interface and initialize it
    if (m_promptServer != NULL)
    {
        delete m_promptServer;
        m_promptServer = NULL;
    }
    m_promptServer = new IPromptServer;
    Log( LOG_DEV_DATA, "m_promptServer::Initialize()\n");
    m_promptServer->Initialize( PROMPT_SERVER_NAME);
    m_promptServer->SetProcessName("SystemMonitor");

    // create a fault server interface
    if (m_faultServer != NULL)
    {
        delete m_faultServer;
        m_faultServer = NULL;
    }
    m_faultServer = new IFaultServer;
    Log( LOG_DEV_DATA, "m_faultServer::Initialize()\n");
    m_faultServer->Initialize( FAULT_SERVER_NAME);
    // subscribe for all faults
    m_faultServer->SubscribeSeverityLevelFaults( 0, GetProcessName(), retVal, true);

    // identify the machine type
    XmlString value;
    ReadNdbData( MACHINE_TYPE, value);
    m_machineType = value;

    // clear out the keypress for the next publication
    WriteSubscribeData(PENDANT_KEY_PRESS, std::string("Blank"));
    // need to update the controls after the system starts
    NotifyDataChanged();
    // flag that the system has been registered
    SetSystemMonitorRegistered(true);

    Log( LOG_FN_ENTRY, "Exit SystemMonitor::Register()\n");

    return( retVal);
}

void SystemMonitor::SpecialProcessing(XmlNode *node, const std:: string type, BepServer *server)
{
    // If this is a Publish or Write message
    if ((type == BEP_PUBLISH) || (type == BEP_WRITE))
    {
        // Look for the MssgClass message classifier
        try
        {
            const XmlNode *mssgClassNode = node->getAttributes().getNode( XML_T( BEP_MSSG_CLASS));
            //Check if this is a Fault message
            if (mssgClassNode->getValue() == XmlString( XML_T( "Fault")))
            {
                // if a fault update was made, update the system
                if (ProcessFault(node))
                    NotifyDataChanged();
            }
        }
        catch (XmlException &err)
        {
        }
        BepServer::SpecialProcessing(node, type, server);
    }
}

bool SystemMonitor::ProcessFault(const XmlNode *fault)
{
    // put the code for handling the specific fault here
    Log(LOG_ERRORS, "Handling Fault %s\n", fault->ToString().c_str());

    return(false);
}

void SystemMonitor::WaitForChange(void)
{
    Log( LOG_FN_ENTRY, "WaitForChange()\n");
    m_updateControl->CriticalSectionEnter();
}

void SystemMonitor::NotifyDataChanged(void)
{
    Log( LOG_FN_ENTRY, "NotifyDataChanged()\n");
    m_updateControl->CriticalSectionExit();
}

ControlData* SystemMonitor::ControlAllocate() const
{
    return( ControlData::Allocate());
}

void SystemMonitor::ControlFree(ControlData* &controlData) const
{
    if( NULL != controlData)
    {
        delete controlData;
        controlData = NULL;
    }
}
void SystemMonitor::SetSystemMonitorRegistered(bool isSysMonRegistered)
{
    m_sysMonRegistered = isSysMonRegistered;
}

bool SystemMonitor::IsSystemMonitorRegistered(void)
{
    return m_sysMonRegistered;
}

void SystemMonitor::StartWheelbaseAdjust(void)
{
    WriteNdbData(WHEELBASE_MOVE_TAG, true);
    m_wheelbasePulseTimer.Start();
    Log( LOG_DEV_DATA, "Wheelbase Adjust bit set true\n");
}

void SystemMonitor::StopWheelbaseAdjust(void)
{
    WriteNdbData(WHEELBASE_MOVE_TAG, false);
    Log( LOG_DEV_DATA, "Wheelbase Adjust bit set false\n");
}

bool const SystemMonitor::GetSystemMonitorWheelbaseAdjust(void)
{
    return m_systemMonitorWheelbaseAdjust;
}



void SystemMonitor::StartReRelaxRetRollsPulse(void)
{
    WriteNdbData(RERELAX_RETROLLS_TAG, true);
    m_ReRelaxRetRollsPulseTimer.Start();
    Log( LOG_DEV_DATA, "ReRelaxRetRolls bit set true\n");
}

void SystemMonitor::StopReRelaxRetRollsPulse(void)
{
    WriteNdbData(RERELAX_RETROLLS_TAG, false);
    Log( LOG_DEV_DATA, "ReRelaxRetRolls bit set false\n");
}

bool const SystemMonitor::GetSystemMonitorFrontReductionPressureAdjust(void)
{
    return m_systemMonitorFrontReductionPressureAdjust;
}



void SystemMonitor::SetSystemMonitorWheelbaseAdjust(bool value)
{
    m_systemMonitorWheelbaseAdjust = value;
}

/*******************************************************************************
    Usage Block
    Information printed when a user types 'use xxxxx' at the QNX prompt

#ifdef __USAGE

System Monitor Task

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
    - Registers name as 'rolls/SystemMonitor' by default
    - Must be run as Super User.

#endif

*******************************************************************************/




