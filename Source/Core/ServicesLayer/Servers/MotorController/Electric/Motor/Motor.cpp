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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Motor/Motor.cpp $
// 
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:05p Rwiersem
// Added the methods needed to reload the configuration items without
// restarting the process.
//
// 2     11/11/05 4:56p Cward
// Changes for 0511101 iteration
//
// 1     8/17/05 7:50p Cward
// Removed implementation from the header files
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

#include "Motor.h"

#include <process.h>                // getpid()

#include "BepSync.h"
#include "AtomicThread.h"
#include "INamedDataBroker.h"
#include "SerialDefs.h"             // SerialString_t
#include "ILogger.h"


Motor::Motor(int detachState /*=PTHREAD_CREATE_DETACHED */) : AtomicThread(detachState),
    m_mphToRpm(28.30612546), m_torqueBitsPerPound(5.723957011), m_maxRPM(2800),
    m_maxPositiveTorqueLimit(100), m_maxNegativeTorqueLimit(-100), m_iNDB(NULL),
    m_commFaultStatus(false), m_commandValue(0.0), m_commandDelay(0),
    m_itsAllGoodCounter(0), m_threadLockupCountLimit(100)
{
    m_threadPriority = getprio( 0);
}

Motor::~Motor(void)
{
}

const INT32 Motor::Initialize(const XmlNode *config)
{
    // load the motor parameters
    // initialize the base class
    INT32 status = EINVAL;
    Log(LOG_FN_ENTRY, "Motor::Initialize()\n");

    try
    {
        // initialize the base class
        if((status = m_CommandSync.Acquire()) == EOK)
        {
            ILogger::Initialize( config);
            Log(LOG_ERRORS, "Acquired command synchronization object\n");

            // set up the configuration
            Motor::LoadAdditionalConfigurationItems(config);

            // initialize the interfaces
            if(m_iNDB == NULL)
            {
                m_iNDB = new INamedDataBroker(IsDebugOn());
            }

            if((status = m_CommandSync.Release()) == EOK)
            {
                Log(LOG_ERRORS, "Released command synchronization object\n");
            }
            else Log(LOG_ERRORS, "Failed to release command synchronization object because: %s\n", strerror(status));

        }
        else Log(LOG_ERRORS, "Failed to acquire command synchronization object because: %s\n", strerror(status));
    }
    catch(BepException &Err)
    {
        Log(LOG_ERRORS, "Motor Configuration Error: %s\n", Err.what());
        if(m_CommandSync.Release() == EOK)
        {
            Log(LOG_ERRORS, "Released command synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to release command synchronization object because: %s\n", strerror(status));
        throw;
    }
    Log(LOG_FN_ENTRY, "Motor::Initialize() done\n");

    return(status);
}

void Motor::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    const XmlNode *parameters = document->getChild("Setup")->getChild("Parameters");

            m_mphToRpm = atof(parameters->getChild("MphToRpm")->getValue().c_str());
            m_torqueBitsPerPound = atof(parameters->getChild("TorqueBitsPerPound")->getValue().c_str());
            m_maxRPM = BposReadInt(parameters->getChild("MaxRpm")->getValue().c_str());
            m_maxPositiveTorqueLimit = BposReadInt(parameters->getChild("MaxPositiveTorqueLimit")->getValue().c_str());
            m_maxNegativeTorqueLimit = BposReadInt(parameters->getChild("MaxNegativeTorqueLimit")->getValue().c_str());
            m_threadLockupCountLimit = BposReadInt(parameters->getChild("ThreadLockupLimit")->getValue().c_str());
            m_motorCommFaultLimit = BposReadInt(parameters->getChild("CommFaultLimit")->getValue().c_str());
            if(m_motorCommFaultLimit < 0)
            {
                Log(LOG_ERRORS, "Tried to set communicaiton fault limit to %d, will set to 0\n", m_motorCommFaultLimit);
                m_motorCommFaultLimit = 0;
            }

            try
            {
                m_threadPriority = BposReadInt(parameters->getChild("Priority")->getValue().c_str());
                BposSetThreadPriority( getpid(), pthread_self(), m_threadPriority);
            }
            catch( ...)
            {
                m_threadPriority = getprio( 0);
            }

            // Set up fault names
    const XmlNode *dataTags = document->getChild("Setup")->getChild("DataTags");
            m_commFault = dataTags->getChild("CommFault")->getValue();
}

