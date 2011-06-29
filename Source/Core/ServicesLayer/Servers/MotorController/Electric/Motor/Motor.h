//*************************************************************************
// Description:
//   Motor interface class
//
//===========================================================================
// Copyright (c) 2003 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Motor/Motor.h $
// 
// 4     10/30/07 4:17p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:05p Rwiersem
// Made the test class a friend.  Added the methods needed to reload the
// configuration items without restarting the process.
//
// 2     8/17/05 8:19p Cward
// moved implementation out of header files.
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 2     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 1     12/21/03 6:41p Cward
// 3     7/09/04 2:17p Bmeinke
// Refactored Motor inheritance tree so that base Motor class no longer
// derives from ILogicalPort
// Made UploadDriveParameters() and Enable() pure virtual methods
// Moved ReadDriveType() to the BaldorMotor class
// Made some of the private members protected so base classes can access
// them
// Added #include <process.h> for he getpid() function
//
// 4     6/29/04 11:25a Bmeinke
// Moved m_threadPriority from private to protected
//
// 3     6/29/04 10:50a Bmeinke
// Added m_threadPriority to allow customization of the individual motor
// threads with config file priority specification
//
// 2     6/17/04 11:28p Bmeinke
// No longer inherit from ILogicalPort (made a SerialMotor for that)
// UploadDriveParameters is now pure virtual
//
// 3     10/24/03 1:45p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:30a Khykin
// Initial checkin
//
// 2     8/07/03 9:32a Khykin
//
// 6     6/26/03 5:05p Tbochene
// Removed complier warnings
//
// 5     6/25/03 1:34p Tbochene
// Updated Execute() to check that status is not terminated
// to prevent race condition on resuming thread.
//
// 4     6/24/03 11:00a Tbochene
// Updated due changes in CreateMessage()
//
// 3     6/17/03 10:02a Tbochene
// Updated status logging and checking and properly initialized variables
// in constructor
//
// 1     5/29/03 2:50p Tbochene
// Motor interface class
//
//*************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _MOTOR_H_INCLUDED_
#define _MOTOR_H_INCLUDED_
//==============================================================================


#include "BepSync.h"
#include "AtomicThread.h"
#include "INamedDataBroker.h"
#include "SerialDefs.h"             // SerialString_t
#include "ILogger.h"

#ifndef MOTOR_BUF_SZ
    #define MOTOR_BUF_SZ    32              /* size of the message buffer to use for sending and receiving  */
#endif

#define MOTOR_STATUS_ENABLE     "Enable"
#define MOTOR_STATUS_DISABLE    "Disable"
#define MOTOR_STATUS_SPEED      "SetSpeed"
#define MOTOR_STATUS_RPM        "SetRPM"
#define MOTOR_STATUS_TORQUE     "SetTorque"
#define MOTOR_STATUS_ENQUIRE    "Enquire"


/**
 * Motor interface class
 *
 * @author Tim Bocehenk
 */
class Motor : public AtomicThread, public ILogger
{
    friend class ElectricMotorTest;

public:

    /**
     * Motor class consructor.
     */
    Motor(int detachState=PTHREAD_CREATE_DETACHED);

    /**
     * Motor class desructor.
     */
    virtual ~Motor(void);

    /**
     * Motor initialization method.  This will set up the motor
     * base on the configuration that is provided.
     *
     * @param config The configuration to use for motor initialization.
     * @return EOK if the initialization is successful, else error code (errno)
     */
    virtual const INT32 Initialize(const XmlNode *config);

    /**
     * Load the additional configuration items needed by the motor
     * class.
     *
     * @param document A pointer to a parsed configuration file.
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);

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
     * Command the motor to the specified MPH.
     *
     * @param mph    The MPH to command the motor to.
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     * 
     * @return The status of the command.
     */
    virtual const INT32 CommandSpeed(const float mph, const UINT32 delay = 0);

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
    virtual const INT32 CommandTorque(const float torque, const UINT32 delay = 0);

    /**
     * Command the motor to a certain RPM.
     *
     * @param rpm    The RPM to command the motor to.
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     * @return The status of the command.
     */
    virtual const INT32 CommandRPM(const float rpm, const UINT32 delay = 0);

