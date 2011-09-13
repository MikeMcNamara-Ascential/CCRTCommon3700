//******************************************************************************
// Description:
//    Allen Bradley PowerFlex 700s Electric Motor Object
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/AB/AB700sMotor.cpp $
// 
// 6     10/30/07 4:14p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 5     7/19/06 6:15p Cward
// Removed unused variable and cleaned up.
//
// 4     4/13/06 4:20a Cward
// Updated Fault Clearing logic
//
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 1:58p Rwiersem
// Added the methods needed to reload the configuration items without
// restarting the process.
//
// 2     8/17/05 8:22p Cward
// Fixed two bugs in the Execute method.
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 1     3/30/05 5:12p Cward
// Newly Added to Core
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 2     7/23/04 3:16p Bmeinke
// Made the EthernetIP packet interval configurable via the CommInterval
// parameter
// Changed the "reconnect" loop delay from 5sec to 2 seconds to avoid
// drive timeout faults
// Dont set a comm fault when we miss a single packet
//
// 2     7/09/04 2:15p Bmeinke
// Removed unused variables
//
// 3     6/29/04 10:51a Bmeinke
// Use the new Motor::m_threadPriority data member to set our thread
// priority
//
// 2     6/17/04 11:25p Bmeinke
// Made error logging more verbose
//
// 1     6/14/04 10:55a Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 6     1/16/04 3:49p Bmeinke
// GetTorqueCommand() now returns an empty string instead of NULL
// GetSpeedCommand() now returns an empty string instead of NULL
// No longer automatically clear drive faults with each message we send;
// any faults should only be cleared on rolls up
// After we send a message we need to clear the 'clear fault' bit in the
// drive command message so we aren't continuously sending the drive the
// 'clear fault' bit
//
// 5     1/13/04 4:49p Bmeinke
// Fixed typo in tag name for FootPoundsPerHP in Initialize() (was
// FootPundsPerHP)
// CommandDisable() now sets torque command and speed command values in
// the motor command packet to 0
// Added m_torqueSpeedLimit parameter to limit the max speed the drive
// will achieve in torque mode
// Changed the torque mode command to ABS_MIN_SPD_TRQ_SELECT to limit
// torque mode motor speed
// The drive command data packet no longer has a secondary speed reference
// (SpeedRef2).
//
// 4     1/09/04 10:43p Bmeinke
// Added more intelligence to CommandRPM() and CommandTorque() to cut-down
// on the amount of traffic being sent to the drives
// Fixed a bug in FollowMaster() where we would not go back into speed
// mode after siwtching from the master drive to a slave drive
// Added some logic so we dont send two consecutive packets to the same
// drive within 5ms (drive does not like that)
//
// 3     1/09/04 1:38p Bmeinke
// Removed ThreadStartupWait() method and m_threadStartup data member (not
// used)
// Replaced m_driveStatus and m_driveCmd objects with
// m_driveStsMssg/m_driveStsData and m_driveCmdMssg/m_driveCmdData objects
// for better CIP compliance
// Added the following data members and corresponding config data for
// torque mode control:
// m_ftLbsPerHp, m_maxNegTorque, m_maxPosTorque, m_motorHP
// Added receive timeout support to detect lack of communication from the
// drive
// Improved the logic for re-connecting to the drive inside the Execute()
// method
// Modified some of the methods that can trigger a command to be sent to
// the drive immediately so they only signal the need to send a new
// command if the command to the drive needs to change
//
// 2     1/05/04 10:35p Bmeinke
// Made base speed a parameter
// Added data members for drive IP address and TCP port
// m_driveStatus and m_driveCommand are no longer dynamically allocated
// Added SetDrivesHwEnabled() method so drive objects know if the hardware
// enable bit from the PLC is set
// Re-ordered the sequence inside of Terminate() to avoid SIGSEGV's
// WriteDriveCommand() toggles the clear fault bit in the drive command
// word
// SignalNewCommand() sends the notification pulse at a priority of 50
//
// 1     12/30/03 7:41p Bmeinke
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

#include "AB700sMotor.h"

//-----------------------------------------------------------------------------
AB700sMotor::AB700sMotor( int speedRefIdx /*=0*/) : ABMotor(speedRefIdx),
    m_sessionHandle( (uint32_t)-1), m_cipFwdOpenRsp( NULL),
    m_driveStsMssg( m_driveStsData), m_driveCmdMssg( 0, m_driveCmdData),
    m_sockFd( -1), m_readSock( -1), m_writeSock( -1), m_chId( -1), m_coId( -1),
    m_ipAddress( "0.0.0.0"), m_tcpPort( (uint16_t)-1), m_origRpi( 1000000), m_targRpi( 1000000),
    m_needClearFault(0)
{
    m_driveCmdMssg.SetSequenceNo( 0);

    m_driveCmdData.SetCommand( 0);
    m_driveCmdData.SetSpeedRef1( 0);
    m_driveCmdData.SelectSpeedRef( 1);
    m_driveCmdData.SelectMode( AB700s::SPEED_MODE_SELECT);
    m_driveCmdData.SetTorqueCmd( 0.0);
    m_driveCmdData.SetMaxPosTrq( GetMaxPosTorque());
    m_driveCmdData.SetMaxNegTrq( GetMaxNegTorque());

    m_connectionPath.clear();
    memset( &m_sockData, 0, sizeof( m_sockData));
    memset( &m_driveAddress, 0, sizeof( m_driveAddress));
}

//-----------------------------------------------------------------------------
AB700sMotor::~AB700sMotor()
{
    EIPDisconnect();

}

//-----------------------------------------------------------------------------
const INT32 AB700sMotor::Initialize(const XmlNode *config)
{
    INT32 status=EOK;

    if( (status=ABMotor::Initialize(config)) == EOK)
    {
        try
        {   // We can't talk to the drives more frequently than 10ms
            if( m_origRpi < 10) m_origRpi = 10;
            // Change msec into usec
            m_targRpi = m_origRpi = m_origRpi * 1000;
            /**
             * Create a channel/connection pair for use in
             * controlling our thread
             **/
            BposCreateSelfAttachment( &m_chId, &m_coId);

            Log( "IP Address = %s:%d\n", m_ipAddress.c_str(), m_tcpPort);
            Log( "Packet Int = %d us\n", m_origRpi);

            // if the ip address or port are not set, set the status to address
            // failure
            if((m_ipAddress == "0.0.0.0") || (m_tcpPort == (uint16_t)-1))
            {
                status = EDESTADDRREQ;
            }
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "Error reading motor parameters %s\n", err.GetReason());
            status = EIDRM;
        }
    }

    return( status);
}

