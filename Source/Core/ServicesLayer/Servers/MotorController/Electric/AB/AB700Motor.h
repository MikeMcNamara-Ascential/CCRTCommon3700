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
#ifndef AB700Motor_h
#define AB700Motor_h
//==============================================================================
#include "ABMotor.h"
#include "BepSync.h"


/**
 * Motor interface object used to control an Allen Bradley PowerFlex 700s
 * Electric Motor Drive
 *
 * @author Brian Meinke
 */
class AB700Motor : public ABMotor
{
public:

    AB700Motor( int speedRefIdx=0);

    virtual ~AB700Motor();

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
     * Command the motor to the specified MPH.
     *
     * @param mph    The MPH to command the motor to.
     * @param delay  The amount of time to delay in ms prior to executing
     *               the command.
     * @return The status of the command.
     */
    virtual const INT32 CommandSpeed(const float mph, const UINT32 delay=0);
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
     * Clears a fault status from the drive
     */
    virtual void ClearFault();
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

protected:
    /**
     * Block until a command has been received and process the
     * command.  This will return when terminated.
     */
    virtual void* Execute(void);

private:
    /** Name to use when referencing this motor */
    string                  m_motorName;
};

//-----------------------------------------------------------------------------
#endif //AB700Motor_h