    /**
     * Command the drive to perform a data read
     *
     * @param command Data tag to read
     * @param delay   Delay before performing command
     *
     * @return If command was successful
     */
    virtual const INT32 Enquire(const std::string command, const UINT32 delay = 0);

    /**
     * Upload the specified drive parameters to the motor.
     *
     * @param parameters The parameters to upload.
     * @return The status of the operation.
     */
    virtual const INT32 UploadDriveParameters(const XmlNode *parameters) = 0;

    /**
     * Check communication fault status
     *
     * @return Communication fault status
     */
    const bool GetCommFault(void) const;

    /**
     * Command the motor to terminate.
     */
    virtual const INT32 Terminate(void);

    /**
     * Returns the name of the motor channel
     *
     * @return Name of motor
     */
    const std::string GetName(void);

    /**
     * Get response to Enquiry command
     *
     * @return Response
     */
    const std::string GetEnquiryResponse(void);

    /**
     * Indicator that the motor may be locked up
     *
     * @return
     */
    const bool IMightBeDead(void);

protected:
    /**
     * Send a command to the drive
     *
     * @param message Command tag to send to the drive
     *
     * @return If operation succeeded
     */
    virtual const INT32 SendCommand(const std::string& message) = 0;

    /**
     * Send a variable length command to the drive
     *
     * @param message  Pattern from which to build command
     * @param tag Response tag
     * @param readResponseDelay
     *                 Amount of time to wait (in ms) before reading response data
     *
     * @return If operation succeeded
     */
    virtual const INT32 SendVariableCommand(const std::string message, const std::string tag, const INT32 readResponseDelay = 0) = 0;

    /**
     * Block until a command has been received and process the
     * command.  This will return when terminated.
     */
    virtual void* Execute(void);

    /**
     * Command the motor to perfrom a task.
     *
     * @param command The task to perform.
     * @param value   The value that should be used (if any)
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     */
    virtual const INT32 Command(const std::string command, const std::string value, const UINT32 delay = 0);

    /**
     * Command the motor to perfrom a task.
     *
     * @param command The task to perform.
     * @param value   The value that should be used (if any)
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     */
    virtual const INT32 Command(const std::string command, const float value = 0.0, const UINT32 delay = 0);

    /**
     * Flag that a communication fault occurred and report it
     * to the system.
     *
     * @param value  State of fault (true == active, else not active)
     */
    void SetCommFault(const bool value);

    /**
     * Enable the motor.  This must be done prior to commanding the
     * motor.
     *
     * @param enable Whether to enable (true) or disable (false) the drive
     *
     * @return The status of the motor enabling.
     */
    virtual const INT32 Enable(const bool enable) = 0;

    /**
     * Set the motor to go to the specified MPH.
     *
     * @param mph The MPH to command the motor to.
     * 
     * @return The status of the command.
     */
    const INT32 SetSpeed(float mph);

    /**
     * Set the motor to provide the specified torque. This
     * assumes a negative torque.  Ex. 40 would be interprited
     * as applying 40 lbs of torque
     *
     * @param torque The torque to apply to the motor.
     * @return The status of the operation.
     */
    const INT32 SetTorque(float torque);

    /**
     * Set the motor to a certain RPM.
     *
     * @param rpm    The RPM to command the motor to.
     * @return The status of the command.
     */
    const INT32 SetRPM(float rpm);

    /**
     * Set the value of the command (tag for message
     * substitution in configuration file)
     *
     * @param command Message tag to be used to send command to the drive
     */
    void SetEnquiryCommand(const std::string& command);

    /**
     * Get the value of the command (tag for message
     * substitution in configuration file)
     *
     * @return Message tag to be used to send command to the drive
     */
    const std::string GetEnquiryCommand(void) const;

    /**
     * Set the system command.
     *
     * @param command The command to update the current command with.
     * @param value   The value to update the current value with.
     * @param delay   The amount of time to delay in ms prior to executing
     *                the command.
     */
    const INT32 SetCommand(const std::string &command, const std::string &value, const UINT32 delay = 0);

