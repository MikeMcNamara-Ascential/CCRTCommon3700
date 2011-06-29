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
//    $Log: /Ccrt/Source/Core/ServicesLayer/Servers/MotorController/Electric/Baldor/BaldorMotorController.h $
// 
// 5     10/30/07 4:16p Rwiersem
// Changes for the 07103101 core release:
// 
// - Fixed Javadoc comments.
// 
// 4     10/26/06 11:30a Rwiersem
// Changes for the 06102701 core release:
// 
// Removed HandlePulse().  This is now handled by BepServer and
// BepComponent.
// 
// 3     4/13/06 4:22a Cward
// cleanup
//
// 2     3/30/06 1:39a Cward
// Changes to allow reloading of config files without rebooting
//
// 1     3/29/06 2:03p Rwiersem
// Made the test class a friend.  Removed the unimplemented
// ReloadConfiguration method.  Added the methods needed to reload the
// configuration items without restarting the process.
//
// 1     5/11/05 10:06p Cward
//
// 1     4/25/05 2:18p Bmeinke
// Temporary core directory until the real Core has been converted to
// CMake
//
// 6     3/30/05 5:30p Cward
// Updated to latest changes for March 30, 2005 Iteration.
//
// 5     3/03/05 1:02a Cward
// Changes for the 050303 core iteration
//
// 2     2/18/05 12:52p Bmeinke
// Removed the USE_TRQ_LOADING and LOG_FORCES #defines
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
// 2     7/08/04 10:50a Bmeinke
// No longer parameterized (Baldor only)
// Moved implementation into cpp file
// Added upper/lower speed limits for motor loading
//
// 4     6/24/04 1:15a Cward
// Removed pound if 0 block. Updated header block comments to the new
// standard.
//
// 3     1/27/04 4:49p Cward
// Changes resulting from DE's testing and RW's trip
//
// 2     1/23/04 9:25a Cward
// Folding in of KH & DE changes
//
// 1     12/21/03 6:41p Cward
//
// 3     10/24/03 1:44p Khykin
// Updated with latest changes from Korea and Flatrock.
//
// 1     10/07/03 11:29a Khykin
// Initial checkin
//
// 2     8/07/03 7:56a Khykin
// Updated with latest changes from Flatrock.
//
// 22     06/18/03 11:53a Tbochene
// Added Quick Brake enable capability
//
// 21     06/17/03 10:11a Tbochene
// Updated to use AllRollDrivesEnabled instead of clutch, changed
// QuiuckBrake to command all rolls to 0, added parameter to
// override minimum speed parameter, use size of motor container
// instead of roll count to eliminate possile segmentation fault.
//
// 20     06/04/03 1:32p Tbochene
// Updated termination sequence to prevent segmentation fault due
// to race condition between base class and derived
//
// 19     05/29/03 2:49p Tbochene
// Branched
//
// 18     05/20/03 6:18p Tbochene
// Removed SetHeartbeat() from Initialize()
//
// 17     05/20/03 3:21p Tbochene
// Updated configuration so serial message protocol is in a separate
// file
//
// 16     05/20/03 2:11p Tbochene
// Updated to use new motor inheritance structure
//
// 15     05/08/03 1:19p Tbochene
// Added parameters to enable/disable thread lockup detection and
// setting of communication faults.  Changes to remove race
// conditions causing segmentation faults.
//
// 14     05/02/03 11:51a Tbochene
// Updated return values of methods to enable status checking
//
// 13     04/10/03 3:37p Tbochene
// Latest Motor Controller changes added SpecialMode()
//
// 12     04/10/03 7:02a Tbochene
// Housekeeping
//
// 11     03/28/03 9:56a Tbochene
// Latest code after initial testing, updated ivar data.
//
// 10     03/18/03 1:29a Tbochene
// Added Set / GetLossCompensationSpeedCommandDelay(), Set /
// GetMotorSpeedCommandDelay() accessor methods
//
// 9     11/19/02 9:18p Tbochene
// Updated CalculateRpm() to take only the spped value (signed),  Changed
// SpeedMatch() to AxlMatch() and added FollowMaser(), added IsBraking()
// method and added typedef of MotorVector
//
// 8     11/08/02 12:29p Tbochene
// Updated CalculateRpm() to calcluate based on speed and direction passed
// in.  Added SpeedMatch() method that will speed match a slave motor to a
// master motor.  Moved GetAcclerations() to base class.  Moved
// LaunchMotor() and MotorThread() to Motor class.  Removed motor pointer
// variables in favour of a vector of motor pointers for better
// extensability.
//
// 7     10/14/02 4:32p Tbochene
// Moved UpdateHeartbeat() to MotorController
//
// 6     9/24/02 7:45a Khykin
// Adding method to command the PLC to enable the roll drives.
//
// 5     9/16/02 11:15a Khykin
// Finished Debugging.  Tim is going to finish on the machine at JTE.
//
// 4     9/10/02 3:38p Khykin
// Updating for Tim to take to CTC.
//
// 3     9/06/02 10:46a Khykin
// Updating for changes to the motor class.
//
// 2     4/12/02 2:49p Kh
// Final draft complete. Next step debug.
//
// 1     4/03/02 1:23p Kh
// Initial checkin. (Does Not Compile)
//
//******************************************************************************
// $NoKeywords: $
//==============================================================================
#ifndef BaldorMotorController_h
#define BaldorMotorController_h
//==============================================================================