//-----------------------------------------------------------------------------
void AB700sMotor::LoadAdditionalConfigurationItems(const XmlNode *config)
{   // Start off by calling the base class
    ABMotor::LoadAdditionalConfigurationItems(config);
    const XmlNode *parameters = config->getChild("Setup")->getChild("Parameters");

            try
            {
                // Read IP Address and port
                const XmlNode *ipNode = config->getChild( XML_T( "Setup/IPAddress"));
                m_ipAddress = ipNode->getAttribute( XML_T( "address"))->getValue();
                m_tcpPort = BposReadInt( ipNode->getAttribute( XML_T( "port"))->getValue().c_str());
            }
            catch( XmlException &err)
            {
                Log( LOG_ERRORS, "Error reading IP address/port for %s\n", GetName().c_str());
        // set the data to invalid values
        m_ipAddress = "0.0.0.0";
        m_tcpPort = (uint16_t)-1;
    }

            try
            {
                // Read Packet Interval
                const XmlNode *rpiNode = parameters->getChild( XML_T( "CommInterval"));
                m_origRpi = atoi( rpiNode->getValue().c_str());
            }
            catch( XmlException &err)
            {
                // Default to 1 second packet interval
                m_origRpi = 1000;
            }
}

/**
 * Command the motor to Enable.  This must be done prior to commanding the
 * motor.
 *
 * @since Version 1.0
 */
const INT32 AB700sMotor::CommandEnable(void)
{
    INT32   status = EINVAL;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::CommandEnable()\n");

    if( m_driveCmdData.IsEnabled() == false)
    {
        m_driveCmdData.Enable();
        SignalNewCommand();
    }
    status = EOK;
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::CommandEnable()\n");

    return( status);
}

/**
 * Command the motor to Disable.
 *
 * @since Version 1.0
 */
const INT32 AB700sMotor::CommandDisable(void)
{
    INT32   status = EINVAL;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::CommandDisable()\n");
    if( m_driveCmdData.IsEnabled() == true)
    {
        // Clear the run command
        m_driveCmdData.Disable();
        // Set torque reference back to 0
        m_driveCmdData.SetSpeedRef1( 0);
        // Set torque reference back to 0
        m_driveCmdData.SetTorqueCmd( 0);
        SignalNewCommand();
    }
    status = EOK;
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::CommandDisable()\n");

    return( status);
}

/**
 * Command the motor to a certain RPM.
 *
 * @param rpm    The RPM to command the motor to.
 * @param delay  The amount of time to delay in ms prior to executing
 *               the command.
 * @return The status of the command.
 */
const INT32 AB700sMotor::CommandRPM(const float rpm, const UINT32 delay)
{
    INT32   status = EINVAL;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::CommandRPM( %+.2f)\n", rpm);
    if( (status=m_driveCmdData.Lock()) == EOK)
    {
        float relRpm = rpm / GetBaseSpeed();
        if( (m_driveCmdData.GetSpeedRef1() != relRpm) ||
            (m_driveCmdData.SelectedSpeedRef() != 1) ||
            (m_driveCmdData.SelectedMode() != AB700s::SPEED_MODE_SELECT))
        {
            // Speed is given as a percentage of base speed
            m_driveCmdData.SetSpeedRef1( relRpm);
            m_driveCmdData.SelectSpeedRef( 1);
            m_driveCmdData.SelectMode( AB700s::SPEED_MODE_SELECT);

            SignalNewCommand();
        }
        m_driveCmdData.Unlock();

    }
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::CommandRPM( %+.2f)\n", rpm);

    return( status);
}

/**
 * Command the motor to provide the specified torque. This
 * assumes a negative torque.  Ex. 40 would be interprited
 * as applying 40 lbs of torque
 *
 * @param torque The torque to apply to the motor.
 * @param delay  The amount of time to delay in ms prior to executing
 *               the command.
 * @return The status of the operation.
 * @since Version 1.0
 */
const INT32 AB700sMotor::CommandTorque(const float torque, const UINT32 delay)
{
    INT32   status = EINVAL;
    float   rpmLimit = MphToRpm( GetTorqueModeSpeedLimit()) / GetBaseSpeed();

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::CommandTorque( %+.4f) up to %d MPH\n", torque, GetTorqueModeSpeedLimit());

    if( (status=m_driveCmdData.Lock()) == EOK)
    {
        float relTrq = -torque / GetMaxTorque();
        if( (m_driveCmdData.GetTorqueCmd() != relTrq) ||
            (m_driveCmdData.SelectedMode() != AB700s::ABS_MIN_SPD_TRQ_SELECT) ||
            (m_driveCmdData.GetSpeedRef1() != rpmLimit) ||
            (m_driveCmdData.SelectedSpeedRef() != 1))
        {
            if( ((relTrq < 0) && (rpmLimit > 0)) ||
                ((relTrq > 0) && (rpmLimit < 0)))
            {
                rpmLimit *= -1;
            }
            // Set torque mode speed limit
            m_driveCmdData.SetSpeedRef1( rpmLimit);
            m_driveCmdData.SelectSpeedRef( 1);
            // Torque is given as a percentage of max torque
            m_driveCmdData.SetTorqueCmd( relTrq);
            m_driveCmdData.SelectMode( AB700s::ABS_MIN_SPD_TRQ_SELECT);

            SignalNewCommand();
        }
        m_driveCmdData.Unlock();
    }
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::CommandTorque( %+.4f)\n", torque);

    return( status);
}

/**
 * Sets a flag indicating if the hardware enable bit from the PLC
 * is set. If the AB drives are enabled before this hardware
 * enable bit is set, the drives will enter a fault state.
 *
 * @param hwEnableSet
 *               State of the hardware enable bit from the PLC
 */
void AB700sMotor::SetDrivesHwEnabled( bool hwEnableSet)
{
    Log( LOG_FN_ENTRY, "Enter AB700sMotor::SetDrivesHwEnabled( %d)\n", hwEnableSet);
    if( (errno=m_driveCmdData.Lock()) == EOK)
    {   // Call the base class to do the work
        ABMotor::SetDrivesHwEnabled(hwEnableSet);
        m_driveCmdData.Unlock();
    }
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::SetDrivesHwEnabled( %d)\n", hwEnableSet);
}

/**
 * Command the drive to perform a data read
 *
 * @param command Data tag to read
 * @param delay   Delay before performing command
 *
 * @return If command was successful
 * @since Version 1.3
 */
