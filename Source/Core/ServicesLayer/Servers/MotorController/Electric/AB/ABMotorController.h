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
//     $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/AB/ABMotorController.h $
// 
// 5     10/30/07 4:15p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     10/26/06 11:25a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 3     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:00p Rwiersem
// Made the test class a friend.  Removed the ReloadConfiguration method
// that was unimplented.  Added the methods needed to reload the
// configuration items without restarting the process.
//
// 2     7/20/05 9:20p Cward
// Axle speed matching support
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
// 2     7/23/04 3:14p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 3     7/21/04 3:29p Bmeinke
// Moved the motor loading parameters to the MotorController class
// Moved the motor loading calculations to the
// MotorController::CalculateTorqueLoad() method
// Implement the new virtual ApplyMotorLoad() method for motor loading
//
// 2     6/28/04 8:34a Bmeinke
// Enabled motor loading and fixed up some syntax errors in the torque
// loading block
//
// 1     6/11/04 4:12p Bmeinke
// Added for Hermosillo
//
// 1     2/24/04 11:01a Bmeinke
// HMMA Alabama with AB motor controller and countdown timer component
//
// 7     1/20/04 10:14a Bmeinke
// Moved implementation code to cpp file
//
// 6     1/19/04 12:57p Bmeinke
// Removed m_serialMessages member (not used)
//
// 5     1/16/04 3:57p Bmeinke
// Removed the code which was attempting to read a SerialMessages node
// inside Initialize() because we do not have any serial messages
// ClearOperationalFaults() was not clearing the system motor operational
// faults (in FaultServer)
// Cleaned up the section of UpdateMachineStatus() that would set/clear
// the EnableAllRollDrives status based on machien conditions
// Read AllRollDrivesEnabled only once (into a local string) inside
// IndividualControlMode() for efficiency.
// Removed the contents of SpecialMode() because we have no 'special mode'
//
// 4     1/13/04 4:54p Bmeinke
// Need to set the max torque mode speed for all motor objects when we
// create them
// Motor objects were not being enabled in IndividualControlMode()
// Removed CheckTorqueModeSpeedLimit(): this is handled internally by the
// drives with the ABS_MIN_SPD_TRQ_SELECT torque mode
// Re-added explicitly commanding the motor objects speed to 0 in
// QuickBrake()
// SetTorqueModeSpeedLimit() sets the max torque mode speed for each motor
// object
//
// 3     1/05/04 10:45p Bmeinke
// Converted MotorVector to a class that inherits from
// std::vector<AB700sMotor*> and adds a mutex for thread safety
// The motor threads will no longer delete themselves when they are done.
// This was done to avoid race conditions that could cause SIGSEGV's when
// terminating
// Re-sequenced the Terminate() method to eliminate SIGSEGV's
// Re-organized the Command() method so it works properly (and fixes
// QuickBrake)
// SetCommandedSpeed() no longer commands speeds to the motor objects
// SetCommandedTorque() no longer commands torques to the motor objects
//
// 2     12/29/03 8:37p Bmeinke
// "Security" checkin (compiles, but not tested)
//
// 1     12/23/03 8:13p Bmeinke
// Initial checkin
//
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef _ABMOTORCONTROLLER_H_INCLUDED_
#define _ABMOTORCONTROLLER_H_INCLUDED_
//==============================================================================

// Don't change the order of these 2 or your eyes will fall out
#include "MotorController.h"
#include "Motor.h"
#include "ABMotor.h"
#include "AB700Motor.h"
#include "AB700sMotor.h"

#include <math.h>
#include <vector>

/**
 * Data type definition of a vector of motor pointers
 */
typedef std::vector<ABMotor*>   MotorVector_t;

/**
 * Mutex protected list class used to manage a list of pointers to
 * AB700sMotor objects
 *
 * @author Brian Meinke
 * @version 1
 * @see AB700sMotor
 * @since January 5, 2004
 */
class MotorVector : public MotorVector_t
{
public:
    /**
     * Default constructor
     */
    MotorVector() : MotorVector_t()
    {
    }

    /**
     * Destructor
     */
    ~MotorVector()
    {
    }