const INT32 Motor::CommandEnable(void)
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::CommandEnable()\n");
    status = Command(MOTOR_STATUS_ENABLE);
    Log(LOG_FN_ENTRY, "Motor::CommandEnable() done\n");

    return(status);
}

const INT32 Motor::CommandDisable(void)
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::CommandDisable()\n");
    status = Command(MOTOR_STATUS_DISABLE);
    Log(LOG_FN_ENTRY, "Motor::CommandDisable() done\n");

    return(status);
}

const INT32 Motor::CommandSpeed(const float mph, const UINT32 delay /*= 0*/ )
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::CommandSpeed(%3.2f, %u)\n", mph, delay);
    status = Command(MOTOR_STATUS_SPEED, mph, delay);
    Log(LOG_FN_ENTRY, "Motor::CommandSpeed(%3.2f, %u) done\n", mph, delay);

    return(status);
}

const INT32 Motor::CommandTorque(const float torque, const UINT32 delay /* = 0 */)
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::CommandTorque(%3.2f, %u)\n", torque, delay);
    status = Command(MOTOR_STATUS_TORQUE, torque, delay);
    Log(LOG_FN_ENTRY, "Motor::CommandTorque(%3.2f, %u) done\n", torque, delay);

    return(status);
}

const INT32 Motor::CommandRPM(const float rpm, const UINT32 delay /* = 0 */)
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::CommandRPM(%3.2f, %u)\n", rpm, delay);
    status = Command(MOTOR_STATUS_RPM, rpm, delay);
    Log(LOG_FN_ENTRY, "Motor::CommandRPM(%3.2f, %u) done\n", rpm, delay);

    return(status);
}

const INT32 Motor::Enquire(const std::string command, const UINT32 delay /* = 0 */)
{
    INT32 status = EOK;

    Log(LOG_FN_ENTRY, "Motor::Enquire()\n");
    status = Command(MOTOR_STATUS_ENQUIRE, command, delay);
    Log(LOG_FN_ENTRY, "Motor::Enquire() done\n");

    return(status);
}

const bool Motor::GetCommFault(void) const
{
    return(m_commFaultStatus);
}