const INT32 AB700sMotor::Enquire(const std::string command, const UINT32 delay)
{
    return( EOK);
}

/**
 * Command the motor to terminate.
 * @since Version 1.0
 */
const INT32 AB700sMotor::Terminate(void)
{
    Log( LOG_DEV_DATA, "Enter AB700sMotor::Terminate()\n");
    // Disable the drive
    m_driveCmdData.Disable();
    // Send the disable command
    SignalNewCommand( THREAD_CTRL_PULSE);
    BposSleep( 10);

    EIPDisconnect();

    Motor::Terminate();

    // let the thread see the terminate request
    SignalNewCommand( THREAD_CTRL_PULSE);
    BposSleep( 10);

    Log( LOG_DEV_DATA, "Exit AB700sMotor::Terminate()\n");

    return( EOK);
}

/**
 * Checks to see if the drive has a current fault condition
 */
bool AB700sMotor::IsFaulted() const
{
    return( m_driveStsData.IsFaulted());
}

/**
 * Clears a fault status from the drive
 */
void AB700sMotor::ClearFault()
{
    m_driveCmdData.Lock();
    // Set the "clear fault" bit so we try to clear
    // any faults the next time we send a message
    m_needClearFault |= CLEAR_FAULT_MASK;
    m_driveCmdData.Unlock();
}

#if 0
/**
 * Gets the speed reference index for this drive. Applicapble only for
 * left front and left rear drive objects. Used for speed matching,
 * each drive will be told to follow a particular speed reference
 * index: LF = 4, LR = 5.
 *
 * @return The speed reference index for this drive
 */
const uint16_t AB700sMotor::GetSpeedRefIdx() const
{
    return( m_driveIndex);
}
#endif

/**
 * Command this drive to follow another master drive (LF or LR)
 *
 * @param master master drive to follow
 *
 * @return EOK if successull, any other value indicates an error
 */
const INT32 AB700sMotor::FollowMaster( const ABMotor *master, const float &masterRollSpeed, const INT32 motorCount /*= 4*/)
{
    INT32   status = EINVAL;
    bool    signalToSend = false;

    ABMotor *masterMotor = const_cast<ABMotor *>(master);
    Log( LOG_DEV_DATA, "Enter AB700sMotor::FollowMaster( %d)\n", masterMotor->SpeedRefDriveIndex());
    // If I am not the master
    if( (master != this) || (motorCount == 2) )
    {
        // If we need to follow a different drive
        // or if we are not in speed mode
        if( (m_driveCmdData.SelectedSpeedRef() != masterMotor->SpeedRefDriveIndex()) ||
            (m_driveCmdData.SelectedMode() != AB700s::SPEED_MODE_SELECT))
        {
            // Set my speed reference to the master's speed ref index
            m_driveCmdData.SelectSpeedRef( masterMotor->SpeedRefDriveIndex());
            m_driveCmdData.SelectMode( AB700s::SPEED_MODE_SELECT);
            signalToSend = true;
        }
    }
    else
    {
        // If we are currently following another drive
        // or if we are not in torque mode (to free wheel)
        if( (m_driveCmdData.SelectedSpeedRef() != 0) ||
            (m_driveCmdData.SelectedMode() != AB700s::ABS_MIN_SPD_TRQ_SELECT))
        {
            // I am the master...just freewheel (no speed reference)
            m_driveCmdData.SetSpeedRef1( 0);
            m_driveCmdData.SelectSpeedRef( 1);
            m_driveCmdData.SelectMode( AB700s::ABS_MIN_SPD_TRQ_SELECT);
            signalToSend = true;
        }
    }
    // If we changed anything
    if( signalToSend == true)
    {
        SignalNewCommand();
    }
    status = EOK;

    Log( LOG_DEV_DATA, "Exit AB700sMotor::FollowMaster( %d)\n", masterMotor->SpeedRefDriveIndex());

    return( status);
}

/**
 * Establish a connection to an Ethernet/IP device on the given port
 * at the given IP address
 *
 * @return Socket handle if successful, -1 if error
 */
int AB700sMotor::EIPConnect()
{
    EIPResponse eipResp;
    const CommonPacketFwdOpenRespItem   *comPktFwdOpen = NULL;

    // Establish connection to the drive
    if( TcpConnect() == EOK)
    {
        Log( LOG_DEV_DATA, "TCP connection established\n");
        // Establish an Ethernet/IP session
        if( EIPRegisterSession() != -1)
        {
            // Invalidate our surrent CIP corward open message reference
            m_cipFwdOpenRsp = NULL;

            Log( LOG_DEV_DATA, "Ethernet/IP session established, handle = $%08X\n", m_sessionHandle);
            // The ForwardOpen needs the handle for the session we just created
            if( EIPForwardOpen() != -1)
            {
                // Get a reference to the CIP ForwardOpen response message
                try
                {
                    const CommonPacket& commonPkt = m_fwdOpenRsp.GetCommonPacket();
                    comPktFwdOpen = dynamic_cast<const CommonPacketFwdOpenRespItem*>(commonPkt.GetDataItem( 0x00B2));
                    if( comPktFwdOpen != NULL)
                    {
                        m_cipFwdOpenRsp = dynamic_cast<const CIPForwardOpenResponse*>(comPktFwdOpen->GetCipMssg());
                        Log( LOG_DEV_DATA, "Ethernet/IP ForwardOpen succeeded\n");
                    }
                }
                catch( ...)
                {
                    Log( LOG_ERRORS, "Ethernet/IP ForwardOpen failed (Invalid CIP message)\n");
                    close( m_sockFd);
                    m_sockFd = -1;
                }
            }
            else
            {
                Log( LOG_ERRORS, "ForwardOpen failed...Closing socket\n");
                close( m_sockFd);
                m_sockFd = -1;
            }
        }
        else
        {
            Log( LOG_ERRORS, "RegisterSession failed...Closing socket\n");
            close( m_sockFd);
            m_sockFd = -1;
        }
    }

    return( m_sockFd);
}

/**
 * Establish an ethernet TCP connection to our drive
 *
 * @return EOK if successful
 */