    /**
     * Lock the list for exclusive access
     *
     * @return EOK if successfully locked
     */
    inline int Lock() const
    {
        return( m_lock.Acquire());
    }

    /**
     * Unlock the list after a successfull call to Lock()
     *
     * @return EOK if successfully unlocked
     */
    inline int Unlock() const
    {
        return( m_lock.Release());
    }

    /**
     * Get the maximum torque speed limit.
     * 
     * @return Maximum torque mode speed limit.
     */
    inline float MaxTorqueSpeedLimit(void)
    {
        return m_maxTorqueSpeed;
    }

    /**
     * Store the max torque mode speed limit.
     * 
     * @param speedLimit Maximum torque mode speed limit.
     */
    inline void StoreMaxTorqueSpeedLimit(const float &speedLimit)
    {
        m_maxTorqueSpeed = speedLimit;
    }

private:
    /** Maximum speed limit for torque mode to prevent runaway */
    float m_maxTorqueSpeed;
    /**
     * Mutex used to lock/guard access to the list
     */
    mutable BepMutex    m_lock;

};

/**
 * Motor controller used to control a system of Allen Bradley
 * PowerFlex 700s drives
 *
 * @author Brian Meinke
 * @version 1
 * @see MotorController
 * @since January 18, 2003
 */
class ABMotorController : public MotorController
{
    friend class ABMotorControllerTest;

public:
    /**
     * Default constructor
     */
    ABMotorController();

    /**
     * Destructor
     */
    virtual ~ABMotorController();

    /**
     * Method for initialization.
     *
     * @param document Reference to a parsed configuration document.
     * @since Version 1.0
     */
    void Initialize(const XmlNode *document);

    /**
     * Method that is called to command the component to perform any
     * system registration functions necessary.
     *
     * @return Status of the registration process.
     *         BEP_FAILURE_RESPONSE == Could not successfully perform registration
     *         BEP_SUCCESS_RESPONSE == Registered
     * @since Version 1.0
     */
    const std::string Register(void);

    /**
     * Method used to set all of the objects in the component
     * to their original states.  This would includes all
     * member variables such as buffers, lists, and stored
     * data.
     * @since Version 1.0
     */
    virtual void Reset(void);

    /**
     * Method used to terminate the component.  This performs
     * any cleanup that the destructor would not and sets
     * the state of the component to terminate.
     * @since Version 1.0
     */
    virtual void Terminate(void);

    /**
     * Request to Perform an action
     *
     * @param node    Action to be performed
     * @return Status of operation
     *         Success, Fail, Invalid, Unavailable
     * @since Version 2.0
     */
    virtual const std::string Command(const XmlNode *node);

    /**
     * Handle the subscribed for data by the process.
     *
     * @param node    Tag and data that was subscribed for
     * @return Status of the updating of the published tag
     *         Success, Fail
     * @since Version 2.1
     */
    virtual const std::string Publish(const XmlNode *node);

    /**
     * Method to get the motor mode.
     *
     * @param roll   Roll to set mode for
     *
     * @return The mode the motors are in.
     * @since Version 1.0
     */
    inline const std::string GetMotorMode(const INT8 roll = All);

    /**
     * Method to get value of the command for a motor
     *
     * @param command Comamnd to retrieve
     *
     * @return The command value
     * @since Version 1.0
     */
    inline const float GetCommandValue(const std::string& command);

    /**
     * Method to set the commanded speed.
     *
     * @param command Comamnd to set
     * @param commandValue
     *               The new speed to command the system to.
     * @since Version 2.5
     */
    inline void SetCommandValue(const std::string& command, const float commandValue);

    /**
     * Method to set the commanded speed.
     *
     * @param driveIdx   Index of the drive to set the speed command for (LeftFront, RightFront,
     *                   LeftRear, RightRear, LeftTandem, RightTandem)
     * @param speedValue Speed to command the drive to (MPH)
     * @since Version 2.5
     */
    inline void SetCommandedSpeed(int driveIdx, const float speedValue);