// Don't change the order of these 2 or your eyes will fall out
#include "MotorController.h"
#include "BaldorMotor.h"

#include <math.h>
#include <vector>

/**
 * Class to control electric motors for Roll Brake test machine
 *
 * @author Tim Bochenek
 * @author Kevin Hykin
 * @version 2.5
 * @since Version 1.0
 */
class BaldorMotorController : public MotorController
{
public:
    friend class BaldorMotorControllerTest;

    /**
     * Serial Motor Controller constructor.
     * @since Version 1.0
     */
    BaldorMotorController(void);

    /**
     * Motor Controller destructor.
     * @since Version 1.0
     */
    virtual ~BaldorMotorController(void);

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

protected:
    /**
     * Load the additional configuration items needed by the baldor motor
     * controller.
     *
     * @param document A pointer to the parsed configuration file
     */
    virtual void LoadAdditionalConfigurationItems(const XmlNode *document);
    /**
     * This function calculates the amount of torque load to be applied
     * for motor loading
     *
     * @return Torque to be applied to the drive axle
     */
    virtual float CalculateTorqueLoad(int masterRoll);
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

    /**
     * Matches speed for 2 motors
     *
     * @param masterSpeed
     *                   Speed of master motor
     * @param slaveSpeed Speed of slave motor
     * @param masterMotor
     *                   Mater motor object
     * @param slaveMotor Slave motor object
     * @return Staus of operation
     * @since Version 2.4
     */
    const INT32 AxleMatch(const float masterSpeed, const float slaveSpeed, Motor* masterMotor, Motor* slaveMotor);

    /**
     * Speed follow a master roll (all motors will follow 1 roll)
     *
     * @param master Master roll that all others will follow
     * @param sameAxleSlave
     *               Slave roll on the same axle as master
     * @return Staus of operation
     * @since 1.4
     */
    const INT32 FollowMaster(const Rolls master, const Rolls sameAxleSlave);

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
     * Sets the delay between motor commands during
     * loss compensation to ease current demand
     *
     * @param delay  Delay value in milliseconds
     * @since Version 1.5
     */
    inline void SetLossCompensationSpeedCommandDelay(const UINT32 delay);

    /**
     * Gets the delay between motor commands during loss compensation to ease 
     * current demand.
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
     * The configuration information to use for the serial messages for controlling the drives.
     * @since Version 1.0
     */
    std::string m_serialMessages;
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
     * Log ElectricMotorController information.
     * @since Version 1.0
     */
    void LogInfo(void);

    /**
     * Checks if machine is braking
     *
     * @param master Master roll
     * @return If machine is braking
     * @since Version 2.4
     */
    const bool IsBraking(const Rolls master);