int AB700sMotor::TcpConnect()
{
    uint64_t    timeOut = mSEC_nSEC( 5000);
    struct hostent *hostEnt;
    int retVal, on=1;

    // Close existing connection
    if( m_sockFd != -1)
    {
        close( m_sockFd);
        m_sockFd = -1;
    }

    // Clear out the server info
    memset( &m_driveAddress, 0, sizeof(m_driveAddress));

    // Get the host name
    if( (hostEnt = gethostbyname( m_ipAddress.c_str())) != NULL)
    {
        // Fill in the sockaddr struct with the true IP Address
        memcpy(&m_driveAddress.sin_addr, hostEnt->h_addr, hostEnt->h_length);
        m_driveAddress.sin_port = htons( m_tcpPort);
        m_driveAddress.sin_family = hostEnt->h_addrtype;

        Log( LOG_DEV_DATA, "Connecting to %s:%d\n", inet_ntoa( m_driveAddress.sin_addr), htons(m_driveAddress.sin_port));

        // Create the socket to talk on
        if( (m_sockFd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1)
        {
            // Disable nagle algorithm
            if( (retVal = setsockopt( m_sockFd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof( on))) != -1)
            {
                // Connect to the drive
                TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_REPLY, NULL, &timeOut, NULL);
                if( connect(m_sockFd, (struct sockaddr*)&m_driveAddress, sizeof(m_driveAddress)) != -1)
                {
                    retVal = EOK;
                }
                else
                {
                    retVal = errno;
                    Log( LOG_ERRORS, "Error connect()'ing: %s\n", strerror( errno));
                    close( m_sockFd);
                    m_sockFd = -1;
                }
            }
            else
            {
                retVal = errno;
                Log( LOG_ERRORS, "Error setting TCP_NODELAY()'ing: %s\n", strerror( errno));
                close( m_sockFd);
                m_sockFd = -1;
            }
        }
        else
        {
            retVal = errno;
            Log( LOG_ERRORS, "Error creating socket(): %s\n", strerror( errno));
        }
    }
    else
    {
        retVal = errno;
        Log( LOG_ERRORS, "Error resolving host <%s>: %s\n", m_ipAddress.c_str(), strerror( errno));
    }


    return(retVal);
}

/**
 * Disconnect a connected Ethernet/IP session
 *
 * @return 0
 */
int AB700sMotor::EIPDisconnect()
{
    Log( LOG_FN_ENTRY, "Enter AB700sMotor::EIPDisconnect()\n");
    EIPUnregisterSession();
    close( m_sockFd);
    m_sockFd = -1;
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::EIPDisconnect()\n");

    return( 0);
}
/**
 * Block until a command has been received and process the
 * command.  This will return when terminated.
 * @since Version 1.0
 */