    /**
     * Method to set the commanded torque.
     *
     * @param driveIdx Index of the drive to set the speed command for (LeftFront, RightFront,
     *                 LeftRear, RightRear, LeftTandem, RightTandem)
     * @param torqueValue
     *                 Torque to command the drive to (NM)
     * @since Version 2.5
     */
    inline void SetCommandedTorque(int driveIdx, const float torqueValue);

protected:
    /**
     * Load the additional configuration items needed by the AB motor
     * controller.
     *
     * @param document A pointer to the parsed configuration file
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * Method to set the commanded speed.
     *
     * @param command Comamnd to set
     * @param commandValue
     *               The new speed to command the system to.
     * @since Version 1.0
     */
    inline void SetCommandValue(const std::string& command, const std::string &commandValue);

    /**
     * Method to set the motor mode.
     *
     * @param motorMode The mode to run the motor task in.
     * @param roll      Roll to set mode for
     * @since Version 1.0
     */
    inline void SetMotorMode(const std::string &motorMode, const INT8 roll = All);

    /**
     * Clears operational faults
     * @since Version 2.1
     */
    void ClearOperationalFaults(void);

    /**
     * Method that will read and update the local machine
     * information for the motor controller.
     * @since Version 1.0
     */
    virtual void UpdateMachineStatus(void);

    /**
     * This controls wheel speed as needed in various tests
     * For AWD, if a side to side speed dif is detected,
     * the slower wheel will be, "boosted" to match the faster
     * wheel. In RWD or FWD modes the left side is in the torque
     * mode, while the right side is speed matching.
     * The non-driven axle is always in the speed match mode.
     * When driving in reverse, the non-driven axle may be boosted.
     * Assumption is made that reverse driving stays below 15 mph
     * or so.
     *
     * Note: a reverse boost parameter has been added to provide the
     * ability to boost during a test.
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 Boost(void);

    /**
     * This function applies a load to the drive axle of the machine
     *
     * @param motorLoad The amount of torquw to apply to the drive axle
     * @param masterRoll The drive wheel of the vehicle
     * @return Staus of operation
     * @since Version 1.1
     */
    virtual const INT32 ApplyMotorLoad( float motorLoad, int masterRoll);

    virtual float CalculateTorqueLoad( int masterRoll);

    /**
     * Checks to see if machine conditions are OK for boost mode
     *
     * @param reason If conditions are not OK for boost mode, this string will be
     *               filled in with the reason why
     *
     * @return
     */
    bool ConditionsOkForBoost( std::string &reason);

    /**
     * This contains the logic for boosting all wheels.  This is used
     * when the start 4wd test to PLC bit is set.
     * @return Staus of operation
     * @since Version 1.0
     */
    const INT32 AllWheelBoost(void);

    /**
     * This contains the logic for boosting a rear wheel drive
     * vehicle.
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 RearWheelBoost(void);

    /**
     * This contains the logic for boosting a front wheel drive
     * vehicle.
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 FrontWheelBoost(void);

    /**
     * This function applies a speed to all wheels. It should
     * be used in all custom testing like Loss Compensation, Debug, ...
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 Speed(void);

    /**
     * Command motor controller to enter mode where all
     * rolls can be commanded independantly
     * @return Staus of operation
     * @since Version 2.5
     */
    virtual const INT32 IndividualControlMode(void);

    /**
     * This function applies a torque to all wheels. Since this is
     * a hydraulic motor controller, if 0 torque is commanded, it
     * will disable boost and declutch the drives.
     *
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 Torque(void);

    /**
     * This function applies a speed all wheels. It will handle
     * any startup sequencing that will need to be dealt with
     * to achieve the commanded speed.
     *
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 LossCompensation(void);

    /**
     * This function will stop all rolls when called.  Its primary
     * use is stoping the rear rolls at the end of a brake test.
     * The function brake() in main will set motomode to QUICK_BRAKE
     * when it done sampling data.
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 QuickBrake(void);

    /**
     * Assist the rear brakes of the vehicle in stopping.  This
     * is used for small cars that do not have any rear brake
     * force.  (Ex. In the Neon with Teves ABS, if the rear wheels
     * are rotating at 20KPH more than the front a fault is
     * recorded).
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 RearBrakeAssist(void);

    /**
     * This function will update all of the roll drive parameters.
     * This will be done by reading the paramters from the configuraion
     * file and sending them along with their value to each of the drives.
     * @return Staus of operation
     * @since Version 1.0
     */
    virtual const INT32 UploadDriveParameters(void);