    /**
     * Set the amount that the roll speed should be decreased by in successive 
     * commands.  Factor must be less than 1 but positive (e.g. 0.8).
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
     * Check the speed of a motor in torque mode.  If it is
     * too high, disable (torque 0) the motor.
     *
     * @param roll   Roll index to speed check
     * @param commandedTorque
     *               Torque value to command motor with
     *
     * @return If the commanded torque was modified
     * @since Version 2.5
     */
    inline const bool CheckTorqueModeSpeedLimit(const UINT32 roll, float& commandedTorque);

    /**
     * The minimum speed to apply brake assist to the vehicle.
     * @since Version 1.0
     */
    float m_minBrakeAssistSpeed;
    /**
     * Amount of brake assist force to apply to the vehicle.
     * @since Version 1.0
     */
    float m_BrakeAssistForce;
    /**
     * Maximum amount of positive torque to apply to the system.
     * @since Version 1.0
     */
    float m_maxPositiveTorqueLimit;
    /**
     * Maximum amoumt of negative torque to apply to the system.
     * @since Version 1.0
     */
    float m_maxNegativeTorqueLimit;
    /**
     * Torque assist minimum MPH to engage.
     * @since Version 1.0
     */
    float m_torqueAssistMinMPH;
    /**
     * The status of the machine boost.
     * @since Version 1.0
     */
    bool m_boost;
    /**
     * The percent braking to be used in quick brake
     * @since Version 1.5
     */
    float m_brakingFactor;
    /**
     * Data type definition of a vector of motor pointers
     * @since Version 1.4
     */
    typedef std::vector<Motor*> MotorVector;
    /**
     * Array of motor objects
     * @since Version 1.3
     */
    MotorVector m_motors;
    /**
     * The amount of time to wait between starting the motors
     * during speed mode.
     * @since Version 1.5
     */
    UINT32 m_interMotorSpeedDelay;
    /**
     * The amount of time to wait between starting the motors
     * during a LossCompensation test.
     * @since Version 1.5
     */
    UINT32 m_lossCompensationMotorDelay;
    /**
     * The amount of time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32 m_lossCompensationSpeedCheckDelay;
    /**
     * The amount of cycles that have been time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32 m_lossCompensationSpeedCheckCount;
    /**
     * The amount of time to wait prior to checking that the
     * rollers have started to turn.
     * @since Version 1.1
     */
    UINT32 m_lossCompensationSpeedCheckCurrentCount;
    /**
     * The minum speed that the rollers should be at when the
     * test to determine if all of the rollers have started
     * in the Loss Compensation mode.
     * @since Version 1.0
     */
    float m_lcValidationSpeed;
    /**
     * Speed to stop applying torque when in torque mode
     * (prevents runaway when in torque mode)
     * @since Version 2.5
     */
    float m_torqueModeSpeedLimit;
    /**
     * Status indicator for health of process
     * @since Version 2.1
     */
    bool m_itsAllGood;
    /**
     * Enable thread lockup checking
     * @since Version 2.1
     */
    bool m_enableStatusChecking;
    /**
     * Enable flag for whether or not to report serial
     * communication faults
     * @since Version 2.1
     */
    bool m_enableMotorCommFault;
    /**
     * Override minimum speed parameter (used to drive rolls to
     * zero speed during braking)
     * @since Version 2.3
     */
    bool m_overrideMinParameter;
    /**
     * State variable indicating if quick brake is enabled
     * @since 2.4
     */
    bool m_enableQuickBrake;

    /**
     * Flag to enable / disable side-to-side speed match on the drive axle
     */
    bool m_axleSpeedMatch;

    /**
     * Factor used to calculate a torque command to apply during quick brake.
     * This factor is multiplied by the current roller speed to calculate the
     * torque command used to stop the rollers.
     */
    float m_quickBrakeFactor;

    /**
     * Flag indicating if we detected a drive fault.
     */
    bool m_driveFaultDetected;
};

#endif // BaldorMotorController_h

