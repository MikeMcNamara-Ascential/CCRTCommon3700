//******************************************************************************
// Description:
//    Class responsible for controlling Allen Bradley motors on an electric roll/brake machine
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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/AB/ABMotorController.cpp $
// 
// 7     1/18/07 2:53p Rwiersem
// Changes for the 07011901 core release:
// 
// - Added a call to GetDataTag() for the first parameter to Write() in
// CheckStatus().
// 
// 6     10/26/06 11:24a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 5     4/13/06 4:21a Cward
// cleanup usage block
//
// 4     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:00p Rwiersem
// Removed the unimplemented ReloadConfiguration method.  Added the
// methods needed to reload the configuration items without restarting the
// process.
//
// 3     8/17/05 8:24p Cward
// Added Clear Faults Pushbutton processing. Removed javadocs comments
// because they duplicate those in the header file
//
// 2     7/20/05 9:19p Cward
// Added support for axle speed matching. Fixed motor loading when axle
// speed amtching is disabled
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
// 3     2/18/05 12:51p Bmeinke
// Call MotorController::Command() at the end of the Command() method
// instead of returning "unsupported"
//
// 2     1/17/05 9:59a Bmeinke
// Removed references to Simulator Switch (not present on CCRT)
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 3     8/30/04 3:53p Bmeinke
// Publish() now calls the base MotorController::Publish() method (was
// calling BepServer::Publish())
//
// 2     7/23/04 3:13p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 4     7/21/04 3:28p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
//
// 3     6/28/04 8:34a Bmeinke
// Enabled motor loading and fixed up some syntax errors in the torque
// loading block
//
// 2     6/17/04 11:30p Bmeinke
// Removed MAchineType check inside Register
// Added a new command: CommandClearFaults for easier debugging/testing
// Changed some int's to uint32_t's to remove compiler warnings
// Added motor torque loading capability (disabled until can be tested)
//
// 1     6/11/04 4:11p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 1     1/20/04 8:51a Bmeinke
// Adapted from ABMotorController.h to seperate implementation from the
// header
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================

//-------------------------------------------------------------------------------------------------
#include "ABMotorController.h"
#include <stdlib.h>     // min

//-------------------------------------------------------------------------------------------------
ABMotorController::ABMotorController() : MotorController(),
m_motorConfig(NULL), m_clearFaultsOnZeroSpeed(false), m_minBrakeAssistSpeed(5.0), m_brakeAssistForce(200.0),
m_maxPositiveTorqueLimit(100.0), m_maxNegativeTorqueLimit(-100.0),
m_torqueAssistMinMPH(10.0), m_boost(false), m_brakingFactor(0.8),
m_masterMotor( LeftFront), m_interMotorSpeedDelay(0),
m_lossCompensationMotorDelay(2000), m_lossCompensationSpeedCheckDelay(2000),
m_lossCompensationSpeedCheckCount(0), m_lossCompensationSpeedCheckCurrentCount(0),m_autoDriveAxleSwitchDelay(750),
m_lcValidationSpeed(3.0), m_torqueModeSpeedLimit(10.0), m_itsAllGood(true),
m_enableStatusChecking(false), m_enableMotorCommFault(false), m_overrideMinParameter(false),
m_enableQuickBrake(false), m_axleSpeedMatch( true), m_enableAutoDriveAxle( false), 
m_prevZeroSpeed(true),m_lostZeroSpeed(false)
{
}

