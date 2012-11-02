//******************************************************************************
// Description:
//    Allen Bradley PowerFlex 700 Electric Motor Object
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
#include "AB700Motor.h"
#include "MotorControllerDefs.h"

//-----------------------------------------------------------------------------
AB700Motor::AB700Motor(int speedRefIdx /*=0*/) : ABMotor(speedRefIdx)
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
AB700Motor::~AB700Motor()
{   // Nothing special to do here
}

//-----------------------------------------------------------------------------
void AB700Motor::LoadAdditionalConfigurationItems(const XmlNode *config)
{   // start off by calling the base class
    ABMotor::LoadAdditionalConfigurationItems(config);
    const XmlNode *parameters = config->getChild("Setup/Parameters");
    try
    {   // Get the motor name
        m_motorName = parameters->getChild("MotorNameReference")->getValue();
    }
    catch(XmlException &excpt)
    {
        m_motorName = "UnknownMotor";
    }
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::CommandEnable(void)
{
    Log(LOG_FN_ENTRY, "Enter AB700Motor::CommandEnable()");
    INT32 status = WriteData(ENABLE_ALL_ROLL_DRIVES, "1");
    Log(LOG_FN_ENTRY, "Exit AB700Motor::CommandEnable()");
    return status;
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::CommandDisable(void)
{
    Log(LOG_FN_ENTRY, "Enter AB700Motor::CommandDisable()");
    // Set speed and torque reference back to 0
    INT32 status = WriteData(m_motorName + PlcSpeedRefTag, "0");
    status = WriteData(m_motorName + PlcTorqueRefTag, "0");
    status = WriteData(m_motorName + PlcMotorModeTag, PlcMotorModeFreeRoll);
    status = WriteData(ENABLE_ALL_ROLL_DRIVES, "0");
    Log(LOG_FN_ENTRY, "Exit AB700Motor::CommandDisable()");
    return(status);
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::CommandSpeed(const float mph, const UINT32 delay/*=0*/)
{   // Set the motor to speed mode
    Log(LOG_FN_ENTRY, "AB700Motor::CommandSpeed(mph: %.2f, %d)", mph, delay);
    INT32 status = WriteData(m_motorName + PlcMotorModeTag, PlcMotorModeSpeed);
    if(BEP_STATUS_SUCCESS == status)
    {   // Call the base class to continue the speed command processing
        status = ABMotor::CommandSpeed(mph, delay);
    }
    Log(LOG_FN_ENTRY, "AB700Motor::CommandSpeed(mph: %.2f, %d) - complete: %d", mph, delay, status);
    return status;
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::CommandRPM(const float rpm, const UINT32 delay)
{
    Log(LOG_FN_ENTRY, "Enter AB700Motor::CommandRPM( %+.2f)", rpm);
    float relRpm = rpm / GetBaseSpeed();  // Convert to a percentage of base speed
    char buff[16];
    Log(LOG_DEV_DATA, "Relative RPM: %.2f", relRpm);
    INT32 txRpm = 0.0;

    //do this to take care of problem when sending negative rpm to PLC
    txRpm = (relRpm < 0) ? ((INT32)(relRpm + 0.5))<<16 : (INT32)(relRpm + 0.5);
    Log(LOG_DEV_DATA, "Writing value to PLC: %d, %x", txRpm, txRpm);


    string rpmVal(CreateMessage(buff, sizeof(buff), "%d", txRpm));
    INT32 status = WriteData(m_motorName + PlcSpeedRefTag, rpmVal);
    Log(LOG_DEV_DATA, "Commanded RPM - tag: %s, rpm: %s, status: %d",
        (m_motorName + PlcSpeedRefTag).c_str(), rpmVal.c_str(), status);
    Log( LOG_FN_ENTRY, "Exit AB700Motor::CommandRPM( %+.2f)\n", rpm);
    return(status);
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::CommandTorque(const float torque, const UINT32 delay)
{
    Log(LOG_FN_ENTRY, "Enter AB700Motor::CommandTorque( %+.4f) up to %d MPH", torque, (int)MaxTorqueSpeed());
    float rpmLimit = MphToRpm(MaxTorqueSpeed()) / GetBaseSpeed();
    float relTrq = -torque / GetMaxTorque();  // Convert torque to percentage of max torque
    Log(LOG_DEV_DATA, "Commanding torque - relTrq: %.2f, MaxTorque: %.2f", relTrq, GetMaxTorque());
    if( ((relTrq < 0) && (rpmLimit > 0)) ||
        ((relTrq > 0) && (rpmLimit < 0)))
    {
        rpmLimit *= -1;
    }
    char buff[16];
    int intRelTrq = (int)((relTrq*1000.0)+0.5);
    string relTrqVal = CreateMessage(buff, sizeof(buff), "%d", intRelTrq);
    // Set the torque reference and limits
    INT32 status = WriteData(m_motorName + PlcMotorModeTag, PlcMotorModeTorque);
    WriteData(m_motorName + PlcTorqueRefTag, relTrqVal);
    WriteData(m_motorName + PlcSpeedRefTag, CreateMessage(buff, sizeof(buff), "%d", (int)(rpmLimit+0.5)));
    WriteData(m_motorName + PlcPosTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (int)GetMaxPosTorque()));
    WriteData(m_motorName + PlcNegTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (int)GetMaxNegTorque()));
    Log(LOG_FN_ENTRY, "Exit AB700Motor::CommandTorque( %+.4f)", torque);
    return(status);
}

//-----------------------------------------------------------------------------
void AB700Motor::ClearFault()
{
    // Set the "clear fault" bit 
    WriteData("ClearMotorDriveFaults", "1");
    BposSleep(100);
    WriteData("ClearMotorDriveFaults", "0");
}

//-----------------------------------------------------------------------------
const INT32 AB700Motor::FollowMaster(const ABMotor *master, const float &masterRollSpeed, const INT32 motorCount /*= 4*/)
{
    INT32   status = EINVAL;
    ABMotor *masterMotor = const_cast<ABMotor *>(master);
    Log( LOG_DEV_DATA, "Enter AB700Motor::FollowMaster(%d)", masterMotor->SpeedRefDriveIndex());
    if((master != this) || (motorCount == 2))
    {   // Set my speed to the master's speed
        status = CommandSpeed(masterRollSpeed, 0);
    }
    Log(LOG_DEV_DATA, "Exit AB700Motor::FollowMaster( %d)", masterMotor->SpeedRefDriveIndex());
    return(status);
}

//-----------------------------------------------------------------------------
void* AB700Motor::Execute(void)
{
    char buff[8];
    // Make sure we don't get lowered in priority
    BposSetThreadPriority( 0, 0, m_threadPriority);
    WriteData(m_motorName + PlcMotorModeTag, PlcMotorModeFreeRoll);
    WriteData(m_motorName + PlcSpeedRefTag, "0");
    WriteData(m_motorName + PlcTorqueRefTag, "0");
    WriteData(m_motorName + PlcNegTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (GetMaxNegTorque()*10)+0.5));
    WriteData(m_motorName + PlcPosTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (GetMaxPosTorque()*10)+0.5));
    // Loop while not told to stop and dont need to reconnect
    while(GetStatus() != BEP_TERMINATE_STATUS)
    {   // Make sure we don't get lowered in priority
        BposSetThreadPriority( 0, 0, m_threadPriority);
        BposSleep(1000);
    }
    // Clear the speed/torque commands when we terminate
    WriteData(m_motorName + PlcMotorModeTag, PlcMotorModeFreeRoll);
    WriteData(m_motorName + PlcSpeedRefTag, "0");
    WriteData(m_motorName + PlcTorqueRefTag, "0");
    WriteData(m_motorName + PlcNegTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (GetMaxNegTorque()*10)+0.5));
    WriteData(m_motorName + PlcPosTorqueLimitTag, CreateMessage(buff, sizeof(buff), "%d", (GetMaxPosTorque()*10)+0.5));

    return( NULL);
}
