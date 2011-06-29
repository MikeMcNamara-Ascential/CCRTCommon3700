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
#ifndef ABMotor_h
#define ABMotor_h
//-----------------------------------------------------------------------------
#include "Motor.h"
#include "BepSync.h"
//-----------------------------------------------------------------------------
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


class ABMotor : public Motor
{
	friend class AB700sMotorTest;

public:
    ABMotor(int speedRefIdx=0);
    virtual ~ABMotor();
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
     * Checks to see if the drive has a current fault condition
     */
    virtual bool IsFaulted() const;
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
    virtual const INT32 FollowMaster( const ABMotor *master, const float &masterRollSpeed);
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
     * Get/Set the maximum speed (MPH) that the drive will go when in torque mode to prevent runaway
     * 
     * @param speed  The maximum speed (MPH) that the drive will go when in torque mode to prevent runaway.
     * 
     * @return The maximum speed (MPH) that the drive will go when in torque mode to prevent runaway.
     */
    const float& MaxTorqueSpeed(const float *speed = NULL);
    /**
     * Upload the specified drive parameters to the motor.
     *
     * @param parameters The parameters to upload.
     * @return The status of the operation.
     */
    virtual const INT32 UploadDriveParameters(const XmlNode *parameters);
    /**
     * Get/Set the drive index that is being used for a speed reference.
     * 
     * @param index  Index of the drive being used for the speed reference.
     * 
     * @return Index of the drive being used for the speed reference.
     */
    const UINT16& SpeedRefDriveIndex(const UINT16 *index = NULL);

    /**
     * Get the maximum torque that can be applied.
     * 
     * @return Maximum allowed torque setting.
     */
    float GetMaxTorque(void);

protected:
    /**
     * Send a command to the drive
     *
     * @param message Command tag to send to the drive
     *
     * @return If operation succeeded
     */
    virtual const INT32 SendCommand(const string &message);
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
    virtual const INT32 SendVariableCommand(const string message, const string tag, const INT32 readResponseDelay);
    /**
     * Enable the motor.  This must be done prior to commanding the
     * motor.
     *
     * @param enable Whether to enable (true) or disable (false) the drive
     *
     * @return The status of the motor enabling.
     */
    virtual const INT32 Enable(const bool enable);
    /**
     * Get the string representing how a speed command should be built
     *
     * @return Speed string
     */
    virtual const char* GetTorqueCommand(void);
    /**
     * Get the string representing how a torque command should be built
     *
     * @return Torqu string
     */
    virtual const char* GetSpeedCommand(void);
    /**
     * Get the motor base speed.
     * 
     * @return Motor base speed.
     */
    float GetBaseSpeed(void);
    /**
     * Get the speed limit for the rollers when in Torque mode.
     * 
     * @return Speed limit of rollers when in torque mode.
     */
    float GetTorqueModeSpeedLimit(void);
    /**
     * Get maximum positive torque percentage to apply while accelerating.
     * 
     * @return Maximum positive torque percentage to apply while accelerating.
     */
    float GetMaxPosTorque(void);
    /**
     * Get maximum negative torque percentage to apply while decelerating.
     * 
     * @return Maximum negative torque percentage to apply while decelerating.
     */
    float GetMaxNegTorque(void);
    /**
     * Get/Set the flag to indicate if the AllRollDrivesEnabled bit is set.
     * 
     * @param enabled Flag indicating if the AllRollDrivesEnabled bit is set.
     * 
     * @return Flag indicating if the AllRollDrivesEnabled bit is set.
     */
    const bool& HwDriveEnabled(const bool *enabled = NULL);
    /**
     * Get/Set the flag to indicate the drives need to be enabled.
     * 
     * @param enableNeeded
     *               Flag to indicate the drives need to be enabled.
     * 
     * @return Flag to indicate the drives need to be enabled.
     */
    const bool& EnableDriveNeeded(const bool *enableNeeded = NULL);

private:
    /**
     * The speed reference index for this drive. Applicapble only for
     * left front and left rear drive objects. Used for speed matching,
     * each drive will be told to follow a particular speed reference
     * index: LF = 4, LR = 5.
     */
    uint16_t                m_driveIndex;
    /** Motor max torque rating (calculated) */
    float                   m_maxTorque;
    /** Motor base speed rating in RPM */
    float                   m_baseSpeed;
    /** Percentage of max torque to apply when accelerating */
    float                   m_maxPosTorque;
    /** Percentage of max torque to apply when decelerating */
    float                   m_maxNegTorque;
    /** Horsepower rating for the drive */
    float                   m_motorHP;
    /** Ft lbs of torque per motor Horsepower */
    float                   m_ftLbsPerHp;
    /** Max speed (MPH) in torque mode */
    float                   m_torqueSpeedLimit;
    /**
     * Flag used to indicate if the AllRollDrivesEnabled bit is set. The
     * drives will not enable unless this bit is set BEFORE we send the
     * actual enable command to the drives
     */
    bool                    m_hwDriveEnabled;
    /**
     * Flag used to indicate that we have need to send the enable command to
     * the drives but we need to wait for the AllRollDrivesEnabled bit to be
     * set.
     */
    bool                    m_enableDriveNeeded;
};

//-----------------------------------------------------------------------------
#endif  //ABMotor_h