void* AB700sMotor::Execute(void)
{
    bool                okToRun = true, reconnect=true;
    IpMulticastAddr_t   readAddr;
    IpMulticastAddr_t   sendAddr;
    io_pulse_t          ctrlPulse;
    uint32_t            loopDelay;
    uint64_t            curRxTime, lastRxTime, lastTxTime;
    uint64_t            rcvTimeout = mSEC_nSEC( 10000LL);
    int                 readErr, errCount=0;
    const int           faultReportCount = 5;

    // Make sure we don't get lowered in priority
    BposSetThreadPriority( 0, 0, m_threadPriority);
    // Make sure our data ready notification pulse has the correct priority
    SIGEV_PULSE_INIT( &m_sockData, m_coId, SIGEV_PULSE_PRIO_INHERIT, 0, DATA_NOTIFY_PULSE);

    // Initialize the timestamp of our last transmission to 3ms ago
    ClockTime( CLOCK_REALTIME, NULL, &lastTxTime);
    lastTxTime -= MAX_AB_TX_RATE;

    // Loop until told to stop
    while( (okToRun == true) && ( reconnect == true) && (GetStatus() != BEP_TERMINATE_STATUS))
    {
        Log( LOG_DEV_DATA, "Attempting to connect to drive\n");

        // No errors yet
        errCount = 0;

        // Default to 2 second delay between connection attempts
        loopDelay = 2000;

        // Assume the connect attempt fails
        reconnect = true;

        // (Re-)Establish our Ethernet/IP session
        if( EIPConnect() != -1)
        {
            Log( LOG_DEV_DATA, "Initial connection established\n");
            // Create a multi-cast listener socket to receive data from the drive on
            if( (m_readSock=OpenMultiCastListener( readAddr)) != -1)
            {
                Log( LOG_DEV_DATA, "Opened multicast listener socket\n");
                // Create a multi-cast sender socket to send our data to the drive on
                if( (m_writeSock=OpenMultiCastSender( sendAddr)) != -1)
                {
                    Log( LOG_DEV_DATA, "Opened multicast sender socket\n");
                    if( DriverDataRegister( m_readSock) != -1)
                    {
                        // Make sure all our messaging objects are properly initialized
                        if( m_cipFwdOpenRsp != NULL)
                        {
                            m_driveStsMssg.SetConnectionId( m_cipFwdOpenRsp->m_targOrigConnId);
                            m_driveCmdMssg.SetConnectionId( m_cipFwdOpenRsp->m_origTargConnId);
                            m_driveCmdMssg.SetSequenceNo( 0);

                            m_driveCmdData.SetCommand( 0);
                            m_driveCmdData.SetSpeedRef1( 0.0);
                            m_driveCmdData.SelectSpeedRef( 1);
                            m_driveCmdData.SelectMode( AB700s::SPEED_MODE_SELECT);
                            m_driveCmdData.SetTorqueCmd( 0.0);
                            m_driveCmdData.SetMaxPosTrq( GetMaxPosTorque());
                            m_driveCmdData.SetMaxNegTrq( GetMaxNegTorque());

                            Log( LOG_DEV_DATA, "Drive Command Connection ID = $%08X ($%08X)\n",
                                 m_driveCmdMssg.GetConnectionId(), m_cipFwdOpenRsp->m_origTargConnId);

                            Log( LOG_DEV_DATA, "\tStarting message loop\n");

                            reconnect = false;
                        }
                        else
                        {
                            Log( LOG_ERRORS, "CIP Message object not initialized...exiting\n");
                            reconnect = true;
                        }
                    }
                }
            }
        }

        // Set our connection fault flag if we did not successfully connect
        SetCommFault( reconnect);

        // If we successfully connected to the drive, we must be receiving data
        if( reconnect == false)
        {
            // Initialize the time stamp of the last RX message to now
            ClockTime( CLOCK_REALTIME, NULL, &lastRxTime);
        }

        // Loop while not told to stop and dont need to reconnect
        while( (okToRun == true) && (reconnect == false) && (GetStatus() != BEP_TERMINATE_STATUS))
        {
            // Make sure we don't get lowered in priority
            BposSetThreadPriority( 0, 0, m_threadPriority);

            // Wait for incoming data or a control signal from our main thread(s)
            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE, NULL, &rcvTimeout, NULL);
            if( MsgReceivePulse( m_chId, &ctrlPulse, sizeof( ctrlPulse), NULL) == 0)
            {
                Log( LOG_DEV_DATA, "Received pulse %d,%d\n", ctrlPulse.code, ctrlPulse.value.sival_int);

                // Make sure this is a pulse we are interested in
                if( ctrlPulse.code == m_sockData.sigev_code)
                {
                    switch( ctrlPulse.value.sival_int)
                    {
                    case DATA_NOTIFY_PULSE:
                        Log( LOG_DEV_DATA, "Data available\n");
                        // try to read the drive status message
                        if( (readErr = ReadDriveStatus( readAddr)) == EOK)
                        {
                            // Convert message time to nano seconds and add a little
                            rcvTimeout = m_targRpi * 1100LL;

                            // Grab the time of the last successfully received packet
                            ClockTime( CLOCK_REALTIME, NULL, &lastRxTime);

                            // Write our current command status to the drive
                            WriteDriveCommand( sendAddr, lastTxTime);

                            // Clear comm fault after successful transaction
                            if( errCount != 0)
                            {
                                Log( LOG_ERRORS, "Clearing motor comm fault\n");
                                SetCommFault( false);
                                errCount = 0;
                            }
                        }
                        // If this was not a valid message from a different drive
                        else if( readErr != EINVAL)
                        {
                            if(errCount <= faultReportCount)
                            {
                                errCount++;
                            }
                            else
                            {
                                errCount = faultReportCount+1;
                            }
                            Log( LOG_ERRORS, "Error #%d reading drive status: %s\n", errCount, strerror( readErr));
                        }
                        // Re-register for data notifications
                        DriverDataRegister( m_readSock);
                        break;
                    case CMD_DELAY_PULSE:
                        break;
                    case THREAD_CTRL_PULSE:
                        break;
                    case SEND_CMD_PULSE:
                        if( GetStatus() != BEP_TERMINATE_STATUS)
                        {
                            WriteDriveCommand( sendAddr, lastTxTime);
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
            else
            {
                Log( LOG_DEV_DATA, "Receive timed out\n");
                rcvTimeout = m_targRpi * 1100LL;
            }

            // Check if we have not received a valid packet within
            //  the allowed time
            ClockTime( CLOCK_REALTIME, NULL, &curRxTime);
            if( (curRxTime-lastRxTime) >= rcvTimeout)
            {
                if(errCount <= faultReportCount)
                {
                    errCount++;
                }
                else
                {
                    errCount = faultReportCount+1;
                }
                Log( LOG_ERRORS, "Failed to receive packet within the alotted RPI: %d\n", errCount);
            }

            // Set comm fault on first error
            if( errCount == faultReportCount)
            {
                Log( LOG_ERRORS, "setting comm fault\n");
                SetCommFault( true);
            }
            // Try to reconnect after 5 consecutive errors
            else if( errCount >= faultReportCount)
            {
                errCount = 0;
                reconnect = true;
                okToRun = false;
            }
        }

        EIPDisconnect();

        // Set our connection fault flag if we did not successfully connect
        SetCommFault( reconnect);

        // If not commanded to terminate
        if( GetStatus() != BEP_TERMINATE_STATUS)
        {
            // If we need to reconnect
            if( (okToRun = reconnect) == true)
            {
                // Wait about 1 second before reconnecting
                while( (GetStatus() != BEP_TERMINATE_STATUS) && (loopDelay > 0))
                {
                    if( loopDelay > 100)    loopDelay -= 100;
                    else                    loopDelay = 0;
                    BposSleep( 100);
                }
            }
        }
        else
        {
            // Clear the speed/torque commands when we terminate
            m_driveCmdData.SetCommand( 0);
            m_driveCmdData.SetSpeedRef1( 0.0);
            m_driveCmdData.SelectSpeedRef( 0);
            m_driveCmdData.SelectMode( AB700s::SPEED_MODE_SELECT);
            m_driveCmdData.SetTorqueCmd( 0.0);
            m_driveCmdData.SetMaxPosTrq( GetMaxPosTorque());
            m_driveCmdData.SetMaxNegTrq( GetMaxNegTorque());

            lastTxTime -= MAX_AB_TX_RATE;
            WriteDriveCommand(sendAddr, lastTxTime);
        }

        if( m_readSock != -1)   close( m_readSock);
        if( m_writeSock != -1)  close( m_writeSock);

        m_readSock = -1;
        m_writeSock = -1;
    }

    return( NULL);
}

/**
 * Waits for a pulse on our "control" channel. Possible pulses we will
 * receive are:
 * 1. A notification pulse from the socket manager indicating
 *    that there is data available for reading.
 * 2. A "control" pulse indicating that we need to send a new
 *    command to the drive or that we need to terminate
 *
 * @param pulse  Pulse we received
 *
 * @return 0 if a valid pulse was received, -1 if an error occurred
 */
int AB700sMotor::WaitForSignal( io_pulse_t &pulse)
{
    return( MsgReceivePulse( m_chId, &pulse, sizeof( pulse), NULL));
}

/**
 * Reads the current drive status message
 *
 * @param readAddr IP address to read the drive message from
 *
 * @return EOK if this is a valid IO message from our drive
 *         EPERM if this is a message from a drive other than ours
 *         EBADMSG if this is an invalid drive I/O message
 */
int AB700sMotor::ReadDriveStatus( const IpMulticastAddr_t &readAddr)
{
    uint64_t    timeOut = m_targRpi * 1000LL;
    int         retVal, bytesRead;
    int         bytesAvail;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::ReadDriveStatus()\n");

    errno = EOK;
    if( (bytesAvail = tcischars( m_readSock)) > 0)
    {
        uint8_t     readBuf[ (const int)bytesAvail];

        Log( LOG_DEV_DATA, "Attempting to read %d bytes\n", bytesAvail);
        // Read the next status message from the drive
        TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_RECEIVE | _NTO_TIMEOUT_REPLY, NULL, &timeOut, NULL);
        if( (bytesRead=read( m_readSock, readBuf, sizeof( readBuf)) ) < 0)
        {
            if( errno == EINTR) errno = ETIMEDOUT;
            Log( LOG_ERRORS, "\tError reading drive status message: %s\n", strerror( errno));
            retVal = errno;
        }
        // Format the raw response into a response object
        else
        {
            Log( LOG_DETAILED_DATA, "Read %d of %d bytes\n", bytesRead, bytesAvail);
            if( m_driveStsMssg.CreateFromBuff( readBuf, bytesRead) > 0)
            {
                // Make sure this response is for me
                if( m_driveStsMssg.GetConnectionId() == m_cipFwdOpenRsp->m_targOrigConnId)
                {
                    if( m_driveStsData.IsFaulted()) Log( LOG_ERRORS, "\tDrive has FAULT BIT set\n");
                    else                            Log( LOG_DETAILED_DATA, "\tDrive has FAULT BIT clear\n");

                    Log( LOG_DETAILED_DATA, "\t Speed = %+7.3f\n", m_driveStsData.GetSpeed() * GetBaseSpeed());
                    Log( LOG_DETAILED_DATA, "\tTorque = %+7.3f\n", m_driveStsData.GetTorque() * GetMaxTorque());

                    retVal = EOK;
                }
                else
                {
                    Log( LOG_DETAILED_DATA, "\tRead a message not for me ($%08X != $%08X)\n",
                         m_cipFwdOpenRsp->m_targOrigConnId, m_driveStsMssg.GetConnectionId());
                    retVal = EINVAL;
                }
            }
            else
            {
                Log( LOG_ERRORS, "\tERROR Not a valid drive status message\n");
                retVal = EBADMSG;
            }
        }
    }
    else
    {
        int err = errno;
        Log( LOG_ERRORS, "\tERROR No data to read: %d bytes, error=%s (%d)\n", bytesAvail, strerror( err), err);
        retVal = ENODATA;
    }
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::ReadDriveStatus( %d)\n", retVal);

    return( retVal);
}

/**
 * Reads the current drive status message
 *
 * @param sendAddr   IP address to send the drive command to
 * @param lastTxTime Timestamp of the last successful transmission to the drive
 *
 * @return EOK if the drive command was successfully sent any
 *         other value indicates an error
 */
int AB700sMotor::WriteDriveCommand( const IpMulticastAddr_t &sendAddr, uint64_t &lastTxTime)
{
    ByteArray_t     sendMssg;
    int             retVal = EOK;
    uint64_t        timeOut = m_origRpi * 1000LL;
    uint64_t        curTxTime;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::WriteDriveCommand()\n");

    if( m_cipFwdOpenRsp != NULL)
    {
        // Grab the current time
        ClockTime( CLOCK_REALTIME, NULL, &curTxTime);

        // Cant send data to the drive more quickly than 3 ms
        if( (curTxTime-lastTxTime) < MAX_AB_TX_RATE)
        {
            curTxTime = mSEC_nSEC( 10LL);//MAX_AB_TX_RATE-(curTxTime-lastTxTime);
            Log( LOG_DETAILED_DATA, "Waiting %lld nsec to send\n", curTxTime);
            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_NANOSLEEP, NULL, &curTxTime, NULL);
            Log( LOG_DETAILED_DATA, "Done Waiting to send\n");
        }
        // Lock the command object while changing status bits
        if( (retVal=m_driveCmdData.Lock()) == EOK)
        {
            // If the hardware enable bit status has changed
            if( EnableDriveNeeded())
            {
                Log( LOG_DEV_DATA, "Changing drive ENABLE status to <%s>\n", HwDriveEnabled() ? "Enabled" : "Disabled");

                // Set the drive status appropriately
                if( HwDriveEnabled() == true)   m_driveCmdData.Enable();
                else                            m_driveCmdData.Disable();

                // Say drive status changed to match HW bit status
                bool enableNeeded = false;
                EnableDriveNeeded(&enableNeeded);
            }

            // Update the sequence number
            m_driveCmdMssg.SetSequenceNo( m_driveCmdMssg.GetSequenceNo()+1);

            // If the drive is currently faulted
            if( IsFaulted())
            {
                // If we need to clear the fault
                if( m_needClearFault & CLEAR_FAULT_MASK)
                {
                    // We need to toggle the "Clear Fault" bit to the drive because the drive does
                    // not always clear its fault with a single Clear Fault command
                    //////////////////////////////////////////////////////////////////////////////

                    if( m_needClearFault & TOGGLE_CLEAR_FAULT_MASK)
                    {
                        // "Unclear" the fault status
                        m_driveCmdData.UnclearFault();
                        Log( LOG_DEV_DATA, "Need to clear the \"Clear Fault\" bit: $%02hhX\n", m_needClearFault);
                    }
                    else
                    {
                        // Clear the fault status
                        m_driveCmdData.ClearFault();
                        Log( LOG_DEV_DATA, "Need to set the \"Clear Fault\" bit: $%02hhX\n", m_needClearFault);
                    }
                    // Toggle the "clear fault" bit and gets it's previous version
                    m_needClearFault ^= TOGGLE_CLEAR_FAULT_MASK;
                }
            }
            else
            {
                if( m_needClearFault)
                {
                    Log( LOG_DEV_DATA, "Need to clear the \"Clear Fault\" bit because drive is no longer faulted: $%02hhX\n", m_needClearFault);
                }
                // Clear the "clear faults" flag if no faults are present
                m_needClearFault = 0;
                m_driveCmdData.UnclearFault();
            }

            // Format the write buffer
            m_driveCmdMssg.GetRawBytes( sendMssg);

            // Send our command to the drive
            TimerTimeout( CLOCK_REALTIME, _NTO_TIMEOUT_SEND | _NTO_TIMEOUT_REPLY, NULL, &timeOut, NULL);
            retVal = sendto( m_writeSock, sendMssg.c_str(), sendMssg.size(), 0, (struct sockaddr*)&sendAddr.addr, sizeof(sendAddr.addr));
            if( retVal < 0)
            {
                retVal = errno;
                Log( LOG_ERRORS, "Error sending command to drive: %s\n", strerror( errno));
            }
            else
            {
                // Update the timestamp for the latest transmission
                ClockTime( CLOCK_REALTIME, NULL, &lastTxTime);

                Log( LOG_DETAILED_DATA, "Sent %d drive command bytes\n", retVal);
                retVal = EOK;
            }

            // We only want to send the clear fault bit to the drive once
            m_driveCmdData.UnclearFault();

            m_driveCmdData.Unlock();
        }
        else
        {
            Log( LOG_ERRORS, "\tERROR locking drive command: %s\n", strerror( retVal));
        }
    }
    else
    {
        retVal = ENOBUFS;
    }

    Log( LOG_FN_ENTRY, "Exit AB700sMotor::WriteDriveCommand()\n");

    return( retVal);
}

/**
 * Creates a multi-cast listener socket
 *
 * @param ipData IP address information containing the multi-cast address and
 *               port to listen on
 *
 * @return File descriptor for the multi-cast listener socket or -1 if error
 */
int AB700sMotor::OpenMultiCastListener( IpMulticastAddr_t &ipData)
{
    const CommonPacketFwdOpenRespItem   *comPktFwdOpen = NULL;
    const CommonPacketTcpDataItem       *readTcp = NULL;
    int                                 fd = -1, on = 1;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::OpenMultiCastListener()\n");
    // Get the CommonPacket items that we need from the response
    try
    {
        comPktFwdOpen = dynamic_cast<const CommonPacketFwdOpenRespItem*>(m_fwdOpenRsp.GetCommonPacket().GetDataItem( 0x00B2));
        readTcp = dynamic_cast<const CommonPacketTcpDataItem*>(m_fwdOpenRsp.GetCommonPacket().GetDataItem( 0x8001));
    }
    catch( std::bad_cast &err)
    {
    }
    // Validate the necessary common packet items
    if( (readTcp == NULL) || (comPktFwdOpen == NULL))
    {
        ByteArray_t     rawData;

        if( readTcp == NULL)        Log( LOG_ERRORS, "No target socket data in reply\n");
        if( comPktFwdOpen == NULL)  Log( LOG_ERRORS, "No forward open reply in encapsulation reply\n");

        PrintMssg( "Forward open reply\n", m_fwdOpenRsp);
        Log(LOG_ERRORS, "Forward open reply bytes\n");
        if( (GetLogStatus() == true) && (GetVerboseMask() & LOG_ERRORS))
        {
            uint32_t        ii=0, jj=0;
            char            logBuff[ 4 + 5*16+1];
            char            byteBuff[ 5];

            m_fwdOpenRsp.GetRawBytes( rawData);
            while( ii<rawData.size())
            {
                sprintf( logBuff, "%02hhX  ", ii);
                for( jj=0; jj<16; jj++)
                {
                    sprintf( byteBuff, "$%02hhX ", rawData[ ii++]);
                    strcat( logBuff, byteBuff);
                }
                strcat(logBuff, "\n");
            }
            Log(LOG_ERRORS, "Forward Open reply bytes: %s", logBuff);
        }

        return(ENOMSG);
    }

    if( (IsDebugOn() == true) || (GetLocalEcho() == true))
    {
        PrintMssg( "ForwardOpen Reply:\n", *comPktFwdOpen);
        PrintMssg( "Target Socket Info:\n", *readTcp);
    }

    // Get the multicast address parameters
    readTcp->GetSocketInfo( ipData);

    // Open a generic socket
    if( (fd = socket( AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        Log( LOG_ERRORS, "Error creating UDP read socket: %s\n", strerror( errno));
    }
    // Allow multiple people to bind to this address
    else if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        Log( LOG_ERRORS, "Error setting SO_REUSEADDR on read socket: %s\n", strerror( errno));
        close( fd);
        fd = -1;
    }
    // Allow multiple people to bind to this port
    else if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)) < 0)
    {
        Log( LOG_ERRORS, "Error setting SO_REUSEPORT on read socket: %s\n", strerror( errno));
        close( fd);
        fd = -1;
    }
    // Bind to this address/port
    else if( bind(fd, (struct sockaddr*)&ipData.addr, sizeof(ipData.addr)) < 0)
    {
        Log( LOG_ERRORS, "Error binding to address %s: %s\n", inet_ntoa( ipData.addr.sin_addr), strerror( errno));
        close( fd);
        fd = -1;
    }
    // Add this socket to the multicast group
    else if(setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipData.mreq, sizeof(ipData.mreq)) < 0)
    {
        Log( LOG_ERRORS, "Error setting IP_ADD_MEMBERSHIP on read socket: %s\n", strerror( errno));
        close( fd);
        fd = -1;
    }
    else
    {
        Log( LOG_DEV_DATA, "Listening on %s:%d\n", inet_ntoa( ipData.mreq.imr_multiaddr), ipData.addr.sin_port);
    }

    Log( LOG_FN_ENTRY, "Exit AB700sMotor::OpenMultiCastListener()\n");
    return( fd);
}

