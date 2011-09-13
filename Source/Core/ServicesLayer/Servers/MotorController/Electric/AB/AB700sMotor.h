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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/AB/AB700sMotor.h $
// 
// 5     10/30/07 4:15p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     4/13/06 4:20a Cward
// Updated Fault Clearing logic
//
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 1:58p Rwiersem
// Made the test class a friend.  Added the methods needed to reload the
// configuration items without restarting the process.
//
// 2     8/17/05 8:25p Cward
// Cleaned up javadocs comments
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
// 1     6/14/04 10:55a Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 7     1/16/04 3:51p Bmeinke
// Changed MAX_AB_TX_RATE from 5ms to 10ms due to the fact that the drive
// will not recognize a hardware enable input for up to 10ms after the
// input is asserted. If we give the drive and enable command before the
// hardware input is asserted, the drive WILL NOT be enabled.
//
// 6     1/13/04 4:50p Bmeinke
// Added m_torqueSpeedLimit parameter to limit the max speed the drive
// will achieve in torque mode
// Changed the torque mode command to ABS_MIN_SPD_TRQ_SELECT to limit
// torque mode motor speed
//
// 5     1/09/04 10:45p Bmeinke
// Added MAX_ABS_TX_RATE constant to define the minimum amount of time
// required between sending 2 consecutive packets to the same drive
// Added a lastTxTime argument to WriteDriveCmd() to determine if we are
// trying to talk to a drive to frequently
//
// 4     1/09/04 1:34p Bmeinke
// Removed ThreadStartupWait() method and m_threadStartup data member (not
// used)
// Replaced m_driveStatus and m_driveCmd objects with
// m_driveStsMssg/m_driveStsData and m_driveCmdMssg/m_driveCmdData objects
// for better CIP compliance
// Added the following data members and corresponding config data for
// torque mode control:
// m_ftLbsPerHp, m_maxNegTorque, m_maxPosTorque, m_motorHP
//
// 3     1/05/04 10:35p Bmeinke
// Added data members for drive IP address and TCP port
// m_driveStatus and m_driveCommand are no longer dynamically allocated
// Added SetDrivesHwEnabled() method so drive objects know if the hardware
// enable bit from the PLC is set
//
// 2     12/29/03 8:42p Bmeinke
// "Security" checkin (compiles. but not tested)
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _AB700SMOTOR_H_INCLUDED_
#define _AB700SMOTOR_H_INCLUDED_
//==============================================================================

#include "CommonPacket.h"
#include "EIPMssg.h"
#include "EIPResponse.h"
#include "CIPMssg.h"
#include "IoMssg.h"
#include "Motor.h"
#include "BepSync.h"
#include "ABMotor.h"

/**
 * Maximum rate at which we can transmit to the drive
 */
#define MAX_AB_TX_RATE  (mSEC_nSEC(10))

/**
 * Flag used to indicate that we need to clear faults on the drive
 */
#define CLEAR_FAULT_MASK            0x01

/**
 * Flag used to indicate that we are actively trying to clear faults on the
 * drive. This is needed because the AB drives do not always clear faults when
 * only a single ClearFualts command is sent: you need to toggle the ClearFault
 * bit in the command word to the drive a few times before the faults actually
 * get cleared.
 */
#define TOGGLE_CLEAR_FAULT_MASK 0x02

/**
 * This is just a thought on the bass-ackwards way we could implement
 * delayed drive commands (i.e. staggered LC startup speeds)
 *
 * class DelayedDriveCommand
 * {
 * public:
 *      DelayedDriveCommand( const ByteArray_t &cmd, uint32_t msDelay) :
 *          m_driveCommand( smd), m_commandDelay( mSEC_nSEC( msDelay))
 *      {
 *      }
 *
 * private:
 *      ByteArray_t     m_driveCommand;
 *
 *      uint64_t        m_commandDelay;
 * };
 *
 * typedef std::list< DelayedDriveCommand>  DelayedDriveCmdQueue_t;
 *
 * DelayedCmdTimer  AB700sMotor::m_delayedTimer;
 *
 *
 * If the delayed command queue is not empty, enable a 1ms timer and
 * check if the delayed command needs to be sent out at each timer
 * tick. When the delayed drive queue gets emptyu, disable the timer
 * and go back to sending drive commands normally. If we detect any
 * faults from the drive, stop the delayed command timer and empty
 * the delayed command queue.
 */

/**
 * Motor interface object used to control an Allen Bradley PowerFlex 700s
 * Electric Motor Drive
 *
 * @author Brian Meinke
 */
class AB700sMotor : public ABMotor
{
    friend class AB700sMotorTest;

