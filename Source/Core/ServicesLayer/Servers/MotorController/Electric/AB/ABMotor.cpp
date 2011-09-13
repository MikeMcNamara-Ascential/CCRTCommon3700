//******************************************************************************
// Description:
//    Allen Bradley Electric Motor Object
//
//==============================================================================
//
//     Copyright © 2008- Burke E. Porter Machinery Co.
//     All Rights Reserved
//
//     This file contains confidential information of Burke E. Porter Machinery
//     and is not to be used in any way detrimental to the interest thereof.
//     Unauthorized use, distribution, copying, or transmittal of this file in
//     any way is strictly prohibited.
//
// LOG:
//     $Log: $
// 
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#include "ABMotor.h"

//-----------------------------------------------------------------------------
ABMotor::ABMotor(int speedRefIdx /*=0*/) : Motor(),
    m_driveIndex(speedRefIdx),
    m_maxTorque( 180.0),
    m_baseSpeed( 1780.0), m_maxPosTorque( 1.0), m_maxNegTorque( -1.0), m_motorHP( 30.0),
    m_ftLbsPerHp(3.0), m_torqueSpeedLimit(10.0), m_hwDriveEnabled( false), 
    m_enableDriveNeeded( false)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
ABMotor::~ABMotor()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::Initialize(const XmlNode *config)
{
    INT32 status=EOK;
    // Start off initializing the base class
    if( (status=Motor::Initialize(config)) == EOK)
    {
        try
        {
            /**
             * m_maxPositiveTorqueLimit and m_maxNegativeTorqueLimit are read
             * by the base class. Convert percentages (0-100) to float (0-1.00)
             */
            m_maxPosTorque = ((float)m_maxPositiveTorqueLimit) / 100.0;
            m_maxNegTorque = ((float)m_maxNegativeTorqueLimit) / 100.0;

            LoadAdditionalConfigurationItems(config);
            /**
             * Calculate the max torque @ base speed:
             * HP * (ftlb/HP) * gearRatio * rollRadius[ft]
             **/
            float rollDiameter = atof(config->getChild("Setup/Parameters/RollerDiameter")->getValue().c_str());
            m_maxTorque = m_motorHP * m_ftLbsPerHp * 3.0 * ((rollDiameter/2.0) / 12.0);

            Log(LOG_DEV_DATA, "  Max +Trq = %+7.3f %%", m_maxPosTorque*100.0);
            Log(LOG_DEV_DATA, "  Max -Trq = %+7.3f %%", m_maxNegTorque*100.0);
            Log(LOG_DEV_DATA, "Base Speed = %+7.3f RPM", m_baseSpeed);
            Log(LOG_DEV_DATA, "  Motor HP = %+7.3f HP", m_motorHP);
            Log(LOG_DEV_DATA, "  FtLbs/HP = %+7.3f", m_ftLbsPerHp);
            Log(LOG_DEV_DATA, "Max Torque = %+7.3f FtLbs @ %.2f RPM", m_maxTorque, m_baseSpeed);
        }
        catch( XmlException &err)
        {
            Log( LOG_ERRORS, "Error reading motor parameters %s", err.GetReason());
            status = EIDRM;
        }
    }

    return( status);
}