    /**
     * This is a mode that can be overridden if a special
     * mode is needed that is not defined in the base class.
     * @return Staus of operation
     * @since Version 2.0
     */
    virtual const INT32 SpecialMode(void);

    /**
     * Enable all of the roll drives.
     *
     * @return The status of the operation.
     * @since Version 1.0
     */
    const INT32 Enable(void);

    /**
     * Disable all of the roll drives.
     *
     * @return The status of the operation.
     * @since Version 1.0
     */
    const INT32 Disable(void);

    /**
     * Update drive axle based on
     * wheel speeds.
     * @since Version 1.0
     */
    virtual void DetectDriveAxle(void);

    /**
     * Sets the delay between motor commands during
     * loss compensation to ease current demand
     *
     * @param delay  Delay value in milliseconds
     * @since Version 1.5
     */
    inline void SetLossCompensationSpeedCommandDelay(const UINT32 delay);

    /**
     * Gets the delay between motor commands during
     * loss compensation to ease current demand
     */
    inline const UINT32 GetLossCompensationSpeedCommandDelay(void);

    /**
     * Set the delay between motor speed commands
     *
     * @param motorDelay The new delay to use
     * @since Version 1.5
     */
    inline void SetMotorSpeedCommandDelay(const std::string &motorDelay);

    /**
     * Set the delay between motor speed commands
     *
     * @param motorDelay The new delay to use
     * @since Version 1.5
     */
    inline void SetMotorSpeedCommandDelay(const UINT32 motorDelay);

    /**
     * Get the delay between motor speed commands.
     *
     * @return Delay value in milliseconds
     * @since Version 1.5
     */
    inline const UINT32 GetMotorSpeedCommandDelay(void);

    /**
     * The parameters to use for the Baldor drives.
     * @since Version 1.0
     */
    XmlNodeMap m_driveParameters;
    /**
     * The configuration information to use for the motors.
     * @since Version 1.0
     */
    const XmlNode* m_motorConfig;
    /**
     * Validate that all of the roller speeds are above the
     * specified speed.  For each one that is not, a fault
     * will be sent to the PLC.
     *
     * @param speed  The speed to verify that the rollers are faster than.
     * @since Version 1.0
     */
    void ValidateRollerSpeeds(float speed);

    /**
     * Check the status of the system.
     *
     * @return The status of the system.
     * @since Version 2.1
     */
    virtual const INT32 CheckStatus(void);

    /**
     * Log ABMotorController information.
     * @since Version 1.0
     */
    void LogInfo(void);

    /**
     * Set the amount that the roll speed should be decreased by in successive 
     * commands.  Factor must be less than 1 but positive (e.g. 0.8)
     *
     * @param factor Braking factor (percent less than 1)
     */
    inline void SetBrakingFactor(const float factor);

    /**
     * Returns the amount that the roll speed should be decreased by in 
     * successive commands.  Factor must be less than 1 but positive (e.g. 0.8).
     *
     * @return Braking factor (percent less than 1)
     */
    inline const float GetBrakingFactor(void) const;

    /**
     * Enables or disables quick brake based on parameter
     *
     * @param enable Sets the state of quick brake enabling (true = enabled, else disabled)
     * @since Version 2.4
     */
    inline void EnableQuickBrake(const bool enable = true);

    /**
     * Returns the status of Quick Brake
     *
     * @return Enable status of Quick Brake (true = enabled, else disabled)
     * @since 2.4
     */
    inline const bool IsQuickBrakeEnabled(void) const;

    /**
     * Sets the speed limit to disable applying torque
     * when in torque mode to prevent runaway.
     *
     * @param limit  Speed limit to set
     * @since Version 2.5
     */
    inline void SetTorqueModeSpeedLimit(const float limit);

    /**
     * Gets the speed limit to disable applying torque
     * when in torque mode to prevent runaway.
     *
     * @return  Speed limit
     * @since Version 2.5
     */
    inline const float GetTorqueModeSpeedLimit(void);