    typedef enum
    {
        DATA_NOTIFY_PULSE   = 10,
        THREAD_CTRL_PULSE   = 11,
        SEND_CMD_PULSE      = 12,
        CMD_DELAY_PULSE     = 13
    } ABMotorPulseCode_t;

public:

    AB700sMotor( int speedRefIdx=0);

    virtual ~AB700sMotor();

    /**
     * AB Specific Interface
     */
    ////////////////////////////////

    /**
     * Motor initialization method.  This will set up the motor
     * base on the configuration that is provided.
     *
     * @param config The configuration to use for motor initialization.
     * @return EOK if the initialization is successful, else error code (errno)
     */
    virtual const INT32 Initialize(const XmlNode *config);

    /**
     * Load the additional configuration items needed by the AB700s motor
     * class.  The "standard" items will be handled by the call to
     * Motor::Initialize().
     *
     * @param config A pointer to the parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *config);

    /**
     * Command the motor to Enable.  This must be done prior to commanding the
     * motor.
     */
    virtual const INT32 CommandEnable(void);

    /**
     * Command the motor to Disable.
     */
    virtual const INT32 CommandDisable(void);

    /**
     * Command the motor to a certain RPM.
     *
     * @param rpm    The RPM to command the motor to.
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     * @return The status of the command.
     */
    virtual const INT32 CommandRPM(const float rpm, const UINT32 delay=0);

    /**
     * Command the motor to provide the specified torque. This
     * assumes a negative torque.  Ex. 40 would be interprited
     * as applying 40 lbs of torque
     *
     * @param torque The torque to apply to the motor.
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     * @return The status of the operation.
     */
    virtual const INT32 CommandTorque(const float torque, const UINT32 delay=0);

    /**
     * Command the drive to perform a data read
     *
     * @param command Data tag to read
     * @param delay   Delay before performing command
     *
     * @return If command was successful
     */
    virtual const INT32 Enquire(const std::string command, const UINT32 delay=0);

    /**
     * Command the motor to terminate.
     */
    virtual const INT32 Terminate(void);

    /**
     * Checks to see if the drive has a current fault condition
     */
    virtual bool IsFaulted() const;

    /**
     * Clears a fault status from the drive
     */
    void ClearFault();

    /**
     * Command this drive to follow another master drive (LF or LR)
     *
     * @param master master drive to follow
     * @param masterRollSpeed
     *               Current speed of the master roller.
     *
     * @return EOK if successull, any other value indicates an error
     */
	virtual const INT32 FollowMaster( const ABMotor *master, const float &masterRollSpeed, const INT32 motorCount = 4);

    /**
     * Sets a flag indicating if the hardware enable bit from the PLC
     * is set. If the AB drives are enabled before this hardware
     * enable bit is set, the drives will enter a fault state.
     *
     * @param hwEnableSet
     *               State of the hardware enable bit from the PLC
     */
    virtual void SetDrivesHwEnabled( bool hwEnableSet);

    /**
     * Returms the maximum speed that the drive will go when in torque
     * mode
     *
     * @return The maximum speed that the drive will go when in torque
     *         mode
     */
    float GetMaxTrqSpeed() const;

protected:
    /**
     * Establish a connection to an Ethernet/IP device on the given port
     * at the given IP address
     *
     * @return Socket handle if successful, -1 if error
     */
    int EIPConnect();

    /**
     * Establish an ethernet TCP connection to our drive
     *
     * @return EOK if successful
     */
    int TcpConnect();

    /**
     * Disconnect a connected Ethernet/IP session
     *
     * @return 0
     */
    int EIPDisconnect();
    /**
     * Block until a command has been received and process the
     * command.  This will return when terminated.
     */
    virtual void* Execute(void);

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
    int WaitForSignal( io_pulse_t &pulse);

    /**
     * Reads the current drive status message
     *
     * @param readAddr IP address to read the drive message from
     *
     * @return EOK if this is a valid IO message from our drive
     *         EPERM if this is a message from a drive other than ours
     *         EBADMSG if this is an invalid drive I/O message
     */
    int ReadDriveStatus( const IpMulticastAddr_t &readAddr);

    /**
     * Reads the current drive status message
     *
     * @param sendAddr   IP address to send the drive command to
     * @param lastTxTime Timestamp of the last successful transmission to the drive
     *
     * @return EOK if the drive command was successfully sent any
     *         other value indicates an error
     */
    int WriteDriveCommand( const IpMulticastAddr_t &sendAddr, uint64_t &lastTxTime);

    /**
     * Creates a multi-cast listener socket
     *
     * @param ipData IP address information containing the multi-cast address and
     *               port to listen on
     *
     * @return File descriptor for the multi-cast listener socket or -1 if error
     */
    int OpenMultiCastListener( IpMulticastAddr_t &ipData);

