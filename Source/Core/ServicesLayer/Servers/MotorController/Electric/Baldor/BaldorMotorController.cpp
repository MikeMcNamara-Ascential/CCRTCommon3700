//*************************************************************************
// Description:
//   Class responsible for controlling Baldor motors on an electric roll/brake machine
//
//===========================================================================
// Copyright (c) 2001 Burke E. Porter Machinery
// All Rights Reserved
//
// This file contains confidential information of Burke E. Porter Machinery
// and is not to be used in any way detrimental to the interests thereof.
// Unauthorized use, distribution, copying, or transmittal of this file in
// any way is strictly prohibited.
//===========================================================================
//
// LOG:
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Baldor/BaldorMotorController.cpp $
// 
// 6     10/26/06 11:29a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 5     4/13/06 4:22a Cward
// cleanup
//
// 4     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:03p Rwiersem
// Removed the unimplemented ReloadConfiguration method.  Added the
// methods needed to reload the configuration items without restarting the
// process.
//
// 3     8/17/05 8:27p Cward
// Beautified module and cleaned up javadocs comments
//
// 2     7/20/05 9:20p Cward
// Fixed motor loading when axle speed matching is disabled
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
// 5     2/18/05 12:52p Bmeinke
// Removed the #if(LOG_FORCES) blocks
//
// 4     1/17/05 9:59a Bmeinke
// Removed references to Simulator Switch (not present on CCRT)
//
// 3     12/22/04 8:25p Bmeinke
// Added a new "DisableSpeedMatching" electric motor parameter to allow us
// to totally turn off all speed matching if needed (like with the
// U152/Explorer that is AWD, but not reported as AWD by eCATS)
//
// 2     12/14/04 8:58p Bmeinke
// Removed the #if(USE_BEP_MUTEX) blocks
//
// 1     9/30/04 2:44p Bmeinke
// Initial full core integration for Ford
//
// 4     8/30/04 3:52p Bmeinke
// Publish() now calls the base MotorController::Publish() method (was
// calling BepServer::Publish())
// Fixed the AxleMatch() method to not command the master roller to match
// a slave roller speed
//
// 3     8/17/04 7:24p Bmeinke
// Added new m_axleSpeedMatch parameter to enable/disable side-to-side
// speed matching on the drive axle to help with motor over current faults
// FollowMaster() will now disable the "same axle slave" if the new
// m_axleSpeedMatch parameter is set to false
//
// 2     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 2     7/08/04 10:49a Bmeinke
// Added upper/lower speed limits on motor loading
// Made motor loading a commandable option (via Command())
//
// 1     6/04/04 3:32p Bmeinke
// Adapted from stand alone ElectricMotorController.h
//
//*************************************************************************
// $NoKeywords: $
//=========================================================================

#include "BaldorMotorController.h"

BaldorMotorController::BaldorMotorController(void) : MotorController(),
    m_motorConfig(NULL), m_minBrakeAssistSpeed(5.0), m_BrakeAssistForce(200.0),
    m_maxPositiveTorqueLimit(100.0), m_maxNegativeTorqueLimit(-100.0),
    m_torqueAssistMinMPH(10.0), m_boost(false), m_brakingFactor(0.8),
    m_interMotorSpeedDelay(0), m_lossCompensationMotorDelay(2000),
    m_lossCompensationSpeedCheckDelay(2000), m_lossCompensationSpeedCheckCount(0),
    m_lossCompensationSpeedCheckCurrentCount(0), m_lcValidationSpeed(3.0),
    m_torqueModeSpeedLimit(10.0), m_itsAllGood(true), m_enableStatusChecking(false),
    m_enableMotorCommFault(false), m_overrideMinParameter(false), m_enableQuickBrake(false),
    m_axleSpeedMatch( true), m_quickBrakeFactor(0), m_driveFaultDetected( false)
{
}

BaldorMotorController::~BaldorMotorController(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::~BaldorMotorController()\n");
    Terminate();
    Log(LOG_FN_ENTRY, "BaldorMotorController::~BaldorMotorController() done\n");
}

void BaldorMotorController::Initialize(const XmlNode *document)
{
    if(document)
    {
        // initialize the base class
        MotorController::Initialize(document);

        // initialize the serial motor controller
        Log(LOG_FN_ENTRY, "BaldorMotorController::Initialize()\n");
        try
        {
            // initialize the parameters
            LoadAdditionalConfigurationItems(document);
        }
        catch(XmlException &Err)
        {
            Log(LOG_DEV_DATA, "BaldorMotorController::Initialize Error: %s\n", Err.what());
            throw;
        }
    }
    else throw BepException("Invalid Configuration File Provided (NULL pointer)");

    Log(LOG_FN_ENTRY, "BaldorMotorController::Initialize() done\n");
}

void BaldorMotorController::LoadAdditionalConfigurationItems(const XmlNode *document)
{
    const XmlNode *setup = document->getChildren().getNode("Setup");
    const XmlNode *parameters = setup->getChild("ElectricMotorParameters");

    m_minBrakeAssistSpeed =
        atof(parameters->getChild("MinBrakeAssistSpeed")->getValue().c_str());
    m_BrakeAssistForce =
        atof(parameters->getChild("BrakeAssistForce")->getValue().c_str());
    m_torqueAssistMinMPH =
        atof(parameters->getChild("TorqueAssistMinSpeed")->getValue().c_str());
    SetLossCompensationSpeedCommandDelay(
        atol(parameters->getChild("InterMotorLossCompensationDelay")->getValue().c_str()));
    m_lossCompensationSpeedCheckDelay =
        atol(parameters->getChild("SpeedCheckDelayTime")->getValue().c_str());
    m_lcValidationSpeed =
        atof(parameters->getChild("LCRollerValidationSpeed")->getValue().c_str());

            // Set up the count for checking the motors in a LC test
    m_lossCompensationSpeedCheckCount =
        ((GetLossCompensationSpeedCommandDelay()*4) +
     m_lossCompensationSpeedCheckDelay) / m_updateRate;

    // get the reference to the drive parameters
    m_driveParameters = setup->getChild("DriveParameters")->getChildren();

    // get the reference to the motor configuration
    m_motorConfig = setup->getChild("Motors");
    m_serialMessages = std::string(BposGetEnv("FTP_ROOT")) + parameters->getChild("ProtocolFile")->getValue();

    SetBrakingFactor(atof(parameters->getChild("BrakingFactor")->getValue().c_str()));
    EnableQuickBrake(BposReadInt(parameters->getChild("EnableQuickBrake")->getValue().c_str()));
    SetTorqueModeSpeedLimit(atof(parameters->getChild("TorqueModeSpeedLimit")->getValue().c_str()));

    try
    {
        m_enableStatusChecking =
            (parameters->getChild("EnableStatusChecking")->getValue() == Enabled);
    }
    catch(BepException& Err)
    {
        // if the tag is missing from the configuration file, use the default
        // set in the constructor
    }

    try
    {
        m_axleSpeedMatch =
            (atoi(parameters->getChild("AxleSpeedMatch")->getValue().c_str()) != 0);
    }
    catch( BepException &err)
    {
        m_axleSpeedMatch = true;
    }

    try
    {
        m_quickBrakeFactor = fabs(atof(parameters->getChild("QuickBrakeFactor")->getValue().c_str()) );
    }
    catch( BepException &err)
    {
        m_quickBrakeFactor = 0;
    }
}