//-------------------------------------------------------------------------------------------------
ABMotorController::~ABMotorController()
{
    Log(LOG_FN_ENTRY, "ABMotorController::~ABMotorController()\n");
    Terminate();
    Log(LOG_FN_ENTRY, "ABMotorController::~ABMotorController() done\n");
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::Initialize(const XmlNode *document)
{
    if(document)
    {
        // initialize the base class
        MotorController::Initialize(document);

        // initialize the serial motor controller
        Log(LOG_FN_ENTRY, "ABMotorController::Initialize()\n");
        try
        {
            // initialize the parameters
            LoadAdditionalConfigurationItems(document);
        }
        catch(XmlException &Err)
        {
            Log(LOG_DEV_DATA, "ABMotorController::Initialize Error: %s\n", Err.what());
            throw;
        }
    }
    else throw BepException("Invalid Configuration File Provided (NULL pointer)");

    Log(LOG_FN_ENTRY, "ABMotorController::Initialize() done\n");
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    const XmlNode *setup = document->getChildren().getNode("Setup");
	const XmlNode *parameters = setup->getChild("ElectricMotorParameters");

    m_minBrakeAssistSpeed = atof(parameters->getChild("MinBrakeAssistSpeed")->getValue().c_str());
    m_brakeAssistForce = atof(parameters->getChild("BrakeAssistForce")->getValue().c_str());
    m_torqueAssistMinMPH = atof(parameters->getChild("TorqueAssistMinSpeed")->getValue().c_str());
    SetLossCompensationSpeedCommandDelay(atol(parameters->getChild("InterMotorLossCompensationDelay")->getValue().c_str()));
    m_lossCompensationSpeedCheckDelay = atol(parameters->getChild("SpeedCheckDelayTime")->getValue().c_str());
    m_lcValidationSpeed = atof(parameters->getChild("LCRollerValidationSpeed")->getValue().c_str());

    // Set up the count for checking the motors in a LC test
    m_lossCompensationSpeedCheckCount = ((GetLossCompensationSpeedCommandDelay()*4) +
                                         m_lossCompensationSpeedCheckDelay) / m_updateRate;

    // get the reference to the drive parameters
    m_driveParameters = setup->getChild("DriveParameters")->getChildren();

    // get the reference to the motor configuration
    m_motorConfig = setup->getChild("Motors");

    SetBrakingFactor(atof(parameters->getChild("BrakingFactor")->getValue().c_str()));
    EnableQuickBrake(BposReadInt(parameters->getChild("EnableQuickBrake")->getValue().c_str()));
    SetTorqueModeSpeedLimit(atof(parameters->getChild("TorqueModeSpeedLimit")->getValue().c_str()));

    try
    {
        m_enableStatusChecking = (parameters->getChild("EnableStatusChecking")->getValue() == Enabled);
    }
    catch(BepException& Err)
    {
        // if the tag is missing from the configuration file, use the default
        // set in the constructor
    }
    try
    {
        m_axleSpeedMatch = (atoi(parameters->getChild("AxleSpeedMatch")->getValue().c_str()) != 0);
    }
    catch( BepException &err)
    {
        m_axleSpeedMatch = true;
    }
    // Set the parameter indicating if faults should be cleared at zero speed
    bool clearFaults = false;
    try
    {
        clearFaults = atob(parameters->getChild("ClearFaultsOnZeroSpeed")->getValue().c_str());
    }
    catch(XmlException &excpt)
    {
        clearFaults = false;
    }
    ClearFaultsOnZeroSpeed(&clearFaults);

    try
    {
        m_enableAutoDriveAxle = (parameters->getChild("EnableAutoDriveAxle")->getValue() == Enabled);
    }
    catch(BepException& Err)
    {
        // if the tag is missing from the configuration file, use the default
        // set in the constructor
    }
    try
    {
        m_autoDriveAxleSwitchDelay = (atol(parameters->getChild("AutoDriveAxleSwitchDelay")->getValue().c_str()));
    }
    catch(BepException& Err)
    {
        // if the tag is missing from the configuration file, use the default
        // set in the constructor
    }
}

//-------------------------------------------------------------------------------------------------
const std::string ABMotorController::Register(void)
{
    INT32   stat;
    UINT32  ii;
    std::string status(BEP_FAILURE_RESPONSE), name;
    int     driveRefIndexes[] = { 4,0,5,0,0,0};
	int     phase2DriveRefIndexes[] = {5,0,6,0,0,0};

    Log(LOG_FN_ENTRY, "ABMotorController::Register()\n");

    if((stat = m_startEndSync.TryAcquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired start/end synchronizarion object\n");
        if((status = MotorController::Register()) == BEP_SUCCESS_RESPONSE)
        {
            // If not running on the proper machine, terminate
            try
            {
                if( (stat=m_motors.Lock()) == EOK)
                {
                    for( ii=LeftFront; ii<m_motors.size(); ii++)
                    {
                        // Terminate all motors
                        int retries = 0;
                        while( (m_motors[ii] != NULL) && (m_motors[ii]->GetId() != -1) )
                        {
                            if((m_motors[ii]->Terminate() != EOK) && retries++ < 5)
                            {
                                Log(LOG_ERRORS, "Waiting for %s to acknowledge Terminate()\n", m_motors[ii]->GetName().c_str());
                                BposSleep( 100);
                            }
                        }

                        if( m_motors[ ii] != NULL)
                        {
                            delete m_motors[ii];
                            m_motors[ii] = NULL;
                        }
                    }
                    m_motors.clear();

                    XmlParser   parser;
                    XmlNode     *motorConfig;
                    string motorType(GetDataTag("ABMotorType"));
					Log(LOG_DEV_DATA, "Creating motors of type: %s", motorType.c_str());
                    for( ii=LeftFront; ii<(UINT32)BposReadInt(GetDataTag("MotorCount").c_str()); ii++)
                    {
                        /**
                         * Create new Motor objects with proper speed reference index values. The
                         *  speed reference index is used with the AB drives for speed matching
                         *  (i.e. following a master drive).
                         *  The LF drive has a speed reference index of 4 (5 for AB700s Phase 2)
                         *  The LR drive has a speed reference index of 5 (6 for AB700s Phase 2)
                         *  All other drives have no speed reference index
                         */
                        if(!motorType.compare("AB700s"))
                            m_motors.push_back(new AB700sMotor( driveRefIndexes[ ii%6]));
						else if(!motorType.compare("AB700s-Phase 2"))
							m_motors.push_back(new AB700sMotor(phase2DriveRefIndexes[ii%6]));
                        else if(!motorType.compare("AB700"))
                            m_motors.push_back(new AB700Motor( driveRefIndexes[ ii%6]));
                        else   // Unknown type, default to 700s
                            m_motors.push_back(new AB700sMotor( driveRefIndexes[ ii%6]));
                        // Initialize motor
                        name = MotorDescriptions[ii] + "Motor";
                        Log(LOG_DEV_DATA, "Initializing %s\n", name.c_str());
                        motorConfig = const_cast<XmlNode*>(m_motorConfig->getChild(MotorDescriptions[ii] + "Motor"));
                        m_motors[ii]->Initialize(motorConfig);  // Initialize the motor
                        m_motors[ ii]->MaxTorqueSpeed(&m_torqueModeSpeedLimit);
                        m_motors.StoreMaxTorqueSpeedLimit(m_torqueModeSpeedLimit);
                    }

                    for( ii=LeftFront; ii<m_motors.size(); ii++)
                    {
                        // Launch all new Motors in threads
                        Log(LOG_DEV_DATA, "Launching %s\n", m_motors[ii]->GetName().c_str());
                        /**
                         * Launch a new motor thread, but tell it to not auto-delete itself. This is
                         * so we can safely detect when the thread is dead
                         */
                        m_motors[ii]->Resume( false);
                    }
                    ClearOperationalFaults();
                    // Default to Boost mode
                    XmlNode defaultMode(MOTOR_MODE, BOOST_MODE);
                    Command(&defaultMode);

                    SetStatus(BEP_RUNNING_STATUS);

                    if((stat = m_startEndSync.Release()) == EOK)
                    {
                        Log(LOG_DEV_DATA, "Released synchronizarion object\n");
                    }
                    else Log(LOG_ERRORS, "Failed to release start/end synchronizarion object because: %s\n", strerror(stat));

                    m_motors.Unlock();
                }
                else
                {
                    Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(stat));
                }
            }
            catch(BepException& Err)
            {
                Log(LOG_ERRORS, "ABMotorController::Register() Failed because: %s\n", Err.what());
            }
        }
        else Log(LOG_ERRORS, "MotorController::Register() failed with return value of %s\n", status.c_str());
    }
    else Log(LOG_ERRORS, "Failed to acquire start/end synchronizarion object because: %s\n", strerror(stat));

    Log(LOG_FN_ENTRY, "ABMotorController::Register() done\n");

    return(status);
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::Reset(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::Reset()\n");
    MotorController::Reset();
    MotorController::Initialize(GetConfigFile());
    Log(LOG_FN_ENTRY, "ABMotorController::Reset() done\n");
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::Terminate(void)
{
    std::string response;
    const UINT32 retryCount = 5;
    UINT32 ii, retries;

    Log(LOG_FN_ENTRY, "ABMotorController::Terminate()\n");

    MotorController::Terminate();

    if(m_dataComm)  m_dataComm->Write(GetDataTag("EnableAllRollDrives"), False, response, true);
    else            Log(LOG_ERRORS, "Named data broker object is uninitialized\n");

    if( (errno=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) 
            Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", 
                m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<m_motors.size(); ii++)
        {
            // Terminate all motors
            retries = 0;
            while( (m_motors[ii] != NULL) && (m_motors[ii]->GetId() != -1) )
            {
                if((m_motors[ii]->Terminate() != EOK) && retries++ < retryCount)
                {
                    Log(LOG_ERRORS, "Waiting for %s to acknowledge Terminate()\n", m_motors[ii]->GetName().c_str());
                    BposSleep( 100);
                }
                else
                {
                    Log(LOG_DEV_DATA, "Waiting for %s to terminate", m_motors[ii]->GetName().c_str());
                    BposSleep(500);
                }
            }

            if( m_motors[ ii] != NULL)
            {
                delete m_motors[ii];
                m_motors[ii] = NULL;
            }
        }
        m_motors.clear();

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::Terminate() done\n");
}

//-------------------------------------------------------------------------------------------------
const std::string ABMotorController::Command(const XmlNode *node)
{
    std::string status(BEP_INVALID_RESPONSE);
    int ii;

    Log(LOG_FN_ENTRY, "ABMotorController::Command(%s)\n", node->ToString().c_str());

    const std::string name = node->getName(), value = node->getValue();
    const INT32 rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());
    if(name == MOTOR_MODE)
    {
        // Make sure this is a valid motor mode
        if((value == BOOST_MODE) || (value == QUICK_BRAKE_MODE) || (value == REAR_BRAKE_ASSIST_MODE) ||
           (value == SPEED_MODE) || (value == TORQUE_MODE) || (value == NO_LIMIT_TORQUE_MODE) || 
           (value == LOSSCOMPENSATION_MODE) || (value == UPLOAD_PARAMETERS_MODE) || (value == INDIVIDUAL_CONTROL_MODE))
        {
            Log(LOG_DEV_DATA, "Commanded to %s Mode\n", value.c_str());

            const std::string curMode = GetMotorMode();

            if( (errno=m_motors.Lock()) == EOK)
            {
                if( value == LOSSCOMPENSATION_MODE)
                {
                    // Switching to Loss Compensation mode
                    m_lossCompensationSpeedCheckCurrentCount = 0;
                }
                else if( (value != QUICK_BRAKE_MODE) && (GetMotorMode() == QUICK_BRAKE_MODE))
                {
                    // Switching out of Quick Brake mode
                    m_overrideMinParameter = false;
                }
                else if( (value == QUICK_BRAKE_MODE) && (IsQuickBrakeEnabled()) )
                {
                    XmlElement  stopNode( COMMAND_SPEED, "0");
                    SetData( &stopNode);
                }
                else if((GetMotorMode() == SPEED_MODE) && ((value != SPEED_MODE) && (value != INDIVIDUAL_CONTROL_MODE)) )
                {
                    // Switching out of speed mode
                    SetMotorSpeedCommandDelay(0);   // Clear motor speed command delay
                }
                // Determine if the torque speed limit should be removed
                Log(LOG_DEV_DATA, "Determining if the speed limit should be changed");
                INT32 wheelCount = min(GetWheelCount(), (INT32)m_motors.size());
                Log(LOG_DEV_DATA, "Using wheel count: %d  (GetWheelCount: %d, m_motors.size: %d)", 
                    wheelCount, GetWheelCount(), (INT32)m_motors.size());
                float speedRef = m_motors.MaxTorqueSpeedLimit();
                Log(LOG_DEV_DATA, "Maximum torque mode speed limit (default): %f", speedRef);
                if(!value.compare(NO_LIMIT_TORQUE_MODE))
                {   // Reset the speed limit to zero
                    speedRef = 0.0;
                }
                // Set the torque speed limit
                Log(LOG_DEV_DATA, "Setting maximum torque mode speed limit to: %f", speedRef);
                for(INT32 index = LeftFront; index < wheelCount; index++)
                {
                    if(m_motors[index] != NULL) m_motors[index]->MaxTorqueSpeed(&speedRef);
                }
                Log(LOG_DEV_DATA, "Done adjusting torque mode speed limit");
                m_motors.Unlock();

                SetMotorMode( value);       // Set the motor mode
                SetData( name, value);

                status = BEP_SUCCESS_RESPONSE;  // It's all good
            }
            else
            {
                Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
            }
        }
        else Log(LOG_ERRORS, "Invalid Motor Mode Received: %s\n", value.c_str());
    }
    else if(name == COMMAND_SPEED)
    {
        Log(LOG_DEV_DATA, "Commanded to Speed %s\n", value.c_str());
        SetData(node);
        for( ii = LeftFront; ii < rolls; ii++)
        {
            SetCommandedSpeed( ii, atof(value.c_str()));            // Set command value
        }
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == COMMAND_TORQUE)
    {
        Log(LOG_DEV_DATA, "Commanded to Torque %s\n", value.c_str());
        SetData(node);
        for( ii = LeftFront; ii < rolls; ii++)
        {
            SetCommandedTorque( ii, atof(value.c_str()) );          // Set command value
        }
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == COMMAND_CLUTCH)
    {
        Log(LOG_DEV_DATA, "Commanded to Clutch %s\n", value.c_str());
        status = (Clutch(value) == EOK) ? BEP_SUCCESS_RESPONSE : BEP_FAILURE_RESPONSE;
    }
    else if(name == COMMAND_MOTOR_DELAY)
    {
        // Commanding motor delay
        Log(LOG_DEV_DATA, "Commanded to set motor delay to %s\n", value.c_str());
        SetMotorSpeedCommandDelay(value);
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == MOTOR_STATUS_ENQUIRE)
    {
        // Commanding enquire
        Log(LOG_DEV_DATA, "Commanded motor status enquire %s\n", value.c_str());
        SetMotorMode(name);
        SetSpecialCommand(value);
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(name == "CommandClearFaults")
    {
        ClearOperationalFaults();   // Clear operational faults
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("LeftFrontSpeedValue"))
    {
        SetCommandedSpeed( LeftFront, atof(value.c_str()));            // Set command value
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("RightFrontSpeedValue"))
    {
        SetCommandedSpeed( RightFront, atof(value.c_str()));            // Set command value
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("LeftRearSpeedValue"))
    {
        if(!MachineType().compare(MACHINE_TYPE_3700) ) //!GetDataTag("ABMotorType").compare("AB700"))
        {
            SetCommandedSpeed( LeftFront, atof(value.c_str()));            // Set command value
        }
        else
        {
            SetCommandedSpeed( LeftRear, atof(value.c_str()));            // Set command value
        }
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("RightRearSpeedValue"))
    {
        if(!MachineType().compare(MACHINE_TYPE_3700) ) //!GetDataTag("ABMotorType").compare("AB700"))
        {
            SetCommandedSpeed( RightFront, atof(value.c_str()));            // Set command value
        }
        else
        {
            SetCommandedSpeed( RightRear, atof(value.c_str()));            // Set command value
        }
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("LeftFrontMotorMode"))
    {
        SetMotorMode(value, LeftFront);
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("RightFrontMotorMode"))
    {
        SetMotorMode(value, RightFront);
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("LeftRearMotorMode"))
    {
        if(!MachineType().compare(MACHINE_TYPE_3700) ) //!GetDataTag("ABMotorType").compare("AB700"))
        {
            SetMotorMode(value, LeftFront);
        }
        else
        {
            SetMotorMode(value, LeftRear);
        }
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("RightRearMotorMode"))
    {
        if(!MachineType().compare(MACHINE_TYPE_3700) ) //!GetDataTag("ABMotorType").compare("AB700"))
        {
            SetMotorMode(value, RightFront);
        }
        else
        {
            SetMotorMode(value, RightRear);
        }
		status = BEP_SUCCESS_RESPONSE;
    }
    else if(!name.compare("TorqueModeSpeedLimit"))
	{
		Log(LOG_DEV_DATA, "Setting torque mode speed limit to %s mph", value.c_str());
		SetTorqueModeSpeedLimit(atof(value.c_str()));
		status = BEP_SUCCESS_RESPONSE;
	}
    else
    {
        status = MotorController::Command( node);
    }

    if(status != BEP_SUCCESS_RESPONSE) Log(LOG_ERRORS, "ABMotorController::Command(%s) error: %s\n",
                                           node->ToString().c_str(), status.c_str());

    Log(LOG_FN_ENTRY, "ABMotorController::Command(%s) [status = %s] done\n", node->ToString().c_str(), status.c_str());

    return(status);
}

//-------------------------------------------------------------------------------------------------
const std::string ABMotorController::Publish(const XmlNode *node)
{
    std::string status;
    const XmlNode Node(MOTOR_MODE, BOOST_MODE);
    Log(LOG_FN_ENTRY, "ABMotorController::Publish(%s)\n", node->ToString().c_str());

    if(node->getName() == GetDataTag("RollsDownElevatorsUp") && (node->getValue() == False))
    {
        // Rolls were down, now they are going up
        ClearOperationalFaults();   // Clear operational faults
        m_itsAllGood = true;        // Reset status to recover from failure if possible
    }
    else if(node->getName() == GetDataTag("RollsUpElevatorDown") && (node->getValue() == True))
    {
        // Rolls are down, set to boost mode
        Command(&Node);
    }
    else if((node->getName() == GetDataTag("PowerSenseLostToIORack") && (node->getValue() == True)) ||
            (node->getName() == GetDataTag("MotorTaskHeartbeatFault") && (node->getValue() == True)))
    {
        // Power lost, motor task has faulted or Drives were disabled by PLC, disable and set to boost mode
        Disable();
        Command(&Node);
    }
    else if( node->getName() == GetDataTag("DriveWheel"))
    {
        string driveAxle, response;
        if( node->getValue() == "Rear")
        {
            m_masterMotor = LeftRear;
            m_slaveMotor = LeftFront;
            driveAxle = "1";
        }
        else
        {
            m_masterMotor = LeftFront;
            m_slaveMotor = LeftRear;
            driveAxle = "0";
        }
        if(m_dataComm)  m_dataComm->Write("DriveAxleToPlc", driveAxle, response, true);
    }
    else if( node->getName() == GetDataTag("DrivesEnabled"))
    {
        if( node->getValue() == False)
        {
            // Drives were disabled by PLC, disable and set to boost mode
            Disable();
            Command(&Node);
        }
        else
        {
            // Drive enabled by PLC
            Enable();
        }
    }
    // If someone just pushed the "Clear Faults" pushbutton
    else if((node->getName() == GetDataTag( "ClearMotorFaults")) && (True == node->getValue()))
    {
        if(True == ReadSubscribeData("Zerospeed"))
        {
            ClearOperationalFaults();   // Clear operational faults
        }
        else
        {
            Log( LOG_ERRORS, "Got %s, but not at zerospeed...cannot clear faults at speed\n",
                 GetDataTag( "ClearMotorFaults").c_str());
        }
    }
    else if( node->getName() == GetDataTag("Zerospeed"))
    {
        if( (node->getValue() == False)  && m_prevZeroSpeed && m_enableAutoDriveAxle)
        {//signal we have just lost zero speed to allow drive axle switching
            m_prevZeroSpeed = false;
            m_lostZeroSpeed = true;
            Log(LOG_DEV_DATA, "Lost Zero Speed, setting m_lostZeroSpeed true, m_prevZeroSpeed false \n");
        }
        else if(node->getValue() == True)
        {
            m_prevZeroSpeed = true;
            Log(LOG_DEV_DATA, "Machine at Zero Speed, setting m_prevZeroSpeed true\n");
        }
    }

    status = MotorController::Publish(node);

    Log(LOG_FN_ENTRY, "ABMotorController::Publish(%s) done\n", node->ToString().c_str());

    return( status);
}

//-------------------------------------------------------------------------------------------------
inline const std::string ABMotorController::GetMotorMode(const INT8 roll)
{
    std::string testMode, motorMode;
    INT32 ii, status, rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());
    Log(LOG_FN_ENTRY, "GetMotorMode(%d)\n", roll);

    if((status = m_modeAccess.Acquire()) == EOK)
    {
        // Acquired exclusive access to mode
        char tag[32];
        if(roll == All)
        {
            // Get mode for all rolls
            motorMode = ReadSubscribeData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[LeftFront].c_str())));
            for( ii=LeftFront; (ii<rolls) && (motorMode != INDIVIDUAL_CONTROL_MODE); ii++)
            {
                // If mode is not same for all rolls, it must be individual control
                testMode = ReadSubscribeData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[ii].c_str())));
                if(motorMode != testMode) motorMode = INDIVIDUAL_CONTROL_MODE;
            }
        }
        else if(roll < rolls)
        {
            // Requesting 1 roll
            motorMode = ReadSubscribeData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[roll].c_str())));
        }
        else Log(LOG_ERRORS, "Invalid roll requested %d, rolls = %d\n", roll, rolls);

        if((status = m_modeAccess.Release()) != EOK)
            Log(LOG_ERRORS, "Failed to release mode access synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire mode access synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "GetMotorMode(%s, %d) done\n", motorMode.c_str(), roll);
    return(motorMode);
}