/**
 * Creates a multi-cast sender socket
 *
 * @param ipData IP address information containing the multi-cast address and
 *               port to write to
 *
 * @return File descriptor for the multi-cast sender socket or -1 if error
 */
int AB700sMotor::OpenMultiCastSender( IpMulticastAddr_t &ipData)
{
    const CommonPacketFwdOpenRespItem   *comPktFwdOpen = NULL;
    const CommonPacketTcpDataItem       *sendTcp = NULL;
    int                                 fd = -1;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::OpenMultiCastSender()\n");

    // Get the CommonPacket items that we need from the response
    try
    {
        comPktFwdOpen = dynamic_cast<const CommonPacketFwdOpenRespItem*>(m_fwdOpenRsp.GetCommonPacket().GetDataItem( 0x00B2));
        sendTcp = dynamic_cast<const CommonPacketTcpDataItem*>(m_fwdOpenRsp.GetCommonPacket().GetDataItem( 0x8000));
    }
    catch( std::bad_cast &err)
    {
    }
    // Validate the necessary common packet items
    if( (sendTcp == NULL) || ( comPktFwdOpen == NULL))
    {
        if( sendTcp == NULL)        Log( LOG_ERRORS, "No originator socket data in reply\n");
        if( comPktFwdOpen == NULL)  Log( LOG_ERRORS, "No forward open reply in encapsulation reply\n");
        return(ENOMSG);
    }

    // Get the multicast address parameters
    sendTcp->GetSocketInfo( ipData);

    // If the drive sent an originator IP address of 0.0.0.0
    if( ipData.addr.sin_addr.s_addr == INADDR_ANY)
    {
        // Send to the same IP address that we sent the ForwardOpen request to
        ipData.addr.sin_addr.s_addr = m_driveAddress.sin_addr.s_addr;
        Log( LOG_DEV_DATA, "Drive returned INADDR_ANY...send to same address as connect address\n");
    }

    /* create what looks like an ordinary UDP socket */
    if((fd=socket(AF_INET,SOCK_DGRAM,0)) < 0)
    {
        Log( LOG_ERRORS, "Error creating UDP send socket: %s\n", strerror( errno));
    }
    else
    {
        Log( LOG_DEV_DATA, "Sending to %s:%d\n", inet_ntoa( ipData.addr.sin_addr), ipData.addr.sin_port);
    }

    Log( LOG_FN_ENTRY, "Exit AB700sMotor::OpenMultiCastSender()\n");

    return( fd);
}