const std::string BaldorMotorController::Register(void)
{
    INT32 stat;
    std::string status(BEP_FAILURE_RESPONSE), name;
    Log(LOG_FN_ENTRY, "BaldorMotorController::Register()\n");

    if((stat = m_startEndSync.TryAcquire()) == EOK)
    {
        Log(LOG_DEV_DATA, "Acquired start/end synchronizarion object\n");
        if((status = MotorController::Register()) == BEP_SUCCESS_RESPONSE)
        {
            // If not running on the proper machine, terminate
            try
            {
                for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
                {
                    // Terminate all motors
                    Log(LOG_DEV_DATA, "Deleting %s\n", m_motors[ii]->GetName().c_str());
                    if(m_motors[ii] != NULL) m_motors[ii]->Terminate();
                }
                m_motors.clear();
                XmlParser parser;
                XmlNode* motorConfig, *messages = const_cast<XmlNode*>(parser.ReturnXMLDocument(m_serialMessages)->getChild("Setup")->getChild("SerialMessages"))->Copy();

                for(UINT8 ii = LeftFront; ii < (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str()); ii++)
                {
                    // Create all new Motors
                    m_motors.push_back(new BaldorMotor);
                    // Initialize motor
                    name = MotorDescriptions[ii] + "Motor";
                    Log(LOG_DEV_DATA, "Initializing %s\n", name.c_str());
                    motorConfig = const_cast<XmlNode*>(m_motorConfig->getChild(MotorDescriptions[ii] + "Motor"));
                    motorConfig->addChild(messages);        // Add serial message protocol stuff
                    m_motors[ii]->Initialize(motorConfig);  // Initialize the motor
                }
                for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
                {
                    // Launch all new Motors in threads (call ProcessCommands() to do in foreground)
                    Log(LOG_DEV_DATA, "Launching %s\n", m_motors[ii]->GetName().c_str());
                    m_motors[ii]->Resume();
                }
                SetStatus(BEP_RUNNING_STATUS);

                if((stat = m_startEndSync.Release()) == EOK)
                {
                    Log(LOG_DEV_DATA, "Released synchronizarion object\n");
                }
                else Log(LOG_ERRORS, "Failed to release start/end synchronizarion object because: %s\n", strerror(stat));
            }
            catch(BepException& Err)
            {
                Log(LOG_ERRORS, "BaldorMotorController::Register() Failed because: %s\n", Err.what());
            }
        }
        else Log(LOG_ERRORS, "MotorController::Register() failed with return value of %s\n", status.c_str());
    }
    else Log(LOG_ERRORS, "Failed to acquire start/end synchronizarion object because: %s\n", strerror(stat));

    Log(LOG_FN_ENTRY, "BaldorMotorController::Register() done\n");

    return(status);
}

void BaldorMotorController::Reset(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::Reset()\n");
    MotorController::Reset();
    MotorController::Initialize(GetConfigFile());
    Log(LOG_FN_ENTRY, "BaldorMotorController::Reset() done\n");
}

void BaldorMotorController::Terminate(void)
{
    std::string response;
    const UINT32 retryCount = 5;
    UINT32 retries;

    Log(LOG_FN_ENTRY, "BaldorMotorController::Terminate()\n");

    if(m_dataComm)  m_dataComm->Write(GetDataTag("EnableAllRollDrives"), False, response, true);
    else            Log(LOG_ERRORS, "Named data broker object is uninitialized\n");

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
    {
        // Terminate all motors (memory will automatically be de-allocated)
        if(m_motors[ii])
        {
            retries = 0;
            while((m_motors[ii]->Terminate() != EOK) && retries++ < retryCount)
            {
                Log(LOG_ERRORS, "Waiting for %s to acknowledge Terminate()\n");
                BposSleep(50);
            }
        }
        else Log(LOG_DEV_DATA, "Motor %d is NULL\n", ii);

        m_motors[ii] = NULL;
    }
    m_motors.clear();

    MotorController::Terminate();

    Log(LOG_FN_ENTRY, "BaldorMotorController::Terminate() done\n");
}