const INT32 Motor::Terminate(void)
{
    // terminate the task
    Log(LOG_FN_ENTRY, "Motor::Terminate()\n");
    INT32 status = EOK, retStatus = status;

    if((retStatus = m_CommandSync.TryAcquire()) == EOK)
    {
        // Locked value
        Log(LOG_DEV_DATA, "Acquired command synchronization object\n");

        SetStatus(BEP_TERMINATE_STATUS);

        if((retStatus = m_CommandSync.Signal(true)) == EOK)
        {
            // Post that a new command is avaliable
            Log(LOG_DEV_DATA, "Signaled command synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to signal command synchronization object because: %s\n", strerror(status));

        if((status = m_CommandSync.Release()) == EOK)
        {
            // Unlocked value
            Log(LOG_DEV_DATA, "Released command synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to release command synchronization object because: %s\n", strerror(status));

        if((retStatus == EOK) || (status != EOK)) retStatus = status;   // Preserve previous failure or if none save last failure
    }
    else Log(LOG_ERRORS, "Failed to acquire command synchronization object because: %s (retStatus = %d)\n", strerror(retStatus), retStatus);

    Log(LOG_FN_ENTRY, "Motor::Terminate() done\n");

    return(retStatus);
}

const std::string Motor::GetName(void)
{
    return(GetProcessName());
}

const std::string Motor::GetEnquiryResponse(void)
{
    return(m_enquiryResponse);
}

const bool Motor::IMightBeDead(void)
{
    return(m_itsAllGoodCounter > m_threadLockupCountLimit);
}

void* Motor::Execute(void)
{
    INT32 status = EOK;

    while(GetStatus() != BEP_TERMINATE_STATUS)
    {
        // Wait util a command has been received until terminated
        try
        {
            // Wait to be told to do something
            if((status = m_CommandSync.Acquire()) == EOK)
            {
                // Resource acquired
                Log(LOG_DEV_DATA, "Acquired command synchronization object\n");
                Log(LOG_DEV_DATA, "%s, Waiting For Command\n", GetName().c_str());

                if((status = m_CommandSync.Wait(true)) == EOK)
                {
                    // Value was signaled
                    Log(LOG_DEV_DATA, "%s, Processing Command\n", GetName().c_str());
                    if(GetStatus() != BEP_TERMINATE_STATUS)
                    {
                        // process the command
                        std::string command = GetCommand();     // wait until a command is received
                        float value = GetValue();               // get the value of the command
                        long delay = GetDelay();                // get the amount of time to delay
                        if(delay)   BposSleep(delay);           // if value is not 0 sleep for delay ms
                        if(command == MOTOR_STATUS_ENABLE)          status = Enable(true);
                        else if(command == MOTOR_STATUS_DISABLE)    status = Enable(false);
                        else if(command == MOTOR_STATUS_SPEED)      status = SetSpeed(value);
                        else if(command == MOTOR_STATUS_RPM)        status = SetRPM(value);
                        else if(command == MOTOR_STATUS_TORQUE)     status = SetTorque(value);
                        else if(command == MOTOR_STATUS_ENQUIRE)
                        {
                            status = SendCommand(GetEnquiryCommand());
                        }
                        else Log(LOG_ERRORS, "Invalid Command Received: %s\n", command.c_str());

                        Log(LOG_ERRORS, "Command %s status: %s\n", command.c_str(), strerror(status));
                    }
                    m_CommandSync.SetValue(false); // Clear signaled variable (can't fail)
                }
                else Log(LOG_ERRORS, "Command synchronization object wait failed with error because %s\n", strerror(status));

                if((status = m_CommandSync.Release()) == EOK)
                {
                    Log(LOG_DEV_DATA, "Released command synchronization object\n");
                }
                else Log(LOG_ERRORS, "Could not release command synchronization object because: %s\n", strerror(status));
            }
            else Log(LOG_ERRORS, "Could not acquire command synchronization object because: %s\n", strerror(status));
        }
        catch(BepException &Err)
        {
            Log("Process commands failed because of BepException: %s\n", Err.what());
        }
        catch(...)
        {
            Log("Process commands failed because of Exception\n");
        }
    }
    Log(LOG_FN_ENTRY, "%s Terminated\n", GetName().c_str());

    return(NULL);
}

const INT32 Motor::Command(const std::string command, const std::string value, const UINT32 delay /*= 0*/)
{
    INT32 status = EINVAL, retStatus = EOK;

    Log(LOG_FN_ENTRY, "Motor::Command(%s, %s, %s, %u)\n", GetName().c_str(), command.c_str(), value.c_str(), delay);

    if((retStatus = m_CommandSync.TryAcquire()) == EOK)
    {
        // Locked value
        Log(LOG_DEV_DATA, "Acquired command synchronization object\n");
        m_itsAllGoodCounter = 0;    // Clear counter (Thread is not hung)

        if((retStatus = SetCommand(command, value, delay)) == EOK)
        {
            // If command was valid
            if((status = m_CommandSync.Signal(true)) == EOK)
            {
                // Post that a new command is avaliable
                status = EOK;
                Log(LOG_DEV_DATA, "Signaled command synchronization object\n");
            }
            else Log(LOG_ERRORS, "Failed to signal command synchronization object because %s\n", strerror(status));
        }
        if((status = m_CommandSync.Release()) == EOK)
        {
            // Unlocked value
            Log(LOG_DEV_DATA, "Released command synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to release command synchronization object because: %s\n", strerror(status));

        if((retStatus == EOK) || (status != EOK)) retStatus = status;   // Preserve previous failure or if none save last failure
    }
    else
    {
        m_itsAllGoodCounter++;  // Thread may be hung
        Log(LOG_ERRORS, "Failed to acquire command synchronization object because: %s\n", strerror(retStatus));
    }

    Log(LOG_FN_ENTRY, "Motor::Command(%s, %s, %s, %u) done\n", GetName().c_str(), command.c_str(), value.c_str(), delay);

    return(retStatus);
}

const INT32 Motor::Command(const std::string command, const float value /*= 0.0*/, const UINT32 delay /*= 0*/)
{
    INT32 status = EINVAL, retStatus = EOK;
    Log(LOG_FN_ENTRY, "Motor::Command(%s, %s, %3.2f, %u)\n", GetName().c_str(), command.c_str(), value, delay);

    if((retStatus = m_CommandSync.TryAcquire()) == EOK)
    {
        // Locked value
        Log(LOG_DEV_DATA, "Acquired command synchronization object\n");
        m_itsAllGoodCounter = 0;    // Clear counter (Thread is not hung)

        if((retStatus = SetCommand(command, value, delay)) == EOK)
        {
            // If command was valid
            if((retStatus = m_CommandSync.Signal(true)) == EOK)
            {
                // Post that a new command is avaliable
                Log(LOG_DEV_DATA, "Signaled command synchronization object\n");
            }
            else Log(LOG_ERRORS, "Failed to signal command synchronization object because: %s\n", strerror(retStatus));
        }

        if((status = m_CommandSync.Release()) == EOK)
        {
            // Unlocked value (don't use status if SetCommand() fails and this is Ok we get a false good return vaule
            Log(LOG_DEV_DATA, "Released command synchronization object\n");
        }
        else Log(LOG_ERRORS, "Failed to release command synchronization object because: %s\n", strerror(status));

        if((retStatus == EOK) || (status != EOK)) retStatus = status;   // Preserve previous failure or if none save last failure
    }
    else
    {
        m_itsAllGoodCounter++;  // Thread may be hung
        Log(LOG_ERRORS, "Failed to acquire command synchronization object because: %s\n", strerror(retStatus));
    }

    Log(LOG_FN_ENTRY, "Motor::Command(%s, %s, %3.2f, %u) done\n", GetName().c_str(), command.c_str(), value, delay);

    return(retStatus);
}

void Motor::SetCommFault(const bool value)
{
    // if initial detection of the communication fault and it has ocurred more than 10 times
    Log(LOG_FN_ENTRY, "Motor::SetCommFault(%d)\n", value);

    if(m_motorCommFaultLimit > 0)
    {
        // If fault reporting is enabled
        if(value)
        {
            // Increment fault counter and check agains limit
            if(++m_motorCommFaultCount >= m_motorCommFaultLimit)
            {
                // Set fault
                if(m_commFaultStatus == false)
                {
                    // Fault not currently set
                    WriteData(m_commFault, "1");
                }
                m_commFaultStatus = value;  // Update fault state
            }
        }
        else
        {
            m_motorCommFaultCount = 0;

            if(m_commFaultStatus == true)
            {
                // Cleared fault
                WriteData(m_commFault, "0");
            }
            m_commFaultStatus = value;  // Update fault state
        }
    }
    else Log(LOG_ERRORS, "Communication fault reporting is disabled\n");

    Log(LOG_FN_ENTRY, "Motor::SetCommFault(%d) done\n", value);
}

const INT32 Motor::SetSpeed(float mph)
{
    // convert the MPH to RPM
    Log(LOG_FN_ENTRY, "Motor::SetSpeed(%3.2f, %s)\n", mph, GetName().c_str());
    return(SetRPM( MphToRpm(mph)));
}

const INT32 Motor::SetTorque(float torque)
{
    // create the message to send
    Log(LOG_FN_ENTRY, "SetTorque(%3.2f)\n", torque);

//  const float actTorque = -(torque * m_torqueBitsPerPound);
    float actTorque = -(torque * m_torqueBitsPerPound);

    // limit  -2048 to +2047
    if(actTorque < -2048.0) actTorque = -2048.0;
    else if(actTorque > 2047.0) actTorque = 2047.0;

    char buffer[MOTOR_BUF_SZ];
    CreateMessage(buffer, sizeof(buffer), GetTorqueCommand(), (INT32)actTorque);

    INT32 status = SendVariableCommand(buffer, "TorqueCommand");

    Log(LOG_FN_ENTRY, "SetTorque(%3.2f -> %3.2f) done\n", torque, actTorque);
    return(status);
}

const INT32 Motor::SetRPM(float rpm)
{
    // create the message to send
    Log(LOG_FN_ENTRY, "Motor::SetRPM(%3.2f)\n", rpm);

    char buffer[MOTOR_BUF_SZ];
    CreateMessage(buffer, sizeof(buffer), GetSpeedCommand(), (INT32)rpm);

    INT32 status = SendVariableCommand(buffer, "VelocityCommand");

    Log(LOG_FN_ENTRY, "Motor::SetRPM(%3.2f) done\n", rpm);
    return(status);
}

void Motor::SetEnquiryCommand(const std::string& command)
{
    Log(LOG_FN_ENTRY, "%s, Motor::SetEnquiryCommand(%s)\n", GetName().c_str(), command.c_str());
    m_enquiry = command;
    Log(LOG_FN_ENTRY, "%s, Motor::SetEnquiryCommand(%s) done\n", GetName().c_str(), command.c_str());
}

const std::string Motor::GetEnquiryCommand(void) const
{
    return(m_enquiry);
}

const INT32 Motor::SetCommand(const std::string &command, const std::string &value, const UINT32 delay /*= 0*/)
{
    // update the values in the system
    INT32 status = EINVAL;

    Log(LOG_FN_ENTRY, "%s, Motor::SetCommand(%s, %s, %u)\n", GetName().c_str(), command.c_str(), value.c_str(), delay);

    if(command == MOTOR_STATUS_ENQUIRE)
    {
        // If the command is valid
        status = EOK;
        m_commandDelay = delay;         // Set delay
        SetEnquiryCommand(value);   // Set commnand
    }
    else Log(LOG_ERRORS, "Invalid command %s\n", command.c_str());

    Log(LOG_FN_ENTRY, "%s, Motor::SetCommand(%s, %s, %u) done\n", GetName().c_str(), command.c_str(), value.c_str(), delay);

    return(status);
}

const INT32 Motor::SetCommand(const std::string &command, const float value, const UINT32 delay /*= 0*/)
{
    // update the values in the system
    INT32 status = EINVAL;

    Log(LOG_FN_ENTRY, "Motor::SetCommand(%s, %3.2f, %u)\n", command.c_str(), value, delay);

    if((command == MOTOR_STATUS_SPEED) || (command == MOTOR_STATUS_RPM) || (command == MOTOR_STATUS_TORQUE) ||
       (command == MOTOR_STATUS_ENABLE) || (command == MOTOR_STATUS_DISABLE))
    {
        status = EOK;

        if((command == MOTOR_STATUS_ENABLE) || (command == MOTOR_STATUS_DISABLE))
        {
            // Clear comamnd value and delay if enabling or disabling motor
            m_commandDelay = 0;
            m_commandValue = 0.0;
        }
        else
        {
            // Otherwise set value and delay
            m_commandDelay = delay;
            m_commandValue = value;
        }
        m_command = command;    // Set command
    }
    else Log(LOG_ERRORS, "Invalid command: %s\n", command.c_str());

    Log(LOG_FN_ENTRY, "Motor::SetCommand(%s, %3.2f, %u) done\n", command.c_str(), value, delay);

    return(status);
}

std::string Motor::GetCommand(void)
{
    INT32 status = EOK;
    std::string command("AcquireFail");
    Log(LOG_FN_ENTRY, "Motor::GetCommand()\n");
    if((status = m_UpdateSynch.Acquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired update synchronization object\n");
        command = m_command;
        if((status = m_UpdateSynch.Release()) == EOK) Log(LOG_DEV_DATA, "Released update synchronization object\n");
        else Log(LOG_ERRORS, "Failed to release update synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire update synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "Motor::GetCommand(%s)\n", command.c_str());
    return(command);
}

const float Motor::GetValue(void)
{
    float value = 0.0;
    INT32 status = EOK;
    Log(LOG_FN_ENTRY, "Motor::GetValue(%s)\n", GetName().c_str());

    if((status = m_UpdateSynch.Acquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired update synchronization object\n");
        value = m_commandValue;
        if((status = m_UpdateSynch.Release()) == EOK) Log(LOG_DEV_DATA, "Released update synchronization object\n");
        else Log(LOG_ERRORS, "Failed to release update synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire update synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "Motor::GetValue(%s) returning %3.2f\n", GetName().c_str(), value);
    return(value);
}

const UINT32 Motor::GetDelay(void)
{
    UINT32 delay = 0;
    INT32 status = EOK;
    Log(LOG_FN_ENTRY, "Motor::GetValue(%s)\n", GetName().c_str());

    if((status = m_UpdateSynch.Acquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired update synchronization object\n");
        delay = m_commandDelay;
        if((status = m_UpdateSynch.Release()) == EOK) Log(LOG_DEV_DATA, "Released update synchronization object\n");
        else Log(LOG_ERRORS, "Failed to release update synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire update synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "Motor::GetValue(%s) returning %u\n", GetName().c_str(), delay);

    return(delay);
}

void Motor::SetEnquiryResponse(const SerialString_t& response)
{
    m_enquiryResponse = (char*)response.c_str();
}

const INT32 Motor::WriteData(const std::string& tag, const std::string value)
{
    INT32 status = EOK, retStatus = status;
    Log(LOG_FN_ENTRY, "Motor::WriteData(%s, %s)\n", tag.c_str(), value.c_str());

    if((retStatus = m_NdbSync.TryAcquire()) == EOK)
    {
        if(m_iNDB)
        {
            // Named Data Broker inerface is valid
            std::string response;

            if((status = m_iNDB->Write(tag, value, response, true)) == EOK)
            {
                // Wrote value
                Log(LOG_DEV_DATA, "Wrote %s = %d\n", m_commFault.c_str(), value.c_str());
            }
            else
            {
                retStatus = ECOMM;  // Failed to send data
                Log(LOG_ERRORS, "Failed to write %s = %d\n", m_commFault.c_str(), value.c_str());
            }
            if((status = m_NdbSync.Release()) != EOK) Log(LOG_ERRORS, "Failed to release named data broker synchronization object\n");
            if((status != EOK) && (retStatus == EOK)) retStatus = status;   // Last operation failed (previous ones are Ok, prevents losing failues communicating)
        }
        else
        {
            retStatus = ENOTCONN;   // Not connected to Named Data broker
            Log(LOG_ERRORS, "Named data broker object was not initialized (data not updated)\n");
        }
    }
    else Log(LOG_ERRORS, "Failed to acquire named data broker synchronization object\n");

    Log(LOG_FN_ENTRY, "Motor::WriteData(%s, %s) done\n", tag.c_str(), value.c_str());

    return(retStatus);
}

float Motor::MphToRpm( float mph) const
{
    return( mph * m_mphToRpm);
}