    /**
     * Get/Set the flag indicating faults should be cleared when the machine reaches zero speed.
     * 
     * @param clearFault Flag indicating faults should be cleared when the machine reaches zero speed.
     * 
     * @return Flag indicating faults should be cleared when the machine reaches zero speed.
     */
    const bool& ClearFaultsOnZeroSpeed(const bool *clearFault = NULL);

	/**
	 * Get the machine type from the data tags.
	 * 
	 * @return Specified machine type.
	 */
	string MachineType(void);

    /** Flag indicating faults should be cleared when the machine reaches zero speed. */
    bool        m_clearFaultsOnZeroSpeed;
    /**
     * The minimum speed to apply brake assist to the vehicle.
     * @since Version 1.0
     */
    float       m_minBrakeAssistSpeed;
    /**
     * Amount of brake assist force to apply to the vehicle.
     * @since Version 1.0
     */
    float       m_brakeAssistForce;
    /**
     * Maximum amount of positive torque to apply to the system.
     * @since Version 1.0
     */
    float       m_maxPositiveTorqueLimit;
    /**
     * Maximum amoumt of negative torque to apply to the system.
     * @since Version 1.0
     */
    float       m_maxNegativeTorqueLimit;
    /**
     * Torque assist minimum MPH to engage.
     * @since Version 1.0
     */
    float       m_torqueAssistMinMPH;
    /**
     * The status of the machine boost.
     * @since Version 1.0
     */
    bool        m_boost;
    /**
     * The percent braking to be used in quick brake
     * @since Version 1.5
     */
    float       m_brakingFactor;
    /**
     * Array of motor objects
     * @since Version 1.3
     */
    MotorVector m_motors;
    /**
     * Index in our m_motors array of the master motor object (LF or LR)
     */
    int         m_masterMotor;
    /**
     * Index in our m_motors array of the master motor object (LF or LR)
     */
    int         m_slaveMotor;
    /**
     * The amount of time to wait between starting the motors
     * during speed mode.
     * @since Version 1.5
     */
    UINT32      m_interMotorSpeedDelay;
    /**
     * The amount of time to wait between starting the motors
     * during a LossCompensation test.
     * @since Version 1.5
     */
    UINT32      m_lossCompensationMotorDelay;
    /**
     * The amount of time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32      m_lossCompensationSpeedCheckDelay;
    /**
     * The amount of cycles that have been time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32      m_lossCompensationSpeedCheckCount;
    /**
     * The amount of time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32      m_lossCompensationSpeedCheckCurrentCount;
    /**
     * The amount of time to wait between starting the motors
     * during a LossCompensation test.
     * @since Version 1.5
     */
    UINT32      m_autoDriveAxleSwitchDelay;
    /**
     * The minum speed that the rollers should be at when the
     * test to determine if all of the rollers have started
     * in the Loss Compensation mode.
     * @since Version 1.0
     */
    float       m_lcValidationSpeed;
    /**
     * Speed to stop applying torque when in torque mode
     * (prevents runaway when in torque mode)
     * @since Version 2.5
     */
    float       m_torqueModeSpeedLimit;
    /**
     * Status indicator for health of process
     * @since Version 2.1
     */
    bool        m_itsAllGood;
    /**
     * Enable thread lockup checking
     * @since Version 2.1
     */
    bool        m_enableStatusChecking;
    /**
     * Enable flag for whether or not to report serial
     * communication faults
     * @since Version 2.1
     */
    bool        m_enableMotorCommFault;
    /**
     * Override minimum speed parameter (used to drive rolls to
     * zero speed during braking)
     * @since Version 2.3
     */
    bool        m_overrideMinParameter;
    /**
     * State variable indicating if quick brake is enabled
     * @since 2.4
     */
    bool        m_enableQuickBrake;

    /**
     * Flag to enable / disable side-to-side speed match on the drive axle
     */
    bool        m_axleSpeedMatch;
    /**
     * State variable indicating if automatic drive axle selection is enabled
     * @since 2.4
     */
    bool        m_enableAutoDriveAxle;
    /**
     * State variable indicating if we were previously at zero speed
     * @since 2.4
     */
    bool        m_prevZeroSpeed;
    /**
     * State variable indicating if we lost zero speed
     * @since 2.4
     */
    bool        m_lostZeroSpeed;
};



#endif // _ABMOTORCONTROLLER_H_INCLUDED_