const std::string BaldorMotorController::Command(const XmlNode *node)
{
    std::string status(BEP_INVALID_RESPONSE);
    const std::string name = node->getName(), value = node->getValue();
    const INT32 rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());

    // an LC just commanded, reset the speed check current count
    Log(LOG_FN_ENTRY, "BaldorMotorController::Command(%s)\n", node->ToString().c_str());

    if(name == MOTOR_MODE)
    {
        Log(LOG_DEV_DATA, "Commanded to %s Mode\n", value.c_str());
        if((value == BOOST_MODE) || (value == QUICK_BRAKE_MODE) || (value == REAR_BRAKE_ASSIST_MODE) ||
           (value == SPEED_MODE) || (value == TORQUE_MODE) || (value == LOSSCOMPENSATION_MODE) ||
           (value == UPLOAD_PARAMETERS_MODE) || (value == INDIVIDUAL_CONTROL_MODE))
        {
            // Set the motor mode
            SetMotorMode(value);
            SetData(name, value);

            // Clear command value
            for(UINT8 ii = LeftFront; ii < rolls; ii++)
            {
                SetCommandValue(GetDataTag(MotorDescriptions[ii] + "SpeedValue"), 0.0);
                SetCommandValue(GetDataTag(MotorDescriptions[ii] + "TorqueValue"), 0.0);
            }
            // It's all good
            status = BEP_SUCCESS_RESPONSE;
        }
        else Log(LOG_ERRORS, "Invalid Motor Mode Received: %s\n", value.c_str());
    }
    else if(COMMAND_SPEED == name)
    {
        Log(LOG_DEV_DATA, "Commanded to Speed %s\n", value.c_str());
        SetData(node);

        // Set command value
        for(UINT8 ii = LeftFront; ii < rolls; ii++)
        {
            SetCommandValue(GetDataTag(MotorDescriptions[ii] + "SpeedValue"), atof(value.c_str()));
        }
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(COMMAND_TORQUE == name)
    {
        Log(LOG_DEV_DATA, "Commanded to Torque %s\n", value.c_str());
        SetData(node);
        // Set command value
        for(UINT8 ii = LeftFront; ii < rolls; ii++)
        {
            SetCommandValue(GetDataTag(MotorDescriptions[ii] + "TorqueValue"), atof(value.c_str()));
        }
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(COMMAND_MOTOR_DELAY == name)
    {
        // Commanding motor delay
        Log(LOG_DEV_DATA, "Commanded to set motor delay to %s\n", node->getValue().c_str());
        SetMotorSpeedCommandDelay(node->getValue());
        status = BEP_SUCCESS_RESPONSE;
    }
    else if(MOTOR_STATUS_ENQUIRE == name)
    {
        // Commanding enquire
        Log(LOG_DEV_DATA, "Commanded motor status enquire %s\n", node->getValue().c_str());
        SetMotorMode(node->getName());
        SetSpecialCommand(node->getValue());
        status = BEP_SUCCESS_RESPONSE;
    }
    else
    {
        status = MotorController::Command( node);
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::Command(%s) [status = %s] done\n",
        node->ToString().c_str(), status.c_str());

    if((MOTOR_MODE == name) && (LOSSCOMPENSATION_MODE == value))
    {
        // Switching to Loss Compensation mode
        m_lossCompensationSpeedCheckCurrentCount = 0;
    }
    if((MOTOR_MODE == name) && (QUICK_BRAKE_MODE != value) && (GetMotorMode() == QUICK_BRAKE_MODE))
    {
        // Switching out of Quick Brake mode
        m_overrideMinParameter = false;
    }
    else if((GetMotorMode() == SPEED_MODE) && (MOTOR_MODE == name) &&
            (SPEED_MODE != value) && (INDIVIDUAL_CONTROL_MODE != value))
    {
        // Switching out of speed mode...clear motor speed command delay
        SetMotorSpeedCommandDelay(0);
    }

    if(status != BEP_SUCCESS_RESPONSE)
    {
        if( ("SpeedValue" != name) && ("TorqueValue" != name))
        {
            Log(LOG_ERRORS, "BaldorMotorController::Command(%s) unknown\n", node->ToString().c_str());
        }
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::Command(%s) done\n", node->ToString().c_str());

    return(status);
}

const std::string BaldorMotorController::Publish(const XmlNode *node)
{
    std::string status;
    const XmlNode Node(MOTOR_MODE, BOOST_MODE);
    Log(LOG_FN_ENTRY, "BaldorMotorController::Publish(%s)\n", node->ToString().c_str());

    // Rolls were down, now they are going up
    if(node->getName() == GetDataTag("RollsDownElevatorsUp") && (node->getValue() == False))
    {
        // Make sure no torque initially
        const XmlNode trqCmd(COMMAND_TORQUE, "0");
        Command( &trqCmd);

        // Make sure rolls don't take off right away
        const XmlNode speedCmd(COMMAND_SPEED, "0");
        Command( &speedCmd);

        ClearOperationalFaults();   // Clear operational faults
        m_itsAllGood = true;        // Reset status to recover from failure if possible
    }
    // Rolls just raised
    else if(node->getName() == GetDataTag("RollsUpElevatorDown") && (node->getValue() == True))
    {
        // Make sure no torque initially
        const XmlNode trqCmd(COMMAND_TORQUE, "0");
        Command( &trqCmd);

        // Make sure rolls don't take off right away
        const XmlNode speedCmd(COMMAND_SPEED, "0");
        Command( &speedCmd);

        // Clutch on initial rolls-up
        Clutch(Enabled);

        // Set boost mode
        Command(&Node);
    }
    // Power lost, motor task has faulted or Drives were disabled by PLC
    else if((node->getName() == GetDataTag("PowerSenseLostToIORack") && (node->getValue() == True)) ||
            (node->getName() == GetDataTag("MotorTaskHeartbeatFault") && (node->getValue() == True)) ||
            (node->getName() == GetDataTag("DrivesEnabled") && (node->getValue() == False)))
    {
        // disable and set to boost mode
        Disable();
        Command(&Node);
    }

    status = MotorController::Publish(node);

    Log(LOG_FN_ENTRY, "BaldorMotorController::Publish(%s) done\n", node->ToString().c_str());

    return( status);
}

const std::string BaldorMotorController::GetMotorMode(const INT8 roll)
{
    std::string testMode, motorMode;
    INT32 status, rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());
    Log(LOG_FN_ENTRY, "GetMotorMode(%d)\n", roll);

    if((status = m_modeAccess.Acquire()) == EOK)
    {
        // Acquired exclusive access to mode
        char tag[32];
        if(roll == All)
        {
            // Get mode for all rolls
            motorMode = ReadSubscribeData(GetDataTag(CreateMessage(tag, sizeof(tag), "%sMotorMode", MotorDescriptions[LeftFront].c_str())));
            for(UINT8 ii = LeftFront; (ii < rolls) && (motorMode != INDIVIDUAL_CONTROL_MODE); ii++)
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

const float BaldorMotorController::GetCommandValue(const std::string& command)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::GetCommandValue(%s)\n", command.c_str());

    float commandedValue = atof(ReadSubscribeData(command).c_str());
    Log(LOG_DEV_DATA, "Command value = %3.2f\n", commandedValue);

    Log(LOG_FN_ENTRY, "BaldorMotorController::GetCommandValue(%s, %3.2f) done\n", command.c_str(), commandedValue);
    return(commandedValue);
}

void BaldorMotorController::SetCommandValue(const std::string& command, const float commandValue)
{
    char data[16];
    Log(LOG_FN_ENTRY, "BaldorMotorController::SetCommandValue(%s, %3.2f)\n", command.c_str(), commandValue);

    SetCommandValue(command, CreateMessage(data, sizeof(data), "%3.2f", commandValue));

    Log(LOG_FN_ENTRY, "BaldorMotorController::SetCommandValue(%s, %3.2f) done\n", command.c_str(), commandValue);
}

void BaldorMotorController::SetCommandValue(const std::string& command, const std::string &commandValue)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::SetCommandValue(%s, %s)\n", command.c_str(), commandValue.c_str());

    SetData(command, commandValue);

    Log(LOG_FN_ENTRY, "BaldorMotorController::SetCommandValue(%s, %s) done\n", command.c_str(), commandValue.c_str());
}

void BaldorMotorController::SetMotorMode(const std::string &motorMode, const INT8 roll)
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
        Log(LOG_DEV_DATA, "Acquuired mode access synchronization object.  ii = %d, end = %d\n", ii, end);
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

void BaldorMotorController::ClearOperationalFaults(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::ClearOperationalFaults()\n");

    std::string tag, response;
    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
    {
        // Clear the fault
        INT32 status;
        tag = GetDataTag(MotorDescriptions[ii] + "RollMotorOperationalFault");

        if(ReadSubscribeData(tag) == True)
        {
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

    Log(LOG_FN_ENTRY, "BaldorMotorController::ClearOperationalFaults() done\n");
}

void BaldorMotorController::UpdateMachineStatus(void)
{
    UINT8   ii;

    Log(LOG_FN_ENTRY, "BaldorMotorController::UpdateMachineStatus(%s = %s, %s = %s, %s = %s, %s = %s)\n",
        GetDataTag("RollsUpElevatorDown").c_str(), ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str(),
        GetDataTag("EnableAllRollDrives").c_str(), ReadSubscribeData(GetDataTag("EnableAllRollDrives")).c_str(),
        GetDataTag("DrivesEnabled").c_str(), ReadSubscribeData(GetDataTag("DrivesEnabled")).c_str(),
        GetDataTag("MotorTaskHeartbeatFault").c_str(), ReadSubscribeData(GetDataTag("MotorTaskHeartbeatFault")).c_str());

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(ii = LeftFront; ii < m_motors.size(); ii++)
    {
        // Check each motor status to make sure it is still alive
        if(m_motors[ii]->IMightBeDead())
        {
            if(m_enableStatusChecking) m_itsAllGood = false;
            Log(LOG_ERRORS, "Motor %s is in a bad condition!\n", m_motors[ii]->GetName().c_str());
        }
    }

    try
    {
        // If retainers raised and the drives are not enabled, enable them
        if(((ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == True) &&
            (ReadSubscribeData(GetDataTag("DrivesEnabled")) == False)) && m_itsAllGood)
        {
            // Enable the drives
            std::string response;
            if(m_dataComm)
            {
                m_dataComm->Write(GetDataTag("EnableAllRollDrives"), Enabled, response, true);
                Log(LOG_DEV_DATA, "Enabled drives\n");
            }
            else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
        }   // If rolls are down and either drives are enabled or PLC says they are enabled or just big trouble
        else if(((ReadSubscribeData(GetDataTag("RollsUpElevatorDown")) == False) &&
                 ((ReadSubscribeData(GetDataTag("EnableAllRollDrives")) == True) ||
                  (ReadSubscribeData(GetDataTag("DrivesEnabled")) == True))) || !m_itsAllGood)
        {
            // if the retainers are lowered and the drives are enabled, disable them
            std::string response;
            if(m_dataComm)
            {
                m_dataComm->Write(GetDataTag("EnableAllRollDrives"), Disabled, response, true);
                Log(LOG_DEV_DATA, "Disabled drives\n");
            }
            else Log(LOG_ERRORS, "Named data broker object is uninitialized\n");
        }
        else Log(LOG_DEV_DATA, "Nothing to do %s = %s, %s = %s\n",
                 GetDataTag("EnableAllRollDrives").c_str(),
                 ReadSubscribeData(GetDataTag("EnableAllRollDrives")).c_str(),
                 GetDataTag("RollsUpElevatorDown").c_str(),
                 ReadSubscribeData(GetDataTag("RollsUpElevatorDown")).c_str());

        MotorController::UpdateMachineStatus(); // Update the internal machine status
        GetAccelerations();                     // Retrieve the acceleration info
    }
    catch(BepException &Err)
    {
        Log(LOG_ERRORS, "BaldorMotorController::UpdateMachineStatus() failed because: %s\n", Err.what());
    }
    Log(LOG_FN_ENTRY, "BaldorMotorController::UpdateMachineStatus() done\n");
}

const INT32 BaldorMotorController::Boost(void)
{
    INT32 status;
    bool enable = false;    // Boost enable flag (set to off by default)
    std::string reason;     // Reason boost was disabled
    std::string response;   // Response from the requested data
    std::string tag, value; // Tag and value for retrieving data

    Log(LOG_FN_ENTRY, "BaldorMotorController::Boost()\n");
    LogInfo();              // log the system information

    if( m_disableSpeedMatching == true)
    {
        reason = "SpeedMatching is disabled";
    }
    else if(ReadSubscribeData(GetDataTag("PowerSenseLostToIORack")) == True)
    {
        reason = "Lost power sense from PLC";
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
    else if((fabs(m_speeds[LeftFront]) < m_minimumSpeedForBoost) && (ReadSubscribeData(GetDataTag("DriveWheel")) == "Front"))
    {
        reason = GetDataTag("DriveWheel") + " wheels not above minimum speed";
    }
    else if((fabs(m_speeds[LeftRear]) < m_minimumSpeedForBoost) && (ReadSubscribeData(GetDataTag("DriveWheel")) == "Rear"))
    {
        reason = GetDataTag("DriveWheel") + " wheels not above minimum speed";
    }
    else enable = true;

    if(enable)
    {
        // else Enable boost to eliminate speed difference between axles
        Log(LOG_DEV_DATA, "Boost Enabled\n");
        if((status = Enable()) == EOK)
        {
            // determine the vehicle type and command the correct boost algorythm
            if(ReadSubscribeData(GetDataTag("FourWheelDriveStatus")) ==  On)
            {
                // If performing an axle ratio (4x4 test) or an AWD vehicle
                Log(LOG_DEV_DATA, "Performing all wheel boost because FourWheelDriveStatus(%s) == On(%s)\n",
                    ReadSubscribeData(GetDataTag("FourWheelDriveStatus")).c_str(), On.c_str());
                AllWheelBoost();
            }
            else if(ReadSubscribeData(GetDataTag("DriveWheel")) == ALL_WHEEL_DRIVE_TAG)
            {
                Log(LOG_DEV_DATA, "Performing all wheel boost because DriveWheel(%s) == %s\n",
                    ReadSubscribeData(GetDataTag("DriveWheel")).c_str(), ALL_WHEEL_DRIVE_TAG);
                AllWheelBoost();
            }
            else if(ReadSubscribeData(GetDataTag("DriveWheel")) == REAR_WHEEL_DRIVE_VALUE)
            {
                // If Rear wheel drive vehicle
                Log(LOG_DEV_DATA, "Performing Rear Wheel Boost\n");
                RearWheelBoost();
            }
            else
            {
                Log(LOG_DEV_DATA, "Performing front wheel boost\n");
                FrontWheelBoost();  // Must be front wheel drive
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
    Log(LOG_FN_ENTRY, "BaldorMotorController::Boost() done\n");

    return(status);
}

const INT32 BaldorMotorController::ApplyMotorLoad( float motorLoad, int masterRoll)
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
                motorLoad /= 2;

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

const INT32 BaldorMotorController::AxleMatch(const float masterSpeed, const float slaveSpeed, Motor* masterMotor, Motor* slaveMotor)
{
    INT32 status, overallStatus = EOK;
    Log(LOG_FN_ENTRY, "BaldorMotorController::AxleMatch(%s:%3.2f, %s:%3.2f)\n",
        masterMotor ? masterMotor->GetName().c_str() : "Master motor is NULL", masterSpeed,
        slaveMotor ? slaveMotor->GetName().c_str() : "Slave motor is NULL", slaveSpeed);

    if(masterMotor != NULL)
    {
        if(slaveMotor != NULL)
        {
            // if the speed difference between the master and slave is
            // greater than m_speedDeltaForBoost
            if(fabs(fabs(masterSpeed) - fabs(slaveSpeed)) > m_speedDeltaForBoost)
            {
                Log(LOG_DEV_DATA,"Speed difference detected, commanding slave to match master\n");

                // disable the master
                if((status = masterMotor->CommandDisable()) == EOK)
                {
                    Log(LOG_DEV_DATA, "CommandDisable(%s) success!\n", masterMotor->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "CommandDisable(%s) failed because: %s\n", masterMotor->GetName().c_str(), strerror(status));
                }

                // command the slave to speed match the master
                if((status = slaveMotor->CommandSpeed(masterSpeed)) == EOK)
                {
                    Log(LOG_DEV_DATA, "CommandSpeed(%s) success!\n", slaveMotor->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "CommandSpeed(%s) failed because: %s\n", slaveMotor->GetName().c_str(), strerror(status));
                }
            }
            else                   // no speed difference, so disable both motors
            {
                // disable the master
                if((status = masterMotor->CommandDisable()) == EOK)
                {
                    Log(LOG_DEV_DATA, "CommandDisable(%s) success!\n", masterMotor->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "CommandDisable(%s) failed because: %s\n", masterMotor->GetName().c_str(), strerror(status));
                }

                // disable the slave
                if((status = slaveMotor->CommandDisable()) == EOK)
                {
                    Log(LOG_DEV_DATA, "CommandDisable(%s) success!\n", slaveMotor->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "CommandDisable(%s) failed because: %s\n", slaveMotor->GetName().c_str(), strerror(status));
                }
            }
        }
        else Log(LOG_ERRORS, "Slave motor is NULL\n");
    }
    else Log(LOG_ERRORS, "Master motor is NULL\n");

    Log(LOG_FN_ENTRY, "BaldorMotorController::AxleMatch(%s:%3.2f, %s:%3.2f) done\n",
        masterMotor ? masterMotor->GetName().c_str() : "Master motor is NULL", masterSpeed,
        slaveMotor ? slaveMotor->GetName().c_str() : "Slave motor is NULL", slaveSpeed);

    return(overallStatus);
}

const INT32 BaldorMotorController::FollowMaster(const Rolls master, const Rolls sameAxleSlave)
{
    INT32 status, overallStatus = EOK;
    Log(LOG_FN_ENTRY, "BaldorMotorController::FollowMaster()\n");
    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    if(IsBraking(master))
    {
        // Disable all while braking (drives can't handle regen)
        for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
        {
            if(m_motors[ii] != NULL)
            {
                if((status = m_motors[ii]->CommandDisable()) == EOK)
                {
                    Log(LOG_DEV_DATA, "CommandDisable(%s) success!\n", m_motors[ii]->GetName().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_ERRORS, "CommandDisable(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                }
            }
            else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
        }
    }
    else
    {
        // not braking so speed follow the master
        for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
        {
            // Spped follow the master roll
            Log(LOG_DEV_DATA, "Commanding %s at %3.2f to %3.2f\n", MotorDescriptions[ii].c_str(), m_speeds[ii], m_speeds[master]);
            if(ii != master)
            {
                if(m_motors[ii] != NULL)
                {
                    if(ShouldBoost(m_speeds[master],m_speeds[ii]))
                    {
                    // Check to see if we should speed match the same axle slave
                    if( (ii == sameAxleSlave) && ( m_axleSpeedMatch == false) )
                    {
                        if((status = m_motors[ii]->CommandDisable()) == EOK)
                        {
                            Log(LOG_DEV_DATA, "CommandDisable(%s) success!\n", m_motors[ii]->GetName().c_str());
                        }
                        else
                        {
                            overallStatus = status;
                            Log(LOG_ERRORS, "CommandDisable(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                        }
                    }
                    else
                    {
                        if((status = m_motors[ii]->CommandSpeed(m_speeds[master])) == EOK)
                        {
                            Log(LOG_DEV_DATA, "CommandSpeed(%s) success!\n", m_motors[ii]->GetName().c_str());
                        }
                        else
                        {
                            overallStatus = status;
                            Log(LOG_ERRORS, "CommandSpeed(%s) failed because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                        }
                    }
                }
                    else
                    {
                        Log(LOG_DEV_DATA, "Not commanding speed ShouldBoost returned false\n");
                        status = EOK;
                    }
                }
                else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
            }
            else Log(LOG_DEV_DATA, "\tMotor %d is master\n", ii);
        }

        status = ApplyMotorLoad( CalculateTorqueLoad( master), master);
    }
    Log(LOG_FN_ENTRY, "BaldorMotorController::FollowMaster() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::AllWheelBoost(void)
{
    INT32 status = EIO;
    Log(LOG_DEV_DATA, "BaldorMotorController::AllWheelBoost()\n");
    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii += 2)
    {
        // check axle for a mismatch
        if(ii < m_motors.size())
        {
            if(m_motors[ii] != NULL)
            {
                status = AxleMatch(m_speeds[ii], m_speeds[ii + 1], m_motors[ii], m_motors[ii + 1]);
            }
            else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
        }
        else Log(LOG_ERRORS, "Motor size = %u, index (ii) = %u\n", m_motors.size(), ii);
    }
    Log(LOG_DEV_DATA, "BaldorMotorController::AllWheelBoost() done\n");

    return(status);
}

const INT32 BaldorMotorController::RearWheelBoost(void)
{
    INT32 status;
    Log(LOG_FN_ENTRY, "BaldorMotorController::RearWheelBoost()\n");
    status = FollowMaster(LeftRear, RightRear);
    Log(LOG_FN_ENTRY, "BaldorMotorController::RearWheelBoost() done\n");
    return(status);
}

const INT32 BaldorMotorController::FrontWheelBoost(void)
{
    INT32 status;
    Log(LOG_FN_ENTRY, "BaldorMotorController::FrontWheelBoost()\n");
    status = FollowMaster(LeftFront, RightFront);
    Log(LOG_FN_ENTRY, "BaldorMotorController::FrontWheelBoost() done\n");
    return(status);
}

const INT32 BaldorMotorController::Speed(void)
{
    INT32 status, overallStatus = EOK;
    float commandedSpeed = 0.0;
    bool shouldCommandSpeed = false;

    Log(LOG_FN_ENTRY, "BaldorMotorController::Speed()\n");
    LogInfo();

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; (ii < m_motors.size()) && (shouldCommandSpeed == false); ii++)
    {
        commandedSpeed = GetCommandValue(MotorDescriptions[ii] + "SpeedValue");
        if((fabs(m_speeds[ii] - commandedSpeed) > m_speedDeltaForBoost) ||
           ((fabs(commandedSpeed) > m_minimumSpeedForBoost)) || m_overrideMinParameter)
        {
            // If the rollers are above minimum speed or should bypass minimum speed check
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

    Log(LOG_FN_ENTRY, "BaldorMotorController::Speed() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::IndividualControlMode(void)
{
    INT32 status = ENOTSUP, overallStatus = EOK;
    Log(LOG_FN_ENTRY, "BaldorMotorController::IndividualControlMode()\n");

    overallStatus = status;

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
    {
        float command = 0.0;
        const std::string mode = GetMotorMode(ii);
        if(mode == SPEED_MODE)
        {
            command = GetCommandValue(MotorDescriptions[ii] + "SpeedValue");

            Log(LOG_DEV_DATA, "%s of %3.2f for %s\n", COMMAND_SPEED, mode.c_str(), MotorDescriptions[ii].c_str());
            m_motors[ii]->CommandSpeed(command);
        }
        else if(mode == TORQUE_MODE)
        {
            command = GetCommandValue(MotorDescriptions[ii] + "TorqueValue");

            CheckTorqueModeSpeedLimit(ii, command);

            Log(LOG_DEV_DATA, "%s of %3.2f for %s\n", COMMAND_TORQUE, mode.c_str(), MotorDescriptions[ii].c_str());
            m_motors[ii]->CommandTorque(command);
        }
        else
        {
            Log(LOG_ERRORS, "Invalid motor mode [%s] for individual control, disabling %s\n", GetMotorMode(ii).c_str(), MotorDescriptions[ii].c_str());
            m_motors[ii]->CommandDisable();
        }
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::IndividualControlMode() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::Torque(void)
{
    INT32 status, overallStatus = EOK;
    Log(LOG_FN_ENTRY, "BaldorMotorController::Torque()\n");
    LogInfo();

    if((ReadSubscribeData(GetDataTag("DrivesEnabled")) == Enabled) &&
       (ReadSubscribeData(GetDataTag("VehiclePresent")) == True))
    {
        if((status = Enable()) == EOK)
        {
            float commandedTorque = 0.0;
            if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));
            for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
            {
                commandedTorque = GetCommandValue(MotorDescriptions[ii] + "TorqueValue");
                Log(LOG_DETAILED_DATA, "Commanding Torque %3.2f to %s\n", commandedTorque, m_motors[ii]->GetName().c_str());

                if(m_motors[ii] != NULL)
                {
                    // Motor object is valid
                    CheckTorqueModeSpeedLimit(ii, commandedTorque);

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
        }
        else
        {
            // Enable failed
            overallStatus = status;
            Log(LOG_ERRORS, "BaldorMotorController::Torque() Error enabling drives because: %s\n", strerror(status));
        }
    }
    else
    {
        Log(LOG_ERRORS, "Disabling drives %s = %s, %s = %s\n",
            GetDataTag("DrivesEnabled").c_str(), ReadSubscribeData(GetDataTag("DrivesEnabled")).c_str(),
            GetDataTag("VehiclePresent").c_str(), ReadSubscribeData(GetDataTag("VehiclePresent")).c_str());
        Disable();   // disable the drives (torque 0)
    }
    Log(LOG_FN_ENTRY, "BaldorMotorController::Torque() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::LossCompensation(void)
{
    // because this is a serial drive system, bring the drives up one at a time
    INT32 status = EIO;
    Log(LOG_FN_ENTRY, "BaldorMotorController::LossCompensation()\n");

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

    Log(LOG_FN_ENTRY, "BaldorMotorController::LossCompensation() done\n");

    return(status);
}

const INT32 BaldorMotorController::QuickBrake(void)
{
    INT32   overallStatus = EOK;

    Log(LOG_FN_ENTRY, "BaldorMotorController::QuickBrake()\n");
    LogInfo();

    if(IsQuickBrakeEnabled() == true)
    {
        const INT8 rolls = BposReadInt(GetData(GetDataTag("RollCount")).c_str());

        // If we want to stop immediately
        if( m_quickBrakeFactor <= 0)
        {
        for(INT8 ii = LeftFront; ii < rolls; ii++)
        {
            SetCommandValue(GetDataTag(MotorDescriptions[ii] + "SpeedValue"), 0.0);
        }
        m_overrideMinParameter = true;
        overallStatus = Speed();
    }
        else
        {
            float   newCmd = 0;
            float   currSpeeds[rolls];

            if(GetSpeeds( currSpeeds, rolls) == BEP_STATUS_SUCCESS)
            {
                for(INT8 ii = LeftFront; ii < rolls; ii++)
                {
                    if( fabs(currSpeeds[ii]) > 1)
                    {
                        if( currSpeeds[ii] > 2) newCmd = m_quickBrakeFactor * currSpeeds[ii];
                        else                    newCmd = 0;
                        (void)m_motors[ii]->CommandTorque( newCmd);
                        SetCommandValue(GetDataTag(MotorDescriptions[ii] + "TorqueValue"), newCmd);
                        Log( "QuickBrake: Commanding %s to %.03f ft-lbs\n",
                             MotorDescriptions[ii].c_str(), newCmd);
                    }
                    else
                    {
                        (void)m_motors[ii]->CommandDisable();
                        SetCommandValue(GetDataTag(MotorDescriptions[ii] + "SpeedValue"), 0.0);
                        SetCommandValue(GetDataTag(MotorDescriptions[ii] + "TorqueValue"), 0.0);
                        Log( "QuickBrake: Disabling %s inside QuickBrake()\n",
                             MotorDescriptions[ii].c_str());
                    }
                }
            }
            else
            {
                overallStatus = errno;
                Log(LOG_ERRORS, "QuickBrake: Could Not read speeds: %s\n", strerror(errno));
            }
        }
    }
    else Log(LOG_DEV_DATA, "QuickBrake is disabled\n");

    Log(LOG_FN_ENTRY, "BaldorMotorController::QuickBrake() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::RearBrakeAssist(void)
{
    bool shouldCommandSpeed = false;
    INT32 status, overallStatus = EOK;
    Log(LOG_FN_ENTRY, "BaldorMotorController::RearBrakeAssist()\n");
    LogInfo();

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftRear; (ii < m_motors.size()) && (shouldCommandSpeed == false); ii++)
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
            Log(LOG_DETAILED_DATA, "BaldorMotorController::RearBrakeAssist() to %3.2f\n", commandSpeed);
            for(UINT8 ii = LeftRear; (ii < m_motors.size()) && (shouldCommandSpeed == false); ii++)
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

    Log(LOG_FN_ENTRY, "BaldorMotorController::RearBrakeAssist() done\n");

    return(overallStatus);
}

const INT32 BaldorMotorController::UploadDriveParameters(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::UpdateDriveParameters()\n");

    INT32 status, overallStatus = EOK;
    if(ReadSubscribeData(GetDataTag("VehiclePresent")) == False)
    {
        if(ReadSubscribeData(GetDataTag("Zerospeed")) == True)
        {
            Log(LOG_FN_ENTRY, "BaldorMotorController Updating Parameters\n");
            if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

            for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
            {
                if(m_motors[ii] != NULL)
                {
                    if((status = m_motors[ii]->UploadDriveParameters(m_driveParameters.getNode(ii))) == EOK)
                    {
                        Log(LOG_DEV_DATA, "Drive parameters uploaded to %s\n", m_motors[ii]->GetName().c_str());
                    }
                    else
                    {
                        // UploadDriveParameters failed
                        overallStatus = status;
                        Log(LOG_ERRORS, "Error, uploading drive parameters to %s because: %s\n", m_motors[ii]->GetName().c_str(), strerror(status));
                    }
                }
                else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
            }
        }
        else Log(LOG_DEV_DATA, "Will not load motor parameters because machine is not at zero speed\n");
    }
    else Log(LOG_DEV_DATA, "Will not load motor parameters because vehicle is present\n");

    const XmlNode Node(MOTOR_MODE, BOOST_MODE);
    Command(&Node); // Set back to boost mode

    Log(LOG_FN_ENTRY, "BaldorMotorController::UpdateDriveParameters() done changing to %s\n", GetMotorMode().c_str());

    return(overallStatus);
}

const INT32 BaldorMotorController::SpecialMode(void)
{
    INT32 status = EIO, overallStatus = EOK;
    static bool commandSent = false;

    Log(LOG_FN_ENTRY, "BaldorMotorController::SpecialMode()\n");

    if(GetMotorMode() == MOTOR_STATUS_ENQUIRE)
    {
        Log(LOG_DEV_DATA, "%s special command, command sent %d\n", GetSpecialCommand().c_str(), commandSent);
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        if(commandSent == false)
        {
            // Command not confirmed sent
            for(UINT8 ii = 0; ii < m_motors.size(); ii++)
            {
                if((status = m_motors[ii]->Enquire(GetSpecialCommand())) == EOK)
                {
                    Log(LOG_DEV_DATA, "%s enquire %s successful\n", m_motors[ii]->GetName().c_str(), GetSpecialCommand().c_str());
                }
                else
                {
                    overallStatus = status;
                    Log(LOG_DEV_DATA, "%s enquire %s failed because: %s\n", m_motors[ii]->GetName().c_str(), GetSpecialCommand().c_str(), strerror(status));
                }
            }

            if(status == EOK)
            {
                // All motors acknowledged commands
                commandSent = true;
                Log(LOG_DEV_DATA, "All motors acknowledged %s command\n", GetSpecialCommand().c_str());
            }
            else Log(LOG_ERRORS, "All motors did not acknowledge %s command\n", GetSpecialCommand().c_str());
        }
        else
        {
            // Command confirmed sent
            std::string response;
            for(UINT8 ii = 0; ii < m_motors.size(); ii++)
            {
                response = m_motors[ii]->GetEnquiryResponse();
                Log(LOG_DEV_DATA, "%s acknowledged command %s with:\n%s\n", m_motors[ii]->GetName().c_str(), GetSpecialCommand().c_str(), response.c_str());
            }
            commandSent = false;
        }
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::SpecialMode() done\n");
    return(overallStatus);
}

const INT32 BaldorMotorController::Enable(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::Enable()\n");

    INT32 status = EOK, overallStatus = EOK;
    // command all of the motors to enable if not currently enabled
    if(ReadSubscribeData(GetDataTag("DrivesEnabled")) != True)
    {
        Log(LOG_DEV_DATA, "Drives not enabled\n");
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
        {
            if(m_motors[ii] != NULL)
            {
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
                overallStatus = EINVAL;
                Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
            }
        }

        if(overallStatus == EOK)    Log(LOG_DEV_DATA, "Enabled The Drives\n");
        else                        Log(LOG_ERRORS, "Error Enabling The Drives\n");
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::Enable() done: %s\n", (overallStatus == EOK) ? BEP_SUCCESS_RESPONSE : BEP_FAIL_RESPONSE);

    return(overallStatus);
}

const INT32 BaldorMotorController::Disable(void)
{
    INT32 status = EOK, overallStatus = EOK;
    // command all of the motors to disable if not currently disabled
    Log(LOG_FN_ENTRY, "BaldorMotorController::Disable()\n");

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
    {
        if(m_motors[ii] != NULL)
        {
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

    if(overallStatus == EOK)    Log(LOG_DEV_DATA, "Disabled The Drives\n");
    else                        Log(LOG_ERRORS, "Error Disabling The Drives\n");

    Log(LOG_FN_ENTRY, "BaldorMotorController::Disable() done\n");
    return(overallStatus);
}

void BaldorMotorController::SetLossCompensationSpeedCommandDelay(const UINT32 delay)
{
    m_lossCompensationMotorDelay = delay;
}

const UINT32 BaldorMotorController::GetLossCompensationSpeedCommandDelay(void)
{
    return(m_lossCompensationMotorDelay);
}

void BaldorMotorController::SetMotorSpeedCommandDelay(const std::string &motorDelay)
{
    SetMotorSpeedCommandDelay(BposReadInt(motorDelay.c_str()));
}

void BaldorMotorController::SetMotorSpeedCommandDelay(const UINT32 motorDelay)
{
    m_interMotorSpeedDelay = motorDelay;
    Log(LOG_FN_ENTRY, "BaldorMotorController::SetMotorSpeedCommandDelay(%u)\n", m_interMotorSpeedDelay);
}

const UINT32 BaldorMotorController::GetMotorSpeedCommandDelay(void)
{
    const UINT32 delay = (GetMotorMode() == LOSSCOMPENSATION_MODE) ? GetLossCompensationSpeedCommandDelay() : m_interMotorSpeedDelay;
    Log(LOG_FN_ENTRY, "BaldorMotorController::GetMotorSpeedCommandDelay(%u)\n", delay);
    return(delay);
}

void BaldorMotorController::ValidateRollerSpeeds(float speed)
{
    // validate that the rollers are above the specified speed
    Log(LOG_FN_ENTRY, "BaldorMotorController::ValidateRollerSpeeds(%3.2f)\n", speed);

    INT32 status;
    std::string response, tag;

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(UINT8 ii = LeftFront; ii < m_motors.size(); ii++)
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
    Log(LOG_FN_ENTRY, "BaldorMotorController::ValidateRollerSpeeds(%3.2f) done\n", speed);
}

const INT32 BaldorMotorController::CheckStatus(void)
{
    std::string tag;
    INT32 status = MotorController::CheckStatus();
    UINT8       ii;
    bool        faultDetected = false;

    Log(LOG_FN_ENTRY, "BaldorMotorController::CheckStatus()\n");

    if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

    for(ii = 0; ii < m_motors.size(); ii++)
    {
        // Check roll motor drive faults
        tag = MotorDescriptions[ii] + "RollMotorDriveFault";
        if(ReadSubscribeData(GetDataTag(tag)) == True)
        {
            // Say we have a problem
            faultDetected = true;

            // Read the fault from the drive
            status = EIO;
            INT32 motorStat;
            Log(LOG_DEV_DATA, "%s Detected\n", tag.c_str());
            if(m_motors[ii] != NULL)
            {
                if((motorStat = m_motors[ii]->Enquire("CheckFault")) == EOK)
                {
                    Log(LOG_DEV_DATA, "Checked fault for %s\n", m_motors[ii]->GetName().c_str());
                }
                else Log(LOG_ERRORS, "Failed to Check fault for %s because: %s\n",
                         m_motors[ii]->GetName().c_str(), strerror(motorStat));
            }
            else Log(LOG_ERRORS, "\tMotor %d is NULL\n", ii);
        }
        else Log(LOG_DEV_DATA, "No %s Detected\n", tag.c_str());
    }

    for(ii = 0; ii < m_motors.size(); ii++)
    {
        // Check roll motor drive faults
        tag = MotorDescriptions[ii] + "RollMotorOperationalFault";
        if(ReadSubscribeData(GetDataTag(tag)) == True)
        {
            // Say we have a problem
            faultDetected = true;
            // Read the fault from the drive
            status = EIO;
            Log(LOG_DEV_DATA, "%s Detected\n", tag.c_str());
        }
        else Log(LOG_DEV_DATA, "No %s Detected\n", tag.c_str());
    }

    // If we have a motor fault
    if( true == faultDetected)
    {
        if( false == m_driveFaultDetected)
        {
            Log( LOG_ERRORS, "Drive fault detected...disabling drives\n");
            Disable();
        }
    }
    else
    {
        if( true == m_driveFaultDetected)
        {
            Log( LOG_ERRORS, "Drive fault consition has been cleared\n");
        }
    }

    m_driveFaultDetected = faultDetected;

    // Check for drive runaway in quick brake mode
    if( QUICK_BRAKE_MODE == GetMotorMode())
    {
        for( ii=LeftFront; ii<m_speeds.size(); ii++)
        {
            if( m_speeds[ ii] < -5)
            {
                Log( LOG_ERRORS, "QUICK_BRAKE runaway detected on drive %d (%s)..disabling\n",
                     ii, MotorDescriptions[ii].c_str());
                if( NULL != m_motors[ ii])
                {
                    m_motors[ ii]->CommandDisable();
                }
            }
        }
    }

    Log(LOG_FN_ENTRY, "BaldorMotorController::CheckStatus() done\n");

    return(status);
}

void BaldorMotorController::LogInfo(void)
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

const bool BaldorMotorController::IsBraking(const Rolls master)
{
    static bool braking = false;
    Log(LOG_FN_ENTRY, "BaldorMotorController::IsBraking(%s)\n", MotorDescriptions[master].c_str());

    if(m_accelerations[master] > m_brakeClearLimit)
    {
        // If master roll is accellerating, not braking
        braking = false;
    }
    else
    {
        // Check to see if any rolls are braking prevents front rolls from matching with weak rear brakes in RWD mode
        if(m_motors.size() != (UINT32)BposReadInt(GetData(GetDataTag("RollCount")).c_str())) Log(LOG_ERRORS, "Motor size = %u, roll count = %u\n", m_motors.size(), BposReadInt(GetData(GetDataTag("RollCount")).c_str()));

        for(UINT8 ii = LeftFront; (ii < m_motors.size()) && (braking == false); ii++)
        {
            if(m_accelerations[ii] < m_brakeDetectLimit) braking = true;
        }
    }
    Log(LOG_FN_ENTRY, "BaldorMotorController::IsBraking(%s) -> %s\n", MotorDescriptions[master].c_str(), braking ? "On" : "Off");

    return(braking);
}

void BaldorMotorController::SetBrakingFactor(const float factor)
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

const float BaldorMotorController::GetBrakingFactor(void) const
{
    return(m_brakingFactor);
}

void BaldorMotorController::EnableQuickBrake(const bool enable)
{
    m_enableQuickBrake = enable;
}

const bool BaldorMotorController::IsQuickBrakeEnabled(void) const
{
    return(m_enableQuickBrake);
}

void BaldorMotorController::SetTorqueModeSpeedLimit(const float limit)
{
    const float upper = 90.0;
    Log(LOG_FN_ENTRY, "BaldorMotorController::SetTorqueModeSpeedLimit(%3.2f)\n", limit);

    if(limit > upper)
    {
        // Speed too high
        m_torqueModeSpeedLimit = upper;
        Log(LOG_ERRORS, "Tried to set torque mode speed limit to %3.2f, will set to %3.2f", limit, m_torqueModeSpeedLimit);
    }
    else m_torqueModeSpeedLimit = limit;    // Speed just right

    Log(LOG_FN_ENTRY, "BaldorMotorController::SetTorqueModeSpeedLimit(%3.2f) done\n", m_torqueModeSpeedLimit);
}

const float BaldorMotorController::GetTorqueModeSpeedLimit(void)
{
    Log(LOG_FN_ENTRY, "BaldorMotorController::GetTorqueModeSpeedLimit(%3.2f)\n", m_torqueModeSpeedLimit);
    return(m_torqueModeSpeedLimit);
}

const bool BaldorMotorController::CheckTorqueModeSpeedLimit(const UINT32 roll, float& commandedTorque)
{
    bool changed = false;
    if(roll < m_motors.size())
    {
        // Roll index is valid
        if(fabs(m_speeds[roll]) > GetTorqueModeSpeedLimit())
        {
            // Speed is too high, disable it
            changed = true;
            commandedTorque = 0.0;
            SetCommandValue(MotorDescriptions[roll] + "TorqueValue", commandedTorque);

            Log(LOG_ERRORS, "Speed limit of %3.2f reached for %s, commanding torque of %3.2f\n",
                GetTorqueModeSpeedLimit(), m_motors[roll]->GetName().c_str(), commandedTorque);
        }
    }
    else Log(LOG_ERRORS, "Roll value [%d] greater than numberof motors [%d]\n", roll, m_motors.size());

    return(changed);
}

float BaldorMotorController::CalculateTorqueLoad( int masterRoll)
{
    float       trqTotal = 0.0;
    float       trqAccel = 0.0;
    float       trqSpeed = 0.0;
    bool        inTrqSpeedBand = false;

    Log(LOG_FN_ENTRY, "MotorController::CalculateTorqueLoad - Enter (%d); accelK=%.2F spdK=%.2f enabled=%d\n", masterRoll, m_accelerationLoadingConstant, m_speedLoadingConstant, m_motorLoadingEnabled);

    // determine the vehicle drive axle
    if( masterRoll != All)
    {
        // Check to see if our speed is within the defined torque loading speed band parameters
        if( (m_speeds[ masterRoll] > m_minSpeedForLoading) && ((m_speeds[ masterRoll] < m_maxSpeedForLoading) || (m_maxSpeedForLoading <= 0)) )
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
            // calculate the torque necessary for windage simulation
            if(m_speedLoadingConstant > 0.0)
            {
                trqSpeed = (m_speeds[masterRoll] * m_speeds[masterRoll]) * m_speedLoadingConstant / 1000.0;
            }
            // sum the torques for overall torque
            trqTotal = trqAccel + trqSpeed;
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