/**
 * Registers the caller for notifications from the serial driver when data is
 * available to be read from the serial port
 *
 * @param dataEvent sigevent structure used to deliver the notification event
 * @return 0 if successful, -1 if an error occurred
 */
int AB700sMotor::DriverDataRegister( int fd)
{
    int status;

    Log( LOG_FN_ENTRY, "Enter AB700sMotor::DriverDataRegister()\n");

    // Cancel notification so we don't have multiple notification requests
    //  active simultaneoulsy
    CancelDriverNotification( fd);

    // Re-register for notifications of received data
    status = ionotify( fd, _NOTIFY_ACTION_POLLARM, _NOTIFY_COND_INPUT, &m_sockData);
    if( status == -1)
    {
        Log( LOG_ERRORS, "\tDriverDataRegister() ionotify failed: %s\n", strerror( errno));
    }
    // If data is currently available
    else if( status & _NOTIFY_COND_INPUT)
    {
        int bytesWaiting;
        if( (bytesWaiting=tcischars( fd)) > 0)
        {
            Log( LOG_DEV_DATA, "DriverDataRegister() %d bytes available, forwarding notification pulse\n", bytesWaiting);
            // Send a pulse to our channel
            MsgSendPulse( m_sockData.sigev_coid, m_sockData.sigev_priority,
                          m_sockData.sigev_code, m_sockData.sigev_value.sival_int);
        }
        else
        {
            Log( LOG_ERRORS, "\tSerial driver says data is ready, but tcischars() <= 0\n");
        }
        status = 0;
    }
    Log( LOG_FN_ENTRY, "Exit AB700sMotor::DriverDataRegister( %d)\n", status);

    return( status);
}