    /**
     * Creates a multi-cast sender socket
     *
     * @param ipData IP address information containing the multi-cast address and
     *               port to write to
     *
     * @return File descriptor for the multi-cast sender socket or -1 if error
     */
    int OpenMultiCastSender( IpMulticastAddr_t &ipData);

    /**
     * Registers the caller for notifications from the serial driver when data is
     * available to be read from the serial port
     *
     * @param fd file descriptor to register
     * 
     * @return 0 if successful, -1 if an error occurred
     */
    int DriverDataRegister( int fd);

    /**
     * Unregisters the caller for notifications from the serial driver when data is
     * available to be read from the serial port
     */
    void CancelDriverNotification( int fd);

private:

    /**
     * Sends a CIP register session message out on the given socket
     *
     * @param sock   The socket to send the message on
     *
     * @return number of bytes received or -1 if error
     */
    int EIPRegisterSession();

    /**
     * Sends a ForwardOpen request
     *
     * @return number of bytes received or -1 if error
     */
    int EIPForwardOpen();

    /**
     * Sends a CIP unregister session message out on the given socket
     *
     * @return 0 if successful or -1 if error
     */
    int EIPUnregisterSession();

    /**
     * Print out the contents of the given string
     *
     * @param prefix  String to print out before the contents of the string
     * @param message The message to be printed out
     */
    void PrintMssg( const char *prefix, const MssgBase &message);

    /**
     * Print out the raw-byte contents of the given string
     *
     * @param prefix  String to print out before the contents of the string
     * @param message The message to be dumped
     */
    void DumpMssg( const char *prefix, const MssgBase &message);

    /**
     * Method used to signal our messaging thread that we need to
     * send new data to the drives
     *
     * @param pulseVal Pulse to be sent to the threads channel. Default value is SEND_CMD_PULSE
     */
    void SignalNewCommand( int pulseVal=SEND_CMD_PULSE) const;

    /**
     * Ethernet/IP session handle. Returnd in the EIPRegisterSession response
     */
    uint32_t                m_sessionHandle;

    /**
     * The response message object used to hold the response to our ForwardOpen
     * request
     *
     * @see EIPSendRRDataResp
     */
    EIPSendRRDataResp       m_fwdOpenRsp;

    /**
     * CIP Response message embedded inside the EIP ForwardOpen response
     */
    const CIPForwardOpenResponse *m_cipFwdOpenRsp;

    /**
     * Full CommonPacket message received from the drive containing an
     * embedded data packet with the drive's current status
     */
    AB700sUdpIoReply        m_driveStsMssg;

    /**
     * Object used to hold the data packet within the full CommonPacket
     * message received from the drive
     */
    DriveReplyDataPacket    m_driveStsData;

    /**
     * Full CommonPacket message sent to the drive containing an
     * embedded data packet with a command for the drive
     */
    AB700sUdpIoMssg         m_driveCmdMssg;

    /**
     * Object used to hold the command data packet embedded within
     * the full CommonPacket to be sent to the drive
     */
    DriveCmdDataPacket      m_driveCmdData;

    /**
     * File descriptor we will use to connect to the drive
     */
    int                     m_sockFd;

    /**
     * Socket we will use to receive the drive's I/O messages on
     */
    int                     m_readSock;

    /**
     * Socket we will use to send our I/O message to the drive on
     */
    int                     m_writeSock;

    /**
     * Ethernet/IP connection path for our forward open message
     *
     * @see ByteArray_t
     */
    ByteArray_t             m_connectionPath;

    /**
     * Channel on which we will be listening for control pulses on
     */
    int                     m_chId;

    /**
     * Connection to which control pulses should be sent
     */
    int                     m_coId;

    /**
     * Event we will use to subscribe for incoming socket data with
     */
    struct sigevent         m_sockData;

    /**
     * IP address of the drive to connect to
     */
    std::string             m_ipAddress;

    /**
     * TCP/IP port number to connect to the drive on
     */
    uint16_t                m_tcpPort;

    /**
     * IP address/port of the drive we want to talk to
     */
    struct sockaddr_in      m_driveAddress;

    /**
     * Interval (micro-seconds) at which we will be sending data to the drive
     */
    uint32_t                m_origRpi;

    /**
     * Interval (micro-seconds) at which we would like to receive data from the drive
     */
    uint32_t                m_targRpi;
    /** Flag used to indicate that we need to clear faults to the drive */
    uint8_t                 m_needClearFault;
};


#endif //_AB700SMOTOR_H_INCLUDED_