//-----------------------------------------------------------------------------
void ABMotor::LoadAdditionalConfigurationItems(const XmlNode *config)
{   
    const XmlNode *parameters = config->getChild("Setup")->getChild("Parameters");

    try
    {   // Read the motor base speed parameter
        m_baseSpeed = atof(parameters->getChild("MotorBaseSpeed")->getValue().c_str());
    }
    catch( XmlException &err)
    {
        m_baseSpeed = 1780.0;
    }

    try
    {   // Read the motor horsepower rating
        m_motorHP = atof(parameters->getChild("MotorHorsePower")->getValue().c_str());
    }
    catch( XmlException &err)
    {
        m_motorHP = 30.0;
    }

    try
    {   // Read the motor HP/(ft*lb) @ base speed parameter
        m_ftLbsPerHp = atof(parameters->getChild("FootPoundsPerHP")->getValue().c_str());
    }
    catch( XmlException &err)
    {
        m_ftLbsPerHp = 3.0;
    }
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::CommandEnable(void)
{
    Log(LOG_FN_ENTRY, "ABMotor::CommandEnable()");
    return(EOK);
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::CommandDisable(void)
{
    Log(LOG_FN_ENTRY, "ABMotor::CommandDisable()");
    return(EOK);
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::CommandSpeed(const float mph, const UINT32 delay)
{
    INT32   status = EINVAL;
    Log(LOG_FN_ENTRY, "Enter ABMotor::CommandSpeed()");
    float rpm = MphToRpm( mph);
    status = CommandRPM(rpm, delay);
    Log(LOG_FN_ENTRY, "Exit ABMotor::CommandSpeed()");
    return(status);
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::CommandRPM(const float rpm, const UINT32 delay /*=0*/)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::CommandTorque(const float torque, const UINT32 delay /*=0*/)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
bool ABMotor::IsFaulted() const
{
    return false;
}

//-----------------------------------------------------------------------------
void ABMotor::ClearFault()
{   // Nothing special to do
    throw BepException("ABMotor::ClearFault not implemented!");
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::FollowMaster( const ABMotor *master, const float &masterRollSpeed, const INT32 motorCount /*= 4*/)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
void ABMotor::SetDrivesHwEnabled(bool hwEnableSet)
{
    Log(LOG_FN_ENTRY, "Enter ABMotor::SetDrivesHwEnabled(%d)", hwEnableSet);
    if(HwDriveEnabled() != hwEnableSet)
    {
        HwDriveEnabled(&hwEnableSet);
        bool enable = true;
        EnableDriveNeeded(&enable);
    }
    Log( LOG_FN_ENTRY, "Exit ABMotor::SetDrivesHwEnabled( %d)\n", hwEnableSet);
}

//-----------------------------------------------------------------------------
const float& ABMotor::MaxTorqueSpeed(const float *speed /*= NULL*/)
{
    if(speed != NULL)  m_torqueSpeedLimit = *speed;
    return m_torqueSpeedLimit;
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::UploadDriveParameters(const XmlNode *parameters)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::SendCommand(const string &message)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::SendVariableCommand(const string message, const string tag, const INT32 readResponseDelay)
{
    return ENOTSUP;
}

//-----------------------------------------------------------------------------
const INT32 ABMotor::Enable(const bool enable)
{
    return CommandEnable();
}

//-----------------------------------------------------------------------------
const char* ABMotor::GetTorqueCommand(void)
{
    return "";
}

//-----------------------------------------------------------------------------
const char* ABMotor::GetSpeedCommand(void)
{
    return "";
}

//-----------------------------------------------------------------------------
const UINT16& ABMotor::SpeedRefDriveIndex(const UINT16 *index /*= NULL*/)
{
    if(index != NULL)  m_driveIndex = *index;
    return m_driveIndex;
}

//-----------------------------------------------------------------------------
float ABMotor::GetBaseSpeed(void)
{
    return m_baseSpeed;
}

//-----------------------------------------------------------------------------
float ABMotor::GetTorqueModeSpeedLimit(void)
{
    return m_torqueSpeedLimit;
}

//-----------------------------------------------------------------------------
float ABMotor::GetMaxTorque(void)
{
    return m_maxTorque;
}

//-----------------------------------------------------------------------------
float ABMotor::GetMaxPosTorque(void)
{
    return m_maxPosTorque;
}

//-----------------------------------------------------------------------------
float ABMotor::GetMaxNegTorque(void)
{
    return m_maxNegTorque;
}

//-----------------------------------------------------------------------------
const bool& ABMotor::HwDriveEnabled(const bool *enabled /*= NULL*/)
{
    if(enabled != NULL)  m_hwDriveEnabled = *enabled;
    return m_hwDriveEnabled;
}

//-----------------------------------------------------------------------------
const bool& ABMotor::EnableDriveNeeded(const bool *enableNeeded /*= NULL*/)
{
    if(enableNeeded != NULL)  m_enableDriveNeeded = *enableNeeded;
    return m_enableDriveNeeded;
}