/**
 * Unregisters the caller for notifications from the serial driver when data is
 * available to be read from the serial port
 */
void AB700sMotor::CancelDriverNotification( int fd)
{
    int status;

    // Pass a NULL sigevent struct to cancel our subscription
    status = ionotify( fd, _NOTIFY_ACTION_POLL, _NOTIFY_COND_INPUT, NULL);
    // If error
    if( status == -1)
    {
        Log( LOG_ERRORS, "\tCancelDriverNotification() ionotify failed: %s\n", strerror( errno));
    }
}

/**
 * Sends a CIP register session message out on the given socket
 *
 * @param sock   The socket to send the message on
 *
 * @return number of bytes received or -1 if error
 */
int AB700sMotor::EIPRegisterSession()
{
    EIPRegisterSessionMssg  eipMssg;
    EIPResponse             eipResp;
    int                     retVal = -1;

    // Send the message
    if( (retVal=eipMssg.Send( m_sockFd)) > 0)
    {
        // Get the response
        if( (retVal = eipResp.Read( m_sockFd)) < 0)
        {
            Log( LOG_ERRORS, "\tError reading RegisterSession responseMesssage: %s\n", strerror( errno));
        }
        else if( retVal == 0)
        {
            Log( LOG_ERRORS, "\tError reading RegisterSession responseMesssage: NO DATA RECEIVED\n");
            errno = ENODATA;
            retVal = -1;
        }
        else
        {
            //PrintMssg( "RegisterSession Reply:\n", eipResp);
            m_sessionHandle = eipResp.GetSessionHandle();
        }
    }
    else
    {
        Log( LOG_ERRORS, "\tError sending RegisterSession request: %s\n", strerror( errno));
    }

    return( retVal);
}

/**
 * Sends a ForwardOpen request
 *
 * @return number of bytes received or -1 if error
 */
int AB700sMotor::EIPForwardOpen()
{
    EIPForwardOpenRequest   eipMssg( m_sessionHandle, m_origRpi, m_targRpi);
    int                     retVal = -1;

    // Send the forward open request
    if( eipMssg.Send( m_sockFd) > 0)
    {
        // Get the response
        if( (retVal = m_fwdOpenRsp.Read( m_sockFd)) > 0)
        {
            if( m_fwdOpenRsp.GetHeader().GetStatus() != 0)
            {
                Log( LOG_ERRORS, "\tForwardOpen status indicates error: %d\n", m_fwdOpenRsp.GetHeader().GetStatus());
                retVal = -1;
            }
        }
        else
        {
            Log( LOG_ERRORS, "ForwardOpen::read failed: %s\n", strerror( errno));;
        }
    }
    else
    {
        Log( LOG_ERRORS, "ForwardOpen::send failed: %s\n", strerror( errno));;
    }

    return( retVal);
}

/**
 * Sends a CIP unregister session message out on the given socket
 *
 * @return 0 if successful or -1 if error
 */
int AB700sMotor::EIPUnregisterSession()
{
    EIPUnRegisterSessionMssg    eipMssg( m_sessionHandle);
    int retVal = -1;

    Log( LOG_DEV_DATA, "Enter AB700sMotor::EIPUnregisterSession()\n");
    // Send the message
    if( eipMssg.Send( m_sockFd) > 0)
    {
        // There is no response to this message
        retVal = 0;
    }
    Log( LOG_DEV_DATA, "Exit AB700sMotor::EIPUnregisterSession()\n");

    return( retVal);
}

/**
 * Print out the contents of the given string
 *
 * @param prefix  String to print out before the contents of the string
 * @param message The message to be printed out
 */
void AB700sMotor::PrintMssg( const char *prefix, const MssgBase &message)
{
    if( prefix != NULL) printf( "%s", prefix);

    message.Print();
}

/**
 * Print out the raw-byte contents of the given string
 *
 * @param prefix  String to print out before the contents of the string
 * @param message The message to be dumped
 */
void AB700sMotor::DumpMssg( const char *prefix, const MssgBase &message)
{
    ByteArray_t         rawBuff;
    ByteArrayCItr_t     itr;
    int                 byteCnt=0;

    if( prefix != NULL) printf( "%s", prefix);
    message.GetRawBytes(rawBuff);
    itr = rawBuff.begin();
    printf( "%d bytes\n", rawBuff.size());
    while( itr != rawBuff.end())
    {
        printf( "$%04X\t", byteCnt);
        for( int ii=0; ii<16 && itr!=rawBuff.end(); byteCnt++, ii++, itr++)
        {
            printf( "$%02hhX ", *itr);
        }
        printf( "\n");
    }
}

/**
 * Method used to signal our messaging thread that we need to
 * send new data to the drives
 *
 * @param pulseVal Pulse to be sent to the threads channel. Default value is SEND_CMD_PULSE
 */
void AB700sMotor::SignalNewCommand( int pulseVal) const
{
    MsgSendPulse( m_coId, 50, 0, pulseVal);
}