    /**
     * Set the system command.
     *
     * @param command The command to update the current command with.
     * @param value   The value to update the current value with.
     * @param delay   The amount of time to delay in ms prior to executing
     *                the command.
     */
    const INT32 SetCommand(const std::string &command, const float value, const UINT32 delay = 0);


    /**
     * Get the command
     *
     * @return The last command that was received.
     */
    std::string GetCommand(void);

    /**
     * Get the command value
     *
     * @return the commanded value
     */
    const float GetValue(void);

    /**
     * Get the amount of time to delay for the current command
     *
     * @return the commanded delay
     */
    const UINT32 GetDelay(void);

    /**
     * Set response to Enquiry command
     *
     * @param response Value to set as response
     */
    void SetEnquiryResponse(const SerialString_t& response);

    /**
     * Get the string representing how a speed command should be built
     *
     * @return Speed string
     */
    virtual const char* GetTorqueCommand(void) = 0;

    /**
     * Get the string representing how a torque command should be built
     *
     * @return Torqu string
     */
    virtual const char* GetSpeedCommand(void) = 0;

    /**
     * Method to write data via the Named Data Broker object
     *
     * @return Status of operation
     */
    const INT32 WriteData(const std::string& tag, const std::string value);

    /**
     * Return the motor's RPM equivalent for the given MPH speed
     *
     * @param mph    MPH speed value to convert into motor RPMs
     *
     * @return The motor's RPM equivalent for the given MPH speed
     */
    virtual float MphToRpm( float mph) const;

    /**
     * Conversion to use for converting MPH to RPM.
     * Ex. 35 5/8 inch rollers - 17.8125 inch radius, 3 motor rev per roller rev
     * MPH2RPM = 1MPH * (63360 in/mile) / (2*pi*17.8125 in/rev) / (60 min/hr) * 3
     */
    float m_mphToRpm;

    /**
     * The number of bits per pound to use for loading.
     */
    float m_torqueBitsPerPound;

    /**
     * The maximum RPM that a motor is allowed to trun.
     */
    INT32 m_maxRPM;

    /**
     * The maximum positive torque limit that a motor is
     * allowed to apply.
     */
    INT32 m_maxPositiveTorqueLimit;

    /**
     * The maximum negative torque limit that a motor is
     * allowed to apply.
     */
    INT32 m_maxNegativeTorqueLimit;

    /**
     * Priority this motor thread should run at
     */
    INT32 m_threadPriority;

private:
    /**
     * Synchrozization variable to protect access to
     * the Named Data Broker interface object
     */
    BepMutex m_NdbSync;

    /**
     * NamedDataBroker interface for updating the system.
     */
    INamedDataBroker *m_iNDB;

    /**
     * System Tag to update when a communication fault is detected.
     */
    std::string m_commFault;

    /**
     * The status of the motor communication.
     * true - fault active
     * false - no fault
     */
    bool m_commFaultStatus;

    /**
     * The last command that was received.
     */
    std::string m_command;

    /**
     * The value that is to be commanded.
     */
    float m_commandValue;

    /**
     * The amount of time to wait prior to executing the command
     * that was received.
     */
    UINT32 m_commandDelay;

    /**
     * Value of enquiry to send to drive
     * (This must have an entry under VeicleMessages,
     * TransmitMessages, and ReplyMessages in configuration file)
     */
    std::string m_enquiry;

    /**
     * Object to block on while waiting for a new command.
     */
    BepCondVar<bool> m_CommandSync;

    /**
     * Object to block on while waiting to update a new command.
     */
    BepMutex m_UpdateSynch;

    /**
     * Response to Enquiry command
     */
    std::string m_enquiryResponse;

    /**
     * Counter for indicating status of process
     */
    INT32 m_itsAllGoodCounter;

    /**
     * Limit on how many failures indicate thread lock up
     */
    INT32 m_threadLockupCountLimit;

    /**
     * Threshhold of how many consecutive faults must
     * happen before a communication fault is reported
     */
    INT32 m_motorCommFaultLimit;

    /**
     * How many consecutive faults have been recorded
     */
    INT32 m_motorCommFaultCount;

};
#endif // _MOTOR_H_INCLUDED_