//-------------------------------------------------------------------------------------------------
inline const float ABMotorController::GetCommandValue(const std::string& command)
{
    Log(LOG_FN_ENTRY, "ABMotorController::GetCommandValue(%s)\n", command.c_str());

    float commandedValue = atof(ReadSubscribeData(command).c_str());
    Log(LOG_DEV_DATA, "Command value = %3.2f\n", commandedValue);

    Log(LOG_FN_ENTRY, "ABMotorController::GetCommandValue(%s, %3.2f) done\n", command.c_str(), commandedValue);
    return(commandedValue);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetCommandValue(const std::string& command, const float commandValue)
{
    char data[16];
    Log(LOG_FN_ENTRY, "ABMotorController::SetCommandValue(%s, %3.2f)\n", command.c_str(), commandValue);

    SetCommandValue(command, CreateMessage(data, sizeof(data), "%3.2f", commandValue));

    Log(LOG_FN_ENTRY, "ABMotorController::SetCommandValue(%s, %3.2f) done\n", command.c_str(), commandValue);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetCommandedSpeed(int driveIdx, const float speedValue)
{
    if( (errno=m_motors.Lock()) == EOK)
    {
        SetCommandValue(GetDataTag(MotorDescriptions[driveIdx] + "SpeedValue"), speedValue);
        //m_motors[ driveIdx]->CommandSpeed( speedValue);

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
    }
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetCommandedTorque(int driveIdx, const float torqueValue)
{
    if( (errno=m_motors.Lock()) == EOK)
    {
        SetCommandValue(GetDataTag(MotorDescriptions[driveIdx] + "TorqueValue"), torqueValue);
        //m_motors[ driveIdx]->CommandTorque( torqueValue);

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
    }
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetCommandValue(const std::string& command, const std::string &commandValue)
{
    Log(LOG_FN_ENTRY, "ABMotorController::SetCommandValue(%s, %s)\n", command.c_str(), commandValue.c_str());

    SetData(command, commandValue);

    Log(LOG_FN_ENTRY, "ABMotorController::SetCommandValue(%s, %s) done\n", command.c_str(), commandValue.c_str());
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetMotorMode(const std::string &motorMode, const INT8 roll)
{
    INT32 status;
    std::string newMode = motorMode;
    Log(LOG_FN_ENTRY, "SetMotorMode(%s, %d)\n", motorMode.c_str(), roll);

    INT8 ii = LeftFront, end = BposReadInt(GetData(GetDataTag("RollCount")).c_str());   // Default to Left Front

    if(roll != All)
    {
        // Comamnding 1 roll
        if((roll < end) && (roll >= LeftFront))
        {
            // Roll index is valid
            ii = roll;      // Start at roll desired
            Log(LOG_DEV_DATA, "Setting mode for %s only\n", MotorDescriptions[ii].c_str());
        }
        else Log(LOG_ERRORS, "Roll value [%d] passed in was incorrect, using %d.  There are %d rolls\n", roll, ii, end);

        end = ii + 1;   // Allow 1 loop since not roll != all
    }

    if((status = m_modeAccess.Acquire()) == EOK)
    {
        // Acquired exclusive access to mode
        char tag[32];
        Log(LOG_DEV_DATA, "Acquired mode access synchronization object.  ii = %d, end = %d\n", ii, end);
        for(; ii < end; ii++)
        {
            // Loop through all motors and set motor mode (may only be 1 motor)
            Log(LOG_DEV_DATA, "Setting mode for %s\n", MotorDescriptions[ii].c_str());
            SetData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[ii].c_str())), motorMode);
        }

        for(ii = LeftFront; (ii < end) && (motorMode != INDIVIDUAL_CONTROL_MODE); ii++)
        {
            // If mode is not same for all rolls, it must be individual control
            if(newMode != ReadSubscribeData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[ii].c_str()))))
                newMode = INDIVIDUAL_CONTROL_MODE;
        }

        if((status = m_modeAccess.Release()) != EOK)
            Log(LOG_ERRORS, "Failed to release mode access synchronization object because: %s\n", strerror(status));
    }
    else Log(LOG_ERRORS, "Failed to acquire mode access synchronization object because: %s\n", strerror(status));

    Log(LOG_FN_ENTRY, "SetMotorMode(%s, %d) done\n", motorMode.c_str(), roll);
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::ClearOperationalFaults(void)
{
    std::string tag, response;
    uint32_t ii;

    Log(LOG_FN_ENTRY, "ABMotorController::ClearOperationalFaults()\n");

    if( (errno=m_motors.Lock()) == EOK)
    {
        for( ii=LeftFront; ii<m_motors.size(); ii++)
        {
            // Clear the fault
            INT32 status;
            tag = GetDataTag(MotorDescriptions[ii] + "RollMotorOperationalFault");

            if( (m_dataComm != NULL) && ReadSubscribeData(tag) == True)
            {
                if((status = m_dataComm->Write(tag, False, response, true)) == BEP_STATUS_SUCCESS)
                {
                    Log(LOG_DEV_DATA, "Cleared %s\n", tag.c_str());
                }
                else Log(LOG_ERRORS, "Failed to clear %s\n", tag.c_str());
            }

            m_motors[ ii]->ClearFault();
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::ClearOperationalFaults() done\n");
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::UpdateMachineStatus(void)
{
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_FN_ENTRY, "ABMotorController::UpdateMachineStatus(%s = %s, %s = %s, %s = %s, %s = %s)\n",
        GetDataTag("RollsUpElevatorDown").c_str(), ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str(),
        GetDataTag("EnableAllRollDrives").c_str(), ReadSubscribeData(GetDataTag("EnableAllRollDrives")).c_str(),
        GetDataTag("DrivesEnabled").c_str(), ReadSubscribeData(GetDataTag("DrivesEnabled")).c_str(),
        GetDataTag("MotorTaskHeartbeatFault").c_str(), ReadSubscribeData(GetDataTag("MotorTaskHeartbeatFault")).c_str());

    if( (errno=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            if( m_motors[ ii] != NULL)
            {
                // Check each motor status to make sure it is still alive
                if(m_motors[ii]->IMightBeDead())
                {
                    if(m_enableStatusChecking) m_itsAllGood = false;
                    Log(LOG_ERRORS, "Motor %s is in a bad condition!\n", m_motors[ii]->GetName().c_str());
                }
                // If at zero speed, check if a fault needs to be cleared
                if(m_motors[ii]->IsFaulted() && !ReadSubscribeData(ZEROSPEED_TAG_TO_PLC).compare("1") && ClearFaultsOnZeroSpeed())
                {
                    m_motors[ii]->ClearFault();
                }
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(errno));
    }

    try
    {
        // Enable the drives
        std::string response, enableStatus;

        // If retainers raised and the drives are not enabled, enable them
        if(((ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == True) &&
            (ReadSubscribeData(GetDataTag("DrivesEnabled")) == False)) && m_itsAllGood)
        {
            Log( LOG_DEV_DATA, "Need to enable the drives\n");
            enableStatus = Enabled;
        }
        // If rolls are down and either drives are enabled or PLC says they are enabled or just big trouble
        else if(((ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == False) &&
                 ((ReadSubscribeData(GetDataTag("EnableAllRollDrives")) == True) ||
                  (ReadSubscribeData(GetDataTag("DrivesEnabled")) == True))) || !m_itsAllGood)
        {
            Log( LOG_DEV_DATA, "Need to disable the drives\n");
            enableStatus = Disabled;
        }
        // If these are AB700 drives and retainers raised and drives enabled from PLC, enable the drives
        else if(!GetDataTag("ABMotorType").compare("AB700") &&
                (ReadSubscribeData(GetDataTag("DrivesEnabled")) == True) &&
                (ReadSubscribeData(GetDataTag("EnableAllRollDrives")) == False))
        {
            Enable();
            XmlNode enableNode(GetDataTag("EnableAllRollDrives"), "1");
            Publish(&enableNode);
        }

        if( enableStatus.empty() == false)
        {
            if( m_dataComm)
            {
                m_dataComm->Write(GetDataTag("EnableAllRollDrives"), Enabled, response, true);
                Log(LOG_DEV_DATA, "Drives set to %s\n", enableStatus.c_str());
            }
            else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
        }

        MotorController::UpdateMachineStatus(); // Update the internal machine status
        GetAccelerations();                     // Retrieve the acceleration info
    }
    catch(BepException &Err)
    {
        Log(LOG_ERRORS, "ABMotorController::UpdateMachineStatus() failed because: %s\n", Err.what());
    }
    Log(LOG_FN_ENTRY, "ABMotorController::UpdateMachineStatus() done\n");
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::Boost(void)
{
    INT32 status;
    bool enable = false;    // Boost enable flag (set to off by default)
    string reason;     // Reason boost was disabled
    string response;   // Response from the requested data
    string tag, value; // Tag and value for retrieving data
    int         masterRoll = All;

    Log(LOG_FN_ENTRY, "ABMotorController::Boost()\n");
    LogInfo();              // log the system information

	// determine if we should set the drive axle based on config param
	if(m_enableAutoDriveAxle && m_lostZeroSpeed)
	{   //auto drive axle selection enabled
		m_lostZeroSpeed = false;
		DetectDriveAxle();
	}
    // Make sure conditions are OK for BOOST mode
    if( (enable=ConditionsOkForBoost(reason)) == true)
    {
        // else Enable boost to eliminate speed difference between axles
        Log(LOG_DEV_DATA, "Boost Enabled\n");
        if((status = Enable()) == EOK)
        {
            // determine the vehicle type and command the correct boost algorythm
            if((ReadSubscribeData(GetDataTag("FourWheelDriveStatus")) ==  On) ||
               (ReadSubscribeData(GetDataTag("DriveWheel")) == "All"))
            {
                // If performing an axle ratio (4x4 test) or an AWD vehicle
                AllWheelBoost();
            }
            else if(ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear")
            {
                // If Rear wheel drive vehicle
                RearWheelBoost();
                masterRoll = LeftRear;
            }
            else
            {
                // Must be front wheel drive
                FrontWheelBoost();
                masterRoll = LeftFront;
            }
            if((masterRoll != All) && MachineType().compare(MACHINE_TYPE_3700) && GetDataTag("ABMotorType").compare("AB700")) 
            {
                status = ApplyMotorLoad( CalculateTorqueLoad( masterRoll), masterRoll);

            }
        }
        else Log(LOG_DEV_DATA, "Error Enabling The Drives because: %s\n", strerror(status));
    }
    else
    {
        Log(LOG_DEV_DATA, "Boost Disabled because %s\n", reason.c_str());
        status = Disable(); // disable the drives (torque 0)
        Log(LOG_DEV_DATA, "Boost%s Disable%s The Drives\n", (status == EOK) ? "" : " Could Not", (status == EOK) ? "d" : "");
    }
    Log(LOG_FN_ENTRY, "ABMotorController::Boost() done\n");

    return(status);
}

void ABMotorController::DetectDriveAxle(void)
{
    string currentDriveAxle  = ReadSubscribeData(GetDataTag("DriveWheel"));
        // if all wheel drive not selected
        if(ReadSubscribeData(GetDataTag("AllWheelDrive")) == "0")
        {//note: assumption is that we are starting from zero speed in boost mode
            Log(LOG_DEV_DATA, "Speed:\n\tLeftFront: %f\n\tRightFront: %f\n\tLeftRear: %f\n\tRightRear: %f\n\t",
                m_speeds[LeftFront],m_speeds[RightFront],m_speeds[LeftRear],m_speeds[RightRear]);
                // determine if the drive axle is front or rear
                if(((fabs(m_speeds[LeftRear]) > m_minimumSpeedForBoost) || 
                   (fabs(m_speeds[RightRear]) > m_minimumSpeedForBoost)) &&
                   ((fabs(m_speeds[LeftFront]) < m_minimumSpeedForBoost) || 
                   (fabs(m_speeds[RightFront]) < m_minimumSpeedForBoost)))
                {//rear speed above min 
                 //and fronts are below min boost speed
                    currentDriveAxle = REAR_WHEEL_DRIVE_VALUE;
                }
                else if(((fabs(m_speeds[LeftFront]) > m_minimumSpeedForBoost) || 
                   (fabs(m_speeds[RightFront]) > m_minimumSpeedForBoost)) &&
                   ((fabs(m_speeds[LeftRear]) < m_minimumSpeedForBoost) || 
                   (fabs(m_speeds[RightRear]) < m_minimumSpeedForBoost)))
                {//front speed above min 
                 //and rears are below min boost speed
                    currentDriveAxle = FRONT_WHEEL_DRIVE_VALUE;
                }
        }
        else                   // all wheel drive selected
        {
            currentDriveAxle = ALL_WHEEL_DRIVE_TAG;
        }

        if( m_dataComm)
        {  
            std::string response;
            m_dataComm->Write(GetDataTag("DriveWheel"), currentDriveAxle, response, true);
            Log(LOG_DEV_DATA, "Drive Axle set to %s\n", currentDriveAxle.c_str());
            //wait for drive axle to be set
            while(ReadSubscribeData(GetDataTag("DriveWheel")) != currentDriveAxle)
            {  
                Log(LOG_DEV_DATA, "Waiting for Drive Axle Switch");
                BposSleep( 25);
            }
            BposSleep(m_autoDriveAxleSwitchDelay);
            //get latest speeds
            GetSpeeds();
        }
        else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
}

const INT32 ABMotorController::ApplyMotorLoad( float motorLoad, int masterRoll)
{
    INT32       status;

    if( masterRoll != All)
    {
        if(m_motors[ masterRoll] != NULL)
        {
            const char *motorName = m_motors[ masterRoll]->GetName().c_str();
            const char *slaveName = NULL;
            int slaveIndex = masterRoll;

            // Figure out which drive is the 'same axle slave'
            switch( masterRoll)
            {
            case LeftFront:     slaveIndex = RightFront;    break;
            case RightFront:    slaveIndex = LeftFront;     break;
            case LeftRear:      slaveIndex = RightRear;     break;
            case RightRear:     slaveIndex = LeftRear;      break;
            default:            slaveIndex = masterRoll;    break;
            }

            // If axle matching is disabled we need to command half the load to each side of the axle
            //  to avoid a speed split
            if( (false == m_axleSpeedMatch) && (slaveIndex != masterRoll))
            {
                // Split torque 50/50 side-to-side
                motorLoad /= 2.0;

                Log(LOG_DEV_DATA, "Commanding torque of %3.2f to %s Motor\n", motorLoad, motorName);
                if((status = m_motors[ masterRoll]->CommandTorque( motorLoad)) == EOK)
                {
                    slaveName = m_motors[ slaveIndex]->GetName().c_str();

                    Log(LOG_DEV_DATA, "Commanding torque of %3.2f to %s Motor\n", motorLoad, slaveName);
                    status = m_motors[ slaveIndex]->CommandTorque( motorLoad);
                    if(EOK == status)
                    {
                        Log( "CommandTorque(%s) success!\n", motorName);
                    }
                    else
                    {
                        Log(LOG_ERRORS, "CommandTorque(%s) (same axle slave) failed because: %s\n",
                            slaveName, strerror(status));
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "CommandTorque(%s) failed because: %s\n", motorName, strerror(status));
                }
            }
            // Axle matching is enabled, so apply the full load to the master roll
            else
            {
                Log(LOG_DEV_DATA, "Commanding torque of %3.2f to %s Motor\n", motorLoad, motorName);
                if((status = m_motors[ masterRoll]->CommandTorque( motorLoad)) == EOK)
                {
                    Log( "CommandTorque(%s) success!\n", motorName);
                }
                else
                {
                    Log(LOG_ERRORS, "CommandTorque(%s) failed because: %s\n", motorName, strerror(status));
                }
            }
        }
        else
        {
            status = EINVAL;
            Log(LOG_ERRORS, "\tMotor %d is NULL\n", masterRoll);
        }
    }
    else
    {
        Log(LOG_DEV_DATA, "\tNo loading for 4WD or all wheel drive\n");
        status = EOK;
    }

    return( status);
}

//-------------------------------------------------------------------------------------------------
float ABMotorController::CalculateTorqueLoad( int masterRoll)
{
    float       trqTotal = 0.0;
    float       trqAccel = 0.0;
    float       trqSpeed = 0.0;
    bool        inTrqSpeedBand = false;

    Log(LOG_FN_ENTRY, "MotorController::CalculateTorqueLoad - Enter (%d); accelK=%.2F spdK=%.2f enabled=%d\n", 
        masterRoll, m_accelerationLoadingConstant, m_speedLoadingConstant, m_motorLoadingEnabled);

    // determine the vehicle drive axle
    if( masterRoll != All)
    {
        // Check to see if our speed is within the defined torque loading speed band parameters
        if( (m_speeds[ masterRoll] > m_minSpeedForLoading) && 
            ((m_speeds[ masterRoll] < m_maxSpeedForLoading) || (m_maxSpeedForLoading <= 0)) )
        {
            inTrqSpeedBand = true;
        }

        // check the speed to determine if motor loading should be applied
        if( ((m_accelerationLoadingConstant != 0) || (m_speedLoadingConstant != 0)) && ( inTrqSpeedBand == true) &&
            ( m_motorLoadingEnabled == true))
        {
            // calculate the torque to apply based on delta acceration
            Log(LOG_DEV_DATA, "Calculating torque loading\n");
            trqAccel = m_accelerations[masterRoll] * m_accelerationLoadingConstant;
            // calculate the % torque necessary for windage simulation
            if(m_speedLoadingConstant > 0.0)
            {
                trqSpeed = ((m_speeds[masterRoll] * m_speeds[masterRoll]) * m_speedLoadingConstant) / 2500;
            }
            // sum the % torques for overall % torque
            trqTotal = trqAccel + trqSpeed;

            //TO GIVE
            Log(LOG_DEV_DATA, "Calculated perc torque load of %.2f\n", trqTotal);

            //CONVERT THE trqTotal to ft/lbs based on max motor torque            

            trqTotal = (trqTotal/100) * m_motors[masterRoll]->GetMaxTorque();
            Log(LOG_DEV_DATA, "Calculated torque load of %.2f\n", trqTotal);
        }
        else if( inTrqSpeedBand == false)
        {
            Log( LOG_DEV_DATA, "Out of torque loading speed range\n");
            trqTotal = 0;
        }
    }

    Log(LOG_FN_ENTRY, "MotorController::CalculateTorqueLoad - Exit(%d) trqTotal=%.2f\n", masterRoll, trqTotal);

    return( trqTotal);
}

//-------------------------------------------------------------------------------------------------
bool ABMotorController::ConditionsOkForBoost( std::string &reason)
{
    bool    enable = false;

    if(ReadSubscribeData(GetDataTag("PowerSenseLostToIORack")) == True)
    {
        reason = "Lost power sense from PLC";
    }
    else if(ReadSubscribeData(GetDataTag("VehiclePresent")) == False)
    {
        reason = "No vehicle present";
    }
    else if(ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == False)
    {
        reason = "Retaining devices not raised";
    }
    else if(ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == On)
    {
        reason = "Loss compensation switch is on";
    }
    else if(ReadSubscribeData(GetDataTag("Zerospeed")) == True)
    {
        reason = "Machine is at zero speed";
    }
    else if(ReadSubscribeData(GetDataTag("DrivesEnabled")) == Disabled)
    {
        reason = "Drives not enabled";
    }
    else if(ReadSubscribeData(GetDataTag("EnableAllRollDrives")) == Disabled)
    {
        reason = "Roll drives not enabled";
    }
    else if(((fabs(m_speeds[m_masterMotor]) < m_minimumSpeedForBoost) && !m_enableAutoDriveAxle) ||
            (m_enableAutoDriveAxle && ((fabs(m_speeds[m_masterMotor]) < m_minimumSpeedForBoost) && 
                                       (fabs(m_speeds[m_slaveMotor]) < m_minimumSpeedForBoost))))
    {
        reason = GetDataTag("DriveWheel") + " wheels not above minimum speed";
    }
    else if(m_speeds[m_masterMotor] < 0 && !m_reverseBoost)
    {
        reason = "Reverse boost disabled by parameter";
    }
    else
    {
        enable  = true;
    }

    return( enable);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::AllWheelBoost(void)
{
    INT32 status = EOK;
    INT32   wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_DEV_DATA, "ABMotorController::AllWheelBoost()\n");

    if( (status=m_motors.Lock()) == EOK)
    {
        m_motors[ LeftFront]->FollowMaster( m_motors[ LeftFront], m_speeds[LeftFront], m_motors.size());
        m_motors[ RightFront]->FollowMaster( m_motors[ LeftFront], m_speeds[LeftFront], m_motors.size());
        m_motors[ LeftRear]->FollowMaster( m_motors[ LeftRear], m_speeds[LeftRear], m_motors.size());
        m_motors[ RightRear]->FollowMaster( m_motors[ LeftRear], m_speeds[LeftRear], m_motors.size());
        if( wheelCount == 6)
        {
            m_motors[ LeftTandem]->FollowMaster( m_motors[ LeftTandem], m_speeds[LeftTandem], m_motors.size());
            m_motors[ RightTandem]->FollowMaster( m_motors[ LeftTandem], m_speeds[LeftTandem], m_motors.size());
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(status));
    }

    Log(LOG_DEV_DATA, "ABMotorController::AllWheelBoost() done\n");

    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::RearWheelBoost(void)
{
    INT32 status = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_FN_ENTRY, "ABMotorController::RearWheelBoost()\n");

    if( (status=m_motors.Lock()) == EOK)
    {
        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            if( (RightRear == ii) && (false == m_axleSpeedMatch))
            {
                if(m_motors[ii] != NULL)
                {
                    m_motors[ ii]->CommandDisable();
                }
            }
            else
            {
                if(m_motors[ii] != NULL)
                {
                    if(!MachineType().compare(MACHINE_TYPE_3700) )
                    {
                        m_motors[ ii]->FollowMaster( m_motors[ LeftFront], m_speeds[LeftRear], m_motors.size());
                    }
                    else
                    {
                        m_motors[ ii]->FollowMaster( m_motors[ LeftRear], m_speeds[LeftRear], m_motors.size());
                    }
                }
                else
                {
                    Log(LOG_DEV_DATA, "Motor %d is NULL, not commanding to follow master!");
                }
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(status));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::RearWheelBoost() done\n");
    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::FrontWheelBoost(void)
{
    INT32 status = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_FN_ENTRY, "ABMotorController::FrontWheelBoost()\n");

    if( (status=m_motors.Lock()) == EOK)
    {
        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            if( (RightFront == ii) && (false == m_axleSpeedMatch))
            {
                m_motors[ ii]->CommandDisable();
            }
            else
            {
                m_motors[ ii]->FollowMaster( m_motors[ LeftFront], m_speeds[LeftFront], m_motors.size());
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(status));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::FrontWheelBoost() done\n");
    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::Speed(void)
{
    INT32 status, overallStatus = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());
    float commandedSpeed = 0.0;

    Log(LOG_FN_ENTRY, "ABMotorController::Speed()\n");
    LogInfo();

    if( (overallStatus=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            commandedSpeed = GetCommandValue(MotorDescriptions[ii] + "SpeedValue");
            // If the rollers are above minimum speed or should bypass minimum speed check
            if((fabs(commandedSpeed) >= m_minimumSpeedForBoost) || (m_overrideMinParameter) )
            {
                if((status = Enable()) == EOK)
                {
                    // Drives are enabled
                    if(ReadSubscribeData(GetDataTag("DrivesEnabled")) == Enabled)
                    {
                        Log(LOG_DEV_DATA, "Commanding Speed %3.2f\n", commandedSpeed);
                        if(m_motors[ii] != NULL)
                        {
                            commandedSpeed = GetCommandValue(MotorDescriptions[ii] + "SpeedValue");

                            if((status = m_motors[ii]->CommandSpeed(commandedSpeed, GetMotorSpeedCommandDelay()*ii)) == EOK)
                            {
                                Log(LOG_DEV_DATA, "CommandSpeed(%s) success!\n", m_motors[ii]->GetName().c_str());
                            }
                            else
                            {
                                // ComamndSpeed failed
                                overallStatus = status;
                                Log(LOG_ERRORS, "CommandSpeed(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                            }
                        }
                        else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
                    }
                    else Log(LOG_ERRORS, "Drives not enabled\n");
                }
                else
                {
                    // Enable failed
                    overallStatus = status;
                    Log(LOG_ERRORS, "Error enabling drives because: %s\n", strerror(overallStatus));
                }
            }
            else
            {
                Log(LOG_FN_ENTRY, "Commanded Speed %3.2f is less than minimum speed for boost %3.2f\n", commandedSpeed, m_minimumSpeedForBoost);
                m_motors[ii]->CommandDisable(); // Disable the motor (torque 0)
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
    }
    Log(LOG_FN_ENTRY, "ABMotorController::Speed() done\n");

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::IndividualControlMode(void)
{
    INT32 status = ENOTSUP, overallStatus = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    std::string drivesEnabled( ReadSubscribeData( GetDataTag( "DrivesEnabled")));

    Log(LOG_FN_ENTRY, "ABMotorController::IndividualControlMode( %s)\n", drivesEnabled.c_str());

    overallStatus = status;

    if( (overallStatus=m_motors.Lock()) == EOK)
    {
        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            ABMotor *motor = m_motors[ ii];
            if( motor != NULL)
            {
                float command = 0.0;
                const std::string mode = GetMotorMode(ii);
                if(mode == SPEED_MODE)
                {
                    command = GetCommandValue(MotorDescriptions[ii] + "SpeedValue");

                    Log(LOG_DEV_DATA, "%s of %3.2f for %s\n", COMMAND_SPEED, command, MotorDescriptions[ii].c_str());

                    motor->CommandSpeed(command);
                    if( drivesEnabled == True)
                    {
                        motor->SetDrivesHwEnabled( true);
                        motor->CommandEnable();
                    }
                    else
                    {
                        motor->SetDrivesHwEnabled( false);
                        motor->CommandDisable();
                    }
                }
                else if(mode == TORQUE_MODE)
                {
                    command = GetCommandValue(MotorDescriptions[ii] + "TorqueValue");

                    Log(LOG_DEV_DATA, "%s of %3.2f for %s\n", COMMAND_TORQUE, command, MotorDescriptions[ii].c_str());
                    motor->CommandTorque(command);
                    if( drivesEnabled == True)
                    {
                        motor->SetDrivesHwEnabled( true);
                        motor->CommandEnable();
                    }
                    else
                    {
                        motor->SetDrivesHwEnabled( false);
                        motor->CommandDisable();
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "Invalid motor mode [%s] for individual control, disabling %s\n", GetMotorMode(ii).c_str(), MotorDescriptions[ii].c_str());
                    motor->SetDrivesHwEnabled( false);
                    motor->CommandDisable();
                }
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::IndividualControlMode() done\n");

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::Torque(void)
{
    INT32 status, overallStatus = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_FN_ENTRY, "ABMotorController::Torque()\n");
    LogInfo();

    if((ReadSubscribeData(GetDataTag("DrivesEnabled")) == Enabled) &&
       (ReadSubscribeData(GetDataTag("VehiclePresent")) == True))
    {
        if((status = Enable()) == EOK)
        {
            if( (overallStatus=m_motors.Lock()) == EOK)
            {
                float commandedTorque = 0.0;
                if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

                for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
                {
                    commandedTorque = GetCommandValue(MotorDescriptions[ii] + "TorqueValue");
                    Log(LOG_DETAILED_DATA, "Commanding Torque %3.2f to %s\n", commandedTorque, m_motors[ii]->GetName().c_str());

                    if(m_motors[ii] != NULL)
                    {
                        if((status = m_motors[ii]->CommandTorque(commandedTorque)) == EOK)
                        {
                            Log(LOG_DEV_DATA, "CommandTorque(%s) success!\n", m_motors[ii]->GetName().c_str());
                        }
                        else
                        {
                            // CommandTorque failed
                            overallStatus = status;
                            Log(LOG_ERRORS, "CommandTorque(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                        }
                    }
                    else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
                }

                m_motors.Unlock();
            }
            else
            {
                Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
            }
        }
        else
        {
            // Enable failed
            overallStatus = status;
            Log(LOG_ERRORS, "ABMotorController::Torque() Error enabling drives because: %s\n", strerror(status));
        }
    }
    else
    {
        Log(LOG_ERRORS, "Disabling drives %s = %s, %s = %s\n",
            GetDataTag("DrivesEnabled").c_str(), ReadSubscribeData(GetDataTag("DrivesEnabled")).c_str(),
            GetDataTag("VehiclePresent").c_str(), ReadSubscribeData(GetDataTag("VehiclePresent")).c_str());
        Disable();   // disable the drives (torque 0)
    }
    Log(LOG_FN_ENTRY, "ABMotorController::Torque() done\n");

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::LossCompensation(void)
{
    // because this is a serial drive system, bring the drives up one at a time
    INT32 status = EOK;
    Log(LOG_FN_ENTRY, "ABMotorController::LossCompensation()\n");

    if(ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")) == On)
    {
        if(ReadSubscribeData(std::string("VehiclePresent")) == False)
        {
            status = Speed();
            if(m_lossCompensationSpeedCheckCurrentCount++ == m_lossCompensationSpeedCheckCount)
            {
                // validate that all of the rollers are above verification speed if the count time has expired
                ValidateRollerSpeeds(m_lcValidationSpeed);
            }
        }
        else Log(LOG_ERRORS, "Vehicle present in LossCompensation\n");
    }
    else Log(LOG_ERRORS, "LossCompensation switch not on in LossCompensation\n");

    Log(LOG_FN_ENTRY, "ABMotorController::LossCompensation() done\n");

    return(status);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::QuickBrake(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::QuickBrake()\n");
    LogInfo();

    INT32 overallStatus = EOK;

    if( (overallStatus=m_motors.Lock()) == EOK)
    {
        if(IsQuickBrakeEnabled() == true)
        {
            const INT8 rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());
            for(INT8 ii = LeftFront; ii < rolls; ii++)
            {
                SetCommandedSpeed( ii, 0.0);
                m_motors[ ii]->CommandSpeed( 0.0);
            }
            m_overrideMinParameter = true;
            overallStatus = Speed();
        }
        else Log(LOG_DEV_DATA, "QuickBrake is disabled\n");

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::QuickBrake() done\n");

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::RearBrakeAssist(void)
{
    bool shouldCommandSpeed = false;
    INT32 status, overallStatus = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    Log(LOG_FN_ENTRY, "ABMotorController::RearBrakeAssist()\n");
    LogInfo();

    if( (overallStatus=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        //      for(UINT8 ii = LeftRear; (ii < m_motors.size()) && (shouldCommandSpeed == false); ii++)
        for(ii = LeftRear; (ii < wheelCount) && (shouldCommandSpeed == false); ii++)
        {
            if(m_speeds[ii] > 10.0) shouldCommandSpeed = true;
        }
        if(shouldCommandSpeed)
        {
            // set the new wheel speeds
            if(Enable() == EOK)
            {
                // fronts have the most braking force so use them as guide
                float commandSpeed = (m_speeds[LeftFront] + m_speeds[RightFront])/2.0 * GetBrakingFactor();
                Log(LOG_DETAILED_DATA, "ABMotorController::RearBrakeAssist() to %3.2f\n", commandSpeed);
                //              for(UINT8 ii = LeftRear; (ii < m_motors.size()) && (shouldCommandSpeed == false); ii++)
                for(ii = LeftRear; (ii < wheelCount) && (shouldCommandSpeed == false); ii++)
                {
                    if(m_motors[ii] != NULL)
                    {
                        if((status = m_motors[ii]->CommandSpeed(commandSpeed)) == EOK)
                        {
                            Log(LOG_DEV_DATA, "CommandSpeed(%s) success!\n", m_motors[ii]->GetName().c_str());
                        }
                        else
                        {
                            // CommandSpeed failed
                            overallStatus = status;
                            Log(LOG_ERRORS, "CommandSpeed(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                        }
                    }
                    else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
                }
            }
        }
        else Disable(); // disable the drives (torque 0)

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::RearBrakeAssist() done\n");

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::UploadDriveParameters(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::UpdateDriveParameters() - Nothing to do for AB");
    return(ENOSYS);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::SpecialMode(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::SpecialMode() - No special mode for AB");
    return(EOK);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::Enable(void)
{
    INT32 status = EOK, overallStatus = EOK;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());
    std::string response;

    Log(LOG_FN_ENTRY, "ABMotorController::Enable()\n");

    // If the hardware enable bit is set
    if( ReadSubscribeData(GetDataTag("DrivesEnabled")) == True)
    {
        Log(LOG_DEV_DATA, "Hardware ENABLE bit is set\n");

        if( (overallStatus=m_motors.Lock()) == EOK)
        {
            if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

            for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
            {
                if(m_motors[ii] != NULL)
                {
                    ((AB700sMotor*)m_motors[ ii])->SetDrivesHwEnabled( true);

                    if((status = m_motors[ii]->CommandEnable()) == EOK)
                    {
                        Log(LOG_DEV_DATA, "Enabled %s\n", m_motors[ii]->GetName().c_str());
                    }
                    else
                    {
                        overallStatus = status;
                        Log(LOG_ERRORS, "Failed to enable %s because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                    }
                }
                else
                {
                    Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
                    overallStatus = EINVAL;
                }
            }

            m_motors.Unlock();
        }
        else
        {
            Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
        }

        if(overallStatus == EOK)    Log(LOG_DEV_DATA, "Enabled The Drives\n");
        else                        Log(LOG_ERRORS, "Error Enabling The Drives\n");
    }
    else
    {
        Log(LOG_ERRORS, "Hardware ENABLE bit is not set...cannot enable the drives\n");
        overallStatus = EAGAIN;
    }

    Log(LOG_FN_ENTRY, "ABMotorController::Enable() done: %s\n", (overallStatus == EOK) ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE);

    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::Disable(void)
{
    INT32 status = EOK, overallStatus = EOK;
    UINT32 ii;
    // command all of the motors to disable if not currently disabled
    Log(LOG_FN_ENTRY, "ABMotorController::Disable()\n");

    if( (overallStatus=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<m_motors.size(); ii++)
        {
            if(m_motors[ii] != NULL)
            {
                ((AB700sMotor*)m_motors[ ii])->SetDrivesHwEnabled( false);
                if((status = m_motors[ii]->CommandDisable()) == EOK)
                {
                    Log(LOG_DEV_DATA, "Disabled %s\n", m_motors[ii]->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "Failed to disable %s because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                }
            }
            else
            {
                overallStatus = EINVAL;
                Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(overallStatus));
    }

    if(overallStatus == EOK)    Log(LOG_DEV_DATA, "Disabled The Drives\n");
    else                        Log(LOG_ERRORS, "Error Disabling The Drives\n");

    Log(LOG_FN_ENTRY, "ABMotorController::Disable() done\n");
    return(overallStatus);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetLossCompensationSpeedCommandDelay(const UINT32 delay)
{
    m_lossCompensationMotorDelay = delay;
}

//-------------------------------------------------------------------------------------------------
inline const UINT32 ABMotorController::GetLossCompensationSpeedCommandDelay(void)
{
    return(m_lossCompensationMotorDelay);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetMotorSpeedCommandDelay(const std::string &motorDelay)
{
    SetMotorSpeedCommandDelay(BposReadInt(motorDelay.c_str()));
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetMotorSpeedCommandDelay(const UINT32 motorDelay)
{
    m_interMotorSpeedDelay = motorDelay;
    Log(LOG_FN_ENTRY, "ABMotorController::SetMotorSpeedCommandDelay(%u)\n", m_interMotorSpeedDelay);
}

//-------------------------------------------------------------------------------------------------
inline const UINT32 ABMotorController::GetMotorSpeedCommandDelay(void)
{
    const UINT32 delay = (GetMotorMode() == LOSSCOMPENSATION_MODE) ? GetLossCompensationSpeedCommandDelay() : m_interMotorSpeedDelay;
    Log(LOG_FN_ENTRY, "ABMotorController::GetMotorSpeedCommandDelay(%u)\n", delay);
    return(delay);
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::ValidateRollerSpeeds(float speed)
{
    INT32 status;
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());
    std::string response, tag;

    // validate that the rollers are above the specified speed
    Log(LOG_FN_ENTRY, "ABMotorController::ValidateRollerSpeeds(%3.2f)\n", speed);

    if( (status=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            // if the roll did not accelerate to speed, flag the failure
            tag = GetDataTag(MotorDescriptions[ii] + "RollMotorOperationalFault");
            if((m_speeds[ii] < speed) && (ReadSubscribeData(tag) == False))
            {
                if(m_dataComm)
                {
                    if((status = m_dataComm->Write(tag, True, response, true)) == BEP_STATUS_SUCCESS)
                    {
                        Log(LOG_DEV_DATA, "Set %s\n", tag.c_str());
                    }
                    else Log(LOG_ERRORS, "Failed to set %s\n", tag.c_str());
                }
                else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
            }
            else if(ReadSubscribeData(tag) == True)
            {
                // else if a previous failure, clear the failure
                if(m_dataComm)
                {
                    if((status = m_dataComm->Write(tag, False, response, true)) == BEP_STATUS_SUCCESS)
                    {
                        Log(LOG_DEV_DATA, "Cleared %s\n", tag.c_str());
                    }
                    else Log(LOG_ERRORS, "Failed to clear %s\n", tag.c_str());
                }
                else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
            }
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(status));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::ValidateRollerSpeeds(%3.2f) done\n", speed);
}

//-------------------------------------------------------------------------------------------------
const INT32 ABMotorController::CheckStatus(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::CheckStatus()\n");

    std::string tag, response;
    INT32 status = MotorController::CheckStatus();
    INT32   ii, wheelCount = min( GetWheelCount(), (INT32)m_motors.size());

    if( (status=m_motors.Lock()) == EOK)
    {
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for( ii=LeftFront; ii<wheelCount/*m_motors.size()*/; ii++)
        {
            if( m_motors[ ii] == NULL)  continue;

            // Check roll motor drive faults
            tag = MotorDescriptions[ii] + "RollMotorOperationalFault";
            // If this drive is faulted
            if( m_motors[ ii]->IsFaulted())
            {
                // Read the fault from the drive
                status = ESTALE;
                Log(LOG_DEV_DATA, "%s Detected\n", tag.c_str());
                m_dataComm->Write(GetDataTag(tag), True, response, true);
            }
            // If this drive is not faulted, but was previously
            else if(ReadSubscribeData(GetDataTag(tag)) == True)
            {
                // Clear the operational fault
                m_dataComm->Write(GetDataTag(tag), False, response, true);
            }
            else Log(LOG_DEV_DATA, "No %s Detected\n", tag.c_str());
        }

        m_motors.Unlock();
    }
    else
    {
        Log(LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror(status));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::CheckStatus() done\n");

    return(status);
}

//-------------------------------------------------------------------------------------------------
void ABMotorController::LogInfo(void)
{
    Log(LOG_CLASSCOMPONENT_DATA,
        "Speeds (lf, lr) %3.2f, %3.2f\n"
        "min boost speed: %3.2f\n"
        "DriveAxle: %s\n"
        "Zerospeed: %s\n"
        "DrivesEnabled: %s\n"
        "RollsUpElevatorDown: %s\n"
        "LossCompensationSwitchStatus: %s\n"
        "VehiclePresent: %s\n",
        m_speeds[LeftFront], m_speeds[LeftRear],
        m_minimumSpeedForBoost,
        ReadSubscribeData(GetDataTag("DriveWheel")).c_str(),
        ReadSubscribeData(GetDataTag("Zerospeed")).c_str(),
        ReadSubscribeData(GetDataTag("DrivesEnabled")).c_str(),
        ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str(),
        ReadSubscribeData(GetDataTag("LossCompensationSwitchStatus")).c_str(),
        ReadSubscribeData(GetDataTag("VehiclePresent")).c_str());
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetBrakingFactor(const float factor)
{
    // Set to default value first
    m_brakingFactor = 0.8;

    if(factor < 1.0)
    {
        // If less than upper limit
        if(factor > 0.0)
        {
            // If greater than lower limit
            m_brakingFactor = factor;   // Set value passed in
        }
        else Log(LOG_ERRORS, "Tried to set braking factor to %f.  Must be greater than 0.0.  Setting to %f\n", factor, m_brakingFactor);
    }
    else Log(LOG_ERRORS, "Tried to set braking factor to %f.  Must be lesss than 1.0.  Setting to %f\n", factor, m_brakingFactor);
}

//-------------------------------------------------------------------------------------------------
inline const float ABMotorController::GetBrakingFactor(void) const
{
    return(m_brakingFactor);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::EnableQuickBrake(const bool enable)
{
    m_enableQuickBrake = enable;
}

//-------------------------------------------------------------------------------------------------
inline const bool ABMotorController::IsQuickBrakeEnabled(void) const
{
    return(m_enableQuickBrake);
}

//-------------------------------------------------------------------------------------------------
inline void ABMotorController::SetTorqueModeSpeedLimit(const float limit)
{
    const float upper = 75.0;
    uint32_t    ii;
    Log(LOG_FN_ENTRY, "ABMotorController::SetTorqueModeSpeedLimit(%3.2f)\n", limit);

    if(limit > upper)
    {
        // Speed too high
        m_torqueModeSpeedLimit = upper;
        Log(LOG_ERRORS, "Tried to set torque mode speed limit to %3.2f, will set to %3.2f", limit, m_torqueModeSpeedLimit);
    }
    else m_torqueModeSpeedLimit = limit;    // Speed just right

    if( (errno=m_motors.Lock()) == EOK)
    {
        for( ii=LeftFront; ii<m_motors.size(); ii++)
        {
            if(m_motors[ii] != NULL) m_motors[ii]->MaxTorqueSpeed(&m_torqueModeSpeedLimit);
        }
        m_motors.Unlock();
    }
    else
    {
        Log( LOG_ERRORS, "\tERROR locking motor list: %s\n", strerror( errno));
    }

    Log(LOG_FN_ENTRY, "ABMotorController::SetTorqueModeSpeedLimit(%3.2f) done\n", m_torqueModeSpeedLimit);
}

//-------------------------------------------------------------------------------------------------
inline const float ABMotorController::GetTorqueModeSpeedLimit(void)
{
    Log(LOG_FN_ENTRY, "ABMotorController::GetTorqueModeSpeedLimit(%3.2f)\n", m_torqueModeSpeedLimit);
    return(m_torqueModeSpeedLimit);
}

//-------------------------------------------------------------------------------------------------
inline const bool& ABMotorController::ClearFaultsOnZeroSpeed(const bool *clearFault /*= NULL*/)
{
    if(clearFault != NULL)  m_clearFaultsOnZeroSpeed = *clearFault;
    return m_clearFaultsOnZeroSpeed;
}

//-------------------------------------------------------------------------------------------------
inline string ABMotorController::MachineType(void)
{
    return GetDataTag("MachineType");
}

